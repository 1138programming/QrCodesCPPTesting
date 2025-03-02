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
        std::vector<TEAM_DATAPOINT> teams;
        MATCH_DATAPOINT temp;
        TEAM_DATAPOINT temp2;

        std::string resultstr;
        
        
    public:
   
    DatabaseMan() {      

    }

    void setmatchdata(std::vector<MATCH_DATAPOINT> datapointsn) {
        this->datapoints = datapointsn;
    }

    void setteamdat(std::vector<TEAM_DATAPOINT> teamsn) {
        this->teams = teamsn;
    }


  


    void addMatchDatapoints() {
        if (!datapoints.empty()) {
        for (auto i = datapoints.begin(); i != datapoints.end(); ++i) {
            if (i.base() != NULL) {
                temp = *i.base();
               

                // std::cout << "insert into matchtransaction ( MatchId, ScouterID, DatapointID,  DCValue, TeamID, AllianceID, DCTimestamp) values (" + temp.matchID + "," + temp.scouterID + "," + temp.datapointID + ",'" + temp.datapointValue + "'," +  temp.teamID + ",'" + temp.AllianceId + "'," + temp.DCTimestamp + ");" << std::endl;
                auto notUsed = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DatapointID,  DCValue, TeamID, AllianceID, DCTimestamp) values (" + temp.matchID + "," + temp.scouterID + "," + temp.datapointID + ",'" + temp.DCValue + "'," +  temp.teamID + ",'" + temp.AllianceId + "'," + temp.DCTimestamp + ");", 1); 
                
            }
            else {
                DebugConsole::print(std::string("Error inserting datapoints. Base is null ")  + "\n", DBGC_RED);
            }   
        }
        }

        else {
            DebugConsole::print(std::string("Error inserting datapoints. Vector is null ")  + "\n", DBGC_RED);
        }


         
  

    }

    void clearTeams() {
        auto deleteRes = database.execQuery("delete from team",1);
    }
    void addTeams() {
        if (!teams.empty()) {
            for (auto i = teams.begin(); i != teams.end(); ++i) {
                if (i.base() != NULL) {
                    temp2 = *i.base();
                    auto insertRes = database.query("insert into team(TeamId, TeamNumber, TeamDesc) values(?, ?, ?)", temp2.teamNum, temp2.teamNum, temp2.teamName);
                }
                else {
                    DebugConsole::print(std::string("Error inserting teams. Base is null ") + "\n", DBGC_RED);
                }
            }
        }
        else {
            DebugConsole::print(std::string("Error inserting teams. Vector is null ") + "\n", DBGC_RED);
        }
    }


         
  

    


   
};
#endif