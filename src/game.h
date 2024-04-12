#ifndef GAME_H
#define GAME_H

#include "screen.h"
#include "shape.h"
#include <string>

void game(Shape shape, Screen screen, int start_level, bool easy,
          std::string basename);

#endif // !GAME_HPP
