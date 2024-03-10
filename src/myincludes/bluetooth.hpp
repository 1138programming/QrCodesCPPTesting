#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

namespace bt {
    #include <winsock2.h>
    #include <windows.h>

    #include <WS2tcpip.h>
    #include <ws2ipdef.h>
    #include <ws2tcpip.h>
    #include <WinSock2.h>
    #include <MSWSock.h>
    #include <bluetoothapis.h>
    #include <ws2bth.h>
    #include <synchapi.h>
}

#include <vector>
#include <iostream>
#include <stdio.h>

#undef MAKEWORD
#define MAKEWORD(a,b) ((bt::WORD) (((bt::BYTE) (((bt::DWORD_PTR) (a)) & 0xff)) | ((bt::WORD) ((bt::BYTE) (((bt::DWORD_PTR) (b)) & 0xff))) << 8))
#undef BT_PORT_ANY
#define BT_PORT_ANY ((bt::ULONG)-1)

static const bt::GUID MY_GUID = {0x00001101, 0x0000, 0x1000, {0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}};

class Bluetooth {
    private:
        bt::SOCKET listener;
        bt::SOCKET connection;
        bt::BLUETOOTH_ADDRESS externalAddress;
    public:
        int initAll() {
            bt::WSADATA wsaData;
            return bt::WSAStartup(MAKEWORD(2, 2), &wsaData);
        }
        int cleanup() {
            return bt::WSACleanup();
        }
        int startAccept() {
            // initialize a windows socket in bluetooth mode
            this->listener = bt::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

            bt::SOCKADDR_BTH listenerAddr;
            memset(&listenerAddr, 0, sizeof(listenerAddr));
                listenerAddr.addressFamily = AF_BTH;
                // if 0 = random port
                // setting this to 0 = work, but not great for what we want to do.
                listenerAddr.btAddr = 0;
                listenerAddr.port = BT_PORT_ANY;
                listenerAddr.serviceClassId = MY_GUID;

            std::cout << "ckpt0" << std::endl;

            int success = bt::bind(this->listener, reinterpret_cast<bt::sockaddr*>(&listenerAddr), sizeof(listenerAddr));
            if (success != 0) {
                std::cerr << "binding to port failed" << std::endl;
                return bt::WSAGetLastError();
            }

            bt::BLUETOOTH_ADDRESS nameLol;

            size_t ushortSize = sizeof(bt::USHORT);
            size_t byteSize = sizeof(bt::BYTE);
            size_t whatINeedLol = 6;
            for (int i = 0; i < 6; i++) {
                nameLol.rgBytes[i] = 0;
            }

            std::cout << "ckpt1" << std::endl;

            success = bt::listen(listener, 8);
            if (success != 0) {
                std::cerr << "listen on port failed" << std::endl;
                return bt::WSAGetLastError();
            }

            std::cout << "ckpt2" << std::endl;

            size_t addrSize = sizeof(this->externalAddress);
            // blocking function lol- accepts connection, but doesn't get any data about the devices
            this->connection = bt::accept(this->listener, nullptr, nullptr);
            if (this->connection != 0) {
                std::cerr << "failed to accept" << std::endl;
                return bt::WSAGetLastError();
            }

            std::cout << "ckpt3" << std::endl;

            return 0;
        }

};

#endif



/*
static const GUID MY_GUID = {0x00001101, 0x0000, 0x1000, {0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}};

class DevicesManagerHostTwo {
    private:
    // vars
        SOCKET listener;
        SOCKET connection;
        BLUETOOTH_ADDRESS externalAddress;
    // consts
    public:
        static int handler() {
            std::cout << "first try 😎" << std::endl;
            return 0;
        }
        int initAll() {
            // Start Winsock on Windows
            WSADATA wsaData;
            return WSAStartup(MAKEWORD(2, 2), &wsaData); // MAKEWORD(2, 2) for Winsock 2.2
        }
        int cleanup() {
            return WSACleanup();
        }
        int startAccept() {
            // initialize a windows socket in bluetooth mode
            listener = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

            // create a BluetoothAddress to add to the SOCKADDR_BTH (THIS MAY BE WRONG)
            // please make sure to note that the bytes are little-endian (ask Jonathan)
            BLUETOOTH_ADDRESS addr;
                // addr.rgBytes[0] = 0x9C;
                // addr.rgBytes[1] = 0x90;
                // addr.rgBytes[2] = 0x90;
                // addr.rgBytes[3] = 0X8A;
                // addr.rgBytes[4] = 0x4F;
                // addr.rgBytes[5] = 0x14;
                // addr.ullLong = 280;

            //14:4F:8A:90:90:9C

            // create a bluetooth socket
            SOCKADDR_BTH listenerAddr;
            memset(&listenerAddr, 0, sizeof(listenerAddr));
                listenerAddr.addressFamily = AF_BTH;
                // if 0 = random port
                // setting this to 0 = work, but not great for what we want to do.
                listenerAddr.btAddr = 0;
                listenerAddr.port = BT_PORT_ANY;
                listenerAddr.serviceClassId = MY_GUID;

            std::cout << "ckpt0" << std::endl;
            
            // bind to the Bluetooth Socket
            // success checks if there is an error value...
            int success = bind(listener, reinterpret_cast<sockaddr*>(&listenerAddr), sizeof(listenerAddr));
            if (success != 0) {
                return WSAGetLastError();
            }            

            // get socket data
            BLUETOOTH_ADDRESS nameLol;

            int ushortSize = sizeof(USHORT);
            int byteSize = sizeof(BYTE);
            int whatINeedLol = 6;
            for (int i = 0; i < 6; i++) {
                nameLol.rgBytes[i] = 0;
            }

            std::cout << "ckpt-1" << std::endl;

            // success = getsockname(listener, reinterpret_cast<sockaddr*>(&nameLol), &whatINeedLol);
            // if (success != 0) {
            //     return WSAGetLastError();
            // }
            // std::cout << "Listening on socket: ";
            // for (int i = 0; i < 16; i++) {
            //     std::cout << std::hex << ((int)nameLol.rgBytes[i]) + ":";
            // }
            // std::cout << std::dec << std::endl;
            
            // all "ckpt" std::couts are just for debugging
            std::cout << "ckpt1" << std::endl;

            // set listener up for connection(?)
            success = listen(listener, 8);
            if (success != 0) {
                return WSAGetLastError();
            }
            
            std::cout << "ckpt2" << std::endl;



            int addrSize = sizeof(externalAddress);
            // blocking function lol- accepts connection, but doesn't get any data about the devices
            connection = accept(listener, nullptr, nullptr);
            if (connection != 0) {
                return WSAGetLastError();
            }

            std::cout << "ckpt3" << std::endl;

            // print address if successfully connected
            // for (int i = 0; i < 14; i++) {
            //     std::cout << externalAddress.sa_data[i];
            // }
            // std::cout << std::endl;
            return 0;
        }
};

#endif
*/