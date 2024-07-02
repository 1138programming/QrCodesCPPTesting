#ifndef TOAST_HPP
#define TOAST_HPP

#include "drawable.hpp"
#include "toastLengths.hpp"
#include "../include/raylib-cpp.hpp"
#include "ezText.hpp"
#include "movementAnimation.hpp"
#include <string>

class Toast : public Drawable {
    private:
        TOASTLENGTHS length;
        std::string message;
        double setTime;
        MovementAnimation selfAnimation;
    public:
        Toast(std::string message, TOASTLENGTHS length) : selfAnimation(this, raylib::Vector2(0.0f, 0.0f)) {
            this->message = message;
            this->length = length;
        }

        void init() {
            switch(length) {
                case LENGTH_NULL:
                    this->setTime = GetTime() - 1.0;
                break;
                case LENGTH_SHORT:
                    this->setTime = GetTime() + 1.0;
                break;
                case LENGTH_NORMAL:
                    this->setTime = GetTime() + 2.0;
                break;
                case LENGTH_LONG:
                    this->setTime = GetTime() + 4.0;
                break;
            }
            this->customTransformation.y -= GetScreenHeight()/10.0;
            this->selfAnimation = MovementAnimation(this, raylib::Vector2(0.0f, 0.0f));
            this->selfAnimation.setDuration((this->setTime - GetTime())/3.0f);
            this->selfAnimation.setEasing(EASING_IN_SINE);
            this->selfAnimation.start();
        }
        bool isDone() {
            return (GetTime() >= this->setTime);
        }

        void draw(int x, int y) override {
            this->selfAnimation.update();
            raylib::Rectangle rect(x + this->customTransformation.x, y + this->customTransformation.y, GetScreenWidth()/5.0, GetScreenHeight()/10.0);
            EzText text(raylib::Text(GetFontDefault(), message, (rect.width/message.length())*1.4), RAYWHITE);
            raylib::Vector2 textSize = text.getSize();
            
            rect.DrawRounded(1.0, 20, raylib::Color(40,40,40,200));
            text.draw(rect.x + (rect.width/2.0 - textSize.x/2.0), rect.y + (rect.height/2.0 - textSize.y/2.0));
        }
        /*
        virtual void setWidth(ShouldScale width) { }
        virtual void setHeight(ShouldScale height) { }
        // accessors
        virtual ShouldScale getWidth() { return 0.0; }
        virtual ShouldScale getHeight() { return 0.0; }
        */

};

#endif