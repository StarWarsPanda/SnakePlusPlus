#include "Game.h"

Game::Game()
{
    srandom(rtc_Time());

    gfx_Begin();
    gfx_SetDrawBuffer();

    gfx_SetPalette(full_palette, sizeof_full_palette, 0);
    gfx_SetTransparentColor(101);

    gfx_SetTextTransparentColor(101);
    gfx_SetTextBGColor(101);
    gfx_SetTextFGColor(255);

    gfx_ZeroScreen();
    gfx_SwapDraw();
    gfx_ZeroScreen();
    gfx_SwapDraw();

    m_gui.style.colorBackground = 0;
    m_gui.style.colorPanel = 7;
    m_gui.style.colorBorder = 9;
    m_gui.style.colorText = 10;
    m_gui.style.colorTextDisabled = 9;
    m_gui.style.colorHighlight = 4;
    m_gui.style.colorButton = 8;
    m_gui.style.colorButtonHovered = 9;
    m_gui.style.colorButtonActive = 2;
    m_gui.style.itemPadding = 3;
    m_gui.style.alignment = GUI::Align::center;
    m_gui.style.borderSize = 1;
    m_gui.style.cornerRadius = 8;

    m_gui.navKeys.up    = { Key_Up_Combine,    false, false, true };
    m_gui.navKeys.left  = { Key_Left_Combine,  false, false, true };
    m_gui.navKeys.right = { Key_Right_Combine, false, false, true };
    m_gui.navKeys.down  = { Key_Down_Combine,  false, false, true };
    m_gui.navKeys.enter = { Key_Enter_Combine, false, false, true };
    m_gui.navKeys.back  = { Key_2nd_Combine,   false, false, true };
    m_gui.navKeys.exit  = { Key_Clear_Combine, false, false, true };

    Load();

    m_gui.SetFocusId(1);

    m_snake.golden = m_golden;
    m_storeSnakeSelected = m_selectedSnake;
}

Game::~Game()
{
    Save();
    gfx_End();
}

void Game::Run()
{
    while (!m_exit)
    {
        if ((m_currentStage == Stage::game || m_currentStage == Stage::pause) && !m_isFirst)
        {
            gfx_BlitScreen();
        }
        else
        {
            gfx_ZeroScreen();
        }

        m_gui.navKeys.Update();

        if (m_gui.navKeys.exit.IsDown())
        {
            m_exit = true;
        }

        m_gui.Begin();

        switch (m_currentStage)
        {
            case main:        StageMain();       break;
            case game_select: StageGameSelect(); break;
            case arcade:      StageArcade();     break;
            case story:       StageStory();      break;
            case game:        StageGame();       break;
            case store:       StageStore();      break;
            case options:     StageOptions();    break;
            case ui_change:   StageUIChange();   break;
            case stats:       StageStats();      break;
            case pause:       StagePause();      break;

            default: StageUnknown(); break;
        }

        if (m_isSecond && m_isFirst)
        {
            m_isFirst  = false;
            m_isSecond = false;
        }

        if (m_isFirst)
        {
            m_isSecond = true;
        }

        m_gui.End();

        m_frame++;

        gfx_SwapDraw();
    }
}

void Game::StageMain()
{
    gfx_SetTextScale(3, 3);
    m_gui.Label("Snake++", 160 - (gfx_GetStringWidth("Snake++") / 2), 20);
    gfx_SetTextScale(1, 1);

    m_gui.BeginPanel(80, 60, 160, 168);

    if (m_gui.ButtonAt(120, 30, "ARCADE"))
    {
        m_currentStage = game_select;
    }

    m_gui.ButtonAt(120, 30, "STORY");
    
    if (m_gui.ButtonAt(120, 30, "SHOP"))
    {
        m_currentStage = store;
        m_gui.SetFocusId(0);
    }

    if (m_gui.ButtonAt(120, 30, "OPTIONS"))
    {
        m_currentStage = options;
    }
    
    if (m_gui.ButtonAt(120, 30, "QUIT"))
    {
        m_exit = true;
    }

    m_gui.EndPanel();

    gfx_PrintStringXY("v1.1.2",5,227);
}

