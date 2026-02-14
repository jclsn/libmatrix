#include "tests.h"

// only define this once, in an executable
JULIA_DEFINE_FAST_TLS

static void mat_to_julia_matrix(const struct matrix *m, const char *julia_name)
{
	char buf[2048];
	size_t pos = 0;

	pos += snprintf(buf + pos, sizeof(buf) - pos, "%s = reshape([", julia_name);

	for (size_t c = 0; c < m->cols; c++) {
		for (size_t r = 0; r < m->rows; r++) {
			pos += snprintf(buf + pos, sizeof(buf) - pos, "%lld", m->data[r][c]);
			if (r != m->rows - 1 || c != m->cols - 1)
				pos += snprintf(buf + pos, sizeof(buf) - pos, ", ");
		}
	}

	snprintf(buf + pos, sizeof(buf) - pos, "], %zu, %zu)", m->rows, m->cols);
	jl_eval_string(buf);
}

int jl_test_mat(const struct matrix *m, const char *julia_expr)
{
	if (!m || !julia_expr)
		return 0;

	/* Create C-side matrix in Julia as `Cmat` */
	mat_to_julia_matrix(m, "Cmat");

	/* Evaluate user expression as `Jmat` */
	char cmd[2048];
	snprintf(cmd, sizeof(cmd), "Jmat = %s", julia_expr);
	jl_eval_string(cmd);

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception while evaluating expression: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	/* Compare */
	jl_value_t *res = jl_eval_string("res = Cmat == Jmat; flush(stdout)");
	jl_eval_string("if !res\n \
		            display(Cmat); display(Jmat); \
			    print(\"[ \"); \
			    printstyled(\"MISMATCH\", color=:red); \
			    print(\" ]\n\"); \
		        end\n \
		        flush(stdout) \
	");

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception during comparison: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	return jl_unbox_bool(res);
}

static void fmat_to_julia_matrix(const struct fmatrix *m, const char *julia_name)
{
	char buf[4096];
	size_t pos = 0;

	pos += snprintf(buf + pos, sizeof(buf) - pos, "%s = reshape(Float64[", julia_name);

	for (size_t c = 0; c < m->cols; c++) {
		for (size_t r = 0; r < m->rows; r++) {
			pos += snprintf(buf + pos, sizeof(buf) - pos, "%.17g", m->data[r][c]);
			if (r != m->rows - 1 || c != m->cols - 1)
				pos += snprintf(buf + pos, sizeof(buf) - pos, ", ");
		}
	}

	snprintf(buf + pos, sizeof(buf) - pos, "], %zu, %zu)", m->rows, m->cols);

	jl_eval_string(buf);
}

int jl_test_fmat(const struct fmatrix *m, const char *julia_expr, double rtol, double atol)
{
	if (!m || !julia_expr)
		return 0;

	/* Push C matrix into Julia as Cmat */
	fmat_to_julia_matrix(m, "Cmat");

	/* Evaluate Julia-side matrix */
	char cmd[2048];
	snprintf(cmd, sizeof(cmd), "Jmat = %s", julia_expr);
	jl_eval_string(cmd);

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception while evaluating expression: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	jl_value_t *res = jl_eval_string("res = Cmat == Jmat; flush(stdout)");

	jl_eval_string("if !res\n \
		            display(Cmat); display(Jmat); \
			    print(\"[ \"); \
			    printstyled(\"MISMATCH\", color=:red); \
			    print(\" ]\n\"); \
		        end\n \
		        flush(stdout) \
	");

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception during float comparison: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	return jl_unbox_bool(res);
}

/*
 * Integer matrix tests
 */

void test_matrix_stack_creation(void **state)
{
	(void)state;

	MATRIX(A, 3, 3);

	mat_set(A, 0, 0, 1);
	mat_set(A, 0, 1, 1);
	mat_set(A, 0, 2, 1);

	if (!jl_test_mat(A, "A = [1 1 1; 0 0 0; 0 0 0]")) {
		fail();
	}
}

