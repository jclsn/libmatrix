

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

struct matrix *matrix_alloc(const int rows, const int cols)
{
	if (!rows || !cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	int row;

	/* Allocate the struct */
	struct matrix *m = malloc(sizeof(struct matrix));
	if (!m) {
		perror(__func__);
	}

	/* Copy over a temporary matrix that holds the const rows and columns */
	struct matrix m_temp = { .cols = cols, .rows = rows, .data = NULL };
	memcpy(m, &m_temp, sizeof(struct matrix));

	/* Allocate the rows */
	m->data = calloc(rows, sizeof(int *));
	if (!m->data) {
		perror(__func__);
		goto error_rows;
	}

	/* Allocate the columns */
	for (row = 0; row < rows; row++) {
		m->data[row] = calloc(cols, sizeof(int));
		if (!m->data[row]) {
			perror(__func__);
			goto error_columns;
		}
	}

	return m;

error_columns:
	row--;
	for (; row >= 0; row--)
		free(m->data[row]);

	free(m->data);

error_rows:
	free(m);

	return NULL;
}

void matrix_delete(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	/* Free the rows */
	for (int row = m->rows - 1; row >= 0; row--) {
		free(m->data[row]);
		m->data[row] = NULL;
	}

	/* Free the rows */
	free(m->data);
	m->data = NULL;

	/* Free the struct */
	free(m);
	m = NULL;
}

void matrix_set_identity(struct matrix *m)
{
	if (!m)
		return;

	if (m->rows != m->cols) {
		printf("rows: %d, cols: %d\nOnly an  m x m matrix can become an identity matrix\n",
		       m->rows,
		       m->cols);
		return;
	}

	for (int col = 0; col < m->cols; ++col)
		for (int row = 0; row < m->rows; row++)
			if (col == row)
				m->data[col][row] = 1;
}

struct matrix *matrix_identity_new(const unsigned int dims)
{
	if (!dims) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = matrix_alloc(dims, dims);
	matrix_set_identity(m);

	return m;
}

void matrix_print(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int row = 0; row < m->rows; row++) {
		for (int col = 0; col < m->cols; col++)
			printf("%u  ", m->data[row][col]);

		printf("\n");
	}
}

void matrix_shift_east(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int row = 0; row < m->rows; row++) {
		for (int col = m->cols - 1; col >= 0; col--) {
			if (col == 0)
				m->data[row][col] = 0;
			else
				m->data[row][col] = m->data[row][col - 1];
		}
	}
}

void matrix_set_field(struct matrix *m, int row, int col, int val)
{
	if (!m || row > m->rows || col > m->cols) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	m->data[row][col] = val;
}

void matrix_set_row_gf2(struct matrix *m, int row, int bits)
{
	if (!m || row > m->rows) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int col = 0; col < m->cols; col++)
		matrix_set_field(m, row, col, (bits >> ((m->cols - 1) - col) & 0x1));
}

int main(int argc, char *argv[])
{
	struct matrix *m;

	m = matrix_identity_new(22);

	matrix_shift_east(m);
	matrix_set_row_gf2(m, 21, 0xff);
	matrix_print(m);

	matrix_delete(m);

	return 0;
}
