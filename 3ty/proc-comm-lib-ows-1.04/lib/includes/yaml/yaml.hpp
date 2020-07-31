
#ifndef CODELITHICCWL_YAML_HPP
#define CODELITHICCWL_YAML_HPP

#include <cstring>
#include "object.hpp"
#include <memory>
#include "yamlutils.hpp"

namespace CWL::PARSER {
class YamlToObject {
  std::unique_ptr<char[]> yaml{nullptr};

 private:
  void loadModel(const YAML::Node &node, std::unique_ptr<TOOLS::Object> &father) {
    if (father == nullptr) {
      return;
    }

    if (YamlUtils::isScalarValid(node)) {
      father->setF(node.as<std::string>());
    } else if (YamlUtils::isSequenceValid(node)) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        if (YamlUtils::isScalarValid(*it)) {
          auto e = std::make_unique<TOOLS::Object>();
          e->setF((*it).as<std::string>());
          father->move(e);
        } else {
          auto e = std::make_unique<TOOLS::Object>();
          loadModel(*it, e);
          father->move(e);
        }
      }
    } else if (YamlUtils::isMapValid(node)) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        std::string key;
        if (YamlUtils::isScalarValid(it->first)) {
          key = it->first.as<std::string>();
        }
        if (YamlUtils::isScalarValid(it->second)) {
          if (!key.empty()) {
            auto e = std::make_unique<TOOLS::Object>();
            e->setQlf(key, it->second.as<std::string>());
            father->move(e);
          }
        } else {
          if (!key.empty()) {
            auto e = std::make_unique<TOOLS::Object>();
            e->setQ(key);
            loadModel(it->second, e);
            father->move(e);
          }
        }
      }
    }
  }

 public:
  YamlToObject(const char *cwl, unsigned int cwlSize) {
    if (cwl && cwlSize > 0) {
      cwlSize = cwlSize + 1;
      yaml = std::make_unique<char[]>(cwlSize);
      std::memset(yaml.get(), '\0', cwlSize);
      std::memcpy(yaml.get(), cwl, cwlSize - 1);
    }
  }

  std::unique_ptr<TOOLS::Object> parse() {
    YAML::Node node = YAML::Load(yaml.get());

    auto father = std::make_unique<TOOLS::Object>();
    loadModel(node, father);

    return father;
  }

  YamlToObject(const YamlToObject &) = delete;
  YamlToObject(YamlToObject &&) = delete;
  ~YamlToObject() = default;
};
}  // namespace CWL::PARSER
#endif  // CODELITHICCWL_YAML_HPP
