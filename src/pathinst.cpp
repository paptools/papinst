#include "pathinst/pathinst.h"

#include <iostream>
#include <string>

namespace pathinst {
namespace {
Node s_root_node("Root Node");
} // namespace

int &GetLevel() {
  static int level = 0;
  return level;
}

std::string GetIndent() { return std::string(GetLevel() * 2, ' '); }

// struct Node {
Node::Node(const std::string &name) : name(name) {
  std::cout << GetIndent() << "[" << GetLevel() << "] Node: " << name
            << std::endl;
  ++(GetLevel());
}

Node::~Node() { --(GetLevel()); }
// } struct Node
} // namespace pathinst
