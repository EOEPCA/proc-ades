#ifndef EOEPCAOWS_PARSER_HPP
#define EOEPCAOWS_PARSER_HPP

#include <eoepca/owl/owsparameter.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <string>

namespace EOEPCA {
class Parser {
  std::string name{"EOEPCA OWS Parser"};

 public:
  Parser() = default;
  Parser(const Parser&) = default;
  Parser(Parser&&) = default;

  ~Parser();

  // Get & Set
 public:

  const std::string& getName() const;

  [[nodiscard]]
  OWS::OWSContext* parseXml(const char *bufferXml, int size);
};

}  // namespace EOEPCA

#endif  // EOEPCAOWS_PARSER_HPP
