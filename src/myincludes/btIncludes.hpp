#ifndef BT_INCLUDES
#define BT_INCLUDES

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
    
    #include "bthSocketCallingType.hpp"
    #include "bluetoothTransactionType.hpp"
    #include "bluetooth/bluetoothTransactionType.hpp"
    // #include "debugConsole.hpp"

    #define BT_EXPECTED_DATA_INITIAL (sizeof(char))
    #define BT_EXPECTED_DATA_READSIZE (sizeof(int))

    #define BT_TAB_ACK_SIZE (sizeof(char) * 3)
    #define BT_TAB_NACK_SIZE (sizeof(char) * 3)

    //(https://cs.android.com/android/platform/superproject/main/+/main:packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothSocket.java;l=1310;drc=fa6448d21951a20245782a33efc67740dd09b244;bpv=1;bpt=1) sendSocketAcceptSignal() method
    static int ANDROID_SOCK_ACCEPT_SIGNAL_SIZE = (sizeof(char) * 4);

}

//includes on timeout >:C
#include "btReadResult.hpp"
#include "winsockErrorDesc.hpp"

template<typename T> void checkSuccessWinsock(T val, T target, std::string errorMessage) {
    if (val != target) {
        int errorCode = bt::WSAGetLastError();
        std::cout << std::string("ERROR: ") + errorMessage + std::string(" Reason:") << std::endl;
        // bt::DebugConsole::println(std::string("ERROR: ") + errorMesage + std::string(" Reason:"), DBGC_RED);
        std::cout << std::to_string(errorCode) + std::string(" (") + WinsockErrorDesc::get(errorCode).errorName + std::string(")") << std::endl;
        // bt::DebugConsole::println(std::to_string(errorCode) + std::string(" (") + WinsockErrorDesc::get(errorCode).errorName + std::string(")"), DBGC_RED);
    }
}

#undef MAKEWORD
#define MAKEWORD(a,b) ((bt::WORD) (((bt::BYTE) (((bt::DWORD_PTR) (a)) & 0xff)) | ((bt::WORD) ((bt::BYTE) (((bt::DWORD_PTR) (b)) & 0xff))) << 8))
#undef BT_PORT_ANY
#define BT_PORT_ANY ((bt::ULONG)-1)
#undef FIONBIO
#define FIONBIO _IOW('f', 126, bt::u_long)
#undef INVALID_SOCKET
#define INVALID_SOCKET (bt::SOCKET)(~0)
#undef STD_OUTPUT_HANDLE
#define STD_OUTPUT_HANDLE ((bt::DWORD)-11)

#endif