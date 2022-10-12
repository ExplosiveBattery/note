#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <iostream>


#define sleep(time) Sleep(time*1000)



using namespace std;
struct ThreadInfo
{
	int tid;            //�߳�ID
	char role;         //���ݽ�ɫ n �� s
	double delay;      //�߳��ӳ�
};

HANDLE num = CreateSemaphore(NULL, 2, 2, NULL);
HANDLE north = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE south = CreateSemaphore(NULL, 1, 1, NULL);


void go_north_Thread(void * p)
{
	DWORD m_delay;

	int m_id;
	//�Ӳ����л�ȡ��Ϣ
	m_delay = (DWORD)(((ThreadInfo *)(p))->delay);
	m_id = ((ThreadInfo*)(p))->tid;


	WaitForSingleObject(num, -1); //ȫ��num  wait����

	printf("Go_north thread %d is ready ! \n", m_id);



	WaitForSingleObject(south, -1); //�ֲ�south  wait����

	printf("Go_north thread %d across the south_bridge ! \n", m_id);//ͨ���϶�
	ReleaseSemaphore(south, 1, NULL);
	printf("Go_north thread %d arrive the middle_bridge ! \n", m_id);//�����ж�
	
	WaitForSingleObject(north, -1); //�ֲ�north  wait����
	printf("Go_north thread %d across the north_bridge ! \n", m_id);//ͨ������
	ReleaseSemaphore(north, 1, NULL);//�ֲ�north  signal����
	ReleaseSemaphore(num, 1, NULL);//ȫ��num  signal����
					  // ִ�ж�����

	printf("Go_north thread %d across the all_bridge ! \n", m_id);
	fflush(stdout);
}

void go_south_Thread(void * p)
{
	DWORD m_delay;

	int m_id;
	//�Ӳ����л�ȡ��Ϣ
	m_delay = (DWORD)(((ThreadInfo *)(p))->delay);
	m_id = ((ThreadInfo*)(p))->tid;

	WaitForSingleObject(num, -1); //ȫ��num  wait����

	printf("Go_south thread %d is ready ! \n", m_id);



	WaitForSingleObject(north, -1); //�ֲ�north  wait����

	printf("Go_south thread %d across the north_bridge ! \n", m_id);//ͨ������
	printf("Go_south thread %d arrive the middle_bridge ! \n", m_id);//�����ж�
	ReleaseSemaphore(north, 1, NULL);//�ֲ�south  signal����
	WaitForSingleObject(south, -1); //�ֲ�south  wait����
	printf("Go_south thread %d across the south_bridge ! \n", m_id);//ͨ���϶�
	ReleaseSemaphore(south, 1, NULL);//�ֲ�south  signal����
	ReleaseSemaphore(num, 1, NULL);//ȫ��num  signal����
					  // ִ�ж�����

	printf("Go_south thread %d across the all_bridge ! \n", m_id);
	fflush(stdout);
}

int main(int argc, char ** argv)
{
	DWORD n_thread = 0;  //�߳���Ŀ
	DWORD thread_ID;     //�߳�ID
						 
	HANDLE h_thread[20];//�̶߳�������
	ThreadInfo thread_info[20];

	//��ȡ�����ļ�
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
	//�����߳�
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
	//�ȴ������߳̽���
	DWORD wait_for_all = WaitForMultipleObjects(n_thread, h_thread, true, -1);
	printf("finished!\n");
	return 0;
}




