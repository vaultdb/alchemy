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
0	003	28605	F	f	2	0	0	1
1	021	10590	M	f	1	0	0	1
2	045	7269	M	t	1	0	0	1
3	016	31134	M	t	5	0	0	1
4	007	36481	M	t	0	0	0	1
5	049	34918	F	t	3	0	0	1
6	037	17639	F	f	5	0	0	1
7	033	25532	F	f	0	1	0	1
8	012	21708	M	t	3	0	0	1
9	026	19059	M	t	4	1	1	1
0	003	28605	F	f	2	0	1	2
1	021	10590	M	f	1	0	1	2
2	045	7269	M	t	1	0	1	2
3	016	31134	M	t	5	0	0	2
5	049	34918	F	t	3	0	0	2
6	037	17639	F	f	5	0	0	2
7	033	25532	F	f	0	1	1	2
8	012	21708	M	t	3	0	1	2
9	026	19059	M	t	4	1	0	2
10	016	26502	F	f	4	0	0	1
2	045	7269	M	t	1	0	0	3
5	049	34918	F	t	3	0	0	3
11	025	16090	M	t	5	1	0	3
12	011	30242	F	t	2	0	1	3
13	036	28225	F	f	4	0	0	3
14	008	25825	M	f	1	0	0	3
15	017	15492	F	f	1	0	0	3
16	042	15586	M	t	4	0	0	3
17	001	22740	M	f	3	0	0	3
18	041	33229	F	f	3	0	0	3
\.


--
-- PostgreSQL database dump complete
--

