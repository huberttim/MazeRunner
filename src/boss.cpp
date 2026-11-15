
#include "boss.h"
#include "maze.h"
#include "player.h"
#include "pixel_art.h"

#include <cmath>


// ============================================================
// PROJECTILE MAZE COLLISION
// ============================================================

bool IsProjectilePositionValid(Vector2 position)
{
    int col = (int)((position.x - MAZE_OFFSET_X) / TILE_SIZE);
    int row = (int)((position.y - MAZE_OFFSET_Y) / TILE_SIZE);

    if (row < 0 || row >= ROWS ||
        col < 0 || col >= COLS)
    {
        return false;
    }

    return maze[row][col] != WALL;
}


// ============================================================
// BOSS INITIALIZATION
// ============================================================

void InitializeBoss(Boss &boss)
{
    boss.gridRow = 5;
    boss.gridCol = 7;

    boss.position = {
        MAZE_OFFSET_X + boss.gridCol * TILE_SIZE + TILE_SIZE / 2.0f,
        MAZE_OFFSET_Y + boss.gridRow * TILE_SIZE + TILE_SIZE / 2.0f
    };

    boss.radius = 25.0f;
    boss.animationTime = 0.0f;

    // Boss attack configuration
    boss.attackTimer = 1.0f;
    boss.attackInterval = 1.35f;
    boss.attackPattern = 0;

    // Boss movement configuration
    boss.movementTargetIndex = 0;
    boss.movementSpeed = 75.0f;

    // Attack warning configuration
    boss.warningTimer = 0.0f;
    boss.warningActive = false;

    boss.active = true;

    // Initialize every projectile slot
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        boss.projectiles[i].position = {0, 0};
        boss.projectiles[i].velocity = {0, 0};
        boss.projectiles[i].radius = 6.0f;
        boss.projectiles[i].lifetime = 0.0f;
        boss.projectiles[i].active = false;
    }
}


// ============================================================
// PROJECTILE CREATION
// ============================================================

void FireProjectileInDirection(Boss &boss, Vector2 direction)
{
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        if (!boss.projectiles[i].active)
        {
            float length = sqrtf(
                direction.x * direction.x +
                direction.y * direction.y
            );

            if (length <= 0.001f)
            {
                return;
            }

            // Normalize direction
            direction.x /= length;
            direction.y /= length;

            const float projectileSpeed = 320.0f;

            BossProjectile &projectile = boss.projectiles[i];

            projectile.position = boss.position;

            projectile.velocity = {
                direction.x * projectileSpeed,
                direction.y * projectileSpeed
            };

            projectile.radius = 6.0f;
            projectile.lifetime = 4.0f;
            projectile.active = true;

            return;
        }
    }
}


// ============================================================
// ATTACK 1: AIMED VOLLEY
// ============================================================

void FireAimedVolley(Boss &boss, const Player &player)
{
    Vector2 direction = {
        player.position.x - boss.position.x,
        player.position.y - boss.position.y
    };

    float baseAngle = atan2f(direction.y, direction.x);

    const float spreadAngles[5] = {
        -30.0f,
        -15.0f,
        0.0f,
        15.0f,
        30.0f
    };

    for (int i = 0; i < 5; i++)
    {
        float angle = baseAngle + spreadAngles[i] * DEG2RAD;

        Vector2 shotDirection = {
            cosf(angle),
            sinf(angle)
        };

        FireProjectileInDirection(boss, shotDirection);
    }
}


// ============================================================
// ATTACK 2: RADIAL BURST
// ============================================================

void FireRadialBurst(Boss &boss)
{
    for (int i = 0; i < 12; i++)
    {
        float angle = i * 30.0f * DEG2RAD;

        Vector2 direction = {
            cosf(angle),
            sinf(angle)
        };

        FireProjectileInDirection(boss, direction);
    }
}


// ============================================================
// BOSS PATROL MOVEMENT
// ============================================================

// Five safe positions in the central Level 10 arena
static const int BOSS_PATROL_POINTS[5][2] =
{
    {3, 7},
    {5, 9},
    {6, 7},
    {4, 6},
    {5, 7}
};


