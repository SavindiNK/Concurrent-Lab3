#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

double getRandomDouble(){
    srand(time(NULL));
    return (double)rand()/RAND_MAX;
}

void makeMatrix(double *matrix, int size){
    for(int r=0;r<size;r++){
        for(int c=0;c<size;c++){
            *(matrix + r*size + c) = getRandomDouble();
        }
    }
}

void printMatrix(double *matrix, int size){
    for(int r=0;r<size;r++){
        for(int c=0;c<size;c++){
            printf("%f ", *(matrix + r*size + c));
        }
        printf("\n");
    }
}

long long matrixMultiplicationSeq(double *matA, double *matB, double *result, int size){

    double locMatA[size][size];
    double locMatB[size][size];
    double locResult[size][size];

    long long begin,end;
    long long time_taken;

    for(int r=0;r<size;r++){
        for(int c=0;c<size;c++){
            locMatA[r][c] =  *(matA +  r*size + c);
            locMatB[r][c] =  *(matB +  r*size + c);
        }
    }

    begin = timeInMilliseconds();
    printf("%d\n",begin);
    for(int r=0;r<size;r++){
        for(int c=0;c<size;c++){
            double sum = 0;
            for(int p=0;p<size;p++){
                sum += locMatA[r][p] * locMatB[p][c];
            }
            locResult[r][c] = sum;
        }
    }
    end = timeInMilliseconds();
    printf("%d\n",end);
    time_taken = end - begin;

    for(int r=0;r<size;r++){
        for(int c=0;c<size;c++){
            *(result +  r*size + c) = locResult[r][c];
        }
    }

    return time_taken;
}

int main()
{
    int SIZE = 500;
    double matA[SIZE][SIZE];
    double matB[SIZE][SIZE];
    double matResult[SIZE][SIZE];

    makeMatrix(*matA, SIZE);
    makeMatrix(*matB, SIZE);

    long long timeTaken = matrixMultiplicationSeq(*matA, *matB, *matResult, SIZE);

    // printMatrix(*matResult, 3);
    printf("Time elapsed: %d\n", timeTaken);

    return 0;
}