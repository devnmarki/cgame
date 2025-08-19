#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "../include/cgame.hpp"

static const float RENDER_SCALE = 2.0f;

int main(int argc, char* argv[]) 
{
    cgame::init();

    {
        cgame::Window& screen = cgame::display::set_mode(1280, 720);
        cgame::Clock clock;

        cgame::Surface display(screen.get_renderer(), screen.get_width() / 2, screen.get_height() / 2);
        
        cgame::Surface playerImage = cgame::image::load(screen.get_renderer(), "assets/images/player.png");
        playerImage.set_color({ 0, 0, 255 });
        cgame::Rect playerRect = playerImage.get_rect(100, 150);
        
        cgame::Surface blueBox(screen.get_renderer(), 50, 100);
        blueBox.fill({ 0, 0, 255 });
        cgame::Rect blueBoxRect = blueBox.get_rect(400, 80);

        cgame::font::Font testFont = cgame::font::Font("assets/fonts/MedodicaRegular.otf", 24);

        cgame::mixer::Sound jumpSound = cgame::mixer::Sound("assets/sfx/jump.wav");
        cgame::mixer::Sound deathSound = cgame::mixer::Sound("assets/sfx/death.wav");
        cgame::mixer::Sound hurtSound = cgame::mixer::Sound("assets/sfx/hurt.wav");
        hurtSound.set_volume(0.3f);

        cgame::mixer::Music music = cgame::mixer::Music("assets/music/rosalia.mp3");
        music.play();

        int randomInteger = cgame::random::randint(5, 10);
        float randomFloat = cgame::random::random();
        float randomFloatRange = cgame::random::uniform(5, 10);

        std::cout << "randint: " << randomInteger << std::endl;
        std::cout << "random: " << randomFloat << std::endl;
        std::cout << "random range: " << randomFloatRange << std::endl;

        float x = 50;
        int movement[4] = { false, false, false, false };
        float rot = 0.0f;

        bool running = true;
        
        while (running)
        {
            cgame::Event e;
            while (cgame::get_events(e))
            {
                if (e.type == cgame::QUIT)
                {
                    running = false;
                }
                if (e.type == cgame::KEYDOWN)
                {
                    if (e.key == SDLK_ESCAPE)
                        running = false;
                    if (e.key == SDLK_a)
                        movement[0] = true;
                    if (e.key == SDLK_d)
                        movement[1] = true;
                    if (e.key == SDLK_w)
                        movement[2] = true;
                    if (e.key == SDLK_s)
                        movement[3] = true;
                    if (e.key == SDLK_SPACE)
                        hurtSound.play();
                }
                if (e.type == cgame::KEYUP)
                {
                    if (e.key == SDLK_a)
                        movement[0] = false;
                    if (e.key == SDLK_d)
                        movement[1] = false;
                    if (e.key == SDLK_w)
                        movement[2] = false;
                    if (e.key == SDLK_s)
                        movement[3] = false;
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
            playerRect.set_top(playerRect.top() + (movement[3] - movement[2]) * 3);

            cgame::draw::rect(display, playerRect, { 255, 0, 0 });

            cgame::Surface text = testFont.render("Hello World!", { 255, 255, 255 });
            cgame::Surface centeredText = testFont.render("Centered Text", { 255, 0, 0 });

            playerImage.set_alpha(std::max(0.0f, 100.0f + playerRect.x));

            rot++;
            display.blit(cgame::transform::rotate(cgame::transform::flip(playerImage, true), rot), playerRect);
            display.blit(blueBox, blueBoxRect);
            display.blit(text, 50, 50);
            display.blit(centeredText, (display.get_width() / 2 - centeredText.get_width()) / RENDER_SCALE, (display.get_height() / 2 - centeredText.get_height()) / RENDER_SCALE);
            
            screen.blit(cgame::transform::scale(display, screen.get_width(), screen.get_height()), 0, 0);

            screen.end_frame();

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(0) << clock.get_fps();
            screen.set_title(("CGame but fast | FPS: " + ss.str()).c_str());

            float dt = clock.tick(60);
        }
    }

    cgame::quit();

    return 0;
}