void test_matrix_heap_creation(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(3, 3);

	mat_set(A, 0, 0, 1);
	mat_set(A, 0, 1, 1);
	mat_set(A, 0, 2, 1);

	if (!jl_test_mat(A, "A = [1 1 1; 0 0 0; 0 0 0]")) {
		fail();
	}
}

void test_matrix_heap_multiplication(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(3, 3);
	struct matrix *B = mat_alloc(3, 3);

	mat_set(A, 0, 0, 1);
	mat_set(A, 0, 1, 1);
	mat_set(A, 0, 2, 1);

	mat_set(B, 0, 0, 1);
	mat_set(B, 1, 0, 1);
	mat_set(B, 2, 0, 1);

	struct matrix *C = mat_mul(NULL, A, B);

	if (!jl_test_mat(C, "[1 1 1; 0 0 0; 0 0 0]*[1 0 0; 1 0 0; 1 0 0]")) {
		fail();
	}
}

void test_matrix_alloc_valid(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(2, 3);
	assert_non_null(A);
	assert_int_equal(A->rows, 2);
	assert_int_equal(A->cols, 3);

	mat_free(A);
}

void test_matrix_alloc_invalid_dims(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(0, 3);
	assert_null(A);
}

void test_matrix_set_identity(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(3, 3);
	mat_set_identity(A);

	if (!jl_test_mat(A, "[1 0 0; 0 1 0; 0 0 1]"))
		fail();

	mat_free(A);
}

void test_matrix_identity_new(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(3);

	if (!jl_test_mat(A, "[1 0 0; 0 1 0; 0 0 1]"))
		fail();

	mat_free(A);
}

void test_matrix_set_and_reset(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(2, 3);
	mat_set(A, 0, 1, 5);
	mat_reset(A);

	if (!jl_test_mat(A, "[0 0 0; 0 0 0]"))
		fail();

	mat_free(A);
}

void test_matrix_set_row_gf2(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(1, 4);
	mat_set_row_gf2(A, 0, 0b1010);

	if (!jl_test_mat(A, "[1 0 1 0]"))
		fail();

	mat_free(A);
}

void test_matrix_set_string(void **state)
{
	(void)state;

	struct matrix *A = mat_set_string("[1 0 0; 0 1 0; 0 0 1]");

	if (!jl_test_mat(A, "[1 0 0; 0 1 0; 0 0 1]"))
		fail();

	mat_free(A);
}

void test_matrix_shift_east(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(1, 4);
	mat_set_row_gf2(A, 0, 0b1111);

	mat_shift_east(A, 2);

	if (!jl_test_mat(A, "[0 0 1 1]"))
		fail();

	mat_free(A);
}

void test_matrix_shift_west(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(1, 4);
	mat_set_row_gf2(A, 0, 0b1111);

	mat_shift_west(A, 2);

	if (!jl_test_mat(A, "[1 1 0 0]"))
		fail();

	mat_free(A);
}

void test_matrix_shift_north(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(3, 1);
	mat_set(A, 2, 0, 1);

	mat_shift_north(A, 1);

	if (!jl_test_mat(A, "[0; 1; 0;;]"))
		fail();

	mat_free(A);
}

void test_matrix_shift_south(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(3, 1);
	mat_set(A, 0, 0, 1);

	mat_shift_south(A, 1);

	if (!jl_test_mat(A, "[0; 1; 0;;]"))
		fail();

	mat_free(A);
}

void test_matrix_copy(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(2);
	struct matrix *B = mat_alloc(2, 2);

	mat_copy(B, A);

	assert_true(mat_equal(A, B));

	mat_free(A);
	mat_free(B);
}

void test_matrix_duplication(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(2);
	struct matrix *B = mat_copy(NULL, A);

	assert_true(mat_equal(A, B));

	mat_free(A);
	mat_free(B);
}

