#ifndef SIZESCALING_HPP
#define SIZESCALING_HPP

#include "../include/raylib-cpp.hpp"

#include <iomanip>


class SizeScaling {
    public:
        static raylib::Vector2 tileSizePx;
        static raylib::Vector2 numOfTiles;

        // constructor-ish
        inline static void init() {
            SizeScaling::tileSizePx.x = 1.0;
            SizeScaling::tileSizePx.y = 1.0;

            SizeScaling::numOfTiles.x = GetScreenWidth()/SizeScaling::tileSizePx.x;
            SizeScaling::numOfTiles.y = GetScreenHeight()/SizeScaling::tileSizePx.y;
        }

        inline static void update() {
            SizeScaling::tileSizePx.x = GetScreenWidth()/SizeScaling::numOfTiles.x;
            SizeScaling::tileSizePx.y = GetScreenHeight()/SizeScaling::numOfTiles.y;
        }
        // this should be constant
        inline static Vector2 getTileSize() {
            SizeScaling::update();
            return SizeScaling::tileSizePx;
        }
        inline static float xMult() {
            SizeScaling::update();
            return SizeScaling::tileSizePx.x;
        }
        inline static float yMult() {
            SizeScaling::update();
            return SizeScaling::tileSizePx.y;
        }
        inline static int round(float num) {
            if (num < 0) {
                return (int)num-0.5;
            }
            else {
                return (int)num+0.5;
            }
        }
};

raylib::Vector2 SizeScaling::tileSizePx = raylib::Vector2(0.0, 0.0);
raylib::Vector2 SizeScaling::numOfTiles = raylib::Vector2(0.0, 0.0);

// custom user literals (https://en.cppreference.com/w/cpp/language/user_literal)
ShouldScale operator"" _spX(long double num) {
    return num;
}
ShouldScale operator"" _spY(long double num) {
    return mult_y(num);
}

unsigned long long int operator"" _spX(unsigned long long int num) {
    return mult_x(num);
}
unsigned long long int operator"" _spY(unsigned long long int num) {
    return mult_y(num);
}

#endif