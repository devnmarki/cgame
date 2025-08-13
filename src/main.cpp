#include <iostream>
#include <sstream>
#include <iomanip>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "../include/cgame.hpp"

int main(int argc, char* argv[]) 
{
    cgame::init();

    cgame::Window window(1280, 720, "CGame example");
    cgame::Clock clock;

    cgame::Surface display(window.get_renderer(), window.get_width() / 2, window.get_height() / 2);
    
    cgame::Surface playerImage = cgame::image::load(window.get_renderer(), "assets/images/player.png");
    cgame::Rect playerRect = playerImage.get_rect(100, 50);
    
    cgame::Surface blueBox(window.get_renderer(), 50, 100);
    blueBox.fill({ 0, 0, 255 });
    cgame::Rect blueBoxRect = blueBox.get_rect(400, 50);

    float x = 50;
    int movement[2] = { false, false };

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
                if (e.key == SDLK_A)
                    movement[0] = true;
                if (e.key == SDLK_D)
                    movement[1] = true;
            }
            if (e.type == cgame::KEYUP)
            {
                if (e.key == SDLK_A)
                    movement[0] = false;
                if (e.key == SDLK_D)
                    movement[1] = false;
            }
        }

        window.begin_frame();

        display.fill({ 0, 255, 0 });

        if (playerRect.colliderect(blueBoxRect))
        {
            std::cout << "collision is happening!" << std::endl;
        }

        cgame::Vec2 mp = cgame::mouse::get_pos();
        if (blueBoxRect.collidepoint(mp.x / 2, mp.y / 2))
        {
            std::cout << "point collision is happening!" << std::endl;
        }

        playerRect.set_left(playerRect.left() + (movement[1] - movement[0]) * 3);

        display.blit(playerImage, playerRect);
        display.blit(blueBox, 400, 50);
        
        window.blit(cgame::transform::scale(display, window.get_width(), window.get_height()), 0, 0);

        window.end_frame();

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(0) << clock.getFPS();
        window.set_title(("CGame but fast | FPS: " + ss.str()).c_str());

        float dt = clock.tick(60);
    }

    cgame::quit();

    return 0;
}