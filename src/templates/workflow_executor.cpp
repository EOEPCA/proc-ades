

#include "workflow_executor.hpp"

#include <utility>



#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


extern "C" int start(
    const std::string& configFile,const std::string &cwlFile,const std::string& inputsFile,
    const std::string &wpsServiceID,const std::string &runId,std::string &serviceID){

  serviceID="test_ID";

  std::cerr<< "bla" <<std::endl;



  exec("echo hello");

  return 0;
}

extern "C" int getStatus(const std::string& configFile,
                 const std::string &serviceID, int &percent,std::string &message) {




  return 0;
}

extern "C" int getResults(const std::string& configFile,
                  const std::string &serviceID,
                  std::list<std::pair<std::string, std::string>> &outPutList) {


  outPutList.push_back(std::make_pair<std::string, std::string>("results","https://catalog....."));



//  la funzione viene richiamata ogni volta che torni con 0... != da zero esce dal loop
//  return 0;

  return 1;
}

extern "C" void clear(const std::string& configFile,
              const std::string &serviceID){




};






