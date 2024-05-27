#ifndef BTHSOCKETHANDLER
#define BTHSOCKETHANDLER

#include "btIncludes.hpp"
#include <future>

class BthSocketHandler {
    private:
        bt::SOCKET internalSocket;
        bool apoptosis; // fatal socket error?
        int errorCode; // error code if error
        std::future<std::vector<char>> data;
        bt::SOCKETCALLTYPE callType;
    public:
        btSocketHandler(bt::SOCKET socket) {
            this->internalSocket = socket;
            this->apoptosis = false;
            this->errorCode = -1;
            this->callType = bt::CALLTYPE_DEFAULT;
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
                if (!this->scanReady()) {
                    success = false;
                    return nullptr;
                }
                size_t currentLengthRecvd = bt::recv(this->internalSocket, dataPtr, dataSizeExpected-dataRecvd, 0);
                // graceful close 🥰
                if (currentLengthRecvd == 0) {
                    this->apoptosis = true;
                    success = false;
                    return nullptr;
                }
                // if SOCKET_ERROR is returned, there was an error (obv.)
                if (currentLengthRecvd == SOCKET_ERROR) {
                    // catches all fatal errors
                    switch(bt::WSAGetLastError()) {
                        case 10038:
                        {
                            //WSAENOTSOCK
                            this->apoptosis = true;
                            this->errorCode = 10038;
                        }
                        break;
                        case 10052:
                        {
                            //WSAENETRESET
                            this->apoptosis = true;
                            this->errorCode = 10052;
                        }
                        break;
                        case 10058:
                        {
                            //WSAESHUTDOWN
                            this->apoptosis = true;
                            this->errorCode = 10058;
                        }
                        break;
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

        /**
         * @returns if bluetooth socket is ready to scan some info
        */
        bool scanReady() {
            if (this->apoptosis) {
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
                this->apoptosis = true;
                this->errorCode = bt::WSAGetLastError();
                return false;
            }
            return (sockState != 0);
        }
        bool scanReady(bt::TIMEVAL disconnectTime) {
            if (this->apoptosis) {
                return false; // we aren't ready if there's been an error
            }
            
            bt::fd_set socketsToScan = {0};
                memset(&socketsToScan, 0, sizeof(bt::fd_set)); // ensures struct is zero (should be garuenteed but yk)

            socketsToScan.fd_array[0] = this->internalSocket; // set only 1 element, as we are scanning for 1 socket
                socketsToScan.fd_count = 1;

            size_t sockState = bt::select(0, &socketsToScan, NULL, NULL, &disconnectTime); // scan for socket reading op
            if (sockState == SOCKET_ERROR) {
                this->apoptosis = true;
                this->errorCode = bt::WSAGetLastError();
                return false;
            }
            return (sockState != 0);
        }

        /**
         * @brief starts internal read operation with previously defined policy
        */
        void initRead() {
            switch(this->callType) {
                case bt::CALLTYPE_ASYNC:
                {
                    this->data = std::async(std::launch::async, readData, this);
                }
                break;
                
                case bt::CALLTYPE_DEFERRED:
                {
                    this->data = std::async(std::launch::deferred, readData, this);
                }
                break;

                case bt::CALLTYPE_DEFAULT:
                {
                    this->data = std::async(std::launch::deferred | std::launch::async, readData, this);
                }
                break;

                default:
                    this->data = std::async(std::launch::deferred | std::launch::async, readData, this);
            }
        }
        std::vector<char> readData() {
            
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

        void destroySelf() {
            bt::closesocket(this->internalSocket);
        }
};

#endif