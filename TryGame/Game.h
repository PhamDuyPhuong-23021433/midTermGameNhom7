#pragma once
#include <SDL.h>
#include <vector>
#include <memory> // Thêm để sử dụng std::unique_ptr
#include "Background.h"
#include "Player.h"
#include "Item.h"
#include "point.h"
#include "menu.h"
#include "score.h"
#include "setting.h"
#include "story.h"
#include "PauseScreen.h"
#include "GameOverScreen.h"
#include <SDL_mixer.h>

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();

private:
    enum class GameState {
        MENU,
        PLAYING,
        HIGH_SCORES,
        SETTINGS,
        STORY,
        PAUSE,
        GAME_OVER,
        EXIT
    };

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    // Sử dụng std::unique_ptr để quản lý tài nguyên
    std::unique_ptr<Background> background;
    std::unique_ptr<Player> player;
    std::unique_ptr<Score> score;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<ScoreManager> scoreManager;
    std::unique_ptr<Setting> setting;
    std::unique_ptr<Story> story;
    std::unique_ptr<PauseScreen> pauseScreen;
    std::unique_ptr<GameOverScreen> gameOverScreen;

    std::vector<Item*> items;
    int itemSpawnCounter;

    int gameSpeed;
    int frameCount;
    bool isSlowed;
    int slowTimer;
    bool isDoubleScore;
    int doubleScoreTimer;

    GameState currentState;
    bool startGame;
    int menuOption;
    bool resumeGame;
    bool returnToMenu;
    bool replayGame;

    Mix_Chunk* clickSound;
    Mix_Chunk* gameOverSound;

    void handleEvents();
    void update();
    void render();
    void spawnItem();
    void checkCollisions();
    void playClickSound();
    void playGameOverSound();
};