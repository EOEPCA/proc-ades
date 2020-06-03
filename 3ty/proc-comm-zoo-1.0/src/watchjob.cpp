
#include "IniReader.h"

#include <cgicc/Cgicc.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/HTTPContentHeader.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTTPResponseHeader.h>
#include <cgicc/HTTPStatusHeader.h>

#include <json/json_object.h>

#include <memory>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using cgicc::HTTPHTMLHeader;
using cgicc::HTTPRedirectHeader;
using cgicc::HTTPResponseHeader;
using cgicc::HTTPStatusHeader;

#define HTTPSTATUSM(x, y) cgicc::HTTPStatusHeader(x, y)
#define M1024 1024

/*
void setStatus(maps*& conf, const char* status, const char* message){

  map* usid = getMapFromMaps (conf, "lenv", "uusid");
  map *r_inputs = NULL;
  r_inputs = getMapFromMaps (conf, "main", "tmpPath");
  char *flenv =(char *) malloc ((strlen (r_inputs->value) +strlen (usid->value)
+ 12) * sizeof (char)); sprintf (flenv, "%s/%s_lenv.cfg", r_inputs->value,
usid->value); setMapInMaps(conf, "lenv", "message",message ); setMapInMaps(conf,
"lenv", "status",status  ); maps* lenvMaps=getMaps(conf,"lenv");
  dumpMapsToFile(lenvMaps,flenv,0);
  free(flenv);
}
*/

//{
//"jobID" : "81574318-1eb1-4d7c-af61-4b3fbcf33c4f",
// "status": "accepted",
//"message": "Process started",
//"progress": 0,
//"links": [
//{
//"href":
//"http://processing.example.org/processes/EchoProcess/jobs/81574318-1eb1-4d7c-
// af61-4b3fbcf33c4f", "rel": "self", "type": "application/json", "title": "this
// document" } ]
//}

// sprintf(workingBoundaryData, "\r\n--%s", cgiMultipartBoundary);
// encrypted.reset(new unsigned char[RSA_size(rsaPublic_key.get())]);
// std::unique_ptr<unsigned char[]> encrypted{ nullptr };
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

void sendStatus(const char* status,const char *uri, const std::string &jobID,
                std::vector<std::string> &params) {

  std::string progress("0");
  std::string message("Process started");

  if (params.size() == 0) {

  } else if (params.size() >= 2) {

    progress = params[0];
    message = "";
    for (int i = 1; i < params.size(); i++)
      message += params[i];
  }

  std::unique_ptr<json_object, decltype(&json_object_put)> js(
      json_object_new_object(), &json_object_put);
  std::unique_ptr<json_object, decltype(&json_object_put)> links(
      json_object_new_object(), &json_object_put);
  std::unique_ptr<json_object, decltype(&json_object_put)> arrayV(
      json_object_new_array(), &json_object_put);

  json_object_object_add(links.get(), "href", json_object_new_string(uri));
  json_object_object_add(links.get(), "rel", json_object_new_string("self"));
  json_object_object_add(links.get(), "type",
                         json_object_new_string("application/json"));
  json_object_object_add(links.get(), "title",
                         json_object_new_string("get Status"));

  json_object_array_add(arrayV.get(), links.get());

  std::cout << "Content-Type: application/json;charset=UTF-8\r\n\r\n";

  json_object_object_add(js.get(), "jobID",
                         json_object_new_string(jobID.c_str()));
  json_object_object_add(js.get(), "status",
                         json_object_new_string(status));
  json_object_object_add(js.get(), "message",
                         json_object_new_string(message.c_str()));
  json_object_object_add(js.get(), "progress",
                         json_object_new_string(progress.c_str()));

  json_object_object_add(js.get(), "links", arrayV.get());

  std::cout << json_object_to_json_string(js.get());
}

int sendFileNotFound(const std::string &file) {}

int loadFile(const char *filePath, std::stringstream &sBuffer) {
  std::ifstream infile(filePath);
  if (infile.good()) {
    sBuffer << infile.rdbuf();
    return 0;
  }
  return 1;
}

