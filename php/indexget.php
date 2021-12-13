<?php
session_start();
if (isset($_GET['name'])) {
	$_SESSION['name'] = htmlspecialchars($_GET['name']);
}

if (!isset($_SESSION['count'])) {
  $_SESSION['count'] = 0;
} else {
  $_SESSION['count']++;
}

if (isset($_SESSION['name'])) {
echo "Hello " . $_SESSION['name'] . " !<br>";
}
else {
	echo "Hello you !<br>";
}
echo "Your page have been refreshed " , $_SESSION['count'] , " times<br>";
echo "Here a var dump of " , '$_SESSION : ', "<br>";
var_dump($_SESSION);