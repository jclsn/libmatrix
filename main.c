#include "matrix.h"

int main()
{
	struct matrix *A = mat_alloc(2, 2);

	mat_set_identity(A);
	mat_print(A);
	mat_delete(A);

	return 0;
}
