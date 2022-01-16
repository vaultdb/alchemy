--
-- PostgreSQL database dump
--

-- Dumped from database version 14.0
-- Dumped by pg_dump version 14.0

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
-- Name: demographics_domain; Type: TABLE; Schema: public; Owner: jennie
--

CREATE TABLE public.demographics_domain (
    age_strata character(1),
    sex character(1),
    ethnicity character(1),
    race character(1)
);


ALTER TABLE public.demographics_domain OWNER TO jennie;

--
-- Name: patient; Type: TABLE; Schema: public; Owner: jennie
--

CREATE TABLE public.patient (
    pat_id integer,
    study_year integer,
    age_strata character(1),
    sex character(1),
    ethnicity character(1),
    race character(1),
    numerator boolean,
    denom_excl boolean,
    site_id integer,
    multisite boolean DEFAULT false
);


ALTER TABLE public.patient OWNER TO jennie;

--
-- Name: patient_exclusion; Type: TABLE; Schema: public; Owner: jennie
--

CREATE TABLE public.patient_exclusion (
    patid integer,
    numerator integer,
    denom_excl integer,
    site_id integer
);


ALTER TABLE public.patient_exclusion OWNER TO jennie;

--
-- Data for Name: demographics_domain; Type: TABLE DATA; Schema: public; Owner: jennie
--

COPY public.demographics_domain (age_strata, sex, ethnicity, race) FROM stdin;
1	F	N	1
1	F	N	2
1	F	N	3
1	F	N	4
1	F	N	5
1	F	N	6
1	F	N	7
1	F	U	1
1	F	U	2
1	F	U	3
1	F	U	4
1	F	U	5
1	F	U	6
1	F	U	7
1	F	Y	1
1	F	Y	2
1	F	Y	3
1	F	Y	4
1	F	Y	5
1	F	Y	6
1	F	Y	7
1	M	N	1
1	M	N	2
1	M	N	3
1	M	N	4
1	M	N	5
1	M	N	6
1	M	N	7
1	M	U	1
1	M	U	2
1	M	U	3
1	M	U	4
1	M	U	5
1	M	U	6
1	M	U	7
1	M	Y	1
1	M	Y	2
1	M	Y	3
1	M	Y	4
1	M	Y	5
1	M	Y	6
1	M	Y	7
1	U	N	1
1	U	N	2
1	U	N	3
1	U	N	4
1	U	N	5
1	U	N	6
1	U	N	7
1	U	U	1
1	U	U	2
1	U	U	3
1	U	U	4
1	U	U	5
1	U	U	6
1	U	U	7
1	U	Y	1
1	U	Y	2
1	U	Y	3
1	U	Y	4
1	U	Y	5
1	U	Y	6
1	U	Y	7
2	F	N	1
2	F	N	2
2	F	N	3
2	F	N	4
2	F	N	5
2	F	N	6
2	F	N	7
2	F	U	1
2	F	U	2
2	F	U	3
2	F	U	4
2	F	U	5
2	F	U	6
2	F	U	7
2	F	Y	1
2	F	Y	2
2	F	Y	3
2	F	Y	4
2	F	Y	5
2	F	Y	6
2	F	Y	7
2	M	N	1
2	M	N	2
2	M	N	3
2	M	N	4
2	M	N	5
2	M	N	6
2	M	N	7
2	M	U	1
2	M	U	2
2	M	U	3
2	M	U	4
2	M	U	5
2	M	U	6
2	M	U	7
2	M	Y	1
2	M	Y	2
2	M	Y	3
2	M	Y	4
2	M	Y	5
2	M	Y	6
2	M	Y	7
2	U	N	1
2	U	N	2
2	U	N	3
2	U	N	4
2	U	N	5
2	U	N	6
2	U	N	7
2	U	U	1
2	U	U	2
2	U	U	3
2	U	U	4
2	U	U	5
2	U	U	6
2	U	U	7
2	U	Y	1
2	U	Y	2
2	U	Y	3
2	U	Y	4
2	U	Y	5
2	U	Y	6
2	U	Y	7
3	F	N	1
3	F	N	2
3	F	N	3
3	F	N	4
3	F	N	5
3	F	N	6
3	F	N	7
3	F	U	1
3	F	U	2
3	F	U	3
3	F	U	4
3	F	U	5
3	F	U	6
3	F	U	7
3	F	Y	1
3	F	Y	2
3	F	Y	3
3	F	Y	4
3	F	Y	5
3	F	Y	6
3	F	Y	7
3	M	N	1
3	M	N	2
3	M	N	3
3	M	N	4
3	M	N	5
3	M	N	6
3	M	N	7
3	M	U	1
3	M	U	2
3	M	U	3
3	M	U	4
3	M	U	5
3	M	U	6
3	M	U	7
3	M	Y	1
3	M	Y	2
3	M	Y	3
3	M	Y	4
3	M	Y	5
3	M	Y	6
3	M	Y	7
3	U	N	1
3	U	N	2
3	U	N	3
3	U	N	4
3	U	N	5
3	U	N	6
3	U	N	7
3	U	U	1
3	U	U	2
3	U	U	3
3	U	U	4
3	U	U	5
3	U	U	6
3	U	U	7
3	U	Y	1
3	U	Y	2
3	U	Y	3
3	U	Y	4
3	U	Y	5
3	U	Y	6
3	U	Y	7
4	F	N	1
4	F	N	2
4	F	N	3
4	F	N	4
4	F	N	5
4	F	N	6
4	F	N	7
4	F	U	1
4	F	U	2
4	F	U	3
4	F	U	4
4	F	U	5
4	F	U	6
4	F	U	7
4	F	Y	1
4	F	Y	2
4	F	Y	3
4	F	Y	4
4	F	Y	5
4	F	Y	6
4	F	Y	7
4	M	N	1
4	M	N	2
4	M	N	3
4	M	N	4
4	M	N	5
4	M	N	6
4	M	N	7
4	M	U	1
4	M	U	2
4	M	U	3
4	M	U	4
4	M	U	5
4	M	U	6
4	M	U	7
4	M	Y	1
4	M	Y	2
4	M	Y	3
4	M	Y	4
4	M	Y	5
4	M	Y	6
4	M	Y	7
4	U	N	1
4	U	N	2
4	U	N	3
4	U	N	4
4	U	N	5
4	U	N	6
4	U	N	7
4	U	U	1
4	U	U	2
4	U	U	3
4	U	U	4
4	U	U	5
4	U	U	6
4	U	U	7
4	U	Y	1
4	U	Y	2
4	U	Y	3
4	U	Y	4
4	U	Y	5
4	U	Y	6
4	U	Y	7
5	F	N	1
5	F	N	2
5	F	N	3
5	F	N	4
5	F	N	5
5	F	N	6
5	F	N	7
5	F	U	1
5	F	U	2
5	F	U	3
5	F	U	4
5	F	U	5
5	F	U	6
5	F	U	7
5	F	Y	1
5	F	Y	2
5	F	Y	3
5	F	Y	4
5	F	Y	5
5	F	Y	6
5	F	Y	7
5	M	N	1
5	M	N	2
5	M	N	3
5	M	N	4
5	M	N	5
5	M	N	6
5	M	N	7
5	M	U	1
5	M	U	2
5	M	U	3
5	M	U	4
5	M	U	5
5	M	U	6
5	M	U	7
5	M	Y	1
5	M	Y	2
5	M	Y	3
5	M	Y	4
5	M	Y	5
5	M	Y	6
5	M	Y	7
5	U	N	1
5	U	N	2
5	U	N	3
5	U	N	4
5	U	N	5
5	U	N	6
5	U	N	7
5	U	U	1
5	U	U	2
5	U	U	3
5	U	U	4
5	U	U	5
5	U	U	6
5	U	U	7
5	U	Y	1
5	U	Y	2
5	U	Y	3
5	U	Y	4
5	U	Y	5
5	U	Y	6
5	U	Y	7
6	F	N	1
6	F	N	2
6	F	N	3
6	F	N	4
6	F	N	5
6	F	N	6
6	F	N	7
6	F	U	1
6	F	U	2
6	F	U	3
6	F	U	4
6	F	U	5
6	F	U	6
6	F	U	7
6	F	Y	1
6	F	Y	2
6	F	Y	3
6	F	Y	4
6	F	Y	5
6	F	Y	6
6	F	Y	7
6	M	N	1
6	M	N	2
6	M	N	3
6	M	N	4
6	M	N	5
6	M	N	6
6	M	N	7
6	M	U	1
6	M	U	2
6	M	U	3
6	M	U	4
6	M	U	5
6	M	U	6
6	M	U	7
6	M	Y	1
6	M	Y	2
6	M	Y	3
6	M	Y	4
6	M	Y	5
6	M	Y	6
6	M	Y	7
6	U	N	1
6	U	N	2
6	U	N	3
6	U	N	4
6	U	N	5
6	U	N	6
6	U	N	7
6	U	U	1
6	U	U	2
6	U	U	3
6	U	U	4
6	U	U	5
6	U	U	6
6	U	U	7
6	U	Y	1
6	U	Y	2
6	U	Y	3
6	U	Y	4
6	U	Y	5
6	U	Y	6
6	U	Y	7
7	F	N	1
7	F	N	2
7	F	N	3
7	F	N	4
7	F	N	5
7	F	N	6
7	F	N	7
7	F	U	1
7	F	U	2
7	F	U	3
7	F	U	4
7	F	U	5
7	F	U	6
7	F	U	7
7	F	Y	1
7	F	Y	2
7	F	Y	3
7	F	Y	4
7	F	Y	5
7	F	Y	6
7	F	Y	7
7	M	N	1
7	M	N	2
7	M	N	3
7	M	N	4
7	M	N	5
7	M	N	6
7	M	N	7
7	M	U	1
7	M	U	2
7	M	U	3
7	M	U	4
7	M	U	5
7	M	U	6
7	M	U	7
7	M	Y	1
7	M	Y	2
7	M	Y	3
7	M	Y	4
7	M	Y	5
7	M	Y	6
7	M	Y	7
7	U	N	1
7	U	N	2
7	U	N	3
7	U	N	4
7	U	N	5
7	U	N	6
7	U	N	7
7	U	U	1
7	U	U	2
7	U	U	3
7	U	U	4
7	U	U	5
7	U	U	6
7	U	U	7
7	U	Y	1
7	U	Y	2
7	U	Y	3
7	U	Y	4
7	U	Y	5
7	U	Y	6
7	U	Y	7
\.


--
-- Data for Name: patient; Type: TABLE DATA; Schema: public; Owner: jennie
--

