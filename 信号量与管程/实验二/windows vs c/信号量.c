//��ʵ�����Ҫע������������⣬����˵������ͬʱ�����϶�׼�������ߣ�������˸��򱱵�
//����취���趨��Դ����˳�򣬱���˵���º��ϡ��������ȱ�����
//���Ǵ��Ÿ�������¶Խ�ͨ�������������������ź��������ֱ��ȱ����Ϻ�һЩ������Ҳû�дﵽ���̶��϶Խ�ͨ�Ľ������
//Ҫ����Դ�����ʴﵽ����Ǿ���Ҫ��������������жϣ��������������ͬһ��������ô�����������������·����Ȼ������˵��ƶ��ٶȶ���һ������ô�ǲ�����������������������������ʱ���������߶�����
//�ܵ���˵��ʹ�������ź������������ߵ����ţ�ʹ����������������֤BridgeControl���ϱ��������Ķ�д����
#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#define sleep(time) Sleep(time*1000)

void BridgeControl(void);
void FromNorth(void); //go to south
void FromSouth(void); //go to north
void PrintBridge(void);

#define THREAD_NUM 11
HANDLE hMutexArray[2]; //[0]hNorthMutex [1]hSouthMutex  //BridgeControl�ٿ�North��South��������ʱ����Ҫ����������Control֮��ֱ𻥳�
char cBridge[4];//[0]NrothBuffer [1][2]MiddleBuffer  [3]SouthBuffer
//HANDLE hNorthSemaphore, hSouthSemaphore;

int main(void) {
	//���б�Ҫ�ĳ�ʼ����׼������
	srand((unsigned)time(0));
	hMutexArray[0] =CreateMutex(NULL, TRUE, NULL);
	hMutexArray[1] =CreateMutex(NULL, TRUE, NULL);
	hVarMutex =CreateMutex(NULL, TRUE, NULL);
//	hNorthSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
//	hSouthSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	//����THREAD_NUM���߳�
	HANDLE hArray[21];
	hArray[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BridgeControl, NULL, 0, NULL);
	for(int i1=1; i1 < THREAD_NUM; ++i1) {//�������THREAD_NUM-1������������Ҫ������ţ�����α���
		sleep(rand() % 3);
		if ( rand()%2==0 )
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FromNorth, NULL, 0, NULL);
		else
			hArray[i1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FromSouth, NULL, 0, NULL);
	}
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		if (!hArray[i1]) {
			fprintf(stderr, "hArray[%d] can't create!", i1);
			exit(2);
		}
	//���̵߳ȴ��������̵߳Ľ����Ž���
	WaitForMultipleObjects(THREAD_NUM, hArray, TRUE, INFINITE);

	//���ٴ�����HANDLE
	CloseHandle(hMutexArray[0]);
	CloseHandle(hMutexArray[1]);
//	CloseHandle(hNorthSemaphore);
//	CloseHandle(hSouthSemaphore);
	for (int i1 = 0; i1 < THREAD_NUM; ++i1)
		CloseHandle(hArray[i1]);
	return 0;
}

void FromNorth(void) {
//	WaitForSingleObject(hNorthSemaphore, INFINITE);
	WaitForSingleObject(hVarMutex, INFINITE);
	WaitForSingleObject(hMutexArray[0], INFINITE);
	cBridge[0] = 'N';
	ReleaseMutex(hVarMutex);
}

void FromSouth(void) {
//	WaitForSingleObject(hSouthSemaphore, INFINITE);
	WaitForSingleObject(hVarMutex, INFINITE);
	WaitForSingleObject(hMutexArray[1], INFINITE);
	cBridge[3] = 'S';
	ReleaseMutex(hVarMutex);
}

//�����������ϵ�����
//���������м������խ�����Դ�Ч��������Ҫ�����˴����ߵ����м䣬�����˴��м䵽���ߣ����ǲ�������ÿ��ֻ����һ���Ļ�Ҳ�Ͳ�����deadlock��
void BridgeControl(void) {
	while(1) {
		//�������ߵ��м�
		WaitForSingleObject(hVarMutex, INFINITE);
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
		ReleaseMutex(hMutexArray[0]);
		ReleaseMutex(hMutexArray[1]);
		ReleaseMutex(hVarMutex);
	}
}

void PrintBridge(void) {
	printf(" %c\n", cBridge[1]);
	printf("%c %c\n", cBridge[0], cBridge[3]);
	printf(" %c\n", cBridge[2]);
	fflush(stdout);
}
