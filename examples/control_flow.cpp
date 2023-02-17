#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>

// Contains implementations of control flow influencing statement types.
namespace statements {
namespace selection {
int if_statement(int x) {
  if (x > 0) {
    --x;
  }
  return x;
}

int if_else_statement(int x) {
  if (x > 0) {
    --x;
  } else if (x < 0) {
    ++x;
  } else {
    x = 100;
  }
  return x;
}

int switch_statement(int x) {
  switch (x) {
  case -1:
    return --x;
    break;
  case 0:
    return x;
    break;
  case 1:
    return ++x;
    break;
  }
  return x;
}
} // namespace selection

namespace iteration {
int while_loop(int x) {
  int y = x - 10;
  while (x != y) {
    --x;
  }
  return x;
}

int do_while_loop(int x) {
  int y = x + 15;
  do {
    ++x;
  } while (x != y);
  return x;
}

int for_loop(int x) {
  for (int i = 0; i < std::abs(x); ++i) {
    --x;
  }
  return x;
}

int range_for_loop(int x) {
  for (int i : {1, 2, 3}) {
    x += i;
  }
  return x;
}
} // namespace iteration

namespace try_blocks {
int try_block(int x) {
  try {
    if (x > 250 || x < -250) {
      throw std::runtime_error("Fake error.");
    }
    return x * -1;
  } catch (const std::exception &) {
    return 123;
  }
}
} // namespace try_blocks
} // namespace statements

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3 || strcmp(argv[1], "-h") == 0 ||
      strcmp(argv[1], "--help") == 0) {
    std::cout << "Usage:\n"
              << "  foo <int>\n"
              << "  foo -h|--help" << std::endl;
    return EXIT_FAILURE;
  }

  int x = std::atoi(argv[1]);

  x = statements::selection::if_statement(x);
  x = statements::selection::if_else_statement(x);
  x = statements::selection::switch_statement(x);

  x = statements::iteration::while_loop(x);
  x = statements::iteration::do_while_loop(x);
  x = statements::iteration::for_loop(x);
  x = statements::iteration::range_for_loop(x);

  x = statements::try_blocks::try_block(x);

  std::cout << "x = " << x << std::endl;
  return EXIT_SUCCESS;
}
