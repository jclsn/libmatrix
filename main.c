#include "fmatrix.h"
#include "matrix.h"

int main(int argc, char *argv[])
{
	/* INT OPERATIONS */

	MATRIX(mat_a, 3, 3);
	MATRIX(mat_b, 3, 3);

	mat_set_field(mat_a, 0, 0, 1);
	mat_set_field(mat_a, 0, 1, 1);
	mat_set_field(mat_a, 0, 2, 1);

	mat_print(mat_a);

	mat_set_field(mat_b, 0, 0, 1);
	mat_set_field(mat_b, 1, 0, 1);
	mat_set_field(mat_b, 2, 0, 1);

	mat_print(mat_b);

	MAT_MUL(mat_d, mat_a, mat_b);

	mat_print(mat_d);

	struct matrix *mat_c = mat_mul(mat_a, mat_b);
	mat_print(mat_c);

	mat_delete(mat_c);

	MAT_TRANS(mat_e, mat_a);
	mat_print(mat_e);

	/* FLOAT OPERATIONS */

	FMATRIX(fmat_a, 3, 3);
	FMATRIX(fmat_b, 3, 3);

	fmat_set_field(fmat_a, 0, 0, 1);
	fmat_set_field(fmat_a, 0, 1, 1);
	fmat_set_field(fmat_a, 0, 2, 1);

	fmat_print(fmat_a);

	fmat_set_field(fmat_b, 0, 0, 1);
	fmat_set_field(fmat_b, 1, 0, 1);
	fmat_set_field(fmat_b, 2, 0, 1);

	fmat_print(fmat_b);

	FMAT_MUL(fmat_d, fmat_a, fmat_b);

	fmat_print(fmat_d);

	struct fmatrix *fmat_c = fmat_mul(fmat_a, fmat_b);
	fmat_print(fmat_c);

	fmat_delete(fmat_c);

	FMAT_TRANS(fmat_e, fmat_a);
	fmat_print(fmat_e);

	return 0;
}
