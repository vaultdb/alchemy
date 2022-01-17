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
0	2019	2	F	U	5	f	f	1	f
6	2019	4	U	N	1	f	f	1	f
14	2018	2	F	Y	7	f	f	1	f
22	2018	6	U	Y	1	f	f	1	f
23	2018	3	M	N	6	f	f	1	f
24	2018	7	F	U	4	f	f	1	f
30	2018	7	U	Y	1	t	f	1	f
32	2020	3	M	U	6	t	f	1	f
34	2019	4	M	N	2	f	t	1	f
37	2019	7	M	Y	2	f	f	1	f
40	2018	2	M	U	3	f	f	1	f
41	2019	5	F	U	2	t	f	1	f
44	2018	1	M	N	6	f	f	1	f
47	2018	6	M	N	4	f	f	1	f
49	2020	7	M	Y	5	f	f	1	f
52	2020	2	U	N	3	t	f	1	f
54	2018	1	U	N	5	f	f	1	f
56	2018	6	U	N	7	f	f	1	f
60	2018	6	F	N	1	f	f	1	f
64	2018	3	U	Y	5	t	f	1	f
68	2019	2	M	Y	2	f	f	1	f
69	2020	1	U	N	3	f	f	1	f
75	2020	2	M	Y	2	f	f	1	f
77	2019	2	M	N	3	t	f	1	f
83	2020	1	U	U	3	f	f	1	f
87	2018	6	F	N	6	t	f	1	f
92	2018	7	F	U	3	f	f	1	f
99	2018	7	M	N	4	f	f	1	f
115	2019	6	U	U	2	f	f	1	f
120	2019	4	U	N	1	f	t	1	f
127	2020	1	M	U	7	f	f	1	f
128	2019	4	M	U	2	f	f	1	f
130	2020	6	M	Y	1	f	f	1	f
135	2019	3	F	U	4	f	t	1	f
141	2019	2	F	U	1	f	f	1	f
144	2019	5	M	N	3	f	f	1	f
147	2020	6	U	N	6	t	t	1	f
149	2020	2	M	Y	6	f	f	1	f
152	2020	6	M	N	7	f	f	1	f
159	2019	2	U	U	2	f	f	1	f
164	2018	6	M	U	7	f	f	1	f
165	2020	6	M	N	2	t	f	1	f
173	2019	7	F	U	7	t	f	1	f
176	2018	6	U	Y	1	f	f	1	f
177	2020	2	M	Y	5	t	t	1	f
178	2019	1	F	N	7	f	f	1	f
180	2018	7	F	Y	3	t	f	1	f
187	2018	3	F	Y	4	t	f	1	f
192	2018	4	M	Y	3	f	f	1	f
193	2020	2	U	U	3	f	f	1	f
201	2019	2	U	N	6	f	f	1	f
205	2018	4	U	U	6	t	f	1	f
208	2019	6	U	U	3	f	f	1	f
209	2019	4	U	N	5	f	f	1	f
212	2019	1	M	Y	6	f	f	1	f
213	2020	7	M	Y	1	f	f	1	f
215	2020	7	M	U	6	f	f	1	f
224	2020	5	F	N	1	f	t	1	f
225	2019	2	U	N	1	t	f	1	f
226	2018	3	F	N	3	f	f	1	f
232	2019	3	M	N	6	f	f	1	f
240	2019	1	M	U	6	f	f	1	f
241	2020	6	U	N	6	f	f	1	f
247	2020	4	U	N	2	f	f	1	f
253	2020	3	F	U	2	f	t	1	f
254	2020	6	M	U	6	f	f	1	f
255	2019	4	M	N	3	f	f	1	f
257	2018	1	F	N	5	f	f	1	f
264	2018	1	M	N	1	f	f	1	f
269	2019	2	M	U	6	f	f	1	f
270	2019	7	U	N	7	t	f	1	f
276	2020	6	M	Y	6	f	f	1	f
279	2020	3	F	N	3	f	f	1	f
287	2018	3	U	U	3	f	f	1	f
289	2019	1	F	Y	1	f	f	1	f
291	2020	2	F	Y	1	f	f	1	f
292	2018	2	U	Y	7	t	f	1	f
293	2018	4	M	Y	1	t	f	1	f
295	2019	2	U	N	2	f	f	1	f
296	2020	1	U	N	3	f	f	1	f
297	2020	6	M	Y	3	t	f	1	f
300	2019	6	M	N	6	f	f	1	f
301	2018	7	U	Y	5	f	f	1	f
302	2019	2	M	U	4	f	f	1	f
309	2018	7	F	Y	3	f	f	1	f
310	2019	7	U	Y	4	f	f	1	f
315	2019	7	F	Y	5	f	f	1	f
318	2019	7	F	Y	3	t	t	1	f
320	2019	6	F	Y	2	f	f	1	f
323	2020	3	M	Y	1	f	t	1	f
329	2019	3	U	N	1	f	f	1	f
330	2018	6	M	N	3	t	f	1	f
333	2020	7	M	N	4	t	f	1	f
335	2019	1	M	U	3	t	f	1	f
336	2020	5	F	Y	4	f	f	1	f
338	2020	6	F	U	4	t	f	1	f
342	2020	6	U	Y	4	f	f	1	f
344	2019	5	U	N	3	f	f	1	f
349	2020	7	F	U	6	f	f	1	f
350	2020	7	U	U	1	f	f	1	f
351	2018	5	U	N	7	f	f	1	f
354	2018	4	U	N	1	f	f	1	f
355	2020	1	M	Y	1	f	f	1	f
357	2019	6	F	Y	1	f	f	1	f
360	2018	7	M	U	7	f	t	1	f
361	2018	3	U	N	4	f	f	1	f
362	2018	1	F	N	1	f	f	1	f
364	2019	4	F	U	4	f	f	1	f
365	2020	3	F	N	2	f	f	1	f
372	2019	6	M	U	4	t	f	1	f
373	2018	3	M	N	5	f	f	1	f
376	2018	6	U	U	1	f	f	1	f
379	2019	5	M	U	3	t	f	1	f
380	2019	4	F	N	7	f	f	1	f
382	2020	1	U	Y	6	f	f	1	f
387	2020	2	M	Y	5	f	f	1	f
388	2018	4	M	Y	7	f	f	1	f
399	2018	6	M	U	5	f	f	1	f
401	2020	2	U	N	3	f	f	1	f
404	2019	3	U	N	1	t	f	1	f
406	2019	7	M	Y	3	f	f	1	f
407	2018	6	F	N	3	f	f	1	f
408	2020	6	F	N	1	f	f	1	f
410	2018	1	F	Y	5	f	f	1	f
414	2020	2	M	U	7	f	f	1	f
416	2020	4	F	U	2	t	f	1	f
417	2019	3	M	Y	7	t	f	1	f
418	2020	3	U	Y	3	f	f	1	f
419	2019	1	U	N	7	f	f	1	f
422	2020	6	U	U	7	f	f	1	f
423	2020	1	F	U	2	f	f	1	f
432	2018	2	U	Y	3	f	f	1	f
435	2018	5	M	N	4	f	f	1	f
436	2019	3	F	Y	2	f	f	1	f
437	2020	6	M	U	1	t	f	1	f
438	2020	4	M	Y	6	f	f	1	f
439	2018	5	F	N	7	f	f	1	f
441	2018	7	F	U	5	f	f	1	f
443	2020	1	F	Y	6	f	f	1	f
447	2019	2	M	N	4	t	f	1	f
453	2019	6	M	U	5	f	f	1	f
458	2020	1	M	N	5	f	t	1	f
462	2019	2	M	Y	1	f	t	1	f
464	2018	4	F	U	6	t	f	1	f
465	2018	2	U	N	5	f	f	1	f
466	2018	7	F	N	7	f	f	1	f
468	2019	1	U	N	4	f	f	1	f
470	2018	2	F	U	7	f	f	1	f
472	2018	6	F	Y	2	f	f	1	f
473	2019	4	M	U	5	f	f	1	f
475	2018	7	F	Y	6	f	f	1	f
480	2020	1	M	N	5	t	f	1	f
481	2018	2	M	U	1	f	f	1	f
484	2019	5	F	N	5	t	f	1	f
487	2019	4	M	N	1	t	f	1	f
488	2018	6	M	U	7	f	f	1	f
496	2018	6	U	Y	5	f	f	1	f
499	2020	4	U	Y	2	t	t	1	f
500	2018	3	U	U	4	t	f	1	f
503	2020	7	M	N	3	f	f	1	f
510	2018	5	U	U	7	f	f	1	f
514	2018	1	F	U	4	t	f	1	f
518	2019	2	F	U	7	f	f	1	f
529	2019	7	F	N	6	f	f	1	f
533	2019	7	F	U	2	f	f	1	f
534	2019	3	M	N	3	f	f	1	f
538	2020	5	U	Y	1	t	f	1	f
540	2020	2	F	Y	7	t	f	1	f
541	2020	1	M	N	2	f	f	1	f
542	2020	3	M	N	1	f	f	1	f
544	2020	3	M	N	4	t	f	1	f
546	2018	3	F	N	7	t	f	1	f
547	2019	1	U	Y	4	f	t	1	f
548	2019	4	F	Y	6	f	f	1	f
550	2018	4	U	Y	3	f	f	1	f
551	2020	6	F	Y	2	f	f	1	f
562	2019	6	M	U	4	t	f	1	f
563	2019	5	M	Y	3	f	f	1	f
564	2019	7	M	U	6	t	f	1	f
566	2019	3	U	N	1	t	f	1	f
567	2018	4	U	U	5	f	f	1	f
568	2018	1	M	N	7	t	t	1	f
571	2019	7	F	Y	7	f	f	1	f
573	2020	1	M	Y	1	f	f	1	f
575	2020	6	U	N	5	t	f	1	f
576	2020	4	F	Y	7	f	f	1	f
577	2020	5	M	N	7	f	f	1	f
582	2019	3	U	N	2	t	f	1	f
584	2019	3	U	Y	6	f	t	1	f
586	2020	4	M	Y	4	f	f	1	f
589	2019	1	F	N	5	f	f	1	f
590	2020	4	U	Y	3	f	f	1	f
593	2019	1	U	U	3	f	f	1	f
596	2020	7	F	U	4	f	f	1	f
600	2019	3	M	N	5	f	f	1	f
603	2018	1	F	N	1	f	f	1	f
604	2020	6	M	N	6	f	f	1	f
608	2020	2	U	Y	6	f	f	1	f
609	2020	7	M	U	7	f	f	1	f
610	2019	3	M	N	7	f	f	1	f
611	2018	1	M	N	7	f	f	1	f
615	2018	5	F	Y	6	f	f	1	f
618	2020	3	F	U	4	f	f	1	f
624	2019	2	M	U	6	f	f	1	f
626	2019	4	F	Y	6	f	f	1	f
629	2019	7	F	Y	4	f	f	1	f
630	2019	3	U	U	7	f	f	1	f
631	2018	7	F	Y	2	f	f	1	f
632	2018	4	F	Y	5	f	f	1	f
633	2019	5	F	U	2	f	f	1	f
635	2018	7	U	U	3	f	f	1	f
642	2018	7	M	Y	5	f	f	1	f
644	2020	3	U	U	5	f	f	1	f
645	2020	6	U	U	3	f	f	1	f
647	2018	1	U	Y	6	t	f	1	f
648	2020	2	U	Y	5	f	t	1	f
651	2019	3	F	N	6	t	f	1	f
656	2019	1	U	Y	4	f	f	1	f
658	2020	1	M	Y	3	f	f	1	f
662	2018	5	F	Y	1	f	f	1	f
666	2020	6	U	Y	6	t	f	1	f
667	2019	3	U	Y	6	f	f	1	f
670	2020	5	F	N	4	f	f	1	f
672	2020	7	M	Y	3	f	f	1	f
673	2019	4	F	Y	6	f	t	1	f
676	2019	1	U	N	1	t	f	1	f
677	2019	6	M	U	1	f	f	1	f
679	2020	3	M	N	4	f	f	1	f
688	2019	1	M	U	2	t	f	1	f
689	2018	7	F	U	2	f	t	1	f
694	2018	2	F	U	2	t	f	1	f
695	2018	4	F	U	7	f	f	1	f
697	2018	3	F	U	2	f	f	1	f
699	2020	6	M	U	1	f	f	1	f
707	2020	1	U	Y	7	f	f	1	f
710	2019	4	F	Y	2	f	f	1	f
711	2019	7	U	U	4	f	f	1	f
713	2018	6	F	Y	7	f	t	1	f
714	2019	2	U	U	1	f	f	1	f
716	2019	3	U	N	7	f	f	1	f
722	2018	6	U	Y	6	f	t	1	f
727	2019	1	U	N	5	f	f	1	f
735	2019	1	F	Y	6	f	f	1	f
736	2019	2	U	Y	2	f	f	1	f
739	2020	3	M	Y	1	f	f	1	f
748	2018	7	U	Y	6	f	f	1	f
755	2019	1	F	U	4	f	t	1	f
757	2019	1	M	Y	5	t	f	1	f
759	2019	5	F	U	3	f	t	1	f
760	2020	4	U	N	3	f	f	1	f
764	2018	7	U	N	2	f	t	1	f
765	2018	7	M	N	4	f	f	1	f
767	2018	5	F	N	4	f	f	1	f
768	2020	4	U	Y	6	f	f	1	f
769	2019	3	U	N	2	f	f	1	f
780	2018	5	M	Y	7	f	t	1	f
781	2019	2	F	Y	7	f	f	1	f
782	2020	3	F	U	3	t	f	1	f
784	2020	1	U	U	4	f	f	1	f
785	2020	7	M	Y	4	f	f	1	f
786	2020	3	M	N	3	f	f	1	f
788	2020	6	U	U	3	f	f	1	f
792	2020	6	M	U	3	f	f	1	f
793	2020	5	F	U	4	f	f	1	f
798	2018	3	F	U	3	f	f	1	f
802	2020	4	M	N	2	f	f	1	f
803	2019	7	F	Y	1	t	f	1	f
807	2019	6	U	Y	3	f	f	1	f
809	2020	4	F	U	7	t	f	1	f
811	2019	3	U	U	4	f	f	1	f
816	2019	3	U	U	2	f	f	1	f
826	2019	4	F	U	7	f	f	1	f
827	2019	2	F	U	5	f	f	1	f
829	2018	6	U	N	7	t	f	1	f
831	2020	4	F	N	5	f	f	1	f
833	2018	2	F	Y	5	t	f	1	f
837	2019	7	M	U	1	f	f	1	f
839	2020	3	F	U	3	f	f	1	f
851	2020	2	U	Y	2	f	f	1	f
852	2019	1	U	Y	7	t	f	1	f
856	2019	1	F	Y	1	f	f	1	f
862	2020	5	U	N	1	f	t	1	f
864	2018	2	F	N	3	f	f	1	f
865	2020	3	M	U	6	t	f	1	f
868	2018	1	F	Y	6	f	f	1	f
869	2018	2	F	Y	1	t	f	1	f
870	2018	4	M	Y	1	f	f	1	f
871	2018	7	M	N	2	f	f	1	f
873	2020	2	U	Y	5	f	f	1	f
875	2019	6	U	Y	7	f	f	1	f
881	2018	1	M	N	6	t	f	1	f
882	2018	2	F	U	5	f	f	1	f
884	2018	5	U	Y	3	f	f	1	f
889	2020	3	F	Y	5	t	f	1	f
890	2020	2	U	N	7	f	f	1	f
893	2018	5	F	Y	3	f	f	1	f
894	2018	7	F	U	2	f	t	1	f
896	2019	4	U	U	5	f	f	1	f
900	2020	1	M	N	5	t	f	1	f
901	2020	2	F	Y	6	f	f	1	f
903	2018	5	F	U	3	f	f	1	f
904	2019	3	F	U	2	f	f	1	f
905	2020	1	M	U	4	f	f	1	f
909	2018	5	M	N	5	f	f	1	f
912	2019	4	M	Y	6	f	t	1	f
914	2020	7	U	U	6	t	f	1	f
918	2019	4	U	U	6	t	f	1	f
919	2020	6	U	Y	5	t	f	1	f
924	2019	2	F	N	3	f	f	1	f
925	2019	3	U	N	4	f	f	1	f
926	2020	2	M	Y	7	f	t	1	f
927	2020	7	F	U	3	f	f	1	f
934	2018	1	U	Y	3	f	f	1	f
938	2020	6	F	N	1	t	f	1	f
940	2020	2	F	N	1	f	f	1	f
943	2018	4	U	U	2	f	f	1	f
949	2019	3	U	U	7	t	f	1	f
950	2020	6	F	U	3	t	f	1	f
960	2019	7	F	U	1	f	f	1	f
964	2020	1	M	U	2	t	f	1	f
967	2018	1	M	N	1	f	f	1	f
971	2019	5	F	Y	7	t	f	1	f
973	2020	4	M	Y	1	f	f	1	f
974	2020	2	M	Y	3	f	f	1	f
977	2018	7	F	U	4	f	f	1	f
981	2020	3	F	U	4	f	f	1	f
985	2020	4	F	N	7	f	f	1	f
986	2020	2	F	Y	4	t	f	1	f
988	2019	6	F	Y	1	f	f	1	f
990	2018	7	M	U	1	f	f	1	f
992	2018	1	F	U	7	f	f	1	f
994	2019	1	U	N	4	f	f	1	f
995	2019	5	F	N	7	f	f	1	f
997	2020	1	F	N	5	t	f	1	f
1001	2018	5	M	N	6	t	f	1	f
1002	2019	4	U	Y	4	t	f	1	f
1005	2019	1	F	U	1	f	t	1	f
1006	2019	1	F	U	2	f	f	1	f
1009	2018	3	U	Y	2	f	f	1	f
1011	2019	3	M	Y	5	f	f	1	f
1013	2020	4	U	U	2	t	f	1	f
1015	2018	2	U	N	5	f	t	1	f
1016	2018	3	F	U	4	f	f	1	f
1017	2020	3	U	Y	3	f	f	1	f
1018	2019	3	F	N	7	t	f	1	f
1019	2018	3	U	Y	5	f	t	1	f
1020	2018	4	U	N	4	f	t	1	f
1021	2019	5	F	Y	6	f	f	1	f
1023	2020	4	U	N	1	f	f	1	f
1024	2019	2	F	U	5	f	f	1	f
1025	2018	3	F	N	1	f	f	1	f
1026	2020	3	U	U	5	f	f	1	f
1028	2019	6	M	Y	5	f	f	1	f
1029	2019	2	M	N	5	f	f	1	f
1030	2019	7	F	N	5	f	f	1	f
1031	2019	4	M	N	5	f	f	1	f
1033	2018	2	F	Y	2	f	f	1	f
1034	2018	7	F	Y	7	f	f	1	f
1035	2019	4	U	Y	1	t	f	1	f
1036	2018	3	F	N	6	f	f	1	f
1038	2020	6	U	N	7	f	t	1	f
1039	2018	2	M	Y	4	f	f	1	f
1040	2019	7	M	U	3	f	f	1	f
1041	2018	3	M	N	3	f	f	1	f
1042	2020	4	U	Y	7	t	f	1	f
1044	2020	4	F	Y	6	t	f	1	f
1045	2020	3	M	N	5	t	f	1	f
1046	2018	7	U	U	2	t	f	1	f
1047	2018	1	M	Y	7	t	f	1	f
1049	2019	7	F	N	3	t	f	1	f
1053	2020	4	U	N	2	f	f	1	f
1054	2020	1	M	Y	7	f	f	1	f
1055	2018	7	M	N	7	f	f	1	f
1058	2019	7	F	N	1	f	f	1	f
1059	2020	3	F	Y	3	f	f	1	f
1060	2020	2	M	N	4	f	f	1	f
1062	2020	1	F	N	7	f	f	1	f
1063	2020	2	F	N	7	t	f	1	f
1065	2020	3	M	Y	7	f	f	1	f
1066	2020	1	U	Y	6	f	f	1	f
1067	2019	2	U	U	7	f	f	1	f
1068	2020	3	U	N	7	f	f	1	f
1069	2020	7	M	U	6	f	f	1	f
1071	2018	4	M	N	5	t	f	1	f
1072	2018	7	F	N	3	f	f	1	f
1073	2018	5	F	N	1	f	f	1	f
1074	2019	4	M	U	2	f	f	1	f
1075	2018	2	M	N	2	f	f	1	f
1077	2018	7	M	Y	2	t	f	1	f
1078	2020	4	M	N	1	f	f	1	f
1080	2020	4	M	U	2	f	f	1	f
1081	2019	4	F	N	6	f	f	1	f
1084	2019	2	U	U	7	f	f	1	f
1085	2020	6	F	U	1	f	f	1	f
1086	2018	3	F	Y	3	t	f	1	f
1087	2019	5	F	N	4	t	f	1	f
1089	2019	4	U	Y	6	t	f	1	f
1091	2020	1	M	U	7	f	f	1	f
1092	2019	5	M	N	5	t	f	1	f
1093	2019	1	M	N	2	f	f	1	f
1094	2018	2	F	N	7	f	f	1	f
1095	2018	7	U	U	1	f	t	1	f
1096	2018	2	F	U	6	f	f	1	f
1099	2018	6	M	U	4	f	f	1	f
1100	2020	6	F	U	2	t	f	1	f
1101	2020	6	M	Y	4	f	t	1	f
1102	2019	5	M	U	3	t	f	1	f
1104	2019	7	F	Y	6	f	f	1	f
1106	2019	4	M	U	3	f	f	1	f
1107	2020	3	U	Y	4	f	f	1	f
1111	2018	7	M	N	5	f	f	1	f
1112	2020	1	U	N	5	t	f	1	f
1115	2020	6	U	N	1	f	f	1	f
1116	2019	1	F	Y	5	f	f	1	f
1118	2018	2	F	Y	6	t	f	1	f
1119	2018	5	M	Y	4	f	f	1	f
1120	2019	7	M	Y	5	t	f	1	f
1123	2018	4	M	Y	5	f	f	1	f
1125	2020	6	M	U	5	f	f	1	f
1126	2018	4	F	N	1	f	f	1	f
1128	2019	3	F	N	3	f	f	1	f
1131	2018	2	F	Y	1	f	f	1	f
1133	2019	2	M	Y	4	f	f	1	f
1136	2019	5	F	Y	4	f	f	1	f
1137	2020	5	M	U	3	f	f	1	f
1139	2020	1	U	Y	3	f	f	1	f
1141	2018	6	M	N	5	f	f	1	f
1144	2018	2	U	Y	3	f	f	1	f
1145	2018	4	U	N	1	f	f	1	f
1146	2018	2	M	U	5	f	f	1	f
1147	2020	4	U	U	1	f	f	1	f
1149	2019	3	U	U	3	f	f	1	f
1150	2018	1	F	U	6	f	f	1	f
1152	2020	4	M	U	3	f	f	1	f
1153	2019	2	M	U	4	f	f	1	f
1154	2018	5	M	U	1	t	f	1	f
1155	2018	1	M	U	6	f	f	1	f
1158	2020	1	M	U	3	f	f	1	f
1159	2019	4	M	N	5	f	f	1	f
1160	2019	6	F	Y	2	t	f	1	f
1161	2019	3	F	Y	4	t	f	1	f
1163	2018	1	U	N	4	t	f	1	f
1164	2020	6	U	U	1	t	f	1	f
1166	2018	7	U	N	3	t	t	1	f
1167	2018	3	U	N	4	t	f	1	f
1168	2020	3	M	Y	7	f	f	1	f
1169	2019	7	M	U	7	f	f	1	f
1170	2018	7	M	U	7	f	f	1	f
1171	2020	3	U	Y	6	t	t	1	f
1172	2020	5	U	Y	6	t	f	1	f
1173	2018	6	F	N	4	f	f	1	f
1176	2018	4	U	N	3	f	t	1	f
1179	2020	6	M	U	5	f	t	1	f
1181	2018	5	M	N	6	f	t	1	f
1182	2020	6	M	U	3	f	f	1	f
1184	2018	4	F	N	6	f	f	1	f
1186	2019	7	M	U	4	t	f	1	f
1187	2018	6	U	Y	7	t	f	1	f
1192	2020	1	F	U	4	f	f	1	f
1195	2019	5	M	Y	3	f	t	1	f
1196	2020	1	F	N	3	f	f	1	f
1197	2020	4	M	Y	5	f	f	1	f
1198	2019	7	F	N	5	f	f	1	f
1199	2019	5	F	U	5	t	f	1	f
1200	2018	4	M	Y	7	f	f	1	f
1202	2018	5	F	U	3	f	f	1	f
1204	2020	2	F	U	3	t	f	1	f
1206	2019	3	U	N	1	f	f	1	f
1208	2019	7	M	U	3	f	t	1	f
1209	2018	2	F	U	7	f	f	1	f
1210	2018	4	U	Y	1	f	f	1	f
1212	2018	4	M	U	3	f	f	1	f
1213	2018	3	F	Y	7	f	f	1	f
1214	2020	1	M	Y	7	f	f	1	f
1215	2019	2	U	N	1	f	f	1	f
1216	2020	4	U	N	6	f	t	1	f
1217	2018	5	U	U	6	f	f	1	f
1219	2019	5	F	N	3	t	f	1	f
1221	2018	5	U	Y	2	f	f	1	f
1222	2020	7	U	U	5	t	f	1	f
1223	2019	2	U	U	5	f	f	1	f
1225	2020	3	M	Y	3	f	f	1	f
1226	2018	1	F	N	2	f	f	1	f
1227	2018	3	F	N	2	f	f	1	f
1228	2019	2	F	U	4	t	f	1	f
1231	2020	2	U	Y	5	f	f	1	f
1232	2019	1	U	N	1	f	f	1	f
1233	2018	4	F	N	4	t	f	1	f
1234	2019	3	M	Y	6	f	f	1	f
1236	2020	6	M	N	4	t	f	1	f
1237	2019	5	F	N	2	f	f	1	f
1238	2018	5	M	N	2	f	f	1	f
1239	2019	4	F	Y	7	t	f	1	f
1243	2019	3	U	U	4	f	f	1	f
1244	2018	5	F	U	1	f	f	1	f
1245	2019	5	F	N	6	t	f	1	f
1248	2018	5	F	U	2	f	f	1	f
1250	2020	3	U	U	2	t	f	1	f
1251	2020	1	M	N	2	f	f	1	f
1252	2019	3	M	U	4	f	t	1	f
1254	2020	1	F	U	2	t	t	1	f
1255	2019	7	M	Y	2	f	f	1	f
1258	2019	1	M	Y	2	t	f	1	f
1263	2019	4	M	Y	1	f	f	1	f
1264	2019	1	F	Y	5	f	f	1	f
1265	2018	1	U	N	1	t	f	1	f
1267	2020	3	F	Y	1	f	f	1	f
1268	2020	2	U	U	3	f	f	1	f
1270	2020	5	M	N	1	f	f	1	f
1272	2019	2	M	U	5	f	f	1	f
1273	2020	6	M	U	2	t	f	1	f
1275	2019	3	M	N	4	f	f	1	f
1276	2018	6	U	U	4	f	f	1	f
1278	2020	4	M	Y	1	f	f	1	f
1279	2019	6	U	N	1	f	f	1	f
1280	2019	4	F	U	2	f	f	1	f
1283	2018	4	U	U	1	f	f	1	f
1284	2020	4	F	Y	2	t	t	1	f
1285	2018	6	M	U	3	f	f	1	f
1286	2018	7	U	Y	6	f	f	1	f
1287	2020	3	F	Y	6	f	f	1	f
1288	2019	1	F	Y	3	t	f	1	f
1289	2020	6	U	U	3	f	f	1	f
1290	2019	4	U	Y	3	f	f	1	f
1292	2019	7	F	Y	1	f	f	1	f
1293	2020	7	U	N	7	f	f	1	f
1294	2018	4	F	N	6	f	f	1	f
1295	2018	6	U	U	1	f	f	1	f
1297	2019	7	U	U	2	t	f	1	f
1299	2020	7	U	U	3	t	f	1	f
1302	2018	3	F	Y	1	f	t	1	f
1303	2020	3	F	N	6	f	f	1	f
1304	2018	6	M	N	6	f	f	1	f
1306	2020	7	F	N	2	f	f	1	f
1307	2018	7	M	Y	3	f	f	1	f
1308	2019	4	U	N	6	t	f	1	f
1309	2018	5	M	U	3	f	t	1	f
1311	2018	3	U	Y	7	t	f	1	f
1312	2020	4	U	U	2	f	f	1	f
1313	2018	5	M	N	7	f	f	1	f
1314	2020	5	M	N	3	f	f	1	f
1315	2018	1	F	U	1	f	f	1	f
1316	2018	3	M	U	7	f	t	1	f
1317	2018	1	U	U	4	f	f	1	f
1318	2019	1	M	N	3	f	f	1	f
1319	2018	1	U	Y	5	t	f	1	f
1320	2019	4	M	U	7	f	f	1	f
1321	2020	2	U	N	2	f	f	1	f
1322	2018	6	U	Y	2	t	f	1	f
1323	2019	1	M	U	7	f	f	1	f
1326	2020	1	U	N	2	f	f	1	f
1328	2018	7	U	Y	3	f	t	1	f
1329	2020	2	U	Y	2	f	f	1	f
1330	2020	3	F	U	2	f	t	1	f
1331	2019	7	F	U	1	t	f	1	f
1332	2019	7	M	Y	1	t	t	1	f
1333	2020	5	F	Y	3	f	t	1	f
1337	2018	4	F	Y	6	f	f	1	f
1338	2018	4	M	U	4	f	f	1	f
1339	2019	3	M	U	1	f	f	1	f
1340	2020	5	F	U	5	f	t	1	f
1343	2018	6	M	Y	6	f	t	1	f
1344	2019	4	U	U	4	f	t	1	f
1345	2018	5	F	N	4	f	f	1	f
1346	2018	3	U	U	4	f	f	1	f
1347	2020	2	M	N	7	f	f	1	f
1348	2020	5	M	Y	1	t	f	1	f
1350	2018	4	F	U	4	f	f	1	f
1351	2019	6	U	Y	6	f	f	1	f
1354	2019	7	M	N	1	t	f	1	f
1355	2018	1	U	U	2	t	t	1	f
1357	2019	5	M	Y	7	f	f	1	f
1358	2018	2	M	N	4	t	f	1	f
1360	2018	1	U	Y	6	t	f	1	f
1361	2019	4	F	U	2	f	f	1	f
1362	2019	7	F	N	7	f	f	1	f
1364	2018	5	U	N	3	f	f	1	f
1365	2020	7	M	U	7	t	t	1	f
1368	2018	5	M	N	5	f	f	1	f
1372	2020	7	M	U	2	f	f	1	f
1373	2018	2	M	U	4	f	f	1	f
1375	2018	6	F	N	5	f	f	1	f
1376	2018	2	F	Y	7	t	f	1	f
1377	2018	7	F	N	2	f	f	1	f
1379	2019	1	F	U	4	t	f	1	f
1380	2019	6	U	U	5	f	f	1	f
1381	2019	7	U	N	5	f	f	1	f
1382	2018	2	U	N	4	t	f	1	f
1385	2019	5	U	N	2	f	f	1	f
1386	2018	6	M	N	4	f	f	1	f
1388	2018	5	U	N	4	f	t	1	f
1390	2019	5	M	U	1	t	f	1	f
1392	2019	1	U	N	3	t	f	1	f
1394	2020	5	F	Y	2	f	f	1	f
1395	2018	4	U	Y	7	f	f	1	f
1396	2018	2	U	N	1	f	f	1	f
1398	2018	5	U	N	6	f	f	1	f
1399	2018	5	F	N	4	f	f	1	f
1401	2018	5	F	Y	3	f	f	1	f
1404	2018	2	M	U	5	f	f	1	f
1405	2018	2	F	Y	6	f	f	1	f
1407	2020	4	M	Y	7	f	f	1	f
1408	2019	6	F	N	5	f	f	1	f
1409	2019	7	F	Y	4	f	f	1	f
1410	2020	1	U	Y	3	f	f	1	f
1413	2018	1	M	N	6	f	t	1	f
1416	2018	1	U	N	4	f	f	1	f
1418	2020	6	M	U	2	t	f	1	f
1421	2020	6	U	N	7	f	t	1	f
1422	2019	2	U	N	6	t	f	1	f
1424	2020	4	F	U	1	f	t	1	f
1426	2019	6	U	U	5	t	f	1	f
1427	2019	4	F	U	4	f	t	1	f
1428	2020	5	F	N	7	f	f	1	f
1429	2020	1	F	Y	4	f	f	1	f
1431	2018	3	M	N	7	f	f	1	f
1432	2020	5	F	N	6	f	f	1	f
1434	2019	3	M	U	7	f	f	1	f
1435	2018	3	M	Y	7	f	f	1	f
1437	2020	5	F	U	7	f	f	1	f
1438	2020	4	U	N	5	t	f	1	f
1439	2020	2	F	U	3	t	f	1	f
1440	2019	5	F	N	2	t	f	1	f
1441	2019	5	M	U	1	f	f	1	f
1443	2018	1	M	N	3	t	f	1	f
1444	2018	4	U	U	2	t	f	1	f
1446	2020	5	F	U	6	f	t	1	f
1449	2019	2	U	Y	4	f	f	1	f
1450	2018	6	F	Y	7	t	t	1	f
1451	2020	4	U	N	1	f	f	1	f
1454	2019	6	F	U	1	f	f	1	f
1455	2018	4	M	U	1	t	f	1	f
1456	2020	2	M	U	2	f	f	1	f
1457	2020	6	F	U	3	f	f	1	f
1460	2018	6	M	N	7	f	f	1	f
1461	2019	5	M	Y	1	f	f	1	f
1463	2018	4	F	N	3	f	f	1	f
1465	2020	4	M	U	7	f	f	1	f
1466	2018	6	M	Y	5	f	f	1	f
1469	2020	7	U	Y	5	f	f	1	f
1471	2019	6	F	N	6	f	f	1	f
1473	2018	7	F	U	4	f	f	1	f
1475	2020	4	F	U	3	f	f	1	f
1476	2018	6	F	Y	6	f	t	1	f
1477	2018	2	M	N	3	f	f	1	f
1478	2020	6	U	U	6	t	f	1	f
1480	2018	6	F	Y	5	f	f	1	f
1482	2019	2	U	Y	7	f	f	1	f
1483	2020	7	U	U	4	f	f	1	f
1486	2018	3	M	N	1	f	f	1	f
1487	2020	2	F	N	6	t	f	1	f
1488	2019	2	F	N	4	f	f	1	f
1489	2018	7	M	N	2	f	f	1	f
1490	2020	2	U	N	7	f	f	1	f
1491	2019	7	U	N	7	f	f	1	f
1492	2020	6	U	U	6	f	f	1	f
1493	2018	1	F	N	1	t	f	1	f
1494	2020	7	U	U	5	f	f	1	f
1495	2018	1	U	U	2	f	f	1	f
1496	2020	3	F	U	5	f	f	1	f
1498	2018	7	M	U	2	f	t	1	f
1500	2020	5	U	U	2	f	t	3	f
1501	2018	4	F	U	4	f	t	3	f
1502	2020	2	U	Y	5	f	f	3	f
1503	2018	5	F	N	3	t	f	3	f
1504	2018	3	U	Y	7	f	f	3	f
1505	2018	1	M	U	6	f	f	3	f
1506	2018	4	M	Y	7	t	f	3	f
1507	2018	6	M	U	5	f	f	3	f
1508	2019	6	F	Y	5	f	f	3	f
1509	2018	7	M	N	6	f	f	3	f
1510	2020	7	F	U	7	f	f	3	f
1511	2018	5	F	U	2	t	f	3	f
1512	2020	7	M	Y	5	f	f	3	f
1513	2019	6	M	U	1	f	f	3	f
1514	2020	5	U	N	5	f	f	3	f
1515	2018	4	U	U	7	t	f	3	f
1516	2018	6	F	N	5	t	f	3	f
1517	2020	1	F	U	1	f	f	3	f
1518	2018	6	M	Y	6	t	f	3	f
1519	2018	6	F	Y	3	f	f	3	f
1520	2018	5	M	U	4	f	f	3	f
1521	2018	3	M	Y	1	f	f	3	f
1522	2020	3	M	N	2	t	f	3	f
1523	2019	2	F	U	7	f	t	3	f
1524	2019	1	F	N	6	t	f	3	f
1525	2020	4	F	Y	5	f	f	3	f
1526	2020	1	U	Y	7	f	f	3	f
1527	2020	3	M	N	6	t	f	3	f
1528	2020	2	M	N	1	f	f	3	f
1529	2019	2	M	U	2	f	f	3	f
1530	2020	1	F	Y	4	f	f	3	f
1531	2020	4	F	Y	5	t	f	3	f
1532	2018	3	U	N	7	t	t	3	f
1533	2018	6	F	N	5	f	f	3	f
1534	2018	4	F	N	2	f	f	3	f
1535	2020	5	F	Y	2	f	f	3	f
1536	2019	7	F	Y	7	f	f	3	f
1537	2018	6	F	U	6	f	f	3	f
1538	2020	5	F	N	5	t	f	3	f
1539	2018	1	M	U	6	f	f	3	f
1540	2019	1	M	N	6	t	f	3	f
1541	2020	7	M	Y	4	f	f	3	f
1542	2018	1	F	N	4	f	f	3	f
1543	2018	4	M	Y	7	f	f	3	f
1544	2020	6	F	N	5	f	f	3	f
1545	2019	7	F	Y	4	t	f	3	f
1546	2019	1	M	N	6	f	f	3	f
1547	2019	3	U	N	6	f	f	3	f
1548	2020	2	F	U	2	f	f	3	f
1549	2018	5	M	N	5	f	f	3	f
1550	2018	7	M	Y	6	f	f	3	f
1551	2019	2	U	Y	2	f	f	3	f
1552	2018	2	M	Y	5	f	f	3	f
1553	2020	4	F	Y	5	f	f	3	f
1554	2019	6	M	Y	2	f	f	3	f
1555	2019	2	F	U	4	t	f	3	f
1556	2020	2	F	U	3	t	f	3	f
1557	2020	2	F	N	5	f	f	3	f
1558	2018	2	U	U	2	t	f	3	f
1559	2020	1	F	U	5	f	f	3	f
1560	2020	3	M	U	1	f	f	3	f
1561	2020	3	M	Y	7	f	f	3	f
1562	2020	3	M	N	1	f	t	3	f
1563	2018	2	M	N	2	t	t	3	f
1564	2020	4	F	U	1	f	f	3	f
1565	2020	3	F	N	6	t	f	3	f
1566	2018	5	F	N	1	f	t	3	f
1567	2019	4	F	Y	3	t	f	3	f
1568	2019	1	M	U	2	t	t	3	f
1569	2020	5	M	Y	3	f	t	3	f
1570	2020	1	M	N	3	f	t	3	f
1571	2019	3	F	Y	5	f	f	3	f
1572	2018	2	M	N	6	f	f	3	f
1573	2019	7	F	N	5	f	f	3	f
1574	2020	2	F	Y	5	f	f	3	f
1575	2018	7	M	Y	1	f	f	3	f
1576	2020	6	F	U	6	t	f	3	f
1577	2020	5	F	U	3	f	f	3	f
1578	2020	4	M	U	5	t	f	3	f
1579	2019	6	F	U	4	f	f	3	f
1580	2019	5	U	N	7	t	f	3	f
1581	2018	1	M	N	4	f	f	3	f
1582	2020	4	M	Y	5	f	f	3	f
1583	2019	3	M	U	6	t	f	3	f
1584	2020	3	U	Y	6	f	f	3	f
1585	2018	5	F	Y	6	f	f	3	f
1586	2018	4	F	Y	6	t	f	3	f
1587	2018	5	F	N	6	t	t	3	f
1588	2019	7	F	Y	2	f	f	3	f
1589	2019	4	F	N	1	t	f	3	f
1590	2018	2	M	N	6	t	f	3	f
1591	2018	5	U	U	6	f	f	3	f
1592	2020	4	M	Y	3	f	f	3	f
1593	2018	3	F	N	5	t	f	3	f
1594	2020	6	F	Y	4	f	f	3	f
1595	2018	6	M	U	4	f	f	3	f
1596	2019	3	F	U	6	f	f	3	f
1597	2020	3	U	Y	1	f	f	3	f
1598	2020	5	M	Y	3	f	f	3	f
1599	2020	7	M	Y	2	f	f	3	f
1600	2018	5	F	N	5	f	f	3	f
1601	2019	4	M	N	7	f	f	3	f
1602	2018	6	U	U	6	f	f	3	f
1603	2019	3	U	Y	2	f	f	3	f
1604	2019	7	F	Y	2	t	f	3	f
1605	2019	3	U	N	6	t	f	3	f
1606	2018	7	F	U	6	f	f	3	f
1607	2019	6	F	Y	6	f	f	3	f
1608	2019	5	F	Y	6	f	f	3	f
1609	2019	6	M	N	1	f	f	3	f
1610	2018	4	F	N	5	f	f	3	f
1611	2020	5	U	U	6	f	f	3	f
1612	2019	4	M	U	4	t	f	3	f
1613	2018	5	M	Y	3	t	f	3	f
1614	2018	7	F	Y	2	f	f	3	f
1615	2018	7	F	Y	1	f	f	3	f
1616	2019	4	M	N	2	t	f	3	f
1617	2018	5	M	N	3	f	f	3	f
1618	2018	4	U	N	7	f	f	3	f
1619	2018	5	F	N	2	f	f	3	f
1620	2020	2	U	N	3	t	f	3	f
1621	2018	4	F	U	4	f	t	3	f
1622	2019	6	U	N	7	f	f	3	f
1623	2018	1	M	Y	7	f	t	3	f
1624	2018	7	U	Y	5	f	f	3	f
1625	2020	2	M	U	7	f	f	3	f
1626	2020	7	M	Y	1	f	f	3	f
1627	2020	6	M	N	4	f	f	3	f
1628	2018	5	U	N	4	f	f	3	f
1629	2018	4	M	U	5	f	f	3	f
1630	2019	5	F	N	3	t	f	3	f
1631	2018	3	M	Y	6	f	f	3	f
1632	2018	6	M	N	3	f	f	3	f
1633	2020	6	F	N	4	f	f	3	f
1634	2020	4	U	N	1	t	f	3	f
1635	2019	1	M	U	2	t	f	3	f
1636	2020	6	M	Y	3	f	f	3	f
1637	2020	6	F	N	3	f	f	3	f
1638	2020	1	U	U	4	f	f	3	f
1639	2018	7	U	U	5	f	f	3	f
1640	2020	3	M	U	1	f	f	3	f
1641	2018	2	F	U	6	t	f	3	f
1642	2020	1	F	N	3	f	f	3	f
1643	2020	1	F	Y	1	f	f	3	f
1644	2020	3	M	N	6	t	f	3	f
1645	2020	5	M	N	1	f	f	3	f
1646	2018	2	F	Y	4	f	t	3	f
1647	2020	3	F	N	1	f	f	3	f
1648	2018	6	M	N	7	f	t	3	f
1649	2020	4	M	N	2	f	t	3	f
1650	2019	1	F	N	1	f	f	3	f
1651	2020	6	F	Y	2	t	f	3	f
1652	2019	7	U	N	7	f	f	3	f
1653	2018	6	U	Y	6	t	t	3	f
1654	2020	2	U	U	7	f	f	3	f
1655	2018	5	M	N	4	f	f	3	f
1656	2019	3	M	N	2	f	f	3	f
1657	2020	1	U	N	6	f	f	3	f
1658	2019	7	U	N	2	t	f	3	f
1659	2018	7	F	N	2	f	t	3	f
1660	2019	4	F	U	4	f	f	3	f
1661	2019	7	M	U	4	f	f	3	f
1662	2018	7	M	N	3	f	f	3	f
1663	2019	1	F	Y	2	t	f	3	f
1664	2019	3	M	N	1	f	f	3	f
1665	2020	7	M	U	7	t	f	3	f
1666	2019	4	U	N	4	f	f	3	f
1667	2018	3	M	Y	5	f	t	3	f
1668	2018	7	U	Y	3	f	f	3	f
1669	2020	3	U	U	5	f	f	3	f
1670	2019	3	U	N	2	f	f	3	f
1671	2019	4	F	Y	4	f	f	3	f
1672	2020	3	F	Y	5	t	f	3	f
1673	2020	7	M	U	5	f	f	3	f
1674	2019	2	F	Y	3	f	f	3	f
1675	2018	4	F	N	7	t	f	3	f
1676	2019	2	U	N	6	f	f	3	f
1677	2018	4	M	N	2	f	f	3	f
1678	2018	2	U	N	3	f	f	3	f
1679	2018	3	F	U	3	f	f	3	f
1680	2020	5	F	N	4	t	f	3	f
1681	2019	7	U	N	5	f	f	3	f
1682	2020	7	M	N	5	f	f	3	f
1683	2019	7	F	Y	3	f	f	3	f
1684	2020	4	F	Y	1	f	f	3	f
1685	2019	1	U	Y	3	f	f	3	f
1686	2018	5	M	Y	6	f	f	3	f
1687	2018	2	U	N	5	f	f	3	f
1688	2020	5	F	U	6	f	f	3	f
1689	2020	3	M	U	1	f	f	3	f
1690	2020	1	F	Y	2	t	f	3	f
1691	2019	5	M	N	3	t	f	3	f
1692	2019	4	M	U	7	t	f	3	f
1693	2019	2	M	U	2	f	f	3	f
1694	2019	2	U	Y	6	f	t	3	f
1695	2019	6	F	Y	2	f	f	3	f
1696	2019	1	M	Y	6	f	f	3	f
1697	2019	1	U	Y	7	f	f	3	f
1698	2019	3	M	Y	1	f	f	3	f
1699	2018	5	M	N	4	f	f	3	f
1700	2020	3	F	N	7	f	f	3	f
1701	2020	5	F	N	4	t	f	3	f
1702	2019	6	M	Y	4	t	f	3	f
1703	2020	5	M	N	2	t	f	3	f
1704	2019	2	F	U	1	f	f	3	f
1705	2018	5	F	Y	4	f	f	3	f
1706	2019	1	U	N	5	f	f	3	f
1707	2018	6	U	U	4	t	f	3	f
1708	2018	3	F	U	3	f	f	3	f
1709	2018	5	U	N	7	t	f	3	f
1710	2019	2	M	U	7	f	f	3	f
1711	2018	1	F	U	1	f	f	3	f
1712	2018	5	M	N	6	f	f	3	f
1713	2019	1	U	U	6	t	t	3	f
1714	2019	6	U	N	1	f	t	3	f
1715	2020	2	U	N	1	t	f	3	f
1716	2020	2	F	N	5	f	t	3	f
1717	2019	3	M	Y	2	f	f	3	f
1718	2018	2	M	U	1	t	f	3	f
1719	2019	7	F	U	2	f	f	3	f
1720	2020	2	U	Y	4	t	f	3	f
1721	2018	1	U	Y	5	f	f	3	f
1722	2020	6	F	N	6	f	f	3	f
1723	2019	6	M	U	1	f	f	3	f
1724	2018	2	F	N	5	f	t	3	f
1725	2018	6	F	U	3	f	f	3	f
1726	2018	7	M	N	3	t	f	3	f
1727	2019	7	U	Y	4	t	f	3	f
1728	2018	1	M	U	2	f	f	3	f
1729	2018	1	U	Y	2	f	f	3	f
1730	2018	3	U	N	4	f	f	3	f
1731	2018	2	F	N	4	f	f	3	f
1732	2020	5	U	U	5	f	f	3	f
1733	2020	1	U	Y	1	f	f	3	f
1734	2019	3	M	N	1	f	f	3	f
1735	2018	3	M	N	7	f	f	3	f
1736	2018	2	U	Y	5	f	f	3	f
1737	2018	4	F	N	2	t	f	3	f
1738	2020	7	M	U	5	f	f	3	f
1739	2020	1	M	N	5	f	f	3	f
1740	2019	5	F	Y	6	f	f	3	f
1741	2020	3	F	U	1	f	f	3	f
1742	2019	5	U	N	2	f	f	3	f
1743	2019	1	F	Y	1	t	f	3	f
1744	2018	5	U	U	1	f	f	3	f
1745	2019	2	U	Y	3	f	f	3	f
1746	2018	3	U	U	2	t	f	3	f
1747	2018	1	F	Y	3	f	f	3	f
1748	2020	2	F	U	5	f	t	3	f
1749	2020	2	F	Y	2	t	f	3	f
1750	2018	7	U	U	4	t	f	3	f
1751	2018	6	U	N	4	f	f	3	f
1752	2020	1	U	Y	2	f	f	3	f
1753	2020	5	F	Y	3	t	t	3	f
1754	2018	4	F	U	5	f	f	3	f
1755	2019	3	M	U	3	f	f	3	f
1756	2018	2	U	N	7	f	f	3	f
1757	2020	5	M	Y	5	f	f	3	f
1758	2018	1	U	N	4	t	f	3	f
1759	2019	4	U	U	5	f	f	3	f
1760	2020	4	U	Y	6	f	f	3	f
1761	2018	3	U	U	1	f	f	3	f
1762	2019	3	M	N	6	f	f	3	f
1763	2020	7	F	U	7	t	f	3	f
1764	2019	2	F	U	2	f	t	3	f
1765	2019	5	M	N	7	f	f	3	f
1766	2019	2	U	U	1	t	f	3	f
1767	2020	4	F	Y	2	t	f	3	f
1768	2018	7	F	U	7	f	f	3	f
1769	2018	4	M	N	7	f	f	3	f
1770	2020	7	F	U	5	f	f	3	f
1771	2018	5	U	N	3	t	f	3	f
1772	2019	5	U	Y	3	f	f	3	f
1773	2018	7	U	U	1	f	f	3	f
1774	2018	1	M	U	6	f	f	3	f
1775	2018	5	U	Y	4	f	f	3	f
1776	2019	3	M	Y	2	f	f	3	f
1777	2018	2	M	N	3	f	f	3	f
1778	2019	3	U	N	7	f	f	3	f
1779	2019	7	U	U	1	f	f	3	f
1780	2018	2	U	Y	6	f	f	3	f
1781	2019	3	M	Y	2	f	f	3	f
1782	2020	1	F	N	5	t	f	3	f
1783	2020	2	M	N	5	t	f	3	f
1784	2019	7	F	Y	3	t	f	3	f
1785	2018	5	F	Y	2	f	f	3	f
1786	2020	3	U	N	5	f	f	3	f
1787	2019	1	U	Y	2	f	f	3	f
1788	2019	2	M	N	7	t	f	3	f
1789	2019	5	F	Y	4	t	f	3	f
1790	2018	2	U	Y	4	f	f	3	f
1791	2018	1	F	Y	3	f	f	3	f
1792	2020	2	U	Y	5	f	f	3	f
1793	2020	4	U	N	6	t	f	3	f
1794	2019	5	U	Y	5	f	f	3	f
1795	2019	2	F	N	4	f	t	3	f
1796	2019	7	F	U	6	f	f	3	f
1797	2018	2	M	N	4	f	t	3	f
1798	2018	6	U	U	7	f	f	3	f
1799	2018	6	U	Y	7	f	f	3	f
1800	2019	2	F	Y	5	t	t	3	f
1801	2020	3	M	N	2	f	f	3	f
1802	2020	7	M	N	7	f	f	3	f
1803	2020	2	F	U	6	f	f	3	f
1804	2019	1	U	Y	6	f	f	3	f
1805	2018	5	M	N	7	f	f	3	f
1806	2018	1	U	Y	5	f	f	3	f
1807	2020	3	F	Y	3	f	t	3	f
1808	2020	2	M	N	5	t	f	3	f
1809	2019	4	U	N	2	f	t	3	f
1810	2018	7	U	Y	5	f	f	3	f
1811	2020	3	M	Y	3	t	t	3	f
1812	2018	6	M	Y	3	f	f	3	f
1813	2018	7	M	Y	7	f	f	3	f
1814	2018	5	U	Y	5	f	f	3	f
1815	2018	3	M	N	5	f	f	3	f
1816	2018	7	M	N	1	t	f	3	f
1817	2020	6	M	Y	7	f	f	3	f
1818	2020	4	F	U	1	f	f	3	f
1819	2018	4	M	U	5	f	f	3	f
1820	2018	4	M	N	2	f	f	3	f
1821	2019	7	U	Y	3	t	f	3	f
1822	2019	6	M	N	3	f	f	3	f
1823	2018	7	U	U	2	f	f	3	f
1824	2020	5	M	N	5	f	f	3	f
1825	2018	6	U	Y	2	f	f	3	f
1826	2020	6	U	U	6	f	f	3	f
1827	2020	1	U	Y	4	f	f	3	f
1828	2018	7	M	N	7	f	f	3	f
1829	2018	1	F	U	4	f	f	3	f
1830	2020	6	U	U	2	f	f	3	f
1831	2018	5	U	N	6	t	f	3	f
1832	2018	6	F	Y	2	f	f	3	f
1833	2020	2	M	U	1	f	f	3	f
1834	2019	6	U	Y	3	f	f	3	f
1835	2020	6	F	N	7	f	f	3	f
1836	2018	1	F	U	4	f	f	3	f
1837	2018	3	U	N	3	f	f	3	f
1838	2019	7	F	U	7	t	t	3	f
1839	2018	2	M	U	3	f	f	3	f
1840	2019	4	U	Y	2	f	f	3	f
1841	2018	5	U	N	5	f	f	3	f
1842	2019	5	U	N	2	t	f	3	f
1843	2020	6	M	U	3	t	f	3	f
1844	2018	2	U	Y	6	f	f	3	f
1845	2018	1	M	Y	6	f	f	3	f
1846	2018	1	U	U	4	t	f	3	f
1847	2019	7	U	N	4	f	f	3	f
1848	2019	4	F	U	2	f	f	3	f
1849	2020	3	M	N	2	f	f	3	f
1850	2019	3	U	Y	2	t	f	3	f
1851	2018	2	U	U	6	f	f	3	f
1852	2019	2	U	U	2	f	f	3	f
1853	2019	5	U	Y	2	f	t	3	f
1854	2018	3	U	U	5	t	f	3	f
1855	2018	5	M	U	3	t	f	3	f
1856	2018	1	F	U	6	f	f	3	f
1857	2018	3	F	U	1	f	f	3	f
1858	2020	4	F	U	5	f	f	3	f
1859	2018	3	F	Y	4	t	f	3	f
1860	2018	3	F	U	1	f	f	3	f
1861	2019	4	U	U	1	f	f	3	f
1862	2018	7	U	Y	6	f	f	3	f
1863	2019	4	U	N	5	t	f	3	f
1864	2019	4	F	Y	7	t	f	3	f
1865	2020	3	U	N	1	t	f	3	f
1866	2019	6	M	Y	4	f	f	3	f
1867	2019	5	U	N	2	t	f	3	f
1868	2019	4	M	U	1	f	f	3	f
1869	2019	4	M	U	4	f	f	3	f
1870	2019	4	M	Y	7	f	f	3	f
1871	2019	5	F	N	7	f	f	3	f
1872	2020	2	F	Y	7	t	t	3	f
1873	2020	6	M	Y	1	f	f	3	f
1874	2018	1	U	Y	3	f	f	3	f
1875	2019	2	U	U	5	f	f	3	f
1876	2018	3	U	Y	5	f	f	3	f
1877	2019	6	U	N	4	f	t	3	f
1878	2019	3	M	N	7	t	t	3	f
1879	2018	4	F	U	1	f	f	3	f
1880	2018	5	U	N	3	f	f	3	f
1881	2020	5	F	Y	4	t	f	3	f
1882	2019	4	F	Y	6	f	t	3	f
1883	2018	4	F	N	3	f	f	3	f
1884	2018	3	F	Y	2	t	f	3	f
1885	2018	3	U	N	5	f	f	3	f
1886	2019	1	F	U	5	f	f	3	f
1887	2018	2	F	Y	3	f	f	3	f
1888	2019	6	F	N	7	f	f	3	f
1889	2019	6	F	N	6	f	f	3	f
1890	2019	3	U	U	5	t	t	3	f
1891	2019	7	F	N	2	f	f	3	f
1892	2019	1	U	Y	1	f	f	3	f
1893	2018	6	M	U	2	f	f	3	f
1894	2018	6	U	Y	5	f	f	3	f
1895	2019	6	F	Y	3	f	f	3	f
1896	2019	1	F	Y	6	f	f	3	f
1897	2018	3	M	U	3	t	f	3	f
1898	2020	1	U	Y	1	f	t	3	f
1899	2019	3	F	Y	6	f	f	3	f
1900	2019	2	M	Y	3	f	f	3	f
1901	2020	3	F	Y	6	t	f	3	f
1902	2020	7	U	U	1	f	f	3	f
1903	2019	3	M	U	7	t	f	3	f
1904	2020	5	F	U	2	t	f	3	f
1905	2020	6	M	U	1	t	f	3	f
1906	2018	2	F	Y	2	t	f	3	f
1907	2018	5	F	U	1	f	f	3	f
1908	2020	5	U	U	5	f	f	3	f
1909	2018	5	M	Y	7	f	f	3	f
1910	2018	7	F	U	4	f	f	3	f
1911	2020	5	U	U	1	f	f	3	f
1912	2019	6	U	U	5	f	f	3	f
1913	2019	7	U	Y	6	f	f	3	f
1914	2020	4	U	U	3	f	f	3	f
1915	2019	4	F	U	6	f	t	3	f
1916	2020	3	M	N	6	t	f	3	f
1917	2020	2	U	N	6	f	f	3	f
1918	2018	6	M	Y	7	t	f	3	f
1919	2019	5	M	Y	3	f	f	3	f
1920	2020	7	F	N	2	f	f	3	f
1921	2020	6	F	N	6	f	f	3	f
1922	2019	7	U	U	1	t	f	3	f
1923	2020	5	F	Y	2	f	f	3	f
1924	2020	1	M	Y	1	f	t	3	f
1925	2018	6	U	U	5	t	f	3	f
1926	2019	3	F	N	7	t	f	3	f
1927	2019	1	F	U	7	f	f	3	f
1928	2018	7	F	Y	3	f	f	3	f
1929	2018	4	U	N	6	f	f	3	f
1930	2018	4	M	N	6	f	f	3	f
1931	2020	5	M	N	7	t	f	3	f
1932	2019	2	M	N	5	f	f	3	f
1933	2019	2	M	N	5	t	f	3	f
1934	2019	2	U	Y	7	t	f	3	f
1935	2020	2	F	N	1	f	f	3	f
1936	2020	1	M	Y	2	f	f	3	f
1937	2019	3	M	U	4	t	f	3	f
1938	2020	6	U	N	5	f	t	3	f
1939	2019	4	F	Y	5	f	f	3	f
1940	2019	6	F	N	6	f	t	3	f
1941	2019	2	F	Y	1	f	f	3	f
1942	2020	1	M	Y	5	f	t	3	f
1943	2020	2	F	U	2	f	f	3	f
1944	2018	2	U	U	1	f	f	3	f
1945	2018	5	F	N	2	f	f	3	f
1946	2020	5	F	U	4	f	f	3	f
1947	2018	1	F	Y	7	f	f	3	f
1948	2018	1	F	Y	2	t	f	3	f
1949	2018	7	U	N	3	t	f	3	f
1950	2018	7	U	Y	7	f	f	3	f
1951	2020	4	U	N	5	f	f	3	f
1952	2020	4	M	N	2	f	f	3	f
1953	2020	2	F	N	7	f	f	3	f
1954	2020	4	F	U	1	t	f	3	f
1955	2019	1	F	N	6	f	f	3	f
1956	2018	6	F	U	5	f	f	3	f
1957	2018	1	M	U	6	t	t	3	f
1958	2019	4	M	N	5	f	f	3	f
1959	2020	2	U	Y	6	f	f	3	f
1960	2018	1	M	U	3	f	f	3	f
1961	2019	6	U	U	2	f	f	3	f
1962	2019	2	F	Y	5	f	f	3	f
1963	2018	3	U	N	2	f	f	3	f
1964	2020	5	F	N	1	f	f	3	f
1965	2020	6	M	Y	1	f	t	3	f
1966	2020	7	M	U	1	f	t	3	f
1967	2018	6	U	Y	6	f	f	3	f
1968	2019	3	U	U	2	t	f	3	f
1969	2018	3	U	U	4	f	f	3	f
1970	2018	5	M	U	6	t	f	3	f
1971	2019	2	M	Y	1	f	f	3	f
1972	2020	7	M	U	3	f	f	3	f
1973	2019	3	F	N	7	t	f	3	f
1974	2019	4	M	N	6	f	f	3	f
1975	2020	6	U	N	7	f	f	3	f
1976	2019	1	F	Y	7	f	f	3	f
1977	2019	5	U	Y	6	f	t	3	f
1978	2018	1	M	N	2	f	f	3	f
1979	2020	7	M	Y	3	f	f	3	f
1980	2019	6	M	Y	6	f	f	3	f
1981	2020	6	M	U	1	t	f	3	f
1982	2019	2	F	U	6	f	f	3	f
1983	2018	5	U	N	5	f	f	3	f
1984	2020	3	U	Y	7	f	f	3	f
1985	2019	1	M	U	1	f	f	3	f
1	2020	3	F	U	6	t	f	1	t
1	2020	3	F	U	6	t	f	2	t
2	2019	4	F	Y	7	f	f	1	t
2	2019	4	F	Y	7	f	f	3	t
3	2018	3	U	N	1	f	f	2	t
3	2018	3	U	N	1	f	f	1	t
3	2018	3	U	N	1	f	f	3	t
4	2019	4	F	Y	7	f	t	2	t
4	2019	4	F	Y	7	f	f	1	t
5	2020	7	F	Y	3	f	f	1	t
5	2020	7	F	Y	3	f	t	3	t
7	2018	7	F	U	1	f	t	1	t
7	2018	7	F	U	1	f	t	3	t
8	2019	3	M	Y	6	f	f	2	t
8	2019	3	M	Y	6	f	f	1	t
9	2018	6	U	Y	5	f	t	2	t
9	2018	6	U	Y	5	f	f	3	t
9	2018	6	U	Y	5	f	f	1	t
10	2020	3	U	Y	1	t	t	2	t
10	2020	3	U	Y	1	t	f	1	t
11	2019	7	U	Y	3	f	f	1	t
11	2019	7	U	Y	3	f	f	3	t
12	2018	4	M	N	5	f	f	1	t
12	2018	4	M	N	5	f	f	2	t
13	2020	2	M	U	4	t	t	2	t
13	2020	2	M	U	4	t	t	1	t
15	2019	7	M	Y	2	f	t	2	t
15	2019	7	M	Y	2	f	f	1	t
16	2018	4	M	Y	6	f	t	3	t
16	2018	4	M	Y	6	f	f	1	t
17	2018	3	U	Y	2	f	t	2	t
17	2018	3	U	Y	2	f	t	1	t
18	2020	1	M	U	6	t	f	1	t
18	2020	1	M	U	6	t	t	2	t
19	2019	3	U	N	2	t	f	3	t
19	2019	3	U	N	2	t	f	1	t
19	2019	3	U	N	2	t	f	2	t
20	2018	6	F	Y	3	t	f	1	t
20	2018	6	F	Y	3	t	t	3	t
20	2018	6	F	Y	3	t	t	2	t
21	2018	3	F	N	4	f	f	2	t
21	2018	3	F	N	4	f	t	3	t
21	2018	3	F	N	4	f	f	1	t
25	2019	7	M	U	3	f	t	2	t
25	2019	7	M	U	3	f	f	1	t
26	2019	7	M	Y	5	f	f	1	t
26	2019	7	M	Y	5	f	f	3	t
27	2019	3	U	N	6	f	f	3	t
27	2019	3	U	N	6	f	f	1	t
28	2020	2	M	U	6	f	t	2	t
28	2020	2	M	U	6	f	f	1	t
29	2019	6	M	U	4	f	t	3	t
29	2019	6	M	U	4	f	t	2	t
29	2019	6	M	U	4	f	f	1	t
31	2020	4	U	N	2	f	f	3	t
31	2020	4	U	N	2	f	f	1	t
33	2019	5	U	N	4	t	f	1	t
33	2019	5	U	N	4	t	t	2	t
35	2020	3	U	U	1	f	t	2	t
35	2020	3	U	U	1	f	t	3	t
35	2020	3	U	U	1	f	f	1	t
36	2018	6	U	N	4	t	f	1	t
36	2018	6	U	N	4	t	t	2	t
38	2019	7	M	Y	5	f	f	1	t
38	2019	7	M	Y	5	f	t	2	t
39	2018	1	F	Y	5	f	t	3	t
39	2018	1	F	Y	5	f	f	1	t
42	2019	3	U	Y	5	f	f	1	t
42	2019	3	U	Y	5	f	t	2	t
43	2019	3	U	N	6	f	f	1	t
43	2019	3	U	N	6	f	t	2	t
45	2018	3	F	N	7	f	f	2	t
45	2018	3	F	N	7	f	f	1	t
45	2018	3	F	N	7	f	f	3	t
46	2020	2	U	N	6	f	f	1	t
46	2020	2	U	N	6	f	t	2	t
46	2020	2	U	N	6	f	f	3	t
48	2018	1	F	U	5	t	f	1	t
48	2018	1	F	U	5	t	f	3	t
50	2019	7	U	N	5	t	f	2	t
50	2019	7	U	N	5	t	f	1	t
51	2019	2	U	Y	3	t	t	3	t
51	2019	2	U	Y	3	t	f	1	t
51	2019	2	U	Y	3	t	f	2	t
53	2019	7	M	U	7	t	t	3	t
53	2019	7	M	U	7	t	f	1	t
55	2020	7	M	U	4	f	f	3	t
55	2020	7	M	U	4	f	f	1	t
57	2018	7	U	U	7	t	t	2	t
57	2018	7	U	U	7	t	f	1	t
58	2020	4	M	N	2	f	t	2	t
58	2020	4	M	N	2	f	f	3	t
58	2020	4	M	N	2	f	f	1	t
59	2018	1	F	N	1	f	f	1	t
59	2018	1	F	N	1	f	t	2	t
61	2019	6	M	Y	5	f	f	1	t
61	2019	6	M	Y	5	f	t	2	t
62	2020	4	U	Y	5	t	t	2	t
62	2020	4	U	Y	5	t	f	1	t
63	2020	4	F	U	1	f	t	2	t
63	2020	4	F	U	1	f	t	3	t
63	2020	4	F	U	1	f	f	1	t
65	2020	4	M	N	1	f	f	3	t
65	2020	4	M	N	1	f	t	1	t
66	2019	6	F	U	3	t	f	3	t
66	2019	6	F	U	3	t	f	2	t
66	2019	6	F	U	3	t	f	1	t
67	2020	6	M	U	5	f	t	1	t
67	2020	6	M	U	5	f	t	3	t
67	2020	6	M	U	5	f	t	2	t
70	2018	6	U	N	6	f	t	2	t
70	2018	6	U	N	6	f	f	1	t
71	2018	4	F	U	3	f	f	1	t
71	2018	4	F	U	3	f	t	3	t
72	2019	7	M	Y	7	f	f	3	t
72	2019	7	M	Y	7	f	f	1	t
73	2019	3	M	Y	7	f	f	1	t
73	2019	3	M	Y	7	f	t	2	t
74	2019	1	M	U	4	f	f	1	t
74	2019	1	M	U	4	f	t	2	t
76	2020	1	F	N	6	f	f	1	t
76	2020	1	F	N	6	f	f	3	t
78	2019	3	U	U	3	t	f	1	t
78	2019	3	U	U	3	t	f	3	t
78	2019	3	U	U	3	t	t	2	t
79	2020	2	M	N	2	f	t	2	t
79	2020	2	M	N	2	f	f	1	t
80	2018	5	M	N	1	f	t	2	t
80	2018	5	M	N	1	f	f	1	t
81	2020	4	U	N	2	f	t	3	t
81	2020	4	U	N	2	f	f	1	t
82	2018	2	U	Y	3	f	t	2	t
82	2018	2	U	Y	3	f	f	3	t
82	2018	2	U	Y	3	f	f	1	t
84	2018	4	U	Y	3	f	f	2	t
84	2018	4	U	Y	3	f	f	3	t
84	2018	4	U	Y	3	f	f	1	t
85	2018	5	U	Y	7	f	f	1	t
85	2018	5	U	Y	7	f	f	2	t
85	2018	5	U	Y	7	f	t	3	t
86	2019	3	F	N	2	f	t	3	t
86	2019	3	F	N	2	f	f	1	t
88	2018	1	U	U	6	f	f	1	t
88	2018	1	U	U	6	f	f	2	t
89	2019	1	U	U	4	t	t	2	t
89	2019	1	U	U	4	t	f	1	t
90	2020	1	U	N	1	f	t	3	t
90	2020	1	U	N	1	f	f	1	t
91	2018	3	M	N	5	t	t	2	t
91	2018	3	M	N	5	t	f	1	t
93	2018	4	F	N	4	f	f	1	t
93	2018	4	F	N	4	f	f	3	t
94	2018	6	F	U	3	f	f	3	t
94	2018	6	F	U	3	f	t	1	t
95	2020	5	U	N	3	t	f	3	t
95	2020	5	U	N	3	t	f	1	t
96	2019	2	M	N	1	f	t	1	t
96	2019	2	M	N	1	f	f	3	t
97	2019	7	F	U	4	f	f	1	t
97	2019	7	F	U	4	f	t	2	t
98	2019	5	F	U	4	f	f	2	t
98	2019	5	F	U	4	f	t	1	t
100	2020	4	F	Y	1	f	t	2	t
100	2020	4	F	Y	1	f	f	1	t
101	2020	6	F	U	5	f	f	1	t
101	2020	6	F	U	5	f	f	3	t
101	2020	6	F	U	5	f	f	2	t
102	2019	7	U	Y	6	f	f	2	t
102	2019	7	U	Y	6	f	f	1	t
103	2018	5	M	N	1	f	t	1	t
103	2018	5	M	N	1	f	f	3	t
103	2018	5	M	N	1	f	t	2	t
104	2019	1	U	N	4	f	f	1	t
104	2019	1	U	N	4	f	t	2	t
105	2020	5	U	N	6	f	f	2	t
105	2020	5	U	N	6	f	f	1	t
106	2018	7	U	Y	3	f	f	1	t
106	2018	7	U	Y	3	f	t	3	t
106	2018	7	U	Y	3	f	f	2	t
107	2020	5	M	N	7	f	f	1	t
107	2020	5	M	N	7	f	f	3	t
107	2020	5	M	N	7	f	t	2	t
108	2020	5	U	Y	7	t	t	2	t
108	2020	5	U	Y	7	t	f	1	t
109	2020	3	M	U	7	f	f	2	t
109	2020	3	M	U	7	f	f	1	t
110	2020	5	M	Y	2	f	t	1	t
110	2020	5	M	Y	2	f	t	2	t
110	2020	5	M	Y	2	f	f	3	t
111	2019	6	F	Y	1	f	t	2	t
111	2019	6	F	Y	1	f	f	1	t
112	2018	4	F	Y	4	f	f	1	t
112	2018	4	F	Y	4	f	f	3	t
112	2018	4	F	Y	4	f	t	2	t
113	2020	1	F	N	3	f	f	1	t
113	2020	1	F	N	3	f	t	2	t
114	2019	4	M	U	3	f	f	3	t
114	2019	4	M	U	3	f	t	2	t
114	2019	4	M	U	3	f	f	1	t
116	2020	6	F	U	7	f	f	3	t
116	2020	6	F	U	7	f	f	1	t
117	2019	2	M	N	6	t	f	1	t
117	2019	2	M	N	6	t	f	3	t
117	2019	2	M	N	6	t	t	2	t
118	2020	5	M	U	2	f	t	1	t
118	2020	5	M	U	2	f	f	2	t
119	2020	1	F	U	7	f	t	2	t
119	2020	1	F	U	7	f	f	3	t
119	2020	1	F	U	7	f	f	1	t
121	2020	1	F	Y	5	f	f	3	t
121	2020	1	F	Y	5	f	f	1	t
122	2020	3	U	U	6	f	t	3	t
122	2020	3	U	U	6	f	f	1	t
123	2018	2	F	N	7	f	f	1	t
123	2018	2	F	N	7	f	f	3	t
124	2018	3	F	U	2	f	f	3	t
124	2018	3	F	U	2	f	f	1	t
125	2019	3	U	U	7	f	f	1	t
125	2019	3	U	U	7	f	f	3	t
126	2020	7	F	U	6	t	t	1	t
126	2020	7	F	U	6	t	t	3	t
129	2020	5	F	N	4	f	t	2	t
129	2020	5	F	N	4	f	t	1	t
131	2019	3	F	N	2	f	f	2	t
131	2019	3	F	N	2	f	f	1	t
132	2018	4	U	N	5	t	t	2	t
132	2018	4	U	N	5	t	f	1	t
133	2020	3	U	U	5	f	f	1	t
133	2020	3	U	U	5	f	t	3	t
133	2020	3	U	U	5	f	f	2	t
134	2020	4	U	N	2	f	f	1	t
134	2020	4	U	N	2	f	f	2	t
136	2019	3	U	U	3	t	f	3	t
136	2019	3	U	U	3	t	f	2	t
136	2019	3	U	U	3	t	f	1	t
137	2019	2	F	Y	5	f	f	2	t
137	2019	2	F	Y	5	f	f	1	t
138	2020	3	U	N	4	f	f	1	t
138	2020	3	U	N	4	f	t	2	t
138	2020	3	U	N	4	f	t	3	t
139	2018	5	M	U	5	f	f	3	t
139	2018	5	M	U	5	f	f	1	t
139	2018	5	M	U	5	f	t	2	t
140	2018	4	F	Y	3	f	t	3	t
140	2018	4	F	Y	3	f	f	1	t
142	2019	7	F	U	6	t	f	3	t
142	2019	7	F	U	6	t	f	2	t
142	2019	7	F	U	6	t	f	1	t
143	2019	7	U	N	1	f	f	1	t
143	2019	7	U	N	1	f	f	2	t
145	2019	7	M	Y	7	t	t	2	t
145	2019	7	M	Y	7	t	t	3	t
145	2019	7	M	Y	7	t	f	1	t
146	2020	4	F	U	2	f	f	3	t
146	2020	4	F	U	2	f	f	1	t
148	2019	1	F	N	4	f	f	1	t
148	2019	1	F	N	4	f	t	2	t
150	2018	2	F	N	6	f	f	1	t
150	2018	2	F	N	6	f	t	3	t
150	2018	2	F	N	6	f	f	2	t
151	2019	4	F	U	2	t	t	2	t
151	2019	4	F	U	2	t	f	1	t
153	2018	1	M	U	7	f	t	2	t
153	2018	1	M	U	7	f	t	1	t
154	2019	4	M	Y	5	f	f	1	t
154	2019	4	M	Y	5	f	t	2	t
155	2020	2	M	U	3	f	f	1	t
155	2020	2	M	U	3	f	f	2	t
156	2019	7	M	U	5	f	f	2	t
156	2019	7	M	U	5	f	f	1	t
157	2020	1	M	U	3	f	t	3	t
157	2020	1	M	U	3	f	f	1	t
157	2020	1	M	U	3	f	f	2	t
158	2018	7	M	U	5	t	t	1	t
158	2018	7	M	U	5	t	t	2	t
160	2018	7	F	N	3	f	f	3	t
160	2018	7	F	N	3	f	f	1	t
161	2018	2	F	N	3	f	f	1	t
161	2018	2	F	N	3	f	t	2	t
161	2018	2	F	N	3	f	f	3	t
162	2018	6	U	N	3	f	t	2	t
162	2018	6	U	N	3	f	t	3	t
162	2018	6	U	N	3	f	f	1	t
163	2019	3	M	N	7	f	t	3	t
163	2019	3	M	N	7	f	f	1	t
163	2019	3	M	N	7	f	t	2	t
166	2020	3	F	Y	4	f	t	3	t
166	2020	3	F	Y	4	f	f	1	t
167	2020	6	F	U	4	t	f	3	t
167	2020	6	F	U	4	t	f	1	t
168	2020	1	U	U	7	f	t	3	t
168	2020	1	U	U	7	f	f	1	t
169	2018	5	U	Y	2	f	f	1	t
169	2018	5	U	Y	2	f	t	2	t
170	2020	6	F	N	3	f	f	1	t
170	2020	6	F	N	3	f	t	2	t
171	2020	1	U	U	6	t	f	2	t
171	2020	1	U	U	6	t	f	1	t
172	2019	1	M	U	1	f	f	1	t
172	2019	1	M	U	1	f	t	3	t
174	2019	6	M	U	6	t	t	2	t
174	2019	6	M	U	6	t	f	1	t
175	2020	2	U	U	4	f	f	1	t
175	2020	2	U	U	4	f	t	2	t
179	2019	5	M	N	4	f	t	3	t
179	2019	5	M	N	4	f	f	2	t
179	2019	5	M	N	4	f	t	1	t
181	2018	5	M	U	6	f	f	1	t
181	2018	5	M	U	6	f	f	3	t
181	2018	5	M	U	6	f	f	2	t
182	2020	1	M	U	5	f	t	2	t
182	2020	1	M	U	5	f	f	1	t
183	2020	7	M	Y	7	f	f	1	t
183	2020	7	M	Y	7	f	t	2	t
184	2019	1	U	U	6	t	t	3	t
184	2019	1	U	U	6	t	t	1	t
185	2018	2	U	U	7	f	f	1	t
185	2018	2	U	U	7	f	f	2	t
185	2018	2	U	U	7	f	f	3	t
186	2020	2	M	N	7	f	f	3	t
186	2020	2	M	N	7	f	f	1	t
188	2018	6	F	N	4	f	t	3	t
188	2018	6	F	N	4	f	t	1	t
189	2019	1	F	U	6	t	f	1	t
189	2019	1	F	U	6	t	f	3	t
189	2019	1	F	U	6	t	t	2	t
190	2018	3	M	N	2	f	t	3	t
190	2018	3	M	N	2	f	f	1	t
191	2019	3	U	Y	7	f	t	2	t
191	2019	3	U	Y	7	f	f	1	t
194	2018	2	U	Y	2	f	f	2	t
194	2018	2	U	Y	2	f	f	1	t
195	2019	6	F	N	1	f	f	1	t
195	2019	6	F	N	1	f	f	2	t
196	2019	6	F	Y	3	f	f	1	t
196	2019	6	F	Y	3	f	t	2	t
197	2018	2	U	N	7	f	f	2	t
197	2018	2	U	N	7	f	f	1	t
198	2018	7	F	U	2	t	f	2	t
198	2018	7	F	U	2	t	t	3	t
198	2018	7	F	U	2	t	f	1	t
199	2020	6	M	U	4	f	f	1	t
199	2020	6	M	U	4	f	t	2	t
200	2020	6	U	Y	4	f	f	2	t
200	2020	6	U	Y	4	f	f	1	t
202	2019	2	F	U	1	t	f	1	t
202	2019	2	F	U	1	t	f	2	t
203	2018	7	F	Y	1	f	t	3	t
203	2018	7	F	Y	1	f	t	1	t
203	2018	7	F	Y	1	f	t	2	t
204	2019	4	M	U	7	f	f	1	t
204	2019	4	M	U	7	f	t	2	t
206	2019	3	U	Y	5	f	f	1	t
206	2019	3	U	Y	5	f	t	3	t
207	2019	6	M	U	7	t	f	3	t
207	2019	6	M	U	7	t	f	1	t
207	2019	6	M	U	7	t	f	2	t
210	2019	4	M	U	1	f	f	1	t
210	2019	4	M	U	1	f	t	3	t
211	2018	3	U	Y	3	f	f	3	t
211	2018	3	U	Y	3	f	f	1	t
214	2020	4	F	U	1	t	f	1	t
214	2020	4	F	U	1	t	t	2	t
216	2018	1	U	N	3	t	f	1	t
216	2018	1	U	N	3	t	f	3	t
217	2020	2	F	U	6	f	f	1	t
217	2020	2	F	U	6	f	f	3	t
218	2019	1	M	Y	7	t	f	1	t
218	2019	1	M	Y	7	t	t	2	t
219	2019	4	M	N	1	t	f	1	t
219	2019	4	M	N	1	t	f	3	t
219	2019	4	M	N	1	t	f	2	t
220	2018	6	U	U	3	t	t	2	t
220	2018	6	U	U	3	t	t	1	t
221	2020	5	F	Y	7	t	f	1	t
221	2020	5	F	Y	7	t	t	2	t
222	2020	4	F	N	5	f	f	2	t
222	2020	4	F	N	5	f	f	1	t
223	2020	6	U	N	6	f	t	3	t
223	2020	6	U	N	6	f	f	1	t
227	2018	4	M	U	6	f	f	2	t
227	2018	4	M	U	6	f	t	1	t
228	2019	4	M	Y	5	f	f	2	t
228	2019	4	M	Y	5	f	f	1	t
229	2019	4	F	N	5	f	f	1	t
229	2019	4	F	N	5	f	t	3	t
230	2018	7	F	Y	4	f	f	1	t
230	2018	7	F	Y	4	f	t	3	t
231	2019	7	F	N	1	f	f	3	t
231	2019	7	F	N	1	f	f	1	t
233	2019	4	M	U	3	f	f	1	t
233	2019	4	M	U	3	f	t	2	t
234	2020	1	M	N	5	f	f	1	t
234	2020	1	M	N	5	f	t	2	t
235	2019	1	M	Y	7	f	t	2	t
235	2019	1	M	Y	7	f	f	1	t
236	2018	1	F	U	6	t	t	3	t
236	2018	1	F	U	6	t	t	2	t
236	2018	1	F	U	6	t	f	1	t
237	2019	3	F	Y	5	f	t	3	t
237	2019	3	F	Y	5	f	t	2	t
237	2019	3	F	Y	5	f	t	1	t
238	2018	1	U	N	7	t	t	2	t
238	2018	1	U	N	7	t	f	1	t
238	2018	1	U	N	7	t	t	3	t
239	2020	6	M	Y	4	t	t	3	t
239	2020	6	M	Y	4	t	f	1	t
242	2019	5	M	N	7	f	f	1	t
242	2019	5	M	N	7	f	t	2	t
243	2019	5	U	Y	5	f	t	3	t
243	2019	5	U	Y	5	f	f	1	t
244	2020	1	F	U	3	f	f	1	t
244	2020	1	F	U	3	f	t	2	t
244	2020	1	F	U	3	f	f	3	t
245	2019	5	F	Y	1	t	f	1	t
245	2019	5	F	Y	1	t	t	2	t
245	2019	5	F	Y	1	t	f	3	t
246	2019	6	U	N	4	f	f	3	t
246	2019	6	U	N	4	f	f	1	t
248	2020	3	U	N	3	f	t	2	t
248	2020	3	U	N	3	f	f	1	t
249	2019	4	M	U	4	f	f	3	t
249	2019	4	M	U	4	f	f	2	t
249	2019	4	M	U	4	f	f	1	t
250	2020	2	F	U	2	f	f	2	t
250	2020	2	F	U	2	f	t	3	t
250	2020	2	F	U	2	f	f	1	t
251	2020	6	M	U	1	f	f	3	t
251	2020	6	M	U	1	f	f	1	t
252	2018	5	U	N	4	f	f	1	t
252	2018	5	U	N	4	f	f	2	t
256	2018	6	M	Y	1	f	f	2	t
256	2018	6	M	Y	1	f	f	1	t
258	2020	4	M	N	6	f	f	2	t
258	2020	4	M	N	6	f	f	3	t
258	2020	4	M	N	6	f	f	1	t
259	2020	5	M	U	3	f	f	1	t
259	2020	5	M	U	3	f	t	2	t
260	2020	3	F	N	2	t	f	3	t
260	2020	3	F	N	2	t	f	1	t
260	2020	3	F	N	2	t	t	2	t
261	2020	4	M	Y	6	t	f	1	t
261	2020	4	M	Y	6	t	f	2	t
262	2019	6	U	U	5	f	f	3	t
262	2019	6	U	U	5	f	f	1	t
263	2019	2	U	U	5	t	t	3	t
263	2019	2	U	U	5	t	f	1	t
263	2019	2	U	U	5	t	t	2	t
265	2020	6	U	Y	5	f	t	2	t
265	2020	6	U	Y	5	f	f	1	t
265	2020	6	U	Y	5	f	f	3	t
266	2019	7	U	Y	5	f	f	1	t
266	2019	7	U	Y	5	f	f	2	t
267	2018	1	M	U	5	t	f	3	t
267	2018	1	M	U	5	t	f	1	t
268	2020	6	U	N	2	f	t	3	t
268	2020	6	U	N	2	f	t	1	t
271	2019	7	M	Y	1	f	f	2	t
271	2019	7	M	Y	1	f	f	1	t
272	2019	2	F	N	4	f	f	2	t
272	2019	2	F	N	4	f	f	1	t
273	2020	4	U	U	6	f	f	1	t
273	2020	4	U	U	6	f	f	3	t
274	2018	1	U	Y	5	f	f	1	t
274	2018	1	U	Y	5	f	f	2	t
274	2018	1	U	Y	5	f	t	3	t
275	2019	6	M	Y	5	f	f	2	t
275	2019	6	M	Y	5	f	f	1	t
277	2018	2	U	Y	2	f	f	2	t
277	2018	2	U	Y	2	f	f	1	t
278	2020	6	U	U	1	f	f	3	t
278	2020	6	U	U	1	f	f	1	t
280	2020	7	F	U	4	f	t	2	t
280	2020	7	F	U	4	f	f	1	t
280	2020	7	F	U	4	f	f	3	t
281	2020	5	F	U	7	f	t	2	t
281	2020	5	F	U	7	f	t	3	t
281	2020	5	F	U	7	f	f	1	t
282	2020	2	F	U	7	f	f	1	t
282	2020	2	F	U	7	f	f	3	t
283	2019	6	U	N	3	t	f	3	t
283	2019	6	U	N	3	t	f	1	t
284	2019	6	U	N	2	f	f	3	t
284	2019	6	U	N	2	f	f	1	t
285	2018	4	M	Y	1	f	f	3	t
285	2018	4	M	Y	1	f	f	1	t
286	2018	5	M	U	2	f	t	2	t
286	2018	5	M	U	2	f	f	1	t
288	2018	5	F	Y	3	f	f	2	t
288	2018	5	F	Y	3	f	t	1	t
290	2018	1	M	Y	1	t	t	2	t
290	2018	1	M	Y	1	t	f	1	t
294	2019	4	M	N	6	f	f	3	t
294	2019	4	M	N	6	f	f	1	t
298	2019	2	F	Y	4	f	f	1	t
298	2019	2	F	Y	4	f	f	2	t
299	2020	2	U	N	1	f	f	2	t
299	2020	2	U	N	1	f	f	1	t
299	2020	2	U	N	1	f	t	3	t
303	2019	2	M	U	2	f	f	1	t
303	2019	2	M	U	2	f	t	2	t
304	2018	5	F	U	6	f	t	2	t
304	2018	5	F	U	6	f	f	1	t
304	2018	5	F	U	6	f	f	3	t
305	2019	1	M	N	5	f	f	2	t
305	2019	1	M	N	5	f	f	1	t
306	2018	4	F	Y	3	f	f	1	t
306	2018	4	F	Y	3	f	t	2	t
307	2019	1	F	N	4	f	f	1	t
307	2019	1	F	N	4	f	f	2	t
308	2019	6	U	N	3	f	f	1	t
308	2019	6	U	N	3	f	t	2	t
311	2020	7	M	U	4	f	f	1	t
311	2020	7	M	U	4	f	t	2	t
311	2020	7	M	U	4	f	f	3	t
312	2018	2	M	U	4	f	f	1	t
312	2018	2	M	U	4	f	t	2	t
312	2018	2	M	U	4	f	t	3	t
313	2020	6	M	N	7	f	t	2	t
313	2020	6	M	N	7	f	f	1	t
314	2020	4	U	N	5	f	f	1	t
314	2020	4	U	N	5	f	t	2	t
316	2018	2	U	N	4	f	f	2	t
316	2018	2	U	N	4	f	f	1	t
317	2019	4	M	Y	7	f	f	3	t
317	2019	4	M	Y	7	f	f	1	t
319	2018	4	U	U	5	f	f	1	t
319	2018	4	U	U	5	f	f	2	t
321	2019	2	F	U	3	f	t	1	t
321	2019	2	F	U	3	f	f	2	t
321	2019	2	F	U	3	f	t	3	t
322	2019	1	M	N	1	f	t	3	t
322	2019	1	M	N	1	f	t	1	t
322	2019	1	M	N	1	f	f	2	t
324	2018	4	M	Y	5	f	f	2	t
324	2018	4	M	Y	5	f	f	1	t
325	2018	4	M	N	3	f	t	2	t
325	2018	4	M	N	3	f	t	1	t
326	2018	5	F	Y	5	f	t	2	t
326	2018	5	F	Y	5	f	f	1	t
327	2020	4	U	U	1	t	f	2	t
327	2020	4	U	U	1	t	f	3	t
327	2020	4	U	U	1	t	f	1	t
328	2020	4	M	U	1	t	t	1	t
328	2020	4	M	U	1	t	t	3	t
331	2018	1	F	U	2	t	f	1	t
331	2018	1	F	U	2	t	f	2	t
331	2018	1	F	U	2	t	f	3	t
332	2020	3	M	Y	4	t	f	3	t
332	2020	3	M	Y	4	t	f	1	t
332	2020	3	M	Y	4	t	f	2	t
334	2018	3	F	Y	1	f	t	2	t
334	2018	3	F	Y	1	f	f	1	t
337	2020	1	M	N	5	f	f	1	t
337	2020	1	M	N	5	f	t	2	t
339	2019	2	F	Y	3	f	f	2	t
339	2019	2	F	Y	3	f	f	1	t
339	2019	2	F	Y	3	f	f	3	t
340	2020	4	U	N	2	f	f	2	t
340	2020	4	U	N	2	f	f	1	t
341	2018	3	F	Y	5	t	f	1	t
341	2018	3	F	Y	5	t	f	3	t
341	2018	3	F	Y	5	t	t	2	t
343	2018	7	F	U	5	f	t	2	t
343	2018	7	F	U	5	f	f	1	t
345	2018	4	M	N	5	f	f	1	t
345	2018	4	M	N	5	f	f	2	t
346	2018	5	M	U	2	f	f	1	t
346	2018	5	M	U	2	f	t	3	t
347	2020	5	F	Y	7	f	f	2	t
347	2020	5	F	Y	7	f	f	1	t
347	2020	5	F	Y	7	f	t	3	t
348	2019	2	M	N	4	t	f	2	t
348	2019	2	M	N	4	t	f	1	t
352	2020	3	U	U	3	t	f	3	t
352	2020	3	U	U	3	t	f	2	t
352	2020	3	U	U	3	t	f	1	t
353	2020	7	F	Y	7	f	f	1	t
353	2020	7	F	Y	7	f	f	2	t
353	2020	7	F	Y	7	f	f	3	t
356	2019	6	F	U	7	t	f	3	t
356	2019	6	F	U	7	t	f	1	t
358	2019	2	M	Y	1	t	t	2	t
358	2019	2	M	Y	1	t	f	1	t
359	2018	2	U	Y	6	f	f	1	t
359	2018	2	U	Y	6	f	f	2	t
359	2018	2	U	Y	6	f	t	3	t
363	2019	3	F	Y	4	f	f	1	t
363	2019	3	F	Y	4	f	t	2	t
366	2019	5	M	N	1	t	f	3	t
366	2019	5	M	N	1	t	f	1	t
367	2020	7	F	N	6	f	t	3	t
367	2020	7	F	N	6	f	t	2	t
367	2020	7	F	N	6	f	f	1	t
368	2019	7	F	U	1	t	f	1	t
368	2019	7	F	U	1	t	t	2	t
369	2018	5	F	N	3	f	f	1	t
369	2018	5	F	N	3	f	f	2	t
369	2018	5	F	N	3	f	t	3	t
370	2020	7	F	N	5	f	f	2	t
370	2020	7	F	N	5	f	f	1	t
371	2019	7	M	Y	4	t	f	2	t
371	2019	7	M	Y	4	t	f	1	t
374	2019	2	U	N	4	t	t	3	t
374	2019	2	U	N	4	t	t	2	t
374	2019	2	U	N	4	t	f	1	t
375	2019	5	U	U	1	f	f	1	t
375	2019	5	U	U	1	f	f	2	t
377	2019	5	U	Y	3	f	f	3	t
377	2019	5	U	Y	3	f	f	1	t
378	2018	2	U	U	5	f	f	2	t
378	2018	2	U	U	5	f	f	1	t
381	2018	1	M	Y	2	t	f	3	t
381	2018	1	M	Y	2	t	f	1	t
383	2018	3	U	U	2	f	f	1	t
383	2018	3	U	U	2	f	t	3	t
384	2020	5	U	N	6	f	f	1	t
384	2020	5	U	N	6	f	f	2	t
385	2018	1	U	N	6	f	f	2	t
385	2018	1	U	N	6	f	f	1	t
386	2019	4	F	U	4	f	f	3	t
386	2019	4	F	U	4	f	f	1	t
389	2019	6	U	U	4	f	f	2	t
389	2019	6	U	U	4	f	f	1	t
390	2018	3	F	N	6	f	t	3	t
390	2018	3	F	N	6	f	t	2	t
390	2018	3	F	N	6	f	t	1	t
391	2019	2	M	N	1	f	f	3	t
391	2019	2	M	N	1	f	f	1	t
391	2019	2	M	N	1	f	t	2	t
392	2019	6	M	Y	1	f	f	2	t
392	2019	6	M	Y	1	f	f	1	t
393	2018	5	U	N	1	f	f	2	t
393	2018	5	U	N	1	f	f	1	t
394	2018	1	F	N	7	f	t	2	t
394	2018	1	F	N	7	f	f	1	t
394	2018	1	F	N	7	f	f	3	t
395	2018	2	F	Y	2	f	f	3	t
395	2018	2	F	Y	2	f	f	1	t
395	2018	2	F	Y	2	f	t	2	t
396	2019	6	U	Y	1	f	f	1	t
396	2019	6	U	Y	1	f	f	3	t
397	2018	6	U	U	3	f	f	3	t
397	2018	6	U	U	3	f	f	1	t
398	2020	1	U	Y	5	t	f	2	t
398	2020	1	U	Y	5	t	f	1	t
398	2020	1	U	Y	5	t	f	3	t
400	2018	5	U	N	4	t	t	2	t
400	2018	5	U	N	4	t	f	1	t
402	2018	2	U	N	7	f	t	2	t
402	2018	2	U	N	7	f	f	1	t
403	2018	3	U	N	2	f	f	3	t
403	2018	3	U	N	2	f	f	1	t
405	2020	2	F	U	2	t	f	1	t
405	2020	2	F	U	2	t	f	2	t
409	2020	6	F	Y	4	f	f	1	t
409	2020	6	F	Y	4	f	t	2	t
411	2020	3	F	Y	4	t	f	1	t
411	2020	3	F	Y	4	t	t	2	t
411	2020	3	F	Y	4	t	t	3	t
412	2020	7	M	U	2	t	t	3	t
412	2020	7	M	U	2	t	f	1	t
413	2018	2	F	Y	1	f	f	1	t
413	2018	2	F	Y	1	f	t	2	t
415	2018	3	F	N	3	f	f	3	t
415	2018	3	F	N	3	f	f	1	t
420	2018	5	F	Y	7	f	t	2	t
420	2018	5	F	Y	7	f	f	1	t
421	2018	5	M	Y	6	t	t	2	t
421	2018	5	M	Y	6	t	t	3	t
421	2018	5	M	Y	6	t	f	1	t
424	2018	5	M	Y	7	f	f	2	t
424	2018	5	M	Y	7	f	f	1	t
425	2018	4	M	N	6	f	f	2	t
425	2018	4	M	N	6	f	f	1	t
426	2018	6	M	N	3	f	f	1	t
426	2018	6	M	N	3	f	t	2	t
427	2018	5	F	N	4	t	f	1	t
427	2018	5	F	N	4	t	f	3	t
428	2018	6	M	U	4	f	t	2	t
428	2018	6	M	U	4	f	f	1	t
429	2018	6	M	N	5	t	t	2	t
429	2018	6	M	N	5	t	f	1	t
430	2019	7	F	N	5	t	t	3	t
430	2019	7	F	N	5	t	t	2	t
430	2019	7	F	N	5	t	f	1	t
431	2018	2	F	Y	7	f	f	2	t
431	2018	2	F	Y	7	f	f	1	t
433	2018	4	M	N	7	f	f	1	t
433	2018	4	M	N	7	f	f	2	t
434	2018	3	M	N	4	f	f	1	t
434	2018	3	M	N	4	f	t	3	t
434	2018	3	M	N	4	f	t	2	t
440	2019	2	F	N	5	f	f	1	t
440	2019	2	F	N	5	f	f	3	t
442	2019	2	M	Y	5	f	f	3	t
442	2019	2	M	Y	5	f	t	1	t
444	2018	5	U	Y	5	f	f	1	t
444	2018	5	U	Y	5	f	t	2	t
445	2019	1	U	Y	6	f	f	3	t
445	2019	1	U	Y	6	f	f	1	t
446	2019	7	M	N	3	f	f	1	t
446	2019	7	M	N	3	f	t	2	t
448	2018	1	F	N	7	f	f	3	t
448	2018	1	F	N	7	f	t	2	t
448	2018	1	F	N	7	f	f	1	t
449	2019	5	M	U	6	f	f	1	t
449	2019	5	M	U	6	f	f	2	t
450	2019	2	F	Y	1	f	f	1	t
450	2019	2	F	Y	1	f	f	2	t
450	2019	2	F	Y	1	f	f	3	t
451	2020	7	F	Y	7	f	f	1	t
451	2020	7	F	Y	7	f	f	3	t
451	2020	7	F	Y	7	f	t	2	t
452	2018	6	M	Y	6	t	f	1	t
452	2018	6	M	Y	6	t	t	3	t
454	2018	4	M	U	3	f	f	1	t
454	2018	4	M	U	3	f	t	2	t
455	2020	1	F	U	4	f	f	1	t
455	2020	1	F	U	4	f	f	2	t
456	2019	1	M	U	5	f	t	3	t
456	2019	1	M	U	5	f	t	1	t
457	2019	1	F	N	1	f	f	1	t
457	2019	1	F	N	1	f	f	2	t
459	2020	1	F	N	3	f	f	1	t
459	2020	1	F	N	3	f	t	2	t
460	2020	5	M	U	1	f	t	3	t
460	2020	5	M	U	1	f	f	1	t
461	2019	2	F	N	3	f	f	2	t
461	2019	2	F	N	3	f	f	1	t
463	2018	5	U	U	1	t	f	2	t
463	2018	5	U	U	1	t	f	3	t
463	2018	5	U	U	1	t	f	1	t
467	2019	3	F	U	7	t	f	1	t
467	2019	3	F	U	7	t	t	2	t
469	2018	6	F	Y	4	t	f	3	t
469	2018	6	F	Y	4	t	f	1	t
469	2018	6	F	Y	4	t	t	2	t
471	2020	7	M	Y	7	f	t	2	t
471	2020	7	M	Y	7	f	f	1	t
471	2020	7	M	Y	7	f	f	3	t
474	2019	5	F	Y	3	f	f	3	t
474	2019	5	F	Y	3	f	f	1	t
476	2020	2	F	Y	5	f	t	2	t
476	2020	2	F	Y	5	f	f	1	t
477	2020	6	M	N	1	t	f	1	t
477	2020	6	M	N	1	t	f	2	t
478	2019	1	M	Y	1	f	f	1	t
478	2019	1	M	Y	1	f	t	2	t
479	2018	1	F	Y	2	f	f	2	t
479	2018	1	F	Y	2	f	f	1	t
482	2019	6	M	U	6	t	f	1	t
482	2019	6	M	U	6	t	t	2	t
483	2018	6	M	Y	1	f	f	1	t
483	2018	6	M	Y	1	f	f	2	t
485	2020	4	U	Y	6	t	t	2	t
485	2020	4	U	Y	6	t	f	1	t
486	2019	3	M	N	5	t	t	2	t
486	2019	3	M	N	5	t	f	1	t
489	2018	2	F	Y	3	f	f	1	t
489	2018	2	F	Y	3	f	t	2	t
490	2019	6	M	Y	4	f	t	2	t
490	2019	6	M	Y	4	f	f	1	t
491	2019	7	M	U	3	f	f	3	t
491	2019	7	M	U	3	f	t	1	t
492	2018	6	F	U	7	f	f	1	t
492	2018	6	F	U	7	f	t	2	t
493	2019	6	F	U	2	f	t	2	t
493	2019	6	F	U	2	f	f	1	t
494	2020	6	M	N	4	f	f	1	t
494	2020	6	M	N	4	f	f	2	t
494	2020	6	M	N	4	f	t	3	t
495	2019	3	F	N	3	t	t	2	t
495	2019	3	F	N	3	t	f	3	t
495	2019	3	F	N	3	t	f	1	t
497	2019	4	U	U	2	f	f	1	t
497	2019	4	U	U	2	f	t	2	t
498	2018	2	U	Y	5	t	f	1	t
498	2018	2	U	Y	5	t	t	2	t
501	2018	6	M	N	3	f	t	2	t
501	2018	6	M	N	3	f	f	1	t
502	2018	1	M	Y	4	f	f	3	t
502	2018	1	M	Y	4	f	f	1	t
504	2019	6	M	N	5	t	f	2	t
504	2019	6	M	N	5	t	f	3	t
504	2019	6	M	N	5	t	t	1	t
505	2020	6	M	Y	1	f	t	2	t
505	2020	6	M	Y	1	f	t	3	t
505	2020	6	M	Y	1	f	f	1	t
506	2020	2	F	Y	1	f	f	1	t
506	2020	2	F	Y	1	f	f	2	t
507	2020	3	F	U	5	t	t	1	t
507	2020	3	F	U	5	t	t	2	t
508	2020	6	F	U	5	f	f	1	t
508	2020	6	F	U	5	f	f	3	t
509	2019	7	M	Y	5	f	f	2	t
509	2019	7	M	Y	5	f	f	1	t
511	2019	2	U	U	7	f	f	2	t
511	2019	2	U	U	7	f	f	1	t
512	2018	6	F	N	7	f	f	1	t
512	2018	6	F	N	7	f	f	2	t
513	2019	3	M	Y	3	t	f	1	t
513	2019	3	M	Y	3	t	t	2	t
515	2019	4	U	U	1	f	f	2	t
515	2019	4	U	U	1	f	f	1	t
516	2020	3	M	Y	3	f	t	2	t
516	2020	3	M	Y	3	f	f	3	t
516	2020	3	M	Y	3	f	f	1	t
517	2019	1	U	N	3	t	t	2	t
517	2019	1	U	N	3	t	f	1	t
519	2019	4	F	N	5	t	f	3	t
519	2019	4	F	N	5	t	f	1	t
519	2019	4	F	N	5	t	t	2	t
520	2019	1	F	N	6	f	f	2	t
520	2019	1	F	N	6	f	f	1	t
521	2018	6	U	Y	4	t	f	1	t
521	2018	6	U	Y	4	t	t	3	t
522	2018	6	M	Y	7	f	f	1	t
522	2018	6	M	Y	7	f	t	2	t
523	2018	3	M	N	5	f	f	2	t
523	2018	3	M	N	5	f	t	1	t
524	2018	6	U	N	4	f	f	1	t
524	2018	6	U	N	4	f	f	2	t
525	2020	7	F	U	4	f	f	3	t
525	2020	7	F	U	4	f	t	1	t
526	2018	5	U	Y	7	f	t	2	t
526	2018	5	U	Y	7	f	f	1	t
527	2019	7	M	Y	3	f	f	2	t
527	2019	7	M	Y	3	f	f	1	t
528	2020	6	F	Y	4	f	t	2	t
528	2020	6	F	Y	4	f	f	1	t
530	2020	5	M	Y	3	f	t	2	t
530	2020	5	M	Y	3	f	f	1	t
531	2020	4	U	Y	3	f	f	1	t
531	2020	4	U	Y	3	f	f	2	t
532	2019	5	F	U	6	f	f	2	t
532	2019	5	F	U	6	f	f	1	t
535	2019	6	M	Y	4	t	t	1	t
535	2019	6	M	Y	4	t	t	3	t
535	2019	6	M	Y	4	t	f	2	t
536	2020	7	F	N	6	f	f	1	t
536	2020	7	F	N	6	f	f	3	t
536	2020	7	F	N	6	f	t	2	t
537	2018	3	F	N	3	f	f	2	t
537	2018	3	F	N	3	f	f	1	t
537	2018	3	F	N	3	f	f	3	t
539	2018	4	M	Y	4	t	f	2	t
539	2018	4	M	Y	4	t	f	1	t
539	2018	4	M	Y	4	t	t	3	t
543	2019	6	M	Y	7	t	f	1	t
543	2019	6	M	Y	7	t	t	3	t
545	2019	1	F	U	6	t	f	1	t
545	2019	1	F	U	6	t	f	2	t
549	2018	7	F	Y	4	f	f	3	t
549	2018	7	F	Y	4	f	f	1	t
552	2020	7	M	U	2	f	f	1	t
552	2020	7	M	U	2	f	t	2	t
553	2018	3	U	U	2	t	f	1	t
553	2018	3	U	U	2	t	f	3	t
554	2020	5	F	N	6	f	t	3	t
554	2020	5	F	N	6	f	f	2	t
554	2020	5	F	N	6	f	f	1	t
555	2018	2	M	U	4	f	f	1	t
555	2018	2	M	U	4	f	f	3	t
556	2020	5	M	N	3	f	t	3	t
556	2020	5	M	N	3	f	f	1	t
557	2018	6	F	U	3	f	f	3	t
557	2018	6	F	U	3	f	f	1	t
558	2018	6	U	N	2	f	f	3	t
558	2018	6	U	N	2	f	f	1	t
559	2018	2	U	N	5	f	t	1	t
559	2018	2	U	N	5	f	t	2	t
560	2020	5	U	U	2	f	f	1	t
560	2020	5	U	U	2	f	f	2	t
561	2018	2	U	U	6	t	t	2	t
561	2018	2	U	U	6	t	f	1	t
565	2019	7	M	Y	3	f	t	2	t
565	2019	7	M	Y	3	f	f	1	t
569	2019	5	F	N	7	f	t	2	t
569	2019	5	F	N	7	f	f	1	t
570	2018	2	F	Y	4	f	f	1	t
570	2018	2	F	Y	4	f	t	2	t
572	2019	3	U	N	2	f	f	1	t
572	2019	3	U	N	2	f	t	2	t
574	2020	7	U	U	2	f	t	2	t
574	2020	7	U	U	2	f	f	1	t
578	2018	4	F	Y	1	f	f	3	t
578	2018	4	F	Y	1	f	f	1	t
579	2018	7	M	U	6	f	f	3	t
579	2018	7	M	U	6	f	f	1	t
580	2019	3	U	N	6	f	f	3	t
580	2019	3	U	N	6	f	t	2	t
580	2019	3	U	N	6	f	f	1	t
581	2020	4	M	Y	6	f	f	2	t
581	2020	4	M	Y	6	f	f	1	t
583	2020	2	M	Y	4	t	f	2	t
583	2020	2	M	Y	4	t	f	1	t
585	2019	5	F	N	4	f	f	1	t
585	2019	5	F	N	4	f	f	3	t
585	2019	5	F	N	4	f	f	2	t
587	2020	6	U	Y	2	t	f	1	t
587	2020	6	U	Y	2	t	f	3	t
588	2020	6	F	N	1	f	f	3	t
588	2020	6	F	N	1	f	f	1	t
591	2020	4	U	N	5	f	f	2	t
591	2020	4	U	N	5	f	f	1	t
592	2018	7	M	U	7	t	t	2	t
592	2018	7	M	U	7	t	f	1	t
594	2018	4	F	N	7	t	f	2	t
594	2018	4	F	N	7	t	f	1	t
594	2018	4	F	N	7	t	f	3	t
595	2019	4	F	U	5	f	t	1	t
595	2019	4	F	U	5	f	f	2	t
597	2019	2	U	U	3	f	f	1	t
597	2019	2	U	U	3	f	f	3	t
598	2018	5	F	U	7	f	f	2	t
598	2018	5	F	U	7	f	f	1	t
599	2018	7	F	Y	2	f	f	2	t
599	2018	7	F	Y	2	f	f	1	t
601	2020	7	U	Y	7	f	f	3	t
601	2020	7	U	Y	7	f	f	1	t
601	2020	7	U	Y	7	f	f	2	t
602	2019	1	U	U	2	f	f	1	t
602	2019	1	U	U	2	f	t	2	t
605	2020	7	U	U	5	f	f	1	t
605	2020	7	U	U	5	f	f	3	t
605	2020	7	U	U	5	f	t	2	t
606	2019	4	F	Y	3	f	f	3	t
606	2019	4	F	Y	3	f	f	2	t
606	2019	4	F	Y	3	f	f	1	t
607	2018	4	F	Y	5	t	t	2	t
607	2018	4	F	Y	5	t	f	1	t
607	2018	4	F	Y	5	t	t	3	t
612	2018	6	M	Y	4	f	f	1	t
612	2018	6	M	Y	4	f	f	3	t
613	2019	5	U	U	6	f	f	2	t
613	2019	5	U	U	6	f	f	1	t
614	2019	5	F	U	4	t	f	3	t
614	2019	5	F	U	4	t	f	1	t
616	2018	6	U	Y	6	f	f	1	t
616	2018	6	U	Y	6	f	f	2	t
617	2020	7	U	U	1	f	f	2	t
617	2020	7	U	U	1	f	f	1	t
619	2020	2	U	Y	3	f	f	3	t
619	2020	2	U	Y	3	f	f	1	t
620	2019	2	F	U	1	t	f	1	t
620	2019	2	F	U	1	t	f	3	t
621	2020	4	F	Y	5	f	f	3	t
621	2020	4	F	Y	5	f	f	1	t
622	2018	6	M	N	4	f	f	1	t
622	2018	6	M	N	4	f	t	2	t
622	2018	6	M	N	4	f	t	3	t
623	2019	7	F	U	6	f	f	2	t
623	2019	7	F	U	6	f	f	1	t
625	2019	7	U	Y	6	f	f	1	t
625	2019	7	U	Y	6	f	t	2	t
627	2020	1	U	Y	7	f	f	2	t
627	2020	1	U	Y	7	f	f	1	t
627	2020	1	U	Y	7	f	t	3	t
628	2018	1	F	N	4	f	f	1	t
628	2018	1	F	N	4	f	f	2	t
634	2018	2	F	N	5	f	t	2	t
634	2018	2	F	N	5	f	f	1	t
636	2018	4	M	U	2	f	f	2	t
636	2018	4	M	U	2	f	f	1	t
637	2020	3	U	Y	6	f	f	1	t
637	2020	3	U	Y	6	f	f	2	t
638	2018	2	M	U	1	t	f	3	t
638	2018	2	M	U	1	t	f	1	t
639	2019	6	M	U	5	f	f	1	t
639	2019	6	M	U	5	f	f	3	t
640	2019	5	F	N	7	f	t	2	t
640	2019	5	F	N	7	f	t	3	t
640	2019	5	F	N	7	f	f	1	t
641	2018	6	F	Y	2	f	f	1	t
641	2018	6	F	Y	2	f	f	3	t
643	2018	6	M	Y	2	f	t	1	t
643	2018	6	M	Y	2	f	f	2	t
646	2018	1	M	N	2	t	t	2	t
646	2018	1	M	N	2	t	f	1	t
649	2018	1	U	U	6	f	f	2	t
649	2018	1	U	U	6	f	f	1	t
650	2020	1	F	U	2	f	f	1	t
650	2020	1	F	U	2	f	f	2	t
652	2019	2	M	N	1	f	f	3	t
652	2019	2	M	N	1	f	f	1	t
653	2019	3	M	N	4	f	f	1	t
653	2019	3	M	N	4	f	t	2	t
654	2019	4	M	U	6	f	t	3	t
654	2019	4	M	U	6	f	f	1	t
654	2019	4	M	U	6	f	t	2	t
655	2018	3	F	U	2	f	f	1	t
655	2018	3	F	U	2	f	f	2	t
657	2020	6	M	N	5	f	f	1	t
657	2020	6	M	N	5	f	f	2	t
659	2019	3	U	Y	1	f	t	2	t
659	2019	3	U	Y	1	f	f	1	t
660	2019	1	U	Y	2	t	t	2	t
660	2019	1	U	Y	2	t	f	1	t
661	2019	1	U	Y	1	f	f	1	t
661	2019	1	U	Y	1	f	t	2	t
661	2019	1	U	Y	1	f	t	3	t
663	2019	4	M	Y	5	f	f	1	t
663	2019	4	M	Y	5	f	f	3	t
663	2019	4	M	Y	5	f	f	2	t
664	2019	7	U	U	6	f	t	2	t
664	2019	7	U	U	6	f	f	1	t
665	2020	1	F	N	1	f	f	1	t
665	2020	1	F	N	1	f	t	3	t
668	2020	3	M	U	3	f	t	3	t
668	2020	3	M	U	3	f	f	1	t
669	2018	3	U	U	7	f	f	1	t
669	2018	3	U	U	7	f	f	2	t
671	2019	3	U	N	6	f	t	2	t
671	2019	3	U	N	6	f	f	1	t
674	2020	1	M	U	3	t	t	2	t
674	2020	1	M	U	3	t	f	1	t
675	2018	2	U	Y	7	f	f	1	t
675	2018	2	U	Y	7	f	f	2	t
678	2018	4	U	N	3	f	f	3	t
678	2018	4	U	N	3	f	f	1	t
680	2018	4	M	U	4	f	f	1	t
680	2018	4	M	U	4	f	t	2	t
681	2020	4	U	U	6	f	f	1	t
681	2020	4	U	U	6	f	f	3	t
682	2018	5	M	N	2	f	f	1	t
682	2018	5	M	N	2	f	t	3	t
683	2020	1	U	U	3	f	f	1	t
683	2020	1	U	U	3	f	f	2	t
683	2020	1	U	U	3	f	t	3	t
684	2020	3	U	N	1	f	t	1	t
684	2020	3	U	N	1	f	f	2	t
685	2020	5	M	N	3	t	t	1	t
685	2020	5	M	N	3	t	f	2	t
686	2018	5	F	U	6	t	f	2	t
686	2018	5	F	U	6	t	f	1	t
687	2018	2	M	N	3	t	f	1	t
687	2018	2	M	N	3	t	f	3	t
690	2019	6	F	N	4	f	f	1	t
690	2019	6	F	N	4	f	t	2	t
691	2019	6	U	Y	7	f	f	2	t
691	2019	6	U	Y	7	f	f	1	t
692	2020	6	F	U	5	f	f	2	t
692	2020	6	F	U	5	f	f	1	t
693	2019	5	U	U	4	f	f	1	t
693	2019	5	U	U	4	f	t	3	t
693	2019	5	U	U	4	f	f	2	t
696	2019	7	U	Y	5	f	f	1	t
696	2019	7	U	Y	5	f	f	3	t
698	2020	4	M	U	4	f	f	1	t
698	2020	4	M	U	4	f	t	2	t
698	2020	4	M	U	4	f	t	3	t
700	2019	6	F	Y	6	f	t	2	t
700	2019	6	F	Y	6	f	f	1	t
701	2020	1	U	U	6	t	f	1	t
701	2020	1	U	U	6	t	f	2	t
702	2019	7	M	Y	3	f	f	1	t
702	2019	7	M	Y	3	f	f	3	t
703	2018	3	M	Y	3	f	f	1	t
703	2018	3	M	Y	3	f	f	3	t
704	2019	6	F	U	7	f	f	1	t
704	2019	6	F	U	7	f	f	2	t
705	2019	4	U	U	4	f	t	3	t
705	2019	4	U	U	4	f	f	1	t
706	2018	5	F	Y	2	f	f	1	t
706	2018	5	F	Y	2	f	t	2	t
706	2018	5	F	Y	2	f	t	3	t
708	2020	7	U	N	2	t	t	2	t
708	2020	7	U	N	2	t	f	1	t
708	2020	7	U	N	2	t	f	3	t
709	2018	6	F	Y	2	t	t	3	t
709	2018	6	F	Y	2	t	f	1	t
712	2020	2	M	N	2	f	f	2	t
712	2020	2	M	N	2	f	t	3	t
712	2020	2	M	N	2	f	f	1	t
715	2019	5	M	U	1	f	f	1	t
715	2019	5	M	U	1	f	t	2	t
717	2019	2	M	N	3	f	f	1	t
717	2019	2	M	N	3	f	t	3	t
718	2020	3	F	Y	5	f	f	2	t
718	2020	3	F	Y	5	f	f	1	t
719	2020	5	F	N	2	f	t	3	t
719	2020	5	F	N	2	f	f	1	t
719	2020	5	F	N	2	f	t	2	t
720	2019	3	F	Y	6	f	t	3	t
720	2019	3	F	Y	6	f	f	1	t
721	2019	6	U	N	1	f	t	2	t
721	2019	6	U	N	1	f	t	3	t
721	2019	6	U	N	1	f	t	1	t
723	2019	7	F	N	5	t	f	2	t
723	2019	7	F	N	5	t	f	1	t
724	2020	3	U	U	6	f	f	3	t
724	2020	3	U	U	6	f	f	1	t
725	2019	5	U	Y	5	f	f	1	t
725	2019	5	U	Y	5	f	f	3	t
725	2019	5	U	Y	5	f	t	2	t
726	2018	2	M	U	3	t	t	2	t
726	2018	2	M	U	3	t	f	1	t
728	2019	3	M	Y	2	f	t	2	t
728	2019	3	M	Y	2	f	t	3	t
728	2019	3	M	Y	2	f	f	1	t
729	2019	4	F	U	4	f	f	1	t
729	2019	4	F	U	4	f	f	2	t
730	2019	5	M	U	6	f	t	3	t
730	2019	5	M	U	6	f	f	1	t
731	2019	7	M	N	5	t	f	1	t
731	2019	7	M	N	5	t	f	2	t
732	2018	2	F	Y	5	f	f	1	t
732	2018	2	F	Y	5	f	f	2	t
733	2020	4	U	N	5	t	f	1	t
733	2020	4	U	N	5	t	t	2	t
734	2020	4	F	U	1	f	f	1	t
734	2020	4	F	U	1	f	t	2	t
737	2020	5	M	Y	6	f	t	3	t
737	2020	5	M	Y	6	f	f	1	t
738	2019	7	F	U	5	f	t	2	t
738	2019	7	F	U	5	f	t	1	t
740	2019	1	U	N	3	f	f	1	t
740	2019	1	U	N	3	f	f	2	t
741	2020	2	U	U	1	f	f	2	t
741	2020	2	U	U	1	f	f	1	t
742	2019	1	F	N	7	f	t	2	t
742	2019	1	F	N	7	f	f	1	t
743	2019	2	F	U	2	t	t	3	t
743	2019	2	F	U	2	t	t	1	t
744	2018	3	F	Y	6	f	t	2	t
744	2018	3	F	Y	6	f	f	1	t
744	2018	3	F	Y	6	f	f	3	t
745	2020	1	F	U	6	t	t	2	t
745	2020	1	F	U	6	t	f	1	t
746	2020	5	M	Y	1	t	t	2	t
746	2020	5	M	Y	1	t	t	1	t
747	2019	2	M	N	6	f	f	1	t
747	2019	2	M	N	6	f	t	2	t
749	2019	4	M	U	1	t	f	2	t
749	2019	4	M	U	1	t	f	1	t
750	2018	2	F	U	3	f	f	1	t
750	2018	2	F	U	3	f	f	3	t
751	2018	5	M	Y	5	f	f	3	t
751	2018	5	M	Y	5	f	t	2	t
751	2018	5	M	Y	5	f	f	1	t
752	2020	6	U	U	1	t	t	2	t
752	2020	6	U	U	1	t	f	1	t
753	2020	3	F	U	4	t	f	1	t
753	2020	3	F	U	4	t	t	2	t
754	2020	6	F	Y	7	t	f	3	t
754	2020	6	F	Y	7	t	f	1	t
754	2020	6	F	Y	7	t	t	2	t
756	2018	3	F	U	5	f	f	1	t
756	2018	3	F	U	5	f	t	2	t
756	2018	3	F	U	5	f	t	3	t
758	2019	1	M	U	5	f	f	3	t
758	2019	1	M	U	5	f	t	2	t
758	2019	1	M	U	5	f	f	1	t
761	2020	2	F	Y	7	f	t	3	t
761	2020	2	F	Y	7	f	f	1	t
761	2020	2	F	Y	7	f	t	2	t
762	2020	5	U	N	2	f	f	1	t
762	2020	5	U	N	2	f	f	2	t
763	2020	3	U	U	6	f	t	2	t
763	2020	3	U	U	6	f	f	1	t
766	2019	1	U	N	1	f	t	2	t
766	2019	1	U	N	1	f	f	1	t
770	2018	3	U	Y	7	f	t	2	t
770	2018	3	U	Y	7	f	f	1	t
771	2020	2	F	Y	3	f	f	3	t
771	2020	2	F	Y	3	f	f	2	t
771	2020	2	F	Y	3	f	f	1	t
772	2019	7	F	N	5	t	t	2	t
772	2019	7	F	N	5	t	f	1	t
773	2018	3	M	U	7	t	f	2	t
773	2018	3	M	U	7	t	f	1	t
774	2019	7	F	N	7	t	f	2	t
774	2019	7	F	N	7	t	f	1	t
775	2018	1	M	U	1	f	t	2	t
775	2018	1	M	U	1	f	f	1	t
776	2020	2	U	N	5	t	f	2	t
776	2020	2	U	N	5	t	f	1	t
776	2020	2	U	N	5	t	f	3	t
777	2019	5	M	Y	1	f	f	1	t
777	2019	5	M	Y	1	f	t	2	t
777	2019	5	M	Y	1	f	f	3	t
778	2019	5	M	N	6	f	t	2	t
778	2019	5	M	N	6	f	t	1	t
779	2018	6	M	N	4	f	t	2	t
779	2018	6	M	N	4	f	f	3	t
779	2018	6	M	N	4	f	f	1	t
783	2018	4	F	N	6	t	f	2	t
783	2018	4	F	N	6	t	t	1	t
783	2018	4	F	N	6	t	t	3	t
787	2020	1	M	U	7	f	t	1	t
787	2020	1	M	U	7	f	t	3	t
789	2018	7	F	U	5	f	f	1	t
789	2018	7	F	U	5	f	f	3	t
790	2020	1	M	Y	7	t	f	1	t
790	2020	1	M	Y	7	t	t	3	t
791	2018	5	F	U	2	t	f	1	t
791	2018	5	F	U	2	t	f	2	t
794	2020	4	F	U	2	f	f	1	t
794	2020	4	F	U	2	f	t	2	t
795	2018	2	U	Y	3	f	f	3	t
795	2018	2	U	Y	3	f	f	2	t
795	2018	2	U	Y	3	f	f	1	t
796	2019	7	F	N	7	f	f	3	t
796	2019	7	F	N	7	f	f	1	t
797	2019	7	F	U	1	f	t	2	t
797	2019	7	F	U	1	f	f	1	t
799	2019	4	F	Y	4	f	f	1	t
799	2019	4	F	Y	4	f	f	3	t
800	2020	5	F	Y	4	f	f	2	t
800	2020	5	F	Y	4	f	f	1	t
801	2019	6	U	Y	6	t	f	1	t
801	2019	6	U	Y	6	t	t	2	t
804	2019	4	F	Y	3	f	f	3	t
804	2019	4	F	Y	3	f	f	1	t
805	2020	3	F	Y	5	f	f	1	t
805	2020	3	F	Y	5	f	f	3	t
806	2020	1	F	Y	5	t	f	3	t
806	2020	1	F	Y	5	t	f	1	t
808	2018	7	F	Y	4	f	f	3	t
808	2018	7	F	Y	4	f	f	1	t
810	2019	1	F	N	3	f	t	2	t
810	2019	1	F	N	3	f	f	1	t
812	2020	7	M	Y	4	f	f	1	t
812	2020	7	M	Y	4	f	t	3	t
813	2018	4	F	U	2	f	t	2	t
813	2018	4	F	U	2	f	t	1	t
814	2018	7	F	U	4	f	f	1	t
814	2018	7	F	U	4	f	t	3	t
815	2020	2	M	N	3	f	f	1	t
815	2020	2	M	N	3	f	f	2	t
817	2020	5	U	Y	4	f	t	2	t
817	2020	5	U	Y	4	f	f	1	t
818	2020	7	U	U	4	f	f	2	t
818	2020	7	U	U	4	f	f	1	t
819	2018	4	F	U	5	f	f	2	t
819	2018	4	F	U	5	f	f	1	t
820	2018	2	U	N	2	f	f	1	t
820	2018	2	U	N	2	f	t	2	t
821	2019	4	M	Y	6	f	f	3	t
821	2019	4	M	Y	6	f	f	1	t
822	2020	5	F	Y	6	f	f	2	t
822	2020	5	F	Y	6	f	f	1	t
822	2020	5	F	Y	6	f	f	3	t
823	2020	6	F	N	7	t	f	3	t
823	2020	6	F	N	7	t	f	1	t
824	2019	5	U	Y	6	f	t	3	t
824	2019	5	U	Y	6	f	f	1	t
825	2020	6	M	U	2	t	f	1	t
825	2020	6	M	U	2	t	f	2	t
828	2019	2	U	U	2	f	f	1	t
828	2019	2	U	U	2	f	f	2	t
830	2019	7	U	U	3	f	t	2	t
830	2019	7	U	U	3	f	f	1	t
832	2020	6	M	Y	1	f	f	1	t
832	2020	6	M	Y	1	f	t	2	t
834	2018	7	U	U	3	f	f	3	t
834	2018	7	U	U	3	f	f	1	t
834	2018	7	U	U	3	f	f	2	t
835	2019	5	U	U	3	f	t	2	t
835	2019	5	U	U	3	f	f	3	t
835	2019	5	U	U	3	f	f	1	t
836	2020	6	M	Y	7	t	f	1	t
836	2020	6	M	Y	7	t	f	3	t
836	2020	6	M	Y	7	t	f	2	t
838	2019	7	M	U	5	f	t	2	t
838	2019	7	M	U	5	f	t	3	t
838	2019	7	M	U	5	f	f	1	t
840	2020	4	M	Y	6	t	f	1	t
840	2020	4	M	Y	6	t	f	2	t
840	2020	4	M	Y	6	t	t	3	t
841	2020	4	U	U	4	t	t	2	t
841	2020	4	U	U	4	t	t	1	t
842	2018	3	M	Y	6	f	f	1	t
842	2018	3	M	Y	6	f	f	2	t
843	2019	5	M	N	6	f	t	2	t
843	2019	5	M	N	6	f	f	1	t
844	2018	3	M	N	2	f	t	1	t
844	2018	3	M	N	2	f	t	2	t
845	2018	6	M	N	6	t	f	2	t
845	2018	6	M	N	6	t	f	1	t
846	2018	1	M	U	7	f	f	3	t
846	2018	1	M	U	7	f	f	2	t
846	2018	1	M	U	7	f	f	1	t
847	2020	3	M	U	1	f	f	1	t
847	2020	3	M	U	1	f	f	3	t
847	2020	3	M	U	1	f	t	2	t
848	2019	6	U	U	7	f	f	3	t
848	2019	6	U	U	7	f	f	1	t
849	2019	2	F	N	6	t	t	2	t
849	2019	2	F	N	6	t	f	1	t
850	2020	5	U	N	4	f	t	2	t
850	2020	5	U	N	4	f	f	1	t
853	2019	5	M	U	6	t	f	3	t
853	2019	5	M	U	6	t	f	1	t
853	2019	5	M	U	6	t	f	2	t
854	2019	1	U	N	2	f	f	3	t
854	2019	1	U	N	2	f	f	1	t
855	2019	3	F	N	5	t	f	1	t
855	2019	3	F	N	5	t	f	3	t
857	2019	7	M	N	7	f	t	3	t
857	2019	7	M	N	7	f	t	2	t
857	2019	7	M	N	7	f	f	1	t
858	2019	6	M	Y	6	t	t	2	t
858	2019	6	M	Y	6	t	f	3	t
858	2019	6	M	Y	6	t	f	1	t
859	2020	2	F	U	1	t	f	2	t
859	2020	2	F	U	1	t	f	1	t
859	2020	2	F	U	1	t	f	3	t
860	2020	1	U	Y	3	f	t	2	t
860	2020	1	U	Y	3	f	t	1	t
861	2018	5	F	U	2	t	f	3	t
861	2018	5	F	U	2	t	f	1	t
863	2020	3	F	Y	5	f	f	1	t
863	2020	3	F	Y	5	f	f	2	t
866	2019	4	M	N	2	f	f	3	t
866	2019	4	M	N	2	f	t	1	t
866	2019	4	M	N	2	f	t	2	t
867	2018	1	F	N	1	f	t	2	t
867	2018	1	F	N	1	f	f	1	t
872	2020	3	F	Y	5	t	f	2	t
872	2020	3	F	Y	5	t	f	1	t
874	2018	4	U	Y	3	f	t	3	t
874	2018	4	U	Y	3	f	f	1	t
876	2018	1	M	Y	1	f	f	2	t
876	2018	1	M	Y	1	f	f	1	t
877	2019	3	F	Y	6	f	t	1	t
877	2019	3	F	Y	6	f	f	3	t
878	2019	5	M	Y	7	f	f	3	t
878	2019	5	M	Y	7	f	f	1	t
879	2018	4	M	Y	5	t	f	2	t
879	2018	4	M	Y	5	t	f	1	t
880	2018	6	U	N	5	f	t	1	t
880	2018	6	U	N	5	f	t	2	t
880	2018	6	U	N	5	f	t	3	t
883	2018	6	M	Y	3	f	f	1	t
883	2018	6	M	Y	3	f	f	2	t
885	2018	6	U	U	2	t	t	3	t
885	2018	6	U	U	2	t	f	1	t
885	2018	6	U	U	2	t	f	2	t
886	2020	6	M	Y	7	f	f	3	t
886	2020	6	M	Y	7	f	t	1	t
887	2020	5	M	N	7	f	f	1	t
887	2020	5	M	N	7	f	t	2	t
888	2019	6	U	Y	1	t	f	3	t
888	2019	6	U	Y	1	t	f	2	t
888	2019	6	U	Y	1	t	f	1	t
891	2020	1	F	N	5	f	f	2	t
891	2020	1	F	N	5	f	f	1	t
891	2020	1	F	N	5	f	f	3	t
892	2020	3	M	Y	1	f	f	3	t
892	2020	3	M	Y	1	f	f	1	t
895	2018	7	F	N	2	t	f	1	t
895	2018	7	F	N	2	t	f	3	t
897	2020	4	F	N	6	t	f	3	t
897	2020	4	F	N	6	t	t	2	t
897	2020	4	F	N	6	t	f	1	t
898	2020	6	M	U	7	f	t	2	t
898	2020	6	M	U	7	f	f	1	t
899	2018	4	M	Y	6	f	f	1	t
899	2018	4	M	Y	6	f	f	2	t
902	2018	7	M	N	2	f	f	2	t
902	2018	7	M	N	2	f	f	3	t
902	2018	7	M	N	2	f	f	1	t
906	2018	1	M	U	5	t	f	1	t
906	2018	1	M	U	5	t	f	3	t
907	2019	4	U	Y	6	f	t	2	t
907	2019	4	U	Y	6	f	f	1	t
908	2018	7	M	Y	5	f	f	3	t
908	2018	7	M	Y	5	f	f	1	t
908	2018	7	M	Y	5	f	t	2	t
910	2020	3	M	Y	1	f	f	1	t
910	2020	3	M	Y	1	f	f	3	t
910	2020	3	M	Y	1	f	f	2	t
911	2018	3	F	Y	5	f	f	3	t
911	2018	3	F	Y	5	f	f	1	t
913	2020	5	F	Y	3	f	f	1	t
913	2020	5	F	Y	3	f	f	3	t
915	2019	5	F	Y	1	f	f	1	t
915	2019	5	F	Y	1	f	t	2	t
915	2019	5	F	Y	1	f	f	3	t
916	2020	6	M	U	2	t	t	3	t
916	2020	6	M	U	2	t	t	2	t
916	2020	6	M	U	2	t	f	1	t
917	2019	2	M	N	7	f	t	3	t
917	2019	2	M	N	7	f	f	1	t
917	2019	2	M	N	7	f	t	2	t
920	2020	6	M	Y	4	f	f	2	t
920	2020	6	M	Y	4	f	f	1	t
921	2020	3	F	N	7	f	t	3	t
921	2020	3	F	N	7	f	f	2	t
921	2020	3	F	N	7	f	f	1	t
922	2020	7	U	U	6	t	f	1	t
922	2020	7	U	U	6	t	t	3	t
923	2019	4	M	U	3	f	f	1	t
923	2019	4	M	U	3	f	t	2	t
928	2019	3	M	N	7	f	t	2	t
928	2019	3	M	N	7	f	f	1	t
929	2018	2	M	N	4	f	f	1	t
929	2018	2	M	N	4	f	f	2	t
930	2019	1	M	U	3	f	t	1	t
930	2019	1	M	U	3	f	t	2	t
931	2018	5	F	U	7	f	t	1	t
931	2018	5	F	U	7	f	t	2	t
932	2020	5	F	Y	1	f	f	1	t
932	2020	5	F	Y	1	f	t	2	t
933	2019	3	U	N	5	t	f	2	t
933	2019	3	U	N	5	t	f	1	t
935	2018	1	U	N	5	t	t	3	t
935	2018	1	U	N	5	t	f	1	t
936	2018	3	U	U	1	f	f	1	t
936	2018	3	U	U	1	f	t	3	t
937	2018	3	M	Y	4	f	f	1	t
937	2018	3	M	Y	4	f	f	2	t
939	2019	4	F	N	7	t	t	2	t
939	2019	4	F	N	7	t	f	1	t
941	2020	3	U	U	5	f	t	3	t
941	2020	3	U	U	5	f	f	1	t
942	2020	3	U	U	3	f	t	2	t
942	2020	3	U	U	3	f	f	1	t
944	2018	6	F	Y	1	t	t	2	t
944	2018	6	F	Y	1	t	f	1	t
945	2019	4	M	U	5	f	f	1	t
945	2019	4	M	U	5	f	f	2	t
945	2019	4	M	U	5	f	f	3	t
946	2019	5	U	Y	6	f	t	1	t
946	2019	5	U	Y	6	f	t	2	t
946	2019	5	U	Y	6	f	f	3	t
947	2019	6	U	N	2	f	f	2	t
947	2019	6	U	N	2	f	f	1	t
948	2019	5	M	U	6	f	f	2	t
948	2019	5	M	U	6	f	f	1	t
951	2019	1	F	Y	3	f	f	1	t
951	2019	1	F	Y	3	f	f	3	t
952	2020	6	F	U	3	t	f	2	t
952	2020	6	F	U	3	t	f	1	t
953	2019	2	U	U	4	f	t	2	t
953	2019	2	U	U	4	f	f	1	t
954	2019	1	M	U	1	f	f	1	t
954	2019	1	M	U	1	f	f	2	t
955	2020	2	M	Y	7	f	f	1	t
955	2020	2	M	Y	7	f	f	2	t
956	2020	1	U	N	7	f	f	1	t
956	2020	1	U	N	7	f	f	2	t
957	2018	2	F	U	5	t	f	1	t
957	2018	2	F	U	5	t	t	2	t
958	2019	7	F	U	5	f	f	2	t
958	2019	7	F	U	5	f	f	1	t
959	2018	2	M	N	3	t	f	3	t
959	2018	2	M	N	3	t	f	1	t
961	2020	7	F	Y	1	f	f	1	t
961	2020	7	F	Y	1	f	f	2	t
962	2018	4	F	U	3	f	f	2	t
962	2018	4	F	U	3	f	f	1	t
963	2018	1	U	N	7	f	t	2	t
963	2018	1	U	N	7	f	f	1	t
965	2019	4	F	Y	3	t	f	1	t
965	2019	4	F	Y	3	t	f	3	t
966	2020	2	U	Y	2	f	f	1	t
966	2020	2	U	Y	2	f	f	3	t
966	2020	2	U	Y	2	f	f	2	t
968	2020	5	F	N	7	f	f	3	t
968	2020	5	F	N	7	f	f	1	t
969	2020	3	M	N	3	f	f	1	t
969	2020	3	M	N	3	f	f	2	t
969	2020	3	M	N	3	f	t	3	t
970	2018	4	M	Y	7	f	t	3	t
970	2018	4	M	Y	7	f	t	1	t
970	2018	4	M	Y	7	f	t	2	t
972	2020	7	F	Y	5	f	f	3	t
972	2020	7	F	Y	5	f	f	1	t
972	2020	7	F	Y	5	f	t	2	t
975	2018	6	U	Y	7	f	t	2	t
975	2018	6	U	Y	7	f	f	1	t
976	2018	3	U	Y	1	t	f	1	t
976	2018	3	U	Y	1	t	t	2	t
978	2018	1	F	Y	4	t	f	1	t
978	2018	1	F	Y	4	t	t	2	t
979	2019	7	U	Y	6	f	f	2	t
979	2019	7	U	Y	6	f	f	1	t
980	2019	4	M	Y	6	f	f	1	t
980	2019	4	M	Y	6	f	t	2	t
982	2020	6	U	U	2	t	t	1	t
982	2020	6	U	U	2	t	t	2	t
983	2018	4	U	U	6	f	f	1	t
983	2018	4	U	U	6	f	t	2	t
984	2019	1	F	Y	5	t	t	2	t
984	2019	1	F	Y	5	t	t	3	t
984	2019	1	F	Y	5	t	f	1	t
987	2018	4	F	Y	7	f	f	1	t
987	2018	4	F	Y	7	f	f	2	t
989	2020	2	M	Y	6	f	f	2	t
989	2020	2	M	Y	6	f	f	1	t
989	2020	2	M	Y	6	f	f	3	t
991	2019	2	M	Y	4	f	f	3	t
991	2019	2	M	Y	4	f	f	1	t
993	2020	5	M	U	1	f	f	2	t
993	2020	5	M	U	1	f	f	3	t
993	2020	5	M	U	1	f	f	1	t
996	2018	4	F	Y	5	f	f	1	t
996	2018	4	F	Y	5	f	f	2	t
996	2018	4	F	Y	5	f	t	3	t
998	2020	7	U	U	5	t	f	1	t
998	2020	7	U	U	5	t	t	2	t
999	2018	2	M	Y	6	f	t	2	t
999	2018	2	M	Y	6	f	f	1	t
999	2018	2	M	Y	6	f	f	3	t
1000	2018	6	U	Y	4	f	f	1	t
1000	2018	6	U	Y	4	f	f	3	t
1003	2018	4	M	U	4	f	f	3	t
1003	2018	4	M	U	4	f	f	1	t
1004	2020	2	F	U	3	f	f	1	t
1004	2020	2	F	U	3	f	t	3	t
1007	2020	5	M	N	4	f	f	3	t
1007	2020	5	M	N	4	f	f	1	t
1008	2020	4	M	Y	5	f	f	1	t
1008	2020	4	M	Y	5	f	f	3	t
1010	2018	6	U	U	7	f	f	3	t
1010	2018	6	U	U	7	f	f	1	t
1012	2018	7	F	Y	7	f	f	1	t
1012	2018	7	F	Y	7	f	t	3	t
1014	2018	6	U	N	3	f	t	3	t
1014	2018	6	U	N	3	f	f	1	t
1022	2019	7	M	Y	1	f	f	3	t
1022	2019	7	M	Y	1	f	f	1	t
1027	2019	5	U	Y	4	f	f	3	t
1027	2019	5	U	Y	4	f	f	1	t
1032	2018	2	F	N	3	f	f	3	t
1032	2018	2	F	N	3	f	f	1	t
1037	2018	1	U	U	6	f	f	3	t
1037	2018	1	U	U	6	f	f	1	t
1043	2018	4	U	N	7	f	t	3	t
1043	2018	4	U	N	7	f	f	1	t
1048	2019	5	F	Y	6	f	f	1	t
1048	2019	5	F	Y	6	f	f	3	t
1050	2020	4	F	N	7	f	t	1	t
1050	2020	4	F	N	7	f	t	3	t
1051	2019	7	U	N	2	f	f	1	t
1051	2019	7	U	N	2	f	t	3	t
1052	2019	2	M	U	7	f	t	3	t
1052	2019	2	M	U	7	f	f	1	t
1056	2018	5	M	U	4	f	f	1	t
1056	2018	5	M	U	4	f	f	3	t
1057	2019	2	U	Y	1	f	t	3	t
1057	2019	2	U	Y	1	f	f	1	t
1061	2020	1	M	N	4	f	f	1	t
1061	2020	1	M	N	4	f	f	3	t
1064	2019	4	M	Y	6	f	t	1	t
1064	2019	4	M	Y	6	f	f	3	t
1070	2020	5	F	N	6	t	f	3	t
1070	2020	5	F	N	6	t	f	1	t
1076	2020	5	M	U	7	f	f	1	t
1076	2020	5	M	U	7	f	t	3	t
1079	2018	1	U	U	7	t	f	3	t
1079	2018	1	U	U	7	t	f	1	t
1082	2020	7	F	Y	3	f	f	1	t
1082	2020	7	F	Y	3	f	f	3	t
1083	2019	6	U	N	6	t	f	3	t
1083	2019	6	U	N	6	t	f	1	t
1088	2019	6	U	Y	3	f	f	1	t
1088	2019	6	U	Y	3	f	f	3	t
1090	2019	3	F	N	3	t	f	1	t
1090	2019	3	F	N	3	t	f	3	t
1097	2019	4	F	N	2	f	f	3	t
1097	2019	4	F	N	2	f	f	1	t
1098	2018	6	F	Y	5	f	f	1	t
1098	2018	6	F	Y	5	f	t	3	t
1103	2018	2	U	U	7	t	f	3	t
1103	2018	2	U	U	7	t	t	1	t
1105	2018	1	U	N	4	f	f	1	t
1105	2018	1	U	N	4	f	t	3	t
1108	2018	4	F	N	1	f	f	1	t
1108	2018	4	F	N	1	f	t	3	t
1109	2019	7	U	N	3	f	f	3	t
1109	2019	7	U	N	3	f	f	1	t
1110	2020	1	M	N	4	f	t	3	t
1110	2020	1	M	N	4	f	f	1	t
1113	2019	4	M	N	2	t	f	3	t
1113	2019	4	M	N	2	t	f	1	t
1114	2018	3	U	U	6	f	f	1	t
1114	2018	3	U	U	6	f	t	3	t
1117	2018	1	M	N	4	f	f	3	t
1117	2018	1	M	N	4	f	f	1	t
1121	2018	1	F	Y	6	f	f	1	t
1121	2018	1	F	Y	6	f	f	3	t
1122	2020	2	U	Y	6	t	f	3	t
1122	2020	2	U	Y	6	t	f	1	t
1124	2018	5	M	Y	3	f	f	1	t
1124	2018	5	M	Y	3	f	f	3	t
1127	2019	3	F	U	2	f	f	1	t
1127	2019	3	F	U	2	f	t	3	t
1129	2019	1	M	N	7	f	f	3	t
1129	2019	1	M	N	7	f	f	1	t
1130	2019	4	U	U	4	f	f	1	t
1130	2019	4	U	U	4	f	t	3	t
1132	2019	7	M	N	1	f	f	1	t
1132	2019	7	M	N	1	f	t	3	t
1134	2020	5	F	U	1	f	f	1	t
1134	2020	5	F	U	1	f	f	3	t
1135	2019	4	U	U	1	f	f	3	t
1135	2019	4	U	U	1	f	f	1	t
1138	2018	1	U	N	3	f	f	1	t
1138	2018	1	U	N	3	f	t	3	t
1140	2020	1	F	Y	3	f	f	1	t
1140	2020	1	F	Y	3	f	f	3	t
1142	2019	7	U	Y	6	f	f	1	t
1142	2019	7	U	Y	6	f	t	3	t
1143	2018	2	U	N	1	f	f	3	t
1143	2018	2	U	N	1	f	f	1	t
1148	2018	7	U	Y	4	f	t	3	t
1148	2018	7	U	Y	4	f	f	1	t
1151	2020	1	M	N	1	f	t	3	t
1151	2020	1	M	N	1	f	f	1	t
1156	2019	3	F	N	5	f	f	1	t
1156	2019	3	F	N	5	f	f	3	t
1157	2019	1	F	N	7	f	f	3	t
1157	2019	1	F	N	7	f	f	1	t
1162	2020	4	U	Y	6	f	t	3	t
1162	2020	4	U	Y	6	f	f	1	t
1165	2018	6	U	N	5	f	f	3	t
1165	2018	6	U	N	5	f	f	1	t
1174	2019	1	F	N	5	f	f	1	t
1174	2019	1	F	N	5	f	f	3	t
1175	2020	4	M	U	7	f	f	1	t
1175	2020	4	M	U	7	f	t	3	t
1177	2020	6	F	N	5	f	f	3	t
1177	2020	6	F	N	5	f	t	1	t
1178	2018	1	U	U	5	t	f	3	t
1178	2018	1	U	U	5	t	f	1	t
1180	2020	2	U	U	4	f	f	1	t
1180	2020	2	U	U	4	f	f	3	t
1183	2020	5	M	N	6	f	f	3	t
1183	2020	5	M	N	6	f	f	1	t
1185	2019	5	U	Y	5	f	f	3	t
1185	2019	5	U	Y	5	f	f	1	t
1188	2020	4	M	Y	5	f	f	3	t
1188	2020	4	M	Y	5	f	f	1	t
1189	2018	1	U	U	6	f	t	1	t
1189	2018	1	U	U	6	f	f	3	t
1190	2020	3	F	Y	1	f	f	3	t
1190	2020	3	F	Y	1	f	f	1	t
1191	2019	6	M	U	6	f	f	1	t
1191	2019	6	M	U	6	f	f	3	t
1193	2019	6	M	U	3	f	t	1	t
1193	2019	6	M	U	3	f	t	3	t
1194	2018	7	F	U	6	f	f	3	t
1194	2018	7	F	U	6	f	f	1	t
1201	2019	3	M	N	2	f	f	1	t
1201	2019	3	M	N	2	f	f	3	t
1203	2020	1	M	U	7	f	f	1	t
1203	2020	1	M	U	7	f	f	3	t
1205	2018	6	U	U	5	f	t	3	t
1205	2018	6	U	U	5	f	f	1	t
1207	2020	7	F	U	7	f	f	1	t
1207	2020	7	F	U	7	f	f	3	t
1211	2018	1	M	N	1	t	t	3	t
1211	2018	1	M	N	1	t	f	1	t
1218	2019	3	U	U	2	f	f	3	t
1218	2019	3	U	U	2	f	f	1	t
1220	2020	2	F	Y	7	f	t	3	t
1220	2020	2	F	Y	7	f	f	1	t
1224	2019	6	U	Y	6	f	f	3	t
1224	2019	6	U	Y	6	f	f	1	t
1229	2020	1	M	Y	3	t	f	3	t
1229	2020	1	M	Y	3	t	f	1	t
1230	2018	2	M	N	1	f	f	1	t
1230	2018	2	M	N	1	f	f	3	t
1235	2018	7	F	N	5	f	f	1	t
1235	2018	7	F	N	5	f	f	3	t
1240	2019	6	M	U	4	t	f	1	t
1240	2019	6	M	U	4	t	f	3	t
1241	2019	3	U	Y	1	t	t	1	t
1241	2019	3	U	Y	1	t	f	3	t
1242	2019	5	F	Y	1	f	f	3	t
1242	2019	5	F	Y	1	f	f	1	t
1246	2019	3	F	Y	1	t	t	3	t
1246	2019	3	F	Y	1	t	f	1	t
1247	2020	5	U	Y	5	f	f	3	t
1247	2020	5	U	Y	5	f	f	1	t
1249	2020	1	U	N	1	f	f	3	t
1249	2020	1	U	N	1	f	f	1	t
1253	2019	1	U	U	7	f	f	1	t
1253	2019	1	U	U	7	f	t	3	t
1256	2018	6	U	U	5	f	f	1	t
1256	2018	6	U	U	5	f	f	3	t
1257	2020	7	F	N	5	f	f	3	t
1257	2020	7	F	N	5	f	f	1	t
1259	2019	7	M	N	4	t	f	3	t
1259	2019	7	M	N	4	t	f	1	t
1260	2019	1	F	Y	2	f	f	1	t
1260	2019	1	F	Y	2	f	f	3	t
1261	2020	1	M	N	4	f	f	1	t
1261	2020	1	M	N	4	f	f	3	t
1262	2018	2	M	N	7	f	f	3	t
1262	2018	2	M	N	7	f	f	1	t
1266	2019	3	M	Y	2	t	f	3	t
1266	2019	3	M	Y	2	t	f	1	t
1269	2018	7	F	Y	7	f	f	3	t
1269	2018	7	F	Y	7	f	f	1	t
1271	2020	4	F	U	6	f	f	3	t
1271	2020	4	F	U	6	f	f	1	t
1274	2018	5	F	Y	3	f	f	1	t
1274	2018	5	F	Y	3	f	f	3	t
1277	2018	1	F	U	6	f	f	1	t
1277	2018	1	F	U	6	f	f	3	t
1281	2019	2	U	U	4	f	f	3	t
1281	2019	2	U	U	4	f	f	1	t
1282	2020	6	M	Y	7	f	f	1	t
1282	2020	6	M	Y	7	f	f	3	t
1291	2018	2	M	N	2	t	f	3	t
1291	2018	2	M	N	2	t	f	1	t
1296	2019	1	U	Y	7	f	f	1	t
1296	2019	1	U	Y	7	f	f	3	t
1298	2019	1	M	U	2	t	f	1	t
1298	2019	1	M	U	2	t	t	3	t
1300	2019	5	U	Y	6	f	f	1	t
1300	2019	5	U	Y	6	f	f	3	t
1301	2019	2	U	Y	5	f	f	1	t
1301	2019	2	U	Y	5	f	f	3	t
1305	2018	4	M	U	1	f	f	3	t
1305	2018	4	M	U	1	f	f	1	t
1310	2020	7	M	Y	3	f	f	3	t
1310	2020	7	M	Y	3	f	f	1	t
1324	2018	5	U	N	2	f	f	3	t
1324	2018	5	U	N	2	f	f	1	t
1325	2018	1	U	U	1	f	f	1	t
1325	2018	1	U	U	1	f	f	3	t
1327	2019	2	U	N	1	f	f	3	t
1327	2019	2	U	N	1	f	f	1	t
1334	2019	3	U	Y	1	f	f	1	t
1334	2019	3	U	Y	1	f	t	3	t
1335	2020	4	U	N	5	f	f	1	t
1335	2020	4	U	N	5	f	f	3	t
1336	2019	4	U	Y	6	f	f	3	t
1336	2019	4	U	Y	6	f	f	1	t
1341	2019	3	M	U	6	f	f	1	t
1341	2019	3	M	U	6	f	t	3	t
1342	2018	2	M	U	2	t	t	3	t
1342	2018	2	M	U	2	t	f	1	t
1349	2020	4	M	N	3	t	f	3	t
1349	2020	4	M	N	3	t	f	1	t
1352	2018	2	U	U	1	f	f	3	t
1352	2018	2	U	U	1	f	f	1	t
1353	2019	4	M	Y	5	t	f	1	t
1353	2019	4	M	Y	5	t	f	3	t
1356	2020	6	F	U	4	f	f	3	t
1356	2020	6	F	U	4	f	f	1	t
1359	2018	3	F	Y	3	f	t	3	t
1359	2018	3	F	Y	3	f	f	1	t
1363	2019	7	F	N	5	t	t	1	t
1363	2019	7	F	N	5	t	f	3	t
1366	2018	7	F	N	4	t	f	1	t
1366	2018	7	F	N	4	t	f	3	t
1367	2020	2	U	Y	5	f	f	3	t
1367	2020	2	U	Y	5	f	f	1	t
1369	2020	4	U	U	1	f	t	3	t
1369	2020	4	U	U	1	f	t	1	t
1370	2020	4	M	U	4	t	f	3	t
1370	2020	4	M	U	4	t	f	1	t
1371	2018	3	F	U	5	f	f	1	t
1371	2018	3	F	U	5	f	f	3	t
1374	2019	4	F	Y	1	t	f	1	t
1374	2019	4	F	Y	1	t	f	3	t
1378	2020	5	M	U	6	f	t	3	t
1378	2020	5	M	U	6	f	f	1	t
1383	2020	4	U	N	5	f	f	1	t
1383	2020	4	U	N	5	f	f	3	t
1384	2020	1	F	U	7	t	f	3	t
1384	2020	1	F	U	7	t	f	1	t
1387	2019	7	M	Y	5	f	f	3	t
1387	2019	7	M	Y	5	f	f	1	t
1389	2019	3	U	N	6	t	f	1	t
1389	2019	3	U	N	6	t	f	3	t
1391	2020	7	M	Y	7	t	f	3	t
1391	2020	7	M	Y	7	t	f	1	t
1393	2020	6	M	U	7	t	t	1	t
1393	2020	6	M	U	7	t	t	3	t
1397	2019	7	U	U	7	f	f	3	t
1397	2019	7	U	U	7	f	f	1	t
1400	2019	6	U	N	4	t	f	1	t
1400	2019	6	U	N	4	t	t	3	t
1402	2018	2	F	N	7	f	f	1	t
1402	2018	2	F	N	7	f	f	3	t
1403	2018	3	U	N	2	f	f	3	t
1403	2018	3	U	N	2	f	f	1	t
1406	2019	4	F	U	7	f	f	3	t
1406	2019	4	F	U	7	f	f	1	t
1411	2018	5	M	Y	6	f	f	1	t
1411	2018	5	M	Y	6	f	f	3	t
1412	2020	3	U	U	6	f	f	1	t
1412	2020	3	U	U	6	f	t	3	t
1414	2020	7	M	Y	7	f	f	3	t
1414	2020	7	M	Y	7	f	t	1	t
1415	2020	4	F	Y	5	f	t	1	t
1415	2020	4	F	Y	5	f	f	3	t
1417	2018	1	F	Y	5	f	f	1	t
1417	2018	1	F	Y	5	f	f	3	t
1419	2018	1	U	Y	1	t	f	1	t
1419	2018	1	U	Y	1	t	t	3	t
1420	2020	2	M	Y	5	t	t	3	t
1420	2020	2	M	Y	5	t	f	1	t
1423	2019	6	F	N	6	f	f	3	t
1423	2019	6	F	N	6	f	f	1	t
1425	2018	6	U	Y	6	f	f	1	t
1425	2018	6	U	Y	6	f	f	3	t
1430	2020	2	U	N	7	f	f	3	t
1430	2020	2	U	N	7	f	f	1	t
1433	2020	1	F	N	2	t	f	1	t
1433	2020	1	F	N	2	t	t	3	t
1436	2018	1	U	U	5	f	f	3	t
1436	2018	1	U	U	5	f	f	1	t
1442	2019	2	M	N	5	f	f	3	t
1442	2019	2	M	N	5	f	f	1	t
1445	2019	6	F	U	4	f	f	3	t
1445	2019	6	F	U	4	f	f	1	t
1447	2019	7	M	N	7	f	f	1	t
1447	2019	7	M	N	7	f	t	3	t
1448	2018	3	F	U	7	t	f	3	t
1448	2018	3	F	U	7	t	f	1	t
1452	2018	2	M	U	7	f	t	3	t
1452	2018	2	M	U	7	f	f	1	t
1453	2020	2	U	U	6	f	t	3	t
1453	2020	2	U	U	6	f	f	1	t
1458	2019	5	F	Y	6	f	f	3	t
1458	2019	5	F	Y	6	f	f	1	t
1459	2018	5	U	Y	5	f	f	3	t
1459	2018	5	U	Y	5	f	f	1	t
1462	2020	6	M	N	4	f	f	3	t
1462	2020	6	M	N	4	f	f	1	t
1464	2018	5	U	U	2	f	f	3	t
1464	2018	5	U	U	2	f	f	1	t
1467	2019	4	M	N	1	f	t	3	t
1467	2019	4	M	N	1	f	f	1	t
1468	2020	4	U	U	6	f	f	1	t
1468	2020	4	U	U	6	f	t	3	t
1470	2020	7	U	U	5	f	f	1	t
1470	2020	7	U	U	5	f	f	3	t
1472	2019	7	F	Y	2	t	t	3	t
1472	2019	7	F	Y	2	t	f	1	t
1474	2019	5	U	U	4	f	t	3	t
1474	2019	5	U	U	4	f	f	1	t
1479	2018	6	F	N	4	f	t	1	t
1479	2018	6	F	N	4	f	t	3	t
1481	2019	6	F	N	2	f	f	1	t
1481	2019	6	F	N	2	f	f	3	t
1484	2018	1	U	Y	2	f	f	1	t
1484	2018	1	U	Y	2	f	f	3	t
1485	2019	4	F	Y	5	f	f	1	t
1485	2019	4	F	Y	5	f	f	3	t
1497	2019	2	M	Y	5	f	f	3	t
1497	2019	2	M	Y	5	f	t	1	t
1499	2020	2	M	U	6	f	f	1	t
1499	2020	2	M	U	6	f	f	3	t
\.


--
-- PostgreSQL database dump complete
--

