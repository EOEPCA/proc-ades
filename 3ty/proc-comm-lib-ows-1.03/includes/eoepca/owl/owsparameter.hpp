
#ifndef EOEPCAOWS_OWSPARAMETER_HPP
#define EOEPCAOWS_OWSPARAMETER_HPP

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>

namespace EOEPCA::OWS {

class Descriptor {
 protected:
  std::string identifier{""};
  std::string title{""};
  std::string abstract{""};
  std::string version{""};
  std::string tag{""};
  bool isArray{false};

 public:
  Descriptor() = default;
  Descriptor(const Descriptor &other) noexcept(false)
      : identifier(other.identifier),
        title(other.title),
        version(other.version),
        abstract(other.abstract),
        tag(other.tag),
        isArray(other.isArray) {}

  Descriptor(Descriptor &&other) noexcept(false)
      : identifier(std::move(other.identifier)),
        title(std::move(other.title)),
        abstract(std::move(other.abstract)),
        version(std::move(other.version)),
        tag(std::move(other.tag)),
        isArray(other.isArray) {}

  Descriptor &operator=(const Descriptor &other) {
    if (this == &other) return *this;

    identifier = other.identifier;
    title = other.title;
    abstract = other.abstract;
    version = other.version;
    tag = other.tag;
    isArray = other.isArray;

    return *this;
  }

  Descriptor &operator=(Descriptor &&other) noexcept(false) {
    if (this == &other) return *this;

    identifier = std::move(other.identifier);
    title = std::move(other.title);
    abstract = std::move(other.abstract);
    version = std::move(other.version);
    tag = std::move(other.tag);
    isArray = other.isArray;

    return *this;
  }

  virtual ~Descriptor() = default;

 public:
  void setVersion(std::string pVersion) {
    Descriptor::version = std::move(pVersion);
  }

  const std::string &getVersion() const { return version; }

  const std::string &getIdentifier() const { return identifier; }
  void setIdentifier(std::string pIdentifier) {
    Descriptor::identifier = std::move(pIdentifier);
  }
  const std::string &getTitle() const { return title; }
  void setTitle(const std::string &title) { Descriptor::title = title; }
  const std::string &getAbstract() const { return abstract; }
  void setAbstract(const std::string &abstract) {
    Descriptor::abstract = abstract;
  }

  const std::string &getTag() const { return tag; }
  void setTag(const std::string &tag) { Descriptor::tag = tag; }

  bool isArrayVal() const { return isArray; }
  void setIsArray(bool isArray) { Descriptor::isArray = isArray; }
};

class Occurs {
 protected:
  int minOccurs{1};
  int maxOccurs{0};

 public:
  Occurs() = default;

  Occurs(const Occurs &) = default;
  Occurs(Occurs &&) = default;

  Occurs &operator=(const Occurs &other) {
    if (this == &other) return *this;
    minOccurs = other.minOccurs;
    maxOccurs = other.maxOccurs;
    return *this;
  }

  Occurs &operator=(Occurs &&other) noexcept(false) {
    if (this == &other) return *this;
    minOccurs = other.minOccurs;
    maxOccurs = other.maxOccurs;
    return *this;
  }

  virtual ~Occurs() = default;

 public:
  int getMinOccurs() const { return minOccurs; }
  int getMaxOccurs() const { return maxOccurs; }

  void setMinOccurs(std::string pMinOccurs) {
    if (pMinOccurs.empty()) {
      minOccurs = 0;
    } else {
      std::istringstream iss(pMinOccurs);
      iss >> minOccurs;
      if (iss.fail()) {
        // TODO: raise the exception
      }
    }
  }

  void setMaxOccurs(std::string pMaxOccurs) {
    if (pMaxOccurs.empty()) {
      maxOccurs = 0;
    } else {
      std::istringstream iss(pMaxOccurs);
      iss >> maxOccurs;
      if (iss.fail()) {
        // TODO: raise the exception
      }
    }
  }

  void setMinOccurs(int minOccurs) { Occurs::minOccurs = minOccurs; }
  void setMaxOccurs(int maxOccurs) { Occurs::maxOccurs = maxOccurs; }
};

class Param : public Descriptor, public Occurs {
 public:
  Param() : Descriptor(), Occurs(){};
  Param(const Param &other) noexcept(false)
      : Descriptor(other), Occurs(other) {}
  Param(Param &&other) noexcept(false)
      : Descriptor(std::move(other)), Occurs(std::move(other)) {}

  Param &operator=(const Param &other) {
    if (this == &other) return *this;

    Descriptor::operator=(other);
    Occurs::operator=(other);

    return *this;
  }

  Param &operator=(Param &&other) noexcept(false) {
    if (this == &other) return *this;

    Descriptor::operator=(std::move(other));
    Occurs::operator=(std::move(other));

    return *this;
  }

