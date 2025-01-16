#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

#include "btIncludes.hpp"
#include "bthSocketHandler.hpp"
#include "verticalScrollable.hpp"
#include "debugConsole.hpp"

#include <vector>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdint.h>

static const bt::GUID MY_GUID = {0x00001101, 0x0000, 0x1000, {0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}};

class Bluetooth {
    private:
        bt::SOCKET listener;
        std::vector<bt::SOCKET> connections = std::vector<bt::SOCKET>();
        std::vector<EzText> thingsToDrawList = std::vector<EzText>();
        VerticalScrollable connListDrawable = VerticalScrollable(600.0_spX, 200.0_spY, WHITE, 3.0);
        bt::BLUETOOTH_ADDRESS externalAddress;
        bt::BTH_ADDR localAddr;
        std::vector<bt::READRES*> runningBtTransactions; // _WE_ are responsible for killing these
        uint8_t port;

        bt::HANDLE btRadio; // init/shutdown vars
        bt::HBLUETOOTH_RADIO_FIND btFindingVal;

        // ___ Useful (private) functions ___
        void removeFromSocketVector(std::vector<bt::SOCKET>* vector, int element) {
            std::vector<bt::SOCKET>::iterator it = (vector->begin() + element);
            vector->erase(it);
        }
        void removeFromDrawablePtrVector(std::vector<Drawable*>* vector, int element) {
            std::vector<Drawable*>::iterator it = (vector->begin()+element);
            vector->erase(it);
        }
        void removeFromEzTextVector(std::vector<EzText>* vector, int element) {
            std::vector<EzText>::iterator it = (vector->begin()+element);
            vector->erase(it);
        }
        void removeFromCurrentConnectionsVector(std::vector<bt::READRES*>* vector, int element) {
            std::vector<bt::READRES*>::iterator it = (vector->begin()+element);
            vector->erase(it);
        }

        int getElement(std::vector<bt::SOCKET>* vector, bt::SOCKET thingToGet) {
            for (int i = 0; i < vector->size(); i++) {
                if (vector->at(i) == thingToGet) {
                    return i;
                }
            }
            return -1;
        }
        void killConnectionAndDisplay(int i) {
            removeFromSocketVector(&(this->connections), i);
            removeFromDrawablePtrVector(this->connListDrawable.getInternalVector(), i);
            removeFromEzTextVector(&(this->thingsToDrawList), i);
        }
        void killNaughtyConnections() {
            for (int i = 0; i < this->connections.size(); i++) {
                char* output = NULL;
                // i need this function to work lol
                while (output == NULL) {
                    output = (char*)malloc(sizeof(bt::DWORD)); // will be ignored
                }
                int outputSize = sizeof(bt::DWORD);
                if (bt::getsockopt(this->connections.at(i), SOL_SOCKET, SO_CONNECT_TIME, output, &outputSize) == 10038) {
                    killConnectionAndDisplay(i);
                }
            }
        }
    public:
        // ___ Simple BT functions ___
        int initAll() {
            bt::WSADATA wsaData;
            int startupVal = bt::WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (startupVal == 0) {
                DebugConsole::print("Bluetooth Successfully Initialized!\n", DBGC_GREEN);
            }
            return startupVal;
        }
        int cleanup() {
            return bt::WSACleanup();
        }

        // ___ useful (public) functions ___
        std::string getLocalMacStr() {
            std::ostringstream macBuilder;
            bt::BYTE* nameInBytes = (bt::BYTE*)(&this->localAddr);
            macBuilder << std::hex;
            for (int i = 5; i > 0; i--) {
                if (nameInBytes[i] <= 0x0f) {
                    macBuilder << 0;
                }
                macBuilder << (int)nameInBytes[i] << ":";
            }
            if (nameInBytes[0] <= 0x0f) {
                macBuilder << 0;
            }
            macBuilder << (int)(nameInBytes[0]);
            std::string macAddr = macBuilder.str();
            for (int i = 0; i < macAddr.length(); i++) {
                macAddr[i] = std::toupper(macAddr[i]);
            }
            return macAddr;
        }
        uint8_t getLocalPort() {
            return this->port;
        }
        int getNumConnections() {
            return this->connections.size();
        }
        int getHostNameStr(std::string* str) {
            char* ptr = (char*)calloc(256, sizeof(char));
            if (ptr == NULL) {
                return -1;
            }
            if (bt::gethostname(ptr, 256) != 0) {
                return -1;
            }

            (*str) = std::string(ptr);
            free(ptr);
            return 0;
        }
        VerticalScrollable* getConnList() {
            return &(this->connListDrawable);
        }

