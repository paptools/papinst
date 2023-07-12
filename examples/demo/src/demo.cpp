#include <demo/demo.h>

namespace demo {
bool IsEven(int value) { return value % 2 == 0; }

int Factorial(int value) {
  if (value < 0 || value > 31) {
    return -1;
  }

  int result = 1;
  for (int i = 1; i <= value; ++i) {
    result *= i;
  }
  return result;
}
} // namespace demo
