#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../brick_game/defines.h"
#include "../../brick_game/specification.h"
#include "../../brick_game/timer.h"

#define Tetris 1
#define Snake 2

void ncurses_init();
void game_loop(WINDOW* my_win);
void set_action();
bool is_end(GameInfo_t stats);
void update_screen(GameInfo_t stats, WINDOW* local_win, s21::Timer timer);
void score_to_string(char* str, int score);
int game_selection(WINDOW* local_win);

WINDOW* create_newwin();
void destroy_win(WINDOW* local_win);

void print_back(WINDOW* local_win);
void print_tetris_stub(WINDOW* local_win);
void print_snake_stub(WINDOW* local_win);
void print_pause_stub(WINDOW* local_win);
void print_end_stub(WINDOW* local_win);
void print_win_stub(WINDOW* local_win);
void print_stats_field(GameInfo_t stats, WINDOW* local_win);
void print_stats_next(GameInfo_t stats, WINDOW* local_win);

// void state_log(Game* game);

#endif  // FRONTEND_H