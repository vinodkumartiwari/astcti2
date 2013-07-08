/*
SQLyog Community Edition- MySQL GUI v8.14 
MySQL - 5.1.37-1ubuntu5 : Database - asteriskcti
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

/*Table structure for table `actions` */

DROP TABLE IF EXISTS `actions`;

CREATE TABLE `actions` (
  `ID_ACTION` bigint(20) NOT NULL AUTO_INCREMENT,
  `ACTION_OS_TYPE` enum('All','Linux','Macintosh','Windows') DEFAULT 'All' NOT NULL,
  `ACTION_TYPE` enum('Application','Browser','InternalBrowser','TcpMessage','UdpMessage') DEFAULT 'Application' NOT NULL,
  `ACTION_DESTINATION` varchar(255) DEFAULT NULL,
  `PARAMETERS` text,
  `ENCODING` enum('Apple Roman','Big5','Big5-HKSCS','CP949','EUC-JP','EUC-KR','GB18030-0','IBM 850','IBM 866','IBM 874','ISO 2022-JP','ISO 8859-1 to 10','ISO 8859-13 to 16','Iscii-Bng','Dev','Gjr','Knd','Mlm','Ori','Pnj','Tlg','Tml','JIS X 0201','JIS X 0208','KOI8-R','KOI8-U','MuleLao-1','ROMAN8','Shift-JIS','TIS-620','TSCII','UTF-8','UTF-16','UTF-16BE','UTF-16LE','UTF-32','UTF-32BE','UTF-32LE','Windows-1250','Windows-1251','Windows-1252','Windows-1253','Windows-1254','Windows-1255','Windows-1256','Windows-1257','Windows-1258','WINSAMI2') DEFAULT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_ACTION`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `seats` */

DROP TABLE IF EXISTS `seats`;

CREATE TABLE `seats` (
  `ID_SEAT` bigint(20) NOT NULL AUTO_INCREMENT,
  `SEAT_MAC` varchar(17) DEFAULT NULL,
  `DESCRIPTION` varchar(255) DEFAULT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_SEAT`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `extensions` */

DROP TABLE IF EXISTS `extensions`;

CREATE TABLE `extensions` (
  `ID_SEAT` bigint(20) NOT NULL,
  `EXTEN` varchar(50) NOT NULL,
  `DESCRIPTION` varchar(255) DEFAULT NULL,
  `CAN_AUTO_ANSWER` tinyint(1) DEFAULT '0' NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_SEAT`,`EXTEN`),
  CONSTRAINT `FK_seats` FOREIGN KEY (`ID_SEAT`) REFERENCES `asteriskcti`.`seats`(`ID_SEAT`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `operators` */

DROP TABLE IF EXISTS `operators`;

CREATE TABLE `operators` (
  `ID_OPERATOR` bigint(20) NOT NULL AUTO_INCREMENT,
  `FULL_NAME` varchar(100) NOT NULL,
  `USERNAME` varchar(100) NOT NULL,
  `PASS_WORD` varchar(32) NOT NULL,
  `BEGIN_IN_PAUSE` tinyint(1) DEFAULT '0' NOT NULL,
  `ID_SEAT` BIGINT(20) DEFAULT NULL,
  `CAN_MONITOR` TINYINT(1) DEFAULT '0' NOT NULL,
  `CAN_ALTER_SPEEDDIALS` TINYINT(1) DEFAULT '1' NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_OPERATOR`),
  UNIQUE KEY `USERNAME` (`USERNAME`)
  CONSTRAINT `FK_seats` FOREIGN KEY (`ID_SEAT`) REFERENCES `asteriskcti`.`seats`(`ID_SEAT`);
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `speeddials` */

DROP TABLE IF EXISTS `speeddials`;

CREATE TABLE `asteriskcti`.`speeddials`(  
  `ID_OPERATOR` BIGINT(20) NOT NULL,
  `GROUP_NAME` VARCHAR(100) NOT NULL,
  `NAME` VARCHAR(100),
  `NUM` VARCHAR(50) NOT NULL,
  `BLF` TINYINT(1) NOT NULL DEFAULT 0,
  `ORDER_NUM` TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (`ID_OPERATOR`, `GROUP_NAME`, `ORDER_NUM`),
  CONSTRAINT `FK_operators_speeddials` FOREIGN KEY (`ID_OPERATOR`) REFERENCES `asteriskcti`.`operators`(`ID_OPERATOR`) ON DELETE CASCADE
) ENGINE=INNODB CHARSET=utf8 COLLATE=utf8_general_ci;

/*Table structure for table `server_settings` */

DROP TABLE IF EXISTS `server_settings`;

CREATE TABLE `server_settings` (
  `name` varchar(128) NOT NULL,
  `val` longtext NOT NULL,
  PRIMARY KEY (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `services` */

DROP TABLE IF EXISTS `services`;

CREATE TABLE `services` (
  `ID_SERVICE` bigint(20) NOT NULL AUTO_INCREMENT,
  `SERVICE_NAME` varchar(100) DEFAULT '' NOT NULL,
  `SERVICE_CONTEXT_TYPE` enum('INBOUND','OUTBOUND') DEFAULT 'INBOUND' NOT NULL,
  `SERVICE_IS_QUEUE` tinyint(1) DEFAULT '0' NOT NULL,
  `SERVICE_QUEUE_NAME` varchar(100) DEFAULT '' NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_SERVICE`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `services_actions` */

DROP TABLE IF EXISTS `services_actions`;

CREATE TABLE `services_actions` (
  `ID_SERVICE` bigint(20) NOT NULL,
  `ID_ACTION` bigint(20) NOT NULL,
  `ACTION_ORDER` tinyint(4) DEFAULT 0 NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  UNIQUE KEY `UniqueServiceAction` (`ID_SERVICE`,`ID_ACTION`),
  KEY `FK_actions` (`ID_ACTION`),
  CONSTRAINT `FK_actions` FOREIGN KEY (`ID_ACTION`) REFERENCES `actions` (`ID_ACTION`) ON DELETE CASCADE,
  CONSTRAINT `FK_services` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `services_operators` */

DROP TABLE IF EXISTS `services_operators`;

CREATE TABLE `services_operators` (
  `ID_SERVICE` bigint(20) NOT NULL,
  `ID_OPERATOR` bigint(20) NOT NULL,
  `PENALTY` int(11) DEFAULT '0' NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  UNIQUE KEY `ID_SERVICE` (`ID_SERVICE`,`ID_OPERATOR`),
  KEY `FK_operators_services` (`ID_OPERATOR`),
  CONSTRAINT `FK_operators_services` FOREIGN KEY (`ID_OPERATOR`) REFERENCES `operators` (`ID_OPERATOR`) ON DELETE CASCADE,
  CONSTRAINT `FK_services_operators` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `services_variables` */

DROP TABLE IF EXISTS `services_variables`;

CREATE TABLE `services_variables` (
  `ID_SERVICE` bigint(20) NOT NULL,
  `VARNAME` varchar(100) NOT NULL,
  `ENABLED` tinyint(1) DEFAULT '1' NOT NULL,
  PRIMARY KEY (`ID_SERVICE`,`VARNAME`),
  CONSTRAINT `FK_services_variables` FOREIGN KEY (`ID_SERVICE`) REFERENCES `services` (`ID_SERVICE`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
