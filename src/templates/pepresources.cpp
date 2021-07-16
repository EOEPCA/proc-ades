#include "pepresources.hpp"
#include "nlohmann/json.hpp"
#include "../deployundeploy/includes/httpfuntions.hpp"


#include "nlohmann/json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>
#include <memory>
#include <list>


std::string replaceStr(std::string &str, const std::string &from,
                       const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}


namespace PEP {
    using nlohmann::json;

    inline json get_untyped(const json &j, const char *property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json &j, std::string property) {
        return get_untyped(j, property.data());
    }

}

namespace nlohmann {

    bool exists_key(const json &j, const std::string &key) {
        return j.find(key) != j.end();
    }

    void from_json(const json &j, mods::PepResource &x);

    void to_json(json &j, const mods::PepResource &x);

    inline void from_json(const json &j, mods::PepResourceResponce &x) {
        if (exists_key(j, "name"))
            x.setName(j.at("name").get<std::string>());
        if (exists_key(j, "icon_uri"))
            x.setIconUri(j.at("icon_uri").get<std::string>());

        if (exists_key(j, "icon_uri"))
            x.setScopes(j.at("scopes").get < std::vector < std::string >> ());

        if (exists_key(j, "ownership_id"))
            x.setOwnershipId(j.at("ownership_id").get<std::string>());

        if (exists_key(j, "id"))
            x.setId(j.at("id").get<std::string>());
    }

    inline void to_json(json &j, const mods::PepResource &x) {
        j = json::object();
        j["name"] = x.getName();
        j["icon_uri"] = x.getIconUri();
        j["scopes"] = x.getScopes();
    }
}


long pepDelete_(mods::PepResourceResponce &resourceResponce) {

    std::string auth{"Authorization: Bearer "};
    auth.append(resourceResponce.getJwt());
    std::string contenttype{"Content-Type: application/json"};

    std::list <std::string> list;
    list.push_back(auth);
    list.push_back(contenttype);
    std::string buffer;

    long ret = 0;
    std::cerr << "pepDelete Request: \n"
                 "Url:" << resourceResponce.getUri().c_str() << "\n"
                                                                "Method: DELETE\n"
                                                                "Content-Type: application/json\n";
    try {
        ret = postputToWeb(buffer, ""/*std::string()*/, resourceResponce.getUri().c_str(), "DELETE", &list);
//        ret = deleteToWeb(buffer, resourceResponce.getUri().c_str());
        std::cerr << "pepDelete Response:\tstatus_code: " << ret << " buffer:" << buffer << "\n";
    } catch (...) {
        std::cerr << "Error ... \n";
        ret = 199;
    }

    return ret;
}

long pepSave_(mods::PepResource &resource) {
    std::cerr << "int (*save)(PepResource& resource);\n";

    resource.dump();
    nlohmann::json json;
    nlohmann::to_json(json, resource);
    std::cerr << "\n pepSave Request:\njson: " << json.dump() << "\nUrl: " << resource.getUri().c_str() << "\n";

    std::string auth{"Authorization: Bearer "};
    auth.append(resource.getJwt());

    std::string contenttype{"Content-Type: application/json"};

    std::list <std::string> list;
    list.push_back(auth);
    list.push_back(contenttype);
    std::string buffer;

    long ret = 0;
    try {
        ret = postputToWeb(buffer, json.dump(), resource.getUri().c_str(), "POST", &list);
        std::cerr << "pepSave Response:\tstatus_code: " << ret << " json:" << buffer << "\n";
    } catch (...) {

        return 199;
    }

    switch (ret) {
        case 200: {
            std::cerr << "pepSave:\treturn: " << ret << " json:" << buffer << "\n";
            mods::PepResourceResponce msgWeb = nlohmann::json::parse(buffer);
            msgWeb.dump();
        }
            break;
        case 422:{
            std::cerr << "pepSave: \treturn: " << ret << " (resource already registered) \n";
        }
            break;
        case 401:
        case 404:
        default:
            std::cerr << "pepSave:\treturn: " << ret << " json:" << buffer << "\n";
            break;
    }

    return ret;
}


