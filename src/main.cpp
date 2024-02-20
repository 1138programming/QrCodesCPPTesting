#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include <iostream>
#include <fstream>


int main() {
    raylib::Window window(640,480,"I am mentally dumb", FLAG_WINDOW_RESIZABLE);
    QrCodeScanner qrScanner;

    // set up display stuff
    SizeScaling::init();
    Empty screen(raylib::Rectangle(0,0, GetScreenWidth(), GetScreenHeight()));
    Button gaming(200_spX, 100_spY, WHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Submit", 20.0_spX), WHITE));
    gaming.setDisplayPos(BOTTOMCENTERED);
    screen.add(&gaming);


    while(!window.ShouldClose()) {
        qrScanner.scan();

        window.BeginDrawing();
            std::cout << SizeScaling::xMult() << ", " << SizeScaling::yMult() << std::endl;
            screen.updateAndDrawAll(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight()));
            window.ClearBackground(BLACK);
        window.EndDrawing();
    }
    return 1;
}