#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
#include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include "../myincludes/scenes.hpp"
#include "../myincludes/datapoints.hpp"
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
#include "myincludes/verticalScrollable.hpp"
#include "myincludes/timer.hpp"
#include "myincludes/WinsockErrorDesc.hpp"
#include "myincludes/bthSocketHandler.hpp"
#include "myincludes/movementAnimation.hpp"
#include "myincludes/debugConsole.hpp"
#include "myincludes/restReqHandler.hpp"
#include "myincludes/bluetooth/bluetooth.hpp"
#include "myincludes/bluetooth/btTabObj.hpp"
#include "myincludes/bluetooth/bluetoothConductor.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>



int main() {
    MYSQL_RES res;
    //  MYSQL_ROWS sqldata;
    MYSQL_ROW row;
    std::string resultstr;
   
    DebugConsole::print("Welcome to the main computer!\n", DBGC_BLUE);
     
    // DrawableGraph<double, double> graph(200, 200, 400.0_spX, 400.0_spY);

    // _____ Constant Things _____
    raylib::Window window(1280,720,"Scouting App Computer UI", FLAG_WINDOW_RESIZABLE);  
        window.SetConfigFlags(FLAG_VSYNC_HINT);
        window.SetConfigFlags(FLAG_MSAA_4X_HINT);
        window.SetIcon(raylib::Image("resources/eagleEngineeringLogoLowRes.png"));
    Pong pongame = Pong(&window);
    window.SetTargetFPS(1000000);

    raylib::Font spaceCadet(std::string("resources/SM.TTF"));
    raylib::Font comicSans(std::string("resources/ComicMono.ttf"));
    raylib::Font spaceMono(std::string("resources/SpaceMono-Bold.ttf"));
    Bluetooth btConn;
    Client client;
    btConn.initWinsock();
    btConn.initAccept();

    RestReqHandler handler;        

    SCENES currentScene = SCANNING;
    Database database;
        database.query("Hello?", ", world!");
        database.query("select * from matchtransaction where id=?", "1");
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
        Button AmplifyBlue(300.0_spX,100.0_spY, RAYWHITE, BLUE, DARKGRAY, EzText(raylib::Text(spaceCadet, "AmplifyBlue"), RAYWHITE, 12.0_spD, 0.0));
        Button AmplifyRed(300.0_spX,100.0_spY, RAYWHITE, RED, DARKGRAY, EzText(raylib::Text(spaceCadet, "AmplifyRed"), RAYWHITE, 12.0_spD, 0.0));
        Button pong(10.0, 10.0, raylib::Color(0,0,0,0), raylib::Color(0,0,0,0), GRAY, EzText(raylib::Text(spaceCadet, "_"), RAYWHITE, 5.0_spD, 0.0));
        Button pongback(100.0, 100.0, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Back"), RAYWHITE, 10.0_spD, 0.0));
        Button rest(100.0_spX, 100.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "rest"), RAYWHITE, 10.0_spD, 0.0));

        goated.setDisplayPos(BOTTOMCENTERED);
        DB.setDisplayPos(BOTTOMLEFT);
        pong.setDisplayPos(TOPRIGHT);
        pongback.setDisplayPos(BOTTOMRIGHT);
        AmplifyBlue.setDisplayPos(CENTERLEFT);
        AmplifyRed.setDisplayPos(CENTERRIGHT);
        TextBox MatchBoxMain(100.0_spX, 50.0_spY, 10, 0.0, 15.0_spD, spaceMono, WHITE, WHITE);
        MatchBoxMain.setDisplayPos(TOPCENTERED);
        rest.setDisplayPos(TOPCENTERED);

        scannerScreen.add(&goated);
        scannerScreen.add(&MatchBoxMain);
        scannerScreen.add(&AmplifyBlue);
        scannerScreen.add(&AmplifyRed);
        scannerScreen.add(&rest);
    
        pongscreen.add(&pongback);

    // __  Database Scene __
        Empty dataVisualizationScreen(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));
        EzText teamdata (raylib::Text(spaceCadet, "Team Data:"), RAYWHITE, 12.0_spD, 0.0);
        EzText matchdata (raylib::Text(spaceCadet, "Match Data:"), RAYWHITE, 12.0_spD, 0.0);
        Empty gap (raylib::Rectangle(0, 0, 1, 40));
        TextBox TeamBox(100.0_spX, 20.0_spY, 10, 0.0, 15.0_spD, spaceMono, WHITE, WHITE);
        TextBox MatchBox(100.0_spX, 20.0_spY, 10, 0.0, 15.0_spD, spaceMono, WHITE, WHITE);

        Button submit (100.0_spX,50.0_spY, RAYWHITE, BLACK, DARKGRAY, EzText(raylib::Text(spaceCadet, "Submit"), RAYWHITE, 10.0_spD, 0.0));
        DrawableList dataList(VERTICAL,10);  
            dataList.add(&teamdata);    
            dataList.add(&TeamBox);
         
            dataList.add(&gap);
            dataList.add(&matchdata);
            dataList.add(&MatchBox);
            dataList.add(&gap);
            dataList.add(&submit);
            dataList.setDisplayPos(CENTERLEFT);


        dataVisualizationScreen.add(&DB)
            .add(&dataList);
            
    // __ BT Scene __
        Empty btTestingScene(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight()));       
        btTestingScene.add(&pong);
        Button disconnectAllTabs(200.0_spX, 100.0_spY, RAYWHITE, raylib::Color(0,0,0,0), raylib::Color(255,255,255,20), EzText(raylib::Text(spaceCadet, "Reset Tabs"), RAYWHITE, 12.0_spD, 0.0));
            disconnectAllTabs.setDisplayPos(BOTTOMRIGHT);
        EzText macAddress(raylib::Text(comicSans, "Mac: " + btConn.getLocalMacStr()), RAYWHITE, 24.0_spD, 0.0);
        EzText port(raylib::Text(comicSans, "Port: " + std::to_string(btConn.getLocalPort())), RAYWHITE, 24.0_spD, 0.0);
        EzText activeConnections(raylib::Text(comicSans, "Connections: " + std::to_string(btConn.getNumConnections())), RAYWHITE, 24.0_spD, 0.0);
        DrawableList serverData(VERTICAL, 0);
            serverData.add(&macAddress);
            serverData.add(&port);
            serverData.add(&activeConnections);
            serverData.setDisplayPos(BOTTOMLEFT);
        btTestingScene.add(&serverData);
        btTestingScene.add(&disconnectAllTabs);
        btConn.getNameList()->setDisplayPos(BOTTOMCENTERED);
        VerticalScrollable* nameList = btConn.getNameList();
        btTestingScene.add(nameList);



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
        if (IsKeyPressed(KEY_E)) {
            toastHandler::add(Toast(WinsockErrorDesc::get(6).errorNameDesc,TOASTLENGTHS::LENGTH_LONG));
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

        btConn.handleReadyConnections();
        activeConnections.setText("Connections: " + std::to_string(btConn.getNumConnections()));
        btConn.updateAllBt();
        activeConnections.setText("Connections: " + std::to_string(btConn.getNumConnections()));

        switch(currentScene) {
            case SCANNING:
                // calculating
                // qrScanner.scan();
                // if (goated.isPressed()) {
                //     qrScanner.update();
                // } 
                if (rest.isPressed()) {
                    handler.deleteteams();
                    for (int i = 0; i < 20; i++)    {
                        handler.getteamdata(i);
                        
                    }
                    
               

                    
                   
                }
                if (AmplifyBlue.isPressed()) {
                    try {
                        auto res = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DataPointID,  DCValue, TeamID,AllianceID) values ( 4,-1,11,'true', -1, 'Blue');", 0);  
                        toastHandler::add(Toast("Amplify Blue Started",LENGTH_NORMAL));

                    }
                    catch (...) {
                        toastHandler::add(Toast("error",LENGTH_NORMAL));
                    }
                   
                }
                if (AmplifyRed.isPressed()) {
                    try {
                        auto res = database.execQuery("insert into matchtransaction ( MatchId, ScouterID, DataPointID,  DCValue, TeamID,AllianceID) values ("+ MatchBoxMain.getText() + ",-1,11,'true', -1, 'Red');", 0);  
                        toastHandler::add(Toast("Amplify Red Started",LENGTH_NORMAL));
                    }
                    catch (...) {
                        toastHandler::add(Toast("error",LENGTH_NORMAL));
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
                 if(DB.isPressed()) {
                    auto vector = database.execQuery("select DatapointID,DCValue,DCTimestamp from matchtransaction;", 3);
                    std::cout << "hhh" << std::endl;
                    for (auto i : vector) {
                        for (std::string j : i) {
                            std::cout << j << std::endl;
                        }
                        std::cout << std::endl;
                    }                    
                }

                teamdata.setText("Team Data:" + TeamBox.getText());
                matchdata.setText("Match Data:" + MatchBox.getText());
                if(submit.isPressed()) {
                    auto vector = database.execQuery("select * from matchtransaction where TeamId =" + TeamBox.getText() + " && MatchId = "+ MatchBox.getText() +";", 8);
                    for (auto i : vector) {
                        for (std::string j : i) {
                            std::cout << j << std::endl;
                        }
                        std::cout << std::endl;
                    }
                }
                window.BeginDrawing();
                    window.ClearBackground(BLACK);
                    texture.draw(0,0);
                    dataVisualizationScreen.updateAndDraw(raylib::Rectangle(0, GetScreenHeight() * 0.15, GetScreenWidth(), GetScreenHeight() * 0.85));
            break;

            case BLUETOOTH:
                if (disconnectAllTabs.isPressed()) {
                    btConn.killAllSockets();
                }
                if (pong.isPressed()) {
                    std::cout << "hello" << std::endl;
                    currentScene = PONG;
                }
                window.BeginDrawing();
                window.ClearBackground(BLACK);
                nameList = btConn.getNameList();
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

                    mainTab.enable();
                    databaseTab.enable();
                    bluetoothTab.disable();
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
    WinsockErrorDesc::destroy();
    return 0;
}