void Game::StageGameSelect()
{
    const uint24_t previousItemPadding = m_gui.style.itemPadding;
    m_gui.style.itemPadding = 5;

    m_gui.BeginPanel(20, 10, 120, 180);

    if (m_gui.ButtonAt(80, 30, "CLASSIC"))
    {
        m_snake.SetType(Snake::SnakeType::classic);
        m_food.SetType(Food::FoodType::classic);
        m_currentStage = game;
        m_isFirst = true;
        m_gui.style.itemPadding = previousItemPadding;
        return;
    }

    if (m_gui.ButtonAt(80, 30, "WRAP"))
    {
        m_snake.SetType(Snake::SnakeType::wrap);
        m_food.SetType(Food::FoodType::classic);
        m_currentStage = game;
        m_isFirst = true;
        m_gui.style.itemPadding = previousItemPadding;
        return;
    }

    if (m_gui.ButtonAt(80, 30, "WINGED"))
    {
        m_snake.SetType(Snake::SnakeType::classic);
        m_food.SetType(Food::FoodType::winged);
        m_currentStage = game;
        m_isFirst = true;
        m_gui.style.itemPadding = previousItemPadding;
        return;
    }

    m_gui.ButtonAt(80, 30); // WALLS
    m_gui.ButtonAt(80, 30); // VS
    
    m_gui.EndPanel();

    if (m_gui.Button(20, 200, 120, 30, "BACK"))
    {
        m_currentStage = main;
        m_gui.SetFocusId(0);
    }

    m_gui.style.itemPadding = previousItemPadding;
}

void Game::StageArcade()
{
    
}

void Game::StageStory()
{
}

void Game::StageGame()
{
    if (m_gui.navKeys.back.OnRisingEdge())
    {
        m_currentStage = pause;
        return;
    }

    if ((m_frame % 10) == 0)
    {
        const Direction currentDirection = m_snake.GetHeadDirection();

        const bool pressedLeft = m_gui.navKeys.left.IsDown();
        const bool pressedRight = m_gui.navKeys.right.IsDown();
        const bool pressedUp = m_gui.navKeys.up.IsDown();
        const bool pressedDown = m_gui.navKeys.down.IsDown();

        if (pressedLeft && IS_VERTICAL(currentDirection))
        {
            m_snake.ChangeDirection(Direction::Left);
        }
        else if (pressedRight && IS_VERTICAL(currentDirection))
        {
            m_snake.ChangeDirection(Direction::Right);
        }
        else if (pressedUp && IS_HORIZONTAL(currentDirection))
        {
            m_snake.ChangeDirection(Direction::Up);
        }
        else if (pressedDown && IS_HORIZONTAL(currentDirection))
        {
            m_snake.ChangeDirection(Direction::Down);
        }

        m_snake.Update();
        m_golden = m_snake.golden;

        if (m_snake.CheckCollision())
        {
            m_currentStage = game_select;
            m_snake.Reset();
        }

        if (m_frame % 20 == 0)
        {
            m_food.Update();
        }

        if (m_food.IsInsideFood(m_snake.GetHeadPosition()))
        {
            m_food.Eat(&m_snake);
            m_snake.AddLength();
        }
    }

    const uint24_t score = m_snake.GetScore();

    if (score > m_highScore)
    {
        m_highScore = score;
    }

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "Score: %d", score);

    if (score != m_previousScore)
    {
        gfx_sprite_t* scoreBackground = gfx_AllocSprite(gfx_GetStringWidth(buffer), 10, malloc);
        gfx_GetSprite(scoreBackground, 10, 10);
        gfx_FillRectangle_NoClip(10, 10, gfx_GetStringWidth(buffer), 10);
        gfx_SetTransparentColor(255);
        gfx_TransparentSprite_NoClip(scoreBackground, 10, 10);
        gfx_SetTransparentColor(101);
        free(scoreBackground);
    }

    m_food.Draw(food_Apple, golden_apple);
    m_snake.Draw(snake_skins[m_selectedSnake], m_food.GetPosition(), !m_isFirst);

    gfx_PrintStringXY(buffer, 10, 10);

    m_previousScore = m_snake.GetScore();
}

