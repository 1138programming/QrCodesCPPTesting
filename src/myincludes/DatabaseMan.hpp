#ifndef DATABASEMAN_HPP
#define DATABASEMAN_HPP

#include "../include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>
#include "toastHandler.hpp"
#include "myincludes/database.hpp"
#include "myincludes/datapointStruct.hpp"
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
        std::cout << 

    }

  


    int maketh() {
        for (auto i = datapoints.begin(); i != datapoints.end(); ++i) {
            temp = *i.base();
            
            resultstr = database.execQuery(std::format("insert into matchtransaction ( MatchId, ScouterID, DataPointID,  DCValue, TeamID, AllianceID) values ({},{},{},{}, {}, {});", temp.matchID, temp.scouterID, temp.datapointID,temp.datapointValue,temp.teamID,"blue"),2);
            
        }
         
  

    }

    ~DatabaseMan() {
            
        }
};
#endif