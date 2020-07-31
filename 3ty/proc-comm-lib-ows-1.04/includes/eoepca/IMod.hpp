
#ifndef T2CWL_IMOD_HPP
#define T2CWL_IMOD_HPP

#include <dlfcn.h>

#include <stdexcept>
#include <string>

namespace mods {
class IModInterface {
  bool valid{false};

 protected:
 public:
  IModInterface() = delete;

  IModInterface(const IModInterface &) = delete;

  explicit IModInterface(std::string path) : path_(std::move(path)) {
    handle = dlopen(path_.data(), RTLD_LAZY);
    if (!handle) {
      handle = nullptr;
      setValid(false);
      setLastError(std::string(dlerror()));
    }
    setValid(true);
    resetError();
  }

  virtual ~IModInterface() {
    if (handle != nullptr) {
      dlclose(handle);
      handle = nullptr;
      resetError();
    }
  }

  bool isValid() const { return valid; }
  void setValid(bool val) { IModInterface::valid = val; }

  const std::string &getLastError() const { return lastError; }
  void resetError() { lastError.clear(); }
  void setLastError(std::string strError) {
    IModInterface::lastError = std::move(lastError);
  }

 protected:
  std::string lastError{""};
  void *handle{nullptr};

 private:
  std::string path_{""};
};
}  // namespace mods

#endif  // T2CWL_IMOD_HPP
