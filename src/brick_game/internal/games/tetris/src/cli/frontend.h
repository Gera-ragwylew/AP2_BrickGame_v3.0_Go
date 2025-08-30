#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>

#include "../backend/backend.h"

WINDOW* create_newwin(int height, int width, int starty, int startx);
void print_new_info(GameInfo_t stats, WINDOW* local_win);
void score_to_string(char* str, int score);
void destroy_win(WINDOW* local_win);
void print_back(WINDOW* local_win);
void print_start_screen(WINDOW* local_win);
void state_log(Tetris* tetris);

#endif