  Param &operator<<(const Descriptor &descriptor) {
    setIdentifier(descriptor.getIdentifier());
    setTitle(descriptor.getTitle());
    setAbstract(descriptor.getAbstract());

    return *this;
  }

  Param &operator<<(const Occurs &occurs) {
    setMinOccurs(occurs.getMinOccurs());
    setMaxOccurs(occurs.getMaxOccurs());
    return *this;
  }

  ~Param() override = default;

  virtual std::string getType() = 0;
};

class Supported {
  std::list<std::string> supported{};
  std::string defaultValue{""};

 public:
  Supported() = default;

  Supported(const Supported &other) : defaultValue(other.defaultValue) {
    for (auto &s : other.supported) {
      supported.emplace_back(s);
    }
  }

  Supported(Supported &&other) noexcept(false)
      : defaultValue(std::move(other.defaultValue)) {
    for (auto &s : other.supported) {
      supported.emplace_back(s);
    }
    other.supported.clear();
  }

  virtual ~Supported() = default;

 public:
  void addSupportedValues(std::string value) {
    supported.emplace_back(std::move(value));
  }

  void setDefault(std::string value) { defaultValue = std::move(value); }

  const std::list<std::string> &getSupported() const { return supported; }
  const std::string &getDefaultValue() const { return defaultValue; }
};

class Values {
  std::list<std::string> allowedValues{};
  std::string defaultValue{""};

 public:
  Values() = default;

  Values(const Values &other) : defaultValue(other.defaultValue) {
    for (auto &s : other.allowedValues) {
      allowedValues.emplace_back(s);
    }
  }

  Values(Values &&other) noexcept(false)
      : defaultValue(std::move(other.defaultValue)) {
    for (auto &s : other.allowedValues) {
      allowedValues.emplace_back(s);
    }
    other.allowedValues.clear();
  }

  virtual ~Values() = default;

 public:
  void addAllowedValues(std::string value) {
    allowedValues.emplace_back(std::move(value));
  }

  void setDefault(std::string value) { defaultValue = std::move(value); }

  const std::list<std::string> &getAllowedValues() const {
    return allowedValues;
  }
  const std::string &getDefaultValue() const { return defaultValue; }
};

class LiteralData final : public Param, public Values {
  std::string dataType{"string"};

 public:
  //  LiteralData():Param(),Values(){}

  LiteralData() = default;

  LiteralData(const LiteralData &) = delete;
  LiteralData(LiteralData &&) = delete;

  ~LiteralData() override = default;

 public:
  std::string getType() override { return std::string("LiteralData"); }
  void setDataType(std::string data) { dataType = std::move(data); }
  const std::string &getDataType() const { return dataType; }
};

class BoundingBoxData final : public Param, public Supported {
 public:
  BoundingBoxData() = default;

  BoundingBoxData(const BoundingBoxData &) = delete;
  BoundingBoxData(BoundingBoxData &&) = delete;

  ~BoundingBoxData() override = default;

 public:
  std::string getType() override { return std::string("BoundingBoxData"); }
};

class Format {
  std::string mimeType{""};
  std::string encoding{""};
  std::string schema{""};

 public:
  Format() = default;
  explicit Format(std::string sMimeType)
      : mimeType(sMimeType), encoding(""), schema("") {}
  Format(const LiteralData &) = delete;
  Format(Format &&) = delete;

  ~Format() {}

  Format &operator=(const Format &other) {
    if (this != &other) {
      mimeType = other.mimeType;
      encoding = other.encoding;
      schema = other.schema;
    }
    return *this;
  }

  void setMimeType(std::string pMimeType) {
    Format::mimeType = std::move(pMimeType);
  }
  void setEncoding(std::string pEncoding) {
    Format::encoding = std::move(pEncoding);
  }
  void setSchema(std::string pSchema) { Format::schema = std::move(pSchema); }

  const std::string &getMimeType() const { return mimeType; }
  const std::string &getEncoding() const { return encoding; }
  const std::string &getSchema() const { return schema; }
};

class ComplexData final : public Param {
  std::list<std::unique_ptr<Format>> supported{};
  std::unique_ptr<Format> defaultSupported{nullptr};

  long maximumMegabytes = 0;

  std::string defaultString{""};

 public:
  ComplexData() = default;

  ComplexData(const ComplexData &) = delete;
  ComplexData(ComplexData &&) = delete;

  ~ComplexData() override = default;

 public:
  void setMaximumMegabytes(long pValue) { maximumMegabytes = pValue; }
  long getMaximumMegabytes() const { return maximumMegabytes; }
  void setMaximumMegabytes(std::string pValue) {
    if (pValue.empty()) {
      maximumMegabytes = 0;
    } else {
      std::istringstream iss(pValue);
      iss >> maximumMegabytes;
      if (iss.fail()) {
        // TODO: raise the exception
      }
    }
  }

