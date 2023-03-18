#ifndef PATHINST_PATHINST_H
#define PATHINST_PATHINST_H

#include <string>

namespace pathinst {
int &GetLevel();

std::string GetIndent();

struct Node {
  std::string name;

  Node(const std::string &name);
  ~Node();
};
} // namespace pathinst

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_NODE TOKENPASTE2(pathinst_node_, __LINE__)

#define PATHINST_CALLEE_NODE(x) auto UNIQUE_NODE = pathinst::Node(x);

#endif // PATHINST_PATHINST_H
