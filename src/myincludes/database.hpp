#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../include/mysql/mysql.h"
#include "toastHandler.hpp"
#include "debugConsole.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <format>

class Database {
    private:
        MYSQL mysql; 
        MYSQL_ROW row;
    public:
   
        Database() { 
            if(mysql_init(&mysql) == NULL) { 
                DebugConsole::print("ERROR: Database failed to initialize. Check that you have it installed?\n", DBGC_RED);
                mysql_close(&mysql);
                toastHandler::add(Toast("fuck you", LENGTH_NORMAL));
            } 

            if (!mysql_real_connect(&mysql, "localhost", "root", "1138", "1138scapp", 0, NULL, 0)) {
                DebugConsole::print(std::string("Error connecting to Server: ") + mysql_error(&mysql) + "\n", DBGC_RED);
                mysql_close(&mysql);
                toastHandler::add(Toast("fuck you", LENGTH_NORMAL));
            }; 

            DebugConsole::print("DB Connected!\n", DBGC_GREEN);
            printf("Client version: %s\n",mysql_get_client_info()); 
            printf("Server version: %s\n",mysql_get_server_info(&mysql));                          
        }
        MYSQL* getMysql() {
            return &this->mysql;
        }
        // std::vector<std::vector<std::string>> query(int len, std::string statement, int length) {

        // }
        std::vector<std::vector<std::string>> execQuery(std::string statement) {

            std::string row2; 
            if (mysql_query(&mysql, statement.c_str())) {
                DebugConsole::print(std::string("Error executing DB query: ") + mysql_error(&mysql) + "\n", DBGC_YELLOW);


                return std::vector<std::vector<std::string>>();
            }
      

 
            MYSQL_RES *resptr = mysql_use_result(&mysql);
            if (mysql.status == MYSQL_STATUS_USE_RESULT) {
                DebugConsole::print(std::string("Error: Status Use Result: ") + mysql_error(&mysql) + " (Can usually safely be ignored)\n", DBGC_YELLOW);
            }    
            if (mysql_field_count(&mysql) == 0) {
                return std::vector<std::vector<std::string>>();
            }
            else {
                if (resptr == 0) {
                    DebugConsole::print(std::string("Warn: result is zero: ") + mysql_error(&mysql) + "\n", DBGC_YELLOW);
                }
                else {
                    int i, j;
                    i = 0;

                    std::vector<std::vector<std::string>> vector;

                    
                    while ((row = mysql_fetch_row(resptr)) != NULL) {
                        vector.push_back(std::vector<std::string>());              
                
                            for (int j=0; j < mysql_field_count(&mysql); j++) {
                                //    std::cout << std::string(row[j]) << std::endl;
                                //    std::cout << "jjj" << std::endl;
                                if (row[j] != NULL) {
                                    vector.at(i).push_back(std::string(row[j])); 
                                }
                                //    std::cout << "please god" << std::endl;
                            }
                        i++;
                    }
                    
                    mysql_free_result(resptr);

                    return vector;
                }
            }
            return std::vector<std::vector<std::string>>();
        }
            
        
        ~Database() {
            mysql_close(&mysql); 
            
        }
};
#endif
