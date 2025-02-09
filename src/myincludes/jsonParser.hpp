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
               
                    std::cout << "hello3" <<std::endl;
                
         
                    
                    std::cout << *it << std::endl;
                    json element = *it;
                    currentDatapoint.scouterID = element["scouterID"];
                    currentDatapoint.datapointID = element["datapointID"];
                    currentDatapoint.teamID = element["teamID"];
                    currentDatapoint.matchID = element["matchID"];
                    currentDatapoint.DCValue = element["DCValue"];
                    std::string stuff = element["DCTimestamp"];
                    if (stuff== "noShow") {
                        currentDatapoint.DCTimestamp ="NULL";
                    }
                    else {
                        
                        currentDatapoint.DCTimestamp = ("'" + stuff + "'");


                    }
                    currentDatapoint.AllianceId = element["allianceID"];
              
              
           
                
                datapoints.push_back(currentDatapoint);  
                
                
                          

                
            }  std::cout << "hello4777777" <<std::endl; 
            }
            catch (...) {
                std::cout << "errbitch" <<std::endl; //uh oh
                // toastHandler::add(Toast("fuck. it no workie", LENGTH_NORMAL));

            }
           
            // std::cout << (datapoints.begin().base())->datapointID <<std::endl; 
           
                
            
             
            return datapoints;
        }
        std::vector<TEAM_DATAPOINT> parseAPI() {                
            std::vector<TEAM_DATAPOINT> Teamsdata = std::vector<TEAM_DATAPOINT>();  
            try {
              

                for (auto it = this->data.begin(); it != this->data.end() ; ++it) { 
                
                    TEAM_DATAPOINT currentDatapoint;
               
                    std::cout << "hello3" <<std::endl;
                
         
                    
                   
                    json element = *it; 
                    std::cout << *it << std::endl;
                    (!element["nickname"].is_null()) ?  currentDatapoint.teamName = element["nickname"] : currentDatapoint.teamName = "NULL";
                    (!element["team_number"].is_null()) ?  currentDatapoint.teamNum = element["team_number"] : currentDatapoint.teamNum = 0;
                    (!element["website"].is_null()) ?  currentDatapoint.teamDesc = element["website"] : currentDatapoint.teamDesc = "NULL";
                  
                    
              
           
                
                Teamsdata.push_back(currentDatapoint);  
                
                
                }
              std::cout << "hello4777777" <<std::endl; 
            }
            catch (...) {
                std::cout << "errbitch" <<std::endl; //uh oh
                // toastHandler::add(Toast("fuck. it no workie", LENGTH_NORMAL));

            }
           
            // std::cout << (datapoints.begin().base())->datapointID <<std::endl; 
           
                
            
             
            return Teamsdata;
        }
};

#endif