#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include "../myincludes/scenes.hpp"
#include "../include/json_fwd.hpp"
#include "myincludes/texturedButton.hpp"
#include "myincludes/toastHandler.hpp"
#include "myincludes/database.hpp"
#include <iostream>
#include <fstream>

int main() {
    // _____ Constant Things _____
    raylib::Window window(1280,720,"I am mentally dumb", FLAG_WINDOW_RESIZABLE);
    window.SetTargetFPS(240);
    QrCodeScanner qrScanner;
    SCENES currentScene = SCANNING;
    Database database;
    // set up display + constant scene stuff
    SizeScaling::init();
    // set up tabs at top of screen
    TabHandler tabs(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));    
    // tab buttons
    Button main(0.0, 0.0, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Main Tab"), RAYWHITE, 20.0_spD, 0.0));
    Button dataVisualization(0.0, 0.0, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Data Visualization"), RAYWHITE, 20.0_spD, 0.0));
    Button matchConfiguration(0.0,0.0, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Match Config"), RAYWHITE, 20.0_spD, 0.0));

    tabs.add(&main)
        .add(&dataVisualization)
        .add(&matchConfiguration);

    // _____ Setting up Scenes _____

    // __ Scanner Scene __
    Empty scannerScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
    Button goated(400.0_spX, 200.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "Submit"), RAYWHITE, 40.0_spD, 0.0_spX));
    Button DB(200.0_spX,100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(GetFontDefault(), "BD"), RAYWHITE, 20.0_spD, 0.0));

    //std::string fileName = "resources/submit.png";
   // TexturedButton goated(400.0_spX, 200.0_spY, raylib::Image(fileName), raylib::Image(fileName));
    goated.setDisplayPos(BOTTOMCENTERED);
    DB.setDisplayPos(BOTTOMLEFT);
    scannerScreen.add(&goated);
    scannerScreen.add(&DB);


    while(!window.ShouldClose()) {
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
                if(DB.isPressed()) {
                    database.innit();
                }
                // drawing
                window.BeginDrawing();
                    window.ClearBackground(BLACK);
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