#ifndef TEXTURED_BUTTON_HPP
#define TEXTURED_BUTTON_HPP

#include "../include/raylib-cpp.hpp"
#include "shouldScale.hpp"
#include "drawable.hpp"

class TexturedButton : public Drawable{
    private:
        raylib::Texture2D mainTexture;
        raylib::Texture2D hoverTexture;
        ShouldScale width, height;
        float lastX, lastY;
        float lastWidth, lastHeight;
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

            if (this->lastWidth != (float)this->width || this->lastHeight != (float)this->height) {
                // update textures if width or height changes (EXPENSIVE)
                raylib::Image mainImage = this->mainTexture.GetData();
                raylib::Image hoverImage = this->hoverTexture.GetData();

                mainImage.Resize(this->width, this->height);
                hoverImage.Resize(this->width, this->height);

                this->mainTexture.Unload();
                this->mainTexture = mainImage;
                this->mainTexture.Unload();
                this->mainTexture = hoverImage;

                mainImage.Unload();
                hoverImage.Unload();
            }

            if (isHovering()) {
                this->hoverTexture.Draw(x, y);
            }
            else {
                this->mainTexture.Draw(x, y);
            }

            this->lastWidth = (float)this->width;
            this->lastHeight = (float)this->height;
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