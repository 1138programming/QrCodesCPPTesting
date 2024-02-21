#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include <iostream>
#include <fstream>


int main() {
    raylib::Window window(1280,720,"I am mentally dumb", FLAG_WINDOW_RESIZABLE);
    QrCodeScanner qrScanner;

    // set up display stuff
    SizeScaling::init();
    Empty screen(raylib::Rectangle(0,0, GetScreenWidth(), GetScreenHeight()));
    Button gaming(400.0_spX, 200.0_spY, WHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Submit"), WHITE, 40.0_spX, 0.0_spX));
    gaming.setDisplayPos(BOTTOMCENTERED);
    screen.add(&gaming);


    while(!window.ShouldClose()) {
        qrScanner.scan();

        window.BeginDrawing();
            screen.updateAndDrawAll(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight()));
            window.ClearBackground(BLACK);
        window.EndDrawing();
    }
    return 1;
}