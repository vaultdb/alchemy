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
1	Customer#000000001	IVhzIApeRb ot,c,E	15	25-989-741-2988	711.56	BUILDING  	to the even, regular platelets. regular, ironic epitaphs nag e
2	Customer#000000002	XSTf4,NCwDVaWNe6tEgvwfmRchLXak	13	23-768-687-3665	121.65	AUTOMOBILE	l accounts. blithely ironic theodolites integrate boldly: caref
6	Customer#000000006	sKZz0CsnMD7mp4Xd0YrBvx,LREYKUWAh yVn	20	30-114-968-4951	7638.57	AUTOMOBILE	tions. even deposits boost according to the slyly bold packages. final accounts cajole requests. furious
7	Customer#000000007	TcGe5gaZNgVePxU5kRrvXBfkasDTea	18	28-190-982-9759	9561.95	AUTOMOBILE	ainst the ironic, express theodolites. express, even pinto beans among the exp
8	Customer#000000008	I0B10bB0AymmC, 0PrRYBCP1yGJ8xcBPmWhl5	17	27-147-574-9335	6819.74	BUILDING  	among the slyly regular theodolites kindle blithely courts. carefully even theodolites haggle slyly along the ide
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
135943	98961	3980	1	37	72518.52	0.03	0.05	A	F	1993-07-07	1993-09-17	1993-07-22	DELIVER IN PERSON        	AIR       	 the final requests. 
135943	36541	4051	2	37	54668.98	0.09	0.08	R	F	1993-08-04	1993-08-28	1993-09-01	NONE                     	FOB       	the regular pac
135943	143743	6258	3	43	76829.82	0.10	0.04	A	F	1993-09-12	1993-07-22	1993-09-29	COLLECT COD              	TRUCK     	ve carefully acros
135943	74859	4860	4	28	51347.80	0.02	0.08	A	F	1993-09-27	1993-09-07	1993-10-27	DELIVER IN PERSON        	SHIP      	y final deposi
135943	153657	1203	5	34	58162.10	0.01	0.05	A	F	1993-10-12	1993-09-06	1993-10-31	TAKE BACK RETURN         	AIR       	ironic excuses are slyl
430243	155290	5291	1	23	30941.67	0.01	0.08	A	F	1995-03-28	1995-03-10	1995-04-06	COLLECT COD              	SHIP      	ter the fluffily even instructions. furi
454791	98296	5824	1	40	51771.60	0.05	0.05	R	F	1992-06-24	1992-06-20	1992-07-11	TAKE BACK RETURN         	MAIL      	 busy theodol
454791	52971	487	2	13	25011.61	0.10	0.02	A	F	1992-05-02	1992-05-19	1992-05-10	NONE                     	REG AIR   	ily silent id
579908	104617	7128	1	33	53513.13	0.00	0.01	N	O	1997-03-30	1997-01-11	1997-04-25	NONE                     	MAIL      	uickly special ideas alongside
1071617	36850	1857	1	27	48244.95	0.00	0.03	N	O	1995-07-09	1995-05-15	1995-07-16	DELIVER IN PERSON        	MAIL      	ic packages are. spe
1071617	137384	2411	2	11	15635.18	0.10	0.01	R	F	1995-04-08	1995-04-16	1995-04-13	COLLECT COD              	MAIL      	ke furiously furiously regular 
1071617	3374	8375	3	36	45985.32	0.02	0.07	N	O	1995-07-03	1995-04-26	1995-07-17	DELIVER IN PERSON        	FOB       	express requests. even patterns 
1071617	90308	309	4	30	38949.00	0.03	0.06	N	F	1995-06-12	1995-05-28	1995-06-26	DELIVER IN PERSON        	REG AIR   	l ideas. blithely ironic instructions are 
1071617	194723	2281	5	39	70891.08	0.07	0.05	A	F	1995-06-08	1995-06-03	1995-06-15	TAKE BACK RETURN         	AIR       	blithely express hockey
1073670	79911	9912	1	41	77527.31	0.09	0.08	R	F	1994-09-06	1994-07-16	1994-09-17	COLLECT COD              	AIR       	theodolites. regular deposits brea
1201223	59527	7043	1	44	65406.88	0.04	0.01	N	O	1996-02-20	1996-04-02	1996-03-19	NONE                     	FOB       	g, unusual platel
1201223	66105	8612	2	2	2142.20	0.04	0.01	N	O	1996-01-25	1996-04-04	1996-02-10	NONE                     	AIR       	kly special deposits wake regularly a
1201223	19891	2393	3	41	74246.49	0.02	0.01	N	O	1996-02-07	1996-03-31	1996-02-25	DELIVER IN PERSON        	MAIL      	ayers. ideas boost at the packages. depos
1374019	45052	7557	1	39	38884.95	0.07	0.00	A	F	1992-05-01	1992-05-20	1992-05-25	TAKE BACK RETURN         	TRUCK     	special, special deposits among the f
1374019	178552	1070	2	1	1630.55	0.10	0.00	A	F	1992-06-21	1992-05-26	1992-07-18	DELIVER IN PERSON        	AIR       	en theodolites. furiously unus
1374019	111572	6595	3	13	20586.41	0.07	0.01	R	F	1992-05-10	1992-06-25	1992-05-30	TAKE BACK RETURN         	SHIP      	furiously ac
1374019	182631	7668	4	42	71972.46	0.09	0.01	A	F	1992-07-06	1992-06-28	1992-08-05	DELIVER IN PERSON        	TRUCK     	deposits sleep across the ironic deposits
1374019	62432	9951	5	33	46016.19	0.10	0.06	A	F	1992-05-20	1992-06-03	1992-05-25	DELIVER IN PERSON        	TRUCK     	e slyly even instructions. furiously i
1485505	90924	5943	1	46	88086.32	0.09	0.00	N	O	1998-10-28	1998-09-12	1998-11-07	COLLECT COD              	REG AIR   	s instruct
1485505	150483	484	2	34	52138.32	0.06	0.03	N	O	1998-10-07	1998-09-24	1998-11-02	TAKE BACK RETURN         	AIR       	ose ideas 
1485505	151566	1567	3	6	9705.36	0.10	0.03	N	O	1998-10-10	1998-09-14	1998-11-05	NONE                     	AIR       	ests according 
1485505	168504	6053	4	32	50320.00	0.06	0.04	N	O	1998-11-18	1998-10-19	1998-11-22	TAKE BACK RETURN         	AIR       	re blithely
1485505	33239	3240	5	10	11722.30	0.04	0.01	N	O	1998-09-15	1998-10-05	1998-10-13	TAKE BACK RETURN         	RAIL      	ular deposits wake blithely across th
1485505	31850	6857	6	50	89092.50	0.05	0.02	N	O	1998-09-18	1998-10-10	1998-10-03	TAKE BACK RETURN         	RAIL      	dependencies above the f
1763205	12629	5131	1	10	15416.20	0.00	0.07	A	F	1994-10-30	1994-10-24	1994-11-23	NONE                     	REG AIR   	. fluffily regular packages 
1842406	84138	6647	1	13	14587.69	0.01	0.04	N	O	1996-11-30	1996-10-15	1996-12-01	DELIVER IN PERSON        	SHIP      	idle, bold requests wake fluff
1842406	105207	2738	2	6	7273.20	0.00	0.02	N	O	1996-09-05	1996-09-24	1996-09-11	NONE                     	REG AIR   	le blithely: express, bold accounts sl
1842406	10233	7737	3	7	8002.61	0.07	0.01	N	O	1996-10-23	1996-11-01	1996-11-14	TAKE BACK RETURN         	REG AIR   	lyly. regular ideas a
1842406	177150	2185	4	48	58903.20	0.01	0.07	N	O	1996-08-17	1996-09-07	1996-08-28	DELIVER IN PERSON        	TRUCK     	ms haggle according to the un
1842406	159230	1746	5	40	51569.20	0.05	0.06	N	O	1996-09-13	1996-10-20	1996-10-02	TAKE BACK RETURN         	RAIL      	s haggle. express, regular packages amon
1842406	166502	1535	6	19	29801.50	0.05	0.06	N	O	1996-10-17	1996-10-08	1996-11-09	DELIVER IN PERSON        	REG AIR   	onic theodolites integrate among the furio
1859778	64986	4987	1	40	78039.20	0.02	0.01	N	O	1998-03-29	1998-03-23	1998-04-01	DELIVER IN PERSON        	MAIL      	 slyly regular theodolites along
1859778	15737	3241	2	5	8263.65	0.02	0.05	N	O	1998-04-11	1998-03-07	1998-04-22	NONE                     	AIR       	grate quickly requests. 
1859778	83429	8446	3	46	64971.32	0.07	0.07	N	O	1998-02-20	1998-03-06	1998-02-27	COLLECT COD              	FOB       	 accounts. careful
1859778	190895	5934	4	27	53619.03	0.00	0.03	N	O	1998-05-19	1998-05-04	1998-05-21	NONE                     	MAIL      	arefully express theodolites sleep caref
1859778	168132	5681	5	48	57606.24	0.06	0.07	N	O	1998-02-17	1998-04-18	1998-03-19	TAKE BACK RETURN         	MAIL      	sual instructions. bo
1894087	53573	6079	1	23	35111.11	0.00	0.04	R	F	1994-03-13	1994-03-28	1994-04-05	DELIVER IN PERSON        	SHIP      	e final packages are even pinto beans. t
1925447	63293	812	1	35	43970.15	0.05	0.02	N	O	1997-06-04	1997-04-13	1997-06-19	DELIVER IN PERSON        	FOB       	iously even instructions. finally bold a
1925447	152744	290	2	3	5390.22	0.10	0.00	N	O	1997-06-14	1997-05-18	1997-07-06	TAKE BACK RETURN         	AIR       	ss the furiou
1925447	161053	6086	3	20	22281.00	0.07	0.04	N	O	1997-04-12	1997-05-20	1997-04-18	TAKE BACK RETURN         	TRUCK     	 special deposits. 
1925447	17020	7021	4	8	7496.16	0.09	0.02	N	O	1997-05-19	1997-03-27	1997-06-05	TAKE BACK RETURN         	RAIL      	ly carefully even th
1925447	49532	2037	5	26	38519.78	0.02	0.08	N	O	1997-06-12	1997-05-01	1997-06-28	DELIVER IN PERSON        	AIR       	ole regular instructions. blithely special
1925447	68539	1046	6	12	18090.36	0.02	0.00	N	O	1997-03-13	1997-04-15	1997-03-26	COLLECT COD              	MAIL      	s haggle bli
1959075	56184	6185	1	27	30784.86	0.01	0.04	R	F	1995-02-26	1995-03-08	1995-03-24	DELIVER IN PERSON        	REG AIR   	mong the ironic, final tithes. furiou
1959075	70761	3269	2	5	8658.80	0.01	0.03	A	F	1995-03-23	1995-03-10	1995-04-22	TAKE BACK RETURN         	RAIL      	al foxes affix. furious deposits
1959075	69504	9505	3	49	72201.50	0.01	0.04	A	F	1995-02-22	1995-02-01	1995-03-14	COLLECT COD              	TRUCK     	 asymptotes wake daringly
1959075	152282	9828	4	37	49368.36	0.08	0.06	A	F	1995-03-20	1995-02-08	1995-03-24	DELIVER IN PERSON        	RAIL      	ly. gifts sleep 
1959075	198170	8171	5	24	30436.08	0.10	0.07	A	F	1995-03-03	1995-03-15	1995-03-26	NONE                     	AIR       	 requests are slyly. requests run qui
2014848	15065	7567	1	44	43122.64	0.00	0.06	N	O	1997-03-28	1997-04-01	1997-04-06	COLLECT COD              	SHIP      	iously thin pinto beans sleep bli
2014848	193155	3156	2	30	37444.50	0.09	0.04	N	O	1997-03-21	1997-02-24	1997-03-23	DELIVER IN PERSON        	AIR       	sly final instructions. regular 
2014848	140639	8182	3	48	80622.24	0.01	0.05	N	O	1997-02-01	1997-02-27	1997-02-28	NONE                     	SHIP      	ages. furio
2014848	112844	378	4	9	16711.56	0.09	0.08	N	O	1997-04-07	1997-03-26	1997-04-12	COLLECT COD              	TRUCK     	sly bold p
2096544	139461	9462	1	20	30009.20	0.02	0.08	R	F	1992-05-26	1992-05-22	1992-06-05	NONE                     	RAIL      	ng to the furiously even
2096544	109740	2251	2	13	22746.62	0.05	0.08	A	F	1992-05-14	1992-06-22	1992-06-03	COLLECT COD              	TRUCK     	ers haggle unusual epitaphs. furiou
2096544	184592	9629	3	4	6706.36	0.06	0.06	R	F	1992-04-19	1992-05-31	1992-04-21	TAKE BACK RETURN         	RAIL      	ong the regular, express pac
2096544	25429	434	4	9	12189.78	0.04	0.01	R	F	1992-07-25	1992-07-15	1992-08-17	COLLECT COD              	RAIL      	xes. furiously thin accounts engage furiou
2096544	179883	9884	5	23	45146.24	0.02	0.05	R	F	1992-05-08	1992-06-24	1992-05-26	COLLECT COD              	REG AIR   	es. regular deposits
2096544	79598	2106	6	9	14198.31	0.02	0.06	A	F	1992-07-19	1992-05-24	1992-07-25	DELIVER IN PERSON        	SHIP      	s the ironic, final accounts. carefully b
2096544	813	814	7	31	53128.11	0.07	0.02	R	F	1992-06-23	1992-06-29	1992-07-11	NONE                     	REG AIR   	ickly around the idle, even foxes. carefull
2415204	117488	5022	1	49	73768.52	0.06	0.04	R	F	1994-11-17	1995-01-22	1994-12-02	NONE                     	AIR       	counts. silent excuses
2415204	17342	2345	2	27	34002.18	0.09	0.01	R	F	1995-01-10	1995-01-01	1995-01-27	TAKE BACK RETURN         	RAIL      	ously unusual theodolite
2415204	62424	4931	3	5	6932.10	0.03	0.06	A	F	1994-11-20	1994-12-26	1994-12-07	NONE                     	MAIL      	ests. blithely ironic pac
2415204	149265	4294	4	7	9199.82	0.05	0.02	R	F	1995-01-02	1995-01-03	1995-01-21	NONE                     	FOB       	. finally ironic deposits detect carefull
2415204	87718	227	5	40	68228.40	0.04	0.00	R	F	1995-01-06	1994-12-27	1995-01-08	COLLECT COD              	REG AIR   	riously. sl
2943521	157394	4940	1	21	30479.19	0.00	0.04	N	O	1998-02-21	1998-04-21	1998-02-22	COLLECT COD              	AIR       	 the bold, special pinto
2992930	84928	7437	1	50	95646.00	0.00	0.08	A	F	1994-09-12	1994-08-18	1994-10-03	NONE                     	AIR       	 slyly final instr
3038880	25391	2898	1	11	14480.29	0.06	0.07	N	O	1995-09-24	1995-11-18	1995-09-28	NONE                     	TRUCK     	ly pending instructions a
3038880	142394	2395	2	42	60328.38	0.10	0.04	N	O	1995-10-30	1995-10-18	1995-11-29	TAKE BACK RETURN         	AIR       	luffily special foxes nag blith
3038880	603	604	3	43	64654.80	0.07	0.07	N	O	1995-10-21	1995-10-16	1995-10-23	DELIVER IN PERSON        	FOB       	 regular account
3038880	124899	9924	4	3	5771.67	0.01	0.02	N	O	1995-11-12	1995-10-24	1995-12-05	COLLECT COD              	AIR       	ecial requests ca
3038880	173759	8794	5	31	56815.25	0.09	0.05	N	O	1995-12-13	1995-10-10	1995-12-16	NONE                     	REG AIR   	special accounts detect beyond the slyly
3038880	151513	6544	6	41	64144.91	0.02	0.05	N	O	1995-10-06	1995-10-28	1995-10-08	NONE                     	SHIP      	onic requests; final, bold packages beneath
3038880	72629	7644	7	41	65666.42	0.05	0.06	N	O	1995-10-21	1995-10-22	1995-11-04	DELIVER IN PERSON        	FOB       	ial deposits. slow, special deposits h
3211909	153053	5569	1	5	5530.25	0.00	0.04	R	F	1993-09-25	1993-08-02	1993-10-06	TAKE BACK RETURN         	AIR       	egular packages integrate 
3211909	155177	208	2	4	4928.68	0.08	0.04	A	F	1993-10-14	1993-07-30	1993-11-02	DELIVER IN PERSON        	FOB       	 furiously speci
3211909	48262	8263	3	8	9682.08	0.00	0.08	R	F	1993-09-14	1993-08-31	1993-10-07	COLLECT COD              	RAIL      	tions are slyly regular requests. 
3211909	131471	6498	4	22	33054.34	0.00	0.04	R	F	1993-08-14	1993-08-10	1993-08-16	DELIVER IN PERSON        	SHIP      	e final, pending accounts. ironic account
3211909	18132	3135	5	35	36754.55	0.03	0.07	R	F	1993-10-17	1993-08-11	1993-11-08	DELIVER IN PERSON        	FOB       	unts according to the slyly regular pinto 
3211909	140675	5704	6	29	49754.43	0.09	0.02	R	F	1993-08-05	1993-09-08	1993-08-07	COLLECT COD              	TRUCK     	packages nag fluffily. care
3211909	113087	8110	7	43	47303.44	0.01	0.02	A	F	1993-08-06	1993-09-10	1993-08-14	COLLECT COD              	SHIP      	. quickly regular requests boost sl
3318789	3143	5644	1	33	34522.62	0.09	0.07	A	F	1992-07-21	1992-05-31	1992-08-15	TAKE BACK RETURN         	AIR       	lar, regular accounts nod 
3318789	15884	5885	2	33	59396.04	0.03	0.02	R	F	1992-08-15	1992-06-05	1992-08-22	COLLECT COD              	REG AIR   	ly ironic dependenc
3318789	154701	9732	3	15	26335.50	0.09	0.05	A	F	1992-05-31	1992-06-21	1992-06-29	NONE                     	FOB       	ackages haggle furio
3318789	52262	9778	4	39	47356.14	0.01	0.06	R	F	1992-07-23	1992-07-26	1992-08-07	DELIVER IN PERSON        	FOB       	ses. blithely regular pac
3318789	167000	9517	5	14	14938.00	0.03	0.01	A	F	1992-08-18	1992-06-26	1992-09-11	DELIVER IN PERSON        	AIR       	requests. quietly unusual foxes 
3318789	155063	94	6	26	29069.56	0.09	0.04	R	F	1992-07-07	1992-06-10	1992-07-26	TAKE BACK RETURN         	AIR       	 bold packages 
3318789	134062	4063	7	36	39458.16	0.01	0.03	R	F	1992-08-13	1992-07-03	1992-09-07	DELIVER IN PERSON        	FOB       	y regular ideas are. furiously unus
3431909	164146	9179	1	29	35094.06	0.05	0.01	N	O	1997-05-25	1997-04-23	1997-06-12	DELIVER IN PERSON        	FOB       	to beans-- pending requests use
3431909	195035	5036	2	38	42941.14	0.09	0.08	N	O	1997-03-05	1997-03-31	1997-03-26	TAKE BACK RETURN         	MAIL      	r packages wake blithely c
3431909	136939	4479	3	36	71133.48	0.00	0.07	N	O	1997-04-05	1997-04-27	1997-05-01	NONE                     	SHIP      	beans breach furiously near the furious
3692388	29806	9807	1	15	26037.00	0.02	0.08	N	O	1997-05-05	1997-03-30	1997-05-14	NONE                     	MAIL      	special pl
3868359	68383	8384	1	36	48649.68	0.04	0.05	A	F	1992-10-29	1992-10-18	1992-10-30	NONE                     	RAIL      	ts. carefully express ideas run quickly c
3868359	105649	3180	2	9	14891.76	0.05	0.03	R	F	1992-10-27	1992-09-27	1992-11-26	DELIVER IN PERSON        	MAIL      	s wake furiously. platele
3868359	39769	2273	3	32	54680.32	0.07	0.01	R	F	1992-11-29	1992-10-13	1992-12-02	TAKE BACK RETURN         	REG AIR   	c ideas are quickly ironic 
3868359	194361	9400	4	6	8732.16	0.09	0.02	A	F	1992-11-03	1992-10-05	1992-11-21	COLLECT COD              	REG AIR   	s are. furiously final foxes are spec
3986496	14260	4261	1	32	37576.32	0.02	0.04	N	O	1997-06-04	1997-04-01	1997-06-12	DELIVER IN PERSON        	FOB       	olites. packages bel
3986496	57360	2371	2	21	27664.56	0.03	0.03	N	O	1997-04-10	1997-05-14	1997-04-18	COLLECT COD              	REG AIR   	lar dependencies
3986496	36515	4025	3	49	71123.99	0.01	0.08	N	O	1997-03-06	1997-04-13	1997-03-09	NONE                     	AIR       	y regular foxes. furiously e
3986496	94207	6717	4	30	36036.00	0.03	0.06	N	O	1997-04-03	1997-04-04	1997-04-30	DELIVER IN PERSON        	FOB       	slyly ironic foxes 
3986496	128461	8462	5	28	41704.88	0.04	0.07	N	O	1997-06-09	1997-05-09	1997-06-27	TAKE BACK RETURN         	SHIP      	y special t
3986496	41797	6806	6	14	24343.06	0.10	0.05	N	O	1997-03-28	1997-04-01	1997-04-02	NONE                     	REG AIR   	ly according to the carefull
3986496	76510	9018	7	44	65406.44	0.04	0.08	N	O	1997-03-08	1997-03-31	1997-04-02	TAKE BACK RETURN         	TRUCK     	ress requests solve b
3995111	189114	1633	1	25	30077.75	0.08	0.07	R	F	1994-03-01	1994-03-15	1994-03-03	TAKE BACK RETURN         	SHIP      	to the closely express 
3995111	29201	4206	2	34	38426.80	0.09	0.00	A	F	1994-06-13	1994-03-18	1994-06-30	TAKE BACK RETURN         	RAIL      	er express accoun
3995111	177916	2951	3	14	27914.74	0.05	0.03	A	F	1994-02-17	1994-05-07	1994-03-01	TAKE BACK RETURN         	RAIL      	 furiously regular ideas. furiously re
3995111	24407	4408	4	16	21302.40	0.02	0.01	A	F	1994-04-06	1994-04-03	1994-04-13	DELIVER IN PERSON        	REG AIR   	ly ironic the
3995111	192174	7213	5	13	16460.21	0.02	0.07	R	F	1994-05-14	1994-04-26	1994-06-09	NONE                     	MAIL      	ake slyly aga
3995111	59999	7515	6	5	9794.95	0.06	0.07	A	F	1994-05-09	1994-03-19	1994-05-10	COLLECT COD              	REG AIR   	 quickly carefully silent pinto beans.
4163013	199414	1934	1	4	6053.64	0.04	0.05	R	F	1993-12-10	1994-01-14	1993-12-18	TAKE BACK RETURN         	SHIP      	heodolites haggle. final packages 
4163013	150895	896	2	41	79781.49	0.10	0.04	R	F	1994-02-19	1994-02-05	1994-03-12	DELIVER IN PERSON        	MAIL      	 carefully quickly even packages. final 
4208674	93319	8338	1	27	35432.37	0.02	0.07	N	F	1995-05-27	1995-07-11	1995-06-24	COLLECT COD              	RAIL      	ly express pinto beans boost slyly. 
4208674	5706	8207	2	25	40292.50	0.04	0.02	N	O	1995-07-02	1995-06-20	1995-07-18	NONE                     	MAIL      	serve carefully ironic dependen
4208674	84394	1919	3	10	13783.90	0.02	0.04	R	F	1995-05-24	1995-07-23	1995-06-06	NONE                     	REG AIR   	lar, even deposits wak
4208674	2853	354	4	41	71989.85	0.06	0.00	N	F	1995-06-02	1995-06-20	1995-06-30	DELIVER IN PERSON        	MAIL      	carefully special deposits
4225824	59760	7276	1	10	17197.60	0.06	0.03	N	O	1995-10-31	1995-09-17	1995-11-18	DELIVER IN PERSON        	REG AIR   	ly ironic accounts about 
4225824	183436	3437	2	22	33427.46	0.06	0.06	N	O	1995-11-11	1995-09-05	1995-12-02	DELIVER IN PERSON        	SHIP      	special theod
4225824	2683	184	3	1	1585.68	0.02	0.05	N	O	1995-10-07	1995-09-09	1995-10-16	COLLECT COD              	MAIL      	luffily closely
4225824	88129	638	4	50	55856.00	0.01	0.07	N	O	1995-08-09	1995-08-26	1995-08-27	TAKE BACK RETURN         	FOB       	s are quickly even accounts. blithely 
4273923	127174	9687	1	28	33632.76	0.09	0.07	N	O	1997-07-04	1997-06-10	1997-07-25	NONE                     	REG AIR   	asymptotes try to nag fluffily acros
4273923	12564	5066	2	42	62015.52	0.03	0.05	N	O	1997-06-06	1997-06-16	1997-06-13	COLLECT COD              	AIR       	ly even packages: ironic as
4328998	89415	9416	1	38	53367.58	0.10	0.02	A	F	1992-05-14	1992-05-26	1992-05-31	TAKE BACK RETURN         	MAIL      	 final foxes detect unus
4328998	195864	8384	2	7	13719.02	0.08	0.02	R	F	1992-06-11	1992-05-31	1992-06-19	NONE                     	AIR       	 close pac
4328998	71586	9108	3	14	21806.12	0.01	0.03	A	F	1992-07-23	1992-06-19	1992-08-13	TAKE BACK RETURN         	TRUCK     	fluffily regular packages hagg
4328998	142508	7537	4	23	35661.50	0.03	0.04	R	F	1992-04-05	1992-06-23	1992-04-17	TAKE BACK RETURN         	MAIL      	ully special deposits. bold ideas s
4328998	119854	9855	5	39	73080.15	0.01	0.07	R	F	1992-06-16	1992-05-21	1992-07-08	NONE                     	MAIL      	kages are. ev
4328998	178840	1358	6	47	90185.48	0.09	0.05	A	F	1992-06-01	1992-05-08	1992-06-24	DELIVER IN PERSON        	FOB       	old instructions. slyly special 
4328998	160194	2711	7	32	40134.08	0.08	0.05	R	F	1992-06-11	1992-04-29	1992-06-29	TAKE BACK RETURN         	AIR       	nal theodolites. fluffily unusual acc
4550145	95701	5702	1	47	79744.90	0.02	0.01	N	O	1995-10-11	1995-08-12	1995-11-02	COLLECT COD              	FOB       	ending foxes. i
4550145	44869	9878	2	34	61671.24	0.07	0.07	N	O	1995-07-01	1995-07-17	1995-07-08	DELIVER IN PERSON        	MAIL      	e quickly iro
4659813	85060	2585	1	7	7315.42	0.01	0.03	R	F	1992-08-27	1992-09-11	1992-08-29	COLLECT COD              	FOB       	counts. slyly final
4808192	118792	8793	1	26	47080.54	0.05	0.08	N	O	1996-07-26	1996-09-07	1996-08-03	TAKE BACK RETURN         	REG AIR   	inal deposits haggle b
4808192	121572	9109	2	11	17529.27	0.03	0.01	N	O	1996-09-15	1996-09-04	1996-09-25	COLLECT COD              	MAIL      	g, final requests sleep quickly fluffi
4860421	171296	8848	1	12	16407.48	0.06	0.08	N	O	1996-01-14	1995-12-31	1996-01-16	COLLECT COD              	MAIL      	etect furiously about the accounts.
4860421	164298	6815	2	41	55853.89	0.08	0.03	N	O	1995-12-31	1995-12-21	1996-01-16	COLLECT COD              	MAIL      	gside of the foxes. slyly ironic ac
4860421	150892	3408	3	5	9714.45	0.06	0.03	N	O	1995-11-04	1995-12-13	1995-11-11	TAKE BACK RETURN         	SHIP      	thely regular packages across 
4860421	32413	7420	4	4	5381.64	0.06	0.02	N	O	1995-11-08	1995-12-17	1995-11-28	NONE                     	REG AIR   	ly. quickly unusual pinto beans 
4860421	160771	5804	5	39	71439.03	0.08	0.03	N	O	1996-01-21	1995-12-09	1996-02-18	TAKE BACK RETURN         	FOB       	 along the regular, ironic theodolites. c
4860421	63006	525	6	29	28101.00	0.03	0.07	N	O	1995-12-23	1995-11-28	1996-01-12	DELIVER IN PERSON        	AIR       	carefully acc
4860421	140576	577	7	6	9699.42	0.03	0.05	N	O	1995-12-13	1995-12-01	1996-01-10	NONE                     	AIR       	ss the final foxes. fluffily fi
5090183	123589	3590	1	14	22576.12	0.05	0.04	N	O	1996-10-08	1996-08-05	1996-10-10	NONE                     	RAIL      	cuses. quickly special requests wake: de
5090183	150056	7602	2	20	22121.00	0.08	0.06	N	O	1996-09-17	1996-07-23	1996-09-22	NONE                     	REG AIR   	osits nag slyly across the sl
5090183	190935	5974	3	18	36466.74	0.08	0.05	N	O	1996-09-02	1996-07-11	1996-09-07	TAKE BACK RETURN         	REG AIR   	nic, express theodolites h
5091364	198182	3221	1	5	6400.90	0.05	0.00	N	O	1997-01-23	1996-12-16	1997-02-06	NONE                     	SHIP      	ounts haggle about the quickly final
5091364	41547	9060	2	23	34236.42	0.06	0.04	N	O	1997-01-03	1997-01-09	1997-02-02	DELIVER IN PERSON        	REG AIR   	 furiously alon
5133509	196216	6217	1	28	36741.88	0.02	0.03	N	O	1996-08-19	1996-09-07	1996-09-18	DELIVER IN PERSON        	REG AIR   	encies nag quickly regular, special de
5133509	198707	6265	2	35	63199.50	0.06	0.05	N	O	1996-08-12	1996-09-24	1996-08-28	COLLECT COD              	REG AIR   	 haggle furiously acr
5133509	60989	6002	3	12	23399.76	0.05	0.00	N	O	1996-09-28	1996-08-07	1996-10-24	NONE                     	TRUCK     	the, final dependencies sleep. f
5133509	31696	1697	4	33	53713.77	0.07	0.06	N	O	1996-08-16	1996-09-16	1996-08-30	NONE                     	SHIP      	ites use across the fluffily regula
5805349	161584	1585	1	11	18101.38	0.08	0.07	N	O	1998-01-28	1998-02-27	1998-02-22	NONE                     	MAIL      	n packages. furiously pending dep
5805349	131435	1436	2	50	73321.50	0.07	0.07	N	O	1998-04-16	1998-03-15	1998-05-02	TAKE BACK RETURN         	FOB       	e among the caref
5805349	15596	3100	3	19	28720.21	0.04	0.05	N	O	1998-04-30	1998-03-12	1998-05-05	DELIVER IN PERSON        	TRUCK     	deas. even foxes are furiously. blit
5805349	109441	4462	4	16	23207.04	0.09	0.02	N	O	1998-04-25	1998-03-04	1998-04-29	COLLECT COD              	TRUCK     	ully pending theodolites nag furiously alo
5805349	170542	543	5	29	46763.66	0.08	0.04	N	O	1998-04-10	1998-03-02	1998-05-01	TAKE BACK RETURN         	SHIP      	nt courts. fi
5805349	94540	9559	6	29	44501.66	0.00	0.06	N	O	1998-03-16	1998-03-16	1998-03-25	DELIVER IN PERSON        	FOB       	ress foxes use against the carefully ir
5805349	118560	8561	7	14	22099.84	0.08	0.08	N	O	1998-02-12	1998-02-23	1998-02-23	DELIVER IN PERSON        	REG AIR   	s. blithely express accounts snooze s
5987111	189145	9146	1	24	29619.36	0.09	0.07	A	F	1992-10-04	1992-11-21	1992-10-24	TAKE BACK RETURN         	MAIL      	. slyly iro
5987111	151273	8819	2	4	5297.08	0.01	0.05	R	F	1992-11-24	1992-11-01	1992-12-07	NONE                     	RAIL      	lithely unusual instructions wake sly
5987111	42842	355	3	36	64254.24	0.06	0.05	A	F	1992-10-01	1992-10-29	1992-10-24	TAKE BACK RETURN         	SHIP      	ic pinto beans are blithely. i
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
135943	7	F	314307.46	1993-06-22	4-NOT SPECIFIED	Clerk#000000685	0	ies. blithely pending deposits 	1993
430243	2	F	33082.83	1994-12-24	4-NOT SPECIFIED	Clerk#000000121	0	n braids among the final packages cajole pendin	1994
454791	1	F	74602.81	1992-04-19	1-URGENT       	Clerk#000000815	0	uriously ironic deposits wake across the s	1992
579908	1	O	54048.26	1996-12-09	5-LOW          	Clerk#000000783	0	 asymptotes. regular requests boost blithely. pending gifts according to the	1996
1071617	2	P	221397.35	1995-03-10	2-HIGH         	Clerk#000000408	0	al, unusual packages. furiously final requests use quickly. f	1995
1073670	8	F	76193.83	1994-05-24	4-NOT SPECIFIED	Clerk#000000710	0	ly alongside of the slyly ironic theodolites. accoun	1994
1201223	8	O	138984.74	1996-01-13	5-LOW          	Clerk#000000287	0	st the carefully final foxes wake carefully according to the	1996
1374019	2	F	167016.61	1992-04-05	1-URGENT       	Clerk#000000440	0	lithe packages. acco	1992
1485505	7	O	286525.11	1998-07-24	1-URGENT       	Clerk#000000228	0	posits. final packages wake carefully express ideas. carefully bold deposits a	1998
1763205	2	F	16495.33	1994-08-28	1-URGENT       	Clerk#000000450	0	ven requests. blithely express excuses nag or	1994
1842406	2	O	174291.41	1996-08-05	1-URGENT       	Clerk#000000521	0	s. excuses lose. pen	1996
1859778	8	O	263567.37	1998-02-04	3-MEDIUM       	Clerk#000000731	0	counts boost slyly. express pinto beans use. furious	1998
1894087	7	F	36515.55	1994-02-20	1-URGENT       	Clerk#000000343	0	 bold, even request	1994
1925447	8	O	134464.25	1997-02-19	4-NOT SPECIFIED	Clerk#000000348	0	nts snooze furiously according to the carefully bold ideas. sometimes regular	1997
1959075	7	F	192318.07	1994-12-23	1-URGENT       	Clerk#000000590	0	long the regular instructions. bli	1994
2014848	7	O	181378.37	1997-01-19	5-LOW          	Clerk#000000333	0	lets use pending accounts. qu	1997
2096544	7	F	185203.14	1992-04-17	1-URGENT       	Clerk#000000964	0	ts. final, silent theodoli	1992
2415204	8	F	184908.92	1994-11-03	5-LOW          	Clerk#000000368	0	inal requests are final, bold dolph	1994
2943521	7	O	31698.35	1998-02-18	1-URGENT       	Clerk#000000675	0	kly against the qui	1998
2992930	2	F	103297.68	1994-05-21	4-NOT SPECIFIED	Clerk#000000913	0	owly slyly final platelets. ironic instructions wake carefully unusual plate	1994
3038880	7	O	327615.97	1995-09-06	2-HIGH         	Clerk#000000458	0	y careful ideas. final, regular theodolites haggle. quic	1995
3211909	7	F	187396.96	1993-06-29	2-HIGH         	Clerk#000000918	0	 slyly according to the even theodolites. slyly ironic requests nag carefu	1993
3318789	7	F	249621.82	1992-04-30	3-MEDIUM       	Clerk#000000331	0	ly blithely regular deposits. furiously regular accounts s	1992
3431909	7	O	151988.10	1997-02-04	2-HIGH         	Clerk#000000744	0	ose carefully. slyly unusual ins	1997
3692388	8	O	27557.56	1997-02-11	4-NOT SPECIFIED	Clerk#000000060	0	s packages are slyly around the regula	1997
3868359	1	F	123076.84	1992-08-22	5-LOW          	Clerk#000000536	0	. furiously bold asymptotes are instructions. quickly ironic dep	1992
3986496	2	O	312692.22	1997-02-22	1-URGENT       	Clerk#000000768	0	. theodolites nag fluffily above the blithely iron	1997
3995111	7	F	140088.51	1994-02-13	3-MEDIUM       	Clerk#000000724	0	ts. slyly ironic requests nag blithely. instruct	1994
4163013	7	F	80777.53	1993-11-14	3-MEDIUM       	Clerk#000000392	0	t slyly. furiously silent packag	1993
4208674	8	P	158327.78	1995-05-02	3-MEDIUM       	Clerk#000000657	0	riously-- final theodolites int	1995
4225824	8	O	110757.73	1995-07-21	4-NOT SPECIFIED	Clerk#000000060	0	furiously final packages print slyly express accounts. r	1995
4273923	1	O	95911.01	1997-03-23	3-MEDIUM       	Clerk#000000381	0	ep fluffily along the carefull	1997
4328998	7	F	322431.85	1992-03-25	4-NOT SPECIFIED	Clerk#000000521	0	nto beans according to the regular accounts cajole per	1992
4550145	8	O	140300.54	1995-06-13	3-MEDIUM       	Clerk#000000554	0	ly fluffy requests. frets affix courts. packages play above the carefully fin	1995
4659813	8	F	7459.52	1992-07-26	4-NOT SPECIFIED	Clerk#000000133	0	ely regular accounts haggle carefully regular pains. ironi	1992
4808192	1	O	65478.05	1996-06-29	2-HIGH         	Clerk#000000473	0	eposits use against the express accounts. carefully regular ideas sleep blithe	1996
4860421	7	O	190889.91	1995-10-24	5-LOW          	Clerk#000000572	0	 express pinto beans. requests believe slyly ex	1995
5090183	7	O	79104.46	1996-06-10	2-HIGH         	Clerk#000000046	0	as. furiously bold requests according to the quickly ironic packages c	1996
5091364	8	O	39550.36	1996-11-12	5-LOW          	Clerk#000000894	0	mptotes after the slyly final asymptotes nag quickly before the carefully ex	1996
5133509	1	O	174645.94	1996-07-01	1-URGENT       	Clerk#000000463	0	al accounts could have to cajole furiously even	1996
5805349	8	O	255145.52	1998-01-01	1-URGENT       	Clerk#000000956	0	ites doze special, express deposits. 	1998
5987111	8	F	97765.58	1992-09-03	2-HIGH         	Clerk#000000822	0	nding pinto beans. carefully pending packages sleep quickly regular dolphi	1992
\.


