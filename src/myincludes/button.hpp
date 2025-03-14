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
        float lastX, lastY;
        raylib::Color borderColor, hoverColor, backgroundColor;
        bool lastClickState = false;
        bool disabled = false;
        public:
        Button(ShouldScale width, ShouldScale height, raylib::Color borderColor, raylib::Color backgroundColor, raylib::Color hoverColor, EzText text) {
            this->width = width;
            this->height = height;
            this->borderColor = borderColor;
            this->hoverColor = hoverColor;
            this->backgroundColor = backgroundColor;
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
            raylib::Rectangle rect(this->lastX, this->lastY, this->width, this->height);
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }
        bool isPressed() {
            if (isDisabled()) {
                return false;
            }
            bool clickState = (isHovering() && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            bool retval = (clickState == true && this->lastClickState == false);
            this->lastClickState = clickState;
            return retval;
        }

        void setDisabled(bool state) {
            this->disabled = state;
        }
        void disable() {
            this->disabled = true;
        }
        void enable() {
            this->disabled = false;
        }
        bool isDisabled() {
            return this->disabled;
        }

        void drawTab(int x, int y) {
            this->lastX = x;
            this->lastY = y;

            // drawing
            raylib::Rectangle rect(x, y, this->getWidth(), this->getHeight());
            // draw BG
            if (isHovering() && !isDisabled()) {
                rect.Draw(this->hoverColor);
            }
            else {
                rect.Draw(this->backgroundColor);
            }
            // draw border
            if (!isDisabled()) {
                rect.DrawLines(this->borderColor);
            }
            else {
                DrawLine(rect.x, rect.y, rect.x+width, rect.y, this->borderColor);
                DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height, this->borderColor);
                DrawLine(rect.x+width, rect.y, rect.x+width, rect.y+rect.height, this->borderColor);
            }
            // draw text
            float textX = (this->getWidth()/2.0)-(this->text.getWidth()/2.0);
            float textY = (this->getHeight()/2.0)-(this->text.getHeight()/2.0);
            textX += x;
            textY += y;

            this->text.draw(textX, textY);
        }

        //overriden functions
        void draw(int x, int y) override {
            // update vars to use for collision detection
            this->lastX = x;
            this->lastY = y;

            // drawing
            raylib::Rectangle rect(x, y, this->getWidth(), this->getHeight());
            // draw BG
            if (isHovering() && !isDisabled()) {
                rect.Draw(this->hoverColor);
            }
            else {
                rect.Draw(this->backgroundColor);
            }
            // draw border
            rect.DrawLines(this->borderColor);
            // draw text
            float textX = (this->getWidth()/2.0)-(this->text.getWidth()/2.0);
            float textY = (this->getHeight()/2.0)-(this->text.getHeight()/2.0);
            textX += x;
            textY += y;

            this->text.draw(textX, textY);
        }
        ShouldScale getWidth() override {
            return (this->width);
        }
        ShouldScale getHeight() override {
            return (this->height);
        }

        void setWidth(ShouldScale width) override {
            this->width = width;
        }
        void setHeight(ShouldScale height) override {
            this->height = height;
        }
};

#endif