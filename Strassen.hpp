#ifndef STRASSEN_H_INCLUDED
#define STRASSEN_H_INCLUDED

//*************************************************************************//

#include "Matrix.hpp"
#include "Thread_Pool_T.hpp"

//*************************************************************************//
//
// parallel and serial Strassen functors
//
//*************************************************************************//

template<typename T>
struct Strassen {

    // intermal matrices
    Matrix<T> A, B;

    // depths for recursion and for using thread pool for parallelism
    size_t r, p;

    // constructor
    Strassen(Matrix<T> A_in, Matrix<T> B_in,
             size_t r_in)

        : A(std::move(A_in))
        , B(std::move(B_in))
        , r(r_in) {

        if(A.rows() != B.rows() ||
           A.rows() != B.columns() ||
           A.columns() != B.columns()) {

            throw std::out_of_range("incorrect dimensions for a Strassen product");
        }
    }

    // operator
    Matrix<T> operator () () {

        // if depth is 1 or dimension is odd, perform ikj multiplication
        if(r == 0 || A.rows() % 2 != 0) { return A * B; }

        // specify ranges for A and B submatrices
        size_t n = A.rows() / 2;
        
        // A submatrices
        Matrix<T> A11 = std::move(A.slice(0, n, 0, n));
        Matrix<T> A12 = std::move(A.slice(0, n, n, 2 * n));
        Matrix<T> A21 = std::move(A.slice(n, 2 * n, 0, n));
        Matrix<T> A22 = std::move(A.slice(n, 2 * n, n, 2 * n));
        
        // B submatrices
        Matrix<T> B11 = std::move(B.slice(0, n, 0, n));
        Matrix<T> B12 = std::move(B.slice(0, n, n, 2 * n));
        Matrix<T> B21 = std::move(B.slice(n, 2 * n, 0, n));
        Matrix<T> B22 = std::move(B.slice(n, 2 * n, n, 2 * n));
        
        // decrement depth for recursion
        r -= 1;

        // M1 = (A11 + A22) * (B11 + B22)
        Matrix<T> M1 = std::move(Strassen<T>(A11 + A22, B11 + B22, r)());
        
        // M2 = (A21 + A22) * B11
        Matrix<T> M2 = std::move(Strassen<T>(A21 + A22, B11, r)());
        
        // M3 = A11 * (B12 - B22)
        Matrix<T> M3 = std::move(Strassen<T>(A11, B12 - B22, r)());
        
        // M4 = A22 * (B21 - B11)
        Matrix<T> M4 = std::move(Strassen<T>(A22, B21 - B11, r)());
        
        // M5 = (A11 + A12) * B22
        Matrix<T> M5 = std::move(Strassen<T>(A11 + A12, B22, r)());
        
        // M6 = (A21 - A11) * (B11 + B12)
        Matrix<T> M6 = std::move(Strassen<T>(A21 - A11, B11 + B12, r)());
        
        // M7 = (A12 - A22) * (B21 + B22)
        Matrix<T> M7 = std::move(Strassen<T>(A12 - A22, B21 + B22, r)());

        // specify return matrix C
        Matrix<T> C(A.rows(), B.columns());

        // put elements into C
        for(size_t i = 0; i < n; ++i) {

            for(size_t j = 0; j < n; ++j) {
                
                // C11 = M1 + M4 - M5 + M7
                C(i, j) = M1(i, j) + M4(i, j) - M5(i, j) + M7(i, j);
                
                // C12 = M3 + M5
                C(i, n + j) = M3(i, j) + M5(i, j);
                
                // C21 = M2 + M4
                C(n + i, j) = M2(i, j) + M4(i, j);
                
                // C22 = M1 - M2 + M3 + M6
                C(n + i, n + j) = M1(i, j) - M2(i, j) + M3(i, j) + M6(i, j);
            }
        }

        return C;
    }

};

//*************************************************************************//

template<typename T>
struct Parallel_Strassen {

    // intermal matrices
    Matrix<T> A, B;

    // depths for recursion and for using thread pool for parallelism
    size_t r, p;
    
    // thread pool reference
    Thread_Pool<Matrix<T>>& TP;

    // constructor
    Parallel_Strassen(Matrix<T> A_in, Matrix<T> B_in,
                      size_t r_in, size_t p_in,
                      Thread_Pool<Matrix<T>>& TP_in)

        : A(std::move(A_in))
        , B(std::move(B_in))
        , r(r_in)
        , p(p_in)
        , TP(TP_in) {

        if(A.rows() != B.rows() ||
           A.rows() != B.columns() ||
           A.columns() != B.columns()) {

            throw std::out_of_range("incorrect dimensions for a Strassen product");
        }
    }

