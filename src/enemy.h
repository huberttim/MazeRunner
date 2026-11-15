
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

struct Player;

const int MAX_ENEMIES = 5;

struct Enemy
{
    int gridRow;
    int gridCol;

    Vector2 position;
    Vector2 targetPosition;
    Vector2 previousPosition;

    float animationTime;

    int directionRow;
    int directionCol;

    float speed;
    float radius;

    bool isMoving;

    Color primaryColor;
    Color secondaryColor;
    Color coreColor;
};

// Single-enemy functions retained for compatibility.
void InitializeEnemy(Enemy &enemy);
void UpdateEnemy(Enemy &enemy, const Player &player);
void DrawEnemy(const Enemy &enemy);

bool IsEnemyCollidingWithPlayer(
    const Enemy &enemy,
    const Player &player
);

// Multi-enemy functions.
int InitializeEnemies(
    Enemy enemies[],
    int count,
    const Player &player
);

void UpdateEnemies(
    Enemy enemies[],
    int count,
    const Player &player
);

void DrawEnemies(
    const Enemy enemies[],
    int count
);

bool IsAnyEnemyCollidingWithPlayer(
    const Enemy enemies[],
    int count,
    const Player &player
);

#endif
