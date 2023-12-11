#ifndef WAR_SHEEP_H
#define WAR_SHEEP_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define ROWS 10
#define COLS 10
extern int shipSizesEasy[];
extern int shipSizesMedium[];
extern int shipSizesHard[];
#define SIZE_EASY 10
#define SIZE_MEDIUM 10
#define SIZE_HARD 10


void initColors();
void initializeBoard(char board[ROWS][COLS]);
int isPlacementValid(char board[ROWS][COLS], int row, int col, int size, int direction);
int isShipDestroyed(char board[ROWS][COLS], int row, int col);
void markAroundDestroyedShip(char board[ROWS][COLS], int row, int col);
int guessShipLocation(char board[ROWS][COLS]);
void printBoardsSideBySide(char board1[ROWS][COLS], char board2[ROWS][COLS]);
int checkGameOver(char board[ROWS][COLS]);
void playerTurn(char board1[ROWS][COLS], char board2[ROWS][COLS], int player);
int startGame();

#endif  // WAR_SHEEP_H
