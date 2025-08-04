#include "gtk_frontend.h"

int main(int argc, char **argv) {
  auto app = Gtk::Application::create("org.gtkmm.examples.base");
  return app->make_window_and_run<MyGtkWindow>(argc, argv);
}