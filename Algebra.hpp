#ifndef ALGEBRA_H_INCLUDED
#define ALGEBRA_H_INCLUDED

//*************************************************************************//

#include <complex>

#include "Matrix.hpp"

//*************************************************************************//
//
// matrix algebra class
//
//*************************************************************************//

class Algebra {
    
private:
    
    // multiplier for determinants used for sign changes
    int determinant_multiplier;

    // dimension comparison functions
    template<typename T>
    bool is_square(const Matrix<T>&);
    
    template<typename T>
    bool same_dimensions(const Matrix<T>& M1, const Matrix<T>&);
    
    // zero value test functions
    template<typename T>
    bool is_zero(const std::complex<T>&);
    
    template<typename T>
    bool is_zero(const T&);
    
    // make a number zero
    template<typename T>
    void make_zero(std::complex<T>&);

    template<typename T>
    void make_zero(T&);

    // make a number one
    template<typename T>
    void make_one(std::complex<T>&);

    template<typename T>
    void make_one(T&);

    // partial sort based on main diagonal entry
    // ensure that it is most likely a non-zero value
    template<typename T>
    void main_diagonal_partial_sort(size_t row_start, Matrix<T>&);

    // form zeros under pivots
    template<typename T>
    void zeros_under_pivots(Matrix<T>&);

    // form zeros above pivots
    template<typename T>
    void zeros_above_pivots(Matrix<T>&);

    // divide rows by pivots
    template<typename T>
    void divide_rows_by_pivots(Matrix<T>&);

    // find out if main diagonal has a zero
    template<typename T>
    bool has_a_main_diagonal_zero(const Matrix<T>&);

public:

    // constructor
    Algebra();

    // move and copy constructors
    Algebra(Algebra&&) = default;

    Algebra(const Algebra&) = default;

    // move and copy assigners
    Algebra& operator = (Algebra&&) = default;

    Algebra& operator = (const Algebra&) = default;

    // destructor
    ~Algebra() = default;

    // determine reduced-row echelon form
    template<typename T>
    Matrix<T> rref(const Matrix<T>&);

    // determine inverse if it exists
    template<typename T>
    Matrix<T> inverse(const Matrix<T>&);

    // find determinant of a complex matrix
    template<typename T>
    std::complex<T> determinant(const Matrix<std::complex<T>>&);

    // find determinant of a matrix
    template<typename T>
    T determinant(const Matrix<T>&);

};

//*************************************************************************//

//*************************************************************************//
//
// private function area
//
//*************************************************************************//

// dimension comparison functions
template<typename T>
bool Algebra::is_square(const Matrix<T>& M) {

    if(M.rows() == M.columns()) { return true; }
    else { return false; }
}

template<typename T>
bool Algebra::same_dimensions(const Matrix<T>& M1, const Matrix<T>& M2) {

    if(M1.rows() == M2.rows() && M1.columns() == M2.columns()) { return true; }
    else { return false; }
}

// zero value test functions
template<typename T>
inline bool Algebra::is_zero(const std::complex<T>& a) {

    if(a.real() == 0 && a.imag() == 0) { return true; }
    else { return false; }
}

template<typename T>
inline bool Algebra::is_zero(const T& a) {

    if(a == 0) { return true; }
    else { return false; }
}

// make a number zero
template<typename T>
inline void Algebra::make_zero(std::complex<T>& a) { a.real(0); a.imag(0); }

template<typename T>
inline void Algebra::make_zero(T& a) { a = 0; }

// make a number one
template<typename T>
inline void Algebra::make_one(std::complex<T>& a) { a.real(1); a.imag(0); }

template<typename T>
inline void Algebra::make_one(T& a) { a = 1; }

