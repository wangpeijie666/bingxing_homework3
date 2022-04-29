#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
using namespace std;
//�߳����ݽṹ����
typedef struct
{
    int t_id; //�߳� id
}threadParam_t;
const int n=256;
float A[n][n];
#define NUM_THREADS 7
//barrier ����
pthread_barrier_t barrier_Divsion;
pthread_barrier_t barrier_Elimination;
//�̺߳�������
void *pthreadFunc(void *param)
{
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id;
    //int n=2048;
    for(int k = 0; k < n; ++k)
    {
        if (t_id == 0)
        {
            for(int j =k+1; j< n; j++)
                A[k][j] = A[k][j] / A[k][k];
            A[k][k] = 1.0;
        }
    //��һ��ͬ����
    pthread_barrier_wait(&barrier_Divsion);
    //ѭ����������ͬѧ�ǿ��Գ��Զ������񻮷ַ�ʽ��
    for(int i = k + 1; i < n; i += 1)
		{

			for(int j = k + 1 + t_id; j < n; j += NUM_THREADS)
			{
				A[i][j] -= (A[i][k]*A[k][j]);
			}
			A[i][k] = 0.0;
		}
    // �ڶ���ͬ����
    pthread_barrier_wait(&barrier_Elimination);
    }
    // t_id Ϊ 0 ���߳����������������������߳��ȵȴ�
    pthread_exit(NULL);
}
int main()
{
    //��ʼ�� barrier
    //int n=2048;
    float time=0.0;
    for (int i = 0; i <n; i++)
        {
            for (int j = 0; j <n; j++)
            {
                A[i][j] = rand() % 100;
            }
        }
    //long long head, tail, freq;
    for(int q=0;q<3;q++)
    {
        struct  timeval start;
        struct  timeval end;
        unsigned  long diff;
        gettimeofday(&start, NULL);
        pthread_barrier_init(&barrier_Divsion, NULL, NUM_THREADS);
        pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);
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
        //�������е� barrier
        pthread_barrier_destroy(&barrier_Divsion);
        pthread_barrier_destroy(&barrier_Elimination);
        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        time+=diff;
    }
    cout<<"n="<<n<<" "<<time/3<<"us"<<endl;
    return 0;
}
