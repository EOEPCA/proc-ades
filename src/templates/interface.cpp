#include "service.h"
#include "service_internal.h"

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

#include <json.h>
#include "workflow_executor.hpp"

//https://gist.github.com/alan-mushi/19546a0e2c6bd4e059fd
struct InputParameter{
  std::string id{""};
  std::string href{""};
  std::string value{""};
  std::string mimeType{""};
  std::string dataType{""};

  void add(json_object* json){
    json_object_object_add(json,"id",json_object_new_string(id.c_str()));
    json_object_object_add(json,"href",json_object_new_string(href.c_str()));
    json_object_object_add(json,"value",json_object_new_string(value.c_str()));
    json_object_object_add(json,"mimeType",json_object_new_string(mimeType.c_str()));
    json_object_object_add(json,"dataType",json_object_new_string(dataType.c_str()));
  }
};

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

void getNodeParameter(maps *&_params,std::list<InputParameter>& parameters) {
  // ZOO puts "NULL" if the value is not present... i want to change in blank
  bool minOccurenceZero = false;
  map *minOcc = getMap(_params->content, "minOccurs");
  if (minOcc) {
    minOccurenceZero = !strcmp(minOcc->value, "0");
  }

  int len = 0;
  map *isArray = getMap(_params->content, "isArray");
  map *length = getMap(_params->content, "length");

  if (isArray && length){
    len = std::stoi(length->value);
  }else{
    len=1;
  }

  std::string sValue;
  std::string postFix;

  map *mimeType = getMap(_params->content, "mimeType");
  map *dataType = getMap(_params->content, "dataType");

  for (int i = 0; i < len; i++) {

    //--------------CLEAR
    InputParameter parameter;

    map *value= nullptr;
    map *href= nullptr;
    sValue.clear();
    postFix.clear();
    postFix = "_" + std::to_string(i);
    //--------------CLEAR

    parameter.id=_params->name;

    if (mimeType && strlen(mimeType->value)>0){
      parameter.mimeType=mimeType->value;
    }
    if (dataType && strlen(dataType->value)>0){
      parameter.dataType=dataType->value;
    }

    //--------------VALUE
    sValue = "value";
    if (i > 0) {
      sValue.append(postFix);
    }
    value = getMap(_params->content, sValue.c_str());
    //--------------VALUE

    //--------------xlink:href
    sValue = "xlink:href";
    if (i > 0) {
      sValue.append(postFix);
    }
    href = getMap(_params->content, sValue.c_str());
    //--------------xlink:href

    if(value && strlen(value->value)){
      parameter.value=value->value;
    }

    if(href && strlen(href->value)){
      parameter.href=href->value;
    }

    if (!parameter.value.empty()){
      auto bb = isBoundingBoxData(_params, postFix);//legacy
      if (!bb.empty()){
        parameter.value=bb;
      }

    }

    parameters.push_back(parameter);
  }

}

