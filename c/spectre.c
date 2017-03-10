#include <gsl/gsl_eigen.h>

#define DEBUG 0

#define EMATRIXTYPE  -1
#define EINSUFFINPUT -2

typedef enum {
  ADJACENCY,
  LAPLACIAN,
  NORMALIZED
} matrix_e;

int main(int argc, char* argv[]){
  int i, j, n, x;
  double new_element;
  char matrix_type[12];
  int* degrees;
  matrix_e matrix;
  gsl_eigen_symm_workspace *w_s;
  gsl_eigen_nonsymm_workspace *w_ns;
  gsl_matrix *A, *I, *D_inv, *L;
  gsl_vector *eigenvalues;
  gsl_vector_complex *complex_eigenvalues;

  /* Set library environment variable */
  putenv("LD_LIBRARY_PATH=/usr/local/lib");

  /* Determine the necessary matrix type */
  scanf(" %s", matrix_type);
  if (!strcmp("adjacency", matrix_type)){
    matrix = ADJACENCY;
  }
  else if (!strcmp("laplacian", matrix_type)){
    matrix = LAPLACIAN;
  }
  else if (!strcmp("normalized", matrix_type)){
    matrix = NORMALIZED;
  }
  else{
    printf("Error: Unsupported matrix type\n");
    return EMATRIXTYPE;
  }

  /* Get dimension of the matrix */
  scanf(" %d", &n);

  /* allocate the result vector and adjacency matrix */
  eigenvalues = gsl_vector_alloc(n);
  A = gsl_matrix_alloc(n, n);
  degrees = malloc(n * sizeof(int));

  /* Scan in degrees */
  for (i = 0; i < n; ++i){
    scanf(" %d", &(degrees[i]));
  }

  if (DEBUG){
    printf("Scanned in degrees\n");
  }

  /* Scan in adjacency matrix */
  for (i = 0; i < n*n; ++i){
    scanf(" %d", &x);
    gsl_matrix_set(A, i / n, i % n, x);
  }

  /* Calculate eigenvalues of A */
  if (matrix == ADJACENCY){
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(A, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);
  }

  /* Calculate eigenvalues of L */
  else if (matrix == LAPLACIAN) {
    L = A; /* Purely for ease of reading */
    gsl_matrix_scale(L, -1);
    for (i = 0; i < n; ++i){
      gsl_matrix_set(L, i, i, degrees[i]);
    }
    
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(L, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);
  }

  /* Calculate eigenvalues of Normalized Laplacian */
  else if (matrix == NORMALIZED) {
    /* Get identity matrix I */
    I = gsl_matrix_alloc(n, n);
    gsl_matrix_set_identity(I);

    /* Get diagonal matrix D_inv */
    D_inv = gsl_matrix_calloc(n, n);
    for (i = 0; i < n; ++i){
      gsl_matrix_set(D_inv, i, i, degrees[i] ? 1.0 / degrees[i] : 0);
    }

    /* Allocate ~L, normalized Laplacian */
    L = gsl_matrix_alloc(n, n);

    /* Compute ~L */
    for (i = 0; i < n; ++i){
      for (j = 0; j < n; ++j){
        new_element  = gsl_matrix_get(A, i, j);
        new_element *= degrees[j] ? 1.0/degrees[j] : 0;
        new_element = (i == j) - new_element;
        gsl_matrix_set(L, i, j, new_element);
      }
    }
  
    //TODO

    /* Allocate result vector */
    complex_eigenvalues = gsl_vector_complex_alloc(n);

    w_ns = gsl_eigen_nonsymm_alloc(n);
    gsl_eigen_nonsymm(L, complex_eigenvalues, w_ns);
    gsl_eigen_nonsymm_free(w_ns);
    gsl_matrix_free(L);
  }

  gsl_matrix_free(A);
  free(degrees);

  /* Print output */
  gsl_sort_vector(eigenvalues);
  gsl_vector_reverse(eigenvalues);

  for (i = 0; i < n; ++i){
    printf("%6.3lf\n", gsl_vector_get(eigenvalues, i));
  }
  gsl_vector_free(eigenvalues);

  return 0;
}
