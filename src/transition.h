
#ifndef TRANSITION_H
#define TRANSITION_H

#include "raylib.h"

// --------------------------------------------------
// TRANSITION STATES
// --------------------------------------------------

enum TransitionPhase
{
    TRANSITION_IDLE,
    TRANSITION_FADING_OUT,
    TRANSITION_HOLD,
    TRANSITION_FADING_IN
};

// --------------------------------------------------
// TRANSITION DATA
// --------------------------------------------------

struct ScreenTransition
{
    float alpha;
    float speed;
    float holdTimer;

    TransitionPhase phase;
};

// --------------------------------------------------
// TRANSITION FUNCTIONS
// --------------------------------------------------

void InitializeTransition(ScreenTransition &transition);

void BeginTransition(ScreenTransition &transition);

bool UpdateTransition(ScreenTransition &transition);

void BeginFadeIn(ScreenTransition &transition);

void DrawTransitionOverlay(const ScreenTransition &transition);

bool IsTransitionActive(const ScreenTransition &transition);

#endif
