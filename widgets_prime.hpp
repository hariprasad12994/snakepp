#include <memory>
#include <vector>
#include <curses.h>
#include <iostream>


class Rect {
public:
  Rect() {}
  Rect(unsigned int rows, unsigned int cols, 
       unsigned int top_left_y, unsigned int top_left_x) :
      win(newwin(rows, cols, top_left_y, top_left_x)),
      top_left_y(top_left_y), top_left_x(top_left_x) {}

  WINDOW* win;
  unsigned int top_left_y;
  unsigned int top_left_x;
};


class _Element {
public:
  _Element() {}
  virtual auto render(void) -> void = 0;
  virtual auto resize(float y_change_ratio, float x_change_ratio) -> void = 0;

protected:
  Rect rect;
};


class Maker {
public:
  virtual auto make_element(const Rect& rect) -> void = 0;
};


class Chunker {
//todo unsigned int shall become a concrete constraint in future
public:
  Chunker(unsigned int chunk_count): chunk_count(chunk_count) {}
  unsigned int chunk_count;
};


class _LHSMaker: public Maker {
  //todo unsigned int shall become a concrete constraint in future
public:
  // todo: private?
  _LHSMaker(std::shared_ptr<Chunker> chunker, std::vector<std::shared_ptr<Maker>> makers) 
    : chunker(chunker), makers(makers) {}

  auto make_element(const Rect& rect) -> void {
    auto chunks = std::vector<Rect>();
    auto rows = 0, cols = 0;
    auto top_left_y = rect.top_left_y, top_left_x = rect.top_left_x;
    getmaxyx(rect.win, rows, cols);

    auto chunk_length = rows / chunker->chunk_count;

    if(chunk_length != makers.size())
      throw "number of elements to be fixed inside the container exceeds the chunks of container";

    for(unsigned int chunk_iter = 0; chunk_iter < chunker->chunk_count; ++chunk_iter) {
      makers[chunk_iter]->make_element(Rect(chunk_length, cols,
                                            top_left_y + (chunk_iter * chunk_length),
                                            top_left_x));
    }
  }

private:
  std::shared_ptr<Chunker> chunker;
  std::vector<std::shared_ptr<Maker>> makers;
};


class _Chunker {
public: 
  _Chunker(std::shared_ptr<Chunker> chunker): chunker(chunker) {}

  std::shared_ptr<Maker> operator<<=(std::vector<std::shared_ptr<Maker>> elems) {
    return std::make_shared<Maker>(_LHSMaker(chunker, elems));
  }

private:
  std::shared_ptr<Chunker> chunker;
};


class LHS: public _Element {
public:
  LHS() {
    initscr();
    noecho();
    
    auto rows = 0, cols = 0;
    getmaxyx(stdscr, rows, cols);
    // todo: refactor "all" constructors for "all" classes
    rect = Rect(rows, cols, 0, 0);
    // todo: by whom and where the refresh is done
    // wrefresh(rect.win);
  }

  auto render(void) -> void {
    // bottom up rendering 
    wrefresh(rect.win);
  }

  std::shared_ptr<_Element> operator<<=(const std::vector<std::shared_ptr<Maker>> makers) {
    return nullptr;
  }

// private:
//   Rect rect;
};


class _LHS: public LHS {
};


class RHS: public _Element {
public:
  RHS() {}

  void operator()(WINDOW* win) {
  }

private:
  std::vector<std::shared_ptr<_Element>> elements;
};


class _RHS: public _LHS, RHS {
};


