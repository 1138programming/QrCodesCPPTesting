#ifndef BTREADRES_HPP
#define BTREADRES_HPP

#include "btIncludes.hpp"
#include <vector>
#include <future>
#include <chrono>

namespace bt {
    typedef struct {
        bt::SOCKET parentSocket; // the socket that produced this result
        bt::TRANSACTIONTYPE transactionType;
        std::future<std::vector<char>> data;
        bool reportedSuccess;
        bool tabletWrite; // if the tablet is going to be the one doing the writing. If not, it's safe to assume the computer will be.
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