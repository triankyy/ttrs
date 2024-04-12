#include "screen.h"
#include "utils.h"
#include <cstring>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

Screen::Screen(int start_level) {
  start_level = start_level;
  level = start_level;

  string line;
  stringstream f(screen_str);
  int line_num = 0;

  while (getline(f, line)) {
    line_num++;
  }

  for (int x = 0; x < line_num; x++) {
    vector<string> row_vec;
    string line = read_line(screen_str, x);
    for (int i = 0; i < line.length(); i++) {
      row_vec.push_back(line.substr(i, 1));
    }
    window.push_back(row_vec);
  }
}

void Screen::top() {
  move(0, 0);
  printw("%s", read_line(screen_str, 0).c_str());
}

void Screen::draw() {
  // Loop through the window and print it
  for (int i = 0; i < window.size(); i++) {
    for (int j = 0; j < window[i].size(); j++) {
      string cur = window[i][j]; // The current character

      // Check if inside the game window or the score/lines display area
      if (is_inside_game_window(i, j)) {
        print_inside_game_window(cur);
      } else {
        // Print characters outside the game window as is
        printw("%s", cur.c_str());
      }
    }
    printw("\n");
  }
}

bool Screen::is_inside_game_window(int i, int j) {
  return ((i >= 1 && i < 19) && (j > 4 && j < 25)) ||
         ((i >= 3 && i < 6) && (j > 33 && j < 43));
}

void Screen::print_inside_game_window(const string &cur) {
  if (cur != " ") {
    int color_num = stoi(cur) / 2 - (stoi(cur) % 2 == 0 ? 1 : 0);
    int color = colors[color_num];
    string print_char = "█";

    // Set the appropriate color
    init_pair(color_num + 3, color, -1);
    attrset(COLOR_PAIR(color_num + 3));

    // Print the character
    printw("%s", print_char.c_str());

    // Reset to default white color
    init_pair(1, COLOR_WHITE, -1);
    attrset(COLOR_PAIR(1));
  } else {
    // Just print a space
    printw(" ");
  }
}

void Screen::add_next(vector<vector<bool>> shape, vector<int> colors) {
  int x = 34;
  int y = 3;

  for (int i = 0; i < 3; i++) {
    vector<bool> line = shape[i];

    for (int j = 0; j < 4; j++) {
      if (line[j]) {
        window[y + i][x + (2 * j)] = to_string(colors[0]);
        window[y + i][x + (2 * j) + 1] = to_string(colors[1]);
      } else {
        window[y + i][x + (2 * j)] = " ";
        window[y + i][x + (2 * j) + 1] = " ";
      }
    }
  }
}

void Screen::update_int_displays(int score, int x, int y) {
  window[x][y] = to_string(score);

  for (int z = 37; z < 42; z++) {
    window[x][z] = " ";
  }

  for (int i = 1; i < to_string(score).length(); i++) {
    window[x][y + i] = "";
  }
}

void Screen::add_shape(Shape shape) {
  for (int i = 0; i < shape.selected.size(); i++) {
    for (int x = 0; x < shape.selected[i].size(); x++) {
      if (shape.selected[i][x]) {
        window[i + shape.tr_coord[0] + shape.default_pos[1]]
              [(2 * x - 1) + shape.tr_coord[1] + shape.default_pos[0] + 3] =
                  to_string(shape.chosen_chars[0]);
        window[i + shape.tr_coord[0] + shape.default_pos[1]]
              [(2 * x) + shape.tr_coord[1] + shape.default_pos[0] + 3] =
                  to_string(shape.chosen_chars[1]);
      }
    }
  }
}

bool Screen::points() {
  // performs point management like adding points to score
  // and returns whether or not to advance to the next level

  // add lines gained this round to line score
  vector<int> fullLines = point_check();
  int linesNew = fullLines.size();
  lines += linesNew;

  // amounts of points gained at different number of clears
  vector<int> pointsPerClear = {40 * (level), 100 * (level), 300 * (level),
                                1200 * level};

  // add points gained this round to point score
  int point;
  if (fullLines.size() == 0)
    point = 0;
  else
    point = pointsPerClear[fullLines.size() - 1];
  score += point;

  // print the stats
  update_int_displays(score, 10, 36);
  update_int_displays(lines, 16, 36);

  // move the lines down
  shift_lines(fullLines);

  // logic for when to advance levels
  if (level == start_level) {
    if (lines > start_level * 10 + 10) {
      window[0][0] = to_string(0);
      advance_level();
      return true;
    }
  } else if ((lines - start_level * 10 + 10) - ((level - start_level) * 10) >=
             10) {
    window[0][0] = to_string(0);
    advance_level();
    return true;
  }

  return false;
}

void Screen::advance_level() {
  level++;
  advancing_level = true; // in order to reduce framerate in mainloop

  // redefine colors
  colors = {COLOR_WHITE, COLOR_WHITE, COLOR_BLUE, COLOR_RED,
            COLOR_RED,   COLOR_BLUE,  COLOR_WHITE};
}

string Screen::get_char(int x, int y) {
  x += default_pos[0];
  y += default_pos[1];

  return window[x][y];
}

vector<int> Screen::point_check() {
  // checks for full lines on board, returns
  // an array of indexes of full lines

  vector<int> full_lines;

  for (int i = 1; i < 19; i++) {
    // loop through lines of board
    bool full = true;

    // add full lines to array
    for (int k = 5; k < 25; k++) {
      const char *chosen = window[i][k].c_str();
      for (int i = 0; i < strlen(chosen); i++) {
        if (!isdigit(chosen[i]))
          full = false;
        if (!full)
          break;
      }
      if (!full)
        break;
    }
    if (full)
      full_lines.push_back(i);
  }

  return full_lines;
}

void Screen::shift_lines(vector<int> lines) {
  // moves lines above full lines down

  for (int i = 0; i < lines.size(); i++)
    // empty full lines
    for (int x = 5; x < 25; x++)
      window[lines[i]][x] = " ";

  usleep(45000);
  move(0, 0);
  refresh();

  // print fullblocks for flashing effect
  if (lines.size() > 0) {
    for (int i = 0; i < lines.size(); i++)
      for (int x = 3; x < 23; x++)
        mvprintw(lines[i], x, "█");

    move(0, 0);
    refresh();
    usleep(45000);
    draw();
    move(0, 0);
    refresh();
    usleep(25000);

    for (int i = 0; i < lines.size(); i++)
      for (int x = 3; x < 23; x++)
        mvprintw(lines[i], x, "█");

    move(0, 0);
    refresh();
    usleep(45000);
  }

  // actually move lines down
  for (int i = 0; i < lines.size(); i++) {
    vector<string> line = window[lines[i]];
    for (int j = lines[i] - 1; j > 0; j--)
      for (int x = 5; x < 25; x++)
        window[j + 1][x] = window[j][x];
  }
}
