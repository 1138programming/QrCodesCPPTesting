#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "drawable.hpp"
#include "../include/raylib-cpp.hpp"
#include "ezText.hpp"
#include <string>

class TextBox : public Drawable {
    private:
        ShouldScale width;
        ShouldScale height;
        raylib::Font font;
        raylib::Color rectColor, textColor;
        ShouldScale textSize;
        ShouldScale spacing;
        char* text;
        int sizeLimit = 0;
        int currentChar = 0;
        int lastX, lastY;

        void updateText() {
            if (isHovering()) {
                int key = GetCharPressed();
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    this->currentChar--;
                    if (this->currentChar < 0) {
                        this->currentChar = 0;
                    }
                    this->text[currentChar] = '\0';
                }
                while (((key > 31 && key < 123)) && (this->currentChar < this->sizeLimit)) {
                        this->text[this->currentChar] = key;
                        this->text[this->currentChar+1] = '\0';
                        this->currentChar++;            
                        key = GetCharPressed();
                }
            }
        }
    public:
        TextBox(ShouldScale width, ShouldScale height, int sizeLimit, ShouldScale spacing,  ShouldScale textSize, Font font, raylib::Color fontColor, raylib::Color rectColor) {
            this->width = width;
            this->height = height;
            this->sizeLimit = sizeLimit;
            this->text = (char*)calloc(this->sizeLimit+1, sizeof(char));
            this->textSize = textSize;
            this->font = raylib::Font(font);
            this->textColor = fontColor;
            this->rectColor = rectColor;
            this->spacing = spacing;
            this->lastX = 0;
            this->lastY = 0;
        }
        void draw(int x, int y) override {
            this->lastX = x;
            this->lastY = y;
            this->updateText();
            
            EzText textToDraw(raylib::Text(this->font, std::string(this->text)), this->textColor, this->textSize, this->spacing);
            raylib::Rectangle rect(x, y, this->width, this->height);
            rect.DrawLines(rectColor);
            textToDraw.draw(x,y);
        }


        std::string getText() {
            return std::string(this->text);
        }


        bool isHovering() {
            raylib::Rectangle size(this->lastX, this->lastY, this->width, this->height);
            return CheckCollisionPointRec(GetMousePosition(), size);
        }

        ShouldScale getWidth() override {
            return this->width;
        }
        ShouldScale getHeight() override {
            return this->height;
        }

        ~TextBox() {
            free(this->text);
        }
};

#endif