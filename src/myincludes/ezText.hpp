#ifndef EZTEXT_HPP
#define EZTEXT_HPP

#include "../include/raylib-cpp.hpp"
#include "drawable.hpp"
#include "shouldScale.hpp"

class EzText : public Drawable {
    private:
        raylib::Text text;
        raylib::Color drawColor;
        ShouldScale textSize, spacing;
    public:
        EzText() {
            this->text = raylib::Text(GetFontDefault(),"");
            this->drawColor = WHITE;
            this->spacing = ShouldScale(0, false, NODEPENDENCY);
            this->textSize = ShouldScale(10.0, false, NODEPENDENCY);
        }
        EzText(raylib::Text text, raylib::Color drawColor) {
            this->text = text;
            this->drawColor = drawColor;
            this->spacing = ShouldScale(this->text.spacing, false, NODEPENDENCY);
            this->textSize = ShouldScale(this->text.fontSize, false, NODEPENDENCY);
        }
        EzText(raylib::Text text, raylib::Color drawColor, ShouldScale textSize, ShouldScale spacing) {
            this->text = text;
            this->drawColor = drawColor;
            this->textSize = textSize;
            this->spacing = spacing;
        }
        void draw(int x, int y) override {
            this->text.fontSize = this->textSize.getData();
            this->text.spacing = this->spacing.getData();

            this->text.Draw(this->text.font, this->text.GetText(), raylib::Vector2(x + this->customTransformation.x, y + this->customTransformation.y), text.GetFontSize(), this->spacing, this->drawColor);
        }

        // accessors / mutators
        std::string getText() {
            return this->text.GetText();
        }
        void setText(std::string str) {
            this->text.SetText(str);
        }

        ShouldScale getTextSize() {
            return this->textSize;
        }
        void setTextSize(ShouldScale textSize) {
            this->textSize = textSize;
        }

        ShouldScale getWidth() override {
            this->text.fontSize = this->textSize.getData();
            this->text.spacing = this->spacing.getData();

            return this->text.MeasureEx().x;
        } 
        ShouldScale getHeight() override {
            this->text.fontSize = this->textSize.getData();
            this->text.spacing = this->spacing.getData();
            
            return this->text.MeasureEx().y;
        }
};

#endif