#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Story {
public:
    Story(SDL_Renderer* renderer);
    ~Story();

    bool showStory(); // Trả về false khi muốn quay lại menu

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;

    std::vector<std::string> lines;  // Lưu trữ các dòng câu chuyện

    SDL_Rect backRect;  // Nút Back
    SDL_Texture* backgroundTexture;  // Hình nền

    // Hàm render văn bản
    void renderText(const std::string& text, int x, int y);

    // Hàm chia câu văn thành các đoạn nhỏ
    std::vector<std::string> wrapText(const std::string& text, int maxWidth);
};
