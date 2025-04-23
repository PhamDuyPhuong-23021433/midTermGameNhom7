#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Setting {
public:
    Setting(SDL_Renderer* renderer);
    ~Setting();
    void run();
    bool isRunningState() const { return isRunning; } // Getter
    void setRunning(bool running) { isRunning = running; } // Setter

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    bool isRunning;

    int musicVolume;
    int sfxVolume;

    SDL_Rect musicSlider;
    SDL_Rect sfxSlider;
    SDL_Rect backButton;

    bool draggingMusic = false;
    bool draggingEffect = false;

    SDL_Texture* backgroundTexture;

    bool loadBackground(const char* path);
    void renderText(const std::string& text, int x, int y);
    void renderSlider(SDL_Rect slider, int value);
};