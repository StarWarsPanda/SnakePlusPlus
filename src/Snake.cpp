#include "Snake.h"

Snake::SnakeSegment::SnakeSegment(Vector2D<int24_t> position, Direction direction) : position(position), direction(direction) {}
Snake::SnakeSegment::SnakeSegment() {}

Snake::SnakeSegment Snake::m_segments[MAX_SNAKE_SEGMENTS];

Snake::Snake(Vector2D<int24_t> startPosition, Direction startDirection, uint8_t defaultSize, const uint24_t& frame)
    : m_head(startPosition, startDirection), m_tail(startPosition, startDirection), m_previousTail(startPosition, startDirection), m_snakeSize(defaultSize), m_defaultSnakeSize(defaultSize), m_frame(frame), m_resetStartPosition(startPosition), m_resetStartDirection(startDirection)
{
    switch (startDirection)
    {
        case Up:
        case Down:
            m_tail.position.y += -(static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE * defaultSize;
            m_previousTail.position.y += -(static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE * (defaultSize + 1);
            break;

        case Left:
        case Right:
            m_tail.position.x += -(static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE * defaultSize;
            m_previousTail.position.x += -(static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE * (defaultSize + 1);
            break;
        case Size:
        default:
            break;
    }
}

Snake::~Snake() {}

void Snake::Reset()
{
    m_head.position = m_resetStartPosition;
    m_head.direction = m_resetStartDirection;
    m_tail.position = m_resetStartPosition;
    m_tail.direction = m_resetStartDirection;
    m_previousTail.position = m_resetStartPosition;
    m_previousTail.direction = m_resetStartDirection;
    m_snakeSize = m_defaultSnakeSize;

    for (size_t i = 0; i < m_segmentLength; i++)
    {
        m_segments[i] = SnakeSegment();
    }

    m_segmentLength = 0;

    switch (m_resetStartDirection)
    {
        case Up:
        case Down:
            m_tail.position.y += -(static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE * m_defaultSnakeSize;
            break;

        case Left:
        case Right:
            m_tail.position.x += -(static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE * m_defaultSnakeSize;
            break;
        case Size:
        default:
            break;
    }
}

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
            m_head.position.y += ((static_cast<int24_t>(m_head.direction) - 1) * BLOCK_SIZE);
            if (m_snakeType == SnakeType::wrap)
            {
                IncrementWrap(m_head.position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);
                DecrementWrap(m_head.position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);
            }
            break;
        case Left:
        case Right:
            m_head.position.x += ((static_cast<int24_t>(m_head.direction) - 2) * BLOCK_SIZE) % (GFX_LCD_WIDTH - BLOCK_SIZE + 1);
            if (m_snakeType == SnakeType::wrap)
            {
                IncrementWrap(m_head.position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
                DecrementWrap(m_head.position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
            }
            break;
        case Size:
        default:
            break;
    }

    if (!m_isAddingLength)
    {
        switch (m_tail.direction)
        {
            case Up:
            case Down:
                m_tail.position.y += ((static_cast<int24_t>(m_tail.direction) - 1) * BLOCK_SIZE);
                IncrementWrap(m_tail.position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);
                DecrementWrap(m_tail.position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);
                break;
            case Left:
            case Right:
                m_tail.position.x += ((static_cast<int24_t>(m_tail.direction) - 2) * BLOCK_SIZE);
                IncrementWrap(m_tail.position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
                DecrementWrap(m_tail.position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
                break;
            case Size:
            default:
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
            case Size:  default:                           return;
        }

        const Vector2D<int24_t> delta = foodPosition - m_head.position;
        const bool isCloseToFood = delta.Dot(delta) < (BLOCK_SIZE * BLOCK_SIZE) * (2 * 2);

        DrawTile(snakeTiles, m_head.position, Vector2D<uint8_t>(0, isCloseToFood ? 1 : 0), transform);
    }

    if(isPartial)
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

        DecrementWrap(position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
        IncrementWrap(position.x, 0, GFX_LCD_WIDTH - BLOCK_SIZE, 0);
        DecrementWrap(position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);
        IncrementWrap(position.y, 0, GFX_LCD_HEIGHT - BLOCK_SIZE, 0);

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

        uint8_t previousColor = gfx_SetColor(0);
        gfx_FillRectangle_NoClip(segment.position.x, segment.position.y, BLOCK_SIZE, BLOCK_SIZE);
        gfx_SetColor(previousColor);

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

        if (isPartial)
        {
            uint8_t previousColor = gfx_SetColor(0);
            gfx_FillRectangle_NoClip(m_tail.position.x, m_tail.position.y, BLOCK_SIZE, BLOCK_SIZE);
            gfx_SetColor(previousColor);
        }

        DrawTile(snakeTiles, m_tail.position, snakeTile, transformA, transformB);
    }
}

bool Snake::CheckCollision(const Vector2D<int24_t>* other) const
{
    const Vector2D<int24_t> testPosition = other ? *other : m_head.position;

    if (m_snakeType == SnakeType::classic && !(gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT
    ))) return true;

    if (gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        m_tail.position.x, m_tail.position.y, BLOCK_SIZE, BLOCK_SIZE
    )) return true;

    if (m_segmentLength == 0)
        return false;

    if (m_segmentLength == 1)
    {
        if (gfx_CheckRectangleHotspot(
            testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
            m_segments[0].position.x, m_segments[0].position.y, BLOCK_SIZE, BLOCK_SIZE
        )) return true;
    }

    for (size_t i = 1; i < m_segmentLength; ++i)
    {
        if (CheckCollisionSegments(testPosition, m_segments[i - 1], m_segments[i]))
        {
            return true;
        }
    }

    if (CheckCollisionSegments(testPosition, m_segments[m_segmentLength - 1], m_tail))
    {
        return true;
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

Snake::SnakeType Snake::SetType(SnakeType type)
{
    const Snake::SnakeType oldType = m_snakeType;
    m_snakeType = type;
    return oldType;
}

inline void Snake::DrawTile(gfx_sprite_t* snakeTiles, Vector2D<int24_t> position, Vector2D<uint8_t> tile, gfx_sprite_t* (*transformA)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict), gfx_sprite_t* (*transformB)(const gfx_sprite_t* __restrict, gfx_sprite_t* __restrict))
{
    if (!((0 <= position.x && position.x <= (GFX_LCD_WIDTH - BLOCK_SIZE)) &&
        (0 <= position.y && position.y <= (GFX_LCD_HEIGHT - BLOCK_SIZE)))
        ) return;
    
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
        DrawTile(snakeTiles, position, Vector2D<uint8_t>(1, 0), IS_HORIZONTAL(lineDirection) ? gfx_RotateSpriteC : nullptr);
        position += step;
    }
}

inline bool Snake::CheckCollisionSegments(const Vector2D<int24_t>& testPosition, const SnakeSegment& a, const SnakeSegment& b) const
{
    if (gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        b.position.x, b.position.y, BLOCK_SIZE, BLOCK_SIZE
    )) return true;

    Rectangle<int24_t> aRect{
        b.position,
        { BLOCK_SIZE, BLOCK_SIZE }
    };

    bool checkPrevious = false; /* Only check if the rects are different (e.g. wrapped) */
    Rectangle<int24_t> bRect{
        b.position,
        { BLOCK_SIZE, BLOCK_SIZE }
    };

    switch (b.direction)
    {
    case Direction::Up:
        if (a.position.y < b.position.y)
        {
            aRect.position.y = a.position.y;
        }
        else
        {
            aRect.position.y = 0;
            bRect.position.y = a.position.y + BLOCK_SIZE;
            bRect.size.y = GFX_LCD_HEIGHT - bRect.position.y;
            checkPrevious = true;
        }

        aRect.size.y = b.position.y - aRect.position.y;

        break;
    case Direction::Left:
        if (a.position.x < b.position.x)
        {
            aRect.position.x = a.position.x;
        }
        else
        {
            aRect.position.x = 0;
            bRect.position.x = a.position.x + BLOCK_SIZE;
            bRect.size.x = GFX_LCD_WIDTH - bRect.position.x;
            checkPrevious = true;
        }

        aRect.size.x = b.position.x - aRect.position.x;

        break;
    case Direction::Down:
        aRect.position.y = b.position.y + BLOCK_SIZE;

        if (a.position.y > b.position.y)
        {
            aRect.size.y = a.position.y - aRect.position.y;
        }
        else
        {
            aRect.size.y = GFX_LCD_HEIGHT - a.position.y;
            bRect.position.y = 0;
            bRect.size.y = a.position.y - bRect.position.y;
            checkPrevious = true;
        }

        break;
    case Direction::Right:
        aRect.position.x = b.position.x + BLOCK_SIZE;

        if (a.position.x > b.position.x)
        {
            aRect.size.x = a.position.x - aRect.position.x;
        }
        else
        {
            aRect.size.x = GFX_LCD_WIDTH - a.position.x;
            bRect.position.x = 0;
            bRect.size.x = a.position.x - bRect.position.x;
            checkPrevious = true;
        }

        break;
    }

    if (gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        aRect.position.x, aRect.position.y, aRect.size.x, aRect.size.y
    )) return true;

    if (gfx_CheckRectangleHotspot(
        testPosition.x, testPosition.y, BLOCK_SIZE, BLOCK_SIZE,
        bRect.position.x, bRect.position.y, bRect.size.x, bRect.size.y
    )) return true;

    return false;
}
