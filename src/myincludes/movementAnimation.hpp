#ifndef MOVEMENTANIMATION_HPP
#define MOVEMENTANIMATION_HPP

#include "animation.hpp"
#include "raylib-cpp.hpp"
#include <iostream>

class MovementAnimation : public Animation {
    private:
        raylib::Vector2 targetTransformation;
        raylib::Vector2 initialTransformation;
    public:
        MovementAnimation(void* drawableObject, raylib::Vector2 targetTransformation) : Animation(drawableObject) {
            this->targetTransformation = targetTransformation;
            this->initialTransformation = this->animatedObject->getCustomOffset();
        }

        void update() override {
            float percentageDoneAdjusted = this->getCurrentEasingResult(this->getPercentageDone());
            raylib::Vector2 currentTransformation = raylib::Vector2(initialTransformation.x + ((targetTransformation.x - initialTransformation.x) * percentageDoneAdjusted), initialTransformation.y + ((targetTransformation.y - initialTransformation.y) * percentageDoneAdjusted));
            this->animatedObject->setCustomOffset(currentTransformation);
        }
        void initPos() {
            this->initialTransformation = this->animatedObject->getCustomOffset();
            std::cout << this->initialTransformation.y << std::endl;
        }
};

#endif