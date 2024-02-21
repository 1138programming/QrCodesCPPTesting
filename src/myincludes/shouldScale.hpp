#ifndef SHOULDSCALE_HPP
#define SHOULDSCALE_HPP

#include "shouldScaleDependencies.hpp"
#include "sizeScaling.hpp"

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
        ShouldScale(float data, bool shouldScale, SHOULDSCALEDEPENDENCIES dependency) {
            this->shouldScale = shouldScale;
            this->data = data;
            this->dependency = dependency;
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
        ShouldScale(const char* a) {
            bool* dataBool = (bool*)a;
            SHOULDSCALEDEPENDENCIES* dataEnum = (SHOULDSCALEDEPENDENCIES*)(&(dataBool[1]));
            long double* dataNum = ((long double*)&(dataEnum[1]));

            this->shouldScale = *dataBool;
            this->dependency = *dataEnum;
            this->data = *dataNum;
            free((void*)a);
        }

        long double getData() {
            if (this->shouldScale) {
                switch (this->dependency)
                {
                    case XDEPENDENT:
                        return this->data * SizeScaling::xMult();
                    break;
                    case YDEPENDENT:
                        return this->data * SizeScaling::yMult();
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