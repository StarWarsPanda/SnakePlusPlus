#pragma once

#include <graphx.h>
#include <sys/util.h>

#include "Vector2d.h"
#include "gfxutils.h"

#include "Snake.h"

class Food
{
    typedef enum
    {
        classic,
        winged
    } FoodType;

    public:
        Food();
        ~Food();

        bool IsInsideFood(Vector2D<int24_t> position) const;
        bool IsGolden() const;
        void Eat(const Snake* snake = nullptr);

        void Update();
        void Draw(gfx_sprite_t* food, gfx_sprite_t* goldenFood) const;

        const Vector2D<int24_t>& GetPosition() const;

    private:
        Vector2D<int24_t> m_position = Vector2D<int24_t>(0, 0);

        /* Direction in winged mode only; rotated by 45º clockwise */
        Direction         m_direction = Direction::Up;

        FoodType          m_foodType = FoodType::classic;
        bool              m_golden = false;
};