void Game::StageStore()
{
    gfx_TransparentSprite_NoClip(golden_apple, 5, 5);
    gfx_SetTextXY(20, 5);
    gfx_PrintInt(m_golden, 1);

    Snake snake = Snake(
        Vector2D<int24_t>(46,50),
        Direction::Up,
        8,
        m_frame
    );

    GUI::DrawRoundRect(30, 30, 260, 130, m_gui.style.colorButton, m_gui.style.colorBorder, m_gui.style.cornerRadius);
    GUI::DrawRoundRect(40, 40, 20, 110, m_gui.style.colorButton, m_gui.GetFocusedId() == -1 ? m_gui.style.colorHighlight : m_gui.style.colorBorder, m_gui.style.cornerRadius);
    
    gfx_sprite_t* selectedSnakeTileset = snake_skins[m_storeSnakeSelected];

    if (m_collectables[m_storeSnakeSelected].locked)
    {
        selectedSnakeTileset = snake_Invalid_Tileset;

        if (m_collectables[m_storeSnakeSelected].cost > 0)
        {
            gfx_TransparentSprite_NoClip(golden_apple, 70, 142);
            gfx_SetTextXY(85, 142);
            gfx_PrintInt(m_collectables[m_storeSnakeSelected].cost, 1);
        }

        gfx_PrintStringXY("Press [enter] to purchase", 70, 132);
        gfx_PrintStringXY("???", 70, 60);
    }
    else
    {
        gfx_PrintStringXYCharWrap(Description(m_storeSnakeSelected), 70, 60, 275, 2);
    }

    if (m_storeSnakeSelected == m_selectedSnake)
    {
        gfx_PrintStringXY("Selected", 30, 20);
    }

    snake.Draw(selectedSnakeTileset, Vector2D<int24_t>(-50, -50), false);

    gfx_PrintStringXY(SnakeType(m_storeSnakeSelected), 70, 40);

    if (m_gui.GetFocusedId() <= 0)
    {
        m_gui.Lock();
    }

    if (m_gui.GetFocusedId() == -1)
    {
        if (m_gui.navKeys.left.OnRisingEdge())
        {
            DecrementWrap(m_storeSnakeSelected, 0, Snake::SnakeSkin::size - 1, 1);
        }

        if (m_gui.navKeys.right.OnRisingEdge())
        {
            IncrementWrap(m_storeSnakeSelected, 0, Snake::SnakeSkin::size - 1, 1);
        }

        if (m_gui.navKeys.down.OnRisingEdge())
        {
            m_gui.Unlock();
            m_gui.SetFocusId(1);
        }

        if (m_collectables[m_storeSnakeSelected].locked && m_golden >= m_collectables[m_storeSnakeSelected].cost && m_gui.navKeys.enter.OnRisingEdge())
        {
            m_golden -= m_collectables[m_storeSnakeSelected].cost;
            m_collectables[m_storeSnakeSelected].locked = false;
            m_collectables[m_storeSnakeSelected].cost = 0;
        }

        if (!m_collectables[m_storeSnakeSelected].locked && m_storeSnakeSelected != m_selectedSnake && m_gui.navKeys.enter.OnRisingEdge())
        {
            m_selectedSnake = (Snake::SnakeSkin)m_storeSnakeSelected;
        }
    }

    if (m_gui.Button(30, 180, 80, 30, "BACK"))
    {
        m_currentStage = main;
        m_gui.SetFocusId(3);
    }
}

void Game::StageStats()
{
    gfx_PrintStringXY("Highscore: ", 5, 5);
    gfx_SetTextXY(100, 5);
    gfx_PrintInt(m_highScore, 1);

    if (m_gui.Button(20, 180, 80, 30, "BACK"))
    {
        m_currentStage = options;
        m_gui.SetFocusId(2);
    }
}

