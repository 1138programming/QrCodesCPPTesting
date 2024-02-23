#ifndef EMPTY_HPP
#define EMPTY_HPP

#include <vector>
#include "drawable.hpp"
#include "shouldScale.hpp"

class Empty : public Drawable{
    private:
        std::vector<Drawable*> thingsToDraw;
        ShouldScale x, y, width, height;
    public:
        Empty(Rectangle size) {
            this->x = size.x;
            this->y = size.y;
            this->width = size.width;
            this->height = size.height;
        }
        Empty(ShouldScale x, ShouldScale y, ShouldScale width, ShouldScale height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }

        Empty& add(void* ref) {
            Drawable* refAsDrawable = (Drawable*)ref;
            this->thingsToDraw.push_back(refAsDrawable);
            return *this;
        }

        void updateAndDraw(Rectangle size) {
            this->width = size.width;
            this->height = size.height;
            draw(size.x, size.y);
        }
        void updateAndDraw(ShouldScale x, ShouldScale y, ShouldScale width, ShouldScale height) {
            this->width = width;
            this->height = height;
            draw(x, x);
        }
        void draw() {
            this->draw(this->x, this->y);
        }
        void draw(int x, int y) override {
            this->x = x;
            this->y = y;
            if (this->thingsToDraw.size() == 0) {
                return;
            }
            
            int xPos;
            int yPos;
            for (int i = 0; i < this->thingsToDraw.size(); i++) {
                Drawable* current = this->thingsToDraw.at(i);
                
                switch(current->getDisplayPos()) {
                    case BOTTOMCENTERED:
                        xPos = this->x + ((this->width/2.0)-(current->getWidth()/2.0));
                        yPos = this->height - current->getHeight();
                        break;
                    case BOTTOMLEFT:
                        xPos = this->x;
                        yPos = this->height - current->getHeight();
                        break;
                    case BOTTOMRIGHT:
                        xPos = this->width - current->getWidth();
                        yPos = this->height - current->getHeight();
                        break;
                    case CENTERED:
                        xPos = this->x + ((this->width/2.0)-(current->getWidth()/2.0));
                        yPos = (this->y + (this->height/2.0)) - current->getHeight()/2.0;
                        break;
                    case CENTERLEFT:
                        xPos = this->x;
                        yPos = (this->y + (this->height/2.0)) - current->getHeight()/2.0;
                        break;
                    case CENTERRIGHT:
                        xPos = (this->x + this->width) - current->getWidth();
                        yPos = (this->y + (this->height/2.0)) - current->getHeight()/2.0;
                        break;
                    case TOPCENTERED:
                        xPos = this->x + ((this->width/2.0) + (current->getWidth()/2.0));
                        yPos = this->y;
                        break;
                    case TOPLEFT:
                        xPos = this->x;
                        yPos = this->y;
                        break;
                    case TOPRIGHT:
                        xPos = (this->x + this->width) - current->getWidth();
                        yPos = this->y;
                        break;
                    default:
                        break;
                }
                current->draw(xPos, yPos);
            }
        };
};

#endif