#include "matrix.h"
#include <string.h>

#define FEATURES 9
#define NUMBER_OF_NEURONS 32
#define OUTPUT_NEURONS 4
#define NUMBER_OF_LAYERS 3
#define EPOCHS 100
#define LEARINING 0.01

typedef enum{
    RELU,
    SOFTMAX
}ActivationFunction;

//enum za 0, 1, 2, 3
typedef enum{
    UP,
    DOWN,
    LEFT,
    RIGHT
}Prediction;

typedef struct Layer{
    ActivationFunction af;
    double** weights;
    double** biases;
    double** input;
    double** output;
    int numberOfNeurons;
    int weightRows;
    int weigthCols;
    int biasRows;
    int biasCols;
    int inputRows;
    int inputCols;
    int outputRows;
    int outputCols;
}Layer;

Layer* createLayer(ActivationFunction activationFunction, int numberOfNeurons,int weightRows,int weightCols,int biasRows,int biasCols,int inputRows,int inputCols,int outputRows,int outputCols){
    Layer* layer = (Layer*)malloc(sizeof(Layer));
    layer->af = activationFunction;
    layer->numberOfNeurons = numberOfNeurons;
    layer->weightRows = weightRows;
    layer->weigthCols = weightCols;
    layer->biasRows = biasRows;
    layer->biasCols = biasCols;
    layer->inputRows = inputRows;
    layer->inputCols = inputCols;
    layer->outputRows = outputRows;
    layer->outputCols = outputCols;
    layer->weights = NULL;
    layer->biases = NULL;
    layer->input = NULL;
    layer->output = NULL;
    return layer;
}

void relu(double** matrix){
    for(int i = 0; i < NUMBER_OF_NEURONS;i++){
        if(matrix[0][i] <= 0) matrix[0][i] = 0;
    }
}

void softmax(double** matrix){
    double sum = 0;
    double max_val = matrix[0][0];

    for (int i = 1; i < OUTPUT_NEURONS; i++) {
        if (matrix[0][i] > max_val) {
            max_val = matrix[0][i];
        }
    }

    for (int i = 0; i < OUTPUT_NEURONS; i++) {
        matrix[0][i] = exp(matrix[0][i] - max_val);
        sum += matrix[0][i];
    }

    for (int i = 0; i < OUTPUT_NEURONS; i++) {
        matrix[0][i] /= sum;
    }
}

void applyActivationFunction(Layer* layer){
    if(layer->af == RELU){
        relu(layer->output);
    }else if(layer->af == SOFTMAX){
        softmax(layer->output);
    }
}

void loadWeights(Layer* layer, char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[10000];
    int indexI = 0;
    while (fgets(buffer, sizeof(buffer), fp) && indexI < layer->numberOfNeurons) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        char* token = NULL;
        int indexJ = 0;
        token = strtok(buffer, ",");
        while (token != NULL) {
            if (indexJ < layer->numberOfNeurons) {
                if (strlen(token) > 0) {
                    double value = strtod(token, NULL);
                    layer->weights[indexI][indexJ] = value;
                } else {
                    layer->weights[indexI][indexJ] = 0.0f;
                }
                indexJ++;
            }
            token = strtok(NULL, ",");
        }

        indexI++;

    }

    fclose(fp);
}

void loadBiases(Layer* layer, char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[10000];

    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        char* token = strtok(buffer, ",");
        int index = 0;
        while (token != NULL) {
            if (index < layer->numberOfNeurons) {
                if (strlen(token) > 0) {
                    double value = strtod(token, NULL);
                    layer->biases[0][index] = value;
                } else {
                    layer->biases[0][index] = 0.0f;
                }
                index++;
            }
            token = strtok(NULL, ",");
        }
        if (index != layer->numberOfNeurons) {
            fprintf(stderr, "Warning: Number of parsed values does not match the number of neurons.\n");
        }
    }

    fclose(fp);
}

