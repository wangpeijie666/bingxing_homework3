#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
using namespace std;
//全局变量
const int n=256;
float A[n][n];
const int NUM_THREADS=7;//分配的线程数
const int NUM_ROWS=n/NUM_THREADS; //每个线程负责的行的个数

typedef struct{
    int k; //消去的轮次
    int t_id; //线程id
}threadParam_t;

void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int k=p->k; //消去的轮次
    int t_id=p->t_id; //线程编号
    //int i=k+t_id+1; //获取自己的计算任务
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
    //初始化数组
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
        //主线程做除法操作
        for(int j=k+1;j<n;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1.0;

        //创建工作线程，进行消去操作
        int worker_count=NUM_THREADS; //工作线程的数量
        pthread_t* handles=new pthread_t[worker_count]; //创建对应的Handle，动态分配
        threadParam_t* param=new threadParam_t[worker_count]; //创建对应的线程数据结构，动态分配

        //分配任务
        for(int t_id=0;t_id<worker_count;t_id++){
            param[t_id].k=k;
            param[t_id].t_id=t_id;
        }

        //创建线程
        for(int t_id=0;t_id<worker_count;t_id++){
            pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
        }

        //主线程挂起等所有工作线程完成此轮消去工作
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

