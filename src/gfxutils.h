#pragma once

#include <graphx.h>
#include <string.h>

#include "Vector2d.h"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 10
#endif

#ifdef MSVC
#define int24_t int32_t
#define uint24_t uint32_t
#endif

#define IncrementWrap(x, min, max, inc) do { if ((x) > ((max) - (inc))){ (x) = (min); }else{ (x) += (inc);}} while (0)
#define DecrementWrap(x, min, max, dec) do { if ((x) < ((min) + (dec))){ (x) = (max); }else{ (x) -= (dec);}} while (0)

#define gfx_ResetClipRegion() gfx_SetClipRegion(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT)

#define gfx_SetPaletteColor(color) const uint8_t gfx_BeforeColor = gfx_SetColor(color)
#define gfx_ResetColor() gfx_SetColor(gfx_BeforeColor)

#define gfx_1555ToRGB(c, out_r, out_g, out_b) do { (out_r) = (uint8_t)((((c) >> 10) & 0x1F) << 3); (out_g) = (uint8_t)((((c) >>  5) & 0x1F) << 3); (out_b) = (uint8_t)(((c) & 0x1F) << 3); } while (0)

#define gfx_CheckRectanglePoint(x0, y0, rx, ry, rw, rh) (((x0) >= (rx)) && ((x0) < ((rx) + (rw))) && ((y0) >= (ry)) && ((y0) < ((ry) + (rh))))

typedef enum 
{
    Up = 0,
    Left = 1,
    Down = 2,
    Right = 3,
    Size
}Direction;

template <typename T>
class Rectangle
{
    public:
        Vector2D<T> position;
        Vector2D<T> size;
};

#define IS_VERTICAL(direction) (((direction) & 1) == 0)
#define IS_HORIZONTAL(direction) (((direction) & 1) == 1)
#define COMBINE(dir_a, dir_b) ((dir_a) + (Direction::Size * (dir_b)))

static inline const char* DirectionAsString(Direction direction)
{
    switch (direction)
    {
        case Up   : return "Up";
        case Down : return "Down";
        case Left : return "Left";
        case Right: return "Right";
        default   : return "Unknown";
    }
}

static inline uint8_t gfx_GetFontHeight()
{
    const uint8_t height = gfx_SetFontHeight(0); gfx_SetFontHeight(height);
    return height;
}

static inline void gfx_PrintStringXYCharWrap(const char* string, int x, int y, uint24_t max_width, uint8_t padding)
{
    gfx_SetTextXY(x, y);

    const uint8_t charHeight = gfx_GetFontHeight();

    for (size_t i = 0; string[i] != '\0'; i++)
    {
        const uint24_t charWidth = gfx_GetCharWidth(string[i]);

        if (gfx_GetTextX() + charWidth > max_width)
        {
            gfx_SetTextXY(x, gfx_GetTextY() + charHeight + padding);
        }

        gfx_PrintChar(string[i]);
    }
}

static inline void gfx_Arrow(Direction direction, Vector2D<int24_t> position, uint8_t length, uint8_t color)
{
    const Vector2D<int24_t> centered = position + Vector2D<int24_t>(BLOCK_SIZE / 2, BLOCK_SIZE / 2);

    const uint8_t previousColor = gfx_SetColor(color);

    switch (direction)
    {
        case Up:
            gfx_Line(centered.x, centered.y, centered.x, centered.y - (BLOCK_SIZE + length));
            break;
        case Down:
            gfx_Line(centered.x, centered.y, centered.x, centered.y + (BLOCK_SIZE + length));
            break;
        case Right:
            gfx_Line(centered.x, centered.y, centered.x + (BLOCK_SIZE + length), centered.y);
            break;
        case Left:
            gfx_Line(centered.x, centered.y, centered.x - (BLOCK_SIZE + length), centered.y);
            break;
        case Size:
        default:
            break;
    }

    gfx_SetColor(previousColor);
}

static gfx_sprite_t* _gfx_tsp(const gfx_sprite_t* sprite, int offset_x, int offset_y, gfx_sprite_t* (*transform_a)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transform_b)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr)
{
    static gfx_UninitedSprite(_gfx_tsp_tile_buffer, BLOCK_SIZE, BLOCK_SIZE);
    static gfx_UninitedSprite(_gfx_tsp_transform_buffer_a, BLOCK_SIZE, BLOCK_SIZE);
    static gfx_UninitedSprite(_gfx_tsp_transform_buffer_b, BLOCK_SIZE, BLOCK_SIZE);

    _gfx_tsp_tile_buffer->width = BLOCK_SIZE;
    _gfx_tsp_tile_buffer->height = BLOCK_SIZE;

    for (uint8_t row = 0; row < BLOCK_SIZE; row++)
    {
        memcpy(
            _gfx_tsp_tile_buffer->data + row * BLOCK_SIZE,
            sprite->data + (offset_y * BLOCK_SIZE + row) * sprite->width + offset_x * BLOCK_SIZE,
            BLOCK_SIZE
        );
    }

    gfx_sprite_t* t_sprite = transform_a != nullptr ? transform_a(_gfx_tsp_tile_buffer, _gfx_tsp_transform_buffer_a) : _gfx_tsp_tile_buffer;
                  t_sprite = transform_b != nullptr ? transform_b(t_sprite, _gfx_tsp_transform_buffer_b) : t_sprite;

    return t_sprite;
}

static inline void gfx_TransparentSpritePartial(const gfx_sprite_t* sprite, int x, int y, int offset_x, int offset_y, gfx_sprite_t* (*transform_a)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transform_b)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr)
{
    gfx_TransparentSprite(_gfx_tsp(sprite, offset_x, offset_y, transform_a, transform_b), x, y);
}

static inline void gfx_TransparentSpritePartial_NoClip(const gfx_sprite_t* sprite, int x, int y, int offset_x, int offset_y, gfx_sprite_t* (*transform_a)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transform_b)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr)
{
    gfx_TransparentSprite_NoClip(_gfx_tsp(sprite, offset_x, offset_y, transform_a, transform_b), x, y);
}

static inline void gfx_SpritePartial(const gfx_sprite_t* sprite, int x, int y, int offset_x, int offset_y, gfx_sprite_t* (*transform_a)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transform_b)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr)
{
    gfx_Sprite(_gfx_tsp(sprite, offset_x, offset_y, transform_a, transform_b), x, y);
}

static inline void gfx_SpritePartial_NoClip(const gfx_sprite_t* sprite, int x, int y, int offset_x, int offset_y, gfx_sprite_t* (*transform_a)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transform_b)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr)
{
    gfx_Sprite_NoClip(_gfx_tsp(sprite, offset_x, offset_y, transform_a, transform_b), x, y);
}