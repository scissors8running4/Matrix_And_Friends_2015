// std::deque and std::memory are included in Thread_Pool_G.hpp
#include <iostream>

#include "Matrix.hpp"
#include "Utilities.hpp"
#include "Timer.hpp"
#include "Thread_Pool_G.hpp"

template<typename T>
struct multiply {

    const Matrix<T>& M;
    const Matrix<T>& N;

    multiply(const Matrix<T>& M_in, const Matrix<T>& N_in)

        : M(M_in)
        , N(N_in) {}

    Matrix<T> operator () () { return M * N; }

};

int main() {
    
    // initialize objects
    Utilities<double> Ut(std::pow(10, -14));
    Timer<double> Ti;

    std::deque<std::unique_ptr<Thread_Pool>> MTP;

    MTP.push_back(std::unique_ptr<Thread_Pool>(new Thread_Pool(2, 'b', 2)));
    MTP.push_back(std::unique_ptr<Thread_Pool>(new Thread_Pool(2, 'f', 2)));

    size_t dim = 1024;

    Matrix<double> M(dim, dim);
    Matrix<double> N(dim, dim);

    Ut.randomize(M, -1.0, 1.0);
    Ut.randomize(N, -1.0, 1.0);

    std::future<Matrix<double>> Pf;
    std::future<Matrix<double>> Qf;
    std::future<Matrix<double>> Rf;
    std::future<Matrix<double>> Sf;

    Matrix<double> P;
    Matrix<double> Q;
    Matrix<double> R;
    Matrix<double> S;

//---------------------------------------------------------//
    
    Ti.start();

    Pf = MTP[0] -> load_front(multiply<double>(M, N));
    Qf = MTP[0] -> load_back(multiply<double>(M, N));
    Rf = MTP[1] -> load_front(multiply<double>(M, N));
    Sf = MTP[1] -> load_back(multiply<double>(M, N));

    P = Pf.get();
    Q = Qf.get();
    R = Rf.get();
    S = Sf.get();

    Ti.stop();

    std::cout << "\nduration: " << Ti.duration() << "\n" << std::endl;

    return 0;
}
