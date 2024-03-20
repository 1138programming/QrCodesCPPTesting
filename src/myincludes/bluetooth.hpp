#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

#include "btIncludes.hpp"
#include "bthAppCxnHandler.hpp"

#include <vector>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdint.h>

static const bt::GUID MY_GUID = {0x00001101, 0x0000, 0x1000, {0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}};

class Bluetooth {
    private:
        bt::SOCKET listener;
        std::vector<bt::SOCKET> connections;
        bt::BLUETOOTH_ADDRESS externalAddress;
        bt::BTH_ADDR localAddr;
        uint8_t port;

        // ___ Useful (private) functions ___
    public:
        // ___ Simple BT functions ___
        int initAll() {
            bt::WSADATA wsaData;
            return bt::WSAStartup(MAKEWORD(2, 2), &wsaData);
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
            return macBuilder.str();
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

        // ___ Connection functions ___
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
            checkSuccessWinsock<int>(bt::WSASetServiceA(&wsaQuery, bt::RNRSERVICE_REGISTER, 0), 0, "Failed to register port to UUID");

            // ___set up port to listen for connection___
            checkSuccessWinsock<int>(bt::listen(this->listener, 8), 0, "Failed to set up port to listen"); // 8 reccomended for BTH

            // ___(OPTIONAL) set up port to be non-blocking, as this is a UI___
            // set non-blocking mode true
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->listener, FIONBIO, &mode), 0, "Failed to make listener non-blocking");

            /*can now accept(), but will be handled in another function.*/

            // ___free pointers we alloc()'d___
            free(wsaQueryInfo);
        }
        void updateConnections() {
            bt::SOCKET sock = bt::accept(this->listener, nullptr, nullptr); // get no data abt. connection
            if (sock != INVALID_SOCKET) { 
                // _this will not get run often, as most of the time there will be nothing in the accept queue_
                // set non-blocking mode true
                bt::ULONG mode = 1;
                checkSuccessWinsock<int>(bt::ioctlsocket(sock, FIONBIO, &mode), 0, "Failed to make new connected port non-blocking");
                this->connections.push_back(sock);
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
                return;
            }
            // ___ Read from all sockets connected ___
            for (size_t i = 0; i < sizeOfVals; i++) {
                std::cout << "#: " << std::to_string(i) << std::endl;
                BthCxnHandler handler(socketsToScan.fd_array[i]);
                BT_TRANSACTIONTYPE transaction = handler.getTransactionType();
                std::cout << transaction << std::endl;
                switch(transaction) {
                    case BT_SOCKET_ERROR:
                    {
                        handler.handleSocketError();
                    }
                    break;

                    case BT_CLOSE_SOCKET:
                    {
                        handler.closeSocket();
                        std::vector<bt::SOCKET>::iterator it;
                        it = find(this->connections.begin(), this->connections.end(), this->connections.at(i));
                        this->connections.erase(it);
                    }
                    break;

                    case WRITE_MATCH:
                    {
                        bool success;
                        std::string data = handler.readMatchFromTablet(&success);
                        
                        if (success) {
                            // parse data and put it into database
                            JsonParser parser(data);
                            std::vector<MATCH_DATAPOINT> vectData = parser.parse();
                            DatabaseMan databaseCall(vectData);
                            databaseCall.maketh();
                        }
                    }
                    break;

                    default:
                        handler.handleSocketError();
                }
            }
        }
};

#endif