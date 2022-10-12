#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <unistd.h>

void BridgeControl(void);
void FromNorth(void); //go to south
void FromSouth(void); //go to north
void PrintBridge(void);

#define THREAD_NUM 11
char cBridge[4];//[0]NrothBuffer [1][2]MiddleBuffer  [3]SouthBuffer
pthread_mutex_t mutex;
pthread_cond_t tNorth= PTHREAD_COND_INITIALIZER; bool bNorth = 1;
pthread_cond_t tSouth= PTHREAD_COND_INITIALIZER; bool bSouth = 1;

int main(void) {
	//进行必要的初始化等准备工作
	srand((unsigned)time(0));

	//创建THREAD_NUM个线程
	pthread_t pThreadArray[21];
	int nReturnValue[21];
	nReturnValue[0] = pthread_create(pThreadArray + 0, NULL, BridgeControl, NULL);
	for (int i1 = 1; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
		sleep(rand() % 3);
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

	return 0;
}

void FromNorth(void) {
	pthread_mutex_lock(&mutex);
	while (bNorth == 0) {
		pthread_cond_wait(&tNorth, &mutex);
	}
	cBridge[0] = 'N';
	bNorth = 0;
	pthread_mutex_unlock(&mutex);
}

void FromSouth(void) {
	pthread_mutex_lock(&mutex);
	while (bSouth == 0) {
		pthread_cond_wait(&tSouth, &mutex);
	}
	cBridge[3] = 'S';
	bSouth = 0;
	pthread_mutex_unlock(&mutex);
}

//控制人在桥上的行走
//由于桥是中间宽两边窄，所以从效率来讲，要先让人从两边到桥中间，再让人从中间到两边，用这种方式来避免死锁
void BridgeControl(void) {
	while(1){
		pthread_mutex_lock(&mutex);
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
		pthread_mutex_unlock(&mutex);		
		pthread_cond_signal(&tNorth);
		pthread_cond_signal(&tSouth);
	}
}

void PrintBridge(void) {
	printf(" %c\n", cBridge[1]);
	printf("%c %c\n", cBridge[0], cBridge[3]);
	printf(" %c\n", cBridge[2]);
	fflush(stdout);
}