long
pepGets_(mods::PepResource &resource, std::list <std::unique_ptr<mods::PepResourceResponce>> *resources = nullptr) {

    std::cerr << "int (*pepGets)(void);\n";

    std::string auth{"Authorization: Bearer "};
    auth.append(resource.getJwt());

    std::list <std::string> list;
    list.push_back(auth);
    std::string buffer;

    long ret = 199;
    try {
        ret = getFromWeb(buffer, resource.getUri().c_str(), &list);
    } catch (...) {
        return 199;
    }

    switch (ret) {
        case 200: {
            std::cerr << "pepGets:\treturn: " << ret << " json:" << buffer << "\n";
            std::vector <mods::PepResourceResponce> msgWeb = nlohmann::json::parse(buffer);
            if (resources)
                for (auto &a:msgWeb) {
                    auto v = std::make_unique<mods::PepResourceResponce>();
                    *v = a;
                    resources->push_back(std::move(v));
//                    a.dump();
                }
        }
            break;
        case 401:
        case 404:
        default:
            std::cerr << "pepGets:\treturn: " << ret << " json:" << buffer << "\n";
            break;
    }

    return ret;
}

extern "C" long pepSave(mods::PepResource &resource) {
    return pepSave_(resource);
}


extern "C" long pepGets(mods::PepResource &resource) {
    return pepGets_(resource);
}

extern "C" long pepGet(const std::string &id, mods::PepResource &resource) {
    std::cerr << "int (*pepGet)(const std::string& id);\n";
    return 0;
}

extern "C" long pepRemove(const std::string &id, mods::PepResource &resource) {
    std::cerr << "int (*pepRemove)(const std::string& id,PepResource& resource);\n";

    return 0;
}

extern "C" long pepRemoveFromZoo(const char *path, const char *host/*base uri*/, char *jwt, int stopOnError) {
    std::cerr << "pepRemoveFromZoo(const std::string& id,const char* host)\n";

    std::string sPath{path};
    std::string baseUri(host);
    std::string auth{"Authorization: Bearer "};
    auth.append(std::string(jwt));


    std::list <std::string> list;
    list.push_back(auth);
    std::string buffer;

    mods::PepResource resource;
    resource.setUri(baseUri + "/resources");
    resource.setJwt(std::string(jwt));

    std::list <std::unique_ptr<mods::PepResourceResponce>> resources{};

    long ret = pepGets_(resource, &resources);
    if (ret != 200) {
        if ( ret == 404 ){
            std::cerr << "pep service did not find the deployed app and returned 404.\n The Ades will proceed anyway with the undeploy.\n";
            return 200;
        }
        else if (stopOnError) {
            return ret;
        }
        else
            return 200;
    }

    std::cerr << "*pepRemoveFromZoo* \n";
    for (auto &r:resources) {
        r->dump();

        std::string ns{sPath};
        replaceStr(ns, "wps3", "watchjob");

        if (r->getIconUri().rfind(sPath, 0) == 0 /*r->getIconUri() == sPath*/) {
            r->setUri(baseUri + "/resources/" + r->getId());
            r->setJwt(std::string(jwt));
            long retDel = pepDelete_(*r);
            if (retDel != 200) {
                if (stopOnError) {
                    return retDel;
                }
            }
        }

        if (r->getIconUri().rfind(ns, 0) == 0) {
            r->setUri(baseUri + "/resources/" + r->getId());
            r->setJwt(std::string(jwt));
            long retDel = pepDelete_(*r);
            if (retDel != 200) {
                if (stopOnError) {
                    return retDel;
                }
            }
        }


        std::cerr << "\n";
    }
    return 200;
}

extern "C" long pepUpdate(const std::string &id, mods::PepResource &resource) {
    std::cerr << "int (*pepUpdate)(const std::string& id,PepResource& resource);\n";
    return 0;
}




