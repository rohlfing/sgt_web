#include <stdlib.h>
#define INIT_MAX_EDGES 4

typedef struct{
  int v1;
  int v2;
} edge_t;

typedef struct{
  int n, m;
  int** edges;
  int* num_edges;
  int* max_edges;
} graph_t;

void graph_init(graph_t* g, int n);

void graph_add_edge(graph_t* g, int v1, int v2);

void graph_dfs(graph_t* g, int v, void (*action)(int));

int graph_is_connected(graph_t* g);

void graph_free(graph_t* g);
