#include <stdio.h>

#include "tests.h"

int main(void)
{
	jl_init();

	/* int matrices */

	struct matrix *A = mat_alloc(3, 3);
	struct matrix *B = mat_alloc(3, 3);

	mat_set(A, 0, 0, 1);
	mat_set(A, 0, 1, 1);
	mat_set(A, 0, 2, 1);

	printf("\nTest A:\n");
	jl_test_mat(A, "A = [1 1 1; 0 0 0; 0 0 0]");

	mat_set(B, 0, 0, 1);
	mat_set(B, 1, 0, 1);
	mat_set(B, 2, 0, 1);

	printf("\nTest B:\n");
	jl_test_mat(B, "B = [1 0 0; 1 0 0; 1 0 0]");

	struct matrix *C = mat_mul(A, B);
	printf("\nTest C:\n");
	jl_test_mat(C, "A * B");

	mat_delete(A);
	mat_delete(B);
	mat_delete(C);

	/* float matrices */

	struct fmatrix *fA = fmat_alloc(3, 3);
	struct fmatrix *fB = fmat_alloc(3, 3);

	fmat_set(fA, 0, 0, 1.0);
	fmat_set(fA, 0, 1, 1.0);
	fmat_set(fA, 0, 2, 1.0);

	printf("\nTest fA:\n");
	jl_test_fmat(fA, "fA = [1. 1. 1.; 0. 0. 0.; 0. 0. 0.]", 1e-12, 1e-12);

	fmat_set(fB, 0, 0, 1.0);
	fmat_set(fB, 1, 0, 1.0);
	fmat_set(fB, 2, 0, 1.0);

	printf("\nTest fB:\n");
	jl_test_fmat(fB, "fB = [1. 0. 0.; 1. 0. 0.; 1. 0. 0.]", 1e-12, 1e-12);

	struct fmatrix *fC = fmat_mul(fA, fB);

	printf("\nTest fC:\n");
	jl_test_fmat(fC, "fC = fA * fB", 1e-12, 1e-12);

	fmat_delete(fA);
	fmat_delete(fB);
	fmat_delete(fC);

	jl_atexit_hook(0);

	return 0;
}
