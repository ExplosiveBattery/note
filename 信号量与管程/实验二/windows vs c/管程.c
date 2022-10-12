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
	//进行必要的初始化等准备工作
	srand((unsigned)time(0));
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cvNorth);
	InitializeConditionVariable(&cvSouth);

	//创建三个线程
	HANDLE hArray[21];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BridgeControl, NULL, 0, NULL);
	for(int i1=1; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
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
	//主线程等待三个子线程的结束才结束
	sleep(5);

	//销毁创建的HANDLE
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

//控制人在桥上的行走
//由于桥是中间宽两边窄，所以从效率来讲，要先让人从两边到桥中间，再让人从中间到两边，用这种方式来避免死锁
void BridgeControl(void) {
	while(1){
		EnterCriticalSection(&cs);
		//从桥两边到中间
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
		//从桥中间到桥两边
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
		//两边到达的人下桥 这一步的判断句其实可以和上面的合起来
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