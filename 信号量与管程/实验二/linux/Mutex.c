#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <unistd.h>



void *BridgeControl(void*);
void *FromNorth(void*); //go to south
void *FromSouth(void*); //go to north
void PrintBridge(void);

#define THREAD_NUM 2
pthread_mutex_t sMutexArray[2] = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };
pthread_mutex_t sVarMutex = PTHREAD_MUTEX_INITIALIZER;
char cBridge[4];//[0]NrothBuffer [1][2]MiddleBuffer  [3]SouthBuffer

int main(void) {
	//进行必要的初始化等准备工作
	srand((unsigned)time(0));

	//创建THREAD_NUM个线程
	pthread_t pThreadArray[THREAD_NUM];
	int nReturnValue[THREAD_NUM];
	nReturnValue[0] = pthread_create(pThreadArray + 0, NULL, BridgeControl, NULL);
	for (int i1 = 1; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
											 //sleep(rand() % 3);
		if (rand() % 2 == 0)
			nReturnValue[i1] = pthread_create(pThreadArray + i1, NULL, FromNorth, NULL);
		else
			nReturnValue[i1] = pthread_create(pThreadArray + i1, NULL, FromSouth, NULL);
	}
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		if (nReturnValue[i1]) {
			fprintf(stderr, "thread %d can't create!", i1);
			exit(2);
		}
		else {
			if (i1 != 0) pthread_join(pThreadArray[i1], NULL);//主线程等待子线程的结束才结束
		}
		sleep(5);
		return 0;
}

void* FromNorth(void* arg) {
	pthread_mutex_lock(&sVarMutex);
	pthread_mutex_lock(&sMutexArray[0]);
	cBridge[0] = 'N';
	pthread_mutex_unlock(&sVarMutex);
	return ((void *)0);
}

void* FromSouth(void* arg) {
	pthread_mutex_lock(&sVarMutex);
	pthread_mutex_lock(&sMutexArray[1]);
	cBridge[3] = 'S';
	pthread_mutex_unlock(&sVarMutex);
	return ((void *)0);
}

//控制人在桥上的行走
//由于桥是中间宽两边窄，所以从效率来讲，要先让人从两边到桥中间，再让人从中间到两边，但是不控制人每次只能走一步的话也就不会有deadlock了
void *BridgeControl(void* arg) {
	while (1) {
		pthread_mutex_lock(&sVarMutex);
		//从桥两边到中间
		if (cBridge[0] == 'N') {
			PrintBridge();
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
			PrintBridge();
			if (cBridge[1] == 0) {
				cBridge[1] = 'S';
				cBridge[3] = 0;
			}
			else if (cBridge[2] == 0) {
				cBridge[2] = 'S';
				cBridge[3] = 0;
			}
			else
				;
		}
		//从桥中间到桥两边
		if (cBridge[0] == 0) {
			if (cBridge[1] == 'S') {
				PrintBridge();
				cBridge[0] = 'S';
				cBridge[1] = 0;
			}
			else if (cBridge[2] == 'S') {
				PrintBridge();
				cBridge[0] = 'S';
				cBridge[2] = 0;
			}
			else
				;
		}
		if (cBridge[3] == 0) {
			if (cBridge[1] == 'N') {
				PrintBridge();
				cBridge[3] = 'N';
				cBridge[1] = 0;
			}
			else if (cBridge[2] == 'N') {
				PrintBridge();
				cBridge[3] = 'N';
				cBridge[2] = 0;
			}
			else
				;
		}
		//两边到达的人下桥 这一步的判断句其实可以和上面的合起来
		if (cBridge[0] == 'S') {
			PrintBridge();
			cBridge[0] = 0;
			PrintBridge();
			pthread_mutex_unlock(&sMutexArray[0]);
		}
			
		if (cBridge[3] == 'N') {
			PrintBridge();
			cBridge[3] = 0;
			PrintBridge();
			pthread_mutex_unlock(&sMutexArray[1]);
		}
		pthread_mutex_unlock(&sVarMutex);
	}
	return((void *)0);
}

void PrintBridge(void) {
	for(int i1=0;i1<4;++i1){
		if (cBridge[i1] == 0)
			printf("0 ");
		else
			printf("%c ", cBridge[i1]);
	}
	printf("\n");
	fflush(stdout);
}