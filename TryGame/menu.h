#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

// Thêm enum để dùng trong Game.cpp
enum MenuOption {
    MENU_PLAY = 0,
    MENU_SCORE = 1,
    MENU_SETTING = 2,
    MENU_STORY = 3,
    MENU_EXIT = 4
};

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();

    void handleEvent(const SDL_Event& e, bool& isRunning, bool& startGame, int& menuOption);
    void render(SDL_Renderer* renderer);
    void clean();

private:
    struct Button {
        SDL_Rect rect;
        std::string text;
        SDL_Color normalColor;
        SDL_Color hoverColor;
        bool isHovered;
    };

    SDL_Texture* createTextTexture(SDL_Renderer* renderer, const std::string& text, SDL_Color color);
    void playClickSound();

    std::vector<Button> buttons;
    TTF_Font* font;
    SDL_Texture* backgroundTexture;
    Mix_Chunk* clickSound;
    Mix_Music* backgroundMusic;

    Uint32 startTime;
};
