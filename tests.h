#ifndef TESTS_H

#define TESTS_H

#include <julia/julia.h>
#include <stddef.h>

#include "fmatrix.h"
#include "matrix.h"

int jl_test_mat(const struct matrix *m, const char *julia_expr);
int jl_test_fmat(const struct fmatrix *m, const char *julia_expr, double rtol, double atol);

#endif /* end of include guard TESTS_H */
