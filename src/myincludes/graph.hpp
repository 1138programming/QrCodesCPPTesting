#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include "graphPoint.hpp"
#include "drawable.hpp"
#include "shouldScale.hpp"

template<typename X, typename Y> class DrawableGraph : public Drawable {
    private:
        ShouldScale x, y, width, height;
        std::vector<GraphPoint<X,Y>> datapoints;
    public:

        DrawableGraph(ShouldScale x, ShouldScale y, ShouldScale width, ShouldScale height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            datapoints.res
        }

        void add(GraphPoint<X,Y> point) {
            this->datapoints.push_back(point);
        }
        void addMultiple(GraphPoint<X,Y>* points, size_t size) {
            // reserve data so adding will be quicker
            this->datapoints.reserve(this->datapoints.size + size);

            for (size_t i = 0; i < size; i++) {
                this->datapoints.reserve(points[i]);
            }
        }
        void addMultiple(std::vector<GraphPoint<T,Y>>& points) {
            // reserve data so adding will be quicker
            this->datapoints.reserve(this->datapoints.size + points.size());

            for (GraphPoint<X,Y> point : points) {
                this->datapoints.push_back(point);
            }
        }
        void addMultipleNoReserve(std::vector<GraphPoint<T,Y>>& points) {
            for(GraphPoint<X,Y> point : points) {
                this->datapoints.push_back(point);
            }
        }

        X minX() {
            X min = this->datapoints.at(0);
            for (X point : this->datapoints) {
                if (point < min) {
                    min = point;
                }
            }
            return min;
        }
        X maxX() {
            X max = this->datapoints.at(0).x;
            for (GraphPoint<X,Y> point : this->datapoints) {
                if (point.x > max.x) {
                    max = point.x;
                }
            }
            return max;
        }

        Y minY() {
            Y max = this->datapoints.at(0);
            for (Y point : this->datapoints)
        }
        Y maxY() {

        }

        ShouldScale getWidth() override {
            return this->width;
        }
        ShouldScale getHeight() override {
            return this->height;
        }
};

#endif