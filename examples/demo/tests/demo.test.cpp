#include <climits>

#include <demo/demo.h>

#include <gtest/gtest.h>

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
} // namespace demo
