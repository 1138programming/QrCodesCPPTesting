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

    TabHandler tabs(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));    
    // tab buttons
    Button main(0.0_spX, 0.0_spX, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Main Tab"), RAYWHITE, 20.0_spD, 0.0_spX));
    Button dataVisualization(0.0_spX, 0.0_spX, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Data Visualization"), RAYWHITE, 20.0_spD, 0.0_spX));
    tabs.add(&main);
    tabs.add(&dataVisualization);

    Empty screen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
    Button gaming(400.0_spX, 200.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Submit"), RAYWHITE, 40.0_spD, 0.0_spX));
    gaming.setDisplayPos(BOTTOMCENTERED);
    screen.add(&gaming);


    while(!window.ShouldClose()) {
        // calculating
        qrScanner.scan();
        if (gaming.isPressed()) {
            qrScanner.update();
        }

        // drawing
        window.BeginDrawing();
            screen.updateAndDrawAll(raylib::Rectangle(0, GetScreenHeight() * 0.2, GetScreenWidth(), GetScreenHeight() * 0.8));
            tabs.updateAndDrawAll(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.2));
            window.ClearBackground(BLACK);
        window.EndDrawing();
    }
    return 1;
}