int sendFinalResult(std::stringstream &finalBuffer) {

  if (finalBuffer.str().empty()) {
    std::cout << HTTPSTATUSM(500, "The result file is not ready") << std::endl;
  } else {
    auto pos = finalBuffer.str().find("{\"outputs\"");
    if (pos != std::string::npos) {

      const char *p = finalBuffer.str().c_str();
      p = p + (pos);
      std::cout << "Content-Type: application/json;charset=UTF-8\r\n\r\n";
      std::cout << p;
    }
  }
  return (0);
}

int main(int argc, char **argv, char **envp) {

  //    std::cout << cgicc::HTTPHTMLHeader() << std::endl;
  //    for (char **env = envp; *env != 0; env++)
  //    {
  //      char *thisEnv = *env;
  //      printf("%s\n", thisEnv);
  //    }
  //    return 0;

  cgicc::Cgicc cgi;
  auto cgiEnv = cgi.getEnvironment();

  auto splitV = split(std::string(cgiEnv.getPathInfo()), '/');
  if (splitV.size() < 4) {
    std::cout << HTTPSTATUSM(500, "Wrong parameter numbers") << std::endl;
    return 0;
  }

  std::string processId("");
  std::string serviceId("");

  bool isResult = false;
  if (splitV[splitV.size() - 1] == "result") {
    processId = splitV[splitV.size() - 2];
    serviceId = splitV[splitV.size() - 4];
    isResult = true;
  } else {
    processId = splitV[splitV.size() - 1];
    serviceId = splitV[splitV.size() - 3];
  }

  auto configFile = std::make_unique<char[]>(M1024);
  std::memset(configFile.get(), '\0', M1024);
  auto statusFile = std::make_unique<char[]>(M1024);
  std::memset(statusFile.get(), '\0', M1024);
  auto finalFile = std::make_unique<char[]>(M1024);
  std::memset(statusFile.get(), '\0', M1024);

  sprintf(configFile.get(), "/var/www/html/res/%s_lenv.cfg", processId.c_str());
  sprintf(statusFile.get(), "/var/www/html/res/%s.status", processId.c_str());
  sprintf(finalFile.get(), "/var/www/html/res/%s_final_%s.json",
          serviceId.c_str(), processId.c_str());

  INIReader info(std::string(configFile.get()));
  std::string theStatus = info.Get("lenv", "status", "none");
  std::string theMessage = info.Get("lenv", "status", "none");

  std::stringstream statusBuffer;
  std::stringstream finalBuffer;


  const char *CONTEXT_PREFIX = std::getenv("CONTEXT_PREFIX");
  const char *HTTP_HOST = std::getenv("HTTP_HOST");
  const char *REQUEST_SCHEME = std::getenv("REQUEST_SCHEME");
  auto uri = std::make_unique<char[]>(M1024 * 2);
  std::memset(uri.get(), '\0', M1024 * 2);
  sprintf(uri.get(), "%s://%s%s%s", REQUEST_SCHEME, HTTP_HOST,
          CONTEXT_PREFIX, cgiEnv.getPathInfo().c_str());



  if (isResult) {
    if (loadFile(finalFile.get(), finalBuffer) == 0) {
      return sendFinalResult(finalBuffer);
    } else {
      std::cout << HTTPSTATUSM(404, "Can't load the result file") << std::endl;
      return 0;
    }

  } else {

    if (loadFile(statusFile.get(), statusBuffer) == 0) {
      auto statusV = split(statusBuffer.str(), '|');
      sendStatus(theStatus.c_str(), uri.get(), processId, statusV);

    } else {
      if (theStatus=="none"){
        std::cout << HTTPSTATUSM(404, "Can't load the status File file")
                  << std::endl;
        return 0;
      }

      if (theStatus=="running"){
        std::vector<std::string> vMessage{"0",theMessage};
        sendStatus(theStatus.c_str(), uri.get(), processId, vMessage);
        return 0;
      }

      std::vector<std::string> vMessage{"100",theMessage};
      sendStatus(theStatus.c_str(), uri.get(), processId, vMessage);

    }
  }

  return 0;
}
