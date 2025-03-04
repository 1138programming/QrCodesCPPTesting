// #ifndef BTHSOCKETHANDLER
// #define BTHSOCKETHANDLER

// #include "btIncludes.hpp"
// #include <future>

// class bthSocketHandler {
//     private:
//         bt::SOCKET internalSocket;
//         bool apoptosis; // fatal socket error?
//         int errorCode; // error code if error
//         bt::SOCKETCALLTYPE callType;
//         bt::READRES currentRead; // we don't want to just keep calling it.

//         bool errorIsFatal(int errorCode) {
//             switch(errorCode) {
//                 case 10038:
//                 {
//                     //WSAENOTSOCK
//                     return true;
//                 }
//                 break;
//                 case 10052:
//                 {
//                     //WSAENETRESET
//                     return true;
//                 }
//                 case 10053:
//                 {
//                     //WSAECONNABORTED
//                     return true;
//                 }
//                 break;
//                 case 10058:
//                 {
//                     //WSAESHUTDOWN
//                     return true;
//                 }
//                 break;
//                 default:
//                     return false;
//             }
//         }

//         std::vector<char> returnEmptyVector() {
//             return std::vector<char>();
//         }
//         std::future<std::vector<char>> returnEmptyFuture() {
//             return std::async(std::launch::deferred, returnEmptyVector, this);
//         }
//     public:
//         bthSocketHandler(bt::SOCKET socket) {
//             this->internalSocket = socket;
//             this->apoptosis = false;
//             this->errorCode = -1;
//             this->callType = bt::CALLTYPE_DEFAULT;
//         }

//         bool invertEndianness(char* ptr, size_t ptrSize) {
//             // copy pointer, returning false if failing
//             char* ptrCopy = (char*) malloc(ptrSize);
//             if (ptrCopy == NULL) {
//                 return false;
//             }
//             // set all the bits of ptr to its inverse
//             memcpy(ptrCopy, ptr, ptrSize);
//             for (int i = 0; i < ptrSize; i++) {
//                 // -1 bc index starts @ 0
//                 std::cout << (int)ptr[i] << std::endl;
//                 ptr[i] = ptrCopy[ptrSize-i-1];
//             }
//             free(ptrCopy);
//             return true;
//         }


//         char* readAllExpectedDataFromSocket(size_t dataSizeExpected, bool& success) {
//             success = true;
//             // return nothing if no memory given (obv.)
//             if (dataSizeExpected < 1) {
//                 std::cout << "?????" << std::endl;
//                 success = false;
//                 return nullptr;
//             }
//             // create data && check if succeded
//             char* dataPtr = (char*)malloc(dataSizeExpected);
//             if (dataPtr == NULL) {
//                 success = false;
//                 return nullptr;
//             }

//             char* dataPtrCopy = dataPtr; // will be used as return val (we modify the original)

//             size_t dataRecvd = 0;
//             while (dataRecvd < dataSizeExpected) {
//                 size_t currentLengthRecvd = bt::recv(this->internalSocket, dataPtr, dataSizeExpected-dataRecvd, 0);
//                 // graceful close 🥰
//                 if (currentLengthRecvd == 0) {
//                     DebugConsole::print("BLUETOOTH SOCKET CLOSED: the tablet's fault\n", DBGC_YELLOW);
//                     this->apoptosis = true;
//                     success = false;
//                     return nullptr;
//                 }
//                 // if SOCKET_ERROR is returned, there was an error (obv.)
//                 if (currentLengthRecvd == SOCKET_ERROR) {
//                     int currError = bt::WSAGetLastError();
//                     if (currError == 10035) {
//                         continue;
//                     }
//                     std::cout << "why?: " << currError << std::endl;
//                     // catches most fatal errors
//                     if (this->errorIsFatal(currError)) {
//                         this->apoptosis = true;
//                         this->errorCode = currError;
//                         // TODO: unsafe?
//                     }
//                     success = false;
//                     return nullptr;
//                 }
//                 dataRecvd += currentLengthRecvd;
//                 dataPtr += currentLengthRecvd;
//             }
//             return dataPtrCopy; // other one points to end of data
//         }

