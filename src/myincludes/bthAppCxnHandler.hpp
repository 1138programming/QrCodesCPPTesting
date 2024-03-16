#ifndef BTH_APPCXN_HANDLER_HPP
#define BTH_APPCXN_HANDLER_HPP

#include "btIncludes.hpp"
#include "bluetoothTransactionType.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>

#define EXPECTED_DATA_INITIAL sizeof(char)

class BthCxnHandler {
    private:
        bt::SOCKET socket;

        // useful functions
        char* readAllExpectedDataFromSocket(size_t dataSizeExpected) {
            char* dataPtr = (char*)malloc(dataSizeExpected);
            if (dataPtr == NULL) {
                std::cerr << "cannot read: malloc() failed" << std::endl;
                return dataPtr;
            }
            char* usePtr = dataPtr;

            size_t dataRecvd = 0;
            while(dataRecvd < dataSizeExpected) {
                size_t currentLengthRecvd = bt::recv(this->socket, usePtr, dataSizeExpected-dataRecvd, 0);
                if (currentLengthRecvd == 0) {
                    std::cerr << "nope: " << std::endl;
                    return dataPtr;
                }
                if (currentLengthRecvd != SOCKET_ERROR) {
                    dataRecvd += currentLengthRecvd;
                    usePtr += currentLengthRecvd; // do this so the buffer is increased
                } 
                else {
                    std::cerr << "error recieving data: " << bt::WSAGetLastError() << std::endl;
                    return dataPtr;
                }
            }
            return dataPtr;
        }

    public:
        BthCxnHandler(bt::SOCKET socket) {
            this->socket = socket;
        }
        BT_TRANSACTIONTYPE getTransactionType() {

        }
};

#endif