void Game::StageUIChange()
{
    if (m_gui.GetFocusedId() <= 3)
    {
        m_gui.Lock();
    }

    if (m_gui.GetFocusedId() == -1)
    {
        if (m_gui.navKeys.enter.OnRisingEdge())
        {
            m_inColorSelect = true;
            switch (m_styleOption)
            {
                case 0:
                    /* background color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorBackground], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 1:
                    /* panel color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorPanel], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 2:
                    /* border color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorBorder], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 3:
                    /* text color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorText], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 4:
                    /* disabled text color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorTextDisabled], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 5:
                    /* highlight color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorHighlight], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 6:
                    /* button fill color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorButton], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 7:
                    /* button hovered fill color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorButtonHovered], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
                case 8:
                    /* button active fill color */
                    gfx_1555ToRGB(gfx_palette[m_gui.style.colorButtonActive], m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                    break;
            }
        }

        if (m_gui.navKeys.back.OnRisingEdge())
        {
            m_inColorSelect = false;
        }

        if (!m_inColorSelect)
        {
            if (m_gui.navKeys.down.OnRisingEdge())
            {
                IncrementWrap(m_styleOption, 0, style_strings_length + 1, 1);
            }

            if (m_gui.navKeys.up.OnRisingEdge())
            {
                DecrementWrap(m_styleOption, 0, style_strings_length + 1, 1);
            }
        }
        else if(m_styleOption <= 8)
        {
            if (m_gui.navKeys.up.OnRisingEdge())
            {
                ++m_SOColor[m_selectedChannel];
            }

            if (m_gui.navKeys.down.OnRisingEdge())
            {
                --m_SOColor[m_selectedChannel];
            }

            if (m_gui.navKeys.right.OnRisingEdge())
            {
                IncrementWrap(m_selectedChannel, 0, 3, 1);
            }

            if (m_gui.navKeys.left.OnRisingEdge())
            {
                DecrementWrap(m_selectedChannel, 0, 3, 1);
            }

            gfx_SetTextXY(180, 20);
            gfx_PrintInt(m_SOColor[0], 1);
            gfx_SetTextXY(210, 20);
            gfx_PrintInt(m_SOColor[1], 1);
            gfx_SetTextXY(240, 20);
            gfx_PrintInt(m_SOColor[2], 1);

            Vector2D<int24_t> triPositions[6] = {
                {198,16},
                {184,16},
                {191,9},
                {198,34},
                {184,34},
                {191,43}
            };

            uint8_t i = 0;

            do
            {
                if (i == m_selectedChannel)
                {
                    gfx_SetColor(m_gui.style.colorButtonActive);
                }
                else
                {
                    gfx_SetColor(m_gui.style.colorBorder);
                }

                gfx_Rectangle_NoClip(178 + triPositions[0].x - 198, 18, 28, 14);

                gfx_SetColor(m_gui.style.colorBorder);

                gfx_FillTriangle(
                    triPositions[0].x, triPositions[0].y,
                    triPositions[1].x, triPositions[1].y,
                    triPositions[2].x, triPositions[2].y
                );

                gfx_FillTriangle(
                    triPositions[3].x, triPositions[3].y,
                    triPositions[4].x, triPositions[4].y,
                    triPositions[5].x, triPositions[5].y
                );

                triPositions[0].x += 30;
                triPositions[1].x += 30;
                triPositions[2].x += 30;
                triPositions[3].x += 30;
                triPositions[4].x += 30;
                triPositions[5].x += 30;
            } while (++i <= 2);
        }
        else if (m_styleOption == 9)
        {
            if (m_gui.navKeys.down.OnRisingEdge())
            {
                IncrementWrap(m_gui.style.itemPadding, 0, 12, 1);
            }

            if (m_gui.navKeys.up.OnRisingEdge())
            {
                DecrementWrap(m_gui.style.itemPadding, 0, 12, 1);
            }

            gfx_SetTextXY(180, 20);
            gfx_PrintInt(m_gui.style.itemPadding, 1);
            const uint8_t previousColor = gfx_SetColor(m_gui.style.colorBorder);
            gfx_Rectangle_NoClip(178, 18, 11, 12);
            gfx_SetColor(previousColor);
        }
        else if (m_styleOption == 10)
        {
            if (m_gui.navKeys.down.OnRisingEdge())
            {
                switch (m_gui.style.alignment)
                {
                    case GUI::Align::left:
                        m_gui.style.alignment = GUI::Align::center;
                        break;
                    case GUI::Align::center:
                        m_gui.style.alignment = GUI::Align::right;
                        break;
                    case GUI::Align::right:
                        m_gui.style.alignment = GUI::Align::left;
                        break;
                }
            }

            if (m_gui.navKeys.up.OnRisingEdge())
            {
                switch (m_gui.style.alignment)
                {
                    case GUI::Align::left:
                        m_gui.style.alignment = GUI::Align::right;
                        break;
                    case GUI::Align::center:
                        m_gui.style.alignment = GUI::Align::left;
                        break;
                    case GUI::Align::right:
                        m_gui.style.alignment = GUI::Align::center;
                        break;
                }
            }

            gfx_PrintStringXY(alignment_strings[m_gui.style.alignment], 180, 20);
        }
        else if (m_styleOption == 11)
        {
            if (m_gui.navKeys.down.OnRisingEdge())
            {
                IncrementWrap(m_gui.style.borderSize, 0, 5, 1);
            }

            if (m_gui.navKeys.up.OnRisingEdge())
            {
                DecrementWrap(m_gui.style.borderSize, 0, 5, 1);
            }

            gfx_SetTextXY(180, 20);
            gfx_PrintInt(m_gui.style.borderSize, 1);
            const uint8_t previousColor = gfx_SetColor(m_gui.style.colorBorder);
            gfx_Rectangle_NoClip(178, 18, 11, 12);
            gfx_SetColor(previousColor);
        }
        else if (m_styleOption == 12)
        {
            if (m_gui.navKeys.down.OnRisingEdge())
            {
                IncrementWrap(m_gui.style.cornerRadius, 0, 12, 1);
            }

            if (m_gui.navKeys.up.OnRisingEdge())
            {
                DecrementWrap(m_gui.style.cornerRadius, 0, 12, 1);
            }

            gfx_SetTextXY(180, 20);
            gfx_PrintInt(m_gui.style.cornerRadius, 1);
            const uint8_t previousColor = gfx_SetColor(m_gui.style.colorBorder);
            gfx_Rectangle_NoClip(178, 18, 19, 12);
            gfx_SetColor(previousColor);
        }

        for (uint8_t i = 0; i < style_strings_length; i++)
        {
            if (i == m_styleOption)
            {
                gfx_SetTextBGColor(m_gui.style.colorButtonActive);
            }
            else
            {
                gfx_SetTextBGColor(101);
            }
            
            gfx_PrintStringXY(style_strings[i], 20, 20 + gfx_GetFontHeight() * i);
        }

        switch (m_styleOption)
        {
            case 0:
                /* background color */
                if(m_inColorSelect) gfx_palette[m_gui.style.colorBackground] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 1:
                /* panel color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorPanel] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 2:
                /* border color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorBorder] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 3:
                /* text color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorText] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 4:
                /* disabled text color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorTextDisabled] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 5:
                /* highlight color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorHighlight] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 6:
                /* button fill color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorButton] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 7:
                /* button hovered fill color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorButtonHovered] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case 8:
                /* button active fill color */
                if (m_inColorSelect) gfx_palette[m_gui.style.colorButtonActive] = gfx_RGBTo1555(m_SOColor[0], m_SOColor[1], m_SOColor[2]);
                break;
            case style_strings_length:
                m_styleOption = style_strings_length - 1;
                m_gui.Unlock();
                m_gui.SetFocusId(4);
                break;
        }

        gfx_SetTextBGColor(101);
    }

    m_gui.BeginPanel(205, 60, 100, 120);

    m_gui.ButtonAt(80, 30, "One");
    m_gui.ButtonAt(80, 30, "Two");
    m_gui.ButtonAt(80, 30, "Three");

    m_gui.EndPanel();
    
    if (m_gui.Button(20, 180, 80, 30, "BACK"))
    {
        m_currentStage = options;
        m_gui.SetFocusId(1);
    }
}

void Game::StageOptions()
{
    m_gui.BeginPanel(20, 10, 120, 180);

    if (m_gui.ButtonAt(80, 30, "UI"))
    {
        m_currentStage = ui_change;
        m_gui.SetFocusId(0);
    }

    if (m_gui.ButtonAt(80, 30, "STATS"))
    {
        m_currentStage = stats;
    }

    m_gui.EndPanel();

    if (m_gui.Button(20, 200, 120, 30, "BACK"))
    {
        m_currentStage = main;
        m_gui.SetFocusId(4);
    }
}

void Game::StageUnknown()
{
}

void Game::StagePause()
{
    m_gui.BeginPanel(50, 51, 220, 138);

    if (m_gui.ButtonAt(172, 30, "RESUME"))
    {
        m_currentStage = game;
        m_isFirst = true;
        return;
    }

    if (m_gui.ButtonAt(172, 30, "SETTINGS"))
    {
        m_currentStage = Stage::options;
        return;
    }

    if (m_gui.ButtonAt(172, 30, "BACK TO MENU"))
    {
        m_currentStage = Stage::main;
        return;
    }

    m_gui.EndPanel();
}

void Game::Load()
{
    const uint8_t snksv = ti_Open("snksv", "r");
    if (!snksv)
        return;

    FileFormat file;
    ti_Read(&file, sizeof(FileFormat), 1, snksv);

    m_gui.style = file.style;
    m_golden = file.goldenApples;
    m_highScore = file.highScore;
    m_selectedSnake = file.selectedSnake;
    memcpy(m_collectables, file.unlockedSnakes, sizeof(SnakeSkinCollectable) * Snake::SnakeSkin::size);

    ti_Close(snksv);

    gfx_SetPalette(file.palette, sizeof(file.palette), 0);
}

void Game::Save()
{
    FileFormat file;
    file.style = m_gui.style;
    file.goldenApples = m_golden;
    file.highScore = m_highScore;
    file.selectedSnake = m_selectedSnake;
    memcpy(file.unlockedSnakes, m_collectables, sizeof(SnakeSkinCollectable) * Snake::SnakeSkin::size);
    memcpy(file.palette, gfx_palette, sizeof(file.palette));;
    
    const uint8_t snksv = ti_Open("snksv", "w");
    if (!snksv)
        return;

    ti_Write(&file, sizeof(FileFormat), 1, snksv);
    ti_SetArchiveStatus(true, snksv);
    ti_Close(snksv);
}

void GUI::Key::Update()
{
    previousKey = key;
    key = (kb_Data[keyRow] & keyIndex) || (isBuffered && key);
}

bool GUI::Key::IsUp()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return !keyVal;
}

