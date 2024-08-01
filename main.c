#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <time.h>
#include "neural.h"
#include "glyph.h"
#include "a_star.h"
#define GRID_SIZE 30
#define GRID_DIM 600
#define DELAY 100


Layer* firstLayer;
Layer* secondLayer;
Layer* thirdLayer;
Layer* outputLayer;

static int highScore = 0;
int localScore = 0;

int *glyphs[10] = {*number0, *number1, *number2, *number3, *number4, *number5, *number6, *number7, *number8, *number9};

typedef enum {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
} Direction;

typedef struct {
    int x;
    int y;
} Apple;
//globalna jabuka
Apple apple;

typedef struct Snake {
    int x;
    int y;
    Direction dir;
    struct Snake* next;
} Snake;

//globalna zmija sa glavom i repom
Snake *head, *tail;

void initSnake() {
    srand(time(NULL));
    Snake *new = (Snake *)malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL;

    head = new;
    tail = new;
}

void increaseSnake() {
    Snake *new = (Snake *)malloc(sizeof(Snake));
    switch (tail->dir) {
        case SNAKE_UP:
            new->x = tail->x;
            new->y = tail->y + 1;
            break;
        case SNAKE_DOWN:
            new->x = tail->x;
            new->y = tail->y - 1;
            break;
        case SNAKE_LEFT:
            new->x = tail->x + 1;
            new->y = tail->y;
            break;
        case SNAKE_RIGHT:
            new->x = tail->x - 1;
            new->y = tail->y;
            break;
    }
    new->next = NULL;
    tail->next = new;
    tail = new;
}

void moveSnake() {
    int prevX = head->x;
    int prevY = head->y;
    int prevDir = head->dir;

    switch (head->dir) {
        case SNAKE_UP:
            head->y--;
            break;
        case SNAKE_DOWN:
            head->y++;
            break;
        case SNAKE_LEFT:
            head->x--;
            break;
        case SNAKE_RIGHT:
            head->x++;
            break;
    }

    Snake *track = head->next;
    while (track != NULL) {
        int saveX = track->x;
        int saveY = track->y;
        int saveDir = track->dir;

        track->x = prevX;
        track->y = prevY;
        track->dir = prevDir;

        prevX = saveX;
        prevY = saveY;
        prevDir = saveDir;

        track = track->next;
    }
}

void renderSnake(SDL_Renderer *renderer, int x, int y) {
    int segSize = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;
    int brightness = 255;
    int bDir = 0;
    seg.w = segSize - 2;
    seg.h = segSize - 2;

    Snake *track = head;
    while (track != NULL) {
        SDL_SetRenderDrawColor(renderer, 0x00, brightness, 0x00, 255);
        seg.x = x + track->x * segSize;
        seg.y = y + track->y * segSize;

        SDL_RenderFillRect(renderer, &seg);
        track = track->next;

        if (bDir == 0) {
            brightness -= 5;
            if (brightness < 150) {
                bDir = 1;
            }
        } else {
            brightness += 5;
            if (brightness > 250) {
                bDir = 0;
            }
        }
    }
}

void renderGrid(SDL_Renderer *renderer, int x, int y,int** backupGrid, bool printPath) {
    int cellSize = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cellSize;
    cell.h = cellSize;

    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(printPath && backupGrid[i][j] == 9){
                SDL_SetRenderDrawColor(renderer, 0xeb, 0xda, 0x21, 255);//zuto
            }else{
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
            }
            cell.x = x + (i * cellSize);
            cell.y = y + (j * cellSize);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}

