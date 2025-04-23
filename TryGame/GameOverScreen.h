#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "score.h" // Thêm header để sử dụng ScoreManager

class GameOverScreen {
public:
    GameOverScreen(SDL_Renderer* renderer, ScoreManager* scoreManager);
    ~GameOverScreen();

    void handleEvent(const SDL_Event& e, bool& isRunning, bool& replayGame, bool& returnToMenu);
    void render(SDL_Renderer* renderer, int currentScore);
    void clean();

private:
    struct Button {
        SDL_Rect rect;
        std::string text; // "Replay" hoặc "Home"
        SDL_Color normalColor;
        SDL_Color hoverColor;
        bool isHovered;
    };

    SDL_Renderer* renderer;
    ScoreManager* scoreManager; // Thêm con trỏ ScoreManager
    TTF_Font* font;
    SDL_Color textColor;
    SDL_Texture* backgroundTexture; // Nền của màn hình game over
    std::vector<Button> buttons;
    std::vector<SDL_Texture*> buttonTextures;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
};