#ifndef DATABASEMAN_HPP
#define DATABASEMAN_HPP

#include "../include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>
#include "toastHandler.hpp"
#include "database.hpp"
#include "datapointStruct.hpp"
#include <iostream>
#include <format>


class DatabaseMan {
    private:
        MYSQL mysql; 
        MYSQL_ROW row;
        Database database;
        std::vector<MATCH_DATAPOINT> datapoints;
        MATCH_DATAPOINT temp;
        std::string resultstr;
        
        
    public:
   
    DatabaseMan(std::vector<MATCH_DATAPOINT> datapointsn) { 
        this->datapoints = datapointsn;
     

    }

  


    void maketh() {
        for (auto i = datapoints.begin(); i != datapoints.end(); ++i) {
            if (i.base() != NULL) {
                temp = *i.base();
                std::cout << "working" <<std::endl;
                
                // std::cout << "insert into matchtransaction ( MatchId, ScouterID, DatapointID,  DCValue, TeamID, AllianceID, DCTimestamp) values (" + temp.matchID + "," + temp.scouterID + "," + temp.datapointID + ",'" + temp.datapointValue + "'," +  temp.teamID + ",'" + temp.AllianceId + "'," + temp.DCTimestamp + ");" << std::endl;
                auto notUsed = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DatapointID,  DCValue, TeamID, AllianceID, DCTimestamp) values (" + temp.matchID + "," + temp.scouterID + "," + temp.datapointID + ",'" + temp.datapointValue + "'," +  temp.teamID + ",'" + temp.AllianceId + "'," + temp.DCTimestamp + ");", 1); 
                
            }
            else {
                std::cerr << " it doesn't work, it is null" << std::endl;
              }   
        }
         
  

    }

    ~DatabaseMan() {
            
        }
};
#endif