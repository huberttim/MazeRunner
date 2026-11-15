
#include "enemy.h"
#include "maze.h"
#include "player.h"
#include "pixel_art.h"

#include <cmath>
#include <cstdlib>
#include <queue>
#include <utility>

// --------------------------------------------------
// ENEMY COLOUR PALETTE
// --------------------------------------------------

const Color ENEMY_MAGENTA = {255, 45, 145, 255};
const Color ENEMY_PURPLE  = {160, 45, 255, 255};
const Color ENEMY_CORE    = {255, 210, 240, 255};

const Color ENEMY_CYAN    = {0, 210, 255, 255};
const Color ENEMY_BLUE    = {40, 100, 255, 255};

const Color ENEMY_ORANGE  = {255, 135, 35, 255};
const Color ENEMY_YELLOW  = {255, 220, 55, 255};

const Color ENEMY_RED     = {255, 55, 65, 255};
const Color ENEMY_WHITE   = {245, 245, 255, 255};

const Color ENEMY_GREEN   = {45, 255, 145, 255};

// --------------------------------------------------
// CONVERT MAZE COORDINATES TO SCREEN COORDINATES
// --------------------------------------------------

static Vector2 GetEnemyTileCenter(int row, int col)
{
    return {
        MAZE_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2.0f,
        MAZE_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2.0f
    };
}

// --------------------------------------------------
// CHECK WHETHER A TILE CAN BE ENTERED
// --------------------------------------------------

static bool CanEnemyMoveTo(int row, int col)
{
    if (
        row < 0 || row >= ROWS ||
        col < 0 || col >= COLS
    )
    {
        return false;
    }

    return maze[row][col] != WALL;
}

// --------------------------------------------------
// ASSIGN ENEMY COLOURS
// --------------------------------------------------

static void SetEnemyAppearance(Enemy &enemy, int index)
{
    switch (index % MAX_ENEMIES)
    {
        case 0:
            enemy.primaryColor = ENEMY_PURPLE;
            enemy.secondaryColor = ENEMY_MAGENTA;
            enemy.coreColor = ENEMY_CORE;
            break;

        case 1:
            enemy.primaryColor = ENEMY_BLUE;
            enemy.secondaryColor = ENEMY_CYAN;
            enemy.coreColor = WHITE;
            break;

        case 2:
            enemy.primaryColor = ENEMY_ORANGE;
            enemy.secondaryColor = ENEMY_YELLOW;
            enemy.coreColor = WHITE;
            break;

        case 3:
            enemy.primaryColor = ENEMY_RED;
            enemy.secondaryColor = ENEMY_ORANGE;
            enemy.coreColor = WHITE;
            break;

        case 4:
            enemy.primaryColor = ENEMY_GREEN;
            enemy.secondaryColor = ENEMY_CYAN;
            enemy.coreColor = WHITE;
            break;
    }
}

// --------------------------------------------------
// INITIALIZE ONE ENEMY AT A SPECIFIC TILE
// --------------------------------------------------

static void InitializeEnemyAt(
    Enemy &enemy,
    int row,
    int col,
    int index
)
{
    enemy.gridRow = row;
    enemy.gridCol = col;

    enemy.position = GetEnemyTileCenter(row, col);
    enemy.targetPosition = enemy.position;
    enemy.previousPosition = enemy.position;
    enemy.animationTime = 0.0f;

    enemy.directionRow = 0;
    enemy.directionCol = 0;

    enemy.speed = 110.0f;
    enemy.radius = 16.0f;

    enemy.isMoving = false;

    SetEnemyAppearance(enemy, index);
}

// --------------------------------------------------
// LEGACY SINGLE-ENEMY INITIALIZATION
// --------------------------------------------------

void InitializeEnemy(Enemy &enemy)
{
    InitializeEnemyAt(enemy, 9, 13, 0);
}

// --------------------------------------------------
// FIND SHORTEST PATH USING BREADTH-FIRST SEARCH
// --------------------------------------------------

