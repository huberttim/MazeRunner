
#include "effects.h"
#include "maze.h"
#include "pixel_art.h"

#include <cmath>

// --------------------------------------------------
// PIXEL-ART PARTICLE COLOURS
// --------------------------------------------------

const Color PARTICLE_PURPLE = PIXEL_PURPLE;
const Color PARTICLE_CYAN   = PIXEL_CYAN;
const Color PARTICLE_WHITE  = PIXEL_WHITE;
const Color PARTICLE_GREEN  = PIXEL_GREEN;

// --------------------------------------------------
// APPLY ALPHA TO A COLOUR
// --------------------------------------------------

static Color ApplyAlpha(Color color, float alpha)
{
    if (alpha < 0.0f)
        alpha = 0.0f;

    if (alpha > 1.0f)
        alpha = 1.0f;

    color.a = static_cast<unsigned char>(
        color.a * alpha
    );

    return color;
}

// --------------------------------------------------
// INITIALIZE EFFECTS
// --------------------------------------------------

void InitializeEffects(EffectsState &effects)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        effects.particles[i].active = false;
    }

    effects.flashPosition = {0.0f, 0.0f};
    effects.flashLifetime = 0.0f;
    effects.flashMaxLifetime = 0.16f;
    effects.flashActive = false;

    effects.exitUnlockTimer = 0.0f;
    effects.levelCompleteTimer = 0.0f;
}

// --------------------------------------------------
// INTERNAL PARTICLE BURST
// --------------------------------------------------

static void SpawnParticleBurst(
    EffectsState &effects,
    Vector2 position,
    int particleCount,
    float baseSpeed,
    float speedVariation,
    float lifetime
)
{
    int created = 0;

    for (
        int i = 0;
        i < MAX_PARTICLES && created < particleCount;
        i++
    )
    {
        Particle &p = effects.particles[i];

        if (!p.active)
        {
            float angle =
                (created / (float)particleCount)
                * 2.0f * PI;

            float speed =
                baseSpeed + (created % 5) * speedVariation;

            p.position = position;
            p.previousPosition = position;

            p.velocity = {
                cosf(angle) * speed,
                sinf(angle) * speed
            };

            p.lifetime = lifetime;
            p.maxLifetime = lifetime;

            p.radius = 2.0f + (created % 3) * 1.2f;

            if (created % 3 == 0)
            {
                p.color = PARTICLE_WHITE;
            }
            else if (created % 3 == 1)
            {
                p.color = PARTICLE_CYAN;
            }
            else
            {
                p.color = PARTICLE_PURPLE;
            }

            p.active = true;

            created++;
        }
    }
}

// --------------------------------------------------
// NORMAL CRYSTAL COLLECTION
// --------------------------------------------------

void SpawnCrystalParticles(
    EffectsState &effects,
    Vector2 position
)
{
    SpawnParticleBurst(
        effects,
        position,
        20,
        85.0f,
        22.0f,
        0.65f
    );

    effects.flashPosition = position;
    effects.flashLifetime = effects.flashMaxLifetime;
    effects.flashActive = true;
}

// --------------------------------------------------
// FINAL CRYSTAL COLLECTION
// --------------------------------------------------

void SpawnFinalCrystalBurst(
    EffectsState &effects,
    Vector2 position
)
{
    // A larger and faster burst for the fifth crystal.
    SpawnParticleBurst(
        effects,
        position,
        36,
        145.0f,
        30.0f,
        0.9f
    );

    // Stronger central flash.
    effects.flashPosition = position;
    effects.flashMaxLifetime = 0.35f;
    effects.flashLifetime = effects.flashMaxLifetime;
    effects.flashActive = true;
}

// --------------------------------------------------
// EXIT UNLOCK EVENT
// --------------------------------------------------

void TriggerExitUnlock(EffectsState &effects)
{
    effects.exitUnlockTimer = 1.8f;
}

// --------------------------------------------------
// LEVEL COMPLETION EVENT
// --------------------------------------------------

void TriggerLevelComplete(EffectsState &effects)
{
    effects.levelCompleteTimer = 1.0f;
}

// --------------------------------------------------
// UPDATE PARTICLE AND EVENT EFFECTS
// --------------------------------------------------

