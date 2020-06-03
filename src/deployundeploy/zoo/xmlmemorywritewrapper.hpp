
#ifndef PROC_COMM_ZOO_SVC_XMLMEMORYWRITEWRAPPER_HPP
#define PROC_COMM_ZOO_SVC_XMLMEMORYWRITEWRAPPER_HPP

#include <iostream>
#include <sstream>
#include <string>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define RETURN_OK 0
#define RETURN_NOT_OK 1
#define RETURN_HTTP_OK 200

#define RETURN_TRUE true
#define RETURN_FALSE false

class XmlWriteMemoryWrapper {
 public:
  XmlWriteMemoryWrapper() = default;

  virtual ~XmlWriteMemoryWrapper();

  int startDocument(std::string version, std::string encoding,
                    std::string standalone);

  int startElement(std::string element, std::string ns = "",
                   std::string nsUri = "");

  int writeAttribute(std::string element, std::string content);

  int writeElement(std::string element, std::string content,
                   std::string ns = "", std::string nsUri = "");

  int writeContent(std::string content);

  int writeRaw(std::string element);

  int endElement();

  int endDocument();

  int getBuffer(std::stringbuf &buffer);

  xmlChar *ConvertInput(const char *in, const char *encoding);

 private:
  int rc{0};
  xmlTextWriterPtr writer{nullptr};
  xmlBufferPtr xmlBuff{nullptr};

 private:
  bool isReady{false};
  std::string mError{""};

  void setError(std::string error);

 public:
  std::string getError() const;

  XmlWriteMemoryWrapper(const XmlWriteMemoryWrapper &) = delete;

  XmlWriteMemoryWrapper(XmlWriteMemoryWrapper &&) = delete;

 private:
};

#endif  // PROC_COMM_ZOO_SVC_XMLMEMORYWRITEWRAPPER_HPP