void formOutputForLayer(Layer* layer,int inputRows,int inputColumns,int weightRows,int weightColumns){
    if(inputColumns != weightRows)printf("CORDINATE MISMATCH formOutputForLayer!\n");
    layer->output = matrixAddition(
            1,layer->numberOfNeurons,matrixMultiplication
                    (inputRows,inputColumns,weightRows,weightColumns,layer->input,layer->weights),layer->biases);
//    for(int i = 0; i < layer->numberOfNeurons; i++){
//        printf("BEFORE ACTIVATION NEURON %d , OUTPUT : %f\n",i,layer->output[0][i]);
//    }
    applyActivationFunction(layer);
    //printf("ACTIVATION\n");
//    for(int i = 0; i < layer->numberOfNeurons; i++){
//        printf("AFTER ACTIVATION NEURON %d , OUTPUT : %f\n",i,layer->output[0][i]);
//    }
}

Prediction directionBasedOnOutput(Layer* outputLayer){
    int indexOfMaxValue = 0;
    double maxValue = 0;
    for(int i = 0; i < outputLayer->numberOfNeurons;i++){
        if(outputLayer->output[0][i] >= maxValue){
            maxValue = outputLayer->output[0][i];
            indexOfMaxValue = i;
        }
    }
    return indexOfMaxValue;
}

void freeLayer(Layer* layer, int weightRows,int layerCount) {
    if (layer == NULL) return;

    if (layer->weights != NULL) {
        freeMatrix(weightRows, layer->weights);
        layer->weights = NULL;
    }

    if (layer->biases != NULL) {
        freeMatrix(1, layer->biases);
        layer->biases = NULL;
    }

//    if(layerCount == 0) {
//        if (layer->input != NULL) {
//            freeMatrix(1, layer->input);
//            layer->input = NULL;
//        }
//    }
//
//    if (layer->output != NULL) {
//        freeMatrix(1, layer->output);
//        layer->output = NULL;
//    }

    free(layer);
    printf("Layer %d structure freed.\n",layerCount+1);
}


//game state je niz od 9 vrijednosti
void formatInput(double** inputMatrix,double score, double headX, double headY, double appleX, double appleY, double dstDup, double dstDdown, double dstDleft, double dstDright){
    inputMatrix[0][0] = score;
    inputMatrix[0][1] = headX;
    inputMatrix[0][2] = headY;
    inputMatrix[0][3] = appleX;
    inputMatrix[0][4] = appleY;
    inputMatrix[0][5] = dstDup;
    inputMatrix[0][6] = dstDdown;
    inputMatrix[0][7] = dstDleft;
    inputMatrix[0][8] = dstDright;
}

void initNN(Layer** firstLayer, Layer** secondLayer,Layer** outputLayer){
    *firstLayer = createLayer(RELU,NUMBER_OF_NEURONS,FEATURES,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,FEATURES,1,NUMBER_OF_NEURONS);
    (*firstLayer)->weights = allocateMatrix((*firstLayer)->weightRows,(*firstLayer)->weigthCols);
    (*firstLayer)->biases = allocateMatrix((*firstLayer)->biasRows,(*firstLayer)->biasCols);

    loadWeights(*firstLayer,"layer_0_weights.csv");
    loadBiases(*firstLayer,"layer_0_biases.csv");


    *secondLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS);
    (*secondLayer)->weights = allocateMatrix((*secondLayer)->weightRows,(*secondLayer)->weigthCols);
    (*secondLayer)->biases = allocateMatrix((*secondLayer)->biasRows,(*secondLayer)->biasCols);

    loadWeights(*secondLayer,"layer_1_weights.csv");
    loadBiases(*secondLayer,"layer_1_biases.csv");

    *outputLayer = createLayer(SOFTMAX,OUTPUT_NEURONS,NUMBER_OF_NEURONS,OUTPUT_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,OUTPUT_NEURONS);
    (*outputLayer)->weights = allocateMatrix((*outputLayer)->weightRows,(*outputLayer)->weigthCols);
    (*outputLayer)->biases = allocateMatrix((*outputLayer)->biasRows,(*outputLayer)->biasCols);

    loadWeights(*outputLayer,"layer_2_weights.csv");
    loadBiases(*outputLayer,"layer_2_biases.csv");
}

//double gameState[] = {0,7,8,9,8,9,1,8,23};

