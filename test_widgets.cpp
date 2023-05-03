#include "widgets.hpp"
#include <bits/chrono.h>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <chrono>
#include <thread>


/*
* Tests to be  explored
* Window resizing 
* Window bordering
* Window out of bounds
* Widget level keystroke event
* Mouse event
* Overlapping windows
* Async keystroke handling
*/


template <typename Widget>
class WidgetTest {
private:
  Screen screen;
  Widget w;

public:
  template<typename ... Args>
  WidgetTest(Args ... args): w(std::forward<Args...>(args...)), screen(Screen()) {}
  virtual void test() = 0;
};


auto test_overlapping_window(void) -> void {
  Constraint c(3);
  Box box(30, 30, 0, 0);
  // std::vector<Box> boxes = c(box);
  // std::for_each(boxes.begin(), boxes.end(), [](auto box) { std::cout << box.row << box.column << box.left_top_vertex_x << box.left_top_vertex_y << '\n';});
  Screen screen;
  int row = screen.screen_length(), column = screen.screen_width();
  int count = 0;
  // Window window(30, 30, 0, 0);
  // std::vector<Window> windows = {
  //   Window(boxes[0].row, boxes[0].column, boxes[0].left_top_vertex_y, boxes[0].left_top_vertex_x),
  //   Window(boxes[1].row, boxes[1].column, boxes[1].left_top_vertex_y, boxes[1].left_top_vertex_x),
  //   Window(boxes[2].row, boxes[2].column, boxes[2].left_top_vertex_y, boxes[2].left_top_vertex_x),
  // };
  // assert(windows[0].win != NULL);
  // assert(windows[1].win != NULL);
  // assert(windows[2].win != NULL);
  Window window(row, column, 0, 0);
  Window another_window(20, 20, 0, 0);
  while(1) {
    // int ch_1 = window.keyboard_input();
    // int ch_2 = another_window.keyboard_input();
    // window.draw_text(25, 25, (std::stringstream() << "count = " << count).str().c_str());
    window.draw_text(10, 10, (std::stringstream() << "count 1 = " << count).str().c_str());
    another_window.draw_text(2, 2, (std::stringstream() << "count 2 = " << count).str().c_str());
    // dummy box input for compilation reason
    another_window.render(Box(0, 0, 0, 0));
    window.render(Box(0, 0, 0, 0));
    using namespace std::chrono_literals;
    // windows[0].draw_text(0, 0, (std::stringstream() << "count = " << count << " one").str());
    // windows[0].draw_text(0, 0, "hello");
    // o << "error_2" << '\n';
    // std::this_thread::sleep_for(2000ms);
    // windows[1].draw_text(0, 0, (std::stringstream() << "count = " << count << " two").str().c_str());
    // std::this_thread::sleep_for(2000ms);
    // windows[2].draw_text(0, 0, (std::stringstream() << "count = " << count << " three").str().c_str());
    std::this_thread::sleep_for(2000ms);
    count++;
    // std::this_thread::sleep_for(2000ms);
    if(count > 100) break;
  }
}

const static std::map<std::string, std::function<void(void)>> test_fn ={
  { "overlapping_window", test_overlapping_window }
};


auto main(int argc, char* argv[]) -> int {
  try {
    namespace opt = boost::program_options;

    opt::options_description desc{"options"};
    desc.add_options()
      ("help,h", "display program usage help")
      ("basic_window_test", "create window")
      ("overlapping_window", "test overlapping window");
    opt::variables_map program_options;
    opt::store(opt::parse_command_line(argc, argv, desc), program_options);
    opt::notify(program_options);

    if(program_options.count("help")) {
      std::cout << desc << '\n';
      return 0;
    }
    test_overlapping_window();

  } catch(const std::exception& exception) {
    std::cerr << exception.what() << '\n';
  }
  return 0;
}
