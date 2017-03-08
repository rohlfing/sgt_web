var graphCanvas  = document.getElementById('graphCanvas');
var graphContext = graphCanvas.getContext('2d');

/* Form fields */
var form_adjacency = document.getElementById('formAdjacency');
var form_diagonal  = document.getElementById('formDiagonal');
var form_num_vert  = document.getElementById('formNumVert');
form_adjacency.value = "";
form_diagonal .value = "";
form_num_vert .value = "";

/* State variables */
var prev = null;
var clicked = 0;

/* Graph Data */
var vertices = [];
var edges = []
var n = 0;
var num_edges = 0;
var adjacency = []; /* Adjacency Matrix of graph */

var draw_vertex = function(x, y, color){
  var c = graphContext;
  c.beginPath();
  c.lineWidth = "2";
  c.strokeStyle= color;
  c.fillStyle= "black";
  c.arc(x, y, 7, 0, 2*Math.PI);
  c.stroke();
}

var draw_edge = function(v1, v2){
  var c = graphContext;
  c.beginPath();
  c.lineWidth = "1";
  c.strokeStyle = "black";
  c.moveTo(v1.x, v1.y);
  c.lineTo(v2.x, v2.y);
  c.stroke();
}

function redraw_graph(){
  graphContext.clearRect(0, 0, graphCanvas.width, graphCanvas.height);

  vertices.forEach(function(v){
    draw_vertex(v.x, v.y, "black");
  });
  edges.forEach(function(e){
    draw_edge(e.v1, e.v2);
  });
}

function reset_graph(){
  n = 0;
  num_edges = 0;
  edges = [];
  vertices = [];
  adjacency = [];
  redraw_graph();
  write_command();
}

var Vertex = function(x, y, i){
  this.x = x;
  this.y = y;
  this.degree = 0;
  this.index = i;
}

var Edge = function(v1, v2, i){
  this.v1 = v1;
  this.v2 = v2;
  this.index = i;
}

function write_command(){
  var adjacency_string;
  var  diagonal_string;

  /* Start strings */
  adjacency_string = "{";
   diagonal_string = "{";

  /* For each row of the matrices */
  for (var i = 0; i < adjacency.length; ++i){
    adjacency_string += "{";

    /* Add element to degree vector */
    diagonal_string += vertices[i].degree;
    if (i < adjacency.length - 1){
      diagonal_string += ",";
    }

    /* For each element of the row */
    for (var j = 0; j < adjacency[0].length; ++j){
      adjacency_string += adjacency[i][j];
      if (j < adjacency[0].length - 1){
        adjacency_string += ",";
      }
    }

    /* End the row */
    adjacency_string += "}";
    if (i < adjacency.length - 1){
      adjacency_string += ",";
    }
  }

  /* End strings; set form values */
  adjacency_string += "}";
   diagonal_string += "}";
  form_adjacency.value = adjacency_string;
  form_diagonal .value =  diagonal_string;
  form_num_vert .value = n;
}

var make_vertex = function(x, y){
  draw_vertex(x, y, "black");
  vertices.push(new Vertex(x, y, n));
  
  /* All other vertices currently unconnected */
  for (var i = 0; i < n; ++i){
    adjacency[i][n] = 0;
  }
  /* Increment n */
  ++n;
  /* Preserve Symmetry */
  adjacency.push([]);
  for (var i = 0; i < n; ++i){
    adjacency[n - 1][i] = 0;
  }
}

var make_edge = function(v1, v2){
  for (var i = 0; i < num_edges; ++i){
    if ((edges[i].v1 == v1 && edges[i].v2 == v2) ||
        (edges[i].v2 == v1 && edges[i].v1 == v2))
      return;
  }

  edges.push(new Edge(v1, v2, num_edges));
  ++num_edges;
  ++v1.degree;
  ++v2.degree;
  draw_edge(v1, v2);
  adjacency[v1.index][v2.index] = 1;
  adjacency[v2.index][v1.index] = 1;
}

