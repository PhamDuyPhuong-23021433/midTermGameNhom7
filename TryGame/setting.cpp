#include "setting.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cmath>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Setting::Setting(SDL_Renderer* renderer)
    : renderer(renderer), isRunning(true), musicVolume(50), sfxVolume(50) {
    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 24);
    textColor = { 255, 255, 255 };

    int sliderWidth = 300;
    int sliderHeight = 20;
    int sliderX = (SCREEN_WIDTH - sliderWidth) / 2;
    musicSlider = { sliderX, 300, sliderWidth, sliderHeight };
    sfxSlider = { sliderX, 400, sliderWidth, sliderHeight };

    int backWidth = 100;
    int backHeight = 40;
    int backX = (SCREEN_WIDTH - backWidth) / 2;
    int backY = SCREEN_HEIGHT - 160;
    backButton = { backX, backY, backWidth, backHeight };

    backgroundTexture = nullptr;
    if (!loadBackground("..\\assets\\img\\menu.png")) {
        std::cerr << "Failed to load background image." << std::endl;
    }

    Mix_VolumeMusic(musicVolume * MIX_MAX_VOLUME / 100);
    Mix_Volume(-1, sfxVolume * MIX_MAX_VOLUME / 100);
}

Setting::~Setting() {
    TTF_CloseFont(font);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
}

bool Setting::loadBackground(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!backgroundTexture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void Setting::renderText(const std::string& text, int x, int y) {
    SDL_Surface* shadowSurface = TTF_RenderText_Solid(font, text.c_str(), { 0, 0, 0 });
    SDL_Texture* shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect dstRect = { x, y, textSurface->w, textSurface->h };
    if (x == -1) dstRect.x = (SCREEN_WIDTH - textSurface->w) / 2;
    SDL_Rect shadowRect = dstRect; shadowRect.x += 2; shadowRect.y += 2;
    SDL_RenderCopy(renderer, shadowTexture, nullptr, &shadowRect);
    SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
    SDL_FreeSurface(textSurface); SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(shadowSurface); SDL_DestroyTexture(shadowTexture);
}

void Setting::renderSlider(SDL_Rect slider, int value) {
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &slider);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &slider);
    SDL_Rect handle = { slider.x + (value * slider.w / 100) - 10, slider.y - 5, 20, slider.h + 10 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &handle);
}

void Setting::run() {
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks();

    while (isRunning) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool hoveringBack = (mouseX >= backButton.x && mouseX <= backButton.x + backButton.w &&
            mouseY >= backButton.y && mouseY <= backButton.y + backButton.h);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= musicSlider.x && x <= musicSlider.x + musicSlider.w &&
                    y >= musicSlider.y && y <= musicSlider.y + musicSlider.h) {
                    draggingMusic = true;
                }
                else if (x >= sfxSlider.x && x <= sfxSlider.x + sfxSlider.w &&
                    y >= sfxSlider.y && y <= sfxSlider.y + sfxSlider.h) {
                    draggingEffect = true;
                }
                else if (hoveringBack) {
                    isRunning = false;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                draggingMusic = false;
                draggingEffect = false;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                int x = e.motion.x;
                if (draggingMusic) {
                    musicVolume = (x - musicSlider.x) * 100 / musicSlider.w;
                    musicVolume = std::max(0, std::min(100, musicVolume));
                    Mix_VolumeMusic(musicVolume * MIX_MAX_VOLUME / 100);
                }
                else if (draggingEffect) {
                    sfxVolume = (x - sfxSlider.x) * 100 / sfxSlider.w;
                    sfxVolume = std::max(0, std::min(100, sfxVolume));
                    Mix_Volume(-1, sfxVolume * MIX_MAX_VOLUME / 100);
                }
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                // Scroll to adjust slider
                if (mouseX >= musicSlider.x && mouseX <= musicSlider.x + musicSlider.w &&
                    mouseY >= musicSlider.y && mouseY <= musicSlider.y + musicSlider.h) {
                    musicVolume += e.wheel.y * 5;
                    musicVolume = std::max(0, std::min(100, musicVolume));
                    Mix_VolumeMusic(musicVolume * MIX_MAX_VOLUME / 100);
                }
                else if (mouseX >= sfxSlider.x && mouseX <= sfxSlider.x + sfxSlider.w &&
                    mouseY >= sfxSlider.y && mouseY <= sfxSlider.y + sfxSlider.h) {
                    sfxVolume += e.wheel.y * 5;
                    sfxVolume = std::max(0, std::min(100, sfxVolume));
                    Mix_Volume(-1, sfxVolume * MIX_MAX_VOLUME / 100);
                }
            }
        }

        if (backgroundTexture) SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        else { SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer); }

        renderText("Music Volume", -1, musicSlider.y - 30);
        renderSlider(musicSlider, musicVolume);
        renderText("SFX Volume", -1, sfxSlider.y - 30);
        renderSlider(sfxSlider, sfxVolume);

        SDL_Color backColor = hoveringBack ? SDL_Color{ 100,100,100,255 } : textColor;
        SDL_Surface* backSurface = TTF_RenderText_Solid(font, "Back", backColor);
        SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
        SDL_Rect backTextRect = { (SCREEN_WIDTH - backSurface->w) / 2, backButton.y + 5, backSurface->w, backSurface->h };
        SDL_RenderCopy(renderer, backTexture, nullptr, &backTextRect);
        SDL_FreeSurface(backSurface); SDL_DestroyTexture(backTexture);

        Uint32 elapsed = SDL_GetTicks() - startTime;
        int floatOffset = static_cast<int>(5 * sin(elapsed * 0.005));
        std::string escHint = "Press ESC to go back";
        SDL_Surface* hintSurface = TTF_RenderText_Solid(font, escHint.c_str(), textColor);
        SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(renderer, hintSurface);
        SDL_Rect hintRect = { (SCREEN_WIDTH - hintSurface->w) / 2, backButton.y + backButton.h + 20 + floatOffset, hintSurface->w, hintSurface->h };
        SDL_RenderCopy(renderer, hintTexture, nullptr, &hintRect);
        SDL_FreeSurface(hintSurface); SDL_DestroyTexture(hintTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}
