#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include <vector>

#include <ncurses.h>

#include "shape.h"

class Screen {
  // game states
  // bool game_over = false;
  int default_pos[2] = {1, 1};

  // game stats
  int score = 0;
  int lines = 0;
  int level = 0;

public:
  // game states
  int start_level = 1;
  bool advancing_level = false;

  // display variables
  // we might not want to hardcode the border
  const string screen_str =
      "  ┏━━━━━━━━━━━━━━━━━━━━┓\n  ┃                    ┃\n  ┃                 "
      "   ┃   ┏━━next━━━┓\n  ┃                    ┃   ┃         ┃\n  ┃         "
      "           ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃ "
      "                   ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃       "
      "             ┃   ┏━━score━━┓\n  ┃                    ┃   ┃         ┃\n  "
      "┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃      "
      "   ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    "
      "┃\n  ┃                    ┃   ┏━━lines━━┓\n  ┃                    ┃   ┃ "
      "        ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                  "
      "  ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  "
      "┗━━━━━━━━━━━━━━━━━━━━┛\n                 ";
  vector<vector<string>> window;
  vector<int> colors = {COLOR_YELLOW, COLOR_CYAN,  COLOR_BLUE,   COLOR_WHITE,
                        COLOR_RED,    COLOR_GREEN, COLOR_MAGENTA};

  Screen(int start_level); // constructor for generating an array and setting a
                           // start level

  // graphics methods for updating the screen
  void draw();
  void top();
  void update_int_displays(int score, int x, int y);
  string get_char(int x, int y); // returns string at coordinates of screen

  void add_start_level(int start_level); // update level variables
  void advance_level();                  // level up the game

  // void game_over();

  void add_shape(Shape shape);
  vector<int> point_check();
  void shift_lines(vector<int> lines);
  bool points();
  void add_next(vector<vector<bool>> shape, vector<int> color);

  bool is_inside_game_window(int i, int j);
  void print_inside_game_window(const string &cur);
};

#endif // SCREEN_HPP
