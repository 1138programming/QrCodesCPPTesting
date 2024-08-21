#ifndef DEBUGCONSOLE_HPP
#define DEBUGCONSOLE_HPP

#include "debugConsoleColors.hpp"
#include "btIncludes.hpp" // just use this header for simplicity sake

#include <iostream>

class DebugConsole {
    public:
        static void print(std::string message, DEBUGCOLORS color) {
            bt::HANDLE stdOut;
            stdOut = bt::GetStdHandle(STD_OUTPUT_HANDLE);

            bt::DWORD request = ENABLE_VIRTUAL_TERMINAL_PROCESSING; // we want to check if color is supported
            if (!bt::GetConsoleMode(stdOut, &request)) {
                std::cout << message;
            }
            else {
                switch(color) {
                    case DBG_RED:
                    {
                        std::cout << "\033[31m";
                    }
                    break;

                    case DBG_BLUE:
                    {
                        std::cout << "\033[36m";
                    }
                    break;

                    case DBG_GREEN:
                    {
                        std::cout << "\033[32m";
                    }
                    break;

                    case DBG_YELLOW:
                    {
                        std::cout << "\033[33m";
                    }
                    break;

                    case DBG_DEFAULT_COLOR:
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
};

#endif