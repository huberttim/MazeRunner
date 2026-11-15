
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

struct AudioState
{
    Sound crystalSound;
    Sound victorySound;
    Sound defeatSound;

    Music backgroundMusic;

    bool ready;
    bool musicReady;
};

void InitializeAudio(AudioState &audio);

void PlayCrystalSound(const AudioState &audio);
void PlayVictorySound(const AudioState &audio);
void PlayDefeatSound(const AudioState &audio);

bool LoadBackgroundMusic(
    AudioState &audio,
    const char *filePath
);

void PlayBackgroundMusic(AudioState &audio);
void UpdateBackgroundMusic(const AudioState &audio);
void StopBackgroundMusic(AudioState &audio);

void SetBackgroundMusicVolume(
    AudioState &audio,
    float volume
);

void CloseAudio(AudioState &audio);

#endif
