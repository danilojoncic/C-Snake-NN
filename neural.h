#include "matrix.h"
#include <string.h>

#define FEATURES 9
#define NUMBER_OF_NEURONS 64
#define OUTPUT_NEURONS 4
#define NUMBER_OF_LAYERS 3
#define EPOCHS 10

/**
 * enum for the activation functions that we will use
 */
typedef enum{
    RELU,
    SOFTMAX
}ActivationFunction;

/**
 * enum for the output used for the snake game
 */
//enum za 0, 1, 2, 3
typedef enum{
    UP,
    DOWN,
    LEFT,
    RIGHT
}Prediction;

/**
 * Layer structure representing a layer of the neural network with its weight, biases
 * input and output matrices
 */
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

/**
 * allocates a new layer and returns a pointer to it therefor creating a new not-usable layer whoose weights and biases matrices are null and not initialized
 * @param activationFunction activation function enum that we will use
 * @param numberOfNeurons numbe of neurons the layer will contain
 * @param weightRows number of rows of the weights matrix
 * @param weightCols number of columns of the weights matrix
 * @param biasRows number of rows of the bias matrix
 * @param biasCols number of columns of the bias matrix
 * @param inputRows number of rows that the input matrix for this layer will have
 * @param inputCols number of columns that the input matrix for this layer will have
 * @param outputRows number of rows that the output matrix of this layer will have
 * @param outputCols number of columns that the output matrix of this layer will have
 * @return a pointer to the newly allocated layer
 */
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
/**
 * relu activation function that will apply relu for all values of an input matrix
 * @param matrix a matrix holding the values that we want to modify with relu
 */
void relu(double** matrix){
    for(int i = 0; i < NUMBER_OF_NEURONS;i++){
        if(matrix[0][i] <= 0) matrix[0][i] = 0;
    }
}
/**
 * softmax activation function that will apply softmax for all values of an input matrix, usualy used for the last layer's output values
 * @param matrix a matrix holding values that we want to modify with softmax
 */
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
/**
 * Applies the chosen activation function on the specific matrix of an input layer, usually the layer's output matrix
 * @param layer the input layer whoose matrix we want to modify with the activiation function that we have selected for the layer when the layer was first created
 */
void applyActivationFunction(Layer* layer){
    if(layer->af == RELU){
        relu(layer->output);
    }else if(layer->af == SOFTMAX){
        softmax(layer->output);
    }
}
/**
 * Loads weights from a file and puts them in the selected layer
 * @param layer the layer whoose weights we want to set
 * @param filename the name of the file containing the weights parameters, a csv file
 */
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
/**
 * loads biases for the selected layer with the contents from a csv file
 * @param layer the layer whoose biases matrix we want to populate
 * @param filename name of the .csv file that we want to read
 */
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
/**
 * calculates the sum of all inputs multiplied by the weights with the biases, and then inputs the result into the coresponding activation function to get the result which is contained
 * in the output matrix of the given layer
 * @param layer the layer whoose weigths,biases and input matrices will be used to do the calculation
 */
 //i need to have input rows, input columns because of the first layer where the input can be fun
void formOutputForLayer(Layer* layer,int inputRows,int inputColumns,int weightRows,int weightColumns){
    if(inputColumns != weightRows)printf("CORDINATE MISMATCH formOutputForLayer!\n");
    layer->output = matrixAddition(
            1,layer->numberOfNeurons,matrixMultiplication
                    (inputRows,inputColumns,weightRows,weightColumns,layer->input,layer->weights),layer->biases);
    applyActivationFunction(layer);

}
/**
 * Goes thru the outputLayer's output matrix that contains the final calculations of the neural network and selects the index of the one neuron i.e. column
 * that has the maximum value and returns it
 * @param outputLayer the layer whoose output matrix will be used
 * @return preditcion enum that based on the index of the maximum value neuron
 */
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
/**
 * Frees the allocated memory of a single layer and all the matrices that the layer has.
 * @param layer the layer that we dont want to use anymore and want to free
 * @param layerCount the index of the layer
 */
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

    free(layer);
    printf("Layer %d structure freed.\n",layerCount+1);
}


//game state je niz od 9 vrijednosti
/**
 *takes in an input matrix and
 */
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

/**
 * takes in the relevant neural network layers and initializes their weights and biases with parameters in the csv files
 */
void initNN(Layer** firstLayer, Layer** secondLayer,Layer** thirdLayer,Layer** outputLayer){
    *firstLayer = createLayer(RELU,NUMBER_OF_NEURONS,FEATURES,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,FEATURES,1,NUMBER_OF_NEURONS);
    (*firstLayer)->weights = allocateMatrix((*firstLayer)->weightRows,(*firstLayer)->weigthCols);
    (*firstLayer)->biases = allocateMatrix((*firstLayer)->biasRows,(*firstLayer)->biasCols);

    loadWeights(*firstLayer,"layer_0_weights (1).csv");
    loadBiases(*firstLayer,"layer_0_biases (1).csv");


    *secondLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS);
    (*secondLayer)->weights = allocateMatrix((*secondLayer)->weightRows,(*secondLayer)->weigthCols);
    (*secondLayer)->biases = allocateMatrix((*secondLayer)->biasRows,(*secondLayer)->biasCols);

    loadWeights(*secondLayer,"layer_1_weights (1).csv");
    loadBiases(*secondLayer,"layer_1_biases (1).csv");

    *thirdLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS);
    (*thirdLayer)->weights = allocateMatrix((*thirdLayer)->weightRows,(*thirdLayer)->weigthCols);
    (*thirdLayer)->biases = allocateMatrix((*thirdLayer)->biasRows,(*thirdLayer)->biasCols);

    loadWeights(*thirdLayer,"layer_2_weights (1).csv");
    loadBiases(*thirdLayer,"layer_2_biases (1).csv");

    *outputLayer = createLayer(SOFTMAX,OUTPUT_NEURONS,NUMBER_OF_NEURONS,OUTPUT_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,OUTPUT_NEURONS);
    (*outputLayer)->weights = allocateMatrix((*outputLayer)->weightRows,(*outputLayer)->weigthCols);
    (*outputLayer)->biases = allocateMatrix((*outputLayer)->biasRows,(*outputLayer)->biasCols);

    loadWeights(*outputLayer,"layer_3_weights (1).csv");
    loadBiases(*outputLayer,"layer_3_biases (1).csv");
}

