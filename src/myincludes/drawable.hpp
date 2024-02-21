#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "displayPos.hpp"
#include "../include/raylib-cpp.hpp"

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
        virtual void setWidth(float width) { }
        virtual void setHeight(float height) { }
        // accessors
        virtual float getWidth() { return 0.0f; }
        virtual float getHeight() { return 0.0f; }
};

#endif