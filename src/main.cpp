
#include "raylib.h"
#include "maze.h"
#include "player.h"
#include "game.h"
#include "effects.h"
#include "audio.h"
#include "enemy.h"
#include "boss.h"
#include "transition.h"
#include "pixel_art.h"

// --------------------------------------------------
// CONVERT WINDOW MOUSE POSITION TO GAME COORDINATES
// --------------------------------------------------

static Vector2 GetLogicalMousePosition(
    int gameWidth,
    int gameHeight
)
{
    float scaleX =
        (float)GetScreenWidth() / gameWidth;

    float scaleY =
        (float)GetScreenHeight() / gameHeight;

    float scale = scaleX < scaleY ? scaleX : scaleY;

    float displayWidth = gameWidth * scale;
    float displayHeight = gameHeight * scale;

    float offsetX =
        (GetScreenWidth() - displayWidth) / 2.0f;

    float offsetY =
        (GetScreenHeight() - displayHeight) / 2.0f;

    Vector2 mouse = GetMousePosition();

    return {
        (mouse.x - offsetX) / scale,
        (mouse.y - offsetY) / scale
    };
}

// --------------------------------------------------
// CONFIGURE ENEMIES FOR THE CURRENT LEVEL
// --------------------------------------------------

void ConfigureEnemiesForCurrentLevel(
    GameState &game,
    Player &player,
    Enemy enemies[],
    int &activeEnemyCount
)
{
    const LevelConfig &config =
        GetLevelConfig(game.currentLevel);

    if (config.isBossLevel)
    {
        activeEnemyCount = 0;
        return;
    }

    activeEnemyCount = InitializeEnemies(
        enemies,
        config.enemyCount,
        player
    );

    for (int i = 0; i < activeEnemyCount; i++)
    {
        enemies[i].speed = config.enemySpeed;
    }
}

// --------------------------------------------------
// START THE NEXT LEVEL
// --------------------------------------------------

void StartNextLevel(
    GameState &game,
    Player &player,
    Enemy enemies[],
    int &activeEnemyCount,
    EffectsState &effects
)
{
    int nextLevel = game.currentLevel + 1;

    InitializeLevel(game, nextLevel);
    InitializePlayer(player);

    ConfigureEnemiesForCurrentLevel(
        game,
        player,
        enemies,
        activeEnemyCount
    );

    InitializeEffects(effects);
}

// --------------------------------------------------
// JUMP TO A SPECIFIC LEVEL (TESTING)
// --------------------------------------------------

void JumpToLevel(
    int level,
    GameState &game,
    Player &player,
    Enemy enemies[],
    int &activeEnemyCount,
    EffectsState &effects
)
{
    InitializeLevel(game, level);
    InitializePlayer(player);

    ConfigureEnemiesForCurrentLevel(
        game,
        player,
        enemies,
        activeEnemyCount
    );

    InitializeEffects(effects);
}

// --------------------------------------------------
// TRANSITION ACTIONS
// --------------------------------------------------

enum TransitionAction
{
    ACTION_NONE,
    ACTION_START_GAME,
    ACTION_LOAD_LEVEL,
    ACTION_NEXT_LEVEL,
    ACTION_SHOW_VICTORY,
    ACTION_SHOW_DEFEAT,
    ACTION_RESTART_GAME
};

// --------------------------------------------------
// REQUEST A SCREEN TRANSITION
// --------------------------------------------------

void RequestTransition(
    ScreenTransition &transition,
    TransitionAction &pendingAction,
    int &requestedLevel,
    TransitionAction action,
    int level = 0
)
{
    if (IsTransitionActive(transition))
    {
        return;
    }

    pendingAction = action;
    requestedLevel = level;

    BeginTransition(transition);
}

// --------------------------------------------------
// APPLY ACTION AT FULL BLACK
// --------------------------------------------------

