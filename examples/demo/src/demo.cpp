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

int Fibonacci(int n) {
  if (n < 0 || n > 46) {
    return -1;
  }

  int a = 0;
  int b = 1;
  int c;
  for (int i = 0; i < n; ++i) {
    c = a + b;
    a = b;
    b = c;
  }
  return a;
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
