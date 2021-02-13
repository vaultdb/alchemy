--
-- PostgreSQL database dump
--

-- Dumped from database version 12.5 (Ubuntu 12.5-0ubuntu0.20.04.1)
-- Dumped by pg_dump version 12.5 (Ubuntu 12.5-0ubuntu0.20.04.1)

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
-- Name: patient; Type: TABLE; Schema: public; Owner: vaultdb
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


ALTER TABLE public.patient OWNER TO vaultdb;

--
-- Data for Name: patient; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.patient (patid, zip_marker, age_days, sex, ethnicity, race, numerator, denom_excl, site_id) FROM stdin;
0	028	24163	M	t	3	0	0	1
1	008	18402	M	f	4	0	0	1
2	040	35043	F	f	3	1	0	1
3	043	25341	F	f	5	0	0	1
4	001	18943	M	t	5	0	0	1
5	009	11289	F	f	2	1	0	1
6	010	36478	F	f	2	0	0	1
7	038	12704	M	t	3	0	0	1
8	020	9460	F	t	4	1	0	1
9	042	9345	M	t	0	1	0	1
10	048	36445	F	f	5	1	0	1
11	037	21896	F	t	0	0	0	1
12	033	20229	F	f	3	0	0	1
13	015	20778	F	f	3	1	1	1
14	017	18058	F	t	5	0	0	1
15	036	21030	M	t	4	0	0	1
16	030	28634	M	t	4	1	0	1
17	014	32828	F	t	0	1	0	1
18	041	25966	M	f	1	0	0	1
19	031	22354	F	t	5	0	0	1
20	029	23206	M	f	2	0	0	1
21	010	25831	M	t	5	1	0	1
22	044	18949	M	f	4	0	0	1
23	007	30099	M	t	5	1	0	1
24	013	17858	M	f	1	1	0	1
25	002	7780	M	t	3	0	1	1
26	035	6866	F	t	5	0	1	1
27	027	34325	F	f	5	0	0	1
28	009	30285	M	f	3	0	1	1
29	042	32696	M	f	4	1	0	1
30	026	20267	M	t	1	1	0	1
31	049	23220	M	f	3	1	0	1
32	025	12258	F	f	2	1	1	1
33	008	8927	M	t	2	1	0	1
34	028	14609	F	t	1	0	0	1
35	031	9754	M	f	0	0	0	1
36	045	12961	M	t	0	0	1	1
37	025	10238	M	t	4	1	0	1
38	046	21279	M	f	1	0	0	1
39	040	35298	F	t	1	0	0	1
40	024	24268	M	t	5	1	0	1
41	008	18460	F	t	5	0	1	1
42	005	10167	F	t	4	0	0	1
43	036	29489	M	t	1	1	0	1
44	015	15473	M	t	3	0	0	1
45	016	11134	F	t	1	0	0	1
46	004	29049	F	t	1	0	0	1
47	035	14358	F	f	0	0	0	1
48	015	9372	M	t	1	1	0	1
49	012	9919	M	t	5	1	0	1
0	028	24163	M	t	3	0	0	2
1	008	18402	M	f	4	0	0	2
4	001	18943	M	t	5	0	1	2
5	009	11289	F	f	2	1	1	2
6	010	36478	F	f	2	0	0	2
11	037	21896	F	t	0	0	0	2
12	033	20229	F	f	3	0	1	2
14	017	18058	F	t	5	0	1	2
15	036	21030	M	t	4	0	0	2
16	030	28634	M	t	4	1	0	2
17	014	32828	F	t	0	1	0	2
19	031	22354	F	t	5	0	1	2
20	029	23206	M	f	2	0	1	2
21	010	25831	M	t	5	1	1	2
26	035	6866	F	t	5	0	0	2
27	027	34325	F	f	5	0	1	2
28	009	30285	M	f	3	0	0	2
29	042	32696	M	f	4	1	0	2
32	025	12258	F	f	2	1	0	2
34	028	14609	F	t	1	0	0	2
37	025	10238	M	t	4	1	0	2
40	024	24268	M	t	5	1	1	2
41	008	18460	F	t	5	0	0	2
44	015	15473	M	t	3	0	1	2
50	005	15977	F	t	4	0	0	1
51	046	19790	M	f	3	1	0	1
52	029	25692	F	f	1	0	0	1
53	045	30317	M	f	4	0	0	1
54	036	17273	M	f	0	0	0	1
55	016	12460	M	t	2	0	0	1
56	036	9007	F	f	3	0	0	1
57	029	6636	M	t	0	0	0	1
58	025	9258	F	f	1	0	0	1
59	025	13176	M	t	1	0	0	1
60	026	6615	F	f	4	1	0	1
61	002	16126	M	t	4	0	0	1
62	014	8362	M	f	5	0	0	1
63	040	11705	M	t	1	0	1	1
64	000	19096	M	f	2	1	0	1
65	047	32151	F	t	3	0	1	1
66	005	8878	M	f	5	1	0	1
67	020	29801	M	f	5	0	0	1
68	008	25294	F	t	3	1	0	1
69	014	9880	F	t	3	1	0	1
70	042	27610	M	f	0	1	0	1
71	012	24865	M	f	5	1	1	1
72	005	7434	M	f	5	0	1	1
73	030	8598	F	f	2	1	0	1
74	046	29664	F	t	1	0	0	1
75	032	34002	F	t	0	0	0	1
0	028	24163	M	t	3	0	1	3
1	008	18402	M	f	4	0	1	3
2	040	35043	F	f	3	1	1	3
5	009	11289	F	f	2	1	0	3
9	042	9345	M	t	0	1	1	3
13	015	20778	F	f	3	1	1	3
16	030	28634	M	t	4	1	0	3
20	029	23206	M	f	2	0	0	3
22	044	18949	M	f	4	0	1	3
26	035	6866	F	t	5	0	1	3
27	027	34325	F	f	5	0	1	3
28	009	30285	M	f	3	0	1	3
30	026	20267	M	t	1	1	0	3
31	049	23220	M	f	3	1	0	3
32	025	12258	F	f	2	1	0	3
45	016	11134	F	t	1	0	0	3
48	015	9372	M	t	1	1	0	3
49	012	9919	M	t	5	1	0	3
54	036	17273	M	f	0	0	0	3
56	036	9007	F	f	3	0	0	3
58	025	9258	F	f	1	0	1	3
61	002	16126	M	t	4	0	0	3
62	014	8362	M	f	5	0	1	3
63	040	11705	M	t	1	0	1	3
66	005	8878	M	f	5	1	1	3
68	008	25294	F	t	3	1	0	3
70	042	27610	M	f	0	1	1	3
72	005	7434	M	f	5	0	1	3
73	030	8598	F	f	2	1	0	3
75	032	34002	F	t	0	0	1	3
76	006	24513	F	f	5	0	0	3
77	045	11468	M	f	4	1	0	3
78	007	26288	M	f	0	0	0	3
79	012	7925	F	f	1	0	0	3
80	044	24859	F	f	0	1	0	3
81	025	17552	M	t	2	0	0	3
82	044	32397	M	f	3	0	0	3
83	028	23858	M	f	1	0	0	3
84	046	31348	F	f	1	0	0	3
85	003	8941	F	f	2	1	0	3
86	036	27964	M	t	1	0	0	3
87	011	13090	F	t	4	1	0	3
88	026	19953	M	f	3	1	0	3
89	022	12782	M	t	1	0	0	3
90	009	19589	F	f	1	0	1	3
91	026	16492	M	t	5	0	0	3
92	003	13834	M	t	0	0	0	3
93	011	23342	F	f	2	1	0	3
94	031	28714	F	t	1	1	0	3
95	048	23567	M	t	3	0	0	3
\.


--
-- PostgreSQL database dump complete
--

