#define CURL_STATICLIB

#ifndef CURLOBJ_H
#define CURLOBJ_H

#include <curl/curl.h>
#include <string>

class CurlObj {
public:
    CurlObj(std::string url) {
        curl = curl_easy_init();
        if (!curl) { throw std::string("Curl did not initialize."); }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlObj::curlWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_perform(curl);
    };

    static int curlWriter(char* data, int size, int nmemb, std::string* buffer) {
        int result = 0;
        if (buffer != NULL) {
            buffer->append(data, size * nmemb);
            result = size * nmemb;
        }
        return result;
    }

    std::string getData() { return curlBuffer; }

protected:
    CURL* curl;
    std::string curlBuffer;
};

#endif