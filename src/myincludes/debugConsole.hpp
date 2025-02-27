#ifndef DEBUGCONSOLE_HPP
#define DEBUGCONSOLE_HPP

#include "debugConsoleColors.hpp"
#include "btIncludes.hpp" // just use this header for simplicity sake

#include <iostream>
#include <sstream>

class DebugConsole {
    public:
        static void print(std::string message) {
            print(message, DBGC_DEFAULT);
        }
        static void println(std::string message) {
            print(message + std::string("\n"), DBGC_DEFAULT);
        }

        static void print(std::string message, DEBUGCOLORS color) {
            bt::HANDLE stdOut;
            stdOut = bt::GetStdHandle(STD_OUTPUT_HANDLE);

            bt::DWORD mode = 0x0;
            bt::GetConsoleMode(stdOut, &mode); // gets current console mode

            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // we want to check if color is supported
            if (!bt::SetConsoleMode(stdOut, mode)) {
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
        static void println(std::string message, DEBUGCOLORS color) {
            print(message + std::string("\n"), color);
        }

        static void print(const std::type_info& callerClass, std::string message){ 
            std::stringstream finalMessage;
            finalMessage << std::string(callerClass.name()) << ": " << message;
            print(finalMessage.str());
        }

        static void print(const std::type_info& callerClass, std::string message, DEBUGCOLORS color) {
            std::stringstream finalMessage;
            finalMessage << std::string(callerClass.name()) << ": " << message;
            print(finalMessage.str(), color);
        }

};

#endif