void getT2InputConf(maps *m, std::list<InputParameter>& parameters) {
  maps *tmp = m;
  while (tmp != NULL) {

    getNodeParameter(tmp,parameters);

    getT2InputConf(tmp->child, parameters);
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


    if (confEoepca["libWorkflowExecutor"].empty()) {
      std::string err{"eoepca configuration libWorkflowExecutor empty"};

      setStatus(conf, "failed", err.c_str());
      updateStatus(conf, 100, err.c_str());
      return SERVICE_FAILED;
    }


    if (confEoepca["WorkflowExecutorConfig"].empty()) {
      std::string err{"eoepca configuration WorkflowExecutorConfig empty" };

      setStatus(conf, "failed", err.c_str());
      updateStatus(conf, 100, err.c_str());
      return SERVICE_FAILED;
    }

    if (!fileExist(confEoepca["WorkflowExecutorConfig"].c_str())){
      std::string err{"eoepca configuration WorkflowExecutorConfig not exist"};

      setStatus(conf, "failed",  err.c_str());
      updateStatus(conf, 100, err.c_str());
      return SERVICE_FAILED;
    }

    std::map<std::string, std::string> userEoepca;
    getConfigurationFromZooMapConfig(conf, "eoepcaUser", userEoepca);
    std::cerr << "user: "<< userEoepca["user"] << " grants: "  << userEoepca["grant"] << "\n\n";


    std::stringstream sConfigBuffer;
    if(loadFile(confEoepca["WorkflowExecutorConfig"].c_str(),sConfigBuffer)){
      std::string err{"eoepca configuration cannot load file: "};
      err.append(confEoepca["WorkflowExecutorConfig"]);

      setStatus(conf, "failed", err.c_str());
      updateStatus(conf, 100, err.c_str());
      return SERVICE_FAILED;
    }

    std::cerr << "WorkflowExecutorConfig: \n" << sConfigBuffer.str() << "\n\n\n";

    auto workflowExecutor=std::make_unique<mods::WorkflowExecutor>(confEoepca["libWorkflowExecutor"]);
    if (!workflowExecutor->IsValid()){
      std::string err{"eoepca libworkflow_executor.so is not valid"};
      setStatus(conf, "failed", err.c_str());
      updateStatus(conf, 100, err.c_str());
      return SERVICE_FAILED;
    }else{
      std::cerr << "\nlibworkflow_executor.so: VALID!\n\n";
    }
    //==================================GET CONFIGURATION

    //==================================GET PARAMETERS

    std::list<InputParameter> params;
    getT2InputConf(inputs, params);

    std::cerr << "ZOO input:\n";
    dumpMaps(inputs);

    json_object *jstart=json_object_new_object();
    json_object *jArray=json_object_new_array();
    for (auto &a : params) {
      json_object *jParamjParam=json_object_new_object();
      a.add(jParamjParam);
      json_object_array_add(jArray, jParamjParam);
    }
    json_object_object_add(jstart, "inputs", jArray);

    std::string jParams{json_object_to_json_string_ext(jstart, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY)};
    json_object_put(jstart);

    Util::innerReplace(jParams,"\\/","/" );
    std::cerr << "\n\nJson Inputs: \n"<< jParams.c_str() << "\n\n";
    params.clear();
    //==================================GET PARAMETERS

    //==================================GET CWL CONTENT
    std::string path(confEoepca["userworkspace"]);
    path.append("/").append(userEoepca["user"]).append("/");

    path.append(lenv["Identifier"]).append(".yaml");
    std::stringstream cwlBuffer;
    if (loadFile(path.c_str(), cwlBuffer)) {
      std::string err("CWL file ");
      err.append(path);
      err.append(" not found!");
      setStatus(conf, "failed", err.c_str());
      return SERVICE_FAILED;
    }

    std::cerr << "User cwl:\n" <<  cwlBuffer.str() << "\n";
    //==================================GET CWL CONTENT

    setStatus(conf, "running", "the service is started");
    std::string argoWorkflowId("");

    int counter=1;
    int percent = 0;
    std::string message("");
    if (confEoepca["WorkflowExecutorHost"].empty()) {

      //=============================START
      std::cerr << "start!\n" <<  std::endl;
      std::string serviceID{""};
      auto resStart=workflowExecutor->start(sConfigBuffer.str(),path/*cwlBuffer.str()*/,jParams,
                                            lenv["Identifier"], lenv["uusid"],serviceID);
      if (resStart){
        std::string err{"Start empty"};

        setStatus(conf, "failed", serviceID.c_str());
        updateStatus(conf, 100, serviceID.c_str());
        return SERVICE_FAILED;
      }
      std::cerr << "serviceID: " << serviceID << "\n\n" << std::endl;
      if (serviceID.empty()) {
        std::string err{"serviceID empty"};

        setStatus(conf, "failed", err.c_str());
        updateStatus(conf, 100, err.c_str());
        return SERVICE_FAILED;
      }
      std::cerr << "start finished" << std::endl;
      //=============================START


      //=============================STATUS

      std::cerr << "getStats start" << std::endl;
      while(workflowExecutor->getStatus(sConfigBuffer.str(),serviceID,percent,message)){
        std::cerr << "going to sleep counter: " << counter << std::endl;
        counter=counter+1;
        sleep(60);
      }

      updateStatus(conf, 95, "waiting for logs");
      sleep(40);
      std::cerr << "status finished" << std::endl;
      //=============================STATUS

      //=============================GETRESULT
      updateStatus(conf, 98, "Get Results");
      std::list<std::pair<std::string, std::string>> outPutList{};
      std::cerr << "getresult " << argoWorkflowId << std::endl;
      workflowExecutor->getResults(sConfigBuffer.str(),serviceID,outPutList);
      std::cerr << "getresults finished" << std::endl;
      for (auto &[k, p] : outPutList) {
        std::cerr << "output" << p << " " << k << std::endl;
        setMapInMaps(outputs, k.c_str(), "value", p.c_str());
      }
      std::cerr << "mapping results" << std::endl;
      //=============================GETRESULT
      //  - accepted
      //  - running
      //  - successful
      //  - failed

    }else if (!confEoepca["WorkflowExecutorHost"].empty()){

      auto wfpm=std::make_unique<mods::WorkflowExecutor::WorkflowExecutorWebParameters>();

      wfpm->hostName=confEoepca["WorkflowExecutorHost"];
      wfpm->serviceID=lenv["Identifier"];
      wfpm->runID = lenv["uusid"];


      std::string prepareID;
      auto retWeb=workflowExecutor->webPrepare(*wfpm);


      while (workflowExecutor->webGetPrepare(*wfpm) ){
        std::cerr << "going to sleep counter[webGetPrepare]: " << counter << std::endl;
        counter=counter+1;
        sleep(20);
      }

      wfpm->cwl=cwlBuffer.str();
      wfpm->inputs=jParams;
      retWeb=workflowExecutor->webExecute(*wfpm);

      counter=1;
      while (workflowExecutor->webGetStatus(*wfpm) ){
        std::cerr << "going to sleep counter[webGetPrepare]: " << counter << std::endl;
        counter=counter+1;
        sleep(60);
      }







    }else{
      //error




    }



    updateStatus(conf, 100, "Done");
    setStatus(conf, "successful", "");
    return SERVICE_SUCCEEDED;

  } catch (std::runtime_error &err) {

    updateStatus(conf, 100, err.what());
    setStatus(conf, "failed", err.what());
    return SERVICE_FAILED;

  } catch (...) {
    updateStatus(conf, 100, "Done");
    setStatus(conf, "failed", "Error undefined");
    return SERVICE_FAILED;
  }
}
}
