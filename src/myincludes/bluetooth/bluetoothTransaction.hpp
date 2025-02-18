#ifndef BLUETOOTHTRANSACTION_HPP
#define BLUETOOTHTRANSACTION_HPP

#include "btTabObj.hpp"

#include <vector>
#include <future>
#include <chrono>

namespace bt {
    typedef struct {
        BtTabObj* parent;
        bt::TRANSACTIONTYPE transactionType;
        std::future<std::optional<std::vector<char>>> data;
        bool success;
        bool writeTransaction; // if not, it's a read transaction. (This could be inferred from TRANSACTIONTYPE, but this is a nice way to package it)
        bool isReady() {
            data.val
            switch(data.wait_for())
        }
    } TABTRANSACTION;
}

#endif