static bool FindNextMove(
    const Enemy &enemy,
    const Player &player,
    int &nextRow,
    int &nextCol
)
{
    int distance[ROWS][COLS];
    int parentRow[ROWS][COLS];
    int parentCol[ROWS][COLS];

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            distance[row][col] = -1;
            parentRow[row][col] = -1;
            parentCol[row][col] = -1;
        }
    }

    std::queue<std::pair<int, int>> searchQueue;

    searchQueue.push({
        enemy.gridRow,
        enemy.gridCol
    });

    distance[enemy.gridRow][enemy.gridCol] = 0;

    const int rowChanges[4] = {-1, 1, 0, 0};
    const int colChanges[4] = {0, 0, -1, 1};

    bool targetFound = false;

    while (!searchQueue.empty())
    {
        std::pair<int, int> current = searchQueue.front();
        searchQueue.pop();

        int currentRow = current.first;
        int currentCol = current.second;

        if (
            currentRow == player.gridRow &&
            currentCol == player.gridCol
        )
        {
            targetFound = true;
            break;
        }

        for (int i = 0; i < 4; i++)
        {
            int newRow = currentRow + rowChanges[i];
            int newCol = currentCol + colChanges[i];

            if (!CanEnemyMoveTo(newRow, newCol))
                continue;

            if (distance[newRow][newCol] != -1)
                continue;

            distance[newRow][newCol] =
                distance[currentRow][currentCol] + 1;

            parentRow[newRow][newCol] = currentRow;
            parentCol[newRow][newCol] = currentCol;

            searchQueue.push({newRow, newCol});
        }
    }

    if (!targetFound)
        return false;

    if (
        enemy.gridRow == player.gridRow &&
        enemy.gridCol == player.gridCol
    )
    {
        return false;
    }

    // Reconstruct the shortest path.
    int pathRow = player.gridRow;
    int pathCol = player.gridCol;

    while (
        parentRow[pathRow][pathCol] != enemy.gridRow ||
        parentCol[pathRow][pathCol] != enemy.gridCol
    )
    {
        int previousRow = parentRow[pathRow][pathCol];
        int previousCol = parentCol[pathRow][pathCol];

        if (previousRow == -1 || previousCol == -1)
            return false;

        pathRow = previousRow;
        pathCol = previousCol;
    }

    nextRow = pathRow;
    nextCol = pathCol;

    return true;
}

// --------------------------------------------------
// UPDATE ONE ENEMY
// --------------------------------------------------

void UpdateEnemy(Enemy &enemy, const Player &player)
{
    float deltaTime = GetFrameTime();

    // Each enemy has its own animation clock.
    enemy.animationTime += deltaTime;

    // Store the previous position for movement trails.
    enemy.previousPosition = enemy.position;

    if (enemy.isMoving)
    {
        float dx = enemy.targetPosition.x - enemy.position.x;
        float dy = enemy.targetPosition.y - enemy.position.y;

        float distance = sqrtf(dx * dx + dy * dy);

        float movement = enemy.speed * deltaTime;

        if (distance <= movement || distance == 0.0f)
        {
            enemy.position = enemy.targetPosition;
            enemy.isMoving = false;
        }
        else
        {
            enemy.position.x += (dx / distance) * movement;
            enemy.position.y += (dy / distance) * movement;
        }

        return;
    }

    int nextRow;
    int nextCol;

    if (FindNextMove(enemy, player, nextRow, nextCol))
    {
        enemy.directionRow = nextRow - enemy.gridRow;
        enemy.directionCol = nextCol - enemy.gridCol;

        enemy.gridRow = nextRow;
        enemy.gridCol = nextCol;

        enemy.targetPosition = GetEnemyTileCenter(
            nextRow,
            nextCol
        );

        enemy.isMoving = true;
    }
}

// --------------------------------------------------
// DRAW ONE ENEMY
// --------------------------------------------------


