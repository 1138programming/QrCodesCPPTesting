#ifndef BTTABOBJ_HPP
#define BTTABOBJ_HPP

#include "../btIncludes.hpp"
#include "../debugConsole.hpp"
#include "bluetoothTransaction.hpp"
#include "../../include/raylib.hpp"

#include <vector>
#include <optional>

class BtTabObj {
    private:
        std::string macStr;
        bt::SOCKADDR_BTH macAddr;
        bt::SOCKET socket;
        bt::SOCKETCALLTYPE callType = bt::CALLTYPE_DEFAULT;
        bt::TABTRANSACTION currTrans;

        std::string tabScoutingName = std::string("NULL");
        bool transactionState = false;

        /*********************************************/
        /* PRIVATE COMMUNICATION PROTO PRIMATIVES */
        /*********************************************/
        std::optional<std::vector<char>> returnEmptyVector() {
            return std::nullopt;
        }
        bt::TRANSACTIONTYPE getTransactionType() {
            bool success;
            char* transRes = readAllSocketData(BT_EXPECTED_DATA_INITIAL, success);
            if (!success) {
                return bt::TRANS_SOCKET_ERROR;
            }
            char transactionType = (*transRes);
            free(transRes);
            return (bt::TRANSACTIONTYPE)transactionType;
        }
        
