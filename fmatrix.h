#ifndef FMATRIX_H
#define FMATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Scalar type for matrix elements */
typedef double fval_t;

struct fmatrix {
	const size_t cols, rows;
	fval_t **data;
};

/* Stack-allocated matrix */
#define FMATRIX(name, R, C)                                   \
	fval_t name##_buf[R][C];                     \
	fval_t *name##_rowptrs[R];                   \
	for (size_t i = 0; i < (R); i++)                        \
		name##_rowptrs[i] = name##_buf[i];           \
	struct fmatrix name##_obj = { C, R, name##_rowptrs }; \
	struct fmatrix *name = &name##_obj;                   \
	memset(name##_buf, 0, sizeof(name##_buf))

#define FMAT_MUL(name, A, B)                                                            \
	FMATRIX(name, (A)->rows, (B)->cols);                                               \
	do {                                                                              \
		if ((A)->cols != (B)->rows) {                                             \
			fprintf(stderr, "FMAT_MUL: dimension mismatch\n");              \
		} else {                                                                  \
			for (size_t i = 0; i < (A)->rows; i++) {                             \
				for (size_t j = 0; j < (B)->cols; j++) {                     \
					fval_t sum = 0.0;                         \
					for (size_t k = 0; k < (A)->cols; k++)               \
						sum += (A)->data[i][k] * (B)->data[k][j]; \
					name->data[i][j] = sum;                           \
				}                                                         \
			}                                                                 \
		}                                                                         \
	} while (0)

#define FMAT_ADD(name, A, B)                                                                \
	FMATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "FMAT_ADD: dimension mismatch\n");                  \
		} else {                                                                      \
			for (size_t i = 0; i < (A)->rows; i++) {                                 \
				for (size_t j = 0; j < (A)->cols; j++) {                         \
					name->data[i][j] = (A)->data[i][j] + (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define FMAT_SUB(name, A, B)                                                                \
	FMATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "FMAT_SUB: dimension mismatch\n");                  \
		} else {                                                                      \
			for (size_t i = 0; i < (A)->rows; i++) {                                 \
				for (size_t j = 0; j < (A)->cols; j++) {                         \
					name->data[i][j] = (A)->data[i][j] - (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define FMAT_TRANSPOSE(name, A)                                   \
	FMATRIX(name, (A)->cols, (A)->rows);                         \
	do {                                                        \
		for (size_t i = 0; i < (A)->rows; i++) {               \
			for (size_t j = 0; j < (A)->cols; j++) {       \
				name->data[j][i] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

#define FMAT_COPY(name, A)                                        \
	FMATRIX(name, (A)->rows, (A)->cols);                         \
	do {                                                        \
		for (size_t i = 0; i < (A)->rows; i++) {               \
			for (size_t j = 0; j < (A)->cols; j++) {       \
				name->data[i][j] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

/* Heap-allocated matrix */
struct fmatrix *fmat_alloc(const size_t rows, const size_t cols);
void fmat_delete(struct fmatrix *m);

/* Identity */
void fmat_set_identity(struct fmatrix *m);
struct fmatrix *fmat_identity_new(const size_t dims);

/* Shifts */
void fmat_shift_north(struct fmatrix *m, size_t nshifts);
void fmat_shift_south(struct fmatrix *m, size_t nshifts);
void fmat_shift_east(struct fmatrix *m, size_t nshifts);
void fmat_shift_west(struct fmatrix *m, size_t nshifts);

/* Accessors */
void fmat_set_field(struct fmatrix *m, size_t row, size_t col, fval_t val);
fval_t fmat_get_field(struct fmatrix *m, size_t row, size_t col);

/*
 * GF(2) helper.
 * Still conceptually integer-based, values are written as 0.0 or 1.0.
 */
void fmat_set_row_gf2(struct fmatrix *m, size_t row, unsigned long long bits);

/* Output */
void fmat_print(struct fmatrix *m);

/* Operations */
struct fmatrix *fmat_add(const struct fmatrix *a, const struct fmatrix *b);
struct fmatrix *fmat_sub(const struct fmatrix *a, const struct fmatrix *b);
struct fmatrix *fmat_mul(const struct fmatrix *a, const struct fmatrix *b);
struct fmatrix *fmat_transpose(const struct fmatrix *m);
struct fmatrix *fmat_copy(const struct fmatrix *src);

bool fmat_equal(const struct fmatrix *a, const struct fmatrix *b);

#endif /* FMATRIX_H */
