/*
SQLyog Community Edition- MySQL GUI v8.14 
MySQL - 5.0.75-0ubuntu10.2 : Database - asteriskcti
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`asteriskcti` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `asteriskcti`;

/*Table structure for table `applications` */

DROP TABLE IF EXISTS `applications`;

CREATE TABLE `applications` (
  `ID_APPLICATION` bigint(20) NOT NULL auto_increment,
  `ID_SERVICE` bigint(20) NOT NULL,
  `APPLICATION_OS_TYPE` enum('LIN','MAC','WIN') NOT NULL,
  `APPLICATION_PATH` varchar(255) NOT NULL,
  `PARAMETERS` text,
  PRIMARY KEY  (`ID_APPLICATION`),
  UNIQUE KEY `idx_appos2service` (`ID_SERVICE`,`APPLICATION_OS_TYPE`),
  CONSTRAINT `FK_services_applications` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `applications` */

LOCK TABLES `applications` WRITE;

insert  into `applications`(`ID_APPLICATION`,`ID_SERVICE`,`APPLICATION_OS_TYPE`,`APPLICATION_PATH`,`PARAMETERS`) values (1,1,'WIN','C:\\Program Files\\Firefox\\Firefox.exe','http://centralino-voip.brunosalzano.com?callerid={CALLERID}&appdata={APPDATA}');
insert  into `applications`(`ID_APPLICATION`,`ID_SERVICE`,`APPLICATION_OS_TYPE`,`APPLICATION_PATH`,`PARAMETERS`) values (2,1,'LIN','/usr/bin/firefox','http://centralino-voip.brunosalzano.com?callerid={CALLERID}&appdata={APPDATA}');

UNLOCK TABLES;

/*Table structure for table `dbversion` */

DROP TABLE IF EXISTS `dbversion`;

CREATE TABLE `dbversion` (
  `VERSION` double default NULL,
  `LAST_UPDATE` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `dbversion` */

LOCK TABLES `dbversion` WRITE;

insert  into `dbversion`(`VERSION`,`LAST_UPDATE`) values (0.5,1255558719);

UNLOCK TABLES;

/*Table structure for table `operators` */

DROP TABLE IF EXISTS `operators`;

CREATE TABLE `operators` (
  `ID_OPERATOR` bigint(20) NOT NULL auto_increment,
  `FULL_NAME` varchar(100) NOT NULL,
  `USERNAME` varchar(100) NOT NULL,
  `PASS_WORD` varchar(32) NOT NULL,
  `LAST_SEAT` bigint(20) default NULL,
  `BEGIN_IN_PAUSE` tinyint(1) default '0',
  `ENABLED` tinyint(1) default '1',
  PRIMARY KEY  (`ID_OPERATOR`),
  UNIQUE KEY `USERNAME` (`USERNAME`),
  KEY `FK_seat_operator` (`LAST_SEAT`),
  CONSTRAINT `FK_seat_operator` FOREIGN KEY (`LAST_SEAT`) REFERENCES `seats` (`ID_SEAT`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `operators` */

LOCK TABLES `operators` WRITE;

insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (1,'Operator 1','oper1','5f4dcc3b5aa765d61d8327deb882cf99',1,0,0);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (2,'Operator 2','oper2','5f4dcc3b5aa765d61d8327deb882cf99',2,0,0);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (3,'Operator 3','oper3','5f4dcc3b5aa765d61d8327deb882cf99',NULL,0,0);

UNLOCK TABLES;

/*Table structure for table `seats` */

DROP TABLE IF EXISTS `seats`;

CREATE TABLE `seats` (
  `ID_SEAT` bigint(20) NOT NULL auto_increment,
  `SEAT_MAC` varchar(17) NOT NULL default '00:00:00:00:00:00',
  `SEAT_EXTEN` varchar(50) NOT NULL,
  `DESCRIPTION` varchar(255) default NULL,
  PRIMARY KEY  (`ID_SEAT`),
  UNIQUE KEY `SEAT_MAC` (`SEAT_MAC`),
  UNIQUE KEY `SEAT_EXTEN` (`SEAT_EXTEN`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `seats` */

LOCK TABLES `seats` WRITE;

insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (1,'00:1c:bf:37:53:ba','SIP/311','Seat 0/0');
insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (2,'00:21:9b:d7:7a:bb','SIP/310','Seat 0/1');

UNLOCK TABLES;

/*Table structure for table `services` */

DROP TABLE IF EXISTS `services`;

CREATE TABLE `services` (
  `ID_SERVICE` bigint(20) NOT NULL auto_increment,
  `SERVICE_NAME` varchar(100) default NULL,
  `SERVICE_CONTEXT_TYPE` enum('INBOUND','OUTBOUND') default 'INBOUND',
  `SERVICE_IS_QUEUE` tinyint(1) default '0',
  `SERVICE_QUEUE_NAME` varchar(100) default NULL,
  `SERVICE_TRIGGER_TYPE` enum('NONE','APPLICATION','BROWSER') default 'NONE',
  `ENABLED` tinyint(1) default '0',
  PRIMARY KEY  (`ID_SERVICE`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `services` */

LOCK TABLES `services` WRITE;

insert  into `services`(`ID_SERVICE`,`SERVICE_NAME`,`SERVICE_CONTEXT_TYPE`,`SERVICE_IS_QUEUE`,`SERVICE_QUEUE_NAME`,`SERVICE_TRIGGER_TYPE`,`ENABLED`) values (1,'ast-cti-demo','INBOUND',1,'600','APPLICATION',1);

UNLOCK TABLES;

/*Table structure for table `services_operators` */

DROP TABLE IF EXISTS `services_operators`;

CREATE TABLE `services_operators` (
  `ID_SERVICE` bigint(20) NOT NULL,
  `ID_OPERATOR` bigint(20) NOT NULL,
  `PENALTY` int(11) default '0',
  UNIQUE KEY `ID_SERVICE` (`ID_SERVICE`,`ID_OPERATOR`),
  CONSTRAINT `FK_operators_services` FOREIGN KEY (`ID_SERVICE`) REFERENCES `operators` (`ID_OPERATOR`) ON DELETE CASCADE,
  CONSTRAINT `FK_services_operators` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `services_operators` */

LOCK TABLES `services_operators` WRITE;

insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,1,0);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,2,0);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,3,0);

UNLOCK TABLES;

/*Table structure for table `services_variables` */

DROP TABLE IF EXISTS `services_variables`;

CREATE TABLE `services_variables` (
  `ID_VARIABLE` bigint(20) NOT NULL auto_increment,
  `ID_SERVICE` bigint(20) NOT NULL,
  `VARNAME` varchar(100) NOT NULL,
  PRIMARY KEY  (`ID_VARIABLE`),
  UNIQUE KEY `SERVICE_VARNAME` (`ID_SERVICE`,`VARNAME`),
  CONSTRAINT `FK_services_variables` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `services_variables` */

LOCK TABLES `services_variables` WRITE;

insert  into `services_variables`(`ID_VARIABLE`,`ID_SERVICE`,`VARNAME`) values (2,1,'APPDATA');
insert  into `services_variables`(`ID_VARIABLE`,`ID_SERVICE`,`VARNAME`) values (1,1,'CALLERID');

UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
