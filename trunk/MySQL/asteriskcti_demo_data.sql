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

/*Data for the table `actions` */

LOCK TABLES `actions` WRITE;

insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`,`ENABLED`) values (1,'All','InternalBrowser',NULL,'http://astcti2.googlecode.com',NULL,1);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`,`ENABLED`) values (2,'Linux','Application','/usr/bin/gedit',NULL,NULL,1);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`,`ENABLED`) values (3,'Windows','Application','notepad.exe',NULL,NULL,1);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`,`ENABLED`) values (4,'Linux','TcpMessage','192.168.0.2:2323','Caller: {CALLERID}','UTF-8',1);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`,`ENABLED`) values (5,'Linux','UdpMessage','192.168.0.2:2323','Caller: {CALLERID}','UTF-8',1);

UNLOCK TABLES;

/*Data for the table `seats` */

LOCK TABLES `seats` WRITE;

insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`DESCRIPTION`,`ENABLED`) values (1,'00:1c:bf:37:53:ba','Seat 0/0',1);
insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`DESCRIPTION`,`ENABLED`) values (2,NULL,'Seat 0/1',1);

UNLOCK TABLES;

/*Data for the table `extensions` */

LOCK TABLES `extensions` WRITE;

insert  into `extensions`(`ID_SEAT`,`EXTEN`,`CAN_AUTO_ANSWER`,`ENABLED`) values (1,'SIP/32',0,1);
insert  into `extensions`(`ID_SEAT`,`EXTEN`,`CAN_AUTO_ANSWER`,`ENABLED`) values (2,'SIP/33',1,1);

UNLOCK TABLES;

/*Data for the table `operators` */

LOCK TABLES `operators` WRITE;

insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`BEGIN_IN_PAUSE`,`ID_SEAT`,`ENABLED`) values (1,'Operator 1','oper1','1a1dc91c907325c69271ddf0c944bc72',0,NULL,1);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`BEGIN_IN_PAUSE`,`ID_SEAT`,`ENABLED`) values (2,'Operator 2','oper2','1a1dc91c907325c69271ddf0c944bc72',0,NULL,1);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`BEGIN_IN_PAUSE`,`ID_SEAT`,`ENABLED`) values (3,'Operator 3','oper3','1a1dc91c907325c69271ddf0c944bc72',0,1,0);

UNLOCK TABLES;

/*Data for the table `server_settings` */

LOCK TABLES `server_settings` WRITE;

insert  into `server_settings`(`name`,`val`) values ('db_version','0.8');
insert  into `server_settings`(`name`,`val`) values ('last_update',UNIX_TIMESTAMP());
insert  into `server_settings`(`name`,`val`) values ('ami_host','localhost');
insert  into `server_settings`(`name`,`val`) values ('ami_port','5038');
insert  into `server_settings`(`name`,`val`) values ('ami_user','ctiserver');
insert  into `server_settings`(`name`,`val`) values ('ami_secret','ctiserver');
insert  into `server_settings`(`name`,`val`) values ('ami_connect_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('ami_read_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('ami_connect_retry_after','30');
insert  into `server_settings`(`name`,`val`) values ('cti_server_address','0.0.0.0');
insert  into `server_settings`(`name`,`val`) values ('cti_server_port','5000');
insert  into `server_settings`(`name`,`val`) values ('cti_connect_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('cti_read_timeout','30000');
insert  into `server_settings`(`name`,`val`) values ('cti_socket_compression_level','7');
insert  into `server_settings`(`name`,`val`) values ('asterisk_version','1.8');
insert  into `server_settings`(`name`,`val`) values ('auto_answer_context','cti-auto-answer');

UNLOCK TABLES;

/*Data for the table `services` */

LOCK TABLES `services` WRITE;

insert  into `services`(`ID_SERVICE`,`SERVICE_NAME`,`SERVICE_CONTEXT_TYPE`,`SERVICE_IS_QUEUE`,`SERVICE_QUEUE_NAME`,`ENABLED`) values (1,'test-ivr','INBOUND',1,'600',1);

UNLOCK TABLES;

/*Data for the table `services_actions` */

LOCK TABLES `services_actions` WRITE;

insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`,`ENABLED`) values (1,1,2,1);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`,`ENABLED`) values (1,2,1,1);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`,`ENABLED`) values (1,3,3,1);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`,`ENABLED`) values (1,4,4,1);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`,`ENABLED`) values (1,5,5,1);

UNLOCK TABLES;

/*Data for the table `services_operators` */

LOCK TABLES `services_operators` WRITE;

insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`,`ENABLED`) values (1,1,0,1);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`,`ENABLED`) values (1,2,0,1);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`,`ENABLED`) values (1,3,0,1);

UNLOCK TABLES;

/*Data for the table `services_variables` */

LOCK TABLES `services_variables` WRITE;

insert  into `services_variables`(`ID_SERVICE`,`VARNAME`,`ENABLED`) values (1,'APPDATA',1);
insert  into `services_variables`(`ID_SERVICE`,`VARNAME`,`ENABLED`) values (1,'CALLERID',1);

UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;