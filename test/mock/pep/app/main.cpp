#include <iostream>
#include <dlfcn.h>
#include <memory>
#include <map>

#include "pepresources.hpp"

int main(int a,char** b){
//    void *handle{nullptr};
//    handle = dlopen(path_.data(), RTLD_LAZY);
//    if (!handle) {
//        return 1;
//    }
//    int (*pepRemoveFromZoo)(const char* id,const char* host) =
//            (int (*)(const char* id,const char* host))dlsym(handle, "pepRemoveFromZoo");
//    if(!pepRemoveFromZoo){
//        return 2;
//    }
//    pepRemoveFromZoo("casca","plooo");

   auto pepRegisterResources=std::make_unique<mods::PepRegisterResources>("./libpep_resource.so");
   auto resource = std::make_unique<mods::PepResource>();
    resource->setJwt("setJwtsetJwt.setJwtsetJwtsetJwtsetJwtsetJwtset.JwtsetJwtsetJwtsetJwt");

    std::map<std::string, std::string> confPep;
    confPep["pepresource"]="/opt/t2service/libpep_resource.so";
    confPep["usepep"]="true";
    confPep["pephost"]="http://mock-pep";
    confPep["scopes"]="public";
    confPep["pathBase"]="/%s/wps3/processes/%s";
    confPep["pathStatus"]="/%s/watchjob/processes/%s/jobs/%s";
    confPep["pathResult"]="/%s/watchjob/processes/%s/jobs/%s/result";
    confPep["stopOnError"]="true";

    resource->setWorkspaceService( "rdirienzo","vegetation");
    resource->prepareStatus(confPep,"wfpm->runID");
    long retCodePep = pepRegisterResources->pepSave(*(resource.get()));

    std::cout << "---------------\n";

    pepRegisterResources->pepGets(*(resource.get()));


    return 0;
}
/*
cp /work/test/mock/pep/app/main.cpp  /project/test/mock/pep/app/main.cpp
rsync -av  /work/src/templates/* /project/src/templates/
make  pep_resource   pep_test
 */

