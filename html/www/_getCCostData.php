<?php 

// *************************************************
// ** Code made by (c) Vincent(at)Cruvellier(dot)eu
// * 2014, November.
// *************************************************
//
// This is just an example of reading server side data and sending it to the client.
// It reads a json formatted text file and outputs it.
// Call this file with the following parameters in GET Mode:
//        t=tablename&f=fieldname&u=tick
// Typical use in external Javascript call:
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
//*  $tableName_ : string Name of the sensor table name to query in database.
//*  $fieldName_ : string Name of the field  name to query in from the table in database.
//*  $tick_      : Future use: tick = Hourly, Daily ....
//*****************************************************************************
function _QueryToJson($tableName_, $fieldName_, $tick_, $sensor_) { 



  require '_f_databaseConfig.php';  // Read MySql DataBase properties

  $conn = new PDO("mysql:host=localhost;dbname=$_DBname", $_DBusername, $_DBpassword);

  $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try { 
	$_StrQuery=";";
	if (strcmp($tick_,"HOURLY")==0) { // Hourly Tick based
		$_StrQuery = "SELECT starttime AS datetime, sensor, value FROM CC_SENSOR_HIST_REPORT WHERE (period='h') AND (sensor = $sensor_) AND (starttime >= DATE_SUB(NOW(), INTERVAL 168 HOUR)) ORDER BY 1 ASC";
	} elseif (strcmp($tick_,"DAILY")==0) { // Hourly Tick based
		$_StrQuery = "SELECT starttime AS datetime, sensor, value FROM CC_SENSOR_HIST_REPORT WHERE (period='d') AND (sensor = $sensor_) AND (starttime >= DATE_SUB(NOW(), INTERVAL 2 MONTH)) ORDER BY 1 ASC";
	} elseif (strcmp($tick_,"RAW")==0) { // RAW based
		if (strcmp($sensor_,"0-1")==0) {
		$_StrQuery = "SELECT datetime, period, MAX( $fieldName_ ) - MIN( $fieldName_ ) as ${fieldName_} FROM CC_SENSOR_DYN_REPORT
		WHERE (DATETIME >= DATE_SUB(NOW(), INTERVAL 12 HOUR))
		GROUP BY 1,2 ORDER BY 1 ASC;";
		} else {
		$_StrQuery = "SELECT datetime, period, $fieldName_ as ${fieldName_}
  		FROM CC_SENSOR_DYN_REPORT
		WHERE (datetime >= DATE_SUB(NOW(), INTERVAL 12 HOUR)) AND (sensor = $sensor_)
		ORDER BY 1 ASC;";
		}
	}
	else { // Monthly Tick based
		$_StrQuery = "SELECT starttime AS datetime, sensor, value FROM CC_SENSOR_HIST_REPORT WHERE period='m' and (sensor = $sensor_) ORDER BY 1 ASC";
	}

	$result = $conn->query($_StrQuery); // Execute the Query
  	// An array will be build in order to store data as per needed for a JSON further use
	$rows = array();
	$table = array();
	if (strcmp($tick_,"RAW")==0)
	{
	$table['cols'] = array(
		array('label' => 'datetime', 'type' => 'datetime'), 
		array('label' => "Heures Creuses", 'type' => 'number'),
		array('label' => "Heures Pleines", 'type' => 'number'),
	); // Creation of the column header definition.
	// We will push the data in the array following a loop from the query results
	$Initialised = false;
	foreach($result as $r) {
		$my_date_ = _DateTimeToJavaScript($r['datetime']); // call function to convert to Javascript format expected
		$data = array();
		$data[] = array('v' =>  $my_date_);
		if ($r['period'] == 'HC') {
			$data[] = array('v' => sprintf("%.1f", $r["${fieldName_}"]));
			$data[] = array('v' => 0);
		} else {
			$data[] = array('v' => 0);
			$data[] = array('v' => sprintf("%.1f", $r["${fieldName_}"]));
		}		
		$rows[] = array('c' => $data);		
  	}
	$table['rows'] = $rows;
	}  //end of raw management
	else {
	$table['cols'] = array(
		array('label' => 'datetime', 'type' => 'datetime'), 
		array('label' => "Energy", 'type' => 'number')
	); // Creation of the column header definition.
	// We will push the data in the array following a loop from the query results
	foreach($result as $r) {
		$my_date_ = _DateTimeToJavaScript($r['datetime']); // call function to convert to Javascript format expected
		$data = array();
		$data[] = array('v' =>  $my_date_);
		$data[] = array('v' => sprintf("%.1f", $r["${fieldName_}"]));
		$rows[] = array('c' => $data);		
  	}
	$table['rows'] = $rows;
	
	} // End of other tick management
  } catch(PDOException $e) {
	echo 'ERROR: ' . $e->getMessage();
  }
return(json_encode($table));
} // End of _QueryToJson

// Default data for easiest use
$tableName_ = "CC_SENSOR_DYN";
$fieldName_ = "value";
$tick_ = "RAW";
$sensor_ = 0-1;

if (isset($_GET['t'])) {$tableName_ = $_GET['t'];}
if (isset($_GET['f'])) {$fieldName_ = $_GET['f'];}
if (isset($_GET['u'])) {$tick_ = $_GET['u'];}
if (isset($_GET['c'])) {$sensor_ = $_GET['c'];}

// push Header (JSON)
header('Content-type: application/json');
// return the JSON Data content
$temp  = _QueryToJson($tableName_, $fieldName_, $tick_, $sensor_);
ob_start('ob_gzhandler');
echo $temp;
?>
