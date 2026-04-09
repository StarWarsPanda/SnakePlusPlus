#pragma once

#include <graphx.h>
#include <string.h>

#include "Vector2d.h"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 10
#endif

#ifdef WIN32
#define int24_t int32_t
#define uint24_t uint32_t
#endif

#define gfx_ResetClipRegion() gfx_SetClipRegion(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT)

#define gfx_SetPaletteColor(color) const uint8_t gfx_BeforeColor = gfx_SetColor(color)
#define gfx_ResetColor() gfx_SetColor(gfx_BeforeColor)

typedef enum 
{
    Up = 0,
    Left = 1,
    Down = 2,
    Right = 3,
    Size
}Direction;

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