//         bool sendAllDataToSocket(char* data, size_t dataLen) {
//             char* currentData = data;
//             size_t sentData = 0;
//             while (sentData < dataLen) {
//                 size_t lenDataSent = bt::send(this->internalSocket, currentData, dataLen-sentData, 0);
//                 if (lenDataSent == SOCKET_ERROR) {
//                     return false;
//                 }
//                 sentData += lenDataSent;
//                 currentData += lenDataSent;
//             }
//             return true;
//         }

//         bool sendAck() {
//             char* dataToSend = (char*) malloc(BT_TAB_ACK_SIZE);
//             // check to make sure malloc() succeded
//             if (dataToSend == NULL) {
//                 return false;
//             }
//             // data for ACK
//                 dataToSend[0] = 'A';
//                 dataToSend[1] = 'C';
//                 dataToSend[2] = 'K';
//             bool success = sendAllDataToSocket(dataToSend, BT_TAB_ACK_SIZE);
//             free(dataToSend);
//             return true;
//         }
        
//         bool sendNack() {
//             char* dataToSend = (char*) malloc(BT_TAB_NACK_SIZE);
//             // check malloc()
//             if (dataToSend == NULL) {
//                 return false;
//             }

//             // data for NACK
//                 dataToSend[0] = 'N';
//                 dataToSend[1] = 'C';
//                 dataToSend[2] = 'K';
//             bool success = sendAllDataToSocket(dataToSend, BT_TAB_NACK_SIZE);
//             free(dataToSend);
//             return true;
//         }
//         bool readAck() {
//             char* ackData = (char*) malloc(BT_TAB_ACK_SIZE);
//             if (ackData = NULL) {
//                 return false;
//             }
//             // data for ACK (backwards because we read it that way)
//                 ackData[0] = 'K';
//                 ackData[1] = 'C';
//                 ackData[2] = 'A';
//             bool success;
//             char* dataGotten = readAllExpectedDataFromSocket(BT_TAB_ACK_SIZE, success);
//             if (!success) {
//                 return false;
//             }
//             if (strncmp(ackData, dataGotten, BT_TAB_ACK_SIZE) != 0) {
//                 return false;
//             }
//             return true;
//         }

//         /**
//          * @returns if bluetooth socket is ready to receive some info
//         */
//         bool readyToRead() {
//             if (this->apoptosis) {
//                 return false; // we aren't ready if there's been an error
//             }

//             bt::TIMEVAL disconnectTime = {0}; // ensures that we don't wait for socket to be ready, just want to know if it currently is
//                 disconnectTime.tv_sec = 0;
//                 disconnectTime.tv_usec = 0;
//             bt::fd_set socketsToScan = {0};
//                 memset(&socketsToScan, 0, sizeof(bt::fd_set)); // ensures struct is zero (should be garuenteed but yk)

//             socketsToScan.fd_array[0] = this->internalSocket; // set only 1 element, as we are scanning for 1 socket
//                 socketsToScan.fd_count = 1;
            
//             size_t sockState = bt::select(0, &socketsToScan, NULL, NULL, &disconnectTime); // scan for socket reading op
//             if (sockState == SOCKET_ERROR) {
//                 if (errorIsFatal(bt::WSAGetLastError())) {
//                     this->errorCode = bt::WSAGetLastError();
//                     this->apoptosis = true;
//                 }
//                 return false;
//             }
//             return (sockState != 0);
//         }

//         /**
//          * @returns only the READRES.transactionType paramater and READRES.tabletWrite param- changing these or reading any other vals will result in undefined behaviour
//          * @warning MAKE SURE THE TABLET IS NOT WAITING FOR A RESPONSE FOR TOO LONG- after this function is called it WILL be expecting one. Drink Responsibly 👍
//          * @warning Make sure this function is added to if another transaction type is created!
//         */
//         bt::READRES* readTransactionData() {
//             this->currentRead.parentSocket = this->internalSocket;

