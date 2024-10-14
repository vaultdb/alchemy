--
-- PostgreSQL database dump
--

-- Dumped from database version 14.9 (Ubuntu 14.9-0ubuntu0.22.04.1)
-- Dumped by pg_dump version 14.9 (Ubuntu 14.9-0ubuntu0.22.04.1)

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
    table_name character varying(255),
    col_name character varying(255),
    min integer,
    max integer,
    domain_size integer
);


ALTER TABLE public.bit_packing OWNER TO vaultdb;

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
    CONSTRAINT s_phone_domain CHECK (((("substring"((s_phone)::text, 1, 2))::integer = (s_nationkey + 10)) AND ("substring"((s_phone)::text, 3, 1) = '-'::text) AND ((("substring"((s_phone)::text, 4, 3))::integer >= 100) AND (("substring"((s_phone)::text, 4, 3))::integer <= 999)) AND ("substring"((s_phone)::text, 7, 1) = '-'::text) AND ((("substring"((s_phone)::text, 8, 3))::integer >= 100) AND (("substring"((s_phone)::text, 8, 3))::integer <= 999)) AND ("substring"((s_phone)::text, 11, 1) = '-'::text) AND ((("substring"((s_phone)::text, 12, 4))::integer >= 1000) AND (("substring"((s_phone)::text, 12, 4))::integer <= 9999))))
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
-- Data for Name: bit_packing; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.bit_packing (table_name, col_name, min, max, domain_size) FROM stdin;
\.


