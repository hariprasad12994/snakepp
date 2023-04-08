#include <deque>
#include <ncurses.h>
#include <random>
#include <algorithm>


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


class Window {
private:
  int row;
  int column;

public:
  Window() { 
    initscr();
    getmaxyx(stdscr, row, column); 
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(100);

    // std::random_device random_device;
    // std::mt19937 generator(random_device());
    // std::uniform_int_distribution<> row_distribution(0, row);
    // std::uniform_int_distribution<> column_distribution(0, column);
    // Coordinate snake_start_coordinate {row_distribution(generator), column_distribution(generator)};
    // mvprintw(snake_start_coordinate.row, snake_start_coordinate.column,  "%c", 'o');
    // refresh();
  }

  ~Window() { endwin(); }
};


auto main(void) -> int {
  Window window;

  int row = 0, column = 0;
  getmaxyx(stdscr, row, column); 
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> row_distribution(0, row);
  std::uniform_int_distribution<> column_distribution(0, column);

  Coordinate snake_start_coordinate {row_distribution(generator), column_distribution(generator)};
  Coordinate food_start_coordinate {row_distribution(generator), column_distribution(generator)};
  Snake snake(snake_start_coordinate);
  const Snake::Body& body = snake.body_coordinates();
  for(auto const& body_part: body)
    mvprintw(body_part.row, body_part.column,  "%c", 'o');
  mvprintw(food_start_coordinate.row, food_start_coordinate.column,  "%c", 'x');
  Direction prev_snake_direction = Direction::UNDEFINED;
  Direction current_snake_direction = Direction::UNDEFINED;
  refresh();
  
  while(1) {
    int ch = getch();

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
      break;
    }

    if(head.row < 0 || head.column < 0 || head.row > row || head.column > column)
      break;

    if(head.row == food_start_coordinate.row &&
       head.column == food_start_coordinate.column) {
      // check if food is within the range body
      snake.grow(food_start_coordinate);
      food_start_coordinate.row = row_distribution(generator);
      food_start_coordinate.column = column_distribution(generator);
    }

    clear();
    for(auto const& body_part: body)
      mvprintw(body_part.row, body_part.column,  "%c", 'o');
    mvprintw(food_start_coordinate.row, food_start_coordinate.column,  "%c", 'x');
    refresh();
  }

  return 0;
}

