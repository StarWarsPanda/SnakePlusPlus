#include "Food.h"
#include "gfx/snkspr.h"

Food::Food()
{
    m_moveSprite = gfx_MallocSprite(food_Apple_width, food_Apple_height);
    gfx_GetSprite(m_moveSprite, 0, 0);
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
    m_isEaten = true;

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

    m_previousPosition = m_position;

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
            case Up: // ↗
                m_position += Vector2D<int24_t>(BLOCK_SIZE, -BLOCK_SIZE);
                if (m_position.y <= 0) m_direction = Right;
                if (m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Left;
                if (m_position.y <= 0 && m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Down;
                    break;
            case Left: // ↖
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, -BLOCK_SIZE);
                if (m_position.y <= 0) m_direction = Down;
                if (m_position.x <= 0) m_direction = Up;
                if (m_position.y <= 0 && m_position.x <= 0) m_direction = Right;
                    break;
            case Down: // ↙
                m_position += Vector2D<int24_t>(-BLOCK_SIZE, BLOCK_SIZE);
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE)) m_direction = Left;
                if (m_position.x <= 0) m_direction = Right;
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE) && m_position.x <= 0) m_direction = Up;
                    break;
            case Right: // ↘
                m_position += Vector2D<int24_t>(BLOCK_SIZE, BLOCK_SIZE);
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE)) m_direction = Up;
                if (m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Down;
                if (m_position.y >= (GFX_LCD_HEIGHT - BLOCK_SIZE) && m_position.x >= (GFX_LCD_WIDTH - BLOCK_SIZE)) m_direction = Left;
                break;
            default:
                break;
        }
    }
}

void Food::Draw(gfx_sprite_t* food, gfx_sprite_t* goldenFood, uint24_t frame)
{
    const uint8_t frameMod = frame % 20;
    const Vector2D<int24_t> lerpPosition = map_range(frameMod, 0, 19, m_previousPosition, m_position);
    const Vector2D<int24_t> lerpPreviousPosition = map_range(frameMod - 1, 0, 19, m_previousPosition, m_position);
 
    if (m_isEaten)
    {
        gfx_Sprite(m_moveSprite, m_previousLerpPosition.x, m_previousLerpPosition.y);
        m_isEaten = false;
        return;
    }

    if (m_foodType == FoodType::winged)
    {
        gfx_Sprite(m_moveSprite, lerpPreviousPosition.x, lerpPreviousPosition.y);
    }

    if (m_golden)
    {
        if (goldenFood == nullptr)
        {
            gfx_SetPaletteColor(78);
            gfx_FillRectangle_NoClip(lerpPosition.x, lerpPosition.y, BLOCK_SIZE, BLOCK_SIZE);
            gfx_ResetColor();
        }

        gfx_GetSprite(m_moveSprite, lerpPosition.x, lerpPosition.y);
        gfx_TransparentSprite_NoClip(goldenFood, lerpPosition.x, lerpPosition.y);

        m_previousLerpPosition = lerpPosition;

        return;
    }

    if (food == nullptr)
    {
        gfx_SetPaletteColor(224);
        gfx_FillRectangle_NoClip(lerpPosition.x, lerpPosition.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_ResetColor();
    }

    gfx_GetSprite(m_moveSprite, lerpPosition.x, lerpPosition.y);
    gfx_TransparentSprite_NoClip(food, lerpPosition.x, lerpPosition.y);

    m_previousLerpPosition = lerpPosition;
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
