#pragma once
#include <SDL.h>

class Background {
public:
    Background(SDL_Renderer* renderer, const char* imagePath1, const char* imagePath2);
    ~Background();

    void update(int speed);
    void render(SDL_Renderer* renderer, int screenWidth, int screenHeight);

private:
    SDL_Texture* backgroundTexture1; // map.png
    SDL_Texture* backgroundTexture2; 
    SDL_Texture* currentTexture;     // Texture đang sử dụng
    int scrollY;
    int loopCount;                   // Đếm số vòng cuộn
    bool isFirstTexture;             // True: dùng backgroundTexture1, False: dùng backgroundTexture2
};