void UpdateEffects(EffectsState &effects)
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle &p = effects.particles[i];

        if (!p.active)
            continue;

        p.previousPosition = p.position;

        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;

        float drag = expf(-2.5f * dt);

        p.velocity.x *= drag;
        p.velocity.y *= drag;

        p.lifetime -= dt;

        if (p.lifetime <= 0.0f)
        {
            p.active = false;
        }
    }

    // Update crystal flash.
    if (effects.flashActive)
    {
        effects.flashLifetime -= dt;

        if (effects.flashLifetime <= 0.0f)
        {
            effects.flashLifetime = 0.0f;
            effects.flashActive = false;

            // Restore the normal flash duration.
            effects.flashMaxLifetime = 0.16f;
        }
    }

    // Update exit-unlock pulse.
    if (effects.exitUnlockTimer > 0.0f)
    {
        effects.exitUnlockTimer -= dt;

        if (effects.exitUnlockTimer < 0.0f)
        {
            effects.exitUnlockTimer = 0.0f;
        }
    }

    // Update level-completion message.
    if (effects.levelCompleteTimer > 0.0f)
    {
        effects.levelCompleteTimer -= dt;

        if (effects.levelCompleteTimer < 0.0f)
        {
            effects.levelCompleteTimer = 0.0f;
        }
    }
}

// --------------------------------------------------
// DRAW PARTICLE EFFECTS
// --------------------------------------------------

