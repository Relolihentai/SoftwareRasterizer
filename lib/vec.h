#ifndef _VEC_H_
#define _VEC_H_

#include <cmath>
#include <ostream>

template <typename T>
struct Vec2
{
    union 
    {
        struct
        {
            T u, v;
        };
        struct
        {
            T x, y;
        };
        T raw[2];
    };
    Vec2() : u(0), v(0) {}
    Vec2(T _u, T _v) : u(_u), v(_v) {}
    inline Vec2<T> operator+(const Vec2<T>& V) const
    {
        return Vec2<T>(u + V.u, v + V.v);
    }
    inline Vec2<T>& operator+=(const Vec2<T>& V)
    {
        x += V.x;
        y += V.y;
        return *this;
    }
    inline Vec2<T> operator-(const Vec2<T>& V) const
    {
        return Vec2<T>(u - V.u, v - V.v);
    }
    inline Vec2<T> operator*(float f) const
    {
        return Vec2<T>(u * f, v * f);
    }
    inline T& operator[](int index)
    {
        return raw[index];
    }
    //有时候类会被声明为const的，就只能用const函数
    inline const T& operator[](int index) const
    {
        return raw[index];
    }
    
    template<typename >
    friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <typename T>
struct Vec3
{
    union 
    {
        struct
        {
            T x, y, z;
        };
        struct
        {
            T ivert, iuv, inorm;
        };
        T raw[3];
    };
    Vec3() : x(0), y(0), z(0) {}
    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    inline Vec3<T> operator^(const Vec3<T>& v) const
    {
        return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    inline Vec3<T> operator+(const Vec3<T>& v) const
    {
        return Vec3<T>(x + v.x, y + v.y, z + v.z);
    }
    inline Vec3<T>& operator+=(const Vec3<T>& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    inline Vec3<T>& operator+=(const T f)
    {
        x += f;
        y += f;
        z += f;
        return *this;
    }
    inline Vec3<T> operator-(const Vec3<T>& v) const
    {
        return Vec3<T>(x - v.x, y - v.y, z - v.z);
    }
    inline Vec3<T> operator*(const T f) const
    {
        return Vec3<T>(x * f, y * f, z * f);
    }
    inline Vec3<T> operator/(const T f) const
    {
        return Vec3<T>(x / f, y / f, z / f);
    }
    inline Vec3<T> operator/=(const T f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }
    inline T& operator[](int index)
    {
        return raw[index];
    }
    //有时候类会被声明为const的，就只能用const函数
    inline const T& operator[](int index) const
    {
        return raw[index];
    }
    inline T operator*(const Vec3<T>& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    T norm() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vec3<T>& normalize(T l = 1)
    {
        *this = (*this)*(l / norm());
        return *this;
    }

    template<typename >
    friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template <typename T> 
std::ostream& operator<<(std::ostream& s, Vec2<T>& v)
{
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <typename T> 
std::ostream& operator<<(std::ostream& s, Vec3<T>& v)
{
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

#endif