#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "drawable.hpp"
#include "easingFunctions.hpp"
#include <iostream>

class Animation : public Drawable {
    protected:
        Drawable* animatedObject;
        EASINGFUNCTION easingType = EASING_LINEAR;
        float duration = 1.0f;
        double startTime = 0.0f;
        bool done = false;

        float getPercentageDone() {
            float percentageDone = (float)(GetTime() - startTime) / duration;
            if (percentageDone > 1.0f) {
                percentageDone = 1.0f;
                done = true;
            }
            return percentageDone;
        }
        float getCurrentEasingResult(float progress) {
            switch(this->easingType) {
                case EASING_LINEAR:
                    return easingLinear(progress);
                    break;
                case EASING_IN_SINE:
                    return easeInSine(progress);
                    break;
                case EASING_OUT_SINE:
                    return easeOutSine(progress);
                    break;
                case EASING_IN_OUT_SINE:
                    return easeInOutSine(progress);
                    break;
            }
            std::cerr << "Problem in getCurrentEasingResult() function" << std::endl;
            return easingLinear(progress);
        }
    public:
        Animation(void* drawableObject) {
            this->animatedObject = (Drawable*) drawableObject; // TODO: fix this terrible code loool
        }

        void setDuration(float duration) {
            this->duration = duration;
        }
        void setEasing(EASINGFUNCTION easing) {
            this->easingType = easing;
        }
        void start() {
            this->startTime = GetTime();
        }
        bool isDone() {
            return this->done;
        }

        virtual void update() {} // child class logic
};

#endif