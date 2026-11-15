
#include "game.h"
#include "maze.h"
#include "pixel_art.h"

#include <cstdio>
#include <cmath>

// --------------------------------------------------
// TEN-LEVEL CONFIGURATION
// --------------------------------------------------

// enemyCount represents ordinary enemies.
// Level 10 reserves four ordinary enemies for the
// boss encounter. The boss is handled separately.
//
// enemySpeed is measured in pixels per second.
// Level 10 uses special boss movement instead.

const LevelConfig LEVEL_CONFIGS[MAX_LEVELS] =
{
    // Level 1: The Initiation
    {1,  95.0f, 100.0f, false},

    // Level 2: The Pursuit
    {1, 110.0f, 100.0f, false},

    // Level 3: The Crossroads
    {2, 105.0f,  95.0f, false},

    // Level 4: The Labyrinth
    {2, 120.0f,  95.0f, false},

    // Level 5: The Ambush
    {3, 115.0f,  90.0f, false},

    // Level 6: The Pressure
    {3, 130.0f,  90.0f, false},

    // Level 7: The Gauntlet
    {4, 125.0f,  85.0f, false},

    // Level 8: The Hunt
    {4, 145.0f,  85.0f, false},

    // Level 9: The Final Descent
    {5, 140.0f,  90.0f, false},

    // Level 10: The Sentinel's Domain
    {4,   0.0f, 120.0f, true}
};

// --------------------------------------------------
// GET LEVEL CONFIGURATION
// --------------------------------------------------

const LevelConfig &GetLevelConfig(int level)
{
    // Keep the requested level within valid bounds.
    if (level < 1)
    {
        level = 1;
    }

    if (level > MAX_LEVELS)
    {
        level = MAX_LEVELS;
    }

    return LEVEL_CONFIGS[level - 1];
}

// --------------------------------------------------
// INITIALIZE AN INDIVIDUAL LEVEL
// --------------------------------------------------

void InitializeLevel(GameState &game, int level)
{
    // Validate the requested level.
    if (level < 1)
    {
        level = 1;
    }

    if (level > MAX_LEVELS)
    {
        level = MAX_LEVELS;
    }

    game.currentLevel = level;
    game.maxLevels = MAX_LEVELS;

    // Load the selected maze layout.
    SetMazeLevel(level);

    // Get this level's configuration.
    const LevelConfig &config = GetLevelConfig(level);

    // Reset level-specific progress.
    game.crystalsCollected = 0;
    game.totalCrystals = 0;

    game.exitUnlocked = false;

    // Restore three lives at the beginning
    // of every new level.
    game.lives = MAX_LIVES;

    // Apply the level's time limit.
    game.timeRemaining = config.timeLimit;

    // Remove any remaining invulnerability.
    game.invulnerabilityTimer = 0.0f;

    // Start the level in the playing state.
    game.status = PLAYING;

    // Count the collectibles in the loaded maze.
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (maze[row][col] == COLLECTIBLE)
            {
                game.totalCrystals++;
            }
        }
    }
}

// --------------------------------------------------
// INITIALIZE A COMPLETELY NEW GAME
// --------------------------------------------------

void InitializeGame(GameState &game)
{
    // Reset the overall score.
    game.score = 0;

    // Start from Level 1.
    InitializeLevel(game, 1);
}

// --------------------------------------------------
// UPDATE GAME LOGIC
// --------------------------------------------------

void UpdateGame(GameState &game, Player &player)
{
    if (game.status != PLAYING)
        return;

    // Update temporary invulnerability.
    if (game.invulnerabilityTimer > 0.0f)
    {
        game.invulnerabilityTimer -= GetFrameTime();

        if (game.invulnerabilityTimer < 0.0f)
        {
            game.invulnerabilityTimer = 0.0f;
        }
    }

    // Update countdown timer.
    game.timeRemaining -= GetFrameTime();

    if (game.timeRemaining <= 0.0f)
    {
        game.timeRemaining = 0.0f;
        game.status = LOST;
        return;
    }

    // Process a tile only when the player
    // has finished moving.
    if (!player.isMoving)
    {
        int row = player.gridRow;
        int col = player.gridCol;

        // Collect a crystal.
        if (maze[row][col] == COLLECTIBLE)
        {
            maze[row][col] = PATH;

            game.crystalsCollected++;
            game.score += 10;

            // Unlock the exit when all crystals
            // have been collected.
            if (game.crystalsCollected == game.totalCrystals)
            {
                game.exitUnlocked = true;
            }
        }

        // Check whether the player has reached
        // the unlocked exit.
        if (maze[row][col] == EXIT && game.exitUnlocked)
        {
            game.status = WON;
        }
    }
}

