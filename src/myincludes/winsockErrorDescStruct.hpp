#ifndef WINSOCKERRORDESCSTRUCT_HPP
#define WINSOCKERRORDESCSTRUCT_HPP

#include <string>

typedef struct {
    std::string errorName;
    std::string errorNameDesc;
    std::string errorDesc;
} WINSOCKERRORDESCSTRUCT;

#endif