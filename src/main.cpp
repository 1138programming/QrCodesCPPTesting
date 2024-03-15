
#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include "../myincludes/scenes.hpp"
#include "../include/json_fwd.hpp"
#include "myincludes/texturedButton.hpp"
#include "myincludes/toastHandler.hpp"
#include "myincludes/texture.hpp"
#include "myincludes/pong.hpp"
#include "myincludes/database.hpp"
#include "myincludes/graph.hpp"
#include "myincludes/bluetooth.hpp"
#include "myincludes/clientForTesting.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>



int main() {
     MYSQL_RES res;
    //  MYSQL_ROWS sqldata;
     MYSQL_ROW row;
     std::string resultstr;
   
     
    // DrawableGraph<double, double> graph(200, 200, 400.0_spX, 400.0_spY);

    // _____ Constant Things _____
    raylib::Window window(1280,720,"Scouting App Computer UI", FLAG_WINDOW_RESIZABLE);  
    Pong pongame = Pong(&window);
    window.SetTargetFPS(2000);
    bool highFPS = true;

    QrCodeScanner qrScanner;
    raylib::Font spaceCadet(std::string("resources/SM.TTF"));
    Bluetooth btConn;
    Client client;
    int sucksess = btConn.initAll();
    if (sucksess != 0) {
        std::cerr << "init failed" << std::endl;
        btConn.cleanup();
    }

    SCENES currentScene = SCANNING;
    Database database;
    // set up display + constant scene stuff
    SizeScaling::init();
    // set up tabs at top of screen
    TabHandler tabs(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));    
    // tab buttons
    Button main(0.0, 0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Main Tab"), RAYWHITE, 18.0_spD, 0.0));
    Button dataVisualization(0.0, 0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Data Visualization"), RAYWHITE, 18.0_spD, 0.0));
    Button matchConfiguration(0.0,0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Match Config"), RAYWHITE, 18.0_spD, 0.0));
    Button BT(0.0,0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "BT"), RAYWHITE, 18.0_spD, 0.0));
    main.disable();
    tabs.add(&main)
        .add(&dataVisualization)
        .add(&matchConfiguration)
        .add(&BT);

    // _____ Setting up Scenes _____
    DrawableTexture texture(1280.0_spX, 720.0_spY, raylib::Image("resources/bg.png"), raylib::Color(100, 100, 100));
    // __ Scanner Scene __
        Empty scannerScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        Empty matchConfigurationScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        Empty pongscreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        TexturedButton goated(400.0_spX, 200.0_spY, raylib::Image("resources/submit-button.png"), raylib::Image("resources/submit-button-hover.png"));
        Button DB(200.0_spX,100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "BD"), RAYWHITE, 18.0_spD, 0.0));
        Button lowPowerMode(200.0_spX, 100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Low Power"), RAYWHITE, 14.0_spD, 0.0));
        Button pong(10.0, 10.0, DARKBLUE, DARKBLUE, DARKGRAY, EzText(raylib::Text(spaceCadet, "_"), RAYWHITE, 5.0_spD, 0.0));
        Button pongback(100.0, 100.0, BLACK, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Back"), RAYWHITE, 10.0_spD, 0.0));
        lowPowerMode.setDisplayPos(BOTTOMRIGHT);
        goated.setDisplayPos(CENTERED);
        DB.setDisplayPos(BOTTOMLEFT);
        pong.setDisplayPos(TOPRIGHT);
        pongback.setDisplayPos(BOTTOMRIGHT);
        scannerScreen.add(&goated);
        scannerScreen.add(&DB);
        scannerScreen.add(&lowPowerMode);
        matchConfigurationScreen.add(&pong);
        pongscreen.add(&pongback);

        //std::string fileName = "resources/submit.png";
        // TexturedButton goated(400.0_spX, 200.0_spY, raylib::Image(fileName), raylib::Image(fileName));
    // __  Data Visualization Scene __
        Empty dataVisualizationScreen(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));
    // __ BT Testing Scene __
        Empty btTestingScene(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));
        Button btServer(200.0_spX, 100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "BT Server"), RAYWHITE, 18.0_spD, 0.0));
        Button btClient(200.0_spX, 100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "BT Client"), RAYWHITE, 18.0_spD, 0.0));
        btServer.setDisplayPos(BOTTOMLEFT);
        btClient.setDisplayPos(BOTTOMRIGHT);
        btTestingScene.add(&btServer);
        btTestingScene.add(&btClient);



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

            main.disable();
            dataVisualization.enable();
            matchConfiguration.enable();
        }
        else if (dataVisualization.isPressed()) {
            currentScene = DATAVISUALIZATION;
            main.enable();
            dataVisualization.disable();
            matchConfiguration.enable();
        }
        else if (matchConfiguration.isPressed()) {
            currentScene = MATCHCONFIG;

            main.enable();
            dataVisualization.enable();
            matchConfiguration.disable();
        }
        else if(BT.isPressed()) {
            currentScene = BLUETOOTHTESTING;
        }

        switch(currentScene) {
            case SCANNING:
                // calculating
                qrScanner.scan();
                if (goated.isPressed()) {
                    qrScanner.update();
                }
                if(DB.isPressed()) {
                    auto vector = database.execQuery("select DatapointID,DCValue,DCTimestamp from matchtransaction;", 3);
                     std::cout << "hhh" << std::endl;
                    for (auto i : vector) {
                        for (std::string j : i) {
                            std::cout << j << std::endl;
                        }
                    }
                    // std::cout << resultstr <<std::endl;
                    // res = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DataPointID,  DCValue, TeamID,AllianceID) values (1,1,2,'hello', 1, 'Blue');");  
                    
                }
                  
           
                
                    
                  

                
                if (lowPowerMode.isPressed()) {
                    highFPS = !highFPS;
                    if (highFPS) {
                        window.SetTargetFPS(10000);
                    }
                    else {
                        window.SetTargetFPS(24);
                    }
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
                    window.ClearBackground(BLACK);
                    texture.draw(0,0);
                    dataVisualizationScreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
            break;

            case MATCHCONFIG:
                if (pong.isPressed()) {
                    std::cout << "hello" << std::endl;
                    currentScene = PONG;
                }
                window.BeginDrawing();
                    window.ClearBackground(DARKBLUE);
                matchConfigurationScreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
               
            break;
            case PONG:
                window.BeginDrawing();
                window.ClearBackground(BEIGE);
                pongame.update();
                DrawCircle(pongame.Ballpos.x,pongame.Ballpos.y,20.0f,BLACK);
                DrawRectangle(pongame.Paddle1pos.x,pongame.Paddle1pos.y,20.0f,80.0f,BLACK);
                DrawRectangle(pongame.Paddle2pos.x,pongame.Paddle2pos.y,20.0f,80.0f,BLACK);
                main.disable();
                dataVisualization.disable();
                matchConfiguration.disable();
                if (pongback.isPressed()) {
                    currentScene = SCANNING;

                    main.disable();
                    dataVisualization.enable();
                    matchConfiguration.enable();
                }
                pongscreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
            break;
            case BLUETOOTHTESTING:
                if (btClient.isPressed()) {
                    std::cerr << client.connectToServer() << std::endl;
                }
                if (btServer.isPressed()) {
                    std::cerr << btConn.startAccept() << std::endl;
                }
                window.BeginDrawing();
                window.ClearBackground(BLACK);
                btTestingScene.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
            break;
        }
        if (currentScene != PONG) {
           
            tabs.updateAndDraw(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));
  

        }
//  std::cout << GetFrameTime() << std::endl;

        window.DrawFPS();
        toastHandler::update();
        window.EndDrawing();
            
    }
    return 0;
}