// --------------------------------------------------
// DRAW PIXEL-ART HUD
// --------------------------------------------------

void DrawHUD(const GameState &game)
{
    const int panelX = 105;
    const int panelY = 620;
    const int panelWidth = 790;
    const int panelHeight = 65;

    const Color mutedText = PIXEL_MID_GRAY;

    const Color scoreColor = PIXEL_CYAN;
    const Color crystalColor = PIXEL_PURPLE;

    const Color timeColor =
        game.timeRemaining <= 20.0f
            ? PIXEL_RED
            : PIXEL_GREEN;

    const Color exitColor =
        game.exitUnlocked
            ? PIXEL_GREEN
            : PIXEL_RED;

    // --------------------------------------------------
    // LIVES INDICATOR
    // --------------------------------------------------

    DrawPixelText(
        "LIVES",
        105, 25,
        2,
        PIXEL_LIGHT_GRAY
    );

    const char* heartRows[] =
    {
        ".RR.RR.",
        "RRRRRRR",
        "RRRRRRR",
        ".RRRRR.",
        "..RRR..",
        "...R...",
        "......."
    };

    const PixelColor heartPalette[] =
    {
        {'R', PIXEL_RED},
        {'D', PIXEL_DARK_GRAY}
    };

    for (int i = 0; i < MAX_LIVES; i++)
    {
        int heartX = 175 + i * 27 - 7;

        if (i < game.lives)
        {
            DrawPixelSprite(
                heartRows,
                7, 7,
                heartX, 27,
                2,
                heartPalette,
                2
            );
        }
        else
        {
            const char* emptyHeartRows[] =
            {
                ".DD.DD.",
                "D.....D",
                "D.....D",
                ".D...D.",
                "..D.D..",
                "...D...",
                "......."
            };

            DrawPixelSprite(
                emptyHeartRows,
                7, 7,
                heartX, 27,
                2,
                heartPalette,
                2
            );
        }
    }

    // --------------------------------------------------
    // MAIN HUD PANEL
    // --------------------------------------------------

    DrawRectangle(
        panelX,
        panelY,
        panelWidth,
        panelHeight,
        PIXEL_NAVY
    );

    // Pixel border.
    DrawRectangle(
        panelX,
        panelY,
        panelWidth,
        2,
        PIXEL_BLUE
    );

    DrawRectangle(
        panelX,
        panelY + panelHeight - 2,
        panelWidth,
        2,
        PIXEL_BLUE
    );

    DrawRectangle(
        panelX,
        panelY,
        2,
        panelHeight,
        PIXEL_BLUE
    );

    DrawRectangle(
        panelX + panelWidth - 2,
        panelY,
        2,
        panelHeight,
        PIXEL_BLUE
    );

    // Top accent.
    DrawRectangle(
        panelX + 3,
        panelY + 3,
        panelWidth - 6,
        2,
        PIXEL_CYAN
    );

    // Vertical dividers.
    for (int i = 1; i <= 3; i++)
    {
        int dividerX = panelX + i * 197;

        DrawRectangle(
            dividerX,
            panelY + 13,
            2,
            panelHeight - 23,
            PIXEL_DARK_BLUE
        );
    }

    // --------------------------------------------------
    // SCORE
    // --------------------------------------------------

    DrawRectangle(
        133, 641,
        6, 6,
        scoreColor
    );

    DrawPixelText(
        "SCORE",
        145, 630,
        2,
        mutedText
    );

    DrawPixelText(
        TextFormat("%03i", game.score),
        130, 649,
        3,
        scoreColor
    );

    // --------------------------------------------------
    // CRYSTALS
    // --------------------------------------------------

    DrawRectangle(
        330, 641,
        6, 6,
        crystalColor
    );

    DrawPixelText(
        "CRYSTALS",
        342, 630,
        2,
        mutedText
    );

    DrawPixelText(
        TextFormat(
            "%i / %i",
            game.crystalsCollected,
            game.totalCrystals
        ),
        330, 649,
        3,
        crystalColor
    );

    // Crystal collection progress.
    float crystalProgress = 0.0f;

    if (game.totalCrystals > 0)
    {
        crystalProgress =
            (float)game.crystalsCollected /
            (float)game.totalCrystals;
    }

    DrawRectangle(
        330, 675,
        135, 4,
        PIXEL_DARK_BLUE
    );

    DrawRectangle(
        330, 675,
        (int)(135 * crystalProgress),
        4,
        crystalColor
    );

    // --------------------------------------------------
    // TIME
    // --------------------------------------------------

    DrawRectangle(
        527, 641,
        6, 6,
        timeColor
    );

    DrawPixelText(
        "TIME LEFT",
        539, 630,
        2,
        mutedText
    );

    DrawPixelText(
        TextFormat("%02i", (int)game.timeRemaining),
        527, 649,
        3,
        timeColor
    );

    // --------------------------------------------------
    // EXIT STATUS
    // --------------------------------------------------

    DrawRectangle(
        724, 641,
        6, 6,
        exitColor
    );

    DrawPixelText(
        "EXIT STATUS",
        736, 630,
        2,
        mutedText
    );

    DrawPixelText(
        game.exitUnlocked ? "UNLOCKED" : "LOCKED",
        724, 650,
        2,
        exitColor
    );
}

