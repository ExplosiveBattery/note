#include <windows.h>
#include <stdio.h>
#include <time.h>

#define sleep(time) Sleep(time*1000)
#define THREAD_NUM 4


//���������˵���Դ������ͬʱͨ���˵���Ŀ����1����Դռ�ú���Ҫ�����ͷţ�����������Ŀ���Գ����Ϊ�����м���Դ��ռ�á�
#include "stdafx.h"

HANDLE middle = CreateSemaphore(NULL, 2, 2, NULL);

void GoNorth(void)
{
	WaitForSingleObject(middle, -1);
	fprintf(stderr, "�����ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "�����ߵ����ߵ��˱���\n");
	ReleaseSemaphore(middle, 1, NULL);
	fprintf(stderr, "�����ߵ����ͷ����м���Դ\n\n");
}

void GoSouth(void)
{
	WaitForSingleObject(middle, -1); 
	fprintf(stderr, "���ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "���ߵ����ߵ��˱���\n");
	ReleaseSemaphore(middle, 1, NULL);
	fprintf(stderr, "���ߵ����ͷ����м���Դ\n\n");
}

int main(int argc, char ** argv)
{
	//���г�ʼ��׼��
	srand(unsigned(time(0)));

	HANDLE hArray[THREAD_NUM];
	for (int i1 = 0; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
		//sleep(rand() % 3);
		if (rand() % 2 == 0)
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GoNorth, NULL, 0, NULL);
		else
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GoSouth, NULL, 0, NULL);
	}
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//���̵߳ȴ��������̵߳Ľ����Ž���
	WaitForMultipleObjects(THREAD_NUM, hArray, TRUE, INFINITE);

	//���ٴ�����HANDLE
	CloseHandle(middle);
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		CloseHandle(hArray[i1]);

	return 0;
}




