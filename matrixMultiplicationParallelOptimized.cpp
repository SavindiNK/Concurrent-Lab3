#include <iostream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace std;

double getRandomNumber(double start, double end);
long getCurrentTime();
double** makeEmptyMatrix(int size);
double** makeMatrix(int size, int start, int end);
void printMatrix(double** matrix, int size);
long matrixMultiplicationSeq(double** matrixA, double** matrixB, double** matrixResult, int size);
double getElapsedTime(int size);
double getMean(double array[], int size);
double getStd(double array[], int size);
int getTestSize(double mean, double s,  double z, double r);

int main(int argc, char *argv[]) 
{
    int SAMPLE_SIZE = 5;
    double Z = 1.96;
    double R = 5;

    int TEST_SIZE;

    ofstream myfile;
    myfile.open ("matrixMultiplicationParallelOptimized.txt");

    for(int SIZE=200; SIZE <= 2000; SIZE=SIZE+200){
        double sampleTime[SAMPLE_SIZE];
        double mean, s;
        double fmean, fs;

        for(int i=0; i<SAMPLE_SIZE; i++){
            sampleTime[i] = getElapsedTime(SIZE);
        }
        mean = getMean(sampleTime, SAMPLE_SIZE);
        s = getStd(sampleTime, SAMPLE_SIZE);
        TEST_SIZE = getTestSize(mean, s, Z, R);
        cout << SIZE << ": " << TEST_SIZE << endl;

        double elapsedTime[TEST_SIZE];
        for(int i=0; i<TEST_SIZE; i++){
            elapsedTime[i] = getElapsedTime(SIZE);
        }
        
        fmean = getMean(elapsedTime, TEST_SIZE);
        fs = getStd(elapsedTime, TEST_SIZE);
        myfile << SIZE << " " << fmean << " " << fs << " " << TEST_SIZE << " " << mean << " " << s << endl;
    }  

    myfile.close();
    return 0;
}

double getRandomNumber(double start, double end) {
    srand(static_cast<unsigned>(time(NULL)));
    double f = (double)rand() / RAND_MAX;
    return start + f * (end - start);
}

long getCurrentTime() {
    timeval now;
    gettimeofday(&now, NULL);
    long timeInMircoSeconds = (now.tv_sec * 1000000) + now.tv_usec;
    return timeInMircoSeconds;
}

double** makeEmptyMatrix(int size){
    double** matrix = new double*[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new double[size];
        for (int j = 0; j < size; j++) {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

double** makeMatrix(int size, int start, int end) {
    double** matrix = new double*[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new double[size];
        for (int j = 0; j < size; j++) {
            matrix[i][j] = getRandomNumber(start, end);
        }
    }
    return matrix;
}

void printMatrix(double** matrix, int size){
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

long matrixMultiplicationSeq(double** matrixA, double** matrixB, double** matrixResult, int size) {
    long startTime, endTime, elapsedTime;

    startTime = getCurrentTime(); // Get the starting time
    double** matrixBT = new double*[size];
    for (int p = 0; p < size; p++) {
        matrixBT[p] = new double[size];
        for (int q = 0; q < size; q++) {
            matrixBT[p][q] = matrixB[q][p];
        }
    }

    int i,j;
    #pragma omp parallel shared(matrixResult) private(i,j,k)
    {
        #pragma omp for  schedule(static)
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                double sum = 0;
                for (k = 0; k < size; k++) {
                    sum += matrixA[i][k] * matrixB[j][k];
                }
                matrixResult[i][j] = sum;
            }
        }
    }

    endTime = getCurrentTime(); // Get the ending time

    elapsedTime = endTime - startTime; 
    return elapsedTime;
}

double getElapsedTime(int size){
    double** matA = makeMatrix(size, 0, 100);
    double** matB = makeMatrix(size, 0, 100);
    double** matResult = makeEmptyMatrix(size);

    long elapsedTime = matrixMultiplicationSeq(matA,matB,matResult,size);

    delete matA;
    delete matB;
    delete matResult;

    return (double)elapsedTime/1000000;
}

double getMean(double array[], int size){
    double sum = 0;
    for(int i=0; i<size; i++){
        sum += array[i];
    }
    return sum/size;
}

double getStd(double array[], int size){
    double mean = getMean(array, size);
    double std = 0;
    for(int i=0; i< size; i++){
        std += pow(array[i]-mean, 2);
    }
    return sqrt(std/(size-1));
}

int getTestSize(double mean, double s, double z, double r){
    int size = (int)pow((100*z*s)/(r*mean),2);
    if(size < 5){
        size = 5;
    }
    return size;
}