COPY public.patient (pat_id, study_year, age_strata, sex, ethnicity, race, numerator, denom_excl, site_id, multisite) FROM stdin;
2	2020	4	U	N	6	f	f	1	f
7	2020	1	M	U	3	t	f	1	f
9	2019	5	F	N	6	f	f	1	f
11	2020	7	F	N	2	t	f	1	f
12	2019	5	M	Y	3	t	f	1	f
15	2019	2	M	N	4	f	t	1	f
16	2020	4	U	N	5	t	f	1	f
17	2019	4	F	U	1	f	f	1	f
25	2020	2	U	N	2	f	t	1	f
28	2018	4	U	U	7	f	f	1	f
35	2020	2	U	U	7	t	f	1	f
37	2019	4	U	N	4	f	t	1	f
42	2019	2	U	Y	3	f	f	1	f
43	2018	6	U	U	5	f	f	1	f
46	2020	4	M	N	4	f	f	1	f
47	2019	2	U	U	4	f	f	1	f
48	2019	1	U	N	4	f	f	1	f
50	2019	6	M	U	2	f	f	1	f
52	2018	5	M	N	7	f	f	1	f
55	2018	3	F	U	3	f	f	1	f
61	2018	4	F	U	4	f	f	1	f
62	2019	2	F	U	2	f	f	1	f
65	2018	7	F	U	1	f	f	1	f
66	2018	1	M	Y	7	t	f	1	f
73	2018	4	F	N	2	f	f	1	f
75	2018	5	F	N	2	f	f	1	f
78	2018	4	M	U	7	f	f	1	f
79	2019	3	F	U	5	f	f	1	f
82	2020	1	U	N	3	f	f	1	f
90	2019	4	M	Y	6	t	f	1	f
92	2018	7	F	U	5	f	f	1	f
93	2020	7	M	N	5	f	f	1	f
94	2019	3	U	N	1	f	f	1	f
98	2019	1	F	Y	1	f	f	1	f
104	2018	6	M	Y	5	f	t	1	f
105	2018	3	M	U	7	f	f	1	f
111	2020	4	F	U	6	f	f	1	f
114	2018	7	U	N	7	f	f	1	f
119	2020	5	U	Y	6	t	f	1	f
132	2020	4	M	N	5	f	f	1	f
133	2019	1	F	Y	2	f	f	1	f
137	2019	5	M	Y	6	t	f	1	f
141	2020	3	F	U	1	f	f	1	f
143	2018	4	F	Y	1	f	f	1	f
144	2020	6	U	Y	3	f	f	1	f
145	2019	1	U	U	2	f	f	1	f
147	2019	2	F	Y	1	f	f	1	f
152	2018	1	U	U	4	f	f	1	f
154	2019	1	U	U	2	t	f	1	f
158	2020	2	M	N	7	f	f	1	f
160	2020	7	M	U	2	f	f	1	f
167	2019	7	F	N	3	t	f	1	f
168	2018	1	U	Y	2	t	f	1	f
169	2018	1	U	U	2	f	f	1	f
180	2020	6	F	N	6	f	f	1	f
185	2019	3	M	Y	2	f	f	1	f
186	2019	1	U	U	5	f	f	1	f
187	2019	2	F	N	5	t	f	1	f
189	2020	4	M	Y	3	f	f	1	f
191	2018	4	U	N	2	f	f	1	f
192	2020	5	F	Y	5	t	f	1	f
196	2018	4	F	U	7	f	f	1	f
200	2020	4	F	N	4	f	f	1	f
204	2020	5	U	Y	4	f	f	1	f
205	2018	2	U	U	2	t	f	1	f
206	2018	1	M	N	7	f	f	1	f
209	2019	6	F	Y	4	f	f	1	f
213	2018	7	U	Y	4	t	t	1	f
215	2018	6	F	N	3	t	f	1	f
217	2020	2	F	Y	6	f	f	1	f
221	2019	7	F	N	3	f	f	1	f
227	2019	6	M	Y	7	f	f	1	f
228	2020	1	U	U	4	f	f	1	f
230	2018	7	U	N	5	f	f	1	f
232	2018	6	F	Y	1	f	f	1	f
233	2020	5	U	N	2	f	f	1	f
239	2020	4	F	Y	2	f	f	1	f
245	2018	3	F	U	5	f	t	1	f
250	2018	6	F	U	4	f	f	1	f
254	2018	6	U	N	2	f	f	1	f
255	2018	5	M	N	6	t	f	1	f
258	2020	6	M	N	6	f	f	1	f
263	2019	1	F	N	2	f	f	1	f
264	2019	3	M	N	5	f	f	1	f
266	2018	7	U	Y	6	f	f	1	f
269	2020	2	U	U	7	f	f	1	f
270	2020	2	U	N	3	t	t	1	f
273	2018	6	U	N	6	t	f	1	f
275	2019	4	U	Y	7	f	t	1	f
277	2019	3	M	U	2	t	f	1	f
280	2020	2	U	N	2	f	f	1	f
290	2020	6	F	U	5	f	f	1	f
292	2018	2	F	N	3	f	f	1	f
294	2019	1	F	Y	5	f	f	1	f
295	2020	4	M	Y	7	f	f	1	f
297	2018	6	M	N	1	f	f	1	f
298	2018	4	F	N	2	f	f	1	f
299	2019	5	U	N	5	f	f	1	f
300	2019	3	F	Y	6	f	t	1	f
301	2020	2	F	N	7	f	f	1	f
305	2020	1	M	Y	5	f	f	1	f
306	2019	7	U	U	2	f	f	1	f
309	2018	1	M	U	4	f	t	1	f
310	2019	1	F	N	6	t	f	1	f
311	2019	5	F	U	2	f	f	1	f
314	2020	5	U	N	1	f	f	1	f
315	2018	1	F	Y	1	t	f	1	f
327	2020	4	F	Y	1	t	f	1	f
328	2020	4	M	U	6	t	f	1	f
330	2020	5	U	N	6	t	f	1	f
331	2019	2	U	N	2	f	f	1	f
339	2018	7	M	U	6	f	t	1	f
344	2019	5	U	U	7	f	f	1	f
345	2019	6	F	Y	2	f	f	1	f
347	2018	4	M	U	6	f	f	1	f
348	2019	6	F	Y	7	f	f	1	f
349	2019	5	F	U	2	f	f	1	f
356	2020	6	F	U	7	f	f	1	f
358	2018	7	M	Y	3	t	f	1	f
365	2018	1	F	Y	4	f	f	1	f
367	2019	5	U	Y	6	f	f	1	f
369	2019	5	U	N	4	t	f	1	f
371	2020	5	F	N	3	f	f	1	f
372	2019	4	F	U	6	f	f	1	f
373	2018	3	F	N	1	f	f	1	f
375	2018	1	M	N	5	f	f	1	f
378	2019	1	M	Y	3	f	t	1	f
380	2020	3	F	U	7	f	f	1	f
381	2019	1	F	Y	5	f	f	1	f
382	2020	1	U	Y	7	f	f	1	f
383	2020	4	M	U	7	f	f	1	f
385	2018	2	M	Y	6	f	t	1	f
391	2020	5	U	N	1	f	f	1	f
397	2020	5	M	N	2	f	t	1	f
399	2018	6	U	Y	5	f	f	1	f
410	2019	6	U	Y	5	t	f	1	f
412	2019	3	U	U	1	f	t	1	f
419	2020	4	U	N	3	f	f	1	f
423	2019	4	F	U	1	f	f	1	f
426	2019	1	M	U	3	t	f	1	f
427	2018	1	U	U	7	t	f	1	f
429	2018	5	M	N	5	f	f	1	f
431	2019	7	M	Y	5	f	t	1	f
433	2020	3	U	U	3	f	f	1	f
434	2018	5	F	U	3	f	f	1	f
437	2018	1	U	N	3	t	f	1	f
438	2018	7	F	N	2	t	f	1	f
439	2019	1	F	N	4	t	t	1	f
441	2020	3	U	N	1	f	f	1	f
443	2018	6	F	N	1	t	f	1	f
456	2018	4	U	Y	2	f	f	1	f
457	2020	1	F	U	3	f	f	1	f
458	2019	5	M	N	1	f	f	1	f
461	2019	4	U	N	3	f	f	1	f
463	2019	4	U	N	6	f	f	1	f
466	2020	6	F	Y	1	f	f	1	f
467	2018	3	U	N	4	f	f	1	f
475	2020	1	F	U	2	f	f	1	f
480	2018	7	F	U	5	f	t	1	f
481	2019	6	M	N	2	f	f	1	f
486	2018	4	M	N	5	f	f	1	f
489	2019	2	F	N	6	f	f	1	f
493	2019	1	U	U	7	f	f	1	f
496	2020	4	F	U	2	f	f	1	f
497	2018	5	F	N	1	t	f	1	f
498	2019	4	M	U	3	f	f	1	f
500	2020	3	M	Y	2	f	f	1	f
501	2020	2	U	N	4	f	f	1	f
506	2019	6	M	Y	5	t	f	1	f
509	2019	7	F	U	1	f	f	1	f
511	2020	5	M	Y	6	f	f	1	f
513	2019	1	U	Y	1	f	f	1	f
514	2020	1	F	N	3	f	f	1	f
515	2020	3	M	U	7	f	f	1	f
516	2020	5	M	U	1	f	f	1	f
517	2019	7	U	Y	3	f	f	1	f
520	2020	2	M	Y	1	f	f	1	f
521	2018	6	M	U	7	t	f	1	f
522	2020	5	F	N	3	f	f	1	f
524	2020	6	M	U	2	f	f	1	f
528	2019	3	U	N	5	f	f	1	f
530	2020	2	F	U	6	f	f	1	f
531	2019	2	M	Y	4	t	f	1	f
541	2019	7	U	U	3	f	f	1	f
542	2020	3	M	N	3	f	f	1	f
545	2019	2	M	N	7	t	t	1	f
546	2018	5	M	Y	7	t	f	1	f
547	2019	1	U	N	7	f	f	1	f
548	2018	5	U	Y	5	f	f	1	f
549	2019	2	F	Y	6	f	t	1	f
552	2020	4	U	U	6	f	f	1	f
559	2019	2	F	U	1	f	f	1	f
560	2019	1	U	U	7	t	f	1	f
563	2018	2	F	U	7	t	f	1	f
565	2019	4	U	U	5	t	f	1	f
566	2019	7	M	N	7	t	f	1	f
568	2018	3	U	U	4	f	f	1	f
574	2019	2	F	U	4	f	f	1	f
578	2019	3	F	Y	5	f	f	1	f
579	2020	7	F	U	4	f	f	1	f
581	2019	1	M	Y	4	f	f	1	f
588	2018	2	U	Y	1	f	f	1	f
591	2018	4	U	U	2	t	t	1	f
598	2019	4	M	Y	3	f	f	1	f
599	2018	6	U	U	5	f	f	1	f
605	2019	5	U	N	1	f	f	1	f
608	2018	2	U	U	7	f	f	1	f
614	2019	3	F	Y	5	f	f	1	f
615	2020	7	F	N	1	f	f	1	f
616	2020	3	M	N	4	f	f	1	f
621	2019	3	U	Y	5	f	f	1	f
630	2019	4	M	U	4	t	f	1	f
631	2019	7	M	Y	7	f	f	1	f
632	2020	6	U	N	2	f	f	1	f
633	2018	5	F	U	4	f	t	1	f
636	2019	2	F	Y	2	t	f	1	f
637	2018	4	M	Y	6	f	f	1	f
641	2020	2	F	Y	5	f	f	1	f
643	2018	1	M	Y	4	f	f	1	f
644	2020	5	F	U	3	f	f	1	f
654	2019	3	F	N	4	f	f	1	f
662	2020	3	M	U	3	t	f	1	f
665	2019	1	U	N	6	t	f	1	f
669	2020	2	M	U	1	f	f	1	f
673	2018	4	U	U	3	f	f	1	f
677	2018	3	F	U	1	t	f	1	f
678	2019	2	F	U	7	f	f	1	f
681	2018	7	F	N	1	f	f	1	f
683	2020	6	U	N	1	f	f	1	f
687	2019	4	U	U	2	f	f	1	f
690	2020	3	F	Y	7	f	f	1	f
695	2020	3	M	N	4	f	f	1	f
696	2018	7	U	N	1	f	f	1	f
697	2018	7	U	U	2	f	f	1	f
699	2019	5	U	N	7	t	t	1	f
708	2018	5	M	U	3	f	f	1	f
710	2019	5	U	N	7	f	f	1	f
714	2018	1	M	U	2	f	f	1	f
719	2018	5	U	Y	7	f	f	1	f
723	2020	3	M	N	1	f	f	1	f
724	2018	6	U	U	1	f	t	1	f
730	2020	2	M	Y	7	t	f	1	f
731	2018	5	M	U	5	f	f	1	f
734	2020	3	F	U	2	f	f	1	f
736	2019	7	U	Y	2	f	f	1	f
747	2020	5	U	N	5	t	f	1	f
748	2019	2	F	U	6	f	t	1	f
753	2018	1	F	U	3	f	f	1	f
759	2018	1	U	U	6	t	f	1	f
765	2018	7	F	U	4	f	f	1	f
766	2020	7	F	U	2	t	f	1	f
769	2019	6	F	Y	2	t	f	1	f
771	2019	2	U	U	3	f	f	1	f
773	2018	6	F	N	1	f	f	1	f
775	2020	5	F	U	4	f	f	1	f
776	2020	3	F	N	2	t	f	1	f
784	2018	6	F	N	2	f	f	1	f
785	2020	6	U	U	6	f	f	1	f
786	2020	3	M	U	6	f	t	1	f
787	2020	7	M	N	7	f	f	1	f
788	2019	6	M	N	1	f	f	1	f
791	2020	3	U	Y	6	f	f	1	f
792	2018	3	U	U	2	t	f	1	f
794	2018	7	M	N	3	t	t	1	f
795	2018	4	F	N	6	t	f	1	f
796	2019	7	U	U	1	f	t	1	f
797	2020	3	F	N	2	f	f	1	f
798	2018	1	M	N	3	t	f	1	f
799	2020	7	U	N	6	t	f	1	f
800	2020	5	U	N	3	f	f	1	f
803	2018	3	U	Y	7	t	f	1	f
807	2018	1	F	U	1	f	f	1	f
809	2020	7	U	U	6	f	f	1	f
814	2020	1	M	Y	7	f	f	1	f
818	2020	4	U	N	7	f	f	1	f
821	2019	4	M	Y	1	f	f	1	f
822	2020	5	F	N	4	f	f	1	f
825	2019	2	F	Y	4	t	f	1	f
829	2020	6	F	U	3	f	f	1	f
832	2018	5	F	Y	2	f	f	1	f
835	2018	1	U	U	6	f	f	1	f
836	2019	5	F	Y	5	f	f	1	f
838	2019	7	M	U	7	f	f	1	f
841	2019	7	U	U	7	f	t	1	f
842	2020	7	F	N	6	f	f	1	f
846	2018	1	F	Y	2	f	f	1	f
849	2018	5	U	N	6	f	f	1	f
856	2018	5	U	U	6	f	t	1	f
857	2019	2	F	U	1	f	f	1	f
858	2020	2	U	N	3	f	f	1	f
862	2020	3	F	Y	4	f	f	1	f
871	2019	2	M	N	5	f	f	1	f
872	2018	1	M	N	3	t	f	1	f
875	2020	6	U	Y	6	t	f	1	f
881	2019	1	F	U	4	t	f	1	f
883	2020	4	M	Y	4	t	f	1	f
884	2020	4	M	N	3	f	f	1	f
886	2020	5	U	Y	4	f	f	1	f
887	2019	3	F	N	3	f	f	1	f
891	2019	7	U	N	3	f	f	1	f
892	2020	4	M	U	3	f	f	1	f
898	2019	4	U	Y	4	f	f	1	f
900	2020	6	F	N	1	t	f	1	f
901	2018	6	F	Y	1	t	f	1	f
902	2018	6	M	Y	1	f	t	1	f
908	2020	1	M	N	2	t	f	1	f
910	2020	5	F	U	6	f	f	1	f
911	2020	1	M	U	2	f	f	1	f
913	2019	2	U	Y	1	f	f	1	f
915	2019	2	U	U	4	f	f	1	f
917	2018	5	M	Y	7	f	f	1	f
923	2020	1	F	Y	4	f	f	1	f
925	2019	7	M	N	7	f	f	1	f
926	2018	5	M	Y	4	t	f	1	f
927	2019	2	M	Y	6	f	t	1	f
930	2020	1	F	U	4	t	f	1	f
931	2020	2	U	Y	6	f	f	1	f
933	2018	2	M	Y	4	f	f	1	f
934	2018	6	U	Y	5	t	f	1	f
939	2019	1	M	U	3	t	f	1	f
942	2020	4	M	N	2	f	f	1	f
943	2018	3	F	U	6	t	f	1	f
956	2020	4	M	Y	1	f	f	1	f
968	2020	4	M	U	2	t	f	1	f
971	2019	5	M	U	7	t	f	1	f
977	2019	3	U	U	7	t	f	1	f
978	2018	2	F	Y	1	f	f	1	f
979	2020	2	U	N	4	f	t	1	f
980	2020	6	M	U	4	f	f	1	f
983	2018	1	M	Y	3	t	f	1	f
985	2019	2	M	U	7	t	f	1	f
993	2020	6	U	Y	3	f	f	1	f
995	2020	3	U	N	1	f	t	1	f
997	2020	4	F	N	4	f	f	1	f
998	2018	2	U	Y	1	f	t	1	f
1000	2020	1	F	Y	2	t	f	1	f
1002	2018	7	U	U	1	f	f	1	f
1003	2019	7	U	N	6	t	f	1	f
1004	2020	6	U	N	6	t	f	1	f
1005	2020	3	M	U	6	f	f	1	f
1006	2018	5	F	U	1	f	f	1	f
1008	2020	1	U	Y	7	f	f	1	f
1009	2019	1	U	U	6	f	f	1	f
1010	2019	3	M	Y	4	f	t	1	f
1011	2019	3	U	N	7	f	f	1	f
1013	2020	6	M	Y	7	f	f	1	f
1014	2018	7	U	U	4	f	f	1	f
1016	2020	4	M	Y	3	f	f	1	f
1017	2018	7	M	N	4	f	f	1	f
1018	2018	1	F	Y	4	f	f	1	f
1019	2018	5	M	Y	2	f	t	1	f
1021	2020	3	U	N	7	f	f	1	f
1023	2020	7	M	Y	1	f	f	1	f
1024	2019	7	F	Y	7	t	f	1	f
1026	2020	3	M	U	1	f	f	1	f
1027	2020	6	M	N	1	f	f	1	f
1029	2020	4	U	U	7	t	t	1	f
1031	2018	1	F	U	3	f	f	1	f
1032	2018	4	U	Y	4	f	f	1	f
1033	2018	5	M	U	5	f	f	1	f
1034	2020	4	M	Y	3	f	f	1	f
1035	2019	4	U	N	2	f	f	1	f
1036	2018	1	U	N	6	t	f	1	f
1037	2018	4	U	Y	1	f	f	1	f
1039	2020	1	U	N	6	f	f	1	f
1041	2018	3	M	Y	3	t	f	1	f
1043	2018	3	F	Y	4	f	f	1	f
1044	2020	6	M	Y	2	f	f	1	f
1045	2020	4	M	N	7	t	f	1	f
1046	2020	4	F	Y	2	f	f	1	f
1050	2020	5	M	N	1	f	f	1	f
1051	2019	7	M	N	1	f	f	1	f
1052	2019	4	U	Y	4	f	f	1	f
1053	2020	6	F	N	3	t	f	1	f
1054	2019	5	F	N	4	f	f	1	f
1056	2020	3	M	U	1	f	f	1	f
1057	2018	4	F	N	7	f	f	1	f
1058	2019	7	F	Y	4	f	f	1	f
1059	2020	5	F	U	2	f	f	1	f
1060	2020	4	U	Y	5	t	f	1	f
1062	2018	4	U	Y	6	t	f	1	f
1063	2018	5	M	Y	6	f	f	1	f
1065	2019	5	F	U	3	f	f	1	f
1069	2020	7	U	U	7	f	f	1	f
1071	2019	5	U	U	5	f	f	1	f
1074	2020	5	F	U	3	f	f	1	f
1076	2019	2	F	U	5	f	f	1	f
1077	2019	6	U	N	5	t	f	1	f
1078	2019	4	U	U	2	f	f	1	f
1079	2018	4	M	U	5	f	f	1	f
1081	2020	6	U	Y	1	f	f	1	f
1082	2019	2	F	Y	5	t	f	1	f
1083	2020	7	U	N	7	t	t	1	f
1084	2018	2	M	U	3	f	f	1	f
1085	2018	4	M	U	5	f	f	1	f
1086	2019	2	M	Y	7	f	t	1	f
1087	2020	1	M	U	7	f	f	1	f
1089	2020	2	M	N	7	t	f	1	f
1090	2020	7	M	N	4	t	f	1	f
1091	2020	3	F	Y	4	f	f	1	f
1092	2019	2	M	U	5	f	f	1	f
1093	2018	2	M	N	7	f	f	1	f
1094	2019	6	U	N	1	f	f	1	f
1095	2019	1	F	N	1	t	f	1	f
1096	2019	1	F	Y	4	f	f	1	f
1097	2018	4	U	N	3	f	f	1	f
1098	2019	7	F	Y	7	f	t	1	f
1099	2019	1	U	N	6	f	f	1	f
1100	2018	1	U	Y	7	f	f	1	f
1101	2020	1	U	Y	3	f	f	1	f
1102	2018	1	U	U	3	f	f	1	f
1103	2018	3	M	N	5	t	f	1	f
1104	2018	2	M	Y	2	f	f	1	f
1105	2020	4	M	Y	5	t	f	1	f
1107	2020	3	U	Y	3	f	f	1	f
1109	2018	7	M	N	1	f	f	1	f
1110	2018	5	U	U	5	t	f	1	f
1111	2020	4	U	Y	5	t	f	1	f
1112	2018	5	U	Y	2	f	f	1	f
1114	2018	4	F	Y	7	f	f	1	f
1115	2019	5	F	U	6	f	f	1	f
1119	2019	5	U	N	2	t	f	1	f
1120	2018	3	F	N	5	f	f	1	f
1121	2019	5	F	N	1	t	f	1	f
1122	2019	2	M	N	2	f	f	1	f
1123	2019	1	F	Y	6	f	f	1	f
1125	2019	1	M	N	5	t	f	1	f
1126	2020	3	F	U	3	f	f	1	f
1127	2018	4	M	Y	4	f	f	1	f
1128	2019	5	U	U	2	t	f	1	f
1129	2018	3	F	Y	4	f	f	1	f
1131	2020	7	F	N	6	f	f	1	f
1132	2018	5	M	N	6	f	f	1	f
1133	2020	3	M	U	3	f	f	1	f
1135	2019	3	M	Y	4	t	f	1	f
1136	2020	7	M	Y	1	t	f	1	f
1137	2019	7	U	N	1	f	f	1	f
1138	2018	5	U	Y	3	t	f	1	f
1140	2019	1	M	U	6	f	t	1	f
1141	2019	5	F	N	6	f	f	1	f
1142	2018	1	M	Y	7	f	f	1	f
1143	2018	5	M	Y	1	f	f	1	f
1144	2018	4	U	Y	3	f	f	1	f
1145	2018	6	F	U	3	f	t	1	f
1146	2020	3	M	Y	5	f	f	1	f
1147	2019	2	F	U	7	t	f	1	f
1148	2020	5	F	Y	2	f	f	1	f
1149	2019	5	U	N	2	t	f	1	f
1150	2018	5	M	Y	3	t	f	1	f
1151	2019	5	M	N	7	f	t	1	f
1155	2018	4	U	Y	6	f	f	1	f
1156	2018	1	F	U	5	t	f	1	f
1157	2020	4	U	U	6	f	f	1	f
1158	2019	6	U	N	4	t	f	1	f
1159	2019	4	M	N	1	f	f	1	f
1161	2020	3	U	U	4	f	f	1	f
1162	2019	4	M	Y	5	t	f	1	f
1163	2018	6	F	U	2	t	f	1	f
1165	2018	1	M	U	1	f	f	1	f
1167	2018	7	U	N	5	f	f	1	f
1169	2018	5	F	N	2	f	f	1	f
1171	2018	4	F	U	1	f	f	1	f
1172	2020	3	M	U	7	t	f	1	f
1173	2020	4	U	U	7	f	f	1	f
1177	2020	5	M	Y	1	t	t	1	f
1180	2018	6	M	U	1	f	f	1	f
1181	2019	2	F	N	7	f	f	1	f
1182	2019	7	F	Y	2	f	f	1	f
1183	2020	3	U	N	1	f	f	1	f
1184	2019	2	M	Y	4	f	f	1	f
1185	2020	7	M	N	3	t	f	1	f
1186	2018	3	M	U	4	f	f	1	f
1187	2018	2	M	U	6	f	f	1	f
1189	2019	1	F	U	5	f	f	1	f
1191	2020	6	F	U	4	f	f	1	f
1193	2018	7	U	N	7	f	f	1	f
1195	2018	7	M	U	4	f	t	1	f
1196	2018	4	F	U	3	f	t	1	f
1197	2020	5	F	U	7	t	f	1	f
1199	2018	5	M	Y	5	f	f	1	f
1200	2020	5	F	N	7	t	f	1	f
1201	2018	7	U	Y	5	f	f	1	f
1203	2020	4	M	Y	3	t	f	1	f
1204	2020	1	U	N	3	f	f	1	f
1205	2019	1	U	U	4	f	f	1	f
1207	2019	1	F	Y	6	f	f	1	f
1208	2019	2	U	U	5	f	f	1	f
1209	2018	7	M	N	7	f	f	1	f
1210	2019	3	M	N	6	f	f	1	f
1212	2020	3	F	N	2	f	f	1	f
1214	2019	6	U	U	6	f	f	1	f
1215	2018	4	U	N	7	t	f	1	f
1218	2018	7	U	U	4	f	f	1	f
1219	2019	5	U	Y	1	f	f	1	f
1220	2018	4	U	Y	5	f	f	1	f
1221	2020	3	M	N	4	f	t	1	f
1222	2019	5	M	U	5	f	t	1	f
1223	2018	7	M	Y	5	f	f	1	f
1226	2018	7	U	N	1	f	f	1	f
1228	2018	5	F	Y	5	f	f	1	f
1231	2018	7	U	U	6	t	f	1	f
1236	2019	1	F	Y	5	f	f	1	f
1237	2019	1	U	N	5	t	t	1	f
1238	2019	2	U	U	5	t	f	1	f
1239	2018	6	U	Y	5	f	f	1	f
1240	2020	7	M	N	1	t	f	1	f
1241	2019	6	M	Y	7	f	f	1	f
1242	2019	6	M	N	1	f	f	1	f
1243	2020	1	F	N	7	f	f	1	f
1244	2020	3	F	Y	6	t	t	1	f
1245	2020	5	U	U	1	f	f	1	f
1246	2019	7	F	U	2	f	f	1	f
1247	2018	3	M	Y	3	f	f	1	f
1248	2018	7	M	N	6	f	f	1	f
1249	2020	6	M	U	6	t	f	1	f
1250	2020	4	F	U	3	f	f	1	f
1252	2019	4	U	U	6	f	f	1	f
1254	2019	2	F	Y	6	t	f	1	f
1255	2020	5	M	U	2	f	f	1	f
1256	2020	5	U	U	1	f	f	1	f
1257	2018	3	F	U	5	f	f	1	f
1258	2019	3	F	Y	4	t	f	1	f
1259	2020	1	M	N	6	f	t	1	f
1260	2018	2	F	Y	6	t	f	1	f
1261	2020	2	M	Y	3	t	f	1	f
1262	2018	6	F	U	2	f	f	1	f
1265	2019	7	M	U	5	f	f	1	f
1266	2019	7	M	Y	4	f	f	1	f
1267	2018	6	F	N	1	f	f	1	f
1268	2018	3	U	U	2	f	f	1	f
1270	2018	1	M	N	4	f	f	1	f
1271	2019	1	M	N	7	f	f	1	f
1273	2020	2	M	N	5	f	f	1	f
1274	2020	7	U	U	6	f	t	1	f
1277	2020	4	M	N	5	f	f	1	f
1278	2019	5	U	Y	7	f	f	1	f
1279	2020	5	F	U	1	t	f	1	f
1281	2020	7	M	N	7	f	f	1	f
1282	2018	6	F	U	1	f	f	1	f
1284	2020	1	F	U	7	f	f	1	f
1285	2019	6	U	N	3	t	t	1	f
1286	2020	1	M	N	5	f	f	1	f
1287	2019	3	U	N	6	t	f	1	f
1288	2020	6	F	U	1	f	f	1	f
1290	2019	6	U	N	5	f	f	1	f
1291	2018	3	F	U	5	f	f	1	f
1293	2019	2	F	U	2	f	t	1	f
1294	2019	5	F	N	5	f	f	1	f
1296	2019	7	M	Y	1	t	f	1	f
1297	2020	7	U	Y	5	f	f	1	f
1298	2020	1	U	Y	5	f	f	1	f
1299	2019	5	M	Y	5	t	f	1	f
1300	2018	4	F	N	2	t	f	1	f
1301	2018	7	M	U	5	t	f	1	f
1302	2020	3	M	U	1	t	f	1	f
1303	2018	5	M	N	5	f	f	1	f
1304	2020	4	M	N	3	f	f	1	f
1305	2019	4	U	N	4	f	f	1	f
1306	2019	4	M	U	6	f	f	1	f
1307	2018	7	U	N	2	f	f	1	f
1308	2018	6	F	N	6	f	f	1	f
1309	2020	5	U	U	3	f	f	1	f
1310	2018	4	F	Y	4	f	f	1	f
1312	2018	6	U	Y	5	f	f	1	f
1316	2019	1	M	U	2	f	f	1	f
1317	2019	2	M	U	2	f	f	1	f
1318	2020	5	U	U	3	t	f	1	f
1319	2019	4	M	Y	4	t	t	1	f
1320	2020	7	U	U	1	f	f	1	f
1321	2019	4	M	Y	1	t	f	1	f
1322	2020	7	U	Y	5	f	f	1	f
1323	2020	2	M	N	3	t	f	1	f
1324	2018	5	F	U	6	f	f	1	f
1325	2018	7	M	Y	4	f	f	1	f
1326	2020	1	U	N	7	f	f	1	f
1327	2019	5	M	U	3	f	f	1	f
1328	2020	1	U	Y	3	f	f	1	f
1330	2018	2	F	N	2	f	t	1	f
1331	2020	5	M	U	2	f	f	1	f
1332	2020	3	M	N	7	f	f	1	f
1333	2019	1	U	N	6	f	f	1	f
1334	2020	5	M	U	5	t	f	1	f
1335	2018	3	M	U	2	f	f	1	f
1339	2018	4	F	Y	1	f	f	1	f
1340	2020	3	F	N	4	f	f	1	f
1341	2018	5	F	N	5	f	f	1	f
1342	2020	3	U	Y	5	t	f	1	f
1343	2018	6	U	Y	6	f	f	1	f
1344	2020	1	F	U	5	f	t	1	f
1345	2020	1	M	U	1	f	f	1	f
1346	2020	3	F	Y	2	f	t	1	f
1347	2018	2	U	Y	6	t	t	1	f
1348	2018	7	M	U	6	f	f	1	f
1349	2019	1	F	U	5	f	f	1	f
1350	2018	2	M	N	4	f	f	1	f
1351	2019	5	M	N	6	t	f	1	f
1352	2019	6	M	N	3	f	f	1	f
1353	2020	1	U	Y	7	f	f	1	f
1357	2019	3	U	U	3	t	f	1	f
1359	2019	5	F	U	3	f	f	1	f
1361	2019	5	M	Y	2	f	f	1	f
1362	2019	7	M	N	2	t	t	1	f
1363	2020	2	F	U	2	f	f	1	f
1365	2018	3	M	Y	3	f	t	1	f
1369	2019	2	U	Y	5	f	t	1	f
1370	2019	2	F	U	7	f	f	1	f
1371	2020	1	M	N	7	t	f	1	f
1374	2020	1	U	N	2	t	f	1	f
1375	2019	1	M	U	6	f	f	1	f
1376	2019	6	U	U	7	f	f	1	f
1377	2019	1	U	U	7	f	t	1	f
1378	2018	1	U	Y	3	f	f	1	f
1380	2020	7	F	U	5	f	t	1	f
1381	2020	1	U	N	7	f	f	1	f
1382	2019	2	F	U	4	f	f	1	f
1383	2018	2	U	N	7	f	f	1	f
1385	2019	7	M	N	3	f	f	1	f
1386	2019	7	F	Y	1	t	f	1	f
1387	2018	7	F	Y	5	f	f	1	f
1389	2019	4	M	Y	7	f	f	1	f
1391	2020	1	F	Y	4	f	f	1	f
1392	2020	4	M	N	7	f	f	1	f
1393	2019	4	F	Y	3	f	f	1	f
1394	2019	4	M	N	3	t	f	1	f
1395	2020	2	F	Y	2	f	f	1	f
1396	2019	2	M	Y	1	f	f	1	f
1397	2020	6	U	U	6	t	f	1	f
1398	2018	1	M	Y	4	t	f	1	f
1400	2020	6	U	Y	2	f	f	1	f
1403	2020	1	U	N	2	t	f	1	f
1406	2020	1	M	N	2	t	f	1	f
1407	2020	3	F	Y	3	f	f	1	f
1408	2020	3	U	Y	1	t	f	1	f
1409	2019	3	U	Y	1	f	f	1	f
1410	2018	5	F	Y	4	t	t	1	f
1411	2020	1	M	U	1	f	f	1	f
1413	2019	2	F	U	3	f	f	1	f
1415	2019	6	F	Y	6	f	t	1	f
1416	2018	4	F	U	7	f	f	1	f
1417	2018	6	F	Y	5	f	f	1	f
1419	2020	5	F	N	1	f	f	1	f
1420	2020	7	U	Y	6	f	f	1	f
1421	2020	7	U	U	1	f	f	1	f
1422	2019	6	M	U	7	t	f	1	f
1423	2018	2	U	Y	3	f	f	1	f
1424	2020	3	M	Y	2	f	f	1	f
1425	2020	5	F	N	6	f	f	1	f
1426	2019	2	U	U	2	f	f	1	f
1427	2020	7	F	Y	3	f	t	1	f
1430	2018	5	M	Y	1	f	f	1	f
1431	2019	6	M	U	6	t	f	1	f
1432	2020	7	F	Y	1	t	f	1	f
1433	2018	5	U	N	7	t	f	1	f
1434	2019	5	U	Y	4	f	f	1	f
1436	2020	3	M	U	2	t	f	1	f
1437	2020	6	M	U	5	f	f	1	f
1440	2020	4	U	U	5	f	f	1	f
1441	2020	3	F	Y	4	t	f	1	f
1442	2018	4	U	U	1	f	f	1	f
1443	2020	5	F	Y	2	f	f	1	f
1444	2018	7	M	Y	5	f	f	1	f
1445	2019	4	F	N	4	t	f	1	f
1446	2019	2	F	Y	5	f	f	1	f
1448	2019	1	U	U	7	t	f	1	f
1451	2018	3	U	N	5	f	f	1	f
1452	2019	7	M	N	4	f	f	1	f
1454	2019	3	M	N	6	f	f	1	f
1455	2019	5	U	N	6	t	f	1	f
1456	2019	2	F	N	4	f	f	1	f
1457	2020	5	F	N	5	f	f	1	f
1458	2019	4	M	N	4	f	f	1	f
1459	2019	1	M	N	3	f	f	1	f
1461	2020	4	M	U	3	f	f	1	f
1463	2020	2	F	Y	2	f	f	1	f
1465	2019	7	U	U	2	f	t	1	f
1466	2020	7	M	U	5	f	f	1	f
1467	2019	4	U	Y	2	f	f	1	f
1469	2019	1	F	N	3	t	f	1	f
1470	2018	1	U	U	1	t	f	1	f
1471	2018	4	M	Y	5	f	f	1	f
1474	2019	4	M	U	2	f	f	1	f
1475	2019	2	U	N	6	f	f	1	f
1476	2018	1	U	U	2	t	f	1	f
1478	2019	5	U	U	5	t	f	1	f
1480	2020	7	U	N	2	f	f	1	f
1481	2019	6	F	N	3	f	f	1	f
1482	2019	5	M	U	3	f	f	1	f
1483	2020	2	F	U	6	f	f	1	f
1484	2020	4	M	Y	5	t	f	1	f
1485	2020	4	M	U	1	t	f	1	f
1486	2018	4	M	N	7	f	f	1	f
1487	2018	2	M	U	4	f	f	1	f
1488	2019	5	F	U	1	f	f	1	f
1490	2019	1	U	Y	1	f	f	1	f
1492	2019	6	M	U	7	f	f	1	f
1493	2020	1	M	Y	5	f	f	1	f
1494	2020	1	U	N	3	f	f	1	f
1495	2019	5	M	Y	4	t	f	1	f
1497	2020	2	M	U	7	f	f	1	f
1498	2020	1	M	Y	3	f	f	1	f
1499	2020	3	F	N	6	f	f	1	f
1500	2020	6	M	Y	4	f	f	1	f
1501	2019	7	F	N	3	f	f	3	f
1502	2020	7	F	U	7	f	f	3	f
1503	2019	2	M	U	3	f	f	3	f
1504	2018	7	M	N	6	f	f	3	f
1505	2020	1	U	Y	7	t	t	3	f
1506	2020	2	F	Y	7	f	f	3	f
1507	2019	6	M	N	6	f	f	3	f
1508	2019	3	F	U	6	t	f	3	f
1509	2018	5	F	Y	2	f	f	3	f
1510	2020	4	U	U	2	f	f	3	f
1511	2018	3	F	Y	7	t	t	3	f
1512	2018	1	F	U	6	t	f	3	f
1513	2018	4	M	N	5	f	f	3	f
1514	2020	3	F	Y	2	f	f	3	f
1515	2018	5	U	Y	2	f	f	3	f
1516	2019	6	U	U	2	t	t	3	f
1517	2019	2	U	U	1	t	f	3	f
1518	2019	7	M	U	2	f	f	3	f
1519	2020	5	M	Y	4	t	f	3	f
1520	2020	7	U	N	7	t	f	3	f
1521	2019	3	U	N	5	t	f	3	f
1522	2019	5	F	N	1	f	t	3	f
1523	2020	6	M	Y	4	f	f	3	f
1524	2018	4	F	U	7	t	f	3	f
1525	2020	1	M	U	1	t	t	3	f
1526	2019	3	U	Y	5	f	t	3	f
1527	2018	7	U	Y	1	t	t	3	f
1528	2020	3	M	Y	5	f	f	3	f
1529	2019	5	M	U	3	f	f	3	f
1530	2020	1	M	U	7	t	f	3	f
1531	2020	6	M	U	7	f	f	3	f
1532	2020	4	U	Y	6	t	f	3	f
1533	2018	7	M	Y	6	t	f	3	f
1534	2020	1	U	U	2	f	f	3	f
1535	2019	1	M	U	6	f	f	3	f
1536	2020	4	M	U	1	t	f	3	f
1537	2019	7	M	N	7	f	f	3	f
1538	2018	4	F	N	1	f	t	3	f
1539	2019	6	M	Y	2	f	f	3	f
1540	2019	2	U	U	4	t	f	3	f
1541	2019	2	U	Y	2	f	f	3	f
1542	2020	4	U	U	7	f	f	3	f
1543	2019	1	U	N	6	f	f	3	f
1544	2019	7	F	Y	5	f	f	3	f
1545	2018	4	U	U	2	f	f	3	f
1546	2019	4	U	Y	5	f	f	3	f
1547	2018	2	F	U	7	t	f	3	f
1548	2019	4	F	Y	5	f	f	3	f
1549	2020	2	M	N	1	t	f	3	f
1550	2020	6	U	Y	5	f	f	3	f
1551	2018	2	U	Y	7	f	f	3	f
1552	2020	3	U	U	4	f	f	3	f
1553	2018	6	M	N	3	f	f	3	f
1554	2018	3	U	U	7	f	f	3	f
1555	2018	7	U	Y	2	f	t	3	f
1556	2019	5	M	N	6	f	f	3	f
1557	2020	1	U	Y	3	f	t	3	f
1558	2018	5	U	Y	7	t	f	3	f
1559	2020	2	F	U	3	f	f	3	f
1560	2018	2	M	Y	5	f	f	3	f
1561	2018	7	F	N	5	f	t	3	f
1562	2020	1	F	N	5	t	f	3	f
1563	2019	2	M	U	7	t	t	3	f
1564	2020	7	U	N	4	t	t	3	f
1565	2020	3	F	Y	5	f	f	3	f
1566	2018	2	U	Y	7	f	f	3	f
1567	2019	7	U	N	7	f	f	3	f
1568	2020	3	F	U	4	f	f	3	f
1569	2020	7	M	N	2	f	f	3	f
1570	2019	2	U	N	5	f	f	3	f
1571	2020	1	F	N	6	f	f	3	f
1572	2018	5	F	Y	1	f	f	3	f
1573	2019	4	U	U	2	t	f	3	f
1574	2019	3	F	U	3	t	f	3	f
1575	2018	4	F	U	7	f	f	3	f
1576	2020	3	U	Y	4	t	f	3	f
1577	2018	3	M	N	2	f	f	3	f
1578	2020	1	U	Y	6	f	f	3	f
1579	2020	7	F	U	4	t	f	3	f
1580	2018	6	M	U	2	f	f	3	f
1581	2019	5	M	U	2	f	f	3	f
1582	2019	7	F	U	3	f	f	3	f
1583	2020	7	U	Y	6	f	f	3	f
1584	2018	3	M	Y	5	f	f	3	f
1585	2020	4	M	Y	3	t	t	3	f
1586	2018	5	U	Y	7	f	f	3	f
1587	2018	1	M	N	4	f	f	3	f
1588	2020	1	U	U	7	f	t	3	f
1589	2019	4	F	Y	7	f	f	3	f
1590	2020	2	U	U	3	f	f	3	f
1591	2018	6	U	Y	6	f	f	3	f
1592	2018	3	M	Y	4	f	f	3	f
1593	2019	5	M	Y	1	f	t	3	f
1594	2018	1	M	Y	5	f	t	3	f
1595	2020	5	F	U	4	f	f	3	f
1596	2019	3	F	U	1	f	f	3	f
1597	2020	2	M	N	2	f	f	3	f
1598	2020	3	F	U	4	f	f	3	f
1599	2020	7	U	U	2	t	f	3	f
1600	2018	5	F	U	2	f	f	3	f
1601	2020	4	M	U	7	f	f	3	f
1602	2018	2	M	N	5	f	f	3	f
1603	2018	5	U	U	4	t	f	3	f
1604	2020	7	M	Y	1	f	f	3	f
1605	2020	5	U	Y	6	f	f	3	f
1606	2018	4	M	N	2	f	t	3	f
1607	2019	3	M	Y	6	f	t	3	f
1608	2019	4	U	U	5	t	f	3	f
1609	2020	4	F	Y	1	f	f	3	f
1610	2018	7	U	U	4	t	f	3	f
1611	2018	5	F	Y	2	f	f	3	f
1612	2019	5	M	N	7	f	t	3	f
1613	2018	4	M	U	1	f	f	3	f
1614	2019	5	M	U	4	f	f	3	f
1615	2020	6	F	N	4	f	t	3	f
1616	2019	4	U	Y	7	f	f	3	f
1617	2018	6	U	Y	1	f	f	3	f
1618	2019	4	F	Y	6	f	f	3	f
1619	2019	2	F	N	7	f	f	3	f
1620	2019	2	U	N	4	f	f	3	f
1621	2019	6	F	N	7	t	f	3	f
1622	2019	5	U	Y	2	f	f	3	f
1623	2020	7	M	Y	1	f	f	3	f
1624	2019	1	F	N	6	t	f	3	f
1625	2020	1	M	N	7	f	t	3	f
1626	2019	5	U	U	4	f	f	3	f
1627	2019	6	F	Y	3	f	f	3	f
1628	2020	6	F	Y	4	f	f	3	f
1629	2018	3	F	N	3	f	f	3	f
1630	2018	4	M	U	1	f	f	3	f
1631	2019	5	M	U	6	f	f	3	f
1632	2020	2	U	N	5	f	t	3	f
1633	2018	7	F	Y	1	f	f	3	f
1634	2020	2	U	U	3	f	f	3	f
1635	2018	7	M	N	3	f	f	3	f
1636	2019	6	M	U	7	t	f	3	f
1637	2019	6	M	U	2	f	f	3	f
1638	2019	5	M	Y	7	f	f	3	f
1639	2018	6	F	U	4	t	f	3	f
1640	2018	3	F	N	4	f	f	3	f
1641	2020	7	U	Y	6	f	f	3	f
1642	2020	7	F	U	2	f	f	3	f
1643	2019	6	U	N	3	t	t	3	f
1644	2018	1	M	Y	2	t	f	3	f
1645	2018	5	U	Y	7	f	t	3	f
1646	2020	5	M	U	3	f	f	3	f
1647	2020	5	U	N	3	f	f	3	f
1648	2018	7	U	N	5	f	f	3	f
1649	2020	5	F	Y	6	f	f	3	f
1650	2018	4	M	U	2	f	f	3	f
1651	2020	6	F	N	4	f	f	3	f
1652	2018	2	M	Y	7	f	t	3	f
1653	2019	6	F	U	5	f	f	3	f
1654	2018	7	M	N	3	f	f	3	f
1655	2019	6	M	U	5	f	f	3	f
1656	2019	3	F	U	1	f	f	3	f
1657	2019	5	M	Y	7	t	f	3	f
1658	2019	7	M	Y	3	f	f	3	f
1659	2019	7	M	U	2	f	f	3	f
1660	2020	2	M	Y	6	f	t	3	f
1661	2019	4	U	Y	4	f	f	3	f
1662	2019	1	U	Y	4	t	f	3	f
1663	2020	4	M	U	3	t	t	3	f
1664	2018	5	U	Y	2	f	t	3	f
1665	2019	5	F	U	6	f	f	3	f
1666	2018	7	U	U	2	t	f	3	f
1667	2019	1	M	N	1	t	f	3	f
1668	2020	4	M	U	3	f	f	3	f
1669	2018	5	F	U	4	f	f	3	f
1670	2018	5	U	U	4	f	f	3	f
1671	2020	2	F	N	6	f	f	3	f
1672	2018	1	M	Y	3	f	f	3	f
1673	2020	6	M	Y	7	f	f	3	f
1674	2019	1	F	N	7	f	f	3	f
1675	2020	3	U	N	7	t	f	3	f
1676	2020	6	M	Y	7	f	f	3	f
1677	2018	2	U	U	6	f	f	3	f
1678	2018	6	U	U	7	f	f	3	f
1679	2019	1	F	N	1	t	f	3	f
1680	2020	1	M	N	3	t	f	3	f
1681	2018	1	U	N	7	f	f	3	f
1682	2020	7	U	Y	4	t	f	3	f
1683	2020	4	M	N	3	f	f	3	f
1684	2020	7	F	Y	1	f	f	3	f
1685	2018	4	F	Y	7	t	f	3	f
1686	2019	3	F	U	4	f	f	3	f
1687	2019	3	F	U	4	t	f	3	f
1688	2019	7	M	Y	3	f	f	3	f
1689	2018	1	U	Y	3	t	f	3	f
1690	2020	2	U	Y	6	f	f	3	f
1691	2018	2	F	N	4	t	f	3	f
1692	2018	2	F	U	5	f	f	3	f
1693	2019	5	F	U	4	f	f	3	f
1694	2018	5	F	N	6	t	f	3	f
1695	2019	3	M	N	3	t	f	3	f
1696	2019	2	F	N	1	t	f	3	f
1697	2018	3	U	U	2	f	f	3	f
1698	2018	4	M	Y	4	f	f	3	f
1699	2020	2	F	Y	1	f	f	3	f
1700	2019	7	F	N	7	t	f	3	f
1701	2019	3	F	N	7	f	f	3	f
1702	2019	2	F	U	3	f	f	3	f
1703	2020	7	F	U	4	t	f	3	f
1704	2019	1	M	N	1	t	t	3	f
1705	2019	2	F	N	6	f	f	3	f
1706	2020	5	U	Y	4	f	f	3	f
1707	2018	5	U	U	7	f	f	3	f
1708	2019	4	U	Y	2	f	f	3	f
1709	2020	7	F	U	4	f	f	3	f
1710	2019	4	F	U	2	t	f	3	f
1711	2020	5	U	U	7	t	t	3	f
1712	2019	2	U	N	5	f	f	3	f
1713	2019	1	M	U	1	f	f	3	f
1714	2020	1	F	U	3	f	f	3	f
1715	2019	1	U	N	7	f	f	3	f
1716	2018	4	F	Y	7	f	f	3	f
1717	2020	6	U	N	2	t	f	3	f
1718	2020	1	F	U	7	f	f	3	f
1719	2018	5	M	U	4	t	f	3	f
1720	2020	1	F	U	3	f	f	3	f
1721	2019	2	U	U	1	f	f	3	f
1722	2018	2	F	U	3	f	f	3	f
1723	2019	5	F	Y	4	f	t	3	f
1724	2019	1	M	U	3	f	f	3	f
1725	2018	7	U	U	2	f	f	3	f
1726	2020	5	U	Y	7	f	f	3	f
1727	2019	6	U	Y	7	t	f	3	f
1728	2019	1	M	N	6	f	f	3	f
1729	2020	5	M	U	7	t	f	3	f
1730	2019	2	U	N	4	f	f	3	f
1731	2018	5	U	Y	2	f	f	3	f
1732	2019	5	U	N	5	t	f	3	f
1733	2020	2	F	N	6	t	t	3	f
1734	2019	6	U	U	1	f	f	3	f
1735	2018	1	F	Y	1	t	t	3	f
1736	2018	3	F	Y	1	f	f	3	f
1737	2018	1	F	U	4	f	f	3	f
1738	2018	4	F	U	4	f	f	3	f
1739	2018	4	F	U	1	t	f	3	f
1740	2018	3	U	Y	1	f	f	3	f
1741	2020	2	F	U	7	t	f	3	f
1742	2018	7	M	N	3	t	f	3	f
1743	2020	2	M	U	7	f	f	3	f
1744	2018	4	U	N	1	f	f	3	f
1745	2019	2	U	U	5	t	f	3	f
1746	2020	2	M	U	2	f	f	3	f
1747	2020	5	U	Y	5	f	f	3	f
1748	2019	3	M	U	3	f	f	3	f
1749	2020	4	F	N	1	f	f	3	f
1750	2020	6	F	U	7	f	f	3	f
1751	2018	3	M	N	1	f	t	3	f
1752	2020	1	U	U	2	t	f	3	f
1753	2020	6	U	N	5	f	f	3	f
1754	2019	7	M	Y	5	f	f	3	f
1755	2018	1	M	U	7	f	f	3	f
1756	2020	7	F	U	5	f	t	3	f
1757	2018	4	U	N	2	f	f	3	f
1758	2018	7	F	U	4	t	t	3	f
1759	2019	7	U	U	3	f	f	3	f
1760	2020	7	U	N	5	f	f	3	f
1761	2018	5	M	Y	1	t	f	3	f
1762	2019	2	M	U	7	f	f	3	f
1763	2019	5	F	N	2	f	f	3	f
1764	2020	4	F	U	5	f	f	3	f
1765	2020	6	F	U	7	f	f	3	f
1766	2020	1	F	Y	1	f	f	3	f
1767	2018	1	F	Y	4	t	f	3	f
1768	2019	3	U	Y	5	t	f	3	f
1769	2020	1	U	U	2	f	t	3	f
1770	2020	1	M	N	5	f	f	3	f
1771	2020	2	M	U	7	f	f	3	f
1772	2020	6	U	N	4	f	f	3	f
1773	2020	5	M	U	5	f	f	3	f
1774	2018	3	F	Y	6	t	f	3	f
1775	2018	3	U	U	3	t	f	3	f
1776	2019	3	U	U	1	t	t	3	f
1777	2019	6	U	Y	4	f	f	3	f
1778	2018	7	M	U	3	f	f	3	f
1779	2020	5	M	U	2	f	f	3	f
1780	2018	1	F	U	2	f	f	3	f
1781	2020	7	M	N	7	t	f	3	f
1782	2019	6	F	Y	5	f	f	3	f
1783	2019	3	U	Y	2	t	f	3	f
1784	2020	1	F	U	2	f	f	3	f
1785	2018	2	M	Y	1	t	f	3	f
1786	2020	3	M	N	6	f	f	3	f
1787	2020	5	U	N	5	t	f	3	f
1788	2020	7	U	Y	2	t	f	3	f
1789	2019	1	M	N	6	t	f	3	f
1790	2019	3	U	N	1	f	f	3	f
1791	2020	7	M	Y	3	t	f	3	f
1792	2018	3	F	N	5	f	f	3	f
1793	2020	6	F	U	4	f	f	3	f
1794	2018	7	F	N	4	f	t	3	f
1795	2020	7	F	Y	5	f	f	3	f
1796	2019	5	M	N	5	f	f	3	f
1797	2019	4	M	U	5	f	f	3	f
1798	2019	4	U	N	5	f	f	3	f
1799	2019	1	U	N	1	f	f	3	f
1800	2018	6	U	Y	7	f	f	3	f
1801	2020	7	F	N	4	f	f	3	f
1802	2019	2	M	U	1	f	f	3	f
1803	2018	4	F	N	7	f	t	3	f
1804	2018	5	M	N	6	f	f	3	f
1805	2018	1	M	Y	2	f	f	3	f
1806	2020	1	U	U	7	f	f	3	f
1807	2020	2	M	U	6	t	f	3	f
1808	2020	1	M	U	5	f	f	3	f
1809	2018	4	M	Y	2	t	f	3	f
1810	2019	3	M	Y	4	f	f	3	f
1811	2020	1	M	Y	6	f	f	3	f
1812	2018	4	U	Y	1	f	f	3	f
1813	2019	7	U	N	4	t	f	3	f
1814	2018	1	F	U	2	f	f	3	f
1815	2020	5	U	N	6	f	f	3	f
1816	2018	7	U	U	2	f	f	3	f
1817	2020	5	F	N	6	t	f	3	f
1818	2018	1	U	Y	3	f	f	3	f
1819	2020	5	M	U	6	f	f	3	f
1820	2020	5	F	N	1	f	f	3	f
1821	2019	1	U	N	6	f	f	3	f
1822	2019	6	U	Y	7	f	f	3	f
1823	2019	7	U	Y	2	f	f	3	f
1824	2019	4	F	Y	6	f	f	3	f
1825	2018	6	U	Y	5	t	f	3	f
1826	2020	3	M	U	4	t	f	3	f
1827	2020	4	M	Y	6	f	f	3	f
1828	2019	6	M	U	2	f	f	3	f
1829	2019	1	U	N	4	t	f	3	f
1830	2018	5	U	U	4	f	t	3	f
1831	2018	4	U	N	3	f	f	3	f
1832	2018	3	F	U	4	f	f	3	f
1833	2019	5	F	N	2	f	f	3	f
1834	2020	1	M	Y	5	t	f	3	f
1835	2019	2	U	U	4	t	f	3	f
1836	2020	3	M	U	1	f	f	3	f
1837	2019	7	F	Y	7	f	f	3	f
1838	2018	4	U	N	2	t	f	3	f
1839	2019	3	F	N	2	f	f	3	f
1840	2019	4	U	Y	7	t	f	3	f
1841	2020	5	F	U	7	f	f	3	f
1842	2018	5	F	Y	1	f	f	3	f
1843	2019	7	U	N	1	f	f	3	f
1844	2018	3	U	U	2	f	f	3	f
1845	2019	2	U	U	6	f	f	3	f
1846	2019	1	U	N	4	f	f	3	f
1847	2018	1	M	Y	1	f	f	3	f
1848	2020	2	F	N	2	f	f	3	f
1849	2019	1	M	N	5	t	f	3	f
1850	2019	7	F	Y	1	f	f	3	f
1851	2018	1	M	Y	2	f	f	3	f
1852	2019	7	M	Y	2	t	f	3	f
1853	2019	3	F	Y	6	f	f	3	f
1854	2018	2	M	N	7	t	f	3	f
1855	2020	4	M	Y	1	f	f	3	f
1856	2018	4	U	Y	7	f	f	3	f
1857	2018	4	F	Y	4	t	f	3	f
1858	2019	1	F	N	2	f	f	3	f
1859	2020	6	U	U	7	f	f	3	f
1860	2019	2	F	N	5	t	f	3	f
1861	2018	4	U	N	5	t	f	3	f
1862	2019	6	F	N	4	f	f	3	f
1863	2020	2	M	Y	1	f	f	3	f
1864	2019	6	U	U	5	f	f	3	f
1865	2018	6	U	Y	3	t	f	3	f
1866	2020	2	U	N	6	f	f	3	f
1867	2019	3	M	Y	7	f	f	3	f
1868	2020	1	U	N	3	f	f	3	f
1869	2020	6	U	N	7	f	f	3	f
1870	2020	2	M	U	5	t	f	3	f
1871	2018	6	F	U	4	f	t	3	f
1872	2020	7	U	Y	3	f	f	3	f
1873	2020	7	U	Y	3	f	f	3	f
1874	2020	5	U	Y	3	f	f	3	f
1875	2019	1	U	N	1	t	f	3	f
1876	2018	5	U	Y	3	t	f	3	f
1877	2018	7	U	Y	4	f	f	3	f
1878	2020	2	M	U	4	f	f	3	f
1879	2020	6	F	N	4	f	f	3	f
1880	2019	1	M	U	2	f	f	3	f
1881	2018	4	F	Y	2	t	f	3	f
1882	2019	6	F	N	3	f	f	3	f
1883	2019	4	M	N	6	f	f	3	f
1884	2019	5	M	Y	5	f	f	3	f
1885	2020	1	M	Y	3	t	f	3	f
1886	2020	5	F	N	4	t	f	3	f
1887	2018	2	U	U	3	t	f	3	f
1888	2019	1	M	U	7	f	f	3	f
1889	2018	2	F	Y	7	f	f	3	f
1890	2019	3	F	Y	6	f	f	3	f
1891	2020	7	U	Y	4	f	f	3	f
1892	2018	6	M	U	7	f	f	3	f
1893	2018	3	U	U	4	f	f	3	f
1894	2018	1	F	U	3	f	f	3	f
1895	2020	2	U	U	1	f	f	3	f
1896	2019	1	M	U	4	t	f	3	f
1897	2020	7	M	N	3	f	f	3	f
1898	2020	1	U	U	1	f	f	3	f
1899	2018	2	U	U	5	f	f	3	f
1900	2018	4	F	U	2	t	f	3	f
1901	2018	1	U	N	2	f	f	3	f
1902	2018	1	M	Y	6	f	f	3	f
1903	2019	2	F	N	7	t	f	3	f
1904	2020	5	F	Y	2	f	f	3	f
1905	2019	7	F	Y	5	t	f	3	f
1906	2019	1	U	N	1	t	f	3	f
1907	2019	2	U	Y	5	f	f	3	f
1908	2018	5	F	U	5	f	f	3	f
1909	2018	2	F	N	1	f	f	3	f
1910	2020	6	M	Y	7	f	f	3	f
1911	2018	6	M	N	6	t	f	3	f
1912	2019	5	F	N	6	f	f	3	f
1913	2019	1	M	Y	4	f	f	3	f
1914	2018	5	F	U	7	f	f	3	f
1915	2019	2	U	U	2	f	f	3	f
1916	2019	7	M	Y	4	t	f	3	f
1917	2020	7	M	N	1	f	t	3	f
1918	2020	5	U	U	3	f	f	3	f
1919	2020	4	F	N	2	f	f	3	f
1920	2020	1	F	N	3	f	f	3	f
1921	2018	5	M	U	2	f	f	3	f
1922	2018	2	M	N	1	f	f	3	f
1923	2018	1	F	Y	5	f	f	3	f
1924	2019	1	M	U	2	f	f	3	f
1925	2020	2	U	U	7	f	f	3	f
1926	2020	4	U	U	2	f	f	3	f
1927	2019	2	U	Y	4	t	f	3	f
1928	2020	3	F	Y	2	t	f	3	f
1929	2019	1	U	Y	6	t	f	3	f
1930	2019	6	M	N	5	t	f	3	f
1931	2019	7	M	N	2	f	f	3	f
1932	2020	4	M	U	2	f	f	3	f
1933	2020	6	U	U	5	f	f	3	f
1934	2018	3	U	U	6	f	f	3	f
1935	2020	3	U	U	7	f	f	3	f
1936	2020	3	M	U	1	f	f	3	f
1937	2018	1	M	Y	6	f	f	3	f
1938	2019	4	F	Y	6	f	f	3	f
1939	2020	4	F	Y	1	f	f	3	f
1940	2018	1	M	Y	5	f	f	3	f
1941	2018	1	U	N	4	t	f	3	f
1942	2020	2	F	U	1	f	f	3	f
1943	2018	6	M	N	6	t	f	3	f
1944	2020	7	F	U	7	f	f	3	f
1945	2020	4	F	N	3	t	f	3	f
1946	2020	4	F	N	6	f	f	3	f
1947	2020	2	M	N	6	t	f	3	f
1948	2018	2	M	N	2	f	f	3	f
1949	2019	1	U	U	4	f	f	3	f
1950	2018	6	F	U	4	t	f	3	f
1951	2020	7	M	Y	7	t	f	3	f
1952	2020	7	M	N	6	f	f	3	f
1953	2018	2	M	Y	4	f	f	3	f
1954	2019	2	U	Y	4	f	t	3	f
1955	2020	6	F	U	2	t	f	3	f
1956	2018	6	F	N	5	f	f	3	f
1957	2019	1	F	U	5	f	f	3	f
1958	2020	1	F	Y	3	f	t	3	f
1959	2018	7	F	U	6	f	f	3	f
1960	2020	5	M	U	3	t	f	3	f
1961	2019	5	M	Y	7	f	f	3	f
1962	2020	6	F	N	5	f	t	3	f
1963	2018	2	F	N	1	f	f	3	f
1964	2019	2	M	Y	2	t	f	3	f
1965	2018	1	F	Y	7	t	f	3	f
1966	2019	1	U	Y	7	f	f	3	f
1967	2019	2	M	Y	6	f	f	3	f
1968	2020	2	F	U	2	t	f	3	f
1969	2018	6	F	N	4	f	f	3	f
1970	2018	2	F	U	3	f	f	3	f
1971	2018	1	M	N	2	f	f	3	f
1972	2020	1	U	N	5	t	f	3	f
1973	2019	2	U	U	7	f	f	3	f
1974	2019	2	U	Y	5	f	f	3	f
1975	2018	5	M	U	1	f	f	3	f
1976	2018	6	U	Y	3	t	f	3	f
1977	2018	4	U	N	7	f	f	3	f
1978	2020	7	F	N	2	f	f	3	f
1979	2018	6	F	U	6	f	t	3	f
1980	2018	4	F	Y	5	f	f	3	f
1981	2018	5	U	Y	6	f	f	3	f
1982	2020	4	F	U	2	f	f	3	f
1983	2018	3	U	N	3	f	f	3	f
1984	2018	6	F	N	7	f	f	3	f
1985	2018	2	U	U	2	f	f	3	f
1986	2018	1	F	N	1	t	f	3	f
1987	2018	4	M	U	1	t	t	3	f
1988	2018	2	M	U	1	f	f	3	f
1989	2020	7	U	U	5	f	t	3	f
1990	2020	1	U	U	7	f	f	3	f
1991	2018	1	M	U	7	f	t	3	f
1992	2018	1	U	N	2	f	f	3	f
1993	2020	3	U	N	3	f	f	3	f
1994	2020	2	U	Y	2	f	f	3	f
1995	2019	3	M	N	2	f	f	3	f
1996	2019	5	U	Y	5	t	f	3	f
1997	2018	1	F	Y	4	f	f	3	f
1998	2020	6	U	N	1	f	f	3	f
1999	2018	2	U	N	1	t	f	3	f
2000	2018	6	M	U	1	t	f	3	f
2001	2018	2	U	Y	5	f	f	3	f
2002	2019	3	F	N	4	f	f	3	f
2003	2020	7	U	U	3	f	f	3	f
2004	2019	1	U	Y	1	f	f	3	f
2005	2018	7	M	N	2	t	f	3	f
2006	2020	3	F	Y	3	f	f	3	f
2007	2019	3	U	N	1	f	f	3	f
2008	2019	4	M	Y	6	f	f	3	f
2009	2018	3	F	Y	4	f	f	3	f
2010	2020	4	F	Y	7	f	t	3	f
2011	2019	7	F	Y	7	f	f	3	f
2012	2019	2	F	U	3	f	f	3	f
2013	2019	4	F	N	3	f	f	3	f
2014	2019	7	U	Y	3	t	f	3	f
2015	2018	3	M	N	5	t	f	3	f
2016	2018	4	F	U	6	f	f	3	f
2017	2019	7	U	U	4	t	f	3	f
2018	2019	3	F	N	7	f	f	3	f
2019	2019	6	M	Y	6	f	f	3	f
2020	2020	7	U	Y	7	t	f	3	f
2021	2019	5	M	N	6	t	f	3	f
2022	2019	6	U	Y	2	f	f	3	f
2023	2019	5	U	N	4	f	f	3	f
2024	2018	1	M	U	4	f	f	3	f
2025	2020	5	F	U	1	f	f	3	f
2026	2018	1	U	Y	4	f	f	3	f
2027	2018	4	M	Y	1	f	f	3	f
2028	2018	2	M	Y	1	f	f	3	f
2029	2019	4	M	U	2	f	f	3	f
0	2020	7	F	Y	1	f	f	2	t
0	2020	7	F	Y	1	f	f	1	t
1	2019	4	F	Y	3	f	f	1	t
1	2019	4	F	Y	3	f	f	3	t
3	2020	6	U	Y	6	f	t	1	t
3	2020	6	U	Y	6	f	t	2	t
4	2019	5	F	U	6	f	t	2	t
4	2019	5	F	U	6	f	f	1	t
4	2019	5	F	U	6	f	f	3	t
5	2018	3	U	Y	4	t	f	1	t
5	2018	3	U	Y	4	t	f	2	t
6	2020	6	U	U	6	f	f	1	t
6	2020	6	U	U	6	f	t	2	t
8	2018	2	U	U	7	f	t	2	t
8	2018	2	U	U	7	f	f	1	t
10	2019	4	F	N	6	t	f	1	t
10	2019	4	F	N	6	t	t	3	t
13	2018	6	M	N	2	f	f	2	t
13	2018	6	M	N	2	f	f	1	t
14	2018	3	U	Y	5	t	f	1	t
14	2018	3	U	Y	5	t	f	2	t
18	2019	7	F	N	6	f	f	1	t
18	2019	7	F	N	6	f	t	2	t
19	2020	2	U	Y	2	f	f	1	t
19	2020	2	U	Y	2	f	f	2	t
20	2018	4	U	N	1	f	f	1	t
20	2018	4	U	N	1	f	f	3	t
20	2018	4	U	N	1	f	t	2	t
21	2018	3	M	Y	4	f	f	2	t
21	2018	3	M	Y	4	f	f	1	t
22	2020	1	F	Y	6	f	f	1	t
22	2020	1	F	Y	6	f	t	2	t
23	2019	2	M	N	7	f	f	1	t
23	2019	2	M	N	7	f	f	2	t
24	2018	3	U	N	5	f	f	1	t
24	2018	3	U	N	5	f	t	2	t
26	2018	6	U	U	5	f	t	2	t
26	2018	6	U	U	5	f	f	1	t
27	2020	5	F	N	3	f	t	3	t
27	2020	5	F	N	3	f	f	1	t
29	2020	3	M	N	5	f	t	2	t
29	2020	3	M	N	5	f	f	1	t
30	2020	6	M	Y	3	f	t	3	t
30	2020	6	M	Y	3	f	t	1	t
31	2018	2	F	N	4	f	f	2	t
31	2018	2	F	N	4	f	f	1	t
32	2019	6	F	Y	3	f	f	1	t
32	2019	6	F	Y	3	f	t	3	t
33	2019	3	U	Y	7	t	f	2	t
33	2019	3	U	Y	7	t	f	1	t
34	2019	2	U	N	4	f	t	3	t
34	2019	2	U	N	4	f	f	1	t
36	2018	2	M	Y	5	t	f	2	t
36	2018	2	M	Y	5	t	t	1	t
38	2020	6	M	N	6	f	f	3	t
38	2020	6	M	N	6	f	f	1	t
39	2018	3	F	U	2	t	f	2	t
39	2018	3	F	U	2	t	f	1	t
40	2018	5	M	Y	6	f	t	2	t
40	2018	5	M	Y	6	f	f	1	t
40	2018	5	M	Y	6	f	t	3	t
41	2018	2	F	N	4	f	f	1	t
41	2018	2	F	N	4	f	f	3	t
44	2019	4	F	Y	1	f	f	2	t
44	2019	4	F	Y	1	f	f	1	t
45	2020	1	U	N	3	f	f	3	t
45	2020	1	U	N	3	f	t	2	t
45	2020	1	U	N	3	f	t	1	t
49	2019	2	F	U	1	f	f	1	t
49	2019	2	F	U	1	f	t	2	t
51	2019	4	F	N	6	f	f	3	t
51	2019	4	F	N	6	f	f	2	t
51	2019	4	F	N	6	f	f	1	t
53	2019	7	M	Y	7	f	f	1	t
53	2019	7	M	Y	7	f	t	2	t
54	2018	4	F	Y	6	t	t	2	t
54	2018	4	F	Y	6	t	f	1	t
54	2018	4	F	Y	6	t	t	3	t
56	2020	2	U	N	1	f	f	1	t
56	2020	2	U	N	1	f	t	2	t
57	2019	4	F	Y	2	f	f	1	t
57	2019	4	F	Y	2	f	f	2	t
58	2020	4	U	N	3	f	t	2	t
58	2020	4	U	N	3	f	f	3	t
58	2020	4	U	N	3	f	f	1	t
59	2020	2	M	N	6	t	f	1	t
59	2020	2	M	N	6	t	f	2	t
60	2018	1	U	Y	6	f	f	1	t
60	2018	1	U	Y	6	f	f	2	t
63	2019	3	F	U	7	f	f	1	t
63	2019	3	F	U	7	f	f	3	t
64	2020	4	F	Y	6	f	f	3	t
64	2020	4	F	Y	6	f	f	1	t
67	2018	2	F	Y	5	f	f	3	t
67	2018	2	F	Y	5	f	f	1	t
68	2018	3	M	Y	4	f	f	3	t
68	2018	3	M	Y	4	f	t	2	t
68	2018	3	M	Y	4	f	f	1	t
69	2018	4	U	U	6	t	f	1	t
69	2018	4	U	U	6	t	t	2	t
69	2018	4	U	U	6	t	f	3	t
70	2018	2	M	Y	5	t	f	2	t
70	2018	2	M	Y	5	t	f	1	t
71	2018	3	F	U	5	f	f	1	t
71	2018	3	F	U	5	f	f	2	t
71	2018	3	F	U	5	f	t	3	t
72	2020	1	U	U	7	f	t	1	t
72	2020	1	U	U	7	f	t	2	t
74	2018	6	U	Y	6	t	t	3	t
74	2018	6	U	Y	6	t	f	1	t
76	2020	3	F	U	1	f	f	1	t
76	2020	3	F	U	1	f	f	2	t
77	2020	4	U	N	1	f	f	2	t
77	2020	4	U	N	1	f	f	1	t
77	2020	4	U	N	1	f	f	3	t
80	2018	4	F	U	4	t	t	3	t
80	2018	4	F	U	4	t	f	1	t
81	2020	3	F	U	6	t	t	2	t
81	2020	3	F	U	6	t	t	3	t
81	2020	3	F	U	6	t	f	1	t
83	2019	4	U	U	3	t	f	1	t
83	2019	4	U	U	3	t	t	3	t
84	2018	6	M	N	4	f	t	3	t
84	2018	6	M	N	4	f	t	1	t
85	2019	7	F	Y	2	t	t	3	t
85	2019	7	F	Y	2	t	f	1	t
86	2020	5	M	Y	7	t	f	3	t
86	2020	5	M	Y	7	t	f	1	t
86	2020	5	M	Y	7	t	f	2	t
87	2019	7	F	U	5	f	f	2	t
87	2019	7	F	U	5	f	f	1	t
87	2019	7	F	U	5	f	f	3	t
88	2018	7	M	U	3	f	f	2	t
88	2018	7	M	U	3	f	f	3	t
88	2018	7	M	U	3	f	f	1	t
89	2018	6	U	Y	2	f	f	2	t
89	2018	6	U	Y	2	f	f	1	t
91	2018	5	U	Y	2	f	t	2	t
91	2018	5	U	Y	2	f	f	1	t
91	2018	5	U	Y	2	f	t	3	t
95	2019	4	M	N	6	t	f	3	t
95	2019	4	M	N	6	t	f	2	t
95	2019	4	M	N	6	t	f	1	t
96	2018	1	U	U	4	f	t	1	t
96	2018	1	U	U	4	f	t	2	t
97	2020	4	M	U	3	f	f	1	t
97	2020	4	M	U	3	f	f	3	t
99	2020	7	F	Y	1	f	f	1	t
99	2020	7	F	Y	1	f	f	2	t
100	2018	5	F	U	3	t	t	2	t
100	2018	5	F	U	3	t	f	1	t
100	2018	5	F	U	3	t	f	3	t
101	2020	6	U	U	2	f	f	1	t
101	2020	6	U	U	2	f	t	3	t
102	2018	5	U	N	5	f	f	2	t
102	2018	5	U	N	5	f	f	1	t
102	2018	5	U	N	5	f	t	3	t
103	2018	1	M	U	4	t	t	2	t
103	2018	1	M	U	4	t	f	1	t
103	2018	1	M	U	4	t	f	3	t
106	2019	2	F	U	3	f	f	2	t
106	2019	2	F	U	3	f	t	3	t
106	2019	2	F	U	3	f	f	1	t
107	2019	4	M	Y	4	f	f	1	t
107	2019	4	M	Y	4	f	f	3	t
107	2019	4	M	Y	4	f	f	2	t
108	2018	3	F	U	5	f	f	2	t
108	2018	3	F	U	5	f	f	1	t
109	2018	2	U	Y	4	f	f	1	t
109	2018	2	U	Y	4	f	t	3	t
109	2018	2	U	Y	4	f	f	2	t
110	2020	4	U	N	1	f	t	3	t
110	2020	4	U	N	1	f	f	1	t
112	2019	7	M	U	4	f	f	1	t
112	2019	7	M	U	4	f	f	2	t
113	2018	3	U	U	3	f	f	1	t
113	2018	3	U	U	3	f	t	2	t
115	2019	2	F	Y	1	f	t	2	t
115	2019	2	F	Y	1	f	f	1	t
115	2019	2	F	Y	1	f	t	3	t
116	2019	1	F	N	5	f	f	3	t
116	2019	1	F	N	5	f	f	1	t
117	2018	5	M	Y	4	f	f	1	t
117	2018	5	M	Y	4	f	t	2	t
117	2018	5	M	Y	4	f	f	3	t
118	2018	5	F	Y	4	f	f	2	t
118	2018	5	F	Y	4	f	f	1	t
120	2020	7	M	Y	3	t	t	2	t
120	2020	7	M	Y	3	t	f	1	t
121	2020	3	M	N	2	f	f	1	t
121	2020	3	M	N	2	f	f	2	t
122	2020	3	U	Y	5	f	f	1	t
122	2020	3	U	Y	5	f	t	2	t
123	2020	2	M	Y	7	f	t	3	t
123	2020	2	M	Y	7	f	f	1	t
124	2020	1	F	U	5	f	f	1	t
124	2020	1	F	U	5	f	f	3	t
125	2018	5	F	U	2	f	f	3	t
125	2018	5	F	U	2	f	f	1	t
126	2018	2	F	U	2	f	t	2	t
126	2018	2	F	U	2	f	f	1	t
127	2019	2	U	U	1	f	f	1	t
127	2019	2	U	U	1	f	t	3	t
128	2020	3	M	U	5	f	f	1	t
128	2020	3	M	U	5	f	t	2	t
129	2018	4	M	N	7	f	t	2	t
129	2018	4	M	N	7	f	f	1	t
130	2019	1	U	Y	7	f	f	2	t
130	2019	1	U	Y	7	f	f	1	t
131	2020	5	F	N	5	f	t	2	t
131	2020	5	F	N	5	f	f	1	t
134	2018	3	F	Y	6	t	f	3	t
134	2018	3	F	Y	6	t	f	1	t
135	2020	1	F	N	5	f	f	3	t
135	2020	1	F	N	5	f	t	2	t
135	2020	1	F	N	5	f	f	1	t
136	2019	6	M	U	5	f	f	1	t
136	2019	6	M	U	5	f	t	2	t
138	2019	2	F	N	5	f	t	2	t
138	2019	2	F	N	5	f	f	1	t
139	2020	2	F	U	7	t	f	2	t
139	2020	2	F	U	7	t	f	1	t
139	2020	2	F	U	7	t	f	3	t
140	2020	2	M	U	3	f	f	1	t
140	2020	2	M	U	3	f	t	2	t
142	2018	1	M	U	1	f	t	2	t
142	2018	1	M	U	1	f	t	1	t
146	2020	2	M	Y	5	f	f	3	t
146	2020	2	M	Y	5	f	f	2	t
146	2020	2	M	Y	5	f	f	1	t
148	2020	7	M	N	7	f	t	3	t
148	2020	7	M	N	7	f	f	1	t
148	2020	7	M	N	7	f	f	2	t
149	2020	4	U	N	5	f	f	1	t
149	2020	4	U	N	5	f	t	2	t
150	2019	3	M	Y	5	t	f	1	t
150	2019	3	M	Y	5	t	f	2	t
151	2020	4	M	Y	6	t	f	1	t
151	2020	4	M	Y	6	t	f	3	t
153	2018	3	M	U	5	t	f	3	t
153	2018	3	M	U	5	t	f	1	t
153	2018	3	M	U	5	t	f	2	t
155	2018	2	F	N	2	t	f	2	t
155	2018	2	F	N	2	t	f	1	t
156	2019	4	F	U	7	f	t	3	t
156	2019	4	F	U	7	f	f	1	t
156	2019	4	F	U	7	f	f	2	t
157	2018	3	M	N	5	f	f	2	t
157	2018	3	M	N	5	f	f	1	t
159	2020	2	F	U	5	f	f	1	t
159	2020	2	F	U	5	f	f	2	t
161	2019	3	U	U	1	f	t	2	t
161	2019	3	U	U	1	f	f	1	t
162	2020	7	U	U	3	f	f	2	t
162	2020	7	U	U	3	f	f	1	t
162	2020	7	U	U	3	f	f	3	t
163	2018	4	F	U	5	f	t	1	t
163	2018	4	F	U	5	f	t	2	t
164	2018	7	M	N	5	t	f	3	t
164	2018	7	M	N	5	t	f	2	t
164	2018	7	M	N	5	t	f	1	t
165	2019	2	M	N	2	f	t	2	t
165	2019	2	M	N	2	f	f	1	t
166	2018	6	U	Y	5	f	t	1	t
166	2018	6	U	Y	5	f	t	3	t
166	2018	6	U	Y	5	f	f	2	t
170	2019	4	F	N	5	t	f	1	t
170	2019	4	F	N	5	t	t	2	t
170	2019	4	F	N	5	t	f	3	t
171	2018	2	F	U	2	f	f	1	t
171	2018	2	F	U	2	f	t	3	t
172	2020	7	U	Y	5	f	f	2	t
172	2020	7	U	Y	5	f	f	1	t
173	2020	2	M	Y	7	t	f	1	t
173	2020	2	M	Y	7	t	f	2	t
174	2018	5	F	U	7	f	f	2	t
174	2018	5	F	U	7	f	f	1	t
175	2019	6	U	U	5	f	t	1	t
175	2019	6	U	U	5	f	f	2	t
176	2019	4	M	N	7	f	f	1	t
176	2019	4	M	N	7	f	f	2	t
177	2018	4	U	N	1	t	f	1	t
177	2018	4	U	N	1	t	f	2	t
178	2019	4	U	N	3	f	f	1	t
178	2019	4	U	N	3	f	f	2	t
179	2018	4	M	Y	3	f	t	2	t
179	2018	4	M	Y	3	f	f	3	t
179	2018	4	M	Y	3	f	f	1	t
181	2020	1	U	Y	6	t	f	1	t
181	2020	1	U	Y	6	t	f	2	t
182	2018	4	M	N	5	f	f	3	t
182	2018	4	M	N	5	f	f	1	t
183	2020	2	F	U	5	t	t	2	t
183	2020	2	F	U	5	t	f	1	t
184	2020	7	F	Y	3	t	f	3	t
184	2020	7	F	Y	3	t	f	1	t
188	2018	1	M	N	2	f	f	3	t
188	2018	1	M	N	2	f	f	1	t
188	2018	1	M	N	2	f	f	2	t
190	2019	4	M	N	5	f	f	1	t
190	2019	4	M	N	5	f	f	3	t
193	2019	1	U	N	1	f	f	1	t
193	2019	1	U	N	1	f	t	3	t
194	2018	3	F	U	5	t	t	2	t
194	2018	3	F	U	5	t	f	1	t
195	2020	7	U	Y	1	f	f	1	t
195	2020	7	U	Y	1	f	t	2	t
195	2020	7	U	Y	1	f	t	3	t
197	2018	6	F	U	1	f	f	3	t
197	2018	6	F	U	1	f	f	1	t
198	2018	7	U	N	5	f	f	3	t
198	2018	7	U	N	5	f	f	1	t
199	2019	4	U	N	2	f	f	2	t
199	2019	4	U	N	2	f	t	3	t
199	2019	4	U	N	2	f	f	1	t
201	2018	2	U	Y	3	f	f	1	t
201	2018	2	U	Y	3	f	f	2	t
202	2019	2	U	N	1	f	t	3	t
202	2019	2	U	N	1	f	f	1	t
203	2018	3	M	Y	3	f	f	1	t
203	2018	3	M	Y	3	f	f	2	t
207	2020	2	M	U	2	t	f	1	t
207	2020	2	M	U	2	t	f	2	t
207	2020	2	M	U	2	t	f	3	t
208	2020	2	M	U	7	f	f	1	t
208	2020	2	M	U	7	f	t	3	t
210	2020	7	F	N	5	f	f	1	t
210	2020	7	F	N	5	f	f	3	t
211	2020	2	M	U	4	t	f	3	t
211	2020	2	M	U	4	t	f	2	t
211	2020	2	M	U	4	t	f	1	t
212	2020	2	F	U	7	f	f	1	t
212	2020	2	F	U	7	f	f	2	t
214	2019	1	M	N	3	f	t	2	t
214	2019	1	M	N	3	f	f	1	t
216	2018	2	U	N	6	f	f	2	t
216	2018	2	U	N	6	f	f	1	t
218	2018	5	F	N	5	f	f	2	t
218	2018	5	F	N	5	f	f	1	t
219	2019	4	F	Y	1	t	t	2	t
219	2019	4	F	Y	1	t	f	1	t
220	2018	4	F	U	3	f	t	3	t
220	2018	4	F	U	3	f	f	1	t
222	2019	5	U	Y	1	f	f	2	t
222	2019	5	U	Y	1	f	f	1	t
223	2019	2	M	U	1	t	t	2	t
223	2019	2	M	U	1	t	f	3	t
223	2019	2	M	U	1	t	f	1	t
224	2019	6	U	Y	7	f	f	1	t
224	2019	6	U	Y	7	f	t	2	t
225	2018	5	F	N	7	f	t	1	t
225	2018	5	F	N	7	f	f	2	t
225	2018	5	F	N	7	f	t	3	t
226	2020	2	F	N	2	f	t	3	t
226	2020	2	F	N	2	f	f	1	t
226	2020	2	F	N	2	f	f	2	t
229	2020	5	U	U	7	f	f	1	t
229	2020	5	U	U	7	f	f	3	t
231	2018	2	U	Y	3	f	f	3	t
231	2018	2	U	Y	3	f	f	1	t
234	2020	6	M	N	7	f	f	1	t
234	2020	6	M	N	7	f	f	3	t
234	2020	6	M	N	7	f	t	2	t
235	2020	4	M	Y	3	f	f	2	t
235	2020	4	M	Y	3	f	f	1	t
236	2018	2	M	N	4	f	f	2	t
236	2018	2	M	N	4	f	f	1	t
237	2019	1	M	U	7	f	f	1	t
237	2019	1	M	U	7	f	f	2	t
238	2019	1	F	N	3	f	f	2	t
238	2019	1	F	N	3	f	f	1	t
240	2019	2	M	U	2	f	f	1	t
240	2019	2	M	U	2	f	f	3	t
241	2018	4	M	Y	2	t	f	3	t
241	2018	4	M	Y	2	t	f	1	t
242	2018	6	F	Y	2	f	f	1	t
242	2018	6	F	Y	2	f	t	2	t
242	2018	6	F	Y	2	f	f	3	t
243	2020	5	M	Y	6	f	f	3	t
243	2020	5	M	Y	6	f	f	2	t
243	2020	5	M	Y	6	f	f	1	t
244	2019	6	U	Y	4	f	f	2	t
244	2019	6	U	Y	4	f	f	1	t
246	2019	2	M	U	4	f	f	3	t
246	2019	2	M	U	4	f	f	1	t
246	2019	2	M	U	4	f	t	2	t
247	2019	5	U	N	1	f	f	2	t
247	2019	5	U	N	1	f	f	1	t
248	2020	1	F	Y	7	f	t	1	t
248	2020	1	F	Y	7	f	f	3	t
248	2020	1	F	Y	7	f	f	2	t
249	2019	2	F	N	1	t	f	1	t
249	2019	2	F	N	1	t	f	3	t
251	2019	3	U	N	3	f	f	3	t
251	2019	3	U	N	3	f	f	1	t
252	2020	5	F	U	1	t	f	1	t
252	2020	5	F	U	1	t	f	2	t
253	2019	6	F	Y	5	f	f	3	t
253	2019	6	F	Y	5	f	t	2	t
253	2019	6	F	Y	5	f	f	1	t
256	2019	2	U	N	2	f	t	3	t
256	2019	2	U	N	2	f	f	1	t
256	2019	2	U	N	2	f	t	2	t
257	2020	5	U	N	2	f	f	1	t
257	2020	5	U	N	2	f	t	2	t
259	2020	6	F	Y	4	t	f	1	t
259	2020	6	F	Y	4	t	t	2	t
260	2018	5	F	U	2	f	f	3	t
260	2018	5	F	U	2	f	f	1	t
261	2018	5	U	U	6	f	f	2	t
261	2018	5	U	U	6	f	f	1	t
262	2020	5	F	N	6	t	f	3	t
262	2020	5	F	N	6	t	f	1	t
265	2020	7	M	Y	4	f	t	2	t
265	2020	7	M	Y	4	f	f	1	t
267	2020	2	U	U	4	f	f	1	t
267	2020	2	U	U	4	f	t	2	t
268	2019	6	U	U	6	f	f	2	t
268	2019	6	U	U	6	f	f	1	t
271	2020	7	M	N	7	f	f	2	t
271	2020	7	M	N	7	f	f	3	t
271	2020	7	M	N	7	f	f	1	t
272	2018	5	F	N	3	f	t	2	t
272	2018	5	F	N	3	f	t	3	t
272	2018	5	F	N	3	f	t	1	t
274	2020	6	F	U	7	f	t	2	t
274	2020	6	F	U	7	f	f	1	t
276	2018	6	F	N	7	f	f	1	t
276	2018	6	F	N	7	f	f	2	t
278	2019	3	M	U	3	f	f	2	t
278	2019	3	M	U	3	f	f	1	t
279	2020	1	M	Y	7	f	t	2	t
279	2020	1	M	Y	7	f	f	1	t
281	2020	5	U	U	2	f	f	1	t
281	2020	5	U	U	2	f	f	2	t
282	2019	7	M	U	5	f	t	2	t
282	2019	7	M	U	5	f	f	1	t
283	2019	1	F	N	2	f	f	1	t
283	2019	1	F	N	2	f	f	2	t
283	2019	1	F	N	2	f	f	3	t
284	2020	2	M	U	4	f	t	2	t
284	2020	2	M	U	4	f	f	1	t
285	2019	3	M	N	3	f	f	1	t
285	2019	3	M	N	3	f	f	2	t
286	2019	6	U	U	5	t	f	1	t
286	2019	6	U	U	5	t	t	2	t
287	2019	2	M	Y	5	t	t	1	t
287	2019	2	M	Y	5	t	t	2	t
288	2020	2	U	N	2	f	f	1	t
288	2020	2	U	N	2	f	t	2	t
288	2020	2	U	N	2	f	t	3	t
289	2019	5	F	U	2	f	f	1	t
289	2019	5	F	U	2	f	t	2	t
289	2019	5	F	U	2	f	f	3	t
291	2018	4	F	N	7	f	t	2	t
291	2018	4	F	N	7	f	f	1	t
293	2018	5	F	N	7	f	f	1	t
293	2018	5	F	N	7	f	f	3	t
296	2018	6	U	N	6	t	t	2	t
296	2018	6	U	N	6	t	f	1	t
302	2019	6	M	N	4	f	f	1	t
302	2019	6	M	N	4	f	f	3	t
303	2018	4	M	Y	5	f	t	2	t
303	2018	4	M	Y	5	f	f	1	t
304	2019	4	F	Y	7	f	t	1	t
304	2019	4	F	Y	7	f	f	2	t
304	2019	4	F	Y	7	f	t	3	t
307	2019	7	U	N	4	f	t	2	t
307	2019	7	U	N	4	f	f	1	t
308	2019	7	F	U	1	f	f	1	t
308	2019	7	F	U	1	f	t	2	t
308	2019	7	F	U	1	f	t	3	t
312	2019	4	U	U	3	f	t	2	t
312	2019	4	U	U	3	f	f	1	t
313	2018	2	M	N	6	f	t	2	t
313	2018	2	M	N	6	f	f	1	t
316	2019	1	F	N	3	f	t	2	t
316	2019	1	F	N	3	f	f	1	t
316	2019	1	F	N	3	f	f	3	t
317	2018	2	M	U	3	t	f	2	t
317	2018	2	M	U	3	t	f	1	t
318	2019	5	M	U	1	t	t	3	t
318	2019	5	M	U	1	t	f	1	t
319	2019	2	F	U	2	f	f	3	t
319	2019	2	F	U	2	f	f	1	t
320	2018	2	U	U	5	t	f	2	t
320	2018	2	U	U	5	t	f	1	t
321	2020	1	F	Y	6	t	f	1	t
321	2020	1	F	Y	6	t	f	3	t
322	2019	7	F	U	4	f	f	1	t
322	2019	7	F	U	4	f	f	2	t
323	2020	1	F	N	5	f	t	3	t
323	2020	1	F	N	5	f	f	1	t
323	2020	1	F	N	5	f	f	2	t
324	2019	6	F	N	6	f	f	1	t
324	2019	6	F	N	6	f	t	2	t
325	2020	1	U	U	4	f	f	2	t
325	2020	1	U	U	4	f	f	1	t
325	2020	1	U	U	4	f	t	3	t
326	2019	5	M	Y	1	f	f	1	t
326	2019	5	M	Y	1	f	t	2	t
329	2018	4	M	U	3	t	f	1	t
329	2018	4	M	U	3	t	f	2	t
332	2019	5	M	N	4	f	f	1	t
332	2019	5	M	N	4	f	t	2	t
333	2020	7	M	Y	4	f	f	1	t
333	2020	7	M	Y	4	f	f	3	t
333	2020	7	M	Y	4	f	f	2	t
334	2020	2	U	N	7	t	f	2	t
334	2020	2	U	N	7	t	f	1	t
335	2018	4	U	Y	6	f	f	3	t
335	2018	4	U	Y	6	f	f	1	t
336	2018	4	F	U	2	f	t	2	t
336	2018	4	F	U	2	f	f	1	t
336	2018	4	F	U	2	f	f	3	t
337	2018	5	F	Y	6	f	t	2	t
337	2018	5	F	Y	6	f	f	1	t
338	2018	7	M	Y	4	f	f	2	t
338	2018	7	M	Y	4	f	f	1	t
340	2018	7	M	N	6	t	f	1	t
340	2018	7	M	N	6	t	t	2	t
340	2018	7	M	N	6	t	f	3	t
341	2020	6	U	U	4	t	f	1	t
341	2020	6	U	U	4	t	f	2	t
342	2020	1	F	Y	6	f	f	3	t
342	2020	1	F	Y	6	f	t	2	t
342	2020	1	F	Y	6	f	f	1	t
343	2018	6	U	U	1	t	t	2	t
343	2018	6	U	U	1	t	f	1	t
346	2019	4	F	U	2	t	f	2	t
346	2019	4	F	U	2	t	t	1	t
350	2019	7	U	Y	7	f	f	1	t
350	2019	7	U	Y	7	f	t	2	t
351	2020	1	U	U	4	f	f	1	t
351	2020	1	U	U	4	f	f	2	t
352	2020	7	M	Y	3	f	t	2	t
352	2020	7	M	Y	3	f	f	3	t
352	2020	7	M	Y	3	f	f	1	t
353	2019	6	U	N	2	f	f	2	t
353	2019	6	U	N	2	f	f	1	t
354	2018	5	U	N	3	t	f	2	t
354	2018	5	U	N	3	t	f	1	t
355	2020	4	M	U	1	t	f	3	t
355	2020	4	M	U	1	t	f	1	t
357	2020	3	M	Y	1	f	f	3	t
357	2020	3	M	Y	1	f	f	1	t
359	2020	6	F	Y	5	f	f	1	t
359	2020	6	F	Y	5	f	t	2	t
360	2018	5	M	N	5	f	f	1	t
360	2018	5	M	N	5	f	f	2	t
361	2018	3	U	U	5	f	f	1	t
361	2018	3	U	U	5	f	f	2	t
362	2018	6	U	U	2	f	t	2	t
362	2018	6	U	U	2	f	f	1	t
363	2020	5	U	N	2	f	f	1	t
363	2020	5	U	N	2	f	f	2	t
364	2018	1	F	N	1	t	t	2	t
364	2018	1	F	N	1	t	f	1	t
366	2020	2	U	Y	3	t	t	3	t
366	2020	2	U	Y	3	t	t	1	t
368	2018	3	U	Y	5	f	f	1	t
368	2018	3	U	Y	5	f	f	2	t
370	2018	5	M	N	4	f	f	1	t
370	2018	5	M	N	4	f	t	2	t
374	2018	6	U	Y	7	f	t	1	t
374	2018	6	U	Y	7	f	t	3	t
376	2019	1	M	N	1	f	t	3	t
376	2019	1	M	N	1	f	f	1	t
377	2019	3	F	Y	4	f	f	1	t
377	2019	3	F	Y	4	f	t	2	t
379	2020	4	M	Y	1	f	t	3	t
379	2020	4	M	Y	1	f	f	1	t
384	2018	2	U	U	2	t	f	3	t
384	2018	2	U	U	2	t	f	1	t
386	2020	4	F	Y	2	f	t	2	t
386	2020	4	F	Y	2	f	f	1	t
387	2020	2	U	Y	3	f	f	1	t
387	2020	2	U	Y	3	f	t	2	t
388	2020	3	F	N	2	f	f	2	t
388	2020	3	F	N	2	f	f	1	t
389	2020	3	M	Y	6	f	t	2	t
389	2020	3	M	Y	6	f	f	1	t
390	2019	1	F	Y	1	f	f	1	t
390	2019	1	F	Y	1	f	f	3	t
392	2018	1	U	U	5	f	f	2	t
392	2018	1	U	U	5	f	f	1	t
393	2020	2	M	Y	6	t	t	3	t
393	2020	2	M	Y	6	t	f	2	t
393	2020	2	M	Y	6	t	f	1	t
394	2018	4	U	U	4	t	t	2	t
394	2018	4	U	U	4	t	f	1	t
395	2019	1	F	Y	2	f	f	1	t
395	2019	1	F	Y	2	f	t	2	t
396	2019	1	U	U	5	f	f	3	t
396	2019	1	U	U	5	f	f	1	t
398	2019	3	U	U	1	f	t	3	t
398	2019	3	U	U	1	f	f	1	t
400	2019	2	U	N	3	t	f	1	t
400	2019	2	U	N	3	t	f	2	t
401	2018	7	M	Y	6	f	f	1	t
401	2018	7	M	Y	6	f	f	3	t
402	2019	1	U	Y	1	f	f	1	t
402	2019	1	U	Y	1	f	t	2	t
403	2018	5	M	N	4	f	f	2	t
403	2018	5	M	N	4	f	f	1	t
404	2018	3	F	N	5	f	t	2	t
404	2018	3	F	N	5	f	f	1	t
404	2018	3	F	N	5	f	f	3	t
405	2019	1	F	Y	1	f	t	3	t
405	2019	1	F	Y	1	f	t	1	t
406	2018	1	M	Y	1	t	f	1	t
406	2018	1	M	Y	1	t	t	3	t
406	2018	1	M	Y	1	t	f	2	t
407	2018	3	U	N	4	f	f	3	t
407	2018	3	U	N	4	f	f	1	t
407	2018	3	U	N	4	f	t	2	t
408	2018	5	U	Y	6	f	f	1	t
408	2018	5	U	Y	6	f	t	3	t
409	2020	4	M	Y	1	f	f	1	t
409	2020	4	M	Y	1	f	f	2	t
411	2020	2	F	Y	1	f	f	3	t
411	2020	2	F	Y	1	f	f	1	t
413	2019	5	M	U	5	f	t	1	t
413	2019	5	M	U	5	f	f	2	t
414	2020	7	M	U	4	f	f	3	t
414	2020	7	M	U	4	f	f	1	t
415	2019	3	U	Y	4	f	f	2	t
415	2019	3	U	Y	4	f	f	1	t
416	2018	1	M	U	6	t	f	2	t
416	2018	1	M	U	6	t	f	1	t
417	2019	6	U	N	1	f	f	1	t
417	2019	6	U	N	1	f	f	3	t
417	2019	6	U	N	1	f	f	2	t
418	2019	5	U	N	3	f	f	3	t
418	2019	5	U	N	3	f	f	1	t
418	2019	5	U	N	3	f	f	2	t
420	2018	1	M	U	3	f	f	1	t
420	2018	1	M	U	3	f	t	2	t
421	2020	4	M	Y	3	f	f	1	t
421	2020	4	M	Y	3	f	t	3	t
422	2018	1	U	N	4	f	f	2	t
422	2018	1	U	N	4	f	f	1	t
424	2019	5	F	U	1	t	f	3	t
424	2019	5	F	U	1	t	t	2	t
424	2019	5	F	U	1	t	f	1	t
425	2020	4	F	Y	3	f	t	3	t
425	2020	4	F	Y	3	f	t	1	t
425	2020	4	F	Y	3	f	t	2	t
428	2019	6	M	U	2	f	f	2	t
428	2019	6	M	U	2	f	f	1	t
430	2020	3	M	Y	2	f	f	2	t
430	2020	3	M	Y	2	f	f	1	t
432	2019	6	M	N	2	f	t	2	t
432	2019	6	M	N	2	f	f	1	t
432	2019	6	M	N	2	f	t	3	t
435	2019	6	F	Y	7	f	f	1	t
435	2019	6	F	Y	7	f	f	3	t
436	2018	2	M	Y	1	f	t	2	t
436	2018	2	M	Y	1	f	f	1	t
440	2019	1	F	N	4	t	f	1	t
440	2019	1	F	N	4	t	f	3	t
442	2020	3	F	U	6	f	f	1	t
442	2020	3	F	U	6	f	f	2	t
444	2018	2	F	U	7	f	f	1	t
444	2018	2	F	U	7	f	f	2	t
445	2019	6	U	N	5	t	t	2	t
445	2019	6	U	N	5	t	f	1	t
446	2018	4	U	Y	5	f	t	2	t
446	2018	4	U	Y	5	f	f	1	t
447	2018	3	U	U	5	f	f	1	t
447	2018	3	U	U	5	f	f	3	t
448	2020	3	M	N	2	f	f	1	t
448	2020	3	M	N	2	f	t	2	t
449	2020	1	M	Y	7	f	t	3	t
449	2020	1	M	Y	7	f	f	1	t
450	2019	5	M	N	7	t	t	1	t
450	2019	5	M	N	7	t	t	2	t
451	2019	4	M	U	6	f	f	2	t
451	2019	4	M	U	6	f	f	1	t
452	2018	4	M	U	1	f	f	1	t
452	2018	4	M	U	1	f	t	2	t
453	2019	2	F	N	3	t	f	1	t
453	2019	2	F	N	3	t	t	2	t
454	2019	1	U	U	1	f	t	2	t
454	2019	1	U	U	1	f	f	3	t
454	2019	1	U	U	1	f	f	1	t
455	2019	5	M	N	7	f	f	1	t
455	2019	5	M	N	7	f	f	3	t
459	2018	5	M	U	5	f	f	2	t
459	2018	5	M	U	5	f	f	1	t
459	2018	5	M	U	5	f	t	3	t
460	2018	3	M	Y	6	f	f	1	t
460	2018	3	M	Y	6	f	f	2	t
462	2019	4	U	U	2	f	t	2	t
462	2019	4	U	U	2	f	f	3	t
462	2019	4	U	U	2	f	f	1	t
464	2019	4	F	Y	6	f	f	3	t
464	2019	4	F	Y	6	f	f	1	t
465	2019	4	F	Y	2	t	f	3	t
465	2019	4	F	Y	2	t	f	1	t
468	2019	2	U	N	2	f	f	3	t
468	2019	2	U	N	2	f	f	1	t
469	2018	4	F	N	1	f	t	1	t
469	2018	4	F	N	1	f	t	3	t
470	2018	6	U	U	4	t	f	3	t
470	2018	6	U	U	4	t	f	2	t
470	2018	6	U	U	4	t	f	1	t
471	2020	4	F	N	1	f	f	1	t
471	2020	4	F	N	1	f	t	3	t
471	2020	4	F	N	1	f	t	2	t
472	2019	5	F	Y	1	f	f	1	t
472	2019	5	F	Y	1	f	t	2	t
473	2019	2	M	Y	3	f	f	3	t
473	2019	2	M	Y	3	f	f	1	t
474	2020	3	M	U	3	f	f	1	t
474	2020	3	M	U	3	f	t	3	t
476	2018	7	U	Y	2	f	t	2	t
476	2018	7	U	Y	2	f	f	1	t
477	2019	2	F	Y	4	f	f	1	t
477	2019	2	F	Y	4	f	f	3	t
477	2019	2	F	Y	4	f	f	2	t
478	2018	7	U	Y	5	t	f	1	t
478	2018	7	U	Y	5	t	t	2	t
478	2018	7	U	Y	5	t	f	3	t
479	2018	5	F	Y	6	f	f	2	t
479	2018	5	F	Y	6	f	f	1	t
482	2020	7	F	N	2	f	f	3	t
482	2020	7	F	N	2	f	f	1	t
483	2018	3	F	N	5	t	f	2	t
483	2018	3	F	N	5	t	f	1	t
484	2019	1	F	N	1	f	t	3	t
484	2019	1	F	N	1	f	t	1	t
485	2018	3	U	U	3	f	f	2	t
485	2018	3	U	U	3	f	f	1	t
487	2019	7	U	N	7	f	f	1	t
487	2019	7	U	N	7	f	t	2	t
488	2018	2	M	N	3	t	f	1	t
488	2018	2	M	N	3	t	f	3	t
488	2018	2	M	N	3	t	t	2	t
490	2018	6	M	U	7	f	t	3	t
490	2018	6	M	U	7	f	f	1	t
491	2019	1	U	U	3	t	f	2	t
491	2019	1	U	U	3	t	f	1	t
492	2018	3	U	N	3	f	t	2	t
492	2018	3	U	N	3	f	f	1	t
494	2020	7	M	Y	5	f	f	2	t
494	2020	7	M	Y	5	f	f	1	t
495	2018	7	F	N	1	f	f	1	t
495	2018	7	F	N	1	f	t	2	t
499	2020	6	M	Y	2	t	t	2	t
499	2020	6	M	Y	2	t	f	1	t
502	2020	6	F	Y	6	f	f	1	t
502	2020	6	F	Y	6	f	f	3	t
502	2020	6	F	Y	6	f	t	2	t
503	2018	7	M	Y	6	t	f	1	t
503	2018	7	M	Y	6	t	t	3	t
503	2018	7	M	Y	6	t	f	2	t
504	2020	6	F	U	1	f	f	2	t
504	2020	6	F	U	1	f	f	1	t
504	2020	6	F	U	1	f	t	3	t
505	2020	5	F	U	5	t	t	1	t
505	2020	5	F	U	5	t	t	2	t
507	2019	2	M	Y	1	f	f	1	t
507	2019	2	M	Y	1	f	f	3	t
508	2018	5	F	N	4	f	f	1	t
508	2018	5	F	N	4	f	t	2	t
510	2019	6	F	N	2	f	t	2	t
510	2019	6	F	N	2	f	f	1	t
512	2019	3	M	N	4	f	f	1	t
512	2019	3	M	N	4	f	f	2	t
518	2018	1	F	U	3	f	f	2	t
518	2018	1	F	U	3	f	f	1	t
518	2018	1	F	U	3	f	f	3	t
519	2018	4	U	N	4	f	f	1	t
519	2018	4	U	N	4	f	t	2	t
523	2020	6	F	U	5	f	f	1	t
523	2020	6	F	U	5	f	f	3	t
525	2018	7	U	Y	6	f	t	2	t
525	2018	7	U	Y	6	f	f	3	t
525	2018	7	U	Y	6	f	f	1	t
526	2019	6	M	N	7	f	f	1	t
526	2019	6	M	N	7	f	f	3	t
527	2019	2	F	Y	6	f	f	2	t
527	2019	2	F	Y	6	f	f	1	t
529	2019	3	M	U	7	f	f	3	t
529	2019	3	M	U	7	f	f	1	t
529	2019	3	M	U	7	f	f	2	t
532	2018	1	F	Y	3	f	f	1	t
532	2018	1	F	Y	3	f	f	2	t
533	2018	2	M	Y	1	t	f	3	t
533	2018	2	M	Y	1	t	f	2	t
533	2018	2	M	Y	1	t	f	1	t
534	2020	4	U	Y	3	t	f	2	t
534	2020	4	U	Y	3	t	f	1	t
535	2019	4	F	Y	7	f	f	1	t
535	2019	4	F	Y	7	f	f	2	t
536	2020	5	M	U	4	f	f	2	t
536	2020	5	M	U	4	f	t	1	t
537	2020	6	F	Y	3	f	t	2	t
537	2020	6	F	Y	3	f	f	1	t
538	2019	4	F	N	7	f	t	1	t
538	2019	4	F	N	7	f	f	2	t
539	2020	6	U	Y	6	t	f	1	t
539	2020	6	U	Y	6	t	f	2	t
540	2019	5	M	U	3	f	f	1	t
540	2019	5	M	U	3	f	f	3	t
543	2019	4	U	U	6	f	f	2	t
543	2019	4	U	U	6	f	t	3	t
543	2019	4	U	U	6	f	t	1	t
544	2020	3	M	U	2	f	f	3	t
544	2020	3	M	U	2	f	f	1	t
550	2018	7	U	N	1	f	f	1	t
550	2018	7	U	N	1	f	t	2	t
551	2020	6	F	U	3	t	t	3	t
551	2020	6	F	U	3	t	f	1	t
553	2019	2	U	U	5	f	f	2	t
553	2019	2	U	U	5	f	f	1	t
554	2019	1	M	Y	3	f	t	2	t
554	2019	1	M	Y	3	f	t	1	t
555	2020	4	M	N	1	t	t	3	t
555	2020	4	M	N	1	t	f	1	t
556	2020	1	F	N	1	f	f	2	t
556	2020	1	F	N	1	f	f	1	t
557	2019	7	F	N	4	f	t	2	t
557	2019	7	F	N	4	f	f	1	t
557	2019	7	F	N	4	f	f	3	t
558	2018	4	M	U	6	f	f	2	t
558	2018	4	M	U	6	f	f	1	t
561	2018	2	U	Y	3	f	f	3	t
561	2018	2	U	Y	3	f	f	1	t
562	2018	5	U	U	1	f	t	2	t
562	2018	5	U	U	1	f	f	1	t
564	2019	1	U	U	1	f	t	2	t
564	2019	1	U	U	1	f	f	1	t
567	2018	5	M	N	1	f	f	3	t
567	2018	5	M	N	1	f	f	1	t
569	2019	7	F	Y	3	t	t	2	t
569	2019	7	F	Y	3	t	f	1	t
570	2020	6	M	U	3	t	t	1	t
570	2020	6	M	U	3	t	f	3	t
570	2020	6	M	U	3	t	f	2	t
571	2018	5	M	N	6	t	t	2	t
571	2018	5	M	N	6	t	f	3	t
571	2018	5	M	N	6	t	f	1	t
572	2020	6	F	N	6	t	t	2	t
572	2020	6	F	N	6	t	t	3	t
572	2020	6	F	N	6	t	f	1	t
573	2018	4	U	U	5	f	f	1	t
573	2018	4	U	U	5	f	f	2	t
575	2018	4	M	Y	3	f	f	3	t
575	2018	4	M	Y	3	f	t	2	t
575	2018	4	M	Y	3	f	f	1	t
576	2019	2	F	N	3	t	t	2	t
576	2019	2	F	N	3	t	f	1	t
577	2020	1	U	U	5	f	f	3	t
577	2020	1	U	U	5	f	f	1	t
580	2020	4	M	Y	6	t	f	1	t
580	2020	4	M	Y	6	t	f	3	t
582	2018	1	F	Y	1	f	f	2	t
582	2018	1	F	Y	1	f	f	1	t
583	2020	6	F	N	7	f	t	3	t
583	2020	6	F	N	7	f	f	1	t
584	2019	6	M	N	2	f	f	1	t
584	2019	6	M	N	2	f	t	3	t
584	2019	6	M	N	2	f	t	2	t
585	2018	4	M	N	5	f	f	1	t
585	2018	4	M	N	5	f	t	2	t
586	2020	5	F	Y	7	t	f	1	t
586	2020	5	F	Y	7	t	t	3	t
586	2020	5	F	Y	7	t	t	2	t
587	2018	2	U	Y	2	f	f	1	t
587	2018	2	U	Y	2	f	t	2	t
589	2018	1	M	N	2	f	t	2	t
589	2018	1	M	N	2	f	f	3	t
589	2018	1	M	N	2	f	f	1	t
590	2018	7	U	N	5	f	t	1	t
590	2018	7	U	N	5	f	f	2	t
592	2020	3	F	N	2	f	f	1	t
592	2020	3	F	N	2	f	t	3	t
593	2018	2	M	N	4	f	t	1	t
593	2018	2	M	N	4	f	t	2	t
594	2018	3	F	N	1	f	f	2	t
594	2018	3	F	N	1	f	f	1	t
595	2018	1	U	N	6	f	t	2	t
595	2018	1	U	N	6	f	f	1	t
596	2019	4	F	U	4	t	f	3	t
596	2019	4	F	U	4	t	f	1	t
596	2019	4	F	U	4	t	f	2	t
597	2018	6	F	N	3	f	t	2	t
597	2018	6	F	N	3	f	f	1	t
600	2020	4	F	Y	3	f	t	2	t
600	2020	4	F	Y	3	f	f	1	t
601	2019	7	F	N	3	f	f	2	t
601	2019	7	F	N	3	f	f	1	t
602	2020	7	M	N	7	f	f	3	t
602	2020	7	M	N	7	f	f	1	t
603	2020	1	U	N	1	f	f	2	t
603	2020	1	U	N	1	f	f	3	t
603	2020	1	U	N	1	f	f	1	t
604	2020	5	M	N	5	f	f	2	t
604	2020	5	M	N	5	f	f	1	t
606	2018	4	F	U	4	t	t	2	t
606	2018	4	F	U	4	t	f	1	t
607	2018	5	F	U	2	f	f	1	t
607	2018	5	F	U	2	f	f	2	t
609	2020	4	U	Y	6	t	f	1	t
609	2020	4	U	Y	6	t	t	2	t
610	2020	5	M	U	6	f	f	3	t
610	2020	5	M	U	6	f	t	1	t
610	2020	5	M	U	6	f	f	2	t
611	2020	2	M	Y	4	f	t	2	t
611	2020	2	M	Y	4	f	f	1	t
612	2018	5	U	N	3	t	t	1	t
612	2018	5	U	N	3	t	t	2	t
613	2020	2	U	Y	4	f	f	3	t
613	2020	2	U	Y	4	f	f	1	t
613	2020	2	U	Y	4	f	t	2	t
617	2018	1	F	U	4	t	f	1	t
617	2018	1	F	U	4	t	f	2	t
617	2018	1	F	U	4	t	t	3	t
618	2019	3	M	N	5	f	f	1	t
618	2019	3	M	N	5	f	t	2	t
618	2019	3	M	N	5	f	f	3	t
619	2019	1	U	U	2	f	t	2	t
619	2019	1	U	U	2	f	f	1	t
620	2018	5	F	Y	2	f	f	1	t
620	2018	5	F	Y	2	f	f	2	t
622	2020	3	U	Y	1	f	f	1	t
622	2020	3	U	Y	1	f	f	2	t
623	2019	3	F	N	4	t	f	2	t
623	2019	3	F	N	4	t	f	1	t
623	2019	3	F	N	4	t	t	3	t
624	2020	3	F	Y	3	t	t	2	t
624	2020	3	F	Y	3	t	f	1	t
625	2020	4	U	N	4	f	f	1	t
625	2020	4	U	N	4	f	t	2	t
626	2020	3	M	N	1	f	f	1	t
626	2020	3	M	N	1	f	t	3	t
627	2019	4	F	U	3	t	f	3	t
627	2019	4	F	U	3	t	f	1	t
628	2018	6	F	Y	1	t	f	1	t
628	2018	6	F	Y	1	t	t	2	t
629	2019	4	M	Y	7	f	f	1	t
629	2019	4	M	Y	7	f	t	2	t
634	2019	7	M	Y	2	f	f	2	t
634	2019	7	M	Y	2	f	f	1	t
635	2018	2	U	U	7	f	f	2	t
635	2018	2	U	U	7	f	f	3	t
635	2018	2	U	U	7	f	f	1	t
638	2020	6	F	U	2	f	f	1	t
638	2020	6	F	U	2	f	t	2	t
639	2020	4	U	Y	7	f	t	2	t
639	2020	4	U	Y	7	f	f	1	t
640	2019	7	F	U	7	f	f	1	t
640	2019	7	F	U	7	f	f	3	t
640	2019	7	F	U	7	f	t	2	t
642	2018	5	F	Y	2	f	t	2	t
642	2018	5	F	Y	2	f	f	1	t
645	2019	6	U	U	4	f	t	2	t
645	2019	6	U	U	4	f	f	1	t
646	2020	6	F	N	5	f	f	1	t
646	2020	6	F	N	5	f	t	3	t
647	2018	5	F	U	6	f	f	1	t
647	2018	5	F	U	6	f	f	3	t
648	2020	6	M	U	4	f	f	1	t
648	2020	6	M	U	4	f	f	3	t
649	2018	2	M	Y	7	f	f	1	t
649	2018	2	M	Y	7	f	t	3	t
650	2018	6	U	U	2	t	f	1	t
650	2018	6	U	U	2	t	f	2	t
651	2019	5	M	Y	1	f	f	1	t
651	2019	5	M	Y	1	f	t	2	t
652	2018	3	F	U	1	f	t	2	t
652	2018	3	F	U	1	f	f	1	t
652	2018	3	F	U	1	f	f	3	t
653	2018	2	M	U	7	t	f	1	t
653	2018	2	M	U	7	t	f	2	t
655	2020	5	F	Y	3	t	t	2	t
655	2020	5	F	Y	3	t	t	3	t
655	2020	5	F	Y	3	t	f	1	t
656	2020	1	F	Y	4	t	f	2	t
656	2020	1	F	Y	4	t	t	3	t
656	2020	1	F	Y	4	t	f	1	t
657	2018	5	U	N	3	f	t	1	t
657	2018	5	U	N	3	f	t	2	t
657	2018	5	U	N	3	f	t	3	t
658	2020	5	F	U	2	f	f	1	t
658	2020	5	F	U	2	f	t	3	t
659	2020	1	F	Y	3	f	f	2	t
659	2020	1	F	Y	3	f	f	1	t
660	2018	5	U	U	3	f	f	1	t
660	2018	5	U	U	3	f	f	3	t
661	2020	6	F	U	7	f	t	3	t
661	2020	6	F	U	7	f	f	1	t
663	2019	2	U	U	3	t	f	1	t
663	2019	2	U	U	3	t	f	2	t
664	2019	6	F	N	7	f	t	2	t
664	2019	6	F	N	7	f	t	1	t
666	2018	6	M	N	3	f	f	3	t
666	2018	6	M	N	3	f	t	1	t
667	2020	7	U	Y	6	f	f	1	t
667	2020	7	U	Y	6	f	t	2	t
668	2018	1	M	Y	7	t	t	2	t
668	2018	1	M	Y	7	t	f	3	t
668	2018	1	M	Y	7	t	f	1	t
670	2018	3	U	N	1	t	f	3	t
670	2018	3	U	N	1	t	f	1	t
670	2018	3	U	N	1	t	f	2	t
671	2018	1	F	N	1	f	t	3	t
671	2018	1	F	N	1	f	t	1	t
672	2020	7	U	N	5	f	t	2	t
672	2020	7	U	N	5	f	f	1	t
672	2020	7	U	N	5	f	t	3	t
674	2018	4	U	U	6	f	f	1	t
674	2018	4	U	U	6	f	t	2	t
674	2018	4	U	U	6	f	f	3	t
675	2020	6	M	Y	3	f	t	2	t
675	2020	6	M	Y	3	f	f	1	t
676	2018	7	M	Y	6	f	f	2	t
676	2018	7	M	Y	6	f	f	1	t
679	2020	4	U	N	1	f	f	3	t
679	2020	4	U	N	1	f	f	1	t
680	2018	6	F	U	4	t	t	3	t
680	2018	6	F	U	4	t	f	2	t
680	2018	6	F	U	4	t	f	1	t
682	2020	7	U	Y	3	f	f	3	t
682	2020	7	U	Y	3	f	f	1	t
684	2020	6	U	N	6	f	f	3	t
684	2020	6	U	N	6	f	f	1	t
684	2020	6	U	N	6	f	f	2	t
685	2019	3	M	N	2	f	f	2	t
685	2019	3	M	N	2	f	f	1	t
686	2018	7	U	U	4	t	t	2	t
686	2018	7	U	U	4	t	f	1	t
688	2020	1	F	U	4	t	f	1	t
688	2020	1	F	U	4	t	f	2	t
689	2018	3	M	Y	4	f	f	1	t
689	2018	3	M	Y	4	f	t	2	t
691	2018	2	F	U	6	t	f	2	t
691	2018	2	F	U	6	t	f	1	t
692	2020	6	M	Y	6	f	t	1	t
692	2020	6	M	Y	6	f	f	2	t
693	2018	5	U	Y	7	t	f	1	t
693	2018	5	U	Y	7	t	f	2	t
694	2019	6	U	N	3	f	f	3	t
694	2019	6	U	N	3	f	f	1	t
698	2019	1	M	Y	6	f	f	1	t
698	2019	1	M	Y	6	f	t	3	t
700	2019	2	U	Y	5	f	t	2	t
700	2019	2	U	Y	5	f	f	1	t
701	2020	4	U	N	3	f	f	1	t
701	2020	4	U	N	3	f	f	2	t
702	2020	6	U	Y	3	f	f	2	t
702	2020	6	U	Y	3	f	f	3	t
702	2020	6	U	Y	3	f	f	1	t
703	2019	7	F	N	3	f	f	2	t
703	2019	7	F	N	3	f	f	1	t
704	2018	4	U	U	4	f	t	3	t
704	2018	4	U	U	4	f	f	1	t
705	2019	6	M	N	5	t	f	3	t
705	2019	6	M	N	5	t	f	1	t
706	2019	4	F	U	3	f	t	2	t
706	2019	4	F	U	3	f	t	3	t
706	2019	4	F	U	3	f	f	1	t
707	2019	4	M	Y	6	t	f	1	t
707	2019	4	M	Y	6	t	t	2	t
709	2020	3	F	N	2	f	f	1	t
709	2020	3	F	N	2	f	f	2	t
711	2018	7	F	N	2	f	f	2	t
711	2018	7	F	N	2	f	f	1	t
711	2018	7	F	N	2	f	t	3	t
712	2019	4	F	U	5	f	t	2	t
712	2019	4	F	U	5	f	f	1	t
713	2018	1	M	Y	6	f	f	1	t
713	2018	1	M	Y	6	f	t	3	t
713	2018	1	M	Y	6	f	t	2	t
715	2019	7	F	U	7	t	f	3	t
715	2019	7	F	U	7	t	f	1	t
716	2020	2	U	Y	5	t	t	2	t
716	2020	2	U	Y	5	t	f	1	t
716	2020	2	U	Y	5	t	f	3	t
717	2019	7	M	Y	3	f	t	3	t
717	2019	7	M	Y	3	f	f	1	t
718	2020	5	M	U	1	f	t	2	t
718	2020	5	M	U	1	f	f	1	t
718	2020	5	M	U	1	f	f	3	t
720	2018	3	M	Y	3	t	f	3	t
720	2018	3	M	Y	3	t	f	1	t
721	2019	7	M	U	2	f	t	3	t
721	2019	7	M	U	2	f	f	2	t
721	2019	7	M	U	2	f	t	1	t
722	2020	7	M	U	7	t	t	3	t
722	2020	7	M	U	7	t	f	2	t
722	2020	7	M	U	7	t	f	1	t
725	2020	2	F	N	5	f	f	2	t
725	2020	2	F	N	5	f	f	1	t
726	2020	1	F	Y	5	f	f	1	t
726	2020	1	F	Y	5	f	f	2	t
726	2020	1	F	Y	5	f	f	3	t
727	2018	4	M	Y	4	t	f	3	t
727	2018	4	M	Y	4	t	f	1	t
727	2018	4	M	Y	4	t	f	2	t
728	2018	4	M	U	3	f	f	2	t
728	2018	4	M	U	3	f	f	1	t
729	2020	3	M	N	3	f	f	1	t
729	2020	3	M	N	3	f	f	2	t
732	2018	4	F	U	4	f	f	2	t
732	2018	4	F	U	4	f	f	1	t
733	2018	6	F	U	4	f	f	1	t
733	2018	6	F	U	4	f	f	3	t
735	2018	7	M	Y	2	f	f	1	t
735	2018	7	M	Y	2	f	t	2	t
735	2018	7	M	Y	2	f	t	3	t
737	2020	3	M	N	6	f	f	1	t
737	2020	3	M	N	6	f	f	3	t
738	2019	5	U	N	7	t	f	1	t
738	2019	5	U	N	7	t	t	3	t
738	2019	5	U	N	7	t	t	2	t
739	2020	6	M	U	5	f	f	1	t
739	2020	6	M	U	5	f	f	2	t
740	2020	7	M	Y	7	f	f	1	t
740	2020	7	M	Y	7	f	t	2	t
741	2018	7	F	U	6	t	t	3	t
741	2018	7	F	U	6	t	f	2	t
741	2018	7	F	U	6	t	f	1	t
742	2020	5	M	U	1	f	f	1	t
742	2020	5	M	U	1	f	t	2	t
743	2018	6	M	Y	1	f	f	1	t
743	2018	6	M	Y	1	f	f	3	t
744	2018	6	M	N	1	f	f	1	t
744	2018	6	M	N	1	f	f	3	t
745	2018	3	F	N	4	f	f	1	t
745	2018	3	F	N	4	f	f	3	t
746	2020	2	U	U	4	t	f	3	t
746	2020	2	U	U	4	t	f	1	t
749	2019	1	U	Y	4	t	f	1	t
749	2019	1	U	Y	4	t	t	3	t
749	2019	1	U	Y	4	t	f	2	t
750	2018	2	M	U	5	f	f	3	t
750	2018	2	M	U	5	f	f	1	t
751	2020	4	M	Y	2	f	t	1	t
751	2020	4	M	Y	2	f	f	2	t
752	2019	7	U	N	3	f	t	2	t
752	2019	7	U	N	3	f	f	1	t
754	2020	3	U	Y	2	f	f	2	t
754	2020	3	U	Y	2	f	f	1	t
754	2020	3	U	Y	2	f	t	3	t
755	2018	4	F	N	2	f	f	2	t
755	2018	4	F	N	2	f	f	1	t
755	2018	4	F	N	2	f	f	3	t
756	2019	4	M	Y	5	f	t	2	t
756	2019	4	M	Y	5	f	f	1	t
757	2019	4	F	Y	4	f	t	2	t
757	2019	4	F	Y	4	f	f	1	t
758	2018	7	M	Y	7	f	t	2	t
758	2018	7	M	Y	7	f	f	1	t
760	2018	3	M	Y	5	f	t	2	t
760	2018	3	M	Y	5	f	f	1	t
761	2019	3	U	U	3	t	t	2	t
761	2019	3	U	U	3	t	f	1	t
761	2019	3	U	U	3	t	t	3	t
762	2020	3	F	U	3	t	t	2	t
762	2020	3	F	U	3	t	f	3	t
762	2020	3	F	U	3	t	f	1	t
763	2019	5	M	U	3	f	t	1	t
763	2019	5	M	U	3	f	f	2	t
764	2018	6	M	Y	6	f	t	1	t
764	2018	6	M	Y	6	f	t	3	t
767	2019	1	M	Y	2	t	t	2	t
767	2019	1	M	Y	2	t	f	1	t
768	2019	4	U	N	2	t	f	1	t
768	2019	4	U	N	2	t	t	2	t
770	2019	7	M	U	6	f	f	1	t
770	2019	7	M	U	6	f	f	3	t
772	2018	3	M	Y	5	f	t	2	t
772	2018	3	M	Y	5	f	f	1	t
772	2018	3	M	Y	5	f	t	3	t
774	2020	4	U	N	5	f	f	1	t
774	2020	4	U	N	5	f	f	3	t
777	2018	2	U	N	6	f	t	2	t
777	2018	2	U	N	6	f	f	1	t
778	2018	1	U	U	7	f	f	2	t
778	2018	1	U	U	7	f	f	1	t
779	2020	6	F	Y	2	f	f	2	t
779	2020	6	F	Y	2	f	f	1	t
780	2020	2	F	Y	6	t	t	2	t
780	2020	2	F	Y	6	t	f	1	t
781	2018	5	U	U	4	t	f	2	t
781	2018	5	U	U	4	t	f	1	t
782	2019	7	U	U	7	f	t	2	t
782	2019	7	U	U	7	f	f	1	t
782	2019	7	U	U	7	f	t	3	t
783	2018	7	F	U	5	f	t	3	t
783	2018	7	F	U	5	f	t	1	t
783	2018	7	F	U	5	f	f	2	t
789	2019	7	F	N	1	t	f	2	t
789	2019	7	F	N	1	t	t	3	t
789	2019	7	F	N	1	t	f	1	t
790	2018	6	U	U	3	f	f	1	t
790	2018	6	U	U	3	f	f	3	t
793	2020	2	M	N	5	f	f	2	t
793	2020	2	M	N	5	f	f	1	t
801	2019	5	U	Y	6	f	f	1	t
801	2019	5	U	Y	6	f	t	2	t
802	2019	4	M	N	6	f	f	1	t
802	2019	4	M	N	6	f	t	2	t
804	2018	6	U	Y	1	t	f	1	t
804	2018	6	U	Y	1	t	t	2	t
804	2018	6	U	Y	1	t	f	3	t
805	2020	6	M	N	4	f	t	3	t
805	2020	6	M	N	4	f	f	1	t
806	2018	4	M	N	2	t	f	2	t
806	2018	4	M	N	2	t	f	3	t
806	2018	4	M	N	2	t	f	1	t
808	2019	3	U	Y	7	t	f	2	t
808	2019	3	U	Y	7	t	f	1	t
810	2018	1	U	N	4	f	f	1	t
810	2018	1	U	N	4	f	f	3	t
811	2019	3	M	N	7	t	f	3	t
811	2019	3	M	N	7	t	f	1	t
812	2018	1	U	U	2	f	t	1	t
812	2018	1	U	U	2	f	t	2	t
813	2019	4	M	N	2	f	f	3	t
813	2019	4	M	N	2	f	f	1	t
813	2019	4	M	N	2	f	f	2	t
815	2020	4	U	U	3	t	f	1	t
815	2020	4	U	U	3	t	t	2	t
815	2020	4	U	U	3	t	f	3	t
816	2019	1	F	Y	7	f	t	2	t
816	2019	1	F	Y	7	f	f	1	t
817	2019	4	M	Y	1	t	f	1	t
817	2019	4	M	Y	1	t	f	3	t
819	2018	2	F	U	5	f	f	1	t
819	2018	2	F	U	5	f	t	2	t
819	2018	2	F	U	5	f	t	3	t
820	2019	3	U	N	7	f	t	2	t
820	2019	3	U	N	7	f	f	1	t
823	2019	7	M	N	2	f	f	2	t
823	2019	7	M	N	2	f	f	1	t
824	2020	7	M	Y	1	f	f	3	t
824	2020	7	M	Y	1	f	f	1	t
824	2020	7	M	Y	1	f	t	2	t
826	2019	7	F	N	2	f	f	2	t
826	2019	7	F	N	2	f	f	1	t
827	2020	4	M	U	2	f	f	1	t
827	2020	4	M	U	2	f	t	2	t
828	2018	1	M	N	4	f	f	1	t
828	2018	1	M	N	4	f	f	3	t
830	2020	4	U	N	4	f	t	1	t
830	2020	4	U	N	4	f	t	3	t
831	2019	3	F	U	6	f	f	1	t
831	2019	3	F	U	6	f	t	2	t
831	2019	3	F	U	6	f	f	3	t
833	2019	3	F	U	5	f	f	1	t
833	2019	3	F	U	5	f	f	2	t
834	2019	6	U	U	6	f	f	2	t
834	2019	6	U	U	6	f	f	1	t
837	2019	1	U	U	7	f	f	3	t
837	2019	1	U	U	7	f	t	2	t
837	2019	1	U	U	7	f	f	1	t
839	2020	1	F	N	7	f	t	1	t
839	2020	1	F	N	7	f	f	2	t
840	2019	7	F	N	7	f	t	2	t
840	2019	7	F	N	7	f	f	3	t
840	2019	7	F	N	7	f	t	1	t
843	2018	5	M	N	3	f	f	1	t
843	2018	5	M	N	3	f	t	2	t
844	2019	4	M	Y	1	f	f	2	t
844	2019	4	M	Y	1	f	f	1	t
845	2020	5	M	U	2	t	t	2	t
845	2020	5	M	U	2	t	f	1	t
847	2018	6	F	N	7	t	f	2	t
847	2018	6	F	N	7	t	f	1	t
848	2019	5	F	U	3	t	f	3	t
848	2019	5	F	U	3	t	f	1	t
850	2018	4	F	Y	6	t	t	2	t
850	2018	4	F	Y	6	t	f	3	t
850	2018	4	F	Y	6	t	f	1	t
851	2018	5	M	N	5	f	t	3	t
851	2018	5	M	N	5	f	f	1	t
852	2019	5	U	N	1	t	t	2	t
852	2019	5	U	N	1	t	t	1	t
853	2020	7	F	U	5	f	f	3	t
853	2020	7	F	U	5	f	f	1	t
854	2020	7	M	Y	1	f	t	3	t
854	2020	7	M	Y	1	f	f	1	t
855	2020	4	U	U	2	f	t	2	t
855	2020	4	U	U	2	f	f	1	t
859	2020	3	F	N	4	f	f	1	t
859	2020	3	F	N	4	f	f	2	t
860	2020	5	U	N	6	f	t	2	t
860	2020	5	U	N	6	f	f	1	t
861	2019	1	F	N	2	f	f	3	t
861	2019	1	F	N	2	f	f	1	t
861	2019	1	F	N	2	f	t	2	t
863	2019	3	M	Y	6	f	f	1	t
863	2019	3	M	Y	6	f	t	3	t
864	2018	6	M	Y	3	f	t	2	t
864	2018	6	M	Y	3	f	f	1	t
865	2019	5	M	N	2	f	t	2	t
865	2019	5	M	N	2	f	f	1	t
866	2019	7	F	Y	1	t	f	1	t
866	2019	7	F	Y	1	t	t	2	t
867	2020	3	F	N	5	f	f	1	t
867	2020	3	F	N	5	f	f	3	t
868	2018	6	F	U	1	t	t	1	t
868	2018	6	F	U	1	t	t	3	t
869	2018	1	F	U	6	f	f	1	t
869	2018	1	F	U	6	f	f	3	t
870	2018	7	F	U	2	f	t	3	t
870	2018	7	F	U	2	f	f	1	t
873	2018	4	U	N	2	f	t	2	t
873	2018	4	U	N	2	f	f	1	t
874	2018	7	U	U	4	t	f	1	t
874	2018	7	U	U	4	t	f	2	t
876	2018	1	U	Y	3	f	f	1	t
876	2018	1	U	Y	3	f	f	3	t
876	2018	1	U	Y	3	f	f	2	t
877	2020	5	F	N	6	f	f	1	t
877	2020	5	F	N	6	f	t	3	t
878	2018	4	M	U	1	f	t	2	t
878	2018	4	M	U	1	f	f	1	t
879	2018	2	F	N	3	f	f	1	t
879	2018	2	F	N	3	f	f	2	t
880	2019	4	F	Y	6	f	f	2	t
880	2019	4	F	Y	6	f	f	1	t
882	2018	6	U	U	5	t	t	3	t
882	2018	6	U	U	5	t	f	1	t
885	2019	3	U	U	1	f	f	1	t
885	2019	3	U	U	1	f	f	3	t
888	2020	1	F	N	3	f	f	3	t
888	2020	1	F	N	3	f	f	1	t
889	2019	3	F	Y	4	f	t	2	t
889	2019	3	F	Y	4	f	t	1	t
890	2020	4	M	U	4	f	f	1	t
890	2020	4	M	U	4	f	t	2	t
893	2019	5	M	N	6	f	t	2	t
893	2019	5	M	N	6	f	f	1	t
894	2019	2	U	N	7	f	t	1	t
894	2019	2	U	N	7	f	f	3	t
895	2019	7	U	N	1	f	t	2	t
895	2019	7	U	N	1	f	f	1	t
895	2019	7	U	N	1	f	f	3	t
896	2018	2	M	N	5	f	f	2	t
896	2018	2	M	N	5	f	f	1	t
897	2020	5	U	U	3	t	f	1	t
897	2020	5	U	U	3	t	t	3	t
899	2019	6	U	Y	2	f	f	2	t
899	2019	6	U	Y	2	f	f	1	t
903	2019	5	M	Y	6	f	f	1	t
903	2019	5	M	Y	6	f	t	2	t
903	2019	5	M	Y	6	f	f	3	t
904	2018	7	M	U	7	f	f	3	t
904	2018	7	M	U	7	f	f	1	t
905	2018	3	U	U	1	f	f	3	t
905	2018	3	U	U	1	f	f	1	t
906	2020	5	M	Y	1	f	f	3	t
906	2020	5	M	Y	1	f	f	1	t
906	2020	5	M	Y	1	f	f	2	t
907	2020	5	F	N	3	f	f	2	t
907	2020	5	F	N	3	f	f	1	t
909	2020	3	M	Y	4	f	f	2	t
909	2020	3	M	Y	4	f	f	3	t
909	2020	3	M	Y	4	f	f	1	t
912	2018	1	M	U	4	f	f	1	t
912	2018	1	M	U	4	f	t	3	t
914	2019	2	U	N	4	f	f	1	t
914	2019	2	U	N	4	f	t	2	t
916	2019	3	M	U	2	f	f	2	t
916	2019	3	M	U	2	f	t	3	t
916	2019	3	M	U	2	f	f	1	t
918	2019	4	F	N	6	f	f	1	t
918	2019	4	F	N	6	f	t	3	t
919	2020	7	F	U	3	t	t	2	t
919	2020	7	F	U	3	t	f	1	t
920	2018	6	U	U	2	f	f	2	t
920	2018	6	U	U	2	f	f	1	t
921	2019	3	U	N	7	f	f	1	t
921	2019	3	U	N	7	f	f	3	t
922	2018	1	F	Y	1	f	f	1	t
922	2018	1	F	Y	1	f	f	3	t
924	2020	7	M	U	2	f	f	2	t
924	2020	7	M	U	2	f	f	3	t
924	2020	7	M	U	2	f	f	1	t
928	2020	7	U	N	1	f	t	2	t
928	2020	7	U	N	1	f	f	1	t
929	2020	1	F	N	7	f	f	3	t
929	2020	1	F	N	7	f	f	1	t
932	2018	5	U	Y	1	t	t	2	t
932	2018	5	U	Y	1	t	f	1	t
935	2019	5	M	N	4	f	f	2	t
935	2019	5	M	N	4	f	f	1	t
935	2019	5	M	N	4	f	f	3	t
936	2020	1	F	N	6	t	f	1	t
936	2020	1	F	N	6	t	t	3	t
936	2020	1	F	N	6	t	t	2	t
937	2019	7	F	Y	4	f	f	2	t
937	2019	7	F	Y	4	f	f	1	t
938	2020	4	M	Y	2	f	t	2	t
938	2020	4	M	Y	2	f	t	1	t
940	2019	7	F	Y	2	f	t	2	t
940	2019	7	F	Y	2	f	f	1	t
940	2019	7	F	Y	2	f	f	3	t
941	2018	2	M	Y	2	f	f	2	t
941	2018	2	M	Y	2	f	f	1	t
944	2019	5	M	Y	6	f	f	1	t
944	2019	5	M	Y	6	f	t	2	t
944	2019	5	M	Y	6	f	t	3	t
945	2018	5	F	N	2	f	f	1	t
945	2018	5	F	N	2	f	f	3	t
945	2018	5	F	N	2	f	f	2	t
946	2018	5	U	N	6	f	f	2	t
946	2018	5	U	N	6	f	f	1	t
947	2019	7	F	Y	1	f	t	1	t
947	2019	7	F	Y	1	f	t	2	t
948	2018	3	M	Y	2	f	t	3	t
948	2018	3	M	Y	2	f	f	1	t
949	2019	6	U	U	3	f	f	1	t
949	2019	6	U	U	3	f	f	2	t
950	2018	4	F	U	7	f	t	2	t
950	2018	4	F	U	7	f	f	1	t
951	2019	5	F	N	1	f	f	2	t
951	2019	5	F	N	1	f	f	1	t
952	2018	3	F	N	3	f	f	3	t
952	2018	3	F	N	3	f	f	1	t
953	2020	3	F	N	7	f	f	1	t
953	2020	3	F	N	7	f	f	2	t
954	2020	2	F	Y	7	f	f	1	t
954	2020	2	F	Y	7	f	t	2	t
955	2020	3	M	Y	5	t	f	1	t
955	2020	3	M	Y	5	t	f	3	t
957	2018	4	F	U	5	t	f	3	t
957	2018	4	F	U	5	t	t	2	t
957	2018	4	F	U	5	t	f	1	t
958	2019	5	F	Y	7	f	f	1	t
958	2019	5	F	Y	7	f	f	3	t
958	2019	5	F	Y	7	f	f	2	t
959	2020	3	F	U	6	f	f	2	t
959	2020	3	F	U	6	f	f	1	t
960	2020	6	F	Y	2	f	f	1	t
960	2020	6	F	Y	2	f	f	2	t
961	2020	2	U	Y	4	t	t	2	t
961	2020	2	U	Y	4	t	f	1	t
961	2020	2	U	Y	4	t	t	3	t
962	2019	3	F	Y	5	f	t	2	t
962	2019	3	F	Y	5	f	f	1	t
963	2018	5	M	Y	2	f	t	2	t
963	2018	5	M	Y	2	f	f	3	t
963	2018	5	M	Y	2	f	f	1	t
964	2019	2	U	N	4	f	f	1	t
964	2019	2	U	N	4	f	f	3	t
965	2020	1	M	N	3	f	f	2	t
965	2020	1	M	N	3	f	f	3	t
965	2020	1	M	N	3	f	f	1	t
966	2018	4	F	N	1	f	f	1	t
966	2018	4	F	N	1	f	f	2	t
967	2020	4	F	N	5	t	t	2	t
967	2020	4	F	N	5	t	f	1	t
969	2018	5	U	N	5	t	f	3	t
969	2018	5	U	N	5	t	f	1	t
970	2019	6	M	U	7	f	t	2	t
970	2019	6	M	U	7	f	f	1	t
972	2018	3	U	U	6	f	f	1	t
972	2018	3	U	U	6	f	f	3	t
972	2018	3	U	U	6	f	t	2	t
973	2019	5	F	U	3	f	f	1	t
973	2019	5	F	U	3	f	f	2	t
974	2020	2	U	U	4	t	f	1	t
974	2020	2	U	U	4	t	f	3	t
975	2019	4	U	U	6	t	f	2	t
975	2019	4	U	U	6	t	f	1	t
976	2018	7	M	N	4	f	f	1	t
976	2018	7	M	N	4	f	t	3	t
981	2018	6	M	N	5	f	f	2	t
981	2018	6	M	N	5	f	f	1	t
982	2018	4	M	Y	1	f	f	2	t
982	2018	4	M	Y	1	f	f	1	t
984	2019	7	F	N	6	f	t	3	t
984	2019	7	F	N	6	f	f	1	t
986	2018	4	F	U	2	f	t	3	t
986	2018	4	F	U	2	f	t	1	t
987	2019	1	F	N	2	f	f	1	t
987	2019	1	F	N	2	f	t	3	t
988	2018	5	F	N	4	f	t	1	t
988	2018	5	F	N	4	f	t	3	t
988	2018	5	F	N	4	f	f	2	t
989	2020	2	F	Y	2	f	f	1	t
989	2020	2	F	Y	2	f	t	2	t
990	2019	4	F	N	4	t	f	1	t
990	2019	4	F	N	4	t	f	2	t
991	2019	2	F	N	4	f	f	2	t
991	2019	2	F	N	4	f	f	1	t
992	2018	2	M	N	6	t	f	1	t
992	2018	2	M	N	6	t	f	3	t
994	2020	2	F	Y	7	f	f	3	t
994	2020	2	F	Y	7	f	f	1	t
996	2020	4	M	U	2	f	t	2	t
996	2020	4	M	U	2	f	f	1	t
996	2020	4	M	U	2	f	f	3	t
999	2020	7	F	N	5	f	t	2	t
999	2020	7	F	N	5	f	f	1	t
1001	2019	5	M	N	3	t	f	1	t
1001	2019	5	M	N	3	t	f	3	t
1007	2020	2	U	U	3	t	t	3	t
1007	2020	2	U	U	3	t	f	1	t
1012	2018	6	U	N	7	f	t	3	t
1012	2018	6	U	N	7	f	t	1	t
1015	2019	6	F	N	3	f	t	3	t
1015	2019	6	F	N	3	f	t	1	t
1020	2019	1	F	N	2	f	f	1	t
1020	2019	1	F	N	2	f	f	3	t
1022	2020	3	F	Y	4	f	t	3	t
1022	2020	3	F	Y	4	f	t	1	t
1025	2020	3	M	N	7	f	f	3	t
1025	2020	3	M	N	7	f	f	1	t
1028	2019	6	F	U	7	t	f	3	t
1028	2019	6	F	U	7	t	f	1	t
1030	2019	7	M	U	2	f	f	1	t
1030	2019	7	M	U	2	f	f	3	t
1038	2020	1	F	Y	4	f	f	1	t
1038	2020	1	F	Y	4	f	t	3	t
1040	2018	6	F	Y	5	f	f	3	t
1040	2018	6	F	Y	5	f	f	1	t
1042	2020	1	F	Y	7	f	f	3	t
1042	2020	1	F	Y	7	f	f	1	t
1047	2020	3	M	Y	5	f	f	3	t
1047	2020	3	M	Y	5	f	f	1	t
1048	2020	5	U	N	2	t	f	1	t
1048	2020	5	U	N	2	t	f	3	t
1049	2020	7	U	U	7	f	t	3	t
1049	2020	7	U	U	7	f	f	1	t
1055	2019	7	F	Y	3	f	f	3	t
1055	2019	7	F	Y	3	f	f	1	t
1061	2018	1	M	N	1	f	f	1	t
1061	2018	1	M	N	1	f	t	3	t
1064	2018	3	U	N	7	f	f	1	t
1064	2018	3	U	N	7	f	t	3	t
1066	2020	3	F	N	2	f	f	1	t
1066	2020	3	F	N	2	f	f	3	t
1067	2018	3	M	U	3	f	t	3	t
1067	2018	3	M	U	3	f	f	1	t
1068	2019	7	M	U	2	t	f	3	t
1068	2019	7	M	U	2	t	f	1	t
1070	2019	7	M	N	5	f	t	3	t
1070	2019	7	M	N	5	f	f	1	t
1072	2019	2	M	N	1	t	f	1	t
1072	2019	2	M	N	1	t	f	3	t
1073	2019	7	F	Y	6	f	f	3	t
1073	2019	7	F	Y	6	f	f	1	t
1075	2019	1	M	Y	3	f	f	1	t
1075	2019	1	M	Y	3	f	t	3	t
1080	2018	7	M	Y	7	f	t	3	t
1080	2018	7	M	Y	7	f	f	1	t
1088	2018	3	F	Y	7	t	f	3	t
1088	2018	3	F	Y	7	t	f	1	t
1106	2020	2	U	N	5	t	f	1	t
1106	2020	2	U	N	5	t	t	3	t
1108	2018	2	M	N	5	f	f	3	t
1108	2018	2	M	N	5	f	f	1	t
1113	2019	5	F	N	2	f	f	1	t
1113	2019	5	F	N	2	f	f	3	t
1116	2019	7	F	Y	7	f	f	3	t
1116	2019	7	F	Y	7	f	f	1	t
1117	2019	7	U	Y	1	f	t	3	t
1117	2019	7	U	Y	1	f	t	1	t
1118	2018	1	U	Y	5	f	f	1	t
1118	2018	1	U	Y	5	f	f	3	t
1124	2019	1	M	U	5	f	f	1	t
1124	2019	1	M	U	5	f	f	3	t
1130	2018	5	U	Y	4	f	t	3	t
1130	2018	5	U	Y	4	f	f	1	t
1134	2020	4	F	N	2	t	f	1	t
1134	2020	4	F	N	2	t	f	3	t
1139	2019	4	U	N	2	f	f	1	t
1139	2019	4	U	N	2	f	t	3	t
1152	2020	5	M	U	1	t	f	3	t
1152	2020	5	M	U	1	t	f	1	t
1153	2018	4	M	U	4	f	f	1	t
1153	2018	4	M	U	4	f	t	3	t
1154	2019	3	M	Y	5	t	f	1	t
1154	2019	3	M	Y	5	t	f	3	t
1160	2019	1	F	Y	5	f	f	1	t
1160	2019	1	F	Y	5	f	f	3	t
1164	2018	6	M	U	7	f	f	3	t
1164	2018	6	M	U	7	f	f	1	t
1166	2020	1	F	U	1	f	t	1	t
1166	2020	1	F	U	1	f	t	3	t
1168	2020	6	F	Y	4	t	f	1	t
1168	2020	6	F	Y	4	t	t	3	t
1170	2020	6	F	N	5	f	t	3	t
1170	2020	6	F	N	5	f	f	1	t
1174	2018	7	U	Y	7	f	f	3	t
1174	2018	7	U	Y	7	f	f	1	t
1175	2018	5	F	U	6	f	f	3	t
1175	2018	5	F	U	6	f	f	1	t
1176	2019	2	M	Y	1	f	t	3	t
1176	2019	2	M	Y	1	f	f	1	t
1178	2020	4	F	U	7	f	t	3	t
1178	2020	4	F	U	7	f	t	1	t
1179	2018	3	U	N	7	f	f	3	t
1179	2018	3	U	N	7	f	f	1	t
1188	2019	2	F	Y	6	f	t	3	t
1188	2019	2	F	Y	6	f	t	1	t
1190	2020	6	F	N	3	t	t	1	t
1190	2020	6	F	N	3	t	t	3	t
1192	2019	2	M	Y	3	t	t	3	t
1192	2019	2	M	Y	3	t	f	1	t
1194	2019	7	U	U	1	f	f	1	t
1194	2019	7	U	U	1	f	f	3	t
1198	2020	2	M	Y	4	f	f	3	t
1198	2020	2	M	Y	4	f	f	1	t
1202	2019	6	M	N	1	f	t	3	t
1202	2019	6	M	N	1	f	t	1	t
1206	2020	1	F	N	1	f	f	3	t
1206	2020	1	F	N	1	f	f	1	t
1211	2018	1	M	N	6	f	f	1	t
1211	2018	1	M	N	6	f	f	3	t
1213	2020	3	F	N	7	t	t	3	t
1213	2020	3	F	N	7	t	t	1	t
1216	2018	5	F	Y	1	t	f	1	t
1216	2018	5	F	Y	1	t	f	3	t
1217	2020	3	F	Y	2	f	f	3	t
1217	2020	3	F	Y	2	f	f	1	t
1224	2018	2	F	Y	7	f	f	1	t
1224	2018	2	F	Y	7	f	t	3	t
1225	2018	3	M	U	5	t	f	3	t
1225	2018	3	M	U	5	t	f	1	t
1227	2018	7	U	U	7	f	t	1	t
1227	2018	7	U	U	7	f	t	3	t
1229	2018	3	U	N	7	f	f	1	t
1229	2018	3	U	N	7	f	f	3	t
1230	2018	7	U	U	4	f	t	3	t
1230	2018	7	U	U	4	f	f	1	t
1232	2020	3	F	N	7	f	f	1	t
1232	2020	3	F	N	7	f	t	3	t
1233	2020	3	M	Y	2	t	t	3	t
1233	2020	3	M	Y	2	t	f	1	t
1234	2020	7	U	N	2	f	f	3	t
1234	2020	7	U	N	2	f	f	1	t
1235	2019	4	U	Y	5	f	f	3	t
1235	2019	4	U	Y	5	f	f	1	t
1251	2019	3	U	Y	5	f	f	3	t
1251	2019	3	U	Y	5	f	f	1	t
1253	2019	5	U	Y	7	f	f	3	t
1253	2019	5	U	Y	7	f	f	1	t
1263	2020	7	F	U	6	f	t	3	t
1263	2020	7	F	U	6	f	f	1	t
1264	2018	7	U	Y	7	f	t	3	t
1264	2018	7	U	Y	7	f	t	1	t
1269	2019	3	U	Y	4	f	f	1	t
1269	2019	3	U	Y	4	f	f	3	t
1272	2020	7	U	U	1	f	t	3	t
1272	2020	7	U	U	1	f	f	1	t
1275	2019	5	U	U	4	t	t	1	t
1275	2019	5	U	U	4	t	t	3	t
1276	2020	4	F	N	3	f	f	1	t
1276	2020	4	F	N	3	f	t	3	t
1280	2019	3	M	Y	3	f	f	3	t
1280	2019	3	M	Y	3	f	f	1	t
1283	2019	7	M	Y	7	t	f	3	t
1283	2019	7	M	Y	7	t	f	1	t
1289	2019	2	M	U	4	t	f	1	t
1289	2019	2	M	U	4	t	f	3	t
1292	2018	7	F	N	4	f	f	3	t
1292	2018	7	F	N	4	f	f	1	t
1295	2020	4	U	N	7	f	f	1	t
1295	2020	4	U	N	7	f	f	3	t
1311	2019	7	F	Y	5	f	f	1	t
1311	2019	7	F	Y	5	f	t	3	t
1313	2020	3	M	N	4	t	f	1	t
1313	2020	3	M	N	4	t	f	3	t
1314	2018	7	F	U	7	f	t	3	t
1314	2018	7	F	U	7	f	t	1	t
1315	2018	6	F	U	6	f	f	1	t
1315	2018	6	F	U	6	f	f	3	t
1329	2018	2	M	Y	3	f	f	3	t
1329	2018	2	M	Y	3	f	f	1	t
1336	2018	6	F	N	1	f	f	1	t
1336	2018	6	F	N	1	f	f	3	t
1337	2020	2	F	U	7	t	f	1	t
1337	2020	2	F	U	7	t	t	3	t
1338	2019	5	M	U	7	t	f	1	t
1338	2019	5	M	U	7	t	f	3	t
1354	2018	1	F	N	6	f	t	3	t
1354	2018	1	F	N	6	f	f	1	t
1355	2019	5	F	Y	7	f	f	1	t
1355	2019	5	F	Y	7	f	f	3	t
1356	2018	4	M	U	6	f	f	3	t
1356	2018	4	M	U	6	f	f	1	t
1358	2018	3	F	N	5	f	f	1	t
1358	2018	3	F	N	5	f	f	3	t
1360	2018	1	F	N	3	f	f	1	t
1360	2018	1	F	N	3	f	f	3	t
1364	2019	2	U	U	1	f	f	1	t
1364	2019	2	U	U	1	f	t	3	t
1366	2020	1	M	U	1	f	f	1	t
1366	2020	1	M	U	1	f	f	3	t
1367	2020	4	M	U	3	f	f	3	t
1367	2020	4	M	U	3	f	f	1	t
1368	2019	3	M	N	7	f	t	3	t
1368	2019	3	M	N	7	f	f	1	t
1372	2019	3	F	Y	2	f	f	1	t
1372	2019	3	F	Y	2	f	t	3	t
1373	2019	7	U	U	4	f	f	3	t
1373	2019	7	U	U	4	f	f	1	t
1379	2018	1	U	Y	1	t	t	1	t
1379	2018	1	U	Y	1	t	f	3	t
1384	2020	1	F	Y	2	f	f	3	t
1384	2020	1	F	Y	2	f	f	1	t
1388	2018	7	M	N	3	f	f	3	t
1388	2018	7	M	N	3	f	f	1	t
1390	2019	3	U	U	4	f	f	1	t
1390	2019	3	U	U	4	f	f	3	t
1399	2019	5	F	U	3	f	f	3	t
1399	2019	5	F	U	3	f	f	1	t
1401	2018	4	M	U	7	f	f	3	t
1401	2018	4	M	U	7	f	f	1	t
1402	2018	6	M	Y	5	f	t	3	t
1402	2018	6	M	Y	5	f	f	1	t
1404	2019	3	F	U	5	f	f	3	t
1404	2019	3	F	U	5	f	f	1	t
1405	2020	2	M	U	2	f	t	3	t
1405	2020	2	M	U	2	f	f	1	t
1412	2019	3	M	N	6	f	t	1	t
1412	2019	3	M	N	6	f	t	3	t
1414	2019	2	U	U	2	f	f	3	t
1414	2019	2	U	U	2	f	f	1	t
1418	2019	1	M	N	2	f	f	3	t
1418	2019	1	M	N	2	f	f	1	t
1428	2018	7	U	Y	2	f	f	1	t
1428	2018	7	U	Y	2	f	f	3	t
1429	2020	3	F	N	2	f	t	3	t
1429	2020	3	F	N	2	f	f	1	t
1435	2020	6	F	U	4	f	t	3	t
1435	2020	6	F	U	4	f	f	1	t
1438	2019	7	F	N	2	f	f	3	t
1438	2019	7	F	N	2	f	f	1	t
1439	2018	6	U	N	6	f	f	3	t
1439	2018	6	U	N	6	f	f	1	t
1447	2020	6	U	Y	4	t	f	3	t
1447	2020	6	U	Y	4	t	f	1	t
1449	2018	5	M	Y	5	t	f	1	t
1449	2018	5	M	Y	5	t	f	3	t
1450	2020	6	U	Y	6	f	f	3	t
1450	2020	6	U	Y	6	f	t	1	t
1453	2019	2	M	U	3	f	t	1	t
1453	2019	2	M	U	3	f	t	3	t
1460	2019	2	M	Y	6	f	f	1	t
1460	2019	2	M	Y	6	f	t	3	t
1462	2020	7	F	U	1	t	f	1	t
1462	2020	7	F	U	1	t	f	3	t
1464	2019	2	U	N	5	t	f	1	t
1464	2019	2	U	N	5	t	t	3	t
1468	2020	6	U	U	2	f	f	1	t
1468	2020	6	U	U	2	f	f	3	t
1472	2020	7	M	N	3	f	f	3	t
1472	2020	7	M	N	3	f	f	1	t
1473	2019	4	U	Y	6	t	f	3	t
1473	2019	4	U	Y	6	t	f	1	t
1477	2018	2	M	N	2	f	f	1	t
1477	2018	2	M	N	2	f	f	3	t
1479	2018	7	F	Y	3	f	t	3	t
1479	2018	7	F	Y	3	f	f	1	t
1489	2018	4	U	U	1	t	f	3	t
1489	2018	4	U	U	1	t	f	1	t
1491	2018	4	F	U	5	f	f	1	t
1491	2018	4	F	U	5	f	f	3	t
1496	2018	7	M	N	6	t	f	1	t
1496	2018	7	M	N	6	t	f	3	t
\.


