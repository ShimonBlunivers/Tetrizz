#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <sys/time.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#define emptyChar ' '
#define height 24
#define width 16
#define upperEdge 8

bool game = true;

int points = 0;

int gameField[height][width];

int spawningLocation[2] =  {5, 0};

int piece[8]; // Coordinates of the falling piece: xy, xy, xy, xy.

int pieceList[32] = {

    0, 0, // 0 0
    1, 0, // 0 0
    0, 1, // 
    1, 1, // 

    0, 0, // 0 0 0 0
    1, 0, //
    2, 0, //
    3, 0, // 

    0, 0, // 0 0
    1, 0, //   0
    1, 1, //   0
    1, 2, // 

    0, 0, // 0 0
    1, 0, //   0 0
    1, 1, //   
    2, 1, // 

};

char key = '0';

struct timeval timer;

long updateTimer = 2; // secs / 10


void spawnPiece(int pieceIndex) {
    for (int i = 0; i < 8; i++) {
        piece[i] = pieceList[i + pieceIndex * 8];
        piece[i] += spawningLocation[i % 2];
    }
}

void fill() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            gameField[y][x] = 0;
        }
    }
}


bool checkCollision(int y, int x){
    if (y >= height) return true;
    for (int i = 0; i < 4; i++) {
        if (piece[(i*2)+1] == y && piece[i*2] == x) return false;
    }
    return gameField[y][x];
}

bool checkWallCollision(int x){
    return (x < 0 || x >= width);
}
void shiftDown(int positionY){
    points += 100;
    for (int y = positionY; y >= 4; y--) {
        for (int x = 0; x < width; x++) {
            gameField[y][x] = gameField[y-1][x];
        }
    }
}

void endGame(){
    game = false;
    wprintf(L"\n\n\n\n    GAME OVER!\n\n        %d\n\n\n\n", points);
}

void rotatePiece(int direction) { // -1 je protisměru; +1 je ve směru hodinových ručiček
    int set[8];
    int x, y;
    int pivotX = piece[0];
    int pivotY = piece[1];
    for (int i = 0; i < 4; i++) {
        x = piece[2 * i];
        y = piece[2 * i + 1];
        x -= pivotX;
        y -= pivotY;
        if (direction == +1) {
            set[2 * i] = y;
            set[2 * i + 1] = -x;
        }
        else if (direction == -1) {
            set[2 * i] = -y;
            set[2 * i + 1] = x;
        }
        else return;
        
        set[2 * i] += pivotX;
        set[2 * i + 1] += pivotY;
    }
    for (int i = 0; i < 4; i++) 
        if (checkCollision(set[2 * i + 1], set[2 * i]) || checkWallCollision(set[2 * i]) || set[2 * i + 1] < 0 || set[2 * i + 1] > height) return;

    for (int i = 0; i < 4; i++) 
        gameField[piece[(i*2)+1]][piece[i*2]] = 0;

    for (int i = 0; i < 4; i++) {
        piece[2 * i] = set[2 * i];
        piece[2 * i + 1] = set[2 * i + 1];

        gameField[piece[(i*2)+1]][piece[i*2]] = 1;
    }
}

void checkForFullRows(){
    bool hasGap;
    for (int y = 0; y < height; y++) {
        hasGap = false;
        for (int x = 0; x < width; x++) {
            if (gameField[y][x] == 0){
                hasGap = true;
                break;
            }
        }
        if (!hasGap) {

            for (int x = 0; x < width; x++) gameField[y][x] = 0;
            shiftDown(y);
        }
    }
}

bool updateFallingPiece(){

    for (int i = 0; i < 4; i++) {
        if (checkCollision(piece[(i*2)+1]+1, piece[i*2])) {
            if (piece[(i*2)+1]+1 <= upperEdge){
                endGame();
            } 
            checkForFullRows();
            spawnPiece(rand() % 4);
            return false;
        };
    }
    for (int i = 0; i < 4; i++) {
        gameField[piece[(i*2)+1]][piece[i*2]] = 0;
    }

    for (int i = 0; i < 4; i++) {
        piece[(i*2)+1]++;
        gameField[piece[(i*2)+1]][piece[i*2]] = 1;
    }
    return true;
}

void move(int dir){
    for (int i = 0; i < 4; i++) {
        if (checkCollision(piece[(i*2)+1], piece[i*2] + dir) || checkWallCollision(piece[i*2] + dir)) {
            return;
        };
    }
    for (int i = 0; i < 4; i++) {
        gameField[piece[(i*2)+1]][piece[i*2]] = 0;
    }

    for (int i = 0; i < 4; i++) {
        piece[(i*2)] += dir;
        gameField[piece[(i*2)+1]][piece[i*2]] = 1;
    }
}

void draw(wchar_t wallChar, wchar_t blockChar) {
    system("cls");
    wprintf(L"%c", wallChar);
    for (int x = 0; x < width; x++) wprintf(L"%c", wallChar);
    wprintf(L"%c\n", wallChar);
    for (int y = upperEdge; y < height; y++) {
        wprintf(L"%c", wallChar);
        for (int x = 0; x < width; x++) {
            if (gameField[y][x] == 0)
                wprintf(L"%c", emptyChar);
            else if (gameField[y][x] == 1)
                wprintf(L"%lc", blockChar);
        }
        wprintf(L"%c\n", wallChar);
    }
    wprintf(L"%c", wallChar);
    for (int x = 0; x < width; x++) wprintf(L"%c", wallChar);
    wprintf(L"%c\n%c", wallChar, wallChar);
    for (int x = 0; x < width - (log10(points)); x++){
        if (x == width/2) wprintf(L"%d", points);
        else wprintf(L" ");
    }
    wprintf(L"%c\n%c", wallChar, wallChar);
    for (int x = 0; x < width; x++) wprintf(L"%c", wallChar);
    wprintf(L"%c\n", wallChar);
}

void pushDown(){
    bool isDown = false;

    while(!isDown){
        isDown = !updateFallingPiece();
    }
}

int main() {   
    setlocale(LC_CTYPE, "");
    _setmode(_fileno(stdout), _O_U16TEXT);

    gettimeofday(&timer, NULL);
    long startTime = timer.tv_sec;
    long currentTime = (timer.tv_sec - startTime) * 1000000 + timer.tv_usec;

    wchar_t wallChar = 0x25A1;
    wchar_t blockChar = 0x25A0;

    fill();
    spawnPiece(rand() % 4);

    long lastTime = 0;

    wchar_t leftC = 0x2190;
    wchar_t rightC = 0x2192;
    wchar_t counterClockwiseC = 0x27F2;
    wchar_t clockwiseC = 0x27F3;
    wchar_t downC = 0x2193;

    wprintf(L"\n\n\n\n   %lc  Q    E %lc\n\n    %lc A    D %lc\n\n     Spacebar \n        %lc\n\n\n\n", counterClockwiseC, clockwiseC, leftC, rightC, downC);

    Sleep(4000);
    key = '0';      

    while (game) {
        gettimeofday(&timer, NULL);
        long currentTime = ((timer.tv_sec - startTime) * 1000000 + timer.tv_usec) / 100000;
        if (kbhit()) {
            key = getch();
        }
        if (lastTime != currentTime && currentTime % updateTimer == 0) {
            lastTime = currentTime;
            points++;
            updateFallingPiece();
            if (key == 'a') move(-1);
            if (key == 'd') move( 1);
            if (key == 'e') rotatePiece(1);
            if (key == 'q') rotatePiece(-1);
            if (key == ' ') pushDown();
            if (game) draw(wallChar, blockChar);
            key = '0';
        }
    }
    return 0;
}
