#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sort_vector.h>
#include <math.h>
#include <string.h>
#include "graph.h"

#define DEBUG 0

#define EMATRIXTYPE  1
#define EINSUFFINPUT 2
#define EBADSIZE     4
#define ENOSUCHVTX   8
#define ETOOBIG     16
#define TOLERANCE 0.0001

#define MATRICES \
  X(ADJACENCY) \
  X(LAPLACIAN) \
  X(SIGNLESS) \
  X(NORMALIZED) \
  X(DISTANCE) \
  X(FLIPPED)

typedef enum {
#define X(M) M,
  MATRICES
#undef X
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

/* Function to be used as bfs or dfs callback */
void print_visited(int v){
  printf("visited node %d<br />\n", v);
}

int main(int argc, char* argv[]){
  int i, j, n, x, y;
  double new_element;
  char matrix_type[12];
  char graph;
  int* degrees;
  int* distances;
  graph_t g;
  matrix_e matrix;
  gsl_eigen_symm_workspace *w_s;
  gsl_eigen_nonsymm_workspace *w_ns;
  gsl_matrix *A;
  gsl_vector *eigenvalues;
  gsl_vector_complex *complex_eigenvalues;
  gsl_vector_view eigenvalues_view;

  /* Set library environment variable */
  putenv("LD_LIBRARY_PATH=/usr/local/lib");

  /* Determine the necessary matrix type */
  graph = 0;
  scanf(" %s", matrix_type);


  /* Assign matrix_type enum */
#define X(M) if(!strcmp(#M, matrix_type)){ \
               matrix = M; \
             } else
  MATRICES
#undef X
  {
    printf("Error: Unsupported matrix type\n");
    return EMATRIXTYPE;
  }

  /* Determine if a graph representation should be made */
  if (matrix == DISTANCE || matrix == FLIPPED){
    graph = 1;
  }

  /* Get number of vertices and edges */
  scanf(" %d", &n);
  if (n <= 0) return EBADSIZE;
  if (n > 256){
    printf("Too many vertices: current limit is 256.\n");
    return ETOOBIG;
  }

  /* Make graph if necessary */
  if (graph){
    graph_init(&g, n);
  }

  /* allocate the result vector and adjacency matrix */
  eigenvalues = gsl_vector_alloc(n);
  A = gsl_matrix_calloc(n, n);
  degrees = calloc(n, sizeof(int));

  /* Scan in edges as adjacency matrix */
  while(scanf(" %d %d", &x, &y) == 2){
    if (x < 0 || y < 0 || x >= n || y >= n){
      printf("Error: Bad edge");
      return ENOSUCHVTX;
    }
    gsl_matrix_set(A, x, y, 1);
    gsl_matrix_set(A, y, x, 1);
    ++degrees[x];
    ++degrees[y];

    if (graph){
      graph_add_edge(&g, x, y);
    }
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
    gsl_matrix_scale(A, -1);
    for (i = 0; i < n; ++i){
      gsl_matrix_set(A, i, i, degrees[i]);
    }
    
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(A, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);

    print_evals(eigenvalues, n, 1, 0);
  }

  /* Calculate eigenvalues of Q */
  else if (matrix == SIGNLESS) {
    for (i = 0; i < n; ++i){
      gsl_matrix_set(A, i, i, degrees[i]);
    }
    
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(A, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);

    print_evals(eigenvalues, n, 1, 0);
  }

  /* Calculate eigenvalues of Normalized Laplacian */
  else if (matrix == NORMALIZED) {
    /* Compute ~L */
    for (i = 0; i < n; ++i){
      for (j = 0; j < n; ++j){
        new_element  = gsl_matrix_get(A, i, j);
        new_element *= degrees[j] ? 1.0/degrees[j] : 0;
        new_element  = degrees[j] ? ((i == j) - new_element) : 0;
        gsl_matrix_set(A, i, j, new_element);
      }
    }

    /* Allocate result vector */
    complex_eigenvalues = gsl_vector_complex_alloc(n);

    w_ns = gsl_eigen_nonsymm_alloc(n);
    gsl_eigen_nonsymm(A, complex_eigenvalues, w_ns);
    gsl_eigen_nonsymm_free(w_ns);

    eigenvalues_view = gsl_vector_complex_real(complex_eigenvalues);

    print_evals(&eigenvalues_view.vector, n, 1, 0);
  }
  
  /* Calculate eigenvalues of D */
  else if (matrix == DISTANCE){

    if (!graph_is_connected(&g)){
      printf("Graph must be connected\n");
      return 0;
    }

    distances = malloc(n * sizeof(int));

    for (i = 0; i < n; ++i){
      graph_bfs(&g, i, distances, NULL);
      for (j = 0; j < n; ++j){
        gsl_matrix_set(A, i, j, distances[j]);
      }
    }

    free(distances);
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(A, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);
    print_evals(eigenvalues, n, 0, 1);
  }
  
  /* Calculate eigenvalues of D (flipped) */
  else if (matrix == FLIPPED){

    distances = malloc(n * sizeof(int));

    for (i = 0; i < n; ++i){
      graph_bfs(&g, i, distances, NULL);
      for (j = 0; j < n; ++j){
        gsl_matrix_set(A, i, j, distances[j] > 0 ? (1.0/distances[j]) : 0);
      }
    }

    free(distances);
    w_s = gsl_eigen_symm_alloc(n);
    gsl_eigen_symm(A, eigenvalues, w_s);
    gsl_eigen_symm_free(w_s);
    print_evals(eigenvalues, n, 0, 1);
  }

#if DEBUG
  {
    char is_int;

    switch(matrix){
      case ADJACENCY:
      case LAPLACIAN:
      case DISTANCE:
        is_int = 1;
        break;
      case NORMALIZED:
      case FLIPPED:
        is_int = 0;
        break;
    }

    printf("<table style=\"text-align:right;\">");
    for (i = 0; i < n; ++i){
      printf("<tr>");
      for (j = 0; j < n; ++j){
        if (is_int)
          printf("<td>%d</td>", (int) gsl_matrix_get(A, i, j));
        else
          printf("<td>%lf</td>", (double) gsl_matrix_get(A, i, j));
      }
      printf("</tr>\n");
    }
    printf("</table>\n");
  }
#endif

  gsl_matrix_free(A);
  free(degrees);

  gsl_vector_free(eigenvalues);

  if(graph){
    graph_free(&g);
  }

  return 0;
}
