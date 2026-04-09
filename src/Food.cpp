#include "Food.h"

Food::Food()
{
    Eat();
}

Food::~Food() {}

bool Food::IsInsideFood(Vector2D<int24_t> position) const
{
    return m_position == position;
}

bool Food::IsGolden() const
{
    return m_golden;
}

void Food::Eat(const Snake* snake)
{
    do
    {
        m_position = Vector2D<int24_t>(
            randInt(0, (LCD_WIDTH - 1) / BLOCK_SIZE) * BLOCK_SIZE,
            randInt(0, (LCD_HEIGHT - 1) / BLOCK_SIZE) * BLOCK_SIZE
        );
    } while (snake != nullptr && snake->CheckCollision(&m_position));

    m_golden = randInt(0, 100) == 50;
}

void Food::Update()
{
    if (m_foodType == FoodType::winged)
    {
        switch (m_direction)
        {
            case Left: // ↖
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, -BLOCK_SIZE);
                break;
            case Down: // ↙
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, BLOCK_SIZE);
                break;
            case Up: // ↗
                m_position += Vector2D<int24_t>(BLOCK_SIZE, -BLOCK_SIZE);
                break;
            case Right: // ↘
                m_position += Vector2D<int24_t>(BLOCK_SIZE, BLOCK_SIZE);
                break;
            default:
                break;
        }
    }
}

void Food::Draw(gfx_sprite_t* food, gfx_sprite_t* goldenFood) const
{
    if (m_golden)
    {
        if (goldenFood == nullptr)
        {
            gfx_SetPaletteColor(78);
            gfx_FillRectangle_NoClip(m_position.x, m_position.y, BLOCK_SIZE, BLOCK_SIZE);
            gfx_ResetColor();
        }

        gfx_TransparentSprite_NoClip(goldenFood, m_position.x, m_position.y);

        return;
    }

    if (food == nullptr)
    {
        gfx_SetPaletteColor(224);
        gfx_FillRectangle_NoClip(m_position.x, m_position.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_ResetColor();
    }

    gfx_TransparentSprite_NoClip(food, m_position.x, m_position.y);
}

const Vector2D<int24_t>& Food::GetPosition() const
{
    return m_position;
}
