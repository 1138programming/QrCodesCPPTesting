#ifndef TOGGLE_HPP
#define TOGGLE_HPP

#include "drawable.hpp"
#include "shouldScale.hpp"
#include "raylib-cpp.hpp"

class Toggle : public Drawable{
    private:
        ShouldScale width,height;
        raylib::Color color;
        double innerTogglePercentage;
        bool checked, lastCheckState;
        int lastX, lastY;
    public:
        Toggle(ShouldScale width, ShouldScale height, double innerTogglePercentage, raylib::Color color) {
            this->width = width;
            this->height = height;
            this->innerTogglePercentage = innerTogglePercentage;
            this->color = color;
            
            this->lastCheckState = false;
            this->checked = false;
        }

        // useful functions
        bool isChecked() {
            return this->checked;
        }
        bool isHovering() {
            raylib::Rectangle rect(this->lastX, this->lastY, this->width, this->height);
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }
        bool isPressed() {
            bool state = (isHovering() && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            bool retval = (state == true && this->lastCheckState == false);
            this->lastCheckState = state;
            return retval;
        }

        void draw(int x, int y) override {
            if (isPressed()) {
                this->checked = (!this->checked);
            }
            raylib::Rectangle outerRect(x, y, this->width, this->height);
            outerRect.DrawLines(this->color);

            if (this->checked) {
                float innerRectWidth = outerRect.width * this->innerTogglePercentage;
                float innerRectHeight = outerRect.height * this->innerTogglePercentage;
                raylib::Rectangle innerRect(outerRect.x + (outerRect.width/2.0 - innerRectWidth/2.0), outerRect.y + (outerRect.height/2.0 - innerRectHeight/2.0), innerRectWidth, innerRectHeight);
                innerRect.Draw(this->color);
            }
            this->lastX = x;
            this->lastY = y;
        }

        ShouldScale getWidth() override {
            return this->width;
        }
        ShouldScale getHeight() override {
            return this->height;
        }
};


#endif