#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

#include "../btIncludes.hpp"
#include "../bthSocketHandler.hpp"
#include "../verticalScrollable.hpp"
#include "../debugConsole.hpp"
#include "../winsockErrorDesc.hpp"
#include "../verticalScrollable.hpp"
#include "../ezText.hpp"
#include "btTabObj.hpp"
#include "bluetoothConductor.hpp"

#include <algorithm>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdint.h>

static const bt::GUID MY_GUID = {0x0007EA11, 0x1138, 0x1000, {0x54, 0x65, 0x61, 0x6D, 0x31, 0x31, 0x33, 0x38}};

class Bluetooth {
    private:
        bt::SOCKET listener;
        std::vector<BtTabObj> connectedTablets;
        std::vector<bt::TABTRANSACTION*> runningTransactions;
        BluetoothConductor conductor;

        bt::BLUETOOTH_ADDRESS localAddr;
        unsigned int port;

        bt::HANDLE btRadio; // init/shutdown vars
        bt::HBLUETOOTH_RADIO_FIND btRadioFindingVal;

        VerticalScrollable nameList = VerticalScrollable(600.0_spX, 200.0_spY, WHITE, 3.0);
        std::vector<EzText> names;
    public:
        /*********************************************/
        /* HOST/CLIENT DATA HANDLING FUNCTIONS */
        /*********************************************/

        /**
         * @returns The given mac address in string form (xx:xx:xx:xx:xx:xx) (xx in hex)
         */
        std::string getMacStr(bt::BLUETOOTH_ADDRESS addr) {
            std::ostringstream addrBuilder;

            addrBuilder << std::hex;
            for (int i = 5; i > 0; i--) { // mac is reversed in storage
                if (addr.rgBytes[i] <= 0x0f) {
                    addrBuilder << 0; // if less than 0x0f, hex only outputs 1 num, but we need 2 for all.
                }
                addrBuilder << (int)addr.rgBytes[i] << ":";
            }
            if (addr.rgBytes[0] <= 0x0f) {
                addrBuilder << 0;
            }
            addrBuilder << (int)addr.rgBytes[0]; // outside of loop so we don't have a hanging ":"
            
            std::string macAddr = addrBuilder.str();
            for (int i = 0; i < macAddr.length(); i++) {
                macAddr[i] = std::toupper(macAddr[i]);
            }
            return macAddr;
        }
        /**
         * @brief Returns the winsock host name (probably the computer's name). On failure, will return ""
         * @returns 0 for success, -1 or -2 for failure
         * @warning ### OVERRIDES str. DO NOT USE A STR YOU CARE ABOUT
         */
        int getHostNameStr(std::string& str) {
            char* name = (char*)calloc(256, sizeof(char)); // I would rather allocate on heap
            if (name == NULL) {
                str = "";
                return -1;
            }
            if (bt::gethostname(name, 256) != 0) {
                str = "";
                return -2;
            }

            str = std::string(name);
            free(name);
            return 0;
        }
        

        /*********************************************/
        /* INITIALIZING / FREEING FUNCTIONS */
        /*********************************************/

        /**
         * @brief Initializes Winsock. If you are using a library that already does this, there is no real reason to do it again.
         * @returns The return value of the startup. If it != 0, there was a problem (will be logged)
        */
        int initWinsock() {
            bt::WSADATA wsaData;
            int startupRet = bt::WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (startupRet == 0) {
                DebugConsole::println("Winsock/Bluetooth Successfully Initialized!", DBGC_GREEN);
            }
            else {
                DebugConsole::println("Winsock/Bluetooth Failed to Init", DBGC_RED);
            }
            return startupRet;
        }
        /**
         * @brief de-inits winsock- this should be called before the end of the program. Only call once per init.
         * @returns cleanup val. Should be 0
         */
        int cleanupWinsock() {
            return bt::WSACleanup();
        }

        /**
         * @brief inits the radio handles for bluetooth. Will get first handle
         * 
         * If you don't know what this does, you are probably looking for the makeDiscoverable() function (it is called in initAccept()).
         */
        void initRadioHandles() {
            bt::BLUETOOTH_FIND_RADIO_PARAMS radioParams;
                radioParams.dwSize = sizeof(bt::BLUETOOTH_FIND_RADIO_PARAMS);

            this->btRadioFindingVal = bt::BluetoothFindFirstRadio(&radioParams, &this->btRadio);
            if (this->btRadioFindingVal == NULL) {
                DebugConsole::print("No valid bluetooth radio found!", DBGC_RED);
            }
        }
        /**
         * @brief frees the radio handles for bluetooth
         * 
         * If you don't know what this does, you are probably looking for the makeDiscoverable() function (it is called in initAccept()).
         */
        void freeRadioHandles() {
            bt::CloseHandle(this->btRadio);
            bt::BluetoothFindRadioClose(this->btRadioFindingVal);
        }

