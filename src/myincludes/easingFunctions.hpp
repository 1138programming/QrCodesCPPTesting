// refer to: https://easings.net
#ifndef EASINGFUNCTIONS_HPP
#define EASINGFUNCTIONS_HPP

#define _USE_MATH_DEFINES
#include <cmath>

typedef enum {
    EASING_LINEAR,
    EASING_IN_SINE,
    EASING_OUT_SINE,
    EASING_IN_OUT_SINE
} EASINGFUNCTION;

float easingLinear(float progress) {
    return progress;
}
float easeInSine(float progress) {
    return (1 - cosf((progress * M_PI)/2.0f));
}
float easeOutSine(float progress) {
    return sin((progress * M_PI)/2.0f);
}
float easeInOutSine(float progress) {
    return -(cos(M_PI * progress) - 1.0f) / 2.0f;
}

#endif