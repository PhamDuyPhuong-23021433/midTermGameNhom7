#include "Player.h"
#include <SDL_image.h>

const int FRAME_COUNT = 3;
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

SDL_Rect Player::getRect() const {
    return destRect;
}

Player::Player() :
    texture(nullptr), frame(0), frameDelay(6), frameCounter(0),
    frameWidth(0), frameHeight(0), lane(1), scale(2.5f) {
}

bool Player::init(SDL_Renderer* renderer, const std::string& file) {
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (!surface) return false;

    frameWidth = surface->w / FRAME_COUNT;
    frameHeight = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return false;

    srcRect = { 0, 0, frameWidth, frameHeight };

    int scaledWidth = static_cast<int>(frameWidth * scale);
    int scaledHeight = static_cast<int>(frameHeight * scale);

    lane = 1;
    destRect = {
        laneX[lane] - scaledWidth / 2 + 10,
        590,
        scaledWidth,
        scaledHeight
    };

    return true;
}

void Player::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
            if (lane > 0) lane--;
            break;
        case SDLK_RIGHT:
            if (lane < 2) lane++;
            break;
        }
        int scaledWidth = static_cast<int>(frameWidth * scale);
        destRect.x = laneX[lane] - scaledWidth / 2;
    }
}

void Player::update() {
    frameCounter++;
    if (frameCounter >= frameDelay) {
        frame = (frame + 1) % FRAME_COUNT;
        srcRect.x = frame * frameWidth;
        frameCounter = 0;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

void Player::clean() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}