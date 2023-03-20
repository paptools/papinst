#ifndef PATHTRACE_PATHTRACE_H
#define PATHTRACE_PATHTRACE_H

#include <memory>
#include <string>

namespace pathtrace {
class Node {
public:
  virtual ~Node() = default;
  virtual const std::string &GetName() const = 0;
};

namespace NodeFactory {
std::unique_ptr<Node> CreateCalleeNode(const std::string &sig);
} // namespace NodeFactory
} // namespace pathtrace

// Utility macros.
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_NODE_NAME TOKENPASTE2(pathinst_node_, __LINE__)

// Instrumentation macros.
#define PATHTRACE_CALLEE_NODE(x)                                               \
  auto UNIQUE_NODE_NAME = pathtrace::NodeFactory::CreateCalleeNode(x);

#endif // PATHTRACE_PATHTRACE_H
