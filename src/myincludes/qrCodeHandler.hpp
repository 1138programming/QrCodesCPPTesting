#ifndef QRCODEHANDLER_HPP
#define QRCODEHANDLER_HPP

#include <string>
#include "../include/qrcodegen.hpp"
#include "../include/raylib-cpp.hpp"
#include "../myincludes/drawable.hpp"

class QrCodeHandler : public Drawable {
    private:
        RenderTexture2D qrCodeTexture;
        qrcodegen::QrCode qrData;
        float previousSize;
        ShouldScale size;
    public:
        QrCodeHandler(std::string text, qrcodegen::QrCode::Ecc errorLevel, ShouldScale size) : qrData(qrcodegen::QrCode::encodeText(text.c_str(), errorLevel)) {
            this->size = size;
            this->previousSize = 0;
            this->qrCodeTexture = LoadRenderTexture(size, size);
        }

        void draw(int x, int y) override {
            if (previousSize != (float)size) {
                std::cout << "haha";
                UnloadRenderTexture(this->qrCodeTexture);
                this->qrCodeTexture = LoadRenderTexture(size, size);
                this->previousSize = (float)size;
                BeginTextureMode(this->qrCodeTexture);
                    ClearBackground(WHITE);
                    for (int i = 0; i < size-1; i++) {
                        for (int j = 0; j < size-1; j++) {
                            if (j == 0 || i == 0) {
                                DrawPixel(j, i, WHITE);
                            }
                            else {
                                DrawPixel(j, i, (qrData.getModule(qrData.getSize() * (j/(size-2)), qrData.getSize() * (i/(size-2))) ? BLACK : WHITE));
                            }
                        }
                    }
                EndTextureMode();
            }

            DrawTextureRec(this->qrCodeTexture.texture, (Rectangle) { 0, 0, (float)this->qrCodeTexture.texture.width, (float)-this->qrCodeTexture.texture.height}, (Vector2) {(float)x, (float)y}, WHITE);
        }

        void setWidth(ShouldScale width) override {
            this->size = width;
        }
        void setHeight(ShouldScale height) { 
            this->size = height;
        }

        ShouldScale getWidth() override {
            return this->size;
        }
        ShouldScale getHeight() override {
            return this->size;
        }
};

#endif