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
//线程数据结构定义
typedef struct
{
    int t_id; //线程 id
}threadParam_t;
const int n=2048;
float A[n][n];
#define NUM_THREADS 7
//barrier 定义
pthread_barrier_t barrier_Divsion;
pthread_barrier_t barrier_Elimination;
//线程函数定义
void *pthreadFunc(void *param)
{
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id;
    //int n=2048;
    for (int k = 0; k < n; k++)
    {
        for(int i=k+1+t_id; i < n; i += NUM_THREADS)
            A[k][i] = A[k][i] / A[k][k];
        A[k][k] = 1.0;
        pthread_barrier_wait(&barrier_Divsion);
        //循环划分任务（同学们可以尝试多种任务划分方式）
        for(int i=k+1+t_id; i < n; i += NUM_THREADS)
        {
            //消去
            for (int j = k + 1; j < n; ++j)
                A[i][j] = A[i][j]-A[i][k]*A[k][j];
            A[i][k]=0.0;
        }
        pthread_barrier_wait(&barrier_Elimination);
    }
    // t_id 为 0 的线程做除法操作，其它工作线程先等待
    pthread_exit(NULL);
}
int main()
{
    //初始化 barrier
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
    for(int p=0;p<3;p++)
    {
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&head);
        pthread_barrier_init(&barrier_Divsion, NULL, NUM_THREADS);
        pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);
        //创建线程
        pthread_t handles[NUM_THREADS];// 创建对应的 Handle
        threadParam_t param[NUM_THREADS];// 创建对应的线程数据结构
        for(int t_id = 0; t_id < NUM_THREADS; t_id++)
        {
            param[t_id].t_id = t_id;
            pthread_create(&handles[t_id],NULL,pthreadFunc,(void*)&param[t_id]);
        }
        for(int t_id = 0; t_id < NUM_THREADS; t_id++)
            pthread_join(handles[t_id],NULL);
        //销毁所有的 barrier
        pthread_barrier_destroy(&barrier_Divsion);
        pthread_barrier_destroy(&barrier_Elimination);
        QueryPerformanceCounter((LARGE_INTEGER *)&tail);
        time+=(tail-head)*1000.0 / freq;
    }
    cout<<"n="<<n<<" "<<time/3<<"ms"<<endl;
    return 0;
}
