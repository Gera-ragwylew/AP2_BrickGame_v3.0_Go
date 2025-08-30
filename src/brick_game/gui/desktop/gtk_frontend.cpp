#include "gtk_frontend.h"

#define GTK_WINDOW_Y 400
#define GTK_WINDOW_X 550

#define SCALE 20.0
#define FRAME_SHIFT 20

#define FRAME_RATE 10

#define COLOR_SWTICH(field)              \
  switch (field[y][x]) {                 \
    case 1:                              \
      cr->set_source_rgb(0, 0, 1);       \
      break;                             \
    case 2:                              \
      cr->set_source_rgb(1, 0, 0);       \
      break;                             \
    case 3:                              \
      cr->set_source_rgb(0, 1, 0);       \
      break;                             \
    case 4:                              \
      cr->set_source_rgb(1, 0.5, 0);     \
      break;                             \
    case 5:                              \
      cr->set_source_rgb(1, 0, 1);       \
      break;                             \
    case 6:                              \
      cr->set_source_rgb(1, 0, 1);       \
      break;                             \
    default:                             \
      cr->set_source_rgb(0.5, 0.5, 0.5); \
      break;                             \
  }

MyGtkWindow::MyGtkWindow(): main_box(Gtk::Orientation::VERTICAL), start_box(Gtk::Orientation::VERTICAL) {
  set_title("Brick game");
  set_default_size(GTK_WINDOW_Y, GTK_WINDOW_X);
  set_child(main_box);

  last_game_id = 0;
  setup_main_menu();

  start_box.set_margin_top(100);
  start_box.set_spacing(50);
  start_box.set_valign(Gtk::Align::CENTER);
  start_box.set_halign(Gtk::Align::CENTER);

  main_box.append(start_box);
}

void MyGtkWindow::start_game() {
  main_box.remove(start_box);

  game_area = Gtk::manage(new GameArea);
  Gtk::Box field_box;
  Gtk::Frame *game_area_frame = Gtk::manage(new Gtk::Frame);
  game_area_frame->set_child(*game_area);
  setup_game_area_frame(game_area_frame);
  field_box.append(*game_area_frame);

  next_area = Gtk::manage(new NextArea);
  Gtk::Frame *next_area_frame = Gtk::manage(new Gtk::Frame);
  next_area_frame->set_child(*next_area);
  next_area_frame->set_size_request(NEXT_SIZE * SCALE, NEXT_SIZE * SCALE);

  Gtk::Box arrow_box;
  auto arrow_image = Gtk::make_managed<Gtk::Image>("../gui/desktop/image.png");
  arrow_image->set_size_request(150, 150);
  arrow_box.append(*arrow_image);

  Gtk::Box button_box(Gtk::Orientation::VERTICAL);
  setup_button_labels(&button_box);

  Gtk::Box info_box(Gtk::Orientation::VERTICAL);
  setup_info_box(&info_box, next_area_frame);

  Gtk::Box top_box(Gtk::Orientation::HORIZONTAL);
  top_box.append(field_box);
  top_box.append(info_box);

  Gtk::Box control_box(Gtk::Orientation::HORIZONTAL);
  control_box.set_margin_start(20);
  control_box.append(button_box);
  control_box.append(arrow_box);

  main_box.append(top_box);
  main_box.append(control_box);
  auto button_controller = Gtk::EventControllerKey::create();
  button_controller->signal_key_pressed().connect(
      sigc::mem_fun(*this, &MyGtkWindow::on_key_press), false);
  add_controller(button_controller);

  timer_connection = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &MyGtkWindow::update_game), FRAME_RATE);
}

void MyGtkWindow::setup_game_area_frame(Gtk::Frame *game_area_frame) {
  game_area_frame->set_label_align(Gtk::Align::CENTER);
  game_area_frame->set_margin_start(FRAME_SHIFT);
  game_area_frame->set_margin_end(FRAME_SHIFT);
  game_area_frame->set_margin_top(FRAME_SHIFT);
  game_area_frame->set_margin_bottom(FRAME_SHIFT);
  game_area_frame->set_size_request(WINDOW_WIDTH * SCALE,
                                    WINDOW_HEIGHT * SCALE);
}

