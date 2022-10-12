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
	fprintf(stderr, "向南走的人申请到了桥中间的资源并走到了桥中间\n");
	fprintf(stderr, "向南走的人走到了北边\n");
	sem_post(&middle);
	fprintf(stderr, "向南走的人释放了中间资源\n\n");
}

void *GoSouth(void * arg)
{
	sem_wait(&middle); 
	fprintf(stderr, "向北走的人申请到了桥中间的资源并走到了桥中间\n");
	fprintf(stderr, "向北走的人走到了北边\n");
	sem_post(&middle);
	fprintf(stderr, "向北走的人释放了中间资源\n\n");
}

int main(void) {
	if (sem_init(&middle, 0, 2) ){
		fprintf(stderr, "sem init failed\n");
		exit(1);
	}

	pthread_t pThreadArray[THREAD_NUM];
	int nReturnValue[THREAD_NUM];
	for (int i1 = 0; i1 < THREAD_NUM; ++i1) {//假设会有THREAD_NUM-1个人自两边来要打算过桥，方向伪随机
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
			pthread_join(pThreadArray[i1], NULL);//主线程等待子线程的结束才结束
		}



	sem_destroy(&middle);
	return 0;
}