void DrawEnemy(const Enemy &enemy)
{
    // --------------------------------------------------
    // PIXEL-ART ENEMY SPRITES
    // --------------------------------------------------

    static const char* ENEMY_FRAME_A[10] =
    {
        "..OO....OO",
        ".OPP..PPO.",
        "OPPPPPPPPO",
        "OPSSSSSSPO",
        "OPSSCCSSPO",
        "OPSSCCSSPO",
        "OPSSSSSSPO",
        ".OPP..PPO.",
        "..OO..OO..",
        ".OO....OO."
    };

    static const char* ENEMY_FRAME_B[10] =
    {
        "..OO....OO",
        ".OPP..PPO.",
        "OPPPPPPPPO",
        "OPSSSSSSPO",
        "OPSSCCSSPO",
        "OPSSCCSSPO",
        "OPSSSSSSPO",
        ".OPP..PPO.",
        ".OO....OO.",
        "..OO..OO.."
    };

    // Each enemy keeps its assigned colour identity.
    const PixelColor enemyPalette[] =
    {
        {'O', PIXEL_DARK_GRAY},
        {'P', enemy.primaryColor},
        {'S', enemy.secondaryColor},
        {'C', enemy.coreColor}
    };

    // --------------------------------------------------
    // FRAME ANIMATION
    // --------------------------------------------------

    int frame = static_cast<int>(enemy.animationTime * 7.0f) % 2;

    const char* const* sprite =
        (enemy.isMoving && frame == 1)
            ? ENEMY_FRAME_B
            : ENEMY_FRAME_A;

    // --------------------------------------------------
    // PIXEL-POSITIONED HOVER
    // --------------------------------------------------

    int hover = static_cast<int>(
        roundf(sinf(enemy.animationTime * 3.0f) * 1.0f)
    );

    int drawX = static_cast<int>(
        roundf(enemy.position.x)
    ) - 20;

    int drawY = static_cast<int>(
        roundf(enemy.position.y)
    ) - 20 + hover;

    // --------------------------------------------------
    // SQUARE MOVEMENT TRAIL
    // --------------------------------------------------

    if (enemy.isMoving)
    {
        int trailX = static_cast<int>(
            roundf(enemy.previousPosition.x)
        );

        int trailY = static_cast<int>(
            roundf(enemy.previousPosition.y)
        );

        DrawRectangle(
            trailX - 3,
            trailY - 3,
            6,
            6,
            enemy.secondaryColor
        );

        DrawRectangle(
            (trailX + drawX + 20) / 2 - 2,
            (trailY + drawY + 20 - hover) / 2 - 2,
            4,
            4,
            enemy.primaryColor
        );
    }

    // --------------------------------------------------
    // DRAW THE ENEMY SPRITE
    // --------------------------------------------------

    DrawPixelSprite(
        sprite,
        10,
        10,
        drawX,
        drawY,
        4,
        enemyPalette,
        sizeof(enemyPalette) / sizeof(enemyPalette[0])
    );
}


// --------------------------------------------------
// INITIALIZE MULTIPLE ENEMIES
// RETURNS THE NUMBER ACTUALLY INITIALIZED
// --------------------------------------------------

