#ifndef ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP
#define ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP

#include <dlfcn.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <list>
#include <map>
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

class PepResource{
    std::string jwt_="";
    std::string name_="";
    std::string icon_uri_="";
    std::list<std::string> scopes_={};

    std::string service_="";
    std::string workspace_="";

public:

    int prepareExec(const std::map<std::string, std::string>& conf){


        return 0;
    }


    void setWorkspaceService(std::string workspace, std::string service){
        reset();
        this->workspace_ = std::move(workspace);
        this->service_ = std::move(service);
    }

    bool jwt_empty(){
        return jwt_.empty();
    }

    void setJwt(std::string jwt){
        this->jwt_=std::move(jwt);
    }

    PepResource(){}
    void reset(){
        scopes_.clear();
        icon_uri_.clear();
        name_.clear();
        service_.clear();
        workspace_.clear();
    }

    void resetAll(){
        reset();
        jwt_.clear();
    }
};


class PepRegisterResources: protected mods::IModInterface {
public:
    PepRegisterResources() = delete;
    explicit PepRegisterResources(const std::string &path)
            : mods::IModInterface(path) {
        setValid(true);

        pepSave = (int (*)(PepResource& resource ))dlsym(handle, "pepSave");
        if(!pepSave){
            std::cerr << "can't load 'PepRegisterResources.pepSave' function\n";
            setValid(false);
            setLastError("can't load 'PepRegisterResources.pepSave' function");
            return;
        }


        if (isValid()) {
            pepGets = (int (*)())dlsym(handle, "pepGets");
            if(!pepGets){
                std::cerr << "can't load 'PepRegisterResources.pepGets' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepGets' function");
                return;
            }
        }


        if (isValid()) {
            pepGet = (int (*)(const std::string& id))dlsym(handle, "pepGet");
            if(!pepGet){
                std::cerr << "can't load 'PepRegisterResources.pepGet' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepGet' function");
                return;
            }
        }


        if (isValid()) {
            pepRemove = (int (*)(const std::string& id,PepResource& resource))dlsym(handle, "pepRemove");
            if(!pepRemove){
                std::cerr << "can't load 'PepRegisterResources.pepRemove' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepRemove' function");
                return;
            }
        }

        if (isValid()) {
            pepUpdate = (int (*)(const std::string& id))dlsym(handle, "pepUpdate");
            if(!pepUpdate){
                std::cerr << "can't load 'PepRegisterResources.pepUpdate' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepUpdate' function");
                return;
            }
        }

    }

public:
    bool IsValid() { return this->isValid(); }
    std::string GetLastError() {
        return std::move(std::string(this->getLastError()));
    };

    int (*pepSave)(PepResource& resource);
    int (*pepGets)(void);
    int (*pepGet)(const std::string& id);
    int (*pepRemove)(const std::string& id,PepResource& resource);
    int (*pepUpdate)(const std::string& id);
};



}


#endif //ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP
