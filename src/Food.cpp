#include "Food.h"
#include "gfx/snkspr.h"

#include <compression.h>

Food::Food()
{
    m_moveSprite = gfx_MallocSprite(max(food_Apple_width, wings_1_width + 1), food_Apple_height + 1);
    gfx_GetSprite(m_moveSprite, 0, 0);
    Eat();
}

void Food::Setup()
{
    m_wings[0] = gfx_MallocSprite(wings_1_width, wings_1_height);
    zx0_Decompress(m_wings[0], wings_1_compressed);
    m_wings[1] = gfx_MallocSprite(wings_2_width, wings_2_height);
    zx0_Decompress(m_wings[1], wings_2_compressed);
    m_wings[2] = gfx_MallocSprite(wings_3_width, wings_3_height);
    zx0_Decompress(m_wings[2], wings_3_compressed);
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

    gfx_GetSprite(m_moveSprite, m_position.x, m_position.y);
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
        gfx_Sprite(m_moveSprite, lerpPreviousPosition.x, lerpPreviousPosition.y - 1);
        gfx_GetSprite(m_moveSprite, lerpPosition.x, lerpPosition.y - 1);
        gfx_TransparentSprite(m_wings[(frame / 2) % 3], lerpPosition.x + 1, lerpPosition.y - 1);
    }

    if (m_golden)
    {
        if (goldenFood == nullptr)
        {
            gfx_SetPaletteColor(78);
            gfx_FillRectangle_NoClip(lerpPosition.x, lerpPosition.y, BLOCK_SIZE, BLOCK_SIZE);
            gfx_ResetColor();
        }

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

    gfx_TransparentSprite_NoClip(food, lerpPosition.x, lerpPosition.y);

    m_previousLerpPosition = lerpPosition;
}

Food::FoodType Food::GetType() const
{
    return m_foodType;
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
