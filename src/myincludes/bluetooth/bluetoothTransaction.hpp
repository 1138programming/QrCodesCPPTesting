#ifndef BLUETOOTHTRANSACTION_HPP
#define BLUETOOTHTRANSACTION_HPP


#include <vector>
#include <future>
#include <optional>
#include <chrono>

#include "btTabObj.hpp"

class BtTabObj; // we need to explicitly tell c++ it exists because it is bad stupid language 😡

namespace bt {
    typedef struct {
        BtTabObj* parent;
        bt::TRANSACTIONTYPE transactionType;
        std::future<std::optional<std::vector<char>>> data;
        bool batmanTrans = false; // true if the parent was killed in the middle of a transaction
        bool success = true;
        bool writeTransaction = false; // if not, it's a read transaction. (This could be inferred from TRANSACTIONTYPE, but this is a nice way to package it)
        /**
         * @returns Whether the future object is ready to be called/whether the data is ready. There are a couple of special cases:
         * 
         * - If the data is async, this will return false until done
         * 
         * - If the data is non-async, this will always return true
         * 
         * It's probably best practice this is not relied on to be true (a non-initialized data obj will cause UB)
         */
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
    } TABTRANSACTION;
}

#endif