<?php
function valid_chars($string){
  $len = strlen($string);

  for($i = 0; $i < $len; ++$i){
    $c = $string[$i];
    if (!($c == '-' || $c =='{' || $c =='}' || $c == ',' || ((ord($c) >= ord("0")) && (ord($c) <= ord("9"))))){
      return False;
    }
  }
  return True;
}

if($_SERVER['REQUEST_METHOD'] == 'POST') {
  $adjacency = $_POST["adjacency"];
  $laplacian = $_POST["laplacian"];
  $diagonal  = $_POST["diagonal"];
  $num_vert  = $_POST["numVert"];
  $matrix    = $_POST["matrix"];

  if ($matrix == "adjacency" && valid_chars($adjacency)){
    $wolf_command = "Eigenvalues[" . $adjacency . "] // N";
  }
  elseif ($matrix == "laplacian" && valid_chars($laplacian)){
    $wolf_command = "Eigenvalues[" . $laplacian . "] // N";
  }
  elseif ($matrix == "normalized" && valid_chars($num_vert) && valid_chars($diagonal) && valid_chars($adjacency)){
    $wolf_command = "Eigenvalues[IdentityMatrix["
      . $num_vert
      . "] - Inverse[DiagonalMatrix["
      . $diagonal . "]] . " . $adjacency . "] // N";
  }
  else {
    echo "INVALID INPUT!<br />";
    return;
  }

  /* Debug print of command passed to wolfram */
  /* echo $wolf_command . "<br />"; */

  $term_command = "echo '" . $wolf_command . "' | wolfram";
  error_log($term_command);  
  //$grep_command = "(" . $term_command . ") | grep -o \"{.*}\"";

  exec($term_command, $outputs);

  /* Debug again */
  /*
  echo count($outputs) . "<br />";
  for ($i = 0; $i < count($outputs); ++$i){
    echo "Line ". $i . ": " . $outputs[$i] . "<br />";
    //if (!(False === strpos($outputs[$i], "Out[1]"))){
    //  echo "Out[1] found on line: " . $i . "<br />";
    //}
  }
   */
  

  if (strpos($outputs[5], "Out[1]") === False){
    echo "Error!<br />";
  }
  else{
    $num_lines = count($outputs) - 7;
    $spectrum = substr($outputs[5], 8);
    for ($i = 2; $i < $num_lines; $i = $i + 2){
      $spectrum = $spectrum . " " . substr($outputs[5 + $i], 5);
    }

    echo $spectrum;
  }
}
?>
