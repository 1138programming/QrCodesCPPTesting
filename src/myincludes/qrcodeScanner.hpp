#ifndef QRCODE_SCANNER_HPP
#define QRCODE_SCANNER_HPP

#include "../include/raylib-cpp.hpp"
#include "../include/json.hpp"
#include "DatabaseMan.hpp"
#include "jsonParser.hpp"
#include "toastHandler.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using json = nlohmann::json;

#define MAX_CHARS 8192

class QrCodeScanner {
    private:
        char* text;
        char key;
        int counter;
    
        
    public:
        QrCodeScanner() {
            text = (char*)malloc(sizeof(char) * MAX_CHARS);
            counter = 0;
        }


        void scan () {
           key = GetCharPressed();
           while (key != 0) {
                if ((counter % 100) == 0) {
                    std::cout << this->counter << std::endl;
                }
                if (counter < (MAX_CHARS-1)) {
                    text[counter] = key;
                    text[counter+1] = '\0';
                    counter++;
                }                   
                key = GetCharPressed();
            }
        }

        void update() {
            std::ifstream i("log copy 2.json");
         
            std::ostringstream sstr;
            sstr << i.rdbuf();
           
            // JsonParser parser(text); 
            JsonParser parser(sstr.str()); 
             
            std::vector<MATCH_DATAPOINT> vect = parser.parseMatch();      
            std::cout << "hello5" <<std::endl;

            DatabaseMan database = * new DatabaseMan();
            database.setmatchdata(vect);
            database.addMatchDatapoints();
            this->counter = 0;
            this->text[0] = '\0';
        }

        // no memory leak 🥳🎊🎈🎉💃
        ~QrCodeScanner() {
            free(this->text);
        }
};

#endif