#include "fmatrix.h"
#include "matrix.h"
#include "tests.h"

int main(void)
{
	jl_init();

	const struct CMUnitTest tests[] = {

		/* Matrix tests */

		/* Allocation test */
		cmocka_unit_test(test_matrix_stack_creation),
		cmocka_unit_test(test_matrix_heap_creation),
		cmocka_unit_test(test_matrix_heap_multiplication),
		cmocka_unit_test(test_matrix_alloc_valid),
		cmocka_unit_test(test_matrix_alloc_invalid_dims),

		/* Setters */
		cmocka_unit_test(test_matrix_set_identity),
		cmocka_unit_test(test_matrix_identity_new),

		cmocka_unit_test(test_matrix_set_string),
		cmocka_unit_test(test_matrix_set_and_reset),
		cmocka_unit_test(test_matrix_set_row_gf2),

		cmocka_unit_test(test_matrix_shift_east),
		cmocka_unit_test(test_matrix_shift_west),
		cmocka_unit_test(test_matrix_shift_north),
		cmocka_unit_test(test_matrix_shift_south),

		cmocka_unit_test(test_matrix_copy),
		cmocka_unit_test(test_matrix_duplication),

		cmocka_unit_test(test_matrix_addition),
		cmocka_unit_test(test_matrix_subtraction),
		cmocka_unit_test(test_matrix_heap_multiplication),

		cmocka_unit_test(test_matrix_transposition),

		/* Floating-point matrix tests */

		cmocka_unit_test(test_fmatrix_stack_creation),
		cmocka_unit_test(test_fmatrix_heap_creation),
		cmocka_unit_test(test_fmatrix_heap_multiplication),
		cmocka_unit_test(test_fmatrix_alloc_valid),
		cmocka_unit_test(test_fmatrix_alloc_invalid_dims),

		cmocka_unit_test(test_fmatrix_set_identity),
		cmocka_unit_test(test_fmatrix_identity_new),

		cmocka_unit_test(test_fmatrix_set_string),
		cmocka_unit_test(test_fmatrix_set_and_reset),
		cmocka_unit_test(test_fmatrix_set_row_gf2),

		cmocka_unit_test(test_fmatrix_shift_east),
		cmocka_unit_test(test_fmatrix_shift_west),
		cmocka_unit_test(test_fmatrix_shift_north),
		cmocka_unit_test(test_fmatrix_shift_south),

		cmocka_unit_test(test_fmatrix_copy),
		cmocka_unit_test(test_fmatrix_duplication),

		cmocka_unit_test(test_fmatrix_addition),
		cmocka_unit_test(test_fmatrix_subtraction),
		cmocka_unit_test(test_fmatrix_heap_multiplication),

		cmocka_unit_test(test_fmatrix_transposition),
		cmocka_unit_test(test_fmatrix_inverse),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
