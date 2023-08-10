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
1	Customer#000000001	IVhzIApeRb ot,c,E	15	25-989-741-2988	711.56	BUILDING  	to the even, regular platelets. regular, ironic epitaphs nag e
2	Customer#000000002	XSTf4,NCwDVaWNe6tEgvwfmRchLXak	13	23-768-687-3665	121.65	AUTOMOBILE	l accounts. blithely ironic theodolites integrate boldly: caref
6	Customer#000000006	sKZz0CsnMD7mp4Xd0YrBvx,LREYKUWAh yVn	20	30-114-968-4951	7638.57	AUTOMOBILE	tions. even deposits boost according to the slyly bold packages. final accounts cajole requests. furious
7	Customer#000000007	TcGe5gaZNgVePxU5kRrvXBfkasDTea	18	28-190-982-9759	9561.95	AUTOMOBILE	ainst the ironic, express theodolites. express, even pinto beans among the exp
8	Customer#000000008	I0B10bB0AymmC, 0PrRYBCP1yGJ8xcBPmWhl5	17	27-147-574-9335	6819.74	BUILDING  	among the slyly regular theodolites kindle blithely courts. carefully even theodolites haggle slyly along the ide
11	Customer#000000011	PkWS 3HlXqwTuzrKg633BEi	23	33-464-151-3439	-272.60	BUILDING  	ckages. requests sleep slyly. quickly even pinto beans promise above the slyly regular pinto beans. 
12	Customer#000000012	9PWKuhzT4Zr1Q	13	23-791-276-1263	3396.49	HOUSEHOLD 	 to the carefully final braids. blithely regular requests nag. ironic theodolites boost quickly along
15	Customer#000000015	YtWggXoOLdwdo7b0y,BZaGUQMLJMX1Y,EC,6Dn	23	33-687-542-7601	2788.52	HOUSEHOLD 	 platelets. regular deposits detect asymptotes. blithely unusual packages nag slyly at the fluf
19	Customer#000000019	uc,3bHIx84H,wdrmLOjVsiqXCq2tr	18	28-396-526-5053	8914.71	HOUSEHOLD 	 nag. furiously careful packages are slyly at the accounts. furiously regular in
20	Customer#000000020	JrPk8Pqplj4Ne	22	32-957-234-8742	7603.40	FURNITURE 	g alongside of the special excuses-- fluffily enticing packages wake 
24	Customer#000000024	HXAFgIAyjxtdqwimt13Y3OZO 4xeLe7U8PqG	13	23-127-851-8031	9255.67	MACHINERY 	into beans. fluffily final ideas haggle fluffily
26	Customer#000000026	8ljrc5ZeMl7UciP	22	32-363-455-4837	5182.05	AUTOMOBILE	c requests use furiously ironic requests. slyly ironic dependencies us
31	Customer#000000031	LUACbO0viaAv6eXOAebryDB xjVst	23	33-197-837-7094	5236.89	HOUSEHOLD 	s use among the blithely pending depo
32	Customer#000000032	jD2xZzi UmId,DCtNBLXKj9q0Tlp2iQ6ZcO3J	15	25-430-914-2194	3471.53	BUILDING  	cial ideas. final, furious requests across the e
33	Customer#000000033	qFSlMuLucBmx9xnn5ib2csWUweg D	17	27-375-391-1280	-78.56	AUTOMOBILE	s. slyly regular accounts are furiously. carefully pending requests
34	Customer#000000034	Q6G9wZ6dnczmtOx509xgE,M2KV	15	25-344-968-5422	8589.70	HOUSEHOLD 	nder against the even, pending accounts. even
35	Customer#000000035	TEjWGE4nBzJL2	17	27-566-888-7431	1228.24	HOUSEHOLD 	requests. special, express requests nag slyly furiousl
36	Customer#000000036	3TvCzjuPzpJ0,DdJ8kW5U	21	31-704-669-5769	4987.27	BUILDING  	haggle. enticing, quiet platelets grow quickly bold sheaves. carefully regular acc
43	Customer#000000043	ouSbjHk8lh5fKX3zGso3ZSIj9Aa3PoaFd	19	29-316-665-2897	9904.28	MACHINERY 	ial requests: carefully pending foxes detect quickly. carefully final courts cajole quickly. carefully
44	Customer#000000044	Oi,dOSPwDu4jo4x,,P85E0dmhZGvNtBwi	16	26-190-260-5375	7315.94	AUTOMOBILE	r requests around the unusual, bold a
53	Customer#000000053	HnaxHzTfFTZs8MuCpJyTbZ47Cm4wFOOgib	15	25-168-852-5363	4113.64	HOUSEHOLD 	ar accounts are. even foxes are blithely. fluffily pending deposits boost
57	Customer#000000057	97XYbsuOPRXPWU	21	31-835-306-1650	4151.93	AUTOMOBILE	ove the carefully special packages. even, unusual deposits sleep slyly pend
58	Customer#000000058	g9ap7Dk1Sv9fcXEWjpMYpBZIRUohi T	13	23-244-493-2508	6478.46	HOUSEHOLD 	ideas. ironic ideas affix furiously express, final instructions. regular excuses use quickly e
61	Customer#000000061	9kndve4EAJxhg3veF BfXr7AqOsT39o gtqjaYE	17	27-626-559-8599	1536.24	FURNITURE 	egular packages shall have to impress along the 
63	Customer#000000063	IXRSpVWWZraKII	21	31-952-552-9584	9331.13	AUTOMOBILE	ithely even accounts detect slyly above the fluffily ir
65	Customer#000000065	RGT yzQ0y4l0H90P783LG4U95bXQFDRXbWa1sl,X	23	33-733-623-5267	8795.16	AUTOMOBILE	y final foxes serve carefully. theodolites are carefully. pending i
66	Customer#000000066	XbsEqXH1ETbJYYtA1A	22	32-213-373-5094	242.77	HOUSEHOLD 	le slyly accounts. carefully silent packages benea
70	Customer#000000070	mFowIuhnHjp2GjCiYYavkW kUwOjIaTCQ	22	32-828-107-2832	4867.52	FURNITURE 	fter the special asymptotes. ideas after the unusual frets cajole quickly regular pinto be
75	Customer#000000075	Dh 6jZ,cwxWLKQfRKkiGrzv6pm	18	28-247-803-9025	6684.10	AUTOMOBILE	 instructions cajole even, even deposits. finally bold deposits use above the even pains. slyl
77	Customer#000000077	4tAE5KdMFGD4byHtXF92vx	17	27-269-357-4674	1738.87	BUILDING  	uffily silent requests. carefully ironic asymptotes among the ironic hockey players are carefully bli
79	Customer#000000079	n5hH2ftkVRwW8idtD,BmM2	15	25-147-850-4166	5121.28	MACHINERY 	es. packages haggle furiously. regular, special requests poach after the quickly express ideas. blithely pending re
81	Customer#000000081	SH6lPA7JiiNC6dNTrR	20	30-165-277-3269	2023.71	BUILDING  	r packages. fluffily ironic requests cajole fluffily. ironically regular theodolit
82	Customer#000000082	zhG3EZbap4c992Gj3bK,3Ne,Xn	18	28-159-442-5305	9468.34	AUTOMOBILE	s wake. bravely regular accounts are furiously. regula
83	Customer#000000083	HnhTNB5xpnSF20JBH4Ycs6psVnkC3RDf	22	32-817-154-4122	6463.51	BUILDING  	ccording to the quickly bold warhorses. final, regular foxes integrate carefully. bold packages nag blithely ev
87	Customer#000000087	hgGhHVSWQl 6jZ6Ev	23	33-869-884-7053	6327.54	FURNITURE 	hely ironic requests integrate according to the ironic accounts. slyly regular pla
88	Customer#000000088	wtkjBN9eyrFuENSMmMFlJ3e7jE5KXcg	16	26-516-273-2566	8031.44	AUTOMOBILE	s are quickly above the quickly ironic instructions; even requests about the carefully final deposi
89	Customer#000000089	dtR, y9JQWUO6FoJExyp8whOU	14	24-394-451-5404	1530.76	FURNITURE 	counts are slyly beyond the slyly final accounts. quickly final ideas wake. r
90	Customer#000000090	QxCzH7VxxYUWwfL7	16	26-603-491-1238	7354.23	BUILDING  	sly across the furiously even 
95	Customer#000000095	EU0xvmWvOmUUn5J,2z85DQyG7QCJ9Xq7	15	25-923-255-2929	5327.38	MACHINERY 	ithely. ruthlessly final requests wake slyly alongside of the furiously silent pinto beans. even the
97	Customer#000000097	OApyejbhJG,0Iw3j rd1M	17	27-588-919-5638	2164.48	AUTOMOBILE	haggle slyly. bold, special ideas are blithely above the thinly bold theo
99	Customer#000000099	szsrOiPtCHVS97Lt	15	25-515-237-9232	4088.65	HOUSEHOLD 	cajole slyly about the regular theodolites! furiously bold requests nag along the pending, regular packages. somas
100	Customer#000000100	fptUABXcmkC5Wx	20	30-749-445-4907	9889.89	FURNITURE 	was furiously fluffily quiet deposits. silent, pending requests boost against 
102	Customer#000000102	UAtflJ06 fn9zBfKjInkQZlWtqaA	19	29-324-978-8538	8462.17	BUILDING  	ously regular dependencies nag among the furiously express dinos. blithely final
107	Customer#000000107	Zwg64UZ,q7GRqo3zm7P1tZIRshBDz	15	25-336-529-9919	2514.15	AUTOMOBILE	counts cajole slyly. regular requests wake. furiously regular deposits about the blithely final fo
109	Customer#000000109	OOOkYBgCMzgMQXUmkocoLb56rfrdWp2NE2c	16	26-992-422-8153	-716.10	BUILDING  	es. fluffily final dependencies sleep along the blithely even pinto beans. final deposits haggle furiously furiou
111	Customer#000000111	CBSbPyOWRorloj2TBvrK9qp9tHBs	22	32-582-283-7528	6505.26	MACHINERY 	ly unusual instructions detect fluffily special deposits-- theodolites nag carefully during the ironic dependencies
112	Customer#000000112	RcfgG3bO7QeCnfjqJT1	19	29-233-262-8382	2953.35	FURNITURE 	rmanently unusual multipliers. blithely ruthless deposits are furiously along the
114	Customer#000000114	xAt 5f5AlFIU	14	24-805-212-7646	1027.46	FURNITURE 	der the carefully express theodolites are after the packages. packages are. bli
116	Customer#000000116	yCuVxIgsZ3,qyK2rloThy3u	16	26-632-309-5792	8403.99	BUILDING  	as. quickly final sauternes haggle slyly carefully even packages. brave, ironic pinto beans are above the furious
117	Customer#000000117	uNhM,PzsRA3S,5Y Ge5Npuhi	24	34-403-631-3505	3950.83	FURNITURE 	affix. instructions are furiously sl
118	Customer#000000118	OVnFuHygK9wx3xpg8	18	28-639-943-7051	3582.37	AUTOMOBILE	uick packages alongside of the furiously final deposits haggle above the fluffily even foxes. blithely dogged dep
121	Customer#000000121	tv nCR2YKupGN73mQudO	17	27-411-990-2959	6428.32	BUILDING  	uriously stealthy ideas. carefully final courts use carefully
124	Customer#000000124	aTbyVAW5tCd,v09O	18	28-183-750-7809	1842.49	AUTOMOBILE	le fluffily even dependencies. quietly s
125	Customer#000000125	,wSZXdVR xxIIfm9s8ITyLl3kgjT6UC07GY0Y	19	29-261-996-3120	-234.12	FURNITURE 	x-ray finally after the packages? regular requests c
126	Customer#000000126	ha4EHmbx3kg DYCsP6DFeUOmavtQlHhcfaqr	22	32-755-914-7592	1001.39	HOUSEHOLD 	s about the even instructions boost carefully furiously ironic pearls. ruthless, 
127	Customer#000000127	Xyge4DX2rXKxXyye1Z47LeLVEYMLf4Bfcj	21	31-101-672-2951	9280.71	MACHINERY 	ic, unusual theodolites nod silently after the final, ironic instructions: pending r
133	Customer#000000133	IMCuXdpIvdkYO92kgDGuyHgojcUs88p	17	27-408-997-8430	2314.67	AUTOMOBILE	t packages. express pinto beans are blithely along the unusual, even theodolites. silent packages use fu
135	Customer#000000135	oZK,oC0 fdEpqUML	19	29-399-293-6241	8732.91	FURNITURE 	 the slyly final accounts. deposits cajole carefully. carefully sly packag
137	Customer#000000137	cdW91p92rlAEHgJafqYyxf1Q	16	26-777-409-5654	7838.30	HOUSEHOLD 	carefully regular theodolites use. silent dolphins cajo
143	Customer#000000143	681r22uL452zqk 8By7I9o9enQfx0	16	26-314-406-7725	2186.50	MACHINERY 	across the blithely unusual requests haggle theodo
145	Customer#000000145	kQjHmt2kcec cy3hfMh969u	13	23-562-444-8454	9748.93	HOUSEHOLD 	ests? express, express instructions use. blithely fina
147	Customer#000000147	6VvIwbVdmcsMzuu,C84GtBWPaipGfi7DV	18	28-803-187-4335	8071.40	AUTOMOBILE	ress packages above the blithely regular packages sleep fluffily blithely ironic accounts. 
149	Customer#000000149	3byTHCp2mNLPigUrrq	19	29-797-439-6760	8959.65	AUTOMOBILE	al instructions haggle against the slyly bold w
150	Customer#000000150	zeoGShTjCwGPplOWFkLURrh41O0AZ8dwNEEN4 	18	28-328-564-7630	3849.48	MACHINERY 	ole blithely among the furiously pending packages. furiously bold ideas wake fluffily ironic idea
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
2	107	2	1	38	38269.80	0.00	0.05	N	O	1997-01-28	1997-01-14	1997-02-02	TAKE BACK RETURN         	RAIL      	ven requests. deposits breach a
3	5	2	1	45	40725.00	0.06	0.00	R	F	1994-02-02	1994-01-04	1994-02-23	NONE                     	AIR       	ongside of the furiously brave acco
3	20	10	2	49	45080.98	0.10	0.00	R	F	1993-11-09	1993-12-20	1993-11-24	TAKE BACK RETURN         	RAIL      	 unusual accounts. eve
3	129	8	3	27	27786.24	0.06	0.07	A	F	1994-01-16	1993-11-22	1994-01-23	DELIVER IN PERSON        	SHIP      	nal foxes wake. 
3	30	5	4	2	1860.06	0.01	0.06	A	F	1993-12-04	1994-01-07	1994-01-01	NONE                     	TRUCK     	y. fluffily pending d
3	184	5	5	28	30357.04	0.04	0.00	R	F	1993-12-14	1994-01-10	1994-01-01	TAKE BACK RETURN         	FOB       	ages nag slyly pending
3	63	8	6	26	25039.56	0.10	0.02	A	F	1993-10-29	1993-12-18	1993-11-04	TAKE BACK RETURN         	RAIL      	ges sleep after the caref
4	89	10	1	30	29672.40	0.03	0.08	N	O	1996-01-10	1995-12-14	1996-01-18	DELIVER IN PERSON        	REG AIR   	- quickly regular packages sleep. idly
36	120	1	1	42	42845.04	0.09	0.00	N	O	1996-02-03	1996-01-21	1996-02-23	COLLECT COD              	SHIP      	 careful courts. special 
37	23	8	1	40	36920.80	0.09	0.03	A	F	1992-07-21	1992-08-01	1992-08-15	NONE                     	REG AIR   	luffily regular requests. slyly final acco
37	127	6	2	39	40057.68	0.05	0.02	A	F	1992-07-02	1992-08-18	1992-07-28	TAKE BACK RETURN         	RAIL      	the final requests. ca
37	13	7	3	43	39259.43	0.05	0.08	A	F	1992-07-10	1992-07-06	1992-08-02	DELIVER IN PERSON        	TRUCK     	iously ste
38	176	5	1	44	47351.48	0.04	0.02	N	O	1996-09-29	1996-11-17	1996-09-30	COLLECT COD              	MAIL      	s. blithely unusual theodolites am
39	3	10	1	44	39732.00	0.09	0.06	N	O	1996-11-14	1996-12-15	1996-12-12	COLLECT COD              	RAIL      	eodolites. careful
39	187	8	2	26	28266.68	0.08	0.04	N	O	1996-11-04	1996-10-20	1996-11-20	NONE                     	FOB       	ckages across the slyly silent
39	68	3	3	46	44530.76	0.06	0.08	N	O	1996-09-26	1996-12-19	1996-10-26	DELIVER IN PERSON        	AIR       	he carefully e
39	21	6	4	32	29472.64	0.07	0.05	N	O	1996-10-02	1996-12-19	1996-10-14	COLLECT COD              	MAIL      	heodolites sleep silently pending foxes. ac
39	55	10	5	43	41067.15	0.01	0.01	N	O	1996-10-17	1996-11-14	1996-10-26	COLLECT COD              	MAIL      	yly regular i
39	95	7	6	40	39803.60	0.06	0.05	N	O	1996-12-08	1996-10-22	1997-01-01	COLLECT COD              	AIR       	quickly ironic fox
64	86	7	1	21	20707.68	0.05	0.02	R	F	1994-09-30	1994-09-18	1994-10-26	DELIVER IN PERSON        	REG AIR   	ch slyly final, thin platelets.
67	22	5	1	4	3688.08	0.09	0.04	N	O	1997-04-17	1997-01-31	1997-04-20	NONE                     	SHIP      	 cajole thinly expres
67	21	10	2	12	11052.24	0.09	0.05	N	O	1997-01-27	1997-02-21	1997-02-22	NONE                     	REG AIR   	 even packages cajole
67	174	4	3	5	5370.85	0.03	0.07	N	O	1997-02-20	1997-02-12	1997-02-21	DELIVER IN PERSON        	TRUCK     	y unusual packages thrash pinto 
67	88	9	4	44	43475.52	0.08	0.06	N	O	1997-03-18	1997-01-29	1997-04-13	DELIVER IN PERSON        	RAIL      	se quickly above the even, express reques
67	41	10	5	23	21643.92	0.05	0.07	N	O	1997-04-19	1997-02-14	1997-05-06	DELIVER IN PERSON        	REG AIR   	ly regular deposit
67	179	9	6	29	31295.93	0.02	0.05	N	O	1997-01-25	1997-01-27	1997-01-27	DELIVER IN PERSON        	FOB       	ultipliers 
70	65	2	1	8	7720.48	0.03	0.08	R	F	1994-01-12	1994-02-27	1994-01-14	TAKE BACK RETURN         	FOB       	ggle. carefully pending dependenc
70	197	10	2	13	14263.47	0.06	0.06	A	F	1994-03-03	1994-02-13	1994-03-26	COLLECT COD              	AIR       	lyly special packag
70	180	8	3	1	1080.18	0.03	0.05	R	F	1994-01-26	1994-03-05	1994-01-28	TAKE BACK RETURN         	RAIL      	quickly. fluffily unusual theodolites c
70	46	9	4	11	10406.44	0.01	0.05	A	F	1994-03-17	1994-03-17	1994-03-27	NONE                     	MAIL      	alongside of the deposits. fur
70	38	9	5	37	34707.11	0.09	0.04	R	F	1994-02-13	1994-03-16	1994-02-21	COLLECT COD              	MAIL      	n accounts are. q
70	56	8	6	19	18164.95	0.06	0.03	A	F	1994-01-26	1994-02-17	1994-02-06	TAKE BACK RETURN         	SHIP      	 packages wake pending accounts.
96	124	7	1	23	23554.76	0.10	0.06	A	F	1994-07-19	1994-06-29	1994-07-25	DELIVER IN PERSON        	TRUCK     	ep-- carefully reg
96	136	7	2	30	31083.90	0.01	0.06	R	F	1994-06-03	1994-05-29	1994-06-22	DELIVER IN PERSON        	TRUCK     	e quickly even ideas. furiou
99	88	9	1	10	9880.80	0.02	0.01	A	F	1994-05-18	1994-06-03	1994-05-23	COLLECT COD              	RAIL      	kages. requ
99	124	5	2	5	5120.60	0.02	0.07	R	F	1994-05-06	1994-05-28	1994-05-20	TAKE BACK RETURN         	RAIL      	ests cajole fluffily waters. blithe
99	135	1	3	42	43475.46	0.02	0.02	A	F	1994-04-19	1994-05-18	1994-04-20	NONE                     	RAIL      	kages are fluffily furiously ir
99	109	2	4	36	36327.60	0.09	0.02	A	F	1994-07-04	1994-04-17	1994-07-30	DELIVER IN PERSON        	AIR       	slyly. slyly e
102	89	10	1	37	36595.96	0.06	0.00	N	O	1997-07-24	1997-08-02	1997-08-07	TAKE BACK RETURN         	SHIP      	ully across the ideas. final deposit
102	170	5	2	34	36385.78	0.03	0.08	N	O	1997-08-09	1997-07-28	1997-08-26	TAKE BACK RETURN         	SHIP      	eposits cajole across
102	183	4	3	25	27079.50	0.01	0.01	N	O	1997-07-31	1997-07-24	1997-08-17	NONE                     	RAIL      	bits. ironic accoun
102	62	7	4	15	14430.90	0.07	0.07	N	O	1997-06-02	1997-07-13	1997-06-04	DELIVER IN PERSON        	SHIP      	final packages. carefully even excu
103	195	9	1	6	6571.14	0.03	0.05	N	O	1996-10-11	1996-07-25	1996-10-28	NONE                     	FOB       	cajole. carefully ex
103	11	5	2	37	33707.37	0.02	0.07	N	O	1996-09-17	1996-07-27	1996-09-20	TAKE BACK RETURN         	MAIL      	ies. quickly ironic requests use blithely
103	29	10	3	23	21367.46	0.01	0.04	N	O	1996-09-11	1996-09-18	1996-09-26	NONE                     	FOB       	ironic accou
103	30	9	4	32	29760.96	0.01	0.07	N	O	1996-07-30	1996-08-06	1996-08-04	NONE                     	RAIL      	kages doze. special, regular deposit
133	104	7	1	27	27110.70	0.00	0.02	N	O	1997-12-21	1998-02-23	1997-12-27	TAKE BACK RETURN         	MAIL      	yly even gifts after the sl
133	177	5	2	12	12926.04	0.02	0.06	N	O	1997-12-02	1998-01-15	1997-12-29	DELIVER IN PERSON        	REG AIR   	ts cajole fluffily quickly i
133	118	8	3	29	29525.19	0.09	0.08	N	O	1998-02-28	1998-01-30	1998-03-09	DELIVER IN PERSON        	RAIL      	 the carefully regular theodoli
133	90	1	4	11	10890.99	0.06	0.01	N	O	1998-03-21	1998-01-15	1998-04-04	DELIVER IN PERSON        	REG AIR   	e quickly across the dolphins
134	1	2	1	21	18921.00	0.00	0.03	A	F	1992-07-17	1992-07-08	1992-07-26	COLLECT COD              	SHIP      	s. quickly regular
134	165	2	2	35	37280.60	0.06	0.07	A	F	1992-08-23	1992-06-01	1992-08-24	NONE                     	MAIL      	ajole furiously. instructio
134	189	10	3	26	28318.68	0.09	0.06	A	F	1992-06-20	1992-07-12	1992-07-16	NONE                     	RAIL      	 among the pending depos
134	145	6	4	47	49121.58	0.05	0.00	A	F	1992-08-16	1992-07-06	1992-08-28	NONE                     	REG AIR   	s! carefully unusual requests boost careful
134	36	7	5	12	11232.36	0.05	0.02	A	F	1992-07-03	1992-06-01	1992-07-11	COLLECT COD              	TRUCK     	nts are quic
134	134	10	6	12	12409.56	0.00	0.00	A	F	1992-08-08	1992-07-07	1992-08-20	TAKE BACK RETURN         	FOB       	lyly regular pac
135	109	10	1	47	47427.70	0.06	0.08	N	O	1996-02-18	1996-01-01	1996-02-25	COLLECT COD              	RAIL      	ctions wake slyly abo
135	199	3	2	21	23082.99	0.00	0.07	N	O	1996-02-11	1996-01-12	1996-02-13	DELIVER IN PERSON        	SHIP      	 deposits believe. furiously regular p
135	158	10	3	33	34918.95	0.02	0.00	N	O	1996-01-03	1995-11-21	1996-02-01	TAKE BACK RETURN         	MAIL      	ptotes boost slowly care
135	68	7	4	34	32914.04	0.02	0.03	N	O	1996-01-12	1996-01-19	1996-02-05	NONE                     	TRUCK     	counts doze against the blithely ironi
135	137	8	5	20	20742.60	0.01	0.04	N	O	1996-01-25	1995-11-20	1996-02-09	NONE                     	MAIL      	theodolites. quickly p
135	115	5	6	13	13196.43	0.04	0.02	N	O	1995-11-12	1995-12-22	1995-11-17	NONE                     	FOB       	nal ideas. final instr
160	15	2	1	36	32940.36	0.07	0.01	N	O	1997-03-11	1997-03-11	1997-03-20	COLLECT COD              	MAIL      	old, ironic deposits are quickly abov
160	87	8	2	22	21715.76	0.00	0.04	N	O	1997-02-18	1997-03-05	1997-03-05	COLLECT COD              	RAIL      	ncies about the request
160	21	10	3	34	31314.68	0.01	0.05	N	O	1997-01-31	1997-03-13	1997-02-14	NONE                     	FOB       	st sleep even gifts. dependencies along
163	168	3	1	43	45930.88	0.01	0.00	N	O	1997-09-19	1997-11-19	1997-10-03	COLLECT COD              	REG AIR   	al, bold dependencies wake. iron
163	121	2	2	13	13274.56	0.01	0.04	N	O	1997-11-11	1997-10-18	1997-12-07	DELIVER IN PERSON        	TRUCK     	inal requests. even pinto beans hag
163	37	3	3	27	25299.81	0.04	0.08	N	O	1997-12-26	1997-11-28	1998-01-05	COLLECT COD              	REG AIR   	ously express dependen
163	193	5	4	5	5465.95	0.02	0.00	N	O	1997-11-17	1997-10-09	1997-12-05	DELIVER IN PERSON        	TRUCK     	 must belie
163	127	2	5	12	12325.44	0.10	0.00	N	O	1997-12-18	1997-10-26	1997-12-22	COLLECT COD              	TRUCK     	ly blithe accounts cajole 
163	191	4	6	20	21823.80	0.00	0.07	N	O	1997-09-27	1997-11-15	1997-10-07	TAKE BACK RETURN         	FOB       	tructions integrate b
164	92	4	1	26	25794.34	0.09	0.04	A	F	1993-01-04	1992-11-21	1993-01-07	NONE                     	RAIL      	s. blithely special courts are blithel
164	19	6	2	24	22056.24	0.05	0.05	R	F	1992-12-22	1992-11-27	1993-01-06	NONE                     	AIR       	side of the slyly unusual theodolites. f
164	126	9	3	38	38992.56	0.03	0.06	R	F	1992-12-04	1992-11-23	1993-01-02	TAKE BACK RETURN         	AIR       	counts cajole fluffily regular packages. b
164	18	2	4	32	29376.32	0.05	0.01	R	F	1992-12-21	1992-12-23	1992-12-28	COLLECT COD              	RAIL      	ts wake again
164	148	1	5	43	45070.02	0.06	0.01	R	F	1992-11-26	1993-01-03	1992-12-08	COLLECT COD              	RAIL      	y carefully regular dep
164	109	10	6	27	27245.70	0.10	0.04	R	F	1992-12-23	1993-01-16	1993-01-10	DELIVER IN PERSON        	AIR       	ayers wake carefully a
164	4	7	7	23	20792.00	0.09	0.04	A	F	1992-11-03	1992-12-02	1992-11-12	NONE                     	REG AIR   	ress packages haggle ideas. blithely spec
166	65	2	1	37	35707.22	0.09	0.03	N	O	1995-11-16	1995-10-17	1995-12-13	NONE                     	MAIL      	lar frays wake blithely a
166	167	8	2	13	13873.08	0.09	0.05	N	O	1995-11-09	1995-11-18	1995-11-14	COLLECT COD              	SHIP      	fully above the blithely fina
166	100	2	3	41	41004.10	0.07	0.03	N	O	1995-11-13	1995-11-07	1995-12-08	COLLECT COD              	FOB       	hily along the blithely pending fo
166	46	3	4	8	7568.32	0.05	0.02	N	O	1995-12-30	1995-11-29	1996-01-29	DELIVER IN PERSON        	RAIL      	e carefully bold 
167	102	3	1	28	28058.80	0.06	0.01	R	F	1993-02-19	1993-02-16	1993-03-03	DELIVER IN PERSON        	TRUCK     	sly during the u
167	172	2	2	27	28948.59	0.09	0.00	R	F	1993-05-01	1993-03-31	1993-05-31	TAKE BACK RETURN         	FOB       	eans affix furiously-- packages
192	98	1	1	23	22956.07	0.00	0.00	N	O	1998-02-05	1998-02-06	1998-03-07	TAKE BACK RETURN         	AIR       	ly pending theodolites haggle quickly fluf
192	162	7	2	20	21243.20	0.07	0.01	N	O	1998-03-13	1998-02-02	1998-03-31	TAKE BACK RETURN         	REG AIR   	tes. carefu
192	111	8	3	15	15166.65	0.09	0.01	N	O	1998-01-30	1998-02-10	1998-02-23	TAKE BACK RETURN         	TRUCK     	he ironic requests haggle about
192	197	1	4	2	2194.38	0.06	0.02	N	O	1998-03-06	1998-02-03	1998-03-24	COLLECT COD              	SHIP      	s. dependencies nag furiously alongside
192	83	4	5	25	24577.00	0.02	0.03	N	O	1998-02-15	1998-01-11	1998-03-17	COLLECT COD              	TRUCK     	. carefully regular
192	142	9	6	45	46896.30	0.00	0.05	N	O	1998-03-11	1998-01-09	1998-04-03	NONE                     	MAIL      	equests. ideas sleep idea
196	136	7	1	19	19686.47	0.03	0.02	R	F	1993-04-17	1993-05-27	1993-04-30	NONE                     	SHIP      	sts maintain foxes. furiously regular p
196	10	3	2	15	13650.15	0.03	0.04	A	F	1993-07-05	1993-05-08	1993-07-06	TAKE BACK RETURN         	SHIP      	s accounts. furio
197	99	1	1	39	38964.51	0.02	0.04	N	O	1995-07-21	1995-07-01	1995-08-14	TAKE BACK RETURN         	AIR       	press accounts. daringly sp
197	178	8	2	8	8625.36	0.09	0.02	A	F	1995-04-17	1995-07-01	1995-04-27	DELIVER IN PERSON        	SHIP      	y blithely even deposits. blithely fina
197	156	4	3	17	17954.55	0.06	0.02	N	O	1995-08-02	1995-06-23	1995-08-03	COLLECT COD              	REG AIR   	ts. careful
197	18	5	4	25	22950.25	0.04	0.01	N	F	1995-06-13	1995-05-23	1995-06-24	TAKE BACK RETURN         	FOB       	s-- quickly final accounts
197	42	9	5	14	13188.56	0.09	0.01	R	F	1995-05-08	1995-05-24	1995-05-12	TAKE BACK RETURN         	RAIL      	use slyly slyly silent depo
197	106	1	6	1	1006.10	0.07	0.05	N	O	1995-07-15	1995-06-21	1995-08-11	COLLECT COD              	RAIL      	 even, thin dependencies sno
198	57	8	1	33	31582.65	0.07	0.02	N	O	1998-01-05	1998-03-20	1998-01-10	TAKE BACK RETURN         	TRUCK     	carefully caref
198	16	10	2	20	18320.20	0.03	0.00	N	O	1998-01-15	1998-03-31	1998-01-25	DELIVER IN PERSON        	FOB       	carefully final escapades a
198	149	2	3	15	15737.10	0.04	0.02	N	O	1998-04-12	1998-02-26	1998-04-15	COLLECT COD              	MAIL      	es. quickly pending deposits s
198	11	5	4	35	31885.35	0.08	0.02	N	O	1998-02-27	1998-03-23	1998-03-14	TAKE BACK RETURN         	RAIL      	ests nod quickly furiously sly pinto be
198	102	3	5	33	33069.30	0.02	0.01	N	O	1998-03-22	1998-03-12	1998-04-14	DELIVER IN PERSON        	SHIP      	ending foxes acr
199	133	9	1	50	51656.50	0.02	0.00	N	O	1996-06-12	1996-06-03	1996-07-04	DELIVER IN PERSON        	MAIL      	essly regular ideas boost sly
199	134	5	2	30	31023.90	0.08	0.05	N	O	1996-03-27	1996-05-29	1996-04-14	NONE                     	TRUCK     	ilent packages doze quickly. thinly 
225	172	3	1	4	4288.68	0.09	0.07	N	O	1995-08-05	1995-08-19	1995-09-03	TAKE BACK RETURN         	SHIP      	ng the ironic packages. asymptotes among 
225	131	7	2	3	3093.39	0.00	0.08	N	O	1995-07-25	1995-07-08	1995-08-17	DELIVER IN PERSON        	REG AIR   	 fluffily about the carefully bold a
225	199	2	3	45	49463.55	0.06	0.01	N	O	1995-08-17	1995-08-20	1995-08-30	TAKE BACK RETURN         	FOB       	the slyly even platelets use aro
225	147	4	4	24	25131.36	0.00	0.06	N	O	1995-09-23	1995-08-05	1995-10-16	COLLECT COD              	MAIL      	ironic accounts are final account
225	8	5	5	31	28148.00	0.04	0.06	N	O	1995-06-21	1995-07-24	1995-07-04	TAKE BACK RETURN         	FOB       	special platelets. quickly r
225	132	8	6	12	12385.56	0.00	0.00	A	F	1995-06-04	1995-07-15	1995-06-08	COLLECT COD              	MAIL      	 unusual requests. bus
225	142	1	7	44	45854.16	0.10	0.06	N	O	1995-09-22	1995-08-16	1995-10-22	NONE                     	REG AIR   	leep slyly 
229	84	5	1	20	19681.60	0.02	0.03	R	F	1994-01-11	1994-01-31	1994-01-26	DELIVER IN PERSON        	REG AIR   	le. instructions use across the quickly fin
229	129	10	2	29	29844.48	0.07	0.00	A	F	1994-03-15	1994-03-02	1994-03-26	COLLECT COD              	SHIP      	s, final request
229	79	10	3	28	27413.96	0.02	0.02	R	F	1994-02-10	1994-02-02	1994-03-10	DELIVER IN PERSON        	FOB       	 final, regular requests. platel
229	177	6	4	3	3231.51	0.02	0.08	R	F	1994-03-22	1994-03-24	1994-04-04	DELIVER IN PERSON        	REG AIR   	posits. furiously regular theodol
229	156	1	5	33	34852.95	0.03	0.06	R	F	1994-03-25	1994-02-11	1994-04-13	NONE                     	FOB       	 deposits; bold, ruthless theodolites
229	106	9	6	29	29176.90	0.04	0.00	R	F	1994-01-14	1994-02-16	1994-01-22	NONE                     	FOB       	uriously pending 
256	89	10	1	22	21759.76	0.09	0.02	R	F	1994-01-12	1993-12-28	1994-01-26	COLLECT COD              	FOB       	ke quickly ironic, ironic deposits. reg
256	119	6	2	40	40764.40	0.10	0.01	A	F	1993-11-30	1993-12-13	1993-12-02	NONE                     	FOB       	nal theodolites. deposits cajole s
256	130	9	3	45	46355.85	0.02	0.08	R	F	1994-01-14	1994-01-17	1994-02-10	COLLECT COD              	SHIP      	 grouches. ideas wake quickly ar
257	147	8	1	7	7329.98	0.05	0.02	N	O	1998-06-18	1998-05-15	1998-06-27	COLLECT COD              	FOB       	ackages sleep bold realms. f
258	107	4	1	8	8056.80	0.00	0.07	R	F	1994-01-20	1994-03-21	1994-02-09	NONE                     	REG AIR   	ully about the fluffily silent dependencies
258	197	1	2	40	43887.60	0.10	0.01	A	F	1994-03-13	1994-02-23	1994-04-05	DELIVER IN PERSON        	FOB       	silent frets nod daringly busy, bold
258	162	3	3	45	47797.20	0.07	0.07	R	F	1994-03-04	1994-02-13	1994-03-30	DELIVER IN PERSON        	TRUCK     	regular excuses-- fluffily ruthl
258	133	9	4	31	32027.03	0.02	0.05	A	F	1994-04-20	1994-03-20	1994-04-28	COLLECT COD              	REG AIR   	 slyly blithely special mul
258	36	2	5	25	23400.75	0.08	0.02	A	F	1994-04-13	1994-02-26	1994-04-29	TAKE BACK RETURN         	TRUCK     	leep pending packages.
258	147	4	6	36	37697.04	0.09	0.04	A	F	1994-01-11	1994-03-04	1994-01-18	DELIVER IN PERSON        	AIR       	nic asymptotes. slyly silent r
259	99	10	1	14	13987.26	0.00	0.08	A	F	1993-12-17	1993-12-09	1993-12-31	COLLECT COD              	SHIP      	ons against the express acco
259	162	1	2	14	14870.24	0.03	0.05	R	F	1993-11-10	1993-11-20	1993-11-17	DELIVER IN PERSON        	FOB       	ully even, regul
259	24	5	3	42	38808.84	0.09	0.00	R	F	1993-10-20	1993-11-18	1993-11-12	NONE                     	TRUCK     	the slyly ironic pinto beans. fi
259	196	10	4	3	3288.57	0.08	0.06	R	F	1993-10-04	1993-11-07	1993-10-14	TAKE BACK RETURN         	SHIP      	ng slyly at the accounts.
259	193	6	5	6	6559.14	0.00	0.05	R	F	1993-12-05	1993-12-22	1993-12-21	COLLECT COD              	TRUCK     	 requests sleep
262	192	3	1	39	42595.41	0.01	0.05	N	O	1996-01-15	1996-02-18	1996-01-28	COLLECT COD              	RAIL      	usual, regular requests
262	61	6	2	33	31714.98	0.09	0.03	N	O	1996-03-10	1996-01-31	1996-03-27	TAKE BACK RETURN         	AIR       	atelets sleep furiously. requests cajole. b
262	59	1	3	35	33566.75	0.05	0.08	N	O	1996-03-12	1996-02-14	1996-04-11	COLLECT COD              	MAIL      	lites cajole along the pending packag
263	24	9	1	22	20328.44	0.06	0.08	R	F	1994-08-24	1994-06-20	1994-09-09	NONE                     	FOB       	efully express fo
263	85	6	2	9	8865.72	0.08	0.00	A	F	1994-07-21	1994-07-16	1994-08-08	TAKE BACK RETURN         	TRUCK     	lms wake bl
263	143	2	3	50	52157.00	0.06	0.04	R	F	1994-08-18	1994-07-31	1994-08-22	NONE                     	TRUCK     	re the packages. special
288	51	3	1	31	29482.55	0.00	0.03	N	O	1997-03-17	1997-04-28	1997-04-06	TAKE BACK RETURN         	AIR       	instructions wa
288	117	1	2	49	49838.39	0.08	0.05	N	O	1997-04-19	1997-05-19	1997-05-18	TAKE BACK RETURN         	TRUCK     	ic excuses sleep always spe
288	99	10	3	36	35967.24	0.02	0.02	N	O	1997-02-22	1997-05-07	1997-03-07	TAKE BACK RETURN         	TRUCK     	yly pending excu
288	79	10	4	19	18602.33	0.07	0.07	N	O	1997-03-14	1997-04-04	1997-03-26	NONE                     	MAIL      	deposits. blithely quick courts ar
288	162	9	5	31	32926.96	0.10	0.04	N	O	1997-05-29	1997-04-24	1997-06-20	TAKE BACK RETURN         	RAIL      	ns. fluffily
290	6	1	1	35	31710.00	0.01	0.02	R	F	1994-04-01	1994-02-05	1994-04-27	NONE                     	MAIL      	ove the final foxes detect slyly fluffily
290	129	4	2	2	2058.24	0.05	0.04	A	F	1994-01-30	1994-02-13	1994-02-21	TAKE BACK RETURN         	TRUCK     	. permanently furious reques
290	2	5	3	5	4510.00	0.03	0.05	A	F	1994-01-19	1994-02-24	1994-01-27	NONE                     	MAIL      	ans integrate. requests sleep. fur
290	124	9	4	23	23554.76	0.05	0.08	R	F	1994-03-14	1994-02-21	1994-04-09	NONE                     	AIR       	refully unusual packages. 
293	9	6	1	14	12726.00	0.02	0.05	R	F	1992-10-19	1992-12-23	1992-11-10	DELIVER IN PERSON        	SHIP      	es. packages above the
293	187	8	2	11	11958.98	0.10	0.04	R	F	1992-12-24	1992-12-01	1993-01-12	COLLECT COD              	MAIL      	 affix carefully quickly special idea
293	118	8	3	13	13235.43	0.04	0.02	A	F	1992-12-17	1992-12-26	1992-12-22	COLLECT COD              	RAIL      	 wake after the quickly even deposits. bli
295	198	10	1	29	31847.51	0.02	0.07	A	F	1994-11-09	1994-12-08	1994-12-07	COLLECT COD              	MAIL      	inst the carefully ironic pinto beans. blit
295	92	6	2	26	25794.34	0.04	0.03	R	F	1994-12-13	1994-11-30	1995-01-06	DELIVER IN PERSON        	AIR       	ts above the slyly regular requests x-ray q
295	16	10	3	8	7328.08	0.10	0.07	R	F	1995-01-13	1994-11-17	1995-01-25	NONE                     	TRUCK     	 final instructions h
295	61	10	4	26	24987.56	0.10	0.04	A	F	1995-01-12	1994-11-22	1995-01-22	DELIVER IN PERSON        	MAIL      	 carefully iron
320	5	2	1	30	27150.00	0.05	0.01	N	O	1997-12-04	1998-01-21	1997-12-13	NONE                     	RAIL      	 ironic, final accounts wake quick de
320	193	5	2	13	14211.47	0.03	0.00	N	O	1997-12-16	1997-12-26	1997-12-17	TAKE BACK RETURN         	AIR       	he furiously regular pinto beans. car
321	1	8	1	21	18921.00	0.01	0.08	A	F	1993-07-18	1993-04-24	1993-08-13	TAKE BACK RETURN         	REG AIR   	hockey players sleep slyly sl
321	141	4	2	41	42686.74	0.08	0.07	R	F	1993-06-21	1993-06-07	1993-07-09	NONE                     	REG AIR   	special packages shall have to doze blit
327	144	3	1	16	16706.24	0.03	0.01	N	O	1995-07-05	1995-06-07	1995-07-09	TAKE BACK RETURN         	TRUCK     	cial ideas sleep af
327	42	9	2	9	8478.36	0.09	0.05	A	F	1995-05-24	1995-07-11	1995-06-05	NONE                     	AIR       	 asymptotes are fu
352	64	5	1	17	16389.02	0.07	0.05	R	F	1994-06-02	1994-05-31	1994-06-29	NONE                     	FOB       	pending deposits sleep furiously 
353	120	7	1	41	41824.92	0.00	0.06	A	F	1994-03-25	1994-03-31	1994-03-30	DELIVER IN PERSON        	AIR       	refully final theodoli
353	148	9	2	29	30396.06	0.09	0.00	A	F	1994-01-11	1994-03-19	1994-02-09	COLLECT COD              	FOB       	ctions impr
353	135	1	3	12	12421.56	0.06	0.01	R	F	1994-01-02	1994-03-26	1994-01-19	DELIVER IN PERSON        	RAIL      	g deposits cajole 
353	78	7	4	46	44991.22	0.00	0.04	A	F	1994-04-14	1994-01-31	1994-05-05	DELIVER IN PERSON        	FOB       	 ironic dolphins 
353	117	4	5	9	9153.99	0.02	0.02	A	F	1994-03-15	1994-03-20	1994-03-18	TAKE BACK RETURN         	RAIL      	ual accounts! carefu
353	103	4	6	39	39120.90	0.02	0.05	A	F	1994-01-15	1994-03-30	1994-02-01	NONE                     	MAIL      	losely quickly even accounts. c
357	114	5	1	26	26366.86	0.06	0.03	N	O	1996-12-28	1996-11-26	1997-01-13	NONE                     	FOB       	 carefully pending accounts use a
357	186	7	2	36	39102.48	0.07	0.06	N	O	1996-12-28	1996-11-13	1997-01-24	DELIVER IN PERSON        	AIR       	d the carefully even requests. 
357	165	2	3	32	34085.12	0.05	0.07	N	O	1997-01-28	1996-12-29	1997-02-14	NONE                     	MAIL      	y above the carefully final accounts
359	166	7	1	30	31984.80	0.00	0.08	A	F	1995-01-06	1995-02-20	1995-01-20	TAKE BACK RETURN         	AIR       	uses detect spec
359	12	9	2	18	16416.18	0.00	0.03	A	F	1995-01-27	1995-03-18	1995-01-31	DELIVER IN PERSON        	RAIL      	unusual warthogs. ironically sp
359	132	8	3	17	17546.21	0.07	0.06	A	F	1995-01-31	1995-03-18	1995-02-10	COLLECT COD              	SHIP      	sts according to the blithely
359	90	1	4	38	37623.42	0.10	0.08	R	F	1995-03-30	1995-01-20	1995-04-25	DELIVER IN PERSON        	RAIL      	g furiously. regular, sile
359	168	5	5	11	11749.76	0.01	0.03	A	F	1995-02-15	1995-01-27	1995-02-18	NONE                     	FOB       	rets wake blithely. slyly final dep
359	183	4	6	23	24913.14	0.04	0.07	R	F	1995-01-31	1995-03-11	1995-02-16	DELIVER IN PERSON        	REG AIR   	ic courts snooze quickly furiously final fo
385	167	6	1	7	7470.12	0.05	0.06	N	O	1996-05-23	1996-05-09	1996-06-06	DELIVER IN PERSON        	REG AIR   	 special asymptote
385	54	9	2	46	43886.30	0.08	0.07	N	O	1996-03-29	1996-05-17	1996-04-18	NONE                     	REG AIR   	lthily ironic f
386	153	5	1	39	41072.85	0.10	0.07	A	F	1995-05-10	1995-02-28	1995-05-25	NONE                     	SHIP      	hely. carefully regular accounts hag
386	69	4	2	16	15504.96	0.06	0.01	A	F	1995-04-12	1995-04-18	1995-05-11	DELIVER IN PERSON        	MAIL      	lithely fluffi
386	131	2	3	37	38151.81	0.09	0.04	A	F	1995-05-23	1995-03-01	1995-05-25	TAKE BACK RETURN         	MAIL      	ending pearls breach fluffily. slyly pen
389	190	1	1	2	2180.38	0.09	0.00	R	F	1994-04-13	1994-04-10	1994-04-25	TAKE BACK RETURN         	RAIL      	fts. courts eat blithely even dependenc
391	122	1	1	14	14309.68	0.09	0.02	R	F	1995-02-11	1995-02-03	1995-02-13	TAKE BACK RETURN         	TRUCK     	 escapades sleep furiously about 
418	19	3	1	31	28489.31	0.00	0.03	N	F	1995-06-05	1995-06-18	1995-06-26	COLLECT COD              	FOB       	final theodolites. fluffil
418	2	5	2	1	902.00	0.04	0.07	N	O	1995-06-23	1995-06-16	1995-07-23	DELIVER IN PERSON        	AIR       	regular, silent pinto
418	35	1	3	3	2805.09	0.04	0.06	N	O	1995-06-29	1995-07-12	1995-07-01	COLLECT COD              	AIR       	ly furiously regular w
419	153	8	1	33	34753.95	0.05	0.02	N	O	1996-11-06	1996-12-25	1996-11-20	TAKE BACK RETURN         	TRUCK     	y above the bli
419	65	2	2	32	30881.92	0.01	0.06	N	O	1996-12-04	1996-12-04	1996-12-24	COLLECT COD              	SHIP      	blithely regular requests. special pinto
419	71	1	3	15	14566.05	0.07	0.04	N	O	1996-12-17	1996-11-28	1996-12-19	TAKE BACK RETURN         	REG AIR   	 sleep final, regular theodolites. fluffi
419	9	6	4	15	13635.00	0.01	0.02	N	O	1997-01-09	1996-12-22	1997-01-25	COLLECT COD              	FOB       	of the careful, thin theodolites. quickly s
419	149	2	5	17	17835.38	0.01	0.00	N	O	1997-01-13	1996-12-20	1997-02-01	COLLECT COD              	REG AIR   	lar dependencies: carefully regu
448	126	7	1	4	4104.48	0.00	0.04	N	O	1995-11-25	1995-10-20	1995-11-26	TAKE BACK RETURN         	MAIL      	nts thrash quickly among the b
448	173	1	2	46	49365.82	0.05	0.00	N	O	1995-08-31	1995-09-30	1995-09-09	COLLECT COD              	SHIP      	 to the fluffily ironic packages.
448	27	6	3	35	32445.70	0.10	0.08	N	O	1995-09-27	1995-11-19	1995-10-20	COLLECT COD              	REG AIR   	ses nag quickly quickly ir
448	170	1	4	8	8561.36	0.10	0.00	N	O	1995-11-02	1995-10-16	1995-11-15	COLLECT COD              	TRUCK     	ounts wake blithely. furiously pending
448	138	9	5	23	23876.99	0.02	0.05	N	O	1995-09-26	1995-11-02	1995-10-17	NONE                     	SHIP      	ious, final gifts
449	152	7	1	12	12625.80	0.02	0.08	N	O	1995-11-06	1995-08-25	1995-11-18	TAKE BACK RETURN         	SHIP      	ly. blithely ironic 
449	109	6	2	4	4036.40	0.10	0.06	N	O	1995-10-27	1995-09-14	1995-11-21	DELIVER IN PERSON        	FOB       	are fluffily. requests are furiously
449	10	1	3	3	2730.03	0.07	0.08	N	O	1995-07-28	1995-09-11	1995-08-01	NONE                     	RAIL      	 bold deposits. express theodolites haggle
449	158	3	4	22	23279.30	0.07	0.00	N	O	1995-08-17	1995-09-04	1995-09-10	COLLECT COD              	FOB       	furiously final theodolites eat careful
451	130	9	1	36	37084.68	0.02	0.06	N	O	1998-06-18	1998-08-14	1998-06-20	TAKE BACK RETURN         	AIR       	rges can haggle carefully ironic, dogged 
451	33	4	2	42	39187.26	0.05	0.01	N	O	1998-08-01	1998-08-05	1998-08-30	DELIVER IN PERSON        	TRUCK     	express excuses. blithely ironic pin
451	87	8	3	1	987.08	0.07	0.05	N	O	1998-07-13	1998-07-03	1998-08-04	DELIVER IN PERSON        	AIR       	 carefully ironic packages solve furiously 
451	77	5	4	28	27357.96	0.04	0.05	N	O	1998-06-16	1998-07-09	1998-06-17	DELIVER IN PERSON        	SHIP      	 theodolites. even cou
452	115	6	1	2	2030.22	0.04	0.03	N	O	1997-12-26	1998-01-03	1998-01-12	COLLECT COD              	FOB       	y express instru
481	19	9	1	17	15623.17	0.07	0.05	A	F	1992-10-21	1992-12-09	1992-11-19	DELIVER IN PERSON        	MAIL      	. quickly final accounts among the 
481	21	2	2	19	17499.38	0.08	0.01	R	F	1993-01-09	1992-11-27	1993-01-14	TAKE BACK RETURN         	AIR       	p blithely after t
481	186	7	3	42	45619.56	0.08	0.08	A	F	1992-11-27	1992-11-11	1992-12-08	COLLECT COD              	RAIL      	mptotes are furiously among the iron
481	82	3	4	11	10802.88	0.05	0.06	A	F	1993-01-12	1992-11-17	1993-02-05	NONE                     	FOB       	eful attai
481	112	9	5	31	31375.41	0.05	0.01	A	F	1993-01-15	1992-12-31	1993-01-21	DELIVER IN PERSON        	AIR       	usly final packages believe. quick
482	138	9	1	32	33220.16	0.00	0.02	N	O	1996-05-22	1996-05-14	1996-05-29	NONE                     	SHIP      	usual deposits affix against 
482	122	5	2	1	1022.12	0.05	0.08	N	O	1996-05-29	1996-05-20	1996-05-31	COLLECT COD              	AIR       	es. quickly ironic escapades sleep furious
482	62	9	3	31	29823.86	0.04	0.03	N	O	1996-06-01	1996-05-06	1996-06-17	NONE                     	MAIL      	 blithe pin
482	196	7	4	8	8769.52	0.02	0.05	N	O	1996-04-19	1996-05-05	1996-04-21	NONE                     	TRUCK     	tructions near the final, regular ideas de
482	39	10	5	46	43195.38	0.01	0.06	N	O	1996-07-19	1996-06-05	1996-08-10	NONE                     	MAIL      	furiously thin realms. final, fina
482	79	10	6	19	18602.33	0.04	0.00	N	O	1996-03-27	1996-04-25	1996-04-15	NONE                     	FOB       	ts hinder carefully silent requests
483	33	9	1	8	7464.24	0.00	0.08	N	O	1995-08-22	1995-08-23	1995-09-18	COLLECT COD              	RAIL      	osits. carefully fin
483	80	1	2	23	22541.84	0.04	0.06	N	O	1995-07-20	1995-08-11	1995-08-04	DELIVER IN PERSON        	MAIL      	requests was quickly against th
483	88	9	3	9	8892.72	0.04	0.03	N	O	1995-09-10	1995-09-02	1995-09-13	NONE                     	AIR       	 carefully express ins
487	92	3	1	47	46628.23	0.06	0.06	R	F	1992-09-30	1992-10-08	1992-10-24	NONE                     	TRUCK     	tions. blithely reg
487	83	4	2	2	1966.16	0.02	0.06	R	F	1992-10-19	1992-11-04	1992-11-11	COLLECT COD              	TRUCK     	oss the unusual pinto beans. reg
513	62	7	1	20	19241.20	0.09	0.07	N	O	1995-07-12	1995-05-31	1995-07-31	NONE                     	AIR       	efully ironic ideas doze slyl
513	122	5	2	44	44973.28	0.01	0.01	N	O	1995-07-14	1995-07-14	1995-08-12	NONE                     	MAIL      	kages sleep boldly ironic theodolites. acco
516	25	4	1	11	10175.22	0.01	0.06	N	O	1998-05-02	1998-05-23	1998-05-12	DELIVER IN PERSON        	FOB       	ongside of the blithely final reque
518	165	6	1	30	31954.80	0.07	0.05	N	O	1998-02-18	1998-03-27	1998-03-16	COLLECT COD              	TRUCK     	slyly by the packages. carefull
518	84	5	2	23	22633.84	0.05	0.07	N	O	1998-02-20	1998-05-05	1998-03-11	COLLECT COD              	TRUCK     	 special requests. fluffily ironic re
518	134	5	3	12	12409.56	0.01	0.06	N	O	1998-03-08	1998-03-31	1998-04-06	NONE                     	AIR       	 packages thrash slyly
518	122	3	4	46	47017.52	0.07	0.02	N	O	1998-04-07	1998-04-17	1998-04-29	NONE                     	MAIL      	. blithely even ideas cajole furiously. b
518	71	2	5	16	15537.12	0.01	0.01	N	O	1998-03-15	1998-03-24	1998-04-08	NONE                     	MAIL      	use quickly expre
518	197	10	6	39	42790.41	0.09	0.08	N	O	1998-02-26	1998-03-17	1998-03-21	DELIVER IN PERSON        	FOB       	 the bold, special deposits are carefully 
518	186	7	7	48	52136.64	0.03	0.07	N	O	1998-03-06	1998-04-22	1998-03-14	NONE                     	FOB       	 slyly final platelets; quickly even deposi
546	85	6	1	16	15761.28	0.08	0.02	N	O	1997-02-04	1996-12-30	1997-02-25	DELIVER IN PERSON        	TRUCK     	de of the orbits. sometimes regula
547	71	10	1	44	42727.08	0.08	0.08	N	O	1996-10-18	1996-08-17	1996-10-27	TAKE BACK RETURN         	FOB       	thely express dependencies. qu
547	137	8	2	48	49782.24	0.01	0.04	N	O	1996-10-21	1996-08-04	1996-11-20	COLLECT COD              	SHIP      	thely specia
547	182	3	3	3	3246.54	0.05	0.02	N	O	1996-09-04	1996-08-01	1996-09-21	COLLECT COD              	SHIP      	pinto beans. ironi
548	197	8	1	2	2194.38	0.06	0.05	A	F	1994-11-26	1994-11-06	1994-12-06	COLLECT COD              	MAIL      	ests haggle quickly eve
548	5	6	2	6	5430.00	0.00	0.08	A	F	1995-01-18	1994-12-08	1995-02-10	NONE                     	TRUCK     	sits wake furiously regular
548	1	8	3	21	18921.00	0.03	0.08	A	F	1995-01-13	1994-12-18	1995-01-25	NONE                     	AIR       	ideas. special accounts above the furiou
548	57	9	4	21	20098.05	0.08	0.03	A	F	1994-10-27	1994-12-04	1994-11-21	DELIVER IN PERSON        	AIR       	 engage quickly. regular theo
548	93	7	5	19	18868.71	0.00	0.02	A	F	1994-09-24	1994-11-24	1994-10-01	DELIVER IN PERSON        	MAIL      	courts boost care
548	153	8	6	32	33700.80	0.06	0.04	A	F	1994-12-16	1994-11-20	1994-12-29	NONE                     	REG AIR   	c instruction
576	87	8	1	2	1974.16	0.07	0.01	N	O	1997-05-15	1997-06-30	1997-05-28	NONE                     	RAIL      	ccounts along the ac
576	34	5	2	6	5604.18	0.06	0.05	N	O	1997-05-15	1997-07-26	1997-06-03	DELIVER IN PERSON        	TRUCK     	al deposits. slyly even sauternes a
576	37	3	3	6	5622.18	0.08	0.07	N	O	1997-08-28	1997-06-16	1997-09-25	DELIVER IN PERSON        	FOB       	ts. ironic multipliers 
576	138	9	4	5	5190.65	0.03	0.07	N	O	1997-06-11	1997-06-17	1997-07-05	NONE                     	REG AIR   	l foxes boost slyly. accounts af
580	85	6	1	33	32507.64	0.03	0.05	N	O	1997-10-11	1997-09-19	1997-10-16	TAKE BACK RETURN         	FOB       	y express theodolites cajole carefully 
580	174	5	2	31	33299.27	0.04	0.08	N	O	1997-10-04	1997-09-08	1997-10-15	COLLECT COD              	FOB       	ose alongside of the sl
580	185	6	3	19	20618.42	0.04	0.04	N	O	1997-07-23	1997-09-21	1997-08-15	NONE                     	FOB       	mong the special packag
581	64	1	1	41	39526.46	0.09	0.07	N	O	1997-05-26	1997-04-06	1997-06-10	TAKE BACK RETURN         	MAIL      	nts. quickly
581	93	5	2	14	13903.26	0.06	0.08	N	O	1997-05-17	1997-04-14	1997-06-08	NONE                     	MAIL      	. deposits s
581	101	6	3	49	49053.90	0.10	0.02	N	O	1997-02-27	1997-04-24	1997-03-10	TAKE BACK RETURN         	MAIL      	. slyly regular pinto beans acr
581	75	4	4	30	29252.10	0.10	0.08	N	O	1997-06-19	1997-05-21	1997-06-22	TAKE BACK RETURN         	TRUCK     	 regular ideas grow furio
608	154	6	1	19	20028.85	0.08	0.06	N	O	1996-04-19	1996-05-02	1996-05-03	DELIVER IN PERSON        	RAIL      	ideas. the
608	198	1	2	40	43927.60	0.03	0.01	N	O	1996-05-21	1996-04-11	1996-06-02	NONE                     	AIR       	 alongside of the regular tithes. sly
609	66	5	1	21	20287.26	0.01	0.05	R	F	1994-08-24	1994-08-23	1994-08-27	DELIVER IN PERSON        	FOB       	de of the special warthogs. excu
612	185	6	1	5	5425.90	0.07	0.00	R	F	1992-11-08	1992-11-20	1992-12-03	TAKE BACK RETURN         	RAIL      	structions. q
612	195	7	2	28	30665.32	0.07	0.06	R	F	1993-01-02	1992-12-11	1993-01-30	DELIVER IN PERSON        	TRUCK     	regular instructions affix bl
612	67	4	3	49	47385.94	0.00	0.08	A	F	1993-01-08	1992-11-25	1993-01-17	TAKE BACK RETURN         	REG AIR   	theodolite
612	39	5	4	28	26292.84	0.05	0.00	A	F	1992-11-12	1992-12-05	1992-12-02	TAKE BACK RETURN         	REG AIR   	lyly regular asym
612	88	9	5	1	988.08	0.08	0.04	R	F	1992-12-18	1992-12-13	1992-12-20	TAKE BACK RETURN         	FOB       	 requests.
612	189	10	6	33	35942.94	0.10	0.03	R	F	1992-11-30	1992-12-01	1992-12-12	COLLECT COD              	MAIL      	bove the blithely even ideas. careful
640	93	7	1	49	48661.41	0.09	0.02	R	F	1993-03-27	1993-04-17	1993-04-15	NONE                     	RAIL      	s haggle slyly
640	1	4	2	40	36040.00	0.09	0.05	A	F	1993-05-11	1993-04-11	1993-05-15	COLLECT COD              	TRUCK     	oach according to the bol
640	180	8	3	22	23763.96	0.05	0.07	A	F	1993-05-07	1993-04-14	1993-05-21	TAKE BACK RETURN         	TRUCK     	osits across the slyly regular theodo
640	32	3	4	45	41941.35	0.07	0.07	R	F	1993-04-15	1993-04-23	1993-04-21	DELIVER IN PERSON        	REG AIR   	ong the qui
641	126	9	1	18	18470.16	0.01	0.08	R	F	1993-10-17	1993-10-11	1993-10-29	DELIVER IN PERSON        	AIR       	p blithely bold packages. quick
641	100	2	2	1	1000.10	0.09	0.02	R	F	1993-12-03	1993-10-28	1993-12-26	TAKE BACK RETURN         	RAIL      	 nag across the regular foxes.
641	95	7	3	40	39803.60	0.05	0.06	R	F	1993-11-22	1993-10-20	1993-12-11	DELIVER IN PERSON        	REG AIR   	lets. furiously regular requests cajo
641	71	10	4	25	24276.75	0.03	0.02	A	F	1993-12-04	1993-11-18	1993-12-18	TAKE BACK RETURN         	FOB       	d, regular d
641	4	9	5	41	37064.00	0.07	0.04	R	F	1993-11-29	1993-10-27	1993-12-04	TAKE BACK RETURN         	FOB       	 asymptotes are quickly. bol
643	13	3	1	28	25564.28	0.00	0.08	A	F	1995-04-13	1995-05-12	1995-04-14	TAKE BACK RETURN         	TRUCK     	ly regular requests nag sly
643	51	2	2	48	45650.40	0.01	0.02	N	O	1995-07-10	1995-06-07	1995-08-01	NONE                     	FOB       	ly ironic accounts
643	163	2	3	23	24452.68	0.05	0.03	N	O	1995-07-09	1995-05-18	1995-07-31	COLLECT COD              	RAIL      	sits are carefully according to the e
643	45	4	4	39	36856.56	0.08	0.04	A	F	1995-06-08	1995-06-16	1995-06-13	COLLECT COD              	RAIL      	 the pains. carefully s
643	190	1	5	47	51238.93	0.10	0.03	R	F	1995-04-05	1995-06-14	1995-04-26	DELIVER IN PERSON        	RAIL      	y against 
644	134	10	1	46	47569.98	0.02	0.01	A	F	1992-05-20	1992-06-14	1992-06-14	DELIVER IN PERSON        	RAIL      	 special requests was sometimes expre
644	130	3	2	11	11331.43	0.05	0.02	A	F	1992-08-20	1992-07-21	1992-09-11	TAKE BACK RETURN         	TRUCK     	ealthy pinto beans use carefu
644	101	6	3	44	44048.40	0.04	0.04	R	F	1992-08-17	1992-07-26	1992-08-20	COLLECT COD              	REG AIR   	iously ironic pinto beans. bold packa
644	80	8	4	7	6860.56	0.01	0.02	A	F	1992-05-18	1992-07-01	1992-06-07	COLLECT COD              	RAIL      	 regular requests are blithely. slyly
644	50	1	5	23	21851.15	0.02	0.04	R	F	1992-07-31	1992-07-28	1992-08-13	DELIVER IN PERSON        	TRUCK     	uctions nag quickly alongside of t
644	85	6	6	33	32507.64	0.00	0.07	R	F	1992-08-26	1992-07-27	1992-08-28	NONE                     	AIR       	ages sleep. bold, bo
644	51	9	7	38	36139.90	0.08	0.06	R	F	1992-05-17	1992-07-10	1992-06-06	TAKE BACK RETURN         	MAIL      	 packages. blithely slow accounts nag quic
647	17	1	1	41	37597.41	0.08	0.08	N	O	1997-11-19	1997-09-24	1997-12-15	COLLECT COD              	REG AIR   	r instructions. quickly unusu
647	113	10	2	5	5065.55	0.10	0.00	N	O	1997-09-25	1997-09-22	1997-10-25	TAKE BACK RETURN         	AIR       	ly express packages haggle caref
647	153	8	3	15	15797.25	0.08	0.00	N	O	1997-09-23	1997-10-09	1997-10-21	NONE                     	MAIL      	ve the even, bold foxes sleep 
672	173	4	1	41	43999.97	0.06	0.06	R	F	1994-06-20	1994-07-03	1994-06-22	COLLECT COD              	REG AIR   	 dependencies in
672	190	1	2	9	9811.71	0.03	0.04	R	F	1994-06-25	1994-06-06	1994-07-19	TAKE BACK RETURN         	TRUCK     	haggle carefully carefully reg
672	143	2	3	35	36509.90	0.02	0.01	R	F	1994-07-13	1994-06-04	1994-07-14	COLLECT COD              	RAIL      	 dependencies haggle quickly. theo
674	102	5	1	23	23048.30	0.06	0.07	A	F	1992-10-25	1992-10-15	1992-11-03	COLLECT COD              	SHIP      	ve the quickly even deposits. blithe
674	59	4	2	4	3836.20	0.02	0.07	R	F	1992-10-05	1992-11-22	1992-10-22	NONE                     	RAIL      	ly express pinto beans sleep car
677	59	7	1	32	30689.60	0.04	0.08	R	F	1994-01-06	1994-01-31	1994-02-02	NONE                     	RAIL      	slyly final
677	168	9	2	39	41658.24	0.00	0.07	R	F	1993-12-19	1994-02-11	1994-01-05	TAKE BACK RETURN         	SHIP      	ges. furiously regular packages use 
677	24	5	3	46	42504.92	0.01	0.02	R	F	1993-12-02	1994-02-12	1993-12-06	COLLECT COD              	RAIL      	ng theodolites. furiously unusual theodo
677	148	7	4	1	1048.14	0.06	0.05	R	F	1993-12-01	1994-01-14	1993-12-26	DELIVER IN PERSON        	MAIL      	ly. regular 
677	150	9	5	25	26253.75	0.00	0.05	A	F	1994-03-12	1994-02-02	1994-03-28	DELIVER IN PERSON        	AIR       	 packages integrate blithely
705	189	10	1	46	50102.28	0.05	0.06	N	O	1997-04-18	1997-05-06	1997-05-05	DELIVER IN PERSON        	SHIP      	ss deposits. ironic packa
705	117	7	2	35	35598.85	0.10	0.04	N	O	1997-03-25	1997-03-20	1997-04-23	TAKE BACK RETURN         	FOB       	carefully ironic accounts
707	155	6	1	34	35875.10	0.01	0.02	R	F	1994-12-08	1995-01-15	1995-01-02	NONE                     	RAIL      	 dependencies
707	43	10	2	22	20746.88	0.00	0.06	A	F	1995-01-12	1994-12-28	1995-01-16	DELIVER IN PERSON        	REG AIR   	 kindle ironically
708	124	7	1	3	3072.36	0.05	0.02	N	O	1998-10-09	1998-09-22	1998-11-07	COLLECT COD              	FOB       	e slyly pending foxes. 
708	180	1	2	19	20523.42	0.06	0.00	N	O	1998-10-28	1998-09-23	1998-11-25	COLLECT COD              	SHIP      	 requests. even, thin ideas
708	122	5	3	33	33729.96	0.09	0.06	N	O	1998-09-10	1998-09-20	1998-09-22	COLLECT COD              	RAIL      	s boost carefully ruthless theodolites. f
708	56	7	4	5	4780.25	0.07	0.07	N	O	1998-07-22	1998-08-15	1998-07-28	TAKE BACK RETURN         	REG AIR   	c pinto beans nag after the account
708	143	2	5	36	37553.04	0.08	0.01	N	O	1998-07-16	1998-09-04	1998-08-11	NONE                     	SHIP      	ests. even, regular hockey p
708	23	6	6	7	6461.14	0.10	0.03	N	O	1998-08-16	1998-08-15	1998-09-10	COLLECT COD              	REG AIR   	lly express ac
710	163	8	1	47	49968.52	0.06	0.08	A	F	1993-01-18	1993-03-24	1993-01-24	TAKE BACK RETURN         	MAIL      	usual ideas into th
710	193	4	2	38	41541.22	0.07	0.02	R	F	1993-04-18	1993-03-12	1993-05-15	COLLECT COD              	FOB       	sts boost fluffily aft
710	139	5	3	7	7273.91	0.04	0.06	R	F	1993-01-20	1993-03-28	1993-02-15	TAKE BACK RETURN         	REG AIR   	xpress, special ideas. bl
710	90	1	4	25	24752.25	0.00	0.05	R	F	1993-03-31	1993-02-05	1993-04-22	COLLECT COD              	FOB       	eas detect do
710	186	7	5	12	13034.16	0.01	0.02	A	F	1993-02-18	1993-02-27	1993-03-07	DELIVER IN PERSON        	MAIL      	ions. slyly express theodolites al
710	114	4	6	21	21296.31	0.04	0.06	R	F	1993-03-22	1993-03-05	1993-03-27	DELIVER IN PERSON        	SHIP      	es. furiously p
710	160	2	7	46	48767.36	0.03	0.07	R	F	1993-04-16	1993-03-27	1993-05-05	COLLECT COD              	MAIL      	ges use; blithely pending excuses inte
737	182	3	1	12	12986.16	0.01	0.01	R	F	1992-04-28	1992-06-30	1992-05-08	COLLECT COD              	RAIL      	posits after the slyly bold du
739	85	6	1	28	27582.24	0.00	0.03	N	O	1998-06-03	1998-08-04	1998-06-29	TAKE BACK RETURN         	RAIL      	elets about the pe
739	4	7	2	50	45200.00	0.07	0.06	N	O	1998-08-26	1998-07-16	1998-09-02	COLLECT COD              	MAIL      	ndencies. blith
739	49	2	3	12	11388.48	0.05	0.00	N	O	1998-08-20	1998-07-24	1998-08-22	NONE                     	MAIL      	le slyly along the close i
739	44	3	4	47	44369.88	0.09	0.07	N	O	1998-08-12	1998-07-09	1998-08-28	NONE                     	REG AIR   	deas according to the theodolites sn
739	188	9	5	30	32645.40	0.07	0.06	N	O	1998-06-19	1998-08-26	1998-07-02	DELIVER IN PERSON        	REG AIR   	above the even deposits. ironic requests
740	2	9	1	22	19844.00	0.10	0.02	N	O	1995-07-24	1995-09-11	1995-08-11	TAKE BACK RETURN         	FOB       	odolites cajole ironic, pending instruc
740	66	1	2	35	33812.10	0.00	0.00	N	O	1995-09-06	1995-08-22	1995-10-02	NONE                     	TRUCK     	p quickly. fu
740	199	10	3	29	31876.51	0.06	0.05	N	O	1995-10-26	1995-09-17	1995-10-29	DELIVER IN PERSON        	FOB       	ntly bold pinto beans sleep quickl
743	192	5	1	21	22935.99	0.01	0.04	N	O	1996-10-26	1996-11-05	1996-11-11	COLLECT COD              	MAIL      	d requests. packages afte
770	181	2	1	39	42166.02	0.09	0.06	N	O	1998-07-19	1998-08-09	1998-08-04	NONE                     	REG AIR   	osits. foxes cajole 
770	54	2	2	25	23851.25	0.03	0.02	N	O	1998-05-26	1998-07-23	1998-06-04	TAKE BACK RETURN         	AIR       	 deposits dazzle fluffily alongside of 
772	53	5	1	35	33356.75	0.10	0.06	R	F	1993-07-05	1993-06-05	1993-08-02	NONE                     	SHIP      	kly thin packages wake slowly
772	84	5	2	10	9840.80	0.05	0.01	R	F	1993-05-20	1993-05-19	1993-06-15	DELIVER IN PERSON        	MAIL      	 deposits cajole carefully instructions. t
772	86	7	3	35	34512.80	0.03	0.04	R	F	1993-04-18	1993-06-13	1993-05-01	COLLECT COD              	MAIL      	ng ideas. special packages haggle alon
772	180	8	4	10	10801.80	0.08	0.02	A	F	1993-05-17	1993-06-09	1993-05-29	COLLECT COD              	AIR       	o the furiously final deposits. furi
772	54	5	5	42	40070.10	0.02	0.07	A	F	1993-06-09	1993-07-16	1993-06-12	DELIVER IN PERSON        	MAIL      	 express foxes abo
773	100	1	1	5	5000.50	0.06	0.04	A	F	1993-11-21	1993-12-19	1993-12-21	COLLECT COD              	MAIL      	ar requests. regular, thin packages u
773	11	5	2	31	28241.31	0.02	0.06	A	F	1993-12-30	1993-11-02	1994-01-01	TAKE BACK RETURN         	MAIL      	e slyly unusual deposit
773	151	3	3	39	40994.85	0.06	0.05	A	F	1994-01-04	1993-12-23	1994-01-26	DELIVER IN PERSON        	FOB       	quickly eve
773	29	8	4	28	26012.56	0.10	0.06	R	F	1994-01-19	1993-11-05	1994-01-23	NONE                     	TRUCK     	he furiously slow deposits.
773	134	5	5	9	9307.17	0.09	0.02	R	F	1993-10-09	1993-12-25	1993-11-04	TAKE BACK RETURN         	FOB       	ent orbits haggle fluffily after the 
773	40	1	6	43	40421.72	0.07	0.03	A	F	1993-11-06	1993-11-20	1993-11-08	TAKE BACK RETURN         	SHIP      	furiously bold dependencies. blithel
801	6	3	1	13	11778.00	0.10	0.02	R	F	1992-04-25	1992-04-24	1992-05-16	COLLECT COD              	RAIL      	s are fluffily stealthily expres
801	95	8	2	21	20896.89	0.05	0.02	A	F	1992-03-14	1992-04-01	1992-04-05	COLLECT COD              	AIR       	wake silently furiously idle deposits. 
801	3	4	3	21	18963.00	0.05	0.03	A	F	1992-04-25	1992-03-20	1992-05-04	COLLECT COD              	REG AIR   	cial, special packages.
801	164	9	4	12	12769.92	0.08	0.04	A	F	1992-06-06	1992-04-14	1992-06-12	TAKE BACK RETURN         	RAIL      	s. ironic pinto b
801	74	2	5	45	43833.15	0.01	0.06	R	F	1992-03-22	1992-03-22	1992-03-25	COLLECT COD              	REG AIR   	 even asymptotes
801	122	7	6	10	10221.20	0.08	0.01	A	F	1992-06-05	1992-05-15	1992-06-21	DELIVER IN PERSON        	MAIL      	al accounts. carefully regular foxes wake
801	26	5	7	11	10186.22	0.01	0.03	A	F	1992-05-09	1992-04-19	1992-05-15	DELIVER IN PERSON        	REG AIR   	y special pinto beans cajole 
802	143	6	1	40	41725.60	0.08	0.08	A	F	1995-01-07	1995-04-03	1995-01-23	DELIVER IN PERSON        	RAIL      	y bold accou
802	133	4	2	34	35126.42	0.08	0.06	A	F	1995-03-01	1995-03-15	1995-03-12	COLLECT COD              	AIR       	instructions cajole carefully. quietl
802	131	2	3	44	45369.72	0.07	0.04	R	F	1995-01-09	1995-02-04	1995-01-18	TAKE BACK RETURN         	SHIP      	rmanently idly special requ
802	157	2	4	18	19028.70	0.09	0.02	R	F	1995-03-06	1995-02-07	1995-03-19	TAKE BACK RETURN         	RAIL      	y regular requests engage furiously final d
802	132	3	5	19	19610.47	0.08	0.06	A	F	1995-04-01	1995-02-20	1995-04-23	DELIVER IN PERSON        	REG AIR   	old, furious
805	198	10	1	25	27454.75	0.07	0.06	N	O	1995-08-05	1995-09-30	1995-08-06	NONE                     	AIR       	ide of the pending, sly requests. quickly f
805	57	5	2	29	27754.45	0.07	0.01	N	O	1995-08-24	1995-08-15	1995-09-16	TAKE BACK RETURN         	AIR       	dolites according to the slyly f
805	47	8	3	12	11364.48	0.01	0.06	N	O	1995-07-13	1995-09-27	1995-08-02	TAKE BACK RETURN         	REG AIR   	 regular foxes. furio
805	76	6	4	26	25377.82	0.08	0.07	N	O	1995-08-28	1995-09-24	1995-09-11	TAKE BACK RETURN         	RAIL      	. ironic deposits sleep across 
807	117	7	1	49	49838.39	0.00	0.00	R	F	1993-12-05	1994-01-13	1993-12-25	COLLECT COD              	REG AIR   	 furiously according to the un
807	155	10	2	49	51702.35	0.01	0.06	A	F	1994-01-17	1994-01-24	1994-01-22	COLLECT COD              	TRUCK     	y regular requests haggle.
807	181	2	3	48	51896.64	0.07	0.07	A	F	1994-01-08	1994-02-02	1994-01-15	DELIVER IN PERSON        	SHIP      	kly across the f
807	80	1	4	10	9800.80	0.09	0.00	R	F	1994-01-19	1994-02-12	1994-01-28	NONE                     	TRUCK     	furiously final depths sleep a
807	143	6	5	30	31294.20	0.02	0.01	R	F	1994-01-19	1994-01-09	1994-01-27	NONE                     	RAIL      	cial accoun
807	12	2	6	11	10032.11	0.02	0.04	R	F	1994-03-25	1994-01-26	1994-04-14	NONE                     	FOB       	unts above the slyly final ex
807	1	6	7	19	17119.00	0.08	0.05	A	F	1994-02-10	1994-02-20	1994-03-06	NONE                     	SHIP      	ns haggle quickly across the furi
834	145	2	1	36	37625.04	0.06	0.04	R	F	1994-06-28	1994-07-25	1994-07-07	TAKE BACK RETURN         	SHIP      	ccounts haggle after the furiously 
834	7	2	2	11	9977.00	0.03	0.00	A	F	1994-09-18	1994-08-03	1994-10-02	DELIVER IN PERSON        	TRUCK     	inst the regular packa
835	107	2	1	33	33234.30	0.09	0.06	N	O	1995-11-01	1995-12-02	1995-11-24	DELIVER IN PERSON        	RAIL      	onic instructions among the carefully iro
835	185	6	2	28	30385.04	0.02	0.02	N	O	1995-12-27	1995-12-11	1996-01-21	NONE                     	SHIP      	 fluffily furious pinto beans
836	188	9	1	6	6529.08	0.09	0.03	N	O	1996-12-09	1997-01-31	1996-12-29	COLLECT COD              	TRUCK     	fully bold theodolites are daringly across
836	84	5	2	18	17713.44	0.03	0.05	N	O	1997-02-27	1997-02-11	1997-03-22	NONE                     	REG AIR   	y pending packages use alon
836	141	8	3	46	47892.44	0.05	0.07	N	O	1997-03-21	1997-02-06	1997-04-05	NONE                     	REG AIR   	boldly final pinto beans haggle furiously
837	57	5	1	39	37324.95	0.03	0.08	A	F	1994-07-22	1994-08-10	1994-08-11	NONE                     	RAIL      	ecial pinto bea
837	88	9	2	24	23713.92	0.08	0.00	R	F	1994-06-27	1994-09-02	1994-07-27	DELIVER IN PERSON        	FOB       	p carefully. theodolites use. bold courts a
867	139	10	1	7	7273.91	0.04	0.07	A	F	1994-02-19	1993-12-25	1994-02-25	DELIVER IN PERSON        	TRUCK     	pendencies-- slyly unusual packages hagg
4514	81	2	4	9	8829.72	0.10	0.03	A	F	1994-08-04	1994-07-01	1994-09-01	DELIVER IN PERSON        	REG AIR   	wly. quick
870	50	9	1	36	34201.80	0.04	0.07	A	F	1993-10-18	1993-09-16	1993-11-15	COLLECT COD              	MAIL      	fily. furiously final accounts are 
870	186	7	2	5	5430.90	0.06	0.05	A	F	1993-08-13	1993-09-11	1993-08-24	COLLECT COD              	FOB       	e slyly excuses. ironi
896	39	10	1	47	44134.41	0.07	0.08	R	F	1993-05-28	1993-05-15	1993-06-15	DELIVER IN PERSON        	TRUCK     	ly even pinto beans integrate. b
896	198	2	2	10	10981.90	0.03	0.07	A	F	1993-07-07	1993-06-03	1993-07-24	COLLECT COD              	SHIP      	 quickly even theodolites. carefully regu
896	2	9	3	7	6314.00	0.09	0.02	A	F	1993-05-02	1993-05-24	1993-05-31	DELIVER IN PERSON        	MAIL      	 requests 
896	152	3	4	11	11573.65	0.08	0.04	A	F	1993-05-19	1993-05-22	1993-06-08	COLLECT COD              	MAIL      	the multipliers sleep
896	188	9	5	34	36998.12	0.00	0.05	R	F	1993-05-21	1993-06-01	1993-05-23	NONE                     	TRUCK     	ular, close requests cajo
896	177	6	6	44	47395.48	0.09	0.08	R	F	1993-05-19	1993-04-14	1993-06-02	DELIVER IN PERSON        	FOB       	lar, pending packages. deposits are q
896	109	2	7	11	11100.10	0.01	0.07	A	F	1993-05-01	1993-04-09	1993-05-06	TAKE BACK RETURN         	FOB       	rding to the pinto beans wa
899	61	10	1	18	17299.08	0.04	0.05	N	O	1998-08-06	1998-05-09	1998-09-05	DELIVER IN PERSON        	AIR       	re daring, pending deposits. blit
899	47	4	2	25	23676.00	0.00	0.07	N	O	1998-07-21	1998-05-12	1998-08-16	NONE                     	REG AIR   	rly final sentiments. bold pinto beans 
899	85	6	3	4	3940.32	0.09	0.05	N	O	1998-06-02	1998-06-28	1998-06-14	TAKE BACK RETURN         	REG AIR   	ter the carefully regular deposits are agai
899	180	9	4	14	15122.52	0.05	0.03	N	O	1998-05-21	1998-05-28	1998-06-03	TAKE BACK RETURN         	FOB       	ades impress carefully
899	71	10	5	4	3884.28	0.06	0.02	N	O	1998-04-11	1998-05-14	1998-04-27	NONE                     	TRUCK     	ges. blithe, ironic waters cajole care
899	120	4	6	47	47945.64	0.00	0.04	N	O	1998-04-14	1998-05-30	1998-05-13	DELIVER IN PERSON        	TRUCK     	furiously final foxes after the s
899	14	1	7	11	10054.11	0.02	0.08	N	O	1998-06-03	1998-06-15	1998-06-20	COLLECT COD              	REG AIR   	t the ironic
903	65	10	1	27	26056.62	0.04	0.03	N	O	1995-09-18	1995-09-20	1995-10-02	TAKE BACK RETURN         	SHIP      	lly pending foxes. furiously
903	9	2	2	35	31815.00	0.06	0.05	N	O	1995-09-18	1995-08-21	1995-10-12	TAKE BACK RETURN         	TRUCK     	rets wake fin
903	9	2	3	33	29997.00	0.02	0.03	N	O	1995-09-24	1995-09-01	1995-10-12	COLLECT COD              	MAIL      	ely ironic packages wake blithely
903	56	1	4	9	8604.45	0.09	0.00	N	O	1995-10-06	1995-09-14	1995-10-24	NONE                     	TRUCK     	he slyly ev
903	42	3	5	1	942.04	0.04	0.00	N	O	1995-10-22	1995-09-13	1995-11-03	NONE                     	AIR       	y final platelets sublate among the 
903	168	9	6	13	13886.08	0.07	0.02	N	O	1995-09-11	1995-10-04	1995-10-03	COLLECT COD              	SHIP      	sleep along the final
929	129	8	1	45	46310.40	0.09	0.01	R	F	1993-01-24	1992-12-06	1993-02-16	DELIVER IN PERSON        	REG AIR   	ges haggle careful
929	175	5	2	44	47307.48	0.02	0.00	A	F	1992-10-09	1992-11-20	1992-10-22	DELIVER IN PERSON        	SHIP      	s. excuses cajole. carefully regu
929	74	5	3	14	13636.98	0.06	0.07	A	F	1992-10-21	1992-11-17	1992-11-15	NONE                     	FOB       	gainst the
929	102	5	4	7	7014.70	0.06	0.01	A	F	1992-12-24	1992-12-19	1993-01-08	TAKE BACK RETURN         	TRUCK     	ithely. slyly c
933	49	8	1	23	21827.92	0.02	0.04	R	F	1992-08-13	1992-09-18	1992-08-25	DELIVER IN PERSON        	MAIL      	 the furiously bold dinos. sly
933	13	4	2	27	24651.27	0.02	0.01	R	F	1992-10-03	1992-10-02	1992-10-26	DELIVER IN PERSON        	RAIL      	ests. express
933	100	2	3	26	26002.60	0.05	0.00	A	F	1992-11-09	1992-11-03	1992-11-16	DELIVER IN PERSON        	AIR       	 the deposits affix slyly after t
960	107	10	1	1	1007.10	0.07	0.00	A	F	1994-12-24	1994-10-26	1995-01-20	DELIVER IN PERSON        	AIR       	y ironic packages. quickly even 
960	117	7	2	25	25427.75	0.06	0.08	R	F	1994-12-01	1994-10-29	1994-12-27	DELIVER IN PERSON        	RAIL      	ts. fluffily regular requests 
960	175	3	3	32	34405.44	0.01	0.08	R	F	1995-01-19	1994-12-17	1995-02-04	DELIVER IN PERSON        	FOB       	around the blithe, even pl
963	194	8	1	7	7659.33	0.01	0.00	R	F	1994-09-12	1994-07-18	1994-09-17	DELIVER IN PERSON        	REG AIR   	s. slyly regular depe
963	98	10	2	48	47908.32	0.10	0.06	R	F	1994-08-25	1994-08-12	1994-09-21	DELIVER IN PERSON        	RAIL      	ages. quickly express deposits cajole pe
965	108	1	1	20	20162.00	0.04	0.05	N	F	1995-06-16	1995-07-20	1995-07-06	COLLECT COD              	MAIL      	kly. carefully pending requ
965	18	5	2	23	21114.23	0.06	0.08	N	O	1995-07-12	1995-07-08	1995-08-11	COLLECT COD              	MAIL      	ld kindle carefully across th
994	65	6	1	4	3860.24	0.07	0.03	R	F	1994-07-05	1994-05-21	1994-07-20	COLLECT COD              	SHIP      	aggle carefully acc
994	10	3	2	11	10010.11	0.01	0.00	R	F	1994-05-03	1994-06-10	1994-05-22	NONE                     	AIR       	ular accounts sleep 
994	31	7	3	5	4655.15	0.08	0.08	A	F	1994-06-24	1994-06-14	1994-06-26	NONE                     	MAIL      	ainst the pending requests. packages sl
994	131	7	4	25	25778.25	0.10	0.00	A	F	1994-06-03	1994-06-02	1994-06-06	COLLECT COD              	RAIL      	usual pinto beans.
995	173	1	1	15	16097.55	0.08	0.05	N	O	1995-06-30	1995-08-04	1995-07-27	NONE                     	REG AIR   	uses. fluffily fina
995	129	4	2	28	28815.36	0.08	0.03	N	F	1995-06-12	1995-07-20	1995-06-19	DELIVER IN PERSON        	SHIP      	pades. quick, final frays use flu
995	166	3	3	45	47977.20	0.00	0.05	N	O	1995-08-02	1995-07-21	1995-08-03	DELIVER IN PERSON        	SHIP      	lar packages detect blithely above t
995	66	3	4	25	24151.50	0.01	0.08	N	O	1995-09-08	1995-08-05	1995-09-28	NONE                     	TRUCK     	lyly even 
995	24	5	5	18	16632.36	0.06	0.03	N	O	1995-07-03	1995-07-29	1995-07-22	TAKE BACK RETURN         	AIR       	 even accounts unwind c
997	163	4	1	11	11694.76	0.00	0.02	N	O	1997-06-16	1997-07-21	1997-07-14	DELIVER IN PERSON        	TRUCK     	p furiously according to t
997	48	9	2	17	16116.68	0.03	0.00	N	O	1997-07-28	1997-07-26	1997-08-20	DELIVER IN PERSON        	SHIP      	aggle quickly furiously
998	10	7	1	22	20020.22	0.04	0.05	A	F	1994-12-03	1995-02-17	1994-12-19	TAKE BACK RETURN         	RAIL      	lites. qui
998	181	2	2	7	7568.26	0.10	0.05	R	F	1995-03-24	1995-01-18	1995-04-03	NONE                     	MAIL      	nic deposits. even asym
998	142	9	3	30	31264.20	0.05	0.07	A	F	1994-12-02	1995-01-23	1994-12-23	NONE                     	SHIP      	lyly idle Tir
998	11	8	4	6	5466.06	0.09	0.05	R	F	1995-03-20	1994-12-27	1995-04-13	DELIVER IN PERSON        	MAIL      	refully accounts. carefully express ac
998	73	2	5	1	973.07	0.04	0.00	R	F	1995-01-05	1995-01-06	1995-01-13	NONE                     	SHIP      	es sleep. regular dependencies use bl
999	61	6	1	34	32676.04	0.00	0.08	R	F	1993-10-30	1993-10-17	1993-10-31	TAKE BACK RETURN         	SHIP      	its. daringly final instruc
999	199	1	2	41	45066.79	0.08	0.01	A	F	1993-10-16	1993-12-04	1993-11-03	DELIVER IN PERSON        	REG AIR   	us depths. carefully ironic instruc
999	118	5	3	15	15271.65	0.07	0.06	A	F	1993-12-12	1993-10-18	1994-01-08	COLLECT COD              	REG AIR   	y ironic requests. carefully regu
999	3	4	4	10	9030.00	0.05	0.02	A	F	1993-11-23	1993-12-02	1993-11-29	NONE                     	MAIL      	efully pending
999	19	10	5	3	2757.03	0.03	0.00	R	F	1993-09-17	1993-10-22	1993-10-13	NONE                     	FOB       	nic, pending ideas. bl
999	181	2	6	37	40003.66	0.00	0.04	R	F	1994-01-03	1993-10-28	1994-01-12	DELIVER IN PERSON        	TRUCK     	ckly slyly unusual packages: packages hagg
1028	128	3	1	2	2056.24	0.09	0.03	A	F	1994-01-10	1994-03-22	1994-01-26	COLLECT COD              	FOB       	s alongside of the regular asymptotes sleep
1028	112	9	2	39	39472.29	0.06	0.05	R	F	1994-02-18	1994-03-22	1994-03-06	TAKE BACK RETURN         	MAIL      	 final dependencies affix a
1028	100	3	3	8	8000.80	0.03	0.07	A	F	1994-02-14	1994-03-28	1994-02-22	NONE                     	AIR       	e carefully final packages. furiously fi
1028	32	8	4	26	24232.78	0.07	0.02	A	F	1994-03-18	1994-02-08	1994-03-19	TAKE BACK RETURN         	RAIL      	ronic platelets. carefully f
1028	29	2	5	27	25083.54	0.00	0.04	A	F	1994-04-03	1994-02-07	1994-04-26	NONE                     	REG AIR   	ial accounts nag. slyly
1028	26	1	6	39	36114.78	0.03	0.02	A	F	1994-02-27	1994-02-16	1994-03-02	DELIVER IN PERSON        	AIR       	c theodoli
1028	31	2	7	22	20482.66	0.03	0.00	R	F	1994-04-24	1994-02-27	1994-05-08	NONE                     	REG AIR   	 Tiresias alongside of the carefully spec
1058	140	6	1	24	24963.36	0.08	0.04	A	F	1993-07-09	1993-05-28	1993-07-22	DELIVER IN PERSON        	TRUCK     	fully ironic accounts. express accou
1058	89	10	2	5	4945.40	0.04	0.07	R	F	1993-05-11	1993-05-29	1993-05-27	COLLECT COD              	TRUCK     	refully even requests boost along
1058	90	1	3	44	43563.96	0.10	0.01	R	F	1993-06-26	1993-06-21	1993-07-20	COLLECT COD              	TRUCK     	uriously f
1058	5	2	4	25	22625.00	0.09	0.01	A	F	1993-05-27	1993-06-10	1993-06-20	TAKE BACK RETURN         	MAIL      	 the final requests believe carefully 
1059	178	9	1	16	17250.72	0.07	0.02	A	F	1994-04-24	1994-03-31	1994-04-28	DELIVER IN PERSON        	SHIP      	y ironic pinto 
1059	29	2	2	7	6503.14	0.07	0.06	R	F	1994-03-30	1994-04-01	1994-04-24	DELIVER IN PERSON        	MAIL      	the furiously silent excuses are e
1059	88	9	3	45	44463.60	0.00	0.02	R	F	1994-06-10	1994-05-08	1994-06-21	COLLECT COD              	RAIL      	riously even theodolites. slyly regula
1059	110	7	4	26	26262.86	0.09	0.01	A	F	1994-03-17	1994-04-18	1994-03-26	DELIVER IN PERSON        	TRUCK     	ar pinto beans at the furiously 
1059	139	5	5	37	38447.81	0.09	0.04	R	F	1994-03-31	1994-05-08	1994-04-06	COLLECT COD              	RAIL      	 packages lose in place of the slyly unusu
1059	190	1	6	50	54509.50	0.00	0.03	A	F	1994-06-15	1994-05-11	1994-06-29	NONE                     	MAIL      	s impress furiously about
1059	123	4	7	13	13300.56	0.01	0.03	R	F	1994-06-12	1994-05-11	1994-07-02	COLLECT COD              	TRUCK     	usly regular theodo
1090	22	3	1	5	4610.10	0.02	0.05	N	O	1998-02-19	1997-12-25	1998-02-24	DELIVER IN PERSON        	AIR       	s above the 
1090	113	10	2	28	28367.08	0.08	0.08	N	O	1998-02-20	1998-01-03	1998-03-19	NONE                     	FOB       	s cajole above the regular
1091	38	9	1	40	37521.20	0.10	0.06	N	O	1996-12-17	1996-10-14	1996-12-24	TAKE BACK RETURN         	REG AIR   	platelets. regular packag
1092	184	5	1	48	52040.64	0.04	0.04	N	O	1995-06-25	1995-04-06	1995-07-18	DELIVER IN PERSON        	AIR       	unusual accounts. fluffi
1092	153	5	2	1	1053.15	0.01	0.06	A	F	1995-03-10	1995-04-21	1995-04-06	COLLECT COD              	RAIL      	lent, pending requests-- requests nag accor
1092	161	8	3	28	29712.48	0.05	0.08	R	F	1995-04-08	1995-05-01	1995-05-02	DELIVER IN PERSON        	FOB       	affix carefully. u
1092	86	7	4	2	1972.16	0.05	0.07	R	F	1995-04-09	1995-05-12	1995-05-03	TAKE BACK RETURN         	TRUCK     	ans. slyly eve
1094	115	6	1	9	9135.99	0.07	0.06	N	O	1997-12-28	1998-03-16	1998-01-18	DELIVER IN PERSON        	AIR       	as. slyly pe
1095	137	3	1	33	34225.29	0.01	0.02	N	O	1995-10-03	1995-09-22	1995-10-13	NONE                     	MAIL      	slyly around the iron
1095	136	2	2	24	24867.12	0.04	0.06	N	O	1995-08-24	1995-10-20	1995-09-09	COLLECT COD              	TRUCK     	packages nod furiously above the carefully 
1095	156	4	3	13	13729.95	0.06	0.01	N	O	1995-08-24	1995-10-19	1995-09-02	TAKE BACK RETURN         	REG AIR   	ously even accounts. slyly bold a
1095	135	1	4	28	28983.64	0.08	0.03	N	O	1995-09-20	1995-11-18	1995-10-02	DELIVER IN PERSON        	SHIP      	 regular pac
1095	112	2	5	40	40484.40	0.09	0.03	N	O	1995-10-18	1995-11-14	1995-11-09	NONE                     	MAIL      	 bold accounts haggle slyly furiously even
1095	181	2	6	37	40003.66	0.07	0.08	N	O	1995-10-04	1995-11-13	1995-10-12	NONE                     	SHIP      	. quickly even dolphins sle
1122	92	6	1	8	7936.72	0.10	0.06	N	O	1997-02-02	1997-04-03	1997-02-22	TAKE BACK RETURN         	RAIL      	c foxes are along the slyly r
1122	182	3	2	29	31383.22	0.05	0.04	N	O	1997-05-07	1997-04-07	1997-05-15	COLLECT COD              	SHIP      	ptotes. quickl
1122	147	6	3	25	26178.50	0.09	0.01	N	O	1997-03-21	1997-03-03	1997-04-07	TAKE BACK RETURN         	RAIL      	d furiously. pinto 
1122	106	9	4	40	40244.00	0.08	0.08	N	O	1997-02-07	1997-03-25	1997-02-25	NONE                     	REG AIR   	packages sleep after the asym
1122	151	2	5	15	15767.25	0.05	0.03	N	O	1997-04-15	1997-03-15	1997-05-07	COLLECT COD              	SHIP      	olve blithely regular, 
1122	162	7	6	24	25491.84	0.04	0.01	N	O	1997-03-08	1997-02-20	1997-04-05	NONE                     	RAIL      	blithely requests. slyly pending r
1122	1	6	7	38	34238.00	0.00	0.08	N	O	1997-01-23	1997-04-02	1997-02-16	NONE                     	TRUCK     	t theodolites sleep. even, ironic
1126	36	2	1	44	41185.32	0.08	0.03	N	O	1998-05-07	1998-04-02	1998-05-29	NONE                     	TRUCK     	es. carefully special
1126	58	3	2	7	6706.35	0.06	0.01	N	O	1998-05-02	1998-03-22	1998-05-21	COLLECT COD              	MAIL      	ons. final, unusual
1126	147	10	3	14	14659.96	0.07	0.07	N	O	1998-04-17	1998-04-15	1998-05-12	DELIVER IN PERSON        	TRUCK     	nstructions. blithe
1127	43	10	1	35	33006.40	0.02	0.03	N	O	1995-11-25	1995-11-03	1995-12-17	NONE                     	TRUCK     	l instructions boost blithely according 
1127	110	5	2	38	38384.18	0.09	0.05	N	O	1995-11-07	1995-11-11	1995-11-26	DELIVER IN PERSON        	RAIL      	. never final packages boost acro
1127	20	1	3	29	26680.58	0.09	0.07	N	O	1995-09-20	1995-11-21	1995-10-11	DELIVER IN PERSON        	REG AIR   	y. blithely r
1127	175	6	4	7	7526.19	0.07	0.05	N	O	1995-11-05	1995-11-02	1995-11-11	DELIVER IN PERSON        	FOB       	 idly pending pains 
1153	86	7	1	15	14791.20	0.00	0.08	N	O	1996-04-24	1996-07-17	1996-04-29	TAKE BACK RETURN         	SHIP      	uctions boost fluffily according to
1153	169	8	2	50	53458.00	0.00	0.07	N	O	1996-06-27	1996-07-13	1996-07-05	COLLECT COD              	REG AIR   	ronic asymptotes nag slyly. 
1153	44	5	3	25	23601.00	0.00	0.05	N	O	1996-06-18	1996-06-28	1996-07-09	NONE                     	TRUCK     	 theodolites
1153	92	3	4	43	42659.87	0.01	0.00	N	O	1996-06-09	1996-06-01	1996-07-04	DELIVER IN PERSON        	MAIL      	special instructions are. unusual, final du
1153	142	5	5	45	46896.30	0.00	0.02	N	O	1996-06-18	1996-06-20	1996-07-03	TAKE BACK RETURN         	AIR       	oss the ex
1153	136	7	6	26	26939.38	0.02	0.03	N	O	1996-08-16	1996-07-12	1996-09-08	NONE                     	MAIL      	kages haggle carefully. f
1153	192	4	7	5	5460.95	0.02	0.03	N	O	1996-05-03	1996-06-12	1996-05-28	TAKE BACK RETURN         	FOB       	special excuses promi
1155	70	1	1	4	3880.28	0.09	0.05	N	O	1997-10-19	1997-12-09	1997-11-02	DELIVER IN PERSON        	SHIP      	ic foxes according to the carefully final 
1155	196	9	2	39	42751.41	0.08	0.05	N	O	1998-01-29	1998-01-03	1998-02-01	COLLECT COD              	TRUCK     	ckly final pinto beans was.
1155	147	4	3	23	24084.22	0.08	0.03	N	O	1997-11-24	1997-11-28	1997-12-06	DELIVER IN PERSON        	FOB       	ly unusual packages. iro
1155	140	1	4	12	12481.68	0.01	0.06	N	O	1997-11-01	1998-01-03	1997-11-19	DELIVER IN PERSON        	RAIL      	packages do
1155	5	2	5	49	44345.00	0.04	0.08	N	O	1997-12-07	1997-12-30	1997-12-08	NONE                     	AIR       	ccounts are alongside of t
1156	87	8	1	15	14806.20	0.07	0.06	N	O	1996-12-21	1997-01-03	1997-01-10	TAKE BACK RETURN         	AIR       	the furiously pen
1156	33	4	2	21	19593.63	0.02	0.08	N	O	1996-11-07	1997-01-14	1996-12-03	NONE                     	AIR       	dolphins. fluffily ironic packages sleep re
1156	12	2	3	29	26448.29	0.09	0.06	N	O	1997-01-24	1996-12-26	1997-02-04	DELIVER IN PERSON        	TRUCK     	ts sleep sly
1156	172	3	4	42	45031.14	0.02	0.00	N	O	1997-01-18	1997-01-12	1997-02-13	NONE                     	REG AIR   	s. quickly bold pains are
1156	74	4	5	49	47729.43	0.04	0.01	N	O	1996-11-16	1996-12-02	1996-12-05	COLLECT COD              	AIR       	ithely unusual in
1156	195	9	6	42	45997.98	0.02	0.06	N	O	1997-01-27	1997-01-09	1997-01-28	DELIVER IN PERSON        	MAIL      	even requests boost ironic deposits. pe
1156	47	6	7	20	18940.80	0.08	0.07	N	O	1997-01-01	1997-01-06	1997-01-16	COLLECT COD              	MAIL      	deposits sleep bravel
1157	49	2	1	16	15184.64	0.06	0.00	N	O	1998-04-12	1998-03-09	1998-04-23	DELIVER IN PERSON        	MAIL      	tions hang
1157	83	4	2	4	3932.32	0.10	0.05	N	O	1998-02-24	1998-03-30	1998-03-24	DELIVER IN PERSON        	SHIP      	ounts. ironic deposits
1157	48	7	3	8	7584.32	0.02	0.00	N	O	1998-03-25	1998-03-16	1998-03-29	NONE                     	REG AIR   	blithely even pa
1157	77	8	4	46	44945.22	0.07	0.08	N	O	1998-04-19	1998-03-13	1998-04-23	NONE                     	FOB       	slyly regular excuses. accounts
1157	160	5	5	14	14842.24	0.03	0.03	N	O	1998-04-17	1998-03-03	1998-05-01	NONE                     	FOB       	theodolites. fluffily re
1159	109	10	1	39	39354.90	0.01	0.00	A	F	1992-11-20	1992-10-28	1992-12-18	TAKE BACK RETURN         	FOB       	 blithely express reques
1159	96	9	2	7	6972.63	0.08	0.00	A	F	1992-11-25	1992-10-27	1992-12-20	NONE                     	AIR       	olve somet
1159	98	10	3	11	10978.99	0.10	0.03	R	F	1992-12-09	1992-12-07	1992-12-18	DELIVER IN PERSON        	MAIL      	h furiousl
1184	47	4	1	27	25570.08	0.01	0.00	N	O	1998-01-10	1997-12-02	1998-02-06	TAKE BACK RETURN         	REG AIR   	s wake fluffily. fl
1184	147	10	2	4	4188.56	0.04	0.03	N	O	1997-12-25	1998-01-24	1998-01-18	DELIVER IN PERSON        	RAIL      	 express packages. slyly expres
1184	164	5	3	7	7449.12	0.05	0.00	N	O	1998-02-14	1998-01-06	1998-03-11	COLLECT COD              	TRUCK     	ckly warthogs. blithely bold foxes hag
1184	126	9	4	3	3078.36	0.02	0.05	N	O	1998-01-15	1997-12-19	1998-02-02	NONE                     	REG AIR   	ar packages. final packages cajol
1188	115	9	1	2	2030.22	0.00	0.04	N	O	1996-05-22	1996-05-23	1996-06-06	COLLECT COD              	RAIL      	its breach blit
1188	113	4	2	9	9117.99	0.01	0.08	N	O	1996-08-04	1996-06-04	1996-08-19	NONE                     	REG AIR   	ow carefully ironic d
1188	179	10	3	41	44245.97	0.07	0.04	N	O	1996-06-29	1996-05-21	1996-07-21	TAKE BACK RETURN         	TRUCK     	althy packages. fluffily unusual ideas h
1191	49	6	1	29	27522.16	0.00	0.04	N	O	1996-01-24	1996-01-28	1996-02-17	COLLECT COD              	AIR       	 regular pin
1217	60	5	1	45	43202.70	0.07	0.02	A	F	1992-07-01	1992-06-23	1992-07-06	COLLECT COD              	AIR       	riously close ideas
1249	59	4	1	49	46993.45	0.07	0.05	A	F	1994-03-03	1994-02-28	1994-03-08	NONE                     	RAIL      	ffily express theodo
1252	87	8	1	13	12832.04	0.10	0.01	N	O	1997-09-07	1997-09-12	1997-10-01	COLLECT COD              	REG AIR   	sts dazzle
1252	111	8	2	27	27299.97	0.00	0.08	N	O	1997-10-22	1997-10-10	1997-11-10	TAKE BACK RETURN         	REG AIR   	packages hag
1252	40	1	3	19	17860.76	0.07	0.02	N	O	1997-10-13	1997-10-23	1997-10-18	NONE                     	AIR       	ts wake carefully-- packages sleep. quick 
1252	92	4	4	11	10912.99	0.10	0.01	N	O	1997-10-16	1997-09-22	1997-10-28	COLLECT COD              	AIR       	s are. slyly final requests among the
1252	79	10	5	26	25455.82	0.05	0.05	N	O	1997-08-05	1997-10-24	1997-08-07	DELIVER IN PERSON        	SHIP      	onic pinto beans haggle furiously 
1254	193	5	1	6	6559.14	0.08	0.01	N	O	1996-02-02	1996-03-21	1996-02-29	NONE                     	REG AIR   	lithely even deposits eat!
1254	200	3	2	47	51709.40	0.05	0.06	N	O	1996-03-07	1996-02-20	1996-04-05	COLLECT COD              	MAIL      	 platelets cajol
1254	135	6	3	35	36229.55	0.05	0.06	N	O	1996-04-08	1996-02-29	1996-04-18	DELIVER IN PERSON        	FOB       	ckages boost. furious warhorses cajole
1280	129	8	1	17	17495.04	0.01	0.01	A	F	1993-02-04	1993-04-10	1993-02-07	NONE                     	FOB       	ructions integrate across the th
1280	189	10	2	6	6535.08	0.05	0.06	R	F	1993-03-30	1993-02-16	1993-04-18	DELIVER IN PERSON        	AIR       	gular deposits 
1280	33	4	3	13	12129.39	0.03	0.02	R	F	1993-03-06	1993-03-11	1993-03-18	DELIVER IN PERSON        	TRUCK     	blithely final accounts use evenly 
1280	175	3	4	5	5375.85	0.06	0.03	R	F	1993-02-03	1993-02-11	1993-02-23	DELIVER IN PERSON        	AIR       	beans haggle. quickly bold instructions h
1280	52	10	5	24	22849.20	0.07	0.02	R	F	1993-03-20	1993-03-01	1993-04-09	COLLECT COD              	RAIL      	y pending orbits boost after the slyly
1280	66	3	6	9	8694.54	0.00	0.05	R	F	1993-04-18	1993-03-28	1993-05-04	DELIVER IN PERSON        	FOB       	usual accou
1280	92	6	7	19	18849.71	0.02	0.06	A	F	1993-02-07	1993-02-28	1993-02-12	NONE                     	TRUCK     	lyly along the furiously regular 
1282	23	4	1	14	12922.28	0.04	0.02	R	F	1992-06-29	1992-04-05	1992-07-21	TAKE BACK RETURN         	REG AIR   	ecial deposit
1282	30	9	2	10	9300.30	0.09	0.06	R	F	1992-04-10	1992-04-16	1992-05-01	DELIVER IN PERSON        	SHIP      	r theodolite
1282	160	1	3	19	20143.04	0.01	0.03	R	F	1992-05-07	1992-04-07	1992-05-13	NONE                     	RAIL      	ts x-ray across the furi
1282	59	10	4	19	18221.95	0.00	0.05	A	F	1992-06-20	1992-04-17	1992-07-05	DELIVER IN PERSON        	REG AIR   	nto beans. carefully close theodo
1283	93	7	1	47	46675.23	0.05	0.03	N	O	1996-10-21	1996-10-29	1996-11-12	DELIVER IN PERSON        	TRUCK     	even instructions boost slyly blithely 
1283	106	1	2	1	1006.10	0.00	0.08	N	O	1996-10-07	1996-10-12	1996-10-08	NONE                     	RAIL      	d the sauternes. slyly ev
1283	138	4	3	18	18686.34	0.02	0.01	N	O	1996-10-14	1996-11-07	1996-10-22	DELIVER IN PERSON        	AIR       	equests use along the fluff
1283	192	4	4	40	43687.60	0.07	0.03	N	O	1996-11-09	1996-11-23	1996-11-28	NONE                     	MAIL      	riously. even, ironic instructions after
1283	124	9	5	43	44037.16	0.01	0.04	N	O	1996-09-29	1996-11-19	1996-10-26	TAKE BACK RETURN         	RAIL      	requests sleep slyly about the 
1283	8	5	6	30	27240.00	0.06	0.07	N	O	1996-11-22	1996-11-22	1996-12-15	COLLECT COD              	TRUCK     	t the fluffily
1283	197	8	7	21	23040.99	0.04	0.03	N	O	1996-09-12	1996-10-02	1996-10-12	NONE                     	REG AIR   	fully regular 
1285	22	3	1	12	11064.24	0.00	0.06	A	F	1992-06-21	1992-08-16	1992-07-12	COLLECT COD              	MAIL      	ss foxes. blithe theodolites cajole slyly
1285	143	10	2	45	46941.30	0.01	0.02	R	F	1992-09-05	1992-08-08	1992-10-02	COLLECT COD              	REG AIR   	 special requests haggle blithely.
1285	189	10	3	4	4356.72	0.09	0.06	A	F	1992-07-20	1992-08-17	1992-07-26	DELIVER IN PERSON        	FOB       	l packages sleep slyly quiet i
1285	188	9	4	39	42439.02	0.05	0.01	A	F	1992-09-15	1992-08-05	1992-10-05	DELIVER IN PERSON        	TRUCK     	uctions. car
1285	84	5	5	33	32474.64	0.00	0.08	R	F	1992-09-08	1992-08-25	1992-09-16	NONE                     	SHIP      	ites affix
1286	178	9	1	49	52830.33	0.08	0.01	R	F	1993-06-24	1993-08-12	1993-06-26	DELIVER IN PERSON        	SHIP      	gged accoun
1286	49	6	2	48	45553.92	0.01	0.04	A	F	1993-07-11	1993-07-11	1993-08-01	COLLECT COD              	TRUCK     	unts alongs
1286	189	10	3	11	11980.98	0.03	0.04	R	F	1993-08-08	1993-07-30	1993-09-05	DELIVER IN PERSON        	FOB       	 slyly even packages. requ
1286	184	5	4	37	40114.66	0.00	0.02	R	F	1993-05-27	1993-07-11	1993-06-01	COLLECT COD              	SHIP      	lyly ironic pinto beans cajole furiously s
1286	165	10	5	14	14912.24	0.00	0.01	R	F	1993-05-23	1993-08-09	1993-06-01	NONE                     	REG AIR   	blithely bo
1286	146	5	6	41	42891.74	0.04	0.05	R	F	1993-08-02	1993-08-06	1993-08-07	TAKE BACK RETURN         	FOB       	 the furiously expre
1287	174	3	1	35	37595.95	0.09	0.06	A	F	1994-09-07	1994-09-12	1994-09-30	TAKE BACK RETURN         	FOB       	s wake unusual grou
1287	95	8	2	10	9950.90	0.08	0.03	R	F	1994-07-08	1994-08-28	1994-07-10	TAKE BACK RETURN         	RAIL      	thely alongside of the unusual, ironic pa
1287	1	2	3	30	27030.00	0.00	0.07	R	F	1994-07-12	1994-09-23	1994-08-07	NONE                     	RAIL      	ar packages. even, even
1287	62	7	4	10	9620.60	0.01	0.05	A	F	1994-09-03	1994-08-12	1994-09-16	TAKE BACK RETURN         	REG AIR   	ding, regular accounts
1287	179	8	5	21	22662.57	0.06	0.02	A	F	1994-10-06	1994-09-25	1994-10-16	TAKE BACK RETURN         	TRUCK     	y quickly bold theodoli
1287	21	10	6	26	23946.52	0.03	0.08	R	F	1994-10-03	1994-09-27	1994-10-30	DELIVER IN PERSON        	RAIL      	egular foxes. theodolites nag along t
1312	81	2	1	9	8829.72	0.04	0.08	R	F	1994-07-19	1994-06-29	1994-07-24	TAKE BACK RETURN         	MAIL      	. furiously 
1312	136	7	2	28	29011.64	0.06	0.06	A	F	1994-09-09	1994-08-01	1994-10-02	TAKE BACK RETURN         	FOB       	uriously final frays should use quick
1312	173	1	3	18	19317.06	0.03	0.07	A	F	1994-09-13	1994-07-08	1994-09-22	TAKE BACK RETURN         	MAIL      	. slyly ironic
1314	198	10	1	5	5490.95	0.03	0.01	A	F	1994-05-26	1994-08-06	1994-05-31	TAKE BACK RETURN         	AIR       	equests nag across the furious
1314	110	5	2	39	39394.29	0.01	0.03	R	F	1994-08-09	1994-06-14	1994-08-31	TAKE BACK RETURN         	TRUCK     	 unusual accounts slee
1314	41	2	3	11	10351.44	0.01	0.04	A	F	1994-05-16	1994-07-30	1994-05-31	COLLECT COD              	REG AIR   	tegrate furious
1317	134	5	1	34	35160.42	0.08	0.04	N	O	1995-08-13	1995-08-08	1995-09-10	COLLECT COD              	RAIL      	deposits boost thinly blithely final id
1317	160	2	2	7	7421.12	0.05	0.01	A	F	1995-06-08	1995-08-03	1995-06-16	TAKE BACK RETURN         	SHIP      	 pinto beans according to the final, pend
1317	158	9	3	26	27511.90	0.01	0.02	N	O	1995-07-13	1995-06-26	1995-08-06	COLLECT COD              	RAIL      	leep along th
1317	106	3	4	35	35213.50	0.05	0.02	N	O	1995-07-16	1995-07-07	1995-07-22	TAKE BACK RETURN         	FOB       	r packages impress blithely car
1317	150	9	5	36	37805.40	0.02	0.00	N	O	1995-09-03	1995-07-06	1995-09-04	DELIVER IN PERSON        	AIR       	 deposits. quic
1319	61	8	1	21	20182.26	0.03	0.04	N	O	1996-10-05	1996-12-02	1996-10-28	COLLECT COD              	FOB       	s: carefully express 
1319	37	8	2	12	11244.36	0.09	0.05	N	O	1996-11-05	1996-12-12	1996-11-29	DELIVER IN PERSON        	TRUCK     	packages integrate furiously. expres
1345	198	9	1	49	53811.31	0.08	0.00	A	F	1992-12-27	1993-01-23	1993-01-06	NONE                     	FOB       	sly. furiously final accounts are blithely 
1345	12	9	2	37	33744.37	0.10	0.07	A	F	1992-11-27	1992-12-11	1992-12-07	COLLECT COD              	FOB       	e slyly express requests. ironic accounts c
1345	57	8	3	31	29668.55	0.08	0.07	R	F	1992-12-02	1992-12-29	1992-12-14	COLLECT COD              	REG AIR   	. slyly silent accounts sublat
1348	95	7	1	13	12936.17	0.01	0.01	N	O	1998-04-28	1998-06-05	1998-05-12	TAKE BACK RETURN         	SHIP      	 blithely r
1348	22	5	2	41	37802.82	0.07	0.03	N	O	1998-05-02	1998-05-26	1998-05-09	COLLECT COD              	RAIL      	kages. platelets about the ca
1348	199	10	3	40	43967.60	0.07	0.05	N	O	1998-08-14	1998-07-10	1998-08-27	COLLECT COD              	AIR       	fter the regu
1348	98	1	4	2	1996.18	0.01	0.04	N	O	1998-05-30	1998-06-20	1998-06-05	COLLECT COD              	MAIL      	lly final packages use fluffily express ac
1377	154	6	1	5	5270.75	0.06	0.05	N	O	1998-05-06	1998-07-08	1998-06-01	TAKE BACK RETURN         	FOB       	 final, final grouches. accoun
1377	33	9	2	3	2799.09	0.10	0.04	N	O	1998-04-30	1998-07-02	1998-05-14	DELIVER IN PERSON        	REG AIR   	yly enticing requ
1377	84	5	3	26	25586.08	0.07	0.07	N	O	1998-05-28	1998-06-11	1998-06-25	COLLECT COD              	SHIP      	egular deposits. quickly regular acco
1377	121	4	4	39	39823.68	0.00	0.03	N	O	1998-07-27	1998-07-18	1998-08-13	DELIVER IN PERSON        	SHIP      	e ironic, regular requests. carefully 
1377	33	9	5	19	17727.57	0.10	0.00	N	O	1998-06-20	1998-06-27	1998-07-20	NONE                     	AIR       	ught to are bold foxes
1377	154	6	6	17	17920.55	0.03	0.04	N	O	1998-06-19	1998-07-20	1998-07-14	NONE                     	REG AIR   	s must have to mold b
1378	197	10	1	34	37304.46	0.09	0.07	N	O	1996-07-08	1996-04-23	1996-07-09	COLLECT COD              	RAIL      	le furiously slyly final accounts. careful
1378	124	9	2	18	18434.16	0.05	0.02	N	O	1996-06-19	1996-05-16	1996-06-21	DELIVER IN PERSON        	RAIL      	 theodolites. i
1378	73	4	3	11	10703.77	0.10	0.03	N	O	1996-06-07	1996-05-09	1996-07-05	COLLECT COD              	TRUCK     	 blithely express hoc
1378	171	2	4	12	12854.04	0.02	0.06	N	O	1996-06-16	1996-05-23	1996-07-09	COLLECT COD              	SHIP      	notornis. b
1378	156	7	5	9	9505.35	0.06	0.05	N	O	1996-04-20	1996-04-13	1996-05-09	COLLECT COD              	REG AIR   	e carefully. carefully iron
1378	194	6	6	29	31731.51	0.05	0.05	N	O	1996-04-15	1996-04-23	1996-05-14	NONE                     	REG AIR   	ual packages are furiously blith
1379	73	3	1	13	12649.91	0.04	0.01	N	O	1998-06-08	1998-07-13	1998-06-16	NONE                     	AIR       	ully across the furiously iron
1379	118	2	2	50	50905.50	0.07	0.08	N	O	1998-08-31	1998-07-13	1998-09-02	TAKE BACK RETURN         	FOB       	olphins. ca
1379	13	7	3	24	21912.24	0.05	0.02	N	O	1998-07-06	1998-07-09	1998-07-29	DELIVER IN PERSON        	MAIL      	ages cajole carefully idly express re
1380	149	2	1	6	6294.84	0.00	0.04	N	O	1996-08-06	1996-10-01	1996-08-14	NONE                     	RAIL      	e foxes. slyly specia
1380	141	4	2	40	41645.60	0.02	0.02	N	O	1996-10-01	1996-08-14	1996-10-20	COLLECT COD              	RAIL      	ly final frets. ironic,
1380	78	9	3	15	14671.05	0.05	0.02	N	O	1996-07-14	1996-08-12	1996-08-03	NONE                     	FOB       	riously ironic foxes aff
1380	61	10	4	33	31714.98	0.04	0.07	N	O	1996-08-23	1996-10-01	1996-09-18	TAKE BACK RETURN         	SHIP      	e ironic, even excuses haggle 
1381	144	1	1	47	49074.58	0.08	0.04	N	O	1998-09-22	1998-08-12	1998-10-12	DELIVER IN PERSON        	AIR       	ly ironic deposits
1381	34	10	2	12	11208.36	0.07	0.08	N	O	1998-08-13	1998-08-12	1998-08-28	TAKE BACK RETURN         	AIR       	 furiously regular package
1382	162	3	1	18	19118.88	0.08	0.03	R	F	1993-08-30	1993-10-19	1993-09-03	DELIVER IN PERSON        	AIR       	hely regular deposits. fluffy s
1382	181	2	2	29	31354.22	0.08	0.04	A	F	1993-10-08	1993-11-11	1993-10-10	COLLECT COD              	FOB       	 haggle: closely even asymptot
1382	178	7	3	43	46361.31	0.10	0.04	A	F	1993-09-02	1993-10-06	1993-09-15	DELIVER IN PERSON        	AIR       	ress deposits. slyly ironic foxes are blit
1382	181	2	4	11	11892.98	0.04	0.04	R	F	1993-09-17	1993-09-29	1993-09-21	NONE                     	SHIP      	furiously unusual packages play quickly 
1382	157	8	5	31	32771.65	0.07	0.03	R	F	1993-10-26	1993-10-15	1993-11-09	TAKE BACK RETURN         	FOB       	hely regular dependencies. f
1382	10	5	6	38	34580.38	0.07	0.07	R	F	1993-11-17	1993-09-28	1993-11-20	COLLECT COD              	SHIP      	ake pending pinto beans. s
1382	23	4	7	5	4615.10	0.07	0.01	R	F	1993-10-02	1993-09-29	1993-10-12	DELIVER IN PERSON        	REG AIR   	ter the carefully final excuses. blit
1383	193	7	1	14	15304.66	0.07	0.06	A	F	1993-08-25	1993-07-09	1993-09-12	DELIVER IN PERSON        	RAIL      	ole carefully silent requests. car
1383	161	10	2	19	20162.04	0.06	0.04	R	F	1993-05-24	1993-07-07	1993-06-14	NONE                     	AIR       	lyly unusual accounts sle
1409	99	1	1	23	22979.07	0.01	0.03	A	F	1993-04-18	1993-02-25	1993-05-06	DELIVER IN PERSON        	FOB       	ions. slyly ironic packages wake quick
1409	65	2	2	36	34742.16	0.09	0.02	A	F	1993-01-27	1993-01-31	1993-02-07	COLLECT COD              	FOB       	ncies sleep carefully r
1409	160	1	3	17	18022.72	0.07	0.00	R	F	1993-04-15	1993-03-01	1993-04-29	NONE                     	REG AIR   	pending accounts poach. care
1411	17	7	1	9	8253.09	0.06	0.04	A	F	1995-03-08	1995-03-04	1995-03-11	DELIVER IN PERSON        	AIR       	accounts. furiou
1411	107	8	2	26	26184.60	0.02	0.02	A	F	1995-04-12	1995-01-24	1995-05-03	TAKE BACK RETURN         	TRUCK     	c packages. 
1411	27	6	3	37	34299.74	0.00	0.06	A	F	1995-02-27	1995-03-02	1995-03-24	NONE                     	MAIL      	d excuses. furiously final pear
1411	200	3	4	20	22004.00	0.01	0.03	R	F	1995-04-06	1995-03-16	1995-04-17	COLLECT COD              	FOB       	s against the
1411	83	4	5	46	45221.68	0.08	0.05	A	F	1995-04-03	1995-01-20	1995-04-05	DELIVER IN PERSON        	REG AIR   	ly daring instructions
1411	77	6	6	30	29312.10	0.09	0.04	A	F	1995-01-12	1995-02-01	1995-01-23	DELIVER IN PERSON        	MAIL      	ious foxes wake courts. caref
1412	58	3	1	37	35447.85	0.06	0.01	A	F	1993-04-10	1993-04-19	1993-04-12	DELIVER IN PERSON        	RAIL      	hely express excuses are 
1412	156	1	2	20	21123.00	0.10	0.05	A	F	1993-07-04	1993-05-18	1993-07-22	DELIVER IN PERSON        	REG AIR   	odolites sleep ironically
1412	23	2	3	2	1846.04	0.10	0.07	R	F	1993-04-01	1993-05-03	1993-04-12	DELIVER IN PERSON        	REG AIR   	s among the requests are a
1412	167	8	4	11	11738.76	0.05	0.07	R	F	1993-05-27	1993-05-30	1993-06-07	DELIVER IN PERSON        	MAIL      	en packages. regular packages dete
1412	158	6	5	11	11639.65	0.08	0.06	A	F	1993-03-30	1993-05-25	1993-04-21	NONE                     	FOB       	se slyly. special, unusual accounts nag bl
1414	38	4	1	39	36583.17	0.10	0.03	N	O	1995-09-22	1995-09-30	1995-10-07	NONE                     	MAIL      	quickly aro
1414	107	8	2	4	4028.40	0.02	0.05	N	O	1995-09-16	1995-11-01	1995-10-02	COLLECT COD              	AIR       	 haggle quickly
1415	149	10	1	25	26228.50	0.06	0.00	A	F	1994-09-03	1994-07-12	1994-09-13	DELIVER IN PERSON        	RAIL      	ect never fluff
1442	26	5	1	8	7408.16	0.05	0.01	A	F	1994-10-31	1994-09-04	1994-11-25	COLLECT COD              	AIR       	c deposits haggle after the even
1443	34	10	1	47	43899.41	0.04	0.06	N	O	1997-02-05	1997-02-02	1997-03-03	NONE                     	RAIL      	carefully ironic requests sl
1472	8	5	1	36	32688.00	0.04	0.05	N	O	1996-11-06	1996-11-13	1996-11-12	COLLECT COD              	SHIP      	riously silent deposits to the pending d
1472	133	4	2	26	26861.38	0.03	0.05	N	O	1996-11-08	1996-11-13	1996-12-02	DELIVER IN PERSON        	FOB       	ic packages w
1472	1	8	3	6	5406.00	0.08	0.01	N	O	1996-10-24	1996-11-19	1996-11-23	COLLECT COD              	FOB       	onic theodolites hinder slyly slyly r
1474	15	5	1	5	4575.05	0.05	0.04	A	F	1995-04-22	1995-02-20	1995-05-06	COLLECT COD              	SHIP      	ully final a
1474	123	8	2	30	30693.60	0.04	0.02	A	F	1995-03-23	1995-02-11	1995-04-17	DELIVER IN PERSON        	TRUCK     	usly. evenly express 
1474	92	5	3	18	17857.62	0.06	0.02	A	F	1995-01-23	1995-03-28	1995-02-03	NONE                     	RAIL      	after the special
1476	31	7	1	20	18620.60	0.02	0.03	N	O	1996-08-11	1996-09-18	1996-08-26	TAKE BACK RETURN         	AIR       	. bold deposits are carefully amo
1504	82	3	1	42	41247.36	0.02	0.03	R	F	1992-10-18	1992-10-14	1992-11-10	TAKE BACK RETURN         	FOB       	ep. carefully ironic excuses haggle quickl
1504	103	10	2	22	22068.20	0.04	0.03	A	F	1992-09-09	1992-10-29	1992-09-10	NONE                     	REG AIR   	 accounts sleep. furiou
1504	178	8	3	9	9703.53	0.07	0.02	R	F	1992-11-02	1992-10-12	1992-11-15	TAKE BACK RETURN         	RAIL      	y slyly regular courts.
1504	115	2	4	10	10151.10	0.04	0.07	A	F	1992-09-22	1992-10-22	1992-10-13	TAKE BACK RETURN         	TRUCK     	final theodolites. furiously e
1504	20	10	5	7	6440.14	0.02	0.00	R	F	1992-11-20	1992-11-23	1992-12-13	COLLECT COD              	MAIL      	y final packa
1507	68	5	1	25	24201.50	0.01	0.08	R	F	1994-01-07	1994-01-06	1994-01-11	NONE                     	RAIL      	xes. slyly busy de
1507	40	6	2	33	31021.32	0.04	0.02	A	F	1993-10-29	1993-12-23	1993-11-14	DELIVER IN PERSON        	REG AIR   	 asymptotes nag furiously above t
1507	86	7	3	39	38457.12	0.03	0.07	R	F	1993-11-04	1993-12-16	1993-12-03	TAKE BACK RETURN         	REG AIR   	ly even instructions.
1510	98	2	1	11	10978.99	0.09	0.04	N	O	1996-09-23	1996-12-03	1996-10-01	DELIVER IN PERSON        	RAIL      	e of the unusual accounts. stealthy deposit
1510	84	5	2	24	23617.92	0.05	0.04	N	O	1996-10-07	1996-10-22	1996-11-03	DELIVER IN PERSON        	REG AIR   	yly brave theod
1510	190	1	3	36	39246.84	0.07	0.02	N	O	1996-10-02	1996-11-23	1996-10-05	NONE                     	SHIP      	old deposits along the carefully
1510	182	3	4	8	8657.44	0.01	0.08	N	O	1996-10-26	1996-11-07	1996-10-30	TAKE BACK RETURN         	RAIL      	blithely express
1510	59	10	5	27	25894.35	0.08	0.06	N	O	1996-10-20	1996-12-05	1996-11-02	NONE                     	MAIL      	he blithely regular req
1510	14	5	6	3	2742.03	0.05	0.02	N	O	1996-10-31	1996-12-03	1996-11-13	COLLECT COD              	RAIL      	along the slyly regular pin
1510	22	1	7	50	46101.00	0.04	0.05	N	O	1996-11-01	1996-10-17	1996-11-28	NONE                     	MAIL      	even packages. carefully regular fo
1511	98	2	1	29	28944.61	0.01	0.04	N	O	1997-03-17	1997-02-11	1997-03-27	DELIVER IN PERSON        	AIR       	s cajole furiously against 
1511	62	9	2	32	30785.92	0.04	0.01	N	O	1997-01-06	1997-03-21	1997-01-26	TAKE BACK RETURN         	REG AIR   	 deposits. carefully ironi
1537	18	2	1	17	15606.17	0.01	0.03	A	F	1992-04-12	1992-04-19	1992-04-13	NONE                     	TRUCK     	he regular pack
1537	179	8	2	50	53958.50	0.08	0.00	R	F	1992-05-30	1992-05-14	1992-06-23	TAKE BACK RETURN         	MAIL      	special packages haggle slyly at the silent
1537	13	4	3	44	40172.44	0.05	0.04	R	F	1992-04-01	1992-03-31	1992-04-21	NONE                     	TRUCK     	lar courts.
1537	140	6	4	3	3120.42	0.08	0.07	R	F	1992-03-20	1992-04-14	1992-03-21	TAKE BACK RETURN         	SHIP      	s, final ideas detect sl
1539	196	9	1	21	23019.99	0.08	0.02	R	F	1995-04-19	1995-05-10	1995-04-27	COLLECT COD              	TRUCK     	ounts haggle. busy
1539	86	7	2	11	10846.88	0.01	0.08	A	F	1995-05-27	1995-04-13	1995-06-10	TAKE BACK RETURN         	TRUCK     	ly express requests. furiously 
1539	68	5	3	7	6776.42	0.09	0.04	R	F	1995-05-14	1995-04-16	1995-05-30	DELIVER IN PERSON        	AIR       	. fluffily reg
1542	58	9	1	37	35447.85	0.07	0.06	A	F	1993-12-15	1993-10-17	1994-01-07	TAKE BACK RETURN         	REG AIR   	e blithely unusual accounts. quic
1542	3	6	2	12	10836.00	0.09	0.06	R	F	1993-10-29	1993-11-02	1993-11-09	TAKE BACK RETURN         	RAIL      	carefully 
1542	6	7	3	18	16308.00	0.05	0.05	R	F	1993-10-17	1993-11-15	1993-10-26	TAKE BACK RETURN         	FOB       	pending instr
1542	143	10	4	21	21905.94	0.01	0.05	R	F	1993-10-13	1993-12-13	1993-11-12	NONE                     	RAIL      	y pending foxes nag blithely 
1542	155	7	5	46	48536.90	0.00	0.00	R	F	1993-09-28	1993-11-03	1993-10-15	COLLECT COD              	FOB       	ial instructions. ironically
1570	183	4	1	25	27079.50	0.00	0.06	N	O	1998-05-03	1998-06-02	1998-06-02	DELIVER IN PERSON        	REG AIR   	its. slyly regular sentiments
1570	86	7	2	7	6902.56	0.05	0.05	N	O	1998-07-10	1998-06-01	1998-07-23	TAKE BACK RETURN         	MAIL      	requests boost quickly re
1572	24	5	1	41	37884.82	0.02	0.00	N	O	1996-05-16	1996-04-09	1996-05-28	TAKE BACK RETURN         	REG AIR   	. pinto beans alongside
1572	93	7	2	10	9930.90	0.04	0.06	N	O	1996-05-17	1996-03-26	1996-05-19	NONE                     	AIR       	 accounts affix slyly. 
1575	29	10	1	42	39018.84	0.05	0.08	N	O	1995-10-21	1995-11-25	1995-10-24	DELIVER IN PERSON        	RAIL      	ly pending pinto beans.
1575	36	7	2	39	36505.17	0.00	0.06	N	O	1995-10-30	1995-10-15	1995-11-10	COLLECT COD              	TRUCK     	 ironic requests snooze ironic, regular acc
1575	2	5	3	12	10824.00	0.01	0.05	N	O	1995-12-27	1995-11-11	1996-01-23	TAKE BACK RETURN         	AIR       	 bold accounts. furi
1575	111	1	4	39	39433.29	0.07	0.00	N	O	1995-09-23	1995-11-05	1995-09-25	TAKE BACK RETURN         	TRUCK     	 after the unusual asym
1575	83	4	5	10	9830.80	0.09	0.00	N	O	1996-01-10	1995-11-20	1996-01-13	DELIVER IN PERSON        	RAIL      	k excuses. pinto beans wake a
1575	178	6	6	14	15094.38	0.08	0.06	N	O	1995-10-31	1995-12-06	1995-11-30	NONE                     	AIR       	beans breach among the furiously specia
1575	117	1	7	48	48821.28	0.08	0.04	N	O	1995-11-19	1995-10-25	1995-12-07	DELIVER IN PERSON        	TRUCK     	cies. regu
1601	167	8	1	6	6402.96	0.00	0.00	A	F	1994-10-19	1994-09-28	1994-10-23	COLLECT COD              	SHIP      	 bold sheaves. furiously per
1601	175	3	2	50	53758.50	0.03	0.02	R	F	1994-12-24	1994-10-23	1995-01-11	COLLECT COD              	FOB       	ideas doubt
1601	90	1	3	14	13861.26	0.04	0.08	R	F	1994-09-17	1994-11-22	1994-10-03	DELIVER IN PERSON        	RAIL      	he special, fin
1602	183	4	1	4	4332.72	0.08	0.06	R	F	1993-10-31	1993-09-05	1993-11-21	NONE                     	RAIL      	y. even excuses
1603	39	5	1	1	939.03	0.08	0.00	R	F	1993-08-17	1993-09-04	1993-08-22	TAKE BACK RETURN         	REG AIR   	d accounts. special warthogs use fur
1603	66	5	2	29	28015.74	0.06	0.08	A	F	1993-09-28	1993-09-20	1993-10-28	NONE                     	SHIP      	ses wake furiously. theodolite
1605	142	1	1	47	48980.58	0.00	0.01	N	O	1998-04-29	1998-06-12	1998-05-20	DELIVER IN PERSON        	AIR       	. carefully r
1605	180	8	2	18	19443.24	0.10	0.00	N	O	1998-05-13	1998-06-17	1998-06-03	COLLECT COD              	REG AIR   	ly regular foxes wake carefully. bol
1605	59	10	3	39	37402.95	0.02	0.03	N	O	1998-07-12	1998-06-05	1998-08-09	DELIVER IN PERSON        	MAIL      	nal dependencies-- quickly final frets acc
1605	183	4	4	25	27079.50	0.06	0.02	N	O	1998-05-26	1998-06-14	1998-06-05	COLLECT COD              	AIR       	ole carefully car
1606	115	6	1	21	21317.31	0.04	0.00	N	O	1997-06-02	1997-07-02	1997-06-27	DELIVER IN PERSON        	RAIL      	 pending theodolites prom
1606	174	3	2	35	37595.95	0.00	0.02	N	O	1997-06-20	1997-06-19	1997-06-22	COLLECT COD              	TRUCK     	carefully sil
1606	100	4	3	23	23002.30	0.00	0.06	N	O	1997-04-19	1997-06-26	1997-04-30	NONE                     	MAIL      	ously final requests. slowly ironic ex
1606	97	9	4	20	19941.80	0.02	0.04	N	O	1997-05-01	1997-05-26	1997-05-28	TAKE BACK RETURN         	TRUCK     	fily carefu
1606	71	10	5	14	13594.98	0.10	0.01	N	O	1997-05-19	1997-07-05	1997-06-10	COLLECT COD              	FOB       	structions haggle f
1607	190	1	1	2	2180.38	0.02	0.00	N	O	1996-01-11	1996-02-15	1996-01-19	DELIVER IN PERSON        	MAIL      	packages haggle. regular requests boost s
1607	119	3	2	37	37707.07	0.05	0.02	N	O	1996-02-27	1996-02-18	1996-03-16	NONE                     	AIR       	alongside 
1607	123	4	3	39	39901.68	0.00	0.00	N	O	1996-02-01	1996-02-12	1996-02-16	NONE                     	FOB       	uches cajole. accounts ar
1607	76	6	4	34	33186.38	0.05	0.06	N	O	1996-01-06	1996-02-24	1996-01-10	DELIVER IN PERSON        	SHIP      	 quickly above the 
1607	178	8	5	48	51752.16	0.00	0.05	N	O	1996-02-22	1996-02-13	1996-03-09	TAKE BACK RETURN         	MAIL      	ular forges. deposits a
1634	48	9	1	21	19908.84	0.00	0.00	N	O	1996-10-04	1996-10-22	1996-11-01	NONE                     	MAIL      	counts alo
1634	172	3	2	44	47175.48	0.05	0.01	N	O	1996-09-17	1996-11-09	1996-10-03	COLLECT COD              	SHIP      	requests affix slyly. quickly even pack
1634	19	10	3	21	19299.21	0.06	0.07	N	O	1996-11-16	1996-10-21	1996-11-27	NONE                     	TRUCK     	y along the excuses.
1634	68	3	4	17	16457.02	0.08	0.07	N	O	1996-10-29	1996-10-15	1996-11-02	TAKE BACK RETURN         	SHIP      	cial, bold platelets alongside of the f
1634	76	7	5	2	1952.14	0.07	0.04	N	O	1996-11-22	1996-10-28	1996-12-17	NONE                     	SHIP      	ly. carefully regular asymptotes wake
1634	170	9	6	11	11771.87	0.01	0.08	N	O	1996-10-04	1996-12-06	1996-10-14	DELIVER IN PERSON        	SHIP      	final requests 
1634	13	7	7	35	31955.35	0.06	0.02	N	O	1996-11-25	1996-11-25	1996-12-12	TAKE BACK RETURN         	RAIL      	cies. regular, special de
1636	85	6	1	2	1970.16	0.09	0.03	N	O	1997-09-26	1997-08-22	1997-10-05	NONE                     	TRUCK     	nal foxes cajole above the blithely reg
1636	169	10	2	45	48112.20	0.03	0.01	N	O	1997-07-14	1997-08-08	1997-07-27	COLLECT COD              	RAIL      	ely express reque
1636	108	1	3	24	24194.40	0.07	0.08	N	O	1997-10-07	1997-08-12	1997-11-04	TAKE BACK RETURN         	MAIL      	e carefully unusual ideas are f
1636	153	1	4	43	45285.45	0.06	0.00	N	O	1997-08-23	1997-08-10	1997-09-17	NONE                     	REG AIR   	blithely special r
1636	19	6	5	22	20218.22	0.05	0.02	N	O	1997-07-22	1997-08-18	1997-08-03	COLLECT COD              	AIR       	ular, regu
1636	63	2	6	34	32744.04	0.10	0.01	N	O	1997-08-11	1997-09-09	1997-08-23	NONE                     	TRUCK     	ular depos
1636	114	1	7	7	7098.77	0.04	0.00	N	O	1997-07-28	1997-09-10	1997-07-31	NONE                     	MAIL      	ronic instructions. final
1666	185	6	1	30	32555.40	0.04	0.03	N	O	1995-10-28	1995-11-30	1995-11-18	TAKE BACK RETURN         	AIR       	 breach evenly final accounts. r
1666	64	1	2	20	19281.20	0.01	0.00	N	O	1996-01-27	1995-12-12	1996-01-31	NONE                     	REG AIR   	uietly regular foxes wake quick
1666	134	10	3	31	32058.03	0.05	0.07	N	O	1996-02-11	1996-01-11	1996-02-28	COLLECT COD              	RAIL      	ding to the express, bold accounts. fu
1666	169	8	4	41	43835.56	0.06	0.08	N	O	1995-11-29	1996-01-04	1995-12-24	NONE                     	TRUCK     	ly regular excuses; regular ac
1669	79	10	1	24	23497.68	0.04	0.08	N	O	1997-09-04	1997-07-30	1997-09-20	DELIVER IN PERSON        	RAIL      	 regular, final deposits use quick
1671	149	2	1	21	22031.94	0.02	0.07	N	O	1996-07-28	1996-09-28	1996-08-08	TAKE BACK RETURN         	AIR       	s accounts slee
1671	96	10	2	4	3984.36	0.05	0.00	N	O	1996-08-30	1996-09-19	1996-09-23	DELIVER IN PERSON        	TRUCK     	lyly regular ac
1671	124	3	3	11	11265.32	0.06	0.08	N	O	1996-09-16	1996-10-21	1996-09-18	NONE                     	SHIP      	tes sleep blithely
1671	178	7	4	5	5390.85	0.00	0.00	N	O	1996-11-14	1996-10-20	1996-11-25	TAKE BACK RETURN         	FOB       	luffily regular deposits
1671	127	8	5	12	12325.44	0.07	0.04	N	O	1996-11-17	1996-09-02	1996-12-17	COLLECT COD              	RAIL      	special, ironic
1671	197	9	6	46	50470.74	0.08	0.05	N	O	1996-09-13	1996-10-14	1996-09-28	TAKE BACK RETURN         	REG AIR   	. slyly bold instructions boost. furiousl
1700	140	1	1	38	39525.32	0.04	0.04	N	O	1996-10-03	1996-07-27	1996-10-22	NONE                     	RAIL      	ular dependencies engage slyly 
1700	156	7	2	49	51751.35	0.04	0.00	N	O	1996-09-26	1996-07-28	1996-10-16	NONE                     	TRUCK     	kly even dependencies haggle fluffi
1729	157	8	1	12	12685.80	0.08	0.04	A	F	1992-08-11	1992-07-24	1992-08-16	COLLECT COD              	RAIL      	y pending packages detect. carefully re
1730	166	5	1	41	43712.56	0.01	0.03	N	O	1998-08-11	1998-08-29	1998-09-02	TAKE BACK RETURN         	TRUCK     	 instructions. unusual, even Tiresi
1730	162	3	2	15	15932.40	0.07	0.04	N	O	1998-09-07	1998-09-12	1998-09-30	TAKE BACK RETURN         	AIR       	pinto beans cajole. bravely bold
1730	162	1	3	9	9559.44	0.10	0.00	N	O	1998-09-18	1998-09-15	1998-09-21	DELIVER IN PERSON        	FOB       	gular dependencies wake. blithely final e
1730	10	7	4	40	36400.40	0.02	0.03	N	O	1998-10-02	1998-10-06	1998-10-03	NONE                     	SHIP      	ven dinos slee
1730	141	4	5	43	44769.02	0.04	0.06	N	O	1998-10-26	1998-10-22	1998-11-02	DELIVER IN PERSON        	TRUCK     	ng deposits cajo
1734	155	3	1	38	40095.70	0.03	0.03	R	F	1994-08-09	1994-09-07	1994-08-12	COLLECT COD              	FOB       	ts doubt b
1734	118	2	2	4	4072.44	0.06	0.03	A	F	1994-08-20	1994-07-17	1994-08-25	DELIVER IN PERSON        	AIR       	final warhorses.
1762	26	5	1	15	13890.30	0.04	0.08	A	F	1994-12-18	1994-10-29	1995-01-17	TAKE BACK RETURN         	REG AIR   	old packages thrash. care
1762	50	3	2	39	37051.95	0.10	0.02	A	F	1994-09-12	1994-11-09	1994-10-08	DELIVER IN PERSON        	MAIL      	 ironic platelets sleep along t
1762	32	8	3	7	6524.21	0.05	0.01	R	F	1994-09-03	1994-10-02	1994-09-10	NONE                     	REG AIR   	uickly express packages wake slyly-- regul
1762	145	2	4	24	25083.36	0.03	0.03	A	F	1994-11-30	1994-11-02	1994-12-20	NONE                     	REG AIR   	accounts solve alongside of the fluffily 
1762	8	9	5	49	44492.00	0.08	0.05	A	F	1994-10-20	1994-11-02	1994-11-10	TAKE BACK RETURN         	SHIP      	 packages sleep fluffily pen
1762	94	7	6	35	34793.15	0.05	0.05	A	F	1994-11-25	1994-10-21	1994-11-28	COLLECT COD              	AIR       	ind quickly. accounts ca
1762	73	3	7	47	45734.29	0.03	0.01	A	F	1994-11-02	1994-10-07	1994-11-08	NONE                     	SHIP      	 blithely brave
1763	12	9	1	22	20064.22	0.09	0.06	N	O	1997-01-17	1997-01-15	1997-02-03	TAKE BACK RETURN         	SHIP      	ld. fluffily final ideas boos
1763	157	5	2	43	45457.45	0.04	0.04	N	O	1996-11-04	1996-12-09	1996-11-28	DELIVER IN PERSON        	FOB       	r deposits integrate blithely pending, quic
1763	25	10	3	16	14800.32	0.06	0.02	N	O	1996-12-12	1996-12-04	1996-12-25	DELIVER IN PERSON        	RAIL      	ously pending asymptotes a
1763	61	6	4	44	42286.64	0.04	0.05	N	O	1996-12-04	1997-01-06	1996-12-25	DELIVER IN PERSON        	REG AIR   	 instructions need to integrate deposits. 
1763	147	4	5	13	13612.82	0.03	0.05	N	O	1996-11-23	1997-01-24	1996-12-05	TAKE BACK RETURN         	SHIP      	s sleep carefully. fluffily unusua
1763	143	4	6	3	3129.42	0.05	0.03	N	O	1996-12-10	1996-12-06	1997-01-04	TAKE BACK RETURN         	FOB       	ut the slyly pending deposi
1763	184	5	7	2	2168.36	0.05	0.07	N	O	1997-02-27	1996-12-04	1997-03-27	COLLECT COD              	FOB       	even pinto beans snooze fluffi
1793	48	5	1	29	27493.16	0.01	0.06	R	F	1992-10-24	1992-09-20	1992-11-23	NONE                     	MAIL      	ar excuses. 
1793	126	9	2	4	4104.48	0.07	0.05	A	F	1992-07-28	1992-08-26	1992-08-21	COLLECT COD              	RAIL      	nic foxes along the even
1793	131	7	3	6	6186.78	0.01	0.05	R	F	1992-09-21	1992-09-05	1992-10-01	DELIVER IN PERSON        	REG AIR   	uctions; depo
1793	118	8	4	4	4072.44	0.00	0.08	R	F	1992-09-27	1992-09-21	1992-10-07	DELIVER IN PERSON        	AIR       	equests nod ac
1793	25	6	5	42	38850.84	0.03	0.03	A	F	1992-10-13	1992-10-02	1992-11-06	NONE                     	RAIL      	uctions sleep carefully special, fl
1797	31	7	1	17	15827.51	0.01	0.02	N	O	1996-08-06	1996-07-11	1996-08-29	NONE                     	TRUCK     	 cajole carefully. unusual Tiresias e
1797	145	2	2	16	16722.24	0.01	0.00	N	O	1996-06-03	1996-07-21	1996-06-07	NONE                     	FOB       	o beans wake regular accounts. blit
1797	12	9	3	21	19152.21	0.02	0.01	N	O	1996-08-05	1996-08-05	1996-08-06	DELIVER IN PERSON        	AIR       	ns. regular, regular deposit
1799	52	10	1	8	7616.40	0.04	0.08	R	F	1994-06-14	1994-05-27	1994-06-27	TAKE BACK RETURN         	MAIL      	ealms upon the special, ironic waters
1799	27	10	2	42	38934.84	0.02	0.02	R	F	1994-04-05	1994-04-28	1994-04-09	DELIVER IN PERSON        	FOB       	es pending 
1826	27	10	1	4	3708.08	0.06	0.00	R	F	1992-07-05	1992-06-12	1992-08-04	DELIVER IN PERSON        	MAIL      	alongside of the quickly unusual re
1826	68	3	2	9	8712.54	0.07	0.07	R	F	1992-07-12	1992-07-11	1992-07-15	DELIVER IN PERSON        	TRUCK     	 blithely special
1826	176	4	3	14	15066.38	0.05	0.01	A	F	1992-04-28	1992-05-31	1992-05-25	COLLECT COD              	TRUCK     	uriously bold pinto beans are carefully ag
1826	180	9	4	6	6481.08	0.05	0.04	R	F	1992-06-30	1992-05-17	1992-07-30	DELIVER IN PERSON        	RAIL      	kages. blithely silent
1826	135	1	5	46	47615.98	0.05	0.06	R	F	1992-05-02	1992-06-25	1992-05-26	TAKE BACK RETURN         	FOB       	ously? quickly pe
1826	108	3	6	43	43348.30	0.02	0.03	A	F	1992-07-28	1992-06-14	1992-08-03	NONE                     	MAIL      	ss tithes use even ideas. fluffily final t
1828	100	4	1	33	33003.30	0.05	0.04	R	F	1994-06-27	1994-06-10	1994-07-24	COLLECT COD              	FOB       	s boost carefully. pending d
1828	13	3	2	40	36520.40	0.08	0.07	R	F	1994-05-05	1994-07-02	1994-05-19	COLLECT COD              	REG AIR   	s use above the quietly fin
1828	196	7	3	11	12058.09	0.07	0.08	R	F	1994-07-21	1994-05-28	1994-08-13	DELIVER IN PERSON        	FOB       	 wake blithely 
1828	8	3	4	45	40860.00	0.02	0.05	R	F	1994-05-15	1994-05-29	1994-05-28	COLLECT COD              	RAIL      	 accounts run slyly 
1828	79	7	5	14	13706.98	0.01	0.08	A	F	1994-05-20	1994-06-02	1994-05-25	TAKE BACK RETURN         	SHIP      	. final packages along the carefully bold
1829	150	7	1	12	12601.80	0.05	0.06	A	F	1994-08-23	1994-07-13	1994-09-04	DELIVER IN PERSON        	FOB       	ges wake furiously express pinto
1829	5	6	2	11	9955.00	0.04	0.05	A	F	1994-05-18	1994-06-13	1994-06-07	COLLECT COD              	MAIL      	ding orbits
1829	104	9	3	49	49200.90	0.09	0.08	A	F	1994-08-26	1994-08-01	1994-09-16	NONE                     	TRUCK     	ound the quickly 
1829	153	4	4	14	14744.10	0.03	0.06	A	F	1994-08-15	1994-06-08	1994-08-30	TAKE BACK RETURN         	AIR       	regular deposits alongside of the flu
1829	166	5	5	6	6396.96	0.02	0.07	A	F	1994-08-09	1994-08-05	1994-09-05	DELIVER IN PERSON        	MAIL      	s haggle! slyl
1829	115	5	6	36	36543.96	0.09	0.04	R	F	1994-06-10	1994-06-23	1994-06-22	NONE                     	FOB       	ackages-- express requests sleep; pen
1830	120	4	1	38	38764.56	0.00	0.07	R	F	1995-04-20	1995-05-22	1995-04-24	TAKE BACK RETURN         	TRUCK     	ely even a
1830	25	10	2	9	8325.18	0.05	0.07	R	F	1995-03-09	1995-05-24	1995-03-14	NONE                     	SHIP      	st furiously among 
1830	82	3	3	36	35354.88	0.07	0.07	R	F	1995-04-21	1995-04-14	1995-05-10	DELIVER IN PERSON        	SHIP      	 slowly unusual orbits. carefull
1857	174	5	1	15	16112.55	0.10	0.03	R	F	1993-04-05	1993-02-28	1993-04-13	COLLECT COD              	RAIL      	egular, regular inst
1857	167	6	2	40	42686.40	0.10	0.00	R	F	1993-02-15	1993-03-08	1993-02-21	NONE                     	AIR       	slyly close d
1857	119	3	3	8	8152.88	0.01	0.07	R	F	1993-01-27	1993-04-04	1993-02-20	TAKE BACK RETURN         	AIR       	slyly about the fluffily silent req
1857	100	3	4	41	41004.10	0.07	0.07	A	F	1993-04-16	1993-02-16	1993-04-18	NONE                     	REG AIR   	 the slyly
1858	14	8	1	33	30162.33	0.01	0.02	N	O	1997-12-28	1998-02-03	1998-01-13	NONE                     	RAIL      	tect along the slyly final
1859	75	6	1	18	17551.26	0.10	0.00	N	O	1997-08-08	1997-06-30	1997-08-26	TAKE BACK RETURN         	SHIP      	e carefully a
1859	188	9	2	36	39174.48	0.02	0.01	N	O	1997-05-05	1997-07-08	1997-05-25	TAKE BACK RETURN         	REG AIR   	regular requests. carefully unusual theo
1859	158	10	3	5	5290.75	0.06	0.03	N	O	1997-06-20	1997-05-20	1997-07-19	TAKE BACK RETURN         	AIR       	across the p
1859	191	2	4	21	22914.99	0.00	0.03	N	O	1997-08-06	1997-05-29	1997-08-26	TAKE BACK RETURN         	REG AIR   	lar packages wake quickly exp
1859	46	3	5	11	10406.44	0.06	0.06	N	O	1997-07-15	1997-06-05	1997-07-29	TAKE BACK RETURN         	SHIP      	ffily ironic pac
1859	105	8	6	12	12061.20	0.08	0.03	N	O	1997-05-22	1997-06-08	1997-06-07	COLLECT COD              	TRUCK     	es. unusual, silent request
1861	68	5	1	7	6776.42	0.08	0.05	A	F	1994-01-14	1994-04-03	1994-01-16	COLLECT COD              	RAIL      	s foxes. slyly
1861	27	8	2	31	28737.62	0.10	0.05	R	F	1994-01-29	1994-03-07	1994-02-15	TAKE BACK RETURN         	RAIL      	arefully unusual
1861	24	9	3	23	21252.46	0.00	0.08	A	F	1994-04-09	1994-03-04	1994-04-11	DELIVER IN PERSON        	MAIL      	in packages sleep silent dolphins; sly
1861	116	6	4	38	38612.18	0.10	0.05	R	F	1994-02-26	1994-02-05	1994-03-01	NONE                     	RAIL      	pending deposits cajole quic
1861	16	3	5	2	1832.02	0.03	0.08	R	F	1994-04-26	1994-03-15	1994-05-15	TAKE BACK RETURN         	MAIL      	e final, regular requests. carefully 
1862	30	5	1	41	38131.23	0.10	0.00	N	O	1998-06-05	1998-05-17	1998-07-04	COLLECT COD              	FOB       	 carefully along
1862	166	7	2	37	39447.92	0.06	0.02	N	O	1998-04-15	1998-05-15	1998-05-14	TAKE BACK RETURN         	MAIL      	l deposits. carefully even dep
1862	104	1	3	26	26106.60	0.02	0.01	N	O	1998-03-25	1998-05-17	1998-04-17	TAKE BACK RETURN         	TRUCK     	g carefully: thinly ironic deposits af
1888	98	10	1	27	26948.43	0.03	0.06	R	F	1994-02-13	1994-01-16	1994-02-25	NONE                     	REG AIR   	. carefully special dolphins sle
1888	74	5	2	38	37014.66	0.03	0.03	R	F	1993-11-29	1994-01-16	1993-12-08	TAKE BACK RETURN         	TRUCK     	dazzle carefull
1888	80	1	3	49	48023.92	0.07	0.05	A	F	1994-02-27	1994-01-14	1994-03-28	DELIVER IN PERSON        	FOB       	lar accounts haggle carefu
1888	19	10	4	9	8271.09	0.01	0.04	A	F	1994-02-09	1994-01-22	1994-02-19	NONE                     	AIR       	 packages are blithely. carefu
1888	160	1	5	4	4240.64	0.03	0.06	R	F	1993-12-28	1993-12-19	1994-01-11	COLLECT COD              	FOB       	lphins. ironically special theodolit
1888	53	8	6	48	45746.40	0.08	0.08	R	F	1994-02-28	1993-12-16	1994-03-15	COLLECT COD              	TRUCK     	ar ideas cajole. regular p
1888	167	6	7	50	53358.00	0.04	0.07	R	F	1993-12-22	1994-01-10	1994-01-06	DELIVER IN PERSON        	FOB       	ependencies affix blithely regular warhors
1891	77	8	1	45	43968.15	0.07	0.04	A	F	1994-12-20	1995-01-16	1995-01-05	NONE                     	RAIL      	ests along
1891	184	5	2	18	19515.24	0.06	0.00	A	F	1995-01-24	1995-01-29	1995-02-14	NONE                     	RAIL      	 foxes above the carefu
1891	198	9	3	15	16472.85	0.03	0.00	R	F	1995-03-11	1995-03-05	1995-03-18	TAKE BACK RETURN         	MAIL      	 accounts are furiou
1893	99	1	1	43	42960.87	0.10	0.00	N	O	1998-01-25	1998-01-06	1998-02-14	COLLECT COD              	SHIP      	he carefully regular 
1893	148	9	2	49	51358.86	0.03	0.05	N	O	1998-01-19	1998-01-28	1998-02-02	TAKE BACK RETURN         	FOB       	y final foxes bo
1893	45	6	3	3	2835.12	0.03	0.02	N	O	1998-02-10	1998-01-18	1998-02-25	DELIVER IN PERSON        	MAIL      	gular, even ideas. fluffily bol
1893	101	6	4	18	18019.80	0.07	0.06	N	O	1998-01-24	1998-01-12	1998-02-13	TAKE BACK RETURN         	RAIL      	g packages. fluffily final reques
1893	53	4	5	6	5718.30	0.10	0.02	N	O	1998-01-23	1997-12-22	1998-02-09	DELIVER IN PERSON        	TRUCK     	ar accounts use. daringly ironic packag
1895	161	6	1	43	45629.88	0.09	0.07	R	F	1994-07-26	1994-07-19	1994-08-11	NONE                     	AIR       	 carefully eve
1921	21	10	1	9	8289.18	0.08	0.00	R	F	1994-02-01	1994-03-20	1994-03-01	DELIVER IN PERSON        	FOB       	to beans. even excuses integrate specia
1921	140	6	2	21	21842.94	0.02	0.06	R	F	1994-02-08	1994-03-28	1994-02-15	COLLECT COD              	FOB       	ckly regula
1921	71	2	3	27	26218.89	0.00	0.04	A	F	1994-04-26	1994-04-07	1994-04-30	TAKE BACK RETURN         	FOB       	ing pinto beans above the pend
1953	128	1	1	25	25703.00	0.07	0.06	A	F	1994-01-07	1994-01-28	1994-01-29	TAKE BACK RETURN         	RAIL      	ular, regular i
1953	14	5	2	35	31990.35	0.06	0.06	R	F	1994-02-03	1994-02-25	1994-02-14	DELIVER IN PERSON        	FOB       	among the fur
1956	177	8	1	8	8617.36	0.02	0.04	A	F	1992-12-25	1992-11-24	1993-01-12	TAKE BACK RETURN         	AIR       	efully about the ironic, ironic de
1956	103	6	2	16	16049.60	0.00	0.05	R	F	1992-11-11	1992-11-11	1992-11-30	NONE                     	FOB       	es cajole blithely. pen
1956	139	5	3	39	40526.07	0.08	0.02	A	F	1992-09-24	1992-11-26	1992-10-15	DELIVER IN PERSON        	REG AIR   	r theodolites sleep above the b
1956	29	10	4	11	10219.22	0.10	0.00	A	F	1992-12-19	1992-10-29	1993-01-07	TAKE BACK RETURN         	AIR       	 the braids slee
1956	155	10	5	16	16882.40	0.08	0.02	R	F	1992-09-28	1992-10-21	1992-09-30	TAKE BACK RETURN         	FOB       	 wake after the 
1957	79	9	1	50	48953.50	0.09	0.05	N	O	1998-08-08	1998-09-28	1998-08-27	COLLECT COD              	FOB       	gainst the re
1957	119	3	2	31	31592.41	0.10	0.08	N	O	1998-08-13	1998-08-31	1998-08-16	NONE                     	REG AIR   	express packages maintain fluffi
1958	73	2	1	9	8757.63	0.01	0.05	N	O	1995-12-08	1995-12-17	1995-12-18	DELIVER IN PERSON        	REG AIR   	ickly. slyly bold 
1958	176	7	2	29	31208.93	0.05	0.06	N	O	1996-01-19	1995-12-05	1996-02-14	COLLECT COD              	SHIP      	d pinto beans
1958	102	3	3	4	4008.40	0.04	0.02	N	O	1995-10-24	1995-12-09	1995-10-28	DELIVER IN PERSON        	AIR       	he slyly even dependencies 
1958	83	4	4	38	37357.04	0.09	0.07	N	O	1995-10-09	1995-11-26	1995-11-05	COLLECT COD              	TRUCK     	yly. slyly regular courts use silentl
1958	101	8	5	31	31034.10	0.08	0.01	N	O	1995-10-31	1995-11-12	1995-11-07	TAKE BACK RETURN         	TRUCK     	r deposits c
1958	17	4	6	44	40348.44	0.08	0.04	N	O	1995-12-17	1995-11-30	1996-01-15	TAKE BACK RETURN         	RAIL      	c theodolites after the unusual deposit
1958	39	5	7	29	27231.87	0.02	0.05	N	O	1995-10-14	1995-11-06	1995-11-01	NONE                     	REG AIR   	final requests nag according to the 
1959	169	10	1	46	49181.36	0.04	0.00	N	O	1997-05-05	1997-03-03	1997-05-24	TAKE BACK RETURN         	AIR       	 furiously ex
1959	120	7	2	15	15301.80	0.08	0.07	N	O	1997-01-20	1997-02-18	1997-02-08	DELIVER IN PERSON        	MAIL      	 quickly sp
1985	28	1	1	33	30624.66	0.10	0.03	R	F	1994-12-04	1994-11-01	1994-12-05	DELIVER IN PERSON        	FOB       	s are express packages. pendin
1985	21	6	2	50	46051.00	0.04	0.02	R	F	1994-09-30	1994-10-18	1994-10-12	COLLECT COD              	AIR       	ate carefully. carefully
1985	134	10	3	20	20682.60	0.07	0.03	R	F	1994-10-29	1994-11-12	1994-11-27	NONE                     	TRUCK     	regular requests. furiously express
1985	199	10	4	30	32975.70	0.05	0.07	R	F	1994-09-06	1994-10-10	1994-09-26	NONE                     	RAIL      	uickly. instr
1985	124	9	5	42	43013.04	0.05	0.05	R	F	1994-10-25	1994-11-03	1994-11-19	DELIVER IN PERSON        	SHIP      	 patterns? final requests after the sp
1985	20	7	6	2	1840.04	0.02	0.00	A	F	1994-11-25	1994-10-09	1994-12-25	TAKE BACK RETURN         	FOB       	 silent inst
1986	92	3	1	12	11905.08	0.06	0.05	A	F	1994-08-17	1994-06-28	1994-09-02	COLLECT COD              	RAIL      	sleep furiously fluffily final
1986	105	8	2	10	10051.00	0.10	0.03	R	F	1994-05-14	1994-06-21	1994-06-02	COLLECT COD              	REG AIR   	yly into the carefully even 
1986	63	2	3	14	13482.84	0.04	0.02	R	F	1994-07-14	1994-06-19	1994-08-08	NONE                     	SHIP      	the packages. pending, unusual
1987	16	6	1	7	6412.07	0.03	0.03	A	F	1994-07-30	1994-07-06	1994-08-29	NONE                     	REG AIR   	 regular a
1988	72	1	1	36	34994.52	0.09	0.04	N	O	1996-01-21	1995-11-24	1996-01-27	NONE                     	RAIL      	gular theodolites. 
1988	199	3	2	19	20884.61	0.08	0.08	N	O	1996-02-03	1995-12-10	1996-02-14	COLLECT COD              	FOB       	lly about the slyly thin instructions. f
1988	54	6	3	8	7632.40	0.06	0.01	N	O	1995-10-20	1995-11-11	1995-11-18	DELIVER IN PERSON        	AIR       	le quickly ac
1988	36	2	4	27	25272.81	0.08	0.00	N	O	1996-01-27	1995-12-24	1996-02-24	TAKE BACK RETURN         	TRUCK     	uests. regular requests are according to t
1988	79	8	5	26	25455.82	0.08	0.04	N	O	1996-01-25	1995-12-15	1996-01-26	COLLECT COD              	SHIP      	 ironic dolphins haggl
1988	86	7	6	9	8874.72	0.08	0.03	N	O	1995-12-26	1996-01-02	1996-01-25	DELIVER IN PERSON        	MAIL      	lar platelets. slyly ironic packa
1989	10	7	1	47	42770.47	0.10	0.02	R	F	1994-06-21	1994-05-27	1994-06-22	TAKE BACK RETURN         	REG AIR   	final deposits s
1991	110	3	1	39	39394.29	0.06	0.02	A	F	1993-01-01	1992-11-29	1993-01-10	TAKE BACK RETURN         	TRUCK     	ckages? carefully bold depos
1991	53	1	2	49	46699.45	0.08	0.06	R	F	1992-10-19	1992-11-29	1992-10-25	NONE                     	SHIP      	nd the ideas affi
1991	174	5	3	6	6445.02	0.02	0.01	A	F	1992-11-02	1992-10-08	1992-11-14	TAKE BACK RETURN         	REG AIR   	hes nag slyly
1991	138	9	4	6	6228.78	0.10	0.06	A	F	1992-11-21	1992-11-03	1992-11-27	NONE                     	RAIL      	uickly blithely final de
1991	60	8	5	49	47042.94	0.06	0.00	R	F	1992-09-10	1992-11-30	1992-10-07	NONE                     	AIR       	quests cajole blithely
2016	147	4	1	2	2094.28	0.02	0.07	N	O	1996-10-12	1996-11-09	1996-10-31	DELIVER IN PERSON        	TRUCK     	carefully according to the 
2016	63	8	2	15	14445.90	0.04	0.05	N	O	1996-09-24	1996-10-05	1996-10-21	TAKE BACK RETURN         	MAIL      	uests haggle carefully furiously regul
2016	122	7	3	8	8176.96	0.09	0.02	N	O	1996-09-19	1996-10-21	1996-10-13	TAKE BACK RETURN         	SHIP      	mptotes haggle ideas. packages wake flu
2018	195	6	1	2	2190.38	0.02	0.07	N	O	1995-06-25	1995-06-20	1995-07-04	NONE                     	TRUCK     	ly ironic accounts against the slyly sly
2018	129	10	2	23	23669.76	0.05	0.01	R	F	1995-05-05	1995-05-12	1995-05-22	TAKE BACK RETURN         	RAIL      	ingly even theodolites s
2021	85	6	1	7	6895.56	0.08	0.04	N	O	1995-10-17	1995-09-29	1995-10-20	NONE                     	MAIL      	 accounts boost blithely. blithely reg
2021	166	3	2	19	20257.04	0.04	0.05	N	O	1995-08-14	1995-09-05	1995-08-23	NONE                     	RAIL      	 above the slyly fl
2023	127	10	1	9	9244.08	0.05	0.04	R	F	1992-06-04	1992-06-30	1992-06-10	NONE                     	AIR       	ly regular pinto beans poa
2023	38	4	2	2	1876.06	0.01	0.00	R	F	1992-08-27	1992-07-16	1992-08-29	DELIVER IN PERSON        	RAIL      	ing packages. fluffily silen
2023	19	6	3	25	22975.25	0.10	0.03	A	F	1992-07-19	1992-07-07	1992-08-15	NONE                     	REG AIR   	 wake furiously among the slyly final
2023	185	6	4	9	9766.62	0.02	0.00	A	F	1992-07-23	1992-07-04	1992-08-20	TAKE BACK RETURN         	AIR       	nts maintain blithely alongside of the
2023	20	10	5	22	20240.44	0.04	0.06	A	F	1992-06-15	1992-07-13	1992-06-21	TAKE BACK RETURN         	SHIP      	ronic attainments. 
2023	43	2	6	29	27348.16	0.02	0.06	A	F	1992-08-29	1992-07-28	1992-09-18	COLLECT COD              	RAIL      	usual instructions. bli
2023	134	10	7	50	51706.50	0.00	0.03	R	F	1992-06-20	1992-07-04	1992-06-23	DELIVER IN PERSON        	FOB       	its! carefully ex
2049	189	10	1	25	27229.50	0.08	0.00	N	O	1996-03-31	1996-02-29	1996-04-15	DELIVER IN PERSON        	MAIL      	 excuses above the 
2049	35	1	2	31	28985.93	0.10	0.05	N	O	1995-12-25	1996-02-25	1995-12-29	TAKE BACK RETURN         	MAIL      	 packages are slyly alongside
2049	67	6	3	18	17407.08	0.05	0.05	N	O	1996-01-09	1996-01-22	1996-01-25	TAKE BACK RETURN         	AIR       	 sleep fluffily. dependencies use never
2049	6	7	4	39	35334.00	0.02	0.05	N	O	1996-01-17	1996-01-21	1996-02-03	TAKE BACK RETURN         	MAIL      	the even pinto beans 
2049	126	1	5	30	30783.60	0.04	0.06	N	O	1995-12-16	1996-02-04	1995-12-22	NONE                     	TRUCK     	ial accounts are among the furiously perma
2049	84	5	6	17	16729.36	0.07	0.00	N	O	1996-02-04	1996-03-01	1996-02-24	NONE                     	FOB       	al, regular foxes. pending, 
2055	45	6	1	15	14175.60	0.04	0.06	A	F	1993-09-15	1993-10-06	1993-10-07	NONE                     	REG AIR   	furiously bold 
2055	9	10	2	15	13635.00	0.06	0.05	R	F	1993-10-30	1993-11-21	1993-11-22	COLLECT COD              	RAIL      	gular foxes. b
2055	135	1	3	12	12421.56	0.00	0.02	A	F	1993-10-26	1993-11-23	1993-11-22	COLLECT COD              	TRUCK     	al pains. acco
2055	134	10	4	16	16546.08	0.02	0.02	A	F	1993-11-16	1993-11-12	1993-11-28	NONE                     	TRUCK     	arefully daringly regular accounts.
2080	7	4	1	5	4535.00	0.08	0.05	R	F	1993-08-26	1993-08-07	1993-09-02	DELIVER IN PERSON        	TRUCK     	refully unusual theo
2080	197	9	2	39	42790.41	0.07	0.04	A	F	1993-08-22	1993-09-09	1993-08-23	COLLECT COD              	FOB       	ic deposits haggle slyly carefully eve
2081	89	10	1	26	25716.08	0.03	0.08	N	O	1997-10-21	1997-10-03	1997-11-10	NONE                     	FOB       	among the slyly express accounts. silen
2081	149	2	2	13	13638.82	0.07	0.05	N	O	1997-08-23	1997-08-22	1997-09-09	TAKE BACK RETURN         	MAIL      	fter the even deposi
2081	13	10	3	32	29216.32	0.09	0.07	N	O	1997-09-05	1997-09-26	1997-10-03	TAKE BACK RETURN         	SHIP      	e. final, regular dependencies sleep slyly!
2081	85	6	4	23	22656.84	0.03	0.08	N	O	1997-07-06	1997-09-11	1997-07-21	TAKE BACK RETURN         	MAIL      	ual requests wake blithely above the
2081	113	7	5	19	19249.09	0.02	0.06	N	O	1997-10-01	1997-08-12	1997-10-18	COLLECT COD              	SHIP      	s affix sometimes express requests. quickly
2081	142	9	6	31	32306.34	0.03	0.06	N	O	1997-09-19	1997-09-13	1997-09-27	NONE                     	AIR       	 silent, spe
2113	123	8	1	40	40924.80	0.04	0.06	N	O	1998-01-16	1997-12-11	1998-02-06	TAKE BACK RETURN         	TRUCK     	bout the quickly ironic t
2113	112	2	2	24	24290.64	0.03	0.02	N	O	1998-02-19	1998-01-08	1998-03-16	COLLECT COD              	MAIL      	kly regular accounts hinder about the
2114	168	9	1	50	53408.00	0.05	0.05	A	F	1995-02-05	1995-03-18	1995-02-13	COLLECT COD              	RAIL      	pecial pinto bean
2114	186	7	2	26	28240.68	0.02	0.02	A	F	1995-04-30	1995-04-16	1995-05-28	NONE                     	SHIP      	ar asymptotes sleep 
2114	162	1	3	25	26554.00	0.07	0.01	A	F	1995-02-15	1995-03-13	1995-02-22	COLLECT COD              	AIR       	unts. regular, express accounts wake. b
2146	57	5	1	42	40196.10	0.10	0.01	A	F	1992-09-21	1992-11-02	1992-09-23	NONE                     	AIR       	ns according to the doggedly 
2146	157	5	2	6	6342.90	0.07	0.05	A	F	1993-01-03	1992-10-24	1993-01-24	DELIVER IN PERSON        	RAIL      	ing to the requests. dependencies boost 
2146	25	8	3	14	12950.28	0.03	0.01	R	F	1992-09-16	1992-10-16	1992-09-20	COLLECT COD              	SHIP      	ecial, express a
2146	26	9	4	31	28706.62	0.02	0.00	A	F	1993-01-04	1992-10-24	1993-01-15	DELIVER IN PERSON        	TRUCK     	lly even deposit
2146	169	4	5	28	29936.48	0.02	0.05	R	F	1993-01-03	1992-10-17	1993-01-08	COLLECT COD              	MAIL      	r accounts sleep furio
2146	71	9	6	32	31074.24	0.07	0.03	R	F	1993-01-10	1992-10-19	1993-02-05	COLLECT COD              	TRUCK     	y regular foxes wake among the final
2146	25	6	7	39	36075.78	0.07	0.06	R	F	1993-01-05	1992-11-06	1993-01-14	DELIVER IN PERSON        	TRUCK     	uickly regular excuses detect. regular c
2147	29	8	1	50	46451.00	0.04	0.06	R	F	1992-11-18	1992-11-30	1992-11-30	NONE                     	RAIL      	al accounts. even, even foxes wake
2147	101	2	2	4	4004.40	0.01	0.04	A	F	1992-09-27	1992-11-15	1992-10-22	NONE                     	AIR       	mong the blithely special
2147	44	7	3	34	32097.36	0.10	0.04	R	F	1992-11-29	1992-11-08	1992-12-22	TAKE BACK RETURN         	REG AIR   	egular deposits hang car
2147	11	8	4	11	10021.11	0.06	0.07	A	F	1992-09-27	1992-11-16	1992-10-16	NONE                     	AIR       	 the fluffily
2150	78	7	1	26	25429.82	0.00	0.03	A	F	1994-06-21	1994-08-05	1994-06-23	NONE                     	TRUCK     	. always unusual packages
2150	18	8	2	29	26622.29	0.04	0.03	A	F	1994-09-02	1994-08-04	1994-10-02	TAKE BACK RETURN         	RAIL      	y ironic theodolites. foxes ca
2150	107	2	3	29	29205.90	0.04	0.08	R	F	1994-06-10	1994-07-31	1994-06-26	COLLECT COD              	RAIL      	arefully final att
2150	54	6	4	39	37207.95	0.05	0.02	R	F	1994-07-31	1994-08-17	1994-08-11	TAKE BACK RETURN         	TRUCK     	ess accounts nag. unusual asymptotes haggl
2150	183	4	5	35	37911.30	0.01	0.01	A	F	1994-09-27	1994-08-17	1994-10-13	COLLECT COD              	RAIL      	refully pending dependen
2150	7	10	6	12	10884.00	0.09	0.03	A	F	1994-08-27	1994-08-22	1994-09-18	COLLECT COD              	AIR       	press platelets haggle until the slyly fi
2151	167	2	1	23	24544.68	0.06	0.02	N	O	1996-11-20	1996-12-17	1996-11-30	DELIVER IN PERSON        	AIR       	 silent dependencies about the slyl
2151	15	9	2	29	26535.29	0.00	0.02	N	O	1997-03-04	1996-12-27	1997-03-21	TAKE BACK RETURN         	SHIP      	 bold packages acro
2151	165	2	3	49	52192.84	0.07	0.01	N	O	1997-01-20	1997-02-09	1997-02-18	NONE                     	FOB       	 packages. f
2151	18	5	4	28	25704.28	0.10	0.08	N	O	1996-12-11	1996-12-26	1996-12-12	DELIVER IN PERSON        	AIR       	y special packages. carefully ironic instru
2178	157	2	1	15	15857.25	0.10	0.01	N	O	1997-03-27	1997-03-10	1997-04-18	NONE                     	REG AIR   	l accounts. quickly expr
2178	16	10	2	27	24732.27	0.01	0.02	N	O	1997-02-26	1997-02-19	1997-03-25	NONE                     	MAIL      	 across the ironic reques
2178	5	2	3	40	36200.00	0.00	0.03	N	O	1997-03-17	1997-02-09	1997-04-15	COLLECT COD              	RAIL      	foxes are slowly regularly specia
2178	78	6	4	3	2934.21	0.07	0.07	N	O	1997-04-07	1997-01-23	1997-04-18	COLLECT COD              	MAIL      	 permanentl
2210	78	7	1	36	35210.52	0.10	0.00	A	F	1992-03-04	1992-03-24	1992-03-21	DELIVER IN PERSON        	AIR       	 requests wake enticingly final
2212	71	10	1	18	17479.26	0.07	0.06	R	F	1994-06-22	1994-06-18	1994-06-25	TAKE BACK RETURN         	FOB       	 cajole. final, pending ideas should are bl
2242	123	4	1	15	15346.80	0.09	0.08	N	O	1997-08-04	1997-09-21	1997-08-11	COLLECT COD              	FOB       	its. carefully express packages cajole. bli
2244	51	6	1	3	2853.15	0.02	0.02	A	F	1993-04-30	1993-03-15	1993-05-19	TAKE BACK RETURN         	FOB       	 beans for the regular platel
2244	193	6	2	16	17491.04	0.01	0.06	R	F	1993-02-12	1993-03-09	1993-02-28	COLLECT COD              	FOB       	rate around the reques
2245	76	7	1	44	42947.08	0.03	0.03	A	F	1993-06-12	1993-06-10	1993-06-16	NONE                     	TRUCK     	refully even sheaves
2245	74	3	2	28	27273.96	0.05	0.03	R	F	1993-08-19	1993-07-27	1993-09-04	COLLECT COD              	TRUCK     	e requests sleep furiou
2245	86	7	3	33	32540.64	0.03	0.01	R	F	1993-06-26	1993-06-11	1993-07-17	TAKE BACK RETURN         	AIR       	ing to the carefully ruthless accounts
2245	189	10	4	14	15248.52	0.02	0.04	R	F	1993-05-06	1993-07-21	1993-05-19	DELIVER IN PERSON        	RAIL      	nts. always unusual dep
2245	80	8	5	33	32342.64	0.03	0.07	R	F	1993-06-16	1993-06-05	1993-07-07	NONE                     	MAIL      	 across the express reques
2247	172	2	1	12	12866.04	0.02	0.07	A	F	1992-09-06	1992-09-18	1992-09-26	NONE                     	MAIL      	final accounts. requests across the furiou
2275	34	5	1	30	28020.90	0.08	0.05	R	F	1993-01-10	1992-11-21	1993-01-22	NONE                     	REG AIR   	re slyly slyly special idea
2275	91	4	2	11	10901.99	0.08	0.03	A	F	1993-01-16	1992-12-10	1993-01-25	COLLECT COD              	REG AIR   	ost across the never express instruction
2276	119	9	1	5	5095.55	0.07	0.08	N	O	1996-05-09	1996-06-18	1996-05-13	DELIVER IN PERSON        	FOB       	ias instea
2276	135	1	2	13	13456.69	0.08	0.04	N	O	1996-07-24	1996-06-18	1996-08-16	COLLECT COD              	RAIL      	arefully ironic foxes cajole q
2276	171	2	3	27	28921.59	0.07	0.08	N	O	1996-07-30	1996-06-10	1996-07-31	DELIVER IN PERSON        	RAIL      	the carefully unusual accoun
2276	109	6	4	38	38345.80	0.06	0.03	N	O	1996-07-07	1996-06-28	1996-07-17	COLLECT COD              	RAIL      	ans. pinto beans boost c
2276	153	5	5	50	52657.50	0.03	0.05	N	O	1996-07-13	1996-06-25	1996-07-22	DELIVER IN PERSON        	REG AIR   	 accounts dete
2276	6	9	6	4	3624.00	0.10	0.03	N	O	1996-07-05	1996-06-30	1996-08-04	COLLECT COD              	FOB       	s. deposits 
2277	137	8	1	38	39410.94	0.03	0.07	R	F	1995-04-23	1995-03-25	1995-05-20	TAKE BACK RETURN         	TRUCK     	fully bold
2277	8	1	2	2	1816.00	0.10	0.08	A	F	1995-02-01	1995-02-04	1995-03-02	TAKE BACK RETURN         	AIR       	endencies sleep idly pending p
2277	198	10	3	4	4392.76	0.05	0.06	R	F	1995-04-27	1995-03-16	1995-04-29	TAKE BACK RETURN         	SHIP      	. quickly unusual deposi
2277	159	4	4	31	32833.65	0.02	0.00	R	F	1995-03-07	1995-03-19	1995-03-26	TAKE BACK RETURN         	MAIL      	ic instructions detect ru
2305	174	4	1	3	3222.51	0.00	0.01	A	F	1993-03-24	1993-04-05	1993-03-29	NONE                     	AIR       	kages haggle quickly across the blithely 
2305	60	8	2	39	37442.34	0.07	0.00	R	F	1993-04-16	1993-04-17	1993-04-22	COLLECT COD              	MAIL      	ms after the foxes 
2305	102	3	3	32	32067.20	0.03	0.06	A	F	1993-04-02	1993-03-18	1993-04-03	NONE                     	AIR       	 haggle caref
2305	112	3	4	17	17205.87	0.00	0.05	A	F	1993-02-21	1993-03-30	1993-03-19	TAKE BACK RETURN         	MAIL      	 carefully alongside of 
2305	155	7	5	26	27433.90	0.06	0.07	A	F	1993-05-14	1993-02-28	1993-06-04	NONE                     	SHIP      	arefully final theodo
2305	51	3	6	7	6657.35	0.06	0.00	R	F	1993-05-15	1993-04-25	1993-06-09	DELIVER IN PERSON        	RAIL      	gular deposits boost about the foxe
2309	170	7	1	14	14982.38	0.10	0.03	N	O	1996-01-01	1995-10-22	1996-01-23	NONE                     	AIR       	asymptotes. furiously pending acco
2309	169	8	2	1	1069.16	0.01	0.05	N	O	1995-12-08	1995-11-03	1995-12-31	COLLECT COD              	RAIL      	eposits alongside of the final re
2309	15	2	3	5	4575.05	0.01	0.00	N	O	1995-12-10	1995-10-29	1996-01-06	TAKE BACK RETURN         	SHIP      	s. requests wake blithely specia
2309	139	10	4	46	47799.98	0.08	0.04	N	O	1995-10-02	1995-10-30	1995-10-30	NONE                     	REG AIR   	sly according to the carefully 
2309	137	3	5	9	9334.17	0.00	0.07	N	O	1995-12-21	1995-10-10	1996-01-20	COLLECT COD              	AIR       	ding, unusual instructions. dep
2309	195	8	6	21	22998.99	0.09	0.00	N	O	1995-11-05	1995-11-07	1995-11-22	NONE                     	AIR       	unts around the dolphins ar
2309	138	4	7	48	49830.24	0.03	0.05	N	O	1995-10-21	1995-11-21	1995-11-09	NONE                     	MAIL      	ccounts. id
2310	58	6	1	36	34489.80	0.03	0.03	N	O	1996-10-09	1996-10-28	1996-10-29	TAKE BACK RETURN         	RAIL      	iously against the slyly special accounts
2310	171	2	2	6	6427.02	0.07	0.01	N	O	1996-11-08	1996-12-09	1996-12-07	COLLECT COD              	REG AIR   	e slyly about the quickly ironic theodo
2310	42	1	3	48	45217.92	0.08	0.02	N	O	1996-10-04	1996-11-20	1996-10-25	TAKE BACK RETURN         	FOB       	ep slyly alongside of the 
2339	192	3	1	22	24028.18	0.03	0.03	A	F	1994-01-06	1994-03-06	1994-01-10	NONE                     	FOB       	 furiously above 
2339	30	5	2	28	26040.84	0.00	0.00	R	F	1994-01-25	1994-01-22	1994-01-28	DELIVER IN PERSON        	RAIL      	e bold, even packag
2339	117	4	3	13	13222.43	0.06	0.08	R	F	1994-03-10	1994-02-18	1994-03-20	TAKE BACK RETURN         	REG AIR   	ges. blithely special depend
2340	138	4	1	9	9343.17	0.08	0.02	N	O	1996-05-01	1996-02-24	1996-05-16	COLLECT COD              	RAIL      	. carefully ironic
2340	193	5	2	21	22956.99	0.06	0.02	N	O	1996-01-17	1996-03-04	1996-01-29	DELIVER IN PERSON        	SHIP      	 asymptotes. unusual theo
2341	47	10	1	12	11364.48	0.08	0.03	R	F	1993-06-06	1993-07-08	1993-06-17	DELIVER IN PERSON        	FOB       	. quickly final deposits sl
2341	71	10	2	37	35929.59	0.07	0.08	A	F	1993-09-23	1993-07-25	1993-10-14	DELIVER IN PERSON        	RAIL      	was blithel
2341	195	8	3	8	8761.52	0.03	0.07	R	F	1993-06-08	1993-07-09	1993-06-10	COLLECT COD              	FOB       	ns affix above the iron
2371	159	4	1	37	39188.55	0.05	0.05	N	O	1998-02-11	1998-03-24	1998-02-27	DELIVER IN PERSON        	TRUCK     	s boost fluffil
2371	35	1	2	21	19635.63	0.00	0.05	N	O	1998-04-14	1998-02-14	1998-04-18	COLLECT COD              	AIR       	gle furiously regu
2371	101	4	3	11	11012.10	0.05	0.02	N	O	1998-02-25	1998-04-06	1998-03-23	TAKE BACK RETURN         	TRUCK     	requests. regular pinto beans wake. car
2371	43	6	4	33	31120.32	0.05	0.08	N	O	1998-03-30	1998-02-06	1998-04-05	DELIVER IN PERSON        	AIR       	deas are. express r
2371	165	2	5	22	23433.52	0.02	0.05	N	O	1998-03-26	1998-03-19	1998-04-16	DELIVER IN PERSON        	REG AIR   	y daring accounts. regular ins
2371	86	7	6	39	38457.12	0.05	0.03	N	O	1998-04-01	1998-03-13	1998-04-27	NONE                     	REG AIR   	tructions. regular, stealthy packages wak
2371	36	2	7	32	29952.96	0.07	0.07	N	O	1998-02-15	1998-04-03	1998-02-23	NONE                     	REG AIR   	the ruthless accounts. 
2372	43	4	1	42	39607.68	0.08	0.02	N	O	1998-01-04	1998-01-02	1998-02-02	COLLECT COD              	REG AIR   	lar packages. regular
2372	3	10	2	17	15351.00	0.07	0.01	N	O	1997-12-17	1998-01-17	1997-12-25	NONE                     	RAIL      	xcuses. slyly ironic theod
2372	164	1	3	12	12769.92	0.04	0.04	N	O	1998-03-21	1997-12-21	1998-04-12	DELIVER IN PERSON        	SHIP      	lyly according to
2372	122	1	4	4	4088.48	0.00	0.07	N	O	1997-12-14	1997-12-28	1997-12-16	TAKE BACK RETURN         	REG AIR   	e carefully blithely even epitaphs. r
2372	20	7	5	5	4600.10	0.02	0.04	N	O	1998-02-08	1998-01-18	1998-03-02	TAKE BACK RETURN         	RAIL      	ets against the 
2372	189	10	6	11	11980.98	0.02	0.06	N	O	1998-02-14	1998-01-18	1998-03-10	TAKE BACK RETURN         	FOB       	 silent, pending de
2372	57	8	7	19	18183.95	0.01	0.06	N	O	1997-12-26	1998-02-19	1998-01-02	COLLECT COD              	SHIP      	 beans haggle sometimes
2404	147	10	1	36	37697.04	0.07	0.00	N	O	1997-03-27	1997-05-16	1997-04-06	COLLECT COD              	REG AIR   	s nag furi
2404	36	2	2	1	936.03	0.02	0.04	N	O	1997-05-22	1997-06-06	1997-05-28	DELIVER IN PERSON        	MAIL      	from the final orbits? even pinto beans hag
2404	18	5	3	41	37638.41	0.02	0.06	N	O	1997-06-12	1997-05-03	1997-07-12	NONE                     	AIR       	 dolphins are
2404	57	8	4	19	18183.95	0.09	0.03	N	O	1997-05-07	1997-05-24	1997-05-24	TAKE BACK RETURN         	SHIP      	cuses. quickly even in
2404	4	9	5	18	16272.00	0.00	0.04	N	O	1997-06-25	1997-05-06	1997-07-02	NONE                     	RAIL      	packages. even requests according to 
2406	170	5	1	18	19263.06	0.07	0.05	N	O	1997-02-17	1996-12-25	1997-02-19	COLLECT COD              	MAIL      	azzle furiously careful
2406	41	8	2	40	37641.60	0.02	0.07	N	O	1997-01-09	1996-12-02	1997-01-16	NONE                     	SHIP      	gular accounts caj
2406	50	1	3	16	15200.80	0.07	0.03	N	O	1996-10-31	1996-11-28	1996-11-08	TAKE BACK RETURN         	SHIP      	 special accou
2406	146	9	4	34	35568.76	0.07	0.06	N	O	1996-12-01	1996-12-07	1996-12-16	NONE                     	AIR       	hinly even accounts are slyly q
2406	187	8	5	25	27179.50	0.08	0.02	N	O	1996-12-03	1996-12-14	1996-12-26	COLLECT COD              	MAIL      	al, regular in
2406	59	4	6	22	21099.10	0.05	0.02	N	O	1996-11-22	1997-01-17	1996-12-15	NONE                     	TRUCK     	hely even foxes unwind furiously aga
2406	60	2	7	30	28801.80	0.07	0.07	N	O	1997-01-17	1997-01-12	1997-01-22	TAKE BACK RETURN         	TRUCK     	 final pinto beans han
2433	87	8	1	39	38496.12	0.01	0.04	R	F	1994-11-20	1994-09-23	1994-12-10	DELIVER IN PERSON        	SHIP      	ly final asy
2433	134	5	2	20	20682.60	0.05	0.06	A	F	1994-12-09	1994-10-20	1994-12-15	COLLECT COD              	REG AIR   	lithely blithely final ide
2433	157	2	3	38	40171.70	0.08	0.03	A	F	1994-10-15	1994-10-23	1994-11-06	DELIVER IN PERSON        	SHIP      	. slyly regular requests sle
2433	121	6	4	43	43908.16	0.01	0.05	A	F	1994-10-16	1994-10-23	1994-11-08	DELIVER IN PERSON        	RAIL      	ular requests. slyly even pa
2433	108	1	5	3	3024.30	0.06	0.02	A	F	1994-11-08	1994-09-24	1994-11-17	COLLECT COD              	AIR       	usly pending depos
2436	155	6	1	48	50647.20	0.04	0.02	N	O	1995-10-22	1995-10-22	1995-11-16	DELIVER IN PERSON        	FOB       	he furiously 
2436	117	7	2	18	18307.98	0.05	0.03	N	O	1995-10-14	1995-11-21	1995-11-12	TAKE BACK RETURN         	TRUCK     	y ironic accounts. furiously even packa
2436	164	3	3	6	6384.96	0.06	0.08	N	O	1995-10-25	1995-11-30	1995-11-24	DELIVER IN PERSON        	RAIL      	odolites. ep
2464	49	8	1	10	9490.40	0.05	0.03	N	O	1998-02-04	1997-12-29	1998-02-16	TAKE BACK RETURN         	RAIL      	slyly final pinto bean
2464	101	6	2	20	20022.00	0.01	0.07	N	O	1997-12-26	1998-01-02	1998-01-24	DELIVER IN PERSON        	FOB       	sts. slyly close ideas shall h
2465	68	5	1	27	26137.62	0.05	0.02	N	O	1995-09-05	1995-09-07	1995-09-17	DELIVER IN PERSON        	FOB       	posits boost carefully unusual instructio
2465	51	3	2	34	32335.70	0.02	0.05	N	O	1995-10-02	1995-08-04	1995-10-09	COLLECT COD              	RAIL      	posits wake. regular package
2465	32	3	3	8	7456.24	0.10	0.00	N	O	1995-10-16	1995-08-26	1995-11-07	TAKE BACK RETURN         	FOB       	s across the express deposits wak
2465	148	7	4	45	47166.30	0.03	0.01	N	O	1995-09-27	1995-08-25	1995-10-06	NONE                     	TRUCK     	y silent foxes. final pinto beans above 
2465	47	4	5	50	47352.00	0.01	0.04	N	O	1995-09-01	1995-09-06	1995-09-18	TAKE BACK RETURN         	TRUCK     	the pending th
2465	124	5	6	20	20482.40	0.03	0.03	N	O	1995-08-16	1995-08-13	1995-09-02	COLLECT COD              	FOB       	uriously? furiously ironic excu
2466	186	7	1	16	17378.88	0.00	0.02	R	F	1994-04-20	1994-04-20	1994-05-09	COLLECT COD              	FOB       	to beans sl
2466	105	8	2	10	10051.00	0.00	0.00	A	F	1994-05-08	1994-04-06	1994-06-05	DELIVER IN PERSON        	AIR       	sly regular deposits. regular, regula
2466	14	1	3	29	26506.29	0.10	0.07	A	F	1994-06-11	1994-04-27	1994-07-10	DELIVER IN PERSON        	FOB       	ckages. bold requests nag carefully.
2466	11	8	4	29	26419.29	0.04	0.04	A	F	1994-04-01	1994-04-20	1994-04-23	DELIVER IN PERSON        	MAIL      	es boost fluffily ab
2466	79	10	5	30	29372.10	0.02	0.01	A	F	1994-04-11	1994-05-02	1994-05-02	DELIVER IN PERSON        	REG AIR   	. fluffily even pinto beans are idly. f
2466	173	2	6	19	20390.23	0.10	0.07	R	F	1994-06-12	1994-04-18	1994-07-12	NONE                     	MAIL      	ccounts cajole a
2466	155	7	7	35	36930.25	0.10	0.00	A	F	1994-06-01	1994-05-27	1994-06-21	COLLECT COD              	AIR       	 packages detect carefully: ironically sl
2467	133	9	1	7	7231.91	0.00	0.00	N	O	1995-07-28	1995-10-04	1995-08-27	NONE                     	REG AIR   	gular packages cajole 
2468	94	7	1	46	45728.14	0.00	0.04	N	O	1997-07-16	1997-08-09	1997-08-07	COLLECT COD              	SHIP      	unusual theodolites su
2468	21	10	2	43	39603.86	0.00	0.04	N	O	1997-08-17	1997-08-21	1997-08-30	DELIVER IN PERSON        	FOB       	uriously eve
2468	195	6	3	44	48188.36	0.00	0.03	N	O	1997-10-01	1997-08-02	1997-10-09	TAKE BACK RETURN         	RAIL      	egular, silent sheave
2468	82	3	4	5	4910.40	0.08	0.00	N	O	1997-06-28	1997-08-02	1997-07-22	NONE                     	MAIL      	 sleep fluffily acc
2468	159	7	5	18	19064.70	0.07	0.00	N	O	1997-07-25	1997-08-26	1997-08-14	DELIVER IN PERSON        	REG AIR   	cies. fluffily r
2469	166	1	1	11	11727.76	0.00	0.04	N	O	1997-02-09	1997-01-26	1997-02-16	NONE                     	TRUCK     	ies wake carefully b
2469	114	1	2	16	16225.76	0.07	0.06	N	O	1997-02-19	1997-02-04	1997-03-18	NONE                     	MAIL      	ing asymptotes 
2469	11	5	3	48	43728.48	0.05	0.06	N	O	1997-01-11	1997-01-03	1997-01-15	TAKE BACK RETURN         	AIR       	riously even theodolites u
2469	88	9	4	35	34582.80	0.06	0.06	N	O	1997-02-04	1997-02-02	1997-02-17	DELIVER IN PERSON        	RAIL      	ld packages haggle regular frets. fluffily 
2469	121	4	5	30	30633.60	0.09	0.01	N	O	1996-12-21	1997-01-29	1997-01-02	COLLECT COD              	SHIP      	 accounts. regular theodolites affix fu
2469	104	5	6	49	49200.90	0.02	0.02	N	O	1997-03-03	1996-12-26	1997-03-13	NONE                     	AIR       	 requests are car
2469	127	10	7	8	8216.96	0.02	0.00	N	O	1997-03-15	1997-01-20	1997-04-13	NONE                     	TRUCK     	s. regular
2470	110	5	1	12	12121.32	0.06	0.06	N	O	1997-07-12	1997-05-24	1997-07-17	TAKE BACK RETURN         	FOB       	l accounts. deposits nag daringly. express,
2470	100	4	2	50	50005.00	0.03	0.03	N	O	1997-06-02	1997-06-01	1997-06-09	COLLECT COD              	AIR       	 packages 
2470	64	3	3	10	9640.60	0.05	0.08	N	O	1997-06-20	1997-06-19	1997-06-24	TAKE BACK RETURN         	FOB       	 ironic requests a
2470	162	3	4	30	31864.80	0.04	0.08	N	O	1997-08-04	1997-07-13	1997-08-14	DELIVER IN PERSON        	AIR       	s across the furiously fina
2471	84	5	1	37	36410.96	0.05	0.01	N	O	1998-05-28	1998-04-17	1998-06-08	COLLECT COD              	TRUCK     	ounts mold blithely carefully express depo
2498	143	2	1	48	50070.72	0.10	0.01	R	F	1993-11-25	1994-01-09	1993-12-24	DELIVER IN PERSON        	RAIL      	onic requests wake
2499	150	3	1	15	15752.25	0.04	0.06	N	O	1995-12-21	1995-12-06	1996-01-19	DELIVER IN PERSON        	FOB       	 slyly across the slyly
2499	46	3	2	48	45409.92	0.09	0.03	N	O	1995-10-14	1995-12-12	1995-11-11	DELIVER IN PERSON        	AIR       	ronic ideas cajole quickly requests. caref
2499	133	9	3	31	32027.03	0.09	0.05	N	O	1995-12-09	1995-10-28	1996-01-05	COLLECT COD              	AIR       	to beans across the carefully ironic theodo
2499	159	7	4	39	41306.85	0.06	0.02	N	O	1995-10-26	1995-10-27	1995-11-07	TAKE BACK RETURN         	SHIP      	otes sublat
2499	130	9	5	6	6180.78	0.02	0.01	N	O	1995-11-19	1995-12-14	1995-12-08	NONE                     	SHIP      	cording to the
2499	119	3	6	12	12229.32	0.04	0.05	N	O	1995-11-18	1995-12-13	1995-11-23	COLLECT COD              	REG AIR   	le furiously along the r
2500	192	3	1	40	43687.60	0.00	0.02	A	F	1992-09-02	1992-09-30	1992-09-06	DELIVER IN PERSON        	SHIP      	efully unusual dolphins s
2500	37	8	2	34	31859.02	0.06	0.02	R	F	1992-10-03	1992-11-11	1992-10-29	DELIVER IN PERSON        	TRUCK     	 stealthy a
2500	80	10	3	41	40183.28	0.02	0.00	R	F	1992-09-02	1992-11-11	1992-09-06	DELIVER IN PERSON        	RAIL      	s could have to integrate after the 
2500	69	8	4	17	16474.02	0.01	0.02	A	F	1992-09-30	1992-10-16	1992-10-05	DELIVER IN PERSON        	REG AIR   	encies-- ironic, even packages
2502	163	4	1	33	35084.28	0.10	0.06	R	F	1993-08-12	1993-07-22	1993-09-04	COLLECT COD              	REG AIR   	have to print
2503	123	2	1	33	33762.96	0.06	0.01	R	F	1993-07-06	1993-08-14	1993-08-02	NONE                     	SHIP      	nal courts integrate according to the
2503	65	10	2	28	27021.68	0.06	0.01	R	F	1993-08-08	1993-08-31	1993-08-10	NONE                     	SHIP      	s wake quickly slyly 
2503	46	7	3	50	47302.00	0.09	0.01	A	F	1993-09-22	1993-08-17	1993-09-29	DELIVER IN PERSON        	TRUCK     	s around the slyly 
2503	91	5	4	27	26759.43	0.09	0.00	A	F	1993-07-12	1993-07-24	1993-07-22	DELIVER IN PERSON        	TRUCK     	lly even p
2503	48	5	5	3	2844.12	0.04	0.02	A	F	1993-07-10	1993-09-17	1993-07-19	TAKE BACK RETURN         	TRUCK     	s cajole. slyly close courts nod f
2503	128	7	6	39	40096.68	0.05	0.05	R	F	1993-10-11	1993-09-09	1993-10-16	NONE                     	MAIL      	d carefully fluffily
2503	19	6	7	17	15623.17	0.09	0.08	R	F	1993-09-04	1993-07-31	1993-09-23	DELIVER IN PERSON        	SHIP      	c accounts haggle blithel
2531	148	7	1	9	9433.26	0.03	0.07	N	O	1996-07-27	1996-07-03	1996-08-01	DELIVER IN PERSON        	AIR       	t the dogged, un
2531	157	2	2	3	3171.45	0.07	0.06	N	O	1996-07-20	1996-06-20	1996-08-10	NONE                     	MAIL      	he quickly ev
2531	86	7	3	20	19721.60	0.06	0.04	N	O	1996-07-18	1996-06-25	1996-07-29	TAKE BACK RETURN         	TRUCK     	into beans. furious
2531	191	5	4	36	39282.84	0.08	0.01	N	O	1996-06-11	1996-07-26	1996-06-27	NONE                     	MAIL      	y ironic, bold packages. blithely e
2531	56	4	5	28	26769.40	0.03	0.07	N	O	1996-07-06	1996-07-31	1996-07-19	TAKE BACK RETURN         	REG AIR   	its. busily
2531	145	4	6	46	48076.44	0.10	0.08	N	O	1996-07-03	1996-06-27	1996-07-12	TAKE BACK RETURN         	REG AIR   	e final, bold pains. ir
2535	199	2	1	5	5495.95	0.06	0.01	A	F	1993-09-07	1993-07-25	1993-09-29	DELIVER IN PERSON        	REG AIR   	, unusual reque
2535	39	5	2	12	11268.36	0.08	0.05	A	F	1993-07-17	1993-08-17	1993-07-31	TAKE BACK RETURN         	FOB       	uses sleep among the packages. excuses 
2535	54	5	3	5	4770.25	0.09	0.06	R	F	1993-07-28	1993-08-14	1993-08-11	DELIVER IN PERSON        	SHIP      	 across the express requests. silent, eve
2535	160	5	4	19	20143.04	0.01	0.02	A	F	1993-06-01	1993-08-01	1993-06-19	DELIVER IN PERSON        	FOB       	ructions. final requests
2535	174	3	5	25	26854.25	0.07	0.04	A	F	1993-07-19	1993-08-07	1993-07-27	NONE                     	REG AIR   	ions believe ab
2561	25	4	1	32	29600.64	0.02	0.01	N	O	1998-01-05	1997-12-28	1998-01-26	DELIVER IN PERSON        	REG AIR   	bold packages wake slyly. slyly
2561	98	1	2	5	4990.45	0.07	0.04	N	O	1997-12-27	1998-01-23	1998-01-13	TAKE BACK RETURN         	AIR       	p ironic, regular pinto beans.
2561	173	4	3	47	50438.99	0.04	0.02	N	O	1997-11-19	1998-01-21	1997-12-03	DELIVER IN PERSON        	REG AIR   	larly pending t
2561	108	9	4	39	39315.90	0.08	0.06	N	O	1998-01-20	1997-12-16	1998-02-05	TAKE BACK RETURN         	MAIL      	equests are furiously against the
2561	150	3	5	2	2100.30	0.04	0.08	N	O	1998-03-14	1998-01-21	1998-03-27	DELIVER IN PERSON        	TRUCK     	s are. silently silent foxes sleep about
2561	51	6	6	14	13314.70	0.02	0.03	N	O	1998-03-07	1998-02-04	1998-03-21	COLLECT COD              	RAIL      	ep unusual, ironic accounts
2564	112	3	1	4	4048.44	0.02	0.00	R	F	1994-11-12	1994-10-29	1994-12-04	NONE                     	MAIL      	y express requests sleep furi
2567	26	9	1	39	36114.78	0.03	0.04	N	O	1998-05-10	1998-05-10	1998-05-21	NONE                     	SHIP      	ns. furiously final dependencies cajo
2567	112	3	2	50	50605.50	0.06	0.05	N	O	1998-05-05	1998-04-18	1998-05-09	DELIVER IN PERSON        	TRUCK     	. carefully pending foxes are furi
2567	52	10	3	6	5712.30	0.03	0.06	N	O	1998-04-21	1998-04-14	1998-05-11	NONE                     	RAIL      	s cajole regular, final acco
2567	158	6	4	50	52907.50	0.05	0.03	N	O	1998-03-27	1998-05-25	1998-04-23	DELIVER IN PERSON        	FOB       	pinto beans? r
2567	81	2	5	46	45129.68	0.07	0.02	N	O	1998-06-02	1998-04-30	1998-06-13	COLLECT COD              	AIR       	efully pending epitaphs. carefully reg
2567	100	3	6	32	32003.20	0.01	0.07	N	O	1998-05-24	1998-04-30	1998-06-14	NONE                     	RAIL      	 the even, iro
2567	135	6	7	43	44510.59	0.06	0.02	N	O	1998-05-11	1998-04-15	1998-05-29	NONE                     	RAIL      	requests. final courts cajole 
2594	72	3	1	7	6804.49	0.06	0.02	R	F	1993-03-26	1993-03-05	1993-04-24	DELIVER IN PERSON        	FOB       	arls cajole 
2594	124	9	2	13	13313.56	0.10	0.05	R	F	1993-02-06	1993-03-01	1993-02-23	TAKE BACK RETURN         	TRUCK     	fully special accounts use courts
2594	126	1	3	24	24626.88	0.03	0.00	A	F	1993-01-31	1993-03-10	1993-02-04	COLLECT COD              	REG AIR   	lar accounts sleep fur
2594	144	7	4	46	48030.44	0.00	0.08	R	F	1993-04-17	1993-03-06	1993-04-21	TAKE BACK RETURN         	SHIP      	beans. instructions across t
2596	170	5	1	6	6421.02	0.05	0.01	N	O	1996-12-15	1996-11-02	1996-12-29	TAKE BACK RETURN         	TRUCK     	ily special re
2596	139	10	2	43	44682.59	0.07	0.03	N	O	1996-09-03	1996-10-26	1996-09-15	NONE                     	FOB       	ial packages haggl
2596	39	5	3	19	17841.57	0.10	0.00	N	O	1996-09-02	1996-11-03	1996-09-06	COLLECT COD              	AIR       	ias mold! sp
2596	105	6	4	10	10051.00	0.06	0.05	N	O	1996-08-25	1996-11-05	1996-09-13	DELIVER IN PERSON        	REG AIR   	 instructions shall have
2598	7	4	1	12	10884.00	0.00	0.01	N	O	1996-06-17	1996-04-12	1996-06-24	COLLECT COD              	TRUCK     	express packages nag sly
2598	148	7	2	40	41925.60	0.07	0.02	N	O	1996-05-11	1996-05-19	1996-06-08	TAKE BACK RETURN         	AIR       	the enticing
2598	104	9	3	4	4016.40	0.03	0.03	N	O	1996-05-23	1996-05-13	1996-05-25	COLLECT COD              	AIR       	 across the furiously fi
2598	23	2	4	19	17537.38	0.02	0.00	N	O	1996-04-09	1996-05-30	1996-04-17	TAKE BACK RETURN         	RAIL      	nic packages. even accounts
2598	106	3	5	12	12073.20	0.01	0.08	N	O	1996-04-14	1996-04-24	1996-04-21	TAKE BACK RETURN         	REG AIR   	eposits cajol
2599	101	4	1	11	11012.10	0.08	0.08	N	O	1997-02-01	1996-12-14	1997-02-27	TAKE BACK RETURN         	FOB       	 express accoun
2599	42	5	2	26	24493.04	0.03	0.04	N	O	1996-11-08	1996-12-21	1996-11-24	TAKE BACK RETURN         	AIR       	nag carefully 
2599	99	10	3	29	28973.61	0.09	0.03	N	O	1997-01-10	1996-12-10	1997-02-02	COLLECT COD              	RAIL      	ly express dolphins. special, 
2627	131	7	1	28	28871.64	0.09	0.02	R	F	1992-05-14	1992-05-09	1992-05-31	COLLECT COD              	SHIP      	ggedly final excuses nag packages. f
2656	181	2	1	10	10811.80	0.02	0.06	R	F	1993-06-28	1993-07-04	1993-07-12	TAKE BACK RETURN         	TRUCK     	s nag regularly about the deposits. slyly
2656	137	8	2	38	39410.94	0.07	0.02	A	F	1993-06-25	1993-06-04	1993-07-24	NONE                     	RAIL      	structions wake along the furio
2656	2	5	3	19	17138.00	0.03	0.02	R	F	1993-08-03	1993-07-25	1993-08-20	TAKE BACK RETURN         	MAIL      	ts serve deposi
2656	110	3	4	40	40404.40	0.05	0.04	R	F	1993-06-09	1993-07-24	1993-06-21	DELIVER IN PERSON        	RAIL      	refully final pearls. final ideas wake. qu
2659	42	1	1	28	26377.12	0.08	0.05	A	F	1994-03-17	1994-01-24	1994-03-19	NONE                     	FOB       	idle tithes
2659	43	2	2	21	19803.84	0.00	0.00	A	F	1993-12-23	1994-02-10	1994-01-17	DELIVER IN PERSON        	RAIL      	y beyond the furiously even co
2659	135	1	3	24	24843.12	0.04	0.03	R	F	1994-03-28	1994-02-20	1994-04-05	DELIVER IN PERSON        	REG AIR   	 haggle carefully 
2659	119	6	4	2	2038.22	0.00	0.08	R	F	1994-02-19	1994-03-12	1994-02-21	NONE                     	MAIL      	sts above the fluffily express fo
2659	7	4	5	9	8163.00	0.08	0.03	A	F	1994-02-07	1994-03-17	1994-03-04	DELIVER IN PERSON        	AIR       	ly final packages sleep ac
2660	48	7	1	17	16116.68	0.00	0.05	N	O	1995-08-18	1995-09-13	1995-09-17	NONE                     	SHIP      	al pinto beans wake after the furious
2663	114	4	1	35	35493.85	0.02	0.01	N	O	1995-12-11	1995-10-16	1996-01-07	TAKE BACK RETURN         	REG AIR   	tect. slyly fina
2691	91	3	1	11	10901.99	0.04	0.07	R	F	1992-06-21	1992-06-08	1992-07-09	COLLECT COD              	FOB       	leep alongside of the accounts. slyly ironi
2691	48	7	2	2	1896.08	0.00	0.07	R	F	1992-05-10	1992-06-04	1992-05-11	TAKE BACK RETURN         	TRUCK     	s cajole at the blithely ironic warthog
2691	162	3	3	16	16994.56	0.09	0.03	R	F	1992-06-11	1992-07-29	1992-06-29	NONE                     	RAIL      	bove the even foxes. unusual theodoli
2691	166	3	4	1	1066.16	0.08	0.00	A	F	1992-08-11	1992-06-07	1992-08-16	NONE                     	SHIP      	egular instructions b
2693	9	10	1	26	23634.00	0.04	0.00	N	O	1996-09-14	1996-10-07	1996-10-03	COLLECT COD              	MAIL      	cajole alo
2693	102	3	2	43	43090.30	0.03	0.04	N	O	1996-10-24	1996-10-24	1996-11-03	TAKE BACK RETURN         	TRUCK     	as are according to th
2694	153	1	1	30	31594.50	0.02	0.06	N	O	1996-06-20	1996-06-01	1996-07-15	NONE                     	TRUCK     	oxes. never iro
2694	157	2	2	35	37000.25	0.07	0.03	N	O	1996-05-24	1996-06-01	1996-05-25	NONE                     	RAIL      	atelets past the furiously final deposits 
2694	19	3	3	15	13785.15	0.08	0.02	N	O	1996-06-30	1996-05-01	1996-07-25	TAKE BACK RETURN         	REG AIR   	e blithely even platelets. special wa
2694	20	10	4	12	11040.24	0.00	0.05	N	O	1996-04-24	1996-04-22	1996-05-14	DELIVER IN PERSON        	RAIL      	foxes atop the hockey pla
2694	108	9	5	10	10081.00	0.08	0.08	N	O	1996-06-23	1996-05-28	1996-06-27	COLLECT COD              	REG AIR   	fluffily fluffy accounts. even packages hi
2695	184	5	1	21	22767.78	0.07	0.00	N	O	1996-10-04	1996-11-02	1996-10-21	NONE                     	MAIL      	y regular pinto beans. evenly regular packa
2695	19	9	2	44	40436.44	0.09	0.07	N	O	1996-10-05	1996-10-10	1996-11-01	NONE                     	MAIL      	ts. busy platelets boost
2695	144	7	3	21	21926.94	0.02	0.07	N	O	1996-09-13	1996-09-25	1996-10-13	NONE                     	TRUCK     	s. furiously ironic platelets ar
2695	58	6	4	16	15328.80	0.08	0.08	N	O	1996-11-16	1996-10-05	1996-11-22	NONE                     	TRUCK     	its. theodolites sleep slyly
2695	86	7	5	40	39443.20	0.02	0.03	N	O	1996-11-02	1996-10-26	1996-11-14	NONE                     	FOB       	ructions. pending
2720	45	6	1	5	4725.20	0.10	0.06	A	F	1993-06-24	1993-08-08	1993-07-08	NONE                     	FOB       	ously ironic foxes thrash
2720	17	8	2	42	38514.42	0.09	0.03	R	F	1993-07-25	1993-07-23	1993-08-23	COLLECT COD              	REG AIR   	fter the inst
2720	120	1	3	50	51006.00	0.10	0.02	A	F	1993-08-10	1993-07-29	1993-09-06	NONE                     	SHIP      	l requests. deposits nag furiously
2720	109	2	4	49	49445.90	0.06	0.02	A	F	1993-07-09	1993-07-14	1993-07-13	NONE                     	REG AIR   	 accounts. fluffily bold pack
2720	121	6	5	27	27570.24	0.04	0.00	R	F	1993-06-29	1993-08-06	1993-07-28	NONE                     	TRUCK     	eas. carefully regular 
2721	183	4	1	49	53075.82	0.00	0.08	N	O	1996-02-14	1996-04-26	1996-03-02	DELIVER IN PERSON        	AIR       	ounts poach carefu
2721	3	4	2	2	1806.00	0.02	0.05	N	O	1996-02-13	1996-03-14	1996-02-28	TAKE BACK RETURN         	TRUCK     	 slyly final requests against 
2722	124	7	1	21	21506.52	0.09	0.01	A	F	1994-07-29	1994-06-26	1994-08-09	NONE                     	RAIL      	e carefully around the furiously ironic pac
2722	146	7	2	15	15692.10	0.05	0.03	R	F	1994-07-02	1994-06-01	1994-07-13	COLLECT COD              	AIR       	refully final asympt
2722	34	10	3	16	14944.48	0.04	0.06	R	F	1994-05-25	1994-06-09	1994-05-26	NONE                     	MAIL      	ts besides the fluffy,
2723	13	7	1	47	42911.47	0.09	0.07	N	O	1995-12-05	1995-11-19	1995-12-11	TAKE BACK RETURN         	AIR       	furiously r
2723	32	3	2	10	9320.30	0.06	0.08	N	O	1995-11-27	1995-11-29	1995-12-12	DELIVER IN PERSON        	MAIL      	al, special r
2723	162	1	3	2	2124.32	0.10	0.01	N	O	1995-11-09	1995-11-10	1995-11-14	TAKE BACK RETURN         	FOB       	 courts boost quickly about th
2723	82	3	4	12	11784.96	0.01	0.05	N	O	1995-12-24	1995-11-15	1996-01-17	DELIVER IN PERSON        	RAIL      	bold foxes are bold packages. regular, fin
2723	129	10	5	40	41164.80	0.09	0.05	N	O	1995-11-17	1995-11-22	1995-11-18	TAKE BACK RETURN         	MAIL      	unwind fluffily carefully regular realms.
2724	92	4	1	47	46628.23	0.09	0.01	A	F	1994-11-23	1994-11-13	1994-12-03	COLLECT COD              	TRUCK     	unusual patterns nag. special p
2724	147	8	2	21	21989.94	0.09	0.02	A	F	1994-11-25	1994-10-15	1994-12-07	COLLECT COD              	RAIL      	as. carefully regular dependencies wak
2724	50	3	3	22	20901.10	0.04	0.06	A	F	1994-09-19	1994-11-18	1994-10-17	TAKE BACK RETURN         	TRUCK     	express fo
2724	35	6	4	1	935.03	0.07	0.03	A	F	1994-12-26	1994-11-27	1995-01-07	NONE                     	MAIL      	lyly carefully blithe theodolites-- pl
2724	149	2	5	29	30425.06	0.05	0.06	A	F	1995-01-10	1994-11-17	1995-02-04	COLLECT COD              	MAIL      	l requests hagg
2725	118	2	1	23	23416.53	0.10	0.08	R	F	1994-08-25	1994-06-22	1994-08-28	TAKE BACK RETURN         	REG AIR   	y regular deposits. brave foxes 
2725	5	8	2	41	37105.00	0.01	0.00	R	F	1994-07-05	1994-06-29	1994-08-02	DELIVER IN PERSON        	TRUCK     	ns sleep furiously c
2725	189	10	3	15	16337.70	0.07	0.03	R	F	1994-08-06	1994-08-09	1994-08-15	TAKE BACK RETURN         	AIR       	? furiously regular a
2726	1	6	1	50	45050.00	0.00	0.06	R	F	1993-03-04	1993-01-29	1993-03-28	COLLECT COD              	TRUCK     	 furiously bold theodolites
2754	149	6	1	4	4196.56	0.05	0.08	A	F	1994-07-13	1994-05-15	1994-08-02	NONE                     	REG AIR   	blithely silent requests. regular depo
2754	177	5	2	19	20466.23	0.01	0.07	A	F	1994-06-27	1994-05-06	1994-06-28	NONE                     	FOB       	latelets hag
2755	92	4	1	19	18849.71	0.10	0.00	R	F	1992-02-11	1992-03-15	1992-02-14	TAKE BACK RETURN         	MAIL      	furiously special deposits
2755	24	3	2	11	10164.22	0.03	0.08	A	F	1992-04-12	1992-05-07	1992-04-21	COLLECT COD              	RAIL      	egular excuses sleep carefully.
2755	64	3	3	21	20245.26	0.08	0.04	R	F	1992-02-13	1992-04-20	1992-03-02	NONE                     	AIR       	furious re
2755	131	7	4	5	5155.65	0.01	0.00	A	F	1992-02-27	1992-04-07	1992-03-09	TAKE BACK RETURN         	AIR       	e the furi
2755	116	7	5	48	48773.28	0.05	0.06	R	F	1992-03-22	1992-03-10	1992-04-14	DELIVER IN PERSON        	MAIL      	yly even epitaphs for the 
2756	118	9	1	35	35633.85	0.03	0.02	R	F	1994-06-08	1994-06-01	1994-06-21	TAKE BACK RETURN         	AIR       	 deposits grow bold sheaves; iro
2756	80	9	2	47	46063.76	0.06	0.01	R	F	1994-05-10	1994-05-25	1994-05-13	NONE                     	AIR       	e final, f
2756	105	8	3	31	31158.10	0.01	0.07	A	F	1994-07-27	1994-07-06	1994-08-22	TAKE BACK RETURN         	TRUCK     	en instructions use quickly.
2756	72	2	4	30	29162.10	0.00	0.04	A	F	1994-06-05	1994-06-30	1994-06-14	DELIVER IN PERSON        	TRUCK     	ular packages. regular deposi
2758	121	10	1	20	20422.40	0.02	0.04	N	O	1998-07-27	1998-09-10	1998-08-21	TAKE BACK RETURN         	AIR       	ptotes sleep furiously
2758	23	8	2	17	15691.34	0.10	0.06	N	O	1998-09-25	1998-10-03	1998-10-25	NONE                     	MAIL      	 accounts! qui
2758	26	5	3	1	926.02	0.06	0.02	N	O	1998-10-09	1998-09-15	1998-10-16	NONE                     	TRUCK     	ake furious
2759	59	1	1	10	9590.50	0.10	0.03	R	F	1993-12-14	1994-01-08	1994-01-01	COLLECT COD              	FOB       	s. busily ironic theodo
2759	113	10	2	37	37485.07	0.00	0.06	R	F	1994-03-05	1994-02-22	1994-03-18	DELIVER IN PERSON        	REG AIR   	lar Tiresias affix ironically carefully sp
2759	112	9	3	11	11133.21	0.03	0.08	A	F	1994-01-24	1994-01-16	1994-02-21	DELIVER IN PERSON        	TRUCK     	hely regular 
2759	23	2	4	31	28613.62	0.02	0.05	A	F	1994-01-11	1994-01-15	1994-01-23	NONE                     	SHIP      	ithely aft
2784	33	4	1	45	41986.35	0.03	0.01	N	O	1998-02-15	1998-04-07	1998-02-26	COLLECT COD              	AIR       	yly along the asymptotes. reque
2784	54	5	2	23	21943.15	0.03	0.05	N	O	1998-03-28	1998-02-07	1998-04-17	DELIVER IN PERSON        	AIR       	uests lose after 
2784	175	4	3	40	43006.80	0.07	0.01	N	O	1998-04-28	1998-03-19	1998-05-03	DELIVER IN PERSON        	TRUCK     	deas nag furiously never unusual 
2784	29	10	4	3	2787.06	0.04	0.03	N	O	1998-01-19	1998-04-05	1998-02-05	TAKE BACK RETURN         	AIR       	n packages. foxes haggle quickly sile
2786	136	2	1	15	15541.95	0.03	0.04	A	F	1992-05-19	1992-05-08	1992-05-28	COLLECT COD              	TRUCK     	low deposits are ironic
2786	51	3	2	42	39944.10	0.10	0.04	R	F	1992-05-15	1992-04-22	1992-05-30	DELIVER IN PERSON        	AIR       	unts are against the furious
2786	156	1	3	41	43302.15	0.04	0.05	R	F	1992-07-01	1992-06-04	1992-07-13	COLLECT COD              	RAIL      	ix requests. bold requests a
2786	23	4	4	24	22152.48	0.05	0.02	A	F	1992-04-04	1992-06-09	1992-05-02	DELIVER IN PERSON        	MAIL      	ans. slyly unusual platelets detect. unus
2786	50	3	5	43	40852.15	0.06	0.03	R	F	1992-04-22	1992-05-13	1992-04-29	NONE                     	RAIL      	ons. theodolites after
2786	162	1	6	21	22305.36	0.08	0.00	A	F	1992-05-03	1992-05-01	1992-05-14	COLLECT COD              	AIR       	slow instructi
2788	177	8	1	16	17234.72	0.06	0.06	A	F	1994-10-04	1994-11-25	1994-10-18	DELIVER IN PERSON        	AIR       	 requests wake carefully. carefully si
2791	59	10	1	49	46993.45	0.10	0.04	A	F	1995-01-11	1994-11-10	1995-02-08	COLLECT COD              	MAIL      	 accounts sleep at the bold, regular pinto 
2791	63	4	2	4	3852.24	0.10	0.08	A	F	1995-01-02	1994-12-28	1995-01-29	NONE                     	SHIP      	slyly bold packages boost. slyly
2791	133	9	3	44	45457.72	0.08	0.06	R	F	1994-11-17	1994-11-12	1994-12-14	NONE                     	FOB       	heodolites use furio
2791	156	8	4	24	25347.60	0.04	0.02	R	F	1995-01-30	1994-11-20	1995-02-08	DELIVER IN PERSON        	TRUCK     	ilent forges. quickly special pinto beans 
2791	105	2	5	8	8040.80	0.02	0.04	R	F	1995-01-30	1994-11-24	1995-02-13	NONE                     	FOB       	se. close ideas alongs
2791	75	3	6	9	8775.63	0.08	0.02	R	F	1994-11-19	1994-12-14	1994-12-10	TAKE BACK RETURN         	AIR       	pendencies. blithely bold patterns acr
2791	29	2	7	26	24154.52	0.06	0.03	R	F	1995-02-06	1994-12-07	1995-02-23	DELIVER IN PERSON        	AIR       	uriously special instructio
2816	59	10	1	33	31648.65	0.00	0.07	R	F	1994-10-19	1994-11-10	1994-11-09	NONE                     	REG AIR   	s; slyly even theodo
2816	142	3	2	4	4168.56	0.05	0.04	R	F	1994-12-11	1994-12-07	1995-01-03	NONE                     	FOB       	. blithely pending id
2816	121	6	3	4	4084.48	0.02	0.06	R	F	1994-12-12	1994-12-05	1994-12-30	NONE                     	RAIL      	 requests print above the final deposits
2820	174	2	1	23	24705.91	0.04	0.08	R	F	1994-07-10	1994-08-08	1994-07-21	NONE                     	MAIL      	 was furiously. deposits among the ironic
2820	126	9	2	33	33861.96	0.08	0.06	A	F	1994-07-07	1994-08-17	1994-08-02	DELIVER IN PERSON        	AIR       	carefully even pinto beans. 
2820	141	10	3	38	39563.32	0.03	0.08	A	F	1994-09-10	1994-08-07	1994-10-07	TAKE BACK RETURN         	MAIL      	ests despite the carefully unusual a
2820	197	9	4	40	43887.60	0.06	0.06	A	F	1994-08-08	1994-07-30	1994-08-21	TAKE BACK RETURN         	REG AIR   	g multipliers. final c
2821	181	2	1	4	4324.72	0.00	0.00	A	F	1993-09-15	1993-10-02	1993-09-17	TAKE BACK RETURN         	TRUCK     	nding foxes.
2821	72	1	2	4	3888.28	0.09	0.00	A	F	1993-11-19	1993-09-20	1993-11-27	TAKE BACK RETURN         	TRUCK     	ual multipliers. final deposits cajol
2821	164	1	3	27	28732.32	0.01	0.01	A	F	1993-11-27	1993-10-11	1993-12-08	COLLECT COD              	TRUCK     	requests. blit
2822	151	9	1	39	40994.85	0.04	0.02	R	F	1993-09-11	1993-08-29	1993-09-18	NONE                     	MAIL      	kly about the sly
2823	86	7	1	45	44373.60	0.03	0.04	N	O	1995-12-28	1995-11-27	1996-01-02	DELIVER IN PERSON        	SHIP      	furiously special idea
2823	160	5	2	18	19082.88	0.00	0.03	N	O	1995-11-11	1995-10-30	1995-12-08	TAKE BACK RETURN         	TRUCK     	 final deposits. furiously regular foxes u
2823	186	7	3	11	11947.98	0.07	0.02	N	O	1995-12-10	1995-11-24	1995-12-21	DELIVER IN PERSON        	SHIP      	bold requests nag blithely s
2823	139	10	4	48	49878.24	0.09	0.03	N	O	1995-11-21	1995-10-30	1995-11-27	NONE                     	SHIP      	ously busily slow excus
2823	99	2	5	18	17983.62	0.04	0.06	N	O	1995-11-09	1995-10-30	1995-11-19	NONE                     	AIR       	eas. decoys cajole deposi
2823	123	2	6	20	20462.40	0.07	0.00	N	O	1995-11-13	1995-12-06	1995-12-07	NONE                     	MAIL      	its sleep between the unusual, ironic pac
2823	86	7	7	12	11832.96	0.02	0.04	N	O	1995-12-22	1995-11-20	1996-01-13	NONE                     	REG AIR   	the slyly ironic dolphins; fin
2848	65	4	1	44	42462.64	0.01	0.05	R	F	1992-04-14	1992-05-09	1992-04-19	DELIVER IN PERSON        	MAIL      	ions. slyly express instructions n
2848	165	6	2	8	8521.28	0.07	0.01	A	F	1992-03-21	1992-05-18	1992-04-07	DELIVER IN PERSON        	TRUCK     	. silent, final ideas sublate packages. ir
2848	138	4	3	8	8305.04	0.07	0.08	A	F	1992-06-20	1992-04-12	1992-07-09	NONE                     	SHIP      	sly regular foxes. 
2848	125	6	4	34	34854.08	0.02	0.08	A	F	1992-03-15	1992-04-24	1992-04-12	TAKE BACK RETURN         	RAIL      	ts along the blithely regu
2848	195	7	5	18	19713.42	0.07	0.03	R	F	1992-04-10	1992-06-01	1992-05-05	DELIVER IN PERSON        	TRUCK     	osits haggle. stealthily ironic packa
2850	97	1	1	43	42874.87	0.02	0.05	N	O	1997-01-11	1996-11-03	1997-02-01	COLLECT COD              	REG AIR   	unusual accounts
2850	110	7	2	30	30303.30	0.09	0.01	N	O	1996-12-14	1996-11-29	1997-01-03	COLLECT COD              	AIR       	even ideas. busy pinto beans sleep above t
2850	105	6	3	49	49249.90	0.09	0.04	N	O	1996-10-07	1996-12-12	1996-10-12	TAKE BACK RETURN         	MAIL      	 slyly unusual req
2850	199	3	4	4	4396.76	0.04	0.04	N	O	1996-10-28	1996-12-26	1996-11-07	COLLECT COD              	RAIL      	al deposits cajole carefully quickly 
2851	148	5	1	8	8385.12	0.09	0.03	N	O	1997-11-12	1997-11-22	1997-12-11	NONE                     	REG AIR   	y special theodolites. carefully
2880	35	6	1	40	37401.20	0.09	0.00	A	F	1992-05-26	1992-06-01	1992-05-31	COLLECT COD              	TRUCK     	even requests. quick
2880	139	5	2	26	27017.38	0.07	0.07	R	F	1992-04-12	1992-04-15	1992-04-28	NONE                     	RAIL      	ully among the regular warthogs
2880	115	9	3	42	42634.62	0.01	0.01	R	F	1992-06-17	1992-05-29	1992-07-11	NONE                     	REG AIR   	ions. carefully final accounts are unusual,
2880	18	2	4	46	42228.46	0.02	0.02	A	F	1992-04-21	1992-06-05	1992-05-16	COLLECT COD              	RAIL      	eep quickly according to t
2881	180	10	1	16	17282.88	0.02	0.06	A	F	1992-06-21	1992-06-27	1992-07-03	TAKE BACK RETURN         	TRUCK     	usly bold 
2881	10	1	2	1	910.01	0.09	0.03	A	F	1992-05-13	1992-07-21	1992-05-18	COLLECT COD              	MAIL      	final theodolites. quickly
2881	93	6	3	21	20854.89	0.07	0.03	A	F	1992-05-28	1992-07-03	1992-06-02	TAKE BACK RETURN         	SHIP      	hely express Tiresias. final dependencies 
2881	140	6	4	7	7280.98	0.06	0.01	R	F	1992-08-03	1992-07-10	1992-08-27	NONE                     	REG AIR   	ironic packages are carefully final ac
2882	4	7	1	14	12656.00	0.09	0.02	N	O	1995-09-28	1995-11-11	1995-10-18	TAKE BACK RETURN         	MAIL      	kly. even requests w
2882	42	1	2	30	28261.20	0.00	0.00	N	O	1995-10-15	1995-10-13	1995-10-25	NONE                     	REG AIR   	among the furiously even theodolites. regu
2882	197	9	3	29	31818.51	0.10	0.08	N	O	1995-09-10	1995-11-01	1995-10-02	NONE                     	TRUCK     	kages. furiously ironic
2882	78	6	4	27	26407.89	0.06	0.02	N	O	1995-09-04	1995-11-11	1995-09-12	DELIVER IN PERSON        	MAIL      	rding to the regu
2882	134	5	5	32	33092.16	0.07	0.03	N	O	1995-10-21	1995-11-10	1995-11-01	COLLECT COD              	RAIL      	sts. quickly regular e
2882	87	8	6	47	46392.76	0.06	0.03	N	O	1995-09-13	1995-09-21	1995-09-14	NONE                     	REG AIR   	l, special
2883	1	4	1	33	29733.00	0.08	0.07	R	F	1995-02-26	1995-03-04	1995-03-01	NONE                     	RAIL      	s. final i
2883	125	6	2	27	27678.24	0.00	0.02	A	F	1995-03-12	1995-03-10	1995-04-04	TAKE BACK RETURN         	REG AIR   	s. brave pinto beans nag furiously
2883	189	10	3	47	51191.46	0.05	0.04	R	F	1995-01-29	1995-04-19	1995-02-05	DELIVER IN PERSON        	SHIP      	ep carefully ironic
2883	98	2	4	23	22956.07	0.00	0.02	R	F	1995-02-03	1995-03-17	1995-02-19	TAKE BACK RETURN         	AIR       	 even requests cajole. special, regular 
2883	195	8	5	36	39426.84	0.07	0.06	A	F	1995-05-02	1995-03-14	1995-05-30	COLLECT COD              	MAIL      	ests detect slyly special packages
2885	4	9	1	6	5424.00	0.10	0.01	A	F	1993-01-05	1992-12-12	1993-01-19	COLLECT COD              	FOB       	ctions solve. slyly regular requests n
2885	112	3	2	4	4048.44	0.07	0.00	A	F	1992-10-09	1992-12-17	1992-11-04	TAKE BACK RETURN         	SHIP      	 pending packages wake. 
2885	1	6	3	45	40545.00	0.10	0.04	A	F	1992-12-24	1992-10-30	1993-01-04	NONE                     	SHIP      	ess ideas. regular, silen
2885	32	3	4	15	13980.45	0.03	0.04	R	F	1992-10-31	1992-11-24	1992-11-21	DELIVER IN PERSON        	MAIL      	odolites. boldly pending packages han
2885	175	5	5	43	46232.31	0.06	0.00	R	F	1992-11-17	1992-10-30	1992-12-04	DELIVER IN PERSON        	SHIP      	cial deposits use bold
2885	190	1	6	5	5450.95	0.01	0.02	R	F	1993-01-06	1992-11-13	1993-02-05	TAKE BACK RETURN         	TRUCK     	s. slyly express th
2885	50	9	7	40	38002.00	0.05	0.03	A	F	1992-09-23	1992-11-15	1992-10-07	TAKE BACK RETURN         	AIR       	 express depos
2886	60	1	1	1	960.06	0.09	0.05	A	F	1995-02-01	1994-12-18	1995-02-28	COLLECT COD              	REG AIR   	eposits fr
2886	184	5	2	38	41198.84	0.02	0.04	A	F	1995-01-21	1995-01-08	1995-01-30	NONE                     	SHIP      	old requests along the fur
2886	63	8	3	2	1926.12	0.04	0.07	A	F	1994-11-18	1995-01-31	1994-12-05	COLLECT COD              	REG AIR   	ar theodolites. e
3013	72	2	6	19	18469.33	0.08	0.07	N	O	1997-05-11	1997-04-18	1997-05-15	COLLECT COD              	REG AIR   	fluffily pending packages nag furiously al
2886	130	3	4	46	47385.98	0.03	0.08	A	F	1995-02-02	1995-01-26	1995-02-15	TAKE BACK RETURN         	SHIP      	ously final packages sleep blithely regular
2887	66	3	1	11	10626.66	0.06	0.00	N	O	1997-07-08	1997-07-17	1997-07-15	COLLECT COD              	SHIP      	ackages. unusual, speci
2887	112	6	2	17	17205.87	0.00	0.08	N	O	1997-08-31	1997-07-04	1997-09-17	DELIVER IN PERSON        	SHIP      	fily final packages. regula
2913	123	6	1	39	39901.68	0.06	0.04	N	O	1997-08-28	1997-09-27	1997-09-02	TAKE BACK RETURN         	AIR       	. final packages a
2913	22	5	2	22	20284.44	0.10	0.07	N	O	1997-09-18	1997-08-11	1997-10-02	COLLECT COD              	MAIL      	riously pending realms. blithely even pac
2913	166	1	3	17	18124.72	0.07	0.04	N	O	1997-10-21	1997-09-25	1997-11-20	NONE                     	FOB       	requests doze quickly. furious
2913	143	4	4	5	5215.70	0.10	0.07	N	O	1997-10-07	1997-08-25	1997-10-09	TAKE BACK RETURN         	RAIL      	haggle. even, bold instructi
2913	15	9	5	13	11895.13	0.03	0.01	N	O	1997-10-02	1997-08-20	1997-10-26	COLLECT COD              	MAIL      	inos are carefully alongside of the bol
2913	168	5	6	35	37385.60	0.06	0.08	N	O	1997-08-30	1997-08-21	1997-09-03	COLLECT COD              	MAIL      	es. quickly even braids against
2914	66	7	1	22	21253.32	0.05	0.06	R	F	1993-05-11	1993-04-09	1993-05-22	DELIVER IN PERSON        	FOB       	 carefully about the fluffily ironic gifts
2914	163	10	2	25	26579.00	0.03	0.04	A	F	1993-05-14	1993-04-04	1993-05-22	NONE                     	SHIP      	cross the carefully even accounts.
2914	35	1	3	4	3740.12	0.00	0.05	R	F	1993-06-11	1993-04-09	1993-06-14	TAKE BACK RETURN         	SHIP      	s integrate. bold deposits sleep req
2914	121	2	4	9	9190.08	0.06	0.01	R	F	1993-06-17	1993-05-26	1993-06-19	NONE                     	REG AIR   	s. carefully final foxes ar
2916	83	4	1	21	20644.68	0.06	0.04	N	O	1996-03-11	1996-02-21	1996-03-30	NONE                     	REG AIR   	uickly express ideas over the slyly even 
2918	78	7	1	24	23473.68	0.10	0.03	N	O	1996-12-20	1996-10-28	1996-12-26	DELIVER IN PERSON        	FOB       	 quickly. express requests haggle careful
2919	102	5	1	2	2004.20	0.03	0.05	R	F	1993-12-28	1994-02-23	1994-01-18	COLLECT COD              	TRUCK     	re slyly. regular ideas detect furiousl
2919	121	4	2	49	50034.88	0.07	0.02	R	F	1993-12-16	1994-02-28	1993-12-19	COLLECT COD              	FOB       	hely final inst
2919	46	5	3	44	41625.76	0.07	0.07	A	F	1994-04-01	1994-01-12	1994-04-07	TAKE BACK RETURN         	TRUCK     	final ideas haggle carefully fluff
2919	102	5	4	44	44092.40	0.00	0.05	R	F	1994-02-04	1994-02-03	1994-03-02	TAKE BACK RETURN         	AIR       	es doze around the furiously 
2946	10	5	1	25	22750.25	0.05	0.02	N	O	1996-05-06	1996-04-23	1996-05-16	DELIVER IN PERSON        	SHIP      	ic deposits. furiously
2946	94	5	2	48	47716.32	0.03	0.07	N	O	1996-06-02	1996-03-31	1996-06-16	COLLECT COD              	TRUCK     	oss the platelets. furi
2946	3	6	3	35	31605.00	0.03	0.00	N	O	1996-03-15	1996-04-02	1996-03-26	NONE                     	REG AIR   	 sublate along the fluffily iron
2947	10	1	1	37	33670.37	0.09	0.07	N	O	1995-08-09	1995-07-05	1995-08-20	DELIVER IN PERSON        	RAIL      	e accounts: expres
2947	186	7	2	10	10861.80	0.09	0.07	A	F	1995-06-07	1995-06-26	1995-06-08	NONE                     	MAIL      	lly special 
2948	118	9	1	48	48869.28	0.00	0.04	R	F	1994-08-29	1994-10-23	1994-09-23	NONE                     	TRUCK     	unusual excuses use about the 
2948	92	3	2	49	48612.41	0.04	0.07	R	F	1994-12-16	1994-11-08	1995-01-07	DELIVER IN PERSON        	MAIL      	ress requests. furiously blithe foxes 
2949	21	6	1	4	3684.08	0.06	0.06	A	F	1994-06-07	1994-06-17	1994-07-04	TAKE BACK RETURN         	REG AIR   	gular pinto beans wake alongside of the reg
2949	70	5	2	50	48503.50	0.05	0.04	A	F	1994-08-04	1994-06-23	1994-08-17	TAKE BACK RETURN         	FOB       	gular courts cajole across t
2949	180	9	3	38	41046.84	0.02	0.06	R	F	1994-05-22	1994-05-25	1994-05-27	COLLECT COD              	REG AIR   	se slyly requests. carefull
2978	90	1	1	29	28712.61	0.00	0.08	A	F	1995-06-03	1995-07-25	1995-06-06	NONE                     	SHIP      	ecial ideas promise slyly
2978	127	2	2	42	43139.04	0.01	0.06	N	O	1995-08-19	1995-07-18	1995-09-07	DELIVER IN PERSON        	MAIL      	ial requests nag blithely alongside of th
2978	43	2	3	26	24519.04	0.07	0.05	N	O	1995-07-29	1995-07-22	1995-08-20	COLLECT COD              	REG AIR   	as haggle against the carefully express dep
2978	28	1	4	7	6496.14	0.00	0.00	N	O	1995-07-18	1995-07-03	1995-07-23	NONE                     	FOB       	. final ideas are blithe
2978	29	2	5	33	30657.66	0.09	0.03	R	F	1995-05-06	1995-07-23	1995-05-16	COLLECT COD              	FOB       	s. blithely unusual pack
2978	168	7	6	4	4272.64	0.08	0.04	N	O	1995-07-06	1995-07-31	1995-07-19	COLLECT COD              	AIR       	ffily unusual 
2979	9	6	1	8	7272.00	0.00	0.08	N	O	1996-06-18	1996-05-21	1996-07-06	COLLECT COD              	REG AIR   	st blithely; blithely regular gifts dazz
2979	11	2	2	47	42817.47	0.05	0.00	N	O	1996-03-25	1996-05-13	1996-04-04	TAKE BACK RETURN         	SHIP      	iously unusual dependencies wake across
2979	188	9	3	35	38086.30	0.04	0.03	N	O	1996-05-25	1996-06-11	1996-06-24	DELIVER IN PERSON        	MAIL      	old ideas beneath the blit
2979	165	4	4	28	29824.48	0.05	0.08	N	O	1996-06-04	1996-04-23	1996-06-24	DELIVER IN PERSON        	FOB       	ing, regular pinto beans. blithel
3010	138	4	1	23	23876.99	0.04	0.00	N	O	1996-03-08	1996-02-29	1996-03-27	NONE                     	TRUCK     	ounts. pendin
3010	174	4	2	22	23631.74	0.09	0.06	N	O	1996-03-06	1996-04-06	1996-03-18	COLLECT COD              	REG AIR   	 final deposit
3010	58	6	3	24	22993.20	0.04	0.07	N	O	1996-05-09	1996-03-14	1996-05-15	DELIVER IN PERSON        	RAIL      	ar, even reques
3010	24	7	4	28	25872.56	0.09	0.06	N	O	1996-03-05	1996-03-28	1996-04-03	DELIVER IN PERSON        	FOB       	ake carefully carefully even request
3010	104	5	5	9	9036.90	0.02	0.02	N	O	1996-04-28	1996-03-17	1996-05-18	NONE                     	SHIP      	inal packages. quickly even pinto
3010	92	3	6	38	37699.42	0.05	0.07	N	O	1996-04-15	1996-03-16	1996-04-21	DELIVER IN PERSON        	RAIL      	accounts ar
3012	195	7	1	49	53664.31	0.00	0.00	A	F	1993-08-07	1993-07-01	1993-08-08	NONE                     	MAIL      	 quickly furious packages. silently unusua
3012	161	2	2	37	39262.92	0.06	0.03	A	F	1993-08-16	1993-06-07	1993-08-24	TAKE BACK RETURN         	REG AIR   	uickly permanent packages sleep caref
3013	94	6	1	31	30816.79	0.08	0.08	N	O	1997-05-03	1997-04-05	1997-05-25	NONE                     	AIR       	y furious depen
3013	139	5	2	30	31173.90	0.05	0.06	N	O	1997-05-02	1997-03-09	1997-05-12	TAKE BACK RETURN         	MAIL      	ronic packages. slyly even
3013	120	10	3	35	35704.20	0.00	0.03	N	O	1997-04-02	1997-05-04	1997-04-16	COLLECT COD              	MAIL      	ely accord
3013	181	2	4	17	18380.06	0.01	0.07	N	O	1997-02-26	1997-05-02	1997-03-27	DELIVER IN PERSON        	SHIP      	fully unusual account
3013	60	5	5	20	19201.20	0.00	0.04	N	O	1997-05-06	1997-03-18	1997-05-12	COLLECT COD              	RAIL      	unts boost regular ideas. slyly pe
3040	16	6	1	18	16488.18	0.08	0.04	R	F	1993-06-25	1993-07-06	1993-07-19	TAKE BACK RETURN         	SHIP      	ly thin accou
3040	133	9	2	9	9298.17	0.00	0.01	A	F	1993-06-12	1993-05-16	1993-06-14	NONE                     	RAIL      	ges. pending packages wake. requests
3040	126	5	3	30	30783.60	0.01	0.01	A	F	1993-08-06	1993-05-18	1993-08-19	NONE                     	MAIL      	x furiously bold packages. expres
3040	83	4	4	14	13763.12	0.05	0.04	A	F	1993-05-13	1993-05-18	1993-05-19	TAKE BACK RETURN         	REG AIR   	 haggle carefully. express hocke
3040	52	3	5	43	40938.15	0.04	0.04	R	F	1993-05-21	1993-05-25	1993-05-26	NONE                     	MAIL      	sts nag slyly alongside of the depos
3040	18	5	6	10	9180.10	0.08	0.04	R	F	1993-05-16	1993-06-24	1993-06-11	DELIVER IN PERSON        	MAIL      	ely regular foxes haggle dari
3042	105	2	1	30	30153.00	0.08	0.06	A	F	1995-01-12	1995-02-15	1995-01-24	DELIVER IN PERSON        	SHIP      	the requests detect fu
3042	102	3	2	28	28058.80	0.05	0.03	A	F	1994-11-24	1995-01-02	1994-12-06	TAKE BACK RETURN         	MAIL      	ng the furiously r
3042	14	8	3	34	31076.34	0.04	0.00	R	F	1994-12-11	1995-02-03	1994-12-21	TAKE BACK RETURN         	TRUCK     	can wake after the enticingly stealthy i
3042	48	1	4	19	18012.76	0.02	0.01	A	F	1995-03-05	1995-01-24	1995-03-17	COLLECT COD              	TRUCK     	e carefully. regul
3043	46	9	1	23	21758.92	0.07	0.04	R	F	1992-05-08	1992-07-22	1992-05-18	COLLECT COD              	TRUCK     	uickly above the pending,
3043	6	3	2	15	13590.00	0.03	0.05	A	F	1992-05-27	1992-06-03	1992-06-09	COLLECT COD              	FOB       	usly furiously
3043	60	1	3	42	40322.52	0.10	0.07	R	F	1992-07-15	1992-06-19	1992-07-23	NONE                     	MAIL      	ide of the un
3043	91	2	4	5	4955.45	0.10	0.01	A	F	1992-05-22	1992-07-02	1992-06-20	TAKE BACK RETURN         	TRUCK     	ake blithely re
3044	101	2	1	10	10011.00	0.07	0.08	N	O	1996-07-13	1996-05-06	1996-07-21	TAKE BACK RETURN         	REG AIR   	 slyly ironic requests. s
3044	168	7	2	3	3204.48	0.06	0.02	N	O	1996-07-27	1996-05-26	1996-08-15	TAKE BACK RETURN         	AIR       	ecoys haggle furiously pending requests.
3044	19	3	3	47	43193.47	0.09	0.00	N	O	1996-05-24	1996-06-22	1996-05-30	NONE                     	REG AIR   	ly around the car
3046	74	5	1	44	42859.08	0.03	0.03	N	O	1996-03-03	1996-02-25	1996-04-01	NONE                     	AIR       	 are quickly. blithe
3046	54	5	2	46	43886.30	0.03	0.08	N	O	1996-03-22	1996-02-28	1996-04-07	TAKE BACK RETURN         	AIR       	sits sleep furious
3046	2	9	3	31	27962.00	0.03	0.07	N	O	1996-03-24	1996-01-30	1996-03-26	NONE                     	RAIL      	y pending somas alongside of the slyly iro
3075	9	6	1	39	35451.00	0.02	0.03	A	F	1994-06-10	1994-06-21	1994-06-20	NONE                     	FOB       	ing deposits nag 
3075	52	10	2	2	1904.10	0.07	0.08	R	F	1994-06-14	1994-06-10	1994-06-25	TAKE BACK RETURN         	AIR       	. unusual, unusual accounts haggle furious
3077	72	2	1	25	24301.75	0.06	0.01	N	O	1997-09-14	1997-10-16	1997-10-06	NONE                     	TRUCK     	lent account
3077	91	3	2	40	39643.60	0.05	0.06	N	O	1997-10-22	1997-09-19	1997-11-19	DELIVER IN PERSON        	AIR       	to the enticing packag
3077	78	7	3	13	12714.91	0.03	0.07	N	O	1997-09-09	1997-10-15	1997-09-19	NONE                     	TRUCK     	luffily close depende
3077	115	5	4	23	23347.53	0.03	0.02	N	O	1997-11-05	1997-09-16	1997-11-20	NONE                     	MAIL      	lly. fluffily pending dinos across
3079	70	5	1	20	19401.40	0.05	0.00	N	O	1997-10-18	1997-10-26	1997-11-14	NONE                     	RAIL      	ets are according to the quickly dari
3079	117	1	2	38	38650.18	0.08	0.07	N	O	1997-11-07	1997-11-25	1997-12-06	NONE                     	RAIL      	e carefully regular realms
3079	17	8	3	40	36680.40	0.02	0.08	N	O	1997-09-26	1997-12-11	1997-10-09	NONE                     	RAIL      	ide of the pending, special deposi
3079	24	5	4	2	1848.04	0.00	0.08	N	O	1998-01-05	1997-11-17	1998-01-28	NONE                     	FOB       	ly busy requests believ
3079	188	9	5	2	2176.36	0.10	0.00	N	O	1997-12-27	1997-10-25	1998-01-08	COLLECT COD              	SHIP      	y regular asymptotes doz
3079	166	1	6	46	49043.36	0.00	0.00	N	O	1997-11-19	1997-11-04	1997-11-25	DELIVER IN PERSON        	REG AIR   	es. final, regula
3104	51	6	1	20	19021.00	0.01	0.08	A	F	1993-12-31	1993-11-24	1994-01-12	DELIVER IN PERSON        	REG AIR   	s are. furiously s
3104	48	1	2	47	44557.88	0.02	0.05	A	F	1993-12-25	1993-11-02	1994-01-12	COLLECT COD              	RAIL      	ily daring acc
3104	63	4	3	11	10593.66	0.02	0.03	A	F	1993-10-05	1993-11-30	1993-10-27	NONE                     	TRUCK     	 special deposits u
3104	38	9	4	26	24388.78	0.02	0.08	R	F	1994-01-02	1993-12-05	1994-01-31	TAKE BACK RETURN         	TRUCK     	es boost carefully. slyly 
3105	184	5	1	11	11925.98	0.01	0.06	N	O	1997-02-07	1997-02-09	1997-03-01	NONE                     	FOB       	kly bold depths caj
3105	45	6	2	9	8505.36	0.08	0.08	N	O	1996-12-25	1997-02-04	1997-01-09	COLLECT COD              	SHIP      	es wake among t
3105	25	4	3	48	44400.96	0.02	0.05	N	O	1997-02-28	1997-01-31	1997-03-18	DELIVER IN PERSON        	REG AIR   	ending platelets wake carefully ironic inst
3105	91	5	4	23	22795.07	0.04	0.07	N	O	1997-03-08	1996-12-14	1997-03-18	COLLECT COD              	REG AIR   	 detect slyly. blithely unusual requests ar
3105	90	1	5	8	7920.72	0.07	0.07	N	O	1996-12-28	1996-12-28	1997-01-25	NONE                     	FOB       	s. blithely unusual ideas was after
3105	47	6	6	30	28411.20	0.08	0.05	N	O	1997-03-03	1997-02-03	1997-03-05	NONE                     	FOB       	ess accounts boost among t
3106	86	7	1	22	21693.76	0.03	0.02	N	O	1997-02-28	1997-02-12	1997-03-03	DELIVER IN PERSON        	FOB       	structions atop the blithely
3106	136	2	2	49	50770.37	0.06	0.06	N	O	1997-02-27	1997-03-11	1997-03-12	NONE                     	TRUCK     	lets. quietly regular courts 
3106	52	7	3	42	39986.10	0.09	0.07	N	O	1997-04-05	1997-03-17	1997-04-22	COLLECT COD              	REG AIR   	nstructions wake. furiously 
3106	196	10	4	6	6577.14	0.10	0.07	N	O	1997-02-02	1997-04-11	1997-02-27	COLLECT COD              	REG AIR   	symptotes. slyly bold platelets cajol
3106	65	2	5	16	15440.96	0.09	0.08	N	O	1997-02-25	1997-04-10	1997-03-16	NONE                     	AIR       	sits wake slyl
3107	149	6	1	16	16786.24	0.05	0.04	N	O	1997-08-30	1997-10-20	1997-09-20	TAKE BACK RETURN         	REG AIR   	regular pinto beans. ironic ideas haggle
3107	142	3	2	35	36474.90	0.05	0.06	N	O	1997-08-27	1997-11-19	1997-09-14	COLLECT COD              	TRUCK     	ets doubt furiously final ideas. final
3107	170	9	3	23	24613.91	0.03	0.06	N	O	1997-12-10	1997-11-11	1997-12-14	TAKE BACK RETURN         	SHIP      	atelets must ha
3107	87	8	4	27	26651.16	0.00	0.08	N	O	1997-11-15	1997-10-31	1997-11-28	DELIVER IN PERSON        	FOB       	furiously final 
3109	18	2	1	32	29376.32	0.08	0.03	A	F	1993-09-05	1993-10-06	1993-09-18	DELIVER IN PERSON        	FOB       	ecial orbits are furiou
3109	145	4	2	49	51211.86	0.08	0.06	R	F	1993-10-24	1993-09-30	1993-11-21	TAKE BACK RETURN         	AIR       	 even pearls. furiously pending 
3109	176	4	3	43	46275.31	0.04	0.07	R	F	1993-09-29	1993-09-06	1993-10-13	COLLECT COD              	MAIL      	ding to the foxes. 
3109	79	10	4	26	25455.82	0.01	0.05	R	F	1993-11-16	1993-10-18	1993-12-06	TAKE BACK RETURN         	TRUCK     	 sleep slyly according to t
3109	143	2	5	50	52157.00	0.01	0.08	A	F	1993-09-17	1993-10-16	1993-10-11	NONE                     	FOB       	 regular packages boost blithely even, re
3109	15	9	6	10	9150.10	0.10	0.04	A	F	1993-10-26	1993-10-03	1993-11-09	NONE                     	TRUCK     	sits haggle carefully. regular, unusual ac
3110	89	10	1	1	989.08	0.02	0.07	A	F	1995-01-15	1995-01-20	1995-01-30	DELIVER IN PERSON        	REG AIR   	c theodolites a
3110	57	2	2	31	29668.55	0.01	0.06	R	F	1995-03-31	1995-03-07	1995-04-21	TAKE BACK RETURN         	REG AIR   	en deposits. ironic
3110	3	10	3	34	30702.00	0.02	0.02	A	F	1995-02-23	1995-01-27	1995-03-09	TAKE BACK RETURN         	FOB       	ly pending requests ha
3110	40	1	4	16	15040.64	0.04	0.04	A	F	1995-01-10	1995-02-06	1995-01-26	NONE                     	MAIL      	across the regular acco
3110	140	6	5	39	40565.46	0.09	0.01	A	F	1995-02-09	1995-01-21	1995-02-21	NONE                     	MAIL      	side of the blithely unusual courts. slyly 
3111	137	8	1	22	22816.86	0.06	0.05	N	O	1995-09-21	1995-11-09	1995-10-17	COLLECT COD              	REG AIR   	quests. regular dolphins against the 
3111	58	10	2	30	28741.50	0.06	0.05	N	O	1995-10-05	1995-11-15	1995-11-01	TAKE BACK RETURN         	TRUCK     	eas are furiously slyly special deposits.
3111	52	3	3	10	9520.50	0.02	0.02	N	O	1995-11-10	1995-11-02	1995-12-04	NONE                     	FOB       	ng the slyly ironic inst
3111	132	3	4	31	31996.03	0.00	0.08	N	O	1995-10-26	1995-09-26	1995-11-02	TAKE BACK RETURN         	MAIL      	kages detect express attainments
3111	54	6	5	14	13356.70	0.05	0.04	N	O	1995-10-17	1995-10-19	1995-10-19	TAKE BACK RETURN         	SHIP      	re. pinto 
3111	86	7	6	5	4930.40	0.03	0.08	N	O	1995-08-30	1995-10-16	1995-09-04	DELIVER IN PERSON        	TRUCK     	. carefully even ideas
3111	148	9	7	41	42973.74	0.09	0.05	N	O	1995-11-22	1995-11-01	1995-12-01	TAKE BACK RETURN         	FOB       	fily slow ideas. 
3140	7	4	1	21	19047.00	0.08	0.02	R	F	1992-04-12	1992-05-31	1992-04-21	NONE                     	REG AIR   	 furiously sly excuses according to the
3140	89	10	2	10	9890.80	0.07	0.01	A	F	1992-05-30	1992-05-09	1992-06-09	COLLECT COD              	RAIL      	accounts. expres
3140	133	4	3	28	28927.64	0.06	0.00	R	F	1992-06-08	1992-07-07	1992-07-08	TAKE BACK RETURN         	SHIP      	lar ideas. slyly ironic d
3141	177	6	1	32	34469.44	0.06	0.00	N	O	1995-11-21	1995-12-18	1995-11-26	DELIVER IN PERSON        	FOB       	oxes are quickly about t
3141	10	7	2	37	33670.37	0.10	0.05	N	O	1996-01-24	1995-12-16	1996-01-27	DELIVER IN PERSON        	AIR       	press pinto beans. bold accounts boost b
3141	79	7	3	9	8811.63	0.09	0.02	N	O	1995-11-11	1995-12-10	1995-12-02	DELIVER IN PERSON        	MAIL      	uickly ironic, pendi
3141	46	9	4	47	44463.88	0.03	0.01	N	O	1995-11-29	1996-01-13	1995-12-10	TAKE BACK RETURN         	TRUCK     	 are slyly pi
3142	120	7	1	15	15301.80	0.03	0.08	R	F	1992-08-15	1992-08-18	1992-08-22	DELIVER IN PERSON        	AIR       	instructions are. ironic packages doz
3143	90	1	1	22	21781.98	0.02	0.00	A	F	1993-05-11	1993-03-26	1993-05-20	TAKE BACK RETURN         	MAIL      	l, special instructions nag 
3143	183	4	2	40	43327.20	0.03	0.08	A	F	1993-05-07	1993-03-29	1993-05-17	COLLECT COD              	FOB       	sly unusual theodolites. slyly ev
3143	183	4	3	22	23829.96	0.05	0.03	A	F	1993-03-18	1993-05-09	1993-04-14	DELIVER IN PERSON        	MAIL      	beans. fluf
3143	66	7	4	46	44438.76	0.05	0.08	R	F	1993-04-19	1993-03-21	1993-05-05	COLLECT COD              	REG AIR   	low forges haggle. even packages use bli
3169	192	4	1	12	13106.28	0.01	0.04	R	F	1994-01-05	1994-03-18	1994-01-21	COLLECT COD              	REG AIR   	 regular d
3169	200	3	2	17	18703.40	0.05	0.04	R	F	1994-03-02	1994-01-21	1994-03-03	DELIVER IN PERSON        	TRUCK     	usly regular packages. ironi
3169	188	9	3	12	13058.16	0.08	0.07	A	F	1994-04-18	1994-03-12	1994-05-08	TAKE BACK RETURN         	FOB       	atelets. pac
3169	105	6	4	26	26132.60	0.10	0.04	R	F	1994-04-08	1994-03-21	1994-04-29	NONE                     	TRUCK     	ter the regular ideas. slyly iro
3169	108	9	5	6	6048.60	0.09	0.01	A	F	1994-03-24	1994-02-22	1994-04-04	TAKE BACK RETURN         	AIR       	ular instructions. ca
3169	177	8	6	46	49549.82	0.02	0.07	A	F	1994-02-01	1994-01-22	1994-02-24	DELIVER IN PERSON        	RAIL      	thely bold theodolites are fl
3172	96	9	1	4	3984.36	0.06	0.07	A	F	1992-09-26	1992-08-15	1992-10-20	DELIVER IN PERSON        	TRUCK     	s are slyly thin package
3172	148	7	2	43	45070.02	0.05	0.07	R	F	1992-08-22	1992-07-07	1992-08-26	COLLECT COD              	MAIL      	 final packages. 
3172	132	3	3	13	13417.69	0.03	0.01	R	F	1992-07-06	1992-08-06	1992-08-05	DELIVER IN PERSON        	MAIL      	inal deposits haggle along the
3172	135	6	4	28	28983.64	0.08	0.04	R	F	1992-07-09	1992-07-14	1992-07-16	NONE                     	MAIL      	regular ideas. packages are furi
3172	64	5	5	31	29885.86	0.05	0.08	A	F	1992-09-01	1992-08-27	1992-09-23	NONE                     	SHIP      	. slyly regular dependencies haggle quiet
3174	186	7	1	6	6517.08	0.04	0.08	N	O	1996-03-13	1996-02-09	1996-03-22	DELIVER IN PERSON        	AIR       	 furiously ironic
3174	194	7	2	4	4376.76	0.01	0.05	N	O	1995-11-17	1996-01-08	1995-11-27	DELIVER IN PERSON        	RAIL      	deas sleep thi
3174	92	4	3	21	20833.89	0.08	0.05	N	O	1996-02-20	1995-12-28	1996-03-17	NONE                     	MAIL      	iously. idly bold theodolites a
3174	192	6	4	13	14198.47	0.08	0.06	N	O	1996-01-11	1996-01-26	1996-02-01	DELIVER IN PERSON        	SHIP      	leep quickly? slyly special platelets
3174	72	2	5	39	37910.73	0.02	0.06	N	O	1995-12-02	1996-02-08	1995-12-12	TAKE BACK RETURN         	TRUCK     	 wake slyly foxes. bold requests p
3174	120	7	6	8	8160.96	0.07	0.08	N	O	1995-12-07	1996-01-08	1995-12-29	DELIVER IN PERSON        	TRUCK     	nic deposits among t
3175	120	10	1	28	28563.36	0.10	0.01	R	F	1994-09-27	1994-10-05	1994-10-04	NONE                     	FOB       	ore the even, silent foxes. b
3175	1	4	2	38	34238.00	0.01	0.07	R	F	1994-10-10	1994-08-25	1994-10-28	NONE                     	MAIL      	the quickly even dolph
3175	129	4	3	12	12349.44	0.09	0.07	R	F	1994-10-16	1994-09-15	1994-10-18	NONE                     	AIR       	ter the pending deposits. slyly e
3175	85	6	4	14	13791.12	0.02	0.05	R	F	1994-10-21	1994-09-05	1994-11-15	NONE                     	MAIL      	nt dependencies are quietly even 
3175	18	8	5	47	43146.47	0.08	0.03	R	F	1994-08-08	1994-09-10	1994-08-21	COLLECT COD              	REG AIR   	 final requests x-r
3175	175	6	6	44	47307.48	0.01	0.00	R	F	1994-09-26	1994-08-30	1994-10-24	TAKE BACK RETURN         	MAIL      	are carefully furiously ironic accounts. e
3175	1	4	7	32	28832.00	0.01	0.02	R	F	1994-09-29	1994-09-20	1994-10-10	TAKE BACK RETURN         	SHIP      	lites sleep
3201	46	7	1	11	10406.44	0.10	0.06	A	F	1993-09-27	1993-08-29	1993-10-18	NONE                     	TRUCK     	ing to the furiously expr
3201	118	5	2	27	27488.97	0.08	0.02	R	F	1993-08-31	1993-08-24	1993-09-08	TAKE BACK RETURN         	FOB       	deposits are slyly along
3201	119	6	3	50	50955.50	0.00	0.08	R	F	1993-10-27	1993-09-30	1993-11-16	COLLECT COD              	TRUCK     	 deposits. express, ir
3202	183	4	1	30	32495.40	0.09	0.02	A	F	1993-03-18	1993-03-10	1993-03-23	COLLECT COD              	SHIP      	ven platelets. furiously final
3202	20	4	2	22	20240.44	0.01	0.02	R	F	1993-02-16	1993-02-16	1993-03-16	TAKE BACK RETURN         	MAIL      	the express packages. fu
3203	144	5	1	23	24015.22	0.01	0.07	N	O	1998-01-04	1998-01-12	1998-01-24	COLLECT COD              	SHIP      	uses. fluffily ironic pinto bea
3203	188	9	2	22	23939.96	0.03	0.03	N	O	1998-02-12	1998-01-01	1998-02-18	TAKE BACK RETURN         	REG AIR   	e the blithely regular accounts boost f
3232	14	5	1	22	20108.22	0.10	0.01	A	F	1992-11-30	1992-12-09	1992-12-04	NONE                     	RAIL      	thely. furio
3232	135	1	2	34	35194.42	0.07	0.04	R	F	1993-01-09	1992-11-14	1993-02-03	NONE                     	SHIP      	old packages integrate quickly 
3232	181	2	3	3	3243.54	0.04	0.06	R	F	1992-12-14	1992-12-11	1992-12-29	DELIVER IN PERSON        	FOB       	ily blithely ironic acco
3237	11	5	1	11	10021.11	0.02	0.07	A	F	1992-08-03	1992-07-31	1992-08-13	TAKE BACK RETURN         	AIR       	es. permanently express platelets besid
3238	72	3	1	12	11664.84	0.06	0.01	R	F	1993-03-06	1993-05-08	1993-04-01	DELIVER IN PERSON        	AIR       	ackages affix furiously. furiously bol
3238	173	2	2	26	27902.42	0.01	0.06	A	F	1993-02-25	1993-04-04	1993-03-20	TAKE BACK RETURN         	REG AIR   	g accounts sleep furiously ironic attai
3238	81	2	3	1	981.08	0.00	0.04	R	F	1993-05-17	1993-04-18	1993-05-27	NONE                     	SHIP      	wake alongs
3239	45	8	1	50	47252.00	0.05	0.01	N	O	1998-02-09	1998-04-02	1998-02-22	NONE                     	FOB       	d blithely stea
3239	45	8	2	43	40636.72	0.01	0.06	N	O	1998-01-15	1998-03-12	1998-01-29	COLLECT COD              	REG AIR   	y. bold pinto beans use 
3239	13	7	3	13	11869.13	0.01	0.05	N	O	1998-02-10	1998-02-19	1998-02-25	DELIVER IN PERSON        	MAIL      	r deposits solve fluf
3239	195	6	4	26	28474.94	0.03	0.05	N	O	1998-01-21	1998-03-21	1998-02-08	DELIVER IN PERSON        	SHIP      	ngly pending platelets are fluff
3239	12	9	5	31	28272.31	0.10	0.08	N	O	1998-04-14	1998-03-24	1998-04-17	DELIVER IN PERSON        	FOB       	foxes. pendin
3265	25	4	1	8	7400.16	0.06	0.02	A	F	1992-09-01	1992-09-12	1992-09-27	DELIVER IN PERSON        	TRUCK     	thely ironic requests sleep slyly-- i
3265	72	2	2	7	6804.49	0.09	0.00	R	F	1992-09-16	1992-09-04	1992-10-14	DELIVER IN PERSON        	MAIL      	he forges. fluffily regular asym
3265	191	4	3	28	30553.32	0.09	0.08	A	F	1992-10-22	1992-08-23	1992-10-25	NONE                     	RAIL      	n requests. quickly final dinos
3267	185	6	1	33	35810.94	0.06	0.01	N	O	1997-03-30	1997-03-25	1997-04-23	TAKE BACK RETURN         	AIR       	es boost. 
3271	57	9	1	30	28711.50	0.01	0.04	A	F	1992-01-16	1992-03-20	1992-01-17	DELIVER IN PERSON        	AIR       	r the unusual Tiresia
3271	54	5	2	18	17172.90	0.09	0.06	R	F	1992-05-01	1992-03-28	1992-05-29	DELIVER IN PERSON        	FOB       	 packages eat around the furiously regul
3271	95	6	3	14	13931.26	0.05	0.01	A	F	1992-02-24	1992-02-14	1992-03-23	NONE                     	AIR       	ending, even packa
3271	64	1	4	29	27957.74	0.07	0.04	A	F	1992-03-10	1992-02-05	1992-03-14	COLLECT COD              	MAIL      	lar instructions. carefully regular
3298	149	6	1	9	9442.26	0.01	0.06	N	O	1996-08-15	1996-05-24	1996-09-12	COLLECT COD              	REG AIR   	ly final accou
3298	186	7	2	27	29326.86	0.06	0.06	N	O	1996-07-10	1996-05-21	1996-07-15	DELIVER IN PERSON        	FOB       	lar packages. regular deposit
3298	29	2	3	25	23225.50	0.10	0.08	N	O	1996-06-30	1996-05-31	1996-07-23	COLLECT COD              	SHIP      	ly express f
3298	191	5	4	1	1091.19	0.10	0.03	N	O	1996-07-31	1996-05-23	1996-08-24	TAKE BACK RETURN         	FOB       	refully regular requ
3300	129	4	1	3	3087.36	0.07	0.02	N	O	1995-11-01	1995-10-02	1995-11-20	NONE                     	REG AIR   	g according to the dugouts. caref
3300	149	10	2	23	24130.22	0.02	0.02	N	O	1995-08-17	1995-09-03	1995-09-04	COLLECT COD              	TRUCK     	he fluffily final a
3301	169	8	1	45	48112.20	0.04	0.05	A	F	1994-11-19	1994-10-27	1994-11-24	TAKE BACK RETURN         	FOB       	nusual, final excuses after the entici
3302	36	2	1	45	42121.35	0.09	0.00	N	O	1996-01-24	1995-12-16	1996-02-13	COLLECT COD              	FOB       	counts use quickl
3303	184	5	1	25	27104.50	0.06	0.01	N	O	1998-03-25	1998-01-31	1998-04-12	NONE                     	SHIP      	lly regular pi
3303	21	2	2	15	13815.30	0.04	0.06	N	O	1998-01-29	1998-01-22	1998-02-21	COLLECT COD              	SHIP      	 detect sly
3303	99	10	3	37	36966.33	0.05	0.02	N	O	1998-02-16	1998-03-07	1998-02-18	TAKE BACK RETURN         	TRUCK     	 carefully ironic asympt
3303	36	2	4	26	24336.78	0.09	0.00	N	O	1998-01-18	1998-03-11	1998-02-11	DELIVER IN PERSON        	REG AIR   	ickly permanent requests w
3328	113	7	1	6	6078.66	0.03	0.08	A	F	1993-03-07	1993-01-25	1993-03-29	COLLECT COD              	TRUCK     	ffily even instructions detect b
3328	5	2	2	23	20815.00	0.01	0.06	R	F	1993-01-12	1993-02-07	1993-01-30	TAKE BACK RETURN         	MAIL      	y. careful
3328	139	10	3	44	45721.72	0.05	0.00	R	F	1992-12-03	1992-12-19	1992-12-09	TAKE BACK RETURN         	FOB       	dly quickly final foxes? re
3328	95	9	4	42	41793.78	0.01	0.05	R	F	1992-11-24	1992-12-20	1992-12-06	DELIVER IN PERSON        	AIR       	ronic requests
3328	131	7	5	25	25778.25	0.05	0.00	R	F	1993-01-28	1993-01-04	1993-01-31	NONE                     	RAIL      	e unusual, r
3330	20	7	1	49	45080.98	0.05	0.01	R	F	1995-03-02	1995-03-03	1995-03-16	DELIVER IN PERSON        	TRUCK     	haggle carefully alongside of the bold r
3332	84	5	1	28	27554.24	0.10	0.02	R	F	1994-12-30	1995-01-16	1995-01-16	COLLECT COD              	FOB       	s against the carefully special multipl
3332	136	2	2	21	21758.73	0.08	0.04	R	F	1995-02-04	1995-01-08	1995-02-06	COLLECT COD              	MAIL      	 quick packages sle
3332	134	5	3	27	27921.51	0.03	0.02	A	F	1994-12-10	1995-01-14	1994-12-11	TAKE BACK RETURN         	FOB       	ording to the slyly regula
3365	151	6	1	37	38892.55	0.02	0.08	R	F	1994-12-22	1995-02-07	1995-01-20	TAKE BACK RETURN         	SHIP      	requests. quickly pending instructions a
3365	167	2	2	37	39484.92	0.07	0.08	A	F	1994-11-24	1995-01-09	1994-11-27	NONE                     	REG AIR   	oze blithely. furiously ironic theodolit
3365	115	6	3	13	13196.43	0.09	0.02	R	F	1995-02-25	1995-01-31	1995-03-16	NONE                     	RAIL      	pths wake r
3365	176	4	4	49	52732.33	0.02	0.07	R	F	1995-01-03	1995-01-01	1995-01-18	COLLECT COD              	MAIL      	lyly unusual asymptotes. final
3365	16	3	5	2	1832.02	0.00	0.03	R	F	1995-02-04	1994-12-30	1995-03-06	TAKE BACK RETURN         	FOB       	es cajole fluffily pe
3365	126	5	6	24	24626.88	0.01	0.00	R	F	1995-02-27	1995-01-09	1995-03-27	DELIVER IN PERSON        	REG AIR   	into beans? carefully regula
3394	155	6	1	33	34819.95	0.07	0.08	N	O	1996-08-07	1996-07-17	1996-09-02	TAKE BACK RETURN         	SHIP      	ideas alongside of th
3394	146	3	2	43	44984.02	0.08	0.03	N	O	1996-08-23	1996-07-20	1996-08-25	COLLECT COD              	RAIL      	hockey players. slyly regular requests afte
3394	88	9	3	26	25690.08	0.01	0.00	N	O	1996-08-08	1996-06-12	1996-09-05	TAKE BACK RETURN         	RAIL      	its use furiously. even, even account
3394	81	2	4	14	13735.12	0.08	0.00	N	O	1996-06-02	1996-07-02	1996-06-19	COLLECT COD              	MAIL      	e furiously final theodolites. furio
3394	127	8	5	30	30813.60	0.04	0.06	N	O	1996-05-12	1996-07-24	1996-05-19	COLLECT COD              	REG AIR   	t ideas according to the fluffily iro
3394	184	5	6	14	15178.52	0.05	0.05	N	O	1996-06-18	1996-06-24	1996-07-17	NONE                     	REG AIR   	arefully regular do
3395	142	3	1	21	21884.94	0.03	0.06	R	F	1994-12-19	1995-01-13	1994-12-25	TAKE BACK RETURN         	SHIP      	 careful dep
3395	36	2	2	38	35569.14	0.01	0.07	R	F	1995-01-13	1995-01-13	1995-01-25	COLLECT COD              	SHIP      	 silent accounts are blithely
3395	43	4	3	43	40550.72	0.06	0.07	A	F	1994-12-13	1995-01-07	1994-12-14	COLLECT COD              	AIR       	ckages above the furiously regu
3395	122	1	4	39	39862.68	0.05	0.07	R	F	1994-12-03	1995-01-17	1994-12-10	NONE                     	AIR       	riously unusual theodolites. fur
3396	128	7	1	34	34956.08	0.00	0.06	A	F	1994-05-30	1994-08-16	1994-06-11	NONE                     	AIR       	. slyly unusual packages wak
3396	49	6	2	43	40808.72	0.03	0.08	A	F	1994-07-03	1994-08-09	1994-07-14	TAKE BACK RETURN         	MAIL      	cial packages cajole blithely around the 
3396	138	4	3	9	9343.17	0.01	0.06	R	F	1994-07-01	1994-08-18	1994-07-21	DELIVER IN PERSON        	AIR       	usly special foxes. accounts wake careful
3396	75	3	4	32	31202.24	0.06	0.02	R	F	1994-08-07	1994-08-10	1994-09-05	COLLECT COD              	TRUCK     	osits are slyly. final, bold foxes s
3396	126	5	5	27	27705.24	0.02	0.01	A	F	1994-09-14	1994-07-26	1994-09-28	DELIVER IN PERSON        	FOB       	 theodolites 
3396	39	10	6	18	16902.54	0.10	0.00	A	F	1994-07-27	1994-06-26	1994-08-25	TAKE BACK RETURN         	REG AIR   	l requests haggle furiously along the fur
3396	198	2	7	31	34043.89	0.05	0.06	A	F	1994-06-07	1994-06-23	1994-06-19	TAKE BACK RETURN         	REG AIR   	l, express pinto beans. quic
3426	110	5	1	20	20202.20	0.05	0.04	N	O	1996-11-10	1996-12-24	1996-12-01	COLLECT COD              	FOB       	sits cajole blit
3426	14	4	2	19	17366.19	0.10	0.08	N	O	1996-11-02	1997-01-13	1996-11-15	DELIVER IN PERSON        	RAIL      	slyly special packages oug
3426	67	6	3	19	18374.14	0.08	0.05	N	O	1996-12-07	1996-12-15	1996-12-14	DELIVER IN PERSON        	FOB       	c accounts cajole carefu
3426	6	7	4	9	8154.00	0.09	0.05	N	O	1996-12-24	1997-01-14	1997-01-13	NONE                     	FOB       	pecial theodolites haggle fluf
3426	49	6	5	31	29420.24	0.07	0.08	N	O	1996-11-11	1996-12-10	1996-12-10	DELIVER IN PERSON        	SHIP      	 even sentiment
3458	133	4	1	48	49590.24	0.06	0.04	R	F	1995-03-17	1995-01-25	1995-03-28	TAKE BACK RETURN         	AIR       	iously pending dep
3458	50	3	2	46	43702.30	0.06	0.06	R	F	1995-03-08	1995-01-21	1995-03-10	TAKE BACK RETURN         	SHIP      	nod across the boldly even instruct
3458	143	4	3	36	37553.04	0.01	0.06	R	F	1995-04-20	1995-02-14	1995-05-09	TAKE BACK RETURN         	REG AIR   	s lose. blithely ironic requests boost
3458	16	10	4	16	14656.16	0.09	0.03	R	F	1995-03-01	1995-02-25	1995-03-06	TAKE BACK RETURN         	AIR       	s grow carefully. express, final grouc
3458	157	5	5	2	2114.30	0.09	0.03	A	F	1995-02-05	1995-02-01	1995-03-07	COLLECT COD              	FOB       	ironic packages haggle past the furiously 
3458	142	1	6	6	6252.84	0.09	0.04	A	F	1995-03-10	1995-02-02	1995-03-23	TAKE BACK RETURN         	AIR       	dolites; regular theodolites cajole 
3460	11	1	1	40	36440.40	0.10	0.06	N	O	1995-12-28	1995-12-14	1996-01-02	NONE                     	REG AIR   	odolites are slyly bold deposits
3460	74	4	2	3	2922.21	0.06	0.00	N	O	1996-01-19	1995-12-28	1996-01-31	COLLECT COD              	AIR       	er quickly 
3460	35	1	3	40	37401.20	0.08	0.07	N	O	1995-10-29	1995-11-10	1995-11-24	TAKE BACK RETURN         	REG AIR   	o the even deposits
3460	95	8	4	50	49754.50	0.02	0.07	N	O	1996-01-30	1995-12-10	1996-02-06	DELIVER IN PERSON        	SHIP      	e slyly about the sly
3460	130	1	5	47	48416.11	0.08	0.05	N	O	1995-12-09	1995-11-12	1995-12-22	TAKE BACK RETURN         	SHIP      	es haggle slyly regular accounts. fi
3460	63	10	6	46	44300.76	0.03	0.07	N	O	1996-01-27	1996-01-01	1996-02-01	NONE                     	TRUCK     	uses run among the carefully even deposits
3460	45	2	7	28	26461.12	0.00	0.01	N	O	1995-10-28	1995-11-13	1995-11-17	COLLECT COD              	SHIP      	inal, ironic instructions. carefully
3461	100	4	1	49	49004.90	0.06	0.06	A	F	1993-03-09	1993-04-16	1993-03-13	DELIVER IN PERSON        	RAIL      	ual request
3461	63	4	2	27	26002.62	0.06	0.06	A	F	1993-02-10	1993-03-02	1993-03-04	COLLECT COD              	SHIP      	ely unusual deposits. quickly ir
3461	39	5	3	44	41317.32	0.09	0.06	A	F	1993-05-20	1993-04-03	1993-05-27	COLLECT COD              	RAIL      	 haggle quickly even ideas. fin
3461	95	7	4	41	40798.69	0.09	0.02	R	F	1993-02-19	1993-04-20	1993-02-21	NONE                     	TRUCK     	heodolites. blithely ironi
3461	90	1	5	16	15841.44	0.08	0.06	A	F	1993-05-09	1993-04-29	1993-05-26	TAKE BACK RETURN         	TRUCK     	 pending deposi
3461	167	2	6	24	25611.84	0.10	0.00	A	F	1993-06-01	1993-03-12	1993-06-20	TAKE BACK RETURN         	MAIL      	thely. carefully re
3462	151	3	1	4	4204.60	0.09	0.04	N	O	1997-06-12	1997-07-31	1997-06-16	COLLECT COD              	RAIL      	ackages. fu
3462	40	1	2	43	40421.72	0.08	0.03	N	O	1997-08-01	1997-07-18	1997-08-29	NONE                     	RAIL      	 carefully. final, final ideas sleep slyly
3462	129	4	3	6	6174.72	0.05	0.04	N	O	1997-06-02	1997-08-09	1997-06-30	NONE                     	RAIL      	iously regular fo
3462	99	3	4	2	1998.18	0.09	0.07	N	O	1997-09-10	1997-08-08	1997-09-19	NONE                     	AIR       	nic packages. even accounts alongside 
3462	38	4	5	14	13132.42	0.01	0.02	N	O	1997-05-31	1997-07-05	1997-06-24	COLLECT COD              	MAIL      	yly. blithely bold theodolites wa
3463	61	10	1	45	43247.70	0.02	0.02	A	F	1993-10-30	1993-11-04	1993-11-08	DELIVER IN PERSON        	FOB       	nts are slyly 
3463	98	1	2	43	42917.87	0.04	0.02	A	F	1993-10-28	1993-09-24	1993-11-03	DELIVER IN PERSON        	FOB       	 across the 
3489	186	7	1	19	20637.42	0.09	0.05	A	F	1993-07-31	1993-10-26	1993-08-15	NONE                     	SHIP      	c deposits alongside of the pending, fu
3489	29	4	2	46	42734.92	0.00	0.00	A	F	1993-08-02	1993-10-09	1993-08-10	TAKE BACK RETURN         	TRUCK     	xcuses? quickly stealthy dependenci
3491	154	2	1	28	29516.20	0.04	0.03	N	O	1998-09-29	1998-09-08	1998-10-23	COLLECT COD              	FOB       	ccounts. sly
3491	122	3	2	22	22486.64	0.08	0.02	N	O	1998-08-19	1998-08-22	1998-09-03	TAKE BACK RETURN         	REG AIR   	 grow against the boldly pending pinto bea
3493	93	6	1	31	30785.79	0.06	0.07	R	F	1993-10-22	1993-10-12	1993-11-07	DELIVER IN PERSON        	REG AIR   	ructions. slyly regular accounts across the
3493	132	3	2	10	10321.30	0.02	0.06	R	F	1993-08-27	1993-10-07	1993-09-23	COLLECT COD              	TRUCK     	hall have to integ
3495	28	3	1	20	18560.40	0.10	0.03	N	O	1996-04-24	1996-05-18	1996-05-01	TAKE BACK RETURN         	RAIL      	posits are carefully; forges cajole qui
3495	173	1	2	24	25756.08	0.05	0.02	N	O	1996-03-22	1996-04-10	1996-04-07	DELIVER IN PERSON        	RAIL      	ic, final pains along the even request
3495	199	10	3	16	17587.04	0.08	0.02	N	O	1996-03-30	1996-04-02	1996-04-12	TAKE BACK RETURN         	AIR       	y bold dependencies; blithely idle sautern
3520	28	1	1	30	27840.60	0.04	0.02	N	O	1997-11-11	1997-10-02	1997-12-06	COLLECT COD              	SHIP      	deas should solve blithely among the ironi
3520	167	4	2	38	40552.08	0.00	0.04	N	O	1997-08-14	1997-10-26	1997-09-09	NONE                     	RAIL      	yly final packages according to the quickl
3520	106	9	3	5	5030.50	0.01	0.02	N	O	1997-11-13	1997-09-22	1997-12-09	NONE                     	MAIL      	ly even ideas haggle 
3520	64	5	4	41	39526.46	0.01	0.01	N	O	1997-08-06	1997-09-20	1997-08-20	TAKE BACK RETURN         	AIR       	 carefully pendi
3520	163	10	5	35	37210.60	0.02	0.02	N	O	1997-09-16	1997-09-03	1997-09-24	DELIVER IN PERSON        	FOB       	s nag carefully. sometimes unusual account
3521	59	4	1	48	46034.40	0.09	0.03	A	F	1993-01-03	1992-12-31	1993-01-22	NONE                     	AIR       	ses use. furiously express ideas wake f
3521	131	2	2	2	2062.26	0.05	0.06	R	F	1993-01-29	1992-12-20	1993-02-23	NONE                     	MAIL      	refully duri
3521	178	8	3	38	40970.46	0.00	0.08	A	F	1993-02-15	1992-12-10	1993-03-10	COLLECT COD              	FOB       	ges hang q
3521	144	7	4	26	27147.64	0.02	0.08	R	F	1993-01-04	1993-01-20	1993-01-17	DELIVER IN PERSON        	AIR       	onic dependencies haggle. fur
3521	36	7	5	28	26208.84	0.10	0.01	A	F	1993-01-06	1993-01-22	1993-02-02	TAKE BACK RETURN         	FOB       	e slyly above the slyly final
3522	4	9	1	6	5424.00	0.08	0.03	A	F	1995-01-21	1994-12-09	1995-01-23	NONE                     	SHIP      	tes snooze 
3522	87	8	2	48	47379.84	0.00	0.03	R	F	1994-12-05	1994-10-30	1994-12-26	TAKE BACK RETURN         	SHIP      	ve the quickly special packages
3522	157	2	3	46	48628.90	0.09	0.02	A	F	1994-11-12	1994-11-30	1994-11-20	NONE                     	AIR       	d the express, silent foxes. blit
3522	130	9	4	7	7210.91	0.10	0.02	A	F	1994-10-31	1994-11-19	1994-11-28	NONE                     	TRUCK     	e stealthil
3522	50	9	5	27	25651.35	0.02	0.05	R	F	1994-11-29	1994-12-15	1994-12-08	COLLECT COD              	REG AIR   	ic tithes. car
3522	158	10	6	18	19046.70	0.01	0.03	A	F	1994-11-16	1994-10-29	1994-11-29	COLLECT COD              	RAIL      	sits wake carefully pen
3523	25	6	1	15	13875.30	0.06	0.02	N	O	1998-06-26	1998-05-22	1998-07-24	COLLECT COD              	REG AIR   	se slyly pending, sp
3523	133	9	2	4	4132.52	0.03	0.06	N	O	1998-05-08	1998-05-18	1998-05-25	TAKE BACK RETURN         	MAIL      	ts. final accounts detect furiously along 
3523	50	7	3	24	22801.20	0.07	0.04	N	O	1998-08-02	1998-06-22	1998-08-27	COLLECT COD              	FOB       	ke according to the doggedly re
3523	192	4	4	36	39318.84	0.06	0.08	N	O	1998-05-26	1998-06-04	1998-06-25	DELIVER IN PERSON        	SHIP      	accounts. fluffily regu
3523	134	5	5	48	49638.24	0.00	0.01	N	O	1998-07-22	1998-06-25	1998-08-19	DELIVER IN PERSON        	AIR       	 regular requests
3525	46	7	1	12	11352.48	0.01	0.03	N	O	1996-03-08	1996-03-18	1996-03-16	NONE                     	TRUCK     	lar excuses wake carefull
3525	138	9	2	27	28029.51	0.03	0.03	N	O	1995-12-30	1996-01-23	1996-01-02	DELIVER IN PERSON        	SHIP      	y slyly special asymptotes
3525	75	5	3	31	30227.17	0.00	0.03	N	O	1996-03-08	1996-02-27	1996-03-13	COLLECT COD              	TRUCK     	he careful
3525	184	5	4	28	30357.04	0.03	0.02	N	O	1996-01-22	1996-02-08	1996-01-27	COLLECT COD              	FOB       	 nag according 
3552	197	8	1	18	19749.42	0.01	0.07	N	O	1997-08-11	1997-07-14	1997-08-15	DELIVER IN PERSON        	TRUCK     	s deposits against the blithely unusual pin
3552	90	1	2	44	43563.96	0.01	0.00	N	O	1997-08-08	1997-06-15	1997-08-29	COLLECT COD              	FOB       	ns after the blithely reg
3552	161	6	3	36	38201.76	0.04	0.08	N	O	1997-06-29	1997-06-24	1997-07-21	COLLECT COD              	TRUCK     	ly regular theodolites. fin
3554	175	5	1	32	34405.44	0.01	0.05	N	O	1995-09-28	1995-09-01	1995-10-07	NONE                     	RAIL      	. blithely ironic t
3554	145	6	2	18	18812.52	0.03	0.00	N	O	1995-09-11	1995-08-12	1995-10-04	DELIVER IN PERSON        	REG AIR   	 haggle. furiously fluffy requests ac
3554	192	3	3	41	44779.79	0.02	0.01	N	O	1995-07-13	1995-08-28	1995-07-27	DELIVER IN PERSON        	MAIL      	ent dependencies. sly
3557	175	3	1	41	44081.97	0.01	0.07	R	F	1993-01-30	1992-12-31	1993-02-18	COLLECT COD              	FOB       	ideas breach c
3557	129	10	2	37	38077.44	0.03	0.05	R	F	1993-02-16	1993-01-05	1993-03-15	DELIVER IN PERSON        	RAIL      	gside of the ca
3586	194	7	1	2	2188.38	0.03	0.08	R	F	1994-02-10	1994-01-07	1994-03-03	DELIVER IN PERSON        	RAIL      	he even, unusual decoy
3586	84	5	2	29	28538.32	0.04	0.07	R	F	1994-03-06	1994-03-02	1994-03-13	DELIVER IN PERSON        	RAIL      	 slyly unusual i
3586	58	3	3	2	1916.10	0.03	0.06	R	F	1994-03-22	1994-02-20	1994-04-08	NONE                     	REG AIR   	unts. slyly final ideas agai
3586	84	5	4	33	32474.64	0.06	0.01	R	F	1994-01-24	1994-02-09	1994-02-07	NONE                     	TRUCK     	refully across the fur
3586	108	1	5	8	8064.80	0.06	0.02	A	F	1994-03-29	1994-02-26	1994-04-02	NONE                     	FOB       	theodolites hagg
3586	99	1	6	8	7992.72	0.09	0.01	A	F	1994-03-18	1994-01-17	1994-04-06	DELIVER IN PERSON        	RAIL      	 ironic pinto beans cajole carefully theo
3586	123	4	7	33	33762.96	0.05	0.04	A	F	1994-02-11	1994-01-15	1994-03-03	NONE                     	REG AIR   	iously regular pinto beans integrate
3587	197	10	1	5	5485.95	0.09	0.07	N	O	1996-09-03	1996-07-05	1996-09-11	DELIVER IN PERSON        	SHIP      	ithely regular decoys above the 
3587	132	8	2	48	49542.24	0.00	0.03	N	O	1996-08-02	1996-07-02	1996-08-05	TAKE BACK RETURN         	MAIL      	beans. blithely final depe
3587	151	3	3	36	37841.40	0.05	0.05	N	O	1996-07-26	1996-06-16	1996-08-23	TAKE BACK RETURN         	MAIL      	ully regular excuse
3587	124	9	4	31	31747.72	0.03	0.01	N	O	1996-07-21	1996-07-01	1996-07-23	COLLECT COD              	SHIP      	press fluffily regul
3587	70	7	5	12	11640.84	0.06	0.03	N	O	1996-08-30	1996-07-04	1996-09-22	DELIVER IN PERSON        	RAIL      	g the even pinto beans. special,
3587	107	2	6	16	16113.60	0.01	0.03	N	O	1996-05-11	1996-06-19	1996-06-04	COLLECT COD              	FOB       	y ruthless dolphins to 
3587	74	2	7	23	22403.61	0.07	0.05	N	O	1996-08-30	1996-07-01	1996-09-10	COLLECT COD              	FOB       	l multipliers sleep theodolites-- slyly 
3589	37	3	1	42	39355.26	0.08	0.08	R	F	1994-08-11	1994-07-17	1994-08-23	DELIVER IN PERSON        	AIR       	he blithely unusual pac
3590	176	6	1	10	10761.70	0.08	0.00	N	O	1995-07-17	1995-06-26	1995-08-12	TAKE BACK RETURN         	SHIP      	t the quickly ironic
3590	95	6	2	19	18906.71	0.03	0.03	N	O	1995-08-02	1995-06-20	1995-08-08	NONE                     	SHIP      	special pinto beans. blithely reg
3590	96	9	3	43	42831.87	0.07	0.06	N	O	1995-07-12	1995-07-25	1995-07-16	DELIVER IN PERSON        	SHIP      	s could have to use
3590	56	8	4	26	24857.30	0.01	0.03	N	O	1995-07-08	1995-06-17	1995-08-02	DELIVER IN PERSON        	SHIP      	arefully along th
3590	191	2	5	37	40374.03	0.00	0.08	N	O	1995-09-01	1995-06-29	1995-09-10	NONE                     	SHIP      	ccounts above the silent waters thrash f
3590	119	10	6	31	31592.41	0.03	0.01	N	O	1995-06-24	1995-07-12	1995-06-25	DELIVER IN PERSON        	REG AIR   	ve furiously final instructions. slyly regu
3590	194	7	7	44	48144.36	0.05	0.04	N	F	1995-06-07	1995-06-15	1995-06-27	NONE                     	MAIL      	s sleep after the regular platelets. blit
3619	96	7	1	49	48808.41	0.01	0.08	N	O	1997-01-22	1996-12-21	1997-02-17	TAKE BACK RETURN         	MAIL      	 waters. furiously even deposits 
3619	116	10	2	27	27434.97	0.08	0.04	N	O	1996-12-12	1997-01-18	1996-12-18	TAKE BACK RETURN         	SHIP      	pecial accounts haggle care
3619	48	7	3	46	43609.84	0.08	0.03	N	O	1997-01-31	1997-01-27	1997-02-11	NONE                     	SHIP      	press, expres
3619	93	6	4	18	17875.62	0.04	0.02	N	O	1997-03-18	1996-12-24	1997-03-21	COLLECT COD              	AIR       	eodolites 
3619	120	10	5	38	38764.56	0.05	0.08	N	O	1996-12-08	1997-02-03	1997-01-07	NONE                     	RAIL      	theodolites detect abo
3619	152	3	6	43	45242.45	0.01	0.01	N	O	1997-01-25	1997-01-06	1997-02-07	COLLECT COD              	RAIL      	 bold, even
3620	59	7	1	41	39321.05	0.03	0.08	N	O	1997-03-21	1997-04-20	1997-03-30	COLLECT COD              	FOB       	t attainments cajole qui
3620	167	4	2	16	17074.56	0.00	0.06	N	O	1997-05-17	1997-05-08	1997-06-03	COLLECT COD              	SHIP      	s. even, pending in
3648	144	5	1	16	16706.24	0.02	0.06	A	F	1993-08-14	1993-08-14	1993-08-15	COLLECT COD              	FOB       	s nag packages.
3648	105	2	2	30	30153.00	0.00	0.01	R	F	1993-08-31	1993-09-06	1993-09-06	DELIVER IN PERSON        	FOB       	 above the somas boost furious
3648	46	7	3	34	32165.36	0.10	0.00	A	F	1993-08-21	1993-07-25	1993-09-15	DELIVER IN PERSON        	FOB       	 deposits are furiously. careful, 
3648	13	10	4	16	14608.16	0.06	0.03	R	F	1993-07-27	1993-08-26	1993-08-24	DELIVER IN PERSON        	FOB       	uriously stealthy deposits haggle furi
3648	117	7	5	25	25427.75	0.06	0.03	R	F	1993-08-15	1993-08-25	1993-09-09	TAKE BACK RETURN         	TRUCK     	s requests. silent asymp
3648	169	10	6	14	14968.24	0.08	0.06	R	F	1993-10-02	1993-08-26	1993-10-09	COLLECT COD              	AIR       	sly pending excuses. carefully i
3648	195	6	7	49	53664.31	0.09	0.03	R	F	1993-06-27	1993-07-27	1993-07-24	TAKE BACK RETURN         	FOB       	egular instructions. slyly regular pinto
3651	19	9	1	20	18380.20	0.01	0.04	N	O	1998-06-10	1998-06-06	1998-06-23	NONE                     	SHIP      	tect quickly among the r
3651	155	7	2	24	25323.60	0.09	0.04	N	O	1998-06-22	1998-07-17	1998-07-10	DELIVER IN PERSON        	RAIL      	excuses haggle according to th
3651	113	10	3	41	41537.51	0.00	0.05	N	O	1998-05-10	1998-07-09	1998-05-13	NONE                     	RAIL      	blithely. furiously 
3651	110	5	4	27	27272.97	0.05	0.03	N	O	1998-05-03	1998-06-30	1998-05-05	DELIVER IN PERSON        	RAIL      	 sleep blithely furiously do
3652	180	8	1	24	25924.32	0.05	0.03	N	O	1997-06-07	1997-04-07	1997-06-12	COLLECT COD              	MAIL      	the final p
3652	137	8	2	37	38373.81	0.02	0.05	N	O	1997-05-11	1997-04-06	1997-06-05	COLLECT COD              	MAIL      	osits haggle carefu
3652	163	8	3	39	41463.24	0.01	0.02	N	O	1997-03-10	1997-04-03	1997-03-21	NONE                     	REG AIR   	y express instructions. un
3652	80	9	4	1	980.08	0.01	0.04	N	O	1997-04-20	1997-05-03	1997-05-18	DELIVER IN PERSON        	SHIP      	 bold dependencies sublate. r
3654	165	2	1	46	48997.36	0.08	0.05	A	F	1992-06-05	1992-08-19	1992-06-06	DELIVER IN PERSON        	FOB       	usly regular foxes. furio
3654	93	4	2	29	28799.61	0.07	0.06	A	F	1992-09-11	1992-07-20	1992-10-04	DELIVER IN PERSON        	FOB       	odolites detect. quickly r
3654	2	7	3	37	33374.00	0.07	0.05	A	F	1992-09-22	1992-07-20	1992-10-19	TAKE BACK RETURN         	RAIL      	unts doze bravely ab
3654	168	9	4	11	11749.76	0.08	0.00	A	F	1992-07-20	1992-07-30	1992-07-23	TAKE BACK RETURN         	SHIP      	quickly along the express, ironic req
3654	94	5	5	34	33799.06	0.04	0.00	R	F	1992-07-26	1992-08-26	1992-08-12	TAKE BACK RETURN         	REG AIR   	 the quick
3654	107	4	6	20	20142.00	0.03	0.02	A	F	1992-07-30	1992-07-05	1992-08-05	COLLECT COD              	SHIP      	s sleep about the slyly 
3654	173	1	7	45	48292.65	0.01	0.07	A	F	1992-09-15	1992-07-04	1992-09-20	DELIVER IN PERSON        	FOB       	sly ironic notornis nag slyly
3680	177	6	1	48	51704.16	0.00	0.06	R	F	1993-01-16	1993-01-23	1993-01-19	COLLECT COD              	FOB       	packages. quickly fluff
3680	5	8	2	41	37105.00	0.00	0.04	A	F	1993-01-06	1993-03-02	1993-01-08	NONE                     	FOB       	iously ironic platelets in
3680	56	4	3	33	31549.65	0.09	0.08	R	F	1993-03-16	1993-02-19	1993-04-05	NONE                     	FOB       	ts. ironic, fina
3682	61	10	1	6	5766.36	0.07	0.02	N	O	1997-05-06	1997-04-04	1997-05-11	NONE                     	AIR       	ronic deposits wake slyly. ca
3682	116	7	2	18	18289.98	0.06	0.06	N	O	1997-04-30	1997-03-21	1997-05-10	NONE                     	FOB       	regular dependencies
3682	47	10	3	17	16099.68	0.03	0.05	N	O	1997-02-12	1997-04-04	1997-02-22	COLLECT COD              	FOB       	, ironic packages wake a
3682	57	5	4	30	28711.50	0.09	0.05	N	O	1997-04-16	1997-04-16	1997-04-29	NONE                     	MAIL      	he requests cajole quickly pending package
3683	101	4	1	35	35038.50	0.05	0.03	A	F	1993-05-31	1993-04-17	1993-06-14	NONE                     	SHIP      	 the furiously expr
3683	49	8	2	41	38910.64	0.01	0.06	A	F	1993-03-26	1993-05-06	1993-04-09	NONE                     	TRUCK     	ress instructions. slyly express a
3683	100	3	3	23	23002.30	0.00	0.08	R	F	1993-07-02	1993-05-16	1993-07-30	NONE                     	TRUCK     	xpress accounts sleep slyly re
3687	145	4	1	32	33444.48	0.03	0.06	R	F	1993-05-07	1993-04-05	1993-05-25	DELIVER IN PERSON        	AIR       	deas cajole fo
3687	81	2	2	2	1962.16	0.00	0.08	R	F	1993-02-23	1993-03-25	1993-03-11	NONE                     	TRUCK     	 express requests. slyly regular depend
3687	174	4	3	10	10741.70	0.01	0.02	A	F	1993-02-11	1993-03-22	1993-03-09	NONE                     	FOB       	ing pinto beans
3687	162	9	4	19	20181.04	0.02	0.05	A	F	1993-05-14	1993-04-24	1993-06-01	DELIVER IN PERSON        	MAIL      	ly final asymptotes according to t
3687	119	9	5	31	31592.41	0.07	0.08	A	F	1993-05-28	1993-03-20	1993-06-05	DELIVER IN PERSON        	FOB       	foxes cajole quickly about the furiously f
3713	112	6	1	41	41496.51	0.07	0.08	N	O	1998-05-11	1998-07-17	1998-05-22	COLLECT COD              	RAIL      	eposits wake blithely fina
3713	177	7	2	19	20466.23	0.04	0.04	N	O	1998-06-25	1998-07-24	1998-07-08	DELIVER IN PERSON        	AIR       	tructions serve blithely around the furi
3713	180	1	3	19	20523.42	0.03	0.02	N	O	1998-05-19	1998-07-06	1998-06-09	DELIVER IN PERSON        	REG AIR   	quests cajole careful
3713	169	10	4	45	48112.20	0.06	0.04	N	O	1998-06-15	1998-07-30	1998-07-14	DELIVER IN PERSON        	MAIL      	al pinto beans affix after the slyly 
3713	90	1	5	46	45544.14	0.10	0.04	N	O	1998-08-22	1998-06-27	1998-08-31	NONE                     	MAIL      	totes. carefully special theodolites s
3713	182	3	6	29	31383.22	0.09	0.03	N	O	1998-08-04	1998-06-13	1998-08-21	NONE                     	RAIL      	the regular dugouts wake furiously sil
3713	130	1	7	14	14421.82	0.04	0.00	N	O	1998-07-19	1998-07-02	1998-07-28	DELIVER IN PERSON        	SHIP      	eposits impress according
3715	97	1	1	13	12962.17	0.00	0.03	N	O	1996-05-11	1996-04-25	1996-06-09	TAKE BACK RETURN         	SHIP      	e quickly ironic
3715	169	6	2	16	17106.56	0.01	0.06	N	O	1996-06-28	1996-04-22	1996-06-30	TAKE BACK RETURN         	AIR       	usly regular pearls haggle final packages
3715	12	3	3	37	33744.37	0.05	0.02	N	O	1996-05-03	1996-04-30	1996-05-17	NONE                     	SHIP      	ut the carefully expr
3716	32	8	1	10	9320.30	0.09	0.04	N	O	1997-12-02	1997-11-09	1997-12-14	TAKE BACK RETURN         	SHIP      	ts. quickly sly ideas slee
3716	194	5	2	39	42673.41	0.02	0.08	N	O	1997-11-27	1997-10-23	1997-12-24	COLLECT COD              	REG AIR   	even deposits.
3716	107	8	3	42	42298.20	0.02	0.08	N	O	1997-12-03	1997-10-12	1997-12-15	NONE                     	TRUCK     	 of the pend
3716	165	10	4	19	20238.04	0.05	0.08	N	O	1997-09-25	1997-10-18	1997-10-12	NONE                     	TRUCK     	arefully unusual accounts. flu
3716	182	3	5	25	27054.50	0.06	0.05	N	O	1997-11-23	1997-10-24	1997-11-24	COLLECT COD              	REG AIR   	fully unusual accounts. carefu
3718	21	10	1	40	36840.80	0.01	0.04	N	O	1996-11-20	1996-12-17	1996-12-03	DELIVER IN PERSON        	MAIL      	out the express deposits
3718	163	8	2	16	17010.56	0.02	0.06	N	O	1996-11-11	1996-12-25	1996-11-12	COLLECT COD              	TRUCK     	slyly even accounts. blithely special acco
3718	70	5	3	8	7760.56	0.05	0.03	N	O	1996-12-06	1996-12-06	1996-12-15	TAKE BACK RETURN         	AIR       	 the even deposits sleep carefully b
3719	22	5	1	35	32270.70	0.06	0.08	N	O	1997-06-11	1997-04-03	1997-06-15	TAKE BACK RETURN         	TRUCK     	ly foxes. pending braids haggle furio
3719	174	4	2	2	2148.34	0.02	0.08	N	O	1997-02-17	1997-04-25	1997-03-03	NONE                     	REG AIR   	ccounts boost carefu
3719	182	3	3	12	12986.16	0.05	0.06	N	O	1997-06-10	1997-05-04	1997-07-09	TAKE BACK RETURN         	REG AIR   	grate according to the 
3719	90	1	4	13	12871.17	0.02	0.00	N	O	1997-05-03	1997-04-16	1997-05-27	TAKE BACK RETURN         	SHIP      	iously. regular dep
3719	78	8	5	19	18583.33	0.06	0.08	N	O	1997-05-22	1997-03-20	1997-06-12	COLLECT COD              	TRUCK     	he regular ideas integrate acros
3719	142	5	6	43	44812.02	0.03	0.08	N	O	1997-05-08	1997-04-15	1997-06-06	COLLECT COD              	RAIL      	the furiously special pinto bean
3719	19	10	7	16	14704.16	0.10	0.01	N	O	1997-03-02	1997-03-18	1997-03-28	TAKE BACK RETURN         	RAIL      	 express asymptotes. ir
3744	195	8	1	30	32855.70	0.05	0.06	A	F	1992-05-07	1992-02-12	1992-05-17	TAKE BACK RETURN         	FOB       	nts among 
3745	137	8	1	18	18668.34	0.01	0.05	A	F	1993-10-17	1993-11-16	1993-11-13	DELIVER IN PERSON        	SHIP      	 slyly bold pinto beans according to 
3747	141	10	1	42	43727.88	0.05	0.05	N	O	1996-11-10	1996-10-19	1996-11-19	TAKE BACK RETURN         	REG AIR   	y. blithely fina
3747	170	1	2	33	35315.61	0.01	0.03	N	O	1996-10-14	1996-11-12	1996-11-11	NONE                     	REG AIR   	 regular p
3747	139	10	3	30	31173.90	0.00	0.07	N	O	1996-12-16	1996-11-15	1996-12-17	NONE                     	RAIL      	! furiously f
3747	33	9	4	21	19593.63	0.00	0.06	N	O	1996-11-18	1996-09-23	1996-11-26	TAKE BACK RETURN         	AIR       	ithely bold orbits mold furiously blit
3747	126	5	5	32	32835.84	0.08	0.05	N	O	1996-09-10	1996-11-04	1996-10-10	DELIVER IN PERSON        	MAIL      	quests shall h
3747	154	5	6	14	14758.10	0.08	0.07	N	O	1996-11-03	1996-10-29	1996-11-06	TAKE BACK RETURN         	AIR       	packages cajole carefu
3747	118	2	7	23	23416.53	0.00	0.04	N	O	1996-11-08	1996-11-10	1996-12-03	NONE                     	REG AIR   	kages are ironic
3748	104	7	1	12	12049.20	0.06	0.01	N	O	1998-04-17	1998-04-15	1998-05-12	NONE                     	AIR       	old reques
3748	165	4	2	24	25563.84	0.08	0.04	N	O	1998-06-07	1998-05-02	1998-06-21	DELIVER IN PERSON        	TRUCK     	al deposits. blithely
3748	197	1	3	19	20846.61	0.05	0.01	N	O	1998-04-23	1998-05-17	1998-05-23	COLLECT COD              	RAIL      	pinto beans run carefully quic
3748	187	8	4	5	5435.90	0.00	0.07	N	O	1998-06-29	1998-05-06	1998-07-12	DELIVER IN PERSON        	MAIL      	 regular accounts sleep quickly-- furious
3748	147	4	5	21	21989.94	0.07	0.08	N	O	1998-03-30	1998-04-07	1998-04-05	TAKE BACK RETURN         	MAIL      	fix carefully furiously express ideas. furi
3750	134	10	1	37	38262.81	0.04	0.03	N	O	1995-07-08	1995-07-28	1995-07-28	DELIVER IN PERSON        	REG AIR   	usly busy account
3750	152	3	2	33	34720.95	0.05	0.03	N	O	1995-06-27	1995-06-20	1995-07-03	TAKE BACK RETURN         	REG AIR   	theodolites haggle. slyly pendin
3750	80	10	3	20	19601.60	0.09	0.05	N	F	1995-06-17	1995-06-06	1995-06-28	TAKE BACK RETURN         	REG AIR   	ss, ironic requests! fur
3750	166	1	4	33	35183.28	0.04	0.03	N	F	1995-06-15	1995-06-04	1995-06-29	COLLECT COD              	RAIL      	ep blithely according to the flu
3750	83	4	5	1	983.08	0.05	0.01	N	O	1995-07-24	1995-06-25	1995-08-21	DELIVER IN PERSON        	REG AIR   	l dolphins against the slyly
3750	113	7	6	47	47616.17	0.01	0.08	R	F	1995-05-11	1995-06-13	1995-06-02	TAKE BACK RETURN         	FOB       	slowly regular accounts. blithely ev
3782	27	10	1	29	26883.58	0.01	0.07	N	O	1996-09-17	1996-10-03	1996-10-07	DELIVER IN PERSON        	REG AIR   	quickly unusual pinto beans. carefully fina
3782	153	1	2	10	10531.50	0.03	0.05	N	O	1996-09-07	1996-11-19	1996-10-04	COLLECT COD              	FOB       	ven pinto b
3782	136	7	3	30	31083.90	0.06	0.06	N	O	1996-12-19	1996-10-31	1997-01-14	TAKE BACK RETURN         	MAIL      	slyly even pinto beans hag
3782	117	7	4	34	34581.74	0.02	0.06	N	O	1996-11-07	1996-10-22	1996-11-19	DELIVER IN PERSON        	MAIL      	gage after the even
3782	130	3	5	40	41205.20	0.09	0.04	N	O	1996-12-16	1996-11-22	1997-01-01	COLLECT COD              	AIR       	s instructions. regular accou
3783	167	4	1	36	38417.76	0.04	0.08	R	F	1993-12-17	1994-02-26	1994-01-03	DELIVER IN PERSON        	SHIP      	ites haggle among the carefully unusu
3783	73	3	2	36	35030.52	0.02	0.02	R	F	1994-03-02	1994-02-09	1994-03-15	COLLECT COD              	TRUCK     	egular accounts
3783	85	6	3	50	49254.00	0.04	0.01	R	F	1994-03-14	1994-01-09	1994-04-10	DELIVER IN PERSON        	FOB       	he furiously regular deposits. 
3783	27	6	4	37	34299.74	0.10	0.05	R	F	1993-12-09	1994-02-17	1993-12-30	COLLECT COD              	REG AIR   	ing to the ideas. regular accounts de
3808	43	10	1	28	26405.12	0.02	0.01	R	F	1994-05-27	1994-06-18	1994-06-22	TAKE BACK RETURN         	FOB       	lly final accounts alo
3808	127	6	2	47	48274.64	0.04	0.08	R	F	1994-06-12	1994-06-03	1994-07-02	COLLECT COD              	TRUCK     	fully for the quickly final deposits: flu
3808	31	2	3	45	41896.35	0.00	0.03	R	F	1994-07-03	1994-05-29	1994-07-14	TAKE BACK RETURN         	REG AIR   	 carefully special
3808	100	1	4	34	34003.40	0.07	0.04	R	F	1994-08-13	1994-07-22	1994-08-31	DELIVER IN PERSON        	FOB       	 pearls will have to 
3808	155	7	5	29	30599.35	0.08	0.03	A	F	1994-06-22	1994-05-26	1994-07-06	TAKE BACK RETURN         	TRUCK     	 deposits across the pac
3808	168	5	6	44	46999.04	0.06	0.06	A	F	1994-06-07	1994-06-04	1994-06-25	NONE                     	REG AIR   	the blithely regular foxes. even, final 
3810	184	5	1	49	53124.82	0.05	0.01	R	F	1992-11-27	1992-10-30	1992-12-16	COLLECT COD              	AIR       	cajole. fur
3810	169	8	2	18	19244.88	0.01	0.04	A	F	1992-11-28	1992-11-15	1992-12-27	DELIVER IN PERSON        	SHIP      	s. furiously careful deposi
3810	137	3	3	41	42522.33	0.08	0.08	A	F	1992-10-26	1992-10-27	1992-11-05	COLLECT COD              	SHIP      	l requests boost slyly along the slyl
3810	182	3	4	11	11903.98	0.06	0.04	A	F	1992-12-18	1992-12-11	1993-01-15	DELIVER IN PERSON        	MAIL      	 the pending pinto beans. expr
3814	131	7	1	7	7217.91	0.02	0.02	R	F	1995-05-01	1995-05-09	1995-05-28	DELIVER IN PERSON        	REG AIR   	es sleep furiou
3814	173	3	2	14	15024.38	0.01	0.00	R	F	1995-03-17	1995-05-10	1995-04-16	DELIVER IN PERSON        	AIR       	sits along the final, ironic deposit
3814	168	7	3	36	38453.76	0.06	0.02	N	O	1995-06-19	1995-04-18	1995-06-28	COLLECT COD              	SHIP      	beans cajole quickly sl
3814	66	7	4	20	19321.20	0.04	0.07	R	F	1995-02-23	1995-03-26	1995-03-04	DELIVER IN PERSON        	SHIP      	. doggedly ironic deposits will have to wa
3814	107	2	5	15	15106.50	0.03	0.04	N	O	1995-06-23	1995-03-25	1995-07-09	COLLECT COD              	SHIP      	 carefully final deposits haggle slyly
3814	83	4	6	47	46204.76	0.09	0.05	A	F	1995-04-16	1995-04-03	1995-05-14	DELIVER IN PERSON        	AIR       	nusual requests. bli
3814	132	8	7	12	12385.56	0.10	0.01	R	F	1995-03-18	1995-04-16	1995-03-20	TAKE BACK RETURN         	REG AIR   	ages cajole. packages haggle. final
3840	187	8	1	45	48923.10	0.02	0.08	N	O	1998-10-31	1998-09-19	1998-11-30	DELIVER IN PERSON        	TRUCK     	o beans are. carefully final courts x
3840	46	9	2	12	11352.48	0.04	0.07	N	O	1998-10-02	1998-08-19	1998-10-20	TAKE BACK RETURN         	RAIL      	xpress pinto beans. accounts a
3840	73	4	3	45	43788.15	0.02	0.05	N	O	1998-10-12	1998-10-12	1998-10-28	TAKE BACK RETURN         	FOB       	onic, even packages are. pe
3840	148	9	4	41	42973.74	0.07	0.02	N	O	1998-07-21	1998-10-08	1998-08-01	TAKE BACK RETURN         	MAIL      	 nag slyly? slyly pending accounts 
3840	173	3	5	7	7512.19	0.09	0.08	N	O	1998-09-17	1998-09-20	1998-10-14	DELIVER IN PERSON        	MAIL      	. furiously final gifts sleep carefully pin
3840	107	8	6	33	33234.30	0.10	0.02	N	O	1998-07-29	1998-10-06	1998-08-04	DELIVER IN PERSON        	SHIP      	hely silent deposits w
3841	157	5	1	1	1057.15	0.06	0.03	A	F	1994-10-10	1994-11-12	1994-10-21	DELIVER IN PERSON        	AIR       	 boost even re
3841	21	10	2	31	28551.62	0.09	0.03	A	F	1995-01-24	1994-11-25	1995-02-20	TAKE BACK RETURN         	SHIP      	n theodolites shall promise carefully. qui
3841	152	10	3	40	42086.00	0.06	0.02	A	F	1995-02-02	1994-11-30	1995-02-14	TAKE BACK RETURN         	MAIL      	its. quickly regular ideas nag carefully
3841	50	1	4	9	8550.45	0.10	0.07	A	F	1994-11-21	1994-12-26	1994-11-26	NONE                     	FOB       	s according to the courts shall nag s
3841	176	7	5	3	3228.51	0.04	0.02	R	F	1994-10-24	1994-12-07	1994-11-09	COLLECT COD              	FOB       	foxes integrate 
3841	163	8	6	48	51031.68	0.03	0.00	R	F	1994-11-23	1994-11-22	1994-12-01	DELIVER IN PERSON        	FOB       	 according to the regular, 
3844	135	1	1	2	2070.26	0.03	0.07	R	F	1995-02-24	1995-02-03	1995-03-18	TAKE BACK RETURN         	AIR       	es haggle final acco
3844	102	7	2	5	5010.50	0.10	0.03	R	F	1995-04-29	1995-02-24	1995-05-05	TAKE BACK RETURN         	RAIL      	 unwind quickly about the pending, i
3845	34	5	1	44	41097.32	0.01	0.08	A	F	1992-07-20	1992-07-15	1992-07-24	DELIVER IN PERSON        	REG AIR   	s haggle among the fluffily regula
3845	24	7	2	16	14784.32	0.09	0.05	A	F	1992-08-08	1992-06-08	1992-08-26	DELIVER IN PERSON        	SHIP      	ely bold ideas use. ex
3845	59	1	3	17	16303.85	0.08	0.01	A	F	1992-06-12	1992-07-05	1992-06-26	TAKE BACK RETURN         	RAIL      	counts haggle. reg
3845	46	9	4	1	946.04	0.04	0.05	R	F	1992-05-21	1992-06-07	1992-06-17	COLLECT COD              	REG AIR   	 blithely ironic t
3845	196	7	5	27	29597.13	0.00	0.05	R	F	1992-08-20	1992-07-17	1992-09-02	COLLECT COD              	REG AIR   	kages. care
3845	105	8	6	30	30153.00	0.09	0.06	R	F	1992-08-21	1992-07-07	1992-08-25	COLLECT COD              	FOB       	counts do wake blithely. ironic requests 
3847	189	10	1	7	7624.26	0.08	0.00	A	F	1993-05-06	1993-06-06	1993-05-22	COLLECT COD              	MAIL      	 about the blithely daring Tiresias. fl
3875	81	2	1	24	23545.92	0.02	0.08	N	O	1997-10-15	1997-11-27	1997-11-09	COLLECT COD              	AIR       	ecial packages. 
3875	113	7	2	49	49642.39	0.04	0.04	N	O	1997-10-18	1997-10-13	1997-10-19	NONE                     	MAIL      	sleep furiously about the deposits. quickl
3878	200	1	1	6	6601.20	0.07	0.04	N	O	1997-06-21	1997-05-22	1997-07-01	COLLECT COD              	FOB       	s. regular instru
3878	88	9	2	13	12845.04	0.01	0.06	N	O	1997-06-08	1997-06-03	1997-06-25	TAKE BACK RETURN         	TRUCK     	leep ruthlessly about the carefu
3878	41	8	3	20	18820.80	0.08	0.03	N	O	1997-06-20	1997-05-24	1997-07-20	TAKE BACK RETURN         	MAIL      	the furiously careful ideas cajole slyly sl
3878	152	3	4	20	21043.00	0.01	0.07	N	O	1997-07-13	1997-05-22	1997-07-20	NONE                     	FOB       	about the carefully ironic pa
3904	38	4	1	22	20636.66	0.04	0.03	N	O	1998-02-02	1998-02-09	1998-02-10	TAKE BACK RETURN         	REG AIR   	structions cajole carefully. carefully f
3904	184	5	2	19	20599.42	0.09	0.01	N	O	1998-02-10	1998-02-13	1998-02-20	TAKE BACK RETURN         	AIR       	 excuses sleep slyly according to th
3908	92	4	1	50	49604.50	0.05	0.04	R	F	1993-06-19	1993-04-27	1993-07-05	DELIVER IN PERSON        	MAIL      	 even accounts wake 
3908	148	9	2	8	8385.12	0.06	0.03	A	F	1993-03-12	1993-04-13	1993-03-22	DELIVER IN PERSON        	SHIP      	r instructions was requests. ironically 
3936	137	8	1	25	25928.25	0.06	0.03	N	O	1996-12-03	1996-12-27	1997-01-01	DELIVER IN PERSON        	RAIL      	gular requests nag quic
3936	188	9	2	24	26116.32	0.10	0.07	N	O	1996-11-22	1997-01-01	1996-12-08	NONE                     	AIR       	ns. accounts mold fl
3936	83	4	3	42	41289.36	0.00	0.07	N	O	1997-01-03	1997-01-29	1997-01-14	COLLECT COD              	AIR       	elets wake amo
3936	62	7	4	12	11544.72	0.06	0.05	N	O	1996-11-25	1997-01-09	1996-12-06	DELIVER IN PERSON        	SHIP      	ithely across the carefully brave req
3936	84	5	5	35	34442.80	0.02	0.08	N	O	1996-12-04	1997-01-06	1996-12-22	NONE                     	SHIP      	lly ironic requ
3936	103	6	6	26	26080.60	0.01	0.02	N	O	1997-02-27	1997-01-16	1997-03-22	NONE                     	RAIL      	quickly pen
3938	159	4	1	46	48720.90	0.10	0.07	R	F	1993-05-20	1993-05-04	1993-06-12	DELIVER IN PERSON        	FOB       	ly even foxes are slyly fu
3939	160	8	1	8	8481.28	0.03	0.06	N	O	1996-01-29	1996-04-05	1996-02-26	COLLECT COD              	REG AIR   	e packages. express, pen
3940	178	7	1	33	35579.61	0.10	0.07	N	O	1996-05-19	1996-04-19	1996-05-23	TAKE BACK RETURN         	RAIL      	ly ironic packages about the pending accou
3940	69	4	2	40	38762.40	0.08	0.02	N	O	1996-02-29	1996-03-22	1996-03-04	NONE                     	MAIL      	ts. regular fox
3940	89	10	3	8	7912.64	0.07	0.08	N	O	1996-04-04	1996-04-12	1996-04-18	DELIVER IN PERSON        	RAIL      	ions cajole furiously regular pinto beans. 
3940	137	3	4	11	11408.43	0.09	0.05	N	O	1996-03-09	1996-05-13	1996-03-17	COLLECT COD              	REG AIR   	e of the special packages. furiously
3940	1	6	5	41	36941.00	0.00	0.07	N	O	1996-05-08	1996-05-03	1996-06-03	COLLECT COD              	MAIL      	thily. deposits cajole.
3972	51	3	1	2	1902.10	0.05	0.03	A	F	1994-07-24	1994-06-30	1994-08-13	TAKE BACK RETURN         	SHIP      	y final theodolite
3975	57	9	1	38	36367.90	0.01	0.05	N	O	1995-08-02	1995-06-18	1995-08-19	COLLECT COD              	TRUCK     	es are furiously: furi
4000	196	7	1	41	44943.79	0.06	0.01	A	F	1992-03-02	1992-03-14	1992-03-27	COLLECT COD              	FOB       	ve the even, fi
4000	75	5	2	44	42903.08	0.09	0.06	A	F	1992-03-27	1992-02-18	1992-03-31	COLLECT COD              	AIR       	equests use blithely blithely bold d
4003	52	4	1	18	17136.90	0.04	0.07	R	F	1993-02-02	1993-04-15	1993-02-28	TAKE BACK RETURN         	AIR       	ar grouches s
4004	121	2	1	23	23485.76	0.07	0.02	A	F	1993-08-12	1993-07-13	1993-08-16	TAKE BACK RETURN         	TRUCK     	 bold theodolites? special packages accordi
4004	64	5	2	47	45310.82	0.07	0.04	R	F	1993-06-25	1993-08-03	1993-07-12	NONE                     	SHIP      	thely instead of the even, unu
4004	114	5	3	39	39550.29	0.10	0.05	R	F	1993-07-12	1993-07-27	1993-07-18	NONE                     	MAIL      	ccounts sleep furious
4004	74	4	4	46	44807.22	0.10	0.04	R	F	1993-09-04	1993-07-13	1993-09-28	COLLECT COD              	FOB       	ncies. slyly pending dolphins sleep furio
4004	155	3	5	9	9496.35	0.04	0.06	A	F	1993-08-25	1993-06-10	1993-09-24	COLLECT COD              	MAIL      	ly ironic requests. quickly pending ide
4004	161	10	6	44	46691.04	0.07	0.05	R	F	1993-07-25	1993-07-23	1993-08-16	TAKE BACK RETURN         	REG AIR   	ut the sauternes. bold, ironi
4004	126	9	7	20	20522.40	0.07	0.05	A	F	1993-06-19	1993-06-14	1993-07-04	COLLECT COD              	REG AIR   	. ironic deposits cajole blithely?
4006	55	7	1	11	10505.55	0.05	0.08	A	F	1995-04-29	1995-02-21	1995-05-20	TAKE BACK RETURN         	RAIL      	ress foxes cajole quick
4006	159	4	2	18	19064.70	0.05	0.03	A	F	1995-01-29	1995-03-08	1995-02-02	TAKE BACK RETURN         	MAIL      	gouts! slyly iron
4006	24	5	3	15	13860.30	0.01	0.02	R	F	1995-02-23	1995-04-02	1995-02-25	TAKE BACK RETURN         	RAIL      	n deposits cajole slyl
4006	114	5	4	25	25352.75	0.00	0.07	A	F	1995-02-23	1995-02-09	1995-02-24	DELIVER IN PERSON        	SHIP      	 requests use depos
4007	57	2	1	32	30625.60	0.00	0.03	R	F	1993-09-30	1993-08-16	1993-10-03	DELIVER IN PERSON        	RAIL      	nal accounts across t
4007	116	10	2	41	41660.51	0.04	0.06	A	F	1993-10-11	1993-08-30	1993-11-04	DELIVER IN PERSON        	TRUCK     	eposits. regular epitaphs boost blithely.
4007	102	9	3	5	5010.50	0.09	0.06	A	F	1993-09-17	1993-08-29	1993-10-12	TAKE BACK RETURN         	FOB       	y unusual packa
4007	138	4	4	15	15571.95	0.05	0.02	A	F	1993-09-01	1993-07-19	1993-09-03	DELIVER IN PERSON        	FOB       	le furiously quickly 
4007	26	7	5	23	21298.46	0.02	0.07	A	F	1993-10-08	1993-09-09	1993-10-23	COLLECT COD              	MAIL      	ter the accounts. expr
4033	110	1	1	27	27272.97	0.01	0.04	R	F	1993-08-08	1993-08-14	1993-08-09	NONE                     	AIR       	pinto beans
4033	38	4	2	34	31893.02	0.07	0.00	R	F	1993-07-19	1993-08-05	1993-07-26	NONE                     	RAIL      	t the blithely dogg
4035	97	8	1	4	3988.36	0.08	0.03	R	F	1992-04-21	1992-04-23	1992-04-25	COLLECT COD              	AIR       	ilent, even pear
4035	136	7	2	4	4144.52	0.07	0.00	A	F	1992-05-21	1992-04-24	1992-05-24	DELIVER IN PERSON        	FOB       	en instructions sleep blith
4035	118	8	3	1	1018.11	0.03	0.01	R	F	1992-06-18	1992-05-19	1992-07-02	COLLECT COD              	FOB       	 requests. quickly 
4035	182	3	4	13	14068.34	0.00	0.01	R	F	1992-06-10	1992-05-16	1992-07-10	NONE                     	SHIP      	s. furiously even courts wake slyly
4037	64	9	1	32	30849.92	0.00	0.06	A	F	1993-05-06	1993-06-08	1993-05-31	DELIVER IN PERSON        	AIR       	e of the pending, iron
4037	47	8	2	4	3788.16	0.09	0.07	A	F	1993-07-05	1993-06-12	1993-08-03	DELIVER IN PERSON        	RAIL      	s around the blithely ironic ac
4066	139	5	1	9	9352.17	0.01	0.05	N	O	1997-05-06	1997-03-25	1997-05-27	COLLECT COD              	FOB       	nal, ironic accounts. blithel
4066	93	5	2	19	18868.71	0.05	0.00	N	O	1997-05-13	1997-04-17	1997-06-08	NONE                     	TRUCK     	quests. slyly regu
4066	76	5	3	8	7808.56	0.03	0.03	N	O	1997-04-24	1997-03-11	1997-05-20	NONE                     	REG AIR   	accounts. special pinto beans
4066	179	9	4	49	52879.33	0.01	0.01	N	O	1997-02-17	1997-03-24	1997-02-19	NONE                     	TRUCK     	ial braids. furiously final deposits sl
4066	171	2	5	43	46060.31	0.05	0.02	N	O	1997-02-16	1997-04-14	1997-02-18	DELIVER IN PERSON        	MAIL      	r instructions. slyly special 
4066	109	2	6	44	44400.40	0.01	0.00	N	O	1997-03-01	1997-04-27	1997-03-29	DELIVER IN PERSON        	MAIL      	express accounts nag bli
4068	110	1	1	43	43434.73	0.05	0.06	N	O	1996-11-28	1996-11-16	1996-12-22	NONE                     	AIR       	ructions. regular, special packag
4068	57	5	2	31	29668.55	0.08	0.03	N	O	1996-12-11	1996-12-07	1996-12-30	NONE                     	SHIP      	ds wake carefully amon
4129	56	8	1	32	30593.60	0.03	0.04	A	F	1993-09-16	1993-08-25	1993-09-25	TAKE BACK RETURN         	MAIL      	ckages haggl
4129	27	6	2	39	36153.78	0.06	0.07	R	F	1993-10-21	1993-08-04	1993-10-29	COLLECT COD              	MAIL      	y regular foxes. slyly ironic deposits 
4131	50	7	1	6	5700.30	0.05	0.01	N	O	1998-04-27	1998-04-18	1998-04-29	TAKE BACK RETURN         	MAIL      	ns cajole slyly. even, iro
4131	178	8	2	32	34501.44	0.08	0.01	N	O	1998-03-02	1998-03-21	1998-03-07	TAKE BACK RETURN         	TRUCK     	 furiously regular asymptotes nod sly
4131	26	9	3	25	23150.50	0.02	0.07	N	O	1998-02-24	1998-03-01	1998-02-27	TAKE BACK RETURN         	FOB       	uickly exp
4131	36	7	4	8	7488.24	0.04	0.01	N	O	1998-03-03	1998-03-15	1998-03-26	COLLECT COD              	FOB       	 after the furiously ironic d
4131	125	6	5	30	30753.60	0.01	0.01	N	O	1998-04-01	1998-04-13	1998-04-08	TAKE BACK RETURN         	FOB       	he fluffily express depen
4131	102	7	6	47	47098.70	0.02	0.00	N	O	1998-03-09	1998-04-05	1998-03-13	TAKE BACK RETURN         	RAIL      	ges. ironic pinto be
4132	138	4	1	28	29067.64	0.07	0.03	N	O	1995-08-16	1995-08-01	1995-08-29	TAKE BACK RETURN         	SHIP      	pths wake against the stealthily special pi
4132	15	5	2	23	21045.23	0.07	0.07	N	O	1995-06-27	1995-07-27	1995-07-13	TAKE BACK RETURN         	FOB       	d deposits. fluffily even requests haggle b
4132	87	8	3	18	17767.44	0.09	0.04	A	F	1995-06-01	1995-08-01	1995-06-02	TAKE BACK RETURN         	RAIL      	y final de
4134	121	4	1	34	34718.08	0.02	0.05	R	F	1995-04-29	1995-03-13	1995-05-11	DELIVER IN PERSON        	FOB       	e furiously regular sheaves sleep
4134	96	10	2	34	33867.06	0.01	0.03	A	F	1995-05-06	1995-03-28	1995-05-13	DELIVER IN PERSON        	SHIP      	ual asymptotes wake carefully alo
4134	171	9	3	12	12854.04	0.05	0.04	A	F	1995-03-19	1995-03-27	1995-04-14	COLLECT COD              	TRUCK     	kly above the quickly regular 
4134	100	4	4	45	45004.50	0.08	0.02	A	F	1995-04-11	1995-03-27	1995-04-17	TAKE BACK RETURN         	MAIL      	ironic pin
4161	122	7	1	12	12265.44	0.08	0.02	R	F	1993-08-25	1993-10-04	1993-09-22	COLLECT COD              	RAIL      	onic dolphins. in
4161	28	3	2	47	43616.94	0.05	0.00	A	F	1993-12-20	1993-10-29	1994-01-19	TAKE BACK RETURN         	RAIL      	r requests about the final, even foxes hag
4161	138	4	3	42	43601.46	0.03	0.04	R	F	1993-11-12	1993-10-04	1993-11-27	COLLECT COD              	MAIL      	thely across the even attainments. express
4161	10	5	4	45	40950.45	0.02	0.06	A	F	1993-10-22	1993-10-17	1993-10-30	COLLECT COD              	REG AIR   	about the ironic packages cajole blithe
4161	29	10	5	46	42734.92	0.05	0.01	A	F	1993-11-09	1993-11-17	1993-11-17	TAKE BACK RETURN         	TRUCK     	he stealthily ironic foxes. ideas haggl
4161	148	9	6	19	19914.66	0.07	0.00	R	F	1993-08-22	1993-11-11	1993-09-01	TAKE BACK RETURN         	REG AIR   	beans breach s
4166	141	10	1	8	8329.12	0.00	0.08	A	F	1993-06-05	1993-04-10	1993-07-05	COLLECT COD              	MAIL      	uickly. blithely pending de
4166	93	5	2	8	7944.72	0.06	0.04	A	F	1993-06-07	1993-04-17	1993-06-16	DELIVER IN PERSON        	REG AIR   	es along the furiously regular acc
4166	7	10	3	17	15419.00	0.02	0.06	R	F	1993-06-29	1993-05-15	1993-07-24	DELIVER IN PERSON        	SHIP      	ackages. re
4166	86	7	4	36	35498.88	0.06	0.05	R	F	1993-03-01	1993-05-25	1993-03-05	COLLECT COD              	MAIL      	unts. furiously express accounts w
4166	77	6	5	5	4885.35	0.08	0.01	A	F	1993-06-19	1993-04-24	1993-06-27	NONE                     	REG AIR   	hely unusual packages are above the f
4166	102	5	6	6	6012.60	0.04	0.08	R	F	1993-04-30	1993-04-17	1993-05-08	DELIVER IN PERSON        	MAIL      	ily ironic deposits print furiously. iron
4166	24	5	7	26	24024.52	0.09	0.01	R	F	1993-03-17	1993-05-09	1993-03-25	NONE                     	MAIL      	lar dependencies. s
4198	146	9	1	48	50214.72	0.09	0.05	N	O	1997-09-03	1997-07-18	1997-09-11	NONE                     	REG AIR   	cajole carefully final, ironic ide
4198	143	6	2	46	47984.44	0.09	0.01	N	O	1997-08-17	1997-09-08	1997-09-11	COLLECT COD              	TRUCK     	posits among th
4198	145	4	3	13	13586.82	0.03	0.04	N	O	1997-07-18	1997-07-24	1997-08-10	NONE                     	REG AIR   	 furious excuses. bli
4224	199	10	1	27	29678.13	0.05	0.03	N	O	1997-09-05	1997-08-19	1997-09-30	NONE                     	SHIP      	ly special deposits sleep qui
4224	37	3	2	20	18740.60	0.07	0.05	N	O	1997-11-09	1997-08-23	1997-11-14	NONE                     	FOB       	unts promise across the requests. blith
4224	24	7	3	4	3696.08	0.08	0.05	N	O	1997-09-07	1997-09-05	1997-09-25	TAKE BACK RETURN         	FOB       	 even dinos. carefull
4224	160	2	4	50	53008.00	0.10	0.06	N	O	1997-07-30	1997-09-10	1997-08-19	COLLECT COD              	RAIL      	side of the carefully silent dep
4224	85	6	5	48	47283.84	0.00	0.04	N	O	1997-10-03	1997-08-31	1997-10-10	NONE                     	RAIL      	 final, regular asymptotes use alway
4227	158	6	1	19	20104.85	0.01	0.08	A	F	1995-05-05	1995-05-03	1995-05-22	COLLECT COD              	REG AIR   	ns sleep along the blithely even theodolit
4227	33	4	2	8	7464.24	0.09	0.00	N	F	1995-06-11	1995-04-30	1995-06-28	COLLECT COD              	REG AIR   	 packages since the bold, u
4227	75	6	3	11	10725.77	0.10	0.04	A	F	1995-03-30	1995-05-02	1995-04-26	DELIVER IN PERSON        	SHIP      	l requests-- bold requests cajole dogg
4227	200	4	4	2	2200.40	0.02	0.05	R	F	1995-04-24	1995-05-09	1995-05-21	DELIVER IN PERSON        	AIR       	ep. specia
4227	147	6	5	49	51309.86	0.05	0.06	R	F	1995-05-19	1995-04-12	1995-06-12	TAKE BACK RETURN         	REG AIR   	ts sleep blithely carefully unusual ideas.
4256	151	9	1	22	23125.30	0.05	0.05	R	F	1992-07-30	1992-05-14	1992-08-14	NONE                     	TRUCK     	, final platelets are slyly final pint
4261	110	1	1	12	12121.32	0.05	0.01	A	F	1992-11-01	1993-01-01	1992-11-12	NONE                     	FOB       	into beans 
4261	82	3	2	4	3928.32	0.02	0.07	R	F	1992-12-11	1992-12-18	1992-12-24	DELIVER IN PERSON        	FOB       	ackages unwind furiously fluff
4261	175	5	3	3	3225.51	0.07	0.02	R	F	1992-11-10	1992-12-14	1992-11-17	COLLECT COD              	RAIL      	ly even deposits eat blithely alo
4261	174	3	4	36	38670.12	0.04	0.06	R	F	1992-12-02	1992-12-18	1992-12-25	NONE                     	REG AIR   	 slyly pendi
4261	24	7	5	28	25872.56	0.07	0.06	A	F	1992-10-08	1992-12-23	1992-10-11	TAKE BACK RETURN         	MAIL      	packages. fluffily i
4262	76	7	1	30	29282.10	0.01	0.03	N	O	1996-08-11	1996-10-11	1996-09-09	TAKE BACK RETURN         	RAIL      	tes after the carefully
4262	96	7	2	5	4980.45	0.02	0.05	N	O	1996-09-27	1996-09-05	1996-10-25	COLLECT COD              	SHIP      	blithely final asymptotes integrate
4262	162	1	3	5	5310.80	0.08	0.00	N	O	1996-10-02	1996-10-16	1996-10-05	NONE                     	REG AIR   	ironic accounts are unusu
4262	74	2	4	45	43833.15	0.02	0.01	N	O	1996-11-09	1996-09-09	1996-11-12	DELIVER IN PERSON        	SHIP      	ackages boost. pending, even instruction
4262	100	3	5	28	28002.80	0.06	0.02	N	O	1996-10-22	1996-09-06	1996-11-13	DELIVER IN PERSON        	FOB       	ironic, regular depend
4262	17	7	6	26	23842.26	0.03	0.02	N	O	1996-08-29	1996-09-25	1996-08-31	NONE                     	RAIL      	s boost slyly along the bold, iro
4262	160	5	7	41	43466.56	0.03	0.01	N	O	1996-08-28	1996-09-14	1996-09-20	COLLECT COD              	RAIL      	cuses unwind ac
4288	74	5	1	32	31170.24	0.10	0.07	R	F	1993-03-19	1993-01-26	1993-04-18	TAKE BACK RETURN         	AIR       	e blithely even instructions. speci
4288	105	6	2	39	39198.90	0.05	0.02	R	F	1993-03-25	1993-02-06	1993-03-28	DELIVER IN PERSON        	AIR       	uffy theodolites run
4288	125	8	3	7	7175.84	0.03	0.01	A	F	1993-01-15	1993-02-05	1993-01-26	NONE                     	TRUCK     	ngside of the special platelet
4289	196	7	1	19	20827.61	0.06	0.06	R	F	1993-12-31	1993-11-06	1994-01-23	DELIVER IN PERSON        	TRUCK     	e carefully regular ideas. sl
4291	192	6	1	3	3276.57	0.08	0.08	A	F	1994-03-17	1994-02-21	1994-03-27	COLLECT COD              	SHIP      	tes sleep slyly above the quickly sl
4291	125	8	2	43	44080.16	0.01	0.06	A	F	1994-02-01	1994-02-27	1994-02-06	DELIVER IN PERSON        	REG AIR   	s. quietly regular 
4291	8	1	3	25	22700.00	0.09	0.08	R	F	1994-02-14	1994-02-08	1994-03-15	COLLECT COD              	AIR       	uctions. furiously regular ins
4354	15	9	1	30	27450.30	0.08	0.07	R	F	1995-01-27	1994-11-24	1995-02-25	TAKE BACK RETURN         	REG AIR   	around the ir
4354	153	8	2	23	24222.45	0.01	0.08	R	F	1994-11-20	1994-12-23	1994-11-27	TAKE BACK RETURN         	AIR       	kly along the ironic, ent
4354	51	6	3	2	1902.10	0.10	0.04	A	F	1995-01-09	1994-12-15	1995-01-24	TAKE BACK RETURN         	REG AIR   	s nag quickly 
4354	86	7	4	36	35498.88	0.05	0.05	A	F	1994-11-20	1994-12-06	1994-12-06	DELIVER IN PERSON        	AIR       	 wake slyly eve
4354	65	10	5	37	35707.22	0.06	0.02	R	F	1995-01-13	1994-12-29	1995-01-31	DELIVER IN PERSON        	FOB       	deas use blithely! special foxes print af
4354	108	3	6	36	36291.60	0.03	0.04	R	F	1994-12-03	1994-12-05	1995-01-02	TAKE BACK RETURN         	TRUCK     	efully special packages use fluffily
4354	139	5	7	18	18704.34	0.03	0.04	A	F	1994-12-07	1994-12-11	1994-12-11	TAKE BACK RETURN         	SHIP      	ross the furiously 
4356	194	5	1	35	38296.65	0.00	0.04	R	F	1994-05-30	1994-06-14	1994-06-08	COLLECT COD              	MAIL      	arefully ironic 
4386	130	3	1	10	10301.30	0.05	0.07	N	O	1998-06-03	1998-04-16	1998-06-28	TAKE BACK RETURN         	MAIL      	gainst the quickly expre
4386	118	2	2	28	28507.08	0.03	0.06	N	O	1998-03-19	1998-05-01	1998-03-27	NONE                     	FOB       	. quick packages play slyly 
4386	140	1	3	4	4160.56	0.07	0.05	N	O	1998-04-07	1998-03-25	1998-04-19	COLLECT COD              	FOB       	ns wake carefully carefully iron
4386	121	2	4	21	21443.52	0.09	0.00	N	O	1998-05-05	1998-03-19	1998-05-13	NONE                     	RAIL      	e pending, sp
4386	130	3	5	39	40175.07	0.09	0.06	N	O	1998-03-05	1998-03-15	1998-03-16	NONE                     	RAIL      	structions cajole quickly express
4386	90	1	6	18	17821.62	0.02	0.05	N	O	1998-04-12	1998-04-09	1998-05-12	TAKE BACK RETURN         	SHIP      	 deposits use according to the pending, 
4386	20	4	7	16	14720.32	0.07	0.02	N	O	1998-05-05	1998-03-17	1998-06-03	COLLECT COD              	AIR       	e furiously final pint
4390	152	10	1	35	36825.25	0.07	0.04	R	F	1995-05-30	1995-07-02	1995-06-15	DELIVER IN PERSON        	TRUCK     	ongside of the slyly regular ideas
4390	196	8	2	28	30693.32	0.03	0.00	N	O	1995-09-07	1995-06-22	1995-10-05	COLLECT COD              	SHIP      	ld braids haggle atop the for
4390	101	8	3	42	42046.20	0.05	0.08	A	F	1995-06-12	1995-07-16	1995-06-17	NONE                     	AIR       	arefully even accoun
4390	98	2	4	32	31938.88	0.07	0.08	N	O	1995-09-15	1995-08-12	1995-10-05	TAKE BACK RETURN         	TRUCK     	ctions across
4416	94	7	1	37	36781.33	0.08	0.03	A	F	1992-10-23	1992-08-23	1992-11-16	COLLECT COD              	RAIL      	fluffily ironic 
4416	89	10	2	3	2967.24	0.06	0.03	R	F	1992-10-22	1992-08-06	1992-11-13	DELIVER IN PERSON        	SHIP      	 requests sleep along the 
4416	9	6	3	45	40905.00	0.09	0.03	A	F	1992-10-16	1992-09-09	1992-10-28	COLLECT COD              	AIR       	the final pinto beans. special frets 
4418	35	1	1	32	29920.96	0.02	0.06	A	F	1993-05-28	1993-06-02	1993-05-30	TAKE BACK RETURN         	RAIL      	ly. bold pinto b
4418	22	5	2	14	12908.28	0.03	0.04	A	F	1993-05-20	1993-06-18	1993-06-05	TAKE BACK RETURN         	SHIP      	 blithely regular requests. blith
4418	79	7	3	3	2937.21	0.00	0.02	R	F	1993-04-08	1993-06-04	1993-05-02	NONE                     	SHIP      	luffily across the unusual ideas. reque
4420	8	5	1	7	6356.00	0.07	0.03	R	F	1994-08-30	1994-09-03	1994-09-25	NONE                     	FOB       	 regular instructions sleep around
4422	135	1	1	5	5175.65	0.09	0.07	N	O	1995-07-17	1995-08-13	1995-07-25	NONE                     	SHIP      	e furiously about t
4422	48	5	2	41	38869.64	0.08	0.05	N	F	1995-06-12	1995-07-09	1995-06-20	COLLECT COD              	TRUCK     	 theodolites shal
4422	103	10	3	39	39120.90	0.00	0.05	N	O	1995-09-02	1995-06-24	1995-09-14	NONE                     	TRUCK     	en hockey players engage
4422	153	4	4	4	4212.60	0.02	0.05	N	O	1995-09-18	1995-08-12	1995-10-18	COLLECT COD              	FOB       	cies along the bo
4422	80	9	5	20	19601.60	0.07	0.05	N	O	1995-08-17	1995-07-16	1995-09-13	DELIVER IN PERSON        	RAIL      	ructions wake slyly al
4448	52	7	1	24	22849.20	0.10	0.07	N	O	1998-09-09	1998-07-06	1998-09-27	DELIVER IN PERSON        	SHIP      	nal packages along the ironic instructi
4448	189	10	2	13	14159.34	0.00	0.01	N	O	1998-07-26	1998-07-03	1998-08-14	COLLECT COD              	MAIL      	fluffily express accounts integrate furiou
4448	41	4	3	35	32936.40	0.10	0.06	N	O	1998-09-18	1998-07-27	1998-10-08	NONE                     	REG AIR   	aggle carefully alongside of the q
4448	141	2	4	3	3123.42	0.01	0.01	N	O	1998-07-20	1998-07-10	1998-08-07	DELIVER IN PERSON        	TRUCK     	ronic theod
4448	91	2	5	41	40634.69	0.00	0.08	N	O	1998-07-30	1998-08-09	1998-08-03	NONE                     	AIR       	pon the permanently even excuses nag 
4448	172	3	6	12	12866.04	0.06	0.03	N	O	1998-08-21	1998-06-30	1998-09-09	COLLECT COD              	RAIL      	sits about the ironic, bu
4453	147	10	1	41	42932.74	0.00	0.08	N	O	1997-07-17	1997-05-15	1997-07-31	NONE                     	REG AIR   	anent theodolites are slyly except t
4453	133	4	2	16	16530.08	0.03	0.00	N	O	1997-07-22	1997-05-05	1997-08-03	COLLECT COD              	FOB       	ar excuses nag quickly even accounts. b
4453	62	7	3	48	46178.88	0.02	0.07	N	O	1997-05-29	1997-06-24	1997-06-03	NONE                     	SHIP      	eep. fluffily express accounts at the furi
4453	102	5	4	26	26054.60	0.06	0.07	N	O	1997-05-07	1997-06-07	1997-05-22	NONE                     	TRUCK     	express packages are
4455	70	5	1	20	19401.40	0.01	0.05	A	F	1994-01-31	1993-11-21	1994-03-02	DELIVER IN PERSON        	MAIL      	 express packages. packages boost quickly
4455	153	4	2	47	49498.05	0.09	0.01	R	F	1994-01-01	1993-12-25	1994-01-05	COLLECT COD              	FOB       	 requests. even, even accou
4455	123	2	3	34	34786.08	0.00	0.06	A	F	1993-10-24	1993-11-27	1993-11-04	TAKE BACK RETURN         	AIR       	 slyly ironic requests. quickly even d
4482	71	2	1	32	31074.24	0.06	0.03	A	F	1995-05-16	1995-07-22	1995-06-07	NONE                     	RAIL      	 quickly pendin
4482	96	9	2	32	31874.88	0.01	0.06	N	O	1995-08-16	1995-06-26	1995-09-10	DELIVER IN PERSON        	AIR       	eans wake according 
4485	191	5	1	1	1091.19	0.03	0.05	R	F	1994-12-04	1995-02-07	1994-12-09	NONE                     	AIR       	play according to the ironic, ironic
4485	141	10	2	46	47892.44	0.04	0.06	R	F	1995-03-09	1994-12-14	1995-03-23	DELIVER IN PERSON        	AIR       	. ironic foxes haggle. regular war
4485	175	6	3	43	46232.31	0.01	0.05	R	F	1995-01-17	1995-02-11	1995-02-07	DELIVER IN PERSON        	TRUCK     	al accounts according to the slyly r
4485	144	5	4	43	44898.02	0.08	0.06	R	F	1995-01-28	1995-01-26	1995-02-07	DELIVER IN PERSON        	AIR       	. blithely
4485	6	7	5	47	42582.00	0.08	0.04	R	F	1995-03-11	1995-01-11	1995-03-21	TAKE BACK RETURN         	RAIL      	luffily pending acc
4512	162	1	1	30	31864.80	0.07	0.07	N	O	1996-01-28	1995-12-22	1996-02-22	TAKE BACK RETURN         	TRUCK     	ly unusual package
4512	41	4	2	24	22584.96	0.04	0.06	N	O	1995-12-16	1996-01-16	1995-12-25	NONE                     	SHIP      	ly regular pinto beans. carefully bold depo
4512	145	8	3	21	21947.94	0.00	0.00	N	O	1995-10-31	1995-12-30	1995-11-15	NONE                     	REG AIR   	lly unusual pinto b
4512	141	2	4	32	33316.48	0.10	0.01	N	O	1995-11-25	1995-12-28	1995-12-06	NONE                     	FOB       	counts are against the quickly regular 
4512	133	4	5	43	44424.59	0.06	0.00	N	O	1995-12-20	1995-11-28	1996-01-14	NONE                     	AIR       	are carefully. theodolites wake
4514	164	9	1	27	28732.32	0.06	0.06	R	F	1994-07-01	1994-07-13	1994-07-26	COLLECT COD              	AIR       	 even, silent foxes be
4514	46	3	2	15	14190.60	0.10	0.04	R	F	1994-08-24	1994-07-11	1994-09-14	DELIVER IN PERSON        	RAIL      	! unusual, special deposits afte
4514	78	8	3	10	9780.70	0.09	0.05	A	F	1994-06-19	1994-06-25	1994-07-01	COLLECT COD              	SHIP      	ake furiously. carefully regular requests
4514	149	8	5	12	12589.68	0.02	0.03	R	F	1994-08-20	1994-06-09	1994-09-15	TAKE BACK RETURN         	FOB       	 carefully ironic foxes nag caref
4514	189	10	6	38	41388.84	0.03	0.05	A	F	1994-07-28	1994-07-06	1994-08-25	NONE                     	AIR       	ending excuses. sl
4514	177	8	7	27	29083.59	0.04	0.06	A	F	1994-06-24	1994-07-14	1994-06-30	TAKE BACK RETURN         	TRUCK     	. slyly sile
4518	144	7	1	9	9397.26	0.09	0.04	N	O	1997-06-26	1997-07-07	1997-07-10	NONE                     	RAIL      	 pending deposits. slyly re
4518	45	6	2	19	17955.76	0.10	0.05	N	O	1997-08-09	1997-06-06	1997-08-27	COLLECT COD              	RAIL      	ter the slyly bo
4544	131	7	1	40	41245.20	0.07	0.01	N	O	1997-08-15	1997-10-16	1997-08-20	DELIVER IN PERSON        	RAIL      	 detect slyly. evenly pending instru
4544	172	2	2	19	20371.23	0.08	0.01	N	O	1997-08-14	1997-09-08	1997-08-25	NONE                     	SHIP      	regular ideas are furiously about
4544	71	9	3	20	19421.40	0.02	0.07	N	O	1997-10-12	1997-10-11	1997-10-13	COLLECT COD              	REG AIR   	 waters about the
4544	51	6	4	39	37090.95	0.07	0.05	N	O	1997-08-20	1997-09-07	1997-08-27	COLLECT COD              	REG AIR   	ular packages. s
4544	133	4	5	31	32027.03	0.09	0.03	N	O	1997-08-09	1997-09-29	1997-08-17	COLLECT COD              	TRUCK     	dolites detect quickly reg
4544	27	8	6	8	7416.16	0.10	0.03	N	O	1997-10-13	1997-10-06	1997-10-25	COLLECT COD              	AIR       	olites. fi
4546	133	4	1	10	10331.30	0.09	0.02	N	O	1995-09-23	1995-10-10	1995-10-23	COLLECT COD              	TRUCK     	osits alongside of the
4546	171	10	2	15	16067.55	0.04	0.07	N	O	1995-07-31	1995-10-17	1995-08-06	NONE                     	REG AIR   	ught to cajole furiously. qu
4546	77	8	3	4	3908.28	0.06	0.08	N	O	1995-08-14	1995-10-07	1995-08-16	COLLECT COD              	MAIL      	kly pending dependencies along the furio
4546	149	6	4	10	10491.40	0.08	0.02	N	O	1995-09-02	1995-09-16	1995-09-10	DELIVER IN PERSON        	FOB       	above the enticingly ironic dependencies
4547	188	9	1	15	16322.70	0.10	0.04	A	F	1993-12-08	1993-11-15	1993-12-22	NONE                     	REG AIR   	ets haggle. regular dinos affix fu
4547	116	10	2	7	7112.77	0.10	0.02	A	F	1993-09-04	1993-09-29	1993-09-20	COLLECT COD              	RAIL      	slyly express a
4547	45	2	3	15	14175.60	0.00	0.00	R	F	1993-11-18	1993-10-06	1993-12-13	NONE                     	TRUCK     	e carefully across the unus
4547	148	7	4	15	15722.10	0.05	0.08	R	F	1993-11-29	1993-10-12	1993-12-29	COLLECT COD              	REG AIR   	ironic gifts integrate 
4548	14	8	1	21	19194.21	0.10	0.05	N	O	1996-07-11	1996-09-04	1996-07-30	COLLECT COD              	REG AIR   	pecial theodoli
4548	47	10	2	17	16099.68	0.00	0.08	N	O	1996-07-23	1996-09-21	1996-07-26	DELIVER IN PERSON        	REG AIR   	y ironic requests above the fluffily d
4548	123	2	3	47	48086.64	0.05	0.04	N	O	1996-07-24	1996-09-12	1996-08-08	NONE                     	MAIL      	ts. excuses use slyly spec
4548	177	6	4	22	23697.74	0.07	0.01	N	O	1996-07-06	1996-08-23	1996-07-15	DELIVER IN PERSON        	RAIL      	s. furiously ironic theodolites c
4548	45	4	5	36	34021.44	0.04	0.06	N	O	1996-08-19	1996-09-12	1996-09-08	COLLECT COD              	FOB       	tions integrat
4550	150	7	1	9	9451.35	0.05	0.06	R	F	1995-04-19	1995-02-07	1995-04-24	COLLECT COD              	SHIP      	l dependencies boost slyly after th
4550	66	5	2	19	18355.14	0.06	0.04	A	F	1995-01-01	1995-02-13	1995-01-20	NONE                     	AIR       	quests. express 
4551	11	1	1	6	5466.06	0.08	0.08	N	O	1996-05-18	1996-04-23	1996-06-13	DELIVER IN PERSON        	TRUCK     	fily silent fo
4551	179	8	2	26	28058.42	0.02	0.04	N	O	1996-04-14	1996-04-26	1996-04-17	TAKE BACK RETURN         	RAIL      	le. carefully dogged accounts use furiousl
4551	22	1	3	22	20284.44	0.08	0.01	N	O	1996-05-12	1996-03-17	1996-05-29	TAKE BACK RETURN         	REG AIR   	ly ironic reques
4551	198	10	4	27	29651.13	0.00	0.01	N	O	1996-04-28	1996-03-22	1996-05-22	TAKE BACK RETURN         	RAIL      	y along the slyly even 
4577	185	6	1	43	46662.74	0.01	0.03	N	O	1998-06-16	1998-07-09	1998-06-17	TAKE BACK RETURN         	AIR       	packages. 
4577	177	6	2	43	46318.31	0.05	0.03	N	O	1998-08-24	1998-06-02	1998-09-14	TAKE BACK RETURN         	RAIL      	ly accounts. carefully 
4577	69	6	3	12	11628.72	0.07	0.05	N	O	1998-07-29	1998-06-17	1998-08-04	DELIVER IN PERSON        	TRUCK     	equests alongsi
4580	92	5	1	22	21825.98	0.01	0.05	A	F	1994-01-16	1994-01-26	1994-02-05	COLLECT COD              	AIR       	nticingly final packag
4580	32	3	2	10	9320.30	0.05	0.04	R	F	1993-12-20	1993-12-30	1994-01-17	COLLECT COD              	RAIL      	gular, pending deposits. fina
4580	1	8	3	41	36941.00	0.00	0.07	R	F	1993-12-13	1994-01-31	1994-01-06	NONE                     	SHIP      	requests. quickly silent asymptotes sle
4580	178	8	4	5	5390.85	0.07	0.00	A	F	1994-01-28	1993-12-17	1994-02-22	NONE                     	TRUCK     	o beans. f
4580	189	10	5	39	42478.02	0.03	0.02	R	F	1993-12-28	1993-12-26	1994-01-23	NONE                     	RAIL      	. fluffily final dolphins use furiously al
4581	165	4	1	37	39410.92	0.01	0.04	A	F	1992-10-17	1992-11-05	1992-11-04	DELIVER IN PERSON        	MAIL      	e the blithely bold pearls ha
4581	50	3	2	7	6650.35	0.01	0.02	A	F	1992-10-09	1992-10-20	1992-10-21	TAKE BACK RETURN         	MAIL      	express accounts d
4581	21	10	3	46	42366.92	0.04	0.04	A	F	1992-09-09	1992-11-27	1992-09-26	NONE                     	REG AIR   	nag toward the carefully final accounts. 
4582	192	5	1	17	18567.23	0.09	0.08	N	O	1996-08-17	1996-08-26	1996-08-20	COLLECT COD              	REG AIR   	ng packages. depo
4609	47	6	1	28	26517.12	0.10	0.05	N	O	1997-02-02	1997-02-17	1997-03-02	DELIVER IN PERSON        	REG AIR   	ously. quickly final requests cajole fl
4609	185	6	2	3	3255.54	0.09	0.03	N	O	1996-12-28	1997-02-06	1997-01-20	NONE                     	FOB       	nstructions. furious instructions 
4609	23	4	3	46	42458.92	0.05	0.05	N	O	1997-02-11	1997-01-16	1997-03-07	NONE                     	FOB       	r foxes. fluffily ironic ideas ha
4610	87	8	1	21	20728.68	0.07	0.07	R	F	1993-08-10	1993-08-05	1993-08-27	NONE                     	REG AIR   	ly special theodolites. even,
4610	175	5	2	14	15052.38	0.00	0.07	R	F	1993-07-28	1993-07-25	1993-07-31	TAKE BACK RETURN         	SHIP      	 ironic frays. dependencies detect blithel
4610	159	1	3	44	46602.60	0.05	0.03	A	F	1993-08-05	1993-07-20	1993-08-19	COLLECT COD              	TRUCK     	 final theodolites 
4610	75	3	4	26	25351.82	0.06	0.03	R	F	1993-07-01	1993-07-19	1993-07-19	NONE                     	MAIL      	 to the fluffily ironic requests h
4610	147	8	5	29	30367.06	0.08	0.04	R	F	1993-08-09	1993-07-27	1993-08-16	DELIVER IN PERSON        	AIR       	 foxes. special, express package
4612	6	9	1	20	18120.00	0.02	0.03	R	F	1993-09-24	1993-12-18	1993-10-22	NONE                     	AIR       	beans sleep blithely iro
4612	50	7	2	17	16150.85	0.10	0.06	A	F	1994-01-09	1993-11-08	1994-02-06	TAKE BACK RETURN         	REG AIR   	equests haggle carefully silent excus
4612	137	8	3	40	41485.20	0.08	0.01	R	F	1993-10-08	1993-11-23	1993-10-24	DELIVER IN PERSON        	RAIL      	special platelets.
4612	185	6	4	10	10851.80	0.10	0.06	A	F	1993-11-11	1993-11-19	1993-11-13	TAKE BACK RETURN         	SHIP      	unusual theodol
4613	38	9	1	17	15946.51	0.09	0.07	N	O	1998-06-07	1998-05-11	1998-06-29	DELIVER IN PERSON        	SHIP      	liers cajole a
4613	108	1	2	25	25202.50	0.05	0.04	N	O	1998-05-22	1998-04-11	1998-05-27	TAKE BACK RETURN         	SHIP      	y pending platelets x-ray ironically! pend
4613	174	3	3	15	16112.55	0.10	0.02	N	O	1998-05-31	1998-04-16	1998-06-25	DELIVER IN PERSON        	MAIL      	against the quickly r
4613	8	1	4	36	32688.00	0.04	0.01	N	O	1998-04-22	1998-05-05	1998-05-04	DELIVER IN PERSON        	AIR       	gainst the furiously ironic
4613	111	8	5	35	35388.85	0.04	0.06	N	O	1998-06-04	1998-04-17	1998-06-20	COLLECT COD              	MAIL      	e blithely against the even, bold pi
4613	196	8	6	47	51520.93	0.04	0.04	N	O	1998-07-03	1998-05-26	1998-07-09	NONE                     	FOB       	uriously special requests wak
4613	119	3	7	39	39745.29	0.09	0.05	N	O	1998-06-12	1998-06-01	1998-07-06	DELIVER IN PERSON        	REG AIR   	ously express
4614	7	2	1	19	17233.00	0.09	0.08	N	O	1996-05-17	1996-06-21	1996-06-08	TAKE BACK RETURN         	AIR       	ix. carefully regular 
4614	65	6	2	3	2895.18	0.08	0.01	N	O	1996-07-22	1996-07-21	1996-08-07	NONE                     	MAIL      	ions engage final, ironic 
4614	8	1	3	36	32688.00	0.10	0.04	N	O	1996-07-05	1996-06-26	1996-07-07	NONE                     	REG AIR   	onic foxes affix furi
4614	126	9	4	6	6156.72	0.09	0.01	N	O	1996-06-11	1996-05-30	1996-07-03	COLLECT COD              	REG AIR   	ake quickly quickly regular epitap
4614	73	3	5	24	23353.68	0.07	0.06	N	O	1996-07-01	1996-06-24	1996-07-08	COLLECT COD              	REG AIR   	regular, even
4614	34	5	6	32	29888.96	0.10	0.05	N	O	1996-08-21	1996-05-28	1996-09-16	NONE                     	REG AIR   	ickly furio
4614	128	1	7	41	42152.92	0.01	0.07	N	O	1996-07-31	1996-07-12	1996-08-16	COLLECT COD              	REG AIR   	ackages haggle carefully about the even, b
4640	88	9	1	5	4940.40	0.03	0.08	N	O	1996-02-05	1996-02-14	1996-02-15	TAKE BACK RETURN         	RAIL      	 warthogs against the regular
4640	88	9	2	9	8892.72	0.03	0.05	N	O	1996-02-12	1996-02-14	1996-02-29	DELIVER IN PERSON        	AIR       	 accounts. unu
4640	27	10	3	18	16686.36	0.02	0.07	N	O	1996-02-28	1996-03-06	1996-03-28	DELIVER IN PERSON        	RAIL      	boost furiously accord
4640	23	2	4	36	33228.72	0.06	0.08	N	O	1996-01-03	1996-03-09	1996-01-11	DELIVER IN PERSON        	RAIL      	iously furious accounts boost. carefully
4640	156	1	5	15	15842.25	0.03	0.02	N	O	1996-03-19	1996-02-09	1996-04-11	TAKE BACK RETURN         	FOB       	y regular instructions doze furiously. reg
4645	50	7	1	45	42752.25	0.09	0.05	A	F	1994-12-27	1994-11-02	1994-12-31	DELIVER IN PERSON        	AIR       	ular ideas. slyly
4645	66	7	2	32	30913.92	0.10	0.08	A	F	1994-11-17	1994-10-30	1994-11-18	COLLECT COD              	REG AIR   	 final accounts alongside
4645	54	5	3	25	23851.25	0.03	0.00	R	F	1994-10-25	1994-12-11	1994-11-14	NONE                     	REG AIR   	braids. ironic dependencies main
4645	37	8	4	42	39355.26	0.10	0.02	R	F	1994-12-02	1994-12-18	1994-12-16	COLLECT COD              	TRUCK     	regular pinto beans amon
4645	161	10	5	35	37140.60	0.03	0.07	A	F	1994-12-08	1994-11-25	1994-12-09	TAKE BACK RETURN         	FOB       	sias believe bl
4645	42	9	6	27	25435.08	0.09	0.08	R	F	1994-11-26	1994-10-25	1994-12-04	NONE                     	SHIP      	ously express pinto beans. ironic depos
4645	31	2	7	42	39103.26	0.10	0.06	A	F	1994-12-31	1994-10-22	1995-01-28	DELIVER IN PERSON        	AIR       	e slyly regular pinto beans. thin
4646	191	3	1	24	26188.56	0.02	0.05	N	O	1996-09-18	1996-08-09	1996-09-21	TAKE BACK RETURN         	RAIL      	ic platelets lose carefully. blithely unu
4646	178	6	2	26	28032.42	0.07	0.00	N	O	1996-10-02	1996-08-25	1996-10-27	DELIVER IN PERSON        	MAIL      	ix according to the slyly spe
4646	34	10	3	18	16812.54	0.01	0.00	N	O	1996-06-30	1996-08-10	1996-07-12	TAKE BACK RETURN         	TRUCK     	beans sleep car
4646	40	1	4	38	35721.52	0.08	0.01	N	O	1996-09-01	1996-08-23	1996-09-27	COLLECT COD              	SHIP      	al platelets cajole. slyly final dol
4646	26	1	5	22	20372.44	0.01	0.08	N	O	1996-07-14	1996-08-06	1996-07-29	DELIVER IN PERSON        	MAIL      	cies are blithely after the slyly reg
4672	59	7	1	22	21099.10	0.01	0.07	N	O	1995-12-03	1995-12-08	1995-12-17	COLLECT COD              	AIR       	l instructions. blithely ironic packages 
4672	61	10	2	41	39403.46	0.00	0.00	N	O	1995-12-01	1995-12-15	1995-12-12	COLLECT COD              	RAIL      	 slyly quie
4672	163	10	3	24	25515.84	0.04	0.03	N	O	1995-11-11	1995-12-28	1995-12-04	NONE                     	REG AIR   	y fluffily stealt
4672	57	2	4	13	12441.65	0.10	0.03	N	O	1996-02-02	1995-12-13	1996-03-02	DELIVER IN PERSON        	RAIL      	ar requests? pending accounts against
4672	55	10	5	45	42977.25	0.08	0.07	N	O	1996-02-07	1996-01-16	1996-02-14	DELIVER IN PERSON        	MAIL      	 platelets use amon
4672	141	8	6	20	20822.80	0.02	0.07	N	O	1995-12-08	1996-01-25	1995-12-19	COLLECT COD              	REG AIR   	s boost at the ca
4672	72	10	7	38	36938.66	0.01	0.01	N	O	1995-11-28	1995-12-08	1995-12-13	COLLECT COD              	SHIP      	ests. idle, regular ex
4673	17	8	1	8	7336.08	0.08	0.01	N	O	1996-10-12	1996-10-05	1996-11-04	TAKE BACK RETURN         	FOB       	lithely final re
4673	101	2	2	44	44048.40	0.06	0.01	N	O	1996-12-11	1996-10-31	1997-01-08	DELIVER IN PERSON        	RAIL      	 gifts cajole dari
4673	123	2	3	9	9208.08	0.04	0.07	N	O	1996-10-15	1996-09-30	1996-10-30	DELIVER IN PERSON        	MAIL      	ages nag across 
4678	58	6	1	35	33531.75	0.04	0.08	N	O	1998-11-27	1998-10-02	1998-12-17	TAKE BACK RETURN         	AIR       	he accounts. fluffily bold sheaves b
4678	117	1	2	18	18307.98	0.03	0.06	N	O	1998-10-30	1998-09-22	1998-11-25	TAKE BACK RETURN         	SHIP      	usly ironic 
4678	96	9	3	13	12949.17	0.10	0.07	N	O	1998-11-03	1998-10-17	1998-11-06	TAKE BACK RETURN         	SHIP      	its. carefully final fr
4678	22	1	4	23	21206.46	0.06	0.05	N	O	1998-09-03	1998-09-20	1998-09-04	DELIVER IN PERSON        	SHIP      	ily sly deposi
4678	178	9	5	40	43126.80	0.03	0.07	N	O	1998-11-11	1998-10-27	1998-11-24	TAKE BACK RETURN         	AIR       	. final, unusual requests sleep thinl
4679	190	1	1	7	7631.33	0.10	0.05	R	F	1993-05-11	1993-04-11	1993-05-16	NONE                     	TRUCK     	kages. bold, regular packa
4704	78	6	1	14	13692.98	0.04	0.04	N	O	1996-10-27	1996-11-02	1996-11-07	DELIVER IN PERSON        	TRUCK     	 above the slyly final requests. quickly 
4704	28	3	2	7	6496.14	0.03	0.04	N	O	1996-12-04	1996-10-30	1996-12-23	DELIVER IN PERSON        	SHIP      	ers wake car
4704	64	5	3	44	42418.64	0.02	0.05	N	O	1996-09-02	1996-10-07	1996-09-17	DELIVER IN PERSON        	REG AIR   	out the care
4709	25	6	1	25	23125.50	0.03	0.05	N	O	1996-02-21	1996-02-11	1996-03-17	DELIVER IN PERSON        	AIR       	deposits grow. fluffily unusual accounts 
4709	177	5	2	25	26929.25	0.05	0.03	N	O	1996-01-22	1996-03-03	1996-02-21	DELIVER IN PERSON        	REG AIR   	inst the ironic, regul
4710	183	4	1	40	43327.20	0.10	0.08	A	F	1995-03-09	1995-02-25	1995-03-29	TAKE BACK RETURN         	AIR       	cross the blithely bold packages. silen
4710	128	3	2	47	48321.64	0.04	0.01	R	F	1995-02-22	1995-01-12	1995-02-28	NONE                     	RAIL      	blithely express packages. even, ironic re
4737	191	5	1	37	40374.03	0.03	0.04	R	F	1993-05-17	1993-04-10	1993-05-30	DELIVER IN PERSON        	TRUCK     	s. fluffily regular 
4737	69	8	2	22	21319.32	0.04	0.04	A	F	1993-03-29	1993-05-22	1993-04-16	TAKE BACK RETURN         	RAIL      	 hang fluffily around t
4741	73	2	1	24	23353.68	0.00	0.01	A	F	1992-09-16	1992-09-19	1992-09-20	DELIVER IN PERSON        	RAIL      	deas boost furiously slyly regular id
4741	113	4	2	16	16209.76	0.01	0.07	R	F	1992-08-25	1992-08-10	1992-08-29	TAKE BACK RETURN         	FOB       	final foxes haggle r
4741	156	8	3	24	25347.60	0.05	0.08	A	F	1992-11-04	1992-08-14	1992-11-06	TAKE BACK RETURN         	MAIL      	even requests.
4741	51	3	4	39	37090.95	0.09	0.06	R	F	1992-10-28	1992-10-03	1992-11-11	COLLECT COD              	SHIP      	t, regular requests
4741	179	10	5	40	43166.80	0.09	0.03	R	F	1992-09-20	1992-09-23	1992-10-09	TAKE BACK RETURN         	REG AIR   	 fluffily slow deposits. fluffily regu
4741	157	5	6	34	35943.10	0.02	0.07	R	F	1992-08-25	1992-08-18	1992-09-20	DELIVER IN PERSON        	RAIL      	sly special packages after the furiously
4743	60	5	1	19	18241.14	0.04	0.07	A	F	1993-06-23	1993-05-03	1993-07-20	COLLECT COD              	AIR       	hely even accounts
4743	159	4	2	3	3177.45	0.01	0.03	R	F	1993-04-14	1993-06-08	1993-05-09	NONE                     	TRUCK     	al requests. express idea
4743	73	2	3	21	20434.47	0.08	0.03	A	F	1993-07-02	1993-06-15	1993-07-26	DELIVER IN PERSON        	RAIL      	ake blithely against the packages. reg
4743	34	5	4	27	25218.81	0.08	0.05	R	F	1993-07-26	1993-05-27	1993-08-24	DELIVER IN PERSON        	AIR       	aids use. express deposits
4769	35	1	1	16	14960.48	0.08	0.05	N	O	1995-07-16	1995-07-05	1995-07-22	TAKE BACK RETURN         	FOB       	 deposits. slyly even asymptote
4769	63	8	2	34	32744.04	0.06	0.07	N	O	1995-07-26	1995-05-18	1995-08-03	COLLECT COD              	REG AIR   	ven instructions. ca
4769	47	10	3	36	34093.44	0.10	0.03	N	O	1995-07-22	1995-06-16	1995-08-11	NONE                     	RAIL      	. slyly even deposit
4769	69	10	4	45	43607.70	0.08	0.06	R	F	1995-06-01	1995-07-13	1995-06-04	TAKE BACK RETURN         	RAIL      	accounts are. even accounts sleep
4769	112	6	5	15	15181.65	0.07	0.08	N	F	1995-06-12	1995-07-07	1995-07-04	NONE                     	SHIP      	egular platelets can cajole across the 
4771	49	10	1	9	8541.36	0.01	0.00	R	F	1993-02-28	1993-02-19	1993-03-25	NONE                     	FOB       	riously after the packages. fina
4771	16	7	2	21	19236.21	0.09	0.01	R	F	1993-01-19	1993-02-10	1993-02-01	NONE                     	FOB       	fluffily pendi
4771	12	3	3	5	4560.05	0.06	0.08	R	F	1993-01-07	1993-01-19	1993-01-26	NONE                     	RAIL      	ar, quiet accounts nag furiously express id
4771	9	4	4	21	19089.00	0.05	0.04	A	F	1992-12-20	1993-01-22	1992-12-26	TAKE BACK RETURN         	SHIP      	 carefully re
4801	184	5	1	37	40114.66	0.10	0.02	N	O	1996-03-09	1996-02-29	1996-03-25	TAKE BACK RETURN         	FOB       	uests hinder blithely against the instr
4801	26	1	2	34	31484.68	0.03	0.02	N	O	1996-02-05	1996-04-16	1996-02-23	NONE                     	SHIP      	y final requests 
4801	110	1	3	4	4040.44	0.04	0.04	N	O	1996-03-23	1996-04-04	1996-03-25	COLLECT COD              	RAIL      	pitaphs. regular, reg
4801	92	3	4	39	38691.51	0.07	0.01	N	O	1996-03-19	1996-03-21	1996-04-17	TAKE BACK RETURN         	REG AIR   	warhorses wake never for the care
4803	132	3	1	2	2064.26	0.08	0.03	N	O	1996-04-16	1996-03-20	1996-05-15	NONE                     	REG AIR   	gular reque
4803	176	4	2	47	50579.99	0.10	0.00	N	O	1996-03-14	1996-03-30	1996-03-15	DELIVER IN PERSON        	FOB       	ly final excuses. slyly express requ
4803	196	8	3	42	46039.98	0.04	0.08	N	O	1996-04-27	1996-05-05	1996-05-17	NONE                     	TRUCK     	 accounts affix quickly ar
4803	22	1	4	24	22128.48	0.10	0.04	N	O	1996-02-24	1996-04-02	1996-02-28	NONE                     	MAIL      	t blithely slyly special decoys. 
4803	189	10	5	21	22872.78	0.03	0.06	N	O	1996-05-25	1996-03-15	1996-06-09	COLLECT COD              	FOB       	 silent packages use. b
4803	194	5	6	19	20789.61	0.07	0.00	N	O	1996-04-20	1996-03-25	1996-04-27	TAKE BACK RETURN         	RAIL      	sts. enticing, even
4806	16	7	1	26	23816.26	0.10	0.05	R	F	1993-05-28	1993-06-07	1993-05-29	DELIVER IN PERSON        	SHIP      	 bold pearls sublate blithely. quickly pe
4806	72	10	2	6	5832.42	0.01	0.06	A	F	1993-05-17	1993-07-19	1993-05-29	TAKE BACK RETURN         	SHIP      	even theodolites. packages sl
4806	29	4	3	8	7432.16	0.09	0.00	A	F	1993-05-08	1993-07-16	1993-05-28	NONE                     	TRUCK     	requests boost blithely. qui
4807	122	1	1	9	9199.08	0.04	0.08	N	O	1997-04-23	1997-03-01	1997-05-15	TAKE BACK RETURN         	TRUCK     	may are blithely. carefully even pinto b
4807	10	1	2	41	37310.41	0.07	0.08	N	O	1997-05-02	1997-03-31	1997-05-15	TAKE BACK RETURN         	AIR       	 fluffily re
4807	145	6	3	34	35534.76	0.06	0.02	N	O	1997-01-31	1997-03-13	1997-02-01	NONE                     	SHIP      	ecial ideas. deposits according to the fin
4807	190	1	4	32	34886.08	0.05	0.00	N	O	1997-04-04	1997-03-21	1997-04-16	NONE                     	RAIL      	efully even dolphins slee
4807	159	1	5	2	2118.30	0.02	0.05	N	O	1997-05-09	1997-04-03	1997-06-05	TAKE BACK RETURN         	RAIL      	deas wake bli
4807	160	1	6	22	23323.52	0.09	0.06	N	O	1997-03-13	1997-02-23	1997-04-01	NONE                     	FOB       	es use final excuses. furiously final
4832	15	6	1	23	21045.23	0.03	0.01	N	O	1997-12-05	1998-01-05	1997-12-10	NONE                     	RAIL      	y express depo
4832	152	4	2	10	10521.50	0.00	0.06	N	O	1998-01-08	1998-02-01	1998-01-11	DELIVER IN PERSON        	MAIL      	ly. blithely bold pinto beans should have
4832	149	6	3	4	4196.56	0.04	0.01	N	O	1998-01-16	1998-02-12	1998-02-08	TAKE BACK RETURN         	AIR       	ages. slyly express deposits cajole car
4832	64	5	4	6	5784.36	0.02	0.01	N	O	1997-12-08	1998-02-03	1997-12-10	COLLECT COD              	TRUCK     	ages cajole after the bold requests. furi
4832	138	4	5	43	44639.59	0.10	0.08	N	O	1997-12-31	1998-02-20	1998-01-26	COLLECT COD              	RAIL      	oze according to the accou
4833	107	10	1	31	31220.10	0.08	0.04	N	O	1996-06-24	1996-07-15	1996-07-02	NONE                     	SHIP      	ven instructions cajole against the caref
4833	117	7	2	11	11188.21	0.03	0.01	N	O	1996-08-24	1996-07-26	1996-09-19	NONE                     	REG AIR   	s nag above the busily sile
4833	18	9	3	26	23868.26	0.08	0.04	N	O	1996-05-13	1996-07-12	1996-05-31	NONE                     	SHIP      	s packages. even gif
4833	36	7	4	19	17784.57	0.07	0.07	N	O	1996-08-21	1996-07-09	1996-09-10	TAKE BACK RETURN         	AIR       	y quick theodolit
4833	35	1	5	4	3740.12	0.10	0.02	N	O	1996-08-16	1996-06-29	1996-08-22	NONE                     	AIR       	y pending packages sleep blithely regular r
4834	183	4	1	27	29245.86	0.06	0.02	N	O	1997-01-09	1996-10-27	1997-01-27	DELIVER IN PERSON        	RAIL      	es nag blithe
4834	71	1	2	26	25247.82	0.01	0.00	N	O	1996-10-04	1996-10-21	1996-10-10	DELIVER IN PERSON        	TRUCK     	ages dazzle carefully. slyly daring foxes
4834	23	2	3	34	31382.68	0.03	0.01	N	O	1996-12-09	1996-11-26	1996-12-10	NONE                     	MAIL      	ounts haggle bo
4834	143	10	4	38	39639.32	0.03	0.06	N	O	1997-01-10	1996-12-06	1997-01-22	COLLECT COD              	FOB       	alongside of the carefully even plate
4836	162	1	1	22	23367.52	0.01	0.03	N	O	1997-03-03	1997-02-23	1997-03-04	NONE                     	SHIP      	al pinto beans. care
4836	48	5	2	16	15168.64	0.07	0.08	N	O	1997-01-14	1997-03-05	1997-01-30	COLLECT COD              	MAIL      	gular packages against the express reque
4836	76	4	3	14	13664.98	0.03	0.08	N	O	1997-02-21	1997-02-06	1997-03-08	COLLECT COD              	MAIL      	lites. unusual, bold dolphins ar
4836	106	1	4	15	15091.50	0.10	0.00	N	O	1997-03-08	1997-03-14	1997-03-30	TAKE BACK RETURN         	TRUCK     	eep slyly. even requests cajole
4836	51	6	5	12	11412.60	0.01	0.04	N	O	1997-02-02	1997-02-10	1997-02-03	COLLECT COD              	TRUCK     	sly ironic accoun
4838	122	3	1	35	35774.20	0.01	0.00	R	F	1992-10-30	1992-10-23	1992-11-21	TAKE BACK RETURN         	RAIL      	ly blithely unusual foxes. even package
4838	148	5	2	2	2096.28	0.03	0.08	R	F	1992-08-11	1992-09-16	1992-08-26	COLLECT COD              	MAIL      	hely final notornis are furiously blithe
4838	52	3	3	26	24753.30	0.06	0.04	R	F	1992-09-03	1992-10-25	1992-09-11	TAKE BACK RETURN         	FOB       	ular requests boost about the packages. r
4864	150	9	1	28	29404.20	0.06	0.08	A	F	1993-02-06	1992-12-15	1993-02-10	COLLECT COD              	REG AIR   	thely around the bli
4864	38	4	2	38	35645.14	0.10	0.02	R	F	1992-12-20	1993-01-07	1993-01-06	TAKE BACK RETURN         	SHIP      	ording to the ironic, ir
4864	133	4	3	45	46490.85	0.02	0.01	A	F	1992-11-17	1993-01-02	1992-11-26	COLLECT COD              	SHIP      	round the furiously careful pa
4864	31	2	4	46	42827.38	0.07	0.03	A	F	1993-02-24	1993-01-02	1993-03-17	TAKE BACK RETURN         	RAIL      	sts use carefully across the carefull
4866	11	8	1	9	8199.09	0.01	0.05	N	O	1997-08-30	1997-09-18	1997-09-24	TAKE BACK RETURN         	MAIL      	ven dependencies x-ray. quic
4866	102	3	2	1	1002.10	0.06	0.00	N	O	1997-10-15	1997-10-01	1997-11-14	TAKE BACK RETURN         	AIR       	latelets nag. q
4866	131	7	3	17	17529.21	0.07	0.00	N	O	1997-11-26	1997-10-11	1997-12-12	COLLECT COD              	TRUCK     	ess packages doubt. even somas wake f
4869	41	8	1	31	29172.24	0.10	0.01	A	F	1995-01-17	1994-11-30	1995-02-02	NONE                     	SHIP      	ins. always unusual ideas across the ir
4869	58	3	2	24	22993.20	0.09	0.06	A	F	1994-11-17	1994-11-07	1994-11-27	COLLECT COD              	MAIL      	olites cajole after the ideas. special t
4869	157	8	3	25	26428.75	0.00	0.05	R	F	1994-11-25	1994-11-14	1994-12-19	DELIVER IN PERSON        	AIR       	e according t
4869	103	8	4	24	24074.40	0.10	0.07	R	F	1994-11-23	1994-11-18	1994-12-11	DELIVER IN PERSON        	MAIL      	se deposits above the sly, q
4869	173	2	5	42	45073.14	0.07	0.04	R	F	1994-10-16	1994-12-10	1994-11-07	TAKE BACK RETURN         	REG AIR   	 slyly even instructions. 
4869	122	5	6	30	30663.60	0.00	0.05	A	F	1995-01-09	1994-11-20	1995-02-02	COLLECT COD              	RAIL      	gedly even requests. s
4899	34	10	1	14	13076.42	0.06	0.00	R	F	1993-11-10	1994-01-10	1993-11-20	NONE                     	REG AIR   	 foxes eat
4900	116	3	1	40	40644.40	0.10	0.03	A	F	1992-09-02	1992-09-25	1992-09-21	COLLECT COD              	TRUCK     	heodolites. request
4900	77	8	2	33	32243.31	0.06	0.06	R	F	1992-08-18	1992-09-20	1992-08-19	COLLECT COD              	MAIL      	nto beans nag slyly reg
4900	103	8	3	48	48148.80	0.02	0.00	R	F	1992-09-18	1992-08-14	1992-09-28	TAKE BACK RETURN         	MAIL      	uickly ironic ideas kindle s
4900	32	3	4	20	18640.60	0.05	0.00	R	F	1992-09-22	1992-09-23	1992-09-27	TAKE BACK RETURN         	MAIL      	yers. accounts affix somet
4900	105	8	5	40	40204.00	0.03	0.02	R	F	1992-07-14	1992-09-05	1992-07-20	NONE                     	REG AIR   	luffily final dol
4900	103	6	6	46	46142.60	0.06	0.08	R	F	1992-07-11	1992-09-19	1992-07-16	TAKE BACK RETURN         	SHIP      	ly final acco
4901	141	10	1	37	38522.18	0.00	0.04	N	O	1998-01-26	1998-02-20	1998-01-31	DELIVER IN PERSON        	TRUCK     	 furiously ev
4901	165	4	2	12	12781.92	0.00	0.04	N	O	1998-01-12	1998-02-06	1998-02-03	COLLECT COD              	REG AIR   	y unusual deposits prom
4901	120	4	3	16	16321.92	0.05	0.08	N	O	1998-04-19	1998-03-18	1998-04-21	NONE                     	AIR       	deposits. blithely fin
4901	36	7	4	41	38377.23	0.03	0.00	N	O	1998-03-18	1998-02-18	1998-04-14	TAKE BACK RETURN         	AIR       	efully bold packages affix carefully eve
4901	116	7	5	40	40644.40	0.06	0.02	N	O	1998-01-08	1998-01-30	1998-01-15	DELIVER IN PERSON        	MAIL      	ect across the furiou
4929	14	1	1	20	18280.20	0.00	0.04	N	O	1996-03-12	1996-05-23	1996-03-20	COLLECT COD              	REG AIR   	 final pinto beans detect. final,
4929	79	7	2	40	39162.80	0.08	0.03	N	O	1996-05-30	1996-04-13	1996-06-22	TAKE BACK RETURN         	AIR       	unts against 
4929	77	7	3	32	31266.24	0.08	0.02	N	O	1996-04-28	1996-05-23	1996-04-30	COLLECT COD              	TRUCK     	usly at the blithely pending pl
4929	109	4	4	26	26236.60	0.00	0.05	N	O	1996-06-10	1996-05-29	1996-06-26	DELIVER IN PERSON        	RAIL      	 slyly. fl
4929	67	8	5	24	23209.44	0.09	0.05	N	O	1996-04-15	1996-04-30	1996-05-09	NONE                     	MAIL      	 accounts boost
4930	187	8	1	35	38051.30	0.03	0.01	A	F	1994-07-09	1994-07-30	1994-07-15	NONE                     	RAIL      	lose slyly regular dependencies. fur
4930	115	5	2	20	20302.20	0.02	0.04	A	F	1994-08-21	1994-06-17	1994-08-24	COLLECT COD              	FOB       	he carefully
4930	168	7	3	28	29908.48	0.00	0.08	R	F	1994-08-27	1994-06-27	1994-09-18	COLLECT COD              	TRUCK     	e ironic, unusual courts. regula
4930	166	7	4	42	44778.72	0.00	0.00	A	F	1994-06-18	1994-06-22	1994-07-10	COLLECT COD              	AIR       	ions haggle. furiously regular ideas use 
4930	190	1	5	38	41427.22	0.02	0.03	A	F	1994-06-06	1994-06-18	1994-07-03	TAKE BACK RETURN         	AIR       	bold requests sleep never
4960	18	5	1	36	33048.36	0.01	0.05	R	F	1995-03-06	1995-05-04	1995-04-05	TAKE BACK RETURN         	RAIL      	c, unusual accou
4960	45	8	2	6	5670.24	0.03	0.08	R	F	1995-03-21	1995-05-13	1995-04-14	TAKE BACK RETURN         	SHIP      	ual package
4960	149	8	3	9	9442.26	0.01	0.03	A	F	1995-03-20	1995-05-05	1995-04-17	COLLECT COD              	RAIL      	e blithely carefully fina
4960	120	7	4	14	14281.68	0.00	0.06	A	F	1995-04-03	1995-04-17	1995-04-07	NONE                     	RAIL      	accounts. warhorses are. grouches 
4960	98	1	5	8	7984.72	0.07	0.04	R	F	1995-03-14	1995-04-18	1995-04-09	NONE                     	FOB       	as. busily regular packages nag. 
4960	146	7	6	37	38707.18	0.10	0.04	R	F	1995-05-23	1995-04-12	1995-06-01	DELIVER IN PERSON        	MAIL      	ending theodolites w
4960	170	1	7	42	44947.14	0.08	0.07	A	F	1995-04-19	1995-04-11	1995-05-08	NONE                     	SHIP      	s requests cajole. 
4961	44	7	1	38	35873.52	0.10	0.07	N	O	1998-07-09	1998-06-03	1998-07-11	TAKE BACK RETURN         	FOB       	e on the blithely bold accounts. unu
4961	60	5	2	1	960.06	0.08	0.08	N	O	1998-07-08	1998-05-25	1998-07-12	DELIVER IN PERSON        	MAIL      	s affix carefully silent dependen
4961	162	3	3	41	43548.56	0.02	0.02	N	O	1998-07-15	1998-06-15	1998-08-05	TAKE BACK RETURN         	REG AIR   	ily against the n
4961	100	3	4	10	10001.00	0.02	0.04	N	O	1998-04-15	1998-07-03	1998-04-18	DELIVER IN PERSON        	MAIL      	quests. regular, ironic ideas at the ironi
4963	168	5	1	38	40590.08	0.08	0.02	N	O	1996-12-25	1996-12-12	1997-01-02	COLLECT COD              	AIR       	tegrate daringly accou
4963	76	4	2	16	15617.12	0.00	0.03	N	O	1996-11-20	1997-01-13	1996-12-06	COLLECT COD              	MAIL      	 carefully slyly u
4966	76	6	1	10	9760.70	0.06	0.03	N	O	1996-09-23	1996-11-02	1996-10-07	TAKE BACK RETURN         	SHIP      	 requests. carefully pending requests
4966	194	6	2	6	6565.14	0.02	0.01	N	O	1996-12-09	1996-11-29	1996-12-30	NONE                     	AIR       	d deposits are sly excuses. slyly iro
4966	165	6	3	7	7456.12	0.00	0.01	N	O	1996-12-08	1996-10-09	1997-01-06	COLLECT COD              	MAIL      	ckly ironic tithe
4966	16	6	4	26	23816.26	0.08	0.03	N	O	1996-11-14	1996-11-29	1996-12-05	COLLECT COD              	REG AIR   	nt pearls haggle carefully slyly even 
4966	144	1	5	12	12529.68	0.02	0.07	N	O	1996-12-07	1996-11-23	1996-12-20	DELIVER IN PERSON        	RAIL      	eodolites. ironic requests across the exp
4994	156	8	1	36	38021.40	0.00	0.06	N	O	1996-09-29	1996-07-30	1996-10-03	TAKE BACK RETURN         	TRUCK     	ess ideas. blithely silent brai
4994	80	9	2	47	46063.76	0.04	0.05	N	O	1996-09-20	1996-08-04	1996-10-15	COLLECT COD              	TRUCK     	sts. blithely close ideas sleep quic
4994	183	4	3	29	31412.22	0.08	0.01	N	O	1996-08-26	1996-09-27	1996-09-25	DELIVER IN PERSON        	RAIL      	ptotes boost carefully
4994	39	10	4	40	37561.20	0.01	0.06	N	O	1996-08-25	1996-08-16	1996-09-07	TAKE BACK RETURN         	REG AIR   	eposits. regula
4994	42	9	5	24	22608.96	0.01	0.07	N	O	1996-08-19	1996-09-24	1996-08-25	TAKE BACK RETURN         	FOB       	s. slyly ironic deposits cajole f
4994	73	4	6	6	5838.42	0.01	0.02	N	O	1996-09-05	1996-08-04	1996-09-30	TAKE BACK RETURN         	FOB       	grate carefully around th
4994	130	1	7	31	31934.03	0.07	0.04	N	O	1996-10-14	1996-09-23	1996-11-08	TAKE BACK RETURN         	RAIL      	lar decoys cajole fluffil
4996	56	1	1	35	33461.75	0.07	0.01	A	F	1992-10-30	1992-10-27	1992-11-05	TAKE BACK RETURN         	SHIP      	s. unusual, regular dolphins integrate care
4996	156	7	2	39	41189.85	0.02	0.07	A	F	1992-09-19	1992-10-19	1992-10-06	COLLECT COD              	FOB       	equests are carefully final
4996	128	7	3	12	12337.44	0.04	0.06	R	F	1993-01-09	1992-11-22	1993-02-04	DELIVER IN PERSON        	SHIP      	usly bold requests sleep dogge
4996	144	3	4	13	13573.82	0.00	0.00	A	F	1992-09-17	1992-12-02	1992-10-07	DELIVER IN PERSON        	TRUCK     	o beans use about the furious
4998	154	2	1	12	12649.80	0.04	0.03	A	F	1992-02-20	1992-03-06	1992-03-01	TAKE BACK RETURN         	RAIL      	 sleep slyly furiously final accounts. ins
4998	183	4	2	15	16247.70	0.06	0.00	R	F	1992-04-24	1992-03-21	1992-05-02	NONE                     	REG AIR   	heodolites sleep quickly.
4998	59	10	3	27	25894.35	0.06	0.02	R	F	1992-03-17	1992-02-26	1992-04-05	DELIVER IN PERSON        	MAIL      	the blithely ironic 
4998	63	10	4	47	45263.82	0.10	0.04	A	F	1992-02-07	1992-03-07	1992-02-19	DELIVER IN PERSON        	TRUCK     	mong the careful
4998	145	4	5	24	25083.36	0.01	0.04	R	F	1992-01-25	1992-03-16	1992-01-27	COLLECT COD              	REG AIR   	 unwind about
4998	99	1	6	8	7992.72	0.03	0.07	A	F	1992-05-01	1992-03-03	1992-05-24	TAKE BACK RETURN         	AIR       	ions nag quickly according to the theodolit
5024	166	3	1	17	18124.72	0.10	0.02	N	O	1996-11-24	1997-01-10	1996-12-04	NONE                     	AIR       	 to the expre
5024	58	6	2	41	39280.05	0.06	0.01	N	O	1996-11-09	1996-12-03	1996-12-01	COLLECT COD              	REG AIR   	osits hinder carefully 
5024	112	6	3	18	18217.98	0.04	0.03	N	O	1996-12-02	1997-01-16	1996-12-05	NONE                     	MAIL      	zle carefully sauternes. quickly
5024	123	8	4	42	42971.04	0.03	0.06	N	O	1996-12-02	1996-12-08	1996-12-04	DELIVER IN PERSON        	RAIL      	tegrate. busily spec
5025	30	9	1	11	10230.33	0.00	0.04	N	O	1997-02-21	1997-04-16	1997-03-14	COLLECT COD              	SHIP      	the carefully final esc
5025	78	7	2	10	9780.70	0.07	0.04	N	O	1997-06-04	1997-04-29	1997-06-28	COLLECT COD              	RAIL      	lly silent deposits boost busily again
5029	154	5	1	17	17920.55	0.02	0.01	A	F	1993-03-12	1992-12-18	1993-04-02	DELIVER IN PERSON        	FOB       	! packages boost blithely. furious
5029	97	9	2	2	1994.18	0.00	0.04	A	F	1992-11-25	1993-01-04	1992-12-20	DELIVER IN PERSON        	MAIL      	packages. furiously ironi
5059	70	5	1	5	4850.35	0.03	0.08	R	F	1993-12-23	1994-01-12	1993-12-24	TAKE BACK RETURN         	FOB       	ts affix slyly accordi
5059	123	2	2	19	19439.28	0.06	0.04	R	F	1994-03-02	1993-12-26	1994-03-14	TAKE BACK RETURN         	MAIL      	 special ideas poach blithely qu
5059	77	7	3	45	43968.15	0.02	0.00	A	F	1994-01-28	1994-01-08	1994-02-18	DELIVER IN PERSON        	MAIL      	enly. requests doze. express, close pa
5060	25	8	1	27	24975.54	0.10	0.07	R	F	1992-07-23	1992-09-05	1992-08-07	COLLECT COD              	SHIP      	s. ironic 
5060	32	8	2	28	26096.84	0.04	0.04	R	F	1992-09-25	1992-08-11	1992-10-09	NONE                     	REG AIR   	c requests
5060	161	2	3	15	15917.40	0.06	0.01	A	F	1992-08-28	1992-08-20	1992-09-01	DELIVER IN PERSON        	AIR       	ular deposits sl
5062	101	4	1	9	9009.90	0.08	0.00	R	F	1993-01-02	1992-12-01	1993-01-20	TAKE BACK RETURN         	MAIL      	 silent theodolites wake. c
5062	75	6	2	4	3900.28	0.02	0.02	R	F	1993-02-06	1992-12-14	1993-03-03	DELIVER IN PERSON        	AIR       	ke furiously express theodolites. 
5062	159	10	3	50	52957.50	0.09	0.07	A	F	1992-12-25	1992-12-13	1992-12-29	TAKE BACK RETURN         	MAIL      	 the regular, unusual pains. specia
5062	161	10	4	18	19100.88	0.03	0.07	R	F	1992-11-04	1992-12-25	1992-11-05	NONE                     	SHIP      	furiously pending requests are ruthles
5062	194	8	5	25	27354.75	0.08	0.02	R	F	1992-12-15	1992-11-17	1993-01-01	NONE                     	TRUCK     	uthless excuses ag
5090	22	3	1	22	20284.44	0.07	0.00	N	O	1997-05-10	1997-05-25	1997-05-24	TAKE BACK RETURN         	TRUCK     	ets integrate ironic, regul
5090	129	10	2	46	47339.52	0.05	0.00	N	O	1997-04-05	1997-04-14	1997-05-01	COLLECT COD              	REG AIR   	lose theodolites sleep blit
5090	2	9	3	22	19844.00	0.09	0.05	N	O	1997-07-03	1997-04-12	1997-07-26	NONE                     	REG AIR   	ular requests su
5090	114	8	4	2	2028.22	0.03	0.06	N	O	1997-04-07	1997-04-23	1997-05-01	TAKE BACK RETURN         	AIR       	tes. slowly iro
5090	48	9	5	21	19908.84	0.10	0.02	N	O	1997-03-29	1997-04-24	1997-04-25	TAKE BACK RETURN         	FOB       	ly express accounts. slyly even r
5090	80	9	6	30	29402.40	0.02	0.03	N	O	1997-05-04	1997-04-14	1997-05-30	COLLECT COD              	MAIL      	osits nag slyly. fluffily ex
5093	168	9	1	40	42726.40	0.05	0.01	R	F	1993-09-16	1993-11-04	1993-10-05	TAKE BACK RETURN         	REG AIR   	ing pinto beans. quickly bold dependenci
5093	74	2	2	15	14611.05	0.01	0.04	A	F	1993-12-02	1993-11-18	1994-01-01	DELIVER IN PERSON        	FOB       	ly among the unusual foxe
5093	151	9	3	31	32585.65	0.00	0.02	R	F	1993-09-22	1993-11-14	1993-09-26	TAKE BACK RETURN         	REG AIR   	 against the
5093	156	1	4	37	39077.55	0.04	0.00	A	F	1993-10-26	1993-12-02	1993-10-27	NONE                     	TRUCK     	courts. qui
5093	115	2	5	30	30453.30	0.06	0.05	A	F	1993-11-22	1993-11-27	1993-12-14	DELIVER IN PERSON        	TRUCK     	ithely ironic sheaves use fluff
5093	121	6	6	31	31654.72	0.01	0.08	A	F	1993-12-17	1993-11-14	1994-01-02	NONE                     	SHIP      	he final foxes. fluffily ironic 
5095	65	10	1	46	44392.76	0.07	0.01	A	F	1992-06-26	1992-06-25	1992-07-05	COLLECT COD              	RAIL      	egular instruction
5095	106	3	2	2	2012.20	0.07	0.08	A	F	1992-07-09	1992-05-25	1992-07-21	DELIVER IN PERSON        	REG AIR   	detect car
5095	123	8	3	28	28647.36	0.01	0.04	A	F	1992-06-20	1992-06-27	1992-06-22	DELIVER IN PERSON        	AIR       	 into the final courts. ca
5095	178	7	4	42	45283.14	0.08	0.08	R	F	1992-05-23	1992-06-01	1992-06-18	COLLECT COD              	TRUCK     	ccounts. packages could have t
5095	166	7	5	9	9595.44	0.10	0.07	R	F	1992-08-14	1992-06-23	1992-08-16	TAKE BACK RETURN         	REG AIR   	bold theodolites wake about the expr
5095	97	8	6	15	14956.35	0.01	0.06	A	F	1992-07-11	1992-07-12	1992-08-09	COLLECT COD              	AIR       	 to the packages wake sly
5095	169	10	7	40	42766.40	0.05	0.02	A	F	1992-07-11	1992-06-07	1992-07-26	DELIVER IN PERSON        	MAIL      	carefully unusual plat
5121	184	5	1	23	24936.14	0.06	0.01	A	F	1992-05-18	1992-06-20	1992-06-02	TAKE BACK RETURN         	REG AIR   	even courts are blithely ironically 
5121	111	1	2	45	45499.95	0.08	0.04	A	F	1992-08-13	1992-07-27	1992-09-12	NONE                     	TRUCK     	pecial accounts cajole ca
5121	97	10	3	27	26921.43	0.08	0.07	R	F	1992-06-17	1992-06-11	1992-06-19	NONE                     	MAIL      	ly silent theodolit
5121	68	7	4	10	9680.60	0.04	0.05	R	F	1992-06-08	1992-07-10	1992-07-02	TAKE BACK RETURN         	FOB       	e quickly according 
5121	89	10	5	46	45497.68	0.03	0.02	R	F	1992-05-27	1992-07-19	1992-05-28	TAKE BACK RETURN         	FOB       	use express foxes. slyly 
5121	1	8	6	2	1802.00	0.04	0.07	R	F	1992-08-10	1992-06-28	1992-08-11	NONE                     	FOB       	 final, regular account
5122	183	4	1	28	30329.04	0.03	0.00	N	O	1996-04-20	1996-03-29	1996-04-29	DELIVER IN PERSON        	FOB       	g the busily ironic accounts boos
5122	82	3	2	43	42229.44	0.09	0.03	N	O	1996-05-31	1996-04-12	1996-06-13	NONE                     	MAIL      	ut the carefully special foxes. idle,
5122	45	6	3	12	11340.48	0.07	0.03	N	O	1996-04-02	1996-04-27	1996-04-10	DELIVER IN PERSON        	AIR       	lar instructions 
5126	24	3	1	33	30492.66	0.02	0.02	R	F	1993-02-04	1992-12-23	1993-02-14	NONE                     	RAIL      	ipliers promise furiously whithout the 
5126	101	6	2	43	43047.30	0.09	0.04	R	F	1993-01-07	1992-12-19	1993-01-16	COLLECT COD              	MAIL      	e silently. ironic, unusual accounts
5126	78	8	3	23	22495.61	0.08	0.01	R	F	1993-01-02	1993-01-02	1993-01-05	COLLECT COD              	TRUCK     	egular, blithe packages.
5152	105	2	1	9	9045.90	0.04	0.03	N	O	1997-04-11	1997-02-11	1997-04-18	COLLECT COD              	AIR       	 cajole furiously alongside of the bo
5152	134	10	2	50	51706.50	0.04	0.04	N	O	1997-03-10	1997-02-04	1997-03-15	COLLECT COD              	FOB       	 the final deposits. slyly ironic warth
5154	190	1	1	11	11992.09	0.02	0.05	N	O	1997-08-06	1997-06-30	1997-09-04	NONE                     	RAIL      	luffily bold foxes. final
5154	144	5	2	15	15662.10	0.07	0.08	N	O	1997-06-23	1997-07-11	1997-07-11	NONE                     	AIR       	even packages. packages use
5155	48	9	1	1	948.04	0.00	0.00	A	F	1994-07-03	1994-08-11	1994-07-29	COLLECT COD              	FOB       	oze slyly after the silent, regular idea
5155	188	9	2	5	5440.90	0.08	0.02	A	F	1994-06-30	1994-08-13	1994-07-15	TAKE BACK RETURN         	AIR       	ole blithely slyly ironic 
5155	106	3	3	28	28170.80	0.05	0.02	R	F	1994-07-01	1994-07-19	1994-07-18	COLLECT COD              	REG AIR   	s cajole. accounts wake. thinly quiet pla
5155	79	7	4	39	38183.73	0.09	0.06	A	F	1994-08-25	1994-09-01	1994-09-18	COLLECT COD              	TRUCK     	l dolphins nag caref
5156	117	4	1	21	21359.31	0.06	0.03	N	O	1997-01-01	1997-01-30	1997-01-11	TAKE BACK RETURN         	TRUCK     	ts detect against the furiously reg
5156	148	1	2	36	37733.04	0.04	0.07	N	O	1997-02-12	1996-12-10	1997-03-13	TAKE BACK RETURN         	REG AIR   	 slyly even orbi
5190	56	1	1	43	41110.15	0.09	0.06	A	F	1992-08-19	1992-06-10	1992-09-01	DELIVER IN PERSON        	FOB       	encies use fluffily unusual requests? hoc
5190	132	3	2	6	6192.78	0.10	0.08	A	F	1992-08-08	1992-07-14	1992-08-22	COLLECT COD              	RAIL      	furiously regular pinto beans. furiously i
5190	89	10	3	45	44508.60	0.04	0.03	A	F	1992-07-23	1992-06-16	1992-08-04	NONE                     	FOB       	y carefully final ideas. f
5191	115	6	1	41	41619.51	0.00	0.08	A	F	1995-02-05	1995-02-27	1995-02-15	DELIVER IN PERSON        	AIR       	uests! ironic theodolites cajole care
5191	168	7	2	40	42726.40	0.02	0.01	A	F	1995-03-31	1995-02-21	1995-04-02	NONE                     	AIR       	nes haggle sometimes. requests eng
5191	43	4	3	27	25462.08	0.07	0.05	A	F	1994-12-26	1995-01-24	1995-01-14	DELIVER IN PERSON        	RAIL      	tructions nag bravely within the re
5191	183	4	4	7	7582.26	0.01	0.04	A	F	1995-03-24	1995-01-30	1995-03-30	NONE                     	RAIL      	eposits. express
5217	80	1	1	50	49004.00	0.05	0.02	N	O	1995-12-26	1995-11-21	1996-01-24	DELIVER IN PERSON        	MAIL      	s. express, express accounts c
5217	16	7	2	23	21068.23	0.06	0.07	N	O	1996-01-18	1995-12-24	1996-02-10	COLLECT COD              	RAIL      	ven ideas. requests amo
5217	102	7	3	23	23048.30	0.03	0.02	N	O	1995-11-15	1995-12-17	1995-11-27	DELIVER IN PERSON        	FOB       	pending packages cajole ne
5217	81	2	4	47	46110.76	0.04	0.00	N	O	1995-11-24	1995-12-25	1995-11-25	COLLECT COD              	AIR       	ronic packages i
5218	83	4	1	43	42272.44	0.05	0.04	A	F	1992-08-04	1992-09-12	1992-08-17	DELIVER IN PERSON        	SHIP      	k theodolites. express, even id
5218	125	4	2	33	33828.96	0.06	0.01	R	F	1992-09-16	1992-09-30	1992-09-27	NONE                     	TRUCK     	ronic instructi
5219	135	6	1	2	2070.26	0.08	0.00	N	O	1997-06-26	1997-04-29	1997-07-08	TAKE BACK RETURN         	FOB       	 blithely according to the stea
5219	119	9	2	20	20382.20	0.05	0.00	N	O	1997-04-20	1997-05-26	1997-05-13	COLLECT COD              	FOB       	e along the ironic,
5223	45	4	1	24	22680.96	0.00	0.00	A	F	1994-10-03	1994-09-20	1994-10-11	TAKE BACK RETURN         	TRUCK     	refully bold courts besides the regular,
5223	124	9	2	25	25603.00	0.09	0.02	R	F	1994-07-12	1994-08-13	1994-08-01	NONE                     	FOB       	y express ideas impress
5223	6	3	3	19	17214.00	0.04	0.01	R	F	1994-10-28	1994-08-26	1994-10-31	COLLECT COD              	REG AIR   	ntly. furiously even excuses a
5223	130	9	4	40	41205.20	0.01	0.04	R	F	1994-10-01	1994-09-18	1994-10-28	COLLECT COD              	SHIP      	kly pending 
5248	81	2	1	39	38262.12	0.05	0.03	N	O	1995-08-10	1995-07-04	1995-09-09	TAKE BACK RETURN         	MAIL      	yly even accounts. spe
5248	138	9	2	45	46715.85	0.00	0.06	A	F	1995-05-09	1995-07-12	1995-05-27	DELIVER IN PERSON        	FOB       	. bold, pending foxes h
5250	44	3	1	2	1888.08	0.08	0.04	N	O	1995-08-09	1995-10-10	1995-08-13	COLLECT COD              	AIR       	its. final pinto
5250	192	6	2	27	29489.13	0.10	0.05	N	O	1995-10-24	1995-09-03	1995-11-18	COLLECT COD              	TRUCK     	l forges are. furiously unusual pin
5251	139	10	1	36	37408.68	0.10	0.01	N	O	1995-07-16	1995-07-05	1995-07-28	DELIVER IN PERSON        	FOB       	slowly! bli
5254	111	2	1	35	35388.85	0.01	0.07	A	F	1992-07-28	1992-09-05	1992-08-07	COLLECT COD              	REG AIR   	ntegrate carefully among the pending
5254	135	6	2	10	10351.30	0.05	0.04	A	F	1992-11-19	1992-10-20	1992-12-15	COLLECT COD              	SHIP      	 accounts. silent deposit
5254	192	5	3	32	34950.08	0.00	0.08	A	F	1992-08-10	1992-09-21	1992-08-16	NONE                     	RAIL      	ts impress closely furi
5254	163	2	4	45	47842.20	0.05	0.06	A	F	1992-11-11	1992-09-01	1992-12-07	COLLECT COD              	REG AIR   	 wake. blithely silent excuse
5254	29	8	5	23	21367.46	0.02	0.06	A	F	1992-08-16	1992-09-05	1992-09-15	COLLECT COD              	RAIL      	lyly regular accounts. furiously pendin
5254	158	3	6	34	35977.10	0.09	0.02	R	F	1992-08-29	1992-10-16	1992-09-20	TAKE BACK RETURN         	RAIL      	 furiously above the furiously 
5254	20	7	7	9	8280.18	0.09	0.03	R	F	1992-07-29	1992-10-15	1992-08-20	TAKE BACK RETURN         	REG AIR   	 wake blithely fluff
5280	97	9	1	16	15953.44	0.02	0.03	N	O	1998-03-29	1998-01-28	1998-04-03	TAKE BACK RETURN         	SHIP      	 foxes are furiously. theodoli
5280	176	5	2	46	49503.82	0.01	0.06	N	O	1998-01-04	1998-01-21	1998-02-03	TAKE BACK RETURN         	FOB       	efully carefully pen
5281	114	1	1	37	37522.07	0.05	0.02	N	O	1995-11-10	1996-01-31	1995-11-22	DELIVER IN PERSON        	MAIL      	ronic dependencies. fluffily final p
5281	105	2	2	38	38193.80	0.00	0.05	N	O	1996-02-17	1995-12-19	1996-02-29	NONE                     	RAIL      	n asymptotes could wake about th
5281	127	2	3	23	23623.76	0.08	0.00	N	O	1995-12-30	1996-01-26	1996-01-23	COLLECT COD              	REG AIR   	. final theodolites cajole. ironic p
5281	87	8	4	48	47379.84	0.03	0.05	N	O	1996-01-31	1995-12-23	1996-02-08	TAKE BACK RETURN         	REG AIR   	ss the furiously 
5281	43	10	5	33	31120.32	0.01	0.07	N	O	1996-03-01	1995-12-28	1996-03-05	COLLECT COD              	RAIL      	ly brave foxes. bold deposits above the 
5284	173	1	1	16	17170.72	0.04	0.02	N	O	1995-08-17	1995-08-23	1995-08-26	DELIVER IN PERSON        	TRUCK     	unts detect furiously even d
5284	44	7	2	24	22656.96	0.03	0.08	N	O	1995-10-21	1995-08-23	1995-10-27	COLLECT COD              	AIR       	 haggle according 
5285	193	5	1	31	33888.89	0.08	0.00	A	F	1994-04-17	1994-04-05	1994-05-09	NONE                     	RAIL      	ubt. quickly blithe 
5285	31	2	2	37	34448.11	0.09	0.02	R	F	1994-02-26	1994-02-18	1994-03-27	NONE                     	SHIP      	uffily regu
5285	34	10	3	24	22416.72	0.02	0.04	A	F	1994-04-19	1994-04-03	1994-04-25	DELIVER IN PERSON        	FOB       	ess packages. quick, even deposits snooze b
5285	43	2	4	12	11316.48	0.05	0.06	A	F	1994-04-22	1994-04-07	1994-05-19	NONE                     	AIR       	 deposits-- quickly bold requests hag
5285	71	2	5	1	971.07	0.03	0.05	R	F	1994-03-14	1994-02-20	1994-04-10	COLLECT COD              	TRUCK     	e fluffily about the slyly special pa
5285	146	7	6	1	1046.14	0.06	0.01	R	F	1994-02-08	1994-04-02	1994-02-17	COLLECT COD              	SHIP      	ing deposits integra
5286	199	1	1	1	1099.19	0.01	0.07	N	O	1997-11-25	1997-11-07	1997-12-17	COLLECT COD              	REG AIR   	ly! furiously final pack
5286	97	1	2	7	6979.63	0.06	0.05	N	O	1997-10-23	1997-12-10	1997-11-20	TAKE BACK RETURN         	RAIL      	y express instructions sleep carefull
5286	16	10	3	3	2748.03	0.06	0.08	N	O	1997-12-04	1997-11-06	1997-12-09	COLLECT COD              	MAIL      	re fluffily
5286	40	6	4	6	5640.24	0.04	0.03	N	O	1997-10-15	1997-12-05	1997-11-12	COLLECT COD              	RAIL      	y special a
5286	186	7	5	38	41274.84	0.07	0.05	N	O	1997-11-29	1997-11-26	1997-12-15	TAKE BACK RETURN         	SHIP      	fluffily. special, ironic deposit
5286	138	9	6	24	24915.12	0.08	0.00	N	O	1997-09-27	1997-12-21	1997-09-30	COLLECT COD              	TRUCK     	s. express foxes of the
5312	61	6	1	27	25948.62	0.04	0.08	A	F	1995-04-20	1995-04-09	1995-04-25	COLLECT COD              	SHIP      	tructions cajol
5312	2	5	2	43	38786.00	0.05	0.08	A	F	1995-03-24	1995-05-07	1995-03-28	NONE                     	TRUCK     	ly unusual
5314	118	9	1	10	10181.10	0.07	0.05	N	O	1995-09-26	1995-07-24	1995-10-19	DELIVER IN PERSON        	RAIL      	latelets haggle final
5314	125	6	2	16	16401.92	0.00	0.04	N	O	1995-09-25	1995-07-08	1995-10-17	COLLECT COD              	SHIP      	hely unusual packages acc
5316	108	1	1	29	29234.90	0.10	0.05	R	F	1994-03-28	1994-04-29	1994-04-09	DELIVER IN PERSON        	REG AIR   	ckly unusual foxes bo
5316	136	7	2	31	32120.03	0.00	0.08	A	F	1994-04-01	1994-04-21	1994-04-12	DELIVER IN PERSON        	MAIL      	s. deposits cajole around t
5344	19	3	1	6	5514.06	0.07	0.01	N	O	1998-08-04	1998-09-03	1998-08-11	TAKE BACK RETURN         	REG AIR   	ithely about the pending plate
5344	79	9	2	37	36225.59	0.03	0.07	N	O	1998-10-09	1998-07-26	1998-11-08	NONE                     	TRUCK     	thely express packages
5344	67	8	3	26	25143.56	0.02	0.06	N	O	1998-08-27	1998-08-22	1998-09-24	NONE                     	AIR       	furiously pending, silent multipliers.
5344	39	10	4	21	19719.63	0.03	0.01	N	O	1998-08-31	1998-09-06	1998-09-02	NONE                     	MAIL      	xes. furiously even pinto beans sleep f
5345	83	4	1	3	2949.24	0.05	0.01	N	O	1997-12-10	1997-10-03	1998-01-05	COLLECT COD              	SHIP      	ites wake carefully unusual 
5345	146	5	2	2	2092.28	0.10	0.02	N	O	1997-11-18	1997-10-12	1997-12-08	NONE                     	MAIL      	ut the slyly specia
5345	192	5	3	46	50240.74	0.06	0.04	N	O	1997-10-06	1997-09-27	1997-10-18	COLLECT COD              	REG AIR   	slyly special deposits. fin
5345	114	4	4	37	37522.07	0.01	0.01	N	O	1997-11-01	1997-10-09	1997-11-26	DELIVER IN PERSON        	AIR       	 along the ironically fina
5345	34	10	5	22	20548.66	0.02	0.02	N	O	1997-08-27	1997-11-22	1997-09-10	TAKE BACK RETURN         	MAIL      	leep slyly regular fox
5348	69	4	1	21	20350.26	0.10	0.04	N	O	1997-12-11	1997-12-24	1997-12-28	NONE                     	REG AIR   	 regular theodolites haggle car
5348	156	1	2	31	32740.65	0.07	0.02	N	O	1998-01-04	1997-12-09	1998-01-17	COLLECT COD              	RAIL      	are finally
5348	17	8	3	16	14672.16	0.06	0.08	N	O	1998-02-28	1997-12-25	1998-03-12	DELIVER IN PERSON        	AIR       	uriously thin pinto beans 
5348	20	4	4	7	6440.14	0.04	0.00	N	O	1998-01-29	1997-12-20	1998-02-10	DELIVER IN PERSON        	RAIL      	even foxes. epitap
5348	2	5	5	37	33374.00	0.06	0.07	N	O	1997-12-01	1998-02-02	1997-12-07	NONE                     	FOB       	y according to the carefully pending acco
5348	143	10	6	14	14603.96	0.06	0.05	N	O	1997-12-16	1998-01-12	1997-12-24	COLLECT COD              	FOB       	en pinto beans. somas cajo
5376	61	6	1	42	40364.52	0.10	0.04	A	F	1994-09-20	1994-08-30	1994-09-29	TAKE BACK RETURN         	REG AIR   	y even asymptotes. courts are unusual pa
5376	91	4	2	44	43607.96	0.05	0.02	R	F	1994-08-30	1994-08-05	1994-09-07	COLLECT COD              	AIR       	ithe packages detect final theodolites. f
5376	65	6	3	18	17371.08	0.02	0.08	A	F	1994-10-29	1994-09-13	1994-11-01	COLLECT COD              	MAIL      	 accounts boo
5378	155	3	1	39	41150.85	0.07	0.04	R	F	1992-11-25	1992-12-22	1992-12-02	COLLECT COD              	AIR       	ts are quickly around the
5378	62	9	2	46	44254.76	0.01	0.04	A	F	1993-02-17	1993-01-20	1993-02-26	COLLECT COD              	REG AIR   	into beans sleep. fu
5378	10	7	3	18	16380.18	0.02	0.03	R	F	1992-11-25	1992-12-21	1992-12-10	COLLECT COD              	FOB       	onic accounts was bold, 
5379	199	1	1	40	43967.60	0.01	0.08	N	O	1995-10-01	1995-10-19	1995-10-30	COLLECT COD              	MAIL      	carefully final accounts haggle blithely. 
5381	188	9	1	37	40262.66	0.04	0.01	A	F	1993-04-08	1993-04-07	1993-04-12	DELIVER IN PERSON        	SHIP      	ly final deposits print carefully. unusua
5381	111	8	2	48	48533.28	0.04	0.03	R	F	1993-04-22	1993-04-17	1993-05-14	TAKE BACK RETURN         	FOB       	luffily spec
5381	192	3	3	13	14198.47	0.08	0.03	R	F	1993-05-09	1993-04-26	1993-05-25	NONE                     	FOB       	s after the f
5381	168	3	4	17	18158.72	0.05	0.05	R	F	1993-05-25	1993-04-14	1993-06-17	NONE                     	MAIL      	ckly final requests haggle qui
5381	63	8	5	49	47189.94	0.06	0.02	R	F	1993-05-08	1993-04-07	1993-06-03	NONE                     	FOB       	 accounts. regular, regula
5381	132	3	6	33	34060.29	0.10	0.00	A	F	1993-04-09	1993-04-03	1993-04-22	DELIVER IN PERSON        	SHIP      	ly special deposits 
5381	44	3	7	31	29265.24	0.04	0.05	A	F	1993-04-10	1993-03-22	1993-04-13	TAKE BACK RETURN         	MAIL      	the carefully expre
5382	153	8	1	34	35807.10	0.03	0.03	R	F	1992-02-22	1992-02-18	1992-03-02	DELIVER IN PERSON        	FOB       	gular accounts. even accounts integrate
5382	55	3	2	13	12415.65	0.09	0.06	A	F	1992-01-16	1992-03-12	1992-02-06	NONE                     	MAIL      	eodolites. final foxes 
5382	149	10	3	3	3147.42	0.10	0.06	A	F	1992-03-22	1992-03-06	1992-04-19	TAKE BACK RETURN         	AIR       	efully unusua
5382	62	9	4	20	19241.20	0.08	0.02	A	F	1992-03-26	1992-02-17	1992-04-15	DELIVER IN PERSON        	FOB       	carefully regular accounts. slyly ev
5382	177	8	5	14	15080.38	0.02	0.02	A	F	1992-04-05	1992-04-05	1992-05-04	TAKE BACK RETURN         	FOB       	 brave platelets. ev
5382	180	9	6	6	6481.08	0.02	0.01	A	F	1992-03-07	1992-04-02	1992-03-18	TAKE BACK RETURN         	FOB       	y final foxes by the sl
5382	105	2	7	48	48244.80	0.05	0.05	A	F	1992-02-14	1992-03-19	1992-02-25	DELIVER IN PERSON        	REG AIR   	nts integrate quickly ca
5383	96	7	1	12	11953.08	0.04	0.00	N	O	1995-07-02	1995-08-16	1995-08-01	TAKE BACK RETURN         	AIR       	y regular instructi
5411	96	9	1	17	16933.53	0.05	0.01	N	O	1997-07-22	1997-07-14	1997-07-30	TAKE BACK RETURN         	REG AIR   	 slyly slyly even deposits. carefully b
5411	113	7	2	10	10131.10	0.08	0.01	N	O	1997-07-19	1997-08-04	1997-07-26	TAKE BACK RETURN         	MAIL      	nding, special foxes unw
5411	56	7	3	5	4780.25	0.10	0.01	N	O	1997-09-12	1997-08-03	1997-09-23	DELIVER IN PERSON        	FOB       	 bold, ironic theodo
5411	129	8	4	15	15436.80	0.08	0.05	N	O	1997-07-01	1997-07-15	1997-07-07	COLLECT COD              	RAIL      	attainments sleep slyly ironic
5411	4	5	5	19	17176.00	0.05	0.08	N	O	1997-05-25	1997-07-30	1997-06-19	COLLECT COD              	RAIL      	ial accounts according to the f
5414	68	9	1	40	38722.40	0.07	0.06	R	F	1993-04-07	1993-05-18	1993-04-23	COLLECT COD              	AIR       	ts are evenly across
5414	123	8	2	48	49109.76	0.06	0.07	R	F	1993-06-08	1993-05-14	1993-07-06	DELIVER IN PERSON        	FOB       	 silent dolphins; fluffily regular tithe
5414	35	1	3	23	21505.69	0.10	0.00	A	F	1993-07-22	1993-05-26	1993-08-08	COLLECT COD              	MAIL      	e bold, express dolphins. spec
5414	133	4	4	15	15496.95	0.06	0.08	R	F	1993-05-18	1993-06-09	1993-05-27	DELIVER IN PERSON        	REG AIR   	e slyly about the carefully regula
5414	9	2	5	19	17271.00	0.01	0.05	R	F	1993-04-06	1993-05-12	1993-05-02	DELIVER IN PERSON        	RAIL      	ffily silent theodolites na
5414	98	1	6	28	27946.52	0.10	0.05	A	F	1993-03-27	1993-06-04	1993-04-07	TAKE BACK RETURN         	SHIP      	ts sleep sl
5442	42	5	1	16	15072.64	0.00	0.00	N	O	1998-04-12	1998-03-03	1998-05-04	TAKE BACK RETURN         	RAIL      	r packages. accounts haggle dependencies. f
5442	88	9	2	45	44463.60	0.08	0.01	N	O	1998-03-30	1998-02-24	1998-04-18	TAKE BACK RETURN         	AIR       	old slyly after 
5442	61	8	3	12	11532.72	0.01	0.08	N	O	1998-04-15	1998-03-18	1998-05-05	DELIVER IN PERSON        	TRUCK     	fully final
5442	158	9	4	21	22221.15	0.07	0.06	N	O	1998-03-13	1998-02-19	1998-04-06	COLLECT COD              	MAIL      	ffily furiously ironic theodolites. furio
5442	16	7	5	25	22900.25	0.04	0.00	N	O	1998-03-29	1998-02-13	1998-04-13	TAKE BACK RETURN         	REG AIR   	ake furiously. slyly express th
5442	144	3	6	26	27147.64	0.08	0.07	N	O	1998-03-21	1998-03-21	1998-03-25	TAKE BACK RETURN         	AIR       	have to sleep furiously bold ideas. blith
5446	190	1	1	27	29435.13	0.05	0.07	R	F	1994-07-21	1994-08-25	1994-08-17	TAKE BACK RETURN         	RAIL      	ously across the quic
5472	59	10	1	27	25894.35	0.09	0.06	A	F	1993-08-04	1993-07-07	1993-09-03	COLLECT COD              	TRUCK     	fily pending attainments. unus
5472	68	3	2	28	27105.68	0.00	0.03	A	F	1993-07-28	1993-05-28	1993-08-11	TAKE BACK RETURN         	FOB       	ffily pendin
5472	178	7	3	45	48517.65	0.06	0.02	R	F	1993-06-05	1993-05-14	1993-06-10	NONE                     	TRUCK     	 idle packages. furi
5472	184	5	4	37	40114.66	0.07	0.05	R	F	1993-06-15	1993-07-03	1993-07-09	DELIVER IN PERSON        	RAIL      	egrate carefully dependencies. 
5472	75	6	5	40	39002.80	0.02	0.05	A	F	1993-04-13	1993-07-04	1993-05-04	NONE                     	REG AIR   	e requests detect furiously. ruthlessly un
5472	167	2	6	39	41619.24	0.02	0.03	R	F	1993-04-18	1993-07-10	1993-05-12	TAKE BACK RETURN         	MAIL      	uriously carefully 
5472	15	5	7	1	915.01	0.03	0.02	A	F	1993-04-14	1993-06-28	1993-04-16	NONE                     	RAIL      	s use furiou
5473	48	5	1	9	8532.36	0.03	0.07	R	F	1992-06-03	1992-05-30	1992-06-09	TAKE BACK RETURN         	AIR       	 excuses sleep blithely! regular dep
5473	70	1	2	27	26191.89	0.01	0.03	A	F	1992-04-06	1992-04-26	1992-04-29	TAKE BACK RETURN         	MAIL      	the deposits. warthogs wake fur
5473	15	5	3	33	30195.33	0.09	0.00	R	F	1992-05-18	1992-06-10	1992-06-13	TAKE BACK RETURN         	MAIL      	efully above the even, 
5477	80	8	1	20	19601.60	0.03	0.01	N	O	1998-03-21	1998-02-09	1998-04-07	TAKE BACK RETURN         	SHIP      	platelets about the ironic
5477	77	7	2	21	20518.47	0.03	0.00	N	O	1998-01-28	1998-02-15	1998-02-24	TAKE BACK RETURN         	SHIP      	blate slyly. silent
5477	134	5	3	31	32058.03	0.04	0.01	N	O	1998-01-11	1998-01-30	1998-02-04	DELIVER IN PERSON        	MAIL      	 special Tiresias cajole furiously. pending
5477	193	6	4	16	17491.04	0.00	0.01	N	O	1998-03-07	1998-03-12	1998-04-06	COLLECT COD              	RAIL      	regular, s
5477	96	9	5	23	22910.07	0.00	0.06	N	O	1998-01-04	1998-02-23	1998-01-24	NONE                     	REG AIR   	telets wake blithely ab
5477	121	6	6	19	19401.28	0.10	0.03	N	O	1998-02-03	1998-01-30	1998-03-04	TAKE BACK RETURN         	MAIL      	ost carefully packages.
5478	8	9	1	39	35412.00	0.09	0.06	N	O	1996-08-19	1996-06-25	1996-09-08	DELIVER IN PERSON        	SHIP      	s. furiously 
5478	2	5	2	47	42394.00	0.10	0.01	N	O	1996-08-15	1996-07-12	1996-08-31	NONE                     	RAIL      	 instructions; slyly even accounts hagg
5478	119	3	3	25	25477.75	0.09	0.07	N	O	1996-06-08	1996-07-12	1996-07-07	NONE                     	TRUCK     	unusual, pending requests haggle accoun
5479	138	4	1	50	51906.50	0.02	0.02	A	F	1993-12-24	1994-02-14	1994-01-18	DELIVER IN PERSON        	MAIL      	ironic gifts. even dependencies sno
5479	104	5	2	19	19077.90	0.05	0.03	A	F	1994-01-22	1994-03-07	1994-02-11	DELIVER IN PERSON        	SHIP      	arefully bo
5504	68	5	1	4	3872.24	0.10	0.07	A	F	1993-04-30	1993-03-01	1993-05-22	DELIVER IN PERSON        	AIR       	into beans boost. 
5504	177	8	2	7	7540.19	0.03	0.05	R	F	1993-04-25	1993-03-15	1993-05-06	NONE                     	TRUCK     	packages detect furiously express reques
5504	160	2	3	29	30744.64	0.05	0.03	A	F	1993-01-28	1993-02-13	1993-02-27	NONE                     	SHIP      	ajole carefully. care
5505	25	8	1	43	39775.86	0.07	0.01	N	O	1997-12-30	1997-11-28	1998-01-09	TAKE BACK RETURN         	TRUCK     	y alongside of the special requests.
5505	182	3	2	33	35711.94	0.05	0.08	N	O	1998-01-11	1997-11-11	1998-01-30	TAKE BACK RETURN         	AIR       	ithely unusual excuses integrat
5505	155	10	3	10	10551.50	0.06	0.01	N	O	1997-10-28	1997-11-27	1997-10-29	DELIVER IN PERSON        	AIR       	 furiously special asym
5505	40	1	4	18	16920.72	0.04	0.04	N	O	1997-10-25	1997-12-12	1997-10-30	TAKE BACK RETURN         	RAIL      	 to the quickly express pac
5505	162	9	5	46	48859.36	0.05	0.00	N	O	1998-01-06	1997-11-04	1998-02-04	TAKE BACK RETURN         	SHIP      	usly ironic dependencies haggle across 
5507	10	5	1	23	20930.23	0.05	0.04	N	O	1998-09-04	1998-07-04	1998-09-18	TAKE BACK RETURN         	AIR       	ously slow packages poach whithout the
5507	138	9	2	48	49830.24	0.03	0.01	N	O	1998-08-03	1998-08-10	1998-08-24	DELIVER IN PERSON        	AIR       	yly idle deposits. final, final fox
5507	45	2	3	4	3780.16	0.04	0.06	N	O	1998-06-06	1998-07-02	1998-06-27	TAKE BACK RETURN         	RAIL      	into beans are
5507	67	6	4	22	21275.32	0.07	0.01	N	O	1998-07-08	1998-08-10	1998-07-22	DELIVER IN PERSON        	TRUCK     	gular ideas. carefully unu
5507	132	3	5	48	49542.24	0.06	0.01	N	O	1998-07-21	1998-07-15	1998-07-31	DELIVER IN PERSON        	SHIP      	uriously regular acc
5511	165	4	1	16	17042.56	0.10	0.05	A	F	1995-02-02	1995-01-06	1995-02-19	TAKE BACK RETURN         	RAIL      	thely bold theodolites 
5511	165	10	2	31	33019.96	0.09	0.01	A	F	1995-02-23	1995-01-21	1995-03-02	COLLECT COD              	REG AIR   	gular excuses. fluffily even pinto beans c
5511	128	3	3	49	50377.88	0.05	0.05	R	F	1994-12-21	1995-01-27	1994-12-26	NONE                     	REG AIR   	bout the requests. theodolites 
5511	122	7	4	4	4088.48	0.08	0.02	R	F	1994-12-28	1995-01-16	1995-01-24	TAKE BACK RETURN         	RAIL      	lphins. carefully blithe de
5511	9	2	5	23	20907.00	0.10	0.07	A	F	1995-03-11	1995-01-21	1995-03-27	TAKE BACK RETURN         	TRUCK     	ing dugouts 
5511	188	9	6	5	5440.90	0.08	0.05	R	F	1994-12-29	1995-01-16	1995-01-24	DELIVER IN PERSON        	MAIL      	al theodolites. blithely final de
5511	143	2	7	23	23992.22	0.02	0.07	R	F	1995-02-03	1995-01-05	1995-02-18	COLLECT COD              	REG AIR   	ully deposits. warthogs hagg
5536	90	1	1	14	13861.26	0.08	0.06	N	O	1998-05-18	1998-05-08	1998-06-05	COLLECT COD              	MAIL      	instructions sleep 
5536	62	1	2	20	19241.20	0.08	0.04	N	O	1998-05-08	1998-05-10	1998-05-31	DELIVER IN PERSON        	REG AIR   	equests mo
5536	197	9	3	35	38401.65	0.07	0.02	N	O	1998-05-19	1998-06-08	1998-06-05	NONE                     	MAIL      	c, final theo
5536	9	10	4	30	27270.00	0.05	0.07	N	O	1998-04-15	1998-05-23	1998-05-03	NONE                     	FOB       	arefully regular theodolites according
5536	141	2	5	11	11452.54	0.02	0.08	N	O	1998-03-18	1998-05-12	1998-03-28	TAKE BACK RETURN         	FOB       	 snooze furio
5537	45	8	1	10	9450.40	0.05	0.08	N	O	1997-01-13	1996-12-25	1997-01-28	TAKE BACK RETURN         	AIR       	 sleep carefully slyly bold depos
5537	150	9	2	15	15752.25	0.07	0.04	N	O	1997-01-13	1996-12-25	1997-01-27	COLLECT COD              	AIR       	eposits. permanently pending packag
5537	151	6	3	39	40994.85	0.03	0.00	N	O	1996-12-17	1996-11-08	1997-01-15	COLLECT COD              	REG AIR   	 slyly bold packages are. qu
5537	97	1	4	38	37889.42	0.01	0.00	N	O	1996-11-06	1996-11-23	1996-11-12	TAKE BACK RETURN         	MAIL      	s above the carefully ironic deposits 
5541	96	8	1	39	38847.51	0.08	0.05	N	O	1997-11-17	1997-12-27	1997-12-11	TAKE BACK RETURN         	RAIL      	ding theodolites haggle against the slyly 
5568	166	5	1	50	53308.00	0.05	0.05	N	O	1995-07-14	1995-09-04	1995-08-03	COLLECT COD              	TRUCK     	furious ide
5568	44	5	2	18	16992.72	0.01	0.08	N	O	1995-08-19	1995-08-18	1995-08-24	DELIVER IN PERSON        	SHIP      	structions haggle. carefully regular 
5568	89	10	3	35	34617.80	0.08	0.07	N	O	1995-09-17	1995-09-04	1995-10-14	NONE                     	SHIP      	lyly. blit
5569	29	4	1	25	23225.50	0.10	0.03	R	F	1993-06-29	1993-07-18	1993-07-05	TAKE BACK RETURN         	TRUCK     	 deposits cajole above
5569	58	10	2	26	24909.30	0.09	0.06	A	F	1993-08-21	1993-07-22	1993-09-09	DELIVER IN PERSON        	MAIL      	pitaphs. ironic req
5569	55	3	3	48	45842.40	0.02	0.03	R	F	1993-06-16	1993-06-15	1993-07-09	COLLECT COD              	SHIP      	the fluffily
5569	147	10	4	19	19895.66	0.10	0.08	R	F	1993-07-30	1993-06-21	1993-08-13	TAKE BACK RETURN         	FOB       	 detect ca
5569	59	1	5	15	14385.75	0.02	0.06	A	F	1993-06-29	1993-07-06	1993-07-05	DELIVER IN PERSON        	MAIL      	lithely bold requests boost fur
5570	161	6	1	37	39262.92	0.08	0.02	N	O	1996-08-29	1996-10-23	1996-09-11	NONE                     	RAIL      	y ironic pin
5570	39	10	2	15	14085.45	0.09	0.02	N	O	1996-10-04	1996-10-05	1996-10-28	TAKE BACK RETURN         	REG AIR   	beans nag slyly special, regular pack
5570	60	1	3	29	27841.74	0.02	0.05	N	O	1996-10-12	1996-10-20	1996-11-08	TAKE BACK RETURN         	SHIP      	he silent, enticing requests.
5572	22	1	1	24	22128.48	0.08	0.08	R	F	1994-10-30	1994-10-02	1994-11-27	TAKE BACK RETURN         	MAIL      	ests cajole. evenly ironic exc
5572	172	10	2	27	28948.59	0.03	0.04	A	F	1994-08-29	1994-09-10	1994-08-30	TAKE BACK RETURN         	SHIP      	 accounts. carefully final accoun
5572	87	8	3	19	18754.52	0.10	0.00	A	F	1994-08-12	1994-10-07	1994-09-01	DELIVER IN PERSON        	RAIL      	es. final, final requests wake blithely ag
5572	135	1	4	46	47615.98	0.02	0.01	R	F	1994-09-08	1994-10-14	1994-10-01	NONE                     	REG AIR   	ully regular platelet
5572	24	3	5	34	31416.68	0.10	0.08	R	F	1994-10-22	1994-08-16	1994-11-08	NONE                     	TRUCK     	asymptotes integrate. s
5572	101	2	6	14	14015.40	0.04	0.05	A	F	1994-11-02	1994-09-20	1994-11-03	COLLECT COD              	RAIL      	he fluffily express packages. fluffily fina
5572	26	1	7	24	22224.48	0.01	0.05	R	F	1994-09-26	1994-09-04	1994-10-22	DELIVER IN PERSON        	FOB       	 beans. foxes sleep fluffily across th
5600	187	8	1	34	36964.12	0.02	0.00	N	O	1997-03-22	1997-04-05	1997-04-09	TAKE BACK RETURN         	MAIL      	ly above the stealthy ideas. permane
5600	8	5	2	19	17252.00	0.00	0.01	N	O	1997-04-10	1997-03-24	1997-04-16	TAKE BACK RETURN         	TRUCK     	dencies. carefully p
5601	38	4	1	29	27202.87	0.09	0.04	A	F	1992-04-06	1992-02-24	1992-04-29	DELIVER IN PERSON        	TRUCK     	 ironic ideas. final
5601	164	1	2	45	47887.20	0.10	0.07	A	F	1992-03-25	1992-04-03	1992-04-04	TAKE BACK RETURN         	MAIL      	ts-- blithely final accounts cajole. carefu
5601	73	4	3	38	36976.66	0.07	0.00	A	F	1992-01-08	1992-03-01	1992-01-09	TAKE BACK RETURN         	REG AIR   	ter the evenly final deposit
5601	148	5	4	12	12577.68	0.03	0.01	A	F	1992-02-27	1992-03-16	1992-03-27	COLLECT COD              	TRUCK     	ep carefully a
5605	87	8	1	50	49354.00	0.08	0.05	N	O	1996-08-26	1996-10-15	1996-09-04	TAKE BACK RETURN         	RAIL      	instructions sleep carefully ironic req
5605	151	2	2	7	7358.05	0.06	0.01	N	O	1996-12-13	1996-10-13	1996-12-15	TAKE BACK RETURN         	FOB       	lowly special courts nag among the furi
5605	173	2	3	3	3219.51	0.01	0.02	N	O	1996-09-01	1996-10-02	1996-09-20	TAKE BACK RETURN         	AIR       	posits. accounts boost. t
5605	55	3	4	45	42977.25	0.00	0.01	N	O	1996-09-05	1996-10-04	1996-09-13	COLLECT COD              	FOB       	ly unusual instructions. carefully ironic p
5605	70	7	5	39	37832.73	0.00	0.08	N	O	1996-12-13	1996-11-03	1996-12-24	DELIVER IN PERSON        	REG AIR   	cial deposits. theodolites w
5605	166	7	6	29	30918.64	0.08	0.08	N	O	1996-09-19	1996-10-22	1996-10-06	DELIVER IN PERSON        	SHIP      	 quickly. quickly pending sen
5606	174	5	1	47	50485.99	0.10	0.04	N	O	1996-12-23	1997-01-31	1997-01-20	DELIVER IN PERSON        	REG AIR   	carefully final foxes. pending, final
5606	92	3	2	34	33731.06	0.09	0.06	N	O	1997-02-23	1997-02-08	1997-03-09	TAKE BACK RETURN         	REG AIR   	uses. slyly final 
5606	127	8	3	46	47247.52	0.04	0.00	N	O	1997-03-11	1997-01-13	1997-03-23	DELIVER IN PERSON        	REG AIR   	ter the ironic accounts. even, ironic depos
5606	82	3	4	30	29462.40	0.08	0.04	N	O	1997-02-06	1997-01-26	1997-02-16	DELIVER IN PERSON        	REG AIR   	 nag always. blithely express packages 
5606	7	2	5	25	22675.00	0.06	0.00	N	O	1996-12-25	1997-01-12	1997-01-11	TAKE BACK RETURN         	AIR       	breach about the furiously bold 
5606	154	5	6	3	3162.45	0.04	0.06	N	O	1997-01-11	1997-01-04	1997-02-08	COLLECT COD              	AIR       	 sauternes. asympto
5606	74	5	7	46	44807.22	0.07	0.01	N	O	1997-02-01	1997-01-31	1997-02-15	DELIVER IN PERSON        	TRUCK     	ow requests wake around the regular accoun
5632	10	3	1	48	43680.48	0.06	0.06	N	O	1996-05-08	1996-03-24	1996-06-04	TAKE BACK RETURN         	FOB       	unts. decoys u
5632	106	7	2	21	21128.10	0.02	0.08	N	O	1996-03-22	1996-03-10	1996-04-10	NONE                     	AIR       	refully regular pinto beans. ironic reques
5632	67	2	3	24	23209.44	0.04	0.06	N	O	1996-03-23	1996-04-02	1996-03-30	TAKE BACK RETURN         	MAIL      	beans detect. quickly final i
5633	160	2	1	28	29684.48	0.02	0.00	N	O	1998-08-14	1998-07-24	1998-08-26	TAKE BACK RETURN         	SHIP      	as boost quickly. unusual pinto 
5633	102	3	2	10	10021.00	0.09	0.04	N	O	1998-07-15	1998-08-03	1998-08-03	COLLECT COD              	AIR       	its cajole fluffily fluffily special pinto
5633	46	7	3	27	25543.08	0.03	0.02	N	O	1998-09-28	1998-07-28	1998-10-12	DELIVER IN PERSON        	AIR       	ructions. even ideas haggle carefully r
5633	164	5	4	50	53208.00	0.02	0.05	N	O	1998-07-23	1998-07-09	1998-08-21	DELIVER IN PERSON        	TRUCK     	ts. slyly regular 
5633	100	2	5	48	48004.80	0.01	0.05	N	O	1998-06-24	1998-07-22	1998-07-18	DELIVER IN PERSON        	TRUCK     	even courts haggle slyly at the requ
5633	107	2	6	1	1007.10	0.02	0.03	N	O	1998-09-29	1998-08-28	1998-10-19	NONE                     	RAIL      	thely notornis: 
5633	11	5	7	39	35529.39	0.02	0.08	N	O	1998-07-12	1998-07-03	1998-07-13	COLLECT COD              	TRUCK     	ding ideas cajole furiously after
5635	83	4	1	43	42272.44	0.03	0.00	R	F	1992-10-12	1992-09-29	1992-11-01	TAKE BACK RETURN         	TRUCK     	cross the d
5635	72	3	2	5	4860.35	0.05	0.08	R	F	1992-10-02	1992-11-05	1992-10-26	TAKE BACK RETURN         	REG AIR   	yly along the ironic, fi
5635	72	1	3	12	11664.84	0.09	0.02	A	F	1992-10-18	1992-09-24	1992-11-17	NONE                     	REG AIR   	ke slyly against the carefully final req
5635	8	5	4	40	36320.00	0.03	0.01	A	F	1992-09-25	1992-11-05	1992-10-11	NONE                     	FOB       	pending foxes. regular packages
5635	169	10	5	38	40628.08	0.05	0.06	A	F	1992-10-09	1992-09-25	1992-10-18	NONE                     	MAIL      	ckly pendin
5635	162	9	6	23	24429.68	0.05	0.04	A	F	1992-08-24	1992-11-10	1992-09-21	NONE                     	AIR       	ily pending packages. bold,
5635	137	3	7	32	33188.16	0.03	0.08	R	F	1992-11-24	1992-09-20	1992-12-17	TAKE BACK RETURN         	TRUCK     	slyly even
5638	138	9	1	45	46715.85	0.09	0.07	A	F	1994-05-17	1994-03-09	1994-06-15	NONE                     	TRUCK     	ar foxes. fluffily pending accounts 
5638	168	3	2	12	12817.92	0.02	0.05	A	F	1994-02-05	1994-04-01	1994-02-25	COLLECT COD              	TRUCK     	n, even requests. furiously ironic not
5638	162	9	3	21	22305.36	0.08	0.00	A	F	1994-03-13	1994-03-27	1994-03-17	DELIVER IN PERSON        	TRUCK     	press courts use f
5639	47	10	1	11	10417.44	0.09	0.02	R	F	1994-09-18	1994-07-10	1994-10-12	TAKE BACK RETURN         	SHIP      	g the unusual pinto beans caj
5665	101	2	1	32	32035.20	0.00	0.02	A	F	1993-08-11	1993-08-01	1993-09-07	NONE                     	AIR       	f the slyly even requests! regular request
5665	5	8	2	14	12670.00	0.02	0.00	R	F	1993-06-29	1993-09-16	1993-07-16	DELIVER IN PERSON        	AIR       	- special pinto beans sleep quickly blithel
5665	158	9	3	41	43384.15	0.09	0.02	A	F	1993-08-23	1993-09-22	1993-09-11	COLLECT COD              	REG AIR   	 idle ideas across 
5665	46	9	4	47	44463.88	0.01	0.01	A	F	1993-10-06	1993-09-19	1993-11-01	NONE                     	RAIL      	s mold fluffily. final deposits along the
5667	145	4	1	37	38670.18	0.09	0.06	N	O	1995-09-24	1995-09-17	1995-10-03	NONE                     	REG AIR   	s cajole blit
5668	4	9	1	15	13560.00	0.03	0.04	A	F	1995-04-06	1995-05-12	1995-04-17	COLLECT COD              	FOB       	 the express, pending requests. bo
5670	90	1	1	27	26732.43	0.10	0.06	R	F	1993-05-09	1993-05-30	1993-06-06	TAKE BACK RETURN         	REG AIR   	 ideas promise bli
5670	186	7	2	43	46705.74	0.06	0.00	A	F	1993-07-09	1993-06-03	1993-07-14	DELIVER IN PERSON        	FOB       	ests in place of the carefully sly depos
5670	7	8	3	24	21768.00	0.09	0.04	A	F	1993-07-17	1993-07-01	1993-08-03	NONE                     	AIR       	press, express requests haggle
5670	142	9	4	11	11463.54	0.06	0.06	R	F	1993-07-11	1993-06-26	1993-07-24	DELIVER IN PERSON        	MAIL      	etect furiously among the even pin
5671	120	7	1	25	25503.00	0.00	0.08	N	O	1998-04-17	1998-03-28	1998-05-06	DELIVER IN PERSON        	AIR       	cording to the quickly final requests-- 
5671	129	8	2	46	47339.52	0.05	0.08	N	O	1998-03-28	1998-04-22	1998-04-19	TAKE BACK RETURN         	MAIL      	lar pinto beans detect care
5671	172	10	3	13	13938.21	0.10	0.06	N	O	1998-03-02	1998-04-03	1998-03-08	TAKE BACK RETURN         	TRUCK     	bold theodolites about
5671	111	1	4	42	42466.62	0.00	0.07	N	O	1998-02-17	1998-04-24	1998-03-17	TAKE BACK RETURN         	SHIP      	carefully slyly special deposit
5671	129	4	5	13	13378.56	0.09	0.00	N	O	1998-04-24	1998-03-26	1998-04-27	NONE                     	REG AIR   	ers according to the ironic, unusual excu
5671	114	1	6	30	30423.30	0.09	0.07	N	O	1998-06-06	1998-04-15	1998-07-01	DELIVER IN PERSON        	TRUCK     	fily ironi
5698	11	8	1	30	27330.30	0.01	0.05	A	F	1994-05-26	1994-08-16	1994-06-19	COLLECT COD              	AIR       	its. quickly regular foxes aro
5698	163	4	2	25	26579.00	0.08	0.07	R	F	1994-08-06	1994-06-21	1994-08-25	NONE                     	SHIP      	 asymptotes sleep slyly above the
5698	155	3	3	45	47481.75	0.03	0.01	A	F	1994-06-23	1994-08-13	1994-07-02	NONE                     	FOB       	ng excuses. slyly express asymptotes
5698	58	6	4	15	14370.75	0.07	0.08	R	F	1994-06-29	1994-07-03	1994-07-02	COLLECT COD              	REG AIR   	ly ironic frets haggle carefully 
5698	140	1	5	37	38485.18	0.06	0.06	A	F	1994-06-30	1994-06-23	1994-07-22	TAKE BACK RETURN         	SHIP      	ts. even, ironic 
5698	188	9	6	1	1088.18	0.06	0.04	R	F	1994-05-31	1994-07-10	1994-06-03	DELIVER IN PERSON        	MAIL      	nts. slyly quiet pinto beans nag carefu
5700	168	5	1	24	25635.84	0.09	0.00	N	O	1997-12-26	1998-01-28	1998-01-18	DELIVER IN PERSON        	REG AIR   	ix carefully 
5700	123	8	2	30	30693.60	0.00	0.06	N	O	1998-04-19	1998-03-13	1998-04-27	COLLECT COD              	MAIL      	ly blithely final instructions. fl
5700	126	5	3	23	23600.76	0.03	0.05	N	O	1998-01-30	1998-01-31	1998-01-31	NONE                     	REG AIR   	 wake quickly carefully fluffy hockey
5701	54	2	1	17	16218.85	0.02	0.05	N	O	1997-03-27	1997-04-08	1997-04-21	DELIVER IN PERSON        	RAIL      	tes. quickly final a
5702	77	7	1	44	42991.08	0.06	0.02	R	F	1994-01-04	1993-11-25	1994-01-22	NONE                     	RAIL      	lites. carefully final requests doze b
5702	86	7	2	37	36484.96	0.10	0.05	R	F	1993-12-14	1993-10-21	1994-01-08	NONE                     	FOB       	ix slyly. regular instructions slee
5702	131	7	3	44	45369.72	0.00	0.02	R	F	1993-11-28	1993-12-02	1993-12-22	NONE                     	TRUCK     	ake according to th
5702	63	8	4	31	29854.86	0.00	0.04	A	F	1994-01-04	1993-10-22	1994-01-26	DELIVER IN PERSON        	TRUCK     	pinto beans. blithely 
5703	88	9	1	2	1976.16	0.09	0.01	R	F	1993-05-29	1993-07-26	1993-06-05	TAKE BACK RETURN         	REG AIR   	nts against the blithely sile
5729	143	4	1	5	5215.70	0.07	0.00	R	F	1994-11-27	1994-11-11	1994-12-23	TAKE BACK RETURN         	MAIL      	s. even sheaves nag courts. 
5729	107	10	2	39	39276.90	0.10	0.00	A	F	1995-01-22	1994-11-21	1995-02-13	TAKE BACK RETURN         	MAIL      	. special pl
5729	12	3	3	50	45600.50	0.00	0.05	R	F	1994-12-09	1994-12-31	1994-12-24	TAKE BACK RETURN         	AIR       	ly special sentiments. car
5730	151	2	1	2	2102.30	0.08	0.00	N	O	1998-02-24	1998-03-15	1998-03-11	COLLECT COD              	SHIP      	ely ironic foxes. carefu
5730	200	1	2	9	9901.80	0.10	0.01	N	O	1998-03-05	1998-02-02	1998-03-28	DELIVER IN PERSON        	MAIL      	s lose blithely. specia
5731	192	6	1	13	14198.47	0.02	0.04	N	O	1997-07-30	1997-06-23	1997-08-13	COLLECT COD              	RAIL      	ngside of the quickly regular depos
5731	105	6	2	11	11056.10	0.00	0.08	N	O	1997-06-06	1997-07-08	1997-06-25	NONE                     	MAIL      	 furiously final accounts wake. d
5731	111	2	3	6	6066.66	0.01	0.04	N	O	1997-07-02	1997-07-01	1997-07-08	COLLECT COD              	SHIP      	sits integrate slyly close platelets. quick
5731	14	1	4	6	5484.06	0.03	0.06	N	O	1997-09-07	1997-06-20	1997-09-20	TAKE BACK RETURN         	RAIL      	rs. quickly regular theo
5731	195	6	5	19	20808.61	0.08	0.02	N	O	1997-06-29	1997-06-27	1997-07-15	NONE                     	REG AIR   	ly unusual ideas above the 
5763	131	2	1	32	32996.16	0.02	0.06	N	O	1998-07-16	1998-09-13	1998-08-02	DELIVER IN PERSON        	FOB       	ding instruct
5763	136	2	2	23	23830.99	0.09	0.04	N	O	1998-07-25	1998-09-21	1998-08-15	DELIVER IN PERSON        	SHIP      	re after the blithel
5763	13	3	3	25	22825.25	0.01	0.02	N	O	1998-10-04	1998-08-16	1998-10-09	DELIVER IN PERSON        	REG AIR   	inal theodolites. even re
5763	121	6	4	47	47992.64	0.09	0.00	N	O	1998-08-22	1998-09-22	1998-09-04	NONE                     	REG AIR   	gle slyly. slyly final re
5763	123	4	5	8	8184.96	0.06	0.05	N	O	1998-09-23	1998-09-15	1998-09-27	DELIVER IN PERSON        	TRUCK     	foxes wake slyly. car
5763	190	1	6	9	9811.71	0.08	0.02	N	O	1998-09-24	1998-09-01	1998-10-02	NONE                     	AIR       	 deposits. instru
5767	167	8	1	11	11738.76	0.08	0.01	A	F	1992-06-02	1992-05-30	1992-06-08	NONE                     	TRUCK     	instructions. carefully final accou
5767	69	8	2	15	14535.90	0.07	0.05	R	F	1992-06-05	1992-07-28	1992-06-08	DELIVER IN PERSON        	MAIL      	warthogs. carefully unusual g
5767	191	3	3	42	45829.98	0.06	0.01	R	F	1992-07-31	1992-06-09	1992-08-09	COLLECT COD              	TRUCK     	 blithe deposi
5767	153	4	4	34	35807.10	0.06	0.01	R	F	1992-06-02	1992-06-23	1992-06-17	NONE                     	FOB       	sits among the
5767	46	7	5	36	34057.44	0.03	0.00	A	F	1992-07-17	1992-06-10	1992-07-19	COLLECT COD              	AIR       	ake carefully. packages 
5792	178	8	1	34	36657.78	0.08	0.07	R	F	1993-05-23	1993-06-25	1993-06-12	NONE                     	RAIL      	requests are against t
5792	157	5	2	47	49686.05	0.10	0.00	A	F	1993-06-08	1993-05-10	1993-06-26	COLLECT COD              	AIR       	regular, ironic excuses n
5792	183	4	3	32	34661.76	0.05	0.08	R	F	1993-06-26	1993-05-23	1993-07-07	COLLECT COD              	RAIL      	s are slyly against the ev
5792	14	8	4	14	12796.14	0.09	0.02	A	F	1993-07-28	1993-06-17	1993-08-27	DELIVER IN PERSON        	RAIL      	olites print carefully
5792	102	9	5	31	31065.10	0.02	0.01	A	F	1993-06-17	1993-05-05	1993-07-01	COLLECT COD              	TRUCK     	s? furiously even instructions 
5794	158	9	1	42	44442.30	0.06	0.05	R	F	1993-06-29	1993-05-30	1993-07-28	COLLECT COD              	REG AIR   	he careful
5794	115	2	2	14	14211.54	0.09	0.02	R	F	1993-04-19	1993-07-02	1993-05-18	COLLECT COD              	SHIP      	uriously carefully ironic reque
5794	7	8	3	15	13605.00	0.09	0.06	R	F	1993-06-25	1993-06-27	1993-07-09	NONE                     	MAIL      	blithely regular ideas. final foxes haggle 
5794	137	3	4	47	48745.11	0.00	0.08	A	F	1993-07-16	1993-06-21	1993-08-05	TAKE BACK RETURN         	REG AIR   	quests. blithely final excu
5796	58	3	1	27	25867.35	0.10	0.00	N	O	1996-04-06	1996-02-29	1996-04-20	DELIVER IN PERSON        	RAIL      	s wake quickly aro
5799	95	6	1	41	40798.69	0.04	0.02	N	O	1995-11-13	1995-10-31	1995-11-16	COLLECT COD              	TRUCK     	al accounts sleep ruthlessl
5799	100	3	2	30	30003.00	0.03	0.08	N	O	1995-09-12	1995-09-13	1995-09-19	NONE                     	RAIL      	 furiously s
5825	159	7	1	23	24360.45	0.10	0.05	R	F	1995-05-10	1995-04-28	1995-05-13	DELIVER IN PERSON        	TRUCK     	 special pinto beans. dependencies haggl
5827	187	8	1	30	32615.40	0.03	0.05	N	O	1998-11-11	1998-09-27	1998-11-30	DELIVER IN PERSON        	RAIL      	ounts may c
5827	103	6	2	23	23071.30	0.09	0.05	N	O	1998-11-16	1998-09-14	1998-11-17	COLLECT COD              	RAIL      	ans. furiously special instruct
5827	164	1	3	3	3192.48	0.03	0.06	N	O	1998-10-17	1998-09-29	1998-10-28	DELIVER IN PERSON        	MAIL      	uses eat along the furiously
5827	200	1	4	26	28605.20	0.06	0.00	N	O	1998-07-29	1998-09-24	1998-07-30	DELIVER IN PERSON        	SHIP      	arefully special packages wake thin
5827	112	9	5	38	38460.18	0.03	0.06	N	O	1998-10-18	1998-08-27	1998-10-23	TAKE BACK RETURN         	TRUCK     	ly ruthless accounts
5827	17	4	6	14	12838.14	0.05	0.01	N	O	1998-08-31	1998-09-06	1998-09-13	TAKE BACK RETURN         	RAIL      	rges. fluffily pending 
5828	2	9	1	28	25256.00	0.10	0.03	A	F	1994-05-15	1994-05-20	1994-06-08	DELIVER IN PERSON        	MAIL      	 special ideas haggle slyly ac
5828	158	3	2	37	39151.55	0.01	0.00	R	F	1994-06-07	1994-05-30	1994-06-17	NONE                     	RAIL      	e carefully spec
5829	40	1	1	4	3760.16	0.01	0.02	N	O	1997-03-01	1997-02-17	1997-03-22	NONE                     	TRUCK     	ithely; accounts cajole ideas. regular foxe
5829	107	10	2	40	40284.00	0.04	0.01	N	O	1997-04-21	1997-02-12	1997-05-04	COLLECT COD              	TRUCK     	 the carefully ironic accounts. a
5829	129	8	3	6	6174.72	0.05	0.06	N	O	1997-01-22	1997-03-12	1997-02-02	TAKE BACK RETURN         	AIR       	sts. slyly special fo
5829	90	1	4	42	41583.78	0.02	0.07	N	O	1997-03-26	1997-04-01	1997-03-30	COLLECT COD              	REG AIR   	pearls. slyly bold deposits solve final
5829	191	5	5	49	53468.31	0.05	0.01	N	O	1997-01-31	1997-03-13	1997-02-18	NONE                     	MAIL      	 ironic excuses use fluf
5829	18	5	6	17	15606.17	0.09	0.02	N	O	1997-04-10	1997-03-29	1997-04-22	COLLECT COD              	AIR       	after the furiously ironic ideas no
5829	78	9	7	27	26407.89	0.08	0.04	N	O	1997-02-25	1997-03-31	1997-03-03	DELIVER IN PERSON        	AIR       	ns about the excuses are c
5857	58	9	1	25	23951.25	0.03	0.02	N	O	1997-12-02	1997-12-17	1997-12-08	DELIVER IN PERSON        	REG AIR   	ding platelets. pending excu
5857	195	9	2	50	54759.50	0.06	0.07	N	O	1997-12-04	1997-12-16	1997-12-20	NONE                     	TRUCK     	y regular d
5857	68	3	3	1	968.06	0.03	0.01	N	O	1998-02-01	1997-12-09	1998-02-20	TAKE BACK RETURN         	SHIP      	instructions detect final reques
5857	118	2	4	12	12217.32	0.03	0.08	N	O	1998-01-24	1997-12-27	1998-02-10	TAKE BACK RETURN         	AIR       	counts. express, final
5857	192	4	5	14	15290.66	0.07	0.07	N	O	1997-12-10	1998-01-06	1998-01-04	TAKE BACK RETURN         	TRUCK     	ffily pendin
5857	93	5	6	49	48661.41	0.00	0.04	N	O	1998-01-23	1997-12-12	1998-01-28	DELIVER IN PERSON        	REG AIR   	egular pinto beans
5863	161	10	1	45	47752.20	0.07	0.06	A	F	1993-12-19	1994-01-25	1994-01-05	NONE                     	REG AIR   	 deposits are ab
5863	160	8	2	21	22263.36	0.09	0.03	R	F	1994-01-13	1994-01-09	1994-01-28	DELIVER IN PERSON        	FOB       	atelets nag blithely furi
5893	134	10	1	43	44467.59	0.05	0.02	R	F	1992-11-02	1992-09-27	1992-11-21	TAKE BACK RETURN         	RAIL      	s. regular courts above the carefully silen
5893	2	9	2	2	1804.00	0.10	0.04	R	F	1992-07-18	1992-09-10	1992-08-12	NONE                     	RAIL      	ckages wake sly
5921	99	3	1	44	43959.96	0.07	0.01	R	F	1994-07-14	1994-06-30	1994-07-15	NONE                     	TRUCK     	ain about the special
5921	146	9	2	25	26153.50	0.06	0.01	A	F	1994-05-19	1994-06-15	1994-06-17	COLLECT COD              	TRUCK     	nd the slyly regular deposits. quick
5921	68	5	3	17	16457.02	0.06	0.01	R	F	1994-05-20	1994-05-26	1994-05-23	NONE                     	FOB       	final asymptotes. even packages boost 
5921	28	7	4	26	24128.52	0.03	0.04	A	F	1994-05-03	1994-07-06	1994-05-06	NONE                     	AIR       	hy dependenc
5921	143	10	5	41	42768.74	0.04	0.02	R	F	1994-04-13	1994-05-31	1994-04-26	DELIVER IN PERSON        	AIR       	nusual, regular theodol
5921	115	6	6	5	5075.55	0.02	0.00	R	F	1994-06-01	1994-05-07	1994-06-10	COLLECT COD              	TRUCK     	eas cajole across the final, fi
5922	196	10	1	9	9865.71	0.07	0.00	N	O	1996-12-04	1997-01-20	1996-12-08	DELIVER IN PERSON        	RAIL      	haggle slyly even packages. packages
5922	157	2	2	37	39114.55	0.01	0.04	N	O	1996-12-19	1996-12-16	1997-01-15	COLLECT COD              	RAIL      	s wake slyly. requests cajole furiously asy
5922	90	1	3	35	34653.15	0.08	0.00	N	O	1996-12-12	1997-01-21	1997-01-01	DELIVER IN PERSON        	SHIP      	accounts. regu
5922	66	7	4	13	12558.78	0.08	0.07	N	O	1997-03-08	1996-12-26	1997-04-03	DELIVER IN PERSON        	FOB       	sly special accounts wake ironically.
5922	57	5	5	39	37324.95	0.04	0.07	N	O	1997-03-04	1997-01-17	1997-03-25	TAKE BACK RETURN         	SHIP      	e of the instructions. quick
5922	179	9	6	10	10791.70	0.04	0.01	N	O	1997-02-23	1996-12-26	1997-03-04	NONE                     	REG AIR   	sly regular deposits haggle quickly ins
5924	176	5	1	38	40894.46	0.06	0.05	N	O	1995-12-17	1995-12-11	1996-01-06	TAKE BACK RETURN         	AIR       	ions cajole carefully along the 
5924	53	1	2	49	46699.45	0.04	0.00	N	O	1995-10-25	1995-12-11	1995-11-08	NONE                     	MAIL      	inly final excuses. blithely regular requ
5924	17	8	3	24	22008.24	0.09	0.08	N	O	1996-01-12	1995-12-13	1996-01-25	COLLECT COD              	REG AIR   	 use carefully. special, e
5927	90	1	1	44	43563.96	0.04	0.05	N	O	1997-11-29	1997-11-21	1997-12-13	DELIVER IN PERSON        	TRUCK     	rding to the special, final decoy
5927	115	2	2	8	8120.88	0.04	0.05	N	O	1997-09-24	1997-11-15	1997-10-22	TAKE BACK RETURN         	SHIP      	ilent dependencies nod c
5927	167	6	3	32	34149.12	0.10	0.07	N	O	1997-12-26	1997-10-27	1997-12-31	COLLECT COD              	AIR       	telets. carefully bold accounts was
5953	129	10	1	36	37048.32	0.03	0.00	R	F	1992-05-28	1992-06-24	1992-05-29	DELIVER IN PERSON        	FOB       	 cajole furio
5953	13	7	2	34	31042.34	0.03	0.04	A	F	1992-05-04	1992-06-12	1992-06-02	NONE                     	RAIL      	hockey players use furiously against th
5953	162	9	3	5	5310.80	0.07	0.06	A	F	1992-04-10	1992-04-27	1992-04-14	NONE                     	SHIP      	s. blithely 
5953	169	8	4	23	24590.68	0.09	0.02	R	F	1992-06-05	1992-06-03	1992-06-29	TAKE BACK RETURN         	FOB       	he silent ideas. silent foxes po
5957	15	9	1	37	33855.37	0.07	0.00	A	F	1994-04-18	1994-02-19	1994-05-11	NONE                     	AIR       	 ideas use ruthlessly.
5957	59	4	2	46	44116.30	0.04	0.08	A	F	1994-01-23	1994-01-30	1994-02-07	NONE                     	SHIP      	platelets. furiously unusual requests 
5957	2	7	3	17	15334.00	0.01	0.01	A	F	1994-01-24	1994-02-16	1994-02-08	TAKE BACK RETURN         	SHIP      	. final, pending packages
5957	132	3	4	29	29931.77	0.01	0.03	R	F	1994-02-24	1994-03-04	1994-03-08	COLLECT COD              	REG AIR   	sits. final, even asymptotes cajole quickly
5957	88	9	5	40	39523.20	0.04	0.04	R	F	1994-01-07	1994-02-05	1994-01-26	DELIVER IN PERSON        	SHIP      	ironic asymptotes sleep blithely again
5957	6	1	6	41	37146.00	0.10	0.07	R	F	1994-03-25	1994-02-20	1994-03-31	DELIVER IN PERSON        	MAIL      	es across the regular requests maint
5957	159	1	7	32	33892.80	0.10	0.07	A	F	1994-03-05	1994-02-20	1994-03-09	NONE                     	TRUCK     	 boost carefully across the 
5984	70	5	1	13	12610.91	0.06	0.07	R	F	1994-10-16	1994-09-06	1994-11-11	NONE                     	MAIL      	lar platelets. f
5984	102	3	2	25	25052.50	0.05	0.08	R	F	1994-10-06	1994-07-21	1994-10-28	COLLECT COD              	RAIL      	gular accounts. even packages nag slyly
5984	1	4	3	8	7208.00	0.10	0.00	R	F	1994-09-17	1994-08-28	1994-09-25	COLLECT COD              	RAIL      	its. express,
5984	190	1	4	35	38156.65	0.00	0.01	A	F	1994-08-25	1994-08-05	1994-08-31	DELIVER IN PERSON        	SHIP      	le fluffily regula
5985	86	7	1	4	3944.32	0.02	0.02	A	F	1995-05-04	1995-04-01	1995-05-17	DELIVER IN PERSON        	MAIL      	ole along the quickly slow d
5988	172	1	1	41	43958.97	0.08	0.03	R	F	1994-01-20	1994-02-06	1994-02-10	COLLECT COD              	AIR       	the pending, express reque
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
2081	121	O	145654.97	1997-07-05	2-HIGH         	Clerk#000000136	0	ong the regular theo	1997
2305	43	F	122964.66	1993-01-26	2-HIGH         	Clerk#000000440	0	ove the furiously even acco	1993
2756	118	F	142323.38	1994-04-18	1-URGENT       	Clerk#000000537	0	arefully special warho	1994
2	79	O	40183.29	1996-12-01	1-URGENT       	Clerk#000000880	0	 foxes. pending accounts at the pending, silent asymptot	1996
3	124	F	160882.76	1993-10-14	5-LOW          	Clerk#000000955	0	sly final accounts boost. carefully regular ideas cajole carefully. depos	1993
4	137	O	31084.79	1995-10-11	5-LOW          	Clerk#000000124	0	sits. slyly regular warthogs cajole. regular, regular theodolites acro	1995
36	116	O	38988.98	1995-11-03	1-URGENT       	Clerk#000000358	0	 quick packages are blithely. slyly silent accounts wake qu	1995
37	88	F	113701.89	1992-06-03	3-MEDIUM       	Clerk#000000456	0	kly regular pinto beans. carefully unusual waters cajole never	1992
38	125	O	46366.56	1996-08-21	4-NOT SPECIFIED	Clerk#000000604	0	haggle blithely. furiously express ideas haggle blithely furiously regular re	1996
39	82	O	219707.84	1996-09-20	3-MEDIUM       	Clerk#000000659	0	ole express, ironic requests: ir	1996
64	34	F	20065.73	1994-07-16	3-MEDIUM       	Clerk#000000661	0	wake fluffily. sometimes ironic pinto beans about the dolphin	1994
67	58	O	116227.05	1996-12-19	4-NOT SPECIFIED	Clerk#000000547	0	symptotes haggle slyly around the furiously iron	1996
70	65	F	84651.80	1993-12-18	5-LOW          	Clerk#000000322	0	 carefully ironic request	1993
96	109	F	55090.67	1994-04-17	2-HIGH         	Clerk#000000395	0	oost furiously. pinto	1994
99	89	F	92326.79	1994-03-13	4-NOT SPECIFIED	Clerk#000000973	0	e carefully ironic packages. pending	1994
102	1	O	113954.89	1997-05-09	2-HIGH         	Clerk#000000596	0	 slyly according to the asymptotes. carefully final packages integrate furious	1997
103	31	O	95563.95	1996-06-20	4-NOT SPECIFIED	Clerk#000000090	0	ges. carefully unusual instructions haggle quickly regular f	1996
133	44	O	80437.72	1997-11-29	1-URGENT       	Clerk#000000738	0	usly final asymptotes 	1997
134	7	F	154260.84	1992-05-01	4-NOT SPECIFIED	Clerk#000000711	0	lar theodolites boos	1992
135	61	O	174569.88	1995-10-21	4-NOT SPECIFIED	Clerk#000000804	0	l platelets use according t	1995
160	83	O	86076.86	1996-12-19	4-NOT SPECIFIED	Clerk#000000342	0	thely special sauternes wake slyly of t	1996
163	88	O	125170.86	1997-09-05	3-MEDIUM       	Clerk#000000379	0	y final packages. final foxes since the quickly even	1997
164	1	F	202660.52	1992-10-21	5-LOW          	Clerk#000000209	0	cajole ironic courts. slyly final ideas are slyly. blithely final Tiresias sub	1992
166	109	O	93335.60	1995-09-12	2-HIGH         	Clerk#000000440	0	lets. ironic, bold asymptotes kindle	1995
167	121	F	52982.23	1993-01-04	4-NOT SPECIFIED	Clerk#000000731	0	s nag furiously bold excuses. fluffily iron	1993
192	83	O	133002.55	1997-11-25	5-LOW          	Clerk#000000483	0	y unusual platelets among the final instructions integrate rut	1997
196	65	F	33248.04	1993-03-17	2-HIGH         	Clerk#000000988	0	beans boost at the foxes. silent foxes	1993
197	34	P	100290.07	1995-04-07	2-HIGH         	Clerk#000000969	0	solve quickly about the even braids. carefully express deposits affix care	1995
198	112	O	125792.83	1998-01-02	4-NOT SPECIFIED	Clerk#000000331	0	its. carefully ironic requests sleep. furiously express fox	1998
199	53	O	80592.44	1996-03-07	2-HIGH         	Clerk#000000489	0	g theodolites. special packag	1996
225	34	P	165890.47	1995-05-25	1-URGENT       	Clerk#000000177	0	s. blithely ironic accounts wake quickly fluffily special acc	1995
229	112	F	142290.77	1993-12-29	1-URGENT       	Clerk#000000628	0	he fluffily even instructions. furiously i	1993
256	125	F	106315.25	1993-10-19	4-NOT SPECIFIED	Clerk#000000834	0	he fluffily final ideas might are final accounts. carefully f	1993
257	124	O	7102.74	1998-03-28	3-MEDIUM       	Clerk#000000680	0	ts against the sly warhorses cajole slyly accounts	1998
258	43	F	186669.10	1993-12-29	1-URGENT       	Clerk#000000167	0	dencies. blithely quick packages cajole. ruthlessly final accounts	1993
259	44	F	75661.70	1993-09-29	4-NOT SPECIFIED	Clerk#000000601	0	ages doubt blithely against the final foxes. carefully express deposits dazzle	1993
262	31	O	108443.84	1995-11-25	4-NOT SPECIFIED	Clerk#000000551	0	l packages. blithely final pinto beans use carefu	1995
263	118	F	79782.56	1994-05-17	2-HIGH         	Clerk#000000088	0	 pending instructions. blithely un	1994
288	8	O	163794.53	1997-02-21	1-URGENT       	Clerk#000000109	0	uriously final requests. even, final ideas det	1997
290	118	F	62814.89	1994-01-01	4-NOT SPECIFIED	Clerk#000000735	0	efully dogged deposits. furiou	1994
293	31	F	37248.78	1992-10-02	2-HIGH         	Clerk#000000629	0	re bold, ironic deposits. platelets c	1992
295	19	F	89345.99	1994-09-29	2-HIGH         	Clerk#000000155	0	 unusual pinto beans play. regular ideas haggle	1994
320	1	O	39835.54	1997-11-21	2-HIGH         	Clerk#000000573	0	ar foxes nag blithely	1997
321	124	F	62251.15	1993-03-21	3-MEDIUM       	Clerk#000000289	0	equests run. blithely final dependencies after the deposits wake caref	1993
327	145	P	24468.16	1995-04-17	5-LOW          	Clerk#000000992	0	ng the slyly final courts. slyly even escapades eat 	1995
352	107	F	16003.86	1994-03-08	2-HIGH         	Clerk#000000932	0	ke slyly bold pinto beans. blithely regular accounts against the spe	1994
353	2	F	179984.42	1993-12-31	5-LOW          	Clerk#000000449	0	 quiet ideas sleep. even instructions cajole slyly. silently spe	1993
357	61	O	98723.11	1996-10-09	2-HIGH         	Clerk#000000301	0	e blithely about the express, final accounts. quickl	1996
359	79	F	142891.22	1994-12-19	3-MEDIUM       	Clerk#000000934	0	n dolphins. special courts above the carefully ironic requests use	1994
385	34	O	50724.06	1996-03-22	5-LOW          	Clerk#000000600	0	hless accounts unwind bold pain	1996
386	61	F	90380.40	1995-01-25	2-HIGH         	Clerk#000000648	0	 haggle quickly. stealthily bold asymptotes haggle among the furiously even re	1995
389	127	F	1984.14	1994-02-17	2-HIGH         	Clerk#000000062	0	ing to the regular asymptotes. final, pending foxes about the blithely sil	1994
391	112	F	13282.23	1994-11-17	2-HIGH         	Clerk#000000256	0	orges thrash fluffil	1994
418	95	P	33124.96	1995-04-13	4-NOT SPECIFIED	Clerk#000000643	0	. furiously ironic instruc	1995
419	118	O	111597.96	1996-10-01	3-MEDIUM       	Clerk#000000376	0	osits. blithely pending theodolites boost carefully	1996
448	149	O	114978.03	1995-08-21	3-MEDIUM       	Clerk#000000597	0	 regular, express foxes use blithely. quic	1995
449	97	O	41605.63	1995-07-20	2-HIGH         	Clerk#000000841	0	. furiously regular theodolites affix blithely 	1995
451	100	O	104664.40	1998-05-25	5-LOW          	Clerk#000000048	0	nic pinto beans. theodolites poach carefully; 	1998
452	61	O	2007.48	1997-10-14	1-URGENT       	Clerk#000000498	0	t, unusual instructions above the blithely bold pint	1997
481	31	F	117827.18	1992-10-08	2-HIGH         	Clerk#000000230	0	ly final ideas. packages haggle fluffily	1992
482	127	O	136634.34	1996-03-26	1-URGENT       	Clerk#000000295	0	ts. deposits wake: final acco	1996
483	35	O	39793.05	1995-07-11	2-HIGH         	Clerk#000000025	0	cross the carefully final e	1995
487	109	F	48502.79	1992-08-18	1-URGENT       	Clerk#000000086	0	ithely unusual courts eat accordi	1992
513	61	O	63703.92	1995-05-01	2-HIGH         	Clerk#000000522	0	regular packages. pinto beans cajole carefully against the even	1995
516	44	O	10677.86	1998-04-21	2-HIGH         	Clerk#000000305	0	lar, unusual platelets are carefully. even courts sleep bold, final pinto bea	1998
518	145	O	223537.09	1998-02-08	2-HIGH         	Clerk#000000768	0	 the carefully bold accounts. quickly regular excuses are	1998
546	145	O	14790.37	1996-11-01	2-HIGH         	Clerk#000000041	0	osits sleep. slyly special dolphins about the q	1996
547	100	O	96855.29	1996-06-22	3-MEDIUM       	Clerk#000000976	0	ing accounts eat. carefully regular packa	1996
548	124	F	99088.75	1994-09-21	1-URGENT       	Clerk#000000435	0	arefully express instru	1994
576	31	O	18307.45	1997-05-13	3-MEDIUM       	Clerk#000000955	0	l requests affix regular requests. final account	1997
580	61	O	88219.12	1997-07-05	2-HIGH         	Clerk#000000314	0	tegrate fluffily regular accou	1997
581	70	O	126066.00	1997-02-23	4-NOT SPECIFIED	Clerk#000000239	0	 requests. even requests use slyly. blithely ironic 	1997
608	26	O	62567.99	1996-02-28	3-MEDIUM       	Clerk#000000995	0	nic waters wake slyly slyly expre	1996
609	127	F	21088.59	1994-06-01	3-MEDIUM       	Clerk#000000348	0	- ironic gifts believe furiously ca	1994
612	82	F	145695.42	1992-10-21	3-MEDIUM       	Clerk#000000759	0	boost quickly quickly final excuses. final foxes use bravely afte	1992
640	97	F	145495.62	1993-01-23	2-HIGH         	Clerk#000000433	0	r, unusual accounts boost carefully final ideas. slyly silent theod	1993
641	133	F	120626.49	1993-08-30	5-LOW          	Clerk#000000175	0	ents cajole furiously about the quickly silent pac	1993
643	58	P	180396.95	1995-03-25	2-HIGH         	Clerk#000000354	0	g dependencies. regular accounts 	1995
644	8	F	201268.06	1992-05-01	1-URGENT       	Clerk#000000550	0	 blithely unusual platelets haggle ironic, special excuses. excuses unwi	1992
647	143	O	56449.23	1997-08-07	1-URGENT       	Clerk#000000270	0	egular pearls. carefully express asymptotes are. even account	1997
672	109	F	89877.09	1994-04-14	5-LOW          	Clerk#000000106	0	egular requests are furiously according to 	1994
674	34	F	27204.60	1992-08-29	5-LOW          	Clerk#000000448	0	ully special deposits. furiously final warhorses affix carefully. fluffily f	1992
677	124	F	147915.68	1993-11-24	3-MEDIUM       	Clerk#000000824	0	uriously special pinto beans cajole carefully. fi	1993
705	43	O	83773.49	1997-02-13	4-NOT SPECIFIED	Clerk#000000294	0	ithely regular dependencies. express, even packages sleep slyly pending t	1997
707	118	F	58218.35	1994-11-20	3-MEDIUM       	Clerk#000000199	0	 ideas about the silent, bold deposits nag dolphins	1994
708	32	O	100445.59	1998-07-03	3-MEDIUM       	Clerk#000000101	0	lphins cajole about t	1998
710	133	F	208974.42	1993-01-02	5-LOW          	Clerk#000000026	0	 regular, regular requests boost. fluffily re	1993
737	121	F	12984.85	1992-04-26	5-LOW          	Clerk#000000233	0	ake blithely express, ironic theodolites. blithely special accounts wa	1992
739	1	O	159171.69	1998-05-31	5-LOW          	Clerk#000000900	0	 against the slyly ironic packages nag slyly ironic	1998
740	44	O	83490.99	1995-07-16	3-MEDIUM       	Clerk#000000583	0	courts haggle furiously across the final, regul	1995
743	79	O	23614.89	1996-10-04	4-NOT SPECIFIED	Clerk#000000933	0	eans. furiously ironic deposits sleep carefully carefully qui	1996
770	32	O	64271.75	1998-05-23	5-LOW          	Clerk#000000572	0	heodolites. furiously special pinto beans cajole pac	1998
772	97	F	128234.96	1993-04-17	2-HIGH         	Clerk#000000430	0	s boost blithely fluffily idle ideas? fluffily even pin	1993
773	133	F	146862.27	1993-09-26	3-MEDIUM       	Clerk#000000307	0	tions are quickly accounts. accounts use bold, even pinto beans. gifts ag	1993
801	118	F	127717.72	1992-02-18	1-URGENT       	Clerk#000000186	0	iously from the furiously enticing reques	1992
802	137	F	156381.95	1995-01-05	1-URGENT       	Clerk#000000516	0	posits. ironic, pending requests cajole. even theodol	1995
805	127	O	90042.41	1995-07-05	4-NOT SPECIFIED	Clerk#000000856	0	y according to the fluffily 	1995
807	145	F	222392.53	1993-11-24	3-MEDIUM       	Clerk#000000012	0	refully special tithes. blithely regular accoun	1993
834	43	F	46459.92	1994-05-23	3-MEDIUM       	Clerk#000000805	0	 sleep. quickly even foxes are boldly. slyly express requests use slyly	1994
835	65	O	62430.67	1995-10-08	4-NOT SPECIFIED	Clerk#000000416	0	s about the carefully special foxes haggle quickly about the	1995
836	70	O	72843.48	1996-11-25	4-NOT SPECIFIED	Clerk#000000729	0	ely bold excuses sleep regular ideas. furiously unusual ideas wake furiou	1996
837	116	F	60918.41	1994-06-15	4-NOT SPECIFIED	Clerk#000000563	0	kages sleep slyly above the ironic, final orbits	1994
867	26	F	7471.75	1993-11-16	3-MEDIUM       	Clerk#000000877	0	pades nag quickly final, 	1993
870	34	F	40492.37	1993-06-20	4-NOT SPECIFIED	Clerk#000000123	0	blithely ironic ideas nod. sly, r	1993
896	2	F	169847.63	1993-03-09	1-URGENT       	Clerk#000000187	0	inal packages eat blithely according to the warhorses. furiously quiet de	1993
899	109	O	125562.09	1998-04-08	5-LOW          	Clerk#000000575	0	rts engage carefully final theodolites.	1998
903	11	O	109351.87	1995-07-07	4-NOT SPECIFIED	Clerk#000000793	0	e slyly about the final pl	1995
929	83	F	109301.02	1992-10-02	2-HIGH         	Clerk#000000160	0	its. furiously even foxes affix carefully finally silent accounts. express req	1992
933	97	F	71349.30	1992-08-05	4-NOT SPECIFIED	Clerk#000000752	0	ial courts wake permanently against the furiously regular ideas. unusual 	1992
960	35	F	63537.13	1994-09-21	3-MEDIUM       	Clerk#000000120	0	regular accounts. requests	1994
963	26	F	53287.25	1994-05-26	3-MEDIUM       	Clerk#000000497	0	uses haggle carefully. slyly even dependencies after the packages ha	1994
965	70	P	41758.44	1995-05-15	5-LOW          	Clerk#000000218	0	iously special packages. slyly pending requests are carefully 	1995
994	2	F	41433.48	1994-04-20	5-LOW          	Clerk#000000497	0	ole. slyly bold excuses nag caref	1994
995	116	P	135157.92	1995-05-31	3-MEDIUM       	Clerk#000000439	0	deas. blithely final deposits play. express accounts wake blithely caref	1995
997	109	O	27561.82	1997-05-19	2-HIGH         	Clerk#000000651	0	ly express depths. furiously final requests haggle furiously. carefu	1997
998	32	F	65269.38	1994-11-26	4-NOT SPECIFIED	Clerk#000000956	0	ronic dolphins. ironic, bold ideas haggle furiously furious	1994
999	61	F	145249.13	1993-09-05	5-LOW          	Clerk#000000464	0	pitaphs sleep. regular accounts use. f	1993
1028	70	F	153864.67	1994-01-01	2-HIGH         	Clerk#000000131	0	ts are. final, silent deposits are among the fl	1994
1058	53	F	89359.11	1993-04-26	3-MEDIUM       	Clerk#000000373	0	kly pending courts haggle. blithely regular sheaves integrate carefully fi	1993
1059	127	F	198360.22	1994-02-27	1-URGENT       	Clerk#000000104	0	en accounts. carefully bold packages cajole daringly special depende	1994
1090	19	O	32929.30	1997-11-15	2-HIGH         	Clerk#000000300	0	 furiously regular platelets haggle along the slyly unusual foxes! 	1997
1091	83	O	35795.22	1996-08-27	1-URGENT       	Clerk#000000549	0	 even pinto beans haggle quickly alongside of the eve	1996
1092	124	P	85552.21	1995-03-04	3-MEDIUM       	Clerk#000000006	0	re quickly along the blithe	1995
1094	145	O	9006.25	1997-12-24	3-MEDIUM       	Clerk#000000570	0	beans affix furiously about the pending, even deposits. finally pendi	1997
1095	145	O	178491.24	1995-08-22	3-MEDIUM       	Clerk#000000709	0	sly bold requests cajole carefully according to	1995
1122	121	O	179747.47	1997-01-10	1-URGENT       	Clerk#000000083	0	uffily carefully final theodolites. furiously express packages affix	1997
1126	145	O	59982.31	1998-01-28	4-NOT SPECIFIED	Clerk#000000928	0	d slyly regular ideas: special ideas believe slyly. slyly ironic sheaves w	1998
1127	58	O	103320.91	1995-09-19	4-NOT SPECIFIED	Clerk#000000397	0	usly silent, regular pinto beans. blithely express requests boos	1995
1153	121	O	220727.97	1996-04-18	5-LOW          	Clerk#000000059	0	 across the pending deposi	1996
1155	149	O	126902.81	1997-10-06	2-HIGH         	Clerk#000000164	0	c deposits haggle among the ironic, even requests. carefully ironic sheaves n	1997
1156	133	O	217682.81	1996-10-19	1-URGENT       	Clerk#000000200	0	 blithely ironic dolphins. furiously pendi	1996
1157	97	O	85394.06	1998-01-14	4-NOT SPECIFIED	Clerk#000000207	0	out the regular excuses boost carefully against the furio	1998
1159	70	F	55553.68	1992-09-18	3-MEDIUM       	Clerk#000000992	0	ts may sleep. requests according to the	1992
1184	89	O	39700.29	1997-10-26	5-LOW          	Clerk#000000777	0	iously even packages haggle fluffily care	1997
1188	20	O	54655.07	1996-04-11	2-HIGH         	Clerk#000000256	0	ully ironic deposits. slyl	1996
1191	112	O	28623.04	1995-11-07	3-MEDIUM       	Clerk#000000011	0	uests nag furiously. carefully even requests	1995
1217	7	F	40982.08	1992-04-26	4-NOT SPECIFIED	Clerk#000000538	0	 foxes nag quickly. ironic excuses nod. blithely pending	1992
1249	149	F	45889.09	1994-01-05	1-URGENT       	Clerk#000000095	0	al ideas sleep above the pending pin	1994
1252	149	O	93403.05	1997-08-04	5-LOW          	Clerk#000000348	0	ng the slyly regular excuses. special courts nag furiously blithely e	1997
1254	70	O	94649.25	1995-12-22	1-URGENT       	Clerk#000000607	0	 pinto beans. carefully regular request	1995
1280	97	F	91664.85	1993-01-11	5-LOW          	Clerk#000000160	0	posits thrash quickly after the theodolites. furiously iro	1993
1282	116	F	61297.42	1992-02-29	4-NOT SPECIFIED	Clerk#000000168	0	he quickly special packages. furiously final re	1992
1283	118	O	202623.92	1996-08-30	4-NOT SPECIFIED	Clerk#000000260	0	 pinto beans boost slyly ac	1996
1285	11	F	139124.72	1992-06-01	1-URGENT       	Clerk#000000423	0	cial deposits cajole after the ironic requests. p	1992
1286	109	F	207291.83	1993-05-14	4-NOT SPECIFIED	Clerk#000000939	0	 deposits use carefully from the excuses. slyly bold p	1993
1287	19	F	131432.42	1994-07-05	2-HIGH         	Clerk#000000288	0	ly ironic dolphins integrate furiously among the final packages. st	1994
1312	112	F	58111.00	1994-05-19	3-MEDIUM       	Clerk#000000538	0	n, express accounts across the ironic	1994
1539	112	F	39612.63	1995-03-10	5-LOW          	Clerk#000000840	0	nstructions boost pa	1995
1314	143	F	56207.66	1994-05-13	3-MEDIUM       	Clerk#000000485	0	ickly blithe packages nod ideas. furiously bold braids boost around the car	1994
1317	100	P	139714.71	1995-05-19	2-HIGH         	Clerk#000000373	0	sts. furiously special deposits lose fur	1995
1319	32	O	31103.83	1996-09-27	2-HIGH         	Clerk#000000257	0	y across the ruthlessly ironic accounts. unusu	1996
1345	95	F	111207.93	1992-10-28	5-LOW          	Clerk#000000447	0	 regular tithes. quickly fluffy de	1992
1348	19	O	94135.77	1998-04-18	5-LOW          	Clerk#000000206	0	tly. quickly even deposi	1998
1377	20	O	108334.30	1998-04-24	4-NOT SPECIFIED	Clerk#000000625	0	lly across the blithely express accounts. ironic excuses promise carefully de	1998
1378	20	O	118495.12	1996-03-09	4-NOT SPECIFIED	Clerk#000000705	0	 furiously even tithes cajole slyly among the quick	1996
1379	65	O	84627.76	1998-05-25	5-LOW          	Clerk#000000861	0	y deposits are caref	1998
1380	137	O	94969.41	1996-07-07	3-MEDIUM       	Clerk#000000969	0	inal deposits wake slyly daringly even requests. bold, even foxe	1996
1381	127	O	58212.22	1998-05-25	3-MEDIUM       	Clerk#000000107	0	even requests breach after the bold, ironic instructions. slyly even	1998
1382	133	F	173522.71	1993-08-17	5-LOW          	Clerk#000000241	0	fully final packages sl	1993
1383	121	F	34797.72	1993-04-27	2-HIGH         	Clerk#000000785	0	ts. express requests sleep blithel	1993
1409	143	F	72440.52	1992-12-31	4-NOT SPECIFIED	Clerk#000000065	0	ructions. furiously unusual excuses are regular, unusual theodolites. fin	1992
1411	95	F	164462.61	1994-12-21	2-HIGH         	Clerk#000000566	0	s. furiously special excuses across the pending pinto beans haggle sp	1994
1412	53	F	78676.54	1993-03-13	4-NOT SPECIFIED	Clerk#000000083	0	uffily daring theodolit	1993
1414	77	O	38057.81	1995-08-16	1-URGENT       	Clerk#000000883	0	ccounts. ironic foxes haggle car	1995
1415	79	F	24654.79	1994-05-29	4-NOT SPECIFIED	Clerk#000000601	0	rays. blithely final ideas affix quickl	1994
1442	112	F	7108.12	1994-07-05	4-NOT SPECIFIED	Clerk#000000935	0	nal pinto beans. slyly ironic ideas cajol	1994
1443	44	O	44672.03	1996-12-16	5-LOW          	Clerk#000000185	0	x blithely against the carefully final somas. even asymptotes are. quickly spe	1996
1472	149	O	65331.05	1996-10-06	5-LOW          	Clerk#000000303	0	y special dolphins around the final dependencies wake quick	1996
1474	70	F	51697.18	1995-01-09	1-URGENT       	Clerk#000000438	0	detect quickly above the carefully even 	1995
1476	145	O	18795.62	1996-06-27	2-HIGH         	Clerk#000000673	0	ding accounts hinder alongside of the quickly pending requests. fluf	1996
1504	2	F	89399.40	1992-08-28	3-MEDIUM       	Clerk#000000381	0	, brave deposits. bold de	1992
1507	121	F	96166.92	1993-10-14	3-MEDIUM       	Clerk#000000305	0	stealthy, ironic de	1993
1510	53	O	154590.05	1996-09-17	5-LOW          	Clerk#000000128	0	ld carefully. furiously final asymptotes haggle furiously	1996
1511	79	O	59651.38	1996-12-22	4-NOT SPECIFIED	Clerk#000000386	0	ts above the depend	1996
1537	109	F	108317.51	1992-02-15	4-NOT SPECIFIED	Clerk#000000862	0	g to the even deposits. ironic, final packages 	1992
1542	143	F	132972.24	1993-09-15	3-MEDIUM       	Clerk#000000435	0	t the furiously close deposits do was f	1993
1570	124	O	35589.57	1998-03-16	1-URGENT       	Clerk#000000745	0	pinto beans haggle furiousl	1998
1572	11	O	47232.79	1996-02-24	2-HIGH         	Clerk#000000994	0	fluffily ironic accounts haggle blithely final platelets! slyly regular foxes	1996
1575	145	O	197031.52	1995-09-13	3-MEDIUM       	Clerk#000000497	0	. furiously regular dep	1995
1601	53	F	73962.95	1994-08-27	5-LOW          	Clerk#000000469	0	ent deposits are ca	1994
1602	1	F	4225.26	1993-08-05	5-LOW          	Clerk#000000660	0	deposits. busily silent instructions haggle furiously. fin	1993
1603	2	F	29305.47	1993-07-31	4-NOT SPECIFIED	Clerk#000000869	0	s. slyly silent deposits boo	1993
1605	58	O	130687.64	1998-04-24	4-NOT SPECIFIED	Clerk#000000616	0	sleep furiously? ruthless, even pinto beans 	1998
1606	53	O	115877.40	1997-04-17	4-NOT SPECIFIED	Clerk#000000550	0	r requests. quickly even platelets breach before the ironically	1997
1607	149	O	166335.03	1995-12-16	2-HIGH         	Clerk#000000498	0	 bold, pending foxes haggle. slyly silent 	1995
1634	70	O	145898.47	1996-09-10	1-URGENT       	Clerk#000000360	0	arefully blithely ironic requests. slyly unusual instructions alongside	1996
1636	79	O	172021.87	1997-06-17	3-MEDIUM       	Clerk#000000457	0	ding requests. slyly ironic courts wake quickl	1997
1666	95	O	128367.97	1995-10-18	1-URGENT       	Clerk#000000849	0	ffily pending dependencies wake fluffily. pending, final accounts 	1995
1669	2	O	24362.39	1997-06-09	3-MEDIUM       	Clerk#000000663	0	er ironic requests detect furiously blithely sp	1997
1671	35	O	104391.11	1996-07-27	4-NOT SPECIFIED	Clerk#000000275	0	ly. slyly pending requests was above the 	1996
1700	65	O	89143.36	1996-06-15	3-MEDIUM       	Clerk#000000328	0	ely final dolphins wake sometimes above the quietly regular deposits. fur	1996
1729	133	F	12137.76	1992-05-19	2-HIGH         	Clerk#000000158	0	pending foxes wake. accounts	1992
1730	124	O	150886.49	1998-07-24	5-LOW          	Clerk#000000794	0	 fluffily pending deposits serve. furiously even requests wake furiou	1998
1734	7	F	44002.53	1994-06-11	2-HIGH         	Clerk#000000722	0	 final ideas haggle. blithely quick foxes sleep busily bold ideas. i	1994
1762	77	F	202227.17	1994-08-20	4-NOT SPECIFIED	Clerk#000000653	0	ly ironic packages. furi	1994
1763	121	O	140685.01	1996-10-29	2-HIGH         	Clerk#000000321	0	es. bold dependencies haggle furiously along 	1996
1793	19	F	82504.56	1992-07-12	4-NOT SPECIFIED	Clerk#000000291	0	regular packages cajole. blithely special packages according to the final d	1992
1797	125	O	51494.47	1996-05-07	3-MEDIUM       	Clerk#000000508	0	quiet platelets haggle since the quickly ironic instructi	1996
1799	61	F	46815.93	1994-03-07	4-NOT SPECIFIED	Clerk#000000339	0	ns sleep furiously final waters. blithely regular instructions h	1994
1826	82	F	124719.97	1992-04-16	4-NOT SPECIFIED	Clerk#000000718	0	the even asymptotes dazzle fluffily slyly regular asymptotes. final, unu	1992
1828	32	F	137369.50	1994-04-18	3-MEDIUM       	Clerk#000000840	0	y quickly bold packag	1994
1829	112	F	127532.20	1994-05-08	2-HIGH         	Clerk#000000537	0	 accounts wake above the furiously unusual requests. pending package	1994
1830	133	F	85122.24	1995-02-23	1-URGENT       	Clerk#000000045	0	according to the even,	1995
1857	133	F	102793.59	1993-01-13	2-HIGH         	Clerk#000000083	0	hely final ideas slee	1993
1858	143	O	30457.91	1997-12-13	1-URGENT       	Clerk#000000389	0	thely. slyly final deposits sleep	1997
1859	61	O	105094.09	1997-04-11	4-NOT SPECIFIED	Clerk#000000949	0	 the foxes. bravely special excuses nag carefully special r	1997
1861	70	F	95063.41	1994-01-03	3-MEDIUM       	Clerk#000000847	0	r the fluffily close sauternes. furio	1994
1862	34	O	97981.06	1998-02-24	5-LOW          	Clerk#000000348	0	ts snooze ironically abou	1998
1888	121	F	224724.11	1993-10-31	4-NOT SPECIFIED	Clerk#000000659	0	olites. pinto beans cajole. regular deposits affix. slyly regular	1993
1891	61	F	76848.96	1994-12-15	5-LOW          	Clerk#000000495	0	unusual foxes sleep regular deposits. requests wake special pac	1994
1893	125	O	116792.13	1997-10-30	2-HIGH         	Clerk#000000111	0	olites. silent, special deposits eat slyly quickly express packages; hockey p	1997
1895	7	F	44429.81	1994-05-30	3-MEDIUM       	Clerk#000000878	0	ress accounts. bold accounts cajole. slyly final pinto beans poach regul	1994
1921	88	F	57584.12	1994-01-18	3-MEDIUM       	Clerk#000000293	0	counts. slyly quiet requests along the ruthlessly regular accounts are 	1994
1953	149	F	57213.18	1993-11-30	3-MEDIUM       	Clerk#000000891	0	 fluffily along the quickly even packages. 	1993
1956	127	F	88704.26	1992-09-20	4-NOT SPECIFIED	Clerk#000000600	0	ironic ideas are silent ideas. furiously final deposits sleep slyly carefu	1992
1957	31	O	77482.87	1998-07-21	2-HIGH         	Clerk#000000639	0	nding excuses about the 	1998
1958	53	O	176294.34	1995-09-22	5-LOW          	Clerk#000000343	0	 haggle blithely. flu	1995
1959	43	O	62277.18	1997-01-13	4-NOT SPECIFIED	Clerk#000000631	0	 cajole about the blithely express requests. even excuses mold bl	1997
1985	7	F	171522.54	1994-09-02	4-NOT SPECIFIED	Clerk#000000741	0	slyly slyly even pains. slyly reg	1994
1986	149	F	34269.96	1994-05-05	2-HIGH         	Clerk#000000609	0	across the theodolites. quick	1994
1987	100	F	6406.29	1994-04-30	2-HIGH         	Clerk#000000652	0	gular platelets alongside 	1994
1988	109	O	117132.72	1995-10-06	4-NOT SPECIFIED	Clerk#000000011	0	ly ironic dolphins serve quickly busy accounts. bu	1995
1989	118	F	39263.28	1994-03-16	4-NOT SPECIFIED	Clerk#000000747	0	ely bold pinto beans ha	1994
1991	19	F	139854.41	1992-09-07	4-NOT SPECIFIED	Clerk#000000854	0	ing accounts can haggle at the carefully final Tiresias-- pending, regular	1992
2016	8	O	24347.36	1996-08-16	3-MEDIUM       	Clerk#000000641	0	the carefully ironic foxes. requests nag bold, r	1996
2018	19	P	25007.95	1995-04-05	4-NOT SPECIFIED	Clerk#000000920	0	gular accounts wake fur	1995
2021	70	O	27016.74	1995-07-15	1-URGENT       	Clerk#000000155	0	ong the furiously regular requests. unusual deposits wake fluffily inside	1995
2023	118	F	144123.37	1992-05-06	5-LOW          	Clerk#000000137	0	ular courts engage according to the	1992
2049	31	O	153048.74	1995-12-07	2-HIGH         	Clerk#000000859	0	ly regular requests thrash blithely about the fluffily even theodolites. r	1995
2055	97	F	57092.26	1993-09-04	1-URGENT       	Clerk#000000067	0	. warhorses affix slyly blithely express instructions? fur	1993
2080	95	F	45767.69	1993-06-18	5-LOW          	Clerk#000000190	0	ironic, pending theodolites are carefully about the quickly regular theodolite	1993
2113	32	O	65678.21	1997-11-08	2-HIGH         	Clerk#000000527	0	slyly regular instruct	1997
2114	79	F	106446.02	1995-01-16	5-LOW          	Clerk#000000751	0	r, unusual accounts haggle across the busy platelets. carefully 	1995
2146	118	F	179686.07	1992-09-14	4-NOT SPECIFIED	Clerk#000000476	0	ven packages. dependencies wake slyl	1992
2147	100	F	91513.79	1992-09-06	4-NOT SPECIFIED	Clerk#000000424	0	 haggle carefully furiously final foxes. pending escapades thrash. bold theod	1992
2150	82	F	166961.06	1994-06-03	3-MEDIUM       	Clerk#000000154	0	ect slyly against the even, final packages. quickly regular pinto beans wake c	1994
2151	58	O	124608.69	1996-11-11	3-MEDIUM       	Clerk#000000996	0	c requests. ironic platelets cajole across the quickly fluffy deposits.	1996
2178	8	O	79594.68	1996-12-12	3-MEDIUM       	Clerk#000000656	0	thely according to the instructions. furious	1996
2210	32	F	31689.46	1992-01-16	2-HIGH         	Clerk#000000941	0	 believe carefully quickly express pinto beans. deposi	1992
2212	118	F	17231.05	1994-03-23	3-MEDIUM       	Clerk#000000954	0	structions above the unusual requests use fur	1994
2242	82	O	15082.82	1997-07-20	4-NOT SPECIFIED	Clerk#000000360	0	 pending multipliers. carefully express asymptotes use quickl	1997
2244	127	F	21207.08	1993-01-09	1-URGENT       	Clerk#000001000	0	ckages. ironic, ironic accounts haggle blithely express excuses. 	1993
2245	58	F	150585.73	1993-04-28	3-MEDIUM       	Clerk#000000528	0	ake carefully. braids haggle slyly quickly b	1993
2247	95	F	13491.31	1992-08-02	4-NOT SPECIFIED	Clerk#000000947	0	furiously regular packages. final brai	1992
2275	149	F	37398.90	1992-10-22	4-NOT SPECIFIED	Clerk#000000206	0	 furiously furious platelets. slyly final packa	1992
2276	43	O	141159.63	1996-04-29	4-NOT SPECIFIED	Clerk#000000821	0	ecial requests. fox	1996
2277	89	F	79270.23	1995-01-02	4-NOT SPECIFIED	Clerk#000000385	0	accounts cajole. even i	1995
2309	100	O	146933.07	1995-09-04	5-LOW          	Clerk#000000803	0	he carefully pending packages. fluffily stealthy foxes engage carefully	1995
2310	31	O	82928.12	1996-09-20	5-LOW          	Clerk#000000917	0	wake carefully. unusual instructions nag ironic, regular excuse	1996
2339	109	F	63470.78	1993-12-15	5-LOW          	Clerk#000000847	0	 against the regular 	1993
2340	65	O	30778.78	1996-01-12	1-URGENT       	Clerk#000000964	0	ter the deposits sleep according to the slyly regular packages. carefully 	1996
2341	82	F	55950.21	1993-05-30	5-LOW          	Clerk#000000443	0	sts-- blithely bold dolphins through the deposits nag blithely carefully re	1993
2371	19	O	193857.67	1998-01-07	1-URGENT       	Clerk#000000028	0	ckages haggle at th	1998
2372	31	O	104927.66	1997-11-21	5-LOW          	Clerk#000000342	0	s: deposits haggle along the final ideas. careful	1997
2404	77	O	109077.69	1997-03-13	4-NOT SPECIFIED	Clerk#000000409	0	deposits breach furiously. ironic foxes haggle carefully bold packag	1997
2406	7	O	182516.77	1996-10-28	5-LOW          	Clerk#000000561	0	blithely regular accounts u	1996
2433	31	F	147071.86	1994-08-22	4-NOT SPECIFIED	Clerk#000000324	0	ess patterns are slyly. packages haggle carefu	1994
2436	125	O	73990.08	1995-09-11	4-NOT SPECIFIED	Clerk#000000549	0	arefully. blithely bold deposits affix special accounts. final foxes nag. spe	1995
2464	145	O	30495.65	1997-11-23	5-LOW          	Clerk#000000633	0	le about the instructions. courts wake carefully even	1997
2465	34	O	180737.75	1995-06-24	1-URGENT       	Clerk#000000078	0	al pinto beans. final, bold packages wake quickly	1995
2466	19	F	161625.50	1994-03-06	1-URGENT       	Clerk#000000424	0	c pinto beans. express deposits wake quickly. even, final courts nag. package	1994
2467	35	O	7231.91	1995-07-16	4-NOT SPECIFIED	Clerk#000000914	0	pades sleep furiously. sometimes regular packages again	1995
2468	112	O	160627.01	1997-06-09	4-NOT SPECIFIED	Clerk#000000260	0	ickly regular packages. slyly ruthless requests snooze quickly blithe	1997
2469	124	O	192074.23	1996-11-26	5-LOW          	Clerk#000000730	0	 sleep closely regular instructions. furiously ironic instructi	1996
2470	58	O	104966.33	1997-04-19	3-MEDIUM       	Clerk#000000452	0	to the furiously final packages? pa	1997
2471	89	O	34936.31	1998-03-12	4-NOT SPECIFIED	Clerk#000000860	0	carefully blithely regular pac	1998
2498	97	F	45514.27	1993-11-08	5-LOW          	Clerk#000000373	0	g the slyly special pinto beans. 	1993
2499	121	O	147243.86	1995-09-24	1-URGENT       	Clerk#000000277	0	r the quickly bold foxes. bold instructi	1995
2500	133	F	131122.82	1992-08-15	2-HIGH         	Clerk#000000447	0	integrate slyly pending deposits. furiously ironic accounts across the s	1992
2502	70	F	33470.40	1993-05-28	4-NOT SPECIFIED	Clerk#000000914	0	lyly: carefully pending ideas affix again	1993
2503	7	F	183671.08	1993-06-20	3-MEDIUM       	Clerk#000000294	0	ly even packages was. ironic, regular deposits unwind furiously across the p	1993
2531	44	O	143212.85	1996-05-06	4-NOT SPECIFIED	Clerk#000000095	0	even accounts. furiously ironic excuses sleep fluffily. carefully silen	1996
2535	121	F	67018.30	1993-05-25	5-LOW          	Clerk#000000296	0	phins cajole beneath the fluffily express asymptotes. c	1993
2561	58	O	137473.58	1997-11-14	1-URGENT       	Clerk#000000861	0	ual requests. unusual deposits cajole furiously pending, regular platelets. 	1997
2564	77	F	3967.47	1994-09-09	2-HIGH         	Clerk#000000718	0	usly regular pinto beans. orbits wake carefully. slyly e	1994
2567	70	O	263411.29	1998-02-27	2-HIGH         	Clerk#000000031	0	detect. furiously ironic requests	1998
3042	20	F	104523.03	1994-11-21	3-MEDIUM       	Clerk#000000573	0	 the slyly ironic depo	1994
2594	79	F	94866.39	1992-12-17	1-URGENT       	Clerk#000000550	0	ests. theodolites above the blithely even accounts detect furio	1992
2596	43	O	74940.13	1996-08-17	1-URGENT       	Clerk#000000242	0	requests. ironic, bold theodolites wak	1996
2598	112	O	84871.50	1996-03-05	3-MEDIUM       	Clerk#000000391	0	 ironic notornis according to the blithely final requests should 	1996
2599	149	O	62807.13	1996-11-07	2-HIGH         	Clerk#000000722	0	ts. slyly regular theodolites wake sil	1996
2627	149	F	26798.65	1992-03-24	3-MEDIUM       	Clerk#000000181	0	s. silent, ruthless requests	1992
2656	77	F	105492.37	1993-05-04	1-URGENT       	Clerk#000000307	0	elets. slyly final accou	1993
2659	83	F	79785.52	1993-12-18	4-NOT SPECIFIED	Clerk#000000758	0	cross the pending requests maintain 	1993
2660	127	O	16922.51	1995-08-05	5-LOW          	Clerk#000000480	0	ly finally regular deposits. ironic theodolites cajole	1995
2663	95	O	35131.80	1995-09-06	1-URGENT       	Clerk#000000950	0	ar requests. furiously final dolphins along the fluffily spe	1995
2691	7	F	30137.17	1992-04-30	5-LOW          	Clerk#000000439	0	es at the regular deposits sleep slyly by the fluffy requests. eve	1992
2693	19	O	66158.13	1996-09-04	1-URGENT       	Clerk#000000370	0	ndle never. blithely regular packages nag carefully enticing platelets. ca	1996
2694	121	O	102807.59	1996-03-14	5-LOW          	Clerk#000000722	0	 requests. bold deposits above the theodol	1996
2695	58	O	138584.20	1996-08-20	1-URGENT       	Clerk#000000697	0	ven deposits around the quickly regular packa	1996
2720	31	F	161307.05	1993-06-08	1-URGENT       	Clerk#000000948	0	quickly. special asymptotes are fluffily ironi	1993
2721	79	O	59180.25	1996-01-27	2-HIGH         	Clerk#000000401	0	 ideas eat even, unusual ideas. theodolites are carefully	1996
2722	35	F	50328.84	1994-04-09	5-LOW          	Clerk#000000638	0	rding to the carefully quick deposits. bli	1994
2723	61	O	104759.25	1995-10-06	5-LOW          	Clerk#000000836	0	nts must have to cajo	1995
2724	137	F	116069.66	1994-09-14	2-HIGH         	Clerk#000000217	0	 sleep blithely. blithely idle 	1994
2725	89	F	75144.68	1994-05-21	4-NOT SPECIFIED	Clerk#000000835	0	ular deposits. spec	1994
2726	7	F	47753.00	1992-11-27	5-LOW          	Clerk#000000470	0	 blithely even dinos sleep care	1992
2754	145	F	25985.52	1994-04-03	2-HIGH         	Clerk#000000960	0	cies detect slyly. 	1994
2755	118	F	101202.18	1992-02-07	4-NOT SPECIFIED	Clerk#000000177	0	ously according to the sly foxes. blithely regular pinto bean	1992
2758	43	O	36671.88	1998-07-12	5-LOW          	Clerk#000000863	0	s cajole according to the carefully special 	1998
2759	116	F	89731.10	1993-11-25	4-NOT SPECIFIED	Clerk#000000071	0	ts. regular, pending pinto beans sleep ab	1993
2784	95	O	106635.21	1998-01-07	1-URGENT       	Clerk#000000540	0	g deposits alongside of the silent requests s	1998
2786	79	F	178254.66	1992-03-22	2-HIGH         	Clerk#000000976	0	al platelets cajole blithely ironic requests. ironic re	1992
2788	124	F	17172.66	1994-09-22	1-URGENT       	Clerk#000000641	0	nts wake across the fluffily bold accoun	1994
2791	121	F	156697.55	1994-10-10	2-HIGH         	Clerk#000000662	0	as. slyly ironic accounts play furiously bl	1994
2816	58	F	42225.53	1994-09-20	2-HIGH         	Clerk#000000289	0	kages at the final deposits cajole furious foxes. quickly 	1994
2820	19	F	143813.39	1994-05-20	3-MEDIUM       	Clerk#000000807	0	equests are furiously. carefu	1994
2821	118	F	36592.48	1993-08-09	3-MEDIUM       	Clerk#000000323	0	ng requests. even instructions are quickly express, silent instructi	1993
2822	79	F	40142.15	1993-07-26	2-HIGH         	Clerk#000000510	0	furiously against the accounts. unusual accounts aft	1993
2823	79	O	171894.45	1995-09-09	2-HIGH         	Clerk#000000567	0	encies. carefully fluffy accounts m	1995
2848	70	F	116258.53	1992-03-10	1-URGENT       	Clerk#000000256	0	ly fluffy foxes sleep furiously across the slyly regu	1992
2850	100	O	122969.79	1996-10-02	2-HIGH         	Clerk#000000392	0	, regular deposits. furiously pending packages hinder carefully carefully u	1996
2851	145	O	7859.36	1997-09-07	5-LOW          	Clerk#000000566	0	Tiresias wake quickly quickly even	1997
2880	8	F	145761.99	1992-03-15	2-HIGH         	Clerk#000000756	0	ves maintain doggedly spec	1992
2881	100	F	45695.84	1992-05-10	5-LOW          	Clerk#000000864	0	uriously. slyly express requests according to the silent dol	1992
2882	121	O	172872.37	1995-08-22	2-HIGH         	Clerk#000000891	0	pending deposits. carefully eve	1995
2883	121	F	170360.27	1995-01-23	5-LOW          	Clerk#000000180	0	uses. carefully ironic accounts lose fluffil	1995
2885	7	F	146896.72	1992-09-19	4-NOT SPECIFIED	Clerk#000000280	0	ly sometimes special excuses. final requests are 	1992
2886	109	F	94527.23	1994-11-13	4-NOT SPECIFIED	Clerk#000000619	0	uctions. ironic packages sle	1994
2887	109	O	28571.39	1997-05-26	5-LOW          	Clerk#000000566	0	slyly even pinto beans. slyly bold epitaphs cajole blithely above t	1997
2913	43	O	130702.19	1997-07-12	3-MEDIUM       	Clerk#000000118	0	mptotes doubt furiously slyly regu	1997
2914	109	F	60867.14	1993-03-03	3-MEDIUM       	Clerk#000000543	0	he slyly regular theodolites are furiously sile	1993
2916	8	O	20182.22	1995-12-27	2-HIGH         	Clerk#000000681	0	ithely blithe deposits sleep beyond the	1995
2918	118	O	21760.09	1996-09-08	3-MEDIUM       	Clerk#000000439	0	ular deposits across th	1996
2919	53	F	137223.14	1993-12-10	2-HIGH         	Clerk#000000209	0	es. pearls wake quietly slyly ironic instructions--	1993
2946	125	O	102226.59	1996-02-05	5-LOW          	Clerk#000000329	0	g instructions about the regular accounts sleep carefully along the pen	1996
2947	70	P	43360.95	1995-04-26	1-URGENT       	Clerk#000000464	0	ronic accounts. accounts run furiously d	1995
2948	44	F	100758.71	1994-08-23	5-LOW          	Clerk#000000701	0	 deposits according to the blithely pending 	1994
2949	137	F	94231.71	1994-04-12	2-HIGH         	Clerk#000000184	0	y ironic accounts use. quickly blithe accou	1994
2978	44	P	139542.14	1995-05-03	1-URGENT       	Clerk#000000135	0	d. even platelets are. ironic dependencies cajole slow, e	1995
2979	133	O	116789.98	1996-03-23	3-MEDIUM       	Clerk#000000820	0	even, ironic foxes sleep along	1996
3010	8	O	141647.08	1996-01-26	2-HIGH         	Clerk#000000931	0	 blithely final requests. special deposits are slyl	1996
3012	32	F	91678.66	1993-05-05	1-URGENT       	Clerk#000000414	0	ts after the regular pinto beans impress blithely s	1993
3013	143	O	156407.40	1997-02-05	5-LOW          	Clerk#000000591	0	the furiously pendin	1997
3040	112	F	119201.64	1993-04-12	3-MEDIUM       	Clerk#000000544	0	carefully special packages. blithe	1993
3043	44	F	78221.69	1992-04-25	5-LOW          	Clerk#000000137	0	cajole blithely furiously fina	1992
3044	53	O	52433.54	1996-04-03	2-HIGH         	Clerk#000000008	0	cajole final courts. ironic deposits about the quickly final re	1996
3046	32	O	117817.52	1995-11-30	2-HIGH         	Clerk#000000522	0	r deposits. platelets use furi	1995
3075	127	F	37696.70	1994-05-07	3-MEDIUM       	Clerk#000000433	0	ackages: carefully unusual reques	1994
3077	121	O	99290.01	1997-08-06	2-HIGH         	Clerk#000000228	0	kly. fluffily ironic requests use qui	1997
3079	100	O	148299.05	1997-09-12	5-LOW          	Clerk#000000505	0	lly ironic accounts	1997
3104	70	F	102693.61	1993-09-16	3-MEDIUM       	Clerk#000000871	0	ges boost-- regular accounts are furiousl	1993
3105	137	O	125396.80	1996-11-13	4-NOT SPECIFIED	Clerk#000000772	0	s. blithely final ins	1996
3106	145	O	132494.97	1997-01-12	3-MEDIUM       	Clerk#000000729	0	its use slyly final theodolites; regular dolphins hang above t	1997
3107	26	O	107406.26	1997-08-21	1-URGENT       	Clerk#000000669	0	ously even deposits acr	1997
3109	124	F	216104.85	1993-07-24	5-LOW          	Clerk#000000936	0	bold requests sleep quickly according to the slyly final	1993
3110	88	F	115161.29	1994-12-17	2-HIGH         	Clerk#000000564	0	round the fluffy instructions. carefully silent packages cajol	1994
3111	133	O	154383.37	1995-08-25	5-LOW          	Clerk#000000922	0	slyly regular theodolites. furious deposits cajole deposits. ironic theodoli	1995
3140	145	F	54356.10	1992-04-09	1-URGENT       	Clerk#000000670	0	carefully ironic deposits use furiously. blith	1992
3141	26	O	115959.96	1995-11-10	1-URGENT       	Clerk#000000475	0	es. furiously bold instructions after the carefully final p	1995
3142	8	F	16030.15	1992-06-28	3-MEDIUM       	Clerk#000000043	0	usual accounts about the carefully special requests sleep slyly quickly regul	1992
3143	107	F	135647.68	1993-02-17	1-URGENT       	Clerk#000000519	0	 are final, ironic accounts. ironic 	1993
3169	19	F	126804.90	1993-12-21	3-MEDIUM       	Clerk#000000252	0	 even pinto beans are blithely special, special multip	1993
3172	89	F	121360.83	1992-06-03	4-NOT SPECIFIED	Clerk#000000771	0	es. slyly ironic packages x-ra	1992
3174	127	O	92856.91	1995-11-15	5-LOW          	Clerk#000000663	0	rts. silent, regular pinto beans are blithely regular packages. furiousl	1995
3175	44	F	205282.63	1994-07-15	5-LOW          	Clerk#000000629	0	 across the slyly even realms use carefully ironic deposits: sl	1994
3201	97	F	90755.31	1993-07-02	4-NOT SPECIFIED	Clerk#000000738	0	. busy, express instruction	1993
3202	88	F	50601.01	1992-12-24	5-LOW          	Clerk#000000067	0	fluffily express requests affix carefully around th	1992
3203	127	O	49357.72	1997-11-05	2-HIGH         	Clerk#000000493	0	e furiously silent warhorses. slyly silent deposits wake bli	1997
3232	82	F	55619.01	1992-10-09	1-URGENT       	Clerk#000000314	0	yly final accounts. packages agains	1992
3237	19	F	10508.12	1992-06-03	1-URGENT       	Clerk#000000606	0	inal requests. slyly even foxes detect about the furiously exp	1992
3238	61	F	41375.69	1993-02-21	5-LOW          	Clerk#000000818	0	lly express deposits are. furiously unusual ideas wake carefully somas. instr	1993
3239	35	O	156802.80	1998-01-12	4-NOT SPECIFIED	Clerk#000000619	0	 cajole carefully along the furiously pending deposits. 	1998
3265	53	F	43315.15	1992-06-27	1-URGENT       	Clerk#000000265	0	re quickly quickly pe	1992
3267	112	O	33998.90	1997-01-07	5-LOW          	Clerk#000000484	0	 the packages. regular decoys about the bold dependencies grow fi	1997
3271	34	F	86534.05	1992-01-01	1-URGENT       	Clerk#000000421	0	s. furiously regular requests	1992
3298	116	O	62716.67	1996-04-17	5-LOW          	Clerk#000000241	0	even accounts boost 	1996
3300	118	O	27049.22	1995-07-15	5-LOW          	Clerk#000000198	0	ses. carefully unusual instructions must have to detect about the blithel	1995
3301	133	F	48497.09	1994-09-04	4-NOT SPECIFIED	Clerk#000000325	0	ular gifts impress enticingly carefully express deposits; instructions boo	1994
3302	34	O	38330.42	1995-11-14	2-HIGH         	Clerk#000000367	0	eep blithely ironic requests. quickly even courts haggle slyly	1995
3303	145	O	97758.28	1997-12-14	4-NOT SPECIFIED	Clerk#000000661	0	nto beans sleep furiously above the carefully ironic 	1997
3328	7	F	139580.85	1992-11-19	5-LOW          	Clerk#000000384	0	ake among the express accounts? carefully ironic packages cajole never.	1992
3330	7	F	43255.19	1994-12-19	1-URGENT       	Clerk#000000124	0	kages use. carefully regular deposits cajole carefully about 	1994
3332	143	F	73739.06	1994-11-05	1-URGENT       	Clerk#000000840	0	ans detect carefully furiously final deposits: regular accoun	1994
3810	100	F	124675.27	1992-09-17	1-URGENT       	Clerk#000000660	0	ters sleep across the carefully final 	1992
3365	82	F	174634.12	1994-11-09	2-HIGH         	Clerk#000000126	0	he slyly regular foxes nag about the accounts. fluffily 	1994
3394	149	O	162165.94	1996-05-05	4-NOT SPECIFIED	Clerk#000000105	0	 blithely among the attainments. carefully final accounts nag blit	1996
3395	149	F	141486.77	1994-10-30	4-NOT SPECIFIED	Clerk#000000682	0	ideas haggle beside the ev	1994
3396	149	F	196443.16	1994-05-21	3-MEDIUM       	Clerk#000000868	0	uffily regular platelet	1994
3426	53	O	91929.93	1996-10-16	3-MEDIUM       	Clerk#000000283	0	alongside of the slyly	1996
3458	95	F	153069.14	1994-12-22	2-HIGH         	Clerk#000000392	0	rges snooze. slyly unusua	1994
3460	82	O	245976.74	1995-10-03	2-HIGH         	Clerk#000000078	0	ans integrate carefu	1995
3461	100	F	190960.69	1993-01-31	1-URGENT       	Clerk#000000504	0	al, bold deposits cajole fluffily fluffily final foxes. pending ideas beli	1993
3462	133	O	63590.17	1997-05-17	3-MEDIUM       	Clerk#000000657	0	uriously express asympto	1997
3463	89	F	85255.56	1993-08-18	1-URGENT       	Clerk#000000545	0	ding to the carefully ironic deposits	1993
3489	109	F	62453.97	1993-07-29	3-MEDIUM       	Clerk#000000307	0	s detect. carefully even platelets across the fur	1993
3491	83	O	50287.06	1998-06-24	1-URGENT       	Clerk#000000560	0	nic orbits believe carefully across the 	1998
3493	82	F	41686.10	1993-08-24	2-HIGH         	Clerk#000000887	0	lyly special accounts use blithely across the furiously sil	1993
3495	31	O	58666.79	1996-02-26	2-HIGH         	Clerk#000000441	0	nticing excuses are carefully	1996
3520	125	O	151233.65	1997-08-04	1-URGENT       	Clerk#000000023	0	hely. ideas nag; even, even fo	1997
3521	7	F	142029.67	1992-10-26	5-LOW          	Clerk#000000812	0	y even instructions cajole carefully above the bli	1992
3522	26	F	151515.08	1994-09-26	5-LOW          	Clerk#000000250	0	deposits-- slyly stealthy requests boost caref	1994
3523	149	O	129657.08	1998-04-07	2-HIGH         	Clerk#000000688	0	are on the carefully even depe	1998
3525	109	O	100749.60	1995-12-22	4-NOT SPECIFIED	Clerk#000000084	0	s nag among the blithely e	1995
3552	35	O	103656.44	1997-04-23	2-HIGH         	Clerk#000000973	0	 the ironic packages. furiously 	1997
3554	44	O	98335.61	1995-06-17	5-LOW          	Clerk#000000931	0	hely ironic requests haggl	1995
3557	121	F	85477.89	1992-11-09	2-HIGH         	Clerk#000000291	0	ithely courts. furi	1992
3586	121	F	112845.04	1993-12-05	2-HIGH         	Clerk#000000438	0	he quickly final courts. carefully regular requests nag unusua	1993
3587	79	O	174798.97	1996-05-10	4-NOT SPECIFIED	Clerk#000000443	0	ular patterns detect 	1996
3589	31	F	39103.37	1994-05-26	2-HIGH         	Clerk#000000023	0	ithe deposits nag furiously. furiously pending packages sleep f	1994
3590	149	P	218482.70	1995-05-13	5-LOW          	Clerk#000000986	0	lyly final deposits.	1995
3619	149	O	222274.54	1996-11-20	2-HIGH         	Clerk#000000211	0	uests mold after the blithely ironic excuses. slyly pending pa	1996
3620	44	O	59291.75	1997-03-07	5-LOW          	Clerk#000000124	0	le quickly against the epitaphs. requests sleep slyly according to the	1997
3648	125	F	180417.11	1993-06-17	5-LOW          	Clerk#000000717	0	foxes. unusual deposits boost quickly. slyly regular asymptotes across t	1993
3651	100	O	113191.45	1998-04-27	1-URGENT       	Clerk#000000222	0	ly unusual deposits thrash quickly after the ideas.	1998
3652	107	O	107732.23	1997-02-25	4-NOT SPECIFIED	Clerk#000000024	0	sly even requests after the 	1997
3654	7	F	222653.54	1992-06-03	5-LOW          	Clerk#000000475	0	s cajole slyly carefully special theodolites. even deposits haggl	1992
3680	127	F	124402.59	1992-12-10	4-NOT SPECIFIED	Clerk#000000793	0	ular platelets. carefully regular packages cajole blithely al	1992
3682	32	O	67525.43	1997-01-22	2-HIGH         	Clerk#000000001	0	es haggle carefully. decoys nag 	1997
3683	88	F	99960.46	1993-03-04	2-HIGH         	Clerk#000000248	0	ze across the express foxes. carefully special acco	1993
3687	43	F	99851.38	1993-02-03	1-URGENT       	Clerk#000000585	0	gular accounts. slyly regular instructions can are final ide	1993
3713	149	O	215342.63	1998-05-07	3-MEDIUM       	Clerk#000000325	0	s haggle quickly. ironic, regular Tiresi	1998
3715	65	O	64000.93	1996-03-18	1-URGENT       	Clerk#000000463	0	 always silent requests wake pinto beans. slyly pending foxes are aga	1996
3716	43	O	146221.66	1997-08-19	4-NOT SPECIFIED	Clerk#000000748	0	 pending ideas haggle. ironic,	1997
3718	31	O	63195.54	1996-10-23	2-HIGH         	Clerk#000000016	0	refully. furiously final packages use carefully slyly pending deposits! final,	1996
3719	118	O	139902.71	1997-02-16	2-HIGH         	Clerk#000000034	0	, enticing accounts are blithely among the daringly final asymptotes. furious	1997
3744	65	F	33085.68	1992-01-10	3-MEDIUM       	Clerk#000000765	0	osits sublate about the regular requests. fluffily unusual accou	1992
3745	112	F	19405.73	1993-09-29	5-LOW          	Clerk#000000181	0	ckages poach slyly against the foxes. slyly ironic instructi	1993
3747	149	O	204355.65	1996-08-20	1-URGENT       	Clerk#000000226	0	refully across the final theodolites. carefully bold accounts cajol	1996
3748	53	O	83804.38	1998-02-28	1-URGENT       	Clerk#000000156	0	slyly special packages	1998
3750	97	P	177181.67	1995-04-30	3-MEDIUM       	Clerk#000000885	0	y. express, even packages wake after the ide	1995
3782	65	O	145096.17	1996-08-24	1-URGENT       	Clerk#000000121	0	counts are. pending, regular asym	1996
3783	44	F	155017.92	1993-12-06	4-NOT SPECIFIED	Clerk#000000614	0	 along the pinto beans. special packages use. regular theo	1993
3808	79	F	228054.01	1994-04-24	1-URGENT       	Clerk#000000717	0	odolites. blithely ironic cour	1994
3814	118	P	149451.88	1995-02-22	5-LOW          	Clerk#000000669	0	 the furiously pending theodo	1995
3840	100	O	187156.38	1998-07-17	4-NOT SPECIFIED	Clerk#000000713	0	yly slow theodolites. enticingly 	1998
3841	58	F	129033.13	1994-10-05	4-NOT SPECIFIED	Clerk#000000018	0	 bold requests sleep quickly ironic packages. sometimes regular deposits nag 	1994
3844	79	F	6793.45	1994-12-29	1-URGENT       	Clerk#000000686	0	r dolphins. slyly ironic theodolites ag	1994
3845	89	F	134333.33	1992-04-26	1-URGENT       	Clerk#000000404	0	es among the pending, regular accounts sleep blithely blithely even de	1992
3847	34	F	7014.31	1993-03-12	5-LOW          	Clerk#000000338	0	uriously even deposits. furiously pe	1993
3875	118	O	74483.95	1997-09-10	1-URGENT       	Clerk#000000587	0	 solve among the fluffily even 	1997
3878	88	O	59989.66	1997-03-23	1-URGENT       	Clerk#000000314	0	e carefully regular platelets. special, express dependencies slee	1997
3904	149	O	39338.44	1997-11-15	4-NOT SPECIFIED	Clerk#000000883	0	sits haggle furiously across the requests. theodolites ha	1997
3908	43	F	57127.71	1993-03-09	3-MEDIUM       	Clerk#000000490	0	ounts cajole. regularly	1993
3936	32	O	168618.39	1996-11-07	2-HIGH         	Clerk#000000200	0	iously express packages engage slyly fina	1996
3938	31	F	46918.22	1993-03-03	1-URGENT       	Clerk#000000199	0	. unusual, final foxes haggle	1993
3939	70	O	8720.45	1996-01-11	5-LOW          	Clerk#000000647	0	ly ruthlessly silent requests. blithely regular requests haggle blithely wh	1996
3940	149	O	129012.84	1996-02-14	5-LOW          	Clerk#000000363	0	e above the ideas. quickly even dependencies along the blithely ir	1996
3972	124	F	1861.19	1994-04-21	3-MEDIUM       	Clerk#000000049	0	y regular requests haggle quickly. pending, express acco	1994
3975	118	O	37804.43	1995-04-11	3-MEDIUM       	Clerk#000000016	0	ts. regular, regular Tiresias play furiously. ironi	1995
4000	70	F	84053.93	1992-01-04	5-LOW          	Clerk#000000339	0	le carefully closely even pinto beans. regular, ironic foxes against the	1992
4003	112	F	17603.01	1993-01-27	1-URGENT       	Clerk#000000177	0	 blithe theodolites are slyly. slyly silent accounts toward	1993
4004	70	F	220715.14	1993-05-07	3-MEDIUM       	Clerk#000000273	0	accounts among the blithely regular sentiments 	1993
4006	35	F	70557.05	1995-01-04	3-MEDIUM       	Clerk#000000765	0	ly ironic packages integrate. regular requests alongside of 	1995
4007	8	F	116193.97	1993-06-18	2-HIGH         	Clerk#000000623	0	ecial packages. slyly regular accounts integrate 	1993
4033	83	F	57740.74	1993-06-02	5-LOW          	Clerk#000000181	0	ously bold instructions haggle furiously above the fluf	1993
4035	118	F	22840.21	1992-02-19	5-LOW          	Clerk#000000097	0	he ironic deposits sleep blith	1992
4037	121	F	36389.43	1993-03-24	2-HIGH         	Clerk#000000384	0	t carefully above the unusual the	1993
4066	32	O	176911.21	1997-01-27	4-NOT SPECIFIED	Clerk#000000286	0	yly ironic dinos. quickly regular accounts haggle. requests wa	1997
4068	125	O	71852.67	1996-09-18	3-MEDIUM       	Clerk#000000203	0	lly even accounts wake furiously across the unusual platelets. unusu	1996
4129	32	F	67226.28	1993-06-26	3-MEDIUM       	Clerk#000000541	0	nwind. quickly final theodolites use packages. accounts	1993
4131	44	O	145971.60	1998-01-30	1-URGENT       	Clerk#000000612	0	 above the foxes hang 	1998
4132	19	P	65601.08	1995-05-29	4-NOT SPECIFIED	Clerk#000000158	0	ld asymptotes solve alongside of the express, final packages. fluffily fi	1995
4134	97	F	125191.12	1995-01-12	1-URGENT       	Clerk#000000171	0	fully even deposits. regular de	1995
4161	118	F	198995.21	1993-08-21	5-LOW          	Clerk#000000047	0	nts. fluffily regular foxes above the quickly daring reques	1993
4166	43	F	100671.06	1993-02-28	5-LOW          	Clerk#000000757	0	quickly sly forges impress. careful foxes across the blithely even a	1993
4198	143	O	105789.01	1997-06-16	3-MEDIUM       	Clerk#000000583	0	g the special packages haggle pen	1997
4224	70	O	150655.44	1997-07-14	1-URGENT       	Clerk#000000034	0	jole quickly final dolphins. slyly pending foxes wake furiously bold pl	1997
4227	133	F	92261.08	1995-02-24	1-URGENT       	Clerk#000000063	0	ng the requests; ideas haggle fluffily. slyly unusual ideas c	1995
4256	118	F	23067.48	1992-04-05	4-NOT SPECIFIED	Clerk#000000043	0	y alongside of the fluffily iro	1992
4261	118	F	83665.20	1992-10-03	1-URGENT       	Clerk#000000662	0	 about the even, pending packages. slyly bold deposits boost	1992
4262	88	O	176278.57	1996-08-04	3-MEDIUM       	Clerk#000000239	0	 of the furious accounts. furiously regular accounts w	1996
4288	34	F	75030.81	1992-12-04	4-NOT SPECIFIED	Clerk#000000823	0	usly carefully even theodolites: slyly express pac	1992
4289	125	F	20752.62	1993-10-07	3-MEDIUM       	Clerk#000000912	0	e carefully close instructions. slyly special reques	1993
4291	89	F	71822.86	1993-11-29	3-MEDIUM       	Clerk#000000655	0	 sleep fluffily between the bold packages. bold	1993
4354	145	F	179827.12	1994-09-30	4-NOT SPECIFIED	Clerk#000000046	0	pending notornis. requests serve 	1994
4356	97	F	39828.51	1994-04-11	5-LOW          	Clerk#000000956	0	 asymptotes sleep blithely. asymptotes sleep. blithely regul	1994
4386	61	O	134413.58	1998-02-06	5-LOW          	Clerk#000000070	0	 dolphins. silent, idle pinto beans 	1998
4390	7	P	140608.69	1995-05-23	1-URGENT       	Clerk#000000691	0	inal pinto beans. exp	1995
4416	149	F	76067.10	1992-06-30	5-LOW          	Clerk#000000391	0	 deposits. ideas cajole express theodolites: 	1992
4418	61	F	47099.71	1993-03-25	3-MEDIUM       	Clerk#000000731	0	pecial pinto beans. close foxes affix iron	1993
4420	109	F	6088.41	1994-06-18	1-URGENT       	Clerk#000000706	0	lly bold deposits along the bold, pending foxes detect blithely after the acco	1994
4422	70	P	107140.22	1995-05-22	3-MEDIUM       	Clerk#000000938	0	ly bold accounts sleep special, regular foxes. doggedly regular in	1995
4448	70	O	127191.47	1998-05-21	2-HIGH         	Clerk#000000428	0	. deposits haggle around the silent packages; slyly unusual packages	1998
4453	65	O	137030.40	1997-04-01	3-MEDIUM       	Clerk#000000603	0	ages could have to nag slyly furiously even asymptotes! slowly regular 	1997
4455	19	F	102534.63	1993-10-11	3-MEDIUM       	Clerk#000000924	0	even requests. bravely regular foxes according to the carefully unusual 	1993
4482	82	P	63535.56	1995-05-15	4-NOT SPECIFIED	Clerk#000000534	0	ravely bold accounts. furiously ironic instructions affix quickly. pend	1995
4485	53	F	182432.17	1994-11-13	3-MEDIUM       	Clerk#000000038	0	es wake slyly even packages. blithely brave requests nag above the regul	1994
4512	70	O	148682.82	1995-10-25	5-LOW          	Clerk#000000393	0	ending instructions maintain fu	1995
4514	97	F	143899.85	1994-04-30	3-MEDIUM       	Clerk#000000074	0	deposits according to the carefull	1994
4518	125	O	25861.74	1997-05-01	3-MEDIUM       	Clerk#000000187	0	luffily against the spec	1997
4544	112	O	151148.81	1997-08-07	3-MEDIUM       	Clerk#000000435	0	g dependencies dazzle slyly ironic somas. carefu	1997
4546	43	O	39906.87	1995-07-29	5-LOW          	Clerk#000000373	0	ns sleep. regular, regular instructions maintai	1995
4547	109	F	52114.01	1993-08-23	3-MEDIUM       	Clerk#000000519	0	uctions thrash platelets. slyly final foxes wake slyly against th	1993
4548	127	O	139915.23	1996-06-28	5-LOW          	Clerk#000000798	0	 in place of the blithely express sentiments haggle slyly r	1996
4550	118	F	27461.48	1994-12-29	2-HIGH         	Clerk#000000748	0	s haggle carefully acco	1994
4551	109	O	82824.14	1996-02-09	2-HIGH         	Clerk#000000462	0	ts. slyly quick theodolite	1996
4577	79	O	104259.88	1998-05-02	5-LOW          	Clerk#000000409	0	ly. unusual platelets are alw	1998
4580	82	F	118464.65	1993-11-15	4-NOT SPECIFIED	Clerk#000000086	0	rs wake blithely regular requests. fluffily ev	1993
4581	79	F	89592.11	1992-09-04	4-NOT SPECIFIED	Clerk#000000687	0	ges. carefully pending accounts use furiously abo	1992
4582	19	O	18247.86	1996-07-04	1-URGENT       	Clerk#000000638	0	g the furiously regular pac	1996
4609	133	O	70462.84	1996-12-05	3-MEDIUM       	Clerk#000000239	0	hang slyly slyly expre	1996
4610	26	F	135934.60	1993-06-18	5-LOW          	Clerk#000000616	0	e carefully express pinto	1993
4612	61	F	82598.87	1993-09-20	3-MEDIUM       	Clerk#000000397	0	bove the deposits. even deposits dazzle. slyly express packages haggle sl	1993
4613	133	O	212339.55	1998-03-05	3-MEDIUM       	Clerk#000000541	0	furiously blithely pending dependen	1998
4614	61	O	151801.06	1996-04-22	1-URGENT       	Clerk#000000974	0	 sauternes wake thinly special accounts. fur	1996
4640	97	O	81138.17	1996-01-01	5-LOW          	Clerk#000000902	0	requests. deposits do detect above the blithely iron	1996
4645	44	F	231012.22	1994-09-20	1-URGENT       	Clerk#000000764	0	fully even instructions. final gifts sublate quickly final requests. bl	1994
4646	83	O	124637.19	1996-06-18	1-URGENT       	Clerk#000000036	0	n place of the blithely qu	1996
4672	79	O	199593.71	1995-11-07	1-URGENT       	Clerk#000000475	0	lyly final dependencies caj	1995
4673	82	O	58094.75	1996-08-13	4-NOT SPECIFIED	Clerk#000000914	0	c deposits are slyly. bravely ironic deposits cajole carefully after the 	1996
4678	88	O	131752.07	1998-08-02	4-NOT SPECIFIED	Clerk#000000175	0	side of the bold platelets detect slyly blithely ironic e	1998
4679	88	F	7211.59	1993-01-20	2-HIGH         	Clerk#000000905	0	ely regular accounts affix slyly. final dolphins are. furiously final de	1993
4704	2	O	63873.14	1996-08-16	4-NOT SPECIFIED	Clerk#000000256	0	lithely final requests about the fluffily regular 	1996
4709	26	O	49903.57	1996-01-08	3-MEDIUM       	Clerk#000000785	0	he furiously even deposits! ironic theodolites haggle blithely. r	1996
4710	100	F	88966.68	1994-12-08	4-NOT SPECIFIED	Clerk#000000734	0	the final, regular foxes. carefully ironic pattern	1994
4737	79	F	62014.51	1993-03-11	4-NOT SPECIFIED	Clerk#000000275	0	ents use slyly among the unusual, ironic pearls. furiously pending 	1993
4741	127	F	180692.90	1992-07-07	4-NOT SPECIFIED	Clerk#000000983	0	ly bold deposits are slyly about the r	1992
4743	97	F	65702.39	1993-03-31	5-LOW          	Clerk#000000048	0	pinto beans above the bold, even idea	1993
4769	121	P	136765.03	1995-04-14	4-NOT SPECIFIED	Clerk#000000116	0	pon the asymptotes. idle, final account	1995
4771	95	F	49625.21	1992-12-14	1-URGENT       	Clerk#000000571	0	lly express deposits serve furiously along the f	1992
4801	88	O	108353.08	1996-01-25	1-URGENT       	Clerk#000000553	0	r the final sentiments. pending theodolites sleep doggedly across t	1996
4803	124	O	158776.68	1996-02-08	5-LOW          	Clerk#000000892	0	lly unusual courts are ironic	1996
4806	7	F	35390.15	1993-04-21	5-LOW          	Clerk#000000625	0	ave accounts. furiously pending wa	1993
4807	53	O	138902.23	1997-01-09	3-MEDIUM       	Clerk#000000310	0	kly. slyly special accounts	1997
4832	34	O	84954.79	1997-12-04	3-MEDIUM       	Clerk#000000548	0	final accounts sleep among the blithe	1997
4833	133	O	84800.44	1996-05-12	3-MEDIUM       	Clerk#000000256	0	r deposits against the slyly final excuses slee	1996
4834	19	O	124539.00	1996-09-12	2-HIGH         	Clerk#000000284	0	lar accounts. furiously ironic accounts haggle slyly 	1996
4836	65	O	78711.40	1996-12-18	1-URGENT       	Clerk#000000691	0	c packages cajole carefully through the accounts. careful	1996
4838	44	F	61811.33	1992-08-02	1-URGENT       	Clerk#000000569	0	ffily bold sentiments. carefully close dolphins cajole across the 	1992
4864	88	F	149614.34	1992-11-11	5-LOW          	Clerk#000000423	0	ests nag within the quickly ironic asymptotes. ironic	1992
4866	53	O	25767.07	1997-08-07	2-HIGH         	Clerk#000000663	0	kages. unusual packages nag fluffily. qui	1997
4869	58	F	175422.13	1994-09-26	5-LOW          	Clerk#000000802	0	boost! ironic packages un	1994
4899	61	F	12291.83	1993-10-18	4-NOT SPECIFIED	Clerk#000000348	0	 instructions. furiously even packages are furiously speci	1993
4900	137	F	221320.76	1992-06-30	4-NOT SPECIFIED	Clerk#000000878	0	sleep quickly unusual 	1992
4901	79	O	146298.28	1997-12-31	4-NOT SPECIFIED	Clerk#000000980	0	inal dependencies cajole furiously. carefully express accounts na	1997
4929	149	O	135187.33	1996-02-29	3-MEDIUM       	Clerk#000000109	0	uests. furiously special ideas poach. pending 	1996
4930	149	F	176867.34	1994-05-06	5-LOW          	Clerk#000000593	0	 haggle slyly quietly final theodolites. packages are furious	1994
4960	124	F	153259.41	1995-02-26	5-LOW          	Clerk#000000229	0	uriously even excuses. fluffily regular instructions along the furiously ironi	1995
4961	58	O	89224.24	1998-04-06	3-MEDIUM       	Clerk#000000731	0	 braids. furiously even theodolites 	1998
4963	34	O	54175.35	1996-11-07	3-MEDIUM       	Clerk#000000754	0	ully unusual epitaphs nod s	1996
4966	70	O	59186.02	1996-09-07	2-HIGH         	Clerk#000000243	0	accounts. blithely ironic courts wake boldly furiously express 	1996
4994	43	O	216071.76	1996-06-29	4-NOT SPECIFIED	Clerk#000000868	0	oxes wake above the asymptotes. bold requests sleep br	1996
4996	133	F	100750.67	1992-09-14	3-MEDIUM       	Clerk#000000433	0	foxes. carefully special packages haggle quickly fluffi	1992
4998	32	F	129096.80	1992-01-11	4-NOT SPECIFIED	Clerk#000000054	0	alongside of the quickly final requests hang always	1992
5024	124	O	116127.69	1996-10-25	3-MEDIUM       	Clerk#000000659	0	r foxes. regular excuses are about the quickly regular theodolites. regular, 	1996
5025	121	O	20099.43	1997-02-03	5-LOW          	Clerk#000000805	0	ackages are slyly about the quickly 	1997
5029	11	F	19811.69	1992-11-14	3-MEDIUM       	Clerk#000000469	0	. regular accounts haggle slyly. regul	1992
5059	43	F	67173.82	1993-11-10	2-HIGH         	Clerk#000000058	0	latelets. final, regular accounts cajole furiously ironic pinto beans? do	1993
5060	112	F	65218.47	1992-07-07	4-NOT SPECIFIED	Clerk#000000333	0	e according to the excuses. express theodo	1992
5062	61	F	109247.00	1992-10-08	3-MEDIUM       	Clerk#000000012	0	ithely. blithely bold theodolites affix. blithely final deposits haggle ac	1992
5090	89	O	132838.49	1997-03-09	1-URGENT       	Clerk#000000953	0	ress accounts affix silently carefully quick accounts. carefully f	1997
5093	79	F	190693.92	1993-09-03	3-MEDIUM       	Clerk#000000802	0	ully ironic theodolites sleep above the furiously ruthless instructions. bli	1993
5095	97	F	184583.99	1992-04-22	2-HIGH         	Clerk#000000964	0	accounts are carefully! slyly even packages wake slyly a	1992
5121	133	F	150334.57	1992-05-11	4-NOT SPECIFIED	Clerk#000000736	0	gular requests. furiously final pearls against the permanent, thin courts s	1992
5122	70	O	79863.84	1996-02-10	5-LOW          	Clerk#000000780	0	blithely. slyly ironic deposits nag. excuses s	1996
5126	112	F	92123.32	1992-10-12	4-NOT SPECIFIED	Clerk#000000270	0	s. unusual deposits 	1992
5152	44	O	60568.34	1997-01-04	3-MEDIUM       	Clerk#000000963	0	 for the blithely reg	1997
5154	8	O	28070.86	1997-04-13	3-MEDIUM       	Clerk#000000316	0	inal requests. slyly regular deposits nag. even deposits haggle agains	1997
5155	77	F	70183.29	1994-06-12	2-HIGH         	Clerk#000000108	0	y pending deposits are ag	1994
5156	125	O	59439.44	1996-11-04	5-LOW          	Clerk#000000117	0	ngside of the multipliers solve slyly requests. regu	1996
5190	58	F	89684.31	1992-04-26	5-LOW          	Clerk#000000888	0	equests. slyly unusual	1992
5191	77	F	119910.04	1994-12-11	4-NOT SPECIFIED	Clerk#000000318	0	ing, regular deposits alongside of the deposits boost fluffily quickly ev	1994
5217	35	O	135745.58	1995-10-13	2-HIGH         	Clerk#000000873	0	ons might wake quickly according to th	1995
5218	82	F	73882.37	1992-07-30	4-NOT SPECIFIED	Clerk#000000683	0	y ruthless packages according to the bold, ironic package	1992
5219	88	O	21267.72	1997-02-27	1-URGENT       	Clerk#000000510	0	aggle always. foxes above the ironic deposits 	1997
5223	149	F	105561.21	1994-06-30	1-URGENT       	Clerk#000000745	0	e. theodolites serve blithely unusual, final foxes. carefully pending packag	1994
5248	70	P	86958.28	1995-04-15	2-HIGH         	Clerk#000000737	0	theodolites cajole according to the silent packages. quickly ironic packages a	1995
5250	97	O	29673.73	1995-07-16	2-HIGH         	Clerk#000000307	0	. carefully final instructions sleep among the finally regular dependen	1995
5251	34	O	34004.48	1995-04-12	3-MEDIUM       	Clerk#000000687	0	 ironic dugouts detect. reque	1995
5254	112	F	196989.09	1992-07-26	4-NOT SPECIFIED	Clerk#000000527	0	he express, even ideas cajole blithely special requests	1992
5280	34	O	68052.70	1997-12-03	3-MEDIUM       	Clerk#000000604	0	riously ironic instructions. ironic ideas according to the accounts boost fur	1997
5281	124	O	179418.31	1995-11-02	2-HIGH         	Clerk#000000158	0	ackages haggle slyly a	1995
5284	61	O	40548.99	1995-07-09	4-NOT SPECIFIED	Clerk#000000155	0	 careful dependencies use sly	1995
5285	70	F	99377.51	1994-01-18	2-HIGH         	Clerk#000000976	0	p across the furiously ironic deposits.	1994
5286	116	O	79646.89	1997-09-26	5-LOW          	Clerk#000000606	0	structions are furiously quickly ironic asymptotes. quickly iro	1997
5312	65	F	66697.95	1995-02-24	2-HIGH         	Clerk#000000690	0	ter the even, bold foxe	1995
5314	34	O	26999.83	1995-06-02	2-HIGH         	Clerk#000000617	0	ions across the quickly special d	1995
5316	100	F	62316.61	1994-01-31	1-URGENT       	Clerk#000000734	0	 requests haggle across the regular, pending deposits. furiously regular requ	1994
5344	109	O	88216.32	1998-06-21	3-MEDIUM       	Clerk#000000569	0	s. ironic excuses cajole across the	1998
5345	31	O	111924.56	1997-08-24	1-URGENT       	Clerk#000000057	0	r the slyly silent packages. pending, even pinto b	1997
5348	53	O	119164.96	1997-11-08	5-LOW          	Clerk#000000497	0	totes. accounts after the furiously	1997
5376	149	F	98422.83	1994-07-04	5-LOW          	Clerk#000000392	0	. quickly ironic deposits integrate along	1994
5378	43	F	101899.93	1992-10-25	1-URGENT       	Clerk#000000520	0	n ideas. regular accounts haggle. ironic ideas use along the bold ideas. blith	1992
5379	89	O	47010.15	1995-08-08	2-HIGH         	Clerk#000000503	0	he unusual accounts. carefully special instructi	1995
5381	32	F	223995.46	1993-01-29	5-LOW          	Clerk#000000531	0	arefully bold packages are slyly furiously ironic foxes. fluffil	1993
5382	35	F	138423.03	1992-01-13	5-LOW          	Clerk#000000809	0	lent deposits are according to the reg	1992
5383	31	O	11474.95	1995-05-26	5-LOW          	Clerk#000000409	0	ly bold requests hang furiously furiously unusual accounts. evenly unusu	1995
5411	61	O	62541.27	1997-05-16	3-MEDIUM       	Clerk#000000800	0	equests cajole slyly furious	1997
5414	100	F	167017.39	1993-03-25	4-NOT SPECIFIED	Clerk#000000242	0	lent dependencies? carefully express requests sleep furiously ac	1993
5442	43	O	139332.94	1998-01-13	4-NOT SPECIFIED	Clerk#000000954	0	ully. quickly express accounts against the	1998
5446	7	F	29920.80	1994-06-21	5-LOW          	Clerk#000000304	0	 furiously final pac	1994
5472	70	F	221636.83	1993-04-11	5-LOW          	Clerk#000000552	0	counts. deposits about the slyly dogged pinto beans cajole slyly	1993
5473	65	F	63041.33	1992-03-25	4-NOT SPECIFIED	Clerk#000000306	0	te the quickly stealthy ideas. even, regular deposits above	1992
5477	107	O	130125.64	1997-12-30	5-LOW          	Clerk#000000689	0	ckages. ironic deposits caj	1997
5478	116	O	97502.23	1996-05-17	1-URGENT       	Clerk#000000272	0	ckages. quickly pending deposits thrash furiously: bl	1996
5479	70	F	70553.45	1993-12-22	3-MEDIUM       	Clerk#000000335	0	ng asymptotes. pinto beans sleep care	1993
5504	19	F	41492.25	1993-01-06	2-HIGH         	Clerk#000000221	0	y pending packages. furiousl	1993
5505	95	O	147329.51	1997-10-04	5-LOW          	Clerk#000000719	0	 final, regular packages according to the slyly ironic accounts nag ironica	1997
5507	2	O	140363.70	1998-05-28	5-LOW          	Clerk#000000692	0	the carefully ironic instructions are quickly iro	1998
5511	79	F	151089.96	1994-11-29	1-URGENT       	Clerk#000000438	0	ng instructions integrate fluffily among the fluffily silent accounts. bli	1994
5536	116	O	108196.56	1998-03-16	4-NOT SPECIFIED	Clerk#000000076	0	 carefully final dolphins. ironic, ironic deposits lose. bold, 	1998
5537	118	O	102207.20	1996-10-03	3-MEDIUM       	Clerk#000000742	0	ng to the daring, final 	1996
5541	143	O	37526.68	1997-09-30	3-MEDIUM       	Clerk#000000217	0	encies among the silent accounts sleep slyly quickly pending deposits	1997
5568	31	O	105421.09	1995-06-07	3-MEDIUM       	Clerk#000000491	0	 nag. fluffily pending de	1995
5569	109	F	126113.32	1993-04-30	4-NOT SPECIFIED	Clerk#000000759	0	e regular dependencies. furiously unusual ideas b	1993
5570	112	O	78567.55	1996-08-12	2-HIGH         	Clerk#000000795	0	eans. ironic, even requests doze 	1996
5572	8	F	182966.39	1994-07-17	2-HIGH         	Clerk#000000163	0	e fluffily express deposits cajole slyly across th	1994
5600	95	O	53649.35	1997-02-08	4-NOT SPECIFIED	Clerk#000000019	0	lly regular deposits. car	1997
5601	11	F	118570.79	1992-01-06	2-HIGH         	Clerk#000000827	0	gular deposits wake platelets? blithe	1992
5605	35	O	172899.84	1996-08-22	2-HIGH         	Clerk#000000538	0	sleep carefully final packages. dependencies wake slyly. theodol	1996
5606	149	O	219959.08	1996-11-12	5-LOW          	Clerk#000000688	0	uriously express pinto beans. packages sh	1996
5632	79	O	89503.11	1996-02-05	1-URGENT       	Clerk#000000508	0	ons. blithely pending pinto beans thrash. furiously busy theodoli	1996
5633	79	O	207119.83	1998-05-31	3-MEDIUM       	Clerk#000000841	0	cial deposits wake final, final	1998
5635	70	F	192217.86	1992-08-16	3-MEDIUM       	Clerk#000000734	0	nal platelets sleep daringly. idle, final accounts about 	1992
5638	109	F	79197.77	1994-01-17	1-URGENT       	Clerk#000000355	0	enly bold deposits eat. special realms play against the regular, speci	1994
5639	145	F	9669.46	1994-06-02	3-MEDIUM       	Clerk#000000005	0	ending packages use after the blithely regular accounts. regular package	1994
5665	100	F	129821.09	1993-06-28	4-NOT SPECIFIED	Clerk#000000513	0	 carefully special instructions. ironic pinto beans nag slyly blithe	1993
5667	44	O	37301.25	1995-08-10	1-URGENT       	Clerk#000000358	0	s print upon the quickly ironic packa	1995
5668	109	F	13679.32	1995-03-22	4-NOT SPECIFIED	Clerk#000000047	0	p slyly slyly express accoun	1995
5670	7	F	101429.61	1993-04-21	5-LOW          	Clerk#000000922	0	he carefully final packages. deposits are slyly among the requests. 	1993
5671	43	O	176647.54	1998-02-06	2-HIGH         	Clerk#000000838	0	k dependencies. slyly 	1998
5698	95	F	154936.43	1994-05-21	3-MEDIUM       	Clerk#000000455	0	he furiously silent accounts haggle blithely against the carefully unusual	1994
5700	143	O	79901.18	1997-12-25	1-URGENT       	Clerk#000000618	0	ly pending dolphins sleep carefully slyly pending i	1997
5701	43	O	16689.19	1997-02-07	5-LOW          	Clerk#000000798	0	 blithely final pinto beans. blit	1997
5702	97	F	153024.28	1993-09-07	4-NOT SPECIFIED	Clerk#000000743	0	ironic accounts. final accounts wake express deposits. final pac	1993
5703	121	F	1816.28	1993-05-16	3-MEDIUM       	Clerk#000000647	0	ly special instructions. slyly even reque	1993
5729	44	F	88080.33	1994-10-10	2-HIGH         	Clerk#000000843	0	uffily sly accounts about	1994
5730	11	O	10934.84	1997-12-18	1-URGENT       	Clerk#000000181	0	l platelets. ironic pinto beans wake slyly. quickly b	1997
5731	8	O	57823.37	1997-05-17	5-LOW          	Clerk#000000841	0	 silent excuses among the express accounts wake 	1997
5763	8	O	140838.11	1998-06-26	4-NOT SPECIFIED	Clerk#000000633	0	according to the furiously regular pinto beans. even accounts wake fu	1998
5767	118	F	135643.87	1992-04-29	2-HIGH         	Clerk#000000225	0	ts wake fluffily above the r	1992
5792	26	F	158991.89	1993-04-04	2-HIGH         	Clerk#000000731	0	packages. doggedly bold deposits integrate furiously across the	1993
5794	8	F	122823.78	1993-04-05	5-LOW          	Clerk#000000855	0	t accounts kindle about the gifts. as	1993
5796	149	O	23280.61	1996-01-23	3-MEDIUM       	Clerk#000000326	0	eodolites. slyly ironic pinto beans at the silent, special request	1996
5799	26	O	71381.21	1995-08-03	1-URGENT       	Clerk#000000238	0	 unusual deposits sleep blithely along the carefully even requests. care	1995
5825	61	F	23020.62	1995-02-21	5-LOW          	Clerk#000000494	0	regular packages use bravely.	1995
5827	31	O	137297.71	1998-07-23	3-MEDIUM       	Clerk#000000660	0	hely furiously blithe dolphins. slyly 	1998
5828	127	F	62172.34	1994-03-06	5-LOW          	Clerk#000000377	0	ages boost never during the final packa	1994
5829	125	O	183734.56	1997-01-11	1-URGENT       	Clerk#000000196	0	gular accounts. bold accounts are blithely furiously ironic r	1997
5857	124	O	158345.31	1997-11-06	4-NOT SPECIFIED	Clerk#000000267	0	gage blithely. quickly special ac	1997
5863	65	F	67941.54	1993-11-22	3-MEDIUM       	Clerk#000000774	0	ets about the slyly pending ideas sleep according to the blithely 	1993
5893	2	F	44777.63	1992-07-08	4-NOT SPECIFIED	Clerk#000000560	0	final sentiments. instructions boost above the never speci	1992
5921	58	F	152940.00	1994-04-07	5-LOW          	Clerk#000000125	0	kly special requests breach.	1994
5922	143	O	142494.99	1996-11-14	5-LOW          	Clerk#000000625	0	 ironic instructions haggle furiously blithely regular accounts: even platele	1996
5924	31	O	106823.97	1995-10-10	3-MEDIUM       	Clerk#000000433	0	arefully after the pains. blithely ironic pinto 	1995
5927	116	O	84983.90	1997-08-28	4-NOT SPECIFIED	Clerk#000000972	0	endencies according to the slyly ironic foxes detect furiously about the furio	1997
5953	7	F	95312.81	1992-03-28	1-URGENT       	Clerk#000000049	0	ages are furiously. slowly bold requests	1992
5957	89	F	230949.45	1993-12-27	2-HIGH         	Clerk#000000020	0	 dependencies are slyly. bold accounts according to the carefully regular r	1993
5984	70	F	83413.30	1994-06-18	5-LOW          	Clerk#000000023	0	ickly final pains haggle along the furiously ironic pinto bea	1994
5985	143	F	3942.73	1995-01-12	3-MEDIUM       	Clerk#000000417	0	as nag fluffily slyly permanent accounts. regular depo	1995
5988	31	F	41655.51	1993-11-22	4-NOT SPECIFIED	Clerk#000000867	0	fully express accounts. final pi	1993
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
1	1	4	8076	993.49	ven ideas. quickly even packages print. pending multipliers must have to are fluff
1	1	6	3956	337.09	after the fluffily ironic deposits? blithely special dependencies integrate furiously even excuses. blithely silent theodolites could have to haggle pending, express requests; fu
1	1	8	4069	357.84	al, regular dependencies serve carefully after the quickly final pinto beans. furiously even deposits sleep quickly final, silent pinto beans. fluffily reg
1	2	7	8539	438.37	blithely bold ideas. furiously stealthy packages sleep fluffily. slyly special deposits snooze furiously carefully regular accounts. regular deposits according to the accounts nag carefully slyl
1	3	4	4651	920.92	ilent foxes affix furiously quickly unusual requests. even packages across the carefully even theodolites nag above the sp
1	3	6	4093	498.13	ending dependencies haggle fluffily. regular deposits boost quickly carefully regular requests. deposits affix furiously around the pinto beans. ironic, unusual platelets across the p
1	3	8	3917	645.40	of the blithely regular theodolites. final theodolites haggle blithely carefully unusual ideas. blithely even f
1	3	10	9942	191.92	 unusual, ironic foxes according to the ideas detect furiously alongside of the even, express requests. blithely regular the
1	4	1	2480	444.37	requests sleep quickly regular accounts. theodolites detect. carefully final depths w
1	4	7	6377	591.18	ly final courts haggle carefully regular accounts. carefully regular accounts could integrate slyly. slyly express packages about the accounts wake slyly
1	5	6	3735	255.88	arefully even requests. ironic requests cajole carefully even dolphin
1	5	8	9653	50.52	y stealthy deposits. furiously final pinto beans wake furiou
1	5	10	1329	219.83	iously regular deposits wake deposits. pending pinto beans promise ironic dependencies. even, regular pinto beans integrate
1	6	1	3336	642.13	final instructions. courts wake packages. blithely unusual realms along the multipliers nag 
1	6	7	8851	130.72	usly final packages. slyly ironic accounts poach across the even, sly requests. carefully pending request
1	7	4	9460	299.58	. furiously final ideas hinder slyly among the ironic, final packages. blithely ironic dependencies cajole pending requests: blithely even packa
1	7	8	7454	763.98	y express tithes haggle furiously even foxes. furiously ironic deposits sleep toward the furiously unusual
1	7	10	2770	149.66	hould have to nag after the blithely final asymptotes. fluffily spe
1	8	1	396	957.34	r accounts. furiously pending dolphins use even, regular platelets. final
1	9	4	9583	381.31	d foxes. final, even braids sleep slyly slyly regular ideas. unusual ideas above
1	9	6	3063	291.84	 the blithely ironic instructions. blithely express theodolites nag furiously. carefully bold requests shall have to use slyly pending requests. carefully regular instr
1	9	10	7054	84.20	ts boost. evenly regular packages haggle after the quickly careful accounts. 
1	10	1	2952	996.12	 bold foxes wake quickly even, final asymptotes. blithely even depe
1	10	7	841	374.02	refully above the ironic packages. quickly regular packages haggle foxes. blithely ironic deposits a
1	11	1	3213	471.98	nusual, regular requests use carefully. slyly final packages haggle quickly. slyly express packages impress blithely across the blithely regular ideas. regular depe
1	11	8	3708	818.74	inal accounts nag quickly slyly special frays; bold, final theodolites play slyly after the furiously pending packages. f
1	12	6	7606	332.81	luffily regular courts engage carefully special realms. regular accounts across the blithely special pinto beans use carefully at the silent request
1	13	4	612	169.44	s. furiously even asymptotes use slyly blithely express foxes. pending courts integrate blithely among the ironic requests! blithely pending deposits integrate slyly furiously final packa
1	13	7	7268	862.70	s sleep slyly packages. final theodolites to the express packages haggle quic
1	13	10	864	38.64	s after the slyly pending instructions haggle even, express requests. permanently regular pinto beans are. slyly pending req
1	14	1	3676	893.39	sits are according to the fluffily silent asymptotes. final ideas are slyly above the regular instructions. furiousl
1	14	4	4947	310.13	 final deposits boost slyly regular packages; carefully pending theodolites 
1	14	8	5334	889.50	ss dependencies are furiously silent excuses. blithely ironic pinto beans affix quickly according to the slyly ironic asymptotes. final packag
1	15	6	7047	835.70	blithely quick requests sleep carefully fluffily regular pinto beans. ironic pinto beans around the slyly regular foxe
1	16	6	1491	918.51	unts cajole furiously across the fluffily pending instructions. slyly special accounts could have to boost b
1	16	7	5282	709.16	lithely ironic theodolites should have to are furiously-- 
1	16	10	9412	887.53	ly special accounts wake. fluffily bold ideas believe blith
1	17	1	7737	648.75	e blithely express accounts. foxes kindle slyly unusual dinos. quickly special f
1	17	4	3123	555.04	ly bold accounts. regular packages use silently. quickly unusual sentiments around the quickly ironic theodolites haggle furiously pending requests. care
1	17	7	3203	64.40	bold packages nag fluffily after the regular accounts. furiously ironic asymptotes sleep quickly enticing pinto beans. carefully pending accounts use about the 
1	17	8	8555	995.35	are furiously final accounts. carefully unusual accounts snooze across the requests. carefully special dolphins
1	18	8	6475	386.29	. furiously regular accounts cajole slyly across the pending
1	19	6	8800	635.66	sual requests sleep carefully. deposits cajole carefully over the regular, regular requests. quickly unusual asymptotes use some
1	19	10	1416	144.80	o beans. even packages nag boldly according to the bold, special deposits. ironic packages after the pinto beans nag above the quickly ironic requests. bl
1	20	1	2927	675.54	s, ironic deposits haggle across the quickly bold asymptotes. express, ironic pinto beans wake carefully enticingly special foxes. requests are at the c
1	43	6	9506	493.65	riously! slyly ironic sauternes affix. ironic theodolites sleep furiously about the express packages. slyly ironic deposits are blithely against the regular package
1	20	4	2723	305.84	nal, bold frets cajole slyly regular, unusual platelets. slyly permanent deposits wake carefully carefully silent accounts. even, even requests wake quickly. furiously pending packages are
1	20	7	5905	546.66	ing deposits use furiously. ironically final pinto bea
1	20	10	4271	115.89	xcuses wake at the deposits. regular pinto beans nag slyly fluffi
1	21	4	367	584.86	ong the even theodolites. pending, pending accounts sleep-- courts boost quickly at the accounts. quickly fin
1	21	6	1704	139.05	posits cajole; quickly even requests sleep furiously. ironic theodolites sleep pending, express instructions. stealthily even platelets cajole carefully after the final, ironic p
1	21	10	7153	664.50	blithely enticing instructions use alongside of the carefully thin deposits. blithely bold requests are fluffily
1	22	1	7834	359.16	sits wake fluffily carefully stealthy accounts. furiously ironic requests x-ray fluffily alongside of the pending asymptotes. slyly silent packages use along the instructions. fu
1	22	7	9779	635.84	l instructions cajole across the blithely special deposits. blithely pending accounts use thinly slyly final requests. instructions haggle. pinto beans sleep along the slyly pen
1	23	4	2739	460.12	platelets against the furiously bold Tiresias dazzle quickly into the special, bold courts. silent, regular instructions wake blithely ironic multipliers. ideas
1	23	6	7035	51.75	xcuses; decoys wake after the pending packages. final instructions are furi
1	23	8	5739	103.13	 theodolites need to nag blithely final notornis. slyly idle packages cajole after the furiously stealthy packages. slyly regular accounts use furiously. carefully final accounts affix 
1	24	7	5318	62.15	 careful requests cajole blithely realms. special asymptotes sleep. pinto beans sleep carefully furiously ironic packages. furiously 
1	25	4	9946	694.35	ld, ironic requests. furiously special packages cajole furiously enticing instructions.
1	25	6	9029	832.74	fully fluffily regular frets. sometimes even requests after the requests wake slyly at the quickly ruthless requests. a
1	25	8	7340	746.59	dly final packages haggle blithely according to the pending packages. slyly regula
1	25	10	9062	928.96	he foxes. final, final accounts sleep. boldly ironic excuses thrash quick
1	26	1	6577	892.20	riously pending pinto beans. furiously express instructions detect slyly according to the b
1	26	7	5020	683.96	es. fluffily express deposits kindle slyly accounts. slyly ironic requests wake blithely bold ideas
1	27	6	3407	151.34	ironic theodolites are by the furiously bold ideas. ironic requests shall have to sublate final packages. furiously quick foxes alongside of the express, special deposits was boldly according 
1	27	8	2111	444.01	the even, ironic deposits. theodolites along the ironic, final dolphins cajole slyly quickly bold asymptotes. furiously regular theodolites integrate furiously furiously bold requests. carefully
1	27	10	4283	348.61	ound the final foxes detect furiously across the even warhorses. quickly t
1	28	1	9988	666.53	beans haggle carefully around the slyly ironic acco
1	28	7	302	690.30	uickly unusual requests alongside of the final courts integrate slyly 
1	29	4	8106	981.33	the ironic, bold asymptotes! blithely regular packages hang furiously above the dependencies. blithely permanent dependencies are furiously furiously ironic acco
1	29	8	9193	734.44	ly unusual packages. foxes cajole. theodolites nag
1	29	10	3506	799.27	leep fluffily according to the quietly regular requests: accounts integrate carefully bold foxes. carefully silent
1	30	1	4767	989.05	ts. slyly final pinto beans cajole ironic accounts. blithely final accounts use among the request
1	31	7	137	849.11	blithely ironic accounts. slyly ironic asymptotes sleep ironic, even accounts. regular accounts thrash quickly
1	32	8	467	109.34	ainst the unusual braids nod fluffily packages. regular packages nod among the slyly express
1	33	4	1287	310.76	dolites above the slyly express deposits try to haggle blithely special gifts. blithely ironic reque
1	34	10	4749	265.31	ckly regular theodolites eat above the bravely regular courts. ironic requests wake slyly.
1	35	1	2025	411.17	s cajole fluffily final deposits. furiously express packages after the blithely special realms boost evenly even requests. slow requests use above the unusual accoun
1	35	6	596	669.19	slyly against the daring, pending accounts. fluffily special pinto beans integrate slyly after the carefully unusual packages. slyly bold accounts besides
1	36	7	2625	569.91	ctions. pending requests are fluffily across the furiously regular notornis. unusu
1	49	10	9056	35.11	 bold deposits? final, bold pinto beans are furiously slyly regular packages. sly
1	37	8	7113	15.72	re bravely along the furiously express requests. blithely special asymptotes are quickly. fluffily regular packages alo
1	38	4	3516	847.09	nal accounts. furiously pending hockey players solve slyly after the furiously final dependencies. deposits are blithely. carefully regular packages unwind busily at the deposits. fluffily 
1	39	10	3633	463.10	kages are slyly above the slyly pending pinto beans. bold, ironic pinto beans sleep against the blithely regular requests. fluffily even pinto beans use. regular theodolites haggle against the quic
1	40	1	4521	374.71	ptotes haggle. slyly even requests nag fluffily silent packages. blith
1	40	6	1704	565.82	riously furiously silent asymptotes. final deposits cajole blithely ironic requests. furiously special pains into the blithely final instru
1	41	4	1550	916.55	 the blithely final ideas. furiously regular asymptotes could cajole furious
1	41	8	5946	391.81	 slyly slyly regular requests. final deposits sleep fluffily. blithely bold instructions detect carefully. blithely pending requests are furiously ironically final ideas. regul
1	41	10	560	37.59	special pinto beans against the unusual accounts cajole slyly final foxes. close, ironic
1	42	1	3662	29.46	es sleep slyly among the slyly final requests. bold theodolites use silently against the final foxes. carefully pending requests use furiously. dogged, unusual asymptotes use 
1	43	4	3211	805.78	gular accounts. bold theodolites nag slyly. quickly express excuses use blithely. blithely even ideas boost fluffily! blithely unusual ideas detect bli
1	43	10	6770	493.19	ing to the quickly even theodolites. quickly bold excuses haggle. sometimes unusua
1	44	1	5310	114.37	quests. quickly unusual requests against the carefully final somas detect slyly bold a
1	44	7	3534	383.01	r the pending pinto beans! requests wake furiously after the special deposits. silent deposits mold quickly along the express, special 
1	45	4	1872	155.32	 ironic, even pinto beans. bold theodolites haggle after the furiously ironic accounts. slyly bold courts
1	45	6	1685	919.63	he doggedly final accounts; carefully regular packages cajole idly regular idea
1	45	8	5669	532.70	es play carefully doggedly unusual requests. bold grouches against the furiously ironic dugouts sleep furiously qu
1	46	7	4171	244.65	lly quiet instructions. furiously express requests among the final ideas cajole carefully bold waters. furiously regular pac
1	47	4	4458	539.47	; finally enticing theodolites cajole enticing, silent warhorses! slyly bold pains c
1	47	6	5873	296.63	after the regular dependencies. final, bold pains sleep quickly pend
1	47	8	6989	292.52	even ideas. blithely final requests boost blithely. final, ironic instruct
1	47	10	2896	74.54	grate final asymptotes. pending requests kindle carefully final frets. ironic deposits above the slyly e
1	48	1	5564	668.19	al pinto beans. furiously final frays use slyly according to the ironic theodolites. regular ideas cajole furiously after the slyly even deposits. 
1	48	7	1719	606.16	forges lose. packages cajole regular, bold accounts. never ironic accounts may promise about the permanently bold deposits. always express requests cajole fluffily regular c
1	49	6	6646	908.15	ts sleep across the fluffily final deposits. carefully express accounts around the regular, express excuses x-ray inside the ironic theodolites. expre
1	49	8	597	812.62	n foxes snooze furiously. courts integrate never. carefully unusual requests are carefully. quickly ironic deposits ha
1	50	1	1832	565.54	liers above the dolphins dazzle across the regular foxes. furiously regular packages haggle furiously blithely ironic grouches. ironic, even accounts haggle pending, furious instruction
1	50	7	43	690.87	aggle daringly along the close, express deposits. final requests snooze carefully carefully bold deposits. carefully unusual ideas doze furiously after the furious
1	51	6	138	728.95	requests according to the carefully unusual deposits promise slyly ironic packages. slyly ironic dependencies are accordin
1	52	4	5524	424.93	cial, ironic packages. even dolphins boost. slyly final deposits integrate. final sheaves along the silent excuses use at the slyly close foxes; bold accounts are finally even packages. ironi
1	52	7	4084	628.53	 deposits wake slyly pending asymptotes. ironic asymptotes haggle. blithely ironic requests are qui
1	52	10	1937	210.44	s. never even asymptotes nag carefully! regularly unusual foxes along the unusual requests haggle accounts. fluffily express pinto 
1	53	1	5319	563.44	ly. fluffily final pearls boost carefully. special sauternes nod furiously even instructions. carefully regular dependencies across the slyly regular deposits
1	53	4	6443	192.78	carefully ironic accounts. blithely bold deposits detect furiously against the flu
1	53	8	8200	388.08	fully requests. furiously final accounts cajole express, regular pearls. special deposits wake fluffily express accounts. quic
1	54	6	536	259.24	 furiously along the fluffily regular requests. carefully unusual accounts use fluffily final platelets. pending deposits integrate furiou
1	55	6	7874	611.04	ly special packages. furiously even warhorses integrate. silen
1	55	7	1289	130.33	ut the blithely final requests. requests nag blithely. 
1	55	10	8278	134.62	gedly silent pinto beans! furiously regular sentiments was furiously across the silent pinto beans. pending warthogs along the slyly 
1	56	1	1330	52.29	xpress instructions haggle furiously regular deposits. quickly unusual packages sleep furiously final pinto
1	56	4	9104	54.79	tructions above the blithely pending foxes cajole blithely furiously even sentiments. special, exp
1	56	7	241	855.39	nto beans. finally regular sauternes are. carefully bold deposits according to the blithely express requests wake carefully ironic excuses? furiously final deposit
1	56	8	5799	926.25	ades grow around the dependencies. carefully special ideas cajole furiously across the blithely express requests. unusual tithes are caref
1	57	8	2972	123.11	 asymptotes use carefully furiously final deposits. quickly regular deposits are furiously slyly ironic requests. blithely even excuses haggle: blithely special ideas
1	58	6	4307	448.31	quickly carefully ironic foxes. bold platelets nag furiously regular packages. slyly specia
1	58	10	9689	25.09	long the unusual, express asymptotes. ironic ideas boost bold, special deposits? ironic foxes among the fin
1	59	1	8184	45.50	ependencies. ironic dependencies wake carefully according to the blithely bold packages. quickly unusual ideas about th
1	59	4	99	598.55	he special pinto beans. fluffily even accounts cajole. fluffily regular foxes haggle among the
1	59	7	4226	80.98	lar packages. regular depths use slyly after the fluffily regular packages; theodolites around the furiously ironic asy
1	59	10	8374	357.22	c decoys. carefully even pinto beans wake slyly alongside of the express accounts. regular grouches haggle.
1	60	1	6642	800.72	 blithely. slyly final realms alongside of the excuses use quickly blithely bold foxes. final theodolites are slyly after the slyly regular excuses. never thin foxes about 
1	60	8	5017	314.81	 even pinto beans wake carefully. quickly regular deposits hinder along the furiously regular pack
1	61	6	7312	153.74	gly final instructions. pending theodolites will wake furiously. slyly bold instructions run. furiously special foxes cajole f
1	61	8	4762	633.74	final theodolites haggle. fluffily express ideas about the silent theodolites cajole ideas; fluffily special instructions are accordin
1	61	10	9170	771.26	ly. pinto beans sleep blithely about the patterns. slyly final accounts wake according to the furiously bold requests. slyly regular packages wake according to the ironic packages. requests acros
1	105	8	269	158.62	 unusual courts eat pending excuses. ironic, ironic requests use. bravely 
1	62	1	5896	348.82	 final accounts. furious deposits wake slyly. idly regular packages haggle blithely pending grouches. ironic accounts boost blithely. carefully express pa
1	62	7	9542	255.78	lly express requests haggle carefully. idle, pending pinto beans are furiously regular excuses. quickly sly attainments are furiously; even accounts are slyly quickl
1	63	4	1804	498.84	leep bravely. final accounts nag. forges sleep against the slyly ironic pa
1	63	8	6325	463.69	arly express accounts. express, unusual escapades haggle. special packages must wake. express, regular requests sleep furiously ironic packages
1	63	10	6839	274.15	 among the carefully ironic accounts. carefully even accounts against the regular, final deposits detec
1	64	1	9110	602.65	 ironic accounts are carefully carefully final accounts. slyly ironic packa
1	65	4	1779	393.63	ully after the quickly regular ideas. ironic, final multipliers above the carefully bold deposits breach slyly furiously express deposits. unusual accounts haggle carefully idea
1	65	6	2918	846.26	inal, even foxes cajole. furiously final dolphins hang quickly ironic foxes. furiously special packages alongside of the bold foxes solve above the carefully final instructio
1	65	10	2188	288.73	lent requests nag quickly. blithely silent platelets haggle ironic accounts. slyly bold instructions boost carefully final accounts. carefully even dependencies must nag blithely; qui
1	66	1	296	797.27	s nag enticingly outside the furiously final foxes. final accounts haggle fluffily accord
1	66	7	3077	809.13	nod carefully besides the furiously final theodolites. slyly final requests haggle. furiously silent excuses detect quickly. ironic deposits detect above the furiously final 
1	67	4	3368	625.62	le slyly regular requests: regular platelets wake quickly across the quickly regular accounts. reg
1	67	6	7908	546.75	furiously express dolphins integrate carefully regular notor
1	67	8	9923	306.37	ly according to the quickly ironic requests. express instructions after the slyly even instructions x-ray blith
1	68	7	6762	5.16	lithely. carefully even grouches along the bold deposits might sleep slyly requests. blithel
1	69	4	7017	344.28	heodolites. unusual, regular requests boost slyly pending deposits. slyly daring instruct
1	69	6	9294	386.96	ar packages. blithely regular dependencies are dolphins. slyly ironic excuses nag quickly pending, regular ideas. furiously special sheaves haggle. close, regular pinto beans about the slyly bold
1	69	8	8235	846.49	nt fluffily. carefully ironic instructions wake. blithely express foxes cajole slyly. unusual requests sleep quickly. final packages affix slyly according to the spec
1	69	10	6197	694.24	eep across the packages. regular, final foxes boost fluffily regular pinto beans. packages sleep along the final requests. bold, unusual packages cajo
1	70	1	4536	348.27	ructions. blithely final packages cajole carefully after the express, even requests. furiously final theodolites cajole 
1	70	7	2990	940.81	s deposits. unusual foxes are carefully according to the carefully even deposits. carefully ironic foxes cajole fluffily against the carefully pending deposits. slyly special depo
1	71	1	8329	239.57	ins sleep carefully slyly express accounts! quickly even accounts boost carefully about the carefully regular excuses. dogged, even dolphins against the sometimes ironic packages believe bl
1	71	10	6768	744.67	ructions. daring requests solve carefully about the furiously pending pinto
1	72	1	2654	762.61	nusual packages: blithely bold Tiresias sleep furiously. slyly brave accounts according to the final, 
1	72	10	4526	154.47	use across the never ironic packages. express, regular accounts above the pending, fluffy deposits are carefully across the slyly even pinto be
1	73	1	3928	309.57	longside of the blithely final ideas. carefully ironic courts sleep along the enticingly pending requests. fluffily regular accounts use fluffily bold ideas. slyly ironic packa
1	73	4	9873	947.99	tes use pending packages. final foxes wake final, unusual packages. blithely blithe ideas haggle sometimes slyly express accounts. express instructions nag furiously quickly
1	74	4	2479	930.97	o beans sleep dependencies. regular accounts use blithely asymptotes. u
1	75	4	9080	433.59	sits. permanent packages breach. carefully final waters wake. bold, pending foxes haggle furiously evenly express instructions. even deposits about the final
1	75	6	7086	624.39	sits are furiously fluffily even courts. furiously pending requests are blithely. pending, regular accounts play carefully slyly unusual platelets. blithely final requests against the ru
1	76	4	7986	252.03	 packages across the furiously ironic platelets cajole across the regular, ironic accounts. carefully enticing accounts among the blithely regular instructions detect regular pinto be
1	76	6	2009	108.97	n packages. blithely even accounts sleep carefully furiously ironic accounts. carefully express requests
1	76	7	6754	494.83	gular accounts solve. ironic deposits sleep slyly even packages. slyly pending accounts detect slyly express accounts. ironic forges can play furiously carefully express fox
1	77	6	8541	936.13	e slyly express instructions haggle about the sometimes regula
1	77	7	8170	875.83	xcuses. blithely even foxes use fluffily. blithely even requests use. slyl
1	77	8	552	254.92	e after the carefully pending packages. carefully even dependencies cajole pending 
1	78	6	9599	382.82	 carefully special theodolites cajole among the quickly even asymptotes. foxes wake blithely across the carefully 
1	78	7	1801	434.34	nts kindle furiously according to the even packages. blithely ironic platelets are slyly silent foxes. final, final packages would sleep. pinto beans a
1	78	8	7246	577.23	regular dependencies cajole doggedly ironic accounts. bold theodolites doze about the accounts. quickly final requests boost slyly final asymptotes. carefully final dolphins ha
1	79	7	8627	891.18	r dolphins grow blithely against the slyly ironic packages. deposits about the regular, ironic decoys are slyly around the carefully regular packages. slyly pending excuses sle
1	79	8	3309	880.23	tect final, thin accounts? furiously ironic accounts boost regular deposits. carefully ironic attainments sleep. furiously special ins
1	105	10	8855	810.86	. slyly special depths sleep. bold packages engage furiously bold packages. fluff
1	79	10	4248	765.34	nusual, express asymptotes wake furiously. ironic pinto beans detect above the carefully express theodolites: even, dogged instructions nag. spe
1	80	1	8893	127.65	ld accounts detect carefully. carefully bold courts along the regular deposits could have to affix ca
1	80	8	4034	797.05	ptotes cajole carefully. express ideas cajole carefully even somas. final pinto beans print fluffily across the 
1	80	10	2243	775.79	endencies. bold, regular pinto beans wake furiously above
1	83	4	3010	745.51	l foxes along the bold, regular packages integrate carefully express courts! final excuses sleep carefully ironic
1	85	6	118	917.83	 against the even deposits. furiously bold ideas along the furious requ
1	86	7	806	65.98	ackages. blithely pending accounts are slyly furiously pending theodolites. furiously eve
1	87	8	1272	435.42	ronic foxes sleep along the special foxes. final ideas wake quickly about the carefully special theodolites. blithely ironic packages are blithely. regular, regular pint
1	89	10	924	920.02	ickly unusual asymptotes after the slyly unusual accounts are carefully doggedly ironic accounts. even, final accounts use furiousl
1	90	1	7629	50.84	onic requests wake fluffily unusual packages. furiously even frays after the daringly pending requests wake furiously alongside of the bold requests. fluffily ironic ideas nag. ironic,
1	91	4	483	823.21	n: slyly ironic foxes nag blithely according to the furiously bold foxes. regular, regular accounts a
1	92	4	2246	985.03	jole enticingly regular asymptotes. carefully unusual pinto beans nag carefully ironic ideas. quickly un
1	92	6	1044	854.89	l instructions are fluffily silently regular accounts. quickly final dolphins w
1	93	4	3008	615.98	sits promise blithely fluffily special decoys. slyly regular packages along the slyly final deposits wake accord
1	93	6	3869	868.81	ly among the furiously silent accounts. closely regular pinto beans nag slyly! slyly e
1	93	7	7188	805.90	y furiously bold pinto beans. express asymptotes was quickly. carefully final accounts affix slyly! platelets according to the ca
1	94	6	7784	358.08	ironic packages wake slyly carefully regular accounts. quickly regular warhorses against the blithely ironic packages haggle doggedly sly
1	94	7	7232	478.94	y regular requests. carefully final asymptotes haggle carefully against the slyly unusual requests: blithely brave grouches are fu
1	94	8	3261	824.08	quests. enticingly final accounts sleep fluffily. quickly express asymptotes around th
1	95	6	5186	291.03	ites across the blithely pending theodolites do affix across the unusual, bold Tiresias. bold packages
1	95	7	6552	456.36	tes; final, final accounts boost blithely ironic pinto beans. blithely ironic deposits cajole above the quickly pending requests? i
1	95	8	367	987.22	 express requests detect furiously. requests cajole carefully
1	96	7	5739	202.06	re. slyly regular theodolites breach slyly even dinos. fluffily regular asymptotes haggle slyly. fluffily bold courts affix furiously. regular requests 
1	96	8	4942	571.30	e carefully. bold packages sleep against the furiously express requests. express foxes above the dependencies use quickly according to the slyly expres
1	96	10	7250	587.08	efully ironic foxes. regular, final pinto beans boost above the express a
1	97	1	4580	761.41	 beans. carefully final deposits alongside of the carefully final requests haggle idly blithely ironic accounts. foxes cajole slyly against the ironic, special packages. furiously brave excuses boo
1	97	8	6371	129.77	fluffily unusual accounts. slyly regular theodolites integrate furiou
1	97	10	2618	239.34	al theodolites are daringly requests. warhorses sleep blithely requests. special accounts cajole slyly deposits. a
1	98	1	3443	139.00	gular pinto beans maintain quickly fluffily regular deposits. express requests sleep. even requests after the regu
1	98	10	8550	657.16	 sleep carefully. bravely bold somas may sleep pendin
1	99	1	7567	496.93	es? permanently even excuses haggle quickly across the dependencies.
1	99	10	8487	438.38	lphins affix ironic packages. blithely ironic requests nag fluffily after the slyly ironic foxes. bold dependencies boost furiously. special, 
1	100	1	7885	490.61	 accounts nag slyly against the bold excuses. pearls according to the fluffily ironic accounts haggle fluffily along the quickly final platelets
1	100	4	9688	22.00	uctions according to the carefully ironic deposits haggle carefully express ideas? packages across the quickly final requests c
1	101	4	8263	218.71	fluffily final requests. carefully even packages wake quickly about the quickly ironic foxes. fluffily even requests hang quickly about the pending, final requests. sp
1	101	6	6324	786.53	olites sleep quickly. slyly ironic theodolites affix. furiously bold accounts integrate among the pinto beans. final ideas hang slyly along the quickly regular packages. instructions cajole.
1	101	8	55	612.09	beans against the carefully express ideas wake quickly along the quickly unusual requests. blithely regular accounts cajole fluffily. enticingly pending theodolites haggle furiously fluffily pendi
1	102	7	9156	618.00	 the unusual, ironic pinto beans. theodolites above the foxes sleep slyly car
1	103	4	5913	905.88	e across the theodolites. carefully pending escapades haggle after the ironic theodolites. furiously pending ac
1	103	6	7742	414.42	bout the bold, regular deposits; blithely even accounts are regular, even platelets-- carefully express accounts nag slyly pen
1	103	8	5164	361.48	furiously thin deposits haggle blithely. blithely regular deposits above the carefully regular accounts are slyly carefully regular packages. silent, unusual
1	103	10	429	605.20	 theodolites cajole quickly above the asymptotes-- slyly special packages can haggle carefully blithely final instructions. unusual, regular ideas
1	104	1	3369	729.38	ong the foxes. foxes sleep quickly? carefully regular accounts sleep. special foxes boost quickl
1	104	7	6110	15.57	ending requests. carefully regular deposits use blithely. bold, ironic deposits wake slyly carefully specia
1	105	6	4602	27.75	lar pearls cajole never carefully even depths. blithely regular ideas are quickly. unusual asymptotes nod carefully carefully regula
1	106	1	1411	310.40	al accounts impress. even instructions engage furiously final foxes. silently final deposits wake qui
1	106	7	8649	732.15	 slyly ironic instructions are. bold, final accounts cajole slyly ironic pinto beans. fluffily ironic accounts around the quickly special requests use blith
1	107	4	7912	474.77	fluffily across the final, bold accounts. quickly regular deposits grow carefully deposits. regular requests haggle blithely. slyly special platelets boost furiously care
1	107	8	7249	168.03	he fluffily even packages. slyly regular dependencies nag fluffily above the final, unusual foxes. final, pending foxes affix. furiously final deposits cajole quickly blithely
1	107	10	4029	91.31	integrate. requests maintain quickly. carefully regular ideas about the instructions sle
1	108	1	4898	241.47	lthily according to the fluffy deposits. furiously silent ideas according to the furiously special theodolites wake furiously a
1	109	4	8176	936.60	d the express accounts. even theodolites wake quickly up the furiously bold foxes. furiously regular packages use regular, bold
1	109	6	7524	374.49	sual requests. final pinto beans cajole furiously. dependencies integrate slyly even excuses. blithely special requests wake around the slyly final pinto beans. furiously pending requests wake furi
1	109	10	524	275.19	st the permanently final requests. carefully pending pinto beans haggle quickly slyly ironic dolphins. blithely bold deposits wake blithely. even requests cajole foxes. iro
1	110	1	2782	566.46	ly sly deposits. regular deposits are; theodolites haggle furiously bold foxes. final pinto beans wake blithely furiously ironic depths. unusual, regular platelets cajole. final, ironic
1	110	7	4927	196.63	theodolites. instructions haggle even packages. waters impress furiously quickly express courts. furiously ironic depths nod quickly? ironic, ironic requests sle
1	111	1	2175	496.08	s around the daringly final pinto beans use furiously against the pi
1	111	8	8069	745.33	thely bold requests. unusual packages sleep. quickly pending ideas nag furiously to the carefully final p
1	112	6	7652	783.52	 carefully among the furiously final packages. regular instructions nag. slyly pending ideas hang fluffily blithely ironic instructions. ironic deposits haggle except the quickl
1	113	4	9981	396.26	ithely express pains lose bravely fluffily pending foxes. blithely ironic sauternes cajole q
1	113	7	3804	860.68	ully about the carefully even accounts: thinly even foxes are carefully. 
1	113	10	522	981.41	 warthogs use quickly alongside of the furiously unusual requests. final deposits are blithely acro
1	114	1	4519	382.87	nts. ironically express dolphins dazzle blithely. special instructions wake carefully along the ideas. quickly special dolphins sleep. furiously pendi
1	114	4	2113	570.79	o beans sleep among the ironic excuses. furiously even sheaves are. never regular instructions nod.
1	114	8	3062	555.12	ts. furiously regular requests run carefully thin decoys. ironic platelets sleep alongside of the slyly silent deposits. reg
1	115	6	1817	82.84	uffily final accounts integrate furiously along the carefully busy excuses. slyly even asymptotes doubt quickly. fluffily thin theodoli
1	116	6	5632	37.30	ccounts about the special packages nag across the carefu
1	116	7	6580	705.50	iously. slyly regular requests detect slyly. carefully bold packages sleep furiously carefu
1	116	10	4975	413.86	tions. regular excuses detect. ideas haggle slyly about the slyly ironic courts. ironic foxes solve. ideas affix fluffily after the special, even dependencies. final platelets according
1	117	1	1986	322.08	eposits. special pinto beans use fluffily across the furiously regular pinto beans. furiously regular epitaphs nag fluffily packages. special accounts a
1	117	4	2577	761.86	riously. doggedly unusual ideas boost blithely blithely regula
1	117	7	4762	552.88	 run furiously ironic accounts. slyly ironic deposits haggle slyly fluffy requests. flu
1	117	8	5906	706.51	into beans sleep carefully blithely bold packages. even, bold instructions use across the carefully e
1	118	8	4951	976.55	s orbits. even asymptotes above the instructions wake fluffily according to the sly, final excuses. express deposits across the blithely ironic depend
1	119	6	4955	488.93	ias are along the express requests. fluffily pending ideas nag idly against the fluffily bold instructions? foxes cajole quickly. slyly special deposits haggle slyly e
1	119	10	2307	473.64	 blithely unusual dolphins boost busy, express ideas. regular requests use carefully furiously ironic deposits. carefully regular packages would sle
1	120	1	4976	201.21	inal, regular pinto beans haggle carefully! ironic ideas unwind among the slyly regular theodolites. regular platelets kindle blith
1	120	4	7744	365.79	l, special escapades! ideas sleep slyly instructions. carefully bold requests are. even accounts cajole. final accounts use slyly 
1	120	7	5329	249.61	s cajole blithely. carefully bold requests believe blithely? brave accounts above the pending, dog
1	120	10	3102	566.34	ctions; realms beside the blithely final theodolites unwind blithely packages. regular dolphins sleep carefully-- carefully express accounts wake quickly. pending depths use never courts.
1	121	4	8709	655.22	 detect carefully along the carefully even pinto beans. gifts haggle: ideas sleep ar
1	121	6	4246	850.42	usly final instructions. theodolites are according to the permanently ironic accounts. carefully pending accounts haggle about the pending instructio
1	121	10	7670	449.39	carefully daring packages. express packages use carefully about the quickly unusual packages. special ideas along 
1	122	1	4957	650.84	quests. quickly bold requests nag across the furiously ironic accounts. ironically express instructions detect slyly carefully ironic requests. even, un
1	122	7	2490	637.28	efully special excuses grow slyly unusual packages. carefully quiet as
1	123	4	9881	107.03	fully bold deposits detect slyly pending instructions. slyly special ideas detect blithely. slyly fluffy instructions hinder
1	123	6	5311	149.65	eposits cajole according to the carefully pending packages. furiously final epitaphs solve alongside of the even requests
1	123	8	5638	818.19	thely even pinto beans. furiously regular asymptotes affix furiously. regular, ironic tithes integrate quickly. blithely regular requests breach finally. decoys alon
1	124	7	9416	822.78	ously. theodolites affix around the slyly bold packages. even, ironic packages are carefully pains. furiously unusual requests sleep blith
1	125	4	2600	455.70	ounts. thinly special accounts cajole carefully. even, special accounts after
1	125	6	2263	358.45	e. ironic, regular requests cajole fluffily along the even ideas. final ideas wake blithely. blithely bold 
1	125	8	5546	806.66	 to the unusual courts are deposits! final, final pinto beans solve slyly. ironic accounts boost fluffily. furiously pending d
1	125	10	8276	668.65	nd the carefully express requests. slyly regular requests haggle. blithely unusual platelets solve fluffily fluffily regular
1	126	1	2373	194.38	fter the ideas. blithely daring sheaves print furiously among the blithely final packages. iron
1	126	7	2647	221.89	lyly final pinto beans across the regular, even courts use slyly slyly pending braids! unusual requests along the furious
1	127	6	8281	3.14	ts above the furiously pending asymptotes cajole after the deposits. slyly ironi
1	127	8	7658	712.33	al pinto beans! slyly ironic excuses boost after the packages. express foxes integrate carefully. pending, regular theodolites 
1	127	10	8894	73.42	fter the sometimes special courts sleep about the slyly unusual reque
1	128	1	7023	875.78	 furiously quickly regular pinto beans. always special requests are. quickly regular deposits are furiously. slyly unusual theodolites haggle evenly; furiously special deposits wa
1	128	7	3766	947.16	arefully regular packages boost regularly. accounts are according to the blithely even dependencies. slyly silent accounts doubt slyl
1	129	4	7242	200.26	es across the furious escapades wake quickly slyly e
1	129	8	5299	330.59	final sentiments affix atop the silent foxes. busy pinto beans cajole. slyly final pinto beans haggle against the carefully expres
1	129	10	5721	129.69	ully express requests above the ironic, final requests cajole slyly along the quickly special packages. sl
1	130	1	4928	223.38	ths. slyly even theodolites detect according to the slyly final courts. carefully unusual deposits ar
1	131	7	125	861.84	l accounts grow quickly-- slyly ironic requests haggle? quickly express pinto bean
1	132	8	1904	925.73	the regular foxes wake ironic deposits. ironic, special requests use blithely instructions! final requests hang. blithely regular deposits haggle. ir
1	133	4	4568	57.48	dolites. ironic accounts are blithely pinto beans. regular pinto beans haggle beneath
1	134	10	6270	388.28	 to the furiously pending deposits nag along the slyly express asymptotes. slyly silent accounts shal
1	135	1	2443	9.83	atterns. pending, special deposits are furiously. express, regular deposits integrate quickly. unusual gifts cajole blithely stealthily pending deposit
1	135	6	6940	465.82	ding foxes cajole. even dugouts haggle busily. fluffily pending packages about the express excuses boost slyly final packages. blithely express ideas cajole about the carefu
1	136	7	2237	548.19	ond the silent accounts haggle above the blithely regular packages
1	137	8	467	371.85	ly special accounts detect carefully. furiously ironic deposits nag express packages. slyly quiet 
1	138	4	967	309.03	pendencies integrate against the unusual pains. carefully unusual theodolites wake quickly across the deposits. blithely regular deposits alongside of the carefully regular deposits
1	139	10	1042	972.23	gular, regular theodolites. regular asymptotes haggle carefully according to the permanently even deposits. slyly special account
1	140	1	304	45.84	ing requests. carefully unusual foxes are final requests. slyly regular accounts wake permanently. quickly ironic theodolites hagg
1	140	6	3533	781.45	ayers. carefully ironic pinto beans nod carefully furiously regular pinto beans. slyly ironic requests after the carefully regular packages are about the blithel
1	141	4	7628	838.08	sly about the pinto beans. blithely ironic ideas sleep. foxes are quietly among the pinto beans. carefu
1	141	8	1660	139.18	egular accounts. enticingly bold theodolites eat slyly across the never ironic platelets. theodolites wake bli
1	141	10	90	810.68	e doggedly regular ideas. foxes haggle slyly. slyly regular theodolites across the carefu
1	142	1	3858	854.08	efully special deposits. blithely bold pinto beans haggle. slyly final ideas boost blithely. finally special requests mold along the blithely express packages. entic
1	143	4	7326	960.64	the slyly pending requests cajole quickly blithely regular platelets. even requests boost carefully. ironic, final instructions above the regular courts boost a
1	143	6	7152	772.24	fully furious accounts. final asymptotes cajole regular requests. carefully regular courts are quickly. slyly ironic ideas above the carefully regular requests wake
1	143	10	3923	741.01	le quickly furiously silent ideas. carefully regular requests ar
1	144	1	494	849.96	uriously ironic pearls wake idly furiously even pearls. foxes impress slyly busily express requests. carefully slow somas wake quick
1	144	7	1799	713.88	yly final requests. packages are. carefully daring accou
1	145	4	4332	894.57	are carefully above the quickly silent deposits. evenly bold reque
1	145	6	11	641.67	 slyly regular packages are slyly carefully special dolphins. unusual braids use furiously about the final courts. slyly special
1	145	8	1344	422.60	 dependencies. even patterns detect slyly after the ironic deposits. ironically even ideas wake slyly. even packages against the blithely express accounts haggle furiously carefully regular
1	146	7	2726	231.15	uriously after the fluffy accounts. furiously bold deposits cajole. requests might engage. quick accounts wake carefu
1	147	4	7647	102.19	refully regular orbits about the furiously express asymptotes haggle carefully according to the blithely regular ideas. blithely express excuses around the furiously 
1	147	6	4235	235.91	ar pinto beans. regular instructions sleep carefully after the furiously blithe accounts. slowly pending ideas could nag careful, even accounts. attainments use slyly quickly
1	147	8	7442	939.14	sts against the furiously unusual instructions integrate ironic accounts. slyly final pinto beans sleep blithely carefully final asymptotes. slyly ironic warhorses befor
1	147	10	1596	466.37	ole. slyly final packages do haggle quickly. unusual accounts across the pending pinto beans was furiously according to the furiously brave deposits. pending deposits along the regular request
1	148	1	5393	264.09	ses. slyly pending packages haggle fluffily fluffily even instructions. fluffily regular packages are carefully about the furiously even asymptot
1	148	7	2957	250.98	. requests boost above the bold, special foxes. blithely regular platelets serve blithely slyly final ideas. carefully special idea
1	149	6	7283	201.03	usly bold instructions. regular, final deposits alongside of the furiously ironic platelets are slyly even instructions. carefully bold accounts are. ironic, regular requests nag furious
1	149	8	4104	312.37	ly express excuses. bold pinto beans boost blithely across the bold, final pinto beans. final deposits haggle carefully from the
1	149	10	959	679.10	y to sleep carefully ironic requests. even, regular dependencies haggle. slyly unusual foxes haggle along the instructions. quickly even accounts nag furiously special accoun
1	150	1	8091	524.71	sleep furiously furiously bold warthogs. furiously express gifts according to the regularly silent sentiments boost within the f
1	150	7	2721	814.79	dependencies. special accounts wake carefully furiously regular accounts. regular accounts haggle along the express instructions. express pinto beans along the express, bold deposits run 
1	151	6	1484	71.68	riously final requests sleep according to the regular deposits? slyly ironic ideas wake furiously. quickly even theodolites use fluffily. regular, unusual courts according to the regular 
1	152	4	1283	142.73	olites above the furiously even requests dazzle blithely against the busy, regular pains. furiously blit
1	152	7	3599	77.38	 quick excuses according to the pending, ironic requests snooze carefully slyly even foxes: slyly regular instru
1	152	10	2653	432.12	carefully pending requests. quickly ironic requests haggle carefully special theodolites. blithely special requests aga
1	153	1	8464	680.14	cording to the final instructions. carefully fluffy asymptotes haggle carefully 
1	153	4	4695	539.86	atelets. dolphins haggle blithely carefully ironic deposits? express, final accounts wake about the requests. even deposits should use quickly. regular,
1	153	8	2916	685.52	ully express deposits boost daringly packages. furiously ironic accounts sleep slyly ironic instructions. special deposits integrate blithely. 
1	154	6	1682	160.31	refully except the sly, even requests. careful ideas haggle after the slyly regular foxes: slyly special packages at the slyly regular deposits wake carefully theod
1	155	6	2893	222.02	. bold packages are toward the silent pinto beans. quickly fin
1	155	7	977	751.45	quickly silent deposits doubt above the unusual instructions. special r
1	155	10	3029	413.02	the carefully ironic asymptotes. even, unusual accounts sleep furiously about the blithely regular ideas. quickly re
1	156	1	3043	252.66	lithely express, silent decoys. bold, special requests along the carefully even accounts
1	156	4	7228	994.19	odolites wake quickly slyly final dinos. requests cajole slyly along the instructions. furiously regular deposits cajole slyly blithely ironic instructions. instructions wake. blithely even pinto be
1	156	7	4755	453.69	e of the excuses. slyly even theodolites boost about the slyly final foxes? requests after the carefully regular platelets sleep above the furiously pending d
1	156	8	3191	9.87	ar instructions-- quickly special deposits wake fluffily about the blithely e
1	157	8	5414	369.44	ong the carefully bold ideas boost across the regular, ironic requests. ironic fo
1	158	6	8715	452.31	instructions along the ironic, final requests are fluffily regular deposits. regular deposits cajole carefully about the silent instructions
1	158	10	4251	431.90	etly special accounts boost carefully final multipliers. carefu
1	159	1	3717	790.87	y blithe dependencies. final accounts haggle furiously. even, special asymptotes
1	159	4	6085	171.40	ross the blithely special deposits are quickly carefully ironic Tiresias. quickly regular deposits was furiously. unusual accounts affix blithely about the regular deposits. asymptotes ab
1	159	7	3585	629.29	g platelets wake furiously slyly bold deposits? slyly regular accounts across the stealthily ironic accounts cajole along the special, ironic pearls. fluffily regular pinto
1	159	10	9200	356.66	ccording to the furiously final accounts. carefully fluffy foxes wake idly against the quickly final requests. evenly even pinto beans must have to are against the carefully regular de
1	160	1	2434	525.73	lithely. furiously silent theodolites after the ca
1	160	8	8324	999.93	ly final instructions. closely final deposits nag furiously alongside of the furiously dogged theodolites. blithely unusual theodolites are furi
1	161	6	679	893.72	 the fluffily final requests. ironic, pending epitaphs affix slyly. qui
1	161	8	9679	688.47	ns. blithely express requests sleep slyly foxes. blithely unusual ideas 
1	161	10	8421	394.05	cial ideas. ironic instructions eat blithely slyly special packages. furiously final packages alongside of the furiously final instructions boost carefully against the quickly 
1	162	1	2604	104.20	usly regular excuses. silent, even sheaves are according to the regular requests. packages grow blithely slyly regular accounts. ca
1	162	7	4381	824.36	as across the furiously ironic notornis print blithely alongside of the final, pending deposits. fluffily express deposits slee
1	163	4	9241	955.81	cial dolphins. furiously bold foxes could have to use. never sly accounts cajole fluffily about the unusual, special pinto beans. pending, even requests around the quickly special deposits use f
1	163	8	9676	573.48	ending accounts haggle blithely ironic, even packages. carefully pending packages wake carefully across the ruthlessly pending accounts! pinto beans wake. slyly final deposits boost slyly. fluffily
1	163	10	5323	920.75	sly even theodolites against the carefully bold packages wake final pinto beans. furiously pending deposits dazzle furiously. blithely exp
1	164	1	3245	814.67	brave accounts cajole according to the final platelets. furiously final dolphins across the furi
1	165	4	5534	717.83	 quickly regular deposits above the fluffily thin deposits haggle furiously against the quickly final depend
1	165	6	4424	943.82	ular requests. regular accounts cajole against the blithely ironic deposits. blithely even packages cajole. furiously final deposits cajole. thinly pending deposits hagg
1	165	10	6114	210.84	foxes. foxes haggle. dolphins use carefully according to the fluffily regular packages. blithely special accounts according to the slyly final frets breach blithely after the care
1	166	1	6713	631.58	 sleep carefully. quickly even deposits run carefully fluffily ironic orbits. ironic deposits wake furiously. close sheaves along the special packages sleep carefully special instr
1	166	7	6527	309.00	lly. dependencies haggle carefully at the slyly special packages. regular, final packages
1	167	4	4756	336.75	es are carefully along the carefully express tithes. furiously even deposits cajole slyly slyly regular deposits. bold excuses about the carefully ironic requests sleep blithely instructions
1	167	6	5789	524.27	 are furiously final, even dugouts. ironic, regular packages nag fu
1	167	8	4933	666.70	ular deposits among the even dolphins are quickly express accounts. final, ironic theodolites cajole closely. th
1	168	7	1281	771.90	, pending packages. ironic pinto beans use carefully. fluffily bold deposits
1	169	4	7691	476.19	slyly alongside of the warthogs. fluffily even instructions poach under the slyly pending packages. blithely silent deposits use across the fur
1	169	6	6731	713.35	 the quickly special excuses wake blithely alongside of the carefully silent accounts. regular dolphin
1	169	8	6589	947.03	gside of the quickly regular asymptotes. quickly even theodolites against the theodolites promise express requests. ironic accounts wake careful
1	169	10	6914	619.53	uickly along the dependencies. furiously pending notornis cajole at the carefully special attainments. carefully ironic packages impress slyly care
1	170	1	7516	581.65	 pinto beans. unusual ideas was fluffily. excuses cajole carefully final dependencies. platelets nag quickly according to the furiously ironic requests. carefully regular dependenci
1	170	7	6498	251.19	 fluffily regular accounts integrate. blithely even packages cajole fluffily. furiously ironic excuses haggle by the finally final requ
1	171	1	2311	864.96	s are along the blithely final deposits. regular asymptotes nag slyly against the requests. accounts cajole carefully carefully 
1	171	10	8561	22.69	y close ideas are quickly silently regular packages. even, silent requests wake against the slyly special dependencies; regular accounts sleep doggedly furiously final pinto beans. slyly unusual pac
1	172	1	3589	437.86	posits should have to boost furiously near the unusual ideas. final packages cajole blithely. carefully final deposits boost carefully. carefully special attainments boost quickly af
1	172	10	1661	687.13	y among the slyly even requests. ideas according to the slyly pending dinos print quickly slyly ironic foxes. pending, even excuses dazzle car
1	173	1	6162	877.84	es. slyly bold requests after the blithely regular dependencies cajole slyly even ideas. unusual deposits integrate about the final somas. 
1	173	4	2536	353.84	ons-- final, silent dependencies sleep across the special, special excuses. furiously even accounts must have to mold after the ironic accounts. reque
1	174	4	6795	143.48	regular theodolites. special accounts integrate across the carefully ironic Tiresias. blithely even platelets detect. foxes about t
1	175	4	8501	706.61	int above the instructions. furiously regular requests integrate blithely according to the instructions. slyly pending foxes are asymptotes. slyly ruthless accounts wake. r
1	175	6	5515	487.68	ages sleep against the Tiresias. slyly pending packages print slyly above the evenly ironic dolphins. furiously ironic packages use f
1	176	4	1783	861.63	g the carefully special platelets. dogged, ironic asymptotes wake requests. regular excus
1	176	6	3589	157.38	inal excuses. express deposits haggle carefully even deposits. carefully unusual requests haggle along the fluffily bold deposits. even, final requests affix. furi
1	176	7	7180	179.09	riously final requests. accounts doubt blithely regular somas. slyly even platelets are. theodolites across 
1	177	6	9872	252.42	sual platelets. bold foxes affix furiously. pending, pending accounts lose furiously. pending platelets along the unusual, even foxes wake regular, even theo
1	177	7	4349	63.36	osits sleep among the fluffily unusual instructions. ironic dolphins cajole. furiously bold deposits sleep carefully. even, unusual accounts
1	177	8	1239	44.75	requests use furiously regular, final requests. regular requests on the pending, ironic deposits use slyly among the excuses. carefully regular sheaves are.
1	178	6	6922	475.18	 regular patterns. fluffily express accounts about the furiously bold deposits cajole slyly about the furiously silent foxe
1	178	7	6836	864.93	y. ideas integrate regular pinto beans. special foxes wake above the slyly ironic asymptotes. quickly ironic ideas sleep. silent dependencies against the slyly bold packa
1	178	8	1919	362.26	 ironic dependencies. blithely regular packages detect fluffily special theodolites. regular instructions poach-- ironic deposits along the final requests 
1	179	7	4776	8.39	sly special pinto beans. pinto beans cajole. carefully unusual ideas around the silent accounts are blithely carefully ev
1	179	8	2710	277.15	d the frets. pending packages doze quickly across the furiously regular deposits. pending, even deposits impress ironic ideas. quickly regular r
1	179	10	6956	444.38	g the furiously careful excuses haggle quickly thinly special Tiresias. furiously express foxes after the quickly regular deposits sleep ironic packages
1	180	1	2467	440.25	 instructions affix. regular packages cajole quickly. carefully express asymptotes use furiously around the pendin
1	180	8	5899	864.83	hin the carefully furious pinto beans. furiously ironic pinto beans use slyly above the even instructio
1	180	10	1108	934.59	hinly after the regular, unusual asymptotes! carefully regular theodolites sublate. regular, ironic deposits against the regular pinto beans nag ca
1	183	4	30	875.44	slyly. furiously regular instructions cajole slyly about the pending, final theodolites. blithely final deposits cajole fluffily alo
1	185	6	1475	538.58	unts hinder slyly. quickly express ideas sleep carefully 
1	186	7	1095	252.84	. carefully regular pinto beans according to the blithely close asymptotes haggle carefully special requests. packages cajole up the furi
1	187	8	3183	362.75	t the bold platelets. fluffily express platelets cajole fluffily along the always bold requests. blith
1	189	10	1305	392.50	packages. regular, unusual accounts lose furiously fluffily regular platelets. requests sleep carefully dependenc
1	190	1	535	621.53	unts must have to haggle; slyly ironic accounts affix slyly alongside of the carefully even accounts. furious deposits haggle quietly among the packages. blithely 
1	191	4	597	126.96	ly final accounts should have to boost above the doggedly express pinto beans. blithely regular packages cajole furiously bold requests. fluf
1	192	4	2656	916.16	uickly. slyly bold ideas affix special, close theodolites. ironic, pending requests use carefully. blithely regular 
1	192	6	8305	861.23	s against the carefully regular foxes haggle fluffily across the pending accounts. blithely final packages sleep after the furiously ironic theodolites. quickly bold r
1	193	4	6184	335.98	 quickly bold deposits cajole furiously ruthless courts. carefully
1	193	6	385	571.71	ons. slyly ironic deposits wake furiously ironic, unus
1	193	7	9791	478.52	quests. carefully even requests use regular excuses. pending accounts are. furiously even pinto beans haggle furi
1	194	6	377	430.21	efully instead of the special ideas. fluffily unusual asymptotes cajole blithely after the regular ideas. final accounts along the silent ex
1	194	7	5294	913.46	indle fluffily despite the carefully silent instructions. furiously regular hockey players cajole slyly unusual accounts. furiously regular realms cajole furiously according to the e
1	194	8	7890	79.40	ctions sleep. carefully unusual theodolites should wake furiously across the deposits-- furiously bold excuses boost furiously carefully slow accounts. boldly final accounts grow. regular excuse
1	195	6	9985	20.39	efully among the fluffily even accounts! requests are slyly ag
1	195	7	2947	271.39	yly regular requests cajole carefully. carefully fina
1	195	8	319	102.58	ts. ironic foxes wake carefully slyly special pinto beans. blithely silent excuses hinder blithely quietly regular accounts. quickly careful foxes maintain slyly above the slyly express fo
1	196	7	3843	859.90	l platelets use blithely alongside of the enticingly final deposits. fluffily final requests boost furiously ag
1	196	8	2515	966.01	final theodolites. fluffily even deposits are against the
1	196	10	1068	928.25	 cajole about the blithely regular ideas. final ideas hin
1	197	1	8950	897.33	ideas. requests wake above the blithely unusual deposits. slyly regular 
1	197	8	9678	753.88	ously. slyly stealthy requests use alongside of the express, unusual packages. final deposits wake. carefully unusual theodolites cajole slyly about the regular foxes. slyly iron
1	197	10	7598	845.51	lets according to the regular deposits wake furiously about the carefully daring theodolites. blithely express dolphins poach after th
1	198	1	8410	166.93	kages. blithely final theodolites dazzle fluffily. accounts boost furiously. furiously unu
1	198	10	6493	673.99	y express excuses use blithely among the pending accounts. stealthy ide
1	199	1	8199	46.52	oost slyly. ironic platelets sleep blithely about the slyly silent foxes. furiously even pl
1	199	10	9343	79.70	ending accounts nag across the instructions. carefully express packages over the blithely even pac
1	200	1	3120	776.41	ntly final packages kindle furiously blithely ironic accounts. carefully final packages according to the carefully 
1	200	4	331	466.07	 slyly even requests. fluffily final packages boost carefully express instructions. slyly regular forges are blithely unusual, regular 
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
1	Supplier#000000001       	 N kD4on9OM Ipw3,gf0JBoQDd7tgrzrddZ	17	27-918-335-1736	5755.94	each slyly above the careful
4	Supplier#000000004       	Bk7ah4CK8SYQTepEmvMkkgMwg	15	25-843-787-7479	4641.08	riously even requests above the exp
6	Supplier#000000006       	tQxuVm7s7CnK	14	24-696-997-4969	1365.79	final accounts. regular dolphins use against the furiously ironic decoys. 
7	Supplier#000000007       	s,4TicNGB4uO6PaSqNBUq	23	33-990-965-2201	6820.35	s unwind silently furiously regular courts. final requests are deposits. requests wake quietly blit
8	Supplier#000000008       	9Sq4bBH2FQEmaFOocY45sRTxo6yuoG	17	27-498-742-3860	7627.85	al pinto beans. asymptotes haggl
10	Supplier#000000010       	Saygah3gYWMp72i PY	24	34-852-489-8585	3891.91	ing waters. regular requests ar
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

