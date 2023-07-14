#include <demo/demo.h>

#include <gtest/gtest.h>

#include <climits>
#include <vector>

namespace demo {
TEST(DemoTest, IsEven) {
  std::vector<int> evens = {0, 2, 4};
  for (int even : evens) {
    EXPECT_TRUE(IsEven(even));
  }

  std::vector<int> odds = {1, 3, 5};
  for (int odd : odds) {
    EXPECT_FALSE(IsEven(odd));
  }

  EXPECT_TRUE(IsEven(INT_MIN));
  EXPECT_FALSE(IsEven(INT_MAX));
}

TEST(DemoTest, Factorial) {
  EXPECT_EQ(Factorial(-1), -1);
  EXPECT_EQ(Factorial(32), -1);

  std::vector<int> expecteds = {1, 1, 2, 6, 24};
  for (int i = 0; i < expecteds.size(); ++i) {
    EXPECT_EQ(Factorial(i), expecteds[i]);
  }

  EXPECT_EQ(Factorial(31), 738197504);
}

TEST(DemoTest, IsPrime) {
  std::vector<int> primes = {2, 3, 5, 7, 11, 29, 59, 89};
  for (int prime : primes) {
    EXPECT_TRUE(IsPrime(prime));
  }

  std::vector<int> not_primes = {0, 1, 4, 6, 9, 25, 57, 91};
  for (int not_prime : not_primes) {
    EXPECT_FALSE(IsPrime(not_prime));
  }
}
} // namespace demo