void test_matrix_addition(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(2);
	struct matrix *B = mat_identity_new(2);

	struct matrix *C = mat_add(NULL, A, B);

	if (!jl_test_mat(C, "[2 0; 0 2]"))
		fail();

	mat_free(A);
	mat_free(B);
	mat_free(C);
}

void test_matrix_subtraction(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(2);
	struct matrix *B = mat_identity_new(2);

	struct matrix *C = mat_sub(NULL, A, B);

	if (!jl_test_mat(C, "[0 0; 0 0]"))
		fail();

	mat_free(A);
	mat_free(B);
	mat_free(C);
}

void test_matrix_multiplication(void **state)
{
	(void)state;

	struct matrix *A = mat_identity_new(3);
	struct matrix *B = mat_identity_new(3);

	struct matrix *C = mat_mul(NULL, A, B);

	assert_true(mat_equal(A, C));

	mat_free(A);
	mat_free(B);
	mat_free(C);
}

void test_matrix_transposition(void **state)
{
	(void)state;

	struct matrix *A = mat_alloc(2, 3);
	mat_set(A, 0, 1, 1);
	mat_set(A, 1, 2, 1);

	struct matrix *T = mat_trans(NULL, A);

	if (!jl_test_mat(T, "[0 0; 1 0; 0 1]"))
		fail();

	mat_free(A);
	mat_free(T);
}

/*
 * Floating-point matrix tests
 */

void test_fmatrix_stack_creation(void **state)
{
	(void)state;

	FMATRIX(A, 3, 3);

	fmat_set(A, 0, 0, 1);
	fmat_set(A, 0, 1, 1);
	fmat_set(A, 0, 2, 1);

	if (!jl_test_fmat(A, "A = [1. 1. 1.; 0. 0. 0.; 0. 0. 0.]", 1e-12, 1e-12)) {
		fail();
	}
}

void test_fmatrix_heap_creation(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 3);

	fmat_set(A, 0, 0, 1);
	fmat_set(A, 0, 1, 1);
	fmat_set(A, 0, 2, 1);

	if (!jl_test_fmat(A, "A = [1. 1. 1.; 0. 0. 0.; 0. 0. 0.]", 1e-12, 1e-12)) {
		fail();
	}
}

void test_fmatrix_heap_multiplication(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 3);
	struct fmatrix *B = fmat_alloc(3, 3);

	fmat_set(A, 0, 0, 1);
	fmat_set(A, 0, 1, 1);
	fmat_set(A, 0, 2, 1);

	fmat_set(B, 0, 0, 1);
	fmat_set(B, 1, 0, 1);
	fmat_set(B, 2, 0, 1);

	struct fmatrix *C = fmat_mul(NULL, A, B);

	if (!jl_test_fmat(C,
			  "[1. 1. 1.;"
			  " 0. 0. 0.;"
			  " 0. 0. 0.] *"
			  "[1. 0. 0.;"
			  " 1. 0. 0.;"
			  " 1. 0. 0.]",
			  1e-12,
			  1e-12)) {
		fail();
	}
}

void test_fmatrix_alloc_valid(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(2, 3);
	assert_non_null(A);
	assert_int_equal(A->rows, 2);
	assert_int_equal(A->cols, 3);

	fmat_free(A);
}

void test_fmatrix_alloc_invalid_dims(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(0, 3);
	assert_null(A);
}