void renderDebugInfo(SDL_Renderer *renderer, int x, int y) {
    int cellSize = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cellSize;
    cell.h = cellSize;
    SDL_SetRenderDrawColor(renderer, 0xbf, 0x45, 0xbd, 255); // roze

    int headX = head->x;
    int headY = head->y;

    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    for (int i = 0; i < 4; i++) {
        int deltaX = directions[i][0];
        int deltaY = directions[i][1];
        int nextX = headX;
        int nextY = headY;

        while (true) {
            nextX += deltaX;
            nextY += deltaY;

            if (nextX < 0 || nextX >= GRID_SIZE || nextY < 0 || nextY >= GRID_SIZE) {
                break;
            }

            Snake *track = head->next;
            bool collision = false;
            while (track != NULL) {
                if (track->x == nextX && track->y == nextY) {
                    collision = true;
                    break;
                }
                track = track->next;
            }

            if (collision) {
                break;
            }
            cell.x = x + (nextX * cellSize);
            cell.y = y + (nextY * cellSize);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}



void generateApple(int** backupGrid, bool printPath) {
    bool in_snake;

    do {
        in_snake = false;
        apple.x = rand() % GRID_SIZE;
        apple.y = rand() % GRID_SIZE;

        Snake *track = head;
        while (track != NULL) {
            if (apple.x == track->x && apple.y == track->y) {
                in_snake = true;
            }
            track = track->next;
        }
    } while (in_snake);
    if(printPath){
        fillGridWithZero(backupGrid,GRID_SIZE);
        Snake *track = head->next;
        while (track != NULL) {
            backupGrid[track->x][track->y] = OBSTACLE;
            track = track->next;
        }
        track = NULL;
        Node* start = createNode(head->x, head->y, 0, heuristic(0, 0, apple.x, apple.y), NULL);
        Node* dest = createNode(apple.x,apple.y, 0, 0, NULL);
        aStar(backupGrid,GRID_SIZE, start,dest);
    }
}

void renderApple(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 255);

    int appleSize = GRID_DIM / GRID_SIZE;
    SDL_Rect app;
    app.w = appleSize;
    app.h = appleSize;
    app.x = x + apple.x * appleSize;
    app.y = y + apple.y * appleSize;
    SDL_RenderFillRect(renderer, &app);
}

void putSnake() {
    initSnake();
    increaseSnake();
    increaseSnake();
    increaseSnake();
}

void resetSnake() {
    Snake *track = head;
    Snake *temp;

    while (track != NULL) {
        temp = track;
        track = track->next;
        free(temp);
    }
    putSnake();
    localScore = 0;
    //printf("LocalScore = %d, HighScore = %d\n", localScore, highScore);
}

void detectApple(int** backupGrid, bool printPath) {
    if (head->x == apple.x && head->y == apple.y) {
        generateApple(backupGrid,printPath);
        increaseSnake();
        localScore++;
        if (localScore >= highScore) {
            highScore = localScore;
        }
        //printf("LocalScore = %d, HighScore = %d\n", localScore, highScore);
    }
}

void detectCrash() {
    if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 || head->y >= GRID_SIZE) {
        resetSnake();
    }

    Snake *track = head->next;
    while (track != NULL) {
        if (track->x == head->x && track->y == head->y) {
            resetSnake();
        }
        track = track->next;
    }
}

void turnLeft() {
    switch (head->dir) {
        case SNAKE_UP:
            head->dir = SNAKE_LEFT;
            break;
        case SNAKE_DOWN:
            head->dir = SNAKE_RIGHT;
            break;
        case SNAKE_LEFT:
            head->dir = SNAKE_DOWN;
            break;
        case SNAKE_RIGHT:
            head->dir = SNAKE_UP;
            break;
    }
}

void turnRight() {
    switch (head->dir) {
        case SNAKE_UP:
            head->dir = SNAKE_RIGHT;
            break;
        case SNAKE_DOWN:
            head->dir = SNAKE_LEFT;
            break;
        case SNAKE_LEFT:
            head->dir = SNAKE_UP;
            break;
        case SNAKE_RIGHT:
            head->dir = SNAKE_DOWN;
            break;
    }
}

typedef enum {
    TRY_FORWARD,
    TRY_LEFT,
    TRY_RIGHT,
} TryDirection;

int state(TryDirection try) {
    int reward = 0;
    int tryX = head->x;
    int tryY = head->y;
    switch (head->dir) {
        case SNAKE_UP:
            switch (try) {
                case TRY_FORWARD:
                    tryY--;
                    break;
                case TRY_LEFT:
                    tryX--;
                    break;
                case TRY_RIGHT:
                    tryX++;
                    break;
            }
            break;
        case SNAKE_DOWN:
            switch (try) {
                case TRY_FORWARD:
                    tryY++;
                    break;
                case TRY_LEFT:
                    tryX++;
                    break;
                case TRY_RIGHT:
                    tryX--;
                    break;
            }
            break;
        case SNAKE_LEFT:
            switch (try) {
                case TRY_FORWARD:
                    tryX--;
                    break;
                case TRY_LEFT:
                    tryY++;
                    break;
                case TRY_RIGHT:
                    tryY--;
                    break;
            }
            break;
        case SNAKE_RIGHT:
            switch (try) {
                case TRY_FORWARD:
                    tryX++;
                    break;
                case TRY_LEFT:
                    tryY--;
                    break;
                case TRY_RIGHT:
                    tryY++;
                    break;
            }
            break;
    }

    if (tryX < 0 || tryX >= GRID_SIZE || tryY < 0 || tryY >= GRID_SIZE) {
        reward -= 100;
    }

    if (tryX == apple.x && tryY == apple.y) {
        reward += 100;
    }

    int diffX = abs(head->x - apple.x);
    int diffY = abs(head->y - apple.y);
    int tryDiffX = abs(tryX - apple.x);
    int tryDiffY = abs(tryY - apple.y);

    if (tryDiffX < diffX) {
        reward += 5;
    }

    if (tryDiffY < diffY) {
        reward += 5;
    }

    Snake *track = head->next;
    while (track != NULL) {
        if (tryX == track->x && tryY == track->y) {
            reward -= 100;
        }
        track = track->next;
    }

    return reward;
}

