#include <errno.h>
#include <stdlib.h>

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
	printf("\n");
}

/* Shifts */

void matrix_shift_east(struct matrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int r = 0; r < m->rows; r++) {
		for (int c = m->cols - 1; c >= 0; c--) {
			int src = c - nshifts;
			m->data[r][c] = (src >= 0) ? m->data[r][src] : 0;
		}
	}
}

void matrix_shift_west(struct matrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int r = 0; r < m->rows; r++) {
		for (int c = 0; c < m->cols; c++) {
			int src = c + nshifts;
			m->data[r][c] = (src < m->cols) ? m->data[r][src] : 0;
		}
	}
}

void matrix_shift_north(struct matrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int r = 0; r < m->rows; r++) {
		int src = r + nshifts;
		for (int c = 0; c < m->cols; c++) {
			m->data[r][c] = (src < m->rows) ? m->data[src][c] : 0;
		}
	}
}

void matrix_shift_south(struct matrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int r = m->rows - 1; r >= 0; r--) {
		int src = r - nshifts;
		for (int c = 0; c < m->cols; c++) {
			m->data[r][c] = (src >= 0) ? m->data[src][c] : 0;
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

void matrix_set_row_gf2(struct matrix *m, int row, unsigned long long bits)
{
	if (!m || row > m->rows - 1) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int col = 0; col < m->cols; col++)
		matrix_set_field(m, row, col, (bits >> ((m->cols - 1) - col) & 0x1));
}

/* ---------------- Operations ---------------- */

struct matrix *matrix_copy(const struct matrix *src)
{
	if (!src)
		return NULL;
	struct matrix *m = matrix_alloc(src->rows, src->cols);
	if (!m)
		return NULL;

	for (int r = 0; r < src->rows; r++)
		for (int c = 0; c < src->cols; c++)
			m->data[r][c] = src->data[r][c];

	return m;
}

int matrix_equal(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols)
		return 0;

	for (int r = 0; r < a->rows; r++)
		for (int c = 0; c < a->cols; c++)
			if (a->data[r][c] != b->data[r][c])
				return 0;

	return 1;
}

struct matrix *matrix_add(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols)
		return NULL;

	struct matrix *m = matrix_alloc(a->rows, a->cols);
	if (!m)
		return NULL;

	for (int r = 0; r < a->rows; r++)
		for (int c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] + b->data[r][c];

	return m;
}

struct matrix *matrix_sub(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols)
		return NULL;

	struct matrix *m = matrix_alloc(a->rows, a->cols);
	if (!m)
		return NULL;

	for (int r = 0; r < a->rows; r++)
		for (int c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] - b->data[r][c];

	return m;
}

struct matrix *matrix_mul(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->cols != b->rows)
		return NULL;

	struct matrix *m = matrix_alloc(a->rows, b->cols);
	if (!m)
		return NULL;

	for (int i = 0; i < a->rows; i++) {
		for (int j = 0; j < b->cols; j++) {
			int sum = 0;
			for (int k = 0; k < a->cols; k++)
				sum += a->data[i][k] * b->data[k][j];
			m->data[i][j] = sum;
		}
	}
	return m;
}

struct matrix *matrix_transpose(const struct matrix *src)
{
	if (!src)
		return NULL;
	struct matrix *m = matrix_alloc(src->cols, src->rows);
	if (!m)
		return NULL;

	for (int r = 0; r < src->rows; r++)
		for (int c = 0; c < src->cols; c++)
			m->data[c][r] = src->data[r][c];

	return m;
}
