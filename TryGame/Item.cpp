#include "Item.h"
#include <SDL_image.h>
#include <cstdlib>

const int laneX[3] = { 160, 300, 440 };

Item::Item(ItemType type, SDL_Renderer* renderer, const std::string& path, int lane)
    : type(type), texture(nullptr)
{
    SDL_Surface* surface = IMG_Load(path.c_str());
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

    if (type == ItemType::PEACH || type == ItemType::BAMBOO || type == ItemType::HONEY) {
        rect.w = 80;
        rect.h = 80;
    }
    else {
        rect.w = 80;
        rect.h = 80;
    }

    rect.x = laneX[lane] - rect.w / 2;
    rect.y = -rand() % 300;
}

Item::~Item() {
    if (texture) SDL_DestroyTexture(texture);
}

void Item::update(int speed) {
    rect.y += speed;
}

void Item::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

SDL_Rect Item::getRect() const {
    return rect;
}

ItemType Item::getType() const {
    return type;
}