--
-- Data for Name: customer; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.customer (c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment) FROM stdin;
3	Customer#000000003	MG9kdTD2WBHm	1	11-719-748-3364	7498.12	AUTOMOBILE	 deposits eat slyly ironic, even instructions. express foxes detect slyly. blithely even accounts abov
4	Customer#000000004	XxVSJsLAGtn	4	14-128-190-5944	2866.83	MACHINERY 	 requests. final, regular ideas sleep final accou
5	Customer#000000005	KvpyuHCplrB84WgAiGV6sYpZq7Tj	3	13-750-942-6364	794.47	HOUSEHOLD 	n accounts will have to unwind. foxes cajole accor
9	Customer#000000009	xKiAFTjUsCuxfeleNqefumTrjS	8	18-338-906-3675	8324.07	FURNITURE 	r theodolites according to the requests wake thinly excuses: pending requests haggle furiousl
10	Customer#000000010	6LrEaV6KR6PLVcgl2ArL Q3rqzLzcT1 v2	5	15-741-346-9870	2753.54	HOUSEHOLD 	es regular deposits haggle. fur
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
36422	154112	6628	1	27	31484.97	0.01	0.07	N	O	1997-05-02	1997-05-01	1997-05-25	TAKE BACK RETURN         	TRUCK     	into beans. blithely regular reque
36422	146555	6556	2	37	59257.35	0.04	0.01	N	O	1997-06-03	1997-04-07	1997-06-17	COLLECT COD              	SHIP      	g deposits are about the as
36422	145767	3310	3	42	76135.92	0.07	0.07	N	O	1997-05-18	1997-05-09	1997-05-28	NONE                     	MAIL      	ly. regular packages sleep. 
36422	90688	689	4	14	23501.52	0.00	0.02	N	O	1997-06-27	1997-05-12	1997-07-23	DELIVER IN PERSON        	SHIP      	eposits. even, regular packages snoo
36422	183949	8986	5	31	63021.14	0.09	0.04	N	O	1997-03-29	1997-05-25	1997-04-19	DELIVER IN PERSON        	REG AIR   	 the quickly stea
36422	17008	2011	6	9	8325.00	0.06	0.05	N	O	1997-06-10	1997-05-14	1997-06-30	TAKE BACK RETURN         	REG AIR   	ajole. fluff
36422	48167	5680	7	27	30109.32	0.06	0.04	N	O	1997-06-18	1997-05-14	1997-07-15	NONE                     	AIR       	kages. blithely final excuses
164711	82527	52	1	34	51323.68	0.09	0.05	A	F	1992-04-27	1992-07-23	1992-05-22	NONE                     	AIR       	kages. regular i
164711	26184	1189	2	15	16652.70	0.01	0.00	A	F	1992-05-18	1992-06-12	1992-06-11	TAKE BACK RETURN         	TRUCK     	raids doubt aga
164711	67393	9900	3	38	51694.82	0.00	0.00	R	F	1992-07-17	1992-06-11	1992-08-12	DELIVER IN PERSON        	MAIL      	eodolites cajol
164711	65941	8448	4	38	72463.72	0.09	0.04	A	F	1992-07-16	1992-07-05	1992-08-06	TAKE BACK RETURN         	FOB       	y bold courts; blithely
164711	69788	9789	5	34	59764.52	0.04	0.06	R	F	1992-05-09	1992-05-26	1992-05-24	DELIVER IN PERSON        	RAIL      	ccounts detect among the carefu
164711	122878	5391	6	33	62728.71	0.03	0.07	A	F	1992-06-12	1992-06-25	1992-07-03	NONE                     	TRUCK     	cuses use afte
224167	120715	5740	1	7	12149.97	0.03	0.00	N	O	1996-06-10	1996-07-27	1996-06-22	NONE                     	REG AIR   	unts. furiously sile
224167	55920	931	2	11	20635.12	0.02	0.01	N	O	1996-07-05	1996-07-11	1996-07-17	NONE                     	AIR       	 use blithely
224167	45636	8141	3	18	28469.34	0.07	0.05	N	O	1996-05-26	1996-07-22	1996-05-28	TAKE BACK RETURN         	RAIL      	s. slyly unusual pinto beans boos
224167	199327	1847	4	18	25673.76	0.02	0.00	N	O	1996-08-25	1996-07-12	1996-09-19	TAKE BACK RETURN         	AIR       	ng to the fluffily
287619	42361	7370	1	11	14336.96	0.05	0.00	N	O	1997-04-14	1997-02-11	1997-05-04	NONE                     	MAIL      	ss the pending platelets wake a
385825	161438	6471	1	47	70473.21	0.01	0.07	N	O	1996-01-17	1995-12-27	1996-02-06	DELIVER IN PERSON        	RAIL      	rses. unusual, special foxes use. f
385825	74506	7014	2	2	2961.00	0.00	0.06	N	O	1995-12-17	1996-01-15	1996-01-11	NONE                     	FOB       	eas according to the regular accounts 
385825	10379	5382	3	40	51574.80	0.09	0.03	N	O	1996-02-03	1996-01-06	1996-02-24	COLLECT COD              	SHIP      	 carefully pending accounts. furiousl
385825	32887	5391	4	28	50956.64	0.03	0.07	N	O	1995-11-05	1995-12-17	1995-11-29	NONE                     	FOB       	s hang slyly unusual accounts. re
385825	118088	3111	5	22	24333.76	0.10	0.00	N	O	1995-12-11	1996-01-06	1995-12-28	NONE                     	MAIL      	 warhorses hinder slyly a
385825	88806	3823	6	16	28716.80	0.05	0.08	N	O	1995-12-25	1995-12-27	1996-01-05	TAKE BACK RETURN         	REG AIR   	iously final theodolites boost 
385825	148780	8781	7	28	51205.84	0.08	0.00	N	O	1996-02-15	1996-01-10	1996-02-22	TAKE BACK RETURN         	MAIL      	arefully even 
816323	149483	4512	1	12	18389.76	0.04	0.08	N	O	1996-05-21	1996-03-10	1996-06-13	DELIVER IN PERSON        	REG AIR   	ronic platelets. ironic, ironic
816323	153105	3106	2	20	23162.00	0.03	0.06	N	O	1996-03-06	1996-04-17	1996-04-02	NONE                     	RAIL      	 the unusual theodolites. carefully bold d
816323	33413	923	3	40	53856.40	0.05	0.08	N	O	1996-03-13	1996-03-29	1996-04-07	COLLECT COD              	TRUCK     	lly pending packages wake f
816323	161159	3676	4	25	30503.75	0.06	0.06	N	O	1996-04-14	1996-03-12	1996-05-01	NONE                     	TRUCK     	thely ironic foxes cajole
816323	182701	2702	5	36	64213.20	0.04	0.03	N	O	1996-03-13	1996-02-29	1996-03-31	TAKE BACK RETURN         	RAIL      	arefully express accounts 
816323	42686	7695	6	21	34202.28	0.03	0.07	N	O	1996-03-14	1996-02-24	1996-03-24	TAKE BACK RETURN         	SHIP      	r the carefully final somas; 
816323	16221	3725	7	19	21607.18	0.04	0.04	N	O	1996-02-05	1996-03-04	1996-02-15	COLLECT COD              	REG AIR   	round the carefully
859108	142807	7836	1	40	73992.00	0.05	0.00	N	O	1996-03-31	1996-03-25	1996-04-21	COLLECT COD              	REG AIR   	ully bold theodolites according to 
859108	146837	4380	2	33	62166.39	0.07	0.07	N	O	1996-04-03	1996-03-25	1996-04-07	NONE                     	FOB       	regular frays. furiou
859108	89079	9080	3	3	3204.21	0.07	0.06	N	O	1996-05-15	1996-05-12	1996-05-20	DELIVER IN PERSON        	MAIL      	final platelets are. furiously expre
883557	187324	2361	1	40	56452.80	0.10	0.06	N	O	1998-06-18	1998-05-27	1998-07-16	COLLECT COD              	MAIL      	lyly along the fina
895172	12359	2360	1	27	34326.45	0.09	0.03	N	O	1996-03-27	1996-01-17	1996-04-08	COLLECT COD              	RAIL      	jole blithely pending dependencies.
895172	117617	5151	2	39	63749.79	0.01	0.06	N	O	1996-03-11	1996-01-24	1996-04-03	DELIVER IN PERSON        	RAIL      	. doggedly bol
895172	172208	7243	3	29	37125.80	0.03	0.02	N	O	1996-02-09	1996-02-03	1996-02-13	DELIVER IN PERSON        	FOB       	 theodolites. theodolites wake daringly
895172	87784	7785	4	30	53153.40	0.00	0.00	N	O	1995-12-29	1996-01-07	1996-01-19	DELIVER IN PERSON        	TRUCK     	ructions haggle b
895172	185046	2601	5	19	21489.76	0.05	0.02	N	O	1996-01-27	1996-02-26	1996-02-26	COLLECT COD              	SHIP      	e furiously special packa
895172	63141	3142	6	3	3312.42	0.10	0.07	N	O	1996-01-20	1996-02-14	1996-01-22	DELIVER IN PERSON        	TRUCK     	oss the even, final packages. d
895172	197028	9548	7	22	24750.44	0.10	0.07	N	O	1996-02-29	1996-01-20	1996-03-27	DELIVER IN PERSON        	SHIP      	ans across the carefully final dep
905633	20814	8321	1	15	26022.15	0.08	0.06	N	O	1995-09-07	1995-09-01	1995-10-04	NONE                     	RAIL      	usual asymptotes boost among t
905633	10167	2669	2	46	49549.36	0.10	0.04	N	O	1995-10-11	1995-10-01	1995-10-17	DELIVER IN PERSON        	SHIP      	fully ironic 
905633	64302	6809	3	39	49385.70	0.07	0.04	N	O	1995-07-24	1995-09-25	1995-08-07	NONE                     	REG AIR   	ly pending acc
905633	111751	1752	4	43	75798.25	0.05	0.02	N	O	1995-08-08	1995-08-23	1995-08-16	DELIVER IN PERSON        	SHIP      	e furiously reg
905633	183408	8445	5	3	4474.20	0.00	0.04	N	O	1995-08-25	1995-08-14	1995-09-06	TAKE BACK RETURN         	FOB       	blithely regular packages nag car
905633	123317	854	6	38	50931.78	0.08	0.01	N	O	1995-08-17	1995-08-08	1995-08-22	DELIVER IN PERSON        	REG AIR   	refully dogged foxes. slyl
905633	172297	2298	7	12	16431.48	0.05	0.05	N	O	1995-08-03	1995-08-24	1995-08-08	NONE                     	MAIL      	usly special ideas affix after t
916775	56922	4438	1	35	65762.20	0.06	0.02	N	O	1996-08-01	1996-07-22	1996-08-29	COLLECT COD              	RAIL      	nal requests.
916775	76682	9190	2	40	66347.20	0.04	0.05	N	O	1996-08-24	1996-06-02	1996-09-11	DELIVER IN PERSON        	RAIL      	furiously 
916775	178845	8846	3	26	50019.84	0.00	0.05	N	O	1996-05-25	1996-06-17	1996-06-08	TAKE BACK RETURN         	RAIL      	lar theodolites eat blithely even de
916775	44007	1520	4	29	27579.00	0.04	0.04	N	O	1996-08-10	1996-06-19	1996-09-02	DELIVER IN PERSON        	SHIP      	s sleep along the bold deposits.
916775	11158	6161	5	3	3207.45	0.10	0.08	N	O	1996-05-10	1996-06-30	1996-05-12	NONE                     	RAIL      	its. quickly ironic instructions can
916775	120334	7871	6	36	48755.88	0.00	0.02	N	O	1996-07-16	1996-05-29	1996-08-15	COLLECT COD              	MAIL      	hockey players 
916775	43250	5755	7	15	17898.75	0.09	0.05	N	O	1996-07-16	1996-07-01	1996-08-13	COLLECT COD              	RAIL      	ic pearls. furiously ironic ideas h
1192231	103367	3368	1	29	39740.44	0.01	0.07	N	O	1996-06-06	1996-08-24	1996-06-15	NONE                     	MAIL      	across the blithe
1192231	86763	1780	2	9	15747.84	0.01	0.05	N	O	1996-09-25	1996-07-22	1996-10-09	TAKE BACK RETURN         	TRUCK     	careful accounts. even ideas haggle aga
1192231	10325	326	3	15	18529.80	0.04	0.03	N	O	1996-09-25	1996-07-31	1996-09-30	COLLECT COD              	REG AIR   	 the furiously furious deposits wake fur
1192231	117347	4881	4	2	2728.68	0.08	0.04	N	O	1996-07-10	1996-07-24	1996-07-12	NONE                     	MAIL      	osits. fluffily regular courts sle
1192231	43478	8487	5	26	36958.22	0.01	0.03	N	O	1996-06-19	1996-08-18	1996-06-23	NONE                     	TRUCK     	fully regular instructions
1192231	120368	369	6	19	26378.84	0.02	0.04	N	O	1996-07-14	1996-08-12	1996-07-16	DELIVER IN PERSON        	AIR       	bold packages haggle furiously against th
1226497	101559	4070	1	32	49937.60	0.08	0.06	R	F	1993-11-02	1993-12-04	1993-11-22	NONE                     	MAIL      	. final, unusual deposits cajole slowly. i
1226497	98547	6075	2	27	41729.58	0.06	0.01	R	F	1994-01-09	1993-12-10	1994-01-15	TAKE BACK RETURN         	RAIL      	e quickly along th
1490087	104957	4958	1	22	43162.90	0.02	0.06	N	O	1996-09-16	1996-09-30	1996-10-13	COLLECT COD              	RAIL      	nag; carefully i
1490087	6676	9177	2	45	71220.15	0.02	0.02	N	O	1996-08-26	1996-08-10	1996-09-14	DELIVER IN PERSON        	AIR       	c, ironic requests haggle blithely
1490087	194371	9410	3	14	20515.18	0.08	0.05	N	O	1996-09-09	1996-08-13	1996-10-09	TAKE BACK RETURN         	SHIP      	cajole fluffily about the instruc
1490087	180589	8144	4	30	50087.40	0.04	0.07	N	O	1996-07-24	1996-09-16	1996-08-23	TAKE BACK RETURN         	SHIP      	old dolphins. quickly ironic accoun
1490087	155076	107	5	36	40718.52	0.05	0.04	N	O	1996-10-31	1996-08-09	1996-11-26	TAKE BACK RETURN         	REG AIR   	al theodolites haggle.
1590469	21835	1836	1	12	21081.96	0.05	0.02	N	O	1997-07-06	1997-05-14	1997-07-07	NONE                     	AIR       	, pending accounts are furiously even foxe
1590469	180776	3295	2	11	20424.47	0.03	0.02	N	O	1997-04-02	1997-05-23	1997-04-12	COLLECT COD              	REG AIR   	etect carefully. blithely ironic dolphins 
1590469	100142	7673	3	30	34264.20	0.00	0.03	N	O	1997-05-08	1997-04-23	1997-06-06	NONE                     	SHIP      	he thinly even in
1755398	175059	7577	1	1	1134.05	0.05	0.05	N	O	1997-08-04	1997-09-09	1997-08-05	TAKE BACK RETURN         	RAIL      	carefully fi
1774689	75407	422	1	10	13824.00	0.01	0.01	R	F	1993-07-12	1993-09-26	1993-07-30	NONE                     	MAIL      	s. slyly express frays are furio
1944711	66583	4102	1	37	57334.46	0.03	0.00	R	F	1995-05-15	1995-07-29	1995-05-20	TAKE BACK RETURN         	MAIL      	ular forges. slyly ev
1944711	48820	3829	2	15	26532.30	0.03	0.02	N	O	1995-08-06	1995-06-20	1995-09-04	TAKE BACK RETURN         	MAIL      	into beans according to the slyl
1944711	103892	8913	3	13	24646.57	0.10	0.04	N	O	1995-07-16	1995-06-28	1995-07-31	DELIVER IN PERSON        	MAIL      	, final instructions.
1953441	3713	8714	1	48	77602.08	0.10	0.08	N	O	1996-07-08	1996-08-09	1996-07-11	DELIVER IN PERSON        	REG AIR   	of the slyly pending theodolites. f
1953441	191708	4228	2	2	3599.40	0.10	0.04	N	O	1996-07-24	1996-08-07	1996-08-16	DELIVER IN PERSON        	RAIL      	uriously express dolphins. blithely clos
1953441	31856	9366	3	2	3575.70	0.02	0.05	N	O	1996-09-09	1996-07-21	1996-09-14	DELIVER IN PERSON        	RAIL      	furiously silent packages. blithely ir
1953441	1186	8687	4	7	7610.26	0.06	0.00	N	O	1996-08-02	1996-08-07	1996-08-11	TAKE BACK RETURN         	TRUCK     	, bold decoys belie
1953441	122322	9859	5	21	28230.72	0.05	0.07	N	O	1996-07-15	1996-08-07	1996-08-05	NONE                     	MAIL      	 packages snooze quickly. final ide
1953441	64462	6969	6	31	44220.26	0.02	0.07	N	O	1996-07-14	1996-08-21	1996-07-16	NONE                     	FOB       	uffily final deposits. furious
1953441	11771	4273	7	44	74041.88	0.07	0.03	N	O	1996-07-23	1996-07-26	1996-07-26	COLLECT COD              	AIR       	l braids. quickly express deposits cajole a
1978756	168332	8333	1	45	63014.85	0.04	0.00	N	O	1996-09-30	1996-09-22	1996-10-06	NONE                     	SHIP      	ing to the carefully even de
1978756	143978	1521	2	49	99076.53	0.10	0.01	N	O	1996-10-20	1996-09-12	1996-10-23	TAKE BACK RETURN         	FOB       	 deposits wake fluffi
1978756	26601	1606	3	35	53466.00	0.10	0.05	N	O	1996-10-03	1996-10-29	1996-10-21	DELIVER IN PERSON        	REG AIR   	 packages haggle s
1978756	81795	1796	4	25	44419.75	0.07	0.03	N	O	1996-12-01	1996-10-21	1996-12-21	COLLECT COD              	REG AIR   	pinto beans cajole carefully enti
1978756	124255	6768	5	22	28143.50	0.06	0.02	N	O	1996-12-01	1996-09-18	1996-12-20	NONE                     	FOB       	n theodolites 
1978756	153258	5774	6	35	45893.75	0.04	0.00	N	O	1996-09-19	1996-09-14	1996-10-06	COLLECT COD              	MAIL      	packages. final, even excuses maint
2126688	26118	1123	1	26	27146.86	0.03	0.00	R	F	1993-10-10	1993-11-02	1993-11-02	TAKE BACK RETURN         	MAIL      	tions cajol
2126688	150126	127	2	20	23522.40	0.02	0.07	R	F	1993-08-24	1993-10-31	1993-08-28	DELIVER IN PERSON        	TRUCK     	 final, regular pac
2459619	85054	71	1	42	43640.10	0.01	0.07	N	O	1996-01-22	1996-02-02	1996-01-27	COLLECT COD              	FOB       	 deposits cajole. fluffily spec
2630562	108745	1256	1	37	64888.38	0.07	0.00	R	F	1993-09-19	1993-07-30	1993-10-18	TAKE BACK RETURN         	REG AIR   	telets are around th
2630562	63400	8413	2	47	64079.80	0.03	0.02	R	F	1993-09-29	1993-07-29	1993-10-24	TAKE BACK RETURN         	FOB       	refully. ironic, pen
2630562	162921	7954	3	8	15871.36	0.05	0.06	A	F	1993-09-25	1993-09-17	1993-10-14	DELIVER IN PERSON        	MAIL      	lets after the bold, pending
2630562	199149	4188	4	45	56166.30	0.02	0.06	R	F	1993-07-14	1993-09-24	1993-08-12	COLLECT COD              	AIR       	 regular instructions. blithe
2630562	83201	3202	5	26	30789.20	0.08	0.08	A	F	1993-09-25	1993-08-29	1993-10-12	DELIVER IN PERSON        	REG AIR   	ously unusual accou
2630562	155844	3390	6	23	43696.32	0.07	0.00	R	F	1993-08-07	1993-08-31	1993-08-19	COLLECT COD              	REG AIR   	 unusual realms. fur
2630562	68414	5933	7	39	53913.99	0.00	0.03	A	F	1993-07-15	1993-09-10	1993-07-27	NONE                     	SHIP      	its. blithely regular pinto beans kindle c
2765152	19827	2329	1	5	8734.10	0.10	0.06	N	O	1996-10-27	1996-10-18	1996-11-07	COLLECT COD              	AIR       	s boost fluffily after the instructions; p
2917345	12883	2884	1	19	34121.72	0.06	0.01	N	O	1995-12-04	1995-12-26	1995-12-26	DELIVER IN PERSON        	MAIL      	l frets. caref
2917345	114072	4073	2	10	10860.70	0.05	0.01	N	O	1995-10-27	1995-12-22	1995-11-10	TAKE BACK RETURN         	AIR       	ss deposits. express,
2917345	729	5730	3	6	9778.32	0.07	0.07	N	O	1995-10-21	1995-12-14	1995-11-05	NONE                     	SHIP      	sual instructions. fluf
2986913	85787	5788	1	3	5318.34	0.09	0.07	A	F	1994-08-12	1994-08-30	1994-09-10	DELIVER IN PERSON        	TRUCK     	lithely bold courts. even 
2986913	8369	8370	2	29	37043.44	0.06	0.01	R	F	1994-07-19	1994-08-23	1994-08-16	DELIVER IN PERSON        	REG AIR   	 regular pinto beans haggle across the furi
3069221	73817	8832	1	13	23280.53	0.03	0.05	N	O	1997-02-27	1997-01-11	1997-03-13	NONE                     	RAIL      	 final deposits sleep spe
3069221	34628	7132	2	2	3125.24	0.04	0.05	N	O	1997-01-31	1996-12-22	1997-02-07	NONE                     	SHIP      	inal pinto beans wake furiously
3251169	77605	7606	1	43	68051.80	0.04	0.00	N	O	1996-01-22	1996-03-01	1996-01-30	DELIVER IN PERSON        	MAIL      	eas. blithely
3251169	106088	6089	2	25	27352.00	0.05	0.00	N	O	1996-03-22	1996-04-05	1996-04-13	TAKE BACK RETURN         	TRUCK     	s cajole id
3251169	11860	6863	3	23	40752.78	0.02	0.04	N	O	1996-02-28	1996-03-15	1996-03-08	COLLECT COD              	TRUCK     	 carefully bold requests. ironic deposits c
3251169	124206	4207	4	48	59049.60	0.00	0.07	N	O	1996-03-01	1996-02-25	1996-03-25	TAKE BACK RETURN         	FOB       	es. quickly even 
3251169	129315	4340	5	23	30919.13	0.09	0.04	N	O	1996-02-23	1996-03-13	1996-03-05	DELIVER IN PERSON        	SHIP      	nding theodolites sleep quickly a
3354726	94998	2526	1	12	23915.88	0.02	0.05	N	O	1998-04-22	1998-03-25	1998-05-21	COLLECT COD              	TRUCK     	refully blithely ironic 
3354726	23666	8671	2	6	9537.96	0.00	0.01	N	O	1998-04-13	1998-03-11	1998-05-07	DELIVER IN PERSON        	SHIP      	ptotes detect blithel
3354726	142776	319	3	39	70932.03	0.00	0.08	N	O	1998-03-13	1998-02-13	1998-04-01	DELIVER IN PERSON        	FOB       	e along the fluffily fi
3354726	31938	1939	4	8	14959.44	0.00	0.07	N	O	1998-05-04	1998-03-06	1998-05-08	TAKE BACK RETURN         	FOB       	sauternes use. iro
3354726	165382	5383	5	23	33289.74	0.06	0.00	N	O	1998-01-27	1998-03-27	1998-02-08	TAKE BACK RETURN         	REG AIR   	s. sly deposits ab
3421092	98162	672	1	36	41765.76	0.04	0.03	N	O	1998-07-29	1998-06-20	1998-08-10	COLLECT COD              	REG AIR   	sly bold asymptotes
3421092	7783	5284	2	38	64249.64	0.01	0.03	N	O	1998-07-18	1998-07-14	1998-07-31	NONE                     	TRUCK     	dly alongside of the spe
3421092	3024	5525	3	42	38934.84	0.08	0.00	N	O	1998-07-21	1998-08-02	1998-07-27	NONE                     	MAIL      	ly according to the busy, special 
3421092	16851	6852	4	5	8839.25	0.07	0.05	N	O	1998-08-29	1998-06-18	1998-09-03	COLLECT COD              	AIR       	ar pinto bea
3421092	4548	9549	5	21	30503.34	0.07	0.07	N	O	1998-08-27	1998-08-13	1998-09-07	NONE                     	REG AIR   	ubt fluffily bold excuses. 
3487745	195005	5006	1	42	46200.00	0.02	0.04	N	O	1997-01-23	1996-11-06	1997-02-04	TAKE BACK RETURN         	TRUCK     	s use. final dolphins should have
3487745	37824	328	2	28	49330.96	0.02	0.02	N	O	1996-12-19	1996-12-25	1996-12-27	NONE                     	TRUCK     	eposits. pending asymptotes according to th
3487745	92415	4925	3	27	38000.07	0.10	0.08	N	O	1996-11-20	1996-12-15	1996-11-27	COLLECT COD              	FOB       	xes. blithely unusual accoun
3487745	17849	2852	4	35	61839.40	0.07	0.04	N	O	1996-12-03	1996-12-02	1996-12-23	DELIVER IN PERSON        	AIR       	nal, idle 
3580034	72757	2758	1	3	5189.25	0.08	0.03	A	F	1992-10-22	1992-08-26	1992-11-09	TAKE BACK RETURN         	MAIL      	 instruction
3580034	13497	3498	2	6	8462.94	0.02	0.04	R	F	1992-10-07	1992-09-20	1992-10-21	NONE                     	MAIL      	ins nag according to
3580034	32403	2404	3	17	22701.80	0.08	0.00	A	F	1992-08-07	1992-08-22	1992-08-12	TAKE BACK RETURN         	AIR       	arefully express ideas. carefully spe
3580034	183536	6055	4	45	72878.85	0.06	0.01	A	F	1992-08-21	1992-10-03	1992-08-23	TAKE BACK RETURN         	AIR       	y regular acco
3580034	84564	9581	5	31	48005.36	0.02	0.03	R	F	1992-08-10	1992-08-15	1992-09-08	COLLECT COD              	RAIL      	olites against 
3580034	53570	3571	6	33	50277.81	0.02	0.05	A	F	1992-08-29	1992-09-17	1992-09-02	DELIVER IN PERSON        	RAIL      	special warhorses. furiously ironic courts 
3580034	148252	8253	7	20	26005.00	0.00	0.00	R	F	1992-09-09	1992-08-29	1992-10-09	TAKE BACK RETURN         	FOB       	 silent realms. special, silent fo
3683623	87587	5112	1	25	39364.50	0.08	0.06	N	O	1997-01-13	1997-02-25	1997-01-24	DELIVER IN PERSON        	TRUCK     	ding packages. furiously ev
3683623	62217	7230	2	3	3537.63	0.00	0.00	N	O	1997-02-25	1997-01-13	1997-03-06	COLLECT COD              	MAIL      	nts nag slyly? f
3683623	74889	2411	3	33	61508.04	0.03	0.08	N	O	1997-01-08	1997-02-04	1997-01-14	NONE                     	SHIP      	lly regular packages
3683623	54352	6858	4	35	45722.25	0.03	0.05	N	O	1997-02-01	1997-02-27	1997-02-26	COLLECT COD              	REG AIR   	dencies cajole. quickly special foxe
3916288	88086	8087	1	15	16111.20	0.02	0.07	N	O	1997-11-11	1997-09-07	1997-12-08	COLLECT COD              	TRUCK     	e blithely express platelets; requests 
3916288	58604	3615	2	8	12500.80	0.01	0.01	N	O	1997-10-21	1997-08-30	1997-11-12	DELIVER IN PERSON        	TRUCK     	s. blithely unusual packages above the car
3916288	154756	2302	3	30	54322.50	0.10	0.00	N	O	1997-07-23	1997-09-21	1997-08-15	DELIVER IN PERSON        	TRUCK     	al instruct
3916288	178978	4013	4	6	12341.82	0.09	0.01	N	O	1997-11-17	1997-10-10	1997-12-01	DELIVER IN PERSON        	REG AIR   	uests. packages nag furiously ironic t
3916288	77571	2586	5	7	10839.99	0.05	0.00	N	O	1997-10-15	1997-08-19	1997-11-02	DELIVER IN PERSON        	MAIL      	ronic accounts are. blithely regu
3942656	177966	7967	1	10	20439.60	0.05	0.03	R	F	1992-10-22	1992-10-01	1992-11-04	DELIVER IN PERSON        	REG AIR   	quests. furiously unusual
3942656	70612	8134	2	45	71217.45	0.07	0.05	R	F	1992-10-15	1992-09-25	1992-10-25	TAKE BACK RETURN         	RAIL      	s integrate ironic pinto bea
3942656	173519	3520	3	41	65292.91	0.09	0.01	A	F	1992-10-26	1992-10-08	1992-11-21	NONE                     	RAIL      	sts impress furiously. carefully silent
3942656	120888	8425	4	50	95444.00	0.01	0.00	A	F	1992-12-14	1992-10-18	1992-12-16	DELIVER IN PERSON        	REG AIR   	ctions. regular dep
3942656	148816	8817	5	50	93240.50	0.10	0.00	A	F	1992-09-05	1992-09-27	1992-09-15	DELIVER IN PERSON        	TRUCK     	ns detect; slyly final theodolites betw
3951331	135093	7607	1	18	20305.62	0.01	0.05	N	O	1998-02-08	1998-02-07	1998-03-10	NONE                     	SHIP      	 furiously from the slyly care
3951331	93639	3640	2	27	44081.01	0.08	0.07	N	O	1998-02-21	1998-01-14	1998-03-16	COLLECT COD              	TRUCK     	lyly express platelets ha
3951331	183141	3142	3	10	12241.40	0.08	0.03	N	O	1998-01-21	1998-01-11	1998-02-03	DELIVER IN PERSON        	TRUCK     	usly speci
4141668	22659	7664	1	23	36377.95	0.03	0.02	A	F	1995-05-22	1995-05-19	1995-06-08	COLLECT COD              	AIR       	ly even pearls boost fur
4141668	27438	4945	2	22	30039.46	0.06	0.01	R	F	1995-05-17	1995-05-29	1995-06-10	COLLECT COD              	SHIP      	arefully special
4141668	137826	7827	3	41	76416.62	0.08	0.03	R	F	1995-04-04	1995-06-13	1995-04-05	COLLECT COD              	TRUCK     	counts nod bl
4141668	34541	4542	4	22	32461.88	0.10	0.08	A	F	1995-05-20	1995-05-26	1995-06-05	NONE                     	SHIP      	n patterns. final, unusual 
4243142	3062	563	1	37	35707.22	0.05	0.04	N	O	1996-02-05	1996-02-17	1996-02-13	NONE                     	FOB       	d dependencies snooze fluffily sly
4243142	48863	8864	2	20	36237.20	0.02	0.05	N	O	1996-01-09	1995-12-29	1996-01-23	TAKE BACK RETURN         	MAIL      	 slyly. furiously express id
4243142	33399	3400	3	44	58625.16	0.05	0.07	N	O	1996-03-19	1996-02-02	1996-04-17	DELIVER IN PERSON        	AIR       	ng the quickly fi
4320612	141370	6399	1	24	33872.88	0.06	0.00	A	F	1992-12-03	1992-11-22	1992-12-14	TAKE BACK RETURN         	MAIL      	uickly bold theodolites
4320612	193219	3220	2	17	22307.57	0.01	0.02	A	F	1992-11-23	1992-10-21	1992-11-28	DELIVER IN PERSON        	SHIP      	 regular asymptotes! id
4320612	19301	1803	3	48	58574.40	0.07	0.01	A	F	1992-10-22	1992-10-22	1992-11-13	NONE                     	SHIP      	 boost carefully
4320612	176957	4509	4	37	75256.15	0.06	0.04	A	F	1992-12-24	1992-12-13	1993-01-14	DELIVER IN PERSON        	MAIL      	counts. furiously pending pinto beans doub
4407621	34109	6613	1	5	5215.50	0.01	0.03	N	O	1995-11-30	1995-11-20	1995-12-27	TAKE BACK RETURN         	RAIL      	cross the slyly silent accounts.
4407621	55841	852	2	10	17968.40	0.10	0.07	N	O	1995-12-15	1995-11-04	1996-01-02	TAKE BACK RETURN         	FOB       	 dependencies cajole
4407621	106161	3692	3	17	19841.72	0.03	0.04	N	O	1996-01-07	1995-11-24	1996-02-05	NONE                     	AIR       	ilent packages. bold requests cajole 
4407621	38424	8425	4	40	54496.80	0.09	0.03	N	O	1995-12-26	1995-12-17	1996-01-07	NONE                     	AIR       	elets! fluffily ironic packages cajole 
4616224	72748	270	1	12	20648.88	0.03	0.08	N	O	1997-03-19	1997-03-10	1997-03-21	TAKE BACK RETURN         	SHIP      	ounts integrate fluffily 
4960614	5975	976	1	10	18809.70	0.01	0.04	N	O	1997-12-24	1997-11-08	1997-12-29	COLLECT COD              	SHIP      	kages. finally ironic 
4960614	14062	9065	2	29	28305.74	0.03	0.03	N	O	1997-10-20	1997-11-11	1997-11-11	TAKE BACK RETURN         	SHIP      	osits at the blithely iron
4960614	128497	3522	3	21	32035.29	0.07	0.02	N	O	1997-12-15	1997-11-25	1997-12-27	NONE                     	RAIL      	luffily pending accou
4960614	32629	139	4	8	12492.96	0.00	0.05	N	O	1998-01-19	1997-12-13	1998-02-10	NONE                     	TRUCK     	ggle blithel
5453440	14306	4307	1	14	17084.20	0.00	0.05	N	O	1997-09-03	1997-09-28	1997-09-08	NONE                     	TRUCK     	uests. finally bold requests are always. s
5612065	56099	8605	1	22	23211.98	0.04	0.08	N	O	1997-11-29	1997-11-24	1997-12-19	DELIVER IN PERSON        	TRUCK     	eodolites nag furiously brave pac
5612065	48883	8884	2	27	49460.76	0.08	0.05	N	O	1997-10-11	1997-11-28	1997-10-24	DELIVER IN PERSON        	FOB       	inal, final ideas hagg
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
36422	10	O	287837.16	1997-03-04	3-MEDIUM       	Clerk#000000532	0	e evenly final dependencies. regular, bold pinto beans 	1997
164711	4	F	311722.87	1992-04-26	3-MEDIUM       	Clerk#000000361	0	accounts among the regular, 	1992
224167	5	O	85170.68	1996-05-08	5-LOW          	Clerk#000000657	0	s along the carefully special pinto beans cajole carefully even sentim	1996
287619	5	O	13620.11	1996-12-26	5-LOW          	Clerk#000000854	0	r requests must sleep carefully furiously regular tithes? slyly unusual reque	1996
385825	4	O	277493.04	1995-11-01	2-HIGH         	Clerk#000000465	0	e even foxes. regularly even instructions boost carefully along	1995
816323	10	O	249097.39	1996-01-23	5-LOW          	Clerk#000000693	0	ts boost furiously pinto beans. regular deposits nag among the express 	1996
859108	10	O	135312.87	1996-02-20	3-MEDIUM       	Clerk#000000573	0	 pinto beans. final, unusual deposits use. final	1996
883557	10	O	53855.97	1998-03-30	2-HIGH         	Clerk#000000758	0	ses are carefully. slyly regular asympto	1998
895172	10	O	236806.93	1995-12-04	1-URGENT       	Clerk#000000673	0	cajole quickly slyly exp	1995
905633	5	O	261338.66	1995-07-05	4-NOT SPECIFIED	Clerk#000000254	0	arefully ironic attainments cajole slyly furiou	1995
916775	10	O	279937.32	1996-04-26	1-URGENT       	Clerk#000000032	0	express instructions. quickly unusual accounts cajole-- carefully i	1996
1192231	4	O	143971.54	1996-06-03	1-URGENT       	Clerk#000000978	0	osits haggle carefully. carefully final grouches are. blithely si	1996
1226497	4	F	88317.19	1993-10-04	1-URGENT       	Clerk#000000154	0	al theodolites. quickly ruthless accounts lose blithely furiously re	1993
1490087	10	O	227526.57	1996-07-10	4-NOT SPECIFIED	Clerk#000000953	0	lly regular deposits. furiously regular instructions thrash	1996
1590469	4	O	75928.49	1997-03-07	2-HIGH         	Clerk#000000217	0	y pending pinto beans. carefully express theodolites cajole 	1997
1755398	4	O	1131.20	1997-06-12	5-LOW          	Clerk#000000765	0	s. slyly ironic packa	1997
1774689	10	F	13822.61	1993-07-08	3-MEDIUM       	Clerk#000000702	0	 deposits are fluffily quick	1993
1944711	4	P	104934.65	1995-05-06	1-URGENT       	Clerk#000000580	0	deas wake. slyly silent packa	1995
1953441	4	O	235621.83	1996-06-06	2-HIGH         	Clerk#000000288	0	iers are. quickly final instructi	1996
1978756	4	O	314671.82	1996-08-02	3-MEDIUM       	Clerk#000000661	0	es. blithely special theodolites cajole slyly carefully regular asymp	1996
2126688	10	F	50998.03	1993-08-15	4-NOT SPECIFIED	Clerk#000000207	0	r, silent accounts haggle o	1993
2459619	4	O	46227.94	1996-01-03	5-LOW          	Clerk#000000917	0	iously ironic pinto beans sleep furiously across the quickly unusual notornis.	1996
2630562	5	F	324835.83	1993-06-27	4-NOT SPECIFIED	Clerk#000000966	0	carefully final deposits. quickly silent pains are among the careful	1993
2765152	4	O	8332.33	1996-08-11	4-NOT SPECIFIED	Clerk#000000618	0	onic packages. slyly express requests	1996
2917345	10	O	52546.37	1995-10-01	4-NOT SPECIFIED	Clerk#000000983	0	ly regular accounts. special packages haggle furio	1995
2986913	4	F	40347.48	1994-06-10	3-MEDIUM       	Clerk#000000633	0	, bold accounts use slyly even excuses. pending req	1994
3069221	10	O	26861.45	1996-10-29	3-MEDIUM       	Clerk#000000005	0	ess requests. carefully si	1996
3251169	4	O	225294.26	1996-01-06	5-LOW          	Clerk#000000310	0	arefully final packages boost against the furiously	1996
3354726	10	O	158148.30	1998-01-10	3-MEDIUM       	Clerk#000000427	0	ggle blithely ironic notornis. carefully bold deposits use iro	1998
3421092	4	O	181618.75	1998-05-16	3-MEDIUM       	Clerk#000000762	0	refully even pinto beans. furiously	1998
3487745	10	O	193145.38	1996-09-26	1-URGENT       	Clerk#000000230	0	uctions. pending dependencies can boost regular attainments-- 	1996
3580034	10	F	229817.03	1992-07-09	4-NOT SPECIFIED	Clerk#000000118	0	 slyly pending accounts after the sly	1992
3683623	4	O	152929.80	1996-11-29	3-MEDIUM       	Clerk#000000652	0	lve. furiously silent fret	1996
3916288	10	O	99925.33	1997-07-20	4-NOT SPECIFIED	Clerk#000000272	0	t integrate. carefully regular theodolites sleep c	1997
3942656	10	F	327960.68	1992-08-16	3-MEDIUM       	Clerk#000000954	0	jole from the slyly bold deposits. quickly expres	1992
3951331	4	O	76100.95	1997-11-23	3-MEDIUM       	Clerk#000000341	0	ly express platelets above the carefully	1997
4141668	10	F	168477.12	1995-03-24	1-URGENT       	Clerk#000000849	0	 dependencies. blithely pending accounts solve carefully furiously ex	1995
4243142	10	O	132159.26	1995-11-20	3-MEDIUM       	Clerk#000000004	0	ironic foxes must haggle blithely 	1995
4320612	4	F	182956.01	1992-09-20	2-HIGH         	Clerk#000000994	0	 silently ironic theodolites are around the r	1992
4407621	10	O	93717.95	1995-09-26	5-LOW          	Clerk#000000354	0	ly pending notornis sleep furiously about the i	1995
4616224	10	O	21631.76	1997-01-22	5-LOW          	Clerk#000000818	0	into beans haggle c	1997
4960614	4	O	91152.97	1997-10-03	4-NOT SPECIFIED	Clerk#000000706	0	ithely final dependencies cajole blithely. express instructions are slyly a	1997
5453440	4	O	17938.41	1997-07-12	2-HIGH         	Clerk#000000360	0	s sleep idly foxes. final accounts x-ray slyly. slyly even platelets h	1997
5612065	4	O	71845.26	1997-09-07	3-MEDIUM       	Clerk#000000999	0	quests boost fluffily. blithely f	1997
\.


