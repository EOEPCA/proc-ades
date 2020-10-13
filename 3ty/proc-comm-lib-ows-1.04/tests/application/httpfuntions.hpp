
#ifndef PROC_COMM_ZOO_SVC_HTTPFUNTIONS_HPP
#define PROC_COMM_ZOO_SVC_HTTPFUNTIONS_HPP

#include <curl/curl.h>

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

long getFromWeb(std::string &buffer, const char *path) {
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
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
    } else {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
  }

  return response_code;
}

#endif
