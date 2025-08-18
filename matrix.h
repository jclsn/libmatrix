#ifndef MATRIX_H

#define MATRIX_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct matrix {
	const unsigned int cols, rows;
	int **data;
};

#define MATRIX(name, R, C)                                      \
	int name##_buf[R][C];                                   \
	int *name##_rowptrs[R];                                 \
	for (int i = 0; i < (R); i++)                           \
		name##_rowptrs[i] = name##_buf[i];              \
	struct matrix name##_obj = { R, C, name##_rowptrs }; \
	struct matrix *name = &name##_obj;                   \
	memset(name##_buf, 0, sizeof(name##_buf))

#define MATRIX_MUL(name, A, B)                                                            \
	MATRIX(name, (A)->rows, (B)->cols);                                               \
	do {                                                                              \
		if ((A)->cols != (B)->rows) {                                             \
			fprintf(stderr, "MATRIX_MUL: dimension mismatch\n");              \
		} else {                                                                  \
			for (int i = 0; i < (A)->rows; i++) {                             \
				for (int j = 0; j < (B)->cols; j++) {                     \
					int sum = 0;                                      \
					for (int k = 0; k < (A)->cols; k++)               \
						sum += (A)->data[i][k] * (B)->data[k][j]; \
					name->data[i][j] = sum;                           \
				}                                                         \
			}                                                                 \
		}                                                                         \
	} while (0)

#define MATRIX_ADD(name, A, B)                                                                \
	MATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "MATRIX_ADD: dimension mismatch\n");                  \
		} else {                                                                      \
			for (int i = 0; i < (A)->rows; i++) {                                 \
				for (int j = 0; j < (A)->cols; j++) {                         \
					name->data[i][j] = (A)->data[i][j] + (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define MATRIX_SUB(name, A, B)                                                                \
	MATRIX(name, (A)->rows, (A)->cols);                                                   \
	do {                                                                                  \
		if ((A)->rows != (B)->rows || (A)->cols != (B)->cols) {                       \
			fprintf(stderr, "MATRIX_SUB: dimension mismatch\n");                  \
		} else {                                                                      \
			for (int i = 0; i < (A)->rows; i++) {                                 \
				for (int j = 0; j < (A)->cols; j++) {                         \
					name->data[i][j] = (A)->data[i][j] - (B)->data[i][j]; \
				}                                                             \
			}                                                                     \
		}                                                                             \
	} while (0)

#define MATRIX_TRANSPOSE(name, A)                                   \
	MATRIX(name, (A)->cols, (A)->rows);                         \
	do {                                                        \
		for (int i = 0; i < (A)->rows; i++) {               \
			for (int j = 0; j < (A)->cols; j++) {       \
				name->data[j][i] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

#define MATRIX_COPY(name, A)                                        \
	MATRIX(name, (A)->rows, (A)->cols);                         \
	do {                                                        \
		for (int i = 0; i < (A)->rows; i++) {               \
			for (int j = 0; j < (A)->cols; j++) {       \
				name->data[i][j] = (A)->data[i][j]; \
			}                                           \
		}                                                   \
	} while (0)

/* Create an empty matrix */
struct matrix *matrix_alloc(const int rows, const int cols);

/* Release the memory for a matrix */
void matrix_delete(struct matrix *m);

/* Turn an empty matrix into an identity matrix */
void matrix_set_identity(struct matrix *m);

/* Create a new identity matrix on the heap */
struct matrix *matrix_identity_new(const unsigned int dims);

/* Shift all fields eastwards */
void matrix_shift_north(struct matrix *m, size_t nshifts);
void matrix_shift_south(struct matrix *m, size_t nshifts);
void matrix_shift_east(struct matrix *m, size_t nshifts);
void matrix_shift_west(struct matrix *m, size_t nshifts);

/* Getters and setters */
void matrix_set_field(struct matrix *m, int row, int col, int val);
int matrix_get_field(struct matrix *m, int row, int col);

/*
 * Set the row of a GF(2) matrix to the bits of an integer. The function does
 * not check if the passed matrix has been populated with GF(2) values.
 */
void matrix_set_row_gf2(struct matrix *m, int bits, unsigned long long row);

/* Print the matrix */
void matrix_print(struct matrix *m);

/* Operations */
struct matrix *matrix_add(const struct matrix *a, const struct matrix *b);
struct matrix *matrix_sub(const struct matrix *a, const struct matrix *b);
struct matrix *matrix_mul(const struct matrix *a, const struct matrix *b);
struct matrix *matrix_transpose(const struct matrix *m);
struct matrix *matrix_copy(const struct matrix *src);

int matrix_equal(const struct matrix *a, const struct matrix *b);

#endif /* end of include guard MATRIX_H */