        // ___ Connection functions ___
        void initRadioHandles() {
            bt::BLUETOOTH_FIND_RADIO_PARAMS radioParams;
                radioParams.dwSize = sizeof(bt::BLUETOOTH_FIND_RADIO_PARAMS);

            this->btFindingVal = bt::BluetoothFindFirstRadio(&radioParams, &this->btRadio);
            if (this->btFindingVal == NULL) {
                DebugConsole::print("ERROR: no valid bluetooth radio found!", DBGC_RED);
            }
        }
        void freeRadioHandles() {
            bt::CloseHandle(this->btRadio);
            bt::BluetoothFindRadioClose(this->btFindingVal);
        }
        void makeDiscoverable() {
            initRadioHandles();
            
            if (bt::BluetoothEnableDiscovery(this->btRadio, true)) {
                DebugConsole::print("Enabled discoverabilitiy\n", DBGC_BLUE);
            }
            else {
                DebugConsole::print("Failed to enable discoverability", DBGC_RED);
            }
            freeRadioHandles();
        }
        void initAccept() {
            // ___init a windows socket in (normal) bluetooth mode___
            this->listener = bt::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
            if (this->listener == INVALID_SOCKET) {
                std::cerr << "big bad happened" << std::endl;
                std::cerr << bt::WSAGetLastError() << std::endl;
            }

            // ___binding addr___
            // create an address for the BT socket to bind to
            bt::SOCKADDR_BTH listenerAddr = {0};
                memset(&listenerAddr, 0, sizeof(listenerAddr)); // you can never be too sure
                listenerAddr.addressFamily = AF_BTH; // address family bluetooth
                listenerAddr.btAddr = 0; // if 0 = local MAC, as far as I'm aware.
                listenerAddr.port = BT_PORT_ANY; // ignore lol (3 could be occupied) ---- have a constant port for the tablets to connect to. (3 seemed not to be occupied in my (limited) testing)
                listenerAddr.serviceClassId = MY_GUID; // will be ignored, but why not set it lol 💀
            // bind the listener socket to the address we created
            checkSuccessWinsock<int>(bt::bind(this->listener, reinterpret_cast<bt::sockaddr*>(&listenerAddr), sizeof(listenerAddr)), 0, "Failed to bind to address"); // check above for function definition, in prevoius ver. checking for errors could get very bloated.

            // ___get socket data___
            bt::SOCKADDR_BTH localSocketName = {0}; // create struct to store addr
            int localSocketStructLen = sizeof(localSocketName); // for some reason you need this for the next function (thx windows!) (though I'm sure there's a good reason)
            checkSuccessWinsock<int>(bt::getsockname(this->listener, reinterpret_cast<bt::sockaddr*>(&localSocketName), &localSocketStructLen), 0, "Failed to get name of local socket (shouldn't be fatal?)");
            // set this data in-class
            this->localAddr = localSocketName.btAddr;
            this->port = localSocketName.port;
            if (this->port != 3) {
                toastHandler::add(Toast("WRONG PORT- CHECK CONSOLE", LENGTH_LONG));
                DebugConsole::print("Incorrect port- ensure there not another instance of the app open or tell the scouts to input the correct port in the menu. Make sure they do not change the MAC and DO NOT IGNORE THIS MESSAGE.\n", DBGC_RED);
            }

            //___This code advertises the BT socket to the world___
            //I do not really understand it, so dont ask (also it's very lengthy.)
            // create an info struct about the port(?)
            bt::LPCSADDR_INFO wsaQueryInfo = (bt::CSADDR_INFO*)calloc(1, sizeof(bt::CSADDR_INFO)); // it is a pointer, so we have to alloc() the memory
                wsaQueryInfo->LocalAddr.iSockaddrLength = sizeof(bt::SOCKADDR_BTH);
                wsaQueryInfo->LocalAddr.lpSockaddr = (bt::LPSOCKADDR)&listenerAddr;
                wsaQueryInfo->RemoteAddr.iSockaddrLength = sizeof(bt::SOCKADDR_BTH);
                wsaQueryInfo->RemoteAddr.lpSockaddr = (bt::LPSOCKADDR)&listenerAddr;
                wsaQueryInfo->iSocketType = SOCK_STREAM; // only socket type avail. for BT
                wsaQueryInfo->iProtocol = BTHPROTO_RFCOMM; // Normal bluetooth, not LE (for LE use BTHPROTO_L2CAP)
            // get host (our) name for next struct
            std::string lpzServiceInstanceNameLocal;
            checkSuccessWinsock<int>(getHostNameStr(&lpzServiceInstanceNameLocal), 0, "Getting Host Name Failed (shouldn't be fatal?)");
            // create struct used to register us as a BTH thingy
            bt::WSAQUERYSETA wsaQuery = {0};
                wsaQuery.dwSize = sizeof(bt::WSAQUERYSETA);
                wsaQuery.lpszServiceInstanceName = (bt::LPSTR)lpzServiceInstanceNameLocal.c_str();
                wsaQuery.lpServiceClassId = (bt::LPGUID)&MY_GUID; // p sure this one isn't ignored- how we're found + what we're being registered as.
                wsaQuery.lpszComment = (bt::LPSTR)L"Example Service instance registered in the directory service through RnR";
                wsaQuery.dwNameSpace = NS_BTH;
                wsaQuery.dwNumberOfProtocols = 0; // used for an optional arg (don't think it's applicable for BT)
                wsaQuery.dwNumberOfCsAddrs = 1; // IG always 1 for BT 🤷‍♂️
                wsaQuery.lpcsaBuffer = wsaQueryInfo;
            // function to actually register us
            checkSuccessWinsock<int>(bt::WSASetServiceA(&wsaQuery, bt::RNRSERVICE_REGISTER, 0), 0, "Failed to register port to UUID (SDP registering)");

            // ___set up port to listen for connection___
            checkSuccessWinsock<int>(bt::listen(this->listener, 8), 0, "Failed to set up port to listen"); // 8 reccomended for BTH

            // ___(OPTIONAL) set up port to be non-blocking, as this is a UI___
            // set non-blocking mode true
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->listener, FIONBIO, &mode), 0, "Failed to make listener non-blocking");

            /*can now accept(), but will be handled in another function.*/

            // ___free pointers we alloc()'d___
            free(wsaQueryInfo);

            makeDiscoverable(); // make ourselves discoverable!!!!! :DDDD
        }
        void updateConnections() {
            bt::SOCKET sock = bt::accept(this->listener, nullptr, nullptr); // get no data abt. connection
            EzText memAdd = EzText(raylib::Text("null"), RAYWHITE, 25.0_spX, 1.0);
            if (sock != INVALID_SOCKET) { 
                // _this will not get run often, as most of the time there will be nothing in the accept queue_
                // set non-blocking mode true
                bt::ULONG mode = 1;
                checkSuccessWinsock<int>(bt::ioctlsocket(sock, FIONBIO, &mode), 0, "Failed to make new connected port non-blocking");
                this->connections.push_back(sock);
                this->thingsToDrawList.push_back(memAdd);
                (*this->connListDrawable.getInternalVector()) = std::vector<Drawable*>();
                for (int i = 0; i < this->thingsToDrawList.size(); i++) {
                    this->connListDrawable.add(&(this->thingsToDrawList.at(i)));
                    std::cout << this->connListDrawable.getInternalVector()->at(i) << std::endl;
                    std::cout << &(this->thingsToDrawList.at(i)) << std::endl;
                }
            }
        }
        void handleRunningConnections() {
            if (this->runningBtTransactions.size() == 0) {
                return; // nothing to do
            }

            for(int i = 0; i < this->runningBtTransactions.size(); i++) {
                bt::READRES* curr = this->runningBtTransactions.at(i);
                if (curr->isReady()) {
                    handleDatabaseWrites(curr);
                    removeFromCurrentConnectionsVector(&this->runningBtTransactions, i);
                    free(curr); // kill the orphan
                }
            }
        }
        void handleDatabaseWrites(bt::READRES* connection) {
            switch(connection->transactionType) {
                case bt::TRANS_WRITE_MATCH:
                {
                    std::vector<char> readDataVec = connection->data.get();
                    if (connection->reportedSuccess) {
                        std::string data = std::string(readDataVec.begin(), readDataVec.end());
                        std::cerr << data << std::endl;
                    
                        // parse data and put it into database
                        JsonParser parser(data);
                        std::vector<MATCH_DATAPOINT> vectData = parser.parse();
                        DatabaseMan databaseCall(vectData);
                        databaseCall.maketh();
                    }
                }
                break;

                case bt::TRANS_WRITE_TABLET_INFO:
                {
                    std::cout << "WRITE_INFO" << std::endl;
                    std::vector<char> infoDataVec = connection->data.get();
                    std::cout << "WRITE_INFO #2: " << connection->reportedSuccess << std::endl;
                    if (connection->reportedSuccess) {
                        std::string data = std::string(infoDataVec.begin(), infoDataVec.end());
                        std::cerr << "Tablet info successfuly gotten, displaying now:" << std::endl;
                        std::cerr << data << std::endl;

                        int sockNumInVector = getElement(&(this->connections), connection->parentSocket);
                        std::cerr << this->thingsToDrawList.size() << " " << this->connListDrawable.getInternalVector()->size() << " " << this->connections.size() << std::endl;
                        std::cerr << &(this->thingsToDrawList.at(sockNumInVector)) << std::endl;
                        ((EzText*)(this->connListDrawable.getInternalVector()->at(sockNumInVector)))->setText(data);
                    }
                }
                break;

                default:
                    DebugConsole::print("ERROR: Something went seriously wrong. Tablet is probably screwed (no valid write transaction sent)\n", DBGC_RED);
            }
        }
        void handleReadyConnections() {
            if (this->connections.size() == 0) {
                // nothing to do lol
                return;
            }
            // __ Get all sockets that are ready to be read from ___
            // set disconnect time to 0ms (if sockets not already ready, continue.)
            bt::TIMEVAL disconnectTime = {0};
                disconnectTime.tv_sec = 0;
                disconnectTime.tv_usec = 0;
            bt::fd_set socketsToScan = {0};
                memset(&socketsToScan, 0, sizeof(bt::fd_set)); // not necessary prob.
                for (size_t i = 0; i < this->connections.size(); i++) {
                    socketsToScan.fd_array[i] = this->connections.at(i); //add all my sockets to  the arr
                }
                socketsToScan.fd_count = this->connections.size(); // set the size to # of sockets inputted
            // technically this returns the # of total sockets, but as we are only querying once, it should be fine.
            size_t sizeOfVals = bt::select(0, &socketsToScan, NULL, NULL, &disconnectTime); // first param ignored
            if (sizeOfVals == SOCKET_ERROR) {
                std::cerr << "Failed to select connections. Err code: " << std::to_string(bt::WSAGetLastError()) << std::endl;
                if (bt::WSAGetLastError() == 10038) {
                    disconnectAll();
                }
                return;
            }
            // ___ Read from all sockets connected ___
            for (size_t i = 0; i < sizeOfVals; i++) {
                std::cout << "#: " << std::to_string(i) << std::endl;
                bthSocketHandler handler(socketsToScan.fd_array[i]);
                handler.setLaunchPolicy(bt::CALLTYPE_ASYNCHRONOUS);

                bt::READRES* readResult = handler.readTabletData();
                std::cout << readResult->isReady() << std::endl;
                std::cout << readResult->transactionType << std::endl;

                if (!readResult->isReady()) {
                    // queue this socket for handling later
                    runningBtTransactions.push_back(handler.transferReadresOwnership());
                }
                switch(readResult->transactionType) {
                    case bt::TRANS_SOCKET_ERROR:
                    {
                        handler.sendNack();
                    }
                    // "fall through" to TRANS_CLOSE_SOCKET, as that's what we probably want to do now.

                    case bt::TRANS_CLOSE_SOCKET:
                    {
                        handler.closeSocket();
                        int socketToKill = getElement(&(this->connections), socketsToScan.fd_array[i]);
                        killConnectionAndDisplay(socketToKill);
                    }
                    break;

                    case bt::TRANS_WRITE_MATCH:
                    {
                        handleDatabaseWrites(readResult);
                    }
                    break;
                    case bt::TRANS_WRITE_TABLET_INFO:
                    {
                        handleDatabaseWrites(readResult);
                    }
                    break;

                    default:
                        handler.sendNack();
                }
            }
        }
        void disconnectAll() {
            for (int i = 0; i < this->connections.size(); i++) {
                checkSuccessWinsock<int>(bt::closesocket(this->connections.at(i)), 0, "failed to propely close socket (memory leak)");
            }
            this->connections = std::vector<bt::SOCKET>();
            (*(this->connListDrawable.getInternalVector())) = std::vector<Drawable*>();
            this->thingsToDrawList = std::vector<EzText>();
        }
};

#endif
