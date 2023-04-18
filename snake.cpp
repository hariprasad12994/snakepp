#include <curses.h>
#include <sstream>
#include <deque>
#include <ncurses.h>
#include <random>
#include <algorithm>
#include <utility>


enum Direction { UP = 0, RIGHT, DOWN, LEFT, UNDEFINED };


struct Coordinate {
  int row;
  int column;
  Coordinate(int row, int column): row(row), column(column) {}
};


void move_up(Coordinate& current_coordinate) {
  current_coordinate.row -= 1;
}

void move_down(Coordinate& current_coordinate) {
  current_coordinate.row += 1;
}

void move_left(Coordinate& current_coordinate) {
  current_coordinate.column -= 1;
}

void move_right(Coordinate& current_coordinate) {
  current_coordinate.column += 1;
}


class Snake {
public:
  using Body = std::deque<Coordinate>;

  Snake(Coordinate start): body(Body()) {
    body.push_back(Coordinate(start.row, start.column));
  }
  const Body& body_coordinates() const { return body; }

  void grow(const Coordinate& new_coordinate) {
    body.push_back(Coordinate(new_coordinate.row, new_coordinate.column));
  }

  void move(Direction direction) {
    Coordinate snake_head = body.front();
    Coordinate new_head {snake_head.row, snake_head.column};
    switch(direction) {
      case Direction::UP:
        move_up(new_head);
      break;

      case Direction::LEFT:
        move_left(new_head);
      break;

      case Direction::RIGHT:
        move_right(new_head);
      break;

      case Direction::DOWN:
        move_down(new_head);
      break;

      default:
      break;
    }
    body.push_front(new_head);
    body.pop_back();
  }

private:
  Body body;
};


// todo handle SIGWINCH
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


// todo constructors for fit row, fit column, fit screen ....
class Window {
private:
  int row;
  int column;
  WINDOW* win;

public:
  Window(int length, int width, int left_top_vertex_y, int left_top_vertex_x) { 
    win = newwin(length, width, left_top_vertex_y, left_top_vertex_x);
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


auto main(void) -> int {
  Screen screen;
  int row = screen.screen_length(), column = screen.screen_width();

  int score_window_length = row / 4, score_window_width = column;
  int game_play_window_length = row * 3 / 4, game_play_window_width = column;

  Window score_win(score_window_length, score_window_width, 0, 0);
  Window window(game_play_window_length, game_play_window_width, score_window_length, 0);

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> row_distribution(0, game_play_window_length);
  std::uniform_int_distribution<> column_distribution(0, game_play_window_width);

  Coordinate snake_start_coordinate {row_distribution(generator), row_distribution(generator)};
  Coordinate food_start_coordinate {row_distribution(generator), column_distribution(generator)};
  Snake snake(snake_start_coordinate);
  unsigned int score = 0;
  const Snake::Body& body = snake.body_coordinates();
  Direction prev_snake_direction = Direction::UNDEFINED;
  Direction current_snake_direction = Direction::UNDEFINED;

  std::string init_score("Score: 0");
  score_win.draw_text(score_window_length / 2, score_window_width / 2 - init_score.length() / 2, init_score);
  for(auto const& body_part: body)
    window.draw_char(body_part.row, body_part.column, 'o');
  window.draw_char(food_start_coordinate.row, food_start_coordinate.column, 'x');
  window.render();
  score_win.render();

  while(1) {
    int ch = window.keyboard_input();

    switch(ch) {
      case 'i': 
        if(prev_snake_direction != Direction::DOWN)
          current_snake_direction = Direction::UP;
      break;

      case 'j':
        if(prev_snake_direction != Direction::RIGHT)
          current_snake_direction = Direction::LEFT;
      break;

      case 'k':
        if(prev_snake_direction != Direction::UP)
          current_snake_direction = Direction::DOWN;
      break;

      case 'l':
        if(prev_snake_direction != Direction::LEFT)
          current_snake_direction = Direction::RIGHT;
      break;

      case ERR:
      default:
      break;
    }
    
    snake.move(current_snake_direction);
    prev_snake_direction = current_snake_direction;

    const Snake::Body& body = snake.body_coordinates();
    const Coordinate head = snake.body_coordinates().front();

    if(std::find_if(body.begin() + 1, 
                    body.end(), 
                    [&head](auto const& coordinate) { 
                      return head.row == coordinate.row && head.column == coordinate.column;
                    }) != body.end())
    {
      window.erase_window();
      window.draw_text(game_play_window_length / 2, game_play_window_width / 2, std::string("game over...."));
      break;
    }

    if(head.row < 0 || head.column < 0 || head.row > row || head.column > column) {
      window.erase_window();
      std::string game_over_str = "game over...";
      window.draw_text(game_play_window_length / 2, game_play_window_width / 2 - game_over_str.length() / 2, std::string("game over...."));
      break;
    }

    if(head.row == food_start_coordinate.row &&
       head.column == food_start_coordinate.column) {
      score++;
      std::stringstream score_stream; 
      score_stream << "Score: " << score;
      std::string score_string = score_stream.str();
      std::size_t score_string_len = score_string.length();
      score_win.draw_text(score_window_length / 2, score_window_width / 2 - score_string_len / 2, score_string);

      snake.grow(food_start_coordinate);
      // todo check if food is within the range body
      food_start_coordinate.row = row_distribution(generator);
      food_start_coordinate.column = column_distribution(generator);
    }

    // clear();
    window.erase_window(); 
    for(auto const& body_part: body)
      window.draw_char(body_part.row, body_part.column, 'o');
    window.draw_char(food_start_coordinate.row, food_start_coordinate.column, 'x');
    window.render();
    score_win.render();
  }

  while(1) {
    int ch = window.keyboard_input();
    if(ch == 'q') {
      break;
    }
  }

  return 0;
}

