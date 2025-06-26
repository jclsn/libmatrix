#ifndef MATRIX_H

#define MATRIX_H

struct matrix {
	const unsigned int cols, rows;
	int **data;
};

/* Create an empty matrix */
struct matrix *matrix_alloc(const int rows, const int cols);

/* Release the memory for a matrix */
void matrix_delete(struct matrix *m);

/* Turn an empty matrix into an identity matrix */
void matrix_set_identity(struct matrix *m);

/* Create a new identity matrix on the heap */
struct matrix *matrix_identity_new(const unsigned int dims);

/* Shift all fields eastwards */
void matrix_shift_east(struct matrix *m);

/* Set the field of a matrix */
void matrix_set_field(struct matrix *m, int row, int col, int val);

/* 
 * Set the row of a GF(2) matrix to the bits of an integer. The function does
 * not check if the passed matrix has been populated with GF(2) values. 
 */
void matrix_set_row_gf2(struct matrix *m, int bits, int row);

/* Print the matrix */
void matrix_print(struct matrix *m);

#endif /* end of include guard MATRIX_H */

