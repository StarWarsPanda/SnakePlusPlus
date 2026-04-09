#include "Snake.h"

Snake::SnakeSegment::SnakeSegment(Vector2D<int24_t> position, Direction direction) : position(position), direction(direction) {}
Snake::SnakeSegment::SnakeSegment() {}

Snake::SnakeSegment Snake::m_segments[MAX_SNAKE_SEGMENTS];

Snake::Snake(Vector2D<int24_t> startPosition, Direction startDirection, uint8_t defaultSize)
    : m_head(startPosition, startDirection), m_tail(startPosition, startDirection),
    m_previousTail(startPosition, startDirection), m_snakeSize(defaultSize), m_defaultSnakeSize(defaultSize)
{
    switch (startDirection)
    {
    case Up:
    case Down:
        m_tail.position.y += -(static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE * defaultSize;
        break;

    case Left:
    case Right:
        m_tail.position.x += -(static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE * defaultSize;
        break;
    }
}

Snake::~Snake() {}

void Snake::ChangeDirection(Direction newDirection)
{
    if (m_head.direction == newDirection)
        return;

    if (m_segmentLength < MAX_SNAKE_SEGMENTS)
    {
        m_segmentLength++;
    }

    for (int8_t i = m_segmentLength - 1; i >= 0; i--)
    {
        m_segments[i + 1] = m_segments[i];
    }

    m_head.direction = newDirection;
    m_segments[0] = m_head;
}

void Snake::AddLength()
{
    m_isAddingLength = true;
    m_snakeSize++;
}

void Snake::Update()
{
    m_previousTail = m_tail;

    switch (m_head.direction)
    {
    case Up:
    case Down:
        m_head.position.y += (static_cast<int24_t>(m_head.direction) - 1) * BLOCK_SIZE;
        break;
    case Left:
    case Right:
        m_head.position.x += (static_cast<int24_t>(m_head.direction) - 2) * BLOCK_SIZE;
        break;
    }

    if (!m_isAddingLength)
    {
        switch (m_tail.direction)
        {
        case Up:
        case Down:
            m_tail.position.y += (static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE;
            break;
        case Left:
        case Right:
            m_tail.position.x += (static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE;
            break;
        }
    }

    m_isAddingLength = false;

    if (m_segmentLength >= 1 && m_segments[m_segmentLength - 1].position == m_tail.position)
    {
        m_tail.direction = m_segments[m_segmentLength - 1].direction;
        m_segmentLength--;
    }
}

void Snake::Draw(gfx_sprite_t* snakeTiles, const Vector2D<int24_t>& foodPosition, bool isPartial) const
{
    {
        gfx_sprite_t* (*transform)(const gfx_sprite_t * __restrict, gfx_sprite_t * __restrict);
        switch (m_head.direction)
        {
        case Up:    transform = nullptr;               break;
        case Down:  transform = gfx_RotateSpriteHalf;  break;
        case Left:  transform = gfx_RotateSpriteCC;    break;
        case Right: transform = gfx_RotateSpriteC;     break;
        }

        const Vector2D<int24_t> delta = foodPosition - m_head.position;
        const bool isCloseToFood = delta.Dot(delta) < (BLOCK_SIZE * BLOCK_SIZE) * (2 * 2);

        DrawTile(snakeTiles, m_head.position, Vector2D<uint8_t>(0, isCloseToFood ? 1 : 0), transform);
    }

    {
        uint8_t previousColor = gfx_SetColor(0);
        gfx_FillRectangle_NoClip(m_previousTail.position.x, m_previousTail.position.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_SetColor(previousColor);
    }

    if (isPartial)
    {
        Vector2D<int24_t> position = m_head.position - Vector2D<int24_t>(
            IS_HORIZONTAL(m_head.direction) ? BLOCK_SIZE * (m_head.direction == Direction::Left ? -1 : 1) : 0,
            IS_VERTICAL(m_head.direction) ? BLOCK_SIZE * (m_head.direction == Direction::Up ? -1 : 1) : 0
        );

        uint8_t previousColor = gfx_SetColor(0);
        gfx_FillRectangle_NoClip(position.x, position.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_SetColor(previousColor);

        gfx_TransparentSpritePartial_NoClip(snakeTiles, position.x, position.y, 1, 0, IS_HORIZONTAL(m_head.direction) ? gfx_RotateSpriteC : nullptr);
    }
    else
    {
        if (m_segmentLength > 0)
        {
            DrawStraightSegments(snakeTiles, m_head.position, m_segments[0].position, m_head.direction);
        }
        else
        {
            DrawStraightSegments(snakeTiles, m_head.position, m_tail.position, m_head.direction);
        }
    }

    if (isPartial && m_segmentLength > 0)
    {
        const SnakeSegment& previousSegment = *(0 >= m_segmentLength - 1 ? &m_tail : &m_segments[1]);
        const SnakeSegment& segment = m_segments[0];
        const uint8_t combinedDirection = COMBINE(segment.direction, previousSegment.direction);

        gfx_sprite_t* (*transform)(const gfx_sprite_t * __restrict, gfx_sprite_t * __restrict) = nullptr;
        if (combinedDirection == COMBINE(Up, Left) || combinedDirection == COMBINE(Right, Down))
        {
            transform = nullptr;
        }
        else if (combinedDirection == COMBINE(Up, Right) || combinedDirection == COMBINE(Left, Down))
        {
            transform = gfx_RotateSpriteCC;
        }
        else if (combinedDirection == COMBINE(Right, Up) || combinedDirection == COMBINE(Down, Left))
        {
            transform = gfx_RotateSpriteC;
        }
        else if (combinedDirection == COMBINE(Left, Up) || combinedDirection == COMBINE(Down, Right))
        {
            transform = gfx_RotateSpriteHalf;
        }

        DrawTile(snakeTiles, segment.position, Vector2D<uint8_t>(2, 0), transform);
    }
    else
    {
        for (uint8_t i = 0; i < m_segmentLength; i++)
        {
            const SnakeSegment& previousSegment = *(i >= m_segmentLength - 1 ? &m_tail : &m_segments[i + 1]);
            const SnakeSegment& segment = m_segments[i];
            const uint8_t combinedDirection = COMBINE(segment.direction, previousSegment.direction);

            gfx_sprite_t* (*transform)(const gfx_sprite_t * __restrict, gfx_sprite_t * __restrict) = nullptr;
            if (combinedDirection == COMBINE(Up, Left) || combinedDirection == COMBINE(Right, Down))
            {
                transform = nullptr;
            }
            else if (combinedDirection == COMBINE(Up, Right) || combinedDirection == COMBINE(Left, Down))
            {
                transform = gfx_RotateSpriteCC;
            }
            else if (combinedDirection == COMBINE(Right, Up) || combinedDirection == COMBINE(Down, Left))
            {
                transform = gfx_RotateSpriteC;
            }
            else if (combinedDirection == COMBINE(Left, Up) || combinedDirection == COMBINE(Down, Right))
            {
                transform = gfx_RotateSpriteHalf;
            }

            DrawTile(snakeTiles, segment.position, Vector2D<uint8_t>(2, 0), transform);
            DrawStraightSegments(snakeTiles, segment.position, previousSegment.position, previousSegment.direction);
        }
    }

    {
        gfx_sprite_t* (*transformA)(const gfx_sprite_t * __restrict, gfx_sprite_t * __restrict) = nullptr;
        gfx_sprite_t* (*transformB)(const gfx_sprite_t * __restrict, gfx_sprite_t * __restrict) = nullptr;
        Vector2D<uint8_t> snakeTile = Vector2D<uint8_t>(3, 0);

        switch (m_tail.direction)
        {
        case Right: transformA = gfx_RotateSpriteC;    break;
        case Left:  transformA = gfx_RotateSpriteCC;   break;
        case Down:  transformA = gfx_RotateSpriteHalf; break;
        default:    break;
        }

        if (m_previousTail.direction != m_tail.direction)
        {
            snakeTile = Vector2D<uint8_t>(1, 1);

            const uint8_t combinedDirection = COMBINE(m_tail.direction, m_previousTail.direction);

            if (combinedDirection == COMBINE(Up, Right) || combinedDirection == COMBINE(Down, Left))
            {
                transformB = gfx_FlipSpriteY;
            }

            if (combinedDirection == COMBINE(Right, Down) || combinedDirection == COMBINE(Left, Up))
            {
                transformB = gfx_FlipSpriteX;
            }
        }

        DrawTile(snakeTiles, m_tail.position, snakeTile, transformA, transformB);
    }
}

bool Snake::CheckCollision(const Vector2D<int24_t>* other) const
{
    const bool otherExists = other != nullptr;
    const Vector2D<int24_t> testPosition = otherExists ? *other : m_head.position;

    if (testPosition == m_tail.position && (!otherExists || testPosition == m_head.position))
        return true;

    if (!otherExists && m_snakeType == SnakeType::classic && !(gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        0, 0, LCD_WIDTH, LCD_HEIGHT
    ))) return true;

    if (m_segmentLength > 0)
    {
        if (m_segmentLength > 1)
        {
            for (size_t i = 1; i < m_segmentLength; i++)
            {
                const SnakeSegment& segment = m_segments[i];
                const SnakeSegment& previousSegment = m_segments[i - 1];

                const Vector2D<int24_t> position = segment.position.Min(previousSegment.position);
                const Vector2D<int24_t> size = (previousSegment.position - segment.position).Absolute() + BLOCK_SIZE;

                if (
                    testPosition.x >= position.x &&
                    testPosition.x < position.x + size.x &&
                    testPosition.y >= position.y &&
                    testPosition.y < position.y + size.y
                    ) return true;
            }
        }

        const Vector2D<int24_t> position = m_segments[m_segmentLength - 1].position.Min(m_tail.position);
        const Vector2D<int24_t> size = (m_segments[m_segmentLength - 1].position - m_tail.position).Absolute() + BLOCK_SIZE;

        if (
            testPosition.x >= position.x &&
            testPosition.x < position.x + size.x &&
            testPosition.y >= position.y &&
            testPosition.y < position.y + size.y
            ) return true;
    }

    return false;
}

Vector2D<int24_t> Snake::GetHeadPosition() const
{
    return m_head.position;
}

Vector2D<int24_t> Snake::GetTailPosition() const
{
    return m_tail.position;
}

Direction Snake::GetHeadDirection() const
{
    return m_head.direction;
}

Direction Snake::GetTailDirection() const
{
    return m_tail.direction;
}

uint24_t Snake::GetScore() const
{
    return m_snakeSize - m_defaultSnakeSize;
}

inline void Snake::DrawTile(gfx_sprite_t* snakeTiles, Vector2D<int24_t> position, Vector2D<uint8_t> tile, gfx_sprite_t* (*transformA)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict), gfx_sprite_t* (*transformB)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict))
{
    if (!((0 <= position.x && position.x <= (LCD_WIDTH - BLOCK_SIZE)) &&
        (0 <= position.y && position.y <= (LCD_HEIGHT - BLOCK_SIZE)))
        ) return;

    uint8_t previousColor = gfx_SetColor(0);
    gfx_FillRectangle_NoClip(position.x, position.y, BLOCK_SIZE, BLOCK_SIZE);
    gfx_SetColor(previousColor);

    gfx_TransparentSpritePartial(snakeTiles, position.x, position.y, tile.x, tile.y, transformA, transformB);
}

void Snake::DrawStraightSegments(gfx_sprite_t* snakeTiles, Vector2D<int24_t> start, Vector2D<int24_t> end, Direction lineDirection)
{
    Vector2D<int24_t> step(0, 0);
    int24_t lengthRemaining;

    if (IS_VERTICAL(lineDirection))
    {
        step.y = end.y > start.y ? BLOCK_SIZE : -BLOCK_SIZE;
        lengthRemaining = end.y > start.y ? end.y - start.y : start.y - end.y;
    }
    else
    {
        step.x = end.x > start.x ? BLOCK_SIZE : -BLOCK_SIZE;
        lengthRemaining = end.x > start.x ? end.x - start.x : start.x - end.x;
    }

    if (lengthRemaining <= BLOCK_SIZE)
        return;

    Vector2D<int24_t> position = start + step;

    while (IS_VERTICAL(lineDirection) ?
        (step.y > 0 ? position.y < end.y : position.y > end.y)
        : (step.x > 0 ? position.x < end.x : position.x > end.x)
        )
    {
        if ((0 <= position.x && position.x <= (LCD_WIDTH - BLOCK_SIZE)) &&
            (0 <= position.y && position.y <= (LCD_HEIGHT - BLOCK_SIZE)))
        {
            uint8_t previousColor = gfx_SetColor(0);
            gfx_FillRectangle_NoClip(position.x, position.y, BLOCK_SIZE, BLOCK_SIZE);
            gfx_SetColor(previousColor);

            gfx_TransparentSpritePartial_NoClip(snakeTiles, position.x, position.y, 1, 0, IS_HORIZONTAL(lineDirection) ? gfx_RotateSpriteC : nullptr);
        }

        position += step;
    }
}