#include <memory>
#include <vector>
#include <curses.h>
#include <iostream>


// todo: refactor "all" ctor for "all" classes, add/del default copy and move ctors
// todo: refactor scopes for "all" fields in "all" classes


// todo: move to _Element as protected?
class Rect {
public:
  Rect() {}
  Rect(WINDOW* win, unsigned int top_left_y, unsigned int top_left_x) :
    win(win), top_left_y(top_left_y), top_left_x(top_left_x) {}
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
  _Element(const Rect& rect): rect(rect) {}
  virtual auto render(void) -> void = 0;
  virtual auto resize(float y_change_ratio, float x_change_ratio) -> void = 0;

protected:
  Rect rect;
};


class Maker {
public:
  virtual auto make_element(const Rect& rect) -> std::shared_ptr<_Element> = 0;
};


class Chunker {
//todo unsigned int shall become a concrete constraint in future
public:
  Chunker(unsigned int chunk_count): chunk_count(chunk_count) {}
  unsigned int chunk_count;

  auto make_chunks(const Rect& rect) -> std::vector<Rect> {
    auto chunks = std::vector<Rect>();
    auto rows = 0, cols = 0;
    auto top_left_y = rect.top_left_y, top_left_x = rect.top_left_x;
    getmaxyx(rect.win, rows, cols);

    auto chunk_length = rows / chunk_count;
    for(unsigned int chunk_iter = 0; chunk_iter < chunk_count; ++chunk_iter) {
      chunks.push_back(Rect(chunk_length, cols,
                            top_left_y + (chunk_iter * chunk_length),
                            top_left_x));
    }
    return chunks;
  }
};


// todo what if maker is an internal class of lhs and made a template in 
// base element class? return maker from a static method> in splittables return
// from chunks method
class _LHS: public _Element {
public:
  class _LHSMaker: public Maker {
    //todo unsigned int shall become a concrete constraint in future
  public:
    // todo: private?
    _LHSMaker(std::shared_ptr<Chunker> chunker, std::vector<std::shared_ptr<Maker>> makers) 
      : chunker(chunker), makers(makers) {}

    auto make_element(const Rect& rect) -> std::shared_ptr<_Element> override {
     // todo: maybe not the right place to throw, maybe in the constraint
     if(chunker->chunk_count != makers.size())
        throw "number of elements to be fixed inside the container exceeds the chunks of container";

      auto chunks = chunker->make_chunks(rect);
      auto box_children = std::vector<std::shared_ptr<_Element>>();
      // todo maybe not the right place for the zip construct
      for(unsigned int chunk_iter = 0; chunk_iter < chunker->chunk_count; ++chunk_iter) {
        box_children.push_back(makers[chunk_iter]->make_element(chunks[chunk_iter]));
      }

      return std::make_shared<_LHS>(box_children);
    }

    std::shared_ptr<Chunker> chunker;
    std::vector<std::shared_ptr<Maker>> makers;
  };

  class _Chunker {
  public: 
    _Chunker(unsigned int count): chunker(std::make_shared<Chunker>(count)) {}

    std::shared_ptr<Maker> operator<<=(std::vector<std::shared_ptr<Maker>> elems) {
      return std::make_shared<_LHSMaker>(_LHSMaker(chunker, elems));
    }

    std::shared_ptr<Chunker> chunker;
  };

  _LHS(std::vector<std::shared_ptr<_Element>> children): children(children) {}

  auto render(void) -> void override {
    wrefresh(rect.win);
    for(auto const& child: children) {
      child->render();
    }
  }

  auto resize(float y_change_ratio, float x_change_ratio) -> void override {
  }

  auto static chunks(unsigned int chunk_count) -> _LHS::_Chunker {
    return _Chunker(chunk_count);
  }

private:
  std::vector<std::shared_ptr<_Element>> children;
};


class LHS {
public:
  // todo maybe a internal class
  class StdScreen {
    StdScreen() {
      initscr();
      noecho();
      curs_set(0);
    }
  };

  struct _Chunker {
    std::shared_ptr<Chunker> chunker;
    
    _Chunker(unsigned int count): chunker(std::make_shared<Chunker>(count)) {}

    std::shared_ptr<LHS> operator<<=(std::vector<std::shared_ptr<Maker>> makers) {
      if(chunker->chunk_count != makers.size())
        throw "number of elements to be fit inside the container exceeds the chunks of container";

      initscr();
      noecho();

      Rect rect(stdscr, 0, 0);
      auto chunks = chunker->make_chunks(rect);
      auto box_children = std::vector<std::shared_ptr<_Element>>();
      for(unsigned int chunk_iter = 0; chunk_iter < chunker->chunk_count; ++chunk_iter) {
        box_children.push_back(makers[chunk_iter]->make_element(chunks[chunk_iter]));
      }

      return std::make_shared<LHS>(rect, std::make_shared<_LHS>(box_children));
    }
  };

  LHS(Rect rect, std::shared_ptr<_LHS> container): rect(rect), container(container) {}

  auto render(void) -> void {
    // bottom up rendering 
    wrefresh(rect.win);
    container->render();
  }

  // auto resize(float y_change_ratio, float x_change_ratio) {
  // todo does the facade need args?
  auto resize(void) -> void {
    // todo: grab old size, maybe two set of win, one is stdscr and another vstdsct
    // compare them for size changes
    auto rows = 0, cols = 0;
    getmaxyx(rect.win, rows, cols);

    float y_change_ratio = 0.0f, x_change_ratio = 0.0f;
    container->resize(y_change_ratio, x_change_ratio);
  }

  auto static chunks(unsigned int chunk_count) -> LHS::_Chunker {
    return LHS::_Chunker(chunk_count);
  }

private:
  std::shared_ptr<_LHS> container;
  Rect rect;
};

// todo seems to be inefficient since there is Maker, static method and a ctor for all elements/widget
class Text: public _Element {
public:
  struct text: public Maker {
    text(std::string txt): txt(txt) {}

    auto make_element(const Rect& rect) -> std::shared_ptr<_Element> override {
      return std::make_shared<Text>(rect, txt);
    }

    std::string txt;
  };

  Text(const Rect& rect, std::string txt): _Element(rect), txt(txt) {}

  auto render(void) -> void override {
    // todo multiple alignment support. as of now default is top right corner
    mvwprintw(rect.win, 0, 0, "%s", txt.c_str());
    // todo: template pattern to hide the wrefresh and ensure the call
    wrefresh(rect.win);
  }

  auto resize(float y_change_ratio, float x_change_ratio) -> void override {
  }

private:
  std::string txt;
};


auto inline text(std::string str) -> std::shared_ptr<Maker> {
  return std::make_shared<Text::text>(str);
}


// todo introduce component level partitioning
// todo event handling, bubbling
// todo better maker classes, subtask - refactor maker, static method, ctor combination
// todo better init list with unique_ptr
// todo optimisation more const, const&
// todo optimisation more constexpr, expecially with respect to tree structure at compile time
// todo optimisation fewer heap allocation

