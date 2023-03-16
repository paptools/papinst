#ifndef PATHINST_PATHINST_H
#define PATHINST_PATHINST_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace pathinst {
struct Path {
  std::string name;
  std::vector<std::string> nodes;
  Path(const std::string &name) : name(name), nodes() {}
};

class PathManager {
public:
  static PathManager &Get() {
    static PathManager instance;
    return instance;
  }

  ~PathManager() {
    for (const auto &it : paths_) {
      std::cout << "Path: " << it.first << std::endl;
      for (const auto &node : it.second.nodes) {
        std::cout << node << std::endl;
      }
    }
  }

  void NewPath(const std::string &name) { paths_.emplace(name, name); }

private:
  std::map<std::string, Path> paths_;

  PathManager() {}
};
} // namespace pathinst

#define PATHINST_CALLEE_NODE(p) pathinst::PathManager::Get().NewPath(p)

#endif // PATHINST_PATHINST_H