//             // check to make sure they didn't call this function accidentally
//             if (!this->readyToRead()) {
//                 std::cerr << "Tablet not ready to read" << std::endl;
//                 this->currentRead.data = returnEmptyFuture();
//                 this->currentRead.transactionType = bt::TRANS_SOCKET_ERROR;
//                 this->currentRead.reportedSuccess = false;
//                 return &this->currentRead;
//             }

//             this->currentRead.transactionType = this->getTransactionType();
//             if (this->currentRead.transactionType == bt::TRANS_SOCKET_ERROR) {
//                 std::cout << "getTransactionType() returned TRANS_SOCKET_ERROR" << std::endl;
//                 this->currentRead.data = returnEmptyFuture();
//                 this->currentRead.transactionType = bt::TRANS_SOCKET_ERROR;
//                 this->currentRead.reportedSuccess = false;
//                 return &this->currentRead;
//             }
//             // TRANS_SOCKET_ERROR = -128,
//             // TRANS_CLOSE_SOCKET = 0,
//             // TRANS_WRITE_MATCH = 1,
//             // TRANS_WRITE_TABLET_INFO = 2,
//             // TRANS_CHECK_LOCAL_DB = 3,
//             // TRANS_UPDATE_LOCAL_DB = 4
//             switch (this->currentRead.transactionType) {
//                 case bt::TRANS_WRITE_MATCH: {
//                     this->currentRead.tabletWrite = true;
//                 }
//                 break;
//                 case bt::TRANS_WRITE_TABLET_INFO: {
//                     this->currentRead.tabletWrite = true;
//                 }
//                 break;
                
//                 case bt::TRANS_CHECK_LOCAL_DB: {
//                     this->currentRead.tabletWrite = false;
//                 }
//                 break;
//                 case bt::TRANS_UPDATE_LOCAL_DB: {
//                     this->currentRead.tabletWrite = false;
//                 }
//                 break;

//                 default:
//                     std::cout << "PROBLEM IN readTransactionData FUNCTION!!!!!! CHECK BTHSOCKETHANDLER CLASS!!" << std::endl;
//             }
//             return &this->currentRead;
//         }
//         /**
//          * @brief starts internal read operation with previously defined policy
//         */
//         bt::READRES* readTabletData() {
//             switch(this->callType) {
//                 case bt::CALLTYPE_ASYNCHRONOUS:
//                 {
//                     this->currentRead.data = std::async(std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;
                
//                 case bt::CALLTYPE_DEFERRED:
//                 {
//                     this->currentRead.data = std::async(std::launch::deferred, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;

//                 case bt::CALLTYPE_DEFAULT:
//                 {
//                     this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;

//                 default:
//                     this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalRead, this, std::ref(this->currentRead.reportedSuccess));
//             }
//             return &this->currentRead;
//         }
//         bt::READRES* writeTabletData(std::vector<char> data) {
//             switch(this->callType) {
//                 case bt::CALLTYPE_ASYNCHRONOUS:
//                 {
//                     this->currentRead.data = std::async(std::launch::async, &bthSocketHandler::internalWrite, this, data, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;
                
//                 case bt::CALLTYPE_DEFERRED:
//                 {
//                     this->currentRead.data = std::async(std::launch::deferred, &bthSocketHandler::internalWrite, this, data, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;

//                 case bt::CALLTYPE_DEFAULT:
//                 {
//                     this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalWrite, this, data, std::ref(this->currentRead.reportedSuccess));
//                 }
//                 break;

//                 default:
//                     this->currentRead.data = std::async(std::launch::deferred | std::launch::async, &bthSocketHandler::internalWrite, this, data, std::ref(this->currentRead.reportedSuccess));
//             }
//             return &this->currentRead;
//         }
//         /***
//             @warning IF YOU WANT TO OWN THE CHILD, *YOU* MUST KILL IT PROPERLY (you get memory ownership)
//         */
//         bt::READRES* transferReadresOwnership() {
//             bt::READRES* orphan = (bt::READRES*)malloc(sizeof(this->currentRead));
//             memcpy(orphan, &(this->currentRead), sizeof(this->currentRead)); // pull a fast one on c++ and don't tell them we moved it 🤫 (TODO: FIX)
//             //orphan->data = std::move(this->currentRead.data); // make sure cpp does cpp things
//             return orphan;
//         }

