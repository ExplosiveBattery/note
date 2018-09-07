#include <windows.h>
#include <stdio.h>
#include <time.h>

#define sleep(time) Sleep(time*1000)
#define THREAD_NUM 4


//由于桥两端的资源数（可同时通过人的数目）是1，资源占用后需要立即释放，所以整个题目可以抽象成为对桥中间资源的占用。
#include "stdafx.h"

HANDLE middle = CreateSemaphore(NULL, 2, 2, NULL);

void GoNorth(void)
{
	WaitForSingleObject(middle, -1);
	fprintf(stderr, "向南走的人申请到了桥中间的资源并走到了桥中间\n");
	fprintf(stderr, "向南走的人走到了北边\n");
	ReleaseSemaphore(middle, 1, NULL);
	fprintf(stderr, "向南走的人释放了中间资源\n\n");
}

void GoSouth(void)
{
	WaitForSingleObject(middle, -1); 
	fprintf(stderr, "向北走的人申请到了桥中间的资源并走到了桥中间\n");
	fprintf(stderr, "向北走的人走到了北边\n");
	ReleaseSemaphore(middle, 1, NULL);
	fprintf(stderr, "向北走的人释放了中间资源\n\n");
}

int main(int argc, char ** argv)
{
	//进行初始化准备
	srand(unsigned(time(0)));

	HANDLE hArray[THREAD_NUM];
	for (int i1 = 0; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
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
	//主线程等待三个子线程的结束才结束
	WaitForMultipleObjects(THREAD_NUM, hArray, TRUE, INFINITE);

	//销毁创建的HANDLE
	CloseHandle(middle);
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		CloseHandle(hArray[i1]);

	return 0;
}




