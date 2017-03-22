#include "graph.h"

void graph_init(graph_t* g, int n){
  int i;

  g->n = n;
  g->m = 0;
  g->edges = malloc(g->n * sizeof(int*));
  g->num_edges = malloc(g->n * sizeof(int));
  g->max_edges = malloc(g->n * sizeof(int));
  for (i = 0; i < n; ++i){
    g->num_edges[i] = 0;
    g->max_edges[i] = INIT_MAX_EDGES;
    g->edges[i] = malloc(g->max_edges[i] * sizeof(int));
  }
}

/* Adds edge from v1 to v2; assumes valid input */
void add_edge(graph_t* g, int v1, int v2){
  if (g->num_edges[v1] == g->max_edges[v1]){
    g->max_edges[v1] *= 2;
    g->edges[v1] = realloc(g->edges[v1], g->max_edges[v1] * sizeof(int));
    // TODO handle realloc failing
  }

  g->edges[v1][g->num_edges[v1]] = v2;
  ++(g->num_edges[v1]);
}

void graph_add_edge(graph_t* g, int v1, int v2){
  if (v1 < 0 || v2 < 0 || v1 >= g->n || v2 >= g->n){
    return;
  }

  /* Add both directions */
  add_edge(g, v1, v2);
  add_edge(g, v2, v1);
}

void dfs(graph_t* g, int v, char* visited, void (*action)(int)){
  int i;

  if (visited[v])
    return;

  visited[v] = 1;
  if (action != NULL){
    action(v);
  }

  for (i = 0; i < g->num_edges[v]; ++i){
    dfs(g, g->edges[v][i], visited, action);
  }
}

void graph_dfs(graph_t* g, int v, void (*action)(int)){
  char* visited;

  visited = calloc(g->n, sizeof(char));
  dfs(g, v, visited, action);
  free(visited);
}

int graph_is_connected(graph_t* g){
  int i;
  char* visited;

  if (g->n == 0)
    return 1;

  visited = calloc(g->n, sizeof(char));
  dfs(g, 0, visited, NULL);
 
  /* Check to see if all nodes are visited */ 
  for (i = 0; i < g->n; ++i){
    if (!visited[i]){
      return 0;
    }
  }

  return 1;
}

void graph_free(graph_t* g){
  int i;

  free(g->num_edges);
  free(g->max_edges);

  for (i = 0; i < g->n; ++i){
    free(g->edges[i]);
  }
  free(g->edges);

  g->n = 0;
  g->m = 0;
}
