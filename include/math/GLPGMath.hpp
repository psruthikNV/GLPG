#pragma once
#include "GLPGVector.hpp"
#include "GLPGMatrix.hpp"
#include <assert.h>
#include <cmath>

constexpr float PI_F = 3.14159265358979F;

namespace GLPG {

    template <std::size_t R, std::size_t C, std::size_t X, typename T>
        inline matrix<R, X, T> matmul(matrix<R, C, T>& m1, matrix<C, X, T>& m2)
        {
            assert(m1.cols == m2.rows);
            matrix<m1.rows, m2.cols, T> rv(0);

            for (std::size_t i = 0; i < m1.rows; i++)
                for (std::size_t j = 0; j < m1.cols; j++)
                    for (std::size_t k = 0; k < m2.rows; k++)
                        rv[i][j] += m1[i][k] * m2[k][j];
            return rv;
        }

    template <std::size_t R, std::size_t C, std::size_t L, typename T>
        inline vec<R, T> matmul(matrix<R, C, T>& m1, vec<L, T>& v1)
        {
            assert(m1.cols == v1.getLength());
            vec<R, T> rv(0);

            for (std::size_t i = 0; i < m1.rows; i++)
                for (std::size_t k = 0; k < v1.getLength(); k++)
                    rv[i] += m1[i][k] * v1[k];

            return rv;
        }

    template <typename T>
        matrix<4, 4, T> translate(matrix<4, 4, T> &m, vec<3, T> &v )
        {
            matrix<4, 4, T> rv = {T(1), T(0), T(0), v[0],
                T(0), T(1), T(0), v[1],
                T(0), T(0), T(1), v[2],
                T(0), T(0), T(0), T(1)};
            return (matmul(m, rv));
        }

    template <typename T>
        matrix<4, 4, T> lookAt(vec<3, T> &eye, vec<3, T> &center, vec<3, T> &up)
        {
            vec<3, T> w = (eye - center).normalize();
            vec<3, T> temp = up.cross(w);
            vec<3, T> u = temp.normalize();
            vec<3, T> v = w.cross(u);

            matrix<4, 4, T> rv = {u[0], u[1], u[2], -u.dot(eye),
                v[0], v[1], v[2], -v.dot(eye),
                w[0], w[1], w[2], -w.dot(eye),
                0, 0, 0, 1};
            return rv;
        }

    template <typename T>
        matrix<4, 4, T> frustum(T l, T r, T b, T t, T n, T f)
        {
            assert(n != 0);

           T A = (r + l) / (r - l);
           T B = (t + b) / (t - b);
           T C = (f + n) / (n - f);
           T D = (2 * f * n) / (n - f);

            matrix<4, 4, T> rv = {(2 * n) / (r - l), 0, A, 0,
                0, (2 * n) / (t - b), B, 0,
                0, 0, C, D,
                0, 0, -1, 0};

            return rv;

        }

    template <typename T>
        matrix<4, 4, T> gluPerspective(T fovy, T aspect, T zNear, T zFar)
        {
            T f = 1.0f / tan(((fovy * PI_F) / 180) / 2);
            
            matrix<4, 4, T> rv = {f / aspect, 0, 0, 0,
                                  0, f, 0, 0,
                                  0, 0, (zFar + zNear) / (zNear - zFar), (2 * zFar * zNear)/ (zNear - zFar),
                                  0, 0, -1, 0};
            return rv;
        }
}
