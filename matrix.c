#include <errno.h>
#include <stdlib.h>

#include "matrix.h"

struct matrix *mat_alloc(const size_t rows, const size_t cols)
{
	if (!rows || !cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	ssize_t row;

	/* Allocate the struct */
	struct matrix *m = malloc(sizeof(struct matrix));
	if (!m) {
		perror(__func__);
		goto error;
	}

	/* Copy over a temporary matrix that holds the const rows and columns */
	struct matrix m_temp = { .cols = cols, .rows = rows, .data = NULL };
	memcpy(m, &m_temp, sizeof(struct matrix));

	/* Allocate the rows */
	m->data = calloc(rows, sizeof(val_t *));
	if (!m->data) {
		perror(__func__);
		goto error_rows;
	}

	/* Allocate the columns */
	for (row = 0; row < rows; row++) {
		m->data[row] = calloc(cols, sizeof(val_t));
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
error:
	return NULL;
}

void mat_delete(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	/* Free the rows */
	for (ssize_t row = m->rows - 1; row >= 0; row--) {
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

void mat_set_identity(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	if (m->rows != m->cols) {
		errno = EINVAL;
		perror(__func__);
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

struct matrix *mat_identity_new(const size_t dims)
{
	if (!dims) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(dims, dims);
	mat_set_identity(m);

	return m;
}

void mat_print(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t row = 0; row < m->rows; row++) {
		for (size_t col = 0; col < m->cols; col++)
			printf("%lld  ", m->data[row][col]);

		printf("\n");
	}
	printf("\n");
}

/* Shifts */

void mat_shift_east(struct matrix *m, size_t nshifts)
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

void mat_shift_west(struct matrix *m, size_t nshifts)
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

void mat_shift_north(struct matrix *m, size_t nshifts)
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

void mat_shift_south(struct matrix *m, size_t nshifts)
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

void mat_set(struct matrix *m, size_t row, size_t col, val_t val)
{
	if (!m || row > m->rows || col > m->cols) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	m->data[row][col] = val;
}

void mat_reset(struct matrix *m)
{
	if (!m) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (size_t row = 0; row < m->rows; row++)
		for (size_t col = 0; col < m->cols; col++)
			m->data[row][col] = 0;
}

void mat_set_row_gf2(struct matrix *m, size_t row, unsigned long long bits)
{
	if (!m || row > m->rows - 1) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	for (int col = 0; col < m->cols; col++)
		mat_set(m, row, col, (bits >> ((m->cols - 1) - col) & 0x1));
}

/* ---------------- Operations ---------------- */

void mat_copy(struct matrix *dst, const struct matrix *src)
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

struct matrix *mat_dup(const struct matrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(src->rows, src->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			m->data[r][c] = src->data[r][c];

	return m;
}

bool mat_equal(const struct matrix *a, const struct matrix *b)
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

struct matrix *mat_add(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(a->rows, a->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] + b->data[r][c];

	return m;
}

struct matrix *mat_sub(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(a->rows, a->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < a->rows; r++)
		for (size_t c = 0; c < a->cols; c++)
			m->data[r][c] = a->data[r][c] - b->data[r][c];

	return m;
}

struct matrix *mat_mul(const struct matrix *a, const struct matrix *b)
{
	if (!a || !b || a->cols != b->rows) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(a->rows, b->cols);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t i = 0; i < a->rows; i++) {
		for (size_t j = 0; j < b->cols; j++) {
			val_t sum = 0;
			for (size_t k = 0; k < a->cols; k++)
				sum += a->data[i][k] * b->data[k][j];
			m->data[i][j] = sum;
		}
	}

	return m;
}

struct matrix *mat_trans(const struct matrix *src)
{
	if (!src) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	struct matrix *m = mat_alloc(src->cols, src->rows);
	if (!m) {
		perror(__func__);
		return NULL;
	}

	for (size_t r = 0; r < src->rows; r++)
		for (size_t c = 0; c < src->cols; c++)
			m->data[c][r] = src->data[r][c];

	return m;
}
