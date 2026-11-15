
#include "player.h"
#include "maze.h"
#include "pixel_art.h"

#include <cmath>

// --------------------------------------------------
// CONVERT MAZE COORDINATES INTO SCREEN COORDINATES
// --------------------------------------------------

static Vector2 GetTileCenter(int row, int col)
{
    return {
        MAZE_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2.0f,
        MAZE_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2.0f
    };
}

// --------------------------------------------------
// INITIALIZE PLAYER
// --------------------------------------------------

void InitializePlayer(Player &player)
{
    player.gridRow = 1;
    player.gridCol = 1;

    player.position = GetTileCenter(
        player.gridRow,
        player.gridCol
    );

    player.targetPosition = player.position;

    player.facingDirection = {1.0f, 0.0f};
    player.visualFacingDirection = {1.0f, 0.0f};

    player.speed = 240.0f;
    player.radius = 15.0f;

    player.animationTime = 0.0f;
    player.movementBlend = 0.0f;
    player.turnPulse = 0.0f;

    player.isMoving = false;
}

// --------------------------------------------------
// CHECK WHETHER A DESTINATION IS ACCESSIBLE
// --------------------------------------------------

static bool CanMoveTo(int row, int col)
{
    if (row < 0 || row >= ROWS ||
        col < 0 || col >= COLS)
    {
        return false;
    }

    if (maze[row][col] == WALL)
    {
        return false;
    }

    return true;
}

// --------------------------------------------------
// HANDLE PLAYER INPUT
// --------------------------------------------------

static void HandlePlayerInput(Player &player)
{
    if (player.isMoving)
        return;

    int nextRow = player.gridRow;
    int nextCol = player.gridCol;

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        nextCol++;

    else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        nextCol--;

    else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        nextRow--;

    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        nextRow++;

    if (nextRow != player.gridRow ||
        nextCol != player.gridCol)
    {
        if (CanMoveTo(nextRow, nextCol))
        {
            Vector2 newDirection = {
                (float)(nextCol - player.gridCol),
                (float)(nextRow - player.gridRow)
            };

            // Detect a change in direction.
            if (
                newDirection.x != player.facingDirection.x ||
                newDirection.y != player.facingDirection.y
            )
            {
                player.turnPulse = 1.0f;
            }

            player.facingDirection = newDirection;

            player.gridRow = nextRow;
            player.gridCol = nextCol;

            player.targetPosition = GetTileCenter(
                nextRow,
                nextCol
            );

            player.isMoving = true;
        }
    }
}

// --------------------------------------------------
// UPDATE PLAYER ANIMATION
// --------------------------------------------------

static void UpdatePlayerAnimation(Player &player, float dt)
{
    player.animationTime += dt;

    // Smoothly transition between idle and movement.
    float targetBlend = player.isMoving ? 1.0f : 0.0f;

    float blendSpeed = 9.0f;

    player.movementBlend +=
        (targetBlend - player.movementBlend)
        * (1.0f - expf(-blendSpeed * dt));

    // Smoothly rotate the visual character.
    float directionSpeed = 14.0f;

    player.visualFacingDirection.x +=
        (player.facingDirection.x -
         player.visualFacingDirection.x)
        * (1.0f - expf(-directionSpeed * dt));

    player.visualFacingDirection.y +=
        (player.facingDirection.y -
         player.visualFacingDirection.y)
        * (1.0f - expf(-directionSpeed * dt));

    // Normalize the visual direction.
    float length = sqrtf(
        player.visualFacingDirection.x *
        player.visualFacingDirection.x +

        player.visualFacingDirection.y *
        player.visualFacingDirection.y
    );

    if (length > 0.001f)
    {
        player.visualFacingDirection.x /= length;
        player.visualFacingDirection.y /= length;
    }

    // Fade the direction-change energy pulse.
    player.turnPulse -= dt * 2.8f;

    if (player.turnPulse < 0.0f)
        player.turnPulse = 0.0f;
}

// --------------------------------------------------
// UPDATE PLAYER MOVEMENT
// --------------------------------------------------

void UpdatePlayer(Player &player)
{
    float deltaTime = GetFrameTime();

    HandlePlayerInput(player);

    if (player.isMoving)
    {
        float dx = player.targetPosition.x -
                   player.position.x;

        float dy = player.targetPosition.y -
                   player.position.y;

        float distance = sqrtf(dx * dx + dy * dy);

        float movement = player.speed * deltaTime;

        if (distance <= movement || distance == 0.0f)
        {
            player.position = player.targetPosition;
            player.isMoving = false;
        }
        else
        {
            player.position.x += (dx / distance) * movement;
            player.position.y += (dy / distance) * movement;
        }
    }

    UpdatePlayerAnimation(player, deltaTime);
}


