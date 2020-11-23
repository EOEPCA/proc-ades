
#ifndef PROC_COMM_ZOO_SVC_HTTPFUNTIONS_HPP
#define PROC_COMM_ZOO_SVC_HTTPFUNTIONS_HPP

#include <curl/curl.h>
#include <string>
#include <list>
#include <iostream>


size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size,
                                        size_t nmemb, std::string *s) {
  size_t newLength = size * nmemb;
  try {
    s->append((char *)contents, newLength);
  } catch (std::bad_alloc &e) {
    // handle memory problem
    return 0;
  }
  return newLength;
}



//long deleteToWeb(std::string &buffer,const char *path,std::list<std::string>* headers= nullptr){
//
//    long response_code;
//    CURL *curl;
//    CURLcode res;
//    curl_global_init(CURL_GLOBAL_DEFAULT);
//    curl = curl_easy_init();
//
//    std::cerr << "&&&&&&&&&&&&&&&&&&&" << path << "\n";
//
//    response_code = 0;
//    if (curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, path);
//        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
//
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // only for https
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // only for https
//
//        res = curl_easy_perform(curl);
//
//
//        std::cerr << "CASACASACASACASA " << res << "\n";
//
//        if (res != CURLE_OK) {
//        } else {
//            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
//        }
//
//        curl_easy_cleanup(curl);
//
//    }
//
//    return response_code;
//
//
//}

long postputToWeb(std::string &buffer,const std::string& content, const char *path,const char* method/*POST/PUT*/,std::list<std::string>* headers= nullptr){


  long response_code;
  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  response_code = 0;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, path);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // only for https
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // only for https

      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                       CurlWrite_CallbackFunc_StdString);


    if (!content.empty()){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    }

    struct curl_slist *chunk = NULL;
    if (headers!= nullptr){
        for(auto&s:*headers){
            chunk = curl_slist_append(chunk, s.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
    } else {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
    if (chunk)
        curl_slist_free_all(chunk);
  }

  return response_code;
}


long getFromWeb(std::string &buffer, const char *path,std::list<std::string>* headers= nullptr) {
  long response_code;
  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  response_code = 0;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, path);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // only for https
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // only for https
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

      struct curl_slist *chunk = NULL;
      if (headers!= NULL){
          for(auto&s:*headers){
              chunk = curl_slist_append(chunk, s.c_str());
          }
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      }

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
    } else {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
  }

  return response_code;
}

#endif
