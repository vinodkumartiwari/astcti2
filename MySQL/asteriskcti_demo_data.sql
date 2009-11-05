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

insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`) values (1,'ALL','INTERNAL_BROWSER',NULL,'http://astcti2.googlecode.com',NULL);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`) values (2,'LINUX','APPLICATION','/usr/bin/gedit',NULL,NULL);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`) values (3,'WINDOWS','APPLICATION','notepad.exe',NULL,NULL);
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`) values (4,'LINUX','TCP_MESSAGE','192.168.0.2:2323','Caller: {CALLERID}','UTF8');
insert  into `actions`(`ID_ACTION`,`ACTION_OS_TYPE`,`ACTION_TYPE`,`ACTION_DESTINATION`,`PARAMETERS`,`ENCODING`) values (5,'LINUX','UDP_MESSAGE','192.168.0.2:2323','Caller: {CALLERID}','UTF8');

UNLOCK TABLES;

/*Data for the table `dbversion` */

LOCK TABLES `dbversion` WRITE;

insert  into `dbversion`(`VERSION`) values (0.9);

UNLOCK TABLES;

/*Data for the table `operators` */

LOCK TABLES `operators` WRITE;

insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (1,'Operator 1','oper1','5ebe2294ecd0e0f08eab7690d2a6ee69',1,0,0);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (2,'Operator 2','oper2','5ebe2294ecd0e0f08eab7690d2a6ee69',2,0,0);
insert  into `operators`(`ID_OPERATOR`,`FULL_NAME`,`USERNAME`,`PASS_WORD`,`LAST_SEAT`,`BEGIN_IN_PAUSE`,`ENABLED`) values (3,'Operator 3','oper3','5ebe2294ecd0e0f08eab7690d2a6ee69',NULL,0,0);

UNLOCK TABLES;

/*Data for the table `seats` */

LOCK TABLES `seats` WRITE;

insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (1,'00:1c:bf:37:53:ba','SIP/311','Seat 0/0');
insert  into `seats`(`ID_SEAT`,`SEAT_MAC`,`SEAT_EXTEN`,`DESCRIPTION`) values (2,'00:21:9b:d7:7a:bb','SIP/310','Seat 0/1');

UNLOCK TABLES;

/*Data for the table `server_settings` */

LOCK TABLES `server_settings` WRITE;

insert  into `server_settings`(`name`,`val`) values ('ami_host','localhost');
insert  into `server_settings`(`name`,`val`) values ('ami_port','5038');
insert  into `server_settings`(`name`,`val`) values ('ami_user','ctiserver');
insert  into `server_settings`(`name`,`val`) values ('ami_secret','ctiserver');
insert  into `server_settings`(`name`,`val`) values ('ami_connect_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('ami_read_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('ami_connect_retry_after','30');
insert  into `server_settings`(`name`,`val`) values ('cti_server_port','5000');
insert  into `server_settings`(`name`,`val`) values ('cti_connect_timeout','1500');
insert  into `server_settings`(`name`,`val`) values ('cti_read_timeout','30000');
insert  into `server_settings`(`name`,`val`) values ('cti_socket_compression_level','7');


UNLOCK TABLES;

/*Data for the table `services` */

LOCK TABLES `services` WRITE;

insert  into `services`(`ID_SERVICE`,`SERVICE_NAME`,`SERVICE_CONTEXT_TYPE`,`SERVICE_IS_QUEUE`,`SERVICE_QUEUE_NAME`,`ENABLED`) values (1,'test-ivr','INBOUND',1,'600',1);

UNLOCK TABLES;

/*Data for the table `services_actions` */

LOCK TABLES `services_actions` WRITE;

insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`) values (1,1,2);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`) values (1,2,1);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`) values (1,3,3);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`) values (1,4,4);
insert  into `services_actions`(`ID_SERVICE`,`ID_ACTION`,`ACTION_ORDER`) values (1,5,5);

UNLOCK TABLES;

/*Data for the table `services_operators` */

LOCK TABLES `services_operators` WRITE;

insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,1,0);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,2,0);
insert  into `services_operators`(`ID_SERVICE`,`ID_OPERATOR`,`PENALTY`) values (1,3,0);

UNLOCK TABLES;

/*Data for the table `services_variables` */

LOCK TABLES `services_variables` WRITE;

insert  into `services_variables`(`ID_VARIABLE`,`ID_SERVICE`,`VARNAME`) values (2,1,'APPDATA');
insert  into `services_variables`(`ID_VARIABLE`,`ID_SERVICE`,`VARNAME`) values (1,1,'CALLERID');

UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
