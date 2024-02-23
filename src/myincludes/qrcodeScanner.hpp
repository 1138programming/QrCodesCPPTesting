#ifndef QRCODE_SCANNER_HPP
#define QRCODE_SCANNER_HPP

#include "../include/raylib-cpp.hpp"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>

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
                if (counter < (MAX_CHARS-1)) {
                    std::cout << key << std::endl;
                    text[counter] = key;
                    text[counter+1] = '\0';
                    counter++;
                }                   
                key = GetCharPressed();
            }
        
           
        }

        void update () {
            std::string fileName;
            std::cout << "helkejkekkedk" << std::endl;
            json data = json::parse(this->text);
            if (data.contains("scouterName")) {
                fileName = data.find("scouterName").value();
                fileName += ".json";
            }
            else {
                fileName = "gaming.json";
            }

            std::ofstream file(fileName);
            int i = 0;
            while(text[i] != '\0') {
                file << text[i];
                text[i] = '\0';
                i++;
            }
            counter = 0;
            file.close(); 
        }

        // no memory leak 🥳🎊🎈🎉💃
        ~QrCodeScanner() {
            free(this->text);
        }
};

#endif