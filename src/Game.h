#pragma once

#include <Keys.h>
#include <graphx.h>
#include <sys/rtc.h>
#include <fileioc.h>

#include "Snake.h"
#include "Food.h"

#include "gfx/snkspr.h"
#include "data/snippets.h"

#define style_strings_length 13

static const char* const style_strings[style_strings_length] = {
    "Background Color",
    "Panel Color",
    "Border Color",
    "Text Color",
    "Disabled Text Color",
    "Highlight Color",
    "Button Background Color",
    "Hovered Button Color",
    "Active Button Color",
    "Item Padding",
    "Alignment",
    "Border Size",
    "Corner Radius"
};

#define alignment_strings_length 3

static const char* const alignment_strings[alignment_strings_length] = {
    "Left",
    "Center",
    "Right"
};

#define GUI_MAX_LABEL_LENGTH 64
#define GUI_MAX_PANELS 8

class GUI
{
    public:
        typedef enum
        {
            left,
            center,
            right
        } Align;

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

        struct NavKeys
        {
            Key up;
            Key left;
            Key right;
            Key down;
            Key enter;
            Key back;
            Key exit;

            void Update();
        };

        struct Style
        {
            uint8_t  colorBackground;
            uint8_t  colorPanel;
            uint8_t  colorBorder;
            uint8_t  colorText;
            uint8_t  colorTextDisabled;
            uint8_t  colorHighlight;
            uint8_t  colorButton;
            uint8_t  colorButtonHovered;
            uint8_t  colorButtonActive;
            uint24_t itemPadding = 0;
            Align    alignment = left;
            uint8_t  borderSize = 0;
            uint8_t  cornerRadius = 0;
        };
    private:
        struct Panel
        {
            int  x;
            int  y;
            int  width;
            int  height;
            int  cursorX;
            int  cursorY;
            bool active;
        };

        struct FocusState
        {
            uint8_t hotId;
            uint8_t activeId;
            int8_t focusedId;
            uint8_t nextId;
            uint8_t maxId;
        };

        typedef enum
        {
            interaction_none,
            interaction_hovered,
            interaction_pressed,
            interaction_held,
            interaction_released
        } InteractionState;
    public:
        void Begin();
        void End();

        void BeginPanel(int x, int y, int width, int height);
        void EndPanel();

        bool Button(int x, int y, int width, int height, const char* label = nullptr);
        bool ButtonAt(int width, int height, const char* label = nullptr);
        void Label(const char* text, int x, int y,Align align = left);
        void LabelAt(const char* text, Align align = left);
        void Separator(int x, int y, int width);
        void SeparatorAt(int width);
        static void DrawRect(int x, int y, int w, int h, uint8_t fillColor, uint8_t borderColor);
        static void DrawRoundRect(int x, int y, int w, int h, uint8_t fillColor, uint8_t borderColor, uint8_t radius);

        void   SetFocusId(int id);
        int    GetFocusedId();

        void Lock();
        void Unlock();
    public:
        Style   style;
        NavKeys navKeys;
    private:
        void DrawText(const char* text, int x, int y, uint8_t color, Align align);

        InteractionState QueryInteraction(int id);
        uint24_t AllocId();
        void AdvanceCursor(int itemWidth, int itemHeight);
    private:
        FocusState m_focus = { 0, 0, 0, 1, 0 };
        Panel m_panelStack[8];
        Panel* m_currentPanel = nullptr;
        uint8_t m_panelDepth = 0;
        bool m_lock = false;
};

class Game
{
    typedef enum
    {
        main,
        game_select,
        arcade,
        story,
        game,
        store,
        options,
        ui_change,
        stats,
        pause
    } Stage;

    struct SnakeSkinCollectable
    {
        bool locked = true;
        uint24_t cost;
    };

    struct FileFormat
    {
        GUI::Style style;
        uint24_t goldenApples;
        uint24_t highScore; /* Classic high score */
        uint16_t palette[256];
        Snake::SnakeSkin selectedSnake;
        SnakeSkinCollectable unlockedSnakes[8];
        uint24_t highScores[2]; /* Wrap high score, Winged high score */
        uint24_t applesEaten;
        uint24_t goldenApplesEaten;
    };

    public:
        Game();
        ~Game();

        void Run();
    private:
        void StageMain();
        void StageGameSelect();
        void StageArcade();
        void StageStory();
        void StageGame();
        void StageStore();
        void StageOptions();
        void StageUIChange();
        void StageStats();
        void StageUnknown();
        void StagePause();

        void Load();
        void Save();

    private:
        Snake::SnakeSkin m_selectedSnake = Snake::SnakeSkin::basic;

        Snake m_snake = Snake(
            Vector2D<int32_t>(160, 120),
            Direction::Left,
            7,
            this->m_frame
        );

        Food m_food;

        SnakeSkinCollectable m_collectables[Snake::SnakeSkin::size] = {
            {
                // basic,
                false,
                0
            },
            {
                // cool,
                true,
                5
            },
            {
                // coral,
                true,
                10
            },
            {
                // skeleton,
                true,
                25
            },
            {
                // steel,
                true,
                25
            },
            {
                // donut
                true,
                50
            },               
            {
                // gold,
                true,
                100
            }
        };

        bool     m_exit = false;
        bool     m_isFirst = true;
        bool     m_isSecond = false;
        uint24_t m_frame = 0;
        uint24_t m_golden = 0;
        uint24_t m_previousScore = 0;
        uint24_t m_highScores[3];
        uint24_t m_applesEaten;
        uint24_t m_goldenApplesEaten;

        Stage m_currentStage = Stage::main;

        GUI m_gui;

        /* Custom Variables */
        bool m_inColorSelect = false;
        uint8_t m_selectedChannel = 0;
        uint8_t m_styleOption = 0;
        uint8_t m_SOColor[3];

        uint8_t m_storeSnakeSelected = Snake::SnakeSkin::basic;

        gfx_sprite_t* m_currentSnake;
        gfx_sprite_t* m_currentFood;
        gfx_sprite_t* m_goldenFood;
};