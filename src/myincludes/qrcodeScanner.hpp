#ifndef QRCODE_SCANNER_HPP
#define QRCODE_SCANNER_HPP

#include "../include/raylib-cpp.hpp"
#include <iostream>
#include <fstream>


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

        // no memory leak 🥳🎊🎈🎉💃

        void scan () {
           key = GetCharPressed();
           while (key != 0) {
                std::cout << key << std::endl;
                if (counter < (MAX_CHARS-1)) {
                    text[counter] = key;
                    text[counter+1] = '\0';
                    counter++;
                }                   
                std::cout << key << std::endl;
                key = GetCharPressed();
            }
        
           
        }

        void update () {
           std::cout << "helkejkekkedk" << std::endl;
                    std::ofstream file("gaming.json");
                    int i = 0;
                    while(text[i] != '\0') {
                        file << text[i];
                        text[i] = '\0';
                        i++;
                    }
                    file << '\n';
                    counter = 0;
                    file.close(); 

        }

        ~QrCodeScanner() {
            free(this->text);
        }
};

#endif