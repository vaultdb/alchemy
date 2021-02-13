--
-- PostgreSQL database dump
--

-- Dumped from database version 12.5
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
0	004	35992	F	t	4	0	0	1
1	033	17226	M	f	4	1	0	1
2	030	13218	F	t	0	0	0	1
3	041	13789	F	t	3	0	1	1
4	032	35108	M	t	2	0	0	1
5	025	20553	F	f	2	1	0	1
6	022	24260	F	t	1	0	0	1
7	007	27472	F	f	5	1	0	1
8	031	30049	F	f	5	0	0	1
9	002	16523	F	f	0	0	0	1
10	046	22238	M	t	2	0	0	1
11	019	21106	F	f	4	0	0	1
12	027	30983	M	t	2	0	0	1
13	017	30892	M	f	4	1	0	1
14	009	9543	F	f	5	1	0	1
15	018	25847	F	f	3	1	0	1
16	001	33617	M	f	1	0	0	1
17	005	30896	M	f	0	0	0	1
18	006	20901	F	t	5	1	0	1
19	010	32615	F	f	2	0	1	1
20	045	12248	M	t	1	0	0	1
21	022	6996	F	f	4	0	0	1
22	014	28751	M	t	3	0	0	1
23	014	32475	F	f	0	1	0	1
24	036	7266	M	t	3	0	0	1
2	030	13218	F	t	0	0	0	2
6	022	24260	F	t	1	0	0	2
8	031	30049	F	f	5	0	1	2
9	002	16523	F	f	0	0	0	2
12	027	30983	M	t	2	0	0	2
16	001	33617	M	f	1	0	0	2
17	005	30896	M	f	0	0	1	2
18	006	20901	F	t	5	1	0	2
20	045	12248	M	t	1	0	1	2
22	014	28751	M	t	3	0	1	2
25	035	33828	M	t	3	0	0	1
26	049	8131	F	t	4	0	0	1
27	030	12683	F	f	1	0	0	1
28	025	13043	F	t	0	0	1	1
29	028	16049	F	f	0	0	0	1
30	027	36384	F	t	5	1	0	1
31	017	31667	M	t	4	1	1	1
32	023	14088	F	f	5	0	0	1
33	028	10658	F	t	2	0	0	1
34	023	31820	F	f	4	1	0	1
35	011	17434	F	f	5	0	0	1
36	020	20076	F	f	4	0	0	1
37	024	35556	M	f	1	1	0	1
38	048	9199	F	f	1	0	0	1
39	035	13720	M	f	1	0	0	1
5	025	20553	F	f	2	1	0	3
7	007	27472	F	f	5	1	0	3
8	031	30049	F	f	5	0	0	3
9	002	16523	F	f	0	0	0	3
10	046	22238	M	t	2	0	1	3
11	019	21106	F	f	4	0	0	3
12	027	30983	M	t	2	0	1	3
16	001	33617	M	f	1	0	1	3
17	005	30896	M	f	0	0	0	3
19	010	32615	F	f	2	0	1	3
25	035	33828	M	t	3	0	0	3
26	049	8131	F	t	4	0	0	3
28	025	13043	F	t	0	0	1	3
29	028	16049	F	f	0	0	0	3
30	027	36384	F	t	5	1	0	3
32	023	14088	F	f	5	0	0	3
38	048	9199	F	f	1	0	0	3
40	027	29595	F	f	1	0	0	3
41	000	34000	M	t	2	0	0	3
42	022	22910	F	t	2	0	0	3
43	005	25693	F	t	1	1	0	3
44	029	17251	M	f	4	1	0	3
45	022	27613	M	t	2	0	1	3
46	008	25179	F	t	0	0	0	3
47	031	32593	F	t	3	0	1	3
\.


--
-- PostgreSQL database dump complete
--

