

#ifndef T2CWL_IMOD_TESTS_HPP
#define T2CWL_IMOD_TESTS_HPP

#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>
#include <fstream>
#include <memory>
#include <sstream>

class Util {
  const char* ows1 = "ows2.xml";
  const char* libLinux = "libeoepcaows.dylib";
  const char* libMac = "libeoepcaows.so";

 public:
  std::string getLibName() {
    std::ifstream inLibLinux(libLinux);
    std::ifstream inLibMac(libMac);

    if (inLibLinux.good()) return std::string(libLinux);

    if (inLibMac.good()) return std::string(libMac);

    return "";
  }

  const char* getOws1() const { return ows1; }
  const char* getLibLinux() const { return libLinux; }
  const char* getLibMac() const { return libMac; }
};

#endif