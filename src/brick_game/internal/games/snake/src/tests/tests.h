#ifndef TESTS_H
#define TESTS_H

#include <gtest/gtest.h>

#include <iostream>

#include "../brick_game/defines.h"
#include "../brick_game/game_fabric.h"

void find_obj_coords(int** field, int& coord_y, int& coord_x,
                     const int& obj_type);
void find_field_half(const int& apple_y, const int& apple_x, int& y_half,
                     int& x_half);
void horizontal_shift(s21::Game* snake_game, const int& x_half,
                      const int& apple_x, int& head_x);
void vertical_shift(s21::Game* snake_game, const int& y_half, const int& x_half,
                    const int& apple_y, int& head_y);

#endif  // TESTS_H