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

        void add(Drawable* ref) {
            this->thingsToDraw.push_back(ref);
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

                        break;
                    case BOTTOMLEFT:

                        break;
                    case BOTTOMRIGHT:

                        break;
                    case CENTERED:

                        break;
                    case CENTERLEFT:

                        break;
                    case CENTERRIGHT:

                        break;
                    case TOPCENTERED:

                        break;
                    case TOPLEFT:
                        xPos = (int) this->emptySize.x;
                        yPos = (int) this->emptySize.y;
                        break;
                    case TOPRIGHT:

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