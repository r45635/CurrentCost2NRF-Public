-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb2+deb7u1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Feb 07, 2015 at 04:04 PM
-- Server version: 5.5.38
-- PHP Version: 5.4.4-14+deb7u12

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `STATION_DATA`
--
SELECT 
DATE_FORMAT(A1.timestamp,'%Y-%m-%d %H:%i:00') as datetime, 
A1.ID as sensor,
A2.PERIOD as period, 
MAX(A1.value) as value_max,
AVG(A1.value) as value,
MIN(A1.value) as value_min
FROM CC_SENSOR_DYN AS A1, CC_COST AS A2 
WHERE 
(A1.timestamp > DATE_SUB(NOW(), INTERVAL 24 HOUR)) 
AND ((TIME(A1.timestamp) >= A2.START) 
AND (TIME(A1.timestamp) < A2.END))
GROUP BY  1,2,3
ORDER BY 1,2,3;
-- --------------------------------------------------------
ALTER TABLE  `CC_SENSOR_DYN_REPORT` ADD PRIMARY KEY ( datetime, sensor, period )
--
-- Table structure for table `CC_SENSOR_DYN_REPORT`
--

CREATE TABLE IF NOT EXISTS `CC_SENSOR_DYN_REPORT` (
  `datetime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `sensor` int(11) NOT NULL,
  `period` varchar(2) NOT NULL,
  `value_max` float NOT NULL,
  `value` float NOT NULL,
  `value_min` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DELIMITER $$
--
-- Events
--
CREATE EVENT UPDATE_CC_SENSOR_DYN_REPORT ON SCHEDULE EVERY 5 MINUTE DO 
INSERT INTO CC_SENSOR_DYN_REPORT
SELECT 
DATE_FORMAT(A1.timestamp,'%Y-%m-%d %H:%i:00') as datetime, 
A1.ID as sensor,
A2.PERIOD as period, 
MAX(A1.value) as value_max,
AVG(A1.value) as value,
MIN(A1.value) as value_min
FROM CC_SENSOR_DYN AS A1, CC_COST AS A2 
WHERE 
(A1.timestamp > DATE_SUB((SELECT MAX(datetime) FROM CC_SENSOR_DYN_REPORT), INTERVAL 1 HOUR)) 
AND ((TIME(A1.timestamp) >= A2.START) 
AND (TIME(A1.timestamp) < A2.END))
GROUP BY  1,2,3
ON DUPLICATE KEY UPDATE value = value, value_max=value_max, value_min = value_min$$

CREATE DEFINER=`root`@`localhost` EVENT `UPDATE_CC_SENSOR_HIST_REPORT_H` ON SCHEDULE EVERY 2 HOUR STARTS '2015-02-06 21:34:57' ON COMPLETION NOT PRESERVE ENABLE DO INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-2) HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'h') and (timestamp > DATE_SUB(NOW(), INTERVAL 4 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value$$

CREATE DEFINER=`root`@`localhost` EVENT `UPDATE_CC_SENSOR_HIST_REPORT_M` ON SCHEDULE EVERY 2 HOUR STARTS '2015-02-06 21:35:54' ON COMPLETION NOT PRESERVE ENABLE DO INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period MONTH),'%Y-%m-01 00:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-1) MONTH),'%Y-%m-01 00:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'm') and (timestamp > DATE_SUB(NOW(), INTERVAL 4 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value$$

DELIMITER ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
