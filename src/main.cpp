#include <iostream>
#include <sstream>
#include <iomanip>

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "../include/cgame.hpp"

int main(int argc, char* argv[]) 
{
    cgame::init();

    cgame::Window screen = cgame::display::set_mode(1280, 720);
    cgame::Clock clock;

    cgame::Surface display(screen.get_renderer(), screen.get_width() / 2, screen.get_height() / 2);
    
    cgame::Surface playerImage = cgame::image::load(screen.get_renderer(), "assets/images/player.png");
    cgame::Rect playerRect = playerImage.get_rect(100, 150);
    
    cgame::Surface blueBox(screen.get_renderer(), 50, 100);
    blueBox.fill({ 0, 0, 255 });
    cgame::Rect blueBoxRect = blueBox.get_rect(400, 80);

    cgame::font::Font testFont = cgame::font::Font("assets/fonts/MedodicaRegular.otf", 24);

    int randomInteger = cgame::random::randint(5, 10);
    float randomFloat = cgame::random::random();
    float randomFloatRange = cgame::random::uniform(5, 10);

    std::cout << "randint: " << randomInteger << std::endl;
    std::cout << "random: " << randomFloat << std::endl;
    std::cout << "random range: " << randomFloatRange << std::endl;

    float x = 50;
    int movement[2] = { false, false };
    float rot = 0.0f;

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

        screen.begin_frame();

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

        cgame::draw::rect(display, playerRect, { 255, 0, 0 });

        cgame::Surface text = testFont.render("Hello World!", { 255, 255, 255 });

        rot++;
        display.blit(cgame::transform::rotate(cgame::transform::flip(playerImage, true), rot), playerRect);
        display.blit(blueBox, blueBoxRect);
        display.blit(text, 50, 50);
        
        screen.blit(cgame::transform::scale(display, screen.get_width(), screen.get_height()), 0, 0);

        screen.end_frame();

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(0) << clock.get_fps();
        screen.set_title(("CGame but fast | FPS: " + ss.str()).c_str());

        float dt = clock.tick(60);
    }

    cgame::quit();

    return 0;
}