#include "service.h"
#include "service_internal.h"

#include "zooargo.hpp"

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#define X3STRING std::string, std::string, std::string
#define X2STRING std::string, std::string
#define X2INPUT std::string, std::unique_ptr<mods::ArgoInterface::tgInput>

class Util {
public:
  static std::string innerReplace(std::string &str, const std::string &from,
                                  const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length();
    }
    return str;
  }
};

#define LOGTEST (std::cerr)
void _MEdumpMap(map *t) {
  if (t != NULL) {
    LOGTEST << t->name << ": (" << t->value << ")\n";
    //        if(t->next!=NULL){
    //            // _MEdumpMap(t->next);
    //        }
  } else {
    LOGTEST << "NULL\n";
  }
}

void MEdumpMap(map *t) {
  map *tmp = t;
  while (tmp != NULL) {
    _MEdumpMap(tmp);
    tmp = tmp->next;
  }
  LOGTEST << "----------------END\n";
}

void MEdumpMaps(maps *m) {
  maps *tmp = m;
  while (tmp != NULL) {
    LOGTEST << "\n----------------INI\n";
    LOGTEST << "MAP => [" << tmp->name << "] \n";
    LOGTEST << " * CONTENT [" << tmp->name << "] \n";
    LOGTEST << "----------------VALUE" << std::endl;
    MEdumpMap(tmp->content);
    LOGTEST << " * CHILD [" << tmp->name << "] \n" << std::endl;
    MEdumpMaps(tmp->child);
    tmp = tmp->next;
  }
}

int loadFile(const char *filePath, std::stringstream &sBuffer) {
  std::ifstream infile(filePath);
  if (infile.good()) {
    sBuffer << infile.rdbuf();
    return 0;
  }
  return 1;
}

bool fileExist(const char *fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}
std::string isBoundingBoxData(maps *&_params, std::string postfix) {
  auto LowerCorner =
      getMap(_params->content, std::string("LowerCorner" + postfix).c_str());
  auto UpperCorner =
      getMap(_params->content, std::string("UpperCorner" + postfix).c_str());
  auto CRS = getMap(_params->content, std::string("CRS" + postfix).c_str());

  if (LowerCorner != nullptr && UpperCorner != nullptr && CRS != nullptr) {
    std::string sLowerCorner{LowerCorner->value};
    Util::innerReplace(sLowerCorner, " ", ",");
    std::string sUpperCorner{UpperCorner->value};
    Util::innerReplace(sUpperCorner, " ", ",");

    return sLowerCorner + "," + sUpperCorner;
  }

  return "";
}

std::pair<X2INPUT> getNodeParameter(maps *&_params) {
  // ZOO puts "NULL" if the value is not present... i want to change in blank
  bool minOccurenceZero = false;
  map *minOcc = getMap(_params->content, "minOccurs");
  if (minOcc) {
    minOccurenceZero = !strcmp(minOcc->value, "0");
  }

  auto param = std::make_pair<X2INPUT>("", nullptr);

  param.first = _params->name;
  param.second = std::make_unique<mods::ArgoInterface::tgInput>();
  param.second->id = _params->name;

  map *isArray = getMap(_params->content, "isArray");
  if (isArray) {
    int len = 0;
    map *length = getMap(_params->content, "length");
    if (length) {
      len = std::stoi(length->value);
      std::string sValue;
      std::string postFix;
      for (int i = 0; i < len; i++) {
        sValue.clear();
        postFix.clear();
        sValue = "value";
        if (i > 0) {
          postFix = "_" + std::to_string(i);
          sValue.append(postFix);
          //                    sValue.append("_");
          //                    sValue.append(std::to_string(i));
        }
        map *value = getMap(_params->content, sValue.c_str());
        if (value) {
          auto bb = isBoundingBoxData(_params, postFix);
          if (i > 0) {
            if (bb.empty()) {
              if (param.second)
                param.second->value.append(",");
            } else {
              if (param.second)
                param.second->value.append("\n");
            }
          }

          if (bb.empty()) {
            if (param.second)
              param.second->value.append(value->value);
          } else {
            if (param.second)
              param.second->value.append(bb);
          }
        }
      }
    }
  } else {
    map *value = getMap(_params->content, "value");
    if (value) {
      if (minOccurenceZero) {
        if (strcmp(value->value, "NULL")) {
          auto bb = isBoundingBoxData(_params, "");
          if (bb.empty())
            if (param.second)
              param.second->value = value->value;
            else {
              if (param.second)
                param.second->value = bb;
            }
        }
      } else {
        auto bb = isBoundingBoxData(_params, "");
        if (bb.empty())
          if (param.second)
            param.second->value = value->value;
          else {
            if (param.second)
              param.second->value = bb;
          }
      }
    }
  }

  map *mimeType = getMap(_params->content, "mimeType");
  if (mimeType) {
    if (param.second)
      param.second->mimeType = mimeType->value;
  }

  return param;
}

