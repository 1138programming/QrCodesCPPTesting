#ifndef RESTREQHANDLER_HPP
#define RESTREQHANDLER_HPP

#include "../include/curlIncludes/curl/curl.h"

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <functional>


class RestReqHandler {
    private:
    DatabaseMan* database = new DatabaseMan();
    
    static size_t readHandler(char* ptr, size_t size, size_t numElements, void* ourPtr) {
        RestReqHandler* self = (RestReqHandler*)ourPtr;
        std::string temp = std::string(ptr, size * numElements);
        self->output.append(temp);
        
        return size * numElements;
    }
    public:
        std::string output; // don't access- it's usually junk data
        /**
         * @brief appends the ```request``` string to after the /v3/ in the link, using the api key given in /src/resources/tbaKey.env
         */
        std::string makeTBAReq(std::string request) {
            CURL* handler = curl_easy_init();

            curl_slist* headerList = NULL;
            this->output.clear();
            if (handler) {
                std::string url = std::string("https://www.thebluealliance.com/api/v3/") + request;
                curl_easy_setopt(handler, CURLOPT_URL, url.c_str());
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYPEER, false);

                // read header file and set header
                std::ifstream headerFile("resources/tbaKey.env");
                if (!headerFile.good()) {
                    DebugConsole::println("ERROR: Unable to make TBA Request. Please add a TBA api key into /resources/tbaKey.env to use TBA.", DBGC_RED);
                    return std::string();
                }
                std::string tbaKey;
                    std::getline(headerFile, tbaKey);
                std::string header = std::string("X-TBA-Auth-Key: ") + tbaKey;
                headerList = curl_slist_append(headerList, header.c_str());
                curl_easy_setopt(handler, CURLOPT_HTTPHEADER, headerList);
                curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, readHandler);
                curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);

                //perform query
                CURLcode res = curl_easy_perform(handler); 
                if (res != CURLcode::CURLE_OK) {
                    DebugConsole::print(std::string("Error Pulling from api Res: ") + std::to_string((int)res) + "\n", DBGC_YELLOW);
                }
            }
            curl_easy_cleanup(handler);

            return output;
        }
        void getteamdata(int page) {
            std::string pageData = makeTBAReq(std::string("teams/") + std::to_string(page));

            JsonParser parser(pageData); 
            std::vector<TEAM_DATAPOINT> teams = parser.parseTeams();

            database->setteamdat(teams);
            database->addTeams();        
        }
        void getmatchdata(std::string eventkey) {
            std::string pageData = makeTBAReq(std::string("event/") + eventkey + std::string("/matches"));
            std::ofstream compTeamsFile("resources/csv/matchesList.csv");
            compTeamsFile << pageData;

            JsonParser parser(pageData); 
            std::vector<MATCHLIST_DATAPOINT> matchs = parser.parseMatchList();

            database->setmatchlistdat(matchs);
            database->addMatchList();        
        }
        void getteamsatcomphdata(std::string eventkey) {
            JsonParser teamsParser(makeTBAReq(std::string("event/") + eventkey + std::string("/teams")));
                    std::vector<TEAM_DATAPOINT> teamsList = teamsParser.parseTeams();
                    if (teamsList.size() >= 1) {
                        std::ofstream compTeamsFile("resources/csv/teamCompList.csv");
                        compTeamsFile << std::to_string(teamsList[0].teamNum);
                        for (int i = 1; i < teamsList.size(); i++) {
                            compTeamsFile << "," << std::to_string(teamsList[i].teamNum);
                        }
                    }
                    else {
                        toastHandler::add(Toast("Invalid Comp ID", LENGTH_NORMAL));
                    }       
        }
        void deleteteams() {
            database->clearTeams();
        }

        ~RestReqHandler() {
            delete database;
        }
};

#endif