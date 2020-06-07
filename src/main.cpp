#include "templates/zooargo.hpp"
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>

#define ARGO_URI "http://localhost:8080"
#define K8_URI "http://127.0.0.1:2746"
#define CWLPATH                                                                \
  "/home/bla/dev/EOEPCA_dev/proc-ades/assets/workflow.yaml"
#define LIBPATH                                                                \
  "/home/bla/dev/EOEPCA_dev/proc-ades/build/libargo_interface.so"

#define EOEPCAARGOPATH "/home/bla/dev/EOEPCA_dev/proc-ades/build/3ty/proc-comm-lib-argo/libeoepcaargo.so"
#define ARGOCONFIG "/home/bla/dev/EOEPCA_dev/proc-ades/assets/argo.json"
struct Util {

    static int loadFile(const char *filePath, std::stringstream &sBuffer) {
        std::ifstream infile(filePath);
        if (infile.good()) {
            sBuffer << infile.rdbuf();
            return 0;
        }
        return 1;
    }
};

int main(int a, char **b) {

    auto argoInterface = std::make_unique<mods::ArgoInterface>(LIBPATH);
    if (!argoInterface->IsValid()) {

        std::cerr << "libargo_interface.dylib NOT VALID\n";
        return 1;
    }

    std::stringstream cwlBuffer;
    Util::loadFile(CWLPATH, cwlBuffer);

    std::stringstream argoConfigJsonBuffer;
    Util::loadFile(ARGOCONFIG, argoConfigJsonBuffer);

    auto argoConfig = std::make_unique<mods::ArgoInterface::ArgoWorkflowConfig>();
    argoConfig->argoUri = ARGO_URI;
    argoConfig->k8Uri = K8_URI;
    argoConfig->eoepcaargoPath = EOEPCAARGOPATH;
    argoConfig->argoConfigFile = argoConfigJsonBuffer.str();

    std::string uuidBaseID("6f189344");
    std::string runId("argoservice");
    std::string argoWorkflowId("");

    std::list<std::pair<std::string, std::unique_ptr<mods::ArgoInterface::tgInput>>> inputList;

    std::unique_ptr<mods::ArgoInterface::tgInput> base_dir = std::make_unique<mods::ArgoInterface::tgInput>();
    std::unique_ptr<mods::ArgoInterface::tgInput> s3_catalog_url = std::make_unique<mods::ArgoInterface::tgInput>();

    base_dir->value = "/tmp/eoepca";
    base_dir->id = "base_dir";
    base_dir->mimeType = "";

    s3_catalog_url->value = "https://catalog.terradue.com/sentinel3/search?uid=S3B_SL_1_RBT____20200520T050759_20200520T051059_20200520T060015_0179_039_105_0360_LN2_O_NR_004";
    s3_catalog_url->id = "input_file";
    s3_catalog_url->mimeType = "application/atom+xml";

    //inputList.emplace_back(std::make_pair<std::string, std::unique_ptr<mods::ArgoInterface::tgInput>>("arg1", std::move(base_dir)));
    inputList.emplace_back(std::make_pair<std::string, std::unique_ptr<mods::ArgoInterface::tgInput>>("arg2", std::move(s3_catalog_url)));

    argoInterface->start(*argoConfig.get(), cwlBuffer.str(), inputList, uuidBaseID, runId, argoWorkflowId);

    std::cout << "argoWorkflowId: " << argoWorkflowId << std::endl;

    int percent = 0;
    std::string message("Running");

    while (message.compare("Running") == 0) {
        sleep(4);
        argoInterface->getStatus(*argoConfig.get(), argoWorkflowId, percent, message);
        std::cout << "percent: " << percent << " message: " << message << std::endl;
    }

    if (message.compare("Succeeded") == 0) {
        std::list<std::pair<std::string, std::string>> outPutList;
        argoInterface->getResults(*argoConfig.get(), argoWorkflowId, outPutList);

        for (auto &[k, p] : outPutList) {
            std::cout << k << " " << p << "\n";
        }
    }
    return 0;
}