void renderGlyph(SDL_Renderer *renderer, int x, int y, int glyph[6][6],int scheme) {
    int cellSize = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cellSize;
    cell.h = cellSize;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (glyph[i][j] == 1) {
                if(scheme == 0){
                    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);//bijelo
                }else if(scheme == 1){
                    SDL_SetRenderDrawColor(renderer, 0xbf, 0x45, 0xbd, 255);//roze
                }else if(scheme == 2){
                    SDL_SetRenderDrawColor(renderer, 0xeb, 0xda, 0x21, 255);//zuto
                }
                cell.x = x + (j * cellSize);
                cell.y = y + (i * cellSize);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
}

void renderScore(SDL_Renderer *renderer) {
    int score = localScore;
    int xOffset = 0;

    if (score == 0) {
        renderGlyph(renderer, 90, 25, glyphs[0],0);
    } else {
        while (score > 0) {
            int digit = score % 10;
            score /= 10;
            renderGlyph(renderer, 90 + xOffset, 25, glyphs[digit],0);
            xOffset -= 80;
        }
    }
    int hs = highScore;
    int x2Offset = 0;
    if (hs == 0) {
        renderGlyph(renderer, 90, 150, glyphs[0],2);
    } else {
        while (hs > 0) {
            int digit = hs % 10;
            hs /= 10;
            renderGlyph(renderer, 90 + x2Offset, 150, glyphs[digit],2);
            x2Offset -= 80;
        }
    }


}
int calculateDistanceToDanger(Direction direction) {
    int distance = 0;
    int currentX = head->x;
    int currentY = head->y;
    int dx = 0, dy = 0;

    switch (direction) {
        case SNAKE_UP:    dy = -1; break;
        case SNAKE_DOWN:  dy = 1;  break;
        case SNAKE_LEFT:  dx = -1; break;
        case SNAKE_RIGHT: dx = 1;  break;
    }

    while (true) {
        distance++;
        currentX += dx;
        currentY += dy;

        if (currentX < 0 || currentX >= GRID_SIZE || currentY < 0 || currentY >= GRID_SIZE) {
            return distance;
        }

        for (Snake* track = head->next; track != NULL; track = track->next) {
            if (track->x == currentX && track->y == currentY) {
                return distance;
            }
        }
    }
}


FILE* fp = NULL;

void fileLoggerInit(){
    fp = fopen("choices.csv","a");
    fprintf(fp,"\"score\",\"headX\",\"headY\",\"appleX\",\"appleY\",\"distance to danger up\",\"distance to danger down\",\" distance to danger left\",\"distance to danger right\",\"choice of direction\"\n");
    //fclose(fp);
}

