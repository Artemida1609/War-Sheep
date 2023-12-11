#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "war_sheep.h"

#define ROWS 10
#define COLS 10
#define INPUT_BUFFER_SIZE 256
char inputBuffer[INPUT_BUFFER_SIZE];

//Colors
enum {
    RED_TEXT = 1,
    GREEN_TEXT = 2,
    WHITE_TEXT = 3,
    BLUE_TEXT = 4,
    YELLOW_TEXT = 5,
};

//Colors
void initColors() {
    initscr();
    start_color();
    init_pair(RED_TEXT, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN_TEXT, COLOR_GREEN, COLOR_BLACK);
    init_pair(WHITE_TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(BLUE_TEXT, COLOR_BLUE, COLOR_BLACK);
    init_pair(YELLOW_TEXT, COLOR_YELLOW, COLOR_BLACK);
    refresh();
}

//Game Over Window
void showWinWindow() {
    int winWidth = 35, winHeight = 10;

    int screenHeight, screenWidth;
    getmaxyx(stdscr, screenHeight, screenWidth);

    int startX = (screenWidth - winWidth) / 2;
    int startY = (screenHeight - winHeight) / 2;

    // Створюємо вікно з обчисленими координатами
    WINDOW *win = newwin(winHeight, winWidth, startY, startX);
    box(win, 0, 0);

    // Отримуємо координати тексту для виведення його посередині вікна
    int textX = (winWidth -9) / 2;
    int textY = winHeight / 2;

    mvwprintw(win, textY, textX, "GAME OVER");
    wrefresh(win);

    getch();  // Чекаємо на будь-яке введення користувача

    delwin(win); 
    refresh(); // Закриваємо вікно
}

//Initialize Board
void initializeBoard(char board[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = ' ';
        }
    }
}

//Placement Valid
int isPlacementValid(char board[ROWS][COLS], int row, int col, int size, int direction) {
    for (int i = 0; i < size; i++) {
        int r = row + (direction == 1 ? i : 0);
        int c = col + (direction == 0 ? i : 0);

        if (r >= ROWS || c >= COLS) return 0;

        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != ' ') {
                    return 0;
                }
            }
        }
    }
    return 1;
}

//Randomly Placement
void placeShipsRandomly(char board[ROWS][COLS], int level) {
    int *shipSizes;
    int n;

    switch (level) {
        case 0: // Легкий рівень
            shipSizes = shipSizesEasy;
            n = SIZE_EASY;
            break;
        case 1: // Середній рівень
            shipSizes = shipSizesMedium;
            n = SIZE_MEDIUM;
            break;
        case 2: // Важкий рівень
            shipSizes = shipSizesHard;
            n = SIZE_HARD;
            break;
        default:
            break;
    }

    for (int k = 0; k < n; k++) {
        int size = shipSizes[k];
        int placed = 0;
        while (!placed) {
            int row = rand() % ROWS;
            int col = rand() % COLS;
            int direction = rand() % 2;

            if (isPlacementValid(board, row, col, size, direction)) {
                for (int i = 0; i < size; i++) {
                    if (direction == 0) {
                        board[row][col + i] = 'W';
                    } else {
                        board[row + i][col] = 'W';
                    }
                }
                placed = 1;
            }
        }
    }
}

//Destroyed Ship
int isShipDestroyed(char board[ROWS][COLS] , int row, int col) {
    int shipLength = 0;
    int hitCount = 0;

    int startCol = col;
    while (startCol > 0 && (board[row][startCol - 1] == 'W' || board[row][startCol - 1] == 'X')) {
        startCol--;
    }
    int endCol = col;
    while (endCol < COLS - 1 && (board[row][endCol + 1] == 'W' || board[row][endCol + 1] == 'X')) {
        endCol++;
    }
    for (int c = startCol; c <= endCol; c++) {
        shipLength++;
        if (board[row][c] == 'X') {
            hitCount++;
        }
    }

    if (shipLength == 1) {
        int startRow = row;
        while (startRow > 0 && (board[startRow - 1][col] == 'W' || board[startRow - 1][col] == 'X')) {
            startRow--;
        }
        int endRow = row;
        while (endRow < ROWS - 1 && (board[endRow + 1][col] == 'W' || board[endRow + 1][col] == 'X')) {
            endRow++;
        }
        for (int r = startRow; r <= endRow; r++) {
            shipLength++;
            if (board[r][col] == 'X') {
                hitCount++;
            }
        }
    }

    return hitCount == shipLength;
}

