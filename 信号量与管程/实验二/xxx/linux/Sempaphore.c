#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_NUM 4

sem_t middle;

void *GoNorth(void * arg)
{
	sem_wait(&middle); 
	fprintf(stderr, "�����ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "�����ߵ����ߵ��˱���\n");
	sem_post(&middle);
	fprintf(stderr, "�����ߵ����ͷ����м���Դ\n\n");
}

void *GoSouth(void * arg)
{
	sem_wait(&middle); 
	fprintf(stderr, "���ߵ������뵽�����м����Դ���ߵ������м�\n");
	fprintf(stderr, "���ߵ����ߵ��˱���\n");
	sem_post(&middle);
	fprintf(stderr, "���ߵ����ͷ����м���Դ\n\n");
}

int main(void) {
	if (sem_init(&middle, 0, 2) ){
		fprintf(stderr, "sem init failed\n");
		exit(1);
	}

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



	sem_destroy(&middle);
	return 0;
}