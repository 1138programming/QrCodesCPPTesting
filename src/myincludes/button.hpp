#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "drawable.hpp"
#include "../include/raylib-cpp.hpp"
#include "../myincludes/ezText.hpp"
#include "../myincludes/shouldScale.hpp"

class Button : public Drawable {
    private:
        EzText text;
        ShouldScale width, height;
        ShouldScale lastX, lastY;
        raylib::Color borderColor, hoverColor, backgroundColor;
    public:
        Button(int width, int height, raylib::Color borderColor, raylib::Color backgroundColor, raylib::Color hoverColor, EzText text) {
            this->width = width;
            this->height = height;
            this->borderColor = borderColor;
            this->hoverColor = hoverColor;
            this->text = text;
        }

        // accessors
        raylib::Color getBorderColor() {
            return this->borderColor;
        }
        raylib::Color getBackgroundColor() {
            return this->backgroundColor;
        }
        raylib::Color getHoverColor() {
            return this->hoverColor;
        }

        // mutators
        void setBorderColor(raylib::Color color) {
            this->borderColor = color;
        }
        void setHoverColor(raylib::Color color) {
            this->hoverColor = color;
        }
        void setBackgroundColor(raylib::Color color) {
            this->backgroundColor = color;
        }

        //functions
        bool isHovering() {
            raylib::Rectangle rect(this->lastX.getData(), this->lastY.getData(), this->width.getData(), this->height.getData());
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }

        //overriden functions
        void draw(int x, int y) override {
            // update vars to use for collision detection
            this->lastX = x;
            this->lastY = y;

            // drawing
            raylib::Rectangle rect(x, y, this->width.getData(), this->height.getData());
            // draw BG
            if (isHovering()) {
                rect.Draw(this->hoverColor);
            }
            else {
                rect.Draw(this->backgroundColor);
            }
            // draw border
            rect.DrawLines(this->borderColor);
            // draw text
            float textX = (this->width.getData()/2.0)-(this->text.getWidth()/2.0);
            float textY = (this->height.getData()/2.0)-(this->text.getHeight()/2.0);
            textX += x;
            textY += y;

            this->text.draw(textX, textY);
        }
        float getWidth() override {
            return (float)this->width.getData();
        }
        float getHeight() override {
            return (float)this->height.getData();
        }
};

#endif