#include "Food.h"

Food::Food()
{
    Eat();
}

Food::~Food() {}

bool Food::IsInsideFood(Vector2D<int24_t> position) const
{
    return m_position == position || m_previousPosition == position;
}

bool Food::IsGolden() const
{
    return m_golden;
}

void Food::Eat(Snake* snake)
{
    if (m_golden)
    {
        snake->golden++;
    }

    do
    {
        m_position = Vector2D<int24_t>(
            randInt(0, (GFX_LCD_WIDTH - 1) / BLOCK_SIZE) * BLOCK_SIZE,
            randInt(0, (GFX_LCD_HEIGHT - 1) / BLOCK_SIZE) * BLOCK_SIZE
        );
    } while (snake != nullptr && snake->CheckCollision(&m_position));

    m_direction = Direction(randInt(0, Direction::Size - 1));

    m_golden = randInt(0, 100) <= 15;
}

void Food::Update()
{
    if (m_foodType == FoodType::winged)
    {
        m_previousPosition = m_position;

        switch (m_direction)
        {
            case Left: // ↖
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, -BLOCK_SIZE);
                if (m_position.y <= 0) m_direction = Down;
                if (m_position.x <= 0) m_direction = Up;
                break;
            case Down: // ↙
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, BLOCK_SIZE);
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE)) m_direction = Left;
                if (m_position.x <= 0) m_direction = Right;
                break;
            case Up: // ↗
                m_position += Vector2D<int24_t>(BLOCK_SIZE, -BLOCK_SIZE);
                if (m_position.y <= 0) m_direction = Right;
                if (m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Left;
                break;
            case Right: // ↘
                m_position += Vector2D<int24_t>(BLOCK_SIZE, BLOCK_SIZE);
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE)) m_direction = Up;
                if (m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Down;
                break;
            default:
                break;
        }
    }
}

void Food::Draw(gfx_sprite_t* food, gfx_sprite_t* goldenFood) const
{
    if (m_foodType == FoodType::winged)
    {
        gfx_SetPaletteColor(0);
        gfx_FillRectangle_NoClip(m_previousPosition.x, m_previousPosition.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_ResetColor();
    }

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

Food::FoodType Food::SetType(FoodType foodType)
{
    const FoodType previousFoodType = m_foodType;

    m_foodType = foodType;

    return previousFoodType;
}

const Vector2D<int24_t>& Food::GetPosition() const
{
    return m_position;
}
