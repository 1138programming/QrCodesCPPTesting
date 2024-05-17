#ifndef BTHSOCKETHANDLER
#define BTHSOCKETHANDLER

#include "btIncludes.hpp"
#include <future>

class bthSocketHandler {
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

            char* dataPtrCopy = dataPtr; // will be used as return val

            size_t dataRecvd = 0;
            while (dataRecvd < dataSizeExpected) {
                
            }
        }

        /**
         * @returns if bluetooth socket is ready to receive some info
        */
        bool scan() {
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
};

#endif