//         bt::TRANSACTIONTYPE getTransactionType() {
//             bool success;
//             char* transactionPtr = readAllExpectedDataFromSocket(BT_EXPECTED_DATA_INITIAL, success);
//             if (!success) {
//                 return bt::TRANS_SOCKET_ERROR;
//             }
//             char transactionType = (*transactionPtr);
//             free(transactionPtr);
//             return static_cast<bt::TRANSACTIONTYPE>(transactionType);
//         }

//         std::vector<char> internalRead(bool& success) {
//             /*
//                 Communication will go as follows:
//                     C: 👍 (Trailing ACK)
//                     T: # of bytes to be sent (signed int)
//                     C: 👍 (ACK)
//                     T: all data
//                     C: 👍 (ACK)
//             */
//            success = true;
//            if (!sendAck()) {
//                 success = false;
//                 return std::vector<char>();
//            }

//             std::cout << "CKPT 1" << std::endl;

//            // get the # of bytes to be sent
//            char* numOfByteData = readAllExpectedDataFromSocket(BT_EXPECTED_DATA_READSIZE, success);
//            std::cout << "CKPT 1.5: " << success << std::endl;
//            if (!success || !invertEndianness(numOfByteData, BT_EXPECTED_DATA_READSIZE)) {
//                 std::cout << "CKPT 1b" << std::endl;
//                 sendNack();
//                 success = false; // for good measure
//                 return std::vector<char>();
//            }
//            int bytesExpected = ((int*)numOfByteData)[0];
//            free(numOfByteData);

//             std::cout << "CKPT 2" << std::endl;
//            // ack dataSize recvd
//            if (!sendAck()) {
//                 success = false;
//                 return std::vector<char>();
//            }
//             std::cout << "CKPT 3" << std::endl;

//            // read expected data from socket && errorcheck
//            char* expectedData = readAllExpectedDataFromSocket(bytesExpected, success);
//            if (!success) {
//                 sendNack();
//                 success = false;
//                 return std::vector<char>();
//            }

//             std::cout << "CKPT 4" << std::endl;
//            // convert char* to vector and return
//            std::vector<char> charData;
//            charData.reserve(bytesExpected);
//            for(int i = 0; i < bytesExpected; i++) {
//                 charData.push_back(expectedData[i]);
//            }
//            // cleaning up
//            free(expectedData);
//            success = true;
//            return charData;
//         }
//         std::vector<char> internalWrite(std::vector<char> data, bool& success) {
//             /*
//                 Communication will go as follows:
//                     C: # of bytes to be sent (signed int) (trailing from connection number)
//                     T: ACK
//                     C: all data
//                     T: ACK
//             */
//            success = true;

//            // send # bytes to tab
//            int dataSize = data.size();
//            success = (success && sendAllDataToSocket((char*)&dataSize, BT_EXPECTED_DATA_READSIZE));
//            // read ack
//            success = (success && readAck());
//            // send full data 
//            success = (success && sendAllDataToSocket(data.data(), data.size()));
//            // read ack
//            success = (success && readAck());

//            std::vector<char> retData;
//               retData.push_back((char)success);
//            // yayy
//            return retData;
//         }


//         void closeSocket() {
//             checkSuccessWinsock<int>(bt::closesocket(this->internalSocket), 0, "failed to propely close socket (minor memory leak)");
//         }


//         /**
//          * @returns -1 if no error, anything else if error
//         */
//         int getErrorCode() {
//             return this->errorCode;
//         }
//         /**
//          * @returns true if socket should die, false otherwise
//         */
//         bool getErrorState() {
//             return this->apoptosis;
//         }
//         /**
//          * @returns current calling policy
//         */
//         bt::SOCKETCALLTYPE getLaunchPolicy() {
//             return this->callType;
//         }

//         void setLaunchPolicy(bt::SOCKETCALLTYPE callType) {
//             this->callType = callType;
//         }
// };

// #endif