#ifndef RESTREQHANDLER_HPP
#define RESTREQHANDLER_HPP

#include "../include/curlIncludes/curl/curl.h"

#include <vector>
#include <iostream>
#include <string>


static std::string output;
class RestReqHandler {
    private:
        static size_t readHandler(char* ptr, size_t size, size_t numElements, void* ourPtr) {
            std::string temp = std::string(ptr, size * numElements);
            output.append(temp);
            


           
            return size * numElements;

        }

  
    public:
        
        void getteamdata() {
            CURL* handler = curl_easy_init();

            curl_slist* headerList = NULL;
            if (handler) {
                curl_easy_setopt(handler, CURLOPT_URL, "https://www.thebluealliance.com/api/v3/teams/0");
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYPEER, false);
                // curl_easy_setopt(handler, CURLOPT_URL, "http://eu.httpbin.org/ip");

                headerList = curl_slist_append(headerList, "X-TBA-Auth-Key: IOB4YXM4T4wE485Kspo8exJBnjhCVoS5bMKxPhcGIrlDA2dIb3iAyNhX76YLRJB0");

                // curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headerList);
                curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headerList);
                curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, readHandler);
               

                CURLcode res = curl_easy_perform(handler); 
                std::cout << "res: " << res << std::endl;
                // if (res == CURLE_OK) {
                // char* Far;
                // res = curl_easy_getinfo(handler, CURLINFO_CONTENT_TYPE, &Far);
                // if (res == CURLE_OK) {
                //     std::string str(Far);
                //     std::cout << str << std::endl;
                // }
                // }
            }
            curl_easy_cleanup(handler);

            // std::cout << output;
            JsonParser parser(output); 
            std::vector<TEAM_DATAPOINT> teams = parser.parseAPI();
            TEAM_DATAPOINT temp;

            
        

            for (auto i = teams.begin(); i != teams.end(); ++i) {
                if (i.base() != NULL) {
                    temp = *i.base();

                    std::cout << temp.teamNum << std::endl;
                }
                else {
                std::cout << " it doesn't work, it is null" << std::endl;
                }   
            }

            // DatabaseMan database(teams)

            // database.maketh();


        
        }
};

#endif