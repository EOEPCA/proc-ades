#include "pepresources.hpp"
#include "nlohmann/json.hpp"
#include "../deployundeploy/includes/httpfuntions.hpp"


extern "C" int pepSave(mods::PepResource& resource){
    std::cerr << "int (*save)(PepResource& resource);\n";
    return 0;
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




