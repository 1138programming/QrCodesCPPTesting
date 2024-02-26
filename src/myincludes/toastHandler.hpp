#ifndef TOASTHANDLER_HPP
#define TOASTHANDLER_HPP

#include "toast.hpp"
#include <queue>

class toastHandler {
    public:
        static std::queue<Toast> toastQueue;
        static Toast currentToast;

        inline static void add(Toast toast) {
            toastHandler::toastQueue.push(toast);
        }
        inline static void update() {
            if (toastHandler::toastQueue.size() == 0 && toastHandler::currentToast.isDone()) {
                return;
            }
            if (toastHandler::currentToast.isDone()) {
                toastHandler::currentToast = toastHandler::toastQueue.front();
                toastHandler::toastQueue.pop();
                toastHandler::currentToast.init();
            }

            toastHandler::currentToast.draw(GetScreenWidth()/2.0 - GetScreenWidth()/10.0, 0);
        }

};
Toast toastHandler::currentToast = Toast("", LENGTH_NULL);
std::queue<Toast> toastHandler::toastQueue = std::queue<Toast>();

#endif