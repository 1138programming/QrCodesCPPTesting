#ifndef ANIMATIONUNION_HPP
#define ANIMATIONUNION_HPP

typedef union {
    struct pos {
        float x;
        float y;
    };
    double num;
} ANIMATIONUNION;

#endif