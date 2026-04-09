#pragma once

#include <Keys.h>
#include <graphx.h>
#include <sys/rtc.h>

#include "Snake.h"
#include "Food.h"

#include "gfx/gfx.h"

gfx_sprite_t* const snake_skins[6] = {
	snake_Basic_Tileset,
	snake_Cool_Tileset,
	snake_Coral_Tileset,
	snake_Skeleton_Tileset,
	snake_Steel_Tileset,
	snake_Gold_Tileset
};

class Game
{
    struct Key
    {
        uint8_t keyRow = 0;
        uint8_t keyIndex = 0;
        bool key = false;
        bool previousKey = false;
        bool isBuffered = false;

        void Update();

        bool IsUp();
        bool IsDown();
        bool OnRisingEdge();
        bool OnFallingEdge();
    };

    typedef enum
    {
        main,
        gameSelect,
        arcade,
        story,
        game,
        store,
        stats,
        options
    } Stage;

    typedef enum
    {
        up,
        left,
        right,
        down,
        enter,
        exit,
        size
    } KeyMap;

    public:
        Game();
        ~Game();

        void Run();

    private:
        void Update();
        void Draw();

    private:
        Key m_keys[6] = {
            { 7, kb_Up,    false, false, true },
            { 7, kb_Left,  false, false, true },
            { 7, kb_Right, false, false, true },
            { 7, kb_Down,  false, false, true },
            { 6, kb_Enter, false, false, true },
            { 6, kb_Clear, false, false, true }
        };

        Snake::SnakeSkin m_selectedSnake = Snake::SnakeSkin::basic;

        Snake m_snake = Snake(
            Vector2D<int32_t>(160, 120),
            Direction::Left,
            7
        );

        Food m_food;

        bool     m_exit = false;
        bool     m_isFirst = true;
        uint32_t m_frame = 0;

        uint24_t m_previousScore = 0;
};