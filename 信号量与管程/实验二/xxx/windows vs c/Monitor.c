//���θ۴����㷨��
#include <windows.h>
#include <stdio.h>
#include <time.h>

#define sleep(time) Sleep(time*1000)
#define THREAD_NUM 4

HANDLE middle = CreateSemaphore(NULL, 2, 2, NULL);

CRITICAL_SECTION cs;
CONDITION_VARIABLE cvMiddle; int nNum=2;

void GoNorth(void)
{
	EnterCriticalSection(&cs);
	while (nNum == 0) 
		SleepConditionVariableCS(&cvMiddle, &cs, INFINITE);
	--nNum;
	fprintf(stderr, "�����ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "�����ߵ����ߵ��˱���\n");
	++nNum;
	LeaveCriticalSection(&cs);
	fprintf(stderr, "�����ߵ����ͷ����м���Դ\n\n");
	WakeConditionVariable(&cvMiddle);
	
}

void GoSouth(void)
{
	EnterCriticalSection(&cs);
	while (nNum == 0) 
		SleepConditionVariableCS(&cvMiddle, &cs, INFINITE);
	--nNum;
	fprintf(stderr, "���ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "���ߵ����ߵ��˱���\n");
	++nNum;
	LeaveCriticalSection(&cs);
	fprintf(stderr, "���ߵ����ͷ����м���Դ\n\n");
	WakeConditionVariable(&cvMiddle);
}

int main(int argc, char ** argv)
{
	//���г�ʼ��׼��
	srand(unsigned(time(0)));
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cvMiddle);

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

