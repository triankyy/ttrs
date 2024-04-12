#include "game.h"
#include "screen.h"
#include "shape.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

void game(Shape shape, Screen screen, int startLevel, bool easy,
          string basename) {
  // General game constants
  int framerate = 24;
  framerate -= startLevel;
  bool newShape = true;
  int count = 0;
  unsigned int microseconds = 10000;
  int restingCursor[2] = {
      23, 22}; // In case the terminal doesn't support invisible cursor

  while (!shape.game_over) {
    if (screen.advancing_level) {
      // Change the colors for the level
      if (framerate > 2) {
        framerate--;
      }
      shape.colors = screen.colors;
      screen.advancing_level = false;
    }

    if (newShape) {
      shape.generate(screen.window);
      screen.add_next(shape.next_up, shape.next_chars);
      shape.drop();
      if (shape.game_over) {
        break;
      }
      newShape = false;
    } else if ((count + 1) % framerate == 0) {
      shape.check_death();
    } else if (count % framerate == 0) {
      if (shape.is_dropping > 0) {
        shape.drop();
      } else {
        shape.fall();
        if (shape.cannot_move) {
          newShape = true;
        }
      }
    }

    usleep(microseconds);

    int ch = getch();
    bool ground = false;
    bool breake = false;

    if (ch) {
      if (ch == KEY_UP) {
        shape.rotate();
      } else if (ch == KEY_DOWN) {
        shape.move(3);
      } else if (ch == KEY_LEFT) {
        shape.move(1);
      } else if (ch == KEY_RIGHT) {
        shape.move(2);
      } else if (ch == ' ') {
        ground = true;
      } else if (ch == 'e') {
        easy = !easy; // Turn on or off guide mode
      } else if (ch == 'r') {
        breake = true; // Instant restart
      }
    }

    if (ground) {
      shape.ground(framerate); // Hard drop the shape
    }

    if (breake) {
      break; // Take the game to the death screen
    }

    screen.draw();
    if (easy) {
      shape.show_ground();
    }
    shape.draw();
    screen.top();
    wmove(stdscr, 0, 0);
    count++;

    if (shape.dead) {
      newShape = true;
      screen.add_shape(shape);
      if (screen.points()) {
        shape.colors = screen.colors;
      }
    }
  }

  nodelay(stdscr, FALSE);
  wrefresh(stdscr);

  for (int i = 8; i < 12; i++) {
    for (int j = 3; j < 23; j++) {
      mvprintw(i, j, " ");
    }
  }

  mvprintw(11, 2, "┣");
  mvprintw(8, 2, "┣");
  for (int i = 3; i < 24; i++) {
    mvprintw(8, i, "━");
    mvprintw(11, i, "━");
  }
  mvprintw(11, 23, "┫");
  mvprintw(8, 23, "┫");
  screen.top();
  mvprintw(9, 8, "Game over!");
  mvprintw(10, 5, "Try again? (y/n)");
  check_next(startLevel, easy, basename); // Wait for input
}
