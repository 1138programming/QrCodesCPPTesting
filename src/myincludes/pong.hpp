#ifndef PONG_HPP
#define PONG_HPP

#include "toastHandler.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/raylib-cpp.hpp"
#include "../myincludes/allincludes.hpp"


class Pong {
    private: 
    int key;

    public: 
        raylib::Vector2 Paddle1pos;
        raylib::Vector2 Paddle2pos;
        raylib::Vector2 Ballpos;
        raylib::Vector2 Ballspeed;
        float MaxPaddlespeed;
        raylib::Window *window;
        bool colast; 
         float adjustedframetime;
    Pong(raylib::Window *window) {
        colast = false;
        srand(time(NULL));
        this->window = window;
        Ballpos =  raylib::Vector2(640,310);
        Ballspeed = raylib::Vector2(4,1.5);

        Paddle1pos = raylib::Vector2(1080,400);
 
        Paddle2pos = raylib::Vector2(200,400);

      
    
    }
    

    float randfr(float min, float max) {
        float range = max - min;
        float div = RAND_MAX / range;
        return min + (rand() / div);
    }


    void run() {

    }

    void update() {
        adjustedframetime = 50 * GetFrameTime();
        // key = GetKeyPressed();
        // while (key != KEY_NULL) {
            
           if (IsKeyDown(KEY_DOWN)) {
                Paddle1pos.y += 5 * adjustedframetime;
           }
           else if (IsKeyDown(KEY_UP)) {
                Paddle1pos.y += -5 * adjustedframetime;
           }
            if (IsKeyDown(KEY_S)) {
                Paddle2pos.y += 5 * adjustedframetime;
           }
           else if (IsKeyDown(KEY_W)) {
                Paddle2pos.y += -5 * adjustedframetime;
           }
            // std::cout << key << std::endl;
            // key = GetKeyPressed();
        // }
        bool xcol, ycol, pcol;
        xcol = Ballpos.x >= window->GetWidth() -5 ||
        Ballpos.x <= 5;
        ycol = Ballpos.y >= window->GetHeight()-5 || Ballpos.y <= 5;
        pcol = CheckCollisionPointRec(Ballpos, raylib::Rectangle(Paddle1pos.x,Paddle1pos.y,40.0f,120.0f)) || CheckCollisionPointRec(Ballpos, raylib::Rectangle(Paddle2pos.x,Paddle2pos.y,40.0f,120.0f));
        if (!colast) {
            if ((xcol && ycol) || pcol)  {
                Ballspeed.x = -(Ballspeed.x + randfr(abs(Ballspeed.x) * -0.5f, abs(Ballspeed.x) * 0.6f));
                Ballspeed.y = -(Ballspeed.y + randfr(abs(Ballspeed.y) * -0.52f, abs(Ballspeed.y) * 0.52f));

            }
            else if (xcol) {
                colast = true; 
                // std::cout <<"collisionx" << std::endl;
           
                Ballspeed.x = -(Ballspeed.x + randfr(abs(Ballspeed.x) * -0.5f, abs(Ballspeed.x) * 0.6f));
        
            
                Ballspeed.y += randfr(-0.05f,0.05f);
            }
            else if (ycol) {
                colast = true;
                // std::cout <<"collisiony" << std::endl;
                Ballspeed.y = -(Ballspeed.y + randfr(abs(Ballspeed.y) * -0.52f, abs(Ballspeed.y) * 0.52f));
                Ballspeed.x += randfr( abs(Ballspeed.x) *-0.5f, abs(Ballspeed.x) * 1.25f);
            }
            else {
                colast = false;
            }
        }
        else {
            if (!xcol && !ycol) {
                colast = false;
            }
            
        }
        std::cout << Ballspeed.x << " | " << Ballspeed.y << std::endl;
        Ballpos.x += Ballspeed.x * adjustedframetime;
        Ballpos.y += Ballspeed.y * adjustedframetime;
       

    }
};

#endif