--
-- Data for Name: part; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.part (p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment) FROM stdin;
1	goldenrod lavender spring chocolate lace	Manufacturer#1           	Brand#13  	PROMO BURNISHED COPPER	7	JUMBO PKG 	901.00	ly. slyly ironi
2	blush thistle blue yellow saddle	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	1	LG CASE   	902.00	lar accounts amo
4	cornflower chocolate smoke green pink	Manufacturer#3           	Brand#34  	SMALL PLATED BRASS	14	MED DRUM  	904.00	p furiously r
8	misty lace thistle snow royal	Manufacturer#4           	Brand#44  	PROMO BURNISHED TIN	41	LG DRUM   	908.00	eposi
2501	cream wheat burlywood spring ivory	Manufacturer#4           	Brand#45  	PROMO BRUSHED STEEL	45	SM PKG    	1403.50	equests haggle car
2502	plum powder steel smoke lemon	Manufacturer#1           	Brand#15  	PROMO POLISHED COPPER	28	SM DRUM   	1404.50	ingly final d
2504	lace magenta royal blanched orchid	Manufacturer#3           	Brand#31  	LARGE BURNISHED BRASS	18	MED JAR   	1406.50	ughout the furiousl
2508	light pale antique wheat lace	Manufacturer#1           	Brand#13  	ECONOMY BRUSHED TIN	44	SM CASE   	1410.50	ng the regular theodol
5001	peach seashell grey blush chocolate	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED TIN	21	JUMBO PACK	906.00	c waters. fluffily pe
5002	firebrick thistle cyan rose maroon	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED NICKEL	25	LG CASE   	907.00	ckages sleep i
5004	orchid salmon magenta seashell sky	Manufacturer#2           	Brand#22  	SMALL BRUSHED BRASS	20	WRAP JAR  	909.00	fully r
5008	powder dodger floral cyan almond	Manufacturer#1           	Brand#12  	LARGE ANODIZED TIN	30	JUMBO PACK	913.00	e the fluff
7501	thistle midnight lemon khaki powder	Manufacturer#2           	Brand#23  	PROMO POLISHED STEEL	50	LG PACK   	1408.50	lly ironic somas impr
7502	rosy peru puff tomato misty	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	29	WRAP BOX  	1409.50	bout 
7504	deep turquoise moccasin peach ghost	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED NICKEL	33	LG PKG    	1411.50	haggle furio
7508	thistle dodger tomato orange navy	Manufacturer#2           	Brand#24  	STANDARD ANODIZED BRASS	42	LG CAN    	1415.50	nto be
10001	saddle honeydew red plum indian	Manufacturer#4           	Brand#42  	SMALL ANODIZED STEEL	34	SM BAG    	911.00	sly regular theodolite
10002	cyan grey saddle olive pale	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	3	SM PACK   	912.00	sly. slyly 
10004	burlywood dim moccasin forest yellow	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED COPPER	4	MED JAR   	914.00	e along the final p
10008	light lavender lime spring burnished	Manufacturer#2           	Brand#24  	ECONOMY PLATED TIN	16	MED CASE  	918.00	counts. carefully spec
12498	papaya smoke honeydew white lawn	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED NICKEL	22	SM BOX    	1410.49	o beans. asymptotes
12499	medium blanched coral beige light	Manufacturer#4           	Brand#41  	MEDIUM BURNISHED COPPER	2	LG BAG    	1411.49	frays sleep caref
12501	metallic rosy orchid deep indian	Manufacturer#4           	Brand#45  	SMALL ANODIZED BRASS	37	SM PACK   	1413.50	 sleep c
12505	thistle seashell indian frosted cornflower	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED TIN	4	LG DRUM   	1417.50	hins. accounts affix
14999	aquamarine sky dark dim brown	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED TIN	27	JUMBO PKG 	1913.99	g after the
15000	snow navy pale drab cornsilk	Manufacturer#2           	Brand#22  	SMALL BRUSHED STEEL	24	MED CAN   	915.00	 regular requests 
15002	blanched ghost cream azure tomato	Manufacturer#5           	Brand#55  	LARGE BURNISHED STEEL	26	MED CAN   	917.00	waters along the
15006	azure bisque olive pink chiffon	Manufacturer#2           	Brand#22  	PROMO BRUSHED STEEL	11	LG BAG    	921.00	arefully 
17500	ivory lemon burnished lime red	Manufacturer#1           	Brand#12  	PROMO POLISHED NICKEL	28	JUMBO CASE	1417.50	wake pen
17501	snow blanched lawn yellow hot	Manufacturer#5           	Brand#51  	SMALL BRUSHED COPPER	45	WRAP BOX  	1418.50	l accou
17503	magenta blanched ghost bisque lemon	Manufacturer#5           	Brand#54  	MEDIUM ANODIZED COPPER	6	SM PACK   	1420.50	ronic instructions na
17507	thistle spring orange khaki magenta	Manufacturer#2           	Brand#21  	SMALL BURNISHED NICKEL	50	JUMBO BAG 	1424.50	ng blithely. thin, f
20001	metallic maroon tomato turquoise grey	Manufacturer#3           	Brand#31  	PROMO POLISHED NICKEL	2	JUMBO PKG 	921.00	press instructions wa
20002	blanched firebrick royal purple chartreuse	Manufacturer#4           	Brand#44  	PROMO BURNISHED STEEL	5	MED BOX   	922.00	 depos
20004	drab chiffon salmon violet lavender	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED COPPER	9	WRAP CASE 	924.00	. slyly exp
20008	salmon burlywood frosted grey drab	Manufacturer#2           	Brand#21  	STANDARD ANODIZED NICKEL	14	SM JAR    	928.00	ithely final 
22495	lavender purple beige violet snow	Manufacturer#1           	Brand#14  	SMALL BURNISHED NICKEL	37	LG PACK   	1417.49	above the fu
22496	cornflower dim black peach blanched	Manufacturer#2           	Brand#23  	PROMO POLISHED NICKEL	20	MED CASE  	1418.49	. regular excuses 
22498	plum almond sky burnished light	Manufacturer#1           	Brand#13  	STANDARD POLISHED STEEL	15	SM BAG    	1420.49	jole carefull
22502	chocolate bisque dim violet moccasin	Manufacturer#4           	Brand#43  	LARGE BURNISHED TIN	30	LG JAR    	1424.50	gs. slyly fluff
24997	magenta orchid lawn lemon lace	Manufacturer#1           	Brand#13  	LARGE PLATED TIN	10	JUMBO CAN 	1921.99	fluffily ironic fox
24998	slate ghost wheat sky cornsilk	Manufacturer#2           	Brand#21  	ECONOMY PLATED COPPER	13	JUMBO BAG 	1922.99	egular, regular pint
25000	ghost purple yellow sandy chocolate	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED COPPER	17	WRAP JAR  	925.00	final ac
25004	deep goldenrod cream medium blue	Manufacturer#5           	Brand#51  	PROMO POLISHED COPPER	25	MED PACK  	929.00	ubt furiously depos
27499	cornflower thistle sandy lemon forest	Manufacturer#2           	Brand#25  	MEDIUM POLISHED STEEL	16	JUMBO PACK	1426.49	ly across the 
27500	tan frosted white burnished royal	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED TIN	8	JUMBO CAN 	1427.50	 final
27502	coral black dodger lemon blush	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED COPPER	15	LG CASE   	1429.50	c accounts. pending 
27506	steel papaya firebrick seashell lime	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED NICKEL	39	MED DRUM  	1433.50	beans. quickly silen
30001	hot firebrick gainsboro goldenrod chartreuse	Manufacturer#1           	Brand#14  	ECONOMY BURNISHED NICKEL	43	WRAP DRUM 	931.00	ickly at the care
30002	burlywood blue brown mint ghost	Manufacturer#1           	Brand#14  	STANDARD ANODIZED TIN	48	SM DRUM   	932.00	ickly pen
30004	powder papaya linen azure cornsilk	Manufacturer#3           	Brand#35  	MEDIUM POLISHED NICKEL	36	WRAP JAR  	934.00	nts against the
30008	almond forest mint midnight violet	Manufacturer#1           	Brand#14  	SMALL PLATED NICKEL	40	WRAP BOX  	938.00	y final packages
32492	tan lawn goldenrod chiffon medium	Manufacturer#3           	Brand#34  	STANDARD POLISHED NICKEL	36	LG DRUM   	1424.49	ely. bli
32493	pink steel hot forest drab	Manufacturer#1           	Brand#12  	STANDARD ANODIZED TIN	4	WRAP CAN  	1425.49	lyly even 
32495	pink sandy orchid cornflower peach	Manufacturer#4           	Brand#45  	SMALL ANODIZED STEEL	11	LG PKG    	1427.49	uests boost slyly 
32499	seashell white blue black wheat	Manufacturer#3           	Brand#33  	PROMO POLISHED COPPER	45	SM CAN    	1431.49	ggle care
34995	snow black linen frosted lawn	Manufacturer#3           	Brand#35  	PROMO ANODIZED TIN	13	MED DRUM  	1929.99	 at the account
34996	moccasin antique puff seashell blue	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	39	LG BOX    	1930.99	 across 
34998	lemon frosted magenta mint rosy	Manufacturer#3           	Brand#34  	STANDARD ANODIZED NICKEL	31	MED BAG   	1932.99	ourts wake blithely 
35002	sky blanched orchid lavender bisque	Manufacturer#4           	Brand#43  	SMALL BRUSHED STEEL	43	LG BAG    	937.00	le fluffily: slyly 
37498	rose burlywood gainsboro midnight linen	Manufacturer#4           	Brand#45  	LARGE BURNISHED TIN	37	MED JAR   	1435.49	 fluffy deposi
37499	orchid maroon metallic red sandy	Manufacturer#5           	Brand#54  	LARGE ANODIZED COPPER	41	MED DRUM  	1436.49	packages. spec
37501	papaya navajo lemon hot blush	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED BRASS	9	WRAP CASE 	1438.50	sits. bl
37505	steel floral tomato burnished chartreuse	Manufacturer#5           	Brand#51  	PROMO ANODIZED TIN	2	MED BOX   	1442.50	he slyly
40001	cream steel purple royal goldenrod	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED COPPER	27	JUMBO PKG 	941.00	lly slow depende
40002	almond cyan forest midnight khaki	Manufacturer#3           	Brand#34  	ECONOMY POLISHED TIN	32	WRAP BOX  	942.00	ng to 
40004	midnight magenta metallic drab bisque	Manufacturer#3           	Brand#34  	LARGE BRUSHED COPPER	29	LG BOX    	944.00	uthlessly al
40008	navy red puff lawn olive	Manufacturer#3           	Brand#32  	SMALL BRUSHED BRASS	41	WRAP BAG  	948.00	 carefully fina
42489	dim chiffon cream lawn steel	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED COPPER	5	MED PACK  	1431.48	ully furious
42490	aquamarine magenta indian frosted goldenrod	Manufacturer#1           	Brand#15  	LARGE POLISHED BRASS	29	WRAP BAG  	1432.49	egula
42492	midnight dodger olive sky sandy	Manufacturer#4           	Brand#43  	STANDARD ANODIZED NICKEL	15	WRAP BAG  	1434.49	 cajol
42496	blanched misty linen maroon pale	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	38	LG BAG    	1438.49	ns. blithely pen
44993	cornflower wheat thistle floral navy	Manufacturer#1           	Brand#15  	SMALL ANODIZED NICKEL	30	JUMBO DRUM	1937.99	eans cajole fluffily 
44994	navy blue chiffon lemon sandy	Manufacturer#1           	Brand#15  	SMALL BURNISHED TIN	8	LG BAG    	1938.99	ic foxe
44996	deep papaya black rose powder	Manufacturer#5           	Brand#55  	MEDIUM BRUSHED BRASS	50	SM JAR    	1940.99	hely iron
45000	cornflower grey midnight magenta black	Manufacturer#1           	Brand#11  	STANDARD BURNISHED COPPER	48	WRAP BAG  	945.00	sits.
47497	firebrick pink goldenrod grey pale	Manufacturer#1           	Brand#15  	PROMO BURNISHED NICKEL	18	JUMBO CASE	1444.49	 carefully bold pl
47498	chocolate blush saddle cornflower hot	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	19	LG PKG    	1445.49	y. spe
47500	coral cream orchid firebrick sandy	Manufacturer#3           	Brand#32  	PROMO ANODIZED TIN	48	SM JAR    	1447.50	lar dependencies. fl
47504	orange lawn snow medium lace	Manufacturer#1           	Brand#11  	LARGE BURNISHED BRASS	22	JUMBO DRUM	1451.50	ss deposits. s
50001	drab burnished lavender maroon cornflower	Manufacturer#5           	Brand#51  	ECONOMY POLISHED STEEL	39	JUMBO PKG 	951.00	luffily unusual the
50002	lawn dim firebrick smoke snow	Manufacturer#2           	Brand#25  	PROMO ANODIZED BRASS	41	JUMBO JAR 	952.00	e furiously 
50004	saddle black maroon magenta dodger	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED BRASS	21	MED CAN   	954.00	s. unusual
50008	aquamarine brown blue sienna coral	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED COPPER	20	MED BOX   	958.00	ven pinto
52486	mint metallic floral pale turquoise	Manufacturer#4           	Brand#45  	ECONOMY POLISHED NICKEL	11	JUMBO CASE	1438.48	ggle against the bus
52487	red thistle blanched aquamarine metallic	Manufacturer#2           	Brand#22  	ECONOMY POLISHED TIN	6	LG PKG    	1439.48	ias. special theodoli
52489	turquoise navy bisque papaya peach	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	6	JUMBO PKG 	1441.48	atelets. furiou
52493	deep khaki peach orchid sandy	Manufacturer#1           	Brand#11  	STANDARD ANODIZED NICKEL	31	LG PACK   	1445.49	oxes wake fu
54991	lavender lemon pink orchid olive	Manufacturer#5           	Brand#54  	SMALL BRUSHED COPPER	31	WRAP PKG  	1945.99	ts integra
54992	rose bisque magenta turquoise floral	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED COPPER	32	SM CAN    	1946.99	idly ironi
54994	snow burlywood smoke salmon orange	Manufacturer#3           	Brand#32  	STANDARD BURNISHED NICKEL	6	LG PKG    	1948.99	ng deposits. speci
54998	lawn metallic orange pink grey	Manufacturer#5           	Brand#55  	LARGE BRUSHED STEEL	40	LG PKG    	1952.99	al instructions
57496	khaki goldenrod slate seashell salmon	Manufacturer#1           	Brand#12  	LARGE PLATED NICKEL	24	LG CASE   	1453.49	unts s
57497	goldenrod khaki sandy rosy thistle	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED NICKEL	43	SM PACK   	1454.49	eques
57499	maroon smoke honeydew brown sienna	Manufacturer#4           	Brand#44  	SMALL BRUSHED STEEL	32	SM PACK   	1456.49	hely regular accoun
57503	goldenrod floral spring orchid powder	Manufacturer#4           	Brand#45  	PROMO ANODIZED BRASS	18	JUMBO JAR 	1460.50	tions cajole 
60001	firebrick hot rose aquamarine navy	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED COPPER	34	SM PKG    	961.00	ss accounts
60002	azure blanched khaki dark aquamarine	Manufacturer#3           	Brand#35  	LARGE BRUSHED STEEL	13	JUMBO BAG 	962.00	ely express 
60004	lace metallic yellow goldenrod bisque	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED BRASS	7	SM PKG    	964.00	above
60008	blue cornflower tomato slate yellow	Manufacturer#4           	Brand#45  	PROMO BRUSHED NICKEL	48	MED DRUM  	968.00	he quickly even 
62483	beige puff firebrick papaya grey	Manufacturer#3           	Brand#31  	STANDARD POLISHED BRASS	25	WRAP DRUM 	1445.48	 packages
62484	burlywood indian royal ivory metallic	Manufacturer#2           	Brand#25  	PROMO BURNISHED BRASS	32	WRAP JAR  	1446.48	 furiously silent acco
62486	wheat brown salmon deep lace	Manufacturer#4           	Brand#45  	LARGE ANODIZED NICKEL	2	SM PKG    	1448.48	s. quickly silent
62490	puff white bisque saddle linen	Manufacturer#5           	Brand#52  	SMALL BURNISHED STEEL	28	JUMBO JAR 	1452.49	kages cajole 
64989	firebrick antique purple white olive	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED COPPER	42	MED JAR   	1953.98	blithely daringly 
64990	cyan violet khaki ivory deep	Manufacturer#5           	Brand#51  	LARGE POLISHED COPPER	48	SM BAG    	1954.99	le slyly spec
64992	burlywood cornflower khaki bisque pink	Manufacturer#4           	Brand#41  	MEDIUM PLATED NICKEL	5	LG JAR    	1956.99	s. slyly i
64996	indian drab deep goldenrod turquoise	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED NICKEL	38	MED JAR   	1960.99	fully according 
67495	dark deep chartreuse moccasin mint	Manufacturer#4           	Brand#41  	MEDIUM BURNISHED BRASS	40	MED DRUM  	1462.49	, regular dependenc
67496	black midnight goldenrod orchid cornflower	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	29	JUMBO CAN 	1463.49	jole quic
67498	lace peru moccasin midnight azure	Manufacturer#2           	Brand#23  	LARGE ANODIZED BRASS	7	WRAP PKG  	1465.49	ts. car
67502	ivory dim smoke violet azure	Manufacturer#2           	Brand#21  	ECONOMY BURNISHED STEEL	16	SM BAG    	1469.50	ions.
70001	sky green dim hot chartreuse	Manufacturer#1           	Brand#12  	PROMO PLATED TIN	31	MED CAN   	971.00	mes ironic
70002	cornflower gainsboro papaya green snow	Manufacturer#4           	Brand#43  	SMALL BRUSHED STEEL	45	SM DRUM   	972.00	ular dependenc
70004	misty yellow drab floral sandy	Manufacturer#4           	Brand#42  	LARGE ANODIZED COPPER	30	WRAP JAR  	974.00	sly. q
70008	goldenrod sandy orchid tan firebrick	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED STEEL	14	WRAP PACK 	978.00	ly regular theodo
72480	tan ghost almond mint drab	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	9	MED BAG   	1452.48	express pl
72481	aquamarine chocolate drab misty brown	Manufacturer#1           	Brand#11  	LARGE PLATED BRASS	25	LG BAG    	1453.48	counts cajole slow e
72483	blue blanched misty plum burnished	Manufacturer#1           	Brand#13  	PROMO BURNISHED STEEL	36	LG CASE   	1455.48	e furiously busily eve
72487	spring metallic puff frosted goldenrod	Manufacturer#5           	Brand#51  	LARGE ANODIZED STEEL	16	JUMBO CAN 	1459.48	against 
74987	slate ivory dark maroon steel	Manufacturer#3           	Brand#35  	STANDARD BRUSHED COPPER	44	LG CAN    	1961.98	ns. bli
74988	lime steel sandy rosy rose	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED COPPER	41	WRAP CAN  	1962.98	ly regu
74990	peru plum dodger gainsboro blush	Manufacturer#5           	Brand#51  	SMALL BURNISHED BRASS	17	MED BOX   	1964.99	nstructio
74994	chiffon firebrick dark spring hot	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED BRASS	19	JUMBO BAG 	1968.99	slyly final f
77494	lawn almond peru pink khaki	Manufacturer#4           	Brand#41  	STANDARD PLATED TIN	25	WRAP JAR  	1471.49	s the slyly 
77495	rosy brown goldenrod steel navy	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	44	WRAP BAG  	1472.49	regular 
77497	magenta tan cornflower blue aquamarine	Manufacturer#5           	Brand#53  	STANDARD PLATED TIN	33	JUMBO PACK	1474.49	lithely regul
77501	snow dim beige cornflower spring	Manufacturer#2           	Brand#21  	LARGE POLISHED BRASS	46	SM PACK   	1478.50	wake 
80001	olive blue peach coral honeydew	Manufacturer#3           	Brand#31  	ECONOMY POLISHED COPPER	38	WRAP PACK 	981.00	ts haggle. s
80002	sandy gainsboro sky linen snow	Manufacturer#2           	Brand#24  	ECONOMY BRUSHED NICKEL	24	JUMBO BAG 	982.00	cajol
80004	lemon papaya maroon seashell peru	Manufacturer#2           	Brand#22  	LARGE ANODIZED BRASS	4	SM CASE   	984.00	nstructi
80008	thistle ghost sienna metallic chocolate	Manufacturer#4           	Brand#45  	STANDARD ANODIZED COPPER	45	WRAP CASE 	988.00	ld requ
82477	antique spring orchid steel snow	Manufacturer#3           	Brand#32  	STANDARD POLISHED NICKEL	40	MED CAN   	1459.47	carefully final as
82478	tan light mint sky dark	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED BRASS	43	LG PKG    	1460.47	kly idle inst
82480	ivory cream green drab cornflower	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED COPPER	17	JUMBO JAR 	1462.48	 of the 
82484	blanched turquoise aquamarine puff saddle	Manufacturer#3           	Brand#31  	LARGE BURNISHED NICKEL	32	WRAP BOX  	1466.48	refully 
84985	gainsboro cream maroon chiffon black	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	48	JUMBO PKG 	1969.98	uld have to impres
84986	navy magenta azure floral salmon	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED STEEL	3	LG DRUM   	1970.98	 ideas. bold i
84988	cornsilk lemon rose chocolate gainsboro	Manufacturer#4           	Brand#44  	ECONOMY PLATED TIN	21	LG BOX    	1972.98	e furiously 
84992	floral pink honeydew tan moccasin	Manufacturer#2           	Brand#23  	LARGE ANODIZED TIN	47	SM CASE   	1976.99	ng, f
87493	violet floral cyan grey steel	Manufacturer#5           	Brand#55  	LARGE BURNISHED STEEL	4	LG BAG    	1480.49	lyly. furious
87494	bisque royal drab azure slate	Manufacturer#1           	Brand#14  	PROMO PLATED NICKEL	15	SM PACK   	1481.49	y carefully specia
87496	goldenrod snow dim burnished honeydew	Manufacturer#5           	Brand#51  	PROMO BURNISHED COPPER	48	MED CAN   	1483.49	st haggle slyly? furio
87500	turquoise peru dodger pink linen	Manufacturer#2           	Brand#24  	STANDARD ANODIZED BRASS	21	WRAP CAN  	1487.50	ly furiously even fr
90001	snow cornflower cream floral lavender	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED COPPER	18	MED PACK  	991.00	ons boost
90002	salmon black cornflower indian lime	Manufacturer#3           	Brand#34  	STANDARD ANODIZED BRASS	31	MED CASE  	992.00	s the
90004	navy saddle maroon violet blush	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	46	MED BOX   	994.00	 furiously. furiousl
90008	brown sandy coral lemon metallic	Manufacturer#2           	Brand#23  	STANDARD POLISHED TIN	35	WRAP PACK 	998.00	ccounts. boldly reg
92474	magenta lime black dodger firebrick	Manufacturer#4           	Brand#42  	LARGE BURNISHED BRASS	42	SM CASE   	1466.47	s. quickly final 
92475	floral orange royal sienna linen	Manufacturer#2           	Brand#22  	STANDARD POLISHED BRASS	25	WRAP PKG  	1467.47	ully express attain
92477	blanched forest peach chiffon thistle	Manufacturer#4           	Brand#44  	LARGE BRUSHED STEEL	33	JUMBO BOX 	1469.47	 carefully busy i
92481	almond cornsilk firebrick sienna sandy	Manufacturer#2           	Brand#22  	SMALL POLISHED COPPER	32	JUMBO CAN 	1473.48	slyly unusual pack
94983	smoke cream olive floral saddle	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	27	SM CASE   	1977.98	 deposi
94984	papaya orchid peru brown smoke	Manufacturer#1           	Brand#15  	ECONOMY PLATED COPPER	40	MED CASE  	1978.98	ms promise slyly!
94986	slate goldenrod spring lavender green	Manufacturer#3           	Brand#35  	PROMO PLATED COPPER	38	JUMBO CAN 	1980.98	into beans
94990	beige slate plum sienna moccasin	Manufacturer#5           	Brand#53  	PROMO ANODIZED STEEL	14	LG CAN    	1984.99	. silently regular de
97492	linen frosted floral purple steel	Manufacturer#2           	Brand#24  	PROMO BRUSHED TIN	19	WRAP BOX  	1489.49	quickly bold dolp
97493	turquoise mint moccasin forest azure	Manufacturer#2           	Brand#21  	PROMO BURNISHED COPPER	3	MED CASE  	1490.49	 requests. ac
97495	gainsboro blush ghost frosted forest	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED BRASS	7	LG CASE   	1492.49	yly. bli
97499	navy coral slate peach seashell	Manufacturer#1           	Brand#12  	STANDARD ANODIZED TIN	13	LG PKG    	1496.49	ge furi
100001	seashell cyan plum purple honeydew	Manufacturer#3           	Brand#35  	STANDARD BRUSHED TIN	37	JUMBO CASE	1001.00	ronic dependencies d
100002	steel moccasin forest cornflower brown	Manufacturer#3           	Brand#34  	STANDARD ANODIZED NICKEL	11	WRAP CAN  	1002.00	 quickly pending 
100004	snow blanched khaki indian azure	Manufacturer#4           	Brand#42  	SMALL POLISHED TIN	29	SM CASE   	1004.00	sly. blithely
100008	spring powder sienna purple lime	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED BRASS	19	SM PKG    	1008.00	ts. furious
102471	lime linen salmon wheat cornsilk	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED BRASS	33	MED DRUM  	1473.47	luffily pending accou
102472	red goldenrod drab orange peru	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED COPPER	14	JUMBO CAN 	1474.47	ickly fina
102474	orange midnight navajo dim seashell	Manufacturer#4           	Brand#42  	STANDARD BURNISHED COPPER	47	JUMBO PKG 	1476.47	refully final
102478	green dark dodger sandy blanched	Manufacturer#1           	Brand#11  	STANDARD POLISHED NICKEL	23	WRAP PACK 	1480.47	 are in place of 
104981	snow sienna lime medium navy	Manufacturer#2           	Brand#25  	SMALL POLISHED STEEL	1	MED CAN   	1985.98	es haggle. ironic, f
104982	moccasin mint orange metallic drab	Manufacturer#4           	Brand#42  	MEDIUM POLISHED COPPER	7	SM JAR    	1986.98	ular d
104984	medium pink khaki cornflower azure	Manufacturer#5           	Brand#51  	SMALL ANODIZED BRASS	38	LG BOX    	1988.98	ts boost dependencies
104988	black blue cyan plum dim	Manufacturer#3           	Brand#33  	MEDIUM BURNISHED BRASS	37	SM CASE   	1992.98	e slyly ev
107491	misty green papaya salmon red	Manufacturer#5           	Brand#53  	PROMO BURNISHED COPPER	37	LG CASE   	1498.49	carefully 
107492	ghost mint white blanched dim	Manufacturer#3           	Brand#34  	ECONOMY PLATED TIN	32	MED CAN   	1499.49	lets. idea
107494	metallic beige dim thistle magenta	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED COPPER	46	LG CAN    	1501.49	 beans haggle
107498	metallic burnished orchid firebrick white	Manufacturer#4           	Brand#43  	STANDARD BURNISHED STEEL	41	MED PACK  	1505.49	cial deposits are 
110001	olive ivory thistle papaya tan	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED NICKEL	3	SM JAR    	1011.00	ly unusual theodoli
110002	lime seashell goldenrod deep peach	Manufacturer#1           	Brand#11  	STANDARD BURNISHED STEEL	6	LG PKG    	1012.00	fluffi
110004	chartreuse bisque coral rose thistle	Manufacturer#1           	Brand#14  	STANDARD PLATED TIN	25	SM JAR    	1014.00	 final dependencies de
110008	lawn pale floral violet chocolate	Manufacturer#4           	Brand#44  	SMALL BRUSHED NICKEL	21	JUMBO PKG 	1018.00	beans cajole sl
112468	almond magenta pale moccasin floral	Manufacturer#5           	Brand#55  	MEDIUM PLATED BRASS	24	SM BOX    	1480.46	es wake blit
112469	light slate plum misty thistle	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED BRASS	38	WRAP CASE 	1481.46	ts. quietly ev
112471	purple royal tomato sky cyan	Manufacturer#4           	Brand#42  	MEDIUM POLISHED BRASS	43	LG BOX    	1483.47	 blithely bold the
112475	magenta blanched snow papaya antique	Manufacturer#3           	Brand#32  	PROMO ANODIZED TIN	5	LG PACK   	1487.47	accounts
114979	beige steel aquamarine tan blush	Manufacturer#3           	Brand#33  	PROMO PLATED TIN	31	SM PACK   	1993.97	. special courts kindl
114980	plum metallic indian snow bisque	Manufacturer#3           	Brand#33  	SMALL BRUSHED NICKEL	42	MED CAN   	1994.98	to beans boost slyl
114982	thistle linen midnight navy honeydew	Manufacturer#4           	Brand#45  	ECONOMY PLATED BRASS	39	LG CAN    	1996.98	 regular packages.
114986	dim firebrick seashell sienna lavender	Manufacturer#4           	Brand#41  	LARGE BRUSHED TIN	46	LG BOX    	2000.98	l the slyly regula
117490	goldenrod tomato almond cream salmon	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED BRASS	24	LG JAR    	1507.49	osely even acco
117491	plum sky olive antique lavender	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	42	LG BAG    	1508.49	t the account
117493	antique navajo dodger burlywood lawn	Manufacturer#4           	Brand#42  	LARGE ANODIZED NICKEL	28	WRAP PKG  	1510.49	efully after
117497	azure grey metallic honeydew navy	Manufacturer#4           	Brand#45  	SMALL BRUSHED TIN	35	JUMBO PKG 	1514.49	ending instruct
120001	yellow hot rose blue green	Manufacturer#1           	Brand#15  	STANDARD ANODIZED STEEL	38	SM BAG    	1021.00	he unusual requests.
120002	yellow pale blanched gainsboro moccasin	Manufacturer#4           	Brand#45  	STANDARD PLATED BRASS	22	WRAP CAN  	1022.00	e furiously even ex
120004	purple cream puff royal chocolate	Manufacturer#2           	Brand#24  	SMALL ANODIZED TIN	41	MED BAG   	1024.00	ly final
120008	forest rose orchid mint lime	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	5	JUMBO BAG 	1028.00	blithely slyly pend
122465	dodger lavender chiffon burnished snow	Manufacturer#3           	Brand#32  	LARGE ANODIZED TIN	38	WRAP CASE 	1487.46	the furious
122466	chiffon forest orange violet frosted	Manufacturer#2           	Brand#23  	SMALL BRUSHED BRASS	25	WRAP BOX  	1488.46	 accounts haggle
122468	blush orange plum royal olive	Manufacturer#4           	Brand#42  	SMALL ANODIZED NICKEL	7	MED CASE  	1490.46	 stealthily regular
122472	grey lemon cornflower deep orange	Manufacturer#3           	Brand#35  	MEDIUM PLATED TIN	30	LG CAN    	1494.47	unts impres
124977	olive azure hot mint lavender	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	23	LG JAR    	2001.97	otes slee
124978	violet tan midnight black wheat	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED TIN	18	MED PKG   	2002.97	nts boost s
124980	red mint navajo black burnished	Manufacturer#1           	Brand#15  	PROMO BRUSHED STEEL	41	JUMBO DRUM	2004.98	en pinto beans. 
124984	almond cream linen navajo rosy	Manufacturer#5           	Brand#52  	PROMO BRUSHED NICKEL	37	SM PACK   	2008.98	ironic 
127489	lime royal beige cream sky	Manufacturer#1           	Brand#15  	PROMO POLISHED STEEL	45	SM BAG    	1516.48	nts hag
127490	cyan firebrick deep turquoise seashell	Manufacturer#2           	Brand#21  	PROMO BURNISHED TIN	6	WRAP JAR  	1517.49	ake blithely. pinto b
127492	smoke chocolate bisque royal lavender	Manufacturer#2           	Brand#24  	STANDARD ANODIZED TIN	31	WRAP JAR  	1519.49	ites cajole fur
127496	linen sandy lemon olive tomato	Manufacturer#2           	Brand#25  	MEDIUM PLATED TIN	26	SM DRUM   	1523.49	ckages. s
130001	beige ivory red tan burlywood	Manufacturer#4           	Brand#44  	PROMO ANODIZED BRASS	50	WRAP PKG  	1031.00	o beans ha
130002	bisque smoke spring black wheat	Manufacturer#4           	Brand#41  	PROMO POLISHED BRASS	10	WRAP PACK 	1032.00	ully ironic deposits 
130004	rosy medium puff tan slate	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED NICKEL	44	MED DRUM  	1034.00	e slyly quickly i
130008	lemon goldenrod grey navy dodger	Manufacturer#4           	Brand#42  	ECONOMY POLISHED STEEL	43	LG PKG    	1038.00	e regular asy
132462	lemon moccasin grey chocolate ghost	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED COPPER	16	WRAP CASE 	1494.46	kly busy dependen
132463	honeydew powder thistle metallic blue	Manufacturer#4           	Brand#42  	SMALL POLISHED COPPER	22	JUMBO CASE	1495.46	 blithely package
132465	rosy indian cyan coral snow	Manufacturer#1           	Brand#12  	LARGE PLATED STEEL	20	LG CASE   	1497.46	ly even de
132469	medium bisque rosy linen maroon	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED STEEL	50	JUMBO JAR 	1501.46	ggle. regu
134975	goldenrod navajo black plum cornflower	Manufacturer#4           	Brand#45  	LARGE POLISHED TIN	44	SM JAR    	2009.97	ully final ex
134976	burnished gainsboro seashell navajo peru	Manufacturer#4           	Brand#44  	STANDARD BURNISHED COPPER	23	WRAP CAN  	2010.97	eposi
134978	beige plum cornsilk honeydew navajo	Manufacturer#2           	Brand#21  	STANDARD PLATED NICKEL	38	JUMBO CAN 	2012.97	usly slyly pendi
134982	blush azure blanched smoke slate	Manufacturer#1           	Brand#12  	MEDIUM PLATED NICKEL	45	LG BOX    	2016.98	 carefully ir
137488	chocolate plum pale cornflower hot	Manufacturer#5           	Brand#55  	LARGE ANODIZED NICKEL	9	SM BAG    	1525.48	furiousl
137489	moccasin maroon violet firebrick peru	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	6	LG CASE   	1526.48	thely even depend
137491	antique snow sandy yellow deep	Manufacturer#4           	Brand#45  	SMALL ANODIZED BRASS	50	LG CAN    	1528.49	 frays! fur
137495	steel light lace lime royal	Manufacturer#4           	Brand#45  	MEDIUM POLISHED TIN	15	JUMBO JAR 	1532.49	y among the
140001	ivory goldenrod sky honeydew misty	Manufacturer#4           	Brand#42  	LARGE BURNISHED TIN	37	SM CAN    	1041.00	 the final, ir
140002	yellow seashell orchid burnished pink	Manufacturer#4           	Brand#44  	LARGE POLISHED COPPER	42	WRAP DRUM 	1042.00	 nag carefully above
140004	dodger hot sienna ghost olive	Manufacturer#4           	Brand#44  	LARGE BURNISHED COPPER	13	SM DRUM   	1044.00	after the
140008	dodger forest tan chiffon lemon	Manufacturer#2           	Brand#22  	ECONOMY BRUSHED BRASS	44	MED JAR   	1048.00	ular packages abo
142459	frosted dark plum purple pale	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED BRASS	37	JUMBO BOX 	1501.45	osits cajole carefully
142460	white salmon black maroon cornflower	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED TIN	39	LG BOX    	1502.46	jole carefully busy p
142462	misty burlywood slate white orchid	Manufacturer#2           	Brand#22  	MEDIUM PLATED COPPER	23	MED BOX   	1504.46	lyly final theodolites
142466	red plum snow dodger goldenrod	Manufacturer#4           	Brand#41  	STANDARD POLISHED COPPER	46	SM CASE   	1508.46	f the fluffi
144973	drab indian black light magenta	Manufacturer#5           	Brand#55  	STANDARD BURNISHED COPPER	45	WRAP PKG  	2017.97	. bravely bold
144974	salmon saddle linen frosted rosy	Manufacturer#3           	Brand#31  	LARGE BURNISHED TIN	22	SM PKG    	2018.97	 pinto beans use
144976	beige red wheat powder misty	Manufacturer#4           	Brand#42  	MEDIUM PLATED BRASS	1	SM BAG    	2020.97	nic re
144980	blue purple medium tomato turquoise	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED COPPER	38	MED CAN   	2024.98	 the fl
147487	azure turquoise cream magenta burlywood	Manufacturer#3           	Brand#31  	LARGE BRUSHED COPPER	48	JUMBO CAN 	1534.48	fluffily final in
147488	blue brown tan steel violet	Manufacturer#2           	Brand#24  	SMALL ANODIZED COPPER	3	LG CAN    	1535.48	efully. b
147490	lawn tan blush mint rosy	Manufacturer#3           	Brand#35  	MEDIUM PLATED COPPER	26	WRAP CAN  	1537.49	ges cajole blithely fi
147494	blanched blue navy pale beige	Manufacturer#2           	Brand#22  	ECONOMY PLATED NICKEL	27	JUMBO PKG 	1541.49	o beans wake amo
150001	aquamarine burnished orchid pink thistle	Manufacturer#1           	Brand#14  	ECONOMY POLISHED NICKEL	38	WRAP BOX  	1051.00	c dependencies haggle 
150002	purple violet salmon firebrick cornflower	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED NICKEL	35	MED CAN   	1052.00	uickly ev
150004	tan grey gainsboro khaki chartreuse	Manufacturer#1           	Brand#13  	MEDIUM PLATED TIN	28	WRAP PKG  	1054.00	en deposits sleep
150008	smoke cream indian slate white	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED BRASS	22	WRAP BAG  	1058.00	lites. de
152456	medium peach lime brown goldenrod	Manufacturer#1           	Brand#13  	MEDIUM POLISHED STEEL	38	JUMBO PKG 	1508.45	lyly special grouch
152457	aquamarine antique floral midnight cream	Manufacturer#4           	Brand#44  	LARGE BURNISHED COPPER	45	WRAP CASE 	1509.45	tions affix above
152459	lawn blue seashell goldenrod lemon	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	24	LG CAN    	1511.45	regular deposi
152463	peach steel chartreuse ivory salmon	Manufacturer#3           	Brand#33  	PROMO PLATED NICKEL	14	WRAP BAG  	1515.46	to the deposits nag 
154971	khaki blue papaya misty forest	Manufacturer#5           	Brand#53  	PROMO POLISHED BRASS	27	LG BOX    	2025.97	s. ironic pains ha
154972	royal dim sienna forest turquoise	Manufacturer#5           	Brand#54  	STANDARD PLATED TIN	35	MED BAG   	2026.97	l foxes wake fu
154974	almond wheat salmon blush spring	Manufacturer#1           	Brand#14  	PROMO BRUSHED STEEL	36	MED CASE  	2028.97	s boost entici
154978	wheat saddle beige blanched puff	Manufacturer#5           	Brand#52  	LARGE PLATED NICKEL	45	MED BAG   	2032.97	luffily across the ca
157486	orange coral cream sandy burlywood	Manufacturer#1           	Brand#15  	STANDARD POLISHED COPPER	46	JUMBO CASE	1543.48	accounts
157487	maroon cyan saddle blue goldenrod	Manufacturer#2           	Brand#21  	PROMO PLATED STEEL	21	JUMBO BAG 	1544.48	key players
157489	steel cornflower cyan ivory antique	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	36	SM DRUM   	1546.48	hes. slyly
157493	purple burlywood red midnight coral	Manufacturer#5           	Brand#55  	STANDARD PLATED NICKEL	31	LG PKG    	1550.49	along the silent foxes
160001	drab metallic midnight tomato thistle	Manufacturer#1           	Brand#14  	LARGE BRUSHED COPPER	7	MED BOX   	1061.00	 careful
160002	saddle green coral white floral	Manufacturer#5           	Brand#54  	SMALL PLATED BRASS	33	MED DRUM  	1062.00	gular de
160004	dim coral dodger hot violet	Manufacturer#5           	Brand#52  	STANDARD BRUSHED TIN	8	LG BOX    	1064.00	 slyly even the
160008	rose misty grey yellow pale	Manufacturer#5           	Brand#55  	PROMO POLISHED STEEL	30	JUMBO BOX 	1068.00	 are furiously ca
162453	orchid mint ghost firebrick floral	Manufacturer#1           	Brand#14  	PROMO PLATED BRASS	12	SM CASE   	1515.45	eat slyly. bold exc
162454	mint navy wheat firebrick steel	Manufacturer#3           	Brand#33  	ECONOMY PLATED BRASS	9	LG BOX    	1516.45	ts hang above 
162456	misty sienna antique thistle almond	Manufacturer#3           	Brand#35  	ECONOMY PLATED TIN	40	LG CAN    	1518.45	nticing i
162460	blush dim ghost pink blanched	Manufacturer#4           	Brand#45  	STANDARD POLISHED NICKEL	41	WRAP PACK 	1522.46	uffily after the fin
164969	dodger chartreuse purple hot sienna	Manufacturer#2           	Brand#22  	ECONOMY PLATED COPPER	26	LG CAN    	2033.96	the some
164970	khaki magenta maroon green indian	Manufacturer#4           	Brand#43  	PROMO POLISHED COPPER	47	JUMBO CAN 	2034.97	s integrate
164972	goldenrod misty lime hot ghost	Manufacturer#5           	Brand#52  	PROMO POLISHED TIN	45	SM PKG    	2036.97	osits woul
164976	yellow black almond bisque purple	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	35	JUMBO JAR 	2040.97	the careful
167485	cornflower lace cream grey tomato	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED NICKEL	1	WRAP PACK 	1552.48	y accordin
167486	snow linen aquamarine cornsilk magenta	Manufacturer#4           	Brand#43  	PROMO POLISHED TIN	10	LG CASE   	1553.48	es. ideas sle
167488	navajo firebrick blush metallic purple	Manufacturer#5           	Brand#54  	ECONOMY BURNISHED BRASS	16	LG CAN    	1555.48	e regular, pendi
167492	slate goldenrod turquoise lime brown	Manufacturer#5           	Brand#55  	STANDARD BRUSHED STEEL	15	JUMBO CASE	1559.49	y ironic requ
170001	azure peach yellow violet papaya	Manufacturer#2           	Brand#25  	LARGE POLISHED NICKEL	29	WRAP DRUM 	1071.00	ix bold
170002	medium metallic moccasin rose bisque	Manufacturer#3           	Brand#32  	PROMO BURNISHED BRASS	10	WRAP BOX  	1072.00	encies su
170004	chartreuse cornsilk grey powder blanched	Manufacturer#2           	Brand#23  	LARGE PLATED BRASS	44	SM BAG    	1074.00	ent dependenc
170008	lemon linen powder ghost lavender	Manufacturer#1           	Brand#12  	SMALL POLISHED TIN	46	JUMBO BOX 	1078.00	ic requests. slyly exp
172450	black orchid cornflower sky tomato	Manufacturer#1           	Brand#15  	STANDARD ANODIZED BRASS	43	WRAP DRUM 	1522.45	ecial packages. furio
172451	moccasin navajo pale burnished sky	Manufacturer#3           	Brand#35  	SMALL BURNISHED TIN	29	JUMBO BOX 	1523.45	es. even, even pi
172453	lawn linen cyan slate turquoise	Manufacturer#1           	Brand#12  	STANDARD ANODIZED COPPER	17	WRAP PACK 	1525.45	g always at th
172457	chocolate lavender almond orchid lace	Manufacturer#5           	Brand#55  	LARGE ANODIZED COPPER	44	MED BAG   	1529.45	regular packag
174967	azure dodger almond ivory rosy	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED TIN	33	LG PACK   	2041.96	 according 
174968	olive spring saddle black chiffon	Manufacturer#3           	Brand#31  	SMALL BURNISHED TIN	12	LG PKG    	2042.96	ideas. ironic depos
174970	lavender black moccasin smoke ghost	Manufacturer#5           	Brand#54  	MEDIUM PLATED STEEL	21	MED BOX   	2044.97	ironic deposits are
174974	frosted lemon sandy tan snow	Manufacturer#5           	Brand#52  	LARGE ANODIZED BRASS	12	MED BOX   	2048.97	e carefully
177484	peach forest saddle lemon azure	Manufacturer#3           	Brand#35  	PROMO POLISHED COPPER	48	JUMBO BAG 	1561.48	 furiously against t
177485	medium saddle indian sky chiffon	Manufacturer#3           	Brand#32  	ECONOMY BURNISHED BRASS	30	LG DRUM   	1562.48	 slyly fi
177487	ghost white wheat light mint	Manufacturer#1           	Brand#12  	STANDARD BURNISHED COPPER	3	JUMBO PACK	1564.48	luffily express
177491	cyan drab pink purple midnight	Manufacturer#4           	Brand#42  	PROMO POLISHED BRASS	22	SM PKG    	1568.49	 foxes cajole ab
180001	aquamarine dodger puff cyan light	Manufacturer#1           	Brand#14  	ECONOMY PLATED BRASS	46	SM CASE   	1081.00	 packages. slyl
180002	lawn cyan khaki burlywood frosted	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED NICKEL	34	WRAP PKG  	1082.00	maintain
180004	navy beige brown rose powder	Manufacturer#2           	Brand#23  	STANDARD BURNISHED COPPER	1	WRAP PKG  	1084.00	y. fu
180008	navy indian beige gainsboro peru	Manufacturer#5           	Brand#51  	PROMO BURNISHED BRASS	42	SM PKG    	1088.00	th the silent, f
182447	smoke lavender light frosted brown	Manufacturer#3           	Brand#33  	ECONOMY POLISHED TIN	17	WRAP JAR  	1529.44	 accou
182448	forest misty navajo dodger cornflower	Manufacturer#4           	Brand#43  	PROMO POLISHED NICKEL	48	SM BAG    	1530.44	nusual deposits.
182450	linen turquoise royal thistle frosted	Manufacturer#2           	Brand#21  	STANDARD BRUSHED BRASS	23	JUMBO PACK	1532.45	ly pen
182454	misty azure almond ghost khaki	Manufacturer#1           	Brand#11  	LARGE ANODIZED COPPER	13	MED CASE  	1536.45	lyly according to the 
184965	drab light sandy medium midnight	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	30	JUMBO PKG 	2049.96	es haggle 
184966	peach hot powder cornsilk magenta	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED STEEL	40	SM JAR    	2050.96	nag fluffily. d
184968	light midnight chiffon spring blue	Manufacturer#4           	Brand#42  	PROMO PLATED COPPER	12	LG BOX    	2052.96	ounts haggle acro
184972	rosy floral khaki dark sandy	Manufacturer#3           	Brand#32  	STANDARD PLATED COPPER	22	JUMBO PACK	2056.97	ect ca
187483	chartreuse drab lime blue floral	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	47	MED DRUM  	1570.48	 sleep furiously
187484	cream red antique chocolate brown	Manufacturer#3           	Brand#31  	STANDARD BURNISHED TIN	1	LG DRUM   	1571.48	ly unus
187486	lavender burlywood burnished linen hot	Manufacturer#3           	Brand#31  	SMALL BURNISHED TIN	29	WRAP PACK 	1573.48	sual ideas 
187490	red drab burlywood dodger pale	Manufacturer#5           	Brand#52  	STANDARD ANODIZED BRASS	38	LG DRUM   	1577.49	. even, pending i
190001	powder coral chiffon burnished bisque	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED NICKEL	26	WRAP BOX  	1091.00	ly busy deposi
190002	peru coral rosy azure green	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	21	SM PKG    	1092.00	 express, daring sh
190004	ivory almond honeydew metallic dodger	Manufacturer#4           	Brand#44  	PROMO PLATED NICKEL	23	MED DRUM  	1094.00	 blithely regular t
190008	cream dark peru thistle gainsboro	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED STEEL	46	WRAP CASE 	1098.00	 pinto beans. fur
192444	cyan thistle salmon yellow lime	Manufacturer#2           	Brand#23  	PROMO PLATED NICKEL	5	MED BOX   	1536.44	deposits. regular, i
192445	sandy dim cornsilk green hot	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	1	MED BOX   	1537.44	s. blithely
192447	brown powder lace purple sky	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED STEEL	25	JUMBO BAG 	1539.44	g the fluffily 
192451	yellow chiffon saddle black midnight	Manufacturer#1           	Brand#12  	SMALL BRUSHED COPPER	4	WRAP CAN  	1543.45	ve ac
194963	hot lime plum purple aquamarine	Manufacturer#5           	Brand#54  	PROMO ANODIZED COPPER	26	MED DRUM  	2057.96	o beans haggle
194964	tomato black lime sandy dim	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED COPPER	3	WRAP CASE 	2058.96	se th
194966	plum gainsboro lemon peru wheat	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED BRASS	24	SM BAG    	2060.96	 fluff
194970	ivory goldenrod dim chiffon royal	Manufacturer#3           	Brand#31  	SMALL PLATED BRASS	8	LG BOX    	2064.97	arefully fl
197482	lace cream orange metallic violet	Manufacturer#5           	Brand#52  	MEDIUM POLISHED COPPER	43	MED DRUM  	1579.48	r deposits use
197483	magenta slate chartreuse cream chiffon	Manufacturer#3           	Brand#33  	SMALL POLISHED COPPER	13	JUMBO DRUM	1580.48	ans. furiously unusua
197485	beige lace burnished steel peru	Manufacturer#2           	Brand#25  	STANDARD BRUSHED COPPER	41	WRAP PKG  	1582.48	refully ironic, 
197489	green aquamarine almond cyan chartreuse	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	15	LG BAG    	1586.48	ches. slyly u
\.


