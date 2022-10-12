#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_NUM 4

pthread_cond_t middle= PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int nNum =2;//���м����Դ��

void *GoNorth(void * arg)
{
	pthread_mutex_lock(&mutex);
	while (nNum == 0) 
		 pthread_cond_wait(&middle, &mutex);
	--nNum;
	fprintf(stderr, "�����ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "�����ߵ����ߵ��˱���\n");
	++nNum;
	pthread_mutex_unlock(&mutex);
	fprintf(stderr, "�����ߵ����ͷ����м���Դ\n\n");
	pthread_cond_signal(&middle);
}

void *GoSouth(void * arg)
{
	pthread_mutex_lock(&mutex); 
	while (nNum == 0) 
		 pthread_cond_wait(&middle, &mutex);
	--nNum;
	fprintf(stderr, "���ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "���ߵ����ߵ��˱���\n");
	++nNum;
	pthread_mutex_unlock(&mutex);
	fprintf(stderr, "���ߵ����ͷ����м���Դ\n\n");
	pthread_cond_signal(&middle);
	
}

int main(void) {
	pthread_t pThreadArray[THREAD_NUM];
	int nReturnValue[THREAD_NUM];
	for (int i1 = 0; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
											 //sleep(rand() % 3);
		if (rand() % 2 == 0)
			nReturnValue[i1] = pthread_create(pThreadArray + i1, NULL, GoNorth, NULL);
		else
			nReturnValue[i1] = pthread_create(pThreadArray + i1, NULL, GoSouth, NULL);
	}
	for (int i1 = 0; i1 < THREAD_NUM; ++i1) 
		if (nReturnValue[i1]) {
			fprintf(stderr, "thread %d can't create!", i1);
			exit(2);
		}
		else {
			pthread_join(pThreadArray[i1], NULL);//���̵߳ȴ����̵߳Ľ����Ž���
		}



	return 0;
}