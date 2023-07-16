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

TEST(DemoTest, IsPrime) {
  std::vector<int> primes = {2,  3,  5,  7,  11, 13, 17, 19, 23, 29,
                             31, 37, 41, 43, 47, 53, 59, 61, 67, 71};
  for (int prime : primes) {
    EXPECT_TRUE(IsPrime(prime));
  }

  std::vector<int> not_primes = {0,  1,  4,  6,  10, 16, 18, 20, 25, 30,
                                 32, 36, 42, 44, 48, 55, 60, 64, 68, 72};
  for (int not_prime : not_primes) {
    EXPECT_FALSE(IsPrime(not_prime));
  }
}

TEST(DemoTest, ShiftsToZero) {
  std::vector<int> values = {0, 1, 3, 7, 15, 31, 63, 127};
  std::vector<int> expecteds = {0, 1, 2, 3, 4, 5, 6, 7};
  for (int i = 0; i < values.size(); ++i) {
    EXPECT_EQ(ShiftsToZero(values[i]), expecteds[i]);
  }
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

TEST(DemoTest, NByNIncrements) {
  std::vector<int> values = {0, 1, 2, 4, 8, 16, 32, 64};
  std::vector<int> expecteds = {0, 1, 3, 10, 36, 136, 528, 2080};
  for (int i = 0; i < values.size(); ++i) {
    EXPECT_EQ(NByNIncrements(values[i]), expecteds[i]);
  }
}

TEST(DemoTest, NByNByNIncrements) {
  std::vector<int> values = {0, 1, 2, 4, 8, 16, 32};
  std::vector<int> expecteds = {0, 1, 4, 20, 120, 816, 5984};
  for (int i = 0; i < values.size(); ++i) {
    EXPECT_EQ(NByNByNIncrements(values[i]), expecteds[i]);
  }
}
} // namespace demo
