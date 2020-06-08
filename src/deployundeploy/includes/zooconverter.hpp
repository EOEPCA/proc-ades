
#ifndef PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP
#define PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP

#include <eoepca/owl/owsparameter.hpp>
#include <list>
#include <memory>
#include <string>

namespace ZOO {

class Zoo {
  std::string configFile{""};
  std::string identifier{""};
  std::string provider{""};
  std::string cwlContent{""};

  std::string packageID{""};
  std::string processDescriptionId{""};
  std::string processVersion{""};

  std::string title{""};
  std::string Abstract{""};
 public:
  Zoo() = default;
  virtual ~Zoo() = default;

  Zoo& operator=(const Zoo&);

  const std::string &getConfigFile() const;
  void setConfigFile(const std::string &configFile);

  const std::string &getIdentifier() const;
  void setIdentifier(std::string identifier);

  const std::string &getProvider() const;
  void setProvider(const std::string &provider);

  const std::string &getPackageId() const;
  void setPackageId(const std::string &packageId);
  const std::string &getProcessDescriptionId() const;
  void setProcessDescriptionId(const std::string &processDescriptionId);
  const std::string &getProcessVersion() const;
  void setProcessVersion(const std::string &processVersion);
  const std::string &getTitle() const;
  void setTitle(const std::string &title);
  const std::string &getAbstract() const;
  void setAbstract(const std::string &abstract);

  const std::string &getCwlContent() const;
  void setCwlContent(const std::string &content);

};

class ZooApplication {
  std::list<std::unique_ptr<Zoo>> Zoos;
  std::string cwlUri{""};
  std::string dockerRef{""};
  std::string code{""};
  std::string packageID{""};

 public:
  ZooApplication() = default;
  virtual ~ZooApplication() = default;

  void setCode(std::string_view code);
  void setContent(std::string_view href, std::string_view type );

  void moveZoo( std::unique_ptr<Zoo>& zoo );

  const std::list<std::unique_ptr<Zoo>> &getZoos() const;
  const std::string &getCwlUri() const;
  const std::string &getDockerRef() const;
  const std::string &getCode() const;

  const std::string &getPackageId() const;
  void setPackageId(const std::string &packageId);
};

class ZooConverter {
 public:
  ZooConverter() = default;
  virtual ~ZooConverter() = default;


  std::list<std::unique_ptr<ZooApplication>> convert(const std::list<std::string>& tags,EOEPCA::OWS::OWSContext *,const std::list<std::pair<std::string,std::string>>& metadata);
  std::list<std::unique_ptr<ZooApplication>> convert(const std::list<std::string>& tags,EOEPCA::OWS::OWSContext *);
};

}  // namespace ZOO

#endif  // PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP
