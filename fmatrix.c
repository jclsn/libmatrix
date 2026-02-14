#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "fmatrix.h"

struct fmatrix *fmat_alloc(const size_t rows, const size_t cols)
{
	if (!rows || !cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	size_t row;

	/* Allocate the struct */
	struct fmatrix *m = malloc(sizeof(struct fmatrix));
	if (!m) {
		perror(__func__);
		goto error;
	}

	/* Copy over a temporary matrix that holds the const rows and columns */
	struct fmatrix m_temp = { .cols = cols, .rows = rows, .data = NULL };
	memcpy(m, &m_temp, sizeof(struct fmatrix));

	/* Allocate the rows */
	m->data = calloc(rows, sizeof(fval_t *));
	if (!m->data) {
		perror(__func__);
		goto error_rows;
	}

	/* Allocate the columns */
	for (row = 0; row < rows; row++) {
		m->data[row] = calloc(cols, sizeof(fval_t));
		if (!m->data[row]) {
			perror(__func__);
			goto error_columns;
		}
	}

	return m;

error_columns:
	while (row > 0) {
		row--;
		free(m->data[row]);
	}

	free(m->data);
error_rows:
	free(m);
error:
	return NULL;
}

void fmat_delete(struct fmatrix *m)
{
	if (!m)
		return;

	for (size_t row = 0; row < m->rows; row++)
		if (m->data[row])
			free(m->data[row]);

	if (m->data)
		free(m->data);

	free(m);
}

void fmat_set_identity(struct fmatrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	if (m->rows != m->cols) {
		printf("rows: %zu, cols: %zu\nOnly an  m x m matrix can become an identity matrix\n",
		       m->rows,
		       m->cols);
		return;
	}

	for (size_t col = 0; col < m->cols; ++col)
		for (size_t row = 0; row < m->rows; row++)
			if (col == row)
				m->data[col][row] = 1;
}

struct fmatrix *fmat_identity_new(const size_t dims)
{
	if (!dims) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(dims, dims);
	fmat_set_identity(m);

	return m;
}

void fmat_print(struct fmatrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t row = 0; row < m->rows; row++) {
		for (size_t col = 0; col < m->cols; col++)
			printf("%.1f  ", m->data[row][col]);

		printf("\n");
	}
	printf("\n");
}

/* Shifts */

void fmat_shift_east(struct fmatrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t r = 0; r < m->rows; r++) {
		for (ssize_t c = m->cols - 1; c >= 0; c--) {
			ssize_t src = c - nshifts;
			m->data[r][c] = (src >= 0) ? m->data[r][src] : 0;
		}
	}
}

void fmat_shift_west(struct fmatrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t r = 0; r < m->rows; r++) {
		for (size_t c = 0; c < m->cols; c++) {
			size_t src = c + nshifts;
			m->data[r][c] = (src < m->cols) ? m->data[r][src] : 0;
		}
	}
}

void fmat_shift_north(struct fmatrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t r = 0; r < m->rows; r++) {
		size_t src = r + nshifts;
		for (size_t c = 0; c < m->cols; c++) {
			m->data[r][c] = (src < m->rows) ? m->data[src][c] : 0;
		}
	}
}

void fmat_shift_south(struct fmatrix *m, size_t nshifts)
{
	if (!m || !nshifts) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (ssize_t r = m->rows - 1; r >= 0; r--) {
		ssize_t src = r - nshifts;
		for (size_t c = 0; c < m->cols; c++) {
			m->data[r][c] = (src >= 0) ? m->data[src][c] : 0;
		}
	}
}

void fmat_set(struct fmatrix *m, size_t row, size_t col, fval_t val)
{
	if (!m || row > m->rows || col > m->cols) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	m->data[row][col] = val;
}

void fmat_reset(struct fmatrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t row = 0; row < m->rows; row++)
		for (size_t col = 0; col < m->cols; col++)
			m->data[row][col] = 0.0;
}

void fmat_set_row_gf2(struct fmatrix *m, size_t row, unsigned long long bits)
{
	if (!m || row > m->rows - 1) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int col = 0; col < m->cols; col++)
		fmat_set(m, row, col, (bits >> ((m->cols - 1) - col) & 0x1));
}

/* ---------------- Operations ---------------- */

void fmat_copy(struct fmatrix *dst, const struct fmatrix *src)
{
	if (!src || !dst) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			dst->data[r][c] = src->data[r][c];
}

struct fmatrix *fmat_dup(const struct fmatrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(src->rows, src->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			m->data[r][c] = src->data[r][c];

	return m;
}

bool fmat_equal(const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return false;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			if (a->data[r][c] != b->data[r][c])
				return false;

	return true;
}

struct fmatrix *fmat_add(const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(a->rows, a->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] + b->data[r][c];

	return m;
}

struct fmatrix *fmat_sub(const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(a->rows, a->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] - b->data[r][c];

	return m;
}

struct fmatrix *fmat_mul(const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->cols != b->rows) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(a->rows, b->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t i = 0; i < a->rows; i++) {
		for (size_t j = 0; j < b->cols; j++) {
			fval_t sum = 0;
			for (size_t k = 0; k < a->cols; k++)
				sum += a->data[i][k] * b->data[k][j];
			m->data[i][j] = sum;
		}
	}

	return m;
}

struct fmatrix *fmat_trans(const struct fmatrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *m = fmat_alloc(src->cols, src->rows);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			m->data[c][r] = src->data[r][c];

	return m;
}

struct fmatrix *fmat_inv(const struct fmatrix *src)
{
	if (!src || src->rows != src->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct fmatrix *inv = fmat_alloc(src->rows, src->cols);
	if (!inv) {
		perror(__func__);
		return NULL;
	}

	struct fmatrix *a = fmat_alloc(src->rows, src->cols);
	if (!a) {
		perror(__func__);
		fmat_delete(inv);
		return NULL;
	}

	double *inv_row = NULL;
	double *a_row = NULL;
	double *inv_i = NULL;
	double factor = 0.0;
	double pivot = 0.0;
	double *a_i = NULL;
	size_t max_row = 0;
	double tmp = 0.0;

	/* Copy src to a, initialize inv as identity */

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++) {
			a->data[r][c] = (double)src->data[r][c];
			inv->data[r][c] = (r == c) ? 1.0 : 0.0;
		}

	for (size_t r = 0; r < src->rows; r++) {

		/* Pivot search */

		max_row = r;

		for (size_t i = r; i < src->rows; i++)
			if (fabs(a->data[i][r]) > fabs(a->data[max_row][r]))
				max_row = i;

		if (fabs(a->data[max_row][r]) < 1e-12) {
			fprintf(stderr, "%s: matrix is singular\n", __func__);
			fmat_delete(a);
			fmat_delete(inv);
			return NULL;
		}

		/* Swap rows in a and inv */

		if (max_row != r)
			for (size_t c = 0; c < src->cols; c++) {
				tmp = a->data[r][c];
				a->data[r][c] = a->data[max_row][c];
				a->data[max_row][c] = tmp;

				tmp = inv->data[r][c];
				inv->data[r][c] = inv->data[max_row][c];
				inv->data[max_row][c] = tmp;
			}

		/* Cache pivot row */

		a_row = a->data[r];
		inv_row = inv->data[r];
		pivot = a_row[r];

		/* Eliminate other rows */

		for (size_t i = 0; i < src->rows; i++) {
			if (i == r)
				continue;

			factor = a->data[i][r] / pivot;
			a_i = a->data[i];
			inv_i = inv->data[i];

			for (size_t c = 0; c < src->cols; c++) {
				a_i[c] -= factor * a_row[c];
				inv_i[c] -= factor * inv_row[c];
			}
		}

		/* Normalize pivot row */

		for (size_t c = 0; c < src->cols; c++) {
			a_row[c] /= pivot;
			inv_row[c] /= pivot;
		}
	}

	fmat_delete(a);
	return inv;
}
