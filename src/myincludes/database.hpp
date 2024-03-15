#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>
#include "toastHandler.hpp"
#include <iostream>
#include <format>

class Database {
    private:
        MYSQL mysql; 
        MYSQL_ROW row;
    public:
   
        Database() { 
            

            if(mysql_init(&mysql)==NULL) { 
                printf("\nInitialization error\n"); 
                mysql_close(&mysql);
                toastHandler::add(Toast("fuck you",LENGTH_NORMAL));
         
            } 
         

            if (!mysql_real_connect(&mysql,"localhost","root","1138","1138scapp",0,NULL,0)) {
                fprintf(stderr, "Error connecting to Server: %s\n", mysql_error(&mysql));
                mysql_close(&mysql);
                toastHandler::add(Toast("fuck you",LENGTH_NORMAL));
        
            }; 

            printf("Client version: %s",mysql_get_client_info()); 
            printf("\nServer version: %s",mysql_get_server_info(&mysql)); 


      
                                        
        }

        std::vector<std::vector<std::string>> execQuery(std::string statement, int length) {
            std::string row2;        
            if (mysql_query(&mysql, statement.c_str())) {
                std::cout << "Error executing Query" << mysql_error(&mysql) << std::endl;


                return std::vector<std::vector<std::string>>();
            }
 
            MYSQL_RES *resptr = mysql_use_result(&mysql);
            if (mysql.status == MYSQL_STATUS_USE_RESULT) {
                std::cout << "Error bad" << mysql_error(&mysql) << std::endl;
            }

            if (resptr == 0) {
                std::cout << "Error result is zero" << mysql_error(&mysql) << std::endl;
                mysql_free_result(resptr);
            }
            else {
                int i, j;
            
            i = 0;
            std::vector<std::vector<std::string>> vector;
            

            while ((row = mysql_fetch_row(resptr)) != NULL) {  std::cout << "hhh" << std::endl;
                vector.push_back(std::vector<std::string>());              
  
              
                    for (int j=0; j < length; j++) {
                    //    std::cout << std::string(row[j]) << std::endl;
                    //    std::cout << "jjj" << std::endl;
                       if (row[j] != NULL) {
                       vector.at(i).push_back(std::string(row[j])); 
                    }
                    //    std::cout << "please god" << std::endl;
                    }
                    
                    
       
                i++;
            }
            return vector;
            }
        }
            
        

        ~Database() {
            mysql_close(&mysql); 
        }
};
#endif
