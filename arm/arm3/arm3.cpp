#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
using namespace std;
typedef struct
{
    int t_id; // 线程 id
}threadParam_t;
const int n=2048;
float A[n][n];
#define NUM_THREADS 7
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS-1];
sem_t sem_Elimination[NUM_THREADS-1];
//线程函数定义
void *pthreadFunc(void *param)
{
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id; //线程编号
    //int n=2048;
    for(int k=0;k<n;k++)
    {
        //id为0的线程先做除法，其他工作线程先等待
        if(t_id==0)
        {
            for(int j=k+1;j<n;j++)
                A[k][j]/=A[k][k];
            A[k][k]=1.0;
        }
        else
            sem_wait(&sem_Division[t_id-1]);
        //阻塞，等待除法完成
        if (t_id == 0)
        {
            //t_id为0的线程唤醒其他工作线程
            for(int i=0;i<NUM_THREADS-1;i++)
                sem_post(&sem_Division[i]);
        }
        //循环划分任务
        for(int i=k+1+t_id; i < n; i += NUM_THREADS)
        //消去
        {
            for (int j = k + 1; j < n; ++j)
                A[i][j] = A[i][j]-A[i][k]*A[k][j];
            A[i][k]=0.0;
        }
        if (t_id == 0)
        {
            for(int i=0;i<NUM_THREADS-1;i++)
                sem_wait(&sem_leader); // 等待其它 worker 完成消去
            for (int i = 0; i < NUM_THREADS-1; ++i)
                sem_post(&sem_Elimination[i]); // 通知其它 worker 进入下一轮
        }
        else
        {
            sem_post(&sem_leader);// 通知 leader, 已完成消去任务
            sem_wait(&sem_Elimination[t_id-1]); // 等待通知，进入下一轮
        }
    }
    pthread_exit(NULL);
}

int main()
{
    //初始化信号量
    //int n=2048;
    float time=0.0;
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
    sem_init(&sem_leader, 0, 0);
    for (int i=0; i < NUM_THREADS-1; ++i)
    {
        sem_init(&sem_Division[i],0,0);
        sem_init(&sem_Elimination[i], 0, 0);
    }
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
    // 销毁所有信号量
    sem_destroy(&sem_leader);
    for (int i=0; i < NUM_THREADS-1; ++i)
    {
        sem_destroy(&sem_Division[i]);
        sem_destroy(&sem_Elimination[i]);
    }
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    time+=diff;
    }
    cout<<"n: "<<n<<" time: "<<time/3<<"us"<<endl;
    return 0;
}
