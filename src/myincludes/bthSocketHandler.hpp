#ifndef BTHSOCKETHANDLER
#define BTHSOCKETHANDLER

#include "btIncludes.hpp"
#include <future>

class bthSocketHandler {
    private:
        bt::SOCKET internalSocket;
        bool apoptosis; // fatal socket error?
        int errorCode; // error code if error
        bt::SOCKETCALLTYPE callType;

        bool errorIsFatal(int errorCode) {
            switch(errorCode) {
                case 10038:
                {
                    //WSAENOTSOCK
                    return true;
                }
                break;
                case 10052:
                {
                    //WSAENETRESET
                    return true;
                }
                case 10053:
                {
                    //WSAECONNABORTED
                    return true;
                }
                break;
                case 10058:
                {
                    //WSAESHUTDOWN
                    return true;
                }
                break;
                default:
                    return false;
            }
        }

        std::vector<char> returnEmptyVector() {
            return std::vector<char>();
        }
        std::future<std::vector<char>> returnEmptyFuture() {
            return std::async(std::launch::deferred, returnEmptyVector, this);
        }
    public:
        bthSocketHandler(bt::SOCKET socket) {
            this->internalSocket = socket;
            this->apoptosis = false;
            this->errorCode = -1;
            this->callType = bt::CALLTYPE_DEFAULT;
        }

        bool invertEndianness(char* ptr, size_t ptrSize) {
            // copy pointer, returning false if failing
            char* ptrCopy = (char*) malloc(ptrSize);
            if (ptrCopy == NULL) {
                return false;
            }
            // set all the bits of ptr to its inverse
            memcpy(ptrCopy, ptr, ptrSize);
            for (int i = 0; i < ptrSize; i++) {
                // -1 bc index starts @ 0
                ptr[i] = ptrCopy[ptrSize-i-1];
            }
            free(ptrCopy);
            return true;
        }


        char* readAllExpectedDataFromSocket(size_t dataSizeExpected, bool& success) {
            success = true;
            // return nothing if no memory given (obv.)
            if (dataSizeExpected < 1) {
                success = false;
                return nullptr;
            }
            // create data && check if succeded
            char* dataPtr = (char*)malloc(dataSizeExpected);
            if (dataPtr == NULL) {
                success = false;
                return nullptr;
            }

            char* dataPtrCopy = dataPtr; // will be used as return val (we modify the original)

            size_t dataRecvd = 0;
            while (dataRecvd < dataSizeExpected) {
                size_t currentLengthRecvd = bt::recv(this->internalSocket, dataPtr, dataSizeExpected-dataRecvd, 0);
                // graceful close 🥰
                if (currentLengthRecvd == 0) {
                    this->apoptosis = true;
                    success = false;
                    return nullptr;
                }
                // if SOCKET_ERROR is returned, there was an error (obv.)
                if (currentLengthRecvd == SOCKET_ERROR) {
                    int currError = bt::WSAGetLastError();
                    // catches most fatal errors
                    if (this->errorIsFatal(currError)) {
                        this->apoptosis = true;
                        this->errorCode = currError;
                    }
                    success = false;
                    return nullptr;
                }
                dataRecvd += currentLengthRecvd;
                dataPtr += currentLengthRecvd;
            }
            return dataPtrCopy; // other one points to end of data
        }

        bool sendAllDataToSocket(char* data, size_t dataLen) {
            char* currentData = data;
            size_t sentData = 0;
            while (sentData < dataLen) {
                size_t lenDataSent = bt::send(this->internalSocket, currentData, dataLen-sentData, 0);
                if (lenDataSent == SOCKET_ERROR) {
                    return false;
                }
                sentData += lenDataSent;
                currentData += lenDataSent;
            }
            return true;
        }

        bool sendAck() {
            char* dataToSend = (char*) malloc(BT_TAB_ACK_SIZE);
            // check to make sure malloc() succeded
            if (dataToSend == NULL) {
                return false;
            }
            // data for 👍 emoji
                dataToSend[0] = 0xf0;
                dataToSend[1] = 0x9f;
                dataToSend[2] = 0x91;
                dataToSend[3] = 0x8d;
            bool success = sendAllDataToSocket(dataToSend, BT_TAB_ACK_SIZE);
            free(dataToSend);
            return true;
        }
        
        bool sendNack() {
            char* dataToSend = (char*) malloc(BT_TAB_NACK_SIZE);
            // check malloc()
            if (dataToSend == NULL) {
                return false;
            }

            // data for 👎 emoji
                dataToSend[0] = 0xf0;
                dataToSend[1] = 0x9f;
                dataToSend[2] = 0x91;
                dataToSend[3] = 0x8e;
            bool success = sendAllDataToSocket(dataToSend, BT_TAB_NACK_SIZE);
            free(dataToSend);
            return true;
        }