int InitializeEnemies(
    Enemy enemies[],
    int count,
    const Player &player
)
{
    if (count < 0)
        count = 0;

    if (count > MAX_ENEMIES)
        count = MAX_ENEMIES;

    if (count == 0)
        return 0;

    // Validate the player's grid position.
    if (
        player.gridRow < 0 || player.gridRow >= ROWS ||
        player.gridCol < 0 || player.gridCol >= COLS
    )
    {
        return 0;
    }

    // Calculate distances from the player using BFS.
    int distances[ROWS][COLS];

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            distances[row][col] = -1;
        }
    }

    std::queue<std::pair<int, int>> searchQueue;

    searchQueue.push({
        player.gridRow,
        player.gridCol
    });

    distances[player.gridRow][player.gridCol] = 0;

    const int rowChanges[4] = {-1, 1, 0, 0};
    const int colChanges[4] = {0, 0, -1, 1};

    while (!searchQueue.empty())
    {
        std::pair<int, int> current = searchQueue.front();
        searchQueue.pop();

        int row = current.first;
        int col = current.second;

        for (int i = 0; i < 4; i++)
        {
            int newRow = row + rowChanges[i];
            int newCol = col + colChanges[i];

            if (!CanEnemyMoveTo(newRow, newCol))
                continue;

            if (distances[newRow][newCol] != -1)
                continue;

            distances[newRow][newCol] =
                distances[row][col] + 1;

            searchQueue.push({newRow, newCol});
        }
    }

    // Track occupied spawn positions.
    bool occupied[ROWS][COLS] = {};

    occupied[player.gridRow][player.gridCol] = true;

    int initializedCount = 0;

    for (int i = 0; i < count; i++)
    {
        int bestRow = -1;
        int bestCol = -1;
        int bestDistance = -1;

        // Prefer positions far from the player
        // and separated from other enemies.
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (maze[row][col] != PATH)
                    continue;

                if (occupied[row][col])
                    continue;

                if (distances[row][col] < 0)
                    continue;

                bool tooClose = false;

                for (
                    int previous = 0;
                    previous < initializedCount;
                    previous++
                )
                {
                    int separation =
                        std::abs(row - enemies[previous].gridRow) +
                        std::abs(col - enemies[previous].gridCol);

                    if (separation < 3)
                    {
                        tooClose = true;
                        break;
                    }
                }

                if (tooClose)
                    continue;

                if (distances[row][col] > bestDistance)
                {
                    bestDistance = distances[row][col];
                    bestRow = row;
                    bestCol = col;
                }
            }
        }

        // Fallback: allow closer positions if
        // there are not enough widely separated tiles.
        if (bestRow == -1)
        {
            for (int row = 0; row < ROWS; row++)
            {
                for (int col = 0; col < COLS; col++)
                {
                    if (
                        maze[row][col] != PATH ||
                        occupied[row][col] ||
                        distances[row][col] < 0
                    )
                    {
                        continue;
                    }

                    if (distances[row][col] > bestDistance)
                    {
                        bestDistance = distances[row][col];
                        bestRow = row;
                        bestCol = col;
                    }
                }
            }
        }

        // Stop safely if no valid tile remains.
        if (bestRow == -1)
            break;

        InitializeEnemyAt(
            enemies[initializedCount],
            bestRow,
            bestCol,
            initializedCount
        );

        occupied[bestRow][bestCol] = true;

        initializedCount++;
    }

    return initializedCount;
}


// --------------------------------------------------
// UPDATE MULTIPLE ENEMIES
// --------------------------------------------------

void UpdateEnemies(
    Enemy enemies[],
    int count,
    const Player &player
)
{
    if (count < 0)
        count = 0;

    if (count > MAX_ENEMIES)
        count = MAX_ENEMIES;

    for (int i = 0; i < count; i++)
    {
        UpdateEnemy(enemies[i], player);
    }
}

// --------------------------------------------------
// DRAW MULTIPLE ENEMIES
// --------------------------------------------------

void DrawEnemies(
    const Enemy enemies[],
    int count
)
{
    if (count < 0)
        count = 0;

    if (count > MAX_ENEMIES)
        count = MAX_ENEMIES;

    for (int i = 0; i < count; i++)
    {
        DrawEnemy(enemies[i]);
    }
}

// --------------------------------------------------
// CHECK COLLISION WITH ONE ENEMY
// --------------------------------------------------

bool IsEnemyCollidingWithPlayer(
    const Enemy &enemy,
    const Player &player
)
{
    return CheckCollisionCircles(
        enemy.position,
        enemy.radius,
        player.position,
        player.radius
    );
}

// --------------------------------------------------
// CHECK COLLISION WITH ANY ENEMY
// --------------------------------------------------

bool IsAnyEnemyCollidingWithPlayer(
    const Enemy enemies[],
    int count,
    const Player &player
)
{
    if (count < 0)
        count = 0;

    if (count > MAX_ENEMIES)
        count = MAX_ENEMIES;

    for (int i = 0; i < count; i++)
    {
        if (IsEnemyCollidingWithPlayer(enemies[i], player))
        {
            return true;
        }
    }

    return false;
}