void test_fmatrix_set_identity(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 3);
	fmat_set_identity(A);

	if (!jl_test_fmat(A, "[1. 0. 0.; 0. 1. 0.; 0. 0. 1.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_identity_new(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(3);

	if (!jl_test_fmat(A, "[1. 0. 0.; 0. 1. 0.; 0. 0. 1.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_set_and_reset(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(2, 3);
	fmat_set(A, 0, 1, 5);
	fmat_reset(A);

	if (!jl_test_fmat(A, "[0. 0. 0.; 0. 0. 0]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_set_row_gf2(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(1, 4);
	fmat_set_row_gf2(A, 0, 0b1010);

	if (!jl_test_fmat(A, "[1. 0. 1. 0]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_set_string(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_set_string("[1. 0. 0.; 0. 1. 0.; 0. 0. 1.]");

	if (!jl_test_fmat(A, "[1. 0. 0.; 0. 1. 0.; 0. 0. 1.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_shift_east(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(1, 4);
	fmat_set_row_gf2(A, 0, 0b1111);

	fmat_shift_east(A, 2);

	if (!jl_test_fmat(A, "[0. 0. 1. 1.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_shift_west(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(1, 4);
	fmat_set_row_gf2(A, 0, 0b1111);

	fmat_shift_west(A, 2);

	if (!jl_test_fmat(A, "[1. 1. 0. 0.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_shift_north(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 1);
	fmat_set(A, 2, 0, 1);

	fmat_shift_north(A, 1);

	if (!jl_test_fmat(A, "[0.; 1.; 0.;;]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_shift_south(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 1);
	fmat_set(A, 0, 0, 1);

	fmat_shift_south(A, 1);

	if (!jl_test_fmat(A, "[0.; 1.; 0.;;]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
}

void test_fmatrix_copy(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(2);
	struct fmatrix *B = fmat_alloc(2, 2);

	fmat_copy(B, A);

	assert_true(fmat_equal(A, B));

	fmat_free(A);
	fmat_free(B);
}

void test_fmatrix_duplication(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(2);
	struct fmatrix *B = fmat_copy(NULL, A);

	assert_true(fmat_equal(A, B));

	fmat_free(A);
	fmat_free(B);
}

void test_fmatrix_addition(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(2);
	struct fmatrix *B = fmat_identity_new(2);

	struct fmatrix *C = fmat_add(NULL, A, B);

	if (!jl_test_fmat(C, "[2. 0.; 0. 2.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
	fmat_free(B);
	fmat_free(C);
}

void test_fmatrix_subtraction(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(2);
	struct fmatrix *B = fmat_identity_new(2);

	struct fmatrix *C = fmat_sub(NULL, A, B);

	if (!jl_test_fmat(C, "[0. 0.; 0. 0.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
	fmat_free(B);
	fmat_free(C);
}

void test_fmatrix_multiplication(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_identity_new(3);
	struct fmatrix *B = fmat_identity_new(3);

	struct fmatrix *C = fmat_mul(NULL, A, B);

	assert_true(fmat_equal(A, C));

	fmat_free(A);
	fmat_free(B);
	fmat_free(C);
}

void test_fmatrix_transposition(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(2, 3);
	fmat_set(A, 0, 1, 1);
	fmat_set(A, 1, 2, 1);

	struct fmatrix *T = fmat_trans(NULL, A);

	if (!jl_test_fmat(T, "[0. 0.; 1. 0.; 0. 1.]", 1e-12, 1e-12))
		fail();

	fmat_free(A);
	fmat_free(T);
}

void test_fmatrix_inverse(void **state)
{
	(void)state;

	struct fmatrix *A = fmat_alloc(3, 3);
	fmat_set(A, 0, 0, 2);
	fmat_set(A, 1, 0, 1);
	fmat_set(A, 2, 0, 0);
	fmat_set(A, 0, 1, -1);
	fmat_set(A, 1, 1, 2);
	fmat_set(A, 2, 1, -1);
	fmat_set(A, 0, 2, 0);
	fmat_set(A, 1, 2, -2);
	fmat_set(A, 2, 2, 1);

	struct fmatrix *T = fmat_inv(NULL, A);

	if (!jl_test_fmat(T,
			  "inv([2. -1. 0.;"
			  "     1. 2. -2.;"
			  "     0. -1. 1.])",
			  1e-12,
			  1e-12))
		fail();

	fmat_free(A);
	fmat_free(T);
}
