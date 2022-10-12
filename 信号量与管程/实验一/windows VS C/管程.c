#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#define sleep(time) Sleep(time*1000)

CRITICAL_SECTION cs;
CONDITION_VARIABLE cvPlate;
CONDITION_VARIABLE cvApple;
CONDITION_VARIABLE cvOrrange;

int iFruitInPalte = 0;
int iAppleInPalte = 0;
int iOrrangeInPalte = 0;

void Father();
void Son();
void Daughter();

int main(void) {
	//进行必要的初始化等准备工作
	srand((unsigned)time(0));
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cvPlate);
	InitializeConditionVariable(&cvApple);
	InitializeConditionVariable(&cvOrrange);

	//创建三个线程
	HANDLE hArray[3];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Father, NULL, 0, NULL);
	hArray[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Son, NULL, 0, NULL);
	hArray[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Daughter, NULL, 0, NULL);
	for (int i1 = 0; i1 < 3; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//主线程等待三个子线程的结束才结束
	WaitForMultipleObjects(3, hArray, TRUE, INFINITE); //INFINITE -1

	//销毁创建的HANDLE
	DeleteCriticalSection(&cs);
	for (int i1 = 0; i1 < 3; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}


void Father() {
	while(1) {
		sleep(2);
		EnterCriticalSection(&cs);
		while ( iFruitInPalte == 1 ) { //必须使用while循环，因为在网上查到他一次唤醒却可能唤醒多个
			SleepConditionVariableCS(&cvPlate, &cs, INFINITE);
		}
		if (rand() % 2 == 0) {
			puts("The Father puts a apple in plate.");
			++iAppleInPalte;
			WakeConditionVariable(&cvApple);
		}
		else {
			puts("The Father puts a orrange in plate.");
			++iOrrangeInPalte;
			WakeConditionVariable(&cvOrrange);
		}
		++iFruitInPalte;
		fflush(stdout);
		LeaveCriticalSection(&cs);
	}
}

void Son() {
	//如果盘子里面有Apple就会进入循环
	while (1) {
		EnterCriticalSection(&cs);
		while (iAppleInPalte==0) { 
			SleepConditionVariableCS(&cvApple, &cs, INFINITE); //相当于P操作
		}
		--iAppleInPalte;
		puts("The son eat a apple.");
		WakeConditionVariable(&cvPlate);//相当于V操作
		--iFruitInPalte;
		LeaveCriticalSection(&cs);
	}
}

void Daughter() {
	//如果盘子里面有Orrange就会进入循环
	while (1) {
		EnterCriticalSection(&cs);
		while (iOrrangeInPalte==0) {
			SleepConditionVariableCS(&cvOrrange, &cs, INFINITE);
		}
		--iOrrangeInPalte;
		puts("The daughter eat a orrange.");
		WakeConditionVariable(&cvPlate);
		--iFruitInPalte;
		LeaveCriticalSection(&cs);
	}
}
