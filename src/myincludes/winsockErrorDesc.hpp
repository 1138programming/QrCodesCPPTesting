#ifndef WINSOCKERRORDESC_HPP
#define WINSOCKERRORDESC_HPP

#include <unordered_map>
#include "winsockErrorDescStruct.hpp"

class winsockErrorDesc {
    private:
        static void initVector() {
            errorCodes.clear();
            
            errorCodes.insert({6, (WINSOCKERRORDESCSTRUCT){"WSA_INVALID_HANDLE", "Specified event object handle is invalid.", "An application attempts to use an event object, but the specified handle is not valid."}});
            errorCodes.insert({8, (WINSOCKERRORDESCSTRUCT){"WSA_NOT_ENOUGH_MEMORY", "Insufficient memory available.", "An application used a Windows Sockets function that directly maps to a Windows function. The Windows function is indicating a lack of required memory resources."}});
            errorCodes.insert({87, (WINSOCKERRORDESCSTRUCT){"WSA_INVALID_PARAMETER", "One or more parameters are invalid.", "An application used a Windows Sockets function which directly maps to a Windows function. The Windows function is indicating a problem with one or more parameters."}});
            errorCodes.insert({995, (WINSOCKERRORDESCSTRUCT){"WSA_OPERATION_ABORTED", "Overlapped operation aborted.", "An overlapped operation was canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl."}});
            errorCodes.insert({996, (WINSOCKERRORDESCSTRUCT){"WSA_IO_INCOMPLETE", "Overlapped I/O event object not in signaled state.", "The application has tried to determine the status of an overlapped operation which is not yet completed. Applications that use WSAGetOverlappedResult (with the fWait flag set to FALSE) in a polling mode to determine when an overlapped operation has completed, get this error code until the operation is complete."}});
            errorCodes.insert({997, (WINSOCKERRORDESCSTRUCT){"WSA_IO_PENDING", "Overlapped operations will complete later.", "The application has initiated an overlapped operation that cannot be completed immediately. A completion indication will be given later when the operation has been completed."}});
            errorCodes.insert({10004, (WINSOCKERRORDESCSTRUCT){"WSAEINTR", "Interrupted function call.", "A blocking operation was interrupted by a call to WSACancelBlockingCall."}});
            errorCodes.insert({10009, (WINSOCKERRORDESCSTRUCT){"WSAEBADF", "File handle is not valid.", "The file handle supplied is not valid."}});
            errorCodes.insert({10013, (WINSOCKERRORDESCSTRUCT){"WSAEACCES", "Permission denied.", "An attempt was made to access a socket in a way forbidden by its access permissions. An example is using a broadcast address for sendto without broadcast permission being set using setsockopt(SO_BROADCAST).\nAnother possible reason for the WSAEACCES error is that when the bind function is called (on Windows NT 4.0 with SP4 and later), another application, service, or kernel mode driver is bound to the same address with exclusive access. Such exclusive access is a new feature of Windows NT 4.0 with SP4 and later, and is implemented by using the SO_EXCLUSIVEADDRUSE option."}});
            errorCodes.insert({10014, (WINSOCKERRORDESCSTRUCT){"WSAEFAULT", "Bad address.", "The system detected an invalid pointer address in attempting to use a pointer argument of a call. This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small. For instance, if the length of an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr)."}});
            errorCodes.insert({10022, (WINSOCKERRORDESCSTRUCT){"WSAEINVAL", "Invalid argument.", "Some invalid argument was supplied (for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socket—for instance, calling accept on a socket that is not listening."}});
            errorCodes.insert({10024, (WINSOCKERRORDESCSTRUCT){"WSAEMFILE", "Too many open files.", "Too many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process, or per thread."}});
            errorCodes.insert({10035, (WINSOCKERRORDESCSTRUCT){"WSAEWOULDBLOCK", "Resource temporarily unavailable.", "This error is returned from operations on nonblocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a nonfatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a nonblocking SOCK_STREAM socket, since some time must elapse for the connection to be established."}});
            errorCodes.insert({10036, (WINSOCKERRORDESCSTRUCT){"WSAEINPROGRESS", "Operation now in progress.", "A blocking operation is currently executing. Windows Sockets only allows a single blocking operation—per- task or thread—to be outstanding, and if any other function call is made (whether or not it references that or any other socket) the function fails with the WSAEINPROGRESS error."}});
            errorCodes.insert({10037, (WINSOCKERRORDESCSTRUCT){"WSAEALREADY", "Operation already in progress.", "An operation was attempted on a nonblocking socket with an operation already in progress—that is, calling connect a second time on a nonblocking socket that is already connecting, or canceling an asynchronous request (WSAAsyncGetXbyY) that has already been canceled or completed."}});
            errorCodes.insert({10038, (WINSOCKERRORDESCSTRUCT){"WSAENOTSOCK", "Socket operation on nonsocket.", "An operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid."}});
            errorCodes.insert({10039, (WINSOCKERRORDESCSTRUCT){"WSAEDESTADDRREQ", "Destination address required.", "A required address was omitted from an operation on a socket. For example, this error is returned if sendto is called with the remote address of ADDR_ANY."}});
            errorCodes.insert({10040, (WINSOCKERRORDESCSTRUCT){"WSAEMSGSIZE", "Message too long.", "A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram was smaller than the datagram itself."}});
            errorCodes.insert({10041, (WINSOCKERRORDESCSTRUCT){"WSAEPROTOTYPE", "Protocol wrong type for socket.", "A protocol was specified in the socket function call that does not support the semantics of the socket type requested. For example, the ARPA Internet UDP protocol cannot be specified with a socket type of SOCK_STREAM."}});
            errorCodes.insert({10042, (WINSOCKERRORDESCSTRUCT){"WSAENOPROTOOPT", "Bad protocol option.", "An unknown, invalid or unsupported option or level was specified in a getsockopt or setsockopt call."}});
            errorCodes.insert({10043, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10044, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10045, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10046, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10047, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10048, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10049, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10050, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10051, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10052, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10053, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10054, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10055, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10056, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10057, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10058, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10059, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10060, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10061, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10062, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10063, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10064, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10065, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10066, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10066, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10067, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10068, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10069, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10070, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10071, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10091, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10092, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10093, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10101, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10102, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10103, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10104, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10105, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10106, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10107, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10108, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10109, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10110, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10111, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10112, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11001, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11002, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11003, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11004, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11005, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11006, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11007, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11008, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11009, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11010, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11011, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11012, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11013, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11014, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11015, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11016, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11017, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11018, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11019, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11020, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11021, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11022, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11023, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11024, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11025, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11026, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11027, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11028, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11029, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11030, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({11031, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
        }
    public:
        static std::unordered_map<int,WINSOCKERRORDESCSTRUCT> errorCodes;
        static bool init;
        static WINSOCKERRORDESCSTRUCT get(int errorCode) {
            // if not init, init it
            if (winsockErrorDesc::init == false) {
                winsockErrorDesc::initVector();
            }
            WINSOCKERRORDESCSTRUCT valCopy = winsockErrorDesc::errorCodes.at(errorCode);
            return valCopy;
        }
};

bool winsockErrorDesc::init = false;
std::unordered_map<int,WINSOCKERRORDESCSTRUCT> winsockErrorDesc::errorCodes = std::unordered_map<int,WINSOCKERRORDESCSTRUCT>();

#endif