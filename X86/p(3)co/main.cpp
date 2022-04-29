#include<iostream>
#include<math.h>
#include<cstdlib>
#include<windows.h>
#include<stdlib.h>
#include<xmmintrin.h>
#include<fstream>
#include<immintrin.h>
#include<pthread.h>
#include <semaphore.h>
using namespace std;
typedef struct
{
    int t_id; // �߳� id
}threadParam_t;
const int n=2048;
float A[n][n];
#define NUM_THREADS 5
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS-1];
sem_t sem_Elimination[NUM_THREADS-1];
//�̺߳�������
void *pthreadFunc(void *param)
{
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id; //�̱߳��
    //int n=2048;
    for(int k=0;k<n;k++)
    {
        //idΪ0���߳��������������������߳��ȵȴ�
        if(t_id==0)
        {
            for(int j=k+1;j<n;j++)
                A[k][j]/=A[k][k];
            A[k][k]=1.0;
        }
        else
            sem_wait(&sem_Division[t_id-1]);
        //�������ȴ��������
        if (t_id == 0)
        {
            //t_idΪ0���̻߳������������߳�
            for(int i=0;i<NUM_THREADS-1;i++)
                sem_post(&sem_Division[i]);
        }
        //ѭ����������
        for(int i = k + 1; i < n; i += 1)
		{

			for(int j = k + 1 + t_id; j < n; j += NUM_THREADS)
			{
				A[i][j] -= (A[i][k]*A[k][j]);
			}
			A[i][k] = 0.0;
		}
        if (t_id == 0)
        {
            for(int i=0;i<NUM_THREADS-1;i++)
                sem_wait(&sem_leader); // �ȴ����� worker �����ȥ
            for (int i = 0; i < NUM_THREADS-1; ++i)
                sem_post(&sem_Elimination[i]); // ֪ͨ���� worker ������һ��
        }
        else
        {
            sem_post(&sem_leader);// ֪ͨ leader, �������ȥ����
            sem_wait(&sem_Elimination[t_id-1]); // �ȴ�֪ͨ��������һ��
        }
    }
    pthread_exit(NULL);
}

int main()
{
    //��ʼ���ź���
    //int n=2048;
    float time=0.0;
    for (int i = 0; i <n; i++)
    {
        for (int j = 0; j <n; j++)
        {
            A[i][j] = rand() % 100;
        }
    }
    long long head, tail, freq;
    for(int q=0;q<3;q++)
    {
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&head);
        sem_init(&sem_leader, 0, 0);
        for (int i=0; i < NUM_THREADS-1; ++i)
        {
            sem_init(&sem_Division[i],0,0);
            sem_init(&sem_Elimination[i], 0, 0);
        }
        //�����߳�
        pthread_t handles[NUM_THREADS];// ������Ӧ�� Handle
        threadParam_t param[NUM_THREADS];// ������Ӧ���߳����ݽṹ
        for(int t_id = 0; t_id < NUM_THREADS; t_id++)
        {
            param[t_id].t_id = t_id;
            pthread_create(&handles[t_id],NULL,pthreadFunc,(void*)&param[t_id]);
        }
        for(int t_id = 0; t_id < NUM_THREADS; t_id++)
            pthread_join(handles[t_id],NULL);
        // ���������ź���
        sem_destroy(&sem_leader);
        for (int i=0; i < NUM_THREADS-1; ++i)
        {
            sem_destroy(&sem_Division[i]);
            sem_destroy(&sem_Elimination[i]);
        }
        QueryPerformanceCounter((LARGE_INTEGER *)&tail);
        time+=(tail-head)*1000.0 / freq;
    }
    cout<<"N="<<n<<" "<<time/3<<"ms"<<endl;
    return 0;
}