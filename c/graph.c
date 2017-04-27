#include "graph.h"

/*
 * Given a graph_t object, initialize it with n vertices and 0 edges
 */
void graph_init(graph_t* g, int n){
  int i;

  g->n = n;
  g->m = 0;
  /* Allocate all data structures */
  /* TODO check for malloc failure */
  g->edges = malloc(g->n * sizeof(int*));
  g->num_edges = malloc(g->n * sizeof(int));
  g->max_edges = malloc(g->n * sizeof(int));
  for (i = 0; i < n; ++i){
    g->num_edges[i] = 0;
    g->max_edges[i] = INIT_MAX_EDGES;
    g->edges[i] = malloc(g->max_edges[i] * sizeof(int));
  }
}

/*
 * Adds edge from v1 to v2; assumes valid input
 */
void add_edge(graph_t* g, int v1, int v2){
  if (g->num_edges[v1] == g->max_edges[v1]){
    g->max_edges[v1] *= 2;
    g->edges[v1] = realloc(g->edges[v1], g->max_edges[v1] * sizeof(int));
    /* TODO handle realloc failing */
  }

  g->edges[v1][g->num_edges[v1]] = v2;
  ++(g->num_edges[v1]);
}

/*
 * Checks input validity, calls add_edge in both directions 
 */
void graph_add_edge(graph_t* g, int v1, int v2){
  if (v1 < 0 || v2 < 0 || v1 >= g->n || v2 >= g->n){
    return;
  }

  /* Add both directions */
  add_edge(g, v1, v2);
  add_edge(g, v2, v1);
}

/*
 * Recursive helper method for BFS traversal
 */
void bfs_recursive(graph_t* g, int v, int* distances, int* vtx_q, int head, int tail, void (*action)(int)){
  int i;

  if (action){
    action(v);
  }

  for(i = 0; i < g->num_edges[v]; ++i){
    if (distances[g->edges[v][i]] < 0){
      vtx_q[tail] = g->edges[v][i];
      distances[vtx_q[tail]] = distances[v] + 1;
      ++tail;
    }
  }

  if (tail == head){
    return;
  }

  else {
    bfs_recursive(g, vtx_q[head], distances, vtx_q, head + 1, tail, action);
  }
}

/* 
 * Runs a BFS on g from vertex v, calliing action(i) on each visited
 * When complete, distances[i] is dist(v, i) or -1 if ther is no path.
 */
void graph_bfs(graph_t* g, int v, int* distances, void (*action)(int)){
  int* vtx_q;
  int i;

  if (g->n <= 0)
    return;

  for (i = 0; i < g->n; ++i){
    distances[i] = -1;
  }
  distances[v] = 0;

  vtx_q = malloc(g->n * sizeof(int));
  bfs_recursive(g, v, distances, vtx_q, 0, 0, action);
  free(vtx_q);
}

/*
 * Runs a BFS on g from vertex v, calliing action(i) on each visited
 * When complete, visited[i] is 1 if there's a path from v to i.
 */
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

/*
 * Calls dfs(), disregards visited[]
 */
void graph_dfs(graph_t* g, int v, void (*action)(int)){
  char* visited;

  visited = calloc(g->n, sizeof(char));
  dfs(g, v, visited, action);
  free(visited);
}

/*
 * Calls dfs(), returns true if all vertices connected
 */
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

/* 
 * Frees internal memory used by graph, but not g itself
 */
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