//Mark Around Destroyed Ship
void markAroundDestroyedShip(char board[ROWS][COLS] , int row, int col) {
    int startRow = row, endRow = row, startCol = col, endCol = col;

    while (startRow > 0 && board[startRow - 1][col] == 'X') startRow--;
    while (endRow < ROWS - 1 && board[endRow + 1][col] == 'X') endRow++;

    while (startCol > 0 && board[row][startCol - 1] == 'X') startCol--;
    while (endCol < COLS - 1 && board[row][endCol + 1] == 'X') endCol++;

    for (int r = startRow - 1; r <= endRow + 1; r++) {
        for (int c = startCol - 1; c <= endCol + 1; c++) {
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == ' ') {
                board[r][c] = 'O';
            }
        }
    }
}

//Ship Location
int guessShipLocation(char board[ROWS][COLS]) {
    char colGuess;
    int rowGuess;
    int validInput;

    do {
        validInput = 1; 

        curs_set(1);
        printw("Enter your guess (e.g. A5) (or 'qq' to quit): ");
        refresh();
        getstr(inputBuffer);
        curs_set(0);

        if ((inputBuffer[0] == 'q' || inputBuffer[0] == 'Q') && (inputBuffer[1] == 'q' || inputBuffer[1] == 'Q')) { 
            return 1;
        }
        if ( sscanf(inputBuffer, " %c%d", &colGuess, &rowGuess) == 2 && isalpha(colGuess)) {
            int colIndex = toupper(colGuess) - 'A';
            int rowIndex = rowGuess - 1;

            if (rowIndex >= 0 && rowIndex < ROWS && colIndex >= 0 && colIndex < COLS) {
                if (board[rowIndex][colIndex] == 'W') {
                    printw("Hit!\n");
                    board[rowIndex][colIndex] = 'X';
                    if (isShipDestroyed(board, rowIndex, colIndex)) {
                        markAroundDestroyedShip(board, rowIndex, colIndex);
                        printw("Ship destroyed!\n");
                    }
                    return 1;
                } else if (board[rowIndex][colIndex] == ' ' || board[rowIndex][colIndex] == 'W') {
                    printw("Miss.\n");
                    board[rowIndex][colIndex] = 'O';
                    return 0;
                } else {
                    printw("Already guessed.\n");
                    return 2;
                }
            } else {
                printw("Invalid guess. Try again.\n");
                validInput = 0; // Введення некоректне
            }
        } else {
            printw("Invalid format. Please enter a letter and a number (e.g. A5).\n");
            validInput = 0; // Формат введення некоректний
        }
    } while (!validInput);

    return 0;
}

