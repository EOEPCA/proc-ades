

#include "workflow_executor.hpp"

#include <utility>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <regex>
#include "exec.hpp"


int  execute(const std::string& cmd,std::string& buffer) {
    Exec exec{};
    exec = cmd;
    try {
        exec.exec();
        buffer=(std::string)exec;
        if (((int) exec) != 0) {
            return (int)exec;
        }
    } catch (...) {
        buffer="Unexpected error";
        return 1;
    }
    return (int)exec;
}

extern "C" int start(const std::string &configFile, const std::string &cwlFile, const std::string &inputsFile, const std::string &wpsServiceID, const std::string &runId, std::string &serviceID) {

    ///////////
    // preparing kubernetes namespace for workflow

    // generating a workflow id
    serviceID = wpsServiceID + "_" + runId;
    serviceID = std::regex_replace(serviceID, std::regex("_"), "-");
    std::stringstream preparecommand;
    preparecommand << "workflow_executor --config /opt/t2config/kubeconfig  prepare " << serviceID << " 2  " << serviceID << "-volume";
    std::string response;
    auto exitcode = execute(preparecommand.str(), response);
    if(exitcode != 0 ) {
        std::cerr << exitcode << std::endl;
        serviceID=response;
        return 1;
    }


    /////////
    // executing workflow

    // creating job_order.json file
    std::string job_inputs_file = "/tmp/job_order.json";
    std::ofstream file(job_inputs_file);
    file << inputsFile;
    std::stringstream executecommand;
    executecommand << "workflow_executor --config /opt/t2config/kubeconfig execute -i "<< job_inputs_file <<" -c "<< cwlFile << " -v "<< serviceID <<"-volume -n "<< serviceID <<" -m \"/workflow\" -w "<< serviceID;
    exitcode = execute(executecommand.str(), response);
    if(exitcode != 0 ) {
        std::cerr << exitcode << std::endl;
        serviceID=response;
        return 1;
    }

    return 0;
}

extern "C" int getStatus(const std::string &configFile, const std::string &serviceID, int &percent, std::string &message) {

    /////////
    // executing getStatus command

    std::stringstream getstatuscommand;
    getstatuscommand << "workflow_executor --config /opt/t2config/kubeconfig getstatus -n t2demo -n "<< serviceID <<" -w "<< serviceID;
    std::string response;
    auto exitcode = execute(getstatuscommand.str(), response);
    if(exitcode != 0 ) {
        std::cerr << exitcode << std::endl;
        throw std::runtime_error(response);
    }
    return 0;
}

extern "C" int getResults(const std::string &configFile, const std::string &serviceID, std::list<std::pair<std::string, std::string>> &outPutList) {

    outPutList.push_back(std::make_pair<std::string, std::string>("results", "https://catalog....."));



//  la funzione viene richiamata ogni volta che torni con 0... != da zero esce dal loop
//  return 0;

    return 1;
}

extern "C" void clear(const std::string &configFile, const std::string &serviceID) {


};






