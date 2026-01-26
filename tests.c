#include "tests.h"

// only define this once, in an executable
JULIA_DEFINE_FAST_TLS

static void mat_to_julia_matrix(const struct matrix *m, const char *julia_name)
{
	char buf[2048];
	size_t pos = 0;

	pos += snprintf(buf + pos, sizeof(buf) - pos, "%s = reshape([", julia_name);

	for (int c = 0; c < m->cols; c++) {
		for (int r = 0; r < m->rows; r++) {
			pos += snprintf(buf + pos, sizeof(buf) - pos, "%lld", m->data[r][c]);
			if (r != m->rows - 1 || c != m->cols - 1)
				pos += snprintf(buf + pos, sizeof(buf) - pos, ", ");
		}
	}

	snprintf(buf + pos, sizeof(buf) - pos, "], %zu, %zu)", m->rows, m->cols);

	jl_eval_string(buf);
}

int jl_test_mat(const struct matrix *m, const char *julia_expr)
{
	if (!m || !julia_expr)
		return 0;

	/* Create C-side matrix in Julia as `Cmat` */
	mat_to_julia_matrix(m, "Cmat");

	/* Evaluate user expression as `Jmat` */
	char cmd[2048];
	snprintf(cmd, sizeof(cmd), "Jmat = %s", julia_expr);
	jl_eval_string(cmd);

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception while evaluating expression: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	/* Compare */
	jl_value_t *res =
		jl_eval_string("display(Cmat); display(Jmat); res = Cmat == Jmat; flush(stdout)");

	jl_eval_string("if res\n"
		       "    printstyled(\"PASS\\n\", color=:green)\n"
		       "else\n"
		       "    printstyled(\"FAIL\\n\", color=:red)\n"
		       "end\n"
		       "flush(stdout)");

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception during comparison: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	return jl_unbox_bool(res);
}

static void fmat_to_julia_matrix(const struct fmatrix *m, const char *julia_name)
{
	char buf[4096];
	size_t pos = 0;

	pos += snprintf(buf + pos, sizeof(buf) - pos, "%s = reshape(Float64[", julia_name);

	for (int c = 0; c < m->cols; c++) {
		for (int r = 0; r < m->rows; r++) {
			pos += snprintf(buf + pos, sizeof(buf) - pos, "%.17g", m->data[r][c]);
			if (r != m->rows - 1 || c != m->cols - 1)
				pos += snprintf(buf + pos, sizeof(buf) - pos, ", ");
		}
	}

	snprintf(buf + pos, sizeof(buf) - pos, "], %zu, %zu)", m->rows, m->cols);

	jl_eval_string(buf);
}

int jl_test_fmat(const struct fmatrix *m, const char *julia_expr, double rtol, double atol)
{
	if (!m || !julia_expr)
		return 0;

	/* Push C matrix into Julia as Cmat */
	fmat_to_julia_matrix(m, "Cmat");

	/* Evaluate Julia-side matrix */
	char cmd[2048];
	snprintf(cmd, sizeof(cmd), "Jmat = %s", julia_expr);
	jl_eval_string(cmd);

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception while evaluating expression: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	/* jl_value_t *res = jl_eval_string( */
	/* 	"display(Cmat); display(Jmat); display(Cmat == Jmat); flush(stdout)"); */

	/* Compare with tolerance */
	snprintf(cmd,
		 sizeof(cmd),
		 "display(Cmat); "
		 "display(Jmat); "
		 "res = isapprox(Cmat, Jmat; rtol=%g, atol=%g); "
		 "flush(stdout); "
		 "res",
		 rtol,
		 atol);

	jl_value_t *res = jl_eval_string(cmd);

	jl_eval_string("if res\n"
		       "    printstyled(\"PASS\\n\", color=:green)\n"
		       "else\n"
		       "    printstyled(\"FAIL\\n\", color=:red)\n"
		       "end\n"
		       "flush(stdout)");

	if (jl_exception_occurred()) {
		fprintf(stderr,
			"Julia exception during float comparison: %s\n",
			jl_typeof_str(jl_exception_occurred()));
		return 0;
	}

	return jl_unbox_bool(res);
}
