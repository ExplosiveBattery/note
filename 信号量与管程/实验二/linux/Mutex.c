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
	//���б�Ҫ�ĳ�ʼ����׼������
	srand((unsigned)time(0));

	//����THREAD_NUM���߳�
	pthread_t pThreadArray[THREAD_NUM];
	int nReturnValue[THREAD_NUM];
	nReturnValue[0] = pthread_create(pThreadArray + 0, NULL, BridgeControl, NULL);
	for (int i1 = 1; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
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
			if (i1 != 0) pthread_join(pThreadArray[i1], NULL);//���̵߳ȴ����̵߳Ľ����Ž���
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

//�����������ϵ�����
//���������м������խ�����Դ�Ч��������Ҫ�����˴����ߵ����м䣬�����˴��м䵽���ߣ����ǲ�������ÿ��ֻ����һ���Ļ�Ҳ�Ͳ�����deadlock��
void *BridgeControl(void* arg) {
	while (1) {
		pthread_mutex_lock(&sVarMutex);
		//�������ߵ��м�
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
		//�����м䵽������
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
		//���ߵ���������� ��һ�����жϾ���ʵ���Ժ�����ĺ�����
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