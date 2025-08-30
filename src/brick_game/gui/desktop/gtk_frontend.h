#ifndef GTK_FRONTEND_H
#define GTK_FRONTEND_H

#include <gtkmm-4.0/gtkmm.h>
#include <vector>
#include <string>

#include "defines.h"
#include "../client_library/game_client.h"

#define Race 1
#define Tetris 2
#define Snake 3

class GameArea : public Gtk::DrawingArea {
 public:
  int **game_field;

  GameArea() : game_field(nullptr) {
    set_draw_func(sigc::mem_fun(*this, &GameArea::on_draw));
  }

  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
};

class NextArea : public Gtk::DrawingArea {
 public:
  int **next_field;

  NextArea() : next_field(nullptr) {
    set_draw_func(sigc::mem_fun(*this, &NextArea::on_draw));
  }
  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
};

class MyGtkWindow : public Gtk::Window {
 public:
  MyGtkWindow();

 private:
  Glib::RefPtr<Gtk::AlertDialog> dialog;

  Gtk::Box main_box;
  // Gtk::Box button_box;
  Gtk::Box start_box;
  // Gtk::Box info_box;

  GameArea *game_area;
  NextArea *next_area;

  sigc::connection timer_connection;

  std::vector<Gtk::Button*> gameButtons;
  int last_game_id;
  Gtk::Button exit_button;

  GameInfo_t current_state;

  Gtk::Label start_label;
  Gtk::Label quit_label;
  Gtk::Label pause_label;
  Gtk::Label action_label;

  Gtk::Label score_label;
  Gtk::Label score_value_label;
  Gtk::Label hi_score_label;
  Gtk::Label hi_score_value_label;
  Gtk::Label next_label;
  Gtk::Label speed_label;
  Gtk::Label speed_value_label;
  Gtk::Label level_label;
  Gtk::Label level_value_label;

 protected:
  std::string format_score(const int score);
  void update_game_info();
  bool update_game();
  void show_game_over_dialog(const Glib::ustring &message);
  bool on_key_press(guint16 keyval, guint, Gdk::ModifierType state);
  void start_game();

  void setup_button_labels(Gtk::Box *button_box);
  void setup_info_box(Gtk::Box *info_box, Gtk::Frame *next_area_frame);
  void setup_game_area_frame(Gtk::Frame *game_area_frame);

  void on_exit_button_clicked();

  void createDynamicButtons(const std::vector<std::pair<int, std::string>>& games);
  std::vector<std::pair<int, std::string>> parse_game_list(int** matrix);
  void return_to_main_menu();
  void on_game_exit_button_clicked();
  void setup_main_menu();
};

#endif  // GTK_FRONTEND_H