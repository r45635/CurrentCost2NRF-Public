-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb2+deb7u1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Feb 15, 2015 at 04:47 PM
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
CREATE DATABASE `STATION_DATA` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `STATION_DATA`;

-- --------------------------------------------------------

--
-- Table structure for table `CC_COST`
--

DROP TABLE IF EXISTS `CC_COST`;
CREATE TABLE IF NOT EXISTS `CC_COST` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `PERIOD` text NOT NULL,
  `START` time NOT NULL,
  `END` time NOT NULL,
  `COST` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

-- --------------------------------------------------------
-- THIS TABLE HAS TO BE LOADED FROM CURRENT COST (CC) DATA
-- timestamp = NOW()
-- id = SENSOR_ID REPORTED by CC;
-- temperature = TEMPERATURE REPORTED by CC;
-- value = WATTS VALUE REPORTED by CC;
-- 
-- Table structure for table `CC_SENSOR_DYN`
--

DROP TABLE IF EXISTS `CC_SENSOR_DYN`;
CREATE TABLE IF NOT EXISTS `CC_SENSOR_DYN` (
  `timestamp` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `id` int(11) DEFAULT NULL,
  `temperature` float DEFAULT NULL,
  `value` int(11) DEFAULT NULL,
  PRIMARY KEY (`timestamp`),
  KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------
-- THIS TABLE WILL BE AUTOMATICALLY POPULATED BY SCHEDULE EVENT
--
-- Table structure for table `CC_SENSOR_DYN_REPORT`
--

DROP TABLE IF EXISTS `CC_SENSOR_DYN_REPORT`;
CREATE TABLE IF NOT EXISTS `CC_SENSOR_DYN_REPORT` (
  `datetime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `sensor` int(11) NOT NULL,
  `period` varchar(2) NOT NULL,
  `value_max` float NOT NULL,
  `value` float NOT NULL,
  `value_min` float NOT NULL,
  PRIMARY KEY (`datetime`,`sensor`,`period`),
  KEY `sensor` (`sensor`),
  KEY `period` (`period`),
  KEY `value` (`value`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------
-- THIS TABLE HAS TO BE LOADED FROM CURRENT COST (CC) DATA
-- timestamp = NOW()
-- id = SENSOR_ID REPORTED by CC;
-- hist_type = 'd' or 'h' or 'm' REPORTED by CC;
-- hist_period = number REPORTED by CC;
-- value = watts values REPORTED by CC;
--
-- Table structure for table `CC_SENSOR_HIST`
--

DROP TABLE IF EXISTS `CC_SENSOR_HIST`;
CREATE TABLE IF NOT EXISTS `CC_SENSOR_HIST` (
  `timestamp` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `id` int(11) NOT NULL DEFAULT '0',
  `hist_type` varchar(1) NOT NULL DEFAULT '',
  `hist_period` int(11) NOT NULL DEFAULT '0',
  `value` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`timestamp`,`id`,`hist_type`,`hist_period`,`value`),
  KEY `id` (`id`),
  KEY `hist_type` (`hist_type`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------
-- THIS TABLE WILL BE AUTOMATICALLY POPULATED BY SCHEDULE EVENT
--
-- Table structure for table `CC_SENSOR_HIST_REPORT`
--

DROP TABLE IF EXISTS `CC_SENSOR_HIST_REPORT`;
CREATE TABLE IF NOT EXISTS `CC_SENSOR_HIST_REPORT` (
  `starttime` datetime NOT NULL,
  `endtime` datetime NOT NULL,
  `sensor` int(11) NOT NULL,
  `period` varchar(1) NOT NULL,
  `value` float NOT NULL,
  PRIMARY KEY (`starttime`,`endtime`,`sensor`,`period`),
  KEY `sensor` (`sensor`),
  KEY `period` (`period`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

DELIMITER $$
--
-- Events
--
DROP EVENT `UPDATE_CC_SENSOR_HIST_REPORT_D`$$
CREATE DEFINER=`root`@`localhost` EVENT `UPDATE_CC_SENSOR_HIST_REPORT_D` ON SCHEDULE EVERY 2 HOUR STARTS '2015-02-06 21:35:36' ON COMPLETION NOT PRESERVE ENABLE DO INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period DAY),'%Y-%m-%d 00:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-1) DAY),'%Y-%m-%d 00:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'd') and (timestamp > DATE_SUB(NOW(), INTERVAL 4 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value$$

DROP EVENT `UPDATE_CC_SENSOR_DYN_REPORT`$$
CREATE DEFINER=`root`@`localhost` EVENT `UPDATE_CC_SENSOR_DYN_REPORT` ON SCHEDULE EVERY 5 MINUTE STARTS '2015-02-07 16:59:02' ON COMPLETION NOT PRESERVE ENABLE DO INSERT INTO CC_SENSOR_DYN_REPORT
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

DROP EVENT `UPDATE_CC_SENSOR_HIST_REPORT_H`$$
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

DROP EVENT `UPDATE_CC_SENSOR_HIST_REPORT_M`$$
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
