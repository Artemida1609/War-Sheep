#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "war_sheep.h"
#include "war_sheep.c" 

//Ship Sizes
int shipSizesEasy[SIZE_EASY] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
int shipSizesMedium[SIZE_MEDIUM] = {4, 2, 2, 2, 1, 1, 1, 1, 1};
int shipSizesHard[SIZE_HARD] = {2, 2, 2, 1, 1, 1, 1, 1};

//Time
void displayCurrentTime() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char str_time[21];
    strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M:%S", tm_now);

    mvprintw(0, 0, "Current time: %s", str_time);
    int temperature = 24; 
    mvprintw(2, 0, "Temperature: %d°C, Sunny", temperature);
}

//MAIN
int main() {
    initscr(); // Initialize ncurses
    keypad(stdscr, TRUE);
    timeout(-1);
    echo();
    curs_set(0);
    initColors();

    timeout(1000);

    const char *choices[] = {
        "   Start Game ",
        "   Settings ",
        "     EXIT   ",
    };
    const char *levelsChoices[] = {
        "  EASY     ",
        " MEDIUM    ",
        "  HARD     ",
    };


    const char *ship[] = {
    "                                        ",
    "              |    |    |               ",
    "             )_)  )_)  )_)              ",
    "            )___))___))___)\\           ",
    "           )____)____)_____)\\          ",
    "         _____|____|____|____\\\\__     ",
    "--------\\                   /--------- ",
    "  ^^^^^ ^^^^^^^^^^^^^^^^^^^^^           ",
    "    ^^^^      ^^^^     ^^^    ^^        ",
    "         ^^^^      ^^^                  ",
    };


    const char *sheep[] = {
    "            __   _               ",
    "       .-;'    `; `-._           ",
    "     (_,              )          ",
    "   ,'o^(               )>        ",
    "  (__,-'              )          ",
    "      (              )     \\|/  ",
    "       `-'._.--._.-'             ",
    "\\|/       |||  |||    \\|/      ",
    };



    const char *arrow = "-->";
    int n_choices = sizeof(choices) / sizeof(char *);
    int settingsHighlight = 0;
    int highlight = 0;
    int settingsC;
    int c;
    int level = 0;

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    int inMenu = 1;
    
    while (1) {
        getmaxyx(stdscr, maxY, maxX);
        clear();
        if(inMenu)
        {
            displayCurrentTime();
        }
        
        // Виведення овечки
        int sheepWidth = 34; 
        int sheepHeight = sizeof(sheep) / sizeof(sheep[0]); 
        int sheepStartX = maxX/2 - sheepWidth/2 + 40; 
        int sheepStartY = maxY / 2 - sheepHeight / 2; 

        for (int i = 0; i < sheepHeight; i++) {
            mvprintw(sheepStartY + i -2, sheepStartX, "%s", sheep[i]);
        }

        int GameNameTitleY = maxY / 2 - n_choices / 2 -3;
        int GameNameTitleX = maxX / 2 - strlen("\\\\\\\\\\\\WAR SHEEP//////") / 2;

        attron(COLOR_PAIR(1)); 
        mvprintw(GameNameTitleY, GameNameTitleX, "\\\\\\\\\\\\WAR SHEEP//////");
        mvprintw(GameNameTitleY +5, GameNameTitleX, "//////WAR SHEEP\\\\\\\\\\\\");
        attroff(COLOR_PAIR(1));

        int shipWidth = 40; 
        int shipHeight = sizeof(ship) / sizeof(ship[0]); 
        int shipStartX = maxX/2 - 53 ; 
        int shipStartY = maxY / 2 - shipHeight / 2; 

        for (int i = 0; i < shipHeight; i++) {
            mvprintw(shipStartY + i -2, shipStartX, "%s", ship[i]);
        }

        int menuTitleY = maxY / 2 - n_choices / 2 - 2;
        int menuTitleCenterX = maxX / 2 - strlen("======MENU======") / 2 ;
        mvprintw(menuTitleY, menuTitleCenterX, "======MENU======");

        // Center the menu options
        int menuStartY = maxY / 2 - n_choices / 2 - 1;
        for (int i = 0; i < n_choices; ++i) {
            int centerX = maxX / 2 - strlen(choices[i]) / 2 -2;
            if (i == highlight) {
                attron(A_REVERSE);
                mvprintw(menuStartY + i, centerX, "%s %s", arrow, choices[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(menuStartY + i, centerX, " %s", choices[i]);
            }
        }

        refresh();
        napms(100);
        c = getch();

        switch (c) {
            case KEY_UP:
                if (highlight > 0)
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight < n_choices - 1)
                    ++highlight;
                break;
            case 10: // Enter key
                if (highlight == 0) {
                    inMenu = 0;
                    timeout(-1);  
                    clear();
                    int gameResult = startGame(level); 
                    if (gameResult == 1) {
                        inMenu = 1;
                    }
                    timeout(1000); 
                } else if (highlight == 1) {
                    // Settings
                    while (1) {
                        clear();
                        displayCurrentTime();
                        int shipWidth = 40; 
                        int shipHeight = sizeof(ship) / sizeof(ship[0]); 
                        int shipStartX = maxX/2 - 53 ;
                        int shipStartY = maxY / 2 - shipHeight / 2; 

                        for (int i = 0; i < shipHeight; i++) {
                            mvprintw(shipStartY + i -2, shipStartX, "%s", ship[i]);
                        }

                        
                        int sheepWidth = 34; 
                        int sheepHeight = sizeof(sheep) / sizeof(sheep[0]); 
                        int sheepStartX = maxX/2 - sheepWidth/2 + 40; 
                        int sheepStartY = maxY / 2 - sheepHeight / 2; 

                        for (int i = 0; i < sheepHeight; i++) {
                            mvprintw(sheepStartY + i -2, sheepStartX, "%s", sheep[i]);
                        }


                        int settingsTitleY = maxY / 2 - sizeof(levelsChoices) / sizeof(char *) ;
                        int settingsTitleCenterX = maxX / 2 - strlen("======LEVELS======") / 2;
                        mvprintw(settingsTitleY, settingsTitleCenterX, "======LEVELS======");
                        int GameNameTitleY = maxY / 2 - n_choices / 2 -3;
                        int GameNameTitleX = maxX / 2 - strlen("\\\\\\\\\\\\WAR SHEEP//////") / 2;
                        attron(COLOR_PAIR(1)); 
                        mvprintw(GameNameTitleY, GameNameTitleX, "\\\\\\\\\\\\WAR SHEEP//////");
                        mvprintw(GameNameTitleY +5, GameNameTitleX, "//////WAR SHEEP\\\\\\\\\\\\");
                        attroff(COLOR_PAIR(1));
                        int settingsStartY = maxY / 2 - sizeof(levelsChoices) / sizeof(char *) + 1;
                        for (int i = 0; i < sizeof(levelsChoices) / sizeof(char *); ++i) {
                            int centerX = maxX / 2 - strlen(levelsChoices[i]) / 2;
                            if (i == settingsHighlight) {
                                attron(A_REVERSE);
                                mvprintw(settingsStartY + i, centerX, "%s %s", arrow, levelsChoices[i]);
                                attroff(A_REVERSE);
                            } else {
                                mvprintw(settingsStartY + i, centerX, " %s", levelsChoices[i]);
                            }
                        }

                        refresh();
                        settingsC = getch();

                        switch (settingsC) {
                            case KEY_UP:
                                if (settingsHighlight > 0)
                                    --settingsHighlight;
                                break;
                            case KEY_DOWN:
                                if (settingsHighlight < sizeof(levelsChoices) / sizeof(char *) - 1)
                                    ++settingsHighlight;
                                break;
                            case 10: // Enter key
                                level = settingsHighlight;
                                break;
                            default:
                                break;
                        }

                        if (settingsC == 10)
                            break; // Exit settings on Enter
                    }
                } else if (highlight == 2) {
                    // Exit
                    endwin(); // End ncurses
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    endwin(); // End ncurses
    return 0;
}
