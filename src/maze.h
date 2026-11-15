
#ifndef MAZE_H
#define MAZE_H

#include "raylib.h"

const int ROWS = 11;
const int COLS = 15;
const int TILE_SIZE = 50;

const int MAZE_OFFSET_X = 125;
const int MAZE_OFFSET_Y = 75;

enum TileType
{
    PATH = 0,
    WALL = 1,
    COLLECTIBLE = 2,
    EXIT = 3,
    PLAYER_START = 4
};

extern int maze[ROWS][COLS];

void DrawMaze(bool exitUnlocked, float animationTime);

void DrawMazeTile(
    int row,
    int col,
    int tileType,
    bool exitUnlocked,
    float animationTime
);

void SetMazeLevel(int level);
void ResetMaze();

// Validate an individual layout before loading it.
bool ValidateMazeLevel(int level);

#endif