void MoveBoss(Boss &boss, float deltaTime)
{
    int targetRow =
        BOSS_PATROL_POINTS[boss.movementTargetIndex][0];

    int targetCol =
        BOSS_PATROL_POINTS[boss.movementTargetIndex][1];

    // Convert target grid position into world coordinates
    Vector2 targetPosition =
    {
        MAZE_OFFSET_X +
            targetCol * TILE_SIZE +
            TILE_SIZE / 2.0f,

        MAZE_OFFSET_Y +
            targetRow * TILE_SIZE +
            TILE_SIZE / 2.0f
    };

    // Calculate distance to target
    Vector2 direction =
    {
        targetPosition.x - boss.position.x,
        targetPosition.y - boss.position.y
    };

    float distance = sqrtf(
        direction.x * direction.x +
        direction.y * direction.y
    );

    float movementStep =
        boss.movementSpeed * deltaTime;

    // Reached the current waypoint
    if (distance <= movementStep || distance <= 0.001f)
    {
        boss.position = targetPosition;

        boss.gridRow = targetRow;
        boss.gridCol = targetCol;

        // Select the next waypoint
        boss.movementTargetIndex++;

        if (boss.movementTargetIndex >= 5)
        {
            boss.movementTargetIndex = 0;
        }

        return;
    }

    // Move smoothly towards the waypoint
    direction.x /= distance;
    direction.y /= distance;

    boss.position.x += direction.x * movementStep;
    boss.position.y += direction.y * movementStep;
}

// ============================================================
// BOSS UPDATE
// ============================================================

void UpdateBoss(Boss &boss, const Player &player)
{
    if (!boss.active)
    {
        return;
    }

    float deltaTime = GetFrameTime();

    boss.animationTime += deltaTime;

    
// UPDATE ATTACK COUNTDOWN AND WARNING

if (!boss.warningActive)
{
    boss.attackTimer -= deltaTime;

    // Begin warning when attack countdown expires
    if (boss.attackTimer <= 0.0f)
    {
        boss.warningActive = true;
        boss.warningTimer = 0.65f;
    }
}
else
{
    // Allow the player time to react
    boss.warningTimer -= deltaTime;

    if (boss.warningTimer <= 0.0f)
    {
        // Execute the announced attack
        if (boss.attackPattern == 0)
        {
            FireAimedVolley(boss, player);

            boss.attackPattern = 1;
        }
        else
        {
            FireRadialBurst(boss);

            boss.attackPattern = 0;
        }

        // Reset for the next attack
        boss.warningActive = false;
        boss.warningTimer = 0.0f;
        boss.attackTimer = boss.attackInterval;
    }
}

    // Move between patrol positions.
    // Stop temporarily while announcing an attack.
    if (!boss.warningActive)
    {
        MoveBoss(boss, deltaTime);
    }

    // Update all active projectiles
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        BossProjectile &projectile = boss.projectiles[i];

        if (!projectile.active)
        {
            continue;
        }

        projectile.position.x +=
            projectile.velocity.x * deltaTime;

        projectile.position.y +=
            projectile.velocity.y * deltaTime;

        projectile.lifetime -= deltaTime;

        // Remove projectiles that expire or hit walls
        if (
            projectile.lifetime <= 0.0f ||
            !IsProjectilePositionValid(projectile.position)
        )
        {
            projectile.active = false;
        }
    }
}


// ============================================================
// NEON WARDEN PIXEL SPRITES
// ============================================================

static const char* BOSS_SPRITE_FRAME_1[16] =
{
    ".......MM.......",
    "......MPM.......",
    ".....MPPPM......",
    "....MPPPPPM.....",
    "...MPPPCCPPM....",
    "..MPPPCCCCPPM...",
    ".MPPPCCWWCCPPM..",
    "MPPPCCWWWWCCPPM.",
    "MPPPCCWWWWCCPPM.",
    ".MPPPCCWWCCPPM..",
    "..MPPPCCCCPPM...",
    "...MPPPCCPPM....",
    "....MPPPPPM.....",
    ".....MPPPM......",
    "......MPM.......",
    ".......MM......."
};

static const char* BOSS_SPRITE_FRAME_2[16] =
{
    ".......MM.......",
    "......MPM.......",
    ".....MPPPM......",
    "....MPPPPPM.....",
    "...MPPPCCPPM....",
    "..MPPPCCWCCPPM..",
    ".MPPPCCWWCCPPM..",
    "MPPPCCWWWWCCPPM.",
    "MPPPCCWWWWCCPPM.",
    ".MPPPCCWWCCPPM..",
    "..MPPPCCWCCPPM..",
    "...MPPPCCPPM....",
    "....MPPPPPM.....",
    ".....MPPPM......",
    "......MPM.......",
    ".......MM......."
};

