#include "Background.h"
#include <SDL_image.h>
#include <iostream>

Background::Background(SDL_Renderer* renderer, const char* imagePath1, const char* imagePath2) {
    backgroundTexture1 = IMG_LoadTexture(renderer, imagePath1);
    if (backgroundTexture1 == nullptr) {
        std::cerr << "Không thể tải background 1: " << IMG_GetError() << std::endl;
    }
    backgroundTexture2 = IMG_LoadTexture(renderer, imagePath2);
    if (backgroundTexture2 == nullptr) {
        std::cerr << "Không thể tải background 2: " << IMG_GetError() << std::endl;
    }
    currentTexture = backgroundTexture1;
    isFirstTexture = true;
    scrollY = 0;
    loopCount = 0;
}

Background::~Background() {
    if (backgroundTexture1 != nullptr) {
        SDL_DestroyTexture(backgroundTexture1);
        backgroundTexture1 = nullptr;
    }
    if (backgroundTexture2 != nullptr) {
        SDL_DestroyTexture(backgroundTexture2);
        backgroundTexture2 = nullptr;
    }
}

void Background::update(int speed) {
    scrollY += speed;
}

void Background::render(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    if (currentTexture == nullptr) {
        return;
    }

    int texWidth, texHeight;
    SDL_QueryTexture(currentTexture, nullptr, nullptr, &texWidth, &texHeight);

    float aspectRatio = (float)texWidth / texHeight;
    int renderWidth = screenWidth;
    int renderHeight = static_cast<int>(screenWidth / aspectRatio);

    if (renderHeight < screenHeight) {
        renderHeight = screenHeight;
        renderWidth = static_cast<int>(screenHeight * aspectRatio);
    }

    if (scrollY >= renderHeight) {
        scrollY -= renderHeight;
        loopCount++;
        if (loopCount >= 15) {
            isFirstTexture = !isFirstTexture;
            currentTexture = isFirstTexture ? backgroundTexture1 : backgroundTexture2;
            loopCount = 0;
        }
    }

    SDL_Rect srcRect = { 0, 0, texWidth, texHeight };
    SDL_Rect destRect1 = { (screenWidth - renderWidth) / 2, scrollY - renderHeight, renderWidth, renderHeight };
    SDL_Rect destRect2 = { (screenWidth - renderWidth) / 2, scrollY, renderWidth, renderHeight };

    SDL_RenderCopy(renderer, currentTexture, &srcRect, &destRect1);
    SDL_RenderCopy(renderer, currentTexture, &srcRect, &destRect2);
}