--
-- Data for Name: patient_exclusion; Type: TABLE DATA; Schema: public; Owner: jennie
--

COPY public.patient_exclusion (patid, numerator, denom_excl, site_id) FROM stdin;
0	0	0	1
1	0	0	1
2	0	0	1
3	1	0	1
4	0	0	1
5	0	0	1
6	0	0	1
7	0	0	1
8	0	1	1
9	0	0	1
10	1	0	1
11	0	0	1
12	1	0	1
13	0	0	1
14	0	0	1
15	0	0	1
16	0	0	1
17	1	0	1
18	1	0	1
19	0	0	1
20	1	0	1
21	0	0	1
22	1	0	1
23	0	0	1
24	0	0	1
25	1	0	1
26	0	0	1
27	0	1	1
28	0	0	1
29	0	0	1
30	0	0	1
31	0	0	1
32	0	0	1
33	0	1	1
34	0	0	1
35	0	0	1
36	1	0	1
37	1	0	1
38	1	0	1
39	0	0	1
40	0	0	1
41	0	0	1
42	1	0	1
43	0	0	1
44	0	0	1
45	0	0	1
46	0	0	1
47	1	0	1
48	0	0	1
49	0	0	1
50	1	0	1
51	0	0	1
52	1	0	1
53	1	0	1
54	1	0	1
55	0	0	1
56	0	0	1
57	0	0	1
58	0	1	1
59	0	0	1
60	1	0	1
61	1	0	1
62	1	0	1
63	1	0	1
64	0	0	1
65	1	0	1
66	0	0	1
67	1	0	1
68	0	0	1
69	1	0	1
70	0	0	1
71	0	0	1
72	0	0	1
73	1	0	1
74	1	0	1
75	0	0	1
76	0	0	1
77	1	0	1
78	1	1	1
79	0	0	1
80	0	0	1
81	0	0	1
82	0	1	1
83	0	0	1
84	0	0	1
85	0	0	1
86	0	0	1
87	0	0	1
88	0	0	1
89	0	0	1
90	0	0	1
91	0	0	1
92	0	0	1
93	0	1	1
94	0	0	1
95	0	0	1
96	0	1	1
97	0	0	1
98	0	0	1
99	0	1	1
0	0	0	2
2	0	0	2
3	1	1	2
5	0	0	2
6	0	1	2
7	0	0	2
9	0	0	2
11	0	1	2
12	1	1	2
14	0	1	2
15	0	1	2
18	1	1	2
19	0	0	2
21	0	0	2
24	0	1	2
25	1	0	2
27	0	0	2
29	0	0	2
30	0	0	2
31	0	0	2
32	0	1	2
33	0	1	2
35	0	0	2
36	1	0	2
38	1	0	2
41	0	0	2
44	0	0	2
48	0	0	2
53	1	1	2
54	1	0	2
55	0	0	2
57	0	1	2
59	0	1	2
60	1	0	2
61	1	1	2
62	1	1	2
64	0	0	2
67	1	1	2
68	0	1	2
70	0	0	2
73	1	0	2
76	0	1	2
78	1	1	2
79	0	0	2
80	0	1	2
82	0	0	2
83	0	0	2
87	0	0	2
91	0	1	2
92	0	0	2
93	0	1	2
97	0	1	2
98	0	1	2
99	0	0	2
100	0	0	2
101	0	0	2
102	1	0	2
103	0	0	2
104	0	1	2
105	1	0	2
106	1	0	2
107	1	0	2
108	0	0	2
109	0	0	2
110	0	0	2
111	0	0	2
112	0	0	2
113	1	0	2
114	0	0	2
115	1	0	2
116	0	1	2
117	0	0	2
118	0	1	2
119	0	0	2
120	0	0	2
121	1	0	2
122	1	0	2
123	0	0	2
124	0	0	2
125	0	0	2
126	0	0	2
127	0	1	2
128	0	0	2
129	1	1	2
130	0	0	2
131	0	0	2
132	0	0	2
133	0	0	2
134	0	0	2
135	0	0	2
136	0	0	2
137	1	0	2
138	0	0	2
139	0	0	2
140	0	1	2
141	0	0	2
142	0	0	2
143	1	0	2
144	0	0	2
145	0	0	2
\.


--
-- PostgreSQL database dump complete
--

