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
        Ballspeed = raylib::Vector2(8,1.5);

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
        bool up;
        // key = GetKeyPressed();
        // while (key != KEY_NULL) {
            
           if (IsKeyDown(KEY_DOWN)) {
                up = false;
                Paddle1pos.y += 5 * adjustedframetime;
           }
           else if (IsKeyDown(KEY_UP)) {
                up = true;
                Paddle1pos.y += -5 * adjustedframetime;
           }
            if (IsKeyDown(KEY_S)) {
                up = false;
                Paddle2pos.y += 5 * adjustedframetime;
           }
           else if (IsKeyDown(KEY_W)) {
                up = true;
                Paddle2pos.y += -5 * adjustedframetime;
           }
            // std::cout << key << std::endl;
            // key = GetKeyPressed();
        // }
        bool xcol, ycol, pcol;
        xcol = Ballpos.x >= window->GetWidth() -10 ||
        Ballpos.x <= 5;
        ycol = Ballpos.y >= window->GetHeight()-10 || Ballpos.y <= 5;
        pcol = CheckCollisionPointRec(Ballpos, raylib::Rectangle(Paddle1pos.x,Paddle1pos.y,30.0f,90.0f)) || CheckCollisionPointRec(Ballpos, raylib::Rectangle(Paddle2pos.x,Paddle2pos.y,30.0f,90.0f));
        if (!colast) {
            if (pcol && ycol) {
                if (up) {
                    Ballspeed.x += randfr( Ballspeed.x *-0.03f, Ballspeed.x * 0.04f);

                    Ballspeed.y -= (2 + (randfr(Ballspeed.y * -0.02f, Ballspeed.y * 0.02f)));
                }
                else if (!up) {
                    Ballspeed.x += randfr(Ballspeed.x *-0.03f, Ballspeed.x * 0.04f);

                    Ballspeed.y += (2 + (randfr(Ballspeed.y * -0.02f, Ballspeed.y * 0.025f)));
                }
                colast = true; 
            }
            else if ((xcol && ycol) || pcol)  {
                Ballspeed.x = -(Ballspeed.x + randfr(Ballspeed.x * -0.2f, Ballspeed.x * 0.4f));
                Ballspeed.y = -(Ballspeed.y + randfr(Ballspeed.y * -0.6f, Ballspeed.y * 0.7f));
                colast = true; 
            }
            else if (xcol) {
                colast = true; 
                // std::cout <<"collisionx" << std::endl;
           
                Ballspeed.x = -(Ballspeed.x + randfr(Ballspeed.x * -0.2f, Ballspeed.x * 0.4f));
        
            
                Ballspeed.y += randfr(-0.06f,0.06f);
            }
            else if (ycol) {
                colast = true;
                // std::cout <<"collisiony" << std::endl;
                Ballspeed.y = -(Ballspeed.y + randfr(Ballspeed.y * -0.5f, Ballspeed.y * 0.6f));
                Ballspeed.x += randfr( Ballspeed.x *-0.3f, Ballspeed.x * 0.6f);
            }
            else {
                colast = false;
            }
        }
        else {
            if (!xcol && !ycol && !pcol) {
                colast = false;
            }
            
        }
     
        // std::cout << Ballspeed.x << " | " << Ballspeed.y << std::endl;
        Ballpos.x += Ballspeed.x * adjustedframetime;
        Ballpos.y += Ballspeed.y * adjustedframetime;   
        if (abs(Ballspeed.x) > 8  ) {
            Ballspeed.x -= (Ballspeed.x) * 0.005;
        }
        else if (abs(Ballspeed.y) > 6) {
            Ballspeed.y -= (Ballspeed.y) * 0.005;
        }
       

    }
};

#endif