  std::string getType() override { return std::string("ComplexData"); }

  void moveDefaultSupported(std::unique_ptr<Format> &format) {
    if (format) {
      defaultSupported = std::move(format);
    }
  }

  void moveAddSupported(std::unique_ptr<Format> &format) {
    if (format) {
      supported.emplace_back(format.release());
    }
  }

  const std::list<std::unique_ptr<Format>> &getSupported() const {
    return supported;
  }

  const std::unique_ptr<Format> &getDefaultSupported() const {
    return defaultSupported;
  }

  const std::string &getDefaultString() const { return defaultString; }
  void setDefaultString(const std::string &defaultString) {
    ComplexData::defaultString = defaultString;
  }
};

struct Content {
  std::string href{""};
  std::string type{""};
  std::string tag{""};
  std::string err{""};

  Content(std::string pCode, std::string pHref)
      : type(std::move(pCode)), href(std::move(pHref)) {}
};

class OWSProcessDescription final : public Descriptor {
  std::list<std::unique_ptr<Param>> inputs;
  std::list<std::unique_ptr<Param>> outputs;

  std::string describer{""};

 public:
  OWSProcessDescription() = default;
  OWSProcessDescription(const OWSProcessDescription &) = delete;
  OWSProcessDescription(OWSProcessDescription &&) = delete;

  ~OWSProcessDescription() override = default;

 public:
  void addMoveInput(std::unique_ptr<Param> &param) {
    if (param) {
      inputs.emplace_back(std::move(param));
    }
  }

  void addMoveOutput(std::unique_ptr<Param> &param) {
    if (param) {
      outputs.emplace_back(std::move(param));
    }
  }

  const std::list<std::unique_ptr<Param>> &getInputs() const { return inputs; }
  const std::list<std::unique_ptr<Param>> &getOutputs() const {
    return outputs;
  }

  const std::string &getDescriber() const { return describer; }
  void setDescriber(const std::string &describer) {
    OWSProcessDescription::describer = describer;
  }
};

class OWSOffering {
  std::list<std::unique_ptr<OWSProcessDescription>> processDescription{};
  std::string code{""};
  std::list<Content> contents{};

 public:
  OWSOffering() = default;
  OWSOffering(const OWSOffering &) = delete;
  OWSOffering(OWSOffering &&) = delete;

  ~OWSOffering() = default;

 public:
  /**
   * The content of the property tag could be altered by adding a value such as
   * CWL
   * @return
   */
  std::list<Content> &getContents() { return contents; }

  void addContent(std::string code, std::string href) {
    if (!code.empty()) {
      contents.emplace_back(std::move(code), std::move(href));
    }
  }

  void moveAddProcessDescription(
      std::unique_ptr<OWSProcessDescription> &offering) {
    if (offering) {
      processDescription.emplace_back(std::move(offering));
    }
  }

  const std::list<std::unique_ptr<OWSProcessDescription>>
      &getProcessDescription() const {
    return processDescription;
  }

  const std::string &getCode() const { return code; }
  void setCode(std::string theCode) { OWSOffering::code = std::move(theCode); }
};

class OWSEntry {
  std::list<std::unique_ptr<OWSOffering>> offerings{};
  std::string packageIdentifier{""};

 public:
  OWSEntry() = default;
  OWSEntry(const OWSEntry &) = delete;
  OWSEntry(OWSEntry &&) = delete;

  ~OWSEntry() = default;

 public:
  void setPackageIdentifier(std::string pPackageIdentifier) {
    OWSEntry::packageIdentifier = std::move(pPackageIdentifier);
  }

  const std::string &getPackageIdentifier() const { return packageIdentifier; }

  void moveAddOffering(std::unique_ptr<OWSOffering> &offering) {
    if (offering) {
      offerings.emplace_back(std::move(offering));
    }
  }

  const std::list<std::unique_ptr<OWSOffering>> &getOfferings() const {
    return offerings;
  }
};

class OWSContext {
  std::list<std::unique_ptr<OWSEntry>> entries;

 public:
  OWSContext() = default;
  OWSContext(const OWSContext &) = delete;
  OWSContext(OWSContext &&) = delete;

  ~OWSContext() = default;

 public:
  const std::list<std::unique_ptr<OWSEntry>> &getEntries() const {
    return entries;
  }

  void moveAddEntry(std::unique_ptr<OWSEntry> &entry) {
    if (entry) {
      entries.emplace_back(std::move(entry));
    }
  }
};

}  // namespace EOEPCA::OWS
#endif  // EOEPCAOWS_OWSPARAMETER_HPP
