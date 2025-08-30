#include "frontend.h"

void game_input() {
  switch (getch()) {
    case KEY_LEFT:
      userInput(Left, false);
      break;
    case KEY_RIGHT:
      userInput(Right, false);
      break;
    case KEY_UP:
      userInput(Up, false);
      break;
    case KEY_DOWN:
      userInput(Down, false);
      break;
    case 'a':
      userInput(Action, false);
      break;
    case ' ':
      userInput(Pause, false);
      break;
    case 27:
      userInput(Terminate, false);
      break;
    case 's':
      userInput(Start, false);
      break;
    default:
      break;
  }
}

int menu_input() {
  int menu_select = 0;
  switch (getch()) {
  case '1':
    userInput(Pause, false); // race selection #1
    menu_select = 1;
    break;
  case '2':
    userInput(Terminate, false); // tetris selection #2
    menu_select = 2;
    break;
  case '3':
    userInput(Left, false); // snake selection #3
    menu_select = 3;
    break;
  case 27:
    menu_select = -1;
    break;  
  default:
    menu_select = 0;
    break;
  }

  return menu_select;
}

void game_loop(WINDOW* my_win) {
  GameInfo_t state;
  
  while (state.level != -1) {
    game_input();
    state = updateCurrentState();
    print_back(my_win);

    if (state.level > 0) {
      print_new_info(state, my_win);
    }
    wrefresh(my_win);
  }
}

void menu_loop(WINDOW* my_win) {
  GameInfo_t state;
  int menu_select = 0;

  while (menu_select != -1) {
    state = updateCurrentState();
    
    print_back(my_win);
    print_game_list(state, my_win);
    wrefresh(my_win);

    menu_select = menu_input();

    if (menu_select != 0 && menu_select != -1) {
      game_loop(my_win);
      menu_select = 0;
      werase(my_win);
      wrefresh(my_win);
    }
  }
}

int main(void) {
  WINDOW* my_win;
  int starty, startx, width, height;

  srand(time(NULL));
  initscr();

  if (!has_colors()) {
    printf("color not support\n");
    endwin();
    return 1;
  }

  start_color();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
  timeout(10);

  height = 2 + WINDOW_HEIGHT + 5;
  width = 3 + WINDOW_WIDTH * 2 + 11;
  starty = 2;
  startx = 4;
  my_win = create_newwin(height, width, starty, startx);

  menu_loop(my_win);

  destroy_win(my_win);
  endwin();

  return 0;
}