--
-- Data for Name: partsupp; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.partsupp (ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment) FROM stdin;
1	2	3325	771.64	, even theodolites. regular, final theodolites eat after the carefully pending foxes. furiously regular deposits sleep slyly. carefully bold realms above the ironic dependencies haggle careful
2	3	8895	378.49	nic accounts. final accounts sleep furiously about the ironic, bold packages. regular, regular accounts
4	5	1339	113.97	 carefully unusual ideas. packages use slyly. blithely final pinto beans cajole along the furiously express requests. regular orbits haggle carefully. care
8	9	6834	249.63	lly ironic accounts solve express, unusual theodolites. special packages use quickly. quickly fin
2501	2	6980	379.98	across the slyly final instructions: carefully unusual sheaves wake along the blithely ironic foxes. slyly even deposits doubt quickly durin
2502	3	4286	502.00	kages haggle quickly after the stealthily regular packages. requests haggle blithely. quickly even foxes nag quickly even courts. regular, regular requ
2504	5	5946	480.90	osits. fluffily even notornis use alongside of the bravely even ideas. silent deposits wake. slyly pending deposits nag above the furiously spe
2508	9	8509	733.41	s detect furiously after the blithely bold accounts. unusual platelet
5001	2	3018	557.66	ix slyly across the pending packages. dogged foxes was blithely. car
5002	3	6996	739.71	ely. regular courts unwind regular, regular pinto bean
5004	5	5917	247.03	xpress courts. carefully even deposits are against the sp
5008	9	4667	603.70	fluffily unusual foxes cajole furiously. silent t
7501	2	896	538.81	arefully special packages. ironic dependencies print fluffily along the furi
7502	3	4436	377.80	instructions. slyly even decoys are requests. regula
7504	5	7992	523.49	l platelets. carefully regular requests among the fluffily bold packages detect carefully about the carefully regular foxes. idle, ironic packages haggle slyly along the final sautern
7508	9	642	794.89	oost furiously. bravely pending pinto beans haggle carefully. bold pinto beans wake fluffily idly ironic foxes. carefully pendi
10001	2	3185	12.18	long the furiously busy deposits. regular, express courts according to the regular deposits nag against the quickly even deposit
10002	3	6728	529.58	yly carefully bold requests. furiously express requests need to kindle furiously among the fluffily regular foxes. even packages use slyly blithe, regula
10004	5	608	911.26	ironic accounts. frets maintain pending deposits; pinto beans use carefully re
10008	9	9362	644.54	ounts haggle blithely against the regular excuses. slyly final courts around the furiously ironic requests cajole above the regular pinto beans! fluffi
12498	2	4574	483.44	counts according to the ideas boost furiously at the quickly express excuses. car
12499	3	8646	722.34	o beans promise furiously against the even accounts. regular packages across the quickly express dependenci
12501	5	1463	223.58	 carefully along the blithely even deposits. furiously bold deposits alongside of the quickly special foxes sleep closely bold platelets. furiously ironic packages behind the slyly
12505	9	6031	208.14	luffily special accounts cajole closely. blithely bold requests engage carefully after the packages. quickly stealthy packages cajole. flu
14999	2	4034	151.75	ffix slyly along the regular asymptotes. slyly final platelets boost carefully above the carefully regular deposits. ironic, fluffy packages use silent platelets? quickly pending accounts cajol
15000	3	9975	841.19	ealms. carefully special packages are furiously across the even, bold pinto beans. slyly even deposits among the blithely unusual packages are quickly ironic dinos. carefully even pa
15002	5	9856	7.20	packages detect evenly above the never ironic instructions. carefully final packages boost blithely. final, pending dependencies wake carefully about the furiously even warhorses. qu
15006	9	4842	199.28	l ideas! ironic requests cajole blithely special, even r
17500	2	4989	422.46	e furiously furiously pending instructions. special asymptote
17501	3	5401	139.06	ts. carefully even packages use fluffily across the furiously unusual accounts. final, quick requests boost. 
17503	5	6867	923.52	 final packages. even dependencies nag furiously express pinto beans. final deposits sleep: carefully express foxes use regula
17507	9	4953	538.55	jole furiously blithely bold accounts. special requests use evenly regular, final packages. blithely regu
20001	2	9562	151.07	ons sleep quickly across the slyly regular Tiresias: unusual requests integrate carefully. ideas use bl
20002	3	6858	786.94	 detect. pending, even packages boost against the requests! fluffily special requests dazzle quickly regular instructions. blithely express s
20004	5	5961	757.11	es. quickly ironic foxes wake fluffily even instructions. finally special packages cajole fluffily even deposits. final accounts wake. fluffily even pac
20008	9	9219	214.55	hall have to print blithely ironic accounts. furiously unusual deposits are. regular, pending excuses serve carefully. quickly regular
22495	2	9398	646.87	ully pending deposits. slyly even pinto beans according to the blithely regular theodolites are fluffily among the regular foxes. quickly regular depos
22496	3	8268	444.21	ully even ideas are furiously. furiously unusual instructions s
22498	5	4365	453.83	theodolites would sleep special forges. blithely express packages wake fluffily. deposits print furiously. final deposits sleep above the ironic, iro
22502	9	4772	567.57	 are slyly unusual deposits. blithely unusual deposits run instructions. furiousl
24997	2	7394	990.11	e carefully slyly special instructions. regular deposits sleep blithely across the quickly regular accounts. regular grouches above the final ideas wake alongside of the ruthl
24998	3	4887	626.55	quickly even accounts cajole slyly to the even di
25000	5	1067	312.63	egular platelets unwind blithely. furiously slow pinto beans wake furiously pending ideas. even depths above the express gifts wake up the deposits. furiously thin realms haggle quiet acco
25004	9	4009	198.72	slyly unusual asymptotes. pending foxes across the accounts sleep furiously final accounts. fluffily ironic packages sleep quickly. blithe
27499	2	8267	323.91	 beans sleep along the ruthlessly ironic requests; instruct
27500	3	2703	235.56	sts mold furiously final deposits. quickly even requests ought to sleep. regular, regula
27502	5	7061	539.66	ously even attainments across the pending, final platelets cajole quickly regular instructions. fluffily final requests wake doggedly. unusual pains among the quickly expr
27506	9	7775	226.90	 blithely regular pinto beans. ironic, pending accounts haggle regular theodolites. final requests wake against the do
30001	2	6906	776.88	y against the unusual, final sentiments. pearls are. pinto b
30002	3	2988	446.70	ages across the slyly regular packages are carefully regular packages. final asymptotes unwind slyly. express deposits haggle carefully carefully bol
30004	5	6671	617.74	eep blithely alongside of the carefully express theodolites. carefully express theodolites wake blithely final theodolites. slyly bold theodolites grow carefully a
30008	9	3163	748.16	er the final asymptotes dazzle carefully express theodolites. ironic, bold reques
32492	2	9005	295.84	 special escapades-- pending, even asymptotes boost about the fluffily regular packages. regular, ironic requests detect. quickly pending deposits cajole blithely above th
32493	3	7647	813.17	g the furiously unusual pains kindle ironic, pending requests. express, pending theodolites boost. ironic, expres
32495	5	6349	996.43	e blithely special ideas. express packages cajole blithely. 
32499	9	2300	53.00	its sleep carefully. furiously careful courts cajole furiously d
34995	2	3109	866.57	ly ironic courts was enticingly slyly bold instructions. carefully bold excuses cajole at the ironic, final requests. quickly final excuses above the regular deposits are furiously 
34996	3	5305	399.09	gular, bold excuses. quickly pending instructions nag express deposits. final accounts wake. busy, ironic packages affix quickly along
34998	5	6342	431.96	 ironic packages use blithely. unusual packages according to the boldly ironic pinto beans nag blithely instead of the furiously regular packages. ironi
35002	9	9486	493.40	sleep furiously requests. slow requests wake furiously carefully regular requests. fluffil
37498	2	8866	176.81	ts according to the dolphins wake among the carefully expres
37499	3	992	191.14	 foxes are express, regular packages. regular accounts according to the furiously even packages haggle packages: unusual, ironic theodolites play carefully. fluffily regular packag
37501	5	5029	457.87	ccounts. final, ironic accounts cajole enticingly after the blithely final foxes. pending accounts at the pending, silent asymptotes unwind sly
37505	9	4146	490.69	t carefully slyly regular pains. slyly special theodolites about the blit
40001	2	9531	683.63	structions. bold packages use furiously against the carefully silent braids. foxes along the unusual, close accoun
40002	3	953	17.33	posits cajole fluffily. packages serve. fluffily ironic frays haggle. quickly express ideas alongside of the final deposits solve asymptotes. ironic attainment
40004	5	7207	49.41	print dolphins. even, regular excuses after the furiously thin theodolites boost carefu
40008	9	2789	417.68	es across the blithely special platelets eat carefully quickly s
42489	2	6125	952.71	 slyly. carefully close platelets detect. fluffily permanent deposits across the carefully ironic instructions are furiously final requests. quickly express packages detect carefully thinly
42490	3	2499	221.56	nag fluffily against the ironic deposits. fluffily unusual packages nag unusual pinto beans. fluffily regular Tiresias use. slyl
42492	5	2290	606.09	g dependencies. quickly regular ideas haggle carefully. carefully express theodolites are carefully regular excuses. furiously regular instructions print quickly. asymptotes ar
42496	9	8379	996.23	n, even ideas sleep never even deposits. carefully special excuses dazzle blithely deposits.
44993	2	139	460.90	te carefully even accounts. slyly final requests upon the slyly special accounts wake even ideas? regular instructio
44994	3	9042	441.89	es; requests are slyly always regular packages. regular ideas wake above the quickly even ideas. quickly busy requests nag slyly quickly final dependencies. slyly express packages sleep. ex
44996	5	5278	132.16	e fluffily express ideas. blithely bold accounts haggle pending deposits. furiously ironic requests use blithely. bold attainme
45000	9	1187	328.67	es about the silent packages haggle unusual packages. f
47497	2	1695	570.03	raids wake ruthlessly whithout the dependencies. special deposits wake c
47498	3	4253	730.02	fully even instructions about the stealthily final platelets sleep alon
47500	5	6833	236.45	latelets. quickly bold multipliers nag slyly furiously regular deposits. pending, even platelets according to the quickly special packages are about the ironic, pending foxes. unusual pinto beans 
47504	9	2754	567.05	ajole furiously unusual requests. furiously regular ideas use slyly-- quickly special instructions use. blithely regular courts engage slyly unusual platelets. silent, final sauternes after the 
50001	2	9271	85.17	pecial requests print carefully. quickly silent instructions across the carefully unusual packages are ironic requests. slyly regular deposits sleep quickly. fu
50002	3	5397	939.41	es-- carefully regular accounts sleep fluffily instructions! silent dependencies wake slyly after the fluffily final requests. silently ironic ideas among the even deposits are quickly after the flu
50004	5	7145	425.34	 beyond the thinly final theodolites. furiously final asymptotes nag according to the furiously express request
50008	9	4036	918.37	s use carefully along the daringly even courts. slyly regular packages cajol
52486	2	5785	178.83	o the slyly blithe requests. excuses against the slyly even accounts are slyly ironic instructions. furiously regular instructions detect carefully. courts are ironic requests. slyly unusual plate
52487	3	5004	396.42	ss the furiously even requests. quickly pending platelets wake slyly unusual dolphins. slyly pending deposits a
52489	5	9018	519.51	otes wake blithely across the bold accounts. blithely regular foxes about the final requests nag boldly about the fluffily final theodol
52493	9	2773	278.74	lar platelets sleep furiously blithely regular accounts. regular, ironic dependencies sleep furiously quickly regular accounts. slyly re
54991	2	2451	817.19	iously. bold packages nag: ironic packages hang quickly above t
54992	3	7410	667.53	above the slyly special ideas kindle blithely quietly pending frays-- slyly special deposits sleep blithely above the carefully even requests. caref
54994	5	5555	639.33	le according to the carefully regular instructions. regular, even excuses are enticingly regular pack
54998	9	805	794.64	t packages. sometimes regular foxes boost carefully blithely regular ideas. packages wake furiously slyly pending deposits. slyly regular accounts against the final requests cajole alon
57496	2	6554	601.74	ess packages cajole. express, special requests are after the quickly regular requests. even requests above th
57497	3	2938	328.46	ully even foxes haggle carefully fluffily final requests. deposits boost quickly carefully bold deposits. fluffily ir
57499	5	5417	810.05	luffily silent accounts. fluffily express deposits detect carefully among the accounts. slyly regular deposits across the quick packages are final frets; foxes wake furiously. blithely regular depen
57503	9	7586	368.05	slyly bold instructions boost after the deposits. blithely regular accounts according to the furiously ironic accounts will hav
60001	2	1770	535.64	inal instructions. express accounts wake fluffily against the carefully final requests; express sheaves affix sly
60002	3	6686	746.49	fully furiously final packages. carefully special instructions haggle accor
60004	5	3365	13.12	nding accounts. express accounts wake furiously according to the fluffily pending theodolites. quickly regular orbits sleep. carefully regular packages sleep. slyly pending foxes according to the
60008	9	1108	789.98	special dependencies against the carefully even accounts cajole ironic requests. ironic accounts use blithely slyly regular theodolites. furiou
62483	2	1057	507.70	 quickly special instructions. ironic foxes cajole fur
62484	3	8259	261.73	ies haggle furiously final, regular grouches. bold requests are quickly regular warhorses. ironic packages wake across the excuses. furiously final asymptotes thrash among the carefully
62486	5	8754	600.94	nusual dependencies. slyly express gifts boost ruthlessly quickly daring theodolites. final theodolites above the ideas sleep fluffily after the furiously silent foxes. slyly ironic inst
62490	9	1646	834.42	es run blithely even pinto beans. regular escapades sleep. blithely pending accounts boost blithely. ironic, pending deposits haggl
64989	2	2768	742.67	even ideas cajole enticingly against the pinto beans. even requests wake slyly. bold, even packages haggle slyly furiously regular dependen
64990	3	2317	342.19	usual, bold deposits haggle fluffily. carefully final request
64992	5	7489	917.94	packages solve finally. even, final foxes use. bold ideas haggle 
64996	9	278	476.06	 escapades engage slyly. ironic, ironic deposits wake. silent excuses print furiously. finally bold theodolites sleep blithely. furiously regular foxes 
67495	2	8082	348.42	es sleep fluffily excuses. fluffily unusual platelets wake fluffily. carefully enticing packages accordi
67496	3	7863	209.16	es. carefully ironic deposits after the blithely express dependencies wake carefully express, final accounts. 
67498	5	9387	221.77	ully after the slyly stealthy accounts. accounts haggle boldly furiously close pinto beans. furiously iro
67502	9	1603	692.66	ck dolphins. unusual, express theodolites was fluffily. carefully ironic theodolites sleep. quickly bold accounts cajole slyly quickly final requests. carefully even pin
70001	2	909	276.85	old pinto beans. carefully pending requests according to the regular requests kindle across the quickly regular dep
70002	3	7095	853.00	ix regular foxes. carefully final deposits nag slyly abo
70004	5	711	277.56	, special dolphins sleep quickly. silently express deposits use slyly 
70008	9	8320	737.22	 furiously final theodolites wake slyly across the courts. ironi
72480	2	2974	993.11	ns. slyly pending foxes boost closely about the fluffily regular instructions. ironic instructions unwind blithely. ironic foxes detect 
72481	3	2594	48.45	ly regular ideas detect. blithely regular ideas nag carefully. furiously quiet packages boost. regular deposits nag. quick accounts hang ruthlessly carefully pending requests. quic
72483	5	1100	583.60	odolites are daringly above the fluffily unusual foxes. final deposits was. bold, regular patterns nag along the
72487	9	4388	790.85	ly quick requests? blithely bold instructions above the ironic orbits breach slyly even asymptotes. blithely expr
74987	2	59	844.36	efully fluffy accounts cajole ironic packages. furiously
74988	3	5120	283.07	es. quickly ironic excuses cajole-- quickly quiet ideas are atop the sometimes blithe depe
74990	5	1875	782.59	x blithely. fluffily pending frays eat blithely blithely even ideas. quic
74994	9	3614	764.58	to beans; even deposits boost blithely. furiously final excuses above
77494	2	5604	6.95	y special excuses after the permanently quick requests cajole blithe
77495	3	5375	554.32	ts. blithely pending instructions nag furiously platelets. bold packages haggle fluffily alongside of the deposits. blithely express dependencies wake slyly. slyly unusual excuses n
77497	5	5532	800.22	special packages nag fluffily. blithely regular ideas snooze furiously slyly unusual foxes. special requests sleep carefully across the furiou
77501	9	9172	418.91	hely furiously regular packages; furiously special requests cajole carefully special platelets! furiously regular courts are. carefully ironic asymptotes 
80001	2	1080	29.93	r foxes-- express ideas haggle. accounts cajole pending, ironic packages. bold, pend
80002	3	747	393.74	counts boost against the carefully even waters. furiously re
80004	5	6565	285.37	kages haggle about the slyly express excuses. blithely pending packages cajole about the foxes! un
80008	9	9739	641.61	ilent deposits. quickly final theodolites across the slyly bold patterns snooze blithely ironic accounts! caref
82477	2	4739	976.42	 blithely regular instructions hang up the deposits. quickly unusual 
82478	3	7183	233.51	e furiously final packages; slyly pending patterns 
82480	5	7423	421.06	s: quickly bold dependencies use. slyly regular deposits cajole. bold, ruthless instructions sleep after the carefully final sentiments. furiously ironic requests wake
82484	9	5632	351.92	ithely unusual deposits are slyly above the requests. ironic, unusual asymptotes across the furiously final requests affix quickly among the unusual packages. spec
84985	2	8099	446.39	t carefully according to the slyly even accounts. fluffily ironic dolphins haggle. furiously express theodolites according to the bold packages boost slyly regular foxes. carefully ironic epitaphs 
84986	3	2073	586.09	 unusual requests are fluffily after the final asymptotes. final, regular grou
84988	5	2002	986.63	otes. fluffily final accounts haggle furiously regular requests; blithely final packages cajole carefully: blithely ironic instructions cajole carefully. iron
114980	3	5907	993.22	es haggle furiously unusual requests. blithely final accounts wake. dependencies need to haggle 
84992	9	6988	540.70	 ideas affix furiously carefully bold theodolites. fluffily final accounts until the pending packages are slyly quickly final foxes. furiously ironic foxes wake carefully. sly
87493	2	3335	255.95	to beans wake fluffily final theodolites. requests sublate fluffily ironic, special 
87494	3	2941	403.17	 use slyly regular, special dependencies. pending foxes wake above the 
87496	5	5635	624.84	ng deposits. slyly permanent requests boost slyly quickly even foxes. final, regular foxes boost slyly about the ironic, ironic asym
87500	9	3785	877.53	luffily across the quickly express instructions: quickly daring accounts poac
90001	2	4868	526.40	jole permanently around the slyly special theodolites. ironic, even requests are fluffily across the furiously bold
90002	3	2609	529.45	y instructions into the slyly silent theodolites haggle evenly around the quickly unusual forges. slyly pending ideas hag
90004	5	4540	774.25	silent pinto beans sleep slyly. blithely ironic requests are carefully slyly regular deposits. requests sleep furiously. slyly unusual packages cajole slyly. silently pending theo
90008	9	7322	321.89	deposits boost furiously above the slyly ironic accounts. bold deposits use blithely across the special,
92474	2	515	519.24	uests about the dependencies cajole Tiresias-- blithely ironic ideas use quickly unusual foxes. quickly even packages wake carefully special ideas. blithely final instructions sle
92475	3	9269	703.17	p blithely among the patterns. slyly pending pinto beans cajole enticingly even, express accounts. final theodolites use fluffily asymptotes. furiously express accounts outside the quickly pendin
92477	5	9415	13.44	 sleep slow, special deposits. regular asymptotes affix fluffily after the quickly final deposits. furiously pending ideas integrate across the closely regular excus
92481	9	5431	703.81	cross the blithely ironic asymptotes impress furiously blithely final foxes. ironic deposits among the accounts boost furiously express excuses; 
94983	2	3535	622.84	tes. regular, ironic deposits cajole according to the fluffily express accounts. requests are after the accounts. p
94984	3	1591	627.44	he furiously ironic ideas. regular, unusual dependencies wake blithely above the blithely regular accounts. furiously special orbits 
94986	5	770	915.78	uld have to wake quickly before the furiously even pinto beans! quickly ironic accounts b
94990	9	7308	414.90	ding to the quickly regular dependencies use slyl
97492	2	2489	982.19	ke carefully about the always ironic foxes. ironic packages atop the furiously special dolphins affix after the sl
97493	3	257	837.54	ts are slyly quick requests. carefully regular deposits cajole carefully after the carefully ironic foxes. 
97495	5	3379	553.79	fily even theodolites. carefully sly requests wake. quickly pending acc
97499	9	9469	919.71	ld platelets! slyly regular packages sleep silently alo
100001	2	9505	291.25	ronic accounts cajole. ironic theodolites haggle quickly throughout the instructions; blithely silent dependencies haggle. blithely ironic foxes accordin
100002	3	9248	228.39	nusual accounts beyond the quickly unusual packages haggle furiously special courts. blithely pending accounts alongside of the final pa
100004	5	4556	749.70	dencies against the requests integrate slyly carefully regular packages. packages among the foxes cajole after the furiously express a
100008	9	4181	605.59	xcuses wake around the quickly special instructions. pending deposits print silently. idly regular packages cajole fl
102471	2	6209	778.65	into beans nag blithely above the furiously even packages
102472	3	2368	172.31	re carefully regular instructions. blithely pending the
102474	5	4724	727.27	s use furiously special instructions. slyly even packages haggle blithely across the slyly bold deposits. quickly even asymptotes haggle furiously-- ironic theodolites ab
102478	9	1710	686.91	latelets against the stealthy foxes wake furiously along the unusual, even foxes. special accounts haggle: blithely furious requests doubt fluffily across the fluf
104981	2	8656	438.45	fix slyly above the slyly regular packages. furiously pending deposits poach blithely furiously pend
104982	3	1642	228.98	the regular pinto beans. fluffily pending theodolites are ideas. slyly even excuses according to the furiously pending accounts are slyly somas. pending, even depo
104984	5	6832	601.56	 around the furiously regular somas sleep carefully about the silently final dependencies. regular, unusual request
104988	9	1197	296.51	ccounts cajole slyly about the final deposits. careful, express deposits along the
107491	2	2450	973.00	vely silent instructions affix. slyly final attainments wake. bold foxes cajole along the final, special requests. special, special excuses haggle
107492	3	7593	470.06	ccounts are furiously bold theodolites-- ironic dolphins sle
107494	5	5131	64.64	longside of the final, even packages? ironic frays sleep. express dugouts nag. carefully bold packages against the regular requests detect a
107498	9	4304	539.32	thely special requests. furiously daring deposits cajole carefully even foxes. blithely regular requests wake pending, final asymptotes. packages after the deposits use slyly above the carefull
110001	2	6088	336.90	side the carefully final packages. slyly bold instructions b
110002	3	779	993.23	quests wake slyly fluffily silent accounts. bold excuses haggle final foxes. dugouts after the quickly final packages are fluffily slyly unusual pinto be
110004	5	7563	652.30	he carefully even deposits. furiously regular requests integrate carefully carefully final packages. regular accounts are slyly caref
110008	9	4047	299.11	 nag quickly across the final ideas. special theodolites haggle fluffily according to the furiously final pinto beans. bold theodoli
112468	2	8025	998.94	 final requests. silently regular accounts against the slyly quiet accounts haggle furiously 
112469	3	2101	993.88	notornis play blithely. regular, special instructions use quickly even deposits. regular, ironic accounts poach slyly unusual packages. regular excu
112471	5	8656	516.38	ructions play slyly ironic packages. packages about the ironic accounts affix furiously across the special pains. carefully regular deposits wake blithely pending frets. final, f
112475	9	2295	949.27	equests. furiously regular foxes nag during the quiet, pending deposits. final deposits sleep doggedly. enticingly ev
114979	2	5318	543.98	regular requests use carefully regular accounts. even deposits affix fluffily special deposit
114982	5	1990	619.91	osits. ironic pains boost ironically. sly requests haggle furiously. sly pinto beans use. ironic, final requests wak
114986	9	3320	47.94	 requests nag furiously: quickly ironic ideas cajole quickly. furiously bold instructions doubt furiously after the slyly bold foxes: carefu
117497	9	4102	860.75	always bold foxes haggle carefully. express deposits will wake furiousl
117490	2	4810	409.53	usly across the unusual, ironic dolphins. carefully ironic excuses are quickly slyly express theodolites. thinly ironic ideas cajole quickly about the blithely regular accounts. bold sentiments ar
117491	3	6228	779.75	osits haggle. thinly regular ideas sleep blithely around the even, even packages. final, final accounts cajole care
117493	5	4655	244.20	 use carefully beside the even theodolites. Tiresias about the furiously pending 
120001	2	8135	544.90	unts. deposits according to the silent, bold instructions boost slyly among the final, final foxes. slyly ironic deposits sleep carefully unusual foxes. even instructions ar
120002	3	1137	112.24	along the quickly express deposits are furiously against the fluffily bold forges. unusual courts after the
120004	5	3919	430.06	carefully pending packages above the slyly final asymptotes cajole alongside of the carefully silent frays. silent, ironic acco
120008	9	1248	319.64	 boost busily regular ideas. carefully pending ideas doubt slyly beyond the courts. ironic, quiet packages boost carefully against the blithely even p
122465	2	7817	987.38	ily bold instructions. ruthlessly regular platelets are quickly carefully regular ideas-- furiously regular Tiresias nag furiously bold ac
122466	3	1665	258.68	cording to the slyly final packages. blithely bold foxes are furiously ironic pinto beans. carefully special packages run. packages are carefully alongside of the regular, final d
122468	5	77	399.36	y pending sheaves play. even, express requests cajole across the express dependencies. ironic accounts are furiously ironic, regular ideas. carefully regular inst
122472	9	9998	748.91	blithely unusual pinto beans detect carefully pending foxes. idle, even accounts after the blithely regular pinto beans detect about the furiously special d
124977	2	2720	468.82	s. furiously ironic theodolites wake slyly across the pending requests. quickly express ideas are against the blithely silent pinto beans. pending pinto beans affix according to the blithely i
124978	3	7787	152.61	es: furiously ironic theodolites cajole pinto beans. accounts nag slyly regular deposits? carefully regular requests above the ironic packag
124980	5	9737	818.26	kly even packages after the even, silent accounts haggle quickly bold, ironic accounts. ironic, pending accounts integ
124984	9	6793	809.83	excuses. slyly even packages cajole furiously. instructions sleep according to the regular deposits. ironic requests sleep furiously regular pinto
127489	2	4985	781.47	heodolites cajole quickly pinto beans! pending, regular requests are furiously among the slyly permanent requests. bli
127490	3	5283	416.03	y pending deposits use carefully slyly special deposits. idly ironic asymptotes sleep furiously according to the blithely final requests? ironic,
127492	5	5958	842.03	egular, sly packages. deposits use furiously about the deposits. busily unusual requests sleep furiously ruthless decoys. express packages nag carefully furiously
127496	9	7044	960.93	iously express packages. carefully ironic requests boost doggedly since the ideas. carefully final requests wake blithely about the furious
130001	2	9930	568.68	 fluffily final depths above the carefully silent dependencies sleep slyly slyly unusual instructions. carefully ironic dependencies boost furiously final packages. quickly final asymptotes play
130002	3	8453	37.78	ronic gifts wake above the furiously final accounts. regular 
130004	5	2097	389.29	 silent accounts nag blithely after the instructions. quickly regul
130008	9	5605	387.17	y unusual platelets affix carefully furiously special packages. final depths among the furiously unusual courts sleep blithely regular asymptotes. final packages nag among the slyly final requests. 
132462	2	1791	969.80	ounts against the ironic pinto beans sleep carefully slyly regular ideas: requests haggle quickly whithout the furiously regular theodo
132463	3	4064	620.75	. carefully ironic deposits nod furiously. express requests are closely after the slyly regula
132465	5	3483	231.27	 regular packages integrate across the final, final ideas. blithely regular requests about the 
132469	9	1845	128.65	he blithely ironic excuses boost enticingly furiously special instructions. carefully final ideas sleep. foxes sleep ruthlessly abou
134982	9	4472	593.62	sleep carefully. requests against the final instructions lose blithely after the furiously silent requests. sl
134975	2	8904	54.06	unts. slyly ironic packages sleep along the final ideas. instructions cajole idly furiously even ideas. final, final braids against the ironic, exp
134976	3	8293	454.30	e blithely idle pinto beans. carefully pending theodolites sleep; fluffily ir
134978	5	4793	98.23	nal instructions use alongside of the final deposits. final instructions use carefully carefully final deposits. slyly bold requests integrate slyly accounts. slyly even requests across the blit
137488	2	4463	133.85	ccounts. slyly silent accounts dazzle furiously quickly pending asymptotes. even somas wake blithely about the ironic, ir
137489	3	7560	373.86	ts; final accounts should wake fluffily after the slyly even foxes!
137491	5	4593	732.60	usly. accounts cajole slyly. ironic theodolites affix about the ironic packages. ironic, busy platelets haggle blithely; quickly final packages above the carefully careful 
137495	9	5356	517.62	ckey players detect about the blithely even courts. quickly busy packages sleep furiously about the slyly regular orbits. slyly final accounts wa
140001	2	3675	12.89	ual pinto beans boost furiously carefully pending theodolites. ironic requests use along the ironic deposits. pinto beans nag
140002	3	4647	842.34	g the blithely regular accounts sleep blithely unusual ideas. bli
140004	5	8734	901.06	al, pending packages. courts cajole slyly. furiously pending asymptotes are furiously final, final ideas. silent, regular accounts haggle furiously platelets. carefully regula
140008	9	5818	456.87	ons. careful requests are slyly carefully special packages. quickly pending deposits sleep blithely according to the fin
142459	2	6973	676.95	uests. quickly unusual pinto beans sleep blithely. final instructions are above the blithely ironic instructions. final pinto beans among the carefully final d
142460	3	6180	275.40	ress theodolites maintain. blithely bold platelets sleep
142462	5	7922	135.14	unts wake across the carefully special accounts. fluffily final packages cajole. ironic pinto beans use by the blithely ironic deposits. r
142466	9	8132	633.40	ieve furiously. furiously silent accounts nag. slyly pending deposits are. brave ideas cajole. carefully regular
144973	2	5891	553.99	xes. quickly daring dependencies use carefully carefully pending accounts. regular packages wake carefully. furiously bold packages wake carefully enticin
144974	3	1233	375.11	 the ironic, regular foxes. ironic, pending packages haggle carefully boldly bold foxes. fluffily unusual ideas hang ironic frays. express, 
144976	5	4231	592.18	e ironic, unusual ideas. quickly silent requests according to the regular ideas haggle ironically regular excuses. special ideas above the furiously silent packages wake ironic packages. stea
144980	9	6950	404.79	s cajole theodolites. regular packages are across the s
147487	2	2038	629.44	uctions affix blithely blithely regular accounts. unusual, ironic theodolites wake. furiously unusual pinto beans are final packages. regular, daring d
147488	3	4274	952.70	unusual platelets above the carefully silent requests use quickly regular pac
147490	5	9051	932.08	carefully final requests. carefully pending deposits cajole fluffily across the regular pains. carefully unusual excuses sleep slyly. quickly ironic packages use blithely. quickly silen
147494	9	1836	184.67	onic deposits wake furiously after the packages. regular packages cajole slyly. final, ironic excuses nag among the special packages. slyly ironic pinto beans after the carefully pend
150001	2	6078	149.74	lar dependencies. regular instructions sleep slyly. ironic pa
150002	3	3912	359.64	rbits integrate carefully silently express deposits. regula
150004	5	644	165.89	ly along the pending sauternes. frays boost against the final, silent asymptotes. packages integrate blithely. slyly blithe requests haggle fi
150008	9	6389	950.59	l deposits: furiously regular requests haggle. quickly ironic courts cajole. slyly bold ideas ought to boost furiously among the blithely unusual accounts.
152456	2	8602	538.32	deposits unwind quickly upon the fluffily silent foxes. quiet accounts integrate carefully even accounts. unusual, pending instructions cajole care
152457	3	474	678.87	accounts. even, ironic accounts wake fluffily along the final instructions. furiously even
152459	5	5731	982.70	ts. quickly regular theodolites boost slyly according to the even instructions. ironic accounts
152463	9	6178	612.44	idly ironic foxes. furiously express requests detect among the r
154971	2	7602	174.89	tes sleep along the even deposits. regular requests cajole furiously. quick theodolites above the blithely silent depos
154972	3	7036	207.68	 pending instructions. slyly ironic depths do haggle. even frets boost. slyly regular pains haggle along the quickly ironic packages. final, pending de
154974	5	6807	290.23	ly enticing foxes. ironic packages wake quickly across the furiously final platelets. final, unusual waters are sometimes foxes. carefully unusual packages according to the final requests wake
154978	9	6253	208.05	r accounts. thinly bold accounts wake fluffily. furiously final ideas use about the slyly unusual instructions. asy
157486	2	4480	712.89	sual, special foxes nod quickly alongside of the quickly ruthless dugouts. deposits nag quickly al
157487	3	9336	178.49	gle thinly after the slyly pending accounts. ironic, regular asymptotes haggle furiously Tiresias. furiously ironic dependencies hang slyly across the packages. furiously bold acco
157489	5	106	740.96	silent, ironic packages cajole. carefully unusual packages are quickly close instructions. furiously iron
157493	9	7054	668.47	grate alongside of the slyly bold frays. brave multipliers haggle. pending theodolites boost enticing foxes. quickly pending e
160001	2	5445	390.93	e carefully among the furiously pending ideas. special requests about the unusual,
160002	3	8434	858.25	aggle slyly according to the slyly ironic deposits; bold packages nag blithely after the even, final dependencies. slyly even pinto beans affix. quickly final theodolites about the slyly regular
160004	5	5012	44.05	oggedly. carefully ironic ideas after the pending, unusual dolphins wake slyly among the regular deposits. unusual packages 
160008	9	6300	672.02	dolites are never packages. blithely regular foxes sleep carefully across the express deposits; fluffily bold pinto beans sleep closely idly bold pinto beans. blith
162453	2	3986	291.65	rave packages shall have to sublate slyly. pinto beans x-ray alongside of t
162454	3	1840	757.04	n ideas. dependencies would are slyly. bold pinto beans wake blithe deposits; furiously ironic accounts haggle quickly. packages after the carefully bold instructions boost slyly after the fur
162456	5	3669	899.30	ns wake quickly alongside of the furiously final platelets. blithely unusual requests cajole. final theodolites are s
162460	9	6216	76.11	ong the slyly ironic accounts. furiously final packages hang up the waters. carefully daring packages dazzle blithely e
164969	2	3183	734.69	. express foxes among the unusual requests wake carefully for the furiously pending pinto beans. carefully ironic instructions sublate carefully furiously pending the
164970	3	2431	18.37	ons boost blithely bold requests. quickly bold grouches cajole slyly fluffily unusual pinto beans. pending accounts wake furiously regular instructions. slyl
164972	5	5307	837.21	e about the carefully special packages. slyly ironic requests detect blithely. slyly bold warhorses cajole for the qui
164976	9	7252	148.90	ts nag carefully regular theodolites. even, express foxes above the pending foxes haggle blithely enticingly special dolphins. ironic 
167485	2	5758	783.06	ly final accounts haggle slyly about the even packages-- final instructions according to the accounts nod around the 
167486	3	9777	137.46	gular pinto beans haggle quickly along the ironic requests. quiet accounts should have to maintain slyly pen
167488	5	3672	951.12	ctions cajole quickly quickly ironic requests. slyly regular accounts wake furiously across the furiously quick deposits. bli
167492	9	6581	29.12	riously busy accounts cajole near the fluffily ironic excuses. quickly special ideas alongside of the regular, ironic excuses use furiously among the fur
170001	2	5068	612.79	ring accounts lose around the blithely final accounts. slyly bold deposits use furiously according to the ironically final reque
170002	3	3487	508.89	sual excuses are. quickly pending theodolites do sleep fluffily special deposits. ironically express pinto beans detect furiously. blithely pending de
170004	5	720	198.77	 foxes along the theodolites haggle quickly theodolites. requests wake furiously final instructions. quickly express excuses wa
170008	9	131	694.29	dencies detect against the fluffily final ideas. regular deposits try to sleep. fluffy, stealthy requests according to the unusual, bold accounts eat quick
172450	2	7060	831.96	xes along the carefully even asymptotes cajole carefully even courts. carefully regular packages use slyly carefully regular pinto beans. final the
172451	3	845	994.54	luffily slyly ironic packages. carefully regular accounts are quickl
172453	5	2668	234.81	 affix quickly blithely final foxes. furiously special pinto beans sleep carefully among the slyly pending excuses. blithely ironi
172457	9	5165	656.11	eep slyly requests. blithely ironic excuses are according to the carefully ironic pinto beans. furiously ironic ideas about the slyly daring instructions haggle furiously bold tithes. carefull
174967	2	5997	676.95	thely final platelets. carefully bold foxes sublate quickly according to the carefully regular deposits. ideas 
174968	3	3096	696.53	 deposits snooze furiously. carefully ruthless dep
174970	5	9351	979.07	 asymptotes hinder. regular ideas haggle. bold, regular d
174974	9	6140	113.21	 carefully regular accounts eat blithely. never unusual accounts wake about the even, ironic accounts. ironic dolphin
177484	2	2847	393.29	sual, regular requests. packages impress furiously quick somas. slyly pending dugouts are furiously excuses. furiously final instruction
177485	3	2782	703.61	s theodolites. blithely final realms use across the pending, ironic ideas. slyly final ideas wake stealthily regular requests. Tiresias around the express, special instructio
177487	5	2243	98.76	yly regular deposits. unusual, unusual deposits across the even deposits sleep furiously above the regular pinto beans. slyly final deposits above the bold, unusual foxes integrate quic
177491	9	5486	412.11	luffily across the regular instructions; regular deposits after the furiously final accounts boost blithely among the accounts. furiously quiet foxes wake furiously. deposits wake carefully 
180001	2	6212	314.54	furiously ironic instructions. slyly unusual platelets cajole furiously above the slyly silent packages. carefully special
180002	3	9550	909.39	 sleep always across the quickly final requests. regular, regular deposits wake quickly among the regular, final pinto beans. theodolites are carefully over the furiou
180004	5	6852	899.73	requests sleep about the blithely regular courts. quickly final requests cajole slyly according to the instructions
180008	9	741	513.87	o the slyly sly theodolites-- quick deposits affix. deposits integrate slyly ironic, regular excuses. carefully special dependencies above the regular theodolites cajole according to 
182447	2	9999	778.05	y special ideas. bold, express accounts cajole. slyly idle instructions un
182448	3	6859	214.67	ly regular accounts sleep. quickly close pinto beans sleep slyly. final, final excuses sleep fluffily furiously e
182450	5	9005	785.17	old platelets. even, ironic pinto beans wake along the ironic, ironic theodolites. ironic foxes affix slyly bold patt
182454	9	607	273.24	requests haggle quickly after the regular, even platelets. pending, regular theodolites unwind slyly quickly final instructions. final, final ideas are furiously. regular, blithe ideas are blithe
184965	2	9171	197.20	slyly around the slyly even waters. slyly special foxes lose. blithely regular accounts use furiously. carefully express packages nag furiously against t
184966	3	7726	473.96	uriously bold ideas. unusual deposits maintain express packages. blithely regular deposits sleep blithely across the slyly final braids. even packages boost furiou
184968	5	2337	191.76	s. regular packages above the fluffily express deposits sleep carefully carefully ironic packages. bold accounts are carefully about
184972	9	5543	257.34	ubt blithely? slyly regular requests are slyly regular requests. blithely silent theodolites wake carefully across the silent ideas. requests haggle furiously above the regular reque
187483	2	9323	160.25	ven, sly dolphins integrate carefully furiously express deposits! regularly
187484	3	1345	46.45	ents are furiously. final foxes engage. fluffily bold do
187486	5	6255	904.70	efully. fluffily regular epitaphs cajole slowly slyly even accounts. slyly even packages unwind; always even notornis are even epitaphs. request
187490	9	945	146.16	requests boost permanently. slyly regular theodolites boost slyly according to the slyly even deposits. requests sleep alongside of the express, unusual requests. idly bold instructions engage 
190001	2	5290	943.64	ial theodolites play according to the blithely regular deposits. furiously regular accounts cajole carefully above 
190002	3	9245	382.80	accounts use stealthily among the carefully final packages. slyly slow deposits boost finally regular platelets. furiously
190004	5	9900	550.50	onic depths around the fluffily regular requests promise furiously quietly regular instructions. slyly regular accounts haggle ab
190008	9	1591	586.67	ld instructions are carefully after the quickly regular p
192444	2	3194	330.64	ly. carefully unusual packages use alongside of the ironic deposits. stealthily ironic waters use about the regular deco
192445	3	1014	960.42	even attainments are. blithely final accounts about the slyly express foxes haggle express packages. excuses alongside of the furiously ironic requests cajole careful
192447	5	2576	668.75	lly ironic accounts cajole slyly. even, express braids use. slow packages
192451	9	6911	88.99	metimes carefully bold accounts. quickly pending platelets sublate after the regular, express packages. even requests detect. quickly silent sentiments haggle. slyly regular dolphins serve blithe
194963	2	3595	935.75	ld, even courts. furiously final pinto beans are according to
194964	3	6280	498.73	 requests are along the regular theodolites; slyly express decoys are quickly regular requests. unusual, unusual instructions doubt within t
194966	5	7431	465.57	ent frays above the slyly bold theodolites are even dolphins; blithely ironic dependencies
194970	9	292	36.14	ncies. blithely unusual hockey players cajole slyly. even asymptotes sleep stealthily alongside of the regular, special packages. carefully regular theodolites cajole ca
197482	2	4229	779.42	quests; furiously final platelets along the blithely special deposits sleep blithely after the blithely final requests. theodol
197483	3	7574	812.22	ajole quickly. quickly regular accounts wake quickly. requests wake blithely according to the regular, ironic ideas. slyly special
197485	5	1870	749.65	layers. carefully special packages after the quickly bold instructions detect blithely regular theodolites. blithely final accounts after the stealthy dependenc
197489	9	9203	643.67	e carefully even accounts nag quietly regular, pending ideas. bold packages doze slyly. fluffil
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

