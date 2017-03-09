#include <gsl/gsl_eigen.h>

int main(int argc, char* argv[]){
  int i, n, x;

  gsl_eigen_symm_workspace *w;
  gsl_matrix* m;
  gsl_vector* v;

  scanf("%d ", &n);

  v = gsl_vector_alloc(n);
  m = gsl_matrix_alloc(n, n);

  for (i = 0; i < n*n; ++i){
    scanf("%d ", &x);
    gsl_matrix_set(m, i / n, i % n, x);
  }

  /*
  for (i = 0; i < N; ++i){
    for (j = 0; j < N; ++j){
      gsl_matrix_set(m, i, j, C5[i][j]);
    }
  }
  */

  w = gsl_eigen_symm_alloc(n);

  gsl_eigen_symm(m, v, w);
  gsl_sort_vector(v);
  gsl_vector_reverse(v);

  for (i = 0; i < n; ++i){
    printf("%6.3lf\n", gsl_vector_get(v, i));
  }

  gsl_eigen_symm_free(w);
  gsl_matrix_free(m);
  gsl_vector_free(v);

  return 0;
}
