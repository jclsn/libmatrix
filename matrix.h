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

#define MAT_TRANSPOSE(name, A)                                      \
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

/* Heap-allocated matrix */
struct matrix *mat_alloc(const size_t rows, const size_t cols);
void mat_delete(struct matrix *m);

/* Identity */
void mat_set_identity(struct matrix *m);
struct matrix *mat_identity_new(const size_t dims);

/* Shifts */
void mat_shift_north(struct matrix *m, size_t nshifts);
void mat_shift_south(struct matrix *m, size_t nshifts);
void mat_shift_east(struct matrix *m, size_t nshifts);
void mat_shift_west(struct matrix *m, size_t nshifts);

/* Accessors */
void mat_set_field(struct matrix *m, size_t row, size_t col, val_t val);
val_t mat_get_field(struct matrix *m, size_t row, size_t col);

/*
 * GF(2) helper.
 * Still conceptually integer-based, values are written as 0.0 or 1.0.
 */
void mat_set_row_gf2(struct matrix *m, size_t row, unsigned long long bits);

/* Output */
void mat_print(struct matrix *m);

/* Operations */
struct matrix *mat_add(const struct matrix *a, const struct matrix *b);
struct matrix *mat_sub(const struct matrix *a, const struct matrix *b);
struct matrix *mat_mul(const struct matrix *a, const struct matrix *b);
struct matrix *mat_transpose(const struct matrix *m);
struct matrix *mat_copy(const struct matrix *src);

bool mat_equal(const struct matrix *a, const struct matrix *b);

#endif /* MATRIX_H */
