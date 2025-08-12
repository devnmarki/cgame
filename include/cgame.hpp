#pragma once

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#ifndef CGAME_HPP
#define CGAME_HPP

namespace cgame 
{
    struct Color
    {
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;
    };

    class Surface
    {
    public:
        Surface(SDL_Renderer* _renderer, int _width, int _height)
            : renderer(_renderer), width(_width), height(_height)
        {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        }

        void fill(Color color)
        {
            SDL_SetRenderTarget(renderer, texture);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, NULL);
        }

        SDL_Texture* getTexture()
        {
            return texture;
        }
    private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        int width, height;
    };

    class Window 
    {
    public:
        SDL_Renderer* renderer;    
        Surface* screen;    

        Window(int _width, int _height, const char* _title)
            : width(_width), height(_height), title(_title)
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

            screen = new Surface(renderer, width, height);
        }   
        
        ~Window()
        {
            delete screen;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
        }

        void fill(Color color)
        {
            screen->fill(color);
        }

        void update()
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderTexture(renderer, screen->getTexture(), NULL, NULL);
            SDL_RenderPresent(renderer);
        }

    private:
        SDL_Window* window;
        int width, height;
        const char* title;
    };
}

#endif