void DrawEffects(const EffectsState &effects)
{
    // ----------------------------------------------
    // PIXEL PARTICLE TRAILS AND PARTICLES
    // ----------------------------------------------

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        const Particle &p = effects.particles[i];

        if (!p.active)
            continue;

        float alpha = p.lifetime / p.maxLifetime;

        if (alpha < 0.0f)
            alpha = 0.0f;

        if (alpha > 1.0f)
            alpha = 1.0f;

        Color particleColor = ApplyAlpha(p.color, alpha);

        Color trailColor = ApplyAlpha(
            p.color,
            alpha * 0.55f
        );

        int particleX = static_cast<int>(
            roundf(p.position.x)
        );

        int particleY = static_cast<int>(
            roundf(p.position.y)
        );

        int previousX = static_cast<int>(
            roundf(p.previousPosition.x)
        );

        int previousY = static_cast<int>(
            roundf(p.previousPosition.y)
        );

        int particleSize = static_cast<int>(
            roundf(p.radius * alpha * 2.0f)
        );

        if (particleSize < 2)
            particleSize = 2;

        // Draw a square trail behind the particle.
        if (
            previousX != particleX ||
            previousY != particleY
        )
        {
            int middleX = (previousX + particleX) / 2;
            int middleY = (previousY + particleY) / 2;

            DrawRectangle(
                middleX - 2,
                middleY - 2,
                4,
                4,
                trailColor
            );
        }

        // Main square particle.
        DrawRectangle(
            particleX - particleSize / 2,
            particleY - particleSize / 2,
            particleSize,
            particleSize,
            particleColor
        );

        // Small bright center for larger particles.
        if (particleSize >= 5)
        {
            DrawRectangle(
                particleX - 1,
                particleY - 1,
                2,
                2,
                ApplyAlpha(PIXEL_WHITE, alpha * 0.8f)
            );
        }
    }

    // ----------------------------------------------
    // PIXEL CRYSTAL COLLECTION FLASH
    // ----------------------------------------------

    if (effects.flashActive)
    {
        float alpha =
            effects.flashLifetime / effects.flashMaxLifetime;

        if (alpha < 0.0f)
            alpha = 0.0f;

        if (alpha > 1.0f)
            alpha = 1.0f;

        float expansion = 1.0f - alpha;

        int flashX = static_cast<int>(
            roundf(effects.flashPosition.x)
        );

        int flashY = static_cast<int>(
            roundf(effects.flashPosition.y)
        );

        int flashSize = static_cast<int>(
            12.0f + expansion * 30.0f
        );

        // Expanding square outline.
        DrawRectangleLines(
            flashX - flashSize / 2,
            flashY - flashSize / 2,
            flashSize,
            flashSize,
            ApplyAlpha(PIXEL_CYAN, alpha)
        );

        // Second expanding outline.
        DrawRectangleLines(
            flashX - flashSize / 2 - 5,
            flashY - flashSize / 2 - 5,
            flashSize + 10,
            flashSize + 10,
            ApplyAlpha(PIXEL_WHITE, alpha * 0.65f)
        );

        // Bright central pixel.
        DrawRectangle(
            flashX - 3,
            flashY - 3,
            6,
            6,
            ApplyAlpha(PIXEL_WHITE, alpha)
        );
    }

    // ----------------------------------------------
    // PIXEL EXIT UNLOCK ANIMATION
    // ----------------------------------------------

    if (effects.exitUnlockTimer > 0.0f)
    {
        float progress =
            effects.exitUnlockTimer / 1.8f;

        float elapsed =
            1.8f - effects.exitUnlockTimer;

        float pulse =
            (sinf(elapsed * 13.0f) + 1.0f) / 2.0f;

        // Find the exit tile in the current maze.
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (maze[row][col] != EXIT)
                    continue;

                int exitX =
                    MAZE_OFFSET_X + col * TILE_SIZE
                    + TILE_SIZE / 2;

                int exitY =
                    MAZE_OFFSET_Y + row * TILE_SIZE
                    + TILE_SIZE / 2;

                int radius = static_cast<int>(
                    25.0f + pulse * 15.0f
                );

                // Expanding green square.
                DrawRectangleLines(
                    exitX - radius,
                    exitY - radius,
                    radius * 2,
                    radius * 2,
                    ApplyAlpha(PIXEL_GREEN, progress)
                );

                // Secondary cyan square.
                DrawRectangleLines(
                    exitX - radius - 6,
                    exitY - radius - 6,
                    radius * 2 + 12,
                    radius * 2 + 12,
                    ApplyAlpha(PIXEL_CYAN, progress * 0.75f)
                );

                // Four small corner pixels.
                int cornerOffset = radius + 9;

                Color cornerColor = ApplyAlpha(
                    PIXEL_WHITE,
                    progress * (0.5f + pulse * 0.5f)
                );

                DrawRectangle(
                    exitX - cornerOffset,
                    exitY - cornerOffset,
                    5,
                    5,
                    cornerColor
                );

                DrawRectangle(
                    exitX + cornerOffset - 5,
                    exitY - cornerOffset,
                    5,
                    5,
                    cornerColor
                );

                DrawRectangle(
                    exitX - cornerOffset,
                    exitY + cornerOffset - 5,
                    5,
                    5,
                    cornerColor
                );

                DrawRectangle(
                    exitX + cornerOffset - 5,
                    exitY + cornerOffset - 5,
                    5,
                    5,
                    cornerColor
                );
            }
        }
    }

    
    // ----------------------------------------------
    // PIXEL-ART LEVEL COMPLETION BANNER
    // ----------------------------------------------

    if (effects.levelCompleteTimer > 0.0f)
    {
        float progress =
            effects.levelCompleteTimer;

        float alpha = progress;

        float pulse =
            (sinf((1.0f - progress) * 12.0f) + 1.0f)
            / 2.0f;

        // Main dark banner.
        DrawRectangle(
            0,
            275,
            1000,
            105,
            ApplyAlpha(PIXEL_NAVY, alpha * 0.96f)
        );

        // Pixel-style top and bottom borders.
        DrawRectangle(
            0,
            275,
            1000,
            4,
            ApplyAlpha(PIXEL_CYAN, alpha)
        );

        DrawRectangle(
            0,
            376,
            1000,
            4,
            ApplyAlpha(PIXEL_PURPLE, alpha)
        );

        // Decorative side blocks.
        for (int i = 0; i < 5; i++)
        {
            int blockY = 295 + i * 14;

            DrawRectangle(
                24,
                blockY,
                8,
                8,
                ApplyAlpha(PIXEL_CYAN, alpha)
            );

            DrawRectangle(
                968,
                blockY,
                8,
                8,
                ApplyAlpha(PIXEL_PURPLE, alpha)
            );
        }

        // Main banner message.
        DrawPixelTextCentered(
            "LEVEL COMPLETE",
            500,
            305,
            5,
            ApplyAlpha(PIXEL_WHITE, alpha)
        );

        // Supporting message.
        DrawPixelTextCentered(
            "PREPARE FOR THE NEXT CHALLENGE",
            500,
            350,
            2,
            ApplyAlpha(
                PIXEL_CYAN,
                alpha * (0.65f + pulse * 0.35f)
            )
        );
    }
}