static const PixelColor BOSS_PIXEL_PALETTE[] =
{
    {'M', PIXEL_MAGENTA},
    {'P', PIXEL_PURPLE},
    {'C', PIXEL_CYAN},
    {'W', PIXEL_WHITE}
};


// ============================================================
// PIXEL ART PROJECTILE SPRITE
// ============================================================

static const char* BOSS_PROJECTILE_SPRITE[8] =
{
    "...CC...",
    "..CWC...",
    ".CWWCC..",
    "CWWWWC..",
    ".CWWCC..",
    "..CCC...",
    "...C....",
    "........"
};

static const PixelColor PROJECTILE_PIXEL_PALETTE[] =
{
    {'C', PIXEL_CYAN},
    {'W', PIXEL_WHITE}
};


// ============================================================
// PIXEL ART BOSS VISUALS
// ============================================================



void DrawBoss(const Boss &boss)
{
    if (!boss.active)
    {
        return;
    }

    float time = boss.animationTime;

    // Two-frame idle animation
    int frame = ((int)(time * 3.0f) % 2);

    const char* const* sprite =
        (frame == 0)
        ? BOSS_SPRITE_FRAME_1
        : BOSS_SPRITE_FRAME_2;

    const int spriteSize = 16;
    const int pixelSize = 4;
    const int drawSize = spriteSize * pixelSize;

    int drawX =
        (int)(boss.position.x - drawSize / 2.0f);

    int drawY =
        (int)(
            boss.position.y -
            drawSize / 2.0f +
            sinf(time * 2.0f) * 3.0f
        );

    bool warningFlash =
        boss.warningActive &&
        ((int)(time * 12.0f) % 2 == 0);

    // --------------------------------------------------------
    // ATTACK WARNING: AIMED VOLLEY
    // --------------------------------------------------------

    if (
        boss.warningActive &&
        boss.attackPattern == 0
    )
    {
        if (warningFlash)
        {
            // Large magenta square warning frame
            DrawRectangleLinesEx(
                {
                    boss.position.x - 48,
                    boss.position.y - 48,
                    96,
                    96
                },
                4,
                PIXEL_MAGENTA
            );

            // Four warning corner blocks
            DrawRectangle(
                (int)boss.position.x - 52,
                (int)boss.position.y - 52,
                8,
                8,
                PIXEL_WHITE
            );

            DrawRectangle(
                (int)boss.position.x + 44,
                (int)boss.position.y - 52,
                8,
                8,
                PIXEL_WHITE
            );

            DrawRectangle(
                (int)boss.position.x - 52,
                (int)boss.position.y + 44,
                8,
                8,
                PIXEL_WHITE
            );

            DrawRectangle(
                (int)boss.position.x + 44,
                (int)boss.position.y + 44,
                8,
                8,
                PIXEL_WHITE
            );
        }
    }

    // --------------------------------------------------------
    // ATTACK WARNING: RADIAL BURST
    // --------------------------------------------------------

    if (
        boss.warningActive &&
        boss.attackPattern == 1
    )
    {
        if (warningFlash)
        {
            const Vector2 warningOffsets[4] =
            {
                {-48, 0},
                {48, 0},
                {0, -48},
                {0, 48}
            };

            for (int i = 0; i < 4; i++)
            {
                int markerX =
                    (int)(boss.position.x + warningOffsets[i].x);

                int markerY =
                    (int)(boss.position.y + warningOffsets[i].y);

                DrawRectangle(
                    markerX - 5,
                    markerY - 5,
                    10,
                    10,
                    PIXEL_WHITE
                );

                DrawRectangle(
                    markerX - 2,
                    markerY - 2,
                    4,
                    4,
                    PIXEL_CYAN
                );
            }
        }
    }

    // --------------------------------------------------------
    // FOUR ANIMATED ENERGY NODES
    // --------------------------------------------------------

    const Vector2 nodeOffsets[4] =
    {
        {-32, -32},
        {32, -32},
        {-32, 32},
        {32, 32}
    };

    for (int i = 0; i < 4; i++)
    {
        float pulse =
            (sinf(time * 4.0f + i * 1.5f) + 1.0f) / 2.0f;

        int nodeX =
            (int)(boss.position.x + nodeOffsets[i].x);

        int nodeY =
            (int)(boss.position.y + nodeOffsets[i].y);

        int nodeSize = 4 + (int)(pulse * 2.0f);

        DrawRectangle(
            nodeX - nodeSize / 2,
            nodeY - nodeSize / 2,
            nodeSize,
            nodeSize,
            PIXEL_MAGENTA
        );

        DrawRectangle(
            nodeX - 2,
            nodeY - 2,
            4,
            4,
            PIXEL_CYAN
        );
    }

    // --------------------------------------------------------
    // PIXEL ENERGY CORE
    // --------------------------------------------------------

    float corePulse =
        (sinf(time * 4.0f) + 1.0f) / 2.0f;

    int coreSize = 4 + (int)(corePulse * 4.0f);

    DrawRectangle(
        (int)boss.position.x - coreSize / 2 - 4,
        (int)boss.position.y - coreSize / 2 - 4,
        coreSize + 8,
        coreSize + 8,
        boss.warningActive && boss.attackPattern == 0
            ? PIXEL_MAGENTA
            : PIXEL_DARK_BLUE
    );

    DrawRectangle(
        (int)boss.position.x - coreSize / 2,
        (int)boss.position.y - coreSize / 2,
        coreSize,
        coreSize,
        PIXEL_WHITE
    );

    // --------------------------------------------------------
    // MAIN CRYSTAL SPRITE
    // --------------------------------------------------------

    DrawPixelSprite(
        sprite,
        spriteSize,
        spriteSize,
        drawX,
        drawY,
        pixelSize,
        BOSS_PIXEL_PALETTE,
        sizeof(BOSS_PIXEL_PALETTE) /
        sizeof(BOSS_PIXEL_PALETTE[0])
    );
}

