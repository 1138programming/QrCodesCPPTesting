#ifndef RESTREQHANDLER_HPP
#define RESTREQHANDLER_HPP

#include "../include/curlIncludes/curl/curl.h"

#include <vector>
#include <iostream>
#include <string>



class RestReqHandler {
    private:
        static size_t readHandler(char* ptr, size_t size, size_t numElements, void* ourPtr) {
            std::cout << std::string(ptr, numElements);
            return numElements;
        }


    public:
        void test() {
            std::cout << " yep" << std::endl;
            CURL* handler = curl_easy_init();

            // curl_slist* headerList = NULL;
            // if (handler) {
            //     curl_easy_setopt(handler, CURLOPT_URL, "https://www.thebluealliance.com/api/v3/teams/0");

            //     headerList = curl_slist_append(headerList, "X-TBA-Auth-Key: IOB4YXM4T4wE485Kspo8exJBnjhCVoS5bMKxPhcGIrlDA2dIb3iAyNhX76YLRJB0");

            //     curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headerList);
            //     curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, readHandler);

            //     curl_easy_perform(handler);
            // }
            // curl_easy_cleanup(handler);
        }
};

#endif