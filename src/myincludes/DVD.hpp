#ifndef DVD_HPP
#define DVD_HPP

#include "toastHandler.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/raylib-cpp.hpp"
#include "../myincludes/allincludes.hpp"


class DVD {
    private: 
    int key;

    public: 

        raylib::Vector2 Logopos;
        raylib::Vector2 LogoSpeed;
        float MaxPaddlespeed;
        raylib::Window *window;
        bool colast; 
        float adjustedframetime;
        ShouldScale xnum,ynum;
        bool xcol, ycol;
    DVD(raylib::Window *window) {
        colast = false;
        srand(time(NULL));
        this->window = window;
        this->xnum = ShouldScale(640.0, true, XDEPENDENT);
        this->ynum = ShouldScale(310.0, true, YDEPENDENT);
        Logopos =  raylib::Vector2(xnum,ynum);
        LogoSpeed = raylib::Vector2(8,1.5);

       
    
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
        
      
        xcol = Logopos.x >= window->GetWidth() -10 ||
        Logopos.x <= 5;
        ycol = Logopos.y >= window->GetHeight()-10 || Logopos.y <= 5;
        
        if (!colast) {
            if (xcol && ycol) {
               
           
                LogoSpeed.x = -(LogoSpeed.x + randfr(LogoSpeed.x * -0.2f, LogoSpeed.x * 0.2f));
                LogoSpeed.y = -(LogoSpeed.y + randfr(LogoSpeed.y * -0.2f, LogoSpeed.y * 0.2f));
                colast = true; 
            }
            else if (xcol) {
                colast = true; 
                // std::cout <<"collisionx" << std::endl;
           
                LogoSpeed.x = -(LogoSpeed.x + randfr(LogoSpeed.x * -0.2f, LogoSpeed.x * 0.4f));
        
            
                LogoSpeed.y += randfr(-0.06f,0.06f);
            }
            else if (ycol) {
                colast = true;
                // std::cout <<"collisiony" << std::endl;
                LogoSpeed.y = -(LogoSpeed.y + randfr(LogoSpeed.y * -0.5f, LogoSpeed.y * 0.6f));
                LogoSpeed.x += randfr( LogoSpeed.x *-0.3f, LogoSpeed.x * 0.6f);
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
     
        // std::cout << LogoSpeed.x << " | " << LogoSpeed.y << std::endl;
        Logopos.x += LogoSpeed.x * adjustedframetime;
        Logopos.y += LogoSpeed.y * adjustedframetime;   
        if (abs(LogoSpeed.x) > 7) {
            LogoSpeed.x -= (LogoSpeed.x) * 0.05;
        }
        else if (abs(LogoSpeed.y) > 7) {
            LogoSpeed.y -= (LogoSpeed.y) * 0.05;
        }
       

    }
};

#endif