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
        JsonParser(std::string data) {
            try {
                this->data = json::parse(data);
                throw errno;
            }
            catch(...) {
                std::ofstream file;
                try {
                    file = std::ofstream("log.json");
                    file << data;
                    file.close();
                    toastHandler::add(Toast("Failed to parse JSON. Writing to file.", LENGTH_NORMAL));
                }
                catch(...) {
                    file.close();
                    toastHandler::add(Toast("Shit hit the fan. Nothing submitted.", LENGTH_NORMAL));
                }
            }
        }
        std::vector<MATCH_DATAPOINT>& parse() {                
            std::vector<MATCH_DATAPOINT> datapoints = std::vector<MATCH_DATAPOINT>();  
            try {
               std::cout << *(this->data["scoutingData"].begin()) <<std::endl;
                for (auto it = this->data["scoutingData"].begin(); it != this->data["scoutingData"].end()-1 ; ++it) { 
                
                 MATCH_DATAPOINT currentDatapoint;
                 std::cout << "hello3" <<std::endl;
                
                // if (it.value() != NULL) {
                //     std::cout << "null" <<std::endl;
                //  }
                //  std::cout << "hel" <<std::endl;
                // auto it2 = it.value().begin();  std::cout << "hello4" <<std::endl;
                    // for (auto it2 = it.value().cbegin(); it2 != it.value().cend()-1; ++it2) {
                    
                     std::cout << *it;
                //         if(it2.key() == "scouterID") {
                //             currentDatapoint.scouterID = it2.value();
                //         }
                //     else if(it2.key() == "matchID") {
                //             currentDatapoint.matchID = it2.value();
                //         }
                //     else if(it2.key() == "teamID") {
                //             currentDatapoint.teamID = it2.value();
                //         }
                //     else if(it2.key() == "datapointID") {
                //             int datapointIDVal = it2.value();
                //             currentDatapoint.datapointID = std::to_string(datapointIDVal);
                //         }
                //      else if(it2.key() == "datapointValue") {
                //             currentDatapoint.datapointValue = it2.value();
                //         }
                //     else if(it2.key() == "DCTimestamp") {
                //             currentDatapoint.DCTimestamp = it2.value();
                //         }
                // // }
                datapoints.push_back(currentDatapoint);   
               
                throw errno;
            } 
            }
            catch (...) {
                std::cout << "errbitch" <<std::endl;
                // toastHandler::add(Toast("fuck. it no workie", LENGTH_NORMAL));

            }
            return datapoints;
        }
};

#endif