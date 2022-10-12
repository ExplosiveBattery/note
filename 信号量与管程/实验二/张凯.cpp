#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<Windows.h>
#include<conio.h>
#include<fstream>
#include<iostream>
using namespace std;
#pragma comment(lib,"x86/pthreadVC2.lib")  


#define sleep(time) Sleep(time*1000)


struct ThreadInfo
{
	int id;
	char role;//North or Sourth
	double delay;
};

pthread_mutex_t northlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t southlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;

int Count = 0;
void ToNorth(int n)
{
	pthread_mutex_lock(&northlock);
	while (Count == 3)
	{
		pthread_cond_wait(&notfull, &northlock);
	}
	printf("North walker %d starts to go north\n", n);
	Count++;
	sleep(1);
	pthread_mutex_unlock(&northlock);

	sleep(1);
	pthread_mutex_lock(&southlock);
	sleep(1);
	printf("North walker %d finishes\n", n);
	Count--;
	pthread_cond_signal(&notfull);
	pthread_mutex_unlock(&southlock);
}
void ToSouth(int s)
{
	pthread_mutex_lock(&southlock);
	while (Count == 3)
	{
		pthread_cond_wait(&notfull, &southlock);
	}
	printf("South walker %d starts to go south\n", s);
	Count++;
	sleep(1);
	pthread_mutex_unlock(&southlock);

	sleep(1);
	pthread_mutex_lock(&northlock);
	sleep(1);
	printf("South walker %d finishes\n", s);
	Count--;
	pthread_cond_signal(&notfull);
	pthread_mutex_unlock(&northlock);
}
void *North_thread(void *arg) {
	int id = (int)arg; // �����ݽ����Ĳ�����Ϊ��ǰ���̵߳ı�ʾ
	fprintf(stderr, "Northwalker %d sents the north request\n", id);
	ToNorth(id);
	return ((void *)0);
}

void *South_thread(void *arg) {
	int id = (int)arg; // �����ݽ����Ĳ�����Ϊ��ǰ���̵߳ı�ʾ
	fprintf(stderr, "Southwalker %d sents the south request\n", id);
	ToSouth(id);
	return ((void *)0);
}

int main(void) {
	// �����������ߵ�����
	for (int i = 1; i <= 3; i++) {
		pthread_t pid; //����㴴������ָ�룬Ȼ���ָ�붪��ȥ����ô����ĳһ����ַ��д��ֵ��������
		int error = pthread_create(&pid, NULL, North_thread, (void *)i);
		if (error != 0) printf("can't create thread: %s\n", strerror(error));
	}
	// �������������ߵ�����
	for (int i = 1; i <= 2; i++) {
		pthread_t pid;
		int error = pthread_create(&pid, NULL, South_thread, (void *)i);
		if (error != 0) printf("can't create thread: %s\n", strerror(error));
	}

	_getch();

	return 0;
}
