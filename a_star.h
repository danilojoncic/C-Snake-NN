#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define OBSTACLE (-1)
#define GO_UP 4
#define GO_DOWN 5
#define GO_LEFT 6
#define GO_RIGHT 7


//the a star algorithm

#define INF 99999
/**
 * a struct represent a grid cell on the snake game board
 */
typedef struct Node{
    int x;
    int y;
    int costFromStart; //g
    int heuristicCostToGoal; //h
    int totalCost; //f-> f = g + h
    struct Node* parentNode; //used for the path tree
}Node;

/**
 * calculates the heuristic value by calculating the Manhattan distance between two points, or in our case two nodes
 * @param x1 x cordinate of node1
 * @param y1 y cordinate of node1
 * @param x2 x cordinate of node2
 * @param y2 y cordinate of node2
 * @return int value of heurstics
 */
int heuristic(int x1, int y1, int x2, int y2){
    return abs(x1-x2) + abs(y1-y2);
}

/**
 * checks if the inputed cordinates are the cordinates of the destination node
 * @return 1 for true, 0 for false for the condition if the cordinates are the ones of the destaintion node
 */
int isDestination(int x, int y,Node* destinationNode){
    return (x == destinationNode->x && y == destinationNode->y);
}

/**
 *checks if the input x and y are parts of the grid and if the x and y are walkable
 * @return 1 for true and 0 for false
 */
int isValid(int gridSize,int** grid, int x, int y){
    return ((x >= 0 && x < gridSize) && (y >= 0 && y < gridSize) && (grid[x][y] == 0));
}

/**
 * creates a new Node based on the input parameters
 * @return a pointer to a newly allocated Node
 */
Node* createNode(int x, int y, int g, int h, Node* parent){
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->x = x;
    newNode->y = y;
    newNode->costFromStart = g;
    newNode->heuristicCostToGoal = h;
    newNode->totalCost = g + h;
    newNode->parentNode = parent;
    return newNode;
}

void freeEverything(Node* start, Node* end, Node** openList, int openListSize) {
    Node* current = end;
    while (current) {
        Node* temp = current;
        current = current->parentNode;
        free(temp);
    }

    // Free remaining nodes in the open list
//    for (int i = 0; i < openListSize-1; i++) {
//        if (openList[i]) {
//            free(openList[i]);
//        }
//    }

//    if (start != end) {
//        free(start);
//    }
}

void freeNode(Node* node){
    free(node);
}
/**
 *
 * @param grid
 * @param gridSize
 * @param start
 * @param dest
 * @return 1 if path is found, and 0 if a path can not be found
 */
int aStar(int** grid, int gridSize, Node* start, Node* dest) {
    int** closedList = (int**)malloc(gridSize * sizeof(int*));
    for (int i = 0; i < gridSize; i++) {
        closedList[i] = (int*)calloc(gridSize, sizeof(int));
    }

    Node* openList[gridSize * gridSize];
    int openListSize = 0;

    openList[openListSize++] = start;

    while (openListSize > 0) {
        // Find the node with the least f value
        int minIndex = 0;
        for (int i = 1; i < openListSize; i++) {
            if (openList[i]->totalCost < openList[minIndex]->totalCost) {
                minIndex = i;
            }
        }
        Node* currentNode = openList[minIndex];
        //if we find a node that is the destination i.e. we found a way
        if (isDestination(currentNode->x, currentNode->y, dest)) {
            printf("Path found!\n");
            Node* path = currentNode;
            while (path) {
                grid[path->x][path->y] = 9;
                path = path->parentNode;
            }
            printf("\n");
            freeEverything(start, currentNode, openList, openListSize);

            for (int i = 0; i < gridSize; i++) {
                free(closedList[i]);
            }
            free(closedList);
            return 1;
        }

        for (int i = minIndex; i < openListSize - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openListSize--;
        closedList[currentNode->x][currentNode->y] = 1;

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; i++) {
            int newX = currentNode->x + dx[i];
            int newY = currentNode->y + dy[i];

            if (isValid(gridSize, grid, newX, newY) && !closedList[newX][newY]) {
                int newG = currentNode->costFromStart + 1;
                int newH = heuristic(newX, newY, dest->x, dest->y);
                int newF = newG + newH;

                Node* neighbor = NULL;
                for (int j = 0; j < openListSize; j++) {
                    if (openList[j]->x == newX && openList[j]->y == newY) {
                        neighbor = openList[j];
                        break;
                    }
                }

                if (neighbor == NULL) {
                    neighbor = createNode(newX, newY, newG, newH, currentNode);
                    openList[openListSize++] = neighbor;
                } else if (newF < neighbor->totalCost) {
                    neighbor->costFromStart = newG;
                    neighbor->heuristicCostToGoal = newH;
                    neighbor->totalCost = newF;
                    neighbor->parentNode = currentNode;
                }
            }
        }
    }

    printf("Path not found.\n");
    freeEverything(start, NULL, openList, openListSize);

    for (int i = 0; i < gridSize; i++) {
        free(closedList[i]);
    }
    free(closedList);
    return 0;
}

void fillGridWithZero(int **grid, int gridSize) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = 0;
        }
    }
}

int** allocateGrid(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
    }
    return matrix;
}


//int main() {
//    int gridSize = 5;
//    int** grid = (int**)malloc(gridSize * sizeof(int*));
//    for (int i = 0; i < gridSize; i++) {
//        grid[i] = (int*)malloc(gridSize * sizeof(int));
//    }
//
//    int tempGrid[5][5] = {
//            {0, 0, 0, 0, 0},
//            {0, 0, 0, 0, 0},
//            {0, 0, 0, 0, 0},
//            {1, 1, 1, 0, 0},
//            {0, 0, 0, 0, 0}
//    };
//
//    for (int i = 0; i < gridSize; i++) {
//        for (int j = 0; j < gridSize; j++) {
//            grid[i][j] = tempGrid[i][j];
//        }
//    }
//
//    Node* start = createNode(0, 0, 0, heuristic(0, 0, 4, 4), NULL);
//    Node* dest = createNode(4, 4, 0, 0, NULL);
//
//    aStar(grid, 5, start, dest);
//
//
//    for (int i = 0; i < gridSize; i++) {
//        for (int j = 0; j < gridSize; j++) {
//            printf("%d ", grid[i][j]);
//        }
//        printf("\n");
//    }
//
//
//    for (int i = 0; i < gridSize; i++) {
//        free(grid[i]);
//    }
//    free(grid);
//
//
//
//
//
//    return 0;
//}
