void ApplyPendingTransition(
    TransitionAction &pendingAction,
    int &requestedLevel,
    Screen &currentScreen,
    GameState &game,
    Player &player,
    Enemy enemies[],
    int &activeEnemyCount,
    EffectsState &effects
)
{
    switch (pendingAction)
    {
        case ACTION_START_GAME:
        {
            currentScreen = GAMEPLAY;
            break;
        }

        case ACTION_LOAD_LEVEL:
        {
            JumpToLevel(
                requestedLevel,
                game,
                player,
                enemies,
                activeEnemyCount,
                effects
            );

            currentScreen = GAMEPLAY;
            break;
        }

        case ACTION_NEXT_LEVEL:
        {
            StartNextLevel(
                game,
                player,
                enemies,
                activeEnemyCount,
                effects
            );

            currentScreen = GAMEPLAY;
            break;
        }

        case ACTION_SHOW_VICTORY:
        {
            currentScreen = VICTORY;
            break;
        }

        case ACTION_SHOW_DEFEAT:
        {
            currentScreen = DEFEAT;
            break;
        }

        case ACTION_RESTART_GAME:
        {
            InitializeGame(game);
            InitializePlayer(player);
            InitializeEffects(effects);

            ConfigureEnemiesForCurrentLevel(
                game,
                player,
                enemies,
                activeEnemyCount
            );

            currentScreen = GAMEPLAY;
            break;
        }

        case ACTION_NONE:
        default:
        {
            break;
        }
    }

    pendingAction = ACTION_NONE;
    requestedLevel = 0;
}

// --------------------------------------------------
// MAIN
// --------------------------------------------------

