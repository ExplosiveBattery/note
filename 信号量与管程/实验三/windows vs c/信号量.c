//因为windows下的Mutex相关函数在实现上有问题，所以使用Semaphore
#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#define sleep(time) Sleep(time*1000)

void Input(void);
void Calculate(void);
void PrintOut(void);

const int n1 = 10;
const int n2 = 20;
//char cBuf1[NBuf1]; //缓冲区1
//char cBuf2[NBuf2]; //缓冲区2
unsigned  nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read; //两个缓冲区的四个待读写位置，默认初始化为0
HANDLE hBuf1Semaphore, hBuf2Semaphore;//分别控制两个缓冲区读写的互斥信号量
HANDLE hBuf1Readable, hBuf1Writable, hBuf2Readable, hBuf2Writable;

int main(void) {
	srand((unsigned)time(0));
	hBuf1Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	hBuf2Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	hBuf1Readable = CreateSemaphore(NULL, 0, n1, NULL);
	hBuf2Readable = CreateSemaphore(NULL, 0, n2, NULL);
	hBuf1Writable = CreateSemaphore(NULL, n1, n1, NULL);
	hBuf2Writable = CreateSemaphore(NULL, n2, n2, NULL);

	//创建三个线程
	HANDLE hArray[3];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Input, NULL, 0, NULL);
	hArray[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Calculate, NULL, 0, NULL);
	hArray[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintOut, NULL, 0, NULL);
	for (int i1 = 0; i1 < 3; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//主线程等待三个子线程的结束才结束
	WaitForMultipleObjects(3, hArray, TRUE, INFINITE);

	//销毁创建的HANDLE
	CloseHandle(hBuf1Semaphore);
	CloseHandle(hBuf2Semaphore);
	return 0;
}

void Input(void) {
	while(1) {
		int nWriteSize =rand() % n1; //假设一次会写入这么多个
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			WaitForSingleObject(hBuf1Writable, INFINITE);//减少1个可写资源或者阻塞buf1写者

			WaitForSingleObject(hBuf1Semaphore, INFINITE);
			nBuf1Write = (nBuf1Write + 1) % n1;
			fprintf(stderr, "input:%u,%u,%u,%u\n",nBuf1Read, nBuf1Write, nBuf2Write,nBuf2Read );
			ReleaseSemaphore(hBuf1Semaphore, 1, NULL);//及时释放buf1读写互斥锁

			
			ReleaseSemaphore(hBuf1Readable, 1, NULL);//增加1个可读资源
			
		}
		getchar();
	}
}

void Calculate(void) {
	while (1) {
		//假设下面的作用是从buf1中读取数据
		int nReadSize = rand() % n1; 
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			WaitForSingleObject(hBuf1Readable, INFINITE);//减少1个可度资源或者阻塞buf1读者

			WaitForSingleObject(hBuf1Semaphore, INFINITE);
			nBuf1Read = (nBuf1Read + 1) % n1;
			fprintf(stderr, "calculate get:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf1Semaphore, 1, NULL);//及时释放buf1读写互斥锁

			
			ReleaseSemaphore(hBuf1Writable, 1, NULL);//增加1个可写资源

		}


		//假设在计算处理来自buf1输入的数据了
		;

		//假设下面的作用是从buf2中读取数据
		int nWriteSize = nReadSize; 
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			WaitForSingleObject(hBuf2Writable, INFINITE);//减少1个可写资源或者阻塞buf2写者

			WaitForSingleObject(hBuf2Semaphore, INFINITE);
			nBuf2Write = (nBuf2Write + 1) % n2;
			fprintf(stderr, "calculate output:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf2Semaphore, 1, NULL);//及时释放读写buf2互斥锁
			
			ReleaseSemaphore(hBuf2Readable, 1, NULL);//增加1个可读资源
		}
	
	}
}

void PrintOut(void) {
	while (1) {
		int nReadSize = rand() % n2; 
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			WaitForSingleObject(hBuf2Readable, INFINITE);//减少1个可读资源或者阻塞buf2读者

			WaitForSingleObject(hBuf2Semaphore, INFINITE);
			nBuf2Read = (nBuf2Read + 1) % n1;
			fprintf(stderr, "print out:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf2Semaphore, 1, NULL);//及时释放buf1读写互斥锁
			
			ReleaseSemaphore(hBuf2Writable, 1, NULL);//增加1个可写资源

		}
	}
}