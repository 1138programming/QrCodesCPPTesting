#ifndef BLUETOOTHCONDUCTOR_HPP
#define BLUETOOTHCONDUCTOR_HPP

#include "btTabObj.hpp"
#include "../jsonParser.hpp"
#include "../DatabaseMan.hpp"
#include "../murmurHash2Neutral.hpp"

#include <vector>
#include <optional>
#include <fstream>

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
                    std::string dbFileStr = readWholeFile("resources/csv/scouterList.csv");
                    dbFileStr.append("\n");
                    dbFileStr.append(readWholeFile("resources/csv/teamCompList.csv"));

                    std::vector<char> dbFileArr;
                    for(char i : dbFileStr) {
                        dbFileArr.push_back(i);
                    }
                    trans->batmanTrans = false;
                    trans->writeTransaction = true;

                    trans->data = std::async(policy, trans->parent->internalWrite, trans->parent, dbFileArr, std::ref(trans->success));
                    break;
                }
                case bt::TRANS_SEND_LOCAL_DB_HASH: {
                    std::string dbFileStr = readWholeFile("resources/csv/scouterList.csv");
                    dbFileStr.append("\n");
                    dbFileStr.append(readWholeFile("resources/csv/teamCompList.csv"));
                    std::vector<char> dbFileVec;
                    for (char i : dbFileStr) {
                        dbFileVec.push_back(i);
                    }
                    
                    int murmurRes = murmurHash(dbFileVec);
                    DebugConsole::println(std::string("Sending Hash: ") + std::to_string(murmurRes), DBGL_DEVEL);

                    std::vector<char> murmurHashData;
                    for (int i = 0; i < BT_HASH_SIZE; i++) {
                        murmurHashData.push_back(((char*)&murmurRes)[i]);
                    }
                    trans->batmanTrans = false;
                    trans->writeTransaction = true;

                    trans->data = std::async(policy, trans->parent->internalWrite, trans->parent, murmurHashData, std::ref(trans->success));
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
            DebugConsole::println(std::string("Got Transaction Code: ") + std::to_string(trans->transactionType), DBGC_BLUE, DBGL_DEVEL);
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
            DebugConsole::println("Transaction result being handled", DBGC_BLUE, DBGL_DEVEL);
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
                        DatabaseMan databaseCall;
                            databaseCall.setmatchdata(vectData);
                        databaseCall.addMatchDatapoints();
                    }
                    break;
                }
                case bt::TRANS_RECV_TABLET_INFO: {
                    std::optional<std::vector<char>> tabletScoutingInfoVec = trans->data.get();
                    if (trans->success && tabletScoutingInfoVec.has_value()) {
                        std::string tabletScoutingInfo = std::string(tabletScoutingInfoVec.value().begin(),  tabletScoutingInfoVec.value().end());
                        trans->parent->setScoutingName(tabletScoutingInfo);
                    }
                    break;
                }
            }
        }
};

#endif