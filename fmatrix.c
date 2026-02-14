#include <ctype.h>
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

void fmat_free(struct fmatrix *m)
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

	for (size_t col = 0; col < m->cols; col++)
		fmat_set(m, row, col, (bits >> ((m->cols - 1) - col) & 0x1));
}

/* ---------------- Operations ---------------- */

struct fmatrix *fmat_copy(struct fmatrix *dest, const struct fmatrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != src->rows || dest->cols != src->cols) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(src->rows, src->cols);
		if (!dest)
			return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			dest->data[r][c] = src->data[r][c];

	return dest;
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

struct fmatrix *fmat_add(struct fmatrix *dest, const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != a->rows || dest->cols != a->cols) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(a->rows, a->cols);
		if (!dest)
			return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			dest->data[r][c] = a->data[r][c] + b->data[r][c];

	return dest;
}

struct fmatrix *fmat_sub(struct fmatrix *dest, const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != a->rows || dest->cols != a->cols) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(a->rows, a->cols);
		if (!dest)
			return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			dest->data[r][c] = a->data[r][c] - b->data[r][c];

	return dest;
}

struct fmatrix *fmat_mul(struct fmatrix *dest, const struct fmatrix *a, const struct fmatrix *b)
{
	if (!a || !b || a->cols != b->rows) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != a->rows || dest->cols != b->cols) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(a->rows, b->cols);
		if (!dest)
			return NULL;
	}

	for (size_t i = 0; i < a->rows; i++) {
		for (size_t j = 0; j < b->cols; j++) {
			fval_t sum = 0;
			for (size_t k = 0; k < a->cols; k++)
				sum += a->data[i][k] * b->data[k][j];
			dest->data[i][j] = sum;
		}
	}

	return dest;
}

struct fmatrix *fmat_trans(struct fmatrix *dest, const struct fmatrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != src->cols || dest->cols != src->rows) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(src->cols, src->rows);
		if (!dest)
			return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			dest->data[c][r] = src->data[r][c];

	return dest;
}

struct fmatrix *fmat_inv(struct fmatrix *dest, const struct fmatrix *src)
{
	if (!src || src->rows != src->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	if (dest) {
		if (dest->rows != src->rows || dest->cols != src->cols) {
			errno = EINVAL;
			perror(__func__);
			return NULL;
		}
	} else {
		dest = fmat_alloc(src->rows, src->cols);
		if (!dest)
			return NULL;
	}

	struct fmatrix *tmp_mat = fmat_alloc(src->rows, src->cols);
	if (!tmp_mat) {
		perror(__func__);
		fmat_free(dest);
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
			tmp_mat->data[r][c] = (double)src->data[r][c];
			dest->data[r][c] = (r == c) ? 1.0 : 0.0;
		}

	for (size_t r = 0; r < src->rows; r++) {
		/* Pivot search */

		max_row = r;

		for (size_t i = r; i < src->rows; i++)
			if (fabs(tmp_mat->data[i][r]) > fabs(tmp_mat->data[max_row][r]))
				max_row = i;

		if (fabs(tmp_mat->data[max_row][r]) < 1e-12) {
			fprintf(stderr, "%s: matrix is singular\n", __func__);
			fmat_free(tmp_mat);
			fmat_free(dest);
			return NULL;
		}

		/* Swap rows in a and inv */

		if (max_row != r)
			for (size_t c = 0; c < src->cols; c++) {
				tmp = tmp_mat->data[r][c];
				tmp_mat->data[r][c] = tmp_mat->data[max_row][c];
				tmp_mat->data[max_row][c] = tmp;

				tmp = dest->data[r][c];
				dest->data[r][c] = dest->data[max_row][c];
				dest->data[max_row][c] = tmp;
			}

		/* Cache pivot row */

		a_row = tmp_mat->data[r];
		inv_row = dest->data[r];
		pivot = a_row[r];

		/* Eliminate other rows */

		for (size_t i = 0; i < src->rows; i++) {
			if (i == r)
				continue;

			factor = tmp_mat->data[i][r] / pivot;
			a_i = tmp_mat->data[i];
			inv_i = dest->data[i];

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

	fmat_free(tmp_mat);

	return dest;
}

struct fmatrix *fmat_set_string(const char *str)
{
	if (!str) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	/* Count rows and columns */
	size_t current_cols = 0;
	const char *p = str;
	int in_number = 0;
	size_t rows = 0;
	size_t cols = 0;
	size_t r = 0;
	size_t c = 0;
	char *endptr;

	while (*p) {
		if (isdigit(*p) || *p == '.' || *p == '-' || *p == '+') {
			if (!in_number) {
				in_number = 1;
				current_cols++;
			}
		} else {
			in_number = 0;
			if (*p == ';') {
				if (cols == 0)
					cols = current_cols;
				else if (current_cols != cols) {
					fprintf(stderr,
						"%s: Inconsistent number of columns\n",
						__func__);
					return NULL;
				}
				rows++;
				current_cols = 0;
			}
		}
		p++;
	}
	if (current_cols > 0) {
		if (cols == 0)
			cols = current_cols;
		else if (current_cols != cols) {
			fprintf(stderr, "%s: Inconsistent number of columns\n", __func__);
			return NULL;
		}
		rows++;
	}

	struct fmatrix *m = fmat_alloc(rows, cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	p = str; /* Parse numbers into the matrix */

	while (*p && r < rows) {
		/* Skip non-numeric, non-minus/plus characters except row separator ; */

		while (*p && !isdigit(*p) && *p != '.' && *p != '-' && *p != '+' && *p != ';')
			p++;

		if (!*p)
			break;
		if (*p == ';') {
			r++;
			c = 0;
			p++;
			continue;
		}

		fval_t val = strtod(p, &endptr);
		if (endptr == p) {
			fprintf(stderr, "%s: Failed to parse number near '%s'\n", __func__, p);
			fmat_free(m);
			return NULL;
		}

		if (c >= cols) {
			fprintf(stderr, "%s: Too many columns in row %zu\n", __func__, r);
			fmat_free(m);
			return NULL;
		}

		m->data[r][c++] = val;
		p = endptr;
	}

	return m;
}
