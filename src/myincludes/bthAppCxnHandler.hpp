#ifndef BTH_APPCXN_HANDLER_HPP
#define BTH_APPCXN_HANDLER_HPP

#include "btIncludes.hpp"
#include "bluetoothTransactionType.hpp"
#include "toastHandler.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>

#define EXPECTED_DATA_INITIAL sizeof(char)

class BthCxnHandler {
    private:
        bt::SOCKET socket;
        bool error;

        // useful functions
        char* readAllExpectedDataFromSocket(size_t dataSizeExpected) {
            this->error = false;
            // return nothing if no memory given (obv.)
            if (dataSizeExpected < 1) {
                return nullptr;
            }
            // create data ptr and check if malloc succeded.
            char* dataPtr = (char*)malloc(dataSizeExpected);
            if (dataPtr == NULL) {
                ToastHandler::add(Toast("SOCKET malloc() failed", LENGTH_NORMAL));
                return dataPtr;
            }
            // create pointer to be used for getting data (as we will need to modify the ptr)
            char* usePtr = dataPtr;

            // receive data (it may not get all the data at one time, hence the while() loop)
            size_t dataRecvd = 0;
            while(dataRecvd < dataSizeExpected) {
                size_t currentLengthRecvd = bt::recv(this->socket, usePtr, dataSizeExpected-dataRecvd, 0);
                // if it returns 0, the socket has requested being closed
                if (currentLengthRecvd == 0) {
                    ToastHandler::add(Toast("Socket Requested Close: " + std::to_string(bt::WSAGetLastError()), LENGTH_NORMAL));
                    (*dataPtr) = BT_CLOSE_SOCKET;
                    return dataPtr;
                }
                // if SOCKET_ERROR is returned, there was an error (obv.)
                if (currentLengthRecvd == SOCKET_ERROR) {
                    ToastHandler::add(Toast("SOCKET ERROR: " + std::to_string(bt::WSAGetLastError()), LENGTH_NORMAL));
                    (*dataPtr) = BT_SOCKET_ERROR;
                    return dataPtr;
                } 
                dataRecvd += currentLengthRecvd;
                usePtr += currentLengthRecvd; // do this so the buffer is increased
            }
            return dataPtr;
        }
        void sendAllDataToSocket(char* data, size_t length) {
            
        }
        void allowSocketBlocking() {
            bt::ULONG mode = 0;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->socket, FIONBIO, &mode), 0, "Failed to set sock blocking mode.");
        }
        void disallowSocketBlocking() {
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->socket, FIONBIO, &mode), 0, "Failed to set sock non-blocking mode.");
        }
        void sendConfirmation() {
            // thumbs up emoji lol (4 bytes)
            char* dataToSend = (char*)calloc(4, sizeof(char));
            if(dataToSend == NULL) {
                toastHandler::add(Toast("Confirmation Send Failed"), LENGTH_NORMAL);
            }
            // thumbs up emoji
            dataToSend[0] = 0xf0;
            dataToSend[1] = 0x9f;
            dataToSend[2] = 0x91;
            dataToSend[3] = 0x8d;
            bt::send(this->socket, dataToSend, 4*sizeof(char), 0);
            free(dataToSend);
        }

    public:
        BthCxnHandler(bt::SOCKET socket) {
            this->socket = socket;
        }
        // should only be called once
        BT_TRANSACTIONTYPE getTransactionType() {
            char* transactionPtr = readAllExpectedDataFromSocket(EXPECTED_DATA_INITIAL);
            char transactionType = (*transactionPtr);
            free(transactionPtr);
            return static_cast<BT_TRANSACTIONTYPE>(transactionType);
        }

        void readMatchFromTablet() {
            // allow socket to block while we are doing this transaction (it shouldn't take long)
            allowSocketBlocking();



            disallowSocketBlocking();
        }
};

#endif