    public:
        BtTabObj(bt::SOCKET socket, bt::SOCKADDR_BTH addr, std::string addrStr) {
            this->socket = socket;
            this->macAddr = addr;
            this->macStr = addrStr;
            this->tabScoutingName = addrStr;

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
         * 
         * This function has a timeout of 5s of no recvd data
         * @warning If success is false, the returned char* is likely NULL. Don't write/read to/from it.
         * @warning the returned char* needs to be freed (using free())
         */
        char* readAllSocketData(size_t sizeExpected, bool& success) {
            success = true;
            double timeoutTime = (GetTime() + 5.0);
            
            // return nothing if no data expected (obv)
            if (sizeExpected < 1) {
                success = true;
                return NULL;
            }

            char* dataPtr = (char*)malloc(sizeExpected); // create buffer & check success
            if (dataPtr == NULL) {
                success = false;
                return NULL;
            }

            char* dataPtrStart = dataPtr; // dataPtr will be modified
            size_t dataRecvd = 0;
            while (dataRecvd < sizeExpected) {
                size_t currentLenRecvd = bt::recv(this->socket, dataPtr, sizeExpected-dataRecvd, 0);
                if (currentLenRecvd > 0 && currentLenRecvd != SOCKET_ERROR) {
                    timeoutTime = (GetTime() + 5.0);
                }
                if (GetTime() > timeoutTime) {
                    DebugConsole::println("Communication with tab timed out.", DBGC_RED, DBGL_ERROR);
                    success = false;
                    return NULL;
                }
                // Windows detected a graceful close 🥰
                if (currentLenRecvd == 0) {
                    DebugConsole::println("BLUETOOTH SOCKET CLOSED: the tablet's fault", DBGC_YELLOW);
                    success = false;
                    return NULL;
                }
                // Error handling (only some are actually a problem)
                if (currentLenRecvd == SOCKET_ERROR) {
                    int currError = bt::WSAGetLastError();
                     // errors when no data or winsock is busy, so just try again lol
                    if (currError == WSAEWOULDBLOCK || currError == WSAEINPROGRESS) {
                        continue;
                    }
                    DebugConsole::println(std::string("Socket communication error. (closing) (error=") + std::to_string(currError) + std::string(")"), DBGC_YELLOW);
                    success = false;
                    return NULL;
                }
                dataRecvd += currentLenRecvd;
                dataPtr += currentLenRecvd;
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
                return false;
            }
            // data for ACK
                ackData[0] = 'A';
                ackData[1] = 'C';
                ackData[2] = 'K';
            bool success;
            char* dataRecvd = readAllSocketData(BT_TAB_ACK_SIZE, success);
            if (!success) {
                return false;
            }
            return (strncmp(ackData, dataRecvd, BT_TAB_ACK_SIZE) == 0);
        }


        /*********************************************/
        /* SOCKET INFO OPERATIONS */
        /*********************************************/
        /**
         * @returns Whether the windows gods tell us if the socket has data to read in a non-blocking manner (includes requests to close socket)
         * @warning If there has been a non-graceful close, this will always return false
         */
        bool readyToRead() {
            bt::TIMEVAL waitTime = {0};
                waitTime.tv_sec = 0; // should already be set, but make sure, as this is important
                waitTime.tv_usec = 0;
            bt::fd_set socketsToScan = {0};
                socketsToScan.fd_count = 1;
                socketsToScan.fd_array[0] = this->socket;

            int sockState = bt::select(0, &socketsToScan, NULL, NULL, &waitTime);
            if (sockState == SOCKET_ERROR) {
                return false;
            }
            return (sockState != 0); // 0 == "graceful" not ready
        }
        /**
         * @returns Whether the windows gods tell us if the socket is able to be written to (this does not always garuentee the call will be non-blocking).
         * 
         * Generally, it is probably okay to assume that the socket is able to be written to, as there are not many situations in which this will not be the case.
         */
        bool readyToWrite() {
            bt::TIMEVAL waitTime = {0};
                waitTime.tv_sec = 0; // should already be set, but make sure, as this is important
                waitTime.tv_usec = 0;
            bt::fd_set socketsToScan = {0};
                socketsToScan.fd_count = 1;
                socketsToScan.fd_array[0] = this->socket;
            
            int sockState = bt::select(0, NULL, &socketsToScan, NULL, &waitTime);
            if (sockState == SOCKET_ERROR) {
                return false;
            }
            return (sockState != 0);
        }


        /*********************************************/
        /* COMMUNICATION PROTO PRIMATIVES */
        /*********************************************/
        /**
         * @brief This function should be called in order for the transaction to be handled by another class.
         * @returns a ```TABTRANSACTION``` w/ only ```parent```, ```success``` (which will also need to be checked after data is gotten), ```transactionType```, and ```writeTransaction``` params.
        */
        bt::TABTRANSACTION* readTransactionData() {
            this->currTrans.parent = this;

            if (!this->readyToRead() || !this->readyToWrite()) {
                DebugConsole::println("Tablet not ready to read/write", DBGC_RED);
                this->currTrans.success = false;
                this->currTrans.transactionType = bt::TRANS_CLOSE_SOCKET;
                this->currTrans.writeTransaction = false;
                return &this->currTrans;
            }

            this->currTrans.transactionType = this->getTransactionType();
            if (this->currTrans.transactionType == bt::TRANS_SOCKET_ERROR) {
                DebugConsole::println("getTransactionType() returned TRANS_SOCKET_ERROR", DBGC_RED);
                this->currTrans.success = false;
                this->currTrans.writeTransaction = false;
                return &this->currTrans;
            }
            this->currTrans.writeTransaction = (((char)this->currTrans.transactionType) < 0);

            return &this->currTrans;
        }
        std::optional<std::vector<char>> internalRead(bool& success) {
            /*
                Communication will go as follows:
                    C: 👍 (Trailing ACK)
                    T: # of bytes to be sent (signed int)
                    C: 👍 (ACK)
                    T: all data
                    C: 👍 (ACK)
            */
           DebugConsole::println("Writing to tablet", DBGC_GREEN, DBGL_DEVEL);
           success = true;
           if (!this->sendAck()) {
                success = false;
                return std::vector<char>();
            }
            
            // get # of bytes to be sent
            char* numOfByteDataPtr = readAllSocketData(BT_EXPECTED_DATA_READSIZE, success);
            if (!success) {
                sendNack();
                success = false;
                return std::vector<char>();
            }
            int bytesExpected = ((int*)numOfByteDataPtr)[0];
            free(numOfByteDataPtr);
            
            // ack recvd data size
            if (!sendAck()) {
                success = false;
                return std::vector<char>();
            }
            
            // read # of bytes from socket and return as vector
            char* messagePtr = readAllSocketData(bytesExpected, success);
            if (!success) {
                sendNack();
                return std::vector<char>(); // (success already false)
            }

            std::vector<char> messageData;
            messageData.reserve(bytesExpected);
            for (int i = 0; i < bytesExpected; i++) {
                messageData.push_back(messagePtr[i]);
            }
            free(messagePtr);
            return messageData;
        }
        std::optional<std::vector<char>> internalWrite(std::vector<char> data, bool& success) {
            /*
                Communication will go as follows:
                    C: # of bytes to be sent (signed int) (trailing from connection number)
                    T: ACK
                    C: all data
                    T: ACK
            */
           DebugConsole::println(std::string("Writing to tablet"), DBGC_GREEN, DBGL_DEVEL);

           success = true;

           // send # of bytes to tablet
           int dataSize = data.size();
           success = (success && this->sendToSocket((char*)&dataSize, BT_EXPECTED_DATA_READSIZE));
           DebugConsole::println(std::string("Data Size: ") + std::to_string(dataSize), DBGC_BLUE, DBGL_DEVEL);
           // read ack
           success = (success && readAck());
           if (!success) {
               DebugConsole::println(std::string("No ack success :C"), DBGC_BLUE, DBGL_DEVEL);
               return std::optional<std::vector<char>>();
            }
            DebugConsole::println(std::string("Read Ack"), DBGC_BLUE, DBGL_DEVEL);
            //send full data
            success = (success && sendToSocket(data.data(), data.size()));
            DebugConsole::println(std::string("Data sent"), DBGC_BLUE, DBGL_DEVEL);
            // read ack
            success = (success && readAck());
            DebugConsole::println(std::string("Read Ack"), DBGC_BLUE, DBGL_DEVEL);

           return std::optional<std::vector<char>>();
        }


        /*********************************************/
        /* GETTER/SETTER FUNCTIONS*/
        /*********************************************/
        std::string getScoutingName() {
            return this->tabScoutingName;
        }
        void setScoutingName(std::string name) {
            this->tabScoutingName = name;
        }

        std::string getMacAddrStr() {
            return this->macStr;
        }
        bt::SOCKADDR_BTH getMacAddrStruct() {
            return this->macAddr;
        }

        bt::SOCKETCALLTYPE getCallType() {
            return this->callType;
        }
        void setCallType(bt::SOCKETCALLTYPE type) {
            this->callType = type;
        }

        bool undergoingTransaction() {
            return this->transactionState;
        }
        void setTransactionState(bool isUndergoing) {
            this->transactionState = isUndergoing;
        }

        bt::SOCKET getWinsockSocket() {
            return this->socket;
        }


        /*********************************************/
        /* DELETION FUNCTIONS*/
        /*********************************************/
        /**
         * @brief Kills internal socket through winsock. Makes basically everything invalid
         * @warning should only ever be called once
         * @returns the winsock value returned by closesocket()
         */
        int sockSuicide() {
            return bt::closesocket(this->socket);
        }
};

#endif