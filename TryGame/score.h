#ifndef SCORE_H
#define SCORE_H

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class ScoreManager {
public:
    ScoreManager(const std::string& filename = "..\\assets\\score\\highscore.txt");

    void loadScores();
    void saveScores() const;
    void addScore(int score);
    const std::vector<int>& getHighScores() const;
    bool showHighScores(SDL_Renderer* renderer); // Trả về false khi muốn quay lại menu

private:
    std::string filename;
    std::vector<int> scores;
    static const int MAX_SCORES = 10;

    void sortScores();
};

#endif