# Maze Runner – The Neon Escape

A 2D retro arcade maze game developed in C++ using the Raylib graphics library. The game features custom 8-bit pixel art, progressively challenging maze levels, enemy AI, animated visual effects, and an interactive boss encounter.

## About the Game

**Maze Runner – The Neon Escape** is a retro arcade maze game in which players navigate through increasingly challenging mazes, collect energy crystals, avoid pursuing enemies, and reach the exit to progress through the game.

The game features a consistent NES-inspired pixel-art palette, custom bitmap sprites, animated characters, particle effects, retro-style typography, and atmospheric background music.

Across ten levels, players must navigate different maze layouts, manage their remaining lives, and survive increasingly difficult encounters before facing the final boss.

## Features

- **10 progressively challenging levels** with individually designed maze layouts.
- **Custom 8-bit pixel art** featuring a shared limited colour palette, bitmap sprites, and pixel typography.
- **Crystal collection system** with five collectibles in each level.
- **Enemy AI** using Breadth-First Search (BFS) pathfinding to pursue the player through the maze.
- **Lives system** with three lives per level and temporary invulnerability after taking damage.
- **Scoring system** that awards 10 points for each collected crystal, with the score carrying forward between levels.
- **Animated visual effects**, including particle bursts, player trails, crystal collection effects, exit-unlock animations, and level-completion banners.
- **Pixel-art screen transitions** using an ordered-dithering effect.
- **Final boss encounter** featuring a moving crystal boss, five patrol positions, and two alternating projectile attack patterns.
- **Attack telegraphing** that gives players a visual warning before the boss attacks.
- **Background music** featuring an original retro electronic and atmospheric chiptune soundtrack.
- **Sound effects** for important gameplay events.
- **Resizable game window** that supports maximising while preserving the game's proportions and crisp pixel-art rendering.

## Controls

| Key | Action |
|---|---|
| W / Up Arrow | Move up |
| S / Down Arrow | Move down |
| A / Left Arrow | Move left |
| D / Right Arrow | Move right |
| Enter | Start the game |

Both WASD and arrow keys can be used for movement.

## Gameplay

1. Start the game from the title screen.
2. Navigate through the maze while avoiding walls and pursuing enemies.
3. Collect all five energy crystals in the current level.
4. Each collected crystal awards 10 points.
5. Once all five crystals are collected, the exit becomes unlocked.
6. Reach the exit to complete the level and advance.
7. Avoid enemy collisions to preserve your three lives.
8. Progress through all ten levels as the difficulty increases.

Your score carries forward as you progress through the game.

### Final Boss Encounter – Level 10

The tenth level introduces the **Neon Warden**, a moving crystal boss that patrols the central arena.

The encounter uses a survival-based objective:

- The boss moves between five designated positions in the central arena.
- The boss is invulnerable and cannot be defeated through attacks.
- An aimed volley fires five projectiles towards the player's position.
- A radial burst releases twelve projectiles in different directions.
- Visual warning animations announce attacks before they occur.
- The boss temporarily stops moving during attack warnings.
- Players must collect all five crystals while surviving the boss's attacks.
- The exit unlocks after all five crystals have been collected.

The player must reach the exit to complete the final level.

## Graphics and Visual Design

The game uses a strict 8-bit pixel-art approach rather than relying on a pixelation filter.

Its visual design incorporates:

- Custom pixel-art sprites for the player, enemies, boss, crystals, and projectiles.
- A shared NES-inspired limited colour palette.
- Bitmap-based typography for consistent retro presentation.
- Square pixel particles and animated effects.
- Nearest-neighbour texture filtering to preserve crisp pixel edges.
- Ordered dithering for pixel-art screen transitions.

The game uses a logical rendering resolution of 1000 × 700 pixels, with a resizable window and proportional scaling.

## Technologies Used

- **Programming Language:** C++
- **Graphics and Game Library:** Raylib
- **Compiler:** GCC (MSYS2 UCRT64)
- **Audio:** Raylib audio module
- **Development Environment:** Visual Studio Code
- **Build Environment:** Windows with MSYS2 UCRT64

## Project Structure

```text
MazeRunner/
│
├── assets/
│   └── audio/
│       └── background_music.ogg
│
├── src/
│   ├── main.cpp
│   ├── maze.cpp
│   ├── maze.h
│   ├── player.cpp
│   ├── player.h
│   ├── game.cpp
│   ├── game.h
│   ├── effects.cpp
│   ├── effects.h
│   ├── audio.cpp
│   ├── audio.h
│   ├── enemy.cpp
│   ├── enemy.h
│   ├── boss.cpp
│   ├── boss.h
│   ├── transition.cpp
│   ├── transition.h
│   ├── pixel_art.cpp
│   └── pixel_art.h
│
├── README.md
└── maze_runner.exe
```

## Requirements

To compile and run the project, you need:

- Windows operating system.
- GCC C++ compiler.
- Raylib library installed and configured.
- MSYS2 UCRT64 environment or an equivalent Raylib-compatible setup.

## Compilation

Open PowerShell or the VS Code terminal in the project directory and run:

```powershell
g++ -Wall -Wextra -Wpedantic src/main.cpp src/maze.cpp src/player.cpp src/game.cpp src/effects.cpp src/audio.cpp src/enemy.cpp src/boss.cpp src/transition.cpp src/pixel_art.cpp -o maze_runner.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

The compiler warning flags enable additional checks during compilation.

## Running the Game

After successful compilation, run:

```powershell
.\maze_runner.exe
```

Run the executable from the project root directory so that the game can locate its background music at:

```text
assets/audio/background_music.ogg
```

## Testing and Verification

The completed game underwent functional and visual regression testing.

The final regression covered:

- Title screen and controls.
- Player movement and maze collision.
- Crystal collection and scoring.
- Exit unlocking and level progression.
- Enemy movement and collision.
- Player lives and invulnerability.
- Boss movement, attack warnings, projectiles, and survival objective.
- Victory and defeat screens.
- Background music and sound effects.
- Window resizing and pixel-art scaling.
- Clean application shutdown.

The final full-game regression completed successfully, with **12 out of 12 test categories passing**.

The project also compiled successfully using `-Wall -Wextra -Wpedantic` without compiler warnings.

## Author

Developed as a C++ game development project.

---

**Maze Runner – The Neon Escape**

*Collect the crystals. Survive the maze. Find the exit.*