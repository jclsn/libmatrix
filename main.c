#include "fmatrix.h"
#include "matrix.h"

int main(void)
{
	/* INT OPERATIONS */

	MATRIX(mat_a, 3, 3);
	MATRIX(mat_b, 3, 3);

	printf("mat_a:\n");
	mat_set(mat_a, 0, 0, 1);
	mat_set(mat_a, 0, 1, 1);
	mat_set(mat_a, 0, 2, 1);
	mat_print(mat_a);

	printf("mat_b:\n");
	mat_set(mat_b, 0, 0, 1);
	mat_set(mat_b, 1, 0, 1);
	mat_set(mat_b, 2, 0, 1);
	mat_print(mat_b);

	MAT_MUL(mat_d, mat_a, mat_b);
	printf("mat_a x mat_b:\n");
	mat_print(mat_d);

	struct matrix *mat_c = mat_mul(mat_a, mat_b);
	printf("mat_a x mat_b:\n");
	mat_print(mat_c);

	mat_delete(mat_c);

	printf("mat_a^T:\n");
	MAT_TRANS(mat_e, mat_a);
	mat_print(mat_e);

	printf("mat_a = mat_b\n");
	mat_copy(mat_a, mat_b);
	printf("mat_a == mat_b:\n");
	if (mat_equal(mat_a, mat_b))
		printf("The matrices equal\n\n");
	else
		printf("The matrices don't equal\n\n");

	printf("mat_f:\n");
	struct matrix *mat_f = mat_alloc(1, 2);
	mat_set(mat_f, 0, 0, 3);
	mat_set(mat_f, 0, 1, 5);
	mat_print(mat_f);

	printf("mat_g:\n");
	struct matrix *mat_g = mat_alloc(2, 1);
	mat_set(mat_g, 0, 0, 3);
	mat_set(mat_g, 1, 0, 5);
	mat_print(mat_g);

	printf("mat_f x mat_g:\n");
	struct matrix *mat_h = mat_mul(mat_f, mat_g);
	mat_print(mat_h);

	printf("mat_a x mat_b:\n");
	struct matrix *mat_i = mat_mul(mat_a, mat_b);
	mat_print(mat_i);


	/* FLOAT OPERATIONS */

	FMATRIX(fmat_a, 3, 3);
	FMATRIX(fmat_b, 3, 3);

	printf("fmat_a:\n");
	fmat_set(fmat_a, 0, 0, 1);
	fmat_set(fmat_a, 0, 1, 1);
	fmat_set(fmat_a, 0, 2, 1);
	fmat_print(fmat_a);

	printf("fmat_b:\n");
	fmat_set(fmat_b, 0, 0, 1);
	fmat_set(fmat_b, 1, 0, 1);
	fmat_set(fmat_b, 2, 0, 1);
	fmat_print(fmat_b);

	FMAT_MUL(fmat_d, fmat_a, fmat_b);
	printf("fmat_a x fmat_b:\n");
	fmat_print(fmat_d);

	struct fmatrix *fmat_c = fmat_mul(fmat_a, fmat_b);
	printf("fmat_a x fmat_b:\n");
	fmat_print(fmat_c);

	fmat_delete(fmat_c);

	printf("fmat_a^T:\n");
	FMAT_TRANS(fmat_e, fmat_a);
	fmat_print(fmat_e);

	printf("fmat_a = fmat_b\n");
	fmat_copy(fmat_a, fmat_b);
	printf("fmat_a == fmat_b:\n");
	if (fmat_equal(fmat_a, fmat_b))
		printf("The fmatrices equal\n\n");
	else
		printf("The fmatrices don't equal\n\n");

	printf("fmat_f:\n");
	struct fmatrix *fmat_f = fmat_alloc(1, 2);
	fmat_set(fmat_f, 0, 0, 3);
	fmat_set(fmat_f, 0, 1, 5);
	fmat_print(fmat_f);

	printf("fmat_g:\n");
	struct fmatrix *fmat_g = fmat_alloc(2, 1);
	fmat_set(fmat_g, 0, 0, 3);
	fmat_set(fmat_g, 1, 0, 5);
	fmat_print(fmat_g);

	printf("fmat_f x fmat_g:\n");
	struct fmatrix *fmat_h = fmat_mul(fmat_f, fmat_g);
	fmat_print(fmat_h);

	printf("fmat_a x fmat_b:\n");
	struct fmatrix *fmat_i = fmat_mul(fmat_a, fmat_b);
	fmat_print(fmat_i);

	return 0;
}
