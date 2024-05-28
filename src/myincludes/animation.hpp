#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "drawable.hpp"
#include "easingFunctions.hpp"
#include "animationType.hpp"

template <unsigned int T> class Animation : public Drawable {
    private:
        Drawable* animatedObject;
        EASINGFUNCTION easingType;
        ANIMATIONTYPE a = T;
    public:
        Animation(void* drawableObject) {
            this->animatedObject = (Drawable*) drawableObject; // TODO: fix this terrible code lool
        }
};

#endif