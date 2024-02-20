#include "include/raylib-cpp.hpp"
#include "../myincludes/qrcodeScanner.hpp"
// #include "myincludes/allincludes.hpp"
#include "../myincludes/button.hpp"
#include <iostream>
#include <fstream>


int main() {
    raylib::Window window(800,600,"I am mentally dumb");
    QrCodeScanner qrScanner; 
    raylib::Color white();
    Button gaming(150,100, );


    while(!window.ShouldClose()) {
        qrScanner.scan();

        window.BeginDrawing();
            window.ClearBackground(BLACK);
        window.EndDrawing();
    }
    return 1;
}