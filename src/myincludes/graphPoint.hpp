#ifndef GRAPHPOINT_HPP
#define GRAPHPOINT_HPP

template<typename X, typename Y> class GraphPoint {
    public:
        X x;
        Y y;
        GraphPoint(X x, Y y) {
            this->x = x;
            this->y = y;
        }
};

#endif