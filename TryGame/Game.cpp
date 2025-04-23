#include "Game.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <ctime>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Game::Game()
    : window(nullptr), renderer(nullptr), isRunning(false),
    background(nullptr), player(nullptr), score(nullptr),
    menu(nullptr), scoreManager(nullptr), setting(nullptr), story(nullptr),
    pauseScreen(nullptr), gameOverScreen(nullptr),
    itemSpawnCounter(0), gameSpeed(5), frameCount(0),
    isSlowed(false), slowTimer(0), isDoubleScore(false),
    doubleScoreTimer(0), currentState(GameState::MENU),
    startGame(false), menuOption(-1), resumeGame(false), returnToMenu(false), replayGame(false),
    clickSound(nullptr), gameOverSound(nullptr) {
}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        return false;
    }

    srand(static_cast<unsigned int>(time(0)));

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo các thành phần bằng std::unique_ptr
    menu = std::make_unique<Menu>(renderer);
    scoreManager = std::make_unique<ScoreManager>("..\\assets\\score\\highscore.txt");
    setting = std::make_unique<Setting>(renderer);
    story = std::make_unique<Story>(renderer);
    pauseScreen = std::make_unique<PauseScreen>(renderer);
    gameOverScreen = std::make_unique<GameOverScreen>(renderer, scoreManager.get());
    background = std::make_unique<Background>(renderer, "..\\assets\\img\\map.png", "..\\assets\\img\\map_winter.png");
    player = std::make_unique<Player>();
    if (!player->init(renderer, "..\\assets\\img\\player.png")) { // Thay đổi từ Run.png thành player.png
        std::cerr << "Failed to load player\n";
        return false;
    }
    score = std::make_unique<Score>(renderer, "..\\assets\\fonts\\ARCADECLASSIC.TTF");

    // Tải âm thanh click.wav
    clickSound = Mix_LoadWAV("..\\assets\\audio\\click.wav");
    if (!clickSound) {
        std::cerr << "Failed to load click sound: " << Mix_GetError() << std::endl;
    }

    // Tải âm thanh game_over.wav
    gameOverSound = Mix_LoadWAV("..\\assets\\audio\\game_over.wav");
    if (!gameOverSound) {
        std::cerr << "Failed to load game over sound: " << Mix_GetError() << std::endl;
    }

    return true;
}

void Game::run() {
    isRunning = true;
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::handleEvents() {
    SDL_Event e;
    bool eventProcessed = false;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
            currentState = GameState::EXIT;
            break;
        }

        switch (currentState) {
        case GameState::MENU:
            menu->handleEvent(e, isRunning, startGame, menuOption);
            eventProcessed = true;
            if (startGame || menuOption != -1) {
                switch (menuOption) {
                case MENU_PLAY:
                    currentState = GameState::PLAYING;
                    score->reset();
                    gameSpeed = 5;
                    frameCount = 0;
                    itemSpawnCounter = 0;
                    isSlowed = false;
                    isDoubleScore = false;
                    for (auto& item : items) delete item;
                    items.clear();
                    break;
                case MENU_SCORE:
                    currentState = GameState::HIGH_SCORES;
                    break;
                case MENU_SETTING:
                    currentState = GameState::SETTINGS;
                    break;
                case MENU_STORY:
                    currentState = GameState::STORY;
                    break;
                case MENU_EXIT:
                    isRunning = false;
                    currentState = GameState::EXIT;
                    break;
                }
                startGame = false;
                menuOption = -1;
            }
            break;
        case GameState::PLAYING:
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                currentState = GameState::PAUSE;
                eventProcessed = true;
            }
            else {
                player->handleEvent(e);
                eventProcessed = true;
            }
            break;
        case GameState::PAUSE:
            pauseScreen->handleEvent(e, isRunning, resumeGame, returnToMenu);
            eventProcessed = true;
            if (resumeGame) {
                currentState = GameState::PLAYING;
                resumeGame = false;
            }
            else if (returnToMenu) {
                scoreManager->addScore(score->getScore());
                score->reset();
                for (auto& item : items) delete item;
                items.clear();
                currentState = GameState::MENU;
                returnToMenu = false;
            }
            break;
        case GameState::GAME_OVER:
            gameOverScreen->handleEvent(e, isRunning, replayGame, returnToMenu);
            eventProcessed = true;
            if (replayGame) {
                currentState = GameState::PLAYING;
                score->reset();
                gameSpeed = 5;
                frameCount = 0;
                itemSpawnCounter = 0;
                isSlowed = false;
                isDoubleScore = false;
                for (auto& item : items) delete item;
                items.clear();
                replayGame = false;
            }
            else if (returnToMenu) {
                currentState = GameState::MENU;
                returnToMenu = false;
            }
            break;
        case GameState::HIGH_SCORES:
        case GameState::SETTINGS:
        case GameState::STORY:
            // Sự kiện sẽ được xử lý trong update() để đảm bảo đồng bộ
            break;
        case GameState::EXIT:
            break;
        }
    }
}

