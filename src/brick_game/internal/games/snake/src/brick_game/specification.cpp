#include "specification.h"
#include <stdio.h>
#include "game_fabric.h"

void userInput(UserAction_t input, bool hold) {
  (void)hold;

  static bool first_call = true;
  if (first_call) {
    s21::GameFabric::set_game(s21::GameFabric::GameName(input));
    input = UserAction_t::Start;
    first_call = false;
  }

  s21::Game* current_game = s21::GameFabric::get_game();
  if (current_game) {
    current_game->set_action(input);
  }
}

GameInfo_t updateCurrentState() {
  GameInfo_t gameinfo = {nullptr, nullptr, 0, 0, 0, 0, 0};
  s21::Game* current_game = s21::GameFabric::get_game();
  if (current_game) {
    current_game->fsm();
    gameinfo = current_game->get_gameinfo();
  }
  return gameinfo;
}
