#include "matrix.h"

int main(int argc, char *argv[])
{
	MATRIX(A, 3, 3);
	MATRIX(B, 3, 3);

	mat_set_field(A, 0, 0, 1);
	mat_set_field(A, 0, 1, 1);
	mat_set_field(A, 0, 2, 1);

	mat_print(A);

	mat_set_field(B, 0, 0, 1);
	mat_set_field(B, 1, 0, 1);
	mat_set_field(B, 2, 0, 1);

	mat_print(B);

	MAT_MUL(D, A, B);

	mat_print(D);

	struct matrix *C = mat_mul(A, B);
	mat_print(C);

	mat_delete(C);

	MAT_TRANSPOSE(E, A);
	mat_print(E);

	return 0;
}
