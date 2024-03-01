#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include "../myincludes/scenes.hpp"
#include "../include/json_fwd.hpp"
#include "myincludes/texturedButton.hpp"
#include "myincludes/toastHandler.hpp"
#include "myincludes/texture.hpp"
#include <iostream>
#include <fstream>

int main() {
    // _____ Constant Things _____
    raylib::Window window(1280,720,"Scouting App Computer UI", FLAG_WINDOW_RESIZABLE);
    window.SetTargetFPS(480);
    
    QrCodeScanner qrScanner;
    SCENES currentScene = SCANNING;

    // set up display + constant scene stuff
    SizeScaling::init();
    // set up tabs at top of screen
    TabHandler tabs(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));    
    // tab buttons
    Button main(0.0, 0.0, RAYWHITE, raylib::Color(0, 0, 0, 190), raylib::Color(0, 0, 0, 50), EzText(raylib::Text(GetFontDefault(), "Main Tab"), RAYWHITE, 20.0_spD, 0.0));
    Button dataVisualization(0.0, 0.0, RAYWHITE, raylib::Color(0, 0, 0, 190), raylib::Color(0, 0, 0, 50), EzText(raylib::Text(GetFontDefault(), "Data Visualization"), RAYWHITE, 20.0_spD, 0.0));
    Button matchConfiguration(0.0,0.0, RAYWHITE, raylib::Color(0, 0, 0, 190), raylib::Color(0, 0, 0, 50), EzText(raylib::Text(GetFontDefault(), "Match Config"), RAYWHITE, 20.0_spD, 0.0));
    tabs.add(&main)
        .add(&dataVisualization)
        .add(&matchConfiguration);

    // _____ Setting up Scenes _____
    DrawableTexture texture(1280.0_spX, 720.0_spY, raylib::Image("resources/maxresdefault.png"), raylib::Color(100, 100, 100));
    // __ Scanner Scene __
    Empty scannerScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
    TexturedButton goated(200.0_spX, 114.0_spY, raylib::Image("resources/submit.png"), raylib::Image("resources/submit.png"));
    goated.setDisplayPos(BOTTOMCENTERED);
    scannerScreen.add(&goated);


    while(!window.ShouldClose()) {
        // make application fullscreen on f11 press (and set resolution)
        if (IsKeyPressed(KEY_F11)) {
            if (!window.IsFullscreen()) {
                window.SetSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
                window.SetFullscreen(true);
            }
            else {
                window.SetFullscreen(false);
                window.SetSize(1280,720);
            }
        }

        if (main.isPressed()) {
            currentScene = SCANNING;
        }
        else if (dataVisualization.isPressed()) {
            currentScene = DATAVISUALIZATION;
        }
        else if (matchConfiguration.isPressed()) {
            currentScene = MATCHCONFIG;
        }

        switch(currentScene) {
            case SCANNING:
                // calculating
                qrScanner.scan();
                if (goated.isPressed()) {
                    qrScanner.update();
                }
                // drawing
                window.BeginDrawing();
                    window.ClearBackground(BLACK);
                    texture.draw(0,0);
                    scannerScreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
                // calling endDrawing later
            break;

            case DATAVISUALIZATION:
                window.BeginDrawing();
                    window.ClearBackground(DARKGREEN);
            break;

            case MATCHCONFIG:
                window.BeginDrawing();
                    window.ClearBackground(DARKBLUE);
            break;

        }
        tabs.updateAndDraw(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));
        window.DrawFPS();
        toastHandler::update();
        window.EndDrawing();
    }
    return 1;
}