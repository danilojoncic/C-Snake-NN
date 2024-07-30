#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <pthread.h>

/**
 * Allocates a matrix on the heap using malloc for the rows and collumns
 * @param rows the number of rows of our matrix
 * @param cols the number of columns of out matrix
 * @return return a matrix of doubles, a double pointer for a double type
 */
double** allocateMatrix(int rows, int cols) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
    }
    return matrix;
}

/**
 * Sets the value of all elements of the input matrix as a random value defined in scope by the min and max values
 * @param rows the number of rows of the matrix
 * @param cols the number of columns of the matrix
 * @param matrix out input matrix whoose values we set to random
 * @param min the minimum used for calculating the random, i.e. no random value of the matrix will be smaller that this number
 * @param max the maximum used for calculating the random, i.e. no random value of the matrix will  be larger that this number
 */
void setRandomValues(int rows, int cols,double **matrix,double min, double max){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            matrix[i][j] = min + (double)rand() / RAND_MAX * (max - min);
        }
    }
}
/**
 * calculates the result and gives back a new allocated matrix that contains the result values after adding together two input matrices
 * @param rows the number of rows that both matrices have
 * @param cols the number of columns that both matrices have
 * @param matrixA first input matrix used in addition
 * @param matrixB second input matrix used in addition
 * @return a new matrix that contains the result of addition between the two input matrices, returns a double pointer to da double data type
 */
double** matrixAddition(int rows, int cols, double** matrixA, double** matrixB){
    double** outputMatrix = allocateMatrix(rows,cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            outputMatrix[i][j] = matrixA[i][j] + matrixB[i][j];
        }
    }
    return outputMatrix;
}
/**
 * Helper function for matrix multiplication, calculates the sum of multiplications from the given row column combination
 * @param i index of the row that we want to use for calculation
 * @param j index of column that we want to use for calculation
 * @param colsFromA number of columns from input matrixA, needs to be the same as the number of rows from input matrixB
 * @param matrixA input matrixA whoose rows we will be going thru
 * @param matrixB input matrixB whoose columns we will be going thru
 * @return a double value that represent the sum of multiplications of all elements from a given row column combination
 */
double rowColumnCalculation(int i, int j,int colsFromA, double** matrixA, double** matrixB) {
    double sum = 0;
    for (int index = 0; index < colsFromA; index++) {
        sum += matrixA[i][index] * matrixB[index][j];
    }
    return sum;
}
/**
 * calculates the dot product of two matrices, does multiplication and gives back the result
 * when calling the function make sure that the number of rows columns from A matches the number of rows from B
 * @param rowsFromA number of rows from input matrixA
 * @param colsFromA number of columns from input matrixA
 * @param rowsFromB number of rows from inputMatrixB
 * @param colsFromB number of columns from inputMatrixB
 * @param matrixA input matrixA used in multiplication
 * @param matrixB input matrixB used in multiplication
 * @return
 */
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
/**
 * Frees the allocated memory of the input matrix
 * @param rows the number of rows of the matrix
 * @param matrix the input matrix that we want to free, and not use anymore
 */
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
/**
 * prints out all values of a matrix
 * @param rows the number of rows of the matrix
 * @param cols the numbers of columns of the matrix
 * @param matrix the input matrix whoose values we want to print to console
 */
void printMatrix(int rows,int cols, double** matrix){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%f ",matrix[i][j]);
        }
        printf("\n");
    }
}
