#ifndef BTTABOBJ_HPP
#define BTTABOBJ_HPP

#include "btIncludes.hpp"
#include "debugConosole.hpp"

class BtTabObj {
    private:
        std::string macStr;
        bt::SOCKADDR_BTH macAddr;
        bt::SOCKET socket;
        bt::SOCKETCALLTYPE callType;

        std::string tabScoutingName;
    public:
        BtTabObj(bt::SOCKET socket, bt::BLUETOOTH_ADDRESS_STRUCT addr, std::string addrStr) {
            this->socket = socket;
            this->macAddr = addr;
            this->macStr = addrStr;

            this->callType = bt::CALLTYPE_DEFAULT;
        }

        /*********************************************/
        /* COMMUNICATION HELPERS/HANDLERS */
        /*********************************************/
        /**
         * @brief The windows read functions almost always return the data backwards, so this function flips the char* in place.
         * @warning ```ptr``` still needs to be freed by caller
         */
        bool invertEndianness(char* ptr, size_t ptrSize) {
            // copy pointer, returning false if failing
            char* ptrCopy = (char*)malloc(ptrSize);
            if (ptrCopy == NULL) {
                return false;
            }
            // invert all bits
            memcpy(ptrCopy, ptr, ptrSize);
            for (int i = 0; i < ptrSize; i++) {
                ptr[i] = ptrCopy[ptrSize-i-1]; // -i-1 because i < ptrSize doesn't get to ptrSize
            }
            free(ptrCopy);
            return true;
        }
        /**
         * @brief Windows returns data in parts (so that you can access it faster if you needed to).
         * 
         * This function just reads all the expected data, because it doesn't really matter if we wait a small amount.
         * @warning If success is false, the returned char* is likely NULL. Don't write/read to/from it.
         * 
         */
        char* readAllSocketData(size_t sizeExpected, bool& success) {
            success = true;
            // return nothing if no data expected (obv)
            if (sizeExpected < 1) {
                success = true;
                return NULL;
            }

            char* dataPtr = (char*)malloc(sizeExpected); // create buffer & check success
            if (dataPtr = NULL) {
                success = false;
                return NULL;
            }

            char* dataPtrStart = dataPtr; // dataPtr will be modified
            size_t dataRecvd = 0;
            while (dataRecvd < sizeExpected) {
                size_t currentLenRecvd = bt::recv(this->socket, dataPtr, sizeExpected-dataRecvd, 0);
                // Windows detected a graceful close 🥰
                if (currentLenRecvd == 0) {
                    DebugConsole::println("BLUETOOTH SOCKET CLOSED: the tablet's fault", DBGC_YELLOW);
                    success = false;
                    return NULL;
                }
                // Error handling (only some are actually a problem)
                if (currentLenRecvd == SOCKET_ERROR) {
                    int currError = bt::WSAGetLastError();
                    if (currError == WSAEWOULDBLOCK || currError == WSAEINPROGRESS) {
                        continue; // errors when no data or winsock is busy, so just try again lol
                    }
                    DebugConsole::println("Socket communication error. (closing)", DBGC_YELLOW);
                    success = false;
                    return NULL;
                }
                else {
                    dataRecvd += currentLenRecvd;
                    dataPtr += currentLenRecvd;
                }
            }
            return dataPtrStart;
        }
        /**
         * @brief sends ```size``` bytes from ```data``` to the internal socket.
         * 
         * ```data``` is unaffected and should still be freed in some way by the caller
         */
        bool sendToSocket(char* data, size_t size) {
            if (size < 1) {
                return true; // yayy, we win!
            }
            char* dataCurr = data;
            size_t sentData = 0;
            while (sentData < size) {
                size_t currSentData = bt::send(this->socket, dataCurr, size-sentData, 0);
                if (currSentData == SOCKET_ERROR) {
                    if (bt::WSAGetLastError() == WSAEWOULDBLOCK) {
                        continue;
                    }
                    else {
                        return false;
                    }
                }
                sentData += currSentData;
                dataCurr += currSentData;
            }
            return true;
        }


        /*********************************************/
        /* ACK/NACK R/W FUNCTIONS */
        /*********************************************/
        /**
         * @returns If the operation succeded
         */
        bool sendAck() {
            char* dataToSend = (char*)malloc(BT_TAB_ACK_SIZE);
            // check to make sure malloc(succeded)
            if (dataToSend == NULL) {
                return false;
            }
            // data for ACK
                dataToSend[0] = 'A';
                dataToSend[1] = 'C';
                dataToSend[2] = 'K';
            bool success = sendToSocket(dataToSend, BT_TAB_ACK_SIZE);
            free(dataToSend);
            return success;
        }
        /**
         * @returns If the operation succeded
         */
        bool sendNack() {
            char* dataToSend = (char*)malloc(BT_TAB_NACK_SIZE);
            // check malloc()
            if (dataToSend == NULL) {
                return false;
            }
            // data for NACK
                dataToSend[0] = 'N';
                dataToSend[1] = 'C';
                dataToSend[2] = 'K';
            bool success = sendToSocket(dataToSend, BT_TAB_NACK_SIZE);
            free(dataToSend);
            return success;
        }
        /**
         * @returns If an ack was read, otherwise it's safe to assume there was a problem/nack
         */
        bool readAck() {
            char* ackData = (char*)malloc(BT_TAB_ACK_SIZE);
            if (ackData == NULL) {
                return false
            }
            // data for ACK (backwards because winsock returns it that way (TODO: check))
                ackData[0] = 'K';
                ackData[1] = 'C';
                ackData[2] = 'A';
            bool success;
            char* dataRecvd = readAllSocketData(BT_TAB_ACK_SIZE, success);
            if (!success) {
                return false;
            }

            return (strncmp(ackData, dataRecvd, BT_TAB_ACK_SIZE) == 0);
        }
};

#endif