/**
 * takes in the relevant neural network layers and initializes their weights and biases matrices by setting their values as random doubles
 */
void initNNwithRand(Layer** firstLayer, Layer** secondLayer,Layer** thirdLayer,Layer** outputLayer){
    *firstLayer = createLayer(RELU,NUMBER_OF_NEURONS,FEATURES,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,FEATURES,1,NUMBER_OF_NEURONS);
    (*firstLayer)->weights = allocateMatrix((*firstLayer)->weightRows,(*firstLayer)->weigthCols);
    (*firstLayer)->biases = allocateMatrix((*firstLayer)->biasRows,(*firstLayer)->biasCols);

    setRandomValues((*firstLayer)->weightRows,(*firstLayer)->weigthCols,(*firstLayer)->weights,0,2);
    setRandomValues((*firstLayer)->biasRows,(*firstLayer)->biasCols,(*firstLayer)->biases,0,2);

    *secondLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS);
    (*secondLayer)->weights = allocateMatrix((*secondLayer)->weightRows,(*secondLayer)->weigthCols);
    (*secondLayer)->biases = allocateMatrix((*secondLayer)->biasRows,(*secondLayer)->biasCols);

    setRandomValues((*secondLayer)->weightRows,(*secondLayer)->weigthCols,(*secondLayer)->weights,0,2);
    setRandomValues((*secondLayer)->biasRows,(*secondLayer)->biasCols,(*secondLayer)->biases,0,2);

    *thirdLayer = createLayer(RELU,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS);
    (*thirdLayer)->weights = allocateMatrix((*thirdLayer)->weightRows,(*thirdLayer)->weigthCols);
    (*thirdLayer)->biases = allocateMatrix((*thirdLayer)->biasRows,(*thirdLayer)->biasCols);

    setRandomValues((*thirdLayer)->weightRows,(*thirdLayer)->weigthCols,(*thirdLayer)->weights,0,2);
    setRandomValues((*thirdLayer)->biasRows,(*thirdLayer)->biasCols,(*thirdLayer)->biases,0,2);

    *outputLayer = createLayer(SOFTMAX,OUTPUT_NEURONS,NUMBER_OF_NEURONS,OUTPUT_NEURONS,1,NUMBER_OF_NEURONS,1,NUMBER_OF_NEURONS,1,OUTPUT_NEURONS);
    (*outputLayer)->weights = allocateMatrix((*outputLayer)->weightRows,(*outputLayer)->weigthCols);
    (*outputLayer)->biases = allocateMatrix((*outputLayer)->biasRows,(*outputLayer)->biasCols);

    setRandomValues((*outputLayer)->weightRows,(*outputLayer)->weigthCols,(*outputLayer)->weights,0,2);
    setRandomValues((*outputLayer)->biasRows,(*outputLayer)->biasCols,(*outputLayer)->biases,0,2);
}



//double gameState[] = {0,7,8,9,8,9,1,8,23};
/**
 * Takes in the relevant layers, and the input data as separate doubles, runs the neural network i.e does inference to find the calculated best choice of action.
 * @return a number that presents the new direction that the snake should take
 */
int predict(Layer* layer1,Layer* layer2,Layer* layer3, Layer* layerOutput,double score, double headX, double headY, double appleX, double appleY, double dstDup, double dstDdown, double dstDleft, double dstDright){
    double** inputData = allocateMatrix(1,FEATURES);
    formatInput(inputData,score,headX,headY,appleX,appleY,dstDup,dstDdown,dstDleft,dstDright);

    layer1->input = inputData;
    formOutputForLayer(layer1,1,layer1->inputCols,layer1->weightRows,layer1->weigthCols);
    printMatrix(layer1->outputRows,layer1->outputCols,layer1->output);

    layer2->input = layer1->output;
    formOutputForLayer(layer2,1,layer2->inputCols,layer2->weightRows,layer2->weigthCols);

    printMatrix(layer2->outputRows,layer2->outputCols,layer2->output);

    layer3->input = layer2->output;
    formOutputForLayer(layer3,1,layer3->inputCols,layer3->weightRows,layer3->weigthCols);

    layerOutput->input = layer3->output;
    printMatrix(layerOutput->inputRows,layerOutput->inputCols,layerOutput->input);

    formOutputForLayer(layerOutput,1,layerOutput->inputCols,layerOutput->weightRows,layerOutput->weigthCols);

    printf("OUTPUT OUTPUT\n");
    printMatrix(layerOutput->outputRows,layerOutput->outputCols,layerOutput->output);

    int direction = directionBasedOnOutput(layerOutput);
    freeMatrix(1,layer1->input);
    freeMatrix(1,layer2->input);
    freeMatrix(1,layer3->input);
    freeMatrix(1,layerOutput->input);
    freeMatrix(1,layerOutput->output);

    return direction;
}

