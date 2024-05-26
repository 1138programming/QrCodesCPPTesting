#ifndef WINSOCKERRORDESC_HPP
#define WINSOCKERRORDESC_HPP

#include <unordered_map>
#include "winsockErrorDescStruct.hpp"

class winsockErrorDesc {
    private:
        static void initVector() {
            errorCodes.clear();
            
            errorCodes.insert({6, (WINSOCKERRORDESCSTRUCT){"WSA_INVALID_HANDLE", "Specified event object handle is invalid.", "An application attempts to use an event object, but the specified handle is not valid."}});
            errorCodes.insert({8, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({87, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({995, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({996, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({997, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10004, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10009, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10013, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10014, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10022, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10024, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10035, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10036, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10037, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10038, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10039, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10040, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10041, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({10042, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
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
            errorCodes.insert({6, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
            errorCodes.insert({6, (WINSOCKERRORDESCSTRUCT){"", "", ""}});
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