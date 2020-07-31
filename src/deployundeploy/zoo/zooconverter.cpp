

#include "zooconverter.hpp"

#include "sha1.hpp"
#include <list>
#include <memory>
#include <sstream>
#include <utility>

namespace ZOO {

// if (input->getType() == "BoundingBoxData") {
// auto data =
//    dynamic_cast<EOEPCA::OWS::BoundingBoxData*>(input.get());
// EOEPCA::OWS::Param

template <class T> std::string parseParam(T *t) { return ""; }

void prepareInfos(std::ostream &os, EOEPCA::OWS::Param *param) {
  if (param) {
    os << "\t[" << param->getIdentifier() << "]\n";
    os << "\tTitle = " << param->getTitle() << "\n";
    os << "\tAbstract = " << param->getAbstract() << "\n";

    os << "\tminOccurs = " << param->getMinOccurs() << "\n";
    os << "\tmaxOccurs = " << param->getMaxOccurs() << "\n";
  }
}

template <>
std::string
parseParam<EOEPCA::OWS::BoundingBoxData>(EOEPCA::OWS::BoundingBoxData *param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  os << "\t<BoundingBoxData>\n";

  if (param->getSupported().empty()) {
    os << "\t\t<Default>\n";
    os << "\t\t\tCRS = urn:ogc:def:crs:EPSG:6.6:4326\n";
    os << "\t\t</Default>\n";

    os << "\t\t<Supported>\n";
    os << "\t\t\tCRS = urn:ogc:def:crs:EPSG:6.6:4326\n";
    os << "\t\t</Supported>\n";

  } else {
    for (auto &sup : param->getSupported()) {
      os << "\t\t<Supported>\n";
      os << "\t\t\tCRS = " << sup << "\n";
      os << "\t\t</Supported>\n";
    }
  }

  if (!param->getDefaultValue().empty()) {
    os << "\t\t<Default>\n";
    os << "\t\t\tCRS = " << param->getDefaultValue() << "\n";
    os << "\t\t</Default>\n";

  } else {
    os << "\t\t<Default />\n";
  }
  os << "\t</BoundingBoxData>\n";

  return buffer.str();
}

template <>
std::string
parseParam<EOEPCA::OWS::LiteralData>(EOEPCA::OWS::LiteralData *param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  os << "\t<LiteralData>\n";
  os << "\t\tdataType = " << param->getDataType() << "\n";
  if (!param->getAllowedValues().empty()) {
    os << "\t\tAllowedValues = ";

    bool isFirst = true;
    for (auto &allow : param->getAllowedValues()) {
      if (isFirst)
        isFirst = false;
      else
        os << ",";
      os << allow;
    }

    os << "\n";
  }

  auto def = param->getDefaultValue();
  if (def.empty()) {
    os << "\t\t<Default />\n";
  } else {
    os << "\t\t<Default>\n";
    os << "\t\tvalue = " << def << "\n";
    os << "\t\t</Default>\n";
  };

  os << "\t</LiteralData>\n";

  return buffer.str();
}

template <>
std::string
parseParam<EOEPCA::OWS::ComplexData>(EOEPCA::OWS::ComplexData *param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  if (param->getMaximumMegabytes() > 0) {
    os << "\tmaximumMegabytes = " << param->getMaximumMegabytes() << "\n";
  }

  os << "\t<ComplexData>\n";


  std::list<std::string> mtInsertedDefault{};


  if (param->getDefaultSupported()) {
    os << "\t\t<Default>\n";

    if (!param->getDefaultSupported()->getMimeType().empty()) {
      os << "\t\t\tmimeType = " << param->getDefaultSupported()->getMimeType()
         << "\n";
      mtInsertedDefault.push_back(param->getDefaultSupported()->getMimeType());
    }

    if (!param->getDefaultSupported()->getEncoding().empty())
      os << "\t\t\tencoding = " << param->getDefaultSupported()->getEncoding()
         << "\n";

    if (!param->getDefaultSupported()->getSchema().empty())
      os << "\t\t\tschema = " << param->getDefaultSupported()->getSchema()
         << "\n";

    os << "\t\t</Default>\n";
  } else {
    os << "\t\t<Default>\n\t\t</Default>\n";
  }

  if (!param->getSupported().empty()) {
    for (auto &supp : param->getSupported()) {

      bool f{false};
      for(auto&inserted:mtInsertedDefault){
          if(inserted==supp->getMimeType()){
            f=true;
          }
      }

      if(!f){
        os << "\t\t<Supported>\n";
        if (!supp->getMimeType().empty())
          os << "\t\t\tmimeType = " << supp->getMimeType() << "\n";

        if (!supp->getEncoding().empty())
          os << "\t\t\tencoding = " << supp->getEncoding() << "\n";

        if (!supp->getSchema().empty())
          os << "\t\t\tschema =   " << supp->getSchema() << "\n";

        os << "\t\t</Supported>\n";
      }
    }
  }


  os << "\t</ComplexData>\n";

  return buffer.str();
}

template <>
std::string parseParam<EOEPCA::OWS::Param>(EOEPCA::OWS::Param *param) {
  // casting otherwise string check

  auto bb = dynamic_cast<EOEPCA::OWS::BoundingBoxData *>(param);
  if (bb) {
    return parseParam(bb);
  }

  auto ld = dynamic_cast<EOEPCA::OWS::LiteralData *>(param);
  if (ld) {
    return parseParam(ld);
  }

  auto cd = dynamic_cast<EOEPCA::OWS::ComplexData *>(param);
  if (cd) {
    return parseParam(cd);
  }

  return "";
}

class ZooJob {
  std::list<std::pair<std::string, std::string>> metadata{};
  std::string identifier{""}, processVersion{""};
  std::string title{""}, abstract{""};

