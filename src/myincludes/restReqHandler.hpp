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
        DatabaseMan database = * new DatabaseMan();
  
    public:
        
        void getteamdata(int page) {
            CURL* handler = curl_easy_init();

            curl_slist* headerList = NULL;
            if (handler) {
                std::string url = std::string("https://www.thebluealliance.com/api/v3/teams/" + std::to_string(page));
                // std::cout << (url.c_str()) << std::endl;
                curl_easy_setopt(handler, CURLOPT_URL, &(url.c_str()[0]));
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

            
            database.setteamdat(teams);
            database.addTeams();


        
        }
        void deleteteams() {
            database.clearTeams();
        }

        ~RestReqHandler() {
            delete &database;
        }
};

#endif