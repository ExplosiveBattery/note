//��Ϊwindows�µ�Mutex��غ�����ʵ���������⣬����ʹ��Semaphore
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
//char cBuf1[NBuf1]; //������1
//char cBuf2[NBuf2]; //������2
unsigned  nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read; //�������������ĸ�����дλ�ã�Ĭ�ϳ�ʼ��Ϊ0
HANDLE hBuf1Semaphore, hBuf2Semaphore;//�ֱ����������������д�Ļ����ź���
HANDLE hBuf1Readable, hBuf1Writable, hBuf2Readable, hBuf2Writable;

int main(void) {
	srand((unsigned)time(0));
	hBuf1Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	hBuf2Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	hBuf1Readable = CreateSemaphore(NULL, 0, n1, NULL);
	hBuf2Readable = CreateSemaphore(NULL, 0, n2, NULL);
	hBuf1Writable = CreateSemaphore(NULL, n1, n1, NULL);
	hBuf2Writable = CreateSemaphore(NULL, n2, n2, NULL);

	//���������߳�
	HANDLE hArray[3];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Input, NULL, 0, NULL);
	hArray[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Calculate, NULL, 0, NULL);
	hArray[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintOut, NULL, 0, NULL);
	for (int i1 = 0; i1 < 3; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//���̵߳ȴ��������̵߳Ľ����Ž���
	WaitForMultipleObjects(3, hArray, TRUE, INFINITE);

	//���ٴ�����HANDLE
	CloseHandle(hBuf1Semaphore);
	CloseHandle(hBuf2Semaphore);
	return 0;
}

void Input(void) {
	while(1) {
		int nWriteSize =rand() % n1; //����һ�λ�д����ô���
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			WaitForSingleObject(hBuf1Writable, INFINITE);//����1����д��Դ��������buf1д��

			WaitForSingleObject(hBuf1Semaphore, INFINITE);
			nBuf1Write = (nBuf1Write + 1) % n1;
			fprintf(stderr, "input:%u,%u,%u,%u\n",nBuf1Read, nBuf1Write, nBuf2Write,nBuf2Read );
			ReleaseSemaphore(hBuf1Semaphore, 1, NULL);//��ʱ�ͷ�buf1��д������

			
			ReleaseSemaphore(hBuf1Readable, 1, NULL);//����1���ɶ���Դ
			
		}
		getchar();
	}
}

void Calculate(void) {
	while (1) {
		//��������������Ǵ�buf1�ж�ȡ����
		int nReadSize = rand() % n1; 
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			WaitForSingleObject(hBuf1Readable, INFINITE);//����1���ɶ���Դ��������buf1����

			WaitForSingleObject(hBuf1Semaphore, INFINITE);
			nBuf1Read = (nBuf1Read + 1) % n1;
			fprintf(stderr, "calculate get:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf1Semaphore, 1, NULL);//��ʱ�ͷ�buf1��д������

			
			ReleaseSemaphore(hBuf1Writable, 1, NULL);//����1����д��Դ

		}


		//�����ڼ��㴦������buf1�����������
		;

		//��������������Ǵ�buf2�ж�ȡ����
		int nWriteSize = nReadSize; 
		for (int i1 = 0; i1 < nWriteSize; ++i1) {
			WaitForSingleObject(hBuf2Writable, INFINITE);//����1����д��Դ��������buf2д��

			WaitForSingleObject(hBuf2Semaphore, INFINITE);
			nBuf2Write = (nBuf2Write + 1) % n2;
			fprintf(stderr, "calculate output:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf2Semaphore, 1, NULL);//��ʱ�ͷŶ�дbuf2������
			
			ReleaseSemaphore(hBuf2Readable, 1, NULL);//����1���ɶ���Դ
		}
	
	}
}

void PrintOut(void) {
	while (1) {
		int nReadSize = rand() % n2; 
		for (int i1 = 0; i1 < nReadSize; ++i1) {
			WaitForSingleObject(hBuf2Readable, INFINITE);//����1���ɶ���Դ��������buf2����

			WaitForSingleObject(hBuf2Semaphore, INFINITE);
			nBuf2Read = (nBuf2Read + 1) % n1;
			fprintf(stderr, "print out:%u,%u,%u,%u\n", nBuf1Read, nBuf1Write, nBuf2Write, nBuf2Read);
			ReleaseSemaphore(hBuf2Semaphore, 1, NULL);//��ʱ�ͷ�buf1��д������
			
			ReleaseSemaphore(hBuf2Writable, 1, NULL);//����1����д��Դ

		}
	}
}