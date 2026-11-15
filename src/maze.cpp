
#include <cmath>
#include <cstring>
#include <queue>
#include <utility>

#include "maze.h"
#include "pixel_art.h"

// --------------------------------------------------
// MAZE LEVEL LAYOUTS
// --------------------------------------------------

const char* LEVEL_LAYOUTS[10][ROWS] =
{
    // LEVEL 1: OPEN COURTYARD
    {
        "###############",
        "#S..........C.#",
        "#..##.###.##..#",
        "#..##.....##.C#",
        "#.............#",
        "#C............#",
        "#..##.###.##..#",
        "#..##.....C#..#",
        "#.............#",
        "#.C.........E.#",
        "###############"
    },

    // LEVEL 2: THE SNAKE
    {
        "###############",
        "#S.....C......#",
        "#.##.#####.##.#",
        "#.C...........#",
        "#.#####.###.#.#",
        "#...........C.#",
        "#.#.####.####.#",
        "#.....C.......#",
        "#.###.####.#..#",
        "#..C........E.#",
        "###############"
    },

    // LEVEL 3: DUAL CHAMBERS
    {
        "###############",
        "#S..#.C.......#",
        "#........#..C.#",
        "#...#....#....#",
        "#.C.#.........#",
        "#...######....#",
        "#........#....#",
        "#...#.C..#....#",
        "#...#.........#",
        "#........#C.E.#",
        "###############"
    },

    // LEVEL 4: THE NEST
    {
        "###############",
        "#S......C.....#",
        "#.####.####.#.#",
        "#.#C........#.#",
        "#.#.#.###.#.#.#",
        "#....C......#.#",
        "#.#.###.##....#",
        "#.#........C#.#",
        "#.#.#####.###.#",
        "#....C......E.#",
        "###############"
    },

    // LEVEL 5: CHECKERED MATRIX
    {
        "###############",
        "#S...C........#",
        "#..##.##.##.#.#",
        "#...........C.#",
        "#.##.##.##.##.#",
        "#.C...........#",
        "#.#.##.##.##..#",
        "#.......C.....#",
        "#..##.##.##.#.#",
        "#.........C.E.#",
        "###############"
    },

    // LEVEL 6: THE HOURGLASS
    {
        "###############",
        "#S.....C......#",
        "#..#.##.##....#",
        "#..#........C.#",
        "#.C#..........#",
        "#.##.#####.##.#",
        "#......C...#..#",
        "#..........#..#",
        "#....##.##.#..#",
        "#....C......E.#",
        "###############"
    },

    // LEVEL 7: POCKET MAZE
    {
        "###############",
        "#S.......C....#",
        "#.##.##..##.#.#",
        "#.C...........#",
        "#..##.###.##..#",
        "#..........C..#",
        "#.####.#......#",
        "#.....C..#.##.#",
        "#..###........#",
        "#..C........E.#",
        "###############"
    },

    // LEVEL 8: DIAGONAL SWEEP
    {
        "###############",
        "#S......C..#..#",
        "#..##......#..#",
        "#.#.##......C.#",
        "#.#..##....#..#",
        "#..C..##...#..#",
        "#.#....##.....#",
        "#.#.....##.C..#",
        "#........##...#",
        "#....C......E.#",
        "###############"
    },

    // LEVEL 9: RING ARENA
    {
        "###############",
        "#S...C....C...#",
        "#..####.####..#",
        "#..#.......#..#",
        "#..#.##.##.#..#",
        "#.C........#..#",
        "#..#.##.##....#",
        "#..#....C..#..#",
        "#..####.####..#",
        "#....C......E.#",
        "###############"
    },

    // LEVEL 10: THE SENTINEL'S CORE
    {
        "###############",
        "#S..C#...C....#",
        "#.##.#.###.##.#",
        "#....#...#....#",
        "#...#.....#...#",
        "#....#.....#..#",
        "#.##.......##.#",
        "#...#...#C...C#",
        "#.###.#.###.#.#",
        "#C....#......E#",
        "###############"
    }
};

// --------------------------------------------------
// ACTIVE MAZE
// --------------------------------------------------

int maze[ROWS][COLS] = {0};

// --------------------------------------------------
// VALIDATE A MAZE LAYOUT
// --------------------------------------------------

