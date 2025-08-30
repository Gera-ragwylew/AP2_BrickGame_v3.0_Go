#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "../client_library/game_client.h"


#define WINDOW_HEIGHT 20
#define WINDOW_WIDTH 10
#define NEXT_SIZE 4

WINDOW* create_newwin(int height, int width, int starty, int startx);
void print_new_info(GameInfo_t stats, WINDOW* local_win);
void score_to_string(char* str, int score);
void destroy_win(WINDOW* local_win);
void print_back(WINDOW* local_win);
void print_start_screen(WINDOW* local_win);
void print_game_list(GameInfo_t state, WINDOW* local_win);
//void state_log(Tetris* tetris);

#endif