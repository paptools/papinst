#ifndef DEMO_DEMO_H
#define DEMO_DEMO_H

namespace demo {

// Returns true if the given value is even.
// Time: O(1)
bool IsEven(int value);

// Returns the factorial of the given value. Returns -1 if the given value is
// negative or would cause an overflow.
// Time: O(n)
int Factorial(int value);

// Returns the Nth Fibonacci number. Returns -1 if the given value is negative
// or would cause an overflow. Time: O(n)
int Fibonacci(int value);
} // namespace demo

#endif // DEMO_DEMO_H
