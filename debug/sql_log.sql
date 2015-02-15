SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (2+hist_period) HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM `CC_SENSOR_HIST` 
WHERE hist_type = 'h' and id=0
GROUP BY 
1,2,3,4
ORDER BY
1,3,4


INSERT INTO `CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (2+hist_period) HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM `CC_SENSOR_HIST` 
WHERE hist_type = 'h'
GROUP BY 
1,2,3,4

INSERT INTO `CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (2+hist_period) HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM `CC_SENSOR_HIST` 
WHERE hist_type = 'h'
GROUP BY 
1,2,3,4
ON DUPLICATE KEY
UPDATE value = value;


// CRON
// http://stackoverflow.com/questions/21196613/run-a-mysql-query-as-a-cron-job
// Enable
SET GLOBAL event_scheduler = ON;

TRUNCATE TABLE  `CC_SENSOR_HIST_REPORT` IF EXISTS `CC_SENSOR_HIST_REPORT`;

DROP EVENT IF EXISTS UPDATE_CC_SENSOR_HIST_REPORT_H;

CREATE EVENT UPDATE_CC_SENSOR_HIST_REPORT_H
ON SCHEDULE EVERY 2 HOUR DO
INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-2) HOUR),'%Y-%m-%d %H:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'h') and (timestamp > DATE_SUB(NOW(), INTERVAL 12 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value;

DROP EVENT IF EXISTS UPDATE_CC_SENSOR_HIST_REPORT_D;

CREATE EVENT UPDATE_CC_SENSOR_HIST_REPORT_D
ON SCHEDULE EVERY 2 HOUR DO
INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period DAY),'%Y-%m-%d 00:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-1) DAY),'%Y-%m-%d 00:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'd') and (timestamp > DATE_SUB(NOW(), INTERVAL 4 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value;

DROP EVENT IF EXISTS UPDATE_CC_SENSOR_HIST_REPORT_M;
CREATE EVENT UPDATE_CC_SENSOR_HIST_REPORT_M
ON SCHEDULE EVERY 2 HOUR DO
INSERT INTO STATION_DATA.`CC_SENSOR_HIST_REPORT`  
SELECT 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL hist_period MONTH),'%Y-%m-01 00:00:00'),'%Y-%m-%d %H:%i:%s')  as starttime, 
STR_TO_DATE(DATE_FORMAT(DATE_SUB(timestamp, INTERVAL (hist_period-1) MONTH),'%Y-%m-01 00:00:00'),'%Y-%m-%d %H:%i:%s') as endtime,
id AS sensor,
hist_type AS period,
max(value) AS value
FROM STATION_DATA.`CC_SENSOR_HIST` 
WHERE (hist_type = 'm') and (timestamp > DATE_SUB(NOW(), INTERVAL 4 HOUR))
GROUP BY 1,2,3,4
ON DUPLICATE KEY UPDATE value = value;
