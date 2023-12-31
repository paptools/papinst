#include <demo/demo.h>

#include <algorithm>
#include <cmath>

bool IsEven(int value) { return value % 2 == 0; }

int ShiftsToZero(int n) {
  int cnt = 0;
  while (n > 0) {
    n >>= 1;
    ++cnt;
  }
  return cnt;
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

int Factorial(int n) {
  if (n < 0 || n > 31) {
    return -1;
  }

  int result = 1;
  for (int i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}

int QuadraInc(int n) {
  int result = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = i; j < n; ++j) {
      ++result;
    }
  }
  return result;
}

int CubicInc(int n) {
  int result = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = i; j < n; ++j) {
      for (int k = j; k < n; ++k) {
        ++result;
      }
    }
  }
  return result;
}
