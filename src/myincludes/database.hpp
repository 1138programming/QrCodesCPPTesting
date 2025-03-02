#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../include/mysql/mysql.h"
#include "toastHandler.hpp"
#include "debugConsole.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
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

        std::vector<std::vector<std::string>> query(const char* queryStatement, ...) {
            // query escaping + setup
            va_list queryArgs;
            va_start(queryArgs, queryStatement);

            std::stringstream fullQuery;
            while ((*queryStatement) != '\0') {
                if ((*queryStatement) == '?') {
                    char* currQuery = va_arg(queryArgs, char*);
                    if (currQuery != NULL) {
                        int currQueryLen = strlen(currQuery);
                        char* currQueryEscaped = (char*)malloc((currQueryLen*2)+1); // len reccomended by mysql docs
                        mysql_real_escape_string(&this->mysql, currQueryEscaped, currQuery, currQueryLen);
                        fullQuery << currQueryEscaped;
                        free(currQueryEscaped);
                    }
                }
                else {
                    fullQuery << (*queryStatement);
                }
                queryStatement++;
            }
            // actually query the db
            if (mysql_query(&this->mysql, fullQuery.str().c_str())) {
                // error case
                DebugConsole::print(std::string("Error executing DB query: ") + mysql_error(&mysql) + "\n", DBGC_YELLOW);
                return std::vector<std::vector<std::string>>();
            }

            MYSQL_RES* result = mysql_use_result(&mysql);
            int resFieldLen = mysql_field_count(&this->mysql);
            if (result == NULL) {
                if (resFieldLen >= 1) {
                    DebugConsole::println(std::string("Result error: ") + mysql_error(&this->mysql) + std::string(" (result not returned)"), DBGC_YELLOW);
                }
                mysql_free_result(result);
                return std::vector<std::vector<std::string>>();
            }

            std::vector<std::vector<std::string>> resultVector;
            for (int currentRow = 0; (row = mysql_fetch_row(result)) != NULL; currentRow++) {
                resultVector.push_back(std::vector<std::string>()); // push empty vector

                for (int i = 0; i < mysql_num_fields(result); i++) {
                    if (row[i] == NULL) {
                        resultVector.at(currentRow).push_back(std::string("NULL"));
                    }
                    else {
                        resultVector.at(currentRow).push_back(std::string(row[i]));
                    }
                }
            }
            mysql_free_result(result);
            return resultVector;
        }

        std::vector<std::vector<std::string>> execQuery(std::string statement, int length) {
            std::string row2;        
            if (mysql_query(&mysql, statement.c_str())) {
                DebugConsole::print(std::string("Error executing DB query: ") + mysql_error(&mysql) + "\n", DBGC_YELLOW);

                return std::vector<std::vector<std::string>>();
            }
 
            MYSQL_RES *resptr = mysql_use_result(&mysql);
            if (mysql.status == MYSQL_STATUS_USE_RESULT) {
                DebugConsole::print(std::string("Error: Status Use Result: ") + mysql_error(&mysql) + " (Can usually safely be ignored)\n", DBGC_YELLOW);
            }    
            if (length == 0) {
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
                
                            for (j = 0; j < length; j++) {
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
