#ifndef DEBUGCONSOLE_HPP
#define DEBUGCONSOLE_HPP

#include "debugConsoleColors.hpp"
#include "btIncludes.hpp" // just use this header for simplicity sake

#include <iostream>

class DebugConsole {
    public:
        static void print(std::string message) {
            print(message, DBGC_DEFAULT);
        }

        static void print(std::string message, DEBUGCOLORS color) {
            bt::HANDLE stdOut;
            stdOut = bt::GetStdHandle(STD_OUTPUT_HANDLE);

            bt::DWORD request = ENABLE_VIRTUAL_TERMINAL_PROCESSING; // we want to check if color is supported
            if (!bt::GetConsoleMode(stdOut, &request)) {
                std::cout << message;
            }
            else {
                switch(color) {
                    case DBGC_RED:
                    {
                        std::cout << "\033[31m";
                    }
                    break;

                    case DBGC_BLUE:
                    {
                        std::cout << "\033[36m";
                    }
                    break;

                    case DBGC_GREEN:
                    {
                        std::cout << "\033[32m";
                    }
                    break;

                    case DBGC_YELLOW:
                    {
                        std::cout << "\033[33m";
                    }
                    break;

                    case DBGC_DEFAULT:
                    {
                        std::cout << "\033[0m";
                    }
                    break;

                    default:
                        std::cout << "\033[0m";
                };
                std::cout << message << "\033[0m";
            }
        }

        static void print(std::type_info& callerClass, std::string message){ 
            std::string finalMessage = callerClass.name() + ": " + message;
            print(finalMessage);
        }

        static void print(std::type_info& callerClass, std::string message, DEBUGCOLORS color) {
            std::string finalMessage = callerClass.name() + ": " + message;
            print(finalMessage, color);
        }

};

#endif