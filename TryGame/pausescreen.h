#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class PauseScreen {
public:
    PauseScreen(SDL_Renderer* renderer);
    ~PauseScreen();

    void handleEvent(const SDL_Event& e, bool& isRunning, bool& resumeGame, bool& returnToMenu);
    void render(SDL_Renderer* renderer, int currentScore);
    void clean();

private:
    struct Button {
        SDL_Rect rect;
        std::string text; // "Continue" hoặc "Home"
        SDL_Color normalColor;
        SDL_Color hoverColor;
        bool isHovered;
    };

    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    SDL_Texture* backgroundTexture; // Nền của màn hình tạm dừng
    std::vector<Button> buttons;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
};