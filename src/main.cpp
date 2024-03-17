
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
#include "myincludes/toggle.hpp"
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
    btConn.initAll();
    btConn.initAccept();

    SCENES currentScene = SCANNING;
    Database database;
    // set up display + constant scene stuff
    SizeScaling::init();
    // set up tabs at top of screen
    TabHandler tabs(raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight() * 0.15));    
    // tab buttons
    Button mainTab(0.0, 0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Main Tab"), RAYWHITE, 18.0_spD, 0.0));
    Button databaseTab(0.0, 0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Database"), RAYWHITE, 18.0_spD, 0.0));
    Button bluetoothTab(0.0,0.0, RAYWHITE, raylib::Color(255, 255, 255, 10), raylib::Color(255, 255, 255, 40), EzText(raylib::Text(spaceCadet, "Bluetooth"), RAYWHITE, 18.0_spD, 0.0));
    mainTab.disable();
    tabs.add(&mainTab)
        .add(&databaseTab)
        .add(&bluetoothTab);

    // _____ Setting up Scenes _____
    DrawableTexture texture(1280.0_spX, 720.0_spY, raylib::Image("resources/bg.png"), raylib::Color(100, 100, 100));
    // __ Scanner Scene __
        Empty scannerScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        Empty matchConfigurationScreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        Empty pongscreen(raylib::Rectangle(0,GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
        TexturedButton goated(400.0_spX, 200.0_spY, raylib::Image("resources/submit-button.png"), raylib::Image("resources/submit-button-hover.png"));
        Button DB(200.0_spX,100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "BD"), RAYWHITE, 18.0_spD, 0.0));
        Button lowPowerMode(200.0_spX, 100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Low Power"), RAYWHITE, 14.0_spD, 0.0));
        Button pong(10.0, 10.0, raylib::Color(0,0,0,0), raylib::Color(0,0,0,0), GRAY, EzText(raylib::Text(spaceCadet, "_"), RAYWHITE, 5.0_spD, 0.0));
        Button pongback(100.0, 100.0, BLACK, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Back"), RAYWHITE, 10.0_spD, 0.0));
        lowPowerMode.setDisplayPos(BOTTOMRIGHT);
        goated.setDisplayPos(BOTTOMCENTERED);
        DB.setDisplayPos(BOTTOMLEFT);
        pong.setDisplayPos(TOPRIGHT);
        pongback.setDisplayPos(BOTTOMRIGHT);
        scannerScreen.add(&goated);
        scannerScreen.add(&lowPowerMode);
        pongscreen.add(&pongback);


    // __  Database Scene __
        Empty dataVisualizationScreen(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));
        TextBox box(100.0_spX, 50.0_spY, 10, 15.0_spD);
            box.setDisplayPos(CENTERED);
        dataVisualizationScreen.add(&DB)
            .add(&box);
    // __ BT Scene __
        Empty btTestingScene(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));       
        btTestingScene.add(&pong);
        EzText discoveryText(raylib::Text(spaceCadet, "BT Discovery Enable:"), RAYWHITE, 12.0_spD, 0.0);
        EzText talkingText(raylib::Text(spaceCadet, "BT Comms Enable:"), RAYWHITE, 12.0_spD, 0.0);
        Toggle discoveryToggle(50.0_spX, 50.0_spY, 0.75, RAYWHITE);
        Toggle talkingToggle(50.0_spX, 50.0_spY, 0.75, RAYWHITE);
        DrawableList discoveryList(VERTICAL, 0);
            discoveryList.add(&discoveryText);
            discoveryList.add(&discoveryToggle);
            discoveryList.setDisplayPos(BOTTOMLEFT);
        DrawableList talkingList(VERTICAL, 0);
            talkingList.add(&talkingText);
            talkingList.add(&talkingToggle);
            talkingList.setDisplayPos(BOTTOMRIGHT);
        EzText macAddress(raylib::Text(spaceCadet, "Mac: " + btConn.getLocalMacStr()), RAYWHITE, 12.0_spD, 0.0);
        EzText port(raylib::Text(spaceCadet, "Port: " + std::to_string(btConn.getLocalPort())), RAYWHITE, 12.0_spD, 0.0);
        EzText activeConnections(raylib::Text(spaceCadet, "Connections: " + std::to_string(btConn.getNumConnections())), RAYWHITE, 12.0_spD, 0.0);
        DrawableList serverData(VERTICAL, 0);
            serverData.add(&macAddress);
            serverData.add(&port);
            serverData.add(&activeConnections);
            serverData.setDisplayPos(CENTERED);
        btTestingScene.add(&serverData);
        btTestingScene.add(&discoveryList);
        btTestingScene.add(&talkingList);



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

        if (mainTab.isPressed()) {
            currentScene = SCANNING;

            mainTab.disable();
            databaseTab.enable();
            bluetoothTab.enable();
        }
        else if (databaseTab.isPressed()) {
            currentScene = DATABASE;
            mainTab.enable();
            databaseTab.disable();
            bluetoothTab.enable();
        }
        else if (bluetoothTab.isPressed()) {
            currentScene = BLUETOOTH;

            mainTab.enable();
            databaseTab.enable();
            bluetoothTab.disable();
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

            case DATABASE:
                window.BeginDrawing();
                    window.ClearBackground(BLACK);
                    texture.draw(0,0);
                    dataVisualizationScreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
            break;

            case BLUETOOTH:
                if (pong.isPressed()) {
                    std::cout << "hello" << std::endl;
                    currentScene = PONG;
                }
                if (discoveryToggle.isChecked()) {
                    btConn.updateConnections();
                    activeConnections.setText("Connections: " + std::to_string(btConn.getNumConnections()));
                }
                if (talkingToggle.isChecked()) {
                    btConn.handleReadyConnections();
                }
                window.BeginDrawing();
                window.ClearBackground(BLACK);
                btTestingScene.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));               
            break;
            case PONG:
                window.BeginDrawing();
                window.ClearBackground(BEIGE);
                pongame.update();
                DrawCircle(pongame.Ballpos.x,pongame.Ballpos.y,20.0f,BLACK);
                DrawRectangle(pongame.Paddle1pos.x,pongame.Paddle1pos.y,20.0f,80.0f,BLACK);
                DrawRectangle(pongame.Paddle2pos.x,pongame.Paddle2pos.y,20.0f,80.0f,BLACK);
                mainTab.disable();
                databaseTab.disable();
                bluetoothTab.disable();
                if (pongback.isPressed()) {
                    currentScene = BLUETOOTH;

                    mainTab.disable();
                    databaseTab.enable();
                    bluetoothTab.enable();
                }
                pongscreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
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