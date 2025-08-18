#include "matrix.h"

int main(int argc, char *argv[])
{
	MATRIX(A, 3, 3);
	MATRIX(B, 3, 3);

	matrix_set_field(A, 0, 0, 1);
	matrix_set_field(A, 0, 1, 1);
	matrix_set_field(A, 0, 2, 1);

	matrix_print(A);

	matrix_set_field(B, 0, 0, 1);
	matrix_set_field(B, 1, 0, 1);
	matrix_set_field(B, 2, 0, 1);

	matrix_print(B);

	MATRIX_MUL(D, A, B);

	matrix_print(D);

	struct matrix *C = matrix_mul(A, B);
	matrix_print(C);

	matrix_delete(C);

	MATRIX_TRANSPOSE(E, A);
	matrix_print(E);

	return 0;
}
