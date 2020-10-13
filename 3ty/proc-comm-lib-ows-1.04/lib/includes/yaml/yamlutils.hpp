
#ifndef CODELITHIC_YAMLUTILS_HPP
#define CODELITHIC_YAMLUTILS_HPP

#include <yaml-cpp/yaml.h>
#include <functional>
#include <string>

#define FNCT(T, C) std::bind(T, C, std::placeholders::_1)
#define FNCT2(T, C) \
  std::bind(T, C, std::placeholders::_1, std::placeholders::_2)

class YamlUtils {
 private:
  template <typename T>
  static bool loadVal(T& t, const YAML::Node& node) {
    if (node.IsDefined() && node.IsScalar()) {
      if (node.IsNull()) {
        return true;
      } else {
        t = node.as<T>();
        return true;
      }
    }
    return false;
  }

  template <typename T>
  static bool loadVal(std::function<void(const T&)> p, const YAML::Node& node) {
    T t{};
    auto r = loadVal(t, node);
    if (r) {
      p(t);
    }

    return r;
  }

  template <typename T>
  static bool loadSequenceVal(std::function<void(const T&)> p,
                              const YAML::Node& node) {
    if (isSequenceValid(node)) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        if (isScalarValid(*it)) {
          p((*it).as<T>());
        }
      }
    }
    return true;
  }

 public:
  template <typename T, typename D>
  static void loadMap(std::function<void(T, D)> p,
                      const YAML::Node& node) {
    if (isMapValid(node)) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        if (isScalarValid(it->first) && isScalarValid(it->second)) {
          p(getScalarValid<T>(it->first),
            getScalarValid<D>(it->second));
        }
      }
    }
  }

  template <typename T>
  static void loadValOrSequenceOfVal(std::function<void(const T&)> p,
                                     const YAML::Node& node) {
    if (isScalarValid(node)) {
      p(node.as<T>());
    } else if (isSequenceValid(node)) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        p(getScalarValid<T>(*it));
      }
    }
  }

  template <typename T>
  static bool load(std::function<void(const T&)> p, const YAML::Node& node) {
    return loadVal(p, node);
  }

  template <typename T>
  static bool load(std::function<void(T)> p, const YAML::Node& node) {
    return loadVal(p, node);
  }

  template <typename T>
  static bool loadSequence(std::function<void(T)> p, const YAML::Node& node) {
    return loadSequenceVal(p, node);
  }
  template <typename T>
  static bool loadSequence(std::function<void(const T&)> p,
                           const YAML::Node& node) {
    return loadSequenceVal(p, node);
  }

  template <typename T>
  static bool load(T& t, const YAML::Node& node) {
    return loadVal(t, node);
  }

  template <typename T>
  static T getScalarValid(const YAML::Node& node) {
    T t;
    if (isScalarValid(node)) {
      return node.as<T>();
    }
    return t;
  }

 public:
  static std::string theType(const YAML::Node& node) {
    switch (node.Type()) {
      case YAML::NodeType::Null:
        return "Null";
      case YAML::NodeType::Scalar:
        return "Scalar";
      case YAML::NodeType::Sequence:
        return "Sequence";
      case YAML::NodeType::Map:
        return "Map";
      case YAML::NodeType::Undefined:
        return "Undefined";
    }

    return "Undefined";
  }

  static bool boolean(std::string_view val) {
    if (val.empty()) {
      return false;
    }

    if (val == "true") {
      return true;
    }

    if (val == "false") {
      return false;
    }

    return false;
  }

  static bool isMapValid(const YAML::Node& node) {
    return (node.IsDefined() && node.IsMap() && !node.IsNull());
  }

  static bool isValid(const YAML::Node& node) {
    return (node.IsDefined() && !node.IsNull());
  }

  static bool isSequenceValid(const YAML::Node& node) {
    return (node.IsDefined() && node.IsSequence() && !node.IsNull());
  }

  static bool isScalarValid(const YAML::Node& node) {
    return (node.IsDefined() && node.IsScalar() && !node.IsNull());
  }
};

#endif  // CODELITHIC_YAMLUTILS_HPP