void getT2InputConf(maps *m, std::list<std::pair<X2INPUT>> &input) {
  maps *tmp = m;
  while (tmp != NULL) {
    input.push_back(getNodeParameter(tmp));
    getT2InputConf(tmp->child, input);
    tmp = tmp->next;
  }
}

int setZooError(maps *conf, const std::string &message,
                const std::string &code) {
  setMapInMaps(conf, "lenv", "message", message.data());
  setMapInMaps(conf, "lenv", "code", code.data());

  return SERVICE_FAILED;
}

void getConfigurationFromZooMapConfig(
    maps *&conf, std::string what,
    std::map<std::string, std::string> &configs) {
  maps *t2wps = getMaps(conf, what.c_str());
  if (t2wps && t2wps->content) {
    map *tmp = t2wps->content;
    while (tmp != NULL) {
      configs[tmp->name] = tmp->value;
      tmp = tmp->next;
    }
  }
}

void log(const char *log) {
  fprintf(stderr, "%s\n", log);
  fflush(stderr);
}

void setStatus(maps *&conf, const char *status, const char *message) {
  map *usid = getMapFromMaps(conf, "lenv", "uusid");
  map *r_inputs = NULL;
  r_inputs = getMapFromMaps(conf, "main", "tmpPath");
  char *flenv = (char *)malloc(
      (strlen(r_inputs->value) + strlen(usid->value) + 12) * sizeof(char));
  sprintf(flenv, "%s/%s_lenv.cfg", r_inputs->value, usid->value);
  setMapInMaps(conf, "lenv", "message", message);
  setMapInMaps(conf, "lenv", "status", status);
  maps *lenvMaps = getMaps(conf, "lenv");
  dumpMapsToFile(lenvMaps, flenv, 0);
  free(flenv);
}