int main()
{
    // Allow resizing and maximizing.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // Logical game resolution.
    const int gameWidth = 1000;
    const int gameHeight = 700;

    // Initial window resolution.
    const int screenWidth = 1400;
    const int screenHeight = 980;

    InitWindow(
        screenWidth,
        screenHeight,
        "Maze Runner - The Neon Escape"
    );

    SetWindowMinSize(800, 560);

    SetTargetFPS(60);

    // Create the logical game render texture.
    RenderTexture2D gameTexture =
        LoadRenderTexture(gameWidth, gameHeight);

    // Use nearest-neighbour filtering to preserve
    // crisp pixel-art edges.
    SetTextureFilter(
        gameTexture.texture,
        TEXTURE_FILTER_POINT
    );

    // --------------------------------
    // GAME OBJECTS
    // --------------------------------

    Player player;
    GameState game;
    EffectsState effects;
    AudioState audio;

    Enemy enemies[MAX_ENEMIES];

    int activeEnemyCount = 0;

    Boss boss;

    // Initialize the game.
    InitializeGame(game);

    InitializePlayer(player);

    InitializeEffects(effects);

    InitializeAudio(audio);

    // --------------------------------
    // BACKGROUND MUSIC
    // --------------------------------

    // Load the original soundtrack.
    // The game continues normally if loading fails.
    if (
        LoadBackgroundMusic(
            audio,
            "assets/audio/background_music.ogg"
        )
    )
    {
        PlayBackgroundMusic(audio);
    }

    InitializeBoss(boss);

    // Initialize Level 1 enemies.
    ConfigureEnemiesForCurrentLevel(
        game,
        player,
        enemies,
        activeEnemyCount
    );

    // Start at the title screen.
    Screen currentScreen = TITLE;

    // Initialize screen transition controller.
    ScreenTransition transition;
    InitializeTransition(transition);

    // Pending transition action.
    TransitionAction pendingAction = ACTION_NONE;

    int requestedLevel = 0;

    // --------------------------------
    // MAIN GAME LOOP
    // --------------------------------

    while (!WindowShouldClose())
    {
        // --------------------------------
        // UPDATE BACKGROUND MUSIC
        // --------------------------------

        // Update the music stream every frame.
        // This allows the soundtrack to loop smoothly.
        UpdateBackgroundMusic(audio);

        // --------------------------------
        // UPDATE SCREEN TRANSITIONS
        // --------------------------------

        if (UpdateTransition(transition))
        {
            ApplyPendingTransition(
                pendingAction,
                requestedLevel,
                currentScreen,
                game,
                player,
                enemies,
                activeEnemyCount,
                effects
            );

            BeginFadeIn(transition);
        }

        // --------------------------------
        // TITLE SCREEN INPUT
        // --------------------------------

        if (
            currentScreen == TITLE &&
            !IsTransitionActive(transition)
        )
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                RequestTransition(
                    transition,
                    pendingAction,
                    requestedLevel,
                    ACTION_START_GAME
                );
            }
        }

        // --------------------------------
        // TEMPORARY LEVEL TESTING SHORTCUTS
        // --------------------------------

        if (
            currentScreen == GAMEPLAY &&
            !IsTransitionActive(transition)
        )
        {
            const int levelKeys[MAX_LEVELS] =
            {
                KEY_ONE,
                KEY_TWO,
                KEY_THREE,
                KEY_FOUR,
                KEY_FIVE,
                KEY_SIX,
                KEY_SEVEN,
                KEY_EIGHT,
                KEY_NINE,
                KEY_ZERO
            };

            if (IsKeyDown(KEY_LEFT_CONTROL))
            {
                for (int i = 0; i < MAX_LEVELS; i++)
                {
                    if (IsKeyPressed(levelKeys[i]))
                    {
                        RequestTransition(
                            transition,
                            pendingAction,
                            requestedLevel,
                            ACTION_LOAD_LEVEL,
                            i + 1
                        );

                        break;
                    }
                }
            }
        }

        // --------------------------------
        // PAUSE INPUT
        // --------------------------------

        if (
            currentScreen == GAMEPLAY &&
            !IsTransitionActive(transition) &&
            game.status == PLAYING
        )
        {
            Vector2 mousePosition =
                GetLogicalMousePosition(gameWidth, gameHeight);

            Rectangle pauseButton = {
                925, 20, 55, 38
            };

            Rectangle resumeButton = {
                400, 390, 200, 54
            };

            bool pauseKeyPressed =
                IsKeyPressed(KEY_P) ||
                IsKeyPressed(KEY_ESCAPE);

            bool pauseButtonClicked =
                CheckCollisionPointRec(
                    mousePosition,
                    pauseButton
                ) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            bool resumeButtonClicked =
                game.isPaused &&
                CheckCollisionPointRec(
                    mousePosition,
                    resumeButton
                ) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if (game.isPaused)
            {
                if (pauseKeyPressed || resumeButtonClicked)
                {
                    game.isPaused = false;
                }
            }
            else
            {
                if (pauseKeyPressed || pauseButtonClicked)
                {
                    game.isPaused = true;
                }
            }
        }

        // --------------------------------
        // GAMEPLAY UPDATE
        // --------------------------------

        if (
            currentScreen == GAMEPLAY &&
            !IsTransitionActive(transition)
        )
        {
            if (game.status == PLAYING && !game.isPaused)
            {
                // Update player movement.
                UpdatePlayer(player);

                // Update all active enemies.
                UpdateEnemies(
                    enemies,
                    activeEnemyCount,
                    player
                );

                // Update the boss only during Level 10.
                if (game.currentLevel == 10)
                {
                    UpdateBoss(boss, player);
                }

                // --------------------------------
                // PLAYER-ENEMY COLLISION
                // --------------------------------

                bool enemyCollision =
                    IsAnyEnemyCollidingWithPlayer(
                        enemies,
                        activeEnemyCount,
                        player
                    );

                bool bossProjectileCollision = false;

                if (game.currentLevel == 10)
                {
                    bossProjectileCollision =
                        IsBossProjectileCollidingWithPlayer(
                            boss,
                            player
                        );
                }

                if (
                    game.invulnerabilityTimer <= 0.0f &&
                    (enemyCollision || bossProjectileCollision)
                )
                {
                    game.lives--;

                    game.invulnerabilityTimer = 2.0f;

                    InitializePlayer(player);

                    InitializeEnemies(
                        enemies,
                        activeEnemyCount,
                        player
                    );

                    if (game.lives <= 0)
                    {
                        game.status = LOST;
                    }
                }

                // --------------------------------
                // UPDATE GAME OBJECTIVES
                // --------------------------------

                int previousScore = game.score;

                bool previousExitUnlocked =
                    game.exitUnlocked;

                UpdateGame(game, player);

                // --------------------------------
                // DETECT CRYSTAL COLLECTION
                // --------------------------------

                if (game.score > previousScore)
                {
                    if (
                        game.crystalsCollected >=
                        game.totalCrystals
                    )
                    {
                        SpawnFinalCrystalBurst(
                            effects,
                            player.position
                        );
                    }
                    else
                    {
                        SpawnCrystalParticles(
                            effects,
                            player.position
                        );
                    }

                    PlayCrystalSound(audio);
                }

                // --------------------------------
                // DETECT EXIT UNLOCKING
                // --------------------------------

                if (
                    !previousExitUnlocked &&
                    game.exitUnlocked
                )
                {
                    TriggerExitUnlock(effects);
                }

                // --------------------------------
                // DETECT GAME OUTCOME
                // --------------------------------

                if (game.status == WON)
                {
                    TriggerLevelComplete(effects);

                    if (game.currentLevel < game.maxLevels)
                    {
                        RequestTransition(
                            transition,
                            pendingAction,
                            requestedLevel,
                            ACTION_NEXT_LEVEL
                        );
                    }
                    else
                    {
                        PlayVictorySound(audio);

                        RequestTransition(
                            transition,
                            pendingAction,
                            requestedLevel,
                            ACTION_SHOW_VICTORY
                        );
                    }
                }
                else if (game.status == LOST)
                {
                    PlayDefeatSound(audio);

                    RequestTransition(
                        transition,
                        pendingAction,
                        requestedLevel,
                        ACTION_SHOW_DEFEAT
                    );
                }
            }
        }

        // --------------------------------
        // UPDATE PARTICLE AND EVENT EFFECTS
        // --------------------------------

        if (currentScreen == GAMEPLAY && !game.isPaused)
        {
            UpdateEffects(effects);
        }

        // --------------------------------
        // RESTART GAME
        // --------------------------------

        if (
            !IsTransitionActive(transition) &&
            (currentScreen == VICTORY ||
             currentScreen == DEFEAT)
        )
        {
            if (IsKeyPressed(KEY_R))
            {
                RequestTransition(
                    transition,
                    pendingAction,
                    requestedLevel,
                    ACTION_RESTART_GAME
                );
            }
        }

        // --------------------------------
        // DRAW TO LOGICAL GAME TEXTURE
        // --------------------------------

        BeginTextureMode(gameTexture);

        ClearBackground(Color{7, 12, 28, 255});

        // --------------------------------
        // TITLE SCREEN
        // --------------------------------

        if (currentScreen == TITLE)
        {
            DrawTitleScreen();
        }

        // --------------------------------
        // GAMEPLAY SCREEN
        // --------------------------------

        else if (currentScreen == GAMEPLAY)
        {
            DrawPixelText(
                "MAZE RUNNER",
                365, 20,
                4,
                PIXEL_CYAN
            );

            DrawPixelText(
                "THE NEON ESCAPE",
                405, 52,
                2,
                PIXEL_LIGHT_GRAY
            );

            DrawPixelText(
                TextFormat(
                    "LEVEL %i / %i",
                    game.currentLevel,
                    game.maxLevels
                ),
                790, 28,
                2,
                PIXEL_GREEN
            );

            DrawMaze(
                game.exitUnlocked,
                (float)GetTime()
            );

            if (
                game.invulnerabilityTimer <= 0.0f ||
                ((int)(GetTime() * 10.0f) % 2 == 0)
            )
            {
                DrawPlayer(player);
            }

            DrawEnemies(
                enemies,
                activeEnemyCount
            );

            if (game.currentLevel == 10)
            {
                DrawBoss(boss);
                DrawBossProjectiles(boss);
            }

            DrawEffects(effects);

            DrawHUD(game);

            // --------------------------------
            // PAUSE BUTTON
            // --------------------------------

            DrawRectangle(
                925, 20,
                55, 38,
                PIXEL_NAVY
            );

            DrawRectangleLinesEx(
                Rectangle{925, 20, 55, 38},
                2,
                PIXEL_CYAN
            );

            // Two vertical bars forming the pause icon.
            DrawRectangle(
                942, 29,
                5, 19,
                PIXEL_CYAN
            );

            DrawRectangle(
                957, 29,
                5, 19,
                PIXEL_CYAN
            );

            // --------------------------------
            // PAUSE OVERLAY
            // --------------------------------

            if (game.isPaused)
            {
                // Dim the background.
                DrawRectangle(
                    0, 0,
                    gameWidth, gameHeight,
                    Color{8, 8, 16, 200}
                );

                // Main pause panel.
                DrawRectangle(
                    300, 245,
                    400, 210,
                    PIXEL_NAVY
                );

                DrawRectangleLinesEx(
                    Rectangle{300, 245, 400, 210},
                    3,
                    PIXEL_CYAN
                );

                // Heading.
                DrawPixelTextCentered(
                    "PAUSED",
                    500,
                    285,
                    5,
                    PIXEL_CYAN
                );

                // Resume button.
                DrawRectangle(
                    400, 390,
                    200, 54,
                    PIXEL_BLUE
                );

                DrawRectangleLinesEx(
                    Rectangle{400, 390, 200, 54},
                    2,
                    PIXEL_CYAN
                );

                DrawPixelTextCentered(
                    "RESUME",
                    500,
                    408,
                    2,
                    PIXEL_WHITE
                );

                DrawPixelTextCentered(
                    "PRESS P OR ESC",
                    500,
                    465,
                    1,
                    PIXEL_LIGHT_GRAY
                );
            }
        }

        // --------------------------------
        // VICTORY / DEFEAT SCREENS
        // --------------------------------

        else if (
            currentScreen == VICTORY ||
            currentScreen == DEFEAT
        )
        {
            DrawMaze(
                game.exitUnlocked,
                (float)GetTime()
            );

            DrawPlayer(player);

            DrawGameOverlay(game);
        }

        // --------------------------------
        // SCREEN TRANSITION OVERLAY
        // --------------------------------

        DrawTransitionOverlay(transition);

        EndTextureMode();

        // --------------------------------
        // DISPLAY SCALED GAME TEXTURE
        // --------------------------------

        BeginDrawing();

        ClearBackground(BLACK);

        float currentWidth = (float)GetScreenWidth();
        float currentHeight = (float)GetScreenHeight();

        float scaleX = currentWidth / gameWidth;
        float scaleY = currentHeight / gameHeight;

        float scale = scaleX < scaleY ? scaleX : scaleY;

        float displayWidth = gameWidth * scale;
        float displayHeight = gameHeight * scale;

        float offsetX = (currentWidth - displayWidth) / 2.0f;
        float offsetY = (currentHeight - displayHeight) / 2.0f;

        DrawTexturePro(
            gameTexture.texture,

            Rectangle{
                0.0f,
                0.0f,
                (float)gameTexture.texture.width,
                -(float)gameTexture.texture.height
            },

            Rectangle{
                offsetX,
                offsetY,
                displayWidth,
                displayHeight
            },

            Vector2{0.0f, 0.0f},
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    // --------------------------------
    // CLEANUP
    // --------------------------------

    CloseAudio(audio);

    UnloadRenderTexture(gameTexture);

    CloseWindow();

    return 0;
}
