#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "drawable.hpp"

class Animation : public Drawable {
    private:
        Drawable* animatedObject;
        
    public:
        Animation(void* drawableObject) {
            this->animatedObject = (Drawable*) drawableObject; // TODO: fix this terrible code lool
        }
}

#endif