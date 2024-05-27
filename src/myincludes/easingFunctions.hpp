// refer to: https://easings.net
#ifndef EASINGFUNCTIONS_HPP
#define EASINGFUNCTIONS_HPP

#define _USE_MATH_DEFINES
#include <cmath>

typedef struct {
    EASING_LINEAR,
    EASING_IN_SINE
} EASINGFUNCTIONS;

float easingLinear(float progress) {
    return progress;
}
float easeInSine(float progress) {
    return (1 - cosf((progress * M_PI)/2.0f))
}

#endif