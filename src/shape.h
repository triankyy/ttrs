#ifndef SHAPE_H
#define SHAPE_H

#include <ncurses.h>
#include <string>
#include <vector>

using namespace std;

class Shape {
public:
  Shape(); // blank constructor

  // shape properties
  int default_pos[2]; // the default position of a shape
  int tr_coord[2];    // the current top right coordinate of the shape
  int shape_height = 0;

  vector<vector<bool>> selected; // the current selected shape
  vector<vector<bool>> next_up;  // the next shape

  vector<int> chosen_chars; // the chosen set of print keys
  vector<int> next_chars;   // the next set of print keys

  vector<vector<vector<bool>>> shape_coords = { // a vector of shapes
      {                                         // the 'o' block
       {0, 0, 0, 0},
       {0, 1, 1, 0},
       {0, 1, 1, 0},
       {0, 0, 0, 0}},
      {// the long block
       {0, 0, 0, 0},
       {1, 1, 1, 1},
       {0, 0, 0, 0},
       {0, 0, 0, 0}},
      {// the l block
       {0, 1, 1, 0},
       {0, 1, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 0, 0}},
      {// the j block
       {0, 1, 1, 0},
       {0, 0, 1, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 0}},
      {// the zag block
       {0, 0, 1, 0},
       {0, 1, 1, 0},
       {0, 1, 0, 0},
       {0, 0, 0, 0}},
      {// the zig block
       {0, 1, 0, 0},
       {0, 1, 1, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 0}},
      {
          // the t block
          {0, 0, 0, 0, 0},
          {0, 0, 1, 0, 0},
          {0, 1, 1, 1, 0},
          {0, 0, 0, 0, 0},
          {0, 0, 0, 0, 0},
      }};

  vector<vector<int>> chars = { // keys for printing each shape
      {1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}, {11, 12}, {13, 14}};

  vector<vector<string>> current_win;

  // vector<int> colors = { COLOR_WHITE, COLOR_WHITE, COLOR_BLUE, COLOR_RED,
  // COLOR_RED, COLOR_BLUE, COLOR_WHITE };
  vector<int> colors = {COLOR_YELLOW, COLOR_CYAN,  COLOR_BLUE,   COLOR_WHITE,
                        COLOR_RED,    COLOR_GREEN, COLOR_MAGENTA};
  int color; // the chosen color in the array

  int shape_type[2];

  // game states
  bool game_over = false;
  bool cannot_move = false;
  bool dead = false;
  int shape_rotation = 1;
  int is_dropping;

  // create a new shape
  void generate(vector<vector<string>> window);

  // methods for changing the shape positions
  void draw();
  void drop();
  void fall();
  void rotate();
  void move(int move_type);
  void ground(int frame_rate);
  vector<int> char_coords(vector<vector<bool>> shape);

  // methods for the ghost tile
  void show_ground();
  void ground_draw(int down);
  vector<int> ground_coords(vector<vector<bool>> shape, int down);

  // updating game states
  void check_death();

  bool can_rotate(const vector<vector<bool>> &shape);
  void update_shape_height();
};

#endif // SHAPE_HPP
