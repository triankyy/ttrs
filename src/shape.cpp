#include "shape.h"
#include "utils.h"
#include <ncurses.h>
#include <random>
#include <string>
#include <vector>

using namespace std;

Shape::Shape() {
  random_device rd;
  mt19937 eng(rd());
  uniform_int_distribution<> distr(1, 7);

  int rand = distr(eng) - 1;
  shape_type[0] = 0;
  shape_type[1] = rand;
}

void Shape::generate(vector<vector<string>> window) {
  dead = false;
  // move old data to new spot
  shape_type[0] = shape_type[1];
  current_win = window;

  int rand = rand_num(1, 7);

  while (rand == shape_type[0])
    // avoid repeating shapes
    rand = rand_num(1, 7);

  shape_type[1] = rand;
  next_up = shape_coords[shape_type[1]];
  next_chars = chars[shape_type[1]];

  selected = shape_coords[shape_type[0]];
  chosen_chars = chars[shape_type[0]];
  color = colors[shape_type[0]];

  // find the height of the shape
  shape_height = 0;

  for (int i = 0; i < 4; i++) {
    vector<bool> line = selected[i];
    bool found = false;

    for (int x = 0; x < line.size(); x++)
      if (i == 1)
        found = true;

    if (found)
      shape_height++;
  }

  default_pos[0] = 0;
  default_pos[1] = 2;
  tr_coord[0] = 0;
  tr_coord[1] = 9;

  // can prob add something here to center shape; ie shapeWidth;
  tr_coord[0] -= shape_height;
  tr_coord[0] -= 2;
  is_dropping = shape_height;
}

vector<int> Shape::char_coords(vector<vector<bool>> shape) {
  vector<int> coords;
  int start_x = tr_coord[0] + default_pos[1] + 1;
  int start_y = tr_coord[1] + default_pos[0];

  for (int i = 0; i < shape.size(); i++) {
    for (int j = 0; j < shape[i].size(); j++) {
      if (shape[i][j]) {
        int x = start_x + i;
        int y = start_y + (2 * j);
        coords.push_back(x);
        coords.push_back(y);
      }
    }
  }
  return coords;
}

void Shape::drop() {
  bool cannotDrop = false;
  vector<int> coords = char_coords(selected);

  for (int i = 0; i < coords.size(); i = i + 2) {
    string check = current_win[coords[i]][coords[i + 1]];
    if (check != " " && is_number(check))
      cannotDrop = true;
  }

  if (cannotDrop) {
    game_over = true;
  } else {
    tr_coord[0]++;
    is_dropping--;
  }
}

void Shape::rotate() {
  vector<vector<bool>> temp(selected.size(),
                            vector<bool>(selected[0].size(), false));

  for (int i = 0; i < selected.size(); i++) {
    for (int j = 0; j < selected[0].size(); j++) {
      temp[temp.size() - j - 1][i] = selected[i][j];
    }
  }

  if (can_rotate(temp)) {
    selected = temp;
    update_shape_height();
  }
}

bool Shape::can_rotate(const vector<vector<bool>> &shape) {
  vector<int> coords = char_coords(shape);

  for (int i = 0; i < coords.size(); i += 2) {
    string chosen = current_win[coords[i] - 1][coords[i + 1] + 2];
    if (chosen != " ") {
      return false;
    }
  }

  return true;
}

void Shape::update_shape_height() {
  shape_height = 0;
  for (int row = 0; row < selected.size(); row++) {
    bool found = false;
    for (int cell = 0; cell < selected[row].size(); cell++) {
      if (selected[row][cell]) {
        found = true;
        break;
      }
    }
    if (found) {
      shape_height++;
    }
  }
}