// --------------------------------------------------
// DRAW PIXEL-ART WIN / LOSS SCREEN
// --------------------------------------------------

void DrawGameOverlay(const GameState &game)
{
    if (game.status == PLAYING)
        return;

    const bool victory = game.status == WON;

    const Color accent = victory
        ? PIXEL_GREEN
        : PIXEL_RED;

    const Color secondary = victory
        ? PIXEL_CYAN
        : PIXEL_PURPLE;

    const Color mutedText = PIXEL_LIGHT_GRAY;

    float pulse =
        (sinf((float)GetTime() * 3.0f) + 1.0f) / 2.0f;

    // --------------------------------------------------
    // BACKGROUND
    // --------------------------------------------------

    DrawRectangle(
        0, 0, 1000, 700,
        PIXEL_BLACK
    );

    // Decorative horizontal pixel lines.
    DrawRectangle(
        120, 110, 760, 3,
        PIXEL_NAVY
    );

    DrawRectangle(
        120, 587, 760, 3,
        PIXEL_NAVY
    );

    // --------------------------------------------------
    // MAIN PANEL
    // --------------------------------------------------

    DrawRectangle(
        190, 150, 620, 400,
        PIXEL_NAVY
    );

    // Pixel border.
    DrawRectangle(190, 150, 620, 3, accent);
    DrawRectangle(190, 547, 620, 3, accent);
    DrawRectangle(190, 150, 3, 400, accent);
    DrawRectangle(807, 150, 3, 400, accent);

    // Corner accents.
    DrawRectangle(190, 150, 42, 5, secondary);
    DrawRectangle(768, 150, 42, 5, secondary);
    DrawRectangle(190, 545, 42, 5, secondary);
    DrawRectangle(768, 545, 42, 5, secondary);

    // Animated pixel indicators.
    int indicatorHeight =
        4 + (int)(pulse * 8.0f);

    DrawRectangle(
        215, 180,
        8, indicatorHeight,
        accent
    );

    DrawRectangle(
        777, 180,
        8, indicatorHeight,
        accent
    );

    // --------------------------------------------------
    // HEADING
    // --------------------------------------------------

    const char* heading = victory
        ? "MAZE CLEARED!"
        : "GAME OVER";

    DrawPixelTextCentered(
        heading,
        500,
        205,
        4,
        accent
    );

    // --------------------------------------------------
    // MESSAGE
    // --------------------------------------------------

    const char* message = victory
        ? "ALL CRYSTALS COLLECTED. ESCAPE SUCCESSFUL."
        : "TIME EXPIRED. THE MAZE REMAINS UNCLEARED.";

    DrawPixelTextCentered(
        message,
        500,
        265,
        1,
        mutedText
    );

    // --------------------------------------------------
    // FINAL SCORE
    // --------------------------------------------------

    DrawPixelTextCentered(
        "FINAL SCORE",
        500,
        325,
        2,
        mutedText
    );

    DrawPixelTextCentered(
        TextFormat("%03i", game.score),
        500,
        355,
        5,
        PIXEL_CYAN
    );

    // --------------------------------------------------
    // CRYSTAL SUMMARY
    // --------------------------------------------------

    DrawRectangle(
        300, 425,
        400, 2,
        PIXEL_DARK_BLUE
    );

    DrawPixelTextCentered(
        "CRYSTALS RECOVERED",
        500,
        440,
        2,
        mutedText
    );

    DrawPixelTextCentered(
        TextFormat(
            "%i / %i",
            game.crystalsCollected,
            game.totalCrystals
        ),
        500,
        463,
        3,
        PIXEL_PURPLE
    );

    // --------------------------------------------------
    // MISSION STATUS
    // --------------------------------------------------

    DrawPixelTextCentered(
        victory ? "MISSION COMPLETE" : "MISSION FAILED",
        500,
        493,
        2,
        accent
    );

    // --------------------------------------------------
    // RESTART INSTRUCTIONS
    // --------------------------------------------------

    Color restartColor =
        pulse > 0.5f
            ? PIXEL_GREEN
            : PIXEL_CYAN;

    DrawPixelTextCentered(
        "PRESS R TO PLAY AGAIN",
        500,
        518,
        2,
        restartColor
    );

    DrawPixelTextCentered(
        "PRESS ESC TO EXIT",
        500,
        535,
        1,
        mutedText
    );
}

