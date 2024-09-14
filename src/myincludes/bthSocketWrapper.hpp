#ifndef BTHSOCKETWRAPPER
#define BTHSOCKETWRAPPER

#include "btIncludes.hpp"
#include "socketDBSelection.hpp"

class bthSocketWrapper {
    public:
        SOCKETDBSELECTION databaseTarget;
        bt::SOCKET socket;


        bthSocketWrapper() {
            this->socket = (bt::SOCKET)(~0); // INVALID_SOCKET
        }
        bthSocketWrapper(bt::SOCKET socket) {
            this->socket = socket;
        }

        // OPERATOR OVERLOADING
        bool operator==(const bthSocketWrapper& other) {
            return (this->socket == other.socket);
        }
};

#endif