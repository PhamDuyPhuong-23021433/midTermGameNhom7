#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>

const int LANE_WIDTH = 150; // Chiều rộng mỗi làn
const int laneX[3] = { 160, 300, 440 };  // Điều chỉnh dựa trên vị trí thực tế của làn

class Player {
public:
    Player();
    bool init(SDL_Renderer* renderer, const std::string& file);
    void handleEvent(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);
    void clean();
    SDL_Rect getRect() const;

private:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;
    int frame, frameDelay, frameCounter;
    int frameWidth, frameHeight;
    int lane;
    float scale;
};