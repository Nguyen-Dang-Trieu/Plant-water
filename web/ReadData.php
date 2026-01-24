<?php
header('Content-Type: application/json');

// dang nhap vao database
include("config.php");

// Doc gia tri RGB tu database
$sql = "select * from DULIEU where stt>(select max(stt) from DULIEU)-7";
$result = mysqli_query($conn,$sql);

$data = array();
foreach ($result as $row){
    $data[] = $row;
}

// // add random data
// $d1 = rand(20, 50);
// $d2 = rand(10, 60);
// $d3=rand(1,0);

// $sql = "insert into DATA(temperature,SOIL,mode) values ($d1,$d2,$d3)";
// mysqli_query($conn,$sql);

mysqli_close($conn);
echo json_encode($data);

?>