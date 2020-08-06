
#include "nlohmann/json.hpp"
#include "workflow_executor.hpp"
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

extern "C" long webPrepare(const std::string &serviceID,
                           const std::string &runID, std::string &prepareID) {

  std::cerr << "**************************************webPrepare\n";
  std::cerr << "webPrepare " << serviceID << runID << prepareID << "\n";

  return 0;
};

extern "C" long webGetPrepare(const std::string &prepareID) {

  std::cerr << "**************************************webGetPrepare\n";
  std::cerr << "webGetPrepare " << prepareID << "\n";

  return 0;
};

extern "C" long webExecute(const std::string &serviceID,
                           const std::string &runID,
                           const std::string &prepareID, const std::string &cwl,
                           const std::string &inputs, std::string &jobID) {

  std::cerr << "**************************************webExecute\n";
  std::cerr << "webExecute " << serviceID << " " << runID << " " << prepareID
            << "\n";
  std::cerr << "webExecute " << cwl << "\n";
  std::cerr << "webExecute " << inputs << "\n";

  return 0;
};

extern "C" long webGetStatus(const std::string &serviceID,
                             const std::string &runID,
                             const std::string &prepareID,
                             const std::string &jobID) {

  std::cerr << "**************************************webGetStatus\n";
  std::cerr << "webGetStatus " << serviceID << " " << runID << " " << prepareID
            << " " << jobID << "\n";

  return 0;
};

extern "C" long
webGetResults(const std::string &serviceID, const std::string &runID,
              const std::string &prepareID, const std::string &jobID,
              std::list<std::pair<std::string, std::string>> &outPutList) {

  std::cerr << "**************************************webGetResults\n";
  std::cerr << "webGetResults " << serviceID << " " << runID << " " << prepareID
            << " " << jobID << "\n";

  return 0;
};

/*
cp /project/build/libworkflow_executor.so /opt/t2service/libworkflow_executor.so && \
cp /project/src/deployundeploy/zoo/build/libepcatransactional.zo /opt/t2service/ && \
cp /project/src/deployundeploy/zoo/build/libepcatransactional.zo /opt/zooservices_user/rdirienzo/libepcatransactional.zo && \
cp /project/src/templates/libinterface.so /opt/t2libs/libinterface.so && \
cp /project/src/templates/libinterface.so /opt/zooservices_user/rdirienzo/


*/