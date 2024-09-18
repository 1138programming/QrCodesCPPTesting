#ifndef BTHSOCKETHANDLER
#define BTHSOCKETHANDLER

#include "btIncludes.hpp"
#include <future>
#include <algorithm>

class bthSocketHandler {
    private:
        bthSocketWrapper internalSocketMgr;
        bool apoptosis; // fatal socket error?
        int errorCode; // error code if error
        bt::SOCKETCALLTYPE callType;
        bt::READRES currentRead; // we don't want to just keep calling it.

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
        bthSocketHandler(bthSocketWrapper socket) {
            this->internalSocketMgr = socket;
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
                std::cout << (int)ptr[i] << std::endl;
                ptr[i] = ptrCopy[ptrSize-i-1];
            }
            free(ptrCopy);
            return true;
        }


        char* readAllExpectedDataFromSocket(size_t dataSizeExpected, bool& success) {
            success = true;
            // return nothing if no memory given (obv.)
            if (dataSizeExpected < 1) {
                std::cout << "?????" << std::endl;
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
                size_t currentLengthRecvd = bt::recv(this->internalSocketMgr.socket, dataPtr, dataSizeExpected-dataRecvd, 0);
                // graceful close 🥰
                if (currentLengthRecvd == 0) {
                    DebugConsole::print("BLUETOOTH SOCKET CLOSED: the tablet's fault\n", DBGC_YELLOW);
                    this->apoptosis = true;
                    success = false;
                    return nullptr;
                }
                // if SOCKET_ERROR is returned, there was an error (obv.)
                if (currentLengthRecvd == SOCKET_ERROR) {
                    int currError = bt::WSAGetLastError();
                    if (currError == 10035) {
                        continue;
                    }
                    std::cout << "why?: " << currError << std::endl;
                    // catches most fatal errors
                    if (this->errorIsFatal(currError)) {
                        this->apoptosis = true;
                        this->errorCode = currError;
                        // TODO: unsafe?
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
                size_t lenDataSent = bt::send(this->internalSocketMgr.socket, currentData, dataLen-sentData, 0);
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
            if (this->apoptosis) {
                return false; // we aren't ready if there's been an error
            }

            bt::TIMEVAL disconnectTime = {0}; // ensures that we don't wait for socket to be ready, just want to know if it currently is
                disconnectTime.tv_sec = 0;
                disconnectTime.tv_usec = 0;
            bt::fd_set socketsToScan = {0};
                memset(&socketsToScan, 0, sizeof(bt::fd_set)); // ensures struct is zero (should be garuenteed but yk)

            socketsToScan.fd_array[0] = this->internalSocketMgr.socket; // set only 1 element, as we are scanning for 1 socket
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
        bt::READRES* readTabletData() {
            this->currentRead->parentSocket = this->internalSocketMgr;

            // check to make sure they didn't call this function accidentally
            if (!this->readyToRead()) {
                std::cerr << "Tablet not ready to read" << std::endl;
                this->currentRead.data = returnEmptyFuture();
                this->currentRead.transactionType = bt::TRANS_SOCKET_ERROR;
                this->currentRead.reportedSuccess = false;
                return &this->currentRead;
            }

            this->currentRead.transactionType = this->getTransactionType();
            if (this->currentRead.transactionType == bt::TRANS_SOCKET_ERROR) {
                std::cout << "getTransactionType() returned TRANS_SOCKET_ERROR" << std::endl;
                this->currentRead.data = returnEmptyFuture();
                this->currentRead.transactionType = bt::TRANS_SOCKET_ERROR;
                this->currentRead.reportedSuccess = false;
                return &this->currentRead;
            }

            switch(this->callType) {
                case bt::CALLTYPE_ASYNCHRONOUS:
                {
                    this->currentRead.data = std::async(std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
                }
                break;
                
                case bt::CALLTYPE_DEFERRED:
                {
                    this->currentRead.data = std::async(std::launch::deferred, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
                }
                break;

                case bt::CALLTYPE_DEFAULT:
                {
                    this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
                }
                break;

                default:
                    this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
            }
            return &this->currentRead;
        }
        /***
            @warning IF YOU WANT TO OWN THE CHILD, *YOU* MUST KILL IT PROPERLY (you get memory ownership)
        */
        bt::READRES* transferReadresOwnership() {
            bt::READRES* orphan = (bt::READRES*)malloc(sizeof(this->currentRead));
            memcpy(orphan, &(this->currentRead), sizeof(this->currentRead)); // pull a fast one on c++ and don't tell them we moved it 🤫 (TODO: FIX)
            //orphan->data = std::move(this->currentRead.data); // make sure cpp does cpp things
            return orphan;
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

            std::cout << "CKPT 1" << std::endl;

           // get the # of bytes to be sent
           char* numOfByteData = readAllExpectedDataFromSocket(BT_EXPECTED_DATA_READSIZE, success);
           std::cout << "CKPT 1.5: " << success << std::endl;
           if (!success || !invertEndianness(numOfByteData, BT_EXPECTED_DATA_READSIZE)) {
                std::cout << "CKPT 1b" << std::endl;
                sendNack();
                success = false; // for good measure
                return std::vector<char>();
           }
           int bytesExpected = ((int*)numOfByteData)[0];
           free(numOfByteData);

            std::cout << "CKPT 2" << std::endl;
           // ack dataSize recvd
           if (!sendAck()) {
                success = false;
                return std::vector<char>();
           }
            std::cout << "CKPT 3" << std::endl;

           // read expected data from socket && errorcheck
           char* expectedData = readAllExpectedDataFromSocket(bytesExpected, success);
           if (!success) {
                sendNack();
                success = false;
                return std::vector<char>();
           }

            std::cout << "CKPT 4" << std::endl;
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

        void closeSocket() {
            checkSuccessWinsock<int>(bt::closesocket(this->internalSocketMgr.socket), 0, "failed to propely close socket (minor memory leak)");
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