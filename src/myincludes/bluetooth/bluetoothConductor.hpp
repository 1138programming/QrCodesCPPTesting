#ifndef BLUETOOTHCONDUCTOR_HPP
#define BLUETOOTHCONDUCTOR_HPP

#include "btTabObj.hpp"

#include <vector>
#include <optional>

class BluetoothConductor {
    private:
        /*********************************************/
        /* PRIVATE COMMUNICATION PROTO FUNCs */
        /*********************************************/
        std::optional<std::vector<char>> returnEmptyVector() {
            return std::nullopt;
        }

        std::optional<std::vector<char>> internalRead(BtTabObj* transactionParent, bool& success) {
            /*
                Communication will go as follows:
                    C: 👍 (Trailing ACK)
                    T: # of bytes to be sent (signed int)
                    C: 👍 (ACK)
                    T: all data
                    C: 👍 (ACK)
            */
           success = true;
           if (!transactionParent->readyToWrite()) {
            
           }
        }
    public:

};

#endif