    // operator
    Matrix<T> operator () () {

        // if depth is 1 or dimension is odd, perform ikj multiplication
        if(r == 0 || A.rows() % 2 != 0) { return A * B; }
            
        // specify ranges for A and B submatrices
        size_t n = A.rows() / 2;
        
        // A submatrices
        Matrix<T> A11 = std::move(A.slice(0, n, 0, n));
        Matrix<T> A12 = std::move(A.slice(0, n, n, 2 * n));
        Matrix<T> A21 = std::move(A.slice(n, 2 * n, 0, n));
        Matrix<T> A22 = std::move(A.slice(n, 2 * n, n, 2 * n));
        
        // B submatrices
        Matrix<T> B11 = std::move(B.slice(0, n, 0, n));
        Matrix<T> B12 = std::move(B.slice(0, n, n, 2 * n));
        Matrix<T> B21 = std::move(B.slice(n, 2 * n, 0, n));
        Matrix<T> B22 = std::move(B.slice(n, 2 * n, n, 2 * n));
        
        // decrement depth for recursion
        r -= 1;

        // declare intermediate matrices
        Matrix<T> M1, M2, M3, M4, M5, M6, M7;

        if(p == 0) {

            // M1 = (A11 + A22) * (B11 + B22)
            std::future<Matrix<T>> f_M1 =

                TP.load_front(std::move(Strassen<T>(A11 + A22, B11 + B22, r)));
            
            // M2 = (A21 + A22) * B11
            std::future<Matrix<T>> f_M2 =

                TP.load_front(std::move(Strassen<T>(A21 + A22, B11, r)));
            
            // M3 = A11 * (B12 - B22)
            std::future<Matrix<T>> f_M3 =

                TP.load_front(std::move(Strassen<T>(A11, B12 - B22, r)));
            
            // M4 = A22 * (B21 - B11)
            std::future<Matrix<T>> f_M4 =

                TP.load_front(std::move(Strassen<T>(A22, B21 - B11, r)));
            
            // M5 = (A11 + A12) * B22
            std::future<Matrix<T>> f_M5 =

                TP.load_front(std::move(Strassen<T>(A11 + A12, B22, r)));
            
            // M6 = (A21 - A11) * (B11 + B12)
            std::future<Matrix<T>> f_M6 =

                TP.load_front(std::move(Strassen<T>(A21 - A11, B11 + B12, r)));
            
            // M7 = (A12 - A22) * (B21 + B22)
            std::future<Matrix<T>> f_M7 =

                TP.load_front(std::move(Strassen<T>(A12 - A22, B21 + B22, r)));

            M1 = std::move(f_M1.get());
            M2 = std::move(f_M2.get());
            M3 = std::move(f_M3.get());
            M4 = std::move(f_M4.get());
            M5 = std::move(f_M5.get());
            M6 = std::move(f_M6.get());
            M7 = std::move(f_M7.get());

        } else {

            // ensure that parallelism is used at a certain depth and not further
            p -= 1;

            // M1 = (A11 + A22) * (B11 + B22)
            M1 = std::move(Parallel_Strassen<T>(A11 + A22, B11 + B22, r, p, TP)());
            
            // M2 = (A21 + A22) * B11
            M2 = std::move(Parallel_Strassen<T>(A21 + A22, B11, r, p, TP)());
            
            // M3 = A11 * (B12 - B22)
            M3 = std::move(Parallel_Strassen<T>(A11, B12 - B22, r, p, TP)());
            
            // M4 = A22 * (B21 - B11)
            M4 = std::move(Parallel_Strassen<T>(A22, B21 - B11, r, p, TP)());
            
            // M5 = (A11 + A12) * B22
            M5 = std::move(Parallel_Strassen<T>(A11 + A12, B22, r, p, TP)());
            
            // M6 = (A21 - A11) * (B11 + B12)
            M6 = std::move(Parallel_Strassen<T>(A21 - A11, B11 + B12, r, p, TP)());
            
            // M7 = (A12 - A22) * (B21 + B22)
            M7 = std::move(Parallel_Strassen<T>(A12 - A22, B21 + B22, r, p, TP)());
        }

        // specify return matrix C
        Matrix<T> C(A.rows(), B.columns());

        // put elements into C
        for(size_t i = 0; i < n; ++i) {

            for(size_t j = 0; j < n; ++j) {
                
                // C11 = M1 + M4 - M5 + M7
                C(i, j) = M1(i, j) + M4(i, j) - M5(i, j) + M7(i, j);
                
                // C12 = M3 + M5
                C(i, n + j) = M3(i, j) + M5(i, j);
                
                // C21 = M2 + M4
                C(n + i, j) = M2(i, j) + M4(i, j);
                
                // C22 = M1 - M2 + M3 + M6
                C(n + i, n + j) = M1(i, j) - M2(i, j) + M3(i, j) + M6(i, j);
            }
        }

        return C;
    }

};

//*************************************************************************//

#endif // STRASSEN_H_INCLUDED
