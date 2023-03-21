#include "paptrace/paptrace.h"

#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

namespace paptrace {
namespace {
nlohmann::json *s_curr_json = nullptr;

nlohmann::json SerializeNode(Node *node) {
  nlohmann::json json;
  json["node"] = node->GetName();
  json["children"] = nlohmann::json::array();
  return json;
}

struct SessionJsonHandler {
  nlohmann::json json;

  SessionJsonHandler() : json() {
    // TODO" Find a way to syncronize this with the version of the library.
    json["version"] = "0.1.0";
    json["nodes"] = nlohmann::json::array();
    s_curr_json = &json["nodes"];
  }

  ~SessionJsonHandler() { std::cout << std::setw(2) << json << std::endl; }
};
SessionJsonHandler s_session_json_handler;

// class MainNode : public Node {
// public:
//   RootNode(const std::string &name) : name_(name), j_(SerializeNode(this)) {
//     s_curr_json = &j_["children"];
//   }
//   virtual ~RootNode() {
//     std::cout << std::setw(2) << j_ << std::endl;
//   }
//   virtual const std::string &GetName() const override { return name_; }
//   json *GetJson() { return &j_; }
//
// private:
//   std::string name_;
//   json j_;
// };
//
// std::unique_ptr<Node> s_root_node = std::make_unique<RootNode>("Root");

class CalleeNode : public Node {
public:
  CalleeNode(const std::string &name)
      : name_(name), parent_json_(s_curr_json), j_(SerializeNode(this)) {
    s_curr_json = &j_["children"];
  }
  virtual ~CalleeNode() {
    parent_json_->push_back(j_);
    s_curr_json = parent_json_;
  }
  virtual const std::string &GetName() const override { return name_; }

private:
  std::string name_;
  nlohmann::json *parent_json_;
  nlohmann::json j_;
};
} // namespace

namespace NodeFactory {
std::unique_ptr<Node> CreateCalleeNode(const std::string &sig) {
  return std::make_unique<CalleeNode>(sig);
}
} // namespace NodeFactory
} // namespace paptrace
