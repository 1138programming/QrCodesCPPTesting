#ifndef DEBUGCONSOLE_HPP
#define DEBUGCONSOLE_HPP

#include "debugConsoleColors.hpp"
#include "debugLevel.hpp"
#include "btIncludes.hpp" // just use this header for simplicity sake
#include "libusb.h"

#include <iostream>
#include <sstream>

class DebugConsole {
    public:
        static void libUSBCallback(libusb_context *ctx, enum libusb_log_level level, const char *str) {
            switch(level) {
                case LIBUSB_LOG_LEVEL_NONE: {  
                    break; 
                }

                case LIBUSB_LOG_LEVEL_ERROR: {
                    DebugConsole::print(std::string(str), DBGC_RED, DBGL_ERROR);
                }
                break;
                
                case LIBUSB_LOG_LEVEL_WARNING: {
                    DebugConsole::print(std::string(str), DBGC_YELLOW, DBGL_WARNING);
                }
                break;

                case LIBUSB_LOG_LEVEL_INFO: {
                    DebugConsole::print(std::string(str), DBGC_BLUE, DBGL_INFO);
                }

                case LIBUSB_LOG_LEVEL_DEBUG: {
                    DebugConsole::print(std::string(str), DBGC_BLUE, DBGL_DEVEL);
                }
            }
        }

        static bool debugLevelAllowed(DEBUGGINGLEVEL debugLevel) {
            return (((int)debugLevel) >= ((int)DEBUG_LEVEL));
        }

        static void print(std::string message) {
            print(message, DBGC_DEFAULT, DBGL_ESSENTIAL);
        }
        static void print(std::string message, DEBUGGINGLEVEL debugLevel) {
            print(message, DBGC_DEFAULT, debugLevel);
        }
        static void print(std::string message, DEBUGCOLORS color) {
            print(message, color, DBGL_ESSENTIAL);
        }

        static void println(std::string message) {
            print(message + std::string("\n"), DBGC_DEFAULT, DBGL_ESSENTIAL);
        }
        static void println(std::string message, DEBUGGINGLEVEL debugLevel) {
            print(message + std::string("\n"), DBGC_DEFAULT, debugLevel);
        }        
        static void println(std::string message, DEBUGCOLORS color) {
            print(message + std::string("\n"), color, DBGL_ESSENTIAL);
        }
        static void println(std::string message, DEBUGCOLORS color, DEBUGGINGLEVEL debugLevel) {
            print(message + std::string("\n"), color, debugLevel);
        }

        static void print(std::string message, DEBUGCOLORS color, DEBUGGINGLEVEL debugLevel) {
            if (((int)debugLevel) < ((int)DEBUG_LEVEL)) {
                return;
            }
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