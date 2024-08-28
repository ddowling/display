#pragma once

#include <stdint.h>

// Only supporting monochrome bitmaps in this application.
// Pack 32 bits into a word. This means some padding but it will make the
// bitblt operations faster as memory reads and writes will be the full width
// and aligned.
struct Bitmap
{
    uint8_t width;         // width in pixels
    uint8_t height;        // height in pixels
    uint8_t wordWidth;     // words per scan line
    uint32_t *buffer;      // base Address
};


#if 0
// From BitBLT_Nov1975.pdf
struct Block
{
    uint32_t *bca; // Bitmap Base Core Address
    int bmw;       // Bitmap width in words
    int x_offset;  // Block left x
    int y_offset;  // Block top y
    int width;     // Block width
    int height;    // Block height
};

enum Function
{
    F_REPLACE = 0, // Destination <- Source
    F_PAINT = 4,   // Destination <- Source | Destination
    F_INVERT = 8,  // Destination <- Source ^ Destination
    F_ERASE = 12,  // Destination <- ~Source & Destination

    F_BLOCK = 0,   // A block of a bitmap
    F_COMPLEMENT = 1, // Complement of a block of a bitmap
    ...
};

fillBitMap(Block *destination, int function; BitMap source);
#endif
