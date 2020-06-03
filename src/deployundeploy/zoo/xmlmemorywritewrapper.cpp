

#include "xmlmemorywritewrapper.hpp"

#include <cstring>

int XmlWriteMemoryWrapper::startDocument(std::string version,
                                         std::string encoding,
                                         std::string standalone) {
  if (writer == nullptr) {
    xmlBuff = xmlBufferCreate();
    if (xmlBuff == nullptr) {
      setError("testXmlwriterMemory: Error creating the xml buffer\n");
      return RETURN_NOT_OK;
    }

    writer = xmlNewTextWriterMemory(xmlBuff, 0);

    if (version.empty() && encoding.empty() && standalone.empty()) {
    } else {
      const char *_encoding{encoding.empty() ? nullptr : encoding.c_str()};
      const char *_version{version.empty() ? nullptr : version.c_str()};
      const char *_standalone{standalone.empty() ? nullptr
                                                 : standalone.c_str()};

      rc = xmlTextWriterStartDocument(writer, _version, _encoding, _standalone);

      if (rc < 0) {
        setError("Can't create the document");
        return RETURN_NOT_OK;
      }
    }

    return RETURN_OK;
  }

  setError("Document started");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::startElement(std::string element, std::string ns,
                                        std::string nsUri) {
  if (writer != nullptr) {
    rc = xmlTextWriterWriteElementNS(
        writer, (ns.empty() ? nullptr : BAD_CAST ns.c_str()),
        BAD_CAST element.c_str(),
        (nsUri.empty() ? nullptr : BAD_CAST nsUri.c_str()), nullptr);

    if (rc < 0) {
      setError("Can't create the Element:" + element);
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::writeAttribute(std::string element,
                                          std::string content) {
  if (writer != nullptr) {
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST element.c_str(),
                                     BAD_CAST content.c_str());
    if (rc < 0) {
      setError("Can't write the element:" + element);
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::writeElement(std::string element,
                                        std::string content, std::string ns,
                                        std::string nsUri) {
  if (writer != nullptr) {
    rc = xmlTextWriterWriteElementNS(
        writer, (ns.empty() ? nullptr : BAD_CAST ns.c_str()),
        BAD_CAST element.c_str(),
        (nsUri.empty() ? nullptr : BAD_CAST nsUri.c_str()),
        BAD_CAST content.c_str());
    if (rc < 0) {
      setError("Can't write the element:" + element);
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::writeContent(std::string content) {
  if (writer != nullptr) {
    rc = xmlTextWriterWriteString(writer, BAD_CAST content.c_str());
    if (rc < 0) {
      setError("Can't write the content:" + content);
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::writeRaw(std::string element) {
  if (writer != nullptr) {
    rc = xmlTextWriterWriteRaw(writer, BAD_CAST element.c_str());
    if (rc < 0) {
      setError("Can't write the element:" + element);
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::endElement() {
  if (writer != nullptr) {
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
      setError("Can't close the last Element:");
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

int XmlWriteMemoryWrapper::endDocument() {
  if (writer != nullptr) {
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
      setError("Can't close the document");
      return RETURN_NOT_OK;
    }

    return RETURN_OK;
  }

  setError("Document did not start");
  return RETURN_NOT_OK;
}

std::string XmlWriteMemoryWrapper::getError() const { return mError; }

int XmlWriteMemoryWrapper::getBuffer(std::stringbuf &buffer) {
  if (xmlBuff != nullptr) {
    std::ostream os(&buffer);
    os << (const char *)xmlBuff->content;
  }

  return RETURN_OK;
}

void XmlWriteMemoryWrapper::setError(std::string error) {
  this->mError = error;
}

/**
 * use this method to convert the input(special char) before set input Value
 *
 * @param in
 * @param encoding
 * @return
 */
xmlChar *XmlWriteMemoryWrapper::ConvertInput(const char *in,
                                             const char *encoding) {
  xmlChar *out{nullptr};
  int ret;
  int size;
  int out_size;
  int temp;
  xmlCharEncodingHandlerPtr handler;

  if (in == 0) return nullptr;

  handler = xmlFindCharEncodingHandler(encoding);

  if (!handler) {
    std::string error{"ConvertInput: no encoding handler found for "};
    error.append(encoding ? encoding : "");
    setError(error);
    return nullptr;
  }

  size = (int)strlen(in) + 1;
  out_size = size * 2 - 1;
  out = (unsigned char *)xmlMalloc((size_t)out_size);

  if (out != 0) {
    temp = size - 1;
    ret = handler->input(out, &out_size, (const xmlChar *)in, &temp);
    if ((ret < 0) || (temp - size + 1)) {
      if (ret < 0) {
        std::string error{"ConvertInput: conversion wasn't successful"};
        setError(error);
      } else {
        std::string error{
            "ConvertInput: conversion wasn't successful. converted: "};
        error.append(std::to_string(temp));
        error.append(" octets.");
        setError(error);
      }

      xmlFree(out);
      out = nullptr;
    } else {
      out = (unsigned char *)xmlRealloc(out, out_size + 1);
      out[out_size] = 0; /*null terminating out */
    }
  } else {
    std::string error{"ConvertInput: no mem"};
    setError(error);
  }

  return out;
}

XmlWriteMemoryWrapper::~XmlWriteMemoryWrapper() {
  if (writer != nullptr) {
    xmlFreeTextWriter(writer);
  }

  if (xmlBuff != nullptr) {
    xmlBufferFree(xmlBuff);
  }
}