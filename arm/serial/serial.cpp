#include<iostream>
#include <sys/time.h>
using namespace std;
void** gaussserial(float** matrix,int N)
//串行高斯消去
{
    for (int k = 0; k < N; k++)
    {
        float  diagonal = matrix[k][k];
        for (int j = k; j < N; j++)
        {
            matrix[k][j] = matrix[k][j] / diagonal;
        }
        for (int i = k + 1; i < N; i++)
        {
            float tmpt = matrix[i][k];
            for (int j = k + 1; j < N; j++)
            {
                matrix[i][j] = matrix[i][j] - tmpt * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}


int main()
{
    int N[9] = {8,32,128,256,512,1024,2048,4096,6000};
    for(int p=0;p<9;p++)
    {
        //初始化矩阵并生成一个随机数矩阵
        float** matrix = new float* [N[p]];
        for (int i = 0; i < N[p]; i++)
        {
            matrix[i] = new float[N[p]];
        }
        for (int i = 0; i < N[p]; i++)
        {
            for (int j = 0; j < N[p]; j++)
            {
                matrix[i][j] = rand() % 100;
            }
        }
        struct  timeval start;
        struct  timeval end;
        unsigned  long diff;
        gettimeofday(&start, NULL);
        gaussserial(matrix,N[p]);
        gettimeofday(&end, NULL);
        cout<<"N="<<N[p]<<" ";
        diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        cout << diff << "us" << endl;
        //print(M,N[p]);
    }
    system("pause");
    return 0;

}

