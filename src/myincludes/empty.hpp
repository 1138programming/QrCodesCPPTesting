#ifndef EMPTY_HPP
#define EMPTY_HPP

#include <vector>
#include "drawable.hpp"

class Empty {
    private:
        std::vector<Drawable*> thingsToDraw;
        Rectangle emptySize;
    public:
        Empty(Rectangle size) {
            this->emptySize = size;
        }

        void add(void* ref) {
            Drawable* refAsDrawable = (Drawable*)ref;
            this->thingsToDraw.push_back(refAsDrawable);
        }

        void drawAll() {
            if (this->thingsToDraw.size() == 0) {
                return;
            }
            
            int xPos;
            int yPos;
            for (int i = 0; i < this->thingsToDraw.size(); i++) {
                Drawable* current = this->thingsToDraw.at(i);
                
                switch(current->getDisplayPos()) {
                    case BOTTOMCENTERED:
                        xPos = this->emptySize.x + ((this->emptySize.width/2.0)-(current->getWidth()/2.0));
                        yPos = this->emptySize.height - current->getHeight();
                        break;
                    case BOTTOMLEFT:
                        xPos = this->emptySize.x;
                        yPos = this->emptySize.height - current->getHeight();
                        break;
                    case BOTTOMRIGHT:
                        xPos = this->emptySize.width - current->getWidth();
                        yPos = this->emptySize.height - current->getHeight();
                        break;
                    case CENTERED:
                        xPos = this->emptySize.x + ((this->emptySize.width/2.0)-(current->getWidth()/2.0));
                        yPos = (this->emptySize.y + (this->emptySize.height/2.0)) - current->getHeight()/2.0;
                        break;
                    case CENTERLEFT:
                        xPos = this->emptySize.x;
                        yPos = (this->emptySize.y + (this->emptySize.height/2.0)) - current->getHeight()/2.0;
                        break;
                    case CENTERRIGHT:
                        xPos = (this->emptySize.x + this->emptySize.width) - current->getWidth();
                        yPos = (this->emptySize.y + (this->emptySize.height/2.0)) - current->getHeight()/2.0;
                        break;
                    case TOPCENTERED:
                        xPos = this->emptySize.x + ((this->emptySize.width/2.0) + (current->getWidth()/2.0));
                        yPos = this->emptySize.y;
                        break;
                    case TOPLEFT:
                        xPos = this->emptySize.x;
                        yPos = this->emptySize.y;
                        break;
                    case TOPRIGHT:
                        xPos = (this->emptySize.x + this->emptySize.width) - current->getWidth();
                        yPos = this->emptySize.y;
                        break;
                    default:
                        break;
                }
                current->draw(xPos, yPos);
            }
        }
        void updateAndDrawAll(Rectangle size) {
            this->emptySize = size;
            drawAll();
        }
};

#endif