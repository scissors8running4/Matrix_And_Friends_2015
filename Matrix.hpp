#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

//*************************************************************************//

#include <vector>
#include <algorithm>

//*************************************************************************//
//
// rectangular matrix class used for matrix algebra
//
//*************************************************************************//

template<typename T>
class Matrix {
    
private:
    
    // rows, columns
    size_t n_rows, n_columns, total_elements;
    
    std::vector<T> elements;

public:
    
//*************************************************************************//
//
// constructors, destructor, assignment operators and member functions
//
//*************************************************************************//
    
    // basic constructors
    Matrix(size_t, size_t);
    
    Matrix(size_t);
    
    Matrix();

    // initializer list constructor
    Matrix(size_t, size_t, std::initializer_list<T>);

    // move and copy constructors
    Matrix(Matrix&&) = default;

    Matrix(const Matrix&) = default;

    // move and copy assigners
    Matrix& operator = (Matrix&&) = default;

    Matrix& operator = (const Matrix&) = default;

    // destructor
    ~Matrix() = default;
    
    // show number of rows
    size_t rows() const;
    
    // show number of columns
    size_t columns() const;
    
    // reference element access operator
    T& operator () (size_t, size_t);
    
    // constant element access operator
    T operator () (size_t, size_t) const;
    
    // row exchange function
    void exchange_rows(size_t, size_t);

    // test for equality
    bool operator == (const Matrix&);

    // test for inequality
    bool operator != (const Matrix&);
    
    // += operator for matrices
    Matrix& operator += (const Matrix&);
    
    // -= operator for matrices
    Matrix& operator -= (const Matrix&);

    // matrix slice
    Matrix slice(size_t, size_t, size_t, size_t);

    // + operator for matrices
    Matrix operator + (const Matrix&) const;
    
    // - operator for matrices
    Matrix operator - (const Matrix&) const;
    
    // * operator for matrices
    Matrix operator * (const Matrix&) const;
    
};

//*************************************************************************//

//*************************************************************************//
//
// constructors, destructor, assignment operators and member functions
//
//*************************************************************************//

// basic constructors
template<typename T>
Matrix<T>::Matrix(size_t r, size_t c)
	
	: n_rows(r)
	, n_columns(c)
	, total_elements(r * c)
	, elements(r * c) {
    
    if(n_rows == 0 || n_columns == 0) {
        
        throw std::out_of_range("matrix size of 0x0 not allowed");
    }
}

template<typename T>
Matrix<T>::Matrix(size_t r)

	: n_rows(r)
	, n_columns(1)
	, total_elements(r)
	, elements(r) {

    if(n_rows == 0) {
        
        throw std::out_of_range("matrix row number of 0 not allowed");
    }
}

template<typename T>
Matrix<T>::Matrix()
    
	: n_rows(1)
	, n_columns(1)
	, total_elements(1)
	, elements(1) {}

// initializer list constructor
    template<typename T>
Matrix<T>::Matrix(size_t r, size_t c,
                  std::initializer_list<T> l)

    : n_rows(r)
    , n_columns(c)
    , total_elements(r * c)
    , elements(l) {
    
    if(total_elements != elements.size()) {
        
        throw std::out_of_range("dimension resolution error");
    }
}

// show number of rows
template<typename T>
size_t Matrix<T>::rows() const { return n_rows; }

// show number of columns
template<typename T>
size_t Matrix<T>::columns() const { return n_columns; }

// reference element access operator
template<typename T>
T& Matrix<T>::operator () (size_t r, size_t c) {
    
    return elements[(r * n_columns) + c];
}

// constant element access operator
template<typename T>
T Matrix<T>::operator () (size_t r, size_t c) const {
    
    return elements[(r * n_columns) + c];
}

