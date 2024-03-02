#ifndef TABHANDLER_HPP
#define TABHANDLER_HPP

#include "drawable.hpp"
#include "empty.hpp"
#include <vector>


class TabHandler : public Drawable {
    private:
        std::vector<Button*> thingsToDraw;
        ShouldScale x, y, width, height;
    public:
        TabHandler() {
            this->x = 0;
            this->y = 0;
            this->width = 0;
            this->height = 0;
        }
        TabHandler(raylib::Rectangle size) {
            this->x = size.x;
            this->y = size.y;
            this->width = size.width;
            this->height = size.width;
        }
        TabHandler(ShouldScale x, ShouldScale y, ShouldScale width, ShouldScale height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }

        TabHandler& add(void* ptr) {
            Button* drawable = (Button*) ptr;
            this->thingsToDraw.push_back(drawable);
            return *this;
        }
        void draw(int x, int y) override {
            this->x = x;
            this->y = y;
            float buttonWidth = this->width / (this->thingsToDraw.size());
            for (unsigned int i = 0; i < this->thingsToDraw.size(); i++) {
                Button* current = this->thingsToDraw.at(i);

                current->setWidth(buttonWidth);
                current->setHeight(this->height);

                current->drawTab(i*buttonWidth + (this->x), this->y);
            }
        }
        void updateAndDraw(Rectangle size) {
            this->width = size.width;
            this->height = size.height;
            this->draw(size.x, size.y);
        }
        void updateAndDraw(ShouldScale x, ShouldScale y, ShouldScale width, ShouldScale height) {
            this->width = width;
            this->height = height;
            this->draw(x, y);
        }
};

#endif