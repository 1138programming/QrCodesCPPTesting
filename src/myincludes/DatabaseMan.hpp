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
            temp = *i.base();
            
            resultstr = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DataPointID,  DCValue, TeamID, AllianceID) values (" + temp.matchID + "," + temp.scouterID + "," + temp.datapointID + "," + temp.datapointValue + "," +  temp.teamID + ", blue);", 2); 
            std::cout << resultstr <<std::endl;
        }
         
  

    }

    ~DatabaseMan() {
            
        }
};
#endif