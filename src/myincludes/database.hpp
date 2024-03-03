#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>

class Database {
    public:
        int innit() { 
            MYSQL mysql; 

            if(mysql_init(&mysql)==NULL) { 
                printf("\nInitialization error\n"); 
                return -1; 
            } 

            mysql_real_connect(&mysql,"localhost","root","1138","1138scapp",0,NULL,0); 

            printf("Client version: %s",mysql_get_client_info()); 
            printf("\nServer version: %s",mysql_get_server_info(&mysql)); 


            mysql_close(&mysql); 
            return 0; 
        }
};
#endif
