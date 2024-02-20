#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "drawable.hpp"
#include "../include/raylib-cpp.hpp"

class Button : Drawable {
    private:
        std::string text;
        int width, height;
        int lastX, lastY;
        raylib::Color textColor, borderColor, hoverColor, backgroundColor;
    public:
        Button(int width, int height, raylib::Color borderColor, raylib::Color backgroundColor, raylib::Color hoverColor, raylib::Color textColor) {
            this->width = width;
            this->height = height;
            this->borderColor = borderColor;
            this->hoverColor = hoverColor;
            this->textColor = textColor;
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
        raylib::Color getTextColor() {
            return this->textColor;
        }

        // mutators
        void setBorderColor(raylib::Color color) {
            this->borderColor = color;
        }
        void setHoverColor(raylib::Color color) {
            this->hoverColor = color;
        }
        void setTextColor(raylib::Color color) {
            this->textColor = color;
        }
        void setBackgroundColor(raylib::Color color) {
            this->backgroundColor = color;
        }

        //functions
        bool isHovering() {
            raylib::Rectangle rect(this->lastX, this->lastY, this->width, this->height);
            return false;
        }

        //overriden functions
        void draw(int x, int y) override {
            this->lastX = x;
            this->lastY = y;
            
        }
        float getWidth() override {
            return (float)this->width;
        }
        float getHeight() override {
            return (float)this->height;
        }
};

#endif