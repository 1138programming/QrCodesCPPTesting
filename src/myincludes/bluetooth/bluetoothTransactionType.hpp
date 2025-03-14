#ifndef BT_TRANSACTIONTYPE_HPP
#define BT_TRANSACTIONTYPE_HPP

/** 
 * @brief from our POV 
 * @warning MAKE SURE IF ADDING TO THIS WRITE IS NEGATIVE AND RECV IS POSITIVE
*/
typedef enum : char {
    TRANS_SOCKET_ERROR = -128,
    
    // send functions
    TRANS_SEND_LOCAL_DB = -2,
    TRANS_SEND_LOCAL_DB_HASH = -1,

    TRANS_CLOSE_SOCKET = 0,
    
    // recv functions
    TRANS_RECV_MATCH = 1,
    TRANS_RECV_TABLET_INFO = 2
} TRANSACTIONTYPE;

#endif