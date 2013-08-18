--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

CREATE DATABASE asteriskcti WITH TEMPLATE = template0 ENCODING = 'UTF8' LC_COLLATE = 'C' LC_CTYPE = 'C';

\connect asteriskcti

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;
COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';

SET search_path = public, pg_catalog;

CREATE TYPE context_type AS ENUM (
    'INBOUND',
    'OUTBOUND'
);

CREATE TYPE cti_action_type AS ENUM (
    'Application',
    'Browser',
    'InternalBrowser',
    'TcpMessage',
    'UdpMessage'
);

CREATE TYPE cti_os_type AS ENUM (
    'All',
    'Linux',
    'Macintosh',
    'Windows'
);

CREATE TYPE msg_encoding AS ENUM (
    'Apple Roman',
    'Big5',
    'Big5-HKSCS',
    'CP949',
    'EUC-JP',
    'EUC-KR',
    'GB18030-0',
    'IBM 850',
    'IBM 866',
    'IBM 874',
    'ISO 2022-JP',
    'ISO 8859-1 to 10',
    'ISO 8859-13 to 16',
    'Iscii-Bng',
    'Dev',
    'Gjr',
    'Knd',
    'Mlm',
    'Ori',
    'Pnj',
    'Tlg',
    'Tml',
    'JIS X 0201',
    'JIS X 0208',
    'KOI8-R',
    'KOI8-U',
    'MuleLao-1',
    'ROMAN8',
    'Shift-JIS',
    'TIS-620',
    'TSCII',
    'UTF-8',
    'UTF-16',
    'UTF-16BE',
    'UTF-16LE',
    'UTF-32',
    'UTF-32BE',
    'UTF-32LE',
    'Windows-1250',
    'Windows-1251',
    'Windows-1252',
    'Windows-1253',
    'Windows-1254',
    'Windows-1255',
    'Windows-1256',
    'Windows-1257',
    'Windows-1258',
    'WINSAMI2'
);

SET default_tablespace = '';

SET default_with_oids = false;

CREATE TABLE actions (
    id bigint NOT NULL,
    os_type cti_os_type DEFAULT 'All'::cti_os_type NOT NULL,
    action_type cti_action_type DEFAULT 'Application'::cti_action_type NOT NULL,
    destination character varying(255) DEFAULT NULL::character varying,
    parameters text,
    encoding msg_encoding,
    enabled boolean NOT NULL
);

CREATE TABLE extensions (
    id_seat bigint NOT NULL,
    exten character varying(50) NOT NULL,
    description character varying(255) DEFAULT NULL::character varying,
    can_auto_answer boolean NOT NULL,
    enabled boolean NOT NULL
);

CREATE TABLE operators (
    id bigint NOT NULL,
    full_name character varying(100) NOT NULL,
    username character varying(100) NOT NULL,
    password character varying(32) NOT NULL,
    begin_in_pause boolean NOT NULL,
    id_seat bigint,
    can_monitor boolean NOT NULL,
    can_alter_speeddials boolean NOT NULL,
    can_record boolean NOT NULL,
    enabled boolean NOT NULL
);

CREATE TABLE seats (
    id bigint NOT NULL,
    mac_address macaddr,
    description character varying(255) DEFAULT NULL::character varying,
    enabled boolean NOT NULL
);

CREATE TABLE server_settings (
    name character varying(128) NOT NULL,
    val text NOT NULL
);

CREATE TABLE services (
    id bigint NOT NULL,
    name character varying(100) DEFAULT ''::character varying NOT NULL,
    service_type context_type DEFAULT 'INBOUND'::context_type NOT NULL,
    queue_name character varying(100) DEFAULT NULL::character varying,
    enabled boolean NOT NULL
);

CREATE TABLE services_actions (
    id_service bigint NOT NULL,
    id_action bigint NOT NULL,
    action_order smallint DEFAULT 0::smallint NOT NULL,
    enabled boolean NOT NULL
);

