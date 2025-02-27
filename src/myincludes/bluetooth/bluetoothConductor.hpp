#ifndef BLUETOOTHCONDUCTOR_HPP
#define BLUETOOTHCONDUCTOR_HPP

#include "btTabObj.hpp"
#include "../jsonParser.hpp"
#include "../DatabaseMan.hpp"

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
        // to prevent clutter
        void handleWriteTransactions(bt::TABTRANSACTION* trans, std::launch policy) {
            switch (trans->transactionType) {
                case bt::TRANS_SEND_LOCAL_DB: {
                    // TODO: IMPLEMENT
                    trans->success = true;
                    trans->batmanTrans = false;
                    trans->writeTransaction = true;

                    trans->data = std::async(policy, this->returnEmptyVector, this);
                    break;
                }
                case bt::TRANS_SEND_LOCAL_DB_HASH: {
                    // TODO: IMPLEMENT
                    trans->success = true;
                    trans->batmanTrans = false;
                    trans->writeTransaction = true;

                    trans->data = std::async(policy, this->returnEmptyVector, this);
                    break;
                }
            }
        }
    public:
        bt::TABTRANSACTION* initReadyTransaction(bt::TABTRANSACTION* trans) {
            // get policy at the beginning, as it will need to be used later in most cases.
            std::launch policy = std::launch::async | std::launch::deferred;
            switch(trans->parent->getCallType()) {
                case bt::CALLTYPE_DEFAULT: {
                    policy = std::launch::async | std::launch::deferred;
                    break;
                }
                case bt::CALLTYPE_DEFERRED: {
                    policy = std::launch::deferred;
                    break;
                }
                case bt::CALLTYPE_ASYNCHRONOUS: {
                    policy = std::launch::async;
                    break;
                }
            }

            // error cases
            if (trans->transactionType == bt::TRANS_SOCKET_ERROR) {
                trans->parent->sendNack(); // honestly could be a bad idea in some scenarios lol
                trans->success = false;
                trans->batmanTrans = true;
                trans->writeTransaction = false;

                trans->parent->sockSuicide();
                return trans;
            }
            if (trans->transactionType == bt::TRANS_CLOSE_SOCKET) {
                trans->success = false;
                trans->batmanTrans = true;
                trans->writeTransaction = false;

                trans->parent->sockSuicide();
                return trans;
            }
            // read/write handling
            if (trans->writeTransaction) {
                this->handleWriteTransactions(trans, policy);
            }
            else {
                trans->success = true;
                trans->batmanTrans = false;
                trans->writeTransaction = false;

                trans->data = std::async(policy, trans->parent->internalRead, trans->parent, std::ref(trans->success));
                return trans;
            }

            return trans;
        }

        void handleTransResult(bt::TABTRANSACTION* trans) {
            if (trans->writeTransaction) {
                return;
            }

            switch(trans->transactionType) {
                case bt::TRANS_RECV_MATCH: {
                    std::vector<char> readDataVec = trans->data.get().value();
                    if (trans->success) {
                        std::string data = std::string(readDataVec.begin(), readDataVec.end());
                        std::cerr << data << std::endl;
                    
                        // parse data and put it into database
                        JsonParser parser(data);
                        std::vector<MATCH_DATAPOINT> vectData = parser.parseMatch();
                        DatabaseMan databaseCall(vectData);
                        databaseCall.maketh();
                    }
                    break;
                }
                case bt::TRANS_RECV_TABLET_INFO: {
                    std::vector<char> tabletScoutingInfoVec = trans->data.get().value();
                    if (trans->success) {
                        std::string tabletScoutingInfo = std::string(tabletScoutingInfoVec.begin(), tabletScoutingInfoVec.end());
                        trans->parent->setScoutingName(tabletScoutingInfo);
                    }
                    break;
                }
            }
        }
};

#endif