function remove_vertex(v){
  if (v == prev){
    prev = null;
    clicked = 0;
  }

  /* Remove incident edges */
  for(var i = num_edges - 1; i >= 0; --i){
    if (edges[i].v1 == v || edges[i].v2 == v){
      --edges[i].v1.degree;
      --edges[i].v2.degree;
      edges.splice(i, 1);
      --num_edges;
    }
  }

  /* Reassign edge indices */
  for(var i = 0; i < num_edges; ++i){
    edges[i].index = i;
  }

  /* Remove vertex from list, reassign indices */
  for(var i = v.index + 1; i < n; ++i){
    --vertices[i].index;
  }
  vertices.splice(v.index, 1);

  /* Remove column from A */
  for (var i = 0; i < n; ++i){
    adjacency[i].splice(v.index, 1);
  }
  /* Remove row */
  adjacency.splice(v.index, 1);

  /* Decrement number of vertices */
  --n;
}

function remove_edge(e){
  /* Update degree of vertices with e incident */
  --e.v1.degree;
  --e.v2.degree;

  /* Remove e and reassign indices */
  for (var i = e.index + 1; i < num_edges; ++i){
    --edges[i].index;
  }
  edges.splice(e.index, 1);

  /* Update A */
  adjacency[e.v1.index][e.v2.index] = 0;
  adjacency[e.v2.index][e.v1.index] = 0;

  /* Decrement number of edges */
  --num_edges;
}

function select_vertex(v){
  /* Fist vertex clicked */
  if (clicked == 0){
    prev = v;
    clicked = 1;
    draw_vertex(v.x, v.y, "red");
    return;
  }

  /* Clicked same vertex */
  if (prev == v){
    prev = null;
    clicked = 0;
    draw_vertex(v.x, v.y, "black");
    return;
  }

  /* Clicked two vertices; draw edge */
  make_edge(prev, v);
  draw_vertex(prev.x, prev.y, "black");
  prev = null;
  clicked = 0;
  write_command();
}

function close_to(x1, y1, x2, y2, radius){
  return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) < radius*radius;
}

function bipartite_double(){
  var old_n;
  var old_num_edges;
  var rad;
  var dy, dx;

  old_n = n;
  old_num_edges = num_edges;
  rad = Math.random() * 2 * Math.PI;
  dy  = Math.floor(35 * Math.sin(rad));
  dx  = Math.floor(35 * Math.cos(rad));

  /* Copy all vertices */  
  for (var i = 0; i < old_n; ++i){
    make_vertex(vertices[i].x + dx, vertices[i].y + dy);
  }

  /* Make new edges */
  for (var i = 0; i < old_num_edges; ++i){
    make_edge(edges[i].v1, vertices[edges[i].v2.index + old_n]);
    make_edge(edges[i].v2, vertices[edges[i].v1.index + old_n]);
  }

  /* Delete old edges */
  for (var i = old_num_edges - 1; i >= 0; --i){
    remove_edge(edges[i]);
  }

  redraw_graph();
  write_command();
}

function petersen(){
  var rads, mag;
  reset_graph();

  for (var i = 0; i < 10; ++i){
    rads = -2*Math.PI/4 + 2*Math.PI*(i % 5)/5;
    mag  = 75 + (Math.floor(i / 5) * 75);
    make_vertex(Math.floor(mag * Math.cos(rads) + 320), Math.floor(mag * Math.sin(rads) + 240));
  } 
  for (var i = 0; i < 5; ++i){
    make_edge(vertices[i], vertices[(i + 2) % 5]); // Star
    make_edge(vertices[i + 5], vertices[(i + 1) % 5 + 5]); // Pentagon
    make_edge(vertices[i], vertices[i + 5]); // Connection
  }

  write_command();
}

graphCanvas.addEventListener("click", function (e){
  var x = e.pageX - this.offsetLeft;
  var y = e.pageY - this.offsetTop;
  var i = 0;

  for (i = 0; i < vertices.length; ++i){
    if (close_to(x, y, vertices[i].x, vertices[i].y, 14)){
      // Ctrl-click removes vertex
      if(e.ctrlKey){
        remove_vertex(vertices[i]);
        write_command();
        redraw_graph();
      }
      else
        select_vertex(vertices[i]);
      return;
    }
  }

  if (i == vertices.length){
    make_vertex(x, y);
    write_command();
  }
});
