#include <iostream>
#include "Game.hpp"

int main()
{
    int screenWidth = 1280;
    int screenHeight = 1000;
    Game* game = new Game();

    if (game->InitializeWindow(screenWidth, screenHeight))
    {
        game->MainLoop();
    }
}

