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
	//���б�Ҫ�ĳ�ʼ����׼������
	srand((unsigned)time(0));

	//����THREAD_NUM���߳�
	pthread_t pThreadArray[21];
	int nReturnValue[21];
	nReturnValue[0] = pthread_create(pThreadArray + 0, NULL, BridgeControl, NULL);
	for (int i1 = 1; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
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
			if (i1 != 0) pthread_join(pThreadArray[i1], NULL);//���̵߳ȴ����̵߳Ľ����Ž���
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

//�����������ϵ�����
//���������м������խ�����Դ�Ч��������Ҫ�����˴����ߵ����м䣬�����˴��м䵽���ߣ������ַ�ʽ����������
void BridgeControl(void) {
	while(1){
		pthread_mutex_lock(&mutex);
		//�������ߵ��м�
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
		//�����м䵽������
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
		//���ߵ���������� ��һ�����жϾ���ʵ���Ժ�����ĺ�����
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