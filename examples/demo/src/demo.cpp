#include <demo/demo.h>

#include <cmath>

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

bool IsPrime(int n) {
  if (n <= 1) {
    return false;
  }

  for (int i = 2; i <= std::sqrt(n); ++i) {
    if (n % i == 0) {
      return false;
    }
  }

  return true;
}
} // namespace demo
