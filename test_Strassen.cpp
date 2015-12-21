#include <iostream>

#include "Matrix.hpp"
#include "Utilities.hpp"
#include "Timer.hpp"
#include "Thread_Pool_T.hpp"
#include "Strassen.hpp"

int main() {

	using namespace std;

	// initialize objects
    Utilities<double> Ut(std::pow(10, -14));
    Timer<double> Ti;
	Thread_Pool<Matrix<double>> TP(4, 'f', 2);

	size_t dim = 2048;

	Matrix<double> M(dim, dim);
    Matrix<double> N(dim, dim);

    Ut.randomize(M, -1.0, 1.0);
    Ut.randomize(N, -1.0, 1.0);

    Ti.start();
    Matrix<double> P = Parallel_Strassen<double>(M, N, 5, 0, TP)();
	Ti.stop();
    cout << "\nparallel duration: " << Ti.duration() << "\n" << endl;

    return 0;
}
