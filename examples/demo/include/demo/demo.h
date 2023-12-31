#ifndef DEMO_DEMO_H
#define DEMO_DEMO_H

// Functions are listed in order of increasing time complexity.

// Returns true if the given value is even.
// Time: O(1)
bool IsEven(int value);

// Returns the number of right shifts needed to convert the given value to zero.
// Time: O(log(n))
int ShiftsToZero(int n);

// Returns true if the given value is prime.
// Time: O(sqrt(n))
bool IsPrime(int n);

// Returns the factorial of the given value.
// Returns -1 if the given value is negative or would cause an overflow.
// Time: O(n)
int Factorial(int n);

// Returns the value of N x N increments.
// Time: O(n^2)
int QuadraInc(int n);

// Returns the value of N x N x N increments.
// Time: O(n^3)
int CubicInc(int n);

#endif // DEMO_DEMO_H
