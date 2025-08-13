#pragma once

#include <iostream>
#include <tuple>

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
        Uint8 a = 255;
    };

    struct Vec2
    {
        float x, y;

        Vec2() : x(0), y(0) { }
        Vec2(float _x, float _y) : x(_x), y(_y) { }
    };

    struct Rect
    {
        float x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0) { }
        Rect(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) { } 

        Rect copy()
        {
            return { x, y, w, h };
        }

        SDL_FRect to_sdl()
        {
            return { x, y, w, h };
        }

        bool colliderect(const Rect &other)
        {
            return !(x + w <= other.x || other.x + other.w <= x || y + h <= other.y || other.y + other.h <= y);
        }

        bool collidepoint(float px, float py)
        {
                return (px >= x && px <= x + w && py >= y && py <= y + h);
        }

        float left() const { return x; }
        float right() const { return x + w; }
        float top() const { return y; }
        float bottom() const { return y + h; }
        float centerx() const { return x + w / 2; }
        float centery() const { return y + h / 2; }
        std::pair<float, float> center() const { return {centerx(), centery()}; }

        void set_left(float val) { x = val; }
        void set_right(float val) { x = val - w; }
        void set_top(float val) { y = val; }
        void set_bottom(float val) { y = val - h; }
        void set_centerx(float val) { x = val - w / 2; }
        void set_centery(float val) { y = val - h / 2; }
        void set_center(float cx, float cy) 
        {
            set_centerx(cx);
            set_centery(cy);
        }
    };

    class Surface
    {
    public:
        Surface(SDL_Renderer* _renderer, float _width, float _height)
            : renderer(_renderer), width(_width), height(_height), x(0), y(0)
        {
            surfaceTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            rect = { x, y, width, height };
        }

        Surface(SDL_Renderer* _renderer, SDL_Texture* _existing)
            : renderer(_renderer), surfaceTex(_existing), width(_existing->w), height(_existing->h), x(0), y(0)
        {
            rect = { x, y, _existing->w, _existing->h };
        }

        ~Surface()
        {
            SDL_DestroyTexture(surfaceTex);
        }

        void fill(Color color = { 0, 0, 0, 255 })
        {
            SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, surfaceTex);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, oldTarget);
        }

        void blit(Surface& surface, float _x, float _y)
        {   
            x = _x;
            y = _y;
            SDL_Texture* previousTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, surfaceTex);   
            SDL_SetTextureScaleMode(surface.get_surface(), SDL_SCALEMODE_NEAREST);
            SDL_FRect dst = { _x, _y, surface.get_width(), surface.get_height() };
            SDL_RenderTexture(renderer, surface.get_surface(), NULL, &dst);
            SDL_SetRenderTarget(renderer, previousTarget);
        }

        void blit(Surface& surface, Rect _rect)
        {
            blit(surface, _rect.x, _rect.y);
        }

        void set_width(float _width) { rect.w = _width; }
        void set_height(float _height) { rect.h = _height; }

        SDL_Texture* get_surface() const { return surfaceTex; }
        float get_width() const { return rect.w; }
        float get_height() const { return rect.h; }
        Rect get_rect(float _x = 0, float _y = 0)
        {
            rect.x = _x;
            rect.y = _y;
            return rect;
        }

    private:
        SDL_Renderer* renderer; 
        SDL_Texture* surfaceTex; 
        
        float x, y;
        float width, height;      
        Rect rect; 
    };

    namespace transform
    {
        Surface& scale(Surface& surface, float newWidth, float newHeight)
        {
            surface.set_width(newWidth);
            surface.set_height(newHeight);
            return surface;
        }
    }

    namespace image
    {
        Surface load(SDL_Renderer* renderer, std::string filePath)
        {
            SDL_Texture* imgTex = IMG_LoadTexture(renderer, filePath.c_str());
            if (imgTex == NULL)
            {
                std::cerr << "Failed to load image " << filePath << " Error: " << SDL_GetError() << std::endl;
            }
    
            return Surface(renderer, imgTex);
        }
    }

    class Window
    {
    public:
        Window(int width, int height, std::string title)
            : m_width(width), m_height(height), m_title(title)
        {
            m_window = SDL_CreateWindow(title.c_str(), width, height, 0);
            if (m_window == NULL)
            {
                std::cerr << "Failed to create window. Error: " << SDL_GetError() << std::endl;
            }

            m_renderer = SDL_CreateRenderer(m_window, NULL);
            if (m_renderer == NULL)
            {
                std::cerr << "Failed to create renderer. Error: " << SDL_GetError() << std::endl;
            }

            screenSurface = new Surface(m_renderer, m_width, m_height);
        }

        ~Window()
        {
            delete screenSurface;
            SDL_DestroyRenderer(m_renderer);
            SDL_DestroyWindow(m_window);
        }

        void begin_frame(Color color = { 0, 0, 0, 255 })
        {
            SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
            SDL_RenderClear(m_renderer);

            SDL_FRect dst = { 0, 0, screenSurface->get_width(), screenSurface->get_height() };
            SDL_RenderTexture(m_renderer, screenSurface->get_surface(), NULL, &dst);
        }

        void blit(Surface& surface, float x, float y)
        {
            screenSurface->blit(surface, x, y);
        }

        void end_frame()
        {
            SDL_RenderPresent(m_renderer);
        }

        void set_title(std::string title)
        {
            SDL_SetWindowTitle(m_window, title.c_str());
        }

        SDL_Renderer* get_renderer() { return m_renderer; }
        SDL_Window* get_window() { return m_window; }
        int get_width() { return m_width; }
        int get_height() { return m_height; }
        std::string get_title() { return m_title; }

    private:
        SDL_Renderer* m_renderer;
        SDL_Window* m_window;
        Surface* screenSurface;

        int m_width, m_height;
        std::string m_title;
    };

    namespace display
    {
        static Window* window = nullptr;

        Window set_mode(int width, int height)
        {
            window = new Window(width, height, "cgame window");
            return *window;
        }

        void set_caption(std::string caption)
        {
            if (window)
            {
                window->set_title(caption);
            }
        }

        int get_width()
        {
            return window->get_width();
        }

        int get_height()
        {
            return window->get_height();
        }

        Vec2 get_size()
        {
            return { window->get_width(), window->get_height() };
        }
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

    void init()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) 
        {
            std::cerr << "Failed to initialize SDL. " << SDL_GetError() << std::endl;
        }
    }

    void quit()
    {
        SDL_Quit();
    }

    namespace mouse
    {
        Vec2 get_pos()
        {
            float mx, my;
            SDL_GetMouseState(&mx, &my);
            return { mx, my };
        }
    }

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