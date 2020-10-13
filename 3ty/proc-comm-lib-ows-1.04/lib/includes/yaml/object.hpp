
#ifndef CODELITHICCWL_OBJECT_HPP
#define CODELITHICCWL_OBJECT_HPP

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace TOOLS {

using Qlf = std::pair<std::string, std::string>;

class Object {
 protected:
  Qlf qlf_;
  std::list<std::unique_ptr<Object>> objects_{};

 public:
  Object() = default;
  Object(const Object &) = delete;
  Object(Object &&) = delete;
  ~Object() = default;

 public:
  Object &operator=(const Object &other) {
    if (this != &other) {
      this->qlf_ = other.qlf_;
      for (auto &a : other.objects_) {
        auto *o = new Object();
        *o = *a;
        this->objects_.emplace_back(o);
      }
    }
    return *this;
  }

  std::optional<std::string> getKey(const std::string &theKey) const {
    if (qlf_.first == theKey) {
      return qlf_.second;
    }
    for (auto &l : objects_) {
      if (l->qlf_.first == theKey) {
        return l->qlf_.second;
      }
    }
    return std::nullopt;
  }

  bool isQlfEmpty() const { return qlf_.first.empty() && qlf_.second.empty(); }

  const std::pair<std::string, std::string> &getQlf() const { return qlf_; }
  void setQlf(const Qlf &qlf) { qlf_ = qlf; }
  void setQlf(const std::string &q, const std::string &f) {
    qlf_.first = q;
    qlf_.second = f;
  }

  bool isQ(const std::string &name) { return qlf_.first == name; }

  const std::string &getQ() const { return qlf_.first; }
  void setQ(std::string q) { qlf_.first = std::move(q); }

  const std::string &getF() const { return qlf_.second; }
  void setF(std::string f) { qlf_.second = std::move(f); }

  bool hasChildren() const { return objects_.size(); }
  const std::list<std::unique_ptr<Object>> &getChildren() const {
    return objects_;
  }

  void moveChildren(std::unique_ptr<Object> &object) {
    if (object) {
      objects_.emplace_back(std::move(object));
    }
  }

  void moveChildren(Object *object) {
    if (object) {
      objects_.emplace_back(object);
    }
  }

  void addChildren(const Object *object) {
    if (object) {
      auto *m = new Object();
      *m = *object;

      objects_.emplace_back(m);
    }
  }

  void move(Object *object) { objects_.emplace_back(object); }
  void move(std::unique_ptr<Object> &object) {
    objects_.emplace_back(object.release());
  }

  bool isArray() const {
    return (!objects_.empty()) && qlf_.first.empty() && qlf_.second.empty();
  }

  std::string findAndReturnF(const std::string &id, const std::string &value,
                             bool depth = true) const {
    auto t_ = find(id, value, depth);
    if (t_) return t_->getF();

    return std::string();
  }

  const Object *find(const std::string &id, const std::string &value,
                     bool depth = true) const {
    if (qlf_.first == id && value.empty()) {
      return this;
    }

    if (qlf_.first == id && qlf_.second == value) {
      return this;
    }

    for (auto &m : objects_) {
      if (m->qlf_.first == id && value.empty()) {
        return m.get();
      }

      if (m->qlf_.first == id && m->qlf_.second == value) {
        return this;
      }

      if (depth) {
        auto k = m->find(id, value);
        if (k) {
          return k;
        }
      }
    }

    return nullptr;
  }
};

}  // namespace TOOLS

#endif  // CODELITHICCWL_OBJECT_HPP