  std::list<std::string> inputs{};
  std::list<std::string> outputs{};

  std::list<std::string> tags;

  static std::string innerReplace(std::string &str, const std::string &from,
                      const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos +=
          to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
  }

public:
  ZooJob() = default;
  virtual ~ZooJob() = default;

public:
  operator std::string() const;
  friend std::ostream &operator<<(std::ostream &os, const ZooJob &zoo);

  std::string getUniqueService() const;
  void addMetadata(std::string, std::string);

public:
  void addInput(std::string);
  void addOutput(std::string);

  void addTags(const std::list<std::string> &pTags);
  void addTags(std::string_view pTag);

  const std::list<std::string> &getInputs() const;
  const std::list<std::string> &getOutputs() const;

  const std::list<std::pair<std::string, std::string>> &getMetadata() const;
  const std::string &getIdentifier() const;
  void setIdentifier(const std::string &identifier);
  const std::string &getProcessVersion() const;
  void setProcessVersion(const std::string &processVersion);
  const std::string &getTitle() const;
  void setTitle(const std::string &title);
  const std::string &getAbstract() const;
  void setAbstract(const std::string &abstract);
};

void ZooJob::addInput(std::string val) {
  if (!val.empty()) {
    inputs.emplace_back(std::move(val));
  }
}
void ZooJob::addOutput(std::string val) {
  if (!val.empty()) {
    outputs.emplace_back(std::move(val));
  }
}

std::string ZooJob::getUniqueService() const {
  //  std::string toSha1{""};
  //  for (auto& tag : tags) {
  //    toSha1.append(tag);
  //  }
  //  toSha1.append(identifier).append(processVersion);
  //
  //  std::string id = "w";
  //  id.append(sha1::parseString(toSha1));
  //
  //  return id;

  std::string ig(identifier + "_" + processVersion);

  innerReplace(ig, ".", "_");
  innerReplace(ig, "-", "_");

  return ig;
}

ZooJob::operator std::string() const {
  std::string s;
  std::stringbuf buffer;
  std::ostream os(&buffer);
  os << *this;
  s = buffer.str();
  return std::move(s);
}

std::ostream &operator<<(std::ostream &os, const ZooJob &zoo) {
  os << "[" << zoo.getUniqueService() << "]\n";
  os << "Title = " << zoo.title << "\n";
  os << "Abstract = " << zoo.abstract << "\n";
  os << "processVersion = " << zoo.processVersion << "\n";

  os << "storeSupported = true"
     << "\n";
  os << "statusSupported = true"
     << "\n";
  os << "serviceType = C"
     << "\n";
  os << "serviceProvider = " << zoo.getUniqueService() << ".zo"
     << "\n";

  os << "<MetaData>"
     << "\n";
  for (auto &[key, value] : zoo.getMetadata()) {
    os << "\t" << key << " = " << value << "\n";
  }
  os << "</MetaData>"
     << "\n";

  for (auto &input : zoo.getInputs()) {
    os << "<DataInputs>"
       << "\n";
    os << input << "\n";
    os << "</DataInputs>"
       << "\n";
  }

  for (auto &output : zoo.getOutputs()) {
    os << "<DataOutputs>"
       << "\n";
    os << output << "\n";
    os << "</DataOutputs>"
       << "\n";
  }

  return os;
}
const std::string &ZooJob::getIdentifier() const { return identifier; }
void ZooJob::setIdentifier(const std::string &identifier) {
  ZooJob::identifier = identifier;
}
const std::string &ZooJob::getProcessVersion() const { return processVersion; }
void ZooJob::setProcessVersion(const std::string &processVersion) {
  ZooJob::processVersion = processVersion;
}
const std::string &ZooJob::getTitle() const { return title; }
void ZooJob::setTitle(const std::string &title) { ZooJob::title = title; }
const std::string &ZooJob::getAbstract() const { return abstract; }
void ZooJob::setAbstract(const std::string &abstract) {
  ZooJob::abstract = abstract;
}

void ZooJob::addMetadata(std::string key, std::string value) {
  if (!key.empty() && !value.empty()) {
    metadata.emplace_back(key, value);
  }
}

const std::list<std::pair<std::string, std::string>> &
ZooJob::getMetadata() const {
  return metadata;
}
const std::list<std::string> &ZooJob::getInputs() const { return inputs; }
const std::list<std::string> &ZooJob::getOutputs() const { return outputs; }

std::list<std::unique_ptr<ZooApplication>>
ZooConverter::convert(const std::list<std::string> &uniqueTags,
                      EOEPCA::OWS::OWSContext *owsContext) {
  return convert(uniqueTags, owsContext,
                 std::list<std::pair<std::string, std::string>>());
}

std::list<std::unique_ptr<ZooApplication>> ZooConverter::convert(
    const std::list<std::string> &uniqueTags,
    EOEPCA::OWS::OWSContext *owsContext,
    const std::list<std::pair<std::string, std::string>> &metadata) {
  std::list<std::unique_ptr<ZooApplication>> all;

  //  ;

  for (auto &entry : owsContext->getEntries()) {
    for (auto &offer : entry->getOfferings()) {
      // create application
      auto zooApplication = std::make_unique<ZooApplication>();
      zooApplication->setCode(offer->getCode());
      zooApplication->setPackageId(entry->getPackageIdentifier());

      // contents
      for (auto &content : offer->getContents()) {
        zooApplication->setContent(content.href, content.type);
      }

      // all process
      for (auto &processDescription : offer->getProcessDescription()) {
        auto zoo = std::make_unique<Zoo>();
        auto zooJob = std::make_unique<ZooJob>();
        zooJob->addTags(entry->getPackageIdentifier());
        zooJob->addTags(uniqueTags);

        zooJob->setTitle(processDescription->getTitle());
        zooJob->setAbstract(processDescription->getAbstract());
        zooJob->setIdentifier(processDescription->getIdentifier());
        zooJob->setProcessVersion(processDescription->getVersion());

        //        zooJob->setTitle(processDescription->getTitle());
        //        zooJob->setAbstract(processDescription->getAbstract());

        for (auto &[k, v] : metadata) {
          zooJob->addMetadata(k, v);
        }
        zooJob->addMetadata("cwl", zooApplication->getCwlUri());
        zooJob->addMetadata("PackageId", entry->getPackageIdentifier());
        zooJob->addMetadata("ProcessDescriptionIdentifier",
                            processDescription->getIdentifier());
        zooJob->addMetadata("ProcessDescriptionVersion",
                            processDescription->getVersion());

        auto zooConfig = std::make_unique<Zoo>();
        zooConfig->setIdentifier(std::move(zooJob->getUniqueService()));

        zooConfig->setCwlContent(processDescription->getDescriber());

        zooConfig->setProvider(zooConfig->getIdentifier() + ".zo");
        zooConfig->setPackageId(entry->getPackageIdentifier());
        zooConfig->setProcessDescriptionId(processDescription->getIdentifier());
        zooConfig->setProcessVersion(processDescription->getVersion());
        zooConfig->setTitle(processDescription->getTitle());
        zooConfig->setAbstract(processDescription->getAbstract());

        for (auto &input : processDescription->getInputs()) {
          auto res = parseParam<EOEPCA::OWS::Param>(input.get());
          if (!res.empty()) {
            zooJob->addInput(std::move(res));
          }
        }


        auto theOutput=std::make_unique<EOEPCA::OWS::ComplexData>();
        {
          theOutput->setIdentifier("wf_outputs");
          theOutput->setTitle("wf_outputs");
          auto format=std::make_unique<EOEPCA::OWS::Format>();
          format->setMimeType("application/geo+json; profile=stac");
          auto supportedDefault=std::make_unique<EOEPCA::OWS::Format>();
          supportedDefault->setMimeType("application/geo+json; profile=stac");

          theOutput->moveDefaultSupported(supportedDefault);
          theOutput->moveAddSupported(format);

          auto res = parseParam<EOEPCA::OWS::Param>(theOutput.get());

          zooJob->addOutput(std::move(res));
        }


//        for (auto &output : processDescription->getOutputs()) {
//          auto res = parseParam<EOEPCA::OWS::Param>(output.get());
//          if (!res.empty()) {
//            zooJob->addOutput(std::move(res));
//          }
//        }

        zooConfig->setConfigFile((std::string)*zooJob);

        zooApplication->moveZoo(zooConfig);
      }

      all.emplace_back(std::move(zooApplication));
    }
  }

  return all;
}

Zoo &Zoo::operator=(const Zoo &other) {
  if (this != &other) {
    this->configFile = other.configFile;
    this->identifier = other.identifier;
    this->provider = other.provider;
    this->packageID = other.packageID;
    this->processDescriptionId = other.processDescriptionId;
    this->processVersion = other.processVersion;

    this->title = other.title;
    this->Abstract = other.Abstract;
  }
  return *this;
}

const std::string &Zoo::getConfigFile() const { return configFile; }
void Zoo::setConfigFile(const std::string &configFile) {
  Zoo::configFile = configFile;
}
const std::string &Zoo::getIdentifier() const { return identifier; }
void Zoo::setIdentifier(std::string identifier) {
  Zoo::identifier = identifier;
}
const std::string &Zoo::getProvider() const { return provider; }
void Zoo::setProvider(const std::string &provider) { Zoo::provider = provider; }
const std::string &Zoo::getPackageId() const { return packageID; }
void Zoo::setPackageId(const std::string &packageId) { packageID = packageId; }
const std::string &Zoo::getProcessDescriptionId() const {
  return processDescriptionId;
}
void Zoo::setProcessDescriptionId(const std::string &processDescriptionId) {
  Zoo::processDescriptionId = processDescriptionId;
}
const std::string &Zoo::getProcessVersion() const { return processVersion; }
void Zoo::setProcessVersion(const std::string &processVersion) {
  Zoo::processVersion = processVersion;
}
const std::string &Zoo::getTitle() const { return title; }
void Zoo::setTitle(const std::string &title) { Zoo::title = title; }
const std::string &Zoo::getAbstract() const { return Abstract; }
void Zoo::setAbstract(const std::string &abstract) { Abstract = abstract; }

const std::string &Zoo::getCwlContent() const { return cwlContent; }
void Zoo::setCwlContent(const std::string &content) { cwlContent = content; }

void ZooApplication::setContent(std::string_view href, std::string_view type) {
  if (type == "application/vnd.docker.distribution.manifest.v1+json") {
    // GET DOCKER MANIFEST

  } else if (type == "application/cwl") {
    cwlUri = href;
  }
}
void ZooApplication::moveZoo(std::unique_ptr<Zoo> &zoo) {
  if (zoo) {
    Zoos.emplace_back(std::move(zoo));
  }
}

void ZooApplication::setCode(std::string_view theCode) { code = theCode; }
const std::list<std::unique_ptr<Zoo>> &ZooApplication::getZoos() const {
  return Zoos;
}
const std::string &ZooApplication::getCwlUri() const { return cwlUri; }
const std::string &ZooApplication::getDockerRef() const { return dockerRef; }
const std::string &ZooApplication::getCode() const { return code; }
const std::string &ZooApplication::getPackageId() const { return packageID; }
void ZooApplication::setPackageId(const std::string &packageId) {
  packageID = packageId;
}

void ZooJob::addTags(const std::list<std::string> &pTags) {
  for (auto &tag : pTags) {
    tags.emplace_back(tag);
  }
}

void ZooJob::addTags(std::string_view pTag) { tags.emplace_back(pTag); }

} // namespace ZOO