bool ValidateMazeLevel(int level)
{
    if (level < 1 || level > 10)
        return false;

    const char** layout = LEVEL_LAYOUTS[level - 1];

    int crystalCount = 0;
    int startCount = 0;
    int exitCount = 0;

    int startRow = -1;
    int startCol = -1;

    // Check row dimensions and tile counts.
    for (int row = 0; row < ROWS; row++)
    {
        if (strlen(layout[row]) != COLS)
            return false;

        for (int col = 0; col < COLS; col++)
        {
            char tile = layout[row][col];

            if (
                tile != '#' &&
                tile != '.' &&
                tile != 'C' &&
                tile != 'S' &&
                tile != 'E'
            )
            {
                return false;
            }

            if (tile == 'C')
                crystalCount++;

            if (tile == 'S')
            {
                startCount++;
                startRow = row;
                startCol = col;
            }

            if (tile == 'E')
                exitCount++;
        }
    }

    // Every layout must have five crystals,
    // one start and one exit.
    if (
        crystalCount != 5 ||
        startCount != 1 ||
        exitCount != 1
    )
    {
        return false;
    }

    // Check the existing enemy spawn location.
    if (layout[9][13] != '.')
        return false;

    // ----------------------------------------------
    // CHECK REACHABILITY USING BREADTH-FIRST SEARCH
    // ----------------------------------------------

    bool visited[ROWS][COLS] = {};

    std::queue<std::pair<int, int>> searchQueue;

    searchQueue.push({startRow, startCol});

    visited[startRow][startCol] = true;

    const int rowChanges[4] = {-1, 1, 0, 0};
    const int colChanges[4] = {0, 0, -1, 1};

    while (!searchQueue.empty())
    {
        std::pair<int, int> current = searchQueue.front();

        searchQueue.pop();

        int currentRow = current.first;
        int currentCol = current.second;

        for (int i = 0; i < 4; i++)
        {
            int newRow = currentRow + rowChanges[i];
            int newCol = currentCol + colChanges[i];

            if (
                newRow < 0 || newRow >= ROWS ||
                newCol < 0 || newCol >= COLS
            )
            {
                continue;
            }

            if (visited[newRow][newCol])
                continue;

            if (layout[newRow][newCol] == '#')
                continue;

            visited[newRow][newCol] = true;

            searchQueue.push({newRow, newCol});
        }
    }

    // Every crystal, the exit and enemy spawn
    // must be reachable.
    int reachableCrystals = 0;

    int exitRow = -1;
    int exitCol = -1;

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (
                layout[row][col] == 'C' &&
                visited[row][col]
            )
            {
                reachableCrystals++;
            }

            if (layout[row][col] == 'E')
            {
                exitRow = row;
                exitCol = col;
            }
        }
    }

    if (reachableCrystals != 5)
        return false;

    if (!visited[exitRow][exitCol])
        return false;

    if (!visited[9][13])
        return false;

    return true;
}

// --------------------------------------------------
// LOAD A MAZE LAYOUT
// --------------------------------------------------

void SetMazeLevel(int level)
{
    if (level < 1 || level > 10)
        level = 1;

    // Never load an invalid layout.
    if (!ValidateMazeLevel(level))
    {
        level = 1;
    }

    const int selectedLayout = level - 1;

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            char tile =
                LEVEL_LAYOUTS[selectedLayout][row][col];

            switch (tile)
            {
                case '#':
                    maze[row][col] = WALL;
                    break;

                case 'C':
                    maze[row][col] = COLLECTIBLE;
                    break;

                case 'E':
                    maze[row][col] = EXIT;
                    break;

                case 'S':
                    maze[row][col] = PLAYER_START;
                    break;

                default:
                    maze[row][col] = PATH;
                    break;
            }
        }
    }
}

// --------------------------------------------------
// RESET TO LEVEL 1
// --------------------------------------------------

void ResetMaze()
{
    SetMazeLevel(1);
}

// --------------------------------------------------
// CHECK WHETHER A NEIGHBOUR IS A WALL
// --------------------------------------------------

bool IsWall(int row, int col)
{
    if (
        row < 0 || row >= ROWS ||
        col < 0 || col >= COLS
    )
    {
        return false;
    }

    return maze[row][col] == WALL;
}

// --------------------------------------------------
// DRAW AN INDIVIDUAL MAZE TILE
// --------------------------------------------------

