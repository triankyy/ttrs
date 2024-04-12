#include "game.h"
#include "screen.h"
#include "shape.h"
#include <clocale>
#include <iostream>
#include <ncurses.h>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  // curses window setup
  setlocale(LC_CTYPE, "");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();

  int start_level = 8;
  bool easy = false;

  // args handler
  for (int i = 1; i < argc; i++) {
    if (argv[i] == string("--start-level")) {
      start_level = stoi(argv[i + 1]);
    } else if (argv[i] == string("--easy")) {
      easy = true;
    }
  }

  Screen screen(start_level);
  Shape shape;

  string basename = argv[0];
  game(shape, screen, start_level, easy, basename);

  endwin();
  return 0;
}
