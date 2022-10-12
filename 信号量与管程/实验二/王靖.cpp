#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <iostream>


#define sleep(time) Sleep(time*1000)



using namespace std;
struct ThreadInfo
{
	int tid;            //线程ID
	char role;         //扮演角色 n 或 s
	double delay;      //线程延迟
};

HANDLE num = CreateSemaphore(NULL, 2, 2, NULL);
HANDLE north = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE south = CreateSemaphore(NULL, 1, 1, NULL);


void go_north_Thread(void * p)
{
	DWORD m_delay;

	int m_id;
	//从参数中获取信息
	m_delay = (DWORD)(((ThreadInfo *)(p))->delay);
	m_id = ((ThreadInfo*)(p))->tid;


	WaitForSingleObject(num, -1); //全局num  wait操作

	printf("Go_north thread %d is ready ! \n", m_id);



	WaitForSingleObject(south, -1); //局部south  wait操作

	printf("Go_north thread %d across the south_bridge ! \n", m_id);//通过南段
	ReleaseSemaphore(south, 1, NULL);
	printf("Go_north thread %d arrive the middle_bridge ! \n", m_id);//到达中段
	
	WaitForSingleObject(north, -1); //局部north  wait操作
	printf("Go_north thread %d across the north_bridge ! \n", m_id);//通过北段
	ReleaseSemaphore(north, 1, NULL);//局部north  signal操作
	ReleaseSemaphore(num, 1, NULL);//全局num  signal操作
					  // 执行读操作

	printf("Go_north thread %d across the all_bridge ! \n", m_id);
	fflush(stdout);
}

void go_south_Thread(void * p)
{
	DWORD m_delay;

	int m_id;
	//从参数中获取信息
	m_delay = (DWORD)(((ThreadInfo *)(p))->delay);
	m_id = ((ThreadInfo*)(p))->tid;

	WaitForSingleObject(num, -1); //全局num  wait操作

	printf("Go_south thread %d is ready ! \n", m_id);



	WaitForSingleObject(north, -1); //局部north  wait操作

	printf("Go_south thread %d across the north_bridge ! \n", m_id);//通过北段
	printf("Go_south thread %d arrive the middle_bridge ! \n", m_id);//到达中段
	ReleaseSemaphore(north, 1, NULL);//局部south  signal操作
	WaitForSingleObject(south, -1); //局部south  wait操作
	printf("Go_south thread %d across the south_bridge ! \n", m_id);//通过南段
	ReleaseSemaphore(south, 1, NULL);//局部south  signal操作
	ReleaseSemaphore(num, 1, NULL);//全局num  signal操作
					  // 执行读操作

	printf("Go_south thread %d across the all_bridge ! \n", m_id);
	fflush(stdout);
}

int main(int argc, char ** argv)
{
	DWORD n_thread = 0;  //线程数目
	DWORD thread_ID;     //线程ID
						 
	HANDLE h_thread[20];//线程对象数组
	ThreadInfo thread_info[20];

	//读取输入文件
	ifstream inFile;
	inFile.open("1.txt");
	if (!inFile)
	{
		printf("error in open file !\n");
		return -1;
	}
	while (inFile)
	{
		inFile >> thread_info[n_thread].tid;
		inFile >> thread_info[n_thread].role;
		inFile >> thread_info[n_thread].delay;
		inFile.get();
		n_thread++;
	}
	//创建线程
	for (int i = 0; i < n_thread; i++)
	{
		if (thread_info[i].role == 'n')
		{
			h_thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)go_north_Thread, &thread_info[i], 0, &thread_ID);
		}
		else
		{
			h_thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)go_south_Thread, &thread_info[i], 0, &thread_ID);
		}
	}
	//等待所有线程结束
	DWORD wait_for_all = WaitForMultipleObjects(n_thread, h_thread, true, -1);
	printf("finished!\n");
	return 0;
}




