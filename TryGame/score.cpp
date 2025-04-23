#include "score.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cmath>

ScoreManager::ScoreManager(const std::string& filename)
    : filename(filename) {
    loadScores();
}

void ScoreManager::loadScores() {
    scores.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open score file: " << filename << std::endl;
        return;
    }
    int score;
    while (file >> score) scores.push_back(score);
    file.close();
    sortScores();
}

void ScoreManager::saveScores() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not write to score file: " << filename << std::endl;
        return;
    }
    for (int s : scores) file << s << '\n';
}

void ScoreManager::addScore(int score) {
    scores.push_back(score);
    sortScores();
    if (scores.size() > MAX_SCORES) scores.resize(MAX_SCORES);
    saveScores();
}

const std::vector<int>& ScoreManager::getHighScores() const {
    return scores;
}

void ScoreManager::sortScores() {
    std::sort(scores.begin(), scores.end(), std::greater<int>());
}

bool ScoreManager::showHighScores(SDL_Renderer* renderer) {
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    TTF_Font* font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 20);
    if (!font) {
        std::cerr << "Failed to load font for scores: " << TTF_GetError() << std::endl;
        return false;
    }

    // Tải ảnh menu.png
    SDL_Surface* menuSurface = IMG_Load("..\\assets\\img\\menu.png");
    if (!menuSurface) {
        std::cerr << "Failed to load menu.png: " << IMG_GetError() << std::endl;
        TTF_CloseFont(font);
        return false;
    }
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_FreeSurface(menuSurface);
    if (!menuTexture) {
        std::cerr << "Failed to create texture from menu.png: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        return false;
    }

    SDL_Event e;
    bool showing = true;
    SDL_Color white = { 255, 255, 255 };
    SDL_Rect backRect = { 200, 650, 200, 50 };
    Uint32 startTime = SDL_GetTicks();

    while (showing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                showing = false;
                break;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                showing = false;
                break;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= backRect.x && x <= backRect.x + backRect.w &&
                    y >= backRect.y && y <= backRect.y + backRect.h) {
                    showing = false;
                    break;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render hình nền menu.png
        SDL_Rect menuDst = { 0, 0, 600, 750 }; // Giả sử kích thước màn hình là 600x750
        SDL_RenderCopy(renderer, menuTexture, nullptr, &menuDst);

        // Render danh sách điểm
        for (size_t i = 0; i < scores.size(); ++i) {
            std::string text = std::to_string(i + 1) + ". Score: " + std::to_string(scores[i]);
            SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), white);
            if (!surf) {
                std::cerr << "Failed to render score text: " << TTF_GetError() << std::endl;
                continue;
            }
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (!tex) {
                std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(surf);
                continue;
            }
            SDL_Rect dst = { 170, 250 + int(i) * 40, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        // Check hover state for Back button
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        bool hoveringBack = (mx >= backRect.x && mx <= backRect.x + backRect.w &&
            my >= backRect.y && my <= backRect.y + backRect.h);

        // Render 'Back' text with hover darkening
        SDL_Color backColor = hoveringBack ? SDL_Color{ 150, 150, 150, 255 } : white;
        SDL_Surface* backSurf = TTF_RenderText_Solid(font, "Back", backColor);
        if (backSurf) {
            SDL_Texture* backTex = SDL_CreateTextureFromSurface(renderer, backSurf);
            if (backTex) {
                SDL_Rect backDst = { backRect.x + (backRect.w - backSurf->w) / 2,
                                    backRect.y + (backRect.h - backSurf->h) / 2,
                                    backSurf->w, backSurf->h };
                SDL_RenderCopy(renderer, backTex, nullptr, &backDst);
                SDL_DestroyTexture(backTex);
            }
            SDL_FreeSurface(backSurf);
        }

        // Render ESC hint with floating effect
        Uint32 elapsed = SDL_GetTicks() - startTime;
        int offset = int(5 * sin(elapsed * 0.005));
        std::string hint = "Press ESC to go back";
        SDL_Surface* hintSurf = TTF_RenderText_Solid(font, hint.c_str(), white);
        if (hintSurf) {
            SDL_Texture* hintTex = SDL_CreateTextureFromSurface(renderer, hintSurf);
            if (hintTex) {
                SDL_Rect hintDst = { (600 - hintSurf->w) / 2,
                                    backRect.y + backRect.h + 20 + offset,
                                    hintSurf->w, hintSurf->h };
                SDL_RenderCopy(renderer, hintTex, nullptr, &hintDst);
                SDL_DestroyTexture(hintTex);
            }
            SDL_FreeSurface(hintSurf);
        }

        // Present
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(menuTexture);
    TTF_CloseFont(font);
    return false; // Quay lại menu
}