--
-- Data for Name: part; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.part (p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment) FROM stdin;
3	spring green yellow purple cornsilk	Manufacturer#4           	Brand#42  	STANDARD POLISHED BRASS	21	WRAP CASE 	903.00	egular deposits hag
5	forest brown coral puff cream	Manufacturer#3           	Brand#32  	STANDARD POLISHED TIN	15	SM PKG    	905.00	 wake carefully 
6	bisque cornflower lawn forest magenta	Manufacturer#2           	Brand#24  	PROMO PLATED STEEL	4	MED BAG   	906.00	sual a
7	moccasin green thistle khaki floral	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	45	SM BAG    	907.00	lyly. ex
9	thistle dim navajo dark gainsboro	Manufacturer#4           	Brand#43  	SMALL BURNISHED STEEL	12	WRAP CASE 	909.00	ironic foxe
2500	wheat rose grey khaki floral	Manufacturer#2           	Brand#25  	STANDARD PLATED TIN	43	JUMBO BAG 	1402.50	e furiously aga
2503	saddle steel aquamarine antique orchid	Manufacturer#4           	Brand#44  	ECONOMY POLISHED BRASS	6	SM CASE   	1405.50	kly ironic d
2505	steel indian misty cyan navy	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED TIN	37	LG BOX    	1407.50	 solve slyly
2506	plum chartreuse lemon ghost dim	Manufacturer#2           	Brand#24  	PROMO ANODIZED BRASS	24	WRAP PACK 	1408.50	r the blithel
2507	spring misty smoke moccasin navajo	Manufacturer#1           	Brand#13  	SMALL POLISHED BRASS	34	MED DRUM  	1409.50	e final fox
2509	powder wheat plum peach deep	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	17	JUMBO BAG 	1411.50	dolphi
5000	lace antique light cream grey	Manufacturer#3           	Brand#34  	MEDIUM BRUSHED BRASS	31	LG JAR    	905.00	ow accounts.
5003	bisque moccasin plum snow olive	Manufacturer#4           	Brand#42  	ECONOMY POLISHED TIN	35	LG DRUM   	908.00	 reque
5005	purple hot light floral peach	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED BRASS	31	WRAP PKG  	910.00	ve the s
5006	smoke frosted wheat steel sky	Manufacturer#2           	Brand#22  	LARGE BRUSHED BRASS	48	JUMBO DRUM	911.00	iously. theo
5007	drab lavender coral mint black	Manufacturer#3           	Brand#33  	MEDIUM PLATED BRASS	10	LG CASE   	912.00	ly re
5009	brown burnished coral smoke plum	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED TIN	37	LG PACK   	914.00	ackages. depo
7500	azure royal aquamarine peach goldenrod	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED TIN	41	WRAP DRUM 	1407.50	 fluffily. sp
7503	forest red maroon olive cornsilk	Manufacturer#2           	Brand#21  	LARGE BURNISHED COPPER	7	JUMBO CASE	1410.50	ggle blithely. ideas 
7505	ghost rosy olive tomato burnished	Manufacturer#1           	Brand#13  	LARGE POLISHED BRASS	23	WRAP CASE 	1412.50	deas c
7506	green linen royal lavender azure	Manufacturer#5           	Brand#54  	LARGE PLATED STEEL	48	SM PKG    	1413.50	eas after the excus
7507	slate blush peach indian light	Manufacturer#3           	Brand#35  	LARGE ANODIZED NICKEL	35	JUMBO PKG 	1414.50	ithely pending package
7509	papaya rose turquoise lawn magenta	Manufacturer#3           	Brand#32  	LARGE ANODIZED NICKEL	35	LG DRUM   	1416.50	ts. slyly silent 
10000	navajo lace black steel brown	Manufacturer#3           	Brand#35  	LARGE POLISHED NICKEL	29	MED BAG   	910.00	efully even deposits! 
10003	spring sky moccasin orchid forest	Manufacturer#2           	Brand#23  	STANDARD PLATED COPPER	30	WRAP CASE 	913.00	s. pending, fin
10005	sky mint magenta black misty	Manufacturer#3           	Brand#35  	MEDIUM POLISHED COPPER	4	WRAP PACK 	915.00	hely unusual pac
10006	azure metallic violet puff saddle	Manufacturer#2           	Brand#25  	LARGE ANODIZED TIN	28	JUMBO PKG 	916.00	thely. quickly pend
10007	burlywood ivory pink yellow forest	Manufacturer#2           	Brand#25  	PROMO POLISHED BRASS	2	SM CAN    	917.00	nt theodolites wake al
10009	indian smoke lime magenta tan	Manufacturer#4           	Brand#44  	SMALL ANODIZED BRASS	47	MED CASE  	919.00	ronic acco
12497	cornsilk rose yellow turquoise lime	Manufacturer#5           	Brand#52  	MEDIUM PLATED NICKEL	35	LG BAG    	1409.49	gular deposits wa
12500	gainsboro frosted cyan floral blanched	Manufacturer#4           	Brand#41  	ECONOMY POLISHED STEEL	43	LG DRUM   	1412.50	nal excuse
12502	frosted chocolate brown tomato goldenrod	Manufacturer#1           	Brand#12  	ECONOMY POLISHED BRASS	31	WRAP CASE 	1414.50	ts play a
12503	magenta hot dark almond ghost	Manufacturer#2           	Brand#25  	STANDARD BRUSHED STEEL	29	JUMBO JAR 	1415.50	ironic req
12504	forest antique khaki linen hot	Manufacturer#2           	Brand#22  	STANDARD ANODIZED COPPER	46	WRAP JAR  	1416.50	usly 
12506	navy ivory rose turquoise blanched	Manufacturer#4           	Brand#43  	LARGE PLATED TIN	45	MED BAG   	1418.50	es are. permanent, fin
15001	green pink dodger violet khaki	Manufacturer#5           	Brand#52  	PROMO BURNISHED BRASS	38	WRAP DRUM 	916.00	efully. unusual r
14998	saddle indian mint olive wheat	Manufacturer#3           	Brand#32  	PROMO ANODIZED BRASS	41	JUMBO PACK	1912.99	ole slyly furiou
15003	spring cornsilk smoke orange lace	Manufacturer#1           	Brand#14  	LARGE BRUSHED TIN	13	MED BOX   	918.00	ding d
15004	puff purple light lemon grey	Manufacturer#5           	Brand#53  	MEDIUM POLISHED NICKEL	2	WRAP JAR  	919.00	after the slyl
15005	bisque chiffon puff saddle khaki	Manufacturer#5           	Brand#51  	PROMO ANODIZED NICKEL	14	WRAP BOX  	920.00	 wake slyly ironic pa
15007	firebrick aquamarine gainsboro almond moccasin	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED BRASS	37	JUMBO PKG 	922.00	ng packages. regul
17499	almond rosy gainsboro plum rose	Manufacturer#4           	Brand#44  	PROMO BRUSHED STEEL	33	WRAP PACK 	1416.49	sts sleep blithely 
17502	tan powder metallic lemon blue	Manufacturer#2           	Brand#22  	PROMO ANODIZED TIN	13	WRAP BOX  	1419.50	l ideas w
17504	navajo saddle light seashell chiffon	Manufacturer#1           	Brand#11  	MEDIUM PLATED BRASS	2	MED BOX   	1421.50	slyly final orbi
17505	orange navajo almond coral wheat	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	14	MED BAG   	1422.50	ickly. ironic platele
17506	ghost white brown spring rose	Manufacturer#3           	Brand#34  	STANDARD PLATED COPPER	21	MED PACK  	1423.50	l orbits. 
17508	khaki pink linen orchid turquoise	Manufacturer#4           	Brand#41  	STANDARD ANODIZED BRASS	27	WRAP BOX  	1425.50	blithely regular
20000	royal red metallic dim azure	Manufacturer#2           	Brand#25  	STANDARD BURNISHED NICKEL	48	SM JAR    	920.00	sts-- 
20003	turquoise papaya navajo cornflower orchid	Manufacturer#2           	Brand#24  	PROMO PLATED BRASS	43	WRAP PKG  	923.00	sentiments slee
20005	dim white violet beige sandy	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED STEEL	12	JUMBO JAR 	925.00	cial theod
20006	snow rosy sienna frosted misty	Manufacturer#5           	Brand#53  	STANDARD BURNISHED NICKEL	32	WRAP JAR  	926.00	hely pending 
20007	thistle cornflower brown snow coral	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED STEEL	47	WRAP PACK 	927.00	refully
20009	floral spring burlywood blush mint	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	2	MED BOX   	929.00	t blithely slyly un
22494	burnished bisque dark rose dim	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	14	LG DRUM   	1416.49	theodolites sleep 
22497	firebrick ivory chartreuse peach hot	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED BRASS	23	LG PACK   	1419.49	latelets cajole regula
22499	magenta misty tan olive cornsilk	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED COPPER	4	LG BAG    	1421.49	jole slyly furiously
22500	chocolate pink cream spring snow	Manufacturer#2           	Brand#23  	PROMO BURNISHED COPPER	32	WRAP PKG  	1422.50	, ironic foxes. slyly 
22501	navajo puff chartreuse tomato plum	Manufacturer#4           	Brand#43  	STANDARD PLATED BRASS	19	LG DRUM   	1423.50	ackages. evenly b
22503	metallic saddle violet navajo smoke	Manufacturer#2           	Brand#24  	SMALL PLATED COPPER	12	LG BAG    	1425.50	ss the fluffily ev
24996	yellow lawn rose burlywood papaya	Manufacturer#1           	Brand#13  	STANDARD ANODIZED TIN	7	WRAP BOX  	1920.99	eep carefully. eve
24999	grey black plum sandy papaya	Manufacturer#4           	Brand#45  	LARGE BURNISHED STEEL	18	MED JAR   	1923.99	ans w
25001	violet turquoise aquamarine beige blue	Manufacturer#4           	Brand#43  	ECONOMY PLATED BRASS	7	WRAP CAN  	926.00	deas 
25002	metallic rosy misty puff lemon	Manufacturer#3           	Brand#35  	STANDARD ANODIZED COPPER	14	LG BAG    	927.00	hely. regular theodoli
25003	almond sienna peru smoke navy	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	10	JUMBO BAG 	928.00	o beans boost; fluffil
25005	misty navajo medium peru yellow	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	5	SM BAG    	930.00	its. furiously speci
27498	coral drab brown rose black	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED NICKEL	40	LG PKG    	1425.49	thinly regu
27501	ghost pink beige turquoise purple	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED STEEL	19	WRAP BAG  	1428.50	posits slee
27503	turquoise smoke olive maroon navy	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED NICKEL	26	MED CASE  	1430.50	g dependenc
27504	blush mint goldenrod light wheat	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	18	JUMBO BOX 	1431.50	nto beans cajo
27505	puff lemon brown chartreuse lavender	Manufacturer#1           	Brand#11  	PROMO BURNISHED STEEL	9	WRAP PKG  	1432.50	gged courts. c
27507	moccasin tan smoke seashell dark	Manufacturer#3           	Brand#33  	SMALL ANODIZED COPPER	41	SM PACK   	1434.50	tions. carefully regul
30000	goldenrod metallic powder floral seashell	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED NICKEL	49	SM CAN    	930.00	counts ha
30003	smoke orchid cornsilk almond dodger	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	9	WRAP JAR  	933.00	al sentiments breac
30005	drab indian honeydew brown burnished	Manufacturer#2           	Brand#21  	SMALL ANODIZED COPPER	16	MED CASE  	935.00	ven deposits. r
30006	medium drab midnight frosted indian	Manufacturer#2           	Brand#21  	SMALL POLISHED NICKEL	24	JUMBO PKG 	936.00	yly silent
30007	misty purple peru lemon spring	Manufacturer#3           	Brand#33  	PROMO BURNISHED STEEL	34	MED JAR   	937.00	 platelets use 
30009	pale sandy sienna linen dark	Manufacturer#3           	Brand#34  	LARGE POLISHED COPPER	34	JUMBO DRUM	939.00	 slyly bold requests 
32491	khaki metallic ghost forest powder	Manufacturer#1           	Brand#14  	MEDIUM PLATED STEEL	41	MED CAN   	1423.49	 dazz
32494	dark deep sienna mint yellow	Manufacturer#4           	Brand#42  	SMALL POLISHED STEEL	10	LG JAR    	1426.49	ts. quickly r
32496	hot puff peach azure magenta	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED TIN	35	LG BOX    	1428.49	 requests wake regular
32497	powder orchid burnished ghost almond	Manufacturer#3           	Brand#31  	PROMO ANODIZED BRASS	6	SM CAN    	1429.49	ecial pa
32498	lime spring brown moccasin cream	Manufacturer#1           	Brand#12  	STANDARD POLISHED COPPER	17	JUMBO PKG 	1430.49	g accounts. regula
32500	light brown pink lawn tan	Manufacturer#1           	Brand#14  	PROMO POLISHED STEEL	25	MED PACK  	1432.50	ckly special foxes us
34994	lace violet blush medium pale	Manufacturer#2           	Brand#22  	STANDARD PLATED TIN	29	WRAP BAG  	1928.99	le carefully.
34997	pale azure yellow mint khaki	Manufacturer#5           	Brand#52  	SMALL BURNISHED BRASS	30	LG JAR    	1931.99	 slyly pending 
34999	burnished purple chocolate dodger blanched	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED BRASS	33	SM DRUM   	1933.99	the entic
35000	saddle indian gainsboro midnight forest	Manufacturer#2           	Brand#25  	SMALL BURNISHED STEEL	25	SM BAG    	935.00	ronic inst
35001	rose indian lime midnight medium	Manufacturer#5           	Brand#52  	SMALL BRUSHED COPPER	22	LG BAG    	936.00	ronic a
35003	peru metallic chocolate turquoise light	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	38	WRAP CAN  	938.00	 furiously eve
37497	goldenrod rosy green peru azure	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	16	WRAP BAG  	1434.49	 beans cajol
37500	navajo indian beige tan sandy	Manufacturer#2           	Brand#22  	PROMO PLATED NICKEL	47	JUMBO DRUM	1437.50	 final i
37502	puff chiffon gainsboro cyan grey	Manufacturer#2           	Brand#23  	SMALL BURNISHED BRASS	33	MED PKG   	1439.50	posits. theodolites
37503	misty red dodger honeydew aquamarine	Manufacturer#1           	Brand#14  	LARGE POLISHED STEEL	2	SM BAG    	1440.50	 slyly express pack
37504	medium black snow lavender cyan	Manufacturer#2           	Brand#22  	MEDIUM POLISHED STEEL	21	JUMBO PACK	1441.50	e bli
37506	ghost orange cyan lavender hot	Manufacturer#3           	Brand#33  	LARGE ANODIZED COPPER	48	WRAP BAG  	1443.50	thy excuses could nag
40000	frosted rose maroon yellow dark	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED TIN	34	SM PKG    	940.00	? qui
40003	drab thistle navajo light purple	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	17	JUMBO PKG 	943.00	 slyl
40005	green cornflower tomato tan dark	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	45	MED CAN   	945.00	nusual, reg
40006	seashell lace rosy midnight tan	Manufacturer#3           	Brand#33  	PROMO BRUSHED NICKEL	36	MED JAR   	946.00	counts cajole carefull
40007	rose lace yellow burlywood lawn	Manufacturer#4           	Brand#43  	LARGE BURNISHED STEEL	45	MED CASE  	947.00	 furiously among the
40009	tan dark red rosy bisque	Manufacturer#3           	Brand#35  	MEDIUM ANODIZED BRASS	45	JUMBO PKG 	949.00	 stealthily unusual de
42488	green wheat goldenrod indian spring	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED NICKEL	48	MED CASE  	1430.48	 the accounts. slyly r
42491	azure antique goldenrod medium navy	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED BRASS	12	WRAP JAR  	1433.49	ly expre
42493	peru gainsboro light lace red	Manufacturer#1           	Brand#14  	LARGE BRUSHED TIN	47	JUMBO CAN 	1435.49	special,
42494	ivory blush coral lawn navy	Manufacturer#3           	Brand#31  	LARGE ANODIZED TIN	27	WRAP CASE 	1436.49	ial warth
42495	olive blush chocolate cyan violet	Manufacturer#2           	Brand#24  	STANDARD BRUSHED NICKEL	42	MED PKG   	1437.49	l excuses. silent
42497	peach dark frosted saddle ghost	Manufacturer#5           	Brand#55  	STANDARD ANODIZED STEEL	44	JUMBO JAR 	1439.49	es detect d
44992	slate khaki smoke almond azure	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED NICKEL	34	JUMBO BOX 	1936.99	he quickly express
44995	purple navajo violet cornsilk lawn	Manufacturer#1           	Brand#12  	SMALL ANODIZED TIN	5	LG PKG    	1939.99	unusual asymptotes us
44997	rose dark ivory red peach	Manufacturer#5           	Brand#52  	LARGE PLATED BRASS	8	LG CAN    	1941.99	ng the silent pinto
44998	medium chiffon black cornflower drab	Manufacturer#4           	Brand#41  	SMALL BRUSHED COPPER	6	MED DRUM  	1942.99	eposits. re
44999	sky cornflower forest khaki goldenrod	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED STEEL	24	LG BOX    	1943.99	onic accounts
45001	plum khaki tan rose linen	Manufacturer#3           	Brand#32  	PROMO BRUSHED TIN	50	MED CAN   	946.00	sual platelets! packag
47496	slate linen burnished dodger burlywood	Manufacturer#5           	Brand#52  	PROMO BURNISHED STEEL	16	LG PACK   	1443.49	boost fu
47499	navajo medium sky gainsboro powder	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	18	WRAP JAR  	1446.49	tes. waters ca
47501	indian orange plum antique ghost	Manufacturer#3           	Brand#33  	LARGE BURNISHED TIN	50	WRAP BAG  	1448.50	e furiously quickly i
47502	lace yellow dim linen dark	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED BRASS	34	SM DRUM   	1449.50	 the regu
47503	sienna deep forest metallic goldenrod	Manufacturer#4           	Brand#43  	ECONOMY POLISHED COPPER	9	MED BAG   	1450.50	ages wake since the un
47505	wheat indian lemon grey peach	Manufacturer#3           	Brand#33  	MEDIUM BRUSHED TIN	41	JUMBO PKG 	1452.50	es run furious
50000	dark blush snow lime smoke	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED STEEL	46	WRAP CAN  	950.00	yly ironic ideas ar
50003	wheat almond lime powder turquoise	Manufacturer#4           	Brand#42  	SMALL BRUSHED TIN	45	JUMBO BAG 	953.00	final pattern
50005	olive blanched pale chartreuse medium	Manufacturer#5           	Brand#55  	STANDARD PLATED STEEL	29	WRAP DRUM 	955.00	pecial instr
50006	navajo peru white khaki smoke	Manufacturer#1           	Brand#14  	STANDARD POLISHED BRASS	25	LG PACK   	956.00	never thin accounts. f
50007	blush rosy puff seashell royal	Manufacturer#2           	Brand#24  	LARGE PLATED BRASS	15	LG BOX    	957.00	. quickly expres
50009	linen peach powder antique metallic	Manufacturer#1           	Brand#14  	PROMO POLISHED BRASS	36	WRAP JAR  	959.00	ly. packages boost bli
52485	green aquamarine lime midnight purple	Manufacturer#1           	Brand#13  	LARGE ANODIZED NICKEL	29	JUMBO PACK	1437.48	ly above the pe
52488	midnight floral azure black goldenrod	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	32	SM BOX    	1440.48	uriously 
52490	floral azure orchid rosy misty	Manufacturer#4           	Brand#43  	PROMO POLISHED NICKEL	40	JUMBO JAR 	1442.49	packages are about the
52491	puff orange dim chocolate burlywood	Manufacturer#1           	Brand#14  	STANDARD POLISHED TIN	22	LG BOX    	1443.49	ironicall
52492	deep azure magenta salmon peach	Manufacturer#4           	Brand#42  	SMALL BRUSHED NICKEL	36	MED CASE  	1444.49	efully unusual r
52494	lemon wheat powder smoke blue	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED NICKEL	39	WRAP PACK 	1446.49	sly ab
54990	dark lemon floral drab lavender	Manufacturer#4           	Brand#42  	MEDIUM POLISHED BRASS	34	LG CAN    	1944.99	uriously reque
54993	sienna light thistle burlywood turquoise	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	13	WRAP BOX  	1947.99	ly furiously
54995	navy white green cornsilk sienna	Manufacturer#3           	Brand#32  	LARGE PLATED STEEL	22	WRAP PKG  	1949.99	counts.
54996	gainsboro peach hot lavender chiffon	Manufacturer#2           	Brand#21  	PROMO POLISHED TIN	40	MED DRUM  	1950.99	atelets wake. bl
54997	powder beige chiffon mint tan	Manufacturer#4           	Brand#41  	PROMO BURNISHED NICKEL	2	WRAP PKG  	1951.99	the express
54999	green dodger rose black magenta	Manufacturer#3           	Brand#33  	STANDARD PLATED TIN	2	SM PKG    	1953.99	efully final attai
57495	green pale rose sienna grey	Manufacturer#5           	Brand#54  	SMALL PLATED TIN	11	JUMBO BOX 	1452.49	ully expres
57498	dark ivory light firebrick thistle	Manufacturer#5           	Brand#54  	STANDARD POLISHED TIN	42	JUMBO CAN 	1455.49	ets. blithely
57500	orchid bisque misty lime beige	Manufacturer#1           	Brand#15  	SMALL POLISHED NICKEL	15	SM BOX    	1457.50	es play quickly ex
57501	ghost blue drab seashell beige	Manufacturer#2           	Brand#22  	STANDARD BRUSHED STEEL	36	WRAP BAG  	1458.50	pliers cajole f
57502	thistle lavender rose orchid firebrick	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED TIN	30	MED CAN   	1459.50	, pending instru
57504	drab purple ghost linen grey	Manufacturer#3           	Brand#33  	STANDARD BURNISHED NICKEL	11	MED CASE  	1461.50	es above the fur
60000	slate blush blue aquamarine lime	Manufacturer#1           	Brand#14  	MEDIUM BRUSHED TIN	40	LG BOX    	960.00	ggle carefully 
60003	bisque almond beige powder hot	Manufacturer#1           	Brand#12  	SMALL ANODIZED STEEL	45	WRAP DRUM 	963.00	ly caref
60005	metallic salmon aquamarine mint lavender	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED BRASS	26	SM CAN    	965.00	cies cajole slyly even
60006	steel orchid mint goldenrod cornflower	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	3	WRAP DRUM 	966.00	furiously regular 
60007	forest peru lavender almond firebrick	Manufacturer#2           	Brand#25  	SMALL BRUSHED TIN	32	LG PKG    	967.00	ests? qui
60009	gainsboro midnight red antique peru	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED COPPER	24	LG CASE   	969.00	iously even dependenci
62482	peru purple salmon lime smoke	Manufacturer#1           	Brand#11  	SMALL BURNISHED COPPER	19	WRAP PKG  	1444.48	ronic requests. regula
62485	moccasin antique brown drab ivory	Manufacturer#1           	Brand#15  	SMALL ANODIZED STEEL	29	WRAP DRUM 	1447.48	even, final fo
62487	seashell pale dark magenta firebrick	Manufacturer#2           	Brand#24  	MEDIUM PLATED STEEL	12	LG DRUM   	1449.48	ccounts use 
62488	cornflower thistle chiffon navy peach	Manufacturer#5           	Brand#54  	LARGE BRUSHED STEEL	19	MED JAR   	1450.48	uests according to th
62489	yellow sienna papaya brown chartreuse	Manufacturer#5           	Brand#52  	MEDIUM PLATED BRASS	39	SM BOX    	1451.48	ges nag furiously ag
62491	khaki beige chartreuse seashell indian	Manufacturer#2           	Brand#25  	LARGE PLATED NICKEL	48	JUMBO BAG 	1453.49	pending d
64988	purple cyan deep thistle orange	Manufacturer#3           	Brand#34  	SMALL PLATED NICKEL	12	SM PACK   	1952.98	gular 
64991	dim linen blush maroon navajo	Manufacturer#1           	Brand#11  	LARGE BURNISHED STEEL	5	JUMBO BOX 	1955.99	totes
64993	navy lime frosted medium cornflower	Manufacturer#2           	Brand#23  	PROMO POLISHED STEEL	37	WRAP JAR  	1957.99	he quickly r
64994	lavender aquamarine mint steel tomato	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED BRASS	15	WRAP JAR  	1958.99	haggle careful
64995	maroon misty frosted slate snow	Manufacturer#5           	Brand#51  	PROMO PLATED NICKEL	41	SM BAG    	1959.99	ggle. slyly ironic a
64997	maroon dark navajo black medium	Manufacturer#4           	Brand#43  	ECONOMY POLISHED TIN	2	WRAP CASE 	1961.99	hy asymptote
67494	turquoise rosy blush olive wheat	Manufacturer#3           	Brand#35  	LARGE BRUSHED STEEL	14	WRAP CASE 	1461.49	ng the furiou
67497	black drab seashell orchid cyan	Manufacturer#2           	Brand#22  	STANDARD BURNISHED BRASS	17	LG PKG    	1464.49	sly regu
67499	metallic gainsboro dodger aquamarine ivory	Manufacturer#4           	Brand#45  	LARGE PLATED COPPER	6	MED CAN   	1466.49	carefully final acco
67500	powder chocolate sky purple pink	Manufacturer#2           	Brand#23  	SMALL PLATED COPPER	27	LG BAG    	1467.50	ly speci
67501	salmon dodger chiffon snow grey	Manufacturer#5           	Brand#53  	STANDARD POLISHED STEEL	46	WRAP BAG  	1468.50	blithel
67503	gainsboro sandy dark plum tomato	Manufacturer#4           	Brand#41  	ECONOMY PLATED TIN	43	JUMBO BOX 	1470.50	y ironic, f
70000	seashell burnished khaki navajo dodger	Manufacturer#3           	Brand#31  	STANDARD ANODIZED TIN	9	SM DRUM   	970.00	e special, e
70003	salmon lemon metallic seashell pink	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED NICKEL	28	LG JAR    	973.00	blithely pendin
70005	saddle burnished royal burlywood dark	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	4	LG DRUM   	975.00	uriously final
70006	blanched rose brown steel deep	Manufacturer#2           	Brand#24  	STANDARD PLATED COPPER	35	JUMBO BAG 	976.00	ully s
70007	moccasin midnight peru rose blue	Manufacturer#4           	Brand#42  	STANDARD ANODIZED BRASS	9	JUMBO JAR 	977.00	ular deposits 
70009	ghost antique almond papaya saddle	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED STEEL	6	SM DRUM   	979.00	leep slyly
72479	snow steel spring forest slate	Manufacturer#1           	Brand#14  	PROMO BURNISHED STEEL	31	WRAP DRUM 	1451.47	nusual packa
72482	grey chocolate metallic linen puff	Manufacturer#3           	Brand#31  	STANDARD ANODIZED TIN	10	LG PKG    	1454.48	ly sly accounts impr
72484	orange honeydew snow blush powder	Manufacturer#2           	Brand#21  	STANDARD BURNISHED BRASS	50	WRAP BOX  	1456.48	 wake
72485	orchid chocolate thistle aquamarine spring	Manufacturer#4           	Brand#41  	LARGE BRUSHED BRASS	31	WRAP BAG  	1457.48	according to the 
72486	burnished grey orchid orange cyan	Manufacturer#4           	Brand#45  	SMALL PLATED BRASS	6	SM JAR    	1458.48	y excuses wake regu
72488	sienna navajo tan rose aquamarine	Manufacturer#1           	Brand#11  	PROMO BURNISHED TIN	6	WRAP JAR  	1460.48	 silent pinto beans 
74986	moccasin seashell burlywood slate tan	Manufacturer#4           	Brand#43  	STANDARD BURNISHED STEEL	25	MED BOX   	1960.98	 beans 
74989	orange tomato chartreuse green deep	Manufacturer#3           	Brand#31  	ECONOMY BURNISHED NICKEL	5	SM DRUM   	1963.98	odolites. furiou
74991	ivory slate firebrick green chocolate	Manufacturer#2           	Brand#24  	PROMO POLISHED STEEL	4	JUMBO CAN 	1965.99	ronic fox
74992	purple chartreuse aquamarine peach coral	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED STEEL	13	LG BAG    	1966.99	ons. blithely exp
74993	mint navajo rose yellow violet	Manufacturer#1           	Brand#12  	LARGE BURNISHED TIN	19	WRAP JAR  	1967.99	inal accounts boo
74995	cyan moccasin dark orange thistle	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED COPPER	41	LG CASE   	1969.99	cajole about the 
77493	gainsboro burnished aquamarine forest black	Manufacturer#1           	Brand#14  	PROMO POLISHED BRASS	48	SM DRUM   	1470.49	ets alongsid
77496	chartreuse honeydew grey misty peach	Manufacturer#1           	Brand#11  	STANDARD PLATED COPPER	48	LG CASE   	1473.49	ly carefully ir
77498	azure light metallic frosted dodger	Manufacturer#5           	Brand#52  	SMALL BRUSHED BRASS	19	LG PACK   	1475.49	 final instructions.
77499	white navy goldenrod ghost burnished	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED TIN	21	LG PACK   	1476.49	s. unusual
77500	brown rose pale peru burnished	Manufacturer#3           	Brand#35  	SMALL ANODIZED TIN	27	SM BAG    	1477.50	l, ironic pinto beans
77502	green dark magenta puff powder	Manufacturer#3           	Brand#34  	PROMO PLATED TIN	24	JUMBO PACK	1479.50	ndencies. slyly
80000	purple coral medium ghost beige	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	7	JUMBO BOX 	980.00	 blithely 
80003	lemon powder rose purple slate	Manufacturer#2           	Brand#22  	SMALL PLATED BRASS	44	LG BOX    	983.00	ven accounts ca
80005	brown maroon royal pink slate	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED TIN	21	JUMBO CAN 	985.00	 ironic foxes. s
80006	purple green misty orange royal	Manufacturer#5           	Brand#51  	SMALL PLATED TIN	12	JUMBO JAR 	986.00	run slyl
80007	purple cyan pale spring chiffon	Manufacturer#4           	Brand#42  	STANDARD ANODIZED COPPER	50	WRAP DRUM 	987.00	s; courts haggle a
80009	tan chiffon beige linen plum	Manufacturer#5           	Brand#55  	LARGE ANODIZED BRASS	32	WRAP DRUM 	989.00	 pending,
82476	burnished almond misty dim light	Manufacturer#1           	Brand#13  	LARGE POLISHED NICKEL	7	WRAP JAR  	1458.47	sleep carefully. 
82479	blanched royal almond goldenrod slate	Manufacturer#3           	Brand#31  	STANDARD BRUSHED COPPER	5	JUMBO PACK	1461.47	arefully
82481	smoke thistle blanched medium gainsboro	Manufacturer#5           	Brand#51  	STANDARD ANODIZED STEEL	39	SM DRUM   	1463.48	eposits sleep quickly
82482	royal maroon violet yellow bisque	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED TIN	9	LG CAN    	1464.48	jole caref
82483	burlywood magenta metallic puff dim	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	22	MED CASE  	1465.48	uests. re
82485	seashell grey coral mint spring	Manufacturer#1           	Brand#11  	ECONOMY PLATED TIN	34	WRAP DRUM 	1467.48	cajole blithely: eve
84984	lime antique cream blush linen	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED COPPER	28	SM JAR    	1968.98	the pending i
84987	sky ghost salmon navy wheat	Manufacturer#1           	Brand#13  	STANDARD POLISHED TIN	32	JUMBO BOX 	1971.98	uffily fu
84989	tomato chiffon dodger cornflower burnished	Manufacturer#4           	Brand#44  	STANDARD ANODIZED COPPER	30	WRAP PKG  	1973.98	gular 
84990	peach turquoise saddle lace lawn	Manufacturer#4           	Brand#43  	SMALL PLATED TIN	14	SM PACK   	1974.99	carefully 
84991	burlywood red royal spring ghost	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED BRASS	41	SM PACK   	1975.99	 solve alon
84993	rosy snow powder metallic navy	Manufacturer#3           	Brand#31  	PROMO ANODIZED NICKEL	6	SM BAG    	1977.99	epths. ironic, bu
87492	tomato plum light rose smoke	Manufacturer#2           	Brand#25  	MEDIUM POLISHED NICKEL	30	LG CASE   	1479.49	posits wake fluf
87495	peru honeydew dodger navajo khaki	Manufacturer#4           	Brand#43  	PROMO ANODIZED STEEL	9	LG JAR    	1482.49	even accounts. 
87497	sky powder ivory cyan pale	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED BRASS	10	LG BOX    	1484.49	latelets cajole bl
87498	bisque blush ivory salmon snow	Manufacturer#5           	Brand#54  	MEDIUM POLISHED NICKEL	41	SM PKG    	1485.49	ut the furiously
87499	plum drab metallic navy salmon	Manufacturer#3           	Brand#34  	PROMO ANODIZED BRASS	47	MED BOX   	1486.49	lyly regular re
87501	mint olive red puff thistle	Manufacturer#3           	Brand#32  	STANDARD ANODIZED COPPER	14	JUMBO CASE	1488.50	deposits nag slyly
90000	azure pale lawn cornflower black	Manufacturer#5           	Brand#52  	STANDARD BRUSHED BRASS	22	MED BAG   	990.00	s are qui
90003	khaki almond indian cyan spring	Manufacturer#3           	Brand#34  	STANDARD PLATED BRASS	42	WRAP CASE 	993.00	efully 
90005	smoke lace plum chartreuse azure	Manufacturer#5           	Brand#51  	STANDARD BURNISHED COPPER	16	SM BAG    	995.00	le slyly slyly even 
90006	aquamarine maroon sky coral ivory	Manufacturer#1           	Brand#15  	PROMO BURNISHED TIN	16	JUMBO JAR 	996.00	y final accounts ca
90007	firebrick ghost blue spring snow	Manufacturer#4           	Brand#41  	SMALL ANODIZED TIN	5	MED DRUM  	997.00	lly special
90009	pale dark light rosy wheat	Manufacturer#4           	Brand#43  	LARGE PLATED STEEL	2	MED JAR   	999.00	ld the
92473	cyan azure gainsboro cornsilk rose	Manufacturer#5           	Brand#55  	PROMO ANODIZED STEEL	45	MED BOX   	1465.47	ding to
92476	black linen mint coral chocolate	Manufacturer#1           	Brand#13  	MEDIUM POLISHED STEEL	18	SM CAN    	1468.47	carefully ironi
92478	coral tomato firebrick chocolate brown	Manufacturer#2           	Brand#22  	PROMO BRUSHED BRASS	17	SM BAG    	1470.47	xpress accounts 
92479	seashell khaki linen goldenrod steel	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	12	MED CASE  	1471.47	blithe
92480	deep hot forest saddle orange	Manufacturer#5           	Brand#51  	LARGE ANODIZED BRASS	7	JUMBO CAN 	1472.48	y ideas. carefully 
92482	thistle dark tan turquoise midnight	Manufacturer#1           	Brand#12  	STANDARD ANODIZED NICKEL	17	SM JAR    	1474.48	into beans 
94982	lace turquoise orchid blush chiffon	Manufacturer#5           	Brand#55  	SMALL ANODIZED TIN	41	WRAP PKG  	1976.98	ounts; slyly bo
94985	forest turquoise coral wheat mint	Manufacturer#2           	Brand#21  	LARGE PLATED COPPER	35	MED BOX   	1979.98	fily pending hocke
94987	floral almond blush spring orchid	Manufacturer#2           	Brand#24  	SMALL POLISHED STEEL	42	JUMBO CASE	1981.98	quests sleep quic
94988	burnished olive sky khaki firebrick	Manufacturer#5           	Brand#54  	STANDARD BRUSHED STEEL	31	LG BAG    	1982.98	eep above the fluff
94989	plum red peru tan burlywood	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED STEEL	10	MED PKG   	1983.98	ges u
94991	azure forest steel snow blue	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED BRASS	28	JUMBO CAN 	1985.99	accou
97491	plum blush pink yellow turquoise	Manufacturer#1           	Brand#12  	PROMO BURNISHED TIN	3	MED JAR   	1488.49	onic instructions aft
97494	wheat coral grey saddle deep	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED TIN	25	MED JAR   	1491.49	c ideas ar
97496	azure gainsboro hot dim light	Manufacturer#3           	Brand#35  	ECONOMY PLATED TIN	42	SM CASE   	1493.49	al co
97497	spring floral peru cream bisque	Manufacturer#4           	Brand#42  	PROMO PLATED NICKEL	4	LG CASE   	1494.49	he dari
97498	burlywood grey deep red powder	Manufacturer#2           	Brand#23  	LARGE BRUSHED COPPER	6	WRAP DRUM 	1495.49	deposits cajole fluff
97500	chocolate cyan plum bisque lime	Manufacturer#1           	Brand#15  	LARGE ANODIZED TIN	32	JUMBO DRUM	1497.50	uffily even theodolite
100000	goldenrod forest salmon white spring	Manufacturer#1           	Brand#15  	LARGE BURNISHED STEEL	17	MED BOX   	1000.00	ies haggle qu
100003	beige powder violet orchid yellow	Manufacturer#2           	Brand#21  	MEDIUM PLATED BRASS	41	SM BOX    	1003.00	 carefully even pac
100005	grey midnight orange peach pale	Manufacturer#2           	Brand#21  	SMALL POLISHED STEEL	7	MED BAG   	1005.00	ajole? blithe
100006	violet sandy olive yellow orange	Manufacturer#4           	Brand#45  	STANDARD BURNISHED COPPER	23	WRAP CASE 	1006.00	he slyly regular pack
100007	snow magenta pale lemon metallic	Manufacturer#1           	Brand#12  	PROMO BURNISHED COPPER	4	MED PKG   	1007.00	ronic accounts in
100009	goldenrod sandy beige hot orange	Manufacturer#3           	Brand#32  	SMALL BURNISHED STEEL	41	WRAP BOX  	1009.00	dinos about the quick
102470	forest peach indian red misty	Manufacturer#5           	Brand#55  	STANDARD POLISHED TIN	1	MED BAG   	1472.47	orbits are furious
102473	azure thistle firebrick slate burnished	Manufacturer#4           	Brand#41  	PROMO POLISHED STEEL	50	LG PACK   	1475.47	s across the carefu
102475	mint cyan moccasin ivory peach	Manufacturer#3           	Brand#32  	LARGE BRUSHED NICKEL	18	WRAP PKG  	1477.47	st among the 
102476	lavender hot powder blanched aquamarine	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	26	LG CAN    	1478.47	lar accounts promise s
102477	thistle cream misty khaki green	Manufacturer#2           	Brand#25  	LARGE PLATED TIN	19	JUMBO PKG 	1479.47	ests to t
102479	cyan puff forest dim mint	Manufacturer#2           	Brand#24  	PROMO BURNISHED NICKEL	48	LG CASE   	1481.47	egular grouches
104980	green linen azure ivory navy	Manufacturer#4           	Brand#44  	SMALL PLATED NICKEL	38	MED CAN   	1984.98	excus
104983	tomato moccasin bisque slate midnight	Manufacturer#1           	Brand#14  	SMALL POLISHED TIN	44	LG BOX    	1987.98	pecial, unus
104985	peru cornsilk lime papaya pale	Manufacturer#2           	Brand#25  	PROMO PLATED COPPER	43	MED BOX   	1989.98	symptotes 
104986	midnight wheat cornflower sky lemon	Manufacturer#2           	Brand#21  	ECONOMY PLATED STEEL	50	JUMBO PKG 	1990.98	counts w
104987	blue lemon thistle dim mint	Manufacturer#4           	Brand#42  	SMALL PLATED STEEL	44	LG PACK   	1991.98	cajole fu
104989	smoke maroon cyan navy mint	Manufacturer#4           	Brand#42  	MEDIUM POLISHED TIN	45	WRAP PACK 	1993.98	kly final reque
107490	honeydew sienna black orchid lime	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	5	LG PACK   	1497.49	ackag
107493	maroon khaki mint puff salmon	Manufacturer#3           	Brand#33  	STANDARD BRUSHED TIN	20	SM BAG    	1500.49	ntain fluffily 
107495	dodger olive salmon cyan azure	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	24	WRAP BAG  	1502.49	riously until the care
107496	salmon hot navy navajo sandy	Manufacturer#5           	Brand#51  	PROMO ANODIZED COPPER	9	LG CAN    	1503.49	t the slyly silent s
107497	lavender bisque sandy rosy aquamarine	Manufacturer#4           	Brand#44  	PROMO PLATED TIN	37	SM CAN    	1504.49	osits h
107499	floral tan ivory bisque ghost	Manufacturer#3           	Brand#34  	ECONOMY PLATED BRASS	40	WRAP CAN  	1506.49	s cajole slyly careful
110000	tomato linen royal turquoise seashell	Manufacturer#5           	Brand#54  	LARGE PLATED NICKEL	24	MED CAN   	1010.00	olphins at t
110003	forest seashell green powder azure	Manufacturer#3           	Brand#31  	LARGE PLATED BRASS	41	LG CASE   	1013.00	egular accounts ca
110005	dodger medium red indian brown	Manufacturer#5           	Brand#53  	STANDARD BURNISHED COPPER	47	SM PKG    	1015.00	- special ideas acc
110006	burnished plum almond orchid magenta	Manufacturer#5           	Brand#53  	LARGE BURNISHED BRASS	38	SM PKG    	1016.00	lly iro
110007	hot drab seashell pale black	Manufacturer#5           	Brand#55  	STANDARD ANODIZED STEEL	8	JUMBO PACK	1017.00	y iro
110009	burlywood plum spring salmon seashell	Manufacturer#4           	Brand#41  	PROMO PLATED COPPER	22	LG JAR    	1019.00	e special theodolite
112467	metallic azure sienna dodger mint	Manufacturer#2           	Brand#22  	MEDIUM PLATED BRASS	22	WRAP PACK 	1479.46	ackages. 
112470	ivory cream honeydew grey brown	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	21	LG BOX    	1482.47	ular 
112472	pale spring beige snow navajo	Manufacturer#1           	Brand#14  	PROMO BRUSHED COPPER	30	MED PKG   	1484.47	ions wake 
112473	thistle peach black firebrick light	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	43	WRAP CASE 	1485.47	gle quick
112474	slate steel cream lawn olive	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED BRASS	46	WRAP PKG  	1486.47	lar, even 
112476	slate peach light deep seashell	Manufacturer#5           	Brand#52  	SMALL ANODIZED NICKEL	7	JUMBO JAR 	1488.47	about the slyl
114978	smoke tan maroon dark metallic	Manufacturer#5           	Brand#51  	SMALL PLATED TIN	47	JUMBO BOX 	1992.97	press 
114981	bisque slate puff plum dodger	Manufacturer#3           	Brand#31  	STANDARD ANODIZED STEEL	13	MED JAR   	1995.98	uickl
114983	blue seashell smoke chiffon chartreuse	Manufacturer#4           	Brand#43  	SMALL BRUSHED TIN	48	WRAP BAG  	1997.98	deposits? caref
114984	honeydew navy floral gainsboro smoke	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	18	JUMBO JAR 	1998.98	as are blithely even,
114985	dim pale magenta turquoise linen	Manufacturer#1           	Brand#14  	STANDARD BURNISHED NICKEL	30	MED BAG   	1999.98	leep again
114987	ivory papaya salmon antique white	Manufacturer#1           	Brand#12  	STANDARD BURNISHED BRASS	21	LG PKG    	2001.98	, special requests
117489	grey saddle light antique orchid	Manufacturer#2           	Brand#21  	PROMO BURNISHED STEEL	41	SM PACK   	1506.48	n pinto beans w
117492	ivory forest tomato azure chiffon	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED STEEL	38	WRAP DRUM 	1509.49	uffil
117494	plum magenta linen bisque metallic	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	3	SM CASE   	1511.49	sual pinto be
117495	burlywood papaya olive yellow navajo	Manufacturer#4           	Brand#44  	MEDIUM POLISHED STEEL	5	MED DRUM  	1512.49	press pinto beans. 
117496	misty frosted maroon moccasin hot	Manufacturer#4           	Brand#43  	SMALL PLATED STEEL	45	JUMBO JAR 	1513.49	gular
117498	lace pale dim steel azure	Manufacturer#4           	Brand#43  	MEDIUM POLISHED COPPER	26	MED BAG   	1515.49	nto beans after 
120000	spring black goldenrod slate beige	Manufacturer#5           	Brand#51  	PROMO POLISHED NICKEL	5	JUMBO CASE	1020.00	onic deposits.
120003	metallic rosy gainsboro dark spring	Manufacturer#4           	Brand#45  	PROMO ANODIZED NICKEL	48	LG DRUM   	1023.00	ounts. c
120005	turquoise floral papaya steel blanched	Manufacturer#2           	Brand#25  	SMALL BURNISHED COPPER	35	SM DRUM   	1025.00	s sleep. slyly final d
120006	maroon olive sky honeydew sienna	Manufacturer#3           	Brand#31  	LARGE BRUSHED TIN	45	LG CAN    	1026.00	carefully fin
120007	black antique bisque misty orange	Manufacturer#3           	Brand#34  	PROMO POLISHED TIN	46	WRAP BOX  	1027.00	al instructions. 
120009	deep coral honeydew cornsilk dodger	Manufacturer#1           	Brand#14  	STANDARD BURNISHED TIN	22	WRAP PACK 	1029.00	ests are ca
122464	magenta deep light green honeydew	Manufacturer#3           	Brand#35  	STANDARD ANODIZED COPPER	50	WRAP JAR  	1486.46	ly expre
122467	midnight peru magenta dark black	Manufacturer#3           	Brand#35  	STANDARD BRUSHED COPPER	10	LG BAG    	1489.46	y fina
122469	ghost hot bisque burnished almond	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	45	SM JAR    	1491.46	gainst the b
122470	deep firebrick indian red light	Manufacturer#1           	Brand#14  	LARGE PLATED BRASS	33	WRAP DRUM 	1492.47	ar, regular packag
122471	aquamarine linen bisque white lemon	Manufacturer#4           	Brand#44  	MEDIUM POLISHED TIN	41	JUMBO JAR 	1493.47	. never final
122473	deep navy cream firebrick dark	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED TIN	42	WRAP BOX  	1495.47	 deposits 
124976	slate green ghost honeydew moccasin	Manufacturer#1           	Brand#13  	PROMO POLISHED BRASS	34	WRAP CAN  	2000.97	ans haggle sly
124979	maroon pale goldenrod bisque lime	Manufacturer#2           	Brand#25  	LARGE BRUSHED NICKEL	19	SM CASE   	2003.97	nal dep
124981	lemon medium aquamarine drab pale	Manufacturer#3           	Brand#32  	STANDARD PLATED BRASS	16	JUMBO PKG 	2005.98	l pinto beans cajo
124982	mint sienna floral navy metallic	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED STEEL	33	WRAP BAG  	2006.98	nts. furiously unu
124983	linen thistle brown white deep	Manufacturer#2           	Brand#25  	LARGE POLISHED STEEL	32	JUMBO PACK	2007.98	 furiously ironic
124985	papaya misty spring lime plum	Manufacturer#1           	Brand#15  	SMALL BURNISHED BRASS	35	JUMBO BAG 	2009.98	d escapades nag across
127488	spring deep olive linen snow	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED TIN	38	JUMBO CASE	1515.48	nding accounts!
127491	maroon cornsilk floral ghost goldenrod	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED COPPER	41	MED BAG   	1518.49	accounts. 
127493	magenta cornsilk orchid violet sandy	Manufacturer#2           	Brand#22  	LARGE ANODIZED TIN	23	JUMBO CASE	1520.49	ons thrash 
127494	ghost green blue yellow ivory	Manufacturer#5           	Brand#54  	ECONOMY BURNISHED TIN	44	WRAP CAN  	1521.49	inal instructions a
127495	thistle chiffon turquoise deep yellow	Manufacturer#3           	Brand#31  	PROMO BURNISHED COPPER	27	WRAP CAN  	1522.49	ly. hockey pla
127497	azure salmon brown yellow green	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED COPPER	40	SM CASE   	1524.49	y special pa
130000	puff deep forest yellow floral	Manufacturer#3           	Brand#34  	PROMO POLISHED COPPER	8	JUMBO CASE	1030.00	ans sleep blith
130003	cornsilk deep midnight goldenrod dim	Manufacturer#3           	Brand#31  	STANDARD BRUSHED COPPER	45	LG PACK   	1033.00	haggle. slyl
130005	magenta chiffon pale lawn snow	Manufacturer#1           	Brand#11  	SMALL BRUSHED TIN	21	LG PACK   	1035.00	ely accoun
130006	magenta misty blanched metallic orchid	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED TIN	38	SM CASE   	1036.00	 furiously bol
130007	salmon smoke lime dodger puff	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	25	SM PKG    	1037.00	ut the blith
130009	lavender purple white navy magenta	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED TIN	40	LG PACK   	1039.00	rts boost furiou
132461	almond royal sky burlywood hot	Manufacturer#1           	Brand#11  	LARGE PLATED TIN	14	LG PKG    	1493.46	slyly 
132464	maroon drab bisque goldenrod dark	Manufacturer#4           	Brand#43  	STANDARD ANODIZED NICKEL	7	LG PACK   	1496.46	y. furiousl
132466	gainsboro midnight forest khaki dim	Manufacturer#3           	Brand#35  	MEDIUM BRUSHED COPPER	33	MED CAN   	1498.46	l instructi
132467	pink smoke slate chocolate lime	Manufacturer#4           	Brand#44  	STANDARD PLATED BRASS	32	WRAP CAN  	1499.46	al requests ca
132468	sky pink maroon turquoise wheat	Manufacturer#5           	Brand#53  	PROMO ANODIZED TIN	23	MED DRUM  	1500.46	old packages. flu
132470	cornflower burnished brown plum linen	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED STEEL	1	JUMBO PKG 	1502.47	lyly 
134974	orange green dim spring lime	Manufacturer#5           	Brand#51  	SMALL PLATED STEEL	10	LG PKG    	2008.97	press r
134977	sandy orchid black dim hot	Manufacturer#3           	Brand#33  	LARGE POLISHED BRASS	7	SM BOX    	2011.97	deposits.
134979	drab cornsilk goldenrod gainsboro sandy	Manufacturer#1           	Brand#15  	ECONOMY POLISHED COPPER	21	JUMBO CAN 	2013.97	ular,
134980	sienna navajo azure coral pink	Manufacturer#1           	Brand#13  	LARGE BRUSHED NICKEL	17	MED BOX   	2014.98	ackages. 
134981	navy almond grey snow dark	Manufacturer#5           	Brand#53  	SMALL BURNISHED BRASS	28	MED BAG   	2015.98	ounts: packages wak
134983	lavender azure chocolate medium goldenrod	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED BRASS	41	SM CASE   	2017.98	c deposits? fox
137487	snow papaya green beige chartreuse	Manufacturer#2           	Brand#25  	LARGE ANODIZED COPPER	50	LG BAG    	1524.48	 to the
137490	bisque rosy moccasin beige seashell	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED STEEL	35	LG PKG    	1527.49	le furiously. en
137492	forest saddle coral khaki dark	Manufacturer#2           	Brand#25  	STANDARD ANODIZED STEEL	26	SM JAR    	1529.49	posits. b
137493	frosted dodger salmon honeydew beige	Manufacturer#2           	Brand#23  	STANDARD PLATED BRASS	45	LG PACK   	1530.49	 accounts cajol
137494	medium brown puff indian deep	Manufacturer#3           	Brand#32  	ECONOMY PLATED TIN	45	WRAP DRUM 	1531.49	p; regular accoun
137496	deep burlywood seashell blue chocolate	Manufacturer#3           	Brand#35  	LARGE PLATED NICKEL	40	LG PKG    	1533.49	 depths haggle fluffil
140000	salmon frosted beige midnight blush	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED NICKEL	8	LG CASE   	1040.00	es wake abov
140003	peach orange magenta cornsilk lavender	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED TIN	38	SM PACK   	1043.00	eep carefull
140005	plum lavender dodger papaya khaki	Manufacturer#4           	Brand#41  	PROMO BURNISHED BRASS	29	WRAP BAG  	1045.00	le regular, regul
140006	sky lime coral plum dodger	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED BRASS	25	SM PACK   	1046.00	grate furio
140007	mint cornflower sandy blanched orchid	Manufacturer#3           	Brand#33  	PROMO BURNISHED COPPER	21	JUMBO JAR 	1047.00	ular instruction
140009	blue misty bisque puff slate	Manufacturer#5           	Brand#52  	PROMO BURNISHED BRASS	37	WRAP PKG  	1049.00	jole iro
142458	almond rosy lime black plum	Manufacturer#2           	Brand#24  	MEDIUM BURNISHED NICKEL	37	JUMBO JAR 	1500.45	 requests are sly
142461	maroon brown powder puff peach	Manufacturer#3           	Brand#33  	STANDARD BRUSHED BRASS	27	MED JAR   	1503.46	. carefully unus
142463	navajo deep almond seashell midnight	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED COPPER	32	JUMBO DRUM	1505.46	ar accounts.
142464	tomato chiffon khaki steel grey	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	22	MED CAN   	1506.46	tain packages. fluffi
142465	white powder ivory cornflower orange	Manufacturer#1           	Brand#14  	SMALL PLATED STEEL	47	SM JAR    	1507.46	l, re
142467	burlywood plum blue peru forest	Manufacturer#2           	Brand#21  	PROMO POLISHED STEEL	37	JUMBO JAR 	1509.46	ending de
144972	cornsilk cornflower misty aquamarine thistle	Manufacturer#2           	Brand#21  	SMALL ANODIZED TIN	35	SM BOX    	2016.97	deas. acc
144975	honeydew misty red deep magenta	Manufacturer#5           	Brand#52  	STANDARD PLATED STEEL	12	WRAP JAR  	2019.97	nto bean
144977	lemon ivory moccasin saddle magenta	Manufacturer#1           	Brand#15  	STANDARD ANODIZED TIN	15	SM CASE   	2021.97	carefully fi
144978	firebrick chiffon ivory sandy spring	Manufacturer#5           	Brand#52  	LARGE BRUSHED COPPER	21	SM JAR    	2022.97	unts. pending 
144979	almond mint powder honeydew orange	Manufacturer#1           	Brand#13  	LARGE POLISHED TIN	42	MED PKG   	2023.97	iously. fluffily pendi
144981	tomato sky lawn aquamarine wheat	Manufacturer#1           	Brand#14  	SMALL ANODIZED COPPER	14	MED PKG   	2025.98	nal instructio
147486	grey cyan plum lime thistle	Manufacturer#4           	Brand#42  	PROMO BURNISHED TIN	9	LG PKG    	1533.48	ans boost bl
147489	peru chiffon violet rose sky	Manufacturer#4           	Brand#43  	LARGE BURNISHED BRASS	19	LG PKG    	1536.48	the b
147491	chartreuse steel lime deep honeydew	Manufacturer#2           	Brand#22  	LARGE BRUSHED BRASS	5	MED BAG   	1538.49	oost alongside of
147492	dark pink olive cyan rose	Manufacturer#4           	Brand#44  	LARGE POLISHED BRASS	16	MED JAR   	1539.49	. carefully p
147493	maroon papaya brown chiffon lemon	Manufacturer#1           	Brand#15  	STANDARD POLISHED COPPER	13	LG CASE   	1540.49	se. c
147495	sandy midnight coral lemon chocolate	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED NICKEL	23	JUMBO BOX 	1542.49	fully special
150000	lemon dim violet royal dark	Manufacturer#1           	Brand#12  	PROMO PLATED NICKEL	48	LG CAN    	1050.00	ts cajole carefully s
150003	navajo peru light forest steel	Manufacturer#2           	Brand#22  	PROMO BURNISHED COPPER	33	WRAP BAG  	1053.00	ses. blithely silent
150005	firebrick drab puff grey lawn	Manufacturer#5           	Brand#51  	LARGE BURNISHED COPPER	50	JUMBO BOX 	1055.00	nts haggle 
150006	peru cyan medium thistle blanched	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED COPPER	16	WRAP JAR  	1056.00	impress blithely a
150007	antique seashell medium magenta yellow	Manufacturer#4           	Brand#43  	SMALL POLISHED COPPER	12	SM BOX    	1057.00	counts. regular deposi
150009	turquoise chocolate maroon linen cornflower	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED NICKEL	44	LG JAR    	1059.00	t the final, 
152455	powder peach frosted purple midnight	Manufacturer#4           	Brand#41  	PROMO PLATED BRASS	39	LG PACK   	1507.45	s boost furiousl
152458	peru dodger sienna lime peach	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED TIN	10	WRAP PKG  	1510.45	yly iron
152460	linen moccasin lime peach blue	Manufacturer#2           	Brand#22  	PROMO PLATED BRASS	29	LG DRUM   	1512.46	efully 
152461	red hot cornsilk orange floral	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	38	JUMBO PACK	1513.46	 foxes hinder qui
152462	drab wheat linen metallic peru	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED TIN	28	SM JAR    	1514.46	integrate qui
152464	salmon burlywood spring coral sienna	Manufacturer#2           	Brand#25  	STANDARD BRUSHED TIN	13	WRAP PKG  	1516.46	instructio
154970	moccasin peach antique honeydew medium	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED NICKEL	41	JUMBO CASE	2024.97	ounts. quickly regu
154973	chocolate magenta pale medium powder	Manufacturer#5           	Brand#53  	MEDIUM PLATED COPPER	44	LG PACK   	2027.97	he instructions. pa
154975	chocolate light almond rosy yellow	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	22	WRAP PACK 	2029.97	even dolphins integr
154976	navy thistle deep bisque navajo	Manufacturer#4           	Brand#41  	LARGE PLATED COPPER	27	LG DRUM   	2030.97	enticing
154977	blush sienna beige grey slate	Manufacturer#5           	Brand#53  	SMALL POLISHED COPPER	25	WRAP PKG  	2031.97	 pending packages. i
154979	goldenrod deep red steel orange	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED STEEL	15	MED DRUM  	2033.97	 accounts. f
157485	sandy azure orchid green tomato	Manufacturer#4           	Brand#44  	LARGE BURNISHED NICKEL	35	JUMBO JAR 	1542.48	lly unusu
157488	cornsilk azure moccasin chiffon ghost	Manufacturer#1           	Brand#15  	PROMO BRUSHED STEEL	26	LG CASE   	1545.48	s wake bravely 
157490	light cornsilk forest antique honeydew	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED TIN	46	MED PACK  	1547.49	. sometimes pending fo
157491	cornflower rosy deep salmon pale	Manufacturer#2           	Brand#23  	PROMO ANODIZED NICKEL	50	SM DRUM   	1548.49	rate furi
157492	almond burlywood olive tan medium	Manufacturer#5           	Brand#55  	SMALL BRUSHED STEEL	50	LG BOX    	1549.49	ly even ac
157494	bisque wheat cream antique saddle	Manufacturer#3           	Brand#33  	MEDIUM POLISHED STEEL	16	WRAP JAR  	1551.49	its. caref
160000	cornsilk moccasin thistle orchid honeydew	Manufacturer#4           	Brand#43  	ECONOMY POLISHED STEEL	13	SM CAN    	1060.00	t the furiously re
160003	medium black blue steel frosted	Manufacturer#2           	Brand#21  	PROMO BURNISHED STEEL	50	WRAP BOX  	1063.00	to beans should ha
160005	medium orchid grey frosted orange	Manufacturer#3           	Brand#32  	LARGE ANODIZED BRASS	8	JUMBO CASE	1065.00	hely iro
160006	lime navy maroon indian drab	Manufacturer#2           	Brand#24  	STANDARD BURNISHED STEEL	50	JUMBO JAR 	1066.00	 foxes cajole quic
160007	forest turquoise purple blush cornflower	Manufacturer#5           	Brand#52  	LARGE POLISHED COPPER	17	MED PKG   	1067.00	ronic instructions
160009	metallic violet antique puff seashell	Manufacturer#4           	Brand#44  	MEDIUM POLISHED COPPER	11	MED JAR   	1069.00	s requests detect 
162452	lace brown powder orchid tomato	Manufacturer#5           	Brand#52  	MEDIUM POLISHED COPPER	22	MED JAR   	1514.45	t, pending requ
162455	puff cyan saddle peru plum	Manufacturer#4           	Brand#41  	LARGE BRUSHED TIN	32	JUMBO CASE	1517.45	nts. furiously regu
162457	cornflower maroon steel blue deep	Manufacturer#4           	Brand#41  	SMALL BRUSHED NICKEL	36	JUMBO DRUM	1519.45	thely qu
162458	smoke powder olive goldenrod sandy	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED COPPER	14	LG DRUM   	1520.45	 requests about th
162459	white rosy yellow magenta firebrick	Manufacturer#5           	Brand#53  	PROMO ANODIZED COPPER	36	SM BOX    	1521.45	ons. sile
162461	white steel blue grey peru	Manufacturer#2           	Brand#23  	MEDIUM PLATED COPPER	1	SM BAG    	1523.46	 foxes boost c
164968	sienna violet linen grey chartreuse	Manufacturer#2           	Brand#24  	SMALL PLATED NICKEL	16	SM PACK   	2032.96	aves dazzle. final, re
164971	seashell hot orchid lime grey	Manufacturer#1           	Brand#15  	STANDARD PLATED TIN	9	MED PACK  	2035.97	packages impress
164973	mint chartreuse grey violet beige	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED COPPER	3	JUMBO PKG 	2037.97	ly final depo
164974	indian cream olive metallic firebrick	Manufacturer#2           	Brand#21  	MEDIUM POLISHED BRASS	40	WRAP DRUM 	2038.97	hlessly packag
164975	khaki drab grey green blush	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED BRASS	30	WRAP BAG  	2039.97	 pending packages are.
164977	goldenrod peru blush pale sienna	Manufacturer#3           	Brand#35  	LARGE ANODIZED TIN	8	SM PACK   	2041.97	y slyly unusual orbits
167484	almond lavender magenta lemon green	Manufacturer#1           	Brand#12  	ECONOMY PLATED BRASS	11	WRAP PKG  	1551.48	carefully even r
167487	violet seashell yellow salmon light	Manufacturer#2           	Brand#22  	LARGE BURNISHED NICKEL	1	WRAP BAG  	1554.48	aggle 
167489	khaki burnished drab burlywood dim	Manufacturer#4           	Brand#42  	PROMO ANODIZED STEEL	28	WRAP DRUM 	1556.48	ts. express,
167490	frosted orange smoke pink slate	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED BRASS	32	SM BAG    	1557.49	ly around the slyl
167491	hot lawn light ghost blanched	Manufacturer#3           	Brand#34  	PROMO BRUSHED BRASS	18	JUMBO BAG 	1558.49	slyly unus
167493	grey red khaki plum light	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED TIN	18	MED CAN   	1560.49	telets. furi
170000	tomato lemon burlywood chartreuse indian	Manufacturer#1           	Brand#11  	STANDARD BRUSHED COPPER	10	WRAP CASE 	1070.00	quests are ag
170003	purple lemon sienna brown metallic	Manufacturer#1           	Brand#12  	LARGE BURNISHED BRASS	15	MED CAN   	1073.00	ackages
170005	lawn beige lavender coral peru	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	19	JUMBO CAN 	1075.00	ly! exp
170006	tomato midnight ivory misty blanched	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED COPPER	34	WRAP JAR  	1076.00	ts. slyly
170007	papaya metallic peru yellow lemon	Manufacturer#2           	Brand#21  	SMALL ANODIZED TIN	35	WRAP JAR  	1077.00	ns se
170009	burnished hot cyan papaya grey	Manufacturer#1           	Brand#12  	STANDARD POLISHED NICKEL	10	WRAP PACK 	1079.00	cajole above
172449	wheat pale misty seashell cyan	Manufacturer#2           	Brand#21  	LARGE BRUSHED COPPER	39	SM CAN    	1521.44	ironic plate
172452	brown azure bisque wheat moccasin	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	44	LG PACK   	1524.45	leep. furiou
172454	beige blanched midnight firebrick thistle	Manufacturer#2           	Brand#23  	MEDIUM BURNISHED BRASS	31	SM CAN    	1526.45	lar asymptotes
172455	light turquoise dark blush saddle	Manufacturer#2           	Brand#24  	SMALL PLATED NICKEL	50	WRAP CASE 	1527.45	capades wak
172456	burlywood grey cornflower red salmon	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED NICKEL	11	JUMBO CAN 	1528.45	es af
172458	lime light red aquamarine yellow	Manufacturer#1           	Brand#15  	LARGE BRUSHED COPPER	48	WRAP JAR  	1530.45	special packages
174966	navajo seashell cyan sandy violet	Manufacturer#3           	Brand#34  	ECONOMY POLISHED COPPER	11	WRAP DRUM 	2040.96	yly re
174969	turquoise peach thistle saddle sky	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	7	LG PKG    	2043.96	slyly ironic
174971	seashell drab peach burnished royal	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED NICKEL	17	LG PACK   	2045.97	 integrate slyly. d
174972	turquoise aquamarine navy sandy cornsilk	Manufacturer#5           	Brand#55  	STANDARD PLATED BRASS	4	LG JAR    	2046.97	e blithely slyly iro
174973	grey floral lemon pale ivory	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED BRASS	12	LG JAR    	2047.97	n deposits
174975	navajo lemon cornflower azure aquamarine	Manufacturer#1           	Brand#12  	PROMO ANODIZED STEEL	39	WRAP PACK 	2049.97	r the carefully expres
177483	brown navy blue hot aquamarine	Manufacturer#4           	Brand#44  	STANDARD BURNISHED TIN	23	WRAP JAR  	1560.48	rouches sleep unusua
177486	black chiffon ivory purple blue	Manufacturer#4           	Brand#44  	ECONOMY POLISHED STEEL	3	LG DRUM   	1563.48	x bold pinto be
177488	cornflower firebrick beige powder spring	Manufacturer#4           	Brand#42  	LARGE BURNISHED COPPER	12	SM PACK   	1565.48	. flu
177489	papaya sandy peru olive misty	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED COPPER	40	JUMBO PACK	1566.48	ckly special asy
177490	chartreuse slate goldenrod honeydew drab	Manufacturer#5           	Brand#53  	STANDARD POLISHED TIN	19	JUMBO CASE	1567.49	will boost above 
177492	aquamarine orchid powder almond thistle	Manufacturer#4           	Brand#42  	PROMO PLATED BRASS	20	WRAP PACK 	1569.49	nal pe
180000	violet tomato almond navy cream	Manufacturer#3           	Brand#34  	STANDARD BRUSHED TIN	32	WRAP CASE 	1080.00	 regular packages. 
180003	burnished pale lawn misty indian	Manufacturer#2           	Brand#22  	PROMO BURNISHED TIN	13	WRAP CASE 	1083.00	riously ironic id
180005	burlywood cyan sky spring dark	Manufacturer#5           	Brand#54  	MEDIUM PLATED NICKEL	39	LG PACK   	1085.00	encies print furiou
180006	lemon pink pale yellow tomato	Manufacturer#1           	Brand#12  	SMALL BURNISHED STEEL	35	LG JAR    	1086.00	 theo
180007	rose aquamarine lime thistle deep	Manufacturer#2           	Brand#21  	PROMO BRUSHED COPPER	50	WRAP BAG  	1087.00	usly against 
180009	tomato red white pale sandy	Manufacturer#5           	Brand#53  	LARGE BRUSHED TIN	50	SM BAG    	1089.00	unts are slyly a
182446	firebrick indian seashell dark dim	Manufacturer#5           	Brand#52  	ECONOMY POLISHED STEEL	7	JUMBO BOX 	1528.44	 furiously qui
182449	slate navajo midnight lemon cornflower	Manufacturer#5           	Brand#51  	SMALL ANODIZED STEEL	27	JUMBO PKG 	1531.44	egular packages. instr
182451	tomato navajo navy papaya lawn	Manufacturer#4           	Brand#42  	STANDARD BRUSHED NICKEL	4	JUMBO PACK	1533.45	arefully carefu
182452	deep mint hot pink aquamarine	Manufacturer#1           	Brand#11  	MEDIUM PLATED NICKEL	45	WRAP PACK 	1534.45	ajole blithely u
182453	misty purple navy white rosy	Manufacturer#2           	Brand#23  	MEDIUM POLISHED NICKEL	46	MED BAG   	1535.45	 foxe
182455	grey white tomato burnished yellow	Manufacturer#4           	Brand#45  	STANDARD BRUSHED STEEL	5	WRAP CASE 	1537.45	al in
184964	blush royal puff firebrick floral	Manufacturer#2           	Brand#23  	STANDARD PLATED COPPER	23	SM CASE   	2048.96	arefully at 
184967	burlywood aquamarine antique tomato frosted	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	45	MED PACK  	2051.96	y even packages unwin
184969	smoke antique cornflower spring lemon	Manufacturer#4           	Brand#44  	ECONOMY PLATED BRASS	2	MED DRUM  	2053.96	nts. ideas again
184970	magenta turquoise yellow dim midnight	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED BRASS	2	SM PACK   	2054.97	 haggle slyly abov
184971	moccasin gainsboro yellow burnished light	Manufacturer#2           	Brand#25  	SMALL POLISHED BRASS	41	LG PACK   	2055.97	ests t
184973	metallic olive aquamarine lawn tan	Manufacturer#3           	Brand#35  	PROMO POLISHED TIN	5	WRAP CASE 	2057.97	detect slyly a
187482	drab blush frosted khaki sandy	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED TIN	37	LG BAG    	1569.48	gside of the quietly
187485	magenta snow seashell mint burnished	Manufacturer#4           	Brand#44  	SMALL POLISHED NICKEL	45	MED BAG   	1572.48	 players. bl
187487	rose moccasin pale salmon lawn	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED TIN	28	JUMBO DRUM	1574.48	lar requests
187488	blanched hot papaya azure aquamarine	Manufacturer#5           	Brand#52  	PROMO PLATED NICKEL	49	WRAP CAN  	1575.48	y. stealthily 
187489	almond floral tan burnished ivory	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	38	WRAP DRUM 	1576.48	ly reg
187491	goldenrod gainsboro blanched frosted sandy	Manufacturer#1           	Brand#15  	LARGE ANODIZED STEEL	50	MED PACK  	1578.49	s war
190000	beige ivory magenta gainsboro linen	Manufacturer#2           	Brand#21  	MEDIUM PLATED COPPER	23	MED CASE  	1090.00	ly accor
190003	white salmon lemon cornsilk ghost	Manufacturer#4           	Brand#41  	PROMO ANODIZED TIN	41	LG BAG    	1093.00	ckages according to th
190005	slate indian forest chartreuse rosy	Manufacturer#1           	Brand#11  	SMALL BRUSHED BRASS	3	SM CASE   	1095.00	ly blithe, regula
190006	navajo lavender smoke puff olive	Manufacturer#5           	Brand#55  	SMALL BRUSHED BRASS	35	LG CASE   	1096.00	ilent ideas boo
190007	khaki lime goldenrod pink grey	Manufacturer#1           	Brand#11  	STANDARD PLATED BRASS	30	SM PKG    	1097.00	fully final gift
190009	orchid goldenrod metallic frosted powder	Manufacturer#3           	Brand#33  	STANDARD ANODIZED COPPER	25	LG BAG    	1099.00	es cajole f
192443	blush hot firebrick deep ghost	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED BRASS	45	SM BOX    	1535.44	re ar
192446	royal turquoise misty burlywood plum	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED BRASS	38	MED DRUM  	1538.44	ckly ironic theodoli
192448	plum blanched azure cornsilk sky	Manufacturer#1           	Brand#11  	ECONOMY BURNISHED BRASS	23	JUMBO BOX 	1540.44	quest
192449	metallic forest cyan cornflower rose	Manufacturer#2           	Brand#23  	ECONOMY POLISHED STEEL	4	LG DRUM   	1541.44	s are slyly packages? 
192450	dodger sandy frosted navajo aquamarine	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED COPPER	45	WRAP CASE 	1542.45	s. quickl
192452	honeydew steel dark white hot	Manufacturer#5           	Brand#55  	LARGE BRUSHED TIN	47	MED BAG   	1544.45	 carefully express do
194962	white purple puff chocolate cornsilk	Manufacturer#1           	Brand#15  	SMALL BRUSHED TIN	23	JUMBO JAR 	2056.96	nic accou
194965	dark cornsilk drab smoke khaki	Manufacturer#4           	Brand#42  	LARGE BRUSHED NICKEL	24	MED CASE  	2059.96	ul packages sleep quic
194967	almond sandy pink lime olive	Manufacturer#3           	Brand#33  	SMALL PLATED TIN	8	MED JAR   	2061.96	e quickly final
194968	spring khaki orchid olive dim	Manufacturer#4           	Brand#45  	SMALL PLATED STEEL	30	WRAP DRUM 	2062.96	e. dogged packag
194969	frosted cyan bisque forest sienna	Manufacturer#5           	Brand#54  	LARGE PLATED BRASS	41	WRAP BOX  	2063.96	pending d
194971	tomato floral chiffon deep tan	Manufacturer#4           	Brand#43  	STANDARD ANODIZED TIN	25	SM BAG    	2065.97	 requests use after
197481	dim tan lace green blanched	Manufacturer#5           	Brand#52  	LARGE POLISHED BRASS	31	LG DRUM   	1578.48	cording to th
197484	spring lemon orchid blush navajo	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED COPPER	42	JUMBO PKG 	1581.48	gular ideas. boldl
197486	rose firebrick purple seashell midnight	Manufacturer#2           	Brand#21  	MEDIUM POLISHED BRASS	37	SM BOX    	1583.48	he furious
197487	misty pale pink midnight brown	Manufacturer#1           	Brand#12  	LARGE BURNISHED STEEL	2	JUMBO PACK	1584.48	! furiously unusua
197488	pale blue sienna navajo white	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	21	SM CAN    	1585.48	ts sleep. foxes
197490	aquamarine navy rose chiffon moccasin	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED STEEL	38	WRAP JAR  	1587.49	ts. quickly special
200000	peach royal cornsilk sky sandy	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED TIN	22	LG CAN    	1100.00	xes sleep quick
\.


