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

    struct Rect
    {
        int x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0) { }
        Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) { }

        Rect copy()
        {
            return { x, y, w, h };
        }

        bool colliderrect(const Rect &other)
        {
            return !(x + w <= other.x || other.x + other.w <= x ||
                 y + h <= other.y || other.y + other.h <= y);
        }

        SDL_Rect to_sdl() const 
        {
            return SDL_Rect{x, y, w, h};
        }

        int left() const { return x; }
        int right() const { return x + w; }
        int top() const { return y; }
        int bottom() const { return y + h; }
        int centerx() const { return x + w / 2; }
        int centery() const { return y + h / 2; }
        std::pair<int,int> center() const { return {centerx(), centery()}; }

        void set_left(int val) { x = val; }
        void set_right(int val) { x = val - w; }
        void set_top(int val) { y = val; }
        void set_bottom(int val) { y = val - h; }
        void set_centerx(int val) { x = val - w / 2; }
        void set_centery(int val) { y = val - h / 2; }
        void set_center(int cx, int cy) 
        {
            set_centerx(cx);
            set_centery(cy);
        }
    };

    class Surface
    {
    public:
        Surface(int _width, int _height)
            : width(_width), height(_height), x(0), y(0)
        {
            rect = { x, y, width, height };
            surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
        }

        Surface(SDL_Surface* existing)
        {
            surface = existing;
            width = existing->w;
            height = existing->h;
            x = 0;
            y = 0;
            rect = { x, y, width, height };
        }

        ~Surface()
        {
            if (surface)
                SDL_DestroySurface(surface);
        }

        void fill(Color color)
        {
            SDL_FillSurfaceRect(surface, NULL,
                SDL_MapSurfaceRGB(surface, color.r, color.g, color.b));
        }

        void blit(Surface &surf, float x, float y, int w = -1, int h = -1)
        {
            SDL_Rect dstRect;
            dstRect.x = static_cast<int>(x);
            dstRect.y = static_cast<int>(y);
            dstRect.w = (w == -1) ? surf.getWidth() : w;
            dstRect.h = (h == -1) ? surf.getHeight() : h;
            
            SDL_BlitSurfaceScaled(surf.surface, NULL, surface, &dstRect, SDL_SCALEMODE_NEAREST);
        }

        void blit(Surface &surf, Rect rect, int w = -1, int h = -1)
        {
            SDL_Rect dstRect;
            dstRect.x = static_cast<int>(rect.x);
            dstRect.y = static_cast<int>(rect.y);
            dstRect.w = (w == -1) ? surf.getWidth() : w;
            dstRect.h = (h == -1) ? surf.getHeight() : h;
            
            SDL_BlitSurfaceScaled(surf.surface, NULL, surface, &dstRect, SDL_SCALEMODE_NEAREST);
        }

        void setWidth(int _width)
        {
            width = _width;
            rect.w = _width;
        }

        void setHeight(int _height)
        {
            height = _height;
            rect.h = _height;
        }

        void setPosition(int _x, int _y)
        {
            x = _x;
            y = _y;
            rect.x = _x;
            rect.y = _y;
        }

        int getX() const { return x; }
        int getY() const { return y; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }

        Rect getRect(int x = 0, int y = 0) const
        {
            return { x, y, rect.w, rect.h };
        }

        SDL_Surface* getSurface() const
        {
            return surface;
        }

    private:
        SDL_Surface* surface;
        int x, y;
        int width, height;
        Rect rect;
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

        void blit(Surface& surface, int x, int y, int w = -1, int h = -1)
        {
            screenSurface->blit(surface, x, y, w, h);
        }

        void blit(Surface& surface, Rect rect, int w = -1, int h = -1)
        {
            screenSurface->blit(surface, rect, w, h);
        }

        void update()
        {
            SDL_SetWindowTitle(window, title);

            SDL_UpdateTexture(texture, NULL, screenSurface->getSurface()->pixels, screenSurface->getSurface()->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        void setTitle(const char* _title)
        {
            title = _title;
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

    class Clock {
    public:
        Clock() {
            lastTick = SDL_GetTicks();
        }

        float tick(int fps = 0) {
            Uint32 now = SDL_GetTicks();
            float delta = (now - lastTick) / 1000.0f;

            if (fps > 0) {
                Uint32 frameDelay = 1000 / fps;
                Uint32 frameTime = SDL_GetTicks() - lastTick;
                if (frameDelay > frameTime)
                    SDL_Delay(frameDelay - frameTime);

                now = SDL_GetTicks();
                delta = (now - lastTick) / 1000.0f;
            }

            lastTick = now;
            currentFPS = (delta > 0) ? (1.0f / delta) : 0.0f;
            return delta;
        }

        float getFPS() const { return currentFPS; }

    private:
        Uint32 lastTick;
        float currentFPS = 0.0f;
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