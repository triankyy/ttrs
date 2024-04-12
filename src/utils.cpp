#include "utils.h"
#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <random>
#include <sstream>

using namespace std;

void check_next(int startLevel, bool easy, string basename) {
  // Read user input for a (y/n) input, restarts game or quits based on input
  int ch = getch();

  // Create a string with CLI options for calling the program again
  string systring = basename + " --start-level " + to_string(startLevel);
  if (easy)
    systring += " --easy";

  // Perform action based on input
  if (ch == 'y') {
    // Call binary
    if (system(systring.c_str()) == -1) {
      cerr << "Error calling '" << systring << "': " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
  } else if (ch == 'n') {
    return; // Exit out of program
  } else {
    // Did not input y or n, try again
    check_next(startLevel, easy, basename);
  }
}

string read_line(string str, int n) {
  // Returns the nth line of a string
  stringstream f(str);
  string s;

  for (int i = 0; i < n; i++) {
    getline(f, s);
  }

  getline(f, s);
  return s;
}

bool is_number(const string &str) {
  // Returns whether the input string is a number
  string::const_iterator it = str.begin();
  while (it != str.end() && isdigit(*it))
    ++it;

  // If iterator got to the end of the string and string is not empty, return
  // true
  return !str.empty() && it == str.end();
}

int rand_num(int start, int end) {
  // Returns random number between start and end
  random_device rd;
  mt19937 eng(rd());
  uniform_int_distribution<> distr(start, end);
  return distr(eng) - 1;
}
