#ifndef BTREADRES_HPP
#define BTREADRES_HPP

#include "btIncludes.hpp"
#include <vector>
#include <future>

namespace bt {
    typedef struct {
        bt::TRANSACTIONTYPE transactionType;
        std::future<std::vector<char>> data;
        bool reportedSuccess;
    } READRES;
}

#endif