int predict(Layer* layer1,Layer* layer2, Layer* layerOutput,double score, double headX, double headY, double appleX, double appleY, double dstDup, double dstDdown, double dstDleft, double dstDright){
    double** inputData = allocateMatrix(1,FEATURES);
    formatInput(inputData,score,headX,headY,appleX,appleY,dstDup,dstDdown,dstDleft,dstDright);

    layer1->input = inputData;
    formOutputForLayer(layer1,1,layer1->inputCols,layer1->weightRows,layer1->weigthCols);
    printf("OUTPUT layer 1 \n");
    printMatrix(layer1->outputRows,layer1->outputCols,layer1->output);

    layer2->input = layer1->output;
    formOutputForLayer(layer2,1,layer2->inputCols,layer2->weightRows,layer2->weigthCols);

    printf("OUTPUT layer 2 \n");

    printMatrix(layer2->outputRows,layer2->outputCols,layer2->output);


    layerOutput->input = layer2->output;
    printf("OUTPUT LAYER INPUT\n");
    printMatrix(layerOutput->inputRows,layerOutput->inputCols,layerOutput->input);

    formOutputForLayer(layerOutput,1,layerOutput->inputCols,layerOutput->weightRows,layerOutput->weigthCols);

    printf("OUTPUT OUTPUT\n");
    printMatrix(layerOutput->outputRows,layerOutput->outputCols,layerOutput->output);

    int direction = directionBasedOnOutput(layerOutput);
    freeMatrix(1,layer1->input);
    freeMatrix(1,layer2->input);
    freeMatrix(1,layerOutput->input);
    freeMatrix(1,layerOutput->output);

    return direction;
}




//int main() {
//
//    Layer* firstLayer = NULL;
//    Layer* secondLayer = NULL;
//    Layer* outputLayer = NULL;
//    initNN(&firstLayer,&secondLayer,&outputLayer);
//    predict(firstLayer,secondLayer,outputLayer,0,7,8,9,8,9,1,8,23);
//    freeLayer(firstLayer,FEATURES,0);
//    freeLayer(secondLayer,NUMBER_OF_NEURONS,1);
//    freeLayer(outputLayer,NUMBER_OF_NEURONS,2);
//
//


//    double** inputData = allocateMatrix(1,FEATURES);
//    formatInput(inputData,0,7,8,9,8,9,1,8,23);
//
//    Layer* firstLayer = createLayer(RELU,NUMBER_OF_NEURONS,FEATURES);
//    firstLayer->input = inputData;
//    firstLayer->weights = allocateMatrix(FEATURES,NUMBER_OF_NEURONS);
//    firstLayer->biases = allocateMatrix(1,NUMBER_OF_NEURONS);
//
//    loadWeights(firstLayer,"layer_0_weights.csv");
//    loadBiases(firstLayer,"layer_0_biases.csv");
//    printf("ACIVATING FIRST LAYER\n");
//    formOutputForLayer(firstLayer,1,FEATURES,FEATURES,NUMBER_OF_NEURONS);
//
//
//    Layer* secondLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS);
//    secondLayer->input = firstLayer->output;
//    secondLayer->weights = allocateMatrix(NUMBER_OF_NEURONS,NUMBER_OF_NEURONS);
//    secondLayer->biases = allocateMatrix(1,NUMBER_OF_NEURONS);
//
//    loadWeights(secondLayer,"layer_1_weights.csv");
//    loadBiases(secondLayer,"layer_1_biases.csv");
//
//    printf("ACIVATING SECOND LAYER\n");
//    formOutputForLayer(secondLayer,1,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS);
//
//
//
//    Layer* outputLayer = createLayer(SOFTMAX,OUTPUT_NEURONS,NUMBER_OF_NEURONS);
//    outputLayer->input = secondLayer->output;
//    outputLayer->weights = allocateMatrix(NUMBER_OF_NEURONS,OUTPUT_NEURONS);
//    outputLayer->biases = allocateMatrix(1,OUTPUT_NEURONS);
//
//    loadWeights(outputLayer,"layer_2_weights.csv");
//    loadBiases(outputLayer,"layer_2_biases.csv");
//
//    printf("ACIVATING OUTPUT LAYER\n");
//    formOutputForLayer(outputLayer,1,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,OUTPUT_NEURONS);
//
//    int direction = directionBasedOnOutput(outputLayer);
//    printf("Direction: %d\n",direction);

//    freeLayer(firstLayer,FEATURES,0);
//    freeLayer(secondLayer,NUMBER_OF_NEURONS,1);
//    freeLayer(outputLayer,NUMBER_OF_NEURONS,2);


//    return 0;
//}
