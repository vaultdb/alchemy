--
-- PostgreSQL database dump
--

-- Dumped from database version 15.2 (Homebrew)
-- Dumped by pg_dump version 15.2 (Homebrew)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: bit_packing; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.bit_packing (
    table_name character varying(25),
    col_name character varying(25),
    min integer,
    max integer,
    domain_size integer
);


ALTER TABLE public.bit_packing OWNER TO vaultdb;

--
-- Data for Name: bit_packing; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.bit_packing (table_name, col_name, min, max, domain_size) FROM stdin;
customer	c_custkey	1	15000	15000
customer	c_nationkey	0	24	25
customer	c_mktsegment	-1	-1	5
supplier	s_suppkey	1	1000	1000
supplier	s_name	-1	-1	2000
supplier	s_nationkey	0	24	25
partsupp	ps_partkey	1	40000	40000
partsupp	ps_suppkey	1	1000	1000
partsupp	ps_availqty	1	9999	9999
nation	n_nationkey	0	24	25
nation	n_name	-1	-1	25
nation	n_regionkey	0	4	5
nation	n_comment	-1	-1	25
orders	o_orderkey	1	1200000	300000
orders	o_custkey	1	15000	15000
orders	o_orderstatus	-1	-1	3
orders	o_orderdate	8035	10591	2557
orders	o_orderpriority	-1	-1	5
orders	o_orderyear	1992	1998	7
lineitem	l_orderkey	1	1200000	300000
lineitem	l_partkey	1	20000	20000
lineitem	l_suppkey	1	2000	2000
lineitem	l_linenumber	1	7	7
lineitem	l_quantity	1	50	50
lineitem	l_returnflag	-1	-1	3
lineitem	l_linestatus	-1	-1	2
lineitem	l_shipinstruct	-1	-1	4
lineitem	l_shipmode	-1	-1	7
part	p_partkey	1	40000	40000
part	p_name	-1	-1	40000
part	p_mfgr	-1	-1	5
part	p_brand	-1	-1	25
part	p_type	-1	-1	150
part	p_size	1	50	50
part	p_container	-1	-1	40
part	p_retailprice	901	1939	4899
part	p_comment	-1	-1	32858
region	r_regionkey	0	4	5
region	r_name	-1	-1	5
region	r_comment	-1	-1	5
orders	o_shippriority	0	0	1
\.


--
-- PostgreSQL database dump complete
--