        /**
         * @returns if bluetooth socket is ready to receive some info
        */
        bool readyToRead() {
            if (this->errorCode) {
                return false; // we aren't ready if there's been an error
            }

            bt::TIMEVAL disconnectTime = {0}; // ensures that we don't wait for socket to be ready, just want to know if it currently is
                disconnectTime.tv_sec = 0;
                disconnectTime.tv_usec = 0;
            bt::fd_set socketsToScan = {0};
                memset(&socketsToScan, 0, sizeof(bt::fd_set)); // ensures struct is zero (should be garuenteed but yk)

            socketsToScan.fd_array[0] = this->internalSocket; // set only 1 element, as we are scanning for 1 socket
                socketsToScan.fd_count = 1;
            
            size_t sockState = bt::select(0, &socketsToScan, NULL, NULL, &disconnectTime); // scan for socket reading op
            if (sockState == SOCKET_ERROR) {
                if (errorIsFatal(bt::WSAGetLastError())) {
                    this->errorCode = bt::WSAGetLastError();
                    this->apoptosis = true;
                }
                return false;
            }
            return (sockState != 0);
        }

        /**
         * @brief starts internal read operation with previously defined policy
        */
        bt::READRES readTabletData() {
            // check to make sure they didn't call this function accidentally
            bt::READRES retVal;
            if (!this->readyToRead()) {
                retVal.data = returnEmptyFuture();
                retVal.transactionType = bt::TRANS_SOCKET_ERROR;
                retVal.reportedSuccess = false;
                return retVal;
            }

            retVal.transactionType = this->getTransactionType();
            if (retVal.transactionType == bt::TRANS_SOCKET_ERROR) {
                retVal.data = returnEmptyFuture();
                retVal.transactionType = bt::TRANS_SOCKET_ERROR;
                retVal.reportedSuccess = false;
                return retVal;
            }

            switch(this->callType) {
                case bt::CALLTYPE_ASYNC:
                {
                    retVal.data = std::async(std::launch::async, &bthSocketHandler::internalRead, this, std::ref(retVal.reportedSuccess));
                }
                break;
                
                case bt::CALLTYPE_DEFERRED:
                {
                    retVal.data = std::async(std::launch::deferred, &bthSocketHandler::internalRead, this, std::ref(retVal.reportedSuccess));
                }
                break;

                case bt::CALLTYPE_DEFAULT:
                {
                    retVal.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(retVal.reportedSuccess));
                }
                break;

                default:
                    retVal.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(retVal.reportedSuccess));
            }
            return retVal;
        }

        bt::TRANSACTIONTYPE getTransactionType() {
            bool success;
            char* transactionPtr = readAllExpectedDataFromSocket(BT_EXPECTED_DATA_INITIAL, success);
            if (!success) {
                return bt::TRANS_SOCKET_ERROR;
            }
            char transactionType = (*transactionPtr);
            free(transactionPtr);
            return static_cast<bt::TRANSACTIONTYPE>(transactionType);
        }

        std::vector<char> internalRead(bool& success) {
            // make sure the tablet actually wants to communicate (though if we're this far, it probably is)
            if (!this->readyToRead()) {
                return std::vector<char>();
            }

            /*
                Communication will go as follows:
                    C: 👍 (Trailing ACK)
                    T: # of bytes to be sent (signed int)
                    C: 👍 (ACK)
                    T: all data
                    C: 👍 (ACK)
            */
           success = true;
           if (!sendAck()) {
                success = false;
                return std::vector<char>();
           }

           // get the # of bytes to be sent
           char* numOfByteData = readAllExpectedDataFromSocket(BT_EXPECTED_DATA_READSIZE, success);
           if (!success || !invertEndianness(numOfByteData, BT_EXPECTED_DATA_READSIZE)) {
                sendNack();
                success = false; // for good measure
                return std::vector<char>();
           }
           int bytesExpected = *((int*)numOfByteData);
           free(numOfByteData);

           // ack dataSize recvd
           if (!sendAck()) {
                success = false;
                return std::vector<char>();
           }

           // read expected data from socket && errorcheck
           char* expectedData = readAllExpectedDataFromSocket(bytesExpected, success);
           if (!success) {
                sendNack();
                success = false;
                return std::vector<char>();
           }

           // convert char* to vector and return
           std::vector<char> charData;
           charData.reserve(bytesExpected);
           for(int i = 0; i < bytesExpected; i++) {
                charData.push_back(expectedData[i]);
           }
           // cleaning up
           free(expectedData);
           success = true;
           return charData;
        }


        /**
         * @returns -1 if no error, anything else if error
        */
        int getErrorCode() {
            return this->errorCode;
        }
        /**
         * @returns true if socket should die, false otherwise
        */
        bool getErrorState() {
            return this->apoptosis;
        }
        /**
         * @returns current calling policy
        */
        bt::SOCKETCALLTYPE getLaunchPolicy() {
            return this->callType;
        }

        void setLaunchPolicy(bt::SOCKETCALLTYPE callType) {
            this->callType = callType;
        }
};

#endif