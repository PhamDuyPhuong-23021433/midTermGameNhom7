#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Score {
public:
    Score(SDL_Renderer* renderer, const std::string& fontPath, int fontSize = 24);
    ~Score();

    void add(int value);               // Cộng điểm
    void reset();                      // Reset điểm
    void render(SDL_Renderer* renderer); // Vẽ điểm số
    int getScore() const;

private:
    int score;
    TTF_Font* font;
    SDL_Color color;
    std::string fontPath;
    int fontSize;
};