extern "C" {

//  auto configFile = std::make_unique<char[]>(M1024);
//  std::memset(configFile.get(), '\0', M1024);
ZOO_DLL_EXPORT int interface(maps *&conf, maps *&inputs, maps *&outputs) {

  try {

    std::cerr << "interface has been loaded!\n";
    fflush(stderr);

    setStatus(conf, "running", "");

    //==================================GET CONFIGURATION
    std::map<std::string, std::string> confEoepca;
    getConfigurationFromZooMapConfig(conf, "eoepca", confEoepca);

    std::map<std::string, std::string> lenv;
    getConfigurationFromZooMapConfig(conf, "lenv", lenv);

    if (confEoepca["libeoepcaargo"].empty()) {
      setStatus(conf, "failed", "eoepca configuration libeoepcaargo.so empty");
      return SERVICE_FAILED;
    }

    if (confEoepca["libargo"].empty()) {
      setStatus(conf, "failed", "eoepca configuration libargo.so empty");
      return SERVICE_FAILED;
    }


    if (confEoepca["argoConfig"].empty()) {
      setStatus(conf, "failed", "eoepca configuration argoConfig empty");
      return SERVICE_FAILED;
    }

    if (!fileExist(confEoepca["argoConfig"].c_str())){
      setStatus(conf, "failed", "eoepca configuration argoConfig not exist");
      return SERVICE_FAILED;
    }

    auto argoConfig =
        std::make_unique<mods::ArgoInterface::ArgoWorkflowConfig>();{

            std::stringstream sBuffer;
            if(!loadFile(confEoepca["argoConfig"].c_str(),sBuffer)){
              std::string err{"eoepca configuration cannot load file: "};
              err.append(confEoepca["argoConfig"]);

              setStatus(conf, "failed", err.c_str());
              return SERVICE_FAILED;
            }
            
            argoConfig->argoConfigFile=sBuffer.str();
        }

    // argoConfig->argoUri = confEoepca["argoUrl"];
    // argoConfig->k8Uri = confEoepca["k8Url"];
    // argoConfig->eoepcaargoPath = confEoepca["libeoepcaargo"];


std::cerr << argoConfig->argoConfigFile << "\n";


    auto argoInterface =
        std::make_unique<mods::ArgoInterface>(confEoepca["libargo"]);

    if (!argoInterface->IsValid()) {
      fflush(stderr);
      std::string err("The library ");
      err.append(confEoepca["libargo"]);
      err.append(" is not valid!");
      setStatus(conf, "failed", err.c_str());
      return SERVICE_FAILED;
    }
    //==================================GET CONFIGURATION

    //==================================GET PARAMETERS
    std::list<std::pair<X2INPUT>> inputParam;
    getT2InputConf(inputs, inputParam);
    MEdumpMaps(inputs);
    for (auto &a : inputParam) {
      std::cerr << "Inputs: " << a.first << " " << a.second->id << " " << a.second->value
                << " " << a.second->mimeType << "\n";
    }
    //==================================GET PARAMETERS

    //==================================GET CWL CONTENT
    std::string path("/zooservices/");
    path.append(lenv["Identifier"]).append(".yaml");
    std::stringstream cwlBuffer;
    if (loadFile(path.c_str(), cwlBuffer)) {
      std::string err("CWL file ");
      err.append(path);
      err.append(" not found!");
      setStatus(conf, "failed", err.c_str());
      return SERVICE_FAILED;
    }
    //  const char *filePath, std::stringstream &sBuffer
    //==================================GET CWL CONTENT

    setStatus(conf, "running", "the service is started");
    std::string argoWorkflowId("");

    std::cerr << "start!\n" <<  std::endl;
    argoInterface->start(*argoConfig.get(), cwlBuffer.str(), inputParam,
                         lenv["Identifier"], lenv["uusid"],
                         argoWorkflowId);

    std::cerr << "start finished" << std::endl;
    int percent = 0;
    std::string message("");
    std::cerr << "getStats start" << std::endl;
    while (argoInterface->getStatus(*argoConfig.get(), argoWorkflowId, percent,
                                    message)) {
      updateStatus(conf, percent, message.c_str());
      std::cerr << "going to sleep" << std::endl;
      sleep(10);
    }

    std::cerr << "status finished" << std::endl;
    updateStatus(conf, 100, "Done");
    sleep(40);

    std::list<std::pair<std::string, std::string>> outPutList{};
    std::cerr << "getresult " << argoWorkflowId << std::endl;
    argoInterface->getResults(*argoConfig.get(), argoWorkflowId, outPutList);
    std::cerr << "getresults finished" << std::endl;
    for (auto &[k, p] : outPutList) {
      std::cerr << "output" << p << " " << k << std::endl;
      setMapInMaps(outputs, k.c_str(), "value", p.c_str());
    }
    std::cerr << "mapping results" << std::endl;

    //  - accepted
    //  - running
    //  - successful
    //  - failed

    setStatus(conf, "successful", "");
    return SERVICE_SUCCEEDED;

  } catch (std::runtime_error &err) {

    updateStatus(conf, 100, "Done");
    setStatus(conf, "failed", err.what());
    return SERVICE_FAILED;

  } catch (...) {
    updateStatus(conf, 100, "Done");
    setStatus(conf, "failed", "Error undefined");
    return SERVICE_FAILED;
  }
}
}
