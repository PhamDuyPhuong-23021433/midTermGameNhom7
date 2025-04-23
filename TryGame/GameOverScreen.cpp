#include "GameOverScreen.h"
#include <SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

GameOverScreen::GameOverScreen(SDL_Renderer* renderer, ScoreManager* scoreManager)
    : renderer(renderer), scoreManager(scoreManager) {
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
        throw std::runtime_error("TTF initialization failed");
    }

    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        throw std::runtime_error("Font loading failed");
    }

    textColor = { 255, 255, 255, 255 }; // Màu trắng cho văn bản

    // Tải hình ảnh nền cho màn hình game over
    SDL_Surface* backgroundSurface = IMG_Load("..\\assets\\img\\game_over.png");
    if (!backgroundSurface) {
        std::cerr << "Failed to load game over background: " << IMG_GetError() << std::endl;
    }
    else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        SDL_SetTextureBlendMode(backgroundTexture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(backgroundSurface);
        if (!backgroundTexture) {
            std::cerr << "Failed to create game over background texture: " << SDL_GetError() << std::endl;
        }
    }

    // Tạo các nút
    buttons = {
        {{150, 300, 100, 100}, "Replay", {100, 100, 100, 255}, {255, 255, 255, 255}, false}, // Biểu tượng Play
        {{350, 300, 100, 100}, "Home", {100, 100, 100, 255}, {255, 255, 255, 255}, false}   // Biểu tượng Home
    };

    // Tải texture cho các nút
    for (const auto& button : buttons) {
        std::string imagePath = button.text == "Replay" ? "..\\assets\\img\\again.png" : "..\\assets\\img\\home_black.png";
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (surface) {
            SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            buttonTextures.push_back(texture);
            SDL_FreeSurface(surface);
        }
        else {
            std::cerr << "Failed to load button image: " << IMG_GetError() << std::endl;
        }
    }
}

GameOverScreen::~GameOverScreen() {
    clean();
}

void GameOverScreen::handleEvent(const SDL_Event& e, bool& isRunning, bool& replayGame, bool& returnToMenu) {
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
                if (i == 0) { // Nút Replay
                    replayGame = true;
                }
                else if (i == 1) { // Nút Home
                    returnToMenu = true;
                }
                break;
            }
        }
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        replayGame = true; // Nhấn Space để chơi lại
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        returnToMenu = true; // Nhấn ESC để về menu
    }
}

void GameOverScreen::render(SDL_Renderer* renderer, int currentScore) {
    // Vẽ hình ảnh nền
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    // Vẽ điểm số hiện tại
    std::string scoreText = "Your Score: " + std::to_string(currentScore);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), { 0, 0, 0, 255 });
    if (scoreSurface) {
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = { (SCREEN_WIDTH - scoreSurface->w) / 2, 210, scoreSurface->w, scoreSurface->h };
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);
    }

    // Vẽ điểm cao nhất
    std::string highScoreText = "High Score: ";
    const auto& highScores = scoreManager->getHighScores();
    highScoreText += highScores.empty() ? "0" : std::to_string(highScores[0]);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), { 0, 0, 0, 255 });
    if (highScoreSurface) {
        SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
        SDL_Rect highScoreRect = { (SCREEN_WIDTH - highScoreSurface->w) / 2, 250, highScoreSurface->w, highScoreSurface->h };
        SDL_RenderCopy(renderer, highScoreTexture, nullptr, &highScoreRect);
        SDL_FreeSurface(highScoreSurface);
        SDL_DestroyTexture(highScoreTexture);
    }

    // Vẽ các nút
    for (size_t i = 0; i < buttons.size(); ++i) {
        SDL_Color color = buttons[i].isHovered ? buttons[i].hoverColor : buttons[i].normalColor;
        SDL_SetTextureColorMod(buttonTextures[i], color.r, color.g, color.b);
        SDL_RenderCopy(renderer, buttonTextures[i], nullptr, &buttons[i].rect);
    }
}

void GameOverScreen::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    for (auto& texture : buttonTextures) {
        SDL_DestroyTexture(texture);
    }
    buttonTextures.clear();
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

SDL_Texture* GameOverScreen::createTextTexture(const std::string& text, SDL_Color color) {
    if (text.empty() || !font) return nullptr;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}