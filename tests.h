#ifndef TESTS_H

#define TESTS_H

#include <julia/julia.h>

#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "fmatrix.h"
#include "format.h"
#include "matrix.h"

#define TEST(...)                                                                 \
	do {                                                                      \
		fprintf(stdout, FG_BLUE "\nTesting " __VA_ARGS__ ":" RESET "\n"); \
		fflush(stdout);                                                   \
	} while (0)

/* Test a matrix with Julia*/
int jl_test_mat(const struct matrix *m, const char *julia_expr);
/* Test a floating-point matrix with Julia*/
int jl_test_fmat(const struct fmatrix *m, const char *julia_expr, double rtol, double atol);

/* CMocka tests */

void test_matrix_stack_creation(void **state);
void test_matrix_heap_creation(void **state);
void test_matrix_heap_multiplication(void **state);

void test_matrix_alloc_valid(void **state);
void test_matrix_alloc_invalid_dims(void **state);

void test_matrix_set_identity(void **state);
void test_matrix_identity_new(void **state);
void test_matrix_set_and_reset(void **state);
void test_matrix_set_row_gf2(void **state);

void test_matrix_shift_east(void **state);
void test_matrix_shift_west(void **state);
void test_matrix_shift_north(void **state);
void test_matrix_shift_south(void **state);

void test_matrix_copy(void **state);
void test_matrix_duplication(void **state);

void test_matrix_addition(void **state);
void test_matrix_subtraction(void **state);
void test_matrix_multiplication(void **state);

void test_matrix_transposition(void **state);

void test_fmatrix_stack_creation(void **state);
void test_fmatrix_heap_creation(void **state);
void test_fmatrix_heap_multiplication(void **state);

void test_fmatrix_alloc_valid(void **state);
void test_fmatrix_alloc_invalid_dims(void **state);

void test_fmatrix_set_identity(void **state);
void test_fmatrix_identity_new(void **state);
void test_fmatrix_set_and_reset(void **state);
void test_fmatrix_set_row_gf2(void **state);

void test_fmatrix_shift_east(void **state);
void test_fmatrix_shift_west(void **state);
void test_fmatrix_shift_north(void **state);
void test_fmatrix_shift_south(void **state);

void test_fmatrix_copy(void **state);
void test_fmatrix_duplication(void **state);

void test_fmatrix_addition(void **state);
void test_fmatrix_subtraction(void **state);
void test_fmatrix_multiplication(void **state);

void test_fmatrix_transposition(void **state);


#endif /* end of include guard TESTS_H */