void DrawMazeTile(
    int row,
    int col,
    int tileType,
    bool exitUnlocked,
    float animationTime
)
{
    float x = MAZE_OFFSET_X + col * TILE_SIZE;
    float y = MAZE_OFFSET_Y + row * TILE_SIZE;

    Vector2 center =
    {
        x + TILE_SIZE / 2.0f,
        y + TILE_SIZE / 2.0f
    };

    switch (tileType)
    {
        // ------------------------------------------
        // WALL
        // ------------------------------------------

        case WALL:
        {
            static const char* wallSprite[8] =
            {
                "..OOOO..",
                ".OBBBBO.",
                "OBHHHHBO",
                "OBBBBBBO",
                "OBBDDBBO",
                "OBBDDBBO",
                ".OBBBBO.",
                "..OOOO.."
            };

            static const PixelColor wallPalette[] =
            {
                {'O', PIXEL_CYAN},
                {'B', PIXEL_DARK_BLUE},
                {'H', PIXEL_BLUE},
                {'D', PIXEL_NAVY}
            };

            DrawPixelSprite(
                wallSprite,
                8,
                8,
                (int)x + 1,
                (int)y + 1,
                6,
                wallPalette,
                4
            );

            break;
        }

        // ------------------------------------------
        // PATH AND PLAYER START
        // ------------------------------------------

        case PATH:
        case PLAYER_START:
        {
            static const char* floorSprite[8] =
            {
                "........",
                ".F....F.",
                "..P..P..",
                "........",
                "........",
                "..P..P..",
                ".F....F.",
                "........"
            };

            static const PixelColor floorPalette[] =
            {
                {'F', PIXEL_NAVY},
                {'P', PIXEL_DARK_BLUE}
            };

            DrawPixelSprite(
                floorSprite,
                8,
                8,
                (int)x + 1,
                (int)y + 1,
                6,
                floorPalette,
                2
            );

            break;
        }

        // ------------------------------------------
        // COLLECTIBLE CRYSTAL
        // ------------------------------------------

        case COLLECTIBLE:
        {
            float pulse =
                (sinf(animationTime * 4.0f) + 1.0f) / 2.0f;

            static const char* crystalFrameA[8] =
            {
                "...W....",
                "..WP....",
                ".WPP....",
                "WPPP....",
                ".WPP....",
                "..WP....",
                "...W....",
                "........"
            };

            static const char* crystalFrameB[8] =
            {
                "........",
                "...W....",
                "..WP....",
                ".WPP....",
                "WPPP....",
                ".WPP....",
                "..WP....",
                "...W...."
            };

            static const PixelColor crystalPalette[] =
            {
                {'W', PIXEL_WHITE},
                {'P', PIXEL_PURPLE}
            };

            const char* const* selectedFrame =
                pulse > 0.5f ? crystalFrameA : crystalFrameB;

            DrawPixelSprite(
                selectedFrame,
                8,
                8,
                (int)x + 9,
                (int)y + 9,
                4,
                crystalPalette,
                2
            );

            break;
        }

        // ------------------------------------------
        // EXIT PORTAL
        // ------------------------------------------

        case EXIT:
        {
            float pulse =
                (sinf(animationTime * 3.0f) + 1.0f) / 2.0f;

            Color portalColor =
                exitUnlocked ? PIXEL_GREEN : PIXEL_RED;

            static const char* portalFrameA[8] =
            {
                "..PPPP..",
                ".P....P.",
                "P..DD..P",
                "P..DD..P",
                "P..DD..P",
                "P..DD..P",
                ".P....P.",
                "..PPPP.."
            };

            static const char* portalFrameB[8] =
            {
                "...PP...",
                "..P..P..",
                ".P.DD.P.",
                "P..DD..P",
                "P..DD..P",
                ".P.DD.P.",
                "..P..P..",
                "...PP..."
            };

            PixelColor portalPalette[] =
            {
                {'P', portalColor},
                {'D', PIXEL_DARK_BLUE}
            };

            const char* const* selectedFrame =
                pulse > 0.5f ? portalFrameA : portalFrameB;

            DrawPixelSprite(
                selectedFrame,
                8,
                8,
                (int)x + 9,
                (int)y + 9,
                4,
                portalPalette,
                2
            );

            // Central indicator communicates portal state.
            Color coreColor =
                exitUnlocked ? PIXEL_WHITE : PIXEL_ORANGE;

            DrawRectangle(
                (int)center.x - 2,
                (int)center.y - 2,
                4,
                4,
                coreColor
            );

            break;
        }
    }
}

// --------------------------------------------------
// DRAW THE COMPLETE MAZE
// --------------------------------------------------

void DrawMaze(bool exitUnlocked, float animationTime)
{
    DrawRectangle(
        MAZE_OFFSET_X,
        MAZE_OFFSET_Y,
        COLS * TILE_SIZE,
        ROWS * TILE_SIZE,
        PIXEL_BLACK
    );

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            DrawMazeTile(
                row,
                col,
                maze[row][col],
                exitUnlocked,
                animationTime
            );
        }
    }
}
