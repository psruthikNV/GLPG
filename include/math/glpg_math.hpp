#pragma once
#include "glpg_vector.hpp"
#include "glpg_matrix.hpp"
#include <assert.h>
#include <cmath>

constexpr float PI_F = 3.14159265358979F;

namespace glpg {

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

    /*
     * Look-At matrix :
     *
     * The Look-At matrix takes in three glpg::vec3 parameters.
     *
     * 1. Eye - The position of the camera/eye in world coordinates
     * 2. Center / View vector - The vector describing the direction in which
     *                           the camera/eye is looking at
     * 3. Up - The vector describing the up direction of the camera/eye
     *
     * The purpose of the Look-At matrix is to provide a transformation matrix that is the inverse
     * of the Eye and Center parameters. This is because in CG there is no actual Camera object
     * and there is only a construct of a camera. Instead of placing the camera in the world and
     * getting its viewpoint we keep the camera stationary and move the world around it.
     *
     * For the above to happen, we need to describe the camera's location and a coordinate system
     * specific to the camera.
     *
     * This coordinate system is setup by the Look-At matrix.
     *
     * We take the view vector which originates from the eye position and perform a cross with the Up vector.
     * This will give us a new vector which is perpendicular to both the Up and View vector.
     *
     * This new vector will represent the second orthogonal axis of the camera coordinate system (the first being
     * the view vector).
     *
     * Now the cross product between the view vector and the temporary vector is performed to get the vector
     * representing the final axis of the camera coordinate system
     */
    template <typename T>
        matrix<4, 4, T> lookAt(vec<3, T> &eye, vec<3, T> &center, vec<3, T> &up)
        {
            vec<3, T> w = (center - eye).normalize();
            vec<3, T> temp = up.cross(w);
            vec<3, T> u = temp.normalize();
            vec<3, T> v = w.cross(u);

            matrix<4, 4, T> rv = {u[0], u[1], u[2], -eye[0],
                v[0], v[1], v[2], -eye[1],
                w[0], w[1], w[2], -eye[2],
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
    //TODO - Understand the relation between gluPerspective and glFrustum
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