//Print 2 Boards
void printBoardsSideBySide(char board1[ROWS][COLS], char board2[ROWS][COLS]) {
    clear();

    printw("\n\n\n  Player 1's Board:   \t\t\t\tPlayer 2's Board:\n");

    printw("     ");
    for (char col = 'A'; col < 'A' + COLS; col++) {
        printw(" %c  ", col);
    }
    printw("  \t        ");
    for (char col = 'A'; col < 'A' + COLS; col++) {
        printw(" %c  ", col);
    }
    printw("\n");

    printw("    ");
    for (int j = 0; j < COLS; j++) {
        attron(COLOR_PAIR(4));
        printw("+---");
        attroff(COLOR_PAIR(4));
    }
    attron(COLOR_PAIR(4));
    printw("+  \t       ");
    attroff(COLOR_PAIR(4));
    for (int j = 0; j < COLS; j++) {
        attron(COLOR_PAIR(4));
        printw("+---");
        attroff(COLOR_PAIR(4));
    }
    attron(COLOR_PAIR(4));
    printw("+\n");
    attroff(COLOR_PAIR(4));

    for (int i = 0; i < ROWS; i++) {
        printw(" %2d ", i + 1);
        for (int j = 0; j < COLS; j++) {
            char symbol1 = board1[i][j];
            if (symbol1 == 'W') {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(BLUE_TEXT));
                printw(" ~ ");
                attroff(COLOR_PAIR(BLUE_TEXT));
            } else if (symbol1 == ' ') {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(BLUE_TEXT));
                printw(" ~ ");
                attroff(COLOR_PAIR(BLUE_TEXT));
            } else {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(symbol1 == 'X' ? RED_TEXT : WHITE_TEXT));
                printw(" %c ", symbol1);
                attroff(COLOR_PAIR(symbol1 == 'X' ? RED_TEXT : WHITE_TEXT));
            }
        }
        attron(COLOR_PAIR(4));
        printw( "|");
        attroff(COLOR_PAIR(4));
        printw("       %2d ", i+1);
        for (int j = 0; j < COLS; j++) {
            char symbol2 = board2[i][j];
            if (symbol2 == 'W') {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(BLUE_TEXT));
                printw(" ~ ");
                attron(COLOR_PAIR(BLUE_TEXT));
            } else if (symbol2 == ' ') {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(BLUE_TEXT));
                printw(" ~ ");
                attron(COLOR_PAIR(BLUE_TEXT));
            } else {
                attron(COLOR_PAIR(4));
                printw("|");
                attroff(COLOR_PAIR(4));
                attron(COLOR_PAIR(symbol2 == 'X' ? RED_TEXT : WHITE_TEXT));
                printw(" %c ", symbol2);
                attroff(COLOR_PAIR(symbol2 == 'X' ? RED_TEXT : WHITE_TEXT));
            }
        }
        attron(COLOR_PAIR(4));
        printw("|\n");
        attroff(COLOR_PAIR(4));

        printw("    ");
        for (int j = 0; j < COLS; j++) {
            attron(COLOR_PAIR(4));
            printw("+---");
            attroff(COLOR_PAIR(4));
        }
        attron(COLOR_PAIR(4));
        printw("+\t       ");
        attroff(COLOR_PAIR(4));
        for (int j = 0; j < COLS; j++) {
            attron(COLOR_PAIR(4));
            printw("+---");
            attroff(COLOR_PAIR(4));
        }
        attron(COLOR_PAIR(4));
        printw("+\n");
        attroff(COLOR_PAIR(4));
    }
    refresh();
}

int checkGameOver(char board[ROWS][COLS]) {
    if (strcmp(inputBuffer, "qq") == 0) {
        return 1;
    }
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == 'W') {
                return 0;
            }
        }
    }
    return 1;
}

//Player Turn
void playerTurn(char board1[ROWS][COLS], char board2[ROWS][COLS] , int player) {
    int hit;
    char (*targetBoard)[COLS] = (player == 1) ? board2 : board1;
    do {
        refresh();
        printw("PLAYER %d'S TURN\n", player);
        
        hit = guessShipLocation(targetBoard);
        if (hit == 1) {
            if (checkGameOver(board2) || checkGameOver(board1)) {
                break;
            }
            clear();
            printBoardsSideBySide(board1, board2);
            printw("YOU HIT A SHIP! GUESS AGAIN.\n");
            refresh();
        } else if (hit == 2) {
            printw("PLEASE MAKE ANOTHER GUESS.\n");
            refresh();
        }
    } while (hit == 1 || hit == 2);
}

//Game Play
int startGame(int level) {

    char board1[ROWS][COLS];
    char board2[ROWS][COLS];
    
    initializeBoard(board1);
    initializeBoard(board2);

    srand(time(NULL));

        printw("Player 1, place your ships.\n");
        placeShipsRandomly(board1, level); 
        printw("Player 2, place your ships.\n");
        placeShipsRandomly(board2, level); 

        int gameOver = 0;
        while (!gameOver) {
            clear();
            printBoardsSideBySide(board1, board2);
            refresh();
            playerTurn(board1, board2, 1);
            if (strcasecmp(inputBuffer, "qq") == 0){
                clear(); 
                refresh();
                showWinWindow();
                return 1;
            }

            gameOver = checkGameOver(board2);
            if (gameOver) {
                clear();  
                refresh();
                showWinWindow();
                return 1;
            }
            printBoardsSideBySide(board1, board2);
            refresh();
            // Player 2's turn
            playerTurn(board1, board2, 2);
            if (strcmp(inputBuffer, "qq") == 0 || strcmp(inputBuffer, "QQ") == 0) {
                clear(); 
                refresh();
                showWinWindow();
                return 1;
            }

            gameOver = checkGameOver(board1);
            if (gameOver) {
                clear();  
                refresh();
                showWinWindow();
                return 1;
            }
        }
        return 0;
}