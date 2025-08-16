#ifndef CGAME_HPP
#define CGAME_HPP

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

namespace cgame
{
    struct Color
    {
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a = 255;

        SDL_Color to_sdl()
        {
            return { r, g, b, a };
        }
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

        SDL_FRect to_sdl() const
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

    void init()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
            std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;

        if (!IMG_Init(IMG_INIT_PNG))
            std::cerr << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;

        if (TTF_Init() == -1)
            std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;

        if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_WAVPACK) == -1)
            std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
            std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
    }

    void quit()
    {
        IMG_Quit();
        TTF_Quit();
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
    }

    class Surface
    {
    public:
        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;

        Surface(Surface&& other) noexcept
            : renderer(other.renderer), surfaceTex(other.surfaceTex), x(other.x), y(other.y), width(other.width), height(other.height), rotation(other.rotation), flip(other.flip), rect(other.rect)
        {
            other.surfaceTex = nullptr;
        }

        Surface& operator=(Surface&& other) noexcept
        {
            if (this != &other)
            {
                if (surfaceTex)
                    SDL_DestroyTexture(surfaceTex);

                renderer = other.renderer;
                surfaceTex = other.surfaceTex;
                x = other.x; y = other.y;
                width = other.width; height = other.height;
                rotation = other.rotation;
                flip = other.flip;
                rect = other.rect;

                other.surfaceTex = nullptr;
            }
            return *this;
        }

        Surface(SDL_Renderer* _renderer, float _width, float _height)
            : renderer(_renderer), width(_width), height(_height), x(0), y(0)
        {
            surfaceTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            rect = { x, y, width, height };
        }

        Surface(SDL_Renderer* _renderer, SDL_Texture* _existing)
            : renderer(_renderer), surfaceTex(_existing), x(0), y(0)
        {
            int texW = 0, texH = 0;
            SDL_QueryTexture(_existing, nullptr, nullptr, &texW, &texH);
            width = static_cast<float>(texW);
            height = static_cast<float>(texH);
            rect = { x, y, width, height };
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

            SDL_FPoint center = { width / 2, height / 2 };
            SDL_FRect dst = { _x, _y, surface.get_width(), surface.get_height() };
            SDL_RendererFlip flipMode = surface.is_flip() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            SDL_Texture* previousTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, surfaceTex);   
            SDL_SetTextureScaleMode(surface.get_surface(), SDL_ScaleModeNearest);
            SDL_RenderCopyExF(renderer, surface.get_surface(), NULL, &dst, surface.get_rotation(), NULL, flipMode);
            SDL_SetRenderTarget(renderer, previousTarget);
        }

        void blit(Surface& surface, Rect _rect)
        {
            blit(surface, _rect.x, _rect.y);
        }

        void set_alpha(float alpha)
        {
            alpha = std::clamp(alpha, 0.0f, 255.0f) / 255.0f;
            SDL_SetTextureAlphaMod(surfaceTex, static_cast<Uint8>(alpha * 255.0f + 0.5f));
        }

        void set_color(Color color = { 255, 255, 255 })
        {
            SDL_SetTextureColorMod(surfaceTex, color.r, color.g, color.b);
        }

        void set_width(float _width) { rect.w = _width; }
        void set_height(float _height) { rect.h = _height; }
        void set_rotation(float _rotation) { rotation = _rotation; }
        void set_flip(bool _flip) { flip = _flip; }

        SDL_Texture* get_surface() const { return surfaceTex; }
        float get_width() const { return rect.w; }
        float get_height() const { return rect.h; }
        float get_rotation() const { return rotation; }
        bool is_flip() const { return flip; }
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
        float rotation = 0.0f;
        bool flip = false;
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

        Surface& rotate(Surface& surface, float newRotation)
        {
            surface.set_rotation(newRotation);
            return surface;
        }

        Surface& flip(Surface& surface, bool flip)
        {
            surface.set_flip(flip);
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
                std::cerr << "Failed to load image " << filePath << " Error: " << IMG_GetError() << std::endl;
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
            m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
            if (m_window == NULL)
            {
                std::cerr << "Failed to create window. Error: " << SDL_GetError() << std::endl;
            }

            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
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
            SDL_RenderCopyF(m_renderer, screenSurface->get_surface(), NULL, &dst);
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

        Window& set_mode(int width, int height)
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
            return { (float)window->get_width(), (float)window->get_height() };
        }

        SDL_Renderer* get_renderer()
        {
            return window->get_renderer();
        }
    }

    namespace draw
    {
        void rect(Surface& surface, Rect rect, Color color = { 0, 0, 0, 255 })
        {
            SDL_Texture* prevTarget = SDL_GetRenderTarget(display::get_renderer());
            SDL_SetRenderTarget(display::get_renderer(), surface.get_surface());
            SDL_SetRenderDrawColor(display::get_renderer(), color.r, color.g, color.b, color.a);
            SDL_FRect sdlRect = rect.to_sdl();
            SDL_RenderDrawRectF(display::get_renderer(), &sdlRect);
            SDL_SetRenderTarget(display::get_renderer(), prevTarget);
        }

        void fill_rect(Surface& surface, Rect rect, Color color = { 0, 0, 0, 255 })
        {
            SDL_Texture* prevTarget = SDL_GetRenderTarget(display::get_renderer());
            SDL_SetRenderTarget(display::get_renderer(), surface.get_surface());
            SDL_SetRenderDrawColor(display::get_renderer(), color.r, color.g, color.b, color.a);
            SDL_FRect sdlRect = rect.to_sdl();
            SDL_RenderFillRectF(display::get_renderer(), &sdlRect);
            SDL_SetRenderTarget(display::get_renderer(), prevTarget);
        }
    }

    namespace font
    {
        struct Font
        {
            std::string filePath;
            float size;
            TTF_Font* font;

            Font(std::string _filePath, float _size)
                : filePath(_filePath), size(_size)
            {
                font = TTF_OpenFont(filePath.c_str(), size);
                if (!font)
                {
                    std::cerr << "Failed to load font " << filePath << " " << TTF_GetError() << std::endl;
                }
            }

            ~Font()
            {
                if (font)
                    TTF_CloseFont(font);
            }

            Surface render(std::string content, Color color = { 0, 0, 0, 255 })
            {
                SDL_Surface* fontSurface = TTF_RenderText_Solid(font, content.c_str(), color.to_sdl());
                if (!fontSurface)
                {
                    std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
                    return Surface(display::get_renderer(), (SDL_Texture*)NULL);
                }

                SDL_Texture* tex = SDL_CreateTextureFromSurface(display::get_renderer(), fontSurface);
                if (tex == NULL)
                {
                    std::cerr << "Failed to create texture from font surface: " << TTF_GetError() << std::endl;
                    return Surface(display::get_renderer(), (SDL_Texture*)NULL);
                }
                SDL_FreeSurface(fontSurface);

                return Surface(display::get_renderer(), tex);
            }
        };
    }

    namespace mixer
    {
        enum LoopMode
        {
            LOOP_NONE = 0,
            LOOP_INFINITE = -1
        };

        class Sound
        {
        public:
            Sound(const std::string& filename)
            {
                sound = Mix_LoadWAV(filename.c_str());
                if (!sound)
                {
                    std::cerr << "Failed to load sound: " << filename << " Error: " << Mix_GetError() << std::endl;
                }
            }

            ~Sound()
            {
                if (sound)
                    Mix_FreeChunk(sound);
            }

            void play(LoopMode loop = LOOP_NONE)
            {
                Mix_PlayChannel(-1, sound, loop);
            }

            void set_volume(float volume)
            {
                if (!sound)
                    return;

                if (volume < 0.0f) volume = 0.0f;
                if (volume > 1.0f) volume = 1.0f;

                int vol = static_cast<int>(volume * static_cast<float>(MIX_MAX_VOLUME) + 0.5f);
                Mix_VolumeChunk(sound, vol);
            }

        private:
            Mix_Chunk* sound = NULL;
        };

        class Music
        {
        public:
            Music(const std::string& filename)
            {
                music = Mix_LoadMUS(filename.c_str());
                if (!music)
                {
                    std::cerr << "Failed to load music: " << filename << " Error: " << Mix_GetError() << std::endl;
                }
            }

            ~Music()
            {
                if (music)
                    Mix_FreeMusic(music);
            }

            void play(LoopMode loop = LOOP_NONE)
            {
                if (music)
                {
                    if (Mix_PlayMusic(music, static_cast<int>(loop)) == -1)
                    {
                        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    }
                }
            }

            void pause()
            {
                Mix_PauseMusic();
            }

            void resume()
            {
                Mix_ResumeMusic();
            }

            void stop()
            {
                Mix_HaltMusic();
            }

            void set_volume(float volume)
            {
                if (volume < 0.0f) volume = 0.0f;
                if (volume > 1.0f) volume = 1.0f;

                int vol = static_cast<int>(volume * static_cast<float>(MIX_MAX_VOLUME) + 0.5f);
                Mix_VolumeMusic(vol);
            }

            float get_volume() const
            {
                int vol = Mix_VolumeMusic(-1);
                return static_cast<float>(vol) / static_cast<float>(MIX_MAX_VOLUME);
            }

            bool is_playing() const
            {
                return Mix_PlayingMusic() != 0;
            }
        private:
            Mix_Music* music = NULL;
        };
    }

    namespace random
    {
        static std::mt19937 rng(std::random_device{}());

        void seed(unsigned int s)
        {
            rng.seed(s);
        }

        float random() 
        {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            return dist(rng);
        }

        float uniform(float a, float b)
        {
            std::uniform_real_distribution<float> dist(a, b);
            return dist(rng);
        }

        int randint(int a, int b)
        {
            std::uniform_int_distribution<int> dist(a, b);
            return dist(rng);
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

        float get_fps() const { return currentFPS; }

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
            case SDL_QUIT:
                e.type = QUIT;
                return true;
            case SDL_KEYDOWN:
                e.type = KEYDOWN;
                e.key = sdlEvent.key.keysym.sym;
                return true;
            case SDL_KEYUP:
                e.type = KEYUP;
                e.key = sdlEvent.key.keysym.sym;
                return true;
            case SDL_MOUSEBUTTONDOWN:
                e.type = MOUSEDOWN;
                e.mouseX = sdlEvent.button.x;
                e.mouseY = sdlEvent.button.y;
                e.mouseButton = sdlEvent.button.button;
                return true;
            case SDL_MOUSEBUTTONUP:
                e.type = MOUSEUP;
                e.mouseX = sdlEvent.button.x;
                e.mouseY = sdlEvent.button.y;
                e.mouseButton = sdlEvent.button.button;
                return true;
            }
        }
        return false;
    }

    namespace mouse
    {
        Vec2 get_pos()
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            return { static_cast<float>(mx), static_cast<float>(my) };
        }
    }
}

#endif