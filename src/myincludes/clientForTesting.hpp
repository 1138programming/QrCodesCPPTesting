#ifndef BTCLIENTTEST_HPP
#define BTCLIENTTEST_HPP

#include "bluetooth.hpp"

//{<short id>-0000-1000-8000-00805F9B34FB}
static const bt::GUID CLIENT_UUID = {0x0000, 0x1000, 0x8000, {0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb}};

class Client {
    public:
        int connectToServer() {
            bt::SOCKET sock;
            sock = bt::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
            if (sock == ((bt::SOCKET)~0)) {
                std::cerr << "bad1" << std::endl;
                return bt::WSAGetLastError();
            }


            int nonBlockRet = 1;
            bt::ULONG mode = 1;
            nonBlockRet = bt::ioctlsocket(sock, (0x80000000|(((long)sizeof(bt::u_long)&0x7f)<<16)|(('f')<<8)|(126)), &mode);

            if (nonBlockRet == SOCKET_ERROR) {
                std::cerr << "bad1.5" << std::endl;
                return bt::WSAGetLastError();
            }

            //B9:BB:70:1D:A5:10
            bt::_BLUETOOTH_ADDRESS btAddr;
                btAddr.rgBytes[5] = 0xb9;
                btAddr.rgBytes[4] = 0xbb;
                btAddr.rgBytes[3] = 0x70;
                btAddr.rgBytes[2] = 0x1d;
                btAddr.rgBytes[1] = 0xa5;
                btAddr.rgBytes[0] = 0x10;

            bt::SOCKADDR_BTH addr;
                addr.addressFamily = AF_BTH;
                addr.btAddr = btAddr.ullLong;
                addr.serviceClassId = CLIENT_UUID;  
                addr.port = 4;

            if (bt::connect(sock, reinterpret_cast<bt::sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
                std::cerr << "bad2" << std::endl;
                return bt::WSAGetLastError();
            }
            return 0;
        }
};

#endif