// partial sort based on main diagonal entry
// ensure that it is most likely a non-zero value
template<typename T>
void Algebra::main_diagonal_partial_sort(size_t row, Matrix<T>& M) {

    size_t last_row = M.rows();
    
    if(is_zero(M(row, row))) {
        
        for(size_t i = row; i < last_row; ++i) {

            if(!Algebra::is_zero(M(i, row))) {
                
                M.exchange_rows(i, row);
                i = last_row;
                
                // flip sign on determinant multiplier
                determinant_multiplier *= -1;
            }
        }
    }
}

// form zeros under pivots
template<typename T>
void Algebra::zeros_under_pivots(Matrix<T>& M) {
    
    // start determinant multiplier at 1
    determinant_multiplier = 1;
    
    size_t last_row = M.rows();
    size_t last_col = M.columns();

    size_t i_end = last_row;
    size_t j_end = last_col;
    
    size_t col;
    size_t right_shift;

    for(size_t row = 0; row != last_row; ++row) {
        
        Algebra::main_diagonal_partial_sort(row, M);
        
        col = row;
        right_shift = 0;

        while(col + right_shift < last_col) {
            
            if(!Algebra::is_zero(M(row, col + right_shift))) {
                
                T pivot = M(row, col + right_shift);

                for(size_t i = 1; row + i != i_end; ++i) {
                    
                    T multiplier = M(row + i, col + right_shift) / pivot;
                    
                    for(size_t j = 1; col + right_shift + j != j_end; ++j) {
                        
                        M(row + i, col + right_shift + j) -=
                            M(row, col + right_shift + j) * multiplier;
                    }
                }
                
                for(size_t i = 1; row + i != i_end; ++i) {
                    
                    Algebra::make_zero(M(row + i, col + right_shift));
                }
                
                right_shift = last_col - col;
                
            } else {
                
                ++right_shift;
            }
        }
    }
}

// form zeros above pivots
template<typename T>
void Algebra::zeros_above_pivots(Matrix<T>& M) {
    
    size_t last_row = M.rows();
    size_t last_col = M.columns();

    size_t j_end = last_col;

    size_t col;
    size_t right_shift;

    for(size_t row = 1; row < last_row; ++row) {
        
        col = row;
        right_shift = 0;

        while(col + right_shift < last_col) {
            
            if(!Algebra::is_zero(M(row, col + right_shift))) {
                
                T pivot = M(row, col + right_shift);

                for(size_t i = 1; row - i + 1 > 0; ++i) {
                    
                    T multiplier = M(row - i, col + right_shift) / pivot;
                    
                    for(size_t j = 1; col + right_shift + j < j_end; ++j) {
                        
                        M(row - i, col + right_shift + j) -=
                            M(row, col + right_shift + j) * multiplier;
                    }
                }
                
                for(size_t i = 1; row - i + 1 > 0; ++i) {
                    
                    Algebra::make_zero(M(row - i, col + right_shift));
                }
                
                right_shift = last_col - col;
                
            } else {
                
                ++right_shift;
            }
        }
    }
}

// divide rows by pivots
template<typename T>
void Algebra::divide_rows_by_pivots(Matrix<T>& M) {
    
    size_t i_end = M.rows();
    size_t j_end = M.columns();

    for(size_t i = 0; i < i_end; ++i) {
        
        for(size_t j = 0; j < j_end; ++j) {
            
            if(!Algebra::is_zero(M(i, j))) {
                
                T divisor = M(i, j);

                for(size_t k = 0; j + k < j_end; ++k) {
                    
                    M(i, j + k) /= divisor;
                }
                
                j = j_end;
            }
        }
    }
}

// find out if main diagonal has a zero
template<typename T>
bool Algebra::has_a_main_diagonal_zero(const Matrix<T>& M) {
    
    size_t last_diagonal;

    if(M.rows() < M.columns()) { last_diagonal = M.rows(); }
    else { last_diagonal = M.columns(); }
    
    bool has_a_zero = false;
    
    for(size_t i = 0; i < last_diagonal; ++i) {
            
        if(Algebra::is_zero(M(i, i))) {

            has_a_zero = true;
            i = last_diagonal;
        }
    }
    
    return has_a_zero;
}

