#include <climits>

#include <demo/demo.h>

#include <gtest/gtest.h>

namespace demo {
TEST(DemoTest, IsSuccessExitCode) {
  EXPECT_TRUE(IsSuccessExitCode(0));
  EXPECT_FALSE(IsSuccessExitCode(1));
  EXPECT_FALSE(IsSuccessExitCode(INT_MIN));
  EXPECT_FALSE(IsSuccessExitCode(INT_MAX));
}
} // namespace demo
