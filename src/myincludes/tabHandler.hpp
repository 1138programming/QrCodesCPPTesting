#ifndef TABHANDLER_HPP
#define TABHANDLER_HPP

#include "drawable.hpp"
#include "empty.hpp"
#include <vector>


class TabHandler {
    private:
        std::vector<Drawable*> thingsToDraw;
        raylib::Rectangle size;
    public:
        TabHandler() {
            this->size = raylib::Rectangle(0,0,0,0);
        }
        TabHandler(raylib::Rectangle size) {
            this->size = size;
        }

        void add(void* ptr) {
            Drawable* drawable = (Drawable*) ptr;
            this->thingsToDraw.push_back(drawable);
        }
        void updateAndDrawAll(Rectangle size) {
            this->size = size;

            float buttonWidth = this->size.width / (this->thingsToDraw.size());
            for (unsigned int i = 0; i < this->thingsToDraw.size(); i++) {
                Drawable* current = this->thingsToDraw.at(i);

                current->setWidth(buttonWidth);
                current->setHeight(this->size.height);

                current->draw(i*buttonWidth + (this->size.x), this->size.y);
            }
        }
};

#endif