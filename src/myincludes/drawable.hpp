#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "displayPos.hpp"
#include "../include/raylib-cpp.hpp"
#include "shouldScale.hpp"

class Drawable {
    protected:
        DISPLAYPOS wantedPos;
        // using normal raylib struct for compatibility
        Vector2 customTransformation = {0, 0};
    public:
        // meant to be overwritten
        virtual void draw(int x, int y) {}

        raylib::Vector2 getSize() {
            return raylib::Vector2(getWidth(), getHeight());
        }
        DISPLAYPOS getDisplayPos() {
            return this->wantedPos;
        }
        Vector2 getCustomOffset() {
            return this->customTransformation;
        }
        void setDisplayPos(DISPLAYPOS pos) {
            this->wantedPos = pos;
        }
        void setCustomOffset(Vector2 transformation) {
            this->customTransformation = transformation;
        }

        // __meant to be overwritten__
        //mutators
        virtual void setWidth(ShouldScale width) { }
        virtual void setHeight(ShouldScale height) { }
        // accessors
        virtual ShouldScale getWidth() { return 0.0; }
        virtual ShouldScale getHeight() { return 0.0; }
};

#endif