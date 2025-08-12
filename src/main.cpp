#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "../include/cgame.hpp"

int main(int argc, char* argv[]) 
{
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        std::cerr << "Failed to initialize SDL. " << SDL_GetError() << std::endl;
    }

    cgame::Window window(1280, 720, "CGame example");
    cgame::Clock clock;
    
    
    cgame::Surface blueBox(50, 100);
    blueBox.fill({0, 0, 255, 255});

    cgame::Surface testImg = cgame::loadImage("assets/images/player.png");
    
    float x = 100;

    bool running = true;
    
    while (running)
    {
        cgame::Event e;
        while (cgame::getEvents(e))
        {
            if (e.type == cgame::QUIT)
            {
                running = false;
            }
            if (e.type == cgame::KEYDOWN)
            {
                if (e.key == SDLK_ESCAPE)
                    running = false;
            }
        }

        window.fill({255, 0, 0, 255});

        x += 3;
        window.blit(blueBox, 100, 200);
        window.blit(testImg, x, 400);

        window.update();
        float dt = clock.tick(60);
    }

    cgame::quit();

    return 0;
}