// row exchange function
template<typename T>
void Matrix<T>::exchange_rows(size_t row_a, size_t row_b) {
    
    if(row_a > n_rows || row_b > n_rows) {
        
        throw std::out_of_range("index out of bounds");
    }
    
    size_t index_a = row_a * n_columns;
    size_t index_b = row_b * n_columns;

    for(size_t j = 0; j < n_columns; ++j) {
        
        std::swap(elements[index_a + j], elements[index_b + j]);
    }
}

// comparison operator for equality
template<typename T>
bool Matrix<T>::operator == (const Matrix<T>& M) {

    if(n_rows != M.n_rows || n_columns != M.n_columns) {

        return false;
    
    } else {

        bool equal = true;

        for(size_t i = 0; i < total_elements; ++i) {

        	if(elements[i] != M.elements[i]) {

        		equal = false;
        		i = total_elements;
        	}
        }

        return equal;
    }
}

// comparison operator for inequality
template<typename T>
bool Matrix<T>::operator != (const Matrix<T>& M) {

    if(n_rows != M.n_rows || n_columns != M.n_columns) {

        return true;
    
    } else {

        bool not_equal = false;

        for(size_t i = 0; i < total_elements; ++i) {

            if(elements[i] != M.elements[i]) {

                not_equal = true;
                i = total_elements;
            }
        }

        return not_equal;
    }
}

// += operators for matrices
template<typename T>
Matrix<T>& Matrix<T>::operator += (const Matrix<T>& M) {
    
    if(n_rows != M.n_rows || n_columns != M.n_columns) {
        
        throw std::out_of_range("dimensions do not match");
    }
    
    for(size_t i = 0; i < total_elements; ++i) {

    	elements[i] += M.elements[i];
	}
    
    return *this;
}

// -= operators for matrices
template<typename T>
Matrix<T>& Matrix<T>::operator -= (const Matrix<T>& M) {
    
    if(n_rows != M.n_rows || n_columns != M.n_columns) {
        
        throw std::out_of_range("dimensions do not match");
    }
    
    for(size_t i = 0; i < total_elements; ++i) {

    	elements[i] -= M.elements[i];
	}
    
    return *this;
}

// matrix slice
template<typename T>
Matrix<T> Matrix<T>::slice(size_t row_a, size_t row_b,
                        size_t column_a, size_t column_b) {
    
    size_t row_range = row_b - row_a;
    size_t column_range = column_b - column_a;

    Matrix<T> MP(row_range, column_range);

    size_t MP_index, index;

    for(size_t i = 0; i < row_range; ++i) {

        MP_index = i * MP.n_columns;
        index = (row_a + i) * n_columns;

        for(size_t j = 0; j < column_range; ++j) {

            MP.elements[MP_index + j] = elements[index + column_a + j];
        }
    }
    
    return MP;
}

// + operator for matrices
template<typename T>
Matrix<T> Matrix<T>::operator + (const Matrix<T>& M) const {
    
    Matrix<T> MS = *this;
    MS += M;

    return MS;
}

// - operator for matrices
template<typename T>
Matrix<T> Matrix<T>::operator - (const Matrix<T>& M) const {
    
    Matrix<T> MD = *this;
    MD -= M;

    return MD;
}

// * operator for matrices
template<typename T>
Matrix<T> Matrix<T>::operator * (const Matrix<T>& M) const {
    
    if(n_columns != M.n_rows) {
        
        throw std::out_of_range("incorrect dimensions for a product");
    }
    
    Matrix<T> MP(n_rows, M.n_columns);
    
    size_t i_end = n_rows;
    size_t k_end = n_columns;
    size_t j_end = M.n_columns;

    size_t MP_index, index, M_index;

    for(size_t i = 0; i < i_end; ++i) {

        MP_index = j_end * i;
        index = k_end * i;

        for(size_t k = 0; k < k_end; ++k) {

            M_index = j_end * k;

            for(size_t j = 0; j < j_end; ++j) {

                MP.elements[MP_index + j] +=
                    elements[index + k] *
                        M.elements[M_index + j];
            }
        }
    }

    return MP;
}

//*************************************************************************//

#endif // MATRIX_H_INCLUDED
