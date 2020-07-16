

#include "workflow_executor.hpp"

#include <utility>


extern "C" int start(
    const std::string& configFile,const std::string &cwlFile,const std::string& inputsFile,
    const std::string &wpsServiceID,const std::string &runId,std::string &serviceID){

  serviceID="test_ID";


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






