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
        Surface(int _width, int _height)
            : width(_width), height(_height)
        {

            surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
        }

        Surface(SDL_Surface* existing)
        {
            surface = existing;
        }

        ~Surface()
        {
            if (surface)
                SDL_DestroySurface(surface);
        }

        void fill(Color color)
        {
            SDL_FillSurfaceRect(surface, NULL, SDL_MapSurfaceRGB(surface, color.r, color.g, color.b));
        }

        void blit(Surface &surf, float x, float y)
        {
            SDL_Rect dstRect = { x, y, surf.getWidth(), surf.getHeight() };
            SDL_BlitSurface(surf.surface, NULL, surface, &dstRect);
        }

        int getWidth()
        {
            return width;
        }

        int getHeight()
        {
            return height;
        }

        SDL_Surface* getSurface()
        {
            return surface;
        }
    private:
        SDL_Surface* surface;
        int width, height;
    };

    Surface loadImage(const char* filePath)
    {
        SDL_Surface* img = IMG_Load(filePath);
        if (!img)
        {
            std::cerr << "Failed to load image. Error: " << filePath << SDL_GetError() << std::endl;
            return Surface(0, 0);
        }

        return Surface(img);
    }

    class Window 
    {
    public:
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

            screenSurface = new Surface(width, height);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        }   
        
        ~Window()
        {
            delete screenSurface;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
        }

        void fill(Color color)
        {
            screenSurface->fill(color);
        }

        void blit(Surface& surface, float x, float y)
        {
            screenSurface->blit(surface, x, y);
        }

        void update()
        {
           SDL_UpdateTexture(texture, NULL, screenSurface->getSurface()->pixels, screenSurface->getSurface()->pitch);
           SDL_RenderClear(renderer);
           SDL_RenderTexture(renderer, texture, NULL, NULL);
           SDL_RenderPresent(renderer);
        }

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;    
        Surface* screenSurface;    
        SDL_Texture* texture;

        int width, height;
        const char* title;
    };

    void quit()
    {
        SDL_Quit();
    }

    class Clock
    {
    public:
        Clock() 
        {
            lastTick = SDL_GetTicks();
        }

        float tick(int fps = 0)
        {
            Uint32 now = SDL_GetTicks();
            float delta = (now - lastTick) / 1000.0f;
            lastTick = now;
            if (fps > 0)
            {
                Uint32 frameDelay = 1000 / fps;
                Uint32 frameTime = SDL_GetTicks() - now;
                if (frameDelay > frameTime)
                    SDL_Delay(frameDelay - frameTime);
            }
            return delta;
        }
    private:
        Uint32 lastTick = 0;
    };

    enum EventType
    {
        QUIT,
        KEYDOWN,
        KEYUP,
        MOUSEDOWN,
        MOUSEUP
    };

    struct Event
    {
        EventType type;
        SDL_Keycode key;
        int mouseX, mouseY;
        Uint8 mouseButton;
    };

    bool getEvents(Event& e)
    {
        SDL_Event sdlEvent;
        if (SDL_PollEvent(&sdlEvent))
        {
            switch (sdlEvent.type)
            {
            case SDL_EVENT_QUIT:
                e.type = QUIT;
                return true;
            case SDL_EVENT_KEY_DOWN:
                e.type = KEYDOWN;
                e.key = sdlEvent.key.key;
                return true;
            case SDL_EVENT_KEY_UP:
                e.type = KEYUP;
                e.key = sdlEvent.key.key;
                return true;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                e.type = MOUSEDOWN;
                e.mouseX = sdlEvent.button.x;
                e.mouseY = sdlEvent.button.y;
                e.mouseButton = sdlEvent.button.button;
                return true;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                e.type = MOUSEUP;
                e.mouseX = sdlEvent.button.x;
                e.mouseY = sdlEvent.button.y;
                e.mouseButton = sdlEvent.button.button;
                return true;
            }
        }
        return false;
    }
}

#endif