CREATE TABLE services_operators (
    id_service bigint NOT NULL,
    id_operator bigint NOT NULL,
    penalty integer DEFAULT 0 NOT NULL,
    enabled boolean NOT NULL
);

CREATE TABLE services_variables (
    id_service bigint NOT NULL,
    varname character varying(100) NOT NULL,
    enabled boolean NOT NULL
);

CREATE TABLE speed_dials (
    id_operator bigint NOT NULL,
    group_name character varying(100) NOT NULL,
    name character varying(100) DEFAULT NULL::character varying,
    num character varying(50) NOT NULL,
    blf boolean NOT NULL,
    order_num smallint NOT NULL
);

ALTER TABLE ONLY actions
    ADD CONSTRAINT actions_pkey PRIMARY KEY (id);

ALTER TABLE ONLY extensions
    ADD CONSTRAINT extensions_pkey PRIMARY KEY (id_seat, exten);

ALTER TABLE ONLY operators
    ADD CONSTRAINT operators_pkey PRIMARY KEY (id);

ALTER TABLE ONLY operators
    ADD CONSTRAINT operators_username_key UNIQUE (username);

ALTER TABLE ONLY seats
    ADD CONSTRAINT seats_pkey PRIMARY KEY (id);

ALTER TABLE ONLY server_settings
    ADD CONSTRAINT server_settings_pkey PRIMARY KEY (name);

ALTER TABLE ONLY services_actions
    ADD CONSTRAINT services_actions_pkey PRIMARY KEY (id_service, id_action);

ALTER TABLE ONLY services_operators
    ADD CONSTRAINT services_operators_pkey PRIMARY KEY (id_service, id_operator);

ALTER TABLE ONLY services
    ADD CONSTRAINT services_pkey PRIMARY KEY (id);

ALTER TABLE ONLY services_variables
    ADD CONSTRAINT services_variables_pkey PRIMARY KEY (id_service, varname);

ALTER TABLE ONLY speed_dials
    ADD CONSTRAINT speed_dials_pkey PRIMARY KEY (id_operator, group_name, order_num);

CREATE INDEX extensions_id_seat_idx ON extensions USING btree (id_seat);

CREATE INDEX operators_id_seat_idx ON operators USING btree (id_seat);

CREATE INDEX services_actions_id_action_idx ON services_actions USING btree (id_action);

CREATE INDEX services_actions_id_service_idx ON services_actions USING btree (id_service);

CREATE INDEX services_operators_id_operator_idx ON services_operators USING btree (id_operator);

CREATE INDEX services_operators_id_service_idx ON services_operators USING btree (id_service);

CREATE INDEX services_variables_id_service_idx ON services_variables USING btree (id_service);

CREATE INDEX speed_dials_id_operator_idx ON speed_dials USING btree (id_operator);

ALTER TABLE ONLY extensions
    ADD CONSTRAINT fk_extensions_seats FOREIGN KEY (id_seat) REFERENCES seats(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY operators
    ADD CONSTRAINT fk_operators_seats FOREIGN KEY (id_seat) REFERENCES seats(id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY services_actions
    ADD CONSTRAINT fk_services_actions_actions FOREIGN KEY (id_action) REFERENCES actions(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY services_actions
    ADD CONSTRAINT fk_services_actions_services FOREIGN KEY (id_service) REFERENCES services(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY services_operators
    ADD CONSTRAINT fk_services_operators_operators FOREIGN KEY (id_operator) REFERENCES operators(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY services_operators
    ADD CONSTRAINT fk_services_operators_services FOREIGN KEY (id_service) REFERENCES services(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY services_variables
    ADD CONSTRAINT fk_services_variables_services FOREIGN KEY (id_service) REFERENCES services(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY speed_dials
    ADD CONSTRAINT fk_speeddials_operators FOREIGN KEY (id_operator) REFERENCES operators(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;

--
-- PostgreSQL database dump complete
--

