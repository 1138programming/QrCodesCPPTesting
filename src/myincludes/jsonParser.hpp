#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "../include/json.hpp"
#include "toastHandler.hpp"
#include "datapointStruct.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class JsonParser {
    private:
        json data;
    public:
        JsonParser(std::string data) {
            try {
                this->data = json::parse(data);
                throw errno;
            }
            catch(...) {
                std::ofstream file;
                try {
                    file = std::ofstream("log.json");
                    file << data;
                    file.close();
                    toastHandler::add(Toast("Failed to parse JSON. Writing to file.", LENGTH_NORMAL));
                }
                catch(...) {
                    file.close();
                    toastHandler::add(Toast("Shit hit the fan. Nothing submitted.", LENGTH_NORMAL));
                }
            }
        }
        void parse() {
            for (json::iterator it = this->data.begin(); it != this->data.end(); ++it) {
                std::cout << it.value() << std::endl;
            }
        }
};

#endif