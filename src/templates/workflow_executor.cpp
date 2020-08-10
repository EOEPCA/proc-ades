
#include "nlohmann/json.hpp"
#include "workflow_executor.hpp"
#include "../deployundeploy/zoo/httpfuntions.hpp"
#include <utility>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <regex>
#include "exec.hpp"
#include <unistd.h>

int exec(const std::string &cmd, std::string &err, std::string &out, std::string base) {
    std::string err_{"/tmp/" + base + ".err"};
    std::string out_{"/tmp/" + base + ".out"};
    std::string cmd_{cmd + " 1>" + out_ + " 2>" + err_};
    auto ret = system(cmd_.c_str());
    std::ifstream ifsERR(err_);
    std::ifstream ifsOUT(out_);
    err.assign((std::istreambuf_iterator<char>(ifsERR)), (std::istreambuf_iterator<char>()));
    out.assign((std::istreambuf_iterator<char>(ifsOUT)), (std::istreambuf_iterator<char>()));
    ifsERR.close();
    ifsOUT.close();
    if (std::remove(err_.c_str()) != 0) {
        perror("Error deleting temporary file");
    }
    if (std::remove(out_.c_str()) != 0) {
        perror("Error deleting temporary file");
    }
    return ret;
}

extern "C" int start(const std::string &configFile, const std::string &cwlFile, const std::string &inputsFile, const std::string &wpsServiceID, const std::string &runId, std::string &serviceID) {

    ///////////
    // preparing kubernetes namespace for workflow

    // generating a workflow id
    std::time_t result = std::time(nullptr);
    serviceID = "wf-" + runId;
    serviceID = std::regex_replace(serviceID, std::regex("_"), "-");
    std::stringstream preparecommand;

    //prepare --kubeconfig /var/snap/microk8s/current/credentials/kubelet.config t2demo 1 t2demo-volume

    preparecommand << "workflow_executor prepare --kubeconfig /opt/t2config/kubeconfig   " << serviceID << " 2  " << serviceID << "-volume";
    std::string response;
    std::string err{""}, out{""}, base{serviceID + "_1"};
    exec(preparecommand.str(), err, out, base);
    auto haserror = err.find("error");
    auto hasstacktrace = err.find("Traceback");

    std::cerr << "err-------------------------------\n";
    std::cerr << err << "\n";
    std::cerr << "out-------------------------------\n";
    std::cerr << out << "\n";
    std::cerr << "-------------------------------\n";
    if (haserror != std::string::npos || hasstacktrace != std::string::npos) {
        std::cerr << err << std::endl;
        serviceID = err;
        return 1;
    }


    /////////
    // executing workflow

    // creating job_order.json file
    std::string job_inputs_file = "/tmp/job_order.json";
    std::ofstream file(job_inputs_file);
    file << inputsFile;
    file.flush();
    file.close();
    sleep(1);
    std::stringstream executecommand;
    // execute --kubeconfig /var/snap/microk8s/current/credentials/kubelet.config
    // /home/bla/dev/EOEPCA_dev/cwl-executor/workflow_executor/examples/job_order2.json
    // /home/bla/dev/EOEPCA_dev/cwl-executor/workflow_executor/examples/vegetation-index.cwl
    // t2demo-volume
    // t2demo
    // t2workflow123
    // /t2application
    executecommand << "workflow_executor execute --kubeconfig /opt/t2config/kubeconfig ";
    executecommand << job_inputs_file << " ";           // input.json
    executecommand << cwlFile << " ";                   // cwl file
    executecommand << serviceID << "-volume ";          // volume
    executecommand << serviceID << " ";                 // namespace
    executecommand << "wf-" << serviceID << " ";        // workflowname, must be unique
    executecommand << "/workflow";                      // mount path in pod, not important
    exec(executecommand.str(), err, out, base);
    haserror = err.find("error");
    hasstacktrace = err.find("Traceback");
    std::cerr << "err-------------------------------\n";
    std::cerr << err << "\n";
    std::cerr << "out-------------------------------\n";
    std::cerr << out << "\n";
    std::cerr << "-------------------------------\n";
    if (haserror != std::string::npos || hasstacktrace != std::string::npos) {
        std::cerr << err << std::endl;
        serviceID = err;
        return 1;
    }
    return 0;
}

extern "C" int getStatus(const std::string &configFile, const std::string &serviceID, int &percent, std::string &message) {

    /////////
    // executing getStatus command

    std::stringstream getstatuscommand;
    getstatuscommand << "workflow_executor status --kubeconfig /opt/t2config/kubeconfig " << serviceID << " wf-" << serviceID;
    std::cerr << getstatuscommand.str() << std::endl;
    std::string status;
    std::string err{""}, out{""}, base{serviceID + "_1"};
    auto exitcode = exec(getstatuscommand.str(), err, out, base);
    std::cerr << "ERR:" << err << std::endl;
    std::cerr << "OUT:" << out << std::endl;
    std::cerr << "EXIT CODE:" << exitcode << std::endl;
//    if (exitcode != 0) {
//        std::cerr << exitcode << std::endl;
//        throw std::runtime_error(err);
//    }




    message = out;
    if (message.find("Running") != std::string::npos) {
        return 1;
    } else if (message.find("Failed") != std::string::npos) {
        throw std::runtime_error(out);
    } else if (message.find("Success") != std::string::npos) {
        return 0;
    } else {
        return 1;
    }

}

