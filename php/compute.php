<?php
function valid_chars($string){
  $len = strlen($string);

  for($i = 0; $i < $len; ++$i){
    $c = $string[$i];
    if (!($c == ' ' || ((ord($c) >= ord("0")) && (ord($c) <= ord("9"))))){
      return False;
    }
  }
  return True;
}

function valid_matrix($string){
  $len = strlen($string);

  for($i = 0; $i < $len; ++$i){
    $c = $string[$i];
    if (!((ord($c) >= ord("a")) && (ord($c) <= ord("z")))){
      return False;
    }
  }
  return True;
}

if($_SERVER['REQUEST_METHOD'] == 'POST') {
  $edges     = $_POST["edges"];
  $num_vert  = $_POST["numVert"];
  $matrix    = $_POST["matrix"];

  $term_command = "echo '" . $matrix . " " . $num_vert . " " . $edges . "' | spectre";

  /* Log all computed matrices */
  //error_log($term_command);  

  if(valid_chars($num_vert) && valid_chars($edges) && valid_matrix($matrix)){
    system($term_command);
  }
  else{
    echo "Invalid input. Either you're trying to hack the server or there's a bug.<br /> If the latter, please let me know. If the former, please stop<br />";
  }
}
?>
