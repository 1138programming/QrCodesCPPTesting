#ifndef DATAPOINT_STRUCT_HPP
#define DATAPOINT_STRUCT_HPP

#include <string>

typedef struct {
    std::string scouterID;
    std::string matchID;
    std::string teamID;
    std::string datapointID;
    std::string datapointValue;
    std::string DCTimestamp;
} MATCH_DATAPOINT;

#endif