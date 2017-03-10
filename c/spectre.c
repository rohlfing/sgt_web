#include <gsl/gsl_eigen.h>
#include <math.h>

#define DEBUG 0

#define EMATRIXTYPE  -1
#define EINSUFFINPUT -2
#define EBADSIZE     -4
#define TOLERANCE 0.0001

typedef enum {
  ADJACENCY,
  LAPLACIAN,
  NORMALIZED
} matrix_e;

void print_individual(double value, int mult){
  if (fabs(value) < TOLERANCE){
    value = 0;
  }

  printf("%6.3lf ", value);
  if (mult > 1){
    printf("(%d)", mult);
  }
  printf("<br />\n");
}

void print_evals(gsl_vector* values, int n, char asc, char abs){
  double last_eval, curr_eval;
  int multiplicity;
  int i;

  gsl_sort_vector(values);

  if (!asc){
    gsl_vector_reverse(values);
  }

  last_eval = gsl_vector_get(values, 0);
  multiplicity = 1;
  for (i = 1; i < n; ++i){
    curr_eval = gsl_vector_get(values, i);

    /* Increment multiplicity for same eigenvalue */
    if (fabs(curr_eval - last_eval) < TOLERANCE){
      ++multiplicity;
    }
    /* Print when all of one value is found */
    else {
      print_individual(last_eval, multiplicity);
      last_eval = curr_eval;
      multiplicity = 1;
    }
  }

  /* Print last eval */
  print_individual(last_eval, multiplicity);
}

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
  gsl_vector_view eigenvalues_view;

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
  if (n <= 0) return EBADSIZE;

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

    print_evals(eigenvalues, n, 0, 1);
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

    print_evals(eigenvalues, n, 1, 0);
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
        new_element  = degrees[j] ? ((i == j) - new_element) : 0;
        gsl_matrix_set(L, i, j, new_element);
      }
    }

    /* Allocate result vector */
    complex_eigenvalues = gsl_vector_complex_alloc(n);

    w_ns = gsl_eigen_nonsymm_alloc(n);
    gsl_eigen_nonsymm(L, complex_eigenvalues, w_ns);
    gsl_eigen_nonsymm_free(w_ns);
    gsl_matrix_free(L);
    gsl_matrix_free(I);
    gsl_matrix_free(D_inv);

    eigenvalues_view = gsl_vector_complex_real(complex_eigenvalues);

    print_evals(&eigenvalues_view.vector, n, 1, 0);
  }

  gsl_matrix_free(A);
  free(degrees);

  gsl_vector_free(eigenvalues);

  return 0;
}
