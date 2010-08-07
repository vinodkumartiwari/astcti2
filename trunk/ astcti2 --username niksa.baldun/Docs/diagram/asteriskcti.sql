/*
SQLyog Community Edition- MySQL GUI v7.15 
MySQL - 5.0.67-0ubuntu6 : Database - asteriskcti
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`asteriskcti` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `asteriskcti`;

/*Table structure for table `Applications` */

DROP TABLE IF EXISTS `Applications`;

CREATE TABLE `Applications` (
  `ID_APPLICATION` bigint(20) NOT NULL auto_increment,
  `ID_SERVICE` bigint(20) NOT NULL,
  `APPLICATION_OS_TYPE` enum('LNX','OSX','WIN') NOT NULL,
  `APPLICATION_PATH` varchar(255) NOT NULL,
  `APPLICATION_PARAMETERS` text,
  PRIMARY KEY  (`ID_APPLICATION`),
  UNIQUE KEY `UniqueApplicationByOs` (`APPLICATION_OS_TYPE`,`APPLICATION_PATH`),
  KEY `FK_Applications` (`ID_SERVICE`),
  CONSTRAINT `FK_Applications` FOREIGN KEY (`ID_SERVICE`) REFERENCES `Services` (`ID_SERVICE`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Applications` */

LOCK TABLES `Applications` WRITE;

insert  into `Applications`(`ID_APPLICATION`,`ID_SERVICE`,`APPLICATION_OS_TYPE`,`APPLICATION_PATH`,`APPLICATION_PARAMETERS`) values (1,1,'LNX','/usr/bin/firefox','http://centralino-voip.brunosalzano.com/test.php');
insert  into `Applications`(`ID_APPLICATION`,`ID_SERVICE`,`APPLICATION_OS_TYPE`,`APPLICATION_PATH`,`APPLICATION_PARAMETERS`) values (2,1,'WIN','C:\\firefox\\firefox.exe','http://centralino-voip.brunosalzano.com/test.php');
insert  into `Applications`(`ID_APPLICATION`,`ID_SERVICE`,`APPLICATION_OS_TYPE`,`APPLICATION_PATH`,`APPLICATION_PARAMETERS`) values (3,1,'OSX','/usr/bin/firefox','http://centralino-voip.brunosalzano.com/test.php');

UNLOCK TABLES;

/*Table structure for table `Operators` */

DROP TABLE IF EXISTS `Operators`;

CREATE TABLE `Operators` (
  `ID_OPERATOR` bigint(20) NOT NULL auto_increment,
  `FULL_NAME` varchar(100) NOT NULL,
  `USER_NAME` varchar(45) NOT NULL,
  `PASS_WORD` varchar(32) NOT NULL,
  `LAST_SEAT` bigint(20) default '0',
  `BEGIN_IN_PAUSE` tinyint(1) default '0',
  `ENABLED` tinyint(1) default '0',
  PRIMARY KEY  (`ID_OPERATOR`),
  UNIQUE KEY `NewIndex1` (`USER_NAME`),
  KEY `FK_Operators` (`LAST_SEAT`),
  CONSTRAINT `FK_Operators` FOREIGN KEY (`LAST_SEAT`) REFERENCES `Seats` (`ID_SEAT`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Operators` */

LOCK TABLES `Operators` WRITE;

insert  into `Operators`(`ID_OPERATOR`,`FULL_NAME`,`USER_NAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (1,'BRUNO SALZANO','bruno','e3928a3bc4be46516aa33a79bbdfdb08',NULL,0,0);

UNLOCK TABLES;

/*Table structure for table `Seats` */

DROP TABLE IF EXISTS `Seats`;

CREATE TABLE `Seats` (
  `ID_SEAT` bigint(20) NOT NULL auto_increment,
  `SEAT_MAC` varchar(17) NOT NULL default '00:00:00:00:00:00',
  `SEAT_EXTEN` varchar(50) NOT NULL,
  `DESCRIPTION` varchar(255) default NULL,
  PRIMARY KEY  (`ID_SEAT`),
  UNIQUE KEY `UniqueExten` (`SEAT_EXTEN`,`SEAT_MAC`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Seats` */

LOCK TABLES `Seats` WRITE;

insert  into `Seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (1,'00:1c:bf:37:53:ba','SIP/200','XPS 1330 wlan0');
insert  into `Seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (2,'00:23:ae:01:b3:7b','SIP/200','XPS 1330 eth0');

UNLOCK TABLES;

/*Table structure for table `Service_Operators` */

DROP TABLE IF EXISTS `Service_Operators`;

CREATE TABLE `Service_Operators` (
  `ID_OPERATOR` bigint(20) NOT NULL,
  `ID_SERVICE` bigint(20) NOT NULL,
  KEY `FK_Oper2Service` (`ID_SERVICE`),
  KEY `FK_Service2Oper` (`ID_OPERATOR`),
  CONSTRAINT `FK_Service2Oper` FOREIGN KEY (`ID_OPERATOR`) REFERENCES `Operators` (`ID_OPERATOR`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `FK_Oper2Service` FOREIGN KEY (`ID_SERVICE`) REFERENCES `Services` (`ID_SERVICE`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Service_Operators` */

LOCK TABLES `Service_Operators` WRITE;

insert  into `Service_Operators`(`ID_OPERATOR`,`ID_SERVICE`) values (1,1);

UNLOCK TABLES;

/*Table structure for table `Service_Variables` */

DROP TABLE IF EXISTS `Service_Variables`;

CREATE TABLE `Service_Variables` (
  `ID_VARIABLE` bigint(20) NOT NULL auto_increment,
  `ID_SERVICE` bigint(20) NOT NULL,
  `VAR_NAME` varchar(100) NOT NULL,
  PRIMARY KEY  (`ID_VARIABLE`),
  UNIQUE KEY `UniqueServiceVariable` (`ID_SERVICE`,`VAR_NAME`),
  CONSTRAINT `FK_Service_Variables` FOREIGN KEY (`ID_SERVICE`) REFERENCES `Services` (`ID_SERVICE`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Service_Variables` */

LOCK TABLES `Service_Variables` WRITE;

insert  into `Service_Variables`(`ID_VARIABLE`,`ID_SERVICE`,`VAR_NAME`) values (3,1,'APPDATA');
insert  into `Service_Variables`(`ID_VARIABLE`,`ID_SERVICE`,`VAR_NAME`) values (1,1,'CALLERID');
insert  into `Service_Variables`(`ID_VARIABLE`,`ID_SERVICE`,`VAR_NAME`) values (2,1,'CALLERNAME');

UNLOCK TABLES;

/*Table structure for table `Services` */

DROP TABLE IF EXISTS `Services`;

CREATE TABLE `Services` (
  `ID_SERVICE` bigint(20) NOT NULL auto_increment,
  `SERVICE_NAME` varchar(100) default NULL,
  `SERVICE_CONTEXT_TYPE` enum('inbound','outbound') default 'inbound',
  `SERVICE_IS_QUEUE` tinyint(1) default '0',
  `SERVICE_QUEUE_NAME` varchar(100) default NULL,
  `SERVICE_TRIGGER_TYPE` enum('none','application','browser') default 'none',
  `ENABLED` tinyint(1) default '1',
  PRIMARY KEY  (`ID_SERVICE`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Data for the table `Services` */

LOCK TABLES `Services` WRITE;

insert  into `Services`(`ID_SERVICE`,`SERVICE_NAME`,`SERVICE_CONTEXT_TYPE`,`SERVICE_IS_QUEUE`,`SERVICE_QUEUE_NAME`,`SERVICE_TRIGGER_TYPE`,`ENABLED`) values (1,'asteriskcti-demo','inbound',1,'asteriskcti-queue','application',1);

UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
