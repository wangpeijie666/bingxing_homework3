#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
using namespace std;
//ȫ�ֱ���
const int n=256;
float A[n][n];
const int NUM_THREADS=7;//������߳���
const int NUM_ROWS=n/NUM_THREADS; //ÿ���̸߳�����еĸ���

typedef struct{
    int k; //��ȥ���ִ�
    int t_id; //�߳�id
}threadParam_t;

void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int k=p->k; //��ȥ���ִ�
    int t_id=p->t_id; //�̱߳��
    //int i=k+t_id+1; //��ȡ�Լ��ļ�������
    for(int i=k+t_id*NUM_ROWS+1;i<min(k+t_id*NUM_ROWS+1+NUM_ROWS,n);i++){
        for(int j=k+1;j<n;j++){
            A[i][j]=A[i][j]-A[i][k]*A[k][j];
        }
        A[i][k]=0;
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

int main()
{
    float time=0.0;
    //��ʼ������
    for (int i = 0; i <n; i++)
        {
            for (int j = 0; j <n; j++)
            {
                A[i][j] = rand() % 100;
            }
        }
    for(int q=0;q<3;q++)
    {
        struct  timeval start;
        struct  timeval end;
        unsigned  long diff;
        gettimeofday(&start, NULL);
        for(int k=0;k<n;k++){
        //���߳�����������
        for(int j=k+1;j<n;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1.0;

        //���������̣߳�������ȥ����
        int worker_count=NUM_THREADS; //�����̵߳�����
        pthread_t* handles=new pthread_t[worker_count]; //������Ӧ��Handle����̬����
        threadParam_t* param=new threadParam_t[worker_count]; //������Ӧ���߳����ݽṹ����̬����

        //��������
        for(int t_id=0;t_id<worker_count;t_id++){
            param[t_id].k=k;
            param[t_id].t_id=t_id;
        }

        //�����߳�
        for(int t_id=0;t_id<worker_count;t_id++){
            pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
        }

        //���̹߳�������й����߳���ɴ�����ȥ����
        for(int t_id=0;t_id<worker_count;t_id++){
            pthread_join(handles[t_id],NULL);
        }

    }

        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        time+=diff;
    }
   cout<<"n: "<<n<<" time: "<<time/3<<"us"<<endl;
    return 0;
}

