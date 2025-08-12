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
    }

    SDL_Quit();

    return 0;
}