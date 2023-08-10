--
-- PostgreSQL database dump
--

-- Dumped from database version 15.3 (Ubuntu 15.3-0ubuntu0.23.04.1)
-- Dumped by pg_dump version 15.3 (Ubuntu 15.3-0ubuntu0.23.04.1)

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
-- Name: customer; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.customer (
    c_custkey integer NOT NULL,
    c_name character varying(25),
    c_address character varying(40),
    c_nationkey integer,
    c_phone character(15),
    c_acctbal numeric,
    c_mktsegment character(10),
    c_comment character varying(117),
    CONSTRAINT c_acctbal_domain CHECK (((c_acctbal >= '-999.99'::numeric) AND (c_acctbal <= 9999.99))),
    CONSTRAINT c_custkey_domain CHECK (((c_custkey >= 1) AND (c_custkey <= 15000))),
    CONSTRAINT c_mktsegment_domain CHECK ((c_mktsegment = ANY (ARRAY['MACHINERY'::bpchar, 'AUTOMOBILE'::bpchar, 'BUILDING'::bpchar, 'FURNITURE'::bpchar, 'HOUSEHOLD'::bpchar]))),
    CONSTRAINT c_name_domain CHECK (((split_part((c_name)::text, '#'::text, 1) = 'Customer'::text) AND ((split_part((c_name)::text, '#'::text, 2))::integer = c_custkey) AND (length(split_part((c_name)::text, '#'::text, 2)) = 9) AND (array_length(string_to_array((c_name)::text, '#'::text), 1) = 2))),
    CONSTRAINT c_nationkey_domain CHECK (((c_nationkey >= 0) AND (c_nationkey <= 24))),
    CONSTRAINT c_phone_domain CHECK (((("substring"((c_phone)::text, 1, 2))::integer = (c_nationkey + 10)) AND ("substring"((c_phone)::text, 3, 1) = '-'::text) AND ((("substring"((c_phone)::text, 4, 3))::integer >= 100) AND (("substring"((c_phone)::text, 4, 3))::integer <= 999)) AND ("substring"((c_phone)::text, 7, 1) = '-'::text) AND ((("substring"((c_phone)::text, 8, 3))::integer >= 100) AND (("substring"((c_phone)::text, 8, 3))::integer <= 999)) AND ("substring"((c_phone)::text, 11, 1) = '-'::text) AND ((("substring"((c_phone)::text, 12, 4))::integer >= 1000) AND (("substring"((c_phone)::text, 12, 4))::integer <= 9999))))
);


ALTER TABLE public.customer OWNER TO vaultdb;

--
-- Name: customer_c_custkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.customer_c_custkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.customer_c_custkey_seq OWNER TO vaultdb;

--
-- Name: customer_c_custkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.customer_c_custkey_seq OWNED BY public.customer.c_custkey;


--
-- Name: lineitem; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.lineitem (
    l_orderkey integer NOT NULL,
    l_partkey integer,
    l_suppkey integer,
    l_linenumber integer NOT NULL,
    l_quantity numeric,
    l_extendedprice numeric,
    l_discount numeric,
    l_tax numeric,
    l_returnflag character(1),
    l_linestatus character(1),
    l_shipdate date,
    l_commitdate date,
    l_receiptdate date,
    l_shipinstruct character(25),
    l_shipmode character(10),
    l_comment character varying(44),
    CONSTRAINT l_discount_domain CHECK (((l_discount >= 0.00) AND (l_discount <= 0.10))),
    CONSTRAINT l_linenumber_domain CHECK (((l_linenumber >= 1) AND (l_linenumber <= 7))),
    CONSTRAINT l_linestatus_based_on_shipdate_domain CHECK ((((l_linestatus = 'O'::bpchar) AND (l_shipdate > '1995-06-17'::date)) OR ((l_linestatus = 'F'::bpchar) AND (l_shipdate <= '1995-06-17'::date)))),
    CONSTRAINT l_linestatus_domain CHECK ((l_linestatus = ANY (ARRAY['O'::bpchar, 'F'::bpchar]))),
    CONSTRAINT l_partkey_domain CHECK (((l_partkey >= 1) AND (l_partkey <= 20000))),
    CONSTRAINT l_quantity_domain CHECK (((l_quantity >= (1)::numeric) AND (l_quantity <= (50)::numeric))),
    CONSTRAINT l_receiptdate_domain CHECK (((l_receiptdate >= (l_shipdate + 1)) AND (l_receiptdate <= (l_shipdate + 30)))),
    CONSTRAINT l_returnflag_based_on_receiptdate_domain CHECK ((((l_returnflag = ANY (ARRAY['R'::bpchar, 'A'::bpchar])) AND (l_receiptdate <= '1995-06-17'::date)) OR ((l_returnflag = 'N'::bpchar) AND (l_receiptdate > '1995-06-17'::date)))),
    CONSTRAINT l_returnflag_domain CHECK ((l_returnflag = ANY (ARRAY['R'::bpchar, 'A'::bpchar, 'N'::bpchar]))),
    CONSTRAINT l_shipinstruct_domain CHECK ((l_shipinstruct = ANY (ARRAY['DELIVER IN PERSON'::bpchar, 'COLLECT COD'::bpchar, 'NONE'::bpchar, 'TAKE BACK RETURN'::bpchar]))),
    CONSTRAINT l_shipmode_domain CHECK ((l_shipmode = ANY (ARRAY['REG AIR'::bpchar, 'AIR'::bpchar, 'RAIL'::bpchar, 'SHIP'::bpchar, 'TRUCK'::bpchar, 'MAIL'::bpchar, 'FOB'::bpchar]))),
    CONSTRAINT l_tax_domain CHECK (((l_tax >= 0.00) AND (l_tax <= 0.08)))
);


ALTER TABLE public.lineitem OWNER TO vaultdb;

--
-- Name: nation; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.nation (
    n_nationkey integer NOT NULL,
    n_name character(25),
    n_regionkey integer,
    n_comment character varying(152),
    CONSTRAINT n_name_domain CHECK ((n_name = ANY (ARRAY['ETHIOPIA'::bpchar, 'IRAN'::bpchar, 'EGYPT'::bpchar, 'RUSSIA'::bpchar, 'SAUDI ARABIA'::bpchar, 'INDONESIA'::bpchar, 'VIETNAM'::bpchar, 'GERMANY'::bpchar, 'PERU'::bpchar, 'FRANCE'::bpchar, 'ALGERIA'::bpchar, 'ROMANIA'::bpchar, 'JORDAN'::bpchar, 'ARGENTINA'::bpchar, 'MOROCCO'::bpchar, 'CANADA'::bpchar, 'JAPAN'::bpchar, 'INDIA'::bpchar, 'UNITED KINGDOM'::bpchar, 'UNITED STATES'::bpchar, 'MOZAMBIQUE'::bpchar, 'CHINA'::bpchar, 'BRAZIL'::bpchar, 'KENYA'::bpchar, 'IRAQ'::bpchar]))),
    CONSTRAINT n_nationkey_domain CHECK (((n_nationkey >= 0) AND (n_nationkey <= 24))),
    CONSTRAINT n_nationkey_name_regionkey_domain CHECK ((((n_nationkey = 0) AND (n_name = 'ALGERIA'::bpchar) AND (n_regionkey = 0)) OR ((n_nationkey = 1) AND (n_name = 'ARGENTINA'::bpchar) AND (n_regionkey = 1)) OR ((n_nationkey = 2) AND (n_name = 'BRAZIL'::bpchar) AND (n_regionkey = 1)) OR ((n_nationkey = 3) AND (n_name = 'CANADA'::bpchar) AND (n_regionkey = 1)) OR ((n_nationkey = 4) AND (n_name = 'EGYPT'::bpchar) AND (n_regionkey = 4)) OR ((n_nationkey = 5) AND (n_name = 'ETHIOPIA'::bpchar) AND (n_regionkey = 0)) OR ((n_nationkey = 6) AND (n_name = 'FRANCE'::bpchar) AND (n_regionkey = 3)) OR ((n_nationkey = 7) AND (n_name = 'GERMANY'::bpchar) AND (n_regionkey = 3)) OR ((n_nationkey = 8) AND (n_name = 'INDIA'::bpchar) AND (n_regionkey = 2)) OR ((n_nationkey = 9) AND (n_name = 'INDONESIA'::bpchar) AND (n_regionkey = 2)) OR ((n_nationkey = 10) AND (n_name = 'IRAN'::bpchar) AND (n_regionkey = 4)) OR ((n_nationkey = 11) AND (n_name = 'IRAQ'::bpchar) AND (n_regionkey = 4)) OR ((n_nationkey = 12) AND (n_name = 'JAPAN'::bpchar) AND (n_regionkey = 2)) OR ((n_nationkey = 13) AND (n_name = 'JORDAN'::bpchar) AND (n_regionkey = 4)) OR ((n_nationkey = 14) AND (n_name = 'KENYA'::bpchar) AND (n_regionkey = 0)) OR ((n_nationkey = 15) AND (n_name = 'MOROCCO'::bpchar) AND (n_regionkey = 0)) OR ((n_nationkey = 16) AND (n_name = 'MOZAMBIQUE'::bpchar) AND (n_regionkey = 0)) OR ((n_nationkey = 17) AND (n_name = 'PERU'::bpchar) AND (n_regionkey = 1)) OR ((n_nationkey = 18) AND (n_name = 'CHINA'::bpchar) AND (n_regionkey = 2)) OR ((n_nationkey = 19) AND (n_name = 'ROMANIA'::bpchar) AND (n_regionkey = 3)) OR ((n_nationkey = 20) AND (n_name = 'SAUDI ARABIA'::bpchar) AND (n_regionkey = 4)) OR ((n_nationkey = 21) AND (n_name = 'VIETNAM'::bpchar) AND (n_regionkey = 2)) OR ((n_nationkey = 22) AND (n_name = 'RUSSIA'::bpchar) AND (n_regionkey = 3)) OR ((n_nationkey = 23) AND (n_name = 'UNITED KINGDOM'::bpchar) AND (n_regionkey = 3)) OR ((n_nationkey = 24) AND (n_name = 'UNITED STATES'::bpchar) AND (n_regionkey = 1)))),
    CONSTRAINT n_regionkey_domain CHECK (((n_regionkey >= 0) AND (n_regionkey <= 4)))
);


ALTER TABLE public.nation OWNER TO vaultdb;

--
-- Name: nation_n_nationkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.nation_n_nationkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.nation_n_nationkey_seq OWNER TO vaultdb;

--
-- Name: nation_n_nationkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.nation_n_nationkey_seq OWNED BY public.nation.n_nationkey;


--
-- Name: orders; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.orders (
    o_orderkey integer NOT NULL,
    o_custkey integer,
    o_orderstatus character(1),
    o_totalprice numeric,
    o_orderdate date,
    o_orderpriority character(15),
    o_clerk character(15),
    o_shippriority integer,
    o_comment character varying(79),
    o_orderyear integer,
    CONSTRAINT o_clerk_domain CHECK (((split_part((o_clerk)::text, '#'::text, 1) = 'Clerk'::text) AND (((split_part((o_clerk)::text, '#'::text, 2))::integer >= 1) AND ((split_part((o_clerk)::text, '#'::text, 2))::integer <= 1000)) AND (length(split_part((o_clerk)::text, '#'::text, 2)) = 9) AND (array_length(string_to_array((o_clerk)::text, '#'::text), 1) = 2))),
    CONSTRAINT o_orderdate_domain CHECK (((o_orderdate >= '1992-01-01'::date) AND (o_orderdate <= ('1998-12-31'::date - 151)))),
    CONSTRAINT o_orderkey_domain CHECK (((o_orderkey >= 1) AND (o_orderkey <= 600000))),
    CONSTRAINT o_orderpriority_domain CHECK ((o_orderpriority = ANY (ARRAY['1-URGENT'::bpchar, '2-HIGH'::bpchar, '3-MEDIUM'::bpchar, '4-NOT SPECIFIED'::bpchar, '5-LOW'::bpchar]))),
    CONSTRAINT o_orderstatus_domain CHECK ((o_orderstatus = ANY (ARRAY['F'::bpchar, 'O'::bpchar, 'P'::bpchar]))),
    CONSTRAINT o_shippriority_domain CHECK ((o_shippriority = 0))
);


ALTER TABLE public.orders OWNER TO vaultdb;

--
-- Name: orders_o_orderkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.orders_o_orderkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.orders_o_orderkey_seq OWNER TO vaultdb;

--
-- Name: orders_o_orderkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.orders_o_orderkey_seq OWNED BY public.orders.o_orderkey;


--
-- Name: part; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.part (
    p_partkey integer NOT NULL,
    p_name character varying(55),
    p_mfgr character(25),
    p_brand character(10),
    p_type character varying(25),
    p_size integer,
    p_container character(10),
    p_retailprice numeric,
    p_comment character varying(23),
    CONSTRAINT p_brand_domain CHECK (((split_part((p_brand)::text, '#'::text, 1) = 'Brand'::text) AND (("left"(split_part((p_brand)::text, '#'::text, 2), 1))::integer = (split_part((p_mfgr)::text, '#'::text, 2))::integer) AND ((("right"(split_part((p_brand)::text, '#'::text, 2), 1))::integer >= 1) AND (("right"(split_part((p_brand)::text, '#'::text, 2), 1))::integer <= 5)) AND (length(split_part((p_brand)::text, '#'::text, 2)) = 2) AND (array_length(string_to_array((p_brand)::text, '#'::text), 1) = 2))),
    CONSTRAINT p_container_domain CHECK (((split_part((p_container)::text, ' '::text, 1) = ANY (ARRAY['JUMBO'::text, 'LG'::text, 'MED'::text, 'SM'::text, 'WRAP'::text])) AND (split_part((p_container)::text, ' '::text, 2) = ANY (ARRAY['BAG'::text, 'BOX'::text, 'CAN'::text, 'CASE'::text, 'DRUM'::text, 'JAR'::text, 'PACK'::text, 'PKG'::text])) AND (array_length(string_to_array((p_container)::text, ' '::text), 1) = 2))),
    CONSTRAINT p_mfgr_domain CHECK (((split_part((p_mfgr)::text, '#'::text, 1) = 'Manufacturer'::text) AND (((split_part((p_mfgr)::text, '#'::text, 2))::integer >= 1) AND ((split_part((p_mfgr)::text, '#'::text, 2))::integer <= 5)) AND (array_length(string_to_array((p_mfgr)::text, '#'::text), 1) = 2))),
    CONSTRAINT p_name_domain CHECK (((split_part((p_name)::text, ' '::text, 1) = ANY (ARRAY['almond'::text, 'antique'::text, 'aquamarine'::text, 'azure'::text, 'beige'::text, 'bisque'::text, 'black'::text, 'blanched'::text, 'blue'::text, 'blush'::text, 'brown'::text, 'burlywood'::text, 'burnished'::text, 'chartreuse'::text, 'chiffon'::text, 'chocolate'::text, 'coral'::text, 'cornflower'::text, 'cornsilk'::text, 'cream'::text, 'cyan'::text, 'dark'::text, 'deep'::text, 'dim'::text, 'dodger'::text, 'drab'::text, 'firebrick'::text, 'floral'::text, 'forest'::text, 'frosted'::text, 'gainsboro'::text, 'ghost'::text, 'goldenrod'::text, 'green'::text, 'grey'::text, 'honeydew'::text, 'hot'::text, 'indian'::text, 'ivory'::text, 'khaki'::text, 'lace'::text, 'lavender'::text, 'lawn'::text, 'lemon'::text, 'light'::text, 'lime'::text, 'linen'::text, 'magenta'::text, 'maroon'::text, 'medium'::text, 'metallic'::text, 'midnight'::text, 'mint'::text, 'misty'::text, 'moccasin'::text, 'navajo'::text, 'navy'::text, 'olive'::text, 'orange'::text, 'orchid'::text, 'pale'::text, 'papaya'::text, 'peach'::text, 'peru'::text, 'pink'::text, 'plum'::text, 'powder'::text, 'puff'::text, 'purple'::text, 'red'::text, 'rose'::text, 'rosy'::text, 'royal'::text, 'saddle'::text, 'salmon'::text, 'sandy'::text, 'seashell'::text, 'sienna'::text, 'sky'::text, 'slate'::text, 'smoke'::text, 'snow'::text, 'spring'::text, 'steel'::text, 'tan'::text, 'thistle'::text, 'tomato'::text, 'turquoise'::text, 'violet'::text, 'wheat'::text, 'white'::text, 'yellow'::text])) AND (split_part((p_name)::text, ' '::text, 2) = ANY (ARRAY['almond'::text, 'antique'::text, 'aquamarine'::text, 'azure'::text, 'beige'::text, 'bisque'::text, 'black'::text, 'blanched'::text, 'blue'::text, 'blush'::text, 'brown'::text, 'burlywood'::text, 'burnished'::text, 'chartreuse'::text, 'chiffon'::text, 'chocolate'::text, 'coral'::text, 'cornflower'::text, 'cornsilk'::text, 'cream'::text, 'cyan'::text, 'dark'::text, 'deep'::text, 'dim'::text, 'dodger'::text, 'drab'::text, 'firebrick'::text, 'floral'::text, 'forest'::text, 'frosted'::text, 'gainsboro'::text, 'ghost'::text, 'goldenrod'::text, 'green'::text, 'grey'::text, 'honeydew'::text, 'hot'::text, 'indian'::text, 'ivory'::text, 'khaki'::text, 'lace'::text, 'lavender'::text, 'lawn'::text, 'lemon'::text, 'light'::text, 'lime'::text, 'linen'::text, 'magenta'::text, 'maroon'::text, 'medium'::text, 'metallic'::text, 'midnight'::text, 'mint'::text, 'misty'::text, 'moccasin'::text, 'navajo'::text, 'navy'::text, 'olive'::text, 'orange'::text, 'orchid'::text, 'pale'::text, 'papaya'::text, 'peach'::text, 'peru'::text, 'pink'::text, 'plum'::text, 'powder'::text, 'puff'::text, 'purple'::text, 'red'::text, 'rose'::text, 'rosy'::text, 'royal'::text, 'saddle'::text, 'salmon'::text, 'sandy'::text, 'seashell'::text, 'sienna'::text, 'sky'::text, 'slate'::text, 'smoke'::text, 'snow'::text, 'spring'::text, 'steel'::text, 'tan'::text, 'thistle'::text, 'tomato'::text, 'turquoise'::text, 'violet'::text, 'wheat'::text, 'white'::text, 'yellow'::text])) AND (split_part((p_name)::text, ' '::text, 3) = ANY (ARRAY['almond'::text, 'antique'::text, 'aquamarine'::text, 'azure'::text, 'beige'::text, 'bisque'::text, 'black'::text, 'blanched'::text, 'blue'::text, 'blush'::text, 'brown'::text, 'burlywood'::text, 'burnished'::text, 'chartreuse'::text, 'chiffon'::text, 'chocolate'::text, 'coral'::text, 'cornflower'::text, 'cornsilk'::text, 'cream'::text, 'cyan'::text, 'dark'::text, 'deep'::text, 'dim'::text, 'dodger'::text, 'drab'::text, 'firebrick'::text, 'floral'::text, 'forest'::text, 'frosted'::text, 'gainsboro'::text, 'ghost'::text, 'goldenrod'::text, 'green'::text, 'grey'::text, 'honeydew'::text, 'hot'::text, 'indian'::text, 'ivory'::text, 'khaki'::text, 'lace'::text, 'lavender'::text, 'lawn'::text, 'lemon'::text, 'light'::text, 'lime'::text, 'linen'::text, 'magenta'::text, 'maroon'::text, 'medium'::text, 'metallic'::text, 'midnight'::text, 'mint'::text, 'misty'::text, 'moccasin'::text, 'navajo'::text, 'navy'::text, 'olive'::text, 'orange'::text, 'orchid'::text, 'pale'::text, 'papaya'::text, 'peach'::text, 'peru'::text, 'pink'::text, 'plum'::text, 'powder'::text, 'puff'::text, 'purple'::text, 'red'::text, 'rose'::text, 'rosy'::text, 'royal'::text, 'saddle'::text, 'salmon'::text, 'sandy'::text, 'seashell'::text, 'sienna'::text, 'sky'::text, 'slate'::text, 'smoke'::text, 'snow'::text, 'spring'::text, 'steel'::text, 'tan'::text, 'thistle'::text, 'tomato'::text, 'turquoise'::text, 'violet'::text, 'wheat'::text, 'white'::text, 'yellow'::text])) AND (split_part((p_name)::text, ' '::text, 4) = ANY (ARRAY['almond'::text, 'antique'::text, 'aquamarine'::text, 'azure'::text, 'beige'::text, 'bisque'::text, 'black'::text, 'blanched'::text, 'blue'::text, 'blush'::text, 'brown'::text, 'burlywood'::text, 'burnished'::text, 'chartreuse'::text, 'chiffon'::text, 'chocolate'::text, 'coral'::text, 'cornflower'::text, 'cornsilk'::text, 'cream'::text, 'cyan'::text, 'dark'::text, 'deep'::text, 'dim'::text, 'dodger'::text, 'drab'::text, 'firebrick'::text, 'floral'::text, 'forest'::text, 'frosted'::text, 'gainsboro'::text, 'ghost'::text, 'goldenrod'::text, 'green'::text, 'grey'::text, 'honeydew'::text, 'hot'::text, 'indian'::text, 'ivory'::text, 'khaki'::text, 'lace'::text, 'lavender'::text, 'lawn'::text, 'lemon'::text, 'light'::text, 'lime'::text, 'linen'::text, 'magenta'::text, 'maroon'::text, 'medium'::text, 'metallic'::text, 'midnight'::text, 'mint'::text, 'misty'::text, 'moccasin'::text, 'navajo'::text, 'navy'::text, 'olive'::text, 'orange'::text, 'orchid'::text, 'pale'::text, 'papaya'::text, 'peach'::text, 'peru'::text, 'pink'::text, 'plum'::text, 'powder'::text, 'puff'::text, 'purple'::text, 'red'::text, 'rose'::text, 'rosy'::text, 'royal'::text, 'saddle'::text, 'salmon'::text, 'sandy'::text, 'seashell'::text, 'sienna'::text, 'sky'::text, 'slate'::text, 'smoke'::text, 'snow'::text, 'spring'::text, 'steel'::text, 'tan'::text, 'thistle'::text, 'tomato'::text, 'turquoise'::text, 'violet'::text, 'wheat'::text, 'white'::text, 'yellow'::text])) AND (split_part((p_name)::text, ' '::text, 5) = ANY (ARRAY['almond'::text, 'antique'::text, 'aquamarine'::text, 'azure'::text, 'beige'::text, 'bisque'::text, 'black'::text, 'blanched'::text, 'blue'::text, 'blush'::text, 'brown'::text, 'burlywood'::text, 'burnished'::text, 'chartreuse'::text, 'chiffon'::text, 'chocolate'::text, 'coral'::text, 'cornflower'::text, 'cornsilk'::text, 'cream'::text, 'cyan'::text, 'dark'::text, 'deep'::text, 'dim'::text, 'dodger'::text, 'drab'::text, 'firebrick'::text, 'floral'::text, 'forest'::text, 'frosted'::text, 'gainsboro'::text, 'ghost'::text, 'goldenrod'::text, 'green'::text, 'grey'::text, 'honeydew'::text, 'hot'::text, 'indian'::text, 'ivory'::text, 'khaki'::text, 'lace'::text, 'lavender'::text, 'lawn'::text, 'lemon'::text, 'light'::text, 'lime'::text, 'linen'::text, 'magenta'::text, 'maroon'::text, 'medium'::text, 'metallic'::text, 'midnight'::text, 'mint'::text, 'misty'::text, 'moccasin'::text, 'navajo'::text, 'navy'::text, 'olive'::text, 'orange'::text, 'orchid'::text, 'pale'::text, 'papaya'::text, 'peach'::text, 'peru'::text, 'pink'::text, 'plum'::text, 'powder'::text, 'puff'::text, 'purple'::text, 'red'::text, 'rose'::text, 'rosy'::text, 'royal'::text, 'saddle'::text, 'salmon'::text, 'sandy'::text, 'seashell'::text, 'sienna'::text, 'sky'::text, 'slate'::text, 'smoke'::text, 'snow'::text, 'spring'::text, 'steel'::text, 'tan'::text, 'thistle'::text, 'tomato'::text, 'turquoise'::text, 'violet'::text, 'wheat'::text, 'white'::text, 'yellow'::text])) AND (array_length(string_to_array((p_name)::text, ' '::text), 1) = 5))),
    CONSTRAINT p_partkey_domain CHECK (((p_partkey >= 1) AND (p_partkey <= 20000))),
    CONSTRAINT p_size_domain CHECK (((p_size >= 1) AND (p_size <= 50))),
    CONSTRAINT p_type_domain CHECK (((split_part((p_type)::text, ' '::text, 1) = ANY (ARRAY['ECONOMY'::text, 'LARGE'::text, 'MEDIUM'::text, 'PROMO'::text, 'SMALL'::text, 'STANDARD'::text])) AND (split_part((p_type)::text, ' '::text, 2) = ANY (ARRAY['ANODIZED'::text, 'BRUSHED'::text, 'BURNISHED'::text, 'PLATED'::text, 'POLISHED'::text])) AND (split_part((p_type)::text, ' '::text, 3) = ANY (ARRAY['BRASS'::text, 'COPPER'::text, 'NICKEL'::text, 'STEEL'::text, 'TIN'::text])) AND (array_length(string_to_array((p_type)::text, ' '::text), 1) = 3)))
);


ALTER TABLE public.part OWNER TO vaultdb;

--
-- Name: part_p_partkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.part_p_partkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.part_p_partkey_seq OWNER TO vaultdb;

--
-- Name: part_p_partkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.part_p_partkey_seq OWNED BY public.part.p_partkey;


--
-- Name: partsupp; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.partsupp (
    r bigint,
    ps_partkey integer NOT NULL,
    ps_suppkey integer NOT NULL,
    ps_availqty integer,
    ps_supplycost numeric,
    ps_comment character varying(199),
    CONSTRAINT ps_availqty_domain CHECK (((ps_availqty >= 1) AND (ps_availqty <= 9999))),
    CONSTRAINT ps_supplycost_domain CHECK (((ps_supplycost >= 1.00) AND (ps_supplycost <= 1000.00)))
);


ALTER TABLE public.partsupp OWNER TO vaultdb;

--
-- Name: region; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.region (
    r_regionkey integer NOT NULL,
    r_name character(25),
    r_comment character varying(152),
    CONSTRAINT r_name_domain CHECK ((r_name = ANY (ARRAY['MIDDLE EAST'::bpchar, 'AMERICA'::bpchar, 'ASIA'::bpchar, 'EUROPE'::bpchar, 'AFRICA'::bpchar]))),
    CONSTRAINT r_regionkey_domain CHECK (((r_regionkey >= 0) AND (r_regionkey <= 4))),
    CONSTRAINT r_regionkey_name_domain CHECK ((((r_regionkey = 0) AND (r_name = 'AFRICA'::bpchar)) OR ((r_regionkey = 1) AND (r_name = 'AMERICA'::bpchar)) OR ((r_regionkey = 2) AND (r_name = 'ASIA'::bpchar)) OR ((r_regionkey = 3) AND (r_name = 'EUROPE'::bpchar)) OR ((r_regionkey = 4) AND (r_name = 'MIDDLE EAST'::bpchar))))
);


ALTER TABLE public.region OWNER TO vaultdb;

--
-- Name: region_r_regionkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.region_r_regionkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.region_r_regionkey_seq OWNER TO vaultdb;

--
-- Name: region_r_regionkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.region_r_regionkey_seq OWNED BY public.region.r_regionkey;


--
-- Name: supplier; Type: TABLE; Schema: public; Owner: vaultdb
--

CREATE TABLE public.supplier (
    s_suppkey integer NOT NULL,
    s_name character(25),
    s_address character varying(40),
    s_nationkey integer,
    s_phone character(15),
    s_acctbal numeric,
    s_comment character varying(101),
    CONSTRAINT s_acctbal_domain CHECK (((s_acctbal >= '-999.99'::numeric) AND (s_acctbal <= 9999.99))),
    CONSTRAINT s_name_domain CHECK (((split_part((s_name)::text, '#'::text, 1) = 'Supplier'::text) AND ((split_part((s_name)::text, '#'::text, 2))::integer = s_suppkey) AND (length(split_part((s_name)::text, '#'::text, 2)) = 9) AND (array_length(string_to_array((s_name)::text, '#'::text), 1) = 2))),
    CONSTRAINT s_nationkey_domain CHECK (((s_nationkey >= 0) AND (s_nationkey <= 24))),
    CONSTRAINT s_phone_domain CHECK (((("substring"((s_phone)::text, 1, 2))::integer = (s_nationkey + 10)) AND ("substring"((s_phone)::text, 3, 1) = '-'::text) AND ((("substring"((s_phone)::text, 4, 3))::integer >= 100) AND (("substring"((s_phone)::text, 4, 3))::integer <= 999)) AND ("substring"((s_phone)::text, 7, 1) = '-'::text) AND ((("substring"((s_phone)::text, 8, 3))::integer >= 100) AND (("substring"((s_phone)::text, 8, 3))::integer <= 999)) AND ("substring"((s_phone)::text, 11, 1) = '-'::text) AND ((("substring"((s_phone)::text, 12, 4))::integer >= 1000) AND (("substring"((s_phone)::text, 12, 4))::integer <= 9999)))),
    CONSTRAINT s_suppkey_domain CHECK (((s_suppkey >= 1) AND (s_suppkey <= 1000)))
);


ALTER TABLE public.supplier OWNER TO vaultdb;

--
-- Name: supplier_s_suppkey_seq; Type: SEQUENCE; Schema: public; Owner: vaultdb
--

CREATE SEQUENCE public.supplier_s_suppkey_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.supplier_s_suppkey_seq OWNER TO vaultdb;

--
-- Name: supplier_s_suppkey_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vaultdb
--

ALTER SEQUENCE public.supplier_s_suppkey_seq OWNED BY public.supplier.s_suppkey;


--
-- Name: customer c_custkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.customer ALTER COLUMN c_custkey SET DEFAULT nextval('public.customer_c_custkey_seq'::regclass);


--
-- Name: nation n_nationkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.nation ALTER COLUMN n_nationkey SET DEFAULT nextval('public.nation_n_nationkey_seq'::regclass);


--
-- Name: orders o_orderkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.orders ALTER COLUMN o_orderkey SET DEFAULT nextval('public.orders_o_orderkey_seq'::regclass);


--
-- Name: part p_partkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.part ALTER COLUMN p_partkey SET DEFAULT nextval('public.part_p_partkey_seq'::regclass);


--
-- Name: region r_regionkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.region ALTER COLUMN r_regionkey SET DEFAULT nextval('public.region_r_regionkey_seq'::regclass);


--
-- Name: supplier s_suppkey; Type: DEFAULT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.supplier ALTER COLUMN s_suppkey SET DEFAULT nextval('public.supplier_s_suppkey_seq'::regclass);


--
-- Data for Name: customer; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.customer (c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment) FROM stdin;
3	Customer#000000003	MG9kdTD2WBHm	1	11-719-748-3364	7498.12	AUTOMOBILE	 deposits eat slyly ironic, even instructions. express foxes detect slyly. blithely even accounts abov
4	Customer#000000004	XxVSJsLAGtn	4	14-128-190-5944	2866.83	MACHINERY 	 requests. final, regular ideas sleep final accou
5	Customer#000000005	KvpyuHCplrB84WgAiGV6sYpZq7Tj	3	13-750-942-6364	794.47	HOUSEHOLD 	n accounts will have to unwind. foxes cajole accor
9	Customer#000000009	xKiAFTjUsCuxfeleNqefumTrjS	8	18-338-906-3675	8324.07	FURNITURE 	r theodolites according to the requests wake thinly excuses: pending requests haggle furiousl
10	Customer#000000010	6LrEaV6KR6PLVcgl2ArL Q3rqzLzcT1 v2	5	15-741-346-9870	2753.54	HOUSEHOLD 	es regular deposits haggle. fur
13	Customer#000000013	nsXQu0oVjD7PM659uC3SRSp	3	13-761-547-5974	3857.34	BUILDING  	ounts sleep carefully after the close frays. carefully bold notornis use ironic requests. blithely
14	Customer#000000014	KXkletMlL2JQEA 	1	11-845-129-3851	5266.30	FURNITURE 	, ironic packages across the unus
16	Customer#000000016	cYiaeMLZSMAOQ2 d0W,	10	20-781-609-3107	4681.03	FURNITURE 	kly silent courts. thinly regular theodolites sleep fluffily after 
17	Customer#000000017	izrh 6jdqtp2eqdtbkswDD8SG4SzXruMfIXyR7	2	12-970-682-3487	6.34	AUTOMOBILE	packages wake! blithely even pint
18	Customer#000000018	3txGO AiuFux3zT0Z9NYaFRnZt	6	16-155-215-1315	5494.43	BUILDING  	s sleep. carefully even instructions nag furiously alongside of t
21	Customer#000000021	XYmVpr9yAHDEn	8	18-902-614-8344	1428.25	MACHINERY 	 quickly final accounts integrate blithely furiously u
22	Customer#000000022	QI6p41,FNs5k7RZoCCVPUTkUdYpB	3	13-806-545-9701	591.98	MACHINERY 	s nod furiously above the furiously ironic ideas. 
23	Customer#000000023	OdY W13N7Be3OC5MpgfmcYss0Wn6TKT	3	13-312-472-8245	3332.02	HOUSEHOLD 	deposits. special deposits cajole slyly. fluffily special deposits about the furiously 
25	Customer#000000025	Hp8GyFQgGHFYSilH5tBfe	12	22-603-468-3533	7133.70	FURNITURE 	y. accounts sleep ruthlessly according to the regular theodolites. unusual instructions sleep. ironic, final
27	Customer#000000027	IS8GIyxpBrLpMT0u7	3	13-137-193-2709	5679.84	BUILDING  	 about the carefully ironic pinto beans. accoun
28	Customer#000000028	iVyg0daQ,Tha8x2WPWA9m2529m	8	18-774-241-1462	1007.18	FURNITURE 	 along the regular deposits. furiously final pac
29	Customer#000000029	sJ5adtfyAkCK63df2,vF25zyQMVYE34uh	0	10-773-203-7342	7618.27	FURNITURE 	its after the carefully final platelets x-ray against 
30	Customer#000000030	nJDsELGAavU63Jl0c5NKsKfL8rIJQQkQnYL2QJY	1	11-764-165-5076	9321.01	BUILDING  	lithely final requests. furiously unusual account
37	Customer#000000037	7EV4Pwh,3SboctTWt	8	18-385-235-7162	-917.75	FURNITURE 	ilent packages are carefully among the deposits. furiousl
38	Customer#000000038	a5Ee5e9568R8RLP 2ap7	12	22-306-880-7212	6345.11	HOUSEHOLD 	lar excuses. closely even asymptotes cajole blithely excuses. carefully silent pinto beans sleep carefully fin
39	Customer#000000039	nnbRg,Pvy33dfkorYE FdeZ60	2	12-387-467-6509	6264.31	AUTOMOBILE	tions. slyly silent excuses slee
40	Customer#000000040	gOnGWAyhSV1ofv	3	13-652-915-8939	1335.30	BUILDING  	rges impress after the slyly ironic courts. foxes are. blithely 
41	Customer#000000041	IM9mzmyoxeBmvNw8lA7G3Ydska2nkZF	10	20-917-711-4011	270.95	HOUSEHOLD 	ly regular accounts hang bold, silent packages. unusual foxes haggle slyly above the special, final depo
42	Customer#000000042	ziSrvyyBke	5	15-416-330-4175	8727.01	BUILDING  	ssly according to the pinto beans: carefully special requests across the even, pending accounts wake special
45	Customer#000000045	4v3OcpFgoOmMG,CbnF,4mdC	9	19-715-298-9917	9983.38	AUTOMOBILE	nto beans haggle slyly alongside of t
46	Customer#000000046	eaTXWWm10L9	6	16-357-681-2007	5744.59	AUTOMOBILE	ctions. accounts sleep furiously even requests. regular, regular accounts cajole blithely around the final pa
47	Customer#000000047	b0UgocSqEW5 gdVbhNT	2	12-427-271-9466	274.58	BUILDING  	ions. express, ironic instructions sleep furiously ironic ideas. furi
48	Customer#000000048	0UU iPhBupFvemNB	0	10-508-348-5882	3792.50	BUILDING  	re fluffily pending foxes. pending, bold platelets sleep slyly. even platelets cajo
49	Customer#000000049	cNgAeX7Fqrdf7HQN9EwjUa4nxT,68L FKAxzl	10	20-908-631-4424	4573.94	FURNITURE 	nusual foxes! fluffily pending packages maintain to the regular 
50	Customer#000000050	9SzDYlkzxByyJ1QeTI o	6	16-658-112-3221	4266.13	MACHINERY 	ts. furiously ironic accounts cajole furiously slyly ironic dinos.
51	Customer#000000051	uR,wEaiTvo4	12	22-344-885-4251	855.87	FURNITURE 	eposits. furiously regular requests integrate carefully packages. furious
52	Customer#000000052	7 QOqGqqSy9jfV51BC71jcHJSD0	11	21-186-284-5998	5630.28	HOUSEHOLD 	ic platelets use evenly even accounts. stealthy theodolites cajole furiou
54	Customer#000000054	,k4vf 5vECGWFy,hosTE,	4	14-776-370-4745	868.90	AUTOMOBILE	sual, silent accounts. furiously express accounts cajole special deposits. final, final accounts use furi
55	Customer#000000055	zIRBR4KNEl HzaiV3a i9n6elrxzDEh8r8pDom	10	20-180-440-8525	4572.11	MACHINERY 	ully unusual packages wake bravely bold packages. unusual requests boost deposits! blithely ironic packages ab
56	Customer#000000056	BJYZYJQk4yD5B	10	20-895-685-6920	6530.86	FURNITURE 	. notornis wake carefully. carefully fluffy requests are furiously even accounts. slyly expre
59	Customer#000000059	zLOCP0wh92OtBihgspOGl4	1	11-355-584-3112	3458.60	MACHINERY 	ously final packages haggle blithely after the express deposits. furiou
60	Customer#000000060	FyodhjwMChsZmUz7Jz0H	12	22-480-575-5866	2741.87	MACHINERY 	latelets. blithely unusual courts boost furiously about the packages. blithely final instruct
62	Customer#000000062	upJK2Dnw13,	7	17-361-978-7059	595.61	MACHINERY 	kly special dolphins. pinto beans are slyly. quickly regular accounts are furiously a
64	Customer#000000064	MbCeGY20kaKK3oalJD,OT	3	13-558-731-7204	-646.64	BUILDING  	structions after the quietly ironic theodolites cajole be
67	Customer#000000067	rfG0cOgtr5W8 xILkwp9fpCS8	9	19-403-114-4356	8166.59	MACHINERY 	indle furiously final, even theodo
68	Customer#000000068	o8AibcCRkXvQFh8hF,7o	12	22-918-832-2411	6853.37	HOUSEHOLD 	 pending pinto beans impress realms. final dependencies 
69	Customer#000000069	Ltx17nO9Wwhtdbe9QZVxNgP98V7xW97uvSH1prEw	9	19-225-978-5670	1709.28	HOUSEHOLD 	thely final ideas around the quickly final dependencies affix carefully quickly final theodolites. final accounts c
71	Customer#000000071	TlGalgdXWBmMV,6agLyWYDyIz9MKzcY8gl,w6t1B	7	17-710-812-5403	-611.19	HOUSEHOLD 	g courts across the regular, final pinto beans are blithely pending ac
72	Customer#000000072	putjlmskxE,zs,HqeIA9Wqu7dhgH5BVCwDwHHcf	2	12-759-144-9689	-362.86	FURNITURE 	ithely final foxes sleep always quickly bold accounts. final wat
73	Customer#000000073	8IhIxreu4Ug6tt5mog4	0	10-473-439-3214	4288.50	BUILDING  	usual, unusual packages sleep busily along the furiou
74	Customer#000000074	IkJHCA3ZThF7qL7VKcrU nRLl,kylf 	4	14-199-862-7209	2764.43	MACHINERY 	onic accounts. blithely slow packages would haggle carefully. qui
76	Customer#000000076	m3sbCvjMOHyaOofH,e UkGPtqc4	0	10-349-718-3044	5745.33	FURNITURE 	pecial deposits. ironic ideas boost blithely according to the closely ironic theodolites! furiously final deposits n
78	Customer#000000078	HBOta,ZNqpg3U2cSL0kbrftkPwzX	9	19-960-700-9191	7136.97	FURNITURE 	ests. blithely bold pinto beans h
80	Customer#000000080	K,vtXp8qYB 	0	10-267-172-7101	7383.53	FURNITURE 	tect among the dependencies. bold accounts engage closely even pinto beans. ca
84	Customer#000000084	lpXz6Fwr9945rnbtMc8PlueilS1WmASr CB	11	21-546-818-3802	5174.71	FURNITURE 	ly blithe foxes. special asymptotes haggle blithely against the furiously regular depo
85	Customer#000000085	siRerlDwiolhYR 8FgksoezycLj	5	15-745-585-8219	3386.64	FURNITURE 	ronic ideas use above the slowly pendin
86	Customer#000000086	US6EGGHXbTTXPL9SBsxQJsuvy	0	10-677-951-2353	3306.32	HOUSEHOLD 	quests. pending dugouts are carefully aroun
91	Customer#000000091	S8OMYFrpHwoNHaGBeuS6E 6zhHGZiprw1b7 q	8	18-239-400-3677	4643.14	AUTOMOBILE	onic accounts. fluffily silent pinto beans boost blithely according to the fluffily exp
92	Customer#000000092	obP PULk2LH LqNF,K9hcbNqnLAkJVsl5xqSrY,	2	12-446-416-8471	1182.91	MACHINERY 	. pinto beans hang slyly final deposits. ac
93	Customer#000000093	EHXBr2QGdh	7	17-359-388-5266	2182.52	MACHINERY 	press deposits. carefully regular platelets r
94	Customer#000000094	IfVNIN9KtkScJ9dUjK3Pg5gY1aFeaXewwf	9	19-953-499-8833	5500.11	HOUSEHOLD 	latelets across the bold, final requests sleep according to the fluffily bold accounts. unusual deposits amon
96	Customer#000000096	vWLOrmXhRR	8	18-422-845-1202	6323.92	AUTOMOBILE	press requests believe furiously. carefully final instructions snooze carefully. 
98	Customer#000000098	7yiheXNSpuEAwbswDW	12	22-885-845-6889	-551.37	BUILDING  	ages. furiously pending accounts are quickly carefully final foxes: busily pe
101	Customer#000000101	sMmL2rNeHDltovSm Y	2	12-514-298-3699	7470.96	MACHINERY 	 sleep. pending packages detect slyly ironic pack
103	Customer#000000103	8KIsQX4LJ7QMsj6DrtFtXu0nUEdV,8a	9	19-216-107-2107	2757.45	BUILDING  	furiously pending notornis boost slyly around the blithely ironic ideas? final, even instructions cajole fl
104	Customer#000000104	9mcCK L7rt0SwiYtrbO88DiZS7U d7M	10	20-966-284-8065	-588.38	FURNITURE 	rate carefully slyly special pla
105	Customer#000000105	4iSJe4L SPjg7kJj98Yz3z0B	10	20-793-553-6417	9091.82	MACHINERY 	l pains cajole even accounts. quietly final instructi
106	Customer#000000106	xGCOEAUjUNG	1	11-751-989-4627	3288.42	MACHINERY 	lose slyly. ironic accounts along the evenly regular theodolites wake about the special, final gifts. 
108	Customer#000000108	GPoeEvpKo1	5	15-908-619-7526	2259.38	BUILDING  	refully ironic deposits sleep. regular, unusual requests wake slyly
110	Customer#000000110	mymPfgphaYXNYtk	10	20-893-536-2069	7462.99	AUTOMOBILE	nto beans cajole around the even, final deposits. quickly bold packages according to the furiously regular dept
113	Customer#000000113	eaOl5UBXIvdY57rglaIzqvfPD,MYfK	12	22-302-930-4756	2912.00	BUILDING  	usly regular theodolites boost furiously doggedly pending instructio
115	Customer#000000115	0WFt1IXENmUT2BgbsB0ShVKJZt0HCBCbFl0aHc	8	18-971-699-1843	7508.92	HOUSEHOLD 	sits haggle above the carefully ironic theodolite
119	Customer#000000119	M1ETOIecuvH8DtM0Y0nryXfW	7	17-697-919-8406	3930.35	FURNITURE 	express ideas. blithely ironic foxes thrash. special acco
120	Customer#000000120	zBNna00AEInqyO1	12	22-291-534-1571	363.75	MACHINERY 	 quickly. slyly ironic requests cajole blithely furiously final dependen
122	Customer#000000122	yp5slqoNd26lAENZW3a67wSfXA6hTF	3	13-702-694-4520	7865.46	HOUSEHOLD 	 the special packages hinder blithely around the permanent requests. bold depos
123	Customer#000000123	YsOnaaER8MkvK5cpf4VSlq	5	15-817-151-1168	5897.83	BUILDING  	ependencies. regular, ironic requests are fluffily regu
128	Customer#000000128	AmKUMlJf2NRHcKGmKjLS	4	14-280-874-8044	-986.96	HOUSEHOLD 	ing packages integrate across the slyly unusual dugouts. blithely silent ideas sublate carefully. blithely expr
129	Customer#000000129	q7m7rbMM0BpaCdmxloCgBDRCleXsXkdD8kf	7	17-415-148-7416	9127.27	HOUSEHOLD 	 unusual deposits boost carefully furiously silent ideas. pending accounts cajole slyly across
130	Customer#000000130	RKPx2OfZy0Vn 8wGWZ7F2EAvmMORl1k8iH	9	19-190-993-9281	5073.58	HOUSEHOLD 	ix slowly. express packages along the furiously ironic requests integrate daringly deposits. fur
131	Customer#000000131	jyN6lAjb1FtH10rMC,XzlWyCBrg75	11	21-840-210-3572	8595.53	HOUSEHOLD 	jole special packages. furiously final dependencies about the furiously speci
132	Customer#000000132	QM5YabAsTLp9	4	14-692-150-9717	162.57	HOUSEHOLD 	uickly carefully special theodolites. carefully regular requests against the blithely unusual instructions 
134	Customer#000000134	sUiZ78QCkTQPICKpA9OBzkUp2FM	11	21-200-159-5932	4608.90	BUILDING  	yly fluffy foxes boost final ideas. b
136	Customer#000000136	QoLsJ0v5C1IQbh,DS1	7	17-501-210-4726	-842.39	FURNITURE 	ackages sleep ironic, final courts. even requests above the blithely bold requests g
138	Customer#000000138	5uyLAeY7HIGZqtu66Yn08f	5	15-394-860-4589	430.59	MACHINERY 	ts doze on the busy ideas. regular
139	Customer#000000139	3ElvBwudHKL02732YexGVFVt 	9	19-140-352-1403	7897.78	MACHINERY 	nstructions. quickly ironic ideas are carefully. bold, 
140	Customer#000000140	XRqEPiKgcETII,iOLDZp5jA	4	14-273-885-6505	9963.15	MACHINERY 	ies detect slyly ironic accounts. slyly ironic theodolites hag
141	Customer#000000141	5IW,WROVnikc3l7DwiUDGQNGsLBGOL6Dc0	1	11-936-295-6204	6706.14	FURNITURE 	packages nag furiously. carefully unusual accounts snooze according to the fluffily regular pinto beans. slyly spec
142	Customer#000000142	AnJ5lxtLjioClr2khl9pb8NLxG2,	9	19-407-425-2584	2209.81	AUTOMOBILE	. even, express theodolites upo
144	Customer#000000144	VxYZ3ebhgbltnetaGjNC8qCccjYU05 fePLOno8y	1	11-717-379-4478	6417.31	MACHINERY 	ges. slyly regular accounts are slyly. bold, idle reque
146	Customer#000000146	GdxkdXG9u7iyI1,,y5tq4ZyrcEy	3	13-835-723-3223	3328.68	FURNITURE 	ffily regular dinos are slyly unusual requests. slyly specia
148	Customer#000000148	BhSPlEWGvIJyT9swk vCWE	11	21-562-498-6636	2135.60	HOUSEHOLD 	ing to the carefully ironic requests. carefully regular dependencies about the theodolites wake furious
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
1	156	4	1	17	17954.55	0.04	0.02	N	O	1996-03-13	1996-02-12	1996-03-22	DELIVER IN PERSON        	TRUCK     	egular courts above the
1	68	9	2	36	34850.16	0.09	0.06	N	O	1996-04-12	1996-02-28	1996-04-20	TAKE BACK RETURN         	MAIL      	ly final dependencies: slyly bold 
1	64	5	3	8	7712.48	0.10	0.02	N	O	1996-01-29	1996-03-05	1996-01-31	TAKE BACK RETURN         	REG AIR   	riously. regular, express dep
1	3	6	4	28	25284.00	0.09	0.06	N	O	1996-04-21	1996-03-30	1996-05-16	NONE                     	AIR       	lites. fluffily even de
1	25	8	5	24	22200.48	0.10	0.04	N	O	1996-03-30	1996-03-14	1996-04-01	NONE                     	FOB       	 pending foxes. slyly re
1	16	3	6	32	29312.32	0.07	0.02	N	O	1996-01-30	1996-02-07	1996-02-03	DELIVER IN PERSON        	MAIL      	arefully slyly ex
5	109	10	1	15	15136.50	0.02	0.04	R	F	1994-10-31	1994-08-31	1994-11-20	NONE                     	AIR       	ts wake furiously 
5	124	5	2	26	26627.12	0.07	0.08	R	F	1994-10-16	1994-09-25	1994-10-19	NONE                     	FOB       	sts use slyly quickly special instruc
5	38	4	3	50	46901.50	0.08	0.03	A	F	1994-08-08	1994-10-13	1994-08-26	DELIVER IN PERSON        	AIR       	eodolites. fluffily unusual
6	140	6	1	37	38485.18	0.08	0.03	A	F	1992-04-27	1992-05-15	1992-05-02	TAKE BACK RETURN         	TRUCK     	p furiously special foxes
7	183	4	1	12	12998.16	0.07	0.03	N	O	1996-05-07	1996-03-13	1996-06-03	TAKE BACK RETURN         	FOB       	ss pinto beans wake against th
7	146	3	2	9	9415.26	0.08	0.08	N	O	1996-02-01	1996-03-02	1996-02-19	TAKE BACK RETURN         	SHIP      	es. instructions
7	95	8	3	46	45774.14	0.10	0.07	N	O	1996-01-15	1996-03-27	1996-02-03	COLLECT COD              	MAIL      	 unusual reques
7	164	5	4	28	29796.48	0.03	0.04	N	O	1996-03-21	1996-04-08	1996-04-20	NONE                     	FOB       	. slyly special requests haggl
7	152	4	5	38	39981.70	0.08	0.01	N	O	1996-02-11	1996-02-24	1996-02-18	DELIVER IN PERSON        	TRUCK     	ns haggle carefully ironic deposits. bl
7	80	10	6	35	34302.80	0.06	0.03	N	O	1996-01-16	1996-02-23	1996-01-22	TAKE BACK RETURN         	FOB       	jole. excuses wake carefully alongside of 
7	158	3	7	5	5290.75	0.04	0.02	N	O	1996-02-10	1996-03-26	1996-02-13	NONE                     	FOB       	ithely regula
32	83	4	1	28	27526.24	0.05	0.08	N	O	1995-10-23	1995-08-27	1995-10-26	TAKE BACK RETURN         	TRUCK     	sleep quickly. req
32	198	10	2	32	35142.08	0.02	0.00	N	O	1995-08-14	1995-10-07	1995-08-27	COLLECT COD              	AIR       	lithely regular deposits. fluffily 
32	45	2	3	2	1890.08	0.09	0.02	N	O	1995-08-07	1995-10-07	1995-08-23	DELIVER IN PERSON        	AIR       	 express accounts wake according to the
32	3	8	4	4	3612.00	0.09	0.03	N	O	1995-08-04	1995-10-01	1995-09-03	NONE                     	REG AIR   	e slyly final pac
32	86	7	5	44	43387.52	0.05	0.06	N	O	1995-08-28	1995-08-20	1995-09-14	DELIVER IN PERSON        	AIR       	symptotes nag according to the ironic depo
32	12	6	6	6	5472.06	0.04	0.03	N	O	1995-07-21	1995-09-23	1995-07-25	COLLECT COD              	RAIL      	 gifts cajole carefully.
33	62	7	1	31	29823.86	0.09	0.04	A	F	1993-10-29	1993-12-19	1993-11-08	COLLECT COD              	TRUCK     	ng to the furiously ironic package
33	61	8	2	32	30753.92	0.02	0.05	A	F	1993-12-09	1994-01-04	1993-12-28	COLLECT COD              	MAIL      	gular theodolites
33	138	4	3	5	5190.65	0.05	0.03	A	F	1993-12-09	1993-12-25	1993-12-23	TAKE BACK RETURN         	AIR       	. stealthily bold exc
33	34	5	4	41	38295.23	0.09	0.00	R	F	1993-11-09	1994-01-24	1993-11-11	TAKE BACK RETURN         	MAIL      	unusual packages doubt caref
34	89	10	1	13	12858.04	0.00	0.07	N	O	1998-10-23	1998-09-14	1998-11-06	NONE                     	REG AIR   	nic accounts. deposits are alon
34	90	1	2	22	21781.98	0.08	0.06	N	O	1998-10-09	1998-10-16	1998-10-12	NONE                     	FOB       	thely slyly p
34	170	7	3	6	6421.02	0.02	0.06	N	O	1998-10-30	1998-09-20	1998-11-05	NONE                     	FOB       	ar foxes sleep 
35	1	4	1	24	21624.00	0.02	0.00	N	O	1996-02-21	1996-01-03	1996-03-18	TAKE BACK RETURN         	FOB       	, regular tithe
35	162	1	2	34	36113.44	0.06	0.08	N	O	1996-01-22	1996-01-06	1996-01-27	DELIVER IN PERSON        	RAIL      	s are carefully against the f
35	121	4	3	7	7147.84	0.06	0.04	N	O	1996-01-19	1995-12-22	1996-01-29	NONE                     	MAIL      	 the carefully regular 
35	86	7	4	25	24652.00	0.06	0.05	N	O	1995-11-26	1995-12-25	1995-12-21	DELIVER IN PERSON        	SHIP      	 quickly unti
35	120	7	5	34	34684.08	0.08	0.06	N	O	1995-11-08	1996-01-15	1995-11-26	COLLECT COD              	MAIL      	. silent, unusual deposits boost
35	31	7	6	28	26068.84	0.03	0.02	N	O	1996-02-01	1995-12-24	1996-02-28	COLLECT COD              	RAIL      	ly alongside of 
65	60	5	1	26	24961.56	0.03	0.03	A	F	1995-04-20	1995-04-25	1995-05-13	NONE                     	TRUCK     	pending deposits nag even packages. ca
65	74	3	2	22	21429.54	0.00	0.05	N	O	1995-07-17	1995-06-04	1995-07-19	COLLECT COD              	FOB       	 ideas. special, r
65	2	5	3	21	18942.00	0.09	0.07	N	O	1995-07-06	1995-05-14	1995-07-31	DELIVER IN PERSON        	RAIL      	bove the even packages. accounts nag carefu
66	116	10	1	31	31499.41	0.00	0.08	R	F	1994-02-19	1994-03-11	1994-02-20	TAKE BACK RETURN         	RAIL      	ut the unusual accounts sleep at the bo
66	174	5	2	41	44040.97	0.04	0.07	A	F	1994-02-21	1994-03-01	1994-03-18	COLLECT COD              	AIR       	 regular de
68	8	1	1	3	2724.00	0.05	0.02	N	O	1998-07-04	1998-06-05	1998-07-21	NONE                     	RAIL      	fully special instructions cajole. furious
68	176	4	2	46	49503.82	0.02	0.05	N	O	1998-06-26	1998-06-07	1998-07-05	NONE                     	MAIL      	 requests are unusual, regular pinto 
68	35	1	3	46	43011.38	0.04	0.05	N	O	1998-08-13	1998-07-08	1998-08-29	NONE                     	RAIL      	egular dependencies affix ironically along 
68	95	9	4	20	19901.80	0.07	0.01	N	O	1998-06-27	1998-05-23	1998-07-02	NONE                     	REG AIR   	 excuses integrate fluffily 
68	83	4	5	27	26543.16	0.03	0.06	N	O	1998-06-19	1998-06-25	1998-06-29	DELIVER IN PERSON        	SHIP      	ccounts. deposits use. furiously
68	103	6	6	30	30093.00	0.05	0.06	N	O	1998-08-11	1998-07-11	1998-08-14	NONE                     	RAIL      	oxes are slyly blithely fin
68	140	6	7	41	42645.74	0.09	0.08	N	O	1998-06-24	1998-06-27	1998-07-06	NONE                     	SHIP      	eposits nag special ideas. furiousl
69	116	10	1	48	48773.28	0.01	0.07	A	F	1994-08-17	1994-08-11	1994-09-08	NONE                     	TRUCK     	regular epitaphs. carefully even ideas hag
69	105	10	2	32	32163.20	0.08	0.06	A	F	1994-08-24	1994-08-17	1994-08-31	NONE                     	REG AIR   	s sleep carefully bold, 
69	138	4	3	17	17648.21	0.09	0.00	A	F	1994-07-02	1994-07-07	1994-07-03	TAKE BACK RETURN         	AIR       	final, pending instr
69	38	9	4	3	2814.09	0.09	0.04	R	F	1994-06-06	1994-07-27	1994-06-15	NONE                     	MAIL      	 blithely final d
69	93	6	5	42	41709.78	0.07	0.04	R	F	1994-07-31	1994-07-26	1994-08-28	DELIVER IN PERSON        	REG AIR   	tect regular, speci
69	19	3	6	23	21137.23	0.05	0.00	A	F	1994-10-03	1994-08-06	1994-10-24	NONE                     	SHIP      	nding accounts ca
71	62	3	1	25	24051.50	0.09	0.07	N	O	1998-04-10	1998-04-22	1998-04-11	COLLECT COD              	FOB       	ckly. slyly
71	66	1	2	3	2898.18	0.09	0.07	N	O	1998-05-23	1998-04-03	1998-06-02	COLLECT COD              	SHIP      	y. pinto beans haggle after the
71	35	1	3	45	42076.35	0.00	0.07	N	O	1998-02-23	1998-03-20	1998-03-24	DELIVER IN PERSON        	SHIP      	 ironic packages believe blithely a
71	97	9	4	33	32903.97	0.00	0.01	N	O	1998-04-12	1998-03-20	1998-04-15	NONE                     	FOB       	 serve quickly fluffily bold deposi
71	104	7	5	39	39159.90	0.08	0.06	N	O	1998-01-29	1998-04-07	1998-02-18	DELIVER IN PERSON        	RAIL      	l accounts sleep across the pack
71	196	9	6	34	37270.46	0.04	0.01	N	O	1998-03-05	1998-04-22	1998-03-30	DELIVER IN PERSON        	TRUCK     	s cajole. 
97	120	4	1	13	13261.56	0.00	0.02	R	F	1993-04-01	1993-04-04	1993-04-08	NONE                     	TRUCK     	ayers cajole against the furiously
97	50	7	2	37	35151.85	0.02	0.06	A	F	1993-04-13	1993-03-30	1993-04-14	DELIVER IN PERSON        	SHIP      	ic requests boost carefully quic
97	78	6	3	19	18583.33	0.06	0.08	R	F	1993-05-14	1993-03-05	1993-05-25	TAKE BACK RETURN         	RAIL      	gifts. furiously ironic packages cajole. 
98	41	2	1	28	26349.12	0.06	0.07	A	F	1994-12-24	1994-10-25	1995-01-16	COLLECT COD              	REG AIR   	 pending, regular accounts s
98	110	7	2	1	1010.11	0.00	0.00	A	F	1994-12-01	1994-12-12	1994-12-15	DELIVER IN PERSON        	TRUCK     	. unusual instructions against
98	45	6	3	14	13230.56	0.05	0.02	A	F	1994-12-30	1994-11-22	1995-01-27	COLLECT COD              	AIR       	 cajole furiously. blithely ironic ideas 
98	168	9	4	10	10681.60	0.03	0.03	A	F	1994-10-23	1994-11-08	1994-11-09	COLLECT COD              	RAIL      	 carefully. quickly ironic ideas
100	63	4	1	28	26965.68	0.04	0.05	N	O	1998-05-08	1998-05-13	1998-06-07	COLLECT COD              	TRUCK     	sts haggle. slowl
100	116	10	2	22	22354.42	0.00	0.07	N	O	1998-06-24	1998-04-12	1998-06-29	DELIVER IN PERSON        	SHIP      	nto beans alongside of the fi
100	47	4	3	46	43563.84	0.03	0.04	N	O	1998-05-02	1998-04-10	1998-05-22	TAKE BACK RETURN         	SHIP      	ular accounts. even
100	39	10	4	14	13146.42	0.06	0.03	N	O	1998-05-22	1998-05-01	1998-06-03	COLLECT COD              	MAIL      	y. furiously ironic ideas gr
100	54	6	5	37	35299.85	0.05	0.00	N	O	1998-03-06	1998-04-16	1998-03-31	TAKE BACK RETURN         	TRUCK     	nd the quickly s
101	119	9	1	49	49936.39	0.10	0.00	N	O	1996-06-21	1996-05-27	1996-06-29	DELIVER IN PERSON        	REG AIR   	ts-- final packages sleep furiousl
101	164	9	2	36	38309.76	0.00	0.01	N	O	1996-05-19	1996-05-01	1996-06-04	DELIVER IN PERSON        	AIR       	tes. blithely pending dolphins x-ray f
101	139	5	3	12	12469.56	0.06	0.02	N	O	1996-03-29	1996-04-20	1996-04-12	COLLECT COD              	MAIL      	. quickly regular
128	107	10	1	38	38269.80	0.06	0.01	A	F	1992-09-01	1992-08-27	1992-10-01	TAKE BACK RETURN         	FOB       	 cajole careful
129	3	6	1	46	41538.00	0.08	0.02	R	F	1993-02-15	1993-01-24	1993-03-05	COLLECT COD              	TRUCK     	uietly bold theodolites. fluffil
129	186	7	2	36	39102.48	0.01	0.02	A	F	1992-11-25	1992-12-25	1992-12-09	TAKE BACK RETURN         	REG AIR   	packages are care
129	40	6	3	33	31021.32	0.04	0.06	A	F	1993-01-08	1993-02-14	1993-01-29	COLLECT COD              	SHIP      	sts nag bravely. fluffily
129	136	7	4	34	35228.42	0.00	0.01	R	F	1993-01-29	1993-02-14	1993-02-10	COLLECT COD              	MAIL      	quests. express ideas
129	32	8	5	24	22368.72	0.06	0.00	A	F	1992-12-07	1993-01-02	1992-12-11	TAKE BACK RETURN         	FOB       	uests. foxes cajole slyly after the ca
129	78	6	6	22	21517.54	0.06	0.01	R	F	1993-02-15	1993-01-31	1993-02-24	COLLECT COD              	SHIP      	e. fluffily regular 
129	169	6	7	1	1069.16	0.05	0.04	R	F	1993-01-26	1993-01-08	1993-02-24	DELIVER IN PERSON        	FOB       	e carefully blithely bold dolp
130	129	10	1	14	14407.68	0.08	0.05	A	F	1992-08-15	1992-07-25	1992-09-13	COLLECT COD              	RAIL      	 requests. final instruction
130	2	5	2	48	43296.00	0.03	0.02	R	F	1992-07-01	1992-07-12	1992-07-24	NONE                     	AIR       	lithely alongside of the regu
130	12	3	3	18	16416.18	0.04	0.08	A	F	1992-07-04	1992-06-14	1992-07-29	DELIVER IN PERSON        	MAIL      	 slyly ironic decoys abou
130	116	6	4	13	13209.43	0.09	0.02	R	F	1992-06-26	1992-07-29	1992-07-05	NONE                     	FOB       	 pending dolphins sleep furious
130	70	7	5	31	30072.17	0.06	0.05	R	F	1992-09-01	1992-07-18	1992-09-02	TAKE BACK RETURN         	RAIL      	thily about the ruth
131	168	7	1	45	48067.20	0.10	0.02	R	F	1994-09-14	1994-09-02	1994-10-04	NONE                     	FOB       	ironic, bold accounts. careful
131	45	8	2	50	47252.00	0.02	0.04	A	F	1994-09-17	1994-08-10	1994-09-21	NONE                     	SHIP      	ending requests. final, ironic pearls slee
131	190	1	3	4	4360.76	0.04	0.03	A	F	1994-09-20	1994-08-30	1994-09-23	COLLECT COD              	REG AIR   	 are carefully slyly i
132	141	8	1	18	18740.52	0.00	0.08	R	F	1993-07-10	1993-08-05	1993-07-13	NONE                     	TRUCK     	ges. platelets wake furio
132	120	1	2	43	43865.16	0.01	0.08	R	F	1993-09-01	1993-08-16	1993-09-22	NONE                     	TRUCK     	y pending theodolites
132	115	6	3	32	32483.52	0.04	0.04	A	F	1993-07-12	1993-08-05	1993-08-05	COLLECT COD              	TRUCK     	d instructions hagg
132	29	2	4	23	21367.46	0.10	0.00	A	F	1993-06-16	1993-08-27	1993-06-23	DELIVER IN PERSON        	AIR       	refully blithely bold acco
161	103	10	1	19	19058.90	0.01	0.01	A	F	1994-12-13	1994-11-19	1994-12-26	DELIVER IN PERSON        	TRUCK     	, regular sheaves sleep along
162	190	1	1	2	2180.38	0.02	0.01	N	O	1995-09-02	1995-06-17	1995-09-08	COLLECT COD              	FOB       	es! final somas integrate
165	34	5	1	3	2802.09	0.01	0.08	R	F	1993-03-29	1993-03-06	1993-04-12	DELIVER IN PERSON        	REG AIR   	riously requests. depos
165	162	7	2	43	45672.88	0.08	0.05	R	F	1993-02-27	1993-04-19	1993-03-03	DELIVER IN PERSON        	TRUCK     	jole slyly according 
165	59	1	3	15	14385.75	0.00	0.05	R	F	1993-04-10	1993-03-29	1993-05-01	COLLECT COD              	SHIP      	 bold packages mainta
165	140	1	4	49	50966.86	0.07	0.06	A	F	1993-02-20	1993-04-02	1993-03-10	COLLECT COD              	REG AIR   	uses sleep slyly ruthlessly regular a
165	156	4	5	27	28516.05	0.01	0.04	R	F	1993-04-27	1993-03-04	1993-05-13	NONE                     	MAIL      	around the ironic, even orb
193	93	5	1	9	8937.81	0.06	0.06	A	F	1993-09-17	1993-10-08	1993-09-30	COLLECT COD              	TRUCK     	against the fluffily regular d
193	154	6	2	15	15812.25	0.02	0.07	R	F	1993-11-22	1993-10-09	1993-12-05	TAKE BACK RETURN         	SHIP      	ffily. regular packages d
193	94	6	3	23	22864.07	0.06	0.05	A	F	1993-08-21	1993-10-11	1993-09-02	DELIVER IN PERSON        	TRUCK     	ly even accounts wake blithely bold
194	3	6	1	17	15351.00	0.05	0.04	R	F	1992-05-24	1992-05-22	1992-05-30	COLLECT COD              	AIR       	 regular deposi
194	184	5	2	1	1084.18	0.04	0.06	R	F	1992-04-30	1992-05-18	1992-05-23	NONE                     	REG AIR   	 regular theodolites. regular, iron
194	66	1	3	13	12558.78	0.08	0.08	A	F	1992-05-07	1992-06-18	1992-05-10	NONE                     	AIR       	about the blit
194	146	7	4	36	37661.04	0.00	0.05	R	F	1992-05-21	1992-05-18	1992-05-27	TAKE BACK RETURN         	RAIL      	pecial packages wake after the slyly r
194	57	2	5	8	7656.40	0.04	0.00	R	F	1992-07-06	1992-06-25	1992-07-11	COLLECT COD              	FOB       	uriously unusual excuses
194	149	6	6	16	16786.24	0.06	0.03	A	F	1992-05-14	1992-06-14	1992-05-21	TAKE BACK RETURN         	TRUCK     	y regular requests. furious
194	168	7	7	21	22431.36	0.02	0.01	R	F	1992-05-06	1992-05-20	1992-05-07	COLLECT COD              	REG AIR   	accounts detect quickly dogged 
195	85	6	1	6	5910.48	0.04	0.02	A	F	1994-01-09	1994-03-27	1994-01-28	COLLECT COD              	REG AIR   	y, even deposits haggle carefully. bli
195	94	8	2	41	40757.69	0.05	0.07	A	F	1994-02-24	1994-02-11	1994-03-20	NONE                     	TRUCK     	rts detect in place of t
195	86	7	3	34	33526.72	0.08	0.08	R	F	1994-01-31	1994-02-11	1994-02-12	NONE                     	TRUCK     	 cajole furiously bold i
195	86	7	4	41	40429.28	0.06	0.04	R	F	1994-03-14	1994-03-13	1994-04-09	COLLECT COD              	RAIL      	ggle fluffily foxes. fluffily ironic ex
224	151	2	1	16	16818.40	0.04	0.00	A	F	1994-08-01	1994-07-30	1994-08-27	DELIVER IN PERSON        	MAIL      	y unusual foxes 
224	109	2	2	34	34309.40	0.04	0.08	R	F	1994-07-13	1994-08-25	1994-07-31	COLLECT COD              	TRUCK     	 carefully. final platelets 
224	190	1	3	41	44697.79	0.07	0.04	A	F	1994-09-01	1994-09-15	1994-09-02	TAKE BACK RETURN         	SHIP      	after the furiou
224	167	4	4	12	12805.92	0.08	0.06	R	F	1994-10-12	1994-08-29	1994-10-20	DELIVER IN PERSON        	MAIL      	uriously regular packages. slyly fina
224	94	7	5	45	44734.05	0.07	0.07	R	F	1994-08-14	1994-09-02	1994-08-27	COLLECT COD              	AIR       	leep furiously regular requests. furiousl
224	51	3	6	4	3804.20	0.02	0.00	R	F	1994-09-08	1994-08-24	1994-10-04	DELIVER IN PERSON        	FOB       	tructions 
226	97	9	1	4	3988.36	0.00	0.00	R	F	1993-03-31	1993-04-30	1993-04-10	NONE                     	TRUCK     	c foxes integrate carefully against th
226	138	4	2	46	47753.98	0.06	0.01	A	F	1993-07-06	1993-04-24	1993-07-13	COLLECT COD              	FOB       	s. carefully bold accounts cajol
226	38	4	3	35	32831.05	0.09	0.03	A	F	1993-03-31	1993-05-18	1993-04-01	NONE                     	RAIL      	osits cajole. final, even foxes a
226	41	10	4	45	42346.80	0.10	0.02	R	F	1993-04-17	1993-05-27	1993-05-11	DELIVER IN PERSON        	AIR       	 carefully pending pi
226	118	8	5	2	2036.22	0.07	0.02	R	F	1993-03-26	1993-04-13	1993-04-20	TAKE BACK RETURN         	SHIP      	al platelets. express somas 
226	83	4	6	48	47187.84	0.02	0.00	A	F	1993-06-11	1993-05-15	1993-06-19	NONE                     	REG AIR   	efully silent packages. final deposit
226	118	8	7	14	14253.54	0.09	0.00	R	F	1993-05-20	1993-06-05	1993-05-27	COLLECT COD              	MAIL      	ep carefully regular accounts. ironic
227	166	1	1	19	20257.04	0.05	0.06	N	O	1995-12-10	1996-01-30	1995-12-26	NONE                     	RAIL      	s cajole furiously a
227	175	3	2	24	25804.08	0.07	0.07	N	O	1996-02-03	1995-12-24	1996-02-12	COLLECT COD              	SHIP      	uses across the blithe dependencies cajol
228	5	8	1	3	2715.00	0.10	0.08	A	F	1993-05-20	1993-04-08	1993-05-26	DELIVER IN PERSON        	SHIP      	ckages. sly
230	186	7	1	46	49964.28	0.09	0.00	R	F	1994-02-03	1994-01-15	1994-02-23	TAKE BACK RETURN         	SHIP      	old packages ha
230	195	7	2	6	6571.14	0.03	0.08	A	F	1994-01-26	1994-01-25	1994-02-13	NONE                     	REG AIR   	 sleep furiously about the p
230	8	5	3	1	908.00	0.07	0.06	R	F	1994-01-22	1994-01-03	1994-02-05	TAKE BACK RETURN         	RAIL      	blithely unusual dolphins. bold, ex
230	10	3	4	44	40040.44	0.08	0.06	R	F	1994-02-09	1994-01-18	1994-03-11	NONE                     	MAIL      	deposits integrate slyly sile
230	19	9	5	8	7352.08	0.09	0.06	R	F	1993-11-03	1994-01-20	1993-11-11	TAKE BACK RETURN         	TRUCK     	g the instructions. fluffil
230	34	10	6	8	7472.24	0.00	0.05	R	F	1993-11-21	1994-01-05	1993-12-19	TAKE BACK RETURN         	FOB       	nal ideas. silent, reg
231	159	10	1	16	16946.40	0.04	0.08	R	F	1994-11-20	1994-10-29	1994-12-17	TAKE BACK RETURN         	AIR       	e furiously ironic pinto beans.
231	84	5	2	46	45267.68	0.04	0.05	R	F	1994-12-13	1994-12-02	1994-12-14	DELIVER IN PERSON        	SHIP      	affix blithely. bold requests among the f
231	199	1	3	50	54959.50	0.09	0.01	A	F	1994-12-11	1994-12-14	1994-12-13	NONE                     	RAIL      	onic packages haggle fluffily a
231	57	8	4	31	29668.55	0.08	0.02	A	F	1994-11-05	1994-12-27	1994-11-30	TAKE BACK RETURN         	SHIP      	iously special decoys wake q
260	156	7	1	50	52807.50	0.07	0.08	N	O	1997-03-24	1997-02-09	1997-04-20	TAKE BACK RETURN         	REG AIR   	c deposits 
260	183	4	2	26	28162.68	0.02	0.07	N	O	1996-12-12	1997-02-06	1996-12-15	NONE                     	TRUCK     	ld theodolites boost fl
260	42	1	3	27	25435.08	0.05	0.08	N	O	1997-03-23	1997-02-15	1997-04-22	TAKE BACK RETURN         	RAIL      	ions according to the
260	6	1	4	29	26274.00	0.10	0.06	N	O	1997-03-15	1997-01-14	1997-04-13	NONE                     	MAIL      	fluffily even asymptotes. express wa
260	96	9	5	44	43827.96	0.01	0.05	N	O	1997-03-26	1997-02-03	1997-04-19	DELIVER IN PERSON        	MAIL      	above the blithely ironic instr
261	2	7	1	34	30668.00	0.05	0.08	R	F	1993-08-18	1993-09-24	1993-08-20	COLLECT COD              	REG AIR   	c packages. asymptotes da
261	66	7	2	20	19321.20	0.00	0.06	R	F	1993-10-21	1993-08-02	1993-11-04	DELIVER IN PERSON        	RAIL      	ites hinder 
261	174	3	3	28	30076.76	0.08	0.03	R	F	1993-07-24	1993-08-20	1993-08-05	COLLECT COD              	AIR       	ironic packages nag slyly. carefully fin
261	119	3	4	49	49936.39	0.04	0.05	R	F	1993-09-12	1993-08-31	1993-10-07	COLLECT COD              	SHIP      	ions. bold accounts 
261	61	6	5	49	47091.94	0.01	0.08	A	F	1993-09-29	1993-09-08	1993-10-01	COLLECT COD              	SHIP      	 pinto beans haggle slyly furiously pending
261	97	9	6	20	19941.80	0.06	0.06	A	F	1993-10-15	1993-09-05	1993-11-07	NONE                     	AIR       	ing to the special, ironic deposi
289	174	2	1	25	26854.25	0.07	0.05	N	O	1997-03-18	1997-05-05	1997-04-15	DELIVER IN PERSON        	FOB       	out the quickly bold theodol
289	112	2	2	6	6072.66	0.06	0.05	N	O	1997-02-18	1997-05-08	1997-03-19	DELIVER IN PERSON        	SHIP      	d packages use fluffily furiously
289	17	4	3	44	40348.44	0.10	0.08	N	O	1997-06-05	1997-04-20	1997-07-02	COLLECT COD              	MAIL      	ly ironic foxes. asymptotes 
289	40	6	4	48	45121.92	0.01	0.08	N	O	1997-03-14	1997-03-30	1997-03-24	DELIVER IN PERSON        	RAIL      	sits cajole. bold pinto beans x-ray fl
289	47	4	5	13	12311.52	0.10	0.03	N	O	1997-06-08	1997-04-06	1997-06-18	TAKE BACK RETURN         	REG AIR   	ts. quickly bold deposits alongside
3584	11	8	1	4	3644.04	0.04	0.08	N	O	1997-08-16	1997-10-31	1997-08-28	DELIVER IN PERSON        	TRUCK     	nal packag
291	123	6	1	21	21485.52	0.05	0.07	A	F	1994-05-26	1994-05-10	1994-06-23	COLLECT COD              	TRUCK     	y quickly regular theodolites. final t
291	138	9	2	19	19724.47	0.08	0.02	R	F	1994-06-14	1994-04-25	1994-06-19	NONE                     	REG AIR   	e. ruthlessly final accounts after the
291	61	8	3	30	28831.80	0.10	0.02	R	F	1994-03-22	1994-04-30	1994-03-24	DELIVER IN PERSON        	FOB       	 fluffily regular deposits. quickl
292	154	5	1	8	8433.20	0.10	0.03	R	F	1992-02-18	1992-03-30	1992-03-18	DELIVER IN PERSON        	RAIL      	sily bold deposits alongside of the ex
292	100	1	2	24	24002.40	0.08	0.04	R	F	1992-03-24	1992-03-06	1992-04-20	COLLECT COD              	TRUCK     	 bold, pending theodolites u
294	60	2	1	31	29761.86	0.00	0.01	R	F	1993-08-06	1993-08-19	1993-08-13	TAKE BACK RETURN         	AIR       	le fluffily along the quick
322	153	8	1	12	12637.80	0.08	0.07	A	F	1992-06-29	1992-05-30	1992-07-11	NONE                     	AIR       	ular theodolites promise qu
322	44	5	2	48	45313.92	0.02	0.07	A	F	1992-06-11	1992-06-16	1992-06-26	COLLECT COD              	RAIL      	dolites detect qu
322	13	3	3	20	18260.20	0.04	0.01	R	F	1992-04-26	1992-05-04	1992-05-22	DELIVER IN PERSON        	MAIL      	ckly toward 
322	184	5	4	10	10841.80	0.06	0.03	R	F	1992-04-12	1992-05-13	1992-04-14	DELIVER IN PERSON        	AIR       	 deposits grow slyly according to th
322	12	2	5	35	31920.35	0.07	0.06	A	F	1992-07-17	1992-05-03	1992-08-14	TAKE BACK RETURN         	RAIL      	egular accounts cajole carefully. even d
322	34	5	6	3	2802.09	0.08	0.05	A	F	1992-07-03	1992-05-10	1992-07-28	NONE                     	AIR       	ending, ironic deposits along the blith
322	38	4	7	5	4690.15	0.01	0.02	A	F	1992-04-15	1992-05-12	1992-04-26	COLLECT COD              	REG AIR   	 special grouches sleep quickly instructio
323	164	9	1	50	53208.00	0.05	0.04	A	F	1994-04-20	1994-04-25	1994-05-12	DELIVER IN PERSON        	REG AIR   	cial requests 
323	96	8	2	18	17929.62	0.06	0.07	R	F	1994-04-13	1994-06-02	1994-05-10	DELIVER IN PERSON        	TRUCK     	posits cajole furiously pinto beans. 
323	143	4	3	9	9388.26	0.07	0.04	A	F	1994-06-26	1994-06-10	1994-07-13	COLLECT COD              	TRUCK     	nic accounts. regular, regular pack
324	200	3	1	26	28605.20	0.07	0.01	R	F	1992-04-19	1992-05-28	1992-05-12	DELIVER IN PERSON        	RAIL      	ross the slyly regular s
325	159	1	1	34	36011.10	0.09	0.04	A	F	1993-10-28	1993-12-13	1993-11-17	TAKE BACK RETURN         	MAIL      	ly bold deposits. always iron
325	186	7	2	5	5430.90	0.07	0.08	A	F	1994-01-02	1994-01-05	1994-01-04	TAKE BACK RETURN         	MAIL      	 theodolites. 
325	19	3	3	35	32165.35	0.07	0.07	A	F	1993-12-06	1994-01-03	1993-12-26	DELIVER IN PERSON        	REG AIR   	packages wa
326	180	9	1	41	44287.38	0.06	0.03	N	O	1995-08-30	1995-07-09	1995-09-12	DELIVER IN PERSON        	TRUCK     	ily quickly bold ideas.
326	20	4	2	38	34960.76	0.02	0.08	N	O	1995-09-12	1995-08-23	1995-09-14	COLLECT COD              	RAIL      	es sleep slyly. carefully regular inst
326	184	5	3	25	27104.50	0.03	0.04	N	O	1995-08-03	1995-07-27	1995-08-16	NONE                     	AIR       	ily furiously unusual accounts. 
326	85	6	4	5	4925.40	0.03	0.08	N	O	1995-07-29	1995-07-13	1995-08-12	NONE                     	REG AIR   	deas sleep according to the sometimes spe
326	35	6	5	31	28985.93	0.04	0.08	N	O	1995-09-27	1995-07-06	1995-10-22	NONE                     	TRUCK     	cies sleep quick
326	157	9	6	41	43343.15	0.02	0.00	N	O	1995-07-05	1995-07-23	1995-07-20	TAKE BACK RETURN         	REG AIR   	to beans wake before the furiously re
326	43	10	7	47	44322.88	0.04	0.04	N	O	1995-09-16	1995-07-04	1995-10-04	NONE                     	REG AIR   	 special accounts sleep 
354	50	7	1	14	13300.70	0.08	0.04	N	O	1996-04-12	1996-06-03	1996-05-08	NONE                     	SHIP      	quickly regular grouches will eat. careful
354	194	8	2	24	26260.56	0.01	0.01	N	O	1996-05-08	1996-05-17	1996-06-07	DELIVER IN PERSON        	AIR       	y silent requests. regular, even accounts
354	59	10	3	50	47952.50	0.08	0.05	N	O	1996-03-21	1996-05-20	1996-04-04	COLLECT COD              	TRUCK     	to beans s
354	107	4	4	7	7049.70	0.06	0.01	N	O	1996-05-07	1996-04-18	1996-05-24	NONE                     	MAIL      	ously idly ironic accounts-- quickl
354	31	2	5	18	16758.54	0.04	0.08	N	O	1996-03-31	1996-05-13	1996-04-27	DELIVER IN PERSON        	RAIL      	 about the carefully unusual 
354	62	1	6	36	34634.16	0.03	0.02	N	O	1996-03-19	1996-05-29	1996-03-30	NONE                     	AIR       	onic requests thrash bold g
354	5	10	7	14	12670.00	0.01	0.07	N	O	1996-07-06	1996-06-08	1996-07-10	TAKE BACK RETURN         	MAIL      	t thinly above the ironic, 
355	114	1	1	31	31437.41	0.09	0.07	A	F	1994-07-13	1994-08-18	1994-07-18	DELIVER IN PERSON        	FOB       	y unusual, ironic
355	97	1	2	41	40880.69	0.05	0.00	A	F	1994-08-15	1994-07-19	1994-09-06	DELIVER IN PERSON        	TRUCK     	 deposits. carefully r
356	46	7	1	4	3784.16	0.10	0.01	A	F	1994-07-28	1994-08-01	1994-08-04	DELIVER IN PERSON        	REG AIR   	 the dependencies nod unusual, final ac
356	108	3	2	48	48388.80	0.02	0.03	R	F	1994-08-12	1994-07-31	1994-08-26	NONE                     	FOB       	unusual packages. furiously 
356	119	3	3	35	35668.85	0.08	0.07	R	F	1994-10-14	1994-07-31	1994-10-23	COLLECT COD              	TRUCK     	s. unusual, final
356	56	1	4	41	39198.05	0.07	0.05	A	F	1994-09-28	1994-09-20	1994-10-07	COLLECT COD              	SHIP      	 according to the express foxes will
356	125	8	5	37	37929.44	0.05	0.03	A	F	1994-07-15	1994-08-24	1994-08-09	DELIVER IN PERSON        	FOB       	ndencies are since the packag
358	191	3	1	41	44738.79	0.06	0.01	A	F	1993-11-18	1993-11-14	1993-11-28	NONE                     	TRUCK     	ely frets. furious deposits sleep 
358	190	1	2	32	34886.08	0.05	0.08	A	F	1993-10-18	1993-12-12	1993-10-31	NONE                     	TRUCK     	y final foxes sleep blithely sl
358	169	6	3	40	42766.40	0.09	0.01	A	F	1993-12-05	1993-11-04	1994-01-01	COLLECT COD              	MAIL      	ng the ironic theo
358	97	10	4	15	14956.35	0.08	0.08	A	F	1993-10-04	1993-12-17	1993-10-23	TAKE BACK RETURN         	MAIL      	out the blithely ironic deposits slee
358	29	2	5	18	16722.36	0.01	0.02	R	F	1993-10-07	1993-11-01	1993-10-26	COLLECT COD              	SHIP      	olphins haggle ironic accounts. f
358	162	3	6	32	33989.12	0.03	0.05	R	F	1993-12-21	1993-11-06	1994-01-17	DELIVER IN PERSON        	RAIL      	lyly express deposits 
358	83	4	7	45	44238.60	0.05	0.02	A	F	1993-12-08	1993-10-29	1993-12-30	NONE                     	REG AIR   	to beans. regular, unusual deposits sl
384	179	8	1	38	41008.46	0.07	0.01	R	F	1992-06-02	1992-04-18	1992-06-10	DELIVER IN PERSON        	TRUCK     	totes cajole blithely against the even
384	64	3	2	49	47238.94	0.09	0.07	A	F	1992-04-01	1992-04-25	1992-04-18	COLLECT COD              	AIR       	refully carefully ironic instructions. bl
384	182	3	3	11	11903.98	0.02	0.08	A	F	1992-04-02	1992-04-21	1992-04-15	COLLECT COD              	MAIL      	ash carefully
384	93	6	4	11	10923.99	0.00	0.06	R	F	1992-06-24	1992-05-29	1992-07-22	COLLECT COD              	TRUCK     	nic excuses are furiously above the blith
384	132	8	5	14	14449.82	0.08	0.06	R	F	1992-06-14	1992-05-29	1992-07-05	DELIVER IN PERSON        	TRUCK     	ckages are slyly after the slyly specia
387	137	8	1	1	1037.13	0.08	0.03	N	O	1997-05-06	1997-04-23	1997-05-10	NONE                     	SHIP      	 pinto beans wake furiously carefu
387	153	4	2	42	44232.30	0.07	0.05	N	O	1997-05-25	1997-02-25	1997-05-29	DELIVER IN PERSON        	RAIL      	lithely final theodolites.
387	97	10	3	40	39883.60	0.09	0.02	N	O	1997-03-08	1997-04-18	1997-03-31	COLLECT COD              	TRUCK     	 quickly ironic platelets are slyly. fluff
387	56	7	4	19	18164.95	0.08	0.00	N	O	1997-03-14	1997-04-21	1997-04-04	NONE                     	REG AIR   	gular dependencies
387	149	6	5	32	33572.48	0.08	0.06	N	O	1997-05-02	1997-04-11	1997-05-11	DELIVER IN PERSON        	TRUCK     	gle. silent, fur
388	33	9	1	42	39187.26	0.05	0.06	R	F	1993-02-21	1993-02-26	1993-03-15	COLLECT COD              	FOB       	accounts sleep furiously
388	128	9	2	46	47293.52	0.07	0.01	A	F	1993-03-22	1993-01-26	1993-03-24	COLLECT COD              	FOB       	to beans nag about the careful reque
388	65	2	3	40	38602.40	0.06	0.01	A	F	1992-12-24	1993-01-28	1993-01-19	TAKE BACK RETURN         	REG AIR   	quests against the carefully unusual epi
390	107	10	1	10	10071.00	0.02	0.05	N	O	1998-05-26	1998-07-06	1998-06-23	TAKE BACK RETURN         	SHIP      	 requests. final accounts x-ray beside the
390	124	7	2	17	17410.04	0.09	0.06	N	O	1998-06-07	1998-06-14	1998-07-07	COLLECT COD              	SHIP      	ending, pending pinto beans wake slyl
390	184	5	3	46	49872.28	0.07	0.04	N	O	1998-06-06	1998-05-20	1998-06-14	DELIVER IN PERSON        	SHIP      	cial excuses. bold, pending packages
390	142	3	4	42	43769.88	0.01	0.05	N	O	1998-06-06	1998-06-22	1998-07-05	COLLECT COD              	SHIP      	counts nag across the sly, sil
390	128	3	5	13	13365.56	0.02	0.06	N	O	1998-07-08	1998-05-10	1998-07-18	DELIVER IN PERSON        	SHIP      	sleep carefully idle packages. blithely 
390	125	4	6	11	11276.32	0.09	0.06	N	O	1998-05-05	1998-05-15	1998-06-01	DELIVER IN PERSON        	SHIP      	according to the foxes are furiously 
390	85	6	7	24	23641.92	0.05	0.02	N	O	1998-04-18	1998-05-19	1998-04-28	TAKE BACK RETURN         	AIR       	y. enticingly final depos
416	94	6	1	25	24852.25	0.00	0.05	A	F	1993-10-11	1993-11-26	1993-10-21	DELIVER IN PERSON        	TRUCK     	y final theodolites about
416	111	1	2	22	22244.42	0.10	0.00	R	F	1993-12-27	1993-12-17	1994-01-09	COLLECT COD              	RAIL      	rint blithely above the pending sentim
416	175	5	3	25	26879.25	0.07	0.01	R	F	1993-10-16	1993-12-03	1993-10-29	NONE                     	AIR       	ses boost after the bold requests.
417	40	1	1	39	36661.56	0.01	0.02	A	F	1994-05-31	1994-05-02	1994-06-06	NONE                     	SHIP      	y regular requests wake along 
417	70	7	2	18	17461.26	0.00	0.01	R	F	1994-03-29	1994-04-10	1994-04-26	TAKE BACK RETURN         	FOB       	- final requests sle
417	45	2	3	41	38746.64	0.10	0.01	R	F	1994-04-11	1994-03-08	1994-05-06	COLLECT COD              	RAIL      	tes. regular requests across the 
417	132	3	4	2	2064.26	0.01	0.03	R	F	1994-02-13	1994-04-19	1994-03-15	DELIVER IN PERSON        	SHIP      	uriously bol
420	101	6	1	5	5005.50	0.04	0.03	N	O	1995-11-04	1996-01-02	1995-11-30	NONE                     	REG AIR   	cajole blit
420	162	7	2	22	23367.52	0.05	0.04	N	O	1996-01-25	1995-12-16	1996-02-03	TAKE BACK RETURN         	AIR       	ly against the blithely re
420	48	1	3	45	42661.80	0.09	0.08	N	O	1996-01-14	1996-01-01	1996-01-26	COLLECT COD              	FOB       	 final accounts. furiously express forges
420	75	6	4	12	11700.84	0.08	0.08	N	O	1996-02-05	1996-01-03	1996-02-12	TAKE BACK RETURN         	REG AIR   	c instructions are 
420	73	2	5	37	36003.59	0.02	0.00	N	O	1995-11-16	1995-12-13	1995-11-19	DELIVER IN PERSON        	SHIP      	rbits. bold requests along the quickl
420	124	7	6	40	40964.80	0.01	0.05	N	O	1995-11-26	1995-12-26	1995-12-20	TAKE BACK RETURN         	FOB       	 after the special
420	16	7	7	39	35724.39	0.00	0.08	N	O	1995-12-09	1995-12-16	1995-12-31	DELIVER IN PERSON        	REG AIR   	s. ironic waters about the car
421	134	5	1	1	1034.13	0.02	0.07	R	F	1992-05-29	1992-04-27	1992-06-09	NONE                     	TRUCK     	oldly busy deposit
422	152	10	1	25	26303.75	0.10	0.07	N	O	1997-07-01	1997-08-17	1997-07-09	DELIVER IN PERSON        	SHIP      	carefully bold theodolit
422	171	1	2	10	10711.70	0.02	0.03	N	O	1997-06-15	1997-08-04	1997-07-08	TAKE BACK RETURN         	AIR       	he furiously ironic theodolite
422	176	4	3	46	49503.82	0.09	0.00	N	O	1997-06-21	1997-07-14	1997-06-27	DELIVER IN PERSON        	RAIL      	 ideas. qu
422	162	7	4	25	26554.00	0.10	0.04	N	O	1997-08-24	1997-07-09	1997-09-22	NONE                     	FOB       	ep along the furiousl
423	132	3	1	27	27867.51	0.06	0.03	N	O	1996-08-20	1996-08-01	1996-08-23	TAKE BACK RETURN         	SHIP      	ccounts. blithely regular pack
450	162	7	1	42	44610.72	0.03	0.00	N	F	1995-06-07	1995-05-29	1995-06-23	TAKE BACK RETURN         	SHIP      	y asymptotes. regular depen
450	107	8	2	5	5035.50	0.03	0.02	A	F	1995-04-02	1995-05-06	1995-04-13	TAKE BACK RETURN         	TRUCK     	the pinto bea
450	143	6	3	32	33380.48	0.06	0.03	N	O	1995-07-02	1995-04-25	1995-07-30	TAKE BACK RETURN         	SHIP      	 accounts nod fluffily even, pending
450	57	9	4	40	38282.00	0.05	0.03	R	F	1995-03-20	1995-05-25	1995-04-14	NONE                     	RAIL      	ve. asymptote
450	79	10	5	2	1958.14	0.09	0.00	A	F	1995-03-11	1995-05-21	1995-03-16	COLLECT COD              	AIR       	y even pinto beans; qui
450	153	1	6	33	34753.95	0.08	0.05	R	F	1995-05-18	1995-05-22	1995-05-23	TAKE BACK RETURN         	REG AIR   	ily carefully final depo
453	198	1	1	45	49418.55	0.01	0.00	N	O	1997-06-30	1997-08-20	1997-07-19	COLLECT COD              	REG AIR   	ifts wake carefully.
453	176	4	2	38	40894.46	0.08	0.04	N	O	1997-06-30	1997-07-08	1997-07-16	DELIVER IN PERSON        	REG AIR   	 furiously f
453	14	1	3	38	34732.38	0.10	0.01	N	O	1997-08-10	1997-07-24	1997-09-07	NONE                     	SHIP      	sts cajole. furiously un
453	96	7	4	45	44824.05	0.10	0.01	N	O	1997-09-18	1997-06-29	1997-10-14	TAKE BACK RETURN         	AIR       	ironic foxes. slyly pending depos
453	26	1	5	32	29632.64	0.04	0.01	N	O	1997-07-15	1997-06-27	1997-07-18	NONE                     	REG AIR   	s. fluffily bold packages cajole. unu
453	95	7	6	28	27862.52	0.07	0.07	N	O	1997-08-16	1997-08-12	1997-08-27	NONE                     	MAIL      	final dependencies. slyly special pl
454	118	8	1	24	24434.64	0.06	0.01	N	O	1996-04-26	1996-03-23	1996-05-20	NONE                     	TRUCK     	le. deposits after the ideas nag unusual pa
455	157	9	1	42	44400.30	0.10	0.02	N	O	1997-01-26	1997-01-10	1997-02-22	DELIVER IN PERSON        	REG AIR   	around the quickly blit
455	28	9	2	44	40832.88	0.05	0.08	N	O	1997-01-17	1997-02-22	1997-02-12	TAKE BACK RETURN         	TRUCK     	 accounts sleep slyly ironic asymptote
455	49	2	3	45	42706.80	0.04	0.06	N	O	1996-12-20	1997-01-31	1997-01-07	TAKE BACK RETURN         	SHIP      	thrash ironically regular packages. qui
455	171	9	4	11	11782.87	0.01	0.02	N	O	1997-03-15	1997-02-14	1997-03-26	DELIVER IN PERSON        	MAIL      	g deposits against the slyly idle foxes u
480	53	4	1	22	20967.10	0.04	0.02	A	F	1993-06-16	1993-07-28	1993-07-09	NONE                     	MAIL      	into beans cajole furiously. accounts s
484	31	2	1	49	45620.47	0.10	0.02	N	O	1997-03-06	1997-02-28	1997-03-23	COLLECT COD              	TRUCK     	ven accounts
484	32	8	2	45	41941.35	0.06	0.07	N	O	1997-04-09	1997-03-20	1997-04-19	DELIVER IN PERSON        	TRUCK     	usly final excuses boost slyly blithe
484	184	5	3	50	54209.00	0.06	0.05	N	O	1997-01-24	1997-03-27	1997-02-22	DELIVER IN PERSON        	MAIL      	uctions wake. final, silent requests haggle
484	165	6	4	22	23433.52	0.07	0.03	N	O	1997-04-29	1997-03-26	1997-05-17	TAKE BACK RETURN         	SHIP      	es are pending instructions. furiously unu
484	77	6	5	48	46899.36	0.00	0.05	N	O	1997-03-05	1997-02-08	1997-03-22	TAKE BACK RETURN         	MAIL      	l, bold packages? even mult
484	97	9	6	10	9970.90	0.01	0.08	N	O	1997-04-06	1997-02-14	1997-04-16	COLLECT COD              	FOB       	x fluffily carefully regular
485	150	1	1	50	52507.50	0.01	0.00	N	O	1997-03-28	1997-05-26	1997-04-18	TAKE BACK RETURN         	MAIL      	iously quick excuses. carefully final f
485	28	7	2	40	37120.80	0.08	0.01	N	O	1997-04-29	1997-05-08	1997-04-30	TAKE BACK RETURN         	TRUCK     	al escapades
485	137	3	3	22	22816.86	0.00	0.05	N	O	1997-04-06	1997-04-27	1997-05-01	DELIVER IN PERSON        	TRUCK     	refully final notornis haggle according 
486	76	7	1	36	35138.52	0.00	0.01	N	O	1996-06-25	1996-05-06	1996-07-07	COLLECT COD              	AIR       	deposits around the quickly regular packa
486	68	9	2	40	38722.40	0.03	0.08	N	O	1996-05-21	1996-06-06	1996-06-07	COLLECT COD              	SHIP      	ts nag quickly among the slyl
486	136	2	3	26	26939.38	0.04	0.03	N	O	1996-03-16	1996-05-25	1996-03-31	NONE                     	RAIL      	forges along the 
486	72	1	4	38	36938.66	0.08	0.05	N	O	1996-05-07	1996-04-26	1996-05-26	TAKE BACK RETURN         	TRUCK     	 blithely final pinto 
486	29	2	5	3	2787.06	0.07	0.05	N	O	1996-07-07	1996-04-20	1996-07-23	DELIVER IN PERSON        	RAIL      	ccounts ha
486	47	4	6	46	43563.84	0.00	0.03	N	O	1996-04-18	1996-05-02	1996-04-20	COLLECT COD              	AIR       	theodolites eat carefully furious
512	189	10	1	19	20694.42	0.08	0.05	N	O	1995-07-12	1995-07-11	1995-08-04	COLLECT COD              	MAIL      	 sleep. requests alongside of the fluff
512	23	2	2	37	34151.74	0.01	0.04	N	O	1995-06-20	1995-07-05	1995-07-16	NONE                     	RAIL      	nic depths cajole? blithely b
512	180	1	3	40	43207.20	0.05	0.02	N	O	1995-07-06	1995-07-08	1995-07-08	COLLECT COD              	TRUCK     	quests are da
512	83	4	4	10	9830.80	0.09	0.02	N	O	1995-09-16	1995-07-29	1995-10-07	NONE                     	AIR       	xes. pinto beans cajole carefully; 
512	65	6	5	6	5790.36	0.03	0.05	R	F	1995-06-10	1995-06-21	1995-06-16	DELIVER IN PERSON        	FOB       	en ideas haggle 
512	33	9	6	12	11196.36	0.04	0.00	R	F	1995-05-21	1995-08-03	1995-06-09	COLLECT COD              	FOB       	old furiously express deposits. specia
512	51	9	7	2	1902.10	0.09	0.08	N	O	1995-06-19	1995-08-13	1995-06-24	NONE                     	TRUCK     	e slyly silent accounts serve with
514	79	9	1	21	20560.47	0.06	0.02	N	O	1996-06-09	1996-05-15	1996-07-07	DELIVER IN PERSON        	RAIL      	s sleep quickly blithely
514	118	2	2	34	34615.74	0.08	0.02	N	O	1996-04-14	1996-06-03	1996-04-23	COLLECT COD              	REG AIR   	ily even patterns. bold, silent instruc
514	13	7	3	6	5478.06	0.06	0.01	N	O	1996-05-30	1996-06-04	1996-06-28	COLLECT COD              	SHIP      	as haggle blithely; quickly s
514	116	7	4	43	43692.73	0.00	0.08	N	O	1996-06-07	1996-05-14	1996-07-01	TAKE BACK RETURN         	FOB       	thely regular 
515	105	8	1	10	10051.00	0.03	0.02	A	F	1993-10-04	1993-11-03	1993-10-08	NONE                     	FOB       	ar deposits th
515	148	1	2	38	39829.32	0.10	0.07	A	F	1993-09-19	1993-11-12	1993-10-03	DELIVER IN PERSON        	SHIP      	ays. furiously express requests haggle furi
515	183	4	3	11	11914.98	0.00	0.02	R	F	1993-09-04	1993-10-02	1993-09-05	DELIVER IN PERSON        	FOB       	ly pending accounts haggle blithel
515	109	10	4	34	34309.40	0.09	0.03	R	F	1993-10-03	1993-10-26	1993-10-15	DELIVER IN PERSON        	REG AIR   	ic dependencie
515	131	7	5	32	32996.16	0.01	0.07	R	F	1993-10-10	1993-10-08	1993-11-02	TAKE BACK RETURN         	FOB       	r sauternes boost. final theodolites wake a
515	109	4	6	25	25227.50	0.04	0.08	R	F	1993-11-14	1993-11-07	1993-12-03	DELIVER IN PERSON        	MAIL      	e packages engag
517	45	6	1	28	26461.12	0.03	0.02	N	O	1997-04-30	1997-05-18	1997-05-17	COLLECT COD              	MAIL      	 requests. special, fi
517	156	4	2	15	15842.25	0.02	0.00	N	O	1997-04-09	1997-06-26	1997-05-01	NONE                     	TRUCK     	 slyly. express requests ar
517	41	8	3	9	8469.36	0.04	0.00	N	O	1997-05-03	1997-06-16	1997-05-24	COLLECT COD              	SHIP      	 slyly stealthily express instructions. 
517	133	4	4	11	11364.43	0.06	0.02	N	O	1997-06-20	1997-06-01	1997-06-27	NONE                     	REG AIR   	ly throughout the fu
517	24	3	5	23	21252.46	0.00	0.01	N	O	1997-04-19	1997-05-07	1997-05-12	COLLECT COD              	RAIL      	 kindle. furiously bold requests mus
519	159	4	1	1	1059.15	0.07	0.07	N	O	1997-12-01	1998-01-26	1997-12-23	COLLECT COD              	REG AIR   	bold requests believe furiou
519	3	4	2	38	34314.00	0.05	0.08	N	O	1998-02-19	1997-12-15	1998-03-19	DELIVER IN PERSON        	FOB       	gular excuses detect quickly furiously 
519	106	1	3	19	19115.90	0.00	0.02	N	O	1998-01-09	1998-01-03	1998-02-06	COLLECT COD              	AIR       	asymptotes. p
519	47	6	4	27	25570.08	0.08	0.06	N	O	1997-11-20	1997-12-06	1997-12-16	DELIVER IN PERSON        	REG AIR   	le. even, final dependencies
519	10	5	5	13	11830.13	0.06	0.08	N	O	1998-02-06	1997-12-02	1998-03-03	TAKE BACK RETURN         	TRUCK     	c accounts wake along the ironic so
519	151	6	6	3	3153.45	0.04	0.00	N	O	1998-02-01	1998-01-25	1998-02-27	TAKE BACK RETURN         	FOB       	erve blithely blithely ironic asymp
544	139	10	1	47	48839.11	0.08	0.06	R	F	1993-03-14	1993-03-27	1993-03-27	COLLECT COD              	SHIP      	ecial pains. deposits grow foxes. 
545	170	1	1	4	4280.68	0.02	0.00	N	O	1996-02-23	1995-12-16	1996-03-21	DELIVER IN PERSON        	FOB       	, ironic grouches cajole over
545	171	10	2	18	19281.06	0.00	0.00	N	O	1996-02-21	1996-01-17	1996-02-26	NONE                     	RAIL      	al, final packages affix. even a
549	196	9	1	18	19731.42	0.07	0.04	R	F	1992-10-19	1992-08-12	1992-11-13	COLLECT COD              	REG AIR   	furiously according to the ironic, regular 
549	189	10	2	38	41388.84	0.07	0.05	A	F	1992-08-17	1992-08-28	1992-09-05	COLLECT COD              	RAIL      	the regular, furious excuses. carefu
549	66	7	3	36	34778.16	0.08	0.04	R	F	1992-09-11	1992-10-11	1992-09-12	DELIVER IN PERSON        	AIR       	ts against the ironic, even theodolites eng
549	21	4	4	18	16578.36	0.09	0.01	A	F	1992-07-31	1992-09-11	1992-08-08	NONE                     	RAIL      	ely regular accounts above the 
549	24	7	5	38	35112.76	0.06	0.02	R	F	1992-08-23	1992-08-12	1992-08-25	COLLECT COD              	REG AIR   	eposits. carefully regular depos
550	191	3	1	31	33826.89	0.04	0.02	N	O	1995-10-24	1995-09-27	1995-11-04	COLLECT COD              	AIR       	thely silent packages. unusual
551	24	9	1	8	7392.16	0.08	0.02	N	O	1995-07-29	1995-07-18	1995-08-02	NONE                     	REG AIR   	 wake quickly slyly pending platel
551	159	4	2	20	21183.00	0.00	0.07	N	O	1995-09-18	1995-08-25	1995-10-11	COLLECT COD              	TRUCK     	r ideas. final, even ideas hinder alongside
551	162	9	3	16	16994.56	0.07	0.06	N	O	1995-07-29	1995-08-19	1995-08-10	COLLECT COD              	MAIL      	y along the carefully ex
577	26	5	1	25	23150.50	0.06	0.01	A	F	1995-04-09	1995-02-20	1995-05-09	TAKE BACK RETURN         	AIR       	ve slyly of the frets. careful
577	64	1	2	14	13496.84	0.08	0.03	R	F	1995-03-19	1995-02-25	1995-04-09	DELIVER IN PERSON        	RAIL      	l accounts wake deposits. ironic packa
578	156	7	1	40	42246.00	0.02	0.08	N	O	1997-02-10	1997-03-18	1997-02-11	NONE                     	SHIP      	usly even platel
578	188	9	2	23	25028.14	0.05	0.08	N	O	1997-03-06	1997-03-03	1997-03-20	TAKE BACK RETURN         	FOB       	nstructions. ironic deposits
579	151	6	1	9	9460.35	0.00	0.05	N	O	1998-06-20	1998-04-28	1998-07-19	DELIVER IN PERSON        	RAIL      	e ironic, express deposits are furiously
579	33	4	2	39	36388.17	0.02	0.01	N	O	1998-06-21	1998-06-03	1998-06-26	COLLECT COD              	REG AIR   	ncies. furiously final r
579	60	5	3	6	5760.36	0.03	0.00	N	O	1998-04-24	1998-05-03	1998-05-08	TAKE BACK RETURN         	TRUCK     	ickly final requests-- bold accou
579	7	10	4	41	37187.00	0.04	0.05	N	O	1998-05-28	1998-05-01	1998-06-04	COLLECT COD              	REG AIR   	bold, express requests sublate slyly. blith
579	13	7	5	28	25564.28	0.00	0.03	N	O	1998-07-10	1998-05-24	1998-07-19	NONE                     	RAIL      	ic ideas until th
579	167	6	6	5	5335.80	0.05	0.08	N	O	1998-05-02	1998-04-25	1998-05-05	COLLECT COD              	REG AIR   	refully silent ideas cajole furious
582	57	9	1	7	6699.35	0.07	0.00	N	O	1997-11-16	1997-11-29	1997-12-10	TAKE BACK RETURN         	FOB       	ithely unusual t
582	51	2	2	49	46601.45	0.05	0.03	N	O	1997-12-17	1998-01-12	1997-12-31	COLLECT COD              	REG AIR   	nts according to the furiously regular pin
582	141	4	3	42	43727.88	0.07	0.00	N	O	1997-11-15	1997-12-21	1997-12-03	COLLECT COD              	SHIP      	iously beside the silent de
582	168	9	4	36	38453.76	0.06	0.01	N	O	1997-12-09	1997-11-27	1997-12-26	TAKE BACK RETURN         	SHIP      	lar requests. quickly 
583	145	6	1	1	1045.14	0.07	0.07	N	O	1997-06-17	1997-04-29	1997-06-28	NONE                     	TRUCK     	 regular, regular ideas. even, bra
583	120	4	2	47	47945.64	0.10	0.06	N	O	1997-07-14	1997-05-12	1997-08-11	DELIVER IN PERSON        	AIR       	nts are fluffily. furiously even re
583	130	5	3	34	35024.42	0.01	0.02	N	O	1997-05-11	1997-04-24	1997-06-03	DELIVER IN PERSON        	MAIL      	express req
583	142	1	4	33	34390.62	0.10	0.01	N	O	1997-05-28	1997-04-25	1997-06-24	NONE                     	AIR       	kages cajole slyly across the
583	189	10	5	13	14159.34	0.04	0.06	N	O	1997-06-23	1997-05-29	1997-07-08	COLLECT COD              	TRUCK     	y sly theodolites. ironi
610	111	8	1	49	49544.39	0.10	0.07	N	O	1995-08-29	1995-10-26	1995-09-12	TAKE BACK RETURN         	SHIP      	ular instruc
610	68	3	2	11	10648.66	0.07	0.08	N	O	1995-10-31	1995-10-25	1995-11-18	NONE                     	MAIL      	blithely final 
610	118	9	3	26	26470.86	0.09	0.04	N	O	1995-11-22	1995-09-09	1995-12-04	TAKE BACK RETURN         	AIR       	cross the furiously even theodolites sl
610	186	7	4	17	18465.06	0.03	0.03	N	O	1995-11-01	1995-10-30	1995-11-04	COLLECT COD              	FOB       	p quickly instead of the slyly pending foxe
610	146	7	5	39	40799.46	0.08	0.05	N	O	1995-10-30	1995-10-21	1995-11-11	TAKE BACK RETURN         	REG AIR   	counts. ironic warhorses are 
610	95	7	6	5	4975.45	0.00	0.07	N	O	1995-08-11	1995-10-22	1995-08-26	TAKE BACK RETURN         	FOB       	n pinto beans. iro
610	190	1	7	27	29435.13	0.06	0.03	N	O	1995-09-02	1995-09-19	1995-09-15	NONE                     	REG AIR   	 ironic pinto beans haggle. blithe
611	17	7	1	39	35763.39	0.05	0.06	R	F	1993-05-06	1993-04-09	1993-05-22	TAKE BACK RETURN         	SHIP      	nto beans 
611	81	2	2	1	981.08	0.08	0.07	R	F	1993-05-17	1993-02-26	1993-06-15	DELIVER IN PERSON        	MAIL      	ts. pending platelets aff
611	120	4	3	39	39784.68	0.09	0.02	A	F	1993-03-10	1993-03-10	1993-03-17	TAKE BACK RETURN         	TRUCK     	the evenly bold requests. furious
613	91	5	1	17	16848.53	0.06	0.06	N	O	1995-09-23	1995-08-04	1995-10-15	NONE                     	SHIP      	ar dependencie
613	79	7	2	6	5874.42	0.05	0.05	N	O	1995-08-05	1995-08-09	1995-08-08	TAKE BACK RETURN         	MAIL      	y ironic deposits eat 
613	186	7	3	3	3258.54	0.03	0.01	N	O	1995-09-27	1995-09-11	1995-10-05	NONE                     	TRUCK     	ccounts cajole. 
613	159	10	4	7	7414.05	0.02	0.04	N	O	1995-09-07	1995-08-02	1995-09-16	DELIVER IN PERSON        	MAIL      	ously blithely final pinto beans. regula
614	195	8	1	21	22998.99	0.00	0.03	R	F	1993-03-29	1993-01-06	1993-04-16	TAKE BACK RETURN         	TRUCK     	arefully. slyly express packag
614	187	8	2	48	52184.64	0.07	0.07	A	F	1993-03-09	1993-01-19	1993-03-19	DELIVER IN PERSON        	SHIP      	riously special excuses haggle along the
614	167	2	3	43	45887.88	0.05	0.00	A	F	1993-03-07	1993-02-22	1993-03-18	DELIVER IN PERSON        	SHIP      	 express accounts wake. slyly ironic ins
614	147	6	4	14	14659.96	0.04	0.06	A	F	1992-12-03	1993-02-14	1992-12-27	DELIVER IN PERSON        	SHIP      	ular packages haggle about the pack
614	196	8	5	30	32885.70	0.08	0.07	R	F	1993-01-16	1993-02-08	1993-02-12	TAKE BACK RETURN         	FOB       	tructions are f
614	137	8	6	48	49782.24	0.04	0.08	A	F	1992-12-14	1993-01-22	1993-01-11	NONE                     	TRUCK     	 regular platelets cajole quickly eve
615	105	6	1	36	36183.60	0.10	0.01	A	F	1992-06-01	1992-07-14	1992-06-27	NONE                     	FOB       	 packages. carefully final pinto bea
642	54	5	1	26	24805.30	0.10	0.03	A	F	1994-04-16	1994-02-01	1994-04-27	COLLECT COD              	REG AIR   	quests according to the unu
645	160	8	1	33	34985.28	0.01	0.02	A	F	1994-12-09	1995-02-21	1995-01-03	NONE                     	TRUCK     	heodolites b
645	170	1	2	47	50297.99	0.07	0.05	R	F	1995-02-16	1995-02-15	1995-02-25	COLLECT COD              	TRUCK     	hely regular instructions alon
645	70	7	3	46	44623.22	0.10	0.01	A	F	1995-01-04	1995-02-21	1995-01-21	COLLECT COD              	REG AIR   	 regular dependencies across the speci
645	96	9	4	49	48808.41	0.05	0.03	R	F	1995-01-24	1995-01-06	1995-02-17	NONE                     	TRUCK     	y. slyly iron
645	5	8	5	43	38915.00	0.06	0.02	A	F	1995-02-12	1995-02-27	1995-03-06	TAKE BACK RETURN         	REG AIR   	 furiously accounts. slyly
645	34	5	6	18	16812.54	0.10	0.08	A	F	1995-03-02	1995-02-08	1995-03-03	COLLECT COD              	RAIL      	ep. slyly even 
645	28	9	7	9	8352.18	0.03	0.03	A	F	1994-12-25	1995-01-04	1995-01-15	COLLECT COD              	REG AIR   	special deposits. regular, final th
646	109	6	1	31	31282.10	0.00	0.05	R	F	1994-12-17	1995-02-16	1995-01-04	COLLECT COD              	MAIL      	ag furiousl
646	127	8	2	1	1027.12	0.07	0.01	A	F	1994-12-05	1995-01-07	1994-12-31	TAKE BACK RETURN         	MAIL      	t blithely regular deposits. quic
646	30	9	3	24	22320.72	0.06	0.02	A	F	1995-02-20	1994-12-30	1995-03-16	TAKE BACK RETURN         	TRUCK     	regular accounts haggle dog
646	99	2	4	34	33969.06	0.01	0.00	R	F	1994-12-28	1994-12-27	1994-12-31	COLLECT COD              	SHIP      	slow accounts. fluffily idle instructions
646	90	1	5	17	16831.53	0.04	0.01	A	F	1994-12-31	1994-12-26	1995-01-01	DELIVER IN PERSON        	REG AIR   	inal packages haggle carefully 
646	115	2	6	40	40604.40	0.10	0.01	R	F	1995-01-01	1995-01-13	1995-01-11	COLLECT COD              	TRUCK     	ronic packages sleep across th
673	71	10	1	22	21363.54	0.03	0.02	R	F	1994-03-15	1994-04-27	1994-03-29	TAKE BACK RETURN         	TRUCK     	 the regular, even requests. carefully fin
675	157	9	1	1	1057.15	0.04	0.08	N	O	1997-11-27	1997-09-30	1997-12-12	DELIVER IN PERSON        	REG AIR   	ide of the slyly regular packages. unus
675	137	3	2	35	36299.55	0.08	0.07	N	O	1997-08-19	1997-10-16	1997-09-17	DELIVER IN PERSON        	REG AIR   	s. furiously expre
675	176	6	3	34	36589.78	0.10	0.04	N	O	1997-11-17	1997-10-07	1997-11-27	NONE                     	FOB       	y final accounts unwind around the 
675	100	4	4	15	15001.50	0.09	0.05	N	O	1997-10-18	1997-09-28	1997-11-13	COLLECT COD              	TRUCK     	posits after the furio
675	5	8	5	46	41630.00	0.09	0.05	N	O	1997-09-18	1997-10-14	1997-10-01	DELIVER IN PERSON        	AIR       	 deposits along the express foxes 
676	51	3	1	9	8559.45	0.09	0.02	N	O	1997-04-03	1997-02-02	1997-04-08	COLLECT COD              	REG AIR   	aintain sl
676	78	6	2	20	19561.40	0.07	0.07	N	O	1997-02-02	1997-02-01	1997-02-11	NONE                     	REG AIR   	riously around the blithely 
676	163	4	3	35	37210.60	0.05	0.01	N	O	1996-12-30	1997-01-13	1997-01-19	DELIVER IN PERSON        	RAIL      	into beans. blithe
676	73	1	4	24	23353.68	0.01	0.06	N	O	1997-02-05	1997-01-16	1997-03-07	TAKE BACK RETURN         	TRUCK     	ress, regular dep
676	166	1	5	31	33050.96	0.01	0.06	N	O	1997-02-06	1997-02-28	1997-03-08	COLLECT COD              	TRUCK     	ial deposits cajo
676	76	7	6	33	32210.31	0.09	0.05	N	O	1997-03-02	1997-02-22	1997-03-19	TAKE BACK RETURN         	TRUCK     	as wake slyly furiously close pinto b
676	143	6	7	11	11474.54	0.07	0.02	N	O	1997-03-09	1997-03-06	1997-03-31	TAKE BACK RETURN         	MAIL      	he final acco
678	146	7	1	20	20922.80	0.05	0.08	R	F	1993-06-21	1993-04-07	1993-07-10	TAKE BACK RETURN         	MAIL      	furiously express excuses. foxes eat fu
678	37	3	2	22	20614.66	0.01	0.02	A	F	1993-05-10	1993-04-29	1993-06-08	NONE                     	REG AIR   	de of the carefully even requests. bl
678	143	10	3	16	16690.24	0.06	0.02	R	F	1993-03-20	1993-04-13	1993-04-16	DELIVER IN PERSON        	REG AIR   	equests cajole around the carefully regular
678	199	10	4	48	52761.12	0.08	0.08	R	F	1993-02-28	1993-04-04	1993-03-24	NONE                     	REG AIR   	ithely. slyly express foxes
678	98	9	5	16	15969.44	0.06	0.04	R	F	1993-03-09	1993-04-18	1993-04-07	NONE                     	AIR       	 about the 
678	43	4	6	11	10373.44	0.09	0.00	R	F	1993-04-28	1993-05-16	1993-05-11	COLLECT COD              	TRUCK     	ess deposits dazzle f
679	192	3	1	9	9829.71	0.09	0.00	N	O	1995-12-20	1996-01-27	1996-01-07	COLLECT COD              	REG AIR   	leep slyly. entici
704	190	1	1	40	43607.60	0.05	0.05	N	O	1997-01-30	1997-01-10	1997-02-20	COLLECT COD              	AIR       	ggle quickly. r
704	4	5	2	14	12656.00	0.07	0.08	N	O	1997-02-02	1996-12-26	1997-02-19	DELIVER IN PERSON        	REG AIR   	ve the quickly final forges. furiously p
706	197	9	1	23	25235.37	0.05	0.00	N	O	1995-12-06	1995-12-02	1995-12-16	COLLECT COD              	SHIP      	ckey players. requests above the
709	87	8	1	7	6909.56	0.00	0.00	N	O	1998-06-14	1998-06-08	1998-06-18	TAKE BACK RETURN         	RAIL      	 special orbits cajole 
709	198	10	2	15	16472.85	0.08	0.00	N	O	1998-07-10	1998-06-26	1998-08-09	NONE                     	RAIL      	ily regular deposits. sauternes was accor
709	169	8	3	10	10691.60	0.01	0.02	N	O	1998-06-04	1998-06-30	1998-06-11	NONE                     	REG AIR   	ts cajole boldly 
709	108	9	4	40	40324.00	0.10	0.08	N	O	1998-08-12	1998-06-20	1998-08-20	DELIVER IN PERSON        	RAIL      	ggle fluffily carefully ironic
711	146	3	1	2	2092.28	0.10	0.04	R	F	1993-12-01	1993-12-09	1993-12-16	DELIVER IN PERSON        	REG AIR   	ely across t
711	103	8	2	27	27083.70	0.00	0.08	A	F	1993-10-02	1993-10-26	1993-10-08	DELIVER IN PERSON        	MAIL      	slyly. ironic asy
711	128	7	3	46	47293.52	0.10	0.00	R	F	1993-12-26	1993-11-19	1994-01-21	TAKE BACK RETURN         	MAIL      	deposits. permanen
711	128	9	4	20	20562.40	0.09	0.00	A	F	1994-01-17	1993-11-10	1994-01-31	DELIVER IN PERSON        	TRUCK     	kly regular acco
736	158	9	1	46	48674.90	0.05	0.01	N	O	1998-07-16	1998-09-01	1998-08-09	NONE                     	AIR       	uctions cajole
736	80	1	2	23	22541.84	0.02	0.05	N	O	1998-10-08	1998-08-27	1998-10-19	TAKE BACK RETURN         	AIR       	k accounts are carefully
736	57	9	3	13	12441.65	0.00	0.03	N	O	1998-08-16	1998-07-26	1998-08-19	DELIVER IN PERSON        	FOB       	st furiously among the 
736	98	2	4	14	13973.26	0.06	0.04	N	O	1998-10-04	1998-08-14	1998-10-16	COLLECT COD              	REG AIR   	nstructions.
736	169	6	5	32	34213.12	0.04	0.03	N	O	1998-07-30	1998-08-22	1998-08-12	DELIVER IN PERSON        	RAIL      	iously final accoun
738	198	1	1	34	37338.46	0.00	0.06	R	F	1993-06-09	1993-04-15	1993-07-09	TAKE BACK RETURN         	TRUCK     	s against the ironic exc
738	188	9	2	4	4352.72	0.00	0.03	A	F	1993-06-20	1993-04-08	1993-07-09	NONE                     	AIR       	ar packages. fluffily bo
738	170	1	3	23	24613.91	0.04	0.08	A	F	1993-03-17	1993-04-02	1993-04-05	TAKE BACK RETURN         	SHIP      	nic, final excuses promise quickly regula
738	141	10	4	12	12493.68	0.04	0.08	A	F	1993-06-16	1993-05-05	1993-06-22	NONE                     	SHIP      	ove the slyly regular p
738	175	4	5	30	32255.10	0.02	0.00	A	F	1993-06-12	1993-05-29	1993-06-25	NONE                     	AIR       	ecial instructions haggle blithely regula
741	187	8	1	25	27179.50	0.03	0.06	N	O	1998-07-15	1998-08-27	1998-08-12	DELIVER IN PERSON        	MAIL      	accounts. blithely bold pa
741	91	4	2	22	21803.98	0.09	0.01	N	O	1998-09-07	1998-09-28	1998-09-12	COLLECT COD              	AIR       	ven deposits about the regular, ironi
742	102	3	1	46	46096.60	0.04	0.08	A	F	1995-03-12	1995-03-20	1995-03-16	TAKE BACK RETURN         	SHIP      	e slyly bold deposits cajole according to
742	96	8	2	15	14941.35	0.08	0.05	A	F	1995-02-26	1995-03-20	1995-03-03	NONE                     	SHIP      	blithely unusual pinto
742	102	5	3	24	24050.40	0.08	0.08	A	F	1995-02-12	1995-03-12	1995-02-14	DELIVER IN PERSON        	SHIP      	affix slyly. furiously i
742	192	4	4	16	17475.04	0.01	0.05	A	F	1995-01-15	1995-02-25	1995-01-24	COLLECT COD              	AIR       	eodolites haggle carefully regul
742	101	4	5	48	48052.80	0.09	0.08	R	F	1995-03-24	1995-01-23	1995-04-08	TAKE BACK RETURN         	TRUCK     	 platelets 
742	192	6	6	49	53517.31	0.02	0.07	A	F	1995-01-13	1995-02-13	1995-01-26	TAKE BACK RETURN         	RAIL      	 carefully bold foxes sle
768	196	7	1	39	42751.41	0.06	0.08	N	O	1996-09-25	1996-10-27	1996-10-20	NONE                     	SHIP      	out the ironic
768	18	9	2	2	1836.02	0.00	0.04	N	O	1996-11-13	1996-10-03	1996-11-25	DELIVER IN PERSON        	SHIP      	ular courts. slyly dogged accou
768	6	1	3	30	27180.00	0.06	0.05	N	O	1996-09-22	1996-11-03	1996-10-13	NONE                     	MAIL      	 furiously fluffy pinto beans haggle along
768	25	8	4	37	34225.74	0.10	0.00	N	O	1996-10-02	1996-09-23	1996-10-14	TAKE BACK RETURN         	REG AIR   	ending requests across the quickly
768	47	10	5	47	44510.88	0.06	0.05	N	O	1996-11-28	1996-10-30	1996-12-12	NONE                     	TRUCK     	foxes. slyly ironic deposits a
768	112	9	6	43	43520.73	0.10	0.06	N	O	1996-09-22	1996-11-03	1996-10-22	TAKE BACK RETURN         	AIR       	sual ideas wake quickly
768	49	10	7	33	31318.32	0.01	0.04	N	O	1996-09-06	1996-09-29	1996-10-01	COLLECT COD              	RAIL      	sly ironic instructions. excuses can hagg
769	176	6	1	36	38742.12	0.02	0.02	A	F	1993-10-01	1993-08-07	1993-10-15	NONE                     	AIR       	es. furiously iro
769	160	8	2	4	4240.64	0.01	0.04	R	F	1993-06-25	1993-08-12	1993-07-15	DELIVER IN PERSON        	FOB       	 ideas. even
771	7	4	1	12	10884.00	0.10	0.08	N	O	1995-07-18	1995-08-02	1995-08-07	COLLECT COD              	TRUCK     	carefully. pending in
771	161	10	2	38	40324.08	0.03	0.08	N	O	1995-07-22	1995-09-10	1995-07-29	TAKE BACK RETURN         	REG AIR   	 quickly final requests are final packages.
771	7	8	3	14	12698.00	0.02	0.05	N	O	1995-07-31	1995-08-13	1995-08-07	DELIVER IN PERSON        	AIR       	r, final packages are slyly iro
771	42	3	4	7	6594.28	0.06	0.02	N	O	1995-06-18	1995-08-31	1995-06-20	NONE                     	REG AIR   	theodolites after the fluffily express 
771	78	6	5	13	12714.91	0.09	0.01	N	O	1995-08-10	1995-08-21	1995-08-30	NONE                     	FOB       	packages affix slyly about the quickly 
771	82	3	6	23	22587.84	0.08	0.03	N	O	1995-06-19	1995-09-07	1995-07-09	COLLECT COD              	FOB       	cajole besides the quickly ironic pin
774	183	4	1	49	53075.82	0.08	0.03	N	O	1995-12-06	1996-01-07	1995-12-14	DELIVER IN PERSON        	SHIP      	ess accounts are carefully 
774	17	4	2	3	2751.03	0.10	0.06	N	O	1996-02-13	1996-01-14	1996-03-04	COLLECT COD              	FOB       	 slyly even courts nag blith
774	148	7	3	34	35636.76	0.02	0.07	N	O	1996-03-16	1996-01-03	1996-03-22	COLLECT COD              	FOB       	lar excuses are furiously final instr
774	15	6	4	8	7320.08	0.00	0.02	N	O	1996-01-24	1996-01-15	1996-02-13	COLLECT COD              	RAIL      	ully ironic requests c
774	177	5	5	44	47395.48	0.09	0.07	N	O	1996-02-29	1996-01-16	1996-03-06	NONE                     	REG AIR   	s according to the deposits unwind ca
774	120	1	6	2	2040.24	0.07	0.03	N	O	1995-12-11	1996-02-10	1995-12-14	TAKE BACK RETURN         	SHIP      	accounts; slyly regular
775	32	3	1	16	14912.48	0.10	0.06	N	F	1995-05-23	1995-05-07	1995-06-19	NONE                     	TRUCK     	un quickly slyly
775	174	2	2	21	22557.57	0.01	0.06	R	F	1995-05-01	1995-06-02	1995-05-13	DELIVER IN PERSON        	FOB       	 quickly sile
775	108	5	3	20	20162.00	0.01	0.08	N	F	1995-06-17	1995-05-22	1995-07-13	COLLECT COD              	AIR       	en dependencies nag slowly 
800	72	1	1	38	36938.66	0.00	0.05	N	O	1998-07-21	1998-09-25	1998-08-07	TAKE BACK RETURN         	TRUCK     	according to the bold, final dependencies 
800	85	6	2	21	20686.68	0.04	0.05	N	O	1998-07-23	1998-10-01	1998-08-20	TAKE BACK RETURN         	RAIL      	ckly even requests after the carefully r
800	176	5	3	26	27980.42	0.01	0.02	N	O	1998-07-23	1998-10-08	1998-07-25	DELIVER IN PERSON        	FOB       	bove the pending requests.
1346	160	8	1	29	30744.64	0.07	0.05	A	F	1992-08-18	1992-09-15	1992-09-17	TAKE BACK RETURN         	REG AIR   	the pinto 
803	54	9	1	8	7632.40	0.07	0.01	N	O	1997-08-04	1997-06-19	1997-08-12	NONE                     	SHIP      	ronic theodo
803	99	10	2	21	20980.89	0.08	0.06	N	O	1997-08-25	1997-06-30	1997-09-10	TAKE BACK RETURN         	AIR       	ironic packages cajole slyly. un
804	126	7	1	30	30783.60	0.08	0.04	A	F	1993-03-29	1993-05-07	1993-04-14	COLLECT COD              	REG AIR   	ehind the quietly regular pac
804	199	3	2	2	2198.38	0.02	0.00	A	F	1993-06-23	1993-04-30	1993-06-25	NONE                     	TRUCK     	slyly silent 
804	76	5	3	44	42947.08	0.04	0.05	R	F	1993-07-06	1993-04-13	1993-07-28	DELIVER IN PERSON        	TRUCK     	ly final deposits? special 
804	38	9	4	21	19698.63	0.01	0.00	A	F	1993-04-12	1993-06-06	1993-04-20	DELIVER IN PERSON        	RAIL      	ular, ironic foxes. quickly even accounts
806	105	2	1	1	1005.10	0.04	0.07	N	O	1996-07-14	1996-09-12	1996-07-25	COLLECT COD              	RAIL      	ar accounts? pending, pending foxes a
806	160	5	2	22	23323.52	0.08	0.02	N	O	1996-10-03	1996-08-11	1996-10-20	DELIVER IN PERSON        	REG AIR   	fily pending 
806	91	3	3	4	3964.36	0.04	0.03	N	O	1996-08-09	1996-09-18	1996-08-13	COLLECT COD              	TRUCK     	eans. quickly ironic ideas 
832	103	6	1	45	45139.50	0.01	0.02	A	F	1992-05-08	1992-06-06	1992-06-04	COLLECT COD              	MAIL      	foxes engage slyly alon
832	48	1	2	24	22752.96	0.05	0.06	A	F	1992-06-15	1992-07-14	1992-06-17	NONE                     	TRUCK     	ully. carefully speci
833	54	5	1	1	954.05	0.04	0.04	R	F	1994-04-26	1994-04-05	1994-04-29	COLLECT COD              	MAIL      	ffily ironic theodolites
833	112	6	2	38	38460.18	0.05	0.05	A	F	1994-04-05	1994-04-21	1994-05-01	COLLECT COD              	TRUCK     	 platelets promise furiously. 
833	162	7	3	9	9559.44	0.05	0.07	A	F	1994-02-28	1994-04-26	1994-03-20	TAKE BACK RETURN         	FOB       	ecial, even requests. even, bold instructi
838	134	10	1	20	20682.60	0.10	0.07	N	O	1998-04-11	1998-03-25	1998-04-19	COLLECT COD              	TRUCK     	 furiously final ideas. slow, bold 
838	29	10	2	27	25083.54	0.05	0.07	N	O	1998-02-15	1998-04-03	1998-02-20	DELIVER IN PERSON        	SHIP      	 pending pinto beans haggle about t
838	95	7	3	23	22887.07	0.10	0.07	N	O	1998-03-26	1998-04-17	1998-04-02	COLLECT COD              	AIR       	ets haggle furiously furiously regular r
838	44	5	4	18	16992.72	0.09	0.00	N	O	1998-03-28	1998-04-06	1998-03-31	TAKE BACK RETURN         	AIR       	hely unusual foxes. furio
839	158	10	1	23	24337.45	0.07	0.02	N	O	1995-10-17	1995-11-03	1995-11-04	COLLECT COD              	AIR       	ng ideas haggle accord
839	189	10	2	47	51191.46	0.08	0.00	N	O	1995-10-17	1995-11-06	1995-11-10	NONE                     	AIR       	refully final excuses about 
864	130	5	1	34	35024.42	0.03	0.04	N	O	1997-12-16	1997-10-23	1998-01-12	TAKE BACK RETURN         	SHIP      	gside of the furiously special
864	98	1	2	7	6986.63	0.01	0.02	N	O	1997-11-13	1997-10-07	1997-12-13	TAKE BACK RETURN         	MAIL      	ven requests should sleep along 
864	80	10	3	34	33322.72	0.03	0.00	N	O	1997-09-14	1997-11-04	1997-09-21	TAKE BACK RETURN         	REG AIR   	to the furiously ironic platelets! 
865	198	10	1	16	17571.04	0.07	0.03	R	F	1993-08-24	1993-06-26	1993-08-28	TAKE BACK RETURN         	TRUCK     	y even accounts. quickly bold decoys
865	20	7	2	3	2760.06	0.02	0.05	A	F	1993-07-17	1993-07-14	1993-08-01	NONE                     	MAIL      	fully regular the
865	87	8	3	15	14806.20	0.00	0.06	R	F	1993-07-05	1993-06-25	1993-07-26	NONE                     	SHIP      	 deposits sleep quickl
865	169	4	4	34	36351.44	0.09	0.06	A	F	1993-05-09	1993-07-28	1993-05-18	DELIVER IN PERSON        	REG AIR   	furiously fluffily unusual account
866	136	7	1	5	5180.65	0.08	0.00	R	F	1993-01-22	1993-01-14	1993-02-07	TAKE BACK RETURN         	AIR       	tegrate fluffily. carefully f
868	168	9	1	8	8545.28	0.06	0.03	R	F	1992-10-07	1992-08-01	1992-10-16	NONE                     	MAIL      	l deposits. blithely regular pint
868	29	8	2	13	12077.26	0.05	0.07	R	F	1992-07-25	1992-08-26	1992-08-04	NONE                     	AIR       	gged instructi
868	68	5	3	19	18393.14	0.09	0.06	R	F	1992-06-22	1992-08-27	1992-07-04	COLLECT COD              	SHIP      	lyly ironic platelets wake. rut
868	122	1	4	43	43951.16	0.02	0.04	A	F	1992-07-02	1992-07-22	1992-07-21	COLLECT COD              	SHIP      	kly silent deposits wake dar
868	25	8	5	27	24975.54	0.04	0.01	R	F	1992-08-01	1992-08-25	1992-08-12	TAKE BACK RETURN         	RAIL      	oss the fluffily unusual pinto 
868	125	6	6	19	19477.28	0.02	0.05	R	F	1992-09-20	1992-07-18	1992-10-04	NONE                     	FOB       	ely even deposits lose blithe
869	63	2	1	27	26002.62	0.07	0.07	N	O	1997-01-30	1997-02-17	1997-02-26	TAKE BACK RETURN         	TRUCK     	uffily even excuses? slyly even deposits 
869	47	4	2	36	34093.44	0.04	0.01	N	O	1997-05-03	1997-03-17	1997-05-24	NONE                     	RAIL      	ong the furiously bold instructi
871	97	8	1	48	47860.32	0.10	0.03	N	O	1996-02-25	1996-02-09	1996-03-18	NONE                     	AIR       	coys dazzle slyly slow notornis. f
871	55	10	2	47	44887.35	0.07	0.03	N	O	1995-12-25	1996-02-01	1996-01-24	TAKE BACK RETURN         	RAIL      	ss, final dep
871	108	5	3	13	13105.30	0.09	0.01	N	O	1996-01-25	1996-01-24	1996-02-03	NONE                     	REG AIR   	 haggle furiou
871	190	1	4	29	31615.51	0.06	0.07	N	O	1995-11-16	1996-01-27	1995-12-16	DELIVER IN PERSON        	RAIL      	ests are carefu
871	128	7	5	8	8224.96	0.00	0.01	N	O	1995-11-25	1996-01-12	1995-12-12	DELIVER IN PERSON        	AIR       	lar ideas-- slyly even accou
871	143	2	6	26	27121.64	0.00	0.06	N	O	1996-02-07	1996-01-05	1996-02-25	COLLECT COD              	AIR       	symptotes use quickly near the 
871	174	3	7	4	4296.68	0.00	0.07	N	O	1996-03-09	1996-01-20	1996-03-26	COLLECT COD              	FOB       	l, regular dependencies w
897	91	4	1	15	14866.35	0.07	0.04	R	F	1995-05-25	1995-05-09	1995-06-07	COLLECT COD              	REG AIR   	r ideas. slyly spec
897	184	5	2	26	28188.68	0.05	0.08	N	O	1995-07-01	1995-06-10	1995-07-14	COLLECT COD              	MAIL      	tions sleep according to the special
897	126	1	3	13	13339.56	0.07	0.00	A	F	1995-03-30	1995-05-17	1995-04-21	TAKE BACK RETURN         	MAIL      	bold accounts mold carefully! braids
897	102	7	4	2	2004.20	0.08	0.08	R	F	1995-05-22	1995-05-07	1995-06-16	COLLECT COD              	RAIL      	into beans. slyly special fox
898	161	2	1	9	9550.44	0.07	0.08	A	F	1993-07-04	1993-07-09	1993-07-25	NONE                     	AIR       	e slyly across the blithe
898	179	7	2	37	39929.29	0.03	0.05	A	F	1993-08-17	1993-08-04	1993-09-01	DELIVER IN PERSON        	REG AIR   	packages sleep furiously
898	49	8	3	11	10439.44	0.01	0.00	A	F	1993-09-13	1993-08-31	1993-09-25	TAKE BACK RETURN         	MAIL      	etly bold accounts 
898	193	6	4	36	39354.84	0.04	0.07	R	F	1993-08-04	1993-07-25	1993-08-23	DELIVER IN PERSON        	REG AIR   	 after the carefully 
900	199	1	1	44	48364.36	0.01	0.06	R	F	1994-12-15	1994-12-03	1994-12-27	COLLECT COD              	MAIL      	 detect quick
900	115	6	2	48	48725.28	0.08	0.04	A	F	1994-12-22	1994-11-08	1995-01-19	COLLECT COD              	TRUCK     	cial pinto beans nag 
900	75	6	3	24	23401.68	0.03	0.00	R	F	1994-10-21	1994-12-25	1994-10-22	TAKE BACK RETURN         	SHIP      	-ray furiously un
901	22	7	1	36	33192.72	0.01	0.01	N	O	1998-08-11	1998-10-09	1998-08-27	DELIVER IN PERSON        	REG AIR   	. accounts are care
901	46	7	2	2	1892.08	0.09	0.02	N	O	1998-10-25	1998-09-27	1998-11-01	DELIVER IN PERSON        	AIR       	d foxes use slyly
901	43	10	3	37	34892.48	0.04	0.08	N	O	1998-11-01	1998-09-13	1998-11-05	NONE                     	AIR       	ickly final deposits 
901	18	9	4	11	10098.11	0.00	0.06	N	O	1998-11-13	1998-10-19	1998-11-14	TAKE BACK RETURN         	TRUCK     	ourts among the quickly expre
902	111	2	1	3	3033.33	0.06	0.00	R	F	1994-10-01	1994-10-25	1994-10-28	COLLECT COD              	MAIL      	into beans thrash blithely about the flu
902	118	2	2	8	8144.88	0.06	0.07	R	F	1994-10-25	1994-09-20	1994-11-07	COLLECT COD              	RAIL      	 orbits al
902	165	2	3	24	25563.84	0.02	0.05	R	F	1994-11-08	1994-10-12	1994-11-26	NONE                     	FOB       	. blithely even accounts poach furiously i
4967	50	1	3	15	14250.75	0.08	0.02	N	O	1997-04-16	1997-04-12	1997-05-08	TAKE BACK RETURN         	MAIL      	y. blithel
928	169	10	1	29	31005.64	0.07	0.02	R	F	1995-05-17	1995-05-12	1995-05-21	NONE                     	REG AIR   	ly alongside of the s
928	48	7	2	24	22752.96	0.05	0.08	A	F	1995-04-06	1995-05-08	1995-04-24	DELIVER IN PERSON        	AIR       	s the furiously regular warthogs im
928	152	10	3	46	48398.90	0.08	0.00	A	F	1995-05-09	1995-04-09	1995-06-01	DELIVER IN PERSON        	REG AIR   	 beans sleep against the carefully ir
928	52	4	4	43	40938.15	0.10	0.05	A	F	1995-04-14	1995-04-21	1995-05-09	NONE                     	REG AIR   	blithely. express, silent requests doze at
928	12	3	5	38	34656.38	0.02	0.08	N	F	1995-06-08	1995-04-15	1995-06-30	TAKE BACK RETURN         	SHIP      	xpress grouc
928	55	6	6	50	47752.50	0.05	0.00	N	F	1995-06-07	1995-04-15	1995-07-01	DELIVER IN PERSON        	TRUCK     	 slyly slyly special request
928	11	5	7	11	10021.11	0.00	0.01	A	F	1995-04-29	1995-04-16	1995-04-30	NONE                     	AIR       	longside of
930	45	4	1	36	34021.44	0.10	0.04	R	F	1994-12-21	1995-02-20	1994-12-24	COLLECT COD              	RAIL      	quickly regular pinto beans sle
930	18	8	2	47	43146.47	0.08	0.00	A	F	1995-03-20	1995-02-04	1995-04-04	DELIVER IN PERSON        	AIR       	ackages. fluffily e
930	65	10	3	10	9650.60	0.07	0.08	A	F	1994-12-18	1995-01-27	1995-01-16	COLLECT COD              	AIR       	ckly regular requests: regular instructions
930	100	2	4	21	21002.10	0.06	0.02	A	F	1995-02-16	1995-03-03	1995-03-13	DELIVER IN PERSON        	SHIP      	foxes. regular deposits integrate carefu
930	164	9	5	50	53208.00	0.03	0.06	A	F	1995-04-03	1995-01-29	1995-04-22	COLLECT COD              	MAIL      	 excuses among the furiously express ideas 
930	145	4	6	10	10451.40	0.00	0.04	A	F	1995-02-09	1995-02-17	1995-02-16	NONE                     	SHIP      	blithely bold i
930	167	4	7	30	32014.80	0.07	0.08	R	F	1995-01-20	1995-02-28	1995-02-04	TAKE BACK RETURN         	RAIL      	g accounts sleep along the platelets.
931	40	1	1	18	16920.72	0.00	0.05	A	F	1993-04-04	1993-01-11	1993-04-13	NONE                     	RAIL      	slyly ironic re
931	17	7	2	10	9170.10	0.05	0.07	A	F	1993-03-01	1993-01-09	1993-03-07	TAKE BACK RETURN         	SHIP      	ajole quickly. slyly sil
931	147	6	3	48	50262.72	0.01	0.08	A	F	1993-02-03	1993-03-02	1993-02-09	TAKE BACK RETURN         	FOB       	ep alongside of the fluffy 
931	82	3	4	38	37319.04	0.08	0.08	A	F	1993-03-06	1993-02-24	1993-03-27	DELIVER IN PERSON        	RAIL      	usly final packages integrate carefully
932	44	1	1	41	38705.64	0.01	0.05	N	O	1997-06-05	1997-07-22	1997-06-26	COLLECT COD              	RAIL      	foxes. ironic pl
934	118	5	1	18	18325.98	0.07	0.01	N	O	1996-09-10	1996-09-20	1996-09-25	COLLECT COD              	RAIL      	y unusual requests dazzle above t
935	28	3	1	23	21344.46	0.05	0.00	N	O	1997-11-11	1997-11-22	1997-11-29	COLLECT COD              	REG AIR   	ular accounts about
935	65	10	2	23	22196.38	0.02	0.01	N	O	1998-01-11	1997-11-25	1998-02-05	COLLECT COD              	TRUCK     	hes haggle furiously dolphins. qu
935	135	1	3	36	37264.68	0.06	0.00	N	O	1997-11-05	1997-12-05	1997-11-25	TAKE BACK RETURN         	AIR       	leep about the exp
935	58	3	4	13	12454.65	0.08	0.04	N	O	1998-01-13	1997-11-30	1998-02-08	DELIVER IN PERSON        	TRUCK     	ld platelet
935	13	7	5	8	7304.08	0.02	0.05	N	O	1998-01-12	1997-11-02	1998-02-05	NONE                     	TRUCK     	cept the quickly regular p
935	59	1	6	1	959.05	0.01	0.08	N	O	1997-12-14	1997-11-22	1998-01-08	DELIVER IN PERSON        	TRUCK     	 instructions. ironic acc
961	118	5	1	7	7126.77	0.10	0.00	N	O	1995-07-23	1995-07-20	1995-08-11	TAKE BACK RETURN         	RAIL      	usual dolphins. ironic pearls sleep blit
961	91	2	2	18	17839.62	0.09	0.05	N	O	1995-07-01	1995-08-14	1995-07-04	DELIVER IN PERSON        	AIR       	rmanent foxes haggle speci
961	97	8	3	42	41877.78	0.06	0.01	N	O	1995-08-24	1995-08-21	1995-09-10	TAKE BACK RETURN         	SHIP      	ests do cajole blithely. furiously bo
961	34	10	4	29	27086.87	0.00	0.07	N	F	1995-06-10	1995-08-20	1995-06-26	TAKE BACK RETURN         	TRUCK     	l accounts use blithely against the
961	26	7	5	38	35188.76	0.03	0.05	N	O	1995-08-21	1995-07-19	1995-08-27	NONE                     	RAIL      	he blithely special requests. furiousl
961	197	8	6	30	32915.70	0.09	0.03	N	O	1995-07-06	1995-07-20	1995-07-26	DELIVER IN PERSON        	MAIL      	warhorses slee
962	57	8	1	36	34453.80	0.01	0.03	R	F	1994-08-09	1994-07-10	1994-09-02	COLLECT COD              	TRUCK     	al foxes. iron
962	36	2	2	27	25272.81	0.09	0.02	A	F	1994-05-11	1994-07-10	1994-06-03	TAKE BACK RETURN         	SHIP      	y slyly express deposits. final i
962	80	1	3	3	2940.24	0.07	0.08	A	F	1994-05-08	1994-07-06	1994-06-02	DELIVER IN PERSON        	FOB       	ag furiously. even pa
962	57	5	4	20	19141.00	0.04	0.02	R	F	1994-08-26	1994-06-27	1994-09-11	DELIVER IN PERSON        	SHIP      	 deposits use fluffily according to 
962	152	7	5	12	12625.80	0.02	0.00	A	F	1994-06-09	1994-06-07	1994-06-11	COLLECT COD              	TRUCK     	across the furiously regular escapades daz
962	188	9	6	5	5440.90	0.02	0.05	A	F	1994-08-29	1994-07-15	1994-09-19	COLLECT COD              	TRUCK     	efully bold packages run slyly caref
964	199	10	1	39	42868.41	0.04	0.01	N	O	1995-06-21	1995-07-24	1995-06-24	NONE                     	AIR       	se furiously regular instructions. blith
964	113	4	2	1	1013.11	0.02	0.05	N	O	1995-08-20	1995-07-29	1995-09-10	DELIVER IN PERSON        	REG AIR   	unts. quickly even platelets s
964	57	5	3	49	46895.45	0.01	0.03	N	O	1995-09-06	1995-08-10	1995-10-05	NONE                     	MAIL      	ounts. blithely regular packag
964	55	3	4	44	42022.20	0.05	0.02	N	O	1995-09-18	1995-08-02	1995-10-17	TAKE BACK RETURN         	TRUCK     	ronic deposit
966	180	8	1	19	20523.42	0.07	0.01	N	O	1998-05-26	1998-07-15	1998-05-29	COLLECT COD              	FOB       	efully final pinto beans. quickly 
966	117	4	2	42	42718.62	0.02	0.06	N	O	1998-06-28	1998-06-20	1998-07-05	NONE                     	TRUCK     	tions boost furiously car
966	22	1	3	42	38724.84	0.06	0.08	N	O	1998-06-15	1998-06-08	1998-07-05	NONE                     	RAIL      	sly ironic asymptotes hagg
966	5	2	4	20	18100.00	0.04	0.01	N	O	1998-07-19	1998-07-15	1998-07-27	NONE                     	TRUCK     	pecial ins
967	59	4	1	41	39321.05	0.05	0.05	R	F	1992-09-21	1992-08-15	1992-10-21	NONE                     	MAIL      	ld foxes wake closely special
967	85	6	2	4	3940.32	0.01	0.02	A	F	1992-07-15	1992-07-27	1992-07-18	DELIVER IN PERSON        	REG AIR   	platelets hang carefully along 
967	132	8	3	10	10321.30	0.00	0.02	A	F	1992-09-18	1992-08-06	1992-09-19	DELIVER IN PERSON        	MAIL      	old pinto beans alongside of the exp
967	148	7	4	49	51358.86	0.01	0.04	A	F	1992-09-28	1992-09-15	1992-10-14	NONE                     	SHIP      	the slyly even ideas. carefully even
967	17	1	5	41	37597.41	0.08	0.04	A	F	1992-07-23	1992-08-07	1992-08-13	TAKE BACK RETURN         	FOB       	efully special ide
967	106	9	6	17	17103.70	0.05	0.06	A	F	1992-10-02	1992-08-19	1992-10-25	NONE                     	MAIL      	y ironic foxes caj
967	161	8	7	18	19100.88	0.00	0.02	A	F	1992-10-06	1992-08-05	1992-10-15	DELIVER IN PERSON        	RAIL      	ngage blith
992	60	2	1	14	13440.84	0.10	0.03	N	O	1998-01-29	1997-12-29	1998-02-18	TAKE BACK RETURN         	MAIL      	the unusual, even dependencies affix fluff
992	38	9	2	34	31893.02	0.02	0.00	N	O	1997-11-29	1998-01-21	1997-11-30	NONE                     	RAIL      	s use silently. blithely regular ideas b
992	105	6	3	30	30153.00	0.10	0.00	N	O	1997-12-15	1998-02-02	1998-01-12	NONE                     	SHIP      	nic instructions n
992	48	5	4	21	19908.84	0.06	0.06	N	O	1997-11-13	1997-12-28	1997-12-10	NONE                     	TRUCK     	fily. quickly special deposit
992	92	4	5	7	6944.63	0.09	0.05	N	O	1997-11-30	1997-12-24	1997-12-16	DELIVER IN PERSON        	TRUCK     	ideas haggle. special theodolit
992	75	3	6	41	39977.87	0.10	0.01	N	O	1997-11-14	1998-02-04	1997-11-23	TAKE BACK RETURN         	AIR       	eodolites cajole across the accounts.
993	175	5	1	33	35480.61	0.01	0.05	N	O	1996-01-03	1995-11-28	1996-01-23	DELIVER IN PERSON        	AIR       	 the deposits affix agains
993	3	6	2	28	25284.00	0.06	0.08	N	O	1995-10-24	1995-11-20	1995-11-06	DELIVER IN PERSON        	RAIL      	lites. even theodolite
993	40	1	3	10	9400.40	0.03	0.08	N	O	1995-12-17	1995-11-13	1995-12-20	NONE                     	RAIL      	encies wake fur
993	191	4	4	40	43647.60	0.01	0.01	N	O	1995-11-16	1995-11-01	1995-12-05	TAKE BACK RETURN         	RAIL      	gle above the furiously 
993	146	7	5	33	34522.62	0.09	0.08	N	O	1995-09-28	1995-10-24	1995-10-03	COLLECT COD              	RAIL      	fluffily. quiet excuses sleep furiously sly
993	137	3	6	35	36299.55	0.04	0.02	N	O	1995-10-26	1995-10-20	1995-11-05	DELIVER IN PERSON        	FOB       	es. ironic, ironic requests
993	5	2	7	15	13575.00	0.09	0.03	N	O	1995-09-27	1995-10-21	1995-10-17	DELIVER IN PERSON        	MAIL      	sits. pending pinto beans haggle? ca
996	173	2	1	43	46146.31	0.03	0.06	N	O	1998-03-27	1998-03-25	1998-04-06	COLLECT COD              	SHIP      	 the blithely ironic foxes. slyly silent d
1024	199	2	1	49	53860.31	0.03	0.05	N	O	1998-03-06	1998-01-26	1998-03-29	TAKE BACK RETURN         	FOB       	ts. asymptotes nag fur
5413	189	10	5	5	5445.90	0.10	0.01	N	O	1997-11-28	1997-11-24	1997-12-05	NONE                     	RAIL      	tes are al
1024	126	5	2	34	34888.08	0.00	0.01	N	O	1998-01-06	1998-02-05	1998-01-26	COLLECT COD              	SHIP      	des the slyly even
1024	44	3	3	28	26433.12	0.04	0.01	N	O	1998-03-04	1998-03-12	1998-03-15	TAKE BACK RETURN         	TRUCK     	e blithely regular pi
1024	184	5	4	13	14094.34	0.02	0.04	N	O	1998-04-11	1998-02-26	1998-04-18	NONE                     	FOB       	e slyly around the slyly special instructi
1024	21	4	5	49	45129.98	0.02	0.04	N	O	1998-02-27	1998-03-10	1998-03-27	COLLECT COD              	FOB       	 carefully bold 
1025	150	1	1	36	37805.40	0.03	0.04	A	F	1995-05-15	1995-07-05	1995-06-10	COLLECT COD              	FOB       	e unusual, regular instr
1025	69	10	2	23	22288.38	0.08	0.03	N	F	1995-06-02	1995-07-29	1995-06-23	COLLECT COD              	RAIL      	 regular platelets nag carefu
1025	23	2	3	25	23075.50	0.06	0.05	R	F	1995-05-29	1995-06-21	1995-06-13	DELIVER IN PERSON        	REG AIR   	xpress foxes. furiousl
1026	38	4	1	36	33769.08	0.10	0.02	N	O	1997-06-14	1997-07-20	1997-06-23	NONE                     	SHIP      	st the ide
1026	37	8	2	6	5622.18	0.10	0.08	N	O	1997-07-07	1997-08-16	1997-07-14	TAKE BACK RETURN         	TRUCK     	to beans. special, regular packages hagg
1027	156	1	1	43	45414.45	0.07	0.08	R	F	1992-06-17	1992-08-28	1992-07-10	DELIVER IN PERSON        	MAIL      	oxes. carefully regular deposits
1027	113	10	2	20	20262.20	0.01	0.02	A	F	1992-06-08	1992-08-29	1992-06-14	NONE                     	TRUCK     	ar excuses eat f
1027	126	9	3	2	2052.24	0.01	0.02	R	F	1992-08-28	1992-07-09	1992-09-10	NONE                     	FOB       	s. quickly unusual waters inside 
1027	100	4	4	13	13001.30	0.08	0.01	R	F	1992-08-22	1992-07-10	1992-09-12	DELIVER IN PERSON        	RAIL      	ily ironic ideas use
1027	136	2	5	22	22794.86	0.02	0.00	A	F	1992-09-03	1992-08-14	1992-10-01	DELIVER IN PERSON        	FOB       	the furiously express ex
1027	105	8	6	10	10051.00	0.06	0.08	R	F	1992-08-28	1992-08-06	1992-09-03	COLLECT COD              	REG AIR   	ilent, express foxes near the blithely sp
1029	137	3	1	45	46670.85	0.05	0.07	R	F	1994-07-21	1994-08-30	1994-07-29	TAKE BACK RETURN         	FOB       	sits boost blithely
1030	65	10	1	17	16406.02	0.06	0.06	R	F	1994-10-13	1994-08-01	1994-11-10	DELIVER IN PERSON        	RAIL      	ly. carefully even packages dazz
1031	46	7	1	15	14190.60	0.10	0.08	A	F	1994-11-07	1994-10-29	1994-11-09	TAKE BACK RETURN         	FOB       	about the carefully bold a
1031	165	4	2	28	29824.48	0.05	0.01	A	F	1994-12-10	1994-10-29	1994-12-18	COLLECT COD              	FOB       	ly ironic accounts across the q
1031	187	8	3	27	29353.86	0.07	0.02	R	F	1994-09-20	1994-10-18	1994-10-10	DELIVER IN PERSON        	SHIP      	gular deposits cajole. blithely unus
1031	88	9	4	7	6916.56	0.03	0.03	R	F	1994-12-07	1994-11-11	1994-12-30	COLLECT COD              	FOB       	r instructions. car
1031	191	5	5	44	48012.36	0.01	0.07	R	F	1994-11-20	1994-11-24	1994-12-11	NONE                     	AIR       	re slyly above the furio
1056	121	6	1	37	37781.44	0.04	0.06	R	F	1995-02-18	1995-04-01	1995-03-20	NONE                     	TRUCK     	 special packages. qui
1057	193	5	1	29	31702.51	0.10	0.01	A	F	1992-05-05	1992-05-05	1992-06-03	TAKE BACK RETURN         	SHIP      	es wake according to the q
1057	169	8	2	11	11760.76	0.00	0.02	R	F	1992-03-31	1992-04-18	1992-04-18	COLLECT COD              	AIR       	yly final theodolites. furi
1057	85	6	3	21	20686.68	0.03	0.04	A	F	1992-02-28	1992-05-01	1992-03-10	NONE                     	REG AIR   	ar orbits boost bli
1057	182	3	4	20	21643.60	0.06	0.03	R	F	1992-03-02	1992-05-19	1992-03-13	DELIVER IN PERSON        	TRUCK     	s wake bol
1057	97	1	5	7	6979.63	0.06	0.05	R	F	1992-06-05	1992-04-30	1992-06-20	NONE                     	TRUCK     	y slyly express theodolites. slyly bo
1057	52	7	6	19	18088.95	0.04	0.07	A	F	1992-05-31	1992-05-09	1992-06-02	DELIVER IN PERSON        	FOB       	r-- packages haggle alon
1060	196	10	1	8	8769.52	0.07	0.04	R	F	1993-05-21	1993-05-06	1993-06-10	DELIVER IN PERSON        	FOB       	iously. furiously regular in
1060	8	5	2	26	23608.00	0.06	0.08	R	F	1993-04-12	1993-04-01	1993-04-20	DELIVER IN PERSON        	TRUCK     	counts; even deposits are carefull
1060	164	3	3	11	11705.76	0.01	0.07	A	F	1993-05-13	1993-05-08	1993-05-17	TAKE BACK RETURN         	MAIL      	e regular deposits: re
1060	110	7	4	16	16161.76	0.03	0.06	A	F	1993-06-15	1993-04-18	1993-07-05	COLLECT COD              	SHIP      	ccounts. foxes maintain care
1060	53	8	5	1	953.05	0.04	0.06	A	F	1993-06-19	1993-05-10	1993-06-21	COLLECT COD              	RAIL      	posits detect carefully abo
1060	72	2	6	26	25273.82	0.01	0.03	A	F	1993-02-28	1993-04-01	1993-03-09	TAKE BACK RETURN         	FOB       	quickly abo
1060	121	10	7	36	36760.32	0.09	0.01	R	F	1993-03-14	1993-03-24	1993-04-02	TAKE BACK RETURN         	FOB       	r the quickly
1061	151	6	1	7	7358.05	0.04	0.04	N	O	1998-08-09	1998-08-12	1998-08-16	COLLECT COD              	FOB       	es are slyly expr
1061	119	3	2	2	2038.22	0.06	0.02	N	O	1998-08-15	1998-08-05	1998-08-24	COLLECT COD              	MAIL      	. regular accounts impre
1061	111	8	3	26	26288.86	0.08	0.02	N	O	1998-06-18	1998-07-25	1998-06-24	TAKE BACK RETURN         	AIR       	ave to slee
1061	136	7	4	41	42481.33	0.00	0.05	N	O	1998-06-29	1998-07-02	1998-07-27	NONE                     	MAIL      	s are. ironic theodolites cajole. dep
1061	131	2	5	50	51556.50	0.04	0.08	N	O	1998-05-25	1998-07-22	1998-06-22	COLLECT COD              	AIR       	nding excuses are around the e
1061	144	1	6	35	36544.90	0.09	0.05	N	O	1998-07-05	1998-07-07	1998-07-30	TAKE BACK RETURN         	MAIL      	ending requests nag careful
1062	137	8	1	38	39410.94	0.00	0.01	N	O	1997-01-27	1997-03-07	1997-02-16	DELIVER IN PERSON        	TRUCK     	deas. pending acc
1063	96	9	1	42	41835.78	0.03	0.02	A	F	1994-07-10	1994-05-25	1994-07-26	NONE                     	RAIL      	tructions about the blithely ex
1088	107	8	1	30	30213.00	0.07	0.03	A	F	1992-05-22	1992-06-25	1992-06-11	TAKE BACK RETURN         	SHIP      	long the packages snooze careful
1088	37	3	2	11	10307.33	0.06	0.00	A	F	1992-08-30	1992-07-25	1992-09-10	TAKE BACK RETURN         	AIR       	inal requests. fluffily express theod
1088	181	2	3	5	5405.90	0.03	0.07	A	F	1992-07-01	1992-07-25	1992-07-02	NONE                     	AIR       	refully ironic packages. r
1088	124	5	4	3	3072.36	0.09	0.03	A	F	1992-06-15	1992-08-02	1992-06-18	DELIVER IN PERSON        	MAIL      	pecial theodolites 
1089	151	3	1	47	49404.05	0.05	0.06	N	O	1996-06-26	1996-06-25	1996-07-11	NONE                     	TRUCK     	aggle furiously among the bravely eve
1089	50	7	2	35	33251.75	0.03	0.00	N	O	1996-08-14	1996-07-10	1996-08-26	NONE                     	TRUCK     	ly express deposits haggle
1089	26	7	3	23	21298.46	0.10	0.05	N	O	1996-06-24	1996-07-25	1996-07-20	DELIVER IN PERSON        	AIR       	g dolphins. deposits integrate. s
1089	141	10	4	1	1041.14	0.01	0.03	N	O	1996-07-08	1996-07-07	1996-07-17	COLLECT COD              	RAIL      	n courts among the caref
1093	87	8	1	7	6909.56	0.04	0.02	N	O	1997-11-24	1997-09-23	1997-11-25	TAKE BACK RETURN         	SHIP      	bold deposits. blithely ironic depos
1093	177	5	2	37	39855.29	0.08	0.04	N	O	1997-11-06	1997-10-08	1997-11-22	COLLECT COD              	FOB       	le furiously across the carefully sp
1093	61	2	3	34	32676.04	0.01	0.06	N	O	1997-11-07	1997-09-06	1997-11-28	TAKE BACK RETURN         	REG AIR   	sits. express accounts play carefully. bol
1120	178	8	1	10	10781.70	0.08	0.05	N	O	1997-12-17	1998-01-21	1997-12-23	DELIVER IN PERSON        	MAIL      	dependencies. blithel
1120	20	1	2	49	45080.98	0.01	0.07	N	O	1998-01-03	1998-02-02	1998-01-09	TAKE BACK RETURN         	RAIL      	heodolites. quick re
1120	76	6	3	21	20497.47	0.06	0.01	N	O	1998-01-11	1998-02-04	1998-01-19	COLLECT COD              	REG AIR   	s: fluffily even packages c
1120	46	9	4	22	20812.88	0.09	0.08	N	O	1997-11-15	1998-01-25	1997-12-07	TAKE BACK RETURN         	REG AIR   	ons. slyly silent requests sleep silent
1120	83	4	5	10	9830.80	0.07	0.08	N	O	1997-11-10	1998-02-01	1997-11-28	TAKE BACK RETURN         	AIR       	ages haggle furiously 
1121	168	3	1	42	44862.72	0.04	0.05	N	O	1997-03-05	1997-03-18	1997-03-14	DELIVER IN PERSON        	SHIP      	nts are slyly special packages. f
1121	161	10	2	27	28651.32	0.08	0.00	N	O	1997-05-08	1997-03-28	1997-05-14	NONE                     	MAIL      	ly ironic accounts cajole slyly abou
1121	157	5	3	10	10571.50	0.00	0.04	N	O	1997-04-17	1997-03-18	1997-05-02	TAKE BACK RETURN         	RAIL      	dencies. quickly regular theodolites n
1121	166	1	4	29	30918.64	0.02	0.01	N	O	1997-03-07	1997-04-02	1997-04-01	DELIVER IN PERSON        	REG AIR   	 use furiously. quickly silent package
1121	30	9	5	47	43711.41	0.09	0.03	N	O	1997-04-27	1997-03-28	1997-05-14	COLLECT COD              	FOB       	ly idle, i
1121	200	1	6	50	55010.00	0.06	0.03	N	O	1997-04-21	1997-02-16	1997-04-25	NONE                     	TRUCK     	odolites. slyly even accounts
1121	80	8	7	37	36262.96	0.06	0.01	N	O	1997-02-27	1997-03-04	1997-03-02	COLLECT COD              	RAIL      	special packages. fluffily final requests s
1123	12	2	1	10	9120.10	0.05	0.08	N	O	1996-11-12	1996-10-04	1996-11-30	NONE                     	MAIL      	ckages are above the depths. slyly ir
1123	178	8	2	39	42048.63	0.03	0.08	N	O	1996-08-25	1996-10-21	1996-09-04	DELIVER IN PERSON        	REG AIR   	rding to the furiously ironic requests: r
1123	101	4	3	38	38041.80	0.03	0.08	N	O	1996-09-23	1996-10-04	1996-09-27	DELIVER IN PERSON        	FOB       	 blithely carefully unusual reques
1124	198	2	1	1	1098.19	0.09	0.08	N	O	1998-10-06	1998-10-02	1998-10-30	NONE                     	REG AIR   	 instructions cajole qu
1124	6	1	2	13	11778.00	0.05	0.04	N	O	1998-09-05	1998-10-03	1998-09-30	DELIVER IN PERSON        	SHIP      	t the slyly 
1124	93	5	3	35	34758.15	0.10	0.05	N	O	1998-11-25	1998-10-08	1998-12-25	TAKE BACK RETURN         	AIR       	ut the slyly bold pinto beans; fi
1124	50	1	4	25	23751.25	0.08	0.05	N	O	1998-08-05	1998-10-14	1998-08-11	NONE                     	MAIL      	ggle slyly according
1124	75	5	5	33	32177.31	0.05	0.04	N	O	1998-10-19	1998-09-17	1998-10-26	TAKE BACK RETURN         	SHIP      	eposits sleep slyly. stealthily f
1124	27	6	6	43	39861.86	0.01	0.03	N	O	1998-09-19	1998-10-28	1998-10-10	COLLECT COD              	MAIL      	across the 
1124	95	6	7	1	995.09	0.09	0.01	N	O	1998-10-07	1998-08-31	1998-10-12	NONE                     	TRUCK     	ly bold accou
1125	133	4	1	4	4132.52	0.08	0.02	A	F	1994-12-10	1994-12-28	1994-12-30	NONE                     	MAIL      	 quickly express packages a
1125	138	9	2	24	24915.12	0.10	0.03	R	F	1995-01-31	1994-12-02	1995-02-20	COLLECT COD              	AIR       	es about the slyly s
1125	122	7	3	26	26575.12	0.05	0.04	A	F	1995-02-24	1995-01-18	1995-03-05	COLLECT COD              	TRUCK     	l instruction
1125	98	1	4	29	28944.61	0.06	0.00	A	F	1994-11-29	1994-12-20	1994-12-10	DELIVER IN PERSON        	RAIL      	 platelets wake against the carefully i
1152	9	10	1	23	20907.00	0.06	0.04	A	F	1994-10-14	1994-10-22	1994-10-21	DELIVER IN PERSON        	MAIL      	equests alongside of the unusual 
1152	100	2	2	25	25002.50	0.04	0.08	R	F	1994-10-20	1994-09-18	1994-10-28	DELIVER IN PERSON        	REG AIR   	efully ironic accounts. sly instructions wa
1152	42	9	3	6	5652.24	0.07	0.03	A	F	1994-12-07	1994-11-05	1994-12-25	DELIVER IN PERSON        	FOB       	p furiously; packages above th
1154	143	10	1	31	32337.34	0.06	0.06	A	F	1992-04-17	1992-04-26	1992-05-17	COLLECT COD              	AIR       	ithely. final, blithe 
1154	148	7	2	50	52407.00	0.07	0.06	A	F	1992-04-22	1992-04-21	1992-05-01	NONE                     	TRUCK     	ove the furiously bold Tires
1154	97	1	3	5	4985.45	0.09	0.04	A	F	1992-06-07	1992-05-07	1992-07-05	DELIVER IN PERSON        	MAIL      	the furiously 
1154	1	2	4	35	31535.00	0.00	0.07	A	F	1992-03-30	1992-04-02	1992-04-21	DELIVER IN PERSON        	TRUCK     	the carefully regular pinto beans boost
1154	36	2	5	18	16848.54	0.02	0.03	A	F	1992-02-26	1992-03-24	1992-03-20	TAKE BACK RETURN         	REG AIR   	y regular excuses cajole blithely. fi
1154	196	8	6	50	54809.50	0.06	0.03	A	F	1992-03-04	1992-04-01	1992-04-01	TAKE BACK RETURN         	TRUCK     	 even, special 
1158	45	2	1	5	4725.20	0.02	0.04	N	O	1996-10-20	1996-07-30	1996-11-14	COLLECT COD              	AIR       	symptotes along the care
1158	157	9	2	23	24314.45	0.00	0.08	N	O	1996-10-21	1996-08-19	1996-10-31	COLLECT COD              	MAIL      	ularly ironic requests use care
1185	72	1	1	8	7776.56	0.01	0.06	A	F	1992-12-05	1992-10-05	1992-12-28	DELIVER IN PERSON        	MAIL      	ely according to the furiously regular r
1185	31	2	2	28	26068.84	0.07	0.06	A	F	1992-09-24	1992-10-07	1992-10-10	DELIVER IN PERSON        	REG AIR   	ke. slyly regular t
1185	190	1	3	12	13082.28	0.05	0.06	R	F	1992-10-12	1992-09-26	1992-11-11	NONE                     	REG AIR   	instructions. daringly pend
1186	3	4	1	28	25284.00	0.08	0.07	N	O	1996-12-08	1996-10-17	1996-12-15	TAKE BACK RETURN         	TRUCK     	ffily spec
1186	92	5	2	11	10912.99	0.07	0.05	N	O	1996-10-03	1996-10-21	1996-10-17	DELIVER IN PERSON        	AIR       	s haggle furiously; slyl
1186	101	2	3	20	20022.00	0.07	0.07	N	O	1996-08-20	1996-10-23	1996-09-05	COLLECT COD              	FOB       	ely alongside of the blithel
1186	106	7	4	27	27164.70	0.06	0.04	N	O	1996-10-08	1996-11-06	1996-10-09	TAKE BACK RETURN         	SHIP      	accounts. express, e
1187	178	6	1	29	31266.93	0.01	0.04	R	F	1992-12-10	1993-02-09	1992-12-29	TAKE BACK RETURN         	RAIL      	riously express ac
1187	131	7	2	15	15466.95	0.03	0.04	A	F	1992-12-22	1993-01-13	1993-01-01	NONE                     	TRUCK     	ests. foxes wake. carefu
1187	78	8	3	40	39122.80	0.08	0.06	R	F	1993-03-05	1992-12-31	1993-03-12	NONE                     	TRUCK     	ar, brave deposits nag blithe
1189	51	2	1	23	21874.15	0.06	0.00	R	F	1994-07-25	1994-06-07	1994-08-02	COLLECT COD              	FOB       	s. fluffy Tiresias run quickly. bra
1189	105	2	2	32	32163.20	0.09	0.02	R	F	1994-05-06	1994-07-03	1994-05-15	TAKE BACK RETURN         	FOB       	e regular deposits. quickly quiet deposi
1189	57	5	3	22	21055.10	0.05	0.03	R	F	1994-06-09	1994-06-29	1994-06-23	DELIVER IN PERSON        	TRUCK     	quickly unusual platelets lose forges. ca
1190	84	5	1	32	31490.56	0.07	0.06	N	O	1997-05-08	1997-04-17	1997-06-01	COLLECT COD              	FOB       	y final packages? slyly even
1216	97	1	1	8	7976.72	0.03	0.04	R	F	1993-02-01	1993-03-06	1993-02-08	TAKE BACK RETURN         	TRUCK     	 of the carefully express
1216	75	3	2	48	46803.36	0.10	0.01	R	F	1993-01-17	1993-02-01	1993-02-13	COLLECT COD              	SHIP      	symptotes use against th
1216	42	3	3	18	16956.72	0.00	0.03	A	F	1993-01-20	1993-01-28	1993-02-02	COLLECT COD              	MAIL      	y final packages nod 
1218	140	6	1	16	16642.24	0.04	0.07	A	F	1994-06-26	1994-08-07	1994-06-30	TAKE BACK RETURN         	FOB       	ven realms be
1218	94	6	2	41	40757.69	0.06	0.06	R	F	1994-08-04	1994-08-05	1994-08-11	TAKE BACK RETURN         	SHIP      	dolphins. theodolites beyond th
1218	48	7	3	44	41713.76	0.07	0.06	A	F	1994-10-05	1994-09-03	1994-10-30	COLLECT COD              	TRUCK     	thely ironic accounts wake slyly
1218	42	9	4	1	942.04	0.01	0.08	R	F	1994-09-15	1994-09-07	1994-10-03	COLLECT COD              	TRUCK     	press furio
1219	132	3	1	6	6192.78	0.08	0.04	N	O	1995-11-13	1995-12-24	1995-11-18	NONE                     	MAIL      	pecial, ironic requ
1219	129	4	2	4	4116.48	0.01	0.04	N	O	1995-11-24	1995-11-22	1995-12-07	TAKE BACK RETURN         	SHIP      	lly quick requests. blithely even h
1220	169	4	1	25	26729.00	0.10	0.03	N	O	1996-10-15	1996-11-07	1996-11-06	COLLECT COD              	REG AIR   	 regular orbi
1220	160	5	2	36	38165.76	0.01	0.02	N	O	1996-12-10	1996-11-14	1997-01-07	COLLECT COD              	SHIP      	ar packages. blithely final acc
1220	37	8	3	3	2811.09	0.08	0.06	N	O	1996-09-06	1996-11-03	1996-09-10	COLLECT COD              	REG AIR   	 final theodolites. blithely silent 
1220	6	1	4	36	32616.00	0.07	0.03	N	O	1996-12-12	1996-10-03	1996-12-15	TAKE BACK RETURN         	TRUCK     	unusual, silent pinto beans aga
1220	49	2	5	25	23726.00	0.03	0.08	N	O	1996-09-11	1996-10-09	1996-09-25	DELIVER IN PERSON        	RAIL      	packages affi
1221	81	2	1	43	42186.44	0.05	0.05	R	F	1992-06-22	1992-07-15	1992-07-20	DELIVER IN PERSON        	FOB       	y slyly above the slyly unusual ideas
1221	170	1	2	12	12842.04	0.00	0.08	R	F	1992-08-07	1992-06-24	1992-08-13	COLLECT COD              	AIR       	yly ironic 
1221	69	6	3	3	2907.18	0.10	0.08	R	F	1992-07-01	1992-06-04	1992-07-27	COLLECT COD              	TRUCK     	ing to the fluffily
1221	120	10	4	41	41824.92	0.06	0.02	A	F	1992-04-28	1992-07-02	1992-05-19	NONE                     	RAIL      	ns. bold deposit
1221	108	1	5	13	13105.30	0.10	0.00	R	F	1992-08-01	1992-06-29	1992-08-27	TAKE BACK RETURN         	AIR       	ajole furiously. blithely expres
1221	85	6	6	7	6895.56	0.08	0.06	A	F	1992-06-27	1992-06-16	1992-07-23	TAKE BACK RETURN         	RAIL      	xpress accounts 
1222	72	10	1	12	11664.84	0.09	0.02	A	F	1993-02-12	1993-03-14	1993-03-12	TAKE BACK RETURN         	RAIL      	s print permanently unusual packages. 
1222	159	7	2	12	12709.80	0.08	0.01	A	F	1993-05-05	1993-03-27	1993-05-18	TAKE BACK RETURN         	REG AIR   	 furiously bold instructions
1222	8	1	3	26	23608.00	0.02	0.08	R	F	1993-02-13	1993-03-20	1993-02-22	TAKE BACK RETURN         	MAIL      	, even accounts are ironic
1223	100	1	1	28	28002.80	0.10	0.06	N	O	1996-08-07	1996-07-24	1996-08-13	TAKE BACK RETURN         	MAIL      	 quickly ironic requests. furious
1248	164	5	1	45	47887.20	0.00	0.08	A	F	1992-04-17	1992-03-31	1992-05-13	NONE                     	RAIL      	ter the pending pl
1248	151	9	2	37	38892.55	0.06	0.06	R	F	1992-01-26	1992-02-05	1992-02-13	COLLECT COD              	TRUCK     	. final requests integrate quickly. blit
1248	56	8	3	26	24857.30	0.09	0.06	A	F	1992-01-16	1992-03-01	1992-02-06	TAKE BACK RETURN         	AIR       	 ironic dependen
1248	156	7	4	49	51751.35	0.02	0.01	A	F	1992-04-24	1992-02-18	1992-05-03	TAKE BACK RETURN         	AIR       	beans run quickly according to the carefu
1248	122	7	5	20	20442.40	0.08	0.00	A	F	1992-03-12	1992-03-23	1992-04-07	TAKE BACK RETURN         	AIR       	nal foxes cajole carefully slyl
1248	62	9	6	30	28861.80	0.10	0.01	R	F	1992-02-01	1992-03-24	1992-02-08	TAKE BACK RETURN         	MAIL      	fily special foxes kindle am
1250	2	3	1	15	13530.00	0.10	0.06	A	F	1992-11-05	1992-12-17	1992-12-03	TAKE BACK RETURN         	SHIP      	 regular, i
1251	4	5	1	37	33448.00	0.08	0.08	N	O	1997-12-21	1998-01-12	1997-12-26	COLLECT COD              	AIR       	. furiously
1251	78	9	2	36	35210.52	0.07	0.04	N	O	1997-11-29	1998-01-07	1997-12-03	TAKE BACK RETURN         	RAIL      	y ironic Tiresias are slyly furio
1251	99	3	3	37	36966.33	0.09	0.02	N	O	1998-01-11	1997-12-01	1998-01-23	DELIVER IN PERSON        	RAIL      	finally bold requests
1251	150	9	4	7	7351.05	0.07	0.00	N	O	1998-01-08	1997-12-27	1998-01-18	COLLECT COD              	MAIL      	riously pe
1251	188	9	5	1	1088.18	0.02	0.03	N	O	1997-12-08	1998-01-06	1998-01-01	DELIVER IN PERSON        	REG AIR   	 use quickly final packages. iron
1253	180	8	1	14	15122.52	0.00	0.06	R	F	1993-04-03	1993-04-16	1993-04-27	TAKE BACK RETURN         	MAIL      	lar foxes sleep furiously final, final pack
1253	54	9	2	13	12402.65	0.01	0.06	A	F	1993-03-05	1993-04-26	1993-03-08	DELIVER IN PERSON        	FOB       	al packages
1253	70	1	3	22	21341.54	0.05	0.06	A	F	1993-02-23	1993-04-06	1993-03-07	TAKE BACK RETURN         	SHIP      	telets cajole alongside of the final reques
1253	176	5	4	23	24751.91	0.09	0.02	R	F	1993-04-18	1993-04-18	1993-05-07	COLLECT COD              	FOB       	 the slyly silent re
1253	114	8	5	19	19268.09	0.05	0.05	A	F	1993-04-01	1993-04-22	1993-04-14	TAKE BACK RETURN         	AIR       	al pinto bea
1255	192	4	1	12	13106.28	0.00	0.02	A	F	1994-08-17	1994-06-29	1994-09-04	TAKE BACK RETURN         	REG AIR   	 regular, express accounts are 
1255	194	8	2	46	50332.74	0.07	0.05	R	F	1994-07-06	1994-07-14	1994-08-05	NONE                     	MAIL      	ons nag qui
1281	138	4	1	33	34258.29	0.07	0.08	R	F	1995-02-01	1995-01-18	1995-03-03	NONE                     	REG AIR   	dencies. thinly final pinto beans wake
1281	7	2	2	37	33559.00	0.08	0.03	A	F	1995-03-19	1995-02-02	1995-03-27	NONE                     	AIR       	ounts detect
1281	94	7	3	2	1988.18	0.05	0.06	A	F	1994-12-27	1995-01-26	1995-01-21	TAKE BACK RETURN         	FOB       	ly unusual requests. final reques
1281	154	2	4	38	40057.70	0.04	0.06	R	F	1995-03-28	1995-01-11	1995-04-14	TAKE BACK RETURN         	MAIL      	 ideas-- blithely regular
1281	152	10	5	13	13677.95	0.03	0.07	A	F	1995-02-06	1995-02-13	1995-02-18	DELIVER IN PERSON        	TRUCK     	fully final platelets wa
1281	50	9	6	4	3800.20	0.07	0.04	R	F	1995-03-15	1995-02-21	1995-03-20	NONE                     	SHIP      	ggle against the even requests. requests 
1281	78	6	7	43	42057.01	0.10	0.02	R	F	1995-01-28	1995-02-08	1995-02-10	DELIVER IN PERSON        	AIR       	final accounts. final packages slee
1284	178	7	1	49	52830.33	0.00	0.06	N	O	1996-04-11	1996-03-04	1996-04-16	NONE                     	MAIL      	lar packages. special packages ac
1284	6	7	2	4	3624.00	0.07	0.06	N	O	1996-02-29	1996-02-11	1996-03-01	TAKE BACK RETURN         	TRUCK     	 regular asymptotes. 
1284	133	4	3	39	40292.07	0.08	0.00	N	O	1996-01-11	1996-02-07	1996-02-05	COLLECT COD              	MAIL      	even accoun
1284	59	10	4	1	959.05	0.01	0.07	N	O	1996-04-28	1996-04-02	1996-05-08	DELIVER IN PERSON        	SHIP      	al packages use carefully express de
1284	34	5	5	9	8406.27	0.05	0.06	N	O	1996-03-03	1996-03-19	1996-04-01	DELIVER IN PERSON        	REG AIR   	after the pending
1313	52	4	1	48	45698.40	0.01	0.03	A	F	1994-12-20	1994-10-29	1995-01-07	COLLECT COD              	MAIL      	s are quick
1315	96	8	1	27	26894.43	0.01	0.03	N	O	1998-07-04	1998-06-13	1998-07-28	NONE                     	SHIP      	latelets. fluffily ironic account
1315	16	6	2	15	13740.15	0.05	0.01	N	O	1998-07-12	1998-06-10	1998-08-07	COLLECT COD              	AIR       	. foxes integrate carefully special
1315	168	3	3	25	26704.00	0.01	0.08	N	O	1998-06-26	1998-06-10	1998-07-06	TAKE BACK RETURN         	FOB       	lites. unusual foxes affi
1315	161	6	4	19	20162.04	0.02	0.05	N	O	1998-07-05	1998-05-23	1998-08-04	TAKE BACK RETURN         	SHIP      	nal, regular warhorses about the fu
1315	159	7	5	32	33892.80	0.10	0.05	N	O	1998-03-30	1998-06-12	1998-04-25	NONE                     	SHIP      	neath the final p
1316	127	6	1	46	47247.52	0.05	0.04	A	F	1994-01-13	1994-01-24	1994-02-03	COLLECT COD              	TRUCK     	ges haggle of the
1316	79	9	2	15	14686.05	0.02	0.01	R	F	1994-03-12	1994-03-02	1994-03-14	COLLECT COD              	FOB       	se. furiously final depo
1316	198	9	3	33	36240.27	0.10	0.06	R	F	1994-03-31	1994-01-23	1994-04-20	TAKE BACK RETURN         	AIR       	manently; blithely special deposits
1316	66	3	4	15	14490.90	0.00	0.06	R	F	1993-12-17	1994-02-04	1993-12-20	NONE                     	RAIL      	fully express dugouts. furiously silent ide
1316	41	2	5	40	37641.60	0.01	0.03	R	F	1994-02-04	1994-02-09	1994-02-27	NONE                     	REG AIR   	l dugouts. co
1316	4	7	6	7	6328.00	0.05	0.04	A	F	1993-12-09	1994-01-12	1993-12-30	TAKE BACK RETURN         	MAIL      	. furiously even accounts a
1316	163	8	7	8	8505.28	0.10	0.04	A	F	1994-03-26	1994-02-08	1994-04-19	NONE                     	SHIP      	packages against the express requests wa
1318	114	4	1	24	24338.64	0.08	0.06	N	O	1998-09-27	1998-09-15	1998-10-12	TAKE BACK RETURN         	AIR       	ual, unusual packages. fluffy, iro
1318	46	3	2	26	24597.04	0.01	0.03	N	O	1998-09-26	1998-08-09	1998-10-07	DELIVER IN PERSON        	FOB       	ly. regular, u
1318	129	4	3	31	31902.72	0.01	0.04	N	O	1998-08-25	1998-07-31	1998-08-31	COLLECT COD              	AIR       	ve the carefully expr
1344	141	4	1	15	15617.10	0.10	0.07	A	F	1992-06-22	1992-06-24	1992-06-23	TAKE BACK RETURN         	MAIL      	rding to the blithely ironic theodolite
1344	190	1	2	29	31615.51	0.09	0.00	A	F	1992-07-17	1992-06-07	1992-07-21	NONE                     	REG AIR   	ffily quiet foxes wake blithely. slyly 
1346	125	6	2	48	49205.76	0.06	0.03	A	F	1992-09-28	1992-07-22	1992-10-13	TAKE BACK RETURN         	REG AIR   	 along the carefully spec
1346	54	5	3	13	12402.65	0.10	0.04	A	F	1992-07-22	1992-08-10	1992-08-06	NONE                     	SHIP      	arefully brave deposits into the slyly iro
1346	124	5	4	6	6144.72	0.02	0.02	R	F	1992-09-13	1992-07-21	1992-09-27	TAKE BACK RETURN         	AIR       	inst the furiously final theodolites. caref
1346	187	8	5	30	32615.40	0.01	0.07	R	F	1992-10-01	1992-07-22	1992-10-24	NONE                     	SHIP      	 nag blithely. unusual, ru
1346	16	6	6	45	41220.45	0.02	0.04	A	F	1992-09-11	1992-08-06	1992-09-12	COLLECT COD              	FOB       	press deposits.
1347	81	2	1	45	44148.60	0.02	0.05	N	O	1997-08-24	1997-09-03	1997-09-08	COLLECT COD              	AIR       	ages wake around t
1347	143	6	2	34	35466.76	0.07	0.04	N	O	1997-06-25	1997-09-08	1997-07-24	COLLECT COD              	FOB       	r packages. f
1347	185	6	3	23	24959.14	0.03	0.04	N	O	1997-07-31	1997-08-25	1997-08-21	COLLECT COD              	SHIP      	ronic pinto beans. express reques
1347	113	7	4	28	28367.08	0.01	0.00	N	O	1997-07-30	1997-07-22	1997-08-18	TAKE BACK RETURN         	FOB       	foxes after the blithely special i
1347	65	6	5	9	8685.54	0.01	0.03	N	O	1997-08-28	1997-09-16	1997-09-26	DELIVER IN PERSON        	AIR       	 detect blithely above the fina
1347	153	8	6	21	22116.15	0.06	0.04	N	O	1997-10-10	1997-08-16	1997-11-02	NONE                     	FOB       	g pinto beans affix car
1347	51	3	7	10	9510.50	0.02	0.07	N	O	1997-07-04	1997-07-23	1997-07-05	DELIVER IN PERSON        	SHIP      	y ironic pin
1349	181	2	1	1	1081.18	0.06	0.03	N	O	1998-01-07	1998-01-14	1998-02-03	COLLECT COD              	REG AIR   	 express inst
1349	118	2	2	45	45814.95	0.03	0.02	N	O	1997-12-24	1998-01-17	1997-12-28	NONE                     	AIR       	 ironic, unusual deposits wake carefu
1350	54	9	1	21	20035.05	0.04	0.04	A	F	1993-12-17	1993-10-17	1993-12-25	COLLECT COD              	REG AIR   	lyly above the evenly 
1350	44	5	2	32	30209.28	0.03	0.00	R	F	1993-11-18	1993-09-30	1993-12-16	COLLECT COD              	MAIL      	ic, final 
1351	108	9	1	25	25202.50	0.06	0.04	N	O	1998-06-02	1998-05-25	1998-06-22	COLLECT COD              	SHIP      	iously regul
1376	169	8	1	22	23521.52	0.01	0.03	N	O	1997-08-05	1997-07-08	1997-09-03	NONE                     	REG AIR   	inst the final, pending 
1408	148	7	1	29	30396.06	0.03	0.04	N	O	1998-03-12	1998-02-14	1998-03-17	COLLECT COD              	MAIL      	en accounts grow furiousl
1408	173	2	2	7	7512.19	0.05	0.06	N	O	1998-01-14	1998-03-21	1998-01-29	COLLECT COD              	AIR       	fully final instructions. theodolites ca
1408	76	6	3	11	10736.77	0.00	0.03	N	O	1998-04-04	1998-01-29	1998-04-18	NONE                     	REG AIR   	y even accounts thrash care
1408	148	5	4	20	20962.80	0.06	0.00	N	O	1998-04-21	1998-01-25	1998-05-12	DELIVER IN PERSON        	TRUCK     	 blithely fluffi
1408	170	1	5	41	43876.97	0.02	0.06	N	O	1998-02-25	1998-02-03	1998-03-13	COLLECT COD              	REG AIR   	ep along the fina
1408	134	10	6	42	43433.46	0.05	0.08	N	O	1998-01-30	1998-02-07	1998-02-18	TAKE BACK RETURN         	REG AIR   	even packages. even accounts cajole
1408	55	6	7	26	24831.30	0.00	0.00	N	O	1998-03-19	1998-03-14	1998-04-01	COLLECT COD              	RAIL      	ic foxes ca
1410	121	10	1	15	15316.80	0.06	0.05	N	O	1997-05-25	1997-07-08	1997-06-15	NONE                     	SHIP      	 bold packages are fluf
1410	179	9	2	18	19425.06	0.03	0.00	N	O	1997-06-03	1997-05-17	1997-06-07	TAKE BACK RETURN         	RAIL      	gle furiously fluffily regular requests
1410	109	4	3	37	37336.70	0.02	0.01	N	O	1997-04-17	1997-06-18	1997-04-19	COLLECT COD              	TRUCK     	to beans b
1410	188	9	4	22	23939.96	0.10	0.00	N	O	1997-07-31	1997-05-17	1997-08-19	TAKE BACK RETURN         	RAIL      	gular account
1410	66	1	5	25	24151.50	0.09	0.02	N	O	1997-05-07	1997-07-10	1997-05-16	NONE                     	REG AIR   	unts haggle against the furiously fina
1413	178	9	1	18	19407.06	0.08	0.05	N	O	1997-10-11	1997-08-17	1997-10-25	NONE                     	FOB       	yly bold packages haggle quickly acr
1413	165	10	2	49	52192.84	0.07	0.06	N	O	1997-08-28	1997-08-23	1997-09-12	DELIVER IN PERSON        	MAIL      	nstructions br
1413	42	9	3	6	5652.24	0.04	0.02	N	O	1997-09-07	1997-07-30	1997-09-21	TAKE BACK RETURN         	MAIL      	lithely excuses. f
1440	193	6	1	3	3279.57	0.06	0.01	N	O	1995-10-30	1995-10-17	1995-11-08	COLLECT COD              	SHIP      	instructions boost. fluffily regul
1440	114	4	2	46	46649.06	0.02	0.03	N	O	1995-09-21	1995-10-19	1995-10-19	NONE                     	RAIL      	blithely even instructions. 
1441	144	7	1	5	5220.70	0.04	0.01	N	O	1997-05-17	1997-05-11	1997-05-30	COLLECT COD              	MAIL      	egular courts. fluffily even grouches 
1441	177	7	2	5	5385.85	0.02	0.05	N	O	1997-04-25	1997-04-16	1997-05-23	COLLECT COD              	FOB       	he quickly enticing pac
1441	118	5	3	14	14253.54	0.01	0.03	N	O	1997-06-30	1997-04-29	1997-07-24	DELIVER IN PERSON        	REG AIR   	special requests ha
1441	160	8	4	37	39225.92	0.01	0.00	N	O	1997-04-26	1997-04-27	1997-04-29	NONE                     	REG AIR   	accounts. slyly special dolphins b
1441	72	10	5	34	33050.38	0.09	0.00	N	O	1997-06-12	1997-05-11	1997-06-29	TAKE BACK RETURN         	RAIL      	e carefully. blithely ironic dep
1441	25	4	6	15	13875.30	0.09	0.08	N	O	1997-05-21	1997-05-06	1997-06-04	NONE                     	REG AIR   	 dependencies-- cour
1441	96	10	7	50	49804.50	0.03	0.01	N	O	1997-06-07	1997-05-12	1997-06-08	NONE                     	SHIP      	 requests. blithely e
1444	170	5	1	42	44947.14	0.01	0.02	R	F	1994-12-22	1995-03-03	1994-12-31	NONE                     	SHIP      	ly bold packages boost regular ideas. spe
1444	57	2	2	34	32539.70	0.04	0.08	A	F	1995-02-22	1995-02-15	1995-03-19	TAKE BACK RETURN         	AIR       	y. doggedly pend
1444	155	3	3	34	35875.10	0.02	0.07	R	F	1994-12-17	1995-01-12	1995-01-03	COLLECT COD              	AIR       	ular accounts 
1444	119	6	4	6	6114.66	0.06	0.03	A	F	1995-01-07	1995-03-05	1995-01-17	COLLECT COD              	RAIL      	al accounts. br
1444	20	1	5	35	32200.70	0.02	0.05	A	F	1995-02-25	1995-03-05	1995-03-24	DELIVER IN PERSON        	SHIP      	aggle furiou
1444	33	4	6	42	39187.26	0.00	0.02	A	F	1994-12-16	1995-02-18	1994-12-22	DELIVER IN PERSON        	RAIL      	ss requests. ironic ideas wake above
1444	82	3	7	12	11784.96	0.00	0.03	R	F	1994-12-23	1995-01-15	1995-01-13	COLLECT COD              	TRUCK     	ly among the bol
1445	100	1	1	24	24002.40	0.01	0.00	A	F	1995-02-21	1995-02-22	1995-03-18	DELIVER IN PERSON        	SHIP      	al accounts use furiously a
1445	67	8	2	48	46418.88	0.10	0.02	A	F	1995-02-28	1995-03-16	1995-03-12	COLLECT COD              	MAIL      	. final ideas are carefully dar
1445	192	4	3	7	7645.33	0.10	0.04	A	F	1995-04-25	1995-02-25	1995-05-10	NONE                     	SHIP      	structions: slyly regular re
1445	28	1	4	17	15776.34	0.04	0.07	A	F	1995-04-02	1995-04-04	1995-05-01	COLLECT COD              	FOB       	ges. furiously regular pint
1445	135	1	5	24	24843.12	0.10	0.06	R	F	1995-04-23	1995-02-16	1995-05-18	NONE                     	REG AIR   	rate after the carefully reg
1445	168	9	6	39	41658.24	0.03	0.02	A	F	1995-02-05	1995-02-20	1995-02-06	NONE                     	MAIL      	ully unusual reques
1446	72	3	1	31	30134.17	0.10	0.02	N	O	1998-05-01	1998-05-17	1998-05-30	NONE                     	REG AIR   	. slyly reg
1447	167	4	1	19	20276.04	0.06	0.04	A	F	1993-01-31	1992-12-07	1993-02-04	COLLECT COD              	MAIL      	. quickly ironic 
1447	32	3	2	6	5592.18	0.01	0.05	A	F	1992-10-24	1992-12-10	1992-11-05	DELIVER IN PERSON        	AIR       	as! regular packages poach above the
1447	39	5	3	9	8451.27	0.04	0.00	R	F	1992-11-15	1993-01-07	1992-11-29	DELIVER IN PERSON        	MAIL      	counts wake s
1447	22	5	4	8	7376.16	0.09	0.08	R	F	1992-11-20	1993-01-12	1992-12-14	COLLECT COD              	FOB       	ost carefully 
1447	130	1	5	23	23692.99	0.02	0.07	A	F	1992-12-07	1992-12-25	1993-01-06	TAKE BACK RETURN         	AIR       	 dazzle quickly deposits. f
1447	200	3	6	41	45108.20	0.08	0.02	R	F	1993-01-06	1993-01-05	1993-01-13	TAKE BACK RETURN         	MAIL      	rts boost s
1473	54	9	1	50	47702.50	0.04	0.03	N	O	1997-05-05	1997-05-20	1997-05-09	NONE                     	TRUCK     	requests wake express deposits. special, ir
1473	68	3	2	32	30977.92	0.00	0.08	N	O	1997-04-18	1997-05-12	1997-05-10	DELIVER IN PERSON        	REG AIR   	out the packages lose furiously ab
1475	168	3	1	15	16022.40	0.08	0.06	N	O	1998-02-12	1997-12-17	1998-03-02	TAKE BACK RETURN         	SHIP      	xpress requests haggle after the final, fi
1475	118	9	2	18	18325.98	0.07	0.00	N	O	1998-03-08	1998-01-18	1998-03-10	TAKE BACK RETURN         	AIR       	al deposits use. ironic packages along the 
1475	144	1	3	30	31324.20	0.03	0.02	N	O	1998-03-11	1997-12-30	1998-03-15	COLLECT COD              	REG AIR   	 regular theodolites mold across th
1475	187	8	4	50	54359.00	0.03	0.05	N	O	1997-12-14	1997-12-13	1997-12-21	COLLECT COD              	AIR       	. slyly bold re
1475	32	3	5	33	30756.99	0.01	0.06	N	O	1998-01-02	1998-01-27	1998-01-11	NONE                     	FOB       	quickly fluffy
1475	50	7	6	12	11400.60	0.04	0.04	N	O	1998-01-09	1997-12-30	1998-01-23	NONE                     	TRUCK     	arefully-- excuses sublate
1475	112	3	7	23	23278.53	0.02	0.00	N	O	1998-02-13	1998-02-05	1998-03-08	NONE                     	TRUCK     	hely regular hocke
1477	72	1	1	31	30134.17	0.00	0.06	N	O	1997-12-16	1997-09-30	1997-12-17	COLLECT COD              	RAIL      	 requests. fluffily final 
1477	110	7	2	8	8080.88	0.09	0.05	N	O	1997-10-25	1997-10-18	1997-11-16	COLLECT COD              	MAIL      	ironic realms wake unusual, even ac
1477	125	6	3	42	43055.04	0.06	0.00	N	O	1997-11-02	1997-11-02	1997-11-20	DELIVER IN PERSON        	SHIP      	lithely after the ir
1477	107	8	4	32	32227.20	0.05	0.08	N	O	1997-09-12	1997-10-26	1997-10-12	TAKE BACK RETURN         	AIR       	; quickly regula
1477	115	6	5	41	41619.51	0.04	0.06	N	O	1997-12-16	1997-10-31	1998-01-12	DELIVER IN PERSON        	REG AIR   	y. final pearls kindle. accounts 
1477	69	6	6	49	47483.94	0.06	0.00	N	O	1997-11-18	1997-11-06	1997-11-27	COLLECT COD              	FOB       	ise according to the sly, bold p
1477	120	4	7	33	33663.96	0.06	0.00	N	O	1997-11-12	1997-11-06	1997-11-24	DELIVER IN PERSON        	TRUCK     	yly regular p
1478	34	5	1	21	19614.63	0.00	0.06	N	O	1997-09-20	1997-10-25	1997-10-06	TAKE BACK RETURN         	MAIL      	 fluffily pending acc
1479	149	6	1	33	34621.62	0.10	0.01	N	O	1996-03-12	1996-02-28	1996-03-31	DELIVER IN PERSON        	FOB       	 carefully special courts affix. fluff
1505	120	7	1	4	4080.48	0.09	0.00	A	F	1992-12-14	1992-11-11	1993-01-02	COLLECT COD              	SHIP      	side of the s
1505	123	8	2	50	51156.00	0.00	0.02	R	F	1992-11-22	1992-09-24	1992-11-26	TAKE BACK RETURN         	FOB       	lyly special platelets. requests ar
1506	133	4	1	46	47523.98	0.04	0.05	R	F	1993-01-18	1992-11-11	1993-02-09	COLLECT COD              	REG AIR   	sits whithout the blithely ironic packages
1506	114	4	2	30	30423.30	0.07	0.02	A	F	1992-11-22	1992-10-25	1992-12-04	DELIVER IN PERSON        	FOB       	deposits cajole 
1506	191	3	3	28	30553.32	0.10	0.06	A	F	1992-09-22	1992-11-19	1992-10-09	TAKE BACK RETURN         	AIR       	 unwind carefully: theodolit
1506	28	7	4	37	34336.74	0.00	0.03	R	F	1992-11-04	1992-12-01	1992-11-23	TAKE BACK RETURN         	TRUCK     	carefully bold dolphins. accounts su
1506	195	8	5	15	16427.85	0.05	0.00	R	F	1992-09-24	1992-11-11	1992-10-05	NONE                     	REG AIR   	 carefully fluffy packages-- caref
1506	50	3	6	38	36101.90	0.05	0.02	R	F	1992-12-02	1992-12-19	1992-12-29	NONE                     	REG AIR   	xpress, regular excuse
1506	169	6	7	4	4276.64	0.07	0.00	R	F	1993-01-03	1992-12-06	1993-01-05	COLLECT COD              	REG AIR   	posits. furiou
1508	51	3	1	16	15216.80	0.02	0.06	N	O	1998-06-21	1998-05-30	1998-07-11	COLLECT COD              	MAIL      	riously across the ironic, unusua
1508	25	4	2	20	18500.40	0.06	0.01	N	O	1998-04-17	1998-06-11	1998-05-17	DELIVER IN PERSON        	MAIL      	nic platelets. carefully final fra
1508	93	7	3	43	42702.87	0.01	0.02	N	O	1998-06-01	1998-06-24	1998-06-03	TAKE BACK RETURN         	TRUCK     	ndencies h
1508	148	7	4	1	1048.14	0.02	0.02	N	O	1998-07-13	1998-06-03	1998-07-17	TAKE BACK RETURN         	AIR       	s the blithely bold instruction
1508	135	6	5	29	30018.77	0.02	0.00	N	O	1998-08-03	1998-07-08	1998-08-22	COLLECT COD              	RAIL      	r instructions. carefully
1508	3	10	6	5	4515.00	0.06	0.08	N	O	1998-05-22	1998-07-06	1998-06-04	COLLECT COD              	REG AIR   	cording to the furiously ironic depe
1508	117	8	7	38	38650.18	0.03	0.06	N	O	1998-04-30	1998-06-23	1998-05-18	DELIVER IN PERSON        	RAIL      	tes wake furiously regular w
1509	28	7	1	14	12992.28	0.04	0.01	A	F	1993-10-04	1993-09-25	1993-10-21	NONE                     	TRUCK     	nal realms
1509	11	2	2	46	41906.46	0.08	0.02	A	F	1993-10-15	1993-10-04	1993-11-01	TAKE BACK RETURN         	FOB       	uriously regula
1509	107	8	3	17	17120.70	0.06	0.05	A	F	1993-07-25	1993-08-28	1993-08-19	DELIVER IN PERSON        	AIR       	 furiously. blithely regular ideas haggle c
1509	20	4	4	11	10120.22	0.03	0.08	R	F	1993-11-04	1993-10-03	1993-11-14	TAKE BACK RETURN         	FOB       	ily ironic packages nod carefully.
1509	90	1	5	37	36633.33	0.01	0.08	A	F	1993-08-31	1993-09-10	1993-09-24	NONE                     	FOB       	he slyly even deposits wake a
1509	187	8	6	31	33702.58	0.04	0.03	A	F	1993-07-14	1993-08-21	1993-08-06	COLLECT COD              	SHIP      	ic deposits cajole carefully. quickly bold 
1509	157	2	7	27	28543.05	0.01	0.01	A	F	1993-09-29	1993-09-08	1993-10-04	TAKE BACK RETURN         	FOB       	lithely after the 
1536	194	5	1	5	5470.95	0.08	0.03	N	O	1997-02-08	1997-03-11	1997-03-02	COLLECT COD              	MAIL      	requests sleep pe
1538	102	5	1	32	32067.20	0.05	0.05	N	O	1995-07-08	1995-07-29	1995-08-01	TAKE BACK RETURN         	RAIL      	uses maintain blithely. fluffily
1538	192	3	2	27	29489.13	0.05	0.01	N	O	1995-09-19	1995-08-03	1995-09-24	DELIVER IN PERSON        	TRUCK     	ngly even packag
1538	130	3	3	36	37084.68	0.08	0.04	N	O	1995-07-11	1995-09-10	1995-07-26	DELIVER IN PERSON        	MAIL      	al deposits mo
1538	104	1	4	28	28114.80	0.10	0.04	N	O	1995-09-19	1995-08-27	1995-10-10	COLLECT COD              	RAIL      	bout the fluffily unusual
1538	178	7	5	13	14016.21	0.01	0.05	N	O	1995-06-26	1995-07-30	1995-07-25	NONE                     	SHIP      	ly. packages sleep f
1538	128	3	6	42	43181.04	0.08	0.08	N	O	1995-10-10	1995-09-12	1995-11-08	DELIVER IN PERSON        	TRUCK     	equests cajole blithely 
1540	173	1	1	38	40780.46	0.03	0.01	R	F	1992-09-30	1992-10-27	1992-10-12	TAKE BACK RETURN         	SHIP      	 final grouches bo
1540	60	2	2	35	33602.10	0.02	0.07	R	F	1992-10-31	1992-09-04	1992-11-05	TAKE BACK RETURN         	SHIP      	e blithely a
1540	8	3	3	25	22700.00	0.08	0.04	R	F	1992-11-15	1992-10-24	1992-12-14	DELIVER IN PERSON        	SHIP      	ironic deposits amo
1540	25	8	4	6	5550.12	0.09	0.03	R	F	1992-08-28	1992-09-17	1992-09-14	COLLECT COD              	MAIL      	ing to the slyly express asymptote
1540	87	8	5	27	26651.16	0.10	0.08	R	F	1992-12-02	1992-10-18	1992-12-31	NONE                     	SHIP      	carefully final packages; b
1541	64	3	1	44	42418.64	0.10	0.05	N	O	1995-08-24	1995-07-13	1995-08-26	TAKE BACK RETURN         	MAIL      	o beans boost fluffily abou
1541	26	7	2	8	7408.16	0.10	0.08	N	F	1995-06-05	1995-08-07	1995-06-21	TAKE BACK RETURN         	TRUCK     	y pending packages. blithely fi
1543	71	10	1	34	33016.38	0.02	0.08	N	O	1997-05-25	1997-03-30	1997-06-04	NONE                     	AIR       	ic requests are ac
1543	115	9	2	6	6090.66	0.09	0.01	N	O	1997-04-16	1997-05-20	1997-05-16	DELIVER IN PERSON        	MAIL      	 among the carefully bold or
1543	67	8	3	42	40616.52	0.06	0.01	N	O	1997-05-26	1997-03-30	1997-06-12	DELIVER IN PERSON        	FOB       	its sleep until the fur
1543	189	10	4	42	45745.56	0.05	0.06	N	O	1997-04-11	1997-04-11	1997-04-23	TAKE BACK RETURN         	MAIL      	xpress instructions. regular acc
1543	40	1	5	9	8460.36	0.08	0.06	N	O	1997-03-14	1997-05-19	1997-03-26	DELIVER IN PERSON        	FOB       	ravely special requests 
1543	49	8	6	3	2847.12	0.10	0.04	N	O	1997-03-29	1997-05-10	1997-04-22	COLLECT COD              	MAIL      	sleep along the furiou
1543	68	7	7	3	2904.18	0.00	0.02	N	O	1997-03-22	1997-04-06	1997-03-30	NONE                     	AIR       	quickly. final accounts haggle slyl
1568	90	1	1	36	35643.24	0.02	0.03	N	O	1997-05-31	1997-04-22	1997-06-21	TAKE BACK RETURN         	RAIL      	platelets-- furiously sly excu
1568	9	2	2	46	41814.00	0.04	0.00	N	O	1997-04-06	1997-04-08	1997-04-23	TAKE BACK RETURN         	MAIL      	g the blithely even acco
1569	75	3	1	5	4875.35	0.07	0.00	N	O	1998-04-16	1998-06-21	1998-04-18	COLLECT COD              	REG AIR   	 packages. ironic, even excuses a
1569	39	10	2	16	15024.48	0.01	0.08	N	O	1998-04-26	1998-06-16	1998-05-26	COLLECT COD              	MAIL      	deposits. blithely final asymptotes ac
1569	49	10	3	43	40808.72	0.10	0.03	N	O	1998-06-05	1998-05-31	1998-06-28	DELIVER IN PERSON        	FOB       	 instructions.
1569	70	1	4	30	29102.10	0.02	0.03	N	O	1998-07-19	1998-06-04	1998-08-10	NONE                     	SHIP      	packages. excuses lose evenly carefully reg
1571	52	3	1	47	44746.35	0.00	0.05	R	F	1992-12-07	1993-02-24	1993-01-01	TAKE BACK RETURN         	REG AIR   	ng to the fluffily unusual 
1571	183	4	2	6	6499.08	0.03	0.00	A	F	1993-01-08	1993-02-13	1993-02-07	COLLECT COD              	SHIP      	 special, ironic depo
1571	59	7	3	18	17262.90	0.05	0.08	A	F	1993-01-09	1993-01-12	1993-01-31	COLLECT COD              	AIR       	 pending grouches 
1571	101	4	4	48	48052.80	0.05	0.05	A	F	1992-12-28	1993-01-04	1993-01-04	DELIVER IN PERSON        	RAIL      	slyly pending p
1571	42	5	5	10	9420.40	0.03	0.06	R	F	1992-12-12	1993-02-13	1992-12-29	DELIVER IN PERSON        	AIR       	lets. carefully regular ideas wake
1571	34	10	6	24	22416.72	0.05	0.07	A	F	1993-03-22	1993-01-31	1993-04-09	NONE                     	TRUCK     	warthogs wake carefully acro
1573	186	7	1	5	5430.90	0.05	0.01	A	F	1993-04-24	1993-03-13	1993-05-17	TAKE BACK RETURN         	MAIL      	ymptotes could u
1573	31	2	2	17	15827.51	0.00	0.06	R	F	1993-02-24	1993-02-16	1993-03-08	TAKE BACK RETURN         	TRUCK     	carefully regular deposits. 
1573	83	4	3	16	15729.28	0.04	0.03	A	F	1993-03-15	1993-03-16	1993-03-31	COLLECT COD              	AIR       	ely. furiously final requests wake slyl
1573	194	7	4	11	12036.09	0.09	0.01	R	F	1993-03-23	1993-03-24	1993-04-12	TAKE BACK RETURN         	RAIL      	nently pending
1573	137	8	5	7	7259.91	0.00	0.01	R	F	1993-01-30	1993-03-14	1993-02-27	DELIVER IN PERSON        	SHIP      	eodolites sleep slyly. slyly f
1573	154	6	6	30	31624.50	0.03	0.01	A	F	1992-12-29	1993-03-06	1993-01-02	DELIVER IN PERSON        	TRUCK     	. blithely even theodolites boos
1574	48	7	1	41	38869.64	0.06	0.02	N	O	1997-03-08	1997-02-09	1997-04-01	COLLECT COD              	AIR       	s. slyly regular depen
1574	191	5	2	50	54559.50	0.00	0.05	N	O	1996-12-14	1997-02-14	1996-12-16	TAKE BACK RETURN         	FOB       	le regular, regular foxes. blithely e
1574	55	3	3	25	23876.25	0.06	0.02	N	O	1997-01-16	1997-02-14	1997-02-12	DELIVER IN PERSON        	TRUCK     	ly silent accounts.
1574	191	4	4	6	6547.14	0.03	0.05	N	O	1997-02-24	1997-02-03	1997-03-01	NONE                     	AIR       	e silent, final packages. speci
1574	109	4	5	6	6054.60	0.05	0.05	N	O	1997-02-09	1997-03-02	1997-02-14	COLLECT COD              	MAIL      	nic, final ideas snooze. 
1574	5	2	6	42	38010.00	0.07	0.01	N	O	1996-12-19	1997-01-13	1996-12-28	NONE                     	FOB       	o beans according t
1574	136	7	7	14	14505.82	0.04	0.01	N	O	1996-12-30	1997-01-19	1997-01-20	NONE                     	AIR       	ily bold a
1600	172	10	1	20	21443.40	0.02	0.01	R	F	1993-06-16	1993-04-23	1993-07-02	COLLECT COD              	FOB       	pths sleep blithely about the
1600	44	3	2	48	45313.92	0.07	0.02	R	F	1993-04-17	1993-04-14	1993-05-03	DELIVER IN PERSON        	FOB       	furiously silent foxes could wake. car
1600	39	10	3	8	7512.24	0.04	0.07	R	F	1993-03-07	1993-04-22	1993-03-26	TAKE BACK RETURN         	FOB       	cajole furiously fluf
1600	69	8	4	25	24226.50	0.00	0.06	A	F	1993-05-25	1993-04-07	1993-06-05	TAKE BACK RETURN         	REG AIR   	press packages. ironic excuses bo
1600	147	8	5	30	31414.20	0.03	0.08	R	F	1993-06-03	1993-05-03	1993-06-07	DELIVER IN PERSON        	RAIL      	al escapades alongside of the depo
1604	42	3	1	15	14130.60	0.09	0.08	R	F	1993-09-22	1993-09-03	1993-09-29	TAKE BACK RETURN         	MAIL      	 instructions haggle
1604	141	4	2	37	38522.18	0.06	0.06	A	F	1993-08-22	1993-09-21	1993-09-10	COLLECT COD              	SHIP      	requests. blithely ironic somas s
1604	114	8	3	19	19268.09	0.09	0.07	A	F	1993-10-15	1993-10-04	1993-11-09	COLLECT COD              	RAIL      	 ideas. bol
1604	175	4	4	15	16127.55	0.03	0.00	R	F	1993-09-10	1993-08-31	1993-09-30	TAKE BACK RETURN         	RAIL      	ending realms along the special, p
1604	21	4	5	23	21183.46	0.08	0.05	A	F	1993-10-11	1993-08-30	1993-10-18	DELIVER IN PERSON        	RAIL      	en requests. blithely fin
1632	191	5	1	47	51285.93	0.08	0.00	N	O	1997-01-25	1997-02-09	1997-02-19	TAKE BACK RETURN         	RAIL      	g to the closely special no
1632	148	7	2	14	14673.96	0.08	0.05	N	O	1997-01-15	1997-02-25	1997-01-28	NONE                     	RAIL      	oxes. deposits nag slyly along the slyly 
1632	177	6	3	47	50626.99	0.03	0.04	N	O	1997-01-29	1997-03-03	1997-02-21	NONE                     	MAIL      	sts. blithely regular 
1632	57	9	4	33	31582.65	0.09	0.02	N	O	1997-04-01	1997-02-24	1997-04-29	TAKE BACK RETURN         	REG AIR   	ructions! slyly
1632	142	1	5	43	44812.02	0.10	0.03	N	O	1997-02-24	1997-02-19	1997-03-25	DELIVER IN PERSON        	FOB       	ts. blithe, bold ideas cajo
1633	178	7	1	35	37735.95	0.01	0.02	N	O	1996-01-09	1995-12-02	1996-01-21	COLLECT COD              	REG AIR   	ly against the dolph
1633	5	6	2	15	13575.00	0.00	0.05	N	O	1995-12-13	1995-11-13	1996-01-04	TAKE BACK RETURN         	FOB       	ges wake fluffil
1635	71	1	1	3	2913.21	0.06	0.08	N	O	1997-03-13	1997-03-25	1997-03-27	COLLECT COD              	FOB       	 quickly ironic r
1635	90	1	2	8	7920.72	0.04	0.05	N	O	1997-04-30	1997-04-21	1997-05-09	DELIVER IN PERSON        	AIR       	ravely carefully express 
1635	114	5	3	20	20282.20	0.07	0.01	N	O	1997-05-19	1997-04-01	1997-06-17	TAKE BACK RETURN         	FOB       	oost according to the carefully even accou
1635	77	5	4	40	39082.80	0.01	0.04	N	O	1997-02-25	1997-03-20	1997-03-12	TAKE BACK RETURN         	RAIL      	uriously up the ironic deposits. slyly i
1637	86	7	1	49	48317.92	0.02	0.03	N	F	1995-06-08	1995-04-19	1995-07-01	COLLECT COD              	REG AIR   	. blithely i
1637	73	2	2	1	973.07	0.10	0.02	A	F	1995-02-14	1995-03-26	1995-03-09	TAKE BACK RETURN         	AIR       	ly final pinto beans. furiously
1637	22	1	3	10	9220.20	0.02	0.05	R	F	1995-02-21	1995-03-17	1995-03-11	NONE                     	AIR       	uriously? blithely even sauternes wake. 
1637	93	5	4	42	41709.78	0.06	0.01	A	F	1995-03-18	1995-04-24	1995-03-31	COLLECT COD              	SHIP      	blithely a
1637	5	8	5	25	22625.00	0.05	0.00	R	F	1995-06-07	1995-03-26	1995-06-08	COLLECT COD              	RAIL      	 haggle carefully silent accou
1637	109	4	6	38	38345.80	0.02	0.08	R	F	1995-03-20	1995-05-05	1995-04-14	DELIVER IN PERSON        	SHIP      	even, pending foxes nod regular
1637	52	10	7	21	19993.05	0.07	0.08	A	F	1995-04-30	1995-04-30	1995-05-05	COLLECT COD              	SHIP      	ly ironic theodolites use b
1638	6	7	1	46	41676.00	0.03	0.02	N	O	1997-10-16	1997-10-28	1997-11-09	COLLECT COD              	MAIL      	otes haggle before the slyly bold instructi
1638	149	10	2	30	31474.20	0.00	0.04	N	O	1997-12-05	1997-09-17	1997-12-06	NONE                     	REG AIR   	s cajole boldly bold requests. closely 
1638	31	7	3	5	4655.15	0.08	0.07	N	O	1997-10-15	1997-11-01	1997-11-08	DELIVER IN PERSON        	FOB       	xcuses sleep furiou
1638	56	8	4	19	18164.95	0.00	0.08	N	O	1997-10-15	1997-10-27	1997-11-03	DELIVER IN PERSON        	MAIL      	 quickly expres
1638	143	6	5	25	26078.50	0.05	0.03	N	O	1997-10-06	1997-09-30	1997-11-02	DELIVER IN PERSON        	REG AIR   	gle final, ironic pinto beans. 
1638	155	10	6	46	48536.90	0.07	0.08	N	O	1997-08-20	1997-10-10	1997-09-09	COLLECT COD              	AIR       	ckages are carefully even instru
1639	187	8	1	24	26092.32	0.07	0.00	N	O	1995-08-24	1995-10-06	1995-08-31	COLLECT COD              	REG AIR   	 the regular packages. courts dou
1639	43	6	2	38	35835.52	0.01	0.04	N	O	1995-08-23	1995-11-09	1995-08-29	TAKE BACK RETURN         	FOB       	y regular packages. b
1639	171	10	3	41	43917.97	0.04	0.02	N	O	1995-12-19	1995-11-11	1996-01-12	DELIVER IN PERSON        	FOB       	structions w
1664	118	5	1	48	48869.28	0.04	0.02	N	O	1996-06-21	1996-05-01	1996-07-19	TAKE BACK RETURN         	RAIL      	 use. ironic deposits integrate. slyly unu
1664	173	2	2	30	32195.10	0.06	0.05	N	O	1996-04-04	1996-05-04	1996-05-03	COLLECT COD              	FOB       	ess multip
1664	151	2	3	10	10511.50	0.00	0.06	N	O	1996-04-10	1996-05-13	1996-05-07	TAKE BACK RETURN         	RAIL      	instructions up the acc
1664	155	3	4	35	36930.25	0.00	0.04	N	O	1996-03-06	1996-05-16	1996-03-09	DELIVER IN PERSON        	REG AIR   	y regular ide
1664	57	8	5	9	8613.45	0.07	0.04	N	O	1996-04-15	1996-05-14	1996-05-11	DELIVER IN PERSON        	TRUCK     	ges. fluffil
1664	141	8	6	40	41645.60	0.09	0.07	N	O	1996-04-02	1996-04-22	1996-04-17	COLLECT COD              	REG AIR   	se blithely unusual pains. carefully
1665	47	6	1	4	3788.16	0.02	0.03	A	F	1994-09-01	1994-06-07	1994-09-12	DELIVER IN PERSON        	TRUCK     	ely final requests. requests
1665	78	6	2	1	978.07	0.03	0.05	R	F	1994-05-22	1994-07-06	1994-05-24	TAKE BACK RETURN         	TRUCK     	sly final p
1667	21	4	1	6	5526.12	0.04	0.02	N	O	1997-12-07	1997-11-16	1998-01-02	COLLECT COD              	FOB       	riously busy requests. blithely final a
1667	22	1	2	29	26738.58	0.06	0.07	N	O	1997-10-15	1997-11-09	1997-11-11	TAKE BACK RETURN         	MAIL      	l accounts. furiously final courts h
1667	95	8	3	48	47764.32	0.05	0.01	N	O	1998-01-27	1998-01-06	1998-02-09	TAKE BACK RETURN         	SHIP      	tes sleep furiously. carefully eve
1667	59	1	4	24	23017.20	0.04	0.01	N	O	1997-10-14	1997-12-01	1997-11-09	TAKE BACK RETURN         	MAIL      	hrash final requests. care
1667	195	9	5	2	2190.38	0.07	0.00	N	O	1997-12-17	1997-11-22	1998-01-16	NONE                     	SHIP      	pecial requests hag
1667	48	7	6	6	5688.24	0.01	0.03	N	O	1998-01-21	1997-12-19	1998-01-28	NONE                     	TRUCK     	 nag quickly above th
1667	40	6	7	19	17860.76	0.09	0.03	N	O	1998-01-23	1997-11-24	1998-01-26	DELIVER IN PERSON        	SHIP      	around the pinto beans. express, special
1668	132	8	1	8	8257.04	0.06	0.01	N	O	1997-07-23	1997-10-09	1997-08-06	DELIVER IN PERSON        	FOB       	arefully regular tithes! slyl
1668	1	8	2	25	22525.00	0.01	0.06	N	O	1997-08-08	1997-09-28	1997-09-01	NONE                     	TRUCK     	y ironic requests. bold, final ideas a
1668	75	5	3	42	40952.94	0.08	0.01	N	O	1997-08-09	1997-09-08	1997-08-31	NONE                     	FOB       	ole carefully excuses. final
1668	191	5	4	9	9820.71	0.05	0.03	N	O	1997-10-17	1997-09-05	1997-11-01	COLLECT COD              	RAIL      	wake furiously even instructions. sil
1668	128	9	5	25	25703.00	0.01	0.02	N	O	1997-10-08	1997-09-20	1997-10-11	DELIVER IN PERSON        	REG AIR   	even platelets across the silent 
1668	10	3	6	38	34580.38	0.07	0.01	N	O	1997-08-26	1997-09-17	1997-09-05	DELIVER IN PERSON        	TRUCK     	ep slyly across the furi
1670	32	3	1	41	38213.23	0.07	0.01	N	O	1997-07-19	1997-08-20	1997-07-23	DELIVER IN PERSON        	TRUCK     	thely according to the sly
1670	122	3	2	10	10221.20	0.07	0.03	N	O	1997-09-14	1997-08-16	1997-09-23	NONE                     	SHIP      	fily special ideas 
1670	186	7	3	41	44533.38	0.07	0.07	N	O	1997-07-19	1997-08-05	1997-07-26	COLLECT COD              	SHIP      	al gifts. speci
1696	16	3	1	8	7328.08	0.04	0.02	N	O	1998-04-28	1998-02-07	1998-05-10	NONE                     	TRUCK     	the blithely
1696	139	5	2	13	13508.69	0.08	0.06	N	O	1998-03-01	1998-03-25	1998-03-24	TAKE BACK RETURN         	TRUCK     	tructions play slyly q
1696	2	5	3	19	17138.00	0.08	0.05	N	O	1998-05-03	1998-03-13	1998-05-28	TAKE BACK RETURN         	REG AIR   	its maintain alongside of the f
1696	193	4	4	21	22956.99	0.05	0.00	N	O	1998-05-04	1998-02-18	1998-05-07	NONE                     	MAIL      	y players sleep along the final, pending 
1696	94	7	5	43	42745.87	0.03	0.06	N	O	1998-02-14	1998-03-29	1998-02-20	COLLECT COD              	FOB       	arefully regular dep
1697	75	5	1	6	5850.42	0.05	0.00	N	O	1997-01-28	1996-11-27	1997-01-31	NONE                     	FOB       	accounts breach slyly even de
1697	104	7	2	24	24098.40	0.00	0.08	N	O	1996-12-29	1996-12-19	1997-01-10	NONE                     	SHIP      	ts cajole carefully above the carefully
1697	124	9	3	27	27651.24	0.06	0.00	N	O	1997-01-20	1996-12-02	1997-02-05	COLLECT COD              	MAIL      	ly regular packages across the silent, b
1697	94	5	4	49	48710.41	0.08	0.04	N	O	1996-12-07	1997-01-02	1996-12-31	COLLECT COD              	TRUCK     	lar foxes. fluffily furious ideas doubt qu
1697	35	1	5	19	17765.57	0.03	0.07	N	O	1997-01-08	1996-11-12	1997-01-11	DELIVER IN PERSON        	FOB       	ons? special, special accounts after
1698	97	8	1	44	43871.96	0.05	0.05	N	O	1997-05-16	1997-07-05	1997-05-27	NONE                     	RAIL      	ts wake slyly after t
1698	93	5	2	6	5958.54	0.08	0.00	N	O	1997-08-21	1997-06-08	1997-09-03	DELIVER IN PERSON        	RAIL      	 pending packages affix ne
1698	21	6	3	22	20262.44	0.03	0.04	N	O	1997-08-07	1997-05-28	1997-08-24	DELIVER IN PERSON        	TRUCK     	oward the furiously iro
1698	112	6	4	19	19230.09	0.00	0.07	N	O	1997-07-04	1997-06-21	1997-08-01	NONE                     	RAIL      	 fluffily e
1698	53	4	5	37	35262.85	0.00	0.03	N	O	1997-05-16	1997-05-29	1997-05-27	NONE                     	AIR       	ly regular ideas. deposit
1698	166	7	6	15	15992.40	0.10	0.01	N	O	1997-07-20	1997-06-07	1997-07-21	TAKE BACK RETURN         	RAIL      	final ideas. even, ironic 
1699	38	9	1	50	46901.50	0.00	0.06	A	F	1994-03-26	1994-03-23	1994-04-20	NONE                     	FOB       	to the final requests are carefully silent 
1699	135	6	2	17	17597.21	0.07	0.02	R	F	1994-01-12	1994-03-12	1994-02-08	NONE                     	AIR       	haggle blithely slyly
1701	150	9	1	47	49357.05	0.08	0.05	R	F	1992-05-25	1992-06-29	1992-06-15	NONE                     	RAIL      	slyly final requests cajole requests. f
1701	54	5	2	2	1908.10	0.01	0.04	R	F	1992-06-24	1992-07-12	1992-06-29	COLLECT COD              	SHIP      	ween the pending, final accounts. 
1701	35	1	3	26	24310.78	0.10	0.06	R	F	1992-06-04	1992-07-11	1992-07-04	DELIVER IN PERSON        	FOB       	 accounts. blithely pending pinto be
1702	67	2	1	19	18374.14	0.02	0.01	N	F	1995-06-02	1995-06-30	1995-06-29	NONE                     	REG AIR   	ies haggle blith
1702	30	5	2	38	35341.14	0.00	0.00	N	O	1995-09-01	1995-06-10	1995-09-10	DELIVER IN PERSON        	REG AIR   	as believe blithely. bo
1702	195	6	3	46	50378.74	0.00	0.08	N	O	1995-07-14	1995-06-30	1995-07-20	NONE                     	FOB       	y even foxes. carefully final dependencies 
1702	93	4	4	28	27806.52	0.07	0.05	R	F	1995-06-10	1995-07-26	1995-06-16	TAKE BACK RETURN         	AIR       	nts haggle along the packa
1702	89	10	5	34	33628.72	0.01	0.06	N	O	1995-07-04	1995-06-08	1995-07-28	DELIVER IN PERSON        	AIR       	y careful packages; dogged acco
1702	42	9	6	28	26377.12	0.10	0.00	N	O	1995-08-14	1995-07-31	1995-09-08	COLLECT COD              	RAIL      	ackages sleep. furiously even excuses snooz
1703	166	5	1	36	38381.76	0.09	0.01	R	F	1993-04-22	1993-03-05	1993-04-24	DELIVER IN PERSON        	SHIP      	riously express 
1703	137	8	2	35	36299.55	0.01	0.08	R	F	1993-04-14	1993-03-31	1993-04-27	NONE                     	RAIL      	he carefully
1703	124	5	3	48	49157.76	0.06	0.02	R	F	1993-02-07	1993-04-20	1993-02-24	TAKE BACK RETURN         	AIR       	ggle slyly furiously regular theodol
1728	126	5	1	1	1026.12	0.07	0.04	N	O	1996-09-16	1996-08-19	1996-09-18	COLLECT COD              	FOB       	lly. carefully ex
1728	105	8	2	23	23117.30	0.05	0.02	N	O	1996-09-08	1996-07-24	1996-09-20	NONE                     	FOB       	ns. pending, final ac
1728	165	10	3	44	46867.04	0.08	0.07	N	O	1996-07-31	1996-06-22	1996-08-06	COLLECT COD              	FOB       	ide of the slyly blithe
1728	27	8	4	34	31518.68	0.08	0.05	N	O	1996-08-28	1996-07-20	1996-09-12	DELIVER IN PERSON        	MAIL      	special req
1728	199	2	5	31	34074.89	0.09	0.02	N	O	1996-07-26	1996-06-28	1996-08-14	NONE                     	REG AIR   	kly sly theodolites.
1731	184	5	1	36	39030.48	0.10	0.00	N	O	1996-04-18	1996-04-03	1996-04-29	TAKE BACK RETURN         	MAIL      	ngside of the even instruct
1731	139	10	2	7	7273.91	0.04	0.07	N	O	1996-04-11	1996-02-13	1996-04-30	DELIVER IN PERSON        	REG AIR   	fily quick asymptotes
1731	51	9	3	50	47552.50	0.05	0.04	N	O	1996-01-14	1996-03-13	1996-01-29	COLLECT COD              	RAIL      	ly slyly speci
1731	196	10	4	23	25212.37	0.10	0.04	N	O	1996-04-22	1996-02-25	1996-05-16	TAKE BACK RETURN         	RAIL      	rays? bold, express pac
1731	53	4	5	37	35262.85	0.10	0.05	N	O	1996-04-30	1996-03-17	1996-05-27	TAKE BACK RETURN         	RAIL      	 beans use furiously slyly b
1731	124	7	6	41	41988.92	0.03	0.08	N	O	1996-04-05	1996-02-28	1996-05-01	TAKE BACK RETURN         	RAIL      	haggle across the blithely ironi
1732	5	6	1	50	45250.00	0.02	0.01	R	F	1993-12-05	1994-01-23	1993-12-20	TAKE BACK RETURN         	FOB       	fily final asymptotes according 
1732	99	10	2	36	35967.24	0.01	0.03	A	F	1994-03-15	1994-02-09	1994-04-02	DELIVER IN PERSON        	TRUCK     	ve the accounts. slowly ironic multip
1732	161	8	3	41	43507.56	0.00	0.04	R	F	1994-02-20	1994-01-07	1994-02-27	TAKE BACK RETURN         	AIR       	quests sublate against the silent 
1732	152	3	4	9	9469.35	0.04	0.04	A	F	1994-02-25	1994-01-29	1994-03-16	TAKE BACK RETURN         	FOB       	ular platelets. deposits wak
1732	169	8	5	25	26729.00	0.02	0.05	A	F	1994-02-15	1994-01-07	1994-02-21	COLLECT COD              	REG AIR   	nag slyly. even, special de
1732	73	1	6	16	15569.12	0.01	0.05	R	F	1994-01-07	1994-01-02	1994-01-25	COLLECT COD              	SHIP      	ix carefully at the furiously regular pac
1733	111	5	1	41	41455.51	0.08	0.01	N	O	1996-06-13	1996-07-08	1996-07-07	TAKE BACK RETURN         	AIR       	ess notornis. fur
1733	24	7	2	16	14784.32	0.00	0.04	N	O	1996-08-28	1996-07-25	1996-09-27	COLLECT COD              	MAIL      	slyly express deposits sleep abo
1733	120	10	3	29	29583.48	0.10	0.06	N	O	1996-07-16	1996-08-08	1996-07-28	NONE                     	TRUCK     	ns detect among the special accounts. qu
1733	136	7	4	38	39372.94	0.01	0.03	N	O	1996-08-26	1996-07-23	1996-08-28	NONE                     	FOB       	 deposits 
1733	34	5	5	22	20548.66	0.06	0.07	N	O	1996-07-16	1996-07-24	1996-07-30	COLLECT COD              	AIR       	gainst the final deposits. carefully final 
1733	66	7	6	9	8694.54	0.06	0.08	N	O	1996-05-25	1996-07-23	1996-06-10	COLLECT COD              	TRUCK     	ven foxes was according to t
1733	146	9	7	13	13599.82	0.02	0.03	N	O	1996-08-03	1996-08-02	1996-08-18	NONE                     	MAIL      	olites sleep furious
1735	156	7	1	43	45414.45	0.02	0.06	A	F	1993-01-14	1993-03-25	1993-02-02	DELIVER IN PERSON        	FOB       	iously after the 
1735	139	5	2	49	50917.37	0.03	0.04	A	F	1992-12-31	1993-02-03	1993-01-25	TAKE BACK RETURN         	TRUCK     	y express accounts above the exp
1760	96	9	1	38	37851.42	0.09	0.03	N	O	1996-06-15	1996-06-29	1996-07-11	NONE                     	MAIL      	tions. blithely regular orbits against the 
1760	8	9	2	3	2724.00	0.00	0.06	N	O	1996-07-18	1996-07-01	1996-08-01	NONE                     	RAIL      	lyly bold dolphins haggle carefully. sl
1760	137	8	3	44	45633.72	0.05	0.01	N	O	1996-06-11	1996-06-16	1996-07-02	COLLECT COD              	REG AIR   	instructions poach slyly ironic theodolites
1761	52	4	1	33	31417.65	0.09	0.03	R	F	1994-01-03	1994-01-23	1994-01-31	NONE                     	FOB       	s. excuses a
1761	52	3	2	37	35225.85	0.02	0.07	R	F	1994-02-17	1994-03-08	1994-03-16	NONE                     	RAIL      	 integrate. quickly unusual
1761	49	6	3	37	35114.48	0.06	0.04	R	F	1994-01-02	1994-03-12	1994-01-25	DELIVER IN PERSON        	TRUCK     	regular packages wake after
1761	73	1	4	49	47680.43	0.06	0.07	R	F	1994-01-08	1994-03-03	1994-02-05	TAKE BACK RETURN         	FOB       	y even packages promise
1761	157	5	5	37	39114.55	0.03	0.04	R	F	1994-04-24	1994-03-14	1994-04-29	TAKE BACK RETURN         	MAIL      	express requests print blithely around the
1761	24	7	6	12	11088.24	0.01	0.05	A	F	1994-04-16	1994-03-08	1994-04-21	DELIVER IN PERSON        	AIR       	 sleep furiously. deposits are acco
1761	1	6	7	13	11713.00	0.03	0.08	R	F	1994-03-06	1994-03-18	1994-03-22	DELIVER IN PERSON        	TRUCK     	ons boost fu
1764	121	2	1	20	20422.40	0.09	0.02	A	F	1992-06-09	1992-05-22	1992-07-06	COLLECT COD              	MAIL      	y quickly regular packages. car
1764	67	4	2	3	2901.18	0.07	0.07	R	F	1992-05-13	1992-06-07	1992-05-26	COLLECT COD              	RAIL      	es wake slowly. 
1764	78	6	3	27	26407.89	0.07	0.04	A	F	1992-05-06	1992-05-11	1992-05-23	COLLECT COD              	TRUCK     	ly final foxes wake blithely even requests
1765	161	2	1	36	38201.76	0.08	0.04	N	O	1996-03-02	1996-02-17	1996-03-14	DELIVER IN PERSON        	SHIP      	he blithely pending accou
1766	87	8	1	32	31586.56	0.08	0.01	N	O	1997-01-08	1996-11-11	1997-01-31	TAKE BACK RETURN         	AIR       	ess accounts. stealthily ironic accou
1766	34	10	2	12	11208.36	0.05	0.01	N	O	1996-10-28	1996-12-18	1996-11-15	DELIVER IN PERSON        	AIR       	heodolites above the final, regular acc
1766	111	1	3	1	1011.11	0.10	0.02	N	O	1997-01-21	1997-01-07	1997-02-19	NONE                     	TRUCK     	ly blithely pending accounts. reg
1767	25	4	1	32	29600.64	0.08	0.04	A	F	1995-05-22	1995-05-14	1995-05-23	COLLECT COD              	SHIP      	to the bravely ironic requests i
1767	42	1	2	1	942.04	0.09	0.05	N	O	1995-06-23	1995-05-25	1995-07-03	TAKE BACK RETURN         	RAIL      	ing to the slyly fin
1767	174	5	3	24	25780.08	0.06	0.03	R	F	1995-03-16	1995-04-29	1995-04-11	DELIVER IN PERSON        	RAIL      	luffy theodolites need to detect furi
1767	23	8	4	50	46151.00	0.01	0.02	R	F	1995-05-29	1995-04-14	1995-06-15	NONE                     	REG AIR   	y unusual foxe
1767	52	10	5	40	38082.00	0.06	0.00	R	F	1995-04-16	1995-05-06	1995-04-21	TAKE BACK RETURN         	AIR       	ep. accounts nag blithely fu
1792	88	9	1	9	8892.72	0.09	0.04	R	F	1994-02-28	1993-12-11	1994-03-12	TAKE BACK RETURN         	AIR       	final packages s
1792	9	6	2	5	4545.00	0.04	0.02	R	F	1994-02-13	1994-01-03	1994-02-28	DELIVER IN PERSON        	TRUCK     	ely regular accounts are slyly. pending, bo
1792	9	2	3	8	7272.00	0.01	0.04	A	F	1994-02-21	1994-01-26	1994-02-27	DELIVER IN PERSON        	RAIL      	nts. fluffily special instructions integr
1792	191	3	4	45	49103.55	0.00	0.01	A	F	1994-02-27	1993-12-24	1994-03-07	DELIVER IN PERSON        	MAIL      	ests are. ironic, regular asy
1792	199	2	5	35	38471.65	0.06	0.05	R	F	1994-01-31	1994-01-20	1994-02-17	NONE                     	FOB       	e against the quic
1794	168	9	1	36	38453.76	0.09	0.08	N	O	1997-11-07	1997-11-01	1997-11-18	TAKE BACK RETURN         	FOB       	ely fluffily ironi
1794	95	8	2	3	2985.27	0.02	0.03	N	O	1997-11-15	1997-12-16	1997-11-20	DELIVER IN PERSON        	FOB       	 sentiments according to the q
1794	117	8	3	23	23393.53	0.08	0.04	N	O	1997-10-13	1997-11-30	1997-10-28	TAKE BACK RETURN         	AIR       	usly unusual theodolites doze about 
1794	85	6	4	34	33492.72	0.06	0.08	N	O	1997-09-29	1997-11-13	1997-10-07	TAKE BACK RETURN         	SHIP      	rs above the accoun
1794	117	4	5	47	47804.17	0.10	0.06	N	O	1998-01-15	1997-11-30	1998-02-14	DELIVER IN PERSON        	TRUCK     	 haggle slyly. furiously express orbit
1794	91	3	6	37	36670.33	0.01	0.01	N	O	1998-01-12	1997-12-21	1998-01-17	DELIVER IN PERSON        	MAIL      	ackages. pinto
1795	137	8	1	44	45633.72	0.08	0.08	A	F	1994-04-28	1994-05-24	1994-05-27	NONE                     	AIR       	ites sleep carefully slyly p
1795	114	5	2	34	34479.74	0.08	0.00	A	F	1994-04-24	1994-06-01	1994-05-08	DELIVER IN PERSON        	SHIP      	closely regular instructions wake. 
1795	168	3	3	25	26704.00	0.07	0.01	A	F	1994-05-18	1994-05-22	1994-05-20	TAKE BACK RETURN         	RAIL      	he always express accounts ca
1795	125	8	4	32	32803.84	0.03	0.06	R	F	1994-05-10	1994-04-21	1994-05-17	DELIVER IN PERSON        	SHIP      	 asymptotes across the bold,
1795	163	8	5	11	11694.76	0.08	0.02	R	F	1994-06-19	1994-04-24	1994-07-02	TAKE BACK RETURN         	TRUCK     	slyly. special pa
1796	10	1	1	28	25480.28	0.08	0.04	A	F	1992-12-01	1993-01-01	1992-12-24	DELIVER IN PERSON        	FOB       	y quickly ironic accounts.
1796	185	6	2	8	8681.44	0.00	0.08	R	F	1993-01-07	1993-01-04	1993-01-10	NONE                     	SHIP      	slyly bold accounts are furiously agains
1798	109	10	1	43	43391.30	0.01	0.08	N	O	1997-08-27	1997-10-23	1997-09-09	DELIVER IN PERSON        	MAIL      	ld packages sleep furiously. depend
1824	120	10	1	45	45905.40	0.03	0.02	R	F	1994-08-21	1994-06-21	1994-09-19	NONE                     	RAIL      	ent Tiresias. quickly express 
1824	69	4	2	40	38762.40	0.10	0.03	A	F	1994-05-08	1994-07-24	1994-06-06	NONE                     	FOB       	es mold furiously final instructions. s
1825	156	1	1	43	45414.45	0.05	0.05	A	F	1994-02-18	1994-02-19	1994-03-02	TAKE BACK RETURN         	RAIL      	 accounts breach fluffily spe
1825	148	5	2	39	40877.46	0.00	0.00	R	F	1994-04-01	1994-01-12	1994-04-21	DELIVER IN PERSON        	REG AIR   	ual, bold ideas haggle above the quickly ir
1825	17	4	3	7	6419.07	0.04	0.03	A	F	1994-01-02	1994-01-30	1994-01-30	TAKE BACK RETURN         	REG AIR   	fully ironic requests. requests cajole ex
1825	121	10	4	23	23485.76	0.05	0.01	R	F	1994-01-08	1994-02-08	1994-01-19	NONE                     	MAIL      	 wake express, even r
1825	178	9	5	33	35579.61	0.04	0.04	A	F	1993-12-07	1994-03-01	1993-12-16	TAKE BACK RETURN         	RAIL      	about the ne
1827	90	1	1	47	46534.23	0.00	0.01	N	O	1996-08-01	1996-08-07	1996-08-23	TAKE BACK RETURN         	RAIL      	. pending courts about the even e
1827	154	9	2	48	50599.20	0.03	0.05	N	O	1996-08-28	1996-09-15	1996-09-01	COLLECT COD              	RAIL      	oxes. special, final asymptote
1827	200	1	3	37	40707.40	0.01	0.07	N	O	1996-07-20	1996-08-18	1996-08-08	DELIVER IN PERSON        	REG AIR   	ously ironic theodolites serve quickly af
1827	127	10	4	4	4108.48	0.04	0.04	N	O	1996-07-22	1996-09-10	1996-08-11	DELIVER IN PERSON        	RAIL      	special requests. blithely
1827	80	10	5	24	23521.92	0.00	0.08	N	O	1996-08-07	1996-09-01	1996-09-04	DELIVER IN PERSON        	SHIP      	al gifts! re
1827	21	2	6	7	6447.14	0.10	0.02	N	O	1996-08-28	1996-08-07	1996-08-31	DELIVER IN PERSON        	AIR       	egular foxes
1827	6	7	7	38	34428.00	0.05	0.01	N	O	1996-10-17	1996-08-29	1996-11-07	TAKE BACK RETURN         	SHIP      	 blithely. express, bo
1831	136	2	1	9	9325.17	0.02	0.03	A	F	1993-12-17	1994-01-27	1993-12-26	NONE                     	TRUCK     	mptotes. furiously regular dolphins al
1831	48	9	2	9	8532.36	0.07	0.06	R	F	1994-03-22	1994-01-07	1994-04-06	COLLECT COD              	MAIL      	ent deposits. regular saute
1831	115	5	3	17	17256.87	0.02	0.08	R	F	1994-01-18	1994-02-12	1994-01-30	TAKE BACK RETURN         	MAIL      	s boost ironic foxe
1831	95	8	4	23	22887.07	0.06	0.02	R	F	1993-12-21	1994-02-08	1994-01-04	NONE                     	SHIP      	ests. express pinto beans abou
1856	55	10	1	10	9550.50	0.05	0.07	R	F	1992-05-11	1992-05-20	1992-06-02	TAKE BACK RETURN         	FOB       	he furiously even theodolites. account
1856	97	10	2	47	46863.23	0.07	0.07	R	F	1992-03-22	1992-06-09	1992-04-17	DELIVER IN PERSON        	FOB       	ingly blithe theodolites. slyly pending 
1856	117	7	3	20	20342.20	0.04	0.06	R	F	1992-05-04	1992-05-06	1992-05-11	DELIVER IN PERSON        	MAIL      	ost carefully. slyly bold accounts
1856	150	1	4	22	23103.30	0.08	0.02	A	F	1992-05-02	1992-05-26	1992-05-20	TAKE BACK RETURN         	REG AIR   	platelets detect slyly regular packages. ca
1856	190	1	5	14	15262.66	0.01	0.01	A	F	1992-04-14	1992-05-02	1992-05-11	COLLECT COD              	SHIP      	ans are even requests. deposits caj
1856	23	6	6	36	33228.72	0.03	0.05	A	F	1992-06-19	1992-05-12	1992-06-28	TAKE BACK RETURN         	TRUCK     	ly even foxes kindle blithely even realm
1856	130	3	7	42	43265.46	0.04	0.00	R	F	1992-05-23	1992-06-06	1992-06-19	COLLECT COD              	RAIL      	usly final deposits
1860	113	4	1	9	9117.99	0.04	0.04	N	O	1996-08-03	1996-05-31	1996-08-04	DELIVER IN PERSON        	TRUCK     	c realms print carefully car
1863	63	2	1	48	46226.88	0.09	0.04	A	F	1993-10-10	1993-12-09	1993-10-19	NONE                     	FOB       	ans hinder furiou
1863	157	2	2	48	50743.20	0.04	0.08	A	F	1993-11-08	1993-11-05	1993-12-08	COLLECT COD              	AIR       	onic theodolites alongside of the pending a
1889	152	4	1	41	43138.15	0.10	0.02	N	O	1997-06-15	1997-05-10	1997-07-08	NONE                     	AIR       	s! furiously pending r
1889	172	3	2	13	13938.21	0.05	0.00	N	O	1997-06-12	1997-04-28	1997-06-23	NONE                     	REG AIR   	to the regular accounts. carefully express
1889	138	9	3	36	37372.68	0.05	0.07	N	O	1997-05-19	1997-06-14	1997-05-23	NONE                     	SHIP      	l pinto beans kindle 
1889	168	5	4	5	5340.80	0.02	0.07	N	O	1997-06-26	1997-06-09	1997-07-21	COLLECT COD              	AIR       	ording to the blithely silent r
1890	141	8	1	26	27069.64	0.03	0.07	N	O	1997-04-02	1997-03-13	1997-04-22	DELIVER IN PERSON        	FOB       	ngage. slyly ironic 
1890	100	1	2	43	43004.30	0.07	0.03	N	O	1996-12-30	1997-01-31	1997-01-19	DELIVER IN PERSON        	FOB       	p ironic, express accounts. fu
1890	59	1	3	24	23017.20	0.06	0.04	N	O	1997-02-09	1997-02-10	1997-02-12	COLLECT COD              	MAIL      	is wake carefully above the even id
1890	68	9	4	43	41626.58	0.09	0.04	N	O	1997-04-08	1997-02-19	1997-04-30	TAKE BACK RETURN         	FOB       	lyly. instructions across the furiously
1890	122	3	5	45	45995.40	0.08	0.05	N	O	1997-04-15	1997-03-16	1997-04-19	COLLECT COD              	FOB       	he carefully regular sauternes. ironic fret
1890	181	2	6	16	17298.88	0.08	0.02	N	O	1997-02-13	1997-02-18	1997-03-12	TAKE BACK RETURN         	TRUCK     	ged pinto beans. regular, regular id
1890	121	4	7	10	10211.20	0.01	0.04	N	O	1996-12-24	1997-02-19	1997-01-01	DELIVER IN PERSON        	AIR       	. even, unusual inst
1892	113	7	1	48	48629.28	0.02	0.01	A	F	1994-06-16	1994-06-16	1994-06-28	NONE                     	RAIL      	tornis detect regul
1892	43	2	2	35	33006.40	0.04	0.08	R	F	1994-04-05	1994-05-09	1994-05-03	NONE                     	MAIL      	hes nod furiously around the instruc
1892	134	5	3	37	38262.81	0.10	0.03	R	F	1994-04-11	1994-06-04	1994-04-24	TAKE BACK RETURN         	SHIP      	nts. slyly regular asymptot
1892	197	9	4	14	15360.66	0.06	0.07	R	F	1994-04-08	1994-06-12	1994-04-27	DELIVER IN PERSON        	FOB       	furiously about the furiously
1894	169	10	1	40	42766.40	0.03	0.07	R	F	1992-06-07	1992-05-11	1992-07-01	DELIVER IN PERSON        	FOB       	ily furiously bold packages. flu
1920	96	7	1	24	23906.16	0.04	0.05	N	O	1998-09-27	1998-08-23	1998-10-15	DELIVER IN PERSON        	AIR       	thely. bold, pend
1920	51	6	2	31	29482.55	0.05	0.06	N	O	1998-08-01	1998-08-30	1998-08-17	COLLECT COD              	SHIP      	lly. ideas wa
1920	18	2	3	6	5508.06	0.01	0.05	N	O	1998-10-01	1998-08-20	1998-10-24	COLLECT COD              	SHIP      	l ideas boost slyly pl
1920	84	5	4	50	49204.00	0.09	0.06	N	O	1998-10-03	1998-08-04	1998-10-29	DELIVER IN PERSON        	MAIL      	e blithely unusual foxes. brave packages
1920	34	10	5	14	13076.42	0.08	0.05	N	O	1998-10-22	1998-08-10	1998-10-27	DELIVER IN PERSON        	AIR       	ickly ironic d
1922	10	5	1	13	11830.13	0.05	0.03	N	O	1996-10-24	1996-09-21	1996-11-15	NONE                     	SHIP      	quests. furiously
1923	37	8	1	9	8433.27	0.01	0.08	N	O	1997-08-29	1997-09-13	1997-09-07	NONE                     	FOB       	lites. ironic instructions integrate bravel
1923	178	8	2	23	24797.91	0.07	0.05	N	O	1997-09-08	1997-08-11	1997-09-14	TAKE BACK RETURN         	MAIL      	aggle carefully. furiously permanent
1923	180	1	3	11	11881.98	0.03	0.03	N	O	1997-07-12	1997-09-04	1997-08-01	TAKE BACK RETURN         	REG AIR   	ages wake slyly about the furiously regular
1923	193	5	4	49	53566.31	0.06	0.05	N	O	1997-07-21	1997-08-08	1997-07-26	NONE                     	AIR       	de of the carefully expre
1923	184	5	5	25	27104.50	0.10	0.08	N	O	1997-08-18	1997-08-20	1997-09-12	DELIVER IN PERSON        	TRUCK     	the ideas: slyly pendin
1923	37	3	6	50	46851.50	0.03	0.03	N	O	1997-11-04	1997-08-08	1997-11-25	NONE                     	TRUCK     	uickly along the bold courts. bold the
1924	73	1	1	7	6811.49	0.06	0.07	N	O	1997-01-01	1996-12-02	1997-01-08	COLLECT COD              	SHIP      	osits. even accounts nag furious
1924	18	8	2	47	43146.47	0.02	0.06	N	O	1996-11-24	1996-10-18	1996-12-13	COLLECT COD              	REG AIR   	silent requests cajole blithely final pack
1924	57	8	3	40	38282.00	0.04	0.08	N	O	1996-10-31	1996-11-30	1996-11-21	NONE                     	REG AIR   	ains sleep carefully
1924	34	5	4	31	28954.93	0.03	0.03	N	O	1996-09-20	1996-10-19	1996-10-19	DELIVER IN PERSON        	SHIP      	 the slyly regular foxes. ruthle
1924	36	7	5	17	15912.51	0.04	0.05	N	O	1996-12-31	1996-11-12	1997-01-25	COLLECT COD              	TRUCK     	e carefully theodolites. ironically ironic 
1924	76	4	6	15	14641.05	0.02	0.04	N	O	1997-01-04	1996-11-13	1997-01-27	NONE                     	SHIP      	he package
1924	40	1	7	21	19740.84	0.09	0.03	N	O	1996-09-21	1996-11-12	1996-10-02	TAKE BACK RETURN         	AIR       	 blithely reg
1925	184	5	1	50	54209.00	0.01	0.02	R	F	1992-04-12	1992-04-23	1992-05-08	TAKE BACK RETURN         	TRUCK     	usual pinto
1925	135	1	2	35	36229.55	0.06	0.06	R	F	1992-05-11	1992-04-10	1992-05-14	TAKE BACK RETURN         	AIR       	counts. carefully ironic packages boost ab
1925	116	10	3	40	40644.40	0.08	0.08	A	F	1992-05-17	1992-05-20	1992-06-08	TAKE BACK RETURN         	AIR       	e carefully regul
1925	30	5	4	17	15810.51	0.06	0.02	R	F	1992-05-18	1992-04-06	1992-06-16	TAKE BACK RETURN         	MAIL      	instructions sleep. pinto bea
1926	51	9	1	24	22825.20	0.06	0.05	N	O	1996-05-04	1996-03-14	1996-06-01	DELIVER IN PERSON        	RAIL      	e theodolites.
1926	106	9	2	29	29176.90	0.09	0.08	N	O	1996-02-26	1996-03-14	1996-03-14	TAKE BACK RETURN         	TRUCK     	es. dependencies according to the fl
1926	178	6	3	10	10781.70	0.02	0.03	N	O	1996-05-23	1996-03-02	1996-06-04	NONE                     	AIR       	usly bold accounts. express accounts
1926	68	9	4	13	12584.78	0.04	0.02	N	O	1996-04-26	1996-04-13	1996-05-08	DELIVER IN PERSON        	MAIL      	eans wake bli
1926	40	1	5	29	27261.16	0.06	0.00	N	O	1996-02-29	1996-03-13	1996-03-24	DELIVER IN PERSON        	MAIL      	hily unusual packages are fluffily am
1927	68	5	1	3	2904.18	0.00	0.05	N	O	1995-10-06	1995-12-08	1995-11-05	COLLECT COD              	FOB       	ccounts affi
1927	73	2	2	15	14596.05	0.08	0.08	N	O	1995-12-25	1995-12-26	1995-12-31	COLLECT COD              	RAIL      	 carefully regular requests sleep car
1927	65	10	3	6	5790.36	0.05	0.05	N	O	1995-11-29	1995-11-20	1995-12-08	TAKE BACK RETURN         	TRUCK     	furiously even wat
1952	53	8	1	7	6671.35	0.04	0.05	A	F	1994-05-06	1994-06-11	1994-05-12	NONE                     	RAIL      	about the express, even requ
1952	142	5	2	6	6252.84	0.06	0.05	A	F	1994-05-09	1994-05-21	1994-05-26	DELIVER IN PERSON        	AIR       	packages haggle. 
1954	152	7	1	31	32616.65	0.06	0.06	N	O	1997-08-18	1997-07-07	1997-09-03	DELIVER IN PERSON        	RAIL      	against the packages. bold, ironic e
1954	182	3	2	1	1082.18	0.03	0.01	N	O	1997-09-16	1997-07-08	1997-10-07	COLLECT COD              	MAIL      	te. furiously final deposits hag
1954	199	2	3	11	12091.09	0.07	0.07	N	O	1997-08-07	1997-07-23	1997-08-25	DELIVER IN PERSON        	TRUCK     	y carefully ironi
1954	159	4	4	12	12709.80	0.02	0.08	N	O	1997-07-19	1997-07-04	1997-08-06	COLLECT COD              	AIR       	ongside of the slyly unusual requests. reg
1954	170	7	5	29	31034.93	0.08	0.08	N	O	1997-08-25	1997-07-15	1997-09-02	DELIVER IN PERSON        	RAIL      	use thinly furiously regular asy
1954	177	8	6	13	14003.21	0.00	0.07	N	O	1997-06-15	1997-08-22	1997-06-20	TAKE BACK RETURN         	MAIL      	y ironic instructions cajole
1954	194	5	7	49	53615.31	0.05	0.06	N	O	1997-06-04	1997-08-29	1997-06-14	COLLECT COD              	TRUCK     	eans. final pinto beans sleep furiousl
1955	137	3	1	32	33188.16	0.02	0.02	A	F	1992-07-05	1992-06-29	1992-08-03	TAKE BACK RETURN         	TRUCK     	g to the carefully sile
1955	18	8	2	2	1836.02	0.03	0.01	R	F	1992-07-06	1992-07-06	1992-08-01	COLLECT COD              	TRUCK     	ickly aroun
1955	158	6	3	41	43384.15	0.08	0.06	A	F	1992-08-01	1992-06-04	1992-08-07	COLLECT COD              	AIR       	 carefully against the furiously reg
1955	9	4	4	16	14544.00	0.03	0.07	A	F	1992-04-30	1992-06-23	1992-05-23	TAKE BACK RETURN         	FOB       	odolites eat s
1955	159	10	5	11	11650.65	0.09	0.01	A	F	1992-06-03	1992-07-04	1992-06-07	NONE                     	REG AIR   	ously quickly pendi
1984	53	5	1	45	42887.25	0.03	0.04	N	O	1998-04-09	1998-06-11	1998-05-01	COLLECT COD              	AIR       	p. quickly final ideas sle
1984	70	7	2	35	33952.45	0.01	0.07	N	O	1998-05-18	1998-05-04	1998-06-01	COLLECT COD              	RAIL      	tes. quickly pending packages haggle boldl
1990	101	2	1	46	46050.60	0.01	0.07	R	F	1994-12-29	1995-03-14	1995-01-13	NONE                     	TRUCK     	ar sentiments.
2017	103	4	1	49	49151.90	0.10	0.06	N	O	1998-05-26	1998-07-01	1998-06-06	COLLECT COD              	TRUCK     	 after the unusual instructions. sly
2017	71	2	2	14	13594.98	0.07	0.04	N	O	1998-06-28	1998-06-15	1998-07-11	NONE                     	TRUCK     	ily final w
2017	84	5	3	11	10824.88	0.05	0.02	N	O	1998-05-22	1998-07-13	1998-05-26	TAKE BACK RETURN         	TRUCK     	gside of the slyly dogged dolp
2019	4	9	1	31	28024.00	0.07	0.03	R	F	1992-11-18	1992-12-26	1992-11-24	DELIVER IN PERSON        	FOB       	l ideas across the slowl
2019	52	7	2	18	17136.90	0.04	0.03	R	F	1993-01-24	1992-12-22	1993-02-02	NONE                     	MAIL      	are carefully furiously regular requ
2020	34	10	1	50	46701.50	0.06	0.01	R	F	1993-07-12	1993-08-28	1993-08-02	COLLECT COD              	TRUCK     	ts against the pending ideas serve along
2020	176	4	2	40	43046.80	0.09	0.00	A	F	1993-10-17	1993-09-14	1993-10-29	TAKE BACK RETURN         	RAIL      	ently across the
2020	14	4	3	30	27420.30	0.07	0.04	A	F	1993-09-08	1993-08-11	1993-09-29	TAKE BACK RETURN         	AIR       	ly about the blithely ironic foxes. bold
2020	61	8	4	27	25948.62	0.05	0.06	A	F	1993-07-14	1993-09-02	1993-08-03	NONE                     	FOB       	e of the bold foxes haggle 
2022	169	8	1	38	40628.08	0.00	0.08	R	F	1992-07-05	1992-04-20	1992-07-13	TAKE BACK RETURN         	REG AIR   	 against the express accounts wake ca
2022	55	3	2	38	36291.90	0.05	0.04	R	F	1992-06-17	1992-05-15	1992-06-28	COLLECT COD              	SHIP      	instructions dazzle carefull
2022	49	10	3	48	45553.92	0.10	0.02	A	F	1992-06-14	1992-06-04	1992-07-12	DELIVER IN PERSON        	SHIP      	counts. slyly enticing accounts are during 
2022	182	3	4	16	17314.88	0.05	0.03	R	F	1992-06-23	1992-05-22	1992-07-07	NONE                     	TRUCK     	ages wake slyly care
2022	100	1	5	36	36003.60	0.05	0.02	R	F	1992-03-24	1992-05-07	1992-04-13	NONE                     	MAIL      	ly after the foxes. regular, final inst
2022	129	2	6	20	20582.40	0.08	0.08	A	F	1992-03-31	1992-04-17	1992-04-02	NONE                     	SHIP      	r deposits kindle 
2022	78	9	7	13	12714.91	0.06	0.08	R	F	1992-04-04	1992-05-30	1992-04-21	NONE                     	FOB       	 orbits haggle fluffily fl
2048	35	1	1	7	6545.21	0.06	0.01	R	F	1993-12-07	1994-01-31	1994-01-05	TAKE BACK RETURN         	REG AIR   	lent platelets boost deposits. carefully sp
2048	8	5	2	5	4540.00	0.04	0.04	A	F	1994-01-18	1994-02-01	1994-01-29	TAKE BACK RETURN         	TRUCK     	affix carefully against 
2048	101	2	3	12	12013.20	0.01	0.05	R	F	1994-01-28	1994-01-19	1994-02-08	NONE                     	AIR       	 even theodoli
2048	97	1	4	11	10967.99	0.10	0.03	R	F	1993-12-20	1994-01-19	1994-01-04	TAKE BACK RETURN         	MAIL      	totes. idly ironic packages nag
2050	73	2	1	47	45734.29	0.05	0.03	A	F	1994-08-25	1994-07-18	1994-09-15	DELIVER IN PERSON        	TRUCK     	tside the blithely pending packages eat f
2050	152	3	2	48	50503.20	0.05	0.01	A	F	1994-09-30	1994-08-23	1994-10-29	COLLECT COD              	AIR       	 final packages. pinto
2050	113	4	3	41	41537.51	0.10	0.04	A	F	1994-06-08	1994-08-27	1994-06-23	NONE                     	AIR       	 final theodolites. depende
2050	32	8	4	11	10252.33	0.02	0.01	A	F	1994-07-27	1994-08-18	1994-08-02	DELIVER IN PERSON        	REG AIR   	ns. bold, final ideas cajole among the fi
2050	168	9	5	16	17090.56	0.07	0.01	R	F	1994-08-17	1994-07-28	1994-09-05	DELIVER IN PERSON        	REG AIR   	al accounts. closely even 
2050	49	2	6	29	27522.16	0.00	0.05	A	F	1994-09-23	1994-08-01	1994-10-23	TAKE BACK RETURN         	MAIL      	oxes alongsid
2050	48	5	7	25	23701.00	0.10	0.00	R	F	1994-08-18	1994-07-04	1994-09-04	TAKE BACK RETURN         	RAIL      	y according to 
2051	25	6	1	43	39775.86	0.08	0.04	N	O	1996-04-22	1996-06-16	1996-04-28	COLLECT COD              	RAIL      	ounts sleep fluffily even requ
2051	130	1	2	48	49446.24	0.01	0.02	N	O	1996-05-04	1996-06-14	1996-05-19	NONE                     	TRUCK     	unts. pending platelets believe about
2052	68	7	1	50	48403.00	0.09	0.08	R	F	1992-06-22	1992-06-03	1992-07-19	DELIVER IN PERSON        	AIR       	wake after the decoy
2052	135	1	2	35	36229.55	0.09	0.05	A	F	1992-05-29	1992-05-24	1992-06-11	NONE                     	TRUCK     	ts according t
2052	43	2	3	16	15088.64	0.01	0.08	A	F	1992-06-30	1992-07-09	1992-07-12	NONE                     	SHIP      	y final deposits cajole according 
2052	96	7	4	47	46816.23	0.08	0.01	A	F	1992-06-18	1992-05-16	1992-07-02	TAKE BACK RETURN         	REG AIR   	final requests. stealt
2053	101	4	1	20	20022.00	0.09	0.00	A	F	1995-04-25	1995-04-12	1995-05-13	NONE                     	TRUCK     	ly ironic foxes haggle slyly speci
2053	33	4	2	34	31723.02	0.07	0.00	A	F	1995-03-15	1995-03-20	1995-04-09	TAKE BACK RETURN         	TRUCK     	ions. unusual dependencies
2053	65	2	3	46	44392.76	0.01	0.03	R	F	1995-04-01	1995-04-02	1995-04-18	NONE                     	RAIL      	tions. furiously even requests hagg
2053	121	6	4	31	31654.72	0.06	0.08	R	F	1995-03-23	1995-03-13	1995-04-16	DELIVER IN PERSON        	SHIP      	ts. fluffily final mul
2054	113	4	1	11	11144.21	0.03	0.05	R	F	1992-08-13	1992-08-26	1992-08-22	NONE                     	AIR       	ular accou
2054	120	7	2	31	31623.72	0.05	0.08	A	F	1992-08-18	1992-09-04	1992-08-24	NONE                     	FOB       	se bold, regular accounts. unusual depos
2054	121	2	3	32	32675.84	0.06	0.00	A	F	1992-06-23	1992-07-08	1992-07-22	NONE                     	FOB       	 packages thrash. carefully final
2054	174	3	4	14	15038.38	0.10	0.05	R	F	1992-06-25	1992-09-05	1992-07-14	DELIVER IN PERSON        	SHIP      	uickly final
2054	6	1	5	40	36240.00	0.08	0.06	R	F	1992-06-23	1992-08-09	1992-07-04	TAKE BACK RETURN         	RAIL      	n pinto beans. ironic courts are iro
2054	134	10	6	17	17580.21	0.08	0.01	A	F	1992-06-09	1992-08-28	1992-06-16	NONE                     	AIR       	ges nag acc
2054	11	1	7	4	3644.04	0.00	0.08	R	F	1992-08-12	1992-08-31	1992-08-15	DELIVER IN PERSON        	AIR       	lyly careful requests wake fl
2082	75	3	1	36	35102.52	0.00	0.00	R	F	1995-01-20	1995-03-18	1995-01-31	COLLECT COD              	MAIL      	haggle furiously silent pinto beans
2082	105	10	2	12	12061.20	0.08	0.05	A	F	1995-01-27	1995-02-11	1995-02-07	NONE                     	FOB       	 ironic instructions. carefull
2083	24	3	1	37	34188.74	0.07	0.00	R	F	1993-09-07	1993-09-30	1993-09-18	TAKE BACK RETURN         	MAIL      	ng the special foxes wake packages. f
2084	182	3	1	42	45451.56	0.03	0.05	A	F	1993-03-29	1993-05-05	1993-04-22	COLLECT COD              	REG AIR   	y fluffily even foxes. 
2084	180	10	2	23	24844.14	0.09	0.08	A	F	1993-06-05	1993-05-26	1993-06-06	DELIVER IN PERSON        	AIR       	es against 
2084	136	2	3	37	38336.81	0.07	0.05	A	F	1993-07-16	1993-04-20	1993-08-06	NONE                     	AIR       	y careful courts.
2084	94	8	4	9	8946.81	0.02	0.02	A	F	1993-03-18	1993-06-08	1993-03-30	NONE                     	TRUCK     	heaves boost slyly after the pla
2084	27	10	5	28	25956.56	0.07	0.02	R	F	1993-05-04	1993-05-14	1993-05-31	COLLECT COD              	TRUCK     	cajole quickly carefu
2084	115	9	6	15	15226.65	0.09	0.04	A	F	1993-06-23	1993-04-25	1993-07-23	COLLECT COD              	SHIP      	tithes. bravely pendi
2084	194	6	7	34	37202.46	0.09	0.02	R	F	1993-06-20	1993-05-28	1993-06-25	DELIVER IN PERSON        	RAIL      	 carefully ironic requests. fluffil
2085	41	8	1	45	42346.80	0.00	0.07	R	F	1994-02-27	1994-01-11	1994-03-29	TAKE BACK RETURN         	MAIL      	. carefully e
2086	60	1	1	22	21121.32	0.03	0.07	R	F	1994-12-04	1994-12-16	1994-12-20	DELIVER IN PERSON        	RAIL      	idly busy acc
2086	141	10	2	32	33316.48	0.04	0.06	A	F	1994-11-15	1995-01-05	1994-12-09	TAKE BACK RETURN         	TRUCK     	e carefully along th
2086	105	6	3	44	44224.40	0.02	0.01	A	F	1994-12-04	1994-11-30	1994-12-21	DELIVER IN PERSON        	FOB       	latelets s
2086	84	5	4	27	26570.16	0.02	0.00	A	F	1994-11-04	1995-01-14	1994-11-25	COLLECT COD              	REG AIR   	theodolites haggle blithely blithe p
2086	156	1	5	33	34852.95	0.04	0.00	A	F	1995-02-06	1994-11-25	1995-02-15	NONE                     	SHIP      	 slyly regular foxes. un
2086	200	3	6	20	22004.00	0.01	0.03	R	F	1994-11-30	1994-12-28	1994-12-07	COLLECT COD              	FOB       	lithely ironic acc
2086	156	8	7	7	7393.05	0.04	0.05	R	F	1994-12-27	1994-12-10	1995-01-05	COLLECT COD              	RAIL      	 beans haggle car
2087	127	8	1	1	1027.12	0.05	0.04	N	O	1998-03-27	1998-03-24	1998-04-18	DELIVER IN PERSON        	REG AIR   	the quickly idle acco
2087	168	3	2	46	49135.36	0.10	0.03	N	O	1998-02-24	1998-04-02	1998-03-04	DELIVER IN PERSON        	AIR       	ter the dolphins.
2087	62	3	3	1	962.06	0.02	0.05	N	O	1998-05-27	1998-04-11	1998-06-12	COLLECT COD              	REG AIR   	hely final acc
2087	59	1	4	6	5754.30	0.03	0.08	N	O	1998-04-23	1998-03-27	1998-05-18	DELIVER IN PERSON        	REG AIR   	dazzle after the slyly si
2112	71	2	1	18	17479.26	0.02	0.05	N	O	1997-05-02	1997-03-16	1997-05-25	TAKE BACK RETURN         	TRUCK     	lphins solve ideas. even, special reque
2115	196	8	1	27	29597.13	0.06	0.03	N	O	1998-09-01	1998-07-29	1998-09-04	NONE                     	AIR       	de of the carefully bold accounts 
2115	184	5	2	43	46619.74	0.06	0.02	N	O	1998-07-14	1998-07-25	1998-07-24	COLLECT COD              	FOB       	 carefully pending requests alongs
2115	51	3	3	3	2853.15	0.03	0.04	N	O	1998-07-23	1998-07-30	1998-08-14	DELIVER IN PERSON        	FOB       	quickly ironic dolphin
2115	49	10	4	47	44604.88	0.06	0.07	N	O	1998-08-29	1998-07-30	1998-09-05	TAKE BACK RETURN         	REG AIR   	regular accounts integrate brav
2115	199	3	5	13	14289.47	0.04	0.00	N	O	1998-08-07	1998-08-06	1998-08-13	DELIVER IN PERSON        	REG AIR   	into beans. even accounts abou
2116	131	2	1	2	2062.26	0.00	0.02	R	F	1994-10-16	1994-11-24	1994-11-09	DELIVER IN PERSON        	TRUCK     	r theodolites use blithely about the ir
2116	140	1	2	47	48886.58	0.10	0.06	R	F	1994-09-01	1994-11-18	1994-09-25	COLLECT COD              	MAIL      	iously ironic dependencies around the iro
2116	184	5	3	11	11925.98	0.03	0.05	R	F	1994-09-15	1994-10-21	1994-09-21	NONE                     	FOB       	 pinto beans. final, final sauternes play 
2117	165	2	1	36	38345.76	0.10	0.01	N	O	1997-08-06	1997-07-15	1997-08-07	DELIVER IN PERSON        	SHIP      	ronic accounts wake
2117	61	6	2	19	18260.14	0.04	0.00	N	O	1997-07-30	1997-06-18	1997-08-13	DELIVER IN PERSON        	REG AIR   	s between the slyly regula
2117	58	3	3	43	41196.15	0.04	0.03	N	O	1997-06-27	1997-06-12	1997-07-22	DELIVER IN PERSON        	SHIP      	 foxes sleep furiously 
2117	91	4	4	24	23786.16	0.00	0.07	N	O	1997-06-15	1997-05-27	1997-06-18	COLLECT COD              	SHIP      	thely slyly pending platelets. ironic, 
2117	147	8	5	3	3141.42	0.02	0.05	N	O	1997-05-05	1997-07-20	1997-05-26	TAKE BACK RETURN         	TRUCK     	tes cajole
2117	1	4	6	27	24327.00	0.09	0.08	N	O	1997-06-30	1997-06-27	1997-07-11	TAKE BACK RETURN         	REG AIR   	 the carefully ironic ideas
2118	160	1	1	24	25443.84	0.10	0.03	N	O	1997-01-06	1996-12-14	1997-01-14	TAKE BACK RETURN         	RAIL      	about the slyly bold depende
2118	184	5	2	4	4336.72	0.08	0.01	N	O	1996-10-25	1996-11-10	1996-11-22	COLLECT COD              	AIR       	theodolites affix according 
2118	145	4	3	11	11496.54	0.05	0.04	N	O	1996-12-23	1996-12-20	1997-01-01	COLLECT COD              	RAIL      	y ironic accounts sleep upon the packages. 
2119	102	7	1	36	36075.60	0.04	0.00	N	O	1996-11-10	1996-10-25	1996-12-03	TAKE BACK RETURN         	RAIL      	ly bold foxes. ironic accoun
2144	92	6	1	33	32738.97	0.00	0.07	R	F	1994-04-04	1994-06-20	1994-04-23	NONE                     	AIR       	 ironic excuses haggle final dependencies. 
2144	51	9	2	46	43748.30	0.03	0.08	R	F	1994-04-08	1994-04-29	1994-05-07	COLLECT COD              	SHIP      	 foxes haggle blithel
2144	4	9	3	29	26216.00	0.00	0.07	R	F	1994-05-03	1994-05-16	1994-06-01	DELIVER IN PERSON        	FOB       	ns wake carefully carefully ironic
2144	158	9	4	10	10581.50	0.00	0.04	R	F	1994-06-16	1994-05-03	1994-07-05	COLLECT COD              	AIR       	 furiously unusual ideas. carefull
2145	78	8	1	13	12714.91	0.04	0.05	A	F	1992-11-12	1992-12-13	1992-12-07	TAKE BACK RETURN         	MAIL      	alongside of the slyly final
2145	154	6	2	6	6324.90	0.05	0.01	A	F	1992-10-10	1992-11-29	1992-10-14	NONE                     	AIR       	s. fluffily express accounts sleep. slyl
2148	116	6	1	21	21338.31	0.09	0.01	R	F	1995-05-28	1995-05-26	1995-06-15	NONE                     	FOB       	deposits ag
2149	19	9	1	12	11028.12	0.05	0.07	R	F	1993-06-01	1993-05-06	1993-06-11	TAKE BACK RETURN         	TRUCK     	riously bl
2149	99	10	2	10	9990.90	0.06	0.01	R	F	1993-06-09	1993-04-17	1993-06-16	DELIVER IN PERSON        	TRUCK     	eposits sleep above
2149	49	2	3	47	44604.88	0.00	0.04	R	F	1993-06-27	1993-05-12	1993-07-11	COLLECT COD              	AIR       	hely final depo
2149	129	8	4	18	18524.16	0.06	0.00	A	F	1993-04-05	1993-05-11	1993-04-23	DELIVER IN PERSON        	REG AIR   	uriously final pac
2149	60	5	5	22	21121.32	0.06	0.04	R	F	1993-05-24	1993-04-23	1993-06-20	TAKE BACK RETURN         	SHIP      	ptotes sleep along the blithely ir
2176	191	4	1	38	41465.22	0.02	0.08	R	F	1992-11-29	1993-01-14	1992-12-22	DELIVER IN PERSON        	REG AIR   	lithely ironic pinto beans. furious
2176	95	8	2	14	13931.26	0.00	0.06	A	F	1992-11-17	1993-01-07	1992-12-03	DELIVER IN PERSON        	SHIP      	ely ironic platelets 
2176	160	1	3	25	26504.00	0.02	0.02	R	F	1993-02-23	1993-01-05	1993-03-07	COLLECT COD              	RAIL      	 ruthless deposits according to the ent
2176	143	6	4	2	2086.28	0.05	0.06	A	F	1993-02-26	1993-01-08	1993-03-23	DELIVER IN PERSON        	AIR       	s pinto beans
2177	129	10	1	45	46310.40	0.02	0.01	N	O	1997-02-11	1997-02-27	1997-02-17	NONE                     	SHIP      	. theodolites haggle carefu
2177	139	5	2	27	28056.51	0.04	0.08	N	O	1997-01-29	1997-03-20	1997-02-04	DELIVER IN PERSON        	SHIP      	even, regula
2177	81	2	3	23	22564.84	0.07	0.05	N	O	1997-01-28	1997-03-02	1997-02-13	DELIVER IN PERSON        	AIR       	he silent foxes. iro
2177	55	3	4	34	32471.70	0.05	0.07	N	O	1997-02-03	1997-04-10	1997-02-21	COLLECT COD              	REG AIR   	tes are doggedly quickly
2177	57	9	5	46	44024.30	0.09	0.05	N	O	1997-05-10	1997-02-23	1997-05-28	COLLECT COD              	RAIL      	ending asymptotes.
2177	122	7	6	11	11243.32	0.02	0.04	N	O	1997-03-20	1997-03-07	1997-04-09	DELIVER IN PERSON        	MAIL      	gainst the ca
2179	130	9	1	22	22662.86	0.05	0.08	N	O	1996-11-16	1996-11-03	1996-11-25	DELIVER IN PERSON        	FOB       	lphins cajole acr
2179	139	5	2	20	20782.60	0.03	0.01	N	O	1996-09-30	1996-11-10	1996-10-30	NONE                     	REG AIR   	ncies. fin
2179	104	9	3	5	5020.50	0.03	0.02	N	O	1996-11-09	1996-10-08	1996-11-11	DELIVER IN PERSON        	REG AIR   	ts haggle blithely. ironic, careful theodol
2179	6	3	4	24	21744.00	0.04	0.04	N	O	1996-10-26	1996-11-05	1996-11-16	COLLECT COD              	RAIL      	 cajole carefully. 
2179	108	5	5	7	7056.70	0.00	0.02	N	O	1996-10-24	1996-11-14	1996-11-21	TAKE BACK RETURN         	RAIL      	gular dependencies. ironic packages haggle
2180	16	3	1	31	28396.31	0.06	0.04	N	O	1996-10-20	1996-11-21	1996-11-06	COLLECT COD              	REG AIR   	n requests are furiously at the quickly
2180	193	7	2	39	42634.41	0.01	0.00	N	O	1997-01-03	1996-10-29	1997-01-25	NONE                     	RAIL      	ep furiously furiously final request
2180	197	9	3	24	26332.56	0.03	0.00	N	O	1997-01-03	1996-10-24	1997-01-19	NONE                     	SHIP      	uriously f
2180	111	5	4	47	47522.17	0.07	0.02	N	O	1996-09-23	1996-12-08	1996-10-12	NONE                     	FOB       	pending, regular ideas. iron
2180	143	2	5	23	23992.22	0.02	0.06	N	O	1996-11-08	1996-10-25	1996-11-28	NONE                     	TRUCK     	ggle alongside of the fluffily speci
2180	55	6	6	48	45842.40	0.09	0.03	N	O	1996-12-30	1996-11-22	1997-01-16	DELIVER IN PERSON        	RAIL      	nic instructions haggle careful
2181	178	9	1	4	4312.68	0.05	0.04	N	O	1995-09-25	1995-11-12	1995-09-28	COLLECT COD              	FOB       	tes. slyly silent packages use along th
2181	88	9	2	46	45451.68	0.00	0.02	N	O	1995-11-28	1995-10-17	1995-12-26	COLLECT COD              	AIR       	osits. final packages sleep
2181	91	2	3	15	14866.35	0.08	0.05	N	O	1995-10-05	1995-10-27	1995-11-03	DELIVER IN PERSON        	FOB       	e above the fluffily regul
2181	55	10	4	28	26741.40	0.04	0.05	N	O	1995-12-21	1995-10-23	1996-01-04	TAKE BACK RETURN         	AIR       	s excuses sleep car
2181	96	7	5	9	8964.81	0.06	0.07	N	O	1996-01-05	1995-12-05	1996-01-08	COLLECT COD              	TRUCK     	ward the quietly even requests. ir
2182	132	8	1	27	27867.51	0.02	0.07	R	F	1994-05-10	1994-07-04	1994-06-04	DELIVER IN PERSON        	SHIP      	en platele
2182	190	1	2	3	3270.57	0.05	0.03	R	F	1994-04-20	1994-07-04	1994-04-24	TAKE BACK RETURN         	SHIP      	y bold theodolites wi
2182	94	6	3	34	33799.06	0.02	0.03	R	F	1994-05-28	1994-06-02	1994-06-10	COLLECT COD              	MAIL      	 slow tithes. ironi
2182	7	4	4	12	10884.00	0.04	0.07	A	F	1994-05-08	1994-06-02	1994-05-09	COLLECT COD              	REG AIR   	ments are fu
2182	179	9	5	37	39929.29	0.06	0.02	A	F	1994-04-08	1994-06-29	1994-04-18	TAKE BACK RETURN         	TRUCK     	ges. blithely ironic
2183	71	1	1	29	28161.03	0.05	0.01	N	O	1996-07-21	1996-08-24	1996-08-15	TAKE BACK RETURN         	RAIL      	ly unusual deposits sleep carefully
2183	52	3	2	25	23801.25	0.06	0.02	N	O	1996-07-06	1996-08-21	1996-08-05	NONE                     	RAIL      	he quickly f
2208	58	3	1	48	45986.40	0.08	0.07	A	F	1995-05-13	1995-06-30	1995-05-20	COLLECT COD              	MAIL      	sits. idly permanent request
2208	97	1	2	11	10967.99	0.08	0.01	A	F	1995-05-06	1995-07-19	1995-05-22	COLLECT COD              	TRUCK     	ding waters lose. furiously regu
2208	74	4	3	41	39936.87	0.08	0.02	N	O	1995-08-18	1995-06-19	1995-09-05	COLLECT COD              	RAIL      	nd the furious, express dependencies.
2208	43	2	4	50	47152.00	0.07	0.07	N	F	1995-06-11	1995-05-31	1995-06-29	TAKE BACK RETURN         	FOB       	al foxes will hav
2208	30	5	5	43	39991.29	0.03	0.06	A	F	1995-05-10	1995-06-02	1995-06-09	TAKE BACK RETURN         	MAIL      	es. accounts cajole. fi
2208	167	2	6	18	19208.88	0.02	0.08	R	F	1995-06-06	1995-06-10	1995-06-11	TAKE BACK RETURN         	TRUCK     	packages are quickly bold de
2208	7	2	7	45	40815.00	0.00	0.08	A	F	1995-05-05	1995-06-10	1995-05-11	NONE                     	SHIP      	e fluffily regular theodolites caj
2209	23	2	1	40	36920.80	0.05	0.01	R	F	1992-11-01	1992-09-25	1992-11-08	DELIVER IN PERSON        	SHIP      	ully special sheaves serve
2209	103	4	2	10	10031.00	0.00	0.02	R	F	1992-09-02	1992-09-24	1992-09-21	DELIVER IN PERSON        	AIR       	players. carefully reg
2209	64	1	3	11	10604.66	0.01	0.01	A	F	1992-07-12	1992-08-24	1992-08-10	DELIVER IN PERSON        	REG AIR   	express, regular pinto be
2209	181	2	4	39	42166.02	0.08	0.07	R	F	1992-11-04	1992-09-02	1992-11-11	TAKE BACK RETURN         	MAIL      	ly around the final packages. deposits ca
2209	124	7	5	24	24578.88	0.08	0.06	R	F	1992-08-09	1992-08-18	1992-08-25	COLLECT COD              	AIR       	 along the bol
2209	178	7	6	7	7547.19	0.09	0.07	A	F	1992-08-18	1992-09-09	1992-09-12	DELIVER IN PERSON        	AIR       	 quickly regular pack
2211	48	1	1	25	23701.00	0.04	0.01	A	F	1994-10-09	1994-08-04	1994-11-03	TAKE BACK RETURN         	RAIL      	deas. carefully special theodolites along
2211	140	6	2	40	41605.60	0.09	0.06	A	F	1994-09-30	1994-09-10	1994-10-26	NONE                     	MAIL      	posits among the express dolphins
2211	160	2	3	25	26504.00	0.00	0.07	A	F	1994-08-13	1994-08-17	1994-08-16	NONE                     	AIR       	ly regular, express
2211	85	6	4	23	22656.84	0.03	0.02	R	F	1994-10-05	1994-09-13	1994-10-17	DELIVER IN PERSON        	AIR       	ependencies 
2211	135	1	5	3	3105.39	0.02	0.04	A	F	1994-08-28	1994-09-10	1994-09-06	TAKE BACK RETURN         	SHIP      	pendencies after the regular f
2211	187	8	6	18	19569.24	0.05	0.08	A	F	1994-08-31	1994-09-07	1994-09-22	NONE                     	TRUCK     	c grouches. slyly express pinto 
2211	79	9	7	3	2937.21	0.06	0.05	R	F	1994-09-21	1994-08-10	1994-10-19	TAKE BACK RETURN         	RAIL      	y slyly final
2213	118	8	1	20	20362.20	0.01	0.00	A	F	1993-01-21	1993-04-14	1993-01-29	COLLECT COD              	REG AIR   	iously express accounts; 
2213	60	1	2	4	3840.24	0.09	0.05	R	F	1993-04-15	1993-04-15	1993-05-05	COLLECT COD              	SHIP      	 affix carefully furiously 
2213	70	5	3	1	970.07	0.05	0.05	A	F	1993-04-25	1993-04-06	1993-04-28	TAKE BACK RETURN         	AIR       	s along the ironic reques
2213	174	3	4	39	41892.63	0.09	0.05	A	F	1993-05-12	1993-04-07	1993-05-23	TAKE BACK RETURN         	SHIP      	the blithely 
2213	38	9	5	43	40335.29	0.04	0.03	A	F	1993-04-18	1993-03-11	1993-05-11	TAKE BACK RETURN         	RAIL      	r packages are along the carefully bol
2213	48	5	6	41	38869.64	0.01	0.00	R	F	1993-01-31	1993-03-31	1993-02-28	COLLECT COD              	FOB       	 carefully pend
2213	64	9	7	3	2892.18	0.02	0.04	A	F	1993-03-09	1993-03-17	1993-04-07	TAKE BACK RETURN         	AIR       	o wake. ironic platel
2214	76	5	1	27	26353.89	0.04	0.04	N	O	1998-05-31	1998-06-07	1998-06-19	DELIVER IN PERSON        	REG AIR   	x fluffily along the even packages-- 
2214	194	5	2	50	54709.50	0.00	0.02	N	O	1998-07-06	1998-06-16	1998-07-16	TAKE BACK RETURN         	MAIL      	accounts. blith
2214	113	7	3	42	42550.62	0.04	0.08	N	O	1998-05-26	1998-07-13	1998-06-22	COLLECT COD              	FOB       	ons. deposi
2214	196	9	4	22	24116.18	0.01	0.01	N	O	1998-05-30	1998-07-02	1998-06-09	DELIVER IN PERSON        	RAIL      	t the blithely
2215	73	1	1	33	32111.31	0.00	0.00	N	O	1996-07-19	1996-08-10	1996-07-30	COLLECT COD              	RAIL      	dolites cajole b
2215	33	9	2	30	27990.90	0.01	0.00	N	O	1996-08-15	1996-09-10	1996-08-25	NONE                     	FOB       	ckages caj
2215	57	5	3	30	28711.50	0.07	0.03	N	O	1996-09-09	1996-07-20	1996-09-28	COLLECT COD              	TRUCK     	against the carefu
2215	146	3	4	20	20922.80	0.02	0.02	N	O	1996-09-09	1996-08-10	1996-09-19	NONE                     	MAIL      	 unusual deposits haggle carefully. ide
2240	164	3	1	6	6384.96	0.01	0.00	A	F	1992-06-23	1992-05-17	1992-07-20	COLLECT COD              	AIR       	ymptotes boost. furiously bold p
2240	28	1	2	37	34336.74	0.03	0.07	R	F	1992-03-16	1992-05-31	1992-04-05	COLLECT COD              	FOB       	 quickly after the packages? blithely si
2240	53	5	3	39	37168.95	0.08	0.06	A	F	1992-05-22	1992-05-10	1992-06-08	NONE                     	FOB       	y orbits. final depos
2240	86	7	4	10	9860.80	0.09	0.00	A	F	1992-05-25	1992-04-14	1992-06-23	DELIVER IN PERSON        	REG AIR   	are across the ironic packages.
2240	161	10	5	29	30773.64	0.02	0.06	A	F	1992-03-29	1992-05-08	1992-04-09	COLLECT COD              	MAIL      	lyly even ideas w
2240	81	2	6	32	31394.56	0.06	0.06	R	F	1992-04-11	1992-04-18	1992-04-22	NONE                     	MAIL      	ss thinly deposits. blithely bold package
2240	78	7	7	24	23473.68	0.04	0.05	R	F	1992-05-13	1992-04-09	1992-05-14	DELIVER IN PERSON        	FOB       	ng the silent accounts. slyly ironic t
2241	5	6	1	25	22625.00	0.00	0.08	R	F	1993-08-11	1993-07-23	1993-09-01	DELIVER IN PERSON        	MAIL      	 final deposits use fluffily. even f
2241	195	8	2	38	41617.22	0.04	0.06	A	F	1993-08-04	1993-07-31	1993-08-06	TAKE BACK RETURN         	TRUCK     	 silent, unusual d
2241	97	10	3	48	47860.32	0.08	0.04	A	F	1993-05-14	1993-07-30	1993-05-26	TAKE BACK RETURN         	RAIL      	ss accounts engage furiously. slyly even re
2241	167	4	4	19	20276.04	0.10	0.00	A	F	1993-06-01	1993-08-05	1993-06-07	TAKE BACK RETURN         	TRUCK     	 are furiously quickl
2241	82	3	5	2	1964.16	0.04	0.03	A	F	1993-08-16	1993-08-02	1993-08-24	NONE                     	REG AIR   	, express deposits. pear
2241	116	3	6	22	22354.42	0.02	0.08	R	F	1993-08-13	1993-06-15	1993-08-16	DELIVER IN PERSON        	TRUCK     	, ironic depen
2241	142	3	7	9	9379.26	0.09	0.03	A	F	1993-05-14	1993-07-12	1993-05-29	NONE                     	AIR       	lyly final 
2243	127	8	1	10	10271.20	0.04	0.06	N	O	1995-07-26	1995-07-18	1995-08-03	NONE                     	RAIL      	express, daring foxes affix fur
2246	53	4	1	22	20967.10	0.02	0.01	N	O	1996-07-25	1996-08-03	1996-08-24	DELIVER IN PERSON        	SHIP      	ructions wake carefully fina
2246	104	5	2	43	43176.30	0.07	0.06	N	O	1996-08-25	1996-08-23	1996-09-19	DELIVER IN PERSON        	AIR       	ainst the ironic theodolites haggle fi
2246	18	8	3	11	10098.11	0.10	0.00	N	O	1996-06-21	1996-07-24	1996-07-18	TAKE BACK RETURN         	TRUCK     	quests alongside o
2246	163	8	4	13	13821.08	0.08	0.05	N	O	1996-09-15	1996-07-21	1996-10-08	DELIVER IN PERSON        	AIR       	equests. fluffily special epitaphs use
2272	90	1	1	18	17821.62	0.04	0.00	R	F	1993-08-01	1993-07-06	1993-08-25	NONE                     	MAIL      	ons along the blithely e
2272	34	10	2	40	37361.20	0.07	0.00	A	F	1993-04-25	1993-07-12	1993-05-15	DELIVER IN PERSON        	FOB       	lithely ir
2272	56	4	3	36	34417.80	0.03	0.02	A	F	1993-05-25	1993-05-23	1993-06-09	TAKE BACK RETURN         	RAIL      	about the ironic packages; quickly iron
2272	138	4	4	30	31143.90	0.09	0.07	A	F	1993-07-27	1993-05-15	1993-08-13	NONE                     	RAIL      	quests at the foxes haggle evenly pack
2272	76	4	5	12	11712.84	0.03	0.03	A	F	1993-04-19	1993-05-14	1993-04-23	NONE                     	RAIL      	 accounts cajole. quickly b
2273	184	5	1	34	36862.12	0.02	0.03	N	O	1997-01-08	1997-02-02	1997-01-23	COLLECT COD              	MAIL      	 furiously carefully bold de
2273	85	6	2	35	34477.80	0.00	0.05	N	O	1997-01-02	1997-01-19	1997-01-14	NONE                     	REG AIR   	arefully f
2273	95	8	3	8	7960.72	0.00	0.04	N	O	1996-12-15	1997-02-27	1997-01-10	NONE                     	FOB       	dependencies. slyly ir
2273	161	6	4	20	21223.20	0.06	0.04	N	O	1997-03-05	1997-02-25	1997-04-01	NONE                     	RAIL      	cuses. quickly enticing requests wake 
2273	162	7	5	18	19118.88	0.07	0.00	N	O	1996-12-16	1997-01-21	1997-01-03	COLLECT COD              	TRUCK     	 beans. doggedly final packages wake
2273	155	7	6	16	16882.40	0.10	0.03	N	O	1997-01-10	1997-02-03	1997-02-01	TAKE BACK RETURN         	RAIL      	furiously above the ironic requests. 
2273	20	1	7	7	6440.14	0.05	0.05	N	O	1997-02-19	1997-01-22	1997-02-21	TAKE BACK RETURN         	TRUCK     	ts. furiou
2274	12	6	1	18	16416.18	0.04	0.03	R	F	1993-09-06	1993-12-03	1993-09-22	COLLECT COD              	SHIP      	usly final re
2274	111	8	2	23	23255.53	0.04	0.03	R	F	1993-10-28	1993-11-03	1993-11-05	NONE                     	MAIL      	kly special warhorse
2274	129	10	3	18	18524.16	0.03	0.06	R	F	1993-09-28	1993-11-22	1993-10-12	DELIVER IN PERSON        	SHIP      	 express packages. even accounts hagg
2278	45	2	1	36	34021.44	0.04	0.05	N	O	1998-06-04	1998-06-06	1998-06-30	NONE                     	TRUCK     	y ironic pinto beans br
2278	45	2	2	50	47252.00	0.02	0.00	N	O	1998-08-09	1998-07-08	1998-09-05	DELIVER IN PERSON        	RAIL      	into beans. blit
2278	97	9	3	22	21935.98	0.03	0.00	N	O	1998-05-15	1998-07-14	1998-06-04	TAKE BACK RETURN         	REG AIR   	ep regular accounts. blithely even
2279	14	5	1	12	10968.12	0.07	0.08	A	F	1993-05-10	1993-03-25	1993-06-02	COLLECT COD              	REG AIR   	lets across the excuses nag quickl
2279	41	2	2	38	35759.52	0.08	0.07	R	F	1993-06-09	1993-04-06	1993-06-26	COLLECT COD              	TRUCK     	s above the furiously express dep
2279	4	7	3	3	2712.00	0.09	0.04	A	F	1993-05-31	1993-05-07	1993-06-05	COLLECT COD              	REG AIR   	ing foxes above the even accounts use slyly
2279	52	4	4	42	39986.10	0.02	0.00	R	F	1993-02-28	1993-04-25	1993-03-02	TAKE BACK RETURN         	REG AIR   	 above the furiously ironic deposits. 
2279	169	8	5	9	9622.44	0.05	0.04	R	F	1993-05-21	1993-03-29	1993-06-17	DELIVER IN PERSON        	MAIL      	ns cajole after the final platelets. s
2279	147	10	6	12	12565.68	0.02	0.00	R	F	1993-05-04	1993-04-26	1993-05-28	DELIVER IN PERSON        	FOB       	ccounts. slyl
2279	119	9	7	32	32611.52	0.05	0.05	A	F	1993-04-20	1993-05-22	1993-05-18	DELIVER IN PERSON        	RAIL      	re quickly. furiously ironic ide
2304	200	2	1	42	46208.40	0.00	0.01	A	F	1994-01-20	1994-03-04	1994-02-05	COLLECT COD              	RAIL      	quests are blithely alongside of
2304	19	9	2	48	44112.48	0.00	0.00	R	F	1994-02-12	1994-02-16	1994-03-10	COLLECT COD              	REG AIR   	 deposits cajole blithely e
2304	48	9	3	3	2844.12	0.00	0.05	R	F	1994-03-19	1994-03-04	1994-03-20	DELIVER IN PERSON        	AIR       	l excuses after the ev
2306	196	9	1	50	54809.50	0.09	0.01	N	O	1995-07-27	1995-09-26	1995-08-06	DELIVER IN PERSON        	FOB       	y quickly 
2306	149	2	2	39	40916.46	0.04	0.00	N	O	1995-09-07	1995-09-13	1995-10-03	COLLECT COD              	SHIP      	f the slyly unusual accounts. furiousl
2306	178	6	3	35	37735.95	0.01	0.07	N	O	1995-08-18	1995-08-30	1995-08-20	TAKE BACK RETURN         	RAIL      	raids along the furiously unusual asympto
2306	119	3	4	21	21401.31	0.06	0.01	N	O	1995-10-07	1995-09-18	1995-10-17	COLLECT COD              	MAIL      	 ironic pinto 
2306	142	9	5	42	43769.88	0.04	0.07	N	O	1995-09-05	1995-08-25	1995-09-28	COLLECT COD              	MAIL      	furiously final acco
2306	124	5	6	29	29699.48	0.00	0.03	N	O	1995-11-01	1995-09-01	1995-11-22	DELIVER IN PERSON        	REG AIR   	uld have to mold. s
2306	176	4	7	19	20447.23	0.07	0.01	N	O	1995-11-17	1995-09-06	1995-11-30	DELIVER IN PERSON        	AIR       	tainments nag furiously carefull
2307	142	9	1	24	25011.36	0.10	0.05	R	F	1993-10-07	1993-08-05	1993-10-20	COLLECT COD              	AIR       	stealthily special packages nag a
2307	140	6	2	2	2080.28	0.01	0.00	A	F	1993-09-21	1993-08-22	1993-10-03	COLLECT COD              	SHIP      	ously. furiously furious requ
2307	34	10	3	7	6538.21	0.07	0.04	R	F	1993-08-03	1993-09-04	1993-08-28	DELIVER IN PERSON        	AIR       	ven instructions wake fluffily 
2307	165	6	4	19	20238.04	0.08	0.06	R	F	1993-10-23	1993-09-09	1993-11-09	TAKE BACK RETURN         	TRUCK     	olites haggle furiously around the 
2307	143	4	5	7	7301.98	0.01	0.06	R	F	1993-09-01	1993-08-08	1993-09-29	NONE                     	AIR       	 packages cajo
2308	118	9	1	24	24434.64	0.06	0.04	R	F	1993-02-23	1992-12-24	1993-03-10	NONE                     	MAIL      	ts sleep. busy excuses along the s
2308	56	1	2	36	34417.80	0.05	0.06	A	F	1992-11-11	1992-11-27	1992-11-23	NONE                     	MAIL      	ong the pending hockey players. blithe
2311	141	8	1	18	18740.52	0.01	0.01	N	F	1995-06-11	1995-06-18	1995-07-02	NONE                     	FOB       	 fluffily even patterns haggle blithely. re
2311	122	1	2	49	50083.88	0.09	0.02	R	F	1995-05-14	1995-07-11	1995-05-20	COLLECT COD              	FOB       	ideas sleep
2311	54	5	3	15	14310.75	0.08	0.04	N	O	1995-06-23	1995-06-06	1995-07-09	COLLECT COD              	AIR       	ve the blithely pending accounts. furio
2311	90	1	4	42	41583.78	0.01	0.06	R	F	1995-06-03	1995-06-27	1995-06-11	DELIVER IN PERSON        	MAIL      	gle furiously. bold 
2311	47	10	5	1	947.04	0.05	0.02	A	F	1995-06-07	1995-06-20	1995-06-10	NONE                     	AIR       	ptotes. furiously regular theodolite
2311	12	9	6	32	29184.32	0.01	0.03	N	O	1995-07-19	1995-06-26	1995-07-26	NONE                     	RAIL      	sts along the slyly
2336	193	5	1	20	21863.80	0.01	0.03	N	O	1996-03-12	1996-02-25	1996-03-18	NONE                     	REG AIR   	across the fi
2337	45	2	1	49	46306.96	0.06	0.05	N	O	1997-08-08	1997-08-15	1997-08-31	TAKE BACK RETURN         	FOB       	 along the packages. furiously p
2338	52	7	1	30	28561.50	0.07	0.06	N	O	1997-12-10	1997-10-15	1997-12-11	TAKE BACK RETURN         	REG AIR   	ould have to nag quickly
2342	42	1	1	12	11304.48	0.00	0.08	N	O	1996-07-31	1996-07-26	1996-08-14	NONE                     	TRUCK     	print blithely even deposits. carefull
2342	117	1	2	24	24410.64	0.10	0.06	N	O	1996-09-30	1996-07-22	1996-10-28	TAKE BACK RETURN         	AIR       	nstructions c
2342	170	1	3	50	53508.50	0.10	0.01	N	O	1996-08-28	1996-07-18	1996-09-22	COLLECT COD              	RAIL      	cial asymptotes pr
2342	36	7	4	1	936.03	0.04	0.06	N	O	1996-08-31	1996-08-09	1996-09-07	DELIVER IN PERSON        	REG AIR   	ffily. unusual pinto beans wake c
2342	27	2	5	22	20394.44	0.08	0.01	N	O	1996-08-10	1996-08-02	1996-08-31	DELIVER IN PERSON        	AIR       	s. ironic 
2343	110	1	1	27	27272.97	0.00	0.00	N	O	1995-11-10	1995-11-17	1995-12-10	TAKE BACK RETURN         	SHIP      	old theodolites.
2343	66	1	2	35	33812.10	0.03	0.06	N	O	1995-10-24	1995-11-09	1995-10-26	COLLECT COD              	TRUCK     	ges haggle furiously carefully regular req
2343	179	7	3	21	22662.57	0.00	0.03	N	O	1995-09-07	1995-10-26	1995-10-07	TAKE BACK RETURN         	RAIL      	osits. unusual theodolites boost furio
2368	152	3	1	16	16834.40	0.04	0.03	R	F	1993-10-31	1993-10-22	1993-11-06	NONE                     	REG AIR   	telets wake carefully iro
2368	14	5	2	32	29248.32	0.03	0.00	R	F	1993-09-23	1993-10-07	1993-09-27	COLLECT COD              	TRUCK     	gular courts use blithely around the
2368	149	6	3	39	40916.46	0.08	0.03	R	F	1993-09-03	1993-09-20	1993-09-28	COLLECT COD              	RAIL      	ng the doggedly ironic requests are blithe
2368	156	8	4	17	17954.55	0.10	0.08	A	F	1993-10-03	1993-09-27	1993-10-05	NONE                     	FOB       	fily. slyly final ideas alongside o
2369	24	7	1	30	27720.60	0.05	0.04	N	O	1997-04-23	1997-02-12	1997-05-21	COLLECT COD              	REG AIR   	pecial deposits sleep. blithely unusual w
2369	169	10	2	47	50250.52	0.10	0.02	N	O	1997-01-02	1997-02-18	1997-01-13	COLLECT COD              	RAIL      	 to the regular dep
2370	46	3	1	3	2838.12	0.03	0.07	R	F	1994-03-24	1994-03-26	1994-04-15	COLLECT COD              	SHIP      	ly regular Tiresia
2370	2	5	2	24	21648.00	0.00	0.05	A	F	1994-05-15	1994-04-09	1994-06-12	NONE                     	REG AIR   	final depen
2370	61	2	3	32	30753.92	0.05	0.02	A	F	1994-04-24	1994-03-03	1994-05-15	DELIVER IN PERSON        	MAIL      	ies since the final deposits
2370	6	3	4	21	19026.00	0.04	0.01	R	F	1994-02-01	1994-02-19	1994-02-09	TAKE BACK RETURN         	MAIL      	ecial dependencies must have to 
2373	191	5	1	17	18550.23	0.02	0.01	R	F	1994-03-29	1994-05-19	1994-04-20	COLLECT COD              	AIR       	auternes. blithely even pinto bea
2373	136	2	2	3	3108.39	0.08	0.08	R	F	1994-05-15	1994-06-10	1994-06-04	COLLECT COD              	TRUCK     	dependencies wake ironical
2373	141	8	3	29	30193.06	0.05	0.02	A	F	1994-06-01	1994-05-14	1994-06-17	NONE                     	TRUCK     	yly silent ideas affix furiousl
2373	91	5	4	5	4955.45	0.10	0.01	R	F	1994-06-02	1994-05-03	1994-06-21	NONE                     	REG AIR   	uffily blithely ironic requests
2374	118	2	1	41	41742.51	0.07	0.00	A	F	1994-01-27	1993-12-11	1994-02-12	TAKE BACK RETURN         	RAIL      	heodolites. requests
2374	160	2	2	24	25443.84	0.07	0.08	A	F	1994-02-02	1994-01-12	1994-02-04	DELIVER IN PERSON        	TRUCK     	. requests are above t
2374	61	8	3	2	1922.12	0.06	0.02	R	F	1993-12-30	1994-01-24	1994-01-02	COLLECT COD              	FOB       	, unusual ideas. deposits cajole quietl
2374	74	5	4	28	27273.96	0.04	0.08	R	F	1994-02-19	1993-12-16	1994-03-15	COLLECT COD              	MAIL      	ets cajole fu
2374	1	2	5	25	22525.00	0.08	0.00	A	F	1993-11-26	1993-12-15	1993-12-10	COLLECT COD              	RAIL      	refully pending d
2375	168	9	1	3	3204.48	0.02	0.08	N	O	1997-02-14	1996-12-25	1997-02-22	COLLECT COD              	RAIL      	slyly across the furiously e
2375	132	8	2	9	9289.17	0.09	0.02	N	O	1997-02-17	1996-12-27	1997-02-27	DELIVER IN PERSON        	MAIL      	ly against the packages. bold pinto bean
2375	47	4	3	26	24623.04	0.02	0.06	N	O	1997-03-18	1997-02-02	1997-03-29	TAKE BACK RETURN         	TRUCK     	rate across the
2375	5	8	4	5	4525.00	0.01	0.00	N	O	1997-01-31	1997-01-25	1997-02-22	COLLECT COD              	REG AIR   	final packages cajole according to the furi
2375	88	9	5	42	41499.36	0.01	0.08	N	O	1997-01-24	1997-02-15	1997-02-07	DELIVER IN PERSON        	FOB       	apades. idea
2375	126	7	6	20	20522.40	0.09	0.08	N	O	1996-12-01	1996-12-26	1996-12-19	TAKE BACK RETURN         	SHIP      	ckages! blithely enticing deposi
2400	103	6	1	48	48148.80	0.01	0.02	N	O	1998-10-07	1998-08-30	1998-11-03	DELIVER IN PERSON        	MAIL      	fore the car
2400	90	1	2	1	990.09	0.04	0.07	N	O	1998-08-18	1998-09-12	1998-09-11	NONE                     	MAIL      	silent deposits serve furious
2400	53	5	3	23	21920.15	0.02	0.08	N	O	1998-08-05	1998-08-28	1998-08-30	NONE                     	SHIP      	tions. fluffily ironic platelets cajole c
2400	17	7	4	23	21091.23	0.09	0.04	N	O	1998-10-04	1998-10-04	1998-10-31	NONE                     	RAIL      	ages lose carefully around the regula
2401	182	3	1	39	42205.02	0.00	0.03	N	O	1997-09-29	1997-10-21	1997-10-17	DELIVER IN PERSON        	FOB       	ould affix 
2401	3	8	2	49	44247.00	0.05	0.07	N	O	1997-09-02	1997-09-11	1997-09-13	TAKE BACK RETURN         	AIR       	lites cajole carefully 
2402	86	7	1	43	42401.44	0.03	0.08	N	O	1996-09-17	1996-11-20	1996-09-22	DELIVER IN PERSON        	RAIL      	slyly slyly blithe sheaves
2402	152	4	2	24	25251.60	0.02	0.05	N	O	1996-11-21	1996-10-19	1996-11-29	DELIVER IN PERSON        	SHIP      	as; blithely ironic requ
2403	83	4	1	34	33424.72	0.04	0.07	N	O	1998-05-30	1998-06-19	1998-06-05	NONE                     	REG AIR   	 slyly bold re
2403	152	4	2	19	19990.85	0.08	0.07	N	O	1998-04-20	1998-07-02	1998-05-13	DELIVER IN PERSON        	FOB       	sits. ironic in
2403	193	4	3	27	29516.13	0.05	0.03	N	O	1998-07-27	1998-07-08	1998-08-03	NONE                     	SHIP      	deposits sleep slyly special theodolit
2403	31	2	4	30	27930.90	0.05	0.06	N	O	1998-08-08	1998-06-17	1998-08-20	NONE                     	TRUCK     	ackages sleep furiously pendin
2405	89	10	1	18	17803.44	0.09	0.07	N	O	1997-01-23	1997-03-10	1997-02-03	COLLECT COD              	REG AIR   	carefully ironic accounts. slyly 
2405	27	10	2	30	27810.60	0.10	0.08	N	O	1997-03-24	1997-03-10	1997-04-14	TAKE BACK RETURN         	AIR       	y final deposits are slyly caref
2405	17	8	3	49	44933.49	0.00	0.06	N	O	1996-12-24	1997-03-23	1997-01-01	TAKE BACK RETURN         	FOB       	cial requests. ironic, regu
2405	177	7	4	23	24774.91	0.08	0.05	N	O	1996-12-28	1997-01-29	1997-01-07	NONE                     	AIR       	t wake blithely blithely regular idea
2407	64	3	1	14	13496.84	0.04	0.02	N	O	1998-10-10	1998-08-25	1998-10-27	NONE                     	FOB       	l dependencies s
2407	166	7	2	9	9595.44	0.07	0.05	N	O	1998-08-06	1998-08-11	1998-08-20	TAKE BACK RETURN         	TRUCK     	ts. special deposits are closely.
2407	131	2	3	39	40214.07	0.02	0.02	N	O	1998-08-20	1998-09-12	1998-08-22	DELIVER IN PERSON        	MAIL      	iously final deposits solv
2407	91	4	4	10	9910.90	0.01	0.07	N	O	1998-08-14	1998-09-10	1998-08-29	COLLECT COD              	FOB       	 pending instructions. theodolites x-
2407	198	1	5	14	15374.66	0.04	0.05	N	O	1998-09-24	1998-08-18	1998-10-06	DELIVER IN PERSON        	FOB       	tructions wake stealt
2407	71	9	6	18	17479.26	0.04	0.01	N	O	1998-10-03	1998-08-30	1998-10-19	TAKE BACK RETURN         	MAIL      	 wake carefully. fluffily 
2407	161	8	7	7	7428.12	0.07	0.03	N	O	1998-09-11	1998-08-15	1998-09-30	TAKE BACK RETURN         	MAIL      	totes are carefully accordin
2432	50	3	1	30	28501.50	0.03	0.02	N	O	1996-09-05	1996-10-10	1996-10-05	TAKE BACK RETURN         	TRUCK     	 requests wake alongside of
2432	162	3	2	8	8497.28	0.07	0.01	N	O	1996-10-16	1996-10-01	1996-11-13	COLLECT COD              	RAIL      	s about the bold, close deposit
2432	109	2	3	13	13118.30	0.07	0.06	N	O	1996-09-03	1996-10-10	1996-10-03	NONE                     	RAIL      	arefully about the caref
2432	13	4	4	14	12782.14	0.00	0.06	N	O	1996-08-18	1996-09-04	1996-08-27	TAKE BACK RETURN         	RAIL      	riously regular packages. p
2434	95	6	1	1	995.09	0.01	0.06	N	O	1997-08-02	1997-05-28	1997-08-19	TAKE BACK RETURN         	MAIL      	 furiously express packages. ironic, pend
2434	127	10	2	39	40057.68	0.09	0.05	N	O	1997-06-10	1997-06-08	1997-07-03	COLLECT COD              	RAIL      	r deposits sleep furiou
2434	130	3	3	28	28843.64	0.02	0.05	N	O	1997-06-28	1997-06-26	1997-07-15	COLLECT COD              	RAIL      	ven theodolites around the slyly
2434	168	9	4	49	52339.84	0.00	0.05	N	O	1997-08-08	1997-07-23	1997-08-27	DELIVER IN PERSON        	FOB       	 after the requests haggle bold, fina
2435	39	10	1	8	7512.24	0.08	0.03	A	F	1993-06-08	1993-04-04	1993-06-29	COLLECT COD              	SHIP      	e fluffily quickly final accounts. care
2435	49	2	2	43	40808.72	0.03	0.08	A	F	1993-03-27	1993-05-20	1993-04-18	DELIVER IN PERSON        	TRUCK     	alongside of the s
2435	12	9	3	24	21888.24	0.07	0.08	R	F	1993-03-14	1993-05-20	1993-03-26	DELIVER IN PERSON        	SHIP      	s. carefully regular d
2435	156	4	4	22	23235.30	0.02	0.05	R	F	1993-05-23	1993-04-14	1993-06-04	NONE                     	SHIP      	e final, final deposits. carefully regular
2435	72	2	5	3	2916.21	0.07	0.07	R	F	1993-06-01	1993-03-25	1993-06-27	DELIVER IN PERSON        	FOB       	 final accounts ar
2435	46	9	6	17	16082.68	0.02	0.02	A	F	1993-06-05	1993-05-05	1993-06-14	NONE                     	TRUCK     	cajole aft
2435	121	10	7	8	8168.96	0.07	0.02	R	F	1993-05-03	1993-04-02	1993-05-17	COLLECT COD              	SHIP      	ng the fluffily special foxes nag 
2437	94	6	1	46	45728.14	0.07	0.04	A	F	1993-08-12	1993-06-16	1993-08-29	NONE                     	RAIL      	e of the bold, dogged requests
2437	190	1	2	26	28344.94	0.00	0.04	A	F	1993-06-25	1993-05-22	1993-07-07	DELIVER IN PERSON        	REG AIR   	lyly regular accounts.
2437	2	7	3	23	20746.00	0.01	0.00	A	F	1993-08-15	1993-06-28	1993-08-23	TAKE BACK RETURN         	SHIP      	s deposits. pendi
2437	116	10	4	12	12193.32	0.03	0.08	A	F	1993-04-27	1993-07-01	1993-05-18	TAKE BACK RETURN         	FOB       	thely regular deposits. ironic fray
2437	17	7	5	29	26593.29	0.02	0.06	A	F	1993-05-12	1993-06-10	1993-05-25	NONE                     	FOB       	ress dolphins. furiously fin
2437	19	3	6	10	9190.10	0.10	0.06	A	F	1993-05-20	1993-06-23	1993-05-22	TAKE BACK RETURN         	MAIL      	unts. even, ironic pl
2438	165	2	1	45	47932.20	0.01	0.00	A	F	1993-10-27	1993-09-24	1993-11-02	COLLECT COD              	REG AIR   	en theodolites w
2438	13	4	2	31	28303.31	0.08	0.01	R	F	1993-10-16	1993-08-31	1993-11-10	COLLECT COD              	REG AIR   	t. slyly ironic sh
2438	68	7	3	10	9680.60	0.10	0.00	R	F	1993-08-18	1993-08-28	1993-09-08	NONE                     	SHIP      	engage car
2438	161	8	4	27	28651.32	0.01	0.02	R	F	1993-07-27	1993-10-01	1993-08-06	TAKE BACK RETURN         	FOB       	inal accounts. slyly final reques
2438	166	3	5	28	29852.48	0.07	0.06	R	F	1993-11-05	1993-08-22	1993-11-22	TAKE BACK RETURN         	TRUCK     	ctions. bli
2438	149	6	6	23	24130.22	0.09	0.02	R	F	1993-10-06	1993-08-17	1993-10-16	DELIVER IN PERSON        	MAIL      	ely; blithely special pinto beans breach
2438	183	4	7	46	49826.28	0.02	0.05	R	F	1993-10-27	1993-08-30	1993-11-14	COLLECT COD              	SHIP      	 ironic requests cajole f
2439	164	1	1	2	2128.32	0.09	0.03	N	O	1997-04-14	1997-06-11	1997-05-09	COLLECT COD              	MAIL      	courts boos
2439	144	5	2	5	5220.70	0.07	0.01	N	O	1997-04-23	1997-04-26	1997-04-28	DELIVER IN PERSON        	FOB       	ites. furiously
2439	195	7	3	33	36141.27	0.08	0.05	N	O	1997-06-01	1997-05-15	1997-06-07	TAKE BACK RETURN         	FOB       	asymptotes wake packages-- furiously
2496	141	8	1	38	39563.32	0.02	0.07	R	F	1994-03-26	1994-04-06	1994-04-23	COLLECT COD              	RAIL      	 bold accounts. furi
2496	23	4	2	39	35997.78	0.03	0.00	R	F	1994-03-23	1994-02-18	1994-04-10	TAKE BACK RETURN         	FOB       	arefully special dependencies abo
2496	189	10	3	36	39210.48	0.09	0.04	R	F	1994-03-27	1994-03-15	1994-04-17	TAKE BACK RETURN         	SHIP      	ully ironic f
2496	24	9	4	30	27720.60	0.04	0.01	A	F	1994-01-27	1994-03-11	1994-01-31	DELIVER IN PERSON        	RAIL      	ake. ironic foxes cajole quickly. fu
2497	12	2	1	34	31008.34	0.02	0.03	R	F	1992-09-02	1992-10-19	1992-09-12	COLLECT COD              	AIR       	ronic accounts. p
2497	77	7	2	15	14656.05	0.09	0.02	A	F	1992-12-23	1992-11-20	1993-01-18	DELIVER IN PERSON        	SHIP      	sly against the
2497	34	5	3	28	26152.84	0.02	0.08	A	F	1992-12-02	1992-11-21	1992-12-04	DELIVER IN PERSON        	REG AIR   	ouches. special, regular requests
2497	144	5	4	48	50118.72	0.06	0.05	A	F	1992-09-29	1992-11-13	1992-10-19	TAKE BACK RETURN         	AIR       	 even, regular requests across 
2497	175	5	5	28	30104.76	0.04	0.05	A	F	1992-11-10	1992-09-30	1992-11-18	DELIVER IN PERSON        	MAIL      	hely bold ideas. unusual instructions ac
2497	71	2	6	19	18450.33	0.05	0.08	A	F	1992-11-10	1992-11-20	1992-12-05	TAKE BACK RETURN         	TRUCK     	 instructions? carefully daring accounts
2501	84	5	1	4	3936.32	0.10	0.06	N	O	1997-07-17	1997-07-27	1997-07-22	COLLECT COD              	RAIL      	quests. furiously final
2501	106	1	2	33	33201.30	0.01	0.04	N	O	1997-07-14	1997-08-09	1997-07-26	NONE                     	MAIL      	leep furiously packages. even sauternes 
2501	72	2	3	20	19441.40	0.10	0.06	N	O	1997-09-23	1997-07-01	1997-10-03	DELIVER IN PERSON        	RAIL      	equests. furiou
2501	58	10	4	26	24909.30	0.09	0.01	N	O	1997-07-15	1997-08-15	1997-07-28	DELIVER IN PERSON        	SHIP      	c accounts. express, iron
2528	1	2	1	10	9010.00	0.02	0.03	R	F	1994-12-12	1994-12-29	1994-12-28	COLLECT COD              	REG AIR   	ely. fluffily even re
2528	74	3	2	13	12662.91	0.00	0.03	A	F	1994-11-27	1995-01-20	1994-12-03	TAKE BACK RETURN         	REG AIR   	ggle furiously. slyly final asympt
2528	175	6	3	35	37630.95	0.10	0.00	R	F	1994-12-19	1995-02-04	1995-01-15	NONE                     	MAIL      	, even excuses. even,
2528	65	4	4	37	35707.22	0.00	0.01	A	F	1994-12-25	1995-02-02	1994-12-31	COLLECT COD              	AIR       	ng the pending excuses haggle after the bl
2529	131	7	1	4	4124.52	0.07	0.07	N	O	1996-10-19	1996-11-18	1996-10-24	DELIVER IN PERSON        	SHIP      	al dependencies haggle slyly alongsi
2530	21	2	1	9	8289.18	0.09	0.03	R	F	1994-05-10	1994-04-30	1994-05-24	TAKE BACK RETURN         	REG AIR   	lyly ironic
2530	93	7	2	42	41709.78	0.04	0.08	R	F	1994-03-27	1994-05-20	1994-03-29	NONE                     	RAIL      	ng platelets wake s
2530	108	1	3	8	8064.80	0.10	0.08	A	F	1994-05-02	1994-05-08	1994-05-24	DELIVER IN PERSON        	MAIL      	ial asymptotes snooze slyly regular 
2532	53	4	1	3	2859.15	0.06	0.07	N	O	1995-12-14	1995-11-28	1995-12-15	COLLECT COD              	FOB       	unusual sentiments. even pinto
2532	160	2	2	33	34985.28	0.06	0.05	N	O	1995-11-23	1996-01-04	1995-12-16	DELIVER IN PERSON        	TRUCK     	rve carefully slyly ironic accounts! fluf
2532	135	1	3	1	1035.13	0.00	0.06	N	O	1996-01-27	1995-11-23	1996-01-29	DELIVER IN PERSON        	REG AIR   	ely final ideas cajole despite the ca
2532	78	8	4	50	48903.50	0.02	0.02	N	O	1995-11-13	1996-01-01	1995-11-26	NONE                     	TRUCK     	yly after the fluffily regul
2532	114	1	5	9	9126.99	0.09	0.04	N	O	1995-11-30	1995-11-23	1995-12-12	DELIVER IN PERSON        	TRUCK     	cial ideas haggle slyly pending request
2532	150	1	6	20	21003.00	0.09	0.05	N	O	1995-12-02	1995-11-26	1995-12-08	TAKE BACK RETURN         	AIR       	er the slyly pending
2533	54	9	1	36	34345.80	0.06	0.04	N	O	1997-06-10	1997-04-28	1997-07-01	NONE                     	REG AIR   	ss requests sleep neve
2533	198	10	2	5	5490.95	0.10	0.04	N	O	1997-05-26	1997-06-02	1997-06-24	NONE                     	FOB       	ccounts. ironic, special accounts boo
2533	183	4	3	37	40077.66	0.00	0.08	N	O	1997-05-10	1997-04-26	1997-05-28	COLLECT COD              	SHIP      	 haggle carefully 
2533	30	5	4	17	15810.51	0.06	0.02	N	O	1997-05-23	1997-05-10	1997-06-18	NONE                     	FOB       	ackages. blith
2533	126	1	5	38	38992.56	0.09	0.00	N	O	1997-05-10	1997-06-02	1997-05-28	TAKE BACK RETURN         	REG AIR   	of the regular accounts. even packages caj
2533	184	5	6	20	21683.60	0.05	0.08	N	O	1997-07-04	1997-04-30	1997-07-05	COLLECT COD              	FOB       	thless excuses are b
2533	94	7	7	14	13917.26	0.06	0.04	N	O	1997-07-06	1997-05-08	1997-08-03	COLLECT COD              	FOB       	ut the pending, special depos
2534	139	5	1	29	30134.77	0.07	0.07	N	O	1996-08-09	1996-09-29	1996-08-11	COLLECT COD              	TRUCK     	ugouts haggle slyly. final
2534	27	6	2	49	45423.98	0.08	0.08	N	O	1996-09-01	1996-08-20	1996-09-06	NONE                     	SHIP      	sometimes regular requests. blithely unus
2534	1	4	3	50	45050.00	0.10	0.06	N	O	1996-09-25	1996-10-07	1996-10-09	TAKE BACK RETURN         	AIR       	ideas. deposits use. slyly regular pa
2534	75	3	4	43	41928.01	0.09	0.02	N	O	1996-10-25	1996-09-30	1996-11-05	TAKE BACK RETURN         	REG AIR   	ngly final depos
2534	165	2	5	14	14912.24	0.05	0.02	N	O	1996-08-12	1996-09-26	1996-08-28	COLLECT COD              	MAIL      	eposits doze quickly final
2534	116	10	6	12	12193.32	0.02	0.02	N	O	1996-07-29	1996-10-12	1996-08-14	TAKE BACK RETURN         	AIR       	sual depos
2534	173	3	7	17	18243.89	0.02	0.07	N	O	1996-07-22	1996-09-15	1996-08-03	NONE                     	SHIP      	riously regular 
2560	169	10	1	41	43835.56	0.07	0.01	R	F	1992-10-23	1992-11-11	1992-11-22	NONE                     	SHIP      	 after the accounts. regular foxes are be
2560	4	9	2	27	24408.00	0.00	0.01	R	F	1992-12-03	1992-11-16	1992-12-30	NONE                     	MAIL      	 against the carefully
2560	46	5	3	31	29327.24	0.01	0.05	A	F	1992-11-14	1992-10-14	1992-12-11	DELIVER IN PERSON        	AIR       	to beans. blithely regular Tiresias int
2560	72	1	4	36	34994.52	0.01	0.02	A	F	1992-10-18	1992-10-30	1992-11-05	TAKE BACK RETURN         	MAIL      	accounts alongside of the excuses are 
2560	42	1	5	9	8478.36	0.04	0.02	A	F	1992-10-23	1992-10-29	1992-11-02	COLLECT COD              	REG AIR   	 deposits affix quickly. unusual, eve
2560	108	9	6	13	13105.30	0.03	0.06	A	F	1992-09-07	1992-10-21	1992-09-24	COLLECT COD              	FOB       	slyly final accoun
2562	53	5	1	28	26685.40	0.04	0.03	R	F	1992-10-04	1992-09-24	1992-10-09	COLLECT COD              	MAIL      	ans haggle special, special packages. 
2562	148	9	2	1	1048.14	0.01	0.06	R	F	1992-10-16	1992-09-18	1992-10-17	NONE                     	TRUCK     	 slyly final ideas haggle car
2562	66	7	3	25	24151.50	0.05	0.03	A	F	1992-11-23	1992-10-08	1992-12-19	DELIVER IN PERSON        	REG AIR   	 accounts-- silent, unusual ideas a
2562	148	1	4	37	38781.18	0.08	0.03	R	F	1992-10-29	1992-10-06	1992-11-09	COLLECT COD              	FOB       	. slyly regular ideas according to the fl
2562	160	8	5	29	30744.64	0.05	0.08	A	F	1992-11-01	1992-09-29	1992-11-13	TAKE BACK RETURN         	MAIL      	eep against the furiously r
2562	50	7	6	17	16150.85	0.01	0.06	A	F	1992-10-15	1992-10-08	1992-10-26	DELIVER IN PERSON        	TRUCK     	lar pinto beans. blithely ev
2563	65	4	1	10	9650.60	0.07	0.04	A	F	1994-01-26	1993-12-19	1994-01-28	DELIVER IN PERSON        	AIR       	tealthily abo
2563	167	4	2	28	29880.48	0.04	0.03	R	F	1994-03-17	1994-02-04	1994-04-13	TAKE BACK RETURN         	RAIL      	hely regular depe
2563	119	9	3	39	39745.29	0.07	0.00	R	F	1994-02-10	1993-12-31	1994-02-19	COLLECT COD              	FOB       	lent requests should integrate; carefully e
2563	90	1	4	50	49504.50	0.01	0.01	A	F	1994-01-26	1994-01-03	1994-02-09	DELIVER IN PERSON        	SHIP      	ly regular, regular excuses. bold plate
2563	15	6	5	42	38430.42	0.06	0.08	R	F	1994-02-21	1994-02-14	1994-03-04	DELIVER IN PERSON        	AIR       	ymptotes nag furiously slyly even inst
2563	121	2	6	5	5105.60	0.10	0.00	R	F	1993-12-27	1993-12-19	1994-01-02	DELIVER IN PERSON        	REG AIR   	 the quickly final theodolite
2565	144	5	1	42	43853.88	0.04	0.08	N	O	1998-04-07	1998-04-02	1998-05-04	NONE                     	AIR       	ngly silent 
2565	189	10	2	26	28318.68	0.05	0.08	N	O	1998-05-07	1998-04-09	1998-05-15	DELIVER IN PERSON        	TRUCK     	 pinto beans about the slyly regula
2565	115	5	3	34	34513.74	0.06	0.06	N	O	1998-03-19	1998-04-12	1998-04-17	DELIVER IN PERSON        	SHIP      	nstructions was carefu
2565	17	7	4	25	22925.25	0.10	0.08	N	O	1998-06-27	1998-05-20	1998-07-13	DELIVER IN PERSON        	RAIL      	, express accounts. final id
2565	76	7	5	26	25377.82	0.08	0.03	N	O	1998-03-05	1998-04-11	1998-03-11	TAKE BACK RETURN         	AIR       	ites wake. ironic acco
2565	141	4	6	48	49974.72	0.08	0.07	N	O	1998-06-18	1998-05-06	1998-07-13	DELIVER IN PERSON        	TRUCK     	r instructions sleep qui
2566	148	5	1	19	19914.66	0.06	0.07	R	F	1992-12-21	1992-11-24	1992-12-22	DELIVER IN PERSON        	MAIL      	ests. silent
2566	181	2	2	42	45409.56	0.08	0.02	R	F	1992-12-20	1992-12-22	1992-12-29	COLLECT COD              	MAIL      	ously ironic accounts
2566	23	8	3	18	16614.36	0.09	0.02	A	F	1992-11-16	1992-12-24	1992-12-16	COLLECT COD              	FOB       	 braids according t
2566	42	9	4	3	2826.12	0.05	0.02	A	F	1992-11-04	1992-12-30	1992-12-04	TAKE BACK RETURN         	FOB       	ckages are ironic Tiresias. furious
2566	22	3	5	9	8298.18	0.04	0.03	R	F	1992-12-14	1992-12-28	1992-12-16	NONE                     	FOB       	blithely bold accounts? quickl
2566	128	3	6	1	1028.12	0.07	0.03	A	F	1992-10-28	1992-11-20	1992-11-22	TAKE BACK RETURN         	AIR       	theodolites wake pending
2592	90	1	1	7	6930.63	0.10	0.04	R	F	1993-03-13	1993-04-25	1993-04-01	NONE                     	REG AIR   	 carefully special theodolites integrate 
2592	66	1	2	2	1932.12	0.10	0.00	A	F	1993-03-24	1993-04-05	1993-04-16	DELIVER IN PERSON        	RAIL      	side of the b
2593	105	2	1	37	37188.70	0.08	0.06	R	F	1993-12-14	1993-10-08	1994-01-04	NONE                     	SHIP      	s wake bravel
2593	90	1	2	28	27722.52	0.08	0.03	A	F	1993-10-30	1993-10-18	1993-11-06	DELIVER IN PERSON        	SHIP      	y even escapades shall
2593	128	3	3	6	6168.72	0.04	0.05	A	F	1993-11-28	1993-10-04	1993-12-28	TAKE BACK RETURN         	REG AIR   	ular packages. re
2593	161	10	4	44	46691.04	0.02	0.08	A	F	1993-09-05	1993-10-23	1993-09-29	NONE                     	RAIL      	ents impress furiously; unusual theodoli
2593	4	5	5	3	2712.00	0.03	0.00	A	F	1993-12-16	1993-11-01	1993-12-29	COLLECT COD              	SHIP      	the furiously 
2593	175	6	6	1	1075.17	0.08	0.08	A	F	1993-11-23	1993-10-25	1993-12-04	DELIVER IN PERSON        	RAIL      	 accounts wake slyly 
2593	192	5	7	11	12014.09	0.00	0.07	R	F	1993-11-01	1993-11-19	1993-11-28	TAKE BACK RETURN         	RAIL      	express packages sleep bold re
2595	61	2	1	42	40364.52	0.08	0.02	N	O	1996-03-24	1996-01-28	1996-04-10	DELIVER IN PERSON        	MAIL      	ggle furiou
2595	88	9	2	30	29642.40	0.05	0.01	N	O	1996-03-05	1996-02-23	1996-03-19	NONE                     	AIR       	ctions. regula
2595	24	3	3	19	17556.38	0.01	0.05	N	O	1995-12-23	1996-03-02	1996-01-17	COLLECT COD              	MAIL      	ns are neve
2595	159	1	4	29	30715.35	0.07	0.05	N	O	1996-01-01	1996-02-13	1996-01-18	TAKE BACK RETURN         	RAIL      	ronic accounts haggle carefully fin
2595	86	7	5	30	29582.40	0.09	0.07	N	O	1996-03-16	1996-01-31	1996-04-05	TAKE BACK RETURN         	FOB       	. final orbits cajole 
2595	82	3	6	31	30444.48	0.06	0.04	N	O	1996-02-07	1996-02-10	1996-03-05	DELIVER IN PERSON        	AIR       	tipliers w
2597	84	5	1	24	23617.92	0.07	0.00	A	F	1993-05-15	1993-03-06	1993-05-25	TAKE BACK RETURN         	FOB       	pending packages. enticingly fi
2624	63	10	1	15	14445.90	0.03	0.07	N	O	1997-02-28	1997-02-19	1997-03-21	DELIVER IN PERSON        	AIR       	le. quickly pending requests
2624	189	10	2	12	13070.16	0.07	0.00	N	O	1997-02-24	1997-02-22	1997-02-27	DELIVER IN PERSON        	SHIP      	er the quickly unu
2625	20	1	1	42	38640.84	0.02	0.04	R	F	1992-10-18	1992-11-17	1992-10-23	DELIVER IN PERSON        	AIR       	 even accounts haggle furiously
2626	22	5	1	45	41490.90	0.09	0.04	N	O	1995-11-22	1995-11-01	1995-11-23	NONE                     	AIR       	deposits wake blithely according to 
2626	175	3	2	2	2150.34	0.05	0.07	N	O	1995-10-19	1995-11-09	1995-10-24	TAKE BACK RETURN         	FOB       	uffy accounts haggle furiously above
2626	154	2	3	40	42166.00	0.05	0.07	N	O	1995-09-28	1995-12-03	1995-10-10	NONE                     	REG AIR   	eans. ironic deposits haggle. depo
2628	106	9	1	44	44268.40	0.07	0.03	R	F	1994-01-11	1994-01-14	1994-01-13	DELIVER IN PERSON        	SHIP      	lyly final, pending ide
2628	106	9	2	14	14085.40	0.01	0.03	A	F	1994-01-28	1993-11-30	1994-02-20	TAKE BACK RETURN         	SHIP      	g the furiously unusual pi
2628	64	9	3	42	40490.52	0.00	0.00	A	F	1993-11-20	1994-01-04	1993-12-19	DELIVER IN PERSON        	TRUCK     	ld notornis alongside 
2628	95	7	4	23	22887.07	0.08	0.04	A	F	1993-10-27	1994-01-08	1993-11-12	DELIVER IN PERSON        	TRUCK     	usual packages sleep about the fina
2628	90	1	5	50	49504.50	0.07	0.01	A	F	1994-01-13	1993-12-11	1994-01-14	NONE                     	AIR       	posits serve carefully toward 
2629	118	9	1	6	6108.66	0.06	0.05	N	O	1998-06-10	1998-05-29	1998-06-13	DELIVER IN PERSON        	SHIP      	dolites hinder bli
2629	124	7	2	31	31747.72	0.08	0.03	N	O	1998-05-24	1998-05-26	1998-06-10	COLLECT COD              	AIR       	ate blithely bold, regular deposits. bold
2629	128	9	3	29	29815.48	0.08	0.07	N	O	1998-07-09	1998-06-17	1998-07-12	TAKE BACK RETURN         	AIR       	eposits serve unusual, express i
2629	70	5	4	33	32012.31	0.06	0.03	N	O	1998-05-29	1998-05-14	1998-05-30	NONE                     	TRUCK     	es. slowly express accounts are along the
2630	29	8	1	46	42734.92	0.05	0.03	R	F	1992-11-05	1992-12-17	1992-12-05	TAKE BACK RETURN         	MAIL      	uests cajole. e
2630	57	2	2	8	7656.40	0.09	0.07	A	F	1992-11-16	1993-01-01	1992-12-07	DELIVER IN PERSON        	TRUCK     	indle fluffily silent, ironic pi
2630	173	2	3	45	48292.65	0.08	0.07	A	F	1993-01-04	1993-01-11	1993-01-09	NONE                     	FOB       	edly express ideas. carefully final 
2630	162	9	4	29	30802.64	0.08	0.07	A	F	1992-12-03	1993-01-04	1992-12-12	DELIVER IN PERSON        	SHIP      	efully unusual dependencies. even i
2631	122	7	1	42	42929.04	0.00	0.03	A	F	1994-01-04	1993-12-01	1994-01-16	TAKE BACK RETURN         	SHIP      	ect carefully at the furiously final the
2631	67	4	2	4	3868.24	0.07	0.06	R	F	1993-11-03	1993-12-17	1993-11-05	COLLECT COD              	AIR       	special theodolites. a
2631	118	8	3	15	15271.65	0.06	0.05	A	F	1993-09-30	1993-11-06	1993-10-13	DELIVER IN PERSON        	SHIP      	y. furiously even pinto be
2657	115	9	1	22	22332.42	0.02	0.03	N	O	1995-12-08	1995-12-28	1995-12-21	TAKE BACK RETURN         	MAIL      	r ideas. furiously special dolphins
2657	165	2	2	15	15977.40	0.08	0.05	N	O	1995-12-09	1995-12-16	1995-12-18	NONE                     	RAIL      	ole carefully above the ironic ideas. b
2657	79	9	3	25	24476.75	0.02	0.04	N	O	1995-10-21	1995-12-12	1995-11-09	COLLECT COD              	FOB       	lly pinto beans. final 
2657	55	7	4	11	10505.55	0.04	0.08	N	O	1995-11-19	1995-12-11	1995-11-24	COLLECT COD              	TRUCK     	ckly enticing requests. fur
2657	78	9	5	42	41078.94	0.06	0.03	N	O	1996-01-23	1995-11-22	1996-01-25	COLLECT COD              	RAIL      	ckly slyly even accounts. platelets x-ray
2657	194	7	6	31	33919.89	0.01	0.03	N	O	1995-11-10	1995-11-27	1995-12-06	COLLECT COD              	RAIL      	re blithely 
2658	132	3	1	41	42317.33	0.05	0.04	N	O	1995-11-07	1995-11-04	1995-12-04	NONE                     	MAIL      	eposits. furiously final theodolite
2658	29	4	2	22	20438.44	0.08	0.05	N	O	1995-11-12	1995-11-18	1995-11-14	DELIVER IN PERSON        	TRUCK     	ts cajole. pending packages affix
2658	18	5	3	13	11934.13	0.07	0.06	N	O	1995-10-24	1995-12-12	1995-11-14	COLLECT COD              	FOB       	s kindle blithely regular accounts.
2658	92	5	4	22	21825.98	0.04	0.04	N	O	1995-12-02	1995-11-03	1995-12-26	DELIVER IN PERSON        	SHIP      	 dependencies. blithely pending foxes abou
2658	7	8	5	45	40815.00	0.03	0.01	N	O	1995-11-02	1995-11-08	1995-11-29	DELIVER IN PERSON        	MAIL      	e special requests. quickly ex
2658	147	4	6	27	28272.78	0.05	0.07	N	O	1995-09-26	1995-12-08	1995-09-30	NONE                     	AIR       	ecial packages use abov
2661	178	9	1	31	33423.27	0.03	0.02	N	O	1997-04-07	1997-03-10	1997-04-23	TAKE BACK RETURN         	AIR       	e ironicall
2661	103	8	2	22	22068.20	0.08	0.02	N	O	1997-03-14	1997-03-17	1997-04-08	COLLECT COD              	REG AIR   	 foxes affix quickly ironic request
2661	67	6	3	11	10637.66	0.00	0.08	N	O	1997-04-14	1997-02-11	1997-05-05	TAKE BACK RETURN         	FOB       	equests are a
2661	137	8	4	41	42522.33	0.06	0.02	N	O	1997-03-06	1997-03-27	1997-03-15	DELIVER IN PERSON        	AIR       	iously ironically ironic requests. 
2662	102	5	1	43	43090.30	0.09	0.07	N	O	1996-11-24	1996-11-04	1996-12-08	NONE                     	RAIL      	. slyly specia
2662	128	9	2	8	8224.96	0.02	0.07	N	O	1996-09-10	1996-10-09	1996-09-21	TAKE BACK RETURN         	REG AIR   	ajole carefully. sp
2662	2	5	3	6	5412.00	0.02	0.00	N	O	1996-11-30	1996-09-20	1996-12-03	DELIVER IN PERSON        	REG AIR   	olites cajole quickly along the b
2662	30	1	4	34	31621.02	0.06	0.07	N	O	1996-10-04	1996-11-05	1996-10-19	NONE                     	SHIP      	ding theodolites use carefully. p
2688	18	5	1	45	41310.45	0.08	0.08	R	F	1992-05-21	1992-04-14	1992-05-28	NONE                     	FOB       	sits run carefully
2688	15	6	2	46	42090.46	0.01	0.01	R	F	1992-05-24	1992-04-01	1992-05-26	COLLECT COD              	TRUCK     	elets. regular reque
2688	89	10	3	30	29672.40	0.05	0.04	A	F	1992-04-18	1992-03-18	1992-05-18	TAKE BACK RETURN         	RAIL      	ithely final 
2688	25	10	4	3	2775.06	0.00	0.03	R	F	1992-02-04	1992-03-18	1992-02-24	DELIVER IN PERSON        	RAIL      	e fluffily 
2688	59	10	5	22	21099.10	0.02	0.05	R	F	1992-02-09	1992-04-09	1992-02-11	DELIVER IN PERSON        	RAIL      	press, ironic excuses wake carefully id
2688	149	10	6	42	44063.88	0.01	0.01	R	F	1992-04-29	1992-04-04	1992-05-17	TAKE BACK RETURN         	FOB       	lly even account
2689	6	1	1	45	40770.00	0.02	0.04	R	F	1992-04-29	1992-06-22	1992-04-30	COLLECT COD              	SHIP      	e quickly. carefully silent
2690	140	1	1	44	45766.16	0.05	0.06	N	O	1996-05-30	1996-05-19	1996-06-26	NONE                     	REG AIR   	ly alongside of th
2690	51	2	2	50	47552.50	0.03	0.03	N	O	1996-06-13	1996-05-22	1996-06-14	DELIVER IN PERSON        	MAIL      	 doubt careful
2690	125	6	3	45	46130.40	0.02	0.07	N	O	1996-05-23	1996-06-02	1996-05-29	DELIVER IN PERSON        	MAIL      	ounts. slyly regular dependencies wa
2690	195	6	4	12	13142.28	0.04	0.07	N	O	1996-07-18	1996-06-03	1996-07-25	NONE                     	AIR       	nal, regular atta
2690	86	7	5	30	29582.40	0.01	0.08	N	O	1996-05-20	1996-06-01	1996-06-04	TAKE BACK RETURN         	SHIP      	d accounts above the express req
2690	189	10	6	3	3267.54	0.07	0.01	N	O	1996-07-04	1996-05-28	1996-07-06	TAKE BACK RETURN         	RAIL      	. final reques
2690	79	7	7	35	34267.45	0.05	0.06	N	O	1996-07-25	1996-05-14	1996-08-03	COLLECT COD              	FOB       	y silent pinto be
2692	17	1	1	3	2751.03	0.10	0.04	N	O	1998-02-25	1998-01-29	1998-03-27	TAKE BACK RETURN         	MAIL      	equests. bold, even foxes haggle slyl
2692	114	1	2	21	21296.31	0.03	0.05	N	O	1998-03-11	1998-02-11	1998-03-19	NONE                     	SHIP      	posits. final, express requests nag furi
2727	151	6	1	3	3153.45	0.03	0.01	N	O	1998-06-18	1998-06-06	1998-06-23	NONE                     	RAIL      	 the carefully regular foxes u
2752	31	2	1	41	38172.23	0.02	0.05	A	F	1994-03-02	1994-01-31	1994-03-06	DELIVER IN PERSON        	AIR       	tructions hag
2752	7	2	2	29	26303.00	0.02	0.04	R	F	1994-01-22	1994-01-08	1994-01-28	COLLECT COD              	TRUCK     	gly blithely re
2752	56	7	3	4	3824.20	0.08	0.00	A	F	1993-12-14	1994-02-13	1994-01-05	DELIVER IN PERSON        	TRUCK     	telets haggle. regular, final 
2752	24	7	4	40	36960.80	0.09	0.06	A	F	1994-01-24	1994-01-18	1994-02-22	DELIVER IN PERSON        	MAIL      	into beans are after the sly
2752	126	5	5	22	22574.64	0.03	0.04	A	F	1994-03-20	1994-02-08	1994-04-01	TAKE BACK RETURN         	TRUCK     	equests nag. regular dependencies are furio
2752	170	5	6	21	22473.57	0.09	0.05	R	F	1994-01-01	1994-01-24	1994-01-24	COLLECT COD              	SHIP      	 along the quickly 
2752	199	10	7	38	41769.22	0.08	0.00	R	F	1994-02-23	1993-12-23	1994-03-24	DELIVER IN PERSON        	SHIP      	es boost. slyly silent ideas
2753	13	3	1	6	5478.06	0.10	0.04	A	F	1993-12-30	1994-01-28	1994-01-29	COLLECT COD              	TRUCK     	s accounts
2753	48	7	2	40	37921.60	0.03	0.05	A	F	1994-01-06	1994-02-13	1994-02-03	DELIVER IN PERSON        	SHIP      	latelets kindle slyly final depos
2753	89	10	3	30	29672.40	0.00	0.07	A	F	1994-01-26	1994-01-29	1994-02-02	NONE                     	RAIL      	ans wake fluffily blithely iro
2753	31	7	4	7	6517.21	0.07	0.03	R	F	1994-02-11	1994-01-22	1994-03-10	DELIVER IN PERSON        	AIR       	xpress ideas detect b
2753	137	8	5	36	37336.68	0.04	0.08	R	F	1994-03-15	1994-01-03	1994-04-03	DELIVER IN PERSON        	SHIP      	gle slyly final c
2753	50	1	6	17	16150.85	0.01	0.08	A	F	1994-03-08	1994-01-17	1994-03-11	TAKE BACK RETURN         	REG AIR   	 carefully bold deposits sublate s
2753	148	9	7	20	20962.80	0.01	0.06	R	F	1994-02-24	1994-02-04	1994-03-23	DELIVER IN PERSON        	FOB       	 express pack
2757	148	5	1	26	27251.64	0.07	0.00	N	O	1995-08-19	1995-10-02	1995-09-06	DELIVER IN PERSON        	MAIL      	around the blithely
2757	22	7	2	12	11064.24	0.07	0.08	N	O	1995-08-01	1995-09-04	1995-08-08	TAKE BACK RETURN         	SHIP      	 regular, eve
2757	73	3	3	17	16542.19	0.10	0.04	N	O	1995-09-06	1995-09-27	1995-09-22	DELIVER IN PERSON        	AIR       	er the furiously silent 
2757	140	1	4	25	26003.50	0.08	0.01	N	O	1995-11-09	1995-09-12	1995-11-23	NONE                     	AIR       	uickly regular 
2757	70	7	5	14	13580.98	0.04	0.05	N	O	1995-09-01	1995-08-24	1995-09-03	TAKE BACK RETURN         	SHIP      	special deposits u
2785	100	3	1	34	34003.40	0.08	0.06	N	O	1995-08-07	1995-09-09	1995-09-05	NONE                     	RAIL      	ly final packages haggl
2785	110	7	2	37	37374.07	0.08	0.04	N	O	1995-07-25	1995-09-12	1995-08-06	DELIVER IN PERSON        	TRUCK     	tructions. furiously 
2785	65	10	3	33	31846.98	0.08	0.06	N	O	1995-10-16	1995-08-24	1995-11-02	DELIVER IN PERSON        	MAIL      	fter the furiously final p
2785	48	1	4	34	32233.36	0.00	0.02	N	O	1995-09-16	1995-09-09	1995-10-11	COLLECT COD              	SHIP      	kages wake carefully silent 
2787	33	9	1	4	3732.12	0.04	0.04	N	O	1996-01-26	1995-11-26	1996-02-20	TAKE BACK RETURN         	SHIP      	ts. instructions nag furiously according 
2789	163	8	1	16	17010.56	0.03	0.02	N	O	1998-04-18	1998-05-25	1998-05-12	DELIVER IN PERSON        	REG AIR   	o beans use carefully
2789	23	4	2	41	37843.82	0.02	0.05	N	O	1998-03-20	1998-05-15	1998-03-21	COLLECT COD              	MAIL      	d packages-- fluffily specia
2789	176	5	3	33	35513.61	0.06	0.02	N	O	1998-04-21	1998-05-02	1998-04-30	COLLECT COD              	TRUCK     	deposits. ironic 
2789	16	3	4	47	43052.47	0.02	0.04	N	O	1998-03-29	1998-05-05	1998-04-07	NONE                     	RAIL      	usly busy packages wake against the unusual
2789	197	1	5	23	25235.37	0.02	0.07	N	O	1998-03-25	1998-05-10	1998-04-24	COLLECT COD              	RAIL      	cording to the careful de
2789	144	5	6	16	16706.24	0.07	0.03	N	O	1998-05-11	1998-05-08	1998-05-24	TAKE BACK RETURN         	RAIL      	d the carefully iron
2789	133	4	7	42	43391.46	0.01	0.00	N	O	1998-04-28	1998-05-17	1998-05-24	TAKE BACK RETURN         	AIR       	ending packages shoul
2790	185	6	1	27	29299.86	0.06	0.08	R	F	1994-09-04	1994-09-27	1994-09-16	TAKE BACK RETURN         	MAIL      	ilent packages cajole. quickly ironic requ
2790	117	1	2	50	50855.50	0.00	0.06	A	F	1994-12-08	1994-11-17	1994-12-19	NONE                     	RAIL      	fter the regular ideas. f
2790	184	5	3	19	20599.42	0.06	0.00	R	F	1994-10-23	1994-10-03	1994-10-26	TAKE BACK RETURN         	RAIL      	uffily even excuses. furiously thin
2790	197	8	4	24	26332.56	0.07	0.01	A	F	1994-12-04	1994-10-10	1994-12-25	NONE                     	MAIL      	ments. slyly f
2790	148	9	5	11	11529.54	0.08	0.03	A	F	1994-09-28	1994-11-14	1994-10-04	TAKE BACK RETURN         	AIR       	lar requests poach slyly foxes
2790	73	3	6	13	12649.91	0.08	0.00	R	F	1994-09-20	1994-10-10	1994-10-20	COLLECT COD              	SHIP      	n deposits according to the regul
2790	4	1	7	32	28928.00	0.08	0.02	A	F	1994-09-25	1994-10-26	1994-10-01	NONE                     	SHIP      	ully pending
2817	60	8	1	25	24001.50	0.07	0.01	R	F	1994-04-21	1994-06-20	1994-05-07	DELIVER IN PERSON        	FOB       	doze blithely.
2817	32	8	2	5	4660.15	0.03	0.04	A	F	1994-05-07	1994-05-31	1994-05-12	TAKE BACK RETURN         	AIR       	furiously unusual theodolites use furiou
2817	172	10	3	35	37525.95	0.01	0.07	A	F	1994-05-20	1994-06-03	1994-05-22	COLLECT COD              	FOB       	gular foxes
2817	161	2	4	4	4244.64	0.00	0.05	R	F	1994-06-04	1994-06-11	1994-06-10	NONE                     	TRUCK     	n accounts wake across the fluf
2818	121	4	1	12	12253.44	0.10	0.03	A	F	1995-02-01	1995-03-10	1995-02-16	NONE                     	AIR       	lms. quickly bold asymp
2818	199	2	2	22	24182.18	0.06	0.07	R	F	1995-02-28	1995-03-10	1995-03-06	TAKE BACK RETURN         	RAIL      	egrate toward the carefully iron
2818	45	6	3	11	10395.44	0.01	0.06	R	F	1995-02-18	1995-02-11	1995-03-19	TAKE BACK RETURN         	TRUCK     	ggle across the carefully blithe
2818	40	6	4	32	30081.28	0.08	0.08	R	F	1995-02-04	1995-03-05	1995-02-18	COLLECT COD              	REG AIR   	arefully! ac
2818	18	8	5	42	38556.42	0.08	0.04	A	F	1995-02-12	1995-02-19	1995-03-13	COLLECT COD              	MAIL      	ar accounts wake carefully a
2818	91	5	6	7	6937.63	0.06	0.03	R	F	1995-03-24	1995-03-09	1995-04-06	TAKE BACK RETURN         	TRUCK     	ly according to the r
2819	70	1	1	17	16491.19	0.08	0.08	A	F	1994-07-16	1994-07-15	1994-07-17	TAKE BACK RETURN         	RAIL      	en deposits above the f
2819	67	2	2	12	11604.72	0.03	0.08	R	F	1994-07-18	1994-06-24	1994-07-28	NONE                     	MAIL      	 regular, regular a
2819	5	2	3	28	25340.00	0.03	0.08	R	F	1994-05-09	1994-07-02	1994-05-15	NONE                     	RAIL      	ckages sublate carefully closely regular 
2819	153	4	4	5	5265.75	0.00	0.02	R	F	1994-05-29	1994-06-12	1994-06-28	NONE                     	TRUCK     	 fluffily unusual foxes sleep caref
2819	200	3	5	6	6601.20	0.03	0.01	A	F	1994-07-22	1994-08-02	1994-07-29	NONE                     	REG AIR   	eas after the carefully express pack
2849	154	2	1	16	16866.40	0.09	0.08	N	O	1996-05-20	1996-07-23	1996-06-18	NONE                     	TRUCK     	. furiously regular requ
2849	187	8	2	39	42400.02	0.10	0.03	N	O	1996-05-22	1996-07-18	1996-06-05	TAKE BACK RETURN         	SHIP      	s sleep furiously silently regul
2849	60	1	3	24	23041.44	0.01	0.05	N	O	1996-06-12	1996-07-10	1996-06-27	TAKE BACK RETURN         	AIR       	e slyly even asymptotes. slo
2849	55	7	4	48	45842.40	0.05	0.02	N	O	1996-05-03	1996-06-05	1996-05-28	NONE                     	AIR       	mong the carefully regular theodol
2849	28	7	5	30	27840.60	0.10	0.06	N	O	1996-08-24	1996-07-08	1996-09-03	TAKE BACK RETURN         	SHIP      	ly. carefully silent
2849	69	4	6	30	29071.80	0.06	0.07	N	O	1996-06-20	1996-07-23	1996-07-06	NONE                     	FOB       	yly furiously even id
2852	177	6	1	6	6463.02	0.01	0.01	R	F	1993-03-02	1993-04-11	1993-03-11	TAKE BACK RETURN         	RAIL      	 accounts above the furiously un
2852	41	10	2	24	22584.96	0.05	0.07	R	F	1993-01-18	1993-03-13	1993-02-14	DELIVER IN PERSON        	MAIL      	 the blithe
2852	164	9	3	29	30860.64	0.09	0.05	R	F	1993-04-21	1993-03-22	1993-05-02	COLLECT COD              	SHIP      	lyly ironi
2852	100	3	4	12	12001.20	0.08	0.02	A	F	1993-02-25	1993-03-24	1993-03-07	TAKE BACK RETURN         	TRUCK     	le. request
2852	154	2	5	28	29516.20	0.05	0.03	R	F	1993-02-08	1993-03-30	1993-02-11	NONE                     	MAIL      	e accounts. caref
2853	139	5	1	14	14547.82	0.07	0.05	R	F	1994-05-16	1994-07-01	1994-05-27	NONE                     	TRUCK     	oach slyly along t
2853	134	10	2	26	26887.38	0.06	0.01	R	F	1994-06-26	1994-06-05	1994-07-02	TAKE BACK RETURN         	MAIL      	dolphins wake slyly. blith
2853	173	3	3	40	42926.80	0.06	0.04	A	F	1994-08-06	1994-06-24	1994-08-29	NONE                     	RAIL      	lyly. pearls cajole. final accounts ca
2853	132	8	4	20	20642.60	0.02	0.04	A	F	1994-08-30	1994-06-16	1994-09-06	TAKE BACK RETURN         	TRUCK     	e slyly silent foxes. express deposits sno
2853	36	7	5	1	936.03	0.08	0.05	R	F	1994-09-01	1994-06-27	1994-09-12	TAKE BACK RETURN         	FOB       	refully slyly quick packages. final c
2854	181	2	1	46	49734.28	0.00	0.04	A	F	1994-09-22	1994-08-02	1994-09-30	COLLECT COD              	AIR       	. furiously regular deposits across th
2854	88	9	2	29	28654.32	0.09	0.07	R	F	1994-07-06	1994-08-26	1994-07-09	COLLECT COD              	SHIP      	y slyly ironic accounts. foxes haggle slyl
2854	160	8	3	20	21203.20	0.08	0.01	R	F	1994-09-18	1994-08-03	1994-10-12	COLLECT COD              	AIR       	rs impress after the deposits. 
2854	170	1	4	34	36385.78	0.06	0.03	A	F	1994-09-06	1994-08-07	1994-09-22	NONE                     	REG AIR   	age carefully
2854	102	3	5	7	7014.70	0.03	0.06	A	F	1994-09-23	1994-08-14	1994-10-10	DELIVER IN PERSON        	REG AIR   	 the pending
2854	18	2	6	13	11934.13	0.04	0.03	R	F	1994-09-15	1994-08-18	1994-09-19	DELIVER IN PERSON        	SHIP      	 excuses wak
2855	33	4	1	50	46651.50	0.03	0.07	A	F	1993-05-20	1993-06-28	1993-06-16	TAKE BACK RETURN         	TRUCK     	beans. deposits 
2884	71	2	1	41	39813.87	0.03	0.00	N	O	1998-01-02	1997-12-17	1998-01-20	DELIVER IN PERSON        	TRUCK     	ep. slyly even accounts a
2884	146	5	2	25	26153.50	0.09	0.08	N	O	1998-01-18	1997-12-06	1998-02-16	TAKE BACK RETURN         	MAIL      	onic theodolites with the instructi
2884	26	7	3	8	7408.16	0.08	0.08	N	O	1997-11-30	1997-11-28	1997-12-14	COLLECT COD              	TRUCK     	pending accounts about 
2912	122	1	1	8	8176.96	0.06	0.04	A	F	1992-04-09	1992-04-19	1992-04-26	NONE                     	RAIL      	hs cajole over the slyl
2912	115	9	2	18	18271.98	0.00	0.08	R	F	1992-03-13	1992-04-19	1992-03-30	TAKE BACK RETURN         	RAIL      	unts cajole reg
2915	175	5	1	28	30104.76	0.10	0.02	R	F	1994-04-17	1994-06-09	1994-05-10	NONE                     	MAIL      	yly special 
2915	94	7	2	12	11929.08	0.00	0.03	A	F	1994-07-18	1994-06-11	1994-07-27	TAKE BACK RETURN         	RAIL      	accounts. slyly final
2915	136	2	3	15	15541.95	0.07	0.00	A	F	1994-05-01	1994-06-12	1994-05-15	DELIVER IN PERSON        	TRUCK     	al requests haggle furiousl
2915	81	2	4	43	42186.44	0.06	0.05	R	F	1994-06-02	1994-05-24	1994-06-06	DELIVER IN PERSON        	SHIP      	into beans dazzle alongside of
2917	93	4	1	36	35751.24	0.10	0.01	N	O	1998-04-07	1998-02-23	1998-05-01	DELIVER IN PERSON        	RAIL      	usly ironic d
2917	21	2	2	20	18420.40	0.06	0.03	N	O	1997-12-31	1998-01-22	1998-01-12	NONE                     	MAIL      	slyly even ideas wa
2917	90	1	3	4	3960.36	0.02	0.07	N	O	1998-01-10	1998-01-18	1998-02-08	TAKE BACK RETURN         	REG AIR   	s. unusual instruct
2917	167	2	4	5	5335.80	0.05	0.01	N	O	1997-12-16	1998-01-26	1998-01-07	NONE                     	RAIL      	bove the furiously silent packages. pend
2917	41	10	5	37	34818.48	0.04	0.01	N	O	1997-12-12	1998-02-03	1997-12-23	COLLECT COD              	RAIL      	dependencies. express 
2917	194	8	6	7	7659.33	0.05	0.01	N	O	1998-03-21	1998-03-03	1998-03-25	NONE                     	REG AIR   	ly about the regular accounts. carefully pe
2944	120	1	1	44	44885.28	0.08	0.05	N	O	1997-12-25	1997-10-28	1998-01-21	COLLECT COD              	AIR       	ickly special theodolit
2944	42	9	2	44	41449.76	0.06	0.02	N	O	1997-10-28	1997-11-22	1997-11-10	NONE                     	SHIP      	ickly. regular requests haggle. idea
2944	170	5	3	2	2140.34	0.06	0.07	N	O	1997-12-13	1997-12-01	1998-01-08	DELIVER IN PERSON        	REG AIR   	luffily expr
2944	17	7	4	23	21091.23	0.02	0.03	N	O	1998-01-12	1997-12-03	1998-01-17	TAKE BACK RETURN         	MAIL      	 excuses? regular platelets e
2944	75	4	5	18	17551.26	0.10	0.01	N	O	1998-01-07	1997-10-26	1998-01-27	TAKE BACK RETURN         	FOB       	 furiously slyl
2944	60	2	6	17	16321.02	0.00	0.03	N	O	1997-10-18	1997-11-27	1997-10-29	TAKE BACK RETURN         	SHIP      	slyly final dolphins sleep silent the
2944	90	1	7	7	6930.63	0.01	0.06	N	O	1997-10-30	1997-11-03	1997-11-03	DELIVER IN PERSON        	FOB       	fluffily blithely express pea
2945	59	10	1	37	35484.85	0.00	0.02	N	O	1996-02-10	1996-03-20	1996-02-12	COLLECT COD              	SHIP      	l instructions. regular, regular 
2945	72	2	2	30	29162.10	0.05	0.01	N	O	1996-01-19	1996-02-11	1996-01-26	NONE                     	TRUCK     	ular instructions
2945	127	8	3	28	28759.36	0.06	0.02	N	O	1996-03-17	1996-03-13	1996-04-15	COLLECT COD              	FOB       	le slyly along the eve
2945	188	9	4	34	36998.12	0.08	0.06	N	O	1996-02-03	1996-03-17	1996-02-29	COLLECT COD              	REG AIR   	at the unusual theodolite
2945	173	1	5	10	10731.70	0.09	0.05	N	O	1996-03-13	1996-03-10	1996-04-06	COLLECT COD              	FOB       	thely. final courts could hang qu
2945	97	9	6	45	44869.05	0.07	0.00	N	O	1996-03-01	1996-03-25	1996-03-08	TAKE BACK RETURN         	MAIL      	ainst the final packages
2945	52	10	7	47	44746.35	0.07	0.05	N	O	1996-01-05	1996-02-11	1996-01-12	DELIVER IN PERSON        	MAIL      	quests use
2950	130	1	1	32	32964.16	0.01	0.05	N	O	1997-09-21	1997-08-25	1997-10-08	DELIVER IN PERSON        	REG AIR   	its wake carefully slyly final ideas.
2950	66	7	2	18	17389.08	0.10	0.01	N	O	1997-07-19	1997-08-29	1997-08-17	COLLECT COD              	TRUCK     	uests cajole furio
2950	53	4	3	14	13342.70	0.01	0.02	N	O	1997-07-29	1997-08-05	1997-07-31	TAKE BACK RETURN         	MAIL      	ccounts haggle carefully according 
2950	187	8	4	45	48923.10	0.08	0.00	N	O	1997-09-05	1997-09-23	1997-09-11	NONE                     	FOB       	ides the b
2950	61	2	5	46	44208.76	0.02	0.05	N	O	1997-07-15	1997-09-30	1997-07-25	COLLECT COD              	RAIL      	to the regular accounts are slyly carefu
2950	174	5	6	27	29002.59	0.01	0.03	N	O	1997-10-01	1997-09-13	1997-10-08	NONE                     	TRUCK     	are alongside of the carefully silent 
2951	3	8	1	5	4515.00	0.03	0.03	N	O	1996-03-27	1996-04-16	1996-03-30	NONE                     	REG AIR   	to beans wake ac
2951	136	2	2	24	24867.12	0.07	0.03	N	O	1996-03-24	1996-04-16	1996-04-08	NONE                     	SHIP      	 ironic multipliers. express, regular
2951	187	8	3	40	43487.20	0.02	0.07	N	O	1996-05-03	1996-04-20	1996-05-22	COLLECT COD              	REG AIR   	ial deposits wake fluffily about th
2951	73	3	4	21	20434.47	0.06	0.08	N	O	1996-04-12	1996-04-27	1996-04-14	DELIVER IN PERSON        	REG AIR   	nt instructions toward the f
2951	51	6	5	15	14265.75	0.07	0.00	N	O	1996-03-25	1996-04-23	1996-03-27	COLLECT COD              	REG AIR   	inal account
2951	138	4	6	18	18686.34	0.06	0.00	N	O	1996-04-04	1996-04-27	1996-04-06	COLLECT COD              	FOB       	ep about the final, even package
2976	9	4	1	32	29088.00	0.06	0.00	A	F	1994-01-26	1994-02-13	1994-02-10	NONE                     	MAIL      	nding, ironic deposits sleep f
2976	4	5	2	24	21696.00	0.00	0.03	A	F	1994-03-19	1994-01-26	1994-04-18	COLLECT COD              	TRUCK     	ronic pinto beans. slyly bol
2976	10	5	3	35	31850.35	0.10	0.07	R	F	1993-12-19	1994-02-14	1994-01-11	NONE                     	RAIL      	boost slyly about the regular, regular re
2976	82	3	4	22	21605.76	0.00	0.04	A	F	1994-02-08	1994-03-03	1994-02-12	TAKE BACK RETURN         	FOB       	ncies kindle furiously. carefull
2976	134	5	5	13	13443.69	0.00	0.06	A	F	1994-02-06	1994-02-02	1994-02-19	NONE                     	FOB       	 furiously final courts boost 
2976	109	2	6	30	30273.00	0.08	0.03	R	F	1994-03-27	1994-02-01	1994-04-26	TAKE BACK RETURN         	RAIL      	c ideas! unusual
2977	70	5	1	25	24251.75	0.03	0.07	N	O	1996-09-21	1996-10-06	1996-10-13	TAKE BACK RETURN         	RAIL      	furiously pe
2980	37	3	1	2	1874.06	0.09	0.03	N	O	1996-11-18	1996-10-22	1996-11-27	TAKE BACK RETURN         	SHIP      	enly across the special, pending packag
2980	10	7	2	48	43680.48	0.04	0.05	N	O	1996-09-25	1996-12-09	1996-10-12	NONE                     	REG AIR   	totes. regular pinto 
2980	133	9	3	27	27894.51	0.08	0.08	N	O	1996-12-08	1996-12-03	1996-12-14	NONE                     	REG AIR   	 theodolites cajole blithely sl
2980	25	10	4	49	45325.98	0.03	0.02	N	O	1996-10-04	1996-12-04	1996-10-06	NONE                     	RAIL      	hy packages sleep quic
2980	187	8	5	24	26092.32	0.05	0.04	N	O	1997-01-12	1996-10-27	1997-01-14	NONE                     	MAIL      	elets. fluffily regular in
2980	109	4	6	43	43391.30	0.01	0.01	N	O	1996-12-07	1996-11-10	1997-01-02	COLLECT COD              	AIR       	sts. slyly regu
2981	14	4	1	17	15538.17	0.03	0.05	N	O	1998-10-17	1998-10-02	1998-10-21	DELIVER IN PERSON        	RAIL      	, unusual packages x-ray. furious
2981	176	4	2	8	8609.36	0.06	0.03	N	O	1998-08-21	1998-09-28	1998-09-05	DELIVER IN PERSON        	MAIL      	ng to the f
2981	37	3	3	14	13118.42	0.03	0.07	N	O	1998-08-30	1998-10-04	1998-09-04	DELIVER IN PERSON        	MAIL      	kages detect furiously express requests.
2982	112	6	1	21	21254.31	0.00	0.01	A	F	1995-04-03	1995-06-08	1995-04-18	DELIVER IN PERSON        	AIR       	ironic deposits. furiously ex
2982	99	2	2	13	12988.17	0.02	0.08	R	F	1995-03-31	1995-05-07	1995-04-18	TAKE BACK RETURN         	RAIL      	regular deposits unwind alongside 
2982	70	5	3	21	20371.47	0.01	0.01	R	F	1995-04-19	1995-06-03	1995-04-28	COLLECT COD              	SHIP      	egular ideas use furiously? bl
2983	163	4	1	44	46779.04	0.03	0.06	R	F	1992-02-09	1992-03-07	1992-03-09	TAKE BACK RETURN         	AIR       	ly regular instruct
2983	49	8	2	11	10439.44	0.09	0.06	A	F	1992-04-29	1992-02-27	1992-05-26	NONE                     	MAIL      	aids integrate s
3008	132	3	1	8	8257.04	0.10	0.04	N	O	1995-12-06	1996-01-12	1995-12-22	TAKE BACK RETURN         	FOB       	yly ironic foxes. regular requests h
3008	200	3	2	31	34106.20	0.05	0.06	N	O	1995-12-14	1995-12-11	1995-12-31	TAKE BACK RETURN         	AIR       	 bold packages. quic
3008	24	5	3	40	36960.80	0.01	0.03	N	O	1995-12-18	1996-01-06	1996-01-11	COLLECT COD              	AIR       	esias. theodolites detect blithely 
3008	60	1	4	48	46082.88	0.07	0.06	N	O	1996-01-23	1996-01-07	1996-02-09	COLLECT COD              	SHIP      	ld theodolites. fluffily bold theodolit
3008	105	10	5	31	31158.10	0.03	0.02	N	O	1995-12-01	1996-01-20	1995-12-28	COLLECT COD              	RAIL      	nts use thinly around the carefully iro
3009	45	8	1	48	45361.92	0.10	0.02	N	O	1997-03-19	1997-05-13	1997-04-11	TAKE BACK RETURN         	TRUCK     	 dependencies sleep quickly a
3009	185	6	2	38	41236.84	0.00	0.01	N	O	1997-05-01	1997-04-10	1997-05-17	TAKE BACK RETURN         	AIR       	nal packages should haggle slyly. quickl
3009	130	3	3	26	26783.38	0.08	0.02	N	O	1997-05-15	1997-05-10	1997-06-13	TAKE BACK RETURN         	SHIP      	uriously specia
3011	198	10	1	5	5490.95	0.02	0.04	R	F	1992-04-21	1992-02-23	1992-05-15	NONE                     	TRUCK     	nusual sentiments. carefully bold idea
3011	123	4	2	42	42971.04	0.05	0.00	A	F	1992-02-01	1992-03-18	1992-02-29	NONE                     	TRUCK     	osits haggle quickly pending, 
3014	163	4	1	36	38273.76	0.05	0.03	A	F	1992-11-16	1993-01-20	1992-11-28	TAKE BACK RETURN         	FOB       	ding accounts boost fu
3014	106	1	2	36	36219.60	0.00	0.08	R	F	1992-12-28	1992-12-29	1993-01-24	COLLECT COD              	MAIL      	iously ironic r
3014	151	9	3	48	50455.20	0.06	0.02	A	F	1992-12-19	1993-01-08	1992-12-25	DELIVER IN PERSON        	REG AIR   	y pending theodolites wake. reg
3014	114	1	4	14	14197.54	0.10	0.02	R	F	1992-11-19	1993-01-01	1992-12-17	DELIVER IN PERSON        	SHIP      	. slyly brave platelets nag. careful,
3014	75	5	5	28	27301.96	0.02	0.08	R	F	1993-01-09	1992-12-18	1993-01-10	TAKE BACK RETURN         	FOB       	es are. final braids nag slyly. fluff
3014	38	4	6	30	28140.90	0.04	0.01	R	F	1993-02-28	1993-01-02	1993-03-20	TAKE BACK RETURN         	AIR       	 final foxes.
3015	3	8	1	5	4515.00	0.09	0.00	A	F	1993-01-10	1992-12-02	1993-01-19	TAKE BACK RETURN         	RAIL      	 the furiously pendi
3015	18	2	2	17	15606.17	0.03	0.01	R	F	1992-10-16	1992-11-20	1992-10-28	COLLECT COD              	AIR       	s above the fluffily final t
3015	91	4	3	23	22795.07	0.03	0.05	A	F	1992-12-03	1992-11-19	1992-12-23	DELIVER IN PERSON        	FOB       	s are slyly carefully special pinto bea
3015	156	7	4	7	7393.05	0.10	0.03	A	F	1992-12-07	1992-12-17	1992-12-30	DELIVER IN PERSON        	REG AIR   	 after the evenly special packages ca
3015	165	4	5	42	44736.72	0.04	0.02	R	F	1993-01-21	1992-11-07	1993-02-11	DELIVER IN PERSON        	AIR       	encies haggle furious
3015	66	7	6	18	17389.08	0.02	0.03	R	F	1992-10-10	1992-11-19	1992-10-18	TAKE BACK RETURN         	MAIL      	equests wake fluffil
3041	181	2	1	5	5405.90	0.07	0.04	N	O	1997-07-20	1997-07-15	1997-08-17	COLLECT COD              	FOB       	posits dazzle special p
3041	146	9	2	9	9415.26	0.03	0.03	N	O	1997-06-29	1997-08-14	1997-07-19	COLLECT COD              	AIR       	iously across the silent pinto beans. furi
3041	68	5	3	9	8712.54	0.09	0.06	N	O	1997-08-28	1997-07-23	1997-09-16	TAKE BACK RETURN         	FOB       	scapades after the special
3045	88	9	1	41	40511.28	0.05	0.01	N	O	1995-09-30	1995-11-24	1995-10-03	TAKE BACK RETURN         	MAIL      	ely final foxes. carefully ironic pinto b
3045	69	6	2	48	46514.88	0.02	0.03	N	O	1995-10-01	1995-12-16	1995-10-10	TAKE BACK RETURN         	MAIL      	ole quickly outside th
3047	104	5	1	17	17069.70	0.08	0.02	N	O	1997-06-14	1997-04-20	1997-06-23	COLLECT COD              	FOB       	onic instruction
3047	14	1	2	23	21022.23	0.00	0.04	N	O	1997-05-20	1997-06-14	1997-05-28	TAKE BACK RETURN         	REG AIR   	 slyly ironi
3072	57	9	1	6	5742.30	0.09	0.05	R	F	1994-02-09	1994-03-24	1994-02-28	DELIVER IN PERSON        	REG AIR   	gular requests abov
3072	108	3	2	36	36291.60	0.07	0.02	R	F	1994-04-14	1994-04-22	1994-05-06	COLLECT COD              	AIR       	 theodolites. blithely e
3072	97	8	3	7	6979.63	0.04	0.07	R	F	1994-05-09	1994-03-31	1994-05-19	COLLECT COD              	TRUCK     	uests. ironic, ironic depos
3072	83	4	4	39	38340.12	0.05	0.08	A	F	1994-05-27	1994-04-20	1994-06-14	COLLECT COD              	MAIL      	es; slyly spe
3072	88	9	5	1	988.08	0.01	0.08	R	F	1994-02-26	1994-03-14	1994-03-19	NONE                     	AIR       	 slyly ironic attainments. car
3073	194	7	1	16	17507.04	0.07	0.01	R	F	1994-03-02	1994-03-23	1994-03-31	DELIVER IN PERSON        	AIR       	n requests. ironi
3073	22	5	2	47	43334.94	0.09	0.00	R	F	1994-03-26	1994-02-12	1994-04-21	NONE                     	REG AIR   	eposits. fluffily
3073	87	8	3	10	9870.80	0.03	0.00	R	F	1994-02-11	1994-03-24	1994-02-26	COLLECT COD              	FOB       	 furiously caref
3073	29	4	4	14	13006.28	0.09	0.07	R	F	1994-03-24	1994-04-01	1994-04-07	NONE                     	RAIL      	ilently quiet epitaphs.
3073	41	10	5	25	23526.00	0.00	0.07	R	F	1994-04-14	1994-03-07	1994-04-22	NONE                     	TRUCK     	nag asymptotes. pinto beans sleep 
3073	147	8	6	39	40838.46	0.09	0.02	R	F	1994-05-01	1994-02-16	1994-05-12	DELIVER IN PERSON        	AIR       	lar excuses across the furiously even 
3073	44	5	7	11	10384.44	0.08	0.07	A	F	1994-05-01	1994-03-06	1994-05-08	COLLECT COD              	SHIP      	instructions sleep according to the 
3074	37	8	1	50	46851.50	0.08	0.08	A	F	1993-01-31	1992-12-15	1993-02-20	NONE                     	AIR       	furiously pending requests haggle s
3074	139	5	2	39	40526.07	0.03	0.00	R	F	1992-12-08	1993-01-28	1992-12-09	DELIVER IN PERSON        	TRUCK     	iously throu
3076	85	6	1	44	43343.52	0.00	0.05	A	F	1993-09-14	1993-10-04	1993-09-17	TAKE BACK RETURN         	FOB       	 instructions h
3076	106	1	2	22	22134.20	0.08	0.00	A	F	1993-09-05	1993-09-10	1993-09-27	NONE                     	REG AIR   	packages wake furiou
3076	5	8	3	31	28055.00	0.06	0.06	A	F	1993-08-10	1993-09-17	1993-08-17	TAKE BACK RETURN         	SHIP      	regular depos
3078	132	3	1	25	25803.25	0.01	0.03	A	F	1993-04-22	1993-05-01	1993-04-28	TAKE BACK RETURN         	AIR       	express dinos. carefully ironic
3078	78	8	2	21	20539.47	0.09	0.07	A	F	1993-03-20	1993-03-21	1993-04-01	COLLECT COD              	AIR       	e fluffily. 
3108	109	2	1	37	37336.70	0.06	0.04	A	F	1993-10-16	1993-10-01	1993-11-09	DELIVER IN PERSON        	RAIL      	 final requests. 
3108	166	1	2	26	27720.16	0.08	0.05	A	F	1993-11-12	1993-10-05	1993-12-09	COLLECT COD              	TRUCK     	 slyly slow foxes wake furious
3136	142	5	1	30	31264.20	0.02	0.08	R	F	1994-08-13	1994-10-02	1994-09-02	TAKE BACK RETURN         	RAIL      	leep blithel
3136	103	4	2	7	7021.70	0.05	0.07	A	F	1994-10-08	1994-09-14	1994-10-11	TAKE BACK RETURN         	SHIP      	ic pinto beans are slyly. f
3136	158	3	3	43	45500.45	0.00	0.07	A	F	1994-09-05	1994-09-25	1994-09-11	NONE                     	RAIL      	. special theodolites ha
3136	116	6	4	26	26418.86	0.04	0.05	A	F	1994-10-13	1994-11-07	1994-11-05	TAKE BACK RETURN         	AIR       	eep fluffily. daringly silent attainments d
3136	67	8	5	2	1934.12	0.08	0.07	R	F	1994-11-21	1994-11-03	1994-11-26	DELIVER IN PERSON        	TRUCK     	? special, silent 
3136	80	1	6	29	28422.32	0.08	0.07	A	F	1994-11-16	1994-10-03	1994-12-14	NONE                     	FOB       	latelets. final 
3137	3	4	1	6	5418.00	0.02	0.02	N	O	1995-09-19	1995-10-23	1995-10-16	NONE                     	SHIP      	ly express as
3137	6	3	2	4	3624.00	0.06	0.04	N	O	1995-10-01	1995-09-11	1995-10-30	COLLECT COD              	RAIL      	posits wake. silent excuses boost about
3138	93	5	1	7	6951.63	0.05	0.05	R	F	1994-03-04	1994-03-14	1994-03-20	NONE                     	AIR       	lithely quickly even packages. packages
3138	44	5	2	27	25489.08	0.09	0.01	R	F	1994-03-24	1994-03-23	1994-04-18	DELIVER IN PERSON        	FOB       	counts cajole fluffily carefully special i
3138	197	8	3	32	35110.08	0.00	0.01	R	F	1994-02-24	1994-05-07	1994-02-28	TAKE BACK RETURN         	MAIL      	inal foxes affix slyly. fluffily regul
3138	172	3	4	38	40742.46	0.07	0.04	R	F	1994-02-21	1994-03-21	1994-03-13	COLLECT COD              	FOB       	lithely fluffily un
3138	10	1	5	12	10920.12	0.09	0.02	A	F	1994-03-04	1994-04-11	1994-03-21	COLLECT COD              	FOB       	. bold pinto beans haggl
3138	44	7	6	25	23601.00	0.05	0.08	A	F	1994-05-19	1994-04-07	1994-06-17	TAKE BACK RETURN         	AIR       	dolites around the carefully busy the
3139	40	6	1	46	43241.84	0.08	0.03	R	F	1992-04-28	1992-03-04	1992-05-19	TAKE BACK RETURN         	FOB       	of the unusual, unusual re
3168	60	8	1	46	44162.76	0.08	0.08	R	F	1992-02-14	1992-03-02	1992-03-02	TAKE BACK RETURN         	SHIP      	y across the express accounts. fluff
3168	154	5	2	1	1054.15	0.06	0.08	A	F	1992-05-27	1992-03-12	1992-06-09	TAKE BACK RETURN         	SHIP      	pinto beans. slyly regular courts haggle 
3168	128	3	3	13	13365.56	0.09	0.02	A	F	1992-03-05	1992-04-29	1992-03-15	NONE                     	SHIP      	ironic somas haggle quick
3168	165	10	4	11	11716.76	0.02	0.05	R	F	1992-04-12	1992-03-17	1992-05-12	COLLECT COD              	SHIP      	ously furious dependenc
3170	40	6	1	12	11280.48	0.03	0.03	N	O	1998-02-12	1998-01-17	1998-02-24	NONE                     	TRUCK     	ing accounts along the speci
3170	100	2	2	21	21002.10	0.01	0.00	N	O	1997-12-09	1998-01-31	1997-12-21	DELIVER IN PERSON        	MAIL      	o beans. carefully final requests dou
3170	89	10	3	27	26705.16	0.00	0.05	N	O	1998-02-25	1998-01-29	1998-02-27	COLLECT COD              	AIR       	efully bold foxes. regular, ev
3170	41	2	4	34	31995.36	0.05	0.04	N	O	1998-02-01	1998-01-11	1998-02-20	TAKE BACK RETURN         	TRUCK     	s about the fluffily final de
3170	90	1	5	32	31682.88	0.02	0.04	N	O	1997-11-24	1997-12-12	1997-12-15	COLLECT COD              	SHIP      	ggle about the furiously r
3170	110	5	6	43	43434.73	0.08	0.05	N	O	1998-01-05	1998-01-04	1998-01-14	NONE                     	REG AIR   	. express dolphins use sly
3170	84	5	7	26	25586.08	0.10	0.05	N	O	1998-02-12	1997-12-22	1998-02-28	COLLECT COD              	TRUCK     	s engage furiously. 
3171	47	4	1	34	32199.36	0.04	0.00	A	F	1993-05-30	1993-05-27	1993-06-06	DELIVER IN PERSON        	REG AIR   	r the final, even packages. quickly
3171	139	10	2	50	51956.50	0.01	0.04	A	F	1993-07-19	1993-05-15	1993-07-31	TAKE BACK RETURN         	REG AIR   	riously final foxes about the ca
3173	195	6	1	35	38331.65	0.01	0.08	N	O	1996-09-09	1996-10-15	1996-10-04	TAKE BACK RETURN         	RAIL      	 across the slyly even requests.
3173	178	7	2	5	5390.85	0.09	0.07	N	O	1996-12-06	1996-09-17	1996-12-07	DELIVER IN PERSON        	REG AIR   	express depo
3173	46	9	3	16	15136.64	0.06	0.01	N	O	1996-08-12	1996-09-21	1996-08-22	NONE                     	SHIP      	e special,
3173	94	5	4	2	1988.18	0.00	0.00	N	O	1996-10-15	1996-11-06	1996-10-18	COLLECT COD              	MAIL      	ular pearls
3173	185	6	5	2	2170.36	0.00	0.06	N	O	1996-08-18	1996-09-21	1996-09-07	DELIVER IN PERSON        	MAIL      	fluffily above t
3200	116	6	1	17	17273.87	0.10	0.00	N	O	1996-06-06	1996-04-21	1996-06-14	DELIVER IN PERSON        	AIR       	side of the furiously pendin
3200	166	1	2	27	28786.32	0.03	0.00	N	O	1996-05-07	1996-05-01	1996-05-09	TAKE BACK RETURN         	REG AIR   	as haggle furiously against the fluff
3200	131	2	3	36	37120.68	0.01	0.01	N	O	1996-03-22	1996-03-19	1996-03-30	DELIVER IN PERSON        	FOB       	f the carefu
3200	30	9	4	11	10230.33	0.10	0.02	N	O	1996-03-18	1996-03-21	1996-04-14	COLLECT COD              	RAIL      	osits sleep fur
3200	198	9	5	16	17571.04	0.05	0.00	N	O	1996-02-28	1996-03-13	1996-03-11	NONE                     	RAIL      	ly against the quiet packages. blith
3200	175	3	6	25	26879.25	0.10	0.01	N	O	1996-02-08	1996-04-11	1996-03-06	COLLECT COD              	FOB       	 slyly regular hockey players! pinto beans 
3204	12	2	1	10	9120.10	0.10	0.07	R	F	1993-01-27	1993-03-08	1993-01-29	COLLECT COD              	SHIP      	counts. bold 
3204	7	10	2	39	35373.00	0.10	0.03	R	F	1993-02-11	1993-03-19	1993-02-28	TAKE BACK RETURN         	MAIL      	sits sleep theodolites. slyly bo
3205	68	5	1	7	6776.42	0.09	0.00	R	F	1992-07-05	1992-06-17	1992-07-07	NONE                     	SHIP      	ly alongsi
3205	29	10	2	32	29728.64	0.08	0.03	A	F	1992-06-01	1992-07-10	1992-06-06	TAKE BACK RETURN         	RAIL      	lar accoun
3205	103	6	3	38	38117.80	0.10	0.08	A	F	1992-07-31	1992-06-03	1992-08-20	DELIVER IN PERSON        	AIR       	usly quiet accounts. slyly pending pinto 
3205	56	7	4	10	9560.50	0.01	0.07	A	F	1992-06-18	1992-07-04	1992-07-16	COLLECT COD              	RAIL      	 deposits cajole careful
3205	70	9	5	18	17461.26	0.03	0.03	A	F	1992-07-04	1992-06-14	1992-08-03	TAKE BACK RETURN         	RAIL      	symptotes. slyly even deposits ar
3205	195	8	6	19	20808.61	0.07	0.08	R	F	1992-05-28	1992-05-30	1992-06-05	COLLECT COD              	AIR       	yly pending packages snooz
3205	69	8	7	36	34886.16	0.06	0.03	A	F	1992-05-31	1992-06-19	1992-06-03	TAKE BACK RETURN         	SHIP      	s. ironic platelets above the s
3206	176	4	1	1	1076.17	0.07	0.05	N	O	1996-11-22	1996-10-16	1996-12-07	TAKE BACK RETURN         	FOB       	y unusual foxes cajole ab
3206	111	5	2	37	37411.07	0.07	0.01	N	O	1996-09-06	1996-10-31	1996-09-25	COLLECT COD              	SHIP      	 quick theodolites hagg
3206	186	7	3	24	26068.32	0.00	0.08	N	O	1996-08-25	1996-10-01	1996-09-04	COLLECT COD              	TRUCK     	encies sleep deposits--
3207	113	3	1	2	2026.22	0.10	0.03	N	O	1998-06-15	1998-04-20	1998-06-21	COLLECT COD              	MAIL      	among the ironic, even packages 
3207	71	9	2	42	40784.94	0.00	0.00	N	O	1998-05-02	1998-05-10	1998-06-01	NONE                     	SHIP      	to the quickly special accounts? ironically
3207	152	7	3	17	17886.55	0.03	0.04	N	O	1998-03-27	1998-04-06	1998-03-28	COLLECT COD              	RAIL      	eep against the instructions. gifts hag
3207	19	6	4	32	29408.32	0.00	0.03	N	O	1998-06-17	1998-04-26	1998-07-07	TAKE BACK RETURN         	SHIP      	y across the slyly express foxes. bl
3207	83	4	5	8	7864.64	0.00	0.06	N	O	1998-06-13	1998-04-26	1998-07-11	COLLECT COD              	SHIP      	y. final pint
3207	134	5	6	32	33092.16	0.03	0.05	N	O	1998-04-19	1998-05-01	1998-05-08	COLLECT COD              	FOB       	l deposits wake beyond the carefully
3233	51	2	1	23	21874.15	0.04	0.05	A	F	1994-12-07	1995-01-11	1994-12-26	NONE                     	AIR       	pending instructions use after the carefu
3233	154	6	2	6	6324.90	0.02	0.08	A	F	1994-12-06	1994-12-05	1994-12-07	TAKE BACK RETURN         	REG AIR   	requests are quickly above the slyly p
3233	100	4	3	2	2000.20	0.04	0.06	R	F	1995-01-03	1995-01-02	1995-01-21	TAKE BACK RETURN         	AIR       	 across the bold packages
3233	9	2	4	25	22725.00	0.04	0.07	A	F	1994-11-24	1995-01-07	1994-12-11	NONE                     	RAIL      	oss the pl
3234	79	10	1	45	44058.15	0.01	0.04	N	O	1996-05-15	1996-05-09	1996-06-02	DELIVER IN PERSON        	TRUCK     	 express packages are carefully. f
3234	84	5	2	23	22633.84	0.03	0.00	N	O	1996-05-29	1996-05-15	1996-06-17	DELIVER IN PERSON        	AIR       	d-- fluffily special packag
3234	75	4	3	16	15601.12	0.06	0.05	N	O	1996-06-10	1996-05-30	1996-06-18	COLLECT COD              	RAIL      	ithely ironic accounts wake along t
3234	122	1	4	50	51106.00	0.09	0.05	N	O	1996-06-11	1996-05-19	1996-06-18	NONE                     	MAIL      	ly regular ideas according to the regula
3234	165	2	5	14	14912.24	0.01	0.07	N	O	1996-04-06	1996-05-30	1996-04-13	NONE                     	REG AIR   	lithely regular f
3235	109	2	1	9	9081.90	0.07	0.00	N	O	1995-11-17	1995-12-24	1995-11-30	COLLECT COD              	AIR       	l courts sleep quickly slyly 
3235	95	6	2	43	42788.87	0.10	0.07	N	O	1995-12-25	1996-01-23	1996-01-09	COLLECT COD              	MAIL      	ckly final instru
3235	138	9	3	29	30105.77	0.06	0.06	N	O	1996-01-28	1995-12-26	1996-02-12	DELIVER IN PERSON        	RAIL      	e fluffy pinto bea
3235	178	9	4	23	24797.91	0.00	0.01	N	O	1996-02-16	1996-01-05	1996-03-07	DELIVER IN PERSON        	SHIP      	ldly ironic pinto beans
3236	117	4	1	10	10171.10	0.06	0.05	N	O	1996-11-15	1996-12-14	1996-11-29	TAKE BACK RETURN         	AIR       	arefully. fluffily reg
3236	122	7	2	21	21464.52	0.01	0.07	N	O	1996-12-23	1996-12-12	1997-01-21	NONE                     	AIR       	 final pinto 
3236	118	2	3	7	7126.77	0.07	0.01	N	O	1996-12-27	1996-12-18	1997-01-24	DELIVER IN PERSON        	SHIP      	dolites. slyly unus
3264	200	1	1	39	42907.80	0.06	0.06	N	O	1996-11-07	1996-12-12	1996-11-20	TAKE BACK RETURN         	REG AIR   	sleep carefully after the slyly final
3264	131	2	2	34	35058.42	0.00	0.01	N	O	1997-01-03	1997-01-06	1997-01-29	TAKE BACK RETURN         	REG AIR   	rns haggle carefully. blit
3264	125	8	3	11	11276.32	0.09	0.03	N	O	1996-12-11	1996-12-19	1996-12-15	DELIVER IN PERSON        	SHIP      	regular packages
3264	109	10	4	24	24218.40	0.09	0.07	N	O	1997-01-07	1996-12-13	1997-01-11	TAKE BACK RETURN         	RAIL      	ctions. quick
3264	63	4	5	6	5778.36	0.04	0.03	N	O	1996-11-10	1996-12-05	1996-11-22	TAKE BACK RETURN         	SHIP      	press packages. ironical
3264	141	2	6	43	44769.02	0.06	0.06	N	O	1997-01-17	1997-01-24	1997-02-01	TAKE BACK RETURN         	TRUCK     	leep at the blithely bold
3266	64	1	1	31	29885.86	0.09	0.02	N	O	1995-06-19	1995-05-04	1995-07-06	COLLECT COD              	MAIL      	grate among the quickly express deposits
3266	38	4	2	43	40335.29	0.06	0.07	R	F	1995-05-04	1995-05-30	1995-05-11	COLLECT COD              	AIR       	ular asymptotes use careful
3268	96	7	1	1	996.09	0.06	0.08	A	F	1994-09-12	1994-08-31	1994-09-16	NONE                     	TRUCK     	. ironic, bold requests use carefull
3268	42	9	2	40	37681.60	0.08	0.01	R	F	1994-06-30	1994-08-22	1994-07-25	COLLECT COD              	FOB       	ly. bold, eve
3269	161	10	1	40	42446.40	0.02	0.07	N	O	1996-06-11	1996-05-06	1996-06-15	DELIVER IN PERSON        	TRUCK     	es. pending d
3269	38	4	2	46	43149.38	0.00	0.02	N	O	1996-04-21	1996-04-12	1996-05-10	DELIVER IN PERSON        	MAIL      	final asymptotes nag
3269	44	3	3	39	36817.56	0.02	0.03	N	O	1996-03-13	1996-05-26	1996-03-19	COLLECT COD              	MAIL      	he express packages?
3269	83	4	4	37	36373.96	0.07	0.05	N	O	1996-06-14	1996-04-27	1996-07-07	NONE                     	MAIL      	egular requests. carefully un
3269	93	7	5	42	41709.78	0.09	0.05	N	O	1996-03-19	1996-04-24	1996-04-18	COLLECT COD              	TRUCK     	 the special packages. 
3269	131	7	6	16	16498.08	0.01	0.08	N	O	1996-03-03	1996-04-06	1996-03-06	NONE                     	RAIL      	s cajole. silent deposits are f
3270	35	1	1	11	10285.33	0.07	0.06	N	O	1997-07-29	1997-08-11	1997-08-05	TAKE BACK RETURN         	AIR       	 solve at the regular deposits. 
3270	38	4	2	44	41273.32	0.10	0.05	N	O	1997-07-20	1997-08-15	1997-08-04	DELIVER IN PERSON        	SHIP      	 accounts. carefully even 
3270	65	4	3	20	19301.20	0.01	0.02	N	O	1997-08-26	1997-07-31	1997-08-30	DELIVER IN PERSON        	FOB       	en accounts among the c
3270	189	10	4	29	31586.22	0.06	0.05	N	O	1997-07-01	1997-07-23	1997-07-10	TAKE BACK RETURN         	MAIL      	sly regular asymptotes. slyly dog
3270	34	10	5	32	29888.96	0.03	0.00	N	O	1997-09-23	1997-08-17	1997-09-27	NONE                     	REG AIR   	promise carefully.
3270	57	5	6	29	27754.45	0.01	0.04	N	O	1997-08-22	1997-08-17	1997-09-06	COLLECT COD              	RAIL      	ptotes nag above the quickly bold deposits
3270	117	1	7	9	9153.99	0.06	0.08	N	O	1997-08-14	1997-08-11	1997-09-09	DELIVER IN PERSON        	SHIP      	ual packages
3296	84	5	1	12	11808.96	0.06	0.07	R	F	1994-12-08	1994-12-14	1994-12-24	COLLECT COD              	AIR       	y about the slyly bold pinto bea
3296	149	8	2	31	32523.34	0.08	0.00	R	F	1995-01-26	1994-12-25	1995-02-16	NONE                     	REG AIR   	ainst the furi
3296	185	6	3	29	31470.22	0.02	0.04	A	F	1995-01-12	1994-11-26	1995-02-06	DELIVER IN PERSON        	SHIP      	ss ideas are reg
3296	140	1	4	47	48886.58	0.06	0.00	A	F	1994-11-08	1994-12-20	1994-11-30	NONE                     	FOB       	egular deposits. quic
3296	177	6	5	16	17234.72	0.06	0.02	R	F	1995-01-11	1994-12-27	1995-01-12	DELIVER IN PERSON        	SHIP      	kages cajole carefully 
3296	197	1	6	40	43887.60	0.00	0.04	A	F	1994-12-28	1994-12-08	1995-01-13	COLLECT COD              	REG AIR   	ronic ideas across the
3296	36	2	7	6	5616.18	0.02	0.01	R	F	1995-01-03	1994-12-23	1995-01-27	TAKE BACK RETURN         	AIR       	carefully fur
3297	134	10	1	10	10341.30	0.10	0.04	A	F	1992-12-14	1993-01-21	1992-12-26	NONE                     	SHIP      	ironic idea
3299	183	4	1	40	43327.20	0.03	0.02	A	F	1994-03-21	1994-03-23	1994-04-12	COLLECT COD              	AIR       	lyly even request
3329	138	4	1	36	37372.68	0.09	0.08	N	O	1995-08-06	1995-08-03	1995-08-14	DELIVER IN PERSON        	TRUCK     	ts at the re
3329	6	3	2	9	8154.00	0.00	0.02	N	O	1995-07-24	1995-08-02	1995-08-01	COLLECT COD              	MAIL      	lly final depo
3329	123	4	3	1	1023.12	0.04	0.08	N	O	1995-08-22	1995-09-28	1995-09-09	COLLECT COD              	REG AIR   	regular packages are carefull
3331	64	9	1	9	8676.54	0.08	0.07	A	F	1993-07-18	1993-07-03	1993-08-16	TAKE BACK RETURN         	AIR       	odolites. bold accounts
3331	21	2	2	38	34998.76	0.06	0.04	R	F	1993-07-24	1993-06-22	1993-08-23	NONE                     	AIR       	ymptotes haggle across the ca
3331	3	10	3	26	23478.00	0.09	0.05	A	F	1993-08-05	1993-07-17	1993-08-29	DELIVER IN PERSON        	MAIL      	p asymptotes. carefully unusual in
3333	150	9	1	27	28354.05	0.06	0.08	A	F	1992-12-06	1992-10-26	1992-12-07	COLLECT COD              	SHIP      	s dazzle fluffil
3333	199	3	2	36	39570.84	0.08	0.07	R	F	1992-11-20	1992-11-06	1992-12-16	TAKE BACK RETURN         	FOB       	foxes sleep neve
3333	108	1	3	38	38307.80	0.05	0.05	A	F	1992-10-30	1992-11-03	1992-11-04	NONE                     	MAIL      	ccounts promise bl
3333	113	4	4	49	49642.39	0.07	0.07	R	F	1992-10-02	1992-11-30	1992-10-12	DELIVER IN PERSON        	MAIL      	riously ironic r
3333	43	2	5	45	42436.80	0.07	0.08	A	F	1992-10-04	1992-11-08	1992-10-27	COLLECT COD              	SHIP      	dolites. quickly r
3334	187	8	1	20	21743.60	0.04	0.03	N	O	1996-05-21	1996-04-08	1996-05-26	TAKE BACK RETURN         	AIR       	uses nag furiously. instructions are ca
3334	190	1	2	7	7631.33	0.09	0.07	N	O	1996-04-28	1996-04-08	1996-05-25	NONE                     	SHIP      	nts sublate slyly express pack
3335	105	10	1	13	13066.30	0.06	0.07	N	O	1996-01-20	1995-12-20	1996-02-09	COLLECT COD              	REG AIR   	out the special asymptotes
3335	31	2	2	44	40965.32	0.07	0.02	N	O	1996-01-05	1995-12-25	1996-01-18	DELIVER IN PERSON        	SHIP      	r packages cajole ac
3335	140	6	3	16	16642.24	0.01	0.06	N	O	1995-10-18	1995-12-08	1995-11-03	DELIVER IN PERSON        	SHIP      	g packages. carefully regular reque
3335	90	1	4	47	46534.23	0.10	0.03	N	O	1995-12-02	1995-11-19	1995-12-27	NONE                     	MAIL      	 quickly special ideas.
3360	174	4	1	31	33299.27	0.08	0.04	N	O	1998-04-24	1998-04-12	1998-05-23	COLLECT COD              	REG AIR   	quests. carefully even deposits wake acros
3360	91	3	2	29	28741.61	0.00	0.06	N	O	1998-04-15	1998-02-25	1998-05-13	TAKE BACK RETURN         	FOB       	press asymptotes. furiously final 
3360	82	3	3	39	38301.12	0.08	0.03	N	O	1998-04-09	1998-04-20	1998-05-05	DELIVER IN PERSON        	REG AIR   	s. blithely express pinto bean
3360	117	7	4	29	29496.19	0.10	0.01	N	O	1998-05-19	1998-03-03	1998-06-09	TAKE BACK RETURN         	FOB       	hely gifts. spe
3360	58	6	5	4	3832.20	0.08	0.07	N	O	1998-02-27	1998-03-23	1998-03-28	COLLECT COD              	SHIP      	ly busy inst
3360	71	1	6	42	40784.94	0.04	0.01	N	O	1998-05-07	1998-04-18	1998-06-04	DELIVER IN PERSON        	FOB       	ages cajole. pending, 
3361	144	5	1	6	6264.84	0.02	0.02	R	F	1992-10-02	1992-10-25	1992-10-05	DELIVER IN PERSON        	FOB       	 packages sleep. furiously unus
3361	171	10	2	33	35348.61	0.01	0.02	R	F	1992-11-09	1992-10-15	1992-11-11	TAKE BACK RETURN         	MAIL      	uriously ironic accounts. ironic, ir
3361	191	5	3	31	33826.89	0.06	0.04	R	F	1992-08-29	1992-10-13	1992-09-08	NONE                     	FOB       	ts. pending, regular accounts sleep fur
3362	22	5	1	14	12908.28	0.06	0.05	N	O	1995-08-01	1995-09-06	1995-08-22	NONE                     	FOB       	even Tires
3362	195	6	2	41	44902.79	0.05	0.03	N	O	1995-10-31	1995-09-04	1995-11-17	COLLECT COD              	REG AIR   	ake alongside of the 
3362	115	9	3	40	40604.40	0.05	0.06	N	O	1995-08-19	1995-10-17	1995-09-05	TAKE BACK RETURN         	FOB       	packages haggle furi
3362	2	7	4	3	2706.00	0.03	0.01	N	O	1995-08-26	1995-09-02	1995-09-17	NONE                     	SHIP      	its cajole blithely excuses. de
3362	138	9	5	36	37372.68	0.06	0.00	N	O	1995-10-05	1995-08-28	1995-11-03	TAKE BACK RETURN         	RAIL      	es against the quickly permanent pint
3362	188	9	6	46	50056.28	0.09	0.05	N	O	1995-08-02	1995-10-12	1995-08-28	COLLECT COD              	REG AIR   	ly bold packages. regular deposits cajol
3363	10	3	1	42	38220.42	0.00	0.08	N	O	1995-11-09	1995-11-25	1995-11-15	TAKE BACK RETURN         	RAIL      	 blithely final ideas nag after
3363	191	4	2	21	22914.99	0.08	0.08	N	O	1995-12-10	1995-10-28	1995-12-28	COLLECT COD              	RAIL      	he regular, brave deposits. f
3363	159	7	3	2	2118.30	0.01	0.07	N	O	1996-01-22	1995-12-01	1996-02-18	TAKE BACK RETURN         	SHIP      	uickly bold ide
3363	113	3	4	20	20262.20	0.07	0.06	N	O	1995-12-11	1995-11-15	1995-12-21	COLLECT COD              	MAIL      	carefully quiet excuses wake. sl
3363	200	4	5	4	4400.80	0.00	0.08	N	O	1995-10-30	1995-11-17	1995-11-22	COLLECT COD              	FOB       	 ironic dependencie
3364	90	1	1	49	48514.41	0.03	0.05	N	O	1997-09-17	1997-08-23	1997-10-06	NONE                     	SHIP      	d accounts? caref
3364	111	2	2	38	38422.18	0.02	0.02	N	O	1997-08-30	1997-09-12	1997-09-27	COLLECT COD              	REG AIR   	 slyly express
3364	156	4	3	10	10561.50	0.00	0.01	N	O	1997-08-10	1997-08-24	1997-08-15	TAKE BACK RETURN         	SHIP      	g the accounts. final, busy accounts wi
3364	160	5	4	7	7421.12	0.10	0.05	N	O	1997-07-09	1997-08-01	1997-07-16	NONE                     	TRUCK     	furiously regular ideas haggle furiously b
3364	81	2	5	3	2943.24	0.01	0.00	N	O	1997-10-19	1997-08-15	1997-10-28	TAKE BACK RETURN         	TRUCK     	c theodolites. blithely ir
3366	40	1	1	4	3760.16	0.07	0.01	N	O	1997-05-20	1997-06-25	1997-06-03	DELIVER IN PERSON        	AIR       	 carefully about 
3366	136	2	2	9	9325.17	0.00	0.08	N	O	1997-06-02	1997-07-05	1997-06-26	COLLECT COD              	REG AIR   	ackages sleep carefully across the bli
3367	41	10	1	27	25408.08	0.01	0.03	A	F	1993-04-13	1993-03-16	1993-04-26	NONE                     	RAIL      	kly even instructions caj
3367	141	10	2	34	35398.76	0.04	0.08	A	F	1993-03-30	1993-02-23	1993-04-11	COLLECT COD              	MAIL      	 accounts wake slyly 
3367	120	7	3	38	38764.56	0.03	0.03	R	F	1993-03-13	1993-02-12	1993-03-31	NONE                     	RAIL      	even packages sleep blithely slyly expr
3392	171	10	1	40	42846.80	0.01	0.01	N	O	1996-02-18	1995-12-16	1996-02-26	COLLECT COD              	MAIL      	ress instructions affix carefully. fur
3392	123	2	2	13	13300.56	0.09	0.02	N	O	1995-11-26	1996-01-17	1995-12-01	NONE                     	MAIL      	across the fluffily bold deposits.
3392	127	10	3	34	34922.08	0.10	0.08	N	O	1996-01-20	1996-01-21	1996-01-24	DELIVER IN PERSON        	MAIL      	e carefully even braids. 
3392	124	3	4	7	7168.84	0.08	0.05	N	O	1995-12-07	1996-01-09	1995-12-29	TAKE BACK RETURN         	RAIL      	as. express, final accounts dou
3393	117	7	1	16	16273.76	0.01	0.00	N	O	1995-07-17	1995-08-19	1995-08-04	COLLECT COD              	TRUCK     	uses. instructions after the blithely 
3393	125	4	2	44	45105.28	0.08	0.04	N	O	1995-10-16	1995-08-05	1995-11-01	NONE                     	AIR       	ld requests hag
3393	97	1	3	25	24927.25	0.07	0.02	N	O	1995-10-17	1995-08-12	1995-11-11	DELIVER IN PERSON        	MAIL      	ng excuses
3393	72	2	4	48	46659.36	0.06	0.06	N	O	1995-07-12	1995-09-15	1995-08-02	NONE                     	FOB       	 blithely final reques
3393	178	7	5	37	39892.29	0.07	0.02	N	O	1995-10-16	1995-08-19	1995-10-19	COLLECT COD              	AIR       	ss the slyly ironic pinto beans. ironic,
3393	62	7	6	17	16355.02	0.04	0.01	N	O	1995-08-15	1995-09-07	1995-09-10	COLLECT COD              	MAIL      	kly ironic deposits could
3397	195	8	1	8	8761.52	0.07	0.01	A	F	1994-08-05	1994-08-11	1994-08-08	DELIVER IN PERSON        	RAIL      	y final foxes
3397	13	3	2	11	10043.11	0.00	0.07	A	F	1994-07-29	1994-09-18	1994-08-12	DELIVER IN PERSON        	REG AIR   	iously careful packages. s
3397	184	5	3	1	1084.18	0.07	0.05	R	F	1994-08-03	1994-07-30	1994-08-28	NONE                     	RAIL      	 regular packag
3397	86	7	4	33	32540.64	0.05	0.01	R	F	1994-09-04	1994-08-06	1994-09-22	COLLECT COD              	RAIL      	gular accounts. blithely re
3397	132	3	5	28	28899.64	0.05	0.05	R	F	1994-07-13	1994-08-26	1994-07-17	NONE                     	TRUCK     	counts around the final reques
3398	173	4	1	1	1073.17	0.01	0.08	N	O	1996-11-22	1996-11-16	1996-12-09	COLLECT COD              	MAIL      	 blithely final deposits.
3399	134	5	1	28	28955.64	0.09	0.05	N	O	1995-06-29	1995-05-19	1995-07-12	COLLECT COD              	AIR       	oggedly final theodolites grow. fi
3399	55	6	2	8	7640.40	0.01	0.05	A	F	1995-05-15	1995-04-19	1995-06-05	COLLECT COD              	TRUCK     	s use carefully carefully ir
3399	67	4	3	3	2901.18	0.03	0.00	N	F	1995-06-16	1995-04-04	1995-06-23	NONE                     	SHIP      	hely pending dugouts 
3399	14	5	4	21	19194.21	0.09	0.06	A	F	1995-03-12	1995-05-18	1995-03-28	TAKE BACK RETURN         	MAIL      	se final courts. exc
3424	181	2	1	39	42166.02	0.06	0.07	N	O	1996-11-03	1996-11-08	1996-11-23	DELIVER IN PERSON        	MAIL      	bits boost closely slyly p
3425	120	1	1	11	11221.32	0.03	0.08	N	O	1996-04-24	1996-05-29	1996-05-23	DELIVER IN PERSON        	FOB       	ckly final deposits use quickly?
3425	79	7	2	37	36225.59	0.06	0.03	N	O	1996-06-04	1996-05-09	1996-06-12	NONE                     	SHIP      	as sleep carefully into the caref
3425	14	4	3	8	7312.08	0.06	0.08	N	O	1996-07-22	1996-06-07	1996-07-26	TAKE BACK RETURN         	AIR       	iously regular theodolites wake. s
3425	19	10	4	37	34003.37	0.04	0.01	N	O	1996-07-10	1996-05-10	1996-08-02	NONE                     	SHIP      	ngside of the furiously thin dol
3425	79	9	5	48	46995.36	0.08	0.04	N	O	1996-04-14	1996-05-25	1996-04-23	TAKE BACK RETURN         	AIR       	uctions wake fluffily. care
3425	148	9	6	24	25155.36	0.05	0.04	N	O	1996-04-22	1996-06-24	1996-04-25	TAKE BACK RETURN         	AIR       	ajole blithely sl
3427	54	5	1	41	39116.05	0.10	0.01	N	O	1997-09-11	1997-07-03	1997-10-04	COLLECT COD              	RAIL      	s the carefully
3427	189	10	2	24	26140.32	0.02	0.04	N	O	1997-07-01	1997-07-28	1997-07-30	NONE                     	SHIP      	y bold, sly deposits. pendi
3427	139	5	3	40	41565.20	0.06	0.05	N	O	1997-06-12	1997-08-19	1997-06-23	COLLECT COD              	MAIL      	patterns cajole ca
3427	119	6	4	31	31592.41	0.08	0.04	N	O	1997-08-12	1997-07-26	1997-08-25	COLLECT COD              	RAIL      	s are carefull
3428	198	9	1	4	4392.76	0.00	0.03	N	O	1996-05-09	1996-06-13	1996-06-02	NONE                     	REG AIR   	sly pending requests int
3428	118	9	2	35	35633.85	0.02	0.03	N	O	1996-05-01	1996-06-07	1996-05-20	COLLECT COD              	TRUCK     	ly regular pinto beans sleep
3428	136	7	3	47	48698.11	0.07	0.05	N	O	1996-04-16	1996-06-08	1996-05-05	NONE                     	REG AIR   	y final pinto 
3429	137	8	1	48	49782.24	0.06	0.02	N	O	1997-04-08	1997-03-09	1997-04-25	TAKE BACK RETURN         	SHIP      	 haggle furiously ir
3429	59	7	2	15	14385.75	0.03	0.04	N	O	1997-02-04	1997-03-09	1997-03-01	TAKE BACK RETURN         	TRUCK     	beans are fu
3429	69	4	3	10	9690.60	0.05	0.07	N	O	1997-01-19	1997-02-22	1997-01-25	TAKE BACK RETURN         	REG AIR   	ackages. quickly e
3429	89	10	4	28	27694.24	0.10	0.07	N	O	1997-01-30	1997-03-18	1997-02-17	TAKE BACK RETURN         	AIR       	nstructions boost. thin
3429	165	6	5	45	47932.20	0.10	0.00	N	O	1997-04-21	1997-03-08	1997-05-05	COLLECT COD              	REG AIR   	ites poach a
3430	189	10	1	2	2178.36	0.07	0.06	R	F	1995-03-07	1995-01-28	1995-03-30	TAKE BACK RETURN         	MAIL      	sh furiously according to the evenly e
3430	81	2	2	32	31394.56	0.08	0.00	R	F	1995-01-17	1995-01-28	1995-02-06	NONE                     	TRUCK     	egular instruction
3430	97	8	3	41	40880.69	0.06	0.04	R	F	1995-02-18	1995-02-21	1995-03-11	TAKE BACK RETURN         	AIR       	cuses. silent excuses h
3430	65	2	4	50	48253.00	0.01	0.00	R	F	1994-12-15	1995-03-03	1994-12-24	COLLECT COD              	REG AIR   	ironic theodolites. carefully regular pac
3430	95	9	5	5	4975.45	0.05	0.05	A	F	1995-04-02	1995-02-12	1995-04-08	DELIVER IN PERSON        	FOB       	even accounts haggle slyly bol
3430	171	10	6	15	16067.55	0.08	0.07	A	F	1995-02-01	1995-03-12	1995-02-04	COLLECT COD              	SHIP      	cajole around the accounts. qui
3430	52	7	7	23	21897.15	0.09	0.08	A	F	1995-03-06	1995-03-01	1995-03-10	COLLECT COD              	MAIL      	eas according to the
3431	180	8	1	41	44287.38	0.03	0.06	A	F	1993-09-26	1993-10-13	1993-10-22	NONE                     	AIR       	 sleep carefully ironically special
3456	111	8	1	34	34377.74	0.10	0.06	A	F	1993-08-29	1993-08-26	1993-09-07	TAKE BACK RETURN         	SHIP      	usy pinto beans b
3457	182	3	1	29	31383.22	0.03	0.02	R	F	1995-05-12	1995-07-13	1995-06-05	NONE                     	TRUCK     	refully final excuses wake
3457	106	7	2	22	22134.20	0.06	0.01	N	O	1995-06-23	1995-06-16	1995-06-29	NONE                     	SHIP      	packages nag furiously against
3457	109	2	3	7	7063.70	0.07	0.08	N	O	1995-08-14	1995-07-06	1995-08-18	COLLECT COD              	SHIP      	 pending accounts along the
3457	1	2	4	24	21624.00	0.07	0.07	N	O	1995-08-03	1995-05-30	1995-08-14	TAKE BACK RETURN         	REG AIR   	tructions haggle alongsid
3457	109	4	5	42	42382.20	0.05	0.01	A	F	1995-06-12	1995-06-14	1995-06-14	COLLECT COD              	MAIL      	riously final instruc
3457	144	1	6	45	46986.30	0.08	0.01	N	O	1995-08-12	1995-07-18	1995-08-23	TAKE BACK RETURN         	SHIP      	 packages. care
3457	167	4	7	9	9604.44	0.04	0.00	R	F	1995-05-29	1995-06-30	1995-06-12	DELIVER IN PERSON        	FOB       	quests. foxes sleep quickly
3459	179	7	1	31	33454.27	0.06	0.01	A	F	1994-09-05	1994-10-20	1994-10-03	NONE                     	REG AIR   	y regular pain
3459	130	9	2	30	30903.90	0.04	0.08	R	F	1994-11-22	1994-09-12	1994-12-11	NONE                     	REG AIR   	nic theodolites; evenly i
3459	41	8	3	45	42346.80	0.04	0.05	A	F	1994-07-31	1994-09-09	1994-08-02	TAKE BACK RETURN         	REG AIR   	ntly speci
3459	69	10	4	10	9690.60	0.05	0.06	A	F	1994-10-06	1994-09-16	1994-11-03	TAKE BACK RETURN         	REG AIR   	 furiously silent dolphi
3459	189	10	5	10	10891.80	0.02	0.02	R	F	1994-08-01	1994-10-17	1994-08-11	TAKE BACK RETURN         	FOB       	. blithely ironic pinto beans above
3488	160	5	1	1	1060.16	0.04	0.01	A	F	1995-03-06	1995-02-16	1995-03-23	DELIVER IN PERSON        	FOB       	 final excuses. carefully even waters hagg
3488	104	9	2	48	48196.80	0.00	0.03	A	F	1995-03-29	1995-03-26	1995-04-28	COLLECT COD              	SHIP      	sly? final requests 
3488	160	1	3	11	11661.76	0.03	0.08	R	F	1995-03-25	1995-02-08	1995-04-16	COLLECT COD              	TRUCK     	unusual re
3488	42	9	4	12	11304.48	0.05	0.07	R	F	1995-04-27	1995-02-16	1995-05-09	DELIVER IN PERSON        	RAIL      	e slyly; furiously final packages wak
3488	156	1	5	18	19010.70	0.09	0.06	A	F	1995-03-18	1995-03-19	1995-03-29	DELIVER IN PERSON        	FOB       	s the carefully r
3490	92	6	1	43	42659.87	0.05	0.05	N	O	1997-08-04	1997-08-06	1997-08-14	TAKE BACK RETURN         	SHIP      	. even requests cajol
3490	86	7	2	50	49304.00	0.05	0.07	N	O	1997-06-27	1997-08-15	1997-06-28	NONE                     	RAIL      	 haggle carefu
3490	93	7	3	8	7944.72	0.10	0.04	N	O	1997-08-11	1997-07-25	1997-08-28	COLLECT COD              	MAIL      	inal deposits use furiousl
3492	156	7	1	3	3168.45	0.02	0.08	R	F	1994-11-26	1994-12-28	1994-12-19	COLLECT COD              	REG AIR   	the deposits. carefully 
3492	126	9	2	7	7182.84	0.04	0.00	R	F	1995-03-10	1995-01-03	1995-03-16	COLLECT COD              	FOB       	thely regular dolphi
3492	109	10	3	34	34309.40	0.05	0.06	A	F	1994-12-07	1994-12-29	1994-12-24	COLLECT COD              	AIR       	 unusual requests. ir
3492	147	6	4	30	31414.20	0.02	0.06	A	F	1995-01-29	1995-01-02	1995-02-13	DELIVER IN PERSON        	MAIL      	 detect furiously permanent, unusual accou
3492	122	1	5	47	48039.64	0.09	0.07	R	F	1995-03-24	1994-12-28	1995-03-29	NONE                     	REG AIR   	deposits. quickly express 
3492	22	7	6	47	43334.94	0.04	0.07	R	F	1994-12-12	1995-01-18	1994-12-26	COLLECT COD              	RAIL      	ronic instructions u
3494	117	1	1	40	40684.40	0.05	0.04	R	F	1993-07-10	1993-06-01	1993-07-25	TAKE BACK RETURN         	TRUCK     	lites haggle furiously about the fin
3494	75	6	2	23	22426.61	0.10	0.01	A	F	1993-06-19	1993-06-04	1993-07-14	NONE                     	FOB       	osits nag 
3494	198	2	3	40	43927.60	0.02	0.08	A	F	1993-05-30	1993-07-02	1993-06-20	TAKE BACK RETURN         	MAIL      	uests cajole blithely
3494	77	8	4	30	29312.10	0.04	0.03	R	F	1993-07-01	1993-06-08	1993-07-15	TAKE BACK RETURN         	TRUCK     	ns are quickly regular, 
3524	137	8	1	5	5185.65	0.01	0.04	R	F	1992-05-23	1992-07-25	1992-06-19	DELIVER IN PERSON        	RAIL      	ts whithout the bold depende
3524	143	6	2	17	17733.38	0.09	0.08	A	F	1992-09-01	1992-07-17	1992-09-05	DELIVER IN PERSON        	FOB       	g, final epitaphs about the pinto 
3526	98	9	1	11	10978.99	0.02	0.03	R	F	1995-05-23	1995-05-28	1995-05-24	NONE                     	TRUCK     	ges. furiously regular d
3526	117	7	2	23	23393.53	0.03	0.04	A	F	1995-05-01	1995-05-31	1995-05-25	DELIVER IN PERSON        	FOB       	special, regular packages cajole. 
3526	33	9	3	20	18660.60	0.05	0.08	N	F	1995-06-16	1995-04-26	1995-06-22	DELIVER IN PERSON        	REG AIR   	kages. bold, special requests detect sl
3527	102	7	1	47	47098.70	0.07	0.02	N	O	1997-07-14	1997-07-29	1997-07-21	DELIVER IN PERSON        	RAIL      	unts. express re
3527	26	9	2	33	30558.66	0.01	0.02	N	O	1997-09-25	1997-09-17	1997-10-12	NONE                     	FOB       	kly alongside of 
3527	162	7	3	50	53108.00	0.09	0.07	N	O	1997-07-17	1997-08-03	1997-07-29	DELIVER IN PERSON        	SHIP      	e even accounts was about th
3527	128	3	4	17	17478.04	0.02	0.05	N	O	1997-07-30	1997-09-01	1997-08-17	COLLECT COD              	MAIL      	ular instruction
3553	143	10	1	4	4172.56	0.05	0.01	R	F	1994-06-13	1994-07-10	1994-07-03	COLLECT COD              	RAIL      	olites boost bli
3553	65	4	2	26	25091.56	0.05	0.08	A	F	1994-08-06	1994-07-30	1994-08-23	DELIVER IN PERSON        	MAIL      	fily special p
3553	22	5	3	18	16596.36	0.04	0.03	A	F	1994-07-03	1994-06-30	1994-07-07	COLLECT COD              	RAIL      	. quickly ironic
3553	32	8	4	40	37281.20	0.06	0.00	A	F	1994-09-14	1994-06-26	1994-09-25	NONE                     	RAIL      	 slyly pending asymptotes against the furi
3553	157	2	5	36	38057.40	0.06	0.08	R	F	1994-08-12	1994-06-25	1994-09-06	DELIVER IN PERSON        	TRUCK     	 realms. pending, bold theodolites 
3555	166	3	1	11	11727.76	0.05	0.02	N	O	1996-09-25	1996-10-01	1996-10-03	NONE                     	FOB       	oost caref
3555	79	10	2	15	14686.05	0.03	0.08	N	O	1996-07-13	1996-09-01	1996-08-02	TAKE BACK RETURN         	RAIL      	y across the pending a
3555	43	2	3	25	23576.00	0.09	0.07	N	O	1996-10-01	1996-08-23	1996-10-24	TAKE BACK RETURN         	MAIL      	sual packages. quickly 
3555	5	6	4	19	17195.00	0.00	0.05	N	O	1996-09-08	1996-09-14	1996-10-01	COLLECT COD              	REG AIR   	leep special theodolit
3555	33	4	5	29	27057.87	0.07	0.04	N	O	1996-08-02	1996-09-04	1996-08-08	DELIVER IN PERSON        	TRUCK     	deas. carefully s
3555	28	3	6	33	30624.66	0.04	0.08	N	O	1996-09-20	1996-09-23	1996-10-05	TAKE BACK RETURN         	AIR       	fluffily regular a
3555	126	5	7	9	9235.08	0.07	0.02	N	O	1996-10-13	1996-10-02	1996-10-22	NONE                     	SHIP      	are. slyly final foxes acro
3556	142	9	1	45	46896.30	0.05	0.06	A	F	1992-10-14	1992-12-21	1992-10-16	NONE                     	TRUCK     	ckages boost quickl
3556	31	2	2	43	40034.29	0.02	0.06	R	F	1993-01-18	1992-11-09	1993-02-04	NONE                     	FOB       	wake carefull
3556	87	8	3	28	27638.24	0.10	0.04	A	F	1993-01-06	1992-11-27	1993-01-16	NONE                     	MAIL      	refully final instructions? ironic packa
3558	87	8	1	8	7896.64	0.01	0.03	N	O	1996-05-31	1996-05-26	1996-06-25	COLLECT COD              	AIR       	? even requests sle
3558	10	7	2	28	25480.28	0.02	0.08	N	O	1996-06-02	1996-04-18	1996-06-24	COLLECT COD              	TRUCK     	l deposits 
3558	187	8	3	3	3261.54	0.03	0.06	N	O	1996-05-19	1996-04-28	1996-05-26	DELIVER IN PERSON        	RAIL      	l, final deposits haggle. fina
3558	91	5	4	22	21803.98	0.06	0.03	N	O	1996-04-27	1996-04-19	1996-04-30	DELIVER IN PERSON        	SHIP      	refully ironic theodolites are fu
3558	29	8	5	38	35302.76	0.03	0.08	N	O	1996-05-29	1996-05-02	1996-06-09	COLLECT COD              	RAIL      	refully permanently iron
3558	72	1	6	17	16525.19	0.07	0.07	N	O	1996-03-14	1996-05-04	1996-04-05	NONE                     	RAIL      	ithely unusual packa
3559	90	1	1	29	28712.61	0.00	0.07	R	F	1992-12-10	1992-12-03	1992-12-20	COLLECT COD              	REG AIR   	l, regular accounts wake flu
3584	160	8	2	23	24383.68	0.00	0.03	N	O	1997-09-10	1997-10-15	1997-09-30	COLLECT COD              	TRUCK     	l platelets until the asymptotes 
3584	24	5	3	6	5544.12	0.03	0.06	N	O	1997-10-28	1997-11-09	1997-11-24	TAKE BACK RETURN         	MAIL      	deposits across the
3584	146	5	4	11	11507.54	0.06	0.02	N	O	1997-11-27	1997-10-15	1997-12-08	NONE                     	REG AIR   	lithely slyly 
3584	18	5	5	39	35802.39	0.09	0.07	N	O	1997-09-20	1997-10-31	1997-10-06	COLLECT COD              	AIR       	eposits. carefu
3585	122	1	1	21	21464.52	0.05	0.04	A	F	1994-12-04	1994-12-25	1995-01-01	TAKE BACK RETURN         	TRUCK     	ounts use. express, final platelets us
3585	19	10	2	40	36760.40	0.03	0.00	R	F	1995-01-22	1995-01-17	1995-02-07	TAKE BACK RETURN         	RAIL      	elets affix. even asymptotes play care
3585	112	2	3	11	11133.21	0.01	0.04	R	F	1995-01-04	1995-02-14	1995-01-15	NONE                     	MAIL      	even packages
3585	48	1	4	33	31285.32	0.08	0.08	A	F	1994-12-14	1995-01-19	1994-12-22	NONE                     	RAIL      	ironic dependencies serve furi
3585	25	8	5	13	12025.26	0.06	0.07	R	F	1995-03-15	1995-01-22	1995-03-17	DELIVER IN PERSON        	AIR       	ccording to the foxes. slyly iro
3585	94	7	6	7	6958.63	0.10	0.02	A	F	1994-12-13	1995-01-20	1995-01-05	TAKE BACK RETURN         	TRUCK     	dependencies sleep un
3585	42	1	7	45	42391.80	0.03	0.00	A	F	1995-01-20	1995-02-19	1995-02-11	DELIVER IN PERSON        	MAIL      	are blithely c
3588	91	5	1	28	27750.52	0.04	0.08	R	F	1995-05-03	1995-05-03	1995-05-14	DELIVER IN PERSON        	TRUCK     	special pinto beans cajole slyly. slyly 
3588	88	9	2	6	5928.48	0.06	0.08	A	F	1995-04-09	1995-05-30	1995-04-10	TAKE BACK RETURN         	MAIL      	s. fluffily fluf
3588	159	10	3	45	47661.75	0.04	0.02	R	F	1995-05-07	1995-05-04	1995-05-28	TAKE BACK RETURN         	TRUCK     	ecial pains integrate blithely. reques
3588	127	10	4	22	22596.64	0.05	0.00	A	F	1995-04-08	1995-05-06	1995-04-27	NONE                     	RAIL      	inal accounts. pending, bo
3588	55	3	5	28	26741.40	0.03	0.03	A	F	1995-04-23	1995-05-25	1995-04-28	DELIVER IN PERSON        	TRUCK     	 express sheaves. unusual theodo
3588	110	3	6	37	37374.07	0.08	0.04	N	F	1995-06-17	1995-05-25	1995-06-24	TAKE BACK RETURN         	RAIL      	xcuses sleep quickly along th
3588	39	5	7	46	43195.38	0.08	0.07	A	F	1995-06-06	1995-05-08	1995-06-08	NONE                     	AIR       	 slyly ironic deposits sublate ab
3591	29	8	1	21	19509.42	0.03	0.03	A	F	1994-02-25	1994-02-02	1994-03-05	DELIVER IN PERSON        	TRUCK     	structions against 
3591	69	6	2	24	23257.44	0.04	0.04	R	F	1993-12-26	1994-01-07	1994-01-25	COLLECT COD              	FOB       	ages. slyly regular dependencies cajo
3591	164	9	3	4	4256.64	0.01	0.03	A	F	1994-04-04	1994-02-19	1994-05-02	DELIVER IN PERSON        	RAIL      	he final packages. deposits serve quick
3591	153	4	4	49	51604.35	0.01	0.00	A	F	1994-03-21	1994-01-26	1994-03-28	COLLECT COD              	AIR       	 mold slyly. bl
3616	197	9	1	30	32915.70	0.01	0.00	A	F	1994-05-05	1994-04-24	1994-05-12	TAKE BACK RETURN         	FOB       	ly ironic accounts unwind b
3616	138	9	2	28	29067.64	0.08	0.06	R	F	1994-02-20	1994-04-18	1994-03-05	DELIVER IN PERSON        	REG AIR   	ironic packages. furiously ev
3617	117	8	1	46	46787.06	0.03	0.02	N	O	1996-05-19	1996-05-14	1996-06-11	NONE                     	RAIL      	ar theodolites. regu
3617	98	9	2	16	15969.44	0.05	0.02	N	O	1996-05-08	1996-06-03	1996-05-19	COLLECT COD              	RAIL      	 slyly on th
3617	98	2	3	32	31938.88	0.00	0.06	N	O	1996-04-20	1996-06-07	1996-05-19	DELIVER IN PERSON        	MAIL      	uriously against the express accounts. ex
3617	41	10	4	22	20702.88	0.10	0.05	N	O	1996-07-11	1996-05-02	1996-07-25	NONE                     	REG AIR   	uffily even accounts. packages sleep blithe
3617	137	8	5	11	11408.43	0.08	0.05	N	O	1996-07-16	1996-04-23	1996-07-28	COLLECT COD              	MAIL      	ly quickly even requests. final
3618	140	1	1	38	39525.32	0.08	0.00	N	O	1997-12-22	1998-02-23	1998-01-03	TAKE BACK RETURN         	TRUCK     	nts haggle fluffily above the regular 
3618	144	5	2	48	50118.72	0.04	0.00	N	O	1998-03-12	1998-02-13	1998-03-29	DELIVER IN PERSON        	TRUCK     	tructions atop the ironi
3618	63	2	3	24	23113.44	0.01	0.04	N	O	1998-01-26	1998-01-15	1998-02-17	TAKE BACK RETURN         	AIR       	xpress acc
3618	161	2	4	26	27590.16	0.01	0.05	N	O	1998-03-23	1998-01-24	1998-04-15	DELIVER IN PERSON        	AIR       	iously regular deposits cajole ruthless
3621	17	8	1	29	26593.29	0.02	0.06	A	F	1993-08-03	1993-07-08	1993-08-10	DELIVER IN PERSON        	FOB       	al requests. fl
3621	93	5	2	13	12910.17	0.09	0.04	R	F	1993-08-30	1993-06-30	1993-09-01	NONE                     	REG AIR   	r the unusual packages. brave theodoli
3621	164	9	3	45	47887.20	0.07	0.07	R	F	1993-08-09	1993-06-18	1993-09-05	DELIVER IN PERSON        	AIR       	 doubt about the bold deposits. carefully
3621	44	3	4	20	18880.80	0.05	0.04	R	F	1993-05-27	1993-07-04	1993-06-22	TAKE BACK RETURN         	SHIP      	gular accounts use carefully with
3622	175	6	1	47	50532.99	0.09	0.00	N	O	1996-02-24	1996-02-22	1996-03-12	TAKE BACK RETURN         	TRUCK     	are careful
3622	89	10	2	4	3956.32	0.04	0.04	N	O	1996-02-03	1996-02-19	1996-02-16	TAKE BACK RETURN         	TRUCK     	lithely brave foxes. furi
3622	190	1	3	46	50148.74	0.07	0.07	N	O	1995-12-18	1996-01-23	1996-01-12	TAKE BACK RETURN         	AIR       	sits wake. blithe
3622	177	8	4	9	9694.53	0.08	0.05	N	O	1995-12-12	1996-02-09	1995-12-13	TAKE BACK RETURN         	SHIP      	arefully. furiously regular ideas n
3623	80	10	1	32	31362.56	0.05	0.00	N	O	1997-04-18	1997-03-15	1997-05-09	COLLECT COD              	SHIP      	 courts. furiously regular ideas b
3623	117	4	2	33	33564.63	0.08	0.01	N	O	1997-03-17	1997-02-13	1997-04-02	TAKE BACK RETURN         	TRUCK     	odolites. blithely spe
3623	24	7	3	21	19404.42	0.02	0.02	N	O	1997-01-19	1997-03-18	1997-01-24	NONE                     	FOB       	ress ideas are furio
3623	165	2	4	42	44736.72	0.05	0.06	N	O	1997-01-11	1997-03-24	1997-01-21	COLLECT COD              	RAIL      	g to the slyly regular packa
3623	88	9	5	30	29642.40	0.10	0.04	N	O	1997-04-04	1997-03-03	1997-05-01	NONE                     	RAIL      	 ironic somas sleep fluffily
3623	186	7	6	7	7603.26	0.01	0.02	N	O	1997-01-05	1997-03-26	1997-01-26	NONE                     	TRUCK     	aves. slyly special packages cajole. fu
3623	140	6	7	13	13521.82	0.03	0.08	N	O	1997-01-02	1997-02-26	1997-01-26	DELIVER IN PERSON        	SHIP      	deas. furiously expres
3649	5	6	1	25	22625.00	0.10	0.04	A	F	1994-10-27	1994-08-23	1994-11-05	TAKE BACK RETURN         	TRUCK     	special re
3649	89	10	2	23	22748.84	0.08	0.00	R	F	1994-09-26	1994-10-01	1994-09-28	NONE                     	REG AIR   	rs promise blithe
3649	70	7	3	14	13580.98	0.02	0.04	A	F	1994-09-19	1994-08-17	1994-10-12	DELIVER IN PERSON        	TRUCK     	ithely bold accounts wake 
3649	76	4	4	40	39042.80	0.00	0.08	R	F	1994-07-20	1994-08-30	1994-08-14	TAKE BACK RETURN         	RAIL      	luffy somas sleep quickly-- ironic de
3649	100	1	5	24	24002.40	0.05	0.03	A	F	1994-07-07	1994-08-20	1994-07-27	TAKE BACK RETURN         	FOB       	c accounts. quickly final theodo
3649	122	3	6	3	3066.36	0.10	0.04	A	F	1994-07-17	1994-08-10	1994-08-03	NONE                     	FOB       	lly bold requests nag; 
3650	136	2	1	30	31083.90	0.10	0.00	A	F	1992-08-26	1992-07-05	1992-09-01	DELIVER IN PERSON        	SHIP      	ckly special platelets. furiously sil
3650	128	9	2	43	44209.16	0.05	0.05	A	F	1992-09-07	1992-08-12	1992-09-10	COLLECT COD              	TRUCK     	gside of the quick
3650	2	9	3	1	902.00	0.04	0.06	A	F	1992-06-23	1992-07-18	1992-07-08	NONE                     	REG AIR   	re about the pinto 
3650	63	2	4	31	29854.86	0.10	0.08	R	F	1992-06-15	1992-07-01	1992-07-15	DELIVER IN PERSON        	RAIL      	 against the ironic accounts cajol
3650	187	8	5	19	20656.42	0.05	0.04	R	F	1992-08-29	1992-08-09	1992-09-21	DELIVER IN PERSON        	AIR       	y even forges. fluffily furious accounts
3650	94	8	6	27	26840.43	0.07	0.08	A	F	1992-07-03	1992-07-23	1992-07-13	COLLECT COD              	MAIL      	ular requests snooze fluffily regular pi
3650	70	7	7	43	41713.01	0.10	0.07	A	F	1992-06-25	1992-07-09	1992-07-22	DELIVER IN PERSON        	RAIL      	structions use caref
3653	145	4	1	38	39715.32	0.08	0.05	A	F	1994-06-26	1994-05-13	1994-07-13	NONE                     	REG AIR   	ainst the 
3653	64	1	2	29	27957.74	0.07	0.01	A	F	1994-04-11	1994-06-11	1994-04-29	COLLECT COD              	RAIL      	ording to the special, final
3653	181	2	3	17	18380.06	0.09	0.03	R	F	1994-06-24	1994-06-02	1994-07-17	DELIVER IN PERSON        	RAIL      	gle slyly regular
3653	186	7	4	9	9775.62	0.10	0.07	R	F	1994-04-03	1994-05-19	1994-04-10	COLLECT COD              	FOB       	slyly silent account
3653	188	9	5	41	44615.38	0.08	0.01	A	F	1994-06-18	1994-05-18	1994-06-20	COLLECT COD              	RAIL      	onic packages affix sly
3653	43	4	6	9	8487.36	0.05	0.03	A	F	1994-07-21	1994-05-31	1994-08-17	NONE                     	MAIL      	tes: blithely bo
3653	49	6	7	2	1898.08	0.06	0.03	R	F	1994-06-02	1994-05-31	1994-06-29	NONE                     	FOB       	n accounts. fina
3655	184	5	1	5	5420.90	0.03	0.04	R	F	1993-01-17	1992-12-31	1993-01-23	DELIVER IN PERSON        	TRUCK     	riously bold pinto be
3655	97	10	2	1	997.09	0.10	0.06	R	F	1992-10-24	1992-12-18	1992-11-07	DELIVER IN PERSON        	AIR       	arefully slow pinto beans are
3655	30	5	3	35	32551.05	0.01	0.04	R	F	1992-12-20	1992-11-16	1993-01-15	TAKE BACK RETURN         	MAIL      	blithely even accounts! furiously regular
3655	72	3	4	35	34022.45	0.04	0.07	R	F	1992-10-17	1992-12-23	1992-10-28	COLLECT COD              	MAIL      	ng foxes cajole fluffily slyly final fo
3681	106	9	1	35	35213.50	0.03	0.08	R	F	1992-07-31	1992-05-18	1992-08-07	COLLECT COD              	FOB       	lyly special pinto 
3684	126	7	1	48	49253.76	0.04	0.06	A	F	1993-08-20	1993-09-02	1993-09-10	DELIVER IN PERSON        	REG AIR   	its boost alongside
3684	46	7	2	6	5676.24	0.06	0.08	R	F	1993-08-09	1993-10-05	1993-09-06	DELIVER IN PERSON        	FOB       	he silent requests. packages sleep fu
3684	163	8	3	19	20200.04	0.04	0.02	A	F	1993-10-19	1993-08-25	1993-11-02	COLLECT COD              	FOB       	e slyly carefully pending foxes. d
3684	135	1	4	13	13456.69	0.02	0.05	A	F	1993-07-23	1993-09-16	1993-08-06	NONE                     	TRUCK     	ing, unusual pinto beans! thinly p
3685	47	4	1	37	35040.48	0.02	0.03	R	F	1992-03-11	1992-04-09	1992-04-05	DELIVER IN PERSON        	TRUCK     	ress attai
3685	58	6	2	7	6706.35	0.05	0.00	R	F	1992-05-16	1992-02-23	1992-05-17	DELIVER IN PERSON        	FOB       	sits. special asymptotes about the r
3685	134	5	3	38	39296.94	0.08	0.03	A	F	1992-05-17	1992-03-16	1992-06-06	TAKE BACK RETURN         	TRUCK     	thely unusual pack
3685	192	5	4	39	42595.41	0.10	0.05	R	F	1992-02-19	1992-04-06	1992-03-02	COLLECT COD              	FOB       	ic courts nag carefully after the 
3685	56	7	5	37	35373.85	0.00	0.01	A	F	1992-03-02	1992-04-10	1992-03-04	NONE                     	FOB       	. carefully sly requests are regular, regu
3686	122	5	1	7	7154.84	0.02	0.04	N	O	1998-07-15	1998-08-22	1998-07-30	DELIVER IN PERSON        	TRUCK     	 furiously unusual accou
3686	200	2	2	38	41807.60	0.06	0.03	N	O	1998-09-04	1998-08-11	1998-09-19	DELIVER IN PERSON        	AIR       	y silent foxes! carefully ruthless cour
3686	45	6	3	31	29296.24	0.10	0.06	N	O	1998-09-09	1998-08-28	1998-10-09	COLLECT COD              	MAIL      	gle across the courts. furiously regu
3686	117	1	4	7	7119.77	0.10	0.01	N	O	1998-07-16	1998-09-02	1998-07-22	NONE                     	FOB       	ake carefully carefully q
3712	141	4	1	27	28110.78	0.01	0.05	R	F	1992-02-01	1992-02-26	1992-03-02	TAKE BACK RETURN         	SHIP      	ctions. even accounts haggle alongside 
3712	185	6	2	13	14107.34	0.03	0.03	R	F	1992-04-30	1992-02-11	1992-05-30	DELIVER IN PERSON        	FOB       	s around the furiously ironic account
3712	64	1	3	44	42418.64	0.01	0.01	A	F	1992-03-26	1992-02-19	1992-04-18	TAKE BACK RETURN         	FOB       	ously permanently regular req
3712	148	7	4	38	39829.32	0.01	0.06	A	F	1992-01-15	1992-03-24	1992-01-27	COLLECT COD              	RAIL      	s nag carefully-- even, reg
3714	69	6	1	13	12597.78	0.07	0.03	N	O	1998-06-26	1998-06-17	1998-07-07	TAKE BACK RETURN         	REG AIR   	 the furiously final
3714	146	3	2	14	14645.96	0.02	0.05	N	O	1998-05-30	1998-06-30	1998-05-31	DELIVER IN PERSON        	RAIL      	ending ideas. thinly unusual theodo
3714	159	10	3	16	16946.40	0.00	0.02	N	O	1998-05-25	1998-07-07	1998-06-17	TAKE BACK RETURN         	AIR       	ccounts cajole fu
3714	30	9	4	44	40921.32	0.04	0.02	N	O	1998-07-18	1998-07-10	1998-07-22	DELIVER IN PERSON        	AIR       	s. quickly ironic dugouts sublat
3717	153	8	1	45	47391.75	0.07	0.04	N	O	1998-08-09	1998-08-18	1998-08-14	TAKE BACK RETURN         	TRUCK     	ests wake whithout the blithely final pl
3717	53	5	2	3	2859.15	0.01	0.07	N	O	1998-06-09	1998-07-31	1998-06-14	NONE                     	REG AIR   	nside the regular packages sleep
3717	196	7	3	45	49328.55	0.05	0.08	N	O	1998-09-19	1998-07-22	1998-09-28	DELIVER IN PERSON        	MAIL      	s the blithely unu
3717	69	6	4	5	4845.30	0.06	0.03	N	O	1998-09-02	1998-08-20	1998-09-26	TAKE BACK RETURN         	AIR       	quickly among 
3717	16	7	5	7	6412.07	0.09	0.02	N	O	1998-09-08	1998-07-18	1998-09-10	DELIVER IN PERSON        	RAIL      	 after the packa
3717	64	1	6	38	36634.28	0.01	0.07	N	O	1998-07-10	1998-07-08	1998-07-29	COLLECT COD              	RAIL      	ly about the car
3717	106	7	7	28	28170.80	0.03	0.01	N	O	1998-07-25	1998-08-12	1998-08-16	COLLECT COD              	RAIL      	ts sleep q
3746	165	6	1	37	39410.92	0.07	0.00	A	F	1994-12-29	1994-10-25	1995-01-03	COLLECT COD              	FOB       	e of the careful
3746	144	7	2	28	29235.92	0.06	0.08	R	F	1994-09-20	1994-10-21	1994-09-27	DELIVER IN PERSON        	FOB       	s after the even, special requests
3746	188	9	3	3	3264.54	0.10	0.01	R	F	1994-11-03	1994-12-10	1994-11-12	NONE                     	MAIL      	 the silent ideas cajole carefully 
3746	28	7	4	11	10208.22	0.00	0.05	R	F	1994-10-02	1994-11-19	1994-10-10	COLLECT COD              	SHIP      	 ironic theodolites are among th
3749	173	3	1	11	11804.87	0.07	0.05	N	O	1995-06-25	1995-05-23	1995-07-10	TAKE BACK RETURN         	RAIL      	egular requests along the 
3749	129	8	2	9	9262.08	0.08	0.05	A	F	1995-04-23	1995-04-18	1995-04-26	NONE                     	REG AIR   	uses cajole blithely pla
3749	199	2	3	31	34074.89	0.00	0.05	N	F	1995-06-11	1995-05-20	1995-06-27	COLLECT COD              	REG AIR   	s. foxes sleep slyly unusual grouc
3749	131	2	4	7	7217.91	0.07	0.06	A	F	1995-03-31	1995-04-05	1995-04-11	NONE                     	TRUCK     	he slyly ironic packages
3749	183	4	5	14	15164.52	0.02	0.00	N	F	1995-06-11	1995-05-19	1995-07-11	DELIVER IN PERSON        	SHIP      	press instruc
3749	54	6	6	10	9540.50	0.10	0.03	N	O	1995-06-24	1995-05-24	1995-07-18	COLLECT COD              	SHIP      	essly. regular pi
3751	172	2	1	37	39670.29	0.00	0.04	R	F	1994-04-30	1994-05-30	1994-05-30	NONE                     	REG AIR   	ly express courts 
3751	141	8	2	32	33316.48	0.03	0.05	R	F	1994-05-05	1994-07-02	1994-06-02	COLLECT COD              	MAIL      	rthogs could have to slee
3751	65	2	3	45	43427.70	0.08	0.06	R	F	1994-05-27	1994-06-19	1994-06-14	NONE                     	RAIL      	according to 
3751	14	4	4	39	35646.39	0.07	0.01	A	F	1994-08-16	1994-07-11	1994-09-12	COLLECT COD              	TRUCK     	refully according to the iro
3751	58	3	5	12	11496.60	0.02	0.03	A	F	1994-08-09	1994-06-30	1994-08-12	TAKE BACK RETURN         	TRUCK     	accounts wake furious
3751	76	5	6	39	38066.73	0.02	0.08	R	F	1994-08-01	1994-06-01	1994-08-26	COLLECT COD              	SHIP      	to beans. pending, express packages c
3776	3	10	1	39	35217.00	0.05	0.01	R	F	1993-01-03	1993-02-05	1993-01-08	COLLECT COD              	FOB       	yly blithely pending packages
3776	159	4	2	14	14828.10	0.06	0.08	R	F	1992-12-30	1993-02-12	1993-01-27	DELIVER IN PERSON        	RAIL      	y special ideas. express packages pr
3776	141	8	3	49	51015.86	0.01	0.08	R	F	1992-12-03	1993-02-16	1992-12-28	TAKE BACK RETURN         	RAIL      	equests. final, thin grouches 
3776	92	6	4	49	48612.41	0.08	0.05	A	F	1993-02-11	1993-01-06	1993-02-27	COLLECT COD              	MAIL      	es: careful warthogs haggle fluffi
3777	100	4	1	11	11001.10	0.02	0.03	A	F	1994-04-09	1994-06-05	1994-04-14	NONE                     	FOB       	ld ideas. even theodolites
3777	8	5	2	10	9080.00	0.03	0.01	R	F	1994-05-22	1994-05-29	1994-06-13	COLLECT COD              	RAIL      	le. ironic depths a
3777	166	7	3	18	19190.88	0.10	0.06	R	F	1994-05-04	1994-05-23	1994-05-22	COLLECT COD              	REG AIR   	eful packages use slyly: even deposits 
3777	18	9	4	35	32130.35	0.10	0.04	A	F	1994-05-25	1994-05-26	1994-06-13	COLLECT COD              	AIR       	s. carefully express asymptotes accordi
3777	98	10	5	14	13973.26	0.04	0.05	R	F	1994-05-06	1994-06-24	1994-05-31	NONE                     	TRUCK     	ording to the iro
3778	57	2	1	21	20098.05	0.01	0.06	R	F	1993-05-27	1993-07-10	1993-06-03	COLLECT COD              	REG AIR   	ts. blithely special theodoli
3778	29	10	2	32	29728.64	0.09	0.00	A	F	1993-06-22	1993-08-18	1993-07-03	TAKE BACK RETURN         	MAIL      	tes affix carefully above the 
3778	94	6	3	41	40757.69	0.05	0.00	R	F	1993-06-21	1993-07-27	1993-07-15	COLLECT COD              	FOB       	e the furiously ironi
3778	169	4	4	28	29936.48	0.03	0.05	R	F	1993-08-18	1993-07-10	1993-09-06	TAKE BACK RETURN         	REG AIR   	y silent orbits print carefully against 
3778	98	2	5	28	27946.52	0.01	0.06	R	F	1993-09-02	1993-08-08	1993-10-02	DELIVER IN PERSON        	FOB       	r deposits. theodol
3778	20	7	6	26	23920.52	0.00	0.01	A	F	1993-09-24	1993-07-06	1993-10-22	NONE                     	TRUCK     	 against the fluffily
3778	105	6	7	49	49249.90	0.02	0.04	A	F	1993-06-13	1993-08-08	1993-07-04	DELIVER IN PERSON        	MAIL      	ans. furiously 
3779	46	5	1	28	26489.12	0.04	0.05	N	O	1997-05-06	1997-04-01	1997-05-18	TAKE BACK RETURN         	AIR       	s. close requests sleep
3779	110	3	2	5	5050.55	0.07	0.03	N	O	1997-01-07	1997-03-26	1997-02-05	DELIVER IN PERSON        	AIR       	heodolites. slyly regular a
3780	127	8	1	25	25678.00	0.08	0.04	N	O	1996-06-27	1996-07-02	1996-07-22	NONE                     	AIR       	l, unusual 
3780	190	1	2	40	43607.60	0.10	0.04	N	O	1996-06-06	1996-05-29	1996-07-01	COLLECT COD              	SHIP      	gular deposits-- furiously regular 
3781	14	5	1	48	43872.48	0.02	0.06	N	O	1996-08-22	1996-08-13	1996-09-15	NONE                     	REG AIR   	equests may cajole careful
3781	188	9	2	39	42439.02	0.10	0.00	N	O	1996-08-20	1996-08-16	1996-09-01	DELIVER IN PERSON        	REG AIR   	unts are carefully. ir
3781	30	1	3	17	15810.51	0.01	0.03	N	O	1996-06-23	1996-09-04	1996-07-19	TAKE BACK RETURN         	REG AIR   	. theodolite
3781	31	2	4	15	13965.45	0.05	0.00	N	O	1996-08-23	1996-08-08	1996-09-06	TAKE BACK RETURN         	AIR       	 carefully blithe
3781	16	6	5	23	21068.23	0.09	0.08	N	O	1996-09-05	1996-08-18	1996-09-27	DELIVER IN PERSON        	SHIP      	pendencies are b
3809	191	3	1	17	18550.23	0.10	0.04	N	O	1996-08-14	1996-07-05	1996-09-04	DELIVER IN PERSON        	FOB       	es detect furiously sil
3809	133	4	2	32	33060.16	0.01	0.02	N	O	1996-07-03	1996-06-01	1996-07-25	COLLECT COD              	SHIP      	xcuses would boost against the fluffily eve
3809	105	6	3	46	46234.60	0.10	0.06	N	O	1996-08-20	1996-06-01	1996-08-24	TAKE BACK RETURN         	TRUCK     	l asymptotes. special 
3809	178	9	4	43	46361.31	0.00	0.04	N	O	1996-05-06	1996-06-22	1996-06-05	TAKE BACK RETURN         	TRUCK     	yly ironic decoys; regular, iron
3811	164	3	1	24	25539.84	0.04	0.02	N	O	1998-07-13	1998-05-16	1998-08-12	TAKE BACK RETURN         	TRUCK     	deposits. slyly regular accounts cajo
3811	166	5	2	2	2132.32	0.01	0.08	N	O	1998-06-16	1998-06-16	1998-06-23	NONE                     	MAIL      	slyly fluff
3811	43	6	3	19	17917.76	0.02	0.06	N	O	1998-07-20	1998-06-14	1998-07-29	NONE                     	MAIL      	s boost blithely furiou
3811	171	1	4	50	53558.50	0.08	0.03	N	O	1998-07-28	1998-07-06	1998-08-16	COLLECT COD              	FOB       	ts are slyly fluffy ideas. furiou
3811	182	3	5	23	24890.14	0.00	0.04	N	O	1998-08-13	1998-07-09	1998-08-29	COLLECT COD              	AIR       	nstructions sleep quickly. slyly final 
3811	2	7	6	35	31570.00	0.04	0.07	N	O	1998-04-17	1998-06-30	1998-04-25	NONE                     	REG AIR   	yly final dolphins? quickly ironic frets
3812	145	4	1	33	34489.62	0.00	0.05	N	O	1996-10-10	1996-10-05	1996-10-15	TAKE BACK RETURN         	MAIL      	posits engage. ironic, regular p
3812	173	2	2	33	35414.61	0.06	0.03	N	O	1996-10-05	1996-10-13	1996-10-22	TAKE BACK RETURN         	MAIL      	inal excuses d
3813	176	7	1	37	39818.29	0.05	0.04	N	O	1998-10-13	1998-09-19	1998-10-28	NONE                     	REG AIR   	ravely special packages haggle p
3813	123	2	2	39	39901.68	0.05	0.00	N	O	1998-08-30	1998-08-12	1998-09-29	COLLECT COD              	FOB       	y ideas. final ideas about the sp
3815	77	7	1	3	2931.21	0.07	0.00	N	O	1997-11-16	1997-11-15	1997-11-30	NONE                     	FOB       	egular, express ideas. ironic, final dep
3815	130	5	2	11	11331.43	0.02	0.04	N	O	1997-11-01	1997-11-05	1997-11-27	COLLECT COD              	TRUCK     	sleep blithe
3842	162	7	1	28	29740.48	0.05	0.07	A	F	1992-06-17	1992-06-03	1992-06-24	DELIVER IN PERSON        	TRUCK     	s excuses thrash carefully.
3842	122	1	2	21	21464.52	0.07	0.05	R	F	1992-07-15	1992-06-02	1992-07-21	NONE                     	RAIL      	r pinto be
3842	194	7	3	28	30637.32	0.00	0.00	A	F	1992-06-20	1992-05-22	1992-07-13	DELIVER IN PERSON        	MAIL      	lly alongside of the
3842	88	9	4	15	14821.20	0.07	0.01	A	F	1992-06-26	1992-06-23	1992-07-09	COLLECT COD              	MAIL      	ave packages are slyl
3842	68	3	5	13	12584.78	0.09	0.02	R	F	1992-04-13	1992-06-22	1992-05-11	COLLECT COD              	RAIL      	t blithely. busily regular accounts alon
3842	107	4	6	24	24170.40	0.08	0.08	R	F	1992-08-05	1992-06-29	1992-08-16	TAKE BACK RETURN         	MAIL      	phins are quickly
3843	15	6	1	7	6405.07	0.10	0.03	N	O	1997-02-13	1997-02-21	1997-02-20	TAKE BACK RETURN         	SHIP      	slyly even instructions. furiously eve
3843	1	4	2	30	27030.00	0.01	0.05	N	O	1997-02-14	1997-03-25	1997-03-13	DELIVER IN PERSON        	AIR       	 wake. slyly even packages boost 
3846	61	10	1	15	14415.90	0.06	0.03	N	O	1998-02-17	1998-04-27	1998-02-21	NONE                     	REG AIR   	uternes. carefully even
3846	171	2	2	30	32135.10	0.08	0.07	N	O	1998-05-01	1998-03-12	1998-05-20	TAKE BACK RETURN         	FOB       	deposits according to the fur
3846	15	5	3	49	44835.49	0.08	0.07	N	O	1998-02-14	1998-03-22	1998-02-17	DELIVER IN PERSON        	RAIL      	efully even packages against the blithe
3846	165	10	4	33	35150.28	0.05	0.00	N	O	1998-05-12	1998-03-14	1998-05-14	DELIVER IN PERSON        	TRUCK     	s instructions are. fu
3872	181	2	1	28	30273.04	0.10	0.04	N	O	1996-11-05	1996-11-10	1996-11-24	DELIVER IN PERSON        	REG AIR   	t after the carefully ironic excuses. f
3872	17	4	2	38	34846.38	0.04	0.05	N	O	1996-10-18	1996-12-03	1996-11-15	TAKE BACK RETURN         	AIR       	iously against the ironic, unusual a
3872	169	4	3	18	19244.88	0.07	0.07	N	O	1996-12-25	1996-10-24	1997-01-08	TAKE BACK RETURN         	SHIP      	s. regular, brave accounts sleep blith
3872	11	2	4	41	37351.41	0.07	0.03	N	O	1996-11-23	1996-11-12	1996-12-03	COLLECT COD              	REG AIR   	ly regular epitaphs boost
3872	70	7	5	42	40742.94	0.03	0.00	N	O	1997-01-03	1996-10-12	1997-01-16	COLLECT COD              	MAIL      	s the furio
3872	140	6	6	40	41605.60	0.07	0.05	N	O	1997-01-02	1996-10-29	1997-01-14	NONE                     	REG AIR   	nts? regularly ironic ex
3873	68	3	1	19	18393.14	0.04	0.04	N	O	1998-05-15	1998-05-10	1998-05-17	NONE                     	FOB       	y final ac
3873	145	8	2	44	45986.16	0.05	0.05	N	O	1998-07-23	1998-05-22	1998-08-14	COLLECT COD              	AIR       	yly even platelets wake. 
3873	140	6	3	29	30164.06	0.01	0.04	N	O	1998-06-22	1998-05-20	1998-07-05	COLLECT COD              	REG AIR   	olphins af
3874	170	7	1	21	22473.57	0.09	0.08	R	F	1993-06-19	1993-07-20	1993-07-08	DELIVER IN PERSON        	SHIP      	 requests cajole fluff
3874	19	6	2	48	44112.48	0.06	0.07	R	F	1993-06-13	1993-07-20	1993-06-20	NONE                     	RAIL      	 ideas throughout 
3876	141	8	1	12	12493.68	0.06	0.07	N	O	1996-09-16	1996-10-23	1996-10-05	TAKE BACK RETURN         	REG AIR   	y above the pending tithes. blithely ironi
3876	140	6	2	37	38485.18	0.00	0.03	N	O	1996-11-30	1996-10-18	1996-12-18	DELIVER IN PERSON        	AIR       	t dependencies. blithely final packages u
3876	127	8	3	41	42111.92	0.02	0.04	N	O	1996-10-15	1996-10-17	1996-10-19	NONE                     	AIR       	 quickly blit
3877	50	7	1	12	11400.60	0.06	0.01	R	F	1993-05-30	1993-08-09	1993-06-24	TAKE BACK RETURN         	FOB       	nal requests. even requests are. pac
3877	145	4	2	47	49121.58	0.05	0.00	A	F	1993-08-01	1993-08-16	1993-08-04	NONE                     	FOB       	furiously quick requests nag along the theo
3877	80	8	3	44	43123.52	0.09	0.00	A	F	1993-06-07	1993-07-15	1993-07-06	DELIVER IN PERSON        	REG AIR   	elets. quickly regular accounts caj
3877	148	9	4	36	37733.04	0.06	0.01	A	F	1993-07-27	1993-07-13	1993-08-11	DELIVER IN PERSON        	AIR       	lithely about the dogged ideas. ac
3877	5	6	5	41	37105.00	0.03	0.07	A	F	1993-06-30	1993-07-20	1993-07-01	DELIVER IN PERSON        	FOB       	integrate against the expres
3877	123	4	6	7	7161.84	0.04	0.08	R	F	1993-06-14	1993-07-09	1993-06-28	NONE                     	TRUCK     	lar dolphins cajole silently 
3879	126	5	1	45	46175.40	0.10	0.08	N	O	1996-03-18	1996-01-03	1996-04-03	COLLECT COD              	RAIL      	ly according to the expr
3879	45	4	2	35	33076.40	0.00	0.07	N	O	1995-12-08	1996-01-23	1995-12-28	TAKE BACK RETURN         	MAIL      	o beans. accounts cajole furiously. re
3905	101	8	1	43	43047.30	0.07	0.08	A	F	1994-03-30	1994-02-18	1994-04-09	DELIVER IN PERSON        	REG AIR   	uses are care
3905	116	10	2	7	7112.77	0.03	0.00	R	F	1994-03-01	1994-02-19	1994-03-11	DELIVER IN PERSON        	AIR       	ully furiously furious packag
3905	170	7	3	6	6421.02	0.07	0.02	R	F	1994-04-07	1994-03-07	1994-04-21	DELIVER IN PERSON        	RAIL      	ow furiously. deposits wake ironic 
3906	153	1	1	42	44232.30	0.00	0.04	R	F	1992-09-03	1992-07-22	1992-09-04	COLLECT COD              	RAIL      	jole blithely after the furiously regular 
3906	40	1	2	50	47002.00	0.01	0.07	R	F	1992-09-24	1992-08-24	1992-09-29	NONE                     	MAIL      	ke slyly. stealt
3906	180	9	3	15	16202.70	0.06	0.02	R	F	1992-07-30	1992-08-26	1992-08-02	TAKE BACK RETURN         	FOB       	dependencies at the 
3906	59	10	4	36	34525.80	0.08	0.08	A	F	1992-08-07	1992-08-08	1992-08-24	NONE                     	SHIP      	y. ironic deposits haggle sl
3907	112	6	1	41	41496.51	0.06	0.02	A	F	1992-09-13	1992-10-23	1992-09-29	COLLECT COD              	MAIL      	ackages wake along the carefully regul
3907	145	4	2	41	42850.74	0.03	0.00	A	F	1992-10-25	1992-10-17	1992-11-01	TAKE BACK RETURN         	RAIL      	s above the unusual ideas sleep furiousl
3907	52	4	3	45	42842.25	0.02	0.07	R	F	1992-09-21	1992-09-19	1992-10-18	COLLECT COD              	REG AIR   	 about the regular pac
3907	176	5	4	48	51656.16	0.05	0.07	A	F	1992-09-24	1992-10-16	1992-10-06	DELIVER IN PERSON        	TRUCK     	nt asymptotes lose across th
3907	62	3	5	22	21165.32	0.09	0.01	R	F	1992-09-20	1992-10-30	1992-09-29	TAKE BACK RETURN         	TRUCK     	ly. furiously unusual deposits use afte
3907	126	9	6	34	34888.08	0.02	0.02	R	F	1992-09-06	1992-10-08	1992-09-12	COLLECT COD              	FOB       	 requests according to the slyly pending 
3907	110	5	7	8	8080.88	0.10	0.01	A	F	1992-09-18	1992-10-29	1992-09-27	NONE                     	REG AIR   	furiously final packages.
3909	178	6	1	30	32345.10	0.03	0.07	N	O	1998-10-17	1998-10-14	1998-10-28	COLLECT COD              	TRUCK     	ly even deposits across the ironic notorni
3909	191	4	2	46	50194.74	0.03	0.01	N	O	1998-10-08	1998-10-15	1998-10-24	NONE                     	FOB       	the blithely unusual ideas
3910	139	10	1	10	10391.30	0.00	0.08	N	O	1996-10-18	1996-10-31	1996-11-14	DELIVER IN PERSON        	FOB       	tions boost furiously unusual e
3910	71	10	2	31	30103.17	0.05	0.03	N	O	1996-12-22	1996-11-14	1997-01-01	TAKE BACK RETURN         	SHIP      	ess instructions. 
3910	20	7	3	6	5520.12	0.04	0.04	N	O	1996-12-08	1996-10-30	1996-12-31	DELIVER IN PERSON        	MAIL      	ly sly platelets are fluffily slyly si
3910	153	1	4	1	1053.15	0.03	0.06	N	O	1996-09-12	1996-10-21	1996-09-19	DELIVER IN PERSON        	FOB       	s sleep neve
3911	113	7	1	10	10131.10	0.07	0.06	N	O	1995-06-22	1995-05-30	1995-06-28	COLLECT COD              	FOB       	ss theodolites are blithely along t
3911	119	9	2	14	14267.54	0.08	0.05	R	F	1995-04-28	1995-05-03	1995-05-22	NONE                     	RAIL      	e blithely brave depo
3911	92	5	3	12	11905.08	0.10	0.05	R	F	1995-04-04	1995-04-16	1995-04-10	COLLECT COD              	FOB       	uctions. blithely regula
3937	70	7	1	48	46563.36	0.10	0.02	N	O	1998-03-15	1998-02-22	1998-03-30	DELIVER IN PERSON        	FOB       	gainst the thinl
3937	48	1	2	30	28441.20	0.01	0.07	N	O	1998-01-17	1998-01-03	1998-02-08	COLLECT COD              	TRUCK     	al packages slee
3937	115	5	3	27	27407.97	0.03	0.00	N	O	1998-02-06	1998-01-12	1998-02-20	NONE                     	MAIL      	ven ideas. slyly expr
3937	154	2	4	50	52707.50	0.01	0.02	N	O	1998-01-15	1998-01-09	1998-02-04	DELIVER IN PERSON        	AIR       	ong the carefully exp
3937	3	10	5	29	26187.00	0.03	0.07	N	O	1998-03-06	1998-02-22	1998-03-14	NONE                     	TRUCK     	nt pinto beans above the pending instr
3937	193	6	6	6	6559.14	0.00	0.00	N	O	1998-01-24	1998-02-13	1998-01-27	DELIVER IN PERSON        	FOB       	into beans. slyly silent orbits alongside o
3937	164	9	7	1	1064.16	0.02	0.05	N	O	1998-03-29	1998-01-08	1998-04-27	TAKE BACK RETURN         	TRUCK     	refully agains
3941	41	2	1	47	44228.88	0.05	0.07	N	O	1996-11-24	1996-10-09	1996-12-22	DELIVER IN PERSON        	RAIL      	 carefully pending
3941	123	6	2	19	19439.28	0.05	0.00	N	O	1996-11-10	1996-10-26	1996-12-05	COLLECT COD              	RAIL      	eposits haggle furiously even
3941	10	3	3	2	1820.02	0.01	0.03	N	O	1996-12-04	1996-10-01	1996-12-25	NONE                     	REG AIR   	es wake after the
3941	110	7	4	29	29293.19	0.00	0.03	N	O	1996-09-14	1996-10-04	1996-09-19	NONE                     	MAIL      	g the blithely
3942	183	4	1	6	6499.08	0.05	0.05	A	F	1993-07-01	1993-09-14	1993-07-23	DELIVER IN PERSON        	SHIP      	ep ruthlessly carefully final accounts: s
3942	194	7	2	5	5470.95	0.06	0.02	R	F	1993-09-27	1993-09-24	1993-10-07	DELIVER IN PERSON        	MAIL      	. fluffily pending deposits above the flu
3942	156	4	3	25	26403.75	0.04	0.06	R	F	1993-09-13	1993-08-01	1993-09-29	COLLECT COD              	RAIL      	d the quick packages
3943	198	2	1	15	16472.85	0.03	0.01	N	O	1997-01-13	1996-12-17	1997-02-02	COLLECT COD              	REG AIR   	 grow fluffily according to the 
3943	96	7	2	9	8964.81	0.00	0.06	N	O	1996-11-27	1997-01-03	1996-12-17	COLLECT COD              	RAIL      	refully ironic 
3943	17	4	3	32	29344.32	0.00	0.02	N	O	1996-10-22	1996-12-17	1996-11-04	TAKE BACK RETURN         	TRUCK     	 unusual ideas into the furiously even pack
3943	50	1	4	5	4750.25	0.04	0.04	N	O	1997-01-09	1996-11-10	1997-02-06	COLLECT COD              	RAIL      	arefully regular deposits accord
3968	54	2	1	27	25759.35	0.04	0.05	N	O	1997-04-25	1997-04-17	1997-05-11	TAKE BACK RETURN         	MAIL      	t silently.
3968	26	9	2	45	41670.90	0.00	0.07	N	O	1997-06-18	1997-04-24	1997-06-25	DELIVER IN PERSON        	FOB       	ully slyly fi
3968	156	7	3	43	45414.45	0.07	0.06	N	O	1997-04-30	1997-05-14	1997-05-18	TAKE BACK RETURN         	SHIP      	ly regular accounts
3968	61	8	4	7	6727.42	0.07	0.02	N	O	1997-03-30	1997-05-01	1997-04-12	DELIVER IN PERSON        	SHIP      	efully bold instructions. express
3969	52	4	1	39	37129.95	0.04	0.04	N	O	1997-06-12	1997-06-13	1997-07-05	NONE                     	MAIL      	ly bold ideas s
3969	197	1	2	26	28526.94	0.05	0.03	N	O	1997-07-08	1997-07-30	1997-07-10	TAKE BACK RETURN         	AIR       	fluffily; braids detect.
3969	79	8	3	46	45037.22	0.04	0.02	N	O	1997-05-29	1997-06-15	1997-06-10	TAKE BACK RETURN         	SHIP      	fully final requests sleep stealthily. care
3969	151	9	4	21	22074.15	0.07	0.04	N	O	1997-08-31	1997-07-16	1997-09-02	TAKE BACK RETURN         	MAIL      	unts doze quickly final reque
3969	72	3	5	40	38882.80	0.09	0.00	N	O	1997-05-19	1997-08-02	1997-06-05	COLLECT COD              	TRUCK     	lar requests cajole furiously blithely regu
3969	105	8	6	4	4020.40	0.02	0.01	N	O	1997-06-04	1997-07-31	1997-06-13	COLLECT COD              	REG AIR   	dencies wake blithely? quickly even theodo
3970	88	9	1	2	1976.16	0.01	0.07	R	F	1992-04-24	1992-06-03	1992-05-16	TAKE BACK RETURN         	RAIL      	carefully pending foxes wake blithely 
3970	109	6	2	18	18163.80	0.03	0.08	A	F	1992-06-06	1992-06-18	1992-07-05	DELIVER IN PERSON        	TRUCK     	 maintain slyly. ir
3970	154	6	3	10	10541.50	0.10	0.04	A	F	1992-07-01	1992-05-31	1992-07-02	NONE                     	AIR       	 special packages wake after the final br
3970	22	5	4	34	31348.68	0.05	0.00	A	F	1992-06-25	1992-05-23	1992-07-12	COLLECT COD              	SHIP      	y final gifts are. carefully pe
3970	30	3	5	23	21390.69	0.05	0.04	A	F	1992-06-04	1992-06-14	1992-06-13	COLLECT COD              	TRUCK     	 above the final braids. regular
3970	9	6	6	46	41814.00	0.07	0.04	R	F	1992-04-29	1992-05-14	1992-05-24	NONE                     	FOB       	yly ironic
3970	5	8	7	46	41630.00	0.08	0.08	R	F	1992-05-02	1992-05-12	1992-05-10	COLLECT COD              	MAIL      	ix slyly. quickly silen
3971	96	8	1	47	46816.23	0.06	0.04	N	O	1996-07-07	1996-08-08	1996-08-01	TAKE BACK RETURN         	RAIL      	e slyly final dependencies x-ray 
3971	191	5	2	2	2182.38	0.04	0.03	N	O	1996-07-15	1996-08-12	1996-07-26	NONE                     	SHIP      	haggle abou
3973	30	9	1	21	19530.63	0.02	0.06	R	F	1992-06-18	1992-06-03	1992-07-02	COLLECT COD              	REG AIR   	equests. furiously
3973	115	2	2	37	37559.07	0.07	0.00	A	F	1992-05-29	1992-05-04	1992-06-23	TAKE BACK RETURN         	SHIP      	inos wake fluffily. pending requests nag 
3973	40	6	3	40	37601.60	0.08	0.05	R	F	1992-05-03	1992-06-09	1992-05-21	COLLECT COD              	RAIL      	g the carefully blithe f
3974	22	1	1	47	43334.94	0.10	0.03	N	O	1996-06-03	1996-05-08	1996-06-28	NONE                     	TRUCK     	dencies above the re
3974	61	8	2	17	16338.02	0.05	0.07	N	O	1996-04-05	1996-05-21	1996-04-28	COLLECT COD              	TRUCK     	ions eat slyly after the blithely 
4001	106	1	1	26	26158.60	0.00	0.01	N	O	1997-07-26	1997-06-18	1997-08-08	DELIVER IN PERSON        	RAIL      	tegrate blithely
4001	41	10	2	19	17879.76	0.03	0.02	N	O	1997-08-23	1997-06-15	1997-09-18	COLLECT COD              	SHIP      	ackages. carefully ironi
4001	94	5	3	18	17893.62	0.07	0.00	N	O	1997-06-04	1997-06-22	1997-06-13	DELIVER IN PERSON        	MAIL      	lithely ironic d
4001	2	9	4	39	35178.00	0.00	0.00	N	O	1997-06-13	1997-06-17	1997-06-25	NONE                     	SHIP      	 dogged excuses. blithe
4002	111	5	1	35	35388.85	0.01	0.08	N	O	1997-05-16	1997-06-15	1997-06-02	DELIVER IN PERSON        	TRUCK     	eep. quickly
4002	198	9	2	20	21963.80	0.00	0.03	N	O	1997-06-15	1997-05-20	1997-07-11	NONE                     	MAIL      	lly even ins
4002	40	1	3	6	5640.24	0.08	0.07	N	O	1997-05-02	1997-07-07	1997-05-16	TAKE BACK RETURN         	RAIL      	 furiously furiously special theodoli
4002	199	3	4	6	6595.14	0.06	0.06	N	O	1997-07-01	1997-05-15	1997-07-31	NONE                     	MAIL      	he slyly iro
4002	99	1	5	4	3996.36	0.08	0.07	N	O	1997-05-06	1997-06-15	1997-05-24	NONE                     	REG AIR   	ccording to the careful
4005	4	1	1	26	23504.00	0.09	0.05	N	O	1996-12-01	1997-02-03	1996-12-15	NONE                     	REG AIR   	 to the quic
4005	17	8	2	28	25676.28	0.02	0.06	N	O	1996-12-11	1997-01-24	1996-12-17	DELIVER IN PERSON        	REG AIR   	ly carefully ironic deposits. slyly
4005	72	10	3	28	27217.96	0.03	0.01	N	O	1996-12-08	1997-01-14	1996-12-30	TAKE BACK RETURN         	MAIL      	y pending dependenc
4005	15	9	4	49	44835.49	0.09	0.00	N	O	1997-01-31	1996-12-24	1997-03-02	NONE                     	RAIL      	tions sleep across the silent d
4005	6	7	5	14	12684.00	0.09	0.08	N	O	1996-11-27	1997-01-09	1996-12-25	NONE                     	TRUCK     	ld requests. slyly final instructi
4032	102	3	1	8	8016.80	0.06	0.00	N	O	1998-06-04	1998-05-17	1998-07-03	TAKE BACK RETURN         	RAIL      	ometimes even cou
4032	2	9	2	27	24354.00	0.09	0.00	N	O	1998-05-31	1998-04-19	1998-06-24	COLLECT COD              	REG AIR   	le furiously according to
4032	154	2	3	23	24245.45	0.09	0.06	N	O	1998-06-12	1998-05-11	1998-06-24	COLLECT COD              	MAIL      	ording to the 
4032	85	6	4	10	9850.80	0.09	0.05	N	O	1998-03-31	1998-04-22	1998-04-07	NONE                     	REG AIR   	 carefully bol
4034	190	1	1	48	52329.12	0.03	0.03	A	F	1994-03-01	1994-01-16	1994-03-16	NONE                     	RAIL      	 blithely regular requests play carefull
4034	57	5	2	47	44981.35	0.07	0.05	A	F	1994-01-27	1993-12-26	1994-02-04	NONE                     	TRUCK     	eodolites was slyly ironic ideas. de
4034	54	5	3	43	41024.15	0.10	0.03	A	F	1993-11-29	1994-01-08	1993-12-10	DELIVER IN PERSON        	FOB       	posits wake carefully af
4034	28	9	4	46	42688.92	0.06	0.00	A	F	1994-02-22	1994-01-09	1994-03-04	DELIVER IN PERSON        	AIR       	uests. furiously unusual instructions wake
4034	196	10	5	7	7673.33	0.07	0.06	R	F	1994-03-04	1994-01-22	1994-04-01	NONE                     	AIR       	y even theodolites. slyly regular instru
4034	50	9	6	5	4750.25	0.01	0.06	A	F	1994-02-12	1994-01-24	1994-02-13	COLLECT COD              	AIR       	fully around the furiously ironic re
4036	6	1	1	46	41676.00	0.09	0.00	N	O	1997-06-21	1997-05-29	1997-07-18	NONE                     	REG AIR   	usly across the even th
4036	53	1	2	21	20014.05	0.09	0.07	N	O	1997-08-08	1997-06-28	1997-08-09	COLLECT COD              	MAIL      	e carefully. qui
4036	142	3	3	6	6252.84	0.07	0.01	N	O	1997-06-19	1997-06-16	1997-07-01	DELIVER IN PERSON        	SHIP      	equests wake about the bold id
4036	127	10	4	20	20542.40	0.08	0.02	N	O	1997-08-11	1997-07-11	1997-09-03	NONE                     	TRUCK     	slyly bold deposits cajole pending, blithe
4038	196	10	1	40	43847.60	0.05	0.01	N	O	1996-01-15	1996-03-13	1996-01-25	COLLECT COD              	TRUCK     	t. slyly silent pinto beans amo
4038	12	9	2	37	33744.37	0.04	0.03	N	O	1996-03-17	1996-03-19	1996-04-07	DELIVER IN PERSON        	REG AIR   	 packages 
4038	32	3	3	24	22368.72	0.10	0.04	N	O	1996-04-06	1996-02-15	1996-04-18	TAKE BACK RETURN         	RAIL      	the furiously regu
4038	150	1	4	29	30454.35	0.07	0.06	N	O	1996-01-07	1996-03-08	1996-01-13	NONE                     	FOB       	ffix. quietly ironic packages a
4038	79	7	5	24	23497.68	0.07	0.06	N	O	1996-04-01	1996-04-05	1996-04-28	DELIVER IN PERSON        	TRUCK     	ake quickly after the final, ironic ac
4038	36	2	6	6	5616.18	0.07	0.05	N	O	1996-02-09	1996-03-05	1996-03-10	COLLECT COD              	SHIP      	 special instructions. packa
4039	94	5	1	38	37775.42	0.03	0.06	N	O	1998-03-09	1997-12-31	1998-03-21	DELIVER IN PERSON        	REG AIR   	sual asymptotes. ironic deposits nag aft
4039	122	5	2	17	17376.04	0.10	0.04	N	O	1998-01-15	1998-01-20	1998-01-28	TAKE BACK RETURN         	MAIL      	 regular foxes haggle carefully bo
4039	64	1	3	9	8676.54	0.10	0.01	N	O	1998-03-08	1998-02-05	1998-04-05	TAKE BACK RETURN         	FOB       	t? pinto beans cajole across the thinly r
4039	28	3	4	43	39904.86	0.01	0.02	N	O	1998-01-02	1997-12-22	1998-01-15	NONE                     	FOB       	beans believe bene
4039	134	5	5	43	44467.59	0.09	0.00	N	O	1998-01-20	1998-01-11	1998-02-05	COLLECT COD              	SHIP      	sts along the regular in
4064	199	1	1	3	3297.57	0.10	0.04	N	O	1997-01-04	1997-01-01	1997-01-23	NONE                     	SHIP      	its! quickly sp
4064	40	6	2	15	14100.60	0.02	0.02	N	O	1996-11-09	1996-12-04	1996-11-18	DELIVER IN PERSON        	MAIL      	braids affix across the regular sheave
4064	197	10	3	32	35110.08	0.04	0.07	N	O	1997-01-14	1997-01-01	1997-01-21	COLLECT COD              	REG AIR   	es boost. careful
4064	163	8	4	24	25515.84	0.02	0.02	N	O	1997-01-01	1996-12-31	1997-01-23	DELIVER IN PERSON        	SHIP      	ly regular ideas.
4064	21	2	5	12	11052.24	0.08	0.08	N	O	1997-02-08	1996-12-18	1997-03-06	TAKE BACK RETURN         	RAIL      	ding to the requests
4064	184	5	6	46	49872.28	0.03	0.00	N	O	1996-10-13	1997-01-05	1996-11-06	DELIVER IN PERSON        	REG AIR   	alongside of the f
4064	200	2	7	9	9901.80	0.01	0.06	N	O	1996-12-17	1996-12-13	1997-01-12	NONE                     	AIR       	furiously f
4065	138	9	1	14	14533.82	0.04	0.02	A	F	1994-08-22	1994-07-29	1994-09-19	DELIVER IN PERSON        	TRUCK     	e furiously outside 
4065	15	6	2	46	42090.46	0.03	0.05	A	F	1994-06-29	1994-08-01	1994-07-03	TAKE BACK RETURN         	SHIP      	, regular requests may mold above the 
4065	97	10	3	33	32903.97	0.00	0.03	A	F	1994-09-03	1994-08-16	1994-09-13	DELIVER IN PERSON        	AIR       	ain blithely 
4065	107	2	4	8	8056.80	0.00	0.01	R	F	1994-10-04	1994-08-05	1994-10-25	TAKE BACK RETURN         	SHIP      	ages haggle carefully
4065	123	4	5	29	29670.48	0.02	0.07	A	F	1994-06-29	1994-08-19	1994-07-17	NONE                     	RAIL      	equests. packages sleep slyl
4065	110	5	6	16	16161.76	0.05	0.00	R	F	1994-08-25	1994-08-06	1994-09-09	COLLECT COD              	TRUCK     	ncies use furiously. quickly un
4065	144	7	7	11	11485.54	0.10	0.04	A	F	1994-07-25	1994-08-02	1994-07-30	NONE                     	RAIL      	hang silently about 
4067	180	1	1	18	19443.24	0.03	0.08	A	F	1993-01-24	1992-12-23	1993-02-20	TAKE BACK RETURN         	FOB       	e the slyly final packages d
4067	96	10	2	14	13945.26	0.00	0.00	R	F	1993-02-03	1992-12-02	1993-02-07	TAKE BACK RETURN         	TRUCK     	ructions. quickly ironic accounts detect 
4067	141	10	3	17	17699.38	0.03	0.05	A	F	1993-01-26	1992-11-23	1993-01-27	NONE                     	REG AIR   	ts haggle slyly unusual, final
4067	90	1	4	40	39603.60	0.07	0.08	R	F	1993-01-09	1992-11-21	1993-01-16	DELIVER IN PERSON        	TRUCK     	lar theodolites nag blithely above the
4067	85	6	5	17	16746.36	0.08	0.03	A	F	1993-01-20	1992-12-29	1993-02-03	DELIVER IN PERSON        	REG AIR   	r accounts. slyly special pa
4067	96	8	6	12	11953.08	0.04	0.03	A	F	1992-12-12	1992-11-28	1992-12-15	DELIVER IN PERSON        	AIR       	lly slyly even theodol
4067	83	4	7	17	16712.36	0.01	0.01	R	F	1992-12-12	1992-12-23	1992-12-30	NONE                     	AIR       	ts affix. regular, regular requests s
4069	129	2	1	39	40135.68	0.09	0.02	R	F	1992-09-06	1992-07-22	1992-09-25	COLLECT COD              	SHIP      	ven theodolites nag quickly. fluffi
4069	43	4	2	32	30177.28	0.10	0.08	A	F	1992-06-18	1992-07-20	1992-07-07	TAKE BACK RETURN         	TRUCK     	unts. deposit
4069	186	7	3	3	3258.54	0.06	0.01	R	F	1992-07-26	1992-07-07	1992-08-04	COLLECT COD              	FOB       	l packages. even, 
4069	79	8	4	22	21539.54	0.10	0.05	A	F	1992-08-05	1992-08-04	1992-08-25	COLLECT COD              	SHIP      	ts. slyly special instruction
4069	157	5	5	50	52857.50	0.09	0.06	A	F	1992-07-26	1992-06-30	1992-08-01	TAKE BACK RETURN         	REG AIR   	even foxes among the express wate
4069	125	8	6	3	3075.36	0.02	0.01	A	F	1992-05-24	1992-06-18	1992-06-12	COLLECT COD              	MAIL      	y final deposits wake furiously! slyl
4069	184	5	7	50	54209.00	0.00	0.01	R	F	1992-09-03	1992-06-14	1992-10-01	NONE                     	REG AIR   	ages. carefully regular 
4070	183	4	1	2	2166.36	0.09	0.08	N	O	1995-08-03	1995-09-10	1995-08-17	TAKE BACK RETURN         	REG AIR   	ptotes affix
4070	155	3	2	40	42206.00	0.07	0.07	N	O	1995-07-13	1995-07-23	1995-08-06	COLLECT COD              	MAIL      	about the sentiments. quick
4070	62	3	3	11	10582.66	0.00	0.08	N	O	1995-08-23	1995-08-15	1995-08-31	TAKE BACK RETURN         	MAIL      	 carefully final pack
4070	29	4	4	46	42734.92	0.02	0.02	N	O	1995-06-22	1995-07-14	1995-07-11	DELIVER IN PERSON        	REG AIR   	nticing ideas. boldly
4071	112	2	1	22	22266.42	0.02	0.07	N	O	1996-10-31	1996-12-14	1996-11-05	NONE                     	REG AIR   	sits cajole carefully final instructio
4071	18	8	2	47	43146.47	0.00	0.03	N	O	1996-11-04	1996-12-09	1996-11-16	NONE                     	TRUCK     	ts cajole furiously along the
4096	27	10	1	31	28737.62	0.10	0.02	A	F	1992-07-14	1992-09-03	1992-07-31	COLLECT COD              	TRUCK     	y final, even platelets. boldly
4096	57	9	2	17	16269.85	0.07	0.03	R	F	1992-09-30	1992-08-11	1992-10-11	TAKE BACK RETURN         	REG AIR   	platelets alongside of the 
4096	9	10	3	21	19089.00	0.08	0.00	A	F	1992-08-24	1992-09-04	1992-09-11	DELIVER IN PERSON        	MAIL      	tes mold flu
4096	128	3	4	20	20562.40	0.02	0.07	R	F	1992-08-24	1992-09-13	1992-08-28	DELIVER IN PERSON        	TRUCK     	sual requests. furiously bold packages wake
4097	74	5	1	50	48703.50	0.04	0.04	N	O	1996-08-31	1996-08-14	1996-09-27	DELIVER IN PERSON        	MAIL      	egular deposits. blithely pending
4097	74	4	2	46	44807.22	0.10	0.01	N	O	1996-07-29	1996-08-19	1996-08-25	COLLECT COD              	AIR       	 even depend
4097	174	2	3	42	45115.14	0.06	0.06	N	O	1996-08-11	1996-07-30	1996-08-15	NONE                     	FOB       	carefully silent foxes are against the 
4098	200	1	1	46	50609.20	0.07	0.03	N	O	1997-01-26	1997-01-27	1997-02-13	TAKE BACK RETURN         	SHIP      	e slyly blithely silent deposits. fluff
4099	4	7	1	29	26216.00	0.09	0.07	R	F	1992-11-21	1992-11-04	1992-11-30	NONE                     	FOB       	 slowly final warthogs sleep blithely. q
4099	137	3	2	3	3111.39	0.04	0.06	A	F	1992-09-12	1992-10-18	1992-10-01	NONE                     	RAIL      	. special packages sleep
4099	51	3	3	36	34237.80	0.06	0.06	R	F	1992-11-06	1992-09-28	1992-12-02	NONE                     	FOB       	beans cajole slyly quickly ironic 
4099	139	5	4	7	7273.91	0.05	0.02	A	F	1992-09-12	1992-11-13	1992-09-14	TAKE BACK RETURN         	AIR       	onic foxes. quickly final fox
4099	163	10	5	48	51031.68	0.00	0.02	R	F	1992-10-18	1992-10-14	1992-11-01	NONE                     	REG AIR   	ts haggle according to the slyly f
4099	59	10	6	39	37402.95	0.07	0.02	R	F	1992-12-13	1992-11-13	1992-12-26	DELIVER IN PERSON        	REG AIR   	fluffy accounts impress pending, iro
4099	180	8	7	46	49688.28	0.06	0.07	R	F	1992-10-29	1992-11-03	1992-11-10	DELIVER IN PERSON        	REG AIR   	ages nag requests.
4100	74	5	1	4	3896.28	0.03	0.03	N	O	1996-06-20	1996-04-29	1996-06-21	TAKE BACK RETURN         	FOB       	lyly regular, bold requ
4101	115	2	1	22	22332.42	0.05	0.02	R	F	1994-02-02	1994-02-19	1994-02-12	COLLECT COD              	AIR       	ly express instructions. careful
4102	10	3	1	17	15470.17	0.02	0.02	N	O	1996-06-03	1996-05-06	1996-07-02	COLLECT COD              	AIR       	ly silent theodolites sleep unusual exc
4102	69	8	2	5	4845.30	0.08	0.02	N	O	1996-05-11	1996-05-11	1996-05-16	COLLECT COD              	AIR       	 the furiously even
4102	67	4	3	39	37715.34	0.08	0.01	N	O	1996-04-14	1996-05-18	1996-04-20	DELIVER IN PERSON        	AIR       	ffix blithely slyly special 
4102	140	6	4	39	40565.46	0.02	0.00	N	O	1996-06-15	1996-06-06	1996-06-30	DELIVER IN PERSON        	SHIP      	y among the furiously special
4102	1	6	5	32	28832.00	0.08	0.01	N	O	1996-05-14	1996-04-29	1996-05-29	NONE                     	RAIL      	 the even requests; regular pinto
4102	137	8	6	7	7259.91	0.02	0.01	N	O	1996-06-19	1996-05-21	1996-07-15	NONE                     	REG AIR   	bove the carefully pending the
4103	75	4	1	40	39002.80	0.05	0.03	R	F	1992-09-19	1992-08-14	1992-09-21	COLLECT COD              	RAIL      	usly across the slyly busy accounts! fin
4128	196	8	1	5	5480.95	0.04	0.04	N	O	1995-10-18	1995-11-28	1995-10-28	TAKE BACK RETURN         	FOB       	ake permanently 
4130	178	6	1	44	47439.48	0.07	0.04	N	O	1996-05-14	1996-04-15	1996-05-15	COLLECT COD              	TRUCK     	eaves haggle qui
4130	63	10	2	2	1926.12	0.05	0.06	N	O	1996-05-19	1996-04-24	1996-06-17	TAKE BACK RETURN         	RAIL      	uriously regular instructions around th
4133	24	5	1	35	32340.70	0.02	0.00	A	F	1992-11-25	1992-09-15	1992-12-25	NONE                     	AIR       	g above the quickly bold packages. ev
4135	2	3	1	23	20746.00	0.06	0.01	N	O	1997-04-09	1997-05-12	1997-04-16	TAKE BACK RETURN         	FOB       	posits cajole furiously carefully
4135	120	1	2	32	32643.84	0.07	0.00	N	O	1997-03-14	1997-04-23	1997-04-12	TAKE BACK RETURN         	TRUCK     	 ideas. requests use. furiously
4135	160	5	3	33	34985.28	0.05	0.05	N	O	1997-05-01	1997-05-23	1997-05-23	DELIVER IN PERSON        	AIR       	he fluffil
4135	195	6	4	13	14237.47	0.04	0.07	N	O	1997-03-16	1997-05-19	1997-04-03	COLLECT COD              	RAIL      	efully special account
4160	113	10	1	25	25327.75	0.10	0.04	N	O	1996-09-22	1996-10-17	1996-09-24	NONE                     	SHIP      	ar accounts sleep blithe
4160	122	7	2	12	12265.44	0.00	0.03	N	O	1996-11-22	1996-09-25	1996-12-10	DELIVER IN PERSON        	REG AIR   	y bold package
4160	63	4	3	48	46226.88	0.04	0.04	N	O	1996-09-19	1996-11-02	1996-09-24	COLLECT COD              	FOB       	 unusual dolphins 
4162	74	3	1	45	43833.15	0.10	0.07	A	F	1992-03-21	1992-05-02	1992-03-29	DELIVER IN PERSON        	AIR       	elets. slyly regular i
4162	90	1	2	29	28712.61	0.00	0.05	R	F	1992-02-25	1992-04-25	1992-03-17	NONE                     	REG AIR   	nding pinto beans haggle blithe
4163	33	4	1	13	12129.39	0.08	0.03	A	F	1993-02-17	1993-03-13	1993-03-15	DELIVER IN PERSON        	REG AIR   	phins wake. pending requests inte
4164	120	7	1	9	9181.08	0.07	0.02	N	O	1998-08-25	1998-08-13	1998-09-19	DELIVER IN PERSON        	SHIP      	re fluffily slyly bold requests. 
4165	41	2	1	12	11292.48	0.00	0.01	N	O	1997-09-20	1997-10-20	1997-10-12	TAKE BACK RETURN         	REG AIR   	nwind slow theodolites. carefully pending 
4167	61	8	1	47	45169.82	0.04	0.02	N	O	1998-08-02	1998-08-24	1998-08-28	DELIVER IN PERSON        	REG AIR   	 carefully final asymptotes. slyly bo
4167	87	8	2	17	16780.36	0.06	0.07	N	O	1998-09-18	1998-09-06	1998-10-07	COLLECT COD              	REG AIR   	ly around the even instr
4167	73	3	3	1	973.07	0.03	0.06	N	O	1998-10-11	1998-08-14	1998-10-13	COLLECT COD              	TRUCK     	xpress platelets. blithely 
4192	11	1	1	36	32796.36	0.06	0.08	N	O	1998-04-25	1998-05-26	1998-05-03	COLLECT COD              	TRUCK     	eodolites sleep
4192	121	6	2	15	15316.80	0.04	0.08	N	O	1998-06-26	1998-05-26	1998-07-16	COLLECT COD              	AIR       	e slyly special grouches. express pinto b
4192	135	6	3	7	7245.91	0.06	0.03	N	O	1998-05-19	1998-07-08	1998-05-31	COLLECT COD              	FOB       	y; excuses use. ironic, close instru
4192	24	3	4	32	29568.64	0.09	0.04	N	O	1998-06-23	1998-06-25	1998-07-17	NONE                     	FOB       	ounts are fluffily slyly bold req
4192	48	7	5	48	45505.92	0.08	0.01	N	O	1998-08-17	1998-07-11	1998-09-03	NONE                     	AIR       	ests. quickly bol
4192	150	7	6	44	46206.60	0.10	0.02	N	O	1998-08-06	1998-07-09	1998-08-20	NONE                     	FOB       	structions mai
4192	170	5	7	27	28894.59	0.02	0.00	N	O	1998-07-03	1998-06-26	1998-07-13	TAKE BACK RETURN         	AIR       	 carefully even escapades. care
4193	131	7	1	37	38151.81	0.09	0.06	A	F	1994-04-25	1994-02-24	1994-05-08	NONE                     	AIR       	er the quickly regular dependencies wake
4193	117	7	2	3	3051.33	0.09	0.05	R	F	1994-04-29	1994-03-20	1994-05-29	TAKE BACK RETURN         	REG AIR   	osits above the depo
4193	179	10	3	10	10791.70	0.06	0.03	A	F	1994-02-10	1994-03-22	1994-03-09	COLLECT COD              	RAIL      	uffily spe
4193	51	9	4	29	27580.45	0.09	0.05	A	F	1994-02-11	1994-03-11	1994-03-13	TAKE BACK RETURN         	RAIL      	ly. final packages use blit
4193	20	7	5	50	46001.00	0.01	0.01	R	F	1994-04-28	1994-03-23	1994-05-09	NONE                     	FOB       	 beans. regular accounts cajole. de
4193	66	1	6	21	20287.26	0.02	0.04	R	F	1994-04-26	1994-03-22	1994-05-23	DELIVER IN PERSON        	TRUCK     	accounts cajole b
4194	197	1	1	43	47179.17	0.08	0.06	A	F	1994-11-06	1994-12-09	1994-11-16	NONE                     	TRUCK     	olites are after the exp
4194	47	10	2	18	17046.72	0.07	0.07	A	F	1995-02-14	1994-12-04	1995-03-11	TAKE BACK RETURN         	TRUCK     	ld packages. quickly eve
4195	6	9	1	14	12684.00	0.09	0.04	R	F	1993-09-06	1993-07-21	1993-09-18	DELIVER IN PERSON        	REG AIR   	ironic packages. carefully express
4195	66	1	2	22	21253.32	0.10	0.08	R	F	1993-07-01	1993-07-23	1993-07-28	COLLECT COD              	RAIL      	lly express pinto bea
4195	194	8	3	19	20789.61	0.01	0.06	R	F	1993-09-06	1993-08-13	1993-09-15	TAKE BACK RETURN         	REG AIR   	telets sleep even requests. final, even i
4196	156	4	1	30	31684.50	0.02	0.06	N	O	1998-08-09	1998-06-30	1998-09-05	COLLECT COD              	SHIP      	egular foxes us
4196	9	6	2	31	28179.00	0.09	0.08	N	O	1998-06-12	1998-07-28	1998-07-11	NONE                     	MAIL      	ut the blithely ironic inst
4196	178	9	3	46	49595.82	0.05	0.00	N	O	1998-09-05	1998-06-28	1998-09-10	TAKE BACK RETURN         	MAIL      	according to t
4196	114	8	4	42	42592.62	0.04	0.06	N	O	1998-08-13	1998-07-18	1998-09-07	TAKE BACK RETURN         	AIR       	 instructions. courts cajole slyly ev
4196	72	2	5	3	2916.21	0.01	0.03	N	O	1998-05-17	1998-07-21	1998-05-18	DELIVER IN PERSON        	TRUCK     	 accounts. fu
4196	87	8	6	43	42444.44	0.01	0.06	N	O	1998-08-12	1998-07-12	1998-08-22	DELIVER IN PERSON        	FOB       	es. slyly even 
4196	4	1	7	3	2712.00	0.00	0.06	N	O	1998-08-05	1998-07-28	1998-08-15	DELIVER IN PERSON        	REG AIR   	y regular packages haggle furiously alongs
4197	129	8	1	50	51456.00	0.06	0.03	N	O	1996-11-15	1996-11-01	1996-11-20	NONE                     	FOB       	. carefully bold asymptotes nag blithe
4197	70	9	2	39	37832.73	0.02	0.08	N	O	1996-10-07	1996-10-11	1996-10-18	DELIVER IN PERSON        	RAIL      	ronic requests. quickly bold packages in
4197	32	8	3	28	26096.84	0.06	0.02	N	O	1996-10-05	1996-10-24	1996-10-22	TAKE BACK RETURN         	AIR       	regular pin
4197	96	7	4	23	22910.07	0.00	0.03	N	O	1996-09-10	1996-10-10	1996-09-25	NONE                     	AIR       	l instructions print slyly past the reg
4197	121	6	5	37	37781.44	0.03	0.04	N	O	1996-10-20	1996-10-10	1996-11-10	COLLECT COD              	TRUCK     	carefully enticing decoys boo
4197	31	7	6	48	44689.44	0.08	0.00	N	O	1996-10-07	1996-10-25	1996-10-23	COLLECT COD              	REG AIR   	 final instructions. blithe, spe
4199	70	5	1	16	15521.12	0.10	0.00	A	F	1992-06-11	1992-04-10	1992-07-10	COLLECT COD              	TRUCK     	ncies. furiously special accounts
4199	9	10	2	18	16362.00	0.00	0.01	A	F	1992-06-01	1992-03-30	1992-06-28	DELIVER IN PERSON        	RAIL      	pending, regular accounts. carefully
4225	49	8	1	25	23726.00	0.08	0.04	N	O	1997-07-10	1997-08-08	1997-07-31	TAKE BACK RETURN         	TRUCK     	se fluffily. busily ironic requests are;
4225	96	8	2	23	22910.07	0.02	0.04	N	O	1997-09-18	1997-08-31	1997-10-11	TAKE BACK RETURN         	RAIL      	. quickly b
4225	98	10	3	28	27946.52	0.08	0.03	N	O	1997-07-11	1997-09-01	1997-08-03	DELIVER IN PERSON        	FOB       	ts are requests. even, bold depos
4226	188	9	1	27	29380.86	0.06	0.08	A	F	1993-05-03	1993-04-12	1993-05-16	COLLECT COD              	AIR       	sly alongside of the slyly ironic pac
4228	141	10	1	20	20822.80	0.00	0.06	N	O	1997-04-24	1997-05-29	1997-05-17	NONE                     	RAIL      	f the slyly fluffy pinto beans are
4229	96	9	1	44	43827.96	0.02	0.05	N	O	1998-05-29	1998-05-12	1998-06-16	DELIVER IN PERSON        	AIR       	s. carefully e
4229	5	8	2	34	30770.00	0.07	0.05	N	O	1998-05-26	1998-04-13	1998-06-08	DELIVER IN PERSON        	MAIL      	thely final accounts use even packa
4230	46	5	1	38	35949.52	0.10	0.03	A	F	1992-04-28	1992-04-21	1992-05-28	TAKE BACK RETURN         	FOB       	ly regular packages. regular ideas boost
4230	199	3	2	43	47265.17	0.02	0.08	R	F	1992-03-14	1992-05-13	1992-03-28	NONE                     	FOB       	ses lose blithely slyly final e
4230	196	9	3	10	10961.90	0.06	0.02	A	F	1992-06-11	1992-04-11	1992-07-02	TAKE BACK RETURN         	MAIL      	ar packages are 
4230	75	6	4	28	27301.96	0.01	0.03	R	F	1992-05-12	1992-05-10	1992-06-01	TAKE BACK RETURN         	MAIL      	nt instruct
4230	125	10	5	50	51256.00	0.00	0.01	A	F	1992-03-29	1992-05-19	1992-04-20	TAKE BACK RETURN         	SHIP      	ts. final instructions in
4230	35	6	6	30	28050.90	0.05	0.07	A	F	1992-03-11	1992-04-29	1992-03-30	NONE                     	AIR       	s. final excuses across the
4230	152	3	7	18	18938.70	0.10	0.04	R	F	1992-06-23	1992-05-10	1992-07-04	COLLECT COD              	SHIP      	 the final acco
4231	142	3	1	47	48980.58	0.09	0.03	N	O	1997-11-27	1998-01-26	1997-12-17	NONE                     	REG AIR   	hely along the silent at
4231	166	3	2	4	4264.64	0.06	0.02	N	O	1997-11-28	1998-01-26	1997-12-12	TAKE BACK RETURN         	MAIL      	lithely even packages. 
4231	121	2	3	31	31654.72	0.07	0.08	N	O	1998-02-14	1997-12-27	1998-03-01	DELIVER IN PERSON        	FOB       	ublate. theodoli
4231	40	1	4	35	32901.40	0.10	0.00	N	O	1998-02-21	1998-01-24	1998-03-18	DELIVER IN PERSON        	FOB       	le quickly regular, unus
4257	65	10	1	3	2895.18	0.10	0.03	N	O	1995-06-18	1995-05-01	1995-07-12	DELIVER IN PERSON        	MAIL      	thin the theodolites use after the bl
4257	35	6	2	5	4675.15	0.01	0.04	R	F	1995-04-29	1995-06-05	1995-05-13	TAKE BACK RETURN         	TRUCK     	n deposits. furiously e
4257	128	9	3	33	33927.96	0.03	0.04	A	F	1995-05-23	1995-05-03	1995-05-31	COLLECT COD              	AIR       	uffily regular accounts ar
4258	166	7	1	36	38381.76	0.02	0.06	N	O	1997-02-23	1997-01-25	1997-02-27	TAKE BACK RETURN         	SHIP      	ns use alongs
4258	162	1	2	19	20181.04	0.03	0.02	N	O	1997-01-14	1996-12-12	1997-01-20	TAKE BACK RETURN         	AIR       	ly busily ironic foxes. f
4258	31	7	3	46	42827.38	0.04	0.07	N	O	1997-01-02	1996-12-26	1997-01-12	DELIVER IN PERSON        	AIR       	 furiously pend
4258	35	6	4	22	20570.66	0.04	0.04	N	O	1996-12-12	1996-12-06	1996-12-20	TAKE BACK RETURN         	AIR       	e regular, even asym
4258	163	10	5	9	9568.44	0.04	0.03	N	O	1996-12-04	1996-12-08	1996-12-20	DELIVER IN PERSON        	TRUCK     	counts wake permanently after the bravely
4259	43	6	1	14	13202.56	0.05	0.03	N	O	1998-01-09	1997-11-21	1998-01-29	TAKE BACK RETURN         	RAIL      	 furiously pending excuses. ideas hagg
4260	24	7	1	21	19404.42	0.08	0.04	R	F	1992-08-06	1992-06-18	1992-08-22	NONE                     	AIR       	al, pending accounts must
4263	18	9	1	9	8262.09	0.08	0.07	N	O	1998-04-04	1998-04-29	1998-05-04	COLLECT COD              	AIR       	structions cajole quic
4263	196	10	2	28	30693.32	0.05	0.03	N	O	1998-06-24	1998-06-08	1998-07-14	NONE                     	MAIL      	ideas for the carefully re
4263	11	1	3	38	34618.38	0.01	0.01	N	O	1998-07-10	1998-05-08	1998-07-17	NONE                     	TRUCK     	rding to the dep
4263	19	3	4	20	18380.20	0.02	0.07	N	O	1998-04-09	1998-04-30	1998-05-04	NONE                     	RAIL      	uietly regular deposits. sly deposits w
4263	198	2	5	14	15374.66	0.09	0.06	N	O	1998-05-06	1998-04-17	1998-05-11	DELIVER IN PERSON        	TRUCK     	d accounts. daringly regular accounts hagg
4263	113	10	6	47	47616.17	0.08	0.06	N	O	1998-06-28	1998-05-09	1998-07-02	DELIVER IN PERSON        	TRUCK     	y. theodolites wake idly ironic do
4263	29	4	7	6	5574.12	0.04	0.04	N	O	1998-05-01	1998-06-02	1998-05-14	TAKE BACK RETURN         	REG AIR   	g the final, regular instructions: 
4290	137	3	1	23	23853.99	0.06	0.04	R	F	1995-04-04	1995-02-16	1995-04-07	TAKE BACK RETURN         	REG AIR   	uests cajole carefully.
4290	99	2	2	3	2997.27	0.09	0.03	A	F	1995-03-25	1995-03-07	1995-04-11	NONE                     	RAIL      	lar platelets cajole
4292	44	3	1	22	20768.88	0.08	0.03	R	F	1992-02-14	1992-02-16	1992-03-01	NONE                     	FOB       	refully expres
4292	40	6	2	1	940.04	0.03	0.01	A	F	1992-02-07	1992-03-16	1992-02-10	DELIVER IN PERSON        	FOB       	 the furiously ev
4292	120	10	3	35	35704.20	0.03	0.06	A	F	1992-03-23	1992-04-04	1992-04-02	COLLECT COD              	TRUCK     	dugouts use. furiously bold packag
4292	163	10	4	40	42526.40	0.05	0.04	A	F	1992-04-27	1992-03-07	1992-05-04	COLLECT COD              	REG AIR   	ounts according to the furiously 
4292	131	7	5	6	6186.78	0.07	0.08	R	F	1992-03-03	1992-02-24	1992-03-25	COLLECT COD              	FOB       	bove the silently regula
4292	4	1	6	47	42488.00	0.05	0.00	R	F	1992-05-02	1992-03-21	1992-05-27	TAKE BACK RETURN         	FOB       	y packages; even ideas boost
4293	1	6	1	34	30634.00	0.03	0.08	N	O	1996-11-05	1996-10-12	1996-12-04	NONE                     	FOB       	ions sleep blithely on
4293	77	5	2	50	48853.50	0.01	0.05	N	O	1996-11-27	1996-10-30	1996-12-22	COLLECT COD              	MAIL      	 special deposits. furiousl
4293	199	1	3	47	51661.93	0.08	0.02	N	O	1996-09-07	1996-10-24	1996-09-15	NONE                     	RAIL      	ithely pending deposits af
4293	88	9	4	25	24702.00	0.04	0.04	N	O	1996-09-11	1996-11-14	1996-09-22	DELIVER IN PERSON        	FOB       	inal asympt
4293	181	2	5	1	1081.18	0.06	0.05	N	O	1996-11-15	1996-10-09	1996-11-26	COLLECT COD              	AIR       	eposits should boost along the 
4293	79	7	6	45	44058.15	0.10	0.04	N	O	1996-11-04	1996-11-06	1996-11-23	NONE                     	MAIL      	lar ideas use carefully
4294	105	8	1	19	19096.90	0.03	0.04	A	F	1992-10-16	1992-11-13	1992-10-26	DELIVER IN PERSON        	AIR       	nt dependencies. furiously regular ideas d
4294	27	2	2	16	14832.32	0.01	0.02	R	F	1992-08-17	1992-09-24	1992-09-04	TAKE BACK RETURN         	REG AIR   	lithely pint
4294	198	1	3	30	32945.70	0.01	0.00	A	F	1992-09-12	1992-11-06	1992-09-25	NONE                     	MAIL      	olites. bold foxes affix ironic theodolite
4294	105	2	4	34	34173.40	0.02	0.01	R	F	1992-09-09	1992-11-06	1992-10-04	TAKE BACK RETURN         	REG AIR   	pendencies!
4294	119	3	5	37	37707.07	0.05	0.01	R	F	1992-09-07	1992-10-13	1992-09-08	NONE                     	REG AIR   	cial packages nag f
4294	87	8	6	42	41457.36	0.02	0.03	A	F	1992-09-30	1992-11-13	1992-10-15	DELIVER IN PERSON        	FOB       	 carefully; furiously ex
4294	175	3	7	47	50532.99	0.02	0.08	R	F	1992-11-09	1992-11-03	1992-12-05	TAKE BACK RETURN         	SHIP      	es. blithely r
4295	29	2	1	49	45521.98	0.09	0.01	N	O	1996-05-25	1996-03-17	1996-06-19	TAKE BACK RETURN         	REG AIR   	refully silent requests. f
4295	71	9	2	4	3884.28	0.09	0.07	N	O	1996-06-05	1996-04-26	1996-06-13	DELIVER IN PERSON        	TRUCK     	arefully according to the pending ac
4295	193	4	3	3	3279.57	0.04	0.00	N	O	1996-06-04	1996-04-24	1996-06-24	DELIVER IN PERSON        	AIR       	telets cajole bravely
4295	80	9	4	30	29402.40	0.07	0.06	N	O	1996-03-22	1996-04-23	1996-04-20	NONE                     	SHIP      	yly ironic frets. pending foxes after 
4320	46	5	1	28	26489.12	0.02	0.06	N	O	1997-01-28	1997-02-07	1997-02-07	COLLECT COD              	FOB       	nts. even, ironic excuses hagg
4320	140	6	2	6	6240.84	0.08	0.08	N	O	1997-01-11	1997-01-26	1997-01-22	DELIVER IN PERSON        	SHIP      	against the carefully careful asym
4320	188	9	3	33	35909.94	0.09	0.02	N	O	1996-12-11	1997-02-27	1997-01-08	TAKE BACK RETURN         	SHIP      	ess asymptotes so
4321	147	6	1	33	34555.62	0.09	0.02	A	F	1994-09-01	1994-08-17	1994-09-05	DELIVER IN PERSON        	TRUCK     	yly special excuses. fluffily 
4321	54	2	2	45	42932.25	0.00	0.08	R	F	1994-11-13	1994-09-15	1994-11-18	DELIVER IN PERSON        	SHIP      	 haggle ironically bold theodolites. quick
4321	186	7	3	23	24982.14	0.01	0.05	A	F	1994-11-03	1994-10-08	1994-11-06	DELIVER IN PERSON        	SHIP      	ly even orbits slee
4321	91	2	4	4	3964.36	0.02	0.00	R	F	1994-09-10	1994-10-06	1994-09-11	NONE                     	FOB       	ironic deposi
4321	172	2	5	10	10721.70	0.04	0.03	A	F	1994-09-07	1994-08-23	1994-09-17	TAKE BACK RETURN         	SHIP      	wake carefully alongside of 
4322	69	4	1	39	37793.34	0.04	0.02	N	O	1998-04-27	1998-06-03	1998-05-04	TAKE BACK RETURN         	MAIL      	its integrate fluffily 
4322	140	1	2	9	9361.26	0.05	0.08	N	O	1998-05-18	1998-04-27	1998-05-28	COLLECT COD              	AIR       	ual instructio
4322	8	9	3	12	10896.00	0.09	0.05	N	O	1998-03-29	1998-06-05	1998-04-16	DELIVER IN PERSON        	TRUCK     	e blithely against the slyly unusu
4322	46	7	4	17	16082.68	0.09	0.08	N	O	1998-05-31	1998-05-31	1998-06-10	TAKE BACK RETURN         	FOB       	ructions boost 
4322	102	7	5	10	10021.00	0.00	0.05	N	O	1998-05-31	1998-04-27	1998-06-25	TAKE BACK RETURN         	REG AIR   	 regular ideas engage carefully quick
4322	60	8	6	39	37442.34	0.09	0.08	N	O	1998-03-16	1998-05-21	1998-04-11	COLLECT COD              	AIR       	ccounts. dogged pin
4322	14	4	7	34	31076.34	0.05	0.00	N	O	1998-05-27	1998-04-12	1998-06-16	NONE                     	REG AIR   	ounts haggle fluffily ideas. pend
4323	1	2	1	33	29733.00	0.09	0.02	A	F	1994-05-04	1994-03-06	1994-05-23	COLLECT COD              	TRUCK     	the slyly bold deposits slee
4324	51	2	1	44	41846.20	0.05	0.04	N	O	1995-10-15	1995-09-07	1995-11-07	DELIVER IN PERSON        	AIR       	ainst the u
4324	48	7	2	12	11376.48	0.04	0.02	N	O	1995-10-05	1995-09-07	1995-10-18	NONE                     	REG AIR   	c packages. furiously express sauternes
4324	82	3	3	14	13749.12	0.07	0.06	N	O	1995-11-12	1995-08-26	1995-11-21	COLLECT COD              	AIR       	 packages nag express excuses. qui
4324	50	7	4	14	13300.70	0.02	0.04	N	O	1995-09-20	1995-10-08	1995-10-06	COLLECT COD              	RAIL      	 express ideas. blithely blit
4324	84	5	5	22	21649.76	0.07	0.03	N	O	1995-09-13	1995-10-04	1995-09-23	DELIVER IN PERSON        	SHIP      	ke express, special ideas.
4324	43	2	6	31	29234.24	0.08	0.04	N	O	1995-10-23	1995-09-14	1995-11-09	COLLECT COD              	RAIL      	efully flu
4324	154	6	7	46	48490.90	0.00	0.03	N	O	1995-11-03	1995-09-28	1995-11-22	NONE                     	SHIP      	ular, final theodo
4325	160	2	1	18	19082.88	0.01	0.07	N	O	1996-10-07	1996-09-28	1996-10-31	DELIVER IN PERSON        	RAIL      	. blithely
4326	163	4	1	11	11694.76	0.01	0.01	N	O	1997-02-02	1996-12-10	1997-02-20	DELIVER IN PERSON        	TRUCK     	press reque
4326	167	6	2	27	28813.32	0.06	0.01	N	O	1996-11-29	1997-01-20	1996-12-23	COLLECT COD              	AIR       	inal packages. final asymptotes about t
4327	95	8	1	18	17911.62	0.08	0.00	N	F	1995-06-16	1995-04-20	1995-07-12	COLLECT COD              	RAIL      	y final excuses. ironic, special requests a
4327	106	9	2	40	40244.00	0.07	0.01	N	F	1995-05-26	1995-04-17	1995-06-18	NONE                     	AIR       	quests. packages are after th
4327	145	2	3	11	11496.54	0.10	0.07	R	F	1995-04-24	1995-05-27	1995-05-24	TAKE BACK RETURN         	FOB       	 ironic dolphins
4327	21	10	4	8	7368.16	0.04	0.08	N	F	1995-05-26	1995-05-28	1995-06-19	DELIVER IN PERSON        	AIR       	eodolites cajole; unusual Tiresias
4327	190	1	5	39	42517.41	0.01	0.00	N	O	1995-06-23	1995-04-18	1995-07-13	TAKE BACK RETURN         	FOB       	kages against the blit
4327	152	4	6	10	10521.50	0.00	0.06	A	F	1995-04-28	1995-06-11	1995-05-07	TAKE BACK RETURN         	TRUCK     	arefully sile
4352	106	9	1	18	18109.80	0.00	0.03	N	O	1998-02-27	1998-02-02	1998-03-01	DELIVER IN PERSON        	RAIL      	ding to th
4353	94	8	1	22	21869.98	0.05	0.05	N	O	1998-01-19	1998-01-23	1998-02-10	COLLECT COD              	FOB       	ent packages. accounts are slyly. 
4355	195	7	1	32	35046.08	0.10	0.02	N	O	1996-12-29	1997-02-08	1997-01-24	DELIVER IN PERSON        	REG AIR   	y silent deposits. b
4355	17	1	2	4	3668.04	0.05	0.02	N	O	1997-02-25	1997-01-29	1997-03-17	TAKE BACK RETURN         	TRUCK     	slyly blithely regular packag
4355	1	2	3	13	11713.00	0.07	0.05	N	O	1997-01-21	1996-12-22	1997-02-14	COLLECT COD              	TRUCK     	 ought to mold. blithely pending ideas 
4355	194	6	4	14	15318.66	0.04	0.02	N	O	1997-03-08	1997-01-22	1997-03-26	NONE                     	RAIL      	he furiously ironic accounts. quickly iro
4355	31	7	5	50	46551.50	0.10	0.00	N	O	1996-11-25	1997-01-01	1996-12-06	DELIVER IN PERSON        	REG AIR   	 regular accounts boost along the 
4355	122	7	6	35	35774.20	0.00	0.08	N	O	1997-01-28	1997-01-28	1997-02-20	NONE                     	FOB       	ess accounts affix ironic
4355	101	4	7	47	47051.70	0.09	0.02	N	O	1996-12-28	1996-12-29	1997-01-09	NONE                     	RAIL      	e. realms integrate 
4357	84	5	1	50	49204.00	0.04	0.07	N	O	1997-11-25	1997-12-03	1997-12-17	DELIVER IN PERSON        	RAIL      	s. final, e
4357	108	9	2	17	17137.70	0.10	0.07	N	O	1998-02-01	1997-12-08	1998-02-09	DELIVER IN PERSON        	MAIL      	e carefully furiou
4358	126	5	1	47	48227.64	0.04	0.00	N	O	1997-10-15	1997-10-14	1997-11-04	DELIVER IN PERSON        	SHIP      	refully busy dep
4359	174	3	1	41	44040.97	0.03	0.07	A	F	1993-04-06	1993-05-06	1993-04-14	COLLECT COD              	RAIL      	s affix sly
4359	153	8	2	8	8425.20	0.03	0.08	R	F	1993-06-27	1993-05-16	1993-07-04	DELIVER IN PERSON        	MAIL      	packages affix. fluffily regular f
4359	193	6	3	32	34982.08	0.10	0.03	R	F	1993-06-18	1993-04-04	1993-07-18	COLLECT COD              	MAIL      	olites nag quietly caref
4359	78	8	4	1	978.07	0.05	0.03	R	F	1993-04-27	1993-05-09	1993-05-08	NONE                     	MAIL      	 fluffily ironic, bold pac
4359	33	4	5	22	20526.66	0.04	0.01	A	F	1993-03-28	1993-06-01	1993-04-13	NONE                     	REG AIR   	accounts wake ironic deposits. ironic
4384	136	7	1	5	5180.65	0.09	0.01	A	F	1992-08-22	1992-08-24	1992-09-20	DELIVER IN PERSON        	MAIL      	instructions sleep. blithely express pa
4384	89	10	2	38	37585.04	0.07	0.06	A	F	1992-10-18	1992-09-24	1992-11-04	NONE                     	FOB       	ly final requests. regu
4384	89	10	3	11	10879.88	0.05	0.04	R	F	1992-08-31	1992-10-04	1992-09-28	TAKE BACK RETURN         	FOB       	deposits promise carefully even, regular e
4385	111	8	1	38	38422.18	0.00	0.02	N	O	1996-11-22	1996-10-30	1996-12-21	DELIVER IN PERSON        	TRUCK     	inal frays. final, bold exc
4387	122	5	1	3	3066.36	0.02	0.01	N	O	1996-01-17	1996-01-14	1996-01-28	COLLECT COD              	AIR       	 boost slyly ironic instructions. furiou
4387	177	5	2	48	51704.16	0.06	0.05	N	O	1995-10-29	1995-12-11	1995-11-01	NONE                     	REG AIR   	sleep slyly. blithely sl
4387	2	5	3	15	13530.00	0.00	0.03	N	O	1996-01-11	1996-01-14	1996-01-30	TAKE BACK RETURN         	REG AIR   	s hinder quietly across the pla
4387	47	8	4	9	8523.36	0.00	0.03	N	O	1996-01-04	1995-12-26	1996-01-12	DELIVER IN PERSON        	REG AIR   	c ideas. slyly regular packages sol
4387	82	3	5	3	2946.24	0.05	0.08	N	O	1995-11-17	1995-12-28	1995-11-25	COLLECT COD              	SHIP      	 pinto beans 
4387	6	3	6	40	36240.00	0.02	0.04	N	O	1995-11-29	1995-12-10	1995-12-20	NONE                     	REG AIR   	deas according to the blithely regular fox
4388	65	10	1	30	28951.80	0.02	0.07	N	O	1996-06-07	1996-05-07	1996-06-22	DELIVER IN PERSON        	FOB       	s cajole fluffil
4388	84	5	2	28	27554.24	0.05	0.04	N	O	1996-05-08	1996-06-20	1996-05-12	TAKE BACK RETURN         	RAIL      	ove the ide
4388	52	4	3	13	12376.65	0.07	0.05	N	O	1996-06-28	1996-05-23	1996-07-04	DELIVER IN PERSON        	REG AIR   	ly even, expre
4389	157	2	1	20	21143.00	0.08	0.00	A	F	1994-06-06	1994-06-17	1994-06-17	DELIVER IN PERSON        	SHIP      	ng the carefully express d
4389	153	5	2	13	13690.95	0.00	0.00	A	F	1994-08-18	1994-06-06	1994-08-20	NONE                     	RAIL      	nal, regula
4389	79	9	3	39	38183.73	0.04	0.07	A	F	1994-06-08	1994-06-04	1994-06-10	TAKE BACK RETURN         	TRUCK     	 unusual, final excuses cajole carefully 
4389	160	2	4	5	5300.80	0.09	0.00	A	F	1994-09-03	1994-06-23	1994-09-16	NONE                     	FOB       	 ironic request
4389	11	5	5	22	20042.22	0.08	0.00	R	F	1994-07-05	1994-06-12	1994-07-12	NONE                     	TRUCK     	lly silent de
4389	2	3	6	22	19844.00	0.01	0.04	R	F	1994-06-07	1994-06-29	1994-06-19	COLLECT COD              	TRUCK     	at the final excuses hinder carefully a
4389	185	6	7	4	4340.72	0.09	0.08	R	F	1994-06-14	1994-06-30	1994-07-06	NONE                     	REG AIR   	 blithely even d
4391	161	10	1	1	1061.16	0.09	0.00	R	F	1992-06-18	1992-04-27	1992-06-20	COLLECT COD              	TRUCK     	ong the silent deposits
4391	187	8	2	45	48923.10	0.07	0.04	R	F	1992-04-01	1992-05-01	1992-04-13	TAKE BACK RETURN         	TRUCK     	ep quickly after 
4417	75	5	1	28	27301.96	0.08	0.02	N	O	1998-09-04	1998-10-04	1998-09-19	TAKE BACK RETURN         	REG AIR   	ies across the furious
4417	181	2	2	1	1081.18	0.06	0.08	N	O	1998-10-23	1998-08-22	1998-10-24	NONE                     	REG AIR   	press deposits promise stealthily amo
4417	98	2	3	35	34933.15	0.06	0.04	N	O	1998-08-08	1998-09-23	1998-09-02	DELIVER IN PERSON        	FOB       	slyly regular, silent courts. even packag
4419	108	9	1	45	45364.50	0.01	0.05	N	O	1996-07-20	1996-09-07	1996-08-18	DELIVER IN PERSON        	TRUCK     	s doze sometimes fluffily regular a
4419	32	8	2	42	39145.26	0.00	0.03	N	O	1996-09-18	1996-07-25	1996-09-21	COLLECT COD              	RAIL      	sts. furious
4419	132	3	3	6	6192.78	0.02	0.08	N	O	1996-06-25	1996-09-04	1996-07-20	DELIVER IN PERSON        	AIR       	ts wake slyly final dugou
4421	98	2	1	37	36929.33	0.09	0.08	N	O	1997-07-22	1997-06-27	1997-07-25	DELIVER IN PERSON        	SHIP      	l accounts. ironic request
4421	56	1	2	46	43978.30	0.04	0.04	N	O	1997-04-21	1997-05-13	1997-05-15	DELIVER IN PERSON        	FOB       	reful packages. bold, 
4421	167	6	3	46	49089.36	0.00	0.06	N	O	1997-05-25	1997-05-21	1997-06-23	COLLECT COD              	TRUCK     	g dependenci
4421	191	4	4	32	34918.08	0.06	0.04	N	O	1997-07-09	1997-06-03	1997-07-25	NONE                     	SHIP      	ar ideas eat among the furiousl
4421	190	1	5	32	34886.08	0.06	0.04	N	O	1997-07-28	1997-06-14	1997-08-13	NONE                     	REG AIR   	uickly final pinto beans impress. bold 
4421	47	6	6	44	41669.76	0.09	0.06	N	O	1997-06-17	1997-06-20	1997-06-29	NONE                     	TRUCK     	le carefully. bl
4421	116	3	7	18	18289.98	0.01	0.00	N	O	1997-06-07	1997-05-13	1997-06-10	DELIVER IN PERSON        	FOB       	. regular, s
4423	150	9	1	3	3150.45	0.03	0.00	A	F	1995-03-22	1995-04-06	1995-04-19	NONE                     	TRUCK     	 final theodolites nag after the bli
4423	60	5	2	2	1920.12	0.07	0.04	A	F	1995-03-04	1995-04-04	1995-03-08	TAKE BACK RETURN         	REG AIR   	old sheaves sleep
4449	32	3	1	42	39145.26	0.10	0.07	N	O	1998-03-22	1998-05-09	1998-04-03	NONE                     	FOB       	 packages. blithely final 
4449	141	8	2	10	10411.40	0.02	0.03	N	O	1998-05-09	1998-05-04	1998-05-15	NONE                     	SHIP      	ccounts alongside of the platelets integr
4450	174	5	1	44	47263.48	0.10	0.00	N	O	1997-10-12	1997-10-13	1997-10-29	DELIVER IN PERSON        	RAIL      	 the slyly eve
4450	15	6	2	9	8235.09	0.03	0.03	N	O	1997-08-13	1997-08-16	1997-08-15	NONE                     	FOB       	gular requests cajole carefully. regular c
4450	96	8	3	45	44824.05	0.08	0.01	N	O	1997-09-01	1997-10-06	1997-09-19	NONE                     	TRUCK     	express ideas are furiously regular
4450	62	9	4	13	12506.78	0.00	0.00	N	O	1997-08-26	1997-09-18	1997-09-20	COLLECT COD              	MAIL      	 brave foxes. slyly unusual
4450	56	7	5	6	5736.30	0.09	0.01	N	O	1997-09-02	1997-09-30	1997-09-09	NONE                     	FOB       	eposits. foxes cajole unusual fox
4451	164	5	1	40	42566.40	0.03	0.03	A	F	1994-11-18	1994-12-25	1994-11-26	DELIVER IN PERSON        	RAIL      	y. slyly special deposits are sly
4451	63	4	2	34	32744.04	0.10	0.02	A	F	1994-11-30	1994-12-04	1994-12-13	COLLECT COD              	SHIP      	 regular ideas.
4451	159	10	3	19	20123.85	0.05	0.06	R	F	1994-10-09	1994-11-26	1994-10-23	COLLECT COD              	FOB       	ly after the fluffi
4452	114	8	1	21	21296.31	0.07	0.03	R	F	1994-10-06	1994-08-23	1994-10-15	COLLECT COD              	TRUCK     	multipliers x-ray carefully in place of 
4452	1	8	2	47	42347.00	0.01	0.06	A	F	1994-10-08	1994-08-09	1994-10-09	TAKE BACK RETURN         	TRUCK     	ts. slyly regular cour
4454	151	9	1	20	21023.00	0.10	0.08	R	F	1994-05-06	1994-03-17	1994-05-20	COLLECT COD              	SHIP      	lar theodolites. even instructio
4454	152	10	2	22	23147.30	0.06	0.02	A	F	1994-02-06	1994-04-11	1994-03-06	DELIVER IN PERSON        	RAIL      	ully. carefully final accounts accordi
4454	192	3	3	45	49148.55	0.07	0.04	A	F	1994-03-29	1994-03-26	1994-04-04	TAKE BACK RETURN         	RAIL      	ests promise. packages print fur
4454	2	3	4	1	902.00	0.09	0.05	A	F	1994-02-05	1994-04-19	1994-02-12	COLLECT COD              	RAIL      	equests run.
4454	52	4	5	48	45698.40	0.00	0.07	R	F	1994-04-23	1994-04-03	1994-04-26	COLLECT COD              	FOB       	to beans wake across th
4454	160	8	6	20	21203.20	0.10	0.03	A	F	1994-04-08	1994-03-06	1994-04-26	DELIVER IN PERSON        	TRUCK     	quickly regular requests. furiously
4480	108	5	1	30	30243.00	0.08	0.03	R	F	1994-07-29	1994-06-22	1994-08-01	NONE                     	FOB       	ven braids us
4481	24	9	1	50	46201.00	0.02	0.06	N	O	1996-07-22	1996-05-13	1996-08-14	DELIVER IN PERSON        	RAIL      	ar packages. regula
4481	190	1	2	27	29435.13	0.02	0.03	N	O	1996-04-06	1996-05-17	1996-04-12	TAKE BACK RETURN         	AIR       	ackages haggle even, 
4483	6	7	1	32	28992.00	0.07	0.07	R	F	1992-04-05	1992-05-25	1992-04-08	DELIVER IN PERSON        	MAIL      	ests haggle. slyl
4483	62	1	2	50	48103.00	0.01	0.06	A	F	1992-06-19	1992-05-12	1992-07-08	DELIVER IN PERSON        	TRUCK     	ag blithely even
4483	9	4	3	50	45450.00	0.00	0.04	R	F	1992-06-10	1992-04-18	1992-06-17	DELIVER IN PERSON        	MAIL      	ackages. furiously ironi
4484	95	9	1	4	3980.36	0.06	0.03	N	O	1997-04-09	1997-02-11	1997-04-12	TAKE BACK RETURN         	TRUCK     	packages de
4484	137	8	2	39	40448.07	0.05	0.02	N	O	1997-04-01	1997-01-26	1997-04-21	NONE                     	RAIL      	onic accounts wake blithel
4484	190	1	3	38	41427.22	0.06	0.07	N	O	1997-03-07	1997-01-31	1997-04-01	COLLECT COD              	REG AIR   	. even requests un
4484	122	5	4	41	41906.92	0.06	0.03	N	O	1997-01-25	1997-02-15	1997-01-29	TAKE BACK RETURN         	REG AIR   	ress accounts. ironic deposits unwind fur
4484	3	4	5	42	37926.00	0.03	0.07	N	O	1997-03-25	1997-02-21	1997-04-05	DELIVER IN PERSON        	REG AIR   	ding, pending requests wake. fluffily 
4484	36	7	6	29	27144.87	0.09	0.06	N	O	1996-12-27	1997-03-10	1997-01-13	NONE                     	FOB       	 wake blithely ironic
4484	103	8	7	50	50155.00	0.07	0.01	N	O	1997-03-17	1997-03-16	1997-03-21	COLLECT COD              	FOB       	the ironic, final theodo
4486	135	1	1	46	47615.98	0.08	0.00	N	O	1998-05-02	1998-04-05	1998-05-08	COLLECT COD              	MAIL      	ackages. specia
4486	49	2	2	19	18031.76	0.10	0.01	N	O	1998-06-07	1998-05-28	1998-07-02	NONE                     	MAIL      	pending foxes after
4486	96	7	3	47	46816.23	0.02	0.07	N	O	1998-04-09	1998-05-24	1998-05-07	DELIVER IN PERSON        	MAIL      	ts around the quiet packages ar
4486	91	4	4	28	27750.52	0.07	0.02	N	O	1998-04-21	1998-04-19	1998-04-26	TAKE BACK RETURN         	AIR       	to the furious, regular foxes play abov
4487	138	4	1	37	38410.81	0.03	0.07	R	F	1993-02-28	1993-04-18	1993-03-17	TAKE BACK RETURN         	MAIL      	bove the fu
4487	113	10	2	49	49642.39	0.10	0.00	R	F	1993-06-13	1993-05-08	1993-07-10	COLLECT COD              	FOB       	sual packages should ha
4487	190	1	3	1	1090.19	0.02	0.07	A	F	1993-05-11	1993-05-23	1993-05-17	TAKE BACK RETURN         	FOB       	ithely final asym
4487	93	4	4	25	24827.25	0.07	0.03	A	F	1993-03-09	1993-04-27	1993-03-30	COLLECT COD              	RAIL      	g the final instructions. slyly c
4513	170	1	1	29	31034.93	0.03	0.01	N	O	1996-05-18	1996-05-23	1996-06-08	NONE                     	REG AIR   	cajole. regular packages boost. s
4513	70	9	2	39	37832.73	0.01	0.04	N	O	1996-06-25	1996-05-14	1996-07-24	NONE                     	MAIL      	slyly furiously unusual deposits. blit
4513	138	4	3	34	35296.42	0.00	0.03	N	O	1996-03-27	1996-06-12	1996-04-06	DELIVER IN PERSON        	SHIP      	sits. quickly even instructions 
4513	192	6	4	13	14198.47	0.08	0.08	N	O	1996-04-12	1996-05-19	1996-04-25	DELIVER IN PERSON        	AIR       	l, final excuses detect furi
4515	39	10	1	15	14085.45	0.06	0.01	R	F	1992-05-26	1992-05-25	1992-06-03	NONE                     	SHIP      	posits wake
4515	103	10	2	50	50155.00	0.06	0.03	A	F	1992-03-28	1992-05-16	1992-04-20	NONE                     	AIR       	ding instructions again
4515	154	6	3	27	28462.05	0.09	0.01	A	F	1992-06-06	1992-06-08	1992-06-07	DELIVER IN PERSON        	REG AIR   	 against the even re
4515	54	5	4	32	30529.60	0.06	0.03	R	F	1992-04-07	1992-05-11	1992-04-09	COLLECT COD              	MAIL      	carefully express depo
4515	45	8	5	22	20790.88	0.09	0.07	A	F	1992-07-16	1992-05-07	1992-07-23	NONE                     	SHIP      	le quickly above the even, bold ideas.
4515	180	8	6	23	24844.14	0.04	0.00	R	F	1992-05-23	1992-06-15	1992-06-20	TAKE BACK RETURN         	FOB       	ns. bold r
4516	170	9	1	34	36385.78	0.05	0.04	A	F	1994-05-16	1994-06-23	1994-06-12	NONE                     	SHIP      	even pinto beans wake qui
4517	43	4	1	50	47152.00	0.01	0.02	N	O	1998-06-08	1998-04-18	1998-06-20	DELIVER IN PERSON        	MAIL      	refully pending acco
4519	55	3	1	30	28651.50	0.09	0.07	R	F	1993-04-11	1993-06-05	1993-04-22	DELIVER IN PERSON        	REG AIR   	totes. slyly bold somas after the 
4519	191	3	2	37	40374.03	0.06	0.08	R	F	1993-07-22	1993-06-16	1993-08-19	COLLECT COD              	AIR       	ly slyly furious depth
4545	173	1	1	38	40780.46	0.06	0.06	R	F	1993-01-27	1993-03-01	1993-02-04	NONE                     	TRUCK     	nts serve according to th
4545	63	4	2	27	26002.62	0.01	0.06	R	F	1993-02-07	1993-02-18	1993-02-18	NONE                     	FOB       	ously bold asymptotes! blithely pen
4545	87	8	3	9	8883.72	0.10	0.06	R	F	1993-03-20	1993-02-23	1993-04-11	TAKE BACK RETURN         	AIR       	xpress accounts
4545	64	9	4	2	1928.12	0.10	0.00	R	F	1993-04-16	1993-04-17	1993-05-03	NONE                     	REG AIR   	ages use. slyly even i
4545	117	1	5	27	27461.97	0.08	0.05	A	F	1993-03-18	1993-02-22	1993-03-23	NONE                     	RAIL      	ccounts haggle carefully. deposits 
4545	109	2	6	8	8072.80	0.03	0.02	A	F	1993-05-01	1993-03-12	1993-05-15	NONE                     	FOB       	 boost slyly. slyly
4545	9	2	7	36	32724.00	0.10	0.04	R	F	1993-01-28	1993-03-30	1993-02-04	DELIVER IN PERSON        	SHIP      	sublate slyly. furiously ironic accounts b
4549	159	1	1	44	46602.60	0.08	0.00	N	O	1998-03-13	1998-04-15	1998-03-27	TAKE BACK RETURN         	TRUCK     	ding to the regular, silent requests
4549	89	10	2	1	989.08	0.05	0.08	N	O	1998-05-04	1998-04-11	1998-05-14	TAKE BACK RETURN         	AIR       	 requests wake. furiously even 
4576	90	1	1	5	4950.45	0.09	0.03	N	O	1996-08-23	1996-11-08	1996-09-20	TAKE BACK RETURN         	AIR       	ly express, special asymptote
4576	58	9	2	43	41196.15	0.08	0.06	N	O	1996-10-24	1996-09-23	1996-11-10	NONE                     	SHIP      	ly final deposits. never
4576	42	1	3	14	13188.56	0.09	0.01	N	O	1996-09-12	1996-09-30	1996-09-24	COLLECT COD              	MAIL      	detect slyly.
4578	74	2	1	10	9740.70	0.09	0.06	R	F	1993-01-01	1992-11-19	1993-01-28	TAKE BACK RETURN         	REG AIR   	uests. blithely unus
4578	169	10	2	42	44904.72	0.06	0.00	R	F	1993-01-05	1992-11-06	1993-01-13	DELIVER IN PERSON        	FOB       	s are caref
4578	179	8	3	15	16187.55	0.01	0.01	R	F	1992-10-23	1992-11-22	1992-11-09	DELIVER IN PERSON        	REG AIR   	gular theodo
4578	139	10	4	7	7273.91	0.09	0.08	A	F	1992-12-07	1992-11-27	1993-01-05	TAKE BACK RETURN         	SHIP      	odolites. carefully unusual ideas accor
4578	163	2	5	20	21263.20	0.04	0.02	A	F	1993-01-11	1992-11-09	1993-01-23	TAKE BACK RETURN         	RAIL      	iously pending theodolites--
4579	175	4	1	14	15052.38	0.02	0.02	N	O	1996-02-01	1996-01-08	1996-02-08	TAKE BACK RETURN         	MAIL      	nding theodolites. fluffil
4579	42	3	2	28	26377.12	0.02	0.05	N	O	1996-01-22	1996-02-13	1996-02-03	DELIVER IN PERSON        	RAIL      	slyly across the 
4579	178	9	3	34	36657.78	0.05	0.02	N	O	1996-02-26	1996-02-22	1996-03-16	COLLECT COD              	MAIL      	hely. carefully blithe dependen
4579	120	1	4	8	8160.96	0.05	0.06	N	O	1995-12-16	1996-01-15	1995-12-18	TAKE BACK RETURN         	AIR       	posits. carefully perman
4583	141	2	1	17	17699.38	0.01	0.05	A	F	1994-11-08	1994-11-03	1994-11-29	COLLECT COD              	MAIL      	romise. reques
4583	187	8	2	43	46748.74	0.04	0.04	A	F	1994-10-30	1994-12-17	1994-11-16	COLLECT COD              	RAIL      	fully after the speci
4583	196	10	3	28	30693.32	0.00	0.07	A	F	1994-10-29	1994-11-21	1994-11-28	NONE                     	SHIP      	to beans haggle sly
4583	173	4	4	27	28975.59	0.08	0.03	R	F	1995-01-11	1994-12-24	1995-02-10	DELIVER IN PERSON        	TRUCK     	 detect silent requests. furiously speci
4583	184	5	5	36	39030.48	0.09	0.06	A	F	1995-01-06	1994-11-25	1995-01-29	DELIVER IN PERSON        	RAIL      	ar requests haggle after the furiously 
4583	122	7	6	14	14309.68	0.09	0.01	R	F	1994-11-17	1994-11-08	1994-11-21	DELIVER IN PERSON        	AIR       	detect. doggedly regular pi
4583	87	8	7	32	31586.56	0.04	0.00	A	F	1995-01-13	1994-10-29	1995-02-08	TAKE BACK RETURN         	RAIL      	across the pinto beans-- quickly
4608	173	1	1	30	32195.10	0.08	0.05	R	F	1994-10-08	1994-07-18	1994-10-25	DELIVER IN PERSON        	SHIP      	s cajole. slyly 
4608	47	8	2	50	47352.00	0.06	0.01	A	F	1994-07-25	1994-09-01	1994-08-10	NONE                     	FOB       	 theodolites
4608	79	9	3	50	48953.50	0.03	0.01	A	F	1994-08-04	1994-09-10	1994-08-13	COLLECT COD              	TRUCK     	 wake closely. even decoys haggle above
4608	31	2	4	36	33517.08	0.05	0.06	R	F	1994-10-04	1994-08-02	1994-10-21	COLLECT COD              	FOB       	ages wake quickly slyly iron
4611	52	7	1	47	44746.35	0.09	0.06	A	F	1993-03-05	1993-03-01	1993-03-17	COLLECT COD              	TRUCK     	iously. furiously regular
4611	35	6	2	31	28985.93	0.04	0.02	A	F	1993-01-28	1993-02-14	1993-01-29	TAKE BACK RETURN         	AIR       	 final pinto beans. permanent, sp
4611	82	3	3	50	49104.00	0.08	0.01	R	F	1993-01-22	1993-03-30	1993-02-16	TAKE BACK RETURN         	AIR       	l platelets. 
4611	71	9	4	48	46611.36	0.02	0.08	R	F	1993-02-28	1993-02-12	1993-03-01	COLLECT COD              	AIR       	ular accounts 
4615	92	4	1	10	9920.90	0.02	0.08	A	F	1993-11-20	1993-10-05	1993-12-08	DELIVER IN PERSON        	AIR       	sits. slyly express deposits are
4641	190	1	1	45	49058.55	0.07	0.03	R	F	1993-05-11	1993-04-19	1993-05-21	DELIVER IN PERSON        	MAIL      	 about the close 
4641	95	7	2	39	38808.51	0.06	0.00	R	F	1993-02-10	1993-03-06	1993-02-15	TAKE BACK RETURN         	REG AIR   	 the bold reque
4641	36	7	3	15	14040.45	0.01	0.08	R	F	1993-01-25	1993-04-09	1993-02-05	TAKE BACK RETURN         	AIR       	s. carefully even exc
4642	194	7	1	11	12036.09	0.04	0.07	A	F	1995-05-23	1995-04-26	1995-06-04	COLLECT COD              	TRUCK     	lithely express asympt
4642	180	10	2	34	36726.12	0.04	0.07	R	F	1995-04-01	1995-05-11	1995-04-23	COLLECT COD              	SHIP      	theodolites detect among the ironically sp
4642	21	2	3	10	9210.20	0.04	0.02	R	F	1995-04-16	1995-04-28	1995-04-24	COLLECT COD              	RAIL      	urts. even deposits nag beneath 
4642	94	7	4	18	17893.62	0.00	0.04	N	F	1995-06-16	1995-04-16	1995-06-21	NONE                     	TRUCK     	ily pending accounts hag
4642	179	10	5	41	44245.97	0.10	0.00	R	F	1995-04-08	1995-04-13	1995-05-01	DELIVER IN PERSON        	MAIL      	s are blithely. requests wake above the fur
4643	185	6	1	50	54259.00	0.08	0.05	N	O	1995-09-11	1995-08-13	1995-09-30	DELIVER IN PERSON        	SHIP      	. ironic deposits cajo
4644	177	7	1	4	4308.68	0.06	0.03	N	O	1998-05-06	1998-03-19	1998-05-28	NONE                     	MAIL      	gular requests? pendi
4644	97	8	2	16	15953.44	0.03	0.04	N	O	1998-03-13	1998-02-21	1998-04-03	COLLECT COD              	SHIP      	lar excuses across the 
4644	115	9	3	10	10151.10	0.02	0.02	N	O	1998-02-21	1998-02-28	1998-03-19	COLLECT COD              	REG AIR   	osits according to the
4644	154	2	4	45	47436.75	0.10	0.07	N	O	1998-02-02	1998-04-08	1998-02-15	COLLECT COD              	SHIP      	 carefully a
4644	87	8	5	10	9870.80	0.08	0.08	N	O	1998-03-12	1998-03-11	1998-03-19	TAKE BACK RETURN         	REG AIR   	 the slow, final fo
4647	93	6	1	16	15889.44	0.09	0.07	R	F	1994-09-07	1994-07-15	1994-10-06	COLLECT COD              	RAIL      	o beans about the fluffily special the
4647	129	2	2	34	34990.08	0.01	0.02	R	F	1994-05-20	1994-06-20	1994-05-29	COLLECT COD              	TRUCK     	ly sly accounts
4647	147	8	3	27	28272.78	0.03	0.08	R	F	1994-05-20	1994-06-26	1994-05-30	NONE                     	FOB       	ully even ti
4647	139	10	4	2	2078.26	0.04	0.07	R	F	1994-07-03	1994-07-22	1994-07-22	TAKE BACK RETURN         	RAIL      	dolites wake furiously special pinto be
4647	187	8	5	2	2174.36	0.07	0.06	A	F	1994-05-27	1994-08-05	1994-06-10	TAKE BACK RETURN         	FOB       	 pinto beans believe furiously slyly silent
4647	29	4	6	28	26012.56	0.02	0.03	A	F	1994-08-25	1994-08-06	1994-09-18	DELIVER IN PERSON        	FOB       	 are above the fluffily fin
4674	150	7	1	50	52507.50	0.07	0.08	A	F	1994-05-13	1994-06-15	1994-06-05	COLLECT COD              	RAIL      	haggle about the blithel
4674	189	10	2	35	38121.30	0.02	0.05	A	F	1994-08-02	1994-06-04	1994-08-21	COLLECT COD              	FOB       	le quickly after the express sent
4674	111	5	3	3	3033.33	0.01	0.05	A	F	1994-07-19	1994-05-28	1994-07-23	TAKE BACK RETURN         	RAIL      	 regular requests na
4674	13	7	4	21	19173.21	0.02	0.08	R	F	1994-05-08	1994-07-02	1994-06-04	COLLECT COD              	RAIL      	ent accounts sublate deposits. instruc
4675	171	2	1	6	6427.02	0.00	0.05	R	F	1994-01-22	1994-01-06	1994-02-12	TAKE BACK RETURN         	TRUCK     	 unusual ideas thrash bl
4675	144	7	2	12	12529.68	0.00	0.04	A	F	1993-12-22	1994-01-12	1993-12-23	TAKE BACK RETURN         	AIR       	posits affix carefully
4675	181	2	3	5	5405.90	0.05	0.05	A	F	1994-01-16	1994-01-05	1994-01-18	DELIVER IN PERSON        	RAIL      	lent pinto beans
4675	34	10	4	26	24284.78	0.03	0.01	A	F	1993-12-16	1993-12-29	1993-12-23	DELIVER IN PERSON        	SHIP      	nts. express requests are quickly 
4675	81	2	5	18	17659.44	0.01	0.08	R	F	1994-02-23	1994-01-18	1994-03-05	TAKE BACK RETURN         	FOB       	cajole unusual dep
4675	119	10	6	1	1019.11	0.10	0.06	R	F	1994-03-18	1994-02-14	1994-04-17	NONE                     	SHIP      	unts. caref
4676	165	2	1	47	50062.52	0.03	0.06	N	O	1995-12-20	1995-10-04	1996-01-09	NONE                     	AIR       	lithely about the carefully special requ
4676	6	1	2	33	29898.00	0.08	0.05	N	O	1995-12-29	1995-10-01	1996-01-18	TAKE BACK RETURN         	FOB       	yly express 
4676	146	3	3	4	4184.56	0.10	0.06	N	O	1995-12-12	1995-10-22	1995-12-13	TAKE BACK RETURN         	TRUCK     	detect above the ironic platelets. fluffily
4676	111	2	4	50	50555.50	0.07	0.01	N	O	1995-09-20	1995-11-20	1995-10-18	TAKE BACK RETURN         	AIR       	r deposits boost boldly quickly quick asymp
4676	122	7	5	29	29641.48	0.01	0.02	N	O	1995-12-29	1995-11-12	1996-01-06	TAKE BACK RETURN         	RAIL      	ly regular theodolites sleep.
4676	46	7	6	8	7568.32	0.08	0.08	N	O	1995-12-05	1995-10-18	1996-01-02	COLLECT COD              	AIR       	cuses boost above
4676	64	1	7	13	12532.78	0.05	0.07	N	O	1995-11-18	1995-11-07	1995-12-10	TAKE BACK RETURN         	TRUCK     	 at the slyly bold attainments. silently e
4677	128	3	1	25	25703.00	0.04	0.04	N	O	1998-04-11	1998-05-11	1998-04-18	TAKE BACK RETURN         	REG AIR   	unts doubt furiousl
4705	111	8	1	22	22244.42	0.04	0.04	R	F	1992-07-05	1992-05-11	1992-07-29	DELIVER IN PERSON        	SHIP      	 fluffily pending accounts ca
4705	31	7	2	14	13034.42	0.00	0.08	R	F	1992-07-14	1992-05-23	1992-07-25	DELIVER IN PERSON        	TRUCK     	ain carefully amon
4705	56	1	3	16	15296.80	0.07	0.08	R	F	1992-07-02	1992-06-06	1992-07-06	DELIVER IN PERSON        	RAIL      	special ideas nag sl
4705	130	3	4	31	31934.03	0.03	0.03	R	F	1992-04-03	1992-05-30	1992-04-05	COLLECT COD              	TRUCK     	furiously final accou
4705	163	10	5	28	29768.48	0.10	0.01	A	F	1992-06-03	1992-06-07	1992-06-22	DELIVER IN PERSON        	MAIL      	tes wake according to the unusual plate
4705	184	5	6	23	24936.14	0.06	0.03	R	F	1992-06-22	1992-06-11	1992-07-18	DELIVER IN PERSON        	MAIL      	 above the furiously ev
4705	89	10	7	40	39563.20	0.08	0.06	A	F	1992-04-19	1992-04-28	1992-05-07	COLLECT COD              	TRUCK     	blithely. sly
4706	182	3	1	37	40040.66	0.02	0.06	A	F	1993-02-20	1993-03-05	1993-03-03	DELIVER IN PERSON        	TRUCK     	kly final deposits c
4706	122	3	2	23	23508.76	0.03	0.01	A	F	1993-04-01	1993-03-13	1993-05-01	COLLECT COD              	FOB       	deas across t
4706	68	5	3	6	5808.36	0.01	0.04	R	F	1993-01-20	1993-03-18	1993-01-26	NONE                     	MAIL      	efully eve
4706	116	10	4	5	5080.55	0.06	0.06	R	F	1993-02-14	1993-01-31	1993-02-26	NONE                     	REG AIR   	ptotes haggle ca
4706	50	7	5	27	25651.35	0.06	0.08	A	F	1993-04-04	1993-03-11	1993-04-09	COLLECT COD              	REG AIR   	into beans. finally special instruct
4707	34	5	1	7	6538.21	0.02	0.05	R	F	1995-05-14	1995-04-06	1995-06-06	COLLECT COD              	SHIP      	ecial sheaves boost blithely accor
4707	136	7	2	49	50770.37	0.00	0.07	N	F	1995-06-17	1995-05-16	1995-06-25	COLLECT COD              	FOB       	 alongside of the slyly ironic instructio
4708	191	4	1	18	19641.42	0.02	0.04	A	F	1994-11-11	1994-11-15	1994-11-26	NONE                     	REG AIR   	special, eve
4708	75	3	2	5	4875.35	0.05	0.05	A	F	1994-10-15	1994-12-02	1994-11-12	COLLECT COD              	MAIL      	ely. carefully sp
4708	77	7	3	32	31266.24	0.04	0.07	A	F	1994-11-12	1994-11-14	1994-11-23	TAKE BACK RETURN         	MAIL      	the accounts. e
4711	133	4	1	7	7231.91	0.03	0.01	N	O	1998-05-12	1998-06-24	1998-05-24	COLLECT COD              	MAIL      	ly. bold accounts use fluff
4711	145	6	2	15	15677.10	0.08	0.07	N	O	1998-06-09	1998-07-30	1998-06-18	COLLECT COD              	SHIP      	 beans wake. deposits could bo
4711	150	1	3	22	23103.30	0.02	0.03	N	O	1998-06-21	1998-06-18	1998-07-19	TAKE BACK RETURN         	REG AIR   	along the quickly careful packages. bli
4711	65	10	4	8	7720.48	0.07	0.00	N	O	1998-06-17	1998-06-13	1998-06-27	TAKE BACK RETURN         	SHIP      	g to the carefully ironic deposits. specia
4711	49	2	5	15	14235.60	0.05	0.01	N	O	1998-09-03	1998-07-15	1998-09-13	TAKE BACK RETURN         	SHIP      	ld requests: furiously final inst
4711	116	7	6	45	45724.95	0.05	0.06	N	O	1998-05-19	1998-07-14	1998-05-21	COLLECT COD              	SHIP      	 ironic theodolites 
4711	46	5	7	18	17028.72	0.05	0.04	N	O	1998-07-03	1998-07-31	1998-07-23	DELIVER IN PERSON        	RAIL      	 blithely. bold asymptote
4736	196	10	1	26	28500.94	0.03	0.03	N	O	1996-02-02	1996-01-18	1996-02-09	DELIVER IN PERSON        	AIR       	efully speci
4736	4	1	2	43	38872.00	0.06	0.07	N	O	1996-02-05	1995-12-21	1996-02-06	COLLECT COD              	MAIL      	quests. carefully 
4738	187	8	1	9	9784.62	0.04	0.04	A	F	1992-06-01	1992-06-26	1992-06-02	COLLECT COD              	TRUCK     	posits serve slyly. unusual pint
4738	173	3	2	16	17170.72	0.07	0.08	A	F	1992-06-17	1992-06-20	1992-06-21	NONE                     	MAIL      	nic deposits are slyly! carefu
4738	100	2	3	50	50005.00	0.04	0.02	A	F	1992-06-18	1992-07-04	1992-07-07	TAKE BACK RETURN         	TRUCK     	the blithely ironic braids sleep slyly
4738	29	4	4	22	20438.44	0.02	0.08	A	F	1992-05-25	1992-05-19	1992-06-12	COLLECT COD              	SHIP      	ld, even packages. furio
4738	187	8	5	13	14133.34	0.04	0.05	R	F	1992-05-30	1992-06-11	1992-06-26	COLLECT COD              	AIR       	 wake. unusual platelets for the
4738	159	1	6	10	10591.50	0.10	0.01	R	F	1992-07-10	1992-06-16	1992-07-25	TAKE BACK RETURN         	SHIP      	hins above the
4738	83	4	7	28	27526.24	0.05	0.07	A	F	1992-06-09	1992-07-05	1992-06-25	NONE                     	AIR       	e furiously ironic excuses. care
4739	168	9	1	8	8545.28	0.07	0.07	R	F	1993-06-22	1993-05-10	1993-07-11	TAKE BACK RETURN         	SHIP      	cording to the 
4739	185	6	2	31	33640.58	0.09	0.06	R	F	1993-06-20	1993-05-18	1993-06-26	COLLECT COD              	SHIP      	blithely special pin
4739	100	4	3	30	30003.00	0.09	0.00	A	F	1993-05-29	1993-04-12	1993-06-18	NONE                     	TRUCK     	ly even packages use across th
4740	3	4	1	22	19866.00	0.06	0.01	N	O	1996-10-04	1996-08-17	1996-10-05	TAKE BACK RETURN         	RAIL      	final dependencies nag 
4740	153	5	2	24	25275.60	0.08	0.02	N	O	1996-09-10	1996-09-27	1996-10-07	TAKE BACK RETURN         	TRUCK     	hely regular deposits
4742	156	4	1	32	33796.80	0.10	0.08	R	F	1995-04-04	1995-06-12	1995-04-19	COLLECT COD              	RAIL      	eposits boost blithely. carefully regular a
4742	155	7	2	29	30599.35	0.02	0.03	N	F	1995-06-15	1995-05-05	1995-06-24	COLLECT COD              	REG AIR   	integrate closely among t
4742	72	10	3	15	14581.05	0.06	0.04	N	O	1995-07-20	1995-05-26	1995-08-11	NONE                     	SHIP      	terns are sl
4742	188	9	4	31	33733.58	0.05	0.08	N	F	1995-06-13	1995-05-08	1995-06-24	COLLECT COD              	REG AIR   	ke slyly among the furiousl
4742	100	1	5	45	45004.50	0.05	0.00	R	F	1995-05-12	1995-05-14	1995-06-07	TAKE BACK RETURN         	RAIL      	ke carefully. do
4768	36	7	1	5	4680.15	0.00	0.03	R	F	1993-12-27	1994-02-09	1994-01-11	NONE                     	MAIL      	egular accounts. bravely final fra
4770	32	8	1	41	38213.23	0.00	0.08	N	O	1995-09-04	1995-08-08	1995-09-10	COLLECT COD              	FOB       	ithely even packages sleep caref
4770	157	5	2	30	31714.50	0.09	0.07	N	O	1995-08-25	1995-08-27	1995-09-07	COLLECT COD              	SHIP      	ffily carefully ironic ideas. ironic d
4772	87	8	1	1	987.08	0.10	0.00	R	F	1994-11-13	1994-10-25	1994-11-15	DELIVER IN PERSON        	AIR       	ans. slyly even acc
4772	146	9	2	16	16738.24	0.07	0.06	R	F	1994-10-27	1994-12-07	1994-10-29	TAKE BACK RETURN         	MAIL      	egular accounts wake s
4772	95	6	3	31	30847.79	0.02	0.04	A	F	1994-10-02	1994-10-21	1994-10-13	TAKE BACK RETURN         	FOB       	ests are thinly. furiously unusua
4772	71	10	4	15	14566.05	0.02	0.07	R	F	1994-09-19	1994-10-22	1994-09-26	COLLECT COD              	TRUCK     	 requests. express, regular th
4773	144	5	1	23	24015.22	0.00	0.08	N	O	1996-01-01	1996-03-19	1996-01-04	NONE                     	FOB       	ly express grouches wak
4773	197	9	2	36	39498.84	0.09	0.04	N	O	1996-04-08	1996-03-03	1996-05-01	COLLECT COD              	REG AIR   	 dependencies. quickly
4773	167	8	3	49	52290.84	0.05	0.02	N	O	1996-01-26	1996-02-29	1996-01-27	TAKE BACK RETURN         	FOB       	y final reque
4773	20	10	4	49	45080.98	0.09	0.04	N	O	1996-01-12	1996-02-17	1996-02-05	TAKE BACK RETURN         	TRUCK     	ly pending theodolites cajole caref
4773	150	3	5	20	21003.00	0.02	0.07	N	O	1995-12-28	1996-02-17	1996-01-15	COLLECT COD              	TRUCK     	 blithely final deposits nag after t
4773	190	1	6	11	11992.09	0.10	0.06	N	O	1996-01-02	1996-01-29	1996-01-24	DELIVER IN PERSON        	REG AIR   	en accounts. slyly b
4773	158	3	7	6	6348.90	0.07	0.01	N	O	1996-03-09	1996-03-18	1996-03-27	NONE                     	AIR       	latelets haggle s
4774	84	5	1	45	44283.60	0.10	0.00	R	F	1993-07-07	1993-06-08	1993-07-31	COLLECT COD              	TRUCK     	 haggle busily afte
4774	39	5	2	4	3756.12	0.02	0.03	A	F	1993-08-03	1993-05-30	1993-08-19	COLLECT COD              	FOB       	xes according to the foxes wake above the f
4774	173	4	3	47	50438.99	0.10	0.08	R	F	1993-06-13	1993-07-04	1993-07-09	TAKE BACK RETURN         	FOB       	regular dolphins above the furi
4774	130	3	4	30	30903.90	0.05	0.08	A	F	1993-08-18	1993-06-08	1993-08-21	DELIVER IN PERSON        	REG AIR   	tions against the blithely final theodolit
4775	74	4	1	1	974.07	0.10	0.02	N	O	1995-09-06	1995-09-28	1995-09-29	DELIVER IN PERSON        	MAIL      	furiously ironic theodolite
4775	153	1	2	37	38966.55	0.02	0.01	N	O	1995-09-06	1995-09-28	1995-09-28	COLLECT COD              	TRUCK     	ts. pinto beans use according to th
4775	153	5	3	34	35807.10	0.09	0.06	N	O	1995-09-14	1995-10-15	1995-09-21	DELIVER IN PERSON        	MAIL      	onic epitaphs. f
4775	119	9	4	39	39745.29	0.07	0.04	N	O	1995-08-30	1995-10-12	1995-09-20	NONE                     	AIR       	eep never with the slyly regular acc
4800	97	10	1	11	10967.99	0.03	0.03	R	F	1992-01-27	1992-03-16	1992-02-19	TAKE BACK RETURN         	RAIL      	ic dependenc
4800	26	5	2	1	926.02	0.06	0.06	A	F	1992-02-23	1992-03-16	1992-03-20	TAKE BACK RETURN         	MAIL      	nal accounts are blithely deposits. bol
4800	11	8	3	21	19131.21	0.09	0.05	A	F	1992-02-14	1992-03-15	1992-02-26	NONE                     	SHIP      	ithely according to 
4800	176	7	4	38	40894.46	0.10	0.08	R	F	1992-02-01	1992-02-28	1992-02-21	NONE                     	TRUCK     	s sleep fluffily. furiou
4800	53	4	5	24	22873.20	0.08	0.04	R	F	1992-01-14	1992-02-23	1992-01-25	NONE                     	TRUCK     	ully carefully r
4802	40	1	1	6	5640.24	0.00	0.06	N	O	1997-04-16	1997-03-25	1997-04-21	TAKE BACK RETURN         	SHIP      	unusual accounts wake blithely. b
4804	128	1	1	44	45237.28	0.06	0.08	A	F	1992-05-02	1992-03-24	1992-05-28	TAKE BACK RETURN         	AIR       	aggle quickly among the slyly fi
4804	35	6	2	41	38336.23	0.10	0.02	R	F	1992-04-06	1992-04-12	1992-05-03	COLLECT COD              	MAIL      	. deposits haggle express tithes?
4804	65	2	3	33	31846.98	0.09	0.05	A	F	1992-03-02	1992-04-14	1992-03-13	DELIVER IN PERSON        	AIR       	, thin excuses. 
4805	150	1	1	7	7351.05	0.09	0.03	A	F	1992-05-01	1992-07-09	1992-05-09	NONE                     	FOB       	 requests. regular deposit
4805	189	10	2	45	49013.10	0.02	0.03	R	F	1992-06-16	1992-06-08	1992-07-03	NONE                     	TRUCK     	the furiously sly t
4805	154	6	3	44	46382.60	0.01	0.02	R	F	1992-05-14	1992-06-23	1992-05-25	DELIVER IN PERSON        	SHIP      	eposits sleep furiously qui
4805	65	2	4	13	12545.78	0.04	0.04	R	F	1992-07-16	1992-06-07	1992-08-10	COLLECT COD              	AIR       	its serve about the accounts. slyly regu
4805	9	10	5	42	38178.00	0.03	0.03	R	F	1992-08-17	1992-07-03	1992-09-14	NONE                     	REG AIR   	the regular, fina
4805	136	7	6	18	18650.34	0.06	0.04	A	F	1992-06-07	1992-07-10	1992-06-12	COLLECT COD              	TRUCK     	o use pending, unusu
4835	179	10	1	18	19425.06	0.00	0.03	R	F	1995-02-17	1994-12-14	1995-03-17	DELIVER IN PERSON        	MAIL      	eat furiously against the slyly 
4835	91	3	2	3	2973.27	0.09	0.06	R	F	1995-01-24	1995-01-12	1995-02-16	COLLECT COD              	AIR       	etimes final pac
4835	86	7	3	27	26624.16	0.05	0.00	A	F	1994-12-10	1994-12-13	1995-01-02	DELIVER IN PERSON        	REG AIR   	 accounts after the car
4835	102	7	4	23	23048.30	0.08	0.07	A	F	1995-02-05	1995-01-04	1995-02-28	NONE                     	SHIP      	e carefully regular foxes. deposits are sly
4837	42	1	1	16	15072.64	0.09	0.04	N	O	1998-08-12	1998-06-06	1998-08-26	COLLECT COD              	FOB       	ing requests are blithely regular instructi
4837	193	5	2	16	17491.04	0.01	0.02	N	O	1998-08-19	1998-06-18	1998-08-26	NONE                     	RAIL      	counts cajole slyly furiou
4837	68	5	3	42	40658.52	0.10	0.00	N	O	1998-06-19	1998-07-06	1998-06-23	COLLECT COD              	MAIL      	o the furiously final theodolites boost
4839	60	2	1	5	4800.30	0.10	0.07	A	F	1994-09-07	1994-07-15	1994-10-05	DELIVER IN PERSON        	FOB       	ses integrate. regular deposits are about 
4839	10	1	2	25	22750.25	0.02	0.02	R	F	1994-05-20	1994-07-08	1994-05-30	NONE                     	REG AIR   	regular packages ab
4839	60	1	3	18	17281.08	0.06	0.01	R	F	1994-05-18	1994-06-13	1994-06-09	TAKE BACK RETURN         	FOB       	blithely ironic theodolites use along
4839	100	1	4	19	19001.90	0.07	0.08	R	F	1994-05-20	1994-07-14	1994-05-30	NONE                     	REG AIR   	 deposits sublate furiously ir
4839	71	10	5	9	8739.63	0.05	0.01	R	F	1994-06-17	1994-06-18	1994-07-10	NONE                     	SHIP      	ounts haggle carefully above
4865	162	7	1	16	16994.56	0.07	0.05	N	O	1997-10-02	1997-08-20	1997-10-04	COLLECT COD              	TRUCK     	osits haggle. fur
4865	137	8	2	4	4148.52	0.07	0.01	N	O	1997-07-24	1997-07-25	1997-08-07	TAKE BACK RETURN         	FOB       	sts. blithely special instruction
4865	68	3	3	44	42594.64	0.10	0.08	N	O	1997-07-25	1997-08-20	1997-08-22	COLLECT COD              	FOB       	even deposits sleep against the quickly r
4865	50	3	4	21	19951.05	0.04	0.02	N	O	1997-07-17	1997-08-10	1997-07-21	NONE                     	RAIL      	eposits detect sly
4865	54	9	5	33	31483.65	0.00	0.05	N	O	1997-07-17	1997-08-16	1997-07-30	TAKE BACK RETURN         	FOB       	y pending notornis ab
4865	65	2	6	47	45357.82	0.00	0.05	N	O	1997-08-26	1997-08-07	1997-08-31	NONE                     	RAIL      	y unusual packages. packages
4867	82	3	1	7	6874.56	0.09	0.03	A	F	1992-07-17	1992-08-17	1992-07-22	COLLECT COD              	FOB       	e carefully even packages. slyly ironic i
4867	160	8	2	3	3180.48	0.04	0.08	R	F	1992-07-04	1992-07-15	1992-07-21	NONE                     	AIR       	yly silent deposits
4868	73	3	1	47	45734.29	0.03	0.03	N	O	1997-04-29	1997-04-27	1997-05-11	DELIVER IN PERSON        	SHIP      	gle unusual, fluffy packages. foxes cajol
4868	180	1	2	8	8641.44	0.10	0.08	N	O	1997-03-26	1997-05-09	1997-04-16	NONE                     	RAIL      	ly special th
4868	191	2	3	49	53468.31	0.09	0.03	N	O	1997-04-23	1997-05-07	1997-04-26	NONE                     	SHIP      	ys engage. th
4868	80	1	4	34	33322.72	0.04	0.02	N	O	1997-05-19	1997-04-27	1997-06-15	NONE                     	RAIL      	en instructions about th
4868	122	3	5	22	22486.64	0.07	0.06	N	O	1997-04-26	1997-05-16	1997-05-01	DELIVER IN PERSON        	FOB       	osits. final foxes boost regular,
4870	48	5	1	49	46453.96	0.05	0.05	R	F	1994-11-14	1994-10-24	1994-12-12	TAKE BACK RETURN         	SHIP      	 regular packages 
4870	127	10	2	6	6162.72	0.06	0.08	A	F	1994-09-09	1994-10-16	1994-09-21	DELIVER IN PERSON        	TRUCK     	ress requests. bold, silent pinto bea
4870	31	2	3	5	4655.15	0.05	0.00	R	F	1994-10-11	1994-10-07	1994-10-24	NONE                     	AIR       	s haggle furiously. slyly ironic dinos
4870	6	9	4	4	3624.00	0.03	0.08	A	F	1994-10-23	1994-09-16	1994-11-04	COLLECT COD              	RAIL      	its wake quickly. slyly quick
4870	71	1	5	36	34958.52	0.09	0.06	A	F	1994-09-06	1994-09-17	1994-10-01	COLLECT COD              	REG AIR   	 instructions. carefully pending pac
4871	177	5	1	14	15080.38	0.07	0.03	N	O	1995-09-30	1995-07-29	1995-10-18	TAKE BACK RETURN         	REG AIR   	inst the never ironic 
4871	161	6	2	17	18039.72	0.07	0.03	N	O	1995-09-09	1995-09-01	1995-10-02	DELIVER IN PERSON        	AIR       	es. carefully ev
4871	63	4	3	3	2889.18	0.03	0.06	N	O	1995-10-03	1995-08-10	1995-10-06	DELIVER IN PERSON        	TRUCK     	y special packages wak
4871	149	8	4	35	36719.90	0.08	0.07	N	O	1995-08-11	1995-07-18	1995-08-29	DELIVER IN PERSON        	TRUCK     	ackages sle
4871	152	3	5	10	10521.50	0.09	0.02	N	O	1995-09-12	1995-09-02	1995-10-05	TAKE BACK RETURN         	AIR       	s integrate after the a
4871	136	2	6	36	37300.68	0.02	0.08	N	O	1995-09-18	1995-08-29	1995-10-05	TAKE BACK RETURN         	AIR       	ely according
4871	140	6	7	10	10401.40	0.10	0.02	N	O	1995-07-13	1995-08-19	1995-07-29	NONE                     	REG AIR   	p ironic theodolites. slyly even platel
4896	41	2	1	19	17879.76	0.09	0.05	A	F	1992-12-13	1992-11-13	1993-01-09	NONE                     	AIR       	nusual requ
4896	140	1	2	44	45766.16	0.04	0.03	A	F	1992-11-24	1992-11-15	1992-12-18	COLLECT COD              	MAIL      	e after the slowly f
4896	58	10	3	6	5748.30	0.04	0.04	A	F	1992-10-30	1992-11-12	1992-11-28	DELIVER IN PERSON        	TRUCK     	usly regular deposits
4896	23	4	4	5	4615.10	0.08	0.02	R	F	1992-12-02	1992-11-11	1992-12-19	COLLECT COD              	SHIP      	eposits hang carefully. sly
4896	86	7	5	21	20707.68	0.07	0.08	R	F	1992-11-18	1992-11-18	1992-11-29	DELIVER IN PERSON        	TRUCK     	ly express deposits. carefully pending depo
4897	55	6	1	26	24831.30	0.01	0.01	R	F	1992-12-22	1992-10-25	1992-12-27	DELIVER IN PERSON        	TRUCK     	. carefully ironic dep
4897	143	6	2	34	35466.76	0.02	0.00	R	F	1992-12-31	1992-11-11	1993-01-30	COLLECT COD              	AIR       	ts. special dependencies use fluffily 
4897	55	7	3	42	40112.10	0.09	0.03	A	F	1992-09-23	1992-10-28	1992-10-02	DELIVER IN PERSON        	FOB       	sts. blithely regular deposits will have
4897	104	5	4	19	19077.90	0.03	0.00	A	F	1992-11-08	1992-12-14	1992-12-03	DELIVER IN PERSON        	FOB       	! ironic, pending dependencies doze furiou
4898	72	1	1	44	42771.08	0.07	0.02	A	F	1994-09-13	1994-08-18	1994-09-16	NONE                     	FOB       	y regular grouches about
4902	196	10	1	22	24116.18	0.00	0.04	N	O	1998-10-17	1998-08-10	1998-10-21	COLLECT COD              	RAIL      	r the furiously final fox
4902	83	4	2	1	983.08	0.09	0.04	N	O	1998-10-12	1998-08-20	1998-11-08	NONE                     	RAIL      	daring foxes? even, bold requests wake f
4903	121	2	1	1	1021.12	0.06	0.03	R	F	1992-04-23	1992-06-13	1992-05-03	NONE                     	SHIP      	nusual requests
4903	165	6	2	6	6390.96	0.09	0.07	R	F	1992-04-01	1992-05-16	1992-04-11	DELIVER IN PERSON        	SHIP      	azzle quickly along the blithely final pla
4903	120	10	3	27	27543.24	0.07	0.06	A	F	1992-06-29	1992-06-09	1992-07-08	COLLECT COD              	RAIL      	pinto beans are; 
4928	100	1	1	4	4000.40	0.04	0.02	R	F	1993-10-25	1993-12-24	1993-11-16	TAKE BACK RETURN         	REG AIR   	bout the slyly final accounts. carefull
4928	93	4	2	20	19861.80	0.03	0.08	A	F	1994-01-19	1993-11-29	1994-02-13	DELIVER IN PERSON        	SHIP      	quiet theodolites ca
4928	149	8	3	34	35670.76	0.06	0.05	A	F	1993-10-12	1993-12-31	1993-10-14	DELIVER IN PERSON        	AIR       	, regular depos
4931	194	7	1	1	1094.19	0.08	0.06	A	F	1995-01-24	1994-12-19	1995-02-07	DELIVER IN PERSON        	SHIP      	 furiously 
4931	151	3	2	8	8409.20	0.06	0.02	R	F	1994-12-15	1995-01-14	1995-01-06	NONE                     	SHIP      	ts boost. packages wake sly
4931	144	5	3	20	20882.80	0.09	0.00	A	F	1995-01-25	1994-12-21	1995-02-06	DELIVER IN PERSON        	MAIL      	the furious
4931	200	4	4	50	55010.00	0.04	0.01	A	F	1994-12-15	1994-12-18	1994-12-23	COLLECT COD              	REG AIR   	s haggle al
4931	150	7	5	25	26253.75	0.05	0.05	R	F	1994-12-19	1995-01-05	1994-12-21	COLLECT COD              	FOB       	aggle bravely according to the quic
4931	103	6	6	8	8024.80	0.02	0.03	A	F	1995-02-16	1994-12-30	1995-03-15	DELIVER IN PERSON        	SHIP      	dependencies are slyly
4932	51	3	1	13	12363.65	0.04	0.03	A	F	1993-09-13	1993-10-16	1993-09-20	DELIVER IN PERSON        	SHIP      	slyly according to the furiously fin
4932	103	10	2	15	15046.50	0.01	0.02	R	F	1993-11-15	1993-10-25	1993-11-29	NONE                     	REG AIR   	yly. unusu
4932	87	8	3	5	4935.40	0.06	0.06	A	F	1993-10-01	1993-09-13	1993-10-04	NONE                     	MAIL      	 haggle furiously. slyly ironic packages sl
4932	98	1	4	11	10978.99	0.09	0.06	A	F	1993-09-21	1993-09-30	1993-09-23	COLLECT COD              	SHIP      	as. special depende
4933	32	8	1	48	44737.44	0.08	0.00	N	O	1995-10-10	1995-10-03	1995-11-04	COLLECT COD              	SHIP      	ideas. sly
4933	82	3	2	2	1964.16	0.09	0.00	N	O	1995-10-01	1995-09-29	1995-10-19	DELIVER IN PERSON        	MAIL      	ctions nag final instructions. accou
4934	97	10	1	48	47860.32	0.00	0.01	N	O	1997-05-20	1997-04-22	1997-06-02	TAKE BACK RETURN         	SHIP      	 ideas cajol
4934	110	1	2	41	41414.51	0.06	0.06	N	O	1997-06-04	1997-04-11	1997-06-25	TAKE BACK RETURN         	FOB       	wake final, ironic f
4934	140	1	3	8	8321.12	0.03	0.06	N	O	1997-05-20	1997-04-30	1997-05-27	TAKE BACK RETURN         	MAIL      	arefully express pains cajo
4934	148	5	4	9	9433.26	0.06	0.08	N	O	1997-06-10	1997-04-09	1997-06-12	TAKE BACK RETURN         	REG AIR   	 haggle alongside of the
4934	138	9	5	29	30105.77	0.09	0.03	N	O	1997-04-10	1997-05-05	1997-05-04	DELIVER IN PERSON        	AIR       	aggle furiously among the busily final re
4934	52	3	6	42	39986.10	0.00	0.07	N	O	1997-03-19	1997-05-05	1997-03-25	NONE                     	MAIL      	ven, ironic ideas
4934	11	5	7	2	1822.02	0.10	0.06	N	O	1997-06-05	1997-03-26	1997-06-09	COLLECT COD              	MAIL      	ongside of the brave, regula
4935	161	2	1	13	13795.08	0.09	0.01	A	F	1993-06-20	1993-08-13	1993-06-27	COLLECT COD              	REG AIR   	ly requests. final deposits might 
4935	40	6	2	37	34781.48	0.01	0.05	R	F	1993-08-30	1993-07-23	1993-09-07	TAKE BACK RETURN         	RAIL      	y even dependencies nag a
4935	11	8	3	24	21864.24	0.06	0.04	A	F	1993-05-29	1993-08-17	1993-06-22	NONE                     	RAIL      	ly quickly s
4935	45	6	4	49	46306.96	0.06	0.01	A	F	1993-09-16	1993-08-21	1993-10-12	COLLECT COD              	TRUCK     	ffily after the furiou
4935	10	1	5	14	12740.14	0.08	0.08	A	F	1993-05-30	1993-07-25	1993-05-31	COLLECT COD              	FOB       	slowly. blith
4935	188	9	6	36	39174.48	0.10	0.00	R	F	1993-07-11	1993-07-04	1993-08-01	DELIVER IN PERSON        	RAIL      	requests across the quick
4962	19	6	1	46	42274.46	0.01	0.07	R	F	1993-08-23	1993-09-04	1993-08-27	COLLECT COD              	REG AIR   	 pinto beans grow about the sl
4964	133	9	1	29	29960.77	0.04	0.01	N	O	1997-10-18	1997-08-30	1997-11-01	NONE                     	AIR       	k accounts nag carefully-- ironic, fin
4964	148	5	2	46	48214.44	0.06	0.06	N	O	1997-10-05	1997-09-12	1997-10-11	NONE                     	TRUCK     	althy deposits
4964	143	4	3	18	18776.52	0.00	0.06	N	O	1997-10-13	1997-09-01	1997-11-10	DELIVER IN PERSON        	AIR       	 platelets. furio
4964	180	10	4	12	12962.16	0.08	0.01	N	O	1997-09-03	1997-10-25	1997-09-15	NONE                     	TRUCK     	ully silent instructions ca
4964	41	10	5	42	39523.68	0.06	0.04	N	O	1997-09-04	1997-08-28	1997-10-02	TAKE BACK RETURN         	AIR       	 hinder. idly even
4964	193	7	6	22	24050.18	0.04	0.08	N	O	1997-09-11	1997-10-06	1997-09-29	NONE                     	AIR       	equests doubt quickly. caref
4964	173	4	7	28	30048.76	0.00	0.05	N	O	1997-08-30	1997-09-15	1997-09-18	COLLECT COD              	RAIL      	among the carefully regula
4965	131	2	1	28	28871.64	0.05	0.03	A	F	1994-01-02	1993-11-20	1994-01-04	TAKE BACK RETURN         	REG AIR   	 deposits. requests sublate quickly 
4965	13	10	2	25	22825.25	0.10	0.02	R	F	1994-02-05	1993-12-15	1994-02-24	TAKE BACK RETURN         	MAIL      	wake at the carefully speci
4965	101	8	3	27	27029.70	0.05	0.06	R	F	1993-11-06	1993-12-24	1993-11-30	TAKE BACK RETURN         	SHIP      	efully final foxes
4965	138	9	4	33	34258.29	0.04	0.04	A	F	1993-12-31	1993-11-29	1994-01-27	DELIVER IN PERSON        	REG AIR   	iously slyly
4967	71	1	1	50	48553.50	0.07	0.01	N	O	1997-05-27	1997-05-13	1997-06-12	NONE                     	REG AIR   	kages. final, unusual accounts c
4967	53	5	2	43	40981.15	0.00	0.07	N	O	1997-05-28	1997-04-10	1997-06-09	NONE                     	TRUCK     	ons. slyly ironic requests
4967	123	2	4	1	1023.12	0.10	0.07	N	O	1997-06-04	1997-03-29	1997-06-23	NONE                     	FOB       	osits. unusual frets thrash furiously
4992	184	5	1	42	45535.56	0.07	0.01	R	F	1992-07-19	1992-06-16	1992-08-17	TAKE BACK RETURN         	RAIL      	foxes about the quickly final platele
4992	147	4	2	47	49215.58	0.10	0.08	A	F	1992-09-04	1992-08-05	1992-09-21	COLLECT COD              	MAIL      	atterns use fluffily.
4992	144	7	3	17	17750.38	0.03	0.03	A	F	1992-07-05	1992-07-19	1992-07-30	TAKE BACK RETURN         	FOB       	s along the perma
4992	70	7	4	25	24251.75	0.04	0.06	R	F	1992-08-06	1992-07-11	1992-08-20	NONE                     	SHIP      	ly about the never ironic requests. pe
4992	139	5	5	23	23899.99	0.01	0.08	R	F	1992-06-28	1992-07-15	1992-07-12	DELIVER IN PERSON        	MAIL      	uickly regul
4992	163	8	6	44	46779.04	0.05	0.02	A	F	1992-06-01	1992-07-22	1992-06-03	NONE                     	RAIL      	rmanent, sly packages print slyly. regula
4993	38	4	1	34	31893.02	0.05	0.00	R	F	1994-09-21	1994-10-31	1994-09-24	TAKE BACK RETURN         	REG AIR   	ular, pending packages at the even packa
4993	129	4	2	39	40135.68	0.03	0.08	R	F	1994-09-10	1994-09-04	1994-09-26	COLLECT COD              	SHIP      	pending, regular requests solve caref
4993	166	1	3	42	44778.72	0.06	0.00	A	F	1994-08-27	1994-09-24	1994-09-05	NONE                     	MAIL      	 final packages at the q
4993	158	6	4	31	32802.65	0.10	0.06	A	F	1994-10-02	1994-10-29	1994-10-15	NONE                     	AIR       	nwind thinly platelets. a
4995	65	4	1	16	15440.96	0.02	0.05	N	O	1996-02-27	1996-04-03	1996-02-29	DELIVER IN PERSON        	MAIL      	egular, bold packages. accou
4995	81	2	2	43	42186.44	0.00	0.06	N	O	1996-02-24	1996-02-20	1996-03-07	NONE                     	AIR       	ts. blithely silent ideas after t
4995	156	7	3	22	23235.30	0.03	0.06	N	O	1996-03-17	1996-03-12	1996-04-01	DELIVER IN PERSON        	MAIL      	s wake furious, express dependencies.
4995	40	1	4	9	8460.36	0.07	0.07	N	O	1996-03-07	1996-03-17	1996-03-11	DELIVER IN PERSON        	FOB       	 ironic packages cajole across t
4995	148	7	5	48	50310.72	0.08	0.07	N	O	1996-03-22	1996-04-01	1996-04-07	NONE                     	SHIP      	t blithely. requests affix blithely. 
4995	110	5	6	48	48485.28	0.09	0.07	N	O	1996-04-14	1996-04-04	1996-05-07	DELIVER IN PERSON        	RAIL      	nstructions. carefully final depos
4997	79	7	1	44	43079.08	0.02	0.05	N	O	1998-06-09	1998-06-12	1998-07-07	NONE                     	RAIL      	r escapades ca
4997	17	7	2	5	4585.05	0.02	0.04	N	O	1998-05-16	1998-06-05	1998-06-07	COLLECT COD              	REG AIR   	cuses are furiously unusual asymptotes
4997	58	9	3	24	22993.20	0.04	0.06	N	O	1998-04-20	1998-04-23	1998-05-16	NONE                     	AIR       	xpress, bo
4997	40	6	4	5	4700.20	0.10	0.03	N	O	1998-06-12	1998-04-24	1998-06-13	DELIVER IN PERSON        	TRUCK     	aggle slyly alongside of the slyly i
4997	22	7	5	46	42412.92	0.00	0.04	N	O	1998-04-28	1998-06-04	1998-05-08	TAKE BACK RETURN         	SHIP      	ecial courts are carefully
4997	29	2	6	2	1858.04	0.07	0.01	N	O	1998-07-09	1998-06-10	1998-07-21	TAKE BACK RETURN         	REG AIR   	counts. slyl
4999	153	8	1	30	31594.50	0.00	0.02	A	F	1993-08-20	1993-08-15	1993-08-30	NONE                     	AIR       	ades cajole carefully unusual ide
4999	10	1	2	44	40040.44	0.03	0.01	A	F	1993-08-01	1993-08-04	1993-08-17	COLLECT COD              	REG AIR   	ependencies. slowly regu
4999	86	7	3	30	29582.40	0.09	0.01	R	F	1993-07-21	1993-08-11	1993-08-20	DELIVER IN PERSON        	RAIL      	s cajole among the blithel
5026	96	8	1	13	12949.17	0.02	0.04	N	O	1997-12-23	1997-11-02	1998-01-03	TAKE BACK RETURN         	SHIP      	endencies sleep carefully alongs
5027	98	2	1	6	5988.54	0.04	0.05	N	O	1997-09-28	1997-11-24	1997-10-25	NONE                     	FOB       	ar, ironic deposi
5027	62	3	2	39	37520.34	0.06	0.01	N	O	1997-09-09	1997-11-13	1997-09-21	TAKE BACK RETURN         	FOB       	ess requests! quickly regular pac
5027	126	5	3	32	32835.84	0.00	0.01	N	O	1997-11-13	1997-10-29	1997-11-18	TAKE BACK RETURN         	RAIL      	cording to
5027	26	7	4	37	34262.74	0.02	0.00	N	O	1997-10-05	1997-10-30	1997-10-26	NONE                     	REG AIR   	ost slyly fluffily
5027	143	4	5	3	3129.42	0.03	0.06	N	O	1997-09-30	1997-11-26	1997-10-05	DELIVER IN PERSON        	AIR       	t the even mu
5027	87	8	6	25	24677.00	0.06	0.00	N	O	1997-09-16	1997-11-25	1997-10-08	TAKE BACK RETURN         	RAIL      	ic ideas. requests sleep fluffily am
5027	81	2	7	50	49054.00	0.07	0.02	N	O	1997-09-18	1997-11-07	1997-10-05	DELIVER IN PERSON        	MAIL      	 beans dazzle according to the fluffi
5028	14	1	1	15	13710.15	0.07	0.07	R	F	1992-07-17	1992-07-16	1992-08-05	COLLECT COD              	REG AIR   	es are quickly final pains. furiously pend
5028	199	10	2	15	16487.85	0.03	0.07	R	F	1992-08-02	1992-07-09	1992-08-30	NONE                     	REG AIR   	gular, bold pinto bea
5030	102	3	1	22	22046.20	0.04	0.06	N	O	1998-09-01	1998-08-15	1998-09-30	TAKE BACK RETURN         	TRUCK     	. quickly regular foxes believe
5030	80	9	2	50	49004.00	0.05	0.06	N	O	1998-08-22	1998-07-25	1998-09-18	TAKE BACK RETURN         	FOB       	ss excuses serve bli
5031	50	1	1	15	14250.75	0.02	0.05	R	F	1995-04-01	1995-02-24	1995-04-12	DELIVER IN PERSON        	AIR       	yly pending theodolites.
5031	161	6	2	40	42446.40	0.10	0.04	A	F	1994-12-04	1995-01-27	1995-01-01	NONE                     	TRUCK     	ns hang blithely across th
5031	154	6	3	4	4216.60	0.01	0.07	R	F	1994-12-26	1995-02-24	1995-01-11	NONE                     	RAIL      	after the even frays: ironic, unusual th
5031	181	2	4	31	33516.58	0.10	0.08	R	F	1995-01-15	1995-01-08	1995-02-09	COLLECT COD              	MAIL      	ts across the even requests doze furiously
5056	48	7	1	7	6636.28	0.09	0.01	N	O	1997-04-28	1997-04-07	1997-05-15	DELIVER IN PERSON        	TRUCK     	rouches after the pending instruc
5056	197	1	2	19	20846.61	0.04	0.00	N	O	1997-03-24	1997-05-05	1997-04-23	DELIVER IN PERSON        	AIR       	c theodolites. ironic a
5056	90	1	3	23	22772.07	0.02	0.05	N	O	1997-05-12	1997-04-28	1997-05-25	NONE                     	SHIP      	ickly regular requests cajole. depos
5056	87	8	4	14	13819.12	0.08	0.00	N	O	1997-06-09	1997-04-13	1997-07-06	COLLECT COD              	SHIP      	sts haggle carefully along the slyl
5057	37	3	1	38	35607.14	0.02	0.03	N	O	1997-10-24	1997-09-07	1997-10-30	TAKE BACK RETURN         	MAIL      	packages. stealthily bold wa
5057	8	1	2	45	40860.00	0.08	0.07	N	O	1997-09-20	1997-10-02	1997-10-20	NONE                     	FOB       	 asymptotes wake slyl
5058	193	5	1	16	17491.04	0.09	0.07	N	O	1998-07-12	1998-06-09	1998-07-15	DELIVER IN PERSON        	SHIP      	 the special foxes 
5061	165	2	1	18	19172.88	0.03	0.00	A	F	1993-10-20	1993-10-05	1993-10-28	TAKE BACK RETURN         	SHIP      	atelets among the ca
5061	198	1	2	8	8785.52	0.01	0.02	R	F	1993-09-07	1993-10-31	1993-10-04	DELIVER IN PERSON        	REG AIR   	regular foxes. ir
5061	24	5	3	26	24024.52	0.02	0.05	A	F	1993-11-07	1993-09-13	1993-11-13	NONE                     	REG AIR   	 cajole slyly. carefully spe
5063	129	10	1	31	31902.72	0.08	0.01	N	O	1997-06-02	1997-06-20	1997-06-27	NONE                     	RAIL      	kages. ironic, ironic courts wake. carefu
5063	174	2	2	43	46189.31	0.04	0.08	N	O	1997-09-14	1997-07-05	1997-10-05	TAKE BACK RETURN         	TRUCK     	latelets might nod blithely regular requ
5063	167	4	3	2	2134.32	0.02	0.03	N	O	1997-06-17	1997-07-27	1997-06-24	COLLECT COD              	SHIP      	kly regular i
5063	135	6	4	18	18632.34	0.08	0.05	N	O	1997-06-02	1997-06-18	1997-06-06	TAKE BACK RETURN         	RAIL      	refully quiet reques
5063	161	8	5	1	1061.16	0.06	0.07	N	O	1997-09-03	1997-06-26	1997-10-03	NONE                     	FOB       	ously special 
5088	78	6	1	23	22495.61	0.06	0.06	R	F	1993-03-03	1993-03-07	1993-03-08	NONE                     	FOB       	cording to the fluffily expr
5088	51	3	2	41	38993.05	0.09	0.00	R	F	1993-01-22	1993-03-07	1993-02-09	TAKE BACK RETURN         	TRUCK     	ing requests. 
5088	86	7	3	36	35498.88	0.10	0.05	A	F	1993-04-16	1993-04-03	1993-05-14	NONE                     	TRUCK     	the furiously final deposits. furiously re
5088	109	6	4	10	10091.00	0.04	0.05	R	F	1993-04-07	1993-02-06	1993-04-26	NONE                     	FOB       	beans. special requests af
5089	158	6	1	4	4232.60	0.05	0.06	R	F	1992-09-18	1992-09-28	1992-10-13	DELIVER IN PERSON        	TRUCK     	nts sleep blithely 
5089	162	3	2	20	21243.20	0.00	0.07	R	F	1992-10-10	1992-10-07	1992-11-06	COLLECT COD              	RAIL      	 ironic accounts
5089	124	7	3	46	47109.52	0.03	0.04	A	F	1992-11-09	1992-10-13	1992-11-10	TAKE BACK RETURN         	RAIL      	above the express accounts. exc
5089	34	10	4	38	35493.14	0.05	0.03	R	F	1992-11-23	1992-09-11	1992-12-22	TAKE BACK RETURN         	SHIP      	regular instructions are
5091	78	6	1	50	48903.50	0.05	0.03	N	O	1998-07-21	1998-06-22	1998-07-26	COLLECT COD              	REG AIR   	al dependencies. r
5092	164	1	1	30	31924.80	0.06	0.00	N	O	1995-12-27	1995-12-08	1996-01-09	DELIVER IN PERSON        	MAIL      	ss, ironic deposits. furiously stea
5092	45	4	2	34	32131.36	0.04	0.02	N	O	1995-12-09	1995-12-26	1995-12-21	TAKE BACK RETURN         	AIR       	ckages nag 
5092	140	6	3	13	13521.82	0.06	0.01	N	O	1995-11-21	1996-01-05	1995-12-19	TAKE BACK RETURN         	SHIP      	es detect sly
5092	180	1	4	14	15122.52	0.04	0.00	N	O	1996-02-20	1995-11-30	1996-03-20	DELIVER IN PERSON        	REG AIR   	 deposits cajole furiously against the sly
5092	186	7	5	42	45619.56	0.01	0.02	N	O	1995-11-06	1996-01-01	1995-12-06	DELIVER IN PERSON        	AIR       	s use along t
5092	178	6	6	11	11859.87	0.03	0.03	N	O	1995-12-02	1995-12-27	1995-12-11	COLLECT COD              	MAIL      	ly against the slyly silen
5092	159	10	7	50	52957.50	0.10	0.03	N	O	1995-11-30	1996-01-14	1995-12-19	NONE                     	REG AIR   	r platelets maintain car
5094	143	10	1	19	19819.66	0.03	0.03	R	F	1993-03-31	1993-06-12	1993-04-04	NONE                     	AIR       	ronic foxes. furi
5094	108	5	2	23	23186.30	0.05	0.07	R	F	1993-06-13	1993-05-19	1993-07-06	NONE                     	MAIL      	st furiously above the fluffily care
5094	92	6	3	11	10912.99	0.04	0.08	A	F	1993-06-25	1993-06-24	1993-07-18	TAKE BACK RETURN         	MAIL      	s cajole quickly against the furiously ex
5094	79	10	4	21	20560.47	0.09	0.08	R	F	1993-07-26	1993-05-03	1993-08-16	NONE                     	MAIL      	 blithely furiously final re
5120	133	4	1	28	28927.64	0.06	0.03	N	O	1996-07-20	1996-08-31	1996-08-06	NONE                     	RAIL      	 across the silent requests. caref
5123	26	7	1	13	12038.26	0.08	0.07	N	O	1998-05-17	1998-03-23	1998-06-02	COLLECT COD              	MAIL      	regular pearls
5124	55	7	1	43	41067.15	0.00	0.02	N	O	1997-07-10	1997-05-13	1997-07-31	COLLECT COD              	AIR       	onic package
5124	6	3	2	41	37146.00	0.02	0.06	N	O	1997-07-05	1997-06-29	1997-07-23	DELIVER IN PERSON        	RAIL      	wake across the
5124	125	6	3	44	45105.28	0.03	0.03	N	O	1997-07-13	1997-06-26	1997-08-01	DELIVER IN PERSON        	RAIL      	equests. carefully unusual d
5124	70	9	4	36	34922.52	0.10	0.07	N	O	1997-04-20	1997-07-03	1997-05-04	TAKE BACK RETURN         	AIR       	r deposits ab
5125	6	9	1	38	34428.00	0.09	0.05	N	O	1998-03-20	1998-04-14	1998-03-22	COLLECT COD              	MAIL      	ily even deposits w
5125	160	1	2	5	5300.80	0.08	0.06	N	O	1998-04-07	1998-04-14	1998-04-29	COLLECT COD              	RAIL      	 thinly even pack
5127	19	3	1	33	30327.33	0.08	0.04	N	O	1997-03-25	1997-03-02	1997-04-04	NONE                     	SHIP      	 bold deposits use carefully a
5127	32	8	2	20	18640.60	0.01	0.03	N	O	1997-05-11	1997-02-26	1997-05-12	TAKE BACK RETURN         	SHIP      	dolites about the final platelets w
5153	35	1	1	42	39271.26	0.03	0.01	N	O	1995-10-03	1995-11-09	1995-10-11	COLLECT COD              	RAIL      	re thinly. ironic
5153	53	5	2	14	13342.70	0.05	0.05	N	O	1995-11-29	1995-10-21	1995-12-08	TAKE BACK RETURN         	TRUCK     	 slyly daring pinto beans lose blithely fi
5153	68	7	3	30	29041.80	0.09	0.01	N	O	1995-11-10	1995-11-14	1995-11-16	DELIVER IN PERSON        	AIR       	beans sleep bl
5153	173	2	4	32	34341.44	0.10	0.08	N	O	1995-12-05	1995-09-25	1996-01-03	DELIVER IN PERSON        	MAIL      	egular deposits. ironi
5153	112	2	5	36	36435.96	0.01	0.03	N	O	1995-12-15	1995-11-08	1995-12-30	COLLECT COD              	TRUCK     	 ironic instru
5153	136	2	6	42	43517.46	0.00	0.03	N	O	1995-10-19	1995-11-23	1995-11-06	TAKE BACK RETURN         	RAIL      	ickly even deposi
5157	55	7	1	35	33426.75	0.06	0.08	N	O	1997-07-28	1997-09-30	1997-08-15	TAKE BACK RETURN         	REG AIR   	to the furiously sil
5157	138	9	2	18	18686.34	0.10	0.04	N	O	1997-09-06	1997-10-03	1997-09-19	COLLECT COD              	MAIL      	y bold deposits nag blithely. final reque
5157	167	8	3	15	16007.40	0.09	0.00	N	O	1997-07-27	1997-08-30	1997-08-08	DELIVER IN PERSON        	RAIL      	cajole. spec
5157	59	7	4	25	23976.25	0.00	0.03	N	O	1997-08-24	1997-09-23	1997-08-28	COLLECT COD              	REG AIR   	 packages detect. even requests against th
5157	149	8	5	40	41965.60	0.09	0.06	N	O	1997-08-11	1997-08-28	1997-09-01	TAKE BACK RETURN         	FOB       	ial packages according to 
5157	150	9	6	26	27303.90	0.10	0.01	N	O	1997-07-28	1997-08-22	1997-08-22	NONE                     	FOB       	nto beans cajole car
5157	49	8	7	12	11388.48	0.10	0.08	N	O	1997-10-19	1997-08-07	1997-10-26	NONE                     	FOB       	es. busily 
5158	45	4	1	43	40636.72	0.10	0.04	N	O	1997-04-10	1997-03-06	1997-04-15	DELIVER IN PERSON        	AIR       	nusual platelets. slyly even foxes cajole 
5158	85	6	2	18	17731.44	0.04	0.04	N	O	1997-04-30	1997-03-28	1997-05-12	COLLECT COD              	REG AIR   	hely regular pa
5158	142	9	3	41	42727.74	0.05	0.05	N	O	1997-02-25	1997-03-19	1997-03-03	COLLECT COD              	AIR       	deposits. quickly special 
5158	131	7	4	49	50525.37	0.05	0.01	N	O	1997-04-10	1997-03-21	1997-04-30	NONE                     	REG AIR   	r requests sleep q
5158	119	9	5	20	20382.20	0.01	0.04	N	O	1997-02-03	1997-02-20	1997-02-08	TAKE BACK RETURN         	AIR       	latelets use accordin
5158	88	9	6	39	38535.12	0.08	0.04	N	O	1997-05-15	1997-04-04	1997-06-02	DELIVER IN PERSON        	FOB       	lithely fina
5158	91	5	7	38	37661.42	0.10	0.05	N	O	1997-05-09	1997-03-03	1997-06-04	NONE                     	SHIP      	uffily regular ac
5159	124	7	1	39	39940.68	0.06	0.07	N	O	1996-12-17	1996-12-08	1997-01-10	COLLECT COD              	MAIL      	re furiously after the pending dolphin
5159	17	1	2	46	42182.46	0.01	0.01	N	O	1996-12-15	1996-12-07	1996-12-30	DELIVER IN PERSON        	SHIP      	s kindle slyly carefully regular
5159	152	4	3	22	23147.30	0.01	0.02	N	O	1996-11-06	1996-11-04	1996-11-15	TAKE BACK RETURN         	SHIP      	he furiously sile
5159	52	3	4	5	4760.25	0.10	0.00	N	O	1996-11-25	1996-12-19	1996-12-25	TAKE BACK RETURN         	FOB       	nal deposits. pending, ironic ideas grow
5159	198	10	5	36	39534.84	0.06	0.01	N	O	1997-01-24	1996-11-07	1997-02-08	NONE                     	REG AIR   	packages wake.
5184	153	8	1	33	34753.95	0.07	0.04	N	O	1998-08-17	1998-10-16	1998-08-24	TAKE BACK RETURN         	AIR       	posits. carefully express asympto
5184	16	6	2	47	43052.47	0.05	0.01	N	O	1998-11-02	1998-08-19	1998-11-07	COLLECT COD              	TRUCK     	se. carefully express pinto beans x
5184	88	9	3	39	38535.12	0.03	0.06	N	O	1998-10-27	1998-10-17	1998-11-19	DELIVER IN PERSON        	FOB       	es above the care
5184	176	7	4	26	27980.42	0.05	0.08	N	O	1998-11-11	1998-08-26	1998-12-01	TAKE BACK RETURN         	TRUCK     	 packages are
5184	124	9	5	19	19458.28	0.06	0.03	N	O	1998-11-15	1998-10-12	1998-11-21	COLLECT COD              	REG AIR   	refully express platelets sleep carefull
5184	80	9	6	49	48023.92	0.02	0.00	N	O	1998-09-18	1998-08-28	1998-10-14	COLLECT COD              	FOB       	thlessly closely even reque
5185	197	1	1	37	40596.03	0.00	0.04	N	O	1997-08-08	1997-09-08	1997-08-14	COLLECT COD              	SHIP      	gainst the courts dazzle care
5185	25	8	2	32	29600.64	0.06	0.00	N	O	1997-08-17	1997-09-30	1997-08-24	TAKE BACK RETURN         	REG AIR   	ackages. slyly even requests
5185	196	9	3	41	44943.79	0.00	0.05	N	O	1997-10-15	1997-10-11	1997-11-02	COLLECT COD              	REG AIR   	ly blithe deposits. furi
5185	96	7	4	30	29882.70	0.09	0.04	N	O	1997-10-17	1997-09-16	1997-10-23	TAKE BACK RETURN         	SHIP      	ress packages are furiously
5185	128	9	5	8	8224.96	0.04	0.00	N	O	1997-08-30	1997-09-02	1997-09-22	COLLECT COD              	REG AIR   	sts around the slyly perma
5185	146	9	6	50	52307.00	0.03	0.04	N	O	1997-10-15	1997-10-19	1997-11-06	TAKE BACK RETURN         	FOB       	final platelets. ideas sleep careful
5186	55	10	1	38	36291.90	0.06	0.02	N	O	1996-11-23	1996-09-21	1996-12-11	DELIVER IN PERSON        	MAIL      	y ruthless foxes. fluffily 
5186	91	2	2	31	30723.79	0.09	0.03	N	O	1996-10-19	1996-09-26	1996-10-25	TAKE BACK RETURN         	REG AIR   	 accounts use furiously slyly spe
5186	89	10	3	26	25716.08	0.03	0.02	N	O	1996-08-08	1996-10-05	1996-08-21	DELIVER IN PERSON        	FOB       	capades. accounts sublate. pinto
5186	90	1	4	8	7920.72	0.10	0.05	N	O	1996-09-23	1996-09-29	1996-09-30	COLLECT COD              	RAIL      	y regular notornis k
5186	18	2	5	28	25704.28	0.09	0.03	N	O	1996-10-05	1996-10-27	1996-10-19	TAKE BACK RETURN         	RAIL      	al decoys. blit
5186	82	3	6	35	34372.80	0.00	0.05	N	O	1996-10-20	1996-10-12	1996-11-12	TAKE BACK RETURN         	RAIL      	sly silent pack
5186	198	10	7	44	48320.36	0.00	0.08	N	O	1996-09-23	1996-10-14	1996-10-01	NONE                     	TRUCK     	old, final accounts cajole sl
5187	11	1	1	49	44639.49	0.04	0.06	N	O	1997-10-20	1997-10-12	1997-10-26	DELIVER IN PERSON        	AIR       	l, regular platelets instead of the foxes w
5187	83	4	2	1	983.08	0.10	0.08	N	O	1997-08-08	1997-08-24	1997-08-22	DELIVER IN PERSON        	REG AIR   	aggle never bold 
5188	118	2	1	18	18325.98	0.04	0.03	N	O	1995-06-19	1995-05-19	1995-06-25	DELIVER IN PERSON        	AIR       	p according to the sometimes regu
5188	194	8	2	36	39390.84	0.04	0.02	A	F	1995-03-09	1995-05-16	1995-03-19	NONE                     	TRUCK     	packages? blithely s
5188	148	1	3	9	9433.26	0.06	0.08	A	F	1995-05-09	1995-05-22	1995-05-19	TAKE BACK RETURN         	REG AIR   	r attainments are across the 
5189	138	9	1	44	45677.72	0.02	0.06	A	F	1994-01-13	1994-02-07	1994-01-21	DELIVER IN PERSON        	MAIL      	y finally pendin
5189	16	3	2	38	34808.38	0.06	0.00	A	F	1994-03-26	1994-01-28	1994-04-20	DELIVER IN PERSON        	REG AIR   	ideas. idle, final deposits de
5189	110	5	3	4	4040.44	0.09	0.02	A	F	1993-12-21	1994-02-23	1994-01-09	DELIVER IN PERSON        	REG AIR   	. blithely exp
5189	94	7	4	49	48710.41	0.05	0.01	R	F	1994-01-22	1994-01-19	1994-02-04	TAKE BACK RETURN         	SHIP      	 requests 
5189	123	2	5	14	14323.68	0.02	0.03	A	F	1994-01-23	1994-01-05	1994-02-12	DELIVER IN PERSON        	REG AIR   	unusual packag
5189	17	8	6	41	37597.41	0.02	0.06	R	F	1993-12-12	1994-02-05	1994-01-09	DELIVER IN PERSON        	RAIL      	ial theodolites cajole slyly. slyly unus
5216	69	10	1	17	16474.02	0.04	0.06	N	O	1997-08-20	1997-11-07	1997-09-14	COLLECT COD              	FOB       	s according to the accounts bo
5220	83	4	1	27	26543.16	0.10	0.04	R	F	1992-09-21	1992-08-29	1992-10-16	DELIVER IN PERSON        	RAIL      	s cajole blithely furiously iron
5221	104	9	1	24	24098.40	0.07	0.03	N	O	1995-10-04	1995-08-11	1995-10-30	COLLECT COD              	REG AIR   	s pinto beans sleep. sly
5221	9	10	2	34	30906.00	0.01	0.05	N	O	1995-09-11	1995-07-17	1995-10-10	COLLECT COD              	SHIP      	eans. furio
5221	180	10	3	16	17282.88	0.04	0.01	N	O	1995-08-29	1995-09-06	1995-09-12	TAKE BACK RETURN         	TRUCK     	ending request
5222	151	3	1	1	1051.15	0.00	0.00	A	F	1994-08-19	1994-07-16	1994-09-08	TAKE BACK RETURN         	FOB       	idle requests. carefully pending pinto bean
5249	50	9	1	31	29451.55	0.07	0.03	A	F	1994-11-21	1994-11-19	1994-12-08	NONE                     	REG AIR   	f the excuses. furiously fin
5249	31	7	2	44	40965.32	0.05	0.00	A	F	1994-12-28	1994-11-29	1994-12-29	TAKE BACK RETURN         	MAIL      	ole furiousl
5249	32	8	3	13	12116.39	0.09	0.00	R	F	1994-09-27	1994-10-20	1994-10-05	DELIVER IN PERSON        	SHIP      	ites. finally exp
5249	146	3	4	29	30338.06	0.00	0.05	A	F	1994-09-16	1994-11-03	1994-10-06	NONE                     	TRUCK     	 players. f
5249	158	6	5	12	12697.80	0.01	0.08	R	F	1994-12-28	1994-11-07	1995-01-15	COLLECT COD              	MAIL      	press depths could have to sleep carefu
5252	141	10	1	13	13534.82	0.02	0.01	N	O	1996-03-02	1996-05-10	1996-03-11	NONE                     	FOB       	boost fluffily across 
5252	139	5	2	39	40526.07	0.06	0.05	N	O	1996-05-17	1996-04-23	1996-05-23	COLLECT COD              	AIR       	gular requests.
5252	195	9	3	9	9856.71	0.09	0.03	N	O	1996-05-30	1996-05-03	1996-06-26	TAKE BACK RETURN         	RAIL      	x. slyly special depos
5252	87	8	4	48	47379.84	0.01	0.06	N	O	1996-04-17	1996-03-19	1996-05-03	COLLECT COD              	AIR       	bold requests. furious
5252	68	5	5	24	23233.44	0.04	0.05	N	O	1996-05-11	1996-04-17	1996-05-12	COLLECT COD              	REG AIR   	posits after the fluffi
5252	3	10	6	41	37023.00	0.02	0.03	N	O	1996-03-16	1996-04-18	1996-03-17	NONE                     	TRUCK     	ording to the blithely express somas sho
5253	31	2	1	35	32586.05	0.02	0.00	N	O	1995-07-23	1995-06-12	1995-08-03	DELIVER IN PERSON        	AIR       	ven deposits. careful
5253	150	7	2	38	39905.70	0.02	0.06	N	O	1995-08-03	1995-06-14	1995-08-27	DELIVER IN PERSON        	REG AIR   	onic dependencies are furiou
5253	14	5	3	9	8226.09	0.03	0.08	N	F	1995-06-08	1995-05-12	1995-06-23	DELIVER IN PERSON        	REG AIR   	lyly express deposits use furiou
5253	166	1	4	25	26654.00	0.04	0.03	A	F	1995-05-21	1995-06-13	1995-06-09	COLLECT COD              	TRUCK     	urts. even theodoli
5255	131	7	1	2	2062.26	0.04	0.08	N	O	1996-09-27	1996-10-04	1996-10-04	DELIVER IN PERSON        	RAIL      	ajole blithely fluf
5255	172	10	2	30	32165.10	0.04	0.08	N	O	1996-09-20	1996-08-18	1996-10-09	TAKE BACK RETURN         	AIR       	 to the silent requests cajole b
5255	130	3	3	41	42235.33	0.09	0.03	N	O	1996-08-21	1996-09-24	1996-09-05	COLLECT COD              	FOB       	tect blithely against t
5282	118	2	1	36	36651.96	0.05	0.02	N	O	1998-05-20	1998-04-10	1998-06-14	DELIVER IN PERSON        	TRUCK     	re slyly accor
5282	52	10	2	32	30465.60	0.02	0.05	N	O	1998-03-01	1998-03-31	1998-03-03	NONE                     	FOB       	onic deposits; furiou
5282	58	10	3	28	26825.40	0.03	0.06	N	O	1998-05-06	1998-04-24	1998-05-30	COLLECT COD              	SHIP      	fily final instruc
5283	5	2	1	20	18100.00	0.05	0.02	A	F	1994-09-16	1994-08-03	1994-10-15	TAKE BACK RETURN         	TRUCK     	al deposits? blithely even pinto beans
5283	186	7	2	1	1086.18	0.10	0.08	R	F	1994-06-20	1994-08-03	1994-07-01	COLLECT COD              	FOB       	deposits within the furio
5287	39	10	1	32	30048.96	0.01	0.01	A	F	1994-01-29	1994-01-27	1994-02-08	NONE                     	RAIL      	heodolites haggle caref
5313	17	1	1	34	31178.34	0.10	0.02	N	O	1997-08-07	1997-08-12	1997-08-24	DELIVER IN PERSON        	FOB       	ccording to the blithely final account
5313	13	10	2	17	15521.17	0.00	0.02	N	O	1997-09-02	1997-08-20	1997-09-07	NONE                     	SHIP      	uests wake
5313	112	9	3	47	47569.17	0.06	0.08	N	O	1997-08-12	1997-08-18	1997-08-13	TAKE BACK RETURN         	RAIL      	pinto beans across the 
5313	197	1	4	16	17555.04	0.08	0.00	N	O	1997-10-04	1997-08-02	1997-10-25	COLLECT COD              	REG AIR   	ckages wake carefully aga
5313	72	1	5	30	29162.10	0.06	0.08	N	O	1997-06-27	1997-07-18	1997-06-30	NONE                     	SHIP      	nding packages use
5313	120	7	6	21	21422.52	0.05	0.05	N	O	1997-09-26	1997-09-02	1997-10-18	COLLECT COD              	FOB       	he blithely regular packages. quickly
5315	35	1	1	12	11220.36	0.08	0.06	R	F	1992-12-18	1993-01-16	1993-01-10	NONE                     	AIR       	ccounts. furiously ironi
5315	179	10	2	39	42087.63	0.00	0.06	R	F	1992-11-09	1992-12-29	1992-12-07	NONE                     	SHIP      	ly alongside of the ca
5317	82	3	1	29	28480.32	0.02	0.06	A	F	1994-11-28	1994-11-27	1994-12-16	COLLECT COD              	FOB       	oss the carefull
5317	171	2	2	18	19281.06	0.06	0.06	A	F	1995-01-02	1994-10-29	1995-01-16	NONE                     	RAIL      	g to the blithely p
5317	120	4	3	37	37744.44	0.09	0.00	R	F	1994-09-15	1994-10-24	1994-09-23	NONE                     	TRUCK     	totes nag theodolites. pend
5317	67	6	4	50	48353.00	0.09	0.01	A	F	1994-10-17	1994-10-25	1994-11-03	NONE                     	REG AIR   	cajole furiously. accounts use quick
5317	95	8	5	19	18906.71	0.07	0.07	R	F	1994-12-15	1994-10-18	1994-12-27	NONE                     	MAIL      	onic requests boost bli
5317	115	9	6	48	48725.28	0.01	0.03	A	F	1994-09-19	1994-11-25	1994-10-03	COLLECT COD              	MAIL      	ts about the packages cajole furio
5317	169	4	7	30	32074.80	0.07	0.07	A	F	1994-10-13	1994-10-31	1994-10-28	NONE                     	AIR       	cross the attainments. slyly 
5318	61	6	1	13	12493.78	0.10	0.04	R	F	1993-07-15	1993-06-25	1993-08-13	COLLECT COD              	REG AIR   	ly silent ideas. ideas haggle among the 
5318	180	1	2	26	28084.68	0.00	0.04	R	F	1993-07-07	1993-05-23	1993-07-28	COLLECT COD              	SHIP      	al, express foxes. bold requests sleep alwa
5318	7	10	3	37	33559.00	0.07	0.05	A	F	1993-07-09	1993-06-22	1993-07-21	COLLECT COD              	SHIP      	ickly final deposi
5318	142	5	4	31	32306.34	0.01	0.04	R	F	1993-07-28	1993-05-06	1993-08-06	DELIVER IN PERSON        	REG AIR   	requests must sleep slyly quickly
5319	150	9	1	31	32554.65	0.04	0.07	N	O	1996-03-26	1996-03-07	1996-04-24	COLLECT COD              	TRUCK     	d carefully about the courts. fluffily spe
5319	44	3	2	39	36817.56	0.09	0.05	N	O	1996-05-17	1996-03-14	1996-06-11	NONE                     	TRUCK     	unts. furiously silent
5346	149	8	1	21	22031.94	0.07	0.08	R	F	1994-03-11	1994-03-07	1994-04-04	DELIVER IN PERSON        	RAIL      	integrate blithely a
5346	192	5	2	13	14198.47	0.04	0.04	A	F	1994-02-03	1994-02-05	1994-02-09	COLLECT COD              	TRUCK     	y. fluffily bold accounts grow. furio
5346	109	2	3	7	7063.70	0.08	0.05	A	F	1994-01-30	1994-03-26	1994-01-31	DELIVER IN PERSON        	SHIP      	equests use carefully care
5346	162	3	4	35	37175.60	0.06	0.02	A	F	1994-02-09	1994-03-01	1994-02-14	TAKE BACK RETURN         	FOB       	nic excuses cajole entic
5346	121	2	5	25	25528.00	0.05	0.06	R	F	1993-12-28	1994-03-19	1994-01-09	TAKE BACK RETURN         	REG AIR   	he ironic ideas are boldly slyly ironi
5346	33	9	6	6	5598.18	0.08	0.04	R	F	1994-03-01	1994-02-04	1994-03-09	NONE                     	REG AIR   	escapades sleep furiously beside the 
5346	80	9	7	41	40183.28	0.05	0.04	R	F	1994-01-10	1994-02-15	1994-01-26	TAKE BACK RETURN         	REG AIR   	fully close instructi
5347	83	4	1	48	47187.84	0.04	0.08	A	F	1995-02-25	1995-04-26	1995-03-26	NONE                     	SHIP      	equests are slyly. blithely regu
5347	124	3	2	47	48133.64	0.02	0.01	N	F	1995-06-05	1995-03-29	1995-06-28	COLLECT COD              	AIR       	across the slyly bol
5347	23	2	3	34	31382.68	0.06	0.00	A	F	1995-05-18	1995-04-04	1995-06-02	DELIVER IN PERSON        	SHIP      	 pending deposits. fluffily regular senti
5347	40	1	4	4	3760.16	0.06	0.03	A	F	1995-03-24	1995-04-03	1995-04-01	NONE                     	SHIP      	ldly pending asymptotes ki
5347	131	2	5	21	21653.73	0.08	0.04	R	F	1995-04-01	1995-04-16	1995-04-23	NONE                     	SHIP      	sly slyly final requests. careful
5347	56	1	6	6	5736.30	0.06	0.02	A	F	1995-04-11	1995-04-14	1995-05-02	NONE                     	TRUCK     	lly unusual ideas. sl
5347	50	7	7	18	17100.90	0.01	0.01	N	F	1995-05-24	1995-05-07	1995-06-19	NONE                     	FOB       	he ideas among the requests 
5349	156	7	1	19	20066.85	0.06	0.01	N	O	1996-09-11	1996-11-18	1996-09-22	TAKE BACK RETURN         	FOB       	endencies use whithout the special 
5349	168	3	2	14	14954.24	0.06	0.00	N	O	1996-11-07	1996-11-17	1996-11-20	TAKE BACK RETURN         	TRUCK     	fully regular 
5349	4	5	3	6	5424.00	0.10	0.01	N	O	1996-12-30	1996-10-08	1997-01-01	DELIVER IN PERSON        	MAIL      	inal deposits affix carefully
5350	122	3	1	19	19420.28	0.02	0.06	R	F	1993-10-20	1993-11-15	1993-11-17	DELIVER IN PERSON        	RAIL      	romise slyly alongsi
5350	191	4	2	44	48012.36	0.04	0.06	R	F	1993-10-30	1993-11-23	1993-11-25	DELIVER IN PERSON        	AIR       	p above the ironic, pending dep
5350	54	9	3	12	11448.60	0.10	0.04	A	F	1994-01-30	1993-11-21	1994-02-15	COLLECT COD              	REG AIR   	 cajole. even instructions haggle. blithe
5350	155	10	4	7	7386.05	0.08	0.00	R	F	1993-10-19	1993-12-28	1993-11-04	NONE                     	SHIP      	alongside of th
5350	129	10	5	27	27786.24	0.07	0.04	A	F	1993-11-25	1993-12-27	1993-12-08	COLLECT COD              	TRUCK     	es. blithe theodolites haggl
5351	7	2	1	36	32652.00	0.06	0.05	N	O	1998-07-27	1998-07-06	1998-08-25	NONE                     	MAIL      	ss the ironic, regular asymptotes cajole 
5351	33	9	2	47	43852.41	0.04	0.01	N	O	1998-05-30	1998-08-08	1998-06-23	DELIVER IN PERSON        	REG AIR   	s. grouches cajole. sile
5351	106	3	3	2	2012.20	0.00	0.02	N	O	1998-05-12	1998-07-15	1998-05-24	NONE                     	TRUCK     	g accounts wake furiously slyly even dolph
5377	79	8	1	40	39162.80	0.00	0.04	N	O	1997-05-21	1997-06-15	1997-05-26	DELIVER IN PERSON        	AIR       	lithely ironic theodolites are care
5377	30	3	2	17	15810.51	0.09	0.00	N	O	1997-07-05	1997-05-25	1997-07-22	COLLECT COD              	RAIL      	dencies. carefully regular re
5377	103	8	3	23	23071.30	0.07	0.08	N	O	1997-06-26	1997-07-13	1997-07-08	COLLECT COD              	RAIL      	 silent wa
5377	104	7	4	12	12049.20	0.05	0.07	N	O	1997-05-08	1997-06-15	1997-05-15	DELIVER IN PERSON        	MAIL      	 ironic, final
5377	173	3	5	27	28975.59	0.08	0.02	N	O	1997-07-11	1997-06-12	1997-08-08	TAKE BACK RETURN         	MAIL      	press theodolites. e
5380	182	3	1	14	15150.52	0.10	0.01	N	O	1997-12-18	1997-12-03	1998-01-06	NONE                     	RAIL      	final platelets.
5380	147	6	2	10	10471.40	0.09	0.05	N	O	1997-11-24	1998-01-10	1997-12-21	COLLECT COD              	AIR       	refully pending deposits. special, even t
5380	184	5	3	40	43367.20	0.02	0.08	N	O	1997-12-30	1997-11-27	1998-01-09	DELIVER IN PERSON        	SHIP      	ar asymptotes. blithely r
5380	66	3	4	6	5796.36	0.09	0.05	N	O	1997-11-15	1998-01-08	1997-12-11	COLLECT COD              	MAIL      	es. fluffily brave accounts across t
5380	107	8	5	48	48340.80	0.04	0.03	N	O	1997-12-01	1997-12-28	1997-12-05	DELIVER IN PERSON        	FOB       	encies haggle car
5408	102	7	1	2	2004.20	0.07	0.04	R	F	1992-08-21	1992-10-03	1992-08-28	DELIVER IN PERSON        	MAIL      	cross the dolphins h
5408	118	2	2	35	35633.85	0.04	0.05	R	F	1992-10-02	1992-10-17	1992-10-13	TAKE BACK RETURN         	AIR       	thely ironic requests alongside of the sl
5408	76	6	3	34	33186.38	0.10	0.02	A	F	1992-10-22	1992-08-25	1992-11-16	DELIVER IN PERSON        	TRUCK     	requests detect blithely a
5408	54	2	4	48	45794.40	0.04	0.05	R	F	1992-09-30	1992-08-27	1992-10-27	NONE                     	TRUCK     	. furiously regular 
5408	183	4	5	8	8665.44	0.03	0.07	A	F	1992-10-24	1992-09-06	1992-11-03	NONE                     	AIR       	thely regular hocke
5409	194	8	1	27	29543.13	0.01	0.02	A	F	1992-02-14	1992-03-18	1992-02-23	DELIVER IN PERSON        	AIR       	eodolites 
5409	104	5	2	38	38155.80	0.01	0.02	A	F	1992-03-17	1992-03-29	1992-04-13	NONE                     	REG AIR   	onic, regular accounts! blithely even
5409	141	10	3	17	17699.38	0.07	0.00	A	F	1992-01-13	1992-04-05	1992-01-20	DELIVER IN PERSON        	AIR       	cross the sil
5409	1	8	4	9	8109.00	0.07	0.03	A	F	1992-02-15	1992-04-02	1992-02-28	DELIVER IN PERSON        	AIR       	 unusual, unusual reques
5409	159	10	5	37	39188.55	0.06	0.04	R	F	1992-05-07	1992-02-10	1992-05-20	DELIVER IN PERSON        	FOB       	ously regular packages. packages
5409	64	3	6	14	13496.84	0.03	0.08	R	F	1992-02-14	1992-03-26	1992-02-29	DELIVER IN PERSON        	AIR       	osits cajole furiously
5410	117	8	1	48	48821.28	0.04	0.08	N	O	1998-09-27	1998-09-11	1998-10-01	TAKE BACK RETURN         	AIR       	 about the slyly even courts. quickly regul
5410	105	8	2	41	41209.10	0.01	0.07	N	O	1998-08-25	1998-10-20	1998-09-01	DELIVER IN PERSON        	REG AIR   	sly. slyly ironic theodolites
5410	29	4	3	40	37160.80	0.07	0.08	N	O	1998-11-17	1998-10-02	1998-11-27	COLLECT COD              	TRUCK     	iously special accounts are along th
5410	50	7	4	8	7600.40	0.05	0.04	N	O	1998-09-12	1998-10-22	1998-09-22	DELIVER IN PERSON        	TRUCK     	ly. fluffily ironic platelets alon
5412	54	9	1	2	1908.10	0.03	0.07	N	O	1998-04-14	1998-04-02	1998-04-19	TAKE BACK RETURN         	REG AIR   	 sleep above the furiou
5412	66	1	2	48	46370.88	0.01	0.08	N	O	1998-02-22	1998-03-28	1998-03-18	TAKE BACK RETURN         	TRUCK     	s. slyly final packages cajole blithe
5412	74	2	3	31	30196.17	0.05	0.08	N	O	1998-03-23	1998-04-17	1998-04-10	NONE                     	SHIP      	t the accounts detect slyly about the c
5412	97	10	4	26	25924.34	0.02	0.08	N	O	1998-01-22	1998-04-19	1998-02-17	NONE                     	AIR       	 the blithel
5413	126	7	1	48	49253.76	0.02	0.08	N	O	1998-01-25	1997-11-20	1998-02-22	COLLECT COD              	SHIP      	 theodolites. furiously ironic instr
5413	142	9	2	37	38559.18	0.02	0.07	N	O	1997-12-08	1998-01-01	1997-12-13	COLLECT COD              	TRUCK     	usly bold instructions affix idly unusual, 
5413	111	8	3	36	36399.96	0.02	0.07	N	O	1997-12-12	1997-11-28	1997-12-25	NONE                     	TRUCK     	ular, regular ideas mold! final requests
5413	110	3	4	22	22222.42	0.02	0.08	N	O	1997-11-10	1997-11-24	1997-11-22	DELIVER IN PERSON        	FOB       	posits. quick
5413	190	1	6	32	34886.08	0.02	0.03	N	O	1997-10-28	1998-01-03	1997-11-10	NONE                     	TRUCK     	refully special package
5413	31	7	7	32	29792.96	0.06	0.07	N	O	1997-10-23	1997-12-09	1997-11-17	NONE                     	TRUCK     	he quickly ironic ideas. slyly ironic ide
5415	102	5	1	44	44092.40	0.00	0.06	A	F	1992-08-19	1992-10-26	1992-09-17	TAKE BACK RETURN         	TRUCK     	 requests. unusual theodolites sleep agains
5415	31	7	2	16	14896.48	0.08	0.00	A	F	1992-09-29	1992-09-12	1992-10-10	NONE                     	AIR       	pinto beans haggle furiously
5415	102	7	3	6	6012.60	0.10	0.03	A	F	1992-10-28	1992-09-09	1992-11-20	COLLECT COD              	RAIL      	ges around the fur
5415	16	7	4	43	39388.43	0.01	0.02	R	F	1992-11-17	1992-09-14	1992-12-14	DELIVER IN PERSON        	SHIP      	yly blithely stealthy deposits. carefu
5415	161	6	5	11	11672.76	0.00	0.01	R	F	1992-11-22	1992-10-19	1992-12-10	DELIVER IN PERSON        	SHIP      	gle among t
5415	144	1	6	46	48030.44	0.03	0.03	R	F	1992-08-25	1992-09-10	1992-09-22	DELIVER IN PERSON        	REG AIR   	ve the fluffily 
5415	153	4	7	11	11584.65	0.08	0.06	A	F	1992-08-21	1992-09-04	1992-08-23	NONE                     	TRUCK     	unts maintain carefully unusual
5440	115	2	1	3	3045.33	0.02	0.08	N	O	1997-02-18	1997-02-28	1997-03-15	NONE                     	SHIP      	y. accounts haggle along the blit
5441	164	1	1	3	3192.48	0.00	0.02	R	F	1994-08-12	1994-10-14	1994-09-01	TAKE BACK RETURN         	REG AIR   	are. unusual, 
5441	131	2	2	49	50525.37	0.02	0.03	A	F	1994-09-23	1994-09-22	1994-10-22	NONE                     	FOB       	ording to the furio
5441	144	3	3	33	34456.62	0.09	0.02	R	F	1994-10-09	1994-10-06	1994-10-30	DELIVER IN PERSON        	TRUCK     	ges. final instruction
5441	67	4	4	47	45451.82	0.07	0.08	R	F	1994-11-19	1994-10-16	1994-12-16	TAKE BACK RETURN         	FOB       	ounts wake slyly about the express instr
5443	178	9	1	14	15094.38	0.02	0.00	N	O	1996-10-27	1996-11-11	1996-11-21	DELIVER IN PERSON        	RAIL      	s after the regular, regular deposits hag
5443	72	3	2	39	37910.73	0.03	0.07	N	O	1996-11-01	1996-11-30	1996-11-19	NONE                     	RAIL      	gage carefully across the furiously
5443	160	5	3	25	26504.00	0.05	0.00	N	O	1996-12-07	1997-01-08	1997-01-05	NONE                     	FOB       	use carefully above the pinto bea
5443	191	4	4	6	6547.14	0.05	0.02	N	O	1996-11-17	1996-12-03	1996-11-30	TAKE BACK RETURN         	AIR       	p fluffily foxe
5443	83	4	5	40	39323.20	0.03	0.03	N	O	1997-01-28	1996-12-10	1997-02-13	NONE                     	FOB       	n courts. special re
5444	186	7	1	21	22809.78	0.01	0.07	A	F	1995-04-11	1995-04-25	1995-04-21	DELIVER IN PERSON        	RAIL      	ar packages haggle above th
5444	43	6	2	40	37721.60	0.05	0.08	N	O	1995-07-09	1995-04-25	1995-07-19	COLLECT COD              	TRUCK     	ously bold ideas. instructions wake slyl
5444	150	9	3	40	42006.00	0.08	0.01	A	F	1995-04-06	1995-05-08	1995-05-06	DELIVER IN PERSON        	AIR       	 even packages.
5444	59	4	4	33	31648.65	0.05	0.04	N	O	1995-06-24	1995-04-24	1995-07-13	DELIVER IN PERSON        	SHIP      	ut the courts cajole blithely excuses
5444	171	9	5	21	22494.57	0.04	0.00	R	F	1995-05-05	1995-05-25	1995-05-29	TAKE BACK RETURN         	REG AIR   	aves serve sly
5444	20	7	6	21	19320.42	0.07	0.01	A	F	1995-03-30	1995-05-01	1995-03-31	COLLECT COD              	AIR       	furiously even theodolites.
5445	90	1	1	33	32672.97	0.08	0.07	A	F	1993-10-21	1993-10-14	1993-10-28	DELIVER IN PERSON        	REG AIR   	ests. final instructions
5445	131	2	2	12	12373.56	0.09	0.08	R	F	1993-11-02	1993-09-05	1993-11-26	COLLECT COD              	FOB       	 slyly pending pinto beans was slyly al
5445	103	8	3	46	46142.60	0.04	0.07	A	F	1993-10-06	1993-09-15	1993-10-28	DELIVER IN PERSON        	RAIL      	old depend
5445	149	10	4	10	10491.40	0.08	0.06	A	F	1993-09-16	1993-10-05	1993-10-01	NONE                     	TRUCK     	ncies abou
5445	13	10	5	14	12782.14	0.00	0.02	R	F	1993-11-19	1993-10-18	1993-12-07	NONE                     	RAIL      	 requests. bravely i
5447	99	1	1	31	30971.79	0.09	0.03	N	O	1996-07-14	1996-05-07	1996-07-17	COLLECT COD              	SHIP      	 foxes sleep. blithely unusual accounts det
5474	184	5	1	38	41198.84	0.01	0.08	A	F	1992-07-15	1992-07-16	1992-07-20	NONE                     	REG AIR   	 slyly beneath 
5474	94	8	2	10	9940.90	0.06	0.00	R	F	1992-08-08	1992-08-10	1992-08-24	TAKE BACK RETURN         	TRUCK     	pinto bean
5474	48	1	3	31	29389.24	0.00	0.08	R	F	1992-08-02	1992-07-12	1992-08-04	NONE                     	TRUCK     	the furiously express ideas. speci
5474	90	1	4	46	45544.14	0.03	0.04	A	F	1992-06-07	1992-07-11	1992-06-22	NONE                     	TRUCK     	nstructions. furio
5475	183	4	1	10	10831.80	0.09	0.08	N	O	1996-07-19	1996-08-22	1996-07-23	COLLECT COD              	AIR       	ding to the deposits wake fina
5476	48	1	1	13	12324.52	0.01	0.04	N	O	1997-12-27	1997-12-08	1997-12-29	COLLECT COD              	TRUCK     	iously special ac
5476	20	4	2	17	15640.34	0.10	0.01	N	O	1998-02-02	1998-01-28	1998-02-14	COLLECT COD              	FOB       	ng dependencies until the f
5506	140	1	1	2	2080.28	0.00	0.03	R	F	1994-02-04	1994-01-13	1994-02-17	COLLECT COD              	MAIL      	onic theodolites are fluffil
5506	160	1	2	6	6360.96	0.07	0.06	R	F	1994-02-21	1994-01-30	1994-02-27	DELIVER IN PERSON        	MAIL      	hely according to the furiously unusua
5508	117	7	1	4	4068.44	0.10	0.04	N	O	1996-09-01	1996-08-02	1996-09-17	COLLECT COD              	AIR       	fluffily about the even 
5509	197	10	1	3	3291.57	0.03	0.02	A	F	1994-06-14	1994-05-11	1994-06-17	NONE                     	SHIP      	 quickly fin
5509	99	3	2	17	16984.53	0.03	0.07	R	F	1994-07-01	1994-06-30	1994-07-31	COLLECT COD              	AIR       	ccounts wake ar
5509	93	7	3	30	29792.70	0.04	0.04	A	F	1994-07-23	1994-06-01	1994-08-08	NONE                     	AIR       	counts haggle pinto beans. furiously 
5509	100	3	4	45	45004.50	0.00	0.07	A	F	1994-07-24	1994-05-28	1994-08-20	COLLECT COD              	AIR       	counts sleep. f
5509	156	8	5	35	36965.25	0.04	0.03	A	F	1994-04-17	1994-06-29	1994-04-24	COLLECT COD              	RAIL      	c accounts. ca
5510	16	6	1	8	7328.08	0.01	0.01	A	F	1993-03-16	1993-03-29	1993-03-24	DELIVER IN PERSON        	FOB       	n packages boost sly
5510	20	10	2	46	42320.92	0.02	0.07	A	F	1993-03-12	1993-02-09	1993-03-19	NONE                     	TRUCK     	silent packages cajole doggedly regular 
5510	162	3	3	47	49921.52	0.03	0.01	A	F	1993-01-20	1993-03-25	1993-02-15	DELIVER IN PERSON        	SHIP      	riously even requests. slyly bold accou
5510	24	7	4	29	26796.58	0.09	0.08	A	F	1993-02-28	1993-03-28	1993-03-12	COLLECT COD              	AIR       	lithely fluffily ironic req
5538	154	9	1	42	44274.30	0.05	0.00	A	F	1994-04-08	1994-03-17	1994-05-05	DELIVER IN PERSON        	REG AIR   	vely ironic accounts. furiously unusual acc
5538	121	2	2	4	4084.48	0.02	0.03	R	F	1994-03-21	1994-02-17	1994-04-11	TAKE BACK RETURN         	REG AIR   	ithely along the c
5538	19	3	3	38	34922.38	0.03	0.06	R	F	1994-03-17	1994-02-11	1994-04-10	TAKE BACK RETURN         	FOB       	ular pinto beans. silent ideas above 
5538	78	6	4	9	8802.63	0.00	0.01	R	F	1993-12-26	1994-01-31	1994-01-03	TAKE BACK RETURN         	REG AIR   	encies across the blithely fina
5539	65	10	1	42	40532.52	0.10	0.08	A	F	1994-09-29	1994-09-17	1994-10-20	DELIVER IN PERSON        	RAIL      	ons across the carefully si
5540	181	2	1	42	45409.56	0.02	0.08	N	O	1996-11-12	1996-12-18	1996-12-05	TAKE BACK RETURN         	RAIL      	ss dolphins haggle 
5540	102	3	2	2	2004.20	0.06	0.02	N	O	1996-12-12	1997-01-09	1996-12-25	DELIVER IN PERSON        	MAIL      	nic asymptotes could hav
5540	64	3	3	19	18317.14	0.01	0.03	N	O	1997-02-06	1996-11-18	1997-02-20	DELIVER IN PERSON        	SHIP      	 slyly slyl
5540	72	10	4	24	23329.68	0.10	0.05	N	O	1997-01-09	1996-12-02	1997-01-23	COLLECT COD              	FOB       	deposits! ironic depths may engage-- b
5542	189	10	1	6	6535.08	0.03	0.01	N	O	1996-06-14	1996-05-28	1996-07-11	DELIVER IN PERSON        	TRUCK     	 foxes doubt. theodolites ca
5543	143	10	1	14	14603.96	0.02	0.03	R	F	1993-10-09	1993-12-09	1993-10-21	NONE                     	SHIP      	ecial reque
5543	162	7	2	22	23367.52	0.04	0.00	A	F	1993-11-06	1993-11-02	1993-12-02	DELIVER IN PERSON        	SHIP      	instructions. deposits use quickly. ir
5543	67	6	3	3	2901.18	0.08	0.05	R	F	1993-11-18	1993-11-05	1993-12-17	NONE                     	FOB       	ress, even 
5543	147	10	4	8	8377.12	0.05	0.01	R	F	1993-10-28	1993-11-18	1993-11-07	NONE                     	SHIP      	totes? iron
5543	80	1	5	32	31362.56	0.03	0.03	R	F	1993-10-04	1993-11-14	1993-11-03	DELIVER IN PERSON        	AIR       	ully around the 
5543	184	5	6	1	1084.18	0.03	0.07	A	F	1993-10-29	1993-11-11	1993-11-23	TAKE BACK RETURN         	FOB       	uriously. slyly
5543	129	8	7	39	40135.68	0.06	0.00	R	F	1993-10-07	1993-11-15	1993-10-28	TAKE BACK RETURN         	MAIL      	l excuses are furiously. slyly unusual requ
5571	154	2	1	32	33732.80	0.05	0.01	R	F	1992-12-25	1993-03-01	1993-01-23	NONE                     	FOB       	 the blithely even packages nag q
5571	94	8	2	31	30816.79	0.09	0.07	R	F	1993-01-05	1993-01-18	1993-02-04	DELIVER IN PERSON        	SHIP      	uffily even accounts. quickly re
5571	92	6	3	18	17857.62	0.10	0.05	R	F	1993-03-11	1993-02-28	1993-04-03	COLLECT COD              	REG AIR   	uests haggle furiously pending d
5573	21	6	1	32	29472.64	0.05	0.07	N	O	1996-09-30	1996-10-25	1996-10-15	DELIVER IN PERSON        	RAIL      	egular depths haggl
5573	50	3	2	2	1900.10	0.01	0.07	N	O	1996-08-26	1996-09-29	1996-09-04	COLLECT COD              	TRUCK     	 even foxes. specia
5573	11	8	3	46	41906.46	0.06	0.01	N	O	1996-11-04	1996-10-02	1996-11-15	DELIVER IN PERSON        	MAIL      	s haggle qu
5573	169	4	4	43	45973.88	0.10	0.03	N	O	1996-10-22	1996-11-03	1996-11-02	TAKE BACK RETURN         	FOB       	 furiously pending packages against 
5573	138	9	5	43	44639.59	0.05	0.04	N	O	1996-09-09	1996-09-24	1996-09-28	COLLECT COD              	AIR       	 bold package
5574	185	6	1	46	49918.28	0.02	0.07	A	F	1992-06-20	1992-04-19	1992-07-11	NONE                     	FOB       	arefully express requests wake furiousl
5574	33	4	2	21	19593.63	0.05	0.08	A	F	1992-03-22	1992-04-26	1992-04-16	TAKE BACK RETURN         	TRUCK     	fully final dugouts. express foxes nag 
5574	119	6	3	27	27515.97	0.10	0.06	R	F	1992-05-08	1992-05-19	1992-06-05	TAKE BACK RETURN         	REG AIR   	ecial realms. furiously entici
5574	94	6	4	14	13917.26	0.09	0.01	R	F	1992-05-20	1992-04-09	1992-05-23	COLLECT COD              	REG AIR   	 use slyly carefully special requests? slyl
5574	85	6	5	19	18716.52	0.05	0.03	A	F	1992-05-28	1992-04-24	1992-06-11	TAKE BACK RETURN         	REG AIR   	old deposits int
5575	58	10	1	7	6706.35	0.01	0.07	N	O	1995-10-01	1995-09-30	1995-10-06	NONE                     	FOB       	s. slyly pending theodolites prin
5575	31	7	2	23	21413.69	0.04	0.02	N	O	1995-10-26	1995-10-09	1995-11-13	TAKE BACK RETURN         	AIR       	enticingly final requests. ironically
5575	63	8	3	16	15408.96	0.00	0.07	N	O	1995-08-17	1995-10-14	1995-08-30	NONE                     	RAIL      	jole boldly beyond the final as
5575	110	1	4	7	7070.77	0.01	0.04	N	O	1995-10-15	1995-09-14	1995-10-18	DELIVER IN PERSON        	RAIL      	special requests. final, final 
5602	176	4	1	9	9685.53	0.08	0.03	N	O	1997-10-14	1997-09-14	1997-11-11	COLLECT COD              	FOB       	lar foxes; quickly ironic ac
5602	62	7	2	31	29823.86	0.04	0.08	N	O	1997-09-04	1997-10-24	1997-09-07	NONE                     	TRUCK     	rate fluffily regular platelets. blithel
5602	68	5	3	30	29041.80	0.04	0.00	N	O	1997-09-20	1997-10-25	1997-10-12	DELIVER IN PERSON        	FOB       	e slyly even packages. careful
5603	98	2	1	50	49904.50	0.03	0.02	A	F	1992-10-06	1992-08-20	1992-10-08	COLLECT COD              	SHIP      	final theodolites accor
5603	116	6	2	49	49789.39	0.06	0.05	A	F	1992-06-24	1992-07-28	1992-07-01	DELIVER IN PERSON        	FOB       	fully silent requests. carefully fin
5603	32	8	3	49	45669.47	0.00	0.02	R	F	1992-10-07	1992-07-21	1992-10-10	DELIVER IN PERSON        	TRUCK     	nic, pending dependencies print
5604	136	7	1	44	45589.72	0.05	0.01	N	O	1998-08-06	1998-07-08	1998-09-04	NONE                     	RAIL      	efully ironi
5604	136	2	2	49	50770.37	0.10	0.00	N	O	1998-05-02	1998-07-07	1998-05-20	NONE                     	FOB       	ove the regula
5604	78	8	3	10	9780.70	0.07	0.05	N	O	1998-08-03	1998-06-23	1998-08-04	COLLECT COD              	SHIP      	ly final realms wake blit
5607	132	8	1	23	23738.99	0.02	0.06	R	F	1992-04-17	1992-02-12	1992-04-30	DELIVER IN PERSON        	MAIL      	the special, final patterns 
5634	185	6	1	26	28214.68	0.10	0.08	N	O	1996-10-29	1996-09-15	1996-11-24	COLLECT COD              	REG AIR   	ptotes mold qu
5634	175	3	2	22	23653.74	0.02	0.05	N	O	1996-09-01	1996-08-31	1996-09-05	DELIVER IN PERSON        	MAIL      	silently unusual foxes above the blithely
5634	109	6	3	16	16145.60	0.08	0.02	N	O	1996-11-15	1996-09-14	1996-12-04	NONE                     	AIR       	ess ideas are carefully pending, even re
5634	182	3	4	29	31383.22	0.00	0.01	N	O	1996-08-10	1996-10-29	1996-08-11	TAKE BACK RETURN         	MAIL      	ely final ideas. deposits sleep. reg
5634	1	2	5	1	901.00	0.04	0.02	N	O	1996-10-02	1996-10-21	1996-10-27	COLLECT COD              	MAIL      	ctions haggle carefully. carefully clo
5636	70	9	1	18	17461.26	0.05	0.03	R	F	1995-05-14	1995-05-17	1995-06-12	DELIVER IN PERSON        	REG AIR   	slyly express requests. furiously pen
5636	70	5	2	26	25221.82	0.03	0.06	A	F	1995-03-05	1995-05-16	1995-03-23	TAKE BACK RETURN         	AIR       	 furiously final pinto beans o
5636	90	1	3	21	20791.89	0.03	0.03	A	F	1995-03-13	1995-05-11	1995-03-24	COLLECT COD              	AIR       	 are furiously unusual 
5636	109	6	4	15	15136.50	0.03	0.04	R	F	1995-04-21	1995-04-30	1995-05-05	DELIVER IN PERSON        	REG AIR   	efully special
5636	47	4	5	13	12311.52	0.10	0.03	A	F	1995-05-11	1995-04-27	1995-05-26	COLLECT COD              	AIR       	en, fluffy accounts amon
5636	12	3	6	33	30096.33	0.06	0.04	A	F	1995-03-09	1995-04-05	1995-03-23	DELIVER IN PERSON        	MAIL      	ding to the 
5636	134	10	7	24	24819.12	0.10	0.05	R	F	1995-04-12	1995-03-27	1995-04-16	DELIVER IN PERSON        	RAIL      	counts sleep furiously b
5637	47	4	1	14	13258.56	0.03	0.05	N	O	1996-07-20	1996-07-26	1996-08-14	COLLECT COD              	MAIL      	y bold deposits wak
5637	172	3	2	35	37525.95	0.09	0.08	N	O	1996-08-01	1996-08-04	1996-08-20	NONE                     	AIR       	s sleep blithely alongside of the ironic
5637	96	10	3	22	21913.98	0.01	0.07	N	O	1996-08-28	1996-07-30	1996-09-17	COLLECT COD              	REG AIR   	nding requests are ca
5637	66	1	4	16	15456.96	0.03	0.03	N	O	1996-09-08	1996-08-31	1996-09-29	TAKE BACK RETURN         	TRUCK     	d packages. express requests
5637	196	7	5	10	10961.90	0.01	0.00	N	O	1996-08-25	1996-08-11	1996-09-23	TAKE BACK RETURN         	MAIL      	ickly ironic gifts. blithely even cour
5637	129	4	6	27	27786.24	0.01	0.05	N	O	1996-06-27	1996-08-09	1996-07-27	DELIVER IN PERSON        	REG AIR   	oss the carefully express warhorses
5664	122	1	1	25	25553.00	0.00	0.06	N	O	1998-10-29	1998-09-23	1998-11-25	COLLECT COD              	FOB       	eposits: furiously ironic grouch
5664	173	2	2	9	9658.53	0.07	0.05	N	O	1998-07-31	1998-08-26	1998-08-12	COLLECT COD              	RAIL      	 ironic deposits haggle furiously. re
5664	53	4	3	31	29544.55	0.01	0.03	N	O	1998-11-10	1998-09-12	1998-12-07	TAKE BACK RETURN         	FOB       	ainst the never silent request
5664	138	9	4	33	34258.29	0.08	0.03	N	O	1998-08-29	1998-09-17	1998-09-25	DELIVER IN PERSON        	RAIL      	d the final 
5664	112	2	5	44	44532.84	0.01	0.06	N	O	1998-09-24	1998-09-26	1998-10-23	NONE                     	TRUCK     	ang thinly bold pa
5664	68	5	6	34	32914.04	0.09	0.01	N	O	1998-09-10	1998-10-05	1998-09-15	COLLECT COD              	RAIL      	st. fluffily pending foxes na
5664	182	3	7	9	9739.62	0.01	0.05	N	O	1998-11-04	1998-10-15	1998-11-20	TAKE BACK RETURN         	REG AIR   	yly. express ideas agai
5666	122	5	1	7	7154.84	0.09	0.08	R	F	1994-05-10	1994-04-06	1994-05-21	NONE                     	FOB       	 ideas. regular packag
5666	36	7	2	14	13104.42	0.08	0.01	A	F	1994-02-27	1994-04-11	1994-03-06	DELIVER IN PERSON        	TRUCK     	lar deposits nag against the slyly final d
5666	193	6	3	39	42634.41	0.00	0.01	A	F	1994-05-13	1994-04-02	1994-06-12	DELIVER IN PERSON        	TRUCK     	the even, final foxes. quickly iron
5666	131	2	4	24	24747.12	0.07	0.01	R	F	1994-02-14	1994-03-09	1994-03-06	DELIVER IN PERSON        	FOB       	on the carefully pending asympto
5666	109	10	5	36	36327.60	0.07	0.07	R	F	1994-03-15	1994-03-16	1994-03-18	COLLECT COD              	TRUCK     	accounts. furiousl
5669	191	2	1	7	7638.33	0.06	0.06	N	O	1996-06-19	1996-07-07	1996-07-11	COLLECT COD              	SHIP      	yly regular requests lose blithely. careful
5669	156	8	2	2	2112.30	0.06	0.07	N	O	1996-08-04	1996-06-15	1996-08-20	NONE                     	SHIP      	 blithely excuses. slyly
5669	158	9	3	40	42326.00	0.00	0.02	N	O	1996-08-30	1996-06-15	1996-09-07	TAKE BACK RETURN         	FOB       	ar accounts alongside of the final, p
5669	90	1	4	31	30692.79	0.04	0.05	N	O	1996-08-05	1996-06-10	1996-08-29	COLLECT COD              	AIR       	to beans against the regular depo
5669	140	6	5	30	31204.20	0.07	0.01	N	O	1996-07-14	1996-07-28	1996-08-10	TAKE BACK RETURN         	TRUCK     	l accounts. care
5696	137	3	1	28	29039.64	0.03	0.06	N	O	1995-07-03	1995-06-14	1995-07-27	COLLECT COD              	REG AIR   	 the fluffily brave pearls 
5696	59	1	2	46	44116.30	0.01	0.00	N	O	1995-08-10	1995-07-08	1995-08-25	COLLECT COD              	AIR       	ter the instruct
5696	167	2	3	42	44820.72	0.04	0.01	N	F	1995-06-06	1995-06-11	1995-06-19	TAKE BACK RETURN         	SHIP      	te furious
5696	98	10	4	20	19961.80	0.08	0.00	N	O	1995-06-25	1995-07-18	1995-07-16	NONE                     	TRUCK     	silent, pending ideas sleep fluffil
5696	124	9	5	19	19458.28	0.07	0.05	N	O	1995-08-31	1995-06-13	1995-09-10	COLLECT COD              	SHIP      	unusual requests sleep furiously ru
5696	132	8	6	37	38188.81	0.04	0.05	N	O	1995-07-21	1995-06-23	1995-08-19	NONE                     	RAIL      	 carefully expres
5696	102	9	7	6	6012.60	0.07	0.05	N	O	1995-08-03	1995-07-15	1995-09-01	DELIVER IN PERSON        	REG AIR   	n patterns lose slyly fina
5697	55	7	1	24	22921.20	0.10	0.07	R	F	1992-10-27	1992-11-28	1992-11-20	NONE                     	RAIL      	uffily iro
5697	16	10	2	43	39388.43	0.06	0.02	R	F	1992-12-08	1992-12-03	1992-12-17	TAKE BACK RETURN         	FOB       	blithely reg
5697	56	8	3	42	40154.10	0.03	0.01	A	F	1992-12-19	1992-12-08	1993-01-03	COLLECT COD              	TRUCK     	inal theodolites cajole after the bli
5699	2	7	1	24	21648.00	0.01	0.07	A	F	1992-10-21	1992-09-04	1992-11-04	COLLECT COD              	AIR       	kages. fin
5699	55	10	2	26	24831.30	0.06	0.06	R	F	1992-08-11	1992-09-21	1992-08-14	COLLECT COD              	MAIL      	y final deposits wake fluffily u
5699	18	2	3	48	44064.48	0.10	0.05	R	F	1992-11-23	1992-10-20	1992-11-29	DELIVER IN PERSON        	TRUCK     	s. carefully regul
5699	55	3	4	46	43932.30	0.08	0.02	A	F	1992-11-28	1992-09-23	1992-12-27	TAKE BACK RETURN         	FOB       	o the slyly
5699	28	7	5	21	19488.42	0.02	0.02	A	F	1992-10-13	1992-09-30	1992-10-19	NONE                     	MAIL      	lyly final pla
5699	191	5	6	30	32735.70	0.08	0.05	R	F	1992-11-13	1992-10-01	1992-12-11	DELIVER IN PERSON        	AIR       	 the carefully final 
5699	129	8	7	45	46310.40	0.09	0.06	A	F	1992-09-23	1992-10-22	1992-10-04	DELIVER IN PERSON        	SHIP      	rmanent packages sleep across the f
5728	44	1	1	47	44369.88	0.10	0.05	A	F	1994-12-13	1995-01-25	1994-12-25	TAKE BACK RETURN         	MAIL      	nd the bravely final deposits. final ideas
5728	159	1	2	40	42366.00	0.05	0.08	A	F	1995-03-28	1995-01-17	1995-04-14	TAKE BACK RETURN         	SHIP      	final deposits. theodolite
5732	139	5	1	26	27017.38	0.02	0.07	N	O	1997-08-18	1997-10-25	1997-09-12	TAKE BACK RETURN         	TRUCK     	totes cajole according to the theodolites.
5733	33	4	1	39	36388.17	0.01	0.07	A	F	1993-03-22	1993-05-24	1993-04-04	DELIVER IN PERSON        	FOB       	side of the
5734	183	4	1	29	31412.22	0.05	0.01	N	O	1997-12-01	1997-12-08	1997-12-23	NONE                     	RAIL      	structions cajole final, express 
5734	150	3	2	6	6300.90	0.07	0.00	N	O	1997-10-27	1997-12-19	1997-11-02	COLLECT COD              	RAIL      	s. regular platelets cajole furiously. regu
5734	67	8	3	10	9670.60	0.01	0.03	N	O	1997-12-28	1997-12-24	1998-01-24	DELIVER IN PERSON        	TRUCK     	equests; accounts above
5735	60	1	1	41	39362.46	0.01	0.01	R	F	1994-12-23	1995-02-10	1995-01-22	COLLECT COD              	MAIL      	lthily ruthless i
5760	1	8	1	6	5406.00	0.09	0.03	R	F	1994-07-30	1994-07-31	1994-08-16	COLLECT COD              	REG AIR   	ng the acco
5760	6	1	2	24	21744.00	0.04	0.05	A	F	1994-07-15	1994-07-04	1994-08-08	NONE                     	MAIL      	s. bravely ironic accounts among
5760	148	5	3	8	8385.12	0.07	0.04	A	F	1994-09-06	1994-08-03	1994-10-06	NONE                     	AIR       	l accounts among the carefully even de
5760	123	4	4	19	19439.28	0.10	0.01	R	F	1994-08-02	1994-08-02	1994-08-15	COLLECT COD              	SHIP      	sits nag. even, regular ideas cajole b
5760	166	1	5	6	6396.96	0.03	0.07	R	F	1994-06-09	1994-07-06	1994-06-16	DELIVER IN PERSON        	MAIL      	 shall have to cajole along the 
5761	47	6	1	41	38828.64	0.08	0.00	N	O	1998-07-31	1998-08-09	1998-08-08	TAKE BACK RETURN         	TRUCK     	pecial deposits. qu
5761	108	9	2	36	36291.60	0.00	0.07	N	O	1998-09-07	1998-09-21	1998-09-11	TAKE BACK RETURN         	TRUCK     	 pinto beans thrash alongside of the pendi
5761	198	2	3	49	53811.31	0.04	0.08	N	O	1998-07-14	1998-08-20	1998-07-25	NONE                     	SHIP      	ly bold accounts wake above the
5762	175	6	1	6	6451.02	0.05	0.02	N	O	1997-04-07	1997-03-25	1997-05-02	NONE                     	AIR       	ironic dependencies doze carefu
5762	102	9	2	27	27056.70	0.02	0.08	N	O	1997-02-21	1997-05-08	1997-03-23	NONE                     	REG AIR   	across the bold ideas. carefully sp
5762	89	10	3	40	39563.20	0.00	0.08	N	O	1997-04-30	1997-05-09	1997-05-08	COLLECT COD              	SHIP      	al instructions. furiousl
5762	133	4	4	47	48557.11	0.05	0.06	N	O	1997-03-02	1997-03-23	1997-03-19	NONE                     	RAIL      	equests sleep after the furiously ironic pa
5762	25	6	5	28	25900.56	0.02	0.06	N	O	1997-02-22	1997-03-25	1997-02-24	TAKE BACK RETURN         	SHIP      	ic foxes among the blithely qui
5762	12	6	6	12	10944.12	0.00	0.06	N	O	1997-04-18	1997-04-27	1997-05-11	DELIVER IN PERSON        	REG AIR   	ages are abo
5764	101	2	1	28	28030.80	0.04	0.04	A	F	1993-12-07	1993-12-20	1993-12-26	TAKE BACK RETURN         	RAIL      	sleep furi
5764	200	3	2	20	22004.00	0.10	0.05	A	F	1993-10-17	1993-12-24	1993-10-18	TAKE BACK RETURN         	FOB       	ng to the fluffily qu
5764	188	9	3	4	4352.72	0.03	0.05	A	F	1993-10-25	1993-12-23	1993-11-06	DELIVER IN PERSON        	AIR       	ily regular courts haggle
5765	162	7	1	31	32926.96	0.00	0.06	A	F	1995-01-11	1995-02-13	1995-01-23	TAKE BACK RETURN         	AIR       	r foxes. ev
5765	124	9	2	29	29699.48	0.07	0.08	A	F	1994-12-29	1995-02-01	1995-01-26	NONE                     	RAIL      	nic requests. deposits wake quickly among 
5765	139	10	3	31	32213.03	0.05	0.01	R	F	1995-03-01	1995-01-23	1995-03-31	TAKE BACK RETURN         	REG AIR   	the furiou
5765	152	4	4	46	48398.90	0.07	0.07	R	F	1995-03-13	1995-02-12	1995-03-20	DELIVER IN PERSON        	MAIL      	ccounts sleep about th
5765	174	3	5	48	51560.16	0.09	0.02	A	F	1995-03-30	1995-01-14	1995-04-09	DELIVER IN PERSON        	SHIP      	theodolites integrate furiously
5765	83	4	6	41	40306.28	0.04	0.00	A	F	1994-12-31	1995-02-11	1995-01-17	TAKE BACK RETURN         	SHIP      	 furiously. slyly sile
5765	42	5	7	21	19782.84	0.05	0.04	R	F	1995-04-05	1995-02-12	1995-05-05	COLLECT COD              	TRUCK     	ole furiously. quick, special dependencies 
5766	188	9	1	1	1088.18	0.10	0.01	R	F	1994-01-16	1993-11-16	1994-01-23	NONE                     	MAIL      	blithely regular the
5766	149	8	2	39	40916.46	0.02	0.07	A	F	1993-10-24	1993-12-07	1993-11-08	DELIVER IN PERSON        	SHIP      	 furiously unusual courts. slyly final pear
5766	118	8	3	4	4072.44	0.08	0.08	R	F	1993-11-10	1993-10-30	1993-12-01	COLLECT COD              	TRUCK     	ly even requests. furiou
5793	53	5	1	20	19061.00	0.05	0.03	N	O	1997-10-05	1997-09-04	1997-10-30	COLLECT COD              	AIR       	e carefully ex
5793	170	5	2	41	43876.97	0.06	0.06	N	O	1997-08-04	1997-10-10	1997-08-12	DELIVER IN PERSON        	TRUCK     	snooze quick
5793	43	4	3	8	7544.32	0.07	0.03	N	O	1997-08-16	1997-09-08	1997-08-28	COLLECT COD              	AIR       	al foxes l
5793	148	7	4	48	50310.72	0.02	0.02	N	O	1997-09-27	1997-08-23	1997-10-27	DELIVER IN PERSON        	REG AIR   	quickly enticing excuses use slyly abov
5795	193	6	1	34	37168.46	0.09	0.05	A	F	1992-08-21	1992-07-30	1992-08-27	COLLECT COD              	REG AIR   	al instructions must affix along the ironic
5797	61	6	1	17	16338.02	0.09	0.03	N	O	1997-12-13	1998-01-12	1997-12-23	NONE                     	REG AIR   	the ironic, even theodoli
5798	127	8	1	2	2054.24	0.09	0.00	N	O	1998-05-25	1998-06-22	1998-06-09	COLLECT COD              	FOB       	e furiously across 
5798	124	9	2	14	14337.68	0.06	0.05	N	O	1998-04-01	1998-06-14	1998-04-27	NONE                     	RAIL      	he special, bold packages. carefully iron
5798	134	5	3	22	22750.86	0.02	0.01	N	O	1998-06-24	1998-06-06	1998-07-20	COLLECT COD              	TRUCK     	sits poach carefully
5798	146	3	4	40	41845.60	0.08	0.06	N	O	1998-07-09	1998-06-24	1998-07-16	NONE                     	TRUCK     	 integrate carefu
5798	149	8	5	7	7343.98	0.06	0.07	N	O	1998-06-06	1998-05-10	1998-06-07	NONE                     	SHIP      	ts against the blithely final p
5798	38	4	6	9	8442.27	0.06	0.02	N	O	1998-05-05	1998-05-25	1998-05-09	DELIVER IN PERSON        	REG AIR   	e blithely
5798	115	9	7	32	32483.52	0.08	0.01	N	O	1998-04-27	1998-05-03	1998-05-08	TAKE BACK RETURN         	REG AIR   	ubt blithely above the 
5824	77	7	1	40	39082.80	0.06	0.06	N	O	1997-01-14	1997-01-17	1997-02-02	NONE                     	REG AIR   	he final packag
5824	182	3	2	42	45451.56	0.09	0.00	N	O	1997-02-01	1997-02-20	1997-02-07	COLLECT COD              	SHIP      	ts sleep. carefully regular accounts h
5824	73	1	3	16	15569.12	0.03	0.02	N	O	1997-02-13	1997-01-07	1997-02-17	TAKE BACK RETURN         	TRUCK     	sly express Ti
5824	92	5	4	32	31746.88	0.03	0.02	N	O	1997-02-16	1997-01-24	1997-02-20	DELIVER IN PERSON        	RAIL      	ven requests. 
5824	107	8	5	44	44312.40	0.08	0.03	N	O	1997-01-24	1997-01-31	1997-02-11	COLLECT COD              	TRUCK     	fily fluffily bold
5826	144	1	1	4	4176.56	0.03	0.06	N	O	1998-07-31	1998-09-10	1998-08-27	NONE                     	AIR       	 packages across the fluffily spec
5826	64	5	2	18	17353.08	0.04	0.01	N	O	1998-07-17	1998-09-03	1998-07-22	NONE                     	SHIP      	atelets use above t
5830	160	2	1	29	30744.64	0.10	0.02	R	F	1993-06-19	1993-05-10	1993-07-13	DELIVER IN PERSON        	REG AIR   	y bold excuses
5831	191	2	1	2	2182.38	0.10	0.01	N	O	1997-02-09	1997-01-20	1997-03-07	TAKE BACK RETURN         	TRUCK     	quickly silent req
5831	74	3	2	33	32144.31	0.04	0.03	N	O	1996-11-20	1997-01-18	1996-12-18	TAKE BACK RETURN         	MAIL      	 instructions wake. slyly sil
5831	82	3	3	6	5892.48	0.05	0.07	N	O	1997-01-29	1997-01-14	1997-02-09	NONE                     	MAIL      	ly ironic accounts nag pendin
5831	13	10	4	46	41998.46	0.06	0.02	N	O	1997-02-24	1997-01-18	1997-03-02	COLLECT COD              	MAIL      	ly final pa
5831	43	4	5	37	34892.48	0.05	0.01	N	O	1997-01-17	1997-02-08	1997-02-01	NONE                     	FOB       	uriously even requests
5856	4	1	1	1	904.00	0.03	0.02	A	F	1994-12-29	1995-01-07	1995-01-10	TAKE BACK RETURN         	MAIL      	tly. special deposits wake blithely even
5856	35	6	2	35	32726.05	0.09	0.02	R	F	1994-11-24	1994-12-23	1994-11-30	COLLECT COD              	AIR       	excuses. finally ir
5856	153	4	3	39	41072.85	0.05	0.03	A	F	1995-01-18	1995-01-11	1995-01-19	DELIVER IN PERSON        	TRUCK     	uickly quickly fluffy in
5858	121	4	1	20	20422.40	0.02	0.06	A	F	1992-07-23	1992-08-26	1992-07-24	COLLECT COD              	SHIP      	uffily unusual pinto beans sleep
5858	16	7	2	36	32976.36	0.00	0.05	A	F	1992-09-25	1992-08-16	1992-10-11	NONE                     	SHIP      	osits wake quickly quickly sile
5858	148	5	3	7	7336.98	0.08	0.02	A	F	1992-10-07	1992-08-16	1992-10-15	TAKE BACK RETURN         	REG AIR   	. doggedly regular packages use pendin
5858	164	9	4	46	48951.36	0.07	0.06	R	F	1992-09-07	1992-10-06	1992-10-06	DELIVER IN PERSON        	MAIL      	posits withi
5858	161	8	5	18	19100.88	0.00	0.07	A	F	1992-11-05	1992-10-08	1992-12-03	NONE                     	TRUCK     	al excuses. bold
5858	154	9	6	7	7379.05	0.04	0.00	A	F	1992-09-14	1992-10-01	1992-10-01	TAKE BACK RETURN         	RAIL      	dly pending ac
5858	11	5	7	50	45550.50	0.06	0.00	R	F	1992-07-20	1992-10-07	1992-07-25	NONE                     	TRUCK     	r the ironic ex
5859	175	4	1	50	53758.50	0.07	0.01	N	O	1997-07-08	1997-06-20	1997-07-27	COLLECT COD              	MAIL      	ly regular deposits use. ironic
5859	9	6	2	17	15453.00	0.03	0.03	N	O	1997-05-15	1997-06-30	1997-05-26	DELIVER IN PERSON        	AIR       	ly ironic requests. quickly unusual pin
5859	46	3	3	33	31219.32	0.10	0.04	N	O	1997-07-08	1997-06-22	1997-07-18	TAKE BACK RETURN         	TRUCK     	eposits unwind furiously final pinto bea
5859	93	4	4	40	39723.60	0.09	0.02	N	O	1997-08-05	1997-06-17	1997-08-20	NONE                     	REG AIR   	l dependenci
5859	153	8	5	35	36860.25	0.00	0.08	N	O	1997-05-28	1997-07-14	1997-06-15	COLLECT COD              	TRUCK     	egular acco
5859	44	5	6	9	8496.36	0.01	0.02	N	O	1997-06-15	1997-06-06	1997-06-20	NONE                     	RAIL      	ges boost quickly. blithely r
5859	191	5	7	27	29462.13	0.05	0.08	N	O	1997-07-30	1997-07-08	1997-08-08	NONE                     	MAIL      	 across th
5860	51	3	1	10	9510.50	0.04	0.04	A	F	1992-03-11	1992-03-30	1992-03-31	NONE                     	MAIL      	ual patterns try to eat carefully above
5861	191	5	1	32	34918.08	0.00	0.03	N	O	1997-05-27	1997-05-29	1997-05-28	TAKE BACK RETURN         	MAIL      	nt asymptotes. carefully express request
5861	86	7	2	6	5916.48	0.10	0.03	N	O	1997-07-28	1997-05-18	1997-08-24	TAKE BACK RETURN         	TRUCK     	olites. slyly
5862	113	7	1	4	4052.44	0.09	0.06	N	O	1997-06-04	1997-04-26	1997-06-19	NONE                     	TRUCK     	yly silent deposit
5862	2	7	2	29	26158.00	0.03	0.05	N	O	1997-04-02	1997-04-16	1997-04-04	NONE                     	FOB       	e fluffily. furiously
5888	62	7	1	46	44254.76	0.02	0.00	N	O	1996-11-18	1996-11-05	1996-12-08	TAKE BACK RETURN         	FOB       	yly final accounts hag
5888	112	3	2	24	24290.64	0.03	0.01	N	O	1996-11-07	1996-11-30	1996-11-20	COLLECT COD              	SHIP      	ing to the spe
5889	77	7	1	17	16610.19	0.09	0.02	N	O	1995-07-01	1995-08-12	1995-07-25	NONE                     	AIR       	blithely pending packages. flu
5890	113	4	1	38	38498.18	0.01	0.08	A	F	1993-02-14	1992-12-09	1993-02-27	COLLECT COD              	FOB       	 accounts. carefully final asymptotes
5891	85	6	1	22	21671.76	0.00	0.06	R	F	1993-01-01	1993-02-18	1993-01-14	DELIVER IN PERSON        	TRUCK     	iresias cajole deposits. special, ir
5891	186	7	2	9	9775.62	0.03	0.07	R	F	1993-01-20	1993-02-27	1993-02-10	COLLECT COD              	REG AIR   	cajole carefully 
5891	30	9	3	10	9300.30	0.08	0.01	A	F	1993-04-14	1993-02-07	1993-04-15	DELIVER IN PERSON        	RAIL      	nding requests. b
5892	148	9	1	7	7336.98	0.02	0.03	N	O	1995-06-26	1995-07-18	1995-07-25	COLLECT COD              	AIR       	e furiously. quickly even deposits da
5892	150	9	2	37	38855.55	0.09	0.06	N	O	1995-08-12	1995-06-11	1995-09-05	NONE                     	REG AIR   	maintain. bold, expre
5892	3	4	3	28	25284.00	0.03	0.06	N	O	1995-08-16	1995-07-06	1995-08-22	DELIVER IN PERSON        	MAIL      	ithely unusual accounts will have to integ
5892	75	6	4	23	22426.61	0.08	0.04	R	F	1995-05-18	1995-07-06	1995-05-29	COLLECT COD              	MAIL      	 foxes nag slyly about the qui
5894	8	5	1	23	20884.00	0.04	0.08	A	F	1994-09-05	1994-10-27	1994-09-13	NONE                     	TRUCK     	 furiously even deposits haggle alw
5894	79	8	2	48	46995.36	0.04	0.08	A	F	1994-09-04	1994-11-03	1994-09-17	NONE                     	TRUCK     	 asymptotes among the blithely silent 
5895	15	9	1	38	34770.38	0.05	0.08	N	O	1997-04-05	1997-03-06	1997-05-03	DELIVER IN PERSON        	RAIL      	ts are furiously. regular, final excuses 
5895	122	3	2	47	48039.64	0.04	0.06	N	O	1997-04-27	1997-03-17	1997-05-07	DELIVER IN PERSON        	AIR       	r packages wake carefull
5895	84	5	3	49	48219.92	0.03	0.07	N	O	1997-03-15	1997-02-17	1997-04-04	NONE                     	TRUCK     	permanent foxes. packages
5895	146	7	4	31	32430.34	0.03	0.01	N	O	1997-03-03	1997-03-30	1997-03-08	TAKE BACK RETURN         	TRUCK     	 final deposits nod slyly careful
5895	200	1	5	20	22004.00	0.07	0.00	N	O	1997-04-30	1997-02-07	1997-05-08	DELIVER IN PERSON        	AIR       	gular deposits wake blithely carefully fin
5895	78	7	6	15	14671.05	0.08	0.08	N	O	1997-04-19	1997-03-09	1997-05-13	TAKE BACK RETURN         	RAIL      	silent package
5920	187	8	1	50	54359.00	0.06	0.00	A	F	1995-03-13	1995-01-03	1995-03-31	TAKE BACK RETURN         	RAIL      	across the carefully pending platelets
5920	58	9	2	24	22993.20	0.01	0.05	A	F	1994-12-28	1995-01-21	1994-12-31	DELIVER IN PERSON        	FOB       	fully regular dolphins. furiousl
5920	117	1	3	2	2034.22	0.08	0.07	A	F	1995-02-18	1995-01-13	1995-03-04	NONE                     	SHIP      	 evenly spe
5920	12	2	4	28	25536.28	0.06	0.02	R	F	1994-12-17	1995-02-13	1994-12-31	NONE                     	SHIP      	le slyly slyly even deposits. f
5920	100	4	5	42	42004.20	0.09	0.08	A	F	1994-12-18	1995-01-07	1995-01-14	COLLECT COD              	AIR       	lar, ironic dependencies sno
5923	177	8	1	27	29083.59	0.08	0.03	N	O	1997-08-16	1997-06-27	1997-08-29	DELIVER IN PERSON        	RAIL      	arefully i
5923	119	3	2	42	42802.62	0.01	0.08	N	O	1997-09-16	1997-07-23	1997-09-27	COLLECT COD              	REG AIR   	y regular theodolites w
5923	108	5	3	2	2016.20	0.06	0.05	N	O	1997-06-19	1997-07-31	1997-06-28	TAKE BACK RETURN         	TRUCK     	express patterns. even deposits
5923	174	4	4	46	49411.82	0.05	0.04	N	O	1997-07-29	1997-07-23	1997-08-23	COLLECT COD              	SHIP      	nto beans cajole blithe
5923	59	4	5	35	33566.75	0.04	0.05	N	O	1997-07-21	1997-07-11	1997-08-01	DELIVER IN PERSON        	AIR       	sts affix unusual, final requests. request
5925	87	8	1	42	41457.36	0.05	0.02	N	O	1996-03-05	1996-01-13	1996-03-10	COLLECT COD              	SHIP      	to the furiously
5925	125	4	2	31	31778.72	0.03	0.03	N	O	1996-01-02	1995-12-14	1996-01-07	TAKE BACK RETURN         	FOB       	e slyly. furiously regular deposi
5925	89	10	3	50	49454.00	0.03	0.04	N	O	1996-02-14	1996-01-10	1996-02-15	NONE                     	TRUCK     	es. stealthily express pains print bli
5925	54	9	4	30	28621.50	0.02	0.07	N	O	1996-02-21	1996-02-11	1996-03-10	NONE                     	TRUCK     	 the packa
5925	160	1	5	41	43466.56	0.00	0.06	N	O	1996-02-03	1995-12-24	1996-02-20	NONE                     	SHIP      	 across the pending deposits nag caref
5925	50	9	6	48	45602.40	0.02	0.00	N	O	1996-02-03	1996-01-19	1996-03-04	DELIVER IN PERSON        	REG AIR   	 haggle after the fo
5926	90	1	1	8	7920.72	0.02	0.00	R	F	1994-07-17	1994-07-20	1994-08-11	COLLECT COD              	MAIL      	gle furiously express foxes. bo
5926	50	9	2	27	25651.35	0.09	0.05	A	F	1994-07-05	1994-08-11	1994-08-02	DELIVER IN PERSON        	MAIL      	ironic requests
5926	127	8	3	46	47247.52	0.01	0.03	R	F	1994-09-05	1994-08-12	1994-09-11	COLLECT COD              	RAIL      	ts integrate. courts haggl
5926	190	1	4	23	25074.37	0.01	0.02	A	F	1994-07-23	1994-08-10	1994-07-27	DELIVER IN PERSON        	FOB       	ickly special packages among 
5952	200	2	1	49	53909.80	0.10	0.02	N	O	1997-06-30	1997-07-10	1997-07-02	COLLECT COD              	AIR       	e furiously regular
5952	191	5	2	11	12003.09	0.10	0.05	N	O	1997-05-13	1997-06-04	1997-05-27	DELIVER IN PERSON        	FOB       	y nag blithely aga
5952	71	2	3	43	41756.01	0.01	0.01	N	O	1997-06-29	1997-06-06	1997-07-15	COLLECT COD              	MAIL      	posits sleep furiously quickly final p
5952	158	3	4	23	24337.45	0.00	0.07	N	O	1997-05-13	1997-06-27	1997-05-20	NONE                     	TRUCK     	e blithely packages. eve
5954	147	6	1	8	8377.12	0.03	0.00	A	F	1993-03-27	1993-01-22	1993-04-04	TAKE BACK RETURN         	AIR       	unusual th
5954	81	2	2	40	39243.20	0.02	0.01	A	F	1992-12-30	1993-01-16	1993-01-09	COLLECT COD              	RAIL      	iously ironic deposits after
5954	94	8	3	20	19881.80	0.09	0.07	A	F	1992-12-25	1993-02-05	1992-12-31	COLLECT COD              	REG AIR   	 accounts wake carefu
5954	145	4	4	20	20902.80	0.00	0.01	R	F	1993-02-27	1993-01-04	1993-03-08	NONE                     	TRUCK     	ke furiously blithely special packa
5954	100	4	5	35	35003.50	0.04	0.06	A	F	1993-03-17	1993-02-06	1993-04-10	NONE                     	SHIP      	tions maintain slyly. furious
5954	193	5	6	39	42634.41	0.04	0.08	A	F	1993-02-27	1993-02-25	1993-03-29	DELIVER IN PERSON        	REG AIR   	 always regular dolphins. furiously p
5955	140	1	1	14	14561.96	0.08	0.08	N	O	1995-06-22	1995-05-23	1995-06-24	DELIVER IN PERSON        	TRUCK     	 unusual, bold theodolit
5955	62	7	2	15	14430.90	0.08	0.07	R	F	1995-04-22	1995-05-28	1995-04-27	NONE                     	FOB       	y final accounts above the regu
5955	112	9	3	40	40484.40	0.03	0.00	R	F	1995-04-01	1995-06-11	1995-04-27	NONE                     	FOB       	oss the fluffily regular
5956	155	3	1	10	10551.50	0.04	0.05	N	O	1998-07-27	1998-07-04	1998-08-21	NONE                     	MAIL      	ic packages am
5956	55	7	2	23	21966.15	0.08	0.03	N	O	1998-06-06	1998-07-10	1998-06-15	DELIVER IN PERSON        	RAIL      	ly slyly special 
5956	175	5	3	47	50532.99	0.04	0.06	N	O	1998-09-06	1998-06-29	1998-09-18	TAKE BACK RETURN         	MAIL      	lyly express theodol
5956	20	10	4	40	36800.80	0.09	0.05	N	O	1998-06-11	1998-07-19	1998-06-21	NONE                     	MAIL      	final theodolites sleep carefully ironic c
5958	149	8	1	33	34621.62	0.02	0.04	N	O	1995-09-24	1995-12-12	1995-10-05	COLLECT COD              	MAIL      	lar, regular accounts wake furi
5958	43	6	2	23	21689.92	0.03	0.04	N	O	1995-09-26	1995-10-19	1995-09-27	COLLECT COD              	SHIP      	regular requests. bold, bold deposits unwin
5958	153	8	3	42	44232.30	0.10	0.00	N	O	1995-12-12	1995-10-19	1996-01-09	NONE                     	AIR       	n accounts. final, ironic packages 
5958	39	10	4	18	16902.54	0.04	0.05	N	O	1995-12-02	1995-10-17	1995-12-22	COLLECT COD              	FOB       	regular requests haggle
5958	132	8	5	32	33028.16	0.06	0.00	N	O	1995-09-20	1995-12-10	1995-10-14	COLLECT COD              	REG AIR   	e carefully special theodolites. carefully 
5959	135	1	1	49	50721.37	0.07	0.03	R	F	1992-07-16	1992-08-09	1992-08-14	DELIVER IN PERSON        	SHIP      	usual packages haggle slyly pi
5959	147	8	2	17	17801.38	0.09	0.07	R	F	1992-06-10	1992-07-06	1992-06-23	COLLECT COD              	MAIL      	ackages. blithely ex
5959	5	6	3	4	3620.00	0.04	0.03	R	F	1992-06-14	1992-07-05	1992-07-01	NONE                     	MAIL      	gular requests ar
5959	196	7	4	13	14250.47	0.03	0.00	A	F	1992-07-29	1992-07-13	1992-08-20	COLLECT COD              	SHIP      	ar forges. deposits det
5959	40	6	5	37	34781.48	0.04	0.01	R	F	1992-06-05	1992-07-18	1992-06-29	NONE                     	TRUCK     	endencies. brai
5959	119	3	6	35	35668.85	0.03	0.00	A	F	1992-05-27	1992-06-19	1992-06-23	NONE                     	TRUCK     	ely silent deposits. 
5959	43	10	7	47	44322.88	0.02	0.01	R	F	1992-08-28	1992-07-24	1992-09-09	TAKE BACK RETURN         	RAIL      	deposits. slyly special cou
5986	79	7	1	26	25455.82	0.00	0.00	R	F	1992-08-10	1992-05-23	1992-08-24	TAKE BACK RETURN         	SHIP      	e fluffily ironic ideas. silent 
5986	196	8	2	25	27404.75	0.03	0.06	A	F	1992-06-16	1992-07-17	1992-06-29	TAKE BACK RETURN         	MAIL      	 instructions. slyly regular de
5986	30	5	3	1	930.03	0.07	0.06	A	F	1992-05-21	1992-06-21	1992-05-24	DELIVER IN PERSON        	REG AIR   	fix quickly quickly final deposits. fluffil
5986	90	1	4	31	30692.79	0.00	0.03	A	F	1992-08-21	1992-06-29	1992-09-14	NONE                     	AIR       	structions! furiously pending instructi
5986	136	7	5	6	6216.78	0.05	0.02	A	F	1992-07-16	1992-06-10	1992-07-29	DELIVER IN PERSON        	RAIL      	al foxes within the slyly speci
5987	23	2	1	1	923.02	0.01	0.04	N	O	1996-09-13	1996-10-29	1996-09-21	DELIVER IN PERSON        	REG AIR   	refully final excuses haggle furiously ag
5987	176	5	2	20	21523.40	0.10	0.06	N	O	1996-11-28	1996-09-17	1996-12-05	TAKE BACK RETURN         	RAIL      	ing excuses nag quickly always bold
5987	92	3	3	43	42659.87	0.08	0.04	N	O	1996-10-30	1996-10-13	1996-11-12	NONE                     	AIR       	theodolites wake above the furiously b
5987	97	1	4	37	36892.33	0.08	0.08	N	O	1996-10-15	1996-10-27	1996-11-09	NONE                     	MAIL      	le furiously carefully special 
\.


--
-- Data for Name: nation; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.nation (n_nationkey, n_name, n_regionkey, n_comment) FROM stdin;
0	ALGERIA                  	0	 haggle. carefully final deposits detect slyly agai
1	ARGENTINA                	1	al foxes promise slyly according to the regular accounts. bold requests alon
2	BRAZIL                   	1	y alongside of the pending deposits. carefully special packages are about the ironic forges. slyly special 
3	CANADA                   	1	eas hang ironic, silent packages. slyly regular packages are furiously over the tithes. fluffily bold
4	EGYPT                    	4	y above the carefully unusual theodolites. final dugouts are quickly across the furiously regular d
5	ETHIOPIA                 	0	ven packages wake quickly. regu
6	FRANCE                   	3	refully final requests. regular, ironi
7	GERMANY                  	3	l platelets. regular accounts x-ray: unusual, regular acco
8	INDIA                    	2	ss excuses cajole slyly across the packages. deposits print aroun
9	INDONESIA                	2	 slyly express asymptotes. regular deposits haggle slyly. carefully ironic hockey players sleep blithely. carefull
10	IRAN                     	4	efully alongside of the slyly final dependencies. 
11	IRAQ                     	4	nic deposits boost atop the quickly final requests? quickly regula
12	JAPAN                    	2	ously. final, express gifts cajole a
13	JORDAN                   	4	ic deposits are blithely about the carefully regular pa
14	KENYA                    	0	 pending excuses haggle furiously deposits. pending, express pinto beans wake fluffily past t
15	MOROCCO                  	0	rns. blithely bold courts among the closely regular packages use furiously bold platelets?
16	MOZAMBIQUE               	0	s. ironic, unusual asymptotes wake blithely r
17	PERU                     	1	platelets. blithely pending dependencies use fluffily across the even pinto beans. carefully silent accoun
18	CHINA                    	2	c dependencies. furiously express notornis sleep slyly regular accounts. ideas sleep. depos
19	ROMANIA                  	3	ular asymptotes are about the furious multipliers. express dependencies nag above the ironically ironic account
20	SAUDI ARABIA             	4	ts. silent requests haggle. closely express packages sleep across the blithely
21	VIETNAM                  	2	hely enticingly express accounts. even, final 
22	RUSSIA                   	3	 requests against the platelets use never according to the quickly regular pint
23	UNITED KINGDOM           	3	eans boost carefully special requests. accounts are. carefull
24	UNITED STATES            	1	y final packages. slow foxes cajole quickly. quickly silent platelets breach ironic accounts. unusual pinto be
\.


--
-- Data for Name: orders; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.orders (o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, o_orderyear) FROM stdin;
3685	16	F	154958.89	1992-01-17	3-MEDIUM       	Clerk#000000954	0	 sleep fluffily special ide	1992
3910	64	O	47272.67	1996-08-26	3-MEDIUM       	Clerk#000000270	0	ickly. furiously final packag	1996
1	37	O	131251.81	1996-01-02	5-LOW          	Clerk#000000951	0	nstructions sleep furiously among 	1996
5	46	F	86615.25	1994-07-30	5-LOW          	Clerk#000000925	0	quickly. bold deposits sleep slyly. packages use slyly	1994
6	56	F	36468.55	1992-02-21	4-NOT SPECIFIED	Clerk#000000058	0	ggle. special, final requests are against the furiously specia	1992
7	40	O	171488.73	1996-01-10	2-HIGH         	Clerk#000000470	0	ly special requests 	1996
32	131	O	116923.00	1995-07-16	2-HIGH         	Clerk#000000616	0	ise blithely bold, regular requests. quickly unusual dep	1995
33	67	F	99798.76	1993-10-27	3-MEDIUM       	Clerk#000000409	0	uriously. furiously final request	1993
34	62	O	41670.02	1998-07-21	3-MEDIUM       	Clerk#000000223	0	ly final packages. fluffily final deposits wake blithely ideas. spe	1998
35	128	O	148789.52	1995-10-23	4-NOT SPECIFIED	Clerk#000000259	0	zzle. carefully enticing deposits nag furio	1995
65	17	P	65883.92	1995-03-18	1-URGENT       	Clerk#000000632	0	ular requests are blithely pending orbits-- even requests against the deposit	1995
66	130	F	79258.24	1994-01-20	5-LOW          	Clerk#000000743	0	y pending requests integrate	1994
68	29	O	215135.72	1998-04-18	3-MEDIUM       	Clerk#000000440	0	 pinto beans sleep carefully. blithely ironic deposits haggle furiously acro	1998
69	85	F	162176.23	1994-06-04	4-NOT SPECIFIED	Clerk#000000330	0	 depths atop the slyly thin deposits detect among the furiously silent accou	1994
71	4	O	178821.73	1998-01-24	4-NOT SPECIFIED	Clerk#000000271	0	 express deposits along the blithely regul	1998
97	22	F	68908.31	1993-01-29	3-MEDIUM       	Clerk#000000547	0	hang blithely along the regular accounts. furiously even ideas after the	1993
98	106	F	51004.44	1994-09-25	1-URGENT       	Clerk#000000448	0	c asymptotes. quickly regular packages should have to nag re	1994
100	148	O	141311.01	1998-02-28	4-NOT SPECIFIED	Clerk#000000577	0	heodolites detect slyly alongside of the ent	1998
101	28	O	95591.40	1996-03-17	3-MEDIUM       	Clerk#000000419	0	ding accounts above the slyly final asymptote	1996
128	74	F	36333.34	1992-06-15	1-URGENT       	Clerk#000000385	0	ns integrate fluffily. ironic asymptotes after the regular excuses nag around 	1992
129	73	F	188124.55	1992-11-19	5-LOW          	Clerk#000000859	0	ing tithes. carefully pending deposits boost about the silently express 	1992
130	37	F	115717.37	1992-05-08	2-HIGH         	Clerk#000000036	0	le slyly unusual, regular packages? express deposits det	1992
131	94	F	96596.81	1994-06-08	3-MEDIUM       	Clerk#000000625	0	after the fluffily special foxes integrate s	1994
132	28	F	118802.62	1993-06-11	3-MEDIUM       	Clerk#000000488	0	sits are daringly accounts. carefully regular foxes sleep slyly about the	1993
161	17	F	19056.99	1994-08-31	2-HIGH         	Clerk#000000322	0	carefully! special instructions sin	1994
162	16	O	2158.13	1995-05-08	3-MEDIUM       	Clerk#000000378	0	nts hinder fluffily ironic instructions. express, express excuses 	1995
165	28	F	141824.23	1993-01-30	4-NOT SPECIFIED	Clerk#000000292	0	across the blithely regular accounts. bold	1993
193	80	F	48053.18	1993-08-08	1-URGENT       	Clerk#000000025	0	the furiously final pin	1993
194	62	F	114097.63	1992-04-05	3-MEDIUM       	Clerk#000000352	0	egular requests haggle slyly regular, regular pinto beans. asymptote	1992
195	136	F	120053.52	1993-12-28	3-MEDIUM       	Clerk#000000216	0	old forges are furiously sheaves. slyly fi	1993
224	4	F	155680.60	1994-06-18	4-NOT SPECIFIED	Clerk#000000642	0	r the quickly thin courts. carefully	1994
226	128	F	180119.22	1993-03-10	2-HIGH         	Clerk#000000756	0	s are carefully at the blithely ironic acc	1993
227	10	O	46076.46	1995-11-10	5-LOW          	Clerk#000000919	0	 express instructions. slyly regul	1995
228	46	F	2638.98	1993-02-25	1-URGENT       	Clerk#000000562	0	es was slyly among the regular foxes. blithely regular dependenci	1993
230	103	F	107231.60	1993-10-27	1-URGENT       	Clerk#000000520	0	odolites. carefully quick requ	1993
231	91	F	141554.06	1994-09-29	2-HIGH         	Clerk#000000446	0	 packages haggle slyly after the carefully ironic instruct	1994
260	106	O	179292.14	1996-12-10	3-MEDIUM       	Clerk#000000960	0	lently regular pinto beans sleep after the slyly e	1996
261	47	F	201003.12	1993-06-29	3-MEDIUM       	Clerk#000000310	0	ully fluffily brave instructions. furiousl	1993
289	104	O	131092.67	1997-02-10	3-MEDIUM       	Clerk#000000103	0	sily. slyly special excuse	1997
291	142	F	66817.05	1994-03-13	1-URGENT       	Clerk#000000923	0	dolites. carefully regular pinto beans cajol	1994
292	23	F	30783.05	1992-01-13	2-HIGH         	Clerk#000000193	0	g pinto beans will have to sleep f	1992
294	52	F	30059.47	1993-07-16	3-MEDIUM       	Clerk#000000499	0	kly according to the frays. final dolphins affix quickly 	1993
322	134	F	127068.89	1992-03-19	1-URGENT       	Clerk#000000158	0	fully across the slyly bold packages. packages against the quickly regular i	1992
323	40	F	79683.42	1994-03-26	1-URGENT       	Clerk#000000959	0	arefully pending foxes sleep blithely. slyly express accoun	1994
324	106	F	26868.85	1992-03-20	1-URGENT       	Clerk#000000352	0	 about the ironic, regular deposits run blithely against the excuses	1992
325	41	F	71543.41	1993-10-17	5-LOW          	Clerk#000000844	0	ly sometimes pending pa	1993
326	76	O	229165.17	1995-06-04	2-HIGH         	Clerk#000000466	0	 requests. furiously ironic asymptotes mold carefully alongside of the blit	1995
354	139	O	157062.70	1996-03-14	2-HIGH         	Clerk#000000511	0	ly regular ideas wake across the slyly silent ideas. final deposits eat b	1996
355	71	F	69447.25	1994-06-14	5-LOW          	Clerk#000000532	0	s. sometimes regular requests cajole. regular, pending accounts a	1994
356	148	F	162786.67	1994-06-30	4-NOT SPECIFIED	Clerk#000000944	0	as wake along the bold accounts. even, 	1994
358	4	F	226806.66	1993-09-20	2-HIGH         	Clerk#000000392	0	l, silent instructions are slyly. silently even de	1993
384	115	F	122785.82	1992-03-03	5-LOW          	Clerk#000000206	0	, even accounts use furiously packages. slyly ironic pla	1992
387	4	O	130647.18	1997-01-26	4-NOT SPECIFIED	Clerk#000000768	0	 are carefully among the quickly even deposits. furiously silent req	1997
388	46	F	120533.46	1992-12-16	4-NOT SPECIFIED	Clerk#000000356	0	ar foxes above the furiously ironic deposits nag slyly final reque	1992
390	103	O	168562.27	1998-04-07	5-LOW          	Clerk#000000404	0	xpress asymptotes use among the regular, final pinto b	1998
416	41	F	71362.50	1993-09-27	5-LOW          	Clerk#000000294	0	 the accounts. fluffily bold depo	1993
417	55	F	91982.29	1994-02-06	3-MEDIUM       	Clerk#000000468	0	ironic, even packages. thinly unusual accounts sleep along the slyly unusual 	1994
420	91	O	198039.23	1995-10-31	4-NOT SPECIFIED	Clerk#000000756	0	leep carefully final excuses. fluffily pending requests unwind carefully above	1995
421	40	F	1084.38	1992-02-22	5-LOW          	Clerk#000000405	0	egular, even packages according to the final, un	1992
422	74	O	106045.89	1997-05-31	4-NOT SPECIFIED	Clerk#000000049	0	aggle carefully across the accounts. regular accounts eat fluffi	1997
423	104	O	26981.31	1996-06-01	1-URGENT       	Clerk#000000674	0	quests. deposits cajole quickly. furiously bold accounts haggle q	1996
450	49	P	153386.61	1995-03-05	4-NOT SPECIFIED	Clerk#000000293	0	d theodolites. boldly bold foxes since the pack	1995
453	46	O	216826.73	1997-05-26	5-LOW          	Clerk#000000504	0	ss foxes. furiously regular ideas sleep according to t	1997
454	49	O	23198.24	1995-12-27	5-LOW          	Clerk#000000890	0	dolites sleep carefully blithely regular deposits. quickly regul	1995
455	13	O	138010.76	1996-12-04	1-URGENT       	Clerk#000000796	0	 about the final platelets. dependen	1996
480	73	F	20530.97	1993-05-08	5-LOW          	Clerk#000000004	0	ealthy pinto beans. fluffily regular requests along the special sheaves wake 	1993
484	55	O	219920.62	1997-01-03	3-MEDIUM       	Clerk#000000545	0	grouches use. furiously bold accounts maintain. bold, regular deposits	1997
485	101	O	110432.76	1997-03-26	2-HIGH         	Clerk#000000105	0	 regular ideas nag thinly furiously s	1997
486	52	O	185968.15	1996-03-11	4-NOT SPECIFIED	Clerk#000000803	0	riously dolphins. fluffily ironic requ	1996
512	64	P	124661.48	1995-05-20	5-LOW          	Clerk#000000814	0	ding requests. carefully express theodolites was quickly. furious	1995
514	76	O	104585.77	1996-04-04	2-HIGH         	Clerk#000000094	0	 cajole furiously. slyly final excuses cajole. slyly special instructions 	1996
515	142	F	153720.22	1993-08-29	4-NOT SPECIFIED	Clerk#000000700	0	eposits are furiously furiously silent pinto beans. pending pack	1993
517	10	O	82197.79	1997-04-07	5-LOW          	Clerk#000000359	0	slyly pending deposits cajole quickly packages. furiou	1997
519	64	O	95731.50	1997-10-31	1-URGENT       	Clerk#000000985	0	ains doze furiously against the f	1997
544	94	F	47627.89	1993-02-17	2-HIGH         	Clerk#000000145	0	the special, final accounts. dogged dolphins	1993
545	64	O	23476.12	1995-11-07	2-HIGH         	Clerk#000000537	0	as. blithely final hockey players about th	1995
549	110	F	141679.41	1992-07-13	1-URGENT       	Clerk#000000196	0	ideas alongside of 	1992
550	25	O	33123.28	1995-08-02	1-URGENT       	Clerk#000000204	0	t requests. blithely 	1995
551	91	O	46355.83	1995-05-30	1-URGENT       	Clerk#000000179	0	xpress accounts boost quic	1995
577	56	F	34768.68	1994-12-19	5-LOW          	Clerk#000000154	0	 deposits engage stealthil	1994
578	94	O	70392.02	1997-01-10	5-LOW          	Clerk#000000281	0	e blithely even packages. slyly pending platelets bes	1997
579	68	O	120828.12	1998-03-11	2-HIGH         	Clerk#000000862	0	 regular instructions. blithely even p	1998
582	50	O	129004.81	1997-10-21	1-URGENT       	Clerk#000000378	0	n pinto beans print a	1997
583	49	O	127817.38	1997-03-19	3-MEDIUM       	Clerk#000000792	0	efully express requests. a	1997
610	52	O	175142.28	1995-08-02	1-URGENT       	Clerk#000000610	0	totes. ironic, unusual packag	1995
611	106	F	73907.63	1993-01-27	1-URGENT       	Clerk#000000401	0	ounts detect furiously ac	1993
613	139	O	33396.35	1995-06-18	2-HIGH         	Clerk#000000172	0	ts hinder among the deposits. fluffily ironic depos	1995
614	134	F	218116.21	1992-12-01	2-HIGH         	Clerk#000000388	0	 deposits! even, daring theodol	1992
615	67	F	32890.89	1992-05-09	5-LOW          	Clerk#000000388	0	t to promise asymptotes. packages haggle alongside of the fluffil	1992
642	40	F	22994.51	1993-12-16	3-MEDIUM       	Clerk#000000357	0	 among the requests wake slyly alongside of th	1993
645	115	F	234763.73	1994-12-03	2-HIGH         	Clerk#000000090	0	quickly daring theodolites across the regu	1994
646	52	F	142070.65	1994-11-22	2-HIGH         	Clerk#000000203	0	carefully even foxes. fina	1994
673	80	F	21137.08	1994-03-10	1-URGENT       	Clerk#000000448	0	 special pinto beans use quickly furiously even depende	1994
675	13	O	125188.72	1997-07-31	2-HIGH         	Clerk#000000168	0	ffily between the careful	1997
676	38	O	163966.67	1996-12-13	2-HIGH         	Clerk#000000248	0	the final deposits. special, pending	1996
678	131	F	135761.05	1993-02-27	5-LOW          	Clerk#000000530	0	. blithely final somas about the	1993
679	49	O	8945.03	1995-12-15	2-HIGH         	Clerk#000000853	0	tealthy, final pinto beans haggle slyly. pending platelets about the special, 	1995
704	85	O	56210.26	1996-11-21	3-MEDIUM       	Clerk#000000682	0	blithely pending platelets wake alongside of the final, iron	1996
706	148	O	23973.60	1995-09-09	1-URGENT       	Clerk#000000448	0	g the packages. deposits caj	1995
709	37	O	72055.87	1998-04-21	1-URGENT       	Clerk#000000461	0	ons alongside of the carefully bold pinto bea	1998
711	64	F	92484.70	1993-09-23	4-NOT SPECIFIED	Clerk#000000856	0	its. fluffily regular gifts are furi	1993
736	47	O	130204.17	1998-06-21	5-LOW          	Clerk#000000881	0	refully of the final pi	1998
738	22	F	114145.18	1993-03-02	4-NOT SPECIFIED	Clerk#000000669	0	ly even foxes. furiously regular accounts cajole ca	1993
741	106	O	47985.98	1998-07-07	2-HIGH         	Clerk#000000295	0	ic instructions. slyly express instructions solv	1998
742	103	F	207632.55	1994-12-23	5-LOW          	Clerk#000000543	0	equests? slyly ironic dolphins boost carefully above the blithely	1994
768	98	O	220636.82	1996-08-20	3-MEDIUM       	Clerk#000000411	0	jole slyly ironic packages. slyly even idea	1996
769	80	F	43092.76	1993-06-02	3-MEDIUM       	Clerk#000000172	0	ggle furiously. ironic packages haggle slyly. bold platelets affix s	1993
771	46	O	105302.05	1995-06-17	1-URGENT       	Clerk#000000105	0	s. furiously final instructions across the deposit	1995
774	80	O	145857.60	1995-12-04	1-URGENT       	Clerk#000000883	0	tealthily even depths	1995
775	134	F	59455.61	1995-03-18	1-URGENT       	Clerk#000000191	0	kly express requests. fluffily silent accounts poach furiously	1995
800	56	O	87892.38	1998-07-14	2-HIGH         	Clerk#000000213	0	y alongside of the pending packages? final platelets nag fluffily carefu	1998
1185	74	F	47033.21	1992-08-24	5-LOW          	Clerk#000000344	0	 even escapades are. package	1992
803	16	O	27629.66	1997-04-29	5-LOW          	Clerk#000000260	0	ic instructions. even deposits haggle furiously at the deposits-- regular de	1997
804	50	F	94400.43	1993-03-12	3-MEDIUM       	Clerk#000000931	0	s. blithely final foxes are about the packag	1993
806	131	O	26839.16	1996-06-20	2-HIGH         	Clerk#000000240	0	 the ironic packages wake carefully fina	1996
832	29	F	68494.08	1992-04-19	5-LOW          	Clerk#000000495	0	xes. bravely regular packages sleep up the furiously bold accou	1992
833	56	F	49033.69	1994-02-13	3-MEDIUM       	Clerk#000000437	0	ts haggle quickly across the slyl	1994
838	17	O	82918.36	1998-01-29	5-LOW          	Clerk#000000213	0	 slyly around the slyly even	1998
839	28	O	70182.63	1995-08-08	1-URGENT       	Clerk#000000951	0	the carefully even platelets. furiously unusual fo	1995
864	139	O	74710.74	1997-08-17	1-URGENT       	Clerk#000000036	0	ly after the slyly regular deposits. express, regular asymptotes nag ca	1997
865	4	F	70430.54	1993-05-04	3-MEDIUM       	Clerk#000000337	0	. special packages wake after the carefully final accounts. express pinto be	1993
866	40	F	4766.19	1992-11-28	3-MEDIUM       	Clerk#000000718	0	ins after the even, even accounts nod blithel	1992
868	104	F	127345.45	1992-06-09	4-NOT SPECIFIED	Clerk#000000782	0	onic theodolites print carefully. blithely dogge	1992
869	136	O	58932.19	1997-01-12	2-HIGH         	Clerk#000000245	0	ar sheaves are slowly. slyly even attainments boost theodolites. furiously	1997
871	16	O	172861.58	1995-11-15	5-LOW          	Clerk#000000882	0	oss the ironic theodolites.	1995
897	49	P	57697.44	1995-03-20	1-URGENT       	Clerk#000000316	0	 wake quickly against 	1995
898	55	F	101020.75	1993-06-03	2-HIGH         	Clerk#000000611	0	. unusual pinto beans haggle quickly across 	1993
900	46	F	120073.51	1994-10-01	4-NOT SPECIFIED	Clerk#000000060	0	 fluffily express deposits nag furiousl	1994
901	13	O	81826.12	1998-07-21	4-NOT SPECIFIED	Clerk#000000929	0	lyly even foxes are furious, silent requests. requests about the quickly 	1998
902	10	F	37348.62	1994-07-27	4-NOT SPECIFIED	Clerk#000000811	0	yly final requests over the furiously regula	1994
928	67	F	228136.49	1995-03-02	5-LOW          	Clerk#000000450	0	ithely express pinto beans. 	1995
930	131	F	199102.23	1994-12-17	1-URGENT       	Clerk#000000004	0	 accounts nag slyly. ironic, ironic accounts wake blithel	1994
931	103	F	117909.23	1992-12-07	1-URGENT       	Clerk#000000881	0	ss packages haggle furiously express, regular deposits. even, e	1992
932	41	O	40234.50	1997-05-16	2-HIGH         	Clerk#000000218	0	ly express instructions boost furiously reg	1997
934	52	O	17213.59	1996-07-03	1-URGENT       	Clerk#000000229	0	ts integrate carefully. sly, regular deposits af	1996
935	50	O	97733.87	1997-09-24	5-LOW          	Clerk#000000180	0	iously final deposits cajole. blithely even packages 	1997
961	56	P	158893.16	1995-06-04	4-NOT SPECIFIED	Clerk#000000720	0	ons nag furiously among the quickl	1995
962	37	F	98258.73	1994-05-06	5-LOW          	Clerk#000000463	0	ments nag deposits. fluffily ironic a	1994
964	76	O	131146.47	1995-05-20	3-MEDIUM       	Clerk#000000657	0	print blithely ironic, careful theodolit	1995
966	14	O	120516.93	1998-04-30	2-HIGH         	Clerk#000000239	0	special deposits. furious	1998
967	110	F	179287.95	1992-06-21	3-MEDIUM       	Clerk#000000167	0	excuses engage quickly bold dep	1992
992	55	O	133665.12	1997-11-11	3-MEDIUM       	Clerk#000000875	0	ts. regular pinto beans thrash carefully sl	1997
993	80	O	198238.65	1995-09-10	3-MEDIUM       	Clerk#000000894	0	quickly express accounts among the furiously bol	1995
996	71	O	47447.63	1997-12-29	1-URGENT       	Clerk#000000497	0	arefully final packages into the slyly final requests affix blit	1997
1024	4	O	176084.63	1997-12-23	5-LOW          	Clerk#000000903	0	 blithely. even, express theodolites cajole slyly across	1997
1025	103	F	82034.03	1995-05-05	2-HIGH         	Clerk#000000376	0	ross the slyly final pa	1995
1026	73	O	36464.76	1997-06-04	5-LOW          	Clerk#000000223	0	s wake blithely. special acco	1997
1027	128	F	112770.89	1992-06-03	3-MEDIUM       	Clerk#000000241	0	equests cajole. slyly final pinto bean	1992
1029	130	F	47440.91	1994-06-21	2-HIGH         	Clerk#000000700	0	quests sleep. slyly even foxes wake quickly final theodolites. clo	1994
1030	134	F	16346.94	1994-06-15	5-LOW          	Clerk#000000422	0	ully ironic accounts sleep carefully. requests are carefully alongside of the 	1994
1031	4	F	128024.71	1994-09-01	3-MEDIUM       	Clerk#000000448	0	s; ironic theodolites along the carefully ex	1994
1056	28	F	38446.39	1995-02-11	1-URGENT       	Clerk#000000125	0	t, even deposits hang about the slyly special i	1995
1057	76	F	108107.42	1992-02-20	1-URGENT       	Clerk#000000124	0	cuses dazzle carefully careful, ironic pinto beans. carefully even theod	1992
1060	140	F	121994.04	1993-02-21	3-MEDIUM       	Clerk#000000989	0	l platelets sleep quickly slyly special requests. furiously 	1993
1061	103	O	166947.75	1998-05-15	5-LOW          	Clerk#000000576	0	uests sleep at the packages. fur	1998
1062	106	O	39805.04	1997-01-15	1-URGENT       	Clerk#000000152	0	eposits use blithely 	1997
1063	37	F	41392.31	1994-04-02	2-HIGH         	Clerk#000000024	0	deposits nag quickly regular deposits. quickl	1994
1088	148	F	47120.41	1992-05-21	5-LOW          	Clerk#000000347	0	counts are blithely. platelets print. carefully 	1992
1089	49	O	103192.74	1996-05-04	4-NOT SPECIFIED	Clerk#000000226	0	ns haggle ruthlessly. even requests are quickly abov	1996
1093	101	O	79189.58	1997-07-31	4-NOT SPECIFIED	Clerk#000000159	0	 after the carefully ironic requests. carefully ironic packages wake fluffil	1997
1120	140	O	107958.62	1997-11-07	3-MEDIUM       	Clerk#000000319	0	lly special requests. slyly pending platelets are quickly pending requ	1997
1121	29	O	241837.88	1997-01-13	3-MEDIUM       	Clerk#000000541	0	r escapades. deposits above the fluffily bold requests hag	1997
1123	73	O	93259.93	1996-08-03	3-MEDIUM       	Clerk#000000929	0	uriously pending requests. slyly regular instruction	1996
1124	80	O	141858.97	1998-07-30	5-LOW          	Clerk#000000326	0	regular pinto beans along the fluffily silent packages	1998
1125	25	F	80438.38	1994-10-27	2-HIGH         	Clerk#000000510	0	ithely final requests. i	1994
1152	49	F	51775.54	1994-08-14	4-NOT SPECIFIED	Clerk#000000496	0	equests. deposits ab	1994
1154	37	F	192417.85	1992-02-15	1-URGENT       	Clerk#000000268	0	old asymptotes are special requests. blithely even deposits sleep furiously	1992
1158	142	O	31075.51	1996-06-30	2-HIGH         	Clerk#000000549	0	integrate slyly furiously ironic deposit	1996
1186	59	O	82026.18	1996-08-15	4-NOT SPECIFIED	Clerk#000000798	0	ingly regular pinto beans: instructi	1996
1187	134	F	85948.02	1992-11-20	3-MEDIUM       	Clerk#000000047	0	s after the furiously final deposits boost slyly under the	1992
1189	46	F	71017.99	1994-04-09	1-URGENT       	Clerk#000000243	0	f the even accounts. courts print blithely ironic accounts. sile	1994
1190	13	O	31043.39	1997-03-16	5-LOW          	Clerk#000000575	0	ccounts above the foxes integrate carefully after the 	1997
1216	122	F	68056.57	1992-12-07	5-LOW          	Clerk#000000918	0	nal foxes around the e	1992
1218	10	F	99834.47	1994-06-20	4-NOT SPECIFIED	Clerk#000000994	0	s cajole. special, silent deposits about the theo	1994
1219	28	O	10163.56	1995-10-05	3-MEDIUM       	Clerk#000000800	0	od carefully. slyly final dependencies across the even fray	1995
1220	49	O	122157.14	1996-08-29	1-URGENT       	Clerk#000000712	0	inal theodolites wake. fluffily ironic asymptotes cajol	1996
1221	14	F	117397.16	1992-04-19	4-NOT SPECIFIED	Clerk#000000852	0	 detect against the silent, even deposits. carefully ironic	1992
1222	10	F	47623.94	1993-02-05	3-MEDIUM       	Clerk#000000811	0	theodolites use quickly even accounts. carefully final asympto	1993
1223	10	O	26714.67	1996-05-25	4-NOT SPECIFIED	Clerk#000000238	0	posits was blithely fr	1996
1248	49	F	210713.88	1992-01-02	1-URGENT       	Clerk#000000890	0	t the carefully regular dugouts. s	1992
1250	37	F	12907.62	1992-09-29	4-NOT SPECIFIED	Clerk#000000652	0	ts after the fluffily pending instructions use slyly about the s	1992
1251	38	O	109536.55	1997-10-30	1-URGENT       	Clerk#000000276	0	, brave sauternes. deposits boost fluffily.	1997
1253	115	F	92730.74	1993-01-26	1-URGENT       	Clerk#000000775	0	 requests sleep furiously even foxes. ruthless packag	1993
1255	122	F	62518.31	1994-05-30	4-NOT SPECIFIED	Clerk#000000798	0	ct slyly regular accounts. quick	1994
1281	62	F	165454.51	1994-12-11	1-URGENT       	Clerk#000000430	0	counts. carefully pending accounts eat 	1994
1284	134	O	106122.38	1996-01-07	2-HIGH         	Clerk#000000492	0	s. blithely silent deposits s	1996
1313	148	F	46598.65	1994-09-13	1-URGENT       	Clerk#000000774	0	ld accounts. regular deposits cajole. ironically pending theodolites use car	1994
1315	22	O	121935.23	1998-03-22	5-LOW          	Clerk#000000840	0	final theodolites alongside of the carefu	1998
1316	16	F	163746.47	1993-12-03	1-URGENT       	Clerk#000000857	0	ully bold theodolites? pending, bold pin	1993
1318	128	O	81663.65	1998-06-27	3-MEDIUM       	Clerk#000000581	0	s hang bold requests. pending, re	1998
1344	17	F	43809.37	1992-04-16	5-LOW          	Clerk#000000178	0	omise close, silent requests. pending theodolites boost pending 	1992
1346	76	F	171975.62	1992-06-18	2-HIGH         	Clerk#000000374	0	ges sleep quickly-- even pint	1992
1347	41	O	173444.60	1997-06-20	5-LOW          	Clerk#000000977	0	he furiously even foxes use carefully express req	1997
1349	64	O	46376.09	1997-10-26	1-URGENT       	Clerk#000000543	0	yly! blithely special theodolites cajole. unusual, reg	1997
1350	52	F	49305.98	1993-08-24	1-URGENT       	Clerk#000000635	0	iously about the blithely special a	1993
1351	106	O	24637.96	1998-04-20	1-URGENT       	Clerk#000000012	0	 cajole. regular, special re	1998
1376	47	O	23984.88	1997-05-04	4-NOT SPECIFIED	Clerk#000000730	0	der furiously final, final frets. carefull	1997
1408	55	O	183965.61	1997-12-26	4-NOT SPECIFIED	Clerk#000000942	0	t the quickly final asymptotes. unusual	1997
1410	113	O	114879.19	1997-04-12	5-LOW          	Clerk#000000123	0	iously along the bravely regular dolphins. pinto beans cajole furiously sp	1997
1413	91	O	75733.58	1997-06-14	3-MEDIUM       	Clerk#000000342	0	, ironic instructions. carefully even packages dazzle	1997
1440	98	O	50201.16	1995-08-10	5-LOW          	Clerk#000000956	0	 pending requests. closely s	1995
1441	122	O	156477.94	1997-03-06	4-NOT SPECIFIED	Clerk#000000156	0	ter the excuses. ironic dependencies m	1997
1444	134	F	207907.60	1994-12-06	3-MEDIUM       	Clerk#000000783	0	ove the bold accounts cajole fluffily about	1994
1445	115	F	154653.32	1995-01-10	3-MEDIUM       	Clerk#000000211	0	even packages wake fluffily 	1995
1446	41	O	27663.16	1998-02-16	5-LOW          	Clerk#000000274	0	lly regular notornis above the requests sleep final accounts! 	1998
1447	91	F	108171.38	1992-10-15	2-HIGH         	Clerk#000000880	0	inly against the blithely pending excuses. regular, pe	1992
1473	94	O	80624.38	1997-03-17	3-MEDIUM       	Clerk#000000960	0	furiously close accoun	1997
1475	5	O	185496.66	1997-11-12	2-HIGH         	Clerk#000000972	0	cally final packages boost. blithely ironic packa	1997
1477	76	O	231831.35	1997-08-24	5-LOW          	Clerk#000000612	0	ly bold foxes. final ideas would cajo	1997
1478	50	O	20791.50	1997-08-03	2-HIGH         	Clerk#000000827	0	lessly. carefully express	1997
1479	16	O	31471.04	1995-12-16	4-NOT SPECIFIED	Clerk#000000697	0	he furiously even foxes. thinly bold deposits	1995
1505	37	F	55892.35	1992-08-21	2-HIGH         	Clerk#000000544	0	s. slyly ironic packages cajole. carefully regular packages haggle 	1992
1506	148	F	195844.84	1992-09-21	3-MEDIUM       	Clerk#000000620	0	 dependencies. accounts affix blithely slowly unusual deposits. slyly regular 	1992
1508	103	O	151282.65	1998-04-10	5-LOW          	Clerk#000000117	0	 after the furiously regular pinto beans hang slyly quickly ironi	1998
1509	64	F	180455.98	1993-07-08	5-LOW          	Clerk#000000770	0	the regular ideas. regul	1993
1536	94	O	5184.26	1997-01-26	3-MEDIUM       	Clerk#000000117	0	ges are! furiously final deposits cajole iron	1997
1538	29	O	179554.41	1995-06-18	4-NOT SPECIFIED	Clerk#000000258	0	 instructions. regular theod	1995
1540	16	F	128014.15	1992-08-05	2-HIGH         	Clerk#000000927	0	r ideas hinder blithe	1992
1541	94	P	47286.32	1995-05-18	1-URGENT       	Clerk#000000906	0	y. slyly ironic warhorses around the furiously regul	1995
1543	52	O	139047.22	1997-02-20	1-URGENT       	Clerk#000000398	0	unts. furiously pend	1997
1568	17	O	76119.72	1997-01-30	4-NOT SPECIFIED	Clerk#000000554	0	d notornis. carefully 	1997
1569	104	O	87803.55	1998-04-02	5-LOW          	Clerk#000000786	0	orbits. fluffily even decoys serve blithely. furiously furious realms nag acro	1998
1571	103	F	151404.78	1992-12-05	2-HIGH         	Clerk#000000565	0	ously furiously bold warthogs. slyly ironic instructions are quickly a	1992
1573	148	F	86918.57	1992-12-28	2-HIGH         	Clerk#000000940	0	ess, ironic deposits use along the carefu	1992
1574	134	O	179923.54	1996-12-12	3-MEDIUM       	Clerk#000000809	0	 ideas hinder after the carefully unusual 	1996
1600	94	F	130515.61	1993-03-03	3-MEDIUM       	Clerk#000000627	0	tions cajole quietly above the regular, silent requests. slyly fin	1993
1604	113	F	107139.29	1993-07-17	5-LOW          	Clerk#000000512	0	lithely silent waters. blithely unusual packages alongside 	1993
1632	67	O	183286.33	1997-01-08	3-MEDIUM       	Clerk#000000351	0	onic requests are accounts. bold a	1997
1633	16	O	52359.51	1995-10-14	2-HIGH         	Clerk#000000666	0	y silent accounts sl	1995
1635	4	O	70232.26	1997-02-13	3-MEDIUM       	Clerk#000000958	0	s. slyly ironic requests affix slyly 	1997
1637	73	F	180912.15	1995-02-08	4-NOT SPECIFIED	Clerk#000000189	0	 final accounts. blithely silent ideas cajole bravely. carefully express 	1995
1638	139	O	172436.30	1997-08-13	2-HIGH         	Clerk#000000643	0	he fluffily regular asymp	1997
1639	5	O	104166.56	1995-08-20	4-NOT SPECIFIED	Clerk#000000939	0	haggle furiously. final requests detect furious	1995
1664	64	O	178060.22	1996-03-03	1-URGENT       	Clerk#000000090	0	y quickly even asymptotes. furiously regular packages haggle quickly fin	1996
1665	76	F	4819.91	1994-05-08	2-HIGH         	Clerk#000000920	0	ly regular packages are fluffily even ideas. fluffily final	1994
1667	5	O	125030.37	1997-10-10	2-HIGH         	Clerk#000000103	0	e accounts. slyly express accounts must are a	1997
1668	142	O	137576.19	1997-07-12	4-NOT SPECIFIED	Clerk#000000148	0	eodolites. carefully dogged dolphins haggle q	1997
1670	25	O	89999.72	1997-05-24	2-HIGH         	Clerk#000000320	0	unusual dependencies. furiously special platelets main	1997
1696	4	O	102665.03	1998-01-08	4-NOT SPECIFIED	Clerk#000000041	0	bravely bold accounts above the quickly bold	1998
1697	76	O	122621.31	1996-10-07	1-URGENT       	Clerk#000000815	0	o x-ray blithely. pl	1996
1698	40	O	141118.87	1997-04-23	2-HIGH         	Clerk#000000432	0	slyly. carefully express deposit	1997
1699	85	F	66408.29	1993-12-30	1-URGENT       	Clerk#000000125	0	jole blithely. furiously un	1993
1701	130	F	72835.95	1992-05-19	2-HIGH         	Clerk#000000395	0	furiously. regular, close theodoli	1992
1702	67	P	194119.31	1995-05-07	2-HIGH         	Clerk#000000300	0	around the carefully final deposits cajole carefully according to the b	1995
1703	134	F	121220.59	1993-01-28	3-MEDIUM       	Clerk#000000463	0	 pinto beans poach. bold courts boost. regular, express deposits at	1993
1728	64	O	131604.34	1996-05-22	2-HIGH         	Clerk#000000711	0	beans. slyly regular instructions sleep! slyly final packages	1996
1731	128	O	190490.78	1996-01-06	1-URGENT       	Clerk#000000268	0	lithely regular, final instructions. ironic, express packages are above	1996
1732	146	F	179854.51	1993-11-29	5-LOW          	Clerk#000000903	0	inal requests integrate dolph	1993
1733	148	O	165489.52	1996-05-12	2-HIGH         	Clerk#000000789	0	e carefully according to the accounts. furiously pending instructions sleep	1996
1735	22	F	98541.95	1992-12-27	1-URGENT       	Clerk#000000458	0	ully idle requests wake qu	1992
1760	115	O	82151.12	1996-05-17	5-LOW          	Clerk#000000917	0	 deposits. busily regular deposits wake blithely along the furiously even re	1996
1761	106	F	211925.95	1993-12-24	2-HIGH         	Clerk#000000817	0	efully slyly bold frets. packages boost b	1993
1764	29	F	47384.71	1992-03-25	1-URGENT       	Clerk#000000182	0	. slyly final packages integrate carefully acro	1992
1765	73	O	36551.43	1995-12-03	4-NOT SPECIFIED	Clerk#000000490	0	 regular excuses wake slyly	1995
1766	139	O	41032.81	1996-10-12	2-HIGH         	Clerk#000000983	0	unusual deposits affix quickly beyond the carefully s	1996
1767	25	P	136582.60	1995-03-14	2-HIGH         	Clerk#000000327	0	eposits use carefully carefully regular platelets. quickly regular packages al	1995
1792	49	F	107919.86	1993-11-09	5-LOW          	Clerk#000000102	0	ructions haggle along the pending packages. carefully speci	1993
1794	140	O	179462.21	1997-09-28	1-URGENT       	Clerk#000000686	0	ally silent pinto beans. regular package	1997
1795	94	F	146849.33	1994-03-19	2-HIGH         	Clerk#000000815	0	 quickly final packages! blithely dogged accounts c	1994
1796	47	F	33755.47	1992-11-21	2-HIGH         	Clerk#000000245	0	eans use furiously around th	1992
1798	52	O	46393.97	1997-07-28	1-URGENT       	Clerk#000000741	0	al foxes are blithe	1997
1824	49	F	81351.53	1994-05-05	1-URGENT       	Clerk#000000972	0	e blithely fluffily	1994
1825	148	F	150582.77	1993-12-05	3-MEDIUM       	Clerk#000000345	0	ironic, final accou	1993
1827	106	O	210113.88	1996-06-22	4-NOT SPECIFIED	Clerk#000000369	0	luffily even requests haggle sly	1996
1831	71	F	58032.77	1993-12-02	1-URGENT       	Clerk#000000854	0	 accounts. carefully even accounts boost furiously. regular ideas engage. 	1993
1856	106	F	189361.42	1992-03-20	4-NOT SPECIFIED	Clerk#000000952	0	. special pinto beans run acr	1992
1860	10	O	9103.40	1996-04-04	3-MEDIUM       	Clerk#000000556	0	osits. quickly bold deposits according to 	1996
1863	74	F	96359.65	1993-09-23	4-NOT SPECIFIED	Clerk#000000658	0	old sentiments. careful, 	1993
1889	25	O	96431.77	1997-03-16	1-URGENT       	Clerk#000000854	0	p around the regular notornis. unusual deposits	1997
1890	10	O	202364.58	1996-12-18	4-NOT SPECIFIED	Clerk#000000627	0	romise final, regular deposits. regular fox	1996
1892	25	F	133273.64	1994-03-26	5-LOW          	Clerk#000000733	0	sts. slyly regular dependencies use slyly. ironic, spec	1994
1894	76	F	44387.23	1992-03-30	1-URGENT       	Clerk#000000626	0	e furiously. furiously even accounts are slyly final accounts. closely speci	1992
1920	110	O	119605.91	1998-06-24	5-LOW          	Clerk#000000018	0	hely; furiously regular excuses	1998
1922	56	O	11575.77	1996-07-13	3-MEDIUM       	Clerk#000000984	0	side of the blithely final re	1996
1923	136	O	171128.10	1997-07-07	1-URGENT       	Clerk#000000471	0	 express dolphins. 	1997
1924	76	O	169756.19	1996-09-07	4-NOT SPECIFIED	Clerk#000000823	0	 of the ironic accounts. instructions near the final instr	1996
1925	17	F	146382.71	1992-03-05	1-URGENT       	Clerk#000000986	0	e slyly regular deposits. furiously 	1992
1926	94	O	100035.03	1996-01-31	2-HIGH         	Clerk#000000568	0	cajole. even warhorses sleep carefully. 	1996
1927	140	O	23327.88	1995-09-30	3-MEDIUM       	Clerk#000000616	0	riously special packages. permanent pearls wake furiously. even packages alo	1995
1952	67	F	12896.25	1994-03-16	2-HIGH         	Clerk#000000254	0	 silent accounts boost 	1994
1954	56	O	158853.63	1997-05-31	4-NOT SPECIFIED	Clerk#000000104	0	 unusual excuses cajole according to the blithely regular theodolites.	1997
1955	13	F	103085.13	1992-04-20	1-URGENT       	Clerk#000000792	0	ly special ideas. sometimes final 	1992
1984	52	O	79230.47	1998-04-01	1-URGENT       	Clerk#000000416	0	 slyly special instructions. unusual foxes use packages. carefully regular req	1998
1990	119	F	48781.39	1994-12-16	2-HIGH         	Clerk#000000114	0	e bold patterns. always regul	1994
2017	101	O	70529.27	1998-05-13	3-MEDIUM       	Clerk#000000427	0	nusual requests. blit	1998
2019	136	F	43789.14	1992-10-23	1-URGENT       	Clerk#000000565	0	 furiously bold packages. fluffily fi	1992
2020	73	F	136162.13	1993-06-21	3-MEDIUM       	Clerk#000000192	0	es. furiously regular packages above the furiously special theodolites are a	1993
2022	62	F	206742.11	1992-03-15	1-URGENT       	Clerk#000000268	0	 dependencies sleep fluffily even, ironic deposits. express, silen	1992
2048	17	F	33401.77	1993-11-15	1-URGENT       	Clerk#000000934	0	s cajole after the blithely final accounts. f	1993
2050	28	F	208517.98	1994-06-02	4-NOT SPECIFIED	Clerk#000000821	0	d accounts against the furiously regular packages use bli	1994
2051	40	O	87988.34	1996-03-18	4-NOT SPECIFIED	Clerk#000000333	0	ctions sleep blithely. blithely regu	1996
2052	91	F	141822.19	1992-04-13	2-HIGH         	Clerk#000000767	0	 requests sleep around the even, even courts. ironic theodolites affix furious	1992
2053	142	F	125125.57	1995-02-07	1-URGENT       	Clerk#000000717	0	ar requests: blithely sly accounts boost carefully across t	1995
2054	41	F	144335.16	1992-06-08	4-NOT SPECIFIED	Clerk#000000103	0	l requests affix carefully about the furiously special	1992
2082	49	F	46753.63	1995-01-10	2-HIGH         	Clerk#000000354	0	cial accounts. ironic, express dolphins nod slyly sometimes final reques	1995
2083	101	F	31795.52	1993-07-14	3-MEDIUM       	Clerk#000000361	0	al patterns. bold, final foxes nag bravely about the furiously express	1993
2084	80	F	190652.53	1993-03-17	2-HIGH         	Clerk#000000048	0	zle furiously final, careful packages. slyly ironic ideas amo	1993
2085	49	F	45311.07	1993-11-21	3-MEDIUM       	Clerk#000000818	0	ress, express ideas haggle	1993
2086	142	F	188985.18	1994-10-19	1-URGENT       	Clerk#000000046	0	 permanently regular	1994
2087	50	O	53581.41	1998-01-31	2-HIGH         	Clerk#000000626	0	e always regular packages nod against the furiously spec	1998
2112	64	O	17986.15	1997-02-05	2-HIGH         	Clerk#000000351	0	against the slyly even id	1997
2115	106	O	134814.65	1998-05-23	4-NOT SPECIFIED	Clerk#000000101	0	odolites boost. carefully regular excuses cajole. quickly ironic pinto be	1998
2116	23	F	60887.90	1994-08-26	1-URGENT       	Clerk#000000197	0	efully after the asymptotes. furiously sp	1994
2117	22	O	145713.03	1997-04-26	2-HIGH         	Clerk#000000887	0	ely even dependencies. regular foxes use blithely.	1997
2118	134	O	38974.67	1996-10-09	1-URGENT       	Clerk#000000196	0	ial requests wake carefully special packages. f	1996
2119	64	O	34632.57	1996-08-20	2-HIGH         	Clerk#000000434	0	uickly pending escapades. fluffily ir	1996
2144	136	F	119917.28	1994-03-29	3-MEDIUM       	Clerk#000000546	0	t. carefully quick requests across the deposits wake regu	1994
2145	134	F	18885.35	1992-10-03	1-URGENT       	Clerk#000000886	0	sts would snooze blithely alongside of th	1992
2148	130	F	19612.03	1995-04-19	4-NOT SPECIFIED	Clerk#000000517	0	ross the furiously unusual theodolites. always expre	1995
2149	101	F	105145.40	1993-03-13	5-LOW          	Clerk#000000555	0	nusual accounts nag furiously special reques	1993
2176	104	F	87248.17	1992-11-10	1-URGENT       	Clerk#000000195	0	s haggle regularly accor	1992
2177	136	O	183493.42	1997-01-20	3-MEDIUM       	Clerk#000000161	0	ove the blithely unusual packages cajole carefully fluffily special request	1997
2179	41	O	77487.09	1996-09-07	2-HIGH         	Clerk#000000935	0	ounts alongside of the furiously unusual braids cajol	1996
2180	76	O	208481.57	1996-09-14	4-NOT SPECIFIED	Clerk#000000650	0	xpress, unusual pains. furiously ironic excu	1996
2181	76	O	100954.64	1995-09-13	3-MEDIUM       	Clerk#000000814	0	y against the ironic, even	1995
2182	23	F	116003.11	1994-04-05	2-HIGH         	Clerk#000000071	0	ccounts. quickly bold deposits across the excuses sl	1994
2183	113	O	49841.12	1996-06-22	1-URGENT       	Clerk#000000287	0	 among the express, ironic packages. slyly ironic platelets integrat	1996
2208	68	P	245388.06	1995-05-01	4-NOT SPECIFIED	Clerk#000000900	0	symptotes wake slyly blithely unusual packages.	1995
2209	91	F	129086.93	1992-07-10	2-HIGH         	Clerk#000000056	0	er above the slyly silent requests. furiously reg	1992
2211	92	F	140031.23	1994-06-30	2-HIGH         	Clerk#000000464	0	ffily bold courts e	1994
2213	122	F	146136.10	1993-01-15	4-NOT SPECIFIED	Clerk#000000598	0	osits are carefully reg	1993
2214	115	O	150345.63	1998-05-05	3-MEDIUM       	Clerk#000000253	0	packages. fluffily even accounts haggle blithely. carefully ironic depen	1998
2215	40	O	108239.46	1996-06-16	4-NOT SPECIFIED	Clerk#000000817	0	le final, final foxes. quickly regular gifts are carefully deposit	1996
2240	56	F	174090.30	1992-03-06	4-NOT SPECIFIED	Clerk#000000622	0	accounts against the slyly express foxes are after the slyly regular 	1992
2241	103	F	165219.08	1993-05-11	1-URGENT       	Clerk#000000081	0	y about the silent excuses. furiously ironic instructions along the sil	1993
2243	49	O	10451.97	1995-06-10	2-HIGH         	Clerk#000000813	0	ously regular deposits integrate s	1995
2246	113	O	85755.84	1996-05-27	4-NOT SPECIFIED	Clerk#000000739	0	 final gifts sleep 	1996
2272	139	F	127934.71	1993-04-13	2-HIGH         	Clerk#000000449	0	s. bold, ironic pinto beans wake. silently specia	1993
2273	136	O	142291.79	1996-12-14	5-LOW          	Clerk#000000155	0	uickly express foxes haggle quickly against	1996
2274	104	F	58273.89	1993-09-04	4-NOT SPECIFIED	Clerk#000000258	0	nstructions try to hag	1993
2278	142	O	101878.46	1998-04-25	3-MEDIUM       	Clerk#000000186	0	r pinto beans integrate after the carefully even deposits. blit	1998
2279	80	F	142322.33	1993-02-23	3-MEDIUM       	Clerk#000000898	0	de of the quickly unusual instructio	1993
2304	46	F	93769.28	1994-01-07	4-NOT SPECIFIED	Clerk#000000415	0	onic platelets. ironic packages haggle. packages nag doggedly according to	1994
2306	28	O	244704.23	1995-07-26	2-HIGH         	Clerk#000000975	0	 wake furiously requests. permanent requests affix. final packages caj	1995
2307	106	F	59417.76	1993-06-29	5-LOW          	Clerk#000000952	0	furiously even asymptotes? carefully regular accounts	1993
2308	25	F	58546.02	1992-10-25	4-NOT SPECIFIED	Clerk#000000609	0	ts. slyly final depo	1992
2311	73	P	153233.93	1995-05-02	2-HIGH         	Clerk#000000761	0	ly pending asymptotes-- furiously bold excus	1995
2336	142	O	22294.51	1996-01-07	4-NOT SPECIFIED	Clerk#000000902	0	c, final excuses sleep furiously among the even theodolites. f	1996
2337	142	O	45704.96	1997-06-18	4-NOT SPECIFIED	Clerk#000000754	0	 quickly. final accounts haggle. carefully final acco	1997
2338	140	O	28155.92	1997-09-15	2-HIGH         	Clerk#000000951	0	riously final dugouts. final, ironic packages wake express, ironic id	1997
2342	37	O	104038.78	1996-06-09	1-URGENT       	Clerk#000000615	0	oost carefully across the regular accounts. blithely final d	1996
2343	73	O	85381.00	1995-08-21	3-MEDIUM       	Clerk#000000170	0	fluffily over the slyly special deposits. quickl	1995
2368	13	F	101240.96	1993-08-20	1-URGENT       	Clerk#000000830	0	t the bold instructions. carefully unusual 	1993
2369	110	O	73517.91	1996-12-24	2-HIGH         	Clerk#000000752	0	iously even requests are dogged, express 	1996
2370	142	F	73924.21	1994-01-17	1-URGENT       	Clerk#000000231	0	lyly final packages. quickly final deposits haggl	1994
2373	28	F	55211.04	1994-03-12	4-NOT SPECIFIED	Clerk#000000306	0	 even, special courts grow quickly. pending,	1994
2374	4	F	115219.88	1993-10-29	4-NOT SPECIFIED	Clerk#000000081	0	 blithely regular packages. blithely unusua	1993
2375	5	O	106612.48	1996-11-20	3-MEDIUM       	Clerk#000000197	0	unusual, pending theodolites cajole carefully 	1996
2400	37	O	92798.66	1998-07-25	5-LOW          	Clerk#000000782	0	nusual courts nag against the carefully unusual pinto b	1998
2401	148	O	88448.24	1997-07-29	4-NOT SPECIFIED	Clerk#000000531	0	ully unusual instructions boost carefully silently regular requests. 	1997
2402	67	O	70403.62	1996-09-06	4-NOT SPECIFIED	Clerk#000000162	0	slyly final sheaves sleep slyly. q	1996
2403	55	O	111020.79	1998-04-11	3-MEDIUM       	Clerk#000000820	0	furiously regular deposits use. furiously unusual accounts wake along the 	1998
2405	73	O	115929.14	1996-12-23	3-MEDIUM       	Clerk#000000535	0	ular, regular asympto	1996
2407	55	O	112843.52	1998-06-19	2-HIGH         	Clerk#000000068	0	uests affix slyly among the slyly regular depos	1998
2432	103	O	62661.93	1996-07-13	1-URGENT       	Clerk#000000115	0	re. slyly even deposits wake bra	1996
2434	25	O	123956.25	1997-04-27	3-MEDIUM       	Clerk#000000190	0	s. quickly ironic dolphins impress final deposits. blithel	1997
2435	73	F	122490.66	1993-02-21	5-LOW          	Clerk#000000112	0	es are carefully along the carefully final instructions. pe	1993
2661	74	O	106036.84	1997-01-04	3-MEDIUM       	Clerk#000000217	0	al, regular pinto beans. silently final deposits should have t	1997
2437	85	F	143411.69	1993-04-21	4-NOT SPECIFIED	Clerk#000000578	0	. theodolites wake slyly-- ironic, pending platelets above the carefully exp	1993
2438	13	F	214494.39	1993-07-15	2-HIGH         	Clerk#000000744	0	the final, regular warhorses. regularly 	1993
2439	55	O	41811.12	1997-03-15	2-HIGH         	Clerk#000000819	0	lithely after the car	1997
2496	136	F	140390.60	1994-01-09	2-HIGH         	Clerk#000000142	0	slyly. pending instructions sleep. quic	1994
2497	47	F	171326.48	1992-08-27	1-URGENT       	Clerk#000000977	0	ily ironic pinto beans. furiously final platelets alongside of t	1992
2501	67	O	79380.51	1997-05-25	5-LOW          	Clerk#000000144	0	ickly special theodolite	1997
2528	55	F	92069.62	1994-11-20	1-URGENT       	Clerk#000000789	0	ular dependencies? regular frays kindle according to the blith	1994
2529	136	O	4104.30	1996-08-20	2-HIGH         	Clerk#000000511	0	posits across the silent instructions wake blithely across 	1996
2530	128	F	58853.11	1994-03-21	3-MEDIUM       	Clerk#000000291	0	ular instructions about the quic	1994
2532	94	O	116093.49	1995-10-11	2-HIGH         	Clerk#000000498	0	the blithely pending accounts. regular, regular excuses boost aro	1995
2533	50	O	168495.03	1997-03-24	1-URGENT       	Clerk#000000594	0	ecial instructions. spec	1997
2534	76	O	202784.54	1996-07-17	3-MEDIUM       	Clerk#000000332	0	packages cajole ironic requests. furiously regular	1996
2560	131	F	153426.79	1992-09-05	1-URGENT       	Clerk#000000538	0	atelets; quickly sly requests	1992
2562	10	F	136360.37	1992-08-01	1-URGENT       	Clerk#000000467	0	elets. pending dolphins promise slyly. bo	1992
2563	62	F	168952.10	1993-11-19	4-NOT SPECIFIED	Clerk#000000150	0	sly even packages after the furio	1993
2565	56	O	204438.57	1998-02-28	3-MEDIUM       	Clerk#000000032	0	x-ray blithely along	1998
2566	86	F	89992.48	1992-10-10	3-MEDIUM       	Clerk#000000414	0	ructions boost bold ideas. idly ironic accounts use according to th	1992
2592	101	F	8225.96	1993-03-05	4-NOT SPECIFIED	Clerk#000000524	0	ts nag fluffily. quickly stealthy theodolite	1993
2593	92	F	134726.09	1993-09-04	2-HIGH         	Clerk#000000468	0	r the carefully final	1993
2595	74	O	173130.20	1995-12-14	4-NOT SPECIFIED	Clerk#000000222	0	arefully ironic requests nag carefully ideas. 	1995
2597	104	F	21964.66	1993-02-04	2-HIGH         	Clerk#000000757	0	iously ruthless exc	1993
2624	52	O	27148.63	1996-11-28	5-LOW          	Clerk#000000930	0	ic, regular packages	1996
2625	40	F	39382.74	1992-10-14	4-NOT SPECIFIED	Clerk#000000386	0	 final deposits. blithely ironic ideas 	1992
2626	139	O	84314.51	1995-09-08	4-NOT SPECIFIED	Clerk#000000289	0	gside of the carefully special packages are furiously after the slyly express 	1995
2628	56	F	165655.99	1993-10-22	5-LOW          	Clerk#000000836	0	ajole across the blithely careful accounts. blithely silent deposits sl	1993
2629	139	O	96458.03	1998-04-06	5-LOW          	Clerk#000000680	0	uches dazzle carefully even, express excuses. ac	1998
2630	85	F	127132.51	1992-10-24	5-LOW          	Clerk#000000712	0	inal theodolites. ironic instructions s	1992
2631	37	F	63103.32	1993-09-24	5-LOW          	Clerk#000000833	0	 quickly unusual deposits doubt around 	1993
2657	25	O	148176.06	1995-10-17	2-HIGH         	Clerk#000000160	0	 foxes-- slyly final dependencies around the slyly final theodo	1995
2658	14	O	163834.46	1995-09-23	3-MEDIUM       	Clerk#000000400	0	bout the slyly regular accounts. ironic, 	1995
2662	37	O	87689.88	1996-08-21	3-MEDIUM       	Clerk#000000589	0	bold pinto beans above the slyly final accounts affix furiously deposits. pac	1996
2688	98	F	181077.36	1992-01-24	2-HIGH         	Clerk#000000720	0	have to nag according to the pending theodolites. sly	1992
2689	103	F	41552.78	1992-04-09	4-NOT SPECIFIED	Clerk#000000698	0	press pains wake. furiously express theodolites alongsid	1992
2690	94	O	224674.27	1996-03-31	3-MEDIUM       	Clerk#000000760	0	ravely even theodolites 	1996
2692	62	O	24265.24	1997-12-02	3-MEDIUM       	Clerk#000000878	0	es. regular asymptotes cajole above t	1997
2727	74	O	3089.42	1998-04-19	4-NOT SPECIFIED	Clerk#000000879	0	sual theodolites cajole enticingly above the furiously fin	1998
5088	130	F	101616.44	1993-01-06	5-LOW          	Clerk#000000930	0	ole slyly since the quickly ironic br	1993
2752	59	F	187932.30	1993-11-19	2-HIGH         	Clerk#000000648	0	 carefully regular foxes are quickly quickl	1993
2753	16	F	159720.39	1993-11-30	2-HIGH         	Clerk#000000380	0	ending instructions. unusual deposits	1993
2757	76	O	89792.48	1995-07-20	2-HIGH         	Clerk#000000216	0	 regular requests subl	1995
2785	148	O	132854.79	1995-07-21	2-HIGH         	Clerk#000000098	0	iously pending packages sleep according to the blithely unusual foxe	1995
2787	103	O	3726.14	1995-09-30	1-URGENT       	Clerk#000000906	0	he ironic, regular 	1995
2789	37	O	219123.27	1998-03-14	2-HIGH         	Clerk#000000972	0	gular patterns boost. carefully even re	1998
2790	25	F	177458.97	1994-08-19	2-HIGH         	Clerk#000000679	0	 the carefully express deposits sleep slyly 	1994
2817	40	F	71453.85	1994-04-19	3-MEDIUM       	Clerk#000000982	0	ic foxes haggle upon the daringly even pinto beans. slyly	1994
2818	49	F	120086.84	1994-12-12	3-MEDIUM       	Clerk#000000413	0	eep furiously special ideas. express 	1994
2819	103	F	66927.16	1994-05-05	1-URGENT       	Clerk#000000769	0	ngside of the blithely ironic dolphins. furio	1994
2849	46	O	180054.29	1996-04-30	2-HIGH         	Clerk#000000659	0	al packages are after the quickly bold requests. carefully special 	1996
2852	91	F	99050.81	1993-01-16	1-URGENT       	Clerk#000000740	0	ruthless deposits against the final instructions use quickly al	1993
2853	94	F	103641.15	1994-05-05	2-HIGH         	Clerk#000000878	0	the carefully even packages.	1994
2854	139	F	153568.02	1994-06-27	1-URGENT       	Clerk#000000010	0	 furiously ironic tithes use furiously 	1994
2855	49	F	48419.58	1993-04-04	4-NOT SPECIFIED	Clerk#000000973	0	 silent, regular packages sleep 	1993
2884	92	O	71683.84	1997-10-12	3-MEDIUM       	Clerk#000000780	0	efully express instructions sleep against	1997
2912	94	F	27727.52	1992-03-12	5-LOW          	Clerk#000000186	0	jole blithely above the quickly regular packages. carefully regular pinto bean	1992
2915	94	F	96015.13	1994-03-31	5-LOW          	Clerk#000000410	0	ld packages. bold deposits boost blithely. ironic, unusual theodoli	1994
2917	91	O	100714.13	1997-12-09	4-NOT SPECIFIED	Clerk#000000061	0	 special dugouts among the special deposi	1997
2944	14	O	146581.14	1997-09-24	4-NOT SPECIFIED	Clerk#000000740	0	deas. permanently special foxes haggle carefully ab	1997
2945	29	O	223507.72	1996-01-03	2-HIGH         	Clerk#000000499	0	ons are carefully toward the permanent, bold pinto beans. regu	1996
2950	136	O	183620.33	1997-07-06	1-URGENT       	Clerk#000000833	0	 dolphins around the furiously 	1997
2951	74	O	125509.17	1996-02-06	2-HIGH         	Clerk#000000680	0	gular deposits above the finally regular ideas integrate idly stealthil	1996
2976	29	F	145768.47	1993-12-10	4-NOT SPECIFIED	Clerk#000000159	0	. furiously ironic asymptotes haggle ruthlessly silently regular r	1993
2977	73	O	25170.88	1996-08-27	3-MEDIUM       	Clerk#000000252	0	quickly special platelets are furio	1996
2980	4	O	187514.11	1996-09-14	3-MEDIUM       	Clerk#000000661	0	y quick pinto beans wake. slyly re	1996
2981	49	O	37776.79	1998-07-29	5-LOW          	Clerk#000000299	0	hely among the express foxes. blithely stealthy requests cajole boldly. regu	1998
2982	85	F	55582.94	1995-03-19	2-HIGH         	Clerk#000000402	0	lyly. express theodolites affix slyly after the slyly speci	1995
2983	62	F	58168.07	1992-01-07	1-URGENT       	Clerk#000000278	0	r the even requests. accounts maintain. regular accounts	1992
3008	40	O	156018.74	1995-11-08	3-MEDIUM       	Clerk#000000701	0	ze quickly. blithely regular packages above the slyly bold foxes shall	1995
3009	55	O	108424.94	1997-02-28	1-URGENT       	Clerk#000000205	0	r ideas. carefully pe	1997
3011	91	F	46418.85	1992-01-14	5-LOW          	Clerk#000000515	0	onic deposits kindle slyly. dependencies around the quickly iro	1992
3014	29	F	194159.59	1992-10-30	4-NOT SPECIFIED	Clerk#000000476	0	ep blithely according to the blith	1992
3015	103	F	110826.83	1992-09-27	5-LOW          	Clerk#000000013	0	ously regular deposits affix carefully. furiousl	1992
3041	113	O	23039.46	1997-06-03	5-LOW          	Clerk#000000092	0	s. unusual, pending deposits use carefully. thinly final	1997
3045	50	O	85822.67	1995-09-27	1-URGENT       	Clerk#000000405	0	 express courts sleep quickly special asymptotes. 	1995
3047	25	O	37881.31	1997-03-21	1-URGENT       	Clerk#000000962	0	as. slyly express deposits are dogged pearls. silent ide	1997
3072	23	F	87475.82	1994-01-30	4-NOT SPECIFIED	Clerk#000000370	0	ely final deposits cajole carefully. ironic, re	1994
3073	136	F	151419.50	1994-01-08	3-MEDIUM       	Clerk#000000404	0	kly slyly bold accounts. express courts near the regular ideas sleep bli	1994
3074	67	F	85861.93	1992-11-01	5-LOW          	Clerk#000000546	0	yly even asymptotes shall have to haggle fluffily. deposits are	1992
3076	92	F	93828.15	1993-07-23	2-HIGH         	Clerk#000000099	0	busy foxes. deposits affix quickly ironic, pending pint	1993
3078	49	F	46310.83	1993-02-12	2-HIGH         	Clerk#000000110	0	ounts are alongside of the blith	1993
3108	85	F	63278.00	1993-08-05	1-URGENT       	Clerk#000000574	0	s packages haggle furiously am	1993
3136	23	F	145426.11	1994-08-10	4-NOT SPECIFIED	Clerk#000000891	0	tructions sleep slyly. pending di	1994
3137	136	O	8958.65	1995-07-26	3-MEDIUM       	Clerk#000000063	0	ymptotes wake carefully above t	1995
3138	139	F	139579.18	1994-02-09	4-NOT SPECIFIED	Clerk#000000650	0	e fluffily final theodolites. even dependencies wake along the quickly ir	1994
3139	17	F	40975.96	1992-01-02	3-MEDIUM       	Clerk#000000855	0	ounts against the ruthlessly unusual dolphins	1992
3168	136	F	69412.71	1992-01-30	5-LOW          	Clerk#000000352	0	s sleep slyly? ironic, furious instructions detect. quickly final i	1992
3170	5	O	190142.17	1997-11-09	1-URGENT       	Clerk#000000288	0	requests. furiously bold	1997
3171	47	F	84405.78	1993-04-06	5-LOW          	Clerk#000000940	0	ar deposits. idly r	1993
3173	148	O	64892.73	1996-08-10	5-LOW          	Clerk#000000516	0	ial requests lose along t	1996
3200	13	O	131103.31	1996-02-07	1-URGENT       	Clerk#000000020	0	 regular dependencies impress evenly even excuses. blithely 	1996
3204	10	F	41573.42	1992-12-26	1-URGENT       	Clerk#000000693	0	ess somas cajole slyly. pending accounts cajole	1992
3205	148	F	153637.79	1992-04-11	5-LOW          	Clerk#000000803	0	e furiously. quickly regular dinos about the final pinto be	1992
3206	122	O	64344.86	1996-08-09	1-URGENT       	Clerk#000000755	0	ntegrate furiously final, express 	1996
3207	22	O	133038.59	1998-02-16	1-URGENT       	Clerk#000000695	0	uriously accounts. fluffily i	1998
3233	140	F	54121.92	1994-10-24	5-LOW          	Clerk#000000470	0	ly ironic epitaphs use stealthy, express deposits. quickly regular instruct	1994
3234	14	O	147343.68	1996-04-05	4-NOT SPECIFIED	Clerk#000000367	0	ents according to the dependencies will sleep after the blithely even p	1996
3235	46	O	104695.09	1995-11-15	5-LOW          	Clerk#000000349	0	 quickly pinto beans. ironi	1995
3236	142	O	39470.39	1996-11-06	4-NOT SPECIFIED	Clerk#000000553	0	ithely slyly pending req	1996
3264	94	O	162634.53	1996-11-02	5-LOW          	Clerk#000000244	0	carefully. express, bold	1996
3266	4	P	68309.28	1995-03-17	5-LOW          	Clerk#000000545	0	refully ironic instructions. slyly final pi	1995
3268	142	F	36024.96	1994-06-25	5-LOW          	Clerk#000000746	0	y brave requests unwind furiously accordin	1994
3269	17	O	218697.85	1996-03-01	3-MEDIUM       	Clerk#000000378	0	ts. accounts wake carefully. carefully dogged accounts wake slyly slyly i	1996
3270	38	O	166669.86	1997-05-28	1-URGENT       	Clerk#000000375	0	uffily pending courts ca	1997
3296	148	F	187553.35	1994-10-19	3-MEDIUM       	Clerk#000000991	0	as! carefully final requests wake. furiously even	1994
3297	139	F	9679.45	1992-11-03	2-HIGH         	Clerk#000000220	0	 after the theodolites cajole carefully according to the finally	1992
3299	91	F	42867.92	1993-12-26	3-MEDIUM       	Clerk#000000853	0	bold deposits. special instructions sleep care	1993
3329	4	O	46107.70	1995-07-03	2-HIGH         	Clerk#000000236	0	old deposits. special accounts haggle furiousl	1995
3331	91	F	65189.17	1993-05-21	2-HIGH         	Clerk#000000901	0	uffily carefully sly accounts. blithely unu	1993
3333	92	F	197973.22	1992-09-16	4-NOT SPECIFIED	Clerk#000000157	0	ctions boost slyly quickly even accounts. deposits along	1992
3334	76	O	28930.68	1996-02-18	5-LOW          	Clerk#000000532	0	ounts maintain carefully. furiously close request	1996
3335	49	O	112603.34	1995-10-15	3-MEDIUM       	Clerk#000000694	0	 deposits poach. ironic ideas about the carefully ironi	1995
3360	103	O	168750.48	1998-01-23	5-LOW          	Clerk#000000254	0	 the deposits. fluffily bold requests cajole regula	1998
3361	49	F	75026.51	1992-08-23	4-NOT SPECIFIED	Clerk#000000577	0	unts detect furiously instructions. slow deposi	1992
3362	140	O	183176.60	1995-07-29	5-LOW          	Clerk#000000011	0	the quickly pending deposits. silent, ev	1995
3363	52	O	91017.61	1995-09-23	2-HIGH         	Clerk#000000615	0	posits. ironic, final deposits are furiously slyly pending 	1995
3364	46	O	108412.57	1997-06-21	1-URGENT       	Clerk#000000280	0	y even foxes? blithely stea	1997
3366	52	O	13603.08	1997-05-18	1-URGENT       	Clerk#000000160	0	 pinto beans upon the quickly expres	1997
3367	73	F	101339.68	1992-12-31	4-NOT SPECIFIED	Clerk#000000029	0	efully blithely ironic pinto beans. carefully close 	1992
3392	74	O	96057.42	1995-10-28	1-URGENT       	Clerk#000000325	0	es thrash blithely depths. bold multipliers wake f	1995
3393	98	O	183104.71	1995-07-04	2-HIGH         	Clerk#000000076	0	even requests. excuses are carefully deposits. fluf	1995
3397	130	F	80084.61	1994-06-23	3-MEDIUM       	Clerk#000000048	0	yly. final deposits wake f	1994
3398	67	O	1147.42	1996-09-23	1-URGENT       	Clerk#000000818	0	uthless, special courts atop the unusual accounts grow fur	1996
3399	122	P	56938.16	1995-02-28	4-NOT SPECIFIED	Clerk#000000575	0	the carefully sly accounts. regular, pending theodolites wa	1995
3424	103	O	42410.57	1996-08-21	1-URGENT       	Clerk#000000190	0	ven requests are quickly pending accounts. blithely furious requests 	1996
3425	115	O	157040.57	1996-03-31	4-NOT SPECIFIED	Clerk#000000188	0	ions. deposits nag blithely alongside of the carefully f	1996
3427	4	O	133451.14	1997-05-29	4-NOT SPECIFIED	Clerk#000000404	0	y final pinto beans snooze fluffily bold asymptot	1997
3428	10	O	88047.04	1996-04-07	5-LOW          	Clerk#000000953	0	lar excuses. slyly pending ideas detect p	1996
3429	146	O	141902.54	1997-01-06	4-NOT SPECIFIED	Clerk#000000737	0	l deposits cajole furiously enticing deposits. blithe packages haggle careful	1997
3430	113	F	161066.22	1994-12-12	4-NOT SPECIFIED	Clerk#000000664	0	 regular attainments are at the final foxes. final packages along the blithe	1994
3431	47	F	45536.27	1993-08-22	1-URGENT       	Clerk#000000439	0	 sleep. slyly busy Tiresias a	1993
3456	46	F	32796.35	1993-06-01	5-LOW          	Clerk#000000924	0	es promise slyly. ironicall	1993
3457	25	P	174223.20	1995-04-27	4-NOT SPECIFIED	Clerk#000000849	0	ely thin asymptotes. deposits kindle. pending	1995
3459	119	F	127134.05	1994-07-28	4-NOT SPECIFIED	Clerk#000000777	0	n instructions? carefully regular excuses are blithely. silent, ironi	1994
3488	148	F	92716.17	1995-01-08	3-MEDIUM       	Clerk#000000694	0	cording to the carefully regular deposits. re	1995
3490	91	O	100106.96	1997-05-26	5-LOW          	Clerk#000000703	0	gular ideas. furiously silent deposits across the unusual accounts boost i	1997
3492	103	F	168721.45	1994-11-24	5-LOW          	Clerk#000000066	0	packages along the regular foxes lose final dependencie	1994
3494	49	F	136058.70	1993-04-04	5-LOW          	Clerk#000000559	0	r instructions haggle. accounts cajole. carefully final requests at the 	1993
3524	94	F	22767.49	1992-05-03	2-HIGH         	Clerk#000000607	0	efully unusual tithes among the foxes use blithely daringly bold deposits. re	1992
3526	56	F	53827.34	1995-03-16	5-LOW          	Clerk#000000364	0	to the quickly special deposits print agai	1995
3527	56	O	145232.09	1997-06-21	5-LOW          	Clerk#000000874	0	regular ideas across the quickly bold theodo	1997
3553	91	F	119838.14	1994-05-18	3-MEDIUM       	Clerk#000000270	0	counts mold furiously. slyly i	1994
3555	46	O	134442.37	1996-07-07	5-LOW          	Clerk#000000585	0	s nag carefully regular, even pinto be	1996
3556	16	F	114681.55	1992-09-23	4-NOT SPECIFIED	Clerk#000000140	0	e. dependencies need to haggle alongs	1992
3558	28	O	112912.00	1996-02-29	1-URGENT       	Clerk#000000841	0	around the furiously even requests. quickl	1996
3559	106	F	30722.49	1992-10-24	3-MEDIUM       	Clerk#000000634	0	sly deposits. fluffily final ideas cajole careful	1992
3584	13	O	80487.97	1997-08-11	1-URGENT       	Clerk#000000760	0	fully bold packages. fluffily final braids haggle final, ironic dolphins. b	1997
3585	139	F	159015.39	1994-11-23	2-HIGH         	Clerk#000000988	0	regular asymptotes. bold pains above the carefully pending asymptot	1994
3588	119	F	207925.83	1995-03-19	4-NOT SPECIFIED	Clerk#000000316	0	ong the pains. evenly unusual 	1995
3591	136	F	98140.86	1993-12-08	3-MEDIUM       	Clerk#000000144	0	ual foxes haggle! unusual request	1993
3616	128	F	60933.29	1994-02-16	4-NOT SPECIFIED	Clerk#000000268	0	uickly about the quickly final requests. fluffily final packages wake evenly	1994
3617	40	O	126205.42	1996-03-19	3-MEDIUM       	Clerk#000000886	0	the carefully regular platelets ha	1996
3618	10	O	136954.81	1997-12-13	3-MEDIUM       	Clerk#000000894	0	. ideas run carefully. thin, pending 	1997
3621	142	F	106150.05	1993-05-06	3-MEDIUM       	Clerk#000000643	0	kly unusual deposits. qu	1993
3622	91	O	109202.90	1995-11-27	5-LOW          	Clerk#000000012	0	c deposits are fluffily about the blithely final theo	1995
3623	4	O	175017.68	1996-12-26	1-URGENT       	Clerk#000000184	0	- ironic excuses boost quickly in place 	1996
3649	40	F	124470.32	1994-07-06	5-LOW          	Clerk#000000349	0	taphs boost above the final p	1994
3650	46	F	189547.57	1992-05-28	4-NOT SPECIFIED	Clerk#000000454	0	kages sleep fluffily slyly	1992
3653	40	F	142866.39	1994-03-27	1-URGENT       	Clerk#000000402	0	 pearls. bold accounts are along the ironic,	1994
3655	49	F	74882.22	1992-10-06	1-URGENT       	Clerk#000000815	0	er the carefully unusual deposits sleep quickly according to	1992
3681	52	F	36889.65	1992-04-04	1-URGENT       	Clerk#000000566	0	. ironic deposits against the ironic, regular frets use pending plat	1992
3684	23	F	89509.91	1993-07-20	2-HIGH         	Clerk#000000835	0	bold accounts affix along the carefully ironic requ	1993
3686	40	O	82190.77	1998-07-07	2-HIGH         	Clerk#000000175	0	s. furiously final pinto beans poach carefully among 	1998
3712	64	F	127527.05	1992-01-02	2-HIGH         	Clerk#000000032	0	 promise according 	1992
3714	40	O	84493.55	1998-05-01	3-MEDIUM       	Clerk#000000595	0	nding accounts. ironic pinto beans wake slyly. furiously pendin	1998
3717	28	O	176525.53	1998-06-03	4-NOT SPECIFIED	Clerk#000000974	0	t the carefully even ideas use sp	1998
3746	74	F	80018.54	1994-09-11	4-NOT SPECIFIED	Clerk#000000188	0	. express, special requests nag quic	1994
3749	38	P	87073.89	1995-02-24	3-MEDIUM       	Clerk#000000639	0	y regular instructions haggle blithel	1995
3751	10	F	202917.72	1994-04-27	4-NOT SPECIFIED	Clerk#000000925	0	sheaves. express, unusual t	1994
3776	85	F	150349.92	1992-11-20	2-HIGH         	Clerk#000000698	0	efully even platelets slee	1992
3777	28	F	82467.29	1994-04-08	3-MEDIUM       	Clerk#000000941	0	 regular, special dolphins cajole enticingly ca	1994
3778	106	F	221036.31	1993-05-26	1-URGENT       	Clerk#000000187	0	 above the express requests. packages maintain fluffily according to	1993
3779	74	O	31538.94	1997-01-05	4-NOT SPECIFIED	Clerk#000000670	0	 against the deposits. quickly bold instructions x-ray. pending fox	1997
3780	41	O	65385.42	1996-04-13	5-LOW          	Clerk#000000967	0	 around the brave, pendin	1996
3781	139	O	133864.82	1996-06-20	1-URGENT       	Clerk#000000394	0	yly after the ruthless packages. pinto beans use slyly: never ironic dependenc	1996
3809	148	O	143070.70	1996-05-01	5-LOW          	Clerk#000000646	0	 regular excuses. even theodolites are fluffily according to t	1996
3811	80	O	154967.89	1998-04-16	3-MEDIUM       	Clerk#000000290	0	sits wake slyly abo	1998
3812	41	O	70502.52	1996-08-13	3-MEDIUM       	Clerk#000000727	0	al, final requests cajole	1996
3813	146	O	77247.05	1998-06-29	1-URGENT       	Clerk#000000531	0	g the furiously regular instructions	1998
3815	104	O	14275.01	1997-08-26	1-URGENT       	Clerk#000000249	0	es snooze carefully stealth	1997
3842	28	F	131447.03	1992-04-09	5-LOW          	Clerk#000000418	0	silent ideas. final deposits use furiously. blithely express excuses cajole fu	1992
3843	10	O	34035.17	1997-01-04	4-NOT SPECIFIED	Clerk#000000693	0	eodolites; slyly unusual accounts nag boldly 	1997
3846	49	O	123120.06	1998-02-05	2-HIGH         	Clerk#000000877	0	y alongside of the slyl	1998
3872	134	O	198538.68	1996-09-06	5-LOW          	Clerk#000000943	0	counts boost slyly against the ironic platelets-- blithely p	1996
3873	55	O	95291.79	1998-03-30	4-NOT SPECIFIED	Clerk#000000791	0	express deposits-- even ideas 	1998
3874	119	F	66455.34	1993-06-09	3-MEDIUM       	Clerk#000000208	0	ular asymptotes sleep blithely ironic ideas. blithel	1993
3876	29	O	95126.32	1996-08-02	5-LOW          	Clerk#000000708	0	into beans. blithely	1996
3877	17	F	178492.01	1993-05-21	5-LOW          	Clerk#000000652	0	foxes. thinly bold reques	1993
3879	142	O	80274.22	1995-11-23	1-URGENT       	Clerk#000000231	0	sts along the quickly ironic sentiments cajole carefully according to t	1995
3905	22	F	56227.04	1993-12-21	4-NOT SPECIFIED	Clerk#000000573	0	usly even accounts lose quietly above the slyly express p	1993
3906	46	F	145630.76	1992-05-28	3-MEDIUM       	Clerk#000000867	0	ironic theodolites haggle blithely above the final re	1992
3907	67	F	240457.56	1992-08-19	3-MEDIUM       	Clerk#000000084	0	gular pinto beans sleep f	1992
3909	22	O	82746.74	1998-07-27	1-URGENT       	Clerk#000000980	0	nic, special theodolites sleep furiously! furiously 	1998
3911	10	P	35019.95	1995-03-17	4-NOT SPECIFIED	Clerk#000000818	0	he fluffily final forges haggle slyly according to the blithely	1995
3937	94	O	187516.29	1997-11-30	4-NOT SPECIFIED	Clerk#000000189	0	ckages boost carefully blithely q	1997
3941	136	O	95453.80	1996-08-29	2-HIGH         	Clerk#000000503	0	gular theodolites integrate quickly 	1996
3942	76	F	38596.81	1993-06-28	4-NOT SPECIFIED	Clerk#000000608	0	eas cajole bold requests. idly silent instructions 	1993
3943	40	O	60314.97	1996-10-09	5-LOW          	Clerk#000000482	0	se alongside of the final pinto beans. regular packages boost across the ca	1996
3968	25	O	121704.45	1997-02-17	4-NOT SPECIFIED	Clerk#000000431	0	 the slyly special accounts; 	1997
3969	52	O	169797.40	1997-05-14	2-HIGH         	Clerk#000000731	0	uriously final dependencies slee	1997
3970	76	F	163709.85	1992-03-27	3-MEDIUM       	Clerk#000000190	0	luffily furiously regular deposits. blithely special requests cajole blithely	1992
3971	104	O	47925.47	1996-06-28	5-LOW          	Clerk#000000287	0	alongside of the instructions ought to are 	1996
3973	103	F	91541.48	1992-03-24	4-NOT SPECIFIED	Clerk#000000114	0	somas according to the quickly even instructions wake fu	1992
3974	94	O	56779.06	1996-03-05	4-NOT SPECIFIED	Clerk#000000938	0	deposits are furiously beneath the bl	1996
4001	115	O	95929.46	1997-05-15	3-MEDIUM       	Clerk#000000878	0	detect. asymptotes sleep furio	1997
4002	104	O	76518.11	1997-04-08	5-LOW          	Clerk#000000097	0	 regular braids are. furiously even patterns agains	1997
4005	140	O	129062.13	1996-11-20	2-HIGH         	Clerk#000000341	0	ily according to the slyly iron	1996
4032	10	O	62497.51	1998-02-26	3-MEDIUM       	Clerk#000000686	0	iresias sleep slyly regular ideas. quickly unusual	1998
4034	94	F	186912.51	1993-11-14	4-NOT SPECIFIED	Clerk#000000548	0	ts x-ray. express requests affix fluffily regular theodolites. pending, fina	1993
4258	92	O	133829.35	1996-10-27	4-NOT SPECIFIED	Clerk#000000364	0	efully final platelets around the blit	1996
4036	47	O	82563.10	1997-04-26	3-MEDIUM       	Clerk#000000398	0	ly express deposits nag slyly. ironic, final asymptotes boost bra	1997
4038	94	O	155045.39	1996-01-06	1-URGENT       	Clerk#000000272	0	re slyly. silent requests wake quickly. regular packages play quickly 	1996
4039	29	O	143753.01	1997-11-16	1-URGENT       	Clerk#000000358	0	ly ironic deposits. ironic reques	1997
4064	130	O	148500.71	1996-10-10	4-NOT SPECIFIED	Clerk#000000598	0	ccounts. furiously unusual theodolites wake carefully about	1996
4065	80	F	156345.64	1994-06-09	1-URGENT       	Clerk#000000131	0	even foxes! slyly final deposits agai	1994
4067	16	F	136517.34	1992-10-07	2-HIGH         	Clerk#000000027	0	tes boost furiously quick asymptotes. final deposits of the dolphins solv	1992
4069	73	F	198816.13	1992-05-13	3-MEDIUM       	Clerk#000000359	0	deposits: slyly bold ideas detect furiously. f	1992
4070	29	O	98275.37	1995-06-12	2-HIGH         	Clerk#000000713	0	xpress ideas poach ab	1995
4071	148	O	67789.42	1996-09-15	4-NOT SPECIFIED	Clerk#000000486	0	nal deposits. pending deposits d	1996
4096	139	F	81089.61	1992-07-03	4-NOT SPECIFIED	Clerk#000000706	0	sits. quickly thin deposits x-ray blith	1992
4097	10	O	134308.04	1996-05-24	1-URGENT       	Clerk#000000475	0	ickly under the even accounts. even packages after the furiously express	1996
4098	23	O	48478.54	1996-11-05	4-NOT SPECIFIED	Clerk#000000491	0	otes. quickly final requests after the stealthily ironic pinto bean	1996
4099	17	F	207364.80	1992-08-21	1-URGENT       	Clerk#000000379	0	r platelets. slyly regular requests cajole carefully against the	1992
4100	4	O	3892.77	1996-03-12	3-MEDIUM       	Clerk#000000429	0	posits. carefully unusual packages use pending deposits. regular she	1996
4101	142	F	21640.10	1993-11-22	4-NOT SPECIFIED	Clerk#000000704	0	y around the express, careful epitaphs. accounts use fluffily. quickly p	1993
4102	22	O	128786.57	1996-03-17	1-URGENT       	Clerk#000000675	0	nding dependencies was slyly about the bl	1996
4103	106	F	38164.23	1992-07-03	5-LOW          	Clerk#000000679	0	fully ironic dependencies.	1992
4128	139	O	5472.17	1995-10-07	4-NOT SPECIFIED	Clerk#000000635	0	ctions. dependencies from the slyly regular accounts nag slyly fu	1995
4130	104	O	47823.04	1996-03-10	5-LOW          	Clerk#000000609	0	omise alongside of the carefully final foxes. blithel	1996
4133	101	F	31693.88	1992-08-07	4-NOT SPECIFIED	Clerk#000000268	0	al, express foxes. quickly pending deposits might cajole alongsi	1992
4135	37	O	99577.55	1997-03-10	3-MEDIUM       	Clerk#000000627	0	ly quietly even ideas. deposits haggle blithely	1997
4160	55	O	82493.07	1996-08-20	5-LOW          	Clerk#000000283	0	the carefully special accounts. furiously regular dugouts alongs	1996
4162	22	F	72359.55	1992-02-10	5-LOW          	Clerk#000000179	0	r packages are slyly accounts. furiously special foxes detect carefully re	1992
4163	64	F	11493.80	1992-12-21	2-HIGH         	Clerk#000000268	0	 integrate furiously slyly regular depende	1992
4164	94	O	8709.16	1998-07-03	2-HIGH         	Clerk#000000720	0	 regularly busy theodolites boost furiously quickly bold packages. express, s	1998
4165	4	O	11405.40	1997-07-25	3-MEDIUM       	Clerk#000000621	0	special foxes affix never blithely ironic pinto beans; blithely 	1997
4167	28	O	62108.45	1998-06-17	1-URGENT       	Clerk#000000917	0	kly furiously even deposits. unu	1998
4192	146	O	197192.95	1998-04-19	1-URGENT       	Clerk#000000369	0	equests above the slyly regular pinto beans unwi	1998
4193	4	F	143191.54	1994-01-09	2-HIGH         	Clerk#000000201	0	ng accounts haggle quickly. packages use fluffily ironic excu	1994
4194	106	F	62972.29	1994-10-16	3-MEDIUM       	Clerk#000000385	0	 instructions are quickly even pinto beans. courts boost furiously regular, ev	1994
4195	104	F	54478.95	1993-05-29	4-NOT SPECIFIED	Clerk#000000777	0	 pinto beans cajole furiously theodolites-- slyly regular deposits doub	1993
4196	106	O	201455.98	1998-05-15	3-MEDIUM       	Clerk#000000532	0	affix carefully. quickly final requests 	1998
4197	92	O	217709.03	1996-08-13	4-NOT SPECIFIED	Clerk#000000264	0	 pinto beans according	1996
4199	5	F	30494.62	1992-02-13	1-URGENT       	Clerk#000000309	0	e blithely. special deposits haggle slyly final foxes. carefully even	1992
4225	128	O	72533.07	1997-06-03	3-MEDIUM       	Clerk#000000992	0	r the platelets nag among the special deposits. ironic, ironic re	1997
4226	92	F	29827.44	1993-03-09	5-LOW          	Clerk#000000203	0	phins wake slyly regular packages. deposits haggle slowl	1993
4228	110	O	22072.16	1997-03-28	5-LOW          	Clerk#000000309	0	pecial requests aft	1997
4229	14	O	75145.87	1998-03-03	1-URGENT       	Clerk#000000301	0	p furiously: final excuses hagg	1998
4230	140	F	219709.60	1992-03-04	1-URGENT       	Clerk#000000364	0	lly ironic deposits integrate carefully about the fu	1992
4231	86	O	111403.66	1997-11-20	4-NOT SPECIFIED	Clerk#000000630	0	ly final accounts cajole furiously accounts. bravely ironic platelets am	1997
4257	17	P	41723.86	1995-03-25	3-MEDIUM       	Clerk#000000682	0	r ideas cajole along the blithely regular gifts.	1995
4259	104	O	12918.70	1997-10-09	5-LOW          	Clerk#000000781	0	es snooze slyly against the furiously unusual ideas. furious	1997
4260	142	F	18566.14	1992-05-16	4-NOT SPECIFIED	Clerk#000000919	0	e among the fluffily bold accounts.	1992
4263	4	O	158885.83	1998-03-16	1-URGENT       	Clerk#000000265	0	sly ruthless deposits. final packages are instructions. fu	1998
4290	41	F	26128.99	1995-01-15	3-MEDIUM       	Clerk#000000688	0	 slyly quickly bold requests. final deposits haggle pending ideas! som	1995
4292	25	F	145906.24	1992-01-09	3-MEDIUM       	Clerk#000000794	0	 ruthlessly. slyly bo	1992
4293	103	O	198322.91	1996-08-20	2-HIGH         	Clerk#000000750	0	ly packages. regular packages nag according to t	1996
4294	49	F	232194.74	1992-08-15	3-MEDIUM       	Clerk#000000407	0	ng pinto beans breach. slyly express requests bo	1992
4295	5	O	77754.62	1996-02-10	3-MEDIUM       	Clerk#000000023	0	e boldly bold dependencies	1996
4320	115	O	67049.37	1996-12-08	4-NOT SPECIFIED	Clerk#000000223	0	ages haggle after the slowly bold se	1996
4321	16	F	118896.95	1994-07-18	3-MEDIUM       	Clerk#000000041	0	ending deposits are carefully carefully regular packa	1994
4322	142	O	149671.92	1998-03-13	3-MEDIUM       	Clerk#000000433	0	totes nag across the fluffily special instructions. quickly silent hockey 	1998
4323	104	F	27598.17	1994-01-23	2-HIGH         	Clerk#000000282	0	lve after the slyly regular multipliers. even, regular excus	1994
4324	73	O	178249.05	1995-07-17	1-URGENT       	Clerk#000000800	0	ccounts. slyly stealthy requests shall have t	1995
4325	130	O	20214.49	1996-07-18	2-HIGH         	Clerk#000000591	0	y around the always ev	1996
4326	29	O	39048.94	1996-10-27	4-NOT SPECIFIED	Clerk#000000869	0	packages. carefully express deposit	1996
4327	146	P	126235.35	1995-03-16	2-HIGH         	Clerk#000000571	0	yly pending braids. final requests abo	1995
4352	14	O	18653.09	1997-11-26	2-HIGH         	Clerk#000000620	0	ly final platelets integrate carefully even requ	1997
4353	73	O	21815.30	1997-12-12	2-HIGH         	Clerk#000000790	0	uickly even ideas cajole	1997
4355	4	O	186370.23	1996-11-16	1-URGENT       	Clerk#000000362	0	ndencies use furiously across the regular 	1996
4357	47	O	67045.94	1997-10-23	4-NOT SPECIFIED	Clerk#000000031	0	ages nag between the	1997
4358	25	O	46298.53	1997-08-12	1-URGENT       	Clerk#000000692	0	according to the fluffily special asymptotes 	1997
4359	16	F	107824.40	1993-03-03	1-URGENT       	Clerk#000000393	0	sts. special, unusual deposits across the ironic theodo	1993
4384	25	F	52562.16	1992-07-13	1-URGENT       	Clerk#000000192	0	onic platelets. furiously regular asymptotes according to the special pac	1992
4385	122	O	39190.62	1996-08-06	2-HIGH         	Clerk#000000597	0	ully final requests. ironic, even dolphins above the regular 	1996
4387	110	O	116740.67	1995-10-23	1-URGENT       	Clerk#000000025	0	ter the regular pinto beans. special, final gifts above the requests wi	1995
4388	10	O	69668.22	1996-03-28	2-HIGH         	Clerk#000000715	0	ts wake against the carefully final accounts. sly	1996
4389	55	F	120324.82	1994-05-05	3-MEDIUM       	Clerk#000000403	0	wly express excuses after the permanently even instructions are	1994
4391	38	F	48284.06	1992-02-18	2-HIGH         	Clerk#000000880	0	regular accounts. even depo	1992
4417	67	O	60868.39	1998-07-09	1-URGENT       	Clerk#000000365	0	ideas are alongside of the blithely final reque	1998
4419	104	O	94030.43	1996-06-12	4-NOT SPECIFIED	Clerk#000000410	0	ages wake furiously slyly thin theodolit	1996
4421	10	O	258779.02	1997-04-04	3-MEDIUM       	Clerk#000000246	0	t the pending warhorses. express waters a	1997
4423	64	F	4913.06	1995-02-17	5-LOW          	Clerk#000000888	0	excuses are ruthless	1995
4449	10	O	48206.14	1998-02-08	5-LOW          	Clerk#000000035	0	ourts are carefully even deposits. pending 	1998
4450	106	O	110194.31	1997-07-15	1-URGENT       	Clerk#000000867	0	quests boost. furiously even realms are blithely bold requests. bl	1997
4451	4	F	92851.80	1994-10-01	1-URGENT       	Clerk#000000181	0	. carefully final foxes along the quickly express T	1994
4452	13	F	64838.66	1994-06-21	5-LOW          	Clerk#000000985	0	oxes are slyly. express, ironic pinto beans wake after the quickly pending re	1994
4454	142	F	159578.94	1994-02-02	5-LOW          	Clerk#000000411	0	uriously regular pint	1994
4480	85	F	28658.26	1994-03-31	4-NOT SPECIFIED	Clerk#000000534	0	press, bold deposits boost blit	1994
4481	148	O	77705.40	1996-03-30	5-LOW          	Clerk#000000443	0	press sheaves cajole furio	1996
4483	52	F	126597.21	1992-03-07	3-MEDIUM       	Clerk#000000615	0	its. blithely idle accounts run; theodolites wake carefully around the fi	1992
4484	131	O	237947.61	1996-12-24	1-URGENT       	Clerk#000000392	0	ct across the pinto beans. quickly pending excuses engage furiously.	1996
4486	37	O	135613.18	1998-03-03	2-HIGH         	Clerk#000000656	0	ffily according to the carefully pending acc	1998
4487	46	F	109469.90	1993-02-23	3-MEDIUM       	Clerk#000000017	0	s up the never pending excuses wake furiously special pinto beans. furiously i	1993
4513	85	O	119820.38	1996-03-15	5-LOW          	Clerk#000000154	0	ests. final, final ideas	1996
4515	140	F	161745.44	1992-03-17	1-URGENT       	Clerk#000000191	0	quests among the accounts sleep boldly about the regular f	1992
4516	130	F	35949.14	1994-03-29	3-MEDIUM       	Clerk#000000739	0	ing packages sleep slyly regular attainments	1994
4517	113	O	47614.08	1998-03-07	4-NOT SPECIFIED	Clerk#000000231	0	uriously final deposits doze furiously furiously reg	1998
4519	136	F	68885.66	1993-03-30	4-NOT SPECIFIED	Clerk#000000938	0	ccording to the final 	1993
4545	59	F	143276.28	1993-01-17	4-NOT SPECIFIED	Clerk#000000303	0	ep. requests use sly	1993
4549	64	O	43889.17	1998-03-05	4-NOT SPECIFIED	Clerk#000000965	0	ully even deposits dazzle. fluffily pending ideas against the requests	1998
4576	139	O	56936.10	1996-08-14	5-LOW          	Clerk#000000798	0	e pending deposits. 	1996
4578	91	F	95761.93	1992-09-13	5-LOW          	Clerk#000000121	0	 to the furiously ironic instructions? furiou	1992
4579	106	O	85927.85	1995-12-01	2-HIGH         	Clerk#000000951	0	its wake quickly blithely specia	1995
4583	22	F	206495.43	1994-09-25	3-MEDIUM       	Clerk#000000240	0	equests. slyly even platelets was qui	1994
4608	80	F	157767.86	1994-06-17	1-URGENT       	Clerk#000000259	0	y even instructions detect slyly asymptotes. blithely final packa	1994
4611	29	F	166506.22	1993-01-10	2-HIGH         	Clerk#000000152	0	. furiously regular instructions haggle dolphins. even instructions det	1993
4615	29	F	10500.27	1993-08-27	3-MEDIUM       	Clerk#000000982	0	jole after the fluffily pending foxes. packages affix carefully acco	1993
4641	134	F	98485.21	1993-01-20	4-NOT SPECIFIED	Clerk#000000755	0	ronic, final requests integrate slyly: specia	1993
4642	148	F	117537.87	1995-02-27	1-URGENT       	Clerk#000000295	0	cial requests wake carefully around the regular, unusual ideas. furi	1995
4643	67	O	52414.19	1995-06-30	2-HIGH         	Clerk#000000292	0	ously regular packages. unusual, special platel	1995
4644	94	O	85901.70	1998-01-17	5-LOW          	Clerk#000000961	0	requests. fluffily even ideas bo	1998
4647	28	F	110958.36	1994-05-14	3-MEDIUM       	Clerk#000000626	0	out the deposits. slyly final pinto beans haggle idly. slyly s	1994
4674	37	F	115411.37	1994-04-19	1-URGENT       	Clerk#000000122	0	careful hockey players. carefully pending deposits caj	1994
4675	86	F	68817.08	1993-11-25	4-NOT SPECIFIED	Clerk#000000741	0	al deposits haggle slyly final	1993
4676	14	O	182025.95	1995-09-01	2-HIGH         	Clerk#000000407	0	s. slyly bold accounts sleep furiously special	1995
4677	40	O	25661.87	1998-02-21	3-MEDIUM       	Clerk#000000245	0	ly pending deposits after the carefully regular foxes sleep blithely after t	1998
4705	98	F	173340.09	1992-03-22	4-NOT SPECIFIED	Clerk#000000522	0	 special instructions poa	1992
4706	25	F	101709.52	1992-12-29	4-NOT SPECIFIED	Clerk#000000722	0	 packages above the never regular packages nag packages. deposits c	1992
4707	91	F	61052.10	1995-02-27	2-HIGH         	Clerk#000000943	0	ully enticing accounts behind the regular	1995
4708	85	F	56998.36	1994-10-01	1-URGENT       	Clerk#000000383	0	ly thinly even accounts. unusu	1994
4711	142	O	129546.56	1998-05-06	1-URGENT       	Clerk#000000818	0	mptotes. unusual packages wake furiously qui	1998
4736	139	O	67572.73	1995-11-20	2-HIGH         	Clerk#000000563	0	blithely regular courts affix into the carefully ironic deposits. slyly exp	1995
4738	5	F	149466.62	1992-04-08	2-HIGH         	Clerk#000000150	0	deposits. thin acco	1992
4739	148	F	68255.82	1993-02-21	5-LOW          	Clerk#000000872	0	ing to the pending attainments: pending, express account	1993
4740	68	O	42579.40	1996-07-05	2-HIGH         	Clerk#000000420	0	 dependencies haggle about the	1996
4742	64	P	155356.80	1995-03-23	3-MEDIUM       	Clerk#000000058	0	n packages. quickly regular ideas cajole blithely	1995
4768	136	F	4820.55	1993-11-22	2-HIGH         	Clerk#000000875	0	ctions snooze idly beneath the quick waters. fluffily u	1993
4770	59	O	72150.68	1995-06-20	2-HIGH         	Clerk#000000461	0	cial instructions believe carefully. 	1995
4772	28	F	64102.93	1994-09-14	1-URGENT       	Clerk#000000708	0	es sleep. regular requests haggle furiously slyly 	1994
4773	122	O	196080.26	1995-12-23	1-URGENT       	Clerk#000000327	0	ptotes was slyly along the	1995
4774	52	F	124380.73	1993-04-20	3-MEDIUM       	Clerk#000000299	0	eposits use blithely bold deposits. carefully regular gifts about the fin	1993
4775	128	O	112444.42	1995-08-13	4-NOT SPECIFIED	Clerk#000000609	0	s integrate slyly slyly final instructions. carefully bold pack	1995
4800	37	F	91795.13	1992-01-06	5-LOW          	Clerk#000000625	0	ggle furiously along the pending pinto beans. deposits use: final foxe	1992
4802	130	O	5978.65	1997-01-23	3-MEDIUM       	Clerk#000000400	0	 ironic, thin packages wake furiously ironic, ironic deposits. the	1997
4804	37	F	111547.31	1992-01-28	2-HIGH         	Clerk#000000614	0	ly final accounts. blithely unusual theodolite	1992
4805	16	F	172102.96	1992-04-25	4-NOT SPECIFIED	Clerk#000000514	0	even accounts wake furiously slyly final accounts; blithel	1992
4835	146	F	70857.51	1994-10-25	1-URGENT       	Clerk#000000250	0	s integrate furiously blithely expr	1994
4837	130	O	68519.84	1998-04-24	4-NOT SPECIFIED	Clerk#000000517	0	n accounts are regular, bold accounts. even instructions use request	1998
4839	25	F	71241.63	1994-05-10	1-URGENT       	Clerk#000000925	0	 even somas. slyly express ideas lose carefully. blithely unusu	1994
4865	85	O	162113.46	1997-06-07	3-MEDIUM       	Clerk#000000418	0	sits boost stealthily above the bl	1997
4867	10	F	9741.03	1992-05-21	1-URGENT       	Clerk#000000891	0	ss the slyly regular dependencies. fluffily regular deposits within the car	1992
4868	76	O	159005.35	1997-03-02	5-LOW          	Clerk#000000729	0	regular asymptotes. regular packages sublate carefully al	1997
4870	103	F	94534.07	1994-08-06	3-MEDIUM       	Clerk#000000911	0	nto beans about the blithely regular d	1994
4871	46	O	129636.99	1995-06-12	1-URGENT       	Clerk#000000531	0	ven, special instructions across t	1995
4896	85	F	93206.35	1992-08-22	1-URGENT       	Clerk#000000622	0	sly pending deposits. final accounts boost above the sly, even	1992
4897	80	F	115688.85	1992-09-17	5-LOW          	Clerk#000000184	0	s. bold pinto beans sleep. evenly final accounts daz	1992
4898	14	F	40572.64	1994-07-11	4-NOT SPECIFIED	Clerk#000000841	0	final patterns. special theodolites haggle ruthlessly at the blithely spec	1994
4902	139	O	26011.20	1998-07-04	3-MEDIUM       	Clerk#000000874	0	 the slyly express dolphins. 	1998
4903	92	F	34363.63	1992-03-22	4-NOT SPECIFIED	Clerk#000000907	0	yly. multipliers within the fo	1992
4928	4	F	59931.42	1993-10-04	4-NOT SPECIFIED	Clerk#000000952	0	slyly brave instructions after the ironic excuses haggle ruthlessly about	1993
4931	50	F	115759.13	1994-11-17	1-URGENT       	Clerk#000000356	0	leep. slyly express dolphins nag slyly. furiously regular s	1994
4932	122	F	42927.07	1993-08-10	1-URGENT       	Clerk#000000830	0	onic foxes. enticingly reg	1993
4933	94	O	42945.82	1995-07-14	3-MEDIUM       	Clerk#000000848	0	y special sauternes integr	1995
4934	40	O	180478.16	1997-02-17	1-URGENT       	Clerk#000000372	0	nes cajole; carefully special accounts haggle. special pinto beans nag 	1997
4935	40	F	162088.30	1993-05-25	4-NOT SPECIFIED	Clerk#000000601	0	c foxes. fluffily pendin	1993
4962	104	F	44781.32	1993-07-28	3-MEDIUM       	Clerk#000000008	0	 breach never ironic 	1993
4964	101	O	204163.10	1997-07-28	4-NOT SPECIFIED	Clerk#000000144	0	ithely final theodolites. blithely regu	1997
4965	52	F	110626.82	1993-10-21	5-LOW          	Clerk#000000638	0	dependencies poach packages. sometim	1993
4967	98	O	103814.27	1997-02-17	3-MEDIUM       	Clerk#000000397	0	e theodolites; furiously b	1997
4992	62	F	203904.80	1992-05-10	1-URGENT       	Clerk#000000166	0	telets nag carefully am	1992
4993	13	F	145730.19	1994-08-04	4-NOT SPECIFIED	Clerk#000000258	0	ing instructions nag furiously. un	1994
4995	40	O	189651.76	1996-01-06	4-NOT SPECIFIED	Clerk#000000748	0	s. even deposits boost along the express, even theodolites. stealthily ir	1996
4997	47	O	122611.05	1998-03-18	5-LOW          	Clerk#000000040	0	egrate final pinto beans. fluffily special notornis use blith	1998
4999	85	F	98643.17	1993-06-26	2-HIGH         	Clerk#000000504	0	 dolphins cajole blithely above the sly 	1993
5026	28	O	13197.78	1997-09-06	1-URGENT       	Clerk#000000955	0	y final requests us	1997
5027	148	O	181346.56	1997-08-30	2-HIGH         	Clerk#000000751	0	e-- final, pending requests along t	1997
5028	13	F	30755.69	1992-04-17	2-HIGH         	Clerk#000000180	0	ickly blithely express deposits. b	1992
5030	106	O	71781.23	1998-05-25	4-NOT SPECIFIED	Clerk#000000564	0	 wake slyly furiously thin requests. ironic pinto beans ha	1998
5031	139	F	91438.59	1994-12-02	3-MEDIUM       	Clerk#000000788	0	lar instructions haggle blithely pending foxes? sometimes final excuses h	1994
5056	52	O	62258.18	1997-02-15	5-LOW          	Clerk#000000828	0	lithely above the express ideas. blithely final deposits are fluffily spec	1997
5057	64	O	76164.41	1997-08-03	1-URGENT       	Clerk#000000955	0	r ironic requests of the carefully ironic dependencies wake slyly a	1997
5058	119	O	17031.01	1998-03-23	1-URGENT       	Clerk#000000367	0	 the pending packages wake after the quickly speci	1998
5061	101	F	52190.52	1993-08-14	1-URGENT       	Clerk#000000009	0	e packages use fluffily according to the carefully ironic deposits. bol	1993
5063	23	O	98753.57	1997-05-17	2-HIGH         	Clerk#000000745	0	lyly after the pending foxes. express theodolites breach across t	1997
5089	130	F	109246.54	1992-07-29	1-URGENT       	Clerk#000000677	0	cial platelets. quiet, final ideas cajole carefully. unusu	1992
5091	148	O	47852.06	1998-05-21	3-MEDIUM       	Clerk#000000311	0	egular decoys mold carefully fluffily unus	1998
5092	22	O	195834.96	1995-10-30	5-LOW          	Clerk#000000194	0	are blithely along the pin	1995
5094	106	F	74892.08	1993-03-29	4-NOT SPECIFIED	Clerk#000000406	0	uickly pending deposits haggle quickly ide	1993
5120	16	O	28007.73	1996-06-05	1-URGENT       	Clerk#000000332	0	against the slyly express requests. furiousl	1996
5123	10	O	11850.45	1998-02-10	1-URGENT       	Clerk#000000776	0	ic requests. furiously ironic packages grow above the express, ironic inst	1998
5124	25	O	159170.80	1997-04-04	4-NOT SPECIFIED	Clerk#000000749	0	kly even courts. bold packages solve. 	1997
5125	28	O	38065.28	1998-02-07	5-LOW          	Clerk#000000834	0	ructions. dolphins wake slowly carefully unusual 	1998
5127	73	O	48024.99	1997-01-15	5-LOW          	Clerk#000000829	0	fully express pinto beans. slyly final accounts along the ironic dugouts use s	1997
5153	113	O	193832.28	1995-08-26	1-URGENT       	Clerk#000000954	0	 the furiously ironic foxes. express packages shall cajole carefully across	1995
5157	142	O	167056.34	1997-07-06	4-NOT SPECIFIED	Clerk#000000689	0	closely above the unusual deposits. furiously	1997
5158	76	O	240284.95	1997-01-21	1-URGENT       	Clerk#000000541	0	 regular foxes. even foxes wake blithely 	1997
5159	106	O	147543.26	1996-09-25	1-URGENT       	Clerk#000000303	0	tegrate slyly around the slyly sly sauternes. final pa	1996
5184	85	O	209155.48	1998-07-20	5-LOW          	Clerk#000000250	0	nding accounts detect final, even	1998
5185	148	O	206179.68	1997-07-25	3-MEDIUM       	Clerk#000000195	0	 regular ideas about the even ex	1997
5186	52	O	208892.63	1996-08-03	1-URGENT       	Clerk#000000332	0	pecial platelets. slyly final ac	1996
5187	55	O	46380.69	1997-07-16	3-MEDIUM       	Clerk#000000682	0	ckly according to t	1997
5188	140	P	66268.86	1995-03-02	4-NOT SPECIFIED	Clerk#000000029	0	counts. finally ironic requests ab	1995
5189	71	F	184172.31	1993-11-26	5-LOW          	Clerk#000000940	0	e after the pending accounts. asymptotes boost. re	1993
5216	59	O	16763.95	1997-08-14	3-MEDIUM       	Clerk#000000418	0	des boost across the platelets. slyly busy theodolit	1997
5220	10	F	24844.39	1992-07-30	2-HIGH         	Clerk#000000051	0	 final packages. ideas detect slyly around	1992
5221	13	O	71968.10	1995-06-09	4-NOT SPECIFIED	Clerk#000000324	0	lar accounts above the sl	1995
5222	80	F	1051.15	1994-05-27	4-NOT SPECIFIED	Clerk#000000613	0	along the bold ideas. furiously final foxes snoo	1994
5249	103	F	123586.03	1994-09-06	3-MEDIUM       	Clerk#000000019	0	refully bold accounts 	1994
5252	91	O	173145.37	1996-02-17	1-URGENT       	Clerk#000000724	0	 ironic accounts among the silent asym	1996
5253	148	P	108361.46	1995-04-11	2-HIGH         	Clerk#000000275	0	egular requests! blithely regular deposits alongside of t	1995
5255	64	O	75074.07	1996-07-12	5-LOW          	Clerk#000000591	0	ly slow forges. express foxes haggle. regular, even asymp	1996
5282	50	O	94446.69	1998-01-30	1-URGENT       	Clerk#000000030	0	rding to the unusual, bold accounts. regular instructions	1998
5283	131	F	18594.66	1994-06-04	3-MEDIUM       	Clerk#000000579	0	ests. even, final ideas alongside of t	1994
5287	25	F	30045.95	1993-12-22	5-LOW          	Clerk#000000406	0	regular packages. bold instructions sleep always. carefully final p	1993
5313	13	O	159870.44	1997-06-17	4-NOT SPECIFIED	Clerk#000000896	0	le. final courts haggle furiously according to the 	1997
5315	139	F	55554.97	1992-10-29	4-NOT SPECIFIED	Clerk#000000035	0	 furiously. quickly unusual packages use. sly	1992
5317	37	F	228002.51	1994-09-09	5-LOW          	Clerk#000000687	0	jole quickly at the slyly pend	1994
5318	59	F	106935.19	1993-04-04	2-HIGH         	Clerk#000000663	0	efully regular dolphins. even ideas nag fluffily furiously even packa	1993
5319	98	O	68619.29	1996-01-21	1-URGENT       	Clerk#000000237	0	lent requests. quickly pe	1996
5346	37	F	149536.20	1993-12-26	2-HIGH         	Clerk#000000220	0	gly close packages against the even, regular escapades boost evenly accordi	1993
5347	49	F	173024.71	1995-02-22	3-MEDIUM       	Clerk#000000180	0	onic, regular deposits. packag	1995
5349	67	O	38038.84	1996-09-01	1-URGENT       	Clerk#000000960	0	le along the carefully bold dolphins. carefully special packa	1996
5350	76	F	113417.03	1993-10-10	5-LOW          	Clerk#000000604	0	ccounts after the carefully pending requests believe 	1993
5351	122	O	76799.25	1998-05-11	1-URGENT       	Clerk#000000443	0	to beans sleep furiously after the carefully even	1998
5377	64	O	117728.37	1997-04-24	2-HIGH         	Clerk#000000917	0	ons nag blithely furiously regula	1997
5380	148	O	123014.83	1997-10-12	1-URGENT       	Clerk#000000481	0	le slyly about the slyly final dolphins. fu	1997
5408	23	F	123477.05	1992-07-21	5-LOW          	Clerk#000000735	0	egular requests according to the	1992
5409	13	F	145040.38	1992-01-09	5-LOW          	Clerk#000000171	0	eans. regular accounts are regul	1992
5410	22	O	139104.17	1998-07-28	4-NOT SPECIFIED	Clerk#000000117	0	final deposits: pending excuses boost. ironic theodolites cajole furi	1998
5412	142	O	109979.71	1998-01-20	2-HIGH         	Clerk#000000151	0	ets boost furiously regular accounts. regular foxes above th	1998
5413	94	O	224382.57	1997-10-17	1-URGENT       	Clerk#000000066	0	e even excuses. always final depen	1997
5415	23	F	176864.83	1992-08-05	3-MEDIUM       	Clerk#000000998	0	ly even ideas nag blithely above the final instructions	1992
5440	130	O	3223.17	1997-01-12	1-URGENT       	Clerk#000000154	0	posits boost regularly ironic packages. regular, ironic deposits wak	1997
5441	41	F	131891.05	1994-07-21	4-NOT SPECIFIED	Clerk#000000257	0	after the furiously ironic 	1994
5443	131	O	124950.79	1996-10-10	4-NOT SPECIFIED	Clerk#000000492	0	al foxes could detect. blithely stealthy asymptotes kind	1996
5444	130	P	172908.01	1995-03-18	1-URGENT       	Clerk#000000677	0	 asymptotes. asymptotes cajole quickly quickly bo	1995
5445	115	F	114990.63	1993-07-26	5-LOW          	Clerk#000000623	0	s. even, special requests cajole furiously even, 	1993
5447	13	O	29029.84	1996-03-16	3-MEDIUM       	Clerk#000000597	0	uternes around the furiously bold accounts wake after 	1996
5474	55	F	131079.52	1992-06-01	4-NOT SPECIFIED	Clerk#000000487	0	gle blithely enticing ideas. final, exp	1992
5475	139	O	10645.48	1996-07-07	5-LOW          	Clerk#000000856	0	es shall boost slyly. furiously even deposits lose. instruc	1996
5476	91	O	26906.38	1997-11-06	1-URGENT       	Clerk#000000189	0	furiously final ideas. furiously bold dependencies sleep care	1997
5506	91	F	8413.31	1993-11-08	1-URGENT       	Clerk#000000292	0	nusual theodolites. sly	1993
5508	56	O	3808.05	1996-06-21	1-URGENT       	Clerk#000000128	0	y express packages cajole furiously. slyly unusual requests 	1996
5509	80	F	135335.96	1994-04-08	5-LOW          	Clerk#000000164	0	usual deposits use packages. furiously final requests wake slyly about th	1994
5510	37	F	126948.81	1993-01-08	3-MEDIUM       	Clerk#000000819	0	 nag slyly. carefully eve	1993
5538	139	F	90981.28	1993-12-25	1-URGENT       	Clerk#000000992	0	ttainments. slyly final ideas are about the furiously silent excuses.	1993
5539	119	F	39397.60	1994-07-31	5-LOW          	Clerk#000000675	0	structions. slyly regular patterns solve above the carefully expres	1994
5540	130	O	90707.58	1996-10-12	4-NOT SPECIFIED	Clerk#000000120	0	y ironic packages cajole blithely	1996
5542	49	O	6402.41	1996-04-20	4-NOT SPECIFIED	Clerk#000000100	0	riously among the regularly regular pac	1996
5543	115	F	118201.53	1993-09-25	3-MEDIUM       	Clerk#000000644	0	ckly regular epitaphs. carefully bold accounts haggle furiously	1993
5571	103	F	79248.35	1992-12-19	4-NOT SPECIFIED	Clerk#000000184	0	ts cajole furiously carefully regular sheaves. un	1992
5573	37	O	158479.37	1996-08-15	3-MEDIUM       	Clerk#000000055	0	lites. slyly final pinto beans about the carefully regul	1996
5574	28	F	129803.03	1992-03-10	4-NOT SPECIFIED	Clerk#000000002	0	n deposits. special, regular t	1992
5575	103	O	51839.94	1995-07-24	5-LOW          	Clerk#000000948	0	uriously express frays breach	1995
5602	130	O	67979.49	1997-07-30	3-MEDIUM       	Clerk#000000395	0	onic asymptotes haggl	1997
5603	71	F	145100.47	1992-06-20	4-NOT SPECIFIED	Clerk#000000535	0	 asymptotes. fluffily ironic instructions are. pending pinto bean	1992
5604	46	O	98987.51	1998-04-14	4-NOT SPECIFIED	Clerk#000000123	0	ously across the blithely ironic pinto beans. sile	1998
5607	92	F	24660.06	1992-01-01	4-NOT SPECIFIED	Clerk#000000137	0	c requests promise quickly fluffily ironic deposits. caref	1992
5634	68	O	99494.67	1996-07-31	3-MEDIUM       	Clerk#000000915	0	out the accounts. carefully ironic ideas are slyly. sheaves could h	1996
5636	122	F	143350.75	1995-02-16	3-MEDIUM       	Clerk#000000916	0	. boldly even Tiresias sleep. blithely ironic packages among the ca	1995
5637	103	O	128776.90	1996-06-17	3-MEDIUM       	Clerk#000000183	0	nic dolphins are regular packages. ironic pinto beans hagg	1996
5664	119	O	186215.81	1998-07-23	2-HIGH         	Clerk#000000789	0	the quickly ironic dolp	1998
5666	14	F	121663.68	1994-02-02	2-HIGH         	Clerk#000000396	0	mptotes. quickly final instructions are 	1994
5669	74	O	113156.30	1996-05-06	1-URGENT       	Clerk#000000336	0	ng packages nag fluffily furio	1996
5696	142	P	198723.30	1995-05-04	1-URGENT       	Clerk#000000447	0	e quickly unusual pack	1995
5697	55	F	99177.69	1992-10-05	1-URGENT       	Clerk#000000112	0	pendencies impress furiously. bold, final requests solve ab	1992
5699	142	F	226314.91	1992-07-30	5-LOW          	Clerk#000000311	0	o beans. ironic asymptotes boost. blithe, final courts integrate	1992
5728	80	F	85397.04	1994-12-11	4-NOT SPECIFIED	Clerk#000000426	0	furiously express pin	1994
5732	37	O	28330.42	1997-08-03	1-URGENT       	Clerk#000000910	0	he quickly bold asymptotes: final platelets wake quickly. blithely final pinto	1997
5733	101	F	38545.97	1993-03-17	2-HIGH         	Clerk#000000873	0	osits. pending accounts boost quickly. furiously permanent acco	1993
5734	94	O	45860.94	1997-10-12	3-MEDIUM       	Clerk#000000084	0	efully even braids detect blithely alo	1997
5735	40	F	39358.51	1994-12-11	3-MEDIUM       	Clerk#000000600	0	 bold realms cajole slyly fu	1994
5760	25	F	59404.77	1994-05-25	4-NOT SPECIFIED	Clerk#000000498	0	s among the blithely regular frays haggle ironically bold theodolites. al	1994
5761	16	O	130345.90	1998-07-06	3-MEDIUM       	Clerk#000000208	0	s asymptotes cajole boldly. regular, 	1998
5762	49	O	165019.32	1997-02-14	1-URGENT       	Clerk#000000901	0	ly bold packages: slyly ironic deposits sleep quietly foxes. express a	1997
5764	131	F	53212.95	1993-10-03	4-NOT SPECIFIED	Clerk#000000363	0	 furiously regular deposits haggle fluffily around th	1993
5765	52	F	249900.42	1994-12-15	5-LOW          	Clerk#000000959	0	longside of the quickly final packages. instructions so	1994
5766	49	F	47940.51	1993-09-27	5-LOW          	Clerk#000000753	0	. quickly final packages print slyly. fu	1993
5793	37	O	119887.47	1997-07-13	2-HIGH         	Clerk#000000294	0	thely. fluffily even instructi	1997
5795	37	F	35514.45	1992-05-05	2-HIGH         	Clerk#000000581	0	 even instructions x-ray ironic req	1992
5797	122	O	15313.61	1997-10-15	4-NOT SPECIFIED	Clerk#000000381	0	ng! packages against the blithely b	1997
5798	106	O	125011.92	1998-03-30	5-LOW          	Clerk#000000343	0	lent accounts affix quickly! platelets run slyly slyly final packages. f	1998
5824	56	O	169107.85	1996-12-03	2-HIGH         	Clerk#000000171	0	unusual packages. even ideas along the even requests are along th	1996
5826	22	O	21119.86	1998-06-13	1-URGENT       	Clerk#000000087	0	even, regular dependenc	1998
5830	85	F	28223.57	1993-03-25	3-MEDIUM       	Clerk#000000233	0	lites haggle. ironic, ironic instructions maintain blit	1993
5831	139	O	113505.19	1996-11-17	5-LOW          	Clerk#000000585	0	s final, final pinto beans. unusual depos	1996
5856	37	F	71460.49	1994-11-06	2-HIGH         	Clerk#000000634	0	special excuses. slyly final theodolites cajole blithely furiou	1994
5858	64	F	181320.50	1992-07-14	4-NOT SPECIFIED	Clerk#000000580	0	lyly pending dugouts believe through the ironic deposits. silent s	1992
5859	5	O	210643.96	1997-04-23	1-URGENT       	Clerk#000000993	0	requests boost. asymptotes across the deposits solve slyly furiously pendin	1997
5860	13	F	9495.28	1992-02-20	4-NOT SPECIFIED	Clerk#000000079	0	 beans. bold, special foxes sleep about the ir	1992
5861	139	O	41450.19	1997-04-10	3-MEDIUM       	Clerk#000000094	0	rthogs cajole slyly. express packages sleep blithely final 	1997
5862	64	O	30550.90	1997-02-20	1-URGENT       	Clerk#000000039	0	leep beneath the quickly busy excuses. ironic theodolit	1997
5888	46	O	67167.19	1996-09-28	3-MEDIUM       	Clerk#000000748	0	quickly against the furiously final requests. evenly fi	1996
5889	22	O	15417.57	1995-05-23	5-LOW          	Clerk#000000690	0	ites wake across the slyly ironic	1995
5890	49	F	41162.24	1992-11-04	2-HIGH         	Clerk#000000013	0	packages. final, final reques	1992
5891	46	F	41760.00	1992-12-29	3-MEDIUM       	Clerk#000000302	0	ounts haggle furiously abo	1992
5892	101	P	92340.77	1995-05-09	5-LOW          	Clerk#000000639	0	 pending instruction	1995
5894	71	F	70377.31	1994-08-13	2-HIGH         	Clerk#000000776	0	regular deposits wake	1994
5895	64	O	201419.83	1997-01-01	4-NOT SPECIFIED	Clerk#000000747	0	 ironic, unusual requests cajole blithely special, special deposits. s	1997
5920	119	F	142767.26	1994-11-20	2-HIGH         	Clerk#000000081	0	ns: even ideas cajole slyly among the packages. never ironic patterns	1994
5923	101	O	157968.27	1997-05-27	2-HIGH         	Clerk#000000304	0	o beans haggle slyly above the regular, even dependencies	1997
5925	146	O	242588.87	1995-11-13	5-LOW          	Clerk#000000602	0	ourts. boldly regular foxes might sleep. slyly express tithes against 	1995
5926	76	F	105770.53	1994-05-20	5-LOW          	Clerk#000000071	0	 carefully after the furiously even re	1994
5952	148	O	128624.99	1997-04-14	3-MEDIUM       	Clerk#000000950	0	 regular, final pla	1997
5954	28	F	167262.34	1992-12-03	1-URGENT       	Clerk#000000968	0	requests along the blith	1992
5955	94	P	67944.38	1995-03-27	5-LOW          	Clerk#000000340	0	deas integrate. fluffily regular pa	1995
5956	22	O	118036.54	1998-05-18	1-URGENT       	Clerk#000000587	0	le even, express platelets.	1998
5958	115	O	145060.41	1995-09-16	3-MEDIUM       	Clerk#000000787	0	e final requests detect alongside of the qu	1995
5959	23	F	195515.26	1992-05-15	3-MEDIUM       	Clerk#000000913	0	into beans use ironic, unusual foxes. carefully regular excuses boost caref	1992
5986	115	F	92187.80	1992-04-22	2-HIGH         	Clerk#000000674	0	iously unusual notornis are 	1992
5987	64	O	98956.82	1996-08-03	1-URGENT       	Clerk#000000464	0	 ideas. quietly final accounts haggle blithely pending escapade	1996
\.


--
-- Data for Name: part; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.part (p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment) FROM stdin;
1	goldenrod lavender spring chocolate lace	Manufacturer#1           	Brand#13  	PROMO BURNISHED COPPER	7	JUMBO PKG 	901.00	ly. slyly ironi
2	blush thistle blue yellow saddle	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	1	LG CASE   	902.00	lar accounts amo
3	spring green yellow purple cornsilk	Manufacturer#4           	Brand#42  	STANDARD POLISHED BRASS	21	WRAP CASE 	903.00	egular deposits hag
4	cornflower chocolate smoke green pink	Manufacturer#3           	Brand#34  	SMALL PLATED BRASS	14	MED DRUM  	904.00	p furiously r
5	forest brown coral puff cream	Manufacturer#3           	Brand#32  	STANDARD POLISHED TIN	15	SM PKG    	905.00	 wake carefully 
6	bisque cornflower lawn forest magenta	Manufacturer#2           	Brand#24  	PROMO PLATED STEEL	4	MED BAG   	906.00	sual a
7	moccasin green thistle khaki floral	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	45	SM BAG    	907.00	lyly. ex
8	misty lace thistle snow royal	Manufacturer#4           	Brand#44  	PROMO BURNISHED TIN	41	LG DRUM   	908.00	eposi
9	thistle dim navajo dark gainsboro	Manufacturer#4           	Brand#43  	SMALL BURNISHED STEEL	12	WRAP CASE 	909.00	ironic foxe
10	linen pink saddle puff powder	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	44	LG CAN    	910.01	ithely final deposit
11	spring maroon seashell almond orchid	Manufacturer#2           	Brand#25  	STANDARD BURNISHED NICKEL	43	WRAP BOX  	911.01	ng gr
12	cornflower wheat orange maroon ghost	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED STEEL	25	JUMBO CASE	912.01	 quickly
13	ghost olive orange rosy thistle	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED NICKEL	1	JUMBO PACK	913.01	osits.
14	khaki seashell rose cornsilk navajo	Manufacturer#1           	Brand#13  	SMALL POLISHED STEEL	28	JUMBO BOX 	914.01	kages c
15	blanched honeydew sky turquoise medium	Manufacturer#1           	Brand#15  	LARGE ANODIZED BRASS	45	LG CASE   	915.01	usual ac
16	deep sky turquoise drab peach	Manufacturer#3           	Brand#32  	PROMO PLATED TIN	2	MED PACK  	916.01	unts a
17	indian navy coral pink deep	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED STEEL	16	LG BOX    	917.01	 regular accounts
18	turquoise indian lemon lavender misty	Manufacturer#1           	Brand#11  	SMALL BURNISHED STEEL	42	JUMBO PACK	918.01	s cajole slyly a
19	chocolate navy tan deep brown	Manufacturer#2           	Brand#23  	SMALL ANODIZED NICKEL	33	WRAP BOX  	919.01	 pending acc
20	ivory navy honeydew sandy midnight	Manufacturer#1           	Brand#12  	LARGE POLISHED NICKEL	48	MED BAG   	920.02	are across the asympt
21	lemon floral azure frosted lime	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	31	MED BAG   	921.02	ss packages. pendin
22	medium forest blue ghost black	Manufacturer#4           	Brand#43  	PROMO POLISHED BRASS	19	LG DRUM   	922.02	 even p
23	coral lavender seashell rosy burlywood	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED TIN	42	JUMBO JAR 	923.02	nic, fina
24	seashell coral metallic midnight floral	Manufacturer#5           	Brand#52  	MEDIUM PLATED STEEL	20	MED CASE  	924.02	 final the
25	aquamarine steel firebrick light turquoise	Manufacturer#5           	Brand#55  	STANDARD BRUSHED COPPER	3	JUMBO BAG 	925.02	requests wake
26	beige frosted moccasin chocolate snow	Manufacturer#3           	Brand#32  	SMALL BRUSHED STEEL	32	SM CASE   	926.02	 instructions i
27	saddle puff beige linen yellow	Manufacturer#1           	Brand#14  	LARGE ANODIZED TIN	20	MED PKG   	927.02	s wake. ir
28	navajo yellow drab white misty	Manufacturer#4           	Brand#44  	SMALL PLATED COPPER	19	JUMBO PKG 	928.02	x-ray pending, iron
29	lemon sky grey salmon orchid	Manufacturer#3           	Brand#33  	PROMO PLATED COPPER	7	LG DRUM   	929.02	 carefully fluffi
30	cream misty steel spring medium	Manufacturer#4           	Brand#42  	PROMO ANODIZED TIN	17	LG BOX    	930.03	carefully bus
31	slate seashell steel medium moccasin	Manufacturer#5           	Brand#53  	STANDARD BRUSHED TIN	10	LG BAG    	931.03	uriously s
32	sandy wheat coral spring burnished	Manufacturer#4           	Brand#42  	ECONOMY PLATED BRASS	31	LG CASE   	932.03	urts. carefully fin
33	spring bisque salmon slate pink	Manufacturer#2           	Brand#22  	ECONOMY PLATED NICKEL	16	LG PKG    	933.03	ly eve
34	khaki steel rose ghost salmon	Manufacturer#1           	Brand#13  	LARGE BRUSHED STEEL	8	JUMBO BOX 	934.03	riously ironic
35	green blush tomato burlywood seashell	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED BRASS	14	JUMBO PACK	935.03	e carefully furi
36	chiffon tan forest moccasin dark	Manufacturer#2           	Brand#25  	SMALL BURNISHED COPPER	3	JUMBO CAN 	936.03	olites o
37	royal coral orange burnished navajo	Manufacturer#4           	Brand#45  	LARGE POLISHED TIN	48	JUMBO BOX 	937.03	silent 
38	seashell papaya white mint brown	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED BRASS	11	SM JAR    	938.03	structions inte
39	rose medium floral salmon powder	Manufacturer#5           	Brand#53  	SMALL POLISHED TIN	43	JUMBO JAR 	939.03	se slowly above the fl
40	lemon midnight metallic sienna steel	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED COPPER	27	SM CASE   	940.04	! blithely specia
41	burlywood goldenrod pink peru sienna	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED TIN	7	WRAP JAR  	941.04	uriously. furiously cl
42	midnight turquoise lawn beige thistle	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED TIN	45	LG BOX    	942.04	the slow
43	medium lace midnight royal chartreuse	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	5	WRAP CASE 	943.04	e slyly along the ir
44	saddle cream wheat lemon burnished	Manufacturer#4           	Brand#45  	MEDIUM PLATED TIN	48	SM PACK   	944.04	pinto beans. carefully
45	lawn peru ghost khaki maroon	Manufacturer#4           	Brand#43  	SMALL BRUSHED NICKEL	9	WRAP BAG  	945.04	nts bo
46	honeydew turquoise aquamarine spring tan	Manufacturer#1           	Brand#11  	STANDARD POLISHED TIN	45	WRAP CASE 	946.04	the blithely unusual 
47	honeydew red azure magenta brown	Manufacturer#4           	Brand#45  	LARGE BURNISHED BRASS	14	JUMBO PACK	947.04	 even plate
48	slate thistle cornsilk pale forest	Manufacturer#5           	Brand#53  	STANDARD BRUSHED STEEL	27	JUMBO CASE	948.04	ng to the depo
49	light firebrick cyan puff blue	Manufacturer#2           	Brand#24  	SMALL BURNISHED TIN	31	MED DRUM  	949.04	ar pack
50	linen blanched tomato slate medium	Manufacturer#3           	Brand#33  	LARGE ANODIZED TIN	25	WRAP PKG  	950.05	kages m
51	lime frosted indian dodger linen	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED NICKEL	34	JUMBO PACK	951.05	n foxes
52	lemon midnight lace sky deep	Manufacturer#3           	Brand#35  	STANDARD BURNISHED TIN	25	WRAP CASE 	952.05	 final deposits. fu
53	bisque rose cornsilk seashell purple	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	32	MED BAG   	953.05	mptot
54	blanched mint yellow papaya cyan	Manufacturer#2           	Brand#21  	LARGE BURNISHED COPPER	19	WRAP CASE 	954.05	e blithely
55	sky cream deep tomato rosy	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED COPPER	9	MED BAG   	955.05	ly final pac
56	antique beige brown deep dodger	Manufacturer#1           	Brand#12  	MEDIUM PLATED STEEL	20	WRAP DRUM 	956.05	ts. blithel
57	purple blue light sienna deep	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED BRASS	49	MED PKG   	957.05	lly abov
58	linen hot cornsilk drab bisque	Manufacturer#5           	Brand#53  	STANDARD POLISHED TIN	44	LG PACK   	958.05	 fluffily blithely reg
59	misty brown medium mint salmon	Manufacturer#5           	Brand#53  	MEDIUM POLISHED TIN	2	LG BAG    	959.05	regular exc
60	snow spring sandy olive tomato	Manufacturer#1           	Brand#11  	LARGE POLISHED COPPER	27	JUMBO CASE	960.06	 integ
61	light tan linen tomato peach	Manufacturer#5           	Brand#54  	SMALL BURNISHED NICKEL	18	WRAP DRUM 	961.06	es. blithely en
62	tan cornsilk spring grey chocolate	Manufacturer#3           	Brand#35  	STANDARD BRUSHED BRASS	39	JUMBO BOX 	962.06	ckly across the carefu
63	burnished puff coral light papaya	Manufacturer#3           	Brand#32  	STANDARD BURNISHED NICKEL	10	JUMBO CAN 	963.06	 quickly 
64	aquamarine coral lemon ivory gainsboro	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED BRASS	1	JUMBO CAN 	964.06	efully regular pi
65	slate drab medium puff gainsboro	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED COPPER	3	MED CAN   	965.06	posits after the quic
66	cornflower pale almond lemon linen	Manufacturer#3           	Brand#35  	PROMO ANODIZED NICKEL	46	SM CASE   	966.06	haggle blithely iro
67	slate salmon rose spring seashell	Manufacturer#2           	Brand#21  	SMALL BRUSHED TIN	31	WRAP DRUM 	967.06	 regular, p
68	bisque ivory mint purple almond	Manufacturer#1           	Brand#11  	PROMO ANODIZED STEEL	10	WRAP BOX  	968.06	eposits shall h
69	lace burnished rosy antique metallic	Manufacturer#5           	Brand#52  	MEDIUM POLISHED BRASS	2	SM BOX    	969.06	ely final depo
70	violet seashell firebrick dark navajo	Manufacturer#1           	Brand#11  	STANDARD BRUSHED STEEL	42	LG PACK   	970.07	inal gifts. sl
71	violet firebrick cream peru white	Manufacturer#3           	Brand#33  	STANDARD PLATED BRASS	26	WRAP DRUM 	971.07	 packages alongside
72	hot spring yellow azure dodger	Manufacturer#2           	Brand#23  	STANDARD ANODIZED TIN	25	JUMBO PACK	972.07	efully final the
73	cream moccasin royal dim chiffon	Manufacturer#2           	Brand#21  	SMALL BRUSHED COPPER	35	WRAP DRUM 	973.07	ts haggl
74	frosted grey aquamarine thistle papaya	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED BRASS	25	JUMBO CASE	974.07	ent foxes
75	aquamarine maroon wheat salmon metallic	Manufacturer#3           	Brand#35  	SMALL BURNISHED NICKEL	39	SM JAR    	975.07	s sleep furiou
76	rosy light lime puff sandy	Manufacturer#3           	Brand#34  	MEDIUM BRUSHED COPPER	9	SM PKG    	976.07	n accounts sleep qu
77	mint bisque chiffon snow firebrick	Manufacturer#5           	Brand#52  	STANDARD BRUSHED COPPER	13	MED PKG   	977.07	uests.
78	blush forest slate seashell puff	Manufacturer#1           	Brand#14  	ECONOMY POLISHED STEEL	24	LG JAR    	978.07	icing deposits wake
79	gainsboro pink grey tan almond	Manufacturer#4           	Brand#45  	PROMO ANODIZED BRASS	22	JUMBO BAG 	979.07	 foxes are slyly regu
80	tomato chartreuse coral turquoise linen	Manufacturer#4           	Brand#44  	PROMO PLATED BRASS	28	MED CAN   	980.08	unusual dependencies i
81	misty sandy cornsilk dodger blush	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED TIN	21	MED BAG   	981.08	ove the furiou
82	khaki tomato purple almond tan	Manufacturer#1           	Brand#15  	ECONOMY POLISHED TIN	12	WRAP BOX  	982.08	ial requests haggle 
83	blush green dim lawn peru	Manufacturer#1           	Brand#12  	PROMO BURNISHED NICKEL	47	SM CAN    	983.08	ly regul
84	salmon floral cream rose dark	Manufacturer#4           	Brand#45  	SMALL ANODIZED NICKEL	26	JUMBO PACK	984.08	ideas nag
85	dim deep aquamarine smoke pale	Manufacturer#5           	Brand#55  	PROMO ANODIZED NICKEL	16	LG BAG    	985.08	 silent
86	green blanched firebrick dim cream	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	37	LG CASE   	986.08	 daring sheaves 
87	purple lace seashell antique orange	Manufacturer#4           	Brand#41  	LARGE PLATED STEEL	41	WRAP PACK 	987.08	yly final
88	lime orange bisque chartreuse lemon	Manufacturer#4           	Brand#44  	PROMO PLATED COPPER	16	SM CASE   	988.08	e regular packages. 
89	ghost lace lemon sienna saddle	Manufacturer#5           	Brand#53  	STANDARD BURNISHED STEEL	7	MED JAR   	989.08	y final pinto 
90	hot rosy violet plum pale	Manufacturer#5           	Brand#51  	ECONOMY POLISHED STEEL	49	JUMBO CAN 	990.09	caref
91	misty bisque lavender spring turquoise	Manufacturer#2           	Brand#21  	STANDARD BRUSHED TIN	32	JUMBO PKG 	991.09	counts dete
92	blush magenta ghost tomato rose	Manufacturer#2           	Brand#22  	STANDARD ANODIZED TIN	35	JUMBO PKG 	992.09	he ironic accounts. sp
93	pale yellow cornsilk dodger moccasin	Manufacturer#2           	Brand#24  	LARGE ANODIZED TIN	2	WRAP DRUM 	993.09	 platel
94	blanched pink frosted mint snow	Manufacturer#3           	Brand#35  	STANDARD POLISHED BRASS	32	SM BOX    	994.09	s accounts cajo
95	dodger beige wheat orchid navy	Manufacturer#3           	Brand#33  	LARGE BRUSHED TIN	36	WRAP DRUM 	995.09	 final pinto beans 
96	chocolate light firebrick rose indian	Manufacturer#5           	Brand#53  	STANDARD BRUSHED STEEL	32	SM CASE   	996.09	ng to the bli
97	coral dodger beige black chartreuse	Manufacturer#3           	Brand#33  	MEDIUM POLISHED BRASS	49	WRAP CAN  	997.09	ss excuses sleep am
98	frosted peru chiffon yellow aquamarine	Manufacturer#5           	Brand#54  	STANDARD ANODIZED BRASS	22	MED JAR   	998.09	e the q
99	mint grey purple sienna metallic	Manufacturer#2           	Brand#21  	SMALL BURNISHED STEEL	11	JUMBO PKG 	999.09	press
100	cyan orchid indian cornflower saddle	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED TIN	4	LG BAG    	1000.10	of the steal
101	powder deep lavender violet gainsboro	Manufacturer#3           	Brand#32  	LARGE ANODIZED STEEL	26	JUMBO JAR 	1001.10	ly even,
102	papaya maroon blush powder sky	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED BRASS	17	SM DRUM   	1002.10	ular packa
103	navy sky spring orchid forest	Manufacturer#2           	Brand#25  	MEDIUM PLATED BRASS	45	WRAP DRUM 	1003.10	e blithely blith
104	plum cyan cornflower midnight royal	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED STEEL	36	JUMBO BAG 	1004.10	ites sleep quickly
105	dodger slate pale mint navajo	Manufacturer#1           	Brand#15  	SMALL POLISHED COPPER	27	LG DRUM   	1005.10	odolites was 
106	cornsilk bisque seashell lemon frosted	Manufacturer#3           	Brand#31  	MEDIUM PLATED BRASS	28	WRAP DRUM 	1006.10	unts maintain 
107	violet honeydew bisque sienna orchid	Manufacturer#5           	Brand#53  	SMALL BURNISHED TIN	12	MED BOX   	1007.10	slyly special depos
108	bisque peach magenta tomato yellow	Manufacturer#1           	Brand#12  	PROMO PLATED NICKEL	41	MED PKG   	1008.10	after the carefully 
109	lemon black indian cornflower pale	Manufacturer#3           	Brand#33  	ECONOMY POLISHED TIN	11	LG PACK   	1009.10	instruction
110	firebrick navy rose beige black	Manufacturer#3           	Brand#33  	STANDARD BURNISHED COPPER	46	LG DRUM   	1010.11	t quickly a
111	orange cornflower mint snow peach	Manufacturer#5           	Brand#54  	LARGE BRUSHED COPPER	28	JUMBO JAR 	1011.11	kly bold epitaphs 
112	hot aquamarine tomato lace indian	Manufacturer#4           	Brand#43  	PROMO BRUSHED STEEL	42	JUMBO CAN 	1012.11	the express, 
113	almond seashell azure blanched light	Manufacturer#3           	Brand#31  	PROMO POLISHED TIN	23	LG CAN    	1013.11	finally even 
114	pink black blanched lace chartreuse	Manufacturer#5           	Brand#51  	MEDIUM POLISHED NICKEL	41	MED PACK  	1014.11	ully final foxes. pint
115	spring chiffon cream orchid dodger	Manufacturer#4           	Brand#45  	STANDARD POLISHED STEEL	24	MED CAN   	1015.11	counts nag! caref
116	goldenrod black slate forest red	Manufacturer#5           	Brand#53  	PROMO POLISHED NICKEL	33	SM PACK   	1016.11	usly final courts 
117	tomato honeydew pale red yellow	Manufacturer#1           	Brand#14  	SMALL BRUSHED TIN	25	LG BAG    	1017.11	ages acc
118	ghost plum brown coral cornsilk	Manufacturer#2           	Brand#25  	PROMO ANODIZED TIN	31	MED PACK  	1018.11	ly ironic pinto
119	olive metallic slate peach green	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	30	WRAP CASE 	1019.11	out the quickly r
120	pink powder mint moccasin navajo	Manufacturer#1           	Brand#14  	SMALL ANODIZED NICKEL	45	WRAP JAR  	1020.12	lly a
121	bisque royal goldenrod medium thistle	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED COPPER	13	SM PKG    	1021.12	deposi
122	gainsboro royal forest dark lace	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED TIN	8	LG DRUM   	1022.12	sts c
123	deep dim peach light beige	Manufacturer#1           	Brand#12  	SMALL BURNISHED TIN	31	JUMBO PKG 	1023.12	ray regula
124	wheat blush forest metallic navajo	Manufacturer#3           	Brand#32  	PROMO ANODIZED STEEL	1	LG BOX    	1024.12	g the expr
125	mint ivory saddle peach midnight	Manufacturer#1           	Brand#12  	STANDARD BRUSHED BRASS	17	WRAP BAG  	1025.12	kages against
126	burnished black blue metallic orchid	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED NICKEL	4	LG BAG    	1026.12	es sleep al
127	royal coral orchid spring sky	Manufacturer#5           	Brand#52  	SMALL BURNISHED NICKEL	14	LG JAR    	1027.12	lithely expr
128	dark burlywood burnished snow sky	Manufacturer#2           	Brand#22  	PROMO PLATED TIN	5	SM BAG    	1028.12	e of the furiously ex
129	grey spring chiffon thistle lime	Manufacturer#1           	Brand#15  	LARGE POLISHED TIN	20	SM JAR    	1029.12	 careful
130	gainsboro powder cyan pale rosy	Manufacturer#2           	Brand#23  	SMALL PLATED NICKEL	26	LG BOX    	1030.13	ake slyly
131	tomato moccasin cyan brown goldenrod	Manufacturer#5           	Brand#52  	STANDARD ANODIZED BRASS	43	MED DRUM  	1031.13	nts wake dar
132	seashell papaya tomato lime hot	Manufacturer#4           	Brand#45  	STANDARD BURNISHED BRASS	2	WRAP DRUM 	1032.13	ckly expre
133	firebrick black dodger pink salmon	Manufacturer#1           	Brand#13  	SMALL BRUSHED NICKEL	19	LG PKG    	1033.13	 final pinto beans
134	steel beige mint maroon indian	Manufacturer#4           	Brand#42  	SMALL POLISHED STEEL	35	SM PKG    	1034.13	es. bold pa
135	thistle chocolate ghost gainsboro peru	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED STEEL	24	JUMBO CASE	1035.13	l frets 
136	cornsilk maroon blanched thistle rosy	Manufacturer#2           	Brand#22  	SMALL PLATED STEEL	2	WRAP BAG  	1036.13	kages print carefully
137	cornsilk drab ghost sandy royal	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	25	MED PACK  	1037.13	the t
138	dark aquamarine tomato medium puff	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED COPPER	42	JUMBO DRUM	1038.13	ts solve acro
139	floral steel burlywood navy cream	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED STEEL	7	SM BOX    	1039.13	ter t
140	aquamarine lavender maroon slate hot	Manufacturer#5           	Brand#53  	STANDARD PLATED STEEL	45	SM BOX    	1040.14	oss the carefu
141	honeydew magenta tomato spring medium	Manufacturer#3           	Brand#35  	STANDARD ANODIZED STEEL	23	SM PKG    	1041.14	ans nag furiously pen
142	chartreuse linen grey slate saddle	Manufacturer#5           	Brand#55  	STANDARD ANODIZED BRASS	36	MED JAR   	1042.14	he accounts. pac
143	bisque dodger blanched steel maroon	Manufacturer#3           	Brand#34  	ECONOMY PLATED TIN	44	MED BAG   	1043.14	nts across the
144	hot midnight orchid dim steel	Manufacturer#1           	Brand#14  	SMALL ANODIZED TIN	26	SM BOX    	1044.14	owly 
145	navajo lavender chocolate deep hot	Manufacturer#5           	Brand#53  	PROMO BRUSHED COPPER	24	SM BAG    	1045.14	es wake furiously blit
146	azure smoke mint cream burlywood	Manufacturer#3           	Brand#34  	STANDARD BRUSHED COPPER	11	WRAP PACK 	1046.14	unts cajole
147	honeydew orange dodger linen lace	Manufacturer#1           	Brand#11  	MEDIUM PLATED COPPER	29	JUMBO PKG 	1047.14	wake never bold 
148	yellow white ghost lavender salmon	Manufacturer#3           	Brand#31  	STANDARD PLATED STEEL	20	SM BOX    	1048.14	platelets wake fu
149	tan thistle frosted indian lawn	Manufacturer#2           	Brand#24  	MEDIUM BURNISHED NICKEL	6	MED PKG   	1049.14	leep requests. dog
150	pale rose navajo firebrick aquamarine	Manufacturer#3           	Brand#35  	LARGE BRUSHED TIN	21	SM BAG    	1050.15	ironic foxes
151	chartreuse linen violet ghost thistle	Manufacturer#3           	Brand#34  	LARGE PLATED BRASS	45	MED CAN   	1051.15	ccounts nag i
152	white sky antique tomato chartreuse	Manufacturer#5           	Brand#53  	MEDIUM POLISHED STEEL	48	MED CASE  	1052.15	thely regular t
153	linen frosted slate coral peru	Manufacturer#1           	Brand#11  	STANDARD PLATED TIN	20	MED BAG   	1053.15	thlessly. silen
154	peru moccasin peach pale spring	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED TIN	1	JUMBO BAG 	1054.15	posits 
155	puff yellow cyan tomato purple	Manufacturer#2           	Brand#21  	SMALL BRUSHED NICKEL	28	WRAP CASE 	1055.15	lly ironic, r
156	almond ghost powder blush forest	Manufacturer#4           	Brand#43  	SMALL POLISHED NICKEL	2	LG PKG    	1056.15	 pinto beans. eve
157	navajo linen coral brown forest	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED STEEL	26	JUMBO PACK	1057.15	ial courts. ru
158	magenta light misty navy honeydew	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED COPPER	47	LG JAR    	1058.15	 ideas detect slyl
159	white orange antique beige aquamarine	Manufacturer#4           	Brand#43  	SMALL ANODIZED BRASS	46	SM BAG    	1059.15	 ironic requests-- pe
160	frosted cornflower khaki salmon metallic	Manufacturer#5           	Brand#55  	STANDARD POLISHED COPPER	47	JUMBO CAN 	1060.16	nts are carefully
161	metallic khaki navy forest cyan	Manufacturer#2           	Brand#22  	STANDARD PLATED TIN	17	SM PACK   	1061.16	r the bl
162	burlywood cornflower aquamarine misty snow	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED COPPER	35	JUMBO PACK	1062.16	e slyly around th
163	blush metallic maroon lawn forest	Manufacturer#2           	Brand#21  	ECONOMY PLATED TIN	34	WRAP DRUM 	1063.16	nly s
164	orange cyan magenta navajo indian	Manufacturer#2           	Brand#23  	LARGE PLATED BRASS	35	JUMBO BAG 	1064.16	mong th
165	white dim cornflower sky seashell	Manufacturer#1           	Brand#15  	STANDARD PLATED STEEL	24	SM CAN    	1065.16	 carefully fin
166	linen bisque tomato gainsboro goldenrod	Manufacturer#5           	Brand#52  	LARGE POLISHED COPPER	4	MED BAG   	1066.16	ss the
167	almond floral grey dim sky	Manufacturer#3           	Brand#32  	LARGE ANODIZED STEEL	46	WRAP BOX  	1067.16	ic ac
168	lace gainsboro burlywood smoke tomato	Manufacturer#1           	Brand#13  	SMALL BRUSHED COPPER	20	JUMBO DRUM	1068.16	ss package
169	bisque misty sky cornflower peach	Manufacturer#5           	Brand#55  	STANDARD POLISHED BRASS	10	JUMBO CASE	1069.16	lets alongside of
170	peru grey blanched goldenrod yellow	Manufacturer#3           	Brand#33  	LARGE POLISHED COPPER	28	LG DRUM   	1070.17	yly s
171	beige violet black magenta chartreuse	Manufacturer#1           	Brand#11  	STANDARD BURNISHED COPPER	40	LG JAR    	1071.17	 the r
172	medium goldenrod linen sky coral	Manufacturer#5           	Brand#53  	PROMO PLATED NICKEL	28	MED CASE  	1072.17	quick as
173	chartreuse seashell powder navy grey	Manufacturer#1           	Brand#12  	ECONOMY BURNISHED TIN	17	LG CASE   	1073.17	sly bold excuses haggl
174	hot cornflower slate saddle pale	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED COPPER	25	LG CASE   	1074.17	 accounts nag ab
175	magenta blue chartreuse tan green	Manufacturer#1           	Brand#11  	PROMO ANODIZED TIN	45	JUMBO JAR 	1075.17	ole against the
176	pink drab ivory papaya grey	Manufacturer#2           	Brand#24  	SMALL ANODIZED STEEL	40	MED CAN   	1076.17	blithely. ironic
177	indian turquoise purple green spring	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED STEEL	42	LG BAG    	1077.17	ermanently eve
178	lace blanched magenta yellow almond	Manufacturer#1           	Brand#13  	STANDARD POLISHED TIN	10	LG JAR    	1078.17	regular instructions.
179	deep puff brown blue burlywood	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED STEEL	20	LG JAR    	1079.17	ely regul
180	seashell maroon lace burnished lavender	Manufacturer#3           	Brand#33  	STANDARD BURNISHED NICKEL	7	WRAP BAG  	1080.18	oss the 
181	antique plum smoke pink dodger	Manufacturer#2           	Brand#24  	MEDIUM PLATED STEEL	19	WRAP CAN  	1081.18	al deposits 
182	beige cyan burlywood chiffon light	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED COPPER	11	JUMBO CAN 	1082.18	bits are 
183	ivory white burnished papaya cornflower	Manufacturer#5           	Brand#52  	PROMO POLISHED STEEL	35	LG PKG    	1083.18	ly regular excus
184	ghost honeydew cyan lawn powder	Manufacturer#5           	Brand#53  	SMALL POLISHED TIN	42	LG BOX    	1084.18	ding courts. idly iro
185	firebrick black ivory spring medium	Manufacturer#4           	Brand#44  	ECONOMY POLISHED TIN	4	WRAP BAG  	1085.18	even foxe
186	grey purple chocolate turquoise plum	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED TIN	15	JUMBO PKG 	1086.18	ly reg
187	white red lace deep pale	Manufacturer#4           	Brand#45  	PROMO ANODIZED BRASS	45	MED CAN   	1087.18	leep slyly s
188	moccasin steel rosy drab white	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED BRASS	9	MED CAN   	1088.18	 above the silent p
189	dodger moccasin lemon purple thistle	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED BRASS	13	WRAP DRUM 	1089.18	en requests. sauternes
190	chartreuse goldenrod midnight cornflower blush	Manufacturer#5           	Brand#53  	LARGE BURNISHED NICKEL	23	WRAP BAG  	1090.19	 furiously even d
191	mint midnight puff forest peach	Manufacturer#3           	Brand#31  	MEDIUM POLISHED BRASS	36	WRAP BOX  	1091.19	 asymptote
192	thistle puff pink cream orange	Manufacturer#3           	Brand#34  	STANDARD BRUSHED COPPER	17	MED BAG   	1092.19	uickly regular, expr
193	turquoise lime royal metallic azure	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED BRASS	31	SM PKG    	1093.19	final ideas wake furi
194	brown black cream navy plum	Manufacturer#5           	Brand#51  	ECONOMY POLISHED STEEL	7	SM CAN    	1094.19	y special accoun
195	bisque sienna hot goldenrod khaki	Manufacturer#4           	Brand#41  	STANDARD BRUSHED NICKEL	40	MED CASE  	1095.19	oxes sleep care
196	pale peru linen hot maroon	Manufacturer#3           	Brand#33  	SMALL BURNISHED NICKEL	3	JUMBO JAR 	1096.19	uickly special 
197	lawn lemon khaki rosy blue	Manufacturer#5           	Brand#52  	SMALL ANODIZED COPPER	18	SM JAR    	1097.19	lithely after the eve
198	orange cornflower indian aquamarine white	Manufacturer#4           	Brand#41  	PROMO BRUSHED NICKEL	43	SM PACK   	1098.19	ackages? carefully re
199	ivory slate lavender tan royal	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	23	JUMBO DRUM	1099.19	ickly regul
200	peach cornsilk navy rosy red	Manufacturer#5           	Brand#54  	MEDIUM POLISHED BRASS	22	LG PKG    	1100.20	furiously even depo
\.


--
-- Data for Name: partsupp; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.partsupp (r, ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment) FROM stdin;
1	1	2	3325	771.64	, even theodolites. regular, final theodolites eat after the carefully pending foxes. furiously regular deposits sleep slyly. carefully bold realms above the ironic dependencies haggle careful
1	2	3	8895	378.49	nic accounts. final accounts sleep furiously about the ironic, bold packages. regular, regular accounts
1	2	5	4969	915.27	ptotes. quickly pending dependencies integrate furiously. fluffily ironic ideas impress blithely above the express accounts. furiously even epitaphs need to wak
1	2	9	3025	306.39	olites. deposits wake carefully. even, express requests cajole. carefully regular ex
1	4	5	1339	113.97	 carefully unusual ideas. packages use slyly. blithely final pinto beans cajole along the furiously express requests. regular orbits haggle carefully. care
1	4	9	2694	51.37	g, regular deposits: quick instructions run across the carefully ironic theodolites-- final dependencies haggle into the dependencies. f
1	5	2	6925	537.98	sits. quickly fluffy packages wake quickly beyond the blithely regular requests. pending requests cajole among the final pinto beans. carefully busy theodolites affix quickly stealthily 
1	6	3	6451	175.32	 accounts alongside of the slyly even accounts wake carefully final instructions-- ruthless platelets wake carefully ideas. even deposits are quickly final,
1	6	9	1627	424.25	 quick packages. ironic deposits print. furiously silent platelets across the carefully final requests are slyly along the furiously even instructi
1	7	2	3377	68.77	usly against the daring asymptotes. slyly regular platelets sleep quickly blithely regular deposits. boldly regular deposits wake blithely ironic accounts
1	8	3	9845	220.62	s against the fluffily special packages snooze slyly slyly regular p
1	8	5	8126	916.91	final accounts around the blithely special asymptotes wake carefully beyond the bold dugouts. regular ideas haggle furiously after
1	8	9	6834	249.63	lly ironic accounts solve express, unusual theodolites. special packages use quickly. quickly fin
1	9	2	7542	811.84	ate after the final pinto beans. express requests cajole express packages. carefully bold ideas haggle furiously. blithely express accounts eat carefully among the evenly busy accounts. carefully un
1	10	3	3335	673.27	s theodolites haggle according to the fluffily unusual instructions. silent realms nag carefully ironic theodolites. furiously unusual instructions would detect fu
1	10	5	5691	164.00	r, silent instructions sleep slyly regular pinto beans. furiously unusual gifts use. silently ironic theodolites cajole final deposits! express dugouts are furiously. packages sleep 
1	11	2	4540	709.87	thely across the blithely unusual requests. slyly regular instructions wake slyly ironic theodolites. requests haggle blithely above the blithely brave p
1	11	5	4729	894.90	ters wake. sometimes bold packages cajole sometimes blithely final instructions. carefully ironic foxes after the furiously unusual foxes cajole carefully acr
1	12	2	5454	901.70	s across the carefully regular courts haggle fluffily among the even theodolites. blithely final platelets x-ray even ideas. fluffily express pinto beans sleep slyly. carefully even a
1	12	3	3610	659.73	jole bold theodolites. final packages haggle! carefully regular deposits play furiously among the special ideas. quickly ironic packages detect quickly carefully final
1	12	9	824	337.06	es are unusual deposits. fluffily even deposits across the blithely final theodolites doubt across the unusual accounts. regular, 
1	13	3	9736	327.18	tect after the express instructions. furiously silent ideas sleep blithely special ideas. attainments sleep furiously. carefully bold requests ab
1	14	5	5278	650.07	e quickly among the furiously ironic accounts. special, final sheaves against the
1	15	2	3316	265.89	e express instructions. ironic requests haggle fluffily along the carefully even packages. furiously final acco
1	15	5	5255	458.67	refully bold instructions among the silent grouches must boost against the express deposits:
1	15	9	3336	784.55	slyly. fluffily bold accounts cajole furiously. furiously regular dependencies wak
1	16	3	854	781.91	 unusual excuses. requests after the carefully regular pinto 
1	18	2	8132	52.44	 final packages wake quickly across the blithely ironic instructions. regular pains integrate slyly across the deposits. carefully regular pinto beans among the close
1	18	5	3133	568.61	riously bold accounts. packages boost daringly. blithely regular requests cajole. regular foxes wake carefully final accounts. blithely unusual excuses det
1	18	9	1125	664.17	. ironic, regular accounts across the furiously express 
1	19	3	5467	405.70	nstructions use furiously. fluffily regular excuses wake. slyly special grouches are carefully regular Tiresias. regular requests use about the quickly furio
1	19	9	1340	346.92	 requests. final, pending realms use carefully; slyly dogged foxes impress fluffily above the blithely regular deposits. ironic, regular courts wake carefully. bold requests impress
1	21	2	6571	944.44	ing instructions impress bold foxes. ironic pinto beans use. thinly even asymptotes cajole ironic packages. quickly ironic pinto beans detect slyly regular deposits. ruthlessly even deposits are. sl
1	22	3	4410	786.18	even accounts. final excuses try to sleep regular, even packages. carefully express dolphins cajole; furiously special pinto bea
1	22	5	1434	597.21	ix across the blithely express packages. carefully regular pinto beans boost across the special, pending d
1	23	2	9898	233.94	l, express packages wake permanently. quickly even deposits sleep quickly slyly silent id
1	24	3	7182	582.03	 the final, ironic asymptotes. regular requests nag instead of the carefully unusual asymptotes. furiously pending attainments among the slyly final packages boost after th
1	24	5	5180	905.41	heodolites above the ironic requests poach fluffily carefully unusual pinto beans. even packages acc
1	24	9	2227	511.20	, silent packages boost around the instructions. special requests sleep slyly against the slyly regular deposits. final, final accounts haggle fluffily among the final requests. regular 
1	26	5	3499	382.11	imes even pinto beans among the busily ironic accounts doubt blithely quickly final courts. furiously fluffy packages despite the carefully even plate
1	26	9	9702	821.89	 behind the blithely regular courts impress after the silent sheaves. bravely final ideas haggle 
1	27	2	9080	157.03	ole express, final requests. carefully regular packages lose about the regular pinto beans. blithely re
1	28	3	2452	744.57	ully regular theodolites haggle about the blithely pending packages. carefully ironic sentiments use quickly around the blithely silent requests. slyly ironic frays bo
1	28	9	6643	204.86	y ironic deposits above the slyly final deposits sleep furiously above the final deposits. quickly even i
1	29	2	6252	186.21	thely carefully even packages. even, final packages cajole after the quickly bold accounts. fluffily quick accounts in place of the theodolites doze slyly f
1	30	3	7945	583.84	 sleep. bold, regular deposits hang doggedly furiously bold requests. slyly bold excuses detect busily above the even gifts. blithely express courts are carefully. blithely final packages until th
1	30	5	535	743.26	sual instructions wake carefully blithely even hockey playe
1	30	9	7756	568.86	 special foxes across the dependencies cajole quickly against the slyly express packages! furiously unusual pinto beans boost blithely ironic Tir
1	31	2	1402	761.64	r platelets nag blithely regular deposits. ironic, bold requests 
1	32	3	2203	406.03	es? slyly enticing dugouts haggle carefully. regular packages alongside of the asymptotes are carefull
1	33	9	4410	929.05	 packages sleep carefully. slyly final instructions boost. slyly even requests among the carefully pending platelets wake along the final accounts. quickly expre
1	34	5	5459	824.69	ong the slyly silent requests. express, even requests haggle slyly
1	36	2	174	434.47	permanently express instructions. unusual accounts nag toward the accou
1	37	3	1449	745.64	y after the ironic accounts. blithely final instructions affix blithely. bold packages sleep carefully regular instructions. regular packages affix carefully. stealthy fo
1	38	9	1135	160.70	express accounts haggle. carefully even pinto beans according to the slyly final foxes nag slyly about the enticingly express dol
1	39	5	3682	300.43	ng requests are according to the packages. regular packages boost quickly. express Tiresias sleep silently across the even, regular ideas! blithely iro
1	41	2	9040	488.55	ss the dinos wake along the blithely regular theodolites. foxes cajole quickly ironic, final foxes. blithely ironic packages haggle against 
1	42	3	2893	716.81	requests nag. furiously brave packages boost at the furiously even waters. slyly pending ideas nag carefully caref
1	42	5	3500	200.00	against the ironic, ironic forges. slyly final deposits wake blithely. ironic courts sleep furiously ab
1	42	9	2927	709.06	g dugouts. carefully careful ideas are fluffily. carefully final pinto beans snooze. ironic deposits wake evenly along 
1	43	2	3232	307.12	counts: express, final platelets use slyly bold ideas. ironic theodolites about the blithely s
1	44	3	4798	833.15	 run. ironic, special dolphins according to the even, ironic deposits haggle carefully alongside of the carefully regular excuses. regular frays haggle carefully ironic dependenc
1	44	5	486	164.22	 final notornis throughout the unusual pinto beans are about the special accounts. bold packages sleep fluffily above the
1	45	2	5202	877.29	ngage blithely after the final requests. bold accounts sleep blithely blithely express dependencies. pinto beans through the carefully regular hockey players wake
1	46	3	8518	106.80	e unusual instructions shall have to detect slyly blithely ironic foxes. bold requests impress silent foxes. ironic, quiet realms haggle quickly pending, express pinto be
1	46	5	1381	985.88	ending platelets are carefully regular accounts. fluffily even accounts against the dependencies nag carefully final, 
1	46	9	7225	14.78	ously about the fluffily pending accounts. fluffily even dugouts are quickly slyly express platelets; quickly bold pearls sleep slyly even instructions. furiously ironic packages poach quic
1	48	5	9451	191.36	ckages cajole never even, special foxes. regular dependencies wake after the blithely ironic instructions. thinly ironic reque
1	48	9	5052	611.16	posits are blithely blithely final foxes. blithely even deposits haggle fluffily express requests. furiously final theodolites use sl
1	49	2	5336	713.25	ld accounts. furiously blithe waters use furiously blithely idle dependencies. pending deposits along the permanently re
1	50	3	6160	301.06	arefully ironic requests use. furiously pending waters play carefully carefully regular platelets. sly requests cajole furiously slyly regular pinto beans. bold packages boost fluffily. furiously i
1	50	9	2104	107.17	t blithely unusual theodolites. quickly final accounts affix fluffily regular requests. c
1	51	2	837	310.74	ly dogged, regular dependencies. express, even packages are 
1	51	3	8062	901.04	le ruthlessly furiously slow requests. fluffily slow depende
1	51	9	7318	85.03	al foxes. carefully ironic accounts detect carefully-- slyly even accounts use. furiously final platelets shall haggle sometimes after the blithely regu
1	52	3	6533	54.92	efully. slyly special deposits haggle along the quick deposits. slyly pending requests use quickly packages. final, final dolphins doubt according to the quickly unusual excuses
1	53	5	6929	224.83	xes. carefully ruthless asymptotes impress slyly. fluffily final deposits sleep against the ideas. slyly final packages wake. pending, express packages sleep quickly.
1	54	2	7079	798.98	he carefully unusual packages wake according to the ironic dolphins. permanently regular sheaves nag quickly. regular, ironic
1	54	5	2515	686.51	ly along the packages. blithely close pinto beans are blithely alongside of the unusual packages. carefully even platelets boost alongside of the even foxes. ironic de
1	54	9	2386	23.78	kly ironic foxes. final instructions hinder doggedly. carefull
1	55	3	8460	236.27	round the special, bold asymptotes cajole alongside of the instructions. qui
1	57	2	3788	211.66	ly according to the ironic requests-- slyly final accounts print carefully depths? pending, unusual accounts solve 
1	57	5	4721	411.08	instructions. quickly unusual deposits about the furiously special ideas believe among the furiously bold theodolites. unusual, even ideas nag: slow, special theodolites hagg
1	57	9	4583	137.68	ts. blithely bold theodolites can boost carefully carefully even instr
1	58	3	4136	512.24	packages cajole slyly quickly pending depths. special, bold realms cajole slyly. slyly ir
1	58	9	4328	542.52	ven deposits wake requests. quickly bold platelets sleep furiously after the ironic requests. even accounts haggle quickly bold 
1	60	2	5792	92.64	s the carefully pending deposits. slyly regular pinto beans against the furiously regular grouches lose carefully around the enticingly final ideas. furiously express packages cajole bold pa
1	60	5	148	504.10	s use fluffily. furiously regular deposits boost furiously against the even instructions. blithely final platelets wake. carefully pending asymptotes sleep blithely. regular, s
1	71	2	508	842.21	es cajole carefully around the furiously pending instructions. 
1	61	2	1540	858.64	 could have to use upon the packages. fluffily special packages integrate slyly final theodolites. pending warhorses wake quickly after the blithely final fo
1	62	3	1780	692.42	s around the even ideas cajole furiously somas. silent asym
1	62	9	9127	620.08	totes. unusual requests after the unusual accounts sleep fluffily bold notornis. slowly careful requests use according to the final ideas. pinto beans sleep. foxes are furiously furiously pe
1	63	2	1998	509.16	yly express theodolites. slyly bold ideas sleep furiously accordi
1	64	3	4542	398.92	. quickly final ideas cajole carefully among the blithely silent requests. sometimes ironic accounts nag furiously against the pending instructions. f
1	64	5	5567	228.61	y even instructions. unusual requests serve slyly. special foxes sleep quickly. fluffily ir
1	64	9	2064	25.77	 quickly regular ideas. carefully final requests snooze carefully regular, regular instructions. stealthily final pi
1	65	2	2054	503.10	e express excuses. ironic, even accounts across the reg
1	66	3	2568	447.08	ously even accounts boost slyly daring requests. even, regular realms kindle blithely. unusual, ironic ins
1	66	5	1076	785.75	its across the blithely regular theodolites wake furiously among the furiously regular accounts. pains are slyly care
1	67	2	5826	397.34	en, ironic deposits affix quickly unusual requests. busily ironic accounts are finally never even sauternes. ironic depos
1	68	3	5399	683.59	. finally final pinto beans play carefully unusual requests. never pending accounts are. regular, final theodolites wake furiously excuses. special request
1	68	5	8300	80.86	nooze according to the furiously even ideas. blithely regular accounts wake blithely. furiously regular Tiresias cajole regular deposits. regular theodolites eat alongside of the
1	68	9	3444	31.37	es impress furiously pending packages. always silent instructions above the fluffily bold packages haggle slyly blit
1	70	5	9074	182.58	ions after the fluffily regular foxes wake above the furiously regular requests: slyly regular deposits wake slyly daringly even Tiresias. express, express deposits are. always unusual pa
1	70	9	8063	452.80	y regular deposits nag about the carefully regular instructions; furiously express accounts along the final, express instruct
1	71	9	5179	329.13	usly at the packages. blithely regular deposits haggle regular packages. quickly special theodolites at the blithely ironic instructions wake
1	72	2	9346	41.04	 pending instructions before the even, silent dep
1	72	3	9855	497.26	tithes. quickly pending foxes haggle enticingly according to the accounts. accounts detect slyly: final packages wake. fina
1	73	2	5327	108.96	 beans are furiously between the regular ideas! unusual pinto beans use. furiously silent requests against the carefully even somas wake care
1	73	3	7729	920.66	ecial accounts sleep according to the slyly sly accounts. slyly express instructions nag. accounts cajole furiously quickly even foxes. furiously regular requests wake. carefully even frets haggle 
1	74	2	6234	849.66	 slyly regular foxes. silent accounts integrate. even deposits are quick
1	74	3	9473	496.36	 haggle carefully alongside of the regular requests. slyly regular accounts belie
1	74	5	3128	345.92	ic theodolites. express deposits haggle blithely pending packages. quickly express foxes could are slyly. deposits sleep deposits. final dependencies sleep ab
1	75	3	5439	884.01	ding excuses snooze special accounts. tithes alongside of the regular dep
1	75	5	6308	759.36	refully ironic dependencies. pinto beans use according to the packages. regular platelets wake around the blithely p
1	76	5	6371	552.38	ts use against the quickly ironic ideas. quickly even deposits are carefully a
1	77	5	1713	402.14	the even ideas kindle after the requests. regular theodolites cajole carefully about the blithely final ideas. carefully even dependencies at the flu
1	78	9	9915	729.94	around the special excuses. furiously even deposits serve boldly according to the platelets. carefully express accounts at the blithely unusual pinto beans sleep furiously against the u
1	79	9	465	28.33	uriously special frays cajole across the finally ironic pinto beans. ironic accounts sleep blithely. fluffily silent accounts are slyly at the slyly unusual ideas. even deposits nag slyly 
1	80	9	5385	945.72	cial asymptotes believe after the blithely unusual deposits. furiously silent pinto beans cajole quickly inside the slyly even deposits. regular, f
1	81	2	58	492.19	 instructions boost furiously across the foxes-- final depo
1	82	3	5532	900.07	 slyly? fluffily special dependencies haggle among the slyly special requests. regular, bold packages after the blithely ironic packages are slyly ironic packages. slyly final deposits w
1	84	5	208	469.80	carefully express dolphins nag about the slyly bold requests. slyly even packages wake among the furiously special attainments.
1	88	9	276	821.43	gular pinto beans. slyly pending excuses breach blithely express accounts. thin deposits sleep slyly around the even accounts; fluffily busy patterns kindle. slyly final deposits along the 
1	91	2	7986	528.64	luffily final instructions. furiously unusual foxes haggle 
1	91	3	3257	906.20	ackages cajole slyly. blithely bold deposits cajole. blithely 
1	91	5	1265	703.41	 quickly silent deposits use attainments. final requests along the carefully ironic accounts wake blithely about the carefully ironic excuses. furiously bold excuses wake final, final ex
1	92	3	9337	224.01	 requests are slyly along the deposits. fluffy pains alongside of the deposits 
1	92	5	3199	91.63	ake carefully: carefully ironic requests sleep careful
1	93	5	5275	376.47	ounts boost fluffily along the thinly regular realms. busily regular a
1	94	5	5433	365.56	 even excuses wake carefully. quickly unusual requests wake accounts. regularly pending packages are regular 
1	95	9	7379	973.74	 above the furiously unusual deposits haggle ironic ideas. express, even packages haggle slyly slyly special asymp
1	96	9	9985	672.29	ecial instructions-- blithely silent theodolites play. even, silent accounts sleep. blithely silent requests haggle final, f
1	97	9	2390	458.34	 carefully unusual pinto beans; even deposits detect furiously
1	98	2	3759	811.55	iously. final, express packages are across the ironic dependencies. slyly thin ideas according to the even Tiresias detect furiou
1	98	9	9486	908.21	usly final deposits mold furiously above the even deposits. carefully ironic packages across the quickly regular dolphins are slyly according to the slyly even
1	99	2	7970	365.83	ending accounts cajole furiously. requests promise care
1	99	3	2789	843.88	ending accounts. furiously sly packages above the carefully unusual dolphins sleep after the thinly even deposits. requests wake abo
1	100	2	2070	196.73	 dolphins. bold deposits along the even theodolites sleep furiously about the final pinto beans. furiously unusual courts cajole about the carefully bold asymptotes. accounts integrate slyly entic
1	100	3	4994	929.57	 deposits. accounts are slyly regular ideas. slyly special pinto beans upo
1	101	2	5589	305.40	blithely above the fluffily pending ideas. quickly quick accounts nod ruthlessly above the carefully pending packages. slyly s
1	102	3	1384	876.75	s wake quickly. carefully express deposits wake. silent, regular requests sleep slyly after the furiously ironic deposits. slyly unusual accounts cajole
1	102	5	9137	332.71	telets are final, special deposits. silently ironic deposits wake. pending, eve
1	102	9	6942	231.02	tions haggle against the furiously ironic deposits. quickly final asymptotes haggle carefully. regular sentiments might cajole silent courts. blithely bold frays 
1	104	5	2368	946.39	packages. final packages wake enticingly. furiously regular asymptotes are always about the carefully regular deposits. slyly regular platelets cajole carefully. final pinto beans must pro
1	104	9	6269	213.89	 ideas against the final accounts wake slyly regular notornis. final deposits haggle a
1	105	2	8519	904.17	ding to the furiously careful ideas. dogged theodolites wake fluffily among the slyly bold ideas. blithely brave warthogs above the slyly even theodolit
1	106	3	2297	281.98	inal packages. pending foxes sleep bold hockey players. courts across the blithely regular packages sleep fl
1	106	9	3144	779.68	 final deposits along the slyly express theodolites cajole blithely after the ironic pinto beans. furiousl
1	107	2	4667	372.94	uctions sleep doggedly final requests. express, final theodolites cajole fluffily furiously silent deposits. blithely regular requests cajole quickly regular instruction
1	108	3	5534	626.89	instructions. blithely regular instructions according to the permanent foxes cajole blithely slyly fluffy foxes. slyly regular asymptotes cajole foxes. slyly unusual deposits 
1	108	5	3142	922.27	slyly express accounts are fluffily along the blithely unusual packages. pinto beans mold furiously. furiously bold instructions are blithely deposits. quickly special accounts detect t
1	108	9	4149	558.85	 of the quickly pending theodolites. fluffily unusual frays wake accounts. carefully even foxes wake slyly. carefully special pinto beans 
1	109	2	4470	992.21	ake furiously packages. blithely even foxes haggle furious
1	110	3	8914	183.47	yly even foxes. carefully regular requests doubt. pending, regular requests across the blithely final theodolites hag
1	110	5	1160	932.08	ake blithely. furiously ironic accounts sleep fluffily
1	111	2	1890	321.97	st foxes. daring depths use above the furiously special ideas. ironic foxes among the carefully final theodolites are alongside of the regular depths. e
1	111	5	9587	978.65	express instructions against the furiously final grouches haggle across the blithely ironic theodolites. slyly special dependencies in place of the carefully pending 
1	112	2	1211	815.88	l requests integrate furiously. quickly quiet packages are carefully regular accounts. regular packages eat quickly express, ironic sheaves. quickly dogged accounts o
1	112	3	8509	111.42	unts. carefully ironic instructions are final, bold foxes. bold excuses run according to the unusual packages. theodolites cajole carefully according to the fluffily pending deposits? sly
1	112	9	4835	913.75	ach slyly special dependencies. furiously ironic pinto beans sleep slyly pen
1	113	3	4692	141.48	 blithely unusual gifts snooze against the quickly ironic packages. regular packages across the carefully regular packages bo
1	114	5	7146	447.24	ideas will nag regular accounts! carefully final requests cajole furiously quickly final tithes. furiously express instructions a
1	115	2	7781	861.93	lyly ironic pinto beans affix alongside of the furiously even ideas: quickly bold warhorses sle
1	115	5	3002	81.52	efully after the quickly regular deposits. daringly pending ideas sleep even ideas. silent, re
1	115	9	983	867.45	kly. requests nag after the blithely bold packages. express requests cajole theodolites. blithely express requests sleep after the furiously regular accounts. fluffily r
1	116	3	8679	866.56	aphs cajole blithely regular accounts. even packages doubt; bold instructions boost quickly. fluffi
1	118	2	6326	325.61	 packages. express, final frays affix quickly above the final asymptotes. carefully regular requests doubt quickly f
1	118	5	7806	283.27	 accounts affix carefully. regular, regular packages among the brave, pendin
1	118	9	694	744.73	refully slow requests. requests against the special pac
1	119	3	1452	676.92	ular instructions was slyly. furiously bold gifts boost f
1	119	9	583	782.47	yly pending requests-- carefully special instructions haggle carefully even instructions. blithely regular theodolites detect blithely final ideas. blithely ironic deposits among the sl
1	151	3	9417	244.06	 foxes along the hockey players are slyly about the blithely even packages. unusu
1	121	2	9741	147.45	ly according to the carefully regular asymptotes. silent excuses cajole carefully against the never silent instructions. furio
1	122	3	1019	451.29	 blithely regular accounts. blithely final pains sleep besides the blithely final warhorses. furiously unusual requests haggle furiously
1	122	5	2083	739.25	counts. unusual requests alongside of the regular requests are carefully stealthy instructions? regular sauternes cajole. final theodolites breach carefully at the blithely final idea
1	123	2	2692	217.01	he ironic accounts nag fluffily after the bold, pending theodolites. blithely final ideas sleep carefully according to the blithely ironic foxes. regular requests are. furiousl
1	124	3	9295	882.54	s along the accounts poach quickly ironic deposits. even, final excuses thrash carefully about the express, special pains. carefully careful accounts breach slyly
1	124	5	7102	901.98	ily accounts. furiously busy theodolites above the deposits thrash above the blithely final foxes. express instructions nod slyly furiously busy packages. special asymp
1	124	9	3969	908.64	l epitaphs. packages cajole among the furiously regular requests. closely
1	126	5	1532	451.61	refully alongside of the quickly bold excuses. enticing, bold 
1	126	9	5458	929.43	leep to the furiously special accounts. furiously final courts 
1	127	2	1467	237.98	regular accounts! quickly ironic packages haggle according to the accounts. carefully ironic 
1	128	3	7602	224.49	xcuses. blithely unusual theodolites use slyly carefully even warthogs. slyly even dugouts haggle slyly final, express pinto beans. furiously bold packages thrash requests? slyly unusual packages 
1	128	9	6982	425.29	ironic asymptotes. fluffily ironic packages use. ironic, regular ideas are in place of the quickly silent deposits. final, bold gifts across the ironic, regular pac
1	129	2	1968	27.22	ealthy, ironic deposits. slyly ironic pinto beans are blithely pinto beans. blithely ironic 
1	130	3	7387	883.99	aggle furiously. even ideas hinder deposits. even, final ideas are. unusual theodolites after the special, express foxes haggle carefully pending accou
1	130	5	6909	275.58	lly unusual accounts try to boost along the special packages. furiously bold requests x-ray blithely ironic waters. slyly unusual orbi
1	130	9	4850	442.81	ully regular deposits snooze. slyly silent foxes detect furiously furiously bold requests. slyly regular accounts breach. blithely bli
1	131	2	3263	211.70	sits sleep quickly regular multipliers. slyly even platelets cajole after the furiously ironic deposits. slyly ironic requests should have to cajole: bl
1	132	3	3092	687.29	y special decoys against the ideas affix against the sly
1	133	9	404	478.18	ly ironic requests run instead of the blithely ironic accounts? regular ideas use fluffily: even, express packages sleep abov
1	134	5	852	927.45	 sleep unusual, express packages. unusual sentiments are furio
1	136	2	6068	806.19	structions. ironic theodolites haggle according to the final, daring pearls. carefully ironic somas are silently requests. express pa
1	137	3	4078	441.11	packages. blithely unusual sentiments should are. furiously regular accounts nag quickly carefully special asymptotes! idly ironic requests dazzle bold requests. carefully expres
1	138	9	133	576.96	regular, final deposits maintain slyly even requests. regularly furious deposits use above the stealthy requests. ironic deposits are. carefully final frays are carefully. carefu
1	139	5	3285	690.00	xpress pains. quickly regular ideas after the special, bold excuses wake furiously final ideas. slyly bold accounts nag packages. ironically regular
1	141	2	6776	293.63	fluffily unusual courts sleep. close pinto beans haggle quickly after the carefully ir
1	142	3	9219	897.49	thlessly special requests sleep blithely about the bold deposits. express, ironic instructions wake. final packages are blithely. deposits are carefully furiously even deposits. furiously regular a
1	142	5	3076	860.55	gular requests about the pending packages wake furiously dogged accounts. th
1	142	9	13	334.33	are blithely blithely brave requests. slyly regular theodolites are furiously. blithely ironic dependencies haggle blithely. furiously unu
1	143	2	1952	199.37	l accounts are quickly after the unusual packages. regular accounts wake among the quickly even accounts. even, ironic
1	144	3	5427	361.83	 foxes integrate carefully. deposits cajole fluffily. pending deposits kindle slyly carefully regular packages. even, thin accounts according to th
1	144	5	6295	457.37	 pinto beans promise across the blithely bold packages. express, regular accounts play around the slyly silent deposits. specia
1	145	2	3838	568.91	ss, final asymptotes are. furiously express accounts run. furiously express dependencies eat carefully blithely ironic theodolites. closely ironic foxes among the silent asymptotes cajole
1	146	3	4628	123.91	ly across the dependencies. daringly ironic deposits are furiously; requests are. quickly regular accounts hang. carefu
1	146	5	9687	882.37	packages? ideas affix slyly even accounts: express requests wake slyly carefully special depths. ironic in
1	146	9	5893	858.59	sual instructions believe. fluffily unusual warhorses nag. unusual dependencies sleep. slow sheaves haggle furiously. carefully ironic dependencies cajole slyly against the accounts. 
1	148	5	1904	774.56	y even pinto beans. fluffily ironic packages sleep slyly. permanently brave requests boost furiously packages. boldly ironic deposits across the carefully bold pinto b
1	148	9	8007	177.40	final requests-- slyly regular theodolites haggle carefully across the blithely final dependencies. slyly even requests about the carefully even accounts sleep 
1	149	2	7392	266.53	es detect along the regular instructions. bold ideas boost slyly. quickly unusual accounts doubt. carefully even foxes thrash slyly silent, ironic dolphins: Tiresias must wake 
1	150	3	3172	33.71	 about the silent ideas. fluffily final requests impress. slyly final requests wake carefully about the slyly express foxes. slyly regular warthogs sleep fur
1	150	9	1640	327.34	slyly even deposits alongside of the furiously even accounts detect boldly quickly regular accounts. final accounts kindle carefu
1	151	2	391	281.90	dolites. boldly ironic packages cajole fluffily regular instructions. regular, ironic accounts are blithely. ironic accounts are alongside of th
1	151	9	253	840.06	 haggle. somas are carefully. slyly regular requests sleep blithely atop the thinly express deposits. stealthily express packages cajole daringly express requests. carefully special requests after t
1	152	3	1396	164.60	d the instructions. carefully pending accounts haggle fluffily ruthless instruc
1	153	5	6397	285.92	furiously special platelets haggle quickly even, bold pinto beans. blithely close pinto beans boost around the furiously regular packages. quickly express requests cajole.
1	154	2	3231	829.29	ins along the packages use carefully requests. furiously unusual packages kindle fluffily quick
1	154	5	3961	474.19	quickly pending requests nag express dependencies. furiously unusual requests about the regular, pending packages wake according to the ironic packages! theodolites wake about the unusual, regula
1	154	9	7315	960.19	uickly regular dolphins ought to believe among the q
1	155	3	7077	413.24	lar instructions against the furiously unusual instructions breach furiously for the bold, even platelets. ironic accounts must have to are quickly across the 
1	157	2	3718	901.53	, ironic foxes. blithely even foxes wake about the carefully special req
1	157	5	763	568.46	cial packages boost along the ideas. packages sleep slyly express packages. ironic, bold requests
1	157	9	3400	288.41	encies integrate carefully even accounts. regular, regular sentiments are against the slyly regular deposits-- even, even ideas use inside the carefull
1	158	3	4465	837.16	 wake carefully in place of the furiously express deposits. slyly regular instructions engage. fluffily f
1	158	9	2845	408.72	y. slyly final pinto beans believe fluffily pending, regular deposits. final, unusual ideas according to t
1	170	9	838	667.16	orges do sleep furiously. fluffily furious requests among the final requests sleep after the slyly bold ideas? regular pinto beans might ha
1	160	2	6872	872.20	ions are carefully. carefully express foxes nag slyly before the carefully final excuses. accounts after the furiously ironic packages are furio
1	160	5	6034	733.59	 furiously against the final instructions. silent accounts sleep blithely after the boldly final requests. ex
1	161	2	9365	790.03	scapades. packages use. slyly final accounts haggle across the quickly final th
1	162	3	315	923.04	 ideas. carefully final dugouts will have to wake quickly regular asymptotes. express grouches unwind carefully after the regula
1	162	9	7014	236.07	cording to the stealthily fluffy theodolites. carefully unusual excuses around the regular deposits cajole slyly amo
1	163	2	3427	499.51	ithely bold packages integrate slyly quiet pinto beans. carefully even deposits boost slyly about the furiously fluffy packages. evenly regular dependencies wa
1	164	3	2134	84.02	ns believe. carefully express theodolites impress. carefully fina
1	164	5	1295	341.95	 bold instructions cajole slyly ironic deposits. quickly ironic foxes are carefully final, bold theodolites. ironic deposi
1	164	9	3028	64.89	fully furiously regular requests. furiously bold orbits serve about the regular packages? carefully final deposits p
1	165	2	3780	730.28	 furiously quickly regular foxes. pending requests engage evenly blithel
1	166	3	9364	581.52	pinto beans. pinto beans cajole furiously carefully special requests-- quickly 
1	166	5	6508	714.49	y express deposits cajole furiously above the carefully even theod
1	167	2	6748	704.97	t the silent ideas are blithely carefully even packages; blithely
1	168	3	7519	963.43	requests above the quickly regular deposits use carefully aft
1	168	5	9089	508.37	ests are always. regular ideas sleep fluffily; special, express instructions cajole slowly. pending platelets boost furiously against the bold, even instructions. bold instructi
1	168	9	347	394.83	hely blithely final theodolites. blithely final deposits among the quickly even ideas haggle about the blithely bold d
1	170	5	6593	202.07	ep blithely final packages. quickly bold pains cajole carefully across the somet
1	171	2	8217	859.60	ress deposits. carefully special requests are furiously final requests. accounts cajole carefully blith
1	171	9	7589	935.29	s above the theodolites wake slyly along the carefully unusual dependencies. carefully express theodolites a
1	172	2	8333	920.74	ronic foxes. quickly unusual accounts cajole blithely. blithely bold deposits cajole. blithely close pinto beans cajole requests. quickly express excuses around the quickly even deposits nag agai
1	172	3	9799	184.96	ts. slyly even asymptotes nag blithely regular accounts. final platelets cajole furiously slyly bold packages. ironic accounts sleep slyly. pendi
1	173	2	6050	683.78	e after the slyly regular ideas. unusual pinto beans cajole even asymptotes-- silent, stealthy requests after the even accounts haggle blithely regular instructions. slyly ev
1	173	3	8307	70.22	alongside of the furiously even packages. furiously final requests snooze blithely alongside of the carefull
1	174	2	8404	126.20	nding accounts mold furiously. slyly ironic foxes boost express sheaves. daringly final packages along the stealthy dependencies are blithely ironic requests. furiously pending pin
1	174	3	111	135.46	 express packages-- quickly unusual courts lose carefully requests. bold accounts solve about the theodolites; pinto beans use. ironic foxes
1	174	5	2103	681.95	sual, express requests wake furiously ruthless, final accounts. carefully ironic somas dazzle furiously. unusual asymptotes sleep-- patterns about the furiousl
1	175	3	9456	978.56	 regular packages. carefully ironic packages use. blithely ironic accounts among the pending, 
1	175	5	7522	784.93	 affix. quickly final theodolites haggle furiously after the slowly even pinto beans. furiously final packages use slyly. slyly regular reque
1	176	5	5407	947.51	ending accounts eat carefully instructions. carefully pending packages detect slyly express accounts. foxes wake fluffily across th
1	177	5	4727	859.82	es are. slyly ironic packages haggle around the slyly bold deposits. bold foxes haggle blithely. f
1	178	9	4231	558.56	deposits. patterns use against the furiously unusual accounts. accounts wake carefully above the careful
1	179	9	1954	372.75	even dependencies print carefully. deposits boost blithely about the ironic, ironic accounts. express, regular deposits are. bli
1	180	9	724	426.16	e, regular accounts. furiously final ideas are furiously above the bold, silent asymptotes. sly instructions are carefully quickly final sentiments. furiously ironic foxes cajole bold, exp
1	181	2	215	938.29	 accounts boost furiously furiously blithe theodolites. slyly bold requests unwind special, unusual requests. furious ideas boost quickly pending 
1	182	3	960	519.36	arefully pending dependencies are always slyly unusual pin
1	184	5	831	186.84	kages cajole carefully furiously ironic instructions. deposits use bl
1	188	9	730	713.62	nag against the final accounts. blithely pending attainments lose. silent requests wake quickly. careful
1	191	2	8310	521.06	the slowly regular deposits. special accounts along the quickly unusual
1	191	3	1693	464.46	y. slyly unusual waters across the special pinto beans nag blithely according to the busy deposits. carefully regular accounts are against the regular accounts; perman
1	191	5	9673	119.41	press deposits kindle theodolites! slyly final dependencies against the blithely final packages sleep slyly regular requests. theodolites cajole furiously quickly bold a
1	192	3	606	198.69	inal platelets integrate regular accounts. accounts wake ironic, silent accounts. slyly unusual accounts kindle carefully-
1	192	5	1811	359.59	ly carefully special asymptotes. furiously pending instructions haggle blithely bravely pending requests. carefully f
1	193	5	4762	606.19	ns sleep against the furiously regular asymptotes. carefully even asymptotes across the daringly final packages sleep fluf
1	194	5	4289	662.17	ic Tiresias serve along the ironic, express accounts. quickly final requests are slyly among the carefully special requests. accounts boost.
1	195	9	2803	992.27	xes according to the regular foxes wake furiously final theodolites. furiously regular packages sleep slyly express theodolites. slyly thin instructions sleep r
1	196	9	4778	37.61	fully final requests cajole fluffily across the furiously ironic accounts. qui
1	197	9	2631	279.05	e blithely. quickly final deposits wake fluffily excuses. even, unusual deposits x-ray among the final accounts. even ideas above the blithely ironic requests sleep furiously slyly final inst
1	198	2	6190	697.10	beans nag fluffily about the asymptotes. slyly bold escapades haggle quickly. fluffily special requests haggle above the ironic,
1	198	9	6878	587.41	y even accounts poach carefully about the asymptotes. deposits haggle slyly. finally unusual requests run silently regular, bold packages: instructions after the 
1	199	2	2742	890.63	 the special deposits? carefully final deposits about the carefully regular sauternes 
1	199	3	7167	884.56	onic platelets use carefully along the slowly stealthy ideas. slyly dogged instructions are quickly above the slyly u
1	200	2	5392	242.52	y unusual ideas. ruthlessly express asymptotes cajole. regular theodolites are. carefully silent deposits poach carefully across the fluffily even theodolites. carefully express realms hag
1	200	3	9408	307.79	oxes! fluffily regular requests use against the unusual, slow ideas. ironic accounts doze b
\.


--
-- Data for Name: region; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.region (r_regionkey, r_name, r_comment) FROM stdin;
0	AFRICA                   	lar deposits. blithely final packages cajole. regular waters are final requests. regular accounts are according to 
1	AMERICA                  	hs use ironic, even requests. s
2	ASIA                     	ges. thinly even pinto beans ca
3	EUROPE                   	ly final courts cajole furiously final excuse
4	MIDDLE EAST              	uickly special accounts cajole carefully blithely close requests. carefully final asymptotes haggle furiousl
\.


--
-- Data for Name: supplier; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.supplier (s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment) FROM stdin;
2	Supplier#000000002       	89eJ5ksX3ImxJQBvxObC,	5	15-679-861-2259	4032.68	 slyly bold instructions. idle dependen
3	Supplier#000000003       	q1,G3Pj6OjIuUYfUoH18BFTKP5aU9bEV3	1	11-383-516-1199	4192.40	blithely silent requests after the express dependencies are sl
5	Supplier#000000005       	Gcdm2rJRzl5qlTVzc	11	21-151-690-3663	-283.84	. slyly regular pinto bea
9	Supplier#000000009       	1KhUgZegwM3ua7dsYmekYBsK	10	20-403-398-8662	5302.37	s. unusual, even requests along the furiously regular pac
\.


--
-- Name: customer_c_custkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.customer_c_custkey_seq', 1, false);


--
-- Name: nation_n_nationkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.nation_n_nationkey_seq', 1, false);


--
-- Name: orders_o_orderkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.orders_o_orderkey_seq', 1, false);


--
-- Name: part_p_partkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.part_p_partkey_seq', 1, false);


--
-- Name: region_r_regionkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.region_r_regionkey_seq', 1, false);


--
-- Name: supplier_s_suppkey_seq; Type: SEQUENCE SET; Schema: public; Owner: vaultdb
--

SELECT pg_catalog.setval('public.supplier_s_suppkey_seq', 1, false);


--
-- Name: customer customer_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.customer
    ADD CONSTRAINT customer_pkey PRIMARY KEY (c_custkey);


--
-- Name: lineitem lineitem_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.lineitem
    ADD CONSTRAINT lineitem_pkey PRIMARY KEY (l_orderkey, l_linenumber);


--
-- Name: nation nation_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.nation
    ADD CONSTRAINT nation_pkey PRIMARY KEY (n_nationkey);


--
-- Name: orders orders_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_pkey PRIMARY KEY (o_orderkey);


--
-- Name: part part_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.part
    ADD CONSTRAINT part_pkey PRIMARY KEY (p_partkey);


--
-- Name: partsupp partsupp_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.partsupp
    ADD CONSTRAINT partsupp_pkey PRIMARY KEY (ps_partkey, ps_suppkey);


--
-- Name: region region_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.region
    ADD CONSTRAINT region_pkey PRIMARY KEY (r_regionkey);


--
-- Name: supplier supplier_pkey; Type: CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.supplier
    ADD CONSTRAINT supplier_pkey PRIMARY KEY (s_suppkey);


--
-- Name: idx_customer_nationkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_customer_nationkey ON public.customer USING btree (c_nationkey);


--
-- Name: idx_lineitem_orderkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_lineitem_orderkey ON public.lineitem USING btree (l_orderkey);


--
-- Name: idx_lineitem_part_supp; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_lineitem_part_supp ON public.lineitem USING btree (l_partkey, l_suppkey);


--
-- Name: idx_lineitem_shipdate; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_lineitem_shipdate ON public.lineitem USING btree (l_shipdate, l_discount, l_quantity);


--
-- Name: idx_nation_regionkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_nation_regionkey ON public.nation USING btree (n_regionkey);


--
-- Name: idx_orders_custkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_orders_custkey ON public.orders USING btree (o_custkey);


--
-- Name: idx_orders_orderdate; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_orders_orderdate ON public.orders USING btree (o_orderdate);


--
-- Name: idx_partsupp_partkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_partsupp_partkey ON public.partsupp USING btree (ps_partkey);


--
-- Name: idx_partsupp_suppkey; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_partsupp_suppkey ON public.partsupp USING btree (ps_suppkey);


--
-- Name: idx_supplier_nation_key; Type: INDEX; Schema: public; Owner: vaultdb
--

CREATE INDEX idx_supplier_nation_key ON public.supplier USING btree (s_nationkey);


--
-- Name: customer customer_c_nationkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.customer
    ADD CONSTRAINT customer_c_nationkey_fkey FOREIGN KEY (c_nationkey) REFERENCES public.nation(n_nationkey) ON DELETE CASCADE;


--
-- Name: lineitem lineitem_l_orderkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.lineitem
    ADD CONSTRAINT lineitem_l_orderkey_fkey FOREIGN KEY (l_orderkey) REFERENCES public.orders(o_orderkey) ON DELETE CASCADE;


--
-- Name: nation nation_n_regionkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.nation
    ADD CONSTRAINT nation_n_regionkey_fkey FOREIGN KEY (n_regionkey) REFERENCES public.region(r_regionkey) ON DELETE CASCADE;


--
-- Name: orders orders_o_custkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_o_custkey_fkey FOREIGN KEY (o_custkey) REFERENCES public.customer(c_custkey) ON DELETE CASCADE;


--
-- Name: partsupp partsupp_ps_partkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.partsupp
    ADD CONSTRAINT partsupp_ps_partkey_fkey FOREIGN KEY (ps_partkey) REFERENCES public.part(p_partkey) ON DELETE CASCADE;


--
-- Name: partsupp partsupp_ps_suppkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.partsupp
    ADD CONSTRAINT partsupp_ps_suppkey_fkey FOREIGN KEY (ps_suppkey) REFERENCES public.supplier(s_suppkey) ON DELETE CASCADE;


--
-- Name: supplier supplier_s_nationkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: vaultdb
--

ALTER TABLE ONLY public.supplier
    ADD CONSTRAINT supplier_s_nationkey_fkey FOREIGN KEY (s_nationkey) REFERENCES public.nation(n_nationkey) ON DELETE CASCADE;


--
-- Name: COLUMN customer.c_custkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(c_custkey) ON TABLE public.customer TO public_attribute;


--
-- Name: COLUMN customer.c_nationkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(c_nationkey) ON TABLE public.customer TO public_attribute;
GRANT SELECT(c_nationkey) ON TABLE public.customer TO partitioned_on;


--
-- Name: COLUMN lineitem.l_orderkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(l_orderkey) ON TABLE public.lineitem TO partitioned_on;


--
-- Name: TABLE nation; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT ON TABLE public.nation TO replicated;


--
-- Name: COLUMN nation.n_nationkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(n_nationkey) ON TABLE public.nation TO public_attribute;


--
-- Name: COLUMN nation.n_name; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(n_name) ON TABLE public.nation TO public_attribute;


--
-- Name: COLUMN nation.n_regionkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(n_regionkey) ON TABLE public.nation TO public_attribute;


--
-- Name: COLUMN nation.n_comment; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(n_comment) ON TABLE public.nation TO public_attribute;


--
-- Name: COLUMN orders.o_orderkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(o_orderkey) ON TABLE public.orders TO public_attribute;


--
-- Name: COLUMN orders.o_custkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(o_custkey) ON TABLE public.orders TO partitioned_on;


--
-- Name: TABLE part; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT ON TABLE public.part TO replicated;


--
-- Name: COLUMN part.p_partkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_partkey) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_name; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_name) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_mfgr; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_mfgr) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_brand; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_brand) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_type; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_type) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_size; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_size) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_container; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_container) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_retailprice; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_retailprice) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN part.p_comment; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(p_comment) ON TABLE public.part TO public_attribute;


--
-- Name: COLUMN partsupp.ps_partkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(ps_partkey) ON TABLE public.partsupp TO public_attribute;


--
-- Name: COLUMN partsupp.ps_suppkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(ps_suppkey) ON TABLE public.partsupp TO public_attribute;
GRANT SELECT(ps_suppkey) ON TABLE public.partsupp TO partitioned_on;


--
-- Name: TABLE region; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT ON TABLE public.region TO replicated;


--
-- Name: COLUMN region.r_regionkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(r_regionkey) ON TABLE public.region TO public_attribute;


--
-- Name: COLUMN region.r_name; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(r_name) ON TABLE public.region TO public_attribute;


--
-- Name: COLUMN region.r_comment; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(r_comment) ON TABLE public.region TO public_attribute;


--
-- Name: COLUMN supplier.s_suppkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(s_suppkey) ON TABLE public.supplier TO public_attribute;


--
-- Name: COLUMN supplier.s_name; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(s_name) ON TABLE public.supplier TO public_attribute;


--
-- Name: COLUMN supplier.s_nationkey; Type: ACL; Schema: public; Owner: vaultdb
--

GRANT SELECT(s_nationkey) ON TABLE public.supplier TO public_attribute;
GRANT SELECT(s_nationkey) ON TABLE public.supplier TO partitioned_on;


--
-- PostgreSQL database dump complete
--

