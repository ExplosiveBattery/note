//对实验二需要注意的是死锁问题，比如说两个任同时在桥上都准备向南走，结果来了个向北的
//解决办法：设定资源请求顺序，比如说先下后上、甚至是先北后南
//但是从桥各种情况下对交通的率用率来讲，先下桥后上桥这种比先北后南好一些，但是也没有达到最大程度上对交通的解决问题
//要想资源利用率达到最大那就需要对特殊情况进行判断：桥上两人如果朝同一个方向，那么这个方向不能有人来堵路（当然，如果人的移动速度都是一样，那么是不会出现这种死锁情况）；桥上有人时，不能两边都有人
//总的来说，使用两个信号量来控制两边的上桥，使用两个互斥锁来保证BridgeControl对南北缓冲区的读写问题
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
HANDLE hMutexArray[2]; //[0]hNorthMutex [1]hSouthMutex  //BridgeControl操控North、South缓冲区的时候需要与其余两个Control之间分别互斥
char cBridge[4];//[0]NrothBuffer [1][2]MiddleBuffer  [3]SouthBuffer
//HANDLE hNorthSemaphore, hSouthSemaphore;

int main(void) {
	//进行必要的初始化等准备工作
	srand((unsigned)time(0));
	hMutexArray[0] =CreateMutex(NULL, TRUE, NULL);
	hMutexArray[1] =CreateMutex(NULL, TRUE, NULL);
	hVarMutex =CreateMutex(NULL, TRUE, NULL);
//	hNorthSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
//	hSouthSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	//创建THREAD_NUM个线程
	HANDLE hArray[21];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BridgeControl, NULL, 0, NULL);
	for(int i1=1; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
		sleep(rand() % 3);
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
	WaitForMultipleObjects(THREAD_NUM, hArray, TRUE, INFINITE);

	//销毁创建的HANDLE
	CloseHandle(hMutexArray[0]);
	CloseHandle(hMutexArray[1]);
//	CloseHandle(hNorthSemaphore);
//	CloseHandle(hSouthSemaphore);
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}

void FromNorth(void) {
//	WaitForSingleObject(hNorthSemaphore, INFINITE);
	WaitForSingleObject(hVarMutex, INFINITE);
	WaitForSingleObject(hMutexArray[0], INFINITE);
	cBridge[0] = 'N';
	ReleaseMutex(hVarMutex);
}

void FromSouth(void) {
//	WaitForSingleObject(hSouthSemaphore, INFINITE);
	WaitForSingleObject(hVarMutex, INFINITE);
	WaitForSingleObject(hMutexArray[1], INFINITE);
	cBridge[3] = 'S';
	ReleaseMutex(hVarMutex);
}

//控制人在桥上的行走
//由于桥是中间宽两边窄，所以从效率来讲，要先让人从两边到桥中间，再让人从中间到两边，但是不控制人每次只能走一步的话也就不会有deadlock了
void BridgeControl(void) {
	while(1) {
		//从桥两边到中间
		WaitForSingleObject(hVarMutex, INFINITE);
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
		ReleaseMutex(hMutexArray[0]);
		ReleaseMutex(hMutexArray[1]);
		ReleaseMutex(hVarMutex);
	}
}

void PrintBridge(void) {
	printf(" %c\n", cBridge[1]);
	printf("%c %c\n", cBridge[0], cBridge[3]);
	printf(" %c\n", cBridge[2]);
	fflush(stdout);
}
