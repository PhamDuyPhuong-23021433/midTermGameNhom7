#include "menu.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cmath>

Menu::Menu(SDL_Renderer* renderer) {
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    backgroundTexture = IMG_LoadTexture(renderer, "..\\assets\\img\\menu.png");
    if (!backgroundTexture) {
        std::cerr << "Failed to load menu background: " << IMG_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "Failed to initialize audio: " << Mix_GetError() << std::endl;
    }

    backgroundMusic = Mix_LoadMUS("..\\assets\\audio\\Soundtrack - Bouncing Balls.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }
    else {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    clickSound = Mix_LoadWAV("..\\assets\\audio\\click.wav");
    if (!clickSound) {
        std::cerr << "Failed to load click sound: " << Mix_GetError() << std::endl;
    }

    buttons = {
        { {200, 250, 200, 50}, "Play",    {255, 255, 255, 255}, {200, 200, 200, 255}, false },
        { {200, 320, 200, 50}, "Score",   {255, 255, 255, 255}, {200, 200, 200, 255}, false },
        { {200, 390, 200, 50}, "Setting", {255, 255, 255, 255}, {200, 200, 200, 255}, false },
        { {200, 460, 200, 50}, "Story",   {255, 255, 255, 255}, {200, 200, 200, 255}, false },
        { {200, 530, 200, 50}, "Exit",    {255, 255, 255, 255}, {200, 200, 200, 255}, false }
    };

    startTime = SDL_GetTicks();
}

Menu::~Menu() {
    clean();
}

void Menu::handleEvent(const SDL_Event& e, bool& isRunning, bool& startGame, int& menuOption) {
    if (e.type == SDL_MOUSEMOTION) {
        int x = e.motion.x;
        int y = e.motion.y;
        SDL_Point mousePoint = { x, y };
        for (auto& button : buttons) {
            button.isHovered = SDL_PointInRect(&mousePoint, &button.rect);
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;
        SDL_Point mousePoint = { x, y };
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (SDL_PointInRect(&mousePoint, &buttons[i].rect)) {
                playClickSound();
                menuOption = static_cast<int>(i);
                if (i == MENU_PLAY) startGame = true;
                break;
            }
        }
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
    }
}

void Menu::render(SDL_Renderer* renderer) {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    for (const auto& button : buttons) {
        SDL_Color color = button.isHovered ? button.hoverColor : button.normalColor;
        SDL_Texture* textTexture = createTextTexture(renderer, button.text, color);
        if (textTexture) {
            SDL_RenderCopy(renderer, textTexture, nullptr, &button.rect);
            SDL_DestroyTexture(textTexture);
        }
        else {
            std::cerr << "Failed to create text texture for button: " << button.text << std::endl;
        }
    }

    // Hiệu ứng lơ lửng ESC text
    Uint32 currentTime = SDL_GetTicks();
    float yOffset = 10 * sin(currentTime / 300.0f);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* escSurface = TTF_RenderText_Solid(font, "Press ESC to exit", textColor);
    if (escSurface) {
        SDL_Texture* escTexture = SDL_CreateTextureFromSurface(renderer, escSurface);
        int textW = escSurface->w;
        int textH = escSurface->h;
        SDL_FreeSurface(escSurface);

        SDL_Rect textRect = { (600 - textW) / 2, 700 + static_cast<int>(yOffset), textW, textH };
        SDL_RenderCopy(renderer, escTexture, nullptr, &textRect);
        SDL_DestroyTexture(escTexture);
    }
}

void Menu::clean() {
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (clickSound) Mix_FreeChunk(clickSound);
    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
    if (font) TTF_CloseFont(font);
    Mix_CloseAudio();
    TTF_Quit();
}

SDL_Texture* Menu::createTextTexture(SDL_Renderer* renderer, const std::string& text, SDL_Color color) {
    if (text.empty() || !font) return nullptr;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Menu::playClickSound() {
    if (clickSound) {
        Mix_PlayChannel(-1, clickSound, 0);
    }
}
