
#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"

struct Player;

const int MAX_BOSS_PROJECTILES = 24;

struct BossProjectile
{
    Vector2 position;
    Vector2 velocity;

    float radius;
    float lifetime;

    bool active;
};

struct Boss
{
    int gridRow;
    int gridCol;

    Vector2 position;

    float radius;
    float animationTime;

    float attackTimer;
    float attackInterval;

    int attackPattern;

    // Boss movement
    int movementTargetIndex;
    float movementSpeed;

    // Attack warning state
    float warningTimer;
    bool warningActive;

    bool active;

    BossProjectile projectiles[MAX_BOSS_PROJECTILES];
};

void InitializeBoss(Boss &boss);

void UpdateBoss(Boss &boss, const Player &player);

void DrawBoss(const Boss &boss);

void DrawBossProjectiles(const Boss &boss);

bool IsBossProjectileCollidingWithPlayer(
    Boss &boss,
    const Player &player
);

#endif
