#ifndef QRCODE_SCANNER_HPP
#define QRCODE_SCANNER_HPP

#include "../include/raylib-cpp.hpp"
#include "../include/json.hpp"
#include "jsonParser.hpp"
#include "toastHandler.hpp"
#include <iostream>
#include <fstream>
#include <string>

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
            JsonParser parser(std::string(this->text));
            parser.parse();
            this->counter = 0;
            this->text[0] = '\0';
        }

        // no memory leak 🥳🎊🎈🎉💃
        ~QrCodeScanner() {
            free(this->text);
        }
};

#endif