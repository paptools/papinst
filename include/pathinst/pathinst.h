#ifndef PATHINST_PATHINST_H
#define PATHINST_PATHINST_H

#include <memory>
#include <string>

namespace pathinst {
class Node {
public:
  virtual ~Node() = default;
  virtual const std::string &GetName() const = 0;
};

namespace NodeFactory {
std::unique_ptr<Node> CreateCalleeNode(const std::string &sig);
} // namespace NodeFactory
} // namespace pathinst

// Utility macros.
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_NODE_NAME TOKENPASTE2(pathinst_node_, __LINE__)

// Instrumentation macros.
#define PATHINST_CALLEE_NODE(x)                                                \
  auto UNIQUE_NODE_NAME = pathinst::NodeFactory::CreateCalleeNode(x);

#endif // PATHINST_PATHINST_H
