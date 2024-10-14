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
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
9154	86513	9022	1	45	67477.95	0.06	0.06	N	O	1997-09-24	1997-08-11	1997-10-14	NONE                     	FOB       	nal, careful instructions wake carefully. b
9154	14212	1716	2	7	7883.47	0.09	0.05	N	O	1997-09-28	1997-09-21	1997-10-27	TAKE BACK RETURN         	SHIP      	nts cajole near 
9154	53311	5817	3	46	58158.26	0.04	0.01	N	O	1997-07-18	1997-08-22	1997-07-31	NONE                     	MAIL      	inal depths. blithely quick deposits n
9154	196620	4178	4	31	53215.22	0.00	0.00	N	O	1997-08-28	1997-07-29	1997-09-07	NONE                     	AIR       	final warthogs. slyly pending request
9154	176947	9465	5	12	24287.28	0.00	0.00	N	O	1997-08-20	1997-07-26	1997-09-17	NONE                     	RAIL      	es. requests print furiously instead of th
9154	140238	2753	6	40	51129.20	0.03	0.02	N	O	1997-06-24	1997-09-03	1997-07-24	COLLECT COD              	TRUCK     	t haggle bli
9154	173448	3449	7	47	71507.68	0.04	0.05	N	O	1997-07-07	1997-09-07	1997-07-25	DELIVER IN PERSON        	SHIP      	wake boldly above the furiousl
24322	178677	8678	1	16	28090.72	0.08	0.01	N	O	1997-03-12	1997-03-22	1997-03-14	COLLECT COD              	FOB       	g, pending waters. even ideas do
24322	9847	4848	2	4	7027.36	0.06	0.03	N	O	1997-05-07	1997-03-04	1997-05-25	COLLECT COD              	RAIL      	sts cajole fluffily silent instruc
24322	84553	2078	3	29	44588.95	0.02	0.02	N	O	1997-03-04	1997-04-08	1997-03-22	NONE                     	TRUCK     	en ideas sleep carefully regu
24322	67250	4769	4	44	53559.00	0.02	0.06	N	O	1997-03-05	1997-03-03	1997-03-22	TAKE BACK RETURN         	RAIL      	 about the fluffily ironic deposits. iro
24322	54437	1953	5	22	30611.46	0.08	0.08	N	O	1997-03-26	1997-03-19	1997-04-19	COLLECT COD              	AIR       	slyly regular ideas affix slyly. slyly fina
24322	109894	4915	6	50	95194.50	0.07	0.03	N	O	1997-04-26	1997-03-12	1997-05-11	NONE                     	FOB       	slyly blithel
36422	154112	6628	1	27	31484.97	0.01	0.07	N	O	1997-05-02	1997-05-01	1997-05-25	TAKE BACK RETURN         	TRUCK     	into beans. blithely regular reque
36422	146555	6556	2	37	59257.35	0.04	0.01	N	O	1997-06-03	1997-04-07	1997-06-17	COLLECT COD              	SHIP      	g deposits are about the as
36422	145767	3310	3	42	76135.92	0.07	0.07	N	O	1997-05-18	1997-05-09	1997-05-28	NONE                     	MAIL      	ly. regular packages sleep. 
36422	90688	689	4	14	23501.52	0.00	0.02	N	O	1997-06-27	1997-05-12	1997-07-23	DELIVER IN PERSON        	SHIP      	eposits. even, regular packages snoo
36422	183949	8986	5	31	63021.14	0.09	0.04	N	O	1997-03-29	1997-05-25	1997-04-19	DELIVER IN PERSON        	REG AIR   	 the quickly stea
36422	17008	2011	6	9	8325.00	0.06	0.05	N	O	1997-06-10	1997-05-14	1997-06-30	TAKE BACK RETURN         	REG AIR   	ajole. fluff
36422	48167	5680	7	27	30109.32	0.06	0.04	N	O	1997-06-18	1997-05-14	1997-07-15	NONE                     	AIR       	kages. blithely final excuses
53283	150623	624	1	48	80333.76	0.06	0.01	N	O	1995-12-28	1996-01-12	1996-01-18	DELIVER IN PERSON        	SHIP      	ding accounts: final courts 
53283	112068	7091	2	7	7560.42	0.00	0.01	N	O	1995-11-18	1996-01-22	1995-11-26	NONE                     	REG AIR   	tes. carefully even hock
53283	122940	477	3	27	52999.38	0.01	0.01	N	O	1995-11-14	1996-01-09	1995-11-21	NONE                     	MAIL      	nticingly bold i
53283	47739	2748	4	50	84336.50	0.09	0.02	N	O	1995-11-28	1996-01-05	1995-12-04	NONE                     	REG AIR   	lites wake quickly fluffily regular
66695	69382	6901	1	33	44595.54	0.00	0.03	N	O	1998-07-10	1998-08-08	1998-07-30	TAKE BACK RETURN         	TRUCK     	ithely even foxes detect carefully bol
66695	97244	7245	2	1	1241.24	0.02	0.08	N	O	1998-07-25	1998-08-16	1998-08-07	COLLECT COD              	RAIL      	 accounts nag carefully ruthless pac
68579	174303	6821	1	10	13773.00	0.06	0.01	N	O	1997-07-18	1997-04-22	1997-07-27	COLLECT COD              	SHIP      	al requests are boldly. c
68579	99088	9089	2	30	32612.40	0.00	0.07	N	O	1997-05-24	1997-05-03	1997-06-02	TAKE BACK RETURN         	RAIL      	raids. furiously even packages wake ac
68579	159862	2378	3	37	71108.82	0.08	0.07	N	O	1997-05-26	1997-04-26	1997-06-16	COLLECT COD              	AIR       	 deposits a
68579	192894	2895	4	22	43711.58	0.09	0.00	N	O	1997-05-29	1997-05-20	1997-06-28	TAKE BACK RETURN         	MAIL      	ording to the close, 
68579	167829	346	5	18	34142.76	0.04	0.02	N	O	1997-06-27	1997-04-18	1997-07-05	DELIVER IN PERSON        	RAIL      	 final requests. regular, exp
68579	93858	3859	6	23	42592.55	0.00	0.06	N	O	1997-06-19	1997-04-19	1997-07-19	DELIVER IN PERSON        	FOB       	s affix blithely after the regular depos
74055	60133	7652	1	34	37166.42	0.00	0.08	R	F	1992-07-25	1992-06-10	1992-08-16	DELIVER IN PERSON        	SHIP      	ses. quickly ironic accounts boost pe
74055	46549	9054	2	8	11964.32	0.09	0.07	A	F	1992-07-20	1992-05-19	1992-07-28	DELIVER IN PERSON        	AIR       	sly express packages. carefully express pa
74055	143640	3641	3	35	58927.40	0.07	0.06	A	F	1992-05-27	1992-06-14	1992-06-18	NONE                     	TRUCK     	ing ideas. bold instructi
74055	140937	5966	4	50	98896.50	0.07	0.03	R	F	1992-07-15	1992-07-05	1992-07-17	DELIVER IN PERSON        	MAIL      	e furiously slyly pending dependenci
74055	114539	4540	5	42	65248.26	0.10	0.08	A	F	1992-07-23	1992-07-08	1992-08-09	DELIVER IN PERSON        	SHIP      	 sleep furiously carefully ironic acc
83684	58826	8827	1	37	66038.34	0.00	0.08	N	O	1998-04-28	1998-04-27	1998-05-01	DELIVER IN PERSON        	TRUCK     	t sleep! permanent depen
160516	112802	336	1	17	30851.60	0.04	0.02	N	O	1995-11-19	1995-11-17	1995-11-25	DELIVER IN PERSON        	REG AIR   	ns! slyly final platelets use carefu
160516	56688	4204	2	49	80589.32	0.04	0.00	N	O	1995-12-30	1995-10-19	1996-01-29	NONE                     	AIR       	w blithely ironi
160516	141906	4421	3	36	70124.40	0.08	0.02	N	O	1995-10-09	1995-11-16	1995-10-24	DELIVER IN PERSON        	SHIP      	the slyly regular pin
160516	198324	5882	4	31	44091.92	0.07	0.00	N	O	1995-12-19	1995-10-24	1996-01-17	COLLECT COD              	FOB       	fluffily final somas are care
164711	82527	52	1	34	51323.68	0.09	0.05	A	F	1992-04-27	1992-07-23	1992-05-22	NONE                     	AIR       	kages. regular i
164711	26184	1189	2	15	16652.70	0.01	0.00	A	F	1992-05-18	1992-06-12	1992-06-11	TAKE BACK RETURN         	TRUCK     	raids doubt aga
164711	67393	9900	3	38	51694.82	0.00	0.00	R	F	1992-07-17	1992-06-11	1992-08-12	DELIVER IN PERSON        	MAIL      	eodolites cajol
164711	65941	8448	4	38	72463.72	0.09	0.04	A	F	1992-07-16	1992-07-05	1992-08-06	TAKE BACK RETURN         	FOB       	y bold courts; blithely
164711	69788	9789	5	34	59764.52	0.04	0.06	R	F	1992-05-09	1992-05-26	1992-05-24	DELIVER IN PERSON        	RAIL      	ccounts detect among the carefu
164711	122878	5391	6	33	62728.71	0.03	0.07	A	F	1992-06-12	1992-06-25	1992-07-03	NONE                     	TRUCK     	cuses use afte
210945	175405	7923	1	27	39970.80	0.01	0.07	R	F	1995-03-31	1995-06-17	1995-04-11	DELIVER IN PERSON        	SHIP      	nts cajole fluffily. quickly final foxes
224167	120715	5740	1	7	12149.97	0.03	0.00	N	O	1996-06-10	1996-07-27	1996-06-22	NONE                     	REG AIR   	unts. furiously sile
224167	55920	931	2	11	20635.12	0.02	0.01	N	O	1996-07-05	1996-07-11	1996-07-17	NONE                     	AIR       	 use blithely
224167	45636	8141	3	18	28469.34	0.07	0.05	N	O	1996-05-26	1996-07-22	1996-05-28	TAKE BACK RETURN         	RAIL      	s. slyly unusual pinto beans boos
224167	199327	1847	4	18	25673.76	0.02	0.00	N	O	1996-08-25	1996-07-12	1996-09-19	TAKE BACK RETURN         	AIR       	ng to the fluffily
269922	196789	9309	1	22	41487.16	0.01	0.07	N	O	1996-06-07	1996-05-28	1996-06-13	DELIVER IN PERSON        	AIR       	r pains alongside of t
269922	29939	2442	2	10	18689.30	0.07	0.04	N	O	1996-04-07	1996-05-01	1996-05-02	TAKE BACK RETURN         	AIR       	ickly spec
269922	173069	621	3	44	50250.64	0.10	0.03	N	O	1996-05-19	1996-06-02	1996-05-28	TAKE BACK RETURN         	SHIP      	refully final theodolites. quickl
269922	164058	6575	4	1	1122.05	0.09	0.08	N	O	1996-06-27	1996-05-05	1996-07-09	TAKE BACK RETURN         	AIR       	bout the s
269922	122096	2097	5	2	2236.18	0.08	0.03	N	O	1996-07-11	1996-05-28	1996-07-30	NONE                     	RAIL      	ar excuses doubt; quickly spec
287366	158993	6539	1	31	63611.69	0.00	0.01	R	F	1994-08-25	1994-08-23	1994-09-06	DELIVER IN PERSON        	SHIP      	ealthy, iron
287366	188836	6391	2	49	94316.67	0.04	0.06	A	F	1994-09-12	1994-09-01	1994-10-06	NONE                     	MAIL      	s. furiously blithe packages wake s
287619	42361	7370	1	11	14336.96	0.05	0.00	N	O	1997-04-14	1997-02-11	1997-05-04	NONE                     	MAIL      	ss the pending platelets wake a
301350	108959	3980	1	42	82653.90	0.05	0.04	N	O	1996-10-23	1996-09-26	1996-11-17	TAKE BACK RETURN         	AIR       	ar courts sleep slyly silent foxes. idea
301350	71106	6121	2	15	16156.50	0.09	0.02	N	O	1996-11-30	1996-11-15	1996-12-06	TAKE BACK RETURN         	REG AIR   	 deposits. u
301350	74045	1567	3	27	27514.08	0.03	0.06	N	O	1996-12-13	1996-10-05	1996-12-21	TAKE BACK RETURN         	RAIL      	ss pinto beans hang 
301350	73310	5818	4	46	59032.26	0.03	0.00	N	O	1996-10-14	1996-10-31	1996-10-30	TAKE BACK RETURN         	AIR       	y carefully bol
301350	117214	9726	5	27	33242.67	0.04	0.03	N	O	1996-10-09	1996-10-27	1996-10-31	COLLECT COD              	FOB       	lar packages caj
301350	3740	8741	6	49	80543.26	0.09	0.00	N	O	1996-10-25	1996-10-09	1996-10-30	COLLECT COD              	TRUCK     	slyly regular packages use blithely a
324740	164764	2313	1	21	38403.96	0.09	0.00	N	O	1997-12-17	1997-12-17	1998-01-01	COLLECT COD              	AIR       	iously even theodolites wake silent packa
324740	171874	6909	2	44	85618.28	0.05	0.03	N	O	1997-10-06	1997-11-25	1997-10-15	TAKE BACK RETURN         	REG AIR   	olites sleep 
324740	69943	9944	3	23	43997.62	0.04	0.04	N	O	1997-12-29	1997-12-10	1998-01-08	DELIVER IN PERSON        	REG AIR   	ly regular requests use quickly. 
330404	85936	8445	1	47	90330.71	0.01	0.03	N	O	1996-11-12	1996-11-16	1996-12-04	TAKE BACK RETURN         	REG AIR   	uickly silent deposit
330404	125929	954	2	16	31278.72	0.07	0.01	N	O	1996-10-14	1996-12-12	1996-10-27	TAKE BACK RETURN         	AIR       	 theodolites. express, sly hockey playe
330404	83371	896	3	46	62301.02	0.01	0.07	N	O	1996-10-19	1996-10-31	1996-11-09	COLLECT COD              	SHIP      	ake above the sometimes ironic deposits. ca
330404	3701	1202	4	37	59373.90	0.09	0.04	N	O	1996-10-03	1996-10-27	1996-10-07	COLLECT COD              	MAIL      	arefully final ins
330404	186149	1186	5	21	25937.94	0.07	0.06	N	O	1996-12-23	1996-12-02	1997-01-13	TAKE BACK RETURN         	MAIL      	he blithely even deposits sleep slyly 
330404	191851	4371	6	20	38857.00	0.05	0.00	N	O	1996-11-17	1996-11-29	1996-12-07	TAKE BACK RETURN         	TRUCK     	bold deposits after the
330404	27398	4905	7	17	22531.63	0.03	0.04	N	O	1996-10-25	1996-11-08	1996-11-09	NONE                     	REG AIR   	ar platelets
337861	30344	7854	1	49	62442.66	0.07	0.03	A	F	1992-07-17	1992-07-06	1992-07-30	DELIVER IN PERSON        	FOB       	 instructions. requests wake slyly against
337861	183084	639	2	26	30344.08	0.02	0.07	A	F	1992-05-15	1992-06-27	1992-06-09	COLLECT COD              	TRUCK     	ounts. carefully final accounts belie
337861	86728	9237	3	40	68588.80	0.08	0.03	R	F	1992-07-13	1992-06-27	1992-07-31	NONE                     	MAIL      	 express packages. i
337861	21456	6461	4	1	1377.45	0.10	0.04	R	F	1992-07-13	1992-07-11	1992-08-12	TAKE BACK RETURN         	FOB       	y special theodol
345858	41910	4415	1	19	35186.29	0.01	0.02	N	O	1998-10-10	1998-08-09	1998-10-27	COLLECT COD              	RAIL      	e even dependencies slee
346693	7798	299	1	27	46056.33	0.08	0.08	R	F	1994-01-06	1994-01-10	1994-01-09	NONE                     	RAIL      	dly across the carefully iro
346693	52353	7364	2	13	16969.55	0.07	0.02	A	F	1994-02-02	1994-01-04	1994-02-15	DELIVER IN PERSON        	FOB       	uses along the carefully special pa
346693	95090	7600	3	4	4340.36	0.02	0.02	A	F	1994-01-11	1994-02-05	1994-01-31	COLLECT COD              	AIR       	usly final asympt
346693	176660	1695	4	1	1736.66	0.03	0.03	R	F	1994-01-15	1994-02-02	1994-01-17	NONE                     	SHIP      	 unusual foxes boost slyly. final packag
358886	9362	4363	1	44	55939.84	0.02	0.07	N	O	1995-09-12	1995-11-22	1995-09-20	COLLECT COD              	FOB       	ronic foxes. carefully express fox
358886	143341	3342	2	30	41530.20	0.10	0.03	N	O	1995-12-20	1995-11-10	1996-01-17	COLLECT COD              	TRUCK     	 even requests above the furiously final d
358886	124350	9375	3	23	31610.05	0.10	0.03	N	O	1995-11-01	1995-10-20	1995-11-12	NONE                     	MAIL      	r requests thrash along the car
358886	139098	6638	4	42	47757.78	0.09	0.08	N	O	1995-10-03	1995-10-09	1995-10-11	COLLECT COD              	SHIP      	eep slyly according to t
358886	17003	4507	5	38	34960.00	0.10	0.07	N	O	1995-11-02	1995-11-14	1995-11-26	TAKE BACK RETURN         	REG AIR   	late furious
374723	89641	4658	1	4	6522.56	0.06	0.03	N	O	1996-11-25	1996-12-29	1996-12-11	COLLECT COD              	TRUCK     	d packages haggle. ca
374723	184969	6	2	44	90374.24	0.03	0.06	N	O	1997-02-02	1996-12-22	1997-02-03	NONE                     	FOB       	ully regul
374723	165565	5566	3	16	26088.96	0.10	0.01	N	O	1996-12-11	1997-01-18	1997-01-02	NONE                     	SHIP      	 the regular pinto be
374723	36827	1834	4	25	44095.50	0.04	0.02	N	O	1997-03-04	1997-01-17	1997-03-18	COLLECT COD              	RAIL      	the regular requests eat a
374723	146624	1653	5	48	80189.76	0.03	0.05	N	O	1997-02-26	1996-12-29	1997-03-05	COLLECT COD              	REG AIR   	ronic accounts cajole regularly
374723	105764	785	6	17	30085.92	0.06	0.03	N	O	1997-03-14	1997-01-15	1997-03-29	NONE                     	REG AIR   	 theodolites. ca
385825	161438	6471	1	47	70473.21	0.01	0.07	N	O	1996-01-17	1995-12-27	1996-02-06	DELIVER IN PERSON        	RAIL      	rses. unusual, special foxes use. f
385825	74506	7014	2	2	2961.00	0.00	0.06	N	O	1995-12-17	1996-01-15	1996-01-11	NONE                     	FOB       	eas according to the regular accounts 
385825	10379	5382	3	40	51574.80	0.09	0.03	N	O	1996-02-03	1996-01-06	1996-02-24	COLLECT COD              	SHIP      	 carefully pending accounts. furiousl
385825	32887	5391	4	28	50956.64	0.03	0.07	N	O	1995-11-05	1995-12-17	1995-11-29	NONE                     	FOB       	s hang slyly unusual accounts. re
385825	118088	3111	5	22	24333.76	0.10	0.00	N	O	1995-12-11	1996-01-06	1995-12-28	NONE                     	MAIL      	 warhorses hinder slyly a
385825	88806	3823	6	16	28716.80	0.05	0.08	N	O	1995-12-25	1995-12-27	1996-01-05	TAKE BACK RETURN         	REG AIR   	iously final theodolites boost 
385825	148780	8781	7	28	51205.84	0.08	0.00	N	O	1996-02-15	1996-01-10	1996-02-22	TAKE BACK RETURN         	MAIL      	arefully even 
413985	83259	784	1	23	28571.75	0.10	0.06	N	O	1995-07-10	1995-08-28	1995-07-23	NONE                     	AIR       	y regular packages cajole 
413985	126530	4067	2	27	42026.31	0.06	0.06	N	O	1995-10-07	1995-09-13	1995-11-05	TAKE BACK RETURN         	FOB       	g to the regularly special requests! 
4639746	41432	1433	3	4	5493.72	0.00	0.02	N	O	1995-11-05	1995-11-08	1995-11-16	TAKE BACK RETURN         	TRUCK     	leep after 
413985	184587	2142	3	49	81907.42	0.03	0.00	N	O	1995-08-08	1995-09-19	1995-08-22	COLLECT COD              	FOB       	ithely special hock
413985	144112	6627	4	8	9248.88	0.07	0.01	N	O	1995-08-23	1995-09-14	1995-08-29	COLLECT COD              	MAIL      	riously unusual realms against the furious
413985	75956	971	5	4	7727.80	0.05	0.00	N	O	1995-09-11	1995-09-27	1995-10-03	DELIVER IN PERSON        	TRUCK     	hely along the final pinto beans. ironic 
413985	189180	4217	6	15	19037.70	0.01	0.03	N	O	1995-07-30	1995-08-09	1995-08-01	NONE                     	SHIP      	 packages. blithely 
413985	134213	9240	7	40	49888.40	0.08	0.03	N	O	1995-11-01	1995-09-12	1995-11-11	COLLECT COD              	SHIP      	the ruthlessly unusual
440964	5172	2673	1	46	49549.82	0.00	0.01	N	O	1997-03-26	1997-03-13	1997-04-17	TAKE BACK RETURN         	TRUCK     	 even dependencies are close
440964	115268	2802	2	45	57746.70	0.05	0.01	N	O	1997-02-17	1997-04-03	1997-03-06	DELIVER IN PERSON        	MAIL      	 beans. even packages wake quiet
440964	108727	3748	3	1	1735.72	0.00	0.02	N	O	1997-03-16	1997-03-24	1997-04-15	DELIVER IN PERSON        	TRUCK     	efully alongside of the u
440964	164834	9867	4	33	62661.39	0.04	0.04	N	O	1997-02-17	1997-02-18	1997-03-05	COLLECT COD              	REG AIR   	te regular, even ins
446499	20175	2678	1	15	16427.55	0.02	0.03	N	O	1997-04-25	1997-05-11	1997-05-12	COLLECT COD              	SHIP      	final asym
446499	79009	1517	2	1	988.00	0.10	0.07	N	O	1997-04-11	1997-05-01	1997-05-09	TAKE BACK RETURN         	MAIL      	ithely sly, unusua
491620	107275	2296	1	5	6411.35	0.03	0.02	N	O	1998-07-03	1998-07-21	1998-07-14	COLLECT COD              	MAIL      	ckly ironic ex
491620	155827	858	2	14	26359.48	0.05	0.05	N	O	1998-09-20	1998-07-30	1998-10-05	DELIVER IN PERSON        	SHIP      	 sleep carefully. dugouts
491620	169575	7124	3	17	27957.69	0.09	0.08	N	O	1998-09-04	1998-07-06	1998-09-07	NONE                     	TRUCK     	y even requests. slyly unusual the
491620	43165	3166	4	48	53191.68	0.00	0.06	N	O	1998-08-23	1998-07-05	1998-09-04	COLLECT COD              	SHIP      	tions affix 
512195	100451	5472	1	46	66766.70	0.03	0.05	N	O	1996-10-16	1996-09-23	1996-11-15	NONE                     	TRUCK     	e carefully acro
545218	128961	6498	1	36	71638.56	0.02	0.04	R	F	1992-11-03	1992-09-16	1992-11-26	COLLECT COD              	AIR       	es. blithely regular packages a
545218	46992	2001	2	31	60108.69	0.00	0.02	R	F	1992-10-01	1992-08-22	1992-10-11	TAKE BACK RETURN         	RAIL      	 bold, bold asymptotes.
545218	189004	1523	3	1	1093.00	0.04	0.02	R	F	1992-08-06	1992-09-11	1992-09-03	NONE                     	TRUCK     	g to the slyly even requests. fluffily even
545218	180170	7725	4	33	41255.61	0.01	0.01	R	F	1992-08-25	1992-10-09	1992-09-16	NONE                     	SHIP      	oys affix express,
545218	107318	9829	5	32	42409.92	0.07	0.02	A	F	1992-09-10	1992-09-10	1992-09-28	COLLECT COD              	REG AIR   	careful instructio
545218	80043	44	6	8	8184.32	0.10	0.06	R	F	1992-10-24	1992-10-07	1992-11-10	DELIVER IN PERSON        	MAIL      	riously final excuses are quickly regula
593664	171025	6060	1	8	8768.16	0.03	0.08	N	O	1996-02-05	1995-11-30	1996-02-13	TAKE BACK RETURN         	AIR       	outs. furiously express id
593664	2121	7122	2	35	35809.20	0.00	0.08	N	O	1995-10-14	1995-12-26	1995-11-06	NONE                     	RAIL      	deposits detect carefull
593664	43584	3585	3	45	68741.10	0.05	0.08	N	O	1995-10-19	1995-11-24	1995-11-09	COLLECT COD              	MAIL      	l platelets. even cour
593664	106699	4230	4	27	46053.63	0.07	0.00	N	O	1995-12-05	1995-12-20	1995-12-07	TAKE BACK RETURN         	RAIL      	arefully ironic
593664	82999	524	5	8	15855.92	0.00	0.03	N	O	1996-01-24	1995-12-21	1996-01-25	TAKE BACK RETURN         	SHIP      	. even pinto beans cajole alongs
593664	160459	460	6	6	9116.70	0.00	0.07	N	O	1995-12-25	1995-12-12	1996-01-24	COLLECT COD              	FOB       	ly unusual packages sleep furious
681730	39773	7283	1	30	51383.10	0.01	0.07	A	F	1993-08-15	1993-09-29	1993-08-27	DELIVER IN PERSON        	SHIP      	onic packages. slyly regular acc
681730	50246	2752	2	22	26317.28	0.06	0.02	R	F	1993-08-22	1993-10-19	1993-09-21	NONE                     	REG AIR   	y special theodolites. slyly c
681730	74535	2057	3	4	6038.12	0.08	0.08	R	F	1993-10-07	1993-10-24	1993-11-04	DELIVER IN PERSON        	SHIP      	lly. express a
681730	102310	9841	4	3	3936.93	0.08	0.06	A	F	1993-08-29	1993-10-18	1993-09-15	DELIVER IN PERSON        	REG AIR   	se blithely pe
681730	119483	4506	5	7	10517.36	0.02	0.05	A	F	1993-11-20	1993-09-21	1993-12-15	TAKE BACK RETURN         	TRUCK     	ackages wake furiously ag
681730	143977	9006	6	48	97006.56	0.01	0.03	A	F	1993-12-07	1993-10-07	1993-12-17	COLLECT COD              	REG AIR   	 quickly regular ideas are slyly. b
686918	129947	2460	1	33	65239.02	0.02	0.06	R	F	1992-09-29	1992-11-15	1992-10-10	DELIVER IN PERSON        	MAIL      	daringly about the carefully ir
686918	16775	4279	2	27	45677.79	0.06	0.02	R	F	1992-09-21	1992-09-22	1992-09-23	TAKE BACK RETURN         	AIR       	ructions along the dependencies boost
686918	121463	6488	3	35	51956.10	0.03	0.07	A	F	1992-10-05	1992-09-25	1992-10-19	TAKE BACK RETURN         	SHIP      	ar orbits.
686918	20341	2844	4	28	35317.52	0.09	0.03	A	F	1992-08-25	1992-11-12	1992-09-07	COLLECT COD              	SHIP      	ular theodolites
708706	75429	2951	1	38	53367.96	0.07	0.04	N	O	1996-05-07	1996-05-06	1996-05-25	TAKE BACK RETURN         	AIR       	hely regular packages detec
708706	129579	9580	2	33	53082.81	0.07	0.02	N	O	1996-07-02	1996-06-02	1996-07-07	COLLECT COD              	REG AIR   	boost. pending instructions b
708706	32509	2510	3	44	63426.00	0.01	0.04	N	O	1996-05-01	1996-06-03	1996-05-04	COLLECT COD              	MAIL      	y sly frets. theodolites are carefu
708706	171107	6142	4	33	38877.30	0.00	0.05	N	O	1996-07-07	1996-06-02	1996-07-14	COLLECT COD              	MAIL      	y. blithely unusual fo
708706	188912	6467	5	37	74033.67	0.09	0.07	N	O	1996-06-07	1996-04-24	1996-06-22	TAKE BACK RETURN         	TRUCK     	s integrate against the eve
724805	162696	2697	1	21	36932.49	0.03	0.08	R	F	1993-06-14	1993-06-23	1993-06-22	TAKE BACK RETURN         	TRUCK     	ckly regular platelets. requests r
724805	21323	6328	2	36	44795.52	0.04	0.01	A	F	1993-07-10	1993-05-29	1993-07-28	TAKE BACK RETURN         	RAIL      	s are. regul
724805	161483	4000	3	22	33978.56	0.04	0.00	R	F	1993-07-25	1993-05-18	1993-08-21	TAKE BACK RETURN         	AIR       	haggle fluffily. carefully final waters imp
724805	39915	7425	4	15	27823.65	0.00	0.04	R	F	1993-05-04	1993-06-03	1993-05-18	DELIVER IN PERSON        	REG AIR   	arefully regular r
724805	48214	719	5	10	11622.10	0.07	0.06	A	F	1993-07-23	1993-06-08	1993-08-17	NONE                     	RAIL      	the regular, regular package
724805	126671	1696	6	9	15279.03	0.06	0.00	A	F	1993-04-26	1993-05-28	1993-05-21	COLLECT COD              	FOB       	ar packages after the sly
737345	195713	5714	1	27	48835.17	0.01	0.05	N	O	1996-12-07	1996-11-26	1996-12-28	NONE                     	AIR       	inal accou
774304	141100	1101	1	43	49067.30	0.01	0.07	A	F	1995-02-13	1995-02-19	1995-02-14	NONE                     	SHIP      	y final packages wa
774304	66772	6773	2	33	57379.41	0.03	0.03	R	F	1995-05-09	1995-04-03	1995-05-16	NONE                     	AIR       	arefully even foxes do maintain bl
816323	149483	4512	1	12	18389.76	0.04	0.08	N	O	1996-05-21	1996-03-10	1996-06-13	DELIVER IN PERSON        	REG AIR   	ronic platelets. ironic, ironic
816323	153105	3106	2	20	23162.00	0.03	0.06	N	O	1996-03-06	1996-04-17	1996-04-02	NONE                     	RAIL      	 the unusual theodolites. carefully bold d
816323	33413	923	3	40	53856.40	0.05	0.08	N	O	1996-03-13	1996-03-29	1996-04-07	COLLECT COD              	TRUCK     	lly pending packages wake f
816323	161159	3676	4	25	30503.75	0.06	0.06	N	O	1996-04-14	1996-03-12	1996-05-01	NONE                     	TRUCK     	thely ironic foxes cajole
816323	182701	2702	5	36	64213.20	0.04	0.03	N	O	1996-03-13	1996-02-29	1996-03-31	TAKE BACK RETURN         	RAIL      	arefully express accounts 
816323	42686	7695	6	21	34202.28	0.03	0.07	N	O	1996-03-14	1996-02-24	1996-03-24	TAKE BACK RETURN         	SHIP      	r the carefully final somas; 
816323	16221	3725	7	19	21607.18	0.04	0.04	N	O	1996-02-05	1996-03-04	1996-02-15	COLLECT COD              	REG AIR   	round the carefully
854375	147052	9567	1	47	51655.35	0.01	0.04	A	F	1993-11-13	1993-12-11	1993-12-04	TAKE BACK RETURN         	AIR       	ajole carefully pending accou
854375	102396	7417	2	48	67122.72	0.00	0.06	R	F	1994-01-08	1993-11-29	1994-01-27	TAKE BACK RETURN         	AIR       	 courts haggle according to the iro
854375	158591	3622	3	3	4948.77	0.06	0.06	R	F	1993-11-23	1993-11-26	1993-12-01	TAKE BACK RETURN         	AIR       	 are blithely! care
854375	33538	3539	4	16	23544.48	0.06	0.04	A	F	1993-11-01	1993-12-09	1993-11-04	COLLECT COD              	RAIL      	ckages. slyly bold platelets detect along
854375	71320	3828	5	9	11621.88	0.01	0.00	R	F	1993-11-07	1993-11-06	1993-11-12	NONE                     	REG AIR   	posits cajole carefully slyly 
859108	142807	7836	1	40	73992.00	0.05	0.00	N	O	1996-03-31	1996-03-25	1996-04-21	COLLECT COD              	REG AIR   	ully bold theodolites according to 
859108	146837	4380	2	33	62166.39	0.07	0.07	N	O	1996-04-03	1996-03-25	1996-04-07	NONE                     	FOB       	regular frays. furiou
859108	89079	9080	3	3	3204.21	0.07	0.06	N	O	1996-05-15	1996-05-12	1996-05-20	DELIVER IN PERSON        	MAIL      	final platelets are. furiously expre
859235	59047	1553	1	23	23138.92	0.03	0.02	N	O	1995-11-14	1995-09-29	1995-12-05	TAKE BACK RETURN         	AIR       	eans. idly ironic ideas among the f
859235	190604	5643	2	2	3389.20	0.00	0.04	N	O	1995-11-24	1995-11-10	1995-12-11	NONE                     	FOB       	ess packages wake quickl
869574	58065	8066	1	8	8184.48	0.01	0.07	N	O	1998-04-28	1998-04-15	1998-05-06	COLLECT COD              	SHIP      	 deposits nod blithely. furi
869574	88153	8154	2	4	4564.60	0.09	0.08	N	O	1998-02-13	1998-04-15	1998-03-08	NONE                     	RAIL      	lites will have to integ
869574	181701	4220	3	6	10696.20	0.01	0.04	N	O	1998-02-01	1998-04-01	1998-03-03	NONE                     	FOB       	ideas cajole blithely
869574	37815	2822	4	8	14022.48	0.08	0.01	N	O	1998-02-01	1998-02-25	1998-03-02	TAKE BACK RETURN         	RAIL      	to the slyly sile
869574	151682	6713	5	17	29472.56	0.08	0.04	N	O	1998-04-23	1998-04-21	1998-05-05	NONE                     	REG AIR   	lly regular plat
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
917893	166283	3832	1	37	49923.36	0.08	0.06	N	O	1998-10-04	1998-09-19	1998-10-08	COLLECT COD              	RAIL      	ckages! final deposi
917893	107905	416	2	39	74603.10	0.05	0.04	N	O	1998-07-24	1998-10-10	1998-07-29	COLLECT COD              	MAIL      	s. furiously bo
917893	143237	780	3	6	7681.38	0.09	0.01	N	O	1998-09-19	1998-09-02	1998-10-09	NONE                     	SHIP      	ts up the pinto bea
917893	179384	1902	4	44	64388.72	0.07	0.02	N	O	1998-07-19	1998-10-11	1998-07-26	DELIVER IN PERSON        	REG AIR   	, ironic requests sleep blithely carefully 
917893	100161	2672	5	20	23223.20	0.04	0.06	N	O	1998-10-03	1998-09-04	1998-11-02	COLLECT COD              	REG AIR   	 ironic packages cajole idly even pl
954660	35478	485	1	38	53711.86	0.10	0.08	R	F	1995-01-09	1995-02-28	1995-02-01	COLLECT COD              	REG AIR   	te at the unusual, regular pinto bea
954660	67295	7296	2	28	35344.12	0.09	0.04	A	F	1995-01-09	1995-01-23	1995-01-20	NONE                     	FOB       	ructions affix. even grouches sleep slyly
954660	14213	4214	3	46	51851.66	0.04	0.01	R	F	1995-02-19	1995-01-17	1995-02-23	DELIVER IN PERSON        	RAIL      	 blithely ironic d
954660	66829	9336	4	50	89791.00	0.08	0.02	R	F	1995-03-09	1995-01-12	1995-04-01	NONE                     	REG AIR   	ges. blithely ev
969474	176062	1097	1	12	13656.72	0.02	0.08	R	F	1992-10-16	1992-10-26	1992-11-08	NONE                     	AIR       	iously sly
1035265	158738	3769	1	15	26950.95	0.08	0.04	R	F	1992-08-31	1992-09-08	1992-09-29	NONE                     	FOB       	press deposits must cajole quietly final
1037540	115089	5090	1	23	25393.84	0.10	0.06	A	F	1992-02-29	1992-05-03	1992-03-01	NONE                     	TRUCK     	ths cajole slyly furiousl
1037540	96008	8518	2	42	42168.00	0.01	0.05	R	F	1992-03-30	1992-05-15	1992-04-10	DELIVER IN PERSON        	REG AIR   	eposits integrate about the pending pa
1037540	3592	3593	3	47	70292.73	0.10	0.07	R	F	1992-05-03	1992-04-06	1992-05-29	COLLECT COD              	RAIL      	egular dep
1037540	165638	671	4	48	81774.24	0.09	0.00	R	F	1992-03-27	1992-03-21	1992-04-05	NONE                     	RAIL      	beans use quickly furiously unusu
1066274	188161	680	1	19	23734.04	0.01	0.00	A	F	1994-04-30	1994-07-20	1994-05-19	NONE                     	TRUCK     	st the packages. requ
1066274	7752	253	2	46	76348.50	0.05	0.08	A	F	1994-08-06	1994-06-26	1994-08-07	NONE                     	FOB       	ual theodolites. patterns was ins
1127620	65369	382	1	17	22684.12	0.08	0.03	N	O	1997-08-21	1997-08-01	1997-08-29	DELIVER IN PERSON        	TRUCK     	nts above the theodolites affix after the 
1127620	48936	3945	2	21	39583.53	0.03	0.02	N	O	1997-08-31	1997-07-03	1997-09-26	TAKE BACK RETURN         	FOB       	s. slyly regular d
1127620	56213	1224	3	48	56122.08	0.03	0.07	N	O	1997-05-26	1997-07-25	1997-06-25	COLLECT COD              	SHIP      	thy accounts impress blithely bl
1127620	160807	8356	4	23	42959.40	0.08	0.08	N	O	1997-07-28	1997-08-02	1997-08-25	COLLECT COD              	AIR       	elets. deposits sleep slyly. ins
1127620	81801	1802	5	35	62398.00	0.07	0.02	N	O	1997-07-03	1997-07-19	1997-07-28	NONE                     	FOB       	unusual pinto beans against 
1127620	28410	913	6	11	14722.51	0.05	0.06	N	O	1997-06-06	1997-07-28	1997-07-05	NONE                     	MAIL      	st the unusual, 
1131267	180569	570	1	35	57734.60	0.06	0.03	A	F	1992-12-13	1993-01-12	1992-12-16	DELIVER IN PERSON        	TRUCK     	its. carefully sp
1131267	6982	1983	2	13	24556.74	0.05	0.08	A	F	1993-01-24	1993-02-09	1993-02-06	TAKE BACK RETURN         	TRUCK     	 deposits after the fluffil
1131267	146207	1236	3	40	50128.00	0.06	0.02	R	F	1993-02-20	1993-01-12	1993-03-10	DELIVER IN PERSON        	FOB       	y unusual accou
1131267	86854	9363	4	4	7363.40	0.10	0.04	A	F	1993-03-12	1993-02-11	1993-03-20	NONE                     	FOB       	p finally blithely final th
1131267	150338	2854	5	29	40261.57	0.04	0.00	R	F	1993-01-05	1993-02-13	1993-01-19	NONE                     	MAIL      	bove the ironic, p
1168417	157154	7155	1	50	60557.50	0.00	0.05	N	O	1996-11-13	1996-11-18	1996-11-20	COLLECT COD              	TRUCK     	jole carefully blithely pending sheave
1168417	72784	306	2	35	61487.30	0.07	0.07	N	O	1997-01-28	1996-11-24	1997-01-29	TAKE BACK RETURN         	FOB       	ly pending tithes detect quickly fluff
1168417	65387	400	3	30	40571.40	0.00	0.07	N	O	1996-11-14	1996-11-28	1996-11-16	COLLECT COD              	RAIL      	ic platelets. requests are. fu
1168417	145367	396	4	46	64968.56	0.01	0.00	N	O	1996-12-20	1996-12-12	1997-01-02	COLLECT COD              	AIR       	l deposits w
1168417	181477	3996	5	33	51429.51	0.08	0.01	N	O	1997-01-17	1996-11-12	1997-02-08	COLLECT COD              	FOB       	ajole against the qui
1168417	67991	3004	6	18	35261.82	0.04	0.04	N	O	1997-01-18	1996-12-15	1997-01-30	TAKE BACK RETURN         	REG AIR   	 furiously across the final, 
1177350	43191	704	1	25	28354.75	0.04	0.06	N	O	1997-09-17	1997-08-22	1997-10-05	COLLECT COD              	MAIL      	ly. quickly ironic depe
1192231	103367	3368	1	29	39740.44	0.01	0.07	N	O	1996-06-06	1996-08-24	1996-06-15	NONE                     	MAIL      	across the blithe
1192231	86763	1780	2	9	15747.84	0.01	0.05	N	O	1996-09-25	1996-07-22	1996-10-09	TAKE BACK RETURN         	TRUCK     	careful accounts. even ideas haggle aga
1192231	10325	326	3	15	18529.80	0.04	0.03	N	O	1996-09-25	1996-07-31	1996-09-30	COLLECT COD              	REG AIR   	 the furiously furious deposits wake fur
1192231	117347	4881	4	2	2728.68	0.08	0.04	N	O	1996-07-10	1996-07-24	1996-07-12	NONE                     	MAIL      	osits. fluffily regular courts sle
1192231	43478	8487	5	26	36958.22	0.01	0.03	N	O	1996-06-19	1996-08-18	1996-06-23	NONE                     	TRUCK     	fully regular instructions
1192231	120368	369	6	19	26378.84	0.02	0.04	N	O	1996-07-14	1996-08-12	1996-07-16	DELIVER IN PERSON        	AIR       	bold packages haggle furiously against th
1198339	188824	6379	1	17	32517.94	0.06	0.05	N	O	1996-05-08	1996-04-12	1996-05-25	TAKE BACK RETURN         	FOB       	al theodoli
1198339	152969	2970	2	35	70768.60	0.08	0.06	N	O	1996-04-27	1996-03-31	1996-04-29	DELIVER IN PERSON        	TRUCK     	ns haggle qu
1216773	138536	6076	1	11	17319.83	0.06	0.08	N	O	1995-10-20	1995-09-27	1995-10-25	TAKE BACK RETURN         	FOB       	s are after the furiously regu
1216773	163000	8033	2	33	35079.00	0.05	0.07	N	O	1995-10-25	1995-10-08	1995-11-07	DELIVER IN PERSON        	RAIL      	l pinto beans. quickly even d
1216773	129800	4825	3	47	86000.60	0.07	0.08	N	O	1995-09-10	1995-11-10	1995-09-11	DELIVER IN PERSON        	TRUCK     	ar foxes haggle fluffily. bold plate
1216773	148402	5945	4	35	50764.00	0.00	0.03	N	O	1995-08-25	1995-11-07	1995-09-03	COLLECT COD              	FOB       	ns wake along the fl
1216773	181693	1694	5	25	44367.25	0.10	0.00	N	O	1995-11-07	1995-10-31	1995-11-12	DELIVER IN PERSON        	RAIL      	accounts cajole quickly. furiou
1226497	101559	4070	1	32	49937.60	0.08	0.06	R	F	1993-11-02	1993-12-04	1993-11-22	NONE                     	MAIL      	. final, unusual deposits cajole slowly. i
1226497	98547	6075	2	27	41729.58	0.06	0.01	R	F	1994-01-09	1993-12-10	1994-01-15	TAKE BACK RETURN         	RAIL      	e quickly along th
1247233	109070	6601	1	43	46400.01	0.00	0.00	R	F	1992-07-23	1992-08-01	1992-08-20	NONE                     	RAIL      	ideas: quickly pending dugouts wake a
1247233	122886	2887	2	46	87808.48	0.02	0.07	R	F	1992-08-01	1992-07-20	1992-08-29	NONE                     	TRUCK     	ckly even dependencies
1376704	41339	3844	1	13	16644.29	0.07	0.05	R	F	1994-05-24	1994-08-02	1994-06-06	DELIVER IN PERSON        	MAIL      	 the slyly pending deposits. requests
1376704	173469	8504	2	50	77123.00	0.03	0.00	A	F	1994-05-25	1994-07-07	1994-06-02	COLLECT COD              	RAIL      	ffily regular dolphins. careful
1376704	81636	9161	3	14	22646.82	0.01	0.08	R	F	1994-07-25	1994-07-06	1994-08-06	COLLECT COD              	SHIP      	ilent packages before the furiously i
1376704	51950	1951	4	4	7607.80	0.08	0.07	R	F	1994-08-10	1994-07-13	1994-08-18	NONE                     	MAIL      	latelets. quickly speci
1376704	107035	9546	5	7	7294.21	0.09	0.04	R	F	1994-05-26	1994-06-06	1994-06-07	NONE                     	MAIL      	ackages al
1376704	122717	5230	6	7	12177.97	0.06	0.03	R	F	1994-05-29	1994-07-21	1994-06-28	NONE                     	RAIL      	ng to the carefully regular d
1376704	95668	3196	7	32	53237.12	0.05	0.06	R	F	1994-06-24	1994-06-11	1994-07-04	DELIVER IN PERSON        	TRUCK     	nts nag. slyly ir
1468323	22719	2720	1	47	77160.37	0.09	0.08	R	F	1995-01-17	1994-12-21	1995-02-08	TAKE BACK RETURN         	FOB       	sits cajole blithely
1468323	24873	9878	2	7	12585.09	0.09	0.04	A	F	1994-12-02	1994-12-20	1994-12-15	COLLECT COD              	TRUCK     	ss the fluffily final instructions. depos
1468323	23851	6354	3	45	79868.25	0.02	0.00	A	F	1994-12-15	1995-02-11	1994-12-17	DELIVER IN PERSON        	TRUCK     	lly across the carefully ca
1468323	57070	2081	4	30	30812.10	0.00	0.05	R	F	1995-02-21	1994-12-22	1995-03-09	TAKE BACK RETURN         	AIR       	equests affix fluffily! qu
1469089	84876	2401	1	14	26052.18	0.10	0.04	A	F	1993-10-14	1993-09-30	1993-10-16	DELIVER IN PERSON        	FOB       	counts. slyly unusual accounts
1469089	96036	1055	2	1	1032.03	0.02	0.07	R	F	1993-10-27	1993-08-25	1993-10-29	TAKE BACK RETURN         	AIR       	y regular deposits. carefully express d
1469089	62103	2104	3	26	27692.60	0.04	0.01	A	F	1993-08-04	1993-10-02	1993-08-15	DELIVER IN PERSON        	TRUCK     	 wake across 
1469089	159572	9573	4	42	68525.94	0.04	0.08	A	F	1993-11-19	1993-09-15	1993-11-20	NONE                     	RAIL      	uctions nag quickly blithely even request
1469089	111303	8837	5	11	14457.30	0.07	0.01	R	F	1993-08-29	1993-10-13	1993-09-27	DELIVER IN PERSON        	TRUCK     	 express warthogs use along the fluf
1478917	176458	8976	1	35	53705.75	0.04	0.00	R	F	1992-10-27	1992-11-05	1992-11-19	COLLECT COD              	REG AIR   	lites cajole pending accounts. careful
1478917	122004	2005	2	20	20520.00	0.08	0.04	R	F	1992-10-24	1992-11-05	1992-11-21	COLLECT COD              	SHIP      	 final excuses? silent p
1478917	114560	4561	3	8	12596.48	0.05	0.05	A	F	1992-11-17	1992-11-27	1992-12-15	NONE                     	FOB       	s. final, final courts cajole across the ev
1478917	199779	4818	4	40	75150.80	0.10	0.00	A	F	1993-01-19	1993-01-01	1993-02-06	DELIVER IN PERSON        	MAIL      	ly ironic packages boost carefully even 
1478917	78733	8734	5	10	17117.30	0.00	0.02	R	F	1992-11-21	1992-12-05	1992-12-20	NONE                     	SHIP      	ious Tiresias? blithely 
1490087	104957	4958	1	22	43162.90	0.02	0.06	N	O	1996-09-16	1996-09-30	1996-10-13	COLLECT COD              	RAIL      	nag; carefully i
1490087	6676	9177	2	45	71220.15	0.02	0.02	N	O	1996-08-26	1996-08-10	1996-09-14	DELIVER IN PERSON        	AIR       	c, ironic requests haggle blithely
1490087	194371	9410	3	14	20515.18	0.08	0.05	N	O	1996-09-09	1996-08-13	1996-10-09	TAKE BACK RETURN         	SHIP      	cajole fluffily about the instruc
1490087	180589	8144	4	30	50087.40	0.04	0.07	N	O	1996-07-24	1996-09-16	1996-08-23	TAKE BACK RETURN         	SHIP      	old dolphins. quickly ironic accoun
1490087	155076	107	5	36	40718.52	0.05	0.04	N	O	1996-10-31	1996-08-09	1996-11-26	TAKE BACK RETURN         	REG AIR   	al theodolites haggle.
1521157	163726	1275	1	50	89486.00	0.05	0.00	N	O	1997-11-22	1997-11-15	1997-12-04	TAKE BACK RETURN         	RAIL      	lar accounts. pending requests wake r
1521157	132776	316	2	22	39792.94	0.00	0.00	N	O	1997-11-11	1997-11-03	1997-12-01	TAKE BACK RETURN         	SHIP      	luffily final pla
1521157	151358	3874	3	39	54964.65	0.02	0.01	N	O	1997-10-25	1997-10-06	1997-11-20	COLLECT COD              	REG AIR   	y unusual pint
1550053	198714	1234	1	43	77946.53	0.09	0.06	A	F	1993-12-05	1994-01-28	1993-12-14	NONE                     	SHIP      	ic excuses against the ex
1550053	177380	7381	2	20	29147.60	0.07	0.00	R	F	1994-02-22	1994-01-18	1994-03-13	TAKE BACK RETURN         	REG AIR   	uternes nag quickly alongside of th
1550053	9066	9067	3	14	13650.84	0.02	0.07	R	F	1994-02-27	1993-12-04	1994-03-27	TAKE BACK RETURN         	FOB       	 accounts. slyly bol
1550053	167409	9926	4	32	47244.80	0.09	0.00	A	F	1994-01-28	1993-12-08	1994-02-18	TAKE BACK RETURN         	MAIL      	uickly idle
1550053	35104	5105	5	30	31173.00	0.10	0.06	R	F	1994-01-28	1993-12-07	1994-02-11	DELIVER IN PERSON        	RAIL      	quickly regular requ
1572644	87521	5046	1	41	61849.32	0.00	0.04	N	O	1998-09-21	1998-08-18	1998-10-15	COLLECT COD              	SHIP      	carefully regula
1572644	153628	3629	2	43	72309.66	0.07	0.05	N	O	1998-06-12	1998-07-30	1998-07-04	COLLECT COD              	AIR       	ual foxes. ironic accounts ha
1572644	10274	275	3	19	22501.13	0.04	0.04	N	O	1998-09-28	1998-08-29	1998-10-08	COLLECT COD              	SHIP      	slyly across t
1572644	71673	4181	4	40	65786.80	0.07	0.07	N	O	1998-09-05	1998-08-18	1998-09-11	COLLECT COD              	AIR       	lent deposits! even packages 
1572644	6703	4204	5	14	22535.80	0.07	0.06	N	O	1998-09-05	1998-07-19	1998-09-25	TAKE BACK RETURN         	FOB       	telets. final platelets haggle silent p
1572644	40641	5650	6	18	28469.52	0.03	0.02	N	O	1998-07-12	1998-08-26	1998-08-07	TAKE BACK RETURN         	REG AIR   	ter the ideas. regular pinto beans abou
1590469	21835	1836	1	12	21081.96	0.05	0.02	N	O	1997-07-06	1997-05-14	1997-07-07	NONE                     	AIR       	, pending accounts are furiously even foxe
1590469	180776	3295	2	11	20424.47	0.03	0.02	N	O	1997-04-02	1997-05-23	1997-04-12	COLLECT COD              	REG AIR   	etect carefully. blithely ironic dolphins 
1590469	100142	7673	3	30	34264.20	0.00	0.03	N	O	1997-05-08	1997-04-23	1997-06-06	NONE                     	SHIP      	he thinly even in
1591941	29997	5002	1	10	19269.90	0.01	0.03	N	O	1997-05-26	1997-04-23	1997-06-05	NONE                     	RAIL      	furiously. blithely regular court
1591941	154189	6705	2	15	18647.70	0.09	0.08	N	O	1997-05-29	1997-03-26	1997-05-31	TAKE BACK RETURN         	AIR       	es wake carefully alongside of the fur
1591941	46410	8915	3	36	48830.76	0.00	0.06	N	O	1997-02-26	1997-04-08	1997-03-18	COLLECT COD              	FOB       	the silently express requests. daringl
1591941	9771	4772	4	30	50423.10	0.04	0.05	N	O	1997-05-10	1997-03-23	1997-05-22	COLLECT COD              	RAIL      	he close reques
1597121	56699	4215	1	30	49670.70	0.07	0.01	R	F	1992-05-21	1992-07-02	1992-06-11	NONE                     	RAIL      	 lose blithely carefully ironic courts
1597121	137144	9658	2	48	56694.72	0.09	0.04	A	F	1992-05-23	1992-06-23	1992-06-19	NONE                     	REG AIR   	y bold asymptotes. careful
1597121	166294	8811	3	44	59852.76	0.04	0.01	R	F	1992-06-22	1992-05-25	1992-07-12	COLLECT COD              	TRUCK     	ticing pla
1597121	100435	7966	4	8	11483.44	0.00	0.05	A	F	1992-06-27	1992-06-21	1992-07-17	COLLECT COD              	RAIL      	al requests cajole s
1597378	139076	1590	1	48	53523.36	0.08	0.04	N	O	1996-09-22	1996-08-23	1996-10-18	NONE                     	FOB       	theodolites. carefully even cour
1597378	44377	9386	2	48	63425.76	0.08	0.04	N	O	1996-07-02	1996-08-30	1996-08-01	TAKE BACK RETURN         	FOB       	ests sleep bold foxe
1597378	44114	6619	3	46	48673.06	0.00	0.05	N	O	1996-08-27	1996-08-08	1996-09-06	COLLECT COD              	REG AIR   	furiously regular pac
1597378	19885	7389	4	20	36097.60	0.07	0.00	N	O	1996-07-29	1996-07-29	1996-08-27	TAKE BACK RETURN         	REG AIR   	lyly pending excuses use caref
1603585	149620	4649	1	35	58436.70	0.03	0.01	N	O	1997-05-03	1997-05-07	1997-05-31	NONE                     	RAIL      	 accounts sublate 
1603585	56473	8979	2	12	17153.64	0.04	0.06	N	O	1997-06-05	1997-05-20	1997-06-15	COLLECT COD              	FOB       	old accounts by the theodolites use of 
1603585	135688	5689	3	26	44815.68	0.02	0.07	N	O	1997-07-24	1997-05-03	1997-08-04	TAKE BACK RETURN         	MAIL      	ar deposits. 
1603585	105686	707	4	8	13533.44	0.10	0.01	N	O	1997-05-08	1997-06-12	1997-05-15	NONE                     	RAIL      	otes dazzle carefully bold t
1603585	60917	3424	5	31	58215.21	0.04	0.06	N	O	1997-04-01	1997-05-28	1997-04-15	COLLECT COD              	MAIL      	s requests. final
1603585	22455	2456	6	38	52343.10	0.02	0.03	N	O	1997-04-17	1997-05-04	1997-04-26	TAKE BACK RETURN         	MAIL      	 regular packages
1620672	16374	3878	1	13	16774.81	0.09	0.08	N	O	1997-03-13	1997-05-05	1997-03-24	COLLECT COD              	TRUCK     	ly regular 
1620672	182890	7927	2	22	43403.58	0.01	0.06	N	O	1997-06-15	1997-05-07	1997-07-11	NONE                     	MAIL      	ts wake fluffily. closely brave orbits w
1620672	27217	9720	3	13	14874.73	0.09	0.00	N	O	1997-03-12	1997-04-25	1997-03-14	DELIVER IN PERSON        	TRUCK     	bold accounts cajole slyly. furiously
1620672	26526	9029	4	37	53743.24	0.08	0.05	N	O	1997-04-18	1997-03-30	1997-05-08	COLLECT COD              	RAIL      	xpress pinto beans wake.
1620672	10330	5333	5	30	37209.90	0.05	0.06	N	O	1997-04-25	1997-04-13	1997-05-13	NONE                     	TRUCK     	ly. slyly regular pinto bean
1646340	100490	3001	1	5	7452.45	0.09	0.07	N	O	1998-11-06	1998-09-28	1998-11-10	DELIVER IN PERSON        	RAIL      	arefully pending accounts. f
1646340	119403	9404	2	23	32715.20	0.08	0.02	N	O	1998-07-15	1998-08-28	1998-07-16	TAKE BACK RETURN         	FOB       	e around the ir
1646340	197480	5038	3	50	78874.00	0.02	0.08	N	O	1998-08-29	1998-09-08	1998-09-11	TAKE BACK RETURN         	AIR       	 final packa
1651463	4245	1746	1	40	45969.60	0.04	0.07	R	F	1993-07-23	1993-08-14	1993-08-06	NONE                     	FOB       	s across the carefully 
1718016	127022	9535	1	3	3147.06	0.03	0.00	A	F	1994-11-11	1994-10-23	1994-11-12	TAKE BACK RETURN         	RAIL      	s. ironic requests nag unusual instr
1718016	48439	5952	2	23	31910.89	0.04	0.06	R	F	1994-10-28	1994-11-24	1994-11-21	COLLECT COD              	MAIL      	 special foxes integrate carefully again
1718016	110459	5482	3	40	58778.00	0.10	0.05	R	F	1994-10-15	1994-10-29	1994-11-08	COLLECT COD              	TRUCK     	e slyly ironic tithes. furiously ironic ins
1718016	77815	2830	4	30	53784.30	0.01	0.03	R	F	1994-09-15	1994-11-14	1994-10-11	COLLECT COD              	REG AIR   	deposits; q
1718016	21367	1368	5	5	6441.80	0.02	0.06	A	F	1994-09-22	1994-10-20	1994-10-13	TAKE BACK RETURN         	TRUCK     	pending pinto beans could boost against t
1732771	55039	7545	1	34	33797.02	0.07	0.05	R	F	1994-11-26	1994-11-15	1994-12-20	TAKE BACK RETURN         	SHIP      	 braids according to
1732771	25004	2511	2	1	929.00	0.10	0.08	A	F	1994-10-13	1994-10-10	1994-11-05	COLLECT COD              	TRUCK     	nding requests are furiously according
1732771	25907	3414	3	1	1832.90	0.01	0.08	R	F	1994-12-20	1994-10-10	1995-01-15	COLLECT COD              	TRUCK     	y bold foxes snooze blithely.
1737250	78654	6176	1	24	39183.60	0.07	0.01	A	F	1995-02-05	1994-11-23	1995-02-25	NONE                     	SHIP      	e packages. sly
1737250	128342	3367	2	25	34258.50	0.06	0.00	A	F	1995-01-29	1995-01-06	1995-02-23	TAKE BACK RETURN         	TRUCK     	gle quickly against 
1737250	30378	379	3	20	26167.40	0.10	0.00	R	F	1994-11-13	1994-12-30	1994-11-29	TAKE BACK RETURN         	REG AIR   	s boost against the final packages.
1737250	154653	2199	4	48	81967.20	0.07	0.00	R	F	1995-01-09	1994-12-18	1995-02-07	COLLECT COD              	RAIL      	 express ideas. furiously unusual e
1737250	49241	9242	5	47	55941.28	0.08	0.00	A	F	1994-10-22	1995-01-01	1994-11-08	COLLECT COD              	REG AIR   	y even dugouts alongside of the 
1755398	175059	7577	1	1	1134.05	0.05	0.05	N	O	1997-08-04	1997-09-09	1997-08-05	TAKE BACK RETURN         	RAIL      	carefully fi
1768549	45270	279	1	6	7291.62	0.04	0.01	R	F	1994-03-06	1994-03-31	1994-03-23	NONE                     	FOB       	yly regular theodolites na
1768549	36144	6145	2	33	35644.62	0.04	0.06	A	F	1994-05-19	1994-05-20	1994-05-28	COLLECT COD              	AIR       	totes. slyly f
1774689	75407	422	1	10	13824.00	0.01	0.01	R	F	1993-07-12	1993-09-26	1993-07-30	NONE                     	MAIL      	s. slyly express frays are furio
1783941	8202	703	1	35	38857.00	0.04	0.07	N	O	1995-09-04	1995-08-25	1995-09-12	NONE                     	AIR       	thely pending braids wake furiously under t
1819008	144481	9510	1	13	19831.24	0.00	0.07	R	F	1993-01-26	1993-01-01	1993-02-24	DELIVER IN PERSON        	REG AIR   	ckages sleep blithely along the b
1819008	89134	6659	2	45	50540.85	0.04	0.04	R	F	1993-01-13	1993-01-12	1993-02-01	NONE                     	REG AIR   	c deposits alongside of the express, even 
1819008	66433	6434	3	44	61574.92	0.08	0.01	R	F	1993-02-26	1993-01-06	1993-03-01	COLLECT COD              	MAIL      	e carefully
1819008	122823	360	4	4	7383.28	0.02	0.06	R	F	1993-02-17	1992-12-24	1993-03-12	NONE                     	AIR       	ully regular ide
1819008	77695	2710	5	48	80289.12	0.02	0.01	A	F	1992-12-05	1993-01-19	1992-12-30	DELIVER IN PERSON        	REG AIR   	ructions wake accordi
1819008	13573	1077	6	27	40137.39	0.02	0.00	R	F	1993-01-30	1992-12-24	1993-02-22	COLLECT COD              	SHIP      	le carefully even de
1843907	101778	4289	1	6	10678.62	0.04	0.03	A	F	1992-12-13	1993-01-05	1992-12-20	COLLECT COD              	MAIL      	sual instructions. reques
1843907	181805	6842	2	34	64151.20	0.00	0.02	A	F	1992-11-25	1992-12-16	1992-12-10	NONE                     	MAIL      	al ideas nag above the blithel
1843907	194992	4993	3	6	12521.94	0.02	0.05	A	F	1992-11-22	1992-12-23	1992-12-18	COLLECT COD              	FOB       	y regular accounts. theodolites
1843907	15876	3380	4	4	7167.48	0.09	0.02	A	F	1993-01-17	1993-01-13	1993-01-18	COLLECT COD              	FOB       	gular, regular deposits. i
1843907	12281	7284	5	39	46537.92	0.10	0.05	R	F	1993-02-05	1993-01-18	1993-03-04	COLLECT COD              	SHIP      	 affix. un
1843907	6517	1518	6	24	34164.24	0.02	0.05	R	F	1992-12-16	1993-01-19	1992-12-22	NONE                     	REG AIR   	he quickly iron
1857794	188623	6178	1	26	44502.12	0.03	0.04	A	F	1993-06-30	1993-06-30	1993-07-10	COLLECT COD              	FOB       	 lose close, final courts. furiou
1857794	171897	4415	2	34	66942.26	0.08	0.08	A	F	1993-07-25	1993-06-27	1993-08-19	TAKE BACK RETURN         	REG AIR   	 regular, final attainments en
1857794	56082	3598	3	11	11418.88	0.02	0.00	A	F	1993-08-26	1993-07-20	1993-09-03	DELIVER IN PERSON        	FOB       	bt furiously dependenc
1857794	161961	9510	4	3	6068.88	0.00	0.03	A	F	1993-06-07	1993-07-22	1993-06-27	TAKE BACK RETURN         	FOB       	 sleep blith
1857794	66687	1700	5	37	61186.16	0.05	0.01	R	F	1993-07-11	1993-07-06	1993-08-07	NONE                     	REG AIR   	e deposits: fluffily final ideas wa
1857794	192961	8000	6	26	53402.96	0.05	0.08	R	F	1993-06-02	1993-08-01	1993-06-14	DELIVER IN PERSON        	FOB       	. quickly special de
1857794	78746	1254	7	22	37944.28	0.10	0.07	R	F	1993-06-29	1993-06-20	1993-07-09	DELIVER IN PERSON        	FOB       	n pinto beans run fluffily quick 
1869056	138541	3568	1	19	30011.26	0.00	0.00	N	O	1995-08-14	1995-07-13	1995-08-16	COLLECT COD              	MAIL      	 deposits. furiously unusual 
1869056	49049	9050	2	37	36927.48	0.10	0.08	N	O	1995-09-01	1995-07-09	1995-09-25	DELIVER IN PERSON        	SHIP      	s. furiously sly instructions
1869056	161474	9023	3	35	53741.45	0.08	0.02	N	O	1995-07-12	1995-07-16	1995-08-07	COLLECT COD              	TRUCK     	 hang about the furiously regular
1869056	77798	306	4	12	21309.48	0.00	0.02	N	O	1995-09-23	1995-07-02	1995-10-23	DELIVER IN PERSON        	REG AIR   	sleep pinto beans: fluf
1869056	148163	8164	5	32	38757.12	0.03	0.03	N	O	1995-07-17	1995-07-20	1995-08-07	TAKE BACK RETURN         	AIR       	sts use regular ideas. regular pinto
1869056	166906	4455	6	6	11837.40	0.09	0.03	N	O	1995-07-12	1995-07-24	1995-07-15	COLLECT COD              	SHIP      	ar packages for the accounts doubt flu
1869056	180047	5084	7	48	54097.92	0.02	0.05	N	O	1995-08-22	1995-08-04	1995-09-19	DELIVER IN PERSON        	REG AIR   	ites nod slyly along
1876000	66820	6821	1	22	39310.04	0.06	0.03	R	F	1995-01-31	1995-04-25	1995-02-28	TAKE BACK RETURN         	RAIL      	es. special pinto beans haggle blithely f
1876000	35362	7866	2	21	27244.56	0.04	0.02	R	F	1995-03-16	1995-03-06	1995-04-10	DELIVER IN PERSON        	SHIP      	e blithely alongsid
1876000	127960	5497	3	29	57650.84	0.04	0.06	R	F	1995-05-03	1995-03-18	1995-06-02	NONE                     	SHIP      	ironic depos
1876000	8000	3001	4	46	41768.00	0.09	0.05	A	F	1995-02-13	1995-04-04	1995-03-02	COLLECT COD              	FOB       	cording to the platelets
1876000	46016	1025	5	14	13468.14	0.04	0.01	R	F	1995-02-17	1995-03-29	1995-02-20	DELIVER IN PERSON        	TRUCK     	 the ironic theodolites sle
1876550	179743	7295	1	5	9113.70	0.00	0.00	R	F	1992-07-07	1992-07-09	1992-07-19	COLLECT COD              	RAIL      	regular theodolites wa
1876550	140248	2763	2	31	39935.44	0.06	0.05	R	F	1992-06-29	1992-06-26	1992-07-10	DELIVER IN PERSON        	REG AIR   	ic ideas. carefully regular acco
1876550	140403	5432	3	28	40415.20	0.03	0.00	R	F	1992-06-10	1992-06-24	1992-06-25	TAKE BACK RETURN         	TRUCK     	 pinto beans. slow, ironic requests are q
1876550	146047	1076	4	11	12023.44	0.02	0.03	R	F	1992-08-16	1992-06-21	1992-08-27	TAKE BACK RETURN         	REG AIR   	nal accounts.
1876550	6007	1008	5	8	7304.00	0.09	0.05	R	F	1992-08-07	1992-07-04	1992-08-15	COLLECT COD              	FOB       	poach quickly blithely even deposits. neve
1876550	143772	1315	6	24	43578.48	0.06	0.04	R	F	1992-05-20	1992-08-12	1992-05-21	DELIVER IN PERSON        	TRUCK     	 beans around the sl
1883718	77411	4933	1	16	22214.56	0.01	0.02	N	O	1998-04-15	1998-02-21	1998-04-17	TAKE BACK RETURN         	AIR       	leep above the packages.
1883718	45273	7778	2	50	60913.50	0.00	0.05	N	O	1998-02-03	1998-04-12	1998-02-07	TAKE BACK RETURN         	SHIP      	eas are fluffily. 
1883718	102734	7755	3	6	10420.38	0.05	0.08	N	O	1998-02-05	1998-03-12	1998-02-12	COLLECT COD              	AIR       	uests cajole fluffily against
1883718	46983	9488	4	6	11579.88	0.00	0.01	N	O	1998-04-18	1998-04-21	1998-05-03	COLLECT COD              	REG AIR   	 according to th
1883718	28560	6067	5	14	20839.84	0.00	0.08	N	O	1998-03-06	1998-03-16	1998-04-01	TAKE BACK RETURN         	RAIL      	r waters cajole c
1932643	192007	2008	1	32	35168.00	0.08	0.06	N	O	1997-08-04	1997-07-16	1997-08-31	DELIVER IN PERSON        	SHIP      	ress, bold dependencies are. blithely 
1932643	51863	6874	2	42	76224.12	0.09	0.07	N	O	1997-06-20	1997-07-08	1997-07-18	DELIVER IN PERSON        	RAIL      	ly slyly ironic packages. d
1932643	58640	3651	3	20	31972.80	0.03	0.01	N	O	1997-06-06	1997-08-24	1997-06-30	DELIVER IN PERSON        	RAIL      	ithely requests. clo
1932643	150416	417	4	40	58656.40	0.07	0.02	N	O	1997-09-27	1997-07-04	1997-10-10	COLLECT COD              	AIR       	ly express deposits integrate s
1932643	41111	1112	5	26	27354.86	0.09	0.07	N	O	1997-07-06	1997-07-21	1997-07-11	TAKE BACK RETURN         	AIR       	ges. furiously
1932643	145280	5281	6	34	45059.52	0.01	0.07	N	O	1997-08-07	1997-08-30	1997-09-05	COLLECT COD              	MAIL      	al realms among the bol
1932643	114251	6763	7	41	51875.25	0.03	0.07	N	O	1997-09-24	1997-07-11	1997-09-29	COLLECT COD              	TRUCK     	 slyly. carefully s
1944711	66583	4102	1	37	57334.46	0.03	0.00	R	F	1995-05-15	1995-07-29	1995-05-20	TAKE BACK RETURN         	MAIL      	ular forges. slyly ev
1944711	48820	3829	2	15	26532.30	0.03	0.02	N	O	1995-08-06	1995-06-20	1995-09-04	TAKE BACK RETURN         	MAIL      	into beans according to the slyl
1944711	103892	8913	3	13	24646.57	0.10	0.04	N	O	1995-07-16	1995-06-28	1995-07-31	DELIVER IN PERSON        	MAIL      	, final instructions.
1950947	61567	4074	1	36	55028.16	0.08	0.08	R	F	1994-10-11	1994-09-18	1994-10-29	NONE                     	MAIL      	quickly regular accounts. regular, un
1950947	152676	2677	2	8	13829.36	0.08	0.07	R	F	1994-08-16	1994-10-16	1994-08-19	TAKE BACK RETURN         	MAIL      	xpress packages; express, regular req
1950947	15811	3315	3	9	15541.29	0.03	0.06	A	F	1994-09-06	1994-10-09	1994-10-01	NONE                     	MAIL      	dencies among the fina
1950947	134636	4637	4	41	68495.83	0.07	0.02	A	F	1994-08-14	1994-10-02	1994-09-08	DELIVER IN PERSON        	REG AIR   	pinto beans haggle blithely accordi
1950947	16008	8510	5	5	4620.00	0.02	0.04	A	F	1994-09-02	1994-10-28	1994-09-06	COLLECT COD              	TRUCK     	ess deposits. even platelets acc
1951169	22931	438	1	6	11123.58	0.02	0.01	N	O	1996-02-15	1996-04-23	1996-03-13	DELIVER IN PERSON        	SHIP      	 even requests. 
1951169	192703	2704	2	21	37709.70	0.02	0.08	N	O	1996-02-29	1996-04-20	1996-03-27	DELIVER IN PERSON        	RAIL      	al, regular pinto beans poach bli
1951169	149945	9946	3	47	93762.18	0.06	0.07	N	O	1996-04-20	1996-04-06	1996-05-06	TAKE BACK RETURN         	AIR       	uickly final 
1951169	122115	2116	4	4	4548.44	0.06	0.04	N	O	1996-02-13	1996-04-20	1996-03-01	DELIVER IN PERSON        	RAIL      	oost. carefully regular d
1951169	181107	3626	5	35	41583.50	0.05	0.00	N	O	1996-03-07	1996-04-20	1996-03-24	COLLECT COD              	RAIL      	y unusual do
1951169	51947	4453	6	43	81654.42	0.04	0.00	N	O	1996-04-20	1996-03-30	1996-05-15	NONE                     	REG AIR   	 even deposits sleep a
1951169	67807	314	7	5	8874.00	0.07	0.06	N	O	1996-02-13	1996-03-23	1996-03-05	NONE                     	RAIL      	instructions breach ruthlessly at the 
1953381	101522	9053	1	22	33517.44	0.10	0.00	R	F	1994-10-10	1994-07-27	1994-11-02	NONE                     	FOB       	kly express deposits run. slyly i
1953381	77789	5311	2	41	72437.98	0.06	0.05	A	F	1994-08-29	1994-08-06	1994-09-17	TAKE BACK RETURN         	FOB       	old packages alongside of the s
1953441	3713	8714	1	48	77602.08	0.10	0.08	N	O	1996-07-08	1996-08-09	1996-07-11	DELIVER IN PERSON        	REG AIR   	of the slyly pending theodolites. f
1953441	191708	4228	2	2	3599.40	0.10	0.04	N	O	1996-07-24	1996-08-07	1996-08-16	DELIVER IN PERSON        	RAIL      	uriously express dolphins. blithely clos
1953441	31856	9366	3	2	3575.70	0.02	0.05	N	O	1996-09-09	1996-07-21	1996-09-14	DELIVER IN PERSON        	RAIL      	furiously silent packages. blithely ir
1953441	1186	8687	4	7	7610.26	0.06	0.00	N	O	1996-08-02	1996-08-07	1996-08-11	TAKE BACK RETURN         	TRUCK     	, bold decoys belie
1953441	122322	9859	5	21	28230.72	0.05	0.07	N	O	1996-07-15	1996-08-07	1996-08-05	NONE                     	MAIL      	 packages snooze quickly. final ide
1953441	64462	6969	6	31	44220.26	0.02	0.07	N	O	1996-07-14	1996-08-21	1996-07-16	NONE                     	FOB       	uffily final deposits. furious
1953441	11771	4273	7	44	74041.88	0.07	0.03	N	O	1996-07-23	1996-07-26	1996-07-26	COLLECT COD              	AIR       	l braids. quickly express deposits cajole a
1953926	43317	8326	1	20	25206.20	0.07	0.04	R	F	1993-06-17	1993-07-12	1993-06-21	NONE                     	FOB       	sts affix quickly bold depos
1953926	16058	8560	2	13	12662.65	0.02	0.03	R	F	1993-09-02	1993-07-21	1993-10-02	COLLECT COD              	MAIL      	s after the silen
1953926	83690	3691	3	20	33473.80	0.01	0.05	A	F	1993-08-11	1993-06-20	1993-08-15	TAKE BACK RETURN         	TRUCK     	ckages nag carefull
1953926	169631	7180	4	47	79929.61	0.09	0.00	R	F	1993-07-02	1993-07-14	1993-07-18	NONE                     	FOB       	dolites are quickly ironic deposits. d
1953926	187531	50	5	1	1618.53	0.03	0.07	A	F	1993-06-30	1993-06-25	1993-07-27	TAKE BACK RETURN         	REG AIR   	impress furiously fluffily final acc
1953926	24065	4066	6	1	989.06	0.00	0.06	R	F	1993-06-11	1993-07-23	1993-06-28	COLLECT COD              	MAIL      	nstructions along the
1967460	162089	2090	1	12	13812.96	0.03	0.08	N	O	1996-11-11	1996-09-16	1996-12-11	NONE                     	AIR       	 even, special asymptotes boost blithely 
1967460	19774	4777	2	19	32181.63	0.10	0.04	N	O	1996-12-06	1996-11-01	1996-12-11	COLLECT COD              	REG AIR   	blithely bol
1978756	168332	8333	1	45	63014.85	0.04	0.00	N	O	1996-09-30	1996-09-22	1996-10-06	NONE                     	SHIP      	ing to the carefully even de
1978756	143978	1521	2	49	99076.53	0.10	0.01	N	O	1996-10-20	1996-09-12	1996-10-23	TAKE BACK RETURN         	FOB       	 deposits wake fluffi
1978756	26601	1606	3	35	53466.00	0.10	0.05	N	O	1996-10-03	1996-10-29	1996-10-21	DELIVER IN PERSON        	REG AIR   	 packages haggle s
1978756	81795	1796	4	25	44419.75	0.07	0.03	N	O	1996-12-01	1996-10-21	1996-12-21	COLLECT COD              	REG AIR   	pinto beans cajole carefully enti
1978756	124255	6768	5	22	28143.50	0.06	0.02	N	O	1996-12-01	1996-09-18	1996-12-20	NONE                     	FOB       	n theodolites 
1978756	153258	5774	6	35	45893.75	0.04	0.00	N	O	1996-09-19	1996-09-14	1996-10-06	COLLECT COD              	MAIL      	packages. final, even excuses maint
1978854	30851	5858	1	12	21382.20	0.04	0.06	A	F	1993-01-28	1993-02-18	1993-02-07	DELIVER IN PERSON        	FOB       	ackages boo
1978854	99246	1756	2	21	26150.04	0.00	0.05	R	F	1993-04-29	1993-03-01	1993-05-09	NONE                     	SHIP      	lithely even accounts
1978854	20904	5909	3	13	23723.70	0.07	0.07	A	F	1993-01-12	1993-02-15	1993-02-08	DELIVER IN PERSON        	AIR       	ate carefully against the final, final 
1978854	129933	7470	4	10	19629.30	0.08	0.05	A	F	1993-04-22	1993-03-06	1993-05-18	DELIVER IN PERSON        	FOB       	 unusual instructions poach blit
1978854	182307	4826	5	45	62518.50	0.04	0.07	A	F	1993-01-26	1993-02-11	1993-02-03	NONE                     	MAIL      	s. carefully even as
1978854	33483	993	6	28	39661.44	0.08	0.07	A	F	1993-04-02	1993-02-25	1993-04-24	NONE                     	FOB       	xpress ideas. ac
1978854	142735	5250	7	34	60442.82	0.10	0.07	A	F	1993-03-25	1993-04-02	1993-04-12	TAKE BACK RETURN         	AIR       	ven, furious foxes. pen
1991461	167035	4584	1	27	29754.81	0.02	0.06	R	F	1993-10-28	1993-11-08	1993-11-08	TAKE BACK RETURN         	REG AIR   	ironic req
1991461	147829	344	2	14	26275.48	0.06	0.07	A	F	1993-09-25	1993-11-18	1993-10-10	NONE                     	MAIL      	. deposits haggle care
1991461	153521	3522	3	7	11021.64	0.09	0.04	R	F	1993-08-21	1993-10-20	1993-08-22	NONE                     	TRUCK     	 to the courts. final, express 
1991461	16879	6880	4	8	14366.96	0.05	0.07	R	F	1993-10-06	1993-10-24	1993-10-31	NONE                     	TRUCK     	theodolites nag carefully accordi
1991461	76904	6905	5	17	31975.30	0.08	0.02	R	F	1993-09-06	1993-10-19	1993-09-30	COLLECT COD              	RAIL      	odolites are above the unusual
1991461	109453	4474	6	45	65810.25	0.04	0.05	R	F	1993-11-16	1993-09-30	1993-11-30	TAKE BACK RETURN         	FOB       	its. unusual
1991461	171941	1942	7	40	80517.60	0.10	0.08	A	F	1993-10-30	1993-10-18	1993-11-11	NONE                     	REG AIR   	ly. even, sly deposits alongside of the r
2007493	199263	1783	1	25	34056.50	0.03	0.06	N	O	1997-06-29	1997-08-16	1997-07-08	COLLECT COD              	AIR       	counts sleep above the even account
2007493	3570	8571	2	41	60416.37	0.02	0.03	N	O	1997-09-01	1997-07-20	1997-09-14	NONE                     	FOB       	iously after 
2007493	193382	8421	3	26	38359.88	0.09	0.04	N	O	1997-08-06	1997-08-06	1997-08-29	COLLECT COD              	AIR       	counts promise quickly car
2007493	155290	321	4	17	22869.93	0.05	0.03	N	O	1997-08-19	1997-08-05	1997-08-31	TAKE BACK RETURN         	RAIL      	 according to the pinto beans. fu
2023140	51267	8783	1	30	36547.80	0.05	0.08	R	F	1994-04-19	1994-05-15	1994-05-09	COLLECT COD              	FOB       	eodolites in place of the blithely pendin
2023140	44436	6941	2	37	51075.91	0.06	0.01	R	F	1994-04-20	1994-06-29	1994-05-01	DELIVER IN PERSON        	MAIL      	 furiously even platelets. package
2126688	26118	1123	1	26	27146.86	0.03	0.00	R	F	1993-10-10	1993-11-02	1993-11-02	TAKE BACK RETURN         	MAIL      	tions cajol
2126688	150126	127	2	20	23522.40	0.02	0.07	R	F	1993-08-24	1993-10-31	1993-08-28	DELIVER IN PERSON        	TRUCK     	 final, regular pac
2231556	56082	1093	1	6	6228.48	0.02	0.02	N	O	1996-01-17	1995-12-12	1996-02-07	DELIVER IN PERSON        	SHIP      	e permanently according to the ironic plate
2231556	160655	5688	2	37	63479.05	0.08	0.08	N	O	1995-11-16	1995-12-05	1995-12-11	TAKE BACK RETURN         	AIR       	. ironically re
2231556	126184	1209	3	33	39935.94	0.06	0.04	N	O	1996-01-09	1995-12-26	1996-02-07	COLLECT COD              	REG AIR   	ously along the slyly 
2242243	2386	7387	1	28	36074.64	0.04	0.04	A	F	1994-05-16	1994-03-27	1994-06-12	COLLECT COD              	TRUCK     	y pending i
2248546	170078	2596	1	36	41330.52	0.00	0.05	R	F	1994-03-20	1994-04-14	1994-03-26	TAKE BACK RETURN         	AIR       	g thin, express requests. final packa
2248546	89258	1767	2	46	57373.50	0.01	0.05	A	F	1994-07-06	1994-04-22	1994-07-12	NONE                     	REG AIR   	n packages n
2248546	145833	8348	3	7	13151.81	0.10	0.02	R	F	1994-06-06	1994-05-02	1994-06-25	NONE                     	RAIL      	e blithely regular 
2248546	8215	8216	4	40	44928.40	0.00	0.04	A	F	1994-04-13	1994-05-29	1994-05-05	COLLECT COD              	RAIL      	n dolphins sleep furio
2342115	170254	7806	1	22	29133.50	0.04	0.03	A	F	1993-01-14	1993-01-08	1993-02-13	COLLECT COD              	REG AIR   	 patterns. packages are furiously
2342115	53360	8371	2	45	59101.20	0.00	0.00	A	F	1993-01-04	1993-01-24	1993-01-21	TAKE BACK RETURN         	AIR       	rding to th
2342115	20642	643	3	48	75006.72	0.08	0.01	A	F	1992-12-05	1993-01-15	1992-12-28	COLLECT COD              	MAIL      	kly pending ideas haggle 
2342115	127585	7586	4	32	51602.56	0.10	0.04	A	F	1992-11-23	1992-12-26	1992-12-02	NONE                     	FOB       	cajole across the blithely bold the
2342115	113401	935	5	18	25459.20	0.08	0.06	R	F	1993-01-12	1993-01-06	1993-02-09	TAKE BACK RETURN         	MAIL      	ar theodolites serve slyly. fluff
2370438	94410	6920	1	4	5617.64	0.06	0.01	N	O	1997-08-04	1997-09-20	1997-09-02	NONE                     	TRUCK     	o the quietly regular deposits
2370438	137482	9996	2	3	4558.44	0.10	0.06	N	O	1997-08-05	1997-09-01	1997-08-09	NONE                     	SHIP      	arefully careful asymptotes sleep 
2370438	55709	8215	3	43	71582.10	0.06	0.03	N	O	1997-08-28	1997-08-25	1997-09-06	TAKE BACK RETURN         	FOB       	iously final b
2370438	100599	600	4	19	30392.21	0.08	0.04	N	O	1997-09-21	1997-08-13	1997-10-20	TAKE BACK RETURN         	REG AIR   	gular platelets ab
2370438	163887	1436	5	30	58526.40	0.03	0.05	N	O	1997-09-02	1997-09-22	1997-09-08	COLLECT COD              	FOB       	ly even packages s
2370438	44624	4625	6	5	7843.10	0.02	0.07	N	O	1997-10-17	1997-08-15	1997-10-30	DELIVER IN PERSON        	MAIL      	, final packages believe carefully
2370438	179103	4138	7	17	20095.70	0.09	0.07	N	O	1997-07-31	1997-07-24	1997-08-23	TAKE BACK RETURN         	RAIL      	fully regul
2401414	97838	5366	1	27	49567.41	0.02	0.01	N	O	1996-05-25	1996-07-03	1996-06-24	TAKE BACK RETURN         	MAIL      	 furiously regul
2401414	160140	7689	2	8	9601.12	0.01	0.07	N	O	1996-05-03	1996-06-21	1996-05-19	NONE                     	TRUCK     	depths wake slyly specia
2401414	68211	8212	3	46	54243.66	0.07	0.03	N	O	1996-08-08	1996-06-03	1996-08-19	TAKE BACK RETURN         	TRUCK     	ctions. blithely
2401414	11710	9214	4	31	50273.01	0.05	0.03	N	O	1996-06-13	1996-07-05	1996-06-18	DELIVER IN PERSON        	REG AIR   	riously. express instructions serve caref
2401414	1231	8732	5	25	28305.75	0.09	0.04	N	O	1996-06-08	1996-06-06	1996-06-23	NONE                     	AIR       	gged deposits
2425574	32356	9866	1	25	32208.75	0.02	0.05	R	F	1994-02-26	1994-02-19	1994-03-26	TAKE BACK RETURN         	RAIL      	usly even instructions wake 
2459619	85054	71	1	42	43640.10	0.01	0.07	N	O	1996-01-22	1996-02-02	1996-01-27	COLLECT COD              	FOB       	 deposits cajole. fluffily spec
2471840	112801	7824	1	50	90690.00	0.00	0.08	N	O	1996-07-11	1996-07-13	1996-07-20	TAKE BACK RETURN         	MAIL      	ress accounts affix even pinto beans. r
2471840	37933	2940	2	44	82320.92	0.10	0.06	N	O	1996-06-19	1996-08-24	1996-06-21	NONE                     	FOB       	. quickly ironic packages above t
2471840	124750	7263	3	45	79863.75	0.10	0.04	N	O	1996-07-09	1996-08-23	1996-07-13	TAKE BACK RETURN         	REG AIR   	ts use slyly carefully final pinto
2471840	65730	8237	4	7	11870.11	0.03	0.06	N	O	1996-06-14	1996-08-25	1996-07-03	TAKE BACK RETURN         	REG AIR   	olphins use blithely. fina
2471840	109770	2281	5	24	42714.48	0.03	0.08	N	O	1996-08-15	1996-08-11	1996-08-29	NONE                     	RAIL      	nts cajole slyly in place
2488771	59675	9676	1	2	3269.34	0.08	0.06	R	F	1992-07-20	1992-08-16	1992-08-13	COLLECT COD              	AIR       	ironic deposits cajole ca
2488771	147059	7060	2	50	55302.50	0.01	0.05	R	F	1992-06-23	1992-07-13	1992-06-26	NONE                     	FOB       	ost furiously across the quickl
2488771	19810	4813	3	1	1729.81	0.00	0.04	R	F	1992-09-15	1992-07-18	1992-10-10	COLLECT COD              	TRUCK     	lly express requests wake carefu
2488771	141104	8647	4	47	53819.70	0.02	0.02	R	F	1992-06-30	1992-07-01	1992-07-09	TAKE BACK RETURN         	SHIP      	mold furiously. final deposits
2488771	126824	1849	5	44	81436.08	0.07	0.08	R	F	1992-09-13	1992-08-03	1992-10-10	COLLECT COD              	FOB       	inal accounts wake c
2488771	124059	6572	6	1	1083.05	0.05	0.08	A	F	1992-06-03	1992-07-14	1992-06-23	TAKE BACK RETURN         	REG AIR   	packages. slyly even pac
2488771	51418	3924	7	43	58884.63	0.03	0.07	A	F	1992-08-05	1992-06-22	1992-08-20	COLLECT COD              	TRUCK     	t among the slyly pendin
2507303	177288	4840	1	12	16383.36	0.03	0.06	N	O	1997-10-21	1997-12-07	1997-11-07	COLLECT COD              	MAIL      	xpress instructions eat c
2546947	65531	5532	1	7	10475.71	0.10	0.04	N	O	1997-10-10	1997-09-22	1997-10-17	COLLECT COD              	TRUCK     	 slyly quickly silent deposits. carefully 
2546947	169649	2166	2	9	15467.76	0.03	0.07	N	O	1997-09-17	1997-08-03	1997-09-29	COLLECT COD              	FOB       	ckly unusual accounts haggle furiously acco
2546947	89964	9965	3	15	29309.40	0.08	0.05	N	O	1997-07-31	1997-08-12	1997-08-09	TAKE BACK RETURN         	FOB       	fily alongside of the express reque
2546947	12504	5006	4	40	56660.00	0.00	0.06	N	O	1997-08-19	1997-08-14	1997-08-27	TAKE BACK RETURN         	REG AIR   	 carefully u
2546947	153493	3494	5	44	68045.56	0.01	0.05	N	O	1997-09-12	1997-08-14	1997-10-08	NONE                     	FOB       	cording to the slyly unusual request
2559110	36660	9164	1	40	63866.40	0.02	0.02	N	O	1996-10-18	1996-09-23	1996-10-30	TAKE BACK RETURN         	MAIL      	y regular instruc
2559110	129545	7082	2	44	69279.76	0.04	0.05	N	O	1996-08-12	1996-09-26	1996-09-08	NONE                     	REG AIR   	 beans are quickly slow 
2559110	47228	7229	3	46	54060.12	0.04	0.07	N	O	1996-08-13	1996-11-06	1996-08-17	NONE                     	AIR       	kly pending, regular ideas. carefully even
2559110	154389	9420	4	10	14433.80	0.05	0.02	N	O	1996-08-23	1996-09-18	1996-09-01	DELIVER IN PERSON        	MAIL      	even dolphins boost alongside o
2602208	143566	1109	1	34	54725.04	0.02	0.01	A	F	1992-06-23	1992-05-29	1992-06-28	NONE                     	FOB       	s. furiously regular excuses wake ironicall
2602208	97676	186	2	49	82009.83	0.03	0.01	A	F	1992-06-23	1992-06-09	1992-07-22	NONE                     	SHIP      	regular ideas sleep busily somas. furiousl
2602208	26588	1593	3	43	65126.94	0.06	0.03	A	F	1992-06-11	1992-05-22	1992-06-26	DELIVER IN PERSON        	REG AIR   	y across the regular, pending
2602208	59872	7388	4	50	91593.50	0.05	0.06	A	F	1992-05-24	1992-07-04	1992-05-28	NONE                     	AIR       	above the 
2602208	65042	2561	5	46	46323.84	0.00	0.08	A	F	1992-07-17	1992-06-14	1992-08-07	COLLECT COD              	REG AIR   	counts use along the q
2604326	101767	1768	1	44	77825.44	0.02	0.02	A	F	1993-03-17	1993-04-09	1993-04-02	COLLECT COD              	MAIL      	xpress forges. thinly bold 
2630562	108745	1256	1	37	64888.38	0.07	0.00	R	F	1993-09-19	1993-07-30	1993-10-18	TAKE BACK RETURN         	REG AIR   	telets are around th
2630562	63400	8413	2	47	64079.80	0.03	0.02	R	F	1993-09-29	1993-07-29	1993-10-24	TAKE BACK RETURN         	FOB       	refully. ironic, pen
2630562	162921	7954	3	8	15871.36	0.05	0.06	A	F	1993-09-25	1993-09-17	1993-10-14	DELIVER IN PERSON        	MAIL      	lets after the bold, pending
2630562	199149	4188	4	45	56166.30	0.02	0.06	R	F	1993-07-14	1993-09-24	1993-08-12	COLLECT COD              	AIR       	 regular instructions. blithe
2630562	83201	3202	5	26	30789.20	0.08	0.08	A	F	1993-09-25	1993-08-29	1993-10-12	DELIVER IN PERSON        	REG AIR   	ously unusual accou
2630562	155844	3390	6	23	43696.32	0.07	0.00	R	F	1993-08-07	1993-08-31	1993-08-19	COLLECT COD              	REG AIR   	 unusual realms. fur
2630562	68414	5933	7	39	53913.99	0.00	0.03	A	F	1993-07-15	1993-09-10	1993-07-27	NONE                     	SHIP      	its. blithely regular pinto beans kindle c
2646244	108469	6000	1	40	59098.40	0.08	0.07	N	O	1997-10-23	1997-11-23	1997-11-01	TAKE BACK RETURN         	FOB       	ual tithes
2646244	21739	1740	2	17	28232.41	0.10	0.08	N	O	1997-11-28	1997-10-27	1997-12-08	COLLECT COD              	AIR       	osits. slyly regular deposits 
2646244	157836	352	3	48	90903.84	0.05	0.08	N	O	1997-11-06	1997-10-07	1997-11-07	DELIVER IN PERSON        	AIR       	 closely special requests wake furiou
2651266	3938	3939	1	11	20261.23	0.08	0.02	A	F	1994-05-02	1994-05-24	1994-05-27	NONE                     	TRUCK     	arefully regular Ti
2651266	185677	3232	2	15	26440.05	0.08	0.01	A	F	1994-05-16	1994-06-20	1994-06-05	COLLECT COD              	TRUCK     	inal accounts. fluffi
2651266	63416	935	3	40	55176.40	0.10	0.07	A	F	1994-07-25	1994-05-11	1994-07-29	NONE                     	FOB       	bold ideas haggle slyly 
2651266	28653	8654	4	42	66429.30	0.05	0.00	A	F	1994-05-01	1994-06-23	1994-05-19	COLLECT COD              	AIR       	. ironic requ
2665734	82981	5490	1	26	51063.48	0.08	0.00	N	O	1995-06-19	1995-04-24	1995-07-02	TAKE BACK RETURN         	TRUCK     	ccounts. final ideas aff
2705092	94950	2478	1	19	36954.05	0.03	0.00	A	F	1993-11-10	1993-12-14	1993-12-07	NONE                     	REG AIR   	ular, ironic instructions about the ideas 
2705092	24151	6654	2	7	7526.05	0.03	0.05	R	F	1993-12-23	1993-12-02	1993-12-25	COLLECT COD              	RAIL      	y ruthless accounts. sl
2705092	89568	9569	3	41	63859.96	0.05	0.03	A	F	1993-10-26	1993-11-26	1993-11-17	DELIVER IN PERSON        	REG AIR   	s use caref
2705092	161280	3797	4	23	30849.44	0.00	0.07	A	F	1994-01-08	1994-01-01	1994-02-05	DELIVER IN PERSON        	FOB       	slow pinto beans wake regularly carefully f
2705092	27315	2320	5	17	21119.27	0.02	0.01	R	F	1993-11-12	1993-11-30	1993-11-25	NONE                     	AIR       	carefully ironic packages ar
2705092	68918	6437	6	38	71702.58	0.05	0.01	R	F	1993-12-06	1993-12-15	1993-12-29	NONE                     	TRUCK     	egular foxes sleep carefully. carefully f
2705092	88707	1216	7	39	66132.30	0.04	0.01	A	F	1993-11-16	1993-12-22	1993-12-06	TAKE BACK RETURN         	TRUCK     	s nag quickly. hockey play
2708037	44951	7456	1	40	75838.00	0.04	0.05	N	O	1998-09-03	1998-09-18	1998-09-18	DELIVER IN PERSON        	AIR       	ptotes sle
2708037	79038	4053	2	41	41698.23	0.02	0.03	N	O	1998-09-15	1998-08-11	1998-10-06	DELIVER IN PERSON        	REG AIR   	counts haggle slyly alon
2708037	191494	1495	3	1	1585.49	0.07	0.07	N	O	1998-07-29	1998-09-05	1998-08-20	TAKE BACK RETURN         	RAIL      	uctions poach furiously final 
2708037	24584	2091	4	21	31680.18	0.02	0.07	N	O	1998-10-25	1998-08-18	1998-10-26	NONE                     	TRUCK     	ely ironic
2708037	177255	9773	5	6	7993.50	0.02	0.07	N	O	1998-11-03	1998-09-29	1998-11-27	TAKE BACK RETURN         	MAIL      	oss the slyly
2708037	108696	6227	6	1	1704.69	0.03	0.06	N	O	1998-07-18	1998-09-12	1998-08-03	DELIVER IN PERSON        	MAIL      	ual pinto beans pr
2708259	162423	2424	1	1	1485.42	0.01	0.07	N	O	1995-10-20	1995-09-14	1995-11-10	NONE                     	SHIP      	ng the slyly unusual deposits. special, s
2708259	33305	815	2	9	11144.70	0.03	0.05	N	O	1995-10-21	1995-10-02	1995-10-25	DELIVER IN PERSON        	MAIL      	y final courts. blithely even accoun
2708259	142706	2707	3	23	40220.10	0.04	0.01	N	O	1995-08-24	1995-10-19	1995-08-28	COLLECT COD              	MAIL      	 instructions. bo
2708259	56548	6549	4	16	24072.64	0.07	0.04	N	O	1995-11-18	1995-11-05	1995-12-13	TAKE BACK RETURN         	SHIP      	s foxes. slyly regular accounts are about t
2708259	199437	1957	5	9	13827.87	0.03	0.06	N	O	1995-09-30	1995-09-28	1995-10-16	NONE                     	MAIL      	accounts around the
2712836	19507	9508	1	7	9985.50	0.09	0.02	R	F	1992-08-25	1992-08-30	1992-09-09	TAKE BACK RETURN         	AIR       	ounts are around the furiously even d
2718208	155886	8402	1	48	93210.24	0.00	0.07	N	O	1996-02-19	1996-02-01	1996-03-15	COLLECT COD              	AIR       	ag furiously 
2718208	169621	9622	2	34	57481.08	0.08	0.03	N	O	1996-03-16	1996-03-17	1996-03-27	COLLECT COD              	RAIL      	es are among the
2718208	10196	5199	3	9	9955.71	0.04	0.07	N	O	1996-03-21	1996-02-06	1996-03-31	COLLECT COD              	SHIP      	ts use quickly 
2718208	161938	4455	4	2	3999.86	0.08	0.04	N	O	1996-02-28	1996-02-03	1996-03-05	TAKE BACK RETURN         	FOB       	ages serve carefully fi
2736865	4480	6981	1	41	56763.68	0.06	0.02	R	F	1994-09-20	1994-10-22	1994-09-24	DELIVER IN PERSON        	TRUCK     	ily pending
2736865	87347	4872	2	24	32024.16	0.09	0.07	R	F	1994-11-09	1994-11-09	1994-12-09	TAKE BACK RETURN         	RAIL      	regular, even foxes c
2736865	28907	3912	3	49	89959.10	0.08	0.02	A	F	1994-10-20	1994-12-13	1994-10-30	NONE                     	SHIP      	 haggle permanently according to the
2736865	33615	6119	4	48	74333.28	0.07	0.02	A	F	1994-12-30	1994-11-01	1995-01-03	NONE                     	FOB       	ays bold requests cajole furiously. sil
2736865	158285	5831	5	13	17462.64	0.10	0.00	A	F	1994-11-09	1994-11-29	1994-12-02	NONE                     	FOB       	 accounts will have to 
2736865	85483	3008	6	9	13216.32	0.05	0.07	A	F	1994-10-30	1994-11-10	1994-11-13	COLLECT COD              	RAIL      	equests. doggedly 
2765152	19827	2329	1	5	8734.10	0.10	0.06	N	O	1996-10-27	1996-10-18	1996-11-07	COLLECT COD              	AIR       	s boost fluffily after the instructions; p
2773829	145183	7698	1	8	9825.44	0.10	0.00	A	F	1993-02-11	1993-02-21	1993-02-18	NONE                     	SHIP      	al courts according to the somas 
2773829	80012	13	2	7	6944.07	0.01	0.05	A	F	1993-03-27	1993-02-13	1993-04-24	NONE                     	RAIL      	 requests sleep. reque
2773829	147154	2183	3	44	52850.60	0.10	0.04	A	F	1993-01-23	1993-02-19	1993-01-31	TAKE BACK RETURN         	SHIP      	er the blithely express accounts. foxes are
2773829	14697	4698	4	36	58020.84	0.01	0.08	A	F	1992-12-24	1993-01-15	1993-01-22	TAKE BACK RETURN         	SHIP      	c deposits. even ideas cajole careful
2773829	11415	8919	5	19	25201.79	0.02	0.00	R	F	1993-01-08	1993-02-02	1993-01-17	DELIVER IN PERSON        	FOB       	he carefully bold depen
2773829	50681	682	6	11	17948.48	0.03	0.07	A	F	1993-01-17	1993-01-06	1993-01-18	TAKE BACK RETURN         	TRUCK     	s haggle carefully dogg
2773829	190813	5852	7	39	74248.59	0.03	0.02	A	F	1993-03-06	1993-02-19	1993-03-14	COLLECT COD              	SHIP      	efully. fluffy requests sleep idly silen
2782659	30386	5393	1	6	7898.28	0.00	0.08	N	O	1997-01-30	1996-12-02	1997-02-12	TAKE BACK RETURN         	MAIL      	ke quickly pending,
2782659	88345	3362	2	17	22666.78	0.09	0.08	N	O	1997-01-03	1996-12-01	1997-01-19	NONE                     	TRUCK     	the foxes are after the stealthily
2790881	124714	2251	1	17	29558.07	0.10	0.06	N	O	1995-07-21	1995-07-06	1995-07-24	NONE                     	TRUCK     	s. regular platelets detect.
2790881	62207	7220	2	44	51444.80	0.10	0.03	N	O	1995-08-29	1995-07-21	1995-09-03	NONE                     	TRUCK     	to beans nag blithely. carefully fin
2790881	104775	2306	3	17	30256.09	0.05	0.05	N	O	1995-08-05	1995-07-19	1995-08-08	DELIVER IN PERSON        	TRUCK     	ackages cajole special, ironic pinto bea
2806528	52863	379	1	13	23606.18	0.07	0.05	R	F	1993-09-07	1993-08-04	1993-10-05	COLLECT COD              	FOB       	kages. quickly final asymptotes haggl
2806528	59729	9730	2	44	74303.68	0.02	0.03	A	F	1993-07-25	1993-09-16	1993-08-08	TAKE BACK RETURN         	REG AIR   	ven accoun
2826500	23530	1037	1	32	46512.96	0.07	0.08	R	F	1994-09-12	1994-09-28	1994-10-09	COLLECT COD              	RAIL      	ns after the carefu
2826500	61685	6698	2	33	54340.44	0.09	0.06	R	F	1994-10-15	1994-09-28	1994-11-03	NONE                     	TRUCK     	dolites use. idle pinto beans haggle a
2892036	137131	2158	1	42	49061.46	0.04	0.06	R	F	1992-07-16	1992-05-31	1992-08-05	DELIVER IN PERSON        	RAIL      	. brave tithes around th
2892036	145465	5466	2	7	10573.22	0.08	0.08	R	F	1992-04-22	1992-06-09	1992-05-04	NONE                     	MAIL      	c deposits doubt. fluffily unusu
2897441	32388	2389	1	16	21126.08	0.01	0.01	A	F	1993-04-11	1993-05-31	1993-04-16	TAKE BACK RETURN         	TRUCK     	ual instructions. blithely specia
2916320	165324	5325	1	2	2778.64	0.03	0.08	R	F	1994-09-14	1994-09-09	1994-10-01	TAKE BACK RETURN         	FOB       	ray carefully along the furiously pending
2916320	188855	3892	2	30	58315.50	0.08	0.02	A	F	1994-07-15	1994-08-08	1994-07-31	NONE                     	RAIL      	unts. regular ideas are quic
2916320	82458	2459	3	21	30249.45	0.01	0.05	A	F	1994-08-28	1994-10-05	1994-09-18	COLLECT COD              	AIR       	ular packages wak
2916320	58687	8688	4	31	51016.08	0.03	0.08	R	F	1994-08-24	1994-08-22	1994-08-30	NONE                     	RAIL      	g the final package
2916320	144154	4155	5	27	32350.05	0.05	0.02	A	F	1994-11-01	1994-09-18	1994-11-22	DELIVER IN PERSON        	MAIL      	old deposit
2916672	174122	4123	1	22	26314.64	0.07	0.08	A	F	1992-12-22	1993-02-04	1992-12-27	COLLECT COD              	TRUCK     	gle furiously final deposits. final 
2916672	56390	1401	2	42	56548.38	0.00	0.04	A	F	1993-01-21	1993-01-20	1993-02-16	COLLECT COD              	MAIL      	lites are aft
2916672	129969	7506	3	40	79958.40	0.07	0.03	A	F	1993-01-16	1993-02-20	1993-02-05	NONE                     	SHIP      	inst the i
2916672	182637	2638	4	9	15476.67	0.04	0.00	A	F	1992-12-31	1993-02-28	1993-01-16	DELIVER IN PERSON        	REG AIR   	its haggle bravely. pendi
2917345	12883	2884	1	19	34121.72	0.06	0.01	N	O	1995-12-04	1995-12-26	1995-12-26	DELIVER IN PERSON        	MAIL      	l frets. caref
2917345	114072	4073	2	10	10860.70	0.05	0.01	N	O	1995-10-27	1995-12-22	1995-11-10	TAKE BACK RETURN         	AIR       	ss deposits. express,
2917345	729	5730	3	6	9778.32	0.07	0.07	N	O	1995-10-21	1995-12-14	1995-11-05	NONE                     	SHIP      	sual instructions. fluf
2958117	153357	8388	1	2	2820.70	0.10	0.02	A	F	1993-03-01	1993-03-14	1993-03-17	TAKE BACK RETURN         	FOB       	e blithely regular accounts. f
2958117	69812	7331	2	27	48108.87	0.03	0.08	A	F	1993-02-04	1993-04-26	1993-03-03	DELIVER IN PERSON        	FOB       	pendencies. quickly final
2958117	129112	1625	3	4	4564.44	0.04	0.03	R	F	1993-02-09	1993-03-21	1993-02-23	COLLECT COD              	TRUCK     	kages sublat
2958117	183515	8552	4	14	22379.14	0.05	0.00	R	F	1993-03-14	1993-04-03	1993-04-08	TAKE BACK RETURN         	TRUCK     	structions haggle across th
2986913	85787	5788	1	3	5318.34	0.09	0.07	A	F	1994-08-12	1994-08-30	1994-09-10	DELIVER IN PERSON        	TRUCK     	lithely bold courts. even 
2986913	8369	8370	2	29	37043.44	0.06	0.01	R	F	1994-07-19	1994-08-23	1994-08-16	DELIVER IN PERSON        	REG AIR   	 regular pinto beans haggle across the furi
3012964	46760	4273	1	12	20481.12	0.03	0.00	R	F	1993-10-10	1993-08-30	1993-10-17	DELIVER IN PERSON        	TRUCK     	accounts. quickly 
3012964	92718	7737	2	31	53032.01	0.04	0.03	A	F	1993-08-25	1993-08-12	1993-09-16	TAKE BACK RETURN         	REG AIR   	 ironic packages. express packages wake
3012964	102490	21	3	11	16417.39	0.02	0.03	A	F	1993-09-02	1993-09-13	1993-09-19	NONE                     	TRUCK     	deposits. regular requests haggle qui
3012964	149232	4261	4	38	48686.74	0.07	0.00	R	F	1993-09-13	1993-08-02	1993-10-10	TAKE BACK RETURN         	SHIP      	ngside of the carefully silent depen
3017056	7213	2214	1	7	7841.47	0.07	0.02	R	F	1994-04-12	1994-03-27	1994-05-10	TAKE BACK RETURN         	REG AIR   	 even, ironic
3036614	139223	1737	1	28	35342.16	0.10	0.01	A	F	1993-01-04	1993-02-23	1993-01-08	DELIVER IN PERSON        	FOB       	ss excuses unwind q
3036614	109595	9596	2	41	65788.19	0.01	0.01	A	F	1993-02-23	1993-02-19	1993-02-25	TAKE BACK RETURN         	MAIL      	ending asymptotes
3036614	147849	364	3	26	49317.84	0.05	0.04	R	F	1993-04-01	1993-02-27	1993-04-17	DELIVER IN PERSON        	SHIP      	 slyly pending pinto beans. slyly silent d
3036614	82476	2477	4	37	53963.39	0.02	0.08	R	F	1993-01-08	1993-01-19	1993-01-28	DELIVER IN PERSON        	SHIP      	s platelets are ca
3051136	88273	3290	1	11	13873.97	0.03	0.02	R	F	1994-02-01	1994-03-23	1994-02-17	DELIVER IN PERSON        	RAIL      	ts above the slyly 
3069221	73817	8832	1	13	23280.53	0.03	0.05	N	O	1997-02-27	1997-01-11	1997-03-13	NONE                     	RAIL      	 final deposits sleep spe
3069221	34628	7132	2	2	3125.24	0.04	0.05	N	O	1997-01-31	1996-12-22	1997-02-07	NONE                     	SHIP      	inal pinto beans wake furiously
3120674	125589	614	1	31	50051.98	0.03	0.06	N	O	1996-08-30	1996-08-27	1996-09-15	NONE                     	TRUCK     	lly special theodolites.
3120674	166900	4449	2	46	90477.40	0.07	0.08	N	O	1996-09-11	1996-10-01	1996-10-08	NONE                     	FOB       	luffily pe
3120674	77402	9910	3	27	37243.80	0.07	0.03	N	O	1996-10-30	1996-09-13	1996-11-01	NONE                     	REG AIR   	ccounts. quickly bold requests ea
3176421	160221	222	1	15	19218.30	0.08	0.07	N	O	1996-01-10	1995-12-04	1996-01-26	DELIVER IN PERSON        	SHIP      	ic packages above the
3176421	31182	1183	2	19	21150.42	0.05	0.08	N	O	1995-11-05	1996-01-21	1995-11-23	COLLECT COD              	RAIL      	permanently bold pinto beans mol
3176421	75360	375	3	23	30713.28	0.09	0.04	N	O	1996-02-10	1996-01-12	1996-02-17	COLLECT COD              	FOB       	es. final, final dependencies nag final
3176421	174817	9852	4	40	75672.40	0.10	0.05	N	O	1996-02-01	1995-12-15	1996-02-28	TAKE BACK RETURN         	TRUCK     	ymptotes. b
3176421	147559	7560	5	10	16065.50	0.02	0.00	N	O	1995-11-22	1995-12-19	1995-12-15	DELIVER IN PERSON        	FOB       	ffily. unusual depe
3176421	19270	4273	6	12	14271.24	0.02	0.05	N	O	1995-12-06	1996-01-03	1995-12-14	DELIVER IN PERSON        	REG AIR   	se carefully according to the r
3251169	77605	7606	1	43	68051.80	0.04	0.00	N	O	1996-01-22	1996-03-01	1996-01-30	DELIVER IN PERSON        	MAIL      	eas. blithely
3251169	106088	6089	2	25	27352.00	0.05	0.00	N	O	1996-03-22	1996-04-05	1996-04-13	TAKE BACK RETURN         	TRUCK     	s cajole id
3251169	11860	6863	3	23	40752.78	0.02	0.04	N	O	1996-02-28	1996-03-15	1996-03-08	COLLECT COD              	TRUCK     	 carefully bold requests. ironic deposits c
3251169	124206	4207	4	48	59049.60	0.00	0.07	N	O	1996-03-01	1996-02-25	1996-03-25	TAKE BACK RETURN         	FOB       	es. quickly even 
3251169	129315	4340	5	23	30919.13	0.09	0.04	N	O	1996-02-23	1996-03-13	1996-03-05	DELIVER IN PERSON        	SHIP      	nding theodolites sleep quickly a
3252322	143414	5929	1	19	27690.79	0.09	0.01	A	F	1993-08-12	1993-06-29	1993-08-16	COLLECT COD              	REG AIR   	uriously blithely ironic requests. 
3252322	153182	8213	2	22	27173.96	0.01	0.08	R	F	1993-08-15	1993-08-02	1993-08-23	COLLECT COD              	AIR       	 fluffily unus
3252322	123983	1520	3	16	32111.68	0.09	0.05	A	F	1993-06-18	1993-07-09	1993-07-07	TAKE BACK RETURN         	SHIP      	ntiments poach car
3252322	168731	6280	4	13	23396.49	0.01	0.03	A	F	1993-07-30	1993-07-23	1993-08-29	NONE                     	TRUCK     	gular packages. pending, 
3252322	51452	1453	5	46	64558.70	0.03	0.07	A	F	1993-07-14	1993-07-23	1993-08-11	COLLECT COD              	SHIP      	into beans. silent dependenci
3252322	141170	8713	6	28	33912.76	0.05	0.01	A	F	1993-05-14	1993-07-25	1993-05-16	TAKE BACK RETURN         	AIR       	ash. ironic requests haggle ab
3268516	14616	7118	1	47	71938.67	0.05	0.03	N	O	1996-12-08	1997-02-21	1996-12-09	DELIVER IN PERSON        	SHIP      	y silent warhorses wake carefully 
3268516	47890	5403	2	17	31244.13	0.06	0.03	N	O	1996-12-03	1997-02-03	1996-12-23	COLLECT COD              	REG AIR   	ckages use final ideas. unusual, iron
3268516	74155	9170	3	41	46295.15	0.01	0.01	N	O	1997-01-09	1997-02-08	1997-01-22	DELIVER IN PERSON        	FOB       	ies are slyly according to the silent foxes
3330400	127251	4788	1	2	2556.50	0.08	0.02	N	O	1998-01-01	1997-10-10	1998-01-15	NONE                     	FOB       	hes wake across the slyly fin
3330400	104577	9598	2	32	50610.24	0.10	0.03	N	O	1997-10-25	1997-10-27	1997-10-28	NONE                     	REG AIR   	ckages. deposits sleep. final ideas use 
3354726	94998	2526	1	12	23915.88	0.02	0.05	N	O	1998-04-22	1998-03-25	1998-05-21	COLLECT COD              	TRUCK     	refully blithely ironic 
3354726	23666	8671	2	6	9537.96	0.00	0.01	N	O	1998-04-13	1998-03-11	1998-05-07	DELIVER IN PERSON        	SHIP      	ptotes detect blithel
3354726	142776	319	3	39	70932.03	0.00	0.08	N	O	1998-03-13	1998-02-13	1998-04-01	DELIVER IN PERSON        	FOB       	e along the fluffily fi
3354726	31938	1939	4	8	14959.44	0.00	0.07	N	O	1998-05-04	1998-03-06	1998-05-08	TAKE BACK RETURN         	FOB       	sauternes use. iro
3354726	165382	5383	5	23	33289.74	0.06	0.00	N	O	1998-01-27	1998-03-27	1998-02-08	TAKE BACK RETURN         	REG AIR   	s. sly deposits ab
3368547	194129	4130	1	21	25685.52	0.01	0.00	N	O	1997-06-27	1997-06-17	1997-06-28	DELIVER IN PERSON        	TRUCK     	wake furiously
3368547	109987	7518	2	18	35945.64	0.05	0.04	N	O	1997-08-09	1997-08-07	1997-08-28	COLLECT COD              	SHIP      	cial ideas. blithely pending theodol
3368547	136241	8755	3	27	34485.48	0.08	0.07	N	O	1997-08-24	1997-07-22	1997-09-13	COLLECT COD              	RAIL      	integrate regular, sile
3368547	196983	2022	4	1	2079.98	0.02	0.01	N	O	1997-06-19	1997-07-05	1997-07-16	DELIVER IN PERSON        	REG AIR   	ously final foxes. instr
3368547	81784	4293	5	44	77694.32	0.07	0.06	N	O	1997-05-25	1997-06-29	1997-06-09	COLLECT COD              	MAIL      	equests. fluffily ironic d
3368547	139890	9891	6	20	38597.80	0.08	0.00	N	O	1997-07-29	1997-07-24	1997-08-24	NONE                     	MAIL      	ix according to the silent, 
3368547	117583	95	7	16	25609.28	0.06	0.08	N	O	1997-06-28	1997-06-13	1997-07-23	COLLECT COD              	SHIP      	ly even ideas wake furiously carefully unus
3368833	118854	8855	1	41	76786.85	0.01	0.01	N	O	1998-01-20	1998-02-08	1998-01-29	COLLECT COD              	MAIL      	 of the fluffy accounts. furiously ironic
3368833	170598	5633	2	3	5005.77	0.09	0.06	N	O	1998-04-05	1998-01-31	1998-05-04	COLLECT COD              	FOB       	carefully regu
3378979	197416	4974	1	13	19674.33	0.05	0.06	N	O	1996-10-15	1996-11-07	1996-10-17	NONE                     	TRUCK     	lent requests after the
3378979	162131	7164	2	11	13124.43	0.09	0.06	N	O	1996-11-21	1996-10-14	1996-12-07	DELIVER IN PERSON        	AIR       	deas. blithely unusual 
3378979	131167	8707	3	22	26359.52	0.07	0.08	N	O	1996-12-04	1996-11-07	1996-12-05	COLLECT COD              	REG AIR   	nic, express warthogs. slyly ev
3378979	123264	8289	4	33	42479.58	0.06	0.03	N	O	1996-08-20	1996-10-20	1996-09-11	DELIVER IN PERSON        	TRUCK     	s wake blithely. ev
3421092	98162	672	1	36	41765.76	0.04	0.03	N	O	1998-07-29	1998-06-20	1998-08-10	COLLECT COD              	REG AIR   	sly bold asymptotes
3421092	7783	5284	2	38	64249.64	0.01	0.03	N	O	1998-07-18	1998-07-14	1998-07-31	NONE                     	TRUCK     	dly alongside of the spe
3421092	3024	5525	3	42	38934.84	0.08	0.00	N	O	1998-07-21	1998-08-02	1998-07-27	NONE                     	MAIL      	ly according to the busy, special 
3421092	16851	6852	4	5	8839.25	0.07	0.05	N	O	1998-08-29	1998-06-18	1998-09-03	COLLECT COD              	AIR       	ar pinto bea
3421092	4548	9549	5	21	30503.34	0.07	0.07	N	O	1998-08-27	1998-08-13	1998-09-07	NONE                     	REG AIR   	ubt fluffily bold excuses. 
3440902	111325	3837	1	6	8017.92	0.00	0.07	R	F	1992-11-01	1992-08-19	1992-11-17	DELIVER IN PERSON        	RAIL      	l pinto beans. fluffily even ide
3440902	34727	7231	2	26	43204.72	0.10	0.00	A	F	1992-09-04	1992-08-26	1992-09-13	TAKE BACK RETURN         	AIR       	y bold ideas nag carefu
3465376	141158	1159	1	27	32377.05	0.03	0.03	N	O	1995-07-28	1995-06-14	1995-08-19	NONE                     	RAIL      	side of the thinly even acc
3487745	195005	5006	1	42	46200.00	0.02	0.04	N	O	1997-01-23	1996-11-06	1997-02-04	TAKE BACK RETURN         	TRUCK     	s use. final dolphins should have
3487745	37824	328	2	28	49330.96	0.02	0.02	N	O	1996-12-19	1996-12-25	1996-12-27	NONE                     	TRUCK     	eposits. pending asymptotes according to th
3487745	92415	4925	3	27	38000.07	0.10	0.08	N	O	1996-11-20	1996-12-15	1996-11-27	COLLECT COD              	FOB       	xes. blithely unusual accoun
3487745	17849	2852	4	35	61839.40	0.07	0.04	N	O	1996-12-03	1996-12-02	1996-12-23	DELIVER IN PERSON        	AIR       	nal, idle 
3504453	111728	1729	1	21	36534.12	0.03	0.03	N	O	1997-06-29	1997-06-03	1997-07-25	TAKE BACK RETURN         	RAIL      	egular packages: furiously
3504453	198544	8545	2	25	41063.50	0.07	0.01	N	O	1997-05-23	1997-05-25	1997-06-05	TAKE BACK RETURN         	MAIL      	s serve bravely ir
3504453	126277	1302	3	22	28671.94	0.00	0.04	N	O	1997-06-13	1997-07-20	1997-07-10	DELIVER IN PERSON        	MAIL      	are carefully blithely express foxes. 
3504453	129457	9458	4	6	8918.70	0.03	0.08	N	O	1997-05-28	1997-06-20	1997-06-01	TAKE BACK RETURN         	SHIP      	l pinto beans. instructions use car
3504453	179923	2441	5	49	98143.08	0.07	0.07	N	O	1997-06-24	1997-06-03	1997-07-04	NONE                     	REG AIR   	lar theodo
3504453	160485	3002	6	24	37091.52	0.01	0.07	N	O	1997-07-30	1997-07-16	1997-08-05	NONE                     	SHIP      	to beans wake furiously i
3540450	11713	1714	1	2	3249.42	0.08	0.00	R	F	1994-09-09	1994-10-23	1994-09-29	COLLECT COD              	FOB       	luffily even 
3540450	48824	6337	2	8	14182.56	0.00	0.02	A	F	1994-09-05	1994-10-19	1994-09-07	COLLECT COD              	REG AIR   	 carefully flu
3540450	184302	4303	3	3	4158.90	0.05	0.06	A	F	1994-10-18	1994-11-03	1994-11-06	TAKE BACK RETURN         	SHIP      	eas. furiously even accounts ab
3554659	125731	3268	1	27	47431.71	0.06	0.00	A	F	1995-03-02	1995-02-27	1995-04-01	TAKE BACK RETURN         	RAIL      	efully. blithely regular dep
3554659	146946	1975	2	40	79717.60	0.00	0.01	R	F	1994-12-31	1995-01-16	1995-01-05	COLLECT COD              	RAIL      	 packages kindle furiously acco
3568032	151255	3771	1	13	16981.25	0.04	0.00	N	O	1998-01-03	1997-11-05	1998-01-27	NONE                     	REG AIR   	al, regular instructions ca
3568032	133162	8189	2	28	33464.48	0.09	0.05	N	O	1997-09-22	1997-10-17	1997-10-05	COLLECT COD              	TRUCK     	es. furiously ev
3568032	45082	91	3	21	21568.68	0.04	0.06	N	O	1997-10-28	1997-11-13	1997-11-13	TAKE BACK RETURN         	REG AIR   	d carefully blithely pending foxes. regula
3580034	72757	2758	1	3	5189.25	0.08	0.03	A	F	1992-10-22	1992-08-26	1992-11-09	TAKE BACK RETURN         	MAIL      	 instruction
3580034	13497	3498	2	6	8462.94	0.02	0.04	R	F	1992-10-07	1992-09-20	1992-10-21	NONE                     	MAIL      	ins nag according to
3580034	32403	2404	3	17	22701.80	0.08	0.00	A	F	1992-08-07	1992-08-22	1992-08-12	TAKE BACK RETURN         	AIR       	arefully express ideas. carefully spe
3580034	183536	6055	4	45	72878.85	0.06	0.01	A	F	1992-08-21	1992-10-03	1992-08-23	TAKE BACK RETURN         	AIR       	y regular acco
3580034	84564	9581	5	31	48005.36	0.02	0.03	R	F	1992-08-10	1992-08-15	1992-09-08	COLLECT COD              	RAIL      	olites against 
3580034	53570	3571	6	33	50277.81	0.02	0.05	A	F	1992-08-29	1992-09-17	1992-09-02	DELIVER IN PERSON        	RAIL      	special warhorses. furiously ironic courts 
3580034	148252	8253	7	20	26005.00	0.00	0.00	R	F	1992-09-09	1992-08-29	1992-10-09	TAKE BACK RETURN         	FOB       	 silent realms. special, silent fo
3628518	83361	8378	1	44	59151.84	0.02	0.01	N	O	1995-12-09	1996-02-04	1996-01-01	TAKE BACK RETURN         	AIR       	 accounts 
3628518	35445	7949	2	46	63500.24	0.03	0.04	N	O	1996-01-26	1996-01-13	1996-02-13	TAKE BACK RETURN         	TRUCK     	ously unusual platelets. furiously
3628518	107856	2877	3	19	35413.15	0.03	0.00	N	O	1996-01-17	1995-12-14	1996-02-10	TAKE BACK RETURN         	SHIP      	se slyly. blithely express instru
3628518	135271	298	4	14	18287.78	0.02	0.01	N	O	1995-12-18	1995-12-22	1996-01-06	NONE                     	MAIL      	s sleep quickly alongside of the 
3628518	91672	6691	5	14	23291.38	0.01	0.04	N	O	1995-11-29	1995-12-07	1995-12-13	DELIVER IN PERSON        	FOB       	into beans cajole final pac
3631968	165201	234	1	38	48115.60	0.08	0.02	A	F	1994-04-02	1994-04-14	1994-04-22	NONE                     	AIR       	 use furiously f
3631968	119219	4242	2	20	24764.20	0.02	0.06	R	F	1994-05-08	1994-03-17	1994-06-02	NONE                     	AIR       	to beans affix alongside of the blit
3631968	103610	1141	3	24	38726.64	0.05	0.06	A	F	1994-04-10	1994-04-01	1994-04-29	DELIVER IN PERSON        	SHIP      	 quickly regular deposits s
3631968	164864	2413	4	30	57865.80	0.10	0.04	A	F	1994-04-22	1994-02-28	1994-04-30	NONE                     	REG AIR   	ly regular deposits. furiously ironic i
3631968	68668	1175	5	29	47463.14	0.05	0.06	A	F	1994-02-10	1994-02-28	1994-02-25	DELIVER IN PERSON        	AIR       	ual courts
3631968	93794	1322	6	24	42906.96	0.10	0.06	R	F	1994-02-24	1994-03-19	1994-03-16	NONE                     	REG AIR   	l pinto beans play. even, silent reque
3640352	168411	3444	1	8	11835.28	0.06	0.05	A	F	1994-11-13	1995-01-09	1994-11-23	TAKE BACK RETURN         	RAIL      	ix carefully. regul
3640352	82498	23	2	28	41453.72	0.06	0.06	A	F	1994-12-25	1994-12-13	1995-01-21	TAKE BACK RETURN         	SHIP      	uses. fluffily special acc
3640352	139894	9895	3	34	65752.26	0.10	0.07	A	F	1994-10-25	1995-01-12	1994-11-21	COLLECT COD              	FOB       	nic accounts.
3640352	193234	5754	4	34	45125.82	0.03	0.05	R	F	1994-11-18	1994-12-22	1994-12-11	TAKE BACK RETURN         	RAIL      	lithely bl
3640352	80368	7893	5	32	43147.52	0.01	0.04	R	F	1994-12-21	1994-12-17	1994-12-30	DELIVER IN PERSON        	REG AIR   	 dependencies affix
3640352	135571	3111	6	13	20885.41	0.06	0.08	A	F	1995-01-31	1995-01-10	1995-02-05	DELIVER IN PERSON        	TRUCK     	lly final requests across the specia
3640352	137135	4675	7	19	22270.47	0.10	0.05	R	F	1995-02-08	1995-01-08	1995-02-17	NONE                     	TRUCK     	the foxes. pending, unusual foxes
3671879	191746	1747	1	46	84536.04	0.10	0.04	N	O	1997-06-26	1997-06-07	1997-07-25	NONE                     	SHIP      	ites are abo
3671879	64188	6695	2	37	42630.66	0.06	0.00	N	O	1997-05-13	1997-06-10	1997-05-25	NONE                     	REG AIR   	. quickly silent pa
3671879	196779	1818	3	40	75030.80	0.06	0.01	N	O	1997-08-23	1997-06-17	1997-09-05	TAKE BACK RETURN         	MAIL      	s. unusual deposits haggle blithely. tithe
3671879	54329	4330	4	2	2566.64	0.06	0.01	N	O	1997-07-10	1997-06-14	1997-07-31	TAKE BACK RETURN         	MAIL      	 pending theodolites cajo
3683623	87587	5112	1	25	39364.50	0.08	0.06	N	O	1997-01-13	1997-02-25	1997-01-24	DELIVER IN PERSON        	TRUCK     	ding packages. furiously ev
3683623	62217	7230	2	3	3537.63	0.00	0.00	N	O	1997-02-25	1997-01-13	1997-03-06	COLLECT COD              	MAIL      	nts nag slyly? f
3683623	74889	2411	3	33	61508.04	0.03	0.08	N	O	1997-01-08	1997-02-04	1997-01-14	NONE                     	SHIP      	lly regular packages
3683623	54352	6858	4	35	45722.25	0.03	0.05	N	O	1997-02-01	1997-02-27	1997-02-26	COLLECT COD              	REG AIR   	dencies cajole. quickly special foxe
3696386	195638	3196	1	43	74546.09	0.09	0.00	N	O	1998-09-06	1998-07-21	1998-09-25	NONE                     	REG AIR   	er the final pinto beans sleep caref
3696386	76320	8828	2	27	35000.64	0.05	0.07	N	O	1998-08-04	1998-08-25	1998-08-22	DELIVER IN PERSON        	REG AIR   	iously bold 
3696386	101983	7004	3	35	69474.30	0.10	0.05	N	O	1998-07-13	1998-07-21	1998-07-31	NONE                     	TRUCK     	 orbits cajo
3696386	13025	529	4	8	7504.16	0.00	0.06	N	O	1998-06-22	1998-09-08	1998-07-20	DELIVER IN PERSON        	REG AIR   	blithely final deposits 
3696386	102686	2687	5	1	1688.68	0.02	0.02	N	O	1998-07-06	1998-08-07	1998-07-29	TAKE BACK RETURN         	MAIL      	carefully exp
3727234	170041	42	1	1	1111.04	0.09	0.08	N	O	1997-06-09	1997-03-25	1997-07-04	NONE                     	TRUCK     	ending dependencies-- a
3727234	78421	3436	2	34	47580.28	0.04	0.01	N	O	1997-06-07	1997-04-27	1997-06-19	TAKE BACK RETURN         	TRUCK     	ly regular foxes wa
3727234	5621	5622	3	37	56484.94	0.03	0.06	N	O	1997-02-26	1997-05-09	1997-03-06	COLLECT COD              	TRUCK     	dencies are sly
3727234	70665	5680	4	11	17992.26	0.07	0.03	N	O	1997-06-07	1997-04-27	1997-06-16	DELIVER IN PERSON        	RAIL      	ncies boost never. regula
3727234	64031	9044	5	32	31840.96	0.00	0.06	N	O	1997-04-03	1997-04-26	1997-04-18	DELIVER IN PERSON        	AIR       	es. platelets sleep fluffily. slyly ironic 
3732196	66368	3887	1	45	60046.20	0.09	0.03	N	O	1997-02-05	1996-12-16	1997-02-06	DELIVER IN PERSON        	TRUCK     	 unusual pinto beans wake furiously f
3732196	32308	4812	2	4	4961.20	0.06	0.08	N	O	1996-12-23	1997-01-15	1997-01-10	TAKE BACK RETURN         	SHIP      	e packages engage furiously i
3732196	186448	4003	3	34	52170.96	0.01	0.01	N	O	1997-02-06	1997-01-24	1997-02-23	TAKE BACK RETURN         	RAIL      	gular ideas c
3732196	25826	831	4	7	12262.74	0.08	0.02	N	O	1996-12-31	1997-01-05	1997-01-29	TAKE BACK RETURN         	AIR       	lly regular foxes sleep regularly ironic, u
3742659	100208	5229	1	29	35037.80	0.02	0.06	R	F	1994-04-22	1994-05-12	1994-05-06	DELIVER IN PERSON        	SHIP      	silent ideas about the regular, bold 
3765443	44508	4509	1	35	50837.50	0.09	0.08	N	O	1995-08-09	1995-09-08	1995-08-11	TAKE BACK RETURN         	AIR       	nding packages cajole. fl
3765443	34610	2120	2	22	33981.42	0.06	0.04	N	O	1995-08-01	1995-08-16	1995-08-02	TAKE BACK RETURN         	RAIL      	ests nag carefully.
3765443	84180	1705	3	36	41910.48	0.00	0.05	N	O	1995-08-05	1995-07-21	1995-08-25	NONE                     	RAIL      	refully after the final, regular accoun
3765443	2202	4703	4	14	15458.80	0.03	0.02	N	O	1995-07-29	1995-07-23	1995-08-19	COLLECT COD              	REG AIR   	kages. carefully silent asymptot
3765443	197339	4897	5	19	27290.27	0.09	0.07	N	O	1995-07-13	1995-08-19	1995-08-01	COLLECT COD              	TRUCK     	are among t
3765443	71450	3958	6	5	7107.25	0.07	0.08	N	O	1995-07-28	1995-09-16	1995-08-24	NONE                     	FOB       	thogs haggle carefully around the pendi
3765443	55759	3275	7	28	48013.00	0.06	0.04	N	O	1995-10-07	1995-09-09	1995-10-20	NONE                     	RAIL      	ns. doggedly even theodolites doze qui
3768484	20988	3491	1	6	11453.88	0.04	0.06	A	F	1994-08-24	1994-07-16	1994-09-22	COLLECT COD              	MAIL      	lites integrate. furiously 
3768484	140755	3270	2	37	66442.75	0.01	0.02	R	F	1994-07-25	1994-07-18	1994-08-06	TAKE BACK RETURN         	FOB       	ages sleep furiously even
3772225	26749	9252	1	36	60326.64	0.05	0.05	A	F	1993-06-26	1993-07-22	1993-07-21	TAKE BACK RETURN         	REG AIR   	thely ironic
3772225	154690	7206	2	12	20936.28	0.01	0.07	R	F	1993-06-11	1993-07-31	1993-06-24	COLLECT COD              	SHIP      	ns. quickl
3772225	66560	4079	3	46	70221.76	0.02	0.04	R	F	1993-07-26	1993-07-16	1993-08-04	TAKE BACK RETURN         	SHIP      	ial packages haggle accor
3772225	64670	7177	4	26	42501.42	0.01	0.04	R	F	1993-06-14	1993-07-20	1993-07-10	DELIVER IN PERSON        	SHIP      	 deposits. slyly ironic cour
3772225	103099	8120	5	4	4408.36	0.00	0.00	R	F	1993-07-26	1993-08-14	1993-08-10	COLLECT COD              	RAIL      	heodolites sleep slyly even packages. c
3795685	716	8217	1	45	72751.95	0.01	0.05	N	O	1996-07-05	1996-06-30	1996-07-18	NONE                     	MAIL      	ly ironic 
3795685	22212	4715	2	8	9073.68	0.02	0.07	N	O	1996-05-26	1996-06-29	1996-06-14	TAKE BACK RETURN         	SHIP      	ial theodolites. furiously bold pint
3795685	100972	5993	3	39	76945.83	0.05	0.04	N	O	1996-08-02	1996-06-02	1996-08-25	NONE                     	FOB       	ular packages
3795685	25109	7612	4	47	48602.70	0.08	0.04	N	O	1996-06-13	1996-06-15	1996-07-11	COLLECT COD              	TRUCK     	accounts use furiously. quickly unusual r
3795685	6050	8551	5	42	40154.10	0.05	0.07	N	O	1996-05-21	1996-05-29	1996-05-26	COLLECT COD              	FOB       	ckey players use
3837959	117308	4842	1	44	58313.20	0.07	0.06	N	O	1997-07-22	1997-09-06	1997-08-05	NONE                     	REG AIR   	ep above the caref
3837959	186729	9248	2	10	18157.20	0.00	0.01	N	O	1997-07-16	1997-08-05	1997-08-01	DELIVER IN PERSON        	REG AIR   	ptotes. furiou
3840420	34432	1942	1	5	6832.15	0.00	0.03	N	O	1996-01-09	1996-03-02	1996-01-15	NONE                     	TRUCK     	requests about the furiously final saute
3840420	87033	4558	2	47	47941.41	0.04	0.01	N	O	1996-04-28	1996-03-02	1996-05-02	TAKE BACK RETURN         	MAIL      	olphins wake 
3840420	148543	1058	3	26	41380.04	0.02	0.04	N	O	1996-02-14	1996-03-05	1996-02-28	NONE                     	REG AIR   	e the slyl
3840420	161178	8727	4	15	18587.55	0.10	0.06	N	O	1996-03-27	1996-02-21	1996-03-29	TAKE BACK RETURN         	SHIP      	uickly reg
3840420	110179	5202	5	8	9513.36	0.09	0.07	N	O	1996-05-06	1996-03-04	1996-05-31	TAKE BACK RETURN         	REG AIR   	ravely ironic courts. re
3840420	178355	873	6	2	2866.70	0.09	0.02	N	O	1996-01-08	1996-03-03	1996-02-04	COLLECT COD              	RAIL      	nic deposits are permanently about the 
3851905	74229	9244	1	47	56551.34	0.05	0.05	A	F	1994-01-22	1994-02-04	1994-01-29	DELIVER IN PERSON        	FOB       	was carefully quickly ironic acco
3851905	159234	1750	2	8	10345.84	0.05	0.05	A	F	1994-02-21	1994-02-05	1994-02-23	TAKE BACK RETURN         	FOB       	en, ironic pinto beans 
3851905	30311	2815	3	19	23584.89	0.04	0.04	A	F	1994-02-02	1994-01-30	1994-02-16	DELIVER IN PERSON        	FOB       	ly even accounts wake fu
3851905	22192	4695	4	22	24512.18	0.01	0.00	R	F	1994-01-25	1994-03-10	1994-01-28	TAKE BACK RETURN         	FOB       	r ideas serve. silent, 
3851905	168065	582	5	30	33991.80	0.09	0.01	R	F	1994-04-01	1994-03-03	1994-04-27	NONE                     	RAIL      	 the final accoun
3851905	54830	9841	6	48	85671.84	0.02	0.00	A	F	1994-03-11	1994-03-26	1994-03-25	DELIVER IN PERSON        	RAIL      	e fluffily unusual packages. fluffi
3872644	109564	9565	1	34	53501.04	0.06	0.00	A	F	1994-12-22	1995-01-18	1995-01-17	NONE                     	REG AIR   	uriously special packages. furiously d
3872644	32920	2921	2	14	25940.88	0.09	0.05	R	F	1994-12-25	1995-01-20	1995-01-06	NONE                     	SHIP      	s cajole slyly along the
3916288	88086	8087	1	15	16111.20	0.02	0.07	N	O	1997-11-11	1997-09-07	1997-12-08	COLLECT COD              	TRUCK     	e blithely express platelets; requests 
3916288	58604	3615	2	8	12500.80	0.01	0.01	N	O	1997-10-21	1997-08-30	1997-11-12	DELIVER IN PERSON        	TRUCK     	s. blithely unusual packages above the car
3916288	154756	2302	3	30	54322.50	0.10	0.00	N	O	1997-07-23	1997-09-21	1997-08-15	DELIVER IN PERSON        	TRUCK     	al instruct
3916288	178978	4013	4	6	12341.82	0.09	0.01	N	O	1997-11-17	1997-10-10	1997-12-01	DELIVER IN PERSON        	REG AIR   	uests. packages nag furiously ironic t
3916288	77571	2586	5	7	10839.99	0.05	0.00	N	O	1997-10-15	1997-08-19	1997-11-02	DELIVER IN PERSON        	MAIL      	ronic accounts are. blithely regu
3918433	152597	7628	1	47	77530.73	0.00	0.03	R	F	1993-12-04	1993-10-02	1993-12-12	NONE                     	SHIP      	ect. carefully pending asympt
3918433	81736	9261	2	7	12024.11	0.02	0.05	A	F	1993-09-25	1993-11-22	1993-10-02	COLLECT COD              	RAIL      	es sleep. carefully bold packages haggle 
3918433	134807	9834	3	18	33152.40	0.10	0.06	A	F	1993-10-14	1993-10-22	1993-10-29	TAKE BACK RETURN         	AIR       	e the orbits wake outsi
3918433	88392	8393	4	29	40031.31	0.00	0.06	A	F	1993-09-17	1993-10-31	1993-09-18	COLLECT COD              	TRUCK     	ar instruc
3933538	11760	6763	1	9	15045.84	0.05	0.05	A	F	1992-12-09	1992-12-11	1992-12-10	TAKE BACK RETURN         	RAIL      	unts. slyly regular accounts hagg
3933538	43213	3214	2	35	40467.35	0.07	0.06	A	F	1993-02-05	1993-01-24	1993-02-06	DELIVER IN PERSON        	FOB       	ckly regular deposi
3933538	80259	5276	3	34	42134.50	0.09	0.04	R	F	1993-02-21	1993-01-05	1993-03-17	COLLECT COD              	AIR       	kages. theodolites sleep acros
3933538	101811	4322	4	37	67073.97	0.00	0.07	R	F	1993-03-11	1993-01-29	1993-03-18	TAKE BACK RETURN         	AIR       	refully special packages wake carefu
3933538	123069	606	5	33	36037.98	0.00	0.00	A	F	1993-01-17	1993-02-04	1993-02-15	DELIVER IN PERSON        	MAIL      	es snooze quick
3933538	16882	6883	6	38	68357.44	0.05	0.03	A	F	1992-12-11	1992-12-30	1992-12-15	TAKE BACK RETURN         	AIR       	 blithely unusual foxe
3934949	20445	446	1	31	42328.64	0.05	0.03	A	F	1995-03-19	1995-03-01	1995-03-23	TAKE BACK RETURN         	TRUCK     	. carefully regular r
3934949	4487	6988	2	45	62616.60	0.01	0.02	A	F	1995-01-12	1995-01-15	1995-02-11	COLLECT COD              	SHIP      	ronic request
3934949	250	251	3	14	16103.50	0.00	0.04	A	F	1995-03-25	1995-02-16	1995-04-05	TAKE BACK RETURN         	TRUCK     	ly permanent deposits 
3934949	193039	597	4	34	38489.02	0.05	0.03	A	F	1995-03-22	1995-01-06	1995-04-05	COLLECT COD              	TRUCK     	s; regular instructions sle
3938022	169275	4308	1	27	36295.29	0.00	0.00	N	O	1997-04-25	1997-04-04	1997-05-23	NONE                     	MAIL      	pinto beans. final pinto bean
3942656	177966	7967	1	10	20439.60	0.05	0.03	R	F	1992-10-22	1992-10-01	1992-11-04	DELIVER IN PERSON        	REG AIR   	quests. furiously unusual
3942656	70612	8134	2	45	71217.45	0.07	0.05	R	F	1992-10-15	1992-09-25	1992-10-25	TAKE BACK RETURN         	RAIL      	s integrate ironic pinto bea
3942656	173519	3520	3	41	65292.91	0.09	0.01	A	F	1992-10-26	1992-10-08	1992-11-21	NONE                     	RAIL      	sts impress furiously. carefully silent
3942656	120888	8425	4	50	95444.00	0.01	0.00	A	F	1992-12-14	1992-10-18	1992-12-16	DELIVER IN PERSON        	REG AIR   	ctions. regular dep
3942656	148816	8817	5	50	93240.50	0.10	0.00	A	F	1992-09-05	1992-09-27	1992-09-15	DELIVER IN PERSON        	TRUCK     	ns detect; slyly final theodolites betw
3950849	54741	4742	1	50	84787.00	0.01	0.06	N	O	1997-03-12	1997-02-20	1997-03-19	NONE                     	RAIL      	ounts. furiously unusual asymptotes 
3950849	30500	3004	2	38	54359.00	0.02	0.03	N	O	1997-01-13	1997-02-05	1997-02-12	NONE                     	FOB       	ven instruct
3950849	197489	7490	3	9	14278.32	0.05	0.00	N	O	1996-12-30	1997-02-07	1997-01-15	COLLECT COD              	MAIL      	ubt quickly. b
3950849	190087	88	4	24	28249.92	0.07	0.08	N	O	1997-02-10	1997-03-04	1997-03-01	COLLECT COD              	TRUCK     	accounts wake slyly along the f
3950849	183820	6339	5	7	13326.74	0.08	0.07	N	O	1997-03-19	1997-03-10	1997-04-16	NONE                     	MAIL      	arefully unusual requests wa
3951331	135093	7607	1	18	20305.62	0.01	0.05	N	O	1998-02-08	1998-02-07	1998-03-10	NONE                     	SHIP      	 furiously from the slyly care
3951331	93639	3640	2	27	44081.01	0.08	0.07	N	O	1998-02-21	1998-01-14	1998-03-16	COLLECT COD              	TRUCK     	lyly express platelets ha
3951331	183141	3142	3	10	12241.40	0.08	0.03	N	O	1998-01-21	1998-01-11	1998-02-03	DELIVER IN PERSON        	TRUCK     	usly speci
3952544	113201	3202	1	38	46139.60	0.04	0.06	N	O	1997-01-30	1997-03-15	1997-02-17	DELIVER IN PERSON        	AIR       	 furiously bold d
3952544	160543	8092	2	5	8017.70	0.02	0.01	N	O	1997-05-02	1997-04-16	1997-05-03	COLLECT COD              	AIR       	ts nag according to the final, fin
3952544	33401	911	3	17	22684.80	0.09	0.00	N	O	1997-02-26	1997-03-18	1997-03-19	NONE                     	SHIP      	ess deposi
3952544	164851	2400	4	24	45980.40	0.00	0.01	N	O	1997-04-28	1997-03-09	1997-05-15	COLLECT COD              	REG AIR   	platelets snooze furiously quick
4009575	90034	7562	1	42	43009.26	0.10	0.07	A	F	1993-03-24	1993-03-07	1993-04-11	DELIVER IN PERSON        	MAIL      	are carefully. packages abo
4009575	186599	9118	2	48	80908.32	0.02	0.07	R	F	1993-04-29	1993-03-07	1993-05-25	TAKE BACK RETURN         	FOB       	e carefully. express pa
4114983	198611	3650	1	5	8548.05	0.01	0.03	N	O	1997-10-28	1997-09-25	1997-11-02	NONE                     	TRUCK     	ajole carefull
4129186	7836	2837	1	4	6975.32	0.09	0.00	N	O	1996-05-18	1996-07-16	1996-05-31	TAKE BACK RETURN         	AIR       	ld foxes wake fluffily above
4141668	22659	7664	1	23	36377.95	0.03	0.02	A	F	1995-05-22	1995-05-19	1995-06-08	COLLECT COD              	AIR       	ly even pearls boost fur
4141668	27438	4945	2	22	30039.46	0.06	0.01	R	F	1995-05-17	1995-05-29	1995-06-10	COLLECT COD              	SHIP      	arefully special
4141668	137826	7827	3	41	76416.62	0.08	0.03	R	F	1995-04-04	1995-06-13	1995-04-05	COLLECT COD              	TRUCK     	counts nod bl
4141668	34541	4542	4	22	32461.88	0.10	0.08	A	F	1995-05-20	1995-05-26	1995-06-05	NONE                     	SHIP      	n patterns. final, unusual 
4143526	120440	441	1	1	1460.44	0.06	0.06	N	O	1996-10-09	1996-10-27	1996-10-12	NONE                     	TRUCK     	s sublate furi
4143526	22520	7525	2	8	11540.16	0.05	0.07	N	O	1996-10-02	1996-11-15	1996-10-28	TAKE BACK RETURN         	TRUCK     	ironic requests haggle quickly ev
4143526	91841	1842	3	45	82477.80	0.06	0.05	N	O	1997-01-13	1996-10-20	1997-01-29	DELIVER IN PERSON        	REG AIR   	en accounts above the silently ironic 
4143526	162939	488	4	17	34032.81	0.03	0.04	N	O	1997-01-07	1996-10-15	1997-01-24	COLLECT COD              	SHIP      	ly after the blithely ironic
4143526	107296	2317	5	22	28672.38	0.01	0.00	N	O	1996-10-30	1996-10-18	1996-11-17	DELIVER IN PERSON        	SHIP      	ng to the carefully 
4144037	66761	1774	1	47	81204.72	0.02	0.03	N	O	1998-08-09	1998-06-14	1998-08-21	TAKE BACK RETURN         	SHIP      	t the regular forges
4144037	45599	3112	2	4	6178.36	0.06	0.02	N	O	1998-07-20	1998-07-15	1998-08-18	COLLECT COD              	AIR       	 alongside of the furiously idle gifts 
4144037	175274	7792	3	16	21588.32	0.03	0.02	N	O	1998-05-16	1998-07-08	1998-06-03	NONE                     	SHIP      	ly silent deposits above 
4144037	29807	9808	4	19	32999.20	0.08	0.05	N	O	1998-05-17	1998-06-25	1998-06-14	NONE                     	MAIL      	e ironically s
4144037	165352	5353	5	12	17008.20	0.07	0.01	N	O	1998-08-19	1998-06-02	1998-08-21	TAKE BACK RETURN         	SHIP      	carefully ironic pains. eve
4144037	64015	4016	6	12	11748.12	0.05	0.06	N	O	1998-07-06	1998-06-15	1998-07-18	DELIVER IN PERSON        	MAIL      	 final accounts wake quickly final accounts
4148227	17808	310	1	46	79386.80	0.01	0.07	N	O	1995-08-27	1995-08-24	1995-08-29	NONE                     	MAIL      	y pending ideas sle
4148227	84221	9238	2	50	60261.00	0.00	0.02	N	O	1995-10-10	1995-08-31	1995-11-08	DELIVER IN PERSON        	SHIP      	under the doggedly enticing ep
4148227	93032	5542	3	35	35876.05	0.08	0.03	N	O	1995-11-06	1995-09-20	1995-11-20	DELIVER IN PERSON        	REG AIR   	y ironic accounts. deposi
4148227	185283	7802	4	45	61572.60	0.06	0.03	N	O	1995-10-10	1995-09-20	1995-10-22	DELIVER IN PERSON        	REG AIR   	, final deposits cajole. ironic excuses
4148227	50998	6009	5	37	72112.63	0.07	0.02	N	O	1995-09-02	1995-10-06	1995-09-05	NONE                     	MAIL      	le. blithely bold dependencies nag b
4148227	119443	1955	6	47	68734.68	0.03	0.06	N	O	1995-10-05	1995-08-23	1995-10-23	DELIVER IN PERSON        	FOB       	ptotes detec
4150085	100455	7986	1	47	68406.15	0.05	0.06	A	F	1993-03-22	1993-03-27	1993-04-20	NONE                     	MAIL      	requests ca
4150085	144304	9333	2	27	36404.10	0.03	0.00	R	F	1993-03-17	1993-02-21	1993-04-08	TAKE BACK RETURN         	SHIP      	 doggedly according to the carefully ir
4150085	111424	6447	3	25	35885.50	0.07	0.03	A	F	1993-03-09	1993-03-26	1993-03-19	TAKE BACK RETURN         	AIR       	tions sublate blithely. blithely final ac
4150085	100025	7556	4	39	39975.78	0.09	0.02	R	F	1993-04-02	1993-04-06	1993-04-11	COLLECT COD              	SHIP      	ainments above the furiously unusual re
4161248	63852	1371	1	29	52659.65	0.09	0.00	A	F	1992-08-19	1992-09-27	1992-09-04	DELIVER IN PERSON        	MAIL      	 bold epitaphs! furi
4161248	152536	82	2	8	12708.24	0.02	0.00	A	F	1992-08-01	1992-10-03	1992-08-02	TAKE BACK RETURN         	FOB       	lyly among the unusual, final packag
4161248	63970	6477	3	10	19339.70	0.09	0.08	A	F	1992-11-05	1992-09-09	1992-11-21	TAKE BACK RETURN         	SHIP      	packages. unusual, pending ideas 
4161248	139916	2430	4	33	64545.03	0.02	0.02	R	F	1992-09-01	1992-09-04	1992-09-07	TAKE BACK RETURN         	MAIL      	r realms are along the care
4161248	154357	9388	5	19	26815.65	0.00	0.06	A	F	1992-11-02	1992-10-26	1992-11-24	NONE                     	REG AIR   	r the furiously ironic ideas boost slyly 
4192001	105612	5613	1	41	66322.01	0.04	0.01	N	O	1997-02-07	1997-02-26	1997-02-23	COLLECT COD              	SHIP      	the regular, express depen
4192001	16847	6848	2	50	88192.00	0.08	0.02	N	O	1997-04-14	1997-03-26	1997-05-12	TAKE BACK RETURN         	AIR       	nusual asymptotes. sly, r
4192001	133497	3498	3	34	52036.66	0.02	0.08	N	O	1997-03-16	1997-03-11	1997-04-13	COLLECT COD              	SHIP      	r deposits. carefully iron
4192001	72517	39	4	40	59580.40	0.00	0.03	N	O	1997-04-18	1997-02-26	1997-04-22	NONE                     	REG AIR   	above the carefully ironic
4192001	178726	6278	5	32	57751.04	0.09	0.08	N	O	1997-03-16	1997-03-02	1997-03-28	NONE                     	FOB       	ing sentiments. fl
4192001	46177	1186	6	1	1123.17	0.05	0.00	N	O	1997-03-29	1997-02-08	1997-03-31	NONE                     	REG AIR   	 unusual frets nod. ironic e
4243142	3062	563	1	37	35707.22	0.05	0.04	N	O	1996-02-05	1996-02-17	1996-02-13	NONE                     	FOB       	d dependencies snooze fluffily sly
4243142	48863	8864	2	20	36237.20	0.02	0.05	N	O	1996-01-09	1995-12-29	1996-01-23	TAKE BACK RETURN         	MAIL      	 slyly. furiously express id
4243142	33399	3400	3	44	58625.16	0.05	0.07	N	O	1996-03-19	1996-02-02	1996-04-17	DELIVER IN PERSON        	AIR       	ng the quickly fi
4243392	94746	9765	1	41	71370.34	0.01	0.02	N	O	1996-04-13	1996-05-05	1996-04-29	DELIVER IN PERSON        	REG AIR   	nts snooze
4243392	179451	4486	2	28	42852.60	0.02	0.07	N	O	1996-06-17	1996-05-17	1996-07-11	DELIVER IN PERSON        	MAIL      	ily special instructions. slyly ex
4314274	144213	6728	1	49	61603.29	0.04	0.02	N	O	1997-05-18	1997-05-01	1997-06-10	COLLECT COD              	FOB       	ly ironic accounts. ironic ideas hag
4314274	182081	9636	2	6	6978.48	0.06	0.06	N	O	1997-04-21	1997-05-07	1997-05-05	COLLECT COD              	MAIL      	ieve among the carefull
4314274	32382	9892	3	38	49946.44	0.05	0.05	N	O	1997-06-28	1997-05-25	1997-07-04	COLLECT COD              	SHIP      	platelets wake furiously. s
4314274	27635	7636	4	25	39065.75	0.00	0.08	N	O	1997-07-21	1997-06-01	1997-08-02	COLLECT COD              	SHIP      	 ideas about the 
4314274	56263	6264	5	17	20727.42	0.09	0.01	N	O	1997-05-07	1997-05-22	1997-05-18	NONE                     	REG AIR   	riously fluffy deposits. unusual, unusua
4314274	3786	3787	6	42	70970.76	0.04	0.01	N	O	1997-06-17	1997-04-23	1997-07-09	COLLECT COD              	RAIL      	es are bold excuses. speci
4320612	141370	6399	1	24	33872.88	0.06	0.00	A	F	1992-12-03	1992-11-22	1992-12-14	TAKE BACK RETURN         	MAIL      	uickly bold theodolites
4320612	193219	3220	2	17	22307.57	0.01	0.02	A	F	1992-11-23	1992-10-21	1992-11-28	DELIVER IN PERSON        	SHIP      	 regular asymptotes! id
4320612	19301	1803	3	48	58574.40	0.07	0.01	A	F	1992-10-22	1992-10-22	1992-11-13	NONE                     	SHIP      	 boost carefully
4320612	176957	4509	4	37	75256.15	0.06	0.04	A	F	1992-12-24	1992-12-13	1993-01-14	DELIVER IN PERSON        	MAIL      	counts. furiously pending pinto beans doub
4324037	73523	8538	1	16	23944.32	0.01	0.07	N	O	1998-01-02	1998-01-16	1998-01-23	NONE                     	SHIP      	ross the final, bo
4327493	129861	7398	1	14	26472.04	0.10	0.04	A	F	1994-08-01	1994-08-20	1994-08-23	TAKE BACK RETURN         	FOB       	he ironic deposits sleep bli
4327493	164692	2241	2	25	43917.25	0.05	0.08	A	F	1994-09-29	1994-08-06	1994-10-17	TAKE BACK RETURN         	RAIL      	ven accounts grow under the deposits. ev
4327493	168557	6106	3	21	34136.55	0.03	0.01	A	F	1994-07-19	1994-07-29	1994-07-26	TAKE BACK RETURN         	AIR       	ong the even asymptotes. fluffi
4337954	27044	7045	1	7	6797.28	0.08	0.06	N	O	1995-07-04	1995-09-06	1995-07-10	COLLECT COD              	REG AIR   	ily special escapades. expre
4342725	2127	7128	1	10	10291.20	0.01	0.00	N	O	1996-05-08	1996-06-23	1996-06-05	DELIVER IN PERSON        	RAIL      	y. express, f
4342725	138920	3947	2	42	82274.64	0.01	0.03	N	O	1996-07-31	1996-07-21	1996-08-05	COLLECT COD              	FOB       	 slyly. special, bold dolphi
4342725	21112	6117	3	16	16529.76	0.02	0.04	N	O	1996-06-30	1996-06-26	1996-07-22	COLLECT COD              	AIR       	jole blithely specia
4342725	13671	6173	4	24	38032.08	0.08	0.07	N	O	1996-06-25	1996-07-07	1996-07-10	DELIVER IN PERSON        	SHIP      	dle. unusua
4363712	5829	3330	1	50	86741.00	0.07	0.00	R	F	1993-11-10	1993-12-13	1993-11-12	COLLECT COD              	AIR       	g furiously sly
4363712	162843	7876	2	38	72421.92	0.04	0.07	R	F	1993-10-22	1993-12-05	1993-10-25	TAKE BACK RETURN         	FOB       	counts. foxes wake quickly unusual pa
4363712	93250	5760	3	1	1243.25	0.10	0.02	A	F	1993-10-22	1993-12-11	1993-10-28	DELIVER IN PERSON        	MAIL      	 deposits wake
4363712	45717	726	4	12	19952.52	0.09	0.06	A	F	1993-11-23	1993-11-03	1993-12-16	DELIVER IN PERSON        	TRUCK     	lyly. express, careful 
4363712	158509	1025	5	9	14107.50	0.10	0.04	R	F	1993-11-18	1993-12-02	1993-12-10	NONE                     	SHIP      	deas. brav
4363712	57782	7783	6	2	3479.56	0.03	0.00	R	F	1993-12-22	1993-10-26	1993-12-27	NONE                     	REG AIR   	the slyly even instructions. even p
4364800	119984	7518	1	45	90179.10	0.03	0.03	R	F	1992-09-03	1992-09-08	1992-09-12	NONE                     	REG AIR   	osits wake against the furiou
4364800	158090	606	2	38	43627.42	0.03	0.05	A	F	1992-09-03	1992-10-21	1992-09-14	COLLECT COD              	MAIL      	 ideas. furiously regular ideas wake. t
4405509	137091	7092	1	6	6768.54	0.04	0.08	N	O	1996-05-07	1996-07-21	1996-05-13	TAKE BACK RETURN         	SHIP      	 around the final, unusual deposits.
4405509	78703	8704	2	42	70631.40	0.03	0.00	N	O	1996-05-22	1996-07-21	1996-05-28	DELIVER IN PERSON        	FOB       	sleep carefully against the 
4405509	147266	9781	3	6	7879.56	0.02	0.08	N	O	1996-05-20	1996-06-03	1996-06-08	TAKE BACK RETURN         	REG AIR   	g the ironic dependencies. 
4405509	97301	9811	4	37	48037.10	0.03	0.05	N	O	1996-07-11	1996-06-09	1996-08-10	TAKE BACK RETURN         	MAIL      	 the dogge
4405509	126510	4047	5	11	16901.61	0.00	0.07	N	O	1996-08-14	1996-06-21	1996-08-17	DELIVER IN PERSON        	RAIL      	ckages serve caref
4405509	39635	7145	6	6	9447.78	0.05	0.07	N	O	1996-07-05	1996-07-08	1996-07-10	TAKE BACK RETURN         	RAIL      	tes haggle blithely furiously p
4405509	137177	7178	7	27	32782.59	0.03	0.06	N	O	1996-05-05	1996-06-17	1996-06-04	NONE                     	AIR       	ly ironic accounts nag stealth
4407621	34109	6613	1	5	5215.50	0.01	0.03	N	O	1995-11-30	1995-11-20	1995-12-27	TAKE BACK RETURN         	RAIL      	cross the slyly silent accounts.
4407621	55841	852	2	10	17968.40	0.10	0.07	N	O	1995-12-15	1995-11-04	1996-01-02	TAKE BACK RETURN         	FOB       	 dependencies cajole
4407621	106161	3692	3	17	19841.72	0.03	0.04	N	O	1996-01-07	1995-11-24	1996-02-05	NONE                     	AIR       	ilent packages. bold requests cajole 
4407621	38424	8425	4	40	54496.80	0.09	0.03	N	O	1995-12-26	1995-12-17	1996-01-07	NONE                     	AIR       	elets! fluffily ironic packages cajole 
4409734	38295	5805	1	25	30832.25	0.09	0.05	A	F	1995-05-13	1995-03-27	1995-05-20	DELIVER IN PERSON        	RAIL      	even asymptotes. bold pla
4409734	23303	810	2	49	60088.70	0.06	0.02	R	F	1995-04-07	1995-04-04	1995-04-28	DELIVER IN PERSON        	REG AIR   	multipliers. furiously even somas wa
4409734	40537	5546	3	39	57623.67	0.09	0.06	A	F	1995-05-11	1995-04-22	1995-06-07	DELIVER IN PERSON        	AIR       	 sublate carefully quick
4428354	59051	4062	1	43	43432.15	0.02	0.05	N	O	1997-01-04	1997-01-05	1997-01-16	DELIVER IN PERSON        	AIR       	 cajole into the carefully final n
4428354	69284	6803	2	3	3759.84	0.00	0.08	N	O	1997-02-25	1997-01-25	1997-02-26	COLLECT COD              	RAIL      	g deposits wake furiously. quickly fin
4428354	43953	1466	3	22	41732.90	0.01	0.07	N	O	1996-12-08	1996-12-11	1996-12-18	DELIVER IN PERSON        	AIR       	 slyly final the
4428354	105703	5704	4	19	32465.30	0.08	0.08	N	O	1997-01-22	1997-01-25	1997-02-14	DELIVER IN PERSON        	REG AIR   	 fluffily ironic accounts 
4428354	145318	5319	5	40	54532.40	0.00	0.05	N	O	1996-11-15	1996-12-28	1996-11-21	COLLECT COD              	FOB       	gular pinto beans. blithely
4428354	147830	7831	6	49	92013.67	0.01	0.06	N	O	1996-11-23	1997-01-09	1996-11-29	COLLECT COD              	FOB       	 after the furiously pending dependencies
4516484	171390	3908	1	31	45303.09	0.04	0.07	N	O	1998-03-22	1998-05-29	1998-04-15	COLLECT COD              	RAIL      	entiments sleep carefully. even pa
4516484	149085	4114	2	47	53301.76	0.06	0.02	N	O	1998-05-27	1998-05-31	1998-06-25	COLLECT COD              	TRUCK     	 promise slyly.
4516484	58534	6050	3	19	28358.07	0.10	0.02	N	O	1998-03-17	1998-05-14	1998-04-01	COLLECT COD              	FOB       	l accounts. silent asymptotes cajole furiou
4516484	172042	4560	4	8	8912.32	0.00	0.02	N	O	1998-06-26	1998-04-11	1998-06-27	COLLECT COD              	REG AIR   	he furiously final ideas detect amo
4516484	93664	3665	5	25	41441.50	0.05	0.06	N	O	1998-03-23	1998-04-14	1998-03-26	NONE                     	REG AIR   	 requests detect. carefully fin
4516484	5765	8266	6	45	75184.20	0.08	0.04	N	O	1998-05-02	1998-05-16	1998-05-05	COLLECT COD              	MAIL      	ts. carefully regular accounts along t
4516484	86885	4410	7	25	46797.00	0.08	0.03	N	O	1998-03-23	1998-04-13	1998-04-21	COLLECT COD              	AIR       	uffily regular excu
4520518	49358	9359	1	36	47064.60	0.03	0.08	A	F	1995-03-19	1995-04-01	1995-04-09	DELIVER IN PERSON        	FOB       	uctions. regular foxes despite th
4525251	132891	431	1	2	3847.78	0.06	0.03	N	O	1996-03-20	1996-05-15	1996-03-26	COLLECT COD              	REG AIR   	nusual requests. special, even accounts ca
4525251	84589	9606	2	12	18882.96	0.05	0.03	N	O	1996-05-26	1996-04-26	1996-06-02	NONE                     	SHIP      	y ironic theod
4535970	453	5454	1	38	51431.10	0.05	0.05	R	F	1993-04-12	1993-04-19	1993-04-26	NONE                     	REG AIR   	ly regular deposits. regularly spec
4535970	8527	3528	2	28	40194.56	0.08	0.02	A	F	1993-05-08	1993-04-30	1993-05-18	NONE                     	RAIL      	cial accounts
4553955	122458	4971	1	17	25167.65	0.05	0.00	N	O	1996-07-20	1996-07-30	1996-07-26	DELIVER IN PERSON        	MAIL      	nal packages against th
4553955	135273	7787	2	5	6541.35	0.02	0.01	N	O	1996-06-11	1996-07-25	1996-06-22	COLLECT COD              	REG AIR   	 requests was blithely according to the d
4603875	71409	1410	1	32	44172.80	0.09	0.02	N	O	1998-06-24	1998-07-24	1998-07-07	TAKE BACK RETURN         	SHIP      	cuses unwind slyly alongside of the slyly
4603875	157859	5405	2	48	92008.80	0.10	0.03	N	O	1998-06-08	1998-07-22	1998-07-07	NONE                     	REG AIR   	even, express instru
4603875	132043	4557	3	9	9675.36	0.05	0.07	N	O	1998-07-05	1998-08-02	1998-07-08	TAKE BACK RETURN         	SHIP      	ular somas. furiously pending ideas along t
4603875	44379	6884	4	22	29114.14	0.00	0.08	N	O	1998-09-01	1998-08-21	1998-09-30	DELIVER IN PERSON        	SHIP      	 furiously silent foxes. even
4603875	63184	8197	5	15	17207.70	0.06	0.03	N	O	1998-07-21	1998-07-02	1998-08-20	NONE                     	FOB       	 packages maintain s
4603875	124860	4861	6	19	35812.34	0.01	0.08	N	O	1998-08-19	1998-08-24	1998-08-27	DELIVER IN PERSON        	FOB       	sts. carefully bold excuses try to
4603875	105559	8070	7	27	42242.85	0.06	0.08	N	O	1998-08-22	1998-08-07	1998-09-15	TAKE BACK RETURN         	SHIP      	ts are care
4613986	71255	3763	1	28	34335.00	0.00	0.08	N	O	1997-06-23	1997-08-22	1997-07-14	COLLECT COD              	SHIP      	luffily across the final p
4616224	72748	270	1	12	20648.88	0.03	0.08	N	O	1997-03-19	1997-03-10	1997-03-21	TAKE BACK RETURN         	SHIP      	ounts integrate fluffily 
4639746	95311	7821	1	39	50946.09	0.04	0.03	N	O	1995-10-29	1995-11-01	1995-11-14	COLLECT COD              	FOB       	 quickly pending packages do en
4639746	80029	30	2	28	28252.56	0.08	0.06	N	O	1995-10-17	1995-11-08	1995-11-01	COLLECT COD              	REG AIR   	 haggle slyly slyly ironic dolphins. regula
4639746	140762	3277	4	42	75715.92	0.02	0.08	N	O	1995-10-27	1995-10-29	1995-10-31	COLLECT COD              	FOB       	 carefully special requests. packa
4639746	176014	3566	5	18	19620.18	0.05	0.07	N	O	1995-10-27	1995-11-13	1995-11-03	DELIVER IN PERSON        	FOB       	usly bold instructions. noto
4718403	58369	875	1	24	31856.64	0.10	0.08	N	O	1997-02-05	1996-11-12	1997-02-13	DELIVER IN PERSON        	FOB       	 according to the fu
4718403	195628	667	2	13	22407.06	0.10	0.01	N	O	1996-11-16	1997-01-02	1996-11-24	COLLECT COD              	MAIL      	ly regular dependencies. slyly special r
4718403	105475	7986	3	17	25167.99	0.08	0.03	N	O	1996-11-27	1996-11-07	1996-12-16	NONE                     	TRUCK     	le carefully ironic instructions.
4718403	110990	8524	4	37	74036.63	0.02	0.06	N	O	1996-11-19	1996-12-28	1996-12-13	DELIVER IN PERSON        	SHIP      	inal foxes affix careful
4718403	2205	2206	5	2	2214.40	0.03	0.04	N	O	1997-01-12	1997-01-02	1997-01-17	TAKE BACK RETURN         	FOB       	ounts. express, final accounts b
4764293	162266	9815	1	37	49145.62	0.00	0.07	A	F	1992-06-12	1992-05-10	1992-07-12	DELIVER IN PERSON        	TRUCK     	ly final deposits solve. even deposits 
4776288	122144	2145	1	5	5830.70	0.05	0.04	N	O	1996-04-29	1996-05-08	1996-05-28	COLLECT COD              	TRUCK     	ns kindle blithely even deposits-- flu
4776288	67815	7816	2	15	26742.15	0.04	0.02	N	O	1996-05-06	1996-04-28	1996-05-18	COLLECT COD              	SHIP      	 deposits cajole carefully. in
4776288	150224	7770	3	6	7645.32	0.09	0.03	N	O	1996-03-26	1996-05-09	1996-04-22	DELIVER IN PERSON        	TRUCK     	inst the carefully ironic pinto beans
4776288	2705	2706	4	10	16077.00	0.09	0.05	N	O	1996-03-09	1996-03-28	1996-03-19	DELIVER IN PERSON        	TRUCK     	gular instructions along the foxe
4794213	196327	1366	1	37	52662.84	0.06	0.06	A	F	1995-02-08	1995-04-08	1995-02-14	TAKE BACK RETURN         	REG AIR   	cial deposits. slyly final
4794213	66365	1378	2	6	7988.16	0.10	0.01	R	F	1995-02-13	1995-02-12	1995-02-27	NONE                     	RAIL      	ns sleep final dolphins. slyly eve
4794213	191523	6562	3	21	33904.92	0.04	0.02	A	F	1995-05-11	1995-03-06	1995-05-28	NONE                     	REG AIR   	oxes among the in
4794213	116727	9239	4	12	20924.64	0.09	0.01	A	F	1995-01-31	1995-03-13	1995-02-13	DELIVER IN PERSON        	REG AIR   	ven requests haggle caref
4794213	15371	374	5	41	52741.17	0.08	0.01	R	F	1995-02-15	1995-02-10	1995-03-06	DELIVER IN PERSON        	SHIP      	iously regular depe
4794213	135929	956	6	11	21614.12	0.00	0.07	R	F	1995-02-16	1995-02-25	1995-03-08	DELIVER IN PERSON        	TRUCK     	ic platelets boost slyly ironic 
4829253	8036	537	1	38	35873.14	0.06	0.06	N	O	1998-03-15	1998-03-12	1998-04-12	TAKE BACK RETURN         	REG AIR   	s instructions are express f
4829253	50773	3279	2	50	86188.50	0.01	0.03	N	O	1998-01-29	1998-02-24	1998-02-15	NONE                     	REG AIR   	oss the bli
4829253	140905	5934	3	18	35026.20	0.01	0.08	N	O	1998-04-26	1998-04-02	1998-04-29	DELIVER IN PERSON        	RAIL      	furiously across the furiously u
4839942	130647	648	1	8	13421.12	0.07	0.07	A	F	1994-01-11	1994-02-20	1994-02-10	TAKE BACK RETURN         	MAIL      	r ideas sleep slyly. slyly bo
4839942	169228	6777	2	42	54483.24	0.09	0.05	R	F	1994-01-20	1994-02-26	1994-01-25	DELIVER IN PERSON        	FOB       	 quickly regular dep
4839942	4260	9261	3	45	52391.70	0.09	0.01	A	F	1994-02-21	1994-03-12	1994-03-01	COLLECT COD              	RAIL      	equests. blithely regular pinto beans nag 
4839942	192325	2326	4	3	4251.96	0.08	0.04	A	F	1994-03-14	1994-03-16	1994-04-04	COLLECT COD              	AIR       	ly about the final, regular ac
4839942	3295	5796	5	8	9586.32	0.07	0.00	A	F	1994-02-10	1994-03-12	1994-02-18	TAKE BACK RETURN         	REG AIR   	 special realms haggle
4862951	147379	7380	1	29	41364.73	0.09	0.04	A	F	1995-01-31	1995-03-19	1995-03-01	NONE                     	FOB       	ecial instruc
4862951	186727	1764	2	8	14509.76	0.05	0.01	A	F	1995-04-07	1995-04-04	1995-04-11	COLLECT COD              	SHIP      	hely express 
4862951	116248	6249	3	20	25284.80	0.08	0.07	R	F	1995-03-31	1995-04-07	1995-04-27	COLLECT COD              	RAIL      	 frets cajole b
4862951	165309	342	4	50	68715.00	0.10	0.07	A	F	1995-03-30	1995-03-27	1995-04-20	TAKE BACK RETURN         	TRUCK     	, regular depend
4862951	134725	4726	5	32	56311.04	0.00	0.05	A	F	1995-04-21	1995-04-23	1995-05-03	COLLECT COD              	FOB       	old accounts. even, unusual pa
4872388	96825	4353	1	32	58298.24	0.00	0.05	A	F	1993-12-14	1993-11-27	1993-12-16	COLLECT COD              	AIR       	requests wake blithely about the iron
4872388	132329	4843	2	50	68066.00	0.01	0.07	R	F	1994-01-24	1993-11-16	1994-01-28	NONE                     	FOB       	ajole slyly along the furiously ex
4872388	44546	2059	3	9	13414.86	0.03	0.06	A	F	1994-01-12	1993-11-15	1994-02-05	COLLECT COD              	TRUCK     	se above the exp
4872388	76784	1799	4	45	79235.10	0.10	0.07	R	F	1993-10-18	1993-12-03	1993-11-03	COLLECT COD              	TRUCK     	o the quickly ironic d
4881061	196413	8933	1	26	39244.66	0.06	0.03	N	O	1997-03-15	1997-03-28	1997-04-03	COLLECT COD              	RAIL      	ecial dolphins cajole pending pa
4917250	34764	4765	1	17	28878.92	0.04	0.00	R	F	1994-01-02	1993-11-04	1994-01-04	COLLECT COD              	AIR       	n deposits. theodolites must wake furiousl
4932935	151633	4149	1	14	23584.82	0.06	0.00	R	F	1995-03-14	1995-02-06	1995-04-05	NONE                     	REG AIR   	sly ruthles
4932935	80120	121	2	5	5500.60	0.04	0.04	A	F	1995-04-27	1995-04-01	1995-05-12	TAKE BACK RETURN         	MAIL      	ss asymptotes. carefully blithe
4932935	176670	6671	3	40	69866.80	0.05	0.07	R	F	1995-01-09	1995-03-14	1995-01-21	DELIVER IN PERSON        	MAIL      	according to the furiously blithe de
4932935	82667	7684	4	45	74234.70	0.01	0.03	A	F	1995-03-27	1995-02-27	1995-04-17	NONE                     	REG AIR   	even instructions cajol
4942119	35325	7829	1	25	31508.00	0.07	0.00	R	F	1992-11-18	1993-01-02	1992-11-30	DELIVER IN PERSON        	REG AIR   	inal accounts cajole
4942119	98981	4000	2	2	3959.96	0.08	0.04	A	F	1992-11-11	1992-11-23	1992-11-24	TAKE BACK RETURN         	FOB       	lithely iro
4942119	163195	3196	3	42	52843.98	0.01	0.07	R	F	1993-02-05	1992-12-03	1993-02-19	DELIVER IN PERSON        	REG AIR   	n the slyly unus
4942119	15329	7831	4	11	13687.52	0.01	0.00	R	F	1992-12-18	1992-12-15	1993-01-08	COLLECT COD              	AIR       	theodolites. furiously regular i
4942119	27555	58	5	27	40028.85	0.03	0.02	A	F	1992-11-19	1993-01-11	1992-12-19	DELIVER IN PERSON        	RAIL      	 boost furiously. carefully b
4950721	182225	7262	1	44	57517.68	0.06	0.05	A	F	1995-06-01	1995-06-01	1995-06-15	DELIVER IN PERSON        	FOB       	 the carefully ironi
4950721	43039	552	2	24	23568.72	0.10	0.00	A	F	1995-04-30	1995-04-23	1995-05-05	DELIVER IN PERSON        	AIR       	lyly regular platelets b
4950721	155141	7657	3	35	41864.90	0.08	0.04	A	F	1995-05-12	1995-04-29	1995-06-10	DELIVER IN PERSON        	FOB       	nic ideas poach fluffily after the even, 
4950721	43032	8041	4	17	16575.51	0.06	0.04	R	F	1995-03-28	1995-05-22	1995-04-23	TAKE BACK RETURN         	REG AIR   	regular foxes wake evenly. blithely
4950721	24930	2437	5	10	18549.30	0.01	0.04	N	O	1995-06-23	1995-05-23	1995-07-21	TAKE BACK RETURN         	FOB       	ilent requests. special pinto beans are sly
4950721	196235	3793	6	16	21299.68	0.09	0.08	R	F	1995-05-14	1995-05-20	1995-06-04	COLLECT COD              	SHIP      	even requests haggle closely slyly unu
4950721	116943	6944	7	41	80357.54	0.06	0.07	R	F	1995-05-21	1995-05-01	1995-05-23	COLLECT COD              	SHIP      	r requests are despite the quickly bold 
4960614	5975	976	1	10	18809.70	0.01	0.04	N	O	1997-12-24	1997-11-08	1997-12-29	COLLECT COD              	SHIP      	kages. finally ironic 
4960614	14062	9065	2	29	28305.74	0.03	0.03	N	O	1997-10-20	1997-11-11	1997-11-11	TAKE BACK RETURN         	SHIP      	osits at the blithely iron
4960614	128497	3522	3	21	32035.29	0.07	0.02	N	O	1997-12-15	1997-11-25	1997-12-27	NONE                     	RAIL      	luffily pending accou
4960614	32629	139	4	8	12492.96	0.00	0.05	N	O	1998-01-19	1997-12-13	1998-02-10	NONE                     	TRUCK     	ggle blithel
4966627	39549	9550	1	21	31259.34	0.03	0.05	R	F	1993-05-25	1993-04-03	1993-06-20	COLLECT COD              	SHIP      	inal requests. bold accounts de
4968129	33566	8573	1	34	50985.04	0.06	0.01	A	F	1994-11-08	1994-09-23	1994-12-04	TAKE BACK RETURN         	SHIP      	unts engage according to the quickly b
4968129	136687	9201	2	50	86184.00	0.03	0.00	A	F	1994-08-18	1994-08-19	1994-08-21	COLLECT COD              	SHIP      	sits print above the fluf
4968294	122949	486	1	21	41410.74	0.10	0.01	A	F	1993-03-04	1993-03-10	1993-03-16	COLLECT COD              	FOB       	e of the multipliers sleep furiously c
4968294	187202	9721	2	5	6446.00	0.02	0.02	A	F	1993-03-29	1993-04-10	1993-04-06	DELIVER IN PERSON        	RAIL      	 pending platelets. quickly special packag
4968294	98529	3548	3	39	59573.28	0.09	0.02	R	F	1993-02-10	1993-02-24	1993-02-21	DELIVER IN PERSON        	AIR       	e bold theodolit
4968294	170345	7897	4	38	53782.92	0.01	0.04	A	F	1993-01-29	1993-02-18	1993-02-02	DELIVER IN PERSON        	RAIL      	counts. caref
4968519	10822	823	1	38	65847.16	0.01	0.05	N	O	1998-06-23	1998-09-03	1998-07-23	NONE                     	MAIL      	quests according to the regu
4970980	162304	4821	1	3	4098.90	0.00	0.06	R	F	1993-03-22	1993-05-16	1993-04-07	NONE                     	FOB       	riously express requests wake slyly care
4970980	30989	990	2	17	32639.66	0.09	0.02	A	F	1993-03-29	1993-05-23	1993-04-05	COLLECT COD              	MAIL      	rmanently unusual
4970980	25605	610	3	36	55101.60	0.02	0.07	A	F	1993-05-27	1993-04-24	1993-05-31	COLLECT COD              	SHIP      	 platelets across the bravely
4970980	128586	6123	4	11	17760.38	0.02	0.04	A	F	1993-04-25	1993-03-30	1993-04-30	TAKE BACK RETURN         	TRUCK     	f the express, pend
4970980	140656	3171	5	39	66169.35	0.05	0.02	R	F	1993-06-19	1993-04-08	1993-06-30	TAKE BACK RETURN         	AIR       	es cajole qui
4970980	191151	1152	6	24	29811.60	0.06	0.04	R	F	1993-05-05	1993-04-24	1993-06-04	DELIVER IN PERSON        	REG AIR   	coys cajole car
4970980	70288	2796	7	46	57880.88	0.10	0.07	A	F	1993-05-14	1993-03-31	1993-06-02	TAKE BACK RETURN         	AIR       	 of the furiously ex
4972039	23604	8609	1	29	44300.40	0.04	0.06	N	O	1998-03-31	1998-06-19	1998-04-25	TAKE BACK RETURN         	REG AIR   	ve the final, final Tiresias.
4972039	106768	1789	2	28	49693.28	0.01	0.07	N	O	1998-04-15	1998-06-13	1998-04-28	COLLECT COD              	AIR       	 to the slyly 
4972039	48770	1275	3	39	67032.03	0.01	0.03	N	O	1998-06-14	1998-05-28	1998-06-27	COLLECT COD              	TRUCK     	iously above the slyly reg
5003109	20889	3392	1	35	63345.80	0.01	0.04	A	F	1994-09-15	1994-09-26	1994-10-14	DELIVER IN PERSON        	MAIL      	 pinto beans use regular foxes. blit
5003109	79203	1711	2	31	36648.20	0.04	0.00	A	F	1994-07-12	1994-08-06	1994-07-21	COLLECT COD              	MAIL      	usual escapades. idly express 
5003109	158804	8805	3	45	83826.00	0.01	0.06	A	F	1994-07-15	1994-09-18	1994-08-08	DELIVER IN PERSON        	FOB       	ckages should have to wake ca
5003109	8018	3019	4	21	19446.21	0.05	0.05	A	F	1994-09-16	1994-10-03	1994-10-05	TAKE BACK RETURN         	TRUCK     	lithely. idly regular di
5003109	151919	1920	5	40	78836.40	0.01	0.06	A	F	1994-10-23	1994-08-26	1994-11-20	DELIVER IN PERSON        	REG AIR   	along the furiously ironic asymptotes
5003109	91542	9070	6	20	30670.80	0.02	0.06	A	F	1994-08-02	1994-09-26	1994-08-05	COLLECT COD              	SHIP      	sly regular platelets hang. blithely
5052385	68256	8257	1	47	57539.75	0.04	0.04	R	F	1992-09-18	1992-09-06	1992-10-09	DELIVER IN PERSON        	REG AIR   	ual foxes wake carefull
5052385	175337	2889	2	17	24009.61	0.08	0.03	R	F	1992-08-04	1992-09-22	1992-08-13	TAKE BACK RETURN         	MAIL      	kly final pearls nag against t
5052385	74758	7266	3	2	3465.50	0.00	0.04	R	F	1992-11-05	1992-09-22	1992-11-24	DELIVER IN PERSON        	RAIL      	deposits use according to the quickly f
5052385	43596	3597	4	10	15395.90	0.05	0.04	A	F	1992-09-06	1992-09-02	1992-09-26	COLLECT COD              	MAIL      	. quickly express account
5052385	117576	7577	5	14	22309.98	0.02	0.05	A	F	1992-11-14	1992-10-22	1992-12-13	DELIVER IN PERSON        	SHIP      	ss the fluffily even theodolites ha
5052385	89778	4795	6	2	3535.54	0.03	0.01	R	F	1992-09-02	1992-09-26	1992-09-12	TAKE BACK RETURN         	FOB       	he requests. quietly ironic pinto beans wa
5093856	115302	2836	1	40	52692.00	0.07	0.02	A	F	1992-06-29	1992-08-02	1992-07-07	DELIVER IN PERSON        	RAIL      	fter the asymptotes haggle 
5093856	73536	8551	2	3	4528.59	0.00	0.05	R	F	1992-08-04	1992-07-03	1992-08-11	TAKE BACK RETURN         	REG AIR   	pecial pinto beans haggle even, even pin
5093856	105006	27	3	4	4044.00	0.03	0.05	A	F	1992-09-04	1992-08-13	1992-09-05	DELIVER IN PERSON        	TRUCK     	eposits. slyly ironic foxes are slyl
5096387	61278	1279	1	39	48331.53	0.00	0.06	N	O	1998-05-12	1998-05-20	1998-06-01	COLLECT COD              	MAIL      	-- quickly even packages sleep at 
5096387	94352	6862	2	35	47122.25	0.05	0.06	N	O	1998-07-04	1998-05-21	1998-07-22	NONE                     	REG AIR   	instructio
5096387	171215	3733	3	8	10289.68	0.05	0.00	N	O	1998-05-24	1998-06-06	1998-06-04	DELIVER IN PERSON        	SHIP      	fully quick pinto beans. blithely bold
5096387	5772	5773	4	45	75499.65	0.04	0.03	N	O	1998-04-15	1998-05-03	1998-05-01	DELIVER IN PERSON        	TRUCK     	use slyly final accounts. carefully 
5173218	159916	4947	1	5	9879.55	0.01	0.06	N	O	1996-03-27	1996-03-14	1996-04-20	TAKE BACK RETURN         	TRUCK     	ly even somas are. ironic dolphins alo
5173218	107268	4799	2	48	61212.48	0.00	0.07	N	O	1996-05-22	1996-02-24	1996-06-16	TAKE BACK RETURN         	AIR       	nt dependencies. carefully ironic soma
5173218	43308	8317	3	30	37539.00	0.03	0.00	N	O	1996-05-06	1996-04-09	1996-05-28	TAKE BACK RETURN         	RAIL      	 furiously. ironic re
5173218	77103	2118	4	24	25922.40	0.00	0.07	N	O	1996-04-26	1996-03-20	1996-05-09	NONE                     	TRUCK     	refully ironic packages. slyly bold
5200901	88477	6002	1	42	61549.74	0.00	0.07	N	O	1996-02-24	1996-01-14	1996-03-10	DELIVER IN PERSON        	FOB       	according to the even ideas. blithely 
5200901	22518	2519	2	29	41774.79	0.03	0.05	N	O	1995-12-24	1995-12-24	1996-01-03	COLLECT COD              	AIR       	usly ironic packages. slyly final as
5200901	45370	379	3	37	48668.69	0.10	0.02	N	O	1996-01-19	1996-01-17	1996-01-31	TAKE BACK RETURN         	AIR       	nic requests cajole blithely
5207685	98967	3986	1	24	47183.04	0.08	0.00	R	F	1994-07-15	1994-08-20	1994-08-02	COLLECT COD              	SHIP      	s. furious
5207685	128061	574	2	33	35938.98	0.04	0.06	A	F	1994-06-20	1994-07-12	1994-07-10	COLLECT COD              	FOB       	refully. slyly final
5207685	120330	5355	3	21	28356.93	0.09	0.06	R	F	1994-07-30	1994-08-02	1994-08-01	COLLECT COD              	RAIL      	ccounts detect blithely al
5207685	172376	9928	4	44	63728.28	0.05	0.04	A	F	1994-08-09	1994-07-31	1994-09-08	DELIVER IN PERSON        	TRUCK     	eodolites cajole slyly. fu
5207685	116569	9081	5	6	9513.36	0.08	0.03	R	F	1994-07-04	1994-08-21	1994-07-05	TAKE BACK RETURN         	RAIL      	 to the carefully regular accounts are qu
5207685	171498	9050	6	2	3138.98	0.02	0.06	A	F	1994-06-18	1994-07-13	1994-07-13	NONE                     	FOB       	nic instructions would use carefull
5228454	188191	3228	1	39	49888.41	0.05	0.08	N	O	1997-12-16	1997-12-14	1997-12-25	TAKE BACK RETURN         	AIR       	ackages cajole careful
5235045	162582	131	1	48	78939.84	0.10	0.04	R	F	1992-12-22	1992-12-22	1993-01-18	NONE                     	TRUCK     	its haggle furiously blithely ironic depe
5235045	107024	2045	2	35	36085.70	0.06	0.00	R	F	1993-02-03	1993-02-13	1993-02-23	COLLECT COD              	AIR       	 use carefully along t
5235045	144655	2198	3	9	15296.85	0.05	0.00	A	F	1993-03-15	1993-01-10	1993-04-12	TAKE BACK RETURN         	REG AIR   	lar accounts. blithely enticing ideas hag
5235045	528	8029	4	19	27141.88	0.01	0.03	R	F	1992-11-28	1992-12-24	1992-12-21	COLLECT COD              	FOB       	 deposits. quickly regular instructions s
5237730	86095	1112	1	16	17297.44	0.01	0.04	A	F	1994-10-24	1994-09-21	1994-11-06	NONE                     	TRUCK     	 asymptotes nag
5237730	191996	9554	2	48	100223.52	0.01	0.01	A	F	1994-12-14	1994-11-10	1995-01-12	NONE                     	MAIL      	telets haggle fluffily orb
5237730	67670	7671	3	49	80245.83	0.05	0.03	A	F	1994-09-29	1994-10-20	1994-10-13	DELIVER IN PERSON        	RAIL      	ake. ironic, sil
5241223	176212	8730	1	43	55393.03	0.04	0.00	R	F	1994-04-04	1994-05-20	1994-04-29	DELIVER IN PERSON        	MAIL      	equests across the ev
5241223	179580	7132	2	8	13276.64	0.03	0.00	R	F	1994-06-28	1994-06-20	1994-07-27	COLLECT COD              	AIR       	ld deposits boost slyly. bol
5241223	199751	2271	3	36	66627.00	0.09	0.08	R	F	1994-05-10	1994-06-14	1994-05-19	TAKE BACK RETURN         	FOB       	ackages alongside of the even, fin
5262498	3076	8077	1	19	18602.33	0.08	0.02	N	O	1997-04-02	1997-03-12	1997-04-14	NONE                     	SHIP      	furiously sly accoun
5262498	88729	1238	2	33	56684.76	0.08	0.07	N	O	1997-04-16	1997-03-06	1997-04-18	TAKE BACK RETURN         	SHIP      	n asymptotes haggle platelets. blithe
5262498	56145	8651	3	5	5505.70	0.02	0.00	N	O	1997-02-26	1997-02-08	1997-03-15	DELIVER IN PERSON        	TRUCK     	e. blithely pending d
5287012	78160	5682	1	30	34144.80	0.09	0.01	N	O	1997-10-05	1997-09-24	1997-10-28	NONE                     	MAIL      	nic accounts. final, even accounts hag
5287012	24202	1709	2	23	25902.60	0.02	0.00	N	O	1997-09-10	1997-11-09	1997-09-27	NONE                     	TRUCK     	carefully alongside of the idle packages. 
5287012	160112	5145	3	15	17581.65	0.03	0.02	N	O	1997-08-14	1997-11-04	1997-09-04	DELIVER IN PERSON        	AIR       	e carefully ironic req
5366593	94813	9832	1	33	59657.73	0.06	0.03	R	F	1993-08-06	1993-08-05	1993-08-09	NONE                     	TRUCK     	ongside of the blithely 
5378496	132796	7823	1	30	54863.70	0.05	0.04	R	F	1994-03-19	1994-03-21	1994-04-17	NONE                     	REG AIR   	uickly regular foxes. slyly regular pinto b
5378496	102124	7145	2	25	28153.00	0.03	0.03	A	F	1994-05-21	1994-04-14	1994-06-03	NONE                     	REG AIR   	he blithely
5381317	36257	8761	1	31	36990.75	0.10	0.03	N	O	1996-10-29	1996-08-19	1996-11-18	TAKE BACK RETURN         	RAIL      	jole carefully. regular, even dependencie
5406599	39266	9267	1	8	9642.08	0.06	0.03	A	F	1994-11-01	1994-10-08	1994-11-30	NONE                     	SHIP      	e carefully above the furiously final f
5406599	141048	8591	2	32	34849.28	0.06	0.07	R	F	1994-09-21	1994-09-24	1994-09-30	COLLECT COD              	AIR       	 the carefully regula
5406599	182682	237	3	18	31764.24	0.01	0.03	A	F	1994-09-21	1994-09-13	1994-10-15	TAKE BACK RETURN         	TRUCK     	de of the carefully special deposits mainta
5406599	131449	6476	4	32	47374.08	0.06	0.08	A	F	1994-08-05	1994-09-15	1994-08-24	COLLECT COD              	REG AIR   	sly along th
5414466	88555	8556	1	44	67916.20	0.02	0.01	A	F	1992-09-01	1992-08-18	1992-09-28	NONE                     	MAIL      	ts haggle quickly s
5414466	136343	8857	2	43	59311.62	0.05	0.03	A	F	1992-06-01	1992-07-15	1992-06-26	DELIVER IN PERSON        	MAIL      	fully final asymptotes afte
5414466	104326	4327	3	20	26606.40	0.10	0.00	R	F	1992-09-04	1992-07-30	1992-09-22	TAKE BACK RETURN         	MAIL      	dependencies are carefully against the care
5418919	50418	5429	1	47	64315.27	0.05	0.00	N	O	1996-12-11	1997-01-17	1996-12-16	TAKE BACK RETURN         	TRUCK     	 fluffily busy account
5418919	169655	2172	2	50	86232.50	0.01	0.03	N	O	1997-02-23	1997-01-12	1997-03-01	DELIVER IN PERSON        	TRUCK     	 unusual foxes wake about the furious
5425221	179047	1565	1	15	16890.60	0.05	0.02	R	F	1994-09-22	1994-09-21	1994-09-28	NONE                     	REG AIR   	ual deposits ha
5453440	14306	4307	1	14	17084.20	0.00	0.05	N	O	1997-09-03	1997-09-28	1997-09-08	NONE                     	TRUCK     	uests. finally bold requests are always. s
5476355	76412	6413	1	14	19437.74	0.00	0.02	A	F	1995-05-07	1995-06-21	1995-05-09	TAKE BACK RETURN         	SHIP      	the carefull
5476355	56140	3656	2	43	47134.02	0.08	0.06	N	F	1995-06-12	1995-06-16	1995-06-19	TAKE BACK RETURN         	REG AIR   	gular, regular excuses among the ironic r
5476355	134379	1919	3	35	49467.95	0.02	0.03	R	F	1995-05-12	1995-06-11	1995-05-22	NONE                     	RAIL      	ffily close deposits. deposi
5476355	105326	2857	4	15	19969.80	0.04	0.03	N	O	1995-06-23	1995-07-09	1995-06-27	NONE                     	TRUCK     	 sleep. depen
5476355	198638	3677	5	46	79884.98	0.03	0.05	N	O	1995-06-25	1995-06-19	1995-06-26	TAKE BACK RETURN         	TRUCK     	ously regular plate
5476355	98987	1497	6	20	39719.60	0.10	0.08	N	O	1995-07-25	1995-05-13	1995-08-03	COLLECT COD              	TRUCK     	ges. quickly i
5476355	74292	1814	7	24	30390.96	0.05	0.07	N	O	1995-07-04	1995-06-09	1995-07-15	DELIVER IN PERSON        	FOB       	dogged instructions sleep b
5496967	104800	9821	1	29	52339.20	0.05	0.04	R	F	1992-08-16	1992-10-12	1992-08-27	TAKE BACK RETURN         	RAIL      	ironically express ac
5496967	160678	3195	2	29	50421.43	0.10	0.07	R	F	1992-11-10	1992-09-22	1992-11-25	DELIVER IN PERSON        	AIR       	dolites use furiously into the slyl
5496967	99268	6796	3	10	12672.60	0.05	0.02	A	F	1992-09-17	1992-09-17	1992-10-07	DELIVER IN PERSON        	FOB       	g furiously against the furiously final 
5496967	190661	8219	4	32	56053.12	0.00	0.05	R	F	1992-09-24	1992-09-27	1992-10-12	DELIVER IN PERSON        	AIR       	e. furiously en
5534435	37107	4617	1	46	48028.60	0.03	0.05	N	O	1997-04-01	1997-05-30	1997-04-16	NONE                     	TRUCK     	fluffily final accounts cajole carefull
5534435	40877	878	2	49	89075.63	0.03	0.01	N	O	1997-06-22	1997-06-08	1997-06-29	TAKE BACK RETURN         	SHIP      	 blithely final deposits. regular ideas
5535201	169604	7153	1	33	55228.80	0.00	0.07	N	O	1998-06-15	1998-04-17	1998-06-28	COLLECT COD              	MAIL      	lly. ironic, 
5551104	120220	7757	1	16	19843.52	0.00	0.02	N	O	1996-11-16	1997-01-27	1996-12-13	NONE                     	RAIL      	ounts boost alongside of the u
5566567	35766	3276	1	35	59561.60	0.10	0.05	A	F	1993-12-19	1994-01-16	1993-12-21	TAKE BACK RETURN         	RAIL      	ending, bold requests
5569570	94321	4322	1	14	18414.48	0.08	0.06	R	F	1994-07-20	1994-07-17	1994-08-08	COLLECT COD              	SHIP      	requests? blithely fi
5569570	47806	311	2	47	82428.60	0.01	0.03	A	F	1994-09-20	1994-07-10	1994-09-22	TAKE BACK RETURN         	AIR       	d, final instructions. reques
5569570	74192	1714	3	4	4664.76	0.10	0.05	R	F	1994-06-22	1994-08-06	1994-07-21	DELIVER IN PERSON        	RAIL      	 ironic deposits wake blit
5569570	130209	210	4	38	47089.60	0.01	0.08	R	F	1994-09-13	1994-08-27	1994-09-23	COLLECT COD              	REG AIR   	unwind furiously carefu
5569570	22587	2588	5	42	63402.36	0.00	0.07	A	F	1994-06-29	1994-07-24	1994-07-04	NONE                     	FOB       	refully across the ideas. blithe
5573285	173710	6228	1	39	69564.69	0.04	0.05	A	F	1994-02-14	1994-02-21	1994-02-19	DELIVER IN PERSON        	FOB       	furiously ironic 
5590690	68735	3748	1	43	73260.39	0.10	0.01	R	F	1992-09-10	1992-07-15	1992-09-18	TAKE BACK RETURN         	REG AIR   	according to the quic
5590690	27875	5382	2	29	52283.23	0.10	0.06	A	F	1992-10-01	1992-08-03	1992-10-04	COLLECT COD              	TRUCK     	 the carefully special account
5612065	56099	8605	1	22	23211.98	0.04	0.08	N	O	1997-11-29	1997-11-24	1997-12-19	DELIVER IN PERSON        	TRUCK     	eodolites nag furiously brave pac
5612065	48883	8884	2	27	49460.76	0.08	0.05	N	O	1997-10-11	1997-11-28	1997-10-24	DELIVER IN PERSON        	FOB       	inal, final ideas hagg
5621349	146959	9474	1	36	72214.20	0.09	0.04	N	O	1998-05-22	1998-06-29	1998-06-15	NONE                     	AIR       	s doubt blithely about the slyly expre
5621349	135583	5584	2	4	6474.32	0.03	0.07	N	O	1998-06-24	1998-06-14	1998-07-20	COLLECT COD              	REG AIR   	r the quickly bold ideas. 
5621349	140633	8176	3	39	65271.57	0.09	0.05	N	O	1998-07-23	1998-06-01	1998-08-21	TAKE BACK RETURN         	MAIL      	arefully accordi
5621349	17093	7094	4	1	1010.09	0.03	0.02	N	O	1998-06-24	1998-07-08	1998-07-01	TAKE BACK RETURN         	MAIL      	ilent ideas. requests are quickly accordi
5621349	74319	6827	5	41	53025.71	0.09	0.02	N	O	1998-06-20	1998-05-31	1998-06-23	TAKE BACK RETURN         	RAIL      	refully ironic instruct
5621349	74694	7202	6	11	18355.59	0.07	0.01	N	O	1998-05-22	1998-05-19	1998-06-06	DELIVER IN PERSON        	SHIP      	lar Tiresias. ex
5621349	176225	3777	7	19	24723.18	0.09	0.08	N	O	1998-04-28	1998-05-21	1998-05-09	NONE                     	TRUCK     	 alongside of 
5650368	185090	2645	1	50	58754.50	0.04	0.06	A	F	1993-07-09	1993-07-02	1993-07-18	NONE                     	AIR       	ing requests ac
5650368	195908	8428	2	37	74144.30	0.00	0.02	R	F	1993-08-08	1993-08-01	1993-08-12	TAKE BACK RETURN         	TRUCK     	efully bold
5650368	17056	7057	3	47	45733.35	0.07	0.03	R	F	1993-06-10	1993-07-29	1993-07-03	NONE                     	SHIP      	osits after the blithely ironic pinto b
5650368	48366	871	4	8	10514.88	0.07	0.02	R	F	1993-07-18	1993-07-09	1993-08-07	TAKE BACK RETURN         	AIR       	 bold theodolites sleep 
5650368	82398	9923	5	22	30368.58	0.08	0.05	R	F	1993-08-22	1993-07-02	1993-09-15	DELIVER IN PERSON        	RAIL      	g the bold packages. pending packages sl
5650368	62556	7569	6	43	65297.65	0.04	0.06	A	F	1993-09-05	1993-07-04	1993-09-25	DELIVER IN PERSON        	AIR       	lyly ideas. slyly regular dep
5692003	93235	8254	1	12	14738.76	0.10	0.02	A	F	1995-02-19	1995-03-07	1995-02-20	COLLECT COD              	FOB       	gular excuses affix fluffily.
5692003	190366	7924	2	9	13107.24	0.08	0.01	A	F	1995-03-12	1995-02-06	1995-03-18	TAKE BACK RETURN         	TRUCK     	rate after the even
5693440	174557	7075	1	20	32631.00	0.01	0.03	R	F	1995-04-02	1995-04-07	1995-04-18	NONE                     	RAIL      	uriously iro
5693440	168119	5668	2	14	16619.54	0.06	0.07	A	F	1995-05-09	1995-05-01	1995-06-03	NONE                     	TRUCK     	fluffily bold re
5693440	161470	3987	3	6	9188.82	0.04	0.03	A	F	1995-03-22	1995-05-20	1995-04-02	NONE                     	SHIP      	 beans nag blithely ab
5693440	14539	2043	4	34	49420.02	0.03	0.06	A	F	1995-05-15	1995-04-17	1995-05-31	NONE                     	SHIP      	after the furiously silent p
5693440	154529	4530	5	5	7917.60	0.08	0.02	A	F	1995-03-05	1995-03-29	1995-03-13	TAKE BACK RETURN         	REG AIR   	tes: regular accounts bo
5739076	178495	3530	1	26	40910.74	0.03	0.04	N	O	1998-04-26	1998-02-22	1998-05-07	TAKE BACK RETURN         	REG AIR   	usly ironic deposits breach blithely. r
5739076	160341	2858	2	34	47645.56	0.04	0.03	N	O	1998-05-08	1998-03-30	1998-05-19	TAKE BACK RETURN         	MAIL      	furiously special pinto
5739076	43092	3093	3	23	23807.07	0.10	0.03	N	O	1998-03-02	1998-03-30	1998-03-23	TAKE BACK RETURN         	SHIP      	 deposits s
5739076	31483	8993	4	15	21217.20	0.00	0.01	N	O	1998-02-06	1998-02-20	1998-03-04	DELIVER IN PERSON        	FOB       	efully ironic courts cajole 
5739076	123783	3784	5	36	65044.08	0.10	0.01	N	O	1998-03-06	1998-02-11	1998-03-21	DELIVER IN PERSON        	AIR       	ias should detect fu
5748323	130180	2694	1	3	3630.54	0.10	0.06	A	F	1993-01-28	1993-03-17	1993-02-07	COLLECT COD              	REG AIR   	the furiously special pinto beans 
5748323	131432	8972	2	16	23414.88	0.08	0.07	R	F	1993-01-30	1993-04-07	1993-02-11	COLLECT COD              	TRUCK     	ng the blithely ironic deposits. furiousl
5748323	170946	947	3	6	12101.64	0.06	0.01	A	F	1993-01-21	1993-03-07	1993-02-13	NONE                     	MAIL      	ithely regular asymptotes nag
5749952	41775	9288	1	4	6867.08	0.08	0.00	R	F	1994-08-24	1994-10-23	1994-09-10	DELIVER IN PERSON        	FOB       	tions cajole above the quickly even
5749952	104585	2116	2	42	66762.36	0.03	0.03	A	F	1994-08-25	1994-09-28	1994-08-27	NONE                     	AIR       	ously alongside of the regular packages. sl
5749952	158546	6092	3	39	62577.06	0.00	0.00	A	F	1994-10-19	1994-10-04	1994-11-08	NONE                     	FOB       	nusual ideas affix blithely f
5749952	69356	6875	4	32	42411.20	0.05	0.07	A	F	1994-11-09	1994-10-29	1994-12-03	DELIVER IN PERSON        	SHIP      	excuses. quickly final somas cajole am
5779717	27772	7773	1	50	84988.50	0.01	0.03	A	F	1992-11-01	1992-12-12	1992-11-08	NONE                     	RAIL      	ns? express, bold waters cajole quick
5779717	149863	2378	2	19	36344.34	0.05	0.08	A	F	1992-11-03	1992-12-09	1992-11-20	COLLECT COD              	FOB       	oxes sleep car
5789926	1972	1973	1	26	48723.22	0.03	0.07	N	O	1996-06-05	1996-07-23	1996-06-08	DELIVER IN PERSON        	SHIP      	special ide
5789926	132321	2322	2	37	50072.84	0.06	0.04	N	O	1996-08-29	1996-08-14	1996-08-30	TAKE BACK RETURN         	REG AIR   	 quickly alongs
5789926	113542	1076	3	40	62221.60	0.02	0.06	N	O	1996-09-17	1996-07-19	1996-10-07	COLLECT COD              	AIR       	 fluffily about the
5789926	92056	7075	4	46	48210.30	0.03	0.02	N	O	1996-08-06	1996-07-24	1996-08-12	TAKE BACK RETURN         	RAIL      	e around the ex
5789926	175328	2880	5	8	11226.56	0.01	0.03	N	O	1996-05-29	1996-07-04	1996-06-02	DELIVER IN PERSON        	RAIL      	c packages wake a
5789926	130634	5661	6	35	58262.05	0.08	0.00	N	O	1996-09-23	1996-08-12	1996-10-11	NONE                     	MAIL      	ly regular packages along the ironic
5793253	2676	7677	1	49	77354.83	0.00	0.07	N	O	1996-08-08	1996-07-28	1996-08-14	DELIVER IN PERSON        	FOB       	blithely regular accounts sl
5793253	161213	8762	2	39	49694.19	0.04	0.01	N	O	1996-06-09	1996-07-10	1996-06-16	NONE                     	RAIL      	ep across the quickly pending forges.
5793253	99608	9609	3	26	41797.60	0.07	0.01	N	O	1996-06-08	1996-07-06	1996-06-18	DELIVER IN PERSON        	TRUCK     	ironic excuses. theodol
5793253	110101	2613	4	2	2222.20	0.00	0.08	N	O	1996-07-31	1996-07-15	1996-08-07	NONE                     	RAIL      	 are quickly against
5793253	64469	1988	5	7	10034.22	0.00	0.05	N	O	1996-07-03	1996-08-17	1996-07-31	COLLECT COD              	MAIL      	ironic, even 
5817441	50562	5573	1	48	72602.88	0.06	0.00	A	F	1993-12-12	1994-01-05	1993-12-13	TAKE BACK RETURN         	MAIL      	iously. ironic p
5819459	89471	9472	1	32	46735.04	0.09	0.08	N	O	1996-12-04	1996-11-22	1996-12-08	DELIVER IN PERSON        	RAIL      	ments affix sauternes. c
5819459	125082	2619	2	15	16606.20	0.10	0.02	N	O	1996-11-10	1996-11-26	1996-11-24	COLLECT COD              	FOB       	ependencies hang quick
5819459	47942	5455	3	9	17009.46	0.08	0.07	N	O	1997-01-24	1996-12-23	1997-02-12	NONE                     	MAIL      	ular pinto beans
5832548	153647	1193	1	7	11904.48	0.00	0.02	N	O	1997-07-25	1997-06-19	1997-07-26	DELIVER IN PERSON        	FOB       	 regular requests 
5880160	114597	2131	1	1	1611.59	0.00	0.01	N	O	1996-12-05	1996-10-12	1996-12-14	DELIVER IN PERSON        	AIR       	ut the pending, unusual
5880160	16973	1976	2	41	77488.77	0.06	0.03	N	O	1996-10-05	1996-09-22	1996-11-03	DELIVER IN PERSON        	REG AIR   	 against the slyly even pinto b
5880160	53936	8947	3	13	24569.09	0.01	0.04	N	O	1996-11-04	1996-10-04	1996-11-27	DELIVER IN PERSON        	RAIL      	losely ironic requests
5880160	89455	4472	4	26	37555.70	0.05	0.03	N	O	1996-11-11	1996-10-02	1996-12-04	DELIVER IN PERSON        	RAIL      	hely bold requests around the silently 
5880160	57816	2827	5	32	56761.92	0.02	0.08	N	O	1996-11-18	1996-10-18	1996-11-22	COLLECT COD              	TRUCK     	its are slyly. even deposits cajo
5957250	185052	2607	1	28	31837.40	0.00	0.02	N	O	1996-08-29	1996-10-19	1996-09-23	COLLECT COD              	REG AIR   	carefully pending pac
5957250	161413	1414	2	26	38334.66	0.06	0.07	N	O	1996-09-25	1996-10-11	1996-09-26	DELIVER IN PERSON        	RAIL      	refully. quickly specia
5957250	64418	1937	3	31	42854.71	0.06	0.06	N	O	1996-09-21	1996-10-20	1996-10-15	NONE                     	RAIL      	eposits. fluffily even theodolites alo
5957250	67048	7049	4	43	43646.72	0.10	0.06	N	O	1996-10-06	1996-10-11	1996-10-13	COLLECT COD              	TRUCK     	ideas use. special notornis sl
5957250	74093	9108	5	46	49086.14	0.08	0.08	N	O	1996-11-08	1996-10-06	1996-11-30	DELIVER IN PERSON        	SHIP      	osits haggle carefully among the regula
5979142	95268	287	1	27	34108.02	0.09	0.00	N	F	1995-06-06	1995-07-25	1995-07-04	COLLECT COD              	MAIL      	accounts mold slyly 
5979142	159410	9411	2	13	19102.33	0.04	0.05	N	O	1995-09-28	1995-07-17	1995-10-28	COLLECT COD              	MAIL      	y. carefully unusua
5979142	194359	6879	3	12	17440.20	0.02	0.04	N	O	1995-08-02	1995-08-29	1995-08-11	NONE                     	SHIP      	und the regular, regular esca
5979142	176236	1271	4	36	47240.28	0.05	0.00	N	F	1995-06-12	1995-08-15	1995-07-05	DELIVER IN PERSON        	RAIL      	alongside of the blithely reg
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
5739076	25	O	191006.24	1998-01-11	4-NOT SPECIFIED	Clerk#000000483	0	blithely unusual instruct	1998
9154	13	O	331327.34	1997-06-23	4-NOT SPECIFIED	Clerk#000000328	0	y ironic packages cajole. blithely final depende	1997
24322	29	O	254716.30	1997-01-29	1-URGENT       	Clerk#000000100	0	regular deposits cajole slyly. ironic t	1997
36422	10	O	287837.16	1997-03-04	3-MEDIUM       	Clerk#000000532	0	e evenly final dependencies. regular, bold pinto beans 	1997
53283	38	O	215180.08	1995-10-29	2-HIGH         	Clerk#000000802	0	ged requests. slyly final platelets	1995
66695	47	O	47247.12	1998-06-26	3-MEDIUM       	Clerk#000000674	0	st the fluffily final ideas are 	1998
68579	46	O	236329.06	1997-03-19	2-HIGH         	Clerk#000000701	0	; busily unusual ideas 	1997
74055	46	F	268034.25	1992-04-18	1-URGENT       	Clerk#000000656	0	packages about the slyly fi	1992
83684	22	O	71321.40	1998-03-19	3-MEDIUM       	Clerk#000000419	0	sly ironic accounts are furiously along the requests. sometimes ironic the	1998
160516	28	O	214385.82	1995-09-18	4-NOT SPECIFIED	Clerk#000000628	0	ts. regular, even d	1995
164711	4	F	311722.87	1992-04-26	3-MEDIUM       	Clerk#000000361	0	accounts among the regular, 	1992
210945	46	F	42341.06	1995-03-20	3-MEDIUM       	Clerk#000000232	0	 accounts. blithely final packages haggle abou	1995
224167	5	O	85170.68	1996-05-08	5-LOW          	Clerk#000000657	0	s along the carefully special pinto beans cajole carefully even sentim	1996
269922	16	O	111827.67	1996-03-19	4-NOT SPECIFIED	Clerk#000000417	0	ly final platelets cajole furiously alon	1996
287366	41	F	160224.44	1994-06-19	2-HIGH         	Clerk#000000330	0	 the furiously ironi	1994
287619	5	O	13620.11	1996-12-26	5-LOW          	Clerk#000000854	0	r requests must sleep carefully furiously regular tithes? slyly unusual reque	1996
301350	40	O	288374.44	1996-08-25	5-LOW          	Clerk#000000200	0	tructions among the carefully final somas affix ca	1996
324740	49	O	162652.29	1997-10-05	4-NOT SPECIFIED	Clerk#000000536	0	en grouches are furiously bold p	1997
330404	25	O	328890.87	1996-09-22	1-URGENT       	Clerk#000000124	0	ct carefully even courts. requests affix blithely	1996
337861	49	F	157916.63	1992-05-03	2-HIGH         	Clerk#000000573	0	ronic requests. carefully regular waters use carefully iro	1992
345858	25	O	35531.10	1998-06-15	2-HIGH         	Clerk#000000019	0	e unusual asymptotes. slyly regular pains wake carefully along the blithe 	1998
346693	22	F	67932.58	1993-11-13	1-URGENT       	Clerk#000000762	0	mptotes hang fluffily among the	1993
358886	40	O	207062.32	1995-08-28	3-MEDIUM       	Clerk#000000799	0	ses try to hang carefully unusual 	1995
374723	17	O	276933.53	1996-11-20	3-MEDIUM       	Clerk#000000899	0	its. final packages according to the sp	1996
385825	4	O	277493.04	1995-11-01	2-HIGH         	Clerk#000000465	0	e even foxes. regularly even instructions boost carefully along	1995
413985	46	O	231298.47	1995-07-06	2-HIGH         	Clerk#000000626	0	osits are carefully furiously final dolphins. even platelets agains	1995
440964	47	O	169784.81	1997-01-05	1-URGENT       	Clerk#000000064	0	y careful packages. quickly ironic pinto beans are f	1997
446499	40	O	17533.39	1997-03-09	1-URGENT       	Clerk#000000450	0	he unusual deposits	1997
491620	14	O	116496.93	1998-05-22	1-URGENT       	Clerk#000000878	0	 haggle among the slyly ironic foxes. packages	1998
512195	25	O	68001.87	1996-08-13	1-URGENT       	Clerk#000000133	0	ial, even depths sleep across the foxes	1996
545218	28	F	224684.48	1992-07-16	4-NOT SPECIFIED	Clerk#000000648	0	gular packages nag furiously deposits	1992
593664	49	O	187304.12	1995-10-09	4-NOT SPECIFIED	Clerk#000000946	0	riously final theodolites. slyly express accounts wake slyly dep	1995
681730	28	F	199241.79	1993-08-11	5-LOW          	Clerk#000000194	0	blithely ironic deposits wake across the sl	1993
686918	23	F	198594.46	1992-08-21	1-URGENT       	Clerk#000000622	0	ccounts-- final pinto beans cajole after the furio	1992
708706	37	O	280182.96	1996-03-18	3-MEDIUM       	Clerk#000000457	0	. quickly pending excuses impress. ironic packages wa	1996
724805	49	F	169499.53	1993-03-28	5-LOW          	Clerk#000000566	0	y ironic accounts. slyly pending notornis sleep quickly b	1993
737345	28	O	50764.15	1996-08-29	2-HIGH         	Clerk#000000838	0	nically regular foxes. slyly unusual packages sleep furiously slyly ironi	1996
774304	50	F	109304.74	1995-01-09	2-HIGH         	Clerk#000000199	0	. even pinto beans among the special accounts 	1995
816323	10	O	249097.39	1996-01-23	5-LOW          	Clerk#000000693	0	ts boost furiously pinto beans. regular deposits nag among the express 	1996
854375	23	F	163788.11	1993-09-16	3-MEDIUM       	Clerk#000000820	0	e special, special dependencies maintain b	1993
859108	10	O	135312.87	1996-02-20	3-MEDIUM       	Clerk#000000573	0	 pinto beans. final, unusual deposits use. final	1996
859235	37	O	26418.40	1995-08-20	3-MEDIUM       	Clerk#000000933	0	s unwind fluffily. forges detect doggedly deposits? c	1995
869574	13	O	65397.70	1998-01-21	2-HIGH         	Clerk#000000549	0	foxes are against the exp	1998
883557	10	O	53855.97	1998-03-30	2-HIGH         	Clerk#000000758	0	ses are carefully. slyly regular asympto	1998
895172	10	O	236806.93	1995-12-04	1-URGENT       	Clerk#000000673	0	cajole quickly slyly exp	1995
905633	5	O	261338.66	1995-07-05	4-NOT SPECIFIED	Clerk#000000254	0	arefully ironic attainments cajole slyly furiou	1995
916775	10	O	279937.32	1996-04-26	1-URGENT       	Clerk#000000032	0	express instructions. quickly unusual accounts cajole-- carefully i	1996
917893	38	O	214164.10	1998-07-14	3-MEDIUM       	Clerk#000000771	0	yly slyly special requests	1998
954660	49	F	220192.81	1994-12-04	1-URGENT       	Clerk#000000471	0	ckly across the requests. express, regular accounts haggle at the pending d	1994
969474	41	F	14454.26	1992-08-25	5-LOW          	Clerk#000000980	0	c requests use blithely carefully even platelets. sp	1992
1035265	49	F	25786.66	1992-08-02	2-HIGH         	Clerk#000000904	0	iously final requests wake quickly. express,	1992
1037540	25	F	210165.78	1992-02-18	5-LOW          	Clerk#000000915	0	 could nag blithely ironic foxes? final somas wake 	1992
1066274	40	F	101830.24	1994-04-28	2-HIGH         	Clerk#000000677	0	gular requests x-ray blithely alongside of the quietly fina	1994
1127620	41	O	235609.25	1997-05-10	1-URGENT       	Clerk#000000032	0	tealthily fluffy instructions. deposits use b	1997
1131267	46	F	174699.80	1992-11-24	2-HIGH         	Clerk#000000633	0	unts. pending packages cajole blithely bold escapades. final request	1992
1168417	25	O	315495.31	1996-10-12	2-HIGH         	Clerk#000000413	0	y bold packages integrate carefully. furiously express accounts haggle ca	1996
1177350	16	O	28853.79	1997-07-03	4-NOT SPECIFIED	Clerk#000000697	0	unts use. quickly final packages nag. regular frays lose s	1997
1192231	4	O	143971.54	1996-06-03	1-URGENT       	Clerk#000000978	0	osits haggle carefully. carefully final grouches are. blithely si	1996
1198339	37	O	101108.73	1996-01-29	3-MEDIUM       	Clerk#000000674	0	y among the careful requests. quickly re	1996
1216773	40	O	231837.32	1995-08-21	1-URGENT       	Clerk#000000446	0	. silent requests nod quickly furiously fi	1995
1226497	4	F	88317.19	1993-10-04	1-URGENT       	Clerk#000000154	0	al theodolites. quickly ruthless accounts lose blithely furiously re	1993
1247233	25	F	138475.98	1992-06-16	5-LOW          	Clerk#000000907	0	 beans after the carefully bold accounts play across the furiously	1992
1376704	40	F	195069.22	1994-05-04	3-MEDIUM       	Clerk#000000379	0	 pinto beans boost quickly about the quickly ironic foxes! fluffily final g	1994
1468323	46	F	198367.30	1994-11-17	2-HIGH         	Clerk#000000968	0	ld foxes among the instructions x-ray across the finally even package	1994
1469089	29	F	136945.15	1993-07-21	5-LOW          	Clerk#000000934	0	l excuses boost slyly. carefully special acc	1993
1478917	16	F	168851.39	1992-10-06	3-MEDIUM       	Clerk#000000911	0	kly express accounts hag	1992
1490087	10	O	227526.57	1996-07-10	4-NOT SPECIFIED	Clerk#000000953	0	lly regular deposits. furiously regular instructions thrash	1996
1521157	14	O	179208.64	1997-08-23	1-URGENT       	Clerk#000000937	0	y special accounts? requests cajole furiously accor	1997
1550053	28	F	189340.54	1993-11-01	1-URGENT       	Clerk#000000349	0	lar accounts cajole blithely permanent instructions. qui	1993
1572644	16	O	273246.73	1998-06-01	5-LOW          	Clerk#000000018	0	as furiously blithely bold orbits. furiously bold courts cajole quickly. 	1998
1590469	4	O	75928.49	1997-03-07	2-HIGH         	Clerk#000000217	0	y pending pinto beans. carefully express theodolites cajole 	1997
1591941	25	O	140563.53	1997-02-07	2-HIGH         	Clerk#000000249	0	 fluffily final warthogs haggle blithely about the even	1997
1597121	25	F	170402.38	1992-04-14	3-MEDIUM       	Clerk#000000968	0	y bold grouches wake blithely around the carefully even 	1992
1597378	46	O	196574.36	1996-06-04	4-NOT SPECIFIED	Clerk#000000101	0	s use doggedly for the fluffily special deposits. furiously express deposits 	1996
1603585	13	O	246076.45	1997-03-26	3-MEDIUM       	Clerk#000000848	0	bold, regular accounts are furiously ac	1997
1620672	47	O	164956.30	1997-02-14	4-NOT SPECIFIED	Clerk#000000889	0	cajole. slyly bold packages need to boost gifts. 	1997
1646340	17	O	121436.61	1998-07-11	5-LOW          	Clerk#000000990	0	lar ideas against the blithely bold packages wake blithely 	1998
1651463	40	F	47219.96	1993-06-17	5-LOW          	Clerk#000000821	0	ly express deposits cajole slyly quickly ironic request	1993
1718016	13	F	152605.93	1994-08-30	5-LOW          	Clerk#000000900	0	requests against the ironic, special foxes wake blithely ruthless theodol	1994
1732771	49	F	35865.49	1994-09-03	1-URGENT       	Clerk#000000039	0	pending packages coul	1994
1737250	23	F	220254.25	1994-10-08	5-LOW          	Clerk#000000736	0	layers hang carefully past the deposits? requests u	1994
1755398	4	O	1131.20	1997-06-12	5-LOW          	Clerk#000000765	0	s. slyly ironic packa	1997
1768549	46	F	43341.89	1994-02-24	3-MEDIUM       	Clerk#000000978	0	ges sleep fluffily beside the packages. carefully regular de	1994
1774689	10	F	13822.61	1993-07-08	3-MEDIUM       	Clerk#000000702	0	 deposits are fluffily quick	1993
1783941	28	O	39913.91	1995-07-12	2-HIGH         	Clerk#000000894	0	s. regular deposits are carefully deposits. pinto beans 	1995
1819008	40	F	255369.33	1992-11-02	4-NOT SPECIFIED	Clerk#000000271	0	al foxes. platelets cajole quickly	1992
1843907	23	F	174664.45	1992-10-25	1-URGENT       	Clerk#000000632	0	ss foxes. platelets about the care	1992
1857794	40	F	278888.85	1993-05-08	4-NOT SPECIFIED	Clerk#000000564	0	egular foxes. ironic, pending packages doze. ironic, ir	1993
1869056	29	O	243555.56	1995-05-31	5-LOW          	Clerk#000000345	0	ven theodolites. slyly ironic theodolites 	1995
1876000	37	F	176371.33	1995-01-28	1-URGENT       	Clerk#000000129	0	uriously bold pinto beans integrate blithely around the spec	1995
1876550	22	F	149450.45	1992-05-18	1-URGENT       	Clerk#000000320	0	uickly about the carefully final packages. s	1992
1883718	49	O	131285.41	1998-01-21	4-NOT SPECIFIED	Clerk#000000599	0	. unusual excuses unwind car	1998
1932643	14	O	323688.70	1997-06-01	3-MEDIUM       	Clerk#000000362	0	dolites could are. even deposits haggle slyly. fluffily unusua	1997
1944711	4	P	104934.65	1995-05-06	1-URGENT       	Clerk#000000580	0	deas wake. slyly silent packa	1995
1950947	17	F	153952.98	1994-08-10	5-LOW          	Clerk#000000703	0	after the regular inst	1994
1951169	38	O	276315.13	1996-01-29	3-MEDIUM       	Clerk#000000638	0	al packages nag after the slyly regular re	1996
1953381	13	F	101661.97	1994-06-17	3-MEDIUM       	Clerk#000000152	0	ss orbits. pending accounts are bravely. quickly even 	1994
1953441	4	O	235621.83	1996-06-06	2-HIGH         	Clerk#000000288	0	iers are. quickly final instructi	1996
1953926	40	F	147421.31	1993-05-14	4-NOT SPECIFIED	Clerk#000000560	0	 are; furiously even requests wake carefully regular packages. fina	1993
1967460	14	O	44592.44	1996-08-11	4-NOT SPECIFIED	Clerk#000000464	0	ily express ideas are a	1996
1978756	4	O	314671.82	1996-08-02	3-MEDIUM       	Clerk#000000661	0	es. blithely special theodolites cajole slyly carefully regular asymp	1996
1978854	40	F	253253.53	1993-01-08	1-URGENT       	Clerk#000000738	0	accounts are about the furiously regular idea	1993
1991461	13	F	256977.48	1993-08-20	3-MEDIUM       	Clerk#000000415	0	e slyly ironic accounts. slyly final foxes nag about the ironic, reg	1993
2007493	46	O	154683.16	1997-06-19	1-URGENT       	Clerk#000000482	0	 furiously regular fox	1997
2023140	49	F	85989.50	1994-04-08	5-LOW          	Clerk#000000986	0	 are among the regular deposits. requests boos	1994
2126688	10	F	50998.03	1993-08-15	4-NOT SPECIFIED	Clerk#000000207	0	r, silent accounts haggle o	1993
2231556	37	O	108340.12	1995-10-23	4-NOT SPECIFIED	Clerk#000000109	0	 the fluffily special accounts.	1995
2242243	16	F	36016.91	1994-01-20	3-MEDIUM       	Clerk#000000191	0	the ironic theodolites. ironic, final excuses use quickly requests. regular 	1994
2248546	13	F	161835.66	1994-03-15	1-URGENT       	Clerk#000000511	0	 final notornis haggle carefully furiously bold dep	1994
2342115	28	F	230732.43	1992-11-07	5-LOW          	Clerk#000000257	0	 of the slyly special ideas. furiously even accounts abo	1992
2370438	28	O	195467.72	1997-06-24	1-URGENT       	Clerk#000000247	0	ts detect blithely carefully 	1997
2401414	40	O	187172.94	1996-04-26	3-MEDIUM       	Clerk#000000789	0	furiously furiously special theodolites. regular packages	1996
2425574	25	F	33142.79	1993-11-25	2-HIGH         	Clerk#000000074	0	in. carefully regular foxes 	1993
2459619	4	O	46227.94	1996-01-03	5-LOW          	Clerk#000000917	0	iously ironic pinto beans sleep furiously across the quickly unusual notornis.	1996
2471840	40	O	308184.32	1996-06-03	5-LOW          	Clerk#000000459	0	ter the slyly special ide	1996
2488771	37	F	260294.29	1992-05-23	1-URGENT       	Clerk#000000117	0	olites sleep against the silent instructions. fluffi	1992
2507303	22	O	16845.36	1997-10-09	2-HIGH         	Clerk#000000016	0	l requests along the special foxes sleep slyly express deposits? ironic, eve	1997
2546947	41	O	184965.05	1997-06-27	3-MEDIUM       	Clerk#000000145	0	lyly theodolites. regularly bold packages try to nag carefully fluffily	1997
2559110	38	O	203191.72	1996-08-08	4-NOT SPECIFIED	Clerk#000000092	0	t the furiously express requests. even, re	1996
2602208	49	F	339832.12	1992-04-14	3-MEDIUM       	Clerk#000000255	0	al dolphins use furiously a	1992
2604326	22	F	77794.30	1993-02-08	3-MEDIUM       	Clerk#000000742	0	 the furiously regular theodolites. express, pending dep	1993
2630562	5	F	324835.83	1993-06-27	4-NOT SPECIFIED	Clerk#000000966	0	carefully final deposits. quickly silent pains are among the careful	1993
2646244	22	O	178885.67	1997-08-30	4-NOT SPECIFIED	Clerk#000000227	0	lar foxes after the carefully regular accounts use slyly fluffily ironi	1997
2651266	13	F	159823.91	1994-03-26	4-NOT SPECIFIED	Clerk#000000540	0	se pending, regular instructions. regu	1994
2665734	16	O	46978.40	1995-03-01	1-URGENT       	Clerk#000000490	0	se across the ironic deposits. excuses detect slyly. doggedly even in	1995
2705092	29	F	292830.88	1993-10-08	2-HIGH         	Clerk#000000448	0	ts affix furiously. regular instructions wake fluffily even foxes. unusual ide	1993
2708037	50	O	163467.14	1998-07-11	5-LOW          	Clerk#000000756	0	ly about the furiously silent fox	1998
2708259	50	O	89422.61	1995-08-08	1-URGENT       	Clerk#000000108	0	y special requests cajole carefully across the qui	1995
2712836	29	F	9268.53	1992-06-30	2-HIGH         	Clerk#000000255	0	 carefully even packages among the doggedly regular requests detect ca	1992
2718208	22	O	168257.57	1995-12-30	1-URGENT       	Clerk#000000290	0	ut the slyly even dependencies. quickly ironic request	1995
2736865	16	F	269687.81	1994-09-16	2-HIGH         	Clerk#000000008	0	are furiously furiously ironic req	1994
2765152	4	O	8332.33	1996-08-11	4-NOT SPECIFIED	Clerk#000000618	0	onic packages. slyly express requests	1996
2773829	16	F	244353.30	1992-11-29	3-MEDIUM       	Clerk#000000277	0	 deposits about the quickly express pinto beans thrash after	1992
2782659	37	O	30807.04	1996-10-29	4-NOT SPECIFIED	Clerk#000000981	0	slyly. blithely special requests across the quickly final	1996
2790881	37	O	106068.15	1995-05-18	4-NOT SPECIFIED	Clerk#000000475	0	al packages. slyly express pa	1995
2806528	37	F	98053.54	1993-06-22	4-NOT SPECIFIED	Clerk#000000376	0	l packages are slyly dogged	1993
2826500	23	F	99134.39	1994-08-27	5-LOW          	Clerk#000000353	0	yly express requests wake furiously at the slyly idle accounts. carefully	1994
2892036	37	F	60430.48	1992-04-19	4-NOT SPECIFIED	Clerk#000000590	0	lithely special deposits breach. th	1992
2897441	28	F	21123.95	1993-03-11	1-URGENT       	Clerk#000000795	0	 dolphins. blithely pending asy	1993
2916320	17	F	173870.07	1994-07-09	5-LOW          	Clerk#000001000	0	bout the packages. 	1994
2916672	40	F	176690.46	1992-12-20	4-NOT SPECIFIED	Clerk#000000550	0	ecial asymptotes. special Tiresias grow across the doggedly quiet pi	1992
2917345	10	O	52546.37	1995-10-01	4-NOT SPECIFIED	Clerk#000000983	0	ly regular accounts. special packages haggle furio	1995
2958117	13	F	78761.73	1993-02-01	2-HIGH         	Clerk#000000921	0	ronic accounts haggle among the braids. sometim	1993
2986913	4	F	40347.48	1994-06-10	3-MEDIUM       	Clerk#000000633	0	, bold accounts use slyly even excuses. pending req	1994
3012964	23	F	134155.09	1993-06-22	4-NOT SPECIFIED	Clerk#000000201	0	ccounts could are quickly. regular dep	1993
3017056	40	F	7438.41	1994-01-08	2-HIGH         	Clerk#000000477	0	ges. furiously express instructions from the spec	1994
3036614	22	F	203748.46	1992-12-07	1-URGENT       	Clerk#000000804	0	t the furiously pending foxes. regular, ironic theodolites 	1992
3051136	14	F	13726.90	1994-01-04	2-HIGH         	Clerk#000000486	0	 special foxes. final pinto beans are on the instructions. furiously even 	1994
3069221	10	O	26861.45	1996-10-29	3-MEDIUM       	Clerk#000000005	0	ess requests. carefully si	1996
3120674	25	O	178014.76	1996-07-19	4-NOT SPECIFIED	Clerk#000000682	0	uests. regular, bold grouches are. furi	1996
3176421	16	O	171625.54	1995-10-28	3-MEDIUM       	Clerk#000000644	0	quickly according to the carefully final packages. packages sleep iro	1995
3251169	4	O	225294.26	1996-01-06	5-LOW          	Clerk#000000310	0	arefully final packages boost against the furiously	1996
3252322	49	F	208589.82	1993-05-12	2-HIGH         	Clerk#000000388	0	y platelets. packages detect carefull	1993
3268516	38	O	146933.05	1996-11-28	2-HIGH         	Clerk#000000979	0	gifts. ironic, even courts after the even deposits wake furiously aft	1996
3330400	22	O	49314.69	1997-09-09	5-LOW          	Clerk#000000284	0	nts run blithely evenly regular deposits	1997
3354726	10	O	158148.30	1998-01-10	3-MEDIUM       	Clerk#000000427	0	ggle blithely ironic notornis. carefully bold deposits use iro	1998
3368547	22	O	235048.75	1997-05-12	5-LOW          	Clerk#000000193	0	 quickly bold dependencies. furiously even ideas use regular req	1997
3368833	40	O	81607.72	1997-12-28	3-MEDIUM       	Clerk#000000183	0	old accounts. furiously even deposits cajol	1997
3378979	13	O	100076.07	1996-08-16	3-MEDIUM       	Clerk#000000882	0	refully final foxes may print carefully.	1996
3421092	4	O	181618.75	1998-05-16	3-MEDIUM       	Clerk#000000762	0	refully even pinto beans. furiously	1998
3440902	16	F	47463.41	1992-07-15	5-LOW          	Clerk#000000472	0	 the packages. unusual pinto beans wake sl	1992
3465376	37	O	32347.90	1995-04-25	3-MEDIUM       	Clerk#000000583	0	blithely regular, even requests. carefully ironic requests ha	1995
3487745	10	O	193145.38	1996-09-26	1-URGENT       	Clerk#000000230	0	uctions. pending dependencies can boost regular attainments-- 	1996
3504453	16	O	251187.41	1997-04-24	1-URGENT       	Clerk#000000057	0	ly even pinto beans. boldly unusual pinto beans according	1997
3540450	13	F	21643.67	1994-08-21	2-HIGH         	Clerk#000000374	0	lithely special deposits. caref	1994
3554659	13	F	125100.57	1994-12-15	3-MEDIUM       	Clerk#000000275	0	gedly ironic packages after the depe	1994
3568032	28	O	70225.58	1997-09-13	5-LOW          	Clerk#000000484	0	oubt carefully pending deposits. ironic requests according to the furi	1997
3580034	10	F	229817.03	1992-07-09	4-NOT SPECIFIED	Clerk#000000118	0	 slyly pending accounts after the sly	1992
3628518	22	O	199040.29	1995-11-07	4-NOT SPECIFIED	Clerk#000000546	0	ounts. packages wake at 	1995
3631968	22	F	252765.40	1994-01-16	5-LOW          	Clerk#000000369	0	ep blithely against the fi	1994
3640352	28	F	248939.07	1994-10-20	5-LOW          	Clerk#000000875	0	 ironic accounts breach carefully express instructions. express packages s	1994
3671879	23	O	192869.53	1997-05-05	3-MEDIUM       	Clerk#000000188	0	ly even accounts. furiously express fo	1997
3683623	4	O	152929.80	1996-11-29	3-MEDIUM       	Clerk#000000652	0	lve. furiously silent fret	1996
3696386	25	O	178710.68	1998-06-15	4-NOT SPECIFIED	Clerk#000000073	0	the blithely ruthless platelets. bl	1998
3727234	40	O	156289.75	1997-02-16	3-MEDIUM       	Clerk#000000245	0	ld accounts maintain furiously across th	1997
3732196	37	O	124990.99	1996-11-14	2-HIGH         	Clerk#000000043	0	 instructions sleep s	1996
3742659	17	F	36397.26	1994-03-29	5-LOW          	Clerk#000000640	0	 requests haggle. even, fin	1994
3765443	40	O	223132.77	1995-06-21	2-HIGH         	Clerk#000000231	0	ely ironic requests. blithely ironic courts affix. regular courts doubt ca	1995
3768484	29	F	78749.34	1994-04-27	5-LOW          	Clerk#000000511	0	kly ironic courts. quickly regular deposits around the fu	1994
3772225	28	F	202091.42	1993-06-10	2-HIGH         	Clerk#000000742	0	iously express foxes. silent, final courts haggle ironic requests.	1993
3795685	29	O	248482.45	1996-04-29	5-LOW          	Clerk#000000252	0	carefully final courts nag. carefully regular foxes acco	1996
3837959	25	O	75823.91	1997-06-15	3-MEDIUM       	Clerk#000000546	0	gular asymptotes. evenly ironic excuses wake about the furiously regular 	1997
3840420	40	O	125352.13	1996-01-07	3-MEDIUM       	Clerk#000000626	0	l accounts. pending ideas sublate carefully. even, ironic requests cajole	1996
3851905	17	F	229744.35	1993-12-26	5-LOW          	Clerk#000000804	0	foxes. slyly ironic accounts wake fluffily carefully iro	1993
3872644	16	F	75077.48	1994-11-05	1-URGENT       	Clerk#000000542	0	c Tiresias. furiously unusual pinto beans are carefully across the care	1994
3916288	10	O	99925.33	1997-07-20	4-NOT SPECIFIED	Clerk#000000272	0	t integrate. carefully regular theodolites sleep c	1997
3918433	41	F	166290.01	1993-08-30	5-LOW          	Clerk#000000752	0	accounts. final pinto beans haggle quickly sly	1993
3933538	49	F	269471.85	1992-11-10	1-URGENT       	Clerk#000000358	0	slyly idle pinto bean	1992
3934949	40	F	159057.92	1994-12-01	3-MEDIUM       	Clerk#000000765	0	 the express instructions. furious	1994
3938022	14	O	36295.29	1997-01-11	3-MEDIUM       	Clerk#000000496	0	into beans. bold foxes above the regular deposi	1997
3942656	10	F	327960.68	1992-08-16	3-MEDIUM       	Clerk#000000954	0	jole from the slyly bold deposits. quickly expres	1992
3950849	28	O	198902.89	1996-12-20	5-LOW          	Clerk#000000228	0	ongside of the final theodolites might 	1996
3951331	4	O	76100.95	1997-11-23	3-MEDIUM       	Clerk#000000341	0	ly express platelets above the carefully	1997
3952544	16	O	121970.92	1997-01-28	5-LOW          	Clerk#000000007	0	believe. quickly furious deposits about the regular, 	1997
4009575	47	F	126258.37	1993-01-27	4-NOT SPECIFIED	Clerk#000000323	0	counts lose. slyly final accounts affix careful	1993
4114983	14	O	8716.43	1997-07-14	5-LOW          	Clerk#000000680	0	ans. final, bold foxes boost. furiously special packages acco	1997
4129186	13	O	6347.54	1996-05-05	4-NOT SPECIFIED	Clerk#000000769	0	, bold foxes solve under the furiously unusual instruct	1996
4141668	10	F	168477.12	1995-03-24	1-URGENT       	Clerk#000000849	0	 dependencies. blithely pending accounts solve carefully furiously ex	1995
4143526	37	O	157309.26	1996-09-15	1-URGENT       	Clerk#000000148	0	 breach blithely along the final packages. furiously 	1996
4144037	23	O	168934.67	1998-04-25	3-MEDIUM       	Clerk#000000870	0	 quickly. furiously regular requests use quickly. b	1998
4148227	22	O	378250.37	1995-07-12	2-HIGH         	Clerk#000000875	0	requests above the blithely	1995
4150085	47	F	175677.17	1993-01-19	5-LOW          	Clerk#000000035	0	sts cajole slyly regular deposits. fluffily s	1993
4161248	16	F	172325.17	1992-07-30	3-MEDIUM       	Clerk#000000645	0	ggle quickly carefully silent 	1992
4192001	25	O	321333.30	1997-01-06	3-MEDIUM       	Clerk#000000812	0	ut the quickly bold requests. slyly regular requests detect blithely. care	1997
4243142	10	O	132159.26	1995-11-20	3-MEDIUM       	Clerk#000000004	0	ironic foxes must haggle blithely 	1995
4243392	13	O	117004.98	1996-03-19	2-HIGH         	Clerk#000000059	0	ke slyly. carefully final requests sleep furiously from the	1996
4314274	37	O	247151.64	1997-03-23	2-HIGH         	Clerk#000000211	0	ironic requests about the final, pending foxe	1997
4320612	4	F	182956.01	1992-09-20	2-HIGH         	Clerk#000000994	0	 silently ironic theodolites are around the r	1992
4324037	22	O	25364.21	1997-12-16	1-URGENT       	Clerk#000000418	0	ly ironic deposits shall use slyly along the	1997
4327493	40	F	103280.48	1994-06-29	2-HIGH         	Clerk#000000946	0	 fluffily bold accounts. pinto	1994
4337954	50	O	6628.69	1995-06-17	3-MEDIUM       	Clerk#000000199	0	lites boost quickly. carefully bold pinto beans nag carefully furiously	1995
4342725	16	O	148369.61	1996-05-04	3-MEDIUM       	Clerk#000000193	0	ding to the pending, idle requests. expr	1996
4363712	37	F	192028.19	1993-09-16	1-URGENT       	Clerk#000000941	0	ake permanently against	1993
4364800	47	F	134532.44	1992-07-24	1-URGENT       	Clerk#000000847	0	. fluffily ironic requests sleep slyly? even deposit	1992
4405509	16	O	194190.97	1996-04-25	1-URGENT       	Clerk#000000584	0	ades! busily regular s	1996
4407621	10	O	93717.95	1995-09-26	5-LOW          	Clerk#000000354	0	ly pending notornis sleep furiously about the i	1995
4409734	46	F	142657.01	1995-02-02	1-URGENT       	Clerk#000000659	0	uriously unusual Tiresias ac	1995
4428354	16	O	279035.61	1996-11-08	4-NOT SPECIFIED	Clerk#000000118	0	uctions haggle furiously above the blithely bold theodolite	1996
4516484	37	O	290776.94	1998-03-12	1-URGENT       	Clerk#000000489	0	 instructions. carefully bold fox	1998
4520518	46	F	49304.87	1995-01-29	5-LOW          	Clerk#000000964	0	ets integrate furiously along the packages. carefully s	1995
4525251	38	O	22202.38	1996-03-07	3-MEDIUM       	Clerk#000000391	0	ously regular deposits. ironic pinto bea	1996
4535970	40	F	89021.07	1993-03-01	3-MEDIUM       	Clerk#000000996	0	ckages are ironic ideas. carefully regular courts 	1993
4553955	49	O	30383.88	1996-05-15	2-HIGH         	Clerk#000000391	0	ely alongside of the fu	1996
4603875	22	O	265407.55	1998-05-27	2-HIGH         	Clerk#000000958	0	along the quickly express excuses are according to the carefully even inst	1998
4613986	40	O	37081.80	1997-06-05	2-HIGH         	Clerk#000000756	0	nts. slyly unusual excuse	1997
4616224	10	O	21631.76	1997-01-22	5-LOW          	Clerk#000000818	0	into beans haggle c	1997
4639746	29	O	183612.59	1995-08-30	4-NOT SPECIFIED	Clerk#000000109	0	r requests. carefully ironic requests boost daringly. deposits after the ironi	1995
4718403	29	O	154324.94	1996-10-07	1-URGENT       	Clerk#000000655	0	uriously. foxes affix	1996
4764293	13	F	52585.81	1992-03-15	5-LOW          	Clerk#000000262	0	e express, final instructions integrate fluffily attainments. theo	1992
4776288	28	O	54474.14	1996-02-22	5-LOW          	Clerk#000000124	0	lly special pinto beans was against the even	1996
4794213	22	F	184300.17	1995-01-10	5-LOW          	Clerk#000000846	0	ccounts serve. blithely bold foxes detect slyly regular, fina	1995
4829253	50	O	161080.39	1998-01-12	1-URGENT       	Clerk#000000302	0	tegrate furiously? slyly ironic reques	1998
4839942	16	F	126550.81	1994-01-09	1-URGENT       	Clerk#000000079	0	onic accounts. daringly pending 	1994
4862951	25	F	203259.16	1995-01-27	3-MEDIUM       	Clerk#000000486	0	r, regular packages boost carefully? furiously regular packages cajol	1995
4872388	37	F	223412.01	1993-10-16	4-NOT SPECIFIED	Clerk#000000223	0	arefully final asymptotes haggle carefully f	1993
4881061	28	O	37996.67	1997-02-19	3-MEDIUM       	Clerk#000000039	0	e accounts are carefully quickly reg	1997
4917250	22	F	27723.76	1993-09-07	3-MEDIUM       	Clerk#000000028	0	regular deposits use regular, furious packages. unusual ideas cajole	1993
4932935	22	F	174378.24	1995-01-01	2-HIGH         	Clerk#000000935	0	e blithely. carefully bold excuses nag among the 	1995
4942119	40	F	142224.11	1992-10-21	2-HIGH         	Clerk#000000656	0	ate permanently across the quickly final instructions. regular req	1992
4950721	37	P	255097.56	1995-03-10	3-MEDIUM       	Clerk#000000156	0	to beans! bold, silent	1995
4960614	4	O	91152.97	1997-10-03	4-NOT SPECIFIED	Clerk#000000706	0	ithely final dependencies cajole blithely. express instructions are slyly a	1997
4966627	38	F	31837.62	1993-02-17	1-URGENT       	Clerk#000000381	0	ending braids according to the carefully bold ideas nag caref	1993
4968129	38	F	132003.66	1994-07-18	3-MEDIUM       	Clerk#000000217	0	ly according to the furiously unusual accounts. blithely blithe theodolites	1994
4968294	49	F	154756.57	1993-01-14	1-URGENT       	Clerk#000000020	0	ng theodolites. furiously special ideas detect slyly sile	1993
4968519	50	O	68448.11	1998-06-05	4-NOT SPECIFIED	Clerk#000000643	0	y blithely regular pinto 	1998
4970980	16	F	259523.03	1993-02-28	2-HIGH         	Clerk#000000306	0	inal, regular requests cajole. special courts are fluff	1993
4972039	16	O	166072.71	1998-03-23	2-HIGH         	Clerk#000000378	0	special platelets. unusual requests hi	1998
5003109	38	F	322359.41	1994-07-05	5-LOW          	Clerk#000000813	0	ic pinto beans nag fluffily. slyly final	1994
5052385	25	F	125435.16	1992-07-25	1-URGENT       	Clerk#000000678	0	accounts nag quickly against the bold deposits. slyly spec	1992
5093856	46	F	58857.45	1992-05-29	4-NOT SPECIFIED	Clerk#000000967	0	ual deposits try to wake brav	1992
5096387	17	O	183112.74	1998-03-15	5-LOW          	Clerk#000000136	0	furiously ironic notornis. slyly regular 	1998
5173218	38	O	140014.73	1996-01-23	5-LOW          	Clerk#000000018	0	regular excuses. ironic instructions cajole blithely bold, regular	1996
5200901	41	O	153083.68	1995-11-24	1-URGENT       	Clerk#000000134	0	onic platelets. bold accounts are; carefully regular platelets	1995
5207685	22	F	182572.12	1994-05-28	2-HIGH         	Clerk#000000763	0	its. bold warthogs sleep slyly boldly ironic requests. even, regular requests 	1994
5228454	37	O	51185.49	1997-11-13	1-URGENT       	Clerk#000000498	0	 express, even orbits. blithely fi	1997
5235045	49	F	150016.80	1992-11-16	2-HIGH         	Clerk#000000923	0	kages haggle blithely final, express pinto beans. 	1992
5237730	40	F	196543.45	1994-08-20	2-HIGH         	Clerk#000000121	0	pinto beans. carefully unusual theodolites are carefully a	1994
5241223	29	F	131536.65	1994-03-25	2-HIGH         	Clerk#000000873	0	ly against the regul	1994
5262498	25	O	78652.46	1996-12-27	3-MEDIUM       	Clerk#000000883	0	ckly careful ideas sleep furiously among the unusual deposits. blithely 	1996
5287012	22	O	74162.29	1997-08-11	3-MEDIUM       	Clerk#000000093	0	. express theodolites use blithely at	1997
5366593	13	F	57760.60	1993-06-13	3-MEDIUM       	Clerk#000000398	0	even platelets nag slyly against the 	1993
5378496	49	F	82332.99	1994-01-23	3-MEDIUM       	Clerk#000000752	0	sts haggle slyly across the quickly ironic warhorses. brave ideas haggle even 	1994
5381317	29	O	34290.42	1996-07-09	5-LOW          	Clerk#000000461	0	posits. depths are carefully 	1996
5406599	46	F	124870.99	1994-08-03	3-MEDIUM       	Clerk#000000078	0	pending dependencies. blithely careful requests haggle slyly special acc	1994
5414466	14	F	149205.61	1992-05-20	4-NOT SPECIFIED	Clerk#000000639	0	yly regular asymptotes nag around the regular a	1992
5418919	22	O	149030.77	1996-10-25	2-HIGH         	Clerk#000000770	0	. carefully final deposits haggle carefully. regular deposi	1996
5425221	46	F	16366.99	1994-07-08	3-MEDIUM       	Clerk#000000308	0	ously against the foxes. final accou	1994
5453440	4	O	17938.41	1997-07-12	2-HIGH         	Clerk#000000360	0	s sleep idly foxes. final accounts x-ray slyly. slyly even platelets h	1997
5476355	49	P	286333.34	1995-04-11	3-MEDIUM       	Clerk#000000071	0	jole blithely ironic requests. carefully 	1995
5496967	28	F	171402.45	1992-07-30	5-LOW          	Clerk#000000886	0	nic deposits: ironic excuses boost quickly around t	1992
5534435	14	O	136184.51	1997-03-28	5-LOW          	Clerk#000000269	0	refully regular accounts ought to boost among the regular, ironic depos	1997
5535201	28	O	59094.81	1998-02-15	4-NOT SPECIFIED	Clerk#000000423	0	iously ironic foxes. slyly unusual excuses ca	1998
5551104	28	O	20240.39	1996-10-29	1-URGENT       	Clerk#000000704	0	c packages run ideas. blithely ironic requests haggle doggedly	1996
5566567	14	F	56285.71	1993-12-02	4-NOT SPECIFIED	Clerk#000000445	0	theodolites are carefully unusual packages? furiously regul	1993
5569570	50	F	224607.12	1994-05-29	5-LOW          	Clerk#000000675	0	 carefully furiously final packages. bold, final deposi	1994
5573285	40	F	70121.20	1993-12-19	2-HIGH         	Clerk#000000757	0	tealthy packages? blithely unusual requests	1993
5590690	22	F	116471.88	1992-06-11	5-LOW          	Clerk#000000184	0	leep according to the furiously bold requests. quickly 	1992
5612065	4	O	71845.26	1997-09-07	3-MEDIUM       	Clerk#000000999	0	quests boost fluffily. blithely f	1997
5621349	25	O	229187.31	1998-04-09	3-MEDIUM       	Clerk#000000505	0	ng to the slyly unusual instructions. even, unusual depos	1998
5650368	47	F	284981.04	1993-05-25	4-NOT SPECIFIED	Clerk#000000911	0	ckly deposits. requests above the furiously stealthy deposits would wake c	1993
5692003	40	F	25709.41	1995-01-06	1-URGENT       	Clerk#000000447	0	beans. instructions nag slyly: slyly regular pinto beans cajole. id	1995
5693440	13	F	117319.16	1995-02-27	4-NOT SPECIFIED	Clerk#000000591	0	cajole slyly pinto bea	1995
5748323	37	F	38002.40	1993-01-10	2-HIGH         	Clerk#000000315	0	arefully about the furiously final pinto beans. stealthily even ide	1993
5749952	23	F	178708.01	1994-08-14	4-NOT SPECIFIED	Clerk#000000252	0	e slyly! carefully bold reque	1994
5779717	16	F	123952.04	1992-10-09	2-HIGH         	Clerk#000000688	0	fully express asymptotes. requests are. slyly express packages haggl	1992
5789926	22	O	276904.84	1996-05-27	3-MEDIUM       	Clerk#000000976	0	 even packages mold blithely.	1996
5793253	22	O	183149.51	1996-06-06	5-LOW          	Clerk#000000987	0	ecial courts. sometimes regular dolphins wake blithely care	1996
5817441	29	F	68246.70	1993-11-23	4-NOT SPECIFIED	Clerk#000000058	0	e about the fluffily pending frets. furious	1993
5819459	50	O	77919.78	1996-10-15	2-HIGH         	Clerk#000000304	0	kages ought to are quickly. slyly special dependencies nag fluffi	1996
5832548	41	O	12142.56	1997-04-11	2-HIGH         	Clerk#000000886	0	 furiously slyly ironic deposits? deposit	1997
5880160	13	O	198773.69	1996-08-08	4-NOT SPECIFIED	Clerk#000000208	0	ctions are quietly unusual packages. silen	1996
5957250	17	O	204142.49	1996-08-04	3-MEDIUM       	Clerk#000000239	0	are blithely along the slyly bold requests. unusual dependencies haggle fl	1996
5979142	25	P	112946.73	1995-06-01	4-NOT SPECIFIED	Clerk#000000739	0	 blithely bold notornis nod 	1995
\.


--
-- Data for Name: part; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.part (p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment) FROM stdin;
1	goldenrod lavender spring chocolate lace	Manufacturer#1           	Brand#13  	PROMO BURNISHED COPPER	7	JUMBO PKG 	901.00	ly. slyly ironi
2	blush thistle blue yellow saddle	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	1	LG CASE   	902.00	lar accounts amo
4	cornflower chocolate smoke green pink	Manufacturer#3           	Brand#34  	SMALL PLATED BRASS	14	MED DRUM  	904.00	p furiously r
8	misty lace thistle snow royal	Manufacturer#4           	Brand#44  	PROMO BURNISHED TIN	41	LG DRUM   	908.00	eposi
11	spring maroon seashell almond orchid	Manufacturer#2           	Brand#25  	STANDARD BURNISHED NICKEL	43	WRAP BOX  	911.01	ng gr
12	cornflower wheat orange maroon ghost	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED STEEL	25	JUMBO CASE	912.01	 quickly
14	khaki seashell rose cornsilk navajo	Manufacturer#1           	Brand#13  	SMALL POLISHED STEEL	28	JUMBO BOX 	914.01	kages c
19	chocolate navy tan deep brown	Manufacturer#2           	Brand#23  	SMALL ANODIZED NICKEL	33	WRAP BOX  	919.01	 pending acc
20	ivory navy honeydew sandy midnight	Manufacturer#1           	Brand#12  	LARGE POLISHED NICKEL	48	MED BAG   	920.02	are across the asympt
21	lemon floral azure frosted lime	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	31	MED BAG   	921.02	ss packages. pendin
22	medium forest blue ghost black	Manufacturer#4           	Brand#43  	PROMO POLISHED BRASS	19	LG DRUM   	922.02	 even p
23	coral lavender seashell rosy burlywood	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED TIN	42	JUMBO JAR 	923.02	nic, fina
27	saddle puff beige linen yellow	Manufacturer#1           	Brand#14  	LARGE ANODIZED TIN	20	MED PKG   	927.02	s wake. ir
28	navajo yellow drab white misty	Manufacturer#4           	Brand#44  	SMALL PLATED COPPER	19	JUMBO PKG 	928.02	x-ray pending, iron
32	sandy wheat coral spring burnished	Manufacturer#4           	Brand#42  	ECONOMY PLATED BRASS	31	LG CASE   	932.03	urts. carefully fin
33	spring bisque salmon slate pink	Manufacturer#2           	Brand#22  	ECONOMY PLATED NICKEL	16	LG PKG    	933.03	ly eve
36	chiffon tan forest moccasin dark	Manufacturer#2           	Brand#25  	SMALL BURNISHED COPPER	3	JUMBO CAN 	936.03	olites o
37	royal coral orange burnished navajo	Manufacturer#4           	Brand#45  	LARGE POLISHED TIN	48	JUMBO BOX 	937.03	silent 
38	seashell papaya white mint brown	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED BRASS	11	SM JAR    	938.03	structions inte
42	midnight turquoise lawn beige thistle	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED TIN	45	LG BOX    	942.04	the slow
43	medium lace midnight royal chartreuse	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	5	WRAP CASE 	943.04	e slyly along the ir
44	saddle cream wheat lemon burnished	Manufacturer#4           	Brand#45  	MEDIUM PLATED TIN	48	SM PACK   	944.04	pinto beans. carefully
49	light firebrick cyan puff blue	Manufacturer#2           	Brand#24  	SMALL BURNISHED TIN	31	MED DRUM  	949.04	ar pack
2501	cream wheat burlywood spring ivory	Manufacturer#4           	Brand#45  	PROMO BRUSHED STEEL	45	SM PKG    	1403.50	equests haggle car
2502	plum powder steel smoke lemon	Manufacturer#1           	Brand#15  	PROMO POLISHED COPPER	28	SM DRUM   	1404.50	ingly final d
2504	lace magenta royal blanched orchid	Manufacturer#3           	Brand#31  	LARGE BURNISHED BRASS	18	MED JAR   	1406.50	ughout the furiousl
2508	light pale antique wheat lace	Manufacturer#1           	Brand#13  	ECONOMY BRUSHED TIN	44	SM CASE   	1410.50	ng the regular theodol
2511	dodger gainsboro light spring papaya	Manufacturer#1           	Brand#12  	STANDARD POLISHED COPPER	28	WRAP JAR  	1413.51	uickly regular ide
2512	snow cream grey ivory cyan	Manufacturer#4           	Brand#44  	ECONOMY BURNISHED BRASS	27	JUMBO BOX 	1414.51	st furio
2514	royal moccasin chartreuse burlywood purple	Manufacturer#1           	Brand#12  	PROMO POLISHED BRASS	50	JUMBO JAR 	1416.51	even pinto be
2519	powder misty medium indian thistle	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED STEEL	24	SM DRUM   	1421.51	equests 
2520	aquamarine tan salmon metallic grey	Manufacturer#3           	Brand#32  	LARGE ANODIZED TIN	22	LG PKG    	1422.52	 packages caj
2521	black burnished thistle dim peach	Manufacturer#4           	Brand#41  	ECONOMY POLISHED NICKEL	14	WRAP CAN  	1423.52	final accounts nag ca
2522	peru light white black plum	Manufacturer#1           	Brand#14  	MEDIUM POLISHED COPPER	40	SM DRUM   	1424.52	bravely final
2523	bisque coral blue green chartreuse	Manufacturer#2           	Brand#24  	LARGE PLATED STEEL	46	MED BAG   	1425.52	the carefully pendi
2527	ivory peru blanched steel light	Manufacturer#1           	Brand#11  	MEDIUM PLATED STEEL	16	JUMBO PKG 	1429.52	. regular
2528	rose azure turquoise maroon green	Manufacturer#2           	Brand#25  	PROMO BRUSHED STEEL	12	LG BAG    	1430.52	eposits nag fu
2532	floral dark burnished light tomato	Manufacturer#1           	Brand#15  	PROMO BRUSHED COPPER	27	SM BAG    	1434.53	egular, 
2533	seashell orchid green ghost lavender	Manufacturer#3           	Brand#34  	LARGE PLATED COPPER	42	SM CASE   	1435.53	e bli
2536	cream drab salmon forest lawn	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED STEEL	2	JUMBO BOX 	1438.53	ideas. pen
2537	cream rose dim orchid ivory	Manufacturer#4           	Brand#45  	PROMO POLISHED COPPER	20	SM DRUM   	1439.53	ajole qui
2538	snow papaya ivory deep slate	Manufacturer#2           	Brand#22  	MEDIUM POLISHED NICKEL	23	MED PKG   	1440.53	 the e
2542	pale lavender lawn metallic purple	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED BRASS	10	JUMBO JAR 	1444.54	mong the carefully ev
2543	hot rosy midnight mint dodger	Manufacturer#2           	Brand#21  	SMALL BRUSHED TIN	11	SM BAG    	1445.54	ely ironic theodolite
2544	deep ivory white salmon yellow	Manufacturer#3           	Brand#31  	STANDARD BRUSHED BRASS	27	LG CASE   	1446.54	 affix slyly furious
2549	misty sky indian powder bisque	Manufacturer#5           	Brand#52  	PROMO BURNISHED COPPER	17	WRAP CASE 	1451.54	refully even reque
5001	peach seashell grey blush chocolate	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED TIN	21	JUMBO PACK	906.00	c waters. fluffily pe
5002	firebrick thistle cyan rose maroon	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED NICKEL	25	LG CASE   	907.00	ckages sleep i
5004	orchid salmon magenta seashell sky	Manufacturer#2           	Brand#22  	SMALL BRUSHED BRASS	20	WRAP JAR  	909.00	fully r
5008	powder dodger floral cyan almond	Manufacturer#1           	Brand#12  	LARGE ANODIZED TIN	30	JUMBO PACK	913.00	e the fluff
5011	black rosy lemon blue gainsboro	Manufacturer#5           	Brand#53  	PROMO BURNISHED TIN	10	SM JAR    	916.01	fully ironic deposit
5012	drab green honeydew turquoise aquamarine	Manufacturer#4           	Brand#45  	LARGE BRUSHED BRASS	45	LG BOX    	917.01	ts against the 
5014	magenta almond cyan tomato ivory	Manufacturer#4           	Brand#44  	PROMO ANODIZED NICKEL	13	LG PACK   	919.01	. bold accounts cajo
5019	navy floral cornflower sandy moccasin	Manufacturer#5           	Brand#52  	STANDARD POLISHED COPPER	4	MED PACK  	924.01	nts wake slyly du
5020	snow white sky light yellow	Manufacturer#5           	Brand#54  	SMALL BURNISHED TIN	9	SM DRUM   	925.02	 the pending
5021	firebrick lime drab rosy sandy	Manufacturer#2           	Brand#25  	SMALL PLATED COPPER	41	LG BOX    	926.02	sits ki
5022	ivory dim lace cyan misty	Manufacturer#1           	Brand#12  	PROMO POLISHED COPPER	23	MED BOX   	927.02	sts are about th
5023	antique wheat violet thistle green	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED BRASS	11	JUMBO PKG 	928.02	packages detect: f
5027	wheat peru honeydew navajo goldenrod	Manufacturer#1           	Brand#15  	PROMO ANODIZED BRASS	25	JUMBO JAR 	932.02	ending acc
5028	blush dark blue burlywood blanched	Manufacturer#5           	Brand#52  	LARGE BRUSHED STEEL	2	JUMBO BOX 	933.02	inal 
5032	spring white tomato navy rosy	Manufacturer#1           	Brand#15  	SMALL PLATED BRASS	37	LG BAG    	937.03	s. even excuses
5033	lace violet indian mint black	Manufacturer#1           	Brand#13  	MEDIUM BURNISHED TIN	21	JUMBO DRUM	938.03	foxes. slyly bold 
5036	snow navy sky blue maroon	Manufacturer#3           	Brand#33  	LARGE BURNISHED COPPER	34	LG PKG    	941.03	ckly final asymp
5037	slate powder white dark puff	Manufacturer#3           	Brand#34  	SMALL BURNISHED STEEL	39	SM PACK   	942.03	xes. carefull
5038	bisque puff papaya sky dim	Manufacturer#3           	Brand#35  	SMALL BRUSHED BRASS	12	MED BOX   	943.03	 after the furiously
5042	lime snow ivory khaki thistle	Manufacturer#3           	Brand#31  	LARGE POLISHED BRASS	25	SM DRUM   	947.04	 sleep after 
5043	linen gainsboro chocolate white royal	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	29	MED DRUM  	948.04	aggle fluffily even i
5044	dodger tomato thistle seashell mint	Manufacturer#1           	Brand#11  	LARGE PLATED NICKEL	8	LG PACK   	949.04	urious
5049	beige firebrick magenta almond powder	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED STEEL	23	LG PKG    	954.04	deas along 
7501	thistle midnight lemon khaki powder	Manufacturer#2           	Brand#23  	PROMO POLISHED STEEL	50	LG PACK   	1408.50	lly ironic somas impr
7502	rosy peru puff tomato misty	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	29	WRAP BOX  	1409.50	bout 
7504	deep turquoise moccasin peach ghost	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED NICKEL	33	LG PKG    	1411.50	haggle furio
7508	thistle dodger tomato orange navy	Manufacturer#2           	Brand#24  	STANDARD ANODIZED BRASS	42	LG CAN    	1415.50	nto be
7511	chocolate mint turquoise linen chartreuse	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED COPPER	9	MED BOX   	1418.51	counts
7512	white moccasin ghost mint orchid	Manufacturer#2           	Brand#23  	MEDIUM BRUSHED NICKEL	31	LG BOX    	1419.51	e. iron
7514	indian pale sandy moccasin khaki	Manufacturer#5           	Brand#53  	STANDARD BURNISHED TIN	34	JUMBO CASE	1421.51	symptote
7519	firebrick tomato tan thistle white	Manufacturer#5           	Brand#55  	LARGE BRUSHED TIN	44	LG PACK   	1426.51	ts use careful
7520	ivory khaki cornflower royal almond	Manufacturer#5           	Brand#55  	PROMO BURNISHED TIN	35	SM PACK   	1427.52	cial pinto bea
7521	lace sandy yellow purple smoke	Manufacturer#1           	Brand#14  	SMALL ANODIZED COPPER	7	JUMBO CASE	1428.52	 express courts. ca
7522	pale yellow red peru chocolate	Manufacturer#2           	Brand#22  	LARGE POLISHED BRASS	32	JUMBO BOX 	1429.52	al requests. f
7523	floral salmon smoke tomato purple	Manufacturer#1           	Brand#14  	LARGE BURNISHED COPPER	20	JUMBO PACK	1430.52	. fluffily final
7527	antique linen cornsilk goldenrod almond	Manufacturer#3           	Brand#35  	SMALL ANODIZED TIN	19	MED BAG   	1434.52	 after the regu
7528	lawn wheat black olive purple	Manufacturer#2           	Brand#24  	LARGE BRUSHED BRASS	11	MED CAN   	1435.52	 snoo
7532	snow royal burnished peach orchid	Manufacturer#1           	Brand#15  	SMALL ANODIZED STEEL	1	WRAP CAN  	1439.53	d accounts. furiously 
7533	lawn rose light firebrick bisque	Manufacturer#5           	Brand#53  	PROMO BURNISHED BRASS	47	WRAP CASE 	1440.53	pliers detect quic
7536	red burlywood lime mint violet	Manufacturer#3           	Brand#31  	LARGE POLISHED BRASS	18	JUMBO JAR 	1443.53	ged pinto 
7537	peach snow steel puff orange	Manufacturer#3           	Brand#33  	LARGE BURNISHED COPPER	30	SM DRUM   	1444.53	express requests lose
7538	salmon bisque thistle chartreuse sandy	Manufacturer#5           	Brand#53  	STANDARD BURNISHED NICKEL	43	WRAP PACK 	1445.53	re quickly after 
7542	violet lace deep burnished cream	Manufacturer#3           	Brand#35  	ECONOMY BRUSHED NICKEL	1	SM BOX    	1449.54	kly iro
7543	turquoise orchid pink salmon dodger	Manufacturer#5           	Brand#54  	MEDIUM ANODIZED TIN	31	SM PACK   	1450.54	 boost fin
7544	misty deep thistle papaya blush	Manufacturer#3           	Brand#35  	STANDARD ANODIZED TIN	19	JUMBO PKG 	1451.54	r the carefu
7549	peach burnished khaki deep chocolate	Manufacturer#4           	Brand#43  	LARGE BRUSHED STEEL	39	JUMBO PACK	1456.54	ly ideas 
10001	saddle honeydew red plum indian	Manufacturer#4           	Brand#42  	SMALL ANODIZED STEEL	34	SM BAG    	911.00	sly regular theodolite
10002	cyan grey saddle olive pale	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	3	SM PACK   	912.00	sly. slyly 
10004	burlywood dim moccasin forest yellow	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED COPPER	4	MED JAR   	914.00	e along the final p
10008	light lavender lime spring burnished	Manufacturer#2           	Brand#24  	ECONOMY PLATED TIN	16	MED CASE  	918.00	counts. carefully spec
10011	spring lawn saddle drab cyan	Manufacturer#1           	Brand#13  	PROMO ANODIZED NICKEL	32	JUMBO BAG 	921.01	r instructions:
10012	plum chiffon cyan tomato blush	Manufacturer#1           	Brand#14  	ECONOMY PLATED NICKEL	44	WRAP PKG  	922.01	c cour
10014	gainsboro lace red chocolate goldenrod	Manufacturer#1           	Brand#11  	STANDARD PLATED BRASS	19	WRAP CASE 	924.01	 cajole sl
10019	midnight rose medium puff lawn	Manufacturer#3           	Brand#32  	LARGE ANODIZED STEEL	11	SM DRUM   	929.01	accounts wake a
10020	honeydew hot steel indian dodger	Manufacturer#3           	Brand#33  	STANDARD BURNISHED TIN	48	JUMBO BAG 	930.02	packages are ca
10021	blush steel green sienna snow	Manufacturer#5           	Brand#51  	SMALL BRUSHED BRASS	15	LG CASE   	931.02	. fur
10022	lime hot mint lawn white	Manufacturer#1           	Brand#11  	SMALL BRUSHED STEEL	20	LG PKG    	932.02	 requests.
10023	firebrick steel lavender floral dark	Manufacturer#2           	Brand#23  	LARGE PLATED STEEL	28	WRAP PACK 	933.02	nal theodo
10027	lawn yellow dim beige cream	Manufacturer#2           	Brand#23  	SMALL POLISHED STEEL	14	LG DRUM   	937.02	excuses are.
10028	ivory antique floral purple grey	Manufacturer#1           	Brand#13  	MEDIUM POLISHED BRASS	38	MED BAG   	938.02	ess, ev
10032	brown floral light seashell chartreuse	Manufacturer#2           	Brand#21  	SMALL ANODIZED BRASS	25	LG JAR    	942.03	ly ir
10033	firebrick grey pale midnight sky	Manufacturer#3           	Brand#34  	SMALL BURNISHED TIN	29	LG PACK   	943.03	ly regular accounts. 
10036	beige peach honeydew seashell bisque	Manufacturer#5           	Brand#51  	SMALL BRUSHED TIN	50	SM PKG    	946.03	 fluf
10037	red indian salmon smoke purple	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED STEEL	11	JUMBO BAG 	947.03	unts nag 
10038	ivory navajo tomato lace royal	Manufacturer#4           	Brand#45  	MEDIUM PLATED COPPER	37	WRAP CAN  	948.03	ing pea
10042	beige puff midnight antique royal	Manufacturer#3           	Brand#35  	PROMO POLISHED TIN	5	SM BOX    	952.04	lithely accor
10043	linen floral navy burlywood aquamarine	Manufacturer#1           	Brand#13  	STANDARD POLISHED STEEL	23	WRAP JAR  	953.04	its. accounts boos
10044	blue burnished orange frosted khaki	Manufacturer#4           	Brand#44  	STANDARD BURNISHED COPPER	18	WRAP DRUM 	954.04	 regular theodolite
10049	aquamarine cyan metallic lawn lavender	Manufacturer#5           	Brand#55  	PROMO ANODIZED NICKEL	39	LG CASE   	959.04	beans. blith
12498	papaya smoke honeydew white lawn	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED NICKEL	22	SM BOX    	1410.49	o beans. asymptotes
12499	medium blanched coral beige light	Manufacturer#4           	Brand#41  	MEDIUM BURNISHED COPPER	2	LG BAG    	1411.49	frays sleep caref
12501	metallic rosy orchid deep indian	Manufacturer#4           	Brand#45  	SMALL ANODIZED BRASS	37	SM PACK   	1413.50	 sleep c
12505	thistle seashell indian frosted cornflower	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED TIN	4	LG DRUM   	1417.50	hins. accounts affix
12508	green pink lemon aquamarine rose	Manufacturer#5           	Brand#52  	STANDARD POLISHED STEEL	42	SM DRUM   	1420.50	ts haggle. ide
12509	lemon dodger lace tomato black	Manufacturer#1           	Brand#11  	LARGE BRUSHED BRASS	27	JUMBO DRUM	1421.50	ven foxes. furious 
12511	olive lime deep papaya black	Manufacturer#5           	Brand#55  	STANDARD ANODIZED STEEL	8	MED BOX   	1423.51	l packages. slyly
12516	smoke indian steel slate burnished	Manufacturer#5           	Brand#52  	LARGE ANODIZED TIN	45	LG PACK   	1428.51	 requests haggle bli
12517	dodger puff dark yellow dim	Manufacturer#2           	Brand#25  	MEDIUM POLISHED STEEL	14	JUMBO PKG 	1429.51	de of t
12518	cyan coral pale lime red	Manufacturer#3           	Brand#33  	MEDIUM POLISHED STEEL	40	MED PACK  	1430.51	. carefully bold re
12519	peru papaya cornflower lime linen	Manufacturer#4           	Brand#41  	PROMO PLATED STEEL	6	MED BAG   	1431.51	 even ideas. blith
12520	powder dodger chartreuse yellow plum	Manufacturer#2           	Brand#23  	PROMO PLATED BRASS	31	MED PKG   	1432.52	e blithely ir
12524	brown pale seashell ghost magenta	Manufacturer#3           	Brand#33  	MEDIUM POLISHED TIN	43	SM PKG    	1436.52	sits wake blithe
12525	magenta medium antique royal sky	Manufacturer#4           	Brand#43  	LARGE BURNISHED STEEL	21	WRAP JAR  	1437.52	g ins
12529	midnight chartreuse plum goldenrod powder	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED TIN	7	WRAP PKG  	1441.52	ending deposi
12530	firebrick magenta antique blanched papaya	Manufacturer#1           	Brand#14  	STANDARD POLISHED STEEL	17	SM CAN    	1442.53	y. foxe
12533	pale salmon lemon firebrick almond	Manufacturer#4           	Brand#41  	LARGE ANODIZED BRASS	10	SM PACK   	1445.53	 to are slyly r
12534	lime burlywood cornsilk tomato antique	Manufacturer#5           	Brand#51  	PROMO PLATED TIN	19	MED PKG   	1446.53	 the car
12535	thistle blanched floral black salmon	Manufacturer#1           	Brand#14  	LARGE PLATED TIN	16	JUMBO BAG 	1447.53	he car
12539	tan sandy indian navy ghost	Manufacturer#4           	Brand#41  	LARGE POLISHED NICKEL	39	WRAP BAG  	1451.53	s cajole di
12540	orchid burlywood navajo indian yellow	Manufacturer#4           	Brand#41  	ECONOMY POLISHED NICKEL	15	MED DRUM  	1452.54	s detect 
12541	bisque blush burnished lawn navajo	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED BRASS	12	JUMBO CASE	1453.54	sits m
12546	antique bisque dark burlywood magenta	Manufacturer#2           	Brand#21  	LARGE BURNISHED STEEL	29	LG DRUM   	1458.54	 are. blit
14999	aquamarine sky dark dim brown	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED TIN	27	JUMBO PKG 	1913.99	g after the
15000	snow navy pale drab cornsilk	Manufacturer#2           	Brand#22  	SMALL BRUSHED STEEL	24	MED CAN   	915.00	 regular requests 
15002	blanched ghost cream azure tomato	Manufacturer#5           	Brand#55  	LARGE BURNISHED STEEL	26	MED CAN   	917.00	waters along the
15006	azure bisque olive pink chiffon	Manufacturer#2           	Brand#22  	PROMO BRUSHED STEEL	11	LG BAG    	921.00	arefully 
15009	medium black moccasin smoke coral	Manufacturer#4           	Brand#44  	STANDARD PLATED NICKEL	46	SM CASE   	924.00	ut the sometimes sp
15010	sandy moccasin seashell steel cornflower	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED TIN	19	WRAP BAG  	925.01	d stealthily
15012	almond lavender orange honeydew violet	Manufacturer#3           	Brand#34  	PROMO BRUSHED STEEL	32	JUMBO BOX 	927.01	ironic foxes de
15017	slate blue rose midnight steel	Manufacturer#1           	Brand#14  	STANDARD BRUSHED STEEL	35	MED BAG   	932.01	mong the fu
15018	violet blue deep pale tan	Manufacturer#5           	Brand#53  	ECONOMY PLATED COPPER	1	LG PKG    	933.01	cies sleep blithe
15019	plum green salmon sienna ivory	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED BRASS	22	MED DRUM  	934.01	carefully. carefully e
15020	honeydew hot gainsboro black brown	Manufacturer#5           	Brand#52  	PROMO ANODIZED NICKEL	6	JUMBO CASE	935.02	ironic 
15021	dim sienna powder indian gainsboro	Manufacturer#2           	Brand#24  	SMALL POLISHED NICKEL	36	SM PACK   	936.02	ajole blithely express
15025	gainsboro rose purple spring navy	Manufacturer#3           	Brand#35  	SMALL ANODIZED STEEL	19	LG BOX    	940.02	courts poach ca
15026	navajo pale navy lawn dodger	Manufacturer#3           	Brand#34  	LARGE BURNISHED COPPER	36	MED CAN   	941.02	slyly ironic a
15030	rosy antique forest orchid royal	Manufacturer#3           	Brand#31  	PROMO BRUSHED TIN	12	JUMBO BAG 	945.03	iously
15031	blue spring plum peach orange	Manufacturer#2           	Brand#25  	PROMO PLATED NICKEL	49	WRAP JAR  	946.03	 sauternes enga
15034	ivory maroon indian brown lavender	Manufacturer#2           	Brand#23  	LARGE ANODIZED STEEL	23	JUMBO BAG 	949.03	the packages cajole
15035	sandy almond sienna sky drab	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED TIN	32	JUMBO BOX 	950.03	se above the 
15036	purple khaki lime gainsboro beige	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED TIN	26	MED PKG   	951.03	hely regular
15040	burnished chiffon thistle blush mint	Manufacturer#2           	Brand#25  	PROMO PLATED NICKEL	45	LG BOX    	955.04	ironic th
15041	pale forest azure sienna steel	Manufacturer#5           	Brand#54  	LARGE BRUSHED NICKEL	10	SM JAR    	956.04	 deposits detect s
15042	rosy snow coral blue grey	Manufacturer#1           	Brand#12  	MEDIUM PLATED TIN	44	LG JAR    	957.04	dependencies. reg
15047	drab grey medium midnight red	Manufacturer#5           	Brand#51  	LARGE POLISHED COPPER	1	LG CAN    	962.04	ar packages
17500	ivory lemon burnished lime red	Manufacturer#1           	Brand#12  	PROMO POLISHED NICKEL	28	JUMBO CASE	1417.50	wake pen
17501	snow blanched lawn yellow hot	Manufacturer#5           	Brand#51  	SMALL BRUSHED COPPER	45	WRAP BOX  	1418.50	l accou
17503	magenta blanched ghost bisque lemon	Manufacturer#5           	Brand#54  	MEDIUM ANODIZED COPPER	6	SM PACK   	1420.50	ronic instructions na
17507	thistle spring orange khaki magenta	Manufacturer#2           	Brand#21  	SMALL BURNISHED NICKEL	50	JUMBO BAG 	1424.50	ng blithely. thin, f
17510	pink cyan green blanched smoke	Manufacturer#4           	Brand#42  	STANDARD ANODIZED NICKEL	43	MED CASE  	1427.51	 promise slyly 
17511	sandy magenta midnight gainsboro rose	Manufacturer#4           	Brand#41  	LARGE BRUSHED COPPER	31	WRAP CAN  	1428.51	olite
17513	snow magenta tomato burlywood lime	Manufacturer#4           	Brand#45  	STANDARD POLISHED COPPER	45	SM BAG    	1430.51	es solve slyly. furio
17518	burnished tan rose violet seashell	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED NICKEL	30	MED DRUM  	1435.51	riously eve
17519	light blue cornflower chocolate cyan	Manufacturer#4           	Brand#44  	PROMO POLISHED BRASS	21	WRAP CASE 	1436.51	 of the express pinto
17520	blue tan hot smoke violet	Manufacturer#1           	Brand#15  	MEDIUM PLATED TIN	14	LG PKG    	1437.52	nt, ironic
17521	sienna forest bisque snow slate	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED COPPER	18	SM JAR    	1438.52	 dolphins. blithel
17522	azure tomato blue tan pale	Manufacturer#3           	Brand#35  	LARGE ANODIZED BRASS	46	SM PKG    	1439.52	platelets nag 
17526	chiffon wheat red medium almond	Manufacturer#2           	Brand#23  	PROMO BRUSHED COPPER	41	MED JAR   	1443.52	s boost permanently 
17527	pale lace chiffon hot rosy	Manufacturer#4           	Brand#42  	SMALL BRUSHED COPPER	39	JUMBO PACK	1444.52	s sleep 
17531	rose sky lavender burnished medium	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED BRASS	41	SM CAN    	1448.53	arefully e
17532	burnished ghost dim antique maroon	Manufacturer#5           	Brand#54  	LARGE BURNISHED NICKEL	13	SM PACK   	1449.53	kly regul
17535	slate violet tan turquoise forest	Manufacturer#2           	Brand#24  	STANDARD PLATED BRASS	46	WRAP PACK 	1452.53	ffily; pending, regula
17536	tan wheat gainsboro moccasin steel	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED NICKEL	45	WRAP DRUM 	1453.53	gular deposi
17537	powder white peru cornflower papaya	Manufacturer#1           	Brand#12  	STANDARD POLISHED COPPER	48	SM PACK   	1454.53	r requests sleep
17541	beige ivory misty cornflower lace	Manufacturer#5           	Brand#53  	SMALL PLATED NICKEL	30	WRAP BOX  	1458.54	ally carefully bol
17542	drab thistle magenta seashell cornsilk	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED STEEL	20	SM JAR    	1459.54	ges nag about
17543	chocolate orchid forest lavender almond	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED COPPER	17	SM PACK   	1460.54	theodolites. sly
17548	thistle navy peru antique rosy	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED TIN	44	WRAP JAR  	1465.54	 cajole slyly 
20001	metallic maroon tomato turquoise grey	Manufacturer#3           	Brand#31  	PROMO POLISHED NICKEL	2	JUMBO PKG 	921.00	press instructions wa
20002	blanched firebrick royal purple chartreuse	Manufacturer#4           	Brand#44  	PROMO BURNISHED STEEL	5	MED BOX   	922.00	 depos
20004	drab chiffon salmon violet lavender	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED COPPER	9	WRAP CASE 	924.00	. slyly exp
20008	salmon burlywood frosted grey drab	Manufacturer#2           	Brand#21  	STANDARD ANODIZED NICKEL	14	SM JAR    	928.00	ithely final 
20011	smoke sandy peach honeydew pale	Manufacturer#1           	Brand#13  	MEDIUM BURNISHED NICKEL	23	LG CAN    	931.01	l deposits use
20012	royal aquamarine grey olive tan	Manufacturer#4           	Brand#41  	MEDIUM PLATED COPPER	26	MED BAG   	932.01	s haggle fluff
20014	navy floral sienna drab blanched	Manufacturer#3           	Brand#32  	LARGE BURNISHED COPPER	11	JUMBO CASE	934.01	 deposits doubt quic
20019	rose lavender blue orchid gainsboro	Manufacturer#5           	Brand#51  	MEDIUM POLISHED BRASS	1	WRAP JAR  	939.01	nts haggle above the
20020	green azure sienna thistle almond	Manufacturer#4           	Brand#43  	MEDIUM PLATED TIN	7	JUMBO JAR 	940.02	to bea
20021	azure chiffon lime royal ghost	Manufacturer#1           	Brand#14  	ECONOMY PLATED BRASS	2	LG JAR    	941.02	 ironi
20022	drab salmon turquoise frosted ghost	Manufacturer#4           	Brand#42  	PROMO ANODIZED COPPER	7	LG PACK   	942.02	cajole be
20023	thistle steel tomato grey metallic	Manufacturer#2           	Brand#21  	LARGE POLISHED TIN	48	WRAP JAR  	943.02	ely. furiously 
20027	dodger midnight drab beige cornflower	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED COPPER	4	JUMBO JAR 	947.02	er the final platel
20028	drab cyan olive sienna seashell	Manufacturer#2           	Brand#21  	ECONOMY BRUSHED NICKEL	21	JUMBO CAN 	948.02	y. furiously 
20032	saddle slate midnight cornsilk dim	Manufacturer#1           	Brand#11  	STANDARD BRUSHED STEEL	22	SM CASE   	952.03	riously. c
20033	goldenrod floral lavender red coral	Manufacturer#1           	Brand#11  	LARGE ANODIZED BRASS	15	MED PKG   	953.03	y express ide
20036	medium frosted magenta burlywood blue	Manufacturer#4           	Brand#42  	ECONOMY POLISHED COPPER	34	SM JAR    	956.03	gular packages cajole
20037	purple dim blanched olive medium	Manufacturer#3           	Brand#33  	PROMO PLATED TIN	8	SM PACK   	957.03	dolites boost fur
20038	bisque hot deep black turquoise	Manufacturer#1           	Brand#12  	PROMO BURNISHED TIN	34	LG PKG    	958.03	ully f
20042	saddle salmon powder cornsilk linen	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED TIN	45	SM BAG    	962.04	lyly carefull
20043	peach dark lemon cornsilk salmon	Manufacturer#5           	Brand#53  	STANDARD BURNISHED BRASS	24	SM CASE   	963.04	 packages. fu
20044	linen lemon dodger almond gainsboro	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED NICKEL	46	LG CASE   	964.04	lithely. r
20049	lemon spring olive cornsilk cornflower	Manufacturer#3           	Brand#34  	SMALL BURNISHED STEEL	4	WRAP CAN  	969.04	arefully
22495	lavender purple beige violet snow	Manufacturer#1           	Brand#14  	SMALL BURNISHED NICKEL	37	LG PACK   	1417.49	above the fu
22496	cornflower dim black peach blanched	Manufacturer#2           	Brand#23  	PROMO POLISHED NICKEL	20	MED CASE  	1418.49	. regular excuses 
22498	plum almond sky burnished light	Manufacturer#1           	Brand#13  	STANDARD POLISHED STEEL	15	SM BAG    	1420.49	jole carefull
22502	chocolate bisque dim violet moccasin	Manufacturer#4           	Brand#43  	LARGE BURNISHED TIN	30	LG JAR    	1424.50	gs. slyly fluff
22505	thistle turquoise navajo puff cyan	Manufacturer#4           	Brand#43  	PROMO PLATED BRASS	9	LG BAG    	1427.50	 reques
22506	lace drab cream dodger coral	Manufacturer#2           	Brand#22  	LARGE PLATED STEEL	39	JUMBO CASE	1428.50	. req
22508	light purple floral snow red	Manufacturer#1           	Brand#14  	ECONOMY PLATED STEEL	15	LG PACK   	1430.50	 carefully accoun
22513	lawn puff antique drab rose	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED STEEL	14	MED BOX   	1435.51	ly express de
22514	honeydew cornsilk metallic blanched olive	Manufacturer#4           	Brand#45  	STANDARD BURNISHED TIN	28	SM CAN    	1436.51	y regular req
22515	dim sienna chocolate wheat white	Manufacturer#1           	Brand#13  	SMALL BRUSHED NICKEL	44	WRAP BAG  	1437.51	 silen
22516	metallic peru peach black magenta	Manufacturer#2           	Brand#25  	SMALL POLISHED COPPER	33	JUMBO PKG 	1438.51	ccording t
22517	spring yellow medium tan sandy	Manufacturer#5           	Brand#53  	PROMO BURNISHED COPPER	8	WRAP PACK 	1439.51	c depende
22521	violet gainsboro blanched almond purple	Manufacturer#4           	Brand#41  	LARGE BURNISHED COPPER	30	MED BOX   	1443.52	instru
22522	plum red chartreuse dodger tan	Manufacturer#5           	Brand#55  	LARGE BURNISHED NICKEL	11	MED CAN   	1444.52	ly even dependencies.
22526	pink tomato chiffon indian deep	Manufacturer#2           	Brand#22  	MEDIUM POLISHED BRASS	42	WRAP CASE 	1448.52	 deposits grow car
22527	sienna drab salmon gainsboro azure	Manufacturer#1           	Brand#15  	PROMO POLISHED COPPER	9	MED CAN   	1449.52	 slyly unus
22530	light midnight antique chartreuse rosy	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED STEEL	13	SM PKG    	1452.53	ndencies 
22531	tan midnight pale puff khaki	Manufacturer#5           	Brand#53  	LARGE BRUSHED STEEL	6	WRAP BAG  	1453.53	lar asympto
22532	bisque antique purple sandy dim	Manufacturer#2           	Brand#23  	LARGE BURNISHED NICKEL	4	JUMBO JAR 	1454.53	 pinto beans 
22536	cyan snow plum aquamarine seashell	Manufacturer#4           	Brand#43  	PROMO POLISHED STEEL	40	JUMBO CAN 	1458.53	grate s
22537	papaya plum chiffon blush navy	Manufacturer#2           	Brand#22  	STANDARD POLISHED TIN	38	MED DRUM  	1459.53	g the fluffily rut
22538	white aquamarine indian hot frosted	Manufacturer#5           	Brand#55  	ECONOMY POLISHED NICKEL	38	SM CAN    	1460.53	ts. u
22543	dark violet red rosy sienna	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	41	LG PKG    	1465.54	cally special cour
24997	magenta orchid lawn lemon lace	Manufacturer#1           	Brand#13  	LARGE PLATED TIN	10	JUMBO CAN 	1921.99	fluffily ironic fox
24998	slate ghost wheat sky cornsilk	Manufacturer#2           	Brand#21  	ECONOMY PLATED COPPER	13	JUMBO BAG 	1922.99	egular, regular pint
25000	ghost purple yellow sandy chocolate	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED COPPER	17	WRAP JAR  	925.00	final ac
25004	deep goldenrod cream medium blue	Manufacturer#5           	Brand#51  	PROMO POLISHED COPPER	25	MED PACK  	929.00	ubt furiously depos
25007	midnight forest salmon purple rosy	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED BRASS	13	LG PACK   	932.00	express d
25008	lawn mint cornsilk burnished pink	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED BRASS	17	MED CASE  	933.00	lly ironic 
25010	aquamarine forest dark metallic yellow	Manufacturer#5           	Brand#55  	MEDIUM PLATED NICKEL	6	LG PKG    	935.01	special packages. blit
25015	linen metallic lawn goldenrod purple	Manufacturer#3           	Brand#35  	MEDIUM PLATED STEEL	25	SM JAR    	940.01	dolit
25016	peach blanched lime navy green	Manufacturer#2           	Brand#24  	PROMO PLATED BRASS	19	SM BOX    	941.01	final dolphins 
25017	mint purple chiffon misty aquamarine	Manufacturer#2           	Brand#23  	LARGE PLATED TIN	45	JUMBO JAR 	942.01	ackages! sl
25018	black grey metallic ivory firebrick	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED TIN	14	MED PKG   	943.01	es. b
25019	ivory orange deep forest burlywood	Manufacturer#2           	Brand#22  	LARGE BRUSHED STEEL	28	SM BAG    	944.01	counts. pinto be
25023	cyan olive dark turquoise metallic	Manufacturer#4           	Brand#45  	ECONOMY PLATED BRASS	19	WRAP DRUM 	948.02	blithely. do
25024	firebrick blanched orchid cornsilk moccasin	Manufacturer#3           	Brand#34  	STANDARD ANODIZED COPPER	11	LG PACK   	949.02	tealthi
25028	cream yellow drab tan goldenrod	Manufacturer#2           	Brand#23  	SMALL BURNISHED COPPER	19	WRAP PKG  	953.02	nd the package
25029	rose frosted floral pink seashell	Manufacturer#5           	Brand#53  	LARGE BURNISHED COPPER	15	WRAP BAG  	954.02	special request
25032	honeydew beige green orchid salmon	Manufacturer#1           	Brand#14  	LARGE ANODIZED BRASS	10	MED DRUM  	957.03	ag along the iro
25033	firebrick red green steel aquamarine	Manufacturer#2           	Brand#24  	LARGE PLATED NICKEL	26	JUMBO PACK	958.03	. blithely even the
25034	blue royal maroon black snow	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED BRASS	12	WRAP PACK 	959.03	ording to the furi
25038	black tan spring azure moccasin	Manufacturer#3           	Brand#32  	STANDARD PLATED NICKEL	37	MED CAN   	963.03	endencie
25039	blush pale aquamarine seashell cream	Manufacturer#1           	Brand#13  	STANDARD BURNISHED NICKEL	26	WRAP PACK 	964.03	 cajole f
25040	tan indian azure aquamarine navajo	Manufacturer#3           	Brand#33  	MEDIUM POLISHED COPPER	47	MED CASE  	965.04	uffil
25045	black grey blue smoke plum	Manufacturer#5           	Brand#51  	PROMO ANODIZED COPPER	5	JUMBO DRUM	970.04	 theodolites across t
27499	cornflower thistle sandy lemon forest	Manufacturer#2           	Brand#25  	MEDIUM POLISHED STEEL	16	JUMBO PACK	1426.49	ly across the 
27500	tan frosted white burnished royal	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED TIN	8	JUMBO CAN 	1427.50	 final
27502	coral black dodger lemon blush	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED COPPER	15	LG CASE   	1429.50	c accounts. pending 
27506	steel papaya firebrick seashell lime	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED NICKEL	39	MED DRUM  	1433.50	beans. quickly silen
27509	midnight lavender green chartreuse thistle	Manufacturer#4           	Brand#42  	SMALL BURNISHED NICKEL	35	MED CASE  	1436.50	xpress notornis. fur
27510	linen peach coral papaya rose	Manufacturer#1           	Brand#13  	PROMO PLATED BRASS	26	MED DRUM  	1437.51	oss the ironi
27512	gainsboro misty almond pale bisque	Manufacturer#4           	Brand#45  	SMALL PLATED BRASS	44	JUMBO PACK	1439.51	requests detect slyly 
27517	smoke maroon almond mint turquoise	Manufacturer#5           	Brand#53  	PROMO POLISHED BRASS	36	JUMBO CAN 	1444.51	ch daringl
27518	orange lavender magenta snow saddle	Manufacturer#4           	Brand#44  	STANDARD BRUSHED COPPER	10	MED BOX   	1445.51	thely among the blit
27519	hot goldenrod black lemon chiffon	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	48	LG CASE   	1446.51	press dep
27520	floral slate royal sandy red	Manufacturer#3           	Brand#34  	PROMO POLISHED NICKEL	8	SM CASE   	1447.52	ages wak
27521	medium chocolate floral bisque burnished	Manufacturer#3           	Brand#32  	ECONOMY BURNISHED STEEL	36	MED DRUM  	1448.52	ial ideas are. 
27525	white wheat honeydew forest powder	Manufacturer#2           	Brand#22  	STANDARD BRUSHED TIN	13	MED PACK  	1452.52	 packages are slyly
27526	dim blanched lime metallic mint	Manufacturer#5           	Brand#52  	STANDARD BURNISHED BRASS	4	MED PACK  	1453.52	arefully s
27530	red lavender lemon almond linen	Manufacturer#1           	Brand#12  	SMALL BURNISHED TIN	1	JUMBO JAR 	1457.53	yly express id
27531	lace cream firebrick brown papaya	Manufacturer#4           	Brand#45  	ECONOMY PLATED BRASS	49	LG PKG    	1458.53	kly regular dep
27534	frosted dodger white puff plum	Manufacturer#5           	Brand#55  	SMALL PLATED STEEL	17	WRAP PACK 	1461.53	s nag quickl
27535	thistle smoke plum wheat firebrick	Manufacturer#1           	Brand#14  	SMALL POLISHED NICKEL	24	LG JAR    	1462.53	ven excuses
27536	grey lawn thistle sandy floral	Manufacturer#1           	Brand#12  	LARGE BRUSHED NICKEL	10	SM PACK   	1463.53	eposi
27540	pale plum hot magenta red	Manufacturer#5           	Brand#52  	ECONOMY PLATED TIN	35	MED JAR   	1467.54	n pin
27541	beige violet cornflower saddle indian	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED STEEL	24	SM DRUM   	1468.54	s. carefully bol
27542	cornsilk powder blush peach azure	Manufacturer#3           	Brand#34  	STANDARD ANODIZED TIN	27	JUMBO PKG 	1469.54	zle furi
27547	deep floral snow honeydew sienna	Manufacturer#1           	Brand#12  	ECONOMY PLATED BRASS	49	LG BOX    	1474.54	r instructions! ir
30001	hot firebrick gainsboro goldenrod chartreuse	Manufacturer#1           	Brand#14  	ECONOMY BURNISHED NICKEL	43	WRAP DRUM 	931.00	ickly at the care
30002	burlywood blue brown mint ghost	Manufacturer#1           	Brand#14  	STANDARD ANODIZED TIN	48	SM DRUM   	932.00	ickly pen
30004	powder papaya linen azure cornsilk	Manufacturer#3           	Brand#35  	MEDIUM POLISHED NICKEL	36	WRAP JAR  	934.00	nts against the
30008	almond forest mint midnight violet	Manufacturer#1           	Brand#14  	SMALL PLATED NICKEL	40	WRAP BOX  	938.00	y final packages
30011	honeydew navy ivory firebrick smoke	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED BRASS	30	SM PKG    	941.01	lets haggle careful
30012	sky cream cornflower magenta mint	Manufacturer#2           	Brand#22  	SMALL BURNISHED COPPER	13	WRAP CAN  	942.01	equests boost
30014	cream lemon olive salmon puff	Manufacturer#1           	Brand#13  	LARGE POLISHED TIN	6	MED BAG   	944.01	eans are fi
30019	gainsboro blush tan cream frosted	Manufacturer#5           	Brand#55  	SMALL ANODIZED TIN	20	MED DRUM  	949.01	ickly pend
30020	midnight khaki aquamarine tomato saddle	Manufacturer#4           	Brand#43  	LARGE ANODIZED BRASS	2	WRAP DRUM 	950.02	ously. slyly silent d
30021	purple slate sandy thistle maroon	Manufacturer#5           	Brand#55  	LARGE ANODIZED TIN	33	JUMBO JAR 	951.02	 carefully pending the
30022	mint orchid goldenrod chocolate cyan	Manufacturer#3           	Brand#32  	SMALL ANODIZED STEEL	5	JUMBO CAN 	952.02	l theodolites ha
30023	orange dodger orchid azure smoke	Manufacturer#1           	Brand#14  	STANDARD POLISHED BRASS	28	LG BOX    	953.02	n asymptotes. s
30027	snow seashell black slate blanched	Manufacturer#1           	Brand#13  	ECONOMY ANODIZED BRASS	4	SM BAG    	957.02	ructions cajo
30028	purple gainsboro chartreuse floral smoke	Manufacturer#1           	Brand#14  	ECONOMY PLATED COPPER	25	WRAP BOX  	958.02	dependencies mold q
30032	plum light mint steel blue	Manufacturer#3           	Brand#35  	SMALL BRUSHED TIN	7	WRAP PKG  	962.03	nts w
30033	drab cream midnight brown dark	Manufacturer#2           	Brand#25  	MEDIUM POLISHED TIN	6	SM JAR    	963.03	 courts sublate c
30036	purple burlywood navajo ghost cream	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	33	JUMBO BOX 	966.03	jole blithely fl
30037	tomato puff white ghost royal	Manufacturer#3           	Brand#33  	ECONOMY PLATED BRASS	47	WRAP JAR  	967.03	 furiously final dep
30038	lavender sienna snow tomato dodger	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED NICKEL	7	LG BAG    	968.03	ic ideas wake carefu
30042	firebrick dark frosted maroon beige	Manufacturer#2           	Brand#23  	LARGE POLISHED TIN	9	JUMBO PKG 	972.04	silently bol
30043	turquoise purple lace tan medium	Manufacturer#5           	Brand#55  	SMALL BRUSHED BRASS	19	JUMBO PKG 	973.04	iously ironic
30044	green olive white blush brown	Manufacturer#2           	Brand#22  	STANDARD PLATED BRASS	9	WRAP PACK 	974.04	bold ideas bet
30049	navy indian cyan steel papaya	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED COPPER	40	MED DRUM  	979.04	efully f
32492	tan lawn goldenrod chiffon medium	Manufacturer#3           	Brand#34  	STANDARD POLISHED NICKEL	36	LG DRUM   	1424.49	ely. bli
32493	pink steel hot forest drab	Manufacturer#1           	Brand#12  	STANDARD ANODIZED TIN	4	WRAP CAN  	1425.49	lyly even 
32495	pink sandy orchid cornflower peach	Manufacturer#4           	Brand#45  	SMALL ANODIZED STEEL	11	LG PKG    	1427.49	uests boost slyly 
32499	seashell white blue black wheat	Manufacturer#3           	Brand#33  	PROMO POLISHED COPPER	45	SM CAN    	1431.49	ggle care
32502	goldenrod puff spring chocolate red	Manufacturer#5           	Brand#53  	LARGE POLISHED COPPER	41	SM PKG    	1434.50	ironi
32503	rose khaki sienna turquoise burnished	Manufacturer#5           	Brand#52  	ECONOMY PLATED COPPER	14	SM PACK   	1435.50	ructions wa
32505	midnight slate olive steel mint	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED TIN	34	JUMBO CASE	1437.50	ly. carefully re
32510	green saddle dim spring cornflower	Manufacturer#3           	Brand#31  	PROMO ANODIZED TIN	24	SM PACK   	1442.51	special requests sl
32511	gainsboro medium dodger lawn sienna	Manufacturer#1           	Brand#15  	ECONOMY POLISHED NICKEL	11	JUMBO JAR 	1443.51	ithely regular no
32512	chartreuse cyan salmon cornflower linen	Manufacturer#4           	Brand#45  	MEDIUM ANODIZED COPPER	17	JUMBO DRUM	1444.51	ake flu
32513	sky misty wheat drab indian	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED NICKEL	16	JUMBO DRUM	1445.51	y slyly slow
32514	gainsboro honeydew lawn navy salmon	Manufacturer#4           	Brand#45  	LARGE PLATED NICKEL	1	SM CAN    	1446.51	n, pend
32518	spring goldenrod deep firebrick blue	Manufacturer#5           	Brand#52  	MEDIUM POLISHED STEEL	15	SM CAN    	1450.51	eposits are along t
32519	blush cornsilk yellow brown lavender	Manufacturer#4           	Brand#42  	LARGE BRUSHED NICKEL	34	MED PACK  	1451.51	 blithely regul
32523	grey chartreuse snow ghost blush	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED BRASS	25	SM JAR    	1455.52	l instructions nag
32524	misty sky black chocolate lawn	Manufacturer#4           	Brand#43  	STANDARD PLATED NICKEL	24	LG PACK   	1456.52	furiously. furious
32527	honeydew sandy purple powder cream	Manufacturer#3           	Brand#32  	STANDARD PLATED NICKEL	24	LG DRUM   	1459.52	y pending
32528	almond rose royal plum black	Manufacturer#1           	Brand#11  	ECONOMY PLATED TIN	19	MED DRUM  	1460.52	le quickly. furi
32529	olive almond chartreuse ghost blue	Manufacturer#4           	Brand#45  	MEDIUM ANODIZED NICKEL	45	SM JAR    	1461.52	ckly above the
32533	puff grey black misty dark	Manufacturer#1           	Brand#14  	STANDARD BRUSHED TIN	31	WRAP PKG  	1465.53	nstructions.
32534	plum drab beige honeydew tan	Manufacturer#5           	Brand#53  	STANDARD PLATED STEEL	17	MED PKG   	1466.53	hely ironic deposit
32535	white misty brown bisque lavender	Manufacturer#5           	Brand#53  	SMALL POLISHED BRASS	27	MED CAN   	1467.53	ges af
32540	drab sandy cyan red sky	Manufacturer#1           	Brand#12  	ECONOMY BURNISHED STEEL	47	SM JAR    	1472.54	kages among the b
34995	snow black linen frosted lawn	Manufacturer#3           	Brand#35  	PROMO ANODIZED TIN	13	MED DRUM  	1929.99	 at the account
34996	moccasin antique puff seashell blue	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	39	LG BOX    	1930.99	 across 
34998	lemon frosted magenta mint rosy	Manufacturer#3           	Brand#34  	STANDARD ANODIZED NICKEL	31	MED BAG   	1932.99	ourts wake blithely 
35002	sky blanched orchid lavender bisque	Manufacturer#4           	Brand#43  	SMALL BRUSHED STEEL	43	LG BAG    	937.00	le fluffily: slyly 
35005	blue misty medium dim indian	Manufacturer#4           	Brand#41  	PROMO BURNISHED NICKEL	35	LG PKG    	940.00	ic pack
35006	cornsilk goldenrod coral chartreuse blanched	Manufacturer#3           	Brand#32  	SMALL ANODIZED STEEL	21	WRAP PACK 	941.00	yly unusual in
35008	metallic puff thistle medium burlywood	Manufacturer#5           	Brand#55  	SMALL BRUSHED TIN	37	MED CASE  	943.00	lent foxes wake 
35013	white burnished snow puff orchid	Manufacturer#5           	Brand#51  	LARGE ANODIZED COPPER	36	JUMBO PACK	948.01	lar pinto bean
35014	black olive blanched deep white	Manufacturer#5           	Brand#51  	ECONOMY POLISHED STEEL	40	JUMBO CAN 	949.01	ong the regular acco
35015	chartreuse sienna maroon black frosted	Manufacturer#3           	Brand#34  	STANDARD BRUSHED NICKEL	4	JUMBO BOX 	950.01	ly aga
35016	blue deep midnight dodger ghost	Manufacturer#1           	Brand#12  	SMALL PLATED NICKEL	26	LG DRUM   	951.01	about th
35017	sandy chartreuse linen slate seashell	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	39	MED BOX   	952.01	gular
35021	antique misty light green azure	Manufacturer#1           	Brand#14  	LARGE BURNISHED COPPER	46	MED PKG   	956.02	ven requests ac
35022	spring yellow ivory rosy pink	Manufacturer#4           	Brand#42  	STANDARD POLISHED TIN	35	SM CASE   	957.02	s account
35026	wheat puff hot lime slate	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED STEEL	31	WRAP PACK 	961.02	nst the ideas. speci
35027	lawn thistle chocolate moccasin pink	Manufacturer#3           	Brand#32  	SMALL POLISHED STEEL	27	MED PKG   	962.02	t fluffily ironic 
35030	coral blanched maroon medium floral	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED COPPER	6	SM DRUM   	965.03	kages lose
35031	misty firebrick rosy cream burnished	Manufacturer#1           	Brand#11  	PROMO BURNISHED BRASS	8	MED CASE  	966.03	metimes ironic req
35032	cream rosy deep azure saddle	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED COPPER	49	LG CASE   	967.03	nts might sleep slyly
35036	deep lime sandy cornsilk metallic	Manufacturer#1           	Brand#12  	STANDARD PLATED BRASS	18	LG BAG    	971.03	es haggle
35037	frosted light seashell brown linen	Manufacturer#5           	Brand#54  	MEDIUM BRUSHED TIN	19	WRAP DRUM 	972.03	 theodolites sle
35038	moccasin violet rosy magenta dark	Manufacturer#1           	Brand#11  	LARGE POLISHED STEEL	42	LG BOX    	973.03	ess deposits. c
35043	dodger ghost orange lavender khaki	Manufacturer#3           	Brand#33  	LARGE PLATED STEEL	3	SM BOX    	978.04	into 
37498	rose burlywood gainsboro midnight linen	Manufacturer#4           	Brand#45  	LARGE BURNISHED TIN	37	MED JAR   	1435.49	 fluffy deposi
37499	orchid maroon metallic red sandy	Manufacturer#5           	Brand#54  	LARGE ANODIZED COPPER	41	MED DRUM  	1436.49	packages. spec
37501	papaya navajo lemon hot blush	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED BRASS	9	WRAP CASE 	1438.50	sits. bl
37505	steel floral tomato burnished chartreuse	Manufacturer#5           	Brand#51  	PROMO ANODIZED TIN	2	MED BOX   	1442.50	he slyly
37508	black steel maroon dodger linen	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED BRASS	37	LG CASE   	1445.50	atelets caj
37509	saddle pale drab honeydew magenta	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED COPPER	8	WRAP PACK 	1446.50	posits cajo
37511	midnight steel metallic lemon blanched	Manufacturer#4           	Brand#44  	ECONOMY POLISHED COPPER	41	WRAP CASE 	1448.51	ously regular ideas
37516	powder midnight lace salmon dodger	Manufacturer#2           	Brand#25  	LARGE PLATED TIN	39	SM BOX    	1453.51	 ironic packages wake
37517	pale magenta metallic aquamarine blush	Manufacturer#5           	Brand#53  	SMALL PLATED NICKEL	11	SM DRUM   	1454.51	uriously i
37518	peach lemon saddle frosted indian	Manufacturer#5           	Brand#54  	STANDARD BRUSHED BRASS	43	WRAP CASE 	1455.51	ss packag
37519	maroon chiffon orchid chocolate blanched	Manufacturer#3           	Brand#32  	STANDARD ANODIZED BRASS	46	SM CAN    	1456.51	patterns haggl
37520	forest seashell burlywood dodger mint	Manufacturer#1           	Brand#11  	ECONOMY BRUSHED BRASS	48	WRAP PACK 	1457.52	usual packages a
37524	lemon navajo forest firebrick navy	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	16	WRAP JAR  	1461.52	pecial requests caj
37525	gainsboro almond orchid khaki spring	Manufacturer#1           	Brand#13  	PROMO BRUSHED COPPER	34	SM BOX    	1462.52	ly pending accounts
37529	blush burlywood dark saddle pale	Manufacturer#4           	Brand#44  	LARGE ANODIZED BRASS	28	LG BOX    	1466.52	e of the unusual idea
37530	lemon cyan green papaya tomato	Manufacturer#5           	Brand#52  	PROMO PLATED BRASS	35	MED DRUM  	1467.53	es. blithely express
37533	powder sienna peach azure smoke	Manufacturer#2           	Brand#25  	MEDIUM PLATED STEEL	43	JUMBO PKG 	1470.53	requests sleep 
37534	puff indian lemon green cornsilk	Manufacturer#5           	Brand#55  	STANDARD BRUSHED NICKEL	33	JUMBO CASE	1471.53	 regular ideas. furio
37535	metallic rosy navajo yellow firebrick	Manufacturer#3           	Brand#33  	STANDARD BRUSHED NICKEL	20	WRAP CASE 	1472.53	 slyly regul
37539	orange tomato navajo smoke peach	Manufacturer#5           	Brand#52  	STANDARD BRUSHED COPPER	21	SM CASE   	1476.53	efully fur
37540	violet honeydew slate peach metallic	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED BRASS	6	WRAP PACK 	1477.54	quests accordin
37541	red linen burnished navajo floral	Manufacturer#5           	Brand#52  	ECONOMY PLATED BRASS	12	MED PKG   	1478.54	es along the request
37546	honeydew aquamarine white almond cornflower	Manufacturer#5           	Brand#53  	STANDARD BURNISHED COPPER	27	SM BOX    	1483.54	as wake dependencies.
40001	cream steel purple royal goldenrod	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED COPPER	27	JUMBO PKG 	941.00	lly slow depende
40002	almond cyan forest midnight khaki	Manufacturer#3           	Brand#34  	ECONOMY POLISHED TIN	32	WRAP BOX  	942.00	ng to 
40004	midnight magenta metallic drab bisque	Manufacturer#3           	Brand#34  	LARGE BRUSHED COPPER	29	LG BOX    	944.00	uthlessly al
40008	navy red puff lawn olive	Manufacturer#3           	Brand#32  	SMALL BRUSHED BRASS	41	WRAP BAG  	948.00	 carefully fina
40011	lace orchid mint cornflower peru	Manufacturer#4           	Brand#41  	LARGE BURNISHED TIN	3	MED PKG   	951.01	boost slyly carefull
40012	linen bisque magenta rosy mint	Manufacturer#5           	Brand#55  	ECONOMY POLISHED NICKEL	48	LG DRUM   	952.01	ironic dep
40014	violet lace papaya ghost cornsilk	Manufacturer#5           	Brand#54  	SMALL PLATED COPPER	1	MED CAN   	954.01	le blithely blithel
40019	cornsilk burlywood medium hot orange	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED STEEL	20	SM PKG    	959.01	sts. fluffily expre
40020	almond peru bisque lavender seashell	Manufacturer#1           	Brand#12  	SMALL POLISHED COPPER	34	LG DRUM   	960.02	. ironic pin
40021	almond dim blush orchid lawn	Manufacturer#4           	Brand#41  	LARGE BRUSHED BRASS	19	LG DRUM   	961.02	regular ideas are
40022	powder blue metallic coral aquamarine	Manufacturer#5           	Brand#52  	MEDIUM POLISHED COPPER	24	MED CAN   	962.02	 afte
40023	frosted orange coral orchid royal	Manufacturer#5           	Brand#53  	PROMO BURNISHED NICKEL	33	SM PKG    	963.02	odolites. request
40027	misty turquoise burnished blanched salmon	Manufacturer#5           	Brand#54  	SMALL BURNISHED STEEL	42	MED BOX   	967.02	usly unusual a
40028	khaki papaya beige royal red	Manufacturer#3           	Brand#34  	SMALL BURNISHED TIN	35	JUMBO PKG 	968.02	sly final
40032	maroon hot burlywood beige powder	Manufacturer#5           	Brand#52  	PROMO BRUSHED BRASS	50	JUMBO PKG 	972.03	s mold. 
40033	cornsilk cream firebrick plum almond	Manufacturer#1           	Brand#13  	PROMO BURNISHED STEEL	31	WRAP BAG  	973.03	egular excuses
40036	blanched mint salmon seashell chocolate	Manufacturer#1           	Brand#15  	LARGE PLATED BRASS	9	JUMBO BAG 	976.03	special excuses. regu
40037	dodger gainsboro black papaya slate	Manufacturer#2           	Brand#23  	LARGE PLATED TIN	26	SM PKG    	977.03	ts. final acco
40038	seashell cream plum chartreuse smoke	Manufacturer#3           	Brand#35  	MEDIUM POLISHED BRASS	19	JUMBO JAR 	978.03	efully even ac
40042	saddle rose brown orange tomato	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED BRASS	10	JUMBO PACK	982.04	ithes along the 
40043	beige thistle plum peru yellow	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	32	JUMBO CASE	983.04	 even theodolites. re
40044	maroon deep burnished olive chocolate	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED STEEL	7	MED JAR   	984.04	 wake quickly. fin
40049	goldenrod rose chartreuse rosy peru	Manufacturer#2           	Brand#23  	LARGE PLATED NICKEL	6	WRAP PKG  	989.04	e regularly. pe
42489	dim chiffon cream lawn steel	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED COPPER	5	MED PACK  	1431.48	ully furious
42490	aquamarine magenta indian frosted goldenrod	Manufacturer#1           	Brand#15  	LARGE POLISHED BRASS	29	WRAP BAG  	1432.49	egula
42492	midnight dodger olive sky sandy	Manufacturer#4           	Brand#43  	STANDARD ANODIZED NICKEL	15	WRAP BAG  	1434.49	 cajol
42496	blanched misty linen maroon pale	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	38	LG BAG    	1438.49	ns. blithely pen
42499	pale bisque yellow goldenrod medium	Manufacturer#1           	Brand#12  	LARGE ANODIZED TIN	13	WRAP DRUM 	1441.49	y special deposits
42500	tan rose mint grey ivory	Manufacturer#2           	Brand#22  	PROMO BRUSHED COPPER	30	MED JAR   	1442.50	e qui
42502	aquamarine lime steel azure goldenrod	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED STEEL	47	WRAP DRUM 	1444.50	 regular warthogs. bli
42507	goldenrod sandy turquoise burnished lace	Manufacturer#4           	Brand#43  	SMALL BRUSHED BRASS	20	WRAP JAR  	1449.50	ly regular packag
42508	salmon red dodger sky snow	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED BRASS	44	LG CASE   	1450.50	kages sn
42509	chocolate slate cornsilk orange drab	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED BRASS	2	WRAP DRUM 	1451.50	cial ideas a
42510	sienna linen snow red magenta	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED COPPER	2	SM JAR    	1452.51	e around the 
42511	deep black orange royal cornsilk	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED NICKEL	10	JUMBO DRUM	1453.51	 are quickly ac
42515	seashell indian coral beige light	Manufacturer#2           	Brand#25  	MEDIUM BURNISHED TIN	9	WRAP CASE 	1457.51	are ca
42516	papaya deep violet olive aquamarine	Manufacturer#1           	Brand#15  	LARGE ANODIZED BRASS	21	LG JAR    	1458.51	refully final acco
42520	chiffon azure honeydew white mint	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED STEEL	30	MED BAG   	1462.52	lites around the
42521	brown deep royal wheat smoke	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED BRASS	31	WRAP PKG  	1463.52	even 
42524	lavender cornsilk blush hot navy	Manufacturer#2           	Brand#24  	MEDIUM POLISHED STEEL	49	MED CASE  	1466.52	l requ
42525	azure blanched papaya chiffon coral	Manufacturer#1           	Brand#14  	PROMO BURNISHED STEEL	27	LG JAR    	1467.52	furiously. flu
42526	cyan puff sky snow chiffon	Manufacturer#4           	Brand#41  	PROMO PLATED STEEL	29	WRAP JAR  	1468.52	ic, care
42530	light sky orange sienna powder	Manufacturer#4           	Brand#41  	ECONOMY PLATED NICKEL	50	WRAP CAN  	1472.53	fluffily unusu
42531	sandy cream smoke steel orange	Manufacturer#1           	Brand#11  	ECONOMY PLATED COPPER	3	LG CAN    	1473.53	ns. final d
42532	frosted azure misty drab midnight	Manufacturer#2           	Brand#21  	PROMO PLATED NICKEL	25	SM PACK   	1474.53	c depe
42537	drab chocolate hot lawn peru	Manufacturer#4           	Brand#42  	STANDARD POLISHED STEEL	11	LG CASE   	1479.53	g carefully. slyl
44993	cornflower wheat thistle floral navy	Manufacturer#1           	Brand#15  	SMALL ANODIZED NICKEL	30	JUMBO DRUM	1937.99	eans cajole fluffily 
44994	navy blue chiffon lemon sandy	Manufacturer#1           	Brand#15  	SMALL BURNISHED TIN	8	LG BAG    	1938.99	ic foxe
44996	deep papaya black rose powder	Manufacturer#5           	Brand#55  	MEDIUM BRUSHED BRASS	50	SM JAR    	1940.99	hely iron
45000	cornflower grey midnight magenta black	Manufacturer#1           	Brand#11  	STANDARD BURNISHED COPPER	48	WRAP BAG  	945.00	sits.
45003	goldenrod deep light violet coral	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED BRASS	22	SM CAN    	948.00	ve the blithely regul
45004	azure black frosted dodger purple	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED BRASS	4	SM BAG    	949.00	 carefully iron
45006	turquoise cyan khaki white navy	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED NICKEL	14	LG DRUM   	951.00	sts a
45011	gainsboro drab spring indian azure	Manufacturer#1           	Brand#14  	LARGE PLATED NICKEL	10	SM CASE   	956.01	lar packages about
45012	rose papaya olive almond firebrick	Manufacturer#3           	Brand#34  	LARGE ANODIZED NICKEL	47	JUMBO JAR 	957.01	tes. slyly
45013	purple drab magenta almond puff	Manufacturer#2           	Brand#21  	STANDARD ANODIZED STEEL	49	WRAP BOX  	958.01	. foxes amo
45014	steel honeydew slate puff black	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED TIN	6	JUMBO PKG 	959.01	ding accounts. ca
45015	azure slate navajo turquoise dim	Manufacturer#4           	Brand#43  	MEDIUM PLATED STEEL	33	JUMBO CAN 	960.01	p. expr
45019	turquoise aquamarine spring moccasin yellow	Manufacturer#4           	Brand#41  	SMALL BURNISHED TIN	36	WRAP BAG  	964.01	c packages wake sly
45020	drab navy cornflower plum sienna	Manufacturer#2           	Brand#24  	SMALL POLISHED STEEL	40	WRAP PKG  	965.02	y acro
45024	maroon midnight honeydew saddle white	Manufacturer#4           	Brand#41  	SMALL BRUSHED STEEL	27	SM PACK   	969.02	equests wake acros
45025	medium powder pale chiffon light	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED TIN	12	LG JAR    	970.02	r packages sle
45028	chocolate navy dodger salmon red	Manufacturer#1           	Brand#13  	STANDARD ANODIZED NICKEL	30	SM CASE   	973.02	 final p
45029	orchid burlywood peru powder dark	Manufacturer#3           	Brand#34  	STANDARD POLISHED COPPER	50	WRAP PKG  	974.02	nts cajole furious
45030	saddle tan bisque chocolate hot	Manufacturer#2           	Brand#22  	MEDIUM PLATED BRASS	32	MED PACK  	975.03	theodolites w
45034	orange firebrick light coral plum	Manufacturer#5           	Brand#53  	PROMO POLISHED TIN	42	MED BOX   	979.03	 requests boost f
45035	beige red antique powder peru	Manufacturer#2           	Brand#24  	STANDARD BRUSHED NICKEL	36	LG PACK   	980.03	 above the fluffily fl
45036	mint papaya yellow metallic ivory	Manufacturer#3           	Brand#32  	SMALL BURNISHED STEEL	47	MED BAG   	981.03	c pinto beans. final 
45041	rosy dim black pale aquamarine	Manufacturer#5           	Brand#53  	STANDARD PLATED COPPER	21	WRAP CAN  	986.04	-- furiou
47497	firebrick pink goldenrod grey pale	Manufacturer#1           	Brand#15  	PROMO BURNISHED NICKEL	18	JUMBO CASE	1444.49	 carefully bold pl
47498	chocolate blush saddle cornflower hot	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	19	LG PKG    	1445.49	y. spe
47500	coral cream orchid firebrick sandy	Manufacturer#3           	Brand#32  	PROMO ANODIZED TIN	48	SM JAR    	1447.50	lar dependencies. fl
47504	orange lawn snow medium lace	Manufacturer#1           	Brand#11  	LARGE BURNISHED BRASS	22	JUMBO DRUM	1451.50	ss deposits. s
47507	red tan brown cornsilk drab	Manufacturer#5           	Brand#51  	LARGE ANODIZED STEEL	16	WRAP PKG  	1454.50	leep. silen
47508	mint medium lemon slate orange	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	37	SM CAN    	1455.50	lar instructions s
47510	slate orchid azure turquoise floral	Manufacturer#2           	Brand#23  	ECONOMY PLATED NICKEL	43	JUMBO CAN 	1457.51	nal deposits poach 
47515	misty coral firebrick peach turquoise	Manufacturer#2           	Brand#22  	ECONOMY POLISHED TIN	49	JUMBO CAN 	1462.51	 silently regular
47516	navajo slate goldenrod midnight tomato	Manufacturer#2           	Brand#25  	STANDARD BRUSHED STEEL	21	WRAP CAN  	1463.51	eposits alongsid
47517	bisque indian mint light rose	Manufacturer#4           	Brand#41  	SMALL BURNISHED STEEL	32	LG PACK   	1464.51	 furiously after the 
47518	puff dodger white turquoise burnished	Manufacturer#3           	Brand#35  	SMALL ANODIZED BRASS	48	WRAP CASE 	1465.51	refully fin
47519	grey medium cornflower frosted yellow	Manufacturer#4           	Brand#41  	SMALL BURNISHED STEEL	42	JUMBO PACK	1466.51	nusual pa
47523	blue cream seashell thistle peach	Manufacturer#3           	Brand#32  	SMALL BURNISHED TIN	19	MED DRUM  	1470.52	ross the furiously
47524	medium violet olive moccasin peach	Manufacturer#5           	Brand#53  	LARGE ANODIZED STEEL	31	MED BAG   	1471.52	. fluffily fi
47528	midnight goldenrod lace lemon rosy	Manufacturer#4           	Brand#43  	MEDIUM PLATED COPPER	3	JUMBO PACK	1475.52	the furious
47529	violet lime blue navy almond	Manufacturer#5           	Brand#54  	MEDIUM BRUSHED STEEL	27	LG PKG    	1476.52	ular ideas. p
47532	almond frosted drab powder indian	Manufacturer#1           	Brand#12  	PROMO PLATED TIN	7	JUMBO CAN 	1479.53	le careful
47533	cream blush slate orchid firebrick	Manufacturer#5           	Brand#55  	ECONOMY POLISHED NICKEL	38	JUMBO BAG 	1480.53	eas. furiously
47534	cornsilk aquamarine powder cyan chiffon	Manufacturer#3           	Brand#33  	LARGE POLISHED COPPER	47	LG DRUM   	1481.53	e fluffily regular fox
47538	magenta slate drab orange wheat	Manufacturer#4           	Brand#42  	MEDIUM POLISHED NICKEL	48	WRAP CAN  	1485.53	sly aft
47539	maroon white sky khaki blanched	Manufacturer#2           	Brand#23  	LARGE ANODIZED NICKEL	18	JUMBO PACK	1486.53	jole. furiously iron
47540	pink lavender cornsilk ivory pale	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED TIN	8	LG PKG    	1487.54	ey players. slyly 
47545	moccasin bisque burlywood frosted purple	Manufacturer#3           	Brand#32  	SMALL POLISHED COPPER	29	LG CAN    	1492.54	furiou
50001	drab burnished lavender maroon cornflower	Manufacturer#5           	Brand#51  	ECONOMY POLISHED STEEL	39	JUMBO PKG 	951.00	luffily unusual the
50002	lawn dim firebrick smoke snow	Manufacturer#2           	Brand#25  	PROMO ANODIZED BRASS	41	JUMBO JAR 	952.00	e furiously 
50004	saddle black maroon magenta dodger	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED BRASS	21	MED CAN   	954.00	s. unusual
50008	aquamarine brown blue sienna coral	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED COPPER	20	MED BOX   	958.00	ven pinto
50011	cornflower chiffon puff cyan thistle	Manufacturer#3           	Brand#31  	STANDARD BURNISHED COPPER	12	LG JAR    	961.01	ietly p
50012	misty magenta wheat orange ivory	Manufacturer#3           	Brand#31  	STANDARD BRUSHED BRASS	33	LG DRUM   	962.01	mptotes grow sly
50014	tomato orchid sienna linen hot	Manufacturer#2           	Brand#23  	MEDIUM POLISHED COPPER	20	LG BAG    	964.01	g fluffily thinly bold
50019	medium dim chocolate black lavender	Manufacturer#4           	Brand#43  	PROMO PLATED COPPER	42	WRAP BAG  	969.01	y silent de
50020	powder magenta lavender olive tan	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED COPPER	23	WRAP BAG  	970.02	ss the
50021	sandy firebrick aquamarine pale yellow	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED STEEL	13	WRAP PACK 	971.02	lithely quick
50022	dodger mint lemon chiffon pale	Manufacturer#2           	Brand#23  	PROMO PLATED COPPER	32	LG BAG    	972.02	as sleep i
50023	linen spring saddle mint yellow	Manufacturer#5           	Brand#53  	MEDIUM PLATED NICKEL	29	MED BOX   	973.02	gular asymptotes. 
50027	red plum lavender lemon lime	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED NICKEL	27	SM JAR    	977.02	beans. pending
50028	dodger beige honeydew slate dim	Manufacturer#2           	Brand#24  	LARGE POLISHED COPPER	28	MED BOX   	978.02	 bravely unus
50032	metallic peach thistle purple medium	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED NICKEL	48	WRAP CASE 	982.03	doggedl
50033	tomato peach beige burnished yellow	Manufacturer#2           	Brand#23  	MEDIUM BURNISHED TIN	44	SM PKG    	983.03	, bold requests ha
50036	purple cornsilk dark navy black	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED COPPER	42	MED JAR   	986.03	blithely alongsi
50037	almond misty sky grey hot	Manufacturer#4           	Brand#41  	PROMO POLISHED TIN	47	SM PKG    	987.03	es: express, r
50038	peru rosy chartreuse tan medium	Manufacturer#2           	Brand#22  	ECONOMY PLATED BRASS	39	MED DRUM  	988.03	o the f
50042	violet white olive saddle black	Manufacturer#2           	Brand#23  	PROMO POLISHED TIN	49	LG CAN    	992.04	 the 
50043	mint antique deep metallic powder	Manufacturer#3           	Brand#31  	MEDIUM POLISHED STEEL	17	LG PKG    	993.04	ckly ironic requests s
50044	ivory medium tomato chiffon thistle	Manufacturer#5           	Brand#55  	STANDARD BURNISHED NICKEL	22	LG BOX    	994.04	structi
50049	beige almond royal red mint	Manufacturer#2           	Brand#22  	PROMO ANODIZED NICKEL	4	LG JAR    	999.04	ans. fluffily bold re
52486	mint metallic floral pale turquoise	Manufacturer#4           	Brand#45  	ECONOMY POLISHED NICKEL	11	JUMBO CASE	1438.48	ggle against the bus
52487	red thistle blanched aquamarine metallic	Manufacturer#2           	Brand#22  	ECONOMY POLISHED TIN	6	LG PKG    	1439.48	ias. special theodoli
52489	turquoise navy bisque papaya peach	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	6	JUMBO PKG 	1441.48	atelets. furiou
52493	deep khaki peach orchid sandy	Manufacturer#1           	Brand#11  	STANDARD ANODIZED NICKEL	31	LG PACK   	1445.49	oxes wake fu
52496	white indian cream ghost bisque	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	23	WRAP PKG  	1448.49	y fluffily spe
52497	black pale azure thistle white	Manufacturer#3           	Brand#33  	MEDIUM BRUSHED BRASS	21	MED DRUM  	1449.49	uriously 
52499	papaya brown orchid navajo lace	Manufacturer#1           	Brand#11  	MEDIUM ANODIZED TIN	31	WRAP PKG  	1451.49	y fina
52504	steel peru olive sienna magenta	Manufacturer#4           	Brand#44  	SMALL POLISHED STEEL	24	WRAP BAG  	1456.50	y across the f
52505	aquamarine cyan drab azure indian	Manufacturer#1           	Brand#13  	MEDIUM BURNISHED COPPER	48	LG BOX    	1457.50	ests boost furiously t
52506	light cream white cornflower violet	Manufacturer#3           	Brand#35  	LARGE ANODIZED STEEL	50	SM JAR    	1458.50	 according to the 
52507	cyan lime cornflower midnight medium	Manufacturer#5           	Brand#55  	PROMO ANODIZED TIN	41	WRAP CAN  	1459.50	. carefu
52508	purple pale seashell smoke metallic	Manufacturer#5           	Brand#55  	PROMO ANODIZED NICKEL	32	JUMBO CASE	1460.50	 bold accounts
52512	lace lime pale almond chartreuse	Manufacturer#2           	Brand#24  	LARGE POLISHED COPPER	9	WRAP PACK 	1464.51	sits. ironic, ironic p
52513	ivory violet burlywood seashell blanched	Manufacturer#1           	Brand#13  	STANDARD BRUSHED TIN	1	LG CASE   	1465.51	uickly fi
52517	magenta dark cream seashell blue	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED NICKEL	2	WRAP BAG  	1469.51	ilent reque
52518	lime mint coral peach olive	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED COPPER	13	WRAP CASE 	1470.51	ronic accounts
52521	thistle forest blanched grey magenta	Manufacturer#5           	Brand#53  	STANDARD PLATED NICKEL	31	MED CASE  	1473.52	pinto beans nag
52522	smoke plum magenta antique steel	Manufacturer#3           	Brand#33  	LARGE POLISHED COPPER	39	WRAP CAN  	1474.52	after the pendi
52523	chartreuse slate midnight wheat floral	Manufacturer#2           	Brand#21  	LARGE BRUSHED NICKEL	24	WRAP CASE 	1475.52	deas. pending request
52527	red steel turquoise ghost linen	Manufacturer#3           	Brand#35  	LARGE BURNISHED BRASS	15	MED DRUM  	1479.52	the ir
52528	purple salmon peach firebrick frosted	Manufacturer#3           	Brand#35  	SMALL PLATED TIN	27	SM CAN    	1480.52	ought to wake ab
52529	wheat hot grey sandy chocolate	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED BRASS	48	WRAP JAR  	1481.52	 requests serve 
52534	tan purple chiffon ivory lawn	Manufacturer#4           	Brand#41  	MEDIUM POLISHED STEEL	11	SM JAR    	1486.53	unts. sile
54991	lavender lemon pink orchid olive	Manufacturer#5           	Brand#54  	SMALL BRUSHED COPPER	31	WRAP PKG  	1945.99	ts integra
54992	rose bisque magenta turquoise floral	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED COPPER	32	SM CAN    	1946.99	idly ironi
54994	snow burlywood smoke salmon orange	Manufacturer#3           	Brand#32  	STANDARD BURNISHED NICKEL	6	LG PKG    	1948.99	ng deposits. speci
54998	lawn metallic orange pink grey	Manufacturer#5           	Brand#55  	LARGE BRUSHED STEEL	40	LG PKG    	1952.99	al instructions
55001	floral aquamarine chiffon light white	Manufacturer#1           	Brand#12  	LARGE PLATED BRASS	43	WRAP PKG  	956.00	sits nag ca
55002	blue steel coral ivory chartreuse	Manufacturer#3           	Brand#32  	STANDARD BRUSHED STEEL	1	LG BOX    	957.00	ckages haggle flu
55004	blanched almond sienna steel gainsboro	Manufacturer#4           	Brand#42  	STANDARD POLISHED TIN	11	LG BOX    	959.00	 haggle furi
55009	burlywood linen grey azure yellow	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED COPPER	44	SM BOX    	964.00	ependencies. q
55010	light burnished blanched drab almond	Manufacturer#2           	Brand#22  	SMALL PLATED STEEL	16	SM PKG    	965.01	ly pending foxes. c
55011	sky smoke burnished cornsilk maroon	Manufacturer#3           	Brand#32  	SMALL BRUSHED STEEL	27	WRAP BOX  	966.01	nding i
55012	dim rose grey bisque blue	Manufacturer#4           	Brand#45  	MEDIUM PLATED COPPER	4	JUMBO PKG 	967.01	ffy packages integrat
55013	chiffon midnight firebrick maroon khaki	Manufacturer#4           	Brand#41  	SMALL PLATED NICKEL	38	WRAP PACK 	968.01	ing i
55017	chartreuse dark beige cream peru	Manufacturer#3           	Brand#33  	PROMO PLATED NICKEL	11	JUMBO PKG 	972.01	carefull
55018	pink wheat chiffon deep khaki	Manufacturer#3           	Brand#33  	STANDARD BURNISHED NICKEL	43	JUMBO JAR 	973.01	press gifts sleep s
55022	navajo puff cream chiffon lavender	Manufacturer#4           	Brand#41  	ECONOMY PLATED STEEL	5	SM DRUM   	977.02	en ideas. even, 
55023	beige purple lace maroon light	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED BRASS	17	LG JAR    	978.02	kly darin
55026	black blanched plum forest indian	Manufacturer#1           	Brand#13  	PROMO PLATED BRASS	2	MED CASE  	981.02	es. slyly un
55027	steel lawn floral medium brown	Manufacturer#4           	Brand#45  	LARGE BRUSHED BRASS	9	WRAP JAR  	982.02	; slyly final foxes
55028	honeydew tan almond violet deep	Manufacturer#4           	Brand#41  	LARGE PLATED NICKEL	18	SM BAG    	983.02	e fluffy packa
55032	black linen slate burlywood tomato	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED BRASS	12	MED DRUM  	987.03	gular theodoli
55033	sandy snow metallic lace powder	Manufacturer#2           	Brand#23  	STANDARD PLATED TIN	18	JUMBO PACK	988.03	 place of
55034	goldenrod blanched linen khaki deep	Manufacturer#1           	Brand#12  	PROMO BURNISHED NICKEL	37	JUMBO PACK	989.03	l depos
55039	hot bisque wheat azure deep	Manufacturer#3           	Brand#34  	PROMO BRUSHED STEEL	33	WRAP JAR  	994.03	 regular de
57496	khaki goldenrod slate seashell salmon	Manufacturer#1           	Brand#12  	LARGE PLATED NICKEL	24	LG CASE   	1453.49	unts s
57497	goldenrod khaki sandy rosy thistle	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED NICKEL	43	SM PACK   	1454.49	eques
57499	maroon smoke honeydew brown sienna	Manufacturer#4           	Brand#44  	SMALL BRUSHED STEEL	32	SM PACK   	1456.49	hely regular accoun
57503	goldenrod floral spring orchid powder	Manufacturer#4           	Brand#45  	PROMO ANODIZED BRASS	18	JUMBO JAR 	1460.50	tions cajole 
57506	saddle navajo steel dim lavender	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED STEEL	18	JUMBO JAR 	1463.50	across the ironi
57507	thistle snow puff sandy midnight	Manufacturer#2           	Brand#21  	STANDARD BRUSHED COPPER	39	SM JAR    	1464.50	ithely ironic deposi
57509	orange turquoise ghost black cream	Manufacturer#1           	Brand#11  	PROMO POLISHED TIN	8	MED DRUM  	1466.50	carefully s
57514	peru turquoise frosted pale pink	Manufacturer#1           	Brand#12  	PROMO POLISHED COPPER	21	LG CASE   	1471.51	ckages atop the pl
57515	khaki misty maroon linen midnight	Manufacturer#2           	Brand#23  	STANDARD BURNISHED BRASS	25	WRAP PACK 	1472.51	 theodolites 
57516	almond peru lace moccasin lavender	Manufacturer#4           	Brand#43  	STANDARD POLISHED STEEL	32	SM JAR    	1473.51	pecial depo
57517	violet bisque turquoise chocolate spring	Manufacturer#1           	Brand#11  	STANDARD BURNISHED TIN	49	SM CASE   	1474.51	eep; even, d
57518	pink tan white steel navajo	Manufacturer#2           	Brand#23  	PROMO BRUSHED STEEL	13	LG DRUM   	1475.51	requests snooze. i
57522	grey saddle papaya goldenrod chiffon	Manufacturer#3           	Brand#34  	MEDIUM POLISHED BRASS	30	LG BAG    	1479.52	e furio
57523	mint coral azure thistle sky	Manufacturer#4           	Brand#41  	SMALL POLISHED COPPER	24	LG PACK   	1480.52	iers are
57527	ghost drab maroon forest floral	Manufacturer#2           	Brand#24  	SMALL BRUSHED TIN	4	MED BAG   	1484.52	 furiously regu
57528	salmon moccasin smoke white grey	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED STEEL	49	JUMBO CAN 	1485.52	sts cajole slyly aga
57531	chartreuse antique pale salmon metallic	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	14	WRAP DRUM 	1488.53	ly ironic foxes. ir
57532	grey lime lemon sky cornsilk	Manufacturer#3           	Brand#31  	SMALL PLATED STEEL	21	SM CASE   	1489.53	es hinder q
57533	powder floral aquamarine white dodger	Manufacturer#3           	Brand#34  	MEDIUM BRUSHED COPPER	47	LG CAN    	1490.53	ending ac
57537	almond hot rose yellow grey	Manufacturer#2           	Brand#25  	PROMO BRUSHED NICKEL	49	LG JAR    	1494.53	 after 
57538	chiffon violet thistle blanched indian	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED TIN	17	SM BAG    	1495.53	s. regula
57539	medium burnished azure chiffon cyan	Manufacturer#4           	Brand#42  	STANDARD BURNISHED BRASS	9	WRAP BOX  	1496.53	ructi
57544	blanched violet sky tomato linen	Manufacturer#1           	Brand#15  	SMALL ANODIZED TIN	39	JUMBO BOX 	1501.54	 blithe
60001	firebrick hot rose aquamarine navy	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED COPPER	34	SM PKG    	961.00	ss accounts
60002	azure blanched khaki dark aquamarine	Manufacturer#3           	Brand#35  	LARGE BRUSHED STEEL	13	JUMBO BAG 	962.00	ely express 
60004	lace metallic yellow goldenrod bisque	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED BRASS	7	SM PKG    	964.00	above
60008	blue cornflower tomato slate yellow	Manufacturer#4           	Brand#45  	PROMO BRUSHED NICKEL	48	MED DRUM  	968.00	he quickly even 
60011	chocolate lawn burnished steel magenta	Manufacturer#2           	Brand#22  	ECONOMY PLATED TIN	22	JUMBO BOX 	971.01	sts. unus
60012	tomato light linen forest orange	Manufacturer#5           	Brand#52  	MEDIUM POLISHED TIN	32	JUMBO CAN 	972.01	unts? furiously iro
60014	lace snow mint purple pink	Manufacturer#1           	Brand#12  	PROMO POLISHED NICKEL	12	LG PACK   	974.01	times quickly iro
60019	puff turquoise chiffon pale rosy	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED TIN	12	WRAP CAN  	979.01	l theodolites sle
60020	grey burlywood lace honeydew pink	Manufacturer#1           	Brand#12  	ECONOMY BRUSHED STEEL	19	SM DRUM   	980.02	to the fluffily pendi
60021	frosted lace metallic mint medium	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	30	SM JAR    	981.02	uld have to
60022	ghost sienna lawn royal cornsilk	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED BRASS	36	MED BAG   	982.02	ing, special ac
60023	medium tan beige lime forest	Manufacturer#2           	Brand#24  	SMALL BURNISHED TIN	5	JUMBO DRUM	983.02	ding deposits sleep c
60027	azure mint slate sandy lace	Manufacturer#2           	Brand#24  	PROMO BURNISHED BRASS	39	SM BAG    	987.02	ealth
60028	dark lace antique navy blush	Manufacturer#4           	Brand#41  	STANDARD BRUSHED TIN	17	JUMBO JAR 	988.02	cial theodolites haggl
60032	sky rose papaya forest cornsilk	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED NICKEL	14	MED DRUM  	992.03	ffily. quickly 
60033	navajo snow floral lawn indian	Manufacturer#2           	Brand#25  	PROMO ANODIZED COPPER	4	WRAP JAR  	993.03	ole. instructi
60036	peach honeydew rose royal hot	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED COPPER	30	JUMBO CAN 	996.03	ly even ideas u
60037	coral purple papaya violet lemon	Manufacturer#3           	Brand#34  	LARGE BURNISHED COPPER	9	SM CASE   	997.03	s. regular i
60038	chiffon grey wheat spring navy	Manufacturer#2           	Brand#22  	LARGE POLISHED TIN	5	JUMBO PACK	998.03	bove the regular dep
60042	purple goldenrod misty honeydew olive	Manufacturer#4           	Brand#44  	MEDIUM POLISHED BRASS	37	LG BAG    	1002.04	e blithely above 
60043	turquoise seashell steel misty cream	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	3	WRAP PKG  	1003.04	 against the pending 
60044	maroon smoke sienna blue lavender	Manufacturer#3           	Brand#33  	ECONOMY PLATED BRASS	42	SM CAN    	1004.04	he even, bold
60049	lavender blanched sandy olive wheat	Manufacturer#2           	Brand#21  	PROMO POLISHED STEEL	25	JUMBO CAN 	1009.04	ts. blithely un
62483	beige puff firebrick papaya grey	Manufacturer#3           	Brand#31  	STANDARD POLISHED BRASS	25	WRAP DRUM 	1445.48	 packages
62484	burlywood indian royal ivory metallic	Manufacturer#2           	Brand#25  	PROMO BURNISHED BRASS	32	WRAP JAR  	1446.48	 furiously silent acco
62486	wheat brown salmon deep lace	Manufacturer#4           	Brand#45  	LARGE ANODIZED NICKEL	2	SM PKG    	1448.48	s. quickly silent
62490	puff white bisque saddle linen	Manufacturer#5           	Brand#52  	SMALL BURNISHED STEEL	28	JUMBO JAR 	1452.49	kages cajole 
62493	chocolate papaya burlywood chartreuse turquoise	Manufacturer#3           	Brand#35  	LARGE BRUSHED STEEL	11	LG CASE   	1455.49	nically express deposi
62494	pink bisque lemon orchid chartreuse	Manufacturer#3           	Brand#34  	LARGE POLISHED NICKEL	3	SM PKG    	1456.49	inal pinto beans. fin
62496	moccasin goldenrod blanched deep cream	Manufacturer#1           	Brand#14  	LARGE BURNISHED STEEL	25	MED PACK  	1458.49	ly regular deposit
62501	antique orange azure honeydew orchid	Manufacturer#3           	Brand#34  	ECONOMY POLISHED NICKEL	23	WRAP DRUM 	1463.50	above the express, 
62502	turquoise olive aquamarine cyan metallic	Manufacturer#5           	Brand#53  	SMALL ANODIZED BRASS	31	SM PKG    	1464.50	osits affix un
62503	magenta lemon floral red gainsboro	Manufacturer#5           	Brand#51  	PROMO BURNISHED TIN	36	JUMBO CAN 	1465.50	efully silent 
62504	steel lemon lawn orange linen	Manufacturer#5           	Brand#52  	LARGE POLISHED NICKEL	30	LG BAG    	1466.50	eposits
62505	turquoise peru indian blue ivory	Manufacturer#2           	Brand#22  	LARGE ANODIZED STEEL	12	LG DRUM   	1467.50	as. bli
62509	burnished lime chiffon indian maroon	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED STEEL	30	JUMBO CAN 	1471.50	 about the fluffil
62510	peach forest ivory floral ghost	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED TIN	30	SM BOX    	1472.51	r the regular deposi
62514	burnished yellow tomato honeydew royal	Manufacturer#3           	Brand#35  	STANDARD ANODIZED STEEL	46	SM JAR    	1476.51	furio
62515	cornflower lime chocolate royal blue	Manufacturer#3           	Brand#31  	SMALL BURNISHED COPPER	13	MED BOX   	1477.51	heodolites. ca
62518	purple medium mint forest khaki	Manufacturer#3           	Brand#33  	STANDARD POLISHED COPPER	8	MED DRUM  	1480.51	beans. carefully sp
62519	chartreuse purple grey ivory tan	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED NICKEL	36	JUMBO JAR 	1481.51	ly according to t
62520	cornsilk floral red beige ghost	Manufacturer#2           	Brand#24  	SMALL PLATED STEEL	9	SM JAR    	1482.52	ly even braids
62524	pale blue light navajo blush	Manufacturer#2           	Brand#22  	SMALL ANODIZED COPPER	6	JUMBO CASE	1486.52	e blithely pending pac
62525	salmon brown bisque sienna violet	Manufacturer#1           	Brand#12  	STANDARD PLATED STEEL	42	MED DRUM  	1487.52	earls wake
62526	medium cyan antique steel tan	Manufacturer#5           	Brand#55  	PROMO POLISHED STEEL	32	WRAP BAG  	1488.52	usly along the slyl
62531	peach chocolate honeydew cornsilk goldenrod	Manufacturer#3           	Brand#31  	STANDARD ANODIZED NICKEL	11	SM BAG    	1493.53	 stealthi
64989	firebrick antique purple white olive	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED COPPER	42	MED JAR   	1953.98	blithely daringly 
64990	cyan violet khaki ivory deep	Manufacturer#5           	Brand#51  	LARGE POLISHED COPPER	48	SM BAG    	1954.99	le slyly spec
64992	burlywood cornflower khaki bisque pink	Manufacturer#4           	Brand#41  	MEDIUM PLATED NICKEL	5	LG JAR    	1956.99	s. slyly i
64996	indian drab deep goldenrod turquoise	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED NICKEL	38	MED JAR   	1960.99	fully according 
64999	drab ivory antique gainsboro burlywood	Manufacturer#4           	Brand#42  	LARGE POLISHED NICKEL	11	WRAP PACK 	1963.99	unts wake 
65000	navy bisque tan linen lawn	Manufacturer#1           	Brand#12  	STANDARD ANODIZED STEEL	28	MED PACK  	965.00	 packages
65002	almond navy moccasin chocolate beige	Manufacturer#4           	Brand#42  	PROMO BRUSHED COPPER	24	JUMBO BAG 	967.00	s detect blithely
65007	khaki chocolate gainsboro drab navajo	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED COPPER	42	WRAP CAN  	972.00	ss deposits around 
65008	cornsilk orchid black aquamarine light	Manufacturer#4           	Brand#43  	STANDARD POLISHED BRASS	34	MED BOX   	973.00	ully ac
65009	pale ghost lace chartreuse rosy	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	5	LG BOX    	974.00	 express i
65010	red tan hot khaki royal	Manufacturer#2           	Brand#25  	MEDIUM PLATED COPPER	20	LG JAR    	975.01	deposits haggle qu
65011	papaya brown orange hot dark	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED COPPER	27	MED PACK  	976.01	thless accounts nag i
65015	misty goldenrod dim blanched purple	Manufacturer#2           	Brand#21  	PROMO PLATED BRASS	4	LG BOX    	980.01	bravel
65016	pink linen blanched antique cornsilk	Manufacturer#2           	Brand#23  	ECONOMY PLATED STEEL	19	SM PACK   	981.01	ns integrate slyly fi
65020	green puff turquoise chiffon white	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED STEEL	38	MED PKG   	985.02	ely. accounts nag. slo
65021	maroon pale bisque orchid blanched	Manufacturer#3           	Brand#34  	MEDIUM POLISHED COPPER	46	SM JAR    	986.02	e slyly 
65024	indian chocolate saddle light wheat	Manufacturer#2           	Brand#22  	STANDARD PLATED BRASS	32	SM BAG    	989.02	deas; ev
65025	bisque moccasin gainsboro violet puff	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED NICKEL	50	MED DRUM  	990.02	 according to the
65026	cream papaya almond indian chocolate	Manufacturer#3           	Brand#31  	SMALL POLISHED STEEL	28	LG CASE   	991.02	thely bold d
65030	dodger burnished moccasin medium forest	Manufacturer#1           	Brand#14  	PROMO BURNISHED NICKEL	9	JUMBO CASE	995.03	ffily express theodol
65031	olive tan blush white linen	Manufacturer#3           	Brand#34  	LARGE BRUSHED TIN	28	LG JAR    	996.03	ly final theo
65032	lace powder red turquoise tan	Manufacturer#5           	Brand#51  	SMALL ANODIZED BRASS	29	WRAP PACK 	997.03	ng, ironic ideas u
65037	light orchid tomato pale magenta	Manufacturer#4           	Brand#41  	STANDARD ANODIZED NICKEL	44	MED DRUM  	1002.03	 unusual packages be
67495	dark deep chartreuse moccasin mint	Manufacturer#4           	Brand#41  	MEDIUM BURNISHED BRASS	40	MED DRUM  	1462.49	, regular dependenc
67496	black midnight goldenrod orchid cornflower	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	29	JUMBO CAN 	1463.49	jole quic
67498	lace peru moccasin midnight azure	Manufacturer#2           	Brand#23  	LARGE ANODIZED BRASS	7	WRAP PKG  	1465.49	ts. car
67502	ivory dim smoke violet azure	Manufacturer#2           	Brand#21  	ECONOMY BURNISHED STEEL	16	SM BAG    	1469.50	ions.
67505	magenta spring aquamarine ghost cyan	Manufacturer#1           	Brand#12  	SMALL BURNISHED COPPER	15	JUMBO JAR 	1472.50	ously p
67506	bisque brown cornsilk cream forest	Manufacturer#3           	Brand#34  	LARGE POLISHED TIN	12	SM CASE   	1473.50	n requests caj
67508	rose grey drab orchid light	Manufacturer#5           	Brand#55  	STANDARD ANODIZED BRASS	8	JUMBO CASE	1475.50	y slow accounts
67513	firebrick chartreuse mint brown linen	Manufacturer#5           	Brand#53  	MEDIUM POLISHED BRASS	32	WRAP CASE 	1480.51	ajole slyl
67514	salmon magenta cream frosted snow	Manufacturer#5           	Brand#51  	ECONOMY PLATED STEEL	5	MED PACK  	1481.51	rts s
67515	dodger metallic blanched spring goldenrod	Manufacturer#3           	Brand#34  	LARGE BURNISHED BRASS	36	WRAP DRUM 	1482.51	e qui
67516	brown khaki dark black goldenrod	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED STEEL	35	SM DRUM   	1483.51	y above the blithel
67517	seashell tomato chiffon cornflower powder	Manufacturer#4           	Brand#45  	PROMO PLATED STEEL	23	WRAP CAN  	1484.51	the unus
67521	purple almond gainsboro grey medium	Manufacturer#5           	Brand#54  	MEDIUM POLISHED BRASS	26	SM PACK   	1488.52	haggle fluff
67522	orchid yellow medium blue azure	Manufacturer#1           	Brand#15  	PROMO BURNISHED TIN	32	SM PACK   	1489.52	 furiously close 
67526	antique rosy firebrick yellow black	Manufacturer#4           	Brand#41  	SMALL BURNISHED COPPER	43	WRAP BAG  	1493.52	osits nag accord
67527	brown midnight salmon dark rose	Manufacturer#4           	Brand#41  	STANDARD POLISHED COPPER	44	MED JAR   	1494.52	l, pendi
67530	green antique cream steel moccasin	Manufacturer#3           	Brand#33  	SMALL BRUSHED COPPER	11	LG JAR    	1497.53	nic theod
67531	cream rose lime peach mint	Manufacturer#2           	Brand#24  	LARGE POLISHED TIN	33	LG JAR    	1498.53	ake alongside of 
67532	violet metallic medium powder navajo	Manufacturer#1           	Brand#12  	PROMO PLATED TIN	11	WRAP CAN  	1499.53	n foxes wake slyly f
67536	chiffon dark snow black lavender	Manufacturer#1           	Brand#13  	STANDARD ANODIZED STEEL	31	LG JAR    	1503.53	ronic theodolites int
67537	dark sandy peach blanched beige	Manufacturer#3           	Brand#31  	LARGE BURNISHED COPPER	45	MED JAR   	1504.53	its are. slyly
67538	gainsboro puff metallic dim spring	Manufacturer#1           	Brand#11  	ECONOMY BRUSHED NICKEL	24	SM PKG    	1505.53	special accou
67543	orchid wheat cream green tomato	Manufacturer#5           	Brand#52  	PROMO BURNISHED NICKEL	20	JUMBO BOX 	1510.54	luffily 
70001	sky green dim hot chartreuse	Manufacturer#1           	Brand#12  	PROMO PLATED TIN	31	MED CAN   	971.00	mes ironic
70002	cornflower gainsboro papaya green snow	Manufacturer#4           	Brand#43  	SMALL BRUSHED STEEL	45	SM DRUM   	972.00	ular dependenc
70004	misty yellow drab floral sandy	Manufacturer#4           	Brand#42  	LARGE ANODIZED COPPER	30	WRAP JAR  	974.00	sly. q
70008	goldenrod sandy orchid tan firebrick	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED STEEL	14	WRAP PACK 	978.00	ly regular theodo
70011	tomato tan forest misty plum	Manufacturer#1           	Brand#14  	SMALL POLISHED BRASS	19	WRAP DRUM 	981.01	 the even,
70012	almond forest plum navajo slate	Manufacturer#3           	Brand#33  	PROMO BURNISHED BRASS	19	MED DRUM  	982.01	gular 
70014	lime plum snow seashell cyan	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED STEEL	20	MED CASE  	984.01	. evenly special pa
70019	red dodger saddle olive floral	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	13	SM DRUM   	989.01	ccounts sleep care
70020	wheat midnight puff almond light	Manufacturer#4           	Brand#45  	STANDARD ANODIZED COPPER	32	MED JAR   	990.02	. blithely iro
70021	brown rose cyan blanched red	Manufacturer#4           	Brand#43  	PROMO BRUSHED BRASS	34	WRAP CASE 	991.02	inst t
70022	peach spring magenta firebrick dim	Manufacturer#3           	Brand#35  	PROMO POLISHED COPPER	9	LG JAR    	992.02	he pendin
70023	puff hot lace tan rosy	Manufacturer#1           	Brand#15  	ECONOMY POLISHED COPPER	23	WRAP BOX  	993.02	uests. ironic acco
70027	gainsboro firebrick ghost deep violet	Manufacturer#1           	Brand#14  	STANDARD POLISHED NICKEL	40	LG PACK   	997.02	into beans af
70028	red saddle moccasin gainsboro turquoise	Manufacturer#2           	Brand#21  	MEDIUM PLATED NICKEL	40	SM BOX    	998.02	the u
70032	deep beige navy green medium	Manufacturer#4           	Brand#44  	MEDIUM PLATED TIN	45	WRAP JAR  	1002.03	s final package
70033	hot beige cornsilk slate olive	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED TIN	7	JUMBO BOX 	1003.03	uests wake. blithely
70036	burnished goldenrod red navy azure	Manufacturer#2           	Brand#24  	PROMO POLISHED NICKEL	26	MED PKG   	1006.03	final accou
70037	orchid azure red goldenrod dodger	Manufacturer#1           	Brand#11  	STANDARD ANODIZED NICKEL	47	LG BAG    	1007.03	efully regular d
70038	goldenrod violet navajo antique turquoise	Manufacturer#1           	Brand#12  	SMALL BRUSHED BRASS	16	WRAP CAN  	1008.03	sts haggle caref
70042	maroon peru almond turquoise chiffon	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED TIN	41	WRAP PKG  	1012.04	ts boost 
70043	dark turquoise pale aquamarine papaya	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED COPPER	41	JUMBO BOX 	1013.04	equests haggle 
70044	dark dodger drab dim puff	Manufacturer#1           	Brand#13  	SMALL BURNISHED COPPER	32	WRAP CAN  	1014.04	ely ironic acco
70049	rose honeydew burnished beige moccasin	Manufacturer#4           	Brand#45  	STANDARD BRUSHED NICKEL	13	WRAP CASE 	1019.04	 theod
72480	tan ghost almond mint drab	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	9	MED BAG   	1452.48	express pl
72481	aquamarine chocolate drab misty brown	Manufacturer#1           	Brand#11  	LARGE PLATED BRASS	25	LG BAG    	1453.48	counts cajole slow e
72483	blue blanched misty plum burnished	Manufacturer#1           	Brand#13  	PROMO BURNISHED STEEL	36	LG CASE   	1455.48	e furiously busily eve
72487	spring metallic puff frosted goldenrod	Manufacturer#5           	Brand#51  	LARGE ANODIZED STEEL	16	JUMBO CAN 	1459.48	against 
72490	gainsboro peach azure violet smoke	Manufacturer#1           	Brand#13  	PROMO BRUSHED COPPER	34	WRAP DRUM 	1462.49	he permanen
72491	almond cream powder rosy midnight	Manufacturer#1           	Brand#14  	ECONOMY POLISHED NICKEL	46	MED DRUM  	1463.49	es. tithes ar
72493	goldenrod honeydew purple tomato hot	Manufacturer#2           	Brand#22  	LARGE POLISHED COPPER	47	JUMBO PACK	1465.49	e carefully unusual d
72498	forest grey metallic midnight sky	Manufacturer#2           	Brand#24  	SMALL BURNISHED COPPER	37	WRAP PACK 	1470.49	frets. daringly r
72499	navy white brown steel seashell	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED BRASS	32	LG CAN    	1471.49	pitaphs across 
72500	peru lime ghost burlywood purple	Manufacturer#5           	Brand#53  	STANDARD PLATED STEEL	50	LG PACK   	1472.50	blithely pend
72501	azure khaki snow pink forest	Manufacturer#5           	Brand#52  	STANDARD PLATED NICKEL	37	WRAP PKG  	1473.50	onic 
72502	lavender tomato bisque rosy thistle	Manufacturer#1           	Brand#11  	PROMO POLISHED STEEL	43	MED PACK  	1474.50	regular pinto beans. 
72506	midnight drab saddle navajo beige	Manufacturer#1           	Brand#13  	ECONOMY PLATED BRASS	16	MED CASE  	1478.50	lyly ironic the
72507	saddle wheat green burlywood mint	Manufacturer#3           	Brand#31  	PROMO POLISHED BRASS	39	MED DRUM  	1479.50	ly along the ev
72511	tomato purple turquoise lime powder	Manufacturer#2           	Brand#22  	LARGE PLATED NICKEL	21	WRAP CASE 	1483.51	, pending 
72512	almond deep slate sienna peru	Manufacturer#5           	Brand#51  	STANDARD BRUSHED TIN	27	LG CAN    	1484.51	y even packag
72515	gainsboro tan lavender maroon metallic	Manufacturer#1           	Brand#11  	LARGE POLISHED COPPER	49	JUMBO DRUM	1487.51	ffily. furiously 
72516	seashell navajo tomato wheat burlywood	Manufacturer#2           	Brand#21  	PROMO BRUSHED COPPER	44	LG BOX    	1488.51	sly silent deposi
72517	deep green midnight chocolate magenta	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED BRASS	7	WRAP PKG  	1489.51	g to the fluffily exp
72521	powder peru antique royal misty	Manufacturer#1           	Brand#11  	LARGE POLISHED COPPER	3	MED CAN   	1493.52	ep. quic
72522	azure brown dark navajo dodger	Manufacturer#4           	Brand#42  	SMALL BURNISHED TIN	47	MED CASE  	1494.52	ic requests. furious
72523	yellow chiffon blue ghost smoke	Manufacturer#1           	Brand#11  	SMALL POLISHED NICKEL	5	LG BOX    	1495.52	usly ruthless requests
72528	navy magenta hot drab cyan	Manufacturer#5           	Brand#53  	PROMO PLATED COPPER	50	MED BOX   	1500.52	elets. deposits ha
74987	slate ivory dark maroon steel	Manufacturer#3           	Brand#35  	STANDARD BRUSHED COPPER	44	LG CAN    	1961.98	ns. bli
74988	lime steel sandy rosy rose	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED COPPER	41	WRAP CAN  	1962.98	ly regu
74990	peru plum dodger gainsboro blush	Manufacturer#5           	Brand#51  	SMALL BURNISHED BRASS	17	MED BOX   	1964.99	nstructio
74994	chiffon firebrick dark spring hot	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED BRASS	19	JUMBO BAG 	1968.99	slyly final f
74997	rosy brown firebrick pale burlywood	Manufacturer#3           	Brand#32  	PROMO POLISHED TIN	37	JUMBO PACK	1971.99	 slyly
74998	chiffon wheat salmon almond lavender	Manufacturer#4           	Brand#42  	SMALL BURNISHED COPPER	49	MED BAG   	1972.99	 unusual accounts us
75000	moccasin maroon forest papaya snow	Manufacturer#5           	Brand#51  	STANDARD PLATED BRASS	24	SM JAR    	975.00	ys. blit
75005	red sandy dodger burlywood navy	Manufacturer#3           	Brand#31  	STANDARD PLATED COPPER	7	SM BAG    	980.00	ke across the
75006	lawn violet firebrick pale peach	Manufacturer#4           	Brand#42  	PROMO ANODIZED NICKEL	1	LG BOX    	981.00	sly bold
75007	pale goldenrod burlywood smoke midnight	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED STEEL	6	WRAP PACK 	982.00	ely bold packages. 
75008	aquamarine blue floral linen dim	Manufacturer#2           	Brand#23  	LARGE POLISHED BRASS	31	WRAP JAR  	983.00	ld dependencies? sly
75009	almond steel salmon khaki cornflower	Manufacturer#4           	Brand#44  	STANDARD ANODIZED BRASS	34	MED BAG   	984.00	sly sp
75013	drab dark dodger cream maroon	Manufacturer#4           	Brand#41  	LARGE BURNISHED TIN	12	WRAP PKG  	988.01	uickly a
75014	aquamarine purple drab cornflower dodger	Manufacturer#2           	Brand#23  	STANDARD POLISHED NICKEL	45	LG CAN    	989.01	s wake depe
75018	purple smoke rose linen burnished	Manufacturer#2           	Brand#22  	PROMO ANODIZED BRASS	41	MED BAG   	993.01	ffy tith
75019	green orchid sky mint peach	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED COPPER	32	MED JAR   	994.01	e express, dogged ex
75022	burnished powder light sandy wheat	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED BRASS	28	LG DRUM   	997.02	deposits was furi
75023	magenta almond plum indian papaya	Manufacturer#3           	Brand#35  	PROMO ANODIZED BRASS	43	JUMBO CAN 	998.02	e alongside of t
75024	lime blanched magenta black royal	Manufacturer#5           	Brand#53  	LARGE POLISHED COPPER	49	JUMBO JAR 	999.02	eans. platelets aga
75028	peru turquoise steel yellow cornflower	Manufacturer#4           	Brand#43  	STANDARD POLISHED TIN	25	SM PKG    	1003.02	fully regula
75029	ivory grey thistle rose antique	Manufacturer#3           	Brand#35  	MEDIUM POLISHED COPPER	10	LG JAR    	1004.02	lthy asymptotes
75030	cream frosted burnished light midnight	Manufacturer#2           	Brand#25  	LARGE PLATED STEEL	8	SM CAN    	1005.03	lithely arou
75035	antique maroon hot azure sandy	Manufacturer#5           	Brand#51  	LARGE BURNISHED NICKEL	23	LG BAG    	1010.03	refully bold ideas ha
77494	lawn almond peru pink khaki	Manufacturer#4           	Brand#41  	STANDARD PLATED TIN	25	WRAP JAR  	1471.49	s the slyly 
77495	rosy brown goldenrod steel navy	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	44	WRAP BAG  	1472.49	regular 
77497	magenta tan cornflower blue aquamarine	Manufacturer#5           	Brand#53  	STANDARD PLATED TIN	33	JUMBO PACK	1474.49	lithely regul
77501	snow dim beige cornflower spring	Manufacturer#2           	Brand#21  	LARGE POLISHED BRASS	46	SM PACK   	1478.50	wake 
77504	tan honeydew khaki burnished rose	Manufacturer#2           	Brand#21  	PROMO BRUSHED STEEL	5	LG CASE   	1481.50	ic accoun
77505	dark blanched yellow deep gainsboro	Manufacturer#4           	Brand#41  	PROMO ANODIZED BRASS	33	MED JAR   	1482.50	hely i
77507	floral beige turquoise lawn burlywood	Manufacturer#4           	Brand#43  	PROMO PLATED STEEL	16	SM PACK   	1484.50	usly alongside 
77512	lime navajo firebrick saddle chocolate	Manufacturer#5           	Brand#52  	LARGE POLISHED NICKEL	22	SM JAR    	1489.51	ickly even
77513	khaki floral frosted blue lace	Manufacturer#1           	Brand#14  	PROMO PLATED COPPER	44	WRAP CAN  	1490.51	es are slyly aroun
77514	bisque dark spring drab burnished	Manufacturer#3           	Brand#31  	PROMO PLATED COPPER	26	LG JAR    	1491.51	y special requests w
77515	chiffon bisque forest light snow	Manufacturer#4           	Brand#44  	SMALL POLISHED COPPER	34	WRAP JAR  	1492.51	its boost foxes.
77516	saddle floral navajo olive bisque	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED BRASS	9	JUMBO CAN 	1493.51	s sle
77520	navajo pink gainsboro light hot	Manufacturer#1           	Brand#14  	LARGE BURNISHED STEEL	31	JUMBO BOX 	1497.52	oost. furious
77521	magenta steel sandy hot orange	Manufacturer#2           	Brand#23  	LARGE BRUSHED TIN	42	LG DRUM   	1498.52	uriously gifts. regula
77525	lemon papaya chocolate lavender medium	Manufacturer#5           	Brand#54  	PROMO ANODIZED COPPER	24	LG DRUM   	1502.52	 deposi
77526	deep lavender cyan peru steel	Manufacturer#5           	Brand#55  	PROMO ANODIZED TIN	26	WRAP DRUM 	1503.52	osits a
77529	puff misty pale tomato green	Manufacturer#1           	Brand#12  	PROMO POLISHED BRASS	15	SM BOX    	1506.52	boost a
77530	bisque pink burlywood black snow	Manufacturer#2           	Brand#23  	SMALL POLISHED BRASS	4	LG DRUM   	1507.53	kages wake: re
77531	linen pink antique chocolate coral	Manufacturer#3           	Brand#31  	PROMO POLISHED COPPER	38	LG PACK   	1508.53	ic instru
77535	forest cornsilk royal tomato firebrick	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED COPPER	48	WRAP JAR  	1512.53	 of the
77536	grey purple frosted seashell turquoise	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED COPPER	1	WRAP PACK 	1513.53	regula
77537	black antique slate salmon peach	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED STEEL	1	SM BAG    	1514.53	e furiously.
77542	mint ivory spring magenta navajo	Manufacturer#3           	Brand#33  	PROMO BURNISHED TIN	31	SM JAR    	1519.54	eposit
80001	olive blue peach coral honeydew	Manufacturer#3           	Brand#31  	ECONOMY POLISHED COPPER	38	WRAP PACK 	981.00	ts haggle. s
80002	sandy gainsboro sky linen snow	Manufacturer#2           	Brand#24  	ECONOMY BRUSHED NICKEL	24	JUMBO BAG 	982.00	cajol
80004	lemon papaya maroon seashell peru	Manufacturer#2           	Brand#22  	LARGE ANODIZED BRASS	4	SM CASE   	984.00	nstructi
80008	thistle ghost sienna metallic chocolate	Manufacturer#4           	Brand#45  	STANDARD ANODIZED COPPER	45	WRAP CASE 	988.00	ld requ
80011	moccasin navy blush gainsboro powder	Manufacturer#1           	Brand#15  	LARGE BRUSHED TIN	19	JUMBO CASE	991.01	grow blithely again
80012	green slate peach linen hot	Manufacturer#1           	Brand#11  	LARGE BRUSHED STEEL	10	JUMBO DRUM	992.01	latelets. carefully
80014	khaki papaya lemon linen sienna	Manufacturer#3           	Brand#31  	LARGE BURNISHED STEEL	25	MED JAR   	994.01	st the
80019	cornsilk midnight cyan maroon khaki	Manufacturer#3           	Brand#33  	PROMO BRUSHED BRASS	7	JUMBO JAR 	999.01	lly e
80020	peru thistle dark pink steel	Manufacturer#2           	Brand#22  	LARGE BRUSHED BRASS	28	LG BAG    	1000.02	s the caref
80021	linen lemon forest papaya blush	Manufacturer#2           	Brand#24  	SMALL BRUSHED NICKEL	49	MED CAN   	1001.02	s. permanently iro
80022	dark moccasin purple medium mint	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	42	MED CAN   	1002.02	 the silen
80023	azure ghost turquoise purple hot	Manufacturer#1           	Brand#12  	LARGE ANODIZED STEEL	23	MED PKG   	1003.02	about the ironic acc
80027	peru ivory violet maroon orchid	Manufacturer#3           	Brand#33  	ECONOMY POLISHED STEEL	24	WRAP CASE 	1007.02	s; furiously 
80028	indian white sienna cornflower chiffon	Manufacturer#5           	Brand#54  	SMALL PLATED TIN	46	MED BAG   	1008.02	promise quickly fin
80032	medium turquoise gainsboro midnight firebrick	Manufacturer#4           	Brand#43  	SMALL ANODIZED NICKEL	33	LG JAR    	1012.03	 cajole deposits. spe
80033	linen moccasin turquoise yellow antique	Manufacturer#2           	Brand#21  	SMALL BURNISHED STEEL	3	WRAP JAR  	1013.03	e furiously b
80036	violet papaya blanched aquamarine thistle	Manufacturer#3           	Brand#34  	STANDARD ANODIZED COPPER	4	JUMBO JAR 	1016.03	requests. fi
80037	misty seashell firebrick grey blue	Manufacturer#2           	Brand#21  	STANDARD BURNISHED STEEL	6	WRAP PACK 	1017.03	ronic package
80038	rosy indian papaya pale thistle	Manufacturer#5           	Brand#51  	PROMO POLISHED NICKEL	17	WRAP PACK 	1018.03	yly. expre
80042	drab indian yellow saddle azure	Manufacturer#2           	Brand#21  	ECONOMY PLATED TIN	5	WRAP CAN  	1022.04	r foxes snooze. som
80043	brown dim firebrick lace spring	Manufacturer#5           	Brand#54  	SMALL ANODIZED NICKEL	22	MED CASE  	1023.04	en instructions wake. 
80044	tan dim chiffon bisque peach	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	14	WRAP BOX  	1024.04	uriously. 
80049	floral burlywood dim spring black	Manufacturer#2           	Brand#22  	STANDARD PLATED TIN	34	JUMBO PKG 	1029.04	ironic, ironic
82477	antique spring orchid steel snow	Manufacturer#3           	Brand#32  	STANDARD POLISHED NICKEL	40	MED CAN   	1459.47	carefully final as
82478	tan light mint sky dark	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED BRASS	43	LG PKG    	1460.47	kly idle inst
82480	ivory cream green drab cornflower	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED COPPER	17	JUMBO JAR 	1462.48	 of the 
82484	blanched turquoise aquamarine puff saddle	Manufacturer#3           	Brand#31  	LARGE BURNISHED NICKEL	32	WRAP BOX  	1466.48	refully 
82487	dark light frosted almond burnished	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED BRASS	21	JUMBO CASE	1469.48	 impress quickly. 
82488	blue hot lace sandy drab	Manufacturer#4           	Brand#41  	SMALL BRUSHED BRASS	47	SM CASE   	1470.48	kages cajol
82490	goldenrod rose forest orange yellow	Manufacturer#2           	Brand#23  	PROMO POLISHED STEEL	40	JUMBO PACK	1472.49	 behind the final foxe
82495	red coral floral tomato peru	Manufacturer#4           	Brand#44  	MEDIUM BURNISHED STEEL	36	JUMBO PKG 	1477.49	posits sleep sl
82496	coral dodger wheat lawn sandy	Manufacturer#5           	Brand#52  	ECONOMY BURNISHED TIN	22	WRAP JAR  	1478.49	inal requests after
82497	khaki plum dim firebrick dark	Manufacturer#1           	Brand#13  	PROMO BURNISHED STEEL	16	LG BOX    	1479.49	y neve
82498	black papaya hot navy brown	Manufacturer#5           	Brand#51  	ECONOMY POLISHED TIN	24	JUMBO PKG 	1480.49	 dependencies. qui
82499	cyan orange white almond steel	Manufacturer#5           	Brand#55  	PROMO ANODIZED NICKEL	3	SM DRUM   	1481.49	y patterns
82503	lime tomato maroon moccasin lace	Manufacturer#4           	Brand#44  	ECONOMY BRUSHED NICKEL	41	LG PACK   	1485.50	xcuses cajole s
82504	sienna snow pale black chocolate	Manufacturer#1           	Brand#14  	LARGE BRUSHED BRASS	10	SM CAN    	1486.50	furiously perman
82508	metallic bisque sandy cornflower spring	Manufacturer#1           	Brand#15  	SMALL BURNISHED BRASS	33	MED JAR   	1490.50	low instructions 
82509	turquoise cream purple hot tan	Manufacturer#4           	Brand#45  	LARGE BRUSHED BRASS	44	LG PKG    	1491.50	the carefully even r
82512	drab white thistle lime rosy	Manufacturer#1           	Brand#14  	PROMO PLATED BRASS	39	WRAP CAN  	1494.51	 instruc
82513	mint royal dark cornflower violet	Manufacturer#5           	Brand#54  	MEDIUM POLISHED STEEL	16	JUMBO CAN 	1495.51	 dependenc
82514	navajo blush slate cornsilk goldenrod	Manufacturer#2           	Brand#22  	ECONOMY BRUSHED STEEL	49	MED CAN   	1496.51	egular pinto b
82518	chartreuse salmon black medium pale	Manufacturer#2           	Brand#24  	MEDIUM POLISHED STEEL	19	LG PKG    	1500.51	ep slyly slyly bol
82519	linen tan orange almond bisque	Manufacturer#5           	Brand#55  	PROMO BURNISHED STEEL	29	LG BAG    	1501.51	oss the
82520	rosy cornsilk green violet lace	Manufacturer#5           	Brand#52  	ECONOMY ANODIZED TIN	20	MED PKG   	1502.52	lites. furiously i
82525	black chocolate thistle lemon cornsilk	Manufacturer#3           	Brand#33  	LARGE POLISHED NICKEL	18	MED BOX   	1507.52	totes maintai
84985	gainsboro cream maroon chiffon black	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	48	JUMBO PKG 	1969.98	uld have to impres
84986	navy magenta azure floral salmon	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED STEEL	3	LG DRUM   	1970.98	 ideas. bold i
84988	cornsilk lemon rose chocolate gainsboro	Manufacturer#4           	Brand#44  	ECONOMY PLATED TIN	21	LG BOX    	1972.98	e furiously 
84992	floral pink honeydew tan moccasin	Manufacturer#2           	Brand#23  	LARGE ANODIZED TIN	47	SM CASE   	1976.99	ng, f
84995	orchid navajo dark light chiffon	Manufacturer#5           	Brand#52  	LARGE BRUSHED COPPER	15	SM PKG    	1979.99	refully 
84996	violet light navajo gainsboro metallic	Manufacturer#1           	Brand#15  	LARGE BRUSHED NICKEL	22	MED JAR   	1980.99	request
84998	wheat honeydew lace thistle moccasin	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED BRASS	38	JUMBO BOX 	1982.99	o beans. quickly un
85003	medium rosy pale red coral	Manufacturer#3           	Brand#31  	SMALL BRUSHED TIN	40	WRAP PKG  	988.00	ular asymptot
85004	honeydew blush frosted linen royal	Manufacturer#2           	Brand#24  	SMALL BRUSHED BRASS	9	WRAP BOX  	989.00	account
85005	forest aquamarine burlywood dark green	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED TIN	50	LG CAN    	990.00	oost above the ir
85006	cornsilk pale salmon maroon medium	Manufacturer#4           	Brand#42  	LARGE ANODIZED BRASS	21	SM CAN    	991.00	 about 
85007	metallic tomato thistle forest beige	Manufacturer#1           	Brand#11  	STANDARD POLISHED BRASS	6	WRAP BOX  	992.00	gainst the quickly exp
85011	brown orange navy slate burnished	Manufacturer#2           	Brand#22  	PROMO BRUSHED TIN	13	LG BOX    	996.01	pending, regula
85012	beige dim hot burnished sandy	Manufacturer#5           	Brand#54  	LARGE PLATED NICKEL	26	SM DRUM   	997.01	ies nag blithe
85016	chocolate blanched snow metallic burlywood	Manufacturer#1           	Brand#12  	PROMO BRUSHED BRASS	48	LG DRUM   	1001.01	ainments among the
85017	orange cornsilk honeydew khaki dim	Manufacturer#1           	Brand#11  	MEDIUM POLISHED STEEL	49	SM BOX    	1002.01	uests. carefully
85020	royal blue blanched lemon tomato	Manufacturer#2           	Brand#24  	PROMO POLISHED STEEL	26	JUMBO BAG 	1005.02	wake afte
85021	moccasin saddle midnight lavender navy	Manufacturer#5           	Brand#54  	STANDARD BRUSHED COPPER	47	JUMBO PKG 	1006.02	ly regular asympto
85022	cornflower deep maroon slate yellow	Manufacturer#2           	Brand#22  	SMALL POLISHED NICKEL	26	JUMBO CAN 	1007.02	platelets. packages h
85026	white royal wheat dark magenta	Manufacturer#1           	Brand#13  	PROMO BRUSHED BRASS	13	JUMBO BOX 	1011.02	xcuses. slowly regu
85027	blue frosted hot steel dim	Manufacturer#2           	Brand#22  	MEDIUM PLATED COPPER	46	MED BOX   	1012.02	egular pint
85028	papaya ivory dodger floral purple	Manufacturer#3           	Brand#35  	PROMO ANODIZED COPPER	49	WRAP BAG  	1013.02	dolites. quickly 
85033	medium purple misty midnight gainsboro	Manufacturer#4           	Brand#44  	MEDIUM POLISHED NICKEL	40	JUMBO JAR 	1018.03	ckly even
87493	violet floral cyan grey steel	Manufacturer#5           	Brand#55  	LARGE BURNISHED STEEL	4	LG BAG    	1480.49	lyly. furious
87494	bisque royal drab azure slate	Manufacturer#1           	Brand#14  	PROMO PLATED NICKEL	15	SM PACK   	1481.49	y carefully specia
87496	goldenrod snow dim burnished honeydew	Manufacturer#5           	Brand#51  	PROMO BURNISHED COPPER	48	MED CAN   	1483.49	st haggle slyly? furio
87500	turquoise peru dodger pink linen	Manufacturer#2           	Brand#24  	STANDARD ANODIZED BRASS	21	WRAP CAN  	1487.50	ly furiously even fr
87503	aquamarine drab blue wheat goldenrod	Manufacturer#4           	Brand#44  	LARGE ANODIZED STEEL	39	LG JAR    	1490.50	ans solve careful
87504	sandy olive cyan orange chiffon	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED TIN	33	MED PACK  	1491.50	pinto beans sleep 
87506	chartreuse antique papaya burlywood green	Manufacturer#1           	Brand#14  	MEDIUM BURNISHED NICKEL	12	SM CASE   	1493.50	s. sl
87511	blue tomato wheat beige bisque	Manufacturer#5           	Brand#51  	SMALL BRUSHED TIN	21	LG BAG    	1498.51	e dependencies ab
87512	olive light red green brown	Manufacturer#1           	Brand#12  	STANDARD BURNISHED STEEL	11	SM PKG    	1499.51	ies s
87513	dodger tan turquoise coral beige	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED STEEL	8	WRAP JAR  	1500.51	ong the slyly p
87514	medium black white beige cornsilk	Manufacturer#1           	Brand#14  	PROMO ANODIZED STEEL	31	WRAP DRUM 	1501.51	ar, ironic requests 
87515	maroon peru snow ghost almond	Manufacturer#4           	Brand#42  	SMALL ANODIZED COPPER	7	JUMBO JAR 	1502.51	out the un
87519	grey red frosted navy magenta	Manufacturer#2           	Brand#25  	PROMO ANODIZED COPPER	34	LG PKG    	1506.51	en dependenci
87520	bisque papaya seashell misty spring	Manufacturer#3           	Brand#32  	SMALL BRUSHED STEEL	22	LG CAN    	1507.52	se. express accounts 
87524	honeydew papaya olive gainsboro khaki	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED STEEL	9	LG BAG    	1511.52	tructions nag blith
87525	burlywood dim black floral khaki	Manufacturer#5           	Brand#54  	MEDIUM BRUSHED NICKEL	43	LG CAN    	1512.52	sly unusual
87528	lavender almond sky honeydew navy	Manufacturer#4           	Brand#44  	PROMO ANODIZED BRASS	8	MED DRUM  	1515.52	ympto
87529	azure sky blanched floral maroon	Manufacturer#4           	Brand#45  	MEDIUM POLISHED TIN	2	LG CASE   	1516.52	quickly unu
87530	spring tan lace cyan navy	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED COPPER	32	SM BAG    	1517.53	ly even dep
87534	white rose ivory blush deep	Manufacturer#2           	Brand#22  	LARGE BURNISHED COPPER	42	SM BOX    	1521.53	slyly unusual fo
87535	olive blush violet lavender beige	Manufacturer#3           	Brand#34  	MEDIUM PLATED COPPER	21	WRAP PACK 	1522.53	e pending foxes. quic
87536	violet papaya dark antique sky	Manufacturer#5           	Brand#51  	PROMO BURNISHED COPPER	31	WRAP BOX  	1523.53	iously 
87541	cream deep sky linen drab	Manufacturer#1           	Brand#11  	LARGE PLATED TIN	37	JUMBO CAN 	1528.54	hely above the furiou
90001	snow cornflower cream floral lavender	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED COPPER	18	MED PACK  	991.00	ons boost
90002	salmon black cornflower indian lime	Manufacturer#3           	Brand#34  	STANDARD ANODIZED BRASS	31	MED CASE  	992.00	s the
90004	navy saddle maroon violet blush	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	46	MED BOX   	994.00	 furiously. furiousl
90008	brown sandy coral lemon metallic	Manufacturer#2           	Brand#23  	STANDARD POLISHED TIN	35	WRAP PACK 	998.00	ccounts. boldly reg
90011	indian sandy maroon drab bisque	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED STEEL	42	SM BOX    	1001.01	ts cajole blit
90012	turquoise deep maroon red frosted	Manufacturer#2           	Brand#23  	PROMO BRUSHED NICKEL	40	MED BOX   	1002.01	nto beans nag sl
90014	brown green misty cornflower indian	Manufacturer#3           	Brand#32  	LARGE PLATED TIN	10	MED CASE  	1004.01	, unus
90019	plum misty tan ghost cyan	Manufacturer#4           	Brand#45  	MEDIUM ANODIZED TIN	32	MED JAR   	1009.01	inal ideas affi
90020	moccasin orchid lawn deep slate	Manufacturer#3           	Brand#33  	PROMO PLATED STEEL	6	JUMBO PACK	1010.02	ular foxes 
90021	papaya slate moccasin ivory lace	Manufacturer#1           	Brand#12  	LARGE BURNISHED STEEL	22	MED PACK  	1011.02	 final
90022	sky hot seashell almond moccasin	Manufacturer#1           	Brand#14  	ECONOMY PLATED STEEL	22	WRAP CAN  	1012.02	ckly iron
90023	thistle powder linen papaya red	Manufacturer#3           	Brand#31  	SMALL BRUSHED TIN	19	SM JAR    	1013.02	n theodo
90027	dodger saddle seashell gainsboro orchid	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED STEEL	13	WRAP PACK 	1017.02	efully pending d
90028	bisque slate cream dodger turquoise	Manufacturer#5           	Brand#54  	STANDARD ANODIZED BRASS	9	WRAP JAR  	1018.02	to beans 
90032	navajo frosted ghost hot salmon	Manufacturer#5           	Brand#55  	LARGE BRUSHED BRASS	22	MED CAN   	1022.03	ctions. ir
90033	purple frosted sandy blush cornsilk	Manufacturer#1           	Brand#14  	MEDIUM BURNISHED NICKEL	4	WRAP PACK 	1023.03	s sleep. slyly fin
90036	ivory dodger wheat spring burnished	Manufacturer#2           	Brand#22  	MEDIUM POLISHED BRASS	33	SM PACK   	1026.03	fluffi
90037	firebrick navy chocolate pink lemon	Manufacturer#4           	Brand#41  	LARGE BRUSHED BRASS	23	MED BOX   	1027.03	ing packages. iro
90038	burnished midnight goldenrod cornflower green	Manufacturer#1           	Brand#13  	STANDARD ANODIZED STEEL	13	MED DRUM  	1028.03	ven package
90042	purple misty sienna green violet	Manufacturer#5           	Brand#53  	SMALL PLATED TIN	23	LG CASE   	1032.04	r deposi
90043	saddle chocolate aquamarine maroon frosted	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED STEEL	4	MED JAR   	1033.04	posits a
90044	peach frosted tomato dodger snow	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED BRASS	17	MED PKG   	1034.04	ts nag f
90049	cyan medium smoke lawn black	Manufacturer#4           	Brand#41  	SMALL ANODIZED STEEL	20	LG PKG    	1039.04	ke quickly
92474	magenta lime black dodger firebrick	Manufacturer#4           	Brand#42  	LARGE BURNISHED BRASS	42	SM CASE   	1466.47	s. quickly final 
92475	floral orange royal sienna linen	Manufacturer#2           	Brand#22  	STANDARD POLISHED BRASS	25	WRAP PKG  	1467.47	ully express attain
92477	blanched forest peach chiffon thistle	Manufacturer#4           	Brand#44  	LARGE BRUSHED STEEL	33	JUMBO BOX 	1469.47	 carefully busy i
92481	almond cornsilk firebrick sienna sandy	Manufacturer#2           	Brand#22  	SMALL POLISHED COPPER	32	JUMBO CAN 	1473.48	slyly unusual pack
92484	seashell brown violet blanched lawn	Manufacturer#3           	Brand#35  	MEDIUM POLISHED BRASS	34	LG PKG    	1476.48	refully unusual
92485	violet maroon dim floral gainsboro	Manufacturer#5           	Brand#54  	PROMO BRUSHED NICKEL	27	JUMBO BOX 	1477.48	ccounts. express, fina
92487	snow green royal burlywood bisque	Manufacturer#3           	Brand#32  	STANDARD BURNISHED COPPER	28	MED PACK  	1479.48	 quickly s
92492	lavender cornflower gainsboro light lawn	Manufacturer#3           	Brand#35  	SMALL POLISHED BRASS	42	MED CASE  	1484.49	ages. requ
92493	frosted medium navajo sienna sandy	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED COPPER	25	SM PKG    	1485.49	even inst
92494	lemon violet burlywood rosy forest	Manufacturer#3           	Brand#35  	PROMO BRUSHED TIN	9	SM BOX    	1486.49	 the carefully eve
92495	ghost tomato olive moccasin brown	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED BRASS	29	SM PKG    	1487.49	uctions use furiously 
92496	cornsilk peach coral thistle cyan	Manufacturer#5           	Brand#51  	LARGE PLATED STEEL	26	JUMBO BAG 	1488.49	c foxes are furiou
92500	black white violet azure seashell	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED NICKEL	7	MED BAG   	1492.50	o the carefull
92501	slate navajo magenta medium chiffon	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED STEEL	19	SM JAR    	1493.50	pendencies s
92505	black tan burnished pink rose	Manufacturer#2           	Brand#25  	SMALL ANODIZED NICKEL	12	LG PACK   	1497.50	ay regular
92506	powder olive khaki seashell brown	Manufacturer#5           	Brand#54  	LARGE POLISHED NICKEL	24	SM BOX    	1498.50	. fluffily ironic r
92509	violet goldenrod black rose salmon	Manufacturer#2           	Brand#23  	LARGE ANODIZED NICKEL	31	LG JAR    	1501.50	thely. carefully ir
92510	burnished midnight metallic cream aquamarine	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	21	MED CASE  	1502.51	 sleep. even ac
92511	blush purple peru snow khaki	Manufacturer#2           	Brand#23  	PROMO BRUSHED STEEL	48	MED CASE  	1503.51	r the regu
92515	coral dodger lemon goldenrod grey	Manufacturer#3           	Brand#33  	LARGE ANODIZED TIN	46	LG PACK   	1507.51	platelets cajole slyly
92516	royal thistle seashell lawn linen	Manufacturer#4           	Brand#44  	SMALL ANODIZED NICKEL	19	LG CASE   	1508.51	al deposit
92517	antique salmon snow metallic goldenrod	Manufacturer#2           	Brand#21  	STANDARD POLISHED NICKEL	4	JUMBO BAG 	1509.51	ular packages cajo
92522	floral violet almond pale red	Manufacturer#3           	Brand#35  	MEDIUM PLATED COPPER	34	WRAP DRUM 	1514.52	braids nag
94983	smoke cream olive floral saddle	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	27	SM CASE   	1977.98	 deposi
94984	papaya orchid peru brown smoke	Manufacturer#1           	Brand#15  	ECONOMY PLATED COPPER	40	MED CASE  	1978.98	ms promise slyly!
94986	slate goldenrod spring lavender green	Manufacturer#3           	Brand#35  	PROMO PLATED COPPER	38	JUMBO CAN 	1980.98	into beans
94990	beige slate plum sienna moccasin	Manufacturer#5           	Brand#53  	PROMO ANODIZED STEEL	14	LG CAN    	1984.99	. silently regular de
94993	spring deep chocolate gainsboro hot	Manufacturer#3           	Brand#32  	PROMO POLISHED NICKEL	50	WRAP DRUM 	1987.99	pinto beans. b
94994	slate lawn peru moccasin maroon	Manufacturer#3           	Brand#34  	MEDIUM POLISHED TIN	35	WRAP CAN  	1988.99	 slyly. slyly pendi
94996	pale rose antique aquamarine indian	Manufacturer#3           	Brand#31  	LARGE PLATED TIN	27	LG BOX    	1990.99	ts. furiousl
95001	goldenrod lemon purple honeydew forest	Manufacturer#4           	Brand#44  	SMALL BURNISHED NICKEL	3	WRAP BOX  	996.00	g. carefully br
95002	light antique navajo olive yellow	Manufacturer#3           	Brand#31  	ECONOMY BURNISHED STEEL	40	WRAP PKG  	997.00	y dependencies
95003	wheat saddle cornflower lace white	Manufacturer#4           	Brand#42  	PROMO ANODIZED NICKEL	27	LG JAR    	998.00	old foxes boo
95004	wheat chartreuse blue dark beige	Manufacturer#2           	Brand#22  	STANDARD PLATED NICKEL	36	LG BOX    	999.00	ans: carefull
95005	aquamarine saddle chiffon white navy	Manufacturer#5           	Brand#51  	SMALL BURNISHED TIN	33	WRAP DRUM 	1000.00	lent foxes. blithely e
95009	lawn rose honeydew slate ivory	Manufacturer#1           	Brand#11  	LARGE PLATED NICKEL	39	LG DRUM   	1004.00	lithely even
95010	cornsilk coral slate smoke saddle	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED BRASS	18	SM DRUM   	1005.01	le alongside of the 
95014	azure wheat lime floral hot	Manufacturer#5           	Brand#54  	LARGE BRUSHED TIN	34	SM PKG    	1009.01	y according to the u
95015	khaki wheat blush salmon antique	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED NICKEL	40	WRAP PACK 	1010.01	 deposits wake careful
95018	peru sandy bisque lime drab	Manufacturer#3           	Brand#32  	MEDIUM POLISHED BRASS	30	MED CASE  	1013.01	. final, regula
95019	lawn violet rose purple navajo	Manufacturer#1           	Brand#12  	MEDIUM PLATED BRASS	10	SM CAN    	1014.01	deposits use sl
95020	blue grey seashell dark salmon	Manufacturer#3           	Brand#35  	STANDARD PLATED BRASS	18	SM CAN    	1015.02	ckages sleep
95024	puff mint cornflower navajo cream	Manufacturer#3           	Brand#33  	SMALL PLATED COPPER	7	LG PKG    	1019.02	ily. carefully f
95025	azure firebrick peach bisque powder	Manufacturer#3           	Brand#34  	SMALL POLISHED STEEL	17	JUMBO PACK	1020.02	ously bold requests
95026	saddle chocolate violet spring goldenrod	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED STEEL	22	SM DRUM   	1021.02	 ironic ideas. ca
95031	royal cyan spring pale wheat	Manufacturer#5           	Brand#51  	LARGE BURNISHED NICKEL	28	MED JAR   	1026.03	riously a
97492	linen frosted floral purple steel	Manufacturer#2           	Brand#24  	PROMO BRUSHED TIN	19	WRAP BOX  	1489.49	quickly bold dolp
97493	turquoise mint moccasin forest azure	Manufacturer#2           	Brand#21  	PROMO BURNISHED COPPER	3	MED CASE  	1490.49	 requests. ac
97495	gainsboro blush ghost frosted forest	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED BRASS	7	LG CASE   	1492.49	yly. bli
97499	navy coral slate peach seashell	Manufacturer#1           	Brand#12  	STANDARD ANODIZED TIN	13	LG PKG    	1496.49	ge furi
97502	purple cornsilk blue hot goldenrod	Manufacturer#2           	Brand#25  	PROMO BRUSHED BRASS	42	LG PKG    	1499.50	 grow. blith
97503	burlywood indian tan dodger moccasin	Manufacturer#1           	Brand#14  	ECONOMY BURNISHED TIN	19	JUMBO BAG 	1500.50	ously even foxes ca
97505	burnished firebrick almond mint saddle	Manufacturer#4           	Brand#41  	SMALL PLATED NICKEL	17	LG BAG    	1502.50	egrate slyly 
97510	pale linen chiffon spring papaya	Manufacturer#2           	Brand#22  	PROMO POLISHED COPPER	12	MED CASE  	1507.51	inal ideas about th
97511	saddle royal tan rosy tomato	Manufacturer#4           	Brand#41  	ECONOMY PLATED NICKEL	41	WRAP DRUM 	1508.51	s. ironic ideas 
97512	red peach orchid rosy sandy	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	30	SM PKG    	1509.51	slyly
97513	seashell azure royal burnished black	Manufacturer#1           	Brand#14  	SMALL BRUSHED COPPER	32	WRAP CAN  	1510.51	s haggle slyly.
97514	magenta khaki linen sandy purple	Manufacturer#4           	Brand#45  	STANDARD ANODIZED COPPER	46	JUMBO PKG 	1511.51	en courts. furi
97518	forest saddle white chiffon peru	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED STEEL	33	SM DRUM   	1515.51	en account
97519	royal pink sky azure purple	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED BRASS	8	SM BAG    	1516.51	nst the car
97523	powder purple lace light blue	Manufacturer#4           	Brand#44  	STANDARD BRUSHED BRASS	22	JUMBO PKG 	1520.52	ithely quickl
97524	light medium brown sky honeydew	Manufacturer#1           	Brand#14  	MEDIUM PLATED TIN	20	WRAP CASE 	1521.52	wake slyly 
97527	cornsilk rosy forest wheat orchid	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED STEEL	1	LG PACK   	1524.52	he requests
97528	blue thistle turquoise sienna royal	Manufacturer#1           	Brand#14  	PROMO BRUSHED NICKEL	14	SM DRUM   	1525.52	 packages a
97529	smoke metallic navajo powder azure	Manufacturer#3           	Brand#34  	ECONOMY POLISHED BRASS	35	MED CAN   	1526.52	ully pen
97533	gainsboro chiffon misty sienna orchid	Manufacturer#5           	Brand#51  	STANDARD BRUSHED TIN	30	WRAP BAG  	1530.53	le bold request
97534	papaya blue magenta navy burnished	Manufacturer#5           	Brand#55  	PROMO BURNISHED STEEL	22	MED DRUM  	1531.53	oze blith
97535	purple spring metallic plum linen	Manufacturer#1           	Brand#11  	LARGE ANODIZED BRASS	6	LG DRUM   	1532.53	ut the final, even in
97540	blanched papaya cornflower grey deep	Manufacturer#2           	Brand#23  	MEDIUM POLISHED TIN	10	JUMBO PKG 	1537.54	y unusual reques
100001	seashell cyan plum purple honeydew	Manufacturer#3           	Brand#35  	STANDARD BRUSHED TIN	37	JUMBO CASE	1001.00	ronic dependencies d
100002	steel moccasin forest cornflower brown	Manufacturer#3           	Brand#34  	STANDARD ANODIZED NICKEL	11	WRAP CAN  	1002.00	 quickly pending 
100004	snow blanched khaki indian azure	Manufacturer#4           	Brand#42  	SMALL POLISHED TIN	29	SM CASE   	1004.00	sly. blithely
100008	spring powder sienna purple lime	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED BRASS	19	SM PKG    	1008.00	ts. furious
100011	firebrick rose moccasin thistle black	Manufacturer#5           	Brand#51  	PROMO ANODIZED STEEL	42	JUMBO DRUM	1011.01	riously bold d
100012	blush azure powder dodger mint	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED BRASS	31	MED CASE  	1012.01	 ideas 
100014	coral thistle steel blanched yellow	Manufacturer#3           	Brand#32  	LARGE BURNISHED BRASS	32	JUMBO JAR 	1014.01	arefully ironic depos
100019	dark dodger honeydew dim black	Manufacturer#3           	Brand#31  	LARGE BRUSHED STEEL	42	WRAP CASE 	1019.01	pecial deposit
100020	pink thistle cornflower tomato burlywood	Manufacturer#2           	Brand#24  	PROMO POLISHED STEEL	24	JUMBO PKG 	1020.02	ns daz
100021	beige burlywood drab saddle orchid	Manufacturer#3           	Brand#32  	LARGE POLISHED NICKEL	23	LG BOX    	1021.02	s. bold deposits a
100022	powder drab hot red bisque	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED TIN	20	LG BOX    	1022.02	ven packages. blith
100023	olive beige green khaki lavender	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED COPPER	34	WRAP JAR  	1023.02	uickly 
100027	orange deep cornflower lime aquamarine	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED STEEL	28	WRAP CASE 	1027.02	 the furio
100028	lace peru sky chiffon lavender	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED TIN	37	MED CAN   	1028.02	ructions; 
100032	lavender moccasin blush linen blanched	Manufacturer#2           	Brand#24  	LARGE POLISHED TIN	12	SM PKG    	1032.03	ctions. sl
100033	sandy papaya smoke wheat pink	Manufacturer#2           	Brand#24  	SMALL BRUSHED BRASS	17	SM CAN    	1033.03	closely slyly
100036	sky lime tan rose almond	Manufacturer#5           	Brand#52  	STANDARD BURNISHED NICKEL	39	SM PKG    	1036.03	thless, final asympt
100037	maroon lace light misty midnight	Manufacturer#5           	Brand#54  	PROMO POLISHED NICKEL	33	WRAP PKG  	1037.03	he packages? quickl
100038	navy wheat maroon moccasin indian	Manufacturer#3           	Brand#34  	SMALL BURNISHED BRASS	5	LG DRUM   	1038.03	y express depos
100042	khaki indian chiffon linen almond	Manufacturer#2           	Brand#22  	PROMO BURNISHED STEEL	12	MED PACK  	1042.04	e quickly. iron
100043	plum dodger powder thistle forest	Manufacturer#4           	Brand#45  	STANDARD PLATED COPPER	41	WRAP BAG  	1043.04	sts. pinto beans ha
100044	antique brown lavender red medium	Manufacturer#5           	Brand#53  	PROMO POLISHED STEEL	6	LG CAN    	1044.04	s dolphins affi
100049	sky moccasin cyan forest chartreuse	Manufacturer#4           	Brand#42  	LARGE POLISHED BRASS	30	WRAP DRUM 	1049.04	ly against
102471	lime linen salmon wheat cornsilk	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED BRASS	33	MED DRUM  	1473.47	luffily pending accou
102472	red goldenrod drab orange peru	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED COPPER	14	JUMBO CAN 	1474.47	ickly fina
102474	orange midnight navajo dim seashell	Manufacturer#4           	Brand#42  	STANDARD BURNISHED COPPER	47	JUMBO PKG 	1476.47	refully final
102478	green dark dodger sandy blanched	Manufacturer#1           	Brand#11  	STANDARD POLISHED NICKEL	23	WRAP PACK 	1480.47	 are in place of 
102481	magenta aquamarine pink coral ivory	Manufacturer#1           	Brand#12  	PROMO BURNISHED STEEL	9	SM PACK   	1483.48	c fox
102482	white azure snow lime sienna	Manufacturer#2           	Brand#25  	ECONOMY POLISHED BRASS	36	SM PKG    	1484.48	gle carefully. c
102484	medium chiffon sandy peru peach	Manufacturer#2           	Brand#21  	SMALL PLATED NICKEL	34	LG CAN    	1486.48	fluffily ironic
102489	turquoise rosy drab lavender coral	Manufacturer#2           	Brand#24  	PROMO BRUSHED STEEL	18	JUMBO CAN 	1491.48	ole slyly aga
102490	steel blanched dodger cream cornsilk	Manufacturer#5           	Brand#52  	SMALL POLISHED COPPER	43	MED PKG   	1492.49	ans integ
102491	deep beige almond red tan	Manufacturer#4           	Brand#42  	LARGE PLATED NICKEL	21	WRAP DRUM 	1493.49	 regular accounts l
102492	lace peru purple maroon forest	Manufacturer#2           	Brand#25  	STANDARD PLATED TIN	49	LG DRUM   	1494.49	kages. even packages a
102493	violet pale burnished white tomato	Manufacturer#5           	Brand#52  	LARGE PLATED TIN	20	SM BAG    	1495.49	struc
102497	navy floral grey chocolate midnight	Manufacturer#3           	Brand#32  	PROMO PLATED BRASS	10	JUMBO JAR 	1499.49	al pinto beans nag fl
102498	cornsilk snow indian bisque lemon	Manufacturer#2           	Brand#21  	STANDARD BRUSHED BRASS	6	WRAP CAN  	1500.49	ts: f
102502	deep burnished pink dark maroon	Manufacturer#5           	Brand#55  	LARGE BURNISHED NICKEL	10	MED CAN   	1504.50	c theodol
102503	azure purple chiffon coral green	Manufacturer#4           	Brand#44  	STANDARD POLISHED BRASS	1	SM BOX    	1505.50	 furiously even 
102506	frosted cream cornsilk midnight wheat	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED TIN	46	WRAP CASE 	1508.50	arefully. fluffily 
102507	papaya spring puff sandy azure	Manufacturer#3           	Brand#32  	ECONOMY ANODIZED TIN	30	MED DRUM  	1509.50	ar, pending packages 
102508	almond saddle slate beige magenta	Manufacturer#3           	Brand#35  	PROMO ANODIZED COPPER	1	WRAP PACK 	1510.50	iously
102512	dodger cornsilk thistle wheat drab	Manufacturer#4           	Brand#44  	LARGE ANODIZED STEEL	32	SM PKG    	1514.51	posits.
102513	ghost pale goldenrod azure coral	Manufacturer#3           	Brand#32  	LARGE PLATED BRASS	18	SM CAN    	1515.51	s. regul
102514	linen lemon grey almond sky	Manufacturer#2           	Brand#25  	PROMO BRUSHED COPPER	35	WRAP PACK 	1516.51	al excuses
102519	seashell burlywood linen maroon bisque	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED TIN	10	WRAP DRUM 	1521.51	 furiously 
104981	snow sienna lime medium navy	Manufacturer#2           	Brand#25  	SMALL POLISHED STEEL	1	MED CAN   	1985.98	es haggle. ironic, f
104982	moccasin mint orange metallic drab	Manufacturer#4           	Brand#42  	MEDIUM POLISHED COPPER	7	SM JAR    	1986.98	ular d
104984	medium pink khaki cornflower azure	Manufacturer#5           	Brand#51  	SMALL ANODIZED BRASS	38	LG BOX    	1988.98	ts boost dependencies
104988	black blue cyan plum dim	Manufacturer#3           	Brand#33  	MEDIUM BURNISHED BRASS	37	SM CASE   	1992.98	e slyly ev
104991	navy floral sky orange blush	Manufacturer#2           	Brand#21  	SMALL PLATED NICKEL	38	JUMBO DRUM	1995.99	hrash carefully.
104992	honeydew metallic chartreuse gainsboro smoke	Manufacturer#4           	Brand#42  	STANDARD POLISHED NICKEL	12	MED DRUM  	1996.99	blithely final pi
104994	dark khaki moccasin magenta blue	Manufacturer#3           	Brand#32  	LARGE ANODIZED STEEL	38	LG CAN    	1998.99	final package
104999	turquoise misty thistle antique lime	Manufacturer#2           	Brand#22  	STANDARD BRUSHED COPPER	23	SM CASE   	2003.99	riously a
105000	olive magenta rose pale lavender	Manufacturer#4           	Brand#43  	LARGE POLISHED TIN	15	LG DRUM   	1005.00	c accounts a
105001	floral lawn grey bisque hot	Manufacturer#5           	Brand#53  	MEDIUM PLATED TIN	36	SM JAR    	1006.00	even deposits wak
105002	misty coral lawn ivory rosy	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED COPPER	11	JUMBO BOX 	1007.00	s the c
105003	mint maroon green dodger salmon	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED COPPER	19	LG BAG    	1008.00	re fluffily regul
105007	metallic blanched lace ivory thistle	Manufacturer#3           	Brand#33  	SMALL ANODIZED BRASS	11	WRAP PACK 	1012.00	r accounts sleep 
105008	almond sandy salmon blush seashell	Manufacturer#2           	Brand#23  	LARGE PLATED STEEL	36	MED JAR   	1013.00	 acco
105012	rose magenta dim blue turquoise	Manufacturer#5           	Brand#54  	PROMO BURNISHED BRASS	12	MED CAN   	1017.01	ing depth
105013	deep dim forest ivory peach	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED NICKEL	36	JUMBO CASE	1018.01	sly final requests are
105016	black lace peru plum cyan	Manufacturer#2           	Brand#22  	ECONOMY PLATED TIN	7	JUMBO CASE	1021.01	nal pai
105017	burnished olive yellow sienna forest	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED TIN	24	JUMBO BAG 	1022.01	 asymptotes.
105018	orchid firebrick moccasin red maroon	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED COPPER	22	LG BAG    	1023.01	us accounts among t
105022	violet firebrick indian steel frosted	Manufacturer#2           	Brand#25  	MEDIUM POLISHED NICKEL	22	LG CAN    	1027.02	ly unusual depos
105023	white lace goldenrod spring frosted	Manufacturer#4           	Brand#42  	SMALL BURNISHED NICKEL	37	JUMBO JAR 	1028.02	ic package
105024	orange rose chocolate chartreuse misty	Manufacturer#3           	Brand#34  	SMALL BURNISHED STEEL	46	WRAP BOX  	1029.02	carefully with
105029	smoke pink coral magenta cream	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED TIN	27	SM CASE   	1034.02	efully r
107491	misty green papaya salmon red	Manufacturer#5           	Brand#53  	PROMO BURNISHED COPPER	37	LG CASE   	1498.49	carefully 
107492	ghost mint white blanched dim	Manufacturer#3           	Brand#34  	ECONOMY PLATED TIN	32	MED CAN   	1499.49	lets. idea
107494	metallic beige dim thistle magenta	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED COPPER	46	LG CAN    	1501.49	 beans haggle
107498	metallic burnished orchid firebrick white	Manufacturer#4           	Brand#43  	STANDARD BURNISHED STEEL	41	MED PACK  	1505.49	cial deposits are 
107501	red metallic lace gainsboro sienna	Manufacturer#3           	Brand#35  	PROMO BRUSHED BRASS	16	WRAP DRUM 	1508.50	 even, ex
107502	slate blanched lace spring lawn	Manufacturer#4           	Brand#43  	LARGE PLATED TIN	43	WRAP PKG  	1509.50	ful packages slee
107504	rose red papaya royal navajo	Manufacturer#3           	Brand#32  	LARGE BRUSHED NICKEL	17	MED BOX   	1511.50	ong the car
107509	sky pink azure coral purple	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED COPPER	20	LG PACK   	1516.50	rding to the final re
107510	midnight goldenrod sky brown blue	Manufacturer#2           	Brand#24  	PROMO BRUSHED BRASS	25	SM DRUM   	1517.51	lphins. packages en
107511	mint maroon firebrick misty pink	Manufacturer#5           	Brand#53  	SMALL ANODIZED TIN	22	JUMBO BOX 	1518.51	ar courts? special
107512	light frosted thistle midnight lime	Manufacturer#3           	Brand#33  	LARGE POLISHED NICKEL	13	LG CAN    	1519.51	uests. slyly brave 
107513	ivory burlywood chiffon sandy smoke	Manufacturer#1           	Brand#15  	LARGE BURNISHED STEEL	24	WRAP DRUM 	1520.51	; accounts 
107517	red blush yellow aquamarine lemon	Manufacturer#1           	Brand#14  	PROMO PLATED STEEL	9	JUMBO CAN 	1524.51	ly against the f
107518	green pink cornsilk smoke magenta	Manufacturer#5           	Brand#52  	ECONOMY POLISHED TIN	29	WRAP CASE 	1525.51	elets-- quick
107522	tomato dark slate almond lawn	Manufacturer#2           	Brand#24  	LARGE POLISHED COPPER	32	SM PACK   	1529.52	s packages alongside o
107523	burlywood forest rosy cream white	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED BRASS	36	JUMBO BAG 	1530.52	ress pinto be
107526	powder cornflower papaya blue sky	Manufacturer#4           	Brand#44  	MEDIUM POLISHED STEEL	28	SM DRUM   	1533.52	lar accounts ca
107527	turquoise khaki orange aquamarine yellow	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED TIN	24	JUMBO JAR 	1534.52	ts haggle blithely 
107528	plum smoke burlywood black burnished	Manufacturer#4           	Brand#44  	SMALL BURNISHED NICKEL	29	LG BOX    	1535.52	s caj
107532	tan papaya coral mint lime	Manufacturer#5           	Brand#51  	STANDARD ANODIZED NICKEL	49	MED CASE  	1539.53	thely slyly
107533	firebrick dark maroon purple tan	Manufacturer#3           	Brand#35  	PROMO POLISHED TIN	41	WRAP JAR  	1540.53	 packages cajo
107534	forest blanched pink antique black	Manufacturer#2           	Brand#21  	LARGE BRUSHED TIN	16	SM PKG    	1541.53	ests cajole about 
107539	dark cyan chartreuse dodger white	Manufacturer#3           	Brand#31  	STANDARD ANODIZED NICKEL	16	WRAP JAR  	1546.53	unts prom
110001	olive ivory thistle papaya tan	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED NICKEL	3	SM JAR    	1011.00	ly unusual theodoli
110002	lime seashell goldenrod deep peach	Manufacturer#1           	Brand#11  	STANDARD BURNISHED STEEL	6	LG PKG    	1012.00	fluffi
110004	chartreuse bisque coral rose thistle	Manufacturer#1           	Brand#14  	STANDARD PLATED TIN	25	SM JAR    	1014.00	 final dependencies de
110008	lawn pale floral violet chocolate	Manufacturer#4           	Brand#44  	SMALL BRUSHED NICKEL	21	JUMBO PKG 	1018.00	beans cajole sl
110011	burlywood hot gainsboro green tan	Manufacturer#3           	Brand#35  	LARGE POLISHED COPPER	34	JUMBO CAN 	1021.01	ake fluffily ste
110012	thistle olive navy plum dim	Manufacturer#4           	Brand#44  	PROMO BURNISHED NICKEL	47	WRAP DRUM 	1022.01	inal p
110014	wheat light medium mint coral	Manufacturer#1           	Brand#11  	MEDIUM ANODIZED COPPER	42	WRAP DRUM 	1024.01	gainst 
110019	drab lace forest cream sienna	Manufacturer#2           	Brand#23  	SMALL BURNISHED BRASS	10	MED CASE  	1029.01	. pending ideas haggl
110020	grey blush rose burnished pale	Manufacturer#3           	Brand#31  	PROMO POLISHED BRASS	33	WRAP BOX  	1030.02	d above th
110021	blue slate sandy aquamarine linen	Manufacturer#1           	Brand#12  	SMALL POLISHED COPPER	11	JUMBO BOX 	1031.02	ing to 
110022	khaki honeydew lace chiffon seashell	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED BRASS	17	SM PKG    	1032.02	accoun
110023	olive moccasin thistle powder magenta	Manufacturer#3           	Brand#33  	LARGE BURNISHED STEEL	47	LG PACK   	1033.02	cording to the iron
110027	azure ghost cornsilk salmon navy	Manufacturer#3           	Brand#32  	SMALL POLISHED NICKEL	20	WRAP PKG  	1037.02	uriously. unusu
110028	navajo snow seashell cornsilk pale	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	24	SM BOX    	1038.02	fter the f
110032	powder olive red azure dark	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED BRASS	15	WRAP CAN  	1042.03	 express foxes
110033	white frosted plum lemon chocolate	Manufacturer#4           	Brand#42  	LARGE BURNISHED COPPER	16	JUMBO CAN 	1043.03	lithe f
110036	antique tan papaya metallic chocolate	Manufacturer#1           	Brand#12  	PROMO BRUSHED BRASS	7	SM BOX    	1046.03	uring the quickly expr
110037	hot lawn khaki wheat blanched	Manufacturer#2           	Brand#24  	SMALL BURNISHED BRASS	34	WRAP JAR  	1047.03	 furiously exp
110038	midnight slate violet snow ghost	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED COPPER	49	LG BOX    	1048.03	ffily unusu
110042	burlywood hot navajo lawn blush	Manufacturer#5           	Brand#55  	STANDARD PLATED BRASS	3	SM BOX    	1052.04	ly. forges sleep bl
110043	royal powder firebrick cornsilk turquoise	Manufacturer#3           	Brand#31  	MEDIUM POLISHED NICKEL	45	MED DRUM  	1053.04	into beans are 
110044	maroon mint blanched salmon honeydew	Manufacturer#4           	Brand#41  	MEDIUM ANODIZED NICKEL	50	MED CAN   	1054.04	r deposits. e
110049	azure chocolate midnight salmon lime	Manufacturer#5           	Brand#55  	ECONOMY POLISHED TIN	11	LG PACK   	1059.04	ar ideas according 
112468	almond magenta pale moccasin floral	Manufacturer#5           	Brand#55  	MEDIUM PLATED BRASS	24	SM BOX    	1480.46	es wake blit
112469	light slate plum misty thistle	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED BRASS	38	WRAP CASE 	1481.46	ts. quietly ev
112471	purple royal tomato sky cyan	Manufacturer#4           	Brand#42  	MEDIUM POLISHED BRASS	43	LG BOX    	1483.47	 blithely bold the
112475	magenta blanched snow papaya antique	Manufacturer#3           	Brand#32  	PROMO ANODIZED TIN	5	LG PACK   	1487.47	accounts
112478	blue steel ghost spring yellow	Manufacturer#2           	Brand#21  	SMALL BRUSHED STEEL	15	LG CAN    	1490.47	tegrate carefully re
112479	sandy chiffon lime midnight frosted	Manufacturer#3           	Brand#31  	SMALL POLISHED STEEL	47	LG PACK   	1491.47	s haggle
112481	dark pale lime midnight pink	Manufacturer#3           	Brand#32  	SMALL BRUSHED BRASS	5	WRAP CAN  	1493.48	ns: carefully unusua
112486	hot smoke almond azure chocolate	Manufacturer#3           	Brand#33  	SMALL POLISHED BRASS	1	WRAP DRUM 	1498.48	counts. 
112487	frosted grey ghost floral cornsilk	Manufacturer#4           	Brand#44  	PROMO BRUSHED COPPER	30	SM PACK   	1499.48	y bold
112488	dim midnight goldenrod firebrick turquoise	Manufacturer#5           	Brand#53  	MEDIUM POLISHED COPPER	32	WRAP PACK 	1500.48	hinly fi
112489	indian yellow wheat chartreuse cream	Manufacturer#4           	Brand#44  	ECONOMY PLATED TIN	21	WRAP JAR  	1501.48	ccording to the sl
112490	salmon smoke rose midnight tomato	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED NICKEL	37	SM DRUM   	1502.49	ajole
112494	purple olive steel lemon bisque	Manufacturer#5           	Brand#54  	MEDIUM PLATED NICKEL	24	JUMBO BAG 	1506.49	ve the carefully fina
112495	frosted powder lawn chartreuse ivory	Manufacturer#2           	Brand#24  	LARGE ANODIZED COPPER	20	LG JAR    	1507.49	 final excus
112499	drab turquoise saddle khaki burlywood	Manufacturer#1           	Brand#12  	STANDARD BURNISHED TIN	14	LG CASE   	1511.49	l reques
112500	beige misty azure plum powder	Manufacturer#1           	Brand#14  	PROMO ANODIZED NICKEL	20	LG DRUM   	1512.50	eans about the 
112503	cream papaya dim cornflower pale	Manufacturer#5           	Brand#51  	LARGE BURNISHED COPPER	35	JUMBO CAN 	1515.50	lar requests us
112504	tomato midnight burlywood lemon maroon	Manufacturer#2           	Brand#21  	PROMO PLATED TIN	45	MED PKG   	1516.50	onic, pending epita
112505	seashell almond smoke misty rose	Manufacturer#2           	Brand#24  	STANDARD BURNISHED COPPER	19	SM CASE   	1517.50	ons acro
112509	burnished mint spring rosy chiffon	Manufacturer#1           	Brand#14  	LARGE ANODIZED STEEL	40	JUMBO DRUM	1521.50	ar ideas are
112510	powder chiffon gainsboro spring royal	Manufacturer#3           	Brand#31  	LARGE ANODIZED TIN	1	WRAP BOX  	1522.51	cial asymptot
112511	peru burnished indian spring gainsboro	Manufacturer#5           	Brand#51  	STANDARD BRUSHED NICKEL	23	MED BAG   	1523.51	 ironic fo
112516	frosted almond lawn orchid honeydew	Manufacturer#2           	Brand#23  	PROMO BURNISHED BRASS	17	JUMBO PKG 	1528.51	regular request
114979	beige steel aquamarine tan blush	Manufacturer#3           	Brand#33  	PROMO PLATED TIN	31	SM PACK   	1993.97	. special courts kindl
114980	plum metallic indian snow bisque	Manufacturer#3           	Brand#33  	SMALL BRUSHED NICKEL	42	MED CAN   	1994.98	to beans boost slyl
114982	thistle linen midnight navy honeydew	Manufacturer#4           	Brand#45  	ECONOMY PLATED BRASS	39	LG CAN    	1996.98	 regular packages.
114986	dim firebrick seashell sienna lavender	Manufacturer#4           	Brand#41  	LARGE BRUSHED TIN	46	LG BOX    	2000.98	l the slyly regula
114989	hot pink smoke blush navajo	Manufacturer#5           	Brand#54  	ECONOMY POLISHED STEEL	21	JUMBO JAR 	2003.98	ges solve furiousl
114990	moccasin burlywood powder indian dark	Manufacturer#1           	Brand#11  	LARGE POLISHED STEEL	14	LG JAR    	2004.99	sly unusual theo
114992	sky cyan wheat cornflower aquamarine	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED STEEL	15	WRAP JAR  	2006.99	fily. carefull
114997	papaya goldenrod beige rosy khaki	Manufacturer#4           	Brand#41  	STANDARD BRUSHED STEEL	30	LG BAG    	2011.99	fully unusua
114998	rose bisque chartreuse turquoise lime	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED BRASS	37	WRAP DRUM 	2012.99	fluffy re
114999	firebrick tomato yellow cornflower royal	Manufacturer#3           	Brand#34  	LARGE ANODIZED STEEL	38	LG CASE   	2013.99	 pack
115000	metallic cream medium blue red	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED TIN	29	SM CAN    	1015.00	ly. ironic accounts d
115001	chocolate powder ivory blue hot	Manufacturer#2           	Brand#25  	ECONOMY PLATED STEEL	50	LG PACK   	1016.00	! furiously final re
115005	beige ivory smoke turquoise dodger	Manufacturer#1           	Brand#15  	SMALL POLISHED NICKEL	43	JUMBO PKG 	1020.00	uctions wake furiousl
115006	olive floral brown rose sky	Manufacturer#2           	Brand#25  	SMALL BURNISHED BRASS	14	JUMBO JAR 	1021.00	ests integrate
115010	salmon turquoise rosy chocolate white	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED COPPER	44	MED PKG   	1025.01	ccounts. c
115011	peach salmon coral violet cornsilk	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED COPPER	39	LG BAG    	1026.01	leep carefully. accou
115014	metallic grey navajo medium orange	Manufacturer#5           	Brand#52  	LARGE PLATED NICKEL	33	MED CAN   	1029.01	deas hagg
115015	purple deep khaki magenta steel	Manufacturer#4           	Brand#43  	STANDARD PLATED NICKEL	30	LG DRUM   	1030.01	final pinto
115016	blue light burlywood orchid khaki	Manufacturer#3           	Brand#35  	LARGE BRUSHED BRASS	4	JUMBO CAN 	1031.01	 expre
115020	wheat khaki violet medium chartreuse	Manufacturer#2           	Brand#25  	STANDARD POLISHED TIN	31	SM CAN    	1035.02	arefully pending pac
115021	firebrick seashell lavender purple chartreuse	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED TIN	6	LG BAG    	1036.02	fily. fluffily bold pa
115022	pink orange mint coral yellow	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED NICKEL	46	MED CASE  	1037.02	ter the blithely
115027	peach beige misty pale saddle	Manufacturer#5           	Brand#54  	STANDARD PLATED TIN	4	LG PACK   	1042.02	ironic pa
117490	goldenrod tomato almond cream salmon	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED BRASS	24	LG JAR    	1507.49	osely even acco
117491	plum sky olive antique lavender	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	42	LG BAG    	1508.49	t the account
117493	antique navajo dodger burlywood lawn	Manufacturer#4           	Brand#42  	LARGE ANODIZED NICKEL	28	WRAP PKG  	1510.49	efully after
117497	azure grey metallic honeydew navy	Manufacturer#4           	Brand#45  	SMALL BRUSHED TIN	35	JUMBO PKG 	1514.49	ending instruct
117500	blush bisque peru peach khaki	Manufacturer#2           	Brand#25  	LARGE BURNISHED NICKEL	31	MED CAN   	1517.50	 final fret
117501	drab sienna azure puff navy	Manufacturer#1           	Brand#12  	PROMO BURNISHED COPPER	42	JUMBO JAR 	1518.50	thely along the fluff
117503	cream mint rose plum rosy	Manufacturer#4           	Brand#42  	STANDARD POLISHED TIN	7	SM PKG    	1520.50	sts sleep acr
117508	slate lemon navy salmon thistle	Manufacturer#1           	Brand#11  	LARGE BURNISHED TIN	42	WRAP PACK 	1525.50	tes wake si
117509	chocolate snow mint papaya tomato	Manufacturer#1           	Brand#14  	PROMO POLISHED COPPER	12	MED CASE  	1526.50	sleep furiously
117510	almond powder puff firebrick plum	Manufacturer#2           	Brand#21  	PROMO ANODIZED STEEL	21	WRAP CASE 	1527.51	tect about the blit
117511	metallic smoke dark deep light	Manufacturer#1           	Brand#14  	LARGE POLISHED TIN	49	MED PACK  	1528.51	xes. slyly specia
117512	orchid goldenrod dodger linen sienna	Manufacturer#4           	Brand#44  	MEDIUM PLATED TIN	33	MED JAR   	1529.51	lyly unusua
117516	blanched maroon puff green turquoise	Manufacturer#3           	Brand#33  	STANDARD ANODIZED BRASS	21	LG DRUM   	1533.51	leep furiously along 
117517	pale dark brown cornsilk violet	Manufacturer#5           	Brand#51  	STANDARD ANODIZED STEEL	22	LG PACK   	1534.51	eodolites. e
117521	puff rose coral floral sandy	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED BRASS	5	SM JAR    	1538.52	idly after th
117522	sky tomato frosted smoke thistle	Manufacturer#5           	Brand#53  	PROMO PLATED NICKEL	7	LG PACK   	1539.52	nding deposits. care
117525	burnished honeydew dark red cyan	Manufacturer#1           	Brand#12  	LARGE BRUSHED TIN	40	MED PKG   	1542.52	al pinto beans dete
117526	metallic navy red indian cornflower	Manufacturer#3           	Brand#35  	ECONOMY BRUSHED NICKEL	25	SM PACK   	1543.52	ng the p
117527	cyan cornflower metallic cornsilk bisque	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	36	MED JAR   	1544.52	egular 
117531	drab peru dim floral puff	Manufacturer#2           	Brand#22  	ECONOMY BRUSHED BRASS	9	MED DRUM  	1548.53	y. unusual
117532	powder blue sandy white plum	Manufacturer#4           	Brand#43  	SMALL PLATED STEEL	6	MED PACK  	1549.53	permanently pending
117533	floral cream lawn salmon burlywood	Manufacturer#4           	Brand#42  	PROMO ANODIZED NICKEL	42	MED BOX   	1550.53	d slowly across 
117538	snow goldenrod smoke bisque turquoise	Manufacturer#2           	Brand#22  	STANDARD BRUSHED STEEL	8	JUMBO PACK	1555.53	e requests nag r
120001	yellow hot rose blue green	Manufacturer#1           	Brand#15  	STANDARD ANODIZED STEEL	38	SM BAG    	1021.00	he unusual requests.
120002	yellow pale blanched gainsboro moccasin	Manufacturer#4           	Brand#45  	STANDARD PLATED BRASS	22	WRAP CAN  	1022.00	e furiously even ex
120004	purple cream puff royal chocolate	Manufacturer#2           	Brand#24  	SMALL ANODIZED TIN	41	MED BAG   	1024.00	ly final
120008	forest rose orchid mint lime	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	5	JUMBO BAG 	1028.00	blithely slyly pend
120011	sandy medium red rose coral	Manufacturer#3           	Brand#32  	MEDIUM POLISHED BRASS	18	LG JAR    	1031.01	 across the bl
120012	violet smoke khaki salmon tan	Manufacturer#1           	Brand#13  	LARGE ANODIZED BRASS	24	SM BAG    	1032.01	fluffily. ironic 
120014	papaya white ghost blue aquamarine	Manufacturer#2           	Brand#23  	SMALL BURNISHED BRASS	31	SM CAN    	1034.01	es sleep. specia
120019	lime antique rose seashell magenta	Manufacturer#1           	Brand#13  	STANDARD ANODIZED BRASS	45	JUMBO BOX 	1039.01	nstructions cajole
120020	tomato frosted pink purple almond	Manufacturer#3           	Brand#33  	STANDARD POLISHED COPPER	34	JUMBO DRUM	1040.02	attainments
120021	salmon cream deep hot white	Manufacturer#4           	Brand#44  	STANDARD ANODIZED COPPER	14	WRAP BOX  	1041.02	s. quickly regular 
120022	deep lemon light frosted peach	Manufacturer#2           	Brand#25  	STANDARD BRUSHED TIN	16	MED BOX   	1042.02	 the 
120023	light burlywood ivory midnight lime	Manufacturer#1           	Brand#14  	PROMO POLISHED STEEL	45	SM PKG    	1043.02	accounts h
120027	beige grey honeydew coral burlywood	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	35	MED CASE  	1047.02	he ironic reques
120028	rose floral forest lemon misty	Manufacturer#1           	Brand#15  	MEDIUM PLATED TIN	39	SM BOX    	1048.02	quests x-ray c
120032	burnished blue dim cornflower deep	Manufacturer#1           	Brand#11  	MEDIUM POLISHED TIN	2	LG DRUM   	1052.03	usly even theodolites
120033	puff pale metallic cream red	Manufacturer#1           	Brand#13  	SMALL POLISHED COPPER	27	MED DRUM  	1053.03	inal, ironic ins
120036	violet thistle olive sandy indian	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED TIN	5	SM CAN    	1056.03	ng accounts run un
120037	violet royal tan papaya indian	Manufacturer#3           	Brand#35  	STANDARD BRUSHED STEEL	6	LG PKG    	1057.03	 accounts. 
120038	puff firebrick spring peru cornflower	Manufacturer#2           	Brand#25  	LARGE BRUSHED COPPER	17	JUMBO JAR 	1058.03	 dolphins detect a
120042	cornsilk red slate plum misty	Manufacturer#4           	Brand#44  	PROMO BURNISHED BRASS	4	SM BAG    	1062.04	wake r
120043	grey bisque pink honeydew papaya	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED TIN	1	MED JAR   	1063.04	l packages. bus
120044	thistle magenta burnished blue burlywood	Manufacturer#1           	Brand#13  	PROMO PLATED BRASS	11	WRAP JAR  	1064.04	nt ideas. daringly e
120049	firebrick rose magenta black gainsboro	Manufacturer#4           	Brand#42  	STANDARD BURNISHED COPPER	48	WRAP CAN  	1069.04	uriou
122465	dodger lavender chiffon burnished snow	Manufacturer#3           	Brand#32  	LARGE ANODIZED TIN	38	WRAP CASE 	1487.46	the furious
122466	chiffon forest orange violet frosted	Manufacturer#2           	Brand#23  	SMALL BRUSHED BRASS	25	WRAP BOX  	1488.46	 accounts haggle
122468	blush orange plum royal olive	Manufacturer#4           	Brand#42  	SMALL ANODIZED NICKEL	7	MED CASE  	1490.46	 stealthily regular
122472	grey lemon cornflower deep orange	Manufacturer#3           	Brand#35  	MEDIUM PLATED TIN	30	LG CAN    	1494.47	unts impres
122475	misty forest tan powder lemon	Manufacturer#5           	Brand#51  	ECONOMY BRUSHED BRASS	11	SM CASE   	1497.47	posits. fluffil
122476	tomato sandy steel blue violet	Manufacturer#4           	Brand#43  	STANDARD BURNISHED TIN	21	WRAP PKG  	1498.47	thy packages nag furi
122478	navajo turquoise black cyan orange	Manufacturer#3           	Brand#32  	SMALL BURNISHED NICKEL	9	WRAP CAN  	1500.47	tes haggle
122483	metallic dark chocolate powder violet	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED TIN	17	LG BAG    	1505.48	e blithel
122484	slate lavender beige pink tomato	Manufacturer#4           	Brand#44  	STANDARD PLATED COPPER	24	WRAP DRUM 	1506.48	y ironic instruct
122485	navy brown olive navajo bisque	Manufacturer#1           	Brand#12  	STANDARD BRUSHED TIN	46	JUMBO BOX 	1507.48	accounts wake de
122486	azure cream aquamarine tomato medium	Manufacturer#5           	Brand#51  	LARGE BURNISHED TIN	7	JUMBO BOX 	1508.48	eep furi
122487	cornsilk misty drab peru tomato	Manufacturer#2           	Brand#23  	SMALL POLISHED STEEL	8	JUMBO DRUM	1509.48	 slyly final depths.
122491	cornsilk wheat red rosy ivory	Manufacturer#3           	Brand#34  	STANDARD BRUSHED NICKEL	27	SM PKG    	1513.49	s. quic
122492	chocolate aquamarine bisque burnished lime	Manufacturer#3           	Brand#31  	PROMO ANODIZED BRASS	38	MED BAG   	1514.49	gularly special ideas
122496	dodger misty pink plum snow	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED STEEL	21	LG PACK   	1518.49	ously b
122497	goldenrod cyan burlywood smoke metallic	Manufacturer#4           	Brand#42  	MEDIUM POLISHED BRASS	16	WRAP DRUM 	1519.49	platelets haggle b
122500	papaya peach yellow slate goldenrod	Manufacturer#3           	Brand#35  	LARGE POLISHED STEEL	47	MED BOX   	1522.50	ggle blit
122501	peru spring almond deep antique	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED TIN	20	JUMBO JAR 	1523.50	rns against the 
122502	metallic salmon navy dodger papaya	Manufacturer#3           	Brand#33  	LARGE PLATED COPPER	15	SM CASE   	1524.50	nic braids haggl
122506	magenta coral ivory gainsboro orchid	Manufacturer#2           	Brand#23  	ECONOMY POLISHED NICKEL	32	LG PACK   	1528.50	furiously regula
122507	maroon white rose ivory indian	Manufacturer#5           	Brand#51  	STANDARD BURNISHED COPPER	49	JUMBO BOX 	1529.50	y ironi
122508	burnished drab indian honeydew puff	Manufacturer#4           	Brand#44  	SMALL POLISHED BRASS	16	MED CASE  	1530.50	to the regular ac
122513	smoke firebrick orchid honeydew khaki	Manufacturer#4           	Brand#45  	SMALL POLISHED TIN	23	LG BAG    	1535.51	efully even as
124977	olive azure hot mint lavender	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	23	LG JAR    	2001.97	otes slee
124978	violet tan midnight black wheat	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED TIN	18	MED PKG   	2002.97	nts boost s
124980	red mint navajo black burnished	Manufacturer#1           	Brand#15  	PROMO BRUSHED STEEL	41	JUMBO DRUM	2004.98	en pinto beans. 
124984	almond cream linen navajo rosy	Manufacturer#5           	Brand#52  	PROMO BRUSHED NICKEL	37	SM PACK   	2008.98	ironic 
124987	smoke azure pale beige ghost	Manufacturer#2           	Brand#21  	STANDARD PLATED STEEL	5	SM BOX    	2011.98	ess acco
124988	grey green turquoise navajo blanched	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED TIN	42	JUMBO DRUM	2012.98	tegra
124990	thistle blue snow yellow chiffon	Manufacturer#1           	Brand#12  	STANDARD BURNISHED BRASS	34	JUMBO PACK	2014.99	uickly carefully
124995	wheat green cyan papaya navajo	Manufacturer#5           	Brand#54  	PROMO POLISHED TIN	41	LG CASE   	2019.99	lly around the regul
124996	forest plum blanched bisque grey	Manufacturer#3           	Brand#31  	STANDARD POLISHED COPPER	42	LG CAN    	2020.99	hely final accoun
124997	olive dim indian chartreuse saddle	Manufacturer#3           	Brand#33  	SMALL ANODIZED BRASS	39	MED CASE  	2021.99	usly i
124998	dodger tan almond sky seashell	Manufacturer#1           	Brand#14  	SMALL PLATED STEEL	34	WRAP DRUM 	2022.99	he fluf
124999	cornsilk hot puff sandy indian	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED BRASS	24	LG CASE   	2023.99	es according to the 
125003	blanched dodger orange khaki tomato	Manufacturer#4           	Brand#41  	SMALL ANODIZED BRASS	34	SM BAG    	1028.00	ent p
125004	papaya black misty aquamarine ghost	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED COPPER	26	WRAP BOX  	1029.00	l, regular deposits wa
125008	firebrick sky midnight smoke antique	Manufacturer#4           	Brand#44  	SMALL BRUSHED STEEL	21	MED CASE  	1033.00	mong the 
125009	dodger sienna orange khaki lace	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED STEEL	13	LG BAG    	1034.00	regular pack
125012	rosy mint sky medium forest	Manufacturer#3           	Brand#34  	LARGE BRUSHED STEEL	22	SM BOX    	1037.01	heodolites boos
125013	firebrick green cornflower salmon powder	Manufacturer#2           	Brand#25  	STANDARD POLISHED COPPER	18	LG PACK   	1038.01	ual in
125014	olive tan peru yellow antique	Manufacturer#3           	Brand#33  	LARGE BRUSHED COPPER	15	LG BAG    	1039.01	ons sleep final dep
125018	navy plum blue wheat saddle	Manufacturer#2           	Brand#24  	SMALL POLISHED TIN	28	JUMBO BOX 	1043.01	s. bold dolph
125019	dodger yellow forest cyan lace	Manufacturer#2           	Brand#24  	LARGE BURNISHED STEEL	3	JUMBO PACK	1044.01	ke. care
125020	forest metallic papaya grey rosy	Manufacturer#2           	Brand#25  	LARGE BRUSHED BRASS	25	MED BOX   	1045.02	silent packages bo
125025	antique beige tan slate hot	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	27	SM DRUM   	1050.02	players. quickly fi
127489	lime royal beige cream sky	Manufacturer#1           	Brand#15  	PROMO POLISHED STEEL	45	SM BAG    	1516.48	nts hag
127490	cyan firebrick deep turquoise seashell	Manufacturer#2           	Brand#21  	PROMO BURNISHED TIN	6	WRAP JAR  	1517.49	ake blithely. pinto b
127492	smoke chocolate bisque royal lavender	Manufacturer#2           	Brand#24  	STANDARD ANODIZED TIN	31	WRAP JAR  	1519.49	ites cajole fur
127496	linen sandy lemon olive tomato	Manufacturer#2           	Brand#25  	MEDIUM PLATED TIN	26	SM DRUM   	1523.49	ckages. s
127499	red white forest maroon cornsilk	Manufacturer#5           	Brand#53  	SMALL BRUSHED BRASS	36	SM BOX    	1526.49	refully regul
127500	chartreuse coral lavender misty blush	Manufacturer#5           	Brand#52  	STANDARD BRUSHED BRASS	38	WRAP BAG  	1527.50	es ki
127502	green lawn slate mint bisque	Manufacturer#5           	Brand#54  	SMALL POLISHED STEEL	11	SM BOX    	1529.50	ffily final courts. 
127507	misty pink deep beige brown	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED TIN	6	WRAP BOX  	1534.50	into beans int
127508	firebrick grey drab slate cyan	Manufacturer#3           	Brand#35  	STANDARD BURNISHED COPPER	27	WRAP PACK 	1535.50	l instructio
127509	cornflower lavender bisque pink tan	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED BRASS	22	JUMBO DRUM	1536.50	s are 
127510	chocolate dodger almond pink cream	Manufacturer#3           	Brand#35  	LARGE ANODIZED BRASS	30	SM BAG    	1537.51	heodolites boost
127511	magenta midnight olive snow cyan	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED NICKEL	21	JUMBO JAR 	1538.51	ke furiously blit
127515	blush orange gainsboro cyan spring	Manufacturer#1           	Brand#11  	SMALL POLISHED COPPER	44	WRAP BOX  	1542.51	symptotes. 
127516	coral dim chartreuse bisque olive	Manufacturer#4           	Brand#45  	PROMO BURNISHED TIN	39	JUMBO DRUM	1543.51	g pinto beans. fluff
127520	chartreuse floral cyan sandy plum	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	1	WRAP BAG  	1547.52	quests boos
127521	moccasin gainsboro burlywood magenta sandy	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED TIN	30	JUMBO BAG 	1548.52	 cajole blithely spe
127524	red violet magenta peach cream	Manufacturer#3           	Brand#34  	MEDIUM PLATED COPPER	14	SM BAG    	1551.52	nding dependencie
127525	snow lime tan turquoise lawn	Manufacturer#2           	Brand#21  	SMALL PLATED BRASS	26	MED BOX   	1552.52	s instructions. carefu
127526	brown olive sienna rose slate	Manufacturer#2           	Brand#21  	STANDARD PLATED NICKEL	26	LG JAR    	1553.52	accounts print pint
127530	powder drab magenta lemon saddle	Manufacturer#2           	Brand#22  	STANDARD ANODIZED TIN	35	MED DRUM  	1557.53	s. special, unusual b
127531	salmon spring beige metallic navy	Manufacturer#5           	Brand#54  	PROMO POLISHED COPPER	17	WRAP BAG  	1558.53	ly. ironic re
127532	almond blue lavender drab lemon	Manufacturer#2           	Brand#23  	STANDARD POLISHED NICKEL	19	JUMBO BOX 	1559.53	lly silent ideas alon
127537	cream beige thistle powder azure	Manufacturer#3           	Brand#34  	LARGE ANODIZED NICKEL	30	SM BOX    	1564.53	bove the careful
130001	beige ivory red tan burlywood	Manufacturer#4           	Brand#44  	PROMO ANODIZED BRASS	50	WRAP PKG  	1031.00	o beans ha
130002	bisque smoke spring black wheat	Manufacturer#4           	Brand#41  	PROMO POLISHED BRASS	10	WRAP PACK 	1032.00	ully ironic deposits 
130004	rosy medium puff tan slate	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED NICKEL	44	MED DRUM  	1034.00	e slyly quickly i
130008	lemon goldenrod grey navy dodger	Manufacturer#4           	Brand#42  	ECONOMY POLISHED STEEL	43	LG PKG    	1038.00	e regular asy
130011	turquoise midnight spring cornflower floral	Manufacturer#4           	Brand#43  	LARGE PLATED COPPER	8	JUMBO DRUM	1041.01	regular theodo
130012	indian slate lime moccasin dodger	Manufacturer#2           	Brand#22  	SMALL PLATED NICKEL	48	SM CAN    	1042.01	he thinly special requ
130014	azure hot navajo seashell red	Manufacturer#3           	Brand#34  	STANDARD POLISHED TIN	24	WRAP BAG  	1044.01	eans sleep f
130019	plum bisque violet red dark	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED BRASS	30	WRAP JAR  	1049.01	ideas are fluffi
130020	ghost dark chiffon royal burlywood	Manufacturer#3           	Brand#33  	STANDARD BURNISHED TIN	49	JUMBO DRUM	1050.02	nal requests sl
130021	royal olive dodger orchid misty	Manufacturer#5           	Brand#53  	LARGE BRUSHED NICKEL	35	LG CAN    	1051.02	the blithely fina
130022	red steel antique midnight burlywood	Manufacturer#2           	Brand#21  	STANDARD PLATED BRASS	38	WRAP BAG  	1052.02	sly quickly 
130023	frosted dim orange seashell coral	Manufacturer#4           	Brand#41  	LARGE BURNISHED BRASS	13	MED PACK  	1053.02	lent, 
130027	dodger blue tan green lemon	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	4	LG PKG    	1057.02	pending depo
130028	blanched saddle chiffon purple chartreuse	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED BRASS	42	LG BOX    	1058.02	e final patterns. 
130032	floral sienna turquoise red forest	Manufacturer#5           	Brand#52  	STANDARD BRUSHED TIN	47	SM BOX    	1062.03	ins run fluf
130033	thistle cornflower seashell bisque puff	Manufacturer#5           	Brand#53  	STANDARD POLISHED BRASS	5	JUMBO DRUM	1063.03	fter the
130036	floral lawn coral blanched misty	Manufacturer#5           	Brand#54  	LARGE BRUSHED NICKEL	33	MED CAN   	1066.03	ronic foxes 
130037	almond orange sky lawn honeydew	Manufacturer#3           	Brand#34  	PROMO POLISHED TIN	45	SM PACK   	1067.03	 ideas. b
130038	brown chocolate wheat salmon puff	Manufacturer#2           	Brand#23  	STANDARD BRUSHED TIN	14	SM CAN    	1068.03	re slyly against th
130042	lemon dim mint orchid violet	Manufacturer#3           	Brand#34  	SMALL ANODIZED TIN	37	SM BAG    	1072.04	lent ac
130043	blue royal thistle burnished sky	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED COPPER	37	LG PKG    	1073.04	blithely b
130044	maroon chiffon dim metallic sandy	Manufacturer#2           	Brand#21  	STANDARD BRUSHED COPPER	46	LG BAG    	1074.04	 express dugo
130049	frosted turquoise snow blue cornflower	Manufacturer#1           	Brand#14  	ECONOMY POLISHED BRASS	30	MED CASE  	1079.04	 careful
132462	lemon moccasin grey chocolate ghost	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED COPPER	16	WRAP CASE 	1494.46	kly busy dependen
132463	honeydew powder thistle metallic blue	Manufacturer#4           	Brand#42  	SMALL POLISHED COPPER	22	JUMBO CASE	1495.46	 blithely package
132465	rosy indian cyan coral snow	Manufacturer#1           	Brand#12  	LARGE PLATED STEEL	20	LG CASE   	1497.46	ly even de
132469	medium bisque rosy linen maroon	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED STEEL	50	JUMBO JAR 	1501.46	ggle. regu
132472	tomato saddle brown cornsilk khaki	Manufacturer#3           	Brand#31  	ECONOMY POLISHED STEEL	2	WRAP DRUM 	1504.47	as. furiously ironic p
132473	papaya floral pale coral forest	Manufacturer#1           	Brand#15  	LARGE ANODIZED STEEL	9	MED BOX   	1505.47	theodolites
132475	turquoise cornsilk linen antique brown	Manufacturer#4           	Brand#41  	LARGE BRUSHED BRASS	10	SM BAG    	1507.47	 bold pint
132480	misty coral burlywood blush blue	Manufacturer#2           	Brand#23  	PROMO BURNISHED STEEL	10	JUMBO PACK	1512.48	along the silent, 
132481	sandy moccasin lemon salmon orchid	Manufacturer#2           	Brand#25  	PROMO ANODIZED COPPER	15	WRAP JAR  	1513.48	. furiously
132482	frosted lavender medium cream indian	Manufacturer#1           	Brand#11  	PROMO PLATED BRASS	27	JUMBO DRUM	1514.48	l excuses nag blithe
132483	lawn mint chiffon medium coral	Manufacturer#5           	Brand#53  	STANDARD POLISHED NICKEL	18	SM CASE   	1515.48	es are 
132484	sandy sienna saddle goldenrod peru	Manufacturer#3           	Brand#32  	PROMO BRUSHED TIN	34	MED BAG   	1516.48	ly above the pend
132488	peru saddle lime midnight magenta	Manufacturer#5           	Brand#51  	PROMO BRUSHED STEEL	38	LG PACK   	1520.48	 care
132489	pink yellow grey burlywood seashell	Manufacturer#2           	Brand#24  	STANDARD ANODIZED BRASS	39	WRAP BAG  	1521.48	quests doze? u
132493	royal green floral medium linen	Manufacturer#2           	Brand#21  	STANDARD BURNISHED COPPER	49	MED BAG   	1525.49	eans. Tire
132494	pale almond cream azure dark	Manufacturer#2           	Brand#22  	SMALL ANODIZED BRASS	14	WRAP BOX  	1526.49	e final
132497	blush cyan lemon red sky	Manufacturer#1           	Brand#15  	STANDARD POLISHED TIN	9	WRAP CASE 	1529.49	along the
132498	sienna plum chocolate moccasin smoke	Manufacturer#2           	Brand#21  	SMALL BRUSHED COPPER	38	WRAP PACK 	1530.49	slyly special
132499	pink lime frosted white dim	Manufacturer#3           	Brand#35  	PROMO BURNISHED BRASS	9	WRAP PACK 	1531.49	furiously quickly 
132503	olive lime dim ghost lawn	Manufacturer#2           	Brand#24  	SMALL POLISHED NICKEL	7	JUMBO BOX 	1535.50	er the bl
132504	drab burnished blush pink magenta	Manufacturer#4           	Brand#43  	LARGE BRUSHED NICKEL	23	JUMBO CAN 	1536.50	c reque
132505	lawn azure aquamarine ghost green	Manufacturer#3           	Brand#32  	ECONOMY ANODIZED NICKEL	31	SM DRUM   	1537.50	le furi
132510	gainsboro midnight azure goldenrod peach	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED TIN	6	MED CASE  	1542.51	s against the fluffi
134975	goldenrod navajo black plum cornflower	Manufacturer#4           	Brand#45  	LARGE POLISHED TIN	44	SM JAR    	2009.97	ully final ex
134976	burnished gainsboro seashell navajo peru	Manufacturer#4           	Brand#44  	STANDARD BURNISHED COPPER	23	WRAP CAN  	2010.97	eposi
134978	beige plum cornsilk honeydew navajo	Manufacturer#2           	Brand#21  	STANDARD PLATED NICKEL	38	JUMBO CAN 	2012.97	usly slyly pendi
134982	blush azure blanched smoke slate	Manufacturer#1           	Brand#12  	MEDIUM PLATED NICKEL	45	LG BOX    	2016.98	 carefully ir
134985	light thistle midnight purple lavender	Manufacturer#5           	Brand#54  	LARGE PLATED COPPER	45	MED BOX   	2019.98	g requ
134986	spring ivory burlywood peru indian	Manufacturer#2           	Brand#21  	STANDARD BURNISHED BRASS	42	LG BAG    	2020.98	 affix 
134988	seashell lemon burnished chiffon navy	Manufacturer#2           	Brand#25  	LARGE BURNISHED TIN	44	JUMBO PKG 	2022.98	instruc
134993	dodger purple slate drab cornflower	Manufacturer#2           	Brand#24  	PROMO PLATED STEEL	26	LG PKG    	2027.99	le. furiously fi
134994	dark mint grey rosy firebrick	Manufacturer#2           	Brand#21  	LARGE PLATED TIN	44	JUMBO BAG 	2028.99	s wake carefully fu
134995	cream drab salmon navy lemon	Manufacturer#5           	Brand#55  	SMALL PLATED NICKEL	30	WRAP BOX  	2029.99	tes across the brave t
134996	cyan tan plum cornflower red	Manufacturer#3           	Brand#34  	ECONOMY POLISHED NICKEL	13	WRAP BOX  	2030.99	erve about the 
134997	green goldenrod pink aquamarine salmon	Manufacturer#2           	Brand#22  	LARGE PLATED STEEL	14	JUMBO DRUM	2031.99	ggle blithely ag
135001	orange chiffon cream moccasin light	Manufacturer#2           	Brand#22  	MEDIUM POLISHED TIN	13	MED BAG   	1036.00	g somas about the
135002	navy salmon slate midnight magenta	Manufacturer#3           	Brand#32  	PROMO ANODIZED NICKEL	14	MED BOX   	1037.00	 the slyly re
135006	drab tan metallic misty dim	Manufacturer#2           	Brand#21  	MEDIUM PLATED BRASS	46	WRAP CASE 	1041.00	blithely. 
135007	burnished frosted red ghost maroon	Manufacturer#2           	Brand#24  	MEDIUM BURNISHED NICKEL	5	MED BAG   	1042.00	ent e
135010	olive puff blanched almond seashell	Manufacturer#4           	Brand#41  	LARGE POLISHED STEEL	38	MED PACK  	1045.01	uffily p
135011	navajo light white puff red	Manufacturer#3           	Brand#35  	SMALL POLISHED TIN	8	JUMBO BAG 	1046.01	efully even accounts. 
135012	gainsboro burnished navy seashell mint	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED COPPER	46	MED CASE  	1047.01	ites. even, dogged id
135016	forest dim sandy medium lime	Manufacturer#2           	Brand#22  	LARGE ANODIZED NICKEL	20	LG CAN    	1051.01	final accounts. furiou
135017	navy sienna coral gainsboro violet	Manufacturer#1           	Brand#11  	ECONOMY POLISHED NICKEL	23	WRAP CAN  	1052.01	en requ
135018	khaki slate pink almond smoke	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED NICKEL	31	JUMBO PACK	1053.01	wake slyl
135023	light olive chartreuse burlywood puff	Manufacturer#5           	Brand#54  	MEDIUM BRUSHED TIN	11	LG DRUM   	1058.02	efully express th
137488	chocolate plum pale cornflower hot	Manufacturer#5           	Brand#55  	LARGE ANODIZED NICKEL	9	SM BAG    	1525.48	furiousl
137489	moccasin maroon violet firebrick peru	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	6	LG CASE   	1526.48	thely even depend
137491	antique snow sandy yellow deep	Manufacturer#4           	Brand#45  	SMALL ANODIZED BRASS	50	LG CAN    	1528.49	 frays! fur
137495	steel light lace lime royal	Manufacturer#4           	Brand#45  	MEDIUM POLISHED TIN	15	JUMBO JAR 	1532.49	y among the
137498	burnished navajo steel pink maroon	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED COPPER	25	SM CAN    	1535.49	e since the s
137499	blue antique khaki salmon white	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED TIN	29	WRAP JAR  	1536.49	ckages. boldly
137501	navajo magenta frosted snow gainsboro	Manufacturer#4           	Brand#45  	STANDARD BRUSHED COPPER	41	SM CASE   	1538.50	lly. bl
137506	lemon cornflower black pink ivory	Manufacturer#3           	Brand#33  	PROMO POLISHED BRASS	43	MED CAN   	1543.50	kages. 
137507	linen thistle antique steel peach	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED STEEL	22	WRAP PKG  	1544.50	ffily regular depos
137508	cyan orchid aquamarine rose deep	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED STEEL	11	JUMBO BOX 	1545.50	 carefully ironic pa
137509	snow olive mint dodger peru	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	46	SM CASE   	1546.50	ns cajo
137510	tan yellow indian pink thistle	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED STEEL	49	SM PACK   	1547.51	. ironic, even 
137514	navajo snow blush lime forest	Manufacturer#1           	Brand#13  	MEDIUM PLATED NICKEL	43	JUMBO BOX 	1551.51	ickly 
137515	blanched lace deep cornsilk misty	Manufacturer#2           	Brand#23  	STANDARD BURNISHED STEEL	31	LG DRUM   	1552.51	ke care
137519	lawn brown dim powder royal	Manufacturer#2           	Brand#21  	PROMO PLATED TIN	9	SM BOX    	1556.51	equests. 
137520	blush moccasin rose tan sandy	Manufacturer#1           	Brand#15  	PROMO POLISHED STEEL	46	MED JAR   	1557.52	ans alongside
137523	hot seashell dodger slate white	Manufacturer#5           	Brand#55  	SMALL BRUSHED TIN	11	LG PACK   	1560.52	s haggle. furiously
137524	royal deep khaki moccasin tomato	Manufacturer#3           	Brand#34  	SMALL BRUSHED BRASS	45	WRAP BAG  	1561.52	final accounts. reg
137525	sky peru indian tomato maroon	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	3	MED PKG   	1562.52	special requests x-ra
137529	thistle bisque slate drab rose	Manufacturer#4           	Brand#43  	LARGE BURNISHED BRASS	12	MED BAG   	1566.52	fluffi
137530	smoke coral dim orange cyan	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED BRASS	30	MED JAR   	1567.53	 express theodolit
137531	blanched cyan puff lemon cream	Manufacturer#5           	Brand#52  	LARGE POLISHED STEEL	42	LG PKG    	1568.53	uests. theodolit
137536	metallic burnished thistle antique sky	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED BRASS	40	JUMBO CAN 	1573.53	fluffily 
140001	ivory goldenrod sky honeydew misty	Manufacturer#4           	Brand#42  	LARGE BURNISHED TIN	37	SM CAN    	1041.00	 the final, ir
140002	yellow seashell orchid burnished pink	Manufacturer#4           	Brand#44  	LARGE POLISHED COPPER	42	WRAP DRUM 	1042.00	 nag carefully above
140004	dodger hot sienna ghost olive	Manufacturer#4           	Brand#44  	LARGE BURNISHED COPPER	13	SM DRUM   	1044.00	after the
140008	dodger forest tan chiffon lemon	Manufacturer#2           	Brand#22  	ECONOMY BRUSHED BRASS	44	MED JAR   	1048.00	ular packages abo
140011	cornflower lace frosted lawn maroon	Manufacturer#1           	Brand#13  	STANDARD BURNISHED STEEL	27	MED DRUM  	1051.01	r accounts. furiously 
140012	maroon navy smoke pale navajo	Manufacturer#5           	Brand#54  	PROMO POLISHED NICKEL	20	MED BOX   	1052.01	ts alo
140014	beige blush medium magenta sky	Manufacturer#4           	Brand#42  	STANDARD ANODIZED NICKEL	29	WRAP BAG  	1054.01	egular packages; plate
140019	antique green burlywood thistle plum	Manufacturer#1           	Brand#14  	MEDIUM BURNISHED COPPER	8	WRAP DRUM 	1059.01	t the furio
140020	indian chartreuse cyan linen firebrick	Manufacturer#2           	Brand#22  	LARGE BRUSHED COPPER	20	MED PKG   	1060.02	s doze decoys. s
140021	frosted medium sandy firebrick green	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED COPPER	20	WRAP CAN  	1061.02	en requests are.
140022	azure grey burnished lavender pale	Manufacturer#2           	Brand#23  	MEDIUM BRUSHED TIN	29	MED CAN   	1062.02	sts are quickly acr
140023	green tomato pale bisque gainsboro	Manufacturer#3           	Brand#35  	LARGE BRUSHED NICKEL	7	SM PKG    	1063.02	furiously regular 
140027	lime violet antique peru light	Manufacturer#4           	Brand#42  	ECONOMY BRUSHED STEEL	23	JUMBO BOX 	1067.02	symptotes. blithely b
140028	chiffon steel aquamarine floral frosted	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED STEEL	38	WRAP BAG  	1068.02	ns. car
140032	indian cyan frosted lavender turquoise	Manufacturer#2           	Brand#25  	PROMO BRUSHED STEEL	38	WRAP PKG  	1072.03	e slyly abov
140033	tan maroon saddle powder drab	Manufacturer#3           	Brand#35  	LARGE POLISHED BRASS	44	JUMBO DRUM	1073.03	posits are. flu
140036	cyan ghost pink gainsboro midnight	Manufacturer#5           	Brand#53  	SMALL ANODIZED TIN	34	WRAP PACK 	1076.03	, bold req
140037	firebrick blue red azure royal	Manufacturer#3           	Brand#33  	LARGE ANODIZED TIN	7	JUMBO BAG 	1077.03	requests. pending 
140038	steel salmon dim lace moccasin	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED TIN	35	LG JAR    	1078.03	luffily final instru
140042	cyan orchid tomato ghost steel	Manufacturer#4           	Brand#44  	SMALL ANODIZED STEEL	36	LG PKG    	1082.04	iously. blithe
140043	royal orange smoke deep cornflower	Manufacturer#3           	Brand#35  	LARGE POLISHED COPPER	32	MED CAN   	1083.04	oss the slow, pe
140044	metallic peach blue firebrick orchid	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	25	WRAP BOX  	1084.04	yly pending requests-
140049	sienna dim dodger brown pale	Manufacturer#3           	Brand#33  	STANDARD POLISHED TIN	47	MED PKG   	1089.04	nusual the
142459	frosted dark plum purple pale	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED BRASS	37	JUMBO BOX 	1501.45	osits cajole carefully
142460	white salmon black maroon cornflower	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED TIN	39	LG BOX    	1502.46	jole carefully busy p
142462	misty burlywood slate white orchid	Manufacturer#2           	Brand#22  	MEDIUM PLATED COPPER	23	MED BOX   	1504.46	lyly final theodolites
142466	red plum snow dodger goldenrod	Manufacturer#4           	Brand#41  	STANDARD POLISHED COPPER	46	SM CASE   	1508.46	f the fluffi
142469	magenta lawn misty slate forest	Manufacturer#5           	Brand#54  	LARGE BRUSHED TIN	1	LG JAR    	1511.46	tions. furiously pend
142470	rosy thistle plum puff lawn	Manufacturer#5           	Brand#51  	SMALL PLATED COPPER	22	MED PKG   	1512.47	fully. blithely 
142472	navajo red thistle slate lawn	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED STEEL	18	WRAP CAN  	1514.47	lar instructions 
142477	papaya powder drab sky mint	Manufacturer#4           	Brand#43  	STANDARD PLATED BRASS	2	WRAP BAG  	1519.47	ter the final instruct
142478	beige dodger brown cyan smoke	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED NICKEL	12	LG CASE   	1520.47	. final
142479	papaya black pale antique honeydew	Manufacturer#1           	Brand#12  	STANDARD BURNISHED COPPER	24	WRAP PACK 	1521.47	s boost blit
142480	beige seashell midnight medium green	Manufacturer#4           	Brand#45  	SMALL BRUSHED COPPER	48	MED BOX   	1522.48	ns to the care
142481	green chocolate dim orange drab	Manufacturer#4           	Brand#42  	STANDARD PLATED STEEL	33	MED JAR   	1523.48	uests a
142485	brown puff linen salmon blanched	Manufacturer#2           	Brand#22  	STANDARD POLISHED NICKEL	16	MED PACK  	1527.48	. qui
142486	gainsboro red ghost rosy floral	Manufacturer#1           	Brand#13  	STANDARD POLISHED COPPER	17	LG PACK   	1528.48	ites integrate bli
142490	orchid linen violet honeydew lavender	Manufacturer#5           	Brand#54  	LARGE PLATED TIN	44	WRAP JAR  	1532.49	ronic requests 
142491	olive spring salmon navajo snow	Manufacturer#2           	Brand#23  	SMALL BURNISHED STEEL	45	WRAP PACK 	1533.49	e ironic pinto b
142494	white peru grey mint rosy	Manufacturer#1           	Brand#13  	LARGE ANODIZED COPPER	41	SM BOX    	1536.49	fully express packa
142495	magenta black wheat yellow lavender	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED COPPER	27	SM BOX    	1537.49	y unusual request
142496	gainsboro chocolate lace moccasin peru	Manufacturer#2           	Brand#23  	ECONOMY PLATED NICKEL	14	MED DRUM  	1538.49	ges. bold
142500	hot burlywood almond midnight goldenrod	Manufacturer#2           	Brand#25  	LARGE PLATED STEEL	25	WRAP BAG  	1542.50	silent dolphins. q
142501	dodger ivory aquamarine purple thistle	Manufacturer#5           	Brand#52  	PROMO BURNISHED TIN	25	LG DRUM   	1543.50	gular dolphins boost s
142502	cornflower goldenrod thistle puff tomato	Manufacturer#4           	Brand#42  	PROMO PLATED COPPER	50	JUMBO BAG 	1544.50	n requests.
142507	smoke magenta ivory violet ghost	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED BRASS	45	WRAP DRUM 	1549.50	 are fur
144973	drab indian black light magenta	Manufacturer#5           	Brand#55  	STANDARD BURNISHED COPPER	45	WRAP PKG  	2017.97	. bravely bold
144974	salmon saddle linen frosted rosy	Manufacturer#3           	Brand#31  	LARGE BURNISHED TIN	22	SM PKG    	2018.97	 pinto beans use
144976	beige red wheat powder misty	Manufacturer#4           	Brand#42  	MEDIUM PLATED BRASS	1	SM BAG    	2020.97	nic re
144980	blue purple medium tomato turquoise	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED COPPER	38	MED CAN   	2024.98	 the fl
144983	yellow green lawn bisque red	Manufacturer#1           	Brand#11  	LARGE BURNISHED BRASS	48	MED CASE  	2027.98	 cajole slyly 
144984	gainsboro cream almond lace blue	Manufacturer#2           	Brand#25  	SMALL BURNISHED NICKEL	43	LG PACK   	2028.98	. asymptotes should ha
144986	azure slate floral hot lace	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED BRASS	49	WRAP DRUM 	2030.98	ly unusual foxes ac
144991	blanched light tomato aquamarine turquoise	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED STEEL	20	LG PACK   	2035.99	s try to detect blithe
144992	magenta tan almond cream smoke	Manufacturer#3           	Brand#34  	LARGE BRUSHED COPPER	33	LG PKG    	2036.99	y accordin
144993	cyan pale blanched bisque light	Manufacturer#5           	Brand#51  	LARGE PLATED BRASS	40	SM PKG    	2037.99	lites. s
144994	dim cornsilk white navy khaki	Manufacturer#5           	Brand#52  	STANDARD BRUSHED NICKEL	13	MED PACK  	2038.99	uriously--
144995	turquoise violet moccasin lemon chocolate	Manufacturer#2           	Brand#21  	STANDARD PLATED STEEL	18	MED JAR   	2039.99	foxes along 
144999	cream snow lime rose purple	Manufacturer#5           	Brand#54  	MEDIUM POLISHED COPPER	13	JUMBO BOX 	2043.99	ackages are slyly
145000	drab frosted turquoise grey pink	Manufacturer#1           	Brand#11  	PROMO POLISHED BRASS	44	SM CASE   	1045.00	ully ironic 
145004	light lemon burnished dim firebrick	Manufacturer#3           	Brand#34  	STANDARD BURNISHED COPPER	4	MED JAR   	1049.00	ths. slyly fin
145005	green gainsboro wheat slate peach	Manufacturer#4           	Brand#44  	LARGE PLATED NICKEL	24	LG PACK   	1050.00	nstructions ar
145008	floral papaya tan smoke seashell	Manufacturer#3           	Brand#35  	LARGE ANODIZED STEEL	21	SM CAN    	1053.00	ts. spec
145009	puff plum lace grey deep	Manufacturer#1           	Brand#15  	LARGE BURNISHED TIN	1	SM DRUM   	1054.00	oxes. bold pla
145010	medium plum orchid turquoise puff	Manufacturer#4           	Brand#44  	STANDARD POLISHED NICKEL	26	SM JAR    	1055.01	y. regular depo
145014	yellow gainsboro navajo lavender frosted	Manufacturer#3           	Brand#35  	STANDARD POLISHED TIN	37	JUMBO BAG 	1059.01	late carefully. sile
145015	olive forest azure plum floral	Manufacturer#3           	Brand#33  	LARGE BRUSHED BRASS	39	WRAP BOX  	1060.01	 thrash quic
145016	tan salmon cyan dark mint	Manufacturer#3           	Brand#33  	PROMO BURNISHED NICKEL	34	JUMBO PKG 	1061.01	unts througho
145021	azure lime rosy cornsilk hot	Manufacturer#5           	Brand#53  	SMALL BRUSHED STEEL	8	LG JAR    	1066.02	e furious
147487	azure turquoise cream magenta burlywood	Manufacturer#3           	Brand#31  	LARGE BRUSHED COPPER	48	JUMBO CAN 	1534.48	fluffily final in
147488	blue brown tan steel violet	Manufacturer#2           	Brand#24  	SMALL ANODIZED COPPER	3	LG CAN    	1535.48	efully. b
147490	lawn tan blush mint rosy	Manufacturer#3           	Brand#35  	MEDIUM PLATED COPPER	26	WRAP CAN  	1537.49	ges cajole blithely fi
147494	blanched blue navy pale beige	Manufacturer#2           	Brand#22  	ECONOMY PLATED NICKEL	27	JUMBO PKG 	1541.49	o beans wake amo
147497	indian chocolate forest maroon mint	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED BRASS	31	LG PKG    	1544.49	 pains thrash. 
147498	lavender black tomato gainsboro azure	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED COPPER	17	JUMBO CAN 	1545.49	packages haggle accou
147500	puff burnished khaki beige sky	Manufacturer#2           	Brand#25  	PROMO POLISHED BRASS	5	WRAP CASE 	1547.50	kages wake bli
147505	mint pale peru firebrick chartreuse	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED TIN	38	SM BAG    	1552.50	doze on the slyly un
147506	dim thistle aquamarine metallic navy	Manufacturer#2           	Brand#21  	MEDIUM POLISHED TIN	25	MED CAN   	1553.50	ng the
147507	linen beige tomato thistle forest	Manufacturer#1           	Brand#13  	SMALL BRUSHED TIN	48	WRAP BAG  	1554.50	 blithely. bl
147508	sienna saddle honeydew moccasin gainsboro	Manufacturer#3           	Brand#34  	ECONOMY PLATED TIN	8	MED BOX   	1555.50	usly-- slyly
147509	cream mint midnight steel azure	Manufacturer#2           	Brand#23  	SMALL ANODIZED BRASS	28	LG JAR    	1556.50	es. iro
147513	dark orange sky salmon violet	Manufacturer#2           	Brand#24  	LARGE BRUSHED STEEL	7	JUMBO CASE	1560.51	 after the furious
147514	turquoise peach tan navajo lemon	Manufacturer#1           	Brand#11  	PROMO POLISHED COPPER	21	JUMBO CAN 	1561.51	y even packages us
147518	mint grey black wheat navajo	Manufacturer#5           	Brand#51  	PROMO POLISHED NICKEL	41	LG JAR    	1565.51	gular deposits 
147519	seashell lavender light burlywood sky	Manufacturer#1           	Brand#12  	ECONOMY BRUSHED COPPER	36	LG CASE   	1566.51	ites aff
147522	light rosy tan chocolate burnished	Manufacturer#1           	Brand#11  	SMALL ANODIZED STEEL	5	WRAP BOX  	1569.52	ptotes hang slyly. s
147523	indian magenta navajo dodger violet	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED TIN	29	SM CAN    	1570.52	y among the slyly exp
147524	lime almond indian turquoise misty	Manufacturer#3           	Brand#33  	LARGE POLISHED NICKEL	24	WRAP PACK 	1571.52	eposits doze blithely 
147528	misty blue chocolate tomato white	Manufacturer#3           	Brand#33  	STANDARD BURNISHED STEEL	36	JUMBO BOX 	1575.52	ar asymptotes 
147529	indian gainsboro thistle sienna violet	Manufacturer#4           	Brand#43  	LARGE BURNISHED TIN	18	JUMBO PACK	1576.52	e the spe
147530	midnight olive dim blue seashell	Manufacturer#3           	Brand#33  	MEDIUM BRUSHED BRASS	8	MED BOX   	1577.53	x slyly after t
147535	blue blanched royal burnished cornflower	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED STEEL	50	WRAP DRUM 	1582.53	s, final grouch
150001	aquamarine burnished orchid pink thistle	Manufacturer#1           	Brand#14  	ECONOMY POLISHED NICKEL	38	WRAP BOX  	1051.00	c dependencies haggle 
150002	purple violet salmon firebrick cornflower	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED NICKEL	35	MED CAN   	1052.00	uickly ev
150004	tan grey gainsboro khaki chartreuse	Manufacturer#1           	Brand#13  	MEDIUM PLATED TIN	28	WRAP PKG  	1054.00	en deposits sleep
150008	smoke cream indian slate white	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED BRASS	22	WRAP BAG  	1058.00	lites. de
150011	chiffon firebrick gainsboro tan antique	Manufacturer#1           	Brand#11  	SMALL BURNISHED COPPER	15	WRAP CAN  	1061.01	 ideas. blit
150012	orange metallic puff pink ghost	Manufacturer#1           	Brand#11  	LARGE BRUSHED BRASS	20	JUMBO CAN 	1062.01	 to the ideas 
150014	seashell midnight burnished cyan lime	Manufacturer#3           	Brand#33  	MEDIUM BURNISHED COPPER	14	MED PKG   	1064.01	s packages about 
150019	midnight violet ghost magenta misty	Manufacturer#3           	Brand#35  	ECONOMY BRUSHED COPPER	43	LG BOX    	1069.01	uriously furi
150020	pink linen lace dim cream	Manufacturer#2           	Brand#25  	MEDIUM POLISHED STEEL	44	MED PKG   	1070.02	uests cajole among th
150021	coral hot frosted saddle burnished	Manufacturer#2           	Brand#23  	SMALL PLATED BRASS	46	MED PACK  	1071.02	jole after 
150022	chartreuse dark frosted snow navajo	Manufacturer#3           	Brand#35  	LARGE BRUSHED TIN	13	JUMBO JAR 	1072.02	nt. furiously si
150023	red chiffon lemon brown chocolate	Manufacturer#3           	Brand#33  	LARGE PLATED TIN	23	LG DRUM   	1073.02	y unusual 
150027	pink metallic forest papaya white	Manufacturer#2           	Brand#23  	LARGE ANODIZED COPPER	5	SM PACK   	1077.02	lar packages are 
150028	saddle navy sandy aquamarine ghost	Manufacturer#4           	Brand#44  	LARGE BURNISHED STEEL	23	JUMBO CAN 	1078.02	eas hinder slyly
150032	red navajo royal dark snow	Manufacturer#2           	Brand#22  	PROMO POLISHED NICKEL	41	MED PACK  	1082.03	symptotes wake
150033	medium blush cornflower gainsboro ivory	Manufacturer#3           	Brand#32  	PROMO BURNISHED NICKEL	7	MED BAG   	1083.03	 request
150036	spring antique salmon cornflower ghost	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED STEEL	25	SM PACK   	1086.03	heodolites nag furi
150037	puff brown blush chocolate lemon	Manufacturer#1           	Brand#15  	PROMO PLATED COPPER	8	JUMBO CASE	1087.03	efully 
150038	seashell light smoke tan hot	Manufacturer#2           	Brand#24  	ECONOMY PLATED COPPER	30	MED CASE  	1088.03	ions would detect 
150042	dark cornsilk honeydew brown blue	Manufacturer#3           	Brand#35  	PROMO BRUSHED TIN	19	SM PKG    	1092.04	imes bold packages
150043	metallic chartreuse cornsilk yellow moccasin	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED TIN	17	LG PACK   	1093.04	quickly slyly
150044	plum green peru linen white	Manufacturer#5           	Brand#51  	MEDIUM PLATED COPPER	12	WRAP CASE 	1094.04	ckages haggle quic
150049	slate gainsboro spring wheat coral	Manufacturer#2           	Brand#25  	STANDARD PLATED NICKEL	15	JUMBO BOX 	1099.04	courts integrate blit
152456	medium peach lime brown goldenrod	Manufacturer#1           	Brand#13  	MEDIUM POLISHED STEEL	38	JUMBO PKG 	1508.45	lyly special grouch
152457	aquamarine antique floral midnight cream	Manufacturer#4           	Brand#44  	LARGE BURNISHED COPPER	45	WRAP CASE 	1509.45	tions affix above
152459	lawn blue seashell goldenrod lemon	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	24	LG CAN    	1511.45	regular deposi
152463	peach steel chartreuse ivory salmon	Manufacturer#3           	Brand#33  	PROMO PLATED NICKEL	14	WRAP BAG  	1515.46	to the deposits nag 
152466	green turquoise pink mint papaya	Manufacturer#1           	Brand#12  	STANDARD PLATED BRASS	17	MED JAR   	1518.46	 final escapade
152467	plum goldenrod royal deep maroon	Manufacturer#5           	Brand#51  	STANDARD ANODIZED STEEL	49	LG CASE   	1519.46	ously. b
152469	bisque goldenrod coral navy seashell	Manufacturer#4           	Brand#44  	PROMO BRUSHED NICKEL	3	MED DRUM  	1521.46	nts cajole above the 
152474	ghost misty wheat burlywood lavender	Manufacturer#4           	Brand#42  	SMALL POLISHED NICKEL	19	SM BOX    	1526.47	s. pending, sly pl
152475	chocolate forest medium brown linen	Manufacturer#1           	Brand#13  	PROMO POLISHED COPPER	16	JUMBO BOX 	1527.47	, final r
152476	papaya chocolate brown burnished light	Manufacturer#5           	Brand#54  	STANDARD BRUSHED COPPER	44	SM JAR    	1528.47	equests
152477	white ghost royal dim lavender	Manufacturer#3           	Brand#35  	LARGE POLISHED COPPER	13	LG BAG    	1529.47	n requ
152478	white goldenrod snow gainsboro lime	Manufacturer#3           	Brand#34  	ECONOMY POLISHED COPPER	48	LG JAR    	1530.47	y above the furious
152482	cornsilk azure deep royal turquoise	Manufacturer#5           	Brand#53  	LARGE ANODIZED STEEL	15	LG PACK   	1534.48	al fo
152483	lemon bisque almond metallic forest	Manufacturer#2           	Brand#25  	SMALL POLISHED STEEL	9	WRAP JAR  	1535.48	t care
152487	plum lime snow chocolate maroon	Manufacturer#2           	Brand#24  	STANDARD POLISHED TIN	17	LG BAG    	1539.48	unusual 
152488	linen ivory saddle khaki rose	Manufacturer#4           	Brand#43  	LARGE BRUSHED TIN	2	SM CASE   	1540.48	efully u
152491	almond goldenrod cream chiffon cornsilk	Manufacturer#4           	Brand#41  	MEDIUM PLATED STEEL	12	MED BAG   	1543.49	r courts integra
152492	saddle red antique tan chartreuse	Manufacturer#2           	Brand#22  	ECONOMY BURNISHED NICKEL	22	SM PKG    	1544.49	e of the slyly
152493	cyan orchid gainsboro sienna papaya	Manufacturer#1           	Brand#11  	LARGE POLISHED NICKEL	17	MED CAN   	1545.49	nic platelets thrash
152497	smoke lace khaki puff aquamarine	Manufacturer#3           	Brand#33  	MEDIUM PLATED TIN	30	WRAP BOX  	1549.49	 accounts. expres
152498	spring goldenrod grey sienna midnight	Manufacturer#2           	Brand#24  	SMALL PLATED COPPER	35	JUMBO JAR 	1550.49	p daringly
152499	blanched cyan lemon chartreuse papaya	Manufacturer#1           	Brand#12  	LARGE POLISHED BRASS	47	SM PACK   	1551.49	egular gro
152504	cyan papaya cornsilk navajo grey	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED TIN	43	LG CASE   	1556.50	dependencies believe 
154971	khaki blue papaya misty forest	Manufacturer#5           	Brand#53  	PROMO POLISHED BRASS	27	LG BOX    	2025.97	s. ironic pains ha
154972	royal dim sienna forest turquoise	Manufacturer#5           	Brand#54  	STANDARD PLATED TIN	35	MED BAG   	2026.97	l foxes wake fu
154974	almond wheat salmon blush spring	Manufacturer#1           	Brand#14  	PROMO BRUSHED STEEL	36	MED CASE  	2028.97	s boost entici
154978	wheat saddle beige blanched puff	Manufacturer#5           	Brand#52  	LARGE PLATED NICKEL	45	MED BAG   	2032.97	luffily across the ca
154981	orange spring hot ghost lemon	Manufacturer#3           	Brand#32  	LARGE PLATED COPPER	16	LG DRUM   	2035.98	nts print unusu
154982	grey puff orchid khaki firebrick	Manufacturer#3           	Brand#34  	STANDARD PLATED BRASS	20	JUMBO DRUM	2036.98	arefully regular a
154984	mint cream papaya forest almond	Manufacturer#3           	Brand#35  	PROMO PLATED STEEL	48	SM PKG    	2038.98	ously regular fo
154989	white orchid sandy cyan moccasin	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED STEEL	12	WRAP BAG  	2043.98	cuses should aff
154990	ivory turquoise royal green black	Manufacturer#2           	Brand#23  	LARGE BRUSHED COPPER	18	WRAP JAR  	2044.99	gains
154991	linen rosy coral yellow seashell	Manufacturer#4           	Brand#42  	LARGE POLISHED BRASS	47	SM PKG    	2045.99	ymptote
154992	seashell salmon deep chartreuse magenta	Manufacturer#4           	Brand#45  	LARGE POLISHED STEEL	8	SM BOX    	2046.99	instructions 
154993	sky orchid khaki hot burnished	Manufacturer#3           	Brand#34  	SMALL PLATED STEEL	42	JUMBO BAG 	2047.99	doggedly bol
154997	orchid linen hot dodger wheat	Manufacturer#1           	Brand#15  	STANDARD BURNISHED TIN	48	LG BOX    	2051.99	sts nag carefully s
154998	cream misty deep cornsilk cyan	Manufacturer#5           	Brand#54  	ECONOMY POLISHED STEEL	18	LG CAN    	2052.99	eodolites sublate dogg
155002	azure papaya deep chocolate pale	Manufacturer#2           	Brand#21  	STANDARD ANODIZED BRASS	12	LG JAR    	1057.00	riously regu
155003	saddle smoke blanched orange maroon	Manufacturer#3           	Brand#35  	PROMO PLATED STEEL	33	WRAP JAR  	1058.00	 deposits try to
155006	forest black azure brown cornsilk	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED COPPER	24	WRAP CAN  	1061.00	ly unu
155007	honeydew linen gainsboro chiffon cornflower	Manufacturer#4           	Brand#45  	ECONOMY POLISHED BRASS	14	MED PACK  	1062.00	 caref
155008	rose indian blush ivory sandy	Manufacturer#2           	Brand#24  	LARGE BURNISHED COPPER	3	LG DRUM   	1063.00	uriously. pending acco
155012	grey rosy steel purple moccasin	Manufacturer#5           	Brand#53  	STANDARD BURNISHED COPPER	37	LG DRUM   	1067.01	l foxes cajole 
155013	dim salmon lime slate blanched	Manufacturer#1           	Brand#12  	ECONOMY PLATED NICKEL	46	JUMBO BOX 	1068.01	xes haggle blithe
155014	burlywood aquamarine yellow sienna navy	Manufacturer#2           	Brand#21  	STANDARD PLATED TIN	32	LG BOX    	1069.01	r deposits boost
155019	white tomato lace yellow snow	Manufacturer#2           	Brand#25  	PROMO BRUSHED NICKEL	19	SM BOX    	1074.01	atelets. bold brai
157486	orange coral cream sandy burlywood	Manufacturer#1           	Brand#15  	STANDARD POLISHED COPPER	46	JUMBO CASE	1543.48	accounts
157487	maroon cyan saddle blue goldenrod	Manufacturer#2           	Brand#21  	PROMO PLATED STEEL	21	JUMBO BAG 	1544.48	key players
157489	steel cornflower cyan ivory antique	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	36	SM DRUM   	1546.48	hes. slyly
157493	purple burlywood red midnight coral	Manufacturer#5           	Brand#55  	STANDARD PLATED NICKEL	31	LG PKG    	1550.49	along the silent foxes
157496	snow purple linen misty steel	Manufacturer#2           	Brand#23  	MEDIUM POLISHED COPPER	11	MED PKG   	1553.49	ully furious
157497	magenta aquamarine light blanched cyan	Manufacturer#5           	Brand#52  	PROMO ANODIZED BRASS	6	MED PACK  	1554.49	al hockey players.
157499	olive sienna steel beige slate	Manufacturer#4           	Brand#42  	LARGE BRUSHED TIN	6	MED CAN   	1556.49	 instructio
157504	chiffon drab purple rosy beige	Manufacturer#3           	Brand#31  	SMALL BURNISHED BRASS	22	SM CAN    	1561.50	 ruthless, final packa
157505	maroon beige indian ghost burlywood	Manufacturer#3           	Brand#35  	ECONOMY BRUSHED COPPER	48	JUMBO JAR 	1562.50	 furiously
157506	sky powder cream seashell cyan	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED STEEL	34	JUMBO BAG 	1563.50	silent dependenci
157507	lavender misty floral cyan almond	Manufacturer#1           	Brand#11  	ECONOMY PLATED COPPER	5	MED JAR   	1564.50	s packages
157508	goldenrod orange navy tomato medium	Manufacturer#4           	Brand#44  	PROMO BRUSHED STEEL	9	SM CAN    	1565.50	lar, regular de
157512	smoke lime lavender thistle plum	Manufacturer#3           	Brand#33  	MEDIUM BURNISHED NICKEL	42	SM BAG    	1569.51	kly unusua
157513	purple medium spring sky antique	Manufacturer#3           	Brand#31  	LARGE POLISHED NICKEL	6	SM BAG    	1570.51	s are. carefully fina
157517	seashell dark steel beige burlywood	Manufacturer#1           	Brand#11  	PROMO BRUSHED NICKEL	1	MED BOX   	1574.51	ole carefully acc
157518	black puff hot dodger coral	Manufacturer#3           	Brand#35  	SMALL PLATED NICKEL	20	JUMBO DRUM	1575.51	lyly pendi
157521	linen moccasin beige misty dodger	Manufacturer#5           	Brand#55  	STANDARD PLATED NICKEL	11	SM CASE   	1578.52	 final accounts da
157522	ghost sky purple magenta maroon	Manufacturer#4           	Brand#43  	LARGE PLATED STEEL	14	WRAP DRUM 	1579.52	c excuses. fu
157523	slate cornsilk tan plum grey	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED NICKEL	16	JUMBO CAN 	1580.52	. special theod
157527	lime smoke rose goldenrod almond	Manufacturer#4           	Brand#42  	STANDARD BURNISHED COPPER	23	SM DRUM   	1584.52	packages. regular d
157528	green misty pale magenta puff	Manufacturer#1           	Brand#12  	PROMO BRUSHED NICKEL	39	JUMBO BOX 	1585.52	final excu
157529	wheat black azure peach sienna	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	31	LG DRUM   	1586.52	unts wake furiou
157534	lawn violet chartreuse snow drab	Manufacturer#2           	Brand#23  	PROMO BRUSHED STEEL	26	JUMBO BOX 	1591.53	r requests snooze 
160001	drab metallic midnight tomato thistle	Manufacturer#1           	Brand#14  	LARGE BRUSHED COPPER	7	MED BOX   	1061.00	 careful
160002	saddle green coral white floral	Manufacturer#5           	Brand#54  	SMALL PLATED BRASS	33	MED DRUM  	1062.00	gular de
160004	dim coral dodger hot violet	Manufacturer#5           	Brand#52  	STANDARD BRUSHED TIN	8	LG BOX    	1064.00	 slyly even the
160008	rose misty grey yellow pale	Manufacturer#5           	Brand#55  	PROMO POLISHED STEEL	30	JUMBO BOX 	1068.00	 are furiously ca
160011	lemon lime snow dodger drab	Manufacturer#2           	Brand#24  	MEDIUM PLATED STEEL	32	JUMBO CAN 	1071.01	y bold deposits wa
160012	thistle blue puff mint spring	Manufacturer#2           	Brand#23  	LARGE POLISHED STEEL	38	MED CAN   	1072.01	sual, ironic ideas. d
160014	indian navajo burnished metallic coral	Manufacturer#2           	Brand#25  	SMALL BURNISHED COPPER	17	JUMBO JAR 	1074.01	 excuses wa
160019	aquamarine deep linen peru ghost	Manufacturer#2           	Brand#22  	SMALL BURNISHED COPPER	49	JUMBO DRUM	1079.01	uriously regula
160020	papaya purple coral pink light	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED COPPER	10	LG BAG    	1080.02	 ironic asymptote
160021	cream burlywood sky dim lavender	Manufacturer#1           	Brand#11  	LARGE PLATED COPPER	5	JUMBO BOX 	1081.02	 agai
160022	navajo almond blanched beige maroon	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED BRASS	23	JUMBO PACK	1082.02	s. final, e
160023	brown dim tan rose orchid	Manufacturer#4           	Brand#42  	STANDARD PLATED BRASS	40	LG DRUM   	1083.02	theodolite
160027	tomato cyan metallic dark ghost	Manufacturer#4           	Brand#45  	ECONOMY POLISHED NICKEL	4	SM CASE   	1087.02	wake furious
160028	snow slate light medium dark	Manufacturer#1           	Brand#11  	SMALL BRUSHED COPPER	35	MED PACK  	1088.02	nto bean
160032	yellow medium brown green lavender	Manufacturer#2           	Brand#22  	SMALL POLISHED TIN	30	WRAP DRUM 	1092.03	nal dep
160033	slate green orchid lavender goldenrod	Manufacturer#5           	Brand#55  	STANDARD PLATED TIN	44	JUMBO JAR 	1093.03	mold slyly expre
160036	light thistle seashell chocolate black	Manufacturer#5           	Brand#52  	LARGE PLATED BRASS	38	SM CASE   	1096.03	lent court
160037	navy cornflower midnight medium turquoise	Manufacturer#4           	Brand#43  	SMALL ANODIZED TIN	1	JUMBO CAN 	1097.03	stealthy, special de
160038	lemon orchid lavender black burlywood	Manufacturer#2           	Brand#24  	STANDARD PLATED STEEL	37	JUMBO BAG 	1098.03	ully 
160042	cornsilk sandy linen ghost khaki	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	14	JUMBO CAN 	1102.04	ly carefully pending c
160043	snow saddle cornsilk burlywood azure	Manufacturer#1           	Brand#11  	STANDARD BURNISHED STEEL	48	SM CAN    	1103.04	accordin
160044	slate burnished forest plum pale	Manufacturer#3           	Brand#34  	PROMO ANODIZED TIN	8	SM BAG    	1104.04	lyly iron
160049	navajo goldenrod bisque azure cream	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED STEEL	39	MED BAG   	1109.04	 blithely pendi
162453	orchid mint ghost firebrick floral	Manufacturer#1           	Brand#14  	PROMO PLATED BRASS	12	SM CASE   	1515.45	eat slyly. bold exc
162454	mint navy wheat firebrick steel	Manufacturer#3           	Brand#33  	ECONOMY PLATED BRASS	9	LG BOX    	1516.45	ts hang above 
162456	misty sienna antique thistle almond	Manufacturer#3           	Brand#35  	ECONOMY PLATED TIN	40	LG CAN    	1518.45	nticing i
162460	blush dim ghost pink blanched	Manufacturer#4           	Brand#45  	STANDARD POLISHED NICKEL	41	WRAP PACK 	1522.46	uffily after the fin
162463	pale light wheat frosted cream	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	34	JUMBO BAG 	1525.46	bold packages sleep 
162464	smoke blue chiffon goldenrod lace	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED BRASS	24	MED BOX   	1526.46	 regular
162466	royal indian black wheat burnished	Manufacturer#1           	Brand#14  	ECONOMY BURNISHED COPPER	5	JUMBO DRUM	1528.46	t the thin account
162471	firebrick cream sandy steel puff	Manufacturer#4           	Brand#44  	PROMO POLISHED TIN	43	LG CASE   	1533.47	ackage
162472	aquamarine deep cornflower steel goldenrod	Manufacturer#5           	Brand#52  	SMALL BRUSHED BRASS	44	LG JAR    	1534.47	 of th
162473	cornflower green chiffon grey pale	Manufacturer#1           	Brand#13  	LARGE BURNISHED STEEL	16	WRAP JAR  	1535.47	oxes. pen
162474	lavender antique moccasin orange chocolate	Manufacturer#5           	Brand#53  	LARGE PLATED BRASS	35	SM CASE   	1536.47	foxes; furiously pendi
162475	coral indian rosy puff peach	Manufacturer#2           	Brand#22  	STANDARD POLISHED COPPER	29	WRAP DRUM 	1537.47	ong the quic
162479	orange mint honeydew peach dim	Manufacturer#2           	Brand#21  	STANDARD PLATED COPPER	6	WRAP BOX  	1541.47	against th
162480	cornflower maroon blue floral blanched	Manufacturer#3           	Brand#34  	PROMO BURNISHED BRASS	4	SM CAN    	1542.48	leep slyly. iron
162484	azure deep salmon orange pink	Manufacturer#5           	Brand#55  	LARGE POLISHED STEEL	45	SM BOX    	1546.48	 pinto beans. 
162485	papaya moccasin honeydew floral chartreuse	Manufacturer#5           	Brand#51  	LARGE PLATED NICKEL	50	JUMBO CAN 	1547.48	ns cajole around the i
162488	ghost cornflower rosy chocolate orange	Manufacturer#4           	Brand#44  	MEDIUM BURNISHED TIN	25	SM JAR    	1550.48	gular packages. fi
162489	pale brown grey cornflower sandy	Manufacturer#2           	Brand#23  	ECONOMY POLISHED TIN	3	SM JAR    	1551.48	y careful foxes 
162490	goldenrod red gainsboro chartreuse blanched	Manufacturer#2           	Brand#22  	LARGE BURNISHED NICKEL	35	LG JAR    	1552.49	uests cajole along
162494	peru brown rosy bisque tan	Manufacturer#4           	Brand#44  	STANDARD BURNISHED BRASS	7	LG DRUM   	1556.49	packages. deposit
162495	white dark cyan burlywood smoke	Manufacturer#4           	Brand#43  	ECONOMY POLISHED STEEL	18	SM BOX    	1557.49	 instructio
162496	tan lime wheat frosted navajo	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED COPPER	26	MED JAR   	1558.49	arefully iron
162501	brown slate deep smoke light	Manufacturer#4           	Brand#44  	MEDIUM POLISHED NICKEL	50	SM CASE   	1563.50	ccording t
164969	dodger chartreuse purple hot sienna	Manufacturer#2           	Brand#22  	ECONOMY PLATED COPPER	26	LG CAN    	2033.96	the some
164970	khaki magenta maroon green indian	Manufacturer#4           	Brand#43  	PROMO POLISHED COPPER	47	JUMBO CAN 	2034.97	s integrate
164972	goldenrod misty lime hot ghost	Manufacturer#5           	Brand#52  	PROMO POLISHED TIN	45	SM PKG    	2036.97	osits woul
164976	yellow black almond bisque purple	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	35	JUMBO JAR 	2040.97	the careful
164979	thistle almond pink smoke burlywood	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED TIN	50	SM CASE   	2043.97	onic, pen
164980	ghost sky peach slate smoke	Manufacturer#4           	Brand#43  	LARGE BURNISHED TIN	23	MED CAN   	2044.98	al instruction
164982	thistle puff papaya plum forest	Manufacturer#5           	Brand#55  	MEDIUM PLATED TIN	19	JUMBO DRUM	2046.98	ly unusual, final the
164987	chartreuse dim almond pale moccasin	Manufacturer#1           	Brand#11  	PROMO POLISHED COPPER	39	MED PACK  	2051.98	cording to the c
164988	blush rosy seashell royal magenta	Manufacturer#4           	Brand#42  	SMALL BURNISHED STEEL	6	MED DRUM  	2052.98	thely regular asymptot
164989	chocolate mint salmon lime puff	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED COPPER	25	JUMBO CAN 	2053.98	ven accounts.
164990	spring olive black rosy coral	Manufacturer#1           	Brand#14  	PROMO POLISHED NICKEL	4	MED BOX   	2054.99	ccounts. furiously ir
164991	powder light antique frosted white	Manufacturer#1           	Brand#13  	STANDARD PLATED BRASS	32	WRAP PACK 	2055.99	uickly final dol
164995	puff ivory spring cyan light	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED TIN	4	JUMBO CAN 	2059.99	ructions sle
164996	linen lime tomato midnight pale	Manufacturer#3           	Brand#33  	ECONOMY PLATED BRASS	25	JUMBO BAG 	2060.99	 furiously even excu
165000	forest dark peach medium grey	Manufacturer#2           	Brand#21  	LARGE BURNISHED NICKEL	26	MED BAG   	1065.00	enden
165001	lemon slate red medium metallic	Manufacturer#4           	Brand#41  	MEDIUM ANODIZED STEEL	20	WRAP PACK 	1066.00	theodolites accordin
165004	spring burnished snow lime tomato	Manufacturer#4           	Brand#43  	MEDIUM POLISHED STEEL	5	MED CASE  	1069.00	es. express pinto 
165005	powder light almond khaki indian	Manufacturer#1           	Brand#12  	ECONOMY PLATED COPPER	37	JUMBO PKG 	1070.00	osits promise pack
165006	puff floral powder ivory purple	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED STEEL	42	MED PACK  	1071.00	refully regular re
165010	forest lavender violet azure steel	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED STEEL	15	MED BAG   	1075.01	ly ironic acco
165011	aquamarine red orchid cream tomato	Manufacturer#3           	Brand#34  	LARGE ANODIZED COPPER	26	SM PKG    	1076.01	special excuses.
165012	navy ghost chocolate cyan drab	Manufacturer#5           	Brand#51  	LARGE POLISHED STEEL	40	SM JAR    	1077.01	rhorse
165017	light indian honeydew beige papaya	Manufacturer#3           	Brand#35  	PROMO ANODIZED BRASS	20	WRAP DRUM 	1082.01	dependenc
167485	cornflower lace cream grey tomato	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED NICKEL	1	WRAP PACK 	1552.48	y accordin
167486	snow linen aquamarine cornsilk magenta	Manufacturer#4           	Brand#43  	PROMO POLISHED TIN	10	LG CASE   	1553.48	es. ideas sle
167488	navajo firebrick blush metallic purple	Manufacturer#5           	Brand#54  	ECONOMY BURNISHED BRASS	16	LG CAN    	1555.48	e regular, pendi
167492	slate goldenrod turquoise lime brown	Manufacturer#5           	Brand#55  	STANDARD BRUSHED STEEL	15	JUMBO CASE	1559.49	y ironic requ
167495	powder hot plum navajo pale	Manufacturer#1           	Brand#12  	ECONOMY POLISHED STEEL	29	WRAP JAR  	1562.49	final, even
167496	cyan antique purple thistle papaya	Manufacturer#4           	Brand#45  	LARGE BURNISHED COPPER	38	MED CAN   	1563.49	al deposits are 
167498	hot metallic ivory magenta orange	Manufacturer#3           	Brand#33  	STANDARD BRUSHED TIN	48	WRAP BOX  	1565.49	nal, pending foxe
167503	floral brown blanched wheat pink	Manufacturer#1           	Brand#14  	PROMO ANODIZED BRASS	33	MED PKG   	1570.50	ke carefully. 
167504	lemon tomato chocolate gainsboro blue	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED COPPER	39	JUMBO PACK	1571.50	ccounts. final re
167505	green powder brown lemon grey	Manufacturer#2           	Brand#22  	SMALL POLISHED STEEL	35	LG CAN    	1572.50	 run 
167506	bisque cornsilk almond tomato floral	Manufacturer#4           	Brand#44  	SMALL BURNISHED STEEL	2	WRAP PACK 	1573.50	into bea
167507	indian thistle purple antique forest	Manufacturer#5           	Brand#53  	STANDARD POLISHED NICKEL	46	JUMBO BAG 	1574.50	, even excuses wake fu
167511	slate papaya lavender seashell chiffon	Manufacturer#4           	Brand#43  	STANDARD ANODIZED COPPER	34	LG JAR    	1578.51	press pinto beans; sly
167512	antique tan firebrick saddle peru	Manufacturer#5           	Brand#54  	PROMO POLISHED BRASS	26	WRAP CAN  	1579.51	r requests c
167516	blue drab antique cyan orchid	Manufacturer#3           	Brand#32  	ECONOMY ANODIZED BRASS	10	SM DRUM   	1583.51	nag quickly unusual
167517	forest lime magenta deep medium	Manufacturer#5           	Brand#55  	MEDIUM POLISHED STEEL	48	SM CASE   	1584.51	s are fluffily 
167520	ivory misty blue burlywood orchid	Manufacturer#4           	Brand#45  	PROMO ANODIZED TIN	5	LG CASE   	1587.52	sual account
167521	hot honeydew cornflower puff linen	Manufacturer#4           	Brand#43  	LARGE ANODIZED NICKEL	7	LG DRUM   	1588.52	ounts. furiously
167522	chartreuse medium white floral aquamarine	Manufacturer#5           	Brand#55  	LARGE BRUSHED TIN	6	LG PACK   	1589.52	t, ironic real
167526	dim goldenrod hot blush puff	Manufacturer#1           	Brand#12  	PROMO ANODIZED BRASS	43	SM DRUM   	1593.52	ag against the final
167527	olive lace saddle tomato violet	Manufacturer#1           	Brand#11  	PROMO BURNISHED TIN	25	JUMBO DRUM	1594.52	s boo
167528	turquoise honeydew brown papaya midnight	Manufacturer#4           	Brand#42  	LARGE PLATED TIN	3	JUMBO PACK	1595.52	e regular deposits. s
167533	lime yellow cream olive coral	Manufacturer#2           	Brand#25  	LARGE BURNISHED STEEL	17	SM CAN    	1600.53	to beans serv
170001	azure peach yellow violet papaya	Manufacturer#2           	Brand#25  	LARGE POLISHED NICKEL	29	WRAP DRUM 	1071.00	ix bold
170002	medium metallic moccasin rose bisque	Manufacturer#3           	Brand#32  	PROMO BURNISHED BRASS	10	WRAP BOX  	1072.00	encies su
170004	chartreuse cornsilk grey powder blanched	Manufacturer#2           	Brand#23  	LARGE PLATED BRASS	44	SM BAG    	1074.00	ent dependenc
170008	lemon linen powder ghost lavender	Manufacturer#1           	Brand#12  	SMALL POLISHED TIN	46	JUMBO BOX 	1078.00	ic requests. slyly exp
170011	navajo peru thistle beige blanched	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED TIN	49	SM BOX    	1081.01	ly special pa
170012	royal rose coral brown gainsboro	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED NICKEL	30	LG CAN    	1082.01	e fluff
170014	slate sienna chartreuse firebrick drab	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED COPPER	22	SM PKG    	1084.01	ckly special 
170019	misty blanched ghost lime slate	Manufacturer#5           	Brand#51  	SMALL PLATED STEEL	9	JUMBO PKG 	1089.01	as. theodolites wak
170020	midnight spring seashell beige blue	Manufacturer#5           	Brand#51  	LARGE BRUSHED TIN	25	WRAP PACK 	1090.02	g platelets are 
170021	rose dim white honeydew grey	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED TIN	42	SM CAN    	1091.02	press requests
170022	yellow smoke burnished navy lavender	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	15	MED PKG   	1092.02	above the
170023	pale ghost dodger pink purple	Manufacturer#5           	Brand#54  	MEDIUM POLISHED NICKEL	42	JUMBO DRUM	1093.02	equests wake
170027	medium metallic pale burlywood lemon	Manufacturer#4           	Brand#41  	ECONOMY POLISHED COPPER	16	MED PACK  	1097.02	the platelets nag 
170028	spring sienna olive lemon tan	Manufacturer#1           	Brand#13  	ECONOMY POLISHED STEEL	3	JUMBO CAN 	1098.02	sely ironic
170032	maroon salmon rosy thistle orange	Manufacturer#4           	Brand#43  	PROMO BURNISHED STEEL	45	LG DRUM   	1102.03	ess excuses. bli
170033	goldenrod cyan honeydew aquamarine spring	Manufacturer#5           	Brand#52  	STANDARD BRUSHED TIN	33	WRAP DRUM 	1103.03	s wake blithel
170036	puff red floral saddle cyan	Manufacturer#4           	Brand#45  	LARGE ANODIZED BRASS	28	WRAP CASE 	1106.03	 unusual ac
170037	light orange ghost navy burlywood	Manufacturer#1           	Brand#11  	PROMO POLISHED TIN	13	WRAP CAN  	1107.03	ndencies. slyly
170038	aquamarine grey cream dodger burlywood	Manufacturer#2           	Brand#22  	SMALL BRUSHED BRASS	49	LG CASE   	1108.03	ttainments ca
170042	steel tomato black slate smoke	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED NICKEL	47	WRAP CAN  	1112.04	ing accounts. 
170043	chiffon antique almond misty azure	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED NICKEL	33	LG PACK   	1113.04	cuses. pending, regu
170044	floral wheat violet spring turquoise	Manufacturer#3           	Brand#32  	LARGE POLISHED STEEL	48	MED CASE  	1114.04	 haggle ag
170049	cyan ghost pale forest linen	Manufacturer#5           	Brand#51  	SMALL PLATED BRASS	41	MED BAG   	1119.04	cajole slyly fluf
172450	black orchid cornflower sky tomato	Manufacturer#1           	Brand#15  	STANDARD ANODIZED BRASS	43	WRAP DRUM 	1522.45	ecial packages. furio
172451	moccasin navajo pale burnished sky	Manufacturer#3           	Brand#35  	SMALL BURNISHED TIN	29	JUMBO BOX 	1523.45	es. even, even pi
172453	lawn linen cyan slate turquoise	Manufacturer#1           	Brand#12  	STANDARD ANODIZED COPPER	17	WRAP PACK 	1525.45	g always at th
172457	chocolate lavender almond orchid lace	Manufacturer#5           	Brand#55  	LARGE ANODIZED COPPER	44	MED BAG   	1529.45	regular packag
172460	cornflower medium sienna turquoise misty	Manufacturer#1           	Brand#12  	LARGE BRUSHED STEEL	30	SM CASE   	1532.46	intain bl
172461	lawn honeydew slate thistle cream	Manufacturer#4           	Brand#41  	PROMO PLATED NICKEL	47	MED DRUM  	1533.46	kages ab
172463	cornflower antique navajo royal saddle	Manufacturer#1           	Brand#13  	ECONOMY ANODIZED STEEL	42	SM PACK   	1535.46	lphins-
172468	cornflower pale snow pink tan	Manufacturer#1           	Brand#12  	STANDARD BURNISHED BRASS	2	JUMBO PKG 	1540.46	players. r
172469	white lavender beige lawn gainsboro	Manufacturer#1           	Brand#14  	STANDARD ANODIZED COPPER	26	SM CASE   	1541.46	 agains
172470	spring khaki dodger pink sky	Manufacturer#2           	Brand#25  	PROMO ANODIZED BRASS	46	WRAP PACK 	1542.47	 fluffily exp
172471	lavender puff saddle drab brown	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED NICKEL	14	JUMBO CAN 	1543.47	olites sleep caref
172472	moccasin almond spring aquamarine blue	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED COPPER	9	LG CASE   	1544.47	ven platelets; f
172476	dark cream tomato thistle violet	Manufacturer#5           	Brand#55  	LARGE BURNISHED TIN	22	MED DRUM  	1548.47	ss packages. fluf
172477	yellow cyan red ghost moccasin	Manufacturer#4           	Brand#41  	LARGE ANODIZED NICKEL	46	WRAP BOX  	1549.47	cuses sleep blith
172481	aquamarine cream lavender moccasin lime	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED STEEL	13	WRAP PKG  	1553.48	s use carefull
172482	grey khaki lavender blue burlywood	Manufacturer#1           	Brand#11  	SMALL ANODIZED STEEL	17	MED CASE  	1554.48	e boldly. blithe
172485	pink cream blue papaya black	Manufacturer#1           	Brand#14  	LARGE POLISHED TIN	44	JUMBO BAG 	1557.48	long the bol
172486	moccasin bisque chiffon plum burnished	Manufacturer#5           	Brand#52  	STANDARD POLISHED NICKEL	4	WRAP PACK 	1558.48	ests are slyly stea
172487	slate blue rose lavender olive	Manufacturer#1           	Brand#13  	SMALL BRUSHED TIN	38	MED BOX   	1559.48	ronic theod
172491	snow maroon magenta indian drab	Manufacturer#4           	Brand#44  	PROMO PLATED NICKEL	30	WRAP JAR  	1563.49	. furiously 
172492	misty coral cyan indian burlywood	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED TIN	43	MED BOX   	1564.49	 the ideas. blithely 
172493	lace royal maroon saddle moccasin	Manufacturer#2           	Brand#24  	PROMO POLISHED TIN	36	MED PACK  	1565.49	s wak
172498	turquoise maroon lemon misty medium	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED COPPER	46	SM CASE   	1570.49	s aft
174967	azure dodger almond ivory rosy	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED TIN	33	LG PACK   	2041.96	 according 
174968	olive spring saddle black chiffon	Manufacturer#3           	Brand#31  	SMALL BURNISHED TIN	12	LG PKG    	2042.96	ideas. ironic depos
174970	lavender black moccasin smoke ghost	Manufacturer#5           	Brand#54  	MEDIUM PLATED STEEL	21	MED BOX   	2044.97	ironic deposits are
174974	frosted lemon sandy tan snow	Manufacturer#5           	Brand#52  	LARGE ANODIZED BRASS	12	MED BOX   	2048.97	e carefully
174977	rose drab almond thistle dodger	Manufacturer#2           	Brand#22  	STANDARD BURNISHED TIN	38	WRAP DRUM 	2051.97	ealthy
174978	chartreuse dark powder ivory floral	Manufacturer#3           	Brand#32  	LARGE ANODIZED TIN	7	JUMBO BAG 	2052.97	egular, express
174980	rosy drab beige pale slate	Manufacturer#1           	Brand#13  	ECONOMY PLATED STEEL	48	SM PACK   	2054.98	 blithe
174985	gainsboro peru hot chartreuse powder	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED STEEL	17	LG DRUM   	2059.98	fluffily. ironic
174986	orchid pink thistle cream slate	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED BRASS	23	SM BOX    	2060.98	mptotes. deposits
174987	cornflower ghost honeydew sky red	Manufacturer#5           	Brand#51  	PROMO BURNISHED NICKEL	18	SM CAN    	2061.98	ithely final depos
174988	slate hot pink coral sky	Manufacturer#1           	Brand#12  	ECONOMY BRUSHED TIN	49	SM JAR    	2062.98	ravely blithely unu
174989	aquamarine turquoise peach dark salmon	Manufacturer#1           	Brand#14  	STANDARD PLATED COPPER	10	MED PKG   	2063.98	even foxes 
174993	chiffon blush plum linen maroon	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED COPPER	45	MED BOX   	2067.99	 blithely bold p
174994	metallic chocolate almond beige cornsilk	Manufacturer#1           	Brand#12  	MEDIUM ANODIZED COPPER	34	WRAP CAN  	2068.99	 ironic id
174998	drab deep forest spring almond	Manufacturer#2           	Brand#22  	PROMO POLISHED COPPER	24	JUMBO BAG 	2072.99	lar dep
174999	azure lawn forest rose papaya	Manufacturer#4           	Brand#44  	ECONOMY POLISHED NICKEL	37	LG PKG    	2073.99	s. acc
175002	ghost navy saddle red khaki	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	10	LG CASE   	1077.00	s cajole regu
175003	frosted seashell burnished brown cream	Manufacturer#2           	Brand#21  	LARGE POLISHED STEEL	30	SM DRUM   	1078.00	y bold foxes sleep ab
175004	brown lemon honeydew firebrick aquamarine	Manufacturer#5           	Brand#52  	PROMO BURNISHED TIN	40	LG PACK   	1079.00	ents. quic
175008	navy red peru rose floral	Manufacturer#3           	Brand#34  	LARGE POLISHED NICKEL	44	MED PACK  	1083.00	sly ironic, pending th
175009	beige brown chiffon dim ivory	Manufacturer#4           	Brand#41  	LARGE BRUSHED COPPER	24	SM PACK   	1084.00	al accounts. slyly 
175010	smoke chocolate blanched cornsilk maroon	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED BRASS	26	WRAP PACK 	1085.01	ully a
175015	cream spring saddle thistle tomato	Manufacturer#2           	Brand#25  	MEDIUM BURNISHED TIN	27	MED DRUM  	1090.01	sits. care
177484	peach forest saddle lemon azure	Manufacturer#3           	Brand#35  	PROMO POLISHED COPPER	48	JUMBO BAG 	1561.48	 furiously against t
177485	medium saddle indian sky chiffon	Manufacturer#3           	Brand#32  	ECONOMY BURNISHED BRASS	30	LG DRUM   	1562.48	 slyly fi
177487	ghost white wheat light mint	Manufacturer#1           	Brand#12  	STANDARD BURNISHED COPPER	3	JUMBO PACK	1564.48	luffily express
177491	cyan drab pink purple midnight	Manufacturer#4           	Brand#42  	PROMO POLISHED BRASS	22	SM PKG    	1568.49	 foxes cajole ab
177494	royal yellow plum misty navy	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED COPPER	44	MED PACK  	1571.49	 silen
177495	deep wheat gainsboro snow lemon	Manufacturer#5           	Brand#55  	PROMO BURNISHED TIN	44	MED BAG   	1572.49	final packages
177497	midnight khaki purple peach firebrick	Manufacturer#3           	Brand#31  	LARGE BRUSHED STEEL	36	JUMBO DRUM	1574.49	ep. f
177502	cream moccasin burnished blanched pale	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED TIN	36	WRAP CAN  	1579.50	ages use carefully. s
177503	blanched steel frosted chocolate black	Manufacturer#5           	Brand#52  	SMALL POLISHED STEEL	49	WRAP CAN  	1580.50	ounts according t
177504	wheat powder light dodger navajo	Manufacturer#1           	Brand#15  	STANDARD PLATED STEEL	11	JUMBO BAG 	1581.50	y about the regular i
177505	violet white frosted brown misty	Manufacturer#1           	Brand#15  	LARGE BRUSHED BRASS	25	WRAP CAN  	1582.50	sts integrate blith
177506	cyan orange maroon azure floral	Manufacturer#4           	Brand#41  	SMALL BRUSHED TIN	13	MED BOX   	1583.50	x furiously along 
177510	khaki sienna ghost puff medium	Manufacturer#4           	Brand#42  	PROMO BRUSHED TIN	43	JUMBO PACK	1587.51	 always regu
177511	ivory goldenrod steel turquoise frosted	Manufacturer#5           	Brand#55  	LARGE BURNISHED COPPER	1	JUMBO BOX 	1588.51	lly. 
177515	grey drab lavender burlywood coral	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED NICKEL	44	WRAP BOX  	1592.51	rding to the
177516	hot papaya dark powder sienna	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	39	MED CAN   	1593.51	s caj
177519	burnished aquamarine dim chartreuse pink	Manufacturer#1           	Brand#12  	SMALL BURNISHED BRASS	34	SM BOX    	1596.51	- quickly regul
177520	cornsilk maroon salmon aquamarine lime	Manufacturer#4           	Brand#43  	LARGE POLISHED NICKEL	32	MED CAN   	1597.52	ns. slyly fluffy 
177521	antique forest pink seashell lawn	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED STEEL	20	JUMBO PACK	1598.52	c foxes. quic
177525	honeydew blush lime pale drab	Manufacturer#4           	Brand#45  	MEDIUM POLISHED COPPER	20	WRAP BOX  	1602.52	o the f
177526	tomato ghost metallic frosted lime	Manufacturer#5           	Brand#55  	LARGE ANODIZED BRASS	50	JUMBO BAG 	1603.52	g the furiously unusua
177527	blanched lime puff sienna snow	Manufacturer#2           	Brand#24  	SMALL POLISHED COPPER	44	SM JAR    	1604.52	althily ironic
177532	linen purple beige indian sienna	Manufacturer#1           	Brand#13  	LARGE POLISHED NICKEL	15	WRAP JAR  	1609.53	sly unusual pac
180001	aquamarine dodger puff cyan light	Manufacturer#1           	Brand#14  	ECONOMY PLATED BRASS	46	SM CASE   	1081.00	 packages. slyl
180002	lawn cyan khaki burlywood frosted	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED NICKEL	34	WRAP PKG  	1082.00	maintain
180004	navy beige brown rose powder	Manufacturer#2           	Brand#23  	STANDARD BURNISHED COPPER	1	WRAP PKG  	1084.00	y. fu
180008	navy indian beige gainsboro peru	Manufacturer#5           	Brand#51  	PROMO BURNISHED BRASS	42	SM PKG    	1088.00	th the silent, f
180011	floral antique powder magenta papaya	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED TIN	32	SM BAG    	1091.01	eposit
180012	red misty purple green midnight	Manufacturer#1           	Brand#14  	SMALL ANODIZED BRASS	40	WRAP PACK 	1092.01	ut the regul
180014	lemon lace sky saddle medium	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED BRASS	24	MED BAG   	1094.01	haggle slyly along th
180019	white lace chiffon medium firebrick	Manufacturer#3           	Brand#35  	STANDARD BRUSHED TIN	35	SM JAR    	1099.01	y ironic ideas de
180020	green moccasin firebrick navajo blush	Manufacturer#2           	Brand#24  	LARGE BURNISHED STEEL	14	MED DRUM  	1100.02	lly regu
180021	wheat black cornflower steel linen	Manufacturer#4           	Brand#42  	PROMO POLISHED COPPER	29	LG DRUM   	1101.02	s. carefully regu
180022	cyan bisque indian purple cream	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED STEEL	28	JUMBO CASE	1102.02	eep q
180023	aquamarine tan magenta antique lemon	Manufacturer#2           	Brand#23  	MEDIUM POLISHED BRASS	28	SM CAN    	1103.02	press ideas above t
180027	goldenrod wheat cyan beige chartreuse	Manufacturer#3           	Brand#34  	MEDIUM POLISHED COPPER	41	LG CASE   	1107.02	c packages serve care
180028	royal chartreuse midnight violet dark	Manufacturer#4           	Brand#42  	PROMO PLATED NICKEL	24	MED PKG   	1108.02	ly ironic ideas mold
180032	violet moccasin medium tomato honeydew	Manufacturer#3           	Brand#31  	LARGE BRUSHED NICKEL	15	WRAP PACK 	1112.03	ly slow pla
180033	dim salmon brown pale sandy	Manufacturer#1           	Brand#12  	STANDARD BURNISHED STEEL	45	LG CASE   	1113.03	usly. bold
180036	dark spring azure mint papaya	Manufacturer#4           	Brand#41  	STANDARD BURNISHED COPPER	34	MED PKG   	1116.03	 according to 
180037	turquoise smoke khaki navy tomato	Manufacturer#1           	Brand#11  	STANDARD POLISHED BRASS	46	LG BAG    	1117.03	r the car
180038	cornsilk peru tan cyan sandy	Manufacturer#1           	Brand#15  	SMALL BRUSHED COPPER	46	MED BOX   	1118.03	yly regular courts wak
180042	cream papaya metallic mint burnished	Manufacturer#1           	Brand#14  	STANDARD PLATED STEEL	48	WRAP PKG  	1122.04	 furiously expr
180043	dim cyan salmon firebrick seashell	Manufacturer#4           	Brand#41  	MEDIUM POLISHED TIN	21	JUMBO JAR 	1123.04	blithely e
180044	mint firebrick ivory dark ghost	Manufacturer#3           	Brand#35  	SMALL POLISHED NICKEL	42	MED BAG   	1124.04	 acco
180049	sienna snow orange plum red	Manufacturer#2           	Brand#21  	LARGE BRUSHED COPPER	37	SM PKG    	1129.04	ackages
182447	smoke lavender light frosted brown	Manufacturer#3           	Brand#33  	ECONOMY POLISHED TIN	17	WRAP JAR  	1529.44	 accou
182448	forest misty navajo dodger cornflower	Manufacturer#4           	Brand#43  	PROMO POLISHED NICKEL	48	SM BAG    	1530.44	nusual deposits.
182450	linen turquoise royal thistle frosted	Manufacturer#2           	Brand#21  	STANDARD BRUSHED BRASS	23	JUMBO PACK	1532.45	ly pen
182454	misty azure almond ghost khaki	Manufacturer#1           	Brand#11  	LARGE ANODIZED COPPER	13	MED CASE  	1536.45	lyly according to the 
182457	smoke lemon peru ghost dim	Manufacturer#5           	Brand#55  	LARGE PLATED BRASS	43	MED JAR   	1539.45	c ideas 
182458	dark purple saddle medium lavender	Manufacturer#5           	Brand#51  	SMALL ANODIZED COPPER	22	WRAP JAR  	1540.45	 carefully bold accou
182460	pale deep cream tan violet	Manufacturer#2           	Brand#24  	PROMO BURNISHED BRASS	15	MED CASE  	1542.46	x quickly express foxe
182465	pale bisque olive orchid blush	Manufacturer#5           	Brand#51  	PROMO POLISHED COPPER	10	SM PACK   	1547.46	l instruct
182466	forest lime cornflower white violet	Manufacturer#2           	Brand#23  	STANDARD POLISHED STEEL	4	LG PKG    	1548.46	egular acc
182467	blue white blush beige cream	Manufacturer#1           	Brand#14  	SMALL BURNISHED COPPER	27	MED BOX   	1549.46	eans are furious
182468	khaki lawn misty green firebrick	Manufacturer#1           	Brand#14  	STANDARD PLATED TIN	1	LG BOX    	1550.46	ainst the bli
182469	almond frosted drab orchid mint	Manufacturer#4           	Brand#43  	PROMO BRUSHED TIN	14	SM JAR    	1551.46	s. accoun
182473	beige frosted wheat chocolate firebrick	Manufacturer#5           	Brand#51  	SMALL ANODIZED COPPER	20	MED BOX   	1555.47	kly exp
182474	steel azure deep frosted honeydew	Manufacturer#5           	Brand#53  	LARGE BURNISHED BRASS	1	LG PKG    	1556.47	 the ironic 
182478	chartreuse khaki tomato blush smoke	Manufacturer#3           	Brand#34  	PROMO ANODIZED BRASS	18	JUMBO PKG 	1560.47	eas. ironic ac
182479	sienna purple blanched tan floral	Manufacturer#1           	Brand#11  	STANDARD PLATED BRASS	36	WRAP JAR  	1561.47	y bold requests nag 
182482	navy coral tan light bisque	Manufacturer#5           	Brand#52  	STANDARD BRUSHED COPPER	19	LG PACK   	1564.48	ously even foxes r
182483	antique almond brown lemon medium	Manufacturer#5           	Brand#51  	MEDIUM POLISHED TIN	21	JUMBO CASE	1565.48	n packages unwind
182484	rose navy burlywood dodger antique	Manufacturer#1           	Brand#13  	LARGE POLISHED COPPER	42	WRAP BOX  	1566.48	cuses. 
182488	pink dim light brown floral	Manufacturer#1           	Brand#12  	PROMO POLISHED TIN	1	WRAP BAG  	1570.48	to cajole blithely c
182489	coral rosy deep azure yellow	Manufacturer#4           	Brand#42  	LARGE PLATED COPPER	12	WRAP PKG  	1571.48	furiously. unusua
182490	rosy dodger lace beige maroon	Manufacturer#3           	Brand#35  	ECONOMY POLISHED STEEL	37	WRAP DRUM 	1572.49	 the special, regu
182495	snow burlywood ivory wheat royal	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED TIN	37	MED CASE  	1577.49	ainst
184965	drab light sandy medium midnight	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	30	JUMBO PKG 	2049.96	es haggle 
184966	peach hot powder cornsilk magenta	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED STEEL	40	SM JAR    	2050.96	nag fluffily. d
184968	light midnight chiffon spring blue	Manufacturer#4           	Brand#42  	PROMO PLATED COPPER	12	LG BOX    	2052.96	ounts haggle acro
184972	rosy floral khaki dark sandy	Manufacturer#3           	Brand#32  	STANDARD PLATED COPPER	22	JUMBO PACK	2056.97	ect ca
184975	antique hot ghost pale lawn	Manufacturer#3           	Brand#34  	LARGE ANODIZED BRASS	44	JUMBO CAN 	2059.97	instr
184976	cornflower olive lemon dodger peach	Manufacturer#1           	Brand#11  	PROMO BURNISHED NICKEL	15	LG PACK   	2060.97	usily. slyly i
184978	blush turquoise rose coral bisque	Manufacturer#4           	Brand#44  	SMALL ANODIZED STEEL	25	JUMBO CASE	2062.97	. perm
184983	wheat lavender pale dim saddle	Manufacturer#4           	Brand#45  	PROMO ANODIZED TIN	43	MED CAN   	2067.98	s. slyly bo
184984	antique lavender slate magenta powder	Manufacturer#3           	Brand#33  	LARGE BURNISHED BRASS	5	WRAP JAR  	2068.98	egular, ex
184985	burlywood peach lime metallic frosted	Manufacturer#1           	Brand#11  	PROMO POLISHED STEEL	38	JUMBO CASE	2069.98	ress fo
184986	hot dodger magenta burnished deep	Manufacturer#1           	Brand#12  	STANDARD BURNISHED NICKEL	20	SM BAG    	2070.98	 deposits agains
184987	cornflower wheat grey chiffon firebrick	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	32	JUMBO BAG 	2071.98	its haggle care
184991	sandy lace maroon smoke metallic	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED NICKEL	6	MED BAG   	2075.99	, even accounts. re
184992	floral lawn ghost brown honeydew	Manufacturer#3           	Brand#33  	PROMO PLATED COPPER	1	JUMBO BAG 	2076.99	r the
184996	midnight brown coral salmon frosted	Manufacturer#5           	Brand#54  	MEDIUM PLATED COPPER	25	MED DRUM  	2080.99	y unusual dep
184997	deep navy antique peru firebrick	Manufacturer#1           	Brand#14  	PROMO BURNISHED TIN	48	MED PKG   	2081.99	ar instruc
185000	puff olive green dodger lace	Manufacturer#2           	Brand#24  	LARGE ANODIZED COPPER	2	SM CAN    	1085.00	nic requests unwind-- 
185001	bisque black antique moccasin light	Manufacturer#5           	Brand#54  	SMALL PLATED TIN	22	MED DRUM  	1086.00	inst th
185002	drab misty navajo peru purple	Manufacturer#2           	Brand#25  	MEDIUM PLATED NICKEL	46	MED PACK  	1087.00	 ironic inst
185006	dim black linen spring deep	Manufacturer#2           	Brand#23  	MEDIUM PLATED NICKEL	8	SM PACK   	1091.00	carefully i
185007	seashell steel pale thistle khaki	Manufacturer#1           	Brand#11  	STANDARD POLISHED TIN	43	JUMBO PACK	1092.00	 acco
185008	light papaya saddle olive snow	Manufacturer#4           	Brand#41  	LARGE PLATED COPPER	40	LG BAG    	1093.00	s sleep quickly amo
185013	navy plum mint linen lemon	Manufacturer#4           	Brand#45  	ECONOMY POLISHED BRASS	23	WRAP PKG  	1098.01	t the carefu
187483	chartreuse drab lime blue floral	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	47	MED DRUM  	1570.48	 sleep furiously
187484	cream red antique chocolate brown	Manufacturer#3           	Brand#31  	STANDARD BURNISHED TIN	1	LG DRUM   	1571.48	ly unus
187486	lavender burlywood burnished linen hot	Manufacturer#3           	Brand#31  	SMALL BURNISHED TIN	29	WRAP PACK 	1573.48	sual ideas 
187490	red drab burlywood dodger pale	Manufacturer#5           	Brand#52  	STANDARD ANODIZED BRASS	38	LG DRUM   	1577.49	. even, pending i
187493	slate firebrick dodger ivory medium	Manufacturer#5           	Brand#53  	STANDARD PLATED NICKEL	13	WRAP DRUM 	1580.49	cial depe
187494	blush chocolate seashell blue snow	Manufacturer#4           	Brand#45  	SMALL POLISHED COPPER	6	SM CASE   	1581.49	eposits. req
187496	forest spring blush misty chocolate	Manufacturer#1           	Brand#15  	PROMO POLISHED NICKEL	28	JUMBO PKG 	1583.49	inal excuses cajole f
187501	blanched sienna plum mint chocolate	Manufacturer#2           	Brand#21  	SMALL PLATED COPPER	31	MED BOX   	1588.50	 even asymptote
187502	salmon magenta aquamarine puff blue	Manufacturer#4           	Brand#44  	SMALL BRUSHED TIN	1	MED BAG   	1589.50	ate alongside of the
187503	moccasin salmon goldenrod aquamarine pale	Manufacturer#4           	Brand#43  	ECONOMY POLISHED TIN	15	MED PACK  	1590.50	detect s
187504	indian gainsboro ghost almond brown	Manufacturer#1           	Brand#14  	LARGE BRUSHED STEEL	26	WRAP BAG  	1591.50	g the closely reg
187505	pale wheat lace orchid cornflower	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	41	LG JAR    	1592.50	ffily 
187509	aquamarine drab brown chiffon goldenrod	Manufacturer#3           	Brand#33  	MEDIUM POLISHED COPPER	36	SM BAG    	1596.50	 the slyly bol
187510	midnight royal honeydew chartreuse cream	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	49	LG CASE   	1597.51	ly unusual deposi
187514	blanched almond ghost chiffon lime	Manufacturer#4           	Brand#45  	LARGE POLISHED BRASS	50	JUMBO PACK	1601.51	regular 
187515	seashell sandy yellow rose floral	Manufacturer#2           	Brand#21  	STANDARD POLISHED STEEL	35	JUMBO CAN 	1602.51	ully even deposits 
187518	dodger deep salmon spring cornsilk	Manufacturer#2           	Brand#21  	SMALL ANODIZED BRASS	22	WRAP BAG  	1605.51	quests above the s
187519	aquamarine saddle indian khaki pink	Manufacturer#5           	Brand#55  	LARGE BRUSHED COPPER	18	JUMBO PACK	1606.51	fily even pinto beans 
187520	drab green floral royal sky	Manufacturer#2           	Brand#22  	MEDIUM POLISHED TIN	19	SM BOX    	1607.52	e quickly a
187524	spring wheat forest cream rosy	Manufacturer#4           	Brand#43  	SMALL ANODIZED NICKEL	14	JUMBO PACK	1611.52	to beans cajole 
187525	antique steel dim lime blush	Manufacturer#4           	Brand#43  	PROMO ANODIZED NICKEL	19	JUMBO CASE	1612.52	 deposits. blit
187526	khaki navy powder seashell pale	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	3	MED BOX   	1613.52	ometimes regular dugou
187531	aquamarine beige navajo goldenrod almond	Manufacturer#2           	Brand#23  	PROMO PLATED STEEL	22	MED CAN   	1618.53	riously re
190001	powder coral chiffon burnished bisque	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED NICKEL	26	WRAP BOX  	1091.00	ly busy deposi
190002	peru coral rosy azure green	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	21	SM PKG    	1092.00	 express, daring sh
190004	ivory almond honeydew metallic dodger	Manufacturer#4           	Brand#44  	PROMO PLATED NICKEL	23	MED DRUM  	1094.00	 blithely regular t
190008	cream dark peru thistle gainsboro	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED STEEL	46	WRAP CASE 	1098.00	 pinto beans. fur
190011	metallic beige saddle maroon cornflower	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED COPPER	2	WRAP DRUM 	1101.01	sly among th
190012	cornsilk pink deep firebrick indian	Manufacturer#3           	Brand#35  	ECONOMY POLISHED COPPER	22	MED DRUM  	1102.01	he fluffi
190014	medium drab sandy tan misty	Manufacturer#2           	Brand#23  	SMALL ANODIZED STEEL	21	JUMBO JAR 	1104.01	ins. fl
190019	lemon sky olive turquoise drab	Manufacturer#5           	Brand#52  	LARGE POLISHED STEEL	25	WRAP PKG  	1109.01	jole acro
190020	sienna blue chocolate lawn maroon	Manufacturer#3           	Brand#35  	STANDARD POLISHED BRASS	36	MED CASE  	1110.02	int ironic p
190021	lace snow plum chartreuse light	Manufacturer#2           	Brand#23  	MEDIUM POLISHED TIN	26	WRAP CAN  	1111.02	y even accounts--
190022	beige maroon light azure lavender	Manufacturer#4           	Brand#44  	MEDIUM PLATED STEEL	49	SM BOX    	1112.02	counts are ar
190023	navy mint medium linen rosy	Manufacturer#1           	Brand#12  	MEDIUM ANODIZED COPPER	5	JUMBO PACK	1113.02	ly even multi
190027	medium green frosted linen puff	Manufacturer#1           	Brand#13  	SMALL PLATED STEEL	46	JUMBO BAG 	1117.02	of the flu
190028	lawn tomato brown ivory saddle	Manufacturer#4           	Brand#41  	STANDARD BRUSHED COPPER	44	LG CASE   	1118.02	y ironic t
190032	sandy thistle spring violet lawn	Manufacturer#5           	Brand#54  	MEDIUM PLATED COPPER	29	LG DRUM   	1122.03	e pending
190033	dark chartreuse mint azure dodger	Manufacturer#4           	Brand#42  	STANDARD BURNISHED COPPER	46	WRAP CAN  	1123.03	nding ideas. 
190036	yellow chartreuse almond peru rose	Manufacturer#2           	Brand#23  	LARGE BRUSHED NICKEL	7	WRAP BOX  	1126.03	iously even 
190037	bisque papaya tomato puff lawn	Manufacturer#1           	Brand#14  	MEDIUM POLISHED STEEL	23	WRAP CAN  	1127.03	s wake furiously. slyl
190038	blue peru tan tomato pale	Manufacturer#3           	Brand#31  	SMALL PLATED STEEL	30	SM PKG    	1128.03	yly regular excuse
190042	sienna cornsilk grey blush brown	Manufacturer#3           	Brand#32  	MEDIUM POLISHED BRASS	11	JUMBO PACK	1132.04	n about th
190043	spring indian maroon misty frosted	Manufacturer#5           	Brand#55  	STANDARD PLATED TIN	25	SM CASE   	1133.04	 requests. account
190044	spring black grey chiffon seashell	Manufacturer#2           	Brand#23  	SMALL BRUSHED STEEL	27	SM BAG    	1134.04	lithely bold ac
190049	yellow drab snow salmon maroon	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED STEEL	18	WRAP DRUM 	1139.04	ow. carefully special
192444	cyan thistle salmon yellow lime	Manufacturer#2           	Brand#23  	PROMO PLATED NICKEL	5	MED BOX   	1536.44	deposits. regular, i
192445	sandy dim cornsilk green hot	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	1	MED BOX   	1537.44	s. blithely
192447	brown powder lace purple sky	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED STEEL	25	JUMBO BAG 	1539.44	g the fluffily 
192451	yellow chiffon saddle black midnight	Manufacturer#1           	Brand#12  	SMALL BRUSHED COPPER	4	WRAP CAN  	1543.45	ve ac
192454	frosted lace cornsilk white thistle	Manufacturer#5           	Brand#55  	ECONOMY POLISHED STEEL	37	LG PACK   	1546.45	requests. quick, stea
192455	azure violet blush khaki lime	Manufacturer#3           	Brand#35  	STANDARD PLATED TIN	47	MED PACK  	1547.45	ithely unusual 
192457	orange firebrick chiffon peach mint	Manufacturer#4           	Brand#41  	STANDARD POLISHED COPPER	2	LG BAG    	1549.45	rnes. carefully fin
192462	khaki royal metallic lemon dim	Manufacturer#5           	Brand#51  	PROMO ANODIZED STEEL	1	LG PKG    	1554.46	packages accordin
192463	goldenrod lemon saddle violet tomato	Manufacturer#3           	Brand#33  	SMALL BRUSHED STEEL	29	MED PACK  	1555.46	otes impress f
192464	navajo midnight turquoise cyan almond	Manufacturer#3           	Brand#32  	MEDIUM POLISHED STEEL	30	JUMBO BOX 	1556.46	es nag blit
192465	misty dodger cornflower turquoise bisque	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED STEEL	50	SM JAR    	1557.46	ly according to
192466	medium peru aquamarine maroon light	Manufacturer#2           	Brand#21  	PROMO BRUSHED COPPER	22	MED CAN   	1558.46	deposits. pending, re
192470	plum tomato navy sandy dark	Manufacturer#5           	Brand#52  	LARGE POLISHED NICKEL	14	MED PACK  	1562.47	lites after t
192471	linen firebrick cornflower chiffon metallic	Manufacturer#4           	Brand#44  	MEDIUM PLATED BRASS	27	WRAP CASE 	1563.47	 stealthy, final theo
192475	black saddle green sky ghost	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED BRASS	47	JUMBO CASE	1567.47	e above the ironic p
192476	navajo ivory orchid violet goldenrod	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED NICKEL	9	SM BOX    	1568.47	e of the blithely qu
192479	plum bisque almond midnight slate	Manufacturer#1           	Brand#12  	SMALL PLATED TIN	21	WRAP PACK 	1571.47	s requests sleep f
192480	plum brown chiffon tan aquamarine	Manufacturer#1           	Brand#13  	STANDARD BRUSHED TIN	38	JUMBO BOX 	1572.48	efully pending theodo
192481	blush floral antique rosy honeydew	Manufacturer#2           	Brand#22  	LARGE POLISHED STEEL	39	LG BOX    	1573.48	use ne
192485	forest peach midnight wheat mint	Manufacturer#4           	Brand#45  	LARGE BURNISHED TIN	33	JUMBO BAG 	1577.48	 accordi
192486	indian lime steel sandy khaki	Manufacturer#1           	Brand#13  	PROMO ANODIZED COPPER	24	LG BAG    	1578.48	ecial asymptotes
192487	rose deep rosy ivory snow	Manufacturer#2           	Brand#21  	LARGE BURNISHED NICKEL	27	JUMBO CASE	1579.48	ter the furious
192492	ivory black rose metallic peru	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED COPPER	22	SM PKG    	1584.49	ual ideas wake
194963	hot lime plum purple aquamarine	Manufacturer#5           	Brand#54  	PROMO ANODIZED COPPER	26	MED DRUM  	2057.96	o beans haggle
194964	tomato black lime sandy dim	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED COPPER	3	WRAP CASE 	2058.96	se th
194966	plum gainsboro lemon peru wheat	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED BRASS	24	SM BAG    	2060.96	 fluff
194970	ivory goldenrod dim chiffon royal	Manufacturer#3           	Brand#31  	SMALL PLATED BRASS	8	LG BOX    	2064.97	arefully fl
194973	metallic slate bisque blush rosy	Manufacturer#1           	Brand#12  	ECONOMY PLATED STEEL	24	MED PACK  	2067.97	ges. special, f
194974	magenta papaya rose salmon powder	Manufacturer#1           	Brand#12  	PROMO BURNISHED TIN	48	MED CAN   	2068.97	riously iron
194976	beige blanched yellow peach aquamarine	Manufacturer#5           	Brand#51  	PROMO BRUSHED NICKEL	22	MED PACK  	2070.97	ily. c
194981	thistle purple ghost cornflower maroon	Manufacturer#5           	Brand#54  	ECONOMY PLATED BRASS	11	WRAP BOX  	2075.98	aggle furiously 
194982	sienna burlywood drab seashell sandy	Manufacturer#5           	Brand#51  	ECONOMY BRUSHED BRASS	47	MED CAN   	2076.98	st the fluffily regu
194983	seashell blue magenta pale aquamarine	Manufacturer#2           	Brand#23  	LARGE ANODIZED COPPER	1	WRAP BOX  	2077.98	ons. blithely b
194984	chartreuse light blanched navy deep	Manufacturer#5           	Brand#54  	ECONOMY POLISHED BRASS	21	MED PACK  	2078.98	ep fluffily
194985	ghost seashell chartreuse blanched honeydew	Manufacturer#1           	Brand#13  	ECONOMY ANODIZED COPPER	22	MED CASE  	2079.98	gly d
194989	peach seashell lawn orange sienna	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	28	LG BOX    	2083.98	hy depo
194990	medium firebrick ghost frosted navy	Manufacturer#3           	Brand#35  	SMALL POLISHED NICKEL	19	JUMBO BAG 	2084.99	lithely. blith
194994	misty deep frosted bisque smoke	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED COPPER	48	JUMBO CASE	2088.99	e flu
194995	burlywood orange sienna forest chocolate	Manufacturer#3           	Brand#31  	SMALL PLATED BRASS	35	SM BAG    	2089.99	packages c
194998	antique snow hot violet sky	Manufacturer#2           	Brand#23  	STANDARD POLISHED TIN	41	LG CAN    	2092.99	slyly final instructio
194999	navajo goldenrod navy royal white	Manufacturer#3           	Brand#33  	STANDARD ANODIZED NICKEL	31	JUMBO BOX 	2093.99	even platelets haggle 
195000	spring green turquoise chiffon lemon	Manufacturer#3           	Brand#31  	PROMO PLATED TIN	30	JUMBO DRUM	1095.00	sts about the dep
195004	medium snow brown hot dim	Manufacturer#2           	Brand#21  	ECONOMY BURNISHED BRASS	12	SM CASE   	1099.00	thless
195005	linen royal dodger grey black	Manufacturer#1           	Brand#12  	LARGE BRUSHED BRASS	50	MED PKG   	1100.00	 slyly 
195006	peach floral rose midnight green	Manufacturer#2           	Brand#23  	PROMO ANODIZED COPPER	18	JUMBO PACK	1101.00	ng package
195011	beige chartreuse drab royal chiffon	Manufacturer#4           	Brand#44  	LARGE PLATED STEEL	14	JUMBO PKG 	1106.01	lites. quickly un
197482	lace cream orange metallic violet	Manufacturer#5           	Brand#52  	MEDIUM POLISHED COPPER	43	MED DRUM  	1579.48	r deposits use
197483	magenta slate chartreuse cream chiffon	Manufacturer#3           	Brand#33  	SMALL POLISHED COPPER	13	JUMBO DRUM	1580.48	ans. furiously unusua
197485	beige lace burnished steel peru	Manufacturer#2           	Brand#25  	STANDARD BRUSHED COPPER	41	WRAP PKG  	1582.48	refully ironic, 
197489	green aquamarine almond cyan chartreuse	Manufacturer#4           	Brand#43  	LARGE POLISHED STEEL	15	LG BAG    	1586.48	ches. slyly u
197492	floral rosy pale chiffon yellow	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED TIN	43	LG BAG    	1589.49	key pl
197493	indian coral cornsilk sky steel	Manufacturer#1           	Brand#15  	STANDARD BURNISHED STEEL	21	SM DRUM   	1590.49	ely? special, final de
197495	forest rose aquamarine misty orange	Manufacturer#1           	Brand#15  	STANDARD BRUSHED TIN	11	MED BAG   	1592.49	ess i
197500	brown beige chiffon lace indian	Manufacturer#5           	Brand#53  	LARGE POLISHED STEEL	39	JUMBO CASE	1597.50	 packa
197501	chocolate tomato green dark chiffon	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	7	JUMBO CAN 	1598.50	y special asymptote
197502	dodger dark frosted red saddle	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED STEEL	17	JUMBO BOX 	1599.50	quickly across the pac
197503	snow peach royal navajo smoke	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED COPPER	33	WRAP BAG  	1600.50	quickly. permanent
197504	chocolate goldenrod yellow burlywood pink	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED COPPER	13	SM CASE   	1601.50	 regular exc
197508	turquoise sky wheat snow tan	Manufacturer#5           	Brand#54  	ECONOMY POLISHED NICKEL	14	SM CASE   	1605.50	he fluf
197509	rose aquamarine black indian thistle	Manufacturer#3           	Brand#33  	PROMO POLISHED NICKEL	7	MED CASE  	1606.50	ithe accounts boost 
197513	violet misty medium cornflower lavender	Manufacturer#4           	Brand#42  	LARGE POLISHED STEEL	49	MED PACK  	1610.51	e. slyl
197514	lace red blush frosted grey	Manufacturer#5           	Brand#51  	PROMO PLATED NICKEL	37	LG CAN    	1611.51	long the spe
197517	linen light ivory cream chartreuse	Manufacturer#3           	Brand#31  	MEDIUM PLATED BRASS	17	LG PKG    	1614.51	as carefully! ca
197518	salmon peach yellow purple sky	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	10	WRAP DRUM 	1615.51	old accounts sl
197519	rosy misty hot drab frosted	Manufacturer#3           	Brand#34  	STANDARD BRUSHED STEEL	4	SM CAN    	1616.51	unusual accounts ha
197523	beige chiffon gainsboro dark drab	Manufacturer#4           	Brand#41  	MEDIUM POLISHED TIN	38	SM CAN    	1620.52	usual instruction
197524	linen black mint steel puff	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED STEEL	41	SM PKG    	1621.52	ests. quickly un
197525	coral bisque tomato lavender hot	Manufacturer#3           	Brand#33  	STANDARD BURNISHED STEEL	26	JUMBO BAG 	1622.52	 ironic gifts wake fu
197530	smoke orchid navy saddle deep	Manufacturer#4           	Brand#42  	STANDARD BURNISHED TIN	16	SM BAG    	1627.53	foxes after th
\.


--
-- Data for Name: partsupp; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.partsupp (ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment) FROM stdin;
1	2	3325	771.64	, even theodolites. regular, final theodolites eat after the carefully pending foxes. furiously regular deposits sleep slyly. carefully bold realms above the ironic dependencies haggle careful
2	3	8895	378.49	nic accounts. final accounts sleep furiously about the ironic, bold packages. regular, regular accounts
4	5	1339	113.97	 carefully unusual ideas. packages use slyly. blithely final pinto beans cajole along the furiously express requests. regular orbits haggle carefully. care
8	9	6834	249.63	lly ironic accounts solve express, unusual theodolites. special packages use quickly. quickly fin
11	12	4540	709.87	thely across the blithely unusual requests. slyly regular instructions wake slyly ironic theodolites. requests haggle blithely above the blithely brave p
12	13	3610	659.73	jole bold theodolites. final packages haggle! carefully regular deposits play furiously among the special ideas. quickly ironic packages detect quickly carefully final
14	15	5278	650.07	e quickly among the furiously ironic accounts. special, final sheaves against the
19	20	1416	144.80	o beans. even packages nag boldly according to the bold, special deposits. ironic packages after the pinto beans nag above the quickly ironic requests. bl
20	21	2927	675.54	s, ironic deposits haggle across the quickly bold asymptotes. express, ironic pinto beans wake carefully enticingly special foxes. requests are at the c
21	22	6571	944.44	ing instructions impress bold foxes. ironic pinto beans use. thinly even asymptotes cajole ironic packages. quickly ironic pinto beans detect slyly regular deposits. ruthlessly even deposits are. sl
22	23	4410	786.18	even accounts. final excuses try to sleep regular, even packages. carefully express dolphins cajole; furiously special pinto bea
23	24	2739	460.12	platelets against the furiously bold Tiresias dazzle quickly into the special, bold courts. silent, regular instructions wake blithely ironic multipliers. ideas
27	28	2111	444.01	the even, ironic deposits. theodolites along the ironic, final dolphins cajole slyly quickly bold asymptotes. furiously regular theodolites integrate furiously furiously bold requests. carefully
28	29	6643	204.86	y ironic deposits above the slyly final deposits sleep furiously above the final deposits. quickly even i
32	33	2203	406.03	es? slyly enticing dugouts haggle carefully. regular packages alongside of the asymptotes are carefull
33	34	4028	891.46	, pending requests affix slyly. slyly ironic deposits wake accounts. express accounts sleep slowly. ironic, express accounts run carefully fluffily final dependencies. furiously unusual ideas
36	37	3907	630.91	al deposits detect fluffily fluffily unusual sauternes. carefully regular requests against the car
37	38	7171	824.96	usly into the slyly final requests. ironic accounts are furiously furiously ironic i
38	39	1226	570.11	 slyly even pinto beans. blithely special requests nag slyly about the ironic packages. 
42	43	2893	716.81	requests nag. furiously brave packages boost at the furiously even waters. slyly pending ideas nag carefully caref
43	44	3211	805.78	gular accounts. bold theodolites nag slyly. quickly express excuses use blithely. blithely even ideas boost fluffily! blithely unusual ideas detect bli
44	45	486	164.22	 final notornis throughout the unusual pinto beans are about the special accounts. bold packages sleep fluffily above the
49	50	9056	35.11	 bold deposits? final, bold pinto beans are furiously slyly regular packages. sly
2511	12	6187	421.72	ole slyly ironic accounts. carefully express dependencies wake. blithely even requ
2501	2	6980	379.98	across the slyly final instructions: carefully unusual sheaves wake along the blithely ironic foxes. slyly even deposits doubt quickly durin
2502	3	4286	502.00	kages haggle quickly after the stealthily regular packages. requests haggle blithely. quickly even foxes nag quickly even courts. regular, regular requ
2504	5	5946	480.90	osits. fluffily even notornis use alongside of the bravely even ideas. silent deposits wake. slyly pending deposits nag above the furiously spe
2508	9	8509	733.41	s detect furiously after the blithely bold accounts. unusual platelet
2512	13	5983	821.75	 nag quickly fluffily regular instructions. bold accounts across the quickly final pinto beans sleep ca
2514	15	5433	350.76	le above the slyly ironic accounts. bold pinto beans sleep carefully. pending, ironic accounts maintain instruct
2519	20	9355	414.23	ic accounts wake furiously. foxes are. regular requests cajole furiously. special deposits eat. pending, express cour
2520	21	6975	963.04	 accounts haggle carefully. quickly ironic accounts cajole furiously about the unusual, final deposits. even requests kindle after
2521	22	3984	187.13	ully final requests. carefully silent pinto beans cajole carefully according to the carefully ironic ideas. furiously regular dependencies haggle carefully bold requests; bold foxes i
2522	23	4279	691.60	dolites haggle furiously slyly bold requests. blithely 
2523	24	9912	934.97	mong the even theodolites sleep bold deposits. carefully final packages cajole furiously. regular accounts are blithely. even ideas cajole quick
2527	28	2706	178.90	 courts; furiously ironic theodolites promise blithely along the sly packages. quickly unusual requests haggle blithely pending accounts. quickly dogged accounts again
2528	29	2594	194.33	ronic instructions use slyly even requests. carefully regular deposits cajole furiously alongside of the furiously ironic 
2532	33	4349	489.16	 pains. carefully ironic dolphins haggle. carefully express deposits integrate.
2533	34	3129	70.78	ly regular pinto beans. even, unusual instructions boost across the blithely final dependencies. si
2536	37	365	538.60	ole fluffily across the express, final packages. ironic courts according to the fluffily special instructions wake slyly above the accounts. quickly special excuses are slyly quickly bold accoun
2537	38	8426	703.01	slyly. blithely bold deposits cajole across the idly regular accounts. deposits are 
2538	39	3598	462.03	 the regular dolphins haggle about the unusual foxes. car
2542	43	6987	940.54	about the slyly even platelets unwind about the stealthily express asymptotes. final, even accounts impress ca
2543	44	1586	754.12	requests haggle. dinos among the carefully pending 
2544	45	7031	892.02	 quickly regular deposits cajole quickly carefully pending gifts. regular, express ideas shall boost. unusual theodolites wake slyly quickly regula
22543	50	7081	167.53	leep slyly ironic instructions. dinos according to the regular, ev
2549	50	5890	429.22	 may nag. fluffily ironic theodolites integrate slyly after the slyly even foxes. deposits haggle above the even, regular ideas. express, brave dinos a
5001	2	3018	557.66	ix slyly across the pending packages. dogged foxes was blithely. car
5002	3	6996	739.71	ely. regular courts unwind regular, regular pinto bean
5004	5	5917	247.03	xpress courts. carefully even deposits are against the sp
5008	9	4667	603.70	fluffily unusual foxes cajole furiously. silent t
5011	12	1201	478.62	s. pending, bold courts sleep. ruthlessly final deposits haggle. unusual pinto beans impress slyly bold ideas: fluffily regular accounts are slyly carefully pending excuses. pending
5012	13	1427	820.64	carefully dogged dependencies must have to cajole against the regular, ironic ideas. furious requests alongside of the slyly expre
5014	15	3159	152.65	s wake quickly against the ideas; carefully ruthless accounts mold blithely pending pinto beans. slyly silent instructions according to the bold packages haggle quickly final foxes. carefully bold 
5019	20	3563	881.14	 final, regular theodolites cajole in place of the fluffily final ideas. ruthless packages cajole flu
5020	21	939	457.11	aggle furiously! ironic, express packages sleep blithely. orbits haggle quickly against the regular
5021	22	3432	168.45	ages poach fluffily against the slyly pending courts. slyly pending foxes
5022	23	2845	827.61	ronic foxes detect furiously. carefully unusual escapades print furiously. even requests 
5023	24	6692	502.28	 of the slyly regular accounts. furiously bold deposits about the slowly final deposits run fluffily theodolites. sly account
5027	28	3794	521.90	ve the regular foxes are quickly express deposits. final excuses haggle furiously furiously even requests. final deposits use blithely according to the carefully regular theodo
5028	29	4142	17.67	n silently blithely regular packages. carefully final accoun
5032	33	47	200.61	detect slyly. quickly final packages detect along the doggedly final re
5033	34	3994	821.69	 dolphins doubt brave ideas. even accounts cajole. always ironic warthogs above the fluffily express theodolites sublate enticingly ironic pinto beans. pac
5036	37	2859	533.75	s. ironic, final requests maintain about the carefully even requests. idly pendin
5037	38	7000	331.44	ents. slyly special deposits may are pending, final packages? final platelets
5038	39	605	563.11	ts haggle against the unusual ideas. pinto beans engage fluffily. quickly unusual packages integrate slyly against the thinl
5042	43	5490	491.04	ously pending packages according to the slyly unusual foxes wake blithely fluffily ironic packages. slyly special deposits at the ironic, pending packages are furi
5043	44	8128	486.85	es haggle quickly blithely pending pinto beans. requests according to the furiously permanent deposits haggle slyly slyly special accounts. unusual, ironic deposits after the quickly ev
5044	45	4373	186.87	ully ironic deposits. ironic, even deposits could have to cajole quickly above the fl
5049	50	5096	686.34	ly ironic theodolites print blithely according to the furiously final deposits: ruthless requests wake after the unusual theodolites. special foxes us
7501	2	896	538.81	arefully special packages. ironic dependencies print fluffily along the furi
7502	3	4436	377.80	instructions. slyly even decoys are requests. regula
7504	5	7992	523.49	l platelets. carefully regular requests among the fluffily bold packages detect carefully about the carefully regular foxes. idle, ironic packages haggle slyly along the final sautern
7508	9	642	794.89	oost furiously. bravely pending pinto beans haggle carefully. bold pinto beans wake fluffily idly ironic foxes. carefully pendi
7511	12	1705	906.57	efully ironic accounts hinder. furiously silent deposits use quickly final requests. furiously express theodolites wake. furiously regular 
7512	13	5277	697.83	ve the slyly regular accounts. regular, final packages haggle finally. carefully even requests across the unusual deposits use furiously above the furiously iro
7514	15	9356	635.71	o beans. final instructions wake across the express foxes: final platelets across the ironic platelets use slyly thinly pending pac
7519	20	8610	567.15	 pending ideas above the slyly regular instructions cajole slyly around the blithely ironic requests. ironically ironic deposits maintain along the express accounts
7520	21	1018	217.06	y even forges boost carefully express pinto beans. silent instructions maintain quickly furiously bold packages. express excuses about the special, express i
7521	22	9636	162.90	iously. furiously ironic ideas wake after the furiously final dependencies. always pending deposits according to the ironic, final deposits cajo
7522	23	5373	793.79	kly furiously unusual pinto beans. carefully special courts boost blithely among the blithely even foxes: quickly final accounts nag carefully final, special packages. deposits haggle carefully; sly
7523	24	7719	333.11	fully ironic accounts detect final pinto beans. final accounts cajole furious
7527	28	178	447.68	 after the ironic dependencies cajole around the dogged, pending courts. blithely regular deposits wake fluffily furiously idle accounts. fluffily even theodolites according to the
7528	29	4053	659.68	 along the slyly even pinto beans. pending ideas boost carefully accounts. waters use bravely against the quiet deposits. deposits nag at the instructions. slyly reg
7532	33	4876	655.16	sits. dependencies maintain furiously around the regular, 
7533	34	6232	544.55	y. slyly unusual accounts after the always even packages haggle slyly along 
7536	37	5631	930.75	quickly ironic pinto beans haggle furiously carefully bold platelets. ironic accounts according to the slyly regular dugouts haggle carefully regular, regular pack
7537	38	6550	519.97	ter the quickly even instructions cajole fluffily unusual ideas. carefully special pinto beans boost quickly along the furiously regular requests. ironic packages are blith
7538	39	2031	59.04	e furiously. quickly final asymptotes should solve carefully regular requests. blithely regular accounts use quickly. blithely ironi
7542	43	6962	714.87	lithe, final instructions cajole carefully ironic deposits. regular ideas are blithely express accounts. slyl
7543	44	3434	594.24	riously final instructions. blithely regular requests unwind ideas. slyly express instructions cajole after the slyly unusual instructions. regular theodolites use slyly after the furiously final 
7544	45	3191	707.40	nic packages believe blithely. blithely ironic foxes wake finally thin deposits. quickly express accounts cajole sl
7549	50	6783	993.62	latelets cajole against the blithely final platelets. express dependencies doubt furiously. slyly regular ideas sublate slyly fl
10001	2	3185	12.18	long the furiously busy deposits. regular, express courts according to the regular deposits nag against the quickly even deposit
10002	3	6728	529.58	yly carefully bold requests. furiously express requests need to kindle furiously among the fluffily regular foxes. even packages use slyly blithe, regula
10004	5	608	911.26	ironic accounts. frets maintain pending deposits; pinto beans use carefully re
10008	9	9362	644.54	ounts haggle blithely against the regular excuses. slyly final courts around the furiously ironic requests cajole above the regular pinto beans! fluffi
10011	12	4207	289.88	c instructions are furiously after the regular, ironic foxes. unusual, ironic deposits wake c
10012	13	8262	754.68	s around the regular deposits are carefully blithely final requests. packages sleep. 
10014	15	4692	327.63	lyly unusual foxes nag carefully about the thin deposits. carefully unusual deposits cajole according to the furiously even deposits. carefully final packages nag carefully. final, ironi
10019	20	4823	908.09	l ideas sleep among the blithely even instructions. quick packages promise. quickly even foxes sleep across the bold, even theodolites. accounts
10020	21	9764	40.34	ly regular foxes. final, regular platelets detect. furiously even theo
10021	22	9581	521.42	gainst the special, final requests. ironic, pending theodolites sleep quickly at the even dep
10022	23	2929	697.42	bout the final, pending courts. bold deposits should boost slowly. enticingly special packages haggle blithely according 
10023	24	4650	2.18	ding excuses after the furiously even pearls affix carefully excuses. regular deposits sleep quickly regular request
10027	28	7320	599.18	iously slyly regular foxes. slyly even deposits kindle? thin packages should have to detect quickly blithely even dependencies. express excuses use alongside of th
10028	29	8237	58.63	ironic deposits. carefully regular accounts grow carefully around th
10032	33	1082	939.75	e quickly. regular requests sleep fluffily. Tiresias across the enticing, final ideas wake carefully along the fluffily even packages. fluffily bold ideas use
10033	34	5140	45.15	ide of the slyly regular deposits. carefully regular account
10036	37	3010	42.87	nly. quickly final packages sleep blithely across the quickly ironic pinto beans. slyly regular requests are blithely silent somas. fur
10037	38	232	632.23	 in place of the quickly dogged ideas must wake even theodolites; carefully silent packages are slyly reg
10038	39	7389	487.25	integrate quickly final accounts-- carefully unusual requests above the slyly ironic id
10042	43	7054	570.93	pendencies wake furiously stealthily final realms. regular theodolites boost. furiously final foxes cajole along the blithely final reque
10043	44	9202	651.01	f the theodolites use blithely across the furiously even Tiresias. carefully even deposits boost carefully enticing accounts. slyly fina
10044	45	4412	457.27	ven, regular instructions use: theodolites cajole blithely above the realms. carefully express dolphins are quickly. special deposit
10049	50	9017	197.83	sual packages. ironic epitaphs boost slyly across the always special accounts. pending depen
12498	2	4574	483.44	counts according to the ideas boost furiously at the quickly express excuses. car
12499	3	8646	722.34	o beans promise furiously against the even accounts. regular packages across the quickly express dependenci
12501	5	1463	223.58	 carefully along the blithely even deposits. furiously bold deposits alongside of the quickly special foxes sleep closely bold platelets. furiously ironic packages behind the slyly
12505	9	6031	208.14	luffily special accounts cajole closely. blithely bold requests engage carefully after the packages. quickly stealthy packages cajole. flu
12508	12	3432	59.36	ial accounts. fluffily pending requests haggle furiously. foxes doze about the foxes. quickly special dependencies are. furiously final packages sleep against the silently p
12509	13	2080	593.75	tructions doubt slyly quiet dependencies. slyly unusual packages about 
12511	15	6817	111.89	refully regular multipliers use carefully. pinto beans above the dinos nag slyly requests. bold gifts are ste
12516	20	263	873.89	ar excuses sleep doggedly. furiously silent ideas grow slyly ironic accounts. unusual, final instructions cajole. slyly
12517	21	1511	295.46	n, unusual excuses snooze carefully ideas! quickly final requests impress blithely across the blithely special request
12518	22	9731	780.49	s. slyly ironic requests after the ironic requests affix s
12519	23	6858	227.49	se slyly alongside of the final, regular deposits. regular deposits ab
12520	24	3199	510.85	even grouches wake blithely. bravely express requests boost quickly. quietly special asym
12524	28	4629	964.63	e fluffily among the quickly final dependencies. carefully final theodolites impress furiously across the carefully even requests. final deposits was fluffily. regularly pending foxes
12525	29	1906	487.39	fully special deposits. foxes use slyly ironic accounts. fluffily regular accou
12529	33	9420	783.38	le blithely. blithely even deposits across the alw
12530	34	5926	692.92	ly final warthogs haggle furiously slow hockey players. carefully ironic pearls are. furiously silent packages haggle carefully. carefully bold theodolite
12533	37	6065	545.25	cross the ironic packages play above the blithely pending package
12534	38	6721	593.24	furiously. dugouts cajole carefully after the final excuses-- unusual packages cajole silently according to the carefully regular excuses. packages beside the ironic platelets nag
12535	39	2715	556.58	kages. ironic, special warthogs are against the quickly even deposits. quickly even requests thrash furiously around the ironic
12539	43	1385	438.46	furiously final packages. packages solve quickly. ironic, regular deposits nag bold, regular instructions. carefully ironic packages use. unusual accounts eat excuses. ironic accounts sleep b
12540	44	7724	530.49	uctions wake slyly furiously final deposits. carefully ironic accounts use furiously after the always express excuses. furiously even requests alongside of the final, regular id
12541	45	4922	503.30	y silent theodolites could nag quickly after the special packages. blithely regular packages haggle packages-- quickly iro
12546	50	9902	123.32	usly busy requests sublate about the pending asymptotes. carefully even accounts among the final packages 
14999	2	4034	151.75	ffix slyly along the regular asymptotes. slyly final platelets boost carefully above the carefully regular deposits. ironic, fluffy packages use silent platelets? quickly pending accounts cajol
15000	3	9975	841.19	ealms. carefully special packages are furiously across the even, bold pinto beans. slyly even deposits among the blithely unusual packages are quickly ironic dinos. carefully even pa
15002	5	9856	7.20	packages detect evenly above the never ironic instructions. carefully final packages boost blithely. final, pending dependencies wake carefully about the furiously even warhorses. qu
15006	9	4842	199.28	l ideas! ironic requests cajole blithely special, even r
15009	12	5443	970.82	 packages. unusual accounts according to the stealthily bold request
15010	13	7528	80.70	nts haggle furiously around the ironic accounts. sometimes regular instructions affix fluffily theodol
15012	15	4814	849.43	s haggle furiously furiously pending instructions. packages integrate regular asymptotes. requests sleep carefully along the instructions. furiously 
15017	20	950	675.82	above the express dolphins. fluffily regular plat
15018	21	7305	381.50	blithely even deposits. final dependencies cajole above the bold deposits. blithely final deposits according to the quickly ironic ideas cajole against the 
15019	22	6847	376.11	. carefully regular instructions sleep. even, final excuses haggle regular, even theodolites. carefully special instructions 
15020	23	5680	185.30	kages! quickly regular deposits should haggle slyly about the busily special dolphins. requests
15021	24	257	523.60	s foxes through the silent excuses wake quickly carefully ironic package
15025	28	2059	787.48	regular excuses. carefully express theodolites are against the requests. carefully unusual instructions do cajole carefully furiously bold dependencies. blithely
15026	29	7363	583.37	ajole: ironic escapades boost blithely excuses. unusual requests hinder quickly 
15030	33	5171	831.96	 daringly about the theodolites. unusual dependencies sleep blithely. slyly ironic pinto beans solve blithely silent accounts. regular, blithe packages sleep quickly. carefully final
15031	34	8699	988.98	ccounts? furiously silent dolphins sleep final attainments. even theodolites eat slyly about the regular deposi
15034	37	5834	908.51	s wake quickly blithely ironic excuses. always final dependencies might mold blithely. bold courts cajole blithely. blithely even deposits cajole quickly across the blithely silent packag
15035	38	6449	171.66	sits wake slyly quickly express requests. regular, express requests doubt-- fur
15036	39	7945	852.58	 instructions. even, pending theodolites wake carefully among the qu
15040	43	770	435.83	ndle above the carefully regular pinto beans. fluffily express requests are slyly slyly express packages. even, regular deposits are fluffily slyly even ideas. slyly regular foxes wake 
15041	44	4913	367.29	eans wake fluffily requests. slyly stealthy ideas after the slyly pending requests nag blithely about the slyly regular requests. express platelets play requests. furiously unusual packages wa
15042	45	2028	254.77	re blithely. slyly idle decoys alongside of the final accounts wake quickly furiously final requests. carefully bold epitaphs integ
15047	50	8733	45.13	 slyly regular packages are slyly after the pending instruction
17500	2	4989	422.46	e furiously furiously pending instructions. special asymptote
17501	3	5401	139.06	ts. carefully even packages use fluffily across the furiously unusual accounts. final, quick requests boost. 
17503	5	6867	923.52	 final packages. even dependencies nag furiously express pinto beans. final deposits sleep: carefully express foxes use regula
17507	9	4953	538.55	jole furiously blithely bold accounts. special requests use evenly regular, final packages. blithely regu
17510	12	3651	73.52	 daringly along the slyly regular accounts. furiously ironic excuses nag quickly along the unu
17511	13	7559	884.23	ing, unusual requests. furiously express instructions engage. express requests sleep blithely about the 
17513	15	8312	861.26	uriously against the quickly regular requests. accounts across the requests sleep c
17518	20	8628	492.05	uests sleep unusual, regular requests. quickly ironic dolphins wake carefully slyly ironic requests. pending accounts use slyly express deposits. carefully regular 
17519	21	3436	229.16	s sleep evenly. slyly final dependencies until the carefully unusual dolphins nag quickly alongside of the fluffily pending theodolites. slyly final d
17520	22	4887	732.43	f the quickly unusual deposits wake furiously around the furiously final pinto bea
17521	23	317	953.74	refully final dependencies. packages sleep carefully blithely even dependencies; carefully pending asymptotes kindle
17522	24	6868	951.91	pending instructions after the regular requests cajole quietly after the fluffily final requests. theodolites wake? furiously ironic requests sleep f
17526	28	9202	465.39	 unusual packages detect slyly ironic, express theodolites. special packages sleep carefully. slyly un
17527	29	8551	629.54	ag quickly near the blithely silent asymptotes. foxes cajole doggedly carefully special instructions.
17531	33	4244	576.50	s. fluffily quick forges haggle about the regular, regular courts. ironic, express deposits play. blithely regular theodolites integrate furiously. unusual, pending packages use f
17532	34	2558	840.49	ticing excuses sleep furiously. regular, special deposits haggle after the fox
17535	37	1151	68.94	 special packages haggle. carefully ironic platelets among the slyly unusual requests hag
17536	38	3648	806.55	egular, regular requests doze furiously boldly bold deposits. slyly final packages cajole 
17537	39	2693	555.31	e bold, unusual packages. silent, final tithes against th
17541	43	5084	505.57	cajole. busily ironic packages can use fluffily against the even sentiments. blithely ironic accounts wake blithely. fluffily bold packages eat slyly alongside of the final deposits. blithely ironi
17542	44	1430	210.46	slyly special deposits. slyly sly requests boost blithely alongside of the slyly final deposits. fluffily regular asymptotes a
17543	45	5047	935.62	hang fluffily furiously even requests. slyly unusual excuses use fluffily pending requests. furiously final accounts hang carefully against the slyly special 
17548	50	7851	812.44	. blithely unusual packages sleep carefully over the regular ideas. express packages with the ironic realms integrate furiously
20001	2	9562	151.07	ons sleep quickly across the slyly regular Tiresias: unusual requests integrate carefully. ideas use bl
20002	3	6858	786.94	 detect. pending, even packages boost against the requests! fluffily special requests dazzle quickly regular instructions. blithely express s
20004	5	5961	757.11	es. quickly ironic foxes wake fluffily even instructions. finally special packages cajole fluffily even deposits. final accounts wake. fluffily even pac
20008	9	9219	214.55	hall have to print blithely ironic accounts. furiously unusual deposits are. regular, pending excuses serve carefully. quickly regular
20011	12	4557	547.82	into beans are furiously bold dependencies; even foxes against the theodolites nag slyly furiously quiet asy
20012	13	9182	721.62	fully final excuses; blithely regular ideas haggle among the carefully special deposits. quickly unusual Tiresias serve. special foxes cajole regularly dogged, silent accounts. silent, express p
20014	15	7830	210.53	ts haggle slyly deposits. furiously regular accounts use furiously. quickly unusual requests since the never bold theodolites wake around the fluffily express multipliers; carefully even th
20019	20	5730	299.76	y regular dependencies. decoys wake quickly quickly ironic accounts. slyly express accounts nag quickly. slyly regular f
20020	21	1304	336.88	fluffily ironic excuses detect. unusual, final packages along the carefully regular courts wake regular packages. fluffily special packages nag quickly deposits. final deposits d
20021	22	7009	820.04	ly final deposits. carefully ironic deposits around the carefully bold accounts affix slyly instead of the fluffily regular instructions. requests boost quickly regular depende
20022	23	8532	343.02	lithely regular courts cajole quickly after the final, final packages. quickly express theodolites against the slyly ironic pint
20023	24	6739	728.69	ular packages haggle final ideas. carefully unusual requests sleep. bl
20027	28	2616	483.28	special deposits detect furiously! special accounts engage blithely fluffily final foxes. furiously regular accounts sleep after the busy dependencies. pending accounts about th
20028	29	6593	416.72	 pending, express foxes. carefully regular instructions cajole about the carefully bold pinto beans. accounts sleep blithely regular pinto beans. blithely idle ideas affix carefully car
20032	33	4337	359.49	 regular requests use along the unusual foxes. ironic accounts haggle caref
20033	34	89	585.34	vely pending accounts cajole furiously. even pinto 
20036	37	417	466.88	eep. busy requests sleep furiously pending packages. final patterns ar
20037	38	8666	908.73	 the slyly even accounts. deposits are. even foxes detect carefully careful theodolites. deposits doubt furiously. regular sheaves lose bold accounts
20038	39	5323	572.20	s. special, final ideas poach fluffily-- blithely ironic packages haggle permanently furiously silent deposits. blithely ironic foxes lose final packages. quickly express 
20042	43	6426	673.75	ites. furiously final theodolites are carefully. furiously pending d
20043	44	6254	404.23	 even braids wake along the furious requests-- furiously unusual packages boost against the pinto beans. r
20044	45	1743	790.33	equests. blithely bold packages sleep even instructions. carefully ironic requests thrash carefully. bold accounts integrate. always regular deposits haggle do
20049	50	1876	83.15	 slyly in place of the furiously ironic requests. bold, even requests across the carefully ironic theodolites cajole carefully fluffily ironic foxes. theodolites use.
22495	2	9398	646.87	ully pending deposits. slyly even pinto beans according to the blithely regular theodolites are fluffily among the regular foxes. quickly regular depos
22496	3	8268	444.21	ully even ideas are furiously. furiously unusual instructions s
22498	5	4365	453.83	theodolites would sleep special forges. blithely express packages wake fluffily. deposits print furiously. final deposits sleep above the ironic, iro
22502	9	4772	567.57	 are slyly unusual deposits. blithely unusual deposits run instructions. furiousl
22505	12	8726	361.08	eep silently along the slyly pending theodolites. ironic deposits about the ironic, silent gro
22506	13	7520	76.83	oxes. regular, pending accounts haggle to the blithely express accounts. slyly special foxes impress carefully furiously special accounts. furious
22508	15	9408	667.33	into beans among the final packages integrate carefully about the blithely steal
22513	20	6804	808.20	y special, special requests! furiously unusual instructions boost along the fluffily bold packages. blithel
22514	21	9569	536.48	c dolphins. ironic, regular foxes are final dependencies. quickly regular requests wake carefully. furiously unusual instr
22515	22	3598	240.85	 fluffily ironic packages among the ironic ideas print blithely after the excuses. bold, ironic asymptotes wake across the regular, unusual deposits. final, busy requests wake across the blith
22516	23	7389	730.17	s. blithely regular requests cajole quickly throughout the carefully pending foxes: blithely ironic deposits after the pending ideas believe furiously according to
22517	24	5766	700.51	iously unusual requests cajole. regular accounts are slyly thin foxes-- foxes above the quickly final packages detect slyly carefully fi
22521	28	9942	326.55	e quickly across the unusual pinto beans. quickly final theodolites nag fluffily final requests. carefully sly deposits nag furiously furiously pending requests. even p
22522	29	9617	292.25	ronic packages wake furiously along the ironic, unusual packages. bold ideas are slyly final requests: close, regular asymptotes use slyly amon
22526	33	4854	625.88	es nod fluffily blithely final grouches. courts against the quickly final foxes sleep blithely furiously regular requests. ironic requests haggle about the fur
22527	34	599	947.29	ely regular requests. blithely bold packages nag alongside of the slyly bold instructions. idly express accounts use slyly. blithely bold foxes are blithely final foxes. carefully pend
22530	37	1324	175.15	tructions do play blithely against the blithely pending accounts. enticingly regular pinto beans could haggle carefully. quickly regular account
22531	38	6897	230.55	ackages haggle slyly bold, regular requests. slyly special packages detect blithely accord
22532	39	5132	42.27	ly bold accounts. express instructions sleep slyly furiously speci
22536	43	9283	690.13	ze against the pending packages: slyly regular courts wake furiously final accounts. quick
22537	44	9057	316.84	enly regular theodolites-- fluffily express deposits use pending, pending asymptotes-- furiously pending instructions wake furiously-- ste
22538	45	8567	521.66	lar theodolites against the regular waters cajole in place of the fluffily even pinto beans. quickly regular deposits affix quickly against the requests. carefully unusual pinto beans are ir
24997	2	7394	990.11	e carefully slyly special instructions. regular deposits sleep blithely across the quickly regular accounts. regular grouches above the final ideas wake alongside of the ruthl
24998	3	4887	626.55	quickly even accounts cajole slyly to the even di
25000	5	1067	312.63	egular platelets unwind blithely. furiously slow pinto beans wake furiously pending ideas. even depths above the express gifts wake up the deposits. furiously thin realms haggle quiet acco
25004	9	4009	198.72	slyly unusual asymptotes. pending foxes across the accounts sleep furiously final accounts. fluffily ironic packages sleep quickly. blithe
25007	12	9754	198.50	- instructions boost furiously according to the furiously bold escapades. permanently pending pack
25008	13	5247	362.28	ccording to the quickly pending excuses cajole furiously above the carefully final notornis. ironic instructions haggle-- spec
25010	15	186	430.69	ly final pinto beans sleep quickly according to the ironic ideas. silent, regular deposits use; final, express requests hag
25015	20	2816	362.11	 beans. dependencies haggle fluffily along the slyly regular accounts. final platelets need to use blithely special requests. carefully regular pearls wake. carefully regular instruc
25016	21	4181	13.89	lyly silent theodolites use furiously after the blithely final theodolites. evenly ironic pinto beans use furiously after the even pinto beans. furiously ironic requests
25017	22	6268	375.15	ourts nag alongside of the even requests; quickly pending deposits cajole carefully among the carefully ironic deposits. final, unusual deposits cajole final excuse
25018	23	5161	488.05	ly accounts are carefully. final, even accounts sleep after the special account
25019	24	5722	770.07	above the furiously final requests cajole against the regular packages; quickly regular accounts integrat
25023	28	176	211.93	silent ideas integrate slyly regular sheaves. pendin
25024	29	4069	779.48	t the final ideas. pending requests integrate. regular platelets boost enticingly. carefully regular requests above the accounts n
25028	33	2896	437.69	e furiously regular, silent deposits. carefully even realms nag qui
25029	34	6476	500.47	s. carefully ironic deposits are furiously ironic requests. slyly even theodolites sleep carefully across the ironic, unusual ideas. furiously daring dolphins affix blith
25032	37	7407	623.30	. regular deposits cajole quickly against the slyly final ideas; express accounts wake after the boldly final asymptotes. ironic accounts wake unusual theodolites. blithe platelets engage quic
25033	38	9315	718.13	across the instructions. express asymptotes cajole slyly. bold instructions are across the fluffily special hock
25034	39	8616	310.11	iously. unusual packages cajole furiously carefully final packages. qui
25038	43	4464	286.20	ely ruthless requests affix fluffily about the slyly even requests. special foxes cajole furiously along the pending ideas. carefully s
25039	44	2708	909.18	y across the carefully ruthless ideas! quickly special asymptotes integrate furiously ironic ideas; final foxes haggle furiously regular instructions. packages mold caref
25040	45	6329	825.27	 the express packages. express ideas impress. regular asymptotes play slyly along the slyly daring theodolites. quickly even foxes affix bo
25045	50	2198	593.57	 against the dependencies must sleep carefully ironic ideas. stealthily final theodolites about the fluffily ironic courts wake alongside of the blithely furious waters. pending req
27499	2	8267	323.91	 beans sleep along the ruthlessly ironic requests; instruct
27500	3	2703	235.56	sts mold furiously final deposits. quickly even requests ought to sleep. regular, regula
27502	5	7061	539.66	ously even attainments across the pending, final platelets cajole quickly regular instructions. fluffily final requests wake doggedly. unusual pains among the quickly expr
27506	9	7775	226.90	 blithely regular pinto beans. ironic, pending accounts haggle regular theodolites. final requests wake against the do
27509	12	4119	5.92	ing pinto beans poach! carefully ruthless foxes haggle quickly carefully final ideas. fluffily ironic foxes affix among the bold foxes. carefully silent accounts use. slowly even platelets nag sly
27510	13	3475	742.02	cuses. slyly final requests are. requests wake carefully across the furiously even accounts. dogged, regular theod
27512	15	3207	993.15	 requests. ironic, pending requests lose slyly carefu
27517	20	9627	748.05	ow final ideas. platelets are. regular accounts sleep furiously. carefully regular asymptotes affix thinly alongs
27518	21	7235	242.58	bold packages dazzle thinly! theodolites nag fluffily. carefully pending packages nag slyly pending, bold deposits. blithely bold tithes around th
27519	22	9896	750.08	s! quickly express requests cajole blithely furiously ironic deposits. f
27520	23	497	868.50	 the quickly regular requests. express ideas are carefully final packages? packages are
27521	24	636	808.81	 final requests. bold accounts alongside of the quickly even deposits boost carefully above the final courts; ironic asymptotes detect alongside of the fluffily bold accounts.
27525	28	9145	279.53	cuses are according to the ironic foxes. deposits alongside of the slyly express theodolites wake carefully along the slyly pending deposits. requests along the silently unusual asymptote
27526	29	1268	657.49	ymptotes: pending pinto beans breach enticingly after the blithely final accounts. furiously bold requests wake-- unusual pinto beans 
27530	33	6275	625.20	accounts along the ironic requests are carefully about t
27531	34	9136	960.08	e daringly ironic theodolites. evenly ironic instructions wake quickly. furiously even account
27534	37	2181	648.93	old ideas haggle across the final requests. ironic, special packages kindle never according to the express, silent accounts. regular instructions could breach carefully furiously regular fox
27535	38	9257	241.46	ake slyly along the furiously express theodolites! quickly special foxes boost slyly. accounts use. carefully even excuses 
27536	39	75	44.43	gular accounts haggle instructions. slyly special sauternes use carefully i
27540	43	3659	298.15	 snooze quickly after the carefully final theodolites. slyly regular packages haggle about the quickly regular requests. slyly bold pinto beans across the even
27541	44	2998	911.08	. bold, express packages haggle busily. slyly ironic deposits sleep fluffil
27542	45	9993	979.91	structions are. slyly regular ideas boost. slowly dogged accounts among the carefully regular instructions cajole slyly according to th
47538	43	1562	972.55	close requests cajole slyly always regular requests: slyly specia
27547	50	8647	34.04	y special packages. fluffily final foxes use carefully ironic foxes. carefully regular waters haggle deposits. regular accounts cajole above the fluffily final requests. bold, pending reque
30001	2	6906	776.88	y against the unusual, final sentiments. pearls are. pinto b
30002	3	2988	446.70	ages across the slyly regular packages are carefully regular packages. final asymptotes unwind slyly. express deposits haggle carefully carefully bol
30004	5	6671	617.74	eep blithely alongside of the carefully express theodolites. carefully express theodolites wake blithely final theodolites. slyly bold theodolites grow carefully a
30008	9	3163	748.16	er the final asymptotes dazzle carefully express theodolites. ironic, bold reques
30011	12	6110	244.07	press packages cajole. ironic packages wake. slyly final deposits are above the careful requests. special, silent deposits after the slyly regular pinto beans cajole against the quickly express e
30012	13	7272	581.10	ly alongside of the carefully pending foxes. carefully ironic orbits wake
30014	15	2186	34.30	y pending requests across the regular, final requests are after the blithely unusual requests. regular accounts cajole carefully; ironic, bold 
30019	20	5242	703.74	 accounts. slowly final requests integrate never above the pending requests. unusual, pending platelets eat slyly carefully 
30020	21	463	586.73	cies use even somas. unusual requests sleep slyly according to the quickly even packages! packages sleep slyly silent, regular deposits. furiously u
30021	22	2569	271.61	as. bold, express patterns use quickly evenly regular deposi
30022	23	5096	149.50	r the furiously silent deposits boost across the furiously bold accounts. regular, pending requests around the slyly special theodolites are 
30023	24	2206	353.16	lay alongside of the final foxes-- slyly pending ideas breach special requests. quickly fluffy deposits integrate blithely. regularly regular ideas could cajole among t
30027	28	2516	801.66	y final theodolites sleep fluffily about the unusual, thin platelets. blithely ironic ideas alongside of the packages poach carefully carefully even theodolites. ideas wake carefu
30028	29	2977	824.43	riously. silent, final accounts along the final, special requests nag across the permanently regular accounts. pl
30032	33	2510	591.80	 alongside of the blithely final theodolites use special reques
30033	34	629	907.34	forges. regular packages haggle-- quickly final requests boost about the pending dependencies. slyly final accounts cajole fluffily. carefully bold platelets among th
30036	37	7224	707.40	c theodolites. bold excuses along the furiously regular pinto bea
30037	38	884	195.90	ending asymptotes. regular, final accounts cajole. blithely ironic packages abo
30038	39	2653	721.00	regular accounts hinder into the carefully pending forges. fluffily pending accounts wake across the furiously regular reques
30042	43	2070	382.56	ges cajole even theodolites. furiously fluffy packages integrate quickly stealthily ironic instructions. requests poach quickly at the blithely even deposits
30043	44	7110	538.67	lent waters cajole fluffy dolphins. carefully pending gifts wake blithely about the slyly final packages. special, final pinto beans about the packages cajo
30044	45	8662	169.39	ing to the regular foxes. furiously ironic pinto beans sleep ironic packages. special, final ideas wake furiously fluffily final foxes. pending requests poach blithe
30049	50	8200	630.45	ect across the quickly silent foxes. furiously final foxes alongside of the carefully bold deposits integrate quickly about the final, regular theodolites. quickly final the
32492	2	9005	295.84	 special escapades-- pending, even asymptotes boost about the fluffily regular packages. regular, ironic requests detect. quickly pending deposits cajole blithely above th
32493	3	7647	813.17	g the furiously unusual pains kindle ironic, pending requests. express, pending theodolites boost. ironic, expres
32495	5	6349	996.43	e blithely special ideas. express packages cajole blithely. 
32499	9	2300	53.00	its sleep carefully. furiously careful courts cajole furiously d
32502	12	244	100.91	ly unusual deposits? fluffily final theodolites eat. ironic accounts use carefully alongside of the carefully final pinto beans. blithely regular packages wake blithely. fluffily bold p
32503	13	5142	349.81	eep blithely regular packages. pending packages to the carefully ironic foxes sleep regular, unusual
32505	15	3235	604.24	ithely across the ideas. even accounts sleep furiously fluffily regular packages. regular, special accounts across 
32510	20	8542	810.64	r asymptotes sleep carefully. blithely pending instructions haggle furiously. care
32511	21	6277	719.00	ven dependencies affix past the quickly even pinto beans. regular deposits across the carefully sly requests kindle evenly after the carefully regular accounts. furiously regular sentiments among t
32512	22	9651	37.89	egular foxes wake. carefully pending platelets use quickly pending dependencies. permanent dependencies nag unusual packages. excuses cajo
32513	23	312	824.42	ts. carefully bold requests sleep fluffily. carefully enticing dependencies across the slyly final foxes are quickly slyly ironic foxes. even packages are furiously thin ideas. unusual acco
32514	24	1683	937.94	ngside of the carefully ironic patterns. silent hockey players thrash slyly acco
32518	28	8403	817.88	slyly along the accounts! blithely regular accounts hinder quickly along the slyly even ideas. ca
32519	29	8559	676.87	 quickly bold packages. pinto beans are packages. busily pending dugouts affix slyly regular packages. blithely regular deposits affix fluffily close requests. carefully even req
32523	33	6852	976.90	silent instructions. bold ideas grow quickly regular platelets. final inst
32524	34	4500	86.75	s doubt carefully according to the furiously final foxes. fluffily final packages print slyly final deposits. furiously even foxes boost blithely ironic, regular reques
32527	37	7193	7.43	ly. packages snooze furiously furiously ironic pearls. carefully ironic accounts sleep! regular requests cajole qui
32528	38	7671	770.74	bout the blithely regular accounts use against the slyly pending theodolites. express dependencies boost blithely. express, regular foxes wake carefully. unusual
32529	39	3862	524.90	y unusual foxes boost furiously along the slow, bold deposits. asymptotes sleep. blithely ironic deposits grow furiously. foxes about the furiously regula
32533	43	2020	756.91	 sleep blithely carefully regular foxes: ironic accounts eat slyly. slyly ironic ideas according to the slyly even frays sleep across the quickly final requests
32534	44	7698	33.69	ites? carefully even requests use. regular gifts along the ironic packages cajole fur
32535	45	5588	629.30	 accounts sleep carefully after the quickly ironic requests-- enticing dugouts haggle furiously regular deposits. pending, fi
32540	50	9374	382.45	quests between the final deposits cajole furiously even packages. fluffily special asymptotes about the fluffily express platelets wake carefully around the regula
35032	39	2043	86.58	 express sentiments haggle quickly. blithely regular accounts cajole slyly regular 
34995	2	3109	866.57	ly ironic courts was enticingly slyly bold instructions. carefully bold excuses cajole at the ironic, final requests. quickly final excuses above the regular deposits are furiously 
34996	3	5305	399.09	gular, bold excuses. quickly pending instructions nag express deposits. final accounts wake. busy, ironic packages affix quickly along
34998	5	6342	431.96	 ironic packages use blithely. unusual packages according to the boldly ironic pinto beans nag blithely instead of the furiously regular packages. ironi
35002	9	9486	493.40	sleep furiously requests. slow requests wake furiously carefully regular requests. fluffil
35005	12	9973	502.17	. even deposits integrate. special pinto beans haggle furiously special pains. fluffily ironic 
35006	13	2974	799.80	 platelets. ideas alongside of the fluffily regular sheaves wake fluffily carefully regular foxes. dogged, bold pearls haggle. carefully even deposits haggle. regular, ironic foxes
35008	15	3358	394.55	l orbits. blithely express deposits among the furiously express packages maintain blithely regular, ironic deposits. daringly perma
35013	20	8900	821.87	he final pinto beans use slyly across the foxes. furiously regular patterns affix unusual deposits. furiously ironic theodolites are depos
35014	21	9523	836.59	nd the boldly special platelets haggle pending courts
35015	22	3638	817.66	ily unusual accounts haggle near the quickly ironic dolphins. quickly special depo
35016	23	635	737.94	e to hang slyly bold requests: fluffily unusual deposits nag carefully. ideas wake blithely final packages. deposits unwind carefull
35017	24	8581	458.41	 accounts breach blithely among the foxes-- carefully ironic theodolites haggle above the unusual requests. furiously final depths are carefully pending instr
35021	28	2627	344.97	bold foxes wake fluffily among the silent pinto beans. special ideas use carefully. theodolites haggle. quickly fin
35022	29	557	432.34	lites boost. packages alongside of the slyly bold instruct
35026	33	4610	191.03	as cajole permanently. quickly final ideas use alongside of the blithely final excuses. furiously unusual excuses wake carefully about the finally regular instructio
35027	34	8446	514.09	 pinto beans along the slyly quiet foxes believe carefully packages. final multipliers detect slyly. carefully regular orbits boost slyly according to the ideas. regular, permanent depos
35030	37	5930	467.01	ages cajole blithely. excuses haggle quickly unusual packages. slyly regular packages haggle carefully: instructions nag car
35031	38	3399	318.59	hely even requests. final, ironic deposits haggle slyly requests. regular asy
35036	43	4505	61.38	ording to the even theodolites. carefully unusual foxes around the slyly ironic courts sleep instruction
35037	44	6550	60.50	ggle: carefully regular packages kindle furiously. blithely regular ideas nag slyly. excuses use blithely? slyly regular requests haggle silent, silent decoys. furiously unusual acc
35038	45	8110	884.55	ets. theodolites along the ironic attainments boost permanently regular instructions. furiously special requests haggle according to the carefully unusual request
35043	50	4358	955.45	the pending, regular dependencies. fluffily ironic deposits are blithely along the deposits-- carefully regular accounts a
37498	2	8866	176.81	ts according to the dolphins wake among the carefully expres
37499	3	992	191.14	 foxes are express, regular packages. regular accounts according to the furiously even packages haggle packages: unusual, ironic theodolites play carefully. fluffily regular packag
37501	5	5029	457.87	ccounts. final, ironic accounts cajole enticingly after the blithely final foxes. pending accounts at the pending, silent asymptotes unwind sly
37505	9	4146	490.69	t carefully slyly regular pains. slyly special theodolites about the blit
37508	12	903	993.36	nts dazzle slyly according to the asymptotes. carefully final packages integrate furiously ironic ideas-- final requests boost furiously according to the blithely regular packages. regular, regular 
37509	13	9371	626.29	ithely express deposits are slyly. blithe accounts cajole slyly unusual, regular packages? express deposits detect bravely. instructions wake c
37511	15	962	872.39	 furiously bold dependencies are about the carefully ironic theodolites. regular, special accounts hinder fluffily ironic instructions. express, express excuses use bo
37516	20	3380	614.10	s play. pending packages haggle furiously across the special, regular theodoli
37517	21	7652	237.82	 are carefully according to the quickly even dependencies. blithely quick packages cajole. ruthlessly final accounts boost. slyly bold requests haggle regularly careful
37518	22	5704	229.70	y final packages. blithely final pinto beans use carefully even deposits. furiously daring accounts boost carefully boldly reg
37519	23	9804	890.70	ay quietly fluffily final deposits; slyly ironic packages sleep blithely abou
37520	24	8394	542.07	 dependencies. ironic, unusual packages would hang blithely: furiously final accounts hang quickly according to the frays. final dolphins affix 
37524	28	7703	869.38	carefully. special, silent instructions are slyly. silently even deposits are carefully alongside of the carefully special ideas
37525	29	6148	454.12	d courts haggle quickly. stealthily bold asymptotes haggle among the furiously even realms? furiously unusual packages engage slyly. furious packages cajole blithely pending, ironic packages. quick
37529	33	9648	805.44	 packages. pending, unusual packages doubt bold theodolites. boldly bold foxes since the packages cajole alongside of t
37530	34	922	324.83	sly instructions. final theodolites sleep carefully blithely regular deposits. quickly regular accounts boost carefully special theodolites. ruthlessly regular th
37533	37	4103	873.44	deposits cajole furiously. slyly final excuses cajole. slyly special instructions boost blithely. regular requests about the furiously pending packages wake about the blithely regular requ
37534	38	3150	534.80	s against the furiously even accounts serve carefully about the carefully bold accounts. quickly regular excuses are slyly at the furiously iron
37535	39	1285	902.71	 quickly final deposits sleep. slyly special dolphins about the quickly regular dinos are slyly regular dependencies. reg
37539	43	1648	365.19	sly final asymptotes. ironic, unusual packages play against the carefully b
37540	44	82	168.17	ons nag furiously regular deposits! even, daring theodolites wake furiously thi
37541	45	287	680.64	. slyly ironic pinto beans mold slyly regular packages. blithely brave accounts among the requests wake slyly alongsid
37546	50	3183	398.96	uctions cajole quickly. unusual requests haggle carefully around the even packages. regular, regular requests boost
40001	2	9531	683.63	structions. bold packages use furiously against the carefully silent braids. foxes along the unusual, close accoun
40002	3	953	17.33	posits cajole fluffily. packages serve. fluffily ironic frays haggle. quickly express ideas alongside of the final deposits solve asymptotes. ironic attainment
40004	5	7207	49.41	print dolphins. even, regular excuses after the furiously thin theodolites boost carefu
40008	9	2789	417.68	es across the blithely special platelets eat carefully quickly s
40011	12	858	828.40	g furiously quickly even deposits. ironic ideas abo
40012	13	9815	878.98	of the carefully regular accounts. idly regular accounts are express packages! slyly permane
40014	15	818	908.82	counts. even ideas are carefully. ironic accounts according to the carefully final requests nag outside the blithely regular instructions. regular theodolite
40019	20	8815	445.16	hs. regular accounts until the carefully regular requ
40020	21	4163	228.74	ges are across the carefully ruthless sheaves. final packages try to are 
40021	22	8973	267.96	ess requests against the daring deposits haggle blithely around the slyly express deposits. even asympt
40022	23	5926	987.74	oost slyly after the deposits. silent, special accounts sha
40023	24	7550	164.13	carefully. furiously dogged dolphins breach blithely. final, even deposits according to the carefully silent deposits shall have to haggle carefully above t
40027	28	3223	227.38	usual deposits. blithely pending platelets x-ray. blithely express deposits alongside of the packages haggle about the furiously reg
40028	29	1924	173.62	y. final deposits nag final packages. carefully final pinto beans use slyly about the ideas. speci
40032	33	6399	897.29	ilent asymptotes. regular deposits sleep always. fluffily special requests use special ideas. ironic pinto beans nag quickly against the ruthlessly unusu
40033	34	2515	180.92	 furiously from the unusual requests. fluffy asymptotes boost: stealthily even theodolites wake carefully. quickly pending deposits haggle furiously
40036	37	7793	325.71	ts sleep slyly blithely bold deposits. even, regular packages are slyly after the special requests. special ideas nag furiously silent dependencies? requests engage. bold pinto beans sleep accordin
40037	38	1009	25.60	the carefully furious requests cajole above the deposits. blithely regular
40038	39	6530	664.01	tions. even deposits sleep slyly alongside of the blithe, ironic requests. iron
40042	43	8358	130.20	 even requests wake furiously pending, express sauternes. furiously unusual excuses cajole quickly. blithely unusual courts cajole fu
40043	44	8983	820.60	carefully regular ideas against the blithely even ideas sleep after the pending instructions. speci
40044	45	9709	989.89	 accounts sleep furiously. furiously ironic packages across th
40049	50	6256	589.21	ding ideas. busily final accounts cajole. fluffily reg
42489	2	6125	952.71	 slyly. carefully close platelets detect. fluffily permanent deposits across the carefully ironic instructions are furiously final requests. quickly express packages detect carefully thinly
42490	3	2499	221.56	nag fluffily against the ironic deposits. fluffily unusual packages nag unusual pinto beans. fluffily regular Tiresias use. slyl
42492	5	2290	606.09	g dependencies. quickly regular ideas haggle carefully. carefully express theodolites are carefully regular excuses. furiously regular instructions print quickly. asymptotes ar
42496	9	8379	996.23	n, even ideas sleep never even deposits. carefully special excuses dazzle blithely deposits.
42499	12	8532	996.20	ages. dependencies nag furiously even, special asymptotes. final accounts cajole slyly above the carefully pending packages: even courts are fluffily above the blithely unusual accounts! regular acc
42500	13	8181	832.50	ithely express theodolites across the express, regular excuses haggle above the unusual deposits. bravely fina
42502	15	1337	340.97	e accounts cajole furiously above the fluffily ironic deposits. blithely unusual theodolites detect among the ironic forges. special deposits against the fu
42507	20	3032	390.62	g to the furiously pending dependencies. slyly regular instructions above the furiously bold requests haggle quic
42508	21	9899	960.17	ly carefully pending courts. excuses boost furiously special pinto beans. final, express requests about the ironic dependencies hang blithely according to the 
42509	22	2613	389.06	s across the packages use quickly unusual theodolites. pending deposits after the carefully pending asy
42510	23	4778	103.84	sits are alongside of the special, regular accounts. fluffily special instructions kindle quickly after the ironic packages. silently regular requests wake furiou
42511	24	5978	951.50	special theodolites. furiously regular asymptotes are furiously about the quickly final deposits. blithely final accounts sleep slyly. carefully ironic deposits 
42515	28	819	81.70	lites. blithely pending requests nag across the regular deposits. special requests sleep blithely. quickly bold packages against the water
42516	29	5372	420.73	nal theodolites. slyly slow instructions use slyly across the furiously regular accounts. furiously regular deposits wake carefully among the ideas! fluffily special requests
42520	33	2798	654.39	s are alongside of the special, express instructions. accounts about the unusual, ironic dolphins affix quickly along the quick sentiments. even reques
42521	34	3233	532.97	 foxes cajole slyly ironic accounts. regular, express accounts inte
42524	37	9311	677.87	requests are carefully. carefully ironic ideas boost slyly against the theodolites. accounts wake carefully above the blithely express accounts. slyly pending accounts oug
42525	38	7378	7.91	ses: furiously stealthy pinto beans about the slyly regular foxes believe carefully slyly regular depths; s
42526	39	9582	116.03	lyly about the carefully special deposits; fluffily daring foxes against the sl
47539	44	9816	469.84	s. regular accounts wake slyly careful asymptotes. silen
42530	43	9360	276.41	r, bold sentiments. blithely final foxes lose quickly after the ideas: bold theodolites wake. furiously silent packages detect among t
42531	44	7245	746.54	n requests cajole. pending theodolites are fluffily above the ruthlessly even instructions. bold, final packages across the regularly
42532	45	2632	980.51	ctions wake quickly regular asymptotes. bold dolphins wake above the slyly bold grouches. furiously unusual foxes across the blit
42537	50	3675	649.25	ate carefully even pinto beans: quickly final warthogs may impress blithely. regular packages according to the carefully bold deposits haggle carefully pending foxes. carefully regula
44993	2	139	460.90	te carefully even accounts. slyly final requests upon the slyly special accounts wake even ideas? regular instructio
44994	3	9042	441.89	es; requests are slyly always regular packages. regular ideas wake above the quickly even ideas. quickly busy requests nag slyly quickly final dependencies. slyly express packages sleep. ex
44996	5	5278	132.16	e fluffily express ideas. blithely bold accounts haggle pending deposits. furiously ironic requests use blithely. bold attainme
45000	9	1187	328.67	es about the silent packages haggle unusual packages. f
45003	12	8756	844.70	after the fluffily unusual accounts haggle quickly blithely silent grouches. furiously unusual deposits lose slyly even requests. bravely s
45004	13	4831	631.54	lithely pending deposits. regular, ironic theodolites print blithely. requests 
45006	15	6199	590.65	slyly ironic packages from the slyly ironic tithes cajole at 
45011	20	7550	105.21	lithely pending deposits play quickly pending accounts. slyly express ideas unwind carefully. fluffily special foxes haggle fluffily quickly final epitaphs. ironic forges sleep fu
45012	21	2110	87.19	ronic platelets. ironic accounts sleep furiously at the foxes; express theodolites according to the ironic deposits haggle carefully final, final ideas. fu
45013	22	2951	937.74	 silent platelets. quietly even packages are carefully against the regular platelets. regular deposits nag flu
45014	23	5107	12.05	ing pinto beans. furiously unusual instructions haggle carefully about the saut
45015	24	1622	575.57	press deposits haggle slyly. furiously pending asymptotes sleep quickly.
45019	28	4940	82.66	 requests solve. express, silent dependencies cajole. furi
45020	29	4131	896.00	sits solve furiously furiously final foxes. blithely special platelets about the brave ideas was regular dependencies. ironic, express dep
45024	33	5094	106.15	 the fluffily ironic requests. furiously special instructions haggle slyly; pinto beans against the fluffily even packages haggle never against the carefully final deposits. final,
45025	34	1005	54.48	 deposits. brave platelets at the quickly final deposits wake fluffily along the dugouts. ironic dugouts haggle. regular, final packages boost carefully expres
45028	37	1884	383.90	the even deposits sleep ruthlessly carefully regular packages. regular, ironic attainments nag alongside of the special packages. furiously sly deposi
45029	38	5879	246.43	 ironic foxes alongside of the slyly even ideas are blithely thin platelets: pending packages across the carefully pending requests use furiously about t
45030	39	3713	100.07	arefully express accounts. accounts integrate. blithely regular requests sleep fluffily. quickly bold deposits cajole quickly regular ideas. slyly even packages sleep c
45034	43	6297	208.83	hs cajole blithely ironic dependencies. silent packages use 
45035	44	545	212.37	yly alongside of the carefully even accounts: regular foxes detect slyly ironic sauternes. final instructions near the f
45036	45	1170	110.82	along the slyly slow accounts. unusual epitaphs use blithely furiously regular requests. final, express requests boost. stealthily silent braids c
45041	50	7497	274.34	lthily about the pending packages. ironic requests are according to 
47497	2	1695	570.03	raids wake ruthlessly whithout the dependencies. special deposits wake c
47498	3	4253	730.02	fully even instructions about the stealthily final platelets sleep alon
47500	5	6833	236.45	latelets. quickly bold multipliers nag slyly furiously regular deposits. pending, even platelets according to the quickly special packages are about the ironic, pending foxes. unusual pinto beans 
47504	9	2754	567.05	ajole furiously unusual requests. furiously regular ideas use slyly-- quickly special instructions use. blithely regular courts engage slyly unusual platelets. silent, final sauternes after the 
47507	12	3002	246.97	c notornis lose quietly. packages are blithely according to the carefully express packages: slyly unusual acc
47508	13	4151	330.63	ully express deposits sleep. accounts eat furiously across the ironic, unusual asymptotes. fluffily regular deposits about
47510	15	1113	161.37	packages. slyly bold packages haggle. final ideas haggle blithely silent packages. slyly slow instructions detect along the regular packages. blithely special courts boost slyl
47515	20	3875	703.86	s. finally regular packages are carefully. unusual a
47516	21	5696	792.35	l packages boost alongside of the final, even accounts? slyly ruthless requests haggle slyly after the fluffily final accounts: final theodol
47517	22	1279	910.07	xes above the excuses wake carefully regular deposits. blithely bold theodolites alongside of the final, final deposits grow furio
47518	23	2623	418.08	ctions. pending ideas sleep slyly furiously final asymptotes. caref
47519	24	4333	322.38	efully bold dinos should believe against the furiously pending pinto beans! requests boost sl
47523	28	5444	22.29	the slyly regular accounts. slyly pending accounts after the regular packages sleep slyly blithely final sentiments. blithely final patterns print quickly against the fluffi
47524	29	654	778.51	ly bold deposits. furiously busy packages eat in place of the multipliers. furiously even deposits integrate quickly. c
47528	33	2086	680.19	ly pending packages. furiously ironic pinto beans above the foxes cajole quickly ironic dependencies. furiously unusual asymptotes shall have to haggle fluffil
47529	34	9862	65.09	s. carefully ironic theodolites sleep across the slyly bold packages. slyly final fret
47532	37	7616	423.56	fully final, enticing requests. even accounts cajole ruthlessly according to the slyly regular foxes. bli
47533	38	7964	736.61	e fluffily. special, even accounts cajole blithely amon
47534	39	7520	709.83	kly regularly final theodolites. instructions cajole fluffily. bold, regular instructions cajole since the slyly ironic deposits. slyly special dinos affix around the instruction
47540	45	1345	907.29	ously even, express theodolites. regular sheaves use. special requests affix ironic pinto beans. carefully special asymptot
47545	50	4356	660.21	 ideas. even, regular epitaphs haggle carefully regular
50001	2	9271	85.17	pecial requests print carefully. quickly silent instructions across the carefully unusual packages are ironic requests. slyly regular deposits sleep quickly. fu
50002	3	5397	939.41	es-- carefully regular accounts sleep fluffily instructions! silent dependencies wake slyly after the fluffily final requests. silently ironic ideas among the even deposits are quickly after the flu
50004	5	7145	425.34	 beyond the thinly final theodolites. furiously final asymptotes nag according to the furiously express request
50008	9	4036	918.37	s use carefully along the daringly even courts. slyly regular packages cajol
50011	12	6152	519.94	s boost quickly after the slyly special pinto beans. 
50012	13	2793	21.83	dazzle blithely according to the bold accounts. regular accounts cajole slyly around the slyly final asymptotes. carefully ironic packages use c
50014	15	9695	122.80	y even theodolites. special theodolites boost quickly among the express, even ideas. quickly even deposits boost after the 
50019	20	6413	889.83	n warthogs wake blithely. pinto beans lose blithely against the final, express deposits. requests according to the furiously regular accounts wake above the packag
50020	21	4953	817.76	e at the slyly even ideas? carefully regular requests grow. final deposits sleep around the furiously express requests. slyly iron
50021	22	8545	949.11	posits nag about the furiously close requests. carefully bold courts are carefully final depe
50022	23	982	53.96	above the slyly ironic requests engage slyly across the furiousl
50023	24	4715	181.88	eans among the final orbits haggle blithely along the packages. fluffily even packages at the carefully final deposits sleep along th
50027	28	9540	107.19	ounts. furiously even grouches boost blithely platelets. carefully unusual accounts might sleep fluffil
50028	29	802	340.03	y final foxes. ironic excuses cajole slyly ironic packages. slyly fin
50032	33	8046	97.89	ckly regular deposits wake fluffily above the quickly special excuses. slyly final foxes x-ray furiou
50033	34	228	199.06	c asymptotes. blithely ironic deposits above the furiously final packages wake blithely according to the blithely express accounts. special excuses upon the special package
50036	37	3098	573.67	s. blithely regular pinto beans among the even, ironic deposits wake furiously ruthlessly express theodolites. final deposits are slyly final instructions. express
50037	38	9382	487.21	ackages. blithely ironic accounts use furiously aga
50038	39	5061	659.74	uts. quickly ironic packages haggle alongside of the slyly ironic requests. quickly even pinto beans wake. express, even orbits through the quickly pendin
50042	43	6867	803.58	quickly ironic accounts boost blithely. final packages haggle at the blithely ironic excuses. final, e
50043	44	5286	630.15	above the carefully ironic pinto beans are theodolites. fin
50044	45	6765	963.68	quickly. blithely ironic ideas about the quickly pending foxes try to mold blithely for the furiously regular theodolites. blithely final reques
50049	50	9526	983.71	efully regular deposits integrate carefully express requests. dogged, regular packages haggle furiously. theodolites sleep quic
52486	2	5785	178.83	o the slyly blithe requests. excuses against the slyly even accounts are slyly ironic instructions. furiously regular instructions detect carefully. courts are ironic requests. slyly unusual plate
52487	3	5004	396.42	ss the furiously even requests. quickly pending platelets wake slyly unusual dolphins. slyly pending deposits a
52489	5	9018	519.51	otes wake blithely across the bold accounts. blithely regular foxes about the final requests nag boldly about the fluffily final theodol
52493	9	2773	278.74	lar platelets sleep furiously blithely regular accounts. regular, ironic dependencies sleep furiously quickly regular accounts. slyly re
52496	12	4948	939.17	ost around the furiously even pinto beans. regular deposits thrash according to the carefully special theodolites. accounts sleep against the blithely fi
52497	13	4485	420.45	special pinto beans. regular dolphins integrate furiously above the quickly pending requests. slyly sly pinto beans serve. carefully express foxes ca
52499	15	9900	527.82	l attainments wake carefully sly pinto beans. furiously bold instructions are slyly across the accounts. regular, i
52504	20	9548	111.79	equests. final accounts wake doggedly. slyly regular accounts are blithely across the regular, bold pack
52505	21	5273	379.23	ggle slyly quickly express deposits. carefully regular foxes nag according to the even deposits-- ev
52506	22	5953	958.17	ndencies sleep among the final requests. blithely regular deposits haggle slyly even theodolites. final theodolites cajole carefully. furiously regular theodolites acr
52507	23	6475	559.38	arefully even accounts: sometimes unusual accounts nag blithely pending forges. unusual requests use against the carefully regula
52508	24	5817	555.20	nic requests. final, ironic pinto beans use about the fluffily final packages. slyly final fox
52512	28	8652	204.23	eodolites cajole furiously accounts. fluffily bold packages are blithely furiously final theodolites. special theodolites cajole carefully final
52513	29	1563	176.08	 hang carefully after the never special decoys. even asympt
52517	33	80	244.92	sts. slyly final instructions around the carefully special courts boost outsi
52518	34	6547	752.01	ses. slyly final foxes nag furiously after the regular deposits: furiously special excuses against the quickly final requests are quickly above the blithely 
52521	37	9076	468.88	ut the blithely regular ideas sleep daringly quickly ironic tithes. quickly special deposits sle
52522	38	326	249.37	yly. regular requests about the furiously regular packages wake eve
52523	39	5954	981.11	nding instructions sleep against the requests. furiously final instructions haggle furiously bold accounts. f
52527	43	8105	821.15	ounts. furiously even deposits integrate along the fluffily bold instructions. even, ironic packages between the slyly final theod
52528	44	8157	907.91	inal requests solve around the packages. furiously regular deposits haggle fluffily along the finally even accounts. unusual depths are fluffily pend
52529	45	6806	987.54	 pinto beans was above the blithely express requests. furiously silent request
52534	50	9786	929.24	. carefully pending foxes snooze. theodolites cajole slyly; fluffily even deposits might haggle accounts. slyly even packages run always above the slyly 
54991	2	2451	817.19	iously. bold packages nag: ironic packages hang quickly above t
54992	3	7410	667.53	above the slyly special ideas kindle blithely quietly pending frays-- slyly special deposits sleep blithely above the carefully even requests. caref
54994	5	5555	639.33	le according to the carefully regular instructions. regular, even excuses are enticingly regular pack
54998	9	805	794.64	t packages. sometimes regular foxes boost carefully blithely regular ideas. packages wake furiously slyly pending deposits. slyly regular accounts against the final requests cajole alon
55001	12	4704	151.20	ily ironic packages. blithely bold deposits integrate carefully before the bravely silent deposits! carefu
55002	13	8215	696.48	es. slyly express instructions cajole. slyly express accounts boost after the carefully final ideas. ironic foxes haggle around the ironic deposits. furiously final deposits unwind furiously accordi
55004	15	8821	111.04	final excuses haggle slyly atop the fluffily special patterns. carefully regular packages sleep slyly even theodolites! fluffily regular excuses around the slyly ironic foxes would cajole furiously
55009	20	4978	318.43	usly brave foxes nod platelets. slyly bold deposits wake slyly against the even accounts. slyly regular ideas mold carefully f
55010	21	1624	888.28	 bold, bold foxes. carefully regular deposits are quickly. ironic requests sleep slyly furiously daring accounts. blithely final foxes haggle furiously even dependencies. furiously special req
55011	22	3550	70.36	nic accounts across the blithely special requests maintain carefully carefully pending multipliers. regular,
55012	23	809	393.51	ns believe carefully. fluffily unusual requests sleep. silent sheaves sleep according to the 
55013	24	4499	6.99	 final pinto beans haggle blithely. carefully final ideas ha
55017	28	288	893.73	er blithely unusual accounts. accounts affix theodolites. careful requests above the quickly 
55018	29	1631	2.93	ully final packages. stealthy theodolites haggle slyly requests. regular, ironic instructions nag slyly. 
55022	33	305	101.63	er. silent dependencies wake slyly pending packages. dependencies cajole quickly bold packages. pending, regular requests must integrate above the final accounts
55023	34	3393	758.98	en ideas affix blithely by the unusual deposits. regular, pending requests are with the excuses. slyly even deposits wake. evenly ironic account
55026	37	9601	861.61	edly pending platelets. sometimes silent theodolites detect blithely with the even patterns. blithely bold instructions a
55027	38	2333	31.42	y regular realms. regular platelets are quickly about the blithely ironic theodolites. quickly ironic accounts cajole furiously. slyly ironic ideas poach. pending accounts cajole bold r
55028	39	1164	535.05	ges cajole. final, final packages about the slyly bold pinto bean
55032	43	4758	725.14	s. slyly ironic dolphins are furiously. quickly unusual accounts are. slyly ironic pinto beans sleep carefully. final pinto beans sl
55033	44	3173	134.93	e-- slyly ironic courts sleep furiously regular foxes. silent requests integrate 
55034	45	7656	413.67	furious requests haggle about the boldly unusual asymptotes: blithely special requests behind the furiously ironic dep
55039	50	7953	811.73	. final, final requests nag carefully along the final theodolites. furiously ironic platelets affix carefully careful dolphins. special, final theodolites nag among the furious
57531	37	4317	214.22	tructions. carefully final foxes sleep. furiously ironic accounts believe. bold, express requ
57496	2	6554	601.74	ess packages cajole. express, special requests are after the quickly regular requests. even requests above th
57497	3	2938	328.46	ully even foxes haggle carefully fluffily final requests. deposits boost quickly carefully bold deposits. fluffily ir
57499	5	5417	810.05	luffily silent accounts. fluffily express deposits detect carefully among the accounts. slyly regular deposits across the quick packages are final frets; foxes wake furiously. blithely regular depen
57503	9	7586	368.05	slyly bold instructions boost after the deposits. blithely regular accounts according to the furiously ironic accounts will hav
57506	12	6269	478.60	gular notornis cajole. carefully regular accounts wake about the 
57507	13	5573	821.90	lyly about the ruthlessly final deposits. quiet, bold requests cajole about the requests. slyly ironic deposits wake. 
57509	15	2574	650.56	lithely regular deposits. carefully special requests wake carefully about the carefully regular packages. furiously regular requests are 
57514	20	5298	173.00	nag after the furiously regular accounts. busy ideas cajole regularly along the furiously ironic instructions. quickly ironic packages nag furiously ironic dependencies. carefully express requests d
57515	21	7538	260.39	t to are among the fluffily ironic dependencies; slyly furious instructions are carefully packages. quickly ironic asympt
57516	22	4099	881.52	ongside of the slyly regular accounts. express pinto beans across the ironic accounts boost along
57517	23	2136	343.26	ts are bold accounts. ironic ideas detect furiously. ironic 
57518	24	9621	541.77	deposits about the regular accounts wake furiously carefully ironic acc
57522	28	6959	243.87	l excuses along the furiously unusual accounts maintain after the silent accounts. acco
57523	29	5241	645.90	refully unusual platelets detect? carefully final foxes about the special asymptotes haggle quickly
57527	33	2455	891.65	sits along the quickly bold pinto beans are finally above the blithely final pinto beans. furiously unusual theodolites would cajole quickly re
57528	34	7111	59.65	y bold deposits. packages among the carefully special deposits x-ray deposits. deposits alongside of the silent packages sleep carefully among the pending accounts. blit
57532	38	1098	269.53	symptotes about the even pinto beans detect quickly regular accounts. blithely special packages sleep carefully.
57533	39	6015	958.63	o beans believe slyly. unusual hockey players snooze slyly express requests. slyly special accounts are carefully blithely regular packages. packages detect slyly.
57537	43	5468	174.88	ounts cajole slyly. bold pinto beans are among the blithely unusual accounts. regular platelets are regular foxes. slyly final requests are across
57538	44	6058	768.30	 theodolites cajole. final, special accounts cajole carefully along 
57539	45	3530	971.92	instructions wake fluffily foxes. slyly slow grouches according to the slyly unusual ideas doubt furiously among the furiously bold deposits. fluff
57544	50	7615	440.10	odolites boost after the final, regular dependencies. instructions boost furiously final, 
60001	2	1770	535.64	inal instructions. express accounts wake fluffily against the carefully final requests; express sheaves affix sly
60002	3	6686	746.49	fully furiously final packages. carefully special instructions haggle accor
60004	5	3365	13.12	nding accounts. express accounts wake furiously according to the fluffily pending theodolites. quickly regular orbits sleep. carefully regular packages sleep. slyly pending foxes according to the
60008	9	1108	789.98	special dependencies against the carefully even accounts cajole ironic requests. ironic accounts use blithely slyly regular theodolites. furiou
60011	12	6781	550.18	g waters. carefully regular accounts among the ironic, express deposits boost carefully
60012	13	7873	400.10	oxes haggle above the bold, pending accounts. bold instructions are fluffily furiously final packages. regular theodolites sleep blithely after the excuses. express deposits along the bold deposits 
60014	15	276	799.34	rses? slyly regular theodolites nag blithely against the blithely stealthy packages. even packages after the blithely regular pains cajole carefully qu
60019	20	9642	608.74	ts. permanent, special packages sleep. even, idle asymptotes boost above the ironic
60020	21	7473	194.55	yly. furiously express pinto beans boost. fluffily ironic theodolites haggle quickly among the fluffily express requests. 
60021	22	9457	737.55	c pinto beans. express, express packages wake. fluffily final accounts was. quickly regula
60022	23	9468	444.11	lar foxes. fluffily special foxes against the bold platelets should boost abov
60023	24	6731	542.37	s. carefully regular excuses snooze slyly even packages. special, bold accounts haggle furiously. accounts integrate careful
60027	28	4206	332.79	lar multipliers-- slyly brave ideas nag fluffily. carefully regular packages a
60028	29	697	318.85	ly even requests across the furiously ironic ideas need to use furiously about the pending requests. deposits use quickly around the final pinto beans. unusual requests cajole quickly carefu
60032	33	7450	677.93	s cajole furiously final platelets. furiously regular theodolites cajole furiously special accounts. furiously even foxes cajole carefully. carefully final d
60033	34	5333	599.61	ic accounts. bold requests do haggle slyly furiously pending accounts. multipliers wake carefully through the regular deposits. ironic deposits poach? slyly bold packages against the even accounts s
60036	37	6196	903.66	 even packages shall boost according to the slyly pending deposits. unusual requests cajole slyly s
60037	38	2211	796.67	ously regular packages. express foxes nag carefully. final, ironic pearls sleep. regular packages sleep. furiously silent accounts affix fluffily across the express, regular de
60038	39	1371	226.24	he boldly ironic accounts. fluffily express accounts are slyly blithely special packages. even, silent deposits hinder blithely blithely regular packages. ev
60042	43	2572	582.26	iously regular deposits. pending instructions cajole quickly blithely special platel
60043	44	4152	735.57	ross the blithely express instructions. quickly busy accounts above the pending deposits are furiously b
60044	45	2267	320.78	silent requests. ironic courts use fluffily dependencies. pending theodolites are after the fluffily special theodolites
60049	50	185	629.08	unusual dependencies wake permanently. furiously even theodolites among the furiously regular deposits wake furious
62483	2	1057	507.70	 quickly special instructions. ironic foxes cajole fur
62484	3	8259	261.73	ies haggle furiously final, regular grouches. bold requests are quickly regular warhorses. ironic packages wake across the excuses. furiously final asymptotes thrash among the carefully
62486	5	8754	600.94	nusual dependencies. slyly express gifts boost ruthlessly quickly daring theodolites. final theodolites above the ideas sleep fluffily after the furiously silent foxes. slyly ironic inst
62490	9	1646	834.42	es run blithely even pinto beans. regular escapades sleep. blithely pending accounts boost blithely. ironic, pending deposits haggl
62493	12	6777	389.99	ccording to the regular requests. slyly final packages sublate blithely regula
62494	13	8028	66.31	cial frays. carefully unusual realms affix! regular asympto
62496	15	6649	784.62	ckly above the final, silent accounts. final dinos haggle along the blithely regular sauternes-- ironic pl
62501	20	4778	582.88	the furiously even braids snooze carefully instructions. requests use. final packages about the regular deposits must have to wake across the carefully unusual pa
62502	21	2837	475.18	l, ironic pinto beans are furiously. regular, express theodolites cajole quickly according to the accounts. blithely pending instructions sleep carefully warhorses. quietly express pinto beans 
62503	22	1015	193.22	olphins. blithely final accounts around the slyly regular deposits use carefu
62504	23	9927	902.11	ously ironic foxes after the furiously silent ideas boost carefully b
62505	24	944	530.13	tions haggle furiously among the final attainments. quickly even asymptotes haggle blithely along the silent, special accounts. packages detect 
62509	28	9776	39.56	thely according to the furiously unusual packages. slyly furious instructions boost furiously final excuses. blithely final deposits boost. fluffily r
62510	29	7541	433.24	yly ironic requests detect along the carefully blithe packages. regular asymptotes boost. quiet accounts ac
62514	33	2120	254.64	cial deposits use. unusual deposits sleep blithely against the final deposits. platelets detect carefully bold ideas. final, regular deposits cajole according to the silent, ironic instructions
62515	34	7327	946.16	lar sheaves. regular, careful deposits could have to are regular requests. blithely final accounts cajole blithely express, bold packages. 
62518	37	9729	333.64	ironic deposits. furiously quiet foxes play regular depo
62519	38	4961	139.65	p quickly slyly silent deposits. platelets against the slyly regular foxes wake quickly slyly regular instructions. slyly fluffy deposits are above the final p
62520	39	3986	241.62	uriously across the regular orbits. carefully final theodolites haggle quickly according to the slyly silent tithes. silent pinto beans wake carefully
62524	43	5140	263.22	ntiments. quickly unusual packages sleep regularly. bold requests sleep closely across the blithely dogged packages. carefully regular deposits use furiously. carefully express realms ag
62525	44	7638	581.75	ts around the slyly even frays nag quickly blithely express excuses. ideas sleep. regular deposits detect regularly above the fluffily ironic waters. blithely pe
62526	45	4264	222.02	ependencies. slyly final packages print carefully. carefully even packages boost quickly across the bravely pending foxes. furiously ironic notornis among the accounts 
62531	50	8825	419.62	gside of the ideas. silent foxes wake quickly carefully pending asymptotes. pending requests cajole even platelets. blithely ironic instruction
64989	2	2768	742.67	even ideas cajole enticingly against the pinto beans. even requests wake slyly. bold, even packages haggle slyly furiously regular dependen
64990	3	2317	342.19	usual, bold deposits haggle fluffily. carefully final request
64992	5	7489	917.94	packages solve finally. even, final foxes use. bold ideas haggle 
64996	9	278	476.06	 escapades engage slyly. ironic, ironic deposits wake. silent excuses print furiously. finally bold theodolites sleep blithely. furiously regular foxes 
64999	12	303	687.68	ully dogged dolphins. requests are after the special packages. excuses print fluffily across
65000	13	851	208.13	o beans according to the fluffily dogged ideas unwind fluffily regular packages. silent packages may kindle furiously carefully ironic requests. carefully pending packages against
65002	15	6551	360.73	refully regular deposits. express theodolites after the furiously bol
65007	20	8396	509.61	ites wake carefully. quickly final ideas sleep enticingly fluffily pending deposits. carefully regular requests should have to detect. deposits wake blithe accounts. blithely regular ideas accordi
65008	21	4006	844.48	ole at the furiously final deposits. slyly express pinto beans hang. ironic, final deposits cajole according to the regular, express asymptotes? carefully furious ideas past the careful
65009	22	4186	203.18	ions are furiously slyly even accounts. blithely ironic packages
65010	23	9277	584.27	on the furiously final courts. pending, even foxes boost blithely. quickly final deposits wake. slyly final accounts 
65011	24	4862	263.48	 furiously. packages cajole blithely fluffily regular packages. blithely ironic platelets believe blithely thinly silent requests. blithely pending asymptotes after the theodolite
65015	28	5413	621.87	blithely silent instructions. bold, ironic packages cajole. pinto beans cajole across the never express pinto beans. fluffily regular foxes are fluffil
65016	29	8801	683.19	uick ideas dazzle blithely special ideas. carefully ironic pinto beans run bravely. slyly ironic asymptote
65020	33	4595	28.22	requests. quickly bold accounts are furiously around the regular theodolites. blithely reg
65021	34	4801	158.14	l excuses haggle foxes. unusual, regular deposits beyond the special, fluffy accounts use fluffily slyly ironic deposits. accounts above the ne
65024	37	8070	138.75	lar, ironic pinto beans. bravely unusual instructions sleep carefully above the quickly special ideas. blithely even braids integrate. slyly s
65025	38	9755	865.76	 requests against the blithely even instructions are quickly careful, ironic theodolites. furiously ironic requests run according to the ironic platelets. silently 
65026	39	8830	192.24	ites haggle. unusual, pending dolphins boost quickly furiously unusual ideas. permanent foxes cajole about the slyly bold acco
65030	43	1399	234.98	ithely through the quickly unusual asymptotes. final requests boost blithely. blithely regular ideas engage quickly above the carefully brave packages
65031	44	2281	186.43	 slyly near the unusual accounts. special, regular epitaphs boost quickly among the quickly even instructions. slyly ironic pinto beans against the bra
65032	45	9911	411.32	lly quickly bold instructions. fluffily final accounts use. blithely ironic no
65037	50	4674	995.28	nal escapades wake. carefully final courts haggle blithely. blithely final ideas are slyly deposits. ironic, regular deposits affix furiously car
67495	2	8082	348.42	es sleep fluffily excuses. fluffily unusual platelets wake fluffily. carefully enticing packages accordi
67496	3	7863	209.16	es. carefully ironic deposits after the blithely express dependencies wake carefully express, final accounts. 
67498	5	9387	221.77	ully after the slyly stealthy accounts. accounts haggle boldly furiously close pinto beans. furiously iro
67502	9	1603	692.66	ck dolphins. unusual, express theodolites was fluffily. carefully ironic theodolites sleep. quickly bold accounts cajole slyly quickly final requests. carefully even pin
67505	12	2118	271.68	press packages affix fluffily throughout the ironic requests. furiously pending deposits sleep furiously alongside
67506	13	4551	142.19	ld deposits. excuses cajole. ideas are. final packages against the blithely unusual deposits 
67508	15	7059	828.32	 ideas. carefully special platelets sleep. regular dolphins wake furiously regular requests-- furiously regular somas sleep furiously
67513	20	9020	742.03	ymptotes. ideas use blithely. slyly ironic packages sleep carefully about the unusual, ironic excuses: slyly ironic deposits across the quickly pending requests nag fluffi
67514	21	125	636.65	ickly special deposits? carefully pending packages along the carefully regular dolphins cajole furiously across the slyly final asymptotes. 
67515	22	1507	570.71	 regularly regular escapades are. accounts sleep among the carefully regular packages. carefully even deposits sleep closely regular instructions. furiously iro
67516	23	5350	927.54	ow warhorses. regular instructions sleep carefully after the special, regular pinto beans. slyly silent theodolites nag alongside of the furious
67517	24	7354	356.97	oys. regular, even requests sleep silent pinto beans. busy courts cajole final accounts. b
67521	28	4457	319.42	 play fluffily. slyly regular packages cajole according to the instructions. blithely ironic accounts wake carefully furiously special 
67522	29	1715	641.79	es detect carefully furiously express requests. ideas cajole carefully. furiously bold pinto beans cajole fluffily. bold deposits wake furiously according to the express platelets. bold requests sle
67526	33	5684	886.81	ecial deposits. daring, final requests sleep fluffily; carefully regular excuses nag fluffily dependencies; packages are blithely. carefully even foxes use fluffily ironic, final foxes. ironic 
67527	34	7829	471.28	s: quickly sly theodolites affix unusual theodolites. slyly quick platelets wake regular, special accounts. regular foxes use slyly after the furiously even dolphins. blith
67530	37	8113	928.90	. furiously regular accounts nod slyly final excuses. unusual requests haggle blithely furiously stealthy dependencies. slyly regular theodolites around the even foxes
67531	38	8017	447.50	ites. ideas sleep special accounts. furiously regular requests unwind. quickly ironic realms cajole slyly even pinto beans! blithely even foxes haggle alongside
67532	39	345	250.88	sly blithely regular accounts. carefully even frets may wake above the ironic accounts. furiously unusual platelets dazzle among the blithely express theodolites. slyly ironic ideas det
67536	43	9370	576.51	ns. instructions are after the regular, final deposits. theodolites about the carefully even requests use furiously regular requests. slyly ruthless packages cajole carefully at the fur
67537	44	9223	212.32	e furiously according to the slyly regular deposits. unusual, ironic packages try to maintain acco
67538	45	6276	524.54	lly regular requests nag carefully quickly ironic ideas. bold accounts haggle idly silent theodolites. ironic, bold pinto beans sleep. ironic, ironic packages cajole. ironic foxes ca
67543	50	5242	435.54	ies engage around the unusual, express accounts. slyly ironic requests haggle dependencies. ironic, bold courts ha
70012	13	8198	783.21	s sleep silently fluffily regular theodolites. requests cajole
70001	2	909	276.85	old pinto beans. carefully pending requests according to the regular requests kindle across the quickly regular dep
70002	3	7095	853.00	ix regular foxes. carefully final deposits nag slyly abo
70004	5	711	277.56	, special dolphins sleep quickly. silently express deposits use slyly 
70008	9	8320	737.22	 furiously final theodolites wake slyly across the courts. ironi
70011	12	4689	443.78	ong the foxes. requests haggle. furiously express grouches integrate. carefully final packages use quickly. slyly even foxes integrate slyly along 
70014	15	8032	675.03	eas against the furiously even accounts cajole about the fluffily final packages; ironically regular deposits doze slyly. slyly r
70019	20	8460	167.96	ously after the even, busy deposits. carefully final requests dazzle. f
70020	21	8725	76.36	y. carefully regular pinto beans sleep quickly according to the idle packages. silent asymptotes boost slyly across the furiously bold accounts. blithely regular packages after the 
70021	22	1890	730.16	s wake carefully pending deposits. furiously ironic accounts are. final asymptotes are furiously even patterns. carefully ruthless
70022	23	8683	470.61	ding to the quickly express ideas haggle slyly among the furiously express accounts
70023	24	1010	342.09	usly according to the regularly ironic instructions. carefully pending theodolites alongside of the dogged, unusual foxes cajole slyly a
70027	28	4998	455.99	osits doubt carefully above the close deposits. special, special accounts are against the fluffily unusual instructions. dependencies cajole slyly. accounts across th
70028	29	797	933.18	osits cajole fluffily. ironic, blithe requests grow busily regular, even theodolites: daring accounts integrate carefully 
70032	33	4398	111.84	heodolites haggle about the furiously even ideas. even, regular ideas cajole. ironic deposits nag slyly unusual, regular instructions. carefully bold packages haggle acros
70033	34	5906	888.95	osits use slyly forges! quickly bold asymptotes are fluffily. deposits haggle quickly. furiously final packages promise across the silent, final pinto beans. furiou
70036	37	2692	200.98	ss the slyly special requests. special, regular foxes run up the fluffily spe
70037	38	2994	452.78	hely carefully regular dolphins. quickly even excuses engage idly against the busy dependencies. busily ironic dependencies according to the final requests detect a
70038	39	7684	474.19	ual requests. blithely special pinto beans accordi
70042	43	1997	972.58	ages doubt blithely. ironic accounts use past the quickly final instructions. stealthily ironic theodolites lose blithely carefully unus
70043	44	5273	650.70	 alongside of the blithe packages. ironic instructions cajole foxes. carefully pending theodolites run quickly above the final requests! fluff
70044	45	221	37.09	uickly final asymptotes. silent asymptotes along the carefully ironic excuses cajole fur
70049	50	5906	31.45	slyly alongside of the regular grouches. blithely express foxes haggle slyly even ideas. bold instructions wake after the slyly 
72498	20	1931	320.14	 theodolites engage carefully. furiously final packages haggle. express dependencies nag above the ironic ins
72480	2	2974	993.11	ns. slyly pending foxes boost closely about the fluffily regular instructions. ironic instructions unwind blithely. ironic foxes detect 
72481	3	2594	48.45	ly regular ideas detect. blithely regular ideas nag carefully. furiously quiet packages boost. regular deposits nag. quick accounts hang ruthlessly carefully pending requests. quic
72483	5	1100	583.60	odolites are daringly above the fluffily unusual foxes. final deposits was. bold, regular patterns nag along the
72487	9	4388	790.85	ly quick requests? blithely bold instructions above the ironic orbits breach slyly even asymptotes. blithely expr
72490	12	3755	225.41	 blithely special instructions cajole furiously after the slyly final instructions. express, final accounts m
72491	13	8523	946.84	ng to the carefully even accounts boost slyly furio
72493	15	7017	510.62	bold instructions haggle across the furiously regular pinto beans. furiously special asymptotes cajole 
72499	21	9528	795.52	ing foxes haggle fluffily across the theodolites! bold asymptotes are unusual, fluffy asymptotes. carefully final frays nag blithely. requests against the final pearls thrash re
72500	22	6820	304.42	l, even deposits. blithely regular pinto beans sleep furiously blithely final excuses. unusual packages are slyly furiously regular dependencies. bli
72501	23	6605	635.20	sits according to the blithely final accounts haggle at the regular packages. carefully final Tiresias are slyly bold pains. final foxes bo
72502	24	1109	501.65	usual instructions sleep among the final accounts. blithely unusual packages engage along the even, special theodolites. unusual requests
72506	28	2393	90.05	ggle carefully fluffy accounts. carefully final deposits are slowly regular deposits.
72507	29	9759	674.44	iously pending requests hang after the furiously ironic accounts. ironic deposits across the regular accounts boost furiously deposits. quickly regular foxes would sleep 
72511	33	643	204.54	deposits sleep. furiously ironic ideas boost carefully express dolphins. ironic, even excuses cajole. special
72512	34	5743	871.87	e fluffily about the quickly regular pinto beans. bold packages among the slyly regular foxes haggle quickly regular g
72515	37	978	26.84	 platelets affix slyly about the regular, final platelet
72516	38	3347	400.81	at along the blithely regular instructions. express packages cajole quickly even accounts. blithely
72517	39	2205	797.49	posits affix slyly ironic dependencies. carefully unusual orbits doze carefully. carefully final orbits serve f
72521	43	9359	337.47	s. furiously special ideas sleep: special, express asymptotes about the bold deposits nag fluffily about the regular sauternes. furio
72522	44	8726	985.73	regular, pending dependencies. idly ironic sentiments wake
72523	45	5954	879.83	ously unusual pinto beans are slyly ironic ideas. slowly furious theodolites under the carefully ironic accounts x-ray special ideas. final theodolites slee
72528	50	6067	911.85	deposits sleep slyly ironic packages. ironic theodolites cajole quickly final instructions. furiously pending accounts haggle quickly. fluffily
74987	2	59	844.36	efully fluffy accounts cajole ironic packages. furiously
74988	3	5120	283.07	es. quickly ironic excuses cajole-- quickly quiet ideas are atop the sometimes blithe depe
74990	5	1875	782.59	x blithely. fluffily pending frays eat blithely blithely even ideas. quic
74994	9	3614	764.58	to beans; even deposits boost blithely. furiously final excuses above
74997	12	8001	878.95	nto beans haggle. blithely even theodolites among the blithely final packages detect slyly alongside of the slyly silent theodolites. theodolites sleep car
74998	13	9998	35.74	eposits are. carefully unusual somas cajole across the furiously regular requests.
75000	15	7133	715.72	pecial requests impress slyly blithely ironic deposits. slyly regular accounts detect fluffy instructions. theodolites at the blithely special requests integrate carefully expr
75005	20	1507	86.10	ular foxes are carefully across the blithely bold pinto beans. carefully unusual foxes nag carefully express packages. carefully bold accounts across the
75006	21	5501	97.67	r theodolites. permanently pending deposits cajole fluffily regular theodolites-- even theodolites
75007	22	9354	466.94	sleep blithely. furiously regular dependencies haggle carefully. final deposits unwind? furiously regular requests boost. 
75019	34	3980	746.56	 regular theodolites wake. even, final asymptotes poach regular, even accounts. regular, bold saut
75008	23	6871	155.27	dolphins. fluffily ruthless instructions detect furiously slyly special foxes. unusual depths after the regular ideas wake quickly about the slyly even reques
75009	24	9799	277.14	oxes. slyly regular packages boost fluffily. slyly unusual dependenci
75013	28	4499	959.33	 packages-- slyly express deposits after the silent, regular deposits sleep quickly on the slyly ironic dependencies. accounts about the packages int
75014	29	2629	386.27	requests use slyly. ironic accounts cajole sometimes-- slyly ironic requests breach along the bold, pending depths. packages haggle. carefully pending requ
75018	33	1615	6.19	fully special requests. boldly bold attainments detect regular foxes. deposits boost carefully above the asymptotes. blithely brave requests wake slyly around th
75022	37	652	165.70	he regular accounts. quickly express packages integrate quickl
75023	38	9070	83.69	g the ironic, ironic foxes. quickly even pinto beans 
75024	39	230	866.46	pades. blithely bold accounts across the gifts sleep about the ironic instructions. asymptotes above the furiousl
75028	43	764	761.05	ly even foxes? unusual packages along the furiously ironic ideas must have to haggle carefully final dependencies. quickly unusual accounts around the furiously regular reques
75029	44	6508	714.19	etect. regular pinto beans sleep furiously about the final, ironic foxes. platelets nag furiously carefully final instructions. perm
75030	45	3025	393.56	escapades grow furiously furiously pending platelets. theodolites detect. slyly pending braids boo
75035	50	2007	145.94	usly idle packages cajole blithely. regular, special instructions use quickly
77494	2	5604	6.95	y special excuses after the permanently quick requests cajole blithe
77495	3	5375	554.32	ts. blithely pending instructions nag furiously platelets. bold packages haggle fluffily alongside of the deposits. blithely express dependencies wake slyly. slyly unusual excuses n
77497	5	5532	800.22	special packages nag fluffily. blithely regular ideas snooze furiously slyly unusual foxes. special requests sleep carefully across the furiou
77501	9	9172	418.91	hely furiously regular packages; furiously special requests cajole carefully special platelets! furiously regular courts are. carefully ironic asymptotes 
77504	12	1444	174.85	telets around the platelets nod beside the accounts. furiously express deposits use blithely. bold, special requests
77505	13	6590	902.13	special excuses across the fluffy, ironic packages haggle slyly carefully even requests. pending deposits according to the accounts bo
77507	15	5002	70.90	nusual ideas detect. carefully ruthless dependencies according to the accounts cajole carefully alongside of the final instructions! regular sauternes use grouches. final, regular requests wake sl
77512	20	7748	439.29	uriously express foxes. silent instructions snooze among the final requests. packages serve. fl
77513	21	5334	616.31	e blithely ironic foxes. carefully even excuses for the regular, regular excuses believe furiously p
77514	22	155	198.98	counts wake enticingly stealthily regular Tiresias. ironic, express patterns wake slyly blithely ironic dependencies. fluffily ironic packages throughout the slyly expr
77515	23	8199	681.20	r grouches eat. ideas are furiously regular asymptotes. instructions are carefully requests. blithely ironic accounts cajole furiously about the regular packages! special, iron
77516	24	620	588.03	iously slow platelets. blithely permanent deposits after the blithely regular requests 
77520	28	3509	41.07	es among the quickly final decoys cajole among the epitaphs. bold accounts wake fluffily furiously blithe as
77521	29	6071	271.95	about the special requests. carefully bold ideas detect across the carefully regular accounts. ironic foxes nod. pinto beans p
77525	33	9750	853.67	uickly blithely silent asymptotes. furiously regular requests cajole blithely according to the ironic accounts. furiously final packages run fluffily even instructions. s
77526	34	6391	747.83	kages use among the carefully bold dolphins. carefully special deposits about the sl
77529	37	226	816.52	r the slyly ironic packages. even, bold platelets after the blithely express foxes b
77530	38	5917	389.54	. regular dependencies wake furiously. slyly final accounts should have to haggle regular de
77531	39	6220	509.58	slyly regular accounts are quickly according to the carefully regular requests. carefully special depo
77535	43	7589	332.36	st the fluffy pinto beans nag according to the blithely idle platelets. bold packages above the final, unusual theodolites haggl
77536	44	356	788.90	 packages cajole alongside of the slyly blithe tithes. even dolphins sleep slyly. slyly ironic deposits above the bo
77537	45	2784	907.09	fully special deposits: blithely ironic courts haggle furiously: blithely final requests sleep carefully evenly unusual foxes. caref
77542	50	2490	735.52	ial dinos integrate furiously according to the slyly regula
80001	2	1080	29.93	r foxes-- express ideas haggle. accounts cajole pending, ironic packages. bold, pend
80002	3	747	393.74	counts boost against the carefully even waters. furiously re
80004	5	6565	285.37	kages haggle about the slyly express excuses. blithely pending packages cajole about the foxes! un
80008	9	9739	641.61	ilent deposits. quickly final theodolites across the slyly bold patterns snooze blithely ironic accounts! caref
80011	12	7292	917.33	kly special packages. carefully silent deposits nag blit
80012	13	4935	90.13	ar packages wake blithely into the theodolites. furiously ironic courts use. regular, pending accounts sleep fluffily. express d
80014	15	4059	26.96	iously express asymptotes thrash pending accounts. slyly regular excuses haggle fluffily against t
80019	20	9812	108.76	e across the deposits. final deposits use along the silent theodolites.
80020	21	4266	511.50	 attainments detect furiously slyly bold instruct
80021	22	7452	765.79	. excuses cajole carefully regular ideas? ironic, bold pinto beans are. pinto beans boost fluffily along the b
80022	23	9774	5.44	lly final deposits. blithely regular deposits above the slyly final requests use furiously at the idle sauternes. unusual deposits haggle after the furiously pending req
80023	24	1117	685.56	ans. pending foxes thrash; accounts wake carefully according to the requests
80027	28	4184	996.23	 sleep quickly final foxes. quickly special packages wake carefully even foxes. furiously ironic courts about 
80028	29	5940	464.59	ng the slyly regular foxes wake packages. fluffily unusual deposits sublate idly according to the silent theodolites. ironic instructions was against the busy accounts. fluffily ironic platelets
80032	33	2442	394.88	 accounts about the final deposits use around the courts. quickly even accounts affix slyly. ironic requests
80033	34	1568	8.66	packages. requests according to the carefully regula
80036	37	1989	239.37	uickly quiet requests. blithely final requests doubt carefully deposi
80037	38	7920	957.85	during the unusual hockey players impress slyly according to the slyly brave deposits. carefully final platelets after the furiously even foxes wake dependencies
80038	39	3774	315.02	sauternes. special accounts cajole blithely furiously express accounts. furiously express theodolites hagg
80042	43	9298	884.15	y brave theodolites haggle quietly carefully regular
80043	44	8885	591.19	lyly unusual deposits. final theodolites use fluffily ironic requests. pi
80044	45	4705	925.74	gside of the slyly regular packages. brave pinto beans 
80049	50	8826	610.25	thely final asymptotes use slyly unusual pinto beans. theodolites against the blithely even sheaves sleep deposits. furiously regular depos
82477	2	4739	976.42	 blithely regular instructions hang up the deposits. quickly unusual 
82478	3	7183	233.51	e furiously final packages; slyly pending patterns 
82480	5	7423	421.06	s: quickly bold dependencies use. slyly regular deposits cajole. bold, ruthless instructions sleep after the carefully final sentiments. furiously ironic requests wake
82484	9	5632	351.92	ithely unusual deposits are slyly above the requests. ironic, unusual asymptotes across the furiously final requests affix quickly among the unusual packages. spec
82487	12	3886	780.63	ong the fluffily pending excuses. carefully final packages unwind idly. furiously express packa
82488	13	7142	561.75	ial escapades wake slyly carefully special requests. ironic, final 
82490	15	8788	610.31	ests sleep slyly final requests. even accounts hinder. blithely ironic accounts sleep regular, final requests. slyly express requests are blithely blithely
82495	20	1852	152.74	efully pending asymptotes. carefully ironic theodolites haggle according to the blithely bold foxes. carefully special dependencies solve. furiously final p
82496	21	5816	933.62	ve the furiously silent foxes. final, express frays around the regular dependencies dazzle slyly ca
82497	22	4941	975.57	thely ironic accounts. regular, final deposits cajole fluffily pending accounts. quickly permanent somas nag fi
82498	23	7043	707.68	ckly above the fluffily silent courts. carefully unusual deposi
82499	24	9534	981.50	eas are according to the deposits! carefully special pinto beans sleep according to the requests. carefully express theodolites according to the careful
82503	28	5619	628.94	final accounts haggle furiously final, pending theodolites? slyly ironic packages detect furiously against the blithely ruthless asymptotes. carefully ironic dolphins ca
82504	29	6521	375.28	 unwind. ironic, ironic packages cajole final foxes: final packages cajole furiously. slyly bold escapades sleep quickly. carefully ironic theodolites cajole furiously. carefully final reque
82508	33	2455	634.71	 slyly final packages. blithely ironic requests are quickly against the packages. requests
82509	34	3219	396.07	its sleep blithely ironic packages. final foxes haggle against the blithely regular instructions. slyly ironic accounts after the carefully pend
82512	37	3561	93.67	ounts impress furiously. slyly ironic pinto beans believe. blithely ironic accounts cajole. slyly special requests impress even ideas. quickly even foxes are slyly quietly regular asympto
82513	38	3620	854.51	ts cajole blithely. special pinto beans are furiousl
82514	39	7094	296.74	f the furiously bold platelets? even, regular instructions use excuses. unusual, bold foxes breach. regular instructions affix. packages across the ideas nod final accounts. pending, care
82518	43	7233	6.63	as sleep furiously doggedly final theodolites. furiously ironic deposits nag fluffily; express, furious asymptotes across the final, bold deposits solve pending
82519	44	6654	579.84	eep furiously regular deposits. accounts according to the asympto
82520	45	9183	285.02	 carefully express accounts. fluffily pending packages cajole. eve
82525	50	2042	833.20	bout the quickly even pinto beans. fluffily regular d
84985	2	8099	446.39	t carefully according to the slyly even accounts. fluffily ironic dolphins haggle. furiously express theodolites according to the bold packages boost slyly regular foxes. carefully ironic epitaphs 
84986	3	2073	586.09	 unusual requests are fluffily after the final asymptotes. final, regular grou
84988	5	2002	986.63	otes. fluffily final accounts haggle furiously regular requests; blithely final packages cajole carefully: blithely ironic instructions cajole carefully. iron
84992	9	6988	540.70	 ideas affix furiously carefully bold theodolites. fluffily final accounts until the pending packages are slyly quickly final foxes. furiously ironic foxes wake carefully. sly
84995	12	9257	75.65	 wake quickly special requests. idle, regular asymptotes cajole quickly even ideas. furiously bold theodolites sleep. special, silent pinto beans again
84996	13	5784	542.83	hely finally regular notornis. fluffily quiet packages promise above the slyly regular pinto beans. slyly silent sheaves are carefully eve
84998	15	734	633.74	 the even instructions. furiously regular asymptotes among the carefully regular tithes nag carefully after the carefully even requests. slyly unusual dolphins use bravely ironi
85003	20	6032	940.95	ructions. final, quiet theodolites alongside of the furiously unusual courts was slyly ironic gi
85004	21	7615	968.17	s. ruthlessly ironic deposits are carefully even foxes. closely final excuses are blithely special accounts. furiously 
85005	22	6228	793.55	 haggle furiously express deposits. carefully special packages sleep care
85006	23	8316	256.73	totes haggle furiously quickly ironic requests; furiously ironic theodolites nag slyly even ideas. carefully express
85007	24	3033	626.55	d accounts. unusual ideas haggle blithely. slyly expr
85011	28	4	628.75	uriously regular Tiresias. slyly ironic pinto beans impress blithely. fluffily regular Tiresias integrate furiously even foxes. ironic excuses around the furiously even waters are quickly
85012	29	1345	892.40	 wake sometimes final foxes. carefully final packages nod blithely bold excuses. braids sleep. quickly pending cour
85016	33	1325	551.17	, pending packages affix carefully alongside of the quickly ironic accounts
85017	34	16	342.35	kages eat after the fluffily ironic instructions: silently bold orbits wake carefully against the blithely
85020	37	1548	470.84	ithely after the deposits. carefully pending instructions use beyond the furiously final
85021	38	4445	762.77	he slyly even deposits. unusual deposits along the packages boost carefully alongside of the quick
85022	39	6193	64.73	sual, regular accounts. fluffily ironic accounts nag. excuses cajole against the ideas. carefully final asymptotes at the carefully regular deposits nag furiously furiously bold foxes. slyly ev
85026	43	5894	334.85	riously ironic ideas wake. carefully unusual platelets wake slyly. fluffily regular foxes can haggle. slyly pending instructions cajole quickly according to the blithely unusual ideas. excuses s
85027	44	654	592.12	ockey players affix furiously idle, pending pains-- regular a
85028	45	6236	193.32	ly pending platelets. furiously bold ideas detect at th
85033	50	1115	414.64	solve carefully. slyly even pinto beans mold slyly after the enticingly even asymptotes. furiously regular theodolites wake. carefully ironic requests sleep blithely! bold packages ag
87493	2	3335	255.95	to beans wake fluffily final theodolites. requests sublate fluffily ironic, special 
87494	3	2941	403.17	 use slyly regular, special dependencies. pending foxes wake above the 
87496	5	5635	624.84	ng deposits. slyly permanent requests boost slyly quickly even foxes. final, regular foxes boost slyly about the ironic, ironic asym
87500	9	3785	877.53	luffily across the quickly express instructions: quickly daring accounts poac
87513	22	9449	978.55	equests. furiously unusual decoys use carefully special, pending platelets. pinto beans wake slyly across the caref
87503	12	9498	616.38	nstructions use blithely quickly regular packages. final requests nag slyly among the sl
87504	13	4296	375.88	its wake closely across the regular dependencies. furiously special packages around the furiously pending theodolites wake blithely among the blithely even instructions. furiously brave
87506	15	7258	353.50	ts. quickly express excuses grow quickly against the furiously regular excuses. blithely final deposits haggle according to the unusual instructions. pearls are slyly fluffily special excuses. ca
87511	20	9103	65.52	sly about the carefully even requests. quickly special deposits integrat
87512	21	1069	462.19	 instructions. furiously special accounts sleep about the slyly silent theodolites. careful
87514	23	669	333.74	. slyly final requests snooze platelets. blithely even theodolites print blithely. slyly pending courts according to the foxes are furiously regular requests. slow ideas 
87515	24	7674	835.78	ost permanent courts. foxes cajole fluffily ironic asymptotes. final, even requests wake at the pending accounts. slyly final instructions about the even requests haggle blithely against the re
87519	28	7192	603.64	quickly even instructions. slyly final foxes doubt slyly? quickly silent requests should have to boost quickly against the final requests-- carefully regular deposits doze furiously. carefully
87520	29	6529	491.17	luffily unusual somas sleep slyly. ruthlessly unusual theodolites wake furiously since the packages. slyly final requests are carefully along the express theodolites! special depos
87524	33	6497	428.45	across the quickly regular ideas. quickly final accounts across the asymptotes need to wake slyly along the blithely ironic ideas. express dependencies cajole carefully carefully special packages. s
87525	34	1441	926.45	n slyly. furiously even theodolites lose according to the furiously regular a
87528	37	4260	856.67	 instructions. Tiresias cajole slyly furious dugouts. even, even asymptotes detect slyly silent theodolites. requests haggle alongside 
87529	38	3130	357.04	ke carefully bold accounts. blithely ironic deposits haggle quickly regular r
87530	39	3444	943.13	nal packages wake carefully quickly bold theodolites. slyly enticing foxes wake. carefully pending instructions use quickly special idea
87534	43	9695	27.56	ronic dolphins use. quickly bold packages are against the blithely regular instructions. fluffily final accounts nag. carefully unusual packages haggle quickly regular platelets
87535	44	1744	402.50	 according to the furiously even asymptotes. furiously special deposits serve above the furiously ironic instructions. special reque
87536	45	7245	766.78	cial requests haggle quickly: slyly pending accounts boost quickly. carefully special dolphins sl
87541	50	4516	79.78	ly fluffy theodolites. regular, bold accounts nag slyly silent
90001	2	4868	526.40	jole permanently around the slyly special theodolites. ironic, even requests are fluffily across the furiously bold
90002	3	2609	529.45	y instructions into the slyly silent theodolites haggle evenly around the quickly unusual forges. slyly pending ideas hag
90004	5	4540	774.25	silent pinto beans sleep slyly. blithely ironic requests are carefully slyly regular deposits. requests sleep furiously. slyly unusual packages cajole slyly. silently pending theo
90008	9	7322	321.89	deposits boost furiously above the slyly ironic accounts. bold deposits use blithely across the special,
90011	12	7433	325.41	es use even asymptotes. carefully enticing dugouts haggle carefully regular, ironic packages. ironic, ironic packages boost about the packages: even, regular packages 
90012	13	3984	468.19	l theodolites cajole. even, silent deposits integrate quickly among the fluffily special packages. regular, bold packages impress according to the careful packages? instruc
90014	15	2254	787.58	nal accounts kindle according to the slyly special deposits. quickly unus
90019	20	6743	616.77	endencies. slyly pending decoys nag ironically express, pending instructions. slyly unusual theodolites against the deposits impress slyly sly dependencies. slyly ironic fo
90020	21	3118	726.25	 accounts sleep bravely after the final instructions. slyly
90021	22	9579	681.31	epitaphs. furiously silent theodolites detect. requests cajole furiously above the special, pending a
90022	23	2707	293.27	egular, pending packages. even, express accounts according to the fluffily bold dolphins wake alongside of the slyly special asymptotes. ironic accounts sleep: fluffily 
90023	24	625	963.40	efully. carefully regular packages after the furiously regular instructions cajole furiously according to the express packages. quickly e
90027	28	6464	844.48	gs boost about the furiously even deposits. quickly pending requests are carefully. final patterns inside the slow, ironic pearls cajole quickly special foxes-- sp
90028	29	8957	314.92	ter the regular deposits. accounts wake across the furiously ironic ideas. slyly regular requests along the express, pending realms cajole special dependencies. deposits among 
90032	33	4621	3.42	 regular ideas. regular, final packages are slyly regular pinto beans-- bold packages above the regular packages boost carefully alon
90033	34	4271	730.32	ts. carefully regular excuses wake quickly accounts.
90036	37	7975	235.67	slyly pending foxes wake quickly after the requests. furiously special requests nag slyly above the carefully express asymptotes. final, bold acc
90037	38	9981	95.87	ng to the furiously silent packages haggle along the slyly final instructions. slyly express packages 
90038	39	2382	753.10	sits. furiously ironic theodolites sleep slyly regular excuses. ironic requests poach slyly after the requests. slyly special as
90042	43	505	160.40	er the furiously unusual deposits. pending deposits wake permanently carefully final pearls. furiously ironic instructions nag. reg
90043	44	4773	175.74	egular dependencies haggle ironic deposits. regular theodolites sleep instead of the carefully brave dependencies. final, express accounts wake. caref
90044	45	6436	614.94	ic platelets. slyly regular asymptotes use blithely final foxes. unusual accounts boost near the furiously regular dolphins. foxes along the accounts sleep carefully af
90049	50	7542	170.36	s. evenly final accounts nod carefully regular requests. blithely even packages 
92474	2	515	519.24	uests about the dependencies cajole Tiresias-- blithely ironic ideas use quickly unusual foxes. quickly even packages wake carefully special ideas. blithely final instructions sle
92475	3	9269	703.17	p blithely among the patterns. slyly pending pinto beans cajole enticingly even, express accounts. final theodolites use fluffily asymptotes. furiously express accounts outside the quickly pendin
92477	5	9415	13.44	 sleep slow, special deposits. regular asymptotes affix fluffily after the quickly final deposits. furiously pending ideas integrate across the closely regular excus
92481	9	5431	703.81	cross the blithely ironic asymptotes impress furiously blithely final foxes. ironic deposits among the accounts boost furiously express excuses; 
92484	12	8877	470.72	e! carefully regular packages haggle slyly along the fluffily unusual excuses. even packages cajole carefully. blithely pending instructions hinder blithely. special accounts wake slyly aroun
92485	13	6275	102.07	c asymptotes. ironic requests wake blithely fluffily unusual realms. pending theodolites nag fluffily ironic instructions. even dependencies are fluffily. carefully express platelet
92487	15	3830	11.05	requests sleep blithely. carefully regular platelets wake fluffily. quickly final instructions wake deposits. ironic, regular requests boo
92492	20	1876	23.00	 regular, pending instructions. deposits alongside of the carefully special de
92493	21	7123	142.81	about the furiously regular requests cajole furious
92494	22	5726	212.24	e ironic packages. blithely pending accounts wake quickly ideas. fluffily 
92495	23	9249	358.35	eans are. blithely final foxes shall have to are. quietly even instructions around
92496	24	3569	268.48	e final packages wake quickly unusual deposits. sp
92500	28	8361	880.81	uriously ironic courts are slyly. carefully pending ideas cajole. bold excuses haggle. slyly furious asymptotes serve. special accounts are quickly furiously ironic
92501	29	41	402.59	bold packages above the deposits wake furiously quickly careful accounts. ironic, regular asymptotes across the foxes sleep blithely pe
92505	33	2361	657.04	 haggle fluffily-- platelets engage blithely. furiously regular asymptotes wake quickly somas. special, fluffy excuses affix carefully. furiously special accounts haggle furiously. even, ironic 
92506	34	6866	252.22	ost. final, special theodolites against the slyly ironic accounts use
92509	37	7738	656.47	r accounts. furiously final requests detect quickly. carefully bold requests use never against the fluffily even packages. bold packages are after the final, regular accounts. carefully
92510	38	638	166.80	bout the regular tithes! quickly special asymptotes wake carefully. slyly regular requests nag among the asymptotes. slyly pending accounts cajole about the furiously unusual hockey players. q
92511	39	1354	350.35	furiously according to the unusual requests: quickly unusual requests integrate final, final accounts. ironic, iro
92515	43	3459	22.19	 blithely regular excuses. ironic instructions sleep quickly alongside of the express, darin
92516	44	9720	823.44	ly. regular deposits cajole about the furiously special excuses. quickly final excuses cajole; blithely furious requests against the ironic, special requests hang fluffily across the fluffily pen
92517	45	759	199.63	ecial foxes against the sometimes ironic ideas haggle across 
92522	50	6009	906.10	ess waters. furiously regular accounts among the quickly ironic platel
94983	2	3535	622.84	tes. regular, ironic deposits cajole according to the fluffily express accounts. requests are after the accounts. p
94984	3	1591	627.44	he furiously ironic ideas. regular, unusual dependencies wake blithely above the blithely regular accounts. furiously special orbits 
94986	5	770	915.78	uld have to wake quickly before the furiously even pinto beans! quickly ironic accounts b
94990	9	7308	414.90	ding to the quickly regular dependencies use slyl
94993	12	3460	141.77	cuses. even packages cajole furiously along the carefully ironic requests-- 
94994	13	5242	491.21	se slyly regular accounts. carefully even pinto beans sno
94996	15	5960	201.54	l pinto beans. requests wake slyly among the blithely regular forges. stealthily even requests boost ironically furiously final accounts. quickly ironic deposits do was according to the silent accou
95001	20	3712	200.51	p blithely slyly final foxes. furious asymptotes sleep slyly. furiously unusual 
95002	21	8405	453.06	beans integrate along the bold packages-- carefully ironic deposits after the blithely express requests are fluffily quickly silent platele
95003	22	1575	522.30	express deposits wake slyly quickly unusual accounts. slyly even ideas dazzle bl
95004	23	9359	781.25	y unusual packages haggle slyly slyly bold foxes. final, pending ideas are blithely. fluffily final deposits above the regular, regular pains sleep quickly ironic excuses. slyly
95005	24	827	968.32	heodolites for the slyly pending theodolites sleep across the slyly pending asymptotes. final, special ideas hang. carefully final platelets wake daringly above t
95009	28	1906	512.05	ial theodolites. dependencies nag blithely silent requests. packages cajole instead of the deposits. regular instructions according to the blithely even warthogs x-ray carefully against the express 
95010	29	7869	728.72	ending warthogs engage quickly against the furiously unusual warthogs. bold packages alongside of the dolphins haggle blithely even, pending deposits. regular, express ideas ag
95014	33	182	134.36	riously regular platelets are quickly even accounts. multipliers about the fluffily final deposits wake furiously against the
95015	34	2240	46.40	uests cajole carefully unusual, even asymptotes. even accounts cajole sometimes pinto beans. sly
95018	37	5677	48.99	arhorses wake excuses. quickly ironic accounts affix slyly at the carefully final packages. bold, final ideas mold. theodolites haggle accord
95019	38	6358	964.25	 above the pearls. ironic packages are. slyly express ideas use carefully unusual deposits. quickly silent accounts haggle carefully furiously special accounts. ca
95020	39	8051	121.84	 according to the carefully silent dinos. quickly pending foxes boost about the 
95024	43	4086	889.93	. pending platelets wake fluffily. quickly ironic instructions use permanently around the bold deposits. regular pinto beans are blithely furiously express foxes. fluffily bold packages are
95025	44	9009	269.17	. pinto beans doze furiously. requests cajole slyly. special deposits at the blithely ironic instructions wake slyly silent ideas. furiously final 
95026	45	681	702.99	lar dolphins after the carefully ironic foxes can integrate express deposits. furiously regular deposits kindle carefully blithe gifts. quickly final packages are caref
95031	50	2014	197.35	sts. regular requests cajole busily ironic requests. express, regular deposits could cajole. pending, even dependencies haggle blithely. pending deposits boost. slyly ex
97492	2	2489	982.19	ke carefully about the always ironic foxes. ironic packages atop the furiously special dolphins affix after the sl
97493	3	257	837.54	ts are slyly quick requests. carefully regular deposits cajole carefully after the carefully ironic foxes. 
97495	5	3379	553.79	fily even theodolites. carefully sly requests wake. quickly pending acc
97499	9	9469	919.71	ld platelets! slyly regular packages sleep silently alo
97512	22	5766	31.85	ts. furiously pending accounts across the carefully pend
97502	12	7518	372.87	ay: slyly even depths across the furiously ironic foxes haggle furiously along the regular packages. even, sil
97503	13	4830	140.48	lly final accounts are blithely among the carefully even packages. quickly unusual requests wake quickly final packages. q
97505	15	2526	410.03	 serve. ideas sleep carefully furiously pending foxes? requests sleep alongside of the fluffily pending deposits. regular, even deposits wake blithely across the
97510	20	7850	493.33	unts affix carefully. instructions wake slyly permanent accounts
97511	21	6781	839.71	ven grouches across the even packages wake quickly ironic deposits. blithely regular pinto beans use fluffily blithely unusual ideas. fluffily bold ideas boost
97513	23	7525	711.68	fluffily regular dependencies. ironic, bold accounts cajole express request
97514	24	2248	896.32	gular packages. furiously final instructions according to the quickly permanent theodolites nag blithely daring platelets. final, ironic frays cajole quickly
97518	28	8474	401.80	nag blithely above the blithely regular foxes. slyly pending foxes breach blithel
97519	29	3556	418.97	ly special packages. slyly final dependencies boost carefully about the pending, special deposits; slyly enticing accounts among the ste
97523	33	6265	452.63	ag fluffily across the asymptotes. quickly regular instructions sleep pending, e
97524	34	6349	754.92	regular foxes breach carefully after the even, brave accounts; quickly ironic accounts haggle boldly. bravely even deposits doze furiously among the regular, bold packag
97527	37	2579	81.46	around the final packages wake slyly after the quickly final accounts. requests wake fluffily idle ideas. regular deposits along the even, bold instructions cajole furiously among the carefull
97528	38	6875	883.42	refully silent accounts grow. slyly thin packages are never along the special pinto beans. furiously fina
97529	39	9227	896.59	e slow, unusual requests. ironic, final somas sleep furiously pending, ironic packages. carefully regular accounts boost blithely a
97533	43	8166	940.57	ietly unusual deposits around the slyly silent dolph
97534	44	408	154.64	 pinto beans integrate slyly fluffily thin deposits. slyly bold requests according to the carefully bold deposits sleep around the slyly regular theodolites. fluffily regular packages alo
97535	45	3492	78.39	ular platelets wake blithely blithely special ideas. deposits haggle iro
97540	50	4847	997.78	y excuses wake. slyly pending deposits use around the pinto beans
100001	2	9505	291.25	ronic accounts cajole. ironic theodolites haggle quickly throughout the instructions; blithely silent dependencies haggle. blithely ironic foxes accordin
100002	3	9248	228.39	nusual accounts beyond the quickly unusual packages haggle furiously special courts. blithely pending accounts alongside of the final pa
100004	5	4556	749.70	dencies against the requests integrate slyly carefully regular packages. packages among the foxes cajole after the furiously express a
100008	9	4181	605.59	xcuses wake around the quickly special instructions. pending deposits print silently. idly regular packages cajole fl
100011	12	3213	256.36	 deposits boost furiously above the slyly pending requests. pearls about the regular th
100012	13	2918	462.49	g the bold accounts. quickly even accounts nag furiously; furiously regular requests boost ac
100014	15	9683	407.41	nding instructions sublate slyly across the ironic 
100019	20	1028	476.91	ly busy instructions. blithely final requests sleep slyly. carefully silent deposits wake theodolites. foxes nag ruthlessly among the furiously even packages. furiously silent reques
100020	21	6283	58.68	 blithely final ideas. thin accounts integrate carefully alongside of the slyly express pinto beans. unusual requests cajole: final requests across the unusual grou
100021	22	2889	204.65	 above the furiously permanent accounts affix carefully across the ironic dep
100022	23	8434	255.04	usual deposits? blithely regular requests integrate
100023	24	4481	109.66	to beans wake carefully pending requests: fluffily even packages are blithely. special acco
100027	28	4673	279.56	ly furiously even ideas. blithely express pinto beans are ac
100028	29	4048	534.06	luffily about the carefully final ideas! ironic escapades are! furiously final deposits nag carefully above the regular packages. blithe
100032	33	1043	801.49	hely pending dependencies? carefully special requests affix slyly quickly sp
100033	34	5216	153.82	y. furiously ironic deposits doubt quickly among the blithely bold packages. special foxes must cajole about the fluffily final packages. slyly final frays toward the dolphins are quickly agains
100036	37	1955	902.11	es alongside of the carefully special theodolites wake blithely above the requests. carefully regular p
100037	38	1537	880.11	the quickly silent theodolites. furiously bold accounts integrate. furiously even patterns run fur
100038	39	2382	982.11	y along the silently ironic packages. packages are slyly thin, final requests. spe
100042	43	894	744.49	ts. blithely express deposits alongside of the blithely silent deposits sleep slyly above the furiously final requests. final requests ar
100043	44	4476	320.48	ons are slyly escapades. slyly final pinto beans use. furiously dogged packages detect slyly after the deposits. accounts are. blithely s
100044	45	8238	695.48	iously ironic sheaves boost. stealthily unusual accounts sleep fluffily. bold ideas use slyly. furiously unusual requests cajole. quickly daring dependencies among the
100049	50	1714	119.94	the ironic foxes. blithely ironic requests integrate furiously amon
102471	2	6209	778.65	into beans nag blithely above the furiously even packages
102472	3	2368	172.31	re carefully regular instructions. blithely pending the
102474	5	4724	727.27	s use furiously special instructions. slyly even packages haggle blithely across the slyly bold deposits. quickly even asymptotes haggle furiously-- ironic theodolites ab
102478	9	1710	686.91	latelets against the stealthy foxes wake furiously along the unusual, even foxes. special accounts haggle: blithely furious requests doubt fluffily across the fluf
102481	12	6582	972.70	ajole slyly furiously bold packages. carefully regular theodolites sleep slyly except the accounts. ironic, express accounts about t
102482	13	7048	903.28	es nag pending ideas. deposits x-ray bravely ironic, express packages. slyly bold 
102484	15	5212	586.23	blithely silent packages may wake quickly after the ironic accounts. slyly dogged platelets might sleep blithely
102489	20	5094	364.45	unts sleep carefully. carefully regular theodolites along the slyly pending requests cajole carefully slow braids! pa
102490	21	4384	439.66	s affix carefully even foxes. bold, express requests haggle sometimes after the even, special asymptotes: slyly final requests play blithely carefully pending packages. carefull
102491	22	5428	172.77	ously about the pending accounts. slyly even theodolites might affix. even, regular requests around t
102492	23	1614	785.89	riously according to the carefully ironic platelets. quickly spe
102493	24	5254	111.95	ns alongside of the special, thin packages wake carefully along the carefully iro
102497	28	2046	289.76	 of the silent pinto beans cajole quickly express, regular packages. pending pearls doubt above the carefully final requests. blithely even instructions 
102498	29	946	203.40	 deposits are. even packages wake carefully across the blit
102502	33	87	908.01	d, even packages nag blithely at the slyly final requests. furiously ironic accounts sleep. slowly express theodolites ca
102503	34	8061	758.39	nic requests haggle furiously across the slyly regular requests. carefully regular instructions wake evenly alongside of the blithely unusual deposits. furiously regular p
102506	37	1117	956.03	nto beans among the final, special deposits wake final requests. furiously regular requests after t
102507	38	543	205.19	 regular pinto beans. quickly ironic instructions affix. carefully final accounts above the express accounts serve slyly quickly bold excuses. bold, even excuses cajole furi
102508	39	5977	385.65	ests are after the furiously final packages. dugouts haggle carefully. ironic platelets use beside the furiously final requests. accounts will haggle
102512	43	4856	478.08	are. final, even requests among the blithely ironic pinto bea
102513	44	9613	766.40	lly silent courts are furiously. silent instructions cajole carefully against the blithely even deposits. quick
102514	45	4084	712.79	 slyly ironic deposits detect blithely. theodolites sleep slyly. furiously regular ideas wake. carefully unusual dependencies haggle. fluffily pending forges use. regular, pending pi
102519	50	717	667.11	e even instructions. regular requests cajole. furiously unusual asymptotes are carefully carefully silent theodolites. ideas affix carefully. fluffily 
104981	2	8656	438.45	fix slyly above the slyly regular packages. furiously pending deposits poach blithely furiously pend
104982	3	1642	228.98	the regular pinto beans. fluffily pending theodolites are ideas. slyly even excuses according to the furiously pending accounts are slyly somas. pending, even depo
104984	5	6832	601.56	 around the furiously regular somas sleep carefully about the silently final dependencies. regular, unusual request
104988	9	1197	296.51	ccounts cajole slyly about the final deposits. careful, express deposits along the
104991	12	1643	168.25	 excuses cajole above the even pinto beans. quickly express requests sleep blithely blithely final dolphins. final foxes boost slyly according t
104992	13	5741	357.20	e furiously along the regular requests. silent, ironic packages affix enticingly above the silent dependencies. pending deposits integrate around the furio
104994	15	7517	280.34	nal ideas. quick pinto beans cajole carefully across the furiously regular accounts. carefully ironic theodolites sleep according to the reg
104999	20	6210	639.66	 ironic packages haggle about the bold requests. final deposits cajole quickly along the slyly expre
105000	21	1203	727.58	sts wake slyly. furiously pending instructions about the slyly even requests believe alongside of the slyly regular accounts. fluffi
105001	22	3318	564.20	ions affix blithely along the foxes. blithely regular requests are doggedly silent asymptotes. slyly ironic theodolites integrate 
105002	23	7974	119.89	efully pending requests are furiously-- quickly final accounts use carefully ironic, special epitaphs. pack
105003	24	4403	73.86	deas. slyly regular deposits wake among the furiously regular instructions.
105007	28	4740	793.19	endencies. silent asymptotes breach furiously after the blithely regular ideas. ironic acco
105008	29	5815	611.13	ncies haggle slyly carefully bold pinto beans. regular requests wake. regular foxes sleep. fina
105012	33	3670	796.15	nal, ironic ideas. excuses are always. sometimes special instructions boost along the quickly pending instructions. unusual decoys nag slyly after the furiously regular reques
105013	34	8368	502.09	about the express, even requests cajole even ideas. blithely bold ideas wake blithely regular deposits. foxes about the blithely even pains use ar
105016	37	5490	666.27	e slyly even requests. carefully pending ideas nag. slyly ironic pinto beans enga
105017	38	7766	986.03	uickly alongside of the ironic attainments. quickly bold theodolites would use. enticingly unusual requests haggle. furiously bold pinto beans use blithely along the
105018	39	3886	677.56	le. accounts nod across the blithely final deposits. carefully express packages wake quickly. carefully final foxes wak
105022	43	8004	539.05	ronic requests according to the ruthlessly bold escapades boost quickly carefully dogged packages. fluffily even packages about the regular theodolites hag
105023	44	37	762.77	ully slyly regular foxes. carefully pending pains according to the fluffily regular instructions wake carefully after the re
105024	45	735	388.74	ss, ironic tithes. quickly ironic packages run blithely; blithely close foxes x-ray alongside of t
105029	50	312	640.63	uests. blithely regular requests sleep. blithely pending deposits a
107491	2	2450	973.00	vely silent instructions affix. slyly final attainments wake. bold foxes cajole along the final, special requests. special, special excuses haggle
107492	3	7593	470.06	ccounts are furiously bold theodolites-- ironic dolphins sle
107494	5	5131	64.64	longside of the final, even packages? ironic frays sleep. express dugouts nag. carefully bold packages against the regular requests detect a
107498	9	4304	539.32	thely special requests. furiously daring deposits cajole carefully even foxes. blithely regular requests wake pending, final asymptotes. packages after the deposits use slyly above the carefull
107501	12	8938	181.79	ely special requests wake. foxes integrate quickly beyo
107502	13	8796	711.66	y bold deposits around the ironic requests boost among the blithely ironic deposits. furiously final pinto beans haggle furiously. blithely regular packages wake slyly after the fluffily regul
107504	15	7425	933.73	unts haggle slyly slyly even packages. quickly bold dependencies sleep pending instructions. slyly unusual requests among the pending, final ideas integrate furiously careful
107509	20	1240	86.93	kly bold asymptotes after the unusual somas integrate slyly ironic courts. furiously ironic deposits are slyly express pinto beans. spe
107510	21	8778	285.98	ending dolphins; regular packages haggle fluffily regular deposits. furiously ironic requests nag fluffily fluffily ironic packa
107511	22	3637	280.35	y. deposits detect furiously carefully blithe requests. slyly even Tiresias after the furiously ironic requests nag according to the even packages. special theodolites above the thin accounts 
107512	23	5410	123.34	 pinto beans cajole across the furiously regular packages. blithely sly ideas sleep along the carefully ironic deposits. quickly ironic requests detect above the furiously final waters. blithe
107513	24	190	872.15	er. express deposits shall boost slyly according to the regular, unusual deposits. slyly regular accounts cajole carefu
107517	28	1483	670.75	boost among the unusual, even accounts. quickly unusual deposits are blithely across the blithely bold deposits. foxes thrash blithely against the fo
107518	29	7172	597.31	s cajole carefully slyly final platelets. ironic, 
107522	33	1266	510.97	mong the deposits. final, unusual packages against the bold deposits sleep slyly ironic, ironic deposits. carefully unusual instructions boost quickly regular re
107523	34	1243	103.74	eans. blithely pending dependencies are carefully slyly even packages. express, pending accounts integrate alongside of the blithely ironic excuses-- r
107526	37	7084	389.38	ickly ironic packages are furiously around the regular deposits. quickly final asymptotes cajole carefully final dependencies. regular foxes boost
107527	38	8740	169.86	nd the pending ideas. ruthlessly regular dugouts was slyly! furiously express foxes are slyly ironic deposits. blithely ironic accounts are slyly fluffily final packages. furi
107528	39	5098	172.10	onic accounts play until the unusual, express requests. slyly r
107532	43	9092	771.51	le furiously final requests. slyly bold asymptotes haggle. final, final theodolites against the slyly express inst
112509	43	3788	350.68	ly bold gifts are. final, regular deposits lose regular deposits. slyly regular ideas wake expres
107533	44	6610	444.96	onic theodolites sublate. multipliers around the unusual, ironic deposits serve final, ironic packages. carefully regular accounts doubt ruthless
107534	45	6777	725.18	hockey players. express packages haggle furiously permanently regular req
107539	50	4464	381.41	nts across the furiously express accounts believe quickly bli
110001	2	6088	336.90	side the carefully final packages. slyly bold instructions b
110002	3	779	993.23	quests wake slyly fluffily silent accounts. bold excuses haggle final foxes. dugouts after the quickly final packages are fluffily slyly unusual pinto be
110004	5	7563	652.30	he carefully even deposits. furiously regular requests integrate carefully carefully final packages. regular accounts are slyly caref
110008	9	4047	299.11	 nag quickly across the final ideas. special theodolites haggle fluffily according to the furiously final pinto beans. bold theodoli
110011	12	8182	912.58	inder about the quickly bold requests. carefully final idea
110012	13	3389	628.59	lyly. slyly ironic pinto beans wake above the slyly final pinto beans. asymptotes are among the furiously ironic deposits. fur
110014	15	6271	322.86	nticing accounts. final, silent deposits haggle. fluffily special deposits haggle carefully above the packages. slyly reg
110019	20	6855	502.14	uickly. blithely bold requests haggle alongside of the silent asymptotes. slyly regular foxes are blithely. fluffily ironic dependencies haggle blithely spec
110020	21	8070	52.46	mong the blithely ironic packages wake according to the carefully even deposits. special, even notornis s
110021	22	2143	192.02	s try to are fluffily final patterns. special foxes boost carefully. carefully regular requests haggle into the accounts. regular accounts about the theodolites cajole about the expre
110022	23	4453	374.06	s. pending foxes cajole: pending Tiresias wake furiously. blithely silent ideas wake furiously. ideas cajole across the blithely ironic ideas. express, regul
110023	24	5347	397.94	haggle over the furiously unusual dolphins! furiously express theodolites are furiously furiously ironic theodolites. quickly ironic instruc
110027	28	8999	612.91	 haggle about the blithely special realms. slyly regular deposits above the carefully special packages are furiously regular accounts. careful, express dolphi
110028	29	3876	44.41	 bold instructions. fluffily ironic dependencies integrate blithely. silently even asymptotes until the furiously ironic requests 
110032	33	4538	971.47	ular packages. blithely regular packages sleep. quickly
110033	34	9512	394.90	accounts use doggedly. carefully brave frets nag blithely. furiously unusual asymptotes doubt quickly furiously pending deposits. carefully ironic tithes sleep ruthlessly furiously regular requests.
110036	37	2035	250.27	usual foxes use furiously along the dugouts? furiously pending dependencies nag slyly after the carefully sly 
110037	38	8623	456.90	ecial platelets. bold theodolites among the final packages sleep after the carefully bold pinto beans. blithely even platelets wake furiously-- pending pinto be
110038	39	6797	203.03	beans. excuses sleep carefully. furiously silent requests above the platelet
110042	43	7980	770.68	 the furiously special instructions. fluffily final deposits hinder carefully along the slyly pending pinto beans. carefully regular theodolites above the permanent, pending deposits
110043	44	2741	298.03	aggle furiously slyly bold requests. foxes boost blithely carefully final dependencies. pending inst
110044	45	5838	68.06	riously final packages across the furiously regular pinto beans use blithely ironic deposits. furiously 
110049	50	9292	496.30	wly even requests wake slow theodolites. blithely final deposits around the fluffily pending theodolites believe furiously furiously even foxes. quickly pending packages after the bl
112468	2	8025	998.94	 final requests. silently regular accounts against the slyly quiet accounts haggle furiously 
112469	3	2101	993.88	notornis play blithely. regular, special instructions use quickly even deposits. regular, ironic accounts poach slyly unusual packages. regular excu
112471	5	8656	516.38	ructions play slyly ironic packages. packages about the ironic accounts affix furiously across the special pains. carefully regular deposits wake blithely pending frets. final, f
112475	9	2295	949.27	equests. furiously regular foxes nag during the quiet, pending deposits. final deposits sleep doggedly. enticingly ev
112478	12	3036	428.14	lar deposits nag fluffily regular foxes. quickly ironic deposits sleep furiously. regular accounts wake carefully. quickly express dolphins haggle-- furiously pending theo
112479	13	604	52.94	accounts x-ray furiously requests. regular, bold deposits across the quickly silent requests nag carefully across the final, final theodolites. ironic, even requests are closely; fu
112481	15	1429	579.84	unts. furiously unusual packages wake slyly quickly final pinto beans
112486	20	2460	96.84	carefully final accounts. final excuses are. regular, express pinto beans wake among the carefully regular instructions. unusual, special pearls are furiously about the 
112487	21	5297	156.07	ges. even, close requests try to are. carefully final theodolites against the even, ironic pinto beans cajole a
112488	22	9984	796.24	 regular sheaves. furiously ironic packages haggle requests. blithely
112489	23	7113	50.17	blithely express ideas haggle slyly quickly busy a
112490	24	3875	710.28	sual, final accounts. carefully even ideas affix. blithely final instructions haggle slyly around the blithely bold foxes. express attainments boost carefully according to the sentiments. ideas kin
112494	28	5267	879.55	uests according to the carefully even sentiments print quickly at the slyly ironic instructions. ironic deposits haggle. deposits along the special packages haggle furiously among the carefull
112495	29	6663	793.94	zzle. instructions haggle carefully. unusual, silent pinto beans solve bravely unusual, regular requests. special accounts sleep fluffily quickly even theodolites. pending depos
112499	33	7561	734.42	ses haggle fluffily. packages integrate after the slyly 
112500	34	175	289.78	leep blithely. even orbits sleep quickly courts. slyly bold instructions doubt. fluffily final i
112503	37	8035	109.88	atelets. quickly special accounts cajole carefully after the fluffily regular packages. slyly regular waters haggle slyly slyly pending pinto b
112504	38	5880	292.23	fluffily. quickly ironic accounts nag slyly ironic, pending requests. slyly regular theodolites unwind pinto beans. pending dinos nag furiously.
112505	39	7360	227.86	es. furiously silent deposits haggle quickly bold packages. slyly even i
112510	44	4060	423.85	al ideas. unusual, express deposits according to the quickly silent ideas poach quickly express requests. carefully express packages slee
112511	45	5906	516.24	equests along the bravely bold foxes will have to nag carefully fluffy dependencies. carefully regular sheaves integrate furiously across the quickly ironic o
112516	50	6497	11.80	int blithely regular depths. silent dinos haggle. slyly b
114979	2	5318	543.98	regular requests use carefully regular accounts. even deposits affix fluffily special deposit
114980	3	5907	993.22	es haggle furiously unusual requests. blithely final accounts wake. dependencies need to haggle 
114982	5	1990	619.91	osits. ironic pains boost ironically. sly requests haggle furiously. sly pinto beans use. ironic, final requests wak
114986	9	3320	47.94	 requests nag furiously: quickly ironic ideas cajole quickly. furiously bold instructions doubt furiously after the slyly bold foxes: carefu
114989	12	4080	812.27	y final foxes use carefully even pinto beans. pending,
114990	13	9685	698.01	tructions use carefully. regular excuses sleep slyly. dogged packages alongside of the packages wake blithely furiously final accounts. slyly even ideas detect above the 
114992	15	5893	110.79	theodolites are carefully beneath the slyly pending asymptotes. quietly regular tithes sleep slyly across the quickly final ideas. regular, bo
114997	20	8759	534.91	inal packages nag blithely alongside of the blithely final platelets. fluffily e
114998	21	9653	936.70	ts are blithely carefully ironic asymptotes. silent ideas cajole carefully.
114999	22	8008	339.65	regular instructions by the carefully special dependencies s
115000	23	8020	230.35	e blithely. even instructions cajole. quickly pending requests detect above the blith
115001	24	9028	61.59	regular instructions. foxes affix boldly. ironic, special packages are special, regular ideas. even deposits among the theodolites sleep blithely final theodolites. regular
115005	28	8517	686.39	r, regular deposits. regular, ironic ideas dazzle slyly above the pending, bold dependencies. blithely even packages nag quickly furiously unusual platelets. car
115006	29	4173	411.17	nic warhorses are. furiously ironic packages are express esc
115010	33	8245	550.85	s accounts sublate carefully above the blithely final pinto beans. carefully final deposits for the pend
115011	34	8721	948.85	y ironic, final accounts. carefully ironic accounts are furiously. even deposits mold above the carefully special instructions; daringly regular
115014	37	3627	727.19	. carefully regular accounts cajole slyly. quietly ironic deposits cajole blithely ironic accounts. quickly unusual account
115015	38	2548	652.85	ons cajole across the blithely even requests. blithely ironic deposits cajole slyly furiously regular accounts: furiously special accounts are. instructions against the unusual packages nag blithel
115016	39	6767	524.93	s use about the slyly regular platelets. slyly final p
115020	43	2659	493.57	lent deposits are blithely. unusual, ironic deposits above t
115021	44	7257	795.18	ut the furiously bold deposits. regular accounts haggle deposits. slyly special re
115022	45	4037	358.98	beans. final deposits above the carefully regular requests sleep after the quickly regular deposi
115027	50	312	77.83	oxes use furiously across the bold deposits. ironic deposits haggle above the special accounts
117497	9	4102	860.75	always bold foxes haggle carefully. express deposits will wake furiousl
117490	2	4810	409.53	usly across the unusual, ironic dolphins. carefully ironic excuses are quickly slyly express theodolites. thinly ironic ideas cajole quickly about the blithely regular accounts. bold sentiments ar
117491	3	6228	779.75	osits haggle. thinly regular ideas sleep blithely around the even, even packages. final, final accounts cajole care
117493	5	4655	244.20	 use carefully beside the even theodolites. Tiresias about the furiously pending 
117500	12	4260	652.33	ts along the regular foxes wake furiously about the quickly 
117501	13	1724	856.56	d packages. unusual deposits impress. final requests among the slyly ironic accounts nag along the carefully pending packages. even accounts sublate slyly ironic asymptotes. silent deposits w
117503	15	6356	799.93	ons use carefully according to the silent courts. even, regular requests 
117508	20	5384	462.60	quests haggle slyly even requests. sometimes regular ideas cajole. carefully ironic fox
117509	21	4937	875.82	deposits maintain carefully along the deposits. quickly ironic ideas nag slyly across the quickly regular dependencies. regular, final deposits integrate blithely. slyly unusual requests ma
117510	22	5944	101.70	l deposits. fluffily unusual waters boost slyly furiously regular frets. furiously special deposits integrate above the blithely special ideas. platelets sleep quickly since the carefully regular ex
117511	23	7837	144.05	 furiously express accounts against the evenly final accounts sleep alongside of the carefully bold deposits. 
117512	24	2251	588.95	out the furiously final foxes haggle slyly blithely regular foxes. furiously regular deposits promise. quickly even gifts
117516	28	194	666.34	ly final theodolites nag slyly furiously express deposits. ironic somas boost careful
117517	29	7169	950.60	orges nag furiously carefully pending pinto beans. furiously special excuses kindle blithel
117521	33	273	588.31	ding to the furiously unusual accounts solve about the ironic, regular accounts. fluffily
117522	34	7022	863.38	egular attainments haggle furiously across the pinto beans. bold, regular instruct
117525	37	9667	104.07	uffily final ideas. express packages detect carefully final requests. slyly ironic foxes nag quickly 
117526	38	8881	645.94	ar requests boost furiously. fluffily even requests maintain slyly ideas. slyly final accoun
117527	39	6174	849.73	 pinto beans. carefully special accounts use. final requests around
117531	43	935	644.24	slyly fluffily unusual theodolites? regular packages wake above the carefully even ideas. quickly
117532	44	8071	745.26	efully ironic packages. doggedly regular deposits haggle doggedly above the bold deposits. final accounts sleep quickly sly accounts? pendi
117533	45	6183	353.47	e the furiously close forges. regular, final pinto beans cajole regularly permanent, dogged deposits. slyly pending ideas according to the special, even instructions use furious
117538	50	2218	122.76	 blithely ironic pinto beans. slyly ironic deposits nag quickly special accounts. slyly ironic courts through the quickly pending package
120001	2	8135	544.90	unts. deposits according to the silent, bold instructions boost slyly among the final, final foxes. slyly ironic deposits sleep carefully unusual foxes. even instructions ar
120002	3	1137	112.24	along the quickly express deposits are furiously against the fluffily bold forges. unusual courts after the
120004	5	3919	430.06	carefully pending packages above the slyly final asymptotes cajole alongside of the carefully silent frays. silent, ironic acco
120008	9	1248	319.64	 boost busily regular ideas. carefully pending ideas doubt slyly beyond the courts. ironic, quiet packages boost carefully against the blithely even p
120011	12	5359	556.25	are about the stealthy excuses. unusual, final theodolites promise blithely about the quickly ironic dep
120012	13	1055	310.38	 final platelets. furiously regular instructions against the carefully ironic warthogs cajole 
120014	15	6826	774.13	. ironic ideas are ironically ruthless foxes. idle requests nag quickly among the slyly bol
120019	20	5552	92.33	 theodolites use ironic packages. carefully regul
120020	21	3018	197.43	iously final ideas to the blithely express accounts cajole furiously above 
120021	22	8588	440.47	pecial accounts. fluffily express requests are quickly above the carefully ironic ideas. furiously even requests serve even accounts. quickly final platelets us
120022	23	5925	484.51	rding to the blithely ironic dependencies. theodolites play furiously according to the furiously regular ideas. special packages are slyly ironic accounts. brave realm
120023	24	2952	469.63	bravely requests. even dugouts integrate across the deposits. express packages ought to haggle along the carefully unusual theodolites. f
120027	28	707	375.80	ithely according to the packages. slyly silent deposits nag carefully. pending, final accounts along the requests sleep 
120028	29	6947	960.24	beans. deposits haggle fluffily on the carefully ironic packages; slyly regular deposits integrate blithely slyly quiet deposits. final instructions mold at the blithely even dolphins. quic
120032	33	7950	597.42	es nag pending excuses. pinto beans snooze carefully alongside of the platelets. unusual deposits nag slyly regular ideas. ideas print quickly. evenly unusual packages nag quickly. pending, regular 
120033	34	5544	667.67	y. pending foxes cajole above the ironic frets. fluffily even deposits nag furiously according t
120036	37	8885	335.50	s. regular, regular foxes wake carefully according to the carefully unusual deposits. fluffily ironic ideas haggle after the regular, final requests. bold pinto 
120037	38	8107	301.62	 blithely final, ironic platelets. blithely silent dolphins cajole even, final foxes. quickly regular platelets nag furiously. slyly special instructions nag furiously silent dolphin
120038	39	8730	713.29	 packages sleep permanently alongside of the warhorses. even, ex
120042	43	7970	570.91	y across the theodolites. slyly pending notornis are
120043	44	5412	344.02	fully regular accounts affix blithely among the theodolites. carefully ironic sentiments alongside of the slyly bold sauternes are blithely sl
120044	45	4745	651.58	fix blithely ironically quick foxes! carefully even dependencies wake blithely bold acc
120049	50	2024	870.37	ounts-- blithely even pinto beans maintain instead of the furiously fin
122465	2	7817	987.38	ily bold instructions. ruthlessly regular platelets are quickly carefully regular ideas-- furiously regular Tiresias nag furiously bold ac
122466	3	1665	258.68	cording to the slyly final packages. blithely bold foxes are furiously ironic pinto beans. carefully special packages run. packages are carefully alongside of the regular, final d
122468	5	77	399.36	y pending sheaves play. even, express requests cajole across the express dependencies. ironic accounts are furiously ironic, regular ideas. carefully regular inst
122472	9	9998	748.91	blithely unusual pinto beans detect carefully pending foxes. idle, even accounts after the blithely regular pinto beans detect about the furiously special d
122475	12	5142	658.55	ly furiously silent requests. even packages haggle carefully. blithely idle instructions against the blithely p
122476	13	2376	223.34	y express requests are. pending, pending asymptotes wake evenly among the blithely ironic courts. carefully regular accounts nag carefully. fluffily ironic ideas boost after the sl
122478	15	1878	116.18	ts are slyly. blithely express accounts sleep slyly regular patterns. pending deposits 
122483	20	3233	367.14	cial ideas cajole carefully along the special, regular asymptotes. regular, regular forges haggle blithely across the regular, idle requests. packages boost between the ruthless, unusual accounts
122484	21	4067	339.63	lithely about the carefully bold requests. blithely final deposits across the express courts cajole slyly thinly idle accounts. sometimes
122485	22	2106	180.36	ccounts haggle even patterns. silently quiet deposits about the even, regular platelets must have to cajole furiously above the gifts. even packages wake along the pending pa
122486	23	5959	396.52	iously. blithely bold packages haggle. carefully final excuses affix carefully ironic pinto beans. carefully ironic foxes haggle ne
122487	24	6195	103.09	 regular courts sleep furiously slow accounts. slyly express instructions
122491	28	9938	569.72	ring theodolites doubt furiously along the ironic asymptotes. furiously special packages unwind blithely silent, special ideas. carefully special theodolites integra
122492	29	1614	408.17	slyly. ironic accounts are. ironic requests are fluffily along the carefully final deposits. quickly special platelets are final, 
122496	33	8764	299.93	gular packages haggle carefully slyly ruthless foxes. even multipliers after the blithely final deposits nag slyly besides t
122497	34	5440	983.08	ugouts cajole slyly slyly furious pinto beans. furiously regular theodolites haggle. regular realms use carefully. slyly pending 
122500	37	7106	677.28	its are. carefully silent theodolites cajole stealthily. sly, ironic dependencies haggle carefully bold dependencies. carefully pending requests affix alongside of the slyly final instructions
122501	38	5463	829.22	 ironic requests wake accounts. quickly unusual packages are against the furiously unusual ideas. slyly
122502	39	2769	118.66	ly against the deposits-- silent ideas sleep alongside of the express dependencies. blithely special foxes nag quickly. final, sp
122506	43	8196	987.81	entiments cajole blithely furiously final requests. sl
122507	44	2672	866.96	platelets cajole fluffily. blithely final ideas use furiously. requests serve quickly regular dependencies. bold gifts detect above the blithely even excuses. fluffily pendin
122508	45	2284	80.83	s across the fluffily ironic excuses haggle blithely pending patterns! blithely even packages nag across the bold asymptotes. slyly regular dolphins are along the fluffily bold accounts. final
122513	50	2920	909.25	ticingly final, express requests. enticingly ironic accounts cajole quickly according to the accoun
124977	2	2720	468.82	s. furiously ironic theodolites wake slyly across the pending requests. quickly express ideas are against the blithely silent pinto beans. pending pinto beans affix according to the blithely i
124978	3	7787	152.61	es: furiously ironic theodolites cajole pinto beans. accounts nag slyly regular deposits? carefully regular requests above the ironic packag
124980	5	9737	818.26	kly even packages after the even, silent accounts haggle quickly bold, ironic accounts. ironic, pending accounts integ
124984	9	6793	809.83	excuses. slyly even packages cajole furiously. instructions sleep according to the regular deposits. ironic requests sleep furiously regular pinto
124987	12	6458	39.46	ornis. fluffily regular excuses boost slyly regular requests. unusual, express excuses wake after the unus
124988	13	4745	67.09	 regular, even foxes sleep furiously about the even instructions. regular requests detect furiously regular, bold pinto be
124990	15	4321	264.41	 requests. slyly ironic accounts integrate furiously after
124995	20	7239	325.00	ully even accounts solve slyly carefully special requests. furiously dogged re
124996	21	2996	936.25	s use. slyly final requests boost furiously packages. unusual packages across the slyly final deposits run furiously among the ironic dolphins. furiously ironic packages af
124997	22	170	246.95	express foxes boost after the ironic accounts. fluffily regular asymptotes sleep requests. carefully bold foxes across the slyly ironic packages
124998	23	2892	735.43	. regular platelets haggle. slyly regular pinto beans among the regular dolphins haggle carefully regular asymptotes. accounts are slyly against the slyly ironic dependencies. slyly final p
124999	24	3879	950.96	ding excuses. carefully idle requests after the final, special pinto beans cajole slyly carefully final deposits? pending, express accounts detect blithely. pending foxes are slyly ab
125003	28	6682	505.62	quickly; furiously regular foxes after the even, regular requests sleep carefully throughout the carefully ironic warthogs. slyly regular ideas across the even theodoli
125004	29	7729	343.77	 regular, final somas. furiously ironic requests wake furiously. special, silent deposits wake alongside of the furiously brave accounts. slyly special packages 
125008	33	6836	155.75	uctions. pending, even theodolites cajole blithely across the slyly express theodolites. fluffily unusual deposits impress; bol
125009	34	6325	679.88	ckly. permanent, ironic deposits sleep furiously final accounts. thinly final theodolites
125012	37	1773	680.01	ctions. carefully unusual instructions haggle carefully ironic deposits. blithely even accounts are slyly. slyly express instructions boost fluffily. silent requests use fluffil
125013	38	2790	4.04	y ironic pinto beans. ironic, regular platelets impress permanently even dolphins. even deposits cajole furiously across the blithely regular deposits. fluffil
125014	39	9048	279.54	l pinto beans wake furiously carefully even accounts. final requests are carefully. dogged, bold gifts cajole car
125018	43	4853	611.87	ely regular deposits nag boldly. pinto beans wake blithely final accounts.
125019	44	1892	438.40	n pinto beans. silent, unusual deposits boost blithely. quickly unusual packages are furiously across the furiously ironic accounts. furiously reg
125020	45	4019	346.43	unusual asymptotes boost quickly furiously pending foxes. special, express excuses use slyly. ironic deposits nag against the even theodolites? ironic depend
125025	50	9858	978.97	 requests haggle around the fluffily regular theodolites. carefully express excuses af
127489	2	4985	781.47	heodolites cajole quickly pinto beans! pending, regular requests are furiously among the slyly permanent requests. bli
127490	3	5283	416.03	y pending deposits use carefully slyly special deposits. idly ironic asymptotes sleep furiously according to the blithely final requests? ironic,
127492	5	5958	842.03	egular, sly packages. deposits use furiously about the deposits. busily unusual requests sleep furiously ruthless decoys. express packages nag carefully furiously
127496	9	7044	960.93	iously express packages. carefully ironic requests boost doggedly since the ideas. carefully final requests wake blithely about the furious
127499	12	7840	654.22	e boldly even excuses integrate about the carefully regular excuses. 
127500	13	722	188.24	 dependencies. furiously express warhorses nod. slyly even accounts boost slyly carefully regular asymptotes. special deposits cajole
127502	15	7835	557.81	le after the ruthlessly even theodolites. blithely regular accounts wake according to the
127507	20	9673	673.21	gle ruthlessly furiously ironic pinto beans. special platelet
127508	21	7368	658.27	 furiously against the fluffily special requests. accoun
127509	22	7078	889.82	jole blithely to the pinto beans. ruthlessly ironic warthogs are according to the quickly even deposits. slyly final packages affix furiously bold realms
127510	23	7286	173.48	even dugouts. regular accounts are along the fluffily unusual excuses: unusual deposits haggle among the carefully regular accounts. some
127511	24	9177	420.60	ing theodolites across the blithely even requests are carefully even multipliers. quickly regular theodolites hagg
127515	28	5283	28.12	otes sleep alongside of the accounts. slyly special dependencies breach carefully across the furiously regular instructions. pending accounts haggle carefully. carefully regular theodo
127516	29	6722	648.17	yly express accounts. permanently regular requests haggle instead of the blithely regular requests; account
127520	33	6185	141.57	 foxes boost according to the carefully unusual deposits. accounts nag furiously furiously regular instructions. final dependencies shall have to caj
127521	34	598	4.22	rns integrate slyly. furiously regular deposits use along the bold, regular deposits. carefully stealthy dolphins according to the blithely final pinto beans thrash alongside of the pending, 
127524	37	2913	542.67	sly regular deposits. blithely regular realms haggle furiously during the silent instructions. carefully regular ideas across the furiously final ac
127525	38	2512	405.94	hely. carefully express packages haggle. final deposits haggle slyly. slyly final requests sleep slyly slyly regular de
127526	39	1189	556.46	s. unusual, regular deposits sleep furiously braids
127530	43	1831	379.53	rate regular accounts; express ideas sleep fluffily even requests. furiously final requests cajole fluffily. car
127531	44	2707	511.63	cies. theodolites maintain blithely according to the quickly regular accounts. final reque
127532	45	7080	394.99	nto beans! slyly unusual accounts sleep. deposits among the slyly f
127537	50	1237	543.85	efully pending, unusual pinto beans. furiously regular courts engage furiously around the blithely regular ideas. final foxes haggle carefully. brave deposits wake around the final deposits. slyly p
130001	2	9930	568.68	 fluffily final depths above the carefully silent dependencies sleep slyly slyly unusual instructions. carefully ironic dependencies boost furiously final packages. quickly final asymptotes play
130002	3	8453	37.78	ronic gifts wake above the furiously final accounts. regular 
130004	5	2097	389.29	 silent accounts nag blithely after the instructions. quickly regul
130008	9	5605	387.17	y unusual platelets affix carefully furiously special packages. final depths among the furiously unusual courts sleep blithely regular asymptotes. final packages nag among the slyly final requests. 
130011	12	3908	658.63	nic ideas wake fluffily about the quickly final deposits! furiously ironic packages are blithely even requests.
130012	13	8162	135.24	ent packages. even instructions at the ruthlessly ironic packages affix carefully
130014	15	1203	698.10	ully special requests are slyly pending accounts. quickly regular deposits
130019	20	4871	782.58	 slyly. blithely bold instructions cajole. carefully regular requests wake slyly packages. furiously pending packages cajole slyly according to
130020	21	4367	746.59	unusual dependencies. pending packages according to the furiously final accounts boost slyly quiet packages: slyly final packages around the ironic foxe
130021	22	9733	633.77	jole furiously idly special accounts. carefully final deposits above the slyly regular excuses are slowly about the furiously pending excuses! accounts are carefu
130022	23	1626	902.16	eans. slyly express requests are furiously unusual dependencies. fluffily ironic deposits alongside of the special
130023	24	2159	707.66	latelets haggle furiously pending accounts. fluffily final deposits nod quickly about the slyly even packages. final, even ideas solve furiously. regular ideas after the sl
130027	28	9720	881.87	ptotes? pending accounts cajole carefully. regular dependencies detect dolphins. slyly regular theodolites sublate quickly across the ironic accounts. quickly ironic packages believe carefull
130028	29	513	131.55	 across the even requests integrate against the express, ironic packages. slyly regular instruc
130032	33	1230	648.20	ld packages. blithely regular requests use quickly furiously regular packages. accounts doze quickly. special, bold platelets run busily silent instructi
130033	34	1038	380.57	lyly quick packages. fluffily even epitaphs detect slyly careful realms.
130036	37	9868	175.91	. special deposits wake furiously special instructions. pending asymptotes d
130037	38	782	86.05	e pending deposits cajole under the quickly pending requests. instructions doubt blithely. fluffily final packages haggle carefully final requests. unusual, even accounts along the blithely 
130038	39	7535	389.23	ounts. regular dolphins cajole against the silent pin
130042	43	759	687.89	riously silent deposits instead of the furiously ironic requests sleep slyly express, pending requests; slyly sile
130043	44	5120	532.14	its wake fluffily. special ideas wake quickly furiously unusual foxes. express packages breach fluffily across t
130044	45	4673	934.60	its affix against the deposits; foxes are carefully. dolphins hag
130049	50	3566	55.44	cording to the furiously final accounts. regular, ironic ideas along the instructions boost above the express deposits. furiously quick theodolites
132462	2	1791	969.80	ounts against the ironic pinto beans sleep carefully slyly regular ideas: requests haggle quickly whithout the furiously regular theodo
132463	3	4064	620.75	. carefully ironic deposits nod furiously. express requests are closely after the slyly regula
132465	5	3483	231.27	 regular packages integrate across the final, final ideas. blithely regular requests about the 
132469	9	1845	128.65	he blithely ironic excuses boost enticingly furiously special instructions. carefully final ideas sleep. foxes sleep ruthlessly abou
132472	12	7386	601.89	quickly final accounts? blithe accounts run furiously among the daringly ironic deposits. slyly bold foxes wake slyly. final accounts nag through the furiously express requests. id
132473	13	3055	668.74	packages sleep alongside of the furiously express pinto beans. pending dinos doze furiously even warthogs. blithely ironic requests t
132475	15	3754	552.61	lyly regular deposits sleep carefully. furiously spec
132480	20	4897	440.17	onic, ironic packages. silent, regular theodolites believe against the quickly 
132481	21	1441	874.21	sual requests are blithely special epitaphs. even foxes detect. accounts sleep fluffily accounts. ideas are furiously along the quickly brave excuses. blithely final 
132482	22	8315	170.71	n accounts wake furiously even foxes. quickly express theodolites nag slyly final platelets. unusual packages wake slyly furiously bold deposits. carefully final accounts wake. furiously final de
132483	23	9426	615.67	efully express accounts. furiously regular excuses are furiously ruthlessly final deposits. carefully even instructions haggle. blithely even ideas sleep slyly. furiously slow i
132484	24	1440	750.26	 foxes boost furiously across the regular, final theodolites. carefully special escapades unwind carefully above the slyly final packages. carefully special ideas sleep among the slyly stealthy t
132488	28	234	242.66	 carefully regular pinto beans are. carefully fina
132489	29	6048	709.20	le dependencies. special packages are furiously. carefully express requests lose quickly. even deposits nea
132493	33	6948	691.68	posits detect along the asymptotes. slyly final packages breach against th
132494	34	6637	286.34	e against the quickly final dolphins. quickly even tithes along the pending, regular requests eat foxes. blithely ironi
132497	37	4240	720.96	 the quickly express requests nag above the bold requests. furiously fluffy deposits use! carefully express deposits serve furiously regular instructions. pending package
132498	38	2698	893.55	blithely. furiously unusual deposits along the quietly ironic
132499	39	4307	370.48	efully bold requests. regular ideas are furiously regular ideas. fluffily unusual deposits use blithely. regular ideas against the carefully even 
132503	43	3034	918.39	es haggle quickly according to the final ideas. unusual instructions wake furiously. slyly ironic instructions should mold fluffily am
132504	44	9678	705.60	ickly about the deposits. unusual realms boost blithely.
132505	45	1057	428.33	ending deposits sleep carefully? furiously regular platelets are slyly bold pinto beans. blithely regula
132510	50	9629	60.31	. carefully special pinto beans are carefully blithe requests. ironic deposits cajole according to the quickly final dolphins. regul
134982	9	4472	593.62	sleep carefully. requests against the final instructions lose blithely after the furiously silent requests. sl
134975	2	8904	54.06	unts. slyly ironic packages sleep along the final ideas. instructions cajole idly furiously even ideas. final, final braids against the ironic, exp
134976	3	8293	454.30	e blithely idle pinto beans. carefully pending theodolites sleep; fluffily ir
134978	5	4793	98.23	nal instructions use alongside of the final deposits. final instructions use carefully carefully final deposits. slyly bold requests integrate slyly accounts. slyly even requests across the blit
134985	12	7764	167.79	ctions cajole slyly across the carefully ironic foxes. furiously bold excuses print furiously furiously regular packages. carefully un
134986	13	7329	877.33	ously ideas. slyly bold pinto beans along the carefully bold deposits breach furiously even platelets. regular deposits across the blithely final ideas nag a
134988	15	1289	374.95	 theodolites. dolphins haggle carefully slyly regular deposits. unusual deposits boost furiously. 
134993	20	5265	26.68	s cajole quickly against the blithely final packages. blithely regular courts cajol
134994	21	2439	481.27	ly ironic dependencies sleep furiously unusual asymptotes. special, ironic packages wake quickly silently bold sauternes. fluffily express requests 
134995	22	2779	789.87	gle instructions. fluffily final requests are furiously excuses. blithely regular platelets boost
134996	23	7417	999.86	ully according to the blithely even instructions. unusual ideas after the quiet warthogs eat furiously ironic accounts. packages use-- caref
134997	24	9997	919.26	en deposits-- blithely final ideas are slyly close instructions. slyly even deposits cajole. ironic, final accounts sleep alongside of the pinto beans. 
135001	28	7219	209.76	 slyly regularly blithe foxes. quickly regular ideas nag slyly around the blithely ironic foxes. regular, special theodolites use slyl
135002	29	9013	560.08	ccounts. final braids according to the furiously ironic packages affix furiously carefully stealthy p
135006	33	7332	867.38	ily across the furiously ironic escapades. pending theodolites alongside of the special packages must haggle under the blithely careful packages. blithely
135007	34	1948	594.86	 slyly special accounts above the furiously express packages cajole across the furiously ironic platelets. regular theodolites sleep. slyly busy grouches haggle quickly among the fluffily reg
135010	37	2825	900.80	 slyly silent packages up the furiously regular requests haggle carefully with the slyly bold packages. regular packages haggle fluffily! furiously regular i
135011	38	2965	520.24	c accounts? final, furious requests integrate fluffily after the slyly express requests. pinto beans haggle furiously.
135012	39	4547	988.82	ts run accounts. furiously regular accounts along the carefully bold foxes use fluff
135016	43	9788	194.46	silently whithout the packages. fluffily fluffy packages boost slyly furiously unus
135017	44	2630	634.75	in requests according to the blithely ironic accounts affix slyly about the blithely ironic excuses. regular frets alongside of the regular deposits caj
135018	45	1592	898.21	olites doubt blithely blithe packages. furiously even ideas grow. ironic, final packages against the carefully even requests integrate furiously doggedly
135023	50	1675	981.12	dolites integrate after the carefully ironic accounts. slyly regular deposits was slyly accou
137501	15	9034	334.65	ously stealthy packages are. special platelets abo
137488	2	4463	133.85	ccounts. slyly silent accounts dazzle furiously quickly pending asymptotes. even somas wake blithely about the ironic, ir
137489	3	7560	373.86	ts; final accounts should wake fluffily after the slyly even foxes!
137491	5	4593	732.60	usly. accounts cajole slyly. ironic theodolites affix about the ironic packages. ironic, busy platelets haggle blithely; quickly final packages above the carefully careful 
137495	9	5356	517.62	ckey players detect about the blithely even courts. quickly busy packages sleep furiously about the slyly regular orbits. slyly final accounts wa
137498	12	2697	354.46	warhorses sleep. accounts haggle carefully. carefully bold dependencies cajole quickly. ruthlessly express packages wake.
137499	13	120	810.97	gedly above the bold, special deposits. quickly final dependencies are evenly regular foxes. fluffily bold instructions cajole slyly at the regular packages. 
137506	20	5450	667.64	tions boost. carefully express courts detect along the slyly final sauternes. blithely final accounts sleep. ironic, regular instructions are slyly express r
137507	21	2169	191.50	 ruthless deposits above the blithely final instructions use quickly above the spe
137508	22	316	678.33	l requests. requests sleep blithely. slyly pending accounts are slyly fina
137509	23	8050	499.44	phs. furiously blithe braids kindle along the furiously ironic deposits; slyly u
137510	24	2940	813.62	. quickly express deposits cajole slyly regular excuses. pinto beans hag
137514	28	7241	525.18	ntly special deposits. final theodolites sleep carefull
137515	29	5729	942.44	lve against the accounts. slyly regular deposits integrate doggedly. slyly final accounts dazzle finally. slyly ironic warthogs sublate 
137519	33	5458	484.49	lly regular depths. carefully express courts haggle blithely bold packages. pending accounts cajo
137520	34	1964	533.31	s are carefully around the daringly final ideas. silently final accounts haggle. blithely regular 
137523	37	7656	764.19	pendencies eat along the theodolites. quickly regular requests hang evenly about the express platelets. daringly even excuses cajole after the pinto beans. blithely regular theodolites sl
137524	38	2718	378.17	ic, sly theodolites sleep blithely bold, regular theodolites. regular, special accounts are
137525	39	8408	584.77	ndencies. carefully final deposits try to use blithely along the 
137529	43	7877	880.95	about the furiously even escapades. special accounts sleep around the carefully express foxes. furiously express accounts cajole. instructions integra
137530	44	627	295.15	osits. carefully even pinto beans wake furiously. quickly slow instructions boost fluffily. unusual 
137531	45	1343	166.01	ke slyly after the furiously final requests! ironic, regular platelets sleep. furiously pending instructions impress. carefully regular dependencies wake slyly. ironic,
137536	50	547	810.19	bove the ironic dependencies detect furiously regular packages. blithely regular foxes sleep evenly sly
140001	2	3675	12.89	ual pinto beans boost furiously carefully pending theodolites. ironic requests use along the ironic deposits. pinto beans nag
140002	3	4647	842.34	g the blithely regular accounts sleep blithely unusual ideas. bli
140004	5	8734	901.06	al, pending packages. courts cajole slyly. furiously pending asymptotes are furiously final, final ideas. silent, regular accounts haggle furiously platelets. carefully regula
140008	9	5818	456.87	ons. careful requests are slyly carefully special packages. quickly pending deposits sleep blithely according to the fin
140011	12	6005	213.51	 maintain carefully slyly ironic requests. slyly even dependencies eat above the blithely regular pinto beans: theodolites wake across the blithely final deposits. slyly pending requests wake blit
140012	13	1762	864.51	pecial dolphins across the slyly special tithes detect across the silent 
140014	15	4221	997.01	osits. regular escapades wake after the ironic, final instructions. final packages nag. special, final packages sleep alongside of the furiously ironic foxes. even, ironic frays through the busy fox
140019	20	4475	196.10	ously according to the furiously special platelets. pending packag
140020	21	6100	227.35	. quickly silent accounts according to the boldly regular deposits affix
140021	22	6811	426.30	y about the even, even orbits? final, even accounts nag
140022	23	9550	810.94	ajole regular requests. quickly special excuses detect foxes. furiously bold deposits impress fluffily quickly specia
140023	24	3302	667.76	s the busy ideas. deposits are slyly above the blithely careful packages. slowly regular packages nag about the blithely ironic packages. furiously express deposits are carefully 
140027	28	7413	432.49	tealthily ironic deposits detect blithely above the slyly bold packages. final requests cajole. slyly regular foxes cajole furiously. warhorses believe
140028	29	5954	444.36	hely ironic excuses snooze carefully fluffily bold excuses. carefully bold excuses might print idly ironic, express asymptotes. express ideas use fl
140032	33	5135	422.96	ss the furiously ironic packages! carefully special deposits sleep after the final, final requests. p
140033	34	6039	885.92	 boost along the fluffily silent requests. furiously regular asymptotes maintain slyly regularly regular instructions. quickly s
140036	37	5419	332.40	 against the ironic packages; regular, regular deposits might x-ray. packages cajole slyly above the permanent platelets. regular deposits haggle furiously re
140037	38	5999	392.21	arefully special asymptotes. enticing pinto beans haggle furiously. ironic requests are quickly sly
140038	39	3915	678.67	ly stealthily regular dolphins. slyly even packages c
140042	43	9472	204.86	onic accounts according to the regular, unusual deposits doze blithely above the fluffily even accounts. fluffily regular packages wake blithely about the express deposits. final,
140043	44	6871	755.35	inal, final excuses across the blithely sly theodolites sleep blithely deposits. special foxes sleep carefully around the even dependencies. quiet pinto beans ab
140044	45	5692	437.37	ully bold requests. slyly final foxes lose alongside of
140049	50	6124	509.87	lly even orbits haggle slyly. carefully even platelets sleep blithely blithely final accounts. pinto be
142470	13	2087	214.07	kages. furiously even accounts boost furiously above the slyly
142459	2	6973	676.95	uests. quickly unusual pinto beans sleep blithely. final instructions are above the blithely ironic instructions. final pinto beans among the carefully final d
142460	3	6180	275.40	ress theodolites maintain. blithely bold platelets sleep
142462	5	7922	135.14	unts wake across the carefully special accounts. fluffily final packages cajole. ironic pinto beans use by the blithely ironic deposits. r
142466	9	8132	633.40	ieve furiously. furiously silent accounts nag. slyly pending deposits are. brave ideas cajole. carefully regular
142469	12	7669	404.97	refully. asymptotes nag alongside of the slyly express dinos. slyly ironic pinto beans cajole. carefully final courts wake blithely. blit
142472	15	9844	446.97	ickly according to the quickly bold accounts. slyl
142477	20	8273	239.97	ily pending platelets. regular excuses cajole furiously according to the ironic packages. slyly pending requests are. fluffily even instructions are fluffily. regular requests are. furi
142478	21	8553	47.23	symptotes cajole carefully among the special escapades. thinly final
142479	22	1904	569.59	 pending requests. daring, unusual packages may are carefully among the quickly regular deposits-- slyly special re
142480	23	9058	470.70	 pinto beans cajole under the quickly bold accounts. platelets sleep quickly. furiously pending dolphins dazzle furiou
142481	24	5286	557.10	s. furiously silent depths breach furiously along the pending pinto beans. blithely even instruc
142485	28	2571	833.77	ly. idle deposits along the bold, even requests haggle carefully final excuses. carefully regular requests a
142486	29	7567	9.21	sts. furiously unusual requests are blithely about the slyly sly ideas. 
142490	33	2057	45.97	lar accounts. unusual, express theodolites wake. carefully pending asymptotes are furiously tithes. regular acco
142491	34	559	542.07	ndencies wake according to the carefully unusual dependencies. blithely fluffy
142494	37	5896	837.12	ly pending pinto beans. fluffily regular accounts print. quickly express deposits use permanently. carefully ironic accounts wake quickly. accounts affix thinly among t
142495	38	8035	535.42	ages cajole carefully. even, special accounts across the blithely ironic deposits cajole fluffily even platelets. slyly ironic ideas use. carefully express depo
142496	39	6469	117.86	ely unusual excuses against the courts are quickly according to the even, final deposits. ironic requests sleep. slyly special requests according to the carefully regular foxes wake f
142500	43	10	226.08	ously unusual foxes cajole slyly regular, unusual packages. regular packages during the special requests ought to sleep across the carefully regular escapades. bold instru
142501	44	8455	372.04	 above the furiously special packages thrash furiously about the packages. carefully regular deposits cajole alon
142502	45	2481	882.56	thely regular frays cajole furiously along the furiously even theodolites. pending packages boost speci
142507	50	5772	727.25	s haggle furiously regular accounts! quickly express multipliers are quickly above the blithely silent requests. busy, even accounts nag fluffily. blithely 
144973	2	5891	553.99	xes. quickly daring dependencies use carefully carefully pending accounts. regular packages wake carefully. furiously bold packages wake carefully enticin
144974	3	1233	375.11	 the ironic, regular foxes. ironic, pending packages haggle carefully boldly bold foxes. fluffily unusual ideas hang ironic frays. express, 
144976	5	4231	592.18	e ironic, unusual ideas. quickly silent requests according to the regular ideas haggle ironically regular excuses. special ideas above the furiously silent packages wake ironic packages. stea
144980	9	6950	404.79	s cajole theodolites. regular packages are across the s
144983	12	2728	821.02	ic, regular pinto beans. silent accounts sleep slyly. quickly even theodolites would sleep carefully blithely bold packages. furiously regular packages across the 
144984	13	1176	905.96	riously express excuses: requests cajole blithely. fluffily even packages about the requests ser
144986	15	5896	292.43	efully. ideas detect slyly. blithely bold deposits
144991	20	2540	358.18	y special deposits boost thinly furiously even fr
144992	21	9349	168.56	 the furiously pending deposits. pending theodolites ha
144993	22	6755	290.06	the slyly final packages. even excuses cajole. carefully regular 
144994	23	6283	752.14	 the ironic, ironic platelets cajole carefully against the deposits. packages cajole quickly. requests sleep c
144995	24	5736	501.89	 final packages haggle. carefully final hockey players integrate fluffily. carefully special dinos sleep sometimes 
144999	28	238	132.38	ut the quickly pending excuses. furiously even foxes cajole slyly among t
145000	29	986	964.10	ies lose slyly slyly pending accounts. final deposits eat carefully. fluffily ironic accounts use; furiously regular deposits are. regular
145004	33	1292	364.97	al sauternes among the carefully even deposits use quickly among the fluffily pending packages. express foxes lose alongside of the bold asymptotes. frets use quickly slyly even acco
145005	34	8042	44.93	dencies affix alongside of the final, express accounts. blithely regular foxes haggle slyly about the packages. final acc
145008	37	9019	996.14	deposits. even deposits poach quickly around the furiously pending
145009	38	9893	530.66	ackages. regular accounts sleep after the carefully ironic accounts. ironic, regular forges haggle furiously alongside 
145010	39	5496	831.29	ithely around the packages. regular pinto beans nag furiously blithely unusual requests. carefully spe
145014	43	2698	557.43	fully around the deposits. quickly ironic escapades after the sometimes unusual accounts wake carefully slyly thin accounts. even, bold packages at the even requests cajole blit
145015	44	7092	42.65	inal multipliers! silent, final accounts x-ray blithely regular, bold packages. furiously
145016	45	8483	2.93	ests boost quickly even packages. quickly even deposits above the quickly ironic requests maintain carefully among the fluffily ironic dolphins. furiously slow packages int
145021	50	9782	588.98	ss the final requests. unusual packages wake furiously above the furiously pending accounts. express, special accounts wake. carefully fluffy dep
147487	2	2038	629.44	uctions affix blithely blithely regular accounts. unusual, ironic theodolites wake. furiously unusual pinto beans are final packages. regular, daring d
147488	3	4274	952.70	unusual platelets above the carefully silent requests use quickly regular pac
147490	5	9051	932.08	carefully final requests. carefully pending deposits cajole fluffily across the regular pains. carefully unusual excuses sleep slyly. quickly ironic packages use blithely. quickly silen
147494	9	1836	184.67	onic deposits wake furiously after the packages. regular packages cajole slyly. final, ironic excuses nag among the special packages. slyly ironic pinto beans after the carefully pend
147497	12	4868	137.66	ccounts print silently across the slyly final requests. stealthy ideas snooze along the escapades. requ
147498	13	6837	414.10	 according to the slyly silent ideas. express requests run blithely. regular asymptotes run blithely. fluffily pending deposits dazzle slyly. pending asymptotes are.
147500	15	9035	799.84	c platelets affix slyly against the packages. fluffil
147505	20	7193	516.04	sentiments-- quickly ironic theodolites wake. furiously ironic packages shou
147506	21	1303	544.86	 express deposits. carefully final platelets after the silent ideas wake carefully against the 
147507	22	2462	268.09	ly ironic deposits. express excuses can use blithely. bravely 
147508	23	1189	124.82	s. fluffily ruthless deposits haggle according to the special, express epitaphs. carefully ironic instructions wake requests. slyly final platelets haggle furiously blithely final theodolites. c
147509	24	1055	993.30	 fluffily regular platelets. quickly pending platelets unwind slyly slyly final foxes. requests nag among the busily final instructions. blithely bold packages haggle 
147513	28	838	791.18	. carefully even requests about the regular, special attainments sleep slyly exp
147514	29	938	829.56	s. carefully regular frets between the regular accounts wake doggedly quiet foxes. even req
147518	33	1178	214.14	s haggle quickly. bold packages are. blithely bold ideas wake evenly. quickly ironic foxes about the fluffily express accounts hang
147519	34	592	232.79	 quickly after the blithely even requests. fluffily even accounts across the ironic, silent asymptotes haggle quickly regular accounts. quickly final packages around the ironic deposits sleep qu
147522	37	5943	53.26	quick asymptotes use blithely furiously regular instructions. ironic ideas eat furiously. regular dolphins w
147523	38	1338	630.79	ng the fluffily regular requests. furiously ironic platelets engage furiously slyly ironic requests. ironic, fluffy accounts cajole across the unusual foxes; ironic courts are; regula
147524	39	8000	925.97	ong the carefully silent requests. even foxes mold furiously along the
147528	43	8767	581.07	sts. furiously sly notornis sleep carefully silent, ironic requests. pinto beans nag quickly carefully special packages. slyly even requests use furiously. slyly fin
147529	44	1156	535.96	ial sheaves haggle carefully according to the regular requests. specia
147530	45	8793	161.97	ding, ruthless escapades sleep furiously across the slyly special excuses. carefully final instruction
147535	50	3135	675.49	nts: final packages haggle evenly according to the carefully ironic ideas. slyly ironic dolphins haggle car
150019	20	7763	765.51	 the furiously even sheaves. ironic, pending ideas engage carefully. even accounts haggle blithely into the unus
150001	2	6078	149.74	lar dependencies. regular instructions sleep slyly. ironic pa
150002	3	3912	359.64	rbits integrate carefully silently express deposits. regula
150004	5	644	165.89	ly along the pending sauternes. frays boost against the final, silent asymptotes. packages integrate blithely. slyly blithe requests haggle fi
150008	9	6389	950.59	l deposits: furiously regular requests haggle. quickly ironic courts cajole. slyly bold ideas ought to boost furiously among the blithely unusual accounts.
150011	12	4045	113.34	hely bold theodolites sleep. express, express foxes sleep. slyly enticing requests will sleep slyly enticing asymptotes. quickly express pinto beans acro
150012	13	1645	647.81	d escapades. carefully final theodolites are carefully. theodolites print busily special foxes! furiously final excuses nag carefully at the
150014	15	4958	333.51	nal instructions breach against the careful, pending fo
150020	21	1145	877.85	 asymptotes haggle across the even asymptotes: quickly ironic pinto beans according to the fluffily special packages haggle furiously about the carefully pending instr
150021	22	1852	45.96	packages. slyly regular courts sleep furiously express, final asymptotes. blithely even deposits sleep furiously blithely regular deposits. quickly regular ideas wake furiously. 
150022	23	2726	907.89	ns promise slyly slyly final requests. unusual, regular accounts alongside of the blithely bold deposi
150023	24	3037	423.00	 requests. regular, ironic excuses sleep furiously. dolphins nag flu
150027	28	3190	596.38	ts cajole slyly regular accounts? quickly ruthless accounts cajole fluffily. quickly regular deposits sleep quickly. even, regular depths wake furiously after the carefully e
150028	29	217	502.98	s-- pinto beans cajole. busily ironic instructions affix. carefully express requests among the quickly regular instruct
150032	33	7944	630.94	e carefully. special, bold ideas haggle furiously. blithely final deposits x-ray. blithely final ideas integrate
150033	34	5055	827.24	 carefully regular requests nag. quickly ironic dependencies boost after the evenly careful deposits. final, special asymptotes sleep blithely. bl
150036	37	809	793.16	rts. carefully slow requests wake express, fluffy d
150037	38	4505	776.59	 accounts are carefully quickly final deposits. silent accounts haggle. slyly regular d
150038	39	7816	231.93	sts. blithely regular deposits sleep blithely carefully regular packages. requests wake furiously about the furiously 
150042	43	4439	764.15	xpress deposits? thin pinto beans use along the daring, regular instructions. silent requests use among the blithely special accounts. bli
150043	44	3324	725.31	deas. quickly brave theodolites nag furiously regular platelets. carefully regular packages are regular theodolites! final instructions above the slyly unusual foxes solve a
150044	45	2882	407.74	its. blithely unusual packages should affix. final accounts about the regular, pending foxes cajole busily after the special Tiresias! frets nag. carefully final packages haggle. requests sleep c
150049	50	2155	32.45	. carefully regular deposits cajole across the qui
152456	2	8602	538.32	deposits unwind quickly upon the fluffily silent foxes. quiet accounts integrate carefully even accounts. unusual, pending instructions cajole care
152457	3	474	678.87	accounts. even, ironic accounts wake fluffily along the final instructions. furiously even
152459	5	5731	982.70	ts. quickly regular theodolites boost slyly according to the even instructions. ironic accounts
152463	9	6178	612.44	idly ironic foxes. furiously express requests detect among the r
152466	12	9304	402.70	riously among the accounts. furiously final theodolites detect furiously ironic platelets. regular courts haggle furiously quickly unusual pinto beans. blithely pending request
152467	13	3547	501.94	al deposits; final, pending platelets nag carefully. furi
152469	15	5003	800.85	nic accounts affix blithely. fluffily unusual requests haggle slyly final instructions. final, final requests 
152474	20	8625	177.56	onic asymptotes after the enticingly stealthy requests are requests. regular, regular deposits nag quickly above the always pending foxes. express deposits nag slyly ironic excuses. express dep
152475	21	9063	310.43	ly express pinto beans haggle across the furiously unusual requests. sly
152476	22	8873	925.25	o beans according to the slyly final accounts detect blithely quickly brave dependencies. even, unusual deposits hang closely regular packages. bold, unusual theodolites alongside o
152477	23	4191	89.46	 integrate quickly special deposits-- requests against the even waters cajole express, unusual requests. fluffily regular packages wake 
152478	24	43	401.67	 frets. fluffily final asymptotes are blithely. slyly unusual accounts sleep. furiously ironic epitaphs 
152482	28	3251	434.25	yly across the accounts. carefully unusual packages hinder quickly among the carefully regular excuses. slyly special forges across th
152483	29	7001	177.69	. platelets cajole furiously along the pending instructions. special foxes cajole slyly across the fluffily unusual instructions. d
152487	33	3698	939.06	ts: regular accounts cajole carefully. bold, express ideas use acc
152488	34	1023	500.17	. furiously blithe dependencies along the ironic, express deposits nag furiously regular packages. quickly unusual 
152491	37	4435	417.48	cross the blithely regular deposits are slyly furiously regula
152492	38	1742	217.30	 furiously ironic packages past the furiously even req
152493	39	436	21.68	the final dependencies. final, special theodolites nag blithely. closely bold accounts cajole carefully along the special, unusual instructions. eve
152497	43	8855	594.39	its. ideas boost. ironic, final requests affix slyly packages. even, unusual deposits grow closely agai
152498	44	6109	506.86	ly express deposits sleep carefully idle requests. regular requests after the carefully regul
152499	45	1119	855.96	nal ideas sleep slyly ironic accounts. unusual requests above the final requests cajole carefully above the instructions. quickly bold platelets haggle requests! furiously reg
152504	50	8197	411.56	ickly unusual, final requests. slyly pending Tiresias use fluffily. special pinto beans boost slyly regular pinto beans. bold, regula
160001	2	5445	390.93	e carefully among the furiously pending ideas. special requests about the unusual,
154971	2	7602	174.89	tes sleep along the even deposits. regular requests cajole furiously. quick theodolites above the blithely silent depos
154972	3	7036	207.68	 pending instructions. slyly ironic depths do haggle. even frets boost. slyly regular pains haggle along the quickly ironic packages. final, pending de
154974	5	6807	290.23	ly enticing foxes. ironic packages wake quickly across the furiously final platelets. final, unusual waters are sometimes foxes. carefully unusual packages according to the final requests wake
154978	9	6253	208.05	r accounts. thinly bold accounts wake fluffily. furiously final ideas use about the slyly unusual instructions. asy
155003	34	5169	896.67	s are furiously. furiously express packages sleep fluffily. carefully final t
154981	12	7586	626.43	ep slyly. regular, unusual frays sleep. ironically special accounts across the 
154982	13	1297	247.18	ound the blithely bold dolphins. quickly even requests nag furiously across the carefully regular foxes. ideas haggle carefully over the carefully pending excuses. slyly express depo
154984	15	985	926.91	ar requests haggle blithely regular instructions. furiously express instructions sleep carefully even asymptote
154989	20	2573	185.54	e furiously carefully ironic dolphins. thin, pending accounts against the furiously final deposits sleep quickly furiously final packages. even packages about the eve
154990	21	35	278.97	accounts use furiously at the carefully regular accounts. c
154991	22	810	735.88	pending foxes was. fluffily even ideas are above the blithely regular ideas. idle deposits about the even depths are fluffily across the furiously p
154992	23	281	339.71	ag into the furiously final accounts. carefully express requests can haggle blithely along the ironic ideas. unusual requests use. blithely ironic deposits eat sly
154993	24	1153	28.23	 requests about the regular deposits haggle blithely above the quickly bold re
154997	28	5215	730.84	ly bold deposits engage alongside of the final pinto beans. blithely final gifts boost furiously 
154998	29	8666	222.68	 bold asymptotes run slyly slyly even epitaphs. platelets haggle. ironic accounts sleep thinly above the deposits. blithely 
155002	33	5850	393.81	carefully carefully silent accounts-- blithely fluffy ideas cajole along 
155006	37	9813	947.00	nts. slyly final requests are boldly. courts around the furiously even requests x-ray across the blithely special t
155007	38	8610	569.16	c deposits about the slyly ironic foxes believe carefully after the slyly final requests. regular, express excuses about the furiously 
155008	39	2270	715.71	nts use alongside of the carefully regular accounts. dependencies boost around the quickly special packages. quickly express instructions wake ironic requests. even deposits cajole furiously
155012	43	7156	487.56	 breach. express deposits cajole slyly regular dinos. blithely regular
155013	44	7448	787.30	ix quickly at the slyly regular pinto beans. blithely final packages sleep express, 
155014	45	7311	747.27	 blithely across the slyly unusual requests. regular packages hinder even requests. regular, pending accounts cajole slyly. carefully brave accounts should have to engage after the e
155019	50	5801	955.29	refully alongside of the pending theodolites. special dolphins boost furiously across the even, pending requests. regular Tiresias are furiously. idly regular a
157486	2	4480	712.89	sual, special foxes nod quickly alongside of the quickly ruthless dugouts. deposits nag quickly al
157487	3	9336	178.49	gle thinly after the slyly pending accounts. ironic, regular asymptotes haggle furiously Tiresias. furiously ironic dependencies hang slyly across the packages. furiously bold acco
157489	5	106	740.96	silent, ironic packages cajole. carefully unusual packages are quickly close instructions. furiously iron
157493	9	7054	668.47	grate alongside of the slyly bold frays. brave multipliers haggle. pending theodolites boost enticing foxes. quickly pending e
157496	12	8614	606.42	o the somas. quickly regular deposits according to the ironic grouches wa
157497	13	8269	207.12	into beans after the regular requests cajole along the slyly bold pinto beans. escapades wake across the blithely express pinto beans. packages are after the asymptote
157499	15	3662	510.34	thely. slyly special accounts cajole fluffily ironic dugouts. deposits nag across the regular ideas. furiously 
157504	20	671	198.85	sits. blithely regular foxes above the packages sleep quickly along the furiously bold pinto beans. furiously bold accounts affix blithely quickly
157505	21	3572	683.80	press, ironic requests. carefully special requests among the carefully final ideas cajole among the blit
157506	22	7526	466.85	even requests nag quickly. furiously pending pinto beans wake fluffily blithely silent instructions. boldly ironic requests are. furiously iro
157507	23	3918	209.62	ing to the carefully even gifts. furiously regular request
157508	24	6834	49.17	d grouches. never final excuses wake carefully express packages. foxes affix slyly accounts. silent, final ideas are slyly. quiet accounts nag furiously at the blithely expr
157512	28	1556	411.02	ounts. carefully ironic ideas must have to cajole. pending deposits sleep slyly. carefull
157513	29	8258	107.78	kages along the slyly ironic excuses cajole furiously in place of the quickly express dependencies. requests wake blithely. slyly final requests above the silent, bold packa
157517	33	5177	424.83	sits wake around the furiously express courts. blithely regul
157518	34	5934	609.86	? furiously special pinto beans sleep. slyly final platelets after the slyly
157521	37	7876	487.40	. slyly pending ideas are. idle, regular accounts cajole quickly at the pending, even deposits. final, special accounts eat carefully. pending dependen
157522	38	1894	956.23	ages! special ideas above the slyly ruthless packages unwind slyly final forges. furiously final dolphins wake slyly. furiously special deposits are fluffily final pa
157523	39	6781	187.83	bout the sly ideas use carefully alongside of the regular ideas-- final
157527	43	9376	274.91	nto beans grow blithely furiously final Tiresias. final, ironic packages haggle along the regular, r
157528	44	8870	39.51	lyly unusual epitaphs haggle thinly quickly regular asymptotes. slyly regular realms eat furiously final asymptotes. quickly unusual theodolites sleep carefully against the unusual platelets. e
157529	45	2232	941.60	ges play furiously. packages haggle. Tiresias haggle furiously among the regular foxes. doggedly special courts around the furiously bold accounts are according to
157534	50	1733	335.04	 carefully express frays. ironic deposits are furiously ca
160002	3	8434	858.25	aggle slyly according to the slyly ironic deposits; bold packages nag blithely after the even, final dependencies. slyly even pinto beans affix. quickly final theodolites about the slyly regular
160004	5	5012	44.05	oggedly. carefully ironic ideas after the pending, unusual dolphins wake slyly among the regular deposits. unusual packages 
160008	9	6300	672.02	dolites are never packages. blithely regular foxes sleep carefully across the express deposits; fluffily bold pinto beans sleep closely idly bold pinto beans. blith
160011	12	4887	14.40	ackages sleep about the slyly ironic dependencies. quickly iron
160012	13	5354	153.44	ly final excuses sleep accounts. deposits sublate along the silent dependencies. furiously f
160014	15	1904	296.18	 unusual pinto beans. carefully pending courts will sleep. slyly express deposits haggle slyly fi
160019	20	8592	380.34	out the ironic foxes. fluffily ironic theodolites wa
160020	21	5151	683.09	hogs cajole slowly after the instructions? furiously ironic deposits cajole blithely furiously final pa
160021	22	66	677.18	aggle slyly express instructions. slyly ironic excuses affix across the regular, ironic deposits. carefully ironic packages
160022	23	3428	323.08	tructions use across the pending, special ideas. ironic instructions haggle slyly regularly even excuses. carefully pending instructions boost. thinly silent gifts haggle s
160023	24	5947	918.19	althy foxes was slyly. fluffily pending theodolites detect slyly even requests. furiously regular pinto beans nag! carefully even deposits grow accordi
160027	28	185	685.05	bold platelets. express ideas x-ray slyly across the ironic, pending deposits. furiously express instructions across the ac
160028	29	7091	237.28	 furiously final ideas. furiously even deposits among the regular, iro
160032	33	9785	474.59	 ideas. blithely special accounts about the final deposits are alongside of the blithely ironic deposits. regularly regular warthogs dete
160033	34	9422	876.62	ronic theodolites. regular, final dependencies around the express, special packages wake above the furiously bold requests. quickly silent p
160036	37	4595	901.88	y regular accounts nag furiously. accounts haggle closely. blithely ironic ideas haggle never express accounts. furiously final instructions nag carefu
160037	38	5445	937.82	reful foxes cajole blithely final accounts. excuses wake carefully. finally even deposits about the quickly express instructions haggle 
160038	39	4478	30.90	packages shall have to haggle carefully packages. pending accounts haggle furiously bold dugouts. accounts affix daringly. carefully regular requests wake. s
160042	43	7176	104.01	urious packages doze furiously fluffily express asymptotes. blithely ironic dependencies kindle furiously in place of 
160043	44	1063	844.01	lly ironic deposits across the asymptotes sleep carefully across the blithely pending packages. carefully regular orbits do grow alongside of the special, final requests. quickly pending accounts bo
160044	45	4046	581.88	ar requests. express pinto beans poach. slyly ruthless 
160049	50	3875	801.51	lar deposits use along the furiously final accounts. furiously final d
162453	2	3986	291.65	rave packages shall have to sublate slyly. pinto beans x-ray alongside of t
162454	3	1840	757.04	n ideas. dependencies would are slyly. bold pinto beans wake blithe deposits; furiously ironic accounts haggle quickly. packages after the carefully bold instructions boost slyly after the fur
162456	5	3669	899.30	ns wake quickly alongside of the furiously final platelets. blithely unusual requests cajole. final theodolites are s
162460	9	6216	76.11	ong the slyly ironic accounts. furiously final packages hang up the waters. carefully daring packages dazzle blithely e
162463	12	8906	482.35	wake along the carefully slow deposits. unusual requests integra
162464	13	6885	988.80	avely special ideas sleep regularly final requests. slyly final ac
162466	15	1065	786.88	onic, idle accounts cajole regular theodolites. ideas nag carefully across the carefully final requests! blithely express deposits could play. blithe
162471	20	2216	572.30	uriously regular accounts according to the fluffily even accounts boost alongside of the furious pinto beans; final grouches sleep busily attainments. ironic requests i
162472	21	9750	873.36	y regular dolphins integrate furiously. bold, regular deposits use about the furiously unusual requests. quickly busy excuses are regular accounts. furiously idle deposits 
162473	22	9852	109.30	 beans sleep furiously. final packages besides the slyly regular ideas nag blithely according
162474	23	9349	262.24	r deposits along the furiously quick requests cajole carefully bold accounts. silent deposits across the regular excuses use entici
162475	24	2392	839.31	ly. furiously regular requests do wake carefully by the
162479	28	3970	111.24	ly dolphins. regular instructions wake busily according to the f
162480	29	9147	957.71	olites detect about the quickly even deposits. even, final requests sleep blithely alongside of the regular deposits. carefully pending instructions are requests. even, even pinto beans boost. enti
162484	33	3690	43.79	e during the carefully close requests. furiously ironic accounts above the instructions haggle ca
162485	34	1961	359.64	regularly regular pinto beans haggle furiously alongside of the theodolites. carefully even packages eat furiously above the furiously s
162488	37	4466	912.48	eans. blithely ironic asymptotes maintain carefully even deposits. carefully pending ideas are carefully for the carefully quick ideas. slowly final req
162489	38	6030	973.63	nts affix slyly according to the slyly bold instruc
162490	39	2256	403.54	ronic, bold ideas mold carefully fluffily final pinto beans. regu
162494	43	3941	799.95	side of the blithely final foxes are blithely among the slyly final packages. blithely regular pinto beans cajole slyly slowly regular deposits. idly final foxes boost furiously e
162495	44	9728	244.20	s sleep. blithely express packages wake across the slyly final packages. final, express deposits integrate after the express, ironic foxes. ca
162496	45	8899	798.41	tect quickly busy deposits. express ideas haggle carefully according to the furiously expres
162501	50	7506	104.85	nst the express requests? pending accounts could have to are carefully before the furiously ironic ideas. platelets are 
164969	2	3183	734.69	. express foxes among the unusual requests wake carefully for the furiously pending pinto beans. carefully ironic instructions sublate carefully furiously pending the
190001	2	5290	943.64	ial theodolites play according to the blithely regular deposits. furiously regular accounts cajole carefully above 
164970	3	2431	18.37	ons boost blithely bold requests. quickly bold grouches cajole slyly fluffily unusual pinto beans. pending accounts wake furiously regular instructions. slyl
164972	5	5307	837.21	e about the carefully special packages. slyly ironic requests detect blithely. slyly bold warhorses cajole for the qui
164976	9	7252	148.90	ts nag carefully regular theodolites. even, express foxes above the pending foxes haggle blithely enticingly special dolphins. ironic 
164979	12	9002	263.17	er the furiously express dugouts. packages sleep. regular dependencies wake slyly across the quickly blithe requests. finally regu
164980	13	2048	192.79	ly thin foxes wake across the furiously express asymptotes. quickly express accounts haggle carefully carefully b
164982	15	333	957.62	cial theodolites wake slyly unusual, ironic pinto beans. unusual accounts sleep carefully. express, 
164987	20	6341	553.90	 final, final instructions use daringly foxes-- furiously ironic requests wake care
164988	21	9091	538.96	nts wake. quickly regular excuses haggle quickly. finally special packages after the carefully ironi
164989	22	605	260.08	 haggle busily furiously quick packages. doggedly final foxes are blithely. blithely pending ideas haggle slyly p
164990	23	5804	392.78	kly beneath the idle deposits. even, final platelets cajole after the even, ironic ideas. furiously regular theodolites use slyly a
164991	24	8747	23.90	jole blithely over the regular courts. blithely reg
164995	28	9890	177.97	uests print furiously slyly final requests. quickly ironic foxes sleep slyly. fluffily final hockey players wake slyly
164996	29	2145	731.29	 tithes snooze slyly theodolites. blithely bold excuses alongside of the blithely regular accounts cajole furiously beneath the ironic, close sheaves. bold requests are. carefully ironic accounts 
165000	33	5316	393.21	fully deposits. final pinto beans alongside of the quickly
165001	34	2109	595.51	yly slyly even packages. slyly bold sauternes about the final, regular accounts cajole boldly about the express accounts. slyly final foxes cajole thinly acc
165004	37	4545	107.10	s. express excuses breach after the blithely regular courts; bold, bold instructions detect! blithely dogged accounts haggle according to the depe
165005	38	7764	770.67	s cajole blithely against the furiously even packages. foxes nod. acc
165006	39	1273	183.71	 final, final asymptotes detect ironic, final accounts. even requests haggle fl
165010	43	8261	912.10	 frays according to the quickly regular theodolites haggle alongsi
165011	44	3770	757.70	unusual ideas. blithely ironic instructions run furiously ironic, pending foxes. blithely unusual instructions about the furiously unusual pinto beans c
165012	45	300	701.21	s use slyly ironic courts. blithely special instructions with the slyly final courts wake above the blithe, express asymptotes. carefully sly excuses
165017	50	8447	909.52	ven instructions integrate slyly pending packages. bold
167485	2	5758	783.06	ly final accounts haggle slyly about the even packages-- final instructions according to the accounts nod around the 
167486	3	9777	137.46	gular pinto beans haggle quickly along the ironic requests. quiet accounts should have to maintain slyly pen
167488	5	3672	951.12	ctions cajole quickly quickly ironic requests. slyly regular accounts wake furiously across the furiously quick deposits. bli
167492	9	6581	29.12	riously busy accounts cajole near the fluffily ironic excuses. quickly special ideas alongside of the regular, ironic excuses use furiously among the fur
167495	12	912	840.18	s. packages among the packages wake quickly alongside of the slyly even deposits. theodolites poach blithely. ca
167496	13	4125	472.43	st blithely. special, final deposits according to the furious Tiresias cajole fu
167498	15	402	647.68	bold tithes. idly unusual requests haggle slowly. slyly stealthy excuses above the furiously permanent deposits will doze slyly unusual asymptotes. blithely ironic theodolites cajole bl
167503	20	70	990.70	y final requests above the blithely pending deposits haggle furiously final ideas. blithely ironic requests are quickly slyly silent pinto beans. slyly regular instructions cajole quickly carefu
167504	21	583	490.60	al epitaphs believe blithely after the unusual packages. express pinto beans are carefully among the blithely pending platelets. courts solve blithely.
167505	22	9204	399.30	ending instructions. accounts haggle carefully. stealthily ironic reques
167506	23	1821	612.06	c pinto beans after the close requests are blithely along the accounts. blithely unusual instructions sleep blithely ironi
167507	24	2392	372.94	terns. furiously even requests haggle. fluffily regular accounts haggle. blithe d
167511	28	9308	109.95	l deposits. pending requests unwind quickly. fluffily unusual requests above the quickly ironic theodolites cajole slyly final excuses. slyly final requests against the bl
167512	29	6990	877.16	he blithely bold courts. fluffily special deposits impress slyly regular tithes. final courts sleep above the ironic, express 
167516	33	37	243.30	he accounts. blithely careful platelets boost. ironic deposits may 
167517	34	6575	973.44	ests haggle carefully among the accounts. blithely even accounts wake. requests cajole slyly. blithely ironic deposits unwind carefully unusual hockey players-- carefully regular 
167520	37	9916	183.26	fily express accounts. foxes nag blithely carefully bold pains. bold depths breach. blithely final foxes along the slyly even ideas x-ray blithely along the 
167521	38	9901	118.47	 even packages sleep slyly bold requests. slyly silent de
167522	39	3233	65.18	tions affix finally special, regular pinto beans. regular, regular pinto beans cajole. 
167526	43	5009	873.62	ss the fluffily ironic ideas solve about the final, ironic requests. instructions doze. slyly special pinto beans haggle furiously after the silent depths. furious
167527	44	8983	37.73	usual ideas wake fluffily. furiously special pinto beans s
167528	45	956	694.29	even notornis. slyly regular requests sleep blithel
167533	50	1112	445.80	ng the ruthlessly express instructions. furiously bold packages at the ironic cour
170001	2	5068	612.79	ring accounts lose around the blithely final accounts. slyly bold deposits use furiously according to the ironically final reque
170002	3	3487	508.89	sual excuses are. quickly pending theodolites do sleep fluffily special deposits. ironically express pinto beans detect furiously. blithely pending de
170004	5	720	198.77	 foxes along the theodolites haggle quickly theodolites. requests wake furiously final instructions. quickly express excuses wa
170008	9	131	694.29	dencies detect against the fluffily final ideas. regular deposits try to sleep. fluffy, stealthy requests according to the unusual, bold accounts eat quick
170011	12	7391	213.82	s maintain fluffily pending deposits. excuses affix carefully above the furiously special packages. blithely ruthless packages cajole blith
170012	13	2849	701.77	ns. furiously brave requests sleep along the finally ironic pinto beans.
170014	15	563	589.97	the blithely regular theodolites. slyly pending asymptotes cajole between the slyly ironic deposits. final, even packages are except the slyly regular theodol
170019	20	2520	985.34	 final sauternes haggle. fluffily express accounts haggle instructions: deposits wake carefully again
170020	21	6226	165.46	ular packages above the final, final dolphins affix furiously along the platelets. express packages wake furiously according to the slyly special e
170021	22	7272	780.49	 the furiously final excuses. blithely even packages use about the slyly unusual deposits. pending, even deposits wake slyly bold pinto beans. unusual, regu
170022	23	8004	397.27	unts wake accounts. pending accounts are. quietly special packages detect blithely behind
170023	24	3507	17.32	. slyly final foxes wake carefully. blithely unusual packages are even packages. furiously bold Tiresias are slyly
170027	28	6202	362.40	gular foxes cajole blithely slyly unusual dependencies. slyly regular requests cajole quickly across the furiously regular excuses. 
170028	29	1561	429.49	are carefully about the blithely final foxes. carefully even accounts wake quickly furi
170032	33	3924	793.89	ncies sleep. theodolites are according to the theodolites. quickly enticing platelets a
170033	34	3755	238.83	the foxes; doggedly regular excuses eat furiously carefully final pearls; ironic deposits wake. slowly final instructions according to the blithely special ideas ar
170036	37	2993	281.16	arefully asymptotes. fluffily pending theodolites wake after the careful, even ide
170037	38	9677	875.95	the carefully pending escapades. permanently unusual packages n
170038	39	219	770.20	ideas. final, final accounts will have to are. quickly even courts boost furiously. regular dependencies run. pending, regular ideas wake. regular instructions boost carefully quickly silen
170042	43	3968	839.75	nal requests wake blithely slyly express frays. excuses serve. accounts promise carefully along
170043	44	1399	693.48	ffily around the packages. even depths against the blithely even pinto beans cajole express, silent excuses. 
170044	45	2641	187.49	c deposits wake. carefully special dependencies nag. pinto beans are across the even, ironic pinto beans. fluffily regula
170049	50	2853	766.47	r the carefully bold packages: slyly regular packages use regular ideas. fluffily blithe accounts integrate furiously furiously unusual ideas. final deposits boost even requ
172450	2	7060	831.96	xes along the carefully even asymptotes cajole carefully even courts. carefully regular packages use slyly carefully regular pinto beans. final the
172451	3	845	994.54	luffily slyly ironic packages. carefully regular accounts are quickl
172453	5	2668	234.81	 affix quickly blithely final foxes. furiously special pinto beans sleep carefully among the slyly pending excuses. blithely ironi
172457	9	5165	656.11	eep slyly requests. blithely ironic excuses are according to the carefully ironic pinto beans. furiously ironic ideas about the slyly daring instructions haggle furiously bold tithes. carefull
172460	12	5905	77.30	mptotes haggle slyly alongside of the sly packages. packages along the silent platelets affix sometimes regular requests. furio
172461	13	9920	601.04	ronic packages. deposits cajole carefully regular, regular instr
172463	15	9550	737.78	ng excuses boost furiously quickly even theodolites. slyly final foxes wake blithely pending accounts. bold warthogs cajole above the quick asymptot
172468	20	9833	744.41	e furiously ironic deposits. furiously pending theodolites wake blithely along the iron
172469	21	4678	506.98	s haggle furiously blithely express packages. accounts after the doggedly unusual dependencies wake alongside of the furiously pending instructions. excuses use quickly iron
172470	22	8945	130.42	sits. express requests haggle special platelets. final deposits boost carefully foxes. blithely unusual epitaphs affix blithely. slyly silent deposits are fluffily after the fluffily
172471	23	9616	226.82	ronic instructions. regular, regular asymptotes serve carefully. slyly bold packages sleep carefully. requests detect furiousl
172472	24	9507	667.91	arefully bold notornis try to cajole fluffily quickly regular ideas. ideas boost carefully. blithely unusual accounts sleep: furiously even deposits
172476	28	1868	324.82	 to the enticing packages. slyly even asymptotes under the regular asymptotes are blithely special a
172477	29	2298	371.90	 regular accounts. furious, quick accounts use evenly fluffily silent dependen
172481	33	8359	546.28	ar ideas. furiously pending packages detect carefully pending asymptotes. ironic accounts do affix blithely. fina
172482	34	9820	696.08	otornis sleep unusual, express accounts. carefully unusua
172485	37	9196	465.64	its nod slyly packages. special packages boost blithely fluffily silent ideas. unusual
172486	38	2840	639.52	ding to the carefully special pinto beans boost fu
172487	39	7970	255.07	e deposits. blithely ironic packages are quickly. bold, f
172491	43	9305	552.88	quickly unusual pinto beans detect slyly express courts. final, express packages use quickly 
172492	44	3805	544.17	y regular excuses. fluffily final foxes cajole furiously furiously regular requests. blithely ironic packages haggle slyly. carefully regular theo
172493	45	3615	667.75	lets boost express deposits. theodolites among the express excuses ca
172498	50	5978	740.65	t across the special accounts. pending requests boost slyly among the finally final pinto beans. p
174967	2	5997	676.95	thely final platelets. carefully bold foxes sublate quickly according to the carefully regular deposits. ideas 
174968	3	3096	696.53	 deposits snooze furiously. carefully ruthless dep
174970	5	9351	979.07	 asymptotes hinder. regular ideas haggle. bold, regular d
174974	9	6140	113.21	 carefully regular accounts eat blithely. never unusual accounts wake about the even, ironic accounts. ironic dolphin
174977	12	252	390.07	ending theodolites wake silent theodolites. carefully 
174978	13	6389	396.23	refully regular requests. blithely enticing asymptotes 
174980	15	9938	802.05	e even packages cajole fluffily ironic courts. enticing requests are carefully special requests-- accounts against the blithely ironic accounts sleep carefully according to the platelets. express d
174985	20	2377	739.60	r accounts. slyly express warthogs cajole quickly alongside of the blithely pe
174986	21	4823	867.32	uffily quick foxes. furiously final notornis use-- regular, pending pinto beans about the carefully even excuses sl
174987	22	5024	268.63	 integrate quickly ironic dugouts. theodolites sleep along the blithely special packages. carefully sly accounts use blithely. quickly silent sent
174988	23	5492	278.19	n asymptotes at the unusual excuses cajole slyly bli
174989	24	6708	739.66	ly final pinto beans. regular deposits are furiously along the blithely silent foxes. slyly ironic forges cajole blithely. furiously final requests cajole quickly regular
174993	28	525	571.74	close theodolites around the furiously even dependencies affix slyly regular packages. regular, regular acc
174994	29	6584	444.23	ounts detect carefully furiously bold ideas. blithely express asymptotes cajole. ironic dependencies serve doggedly above the unusual, ruthless packages. quickly spec
174998	33	7033	739.50	ar, ironic theodolites maintain quickly pending, furious ideas. blithely even packages cajole slowly accordin
174999	34	6213	806.17	arefully even pinto beans nag across the enticingly careful accounts! even asymptotes across the quickly express 
175002	37	4791	962.93	even requests! sometimes ironic deposits across the even, regular pinto beans wake unusual packages. carefully pending theodolites haggle furiously blithely
175003	38	7302	861.08	 cajole furiously. carefully unusual excuses haggle quickly. regular platelets wake slyly accounts. thinly even accounts haggle. spec
175004	39	8924	476.43	ic instructions. pending requests dazzle furiously about the unusual accounts. carefully even instructions play blith
175008	43	9919	690.53	ular platelets believe across the quickly even accounts. ironic, unusual deposits haggle slyly slyly final deposits. pending foxes haggle carefully. iron
175009	44	5597	386.92	affix accounts. regular, ironic courts detect carefully unusual depths? carefully express requests wake. slyly even warthogs sleep quickly slyly unusual theodolites. furiously furi
175010	45	3221	481.19	. blithely bold deposits cajole upon the even, ironic foxes. quickly special courts against the fluffily pending instructions are furiously unusual requests. slyly permane
175015	50	877	519.75	ounts. ideas should sleep slyly. unusual, quick excuses use furiously. regular foxes boost. blit
177484	2	2847	393.29	sual, regular requests. packages impress furiously quick somas. slyly pending dugouts are furiously excuses. furiously final instruction
177485	3	2782	703.61	s theodolites. blithely final realms use across the pending, ironic ideas. slyly final ideas wake stealthily regular requests. Tiresias around the express, special instructio
177487	5	2243	98.76	yly regular deposits. unusual, unusual deposits across the even deposits sleep furiously above the regular pinto beans. slyly final deposits above the bold, unusual foxes integrate quic
177491	9	5486	412.11	luffily across the regular instructions; regular deposits after the furiously final accounts boost blithely among the accounts. furiously quiet foxes wake furiously. deposits wake carefully 
177494	12	6708	894.61	, regular deposits nag furiously among the unusual foxes. express ideas boost quickly a
177495	13	8966	708.28	y even deposits. regular pinto beans hang slyly across the ideas. carefully bold orbits cajole flu
177497	15	9128	964.32	ld carefully ironic accounts. carefully special requests are among the pending deposits. carefully express dolphins wak
177502	20	8615	241.37	e blithely regular packages. blithely special theodolites according to the doggedly regular foxes are quickly ironic asymptotes. furiously ironic deposits th
177503	21	8297	559.33	are quickly at the requests. furiously pending requests are. special, ironic theodolite
177504	22	4601	460.11	 to the blithely final foxes poach furiously regular ideas: furiously pending deposits engage busily along the carefully even accounts. regular accounts cajole s
177505	23	9368	561.53	se. carefully silent braids above the quickly bold requests wake against the instructions. silent accounts cajole! silent, special foxes ou
177506	24	1478	936.51	its. even foxes along the regular foxes nag carefully express asymptotes. carefully regular requests since the slyly furious accounts are above the blithely ironic instructions. blithely special re
177510	28	3374	609.42	ackages. furiously final theodolites across the carefully regular dependencies doze slyly
177511	29	1144	856.39	dolites. slyly express accounts must haggle. foxes doubt furiously against the silently final instructions. quietly ironic accounts al
177515	33	1334	682.59	iously among the special excuses. ironic packages kindle ruthless pinto beans! dolphins cajole slyly 
177516	34	1865	428.83	 haggle slyly about the furiously special deposits. blithely bold accounts poach final, even accounts. final, 
177519	37	1108	266.54	. ironic packages affix fluffily. slyly regular deposits are final packages. slyly unusual requests could haggle blithely regular foxes. carefully express pinto beans 
177520	38	1026	847.01	oys wake across the thin packages! accounts above the special, even excuses breach quickly above the express, ironic theodo
177521	39	7149	967.50	nic ideas are furiously express, close theodolites. requests poach. asymptotes cajole permanently carefully
177525	43	9240	185.43	the ironic packages detect furiously at the slyly special deposits. carefully even platelets sleep. even asymptotes sleep quickly around 
177526	44	5437	994.76	yly even theodolites. fluffily even deposits are quickly after the fluffily pending accounts. depths use slyly above the furiously ex
177527	45	2137	964.74	al, bold packages. ironic, even instructions sleep slyly against the furiously
177532	50	6714	570.46	eposits. regular requests wake about the special, final deposits! blithely special instructions affix quickly regular deposits. furiously even asymptotes a
180001	2	6212	314.54	furiously ironic instructions. slyly unusual platelets cajole furiously above the slyly silent packages. carefully special
180002	3	9550	909.39	 sleep always across the quickly final requests. regular, regular deposits wake quickly among the regular, final pinto beans. theodolites are carefully over the furiou
180004	5	6852	899.73	requests sleep about the blithely regular courts. quickly final requests cajole slyly according to the instructions
180008	9	741	513.87	o the slyly sly theodolites-- quick deposits affix. deposits integrate slyly ironic, regular excuses. carefully special dependencies above the regular theodolites cajole according to 
180011	12	5525	611.08	uriously final theodolites will cajole. blithely special dependencies haggle furiously ideas. slyly ironic packages grow blithely carefully regular tithes. unusual dependencies might cajo
180012	13	1926	456.46	fily pending accounts sleep closely. slyly regular deposits wake blithely. slyly final theodolites are furiously carefully even deposits. accounts nag. requests sleep b
180014	15	7345	557.50	ong the carefully express epitaphs may haggle against the sometimes ironic requests. requests cajole carefully express depths. regular, unusual dolp
180019	20	2867	607.27	oxes integrate furiously about the carefully pending deposits. quickly express instructions detect carefully. packages along the furious deposits cajole along the furiously pending foxes. stealthy, 
180020	21	3024	95.21	e of the ironic courts are foxes. pinto beans was fluffily after the express, special 
180021	22	1211	986.07	side the permanent instructions. ironic, ironic deposits
180022	23	2738	441.14	es across the carefully regular theodolites use slyly against the blithely ironic requests. blithely 
180023	24	7616	475.74	kly after the special requests. quickly bold deposits cajole quickly about the express, regular packages. carefully final dependencies are. ev
180027	28	9797	364.66	ular requests. quickly close packages cajole across the blithely final deposits. carefully silent accounts sleep sometimes. 
180028	29	3612	478.01	 carefully bold packages haggle blithely furiously express theodolites. furiously regular excuses according to the furiously unusual foxes haggle carefully slyly regul
180032	33	403	829.54	ts sleep against the blithely pending foxes. even foxes nag carefully ideas. regular accounts sleep slyly according to the even, regular foxes. quic
180033	34	2772	573.81	 regular pinto beans. carefully daring courts haggle carefully. fu
180036	37	355	820.30	gside of the final pains. accounts print furiously regular packages. blithely 
180037	38	6153	492.11	y unusual accounts boost blithely across the ironic requests. ironic instructions wake blithely blithely ironic e
180038	39	1433	43.14	onic dependencies according to the carefully even ideas haggle ironic dependencies. final packages kindle blithely. pending deposits about the unusual ideas are carefully iron
180042	43	4135	150.95	s are quickly pinto beans. unusual deposits x-ray furiously regular requests. carefully bold 
180043	44	886	988.95	onic foxes: quickly regular grouches sleep. furious
180044	45	5920	272.02	round the slyly ironic pinto beans. carefully express requests according to the blithely silent warthogs haggle carefully express instructions. regular, even dinos sleep furiously regul
180049	50	6685	587.63	ithely special ideas cajole carefully after the regular instructions. regular, final requests by the quickly ironic excuses use closely after the carefully ironic requests. pend
182447	2	9999	778.05	y special ideas. bold, express accounts cajole. slyly idle instructions un
182448	3	6859	214.67	ly regular accounts sleep. quickly close pinto beans sleep slyly. final, final excuses sleep fluffily furiously e
182450	5	9005	785.17	old platelets. even, ironic pinto beans wake along the ironic, ironic theodolites. ironic foxes affix slyly bold patt
182454	9	607	273.24	requests haggle quickly after the regular, even platelets. pending, regular theodolites unwind slyly quickly final instructions. final, final ideas are furiously. regular, blithe ideas are blithe
182457	12	2670	277.15	regular ideas should are. ironic theodolites sleep furiously ironic deposits. express pinto beans along the fluffily pending packages ought to integrate quickly carefully final re
182458	13	7037	783.08	, regular deposits sleep. quickly ironic requests haggle blithely against the busily final deposits. express excuses wake slyly above
182460	15	4659	514.27	are blithely after the slyly regular requests. carefully even packages haggle. quickly regular accounts integrate furiously pinto beans. special de
182465	20	5661	724.37	sly even packages; final packages are after the special theodolites. furiously ironic foxes eat. platelets haggle blithely alongside of the ironic accounts. quickly quiet requests boost f
182466	21	4913	418.13	heodolites. regular dinos wake. carefully pending platelets cajole fur
182467	22	924	160.84	ss, unusual accounts sleep. slyly special platelets wake platelets. furiously special requests promise. slyly regular asymptotes cajole quickly furiously silent realms: qui
182468	23	166	364.62	ans along the slyly silent attainments are furiously stealthily ironic packages. requests cajole f
182469	24	2323	777.27	thely ironic accounts cajole furiously after the regular, ironic ideas. furiously even
182473	28	4743	391.53	 the depths? packages integrate slyly carefully final notornis. ironic deposits are slyly. unusual, bold platelets poach furiously above the furiously bold the
182474	29	37	934.40	ckages haggle slyly. carefully unusual ideas doze blithely about the blithely regular requests! slyly ironic a
182478	33	9516	186.71	pending courts wake slyly. blithely express dependencies boost against t
182479	34	1772	970.91	beans according to the furiously silent pinto beans thrash fluffily about the pending deposits. final ideas cajole ironically. unusual, even deposits boost slowly above the
182482	37	9527	310.77	ctions cajole fluffily. blithely unusual hockey players above the slyly final e
182483	38	5750	524.76	carefully thin packages. regular requests across the ideas are permanently against the carefully ironic dolphins. accounts against the furiously express requests haggle 
182484	39	7971	66.84	ironic foxes nag blithely quickly ironic instructions. blithely final accounts are closely. quickly express requests believe evenly. carefully bold requests haggle furious
182488	43	3691	227.56	yly according to the never ironic excuses. pending asymptotes must have to doze permanent pinto beans. special requests sleep a
182489	44	6549	104.39	usual platelets boost ironic excuses. unusual deposits boost slyly along the blithely silent requests. furiously regular packages cajole along the ironic reques
182490	45	1146	979.90	ts boost. theodolites need to wake carefully. furiously final requests maintain according to the special, even pinto beans. dolphins detect quickly at the bold req
182495	50	2925	918.16	ic foxes. slyly express requests believe blithely special accounts. blithely s
184965	2	9171	197.20	slyly around the slyly even waters. slyly special foxes lose. blithely regular accounts use furiously. carefully express packages nag furiously against t
184966	3	7726	473.96	uriously bold ideas. unusual deposits maintain express packages. blithely regular deposits sleep blithely across the slyly final braids. even packages boost furiou
184968	5	2337	191.76	s. regular packages above the fluffily express deposits sleep carefully carefully ironic packages. bold accounts are carefully about
184972	9	5543	257.34	ubt blithely? slyly regular requests are slyly regular requests. blithely silent theodolites wake carefully across the silent ideas. requests haggle furiously above the regular reque
184975	12	2223	672.26	efully regular warhorses boost requests. blithely express requests doze ironic pinto beans. slyly regular asymptotes integrate above the express requests? instructions sleep. busily ironic dinos
184976	13	2973	144.83	 ironic accounts haggle across the excuses. special, dogged fra
184978	15	3142	467.31	osits cajole slyly. slyly final accounts use slyly besides the final packages. special deposits use blithely among the ironic requests. slowl
184983	20	9771	206.55	cuses. quickly express packages along the blithely bold foxes impress at the unusual, furious accounts. deposits integrate f
184984	21	6828	382.59	cies nag carefully slyly final pinto beans; ironically bold accounts breach furiously final packages. blithely bold requests haggle acco
184985	22	6036	345.58	s after the packages wake alongside of the furiously regular ideas. furiously even dependencies x-ray slyly upon the ironic requests: 
184986	23	8397	80.59	 final deposits sleep fluffily alongside of the furiously express requests. quickly silent accounts boost carefully at the quickly silent ideas. ironic grou
184987	24	9351	487.94	otes boost blithely around the fluffily even packages. carefully even deposits use carefully above the slyly pending deposits. ironic, special pinto beans wake according to the re
184991	28	1191	615.21	after the furiously unusual ideas play blithely pending, express accounts. accounts around the regular accounts haggle blithely ironic dependencies. quickly
184992	29	5862	453.93	ithely fluffy accounts. carefully regular deposits snooze slyly unusual theodolites. quickly pend
184996	33	6279	324.60	uffily. requests after the furiously final deposits are slyly across the slyly final theodolites. requests sleep carefully. furiously special deposits by the regular platelets doze carefully 
184997	34	7018	709.09	sits use carefully against the carefully ironic requests. furiously bold somas sleep closely. quickly ironic requests about the regular excuses haggle against the quickly pending pac
185000	37	3829	603.76	fix slyly final, silent foxes. ironic asymptotes are furiously furiously unusual requests. slyly special dependencies across the unusual theodolites are busily the
185001	38	8689	8.20	ackages thrash. slyly special ideas nag blithely against the regular, regular packages? furiously regular excuse
185002	39	2858	978.10	ts haggle slyly carefully silent requests. slyly regular dependencies cajole. boldly final packages wake slyly abo
185006	43	9154	773.18	g packages. slyly final dependencies wake furiously above the final, enticing packages. blithely pending packages boost carefully about the even requests. f
185007	44	7132	59.14	ys pending deposits haggle furiously since the reque
185008	45	3051	186.20	en, final dependencies wake blithely final, even dolphins. bold
185013	50	5165	824.42	nts. final, final foxes affix carefully. carefully un
187483	2	9323	160.25	ven, sly dolphins integrate carefully furiously express deposits! regularly
187484	3	1345	46.45	ents are furiously. final foxes engage. fluffily bold do
187486	5	6255	904.70	efully. fluffily regular epitaphs cajole slowly slyly even accounts. slyly even packages unwind; always even notornis are even epitaphs. request
187490	9	945	146.16	requests boost permanently. slyly regular theodolites boost slyly according to the slyly even deposits. requests sleep alongside of the express, unusual requests. idly bold instructions engage 
187493	12	2956	931.20	tes cajole. express, express packages above the accounts dazzle carefully unusual requests. regular, ironic accounts are. furiously special requests against the packages cajole blithely at 
187494	13	4191	466.84	nal pinto beans wake. blithely regular foxes nag under the ideas. carefully final asymptotes wake beyond the id
187496	15	2638	404.10	yly special theodolites. slyly final foxes sleep whithout the final, final instr
187501	20	840	760.17	ly regular dependencies grow quickly beside the furiously even packages. furiously ironic asymptotes boost slowly slyly silent accounts. regular deposits serve blithely; b
187502	21	7151	373.47	es among the deposits are above the carefully sly instructions. slyly ironic deposits nag among the blithely final foxes. slyly u
187503	22	6546	213.56	s. blithely regular deposits cajole carefully furiously ironic theodolites
187504	23	9165	900.01	inal courts detect slyly around the even, final ideas. blithely final foxes impress carefully u
187505	24	1102	227.80	s detect quickly stealthily final accounts. even, regular deposits are q
187509	28	4252	218.60	g carefully pending deposits. special, final excuses after the slyly ironic theodolites b
187510	29	2665	480.45	sits. carefully unusual theodolites wake enticingly. ideas haggle about the regular accounts. packages sleep blithely slyly unusual pinto beans. ironic, regular theodolites among the 
187514	33	3299	440.10	ent instructions. regular, special accounts wake furiously furiously pending deposits. blithely ironic deposits across the thinly reg
187515	34	6810	413.74	ously pending deposits across the ironic instructions boost boldly fluffily regular accounts. silently unus
187518	37	7847	705.50	 regular deposits. slyly regular accounts sleep? fluffily even requests detect slyly about the pending a
187519	38	6552	265.76	ully ironic pinto beans sleep. furiously ironic packag
187520	39	4153	199.38	ideas. pending accounts about the regular accounts cajole through the requests. furiously regu
187524	43	5215	956.77	s haggle carefully final accounts. final, final instructions cajo
187525	44	3293	835.68	yly express pinto beans. asymptotes wake according to the ironic dependencies. regular foxes i
187526	45	4439	190.29	nal pinto beans cajole at the regular dolphins. blithely even deposits use. bold packages are. instructions cajole 
187531	50	134	164.59	in ideas. slyly express packages sleep thinly at the special dependencies. special requests nag slyly furiously do
190002	3	9245	382.80	accounts use stealthily among the carefully final packages. slyly slow deposits boost finally regular platelets. furiously
190004	5	9900	550.50	onic depths around the fluffily regular requests promise furiously quietly regular instructions. slyly regular accounts haggle ab
190008	9	1591	586.67	ld instructions are carefully after the quickly regular p
190011	12	3848	752.51	 even platelets are furiously express notornis. furiously final multipliers haggle furiously across the blithely even requests. fi
190012	13	2444	792.42	special deposits: fluffily regular packages wake blithely carefully regular instructions. even instructions wake furiously among 
190014	15	1294	499.68	instructions. final pinto beans according to the fluffily final hockey players nag furiously ironic theodolites. blithely final platelets integrate furiously
190019	20	2734	793.17	excuses. final excuses sleep. even, ironic deposits cajole carefully even acc
190020	21	4431	692.04	eposits cajole fluffily. slyly ironic ideas haggle above the quickly regular foxes. slyly express ideas kindle carefully slyly regular theodolites-- furiously unusual requests boost slyly
190021	22	4149	276.98	instructions boost carefully: regular packages solve furiously even ideas. furiously u
190022	23	7619	823.07	lar packages cajole closely slyly unusual ideas. deposits sleep slyly across the even pinto beans. caref
190023	24	2926	89.41	xpress pinto beans across the slyly regular ideas serve against the slyly even accounts: special, regular grouches haggle quickly after the slyly final pl
190027	28	9068	535.78	long the quickly bold requests. furiously regular theodolites are. slyly regular courts do haggle slyly. furiously pending accounts sleep. unusual packages detect
190028	29	8386	868.20	the silently special foxes. stealthily regular deposits cajole above the even p
190032	33	558	221.01	nag blithely across the carefully final theodolites. carefully even accounts are furiously fluffy deposits. furious
190033	34	2218	317.46	silently slyly final deposits. final accounts haggle. quickly regular packages along the carefully ironic ideas sublate quickly quickly special requests. carefully express 
190036	37	9327	208.93	rls above the final dolphins dazzle slyly furiously final pinto beans. ironic requests across the blithely final packages use quickly unusual instructions. 
190037	38	8299	550.38	 among the pending requests. quickly regular hockey players run pending, unusual 
190038	39	1998	8.91	kly express accounts cajole slowly. furiously regular dependencies wake blithely. fluffily express requests sleep furiously regular theodolites. silent, final fo
190042	43	6803	432.35	 packages. furiously special theodolites after the furiously silent ideas sublate daringly regular accounts. final accounts except the carefully bo
190043	44	1650	185.36	y. blithely regular gifts across the slyly regular ideas boost carefully alongside of the ideas. ironic, special pinto beans haggle furiously furiously unusual deposits. caref
190044	45	5524	391.44	ual packages haggle alongside of the carefully stealthy dependencies. even deposits haggle. pending asymptotes w
190049	50	4348	663.99	iously special deposits; furiously bold requests use blithely even orbits. courts across the regular dinos use slyly ironic packages. carefully bold foxes after the furiously 
192444	2	3194	330.64	ly. carefully unusual packages use alongside of the ironic deposits. stealthily ironic waters use about the regular deco
192445	3	1014	960.42	even attainments are. blithely final accounts about the slyly express foxes haggle express packages. excuses alongside of the furiously ironic requests cajole careful
192447	5	2576	668.75	lly ironic accounts cajole slyly. even, express braids use. slow packages
192451	9	6911	88.99	metimes carefully bold accounts. quickly pending platelets sublate after the regular, express packages. even requests detect. quickly silent sentiments haggle. slyly regular dolphins serve blithe
192454	12	467	619.27	bove the furiously pending accounts. carefully regular foxes about the quickly ironic packages wake blithely blithely even pinto beans. express, regular somas integrate furiously. unusual, 
192455	13	8343	818.21	ic, silent packages boost alongside of the ironic requests. furiously special requests use furiously. quickly regular accounts along the slyly ir
192457	15	2198	808.98	blithely. foxes above the slyly regular ideas snooze furiously careful requests. boldly quiet excuses of the stealthily unusual foxes haggle fur
192462	20	6688	65.37	arefully final deposits use around the regular, dogged pinto beans. care
192463	21	2952	668.81	g packages. slyly unusual ideas along the regular, final foxes believe slyly furiously ironic requests. i
192464	22	1927	774.18	r deposits. furiously pending requests boost slyly. blithely special deposits will have to wake against the quiet
192465	23	4805	143.12	ate blithely. express ideas grow above the final theodolites. slyly unusual packages use. enticingly ironic accounts are according to 
192466	24	5979	496.30	dolphins above the special asymptotes wake slyly according to the furiously even deposits. evenly ironic accounts haggle ruthlessly across the ir
192470	28	1722	402.63	unusual deposits haggle about the quickly stealthy accounts. blithely daring ideas sleep ruthlessly according to the regular requests. fina
192471	29	9801	879.32	ounts according to the blithely bold foxes are carefully at the enticingly even dolphins. furiously brave packages nag slyly furiously even dependenci
192475	33	1371	473.87	kages affix quickly blithely pending requests: carefully special asymptotes are above the express, ironic foxes. final, ironic accounts sleep near the blithe
192476	34	5880	914.20	ructions. regular pinto beans along the slyly final theodolites engage slyly blithely ironic pinto beans. always even theodolites use slyly blithely ironic theodolites. carefully bold 
192479	37	918	920.80	uriously slyly even ideas. pending packages detect carefully. enticingly bold deposits wake agai
192480	38	6971	746.25	deas affix furiously regular packages. blithely express accounts affix idly across the fluffily pending foxes. ironic deposits haggle furiously express requests. ironic, unusual 
192481	39	4367	35.96	, ironic theodolites-- slyly regular dependencies x-ray furiously. slyly ironic orbits are carefully bold packages. final packages are. slyly regular foxes according to the fluf
192485	43	9229	684.64	s the special packages. unusual, silent ideas haggle according to the
192486	44	1375	258.62	thely regular packages. slyly express requests are slyly. quickly express deposits haggle slyly express depths. ruthless asymptotes are slyly b
192487	45	6146	492.67	ons. even, final packages haggle fluffily. furiously regular deposits cajole furiously above the blithely even foxes. blithely even pinto beans about the closely special r
192492	50	4289	266.88	fully silent dependencies alongside of the silently even asymptotes nag furiously 
194963	2	3595	935.75	ld, even courts. furiously final pinto beans are according to
194964	3	6280	498.73	 requests are along the regular theodolites; slyly express decoys are quickly regular requests. unusual, unusual instructions doubt within t
194966	5	7431	465.57	ent frays above the slyly bold theodolites are even dolphins; blithely ironic dependencies
194970	9	292	36.14	ncies. blithely unusual hockey players cajole slyly. even asymptotes sleep stealthily alongside of the regular, special packages. carefully regular theodolites cajole ca
194973	12	6371	232.05	, even ideas. fluffily even orbits boost. quickly ironic ideas wake silently furi
194974	13	4642	545.82	e the blithely blithe ideas wake fluffily blithely final requests. slyly bold pinto beans
194976	15	4077	616.41	onic asymptotes. carefully regular hockey players doubt. fluffily regular deposits along the quickly pending accounts believe along the carefully regular courts! blithely close 
194981	20	8409	814.10	e. quickly even requests dazzle silently regular excuses. express instructions cajole express, special theodolites. pending, express pinto beans are blithely. dependencies cajole. 
194982	21	4250	966.50	ons; quickly special theodolites may haggle above the ironically final foxes. blithely ironic accounts c
194983	22	5613	445.98	fully careful sheaves. ironic foxes haggle slyly beneath the evenly unusual re
194984	23	7772	91.04	yly bold deposits. excuses sleep stealthily ironic i
194985	24	4715	835.41	ests. silently unusual instructions through the quickly even r
194989	28	7377	814.82	ully pending excuses are. accounts haggle. carefully ironic foxes integrate sly
194990	29	5126	431.46	ns will have to detect furiously. carefully final deposits boost never! even, pending requests 
194994	33	6078	456.10	ainst the quickly special foxes. final theodolites are. pending, regular asymptotes cajole behind the accounts. fluffily pending courts cajole slyly. flu
194995	34	3509	192.64	re furiously along the furiously special requests. pending, bold ideas nag against the even acc
194998	37	5429	58.91	eas. carefully bold packages solve carefully. pending pearls sleep furiously. furiously silent pinto b
194999	38	6647	796.32	ages. quickly ironic packages detect above the special deposits. quickly final foxes cajole even, regular theodolites. slyly even deposits cajole blithely even packages. deposits sleep sly
195000	39	6057	403.13	ly ironic deposits. furiously final dugouts hang blithely busily final deposits. slyly final theodolites boost furiously carefully bo
195004	43	2306	989.25	ggle. express, even ideas should cajole carefully bold courts. requests boost. furiously idle accounts wake carefully above th
195005	44	7320	519.74	r Tiresias detect furiously according to the carefully regular deposits. inst
195006	45	4294	510.51	ly furiously regular excuses. blithely special instructio
195011	50	9261	350.78	 final requests sleep quickly across the final, final dependencies: carefully pending deposits wake slyly 
197482	2	4229	779.42	quests; furiously final platelets along the blithely special deposits sleep blithely after the blithely final requests. theodol
197483	3	7574	812.22	ajole quickly. quickly regular accounts wake quickly. requests wake blithely according to the regular, ironic ideas. slyly special
197485	5	1870	749.65	layers. carefully special packages after the quickly bold instructions detect blithely regular theodolites. blithely final accounts after the stealthy dependenc
197489	9	9203	643.67	e carefully even accounts nag quietly regular, pending ideas. bold packages doze slyly. fluffil
197492	12	4823	152.00	ckages. blithely even accounts play furiously despite the ironic packages. blithely stealthy deposits are carefully quickly pending instructions: slyly pending ideas wake. furiously even pinto be
197493	13	8608	771.82	hely around the slyly pending packages. regularly unusual deposits are furiously after the dependencies. even dolphins affix. slyly regular requests wake fluffily even court
197495	15	3757	671.48	about the slyly ironic dolphins. furiously pending packages are carefully. furiously ironic co
197500	20	5288	829.83	e blithely bold instructions. final requests may cajole furiously quickly speci
197501	21	6860	330.08	elets boost furiously. silently final dolphins according to the furiousl
197502	22	1546	418.88	the furiously ironic packages. even theodolites must haggle thinly agai
197503	23	612	393.56	sly regular ideas! final theodolites boost slyly above 
197504	24	9224	212.01	sts alongside of the blithely final requests detect carefully whithout the furiously final accounts. carefully special dependencies los
197508	28	3784	16.27	regular packages wake regular ideas! courts cajole boldly. ironic, express id
197509	29	9854	870.45	 theodolites use. furiously even foxes above the unusual frays boost across the furiously regular requests. furiously daring foxes are slyly special, express 
197513	33	2427	419.79	deposits after the unusual, ruthless packages affix blithely even, regular dugouts. slyly fluffy packages wake quick
197514	34	4966	958.48	. regular packages against the furiously even packages nag carefully against the even, ironic accounts. blithely special foxes are carefully final theodo
197517	37	5098	676.95	ly even theodolites play blithely quickly final deposits. bold ideas use carefully special packages. carefully ironic requests doze blithely. furiously special foxes above the busy, regular f
197518	38	6534	124.42	. ideas wake slyly about the slowly special deposits. quickly regular requests print silent sheaves. platelets nag slyly bold accounts. slyly express
197519	39	7452	554.36	ing dependencies are furiously. even accounts cajole s
197523	43	3186	498.56	ng the even packages. pending, bold orbits cajole carefully expres
197524	44	1291	694.68	ounts wake against the silent, unusual dependencies. ironic, ironic requests nag slyly: silent packages are blithely. slyly even deposits nag furiously abo
197525	45	1491	607.21	ckages. ironic courts are express theodolites. grouches sleep carefully ironic deposits. 
197530	50	3766	212.61	even foxes. final requests affix. even deposits are slyly against the blithely regular platelets. carefully final attainments wake. unusual pinto beans about the quickly unusual deposits b
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
12	Supplier#000000012       	aLIW  q0HYd	8	18-179-925-7181	1432.69	al packages nag alongside of the bold instructions. express, daring accounts
13	Supplier#000000013       	HK71HQyWoqRWOX8GI FpgAifW,2PoH	3	13-727-620-7813	9107.22	requests engage regularly instructions. furiously special requests ar
15	Supplier#000000015       	olXVbNBfVzRqgokr1T,Ie	8	18-453-357-6394	308.56	 across the furiously regular platelets wake even deposits. quickly express she
20	Supplier#000000020       	iybAE,RmTymrZVYaFZva2SH,j	3	13-715-945-6730	530.82	n, ironic ideas would nag blithely about the slyly regular accounts. silent, expr
21	Supplier#000000021       	81CavellcrJ0PQ3CPBID0Z0JwyJm0ka5igEs	2	12-253-590-5816	9365.80	d. instructions integrate sometimes slyly pending instructions. accounts nag among the 
22	Supplier#000000022       	okiiQFk 8lm6EVX6Q0,bEcO	4	14-144-830-2814	-966.20	 ironically among the deposits. closely expre
23	Supplier#000000023       	ssetugTcXc096qlD7 2TL5crEEeS3zk	9	19-559-422-5776	5926.41	ges could have to are ironic deposits. regular, even request
24	Supplier#000000024       	C4nPvLrVmKPPabFCj	0	10-620-939-2254	9170.71	usly pending deposits. slyly final accounts run 
28	Supplier#000000028       	GBhvoRh,7YIN V	0	10-538-384-8460	-891.99	ld requests across the pinto beans are carefully against the quickly final courts. accounts sleep 
29	Supplier#000000029       	658tEqXLPvRd6xpFdqC2	1	11-555-705-5922	-811.62	y express ideas play furiously. even accounts sleep fluffily across the accounts. careful
33	Supplier#000000033       	gfeKpYw3400L0SDywXA6Ya1Qmq1w6YB9f3R	7	17-138-897-9374	8564.12	n sauternes along the regular asymptotes are regularly along the 
34	Supplier#000000034       	mYRe3KvA2O4lL4HhxDKkkrPUDPMKRCSp,Xpa	10	20-519-982-2343	237.31	eposits. slyly final deposits toward the slyly regular dependencies sleep among the excu
37	Supplier#000000037       	cqjyB5h1nV	0	10-470-144-1330	3017.47	iously final instructions. quickly special accounts hang fluffily above the accounts. deposits
38	Supplier#000000038       	xEcx45vD0FXHT7c9mvWFY	4	14-361-296-6426	2512.41	ins. fluffily special accounts haggle slyly af
39	Supplier#000000039       	ZM, nSYpEPWr1yAFHaC91qjFcijjeU5eH	8	18-851-856-5633	6115.65	le slyly requests. special packages shall are blithely. slyly unusual packages sleep 
43	Supplier#000000043       	Z5mLuAoTUEeKY5v22VnnA4D87Ao6jF2LvMYnlX8h	12	22-421-568-4862	7773.41	unts. unusual, final asymptotes 
44	Supplier#000000044       	kERxlLDnlIZJdN66zAPHklyL	7	17-713-930-5667	9759.38	x. carefully quiet account
45	Supplier#000000045       	LcKnsa8XGtIO0WYSB7hkOrH rnzRg1	9	19-189-635-8862	2944.23	iously according to the ironic, silent accounts.
50	Supplier#000000050       	rGobqSMMYz0ErrPhCGS	9	19-561-560-7437	4515.87	warhorses. ironic, regular accounts detect slyly after the quickly f
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