bool GUI::Key::IsDown()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return keyVal;
}

bool GUI::Key::OnRisingEdge()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return !previousKey && keyVal;
}

bool GUI::Key::OnFallingEdge()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return previousKey && !keyVal;
}

void GUI::NavKeys::Update()
{
    up.Update();
    left.Update();
    right.Update();
    down.Update();
    enter.Update();
    back.Update();
    exit.Update();
}

void GUI::Begin()
{
    kb_Scan();
    navKeys.Update();

    m_focus.nextId = 1;
    m_focus.hotId = 0;
    m_panelDepth = 0;
    m_currentPanel = nullptr;
}

void GUI::End()
{
    if (m_focus.hotId == 0)
    {
        m_focus.activeId = 0;
    }
}

void GUI::BeginPanel(int x, int y, int width, int height)
{
    if (m_panelDepth >= GUI_MAX_PANELS)
        return;

    Panel& panel = m_panelStack[m_panelDepth++];
    m_currentPanel = &panel;
    panel.x = x;
    panel.y = y;
    panel.width = width;
    panel.height = height;

    switch (style.alignment)
    {
        case left:
            panel.cursorX = x + style.itemPadding;
            break;
        case right:
            panel.cursorX = x + width - style.itemPadding;
            break;
        case center:
            panel.cursorX = x + (width / 2);
            break;
    }

    panel.cursorY = y + style.itemPadding;
    panel.active = true;

    if (style.cornerRadius > 0)
    {
        DrawRoundRect(x, y, width, height, style.colorPanel, style.colorBorder, style.cornerRadius);
    }
    else
    {
        DrawRect(x, y, width, height, style.colorPanel, style.colorBorder);
    }
}

