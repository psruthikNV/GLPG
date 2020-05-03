#pragma once
#include <array>
#include <assert.h>
#include <math.h>

namespace GLPG {

    template <std::size_t L, typename T>
        class vec
        {
            public:
                static constexpr std::size_t dims = L;
                std::array<T, L> vals = {};

                /* Constructors */
                vec();
                vec(T val);
                vec(std::initializer_list<T> values);

                /* Operators */
                vec operator+(vec &) const;
                vec operator-(vec &) const;
                vec operator/(T val) const;
                T operator[](std::size_t idx) const;
                T& operator[](std::size_t idx);
                friend std::ostream& operator<< (std::ostream& os, const vec<L, T> &v);

                /* Getters */
                inline constexpr std::size_t getLength() const { return dims;  }

                /* Functions */
                vec normalize();
                T length();
                vec<3, T> cross(vec<3, T>&);
                T dot(vec<L, T>&);
        };

    template <std::size_t L, typename T>
        vec<L, T>::vec()
        {
            for (std::size_t i = 0; i < dims; i++)
                vals[i] = 0;
        }

    template <std::size_t L, typename T>
        vec<L, T>::vec(T val)
        {
            for (std::size_t i = 0; i < dims; i++) {
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
            assert(idx < dims);
            return vals[idx];

        }

    template <std::size_t L, typename T>
        T& vec<L, T>::operator[](std::size_t idx)
        {
            assert(idx < dims);
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

    template <std::size_t L, typename T>
        vec<L, T> vec<L, T>::operator+(vec<L, T> &v1) const
        {
            vec<L, T> temp;

            for (std::size_t i = 0; i < L; i++) {
                temp[i] = vals[i] + v1[i];
            }

            return temp;
        }

    template <std::size_t L, typename T>
        vec<L, T> vec<L, T>::operator-(vec<L, T> &v1) const
        {
            vec<L, T> temp;

            for (std::size_t i = 0; i < L; i++) {
                temp[i] = vals[i] - v1[i];
            }

            return temp;
        }

    template <std::size_t L, typename T>
        vec<L, T> vec<L, T>::operator/(T val) const
        {
            vec<L, T> temp;

            for (std::size_t i = 0; i < L; i++) {
                temp[i] = vals[i] / val;
            }

            return temp;
        }

    template <std::size_t L, typename T>
        T vec<L, T>::length()
        {
            T temp = 0;

            for (std::size_t i = 0; i < L; i++) {
                temp += vals[i] * vals[i];
            }

            return static_cast<T>(sqrt(temp));
        }

    template <std::size_t L, typename T>
        vec<L, T> vec<L, T>::normalize()
        {
            return (*this / length());
        }

    template <std::size_t L, typename T>
        vec<3, T> vec<L, T>::cross(vec<3, T> &v1)
        {
            vec<3, T> temp = {vals[1]*v1.vals[2] - vals[2]*v1.vals[1],
                vals[2]*v1.vals[0] - vals[0]*v1.vals[2],
                vals[0]*v1.vals[1] - vals[1]*v1.vals[0]};
            return temp;
        }
    template <std::size_t L, typename T>
    T vec<L, T>::dot(vec<L, T> &v1)
    {
        T rv = T(0);
        for (std::size_t i = 0; i < L; i++) {
            rv += vals[i] * v1.vals[i];
        }
        return rv;
    }

    using vec4_i = vec<4, int>;
    using vec4_f = vec<4, float>;
    using vec4_d = vec<4, double>;
    using vec3_f = vec<3, float>;
    using vec3_i = vec<3, int>;
    using vec3_d = vec<3, double>;
}
