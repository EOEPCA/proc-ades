#ifndef ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP
#define ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP

#include <dlfcn.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <sstream>
#include <cstring>
#include <cstdarg>

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
    std::string uri_;

    std::string jwt_;
    std::string name_;
    std::string icon_uri_;
    std::vector<std::string> scopes_={};

    std::string service_;
    std::string workspace_;
public:

    std::string getUri() const {
        return uri_;
    }

    std::string getName() const {
        return name_;
    }

    std::string getIconUri() const {
        return icon_uri_;
    }

    const std::vector<std::string> &getScopes() const {
        return scopes_;
    }

    std::string getJwt(){
        return jwt_;
    }


    void setName(const std::string &name) {
        name_ = name;
    }

    void setIconUri(const std::string &iconUri) {
        icon_uri_ = iconUri;
    }

    void setScopes(const std::vector<std::string> &scopes) {

        for(auto&s:scopes)
            scopes_.push_back(s);
    }

protected:


    int add_nums(char* budder,int count, ...){

    }

    template <typename... Args>
    static std::string string_format(const std::string &format, Args... args) {
        size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) +1;
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(),buf.get() + size - 1);
    }

    template <typename Out>
    static void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }


    std::unique_ptr<char[]>  prepareMem(int size){
        auto rt = std::make_unique<char[]>(size);
        std::memset(rt.get(),0,size);
        return rt;
    }

    void addScopes(std::map<std::string, std::string>& conf){

        std::string sScoops = conf["scopes"];
        if (sScoops.empty()){
            sScoops="public";
        }

        std::vector<std::string> scoops = split(sScoops,'|');
        for(auto&s: scoops){
            scopes_.push_back(s);
        }
    }

    int prepareStatusResult(std::map<std::string, std::string>& conf, std::string formatter,std::string id){
        reset();

        name_ = service_;
        std::string pathBase = conf[formatter];
        addScopes(conf);

        int max{1024*3};
        std::cerr << "\npathBase: " << pathBase << "\n";
        auto co=prepareMem(max);
        std::snprintf(co.get(), max-1, pathBase.c_str(),workspace_.c_str(),service_.c_str(),id.c_str());
        icon_uri_ = std::string(co.get());
        std::cerr << "\nicon_uri_: " << icon_uri_ << "\n";

        this->uri_ = conf["pephost"];
        this->uri_.append("/resources");

        return 0;
    }


public:

    int prepareStatus(std::map<std::string, std::string>& conf,std::string id){
        return prepareStatusResult(conf,"pathStatus",std::move(id));
    }

    int prepareResults(std::map<std::string, std::string>& conf,std::string id){
        return prepareStatusResult(conf,"pathResult",std::move(id));
    }

    int prepareBase(std::map<std::string, std::string>& conf){
        reset();
        name_ = service_;
        std::string pathBase = conf["pathBase"];
        addScopes(conf);

        int max{1024*3};
        std::cerr << "\npathBase: " << pathBase << "\n";
        auto co=prepareMem(max);
        std::snprintf(co.get(), max-1, pathBase.c_str(),workspace_.c_str(),service_.c_str() );
        icon_uri_ = std::string(co.get());
        std::cerr << "\nicon_uri_: " << icon_uri_ << "\n";

        this->uri_ = conf["pephost"];
        this->uri_.append("/resources");

        return 0;
    }

    void setJwt(std::string jwt){
        this->jwt_=std::move(jwt);
    }

    bool jwt_empty(){
        return jwt_.empty();
    }

    virtual void dump(){
        std::cerr << "jwt_: " << jwt_ << " name_: " << name_ << " icon_uri_: " << icon_uri_
                  << " scopes_: " << " service_: " << service_ << " workspace_: "
                  << workspace_ << " scopes: ";
        for(auto&s:scopes_){
            std::cerr << s << "| ";
        }
        std::cerr<<"\n";
    }

    virtual void resetAll(){
        reset();
        jwt_.clear();
        service_.clear();
        workspace_.clear();
    }

    virtual void reset(){
        scopes_.clear();
        icon_uri_.clear();
        name_.clear();
        uri_.clear();
    }

    void setWorkspaceService(std::string workspace, std::string service){
        this->workspace_ = std::move(workspace);
        this->service_ = std::move(service);
        std::cerr << "------------ssss-----------------\n";

    }
};

class PepResourceResponce: public PepResource{
    std::string ownership_id_;
    std::string id_;
public:

    void setOwnershipId(const std::string &ownershipId) {
        ownership_id_ = ownershipId;
    }

    void setId(const std::string &id) {
        id_ = id;
    }

    void reset() override{
        PepResource::reset();
        id_.clear();
        ownership_id_.clear();
    }

    void dump() override{
        PepResource::dump();
        std::cerr << "ownership_id_: " << ownership_id_ << " id_: " << id_ << "\n";
    }
};

class PepRegisterResources: protected mods::IModInterface {
public:
    PepRegisterResources() = delete;
    explicit PepRegisterResources(const std::string &path)
            : mods::IModInterface(path) {
        setValid(true);

        pepSave = (long (*)(PepResource& resource))dlsym(handle, "pepSave");
        if(!pepSave){
            std::cerr << "can't load 'PepRegisterResources.pepSave' function\n";
            setValid(false);
            setLastError("can't load 'PepRegisterResources.pepSave' function");
            return;
        }

        if (isValid()) {
            pepGets = (long (*)(PepResource& resource))dlsym(handle, "pepGets");
            if(!pepGets){
                std::cerr << "can't load 'PepRegisterResources.pepGets' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepGets' function");
                return;
            }
        }

        if (isValid()) {
            pepGet = (long (*)(const std::string& id,PepResource& resource))dlsym(handle, "pepGet");
            if(!pepGet){
                std::cerr << "can't load 'PepRegisterResources.pepGet' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepGet' function");
                return;
            }
        }


        if (isValid()) {
            pepRemove = (long (*)(const std::string& id,PepResource& resource))dlsym(handle, "pepRemove");
            if(!pepRemove){
                std::cerr << "can't load 'PepRegisterResources.pepRemove' function\n";
                setValid(false);
                setLastError("can't load 'PepRegisterResources.pepRemove' function");
                return;
            }
        }

        if (isValid()) {
            pepUpdate = (long (*)(const std::string& id,PepResource& resource))dlsym(handle, "pepUpdate");
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

    long (*pepSave)(PepResource& resource);
    long (*pepGets)(PepResource& resource);
    long (*pepGet)(const std::string& id,PepResource& resource);
    long (*pepRemove)(const std::string& id,PepResource& resource);
    long (*pepUpdate)(const std::string& id,PepResource& resource);
};



}


#endif //ADES_CODE_ENGINE_API_FRAMEWORK_PEPRESOURCES_HPP
