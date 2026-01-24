<?php
$server="localhost";
$user="trongtin";
$password="123";
$dbname="IOT";

$conn=mysqli_connect($server,$user,$password,$dbname);


// Check connection
if($conn === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}



?>