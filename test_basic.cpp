#include <iostream>

#include "Matrix.hpp"
#include "Utilities.hpp"
#include "Algebra.hpp"

template<typename T>
void print(const Matrix<T>& M) {

	size_t i_end = M.rows();
	size_t j_end = M.columns();

	for(size_t i = 0; i < i_end; ++i) {

		for(size_t j = 0; j < j_end; ++j) {
		
			std::cout << M(i, j) << " ";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}

int main() {

	std::cout << "\n";

	Matrix<double> A(2, 2, { 1, 2,
					   		 3, 4 });

	print<double>(A);

	Utilities<double> Ut(std::pow(10, -14));
	Algebra Al;

	size_t dim = 4;

	Matrix<double> M(dim, dim);
	Ut.randomize(M, -1.0, 1.0);
	print<double>(M);

	Matrix<double> N = Al.rref(M);
	Ut.round_values(N);
	print<double>(N);

	Matrix<double> P(dim, dim);
	Ut.randomize(P, -1.0, 1.0);
	print<double>(P);

	Matrix<double> Q = Al.inverse(P);
	print<double>(Q);

	Matrix<double> R = P * Q;
	Ut.round_values(R);
	print<double>(R);

	return 0;
}
