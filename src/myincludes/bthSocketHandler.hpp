#ifndef BTHSOCKETHANDLER
#define BTHSOCKETHANDLER

#include "btIncludes.hpp"
#include <future>

class bthSocketHandler {
    private:
        bt::SOCKET internalSocket;
        bool apoptosis; // fatal socket error?
        int errorCode;
    public:
        btSocketHandler(bt::SOCKET socket) {
            this->internalSocket = socket;
            this->apoptosis = false;
            this->errorCode = -1;
        }

        /**
         * @returns if bluetooth socket is ready to receive some info
        */
        bool scan() {
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
         * @returns A std::future<> of byte[]
        */
        std::future<std::vector<char>> readData() {

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
};

#endif