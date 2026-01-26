#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Scalar type for matrix elements */
typedef long long val_t;

struct matrix {
	const size_t cols, rows;
	val_t **data;
};

/* Stack-allocated matrix */
#define MATRIX(name, R, C)                                   \
	val_t name##_buf[R][C];                              \
	val_t *name##_rowptrs[R];                            \
	for (size_t i = 0; i < (R); i++)                     \
		name##_rowptrs[i] = name##_buf[i];           \
	struct matrix name##_obj = { C, R, name##_rowptrs }; \
	struct matrix *name = &name##_obj;                   \
	memset(name##_buf, 0, sizeof(name##_buf))

#define MAT_MUL(name, A, B)                                                               \
	MATRIX(name, (A)->rows, (B)->cols);                                               \
	do {                                                                              \
		if ((A)->cols != (B)->rows) {                                             \
			fprintf(stderr, "MAT_MUL: dimension mismatch\n");                 \
		} else {                                                                  \
			for (size_t i = 0; i < (A)->rows; i++) {                          \
				for (size_t j = 0; j < (B)->cols; j++) {                  \
					val_t sum = 0.0;                                  \
					for (size_t k = 0; k < (A)->cols; k++)            \
						sum += (A)->data[i][k] * (B)->data[k][j]; \
					name->data[i][j] = sum;                           \
				}                                                         \
			}                                                                 \
		}                                                                         \
	} while (0)

#define MAT_ADD(name, A, B)                                                                   \
	MATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "MAT_ADD: dimension mismatch\n");                     \
		} else {                                                                      \
			for (size_t i = 0; i < (A)->rows; i++) {                              \
				for (size_t j = 0; j < (A)->cols; j++) {                      \
					name->data[i][j] = (A)->data[i][j] + (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define MAT_SUB(name, A, B)                                                                   \
	MATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "MAT_SUB: dimension mismatch\n");                     \
		} else {                                                                      \
			for (size_t i = 0; i < (A)->rows; i++) {                              \
				for (size_t j = 0; j < (A)->cols; j++) {                      \
					name->data[i][j] = (A)->data[i][j] - (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define MAT_TRANS(name, A)                                      \
	MATRIX(name, (A)->cols, (A)->rows);                         \
	do {                                                        \
		for (size_t i = 0; i < (A)->rows; i++) {            \
			for (size_t j = 0; j < (A)->cols; j++) {    \
				name->data[j][i] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

#define MAT_COPY(name, A)                                           \
	MATRIX(name, (A)->rows, (A)->cols);                         \
	do {                                                        \
		for (size_t i = 0; i < (A)->rows; i++) {            \
			for (size_t j = 0; j < (A)->cols; j++) {    \
				name->data[i][j] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

/* Allocate an empty matrix */
struct matrix *mat_alloc(const size_t rows, const size_t cols);
/* Delete a matrix */
void mat_delete(struct matrix *m);

/* Set the matrix to an idenitity matrix */
void mat_set_identity(struct matrix *m);
/* Allocate a new identity matrix */
struct matrix *mat_identity_new(const size_t dims);

/* Shift all fields of the matrix up */
void mat_shift_north(struct matrix *m, size_t nshifts);
/* Shift all fields of the matrix down */
void mat_shift_south(struct matrix *m, size_t nshifts);
/* Shift all fields of the matrix right */
void mat_shift_east(struct matrix *m, size_t nshifts);
/* Shift all fields of the matrix left */
void mat_shift_west(struct matrix *m, size_t nshifts);

/* Set a single field of a matrix */
void mat_set(struct matrix *m, size_t row, size_t col, val_t val);
/* Get a single field of a matrix */
val_t mat_get(struct matrix *m, size_t row, size_t col);

/* GF(2) helper that sets a row of a matrix to the bits of an integer */
void mat_set_row_gf2(struct matrix *m, size_t row, unsigned long long bits);

/* Print a matrix */
void mat_print(struct matrix *m);

/* Add two matrices */
struct matrix *mat_add(const struct matrix *a, const struct matrix *b);
/* Subtract two matrices */
struct matrix *mat_sub(const struct matrix *a, const struct matrix *b);
/* Multiply two matrices */
struct matrix *mat_mul(const struct matrix *a, const struct matrix *b);
/* Transpose a matrix */
struct matrix *mat_trans(const struct matrix *m);
/* Copy a matrix */
void mat_copy(struct matrix *dst, const struct matrix *src);
/* Duplicate a matrix */
struct matrix *mat_dup(const struct matrix *src);

/* Compare two matrices */
bool mat_equal(const struct matrix *a, const struct matrix *b);

#endif /* MATRIX_H */
