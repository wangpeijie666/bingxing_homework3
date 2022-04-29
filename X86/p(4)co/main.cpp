#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<iostream>
#include<time.h>
#include<Windows.h>
using namespace std;
typedef struct{
	int t_id;
}threadParam_t;

const int n=2048;
float A[n][n];
#define NUM_THREADS 7

pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;

void *threadFunc(void *param){
	threadParam_t *p = (threadParam_t *)param;
	int t_id = p->t_id;
	for(int k = 0; k < n; k ++)
    {
		if(t_id == 0)
        {
			for(int j = k + 1; j < n; j++)
			{
				A[k][j] = A[k][j] / A[k][k];
			}
			A[k][k] = 1.0;
        }
		pthread_barrier_wait(&barrier_Division);//第一个同步点
		for(int i = k + 1; i < n; i += 1)
		{

			for(int j = k + 1 + t_id; j < n; j += NUM_THREADS)
			{
				A[i][j] -= (A[i][k]*A[k][j]);
			}
			A[i][k] = 0.0;
		}
		pthread_barrier_wait(&barrier_Elimination);
	}
	pthread_exit(NULL);

}

int main()
{
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
        long long head, tail, freq;
        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        pthread_barrier_init(&barrier_Division, NULL, NUM_THREADS);
        pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);
        pthread_t handles[NUM_THREADS];
        threadParam_t param[NUM_THREADS];
        for(int t_id = 0; t_id < NUM_THREADS; t_id ++){
            param[t_id].t_id = t_id;
            pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
        }
        for(int t_id = 0; t_id < NUM_THREADS; t_id ++){
            pthread_join(handles[t_id], NULL);
        }
        pthread_barrier_destroy(&barrier_Division);
        pthread_barrier_destroy(&barrier_Elimination);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        time+=(tail-head)*1000.0/freq;
    }
    cout<<"n="<<n<<" Col: "<<time/3<<"ms"<<endl;
	return 0;
}