void MyGtkWindow::setup_button_labels(Gtk::Box *button_box) {
  start_label.set_markup("<span font_desc='15'>S - START</span>");
  quit_label.set_markup("<span font_desc='15'>Q - QUIT</span>");
  pause_label.set_markup("<span font_desc='15'>SPACE - PAUSE</span>");
  action_label.set_markup("<span font_desc='15'>A - ACTION</span>");
  start_label.set_valign(Gtk::Align::START);
  quit_label.set_valign(Gtk::Align::START);
  pause_label.set_valign(Gtk::Align::START);
  action_label.set_valign(Gtk::Align::START);

  button_box->set_margin_end(50);
  button_box->append(start_label);
  button_box->append(quit_label);
  button_box->append(pause_label);
  button_box->append(action_label);
}

void MyGtkWindow::setup_info_box(Gtk::Box *info_box, Gtk::Frame *next_area_frame) {
  info_box->set_margin_start(10);
  info_box->set_margin_end(10);
  info_box->set_margin_top(10);
  info_box->set_margin_bottom(10);
  info_box->append(score_label);
  info_box->append(score_value_label);
  info_box->append(hi_score_label);
  info_box->append(hi_score_value_label);
  info_box->append(next_label);
  info_box->append(*next_area_frame);
  info_box->append(speed_label);
  info_box->append(speed_value_label);
  info_box->append(level_label);
  info_box->append(level_value_label);
  score_label.set_markup("<span font_desc='15'>SCORE</span>");
  score_value_label.set_markup("<span font_desc='15'>000000</span>");
  hi_score_label.set_markup("<span font_desc='15'>HI-SCORE</span>");
  hi_score_value_label.set_markup("<span font_desc='15'>000000</span>");
  next_label.set_markup("<span font_desc='15'>NEXT</span>");
  speed_label.set_markup("<span font_desc='15'>SPEED</span>");
  speed_value_label.set_markup("<span font_desc='15'>0</span>");
  level_label.set_markup("<span font_desc='15'>LEVEL</span>");
  level_value_label.set_markup("<span font_desc='15'>1</span>");

  hi_score_label.set_margin_top(20);
  next_label.set_margin_top(20);
  speed_label.set_margin_top(20);
  level_label.set_margin_top(20);
}

bool MyGtkWindow::on_key_press(guint16 keyval, guint, Gdk::ModifierType state) {
  bool res = true;
  (void)state;
  if (keyval == GDK_KEY_Right) {
    userInput(UserAction_t::Right, false);
  } else if (keyval == GDK_KEY_Left) {
    userInput(UserAction_t::Left, false);
  } else if (keyval == GDK_KEY_Up) {
    userInput(UserAction_t::Up, false);
  } else if (keyval == GDK_KEY_Down) {
    userInput(UserAction_t::Down, false);
  } else if (keyval == GDK_KEY_s) {
    userInput(UserAction_t::Start, false);
  } else if (keyval == GDK_KEY_space) {
    userInput(UserAction_t::Pause, false);
  } else if (keyval == GDK_KEY_a) {
    userInput(UserAction_t::Action, false);
  } else if (keyval == GDK_KEY_q) {
    userInput(UserAction_t::Terminate, false);
    res = false;
  }
  return res;
}

void MyGtkWindow::on_exit_button_clicked() { close(); }

void MyGtkWindow::on_game_exit_button_clicked() {
  return_to_main_menu();
}

bool MyGtkWindow::update_game() {
  bool res = false;
  current_state = updateCurrentState();
  
  if (current_state.level == LOSE_LVL) {
    show_game_over_dialog("You lose! Try again?");
    res = false;
  } else if (current_state.level == WIN_LVL) {
    show_game_over_dialog("You win! Play again?");
    res = false;
  } else {
    game_area->game_field = current_state.field;
    game_area->queue_draw();

    next_area->next_field = current_state.next;
    next_area->queue_draw();
    
    update_game_info();
    res = true;
  }
  return res;
}

void MyGtkWindow::show_game_over_dialog(const Glib::ustring &message) {
  auto dialog = std::make_shared<Gtk::MessageDialog>(*this, "Game Over", false,
                                                    Gtk::MessageType::INFO,
                                                    Gtk::ButtonsType::OK_CANCEL);
  dialog->set_secondary_text(message);
  dialog->set_modal(true);
  dialog->set_hide_on_close(true);
  
  dialog->signal_response().connect([this, dialog](int response_id) {
      dialog->hide();
      if (response_id == Gtk::ResponseType::CANCEL) {
        return_to_main_menu();
      } else if (response_id == Gtk::ResponseType::OK) {
        return_to_main_menu();
        if (last_game_id > 0) {
          gameButtons[last_game_id - 1]->activate();
        }
      }
  });
  
  dialog->show();
}

