#pragma once
#include <array>
#include <assert.h>

template <std::size_t L, typename T>
class vec
{
    public:
        static constexpr std::size_t length = L;
        std::array<T, L> vals = {};

        /* Constructors */
        vec();
        vec(T val);
        vec(std::initializer_list<T> values);

        /* Operators */
        T operator[](std::size_t idx) const;
        T& operator[](std::size_t idx);
        friend std::ostream& operator<< (std::ostream& os, const vec<L, T> &v);

        /* Getters */
        inline constexpr std::size_t getLength() const { return length;  }
};

template <std::size_t L, typename T>
vec<L, T>::vec()
{
    for (std::size_t i = 0; i < length; i++)
        vals[i] = 0;
}

template <std::size_t L, typename T>
vec<L, T>::vec(T val)
{
    for (std::size_t i = 0; i < length; i++) {
        vals[i] = val;
    }
}

template <std::size_t L, typename T>
vec<L, T>::vec(std::initializer_list<T> values)
{
    std::size_t i = 0;
    for (auto val : values) {
        vals[i++] = val;
    }
}

template <std::size_t L, typename T>
T vec<L, T>::operator[](std::size_t idx) const
{
    assert(idx < length);
    return vals[idx];

}

template <std::size_t L, typename T>
T& vec<L, T>::operator[](std::size_t idx)
{
    assert(idx < length);
    return vals[idx];
}

template <std::size_t L, typename T>
std::ostream& operator<<(std::ostream& os, vec<L, T> &v)
{
    os << std::endl;
    for (std::size_t i = 0; i < v.getLength(); i++)
        os << v[i] << std::endl;
    return os;
}

using vec4_i = vec<4, int>;
using vec4_f = vec<4, float>;
using vec4_d = vec<4, double>;
using vec3_f = vec<3, float>;
