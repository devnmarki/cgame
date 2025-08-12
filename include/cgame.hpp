#pragma once

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#ifndef CGAME_HPP
#define CGAME_HPP

namespace cgame 
{
    class Window 
    {
    public:
        SDL_Renderer* renderer;        

        Window(int _width, int _height, const char* _title)
            : window(NULL), renderer(NULL), width(_width), height(_height), title(_title)
        {
            window = SDL_CreateWindow(title, width, height, 0);
            if (window == NULL)
            {
                std::cerr << "Failed to create window. Error: " << SDL_GetError() << std::endl;
            }

            renderer = SDL_CreateRenderer(window, NULL);
            if (renderer == NULL)
            {
                std::cerr << "Failed to create renderer. Error: " << SDL_GetError() << std::endl;
            }
        }   
        
        ~Window()
        {
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
        }

    private:
        SDL_Window* window;
        int width, height;
        const char* title;
    };
}

#endif