#ifndef PAPTRACE_PAPTRACE_H
#define PAPTRACE_PAPTRACE_H

// Local headers.
#include "paptrace/utils.h"

// C++ standard library headers.
#include <iostream>
#include <memory>
#include <string>

namespace paptrace {
struct Param {
  const std::string name;
  const std::string value;

  Param(const std::string &name, const std::string &value);
  std::string Serialize() const;
};

class Node {
public:
  // Factory method.
  static std::unique_ptr<Node> Create(const std::string &sig);

  // Virtual destructor.
  virtual ~Node() = default;

  virtual std::string Serialize() const = 0;
  virtual void AddParam(const Param &param) = 0;
  virtual void AddChild(Node *child) = 0;
};

void AddStmt(const std::string &id);
} // namespace paptrace

// Utility macros.
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define NODE_NAME(id) TOKENPASTE2(paptrace_node_, id)

// #define PAPTRACE_TRACE_STMT(x) \
//  paptrace::AddChild(paptrace::NodeFactory::CreateStmtNode(x));

// Instrumentation macros.
#define PAPTRACE_TRACE_CALLEE(id, sig)                                         \
  auto NODE_NAME(id) = paptrace::Node::Create(sig);
#define PAPTRACE_TRACE_PARAM(id, x)                                            \
  NODE_NAME(id)->AddParam(                                                     \
      paptrace::Param(#x, paptrace::utils::PrintToString(x)));
#define PAPTRACE_TRACE_STMT(x) paptrace::AddStmt(x);

#endif // PAPTRACE_PAPTRACE_H
