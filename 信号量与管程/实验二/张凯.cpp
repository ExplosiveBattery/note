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
	int id = (int)arg; // 将传递进来的参数作为当前的线程的标示
	fprintf(stderr, "Northwalker %d sents the north request\n", id);
	ToNorth(id);
	return ((void *)0);
}

void *South_thread(void *arg) {
	int id = (int)arg; // 将传递进来的参数作为当前的线程的标示
	fprintf(stderr, "Southwalker %d sents the south request\n", id);
	ToSouth(id);
	return ((void *)0);
}

int main(void) {
	// 创建几个向北走的行人
	for (int i = 1; i <= 3; i++) {
		pthread_t pid; //如果你创建的是指针，然后把指针丢过去，那么会向某一个地址中写入值，这很糟糕
		int error = pthread_create(&pid, NULL, North_thread, (void *)i);
		if (error != 0) printf("can't create thread: %s\n", strerror(error));
	}
	// 创建几个向南走的行人
	for (int i = 1; i <= 2; i++) {
		pthread_t pid;
		int error = pthread_create(&pid, NULL, South_thread, (void *)i);
		if (error != 0) printf("can't create thread: %s\n", strerror(error));
	}

	_getch();

	return 0;
}
