--
-- PostgreSQL database dump
--

-- Dumped from database version 13.2
-- Dumped by pg_dump version 13.2

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
-- Name: patient; Type: TABLE; Schema: public; Owner: jennie
--

CREATE TABLE public.patient (
    patid integer,
    zip_marker character varying(3),
    age_days integer,
    sex character varying(1),
    ethnicity boolean,
    race integer,
    numerator integer,
    denom_excl integer,
    site_id integer
);


ALTER TABLE public.patient OWNER TO jennie;

--
-- Data for Name: patient; Type: TABLE DATA; Schema: public; Owner: jennie
--

COPY public.patient (patid, zip_marker, age_days, sex, ethnicity, race, numerator, denom_excl, site_id) FROM stdin;
0	015	24571	M	f	3	0	0	1
1	030	9005	M	t	0	0	0	1
2	035	6874	F	f	0	0	0	1
3	024	8689	F	f	3	0	0	1
4	011	17126	F	t	5	1	0	1
5	016	29819	M	t	2	1	1	1
6	046	18089	M	f	0	0	0	1
7	038	9621	M	f	2	1	0	1
8	015	35738	F	t	3	0	0	1
9	047	12569	M	f	3	0	0	1
0	015	24571	M	f	3	0	0	2
1	030	9005	M	t	0	0	1	2
3	024	8689	F	f	3	0	0	2
4	011	17126	F	t	5	1	0	2
5	016	29819	M	t	2	1	0	2
8	015	35738	F	t	3	0	1	2
9	047	12569	M	f	3	0	1	2
10	029	20661	M	t	4	0	0	1
11	016	33851	F	f	0	0	0	1
12	007	10940	M	t	0	1	0	1
10	029	20661	M	t	4	0	0	3
12	007	10940	M	t	0	1	0	3
13	005	17055	F	f	2	1	0	3
14	004	28215	F	t	5	0	0	3
15	035	24616	F	f	1	1	0	3
16	000	23384	M	f	0	0	0	3
17	042	23027	F	f	1	0	0	3
18	032	19599	M	t	1	0	1	3
19	010	17050	M	f	3	0	0	3
20	020	11377	F	t	0	0	0	3
\.


--
-- PostgreSQL database dump complete
--