void Shape::draw() {
  int start_x = tr_coord[0] + default_pos[1];
  int start_y = tr_coord[1] + default_pos[0];

  init_pair(2, color, -1);
  attrset(COLOR_PAIR(2));

  for (int i = 0; i < selected.size(); i++) {
    vector<bool> line = selected[i];

    for (int j = 0; j < 4; j++) {
      if (line[j]) {
        mvprintw(start_x, start_y, "██");
      } else {
        mvprintw(start_x, start_y, "");
      }
      start_y += 2;
    }

    start_x += 1;
    start_y = tr_coord[1] + default_pos[0];
  }

  init_pair(1, COLOR_WHITE, -1);
  attrset(COLOR_PAIR(1));
}

void Shape::check_death() {
  dead = false;
  vector<int> coords = char_coords(selected);

  for (int i = 0; i < coords.size(); i += 2) {
    string check = current_win[coords[i]][coords[i + 1] + 3];
    if (check != " ") {
      dead = true;
      break;
    }
  }
}

void Shape::fall() { tr_coord[0]++; }

void Shape::move(int movetype) {
  vector<int> coords = char_coords(selected);
  bool move = true;

  if (movetype == 1) {
    // Move left
    for (int i = 0; i < coords.size(); i += 2) {
      if (current_win[coords[i] - 1][coords[i + 1]] != " ") {
        move = false;
        break;
      }
    }

    if (move) {
      tr_coord[1] -= 2;
    }
  } else if (movetype == 2) {
    // Move right
    for (int i = 0; i < coords.size(); i += 2) {
      if (current_win[coords[i] - 1][coords[i + 1] + 4] != " ") {
        move = false;
        break;
      }
    }

    if (move) {
      tr_coord[1] += 2;
    }
  } else if (movetype == 3) {
    // Move down
    for (int i = 0; i < coords.size(); i += 2) {
      int y = coords[i + 1];
      if (y >= 18 || current_win[coords[i] + 1][y + 2] != " ") {
        move = false;
        break;
      }
    }

    if (move) {
      tr_coord[0] += 1;
    }
  }
}

void Shape::ground(int framerate) {
  bool moveDown = true;
  while (moveDown) {
    vector<int> coords = char_coords(selected);
    for (int i = 0; i < coords.size(); i += 2) {
      if (current_win[coords[i] + 1][coords[i + 1] + 2] != " ") {
        moveDown = false;
      }
    }

    if (moveDown) {
      tr_coord[0] += 1;
    }
  }
}

vector<int> Shape::ground_coords(vector<vector<bool>> shape, int down) {
  vector<int> coords;
  int start_x = tr_coord[0] + default_pos[1] + down;
  int start_y = tr_coord[1] + default_pos[0];

  for (int i = 0; i < shape.size(); i++) {
    for (int j = 0; j < shape[i].size(); j++) {
      if (shape[i][j]) {
        int x = start_x + i;
        int y = start_y + (2 * j);
        coords.push_back(x);
        coords.push_back(y);
      }
    }
  }
  return coords;
}

void Shape::ground_draw(int down) {
  mvprintw(0, 0, "5");
  int start_x = tr_coord[0] + default_pos[1] + down;
  int start_y = tr_coord[1] + default_pos[0];

  init_pair(2, color, -1);
  attrset(COLOR_PAIR(2));

  for (int i = 0; i < selected.size(); i++) {
    vector<bool> line = selected[i];

    for (int j = 0; j < 4; j++) {
      if (line[j]) {
        mvprintw(start_x, start_y, "░░");
      } else {
        mvprintw(start_x, start_y, "");
      }
      start_y += 2;
    }

    start_x += 1;
    start_y = tr_coord[1] + default_pos[0];
  }

  init_pair(1, COLOR_WHITE, -1);
  attrset(COLOR_PAIR(1));
}

void Shape::show_ground() {
  bool moveDown = true;
  int down = 0;
  while (moveDown) {
    vector<int> coords = ground_coords(selected, down);
    for (int i = 0; i < coords.size(); i += 2) {
      if (current_win[coords[i] + 1][coords[i + 1] + 2] != " ") {
        moveDown = false;
      }
    }

    if (moveDown) {
      down += 1;
    }
  }

  ground_draw(down);
}
