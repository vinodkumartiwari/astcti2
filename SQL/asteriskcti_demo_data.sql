--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

COPY actions (id, os_type, action_type, destination, parameters, encoding, enabled) FROM stdin;
1	All	InternalBrowser	\N	http://astcti2.googlecode.com	\N	t
2	Linux	Application	/usr/bin/gedit	\N	\N	t
3	Windows	Application	notepad.exe	\N	\N	t
4	Linux	TcpMessage	192.168.0.2:2323	Caller: {CALLERID}	UTF-8	t
5	Linux	UdpMessage	192.168.0.2:2323	Caller: {CALLERID}	UTF-8	t
\.

COPY extensions (id_seat, exten, description, can_auto_answer, enabled) FROM stdin;
1	SIP/32	\N	f	t
3	SIP/33	\N	t	t
3	SIP/34	\N	f	t
\.

COPY operators (id, full_name, username, password, begin_in_pause, id_seat, can_monitor, can_alter_speeddials, can_record, enabled) FROM stdin;
1	Operator 1	oper1	1a1dc91c907325c69271ddf0c944bc72	f	3	f	t	f	t
2	Operator 2	oper2	5ebe2294ecd0e0f08eab7690d2a6ee69	f	\N	f	t	f	t
3	Operator 3	oper3	5ebe2294ecd0e0f08eab7690d2a6ee69	f	\N	f	t	f	f
\.

COPY seats (id, mac_address, description, enabled) FROM stdin;
1	00:1c:bf:37:53:ba	Seat 0/0	t
2	00:21:9b:d7:7a:bb	Seat 0/1	t
3	\N	Test	t
\.

COPY server_settings (name, val) FROM stdin;
ami_host	192.168.1.223
ami_port	5038
ami_user	ctiserver
ami_secret	ctiserver
ami_connect_timeout	1500
ami_connect_retry_after	2
cti_server_port	5000
cti_connect_timeout	1500
cti_read_timeout	30000
cti_socket_compression_level	7
db_version	0.8
last_update	1371630006
\.

COPY services (id, name, service_type, queue_name, enabled) FROM stdin;
1	test-ivr	INBOUND	600	t
\.

COPY services_actions (id_service, id_action, action_order, enabled) FROM stdin;
1	1	2	t
1	2	1	t
1	3	3	t
1	4	4	t
1	5	5	t
\.

COPY services_operators (id_service, id_operator, penalty, enabled) FROM stdin;
1	1	0	t
1	2	0	t
1	3	0	t
\.

COPY services_variables (id_service, varname, enabled) FROM stdin;
1	APPDATA	t
1	CALLERID	t
\.

COPY speed_dials (id_operator, group_name, name, num, blf, order_num) FROM stdin;
\.

--
-- PostgreSQL database dump complete
--

