
#ifndef EFFECTS_H
#define EFFECTS_H

#include "raylib.h"

const int MAX_PARTICLES = 60;

struct Particle
{
    Vector2 position;
    Vector2 previousPosition;
    Vector2 velocity;

    float lifetime;
    float maxLifetime;
    float radius;

    Color color;

    bool active;
};

struct EffectsState
{
    Particle particles[MAX_PARTICLES];

    Vector2 flashPosition;
    float flashLifetime;
    float flashMaxLifetime;
    bool flashActive;

    float exitUnlockTimer;
    float levelCompleteTimer;
};

void InitializeEffects(EffectsState &effects);

void SpawnCrystalParticles(
    EffectsState &effects,
    Vector2 position
);

void SpawnFinalCrystalBurst(
    EffectsState &effects,
    Vector2 position
);

void TriggerExitUnlock(EffectsState &effects);

void TriggerLevelComplete(EffectsState &effects);

void UpdateEffects(EffectsState &effects);

void DrawEffects(const EffectsState &effects);

#endif
