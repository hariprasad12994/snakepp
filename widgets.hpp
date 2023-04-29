#include <curses.h>
#include <queue>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>


// todo: handle SIGWINCH
// todo: constructors for fit row, fit column, fit screen ....
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

// Separate box class from the border
// Root is always focused
// Box with dimension control and alignment management
//  Make the dimension auto deduced, is flex needed for boxed elements
// React like component with handler and ui wrapped together
// Mouse is the last priority
// Compile time HTML to C++ dom


/*
* End goal
* auto app = loop.run(Component(handler, screen <= {
*     hbox_handler, hbox.split([constraints...]) <= {
*       Component(text_handler, Text("")
*     }
*   })
* )
*/


// todo: nest this elsewhere inside the class and make it a struct?
class Box {
public:
  // todo Coordinates to be made int and row/column to be made unsigned
  int row;
  int column;
  int left_top_vertex_y;
  int left_top_vertex_x;
};


class Cursor {};


class Event {
};


class Element {
public:
  virtual auto render() -> void = 0;
  virtual auto handle_event(Event event) -> void = 0;
  // todo is_required?
  virtual auto size() -> unsigned int = 0;

protected:
  WINDOW* logical_region;
  Box box;
};


class Container: public Element {
public:
  auto render() -> void override {
    std::for_each(children.cbegin(), children.cend(), [](auto const& child) { child->render(); });
  }

  auto handle_event(Event event) -> void override {
    std::for_each(children.cbegin(), children.cend(), [&event](auto const& child) { child->handle_event(event); });
  }

  auto chunk() -> std::vector<Box> {
    // todo: dummy return remove after logic introduced
    auto chunks = std::vector<Box>();
    auto length = box.row;
    auto width = box.column;
    auto top_left_x = box.left_top_vertex_x;
    auto top_left_y = box.left_top_vertex_y;

    return chunks;
  }

  // todo: change to private, for test purposes of chunking this has
  // been made public
// private: 
  std::vector<std::unique_ptr<Element>> children;
};


class Widget: public Element {
private:
  int row;
  int column;
  int left_top_vertex_y;
  int left_top_vertex_x;
  WINDOW* win;
  
public:
  Widget() = delete;
  Widget(int length, int width, int left_top_vertex_y_, int left_top_vertex_x_) { }
  auto render() -> void override {}
};


class Component: public Element {
public:
  auto render(void) -> void override {}
  auto handle_event(Event event) -> void override {}

private:
  std::unique_ptr<Element> ui_root;
  std::function<void(Event)> handler;
};


class Border {};


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

