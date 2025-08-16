# cgame — a tiny Pygame-like SDL2 helper (C++)

<img alt="build status" src="https://img.shields.io/badge/build-local-brightgreen"> <img alt="license" src="https://img.shields.io/badge/license-MIT-blue">

A minimal, Pygame-style helper library written in modern C++ on top of SDL2, SDL_image, SDL_ttf and SDL_mixer.
This repo contains a small demo (`src/main.cpp`) that shows windowing, surfaces, transforms, fonts and sound.

## Quick overview
- `Window`, `Surface` abstractions for rendering
- Image loading + transforms (scale / rotate / flip)
- Font rendering helper and blended text
- Sound wrapper with normalized float volume (0.0–1.0)
- Event polling, `Rect`/`Vec2` helpers, and a `Clock` for FPS limiting

## Build & run (Windows PowerShell)
Prerequisites: a C++ toolchain (g++, clang or MSVC), `make` (or adapt), and SDL2 + SDL_image + SDL_ttf + SDL_mixer development libs. The project includes `inc/` and `lib/` used by the Makefile.

Open PowerShell in the project root and run:

```powershell
# build
make

# run
.\build\cgame.exe
```

If you use Visual Studio or CMake, adapt the build steps accordingly.

## Examples
Copy-pasteable snippets that show common tasks. These assume you have a `Window` bound to `screen` and a `Clock clock;`.

1) Basic setup

```c++
// create window and renderer
auto& screen = cgame::display::set_mode(1280, 720); // returns Window&
cgame::Clock clock;
```

2) Load an image and render each frame

```c++
// load once
cgame::Surface player = cgame::image::load(screen.get_renderer(), "assets/images/player.png");
cgame::Rect playerRect = player.get_rect(100, 150);

// main loop
while (running) {
    screen.begin_frame({0,0,0});
    screen.blit(player, playerRect.left(), playerRect.top());
    screen.end_frame();
    float dt = clock.tick(60); // cap at 60 FPS
}
```

3) Clock and FPS display

```c++
float dt = clock.tick(60); // tick and cap to 60
```

4) Scale, rotate and flip

```c++
// scale in-place
cgame::transform::scale(surface, 64, 128);

// rotate every frame
float rot = 0.0f; 
rot += 1.0f;
screen.blit(cgame::transform::rotate(surface, rot), 200, 100);

// flip horizontally
cgame::transform::flip(surface, true);
screen.blit(surface, 300, 100);
```

Note: transforms in this library modify the `Surface` in place and return a reference. Use moves or copies if you need the original preserved.

5) Alpha / fading

```c++
// alpha accepts 0..255
surface.set_alpha(155.0f);
screen.blit(surface, 100, 200);
```

6) Text rendering

```c++
cgame::font::Font font("assets/fonts/your_font_here.ttf", 24);
cgame::Surface text = font.render("Hello World", {255,255,255});
screen.blit(text, 10, 10);
```

7) Sound playback

```c++
cgame::mixer::Sound s("assets/sfx/your_sound_here.wav");
s.set_volume(0.5f); // 0..1
s.play();
```

## API notes & gotchas
- `Surface` is non-copyable and movable — it owns an `SDL_Texture*`. Pass by reference or move it.
- `display::set_mode` returns a reference to the created `Window`. Do not copy the returned `Window`.
- `Surface::set_alpha(float)` accepts 0..255 and clamps/normalizes input.
- `mixer::Sound::set_volume(float)` expects 0.0–1.0 and converts to SDL_mixer's range internally.
- Fonts: `font::Font::render(...)` returns a `Surface` that owns a texture — it will be destroyed when the Surface is destroyed or moved.

## Troubleshooting
- Audio errors like "Audio device hasn't been opened": ensure `Mix_OpenAudio` succeeds on init and runtime DLLs (`SDL2.dll`, `SDL2_mixer.dll`) are available.
- Disappearing sprites/alpha: use `set_alpha(0.0f..1.0f)` or `set_alpha(0..255)`; older buggy conversions were fixed.
- Crashes on exit: usually caused by copying `Window` or `Surface` (double-free). Use references/moves.

## Contributing
- Fork, branch, PR. Keep demos small and include update to `src/main.cpp` showing changes.