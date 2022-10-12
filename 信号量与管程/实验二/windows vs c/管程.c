#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#define sleep(time) Sleep(time*1000)
void BridgeControl(void);
void FromNorth(void); //go to south
void FromSouth(void); //go to north
void PrintBridge(void);

#define THREAD_NUM 11
char cBridge[4];//[0]NrothBuffer [1][2]MiddleBuffer  [3]SouthBuffer
CRITICAL_SECTION cs;
CONDITION_VARIABLE cvNorth; bool bNorth = 1;
CONDITION_VARIABLE cvSouth; bool bSouth = 1;

int main(void) {
	//���б�Ҫ�ĳ�ʼ����׼������
	srand((unsigned)time(0));
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cvNorth);
	InitializeConditionVariable(&cvSouth);

	//���������߳�
	HANDLE hArray[21];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BridgeControl, NULL, 0, NULL);
	for(int i1=1; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
		if ( rand()%2==0 )
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FromNorth, NULL, 0, NULL);
		else
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FromSouth, NULL, 0, NULL);
	}
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//���̵߳ȴ��������̵߳Ľ����Ž���
	sleep(5);

	//���ٴ�����HANDLE
	DeleteCriticalSection(&cs);
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}

void FromNorth(void) {
	EnterCriticalSection(&cs);
	while (bNorth == 0) {
		SleepConditionVariableCS(&cvNorth, &cs, INFINITE);
	}
	cBridge[0] = 'N';
	bNorth = 0;
	LeaveCriticalSection(&cs);
}

void FromSouth(void) {
	EnterCriticalSection(&cs);
	while (bSouth == 0) {
		SleepConditionVariableCS(&cvSouth, &cs, INFINITE);
	}
	cBridge[3] = 'S';
	bSouth = 0;
	LeaveCriticalSection(&cs);
}

//�����������ϵ�����
//���������м������խ�����Դ�Ч��������Ҫ�����˴����ߵ����м䣬�����˴��м䵽���ߣ������ַ�ʽ����������
void BridgeControl(void) {
	while(1){
		EnterCriticalSection(&cs);
		//�������ߵ��м�
		if (cBridge[0] == 'N') {
			if (cBridge[1] == 0) {
				cBridge[1] = 'N';
				cBridge[0] = 0;
			}
			else if (cBridge[2] == 0) {
				cBridge[2] = 'N';
				cBridge[0] = 0;
			}
			else
				;
		}
		if (cBridge[3] == 'S') {
			if(cBridge[1]==0){
				cBridge[1] = 'S';
				cBridge[3] = 0;
			}else if (cBridge[2] == 0) {
				cBridge[2] = 'S';
				cBridge[3] = 0;
			}
			else
				;
		}
		//�����м䵽������
		if (cBridge[0] == 0) {
			if (cBridge[1] == 'S') {
				cBridge[0] = 'S';
				cBridge[1] = 0;
			}else if (cBridge[2] == 'S'){
				cBridge[0] = 'S';
				cBridge[2] = 0;
			}else
				;
		}
		if (cBridge[3] == 0) {
			if (cBridge[1] == 'N') {
				cBridge[3] = 'N';
				cBridge[1] = 0;
			}
			else if (cBridge[2] == 'N') {
				cBridge[3] = 'N';
				cBridge[2] = 0;
			}
			else
				;
		}
		//���ߵ���������� ��һ�����жϾ���ʵ���Ժ�����ĺ�����
		if (cBridge[0] == 'S')
			cBridge[0] = 0;
		if (cBridge[3] == 'N')
			cBridge[3] = 0;
		
		bNorth=1;bSouth=1;
		WakeConditionVariable(&cvNorth);
		WakeConditionVariable(&cvSouth);
		LeaveCriticalSection(&cs);
	}
}

void PrintBridge(void) {
	printf(" %c\n", cBridge[1]);
	printf("%c %c\n", cBridge[0], cBridge[3]);
	printf(" %c\n", cBridge[2]);
	fflush(stdout);
}