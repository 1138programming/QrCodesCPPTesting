#ifndef MURMURHASH2NEUTRAL_HPP
#define MURMURHASH2NEUTRAL_HPP

#include <string>
#include <fstream>
#include <sstream>

std::string readWholeFile(std::string fileName) {
    std::stringstream fileContents;
    std::ifstream file(fileName);

    fileContents << file.rdbuf();

    return fileContents.str();
}

int murmurHash(std::vector<char> data) {
    const int multVal = 0x5bd1e995;
    const int rotate = 24;

    int hash = 1138 ^ ((int)data.size());

    int index = 0;
    while (((int)data.size())-index >= 4) {
        int curr;

        curr = data[index];
        curr |= ((int)data[1 + index] << 8);
        curr |= ((int)data[2 + index] << 16);
        curr |= ((int)data[3 + index] << 24);

        curr *= multVal;
        curr ^= (curr >> rotate);
        curr *= multVal;
        
        hash *= multVal;
        hash ^= curr;

        index += 4;
    }

    switch (((int)data.size()) - index) {
        case 3: {
            hash ^= (((int)data[2 + index]) << 16);
        }
        case 2: {
            hash ^= (((int)data[1 + index]) << 8);

        }
        case 1: {
            hash ^= ((int)data[index]);
            hash *= multVal;
        }
    }

    hash ^= (hash >> 13);
    hash *= multVal;
    hash ^= (hash >> 15);

    return hash;
}

#endif