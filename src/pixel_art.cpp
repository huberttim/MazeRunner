#include "pixel_art.h"

// --------------------------------------------------
// SHARED NES-INSPIRED PIXEL PALETTE
// --------------------------------------------------

const Color PIXEL_BLACK      = {8, 8, 16, 255};
const Color PIXEL_NAVY       = {24, 24, 64, 255};
const Color PIXEL_DARK_BLUE  = {24, 48, 96, 255};
const Color PIXEL_BLUE       = {48, 96, 192, 255};
const Color PIXEL_CYAN       = {80, 200, 232, 255};
const Color PIXEL_PURPLE     = {120, 56, 184, 255};
const Color PIXEL_MAGENTA    = {216, 80, 152, 255};
const Color PIXEL_GREEN      = {80, 200, 120, 255};
const Color PIXEL_YELLOW     = {248, 216, 88, 255};
const Color PIXEL_ORANGE     = {232, 152, 72, 255};
const Color PIXEL_RED        = {200, 56, 64, 255};
const Color PIXEL_WHITE      = {248, 248, 240, 255};
const Color PIXEL_LIGHT_GRAY = {184, 192, 200, 255};
const Color PIXEL_MID_GRAY   = {104, 112, 128, 255};
const Color PIXEL_DARK_GRAY  = {48, 48, 64, 255};

// --------------------------------------------------
// FIND A COLOUR IN THE SPRITE PALETTE
// --------------------------------------------------

static bool FindPixelColor(
    char symbol,
    const PixelColor* palette,
    int paletteCount,
    Color &result
)
{
    for (int i = 0; i < paletteCount; i++)
    {
        if (palette[i].symbol == symbol)
        {
            result = palette[i].color;
            return true;
        }
    }

    return false;
}

// --------------------------------------------------
// DRAW PIXEL SPRITE
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
)
{
    if (
        rows == nullptr ||
        palette == nullptr ||
        width <= 0 ||
        height <= 0 ||
        pixelSize <= 0 ||
        paletteCount <= 0
    )
    {
        return;
    }

    for (int row = 0; row < height; row++)
    {
        if (rows[row] == nullptr)
            continue;

        int col = 0;

        while (col < width)
        {
            char symbol = rows[row][col];

            if (symbol == '.')
            {
                col++;
                continue;
            }

            Color pixelColor;

            if (!FindPixelColor(
                symbol,
                palette,
                paletteCount,
                pixelColor
            ))
            {
                col++;
                continue;
            }

            int runLength = 1;

            while (
                col + runLength < width &&
                rows[row][col + runLength] == symbol
            )
            {
                runLength++;
            }

            DrawRectangle(
                x + col * pixelSize,
                y + row * pixelSize,
                runLength * pixelSize,
                pixelSize,
                pixelColor
            );

            col += runLength;
        }
    }
}

// --------------------------------------------------
// 5 x 7 PIXEL FONT
// --------------------------------------------------

struct PixelGlyph
{
    char character;
    const char* rows[7];
};

static const PixelGlyph PIXEL_FONT[] =
{
    {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
    {'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
    {'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
    {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
    {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
    {'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
    {'G', {"01111", "10000", "10000", "10111", "10001", "10001", "01111"}},
    {'H', {"10001", "10001", "10001", "11111", "10001", "10001", "10001"}},
    {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
    {'J', {"00111", "00010", "00010", "00010", "10010", "10010", "01100"}},
    {'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
    {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
    {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
    {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
    {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
    {'P', {"11110", "10001", "10001", "11110", "10000", "10000", "10000"}},
    {'Q', {"01110", "10001", "10001", "10001", "10101", "10010", "01101"}},
    {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
    {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
    {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
    {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
    {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
    {'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
    {'X', {"10001", "10001", "01010", "00100", "01010", "10001", "10001"}},
    {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
    {'Z', {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}},

    {'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
    {'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
    {'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
    {'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
    {'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
    {'5', {"11111", "10000", "10000", "11110", "00001", "00001", "11110"}},
    {'6', {"01110", "10000", "10000", "11110", "10001", "10001", "01110"}},
    {'7', {"11111", "00001", "00010", "00100", "01000", "01000", "01000"}},
    {'8', {"01110", "10001", "10001", "01110", "10001", "10001", "01110"}},
    {'9', {"01110", "10001", "10001", "01111", "00001", "00001", "01110"}},

    {':', {"00000", "00100", "00100", "00000", "00100", "00100", "00000"}},
    {'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}},
    {'.', {"00000", "00000", "00000", "00000", "00000", "00110", "00110"}},
    {'/', {"00001", "00010", "00010", "00100", "01000", "01000", "10000"}},
    {'!', {"00100", "00100", "00100", "00100", "00100", "00000", "00100"}},
    {'?', {"01110", "10001", "00001", "00010", "00100", "00000", "00100"}},
    {'+', {"00000", "00100", "00100", "11111", "00100", "00100", "00000"}},
    {'%', {"11001", "11010", "00100", "01000", "10110", "00110", "00000"}},
    {'\'', {"00100", "00100", "00000", "00000", "00000", "00000", "00000"}}
};

static const int PIXEL_FONT_COUNT =
    sizeof(PIXEL_FONT) / sizeof(PIXEL_FONT[0]);

// --------------------------------------------------
// FIND A FONT GLYPH
// --------------------------------------------------

static const PixelGlyph* FindPixelGlyph(char character)
{
    if (character >= 'a' && character <= 'z')
    {
        character =
            static_cast<char>(character - 'a' + 'A');
    }

    for (int i = 0; i < PIXEL_FONT_COUNT; i++)
    {
        if (PIXEL_FONT[i].character == character)
            return &PIXEL_FONT[i];
    }

    return nullptr;
}

// --------------------------------------------------
// MEASURE PIXEL TEXT
// --------------------------------------------------

int MeasurePixelText(
    const char* text,
    int pixelSize
)
{
    if (text == nullptr || pixelSize <= 0)
        return 0;

    int characterCount = 0;

    while (text[characterCount] != '\0')
        characterCount++;

    if (characterCount == 0)
        return 0;

    return (characterCount * 6 - 1) * pixelSize;
}

// --------------------------------------------------
// DRAW PIXEL TEXT
// --------------------------------------------------

void DrawPixelText(
    const char* text,
    int x,
    int y,
    int pixelSize,
    Color color
)
{
    if (text == nullptr || pixelSize <= 0)
        return;

    int cursorX = x;

    for (int i = 0; text[i] != '\0'; i++)
    {
        const PixelGlyph* glyph =
            FindPixelGlyph(text[i]);

        if (glyph != nullptr)
        {
            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    if (glyph->rows[row][col] == '1')
                    {
                        DrawRectangle(
                            cursorX + col * pixelSize,
                            y + row * pixelSize,
                            pixelSize,
                            pixelSize,
                            color
                        );
                    }
                }
            }
        }

        cursorX += 6 * pixelSize;
    }
}

// --------------------------------------------------
// DRAW CENTRED PIXEL TEXT
// --------------------------------------------------

void DrawPixelTextCentered(
    const char* text,
    int centerX,
    int y,
    int pixelSize,
    Color color
)
{
    int textWidth =
        MeasurePixelText(text, pixelSize);

    DrawPixelText(
        text,
        centerX - textWidth / 2,
        y,
        pixelSize,
        color
    );
}