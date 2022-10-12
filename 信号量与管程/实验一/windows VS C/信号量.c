//以第一题吃水果问题为例

//用到!的地方都要注意下，一般函数执行成功返回0，但是函数分配内存、制造失败返回0
#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

void Father();
void Son();
void Daughter();
#define sleep(time) Sleep(time*1000)
HANDLE hPlate = CreateSemaphore(NULL, 1, 1, (LPCTSTR)"plate");
HANDLE hApple = CreateSemaphore(NULL, 0, 1, (LPCTSTR)"apple");
HANDLE hOrrange = CreateSemaphore(NULL, 0, 1, (LPCTSTR)"orrange");

int main(void) {
	if ( !hApple && !hOrrange && !hPlate ) {
		fprintf(stderr, "hApple or hOrrange or hPlate can't create!");
		exit(1);
	}

	//创建三个线程
	HANDLE hArray[3];
	hArray[0] =CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Father, NULL, 0, NULL);
	hArray[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Son, NULL, 0, NULL);
	hArray[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Daughter, NULL, 0, NULL);
	for (int i1 = 0; i1 < 3; ++i1) 
		if(!hArray[i1]){
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//主线程等待三个子线程的结束才结束
	WaitForMultipleObjects(1, hArray, TRUE,  INFINITE);

	//销毁创建的HANDLE
	CloseHandle(hApple);
	CloseHandle(hOrrange);
	for (int i1 = 0; i1 < 3; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}


void Father() {
	srand( (unsigned)time(0) );//把时间作为种子来进行之后的随机运算
	while( !WaitForSingleObject(hPlate, INFINITE) ) {
		sleep(3);
		if( rand()%2==0 ) {
			puts("The Father puts a apple in plate.");
			ReleaseSemaphore(hApple, 1, NULL);
			
		}else{
			puts("The Father puts a orrange in plate.");
			ReleaseSemaphore(hOrrange, 1, NULL);
		}
		fflush(stdout);
	}

	CloseHandle(hPlate);
	
}

void Son() {
	//如果盘子里面有Apple就会进入循环，否则就会阻塞在循环入口
	while( !WaitForSingleObject(hApple, INFINITE) ) { //相当于P操作
		puts("The son eat a apple."); 
		ReleaseSemaphore(hPlate, 1, NULL); //相当于V操作
	}
}

void Daughter() {
	//如果盘子里面有Orrange就会进入循环，否则就会阻塞在循环入口
	while( !WaitForSingleObject(hOrrange, INFINITE) ) {
		puts("The daughter eat a orrange.");
		ReleaseSemaphore(hPlate, 1, NULL);
	}
}
