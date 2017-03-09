#include <gsl/gsl_eigen.h>
#define N 5

int main(int argc, char* argv[]){
  int i, j;
  int C5[N][N] =
  {
    {0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 1},
    {1, 0, 0, 1, 0}
  };

  gsl_eigen_symm_workspace *w;
  gsl_matrix* m;
  gsl_vector* v;

  v = gsl_vector_alloc(N);
  m = gsl_matrix_alloc(N, N);

  for (i = 0; i < N; ++i){
    for (j = 0; j < N; ++j){
      gsl_matrix_set(m, i, j, C5[i][j]);
    }
  }

  w = gsl_eigen_symm_alloc(N);

  gsl_eigen_symm(m, v, w);
  gsl_sort_vector(v);
  gsl_vector_reverse(v);

  for (i = 0; i < N; ++i){
    printf("%6.3lf\n", gsl_vector_get(v, i));
  }

  gsl_eigen_symm_free(w);
  gsl_matrix_free(m);
  gsl_vector_free(v);

  return 0;
}