void Game::update() {
    switch (currentState) {
    case GameState::MENU:
        break;
    case GameState::PLAYING: {
        frameCount++;
        if (frameCount % 600 == 0 && gameSpeed < 20) {
            gameSpeed++;
        }

        if (isSlowed) {
            slowTimer--;
            if (slowTimer <= 0) {
                isSlowed = false;
            }
        }

        if (isDoubleScore) {
            doubleScoreTimer--;
            if (doubleScoreTimer <= 0) {
                isDoubleScore = false;
            }
        }

        int currentSpeed = isSlowed ? gameSpeed / 2 : gameSpeed;
        background->update(currentSpeed);
        player->update();
        spawnItem();
        checkCollisions();

        for (auto& item : items) {
            item->update(currentSpeed);
        }
        break;
    }
    case GameState::PAUSE:
    case GameState::GAME_OVER:
        break;
    case GameState::HIGH_SCORES:
        if (!scoreManager->showHighScores(renderer)) {
            currentState = GameState::MENU;
        }
        break;
    case GameState::SETTINGS:
        setting->run();
        if (!setting->isRunningState()) {
            currentState = GameState::MENU;
            setting->setRunning(true);
        }
        break;
    case GameState::STORY:
        if (!story->showStory()) {
            currentState = GameState::MENU;
        }
        break;
    case GameState::EXIT:
        break;
    }
}

void Game::spawnItem() {
    itemSpawnCounter++;
    if (itemSpawnCounter >= 90) {
        int lane = rand() % 3;
        int randItem = rand() % 100;

        ItemType type;
        std::string path;

        if (randItem < 5) {
            type = ItemType::X2;     path = "..\\assets\\img\\X2.png";
        }
        else if (randItem < 10) {
            type = ItemType::HONEY;  path = "..\\assets\\img\\clock.png";
        }
        else if (randItem < 32) {
            type = ItemType::PEACH;  path = "..\\assets\\img\\peach.png";
        }
        else if (randItem < 55) {
            type = ItemType::BAMBOO; path = "..\\assets\\img\\bamboo.png";
        }
        else if (randItem < 77) {
            type = ItemType::TRAP;   path = "..\\assets\\img\\trap.png";
        }
        else {
            type = ItemType::ROCK;   path = "..\\assets\\img\\rock.png";
        }

        Item* newItem = new Item(type, renderer, path, lane);
        items.push_back(newItem);
        itemSpawnCounter = 0;
    }
}

void Game::checkCollisions() {// kiem tra va cham
    for (auto it = items.begin(); it != items.end();) {
        SDL_Rect itemRect = (*it)->getRect();
        SDL_Rect playerRect = player->getRect();

        if (SDL_HasIntersection(&itemRect, &playerRect)) {
            ItemType type = (*it)->getType();
            if (type == ItemType::PEACH) {
                score->add(isDoubleScore ? 2 : 1);
                playClickSound();
            }
            else if (type == ItemType::BAMBOO) {
                score->add(isDoubleScore ? 4 : 2);
                playClickSound();
            }
            else if (type == ItemType::HONEY) {
                isSlowed = true;
                slowTimer = 5 * 60;
                playClickSound();
            }
            else if (type == ItemType::X2) {
                isDoubleScore = true;
                doubleScoreTimer = 10 * 60;
                playClickSound();
            }
            else if (type == ItemType::TRAP || type == ItemType::ROCK) {
                playGameOverSound();
                scoreManager->addScore(score->getScore());
                currentState = GameState::GAME_OVER;
                for (auto& item : items) delete item;
                items.clear();
                return;
            }
            delete* it;
            it = items.erase(it);
        }
        else if ((*it)->getRect().y > SCREEN_HEIGHT) {
            delete* it;
            it = items.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    switch (currentState) {
    case GameState::MENU:
        menu->render(renderer);
        break;
    case GameState::PLAYING:
        background->render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        for (auto& item : items) item->render(renderer);
        player->render(renderer);
        score->render(renderer);
        break;
    case GameState::PAUSE:
        background->render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        for (auto& item : items) item->render(renderer);
        player->render(renderer);
        score->render(renderer);
        pauseScreen->render(renderer, score->getScore());
        break;
    case GameState::GAME_OVER:
        background->render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        for (auto& item : items) item->render(renderer);
        player->render(renderer);
        score->render(renderer);
        gameOverScreen->render(renderer, score->getScore());
        break;
    case GameState::HIGH_SCORES:
    case GameState::STORY:
        // Đã render trong showHighScores hoặc showStory
        break;
    case GameState::SETTINGS:
        break;
    case GameState::EXIT:
        break;
    }
    if (currentState != GameState::SETTINGS) {
        SDL_RenderPresent(renderer);
    }
}

void Game::clean() {
    for (auto& item : items) delete item;
    items.clear();

    background.reset();
    player->clean(); // Gọi clean() trước khi reset
    player.reset();
    score.reset();
    menu.reset();
    scoreManager.reset();
    setting.reset();
    story.reset();
    pauseScreen.reset();
    gameOverScreen.reset();

    if (clickSound) {
        Mix_FreeChunk(clickSound);
        clickSound = nullptr;
    }
    if (gameOverSound) {
        Mix_FreeChunk(gameOverSound);
        gameOverSound = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::playClickSound() {
    if (clickSound) {
        Mix_PlayChannel(-1, clickSound, 0);
    }
}

void Game::playGameOverSound() {
    if (gameOverSound) {
        Mix_PlayChannel(-1, gameOverSound, 0);
    }
}