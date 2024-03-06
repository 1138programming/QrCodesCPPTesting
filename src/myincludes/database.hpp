#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>
#include "toastHandler.hpp"

class Database {
    private:
        MYSQL mysql; 
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

        MYSQL_RES& execQuery(std::string statement) {
           
        
            if (mysql_query(&mysql, statement.c_str())) {
                std::cout << "Error executing Query" << mysql_error(&mysql) << std::endl;
                MYSQL_RES val = (MYSQL_RES){};
                return val;
            }
 
            MYSQL_RES *resptr = mysql_use_result(&mysql);
            if (mysql.status == MYSQL_STATUS_USE_RESULT) {
                 std::cout << "Error bad" << mysql_error(&mysql) << std::endl;
            }

            if (resptr == 0) {
                std::cout << "Error result is zero" << mysql_error(&mysql) << std::endl;
                MYSQL_RES val = (MYSQL_RES){};
                return val;
            }
            return *resptr;
            
        }

        ~Database() {
            mysql_close(&mysql); 
        }
};
#endif
