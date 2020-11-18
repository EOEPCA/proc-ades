#include "pepresources.hpp"
#include "nlohmann/json.hpp"
#include "../deployundeploy/includes/httpfuntions.hpp"


#include "nlohmann/json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>
#include <list>

namespace PEP {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

}

namespace nlohmann {

    bool exists_key(const json &j, const std::string &key) {
        return j.find(key) != j.end();
    }

    void from_json(const json & j, mods::PepResource & x);
    void to_json(json & j, const mods::PepResource & x);

    inline void from_json(const json & j, mods::PepResourceResponce& x) {
        if (exists_key(j, "name"))
            x.setName(j.at("name").get<std::string>());
        if (exists_key(j, "icon_uri"))
            x.setIconUri(j.at("icon_uri").get<std::string>());

        if (exists_key(j, "icon_uri"))
            x.setScopes(j.at("scopes").get<std::vector<std::string>>());

        if (exists_key(j, "ownership_id"))
            x.setOwnershipId(j.at("ownership_id").get<std::string>());

        if (exists_key(j, "id"))
            x.setId(j.at("id").get<std::string>());
    }

    inline void to_json(json & j, const mods::PepResource & x) {
        j = json::object();
        j["name"] = x.getName();
        j["icon_uri"] = x.getIconUri();
        j["scopes"] = x.getScopes();
    }
}

extern "C" int pepSave(mods::PepResource& resource){
    std::cerr << "int (*save)(PepResource& resource);\n";

    resource.dump();
    nlohmann::json json;
    nlohmann::to_json(json,resource);
    std::cerr << "\n to send: "<< json.dump()  << "\n";

    std::string auth{"Authorization: Bearer "};
    auth.append(resource.getJwt());

    std::list<std::string> list;
    list.push_back(auth);
    std::string buffer;

    auto ret = postputToWeb(buffer, json.dump(), resource.getUri().c_str(), "POST",&list);
    std::cerr << "pepSave:\treturn: " << ret << " json:" << buffer <<"\n";

    switch (ret) {
        case 200:{
                std::cerr << "pepSave:\treturn: " << ret << " json:" << buffer <<"\n";
                mods::PepResourceResponce msgWeb= nlohmann::json::parse(buffer);
                msgWeb.dump();
            }
            break;
        case 401:
        case 404:
        default:
            std::cerr << "pepSave:\treturn: " << ret << " json:" << buffer <<"\n";
            break;
    }

    return ret;
}


extern "C" int pepGets(){
    std::cerr << "int (*pepGets)(void);\n";
    return 0;
}

extern "C" int pepGet(const std::string& id){
    std::cerr << "int (*pepGet)(const std::string& id);\n";
    return 0;
}

extern "C" int pepRemove(const std::string& id,mods::PepResource& resource){
    std::cerr << "int (*pepRemove)(const std::string& id,PepResource& resource);\n";
    return 0;
}

extern "C" int pepUpdate(const std::string& id){
    std::cerr << "int (*pepUpdate)(const std::string& id,PepResource& resource);\n";
    return 0;
}




