#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "drawable.hpp"
#include "easingFunctions.hpp"
#include "animationType.hpp"
#include "animationNumUnion.hpp"

template <ANIMATIONTYPE T> class Animation : public Drawable {
    private:
        Drawable* animatedObject;
        EASINGFUNCTION easingType = EASING_LINEAR;
        float duration = 1000.0f;
        ANIMATIONUNION target;
    public:
        Animation(void* drawableObject) {
            this->animatedObject = (Drawable*) drawableObject; // TODO: fix this terrible code lool
        }

        void setDuration(float duration) {
            this->duration = duration;
        }
        void setTarget(ANIMATIONUNION target) {
            this->target = target;
        }

};

#endif