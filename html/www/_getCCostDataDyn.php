<?php 

// *************************************************
// ** Code made by (c) Vincent(at)Cruvellier(dot)eu
// * 2015, February.
// *************************************************
//
//


// Configuration file for the Database Connection
require '_f_databaseConfig.php';
// database connection parameters: shall contains:
// $_DBname = 'XXXXXXXXX'; // Enter DB Here
// $_DBusername = 'XXXXXXX'; // Enter Username Here
// $_DBpassword = 'XXXXXXX'; // Enter Password Here

date_default_timezone_set('Europe/Paris');

//*****************************************************************************
//* function _DateTimeToJavaScript => return Date Time script useable in JSON data format
//*  $adatetime : string from a query database following this format: Y-m-d H:i:s.
//*		refers to mysql manual for DATE_FORMAT function if needed.
//*****************************************************************************
function _DateTimeToJavaScript($adatetime) {
   // WARNING: assumes dates are patterned 'yyyy-MM-dd hh:mm:ss'  
   preg_match('/(\d{4})-(\d{2})-(\d{2})\s(\d{2}):(\d{2}):(\d{2})/', $adatetime, $match);
   $year = (int) $match[1];
   $month = (int) $match[2] - 1; // convert to zero-index to match javascript's dates
   $day = (int) $match[3];
   $hours = (int) $match[4];
   $minutes = (int) $match[5];
   $seconds = (int) $match[6];
   return "Date($year, $month, $day, $hours, $minutes, $seconds)";
} // End of _DateTimeToJavaScript

//*****************************************************************************
//* function _QueryToJson => return a Json Array useable with Google Charts
//*****************************************************************************
function _QueryToJson($field_) { 
  require '_f_databaseConfig.php';  // Read MySql DataBase properties

  $conn = new PDO("mysql:host=localhost;dbname=$_DBname", $_DBusername, $_DBpassword);
  $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try { 
switch ($field_) {

	case "Watts0":
		$_StrQuery="SELECT 'SENSOR 0' as label, value FROM CC_SENSOR_DYN WHERE id=0 ORDER BY timestamp DESC LIMIT 1;";
		$label = "HOME";
		break;
	case "Watts1":
		$_StrQuery="SELECT 'SENSOR 1' as label, value FROM CC_SENSOR_DYN WHERE id=1 ORDER BY timestamp DESC LIMIT 1;";
		$label = "HVAC";
		break;

	default:
		break;

}  	// An array will be build in order to store data as per needed for a JSON further use
	$result = $conn->query($_StrQuery); // Execute the Query
	$rows = array();
	$table = array();
	$table['cols'] = array(
		array('label' => $label, 'type' => 'number')
	); // Creation of the column header definition.
	// We will push the data in the array following a loop from the query results
	foreach($result as $r) {
		//echo $r["value"]."\n";
		//$my_date_ = _DateTimeToJavaScript($r['datetime']); // call function to convert to Javascript format expected
		$data = array();
		$data[] = array('v' =>  (int) $r["value"]);
		//$data[] = array('v' => (int) $r["value"]);
		$rows[] = array('c' => $data);
  	}
	$table['rows'] = $rows;
  } catch(PDOException $e) {
	echo 'ERROR: ' . $e->getMessage();
  }
return(json_encode($table));
} // End of _QueryToJson

// Default data for easiest use
$field_ = "Watts0";

if (isset($_GET['f'])) {$field_ = $_GET['f'];}


if ($field_=='LastWattsUpdate') {
	require '_f_databaseConfig.php';  // Read MySql DataBase properties
	$conn = new PDO("mysql:host=localhost;dbname=$_DBname", $_DBusername, $_DBpassword);
	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$_StrQuery="SELECT timestamp as value FROM CC_SENSOR_DYN ORDER BY timestamp DESC LIMIT 1;";	
	$result = $conn->query($_StrQuery); // Execute the Query
	foreach($result as $r) {
		echo($r["value"]);
	}
	exit();
}


// push Header (JSON)
header('Content-type: application/json');
// return the JSON Data content
$temp  = _QueryToJson($field_);
ob_start('ob_gzhandler');
echo $temp;
?>