void DrawPlayer(const Player &player)
{
    // --------------------------------------------------
    // PLAYER SPRITE FRAMES (10 x 10)
    // --------------------------------------------------

    static const char* PLAYER_FRAME_A[10] =
    {
        "...OOOO...",
        "..OBBBBO..",
        ".OBVVVVBO.",
        ".OBCCCCBO.",
        ".OBBBBBBO.",
        "..OBYYBO..",
        "..OBBBBO..",
        ".OBB..BBO.",
        ".OO....OO.",
        "OO......OO"
    };

    static const char* PLAYER_FRAME_B[10] =
    {
        "...OOOO...",
        "..OBBBBO..",
        ".OBVVVVBO.",
        ".OBCCCCBO.",
        ".OBBBBBBO.",
        "..OBYYBO..",
        "..OBBBBO..",
        ".OB....BO.",
        ".OO....OO.",
        "OO......OO"
    };

    // --------------------------------------------------
    // HIGH-CONTRAST PLAYER PALETTE
    // --------------------------------------------------

    static const PixelColor PLAYER_PALETTE[] =
    {
        {'O', PIXEL_DARK_GRAY},
        {'B', PIXEL_ORANGE},
        {'Y', PIXEL_YELLOW},
        {'V', PIXEL_WHITE},
        {'C', PIXEL_CYAN}
    };

    const int paletteCount =
        sizeof(PLAYER_PALETTE) / sizeof(PLAYER_PALETTE[0]);

    // --------------------------------------------------
    // SPRITE POSITION AND ANIMATION
    // --------------------------------------------------

    const int pixelSize = 4;
    const int spriteSize = 10 * pixelSize;

    int spriteX =
        (int)roundf(player.position.x) - spriteSize / 2;

    int spriteY =
        (int)roundf(player.position.y) - spriteSize / 2;

    int animationFrame =
        (int)(player.animationTime * 8.0f) % 2;

    const char* const* currentFrame =
        PLAYER_FRAME_A;

    if (player.movementBlend > 0.1f &&
        animationFrame == 1)
    {
        currentFrame = PLAYER_FRAME_B;
    }

    // --------------------------------------------------
    // PIXEL MOVEMENT TRAIL
    // --------------------------------------------------

    if (player.movementBlend > 0.05f)
    {
        Vector2 direction =
            player.visualFacingDirection;

        for (int i = 1; i <= 3; i++)
        {
            float distance = i * 7.0f;

            int trailX =
                (int)roundf(
                    player.position.x -
                    direction.x * distance
                );

            int trailY =
                (int)roundf(
                    player.position.y -
                    direction.y * distance
                );

            Color trailColor =
                (i == 1) ? PIXEL_CYAN : PIXEL_BLUE;

            unsigned char alpha =
                (unsigned char)(
                    (4 - i) * 45.0f *
                    player.movementBlend
                );

            trailColor.a = alpha;

            DrawRectangle(
                trailX - 2,
                trailY - 2,
                4,
                4,
                trailColor
            );
        }
    }

    // --------------------------------------------------
    // DRAW PLAYER SPRITE
    // --------------------------------------------------

    DrawPixelSprite(
        currentFrame,
        10,
        10,
        spriteX,
        spriteY,
        pixelSize,
        PLAYER_PALETTE,
        paletteCount
    );

    // --------------------------------------------------
    // DIRECTION INDICATOR
    // --------------------------------------------------

    Vector2 direction =
        player.visualFacingDirection;

    int indicatorX =
        (int)roundf(
            player.position.x + direction.x * 17.0f
        );

    int indicatorY =
        (int)roundf(
            player.position.y + direction.y * 17.0f
        );

    DrawRectangle(
        indicatorX - 1,
        indicatorY - 1,
        3,
        3,
        PIXEL_WHITE
    );

    // --------------------------------------------------
    // PIXEL TURN SPARK
    // --------------------------------------------------

    if (player.turnPulse > 0.0f)
    {
        int sparkOffset =
            (int)(player.turnPulse * 5.0f);

        int sparkX =
            (int)roundf(player.position.x) -
            (int)roundf(direction.y * 22.0f);

        int sparkY =
            (int)roundf(player.position.y) +
            (int)roundf(direction.x * 22.0f);

        DrawRectangle(
            sparkX - sparkOffset / 2,
            sparkY - sparkOffset / 2,
            2 + sparkOffset,
            2 + sparkOffset,
            PIXEL_YELLOW
        );
    }
}
