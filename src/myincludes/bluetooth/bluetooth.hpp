#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

#include "../btIncludes.hpp"
#include "../bthSocketHandler.hpp"
#include "../verticalScrollable.hpp"
#include "../debugConsole.hpp"
#include "../winsockErrorDesc.hpp"

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

        bt::BLUETOOTH_ADDRESS_STRUCT localAddr;
        unsigned int port;

        bt::HANDLE btRadio; // init/shutdown vars
        bt::HBLUETOOTH_RADIO_FIND btRadioFindingVal;
    public:
        /*********************************************/
        /* HOST/CLIENT DATA HANDLING FUNCTIONS */
        /*********************************************/

        /**
         * @returns The given mac address in string form (xx:xx:xx:xx:xx:xx) (xx in hex)
         */
        std::string getMacStr(bt::BLUETOOTH_ADDRESS_STRUCT addr) {
            std::ostringstream addrBuilder;

            addrBuilder << std::hex;
            for (int i = 5; i > 0; i--) { // mac is reversed in storage
                if (addr.rgBytes[i] <= 0x0f) {
                    addrBuilder << 0; // if less than 0x0f, hex only outputs 1 num, but we need 2 for all.
                }
                addrBuilder << (int)addr.rgBytes[i] << ":";
            }
            if (addr.rgBytes[i] <= 0x0f) {
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
            if (bt::gethostname(hostName, 256) != 0) {
                str = "";
                return -2;
            }

            (*str) = std::str(name);
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
            if (this->btFindingVal == NULL) {
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
            bt::BluetoothFindRadioClose(this->btFindingVal);
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
            // handles are freed in destructor- they are used later.
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
                listenerAddr.bthAddr = 0; // if 0, it's local MAC addr. Unsure if you can actually change this
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
            checkSuccesWinsock<int>(getHostNameStr(lpszServiceInstanceNameLocal), 0, "Getting host name failed (this shouldn't matter too much)");

            // this structure is needed for the next structure created
            bt::CSADDR_INFO sdpSockInfo = {0};
                sdpSockInfo.LocalAddr.iSockaddrLength = sizeof(bt::SOCKADDR_BTH);
                sdpSockInfo.LocalAddr.lpSockaddr = (bt::LPSOCKADDR)&localSocketName;
                sdpSockInfo.iSocketType = SOCK_STREAM; // BT is a streaming protocol
                sdpSockIngo.iProtocol = BTHPROTO_RFCOMM; // bluetooth classic (not LE)

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
            checkSuccesWinsock<int>(bt::listen(this->listener, 7), 0, "Failed to set socket in listening mode."); // I use 7, as that is the max number of bluetooth connections (there is no possible way we are getting that many in a couple milliseconds/a frame)

            // ___(Technically optional) set up port to be non-blocking, as we are using this w/ a UI___
            // set non blocking mode true
            bt::ULONG mode = 1;
            checkSuccessWinsock<int>(bt::ioctlsocket(this->listener, FIONBIO, &mode), 0, "Failed to set socket to non-blocking mode.")

            // we can now accept() connections!!!! :DDDDD (we will handle this in a separate function)
            makeDiscoverable(); // yipee!
        }


        /*********************************************/
        /* PER-FRAME FUNCTIONS */
        /*********************************************/
        /**
         * @brief accepts the incoming connections for this frame. This call is non-blocking, so should be called at least once every frame (thought there is no point in doing it multiple times)
         */
        void acceptConn() {
            bt::SOCKADDR_BTH peerAddr = {0};
                int sizeOfPeerAddr = sizeof(peerAddr);
            bt::SOCKET curr = bt::accept(this->listener, (bt::sockaddr*)&peerAddr, sizeOfPeeraddr);

            if (curr != INVALID_SOCKET) {
                // _this will not get run often, as most of the time there will be nothing in the accept queue_
                // set non-blocking mode true
                bt::ULONG mode = 1;
                checkSuccessWinsock<int>(bt::ioctlsocket(sock, FIONBIO, &mode), 0, "Failed to make new connected port non-blocking.");
                
                // attempt to get/print name:
                bt::BLUETOOTH_DEVICE_INFO_STRUCT deviceInfo = {0};

                bt::BluetoothGetDeviceInfo(this->radio, &deviceInfo);

                std::cout << "Connection attempt name: " << deviceInfo.szName << std::endl;

            }

            ~Bluetooth() {
                freeRadioHandles();
            }
        }
};

#endif
