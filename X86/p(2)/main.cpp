#include<iostream>
#include<windows.h>
#include<stdlib.h>
#include<xmmintrin.h>
#include<immintrin.h>
#include<pthread.h>
#include <semaphore.h>
#include <time.h>
using namespace std;

//ȫ�ֱ���
const int n=1024;
float A[n][n];
const int NUM_THREADS=7;//������߳���

//�߳����ݽṹ����
typedef struct{
    int t_id;
}threadParam_t;

//�ź�������
sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];//ÿ���̵߳��ź���
sem_t sem_workerend[NUM_THREADS];

//�̺߳�������
void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int t_id=p->t_id;
    for(int k=0;k<n;k++){
        sem_wait(&sem_workerstart[t_id]);//�������ȴ����߳���ɳ�������
        //ѭ����������
        for(int i=k+1+t_id;i<n;i+=NUM_THREADS){
            //��ȥ
            for(int j=k+1;j<n;j++){
                A[i][j]=A[i][j]-A[i][k]*A[k][j];
            }
            A[i][k]=0.0;
        }
        sem_post(&sem_main);//�������߳�
        sem_wait(&sem_workerend[t_id]); //�������ȴ����̻߳��ѽ�����һ��

    }
    pthread_exit(NULL);
}

void display(){
	for(int i = 0; i < n; i ++){
		for(int j = 0; j < n; j ++){
			cout<<A[i][j]<<" ";
		}
		cout<<endl;
	}
}

int main(){
    float time=0.0;
    long long head, tail, freq;
    //��ʼ������
    for (int i = 0; i <n; i++)
        {
            for (int j = 0; j <n; j++)
            {
                A[i][j] = rand() % 100;
            }
        }
    for(int i=0;i<10;i++)
    {
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    //��ʼ���ź���
    sem_init(&sem_main,0,0);
    for(int i=0;i<NUM_THREADS;i++){
        sem_init(&sem_workerstart[i],0,0);
        sem_init(&sem_workerend[i],0,0);
    }

    //�����߳�
    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for(int t_id=0;t_id<NUM_THREADS;t_id++){
        param[t_id].t_id=t_id;
        pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
    }

    for(int k=0;k<n;k++){
        //���߳���������������ʱ�����̴߳�������״̬
        for(int j=k+1;j<n;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1.0;

        //��ʼ���ѹ����߳�
        for(int t_id=0;t_id<NUM_THREADS;t_id++){
            sem_post(&sem_workerstart[t_id]);
        }
        //���߳�˯�ߣ��ȴ����й����߳���ɴ�����ȥ��
        for(int t_id=0;t_id<NUM_THREADS;t_id++){
            sem_wait(&sem_main);
        }
        //���߳��ٴλ��ѹ����߳̽�����һ����ȥ
        for(int t_id=0;t_id<NUM_THREADS;t_id++){
            sem_post(&sem_workerend[t_id]);
        }

    }
    for(int t_id=0;t_id<NUM_THREADS;t_id++){
        pthread_join(handles[t_id],NULL);
    }
    //�����ź���
    sem_destroy(&sem_main);
    for(int i=0;i<NUM_THREADS;i++){
        sem_destroy(&sem_workerstart[i]);
        sem_destroy(&sem_workerend[i]);
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    time+=(tail-head)*1000.0 / freq;
    //cout<<"n: "<<n<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
    }
    cout<<"n: "<<n<<" time: "<<time/10<<"ms"<<endl;
    return 0;
}

