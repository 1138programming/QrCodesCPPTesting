#ifndef VERTICAL_SCROLLABLE_HPP
#define VERTICAL_SCROLLABLE_HPP

#include "drawable.hpp"
#include "../include/raylib-cpp.hpp"

#include <vector>

class VerticalScrollable : public Drawable {
    private:
        std::vector<Drawable*> thingsToDraw;
        ShouldScale width, height, scrollMultiple;
        raylib::Color borderColor;
        int lastX, lastY;
        double scrollPx;
    public:
        VerticalScrollable(ShouldScale width, ShouldScale height, raylib::Color borderColor, ShouldScale scrollMultiple) {
            this->width = width;
            this->height = height;   
            this->borderColor = borderColor;
            this->scrollPx = 0.0;
            this->scrollMultiple = scrollMultiple;
        }

        void add(void* thingToDraw) {
            Drawable* drawableThing = (Drawable*)thingToDraw;
            this->thingsToDraw.push_back(drawableThing);
        } 
        std::vector<Drawable*>* getInternalVector() {
            return &(this->thingsToDraw);
        }
        raylib::Color getBorderColor() {
            return this->borderColor;
        }
        ShouldScale getScrollMultiple() {
            return this->scrollMultiple;
        }
        bool isHovering() {
            raylib::Rectangle rect(this->lastX, this->lastY, this->width, this->height);
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }

        void draw(int x, int y) override {
            this->lastX = x;
            this->lastY = y;
            if (isHovering()) {
                this->scrollPx += (GetMouseWheelMoveV().y * this->scrollMultiple);
            }
            raylib::Rectangle border(x, y, this->width, this->height);
            border.DrawLines(this->borderColor);

            BeginScissorMode(x, y, this->width, this->height);
                double currentY = y+this->scrollPx;
                for (Drawable* i : this->thingsToDraw) {
                    i->draw(x, currentY);
                    currentY += i->getHeight();
                }
            EndScissorMode();
        }
        ShouldScale getWidth() override {
            return this->width;
        }
        ShouldScale getHeight() override {
            return this->height;
        }
};

#endif