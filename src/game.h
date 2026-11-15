#ifndef GAME_H
#define GAME_H

#include "player.h"

const int MAX_LEVELS = 10;
const int MAX_LIVES = 3;

struct LevelConfig
{
    int enemyCount;
    float enemySpeed;
    float timeLimit;
    bool isBossLevel;
};

extern const LevelConfig LEVEL_CONFIGS[MAX_LEVELS];

const LevelConfig &GetLevelConfig(int level);

enum GameStatus
{
    PLAYING,
    WON,
    LOST
};

enum Screen
{
    TITLE,
    GAMEPLAY,
    VICTORY,
    DEFEAT
};

struct GameState
{
    int score;
    int crystalsCollected;
    int totalCrystals;
    int lives;
    int currentLevel;
    int maxLevels;

    float invulnerabilityTimer;
    float timeRemaining;

    bool exitUnlocked;
    bool isPaused;

    GameStatus status;
};

void InitializeGame(GameState &game);
void InitializeLevel(GameState &game, int level);
void UpdateGame(GameState &game, Player &player);

void DrawHUD(const GameState &game);
void DrawGameOverlay(const GameState &game);
void DrawTitleScreen();

#endif