extern "C" int getResults(const std::string &configFile, const std::string &serviceID, std::list<std::pair<std::string, std::string>> &outPutList) {

    std::stringstream getresultcommand;
    getresultcommand << "workflow_executor result --kubeconfig /opt/t2config/kubeconfig " << serviceID << " /t2application " << serviceID << "-volume wf-" << serviceID;
    std::cerr << getresultcommand.str() << std::endl;
    std::string status;
    std::string err{""}, out{""}, base{serviceID + "_1"};

    auto exitcode = exec(getresultcommand.str(), err, out, base);
//    if (exitcode != 0) {
//        std::cerr << exitcode << std::endl;
//        throw std::runtime_error(err);
//    }

    std::cerr << "ERR:" << err << std::endl;
    std::cerr << "OUT:" << out << std::endl;
    std::cerr << "EXIT CODE:" << exitcode << std::endl;
    outPutList.push_back(std::make_pair<std::string, std::string>("wf_outputs", out.c_str()));




//  la funzione viene richiamata ogni volta che torni con 0... != da zero esce dal loop
//  return 0;

    return 1;
}

extern "C" void clear(const std::string &configFile, const std::string &serviceID) {


};

namespace wfexe {
using nlohmann::json;

inline json get_untyped(const json &j, const char *property) {
  if (j.find(property) != j.end()) {
    return j.at(property).get<json>();
  }
  return json();
}

inline json get_untyped(const json &j, std::string property) {
  return get_untyped(j, property.data());
}

struct error {
  int64_t code;
  std::string message;

  void dump()const{
    std::cerr << "\tcode:" << code <<"\n";
    std::cerr << "\tmessage:" << message <<"\n";
  }


};

struct workflowExecutorWebError {
  struct error error;

  void dump()const{
    std::cerr << "error:\n";
    error.dump();
  }

};

struct SingleResult{
  std::string result;
};

struct statusWebJob {
  int64_t percent;
  std::string msg;

   void dump()const{
    std::cerr << "dump:\n";
     std::cerr << "\tpercent:" << percent <<"\n";
     std::cerr << "\tmsg:" << msg <<"\n";
  }

};


} // namespace wfexe

namespace nlohmann {

bool exists_key(const json &j, const std::string &key) {
  return j.find(key) != j.end();
}


void from_json(const json & j, wfexe::error & x);

void from_json(const json & j, wfexe::SingleResult & x);
void from_json(const json & j, wfexe::workflowExecutorWebError & x);
void from_json(const json & j, wfexe::statusWebJob & x);
void from_json(const json &j,
               mods::WorkflowExecutor::WorkflowExecutorWebParameters &x);
void to_json(json &j,
             const mods::WorkflowExecutor::WorkflowExecutorWebParameters &x);


inline void from_json(const json & j, wfexe::SingleResult& x) {
  if (exists_key(j, "wf_output"))
    x.result=j.at("wf_output").get<std::string>();
}

inline void from_json(const json & j, wfexe::statusWebJob& x) {

  if (exists_key(j, "percent"))
    x.percent = j.at("percent").get<int64_t>();

  if (exists_key(j, "msg"))
    x.msg = j.at("msg").get<std::string>();
}



inline void from_json(const json & j, wfexe::error& x) {

  if (exists_key(j, "code")) {
    x.code = j.at("code").get<int64_t>();
  }else{
    x.code=-199;
  }

  if (exists_key(j, "message")) {
    x.message = j.at("message").get<std::string>();
  }
}

inline void from_json(const json & j, wfexe::workflowExecutorWebError& x) {
  if (exists_key(j, "error")) {
    x.error = j.at("error").get<wfexe::error>();
  }else{
    x.error.code=-199;
  }
}



inline void
from_json(const json &j,
          mods::WorkflowExecutor::WorkflowExecutorWebParameters &x) {

  if (exists_key(j, "prepareID")) {
    x.prepareID = j.at("prepareID").get<std::string>();
  }

  if (exists_key(j, "serviceID")) {
    x.serviceID = j.at("serviceID").get<std::string>();
  }

  if (exists_key(j, "runID")) {
    x.runID = j.at("runID").get<std::string>();
  }

  if (exists_key(j, "jobID")) {
    x.jobID = j.at("jobID").get<std::string>();
  }
}

inline void
to_json(json &j,
        const mods::WorkflowExecutor::WorkflowExecutorWebParameters &x) {
  j = json::object();
  j["runID"] = x.runID;
  j["serviceID"] = x.serviceID;
  j["prepareID"] = x.prepareID;
  j["jobID"] = x.runID;
  j["cwl"] = x.cwl;
  j["inputs"] = x.inputs;
}

} // namespace nlohmann


