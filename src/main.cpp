#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "../include/cgame.hpp"

int main(int argc, char* argv[]) 
{
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        std::cerr << "Failed to initialize SDL. Error: " << SDL_GetError() << std::endl;
    }

    cgame::Window window(1280, 720, "CGame example");
    cgame::time::Clock clock;
    
    cgame::Surface blueBox(window.renderer, 50, 100);

    bool running = true;
    SDL_Event e;
    
    while (running)
    {
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        window.fill({255, 0, 0, 255});
        
        blueBox.fill({0, 0, 255, 255});
        window.blit(blueBox, 200, 300);

        window.update();
        float dt = clock.tick(60);
    }

    cgame::quit();

    return 0;
}