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

std::string GetIndent() {
  return std::string(GetLevel()*2, ' ');
}

// struct Node {
Node::Node(const std::string &name) : name(name) {
  std::cout << GetIndent() << "[" << GetLevel() << "] Node: " << name << std::endl;
  ++(GetLevel());
}

Node::~Node() {
  --(GetLevel());
}
// } struct Node
} // namespace pathinst

//struct Node {
//  std::string name;
//  std::vector<Node> children;
//
//  Node(const std::string &name) : name(name), children() {}
//  Node& AddChild(const std::string &name) { return children.emplace_back(name); }
//};
//
//class NodeManager {
//public:
//  static NodeManager &Get() {
//    static NodeManager instance;
//    return instance;
//  }
//
//  ~NodeManager() {
//    for (const auto &it : paths_) {
//      std::cout << "Path: " << it.first << std::endl;
//      for (const auto &node : it.second.nodes) {
//        std::cout << node << std::endl;
//      }
//    }
//  }
//
//  void AddCallee(const std::string &name) {
//    root_node_.AddChild(name);
//  }
//
//  void NewNode(const std::string &name) { paths_.emplace(name, name); }
//
//private:
//  Node root_node_;
//  Node* curr_node_;
//
//  NodeManager() : root_node_("Root Node"), curr_node_({}
//};

//#define PATHINST_CALLEE_NODE(x) pathinst::NodeManager::Get().AddCallee(x)