void GUI::EndPanel()
{
    const bool panelDepthHasSize = m_panelDepth > 0;

    if (panelDepthHasSize)
    {
        m_panelStack[--m_panelDepth].active = false;
    }

    m_currentPanel = panelDepthHasSize ? &m_panelStack[m_panelDepth - 1] : nullptr;
}

bool GUI::Button(int x, int y, int width, int height, const char* label)
{
    const uint24_t id = AllocId();
    const GUI::InteractionState state = QueryInteraction(id);

    uint8_t fillColor = style.colorButton;
    uint8_t borderColor = style.colorBorder;
    
    switch (state)
    {
        case interaction_hovered:
            fillColor = style.colorButtonHovered;
            borderColor = style.colorHighlight;
            break;
        case interaction_pressed:
        case interaction_held:
            fillColor = style.colorButtonActive;
            borderColor = style.colorHighlight;
            break;
        case interaction_released:
        case interaction_none:
        default:
            break;
    }

    if (style.cornerRadius > 0)
    {
        DrawRoundRect(x, y, width, height, fillColor, borderColor, style.cornerRadius);
    }
    else
    {
        DrawRect(x, y, width, height, fillColor, borderColor);
    }

    if (label != nullptr)
    {
        DrawText(label, x + (width / 2), y + (height / 2) - (gfx_GetFontHeight() / 2), style.colorText, center);
    }
    
    return state == interaction_released;
}

