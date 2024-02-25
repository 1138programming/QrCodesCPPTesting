#ifndef TEXTURED_BUTTON_HPP
#define TEXTURED_BUTTON_HPP

#include "../include/raylib-cpp.hpp"
#include "shouldScale.hpp"
#include "drawable.hpp"

class TexturedButton : public Drawable{
    private:
        raylib::Texture2D mainTexture = GetShapesTexture();
        raylib::Texture2D hoverTexture = GetShapesTexture();
        ShouldScale width, height;
        float lastX, lastY;
        bool lastClickState = false;
    public:
        TexturedButton(ShouldScale width, ShouldScale height, raylib::Image mainTexture, raylib::Image hoverTexture) {
            this->width = width;
            this->height = height;
            this->mainTexture = mainTexture;
            this->hoverTexture = hoverTexture;
        }

        bool isHovering() {
            raylib::Rectangle rect(this->lastX, this->lastY, this->width, this->height);
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }
        bool isPressed() {
            bool clickState = (isHovering() && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            bool retval = (clickState == true && this->lastClickState == false);
            this->lastClickState = clickState;
            return retval;
        }


        void draw(int x, int y) override {
            this->lastX = x;
            this->lastY = y;

            raylib::Rectangle destinationRect(x, y, this->width, this->height);
            if (isHovering()) {
                this->hoverTexture.Draw(x, y);
            }
            else {
                this->mainTexture.Draw(x, y);
            }
        }

        ShouldScale getWidth() override {
            return this->width;
        }
        ShouldScale getHeight() override {
            return this->height;
        }

        void setWidth(ShouldScale width) override {
            this->width = width;
        }
        void setHeight(ShouldScale height) override {
            this->height = height;
        }

        ~TexturedButton() {
            mainTexture.Unload();
            hoverTexture.Unload();
        }
};

#endif