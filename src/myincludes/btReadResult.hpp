#ifndef BTREADRES_HPP
#define BTREADRES_HPP

#include <vector>
#include <future>
#include "btIncludes.hpp"

typedef struct {
    bt::TRANSACTIONTYPE transactionType;
    std::future<std::vector<char>> result;
} READRES;

#endif