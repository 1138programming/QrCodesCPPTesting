#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "../include/json.hpp"
#include "toastHandler.hpp"
#include "datapointStruct.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using json = nlohmann::json;

class JsonParser {
    private:
        json data;
    public:
        JsonParser(std::string datan) {
            try {
                this->data = json::parse(datan);
                toastHandler::add(Toast("JSON parsed", LENGTH_NORMAL));
            }
            catch(...) {
               toastHandler::add(Toast("Failed to parse JSON", LENGTH_NORMAL));
            }
            std::ofstream file;
            try {
                file = std::ofstream("log copy 88889.json");
                file << datan;
                file.close();
            }
            catch(...) {
                file.close();
                toastHandler::add(Toast("Shit hit the fan. Nothing submitted.", LENGTH_NORMAL));
            }
           
        }
        std::vector<MATCH_DATAPOINT> parseMatch() {                
            std::vector<MATCH_DATAPOINT> datapoints = std::vector<MATCH_DATAPOINT>();  
            try {
                for (auto it = this->data["scoutingData"].begin(); it != this->data["scoutingData"].end() ; ++it) { 
                    MATCH_DATAPOINT currentDatapoint;
                
                    std::cout << *it << std::endl;
                    json element = *it;
                    currentDatapoint.scouterID = element["scouterID"];
                    currentDatapoint.datapointID = element["datapointID"];
                    currentDatapoint.teamID = element["teamID"];
                    currentDatapoint.matchID = element["matchID"];
                    currentDatapoint.DCValue = element["DCValue"];
                    std::string stuff = element["DCTimestamp"];
                    if (stuff == "noShow") {
                        currentDatapoint.DCTimestamp ="NULL";
                    }
                    else {
                        currentDatapoint.DCTimestamp = ("'" + stuff + "'");
                    }
                    currentDatapoint.AllianceId = element["allianceID"];    
                
                datapoints.push_back(currentDatapoint);
            }  
            // std::cout << "end" <<std::endl; 
            }
            catch (...) {
                DebugConsole::print(std::string("Error parsing match ") + "\n", DBGC_YELLOW);
            }
           
            return datapoints;
        }
        std::vector<TEAM_DATAPOINT> parseTeams() {                
            std::vector<TEAM_DATAPOINT> Teamsdata = std::vector<TEAM_DATAPOINT>();  
            try {
                for (auto it = this->data.begin(); it != this->data.end() ; ++it) { 
                
                    TEAM_DATAPOINT currentDatapoint; 
                    
                    json element = *it;
                    (!element["nickname"].is_null()) ?  currentDatapoint.teamName = element["nickname"] : currentDatapoint.teamName = "NULL";
                    (!element["team_number"].is_null()) ?  currentDatapoint.teamNum = element["team_number"] : currentDatapoint.teamNum = 0;
                    (!element["website"].is_null()) ?  currentDatapoint.teamDesc = element["website"] : currentDatapoint.teamDesc = "NULL";  

                    Teamsdata.push_back(currentDatapoint);  
                }
            }
            catch (...) {
                DebugConsole::print(std::string("Error parsing teams ") +  "\n", DBGC_YELLOW);
                // toastHandler::add(Toast("fuck. it no workie", LENGTH_NORMAL));
            }
            // std::cout << (datapoints.begin().base())->datapointID <<std::endl; 
             
            return Teamsdata;
        }
};

#endif