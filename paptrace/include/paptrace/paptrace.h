#ifndef PAPTRACE_PAPTRACE_H
#define PAPTRACE_PAPTRACE_H

// Local headers.
#include "paptrace/utils.h"

// Third-party headers.
#include <nlohmann/json.hpp>

// C++ standard library headers.
#include <iostream>
#include <memory>
#include <string>

namespace paptrace {
struct Param {
  const std::string name;
  const std::string value;

  Param(const std::string &name, const std::string &value);
  nlohmann::json Serialize() const;
};

class Node {
public:
  // Factory method.
  static std::unique_ptr<Node> Create(const std::string &sig);

  // Virtual destructor.
  virtual ~Node() = default;

  virtual nlohmann::json Serialize() const = 0;
  virtual void AddParam(const Param &param) = 0;
  virtual void AddChild(Node *child) = 0;
};

void AddStmt(const std::string &type, int id);
} // namespace paptrace

// Utility macros.
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define NODE_NAME(id) TOKENPASTE2(paptrace_node_, id)

// Instrumentation macros.
#define PAPTRACE_CALLEE_NODE(id, sig)                                          \
  auto NODE_NAME(id) = paptrace::Node::Create(sig)
#define PAPTRACE_TRACE_PARAM(id, x)                                            \
  NODE_NAME(id)->AddParam(                                                     \
      paptrace::Param(#x, paptrace::utils::PrintToString(x)))
#define PAPTRACE_IF_THEN_STMT(x) paptrace::AddStmt("IfThenStmt", x)
#define PAPTRACE_IF_ELSE_STMT(x) paptrace::AddStmt("IfElseStmt", x)
#define PAPTRACE_TRACE_STMT(x, id) paptrace::AddStmt(x, id)
#define PAPTRACE_CALLER_NODE(id, sig)                                          \
  auto NODE_NAME(id) = paptrace::Node::Create(sig)

#endif // PAPTRACE_PAPTRACE_H
