#ifndef PAPTRACE_PAPTRACE_H
#define PAPTRACE_PAPTRACE_H

// Local headers.
#include "paptrace/utils.h"

// Third-party headers.
#include <nlohmann/json.hpp>

// C++ standard library headers.
#include <initializer_list>
#include <iostream> // IRD TODO: Remove this when no longer needed for debugging.
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
  // Virtual destructor.
  virtual ~Node() = default;

  virtual nlohmann::json Serialize() const = 0;
  virtual Node *AddParam(const Param &param) = 0;
  virtual void AddChild(Node *child) = 0;
};

namespace NodeFactory {
std::unique_ptr<Node> CreateCallNode(int id, const std::string &type,
                                     const std::string &sig);
std::unique_ptr<Node>
CreateCallNode(int id, const std::string &type, const std::string &sig,
               const std::initializer_list<Param> &params);
std::unique_ptr<Node> CreateStmtNode(int id, const std::string &type);
} // namespace NodeFactory
} // namespace paptrace

// Utility macros.
#define NODE_NAME(id) TOKEN_PASTE2(paptrace_node_, id)
#define NODE_DECL(id) auto NODE_NAME(id)
#define TO_PARAM(x) paptrace::Param(#x, TO_STRING(x)),
#define TO_PARAMS(...) FOR_EACH(TO_PARAM, __VA_ARGS__)

// Instrumentation macros.
#define PAPTRACE_CALLEE_NODE(id, sig, ...)                                     \
  NODE_DECL(id) = paptrace::NodeFactory::CreateCallNode(                       \
      id, "CalleeExpr", sig, {TO_PARAMS(__VA_ARGS__)})
#define PAPTRACE_CALLER_NODE(id, sig)                                          \
  paptrace::NodeFactory::CreateCallNode(id, "CallerExpr", sig)
#define PAPTRACE_CALLER_PARAM(x) ->AddParam(paptrace::Param(#x, TO_STRING(x)))
#define PAPTRACE_STMT_NODE(id, x)                                              \
  NODE_DECL(id) = paptrace::NodeFactory::CreateStmtNode(id, x)
#define PAPTRACE_IF_THEN_NODE(id) PAPTRACE_STMT_NODE(id, "IfThenStmt")
#define PAPTRACE_IF_ELSE_NODE(id) PAPTRACE_STMT_NODE(id, "IfElseStmt")

#endif // PAPTRACE_PAPTRACE_H
