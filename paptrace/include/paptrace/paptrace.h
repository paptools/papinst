#ifndef PAPTRACE_PAPTRACE_H
#define PAPTRACE_PAPTRACE_H

#include <memory>
#include <string>

namespace paptrace {
class Node {
public:
  virtual ~Node() = default;
  virtual const std::string &GetName() const = 0;
};

namespace NodeFactory {
std::unique_ptr<Node> CreateCalleeNode(const std::string &sig);
std::unique_ptr<Node> CreateStmtNode(const std::string &stmt_id);
} // namespace NodeFactory
} // namespace paptrace

// Utility macros.
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_NODE_NAME TOKENPASTE2(paptrace_node_, __LINE__)

// Instrumentation macros.
#define PAPTRACE_CALLEE_NODE(x)                                                \
  auto UNIQUE_NODE_NAME = paptrace::NodeFactory::CreateCalleeNode(x);
#define PAPTRACE_STMT_NODE(x)                                                  \
  auto UNIQUE_NODE_NAME = paptrace::NodeFactory::CreateStmtNode(x);

#endif // PAPTRACE_PAPTRACE_H
