#include "paptrace/paptrace.h"

// Third-party headers.
#include <nlohmann/json.hpp>

// C++ standard library headers.
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <string>

namespace paptrace {
namespace {
std::stack<Node *> s_node_stack;

class SessionNode : public Node {
public:
  SessionNode() : children_() { Register(); }

  ~SessionNode() {
    Deregister();
    Save();
  }

  nlohmann::json Serialize() const override {
    auto j_children = nlohmann::json::array();
    for (const auto &child : children_) {
      j_children.push_back(child);
    }
    nlohmann::json obj = {{"version", "0.1.0"}, {"traces", j_children}};
    return obj;
  }

  Node *AddParam(const Param &param) override { return this; }

  void AddChild(Node *child) override {
    assert(child);
    children_.push_back(child->Serialize());
  }

private:
  std::list<nlohmann::json> children_;

  void Register() { s_node_stack.push(this); }

  void Deregister() {
    assert(s_node_stack.top() == this);
    s_node_stack.pop();
  }

  void Save() {
    std::ofstream ofs("paptrace.json");
    ofs << std::setw(2) << Serialize() << std::endl;
    std::cout << "Paptace data saved to \"paptrace.json\"." << std::endl;
  }
};
SessionNode s_session_node;

class CallNode : public Node {
public:
  CallNode(const std::string &type, const std::string &sig)
      : type_(type), sig_(sig), params_(), children_() {
    Register();
  }

  ~CallNode() { Deregister(); }

  nlohmann::json Serialize() const override {
    auto j_params = nlohmann::json::array();
    for (const auto &param : params_) {
      j_params.push_back(param.Serialize());
    }
    auto j_children = nlohmann::json::array();
    for (const auto &child : children_) {
      j_children.push_back(child);
    }
    nlohmann::json obj = {
        {"type", type_},
        {"sig", sig_},
        {"params", j_params},
        {"children", j_children},
    };
    return obj;
  }

  Node *AddParam(const Param &param) override {
    params_.push_back(param);
    return this;
  }

  void AddChild(Node *child) override {
    assert(child);
    children_.push_back(child->Serialize());
  }

private:
  const std::string type_;
  const std::string sig_;
  std::list<Param> params_;
  std::list<nlohmann::json> children_;

  void Register() { s_node_stack.push(this); }

  void Deregister() {
    assert(s_node_stack.top() == this);
    s_node_stack.pop();
    assert(s_node_stack.top());
    s_node_stack.top()->AddChild(this);
  }
};

class StmtNode : public Node {
public:
  StmtNode(Node *parent, const std::string &type, int id)
      : parent_(parent), type_(type), id_(id), children_() {
    Register();
  }

  ~StmtNode() { Deregister(); }

  nlohmann::json Serialize() const override {
    auto j_children = nlohmann::json::array();
    for (const auto &child : children_) {
      j_children.push_back(child);
    }
    nlohmann::json obj = {
        {"type", type_},
        {"id", id_},
        {"children", j_children},
    };
    return obj;
  }

  Node *AddParam(const Param &param) override { return this; }

  void AddChild(Node *child) override {
    assert(child);
    children_.push_back(child->Serialize());
  }

private:
  Node *parent_;
  const std::string type_;
  int id_;
  std::list<nlohmann::json> children_;

  void Register() { s_node_stack.push(this); }

  void Deregister() {
    assert(s_node_stack.top() == this);
    s_node_stack.pop();
    assert(s_node_stack.top());
    s_node_stack.top()->AddChild(this);
  }
};
} // namespace

// struct Param {
Param::Param(const std::string &name, const std::string &value)
    : name(name), value(value) {}

nlohmann::json Param::Serialize() const {
  nlohmann::json obj = {{"name", name}, {"value", value}};
  return obj;
}
// } struct Param

// class Node {
std::unique_ptr<Node> Node::Create(const std::string &type,
                                   const std::string &sig) {
  return std::make_unique<CallNode>(type, sig);
}
// } class Node

std::unique_ptr<Node> AddStmt(const std::string &type, int id) {
  assert(s_node_stack.top());
  return std::make_unique<StmtNode>(s_node_stack.top(), type, id);
}
} // namespace paptrace
