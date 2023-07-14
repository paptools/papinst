#ifndef DEMO_DEMO_H
#define DEMO_DEMO_H

namespace demo {

// Returns true if the given value is even.
// Time: O(1)
bool IsEven(int value);

// Returns the factorial of the given value. Returns -1 if the given value is
// negative or would cause an overflow.
// Time: O(n)
int Factorial(int n);

// Returns true if the given value is prime.
// Time: O(sqrt(n))
bool IsPrime(int n);

// Returns the value of N x N increments.
// Time: O(n^2)
int NByNIncrements(int n);

// Returns the value of N x N x N increments.
// Time: O(n^3)
int NByNByNIncrements(int n);
} // namespace demo

#endif // DEMO_DEMO_H