bool GUI::ButtonAt(int width, int height, const char* label)
{
    if (!m_currentPanel)
        return false;

    int x;

    switch (style.alignment)
    {
        case left:
            x = m_currentPanel->cursorX;
            break;
        case right:
            x = m_currentPanel->cursorX - width;
            break;
        case center:
            x = m_currentPanel->cursorX - (width / 2);
            break;
    }

    const bool button = Button(x, m_currentPanel->cursorY, width, height, label);
    AdvanceCursor(width, height);
   
    return button;
}

void GUI::Label(const char* text, int x, int y, Align align)
{
    DrawText(text, x, y, style.colorText, align);
}

void GUI::LabelAt(const char* text, Align align)
{
    if (!m_currentPanel)
        return;

    const unsigned int stringWidth = gfx_GetStringWidth(text);
    int x;

    switch (align)
    {
        case left:
            x = m_currentPanel->cursorX;
            break;
        case right:
            x = m_currentPanel->cursorX - stringWidth;
            break;
        case center:
            x = m_currentPanel->cursorX - (stringWidth / 2);
            break;
    }

    Label(text, x, m_currentPanel->cursorY);
    AdvanceCursor(m_currentPanel->width - 2 * style.itemPadding, 16);
}

void GUI::Separator(int x, int y, int width)
{
    const uint8_t previousColor = gfx_SetColor(style.colorBorder);
    gfx_HorizLine(x, y + 8, width);
    gfx_SetColor(previousColor);
}

void GUI::SeparatorAt(int width)
{
    if (!m_currentPanel)
        return;

    Separator(m_currentPanel->cursorX, m_currentPanel->cursorY, width);
    AdvanceCursor(width, 16);
}

void GUI::SetFocusId(int id)
{
    m_focus.focusedId = id;
}

int GUI::GetFocusedId()
{
    return m_focus.focusedId;
}

void GUI::Lock()
{
    m_lock = true;
}

void GUI::Unlock()
{
    m_lock = false;
    m_focus.focusedId = 0;
}

void GUI::DrawRect(int x, int y, int width, int height, uint8_t fillColor, uint8_t borderColor)
{
    const uint8_t previousColor = gfx_SetColor(fillColor);
    gfx_FillRectangle(x + 1, y + 1, width - 2, height - 2);
    gfx_SetColor(borderColor);
    gfx_Rectangle(x, y, width, height);
    gfx_SetColor(previousColor);
}

