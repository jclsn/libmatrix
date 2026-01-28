#include "matrix.h"
#include "tests.h"

int main(void)
{
	jl_init();

	/* int matrices */

	TEST("creation of A1 on the stack");
	MATRIX(A1, 3, 3);
	mat_set(A1, 0, 0, 1);
	mat_set(A1, 0, 1, 1);
	mat_set(A1, 0, 2, 1);
	jl_test_mat(A1, "A1 = [1 1 1; 0 0 0; 0 0 0]");

	TEST("creation of B1 on the stack");
	MATRIX(B1, 3, 3);
	mat_set(B1, 0, 0, 1);
	mat_set(B1, 1, 0, 1);
	mat_set(B1, 2, 0, 1);
	jl_test_mat(B1, "B1 = [1 0 0; 1 0 0; 1 0 0]");

	TEST("multiplication of A1 * B1 ");
	MAT_MUL(C1, A1, B1);
	jl_test_mat(C1, "A1 * B1");

	TEST("creation of A2 on the heap");
	struct matrix *A2 = mat_alloc(3, 3);
	mat_set(A2, 0, 0, 1);
	mat_set(A2, 0, 1, 1);
	mat_set(A2, 0, 2, 1);
	jl_test_mat(A2, "A2 = [1 1 1; 0 0 0; 0 0 0]");

	TEST("creation of B2 on the heap");
	struct matrix *B2 = mat_alloc(3, 3);
	mat_set(B2, 0, 0, 1);
	mat_set(B2, 1, 0, 1);
	mat_set(B2, 2, 0, 1);
	jl_test_mat(B2, "B2 = [1 0 0; 1 0 0; 1 0 0]");

	TEST("multiplication of A2 * B2 ");
	struct matrix *C2 = mat_mul(A2, B2);
	jl_test_mat(C2, "A2 * B2");

	mat_delete(A2);
	mat_delete(B2);
	mat_delete(C2);

	TEST("shifting of A");
	mat_reset(A1);
	mat_set(A1, 1, 1, 1);
	mat_print(A1);

	printf("East: \n");
	mat_reset(A1);
	mat_set(A1, 1, 1, 1);
	mat_shift_east(A1, 1);
	mat_print(A1);

	printf("West: \n");
	mat_reset(A1);
	mat_set(A1, 1, 1, 1);
	mat_shift_west(A1, 1);
	mat_print(A1);

	printf("North: \n");
	mat_reset(A1);
	mat_set(A1, 1, 1, 1);
	mat_shift_north(A1, 1);
	mat_print(A1);

	printf("South: \n");
	mat_reset(A1);
	mat_set(A1, 1, 1, 1);
	mat_shift_south(A1, 1);
	mat_print(A1);

	jl_atexit_hook(0);

	return 0;
}
