#ifndef SHOULDSCALE_HPP
#define SHOULDSCALE_HPP

#include "sizeScaling.hpp"
#include "shouldScaleDependencies.hpp"

class ShouldScale {
    private:
        long double data;
        bool shouldScale;
        SHOULDSCALEDEPENDENCIES dependency;
    public:
        ShouldScale() {
            this->data = 0.0;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }
        ShouldScale(int& a) {
            this->data = a;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }
        ShouldScale(long& a) {
            this->data = a;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }
        ShouldScale(float& a) {
            this->data = a;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }
        ShouldScale(double& a) {
            this->data = a;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }
        ShouldScale(long double& a) {
            this->data = a;
            this->shouldScale = false;
            this->dependency = NODEPENDENCY;
        }

        long double getData() {
            if (this->shouldScale) {
                switch (this->dependency)
                {
                    case XDEPENDENT:
                        return SizeScaling::round(this->data * SizeScaling::xMult());
                    break;
                    case YDEPENDENT:
                        return SizeScaling::round(this->data * SizeScaling::yMult());
                    break;
                    default:
                        return this->data;
                    break;
                }
            }
            else {
                return this->data;
            }
        }
};

#endif