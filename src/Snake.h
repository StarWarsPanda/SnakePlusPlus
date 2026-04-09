#pragma once

#include <Keys.h>
#include <graphx.h>
#include <debug.h>

#include "Vector2d.h"
#include "gfxutils.h"

#define BLOCK_SIZE 10
#define MAX_SNAKE_SEGMENTS ((LCD_WIDTH * LCD_HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE) - 2)

class Snake
{
    public:
        struct SnakeSegment
        {
            SnakeSegment(Vector2D<int24_t> position, Direction direction);
            SnakeSegment();

            Vector2D<int24_t> position = Vector2D<int24_t>(0, 0);
            Direction direction = Direction::Up;
        };

        enum SnakeSkin
        {
            basic,
            cool,
            coral,
            skeleton,
            steel,
            gold,
            size
        };

        enum SnakeType
        {
            classic,
            wrap
        };

    public:
        Snake(Vector2D<int24_t> startPosition, Direction startDirection, uint8_t defaultSize);
        ~Snake();

        void Update();
        void ChangeDirection(Direction newDirection);
        void AddLength();

        void Draw(gfx_sprite_t* snakeTiles, const Vector2D<int24_t>& foodPosition, bool isPartial = false) const;
        bool CheckCollision(const Vector2D<int24_t>* other = nullptr) const;

        Vector2D<int24_t> GetHeadPosition() const;
        Vector2D<int24_t> GetTailPosition() const;

        Direction GetHeadDirection() const;
        Direction GetTailDirection() const;

        uint24_t GetScore() const;

    private:
        static inline void DrawTile(gfx_sprite_t* snakeTiles, Vector2D<int24_t> position, Vector2D<uint8_t> tile, gfx_sprite_t* (*transformA)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr, gfx_sprite_t* (*transformB)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict) = nullptr);
        static void DrawStraightSegments(gfx_sprite_t* snakeTiles, Vector2D<int24_t> start, Vector2D<int24_t> end, Direction lineDirection);

    private:
        static SnakeSegment m_segments[MAX_SNAKE_SEGMENTS];

        SnakeSegment m_head;
        SnakeSegment m_tail, m_previousTail;

        uint24_t m_segmentLength = 0;
        uint24_t m_snakeSize;
        uint24_t m_defaultSnakeSize;

        bool m_isAddingLength = false;

        SnakeType m_snakeType = SnakeType::classic;
};