std::string MyGtkWindow::format_score(const int score) {
  std::string score_str = std::to_string(score);
  while (score_str.length() < 7) {
    score_str = "0" + score_str;
  }
  return score_str;
}

void MyGtkWindow::update_game_info() {
  score_value_label.set_markup("<span font_desc='15'>" +
                               format_score(current_state.score) + "</span>");
  hi_score_value_label.set_markup("<span font_desc='15'>" +
                                  format_score(current_state.high_score) +
                                  "</span>");
  speed_value_label.set_markup("<span font_desc='15'>" +
                               std::to_string(current_state.speed) + "</span>");
  level_value_label.set_markup("<span font_desc='15'>" +
                               std::to_string(current_state.level) + "</span>");
}

void GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) {
  (void)width;
  (void)height;
  cr->scale(SCALE, SCALE);
  if (game_field != nullptr) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      for (int x = 0; x < WINDOW_WIDTH; x++) {
        if (game_field[y][x] != 0) {
          COLOR_SWTICH(game_field)
          cr->rectangle(x, y, 1, 1);
          cr->fill();
        }
      }
    }
  }
}

void NextArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) {
  (void)width;
  (void)height;
  cr->scale(SCALE, SCALE);
  if (next_field != nullptr) {
    for (int y = 0; y < NEXT_SIZE; y++) {
      for (int x = 0; x < NEXT_SIZE; x++) {
        if (next_field[y][x] != 0) {
          COLOR_SWTICH(next_field)
          cr->rectangle(x, y, 1, 1);
          cr->fill();
        }
      }
    }
  }
}

// void MyGtkWindow::createDynamicButtons(const std::vector<std::pair<int, std::string>>& games) {
//     for (auto& button : gameButtons) {
//         start_box.remove(*button);
//     }
//     gameButtons.clear();

//     for (const auto& [id, name] : games) {
//       auto btn = Gtk::make_managed<Gtk::Button>(name);
//       btn->signal_clicked().connect([this, id]() {
//           userInput(UserAction_t(id), false);
//           start_game();
//       });
//       start_box.append(*btn);
//         gameButtons.push_back(btn);
//     }
// }

std::vector<std::pair<int, std::string>> MyGtkWindow::parse_game_list (int** matrix) {
  std::vector<std::pair<int, std::string>> gamesList;
  
  if (!matrix) {
      return gamesList;
  }

  for (int i = 0; i < WINDOW_HEIGHT; ++i) {
    int gameNumber = -1;
    std::string gameName;
    
    for (int j = 0; j < WINDOW_WIDTH; ++j) {
      if (matrix[i][j] != 0) {
        gameNumber = matrix[i][j] - '0';
        for (; j < WINDOW_WIDTH; j++){
          if (matrix[i][j] >= 'A' && matrix[i][j] <= 'z') {
            gameName += static_cast<char>(matrix[i][j]);
          }
        }

        if (gameNumber != -1 && !gameName.empty()) {
          gamesList.emplace_back(gameNumber, gameName);
        }
      }
    }
  }
  
  return gamesList;
}

void MyGtkWindow::setup_main_menu() {
  // Очищаем старые кнопки
  for (auto& button : gameButtons) {
      if (button->get_parent() == &start_box) {
          start_box.remove(*button);
      }
      delete button;
  }
  gameButtons.clear();

  // Получаем актуальный список игр
  GameInfo_t state = updateCurrentState();
  auto game_list = parse_game_list(state.field);
  
  // Создаем кнопки для каждой игры
  for (const auto& [id, name] : game_list) {
      auto btn = new Gtk::Button(name);
      btn->signal_clicked().connect([this, id]() {
          userInput(UserAction_t(id), false);
          last_game_id = id;
          start_game();
      });
      start_box.append(*btn);
      gameButtons.push_back(btn);
  }

  auto exit_button = new Gtk::Button("exit");
  exit_button->signal_clicked().connect(
      sigc::mem_fun(*this, &MyGtkWindow::on_exit_button_clicked));
  start_box.append(*exit_button);
  gameButtons.push_back(exit_button);
}

void MyGtkWindow::return_to_main_menu() {
  // Останавливаем таймер игры
  if (timer_connection.connected()) {
      timer_connection.disconnect();
  }
  
  while (Gtk::Widget* child = main_box.get_first_child()) {
    main_box.remove(*child);
  }
  
  setup_main_menu();

  main_box.append(start_box);
}