// ============================================================
// PIXEL ART PROJECTILE VISUALS
// ============================================================

void DrawBossProjectiles(const Boss &boss)
{
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        const BossProjectile &projectile =
            boss.projectiles[i];

        if (!projectile.active)
        {
            continue;
        }

        const int spriteSize = 8;
        const int pixelSize = 2;
        const int drawSize = spriteSize * pixelSize;

        int drawX =
            (int)(projectile.position.x - drawSize / 2.0f);

        int drawY =
            (int)(projectile.position.y - drawSize / 2.0f);

        // ----------------------------------------------------
        // SQUARE ENERGY TRAIL
        // ----------------------------------------------------

        float velocityLength = sqrtf(
            projectile.velocity.x * projectile.velocity.x +
            projectile.velocity.y * projectile.velocity.y
        );

        if (velocityLength > 0.001f)
        {
            float directionX =
                projectile.velocity.x / velocityLength;

            float directionY =
                projectile.velocity.y / velocityLength;

            for (int trail = 1; trail <= 3; trail++)
            {
                int trailSize = 6 - trail;

                int trailX = (int)(
                    projectile.position.x -
                    directionX * trail * 7.0f
                );

                int trailY = (int)(
                    projectile.position.y -
                    directionY * trail * 7.0f
                );

                DrawRectangle(
                    trailX - trailSize / 2,
                    trailY - trailSize / 2,
                    trailSize,
                    trailSize,
                    trail == 1
                        ? PIXEL_PURPLE
                        : PIXEL_DARK_BLUE
                );
            }
        }

        // ----------------------------------------------------
        // MAIN PROJECTILE SPRITE
        // ----------------------------------------------------

        DrawPixelSprite(
            BOSS_PROJECTILE_SPRITE,
            spriteSize,
            spriteSize,
            drawX,
            drawY,
            pixelSize,
            PROJECTILE_PIXEL_PALETTE,
            sizeof(PROJECTILE_PIXEL_PALETTE) /
            sizeof(PROJECTILE_PIXEL_PALETTE[0])
        );
    }
}


// ============================================================
// PROJECTILE-PLAYER COLLISION
// ============================================================

bool IsBossProjectileCollidingWithPlayer(
    Boss &boss,
    const Player &player
)
{
    if (!boss.active)
    {
        return false;
    }

    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        BossProjectile &projectile = boss.projectiles[i];

        if (!projectile.active)
        {
            continue;
        }

        bool collision = CheckCollisionCircles(
            projectile.position,
            projectile.radius,
            player.position,
            player.radius
        );

        if (collision)
        {
            // Remove the projectile after impact
            projectile.active = false;

            return true;
        }
    }

    return false;
}