--
-- Data for Name: partsupp; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.partsupp (ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment) FROM stdin;
3	4	4651	920.92	ilent foxes affix furiously quickly unusual requests. even packages across the carefully even theodolites nag above the sp
5	6	3735	255.88	arefully even requests. ironic requests cajole carefully even dolphin
6	7	8851	130.72	usly final packages. slyly ironic accounts poach across the even, sly requests. carefully pending request
7	8	7454	763.98	y express tithes haggle furiously even foxes. furiously ironic deposits sleep toward the furiously unusual
9	10	7054	84.20	ts boost. evenly regular packages haggle after the quickly careful accounts. 
2500	1	6971	97.65	cies haggle doggedly alongside of the quickly regular dependencies. fluffily ironic foxes use against the furiously regular dependencies. packages use slyly. reg
2503	4	568	825.31	fully bold packages. requests use slyly against the final, express accounts. regular instructions alongside of the furiously final packages are unusual, ironic ideas. dolphins use bli
2505	6	8164	389.24	yly regular courts. furiously ironic asymptotes lose furiously permanently daring ideas. slyly pending deposits cajole carefully across the carefully 
2506	7	9435	670.43	uffily even requests. carefully even accounts was. final packages use among
2507	8	4284	451.62	mptotes. closely pending accounts detect furiously requests. furiously perma
2509	10	7928	425.12	lites against the blithely silent courts boost stealthily unusual foxes. furiously unusual 
5000	1	510	865.95	ake carefully slyly final deposits. foxes boost furiously after the blithely special pa
5003	4	7336	671.03	usly bold foxes haggle fluffily according to the quickly special packages. blithely even packages sleep according to 
5005	6	7721	208.89	aggle blithely special deposits. final requests wake slyly carefully ironic deposits. ruthlessly express packages nag furiously across the regular, special ideas. theodoli
5006	7	9218	216.35	carefully. slyly regular pinto beans are furiously. final, even theodolites wake slyly accounts. slyly final a
5007	8	9218	211.85	mong the platelets nag final packages. carefully final pains detect quickly according to the silent dugouts. slyly silent instructions cajole. furiously bold plat
5009	10	9192	87.88	ending foxes wake near the quickly express deposits. silent, regular instructions wake slyly furiously fina
7500	1	3305	697.68	accounts are fluffily fluffily unusual excuses. ironic escapades wake blithely. silent dugouts boost regular requests. furiously ironic
7503	4	6564	222.81	er the quickly even packages. furiously bold ideas haggle carefully blithely ironic packages. final, unusual accounts are slyly. special, even deposits affix carefully after the furiously brave
7505	6	3282	88.29	the carefully regular packages. quickly final excus
7506	7	9097	509.39	ic, even theodolites. slyly final ideas wake furiously--
7507	8	7904	178.79	refully unusual deposits sleep furiously. ironic ideas haggle carefully. furiously unusual deposits wake permanently express theodolites. blithely final deposits haggle. c
7509	10	7178	481.58	cies. requests x-ray fluffily at the closely bold requests. quickly brave ideas detect blithely. bold foxes wake slyly. carefully even pinto beans alongside of the platelets nag slyly
10000	1	253	563.61	es according to the blithely even deposits solve pending, unusual requests. carefully unusual packages are carefully against the eve
10003	4	7827	787.97	ts boost. dugouts nag final forges. blithely regular packages cajole fluffily furiously final pains. s
10005	6	3463	496.38	unts use blithely blithely even foxes! carefully silent accounts about the blithely silent instructions unwind carefully alongside of the deposits. furiously ironic requests sleep sl
10006	7	7808	748.62	e furiously. final requests above the quickly even deposits boost carefully blithely even accounts-- ruthless pinto beans doubt carefully. blithely ironic 
10007	8	9682	554.10	y silent foxes. pending, even accounts believe even courts; furiously final theodolites according to the accounts use fluffily carefully regular requests. ironic dolphins sleep carefully. even pinto
10009	10	6742	701.78	iously slyly even instructions. slyly brave dolphins are slyly bold accounts! special packages are carefully pending i
12497	1	4839	502.99	 tithes. furiously regular platelets nag carefully express accounts. slyly even dependenci
12500	4	1892	900.83	y ironic instructions need to sleep along the carefully special packages. carefully ironic foxes around the regular, final foxes sleep
12502	6	1147	150.72	ironic ideas sleep quickly alongside of the quickly final epitaphs. ironic, 
12503	7	1774	875.24	requests are finally even requests. furiously bold packages haggle regular, pending foxes. daringly regular depo
12504	8	8653	994.41	fully permanently ironic dugouts! quickly regular requests boost furiously across 
12506	10	261	994.62	 accounts: requests boost carefully. bold ideas play furiously according to the carefully final deposits. final, final foxes by the furiously regular sentiments haggle quickly fur
14998	1	1777	552.89	osits are against the final accounts; blithely regular pac
15001	4	1402	692.24	 enticingly ironic excuses maintain slyly. carefully final escapades wake blithely express excuses. final platelets haggl
15003	6	4601	180.99	idly pinto beans. furiously pending accounts wake furiously. regular, express accounts according to the busily re
15004	7	112	303.98	 pending deposits engage ironic theodolites. furiously even instructions are at the quickly final excuses. unusual, final deposits against the carefully enticing ideas wake blithely against t
15005	8	6119	168.68	deas across the carefully ironic deposits integrate carefully blithely unusual dependencies. furiously regular packages are f
15007	10	5297	534.36	lly final excuses upon the blithely regular theodolites cajole quic
17499	1	9946	982.32	 pending theodolites wake slyly even excuses. fluffily brave realms are a
17502	4	4873	347.06	s. ironic requests according to the blithely ironic deposits cajole bli
17504	6	9537	146.45	ons. final, bold asymptotes haggle above the carefully regular warthogs. pending, dogged deposits atop the bold instructions w
17505	7	7637	738.73	. slyly pending accounts integrate slyly regular requests.
17506	8	5621	3.32	deas play blithely about the special ideas. deposits haggle among the blithely final fo
17508	10	3867	569.36	g pinto beans wake about the blithely unusual deposits:
74991	6	8418	565.65	yly regular theodolites thrash across the slyly express deposits? deposits cajole blithely. quickly express requests nag blith
20000	1	4397	203.29	ular deposits. express packages cajole. slyly final accounts use slyly. carefully bold ideas nag quickly. bold accounts was fluffily furiously final requests. carefully 
20003	4	4602	113.83	arefully pending deposits above the busy requests affix slyly even requests. slyly final deposits after the pending dependencies haggle carefully furiously final dependencies
20005	6	5156	265.59	ular accounts. furiously final deposits sleep slyly! thinly unusual accounts cajole blithely. ruthless ideas boost furiously grouches. fluffily unusual pin
20006	7	6298	314.52	according to the unusual, even ideas. blithely final deposits wake; accounts nod furiously final ideas. quickly even accounts a
20007	8	5895	475.61	slyly special requests wake according to the ironic excuses. excuses wake carefully regular packages. ironic, regular depo
20009	10	5308	720.73	attainments are carefully courts. furiously regular ideas haggle. regularly even excuses unwind alongside of the carefully iron
22494	1	8199	887.64	requests. carefully regular requests along the regular deposits cajole according to the slyly even dependencies-- accounts above the packages boost packages. quietly regular
22497	4	8205	551.63	packages are. blithely regular requests wake furiously slyly even packages. accounts haggle furiously about the ironic, bold multipliers? even accou
22499	6	3188	558.31	onic, final packages are carefully carefully even accounts. thinly even ideas above the fluffily even pinto beans sleep regular theodolites. furiously final p
22500	7	3596	896.06	l packages wake furiously. furiously express dependencies are blithely among the slyly express packages
22501	8	5032	879.83	equests. furiously regular courts cajole inside the bol
22503	10	5130	368.02	even, blithe theodolites. packages sleep never. carefully express frets among the blithely silent asymptotes wake slyly ironic pinto beans. regular, unusual instructions maintain. blithely ironic
24996	1	9227	152.65	dependencies boost. blithely ironic accounts haggle furiously. furiously unusual accounts haggle carefully. closely express packages are-- fluffily
24999	4	5092	181.43	kages after the bravely even deposits play carefully according to the ironic foxes. furiously ironic deposits a
25001	6	5584	27.45	y final instructions integrate carefully about the carefu
25002	7	5999	989.73	nd the quickly bold theodolites; bold dependencies sleep slyly about the carefully final dependencies. furiously special gifts are slyly ca
25003	8	4199	591.34	kages boost furiously silent excuses. slyly regular instructions solve after the fluffily bold ac
25005	10	7266	738.00	xcuses. requests sleep carefully regular theodolites. ironic foxes nag quickly among the carefully express packages. final foxes use. pinto beans haggle ev
27498	1	1953	170.95	dolphins sleep carefully about the special, unusual requests. blithely ironic foxes nag boldly above the pending ideas. slyly unusual dugouts affix among the thinly even deposits: blit
27501	4	7206	651.32	l deposits. carefully express ideas sleep. furiously dogged ideas use carefully even packages. express foxes cajole ironic packages. 
27503	6	4357	623.00	e special pearls boost according to the special, ironic requests. deposits affix slyly unusual instructions. regular, blithe ideas haggle slyly carefully fluffy deposits. slyly ironic deposits sl
27504	7	7102	752.27	 use slyly among the carefully ironic platelets. ironic excuses nag stealthily. furiously unusual ideas sleep. ideas nag quickly. final,
27505	8	7708	503.86	luffily express theodolites after the blithely final packages impress furiously unusual, regular accounts. even requests cajole fluffy requests. pending
27507	10	9679	768.05	blithely even pinto beans. blithely ironic sauternes are carefully. blithely ruthless reques
30000	1	1063	46.63	arefully alongside of the ironic, silent foxes. regular, regular accounts along the regular, regular instructions sleep evenly across the regular, express ideas! carefully bold hockey 
30003	4	7013	621.77	y final pinto beans boost furiously after the carefully express theodolites. 
30005	6	4501	859.88	lar deposits. carefully regular ideas wake blithel
30006	7	7940	939.46	ckly express deposits serve pinto beans. fluffily final deposits against the slyly careful packages grow unus
30007	8	3209	281.42	tructions wake blithely according to the quickly ironic reque
30009	10	8541	82.77	e slyly platelets. deposits wake. carefully bold asymptotes can sleep above the blithely pending instructions. carefully ironic pi
32491	1	7248	318.51	ally regular deposits along the blithely special theodolites are ironically blithely special 
32494	4	3323	939.19	ly unusual pinto beans serve carefully unusual packages. furiously regular sentiments boost quickly final instructions. s
32496	6	5283	808.78	wake quickly furiously ironic ideas. ironic packages alongside of
32497	7	4350	935.05	y. unusual excuses wake carefully. blithely pending packages use above the unusual, pending foxes. furiously ironic excuses kindle finally. busi
32498	8	6684	330.19	inal dependencies nag quickly special deposits. carefully final sheaves cajole after the blithely bold packages. slyly even instructions hinder enticingly slyly regular requests. slyly final
32500	10	9123	690.91	express theodolites haggle carefully even deposits. quickly unusual accounts alongside of the c
34994	1	3210	995.83	sleep slyly between the blithely regular accounts. carefully even theodolites are. regular, final instructions cajole slyly across the furiously bold deposits.
34997	4	3530	615.75	ornis among the asymptotes use quickly regular requests. quickly regular packages wake. blithely ironic ideas cajole accounts. theodolites haggle thinly above the 
34999	6	2192	649.00	 instructions serve slyly. express packages are blithely
35000	7	7024	875.36	al theodolites lose. slyly special ideas haggle blithely packages: slyly pending requests haggle blithely special instructions. packages cajole furiously past the furiously special courts. furious
35001	8	6147	383.66	o beans above the regular, regular packages dazzle furiously along the carefully regular theodolites. quickly pending packages are furiously furiously regular deposits. bravely ironic pa
35003	10	8329	382.33	kages. blithely express foxes promise even realms. theodolites cajole carefully after the furi
37497	1	5617	828.23	y ironic deposits are alongside of the express dependencies. permanently unusual requests wake carefully even deposits. fluffily 
37500	4	8835	54.68	nts impress furiously furiously regular packages! quickly regular accounts past the regular theodolites are blithely silent theodolites. furiously unu
37502	6	442	956.96	ar accounts try to nag slyly ironic pinto beans. quickly special instructions haggle. special, final requests are against the furiously special deposits. pend
37503	7	7115	371.16	to the slyly final packages. fluffily final deposits wake blithely ideas. special, final accounts nag carefully. ironic, express asymptotes across the carefully regular 
37504	8	394	158.03	efully silent accounts; furiously special courts kindle. blithely unusual packages haggle blithely. furiously express ideas haggle blithely furiously regular realms. furiously ironic instructions
37506	10	1106	996.08	thogs cajole fluffily bold theodolites. slyly even frays haggle 
40000	1	9808	945.41	 silent pains breach. even requests nag? ironic accounts affix slowly. slyly regular pinto beans cajole carefully. regular, even dinos sleep. fluffil
40003	4	5045	70.31	uriously bold accounts. express packages affix silent pinto beans. blithely unusual accounts use slyly along the silent ideas. quiet, 
40005	6	1460	62.69	osits haggle. carefully regular courts haggle across the slyly regular excuses. unusu
40006	7	1748	307.34	 deposits. furiously pending theodolites wake blithely along the ironic platelets. unus
40007	8	8630	294.07	ependencies use deposits. carefully even instruction
40009	10	3949	638.35	sly regular requests wake against the regular deposits. regular, special pinto beans cajole against the furiously unusual patterns. carefully final deposits haggle blithely about the f
42488	1	35	51.48	tructions. requests nag slyly according to the carefully regular dependencies. slyly even deposits sleep. fluffily final packages alongside of the ironic, bold accounts wake furiously fu
42491	4	4479	428.90	hely bold packages. slyly unusual packages affix carefully fluffy deposits? slyly special foxes nag blithely. even deposits across the enticing pains are fluffily across the bravely express cour
42493	6	1341	919.46	gular pinto beans. express, even deposits use after the blithely 
42494	7	1567	428.14	 blithely even packages detect carefully even ideas. blithely regular excuses nag quickly. ideas sleep slyly. fluffily unusual accounts are about the Tiresias. slyly ironic acco
42495	8	990	233.95	cross the accounts. quickly thin pinto beans boost silently. ruthless deposits haggle quickly above the slyly unusual theodolites. carefully thin accounts wake furiously against the 
42497	10	7826	664.41	ndencies. blithely bold accounts was asymptotes. slyly bold packages sleep alongside of the carefully regular deposits. ir
44992	1	1861	79.74	y regular requests. requests alongside of the carefully pending accoun
44995	4	8303	240.42	pecial, regular somas! final accounts must have to detect. special requests across the slyly express deposits doze against the fluffily bold
44997	6	2947	590.57	lar ideas haggle ironic packages. even, unusual accounts are.
44998	7	6531	346.97	sly since the ironic, ruthless deposits. express accounts sleep
44999	8	6830	88.93	 blithely. regular theodolites among the furiously final deposits nag against the carefully unusual deposits. carefully ironic packages cajole carefully according to the fluffy, even instructions? 
45001	10	4343	260.50	c pinto beans cajole. blithely final pinto beans print after the ironically bold foxes. bold deposits boost slyly. blithely ruthless instructions after the even requests hang blithely special 
47496	1	9660	804.47	lly express accounts play blithely furiously final packages. bold, silent ac
47499	4	3650	462.22	ngside of the unusual ideas. carefully unusual excuses ought to use at the fluffily quick theodo
47501	6	2083	672.98	pinto beans sleep carefully behind the express deposits. ideas use boldly along the packages. regular deposits haggle across the furiously ironic deposits. ac
47502	7	9297	653.02	 blithely furiously even courts. blithely ironic excus
47503	8	5372	205.45	unts haggle deposits. furiously silent accounts doubt blithely along the ironic, unusual attainments. dep
47505	10	5378	515.49	aggle slyly quickly silent requests. furiously express instructions are furiously about the slyly bold excuses. ironic packages unwind quickly slyly pending dugouts. quickly regular theodolites alo
50000	1	6628	389.00	en packages cajole slyly. carefully pending pinto beans wake regular, express deposits. fluffily final accounts haggle. fluffily final dependencies detect carefully unusual requests. blithely
50003	4	5701	745.93	ide of the furiously express sentiments. regular asymptotes wake asymptotes. regular pinto bean
50005	6	9062	276.69	ependencies sleep. even theodolites integrate about the d
50006	7	4989	946.76	. fluffily unusual pearls wake carefully final pinto beans. even pinto beans play. regular, regular pinto beans wake furiousl
50007	8	853	104.11	efully across the slyly regular ideas. blithely ironic
50009	10	4648	10.48	 pinto beans hang fluffily special pinto beans. final platelets affix fluffily final pinto bea
52485	1	7811	345.35	ers cajole slyly regular hockey players. carefully regular packages are about the quickly unusual accounts? carefully unusual ideas use fluffily about the carefully final courts. fur
52488	4	1846	380.11	 pinto beans. slyly ironic instructions sleep. packages cajole carefully along the quickly ironic accounts: dependencies sleep blith
52490	6	2474	759.52	ironically even packages. even dolphins cajole quickly. blithely pending requests sleep. quickly bold theodolites use 
52491	7	7323	391.78	y regular theodolites. final requests sleep blithely. special, even accounts wake carefully against t
52492	8	6324	966.06	ly unusual packages haggle after the slyly silent deposits. requests wake. accou
52494	10	2865	276.88	ly special requests! frays cajole blithely. express foxes wake blithely regular ideas. slyly final asymptotes integrate slyly 
54990	1	588	644.17	losely regular deposits wake. slyly regular accounts dazzle quickly. ironic, ironic dependencies integrate blithely among the slyly ironic packages. bold, unusual requests among th
54993	4	5696	426.78	ts thrash slyly. furiously slow deposits nag according to the ironic theodolites! slyly final sentiments wake blithely regular waters. even, final 
54995	6	1680	192.91	ts among the furiously pending pinto beans sleep 
54996	7	9497	127.11	y-- patterns may cajole across the theodolites. never final accounts against the ironic, regular waters haggle after the asymptotes. blithely reg
54997	8	3302	302.16	pliers are blithely ironic warthogs. unusual, final instructions cajole. quickly final accounts grow carefully regu
74992	7	5674	703.57	luffy accounts about the slyly regular requests are d
54999	10	4526	59.79	usly unusual packages boost slyly. regular packages haggle furiously even, even packages. ironic, regular attainments eat carefully final theodolites. stealthily regular ideas hag
57495	1	4997	924.52	 blithely silent requests. furiously unusual deposits sleep. platelets sleep. carefully ironic foxes wake fluffily unusual requests. special packages impress fu
57498	4	4355	490.90	ges nag blithely slyly unusual courts. quickly ironic asymptotes use carefully quickly regular asymptotes. slyly special requests wake furiously s
57500	6	2945	53.60	unusual accounts. ironic, final foxes above the carefully b
57501	7	8746	611.48	quests. closely regular platelets wake furiously. fluffily unusual deposits are fluffily. carefully final requests sleep. requests impress among the regular, even sheaves. furiou
57502	8	39	408.52	n packages. quickly furious dependencies wake slyly since the carefu
57504	10	8839	161.03	 carefully unusual foxes try to wake furiously requests. quickly express dependencies cajole furiously ironic accounts! fluffily special packages haggle carefully carefully ironic pinto beans. pac
60000	1	4491	692.11	furiously pending instructions boost carefully among the fluffily blithe asymptotes. slyly bold packages wake slyly. furiously ironic deposits are even, pending platelets. even, even requests na
60003	4	3166	779.45	hely unusual requests about the slyly even accounts wake bold deposits. final courts cajole carefully against the silent, regular foxes. carefully regular deposits wake. unus
60005	6	6613	314.60	tegrate carefully. unusual, fluffy ideas haggle slyly about the bold deposits. even deposits according to the pending requests use f
60006	7	3026	7.95	nto beans after the ironic platelets sleep pinto beans. quickly brave id
60007	8	466	279.60	ilent accounts sleep slyly fluffily ironic requests! slyly regular packages nag carefully. regular pint
60009	10	7369	27.48	nts before the final pinto beans integrate blithely after the quickly regular accounts. foxes alongside of the slyly regular i
62482	1	9001	576.52	silent foxes haggle ironic, express requests. sly
62485	4	7857	846.81	are quickly furiously final packages. instructions boost carefully. final accounts detect. carefully regular foxes mold carefully bold accounts. idly
62487	6	3389	426.95	side of the final, even instructions. slow packages haggle furiously about the car
62488	7	9889	128.54	s. slyly ironic requests wake against the slyly final instructions. even platelets unwind furiously. silent depo
62489	8	6014	422.18	arefully along the unusual accounts. carefully thin frets across the carefully final asymptotes snooze slyly blithely unusual requests. carefully final asymptotes haggle ca
62491	10	5666	143.13	 carefully against the ironic dolphins. final pinto beans according to the requests use quickly against the packa
64988	1	4629	938.78	counts haggle slyly after the bold, special instructions. unusual, furious requests sleep blithely quickly regular the
64991	4	8254	827.15	 pinto beans. even dinos according to the furiously ironic packages cajole slyly regular accounts. accounts nag furiously e
64993	6	1704	863.36	hely regular dependencies sleep against the pending r
64994	7	1641	769.18	 fluffily bold accounts nod even deposits. final pinto beans wake blithely. slyly even deposits haggle slyl
64995	8	3699	943.26	ainst the slyly ironic requests. final packages affix slyly fluffily final packages. quickly
64997	10	2424	727.76	instructions sleep carefully fluffily special waters. slyly express accounts cajole above the deposits! furiously ironic requests against the ironic dependencies cajole carefully ruthles
67494	1	5665	469.87	ate. blithely ironic theodolites haggle fluffily pending instructions-- regular accounts nag slyly express, final instructions. quickly final accounts 
67497	4	1662	675.29	luffily special dolphins detect slyly along the furiously 
67499	6	2978	764.17	telets wake silent, even pinto beans. blithely final pinto beans c
67500	7	8208	216.17	 express accounts impress slyly dolphins. furiously final requests hang fluffily above the ironic packages. fluffy packages solve blithely regular sheaves. final requests sleep regularly fin
67501	8	1593	197.17	ages. furiously regular requests was slyly furiously regular pinto beans. furiously even requests are blithely quickly silent accounts. carefully fi
67503	10	8301	488.26	s are about the slyly ironic warhorses. furiously regular accounts affix furiously. 
70000	1	9060	282.60	its detect furiously special deposits? even, ironic packages sublate fluffily. silent 
70003	4	7762	67.82	ly against the unusual, final foxes. ironic, permanent theodolites cajole furiously. blithely 
70005	6	3316	38.25	ic warthogs. blithely special requests boost carefully ironic, ironic dependencies. deposits after the regular dependencies boost after the quickly fina
70006	7	5917	785.26	ly even deposits. pending, final accounts nag. blithely ironic packages wake after the quickly ironic packages. express asymptotes wake blithely. furiously silent accounts cajole qu
70007	8	5061	685.61	tructions haggle fluffily pending accounts! final dinos doze furiously above the furiously final foxes. slyly ironic excuses along the furiously even pinto beans haggle fur
70009	10	1404	664.74	ve the final depths. carefully final ideas wake blithely: blithely final r
72479	1	9514	616.92	requests cajole blithely above the bold requests. blithely pending platelets dazzle despite the accounts. carefully regular d
72482	4	253	64.79	kages nag blithely slowly regular excuses. special packages haggle furiously. deposits use about the dependencies. permanently regular accounts wak
72484	6	6129	1.03	. silent asymptotes mold enticingly above the unusual, special requests. blithely
72485	7	3352	783.86	y cajole quickly about the regular accounts. silently pending theodolites snooze behind the blithely even ideas. furio
72486	8	3483	310.73	totes. instructions cajole fluffily ironically final packages. quickly unusual excuses was quickly. furiously special requests mold blithely. slyly express foxes against the quic
72488	10	592	553.61	e-- furiously regular foxes sleep furiously furiously regular platelets. attainments after the quickly bold requests cajole furiously blithely ruthless pinto beans. quick inst
74986	1	3351	963.94	ing to the slyly regular accounts. slyly final ideas are fluffily. ideas above the bold courts cajole slyly furi
74989	4	6070	101.40	ve the slyly final instructions. quickly silent foxes alongside of the slyly even accounts are slyly final, regular plat
74993	8	6483	946.55	lyly express theodolites. carefully ironic foxes haggle alongside of the asymptotes. carefully pending foxes was slyly according to the carefully ironic dolphins! fluffily unusual 
74995	10	4748	286.25	 final excuses sleep slyly above the bold, silent accounts. furiously ironic ideas are according to the slyly regular pinto beans. regular asymptote
77493	1	5920	220.80	 final packages. unusual, final requests integrate slyly according to the final, final excuses. slyly pending deposits haggle for the bold pinto beans. furiously special i
77496	4	9818	600.06	. carefully regular platelets use ideas. quickly r
77498	6	6505	466.83	mold. slyly regular packages wake quickly carefully regular requests. closely bold packages above the quickly p
77499	7	1513	53.27	pinto beans. furiously final requests sleep blithely. ironic pinto beans wake careful
77500	8	598	899.50	 regular accounts. regular packages wake fluffily according to the slyly ironic
77502	10	6588	798.63	ecial platelets. ironic, ironic instructions mold slyly across the special, 
80000	1	9933	248.07	ar packages about the final accounts use blithely at the requests. carefully even ideas haggle. even, regular dependencies nag stealthily regular plat
80003	4	7121	925.81	ets cajole about the unusual requests. ironic accounts are enticingly. slyly special pinto beans eat. special ideas wake slyly above the pl
80005	6	609	573.78	eposits! fluffily regular ideas are alongside of the
80006	7	1543	101.94	dly. blithely ironic requests about the express accounts are ironic, pending asymptotes. carefully regular accounts detect. qui
80007	8	2397	770.70	ccounts nag idly at the carefully ironic accounts. blithely 
80009	10	8572	855.06	e packages. furiously bold requests mold quickly. foxes sleep quickly above the furiously express deposits. furiously even accounts wake care
82476	1	3906	272.23	gle carefully against the furiously special instructions. slyly ironic packages haggle b
82479	4	5956	476.82	ole quickly. final pinto beans cajole slyly through the closely final notornis. furiously final deposits cajole above the quickly regular accounts? grouches could 
82481	6	2562	287.56	hely regular packages. blithe packages alongside of the final ideas are slyly quickly f
82482	7	7277	973.82	ccounts alongside of the slyly pending instructions boost furiously along the fluffily
82483	8	6631	587.84	even deposits use carefully even packages. regular instructio
82485	10	6438	593.16	 ironically regular warthogs grow beyond the carefully regular requests: ironic, daring pinto beans according to the stealthily daring asymptotes kindle bli
84984	1	6277	832.44	 blithely regular theodolites-- slyly ironic packages sleep along the slyly regular packages-- bold foxes are furiously regular requ
84987	4	9292	797.68	ng the bold foxes sublate slyly against the slowly idle deposits. final, ironic ideas cajole boldly. carefully ironic foxes wake. ironic dependencies wake never carefully unus
84989	6	1602	1.91	 theodolites integrate around the furiously express warthogs: carefully pending instructions integrate ironically even packages. fluffy deposits 
84990	7	7657	74.29	kages after the carefully express courts wake permanently even accounts. bold, ruthless requests lose carefully. quickly ironic depos
84991	8	9620	982.53	nic theodolites nag furiously among the regular dolphins. silent packages cajole. quickly pending accounts nag. furiously ironic waters haggle quickly. pending, pending deposits haggl
84993	10	8049	895.68	lar, regular requests. slyly express pinto beans nag slyly! slyly even multipliers was. regular requests nag carefully. regular theodolites use slyly around the slyl
87492	1	1476	38.97	yly ironic packages above the furiously silent acco
87495	4	7635	406.17	ages integrate furiously furiously bold ideas. brave, ironic asymptotes nag blithely quick platelets. furiously regular foxe
87497	6	1094	109.37	aphs. quickly unusual requests poach furiously around the idle, ironic d
87498	7	1544	311.74	ideas. ironic platelets cajole furiously. blithely bold packages use quickly regular packages. regular accounts cajole regular deposits;
87499	8	3311	925.80	old instructions. slyly ironic deposits are bravely alongside of the carefully even package
87501	10	3995	559.24	gular theodolites sleep furiously bold, regular sentiments. blithely express requests are furiously final dolphins. excuses play quickly. furiously ir
90000	1	8758	548.40	y express packages integrate against the furiously ironic theodolites. final ideas sleep slyly along the bold, regular requests. pending packages doze. quickly re
90003	4	7584	67.33	n excuses impress blithely above the carefully pending pinto beans. requests snooze slyly after the furious
90005	6	6207	824.56	otes use furiously. slyly pending warhorses alongside of the quickly regular requests cajole across the blithely regular courts. slyly ev
90006	7	5686	31.98	ts along the final deposits breach pending accounts. quickly final dolphins engage. blithely brave pinto beans integrate blithely. slyly ironic theodolites nag carefully f
90007	8	5621	984.01	ly ironic theodolites. carefully final platelets detect furiously according to the special asymptotes. carefully sly dependencies haggle b
90009	10	3472	772.81	 regular theodolites. final, express requests are carefully special, pending forges! blithely unusual excuses serve f
92473	1	7292	284.82	bove the closely silent accounts x-ray alongside of the final ideas. furiously pending instructions serve. regular requests use. even ideas affix
92476	4	7349	283.42	s nag blithely along the special, final platelets. accounts cajole quickly about the stealthily ironic instructions. depend
92478	6	9012	521.77	ove the regular requests. blithely express dependencies cajole final theodolites. furiously ir
92479	7	1400	50.10	ickly; regular packages cajole ironic dolphins. quickly unus
92480	8	8389	912.94	ts breach furiously around the slyly regular platelets. deposits under the ironic pinto beans are after the fluffily regular pa
92482	10	7407	567.87	inst the carefully regular theodolites. regular deposits promise furiously ironic requests. packages boost carefully express accounts. furiousl
94982	1	5208	113.62	egrate. bold, final ideas around the pending deposits integrate blithely carefully perma
94985	4	4251	26.17	 final instructions boost carefully according to the regular multipliers? carefully fluffy deposits use permanent requests. express, silent pains haggle. even courts are carefully a
94987	6	5283	627.12	ites. furiously express deposits wake quickly furiously regular foxes.
94988	7	946	670.31	eposits around the foxes haggle ironic packages. packages engage near the requests. stealthily 
94989	8	1145	619.88	furiously special, regular accounts. regular, regular ideas haggle furiously regular sheaves. final instructions sleep. pinto beans hagg
94991	10	4417	551.71	round the accounts. quickly daring foxes about the quickly final requests boost
97491	1	8558	56.34	eodolites wake blithely. regular foxes x-ray carefully carefully ironic deposits. regular theodolites are according to the ironically final accoun
97494	4	4242	885.19	ual requests wake furiously among the ruthless, idle packages. final accounts along the blithely final pinto beans wake about the express instructions. blithely regular packages are furiously e
97496	6	8306	444.09	ronic foxes. ironic, dogged instructions against the bold deposits cajole furiously pending, final asymptotes. special 
97497	7	3629	886.18	gle slyly blithely final packages-- final deposits wake-- fluffily regular packages against the carefully final deposits sleep ironic accounts. special, unusua
97498	8	1064	665.99	s packages. quickly regular accounts affix. silent packages x-ray furiously. regular, ironic accounts sleep according to the regular accounts. furiously express instructions are
97500	10	543	704.78	ely even requests sleep. final packages wake quickly alongside of the 
100000	1	5944	577.80	unts. foxes sleep quickly. quickly regular deposits nag slyly regular in
100003	4	4086	975.91	g furiously. furiously regular dependencies x-ray. theodolites must nag requests. furiously even asymptotes boost along the furiously final pinto beans. express, regular requests ha
100005	6	6320	378.27	s sleep. silent, pending theodolites about the fluffily ironic requests use slyly blithely express
100006	7	3299	887.13	gside of the bold pinto beans. silent, final grouches haggle quickly against the pending dolphins. silent, unusual packages boost slyly. even, unusual courts a
100007	8	5468	28.49	ual, enticing gifts detect slyly furiously ironic instructions. quickly express deposits cajole. carefully pending accounts use slyly alongside of 
100009	10	8587	581.49	 quietly even packages. carefully careful deposits nag after the unusual, even requests. regular, express pains sleep. even deposits haggle: thinly pending requests sleep regular, re
102470	1	7520	604.89	ully blithely final packages. always silent pinto beans above the quickly unusual foxes cajole slyly among the asymptotes. special acco
102473	4	295	781.41	e carefully beyond the bold requests. even ideas 
102475	6	4907	32.22	y. carefully unusual pains according to the unusual deposits impress carefully furiously final deposits. pending dolphins grow a
102476	7	7889	488.37	the special, ironic accounts. slyly regular platelets across the regul
102477	8	4569	203.28	ges cajole slyly. requests haggle carefully furiously final grouches. regular pains sleep furiously. pending ideas nag
102479	10	71	369.11	lly regular requests sleep slyly dugouts. unusual instructio
104980	1	2861	348.21	along the unusual requests affix alongside of the slyly regular platelets. slyly bold excuses
104983	4	3168	497.25	dencies believe slyly even accounts. closely bold foxes snooze blithely. blithely
104985	6	9843	53.32	y close platelets. blithely final packages wake daringly above the d
104986	7	7543	849.58	carefully daring ideas. dependencies wake blithely in place of the quickly unusual packages. bold, final
104987	8	4776	103.85	nusual ideas boost quickly regular foxes. pains sleep quickly for the asymptotes. doggedly express excuses cajole ironic platelets. blithely unusual excuses against the courts are qu
104989	10	6839	774.76	counts use fluffily alongside of the requests; carefully ironic asymptotes wake. final, final packages cajole furiously about the slyly ironic packages. carefully regular instructions kindl
107490	1	4684	676.22	ke slyly regular realms. quickly bold theodolites sleep carefully unusual deposits. silent
107493	4	5457	178.98	r packages affix furiously quickly final asymptotes. slyly reg
107495	6	514	871.19	 silent foxes wake above the realms-- even foxes across the furiously even accounts sleep furiously unusual requests. furiously ironic requests among the regular packages hagg
107496	7	8828	234.20	ts. furiously quick deposits haggle silently above the slyly unusual theodolites. even dolphins are furiously unusual theodolites. 
107497	8	3876	11.95	 ironic, even asymptotes-- quickly regular ideas about the deposits detect silent theodolites
107499	10	3470	122.89	e instructions boost behind the carefully bold deposits. regular packages wake furiously ironic requests. furiously ironic warhorses maintain carefully. enticingly ruthless 
110000	1	8121	547.82	packages engage slyly deposits; quickly ironic requests wake. packages according to the blithely idle dependencies are sl
110003	4	213	281.70	usy requests. regular, regular requests haggle carefully ironically ironic pinto beans. furiously pending instructions boos
110005	6	7142	841.55	ronic dependencies. final dependencies snooze requests! furiously express foxes play. fluffily regular foxes nag.
110006	7	861	851.50	e furiously deposits. regular accounts unwind furiously against the carefully unusual theodolites. slyly regular accounts must are
110007	8	4596	943.28	against the even, express accounts. regular instructions us
110009	10	5225	188.46	express deposits along the slyly final dependencies boost alongside of the slyly silent accounts. even ideas wake carefully. bold d
112467	1	6818	651.77	lent, unusual instructions. evenly pending pinto beans nod furio
112470	4	4291	707.18	y asymptotes. carefully special platelets use furiously bold patterns: even requests snooze blithely. idly final frets against the s
112472	6	9929	558.52	ependencies. unusual pinto beans above the slyly bold accounts x-ray regular ideas. accounts against the ironic accounts are around the permanently silent pearls. silent requests wake quickl
112473	7	7750	703.02	ts about the ironic theodolites detect at the even accounts. always spe
112474	8	6930	688.68	quests. blithely special packages wake blithely. blithely ironic packages are about the slyly even deposits. quick, ironic d
112476	10	1389	398.13	to beans among the bold, regular deposits haggle blithely furiously ironic foxes. blithely regular accounts detect quickly. regular, ironic requests p
114978	1	9265	783.82	ar deposits around the furiously final requests use carefully across the quickly unusual realms. blithely regular theodolites are quickly. blithe
114981	4	2630	43.85	ages haggle fluffily above the blithely bold requests. carefully careful foxes above th
114983	6	963	364.70	ct quickly forges. packages are. unusual pinto beans doubt requests. stealthily pending deposits wake furiously final platelets-- blithely express packages snooze furiously upon the special, i
114984	7	4609	772.35	s serve carefully quickly express packages. carefully final accounts sleep. slyly silent braids boost according to the even platelets. accounts against the final, express packages haggle quickly a
114985	8	6803	96.61	furiously express theodolites run always regular r
114987	10	7520	602.10	t warhorses. requests are. furiously unusual excuses are about the ironic sauternes. even theodolites are quickly across the carefully regular accounts. slyly ironic orbits above the ironic, i
117489	1	4190	760.49	ns about the furiously final ideas use closely above the furiously final theodol
117492	4	6254	817.85	lithely express deposits. furiously even foxes wake furiously express requests. regular, ironic de
117494	6	7994	784.54	sleep furiously packages. deposits cajole quickly carefully even
117495	7	1008	604.57	. regular, pending realms cajole carefully. pending foxes believe carefully. ironic, pending deposits nag
117496	8	2113	149.31	xpress forges use across the blithely silent packages. quickly fina
117498	10	1788	499.68	ckages wake furiously ironic, final theodolites. furiously pending deposits are fluffily. ironic requests wake fluffily about the regular, final 
120006	7	8719	592.81	requests x-ray. express, regular deposits nag fluffily. pinto beans haggle carefully. bold deposits across the fluffily even asympto
120000	1	2357	39.73	to integrate. furiously even requests haggle carefully. blithely expres
120003	4	5852	487.52	le slyly along the final, regular accounts? foxes above the dependencies promise carefully furiously silent deposits. ironic requests cajole furiously across the quickly regu
120005	6	5017	554.41	 ruthless accounts boost fluffily; special, silent platelets cajole. slyly 
120007	8	4052	321.07	packages affix blithely accounts. carefully ironic pinto beans sleep blithely? blithely express dependencies believe blithely slyly regular accounts. furiously regular requests
120009	10	5452	275.71	pecial theodolites. carefully final platelets haggle blithely regular accounts. blithely bold requests detect blithely daring deposits. carefully unusual patterns wake. furi
122464	1	554	521.19	gular foxes-- furiously quiet platelets haggle slyly against the slyly bo
122467	4	6017	612.10	ly even Tiresias. deposits nag carefully on the slyly special pinto beans. sly packages promise furiously. brave, final deposits haggle express deposits. slyly special pinto beans are carefully a
122469	6	203	473.22	 the carefully bold instructions nag quickly about the even foxes. slyly final theodolites wake blithely among the pending, unusual requests. slyly final d
122470	7	1615	62.19	xpress packages. fluffily pending asymptotes are alongside of the ironic, pending dependencies. blithely pending instructions hag
122471	8	8921	240.02	ar accounts cajole slyly regular frays. special deposits boost carefully final packages. furiously final foxes are carefully. slyly even attainments impress carefully across the fin
122473	10	1979	231.21	ges engage pending, unusual packages. blithely blithe deposits sleep slyly iron
124976	1	9010	705.48	ly. unusual, final dugouts use careful, unusual deposits. permanent packages along the bold deposits use furiously special asymptotes. carefull
124979	4	6813	163.46	gular instructions. accounts nag blithely blithely even deposits. boldly pending accounts nag carefully according to the accounts. ironic deposits according to the slyly fin
124981	6	3663	794.65	final accounts boost furiously deposits. permanently idle ideas play slyly fluffy sauternes. idle ideas sleep doggedly across the regular deposits. furious
124982	7	1328	957.80	eep slyly. quickly regular pains use final, final requests. carefully ironic packages cajole fluffily slyly unusual requests. furiously bol
124983	8	8297	968.31	en theodolites nag quickly-- unusual, bold requests cajole fluffily above the f
124985	10	8728	504.28	ests haggle. carefully final instructions wake fluffily. blithely regular packages across the carefully special
127488	1	1694	625.67	ic packages; quietly even dependencies breach. carefully unusual asymptotes haggle along the even dependencies: foxes boost regularly. expre
127491	4	1719	306.12	 unusual requests after the fluffily brave pearls haggle about the fluffily enticing Tiresias. accounts since the bold instr
127493	6	5813	718.29	ts. regular, express deposits haggle blithely regular deposit
127494	7	3618	450.15	fily between the carefully ironic depths. unusual pinto beans haggle slyly above the pinto beans. quickly reg
127495	8	8852	488.25	nstructions cajole pending excuses. slyly express asymptotes alongside of the furiously regular requests sleep slyly according to the quickly regular packages. slyly i
127497	10	4977	745.93	tructions. bold, final accounts promise carefully pen
130000	1	3558	516.36	ding excuses. silent warthogs about the daringly express orbits wake pe
130003	4	2610	262.11	ions sleep slyly. fluffily regular attainments haggle slyly across the blithely silent ideas. carefully regular requests cajole careful
130005	6	1172	644.67	pinto beans; quickly final deposits above the ironic frays wake carefully regular requests. unusual accounts along the blithely regular req
130006	7	1339	495.95	ily special foxes. blithely special packages wake among the slyly final account
130007	8	1260	366.83	 the instructions. carefully final deposits maintain furiously regular, ironic requests. packages wake quickly. silent, express excuses wake slyly along t
130009	10	10	345.76	ts are quietly dogged accounts. asymptotes serve slyly final packages. furiously final deposits sleep about the daringly unu
132461	1	1206	37.53	 dugouts. even accounts wake according to the furi
132464	4	5146	438.12	y regular platelets haggle above the pending accounts. dep
132466	6	7758	400.27	ake among the blithely bold requests. daringly ironic requests wake. theodolites boost along the furious platelets-- special, final deposits nod after
132467	7	9214	457.77	s sleep carefully around the regular foxes. fluffily enticing pinto beans above the unusual requests maintain furiously around the carefully unusual platelets. fur
132468	8	6926	133.23	 the furiously furious theodolites. epitaphs boost among the instructions. dependencies detect above the blithely bold excuses. regular somas thrash slyly. carefull
152461	7	1307	943.74	leep furiously. furiously pending excuses cajole final, ironic dependencies. fluffily pending theodolites 
132470	10	8882	121.22	yly. carefully regular packages sleep furiously furiously ironic dependencies. express, silent warthogs sleep. quickly final excuses across the decoys nag among the express 
134974	1	7143	463.00	pades poach blithely ironic accounts. regular somas along 
134977	4	5537	380.43	e unusual, express accounts. regular epitaphs snooze blithely after the slyly regular dolphins! foxes sleep furiously quickly regular decoys. slyly even depths are abou
134979	6	4469	328.29	 wake slyly. braids impress fluffily! quickly regular dolphins sleep enticingly quickly regular platelets. carefully regular foxes need to cajole fluffily bold reques
134980	7	1396	825.37	lites. requests along the blithely ironic dolphins cajole carefully slyly bold pinto beans. quickly regular packages across the excuses sleep even pinto beans. furiously unusual courts after the reg
134981	8	4037	29.25	 detect furiously accounts. carefully bold packages sleep furiously. ruthlessly ironic ideas impress. furiously pending accounts cajole blithely slyly final packages. packages along the regular, re
134983	10	1111	597.27	kages use carefully above the slowly unusual deposits! express accounts use quickly. even pinto beans sleep blithely bold foxes. quickly unusual instructions haggle furiously ironic pack
137487	1	9322	647.12	 special accounts use carefully slyly bold pinto beans. slyly final courts sleep blithely. ideas x-ray blithely. 
137490	4	6587	813.19	oze fluffily about the fluffily express deposits. slyly regular dinos cajole furio
137492	6	9786	298.08	gular requests are stealthily regular foxes. doggedly even courts detect about the caref
137493	7	502	433.03	ithely final pinto beans. blithely even instructions cajole brave pinto beans. special pinto beans against the slyly even 
137494	8	5705	374.26	te along the slyly even dependencies. final, regular accounts nag quickly. fluffily even packages eat unusual packages. final 
137496	10	3308	393.62	ithely around the fluffily express courts-- slyly eve
140000	1	4339	445.68	ffily careful packages wake finally after the furious
140003	4	3983	399.17	en packages. quickly pending asymptotes engage slyly. quickly special requests integrate above the care
140005	6	960	214.48	inal foxes sleep permanently about the carefully final platelets. dependencies unwind furiously against the blithely final foxes. carefully regular theodolites wake alongside of the f
140006	7	1510	544.78	ar foxes. closely regular requests eat fluffily. final, unusual packages hinder closely after the furiously silent requests. r
140007	8	2613	76.33	uses wake slyly carefully regular packages. regular, special theodolites run pinto beans. quickly final orbits aga
140009	10	5164	630.09	oubt blithely. furiously unusual requests are. carefully sil
142458	1	6150	973.09	ending theodolites. slyly even instructions after the furiously regular packages nag blithely spec
142461	4	2072	831.13	ronic warthogs. even requests detect blithely. slyly ex
142463	6	1957	420.80	nag. carefully regular requests maintain slyly among the furiously pending asymptotes. furiously regular accounts after the quickly regular requests x-ray carefully furiously pending asymp
142464	7	3504	912.41	ely special packages cajole. carefully even patterns haggle. blithely regular accounts sleep. ironic packages abov
142465	8	5669	611.85	ss the slyly unusual accounts sleep blithely silent pinto beans. slyly ironic platelets abo
142467	10	8750	717.48	 about the slyly pending instructions sleep doggedly bold, special accounts. quickly bold deposits sleep blithely. ruthless req
144972	1	8418	324.08	y final notornis-- pinto beans are quickly after the ironic, regular pinto beans. ironic deposits use blithely courts. ruthless theodolites print sl
144975	4	9183	649.40	 the slyly express foxes use slyly after the pending packages! carefully regular packages wake carefully after 
144977	6	5962	895.30	lithely above the furiously ironic deposits. final asymptotes around the unusual dependencies are acros
144978	7	8386	437.28	 promise into the carefully pending instructions. blithely final asymptotes haggle furiously final instructions. 
144979	8	447	28.89	even accounts. fluffily furious pinto beans wake slyly even requests! accounts lose foxes. blithely ironic requests nag alongside of the pending requests. pa
144981	10	8638	398.81	 ideas. bold requests sleep silently accounts. excuses detect quickly slyly special requests: pending accounts cajole blithely. slyly final pinto be
147486	1	736	90.30	gular deposits nag. blithely bold accounts among the regular deposits wake according to 
147489	4	656	78.12	 packages haggle blithely. quickly express theodolites are along the slyly final accounts. carefully pending dependencies nag quickly around the account
147491	6	8006	297.64	ckly. special packages above the even deposits haggle after the requests. quickly silent requests run quickly toward the regular foxes. slyly dogged multipliers cajole silent requests. carefully 
147492	7	4589	545.14	 grow fluffily. bold dependencies use furiously express platelets. quickly regular multipliers cajole quickly. quickly final depths nag fluffily. blithely final accounts are final pinto beans! bold,
147493	8	1269	61.68	thely even accounts. ironic requests engage slyly according to the blithely even instructions. even deposits unwind quickly. final, express in
147495	10	9645	537.95	ounts thrash slyly among the furiously even accounts. furiously unusual deposits thrash carefully c
150000	1	6975	325.03	enly final theodolites haggle slyly bold, special deposits. regular fox
150003	4	2768	707.77	ously regular packages around the final theodolites nag fluffily among the blithely even deposits; regul
150005	6	4873	423.62	g above the slyly even excuses. accounts before the regular packages promise permanently regular multipliers. instructio
150006	7	3680	625.51	y. blithely regular requests sleep carefully pending platelets. ironic packages detect blithely along the quickly bold dep
150007	8	1640	275.88	arefully. slyly even requests wake. regular deposits alongside of the furiously silent forges play furiously blithely bold packages. carefully pending platelets 
150009	10	1347	512.20	t attainments according to the ironic accounts detect blithely blithely pending theodolites. quickly final dolphins grow quickly slyly final requests
152455	1	5024	873.92	ly special, ironic deposits. blithely even deposits snooze special instructions. carefully even theodolites play qui
152458	4	8124	554.02	y. fluffily final ideas are carefully ironic dependencies. pinto beans use furiously bold, bold instructions. slyly close deposi
152460	6	4447	654.01	. carefully regular warthogs haggle blithely slyl
152462	8	8344	686.30	ly after the deposits. asymptotes affix silently. final foxes integra
152464	10	8016	241.12	e. regular accounts boost along the permanently special dolphins. enticing decoys poach. slyly even foxes after the quickly fina
154970	1	5238	907.83	 the ironic braids. furiously regular attainments detect notornis. furiously regular instructions above the carefully pending platelets cajole furiously unu
154973	4	8119	687.52	ges. fluffily regular packages are. final foxes across the 
154975	6	4284	229.52	ld courts sleep quickly among the special dependencies. blithely special theodolites cajole furiously. carefully careful accounts hinder. slyly silent 
154976	7	9200	452.08	eans. regular, ironic deposits nag. furiously quiet
154977	8	8973	489.55	all sleep carefully above the platelets. warthogs must wake regular instructions. sentiments wake blithely furiously permanent ideas. furiously bold dolp
154979	10	7265	173.73	blithely regular accounts was blithely furiously ironic theodolites. pending 
157485	1	6042	690.77	use. bold, final deposits sleep. even requests cajole above the furiously regular instructions. carefully even deposits cajole
157488	4	5355	57.98	ts wake quickly about the theodolites. excuses sleep against the pac
157490	6	1226	228.32	ending packages. silent theodolites integrate special, regular deposits. ruthless excuses haggle carefully 
157491	7	7309	767.39	y regular warhorses haggle slyly quickly ironic instructions. even requests cajole bold instructions-- fluffily final decoys use quickly. pending, final theodolites among the final ideas affix qui
157492	8	5372	190.40	ke after the carefully even deposits. carefully unusual
157494	10	9185	797.59	es nag. unusual warthogs are furiously. finally regular platelets cajole fluffily. blithely even dinos detect blithely alongside of the pen
160000	1	5962	639.61	e idly ironic courts. ideas sleep furiously silent,
160003	4	9520	872.93	ts haggle carefully. regular accounts along the quickly ironic pinto beans dazzle furiously after the slyly special dependencies. furiously final deposits among the 
160005	6	4895	890.95	sual accounts. blithely special deposits boost slyly around the blithely bold theodolites. furiously final ideas run again
160006	7	5213	43.44	egular accounts. slyly ironic dependencies wake. fluffily slow orbits according 
160007	8	1991	468.03	eposits. furiously special excuses sleep quickly about the quickly regular ideas. regular pinto be
160009	10	4893	641.64	ts cajole after the slyly pending theodolites. regular, final asy
162452	1	5489	56.95	 wake furiously slyly regular accounts. slyly final ideas cajole quickly. furiously final warthogs along t
162455	4	7920	620.52	blithely bold deposits lose alongside of the regular, unusual deposits. carefully ironic dept
162457	6	1934	986.77	y against the blithely even requests. deposits breach across the fluffily ruthless accounts. carefully sly ideas wake slyly packages. fluffily 
162458	7	4730	629.38	ed requests cajole blithely across the slyly final accounts. regular courts around the packages use slyly regular foxes. pinto beans boost furiously according to the pending, even excuses. ideas h
162459	8	6183	487.79	into beans. quickly unusual accounts are furiously ironic excuses. slyly express dependencies boost according to the carefully ironic accounts. sl
162461	10	7081	799.79	 accounts. pending courts nod furiously until the slyly unusual packages. bold, express excuses cajole against the slyly ironi
164968	1	3993	599.78	hely among the bold dependencies. furiously pending requests hang blithely! ironic, express foxes across the slyly final r
164971	4	4862	618.42	ideas? fluffily unusual foxes nag slyly after the fluffily final theodolites. even packages integrate fluffily across the even requests. fu
164973	6	1302	886.44	 ironic ideas wake slyly thinly bold requests. blithely silent deposits mold blithely. furiously final dinos run furiously q
164974	7	4402	595.56	 slyly bold requests cajole quickly. pending packages sleep fluffily. regular packages wake quickly courts. furiously bol
164975	8	1534	496.83	s. pending platelets above the carefully pending platelets are blithely quickly close requests. furiously unusual theodolites affix carefully quickly final pac
164977	10	6323	366.39	ckages boost blithely slyly ironic ideas: slyly even requests was bravely along the slyly even deposits-- furiously silent packages haggl
167484	1	9514	50.03	ly regular pinto beans sleep slyly. blithely final pinto beans haggle. unusual requests are across the fluffily final packages
167487	4	4421	864.31	ly regular ideas sleep. pinto beans integrate blithely bold excuses. even pinto beans haggle above the ironic accounts. final a
167489	6	3255	439.73	s haggle across the dolphins. special, special requests haggle dogg
167490	7	3706	533.94	usly bold accounts use slowly among the blithely final requests. slyly express accounts are slyly slyly final pinto beans. e
167491	8	4990	213.20	gular excuses. foxes cajole after the express tithes. furiously even dolphins eat carefully. pinto beans cajole carefully. carefully blithe requests use quickly slyly final packages. blithely re
167493	10	5219	602.19	among the quickly even excuses. packages across the quickly bold deposits was slyly after the thin dolphins. even deposits will nag quickly. final plate
170000	1	1330	597.35	- bravely enticing dependencies across the ironic requests mold furiously about the ironic requests. even accounts haggle carefully! slyly regular pinto beans grow slyl
170003	4	3609	594.65	ial ideas wake carefully. furiously ruthless theodolites about the ideas sleep never bravely even requests. bold, final requests print furiously furiously final dependencies. iron
170005	6	1389	966.34	ully final dolphins sleep fluffily fluffily special packages. even foxes are bold platelets. quickly even excuses doze carefully even instructions. carefully regular requests shall h
170006	7	7435	256.29	ing requests haggle carefully across the regular pinto beans. carefully express instructions detect blithe
170007	8	6641	952.03	ideas nag slyly pending, unusual foxes. ironic re
170009	10	3315	681.26	 packages haggle quickly carefully final foxes. bold courts wake carefully. special instructions sleep slyly? pending, express packages sleep after th
172449	1	916	108.14	 the furiously even ideas: foxes sleep express packages. bold, pending pinto beans sleep. bold deposits nag blithely express deposits. asymptotes haggle slyly. blithely special p
172452	4	3827	62.08	inder furiously carefully final accounts. warhorses wake slyly express dependencies; regular 
172454	6	8154	124.70	 deposits wake quickly across the quickly pending platelets. quickly final packages use around the bold packages. quick, ironic 
172455	7	7665	719.91	s-- furiously pending pinto beans after the slyly 
172456	8	4007	432.82	ly even asymptotes according to the final ideas thrash slyly across the slowly even deposits. ironic deposits are ruthlessly final at
172458	10	3543	776.71	n requests; ironic, pending foxes nag furiously unusual accounts. final, express instructions wake furiously. care
174966	1	6950	34.20	 regular, even packages. special multipliers along the slyly regular packages 
174969	4	8397	239.30	cies haggle furiously among the slyly unusual excuses. fi
174971	6	4889	693.41	lyly final accounts are slyly ironic asymptotes. ironic dolphins mold blithely. slyly unusual asymptotes doze slyly against the carefully final packages. unusual, ironic accounts bre
174972	7	5072	173.24	attainments according to the packages are blithely regular courts. foxes haggle above the unusual, final attainments. furiously even deposits haggle bold accounts. acco
174973	8	3483	687.54	 sleep blithely on the carefully even asymptotes. careful ideas integrate blithely. ironic, regular accounts use slyly. deposits cajole ruthlessly quickly final instructions. regular, silent reques
174975	10	5454	791.56	y regular accounts? regular, pending courts cajole instructions. blithely regular
177483	1	434	496.82	al pinto beans. slyly final packages by the permanent, even pinto beans haggle quickly across the slyly brave instructions. carefully regular deposits haggle. furiously pending pinto b
177486	4	387	807.72	the theodolites. even escapades thrash furiously special pearls. furiously unusual requests haggle slyly according to the even requests. carefully 
177488	6	4606	964.41	ages use. express packages about the slyly even courts haggle daringly above the qu
177489	7	2939	2.35	l theodolites sleep among the quickly special packages. fluffily final accounts boost furiously. quickly final accounts sleep blithely fluffily bold deposits. fluffily pending depos
177490	8	8289	220.86	uses across the quickly furious asymptotes nag bold packages. bold foxes after the f
177492	10	3604	689.76	inal foxes cajole blithely. quickly special accounts haggle furiously about the carefully even deposits. final requests after the slyly final foxes are ironic instruct
180000	1	907	25.88	hely regular forges cajole. blithely unusual platelets affix furiously alongside of the requests. special platelets detect busy, bold accoun
180003	4	6796	551.49	ackages wake. fluffily regular epitaphs are furiously about the slyly silent accounts. epitaphs wake fluffily. carefully bold waters h
180005	6	6514	638.63	nding asymptotes. unusual, unusual instructions run quietly across the accounts. pending foxes integrate. fluffily bold ideas according to the express pa
180006	7	5833	939.09	. slyly bold notornis sleep according to the furiously ironic platelets. final, pending sentiments might haggle slyly! carefully final deposits nag above the slyly final pinto beans
180007	8	8073	562.97	 furiously about the quickly special ideas. accounts sleep fluffil
180009	10	8997	796.76	c, special deposits. furiously regular foxes wake regular deposits. carefully regular deco
182446	1	5959	16.83	ss the quickly ironic pinto beans. furiously bold packages will have to play quickly regular de
182449	4	5822	322.61	s integrate; slyly quiet accounts was carefully aft
182451	6	1650	963.44	fully alongside of the courts. regular, ironic pinto beans boost against the slyly e
182452	7	3732	510.10	gedly according to the quickly ironic deposits. furiously pending asymptotes cajole even ideas. bold packages was after the furiously express packages. even packages cajol
182453	8	2375	474.10	ackages sleep furiously about the enticingly even foxes. foxes according to the pending instruct
182455	10	5828	256.80	ounts. slyly pending excuses according to the permanently regular packages sleep furiously across the fluffily final asymptotes. carefully special foxes integr
184964	1	6554	412.40	even, brave excuses are quickly even pearls. sauternes wake around the regular, bold asymptotes. final ac
184967	4	6052	401.62	se pinto beans. blithely express foxes are fluffily final requests. slyly special platelets are
184969	6	562	145.81	furiously final platelets kindle blithely quickly final requests. ironically eve
184970	7	7412	290.29	r packages. furiously silent notornis unwind blithely. ironic requests boost slyly quickly final ideas. carefully even platelets cajole. blith
184971	8	5166	820.59	usly pending theodolites. packages against the furio
184973	10	6436	882.37	above the furiously express platelets. asymptotes can boost according to the carefully ironic accounts. regular, final foxes are carefully along the slyly unusual deposits. quickly regular packages
187482	1	7803	177.21	inst the bravely final escapades cajole among the slyly blithe asymptotes. unusual accounts
187485	4	3375	812.71	ly even deposits. carefully pending requests use furiously fluffily special frays. special theodolites wake quickly furiously ironic dependencies.
187487	6	7	447.80	y doggedly regular excuses. slyly bold deposits according to the instructions sleep carefully even instructions. flu
187488	7	442	229.19	foxes. slyly bold instructions cajole. furiously express deposits sleep blithely alo
187489	8	2750	472.16	he quickly ironic courts. slyly unusual requests above the slyly express foxes are quickly blithely bold platelets. slyly express pinto beans wake. furiously final packages against the unus
187491	10	1164	213.32	across the special accounts. quickly even accounts use furiously even deposits. furiously 
190000	1	4988	209.46	theodolites until the regular accounts are carefully at the final courts. furiously final sentiments use ruthless, even asymptotes. carefully express 
190003	4	6024	588.99	posits detect according to the slyly ironic requests. fluffily even requests integrate carefully. special, special packages boost s
190005	6	9896	794.43	nal, final packages. furiously busy deposits affix silently even foxes. carefully ironic requests use slyly after the daring packages. dolphins 
190006	7	8537	214.97	blithely ironic, pending ideas. slyly express sheaves use blithely express platelets. slyly unusual requests are slyly un
190007	8	8128	718.03	rs detect furiously unusual dependencies. blithely final requests sleep carefully quickly final dependencies. carefully enticing asymptotes until the never ironic theodolites engage furiously slyly
190009	10	6551	613.20	ls haggle. ironic excuses are fluffily about the enticingly ironic
192443	1	9554	946.43	 excuses cajole furiously. regular deposits alongside of the furiously pending sheaves cajole quickly pending requests. fluffily u
192446	4	5193	334.94	 boost carefully. packages sleep quickly carefully regular theodoli
192448	6	1899	168.85	, bold theodolites cajole. requests affix. blithely silent pinto beans haggl
192449	7	4010	330.38	althy requests. slyly bold requests maintain quickly. silent depths above
192450	8	5935	336.02	ges according to the ironic requests haggle after the ironic, regular packages. carefully pending instruc
192452	10	1403	799.18	ckages haggle furiously escapades. slowly even theodolites s
194962	1	8641	688.71	y. busily pending platelets cajole at the fluffily ironic excuses. express packages must have to cajole quickly sentiments. furiously express depos
194965	4	9614	384.27	 theodolites. blithely pending deposits nag. pending pinto beans boost blithely after the regular theodolites. fluffily pending deposit
194967	6	2667	312.92	oxes. final accounts along the ironic, special accounts integrate closely among the fluffily pending accounts; boldly fluffy excuses haggle. foxes above the theodolites grow carefully
194968	7	1641	214.92	gular courts sleep regular, pending requests. fluffily unusual platelets haggl
194969	8	6673	255.82	deposits sleep blithely ironic packages; daringly idle packages could have to nag alongside of the furiously brave foxes. fluffily unusual escapades impress regular deposits. fluffily final depos
194971	10	7597	336.80	ounts are against the asymptotes. regular packages boost accordi
197481	1	5205	465.57	ironic theodolites. fluffily close accounts boost slyly during t
197484	4	7751	619.15	oxes. slowly express accounts haggle furiously special, final theodolites-- furiously pending packages above the blithely final theodoli
197486	6	4031	625.45	 are slyly within the express dolphins. slyly silent requests print. slyly ironic packages sleep blithely. bol
197487	7	3341	862.38	express, express requests are carefully carefully even requests. fluffily ironic instructions wake carefully bold waters. furiously pending asymptotes after the regular packages cajole slyly ins
197488	8	6894	859.63	ronic dependencies kindle quickly furiously pending asymptotes; car
197490	10	5572	189.32	s cajole carefully across the quickly special platelets. furiously regular 
200000	1	3372	74.18	egular, pending warthogs along the even pinto beans boost foxes. blithely ironic deposits along the carefully ironic requests d
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