void logFile(){
    //fp = fopen("choices.csv","a");//append mode
    int dangerUp = calculateDistanceToDanger(SNAKE_UP);
    int dangerDown = calculateDistanceToDanger(SNAKE_DOWN);
    int dangerLeft = calculateDistanceToDanger(SNAKE_LEFT);
    int dangerRight = calculateDistanceToDanger(SNAKE_RIGHT);
    fprintf(fp,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", localScore,head->x, head->y, apple.x, apple.y,dangerUp,dangerDown,dangerLeft,dangerRight,head->dir);
}
void closeLog(){
    fclose(fp);
    fp = NULL;
}

void ai() {
    int tryForward = state(TRY_FORWARD);
    int tryLeft = state(TRY_LEFT);
    int tryRight = state(TRY_RIGHT);

    if (tryForward >= tryLeft && tryForward >= tryRight) {
        // Continue forward
    } else if (tryLeft > tryRight) {
        logFile();
        turnLeft();
        logFile();
    } else {
        logFile();
        turnRight();
        logFile();
    }
}


void nn() {
    int score = localScore;
    int headX = head->x;
    int headY = head->y;
    int appleX = apple.x;
    int appleY = apple.y;
    int dstUp = calculateDistanceToDanger(SNAKE_UP);
    int dstDown = calculateDistanceToDanger(SNAKE_DOWN);
    int dstLeft = calculateDistanceToDanger(SNAKE_LEFT);
    int dstRight = calculateDistanceToDanger(SNAKE_RIGHT);
    int currentDirection = head->dir;

    int choice = predict(firstLayer, secondLayer,thirdLayer, outputLayer, score, headX, headY, appleX, appleY, dstUp, dstDown, dstLeft, dstRight);

    void (*turnFunction)(void) = NULL;
    switch (currentDirection) {
        case SNAKE_UP:
            if (choice == SNAKE_RIGHT) turnFunction = turnRight;
            else if (choice == SNAKE_LEFT) turnFunction = turnLeft;
            break;
        case SNAKE_DOWN:
            if (choice == SNAKE_RIGHT) turnFunction = turnRight;
            else if (choice == SNAKE_LEFT) turnFunction = turnLeft;
            break;
        case SNAKE_LEFT:
            if (choice == SNAKE_UP) turnFunction = turnRight;
            else if (choice == SNAKE_DOWN) turnFunction = turnLeft;
            break;
        case SNAKE_RIGHT:
            if (choice == SNAKE_DOWN) turnFunction = turnRight;
            else if (choice == SNAKE_UP) turnFunction = turnLeft;
            break;
    }

    if (turnFunction) turnFunction();
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR SDL INIT VIDEO\n");
        return 1;
    }

    firstLayer = NULL;
    secondLayer = NULL;
    thirdLayer = NULL;
    outputLayer = NULL;

    int** grid = allocateGrid(GRID_SIZE,GRID_SIZE);
    fillGridWithZero(grid,GRID_SIZE);
    bool printPath = false;


    initNN(&firstLayer,&secondLayer,&thirdLayer,&outputLayer);


    fileLoggerInit();
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int screenWidth = DM.w;
    int screenHeight = DM.h;
    int windowWidth = screenWidth * 0.75;
    int windowHeight = screenHeight * 0.75;

    SDL_Window *window = SDL_CreateWindow("Snake",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowWidth,
                                          windowHeight,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "ERROR creating window\n");
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "ERROR creating renderer\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int gridX = (windowWidth / 2) - (GRID_DIM / 2);
    int gridY = (windowHeight / 2) - (GRID_DIM / 2);

    putSnake();
    generateApple(grid,printPath);

    bool quit = false;
    bool debugMode = false;
    bool paused = false;
    int decider = 1;

    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_LEFT:
                        turnLeft();
                        break;
                    case SDLK_RIGHT:
                        turnRight();
                        break;
                    case SDLK_d:
                        debugMode = !debugMode;
                        break;
                    case SDLK_p:
                        paused = !paused;
                        break;
                    case SDLK_i:
                        decider++;
                        if(decider > 3)decider %= 4;
                        break;
                }
            }
        }
        SDL_RenderClear(renderer);
        if(!paused){
            //pomjeram zmiju na osnovu smjera
            moveSnake();
            //provjeravam da li je doslo do sudara sa zidom ili tijelom
            detectCrash();
            //provjeravam da li je doslo da sudara sa jabukom
            detectApple(grid,printPath);
        }else{
            renderGlyph(renderer, 900, 25, stopIcon,0);
        }



        //poziv funkcija koje ce da crtaju sadrzaj ekrana
        renderGrid(renderer, gridX, gridY,grid,printPath);
        renderSnake(renderer, gridX, gridY);
        renderApple(renderer, gridX, gridY);
        renderScore(renderer);
        if(debugMode){
            renderDebugInfo(renderer,gridX,gridY);
            renderGlyph(renderer, 1000, 25, debugIcon,1);
        }

        switch (decider) {
            case 0:
                renderGlyph(renderer,900,200,letterP,0);
                break;
            case 1:
                ai();
                renderGlyph(renderer,900,200,letterA,0);
                renderGlyph(renderer,1000,200,letterI,0);
                break;
            case 2:
                nn();
                renderGlyph(renderer,900,200,letterN,0);
                renderGlyph(renderer,1000,200,letterN,0);
                break;
            case 3:
                printPath = true;
                renderGlyph(renderer,900,200,letterA,0);
                renderGlyph(renderer,1000,200,star,0);
                break;



        }

        SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255);
        SDL_RenderPresent(renderer);

        //choices.csv fajl je ispisan sa ~70k redova, nema potrebe za vise od ovoga
        if(decider != 2 ) {
            //logFile();
        }

        SDL_Delay(DELAY);
    }

    freeLayer(firstLayer,FEATURES,0);
    freeLayer(secondLayer,NUMBER_OF_NEURONS,1);
    freeLayer(thirdLayer,NUMBER_OF_NEURONS,2);
    freeLayer(outputLayer,NUMBER_OF_NEURONS,3);


    closeLog();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}