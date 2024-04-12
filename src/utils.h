#ifndef UTILS_H
#define UTILS_H

#include <string>

void check_next(int startLevel, bool easy, std::string basename);
std::string read_line(std::string str, int n);
bool is_number(const std::string &s);
int rand_num(int start, int end);

#endif // UTILS_H
