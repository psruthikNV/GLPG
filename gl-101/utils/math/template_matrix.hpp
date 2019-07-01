/*
 * Bit of a primer on C++ references as I keep forgetting them!
 *
 * A Reference variable does not occupy any extra memory and its
 * address is the same as the variable it is referring to.
 *
 * Reference can never be null and always need to be initialized.
 * Functions returning Reference variables implicitly convert the
 * return value to reference.
*/

#pragma once
#include <array>
#include <initializer_list>

template <std::size_t R, std::size_t C, typename T>
class matrix
{
public:
	/* TODO: The below members must be made private */
	static constexpr std::size_t rows = R;
	static constexpr std::size_t cols = C;
	static constexpr std::size_t size = C * R;

	/* Constructors */
	matrix(void);
	matrix(T val);
	matrix(std::initializer_list<T> vals);

	/* Operators */
	friend std::ostream& operator<<(std::ostream& os, const matrix& m);
	std::array<T, C> operator[](std::size_t idx) const;
	std::array<T, C>& operator[](std::size_t idx);

	/* Getters. Useless untill the constexpr members are made private */
	inline constexpr std::size_t numRows() const;
	inline constexpr std::size_t numCols() const;
	inline constexpr std::size_t numVals() const;

	/* Storage backing 
	 * TODO: Currently, the assumption is that all the
	 * values in the arrays are allocated contigiously.
	 * Verify this assumption 
	 * TODO: Must be made private
	 */
	std::array<T, cols> row[rows]{};

	/* Functions */
	T* data();
};

template <std::size_t R, std::size_t C, typename T>
matrix<R, C, T>::matrix()
{
	for (std::size_t i = 0; i < rows; i++)
		row[i][i] = T(1);
}

template <std::size_t R, std::size_t C, typename T>
matrix<R, C, T>::matrix(T val)
{
	for (std::size_t i = 0; i < rows; i++)
		row[i][i] = val;
}

template <std::size_t R, std::size_t C, typename T>
matrix<R, C, T>::matrix(std::initializer_list<T> vals)
{
	std::size_t i = 0;
	std::size_t j = 0;

	for (auto val : vals) {
		row[i][j++] = val;
		i = ((j == C) && !(j = 0)) ? ++i : i;
	}
}

template <std::size_t R, std::size_t C, typename T>
std::ostream& operator<<(std::ostream& os, matrix<R, C, T>& m)
{
	os << std::endl;
	for (int i = 0; i < R; i++) {
		for (int j = 0; j < C; j++)
			os << m.row[i][j] << " ";
		os << std::endl;
	}
	return os;
}

template <std::size_t R, std::size_t C, typename T>
std::array<T, C>& matrix<R, C, T>::operator[](std::size_t idx)
{
	return row[idx];
}

template <std::size_t R, std::size_t C, typename T>
std::array<T, C> matrix<R, C, T>::operator[](std::size_t idx) const
{
	return row[idx];
}

template <std::size_t R, std::size_t C, typename T>
constexpr std::size_t matrix<R, C, T>::numRows() const
{
	return rows;
}

template <std::size_t R, std::size_t C, typename T>
constexpr std::size_t matrix<R, C, T>::numCols() const
{
	return cols;
}

template <std::size_t R, std::size_t C, typename T>
constexpr std::size_t matrix<R, C, T>::numVals() const
{
	return size;
}

template <std::size_t R, std::size_t C, typename T>
T* matrix<R, C, T>::data()
{
	return row[0].data();
}

using mat4x4_i = matrix<4, 4, int>;
using mat4x4_f = matrix<4, 4, float>;
using mat4x4_d = matrix<4, 4, double>;