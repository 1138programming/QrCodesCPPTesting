#ifndef BT_TRANSACTIONTYPE_HPP
#define BT_TRANSACTIONTYPE_HPP

// from the POV of the tablet
typedef enum {
    BT_SOCKET_ERROR = -128,
    BT_CLOSE_SOCKET = 0,
    WRITE_MATCH = 1,
    WRITE_TABLET_INFO = 2
} BT_TRANSACTIONTYPE;

#endif