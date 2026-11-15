
#include "transition.h"
#include "pixel_art.h"

// --------------------------------------------------
// INITIALIZE TRANSITION
// --------------------------------------------------

void InitializeTransition(ScreenTransition &transition)
{
    transition.alpha = 0.0f;

    // Two seconds for each fade.
    transition.speed = 0.75f;

    // No initial hold.
    transition.holdTimer = 0.0f;

    transition.phase = TRANSITION_IDLE;
}

// --------------------------------------------------
// BEGIN A NEW TRANSITION
// --------------------------------------------------

void BeginTransition(ScreenTransition &transition)
{
    transition.alpha = 0.0f;
    transition.holdTimer = 0.0f;

    transition.phase = TRANSITION_FADING_OUT;
}

// --------------------------------------------------
// UPDATE TRANSITION
// --------------------------------------------------

bool UpdateTransition(ScreenTransition &transition)
{
    if (transition.phase == TRANSITION_IDLE)
    {
        return false;
    }

    float deltaTime = GetFrameTime();

    // --------------------------------
    // FADE OUT
    // --------------------------------

    if (transition.phase == TRANSITION_FADING_OUT)
    {
        transition.alpha += transition.speed * deltaTime;

        if (transition.alpha >= 1.0f)
        {
            transition.alpha = 1.0f;

            transition.phase = TRANSITION_HOLD;

            // Tell main.cpp to switch screens.
            return true;
        }
    }

    // --------------------------------
    // HOLD
    // --------------------------------

    else if (transition.phase == TRANSITION_HOLD)
    {
        transition.holdTimer -= deltaTime;

        if (transition.holdTimer <= 0.0f)
        {
            transition.holdTimer = 0.0f;

            transition.phase = TRANSITION_FADING_IN;
        }
    }

    // --------------------------------
    // FADE IN
    // --------------------------------

    else if (transition.phase == TRANSITION_FADING_IN)
    {
        transition.alpha -= transition.speed * deltaTime;

        if (transition.alpha <= 0.0f)
        {
            transition.alpha = 0.0f;

            transition.phase = TRANSITION_IDLE;
        }
    }

    return false;
}

// --------------------------------------------------
// BEGIN FADE IN
// --------------------------------------------------

void BeginFadeIn(ScreenTransition &transition)
{
    // Keep the screen completely covered first.
    // The actual fade-in begins after the hold timer.
    if (transition.phase == TRANSITION_HOLD)
    {
        transition.holdTimer = 0.75f;
    }
}

// --------------------------------------------------
// DRAW PIXEL-ART TRANSITION OVERLAY
// --------------------------------------------------

void DrawTransitionOverlay(const ScreenTransition &transition)
{
    if (transition.alpha <= 0.0f)
    {
        return;
    }

    const int BLOCK_SIZE = 20;

    const int GRID_WIDTH = 1000 / BLOCK_SIZE;
    const int GRID_HEIGHT = 700 / BLOCK_SIZE;

    // 4 x 4 Bayer ordered-dither matrix.
    const int DITHER_MATRIX[4][4] =
    {
        { 0,  8,  2, 10},
        {12,  4, 14,  6},
        { 3, 11,  1,  9},
        {15,  7, 13,  5}
    };

    float progress = transition.alpha;

    if (progress > 1.0f)
    {
        progress = 1.0f;
    }

    // Progressively cover the screen with pixel blocks.
    for (int row = 0; row < GRID_HEIGHT; row++)
    {
        for (int col = 0; col < GRID_WIDTH; col++)
        {
            int matrixValue =
                DITHER_MATRIX[row % 4][col % 4];

            float threshold =
                ((float)matrixValue + 0.5f) / 16.0f;

            if (progress >= threshold)
            {
                DrawRectangle(
                    col * BLOCK_SIZE,
                    row * BLOCK_SIZE,
                    BLOCK_SIZE,
                    BLOCK_SIZE,
                    PIXEL_BLACK
                );
            }
        }
    }
}

// --------------------------------------------------
// CHECK WHETHER TRANSITION IS ACTIVE
// --------------------------------------------------

bool IsTransitionActive(const ScreenTransition &transition)
{
    return transition.phase != TRANSITION_IDLE;
}
