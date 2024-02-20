#ifndef SCENES_HPP
#define SCENES_HPP

#include "currentScreen.hpp"
#include "../include/raylib-cpp.hpp"
#include "sizeScaling.hpp"
#include <iostream>

class Scenes {

    private:
        CURRENTSCREEN screen = MAINMENU;
        SizeScaling* scaling;
        Vector2 tileSize;
        raylib::Rectangle* rect = new raylib::Rectangle(0, 0, 250, 250);

        void mainMenu() {
           // make text and get text size for title...
            raylib::Text startScreenText(GetFontDefault(),"Hello!", this->tileSize.x * 10.0);
            startScreenText.SetSpacing(this->tileSize.x);
            Vector2 textSize = startScreenText.MeasureEx();

            raylib::Vector2 titlePos((float)(this->scaling->getWindow()->GetRenderWidth()/2.0 - textSize.x/2.0), (float)(this->scaling->getWindow()->GetRenderHeight()/2.0 - textSize.y/2.0));
            startScreenText.Draw(titlePos);
        }
        void game() {
            rect->Draw(WHITE);
        }
        void settings() {

        }
        void levelEditorInit() {

        }
        void levelEditor() {

        }
    public:
        Scenes(SizeScaling* scale) {
            this->scaling = scale;
        }
        void drawScene() {
            this->scaling->getWindow()->ClearBackground(BLACK);
            // get tile size which will be used to draw the scene
            this->tileSize = this->scaling->getTileSize(); 
            std::cout << this->scaling->getTileSize().x << " + " << this->scaling->getTileSize().y << std::endl;

            switch (this->screen)
            {
                case MAINMENU:
                    this->mainMenu();
                    break;
                case GAME:
                    this->game();
                    break;
                case SETTINGS:
                    this->settings();
                    break;
                case LEVELEDITORINIT:
                    this->levelEditorInit();
                    break;
                case LEVELEDITOR:
                    this->levelEditor();
                    break;
                default:
                    break;
            }
        }
};

#endif