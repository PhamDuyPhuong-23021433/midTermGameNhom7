#include "PauseScreen.h"
#include <SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

PauseScreen::PauseScreen(SDL_Renderer* renderer) : renderer(renderer) {
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    textColor = { 255, 255, 255, 255 }; // Màu trắng cho văn bản

    // Tải hình ảnh nền cho màn hình tạm dừng
    SDL_Surface* backgroundSurface = IMG_Load("..\\assets\\img\\miniMenu.png");
    if (!backgroundSurface) {
        std::cerr << "Failed to load pause background: " << IMG_GetError() << std::endl;
    }
    else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        SDL_SetTextureBlendMode(backgroundTexture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(backgroundSurface);
        if (!backgroundTexture) {
            std::cerr << "Failed to create pause background texture: " << SDL_GetError() << std::endl;
        }
    }

    // Tạo các nút
    buttons = {
        {{150, 400, 100, 100}, "Continue", {100, 100, 100, 255}, {255, 255, 255, 255}, false}, // Biểu tượng Play
        {{350, 400, 100, 100}, "Home", {100, 100, 100, 255}, {255, 255, 255, 255}, false}     // Biểu tượng Home
    };
}

PauseScreen::~PauseScreen() {
    clean();
}

void PauseScreen::handleEvent(const SDL_Event& e, bool& isRunning, bool& resumeGame, bool& returnToMenu) {
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
                if (i == 0) { // Nút Continue
                    resumeGame = true;
                }
                else if (i == 1) { // Nút Home
                    returnToMenu = true;
                }
                break;
            }
        }
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        resumeGame = true; // Nhấn Space để tiếp tục
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        returnToMenu = true; // Nhấn ESC để về menu
    }
}

void PauseScreen::render(SDL_Renderer* renderer, int currentScore) {
    // Vẽ hình ảnh nền
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    // Vẽ chữ "PAUSE"
    SDL_Surface* pauseSurface = TTF_RenderText_Solid(font, " ", { 0, 0, 0, 255 });
    SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    SDL_Rect pauseRect = { (SCREEN_WIDTH - pauseSurface->w) / 2, 100, pauseSurface->w, pauseSurface->h };
    SDL_RenderCopy(renderer, pauseTexture, nullptr, &pauseRect);
    SDL_FreeSurface(pauseSurface);
    SDL_DestroyTexture(pauseTexture);

    // Vẽ điểm số
    std::string scoreText = "Your Score: " + std::to_string(currentScore);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), { 0, 0, 0, 255 });
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = { (SCREEN_WIDTH - scoreSurface->w) / 2, 300, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    // Vẽ các nút (sử dụng hình ảnh thay vì văn bản)
    for (const auto& button : buttons) {
        SDL_Color color = button.isHovered ? button.hoverColor : button.normalColor;
        std::string imagePath = button.text == "Continue" ? "..\\assets\\img\\continue_black.png" : "..\\assets\\img\\home_black.png";
        SDL_Surface* buttonSurface = IMG_Load(imagePath.c_str());
        if (buttonSurface) {
            SDL_SetColorKey(buttonSurface, SDL_TRUE, SDL_MapRGB(buttonSurface->format, 0, 0, 0));
            SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
            SDL_SetTextureColorMod(buttonTexture, color.r, color.g, color.b);
            SDL_RenderCopy(renderer, buttonTexture, nullptr, &button.rect);
            SDL_FreeSurface(buttonSurface);
            SDL_DestroyTexture(buttonTexture);
        }
    }
}

void PauseScreen::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

SDL_Texture* PauseScreen::createTextTexture(const std::string& text, SDL_Color color) {
    if (text.empty() || !font) return nullptr;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}