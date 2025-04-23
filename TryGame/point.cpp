#include "point.h"
#include <SDL_ttf.h>
#include <SDL.h>
#include <string>
#include <iostream>

Score::Score(SDL_Renderer* renderer, const std::string& fontPath, int fontSize)
    : score(0), fontPath(fontPath), fontSize(fontSize), color({ 255, 255, 255 }) // Trắng
{
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
}

Score::~Score() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

void Score::add(int value) {
    score += value;
}

void Score::reset() {
    score = 0;
}

int Score::getScore() const {
    return score;
}

void Score::render(SDL_Renderer* renderer) {
    std::string scoreText = "Score  " + std::to_string(score);
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = { 20, 20, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
