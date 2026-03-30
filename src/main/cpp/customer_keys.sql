--
-- PostgreSQL database dump
--

-- Dumped from database version 13.7 (Ubuntu 13.7-0ubuntu0.21.10.1)
-- Dumped by pg_dump version 13.7 (Ubuntu 13.7-0ubuntu0.21.10.1)

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
-- Name: customer_keys; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.customer_keys (
    c_custkey integer NOT NULL
);


ALTER TABLE public.customer_keys OWNER TO vaultdb;

--
-- Data for Name: customer_keys; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.customer_keys (c_custkey) FROM stdin;
4
90
43
44
45
46
47
48
49
50
51
52
53
54
55
56
69
70
71
72
73
74
75
76
77
57
58
59
60
61
62
63
64
65
66
67
68
120
78
79
80
81
82
83
84
85
86
87
88
89
1
2
3
5
6
7
8
9
10
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
121
122
123
124
141
142
143
144
145
146
147
148
149
150
133
136
137
17
18
19
20
21
22
23
24
25
26
27
28
29
30
138
139
140
11
12
13
14
15
16
31
32
33
34
35
36
37
38
39
40
41
42
134
125
126
127
128
129
130
131
132
135
\.


--
-- Name: customer_keys customer_keys_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.customer_keys
    ADD CONSTRAINT customer_keys_pkey PRIMARY KEY (c_custkey);


--
-- PostgreSQL database dump complete
--

