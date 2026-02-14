#include "fmatrix.h"
#include "matrix.h"

int main(void)
{
	/* ---------------- Integer matrices ---------------- */
	struct matrix *A = mat_alloc(3, 3);
	struct matrix *B = mat_alloc(3, 3);
	struct matrix *C = NULL;
	struct matrix *T = NULL;

	/* Fill A */
	mat_set(A, 0, 0, 1);
	mat_set(A, 0, 1, 1);
	mat_set(A, 0, 2, 1);

	/* Fill B */
	mat_set(B, 0, 0, 1);
	mat_set(B, 1, 0, 1);
	mat_set(B, 2, 0, 1);

	/* Perform addition */
	C = mat_add(NULL, A, B);

	/* Perform subtraction */
	C = mat_sub(C, A, B);

	/* Perform multiplication */
	C = mat_mul(C, A, B);

	/* Perform transposition */
	T = mat_trans(NULL, A);

	/* Set identity */
	mat_set_identity(A);
	struct matrix *I = mat_identity_new(3);

	/* Copy and duplicate matrices */
	struct matrix *D = mat_copy(NULL, I);
	mat_copy(D, A);

	/* Perform shift operations */
	struct matrix *S = mat_alloc(3, 1);
	mat_set(S, 2, 0, 1);
	mat_shift_north(S, 1);
	mat_shift_south(S, 1);
	mat_shift_east(S, 1);
	mat_shift_west(S, 1);

	/* Free integer matrices */
	mat_free(A);
	mat_free(B);
	mat_free(C);
	mat_free(T);
	mat_free(I);
	mat_free(D);
	mat_free(S);

	/* ---------------- Floating-point matrices ---------------- */
	struct fmatrix *FA = fmat_alloc(3, 3);
	struct fmatrix *FB = fmat_alloc(3, 3);
	struct fmatrix *FC = NULL;
	struct fmatrix *FT = NULL;

	/* Fill FA */
	fmat_set(FA, 0, 0, 1.0);
	fmat_set(FA, 0, 1, 1.0);
	fmat_set(FA, 0, 2, 1.0);

	/* Fill FB */
	fmat_set(FB, 0, 0, 1.0);
	fmat_set(FB, 1, 0, 1.0);
	fmat_set(FB, 2, 0, 1.0);

	/* Perform addition */
	FC = fmat_add(NULL, FA, FB);

	/* Perform subtraction */
	FC = fmat_sub(FC, FA, FB);

	/* Perform multiplication */
	FC = fmat_mul(FC, FA, FB);

	/* Perform transposition */
	FT = fmat_trans(NULL, FA);

	/* Set identity */
	fmat_set_identity(FA);
	struct fmatrix *FI = fmat_identity_new(3);

	/* Copy and duplicate matrices */
	struct fmatrix *FD = fmat_copy(NULL, FI);
	fmat_copy(FD, FA);

	/* Perform shift operations */
	struct fmatrix *FS = fmat_alloc(3, 1);
	fmat_set(FS, 2, 0, 1.0);
	fmat_shift_north(FS, 1);
	fmat_shift_south(FS, 1);
	fmat_shift_east(FS, 1);
	fmat_shift_west(FS, 1);

	/* Perform inverse */
	struct fmatrix *FIINV = fmat_alloc(3, 3);
	fmat_set(FIINV, 0, 0, 2);
	fmat_set(FIINV, 1, 0, 1);
	fmat_set(FIINV, 2, 0, 0);
	fmat_set(FIINV, 0, 1, -1);
	fmat_set(FIINV, 1, 1, 2);
	fmat_set(FIINV, 2, 1, -1);
	fmat_set(FIINV, 0, 2, 0);
	fmat_set(FIINV, 1, 2, -2);
	fmat_set(FIINV, 2, 2, 1);
	struct fmatrix *FINV = fmat_inv(NULL, FIINV);

	/* Free floating-point matrices */
	fmat_free(FA);
	fmat_free(FB);
	fmat_free(FC);
	fmat_free(FT);
	fmat_free(FI);
	fmat_free(FD);
	fmat_free(FS);
	fmat_free(FIINV);
	fmat_free(FINV);

	return 0;
}
