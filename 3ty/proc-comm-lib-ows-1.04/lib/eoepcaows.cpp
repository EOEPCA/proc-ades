
#include <cstring>
#include <eoepca/owl/owsparameter.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "includes/parser.hpp"

extern "C" long version() { return 1; }

extern "C" void getParserName(char* buffer, int maxSize) {
  auto parser = std::make_unique<EOEPCA::Parser>();

  int val = parser->getName().size();
  int len = (maxSize < val ? val : maxSize - 1);

  std::memset(buffer, '\0', maxSize);
  std::strncpy(buffer, parser->getName().c_str(), len);
}

extern "C" EOEPCA::OWS::OWSContext* parseFromFile(const char* fileName) {
  auto parser = std::make_unique<EOEPCA::Parser>();

  std::ifstream infile(fileName);
  if (infile.good()) {
    std::stringstream sBuffer;
    sBuffer << infile.rdbuf();

    return parser->parseXml(sBuffer.str().c_str(), sBuffer.str().size());

  } else {
    std::string err("failed to open ");
    err.append(fileName);
    throw std::runtime_error(err);
  }
}

extern "C" EOEPCA::OWS::OWSContext* parseFromMemory(const char* xmlBuffer,
                                                      int size) {
  auto parser = std::make_unique<EOEPCA::Parser>();
  return parser->parseXml(xmlBuffer, size);
}


extern "C" void releaseParameter(EOEPCA::OWS::OWSContext* parameter) {
  delete parameter;
}
