#ifndef DRAWABLELIST_HPP
#define DRAWABLELIST_HPP

#include <vector>
#include "drawable.hpp"
#include "expansionType.hpp"
#include <iostream>

class DrawableList : public Drawable {
    private:
        std::vector<Drawable*> thingsToDraw;
        EXPANSIONTYPE expansionType = NONE;
        bool ignoreChildren = true;
        int seperation = 0;
    public:
        // TODO: Add constructor
        DrawableList() {
            expansionType = NONE;
            ignoreChildren = true;
            seperation = 0;
        }
        DrawableList(EXPANSIONTYPE expansionType, int seperation) {
            this->expansionType = expansionType;
            this->seperation = seperation;
        }
        void add(void* thingToAdd) {
            Drawable* thingToAddDrawable = (Drawable*)thingToAdd;
            this->thingsToDraw.push_back(thingToAddDrawable);
        }

        void draw(int x, int y) override {
            int xCurrent = x;
            int yCurrent = y;
            switch(this->expansionType) {
                case HORIZONTAL:
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        current->draw(xCurrent, yCurrent);
                        // prepare next item pos
                        xCurrent += this->seperation;
                        xCurrent += current->getWidth();
                    }
                break;
                case VERTICAL:
                    for(int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        current->draw(xCurrent, yCurrent);
                        // prep next item pos
                        yCurrent += this->seperation;
                        yCurrent += current->getHeight();
                    }
                break;
                case NONE:
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        current->draw(xCurrent, yCurrent);
                    }
                break;
            }
        }

        float getWidth() override {
            switch(this->expansionType) {
                case VERTICAL:
                    // if vertical, width is just width of largest element
                    float maxWidth;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        if (current->getWidth() > maxWidth) {
                            maxWidth = current->getWidth();
                        }
                    }
                    return maxWidth;
                break;
                case HORIZONTAL:
                    // if horizontal, width is the width of all elements added, plus padding
                    float width;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        width += current->getWidth();
                        // have to remember about the padding!
                        width += (float) this->seperation;
                    }
                    return width;
                break;
                case NONE:
                    // same as vertical
                    float width2;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        if (current->getWidth() > width2) {
                            width2 = current->getWidth();
                        }
                    }
                    return width2;
                break;
                default:
                    return 0.0f;
            }
        }
        float getHeight() override {
            switch(this->expansionType) {
                case VERTICAL:
                    // all added
                    float height;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        height += current->getHeight();
                        height += this->seperation;
                    }
                    return height;
                break;
                case HORIZONTAL:
                    // heigest
                    float maxHeight;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        if (current->getHeight() > maxHeight) {
                           maxHeight = current->getHeight(); 
                        }
                    }
                    return maxHeight;
                    break;
                case NONE:
                    // same as horiz.
                    float maxHeight2;
                    for (int i = 0; i < this->thingsToDraw.size(); i++) {
                        Drawable* current = this->thingsToDraw.at(i);
                        if (current->getHeight() > maxHeight2) {
                           maxHeight2 = current->getHeight(); 
                        }
                    }
                    return maxHeight2;
                break;
                default:
                    return 0.0f;
            }
        }
};

#endif