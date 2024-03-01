#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../include/raylib-cpp.hpp"
#include "drawable.hpp"

class DrawableTexture : public Drawable {
    private:
        raylib::Texture2D internalTexture;
        ShouldScale width, height;
        float lastWidth, lastHeight;
        Color tint = WHITE;
    public:
        DrawableTexture(ShouldScale width, ShouldScale height, raylib::Image texture, raylib::Color tint = WHITE) {
            this->internalTexture = texture;
            this->width = width;
            this->height = height;
            this->tint = tint;
        }

        void draw(int x, int y) override {

            if (this->lastWidth != (float)this->width || this->lastHeight != (float)this->height) {
                // scale texture (EXPENSIVE)
                raylib::Image textureImg = this->internalTexture.GetData();

                textureImg.Resize(this->width, this->height);

                this->internalTexture.Unload();
                this->internalTexture = textureImg;
                textureImg.Unload();
            }

            this->internalTexture.Draw(x, y, this->tint);

            this->lastWidth = (float)this->width;
            this->lastHeight = (float)this->height;
        }
};

#endif