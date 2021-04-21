#ifndef ADES_CODE_ENGINE_API_FRAMEWORK_ZOOARGO_HPP
#define ADES_CODE_ENGINE_API_FRAMEWORK_ZOOARGO_HPP

#include <dlfcn.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <list>
#include <memory>
#include <utility>

namespace mods {
#ifndef T2CWL_IMOD_HPP
#define T2CWL_IMOD_HPP
class IModInterface {
  bool valid{false};

protected:
public:
  IModInterface() = delete;

  IModInterface(const IModInterface &) = delete;

  explicit IModInterface(std::string path) : path_(std::move(path)) {
    handle = dlopen(path_.data(), RTLD_LAZY);
    if (!handle) {
      handle = nullptr;
      setValid(false);
      setLastError(std::string(dlerror()));
    }
    setValid(true);
    resetError();
  }

  virtual ~IModInterface() {
    if (handle != nullptr) {
      dlclose(handle);
      handle = nullptr;
      resetError();
    }
  }

  bool isValid() const { return valid; }
  void setValid(bool val) { IModInterface::valid = val; }

  const std::string &getLastError() const { return lastError; }
  void resetError() { lastError.clear(); }
  void setLastError(std::string strError) {
    IModInterface::lastError = std::move(lastError);
  }

protected:
  std::string lastError{""};
  void *handle{nullptr};

private:
  std::string path_{""};
};
#endif

class WorkflowExecutor : protected mods::IModInterface {



public:
  struct WorkflowExecutorWebParameters{

    WorkflowExecutorWebParameters()=default;

    std::string serviceID;
    std::string runID;
    std::string prepareID;
    std::string jobID;
    std::string cwl;
    std::string inputs;
    int perc;
    std::string message;
    std::string username;

    std::string hostName;
    ~WorkflowExecutorWebParameters()=default;

    void dump() const{
      std::cerr<< "WorkflowExecutorWebParameters: \n";
      std::cerr << "\thostName: " << hostName << "\n";
      std::cerr << "\tusername: " << username << "\n";
      std::cerr << "\tserviceID: " << serviceID << "\n";
      std::cerr << "\trunID: " << runID << "\n";
      std::cerr << "\tprepareID: " << prepareID << "\n";
      std::cerr << "\tjobID: " << jobID << "\n";
      std::cerr << "\tcwl: " << cwl << "\n";
      std::cerr << "\tinputs: " << inputs << "\n";

      std::cerr << "\tperc: " << perc << "\n";
      std::cerr << "\tmessage: " << message << "\n";

    }

  };


public:
  WorkflowExecutor() = delete;
  explicit WorkflowExecutor(const std::string &path)
      : mods::IModInterface(path) {

    setValid(true);

    start = (int (*)(const std::string &configFile, const std::string &cwlFile,
                     const std::string &inputsFile,
                     const std::string &wpsServiceID, const std::string &runId,
                     std::string &serviceID))dlsym(handle, "start");
    if (!start) {
      std::cerr << "can't load 'start' function\n";
      setValid(false);
      setLastError("can't load 'start' function");
      return;
    }

    if (isValid()) {
      getStatus = (int (*)(const std::string &configFile,
                           const std::string &serviceID, int &percent,
                           std::string &message))dlsym(handle, "getStatus");
      if (!getStatus) {
        std::cerr << "can't load 'getStatus' function\n";
        setValid(false);
        setLastError("can't load 'getStatus' function");
        return;
      }
    }

    if (isValid()) {
      getResults =
          (int (*)(const std::string &configFile, const std::string &serviceID,
                   std::list<std::pair<std::string, std::string>> &outPutList))
              dlsym(handle, "getResults");

      if (!getResults) {
        std::cerr << "can't load 'getResults' function\n";
        setValid(false);
        setLastError("can't load 'getResults' function");
        return;
      }
    }

    if (isValid()) {
      clear = (void (*)(const std::string &configFile,
                        const std::string &serviceID))dlsym(handle, "clear");
      if (!clear) {
        std::cerr << "can't load 'clear' function\n";
        setValid(false);
        setLastError("can't load 'clear' function");
        return;
      }
    }

    if (isValid()) {
      webPrepare =
          (long (*)(WorkflowExecutorWebParameters& wfpm))dlsym(handle, "webPrepare");
      if (!clear) {
        std::cerr << "can't load 'webPrepare' function\n";
        setValid(false);
        setLastError("can't load 'webPrepare' function");
        return;
      }
    }

    if (isValid()) {
      webGetPrepare = (long (*)(WorkflowExecutorWebParameters& wfpm))dlsym(
          handle, "webGetPrepare");
      if (!clear) {
        std::cerr << "can't load 'webPrepare' function\n";
        setValid(false);
        setLastError("can't load 'webPrepare' function");
        return;
      }
    }

    if (isValid()) {
      webExecute =
          (long (*)(WorkflowExecutorWebParameters& wfpm))dlsym(handle, "webExecute");
      if (!clear) {
        std::cerr << "can't load 'webExecute' function\n";
        setValid(false);
        setLastError("can't load 'webExecute' function");
        return;
      }
    }


    if (isValid()) {
      webGetStatus = (long (*)(WorkflowExecutorWebParameters& wfpm))dlsym(
          handle, "webGetStatus");
      if (!clear) {
        std::cerr << "can't load 'webGetStatus' function\n";
        setValid(false);
        setLastError("can't load 'webGetStatus' function");
        return;
      }
    }


    if (isValid()) {
      webGetResults = (long (*)(WorkflowExecutorWebParameters& wfpm,std::list<std::pair<std::string, std::string>> &outPutList))dlsym(
          handle, "webGetResults");
      if (!clear) {
        std::cerr << "can't load 'webGetResults' function\n";
        setValid(false);
        setLastError("can't load 'webGetResults' function");
        return;
      }
    }



  }

public:
  bool IsValid() { return this->isValid(); }
  std::string GetLastError() {
    return std::move(std::string(this->getLastError()));
  };

  /***
   *
   * @param configFile: the configuration
   * @param cwlFile:  user's cwl
   * @param inputsFile: input json
   * @param wpsServiceID:  wps service ID
   * @param runId: wps run id
   * @param serviceID: fill with libId
   * @return 0 OK
   */
  int (*start)(const std::string &configFile, const std::string &cwlFile,
               const std::string &inputsFile, const std::string &wpsServiceID,
               const std::string &runId, std::string &serviceID){nullptr};

  /***
   *
   * @param configFile: the configuration
   * @param serviceID
   * @param percent
   * @param message
   * @return 0 exit >0 continue
   */
  int (*getStatus)(const std::string &configFile, const std::string &serviceID,
                   int &percent, std::string &message){nullptr};

  int (*getResults)(const std::string &configFile, const std::string &serviceID,
                    std::list<std::pair<std::string, std::string>> &outPutList){
      nullptr};

  void (*clear)(const std::string &configFile,
                const std::string &serviceID){nullptr};

  long (*webPrepare)(WorkflowExecutorWebParameters& wfpm){nullptr};
  long (*webGetPrepare)(WorkflowExecutorWebParameters& wfpm){nullptr};
  long (*webExecute)(WorkflowExecutorWebParameters& wfpm){nullptr};

  long (*webGetStatus)(WorkflowExecutorWebParameters& wfpm){nullptr};

  long (*webGetResults)(WorkflowExecutorWebParameters& wfpm,
                        std::list<std::pair<std::string, std::string>> &outPutList
                        ){nullptr};
};

} // namespace mods

#endif // ADES_CODE_ENGINE_API_FRAMEWORK_ZOOARGO_HPP