//*************************************************************************//

//*************************************************************************//
//
// public function area
//
//*************************************************************************//

// constructor
Algebra::Algebra() : determinant_multiplier(1) {}

// determine reduced-row echelon form
template<typename T>
Matrix<T> Algebra::rref(const Matrix<T>& M) {
    
    // initialize matrix for row-reduction
    Matrix<T> MR = M;
    
    // form zeros below pivots
    Algebra::zeros_under_pivots(MR);
    
    // form zeros above pivots
    Algebra::zeros_above_pivots(MR);
    
    // divide rows by pivot points
    Algebra::divide_rows_by_pivots(MR);
    
    return MR;
}

// determine inverse if it exists
template<typename T>
Matrix<T> Algebra::inverse(const Matrix<T>& M) {

    size_t i_end = M.rows();
    size_t j_end = M.columns();
    
    // make augmented matrix for modification
    Matrix<T> MA(i_end, 2 * j_end);
    
    for(size_t i = 0; i < i_end; ++i) {

        for(size_t j = 0; j < j_end; ++j) {

            MA(i, j) = M(i, j);
        }
    }
    
    for(size_t i = 0; i < i_end; ++i) {

        for(size_t j = j_end; j< 2 * j_end; ++j) {

            if(i == j - j_end) { Algebra::make_one(MA(i, j)); }
            else { Algebra::make_zero(MA(i, j)); }
        }
    }
    
    // form zeros below main diagonal, sorting is done as zeros are found
    Algebra::zeros_under_pivots(MA);
    
    // if a main diagonal entry is zero, matrix is singular
    if(Algebra::has_a_main_diagonal_zero(MA)) {
        
        throw std::out_of_range("matrix is singular");
        
    } else {
        
        // form zeros above pivots, sorting is done as zeros are found
        Algebra::zeros_above_pivots(MA);
        
        // divide rows by pivot points
        Algebra::divide_rows_by_pivots(MA);
        
        size_t i_end = M.rows();
        size_t j_end = M.columns();

        // copy numbers from right side of MA to MI
        Matrix<T> MI(i_end, j_end);

        for (size_t i = 0; i < i_end; ++i) {

            for (size_t j = 0; j < j_end; ++j) {

                MI(i, j) = MA(i, j + j_end);
            }
        }
        
        return MI;
    }
}

// find determinant of a complex matrix
template<typename T>
std::complex<T> Algebra::determinant(const Matrix<std::complex<T>>& M) {
    
    if(Algebra::is_square(M)) {
        
        // make copy of M for reduction
        Matrix<std::complex<T>> MD = M;

        // complex determinant value
        std::complex<T> det(1, 0);

        // partial reduction
        Algebra::zeros_under_pivots(MD);

        size_t i_end = MD.rows();

        // multiply det by diagonal entries
        for(size_t i = 0; i < i_end; ++i) {
            
            det *= MD(i, i);
        }
        
        // make sure that row-exchanges are accounted for
        det *= determinant_multiplier;
        
        return det;
        
    } else {
        
        std::complex<T> det(0, 0);

        return det;
    }
}

// find determinant of a matrix
template<typename T>
T Algebra::determinant(const Matrix<T>& M) {
    
    if(Algebra::is_square(M)) {
        
        // make copy of M for reduction
        Matrix<T> MD = M;

        // real determinant value
        T det = 1;

        // partial reduction
        Algebra::zeros_under_pivots(MD);

        size_t i_end = MD.rows();

        // multiply det by diagonal entries
        for(size_t i = 0; i < i_end; ++i) {
            
            det *= MD(i, i);
        }
        
        // make sure that row-exchanges are accounted for
        det *= determinant_multiplier;
        
        return det;
        
    } else {
        
        T det = 0;

        return det;
    }
}

//*************************************************************************//

#endif // ALGEBRA_H_INCLUDED
