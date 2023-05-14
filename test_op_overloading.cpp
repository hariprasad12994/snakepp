#include <iostream>
#include <memory>
#include <vector>


class B {
public:
  B(unsigned int count) {
    std::cout << "B." << count << '\n';
  }
};


class A {
public:
  A(unsigned int id): id(id){
    std::cout << "A." << id << '\n';
  }

  std::shared_ptr<B> operator<<=(const std::vector<std::shared_ptr<B>>& elems) {
    std::cout << "A." << id << " <<=" << '\n';
    return nullptr;
  }

private:
  unsigned int id;
};


int main(void) {
  auto a = A(1) <<= {
    std::make_shared<B>(1),
    std::make_shared<B>(2),
    std::make_shared<B>(3),
    std::make_shared<B>(4),
    A(2) <<= {
      std::make_shared<B>(5),
      std::make_shared<B>(6),
      std::make_shared<B>(7)
    }
  };
  return 0;
}
