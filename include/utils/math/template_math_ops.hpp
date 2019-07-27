#pragma once
#include "template_vector.hpp"
#include "template_matrix.hpp"
#include <assert.h>

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
    matrix<4, 4, T> temp = {T(1), T(0), T(0), v[0],
                            T(0), T(1), T(0), v[1],
                            T(0), T(0), T(1), v[2],
                            T(0), T(0), T(0), T(1)};
    return (matmul(m, temp));
}

template <typename T>
matrix<4, 4, T> lookAt(vec<3, T> &eye, vec<3, T> &center, vec<3, T> &up)
{
    
    vec<3, T> w = center.normalize();
    vec<3, T> temp = up.cross(w);
    vec<3, T> u = temp.normalize();
    vec<3, T> v = w.cross(u);

    matrix<4, 4, T> viewMatrix = {u[0], u[1], u[2], -eye[0],
                                  v[0], v[1], v[2], -eye[1],
                                  w[0], w[1], w[2], -eye[2],
                                  0, 0, 0, 1};
    return viewMatrix;
}