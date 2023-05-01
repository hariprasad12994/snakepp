#include "widgets.hpp"
#include <exception>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>


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
  Screen screen;
  int row = screen.screen_length(), column = screen.screen_width();
  int count = 0;
  Window window(row, column, 0, 0);
  Window another_window(20, 20, 0, 0);
  while(1) {
    int ch = window.keyboard_input();
    window.draw_text(25, 25, (std::stringstream() << "count = " << count).str().c_str());
    another_window.draw_text(2, 2, (std::stringstream() << "count = " << count).str().c_str());
    // dummy box input for compilation reason
    window.render(Box(0, 0, 0, 0));
    // another_window.render(Box(0, 0, 0, 0));
    count++;
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
