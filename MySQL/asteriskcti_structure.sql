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

/*Table structure for table `dbversion` */

DROP TABLE IF EXISTS `dbversion`;

CREATE TABLE `dbversion` (
  `VERSION` double default NULL,
  `LAST_UPDATE` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

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

/*Table structure for table `server_settings` */

DROP TABLE IF EXISTS `server_settings`;

CREATE TABLE `server_settings` (
  `name` varchar(128) NOT NULL,
  `val` longtext NOT NULL,
  PRIMARY KEY  (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
