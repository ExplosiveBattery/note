//这是石头的实验三windows下的管程，以C语言编写。
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
CRITICAL_SECTION csBuf1, csBuf2;//配合条件变量使用的临界区
CONDITION_VARIABLE cvBuf1Read, cvBuf1Write, cvBuf2Read, cvBuf2Write;//分别控制两个缓冲区读写的条件量
unsigned  nBuf1, nBuf2; //两个缓冲区的已写代读数据量，默认自动初始化为0，用来判断缓冲区的空、满状态
unsigned  nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read;


int main(void) {
	srand((unsigned)time(0));
	InitializeCriticalSection(&csBuf1);
	InitializeCriticalSection(&csBuf2);
	InitializeConditionVariable(&cvBuf1Read);
	InitializeConditionVariable(&cvBuf1Write);
	InitializeConditionVariable(&cvBuf2Read);
	InitializeConditionVariable(&cvBuf2Write);

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
	DeleteCriticalSection(&csBuf1);
	DeleteCriticalSection(&csBuf2);
	for (int i1 = 0; i1 < 3; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}

void Input(void) {
	while(1) {
		int nWriteSize =rand() % n1; //假设一次会写入这么多个
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			EnterCriticalSection(&csBuf1);

			while (nBuf1 == n1)//缓冲区已满，当前不再写入
				SleepConditionVariableCS(&cvBuf1Write, &csBuf1, INFINITE);

			nBuf1Write = (nBuf1Write + 1) % n1;++nBuf1;
			fprintf(stderr, "input:%u,%u,%u,%u\n", nBuf1Write, nBuf1Read,  nBuf2Write,nBuf2Read );

			LeaveCriticalSection(&csBuf1);
			WakeConditionVariable(&cvBuf1Read);
		}
		getchar();
	}
}

void Calculate(void) {
	while (1) {
		//假设下面的作用是从buf1中读取数据
		int nReadSize = rand() % n1; 
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			EnterCriticalSection(&csBuf1);

			while (nBuf1 == 0)//缓冲区为空，当前不能读取
				SleepConditionVariableCS(&cvBuf1Read, &csBuf1, INFINITE);

			nBuf1Read = (nBuf1Read + 1) % n1; --nBuf1;
			fprintf(stderr, "calculate get:%u,%u,%u,%u\n", nBuf1Write, nBuf1Read, nBuf2Write, nBuf2Read);

			LeaveCriticalSection(&csBuf1);
			WakeConditionVariable(&cvBuf1Write);
		}


		//假设在计算处理来自buf1输入的数据了
		;

		//假设下面的作用是从buf2中读取数据
		int nWriteSize = nReadSize; 
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			EnterCriticalSection(&csBuf2);

			while (nBuf2 == n2)//缓冲区已满，当前不再写入
				SleepConditionVariableCS(&cvBuf2Write, &csBuf2, INFINITE);

			nBuf2Write = (nBuf2Write + 1) % n2; ++nBuf2;
			fprintf(stderr, "calculate output:%u,%u,%u,%u\n", nBuf1Write, nBuf1Read, nBuf2Write, nBuf2Read);

			LeaveCriticalSection(&csBuf2);
			WakeConditionVariable(&cvBuf2Read);
		}
	
	}
}

void PrintOut(void) {
	while (1) {
		int nReadSize = rand() % n2;
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			EnterCriticalSection(&csBuf2);

			while (nBuf2 == 0)//缓冲区为空，当前不能读取
				SleepConditionVariableCS(&cvBuf2Read, &csBuf2, INFINITE);

			nBuf2Read = (nBuf2Read + 1) % n2; --nBuf2;
			fprintf(stderr, "print out:%u,%u,%u,%u\n", nBuf1Write, nBuf1Read, nBuf2Write, nBuf2Read);

			LeaveCriticalSection(&csBuf2);
			WakeConditionVariable(&cvBuf2Write);
		}
	}
}