        /**
         * @brief Makes this computer discoverable & allows connections to this computer. Needed in order for the tablets to be able to scan this computer's SDP server. (It's important)
         */
        void makeDiscoverable() {
            initRadioHandles();

            if (bt::BluetoothEnableDiscovery(this->btRadio, true)) {
                std::string hostName;
                    getHostNameStr(hostName);
                DebugConsole::println(std::string("Computer Discoverable- Host Name: ") + hostName, DBGC_GREEN);
            }
            else {
                DebugConsole::println("Failed to Enable Discoverability", DBGC_RED);
            }
            freeRadioHandles();
        }

        /**
         * @brief initializes a socket & SDP server for this computer to be connected to
         */
        void initAccept() {
            // init a windows socket in bluetooth (classic) mode
            this->listener = bt::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
            if (this->listener == INVALID_SOCKET) {
                int error = bt::WSAGetLastError();
                DebugConsole::print("Failed to create socket in RFCOMM mode. Error: ", DBGC_RED);
                DebugConsole::println(std::to_string(error) + std::string(" (") + WinsockErrorDesc::get(error).errorName + std::string(")"), DBGC_RED);
            }

            // ___binding addr___
            // create ADDR for BT socket to bind to
            bt::SOCKADDR_BTH listenerAddr = {0};
                listenerAddr.addressFamily = AF_BTH; // address family for bluetooth
                listenerAddr.btAddr = 0; // if 0, it's local MAC addr. Unsure if you can actually change this
                listenerAddr.port = BT_PORT_ANY; // let windows decide :D
                listenerAddr.serviceClassId = MY_GUID; // will be ignored, why not set it
            // bind addr to sock we created earlier
            checkSuccessWinsock<int>(bt::bind(this->listener, (bt::sockaddr*)(&listenerAddr), sizeof(bt::SOCKADDR_BTH)), 0, "Failed to bind socket to port/address.");

            // ___ get socket data windows gave us___
            bt::SOCKADDR_BTH localSocketName = {0};
            int socketNameLen = sizeof(bt::SOCKADDR_BTH); // windows uses the size of the struct to determine it's version/what addres family if I understand correctly
            checkSuccessWinsock<int>(bt::getsockname(this->listener, (bt::sockaddr*)&localSocketName, &socketNameLen), 0, "Failed to get windows-given socket info.");
            // set in-class vars
            this->localAddr.ullLong = localSocketName.btAddr;
            this->port = (unsigned int)localSocketName.port; 

            //___code to create/advertise SDP server___
            // In short, an SDP server is a server that BT can scan & read w/o actually connecting to the device.
            // It includes, among other things, the port that our service is running on, allowing other devices to connect w/o already knowing it.
            // I will try my best to explain these things line-by-line, though some of the things may be past me.
            
            std::string lpszServiceInstanceNameLocal; // get the host name to set as our instance name. I'm like 90% sure windows will override if it is not set to this anyway.
            checkSuccessWinsock<int>(getHostNameStr(lpszServiceInstanceNameLocal), 0, "Getting host name failed (this shouldn't matter too much)");
            DebugConsole::println(std::string("Using Hostname: ") + lpszServiceInstanceNameLocal, DBGC_DEFAULT);

            // this structure is needed for the next structure created
            bt::CSADDR_INFO sdpSockInfo = {0};
                sdpSockInfo.LocalAddr.iSockaddrLength = sizeof(bt::SOCKADDR_BTH);
                sdpSockInfo.LocalAddr.lpSockaddr = (bt::LPSOCKADDR)&localSocketName;
                sdpSockInfo.RemoteAddr.iSockaddrLength = sizeof(bt::SOCKADDR_BTH);
                sdpSockInfo.RemoteAddr.lpSockaddr = (bt::LPSOCKADDR)&localSocketName;
                sdpSockInfo.iSocketType = SOCK_STREAM; // BT is a streaming protocol
                sdpSockInfo.iProtocol = BTHPROTO_RFCOMM; // bluetooth classic (not LE)

            bt::WSAQUERYSETA sdpRegistration = {0};
                sdpRegistration.dwSize = sizeof(bt::WSAQUERYSETA); // for versioning
                sdpRegistration.lpszServiceInstanceName = (bt::LPSTR)lpszServiceInstanceNameLocal.c_str(); // name of service (not super important for our use-case)
                sdpRegistration.lpServiceClassId = (bt::LPGUID)&MY_GUID; // register a UUID for our service so the tablets can find it
                sdpRegistration.lpszComment = (bt::LPSTR)L"Registration for scouting app";
                sdpRegistration.dwNameSpace = NS_BTH; // obv.
                sdpRegistration.lpNSProviderId = NULL; // this doesn't need to be set if dwNameSpace is
                sdpRegistration.dwNumberOfProtocols = 0; // ignored
                sdpRegistration.dwNumberOfCsAddrs = 1; // we only want to register 1 service
                sdpRegistration.lpcsaBuffer = (bt::LPCSADDR_INFO)&sdpSockInfo; // set the socket's info
            checkSuccessWinsock<int>(bt::WSASetServiceA(&sdpRegistration, bt::RNRSERVICE_REGISTER, 0), 0, "Failed to register SDP server (cooked).");

            //___set port to listen for connections___
            checkSuccessWinsock<int>(bt::listen(this->listener, 7), 0, "Failed to set socket in listening mode."); // I use 7, as that is the max number of bluetooth connections (there is no possible way we are getting that many in a couple milliseconds/a frame)

            // ___(Technically optional) set up port to be non-blocking, as we are using this w/ a UI___
            // set non blocking mode true
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->listener, FIONBIO, &mode), 0, "Failed to set socket to non-blocking mode.");

            // we can now accept() connections!!!! :DDDDD (we will handle this in a separate function)
            makeDiscoverable(); // yipee!
        }


        /*********************************************/
        /* PER-FRAME FUNCTIONS */
        /*********************************************/
        /**
         * @brief accepts the incoming connections for this frame. This call is non-blocking, so should be called at least once every frame (though there is no point in doing it multiple times)
         */
        void acceptConn() {
            bt::SOCKADDR_BTH peerAddr = {0};
                int sizeOfPeerAddr = sizeof(peerAddr);
            bt::SOCKET peer = bt::accept(this->listener, (bt::sockaddr*)&peerAddr, &sizeOfPeerAddr);

            if (peer != INVALID_SOCKET) {
                // _this will not get run often, as most of the time there will be nothing in the accept queue_
                // set non-blocking mode true
                bt::ULONG mode = 1;
                checkSuccessWinsock<int>(bt::ioctlsocket(peer, FIONBIO, &mode), 0, "Failed to make new connected port non-blocking.");

                // add the socket to our tablet list
                bt::BLUETOOTH_ADDRESS addrStruct = {0};
                    addrStruct.ullLong = peerAddr.btAddr;
                this->connectedTablets.push_back(BtTabObj(peer, peerAddr, getMacStr(addrStruct)));
            }
        }
        void handleReadyConnections() {
            if (this->connectedTablets.size() == 0) {
                // nothing to do lol
                return;
            }

            for (int i = 0; i < this->connectedTablets.size(); i++) {
                BtTabObj* tabCurr = &this->connectedTablets.at(i);
                if (!tabCurr->readyToRead() || tabCurr->undergoingTransaction()) {
                    continue;
                }
                else {
                    DebugConsole::println(std::string("Tablet #") + std::to_string(i) + std::string(" transaction:"), DBGL_DEVEL);
                    bt::TABTRANSACTION* currTrans = this->conductor.initReadyTransaction(tabCurr->readTransactionData());
                    if (currTrans->batmanTrans) {
                        killSocket(currTrans->parent->getWinsockSocket());
                    }
                    else if (currTrans->isReady()) {
                        this->conductor.handleTransResult(currTrans);
                    }
                    else {
                        runningTransactions.push_back(currTrans);
                        currTrans->parent->setTransactionState(true);
                    }
                }
            }
        }
        void handleRunningTransactions() {
            if (this->runningTransactions.size() == 0) {
                return; // nothing to do lol
            }

            for (int i = 0; i < this->runningTransactions.size(); i++) {
                if (this->runningTransactions.at(i)->isReady()) {
                    this->conductor.handleTransResult(this->runningTransactions.at(i));
                    this->runningTransactions.at(i)->parent->setTransactionState(false);
                    this->runningTransactions.erase(this->runningTransactions.begin() + i);
                }
            }
        }
        void updateAllBt() {
            acceptConn();
            handleReadyConnections();
            handleRunningTransactions();
        }

        VerticalScrollable* getNameList() {
            std::vector<Drawable*>* listVector = this->nameList.getInternalVector();
            listVector->clear();
            this->names.clear();
            for (int i = 0; i < this->connectedTablets.size(); i++) {
                this->names.push_back(EzText(raylib::Text(this->connectedTablets.at(i).getScoutingName()), RAYWHITE, 25.0_spX, 1.0));
                this->names.at(i).setCustomOffset(raylib::Vector2(2.0, 0.0));
                listVector->push_back(&this->names.at(i));
            }
            return &this->nameList;
        }

        /*********************************************/
        /* GETTER/SETTER-TYPE FUCNTIONS */
        /*********************************************/
        /**
         * @returns whether a socket matching the conditions was successfully erased
         */
        bool killSocket(bt::SOCKET deleteSock) {
            for (int i = 0; i < this->connectedTablets.size(); i++) {
                bt::SOCKET currSock = this->connectedTablets.at(i).getWinsockSocket();
                if (currSock == deleteSock) {
                    this->connectedTablets.at(i).sockSuicide();
                    this->connectedTablets.erase(this->connectedTablets.begin() + i);
                    return true;
                }
            }
            return false;
        }
        void killAllSockets() {
            for (int i = 0; i < this->connectedTablets.size(); i++) {
                this->connectedTablets.at(i).sockSuicide();
            }
            this->connectedTablets.clear();
        }

        std::string getLocalMacStr() {
            return getMacStr(this->localAddr);
        }
        int getLocalPort() {
            return this->port;
        }
        int getNumConnections() {
            return this->connectedTablets.size();
        }
};

#endif
