#include <curses.h>
#include<queue>
#include <string>


class Screen {
private:
  int row;
  int column;

public:
  Screen() {
    initscr();
    getmaxyx(stdscr, row, column);
    clear();
  }

  auto screen_length(void) const -> int { return row; }
  auto screen_width(void) const -> int { return column; }
};


class Cursor {};


class Box {
  int row;
  int column;
  int left_top_vertex_y;
  int left_top_vertex_x;
};


class Widget {
  int row;
  int column;
  int left_top_vertex_y;
  int left_top_vertex_x;
  WINDOW* win;

  Widget() = delete;
  Widget(int length, int width, int left_top_vertex_y_, int left_top_vertex_x_) { 
  }
  virtual void render() = 0;
};


// todo handle SIGWINCH
// todo constructors for fit row, fit column, fit screen ....
// todo: box, hbox, vbox, hsep, vsep
// todo: frame
// todo: container
// todo: notebook, pane
// todo: scroller
// todo: list, menu
// todo: canvas
// todo: radio, label, input, dropdown, checkbox
// todo: slider
// todo: gauge
// todo: tree
// todo: event: keystroker
// todo: event mouse
// todo: event custom
//

class Window {
private:
  int row;
  int column;
  int left_top_vertex_y;
  int left_top_vertex_x;
  WINDOW* win;

public:
  Window(int length, int width, int left_top_vertex_y_, int left_top_vertex_x_) { 
    win = newwin(length, width, left_top_vertex_y_, left_top_vertex_x_);
    left_top_vertex_x = left_top_vertex_x_;
    left_top_vertex_y = left_top_vertex_y_;
    getmaxyx(win, row, column); 
    keypad(win, TRUE);
    noecho();
    curs_set(0);
    wtimeout(win, 100);
    box(win, 0, 0);
    wrefresh(win);
  }

  auto keyboard_input() -> char {
    return wgetch(win);
  }

  auto draw_char(int row, int column, char ch) -> void {
    mvwprintw(win, row, column, "%c", ch);
  }

  auto draw_text(int row, int column, std::string str) -> void {
    mvwprintw(win, row, column, "%s", str.c_str());
  }

  auto erase_char(int row, int column) -> void {
    mvwdelch(win, row, column);
  }
  
  auto erase_window() -> void {
    werase(win);
  }

  auto render(void) -> void {
    box(win, 0, 0);
    wrefresh(win);
  }

  ~Window() {
    delwin(win);
    endwin();
  }
};


class Event { 
};


class App {
private:
  std::queue<Event> event_queue;

public:
  App() = default;
};

