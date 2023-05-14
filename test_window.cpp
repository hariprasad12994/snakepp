#include <curses.h>


int main(void) {
  WINDOW* win_1 = newwin(10, 10, 0, 0);
  WINDOW* win_2 = newwin(10, 10, 10, 0);
  initscr(); 
  noecho();
  
  // WINDOW* win_1 = newwin(10, 10, 0, 0);
  // WINDOW* win_2 = newwin(10, 10, 10, 10);
  box(win_1, 0, 0);
  box(win_2, 0, 0);

  wrefresh(win_1);
  wrefresh(win_2);

  int ch = wgetch(win_1);

  delwin(win_1);
  delwin(win_2);
  endwin();

  return 0;
}
