#ifndef PIXEL_ART_H
#define PIXEL_ART_H

#include "raylib.h"

// --------------------------------------------------
// SHARED PIXEL ART PALETTE
// --------------------------------------------------

extern const Color PIXEL_BLACK;
extern const Color PIXEL_NAVY;
extern const Color PIXEL_DARK_BLUE;
extern const Color PIXEL_BLUE;
extern const Color PIXEL_CYAN;
extern const Color PIXEL_PURPLE;
extern const Color PIXEL_MAGENTA;
extern const Color PIXEL_GREEN;
extern const Color PIXEL_YELLOW;
extern const Color PIXEL_ORANGE;
extern const Color PIXEL_RED;
extern const Color PIXEL_WHITE;
extern const Color PIXEL_LIGHT_GRAY;
extern const Color PIXEL_MID_GRAY;
extern const Color PIXEL_DARK_GRAY;

// --------------------------------------------------
// PIXEL SPRITE COLOUR MAPPING
// --------------------------------------------------

struct PixelColor
{
    char symbol;
    Color color;
};

// --------------------------------------------------
// PIXEL SPRITE RENDERING
// --------------------------------------------------

void DrawPixelSprite(
    const char* const* rows,
    int width,
    int height,
    int x,
    int y,
    int pixelSize,
    const PixelColor* palette,
    int paletteCount
);

// --------------------------------------------------
// BITMAP TYPOGRAPHY
// --------------------------------------------------

void DrawPixelText(
    const char* text,
    int x,
    int y,
    int pixelSize,
    Color color
);

int MeasurePixelText(
    const char* text,
    int pixelSize
);

void DrawPixelTextCentered(
    const char* text,
    int centerX,
    int y,
    int pixelSize,
    Color color
);

#endif