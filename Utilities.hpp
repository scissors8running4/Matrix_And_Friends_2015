#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

//*************************************************************************//

#include <complex>
#include <random>
#include <chrono>

#include <cmath>

#include <iostream>
#include <fstream>
#include <string>

#include "Matrix.hpp"

//*************************************************************************//
//
// random number class for matrices
//
//*************************************************************************//

template<typename T>
class Utilities {

private:
    
    // seed comes from current system time
    std::default_random_engine dre;

    // value for rounding down
    T rounding_value;

    // assign a complex value to an index
    void assign(std::complex<T>&,
                std::uniform_real_distribution<T>&);

    // assign a value to an index
    void assign(T& value,
                std::uniform_real_distribution<T>&);

    // round a complex value down
    void round_down(std::complex<T>&);

    // round a real value down
    void round_down(T&);

public:
    
    // basic constructors
    Utilities(T);

    Utilities();

    // move and copy constructors
    Utilities(Utilities&&) = default;

    Utilities(const Utilities&) = default;

    // move and copy assignors
    Utilities& operator = (Utilities&&) = default;

    Utilities& operator = (const Utilities&) = default;
    
    // destructor
    ~Utilities() = default;
    
    // reseeds random generator
    void reseed();

    // set a value for rounding
    void set_rounding_value(T);

    // fill matrix with random values
    void randomize(Matrix<T>&, T, T);

    // round values close to zero to zero
    void round_values(Matrix<T>&);

    // write data from a text file
    void write_to_file(const Matrix<T>&, std::string);
    
    // read data to a text file
    void read_from_file(const Matrix<T>&, std::string);

};

//*************************************************************************//

// assign a complex value to an index
template<typename T>
inline void Utilities<T>::assign(std::complex<T>& value,
                                 std::uniform_real_distribution<T>& urd) {

    value.real(urd(dre));
    value.imag(urd(dre));
}

// assign a complex value to an index
template<typename T>
inline void Utilities<T>::assign(T& value,
                                 std::uniform_real_distribution<T>& urd) {

    value = urd(dre);
}

// round a complex value down
template<typename T>
inline void Utilities<T>::round_down(std::complex<T>& value) {

    if(value.real() > -rounding_value && value.real() < rounding_value) { value.real(0); }
    if(value.imag() > -rounding_value && value.imag() < rounding_value) { value.imag(0); }
}

// round a real value down
template<typename T>
inline void Utilities<T>::round_down(T& value) {

    if(value > -rounding_value && value < rounding_value) { value = 0; }
}
    
//*************************************************************************//

// constructors
template<typename T>
Utilities<T>::Utilities(T new_rounding_value) {
    
    dre.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    rounding_value = new_rounding_value;
}

template<typename T>
Utilities<T>::Utilities() {
    
    dre.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    rounding_value = std::pow(10, -14);
}

// reseeds random generator
template<typename T>
void Utilities<T>::reseed() {
    
    dre.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

// set a value for rounding
template<typename T>
void Utilities<T>::set_rounding_value(T new_rounding_value) {

    rounding_value = new_rounding_value;
}

// fill T matrix with random values
template<typename T>
void Utilities<T>::randomize(Matrix<T>& M, T min, T max) {

    std::uniform_real_distribution<T> urd(min, max);
    
    size_t i_end = M.rows();
    size_t j_end = M.columns();

    for(size_t i = 0; i != i_end; ++i) {
        
        for(size_t j = 0; j != j_end; ++j) {

            Utilities<T>::assign(M(i, j), urd);
        }
    }
}

// round values close to zero to zero
template<typename T>
void Utilities<T>::round_values(Matrix<T>& M) {
    
    size_t i_end = M.rows();
    size_t j_end = M.columns();

    for(size_t i = 0; i != i_end; ++i) {

        for(size_t j = 0; j != j_end; ++j) {
            
            Utilities<T>::round_down(M(i, j));
        }
    }
}

// read data from a text file
template<typename T>
void Utilities<T>::write_to_file(const Matrix<T>& M,
                                 std::string file_name) {
    
    std::ofstream fout(file_name);
    
    if(!fout.is_open()) {

        std::cerr << "cannot open \"" << file_name << "\"" << std::endl;
        return;
    }
    
    size_t i_end = M.rows();
    size_t j_end = M.columns();

    for(size_t i = 0; i != i_end; ++i) {

        for(size_t j = 0; j != j_end; ++j) {

            fout << M(i, j) << " ";
        }

        fout << std::endl;
    }
}

// write data to a text file
template<typename T>
void Utilities<T>::read_from_file(const Matrix<T>& M,
                                  std::string file_name) {
    
    std::ifstream fin(file_name);
    
    if(!fin.is_open()) {

        std::cerr << "cannot open \"" << file_name << "\"" << std::endl;
        return;
    }
    
    size_t i_end = M.rows();
    size_t j_end = M.columns();
    
    for(size_t i = 0; i != i_end; ++i) {

        for(size_t j = 0; j != j_end; ++j) {

            fin >> M(i, j);
        }
    }
}

//*************************************************************************//

#endif // UTILITIES_H_INCLUDED
