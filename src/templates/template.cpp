#include "service.h"
#include "service_internal.h"

#include <dlfcn.h>
#include <string>


void setStatus(maps *&conf, const char *status, const char *message) {
  map *usid = getMapFromMaps(conf, "lenv", "uusid");
  map *r_inputs = NULL;
  r_inputs = getMapFromMaps(conf, "main", "tmpPath");
  char *flenv = (char *)malloc(
      (strlen(r_inputs->value) + strlen(usid->value) + 12) * sizeof(char));
  sprintf(flenv, "%s/%s_lenv.cfg", r_inputs->value, usid->value);
  setMapInMaps(conf, "lenv", "message", message);
  setMapInMaps(conf, "lenv", "status", status);
  maps *lenvMaps = getMaps(conf, "lenv");
  dumpMapsToFile(lenvMaps, flenv, 0);
  free(flenv);
}

extern "C" {

ZOO_DLL_EXPORT int SERVICENAME(maps *&conf, maps *&inputs, maps *&outputs) {

  void *handle{nullptr};
  handle = dlopen("/opt/t2libs/libinterface.so", RTLD_LAZY);
  int (*run)(maps *&, maps *&, maps *&);

  run=(int (*)(maps *&, maps *&, maps *&))dlsym(handle, "interface");
  if (!run){
    setStatus(conf, "failed", "can't load /opt/t2libs/libinterface.so");
    return SERVICE_FAILED;
  }

  return run(conf,inputs,outputs) ;
}
}
