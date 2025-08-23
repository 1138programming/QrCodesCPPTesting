#ifndef QRCODEHANDLER_HPP
#define QRCODEHANDLER_HPP

#include <string>
#include "../include/qrcodegen.hpp"
#include "../include/raylib-cpp.hpp"
#include "../myincludes/drawable.hpp"

class QrCodeHandler : public Drawable {
    private:
        qrcodegen::QrCode qrData;
        ShouldScale size;
    public:
        QrCodeHandler(std::string text, qrcodegen::QrCode::Ecc errorLevel, ShouldScale size) : qrData(qrcodegen::QrCode::encodeText(text.c_str(), errorLevel)) {
            this->size = size;
        }

        void draw(int x, int y) override {
            for (int i = 0; i < size-1; i++) {
                for (int j = 0; j < size-1; j++) {
                    if (j == 0 || i == 0) {
                        DrawPixel(x + j, y + i, WHITE);
                    }
                    else {
                        DrawPixel(x + j, y + i, (qrData.getModule(qrData.getSize() * (j/(size-2)), qrData.getSize() * (i/(size-2))) ? BLACK : WHITE));
                    }
                }
            }
        }

        void setWidth(ShouldScale width) override {
            this->size = width;
        }
        void setHeight(ShouldScale height) { 
            this->size = height;
        }

        virtual ShouldScale getWidth() override {
            return this->size;
        }
        virtual ShouldScale getHeight() {
            return this->size;
        }
};

#endif