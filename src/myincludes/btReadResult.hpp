#ifndef BTREADRES_HPP
#define BTREADRES_HPP

#include "btIncludes.hpp"
#include <vector>
#include <future>
#include <chrono>

namespace bt {
    typedef struct {
        bt::TRANSACTIONTYPE transactionType;
        std::future<std::vector<char>> data;
        bool reportedSuccess;
        bool isReady() {
            
            switch(data.wait_for(std::chrono::seconds(0))) {
                case std::future_status::deferred:
                    return true;
                case std::future_status::ready:
                    return true;
                case std::future_status::timeout:
                    return false;
            }
            return true; // best to wait rather than never get the result if there's an error.
        }
    } READRES;
}

#endif