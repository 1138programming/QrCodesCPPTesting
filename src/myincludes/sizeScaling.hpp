#ifndef SIZESCALING_HPP
#define SIZESCALING_HPP

#include "../include/raylib-cpp.hpp"

#include <iomanip>


class SizeScaling {
    public:
        static raylib::Vector3 tileSizePx;
        static raylib::Vector3 numOfTiles;

        // constructor-ish
        inline static void init() {
            SizeScaling::tileSizePx.x = 1.0;
            SizeScaling::tileSizePx.y = 1.0;
            SizeScaling::tileSizePx.z = 1.0;

            SizeScaling::numOfTiles.x = GetScreenWidth()/SizeScaling::tileSizePx.x;
            SizeScaling::numOfTiles.y = GetScreenHeight()/SizeScaling::tileSizePx.y;
            SizeScaling::numOfTiles.z = (sqrt((GetScreenWidth() * GetScreenWidth()) + (GetScreenHeight() * GetScreenHeight()))/SizeScaling::tileSizePx.z); //pythag. thrm
        }

        inline static void update() {
            SizeScaling::tileSizePx.x = GetScreenWidth()/SizeScaling::numOfTiles.x;
            SizeScaling::tileSizePx.y = GetScreenHeight()/SizeScaling::numOfTiles.y;
            SizeScaling::tileSizePx.z = (sqrt((GetScreenWidth() * GetScreenWidth()) + (GetScreenHeight() * GetScreenHeight()))/SizeScaling::numOfTiles.z);
        }
        // this should be constant
        inline static Vector3 getTileSize() {
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
        inline static float diagMult() {
            SizeScaling::update();
            return SizeScaling::tileSizePx.z;
        }
        inline static int round(float num) {
            if (num < 0) {
                return (int)num-0.5;
            }
            else {
                return (int)num+0.5;
            }
        }
        inline static const char* shouldScaleToCharPtr(bool shouldScale, SHOULDSCALEDEPENDENCIES dependency, long double num) {
                bool* data = (bool*)malloc(sizeof(bool) + sizeof(SHOULDSCALEDEPENDENCIES) + sizeof(long double));
                *data = shouldScale;
                SHOULDSCALEDEPENDENCIES* dataShouldScale = ((SHOULDSCALEDEPENDENCIES*)&(data[1]));
                *dataShouldScale = dependency;
                *((long double*)&(dataShouldScale[1])) = num;
                return (char*)data;
        }

};

raylib::Vector3 SizeScaling::tileSizePx = raylib::Vector3(0.0, 0.0, 0.0);
raylib::Vector3 SizeScaling::numOfTiles = raylib::Vector3(0.0, 0.0, 0.0);

// custom user literals (https://en.cppreference.com/w/cpp/language/user_literal)
const char* operator"" _spX(long double num) {
    return SizeScaling::shouldScaleToCharPtr(true, XDEPENDENT, num);
}
const char* operator"" _spY(long double num) {
    return SizeScaling::shouldScaleToCharPtr(true, YDEPENDENT, num);
}
const char* operator"" _spD(long double num) {
    return SizeScaling::shouldScaleToCharPtr(true, DIAGDEPENDENT, num);
}



#endif