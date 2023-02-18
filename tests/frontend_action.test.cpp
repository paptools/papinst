#include "pathinst/frontend_action.h"

#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

TEST(FrontEndActionTests, RunOnCode_WithReturnStmt_EditBeforeStmt) {
  std::vector<std::string> streams;
  std::string source_code = "int main() { return 0; }";
  std::vector<std::string> parse_args = {};
  bool success = clang::tooling::runToolOnCode(
      std::make_unique<pathinst::FrontendAction>(streams), source_code);
  ASSERT_TRUE(success);
  ASSERT_EQ(streams.size(), 1);
  std::string expected =
      "#include <iostream>\nint main() { std::cout << __FILE__ << ':' << "
      "__LINE__ << std::endl;return 0; }";
  ASSERT_EQ(streams[0], expected);
}
