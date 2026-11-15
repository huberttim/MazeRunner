
#include "audio.h"

#include <cmath>

// --------------------------------------------------
// GENERATE SHORT ELECTRONIC SOUND EFFECTS
// --------------------------------------------------

static Sound GenerateTone(
    const float frequencies[],
    int noteCount,
    float noteDuration
)
{
    const unsigned int sampleRate = 44100;

    unsigned int samplesPerNote =
        (unsigned int)(sampleRate * noteDuration);

    Wave wave = {};

    wave.frameCount = samplesPerNote * noteCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    short *samples = (short *)MemAlloc(
        wave.frameCount * sizeof(short)
    );

    if (samples == nullptr)
        return Sound{};

    for (unsigned int i = 0; i < wave.frameCount; i++)
    {
        unsigned int noteIndex = i / samplesPerNote;
        unsigned int notePosition = i % samplesPerNote;

        float frequency = frequencies[noteIndex];

        float t = (float)notePosition / sampleRate;

        float attack = (float)notePosition / 300.0f;

        float release =
            (float)(samplesPerNote - notePosition) / 1500.0f;

        float envelope = fminf(1.0f, attack);
        envelope = fminf(envelope, release);
        envelope = fmaxf(0.0f, envelope);

        float sample = sinf(2.0f * PI * frequency * t);

        samples[i] = (short)(
            sample * envelope * 10000.0f
        );
    }

    wave.data = samples;

    Sound sound = LoadSoundFromWave(wave);

    UnloadWave(wave);

    return sound;
}

// --------------------------------------------------
// INITIALIZE AUDIO SYSTEM
// --------------------------------------------------

void InitializeAudio(AudioState &audio)
{
    audio.ready = false;
    audio.musicReady = false;

    audio.backgroundMusic = Music{};

    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
    }

    if (!IsAudioDeviceReady())
    {
        TraceLog(LOG_WARNING, "Audio device unavailable.");
        return;
    }

    // Crystal collection
    float crystalNotes[] = {
        880.0f, 1174.66f
    };

    // Victory
    float victoryNotes[] = {
        523.25f, 659.25f, 783.99f, 1046.50f
    };

    // Defeat
    float defeatNotes[] = {
        523.25f, 392.00f, 293.66f
    };

    audio.crystalSound =
        GenerateTone(crystalNotes, 2, 0.09f);

    audio.victorySound =
        GenerateTone(victoryNotes, 4, 0.16f);

    audio.defeatSound =
        GenerateTone(defeatNotes, 3, 0.20f);

    audio.ready = true;

    SetSoundVolume(audio.crystalSound, 0.45f);
    SetSoundVolume(audio.victorySound, 0.55f);
    SetSoundVolume(audio.defeatSound, 0.50f);
}

// --------------------------------------------------
// SOUND EFFECT PLAYBACK
// --------------------------------------------------

void PlayCrystalSound(const AudioState &audio)
{
    if (audio.ready)
        PlaySound(audio.crystalSound);
}

void PlayVictorySound(const AudioState &audio)
{
    if (audio.ready)
        PlaySound(audio.victorySound);
}

void PlayDefeatSound(const AudioState &audio)
{
    if (audio.ready)
        PlaySound(audio.defeatSound);
}

// --------------------------------------------------
// LOAD BACKGROUND MUSIC
// --------------------------------------------------

bool LoadBackgroundMusic(
    AudioState &audio,
    const char *filePath
)
{
    if (!audio.ready || filePath == nullptr)
        return false;

    // Release an existing track before loading another.
    if (audio.musicReady)
    {
        StopMusicStream(audio.backgroundMusic);
        UnloadMusicStream(audio.backgroundMusic);

        audio.musicReady = false;
    }

    audio.backgroundMusic = LoadMusicStream(filePath);

    // Raylib does not provide IsMusicReady().
    // Check whether the music stream has valid audio data.
    if (
        audio.backgroundMusic.stream.buffer == nullptr &&
        audio.backgroundMusic.stream.processor == nullptr
    )
    {
        TraceLog(
            LOG_WARNING,
            "Could not load background music: %s",
            filePath
        );

        return false;
    }

    // Background music should sit beneath sound effects.
    SetMusicVolume(audio.backgroundMusic, 0.20f);

    // Allow the soundtrack to repeat.
    audio.backgroundMusic.looping = true;

    audio.musicReady = true;

    return true;
}

// --------------------------------------------------
// BACKGROUND MUSIC PLAYBACK
// --------------------------------------------------

void PlayBackgroundMusic(AudioState &audio)
{
    if (!audio.musicReady)
        return;

    if (!IsMusicStreamPlaying(audio.backgroundMusic))
    {
        PlayMusicStream(audio.backgroundMusic);
    }
}

// --------------------------------------------------
// UPDATE STREAMING MUSIC
// --------------------------------------------------

void UpdateBackgroundMusic(const AudioState &audio)
{
    if (!audio.musicReady)
        return;

    UpdateMusicStream(audio.backgroundMusic);
}

// --------------------------------------------------
// STOP BACKGROUND MUSIC
// --------------------------------------------------

void StopBackgroundMusic(AudioState &audio)
{
    if (!audio.musicReady)
        return;

    StopMusicStream(audio.backgroundMusic);
}

// --------------------------------------------------
// CONTROL BACKGROUND MUSIC VOLUME
// --------------------------------------------------

void SetBackgroundMusicVolume(
    AudioState &audio,
    float volume
)
{
    if (!audio.musicReady)
        return;

    if (volume < 0.0f)
        volume = 0.0f;

    if (volume > 1.0f)
        volume = 1.0f;

    SetMusicVolume(audio.backgroundMusic, volume);
}

// --------------------------------------------------
// RELEASE AUDIO RESOURCES
// --------------------------------------------------

void CloseAudio(AudioState &audio)
{
    if (audio.musicReady)
    {
        StopMusicStream(audio.backgroundMusic);
        UnloadMusicStream(audio.backgroundMusic);

        audio.musicReady = false;
    }

    if (audio.ready)
    {
        UnloadSound(audio.crystalSound);
        UnloadSound(audio.victorySound);
        UnloadSound(audio.defeatSound);

        audio.ready = false;
    }

    if (IsAudioDeviceReady())
    {
        CloseAudioDevice();
    }
}
