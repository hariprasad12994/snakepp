#include <curses.h>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>


/*
* cases to be tested
* 1. a and b are overlapping but coordinates of print start point are not
* 2. a and b are overlapping but coordinates of print start point are also overlapping
* 3. a is root and b-d are contained inside the root
* 4. scroll and wrapping for EXAMPL 10, 10, 0, 0  and print hello inside it
*/

using namespace std::chrono_literals;

// 1. refresh box_1
// 2. refresh box_2 alone
// 3. refresh box_1 and box_2 in both orders
void case_1(void) {
  WINDOW* box_1 = newwin(10, 10, 0, 0);
  WINDOW* box_2 = newwin(5, 10, 0, 0);
  box(box_1, 0, 0);
  box(box_2, 0, 0);
  int count = 0;
  while(count++ < 100) {
    mvwprintw(box_1, 1, 1, "hello %d", count);
    std::this_thread::sleep_for(2000ms);
    mvwprintw(box_2, 1, 1, "world %d", count);
    // wrefresh(box_1);
    wrefresh(box_2);
  }
}

// contain b1 and b2 in root
// 1. refresh box_1 alone
// 2. refresh box_2 alone
// 3. refresh box_1 and box_2 in both orders
// 4. refresh root alone -> no rendering of inner box though the inner box overlap with the outer box
// 5. refresh all (order - b1, b2, ob) -> erases border in between
// 5a. refresh all (order - ob, b1, b2) -> proper rendering
// 6. out of bounds write -> no effect, possibly undefined behavior, might crash in other scenarioes
// 7. greater string length -> wraps automatically
// 8. behavior with manual refresh using wnoutrefresh and doupdate -> same as 5, 5a
// 9. touch win with ob as last moves it up the stack
void case_2(void) {
  WINDOW* outer_box = newwin(10, 10, 0, 0);
  // WINDOW* outer_box = newwin(11, 11, 0, 0); // same behavior
  WINDOW* box_1 = newwin(5, 10, 0, 0);
  WINDOW* box_2 = newwin(5, 10, 5, 0);
  box(outer_box, 0, 0);
  box(box_1, 0, 0);
  box(box_2, 0, 0);
  int count = 0;
  while(count++ < 100) {
    mvwprintw(box_1, 1, 1, "hello %d", count);
    mvwprintw(box_2, 1, 1, "world %d", count);
    wnoutrefresh(box_1);
    wnoutrefresh(box_2);
    wnoutrefresh(outer_box);
    doupdate();
    std::this_thread::sleep_for(2000ms);
  }
}

void case_3(void) {
}

int main(int argc, char* argv[]) {
  initscr();
  noecho();
  curs_set(0);
  WINDOW* root = newwin(0, 0, 0, 0);
  box(root, 0, 0);
  wrefresh(root);
  
  std::string test_case(argv[1]);
  
  if(test_case == std::string("case_1")) {
    case_1();
  } else if(test_case == std::string("case_2")) {
    case_2();
  } else if(test_case == std::string("case_3")) {
    case_3();
  } else {
    std::cout << "illegal test_case choice\n";
  }

  delwin(root);
  endwin();
  return 0;
}