void GUI::DrawRoundRect(int x, int y, int width, int height, uint8_t fillColor, uint8_t borderColor, uint8_t radius)
{
    if (radius == 0)
    {
        GUI::DrawRect(x, y, width, height, fillColor, borderColor);
        return;
    }

    const uint8_t radiusSubOne = radius - 1;
    const uint8_t diameter = radius * 2;

    const uint8_t previousColor = gfx_SetColor(borderColor);
    gfx_Circle(x + radius, y + radius, radius);
    gfx_Circle(x + radius, y + height - radius, radius);
    gfx_Circle(x + width - radius, y + height - radius, radius);
    gfx_Circle(x + width - radius, y + radius, radius);

    gfx_Line(x + radius, y + height, x + width - radius, y + height);
    gfx_Line(x + width, y + height - radius, x + width, y + radius);
    gfx_Line(x + radius, y, x + width - radius, y);
    gfx_Line(x, y + radius, x, y + height - radius);

    gfx_SetColor(fillColor);
    gfx_FillCircle(x + radius, y + radius, radiusSubOne);
    gfx_FillCircle(x + radius, y + height - radius, radiusSubOne);
    gfx_FillCircle(x + width - radius, y + height - radius, radiusSubOne);
    gfx_FillCircle(x + width - radius, y + radius, radiusSubOne);

    gfx_FillRectangle(x + radius + 1, y + 1, width - diameter, height - 1);
    gfx_FillRectangle(x + 1, y + radius, radius, height - diameter);
    gfx_FillRectangle(x + width - radius, y + radius, radius, height - diameter);

    gfx_SetColor(previousColor);
}

void GUI::DrawText(const char* text, int x, int y, uint8_t color, Align align)
{
    if (text == nullptr || text[0] == '\0')
    {
        return;
    }

    const uint8_t previousForegroundColor = gfx_SetTextFGColor(color);
    const uint24_t textWidth = gfx_GetStringWidth(text);

    switch (align)
    {
        case Align::left:
            gfx_PrintStringXY(text, x, y);
            break;
        case Align::center:
            gfx_PrintStringXY(text, x - (textWidth / 2), y);
            break;
        case Align::right:
            gfx_PrintStringXY(text, x - textWidth, y);
            break;
    }

    gfx_SetTextFGColor(previousForegroundColor);
}

GUI::InteractionState GUI::QueryInteraction(int id)
{
    if (m_lock)
    {
        m_focus.focusedId = -1;
        return interaction_none;
    }

    if (navKeys.down.OnRisingEdge())
    {
        if (m_focus.focusedId == 0 || m_focus.focusedId == id - 1)
        {
            m_focus.focusedId = id;
        }
        else
        {
            IncrementWrap(m_focus.focusedId, 0, m_focus.maxId, 1);
        }
    }

    if (navKeys.up.OnRisingEdge())
    {
        if (m_focus.focusedId == id + 1)
        {
            m_focus.focusedId = id;
        }
        else if(m_focus.focusedId == 0)
        {
            m_focus.focusedId = m_focus.maxId - 1;
        }
        else
        {
            DecrementWrap(m_focus.focusedId, 0, m_focus.maxId, 1);
        }
    }

    if (navKeys.exit.OnRisingEdge() && m_focus.focusedId == id)
    {
        m_focus.focusedId = 0;
        return interaction_none;
    }

    if (m_focus.focusedId != id)
    {
        return interaction_none;
    }

    m_focus.hotId = id;

    if (navKeys.enter.IsDown())
    {
        if (m_focus.activeId > 0 && m_focus.activeId != m_focus.focusedId)
        {
            m_focus.activeId = 0;
            return interaction_released;
        }

        if (m_focus.focusedId == id)
        {
            m_focus.activeId = m_focus.focusedId;
            return interaction_held;
        }
    }

    if (!navKeys.enter.IsDown() && m_focus.activeId == id)
    {
        m_focus.activeId = 0;
        return interaction_released;
    }

    return interaction_hovered;

}

uint24_t GUI::AllocId()
{
    if (m_focus.nextId + 1 > m_focus.maxId)
    {
        m_focus.maxId++;
    }

    return m_focus.nextId++;
}

void GUI::AdvanceCursor(int itemWidth, int itemHeight)
{
    m_currentPanel->cursorY += itemHeight + style.itemPadding;
}