void parseError(const std::string& buffer){

  wfexe::workflowExecutorWebError data =
      nlohmann::json::parse(buffer);
  data.dump();

  if(data.error.code==-199){

    if (buffer.empty()){
      throw std::runtime_error("unexpected error [webcall]");
    }

    throw std::runtime_error(buffer);
  }else{
    throw std::runtime_error(data.error.message);
  }

}

extern "C" long
webPrepare(mods::WorkflowExecutor::WorkflowExecutorWebParameters &wfpm) {

  std::cerr << "**************************************webPrepare\n";
  std::string buffer;
  std::string request{wfpm.hostName};
  request.append("/prepare");

  nlohmann::json json;
  nlohmann::to_json(json,wfpm);
//  std::string content = R"({"runID": "string","serviceID": "string"})";

  auto ret = postputToWeb(buffer, json.dump(), request.c_str(), "POST");
  std::cerr << "webPrepare:\treturn: " << ret << " json:" << buffer <<"\n";

  if (ret==201){
    mods::WorkflowExecutor::WorkflowExecutorWebParameters data =
        nlohmann::json::parse(buffer);
    data.dump();

    wfpm.prepareID=data.prepareID;

  }else{
    parseError(buffer);
  }

  return ret;
};

extern "C" long
webGetPrepare(mods::WorkflowExecutor::WorkflowExecutorWebParameters &wfpm) {

  std::cerr << "**************************************webGetPrepare\n";
  wfpm.dump();

  std::string buffer;
  std::string request{wfpm.hostName};
  request.append("/prepare/").append(wfpm.prepareID);
  std::cerr  << "request: "<< request <<"\n";

  std::string bufffer;
  auto ret=getFromWeb(bufffer,request.c_str());
  if(ret==200){
    ret=0;
  }else if (ret!=100){
    parseError(buffer);
  }


  return ret;
};

extern "C" long
webExecute(mods::WorkflowExecutor::WorkflowExecutorWebParameters &wfpm) {

  std::cerr << "**************************************webExecute\n";
//  wfpm.dump();

  std::string buffer;
  std::string request{wfpm.hostName};
  request.append("/execute");
  nlohmann::json json;
  nlohmann::to_json(json,wfpm);

  auto ret = postputToWeb(buffer, json.dump(), request.c_str(), "POST");
  if (ret==201){
    mods::WorkflowExecutor::WorkflowExecutorWebParameters data =
        nlohmann::json::parse(buffer);
    data.dump();
    wfpm.jobID=data.jobID;
  }else{
    parseError(buffer);
  }

  wfpm.dump();
  return 0;
};

extern "C" long
webGetStatus(mods::WorkflowExecutor::WorkflowExecutorWebParameters &wfpm) {

  std::cerr << "**************************************webGetStatus\n";
  std::string buffer;

  std::stringstream request;
  request<< wfpm.hostName << "/status/"
          <<wfpm.serviceID<<"/"
          <<wfpm.runID<<"/"
          << wfpm.prepareID
          <<"/"<<wfpm.jobID;

  auto ret=getFromWeb(buffer,request.str().c_str());
  if(ret==200){

    wfexe::statusWebJob msgWeb= nlohmann::json::parse(buffer);
    std::cerr << "buffer: " << buffer<<"\n";
    msgWeb.dump();

    if (msgWeb.percent==100){
      ret=0;
    }

  }else {
    parseError(buffer);
    ret=0;
  }

  return ret;
};

extern "C" long
webGetResults(mods::WorkflowExecutor::WorkflowExecutorWebParameters &wfpm,
              std::list<std::pair<std::string, std::string>> &outPutList) {

  std::cerr << "**************************************webGetResults\n";
  std::string buffer;

  std::stringstream request;
  request<< wfpm.hostName << "/result/"
         <<wfpm.serviceID<<"/"
         <<wfpm.runID<<"/"
         << wfpm.prepareID
         <<"/"<<wfpm.jobID;


  auto ret=getFromWeb(buffer,request.str().c_str());
  if(ret==200){

    wfexe::SingleResult msgWeb= nlohmann::json::parse(buffer);
    std::cerr << "buffer: " << buffer<<"\n";

    outPutList.emplace_back("wf_output",msgWeb.result);

  }else {
    parseError(buffer);
    ret=0;
  }

  return ret;

};

/*

docker cp workflow_executor.cpp   zoo:/project/src/templates/ && docker cp workflow_executor.hpp   zoo:/project/src/templates/ && docker cp interface.cpp   zoo:/project/src/templates/ && docker cp  ../deployundeploy/zoo/httpfuntions.hpp  zoo:/project/src/deployundeploy/zoo/httpfuntions.hpp

 * */


/*
cp /project/build/libworkflow_executor.so /opt/t2service/libworkflow_executor.so && \
cp /project/src/deployundeploy/zoo/build/libepcatransactional.zo /opt/t2service/ && \
cp /project/src/deployundeploy/zoo/build/libepcatransactional.zo /opt/zooservices_user/rdirienzo/libepcatransactional.zo && \
cp /project/src/templates/libinterface.so /opt/t2libs/libinterface.so && \
cp /project/src/templates/libinterface.so /opt/zooservices_user/rdirienzo/


*/