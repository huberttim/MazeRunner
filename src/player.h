
#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

struct Player
{
    int gridRow;
    int gridCol;

    Vector2 position;
    Vector2 targetPosition;

    // Actual movement direction
    Vector2 facingDirection;

    // Smooth visual direction
    Vector2 visualFacingDirection;

    float speed;
    float radius;

    // Animation state
    float animationTime;
    float movementBlend;
    float turnPulse;

    bool isMoving;
};

// Player functions
void InitializePlayer(Player &player);

void UpdatePlayer(Player &player);

void DrawPlayer(const Player &player);

#endif
