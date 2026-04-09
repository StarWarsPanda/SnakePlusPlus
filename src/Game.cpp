#include "Game.h"

Game::Game()
{
    srandom(rtc_Time());

    gfx_Begin();
    gfx_SetDrawBuffer();

    gfx_SetPalette(full_palette, sizeof_full_palette, 0);
    gfx_SetTransparentColor(97);

    gfx_SetTextTransparentColor(97);
    gfx_SetTextBGColor(97);
    gfx_SetTextFGColor(255);

    gfx_ZeroScreen();
    gfx_SwapDraw();
    gfx_ZeroScreen();
    gfx_SwapDraw();
}

Game::~Game()
{
    gfx_End();
}

void Game::Run()
{
    while (!m_exit)
    {
        Update();
        Draw();
        if (m_isFirst) m_isFirst = false;
    }
}

void Game::Update()
{
    kb_Scan();

    for (Key& key : m_keys)
    {
        key.Update();
    }

    if (m_keys[KeyMap::exit].IsDown())
    {
        m_exit = true;
    }

    if ((m_frame % 10) == 0)
    {
        const Direction currentDirection = m_snake.GetHeadDirection();

        const bool pressedLeft  = m_keys[KeyMap::left].IsDown();
        const bool pressedRight = m_keys[KeyMap::right].IsDown();
        const bool pressedUp    = m_keys[KeyMap::up].IsDown();
        const bool pressedDown  = m_keys[KeyMap::down].IsDown();

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

        if (m_snake.CheckCollision())
        {
            m_exit = true;
        }

        if (m_food.IsInsideFood(m_snake.GetHeadPosition()))
        {
            m_food.Eat(&m_snake);
            m_snake.AddLength();
        }
    }

    m_frame++;
}

void Game::Draw()
{
    gfx_BlitScreen();

    const uint24_t score = m_snake.GetScore();

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "Score: %d", score);

    if (score != m_previousScore)
        gfx_FillRectangle_NoClip(10, 10, gfx_GetStringWidth(buffer), 10);

    m_food.Draw(food_Apple, golden_apple);
    m_snake.Draw(snake_skins[m_selectedSnake], m_food.GetPosition(), !m_isFirst);

    gfx_PrintStringXY(buffer, 10, 10);

    m_previousScore = m_snake.GetScore();

    gfx_SwapDraw();
}

void Game::Key::Update()
{
    previousKey = key;
    key = (kb_Data[keyRow] & keyIndex) || (isBuffered && key);
}

bool Game::Key::IsUp()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return !keyVal;
}

bool Game::Key::IsDown()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return keyVal;
}

bool Game::Key::OnRisingEdge()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return !previousKey && keyVal;
}

bool Game::Key::OnFallingEdge()
{
    const bool keyVal = key;
    key = !isBuffered && key;
    return previousKey && !keyVal;
}