// --------------------------------------------------
// DRAW PIXEL-ART TITLE SCREEN
// --------------------------------------------------

void DrawTitleScreen()
{
    float time = (float)GetTime();

    float pulse =
        (sinf(time * 3.0f) + 1.0f) / 2.0f;

    ClearBackground(PIXEL_BLACK);

    // --------------------------------------------------
    // BACKGROUND GRID
    // --------------------------------------------------

    for (int i = 0; i < 9; i++)
    {
        int y = 75 + i * 65;

        DrawRectangle(
            100, y,
            800, 2,
            PIXEL_NAVY
        );
    }

    // Side decorations.
    DrawRectangle(
        105, 155,
        4, 350,
        PIXEL_BLUE
    );

    DrawRectangle(
        891, 155,
        4, 350,
        PIXEL_PURPLE
    );

    // Small pixel accents along the sides.
    for (int i = 0; i < 5; i++)
    {
        int y = 190 + i * 65;

        DrawRectangle(
            97, y,
            8, 8,
            PIXEL_CYAN
        );

        DrawRectangle(
            895, y,
            8, 8,
            PIXEL_MAGENTA
        );
    }

    // --------------------------------------------------
    // DECORATIVE CRYSTALS
    // --------------------------------------------------

    const char* crystalRows[] =
    {
        "...C....",
        "..CCC...",
        ".CCCCC..",
        "CCCCC...",
        ".CCCCC..",
        "..CCC...",
        "...C....",
        "........"
    };

    for (int i = 0; i < 5; i++)
    {
        int crystalX = 190 + i * 155;

        int crystalY =
            157 + (int)(sinf(time * 2.5f + i * 1.2f) * 4.0f);

        Color crystalColor =
            i % 2 == 0
                ? PIXEL_CYAN
                : PIXEL_PURPLE;

        PixelColor currentPalette[] =
        {
            {'C', crystalColor}
        };

        DrawPixelSprite(
            crystalRows,
            8, 8,
            crystalX,
            crystalY,
            3,
            currentPalette,
            1
        );

        // Small highlight.
        DrawRectangle(
            crystalX + 10,
            crystalY + 7,
            3, 3,
            PIXEL_WHITE
        );
    }

    // --------------------------------------------------
    // TOP ACCENT
    // --------------------------------------------------

    DrawRectangle(
        200, 205,
        600, 3,
        PIXEL_BLUE
    );

    // --------------------------------------------------
    // GAME TITLE
    // --------------------------------------------------

    DrawPixelTextCentered(
        "MAZE RUNNER",
        500,
        245,
        6,
        PIXEL_CYAN
    );

    // --------------------------------------------------
    // SUBTITLE
    // --------------------------------------------------

    DrawPixelTextCentered(
        "THE NEON ESCAPE",
        500,
        315,
        3,
        PIXEL_PURPLE
    );

    DrawRectangle(
        390, 365,
        220, 3,
        PIXEL_BLUE
    );

    // --------------------------------------------------
    // GAME OBJECTIVE
    // --------------------------------------------------

    DrawPixelTextCentered(
        "COLLECT ALL FIVE CRYSTALS",
        500,
        395,
        2,
        PIXEL_WHITE
    );

    DrawPixelTextCentered(
        "UNLOCK THE EXIT BEFORE TIME RUNS OUT",
        500,
        425,
        1,
        PIXEL_LIGHT_GRAY
    );

    // --------------------------------------------------
    // CONTROLS
    // --------------------------------------------------

    DrawPixelTextCentered(
        "CONTROLS",
        500,
        470,
        2,
        PIXEL_CYAN
    );

    DrawPixelTextCentered(
        "W A S D / ARROW KEYS",
        500,
        495,
        2,
        PIXEL_WHITE
    );

    DrawRectangle(
        200, 535,
        600, 3,
        PIXEL_PURPLE
    );

    // --------------------------------------------------
    // START INSTRUCTION
    // --------------------------------------------------

    Color startColor =
        pulse > 0.5f
            ? PIXEL_GREEN
            : PIXEL_CYAN;

    DrawPixelTextCentered(
        "PRESS ENTER TO START",
        500,
        565,
        3,
        startColor
    );

    DrawPixelTextCentered(
        "ESC TO QUIT",
        500,
        610,
        2,
        PIXEL_MID_GRAY
    );
}
