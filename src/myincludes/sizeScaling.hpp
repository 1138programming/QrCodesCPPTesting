#ifndef SIZESCALING_HPP
#define SIZESCALING_HPP

#include "../include/raylib-cpp.hpp"

#include <iomanip>

class SizeScaling {
    private:
        Vector2 tileSizePx;
        Vector2 numOfTiles;
        raylib::Window* window;
        void update() {
            this->tileSizePx.x = this->window->GetWidth()/this->numOfTiles.x;
            this->tileSizePx.y = this->window->GetHeight()/this->numOfTiles.y;
        }
    public:
        SizeScaling(raylib::Window* window) {
            this->window = window;

            this->tileSizePx.x = 1.0;
            this->tileSizePx.y = 1.0;

            this->numOfTiles.x = this->window->GetWidth()/tileSizePx.x;
            this->numOfTiles.y = this->window->GetHeight()/tileSizePx.y;
        }
        // this should be constant
        Vector2 getNumTiles() {
            return this->numOfTiles;
        }
        Vector2 getTileSize() {
            this->tileSizePx.x = this->window->GetWidth()/this->numOfTiles.x;
            this->tileSizePx.y = this->window->GetHeight()/this->numOfTiles.y;
            return this->tileSizePx;
        }
        raylib::Window* getWindow() {
            return this->window;
        }
        float xMult() {
            this->update();
            return this->tileSizePx.x;
        }
        float yMult() {
            this->update();
            return this->tileSizePx.y;
        }
};

#endif