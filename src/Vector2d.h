#pragma once

#include <tice.h>
#include <math.h>
#include <graphx.h>

#define MAX_ITERATIONS 5

template <typename T>
class Vector2D
{
    T abs(T a) const
    {
        if (a >= 0)
            return a;

        return -a;
    }

    T min(T a, T b) const
    {
        if (a > b)
            return b;

        return a;
    }

    T max(T a, T b) const
    {
        if (a < b)
            return b;

        return a;
    }

    public:
        Vector2D();
        Vector2D(T x, T y);

        template <typename U>
        Vector2D(const Vector2D<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        Vector2D<T> operator+(const Vector2D& other) const
        {
            return Vector2D(x + other.x, y + other.y);
        }

        Vector2D<T> operator+(T scalar) const
        {
            return Vector2D(x + scalar, y + scalar);
        }

        Vector2D<T> operator-(const Vector2D& other) const
        {
            return Vector2D(x - other.x, y - other.y);
        }

        Vector2D<T> operator*(T scalar) const
        {
            return Vector2D(x * scalar, y * scalar);
        }

        Vector2D<T> operator/(T scalar) const
        {
            return Vector2D(x / scalar, y / scalar);
        }

        Vector2D<T>& operator+=(const Vector2D& other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }

        Vector2D<T>& operator-=(const Vector2D& other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }

        Vector2D<T>& operator*=(T scalar)
        {
            x *= scalar;
            y *= scalar;

            return *this;
        }

        Vector2D<T>& operator/=(T scalar)
        {
            x /= scalar;
            y /= scalar;

            return *this;
        }

        bool operator==(const Vector2D& other) const
        {
            return (x == other.x) && (y == other.y);
        }

        bool operator!=(const Vector2D& other) const
        {
            return (x != other.x) || (y != other.y);
        }

        T Dot(const Vector2D& other) const;
        T Magnitude() const;
        Vector2D<T> Normalized(T normalValue) const;
        Vector2D<T> Absolute() const;
        Vector2D<T> Min(const Vector2D& other) const;
        Vector2D<T> Max(const Vector2D& other) const;

    public:
        T x;
        T y;

    private:
        T Sqrt(T value);
};

template<typename T>
Vector2D<T>::Vector2D()
{
    x = T(0);
    y = T(0);
}

template<typename T>
Vector2D<T>::Vector2D(T x, T y)
{
    this->x = x;
    this->y = y;
}

template<typename T>
T Vector2D<T>::Dot(const Vector2D& other) const
{
    return x * other.x + y * other.y;
}

template<typename T>
T Vector2D<T>::Magnitude() const
{
    return Sqrt(x * x + y * y);
}

template<typename T>
Vector2D<T> Vector2D<T>::Normalized(T normalValue) const
{
    Vector2D<T> temp = Vector2D(x, y);
    T magnitude = temp.Magnitude();

    if (normalValue == ((T)1))
    {
        temp.x /= magnitude;
        temp.y /= magnitude;
    }
    else
    {
        temp.x = (temp.x * normalValue) / magnitude;
        temp.y = (temp.y * normalValue) / magnitude;
    }

    return temp;
}

template<typename T>
inline Vector2D<T> Vector2D<T>::Absolute() const
{
    return Vector2D<T>(
        Vector2D::abs(x),
        Vector2D::abs(y)
    );
}

template<typename T>
inline Vector2D<T> Vector2D<T>::Min(const Vector2D& other) const
{
    return Vector2D<T>(
        Vector2D::min(x, other.x),
        Vector2D::min(y, other.y)
    );
}

template<typename T>
inline Vector2D<T> Vector2D<T>::Max(const Vector2D& other) const
{
    return Vector2D<T>(
        Vector2D::max(x, other.x),
        Vector2D::max(y, other.y)
    );
}

template<typename T>
T Vector2D<T>::Sqrt(T value)
{
    T result = value / ((T)2);

    for (size_t i = 0; i < MAX_ITERATIONS; i++)
    {
        T next = (result + (value / result)) / ((T)2);

        if (abs(next - result) <= 0)
        {
            return next;
        }

        result = next;
    }

    return result;
}

template <typename T>
T Sign(T x)
{
    if (x == T(0))
        return T(0);

    return (x > T(0)) ? T(1) : T(-1);
}