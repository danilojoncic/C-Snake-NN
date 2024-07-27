#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <pthread.h>


double** allocateMatrix(int rows, int cols) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
    }
    return matrix;
}

void setRandomValues(int rows, int cols,double **matrix,double min, double max){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            matrix[i][j] = min + (double)rand() / RAND_MAX * (max - min);
        }
    }
}

double** matrixAddition(int rows, int cols, double** matrixA, double** matrixB){
    double** outputMatrix = allocateMatrix(rows,cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            outputMatrix[i][j] = matrixA[i][j] + matrixB[i][j];
        }
    }
    return outputMatrix;
}

double rowColumnCalculation(int i, int j,int colsFromA, double** matrixA, double** matrixB) {
    double sum = 0;
    for (int index = 0; index < colsFromA; index++) {
        sum += matrixA[i][index] * matrixB[index][j];
    }
    return sum;
}

double** matrixMultiplication(int rowsFromA,int colsFromA,int rowsFromB,int colsFromB,double** matrixA, double** matrixB){
    if(colsFromA != rowsFromB) printf("CORDINATE MISMATCH!\n");
    double** outputMatrix = allocateMatrix(rowsFromA,colsFromB);

    for(int i = 0; i < rowsFromA; i++){
        for(int j = 0; j < colsFromB; j++){
            outputMatrix[i][j] = rowColumnCalculation(i,j,colsFromA,matrixA,matrixB);
        }
    }

    return outputMatrix;
}

void freeMatrix(int rows, double** matrix) {
    if (matrix != NULL) {
        for (int i = 0; i < rows; i++) {
            if (matrix[i] != NULL) {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

void printMatrix(int rows,int cols, double** matrix){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%f ",matrix[i][j]);
        }
        printf("\n");
    }
}
