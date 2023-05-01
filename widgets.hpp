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

  Box(int row, int column, int left_top_vertex_x, int left_top_vertex_y):
    row(row), column(column), left_top_vertex_x(left_top_vertex_x), left_top_vertex_y(left_top_vertex_y) {}
};


class Cursor {};


class Event {
};


class Element {
public:
  Element() {}
  virtual auto render(const Box& box) -> void = 0;
  virtual auto handle_event(Event event) -> void = 0;
  // todo is_required?
  // virtual auto size() -> unsigned int = 0;

protected:
  // todo how to leverage this, since window has to be created
  // everytime with newwin, also it currently partial refresh is not possible
  WINDOW* logical_region;
};


// Box size is known during rendering and 
// chunks, elemen hierarchy and relationship at compile time
// Spillatble.split(constraints) <= { Elements... }
// .split returns a type
// type on <= operator with n elements return Element*(splittable(constraint, element))


// todo: generalise constraint, now specialising for fixed size hchunk
// algorithm
// todo: separate constraining from chunking
class Constraint {
public:
  Constraint(unsigned int chunk_count): chunk_count(chunk_count) {}

  // todo use window to do actual chunking, replace Box with WINDOW
  auto operator()(const Box& box) -> std::vector<Box> {
    auto chunks = std::vector<Box>();

    // todo: structured binding?
    auto length = box.row;
    auto width = box.column;
    auto top_left_x = box.left_top_vertex_x;
    auto top_left_y = box.left_top_vertex_y;

    auto chunk_length = length / chunk_count;
    for(unsigned int chunk_iter = 0; chunk_iter < chunk_count; ++chunk_iter) {
      chunks.push_back(Box(chunk_length, width, top_left_x + (chunk_iter * chunk_length), top_left_y));
    }

    return chunks;
  }

private:
  unsigned int chunk_count;
};


class Container: public Element {
public:
  Container(Constraint constraint, std::vector<std::unique_ptr<Element>> elems):
    constraint(constraint), children(std::move(elems)) {}

  struct Chunker {
    Constraint constraint;
    Chunker(Constraint constraint): constraint(constraint) {}
    // todo templated function to accept variadic number of elements and construct vector
    // this is to hide the implementation details like using vector and also to provide
    // a syntatic sugar for the lib user
    std::unique_ptr<Element> operator<<=(std::vector<std::unique_ptr<Element>> elems) {
      // todo how to make it better syntatic sugar
      std::unique_ptr<Element> container = std::make_unique<Container>(constraint, std::move(elems));
      return container;
    }
  };

  auto render(const Box& box) -> void override {
    std::for_each(children.cbegin(), children.cend(), [&box](auto const& child) { child->render(box); });
  }

  auto handle_event(Event event) -> void override {
    std::for_each(children.cbegin(), children.cend(), [&event](auto const& child) { child->handle_event(event); });
  }
  
  // Why intermediate types and all the complex mechanism. As of now just to provide the
  // user API interfaces similar to the end goal;
  auto static chunk(unsigned int chunk_count) -> Chunker {
    return Chunker(Constraint(chunk_count));
  }

  // todo: change to private, for test purposes of chunking this has
  // been made public
// private: 
  std::vector<std::unique_ptr<Element>> children;
  Constraint constraint;
};


// class hierarchy to separate from containers from actual widgets like
// text, edit field, menu, canvas etc
class Widget: public Element {
public: 
  Widget() {}
  auto render(const Box& box) -> void override {}
};


// Component is treated as element since it is group of
// elements logically group as a bigger element, kinda a composition
// Component can redirect, propagate events between them and downwards in
// hierarchy. Especially this is good at custom events and also in
// case of basic events shared between elements
// for example handle custom keybindings in the component and pass down 
// keys or mouse event down to the component. Also to pass down events like
// quit. Components also help in propogating events into the core event queue
class Component: public Element {
public:
  auto render(const Box& box) -> void override {}
  auto handle_event(Event event) -> void override {}

private:
  std::unique_ptr<Element> ui_root;
  std::function<void(Event)> handler;
};


class Border {};


class Text: public Widget {
public:
  Text(const std::string& text) {}
  auto render(const Box& box) -> void {}
  auto handle_event(Event event) -> void {}

private:
  std::string text;
};


class NScreen: public Container {
public:
  NScreen(): Container(Constraint(2), std::move(std::vector<std::unique_ptr<Element>>())) {
    initscr();
    clear();
  }
};


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

  auto render(const Box& box_) -> void {
    box(win, 0, 0);
    wrefresh(win);
  }

  ~Window() {
    delwin(win);
    endwin();
  }
};

