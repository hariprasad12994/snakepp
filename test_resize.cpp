#include "widgets.hpp"
#include <curses.h>
#include <thread>
#include <chrono>
#include <csignal>


using namespace std::chrono_literals;

// todo how the border is padded to resize the element on border

auto main(void) -> int {
  initscr();
  noecho();
  curs_set(0);
  WINDOW* root = newwin(0, 0, 0, 0);
  int original_y = 0, original_x = 0;
  getmaxyx(stdscr, original_y, original_x);
  wtimeout(root, 100);
  box(root, 0, 0);
  wrefresh(root);

  WINDOW* outer_box = newwin(10, 10, 0, 0);
  WINDOW* box_1 = newwin(5, 10, 0, 0);
  WINDOW* box_2 = newwin(5, 10, 5, 0);
  box(outer_box, 0, 0);
  box(box_1, 0, 0);
  box(box_2, 0, 0);
  int count = 0;
  int y = 0, x = 0;
  while(count++ < 1000) {
    int ch = wgetch(root);
    if(ch == KEY_RESIZE) {
      // resizeterm(0, 0);
      // todo: is this needed?
      endwin();

      // resize all windows proportional to the stdscr
      getmaxyx(stdscr, y, x);
      float ratio_y = (float)original_y / (float)y;
      float ratio_x = (float)original_x / (float)x;
      wresize(root, y, x);
      std::cout << original_y << " " << original_x << '\n';
      std::cout << y << " " << x << '\n';
      std::cout << ratio_y << " " << ratio_x << '\n';
      original_y = y;
      original_x = x;
      werase(root);
      werase(outer_box);
      werase(box_1);
      werase(box_2);
      wresize(outer_box, 10 * ratio_y, 10 * ratio_x);
      wresize(box_1, 5 * ratio_y, 10 * ratio_x);
      wresize(box_2, 5 * ratio_y, 10 * ratio_x);
      box(root, 0, 0);
      box(outer_box, 0, 0);
      box(box_1, 0, 0);
      box(box_2, 0, 0);
      // bottom up repaint
      // wrefresh(root);
      // wrefresh(outer_box);
      // wrefresh(box_1);
      // wrefresh(box_2);
      wnoutrefresh(root);
      wnoutrefresh(outer_box);
      wnoutrefresh(box_1);
      wnoutrefresh(box_2);
      doupdate();
    }
    else {
      mvwprintw(box_1, 1, 1, "hello %d", count);
      mvwprintw(box_2, 1, 1, "world %d", count);
      wnoutrefresh(outer_box);
      wnoutrefresh(box_1);
      wnoutrefresh(box_2);
      doupdate();
    }
  }

  delwin(root);
  endwin();

  return 0;
}
