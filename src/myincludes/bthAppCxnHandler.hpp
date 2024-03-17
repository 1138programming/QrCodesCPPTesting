#ifndef BTH_APPCXN_HANDLER_HPP
#define BTH_APPCXN_HANDLER_HPP

#include "btIncludes.hpp"
#include "bluetoothTransactionType.hpp"
#include "toastHandler.hpp"
#include "jsonParser.hpp"
#include "DatabaseMan.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>

#define EXPECTED_DATA_INITIAL sizeof(char)
#define EXPECTED_DATA_READSIZE sizeof(int)

class BthCxnHandler {
    private:
        bt::SOCKET socket;
        bool errorCheck;

        // useful functions
        char* readAllExpectedDataFromSocket(size_t dataSizeExpected, bool* success) {
            (*success) = true;
            // return nothing if no memory given (obv.)
            if (dataSizeExpected < 1) {
                (*success) = false;
                return nullptr;
            }
            // create data ptr and check if malloc succeded.
            char* dataPtr = (char*)malloc(dataSizeExpected);
            if (dataPtr == NULL) {
                (*success) = false;
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
                    toastHandler::add(Toast("Socket Requested Close: " + std::to_string(bt::WSAGetLastError()), LENGTH_NORMAL));
                    (*dataPtr) = BT_CLOSE_SOCKET;
                    return dataPtr;
                }
                // if SOCKET_ERROR is returned, there was an error (obv.)
                if (currentLengthRecvd == SOCKET_ERROR) {
                    toastHandler::add(Toast("SOCKET ERROR: " + std::to_string(bt::WSAGetLastError()), LENGTH_NORMAL));
                    (*dataPtr) = BT_SOCKET_ERROR;
                    (*success) = false;
                    return dataPtr;
                } 
                dataRecvd += currentLengthRecvd;
                usePtr += currentLengthRecvd; // do this so the buffer is increased
            }
            return dataPtr;
        }
        bool sendAllDataToSocket(char* data, size_t expectedLength) {
            char* currentData = data;
            size_t sentData = 0;
            while(sentData < expectedLength) {
                size_t lenDataSent = bt::send(this->socket, currentData, expectedLength-sentData, 0);
                if (lenDataSent == SOCKET_ERROR) {
                    return false;
                }
                sentData += lenDataSent;
                currentData += lenDataSent;
            }
            return true;
        }
        void allowSocketBlocking() {
            bt::ULONG mode = 0;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->socket, FIONBIO, &mode), 0, "Failed to set sock blocking mode.");
        }
        void disallowSocketBlocking() {
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->socket, FIONBIO, &mode), 0, "Failed to set sock non-blocking mode.");
        }
        bool sendAck() {
            // thumbs up emoji lol (4 bytes)
            size_t ackSize = 4*sizeof(char);
            char* dataToSend = (char*)calloc(1, ackSize);
            if(dataToSend == NULL) {
                toastHandler::add(Toast("Confirmation Send Failed", LENGTH_NORMAL));
                return false;
            }
            // thumbs up emoji
                dataToSend[0] = 0xf0;
                dataToSend[1] = 0x9f;
                dataToSend[2] = 0x91;
                dataToSend[3] = 0x8d;
            // send thumbs up emoji and return success val
            bool success = sendAllDataToSocket(dataToSend, ackSize);
            free(dataToSend);
            return success;
        }
        bool sendNack() {
            size_t nackSize = 4*sizeof(char);
            char* dataToSend = (char*)calloc(1, nackSize);
            // make sure malloc succeded
            if(dataToSend == NULL) {
                toastHandler::add(Toast("Nack Send Failed", LENGTH_NORMAL));
                return false;
            }
            // thumbs down emoji
                dataToSend[0] = 0xf0;
                dataToSend[1] = 0x9f;
                dataToSend[2] = 0x91;
                dataToSend[3] = 0x8e;
            bool success = sendAllDataToSocket(dataToSend, nackSize);
            return success;
        }
        bool invertEndianness(char* ptr, size_t ptrSize) {
            // copy pointer, returning false if failing.
            char* ptrCopy = (char*)malloc(ptrSize);
            if(ptrCopy == NULL) {
                return false;
            }
            ptrCopy = (char*)memcpy(ptrCopy, ptr, ptrSize);
            // set all the bits of ptr to the inverse of ptrCopy
            for(int i = 0; i < ptrSize; i++) {
                // -1 because index starts @ 0
                ptr[i] = ptrCopy[ptrSize-i-1];
            }
            free(ptrCopy);
            return true;
        }
        void endInteraction() {
            disallowSocketBlocking();
            sendNack();
        }

    public:
        BthCxnHandler(bt::SOCKET socket) {
            this->socket = socket;
        }
        // should only be called once
        BT_TRANSACTIONTYPE getTransactionType() {
            bool success;
            char* transactionPtr = readAllExpectedDataFromSocket(EXPECTED_DATA_INITIAL, &success);
            char transactionType = (*transactionPtr);
            free(transactionPtr);

            // if it's a SOCKET_ERROR, check to make sure it's not fatal. If it is, change it to SOCKET_CLOSE, 
            // as there is no point in reading from a dead socket.
            if (transactionType == -128) {
                if (bt::WSAGetLastError() == 10053) { // WSAECONNABORTED
                    transactionType = BT_CLOSE_SOCKET;
                    success = true;
                }
            }
            if (!success) {
                return BT_SOCKET_ERROR;
            }
            return static_cast<BT_TRANSACTIONTYPE>(transactionType);
        }

        std::string readMatchFromTablet(bool* readSuccess) {
            /*
                C: 👍 (ACK)
                T: # of bytes will be sent
                C: 👍 (ACK)
                T: send all data (blocking)
                C: 👍 (ACK)
            */
            (*readSuccess) = false;
            // checking if getting data is successful.
            bool dataGetSuccess = true;
            // allow socket to block while we are doing this transaction (it shouldn't take long)
            allowSocketBlocking();
                // ___ ack ___
                if (!sendAck()) {
                    disallowSocketBlocking();
                    return std::string();
                }
                // ___ get data size ___
                char* dataSizePtr = readAllExpectedDataFromSocket(EXPECTED_DATA_READSIZE, &dataGetSuccess);
                // if we are not successful in reading (or inverting the endian-ness, as it is currently incorrect), send a NACK and return
                if (!dataGetSuccess || !invertEndianness(dataSizePtr, EXPECTED_DATA_READSIZE)) {
                    endInteraction();
                    return std::string();
                }
                int dataSize = ((int*)dataSizePtr)[0];
                // free the memory we created
                free(dataSizePtr);
                std::cout << std::to_string(dataSize) << std::endl;
                // ___ ack dataSize recvd ___
                if (!sendAck()) {
                    disallowSocketBlocking();
                    return std::string();
                }
                // ___ read all expected data ___
                char* jsonData = readAllExpectedDataFromSocket(dataSize, &dataGetSuccess);
                // if we are not successful in reading, send a NACK and return
                if (!dataGetSuccess) {
                    endInteraction();
                    return std::string();
                }
                // get data as string before freeing ptr
                std::string dataInStringFormat = std::string(jsonData, dataSize);

                // free ptr
                free(jsonData);
            disallowSocketBlocking();
            (*readSuccess) = true;
            return dataInStringFormat;
        }
        void handleSocketError() {
            sendNack();
            disallowSocketBlocking();
        }
        void closeSocket() {
            checkSuccessWinsock<int>(bt::closesocket(this->socket), 0, "failed to propely close socket (memory leak)");
        }
};

#endif