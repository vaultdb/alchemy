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
\.


--
-- Data for Name: lineitem; Type: TABLE DATA; Schema: public; Owner: vaultdb
--

COPY public.lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment) FROM stdin;
43879	24960	2467	1	17	32044.32	0.00	0.03	R	F	1993-09-28	1993-11-04	1993-10-16	COLLECT COD              	SHIP      	arefully silent dolphins. s
43879	86524	6525	2	37	55889.24	0.10	0.07	A	F	1993-09-16	1993-11-06	1993-10-09	TAKE BACK RETURN         	RAIL      	eas? bold, regular fret
52263	193924	6444	1	27	54483.84	0.03	0.06	A	F	1994-05-24	1994-06-14	1994-06-08	TAKE BACK RETURN         	TRUCK     	nding instructions. slyly unusual reque
70819	108196	3217	1	31	37329.89	0.00	0.08	N	O	1997-03-12	1997-02-13	1997-03-13	TAKE BACK RETURN         	AIR       	ets. deposits hag
70819	41682	9195	2	22	35720.96	0.04	0.08	N	O	1996-12-16	1997-02-02	1997-01-05	COLLECT COD              	MAIL      	breach furiously final warthogs. requests
70819	139297	9298	3	36	48106.44	0.10	0.04	N	O	1996-12-16	1997-02-01	1996-12-26	COLLECT COD              	RAIL      	idle asympt
70819	164149	6666	4	38	46099.32	0.02	0.08	N	O	1997-01-23	1997-01-07	1997-01-27	DELIVER IN PERSON        	RAIL      	oost. final packages promise fluffily fina
70819	44827	9836	5	30	53154.60	0.05	0.08	N	O	1997-01-22	1997-02-18	1997-02-19	NONE                     	REG AIR   	e final requests;
90019	142067	9610	1	24	26617.44	0.08	0.03	R	F	1994-02-23	1993-12-26	1994-03-05	COLLECT COD              	AIR       	ound the express, ironi
90019	107326	7327	2	22	29333.04	0.02	0.00	R	F	1994-01-09	1993-12-14	1994-01-26	DELIVER IN PERSON        	FOB       	ven theodolites was. blithely enticing
90019	19551	9552	3	6	8823.30	0.00	0.02	A	F	1993-11-16	1993-12-03	1993-12-11	COLLECT COD              	TRUCK     	ole furiously. instruc
90019	185105	2660	4	17	20231.70	0.00	0.01	R	F	1993-11-26	1993-12-03	1993-12-23	TAKE BACK RETURN         	RAIL      	sts are quic
100064	171312	1313	1	46	63632.26	0.01	0.08	N	O	1996-06-10	1996-07-02	1996-06-27	DELIVER IN PERSON        	REG AIR   	 inside the iron
120160	143200	5715	1	45	55944.00	0.04	0.00	N	O	1995-08-08	1995-06-04	1995-08-14	COLLECT COD              	SHIP      	unusual foxes after t
120160	148871	1386	2	17	32637.79	0.03	0.00	N	O	1995-06-22	1995-05-27	1995-06-30	COLLECT COD              	REG AIR   	accounts sleep sly
120160	73950	3951	3	48	92349.60	0.10	0.07	N	O	1995-07-19	1995-06-21	1995-08-12	NONE                     	FOB       	usly expres
120160	76158	8666	4	18	20414.70	0.06	0.06	A	F	1995-04-13	1995-06-25	1995-04-15	TAKE BACK RETURN         	SHIP      	deposits need to cajole caref
120160	179866	9867	5	21	40863.06	0.07	0.04	N	O	1995-07-27	1995-06-03	1995-08-26	NONE                     	RAIL      	 snooze ironic 
135943	98961	3980	1	37	72518.52	0.03	0.05	A	F	1993-07-07	1993-09-17	1993-07-22	DELIVER IN PERSON        	AIR       	 the final requests. 
135943	36541	4051	2	37	54668.98	0.09	0.08	R	F	1993-08-04	1993-08-28	1993-09-01	NONE                     	FOB       	the regular pac
135943	143743	6258	3	43	76829.82	0.10	0.04	A	F	1993-09-12	1993-07-22	1993-09-29	COLLECT COD              	TRUCK     	ve carefully acros
135943	74859	4860	4	28	51347.80	0.02	0.08	A	F	1993-09-27	1993-09-07	1993-10-27	DELIVER IN PERSON        	SHIP      	y final deposi
135943	153657	1203	5	34	58162.10	0.01	0.05	A	F	1993-10-12	1993-09-06	1993-10-31	TAKE BACK RETURN         	AIR       	ironic excuses are slyl
193030	191397	8955	1	29	43163.31	0.07	0.01	R	F	1992-06-12	1992-07-10	1992-06-15	NONE                     	TRUCK     	 packages are against the fluffi
193030	194544	7064	2	24	39324.96	0.04	0.08	A	F	1992-06-29	1992-09-07	1992-07-11	COLLECT COD              	RAIL      	, quiet ideas wake furiously 
193030	91814	4324	3	31	55980.11	0.04	0.05	R	F	1992-09-24	1992-08-02	1992-10-03	NONE                     	TRUCK     	l dependencies. ironic dolphins impress pl
193030	107973	484	4	49	97067.53	0.07	0.05	R	F	1992-07-14	1992-09-03	1992-07-30	TAKE BACK RETURN         	RAIL      	boost idly along the slyly regular 
193030	99981	9982	5	3	5942.94	0.08	0.08	A	F	1992-08-26	1992-07-17	1992-08-30	COLLECT COD              	AIR       	lets. express, unusual hockey player
193030	169702	7251	6	21	37205.70	0.01	0.05	A	F	1992-07-10	1992-08-03	1992-07-30	NONE                     	REG AIR   	slyly theodolites. carefully regula
193030	171266	3784	7	32	42792.32	0.01	0.07	A	F	1992-08-16	1992-08-09	1992-09-03	DELIVER IN PERSON        	SHIP      	ng requests detect. c
212870	48274	5787	1	39	47668.53	0.04	0.05	N	O	1997-02-27	1996-12-14	1997-03-08	COLLECT COD              	AIR       	to beans. slyly regular d
212870	124666	4667	2	48	81151.68	0.03	0.00	N	O	1997-01-09	1997-01-11	1997-02-03	COLLECT COD              	RAIL      	ular asymptotes
212870	54740	2256	3	26	44063.24	0.00	0.02	N	O	1996-12-14	1997-01-23	1997-01-13	TAKE BACK RETURN         	REG AIR   	 ironic pinto beans
226818	147910	5453	1	42	82232.22	0.09	0.01	N	O	1995-09-04	1995-06-18	1995-10-02	COLLECT COD              	AIR       	s. final acco
226818	109777	2288	2	2	3573.54	0.09	0.05	N	O	1995-07-12	1995-06-28	1995-07-28	COLLECT COD              	MAIL      	r deposits are furiously slyly final requ
226818	21107	8614	3	38	39067.80	0.05	0.03	N	O	1995-07-25	1995-08-10	1995-08-12	TAKE BACK RETURN         	TRUCK     	refully bold ideas according to
235779	137931	5471	1	30	59067.90	0.09	0.02	A	F	1994-06-25	1994-07-08	1994-07-24	NONE                     	FOB       	unusual reques
235779	195253	5254	2	18	24268.50	0.09	0.07	A	F	1994-05-09	1994-06-25	1994-05-25	TAKE BACK RETURN         	MAIL      	 even deposits was slyly ironic
235779	12960	7963	3	11	20602.56	0.05	0.06	R	F	1994-05-07	1994-06-06	1994-05-25	TAKE BACK RETURN         	FOB       	 carefully about the quickly even requests
235779	8871	6372	4	30	53396.10	0.10	0.03	A	F	1994-06-15	1994-07-03	1994-07-07	NONE                     	MAIL      	ter the silent th
235779	91322	1323	5	13	17073.16	0.05	0.05	R	F	1994-06-29	1994-07-12	1994-07-12	TAKE BACK RETURN         	SHIP      	 are slyly across the carefully special
235779	43138	5643	6	17	18379.21	0.10	0.03	A	F	1994-07-05	1994-06-13	1994-07-27	NONE                     	RAIL      	ilently orbits. fu
235779	12487	4989	7	29	40584.92	0.07	0.00	R	F	1994-06-12	1994-06-19	1994-06-28	DELIVER IN PERSON        	MAIL      	uctions integrate. re
306439	58578	6094	1	7	10755.99	0.05	0.02	N	O	1997-08-26	1997-06-18	1997-08-29	COLLECT COD              	SHIP      	ounts kindle slyly final packages. quic
306439	105942	8453	2	49	95449.06	0.10	0.03	N	O	1997-07-31	1997-07-15	1997-08-19	NONE                     	TRUCK     	final foxes boost. express reque
306439	160364	365	3	18	25638.48	0.03	0.02	N	O	1997-05-23	1997-06-28	1997-06-05	TAKE BACK RETURN         	TRUCK     	old, even accounts cajol
306439	32522	2523	4	39	56726.28	0.10	0.05	N	O	1997-06-29	1997-06-27	1997-07-25	DELIVER IN PERSON        	REG AIR   	eans are. blithely silent fox
306439	193976	3977	5	18	37259.46	0.07	0.08	N	O	1997-07-30	1997-08-14	1997-08-20	COLLECT COD              	AIR       	wake across 
306439	137889	403	6	36	69367.68	0.10	0.06	N	O	1997-07-31	1997-07-24	1997-08-08	TAKE BACK RETURN         	SHIP      	e the acco
360067	82769	5278	1	40	70070.40	0.07	0.07	A	F	1992-12-27	1993-02-23	1992-12-30	TAKE BACK RETURN         	TRUCK     	 along the ironic accounts bo
360067	86523	4048	2	1	1509.52	0.06	0.08	A	F	1993-01-02	1993-01-26	1993-01-18	COLLECT COD              	REG AIR   	er final dugouts sublate b
360067	187572	5127	3	8	13276.56	0.01	0.01	A	F	1993-01-05	1993-01-30	1993-01-07	NONE                     	AIR       	. carefully silent pinto beans boost c
360067	120549	550	4	4	6278.16	0.03	0.05	A	F	1993-01-11	1993-03-07	1993-01-14	NONE                     	TRUCK     	xpress pinto beans are sl
360067	58990	8991	5	45	87704.55	0.05	0.04	A	F	1993-02-08	1993-01-17	1993-02-21	NONE                     	TRUCK     	ructions. platelets haggle according 
360067	44289	6794	6	37	45631.36	0.01	0.01	A	F	1993-04-05	1993-01-30	1993-04-11	DELIVER IN PERSON        	SHIP      	he carefully ironic d
430243	155290	5291	1	23	30941.67	0.01	0.08	A	F	1995-03-28	1995-03-10	1995-04-06	COLLECT COD              	SHIP      	ter the fluffily even instructions. furi
454791	98296	5824	1	40	51771.60	0.05	0.05	R	F	1992-06-24	1992-06-20	1992-07-11	TAKE BACK RETURN         	MAIL      	 busy theodol
454791	52971	487	2	13	25011.61	0.10	0.02	A	F	1992-05-02	1992-05-19	1992-05-10	NONE                     	REG AIR   	ily silent id
529350	170852	8404	1	13	24997.05	0.04	0.08	N	O	1996-03-08	1996-04-08	1996-03-15	TAKE BACK RETURN         	FOB       	nts. slyly ironic packages sleep carefu
529350	106185	8696	2	14	16676.52	0.10	0.04	N	O	1996-06-24	1996-05-11	1996-07-07	TAKE BACK RETURN         	TRUCK     	 alongside of
554115	18077	579	1	39	38807.73	0.00	0.05	R	F	1992-11-28	1993-01-06	1992-12-23	NONE                     	SHIP      	ns. furiously
554115	11649	4151	2	25	39016.00	0.07	0.02	A	F	1993-01-29	1992-12-30	1993-02-17	COLLECT COD              	FOB       	ar ideas sleep alongside of the slyl
554115	172561	5079	3	44	71876.64	0.05	0.03	R	F	1992-11-10	1992-11-20	1992-11-28	TAKE BACK RETURN         	MAIL      	ing, bold instructions use quickl
554115	23367	8372	4	26	33549.36	0.06	0.00	A	F	1993-01-29	1993-01-08	1993-02-27	COLLECT COD              	REG AIR   	 requests: packages a
554115	122184	9721	5	46	55484.28	0.10	0.00	A	F	1992-10-27	1992-11-27	1992-11-21	DELIVER IN PERSON        	FOB       	furiously among the blithely ironic ideas
576263	159847	7393	1	24	45764.16	0.03	0.02	A	F	1994-06-05	1994-05-09	1994-07-03	NONE                     	FOB       	atelets. courts doze furiously. reg
576263	135388	415	2	36	51241.68	0.01	0.02	R	F	1994-03-18	1994-05-20	1994-03-25	TAKE BACK RETURN         	MAIL      	otes. fluffily final deposits
576263	58908	6424	3	25	46672.50	0.04	0.05	A	F	1994-03-26	1994-05-08	1994-04-01	NONE                     	MAIL      	g the blithely ironic p
576263	65305	2824	4	13	16513.90	0.00	0.05	R	F	1994-04-19	1994-05-30	1994-05-19	NONE                     	TRUCK     	eas. ironic 
576263	170796	797	5	6	11200.74	0.10	0.01	R	F	1994-03-10	1994-05-23	1994-03-14	TAKE BACK RETURN         	RAIL      	y furiously regular ideas. regular foxes 
576263	152516	2517	6	32	50192.32	0.05	0.08	A	F	1994-06-25	1994-05-10	1994-07-10	TAKE BACK RETURN         	AIR       	deposits nag blithely special exc
576263	136196	6197	7	49	60377.31	0.09	0.00	A	F	1994-06-24	1994-04-07	1994-07-04	COLLECT COD              	MAIL      	its. brave packages nod across th
579908	104617	7128	1	33	53513.13	0.00	0.01	N	O	1997-03-30	1997-01-11	1997-04-25	NONE                     	MAIL      	uickly special ideas alongside
601794	139227	6767	1	14	17727.08	0.01	0.01	N	O	1997-03-25	1997-02-26	1997-04-08	TAKE BACK RETURN         	AIR       	iers sleep above the epitaphs
601794	176881	4433	2	47	92020.36	0.08	0.02	N	O	1997-03-03	1997-02-22	1997-03-28	TAKE BACK RETURN         	TRUCK     	al warhorses use 
601794	30828	829	3	18	31658.76	0.04	0.02	N	O	1996-12-11	1997-02-14	1997-01-07	NONE                     	FOB       	riously. caref
601794	123673	1210	4	22	37326.74	0.07	0.02	N	O	1997-04-02	1997-01-18	1997-04-30	NONE                     	RAIL      	 ironic ideas-- car
601794	159219	6765	5	49	62632.29	0.05	0.02	N	O	1997-01-05	1997-01-28	1997-01-31	TAKE BACK RETURN         	FOB       	iously. final dependencies sleep care
601794	190611	3131	6	41	69766.01	0.09	0.07	N	O	1997-03-13	1997-01-10	1997-03-21	COLLECT COD              	RAIL      	quests. furiously ironic dep
601794	188047	8048	7	45	51076.80	0.04	0.04	N	O	1997-02-06	1997-01-23	1997-02-10	TAKE BACK RETURN         	SHIP      	t accounts boost. ironic idea
639490	181754	4273	1	8	14686.00	0.09	0.02	N	O	1996-01-04	1995-12-01	1996-01-30	COLLECT COD              	REG AIR   	ut the slyl
639490	129635	7172	2	34	56597.42	0.00	0.04	N	O	1996-02-04	1995-11-28	1996-02-09	DELIVER IN PERSON        	SHIP      	e of the ideas. regular packages are
639490	161563	9112	3	36	58484.16	0.03	0.02	N	O	1996-02-11	1995-12-01	1996-03-02	TAKE BACK RETURN         	AIR       	lithely according to the regular request
639490	185170	2725	4	47	58992.99	0.01	0.04	N	O	1996-01-15	1995-12-25	1996-02-07	TAKE BACK RETURN         	TRUCK     	ing accounts
639490	136620	6621	5	31	51355.22	0.07	0.05	N	O	1996-01-01	1995-12-22	1996-01-30	COLLECT COD              	FOB       	foxes. carefully final requests across the
641253	120927	928	1	28	54541.76	0.00	0.03	N	O	1995-09-14	1995-07-29	1995-09-15	DELIVER IN PERSON        	REG AIR   	al ideas. slyly bol
641253	9169	6670	2	39	42048.24	0.04	0.08	N	O	1995-09-13	1995-07-01	1995-09-17	NONE                     	TRUCK     	are furiously even theodolites. regu
641253	90524	8052	3	18	27261.36	0.05	0.08	N	O	1995-08-22	1995-06-28	1995-09-19	DELIVER IN PERSON        	RAIL      	ic ideas? fin
641253	20536	5541	4	11	16021.83	0.06	0.01	N	O	1995-08-11	1995-07-25	1995-08-21	TAKE BACK RETURN         	FOB       	ly among the carefully regular platelets; b
653063	18151	8152	1	36	38489.40	0.03	0.00	R	F	1995-04-30	1995-05-30	1995-05-13	DELIVER IN PERSON        	SHIP      	ld alongside of the f
653063	49413	4422	2	15	20436.15	0.09	0.02	A	F	1995-04-07	1995-04-14	1995-05-05	DELIVER IN PERSON        	AIR       	lly after the
655010	68886	6405	1	11	20403.68	0.03	0.08	A	F	1995-03-23	1995-04-27	1995-04-16	TAKE BACK RETURN         	FOB       	riously final
655010	154771	7287	2	39	71205.03	0.09	0.01	N	O	1995-07-04	1995-05-04	1995-07-14	NONE                     	REG AIR   	en platelets wake according to the regula
655010	21253	3756	3	36	42273.00	0.06	0.03	A	F	1995-03-13	1995-05-31	1995-03-29	COLLECT COD              	RAIL      	lve across the furious
729504	58472	978	1	31	44344.57	0.03	0.08	A	F	1993-12-02	1993-11-15	1993-12-13	DELIVER IN PERSON        	FOB       	usual ideas are. unusual, 
729504	123934	1471	2	8	15663.44	0.09	0.07	A	F	1993-12-08	1993-12-05	1994-01-05	NONE                     	MAIL      	totes sleep fluffily idea
729504	153076	3077	3	47	53066.29	0.03	0.04	A	F	1993-12-16	1994-01-01	1993-12-22	TAKE BACK RETURN         	FOB       	ly even requests. carefully bold accou
787105	8008	3009	1	9	8244.00	0.05	0.06	A	F	1994-06-19	1994-08-25	1994-06-24	TAKE BACK RETURN         	RAIL      	ously accounts. regular excus
787105	175350	385	2	4	5701.40	0.07	0.07	A	F	1994-06-23	1994-08-18	1994-07-03	COLLECT COD              	AIR       	ions. quickly daring theodolite
787105	93811	6321	3	5	9024.05	0.03	0.05	A	F	1994-08-12	1994-08-22	1994-08-14	TAKE BACK RETURN         	FOB       	telets. fluffily even d
787105	81056	1057	4	48	49778.40	0.03	0.02	R	F	1994-09-02	1994-07-29	1994-09-17	TAKE BACK RETURN         	REG AIR   	ites would sleep furiously express dep
787105	106060	3591	5	50	53303.00	0.08	0.03	A	F	1994-07-14	1994-07-19	1994-07-23	TAKE BACK RETURN         	MAIL      	onic deposits 
792900	181114	3633	1	15	17926.65	0.04	0.06	N	O	1995-09-19	1995-09-08	1995-10-19	NONE                     	SHIP      	ke. regular 
792900	172323	4841	2	23	32092.36	0.05	0.06	N	O	1995-08-31	1995-09-27	1995-09-07	NONE                     	REG AIR   	 regular courts a
792900	24219	4220	3	32	36582.72	0.09	0.05	N	O	1995-10-30	1995-09-29	1995-11-10	DELIVER IN PERSON        	FOB       	s. blithely final requ
792900	43355	3356	4	18	23370.30	0.10	0.02	N	O	1995-08-03	1995-10-07	1995-08-10	COLLECT COD              	FOB       	oss the unusual, regular accounts. 
823525	28713	8714	1	30	49251.30	0.03	0.05	N	O	1997-09-01	1997-10-07	1997-09-12	NONE                     	AIR       	ounts wake. furiously special requests br
823525	173721	6239	2	33	59225.76	0.05	0.01	N	O	1997-08-26	1997-10-01	1997-09-18	COLLECT COD              	REG AIR   	hely express packages hang 
823525	118003	5537	3	7	7147.00	0.03	0.00	N	O	1997-10-19	1997-11-20	1997-10-24	DELIVER IN PERSON        	REG AIR   	ackages boost fluffily
835173	130029	7569	1	7	7413.14	0.01	0.06	A	F	1993-12-11	1993-11-06	1993-12-31	DELIVER IN PERSON        	REG AIR   	 special platelets along the flu
835173	182778	5297	2	19	35354.63	0.04	0.03	A	F	1993-10-17	1993-10-17	1993-10-27	TAKE BACK RETURN         	RAIL      	n packages sle
835173	142853	7882	3	46	87209.10	0.04	0.01	A	F	1993-08-28	1993-10-20	1993-09-21	NONE                     	SHIP      	 ideas detect
835173	115429	5430	4	13	18777.46	0.08	0.04	R	F	1993-09-25	1993-10-29	1993-10-21	TAKE BACK RETURN         	TRUCK     	r packages against the car
835173	10178	179	5	17	18498.89	0.04	0.08	A	F	1993-10-03	1993-10-01	1993-10-05	NONE                     	REG AIR   	s. idly pending platelets lose. pinto beans
835173	57781	7782	6	4	6955.12	0.04	0.08	A	F	1993-10-17	1993-09-17	1993-10-27	TAKE BACK RETURN         	TRUCK     	g packages. iro
835173	110026	5049	7	26	26936.52	0.06	0.08	A	F	1993-09-03	1993-10-31	1993-09-07	DELIVER IN PERSON        	TRUCK     	oss the carefully iro
849831	97240	2259	1	27	33405.48	0.07	0.07	R	F	1994-03-10	1994-02-05	1994-04-03	DELIVER IN PERSON        	TRUCK     	tegrate carefully according to the ir
849831	111885	4397	2	45	85359.60	0.08	0.04	R	F	1993-12-14	1994-02-09	1993-12-31	TAKE BACK RETURN         	AIR       	ound the careful
849831	44012	4013	3	44	42064.44	0.05	0.03	A	F	1993-11-28	1994-01-11	1993-11-30	TAKE BACK RETURN         	FOB       	. blithe accounts
849831	103118	649	4	11	12332.21	0.06	0.01	R	F	1994-01-31	1994-01-11	1994-02-27	DELIVER IN PERSON        	SHIP      	er the quick, ironic packages. slyly unusua
849831	89345	4362	5	34	45367.56	0.03	0.03	A	F	1994-02-13	1993-12-31	1994-02-27	NONE                     	MAIL      	ounts haggle above the deposits
849831	5786	787	6	5	8458.90	0.00	0.02	R	F	1994-03-08	1994-01-24	1994-03-25	TAKE BACK RETURN         	REG AIR   	regular frets are carefully. caref
943270	96280	1299	1	1	1276.28	0.07	0.05	A	F	1993-10-15	1993-10-31	1993-10-17	DELIVER IN PERSON        	AIR       	t slyly against the
943270	166489	6490	2	17	26443.16	0.04	0.01	A	F	1993-09-05	1993-10-19	1993-09-19	DELIVER IN PERSON        	RAIL      	slyly bold reques
943270	174423	4424	3	17	25456.14	0.08	0.05	A	F	1993-11-02	1993-10-12	1993-12-02	NONE                     	AIR       	. silent foxes beside the expr
943270	112664	2665	4	26	43593.16	0.06	0.08	A	F	1993-11-27	1993-09-21	1993-12-23	TAKE BACK RETURN         	RAIL      	ully furiously final re
943270	133533	6047	5	13	20364.89	0.08	0.01	A	F	1993-10-28	1993-10-23	1993-11-13	TAKE BACK RETURN         	TRUCK     	ng the bli
943270	85794	811	6	43	76530.97	0.08	0.03	A	F	1993-09-23	1993-09-17	1993-10-23	COLLECT COD              	MAIL      	l deposits cajole carefully slyly s
943270	132420	2421	7	24	34858.08	0.00	0.01	R	F	1993-09-15	1993-10-23	1993-09-27	DELIVER IN PERSON        	AIR       	ackages use slyly deposits. unusu
1008069	58121	5637	1	30	32373.60	0.03	0.03	N	O	1995-10-24	1995-11-30	1995-11-12	NONE                     	TRUCK     	ies. slyly ironic requests unw
1008069	33641	1151	2	33	51963.12	0.07	0.03	N	O	1995-10-06	1995-11-27	1995-10-26	DELIVER IN PERSON        	MAIL      	ly regular dugouts abo
1008069	198811	3850	3	48	91670.88	0.02	0.00	N	O	1996-01-20	1995-12-27	1996-02-04	NONE                     	SHIP      	lyly bold pac
1008069	5579	580	4	47	69774.79	0.00	0.01	N	O	1995-10-31	1995-12-20	1995-11-23	NONE                     	MAIL      	otes haggle slyly 
1008069	156819	6820	5	17	31888.77	0.10	0.07	N	O	1995-11-12	1995-11-19	1995-11-28	NONE                     	FOB       	ep. quickly unusual requests affix quickly
1008069	49760	7273	6	31	53002.56	0.00	0.03	N	O	1995-12-11	1995-10-30	1995-12-29	TAKE BACK RETURN         	RAIL      	ong the slyly ironic 
1071617	36850	1857	1	27	48244.95	0.00	0.03	N	O	1995-07-09	1995-05-15	1995-07-16	DELIVER IN PERSON        	MAIL      	ic packages are. spe
1071617	137384	2411	2	11	15635.18	0.10	0.01	R	F	1995-04-08	1995-04-16	1995-04-13	COLLECT COD              	MAIL      	ke furiously furiously regular 
1071617	3374	8375	3	36	45985.32	0.02	0.07	N	O	1995-07-03	1995-04-26	1995-07-17	DELIVER IN PERSON        	FOB       	express requests. even patterns 
1071617	90308	309	4	30	38949.00	0.03	0.06	N	F	1995-06-12	1995-05-28	1995-06-26	DELIVER IN PERSON        	REG AIR   	l ideas. blithely ironic instructions are 
1071617	194723	2281	5	39	70891.08	0.07	0.05	A	F	1995-06-08	1995-06-03	1995-06-15	TAKE BACK RETURN         	AIR       	blithely express hockey
1072832	171223	3741	1	4	5176.88	0.10	0.05	A	F	1994-03-13	1994-02-17	1994-03-27	DELIVER IN PERSON        	REG AIR   	 carefully carefully ironic reques
1072832	136106	3646	2	50	57105.00	0.01	0.00	R	F	1994-03-23	1994-03-12	1994-03-30	DELIVER IN PERSON        	SHIP      	ess packages hag
1072832	125799	5800	3	1	1824.79	0.03	0.00	R	F	1994-02-09	1994-03-29	1994-02-13	NONE                     	TRUCK     	carefully ironic a
1072832	89201	9202	4	24	28564.80	0.00	0.08	A	F	1994-02-03	1994-03-19	1994-02-24	NONE                     	TRUCK     	beans about the qui
1072832	195568	8088	5	31	51570.36	0.07	0.03	R	F	1994-01-04	1994-02-14	1994-01-26	TAKE BACK RETURN         	TRUCK     	grate slyly. bold pinto beans
1072832	100439	2950	6	6	8636.58	0.09	0.02	R	F	1994-01-06	1994-03-01	1994-01-09	DELIVER IN PERSON        	MAIL      	 unusual deposits are furiously aft
1072832	75465	480	7	26	37451.96	0.07	0.04	R	F	1994-04-17	1994-03-14	1994-05-09	NONE                     	RAIL      	 even ideas wake after the evenl
1073670	79911	9912	1	41	77527.31	0.09	0.08	R	F	1994-09-06	1994-07-16	1994-09-17	COLLECT COD              	AIR       	theodolites. regular deposits brea
1074979	86383	3908	1	14	19171.32	0.07	0.00	A	F	1992-05-11	1992-07-13	1992-05-16	DELIVER IN PERSON        	AIR       	 believe slyly ironic depend
1074979	28149	8150	2	8	8617.12	0.08	0.05	R	F	1992-08-03	1992-06-27	1992-08-23	TAKE BACK RETURN         	FOB       	the ideas. carefully final instructions caj
1074979	92020	9548	3	49	49588.98	0.05	0.01	R	F	1992-08-13	1992-07-04	1992-09-11	TAKE BACK RETURN         	TRUCK     	each carefully. 
1074979	166903	1936	4	1	1969.90	0.04	0.05	R	F	1992-05-28	1992-06-14	1992-06-05	DELIVER IN PERSON        	SHIP      	ests. even, even depend
1115366	39253	4260	1	26	30998.50	0.07	0.08	N	O	1996-11-10	1996-10-11	1996-11-15	DELIVER IN PERSON        	TRUCK     	le blithely carefully regula
1115366	123672	8697	2	1	1695.67	0.05	0.06	N	O	1996-11-03	1996-10-12	1996-11-16	NONE                     	MAIL      	sly regular accounts. regular, reg
1115366	146924	1953	3	19	37447.48	0.05	0.03	N	O	1996-08-16	1996-10-20	1996-09-05	TAKE BACK RETURN         	SHIP      	es. slyly final foxes wake carefully q
1115366	144746	9775	4	34	60885.16	0.07	0.04	N	O	1996-09-14	1996-09-22	1996-09-25	COLLECT COD              	RAIL      	egular instructions. bl
1115366	132086	4600	5	33	36896.64	0.06	0.05	N	O	1996-10-21	1996-08-29	1996-11-08	COLLECT COD              	TRUCK     	ld platelets sleep
1115366	153384	930	6	2	2874.76	0.00	0.06	N	O	1996-11-03	1996-09-03	1996-11-28	NONE                     	MAIL      	 packages. blithel
1115366	108180	3201	7	41	48715.38	0.02	0.00	N	O	1996-08-26	1996-09-15	1996-09-24	COLLECT COD              	FOB       	ironic requests
1176518	58119	8120	1	8	8616.88	0.10	0.08	A	F	1994-11-10	1994-11-05	1994-11-15	DELIVER IN PERSON        	REG AIR   	sly pending ideas use carefully silent, pe
1176518	1145	8646	2	46	48122.44	0.02	0.02	R	F	1994-11-25	1994-10-16	1994-12-25	COLLECT COD              	MAIL      	arefully slyly
1176518	27607	5114	3	26	39899.60	0.09	0.07	R	F	1994-10-31	1994-10-08	1994-11-03	COLLECT COD              	REG AIR   	lar, bold requests across the
1176518	181160	6197	4	10	12411.60	0.00	0.03	A	F	1994-09-14	1994-10-09	1994-10-07	TAKE BACK RETURN         	FOB       	the quickly unusual warhors
1176518	149741	7284	5	40	71629.60	0.03	0.08	R	F	1994-10-29	1994-10-10	1994-11-12	TAKE BACK RETURN         	SHIP      	ic, regular theodolites. bold ideas wa
1186787	132661	5175	1	4	6774.64	0.03	0.00	A	F	1995-03-21	1995-04-28	1995-04-12	DELIVER IN PERSON        	REG AIR   	furiously unusual ideas. fluffily sp
1186787	22556	5059	2	37	54706.35	0.06	0.07	R	F	1995-04-14	1995-04-09	1995-05-06	COLLECT COD              	AIR       	play carefully regular i
1186787	89149	9150	3	21	23900.94	0.08	0.05	R	F	1995-03-14	1995-05-20	1995-04-10	DELIVER IN PERSON        	MAIL      	ajole around the thin ideas. furious
1186787	126930	1955	4	13	25440.09	0.05	0.08	R	F	1995-03-14	1995-04-21	1995-04-11	NONE                     	TRUCK     	sits. fluffi
1201223	59527	7043	1	44	65406.88	0.04	0.01	N	O	1996-02-20	1996-04-02	1996-03-19	NONE                     	FOB       	g, unusual platel
1201223	66105	8612	2	2	2142.20	0.04	0.01	N	O	1996-01-25	1996-04-04	1996-02-10	NONE                     	AIR       	kly special deposits wake regularly a
1201223	19891	2393	3	41	74246.49	0.02	0.01	N	O	1996-02-07	1996-03-31	1996-02-25	DELIVER IN PERSON        	MAIL      	ayers. ideas boost at the packages. depos
1243748	16827	1830	1	5	8719.10	0.04	0.07	N	O	1996-07-17	1996-05-07	1996-07-28	DELIVER IN PERSON        	RAIL      	ress packages after the final, final req
1252005	73233	5741	1	29	34980.67	0.10	0.05	A	F	1992-03-05	1992-04-06	1992-03-22	TAKE BACK RETURN         	AIR       	arefully final excuses nag accounts. sly
1252005	26101	6102	2	3	3081.30	0.05	0.07	A	F	1992-04-02	1992-03-10	1992-04-21	COLLECT COD              	REG AIR   	e carefully about the car
1252005	67445	7446	3	39	55085.16	0.08	0.06	A	F	1992-03-13	1992-03-20	1992-04-03	NONE                     	SHIP      	ly blithely pending 
1252005	138857	6397	4	30	56875.50	0.06	0.04	A	F	1992-03-26	1992-03-14	1992-04-13	TAKE BACK RETURN         	TRUCK     	 along the regular, final deposits. re
1252005	163296	3297	5	37	50293.73	0.04	0.01	R	F	1992-04-14	1992-03-17	1992-04-17	COLLECT COD              	AIR       	 quickly bold dugouts a
1252005	113611	3612	6	35	56861.35	0.03	0.05	R	F	1992-03-30	1992-03-21	1992-04-10	COLLECT COD              	RAIL      	al, even foxes haggle? quickly even s
1252005	43586	1099	7	23	35180.34	0.06	0.06	R	F	1992-05-11	1992-04-02	1992-05-12	DELIVER IN PERSON        	TRUCK     	pending dependencies. furiousl
1358113	66879	9386	1	44	81218.28	0.03	0.00	A	F	1992-05-01	1992-03-28	1992-05-17	DELIVER IN PERSON        	SHIP      	ns use carefully special de
1358113	32351	2352	2	20	25667.00	0.00	0.08	A	F	1992-05-18	1992-04-30	1992-05-30	COLLECT COD              	MAIL      	heodolites. carefully ironic
1374019	45052	7557	1	39	38884.95	0.07	0.00	A	F	1992-05-01	1992-05-20	1992-05-25	TAKE BACK RETURN         	TRUCK     	special, special deposits among the f
1374019	178552	1070	2	1	1630.55	0.10	0.00	A	F	1992-06-21	1992-05-26	1992-07-18	DELIVER IN PERSON        	AIR       	en theodolites. furiously unus
1374019	111572	6595	3	13	20586.41	0.07	0.01	R	F	1992-05-10	1992-06-25	1992-05-30	TAKE BACK RETURN         	SHIP      	furiously ac
1374019	182631	7668	4	42	71972.46	0.09	0.01	A	F	1992-07-06	1992-06-28	1992-08-05	DELIVER IN PERSON        	TRUCK     	deposits sleep across the ironic deposits
1374019	62432	9951	5	33	46016.19	0.10	0.06	A	F	1992-05-20	1992-06-03	1992-05-25	DELIVER IN PERSON        	TRUCK     	e slyly even instructions. furiously i
1406820	7895	396	1	29	52283.81	0.08	0.05	N	O	1996-06-12	1996-04-14	1996-06-21	DELIVER IN PERSON        	TRUCK     	usly final theodolites are ar
1406820	76566	6567	2	48	74042.88	0.03	0.02	N	O	1996-05-01	1996-05-05	1996-05-08	TAKE BACK RETURN         	SHIP      	 final excuses 
1406820	37494	7495	3	11	15746.39	0.09	0.05	N	O	1996-02-26	1996-03-26	1996-03-23	DELIVER IN PERSON        	TRUCK     	e slyly slyly silent 
1406820	153659	1205	4	19	32540.35	0.10	0.01	N	O	1996-03-01	1996-04-27	1996-03-14	TAKE BACK RETURN         	RAIL      	quickly according to the qui
1406820	146413	1442	5	15	21891.15	0.02	0.05	N	O	1996-05-13	1996-05-15	1996-06-05	NONE                     	SHIP      	thely permanent r
1406820	162620	7653	6	6	10095.72	0.03	0.06	N	O	1996-04-21	1996-04-30	1996-05-19	COLLECT COD              	RAIL      	yly special asymptotes
1428645	169095	1612	1	19	22117.71	0.05	0.08	A	F	1992-10-12	1992-10-22	1992-10-15	COLLECT COD              	TRUCK     	nding accounts. unusual 
1428645	164223	9256	2	42	54063.24	0.03	0.05	R	F	1992-11-15	1992-09-30	1992-12-11	COLLECT COD              	TRUCK     	n foxes. final instr
1428645	93831	6341	3	30	54744.90	0.03	0.04	A	F	1992-09-14	1992-11-18	1992-09-27	NONE                     	MAIL      	er final acc
1485505	90924	5943	1	46	88086.32	0.09	0.00	N	O	1998-10-28	1998-09-12	1998-11-07	COLLECT COD              	REG AIR   	s instruct
1485505	150483	484	2	34	52138.32	0.06	0.03	N	O	1998-10-07	1998-09-24	1998-11-02	TAKE BACK RETURN         	AIR       	ose ideas 
1485505	151566	1567	3	6	9705.36	0.10	0.03	N	O	1998-10-10	1998-09-14	1998-11-05	NONE                     	AIR       	ests according 
1485505	168504	6053	4	32	50320.00	0.06	0.04	N	O	1998-11-18	1998-10-19	1998-11-22	TAKE BACK RETURN         	AIR       	re blithely
1485505	33239	3240	5	10	11722.30	0.04	0.01	N	O	1998-09-15	1998-10-05	1998-10-13	TAKE BACK RETURN         	RAIL      	ular deposits wake blithely across th
1485505	31850	6857	6	50	89092.50	0.05	0.02	N	O	1998-09-18	1998-10-10	1998-10-03	TAKE BACK RETURN         	RAIL      	dependencies above the f
1490341	138455	5995	1	15	22401.75	0.04	0.02	N	O	1996-02-29	1996-01-20	1996-03-16	NONE                     	REG AIR   	 pending requests ea
1490341	1841	1842	2	42	73199.28	0.02	0.02	N	O	1996-01-12	1995-12-11	1996-01-29	COLLECT COD              	AIR       	according to the blithely sly
1490341	130302	7842	3	12	15987.60	0.10	0.01	N	O	1996-01-26	1996-01-14	1996-01-27	TAKE BACK RETURN         	TRUCK     	ickly final courts. sile
1490341	173008	560	4	4	4324.00	0.10	0.07	N	O	1995-12-21	1996-01-29	1995-12-30	DELIVER IN PERSON        	SHIP      	counts shall have to use quickly across t
1490341	134208	4209	5	2	2484.40	0.01	0.03	N	O	1995-12-11	1996-01-07	1995-12-13	DELIVER IN PERSON        	TRUCK     	deas wake against the furiously blithe theo
1490341	32860	2861	6	16	28685.76	0.06	0.01	N	O	1995-11-09	1995-12-10	1995-11-17	TAKE BACK RETURN         	TRUCK     	 haggle furiously. special, iro
1490849	180261	2780	1	17	22801.42	0.03	0.06	A	F	1993-08-12	1993-08-24	1993-08-19	NONE                     	AIR       	ent Tiresia
1490849	36243	3753	2	2	2358.48	0.00	0.08	A	F	1993-08-23	1993-08-22	1993-09-09	NONE                     	FOB       	 express packages. blithely f
1490849	123568	1105	3	50	79578.00	0.05	0.03	A	F	1993-08-22	1993-08-31	1993-09-12	DELIVER IN PERSON        	TRUCK     	 deposits 
1490849	2497	9998	4	32	44783.68	0.00	0.01	A	F	1993-10-06	1993-08-20	1993-10-26	NONE                     	TRUCK     	ecial theodolites! stea
1490849	155609	3155	5	32	53267.20	0.04	0.08	A	F	1993-10-15	1993-09-22	1993-10-30	DELIVER IN PERSON        	FOB       	y tithes. blit
1495106	66694	9201	1	22	36535.18	0.00	0.00	N	O	1997-02-11	1997-02-25	1997-02-16	COLLECT COD              	RAIL      	l excuses. ironic accounts are slyl
1495106	118408	920	2	23	32807.20	0.07	0.02	N	O	1997-01-26	1997-02-23	1997-02-25	TAKE BACK RETURN         	FOB       	ts integrate c
1495106	71645	9167	3	38	61432.32	0.09	0.06	N	O	1997-03-12	1997-03-03	1997-03-20	NONE                     	SHIP      	 of the careful
1495106	124399	6912	4	25	35584.75	0.02	0.05	N	O	1997-01-21	1997-03-19	1997-01-23	DELIVER IN PERSON        	RAIL      	riously regular deposit
1526534	30536	8046	1	13	19064.89	0.03	0.03	N	F	1995-06-04	1995-07-20	1995-07-01	TAKE BACK RETURN         	FOB       	ular pinto beans cajole according to th
1526534	9428	1929	2	2	2674.84	0.03	0.06	N	O	1995-08-25	1995-08-07	1995-09-01	COLLECT COD              	MAIL      	ithely. slyly bold
1526534	50819	8335	3	46	81411.26	0.00	0.06	N	O	1995-08-14	1995-08-21	1995-08-20	TAKE BACK RETURN         	AIR       	osits. accounts among the bl
1526534	110599	3111	4	30	48287.70	0.03	0.06	N	O	1995-07-21	1995-07-18	1995-08-12	NONE                     	TRUCK     	he permanently ironic instructions are flu
1526534	25821	3328	5	4	6987.28	0.09	0.05	N	O	1995-09-28	1995-08-03	1995-10-17	DELIVER IN PERSON        	SHIP      	according to the instr
1543201	186021	3576	1	36	39852.72	0.08	0.01	N	O	1998-07-16	1998-09-04	1998-07-23	COLLECT COD              	FOB       	s. quickly silent deposits sle
1543201	49642	2147	2	23	36607.72	0.09	0.01	N	O	1998-08-03	1998-08-08	1998-08-30	NONE                     	SHIP      	posits sleep slyly beyo
1543201	124949	4950	3	30	59218.20	0.07	0.04	N	O	1998-07-29	1998-08-04	1998-08-14	TAKE BACK RETURN         	TRUCK     	 according to the slyly unusual th
1543201	5753	5754	4	11	18246.25	0.03	0.06	N	O	1998-08-13	1998-09-12	1998-08-27	DELIVER IN PERSON        	REG AIR   	sual waters
1543201	86542	9051	5	3	4585.62	0.10	0.05	N	O	1998-10-08	1998-09-11	1998-10-09	NONE                     	FOB       	oss the furiously ca
1543201	105373	5374	6	8	11026.96	0.08	0.07	N	O	1998-08-12	1998-07-17	1998-09-02	DELIVER IN PERSON        	TRUCK     	ng to the final accou
1543201	196589	6590	7	39	65737.62	0.05	0.08	N	O	1998-10-04	1998-08-30	1998-10-19	TAKE BACK RETURN         	RAIL      	ess depende
1546274	101616	4127	1	45	72792.45	0.03	0.08	R	F	1995-02-22	1995-02-27	1995-02-27	NONE                     	TRUCK     	uickly pending pinto b
1546274	54892	2408	2	29	53559.81	0.09	0.03	R	F	1995-02-06	1995-02-01	1995-02-19	TAKE BACK RETURN         	AIR       	sly regular ideas. furiously slow depo
1546274	157005	2036	3	34	36108.00	0.03	0.06	A	F	1994-12-31	1995-02-01	1995-01-14	TAKE BACK RETURN         	RAIL      	yly regular instructions nag car
1546274	173642	6160	4	28	48037.92	0.04	0.01	R	F	1994-12-28	1995-03-08	1995-01-13	TAKE BACK RETURN         	SHIP      	ickly among the regular, special plat
1546274	197956	7957	5	13	26701.35	0.05	0.02	A	F	1994-12-16	1995-02-02	1995-01-11	TAKE BACK RETURN         	TRUCK     	ctions. carefully regula
1546274	35947	5948	6	19	35775.86	0.07	0.07	A	F	1994-12-31	1995-02-03	1995-01-12	NONE                     	FOB       	rges wake around the fluffily bold packag
1546274	153670	3671	7	34	58604.78	0.07	0.00	R	F	1995-01-14	1995-02-23	1995-02-02	TAKE BACK RETURN         	AIR       	onic hockey players. pending orbits t
1576097	165557	590	1	5	8112.75	0.10	0.00	N	O	1998-08-02	1998-08-05	1998-08-23	TAKE BACK RETURN         	MAIL      	pendencies sleep before the pendin
1576097	110960	961	2	10	19709.60	0.06	0.05	N	O	1998-09-02	1998-07-27	1998-09-09	DELIVER IN PERSON        	TRUCK     	ggle slyly within the speci
1593344	85734	8243	1	37	63630.01	0.01	0.07	R	F	1994-03-11	1994-03-07	1994-03-21	NONE                     	SHIP      	are slyly furio
1593344	126687	6688	2	12	20564.16	0.09	0.07	R	F	1994-02-18	1994-02-26	1994-02-26	DELIVER IN PERSON        	RAIL      	telets hagg
1693984	168409	3442	1	29	42844.60	0.02	0.02	N	O	1996-07-13	1996-05-24	1996-08-09	DELIVER IN PERSON        	SHIP      	 final dugouts: slyly
1693984	39580	4587	2	24	36469.92	0.02	0.03	N	O	1996-04-30	1996-06-03	1996-05-26	COLLECT COD              	FOB       	ven ideas was. blithely ir
1693984	12972	476	3	43	81053.71	0.04	0.03	N	O	1996-05-26	1996-06-08	1996-06-24	DELIVER IN PERSON        	MAIL      	kly final, enticing instructions. pac
1693984	4768	9769	4	25	41819.00	0.02	0.06	N	O	1996-06-06	1996-04-23	1996-06-20	NONE                     	RAIL      	hely quiet accounts. quickly special i
1693984	134774	7288	5	6	10852.62	0.05	0.08	N	O	1996-05-02	1996-06-05	1996-05-30	TAKE BACK RETURN         	SHIP      	ses across the sl
1693984	146595	4138	6	45	73871.55	0.07	0.08	N	O	1996-05-06	1996-05-22	1996-05-25	COLLECT COD              	TRUCK     	egular deposits. slyly blithe foxes 
1709703	22020	7025	1	15	14130.30	0.10	0.08	N	O	1995-07-10	1995-09-05	1995-07-20	COLLECT COD              	SHIP      	final instr
1709703	73349	8364	2	14	18512.76	0.01	0.05	N	O	1995-07-30	1995-08-25	1995-08-28	TAKE BACK RETURN         	REG AIR   	ilent dolphins. 
1709703	167743	7744	3	1	1810.74	0.06	0.03	N	O	1995-08-13	1995-07-22	1995-08-14	COLLECT COD              	AIR       	slyly above the ironic, 
1709703	50998	6009	4	45	87704.55	0.09	0.03	N	O	1995-10-08	1995-07-29	1995-10-29	NONE                     	AIR       	ways bold, permanent instruction
1709703	112119	2120	5	36	40719.96	0.06	0.05	N	O	1995-08-22	1995-07-30	1995-09-18	TAKE BACK RETURN         	AIR       	lets: quickl
1709703	182795	7832	6	32	60089.28	0.04	0.00	N	O	1995-06-19	1995-08-23	1995-07-18	TAKE BACK RETURN         	AIR       	s. ironic deposits wa
1709703	30987	8497	7	13	24933.74	0.02	0.03	N	O	1995-08-05	1995-07-17	1995-08-23	TAKE BACK RETURN         	AIR       	hely after the regular reque
1763205	12629	5131	1	10	15416.20	0.00	0.07	A	F	1994-10-30	1994-10-24	1994-11-23	NONE                     	REG AIR   	. fluffily regular packages 
1783749	120697	8234	1	29	49813.01	0.07	0.01	N	O	1998-10-15	1998-10-11	1998-11-10	COLLECT COD              	MAIL      	lithely special requ
1783749	146370	8885	2	7	9914.59	0.00	0.01	N	O	1998-10-11	1998-09-21	1998-10-31	NONE                     	RAIL      	al platelets. regular accounts
1783749	36732	9236	3	15	25030.95	0.01	0.00	N	O	1998-07-23	1998-10-20	1998-07-30	COLLECT COD              	SHIP      	packages cajol
1783749	146503	6504	4	49	75925.50	0.00	0.06	N	O	1998-08-17	1998-08-25	1998-09-15	DELIVER IN PERSON        	FOB       	accounts. 
1783749	62980	7993	5	24	46631.52	0.00	0.00	N	O	1998-08-13	1998-09-07	1998-09-12	NONE                     	TRUCK     	uriously. furiously spec
1823424	180269	270	1	47	63415.22	0.03	0.01	R	F	1992-02-20	1992-04-05	1992-03-20	TAKE BACK RETURN         	TRUCK     	ironic fret
1823424	174544	4545	2	14	22659.56	0.08	0.04	R	F	1992-02-02	1992-03-24	1992-03-02	COLLECT COD              	MAIL      	 alongside of 
1823424	154130	6646	3	3	3552.39	0.08	0.01	R	F	1992-02-28	1992-03-18	1992-03-19	DELIVER IN PERSON        	SHIP      	uests. packages wake according to
1823424	55266	7772	4	4	4885.04	0.04	0.05	R	F	1992-03-04	1992-02-19	1992-04-01	DELIVER IN PERSON        	FOB       	daringly pending instructions. fluffily 
1823424	53820	1336	5	26	46119.32	0.07	0.05	R	F	1992-01-27	1992-02-25	1992-02-20	COLLECT COD              	MAIL      	 detect among the furiously regular deposit
1823424	142622	2623	6	38	63255.56	0.10	0.03	A	F	1992-04-03	1992-02-28	1992-04-23	COLLECT COD              	FOB       	arefully pe
1823424	38402	3409	7	27	36190.80	0.07	0.06	A	F	1992-04-17	1992-03-17	1992-05-06	COLLECT COD              	TRUCK     	 packages promise busily. quickly eve
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
1868608	99455	9456	1	17	24725.65	0.10	0.03	N	O	1998-06-20	1998-04-08	1998-07-07	TAKE BACK RETURN         	AIR       	uickly close theodolites. i
1868608	138349	3376	2	39	54106.26	0.04	0.07	N	O	1998-04-19	1998-05-18	1998-05-03	NONE                     	TRUCK     	odolites. permanently regular a
1868608	15748	3252	3	30	49912.20	0.06	0.01	N	O	1998-05-15	1998-05-22	1998-05-31	COLLECT COD              	TRUCK     	ily ironic de
1868608	60288	5301	4	46	57420.88	0.01	0.07	N	O	1998-05-06	1998-05-12	1998-06-02	NONE                     	SHIP      	uickly even foxes. carefully unusu
1894087	53573	6079	1	23	35111.11	0.00	0.04	R	F	1994-03-13	1994-03-28	1994-04-05	DELIVER IN PERSON        	SHIP      	e final packages are even pinto beans. t
1895491	89325	6850	1	21	27600.72	0.08	0.01	A	F	1993-03-09	1993-03-28	1993-03-25	DELIVER IN PERSON        	MAIL      	 asymptotes. alway
1895491	68886	1393	2	30	55646.40	0.00	0.05	A	F	1993-06-10	1993-05-16	1993-06-23	TAKE BACK RETURN         	REG AIR   	y unusual foxes. furiousl
1895491	154663	7179	3	47	80730.02	0.07	0.05	A	F	1993-03-07	1993-03-20	1993-04-05	TAKE BACK RETURN         	AIR       	ss accounts. f
1895491	6031	3532	4	32	29984.96	0.01	0.05	R	F	1993-03-23	1993-03-31	1993-04-17	COLLECT COD              	FOB       	slyly special requests. pinto beans
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
1959365	54586	7092	1	37	57001.46	0.02	0.02	N	O	1998-08-12	1998-06-24	1998-08-28	NONE                     	SHIP      	special pinto beans wake slyly bold packag
1959365	13493	5995	2	34	47820.66	0.06	0.01	N	O	1998-06-30	1998-06-08	1998-07-01	COLLECT COD              	MAIL      	kages? unusual t
1959365	170969	6004	3	22	44879.12	0.06	0.01	N	O	1998-06-13	1998-06-14	1998-07-10	TAKE BACK RETURN         	AIR       	uld have to sleep after the fluffily bli
2014848	15065	7567	1	44	43122.64	0.00	0.06	N	O	1997-03-28	1997-04-01	1997-04-06	COLLECT COD              	SHIP      	iously thin pinto beans sleep bli
2014848	193155	3156	2	30	37444.50	0.09	0.04	N	O	1997-03-21	1997-02-24	1997-03-23	DELIVER IN PERSON        	AIR       	sly final instructions. regular 
2014848	140639	8182	3	48	80622.24	0.01	0.05	N	O	1997-02-01	1997-02-27	1997-02-28	NONE                     	SHIP      	ages. furio
2014848	112844	378	4	9	16711.56	0.09	0.08	N	O	1997-04-07	1997-03-26	1997-04-12	COLLECT COD              	TRUCK     	sly bold p
2022438	62908	427	1	33	61739.70	0.06	0.07	N	O	1997-11-02	1997-09-20	1997-11-28	DELIVER IN PERSON        	AIR       	 deposits across the carefully regular req
2022438	94167	9186	2	9	10450.44	0.10	0.06	N	O	1997-11-07	1997-09-05	1997-11-26	TAKE BACK RETURN         	FOB       	deposits. slyly even 
2022438	63838	3839	3	9	16216.47	0.07	0.08	N	O	1997-09-16	1997-09-06	1997-09-27	TAKE BACK RETURN         	TRUCK     	the regular courts. quickly
2022438	121290	8827	4	11	14424.19	0.04	0.04	N	O	1997-08-21	1997-09-26	1997-09-07	COLLECT COD              	AIR       	ns wake. quickly final realms according 
2022438	116944	4478	5	17	33335.98	0.10	0.05	N	O	1997-10-21	1997-10-05	1997-11-17	COLLECT COD              	FOB       	 nag quickly quick f
2022438	69461	4474	6	18	25748.28	0.02	0.01	N	O	1997-10-03	1997-10-04	1997-10-11	COLLECT COD              	REG AIR   	encies was blit
2046658	162152	2153	1	1	1214.15	0.06	0.07	R	F	1992-06-11	1992-08-04	1992-07-06	NONE                     	TRUCK     	st blithely final
2046658	79466	1974	2	48	69382.08	0.07	0.00	R	F	1992-08-22	1992-08-26	1992-08-24	NONE                     	REG AIR   	al notornis wake ca
2046658	175758	793	3	39	71516.25	0.03	0.02	A	F	1992-09-21	1992-07-08	1992-09-24	COLLECT COD              	REG AIR   	blithely carefully
2046658	152744	290	4	11	19764.14	0.08	0.01	A	F	1992-09-04	1992-07-04	1992-09-27	DELIVER IN PERSON        	RAIL      	lly silent packages. ins
2046658	167516	33	5	33	52255.83	0.09	0.05	A	F	1992-09-21	1992-07-05	1992-10-03	TAKE BACK RETURN         	AIR       	to beans ser
2073893	120150	151	1	26	30423.90	0.02	0.07	A	F	1994-03-15	1994-01-20	1994-04-05	COLLECT COD              	AIR       	among the blithely even accounts. bli
2073893	144321	1864	2	23	31402.36	0.02	0.03	R	F	1994-03-14	1994-02-11	1994-04-06	DELIVER IN PERSON        	TRUCK     	egular, special
2073893	193878	1436	3	41	80846.67	0.06	0.01	R	F	1993-12-04	1993-12-22	1994-01-01	TAKE BACK RETURN         	FOB       	 slyly even excuses us
2073893	98799	6327	4	25	44944.75	0.07	0.06	R	F	1994-02-20	1994-01-10	1994-03-15	NONE                     	MAIL      	ggle slyly
2096544	139461	9462	1	20	30009.20	0.02	0.08	R	F	1992-05-26	1992-05-22	1992-06-05	NONE                     	RAIL      	ng to the furiously even
2096544	109740	2251	2	13	22746.62	0.05	0.08	A	F	1992-05-14	1992-06-22	1992-06-03	COLLECT COD              	TRUCK     	ers haggle unusual epitaphs. furiou
2096544	184592	9629	3	4	6706.36	0.06	0.06	R	F	1992-04-19	1992-05-31	1992-04-21	TAKE BACK RETURN         	RAIL      	ong the regular, express pac
2096544	25429	434	4	9	12189.78	0.04	0.01	R	F	1992-07-25	1992-07-15	1992-08-17	COLLECT COD              	RAIL      	xes. furiously thin accounts engage furiou
2096544	179883	9884	5	23	45146.24	0.02	0.05	R	F	1992-05-08	1992-06-24	1992-05-26	COLLECT COD              	REG AIR   	es. regular deposits
2096544	79598	2106	6	9	14198.31	0.02	0.06	A	F	1992-07-19	1992-05-24	1992-07-25	DELIVER IN PERSON        	SHIP      	s the ironic, final accounts. carefully b
2096544	813	814	7	31	53128.11	0.07	0.02	R	F	1992-06-23	1992-06-29	1992-07-11	NONE                     	REG AIR   	ickly around the idle, even foxes. carefull
2125318	66778	4297	1	5	8723.85	0.08	0.02	N	O	1995-07-06	1995-06-10	1995-07-30	TAKE BACK RETURN         	FOB       	e blithely bold packages. packages
2129698	26338	1343	1	41	51837.53	0.10	0.01	N	O	1995-07-19	1995-08-13	1995-08-13	DELIVER IN PERSON        	FOB       	uthlessly special request
2129698	52784	5290	2	46	79891.88	0.02	0.03	N	O	1995-08-05	1995-09-16	1995-08-10	COLLECT COD              	AIR       	p ironic theodol
2129698	132413	7440	3	44	63598.04	0.08	0.07	N	O	1995-07-09	1995-08-09	1995-07-10	TAKE BACK RETURN         	SHIP      	tect. foxes doubt slyly regul
2164326	194917	2475	1	43	86512.13	0.10	0.00	N	O	1996-05-24	1996-07-03	1996-06-04	DELIVER IN PERSON        	REG AIR   	dolphins. depo
2164326	154686	7202	2	34	59183.12	0.01	0.05	N	O	1996-08-16	1996-06-24	1996-08-29	TAKE BACK RETURN         	REG AIR   	 instructions cajole above the silent depo
2164326	197566	86	3	12	19962.72	0.06	0.02	N	O	1996-05-25	1996-07-18	1996-05-28	TAKE BACK RETURN         	REG AIR   	es sleep furiously furious packag
2164326	80145	2654	4	28	31503.92	0.01	0.01	N	O	1996-07-04	1996-06-18	1996-07-26	COLLECT COD              	MAIL      	eans: enticingly fina
2164326	188821	6376	5	39	74482.98	0.03	0.07	N	O	1996-07-29	1996-06-18	1996-08-16	TAKE BACK RETURN         	FOB       	inst the slyly ironic excuses. carefully si
2164326	99359	9360	6	25	33958.75	0.00	0.06	N	O	1996-08-13	1996-08-09	1996-08-24	TAKE BACK RETURN         	FOB       	g furiously quickly regul
2177348	74556	9571	1	31	47447.05	0.07	0.07	A	F	1994-04-12	1994-02-28	1994-05-06	TAKE BACK RETURN         	TRUCK     	 requests 
2205862	57584	5100	1	23	35456.34	0.02	0.01	N	O	1998-02-16	1998-02-08	1998-02-28	NONE                     	RAIL      	hlessly dogged deposits. careful
2205862	2980	2981	2	7	13180.86	0.02	0.08	N	O	1998-01-17	1998-03-16	1998-01-19	DELIVER IN PERSON        	MAIL      	 patterns sl
2205862	75672	5673	3	46	75792.82	0.09	0.07	N	O	1998-04-26	1998-03-06	1998-04-27	COLLECT COD              	SHIP      	 quickly b
2205862	7705	206	4	34	54831.80	0.08	0.00	N	O	1998-01-22	1998-03-16	1998-02-19	TAKE BACK RETURN         	TRUCK     	ackages boost quickly alongside of the slyl
2205862	85466	2991	5	20	29029.20	0.04	0.07	N	O	1998-02-14	1998-03-14	1998-03-05	NONE                     	FOB       	 courts hag
2205862	14370	6872	6	3	3853.11	0.10	0.03	N	O	1998-03-23	1998-02-10	1998-03-26	TAKE BACK RETURN         	FOB       	special instruction
2205862	106404	8915	7	21	29618.40	0.01	0.08	N	O	1998-01-29	1998-02-06	1998-02-27	DELIVER IN PERSON        	FOB       	ld packages. fina
2264739	83571	6080	1	42	65291.94	0.09	0.00	N	O	1998-07-07	1998-08-21	1998-08-04	COLLECT COD              	REG AIR   	egrate blithely slyly 
2264739	170249	5284	2	44	58046.56	0.06	0.06	N	O	1998-08-08	1998-06-28	1998-08-28	DELIVER IN PERSON        	TRUCK     	olites haggl
2264739	170792	3310	3	33	61472.07	0.00	0.08	N	O	1998-07-06	1998-08-04	1998-07-28	TAKE BACK RETURN         	FOB       	cajole slyly: blithely special pa
2264739	96977	4505	4	3	5921.91	0.07	0.07	N	O	1998-07-13	1998-07-11	1998-07-26	TAKE BACK RETURN         	FOB       	arly special f
2264739	91436	8964	5	38	54242.34	0.01	0.00	N	O	1998-06-20	1998-07-26	1998-06-25	COLLECT COD              	REG AIR   	bout the accounts.
2264739	154510	2056	6	4	6258.04	0.00	0.07	N	O	1998-08-20	1998-07-06	1998-09-05	DELIVER IN PERSON        	REG AIR   	ckages sleep slyly alongside of the
2264739	34183	6687	7	27	30163.86	0.04	0.05	N	O	1998-08-01	1998-07-24	1998-08-17	DELIVER IN PERSON        	MAIL      	s? express deposits are furiously. pa
2290245	197759	7760	1	46	85410.50	0.07	0.02	N	O	1997-03-21	1997-01-24	1997-03-30	DELIVER IN PERSON        	TRUCK     	quickly pending accounts. slo
2290245	134594	9621	2	1	1628.59	0.03	0.06	N	O	1997-04-04	1997-02-09	1997-04-15	TAKE BACK RETURN         	REG AIR   	ding packages. quickly ev
2290245	105110	5111	3	18	20071.98	0.07	0.07	N	O	1997-01-13	1997-02-24	1997-02-11	COLLECT COD              	REG AIR   	t the regular accounts wake about t
2290245	167214	4763	4	48	61498.08	0.05	0.00	N	O	1997-02-05	1997-03-07	1997-03-04	DELIVER IN PERSON        	MAIL      	. furiously final requests haggle. final re
2290245	149442	6985	5	24	35794.56	0.01	0.08	N	O	1997-01-10	1997-03-06	1997-02-01	TAKE BACK RETURN         	FOB       	. furiously ir
2290245	62641	2642	6	31	49712.84	0.02	0.07	N	O	1997-01-13	1997-01-15	1997-02-11	TAKE BACK RETURN         	RAIL      	es. furiously ironic ideas wake quickly f
2332928	155158	2704	1	47	57018.05	0.06	0.08	R	F	1995-01-03	1994-12-30	1995-01-09	TAKE BACK RETURN         	SHIP      	hely final 
2415204	117488	5022	1	49	73768.52	0.06	0.04	R	F	1994-11-17	1995-01-22	1994-12-02	NONE                     	AIR       	counts. silent excuses
2415204	17342	2345	2	27	34002.18	0.09	0.01	R	F	1995-01-10	1995-01-01	1995-01-27	TAKE BACK RETURN         	RAIL      	ously unusual theodolite
2415204	62424	4931	3	5	6932.10	0.03	0.06	A	F	1994-11-20	1994-12-26	1994-12-07	NONE                     	MAIL      	ests. blithely ironic pac
2415204	149265	4294	4	7	9199.82	0.05	0.02	R	F	1995-01-02	1995-01-03	1995-01-21	NONE                     	FOB       	. finally ironic deposits detect carefull
2415204	87718	227	5	40	68228.40	0.04	0.00	R	F	1995-01-06	1994-12-27	1995-01-08	COLLECT COD              	REG AIR   	riously. sl
2420194	49067	6580	1	6	6096.36	0.03	0.05	N	O	1996-05-21	1996-07-08	1996-06-20	TAKE BACK RETURN         	MAIL      	ages. even pack
2420194	57168	4684	2	41	46131.56	0.01	0.06	N	O	1996-04-28	1996-06-02	1996-05-16	TAKE BACK RETURN         	MAIL      	lar accounts us
2420194	12202	2203	3	27	30083.40	0.07	0.05	N	O	1996-05-06	1996-07-07	1996-05-13	COLLECT COD              	MAIL      	f the express foxes. blith
2462560	98111	5639	1	7	7763.77	0.09	0.02	N	O	1996-06-26	1996-07-05	1996-07-07	TAKE BACK RETURN         	MAIL      	fluffily after the ironic ideas. even, s
2462560	137375	2402	2	43	60731.91	0.05	0.07	N	O	1996-06-22	1996-06-27	1996-07-22	DELIVER IN PERSON        	AIR       	uses nag blithely boldly s
2514754	6886	9387	1	7	12550.16	0.03	0.01	R	F	1994-05-21	1994-07-03	1994-06-02	NONE                     	TRUCK     	, final reques
2514754	123177	714	2	10	12001.70	0.02	0.03	R	F	1994-04-10	1994-07-01	1994-05-01	NONE                     	FOB       	e ironic, ironi
2514754	32964	474	3	40	75878.40	0.06	0.01	R	F	1994-06-26	1994-06-28	1994-07-07	DELIVER IN PERSON        	RAIL      	heodolites haggle car
2541764	183078	633	1	20	23221.40	0.09	0.03	R	F	1993-05-03	1993-04-19	1993-05-24	TAKE BACK RETURN         	FOB       	odolites. slyly special accounts 
2755046	180205	7760	1	27	34700.40	0.06	0.02	A	F	1993-06-01	1993-06-14	1993-06-26	TAKE BACK RETURN         	REG AIR   	ly ironic foxes sleep s
2755046	100904	5925	2	32	60956.80	0.09	0.01	A	F	1993-06-27	1993-06-23	1993-07-27	DELIVER IN PERSON        	FOB       	 final accounts integrate. quickly e
2799425	51512	4018	1	21	30733.71	0.03	0.03	N	O	1996-01-23	1996-02-04	1996-02-11	COLLECT COD              	TRUCK     	ckly along the
2799425	146923	6924	2	15	29548.80	0.03	0.03	N	O	1996-01-30	1996-01-20	1996-02-28	TAKE BACK RETURN         	FOB       	lly slyly express instructions. quietly 
2799425	124610	4611	3	14	22884.54	0.04	0.00	N	O	1996-02-04	1995-12-31	1996-02-21	NONE                     	MAIL      	uests. accounts haggle
2799425	111664	9198	4	1	1675.66	0.02	0.03	N	O	1996-02-24	1995-12-21	1996-02-25	TAKE BACK RETURN         	SHIP      	 the even requests. slyly ironic theodolit
2842599	192586	144	1	48	80571.84	0.08	0.08	R	F	1994-12-09	1994-10-21	1995-01-02	COLLECT COD              	MAIL      	 packages cajole blithely. express packa
2842599	187220	9739	2	50	65361.00	0.10	0.07	R	F	1994-09-08	1994-10-08	1994-09-29	TAKE BACK RETURN         	RAIL      	erve daringly final accounts. dep
2842599	2069	7070	3	41	39813.46	0.00	0.02	A	F	1994-10-06	1994-10-16	1994-11-02	NONE                     	RAIL      	gle slyly. slyly final 
2842599	158227	3258	4	32	41127.04	0.02	0.06	A	F	1994-11-28	1994-10-13	1994-12-19	NONE                     	FOB       	hin the furiously bold deposits. iron
2842599	120723	8260	5	45	78467.40	0.07	0.05	R	F	1994-09-10	1994-11-02	1994-10-01	COLLECT COD              	AIR       	ut the bold packages. slyly final inst
2872675	173768	1320	1	50	92088.00	0.10	0.06	N	O	1997-05-29	1997-05-21	1997-06-10	TAKE BACK RETURN         	TRUCK     	 are carefully pending a
2872675	112088	9622	2	1	1100.08	0.00	0.03	N	O	1997-04-05	1997-05-19	1997-04-20	COLLECT COD              	SHIP      	ickly thin theod
2872675	59735	9736	3	47	79652.31	0.07	0.00	N	O	1997-03-15	1997-04-16	1997-03-29	TAKE BACK RETURN         	SHIP      	o the slowly express idea
2872675	165992	1025	4	46	94667.54	0.09	0.06	N	O	1997-06-26	1997-04-27	1997-07-15	TAKE BACK RETURN         	FOB       	xes cajole
2872675	13832	8835	5	20	34916.60	0.08	0.08	N	O	1997-03-25	1997-05-15	1997-03-27	TAKE BACK RETURN         	REG AIR   	 the blithely permanent accounts. 
2872675	60332	5345	6	3	3876.99	0.05	0.05	N	O	1997-05-31	1997-04-03	1997-06-13	NONE                     	MAIL      	e. excuses integrate alo
2872675	196529	9049	7	18	29259.36	0.05	0.03	N	O	1997-06-21	1997-04-01	1997-07-11	TAKE BACK RETURN         	MAIL      	less accounts boost slowly along the bold,
2923650	76872	4394	1	1	1848.87	0.06	0.05	A	F	1994-06-08	1994-04-18	1994-06-09	NONE                     	RAIL      	lar asymptotes. slyly
2923650	180031	2550	2	45	49996.35	0.03	0.03	A	F	1994-05-06	1994-04-14	1994-05-21	TAKE BACK RETURN         	RAIL      	uests about the sauternes wake flu
2923650	166725	4274	3	37	66293.64	0.02	0.06	R	F	1994-05-24	1994-04-08	1994-05-29	COLLECT COD              	FOB       	egular accounts
2923650	131432	3946	4	32	46829.76	0.04	0.06	A	F	1994-04-17	1994-05-01	1994-04-19	NONE                     	AIR       	 haggle furiously 
2943521	157394	4940	1	21	30479.19	0.00	0.04	N	O	1998-02-21	1998-04-21	1998-02-22	COLLECT COD              	AIR       	 the bold, special pinto
2953057	2298	2299	1	25	30007.25	0.08	0.05	N	O	1996-06-03	1996-07-03	1996-06-29	TAKE BACK RETURN         	TRUCK     	al, even theodolites. blithely silen
2953057	18886	1388	2	17	30682.96	0.09	0.04	N	O	1996-07-06	1996-07-19	1996-07-13	COLLECT COD              	TRUCK     	aters about the
2953057	12083	7086	3	27	26867.16	0.09	0.00	N	O	1996-06-04	1996-07-15	1996-06-27	TAKE BACK RETURN         	AIR       	deposits! instructions w
2953057	73585	1107	4	10	15585.80	0.07	0.04	N	O	1996-07-05	1996-07-01	1996-07-06	TAKE BACK RETURN         	AIR       	deas. slyly even pinto bea
2992930	84928	7437	1	50	95646.00	0.00	0.08	A	F	1994-09-12	1994-08-18	1994-10-03	NONE                     	AIR       	 slyly final instr
3038880	25391	2898	1	11	14480.29	0.06	0.07	N	O	1995-09-24	1995-11-18	1995-09-28	NONE                     	TRUCK     	ly pending instructions a
3038880	142394	2395	2	42	60328.38	0.10	0.04	N	O	1995-10-30	1995-10-18	1995-11-29	TAKE BACK RETURN         	AIR       	luffily special foxes nag blith
3038880	603	604	3	43	64654.80	0.07	0.07	N	O	1995-10-21	1995-10-16	1995-10-23	DELIVER IN PERSON        	FOB       	 regular account
3038880	124899	9924	4	3	5771.67	0.01	0.02	N	O	1995-11-12	1995-10-24	1995-12-05	COLLECT COD              	AIR       	ecial requests ca
3038880	173759	8794	5	31	56815.25	0.09	0.05	N	O	1995-12-13	1995-10-10	1995-12-16	NONE                     	REG AIR   	special accounts detect beyond the slyly
3038880	151513	6544	6	41	64144.91	0.02	0.05	N	O	1995-10-06	1995-10-28	1995-10-08	NONE                     	SHIP      	onic requests; final, bold packages beneath
3038880	72629	7644	7	41	65666.42	0.05	0.06	N	O	1995-10-21	1995-10-22	1995-11-04	DELIVER IN PERSON        	FOB       	ial deposits. slow, special deposits h
3082272	153852	1398	1	31	59081.35	0.09	0.05	N	O	1997-02-20	1997-01-31	1997-03-20	DELIVER IN PERSON        	MAIL      	 regular ac
3082272	173908	1460	2	4	7927.60	0.09	0.06	N	O	1997-02-05	1996-12-26	1997-02-27	NONE                     	AIR       	l requests cajole pending fo
3154370	12061	4563	1	22	21407.32	0.10	0.06	R	F	1993-09-02	1993-08-31	1993-09-29	TAKE BACK RETURN         	MAIL      	th the requests. regular dep
3154370	101775	6796	2	25	44419.25	0.03	0.06	R	F	1993-09-30	1993-08-15	1993-10-17	DELIVER IN PERSON        	REG AIR   	 even theodolites. s
3154370	119165	4188	3	3	3552.48	0.04	0.03	A	F	1993-09-02	1993-07-26	1993-09-13	NONE                     	MAIL      	ording to the final, e
3178627	31106	3610	1	5	5185.50	0.10	0.08	N	O	1996-05-23	1996-05-11	1996-06-11	NONE                     	FOB       	ayers throughout the 
3178627	191075	8633	2	26	30317.82	0.06	0.07	N	O	1996-04-11	1996-04-25	1996-04-23	DELIVER IN PERSON        	FOB       	 the quickly silent
3178627	192089	9647	3	49	57872.92	0.02	0.06	N	O	1996-05-26	1996-03-28	1996-05-28	TAKE BACK RETURN         	RAIL      	lar instructions. slyl
3178627	29882	9883	4	12	21742.56	0.00	0.05	N	O	1996-03-08	1996-04-30	1996-03-29	TAKE BACK RETURN         	RAIL      	 special foxes are carefully along the qu
3211909	153053	5569	1	5	5530.25	0.00	0.04	R	F	1993-09-25	1993-08-02	1993-10-06	TAKE BACK RETURN         	AIR       	egular packages integrate 
3211909	155177	208	2	4	4928.68	0.08	0.04	A	F	1993-10-14	1993-07-30	1993-11-02	DELIVER IN PERSON        	FOB       	 furiously speci
3211909	48262	8263	3	8	9682.08	0.00	0.08	R	F	1993-09-14	1993-08-31	1993-10-07	COLLECT COD              	RAIL      	tions are slyly regular requests. 
3211909	131471	6498	4	22	33054.34	0.00	0.04	R	F	1993-08-14	1993-08-10	1993-08-16	DELIVER IN PERSON        	SHIP      	e final, pending accounts. ironic account
3211909	18132	3135	5	35	36754.55	0.03	0.07	R	F	1993-10-17	1993-08-11	1993-11-08	DELIVER IN PERSON        	FOB       	unts according to the slyly regular pinto 
3211909	140675	5704	6	29	49754.43	0.09	0.02	R	F	1993-08-05	1993-09-08	1993-08-07	COLLECT COD              	TRUCK     	packages nag fluffily. care
3211909	113087	8110	7	43	47303.44	0.01	0.02	A	F	1993-08-06	1993-09-10	1993-08-14	COLLECT COD              	SHIP      	. quickly regular requests boost sl
3247714	159482	1998	1	10	15414.80	0.04	0.01	R	F	1993-10-06	1993-11-06	1993-10-13	COLLECT COD              	FOB       	ironic instructions haggle quic
3247714	197196	2235	2	5	6465.95	0.02	0.01	R	F	1993-09-17	1993-10-26	1993-10-16	DELIVER IN PERSON        	MAIL      	rs snooze carefu
3247714	71511	9033	3	44	65230.44	0.05	0.06	R	F	1993-11-09	1993-10-16	1993-12-09	TAKE BACK RETURN         	AIR       	 asymptotes. slyly i
3247714	82408	4917	4	35	48664.00	0.05	0.04	A	F	1993-10-29	1993-09-22	1993-11-25	TAKE BACK RETURN         	SHIP      	 furiously regular asymptot
3318789	3143	5644	1	33	34522.62	0.09	0.07	A	F	1992-07-21	1992-05-31	1992-08-15	TAKE BACK RETURN         	AIR       	lar, regular accounts nod 
3318789	15884	5885	2	33	59396.04	0.03	0.02	R	F	1992-08-15	1992-06-05	1992-08-22	COLLECT COD              	REG AIR   	ly ironic dependenc
3318789	154701	9732	3	15	26335.50	0.09	0.05	A	F	1992-05-31	1992-06-21	1992-06-29	NONE                     	FOB       	ackages haggle furio
3318789	52262	9778	4	39	47356.14	0.01	0.06	R	F	1992-07-23	1992-07-26	1992-08-07	DELIVER IN PERSON        	FOB       	ses. blithely regular pac
3318789	167000	9517	5	14	14938.00	0.03	0.01	A	F	1992-08-18	1992-06-26	1992-09-11	DELIVER IN PERSON        	AIR       	requests. quietly unusual foxes 
3318789	155063	94	6	26	29069.56	0.09	0.04	R	F	1992-07-07	1992-06-10	1992-07-26	TAKE BACK RETURN         	AIR       	 bold packages 
3318789	134062	4063	7	36	39458.16	0.01	0.03	R	F	1992-08-13	1992-07-03	1992-09-07	DELIVER IN PERSON        	FOB       	y regular ideas are. furiously unus
3330881	743	744	1	40	65749.60	0.10	0.07	A	F	1995-03-05	1995-04-01	1995-03-14	DELIVER IN PERSON        	REG AIR   	oost carefull
3330881	87412	7413	2	15	20991.15	0.04	0.03	R	F	1995-02-20	1995-04-26	1995-02-26	TAKE BACK RETURN         	FOB       	nstructions cajole bl
3330881	133407	947	3	33	47533.20	0.09	0.01	A	F	1995-03-23	1995-03-10	1995-04-04	NONE                     	RAIL      	ly carefully ironic excuses. furiously e
3330881	156582	1613	4	14	22940.12	0.03	0.01	A	F	1995-04-16	1995-03-17	1995-05-01	TAKE BACK RETURN         	SHIP      	 ironic depende
3330881	61622	1623	5	45	71262.90	0.06	0.06	A	F	1995-03-07	1995-04-04	1995-04-03	COLLECT COD              	REG AIR   	accounts among the blithely final requests 
3358691	139096	1610	1	46	52214.14	0.10	0.08	A	F	1994-02-19	1994-01-05	1994-03-01	NONE                     	TRUCK     	 furiously blithely r
3358691	174979	2531	2	26	53403.22	0.07	0.03	A	F	1994-03-02	1994-02-15	1994-03-20	NONE                     	SHIP      	ven accounts. pendi
3358691	63361	5868	3	41	54298.76	0.05	0.00	A	F	1993-12-11	1994-02-17	1993-12-27	COLLECT COD              	TRUCK     	t the slyly ironic instr
3358691	99905	7433	4	20	38098.00	0.01	0.00	R	F	1994-01-05	1994-01-15	1994-01-08	TAKE BACK RETURN         	TRUCK     	e blithely ironic packages. p
3358691	96503	1522	5	48	71976.00	0.06	0.05	A	F	1993-12-25	1994-01-15	1994-01-08	TAKE BACK RETURN         	FOB       	its boost of the slyly regular p
3358691	124968	9993	6	42	83704.32	0.08	0.03	R	F	1993-12-14	1994-01-19	1994-01-11	TAKE BACK RETURN         	MAIL      	 blithely bold deposits wake always final,
3358691	134728	2268	7	34	59932.48	0.02	0.02	R	F	1994-01-10	1994-01-20	1994-01-28	COLLECT COD              	AIR       	e the even packages.
3419044	125573	3110	1	18	28774.26	0.00	0.04	N	O	1998-04-14	1998-04-18	1998-05-13	NONE                     	SHIP      	he final excuses affix furiously frets.
3419044	105075	96	2	49	52923.43	0.03	0.00	N	O	1998-04-23	1998-03-29	1998-04-25	COLLECT COD              	MAIL      	 according to the blithely e
3419044	185930	967	3	22	44350.46	0.07	0.06	N	O	1998-05-21	1998-04-27	1998-05-31	COLLECT COD              	SHIP      	ng to the ironic, final accounts. pinto b
3419044	122226	2227	4	3	3744.66	0.00	0.02	N	O	1998-05-20	1998-04-11	1998-06-10	NONE                     	SHIP      	egular deposits. quickly final ho
3419044	41909	4414	5	43	79588.70	0.02	0.08	N	O	1998-05-25	1998-04-19	1998-05-27	DELIVER IN PERSON        	SHIP      	al, regular theodolite
3419044	155379	2925	6	19	27253.03	0.05	0.07	N	O	1998-03-13	1998-03-21	1998-03-15	COLLECT COD              	SHIP      	 blithely final dependencies. carefully 
3431909	164146	9179	1	29	35094.06	0.05	0.01	N	O	1997-05-25	1997-04-23	1997-06-12	DELIVER IN PERSON        	FOB       	to beans-- pending requests use
3431909	195035	5036	2	38	42941.14	0.09	0.08	N	O	1997-03-05	1997-03-31	1997-03-26	TAKE BACK RETURN         	MAIL      	r packages wake blithely c
3431909	136939	4479	3	36	71133.48	0.00	0.07	N	O	1997-04-05	1997-04-27	1997-05-01	NONE                     	SHIP      	beans breach furiously near the furious
3478503	188035	5590	1	27	30321.81	0.06	0.01	A	F	1992-04-15	1992-03-30	1992-04-27	NONE                     	MAIL      	theodolites sublate among t
3541094	39509	7019	1	3	4345.50	0.00	0.07	N	O	1998-05-08	1998-06-15	1998-05-20	DELIVER IN PERSON        	SHIP      	e blithely regular instructions? bol
3692388	29806	9807	1	15	26037.00	0.02	0.08	N	O	1997-05-05	1997-03-30	1997-05-14	NONE                     	MAIL      	special pl
3692615	59528	7044	1	18	26775.36	0.05	0.03	N	O	1995-12-04	1995-12-28	1995-12-16	DELIVER IN PERSON        	SHIP      	ges wake request
3692615	80377	378	2	31	42078.47	0.10	0.02	N	O	1995-11-27	1995-11-13	1995-12-19	COLLECT COD              	FOB       	pecial requests. deposits along t
3692615	92202	4712	3	38	45379.60	0.05	0.03	N	O	1995-12-24	1995-11-21	1996-01-21	NONE                     	TRUCK     	ously final reque
3692615	29197	6704	4	50	56309.50	0.09	0.01	N	O	1995-11-16	1996-01-02	1995-12-06	NONE                     	AIR       	pending epitaphs dete
3723172	164949	4950	1	7	14097.58	0.07	0.07	N	O	1995-07-29	1995-08-04	1995-08-25	NONE                     	AIR       	ly express packages. accou
3723172	96447	8957	2	48	69285.12	0.08	0.06	N	O	1995-07-27	1995-07-07	1995-08-01	TAKE BACK RETURN         	RAIL      	uffily furious patterns. furious
3723172	169253	1770	3	18	23800.50	0.09	0.00	N	O	1995-07-21	1995-07-05	1995-07-26	DELIVER IN PERSON        	AIR       	blithely unusual foxes-- evenly pe
3723172	24037	1544	4	20	19220.60	0.04	0.01	N	O	1995-09-08	1995-06-25	1995-09-09	NONE                     	MAIL      	ccording to the
3783492	175320	355	1	4	5581.28	0.07	0.06	N	O	1996-04-02	1996-03-30	1996-04-30	NONE                     	AIR       	eans haggle bli
3783492	2932	2933	2	25	45873.25	0.00	0.08	N	O	1996-03-08	1996-04-07	1996-03-31	TAKE BACK RETURN         	AIR       	ut the carefully sly ideas. req
3783492	73139	3140	3	46	51157.98	0.00	0.06	N	O	1996-05-24	1996-05-14	1996-06-18	COLLECT COD              	TRUCK     	 sheaves. s
3783492	33525	1035	4	25	36463.00	0.08	0.02	N	O	1996-04-03	1996-04-24	1996-04-24	DELIVER IN PERSON        	AIR       	ly regular Tiresias; instructions cajole 
3783492	53396	5902	5	25	33734.75	0.09	0.08	N	O	1996-03-30	1996-03-26	1996-04-23	COLLECT COD              	REG AIR   	es. pending instructions nod slyly. furiou
3783492	20812	5817	6	2	3465.62	0.04	0.02	N	O	1996-05-21	1996-04-06	1996-06-07	COLLECT COD              	TRUCK     	ependencies sleep quickly. carefully ir
3783492	181791	4310	7	10	18727.90	0.05	0.04	N	O	1996-06-01	1996-03-22	1996-06-20	TAKE BACK RETURN         	REG AIR   	ove the regular pinto be
3807267	129273	1786	1	16	20836.32	0.10	0.06	N	O	1995-12-17	1995-11-28	1995-12-23	COLLECT COD              	FOB       	y bold requests use even
3807267	83375	3376	2	36	48901.32	0.03	0.05	N	O	1995-09-21	1995-10-30	1995-10-06	TAKE BACK RETURN         	AIR       	gular plat
3807267	71687	1688	3	12	19904.16	0.07	0.00	N	O	1995-11-02	1995-11-26	1995-11-09	TAKE BACK RETURN         	TRUCK     	 the ruthlessly regular packages. furiously
3860257	16838	9340	1	28	49135.24	0.07	0.07	N	O	1998-06-24	1998-05-20	1998-07-24	NONE                     	RAIL      	en requests aga
3860257	193328	886	2	34	48324.88	0.10	0.08	N	O	1998-04-18	1998-06-12	1998-05-07	NONE                     	RAIL      	ing requests along the
3860257	57039	4555	3	2	1992.06	0.07	0.04	N	O	1998-04-12	1998-06-18	1998-04-30	DELIVER IN PERSON        	AIR       	he pending dependencies. fina
3860257	38527	3534	4	13	19051.76	0.09	0.06	N	O	1998-04-01	1998-06-13	1998-04-06	TAKE BACK RETURN         	MAIL      	y. regular packages integrate. bli
3860257	114044	6556	5	48	50785.92	0.08	0.01	N	O	1998-05-18	1998-05-15	1998-05-25	NONE                     	MAIL      	es wake quickly ironic accounts. regu
3860257	136848	6849	6	34	64084.56	0.09	0.05	N	O	1998-05-08	1998-05-04	1998-05-19	NONE                     	REG AIR   	he idly even accounts. re
3860257	190107	5146	7	43	51475.30	0.08	0.07	N	O	1998-05-02	1998-05-14	1998-05-15	DELIVER IN PERSON        	MAIL      	ly. fluffily iron
3864070	175272	5273	1	1	1347.27	0.02	0.03	R	F	1993-06-07	1993-08-12	1993-07-01	COLLECT COD              	REG AIR   	t the quick
3864070	193327	3328	2	29	41189.28	0.03	0.00	A	F	1993-06-19	1993-08-18	1993-07-01	TAKE BACK RETURN         	RAIL      	 carefully regular requ
3868359	68383	8384	1	36	48649.68	0.04	0.05	A	F	1992-10-29	1992-10-18	1992-10-30	NONE                     	RAIL      	ts. carefully express ideas run quickly c
3868359	105649	3180	2	9	14891.76	0.05	0.03	R	F	1992-10-27	1992-09-27	1992-11-26	DELIVER IN PERSON        	MAIL      	s wake furiously. platele
3868359	39769	2273	3	32	54680.32	0.07	0.01	R	F	1992-11-29	1992-10-13	1992-12-02	TAKE BACK RETURN         	REG AIR   	c ideas are quickly ironic 
3868359	194361	9400	4	6	8732.16	0.09	0.02	A	F	1992-11-03	1992-10-05	1992-11-21	COLLECT COD              	REG AIR   	s are. furiously final foxes are spec
3940897	156250	1281	1	3	3918.75	0.05	0.01	R	F	1992-06-11	1992-06-24	1992-06-19	TAKE BACK RETURN         	REG AIR   	ke quickly carefully ironic accounts. fina
3947046	68344	3357	1	40	52493.60	0.06	0.03	A	F	1993-09-30	1993-10-06	1993-10-02	NONE                     	RAIL      	he slyly ironic packages wake across t
3947046	44340	1853	2	30	38530.20	0.03	0.04	A	F	1993-09-10	1993-10-11	1993-09-15	TAKE BACK RETURN         	TRUCK     	ckly bold accoun
3947046	12362	2363	3	39	49700.04	0.06	0.05	R	F	1993-10-22	1993-08-17	1993-10-28	COLLECT COD              	SHIP      	posits are furiously. blithely exp
3947046	158088	8089	4	27	30944.16	0.00	0.03	R	F	1993-11-08	1993-08-26	1993-12-02	COLLECT COD              	REG AIR   	y regular ideas are furiousl
3947046	180418	2937	5	45	67428.45	0.03	0.08	R	F	1993-08-26	1993-09-27	1993-09-13	NONE                     	AIR       	le. carefully bold accoun
3955908	190155	5194	1	21	26148.15	0.08	0.00	N	O	1995-06-28	1995-09-07	1995-07-09	NONE                     	TRUCK     	heodolites boost a
3955908	123593	1130	2	45	72746.55	0.10	0.03	N	O	1995-07-22	1995-09-01	1995-08-11	DELIVER IN PERSON        	AIR       	 daringly against the accounts.
3955908	10974	975	3	35	65973.95	0.04	0.02	N	O	1995-07-06	1995-08-09	1995-07-22	DELIVER IN PERSON        	MAIL      	grow slyly. caref
3955908	28463	3468	4	22	30612.12	0.04	0.06	N	O	1995-09-29	1995-08-01	1995-10-12	DELIVER IN PERSON        	MAIL      	 ironic foxes. blithely
3986496	14260	4261	1	32	37576.32	0.02	0.04	N	O	1997-06-04	1997-04-01	1997-06-12	DELIVER IN PERSON        	FOB       	olites. packages bel
3986496	57360	2371	2	21	27664.56	0.03	0.03	N	O	1997-04-10	1997-05-14	1997-04-18	COLLECT COD              	REG AIR   	lar dependencies
3986496	36515	4025	3	49	71123.99	0.01	0.08	N	O	1997-03-06	1997-04-13	1997-03-09	NONE                     	AIR       	y regular foxes. furiously e
3986496	94207	6717	4	30	36036.00	0.03	0.06	N	O	1997-04-03	1997-04-04	1997-04-30	DELIVER IN PERSON        	FOB       	slyly ironic foxes 
3986496	128461	8462	5	28	41704.88	0.04	0.07	N	O	1997-06-09	1997-05-09	1997-06-27	TAKE BACK RETURN         	SHIP      	y special t
3986496	41797	6806	6	14	24343.06	0.10	0.05	N	O	1997-03-28	1997-04-01	1997-04-02	NONE                     	REG AIR   	ly according to the carefull
3986496	76510	9018	7	44	65406.44	0.04	0.08	N	O	1997-03-08	1997-03-31	1997-04-02	TAKE BACK RETURN         	TRUCK     	ress requests solve b
3994567	170491	492	1	19	29668.31	0.07	0.01	N	O	1997-05-20	1997-04-23	1997-06-19	TAKE BACK RETURN         	MAIL      	ironic asymptotes sleep slyly s
3994567	26119	1124	2	14	14631.54	0.01	0.04	N	O	1997-03-01	1997-04-22	1997-03-30	TAKE BACK RETURN         	TRUCK     	ully even asymptotes. s
3994567	180357	5394	3	13	18685.55	0.00	0.08	N	O	1997-03-12	1997-03-05	1997-03-27	NONE                     	MAIL      	among the doggedly bold acc
3994567	56395	3911	4	17	22973.63	0.02	0.04	N	O	1997-05-08	1997-04-08	1997-05-29	COLLECT COD              	REG AIR   	egular accounts. furiously final pac
3994567	24167	9172	5	23	25096.68	0.09	0.01	N	O	1997-04-20	1997-03-06	1997-05-03	NONE                     	RAIL      	st the never
3995111	189114	1633	1	25	30077.75	0.08	0.07	R	F	1994-03-01	1994-03-15	1994-03-03	TAKE BACK RETURN         	SHIP      	to the closely express 
3995111	29201	4206	2	34	38426.80	0.09	0.00	A	F	1994-06-13	1994-03-18	1994-06-30	TAKE BACK RETURN         	RAIL      	er express accoun
3995111	177916	2951	3	14	27914.74	0.05	0.03	A	F	1994-02-17	1994-05-07	1994-03-01	TAKE BACK RETURN         	RAIL      	 furiously regular ideas. furiously re
3995111	24407	4408	4	16	21302.40	0.02	0.01	A	F	1994-04-06	1994-04-03	1994-04-13	DELIVER IN PERSON        	REG AIR   	ly ironic the
3995111	192174	7213	5	13	16460.21	0.02	0.07	R	F	1994-05-14	1994-04-26	1994-06-09	NONE                     	MAIL      	ake slyly aga
3995111	59999	7515	6	5	9794.95	0.06	0.07	A	F	1994-05-09	1994-03-19	1994-05-10	COLLECT COD              	REG AIR   	 quickly carefully silent pinto beans.
4099329	68894	1401	1	25	46572.25	0.07	0.01	R	F	1992-07-08	1992-07-16	1992-08-07	COLLECT COD              	MAIL      	final ideas use express platelets. theod
4099329	95476	495	2	11	16186.17	0.01	0.05	A	F	1992-08-08	1992-08-22	1992-08-10	COLLECT COD              	MAIL      	lyly ironic pinto beans abou
4099329	60223	5236	3	42	49695.24	0.06	0.08	R	F	1992-06-22	1992-08-03	1992-06-29	TAKE BACK RETURN         	RAIL      	ly across th
4099329	162373	4890	4	5	7176.85	0.07	0.08	A	F	1992-07-31	1992-08-09	1992-08-02	DELIVER IN PERSON        	AIR       	ular deposits cajole quickly
4128289	113699	6211	1	10	17126.90	0.00	0.08	N	O	1998-03-14	1998-03-29	1998-03-15	COLLECT COD              	FOB       	 requests against the carefully bo
4128289	123197	8222	2	31	37825.89	0.05	0.08	N	O	1998-04-23	1998-04-12	1998-04-27	DELIVER IN PERSON        	REG AIR   	ronic accounts are. slyly quie
4163013	199414	1934	1	4	6053.64	0.04	0.05	R	F	1993-12-10	1994-01-14	1993-12-18	TAKE BACK RETURN         	SHIP      	heodolites haggle. final packages 
4163013	150895	896	2	41	79781.49	0.10	0.04	R	F	1994-02-19	1994-02-05	1994-03-12	DELIVER IN PERSON        	MAIL      	 carefully quickly even packages. final 
4204384	26622	4129	1	38	58847.56	0.05	0.06	R	F	1994-01-22	1993-12-21	1994-02-21	DELIVER IN PERSON        	AIR       	blithely bo
4204384	22532	39	2	44	63999.32	0.03	0.00	R	F	1993-11-23	1993-11-09	1993-11-25	TAKE BACK RETURN         	REG AIR   	ully regular somas. slyly final asymptotes 
4204384	84938	9955	3	42	80763.06	0.00	0.05	A	F	1993-12-30	1993-10-28	1994-01-08	DELIVER IN PERSON        	FOB       	ickly special requests cajo
4204384	142171	2172	4	8	9705.36	0.05	0.01	R	F	1993-12-02	1993-11-01	1993-12-14	NONE                     	AIR       	ic instructions. s
4204384	55169	5170	5	16	17986.56	0.09	0.03	R	F	1993-11-22	1993-12-20	1993-12-06	NONE                     	REG AIR   	 special somas sleep bli
4208006	129762	7299	1	39	69878.64	0.02	0.02	N	O	1997-03-20	1997-05-04	1997-04-19	DELIVER IN PERSON        	TRUCK     	ven accounts sleep fluffily
4208006	95047	66	2	14	14588.56	0.09	0.07	N	O	1997-02-21	1997-04-09	1997-03-04	COLLECT COD              	TRUCK     	across the fluffily id
4208006	49293	6806	3	15	18634.35	0.04	0.08	N	O	1997-04-18	1997-04-14	1997-05-03	COLLECT COD              	MAIL      	nticingly slyly bold fra
4208006	58661	1167	4	12	19435.92	0.00	0.02	N	O	1997-05-29	1997-03-13	1997-06-18	NONE                     	RAIL      	slowly across the slowly special theodolit
4208006	104192	6703	5	32	38278.08	0.04	0.03	N	O	1997-05-31	1997-04-06	1997-06-23	DELIVER IN PERSON        	MAIL      	uriously even 
4208674	93319	8338	1	27	35432.37	0.02	0.07	N	F	1995-05-27	1995-07-11	1995-06-24	COLLECT COD              	RAIL      	ly express pinto beans boost slyly. 
4208674	5706	8207	2	25	40292.50	0.04	0.02	N	O	1995-07-02	1995-06-20	1995-07-18	NONE                     	MAIL      	serve carefully ironic dependen
4208674	84394	1919	3	10	13783.90	0.02	0.04	R	F	1995-05-24	1995-07-23	1995-06-06	NONE                     	REG AIR   	lar, even deposits wak
4208674	2853	354	4	41	71989.85	0.06	0.00	N	F	1995-06-02	1995-06-20	1995-06-30	DELIVER IN PERSON        	MAIL      	carefully special deposits
4225824	59760	7276	1	10	17197.60	0.06	0.03	N	O	1995-10-31	1995-09-17	1995-11-18	DELIVER IN PERSON        	REG AIR   	ly ironic accounts about 
4225824	183436	3437	2	22	33427.46	0.06	0.06	N	O	1995-11-11	1995-09-05	1995-12-02	DELIVER IN PERSON        	SHIP      	special theod
4225824	2683	184	3	1	1585.68	0.02	0.05	N	O	1995-10-07	1995-09-09	1995-10-16	COLLECT COD              	MAIL      	luffily closely
4225824	88129	638	4	50	55856.00	0.01	0.07	N	O	1995-08-09	1995-08-26	1995-08-27	TAKE BACK RETURN         	FOB       	s are quickly even accounts. blithely 
4254181	20537	5542	1	35	51013.55	0.03	0.05	N	O	1997-05-11	1997-05-06	1997-06-04	NONE                     	TRUCK     	ets haggle slyly. final ins
4254181	156899	6900	2	24	46941.36	0.06	0.03	N	O	1997-06-23	1997-06-04	1997-07-14	NONE                     	REG AIR   	hely across the theodolites. ti
4254181	188678	1197	3	17	30033.39	0.09	0.06	N	O	1997-03-09	1997-05-26	1997-04-03	COLLECT COD              	TRUCK     	 quickly ironic excuses. closely fin
4254181	95691	3219	4	12	20240.28	0.04	0.00	N	O	1997-05-07	1997-04-20	1997-05-25	NONE                     	REG AIR   	regular, even i
4254181	76751	6752	5	41	70837.75	0.01	0.04	N	O	1997-05-12	1997-05-11	1997-06-09	TAKE BACK RETURN         	TRUCK     	ithely silent 
4254181	142892	7921	6	41	79330.49	0.01	0.02	N	O	1997-06-17	1997-04-30	1997-06-21	COLLECT COD              	RAIL      	equests haggle 
4273923	127174	9687	1	28	33632.76	0.09	0.07	N	O	1997-07-04	1997-06-10	1997-07-25	NONE                     	REG AIR   	asymptotes try to nag fluffily acros
4273923	12564	5066	2	42	62015.52	0.03	0.05	N	O	1997-06-06	1997-06-16	1997-06-13	COLLECT COD              	AIR       	ly even packages: ironic as
4303206	92235	2236	1	28	34362.44	0.05	0.05	A	F	1995-03-14	1995-01-11	1995-03-17	NONE                     	FOB       	y bold pinto beans above the pending, iro
4328998	89415	9416	1	38	53367.58	0.10	0.02	A	F	1992-05-14	1992-05-26	1992-05-31	TAKE BACK RETURN         	MAIL      	 final foxes detect unus
4328998	195864	8384	2	7	13719.02	0.08	0.02	R	F	1992-06-11	1992-05-31	1992-06-19	NONE                     	AIR       	 close pac
4328998	71586	9108	3	14	21806.12	0.01	0.03	A	F	1992-07-23	1992-06-19	1992-08-13	TAKE BACK RETURN         	TRUCK     	fluffily regular packages hagg
4328998	142508	7537	4	23	35661.50	0.03	0.04	R	F	1992-04-05	1992-06-23	1992-04-17	TAKE BACK RETURN         	MAIL      	ully special deposits. bold ideas s
4328998	119854	9855	5	39	73080.15	0.01	0.07	R	F	1992-06-16	1992-05-21	1992-07-08	NONE                     	MAIL      	kages are. ev
4328998	178840	1358	6	47	90185.48	0.09	0.05	A	F	1992-06-01	1992-05-08	1992-06-24	DELIVER IN PERSON        	FOB       	old instructions. slyly special 
4328998	160194	2711	7	32	40134.08	0.08	0.05	R	F	1992-06-11	1992-04-29	1992-06-29	TAKE BACK RETURN         	AIR       	nal theodolites. fluffily unusual acc
4378720	17294	2297	1	39	47240.31	0.01	0.05	N	O	1998-02-02	1997-12-21	1998-02-20	TAKE BACK RETURN         	FOB       	oost. final package
4378720	52011	2012	2	47	45261.47	0.01	0.08	N	O	1998-02-24	1998-01-09	1998-03-12	COLLECT COD              	REG AIR   	riously bold pinto be
4378720	142841	5356	3	2	3767.68	0.05	0.06	N	O	1998-03-04	1998-01-20	1998-03-16	TAKE BACK RETURN         	SHIP      	al, pending ideas haggle ab
4383265	138625	1139	1	46	76526.52	0.02	0.06	N	F	1995-06-06	1995-05-15	1995-07-02	DELIVER IN PERSON        	RAIL      	 bold, final
4383265	66541	1554	2	49	73869.46	0.05	0.05	A	F	1995-05-10	1995-05-31	1995-06-06	DELIVER IN PERSON        	SHIP      	ajole carefully am
4383265	143664	1207	3	28	47814.48	0.00	0.00	A	F	1995-05-04	1995-06-08	1995-05-10	COLLECT COD              	AIR       	le carefull
4383265	149180	6723	4	14	17208.52	0.10	0.08	R	F	1995-05-28	1995-05-18	1995-06-02	NONE                     	RAIL      	furiously across the final, express packa
4383265	67567	5086	5	15	23018.40	0.08	0.03	R	F	1995-04-02	1995-06-02	1995-04-23	NONE                     	REG AIR   	iously even instructio
4391456	47537	7538	1	10	14845.30	0.01	0.00	R	F	1992-07-18	1992-06-21	1992-08-01	NONE                     	AIR       	ully even deposits
4391456	140930	3445	2	44	86720.92	0.06	0.03	A	F	1992-06-23	1992-06-26	1992-07-11	NONE                     	RAIL      	ke carefully above the slyly final theodol
4391456	2414	4915	3	8	10531.28	0.03	0.00	R	F	1992-06-06	1992-05-16	1992-06-07	TAKE BACK RETURN         	TRUCK     	 carefully ironic deposits? sly
4391456	156296	8812	4	35	47330.15	0.00	0.07	A	F	1992-05-26	1992-06-10	1992-06-16	DELIVER IN PERSON        	MAIL      	ent, final instructions. instru
4391456	60000	2506	5	13	12480.00	0.09	0.05	R	F	1992-04-27	1992-07-01	1992-05-23	TAKE BACK RETURN         	TRUCK     	 foxes. fluffily s
4391456	15489	2993	6	40	56179.20	0.00	0.01	R	F	1992-04-21	1992-05-24	1992-05-05	COLLECT COD              	SHIP      	r ideas. even, regul
4391456	118989	8990	7	7	14055.86	0.05	0.05	A	F	1992-05-19	1992-05-12	1992-06-18	NONE                     	AIR       	hould use qu
4395841	108213	724	1	13	15875.73	0.06	0.06	N	O	1998-04-05	1998-02-27	1998-04-13	COLLECT COD              	SHIP      	l deposits thra
4395841	134166	4167	2	23	27603.68	0.07	0.07	N	O	1998-05-01	1998-03-26	1998-05-06	DELIVER IN PERSON        	AIR       	ual instructions impress blithely after t
4395841	116901	1924	3	44	84387.60	0.01	0.01	N	O	1998-02-24	1998-02-26	1998-03-26	NONE                     	TRUCK     	dolites. express
4395841	144008	6523	4	32	33664.00	0.05	0.08	N	O	1998-02-23	1998-03-04	1998-03-10	COLLECT COD              	REG AIR   	 at the carefully final requests
4395841	134098	9125	5	43	48679.87	0.08	0.00	N	O	1998-02-04	1998-04-07	1998-02-24	TAKE BACK RETURN         	FOB       	fully regular foxes integrate furious
4395841	170745	8297	6	48	87155.52	0.00	0.01	N	O	1998-05-18	1998-02-26	1998-05-27	TAKE BACK RETURN         	REG AIR   	furiously ironic packages. fluffily final
4395841	98010	520	7	7	7056.07	0.02	0.02	N	O	1998-04-30	1998-04-17	1998-05-06	NONE                     	MAIL      	quickly. quick
4402819	172854	406	1	21	40463.85	0.04	0.06	N	O	1996-01-28	1996-01-12	1996-02-13	TAKE BACK RETURN         	AIR       	, even theodolites sleep slyly
4402819	155716	5717	2	13	23032.23	0.05	0.04	N	O	1995-12-04	1995-12-09	1995-12-31	TAKE BACK RETURN         	REG AIR   	ronic, pending instructi
4402819	103795	6306	3	15	26981.85	0.01	0.08	N	O	1996-02-13	1996-01-03	1996-02-18	TAKE BACK RETURN         	RAIL      	deposits. b
4404676	127445	7446	1	48	70677.12	0.04	0.06	A	F	1992-02-23	1992-03-13	1992-03-20	COLLECT COD              	SHIP      	ly above the slow packag
4411653	79879	2387	1	46	85508.02	0.10	0.02	A	F	1992-04-10	1992-03-25	1992-04-19	TAKE BACK RETURN         	MAIL      	ss packages. slyly even packages against
4468801	117381	7382	1	44	61528.72	0.07	0.04	N	O	1997-03-13	1997-02-19	1997-04-09	COLLECT COD              	MAIL      	! blithely regula
4468801	18566	1068	2	41	60866.96	0.03	0.02	N	O	1997-01-31	1997-01-13	1997-02-09	NONE                     	SHIP      	. blithely ironic pinto beans are theod
4468801	22271	4774	3	38	45344.26	0.04	0.00	N	O	1997-03-17	1997-02-08	1997-04-06	COLLECT COD              	MAIL      	s. slyly expre
4468801	97796	2815	4	21	37669.59	0.00	0.08	N	O	1996-12-26	1997-02-05	1997-01-16	NONE                     	MAIL      	eposits play packages. final deposits s
4468801	51187	6198	5	46	52356.28	0.04	0.07	N	O	1997-02-18	1997-02-19	1997-03-13	DELIVER IN PERSON        	TRUCK     	uctions above the quic
4474501	159430	4461	1	21	31278.03	0.05	0.03	N	O	1995-12-30	1995-11-26	1996-01-03	TAKE BACK RETURN         	MAIL      	uriously bold acc
4474501	136826	4366	2	40	74512.80	0.03	0.01	N	O	1995-12-03	1995-12-09	1995-12-05	COLLECT COD              	FOB       	ctions run furiousl
4474501	33078	8085	3	23	23254.61	0.01	0.08	N	O	1995-10-02	1995-12-11	1995-10-14	NONE                     	SHIP      	egular ideas haggle blithe
4550145	95701	5702	1	47	79744.90	0.02	0.01	N	O	1995-10-11	1995-08-12	1995-11-02	COLLECT COD              	FOB       	ending foxes. i
4550145	44869	9878	2	34	61671.24	0.07	0.07	N	O	1995-07-01	1995-07-17	1995-07-08	DELIVER IN PERSON        	MAIL      	e quickly iro
4563873	27049	9552	1	41	40017.64	0.07	0.04	N	O	1997-03-25	1997-03-30	1997-04-22	COLLECT COD              	FOB       	ully regular foxes
4569348	10664	3166	1	43	67710.38	0.02	0.01	N	O	1998-03-02	1998-04-11	1998-03-20	NONE                     	RAIL      	ly even packages. ironic, ironi
4569348	23057	3058	2	22	21561.10	0.09	0.04	N	O	1998-06-07	1998-05-12	1998-06-10	DELIVER IN PERSON        	MAIL      	ke quickly. slyly final req
4569348	105317	7828	3	46	60826.26	0.09	0.02	N	O	1998-05-06	1998-04-03	1998-05-18	TAKE BACK RETURN         	MAIL      	fully ironic dependencies play throughout t
4569348	55403	7909	4	15	20376.00	0.06	0.00	N	O	1998-04-25	1998-04-06	1998-05-14	NONE                     	MAIL      	sts engage carefully. pl
4569348	147004	7005	5	46	48346.00	0.05	0.07	N	O	1998-05-15	1998-03-26	1998-06-09	COLLECT COD              	TRUCK     	sleep despite th
4569348	87928	2945	6	25	47898.00	0.01	0.04	N	O	1998-05-31	1998-03-29	1998-06-04	COLLECT COD              	SHIP      	kages sleep quickly furiously r
4587526	157319	7320	1	11	15139.41	0.03	0.01	N	O	1997-07-02	1997-07-06	1997-07-07	TAKE BACK RETURN         	MAIL      	lent instructions detect quickl
4587526	47219	2228	2	13	15160.73	0.03	0.04	N	O	1997-06-15	1997-07-01	1997-06-18	DELIVER IN PERSON        	TRUCK     	y final deposits integrate blithely
4587526	823	3324	3	48	82743.36	0.02	0.02	N	O	1997-06-10	1997-07-08	1997-07-05	TAKE BACK RETURN         	SHIP      	blithely e
4625152	148828	3857	1	22	41290.04	0.04	0.05	A	F	1993-10-22	1993-12-10	1993-11-11	DELIVER IN PERSON        	SHIP      	olites. regular requests 
4625152	138331	3358	2	47	64358.51	0.08	0.03	R	F	1993-12-04	1993-12-10	1993-12-23	TAKE BACK RETURN         	REG AIR   	beans. even accounts serve careful
4625152	123760	3761	3	14	24972.64	0.03	0.04	A	F	1994-01-22	1993-11-07	1994-02-04	DELIVER IN PERSON        	MAIL      	eas. final excuses use bl
4625152	22181	7186	4	36	39714.48	0.03	0.08	A	F	1993-10-06	1993-11-16	1993-11-02	COLLECT COD              	SHIP      	furiously. even, regular acco
4625152	181043	3562	5	2	2248.08	0.07	0.04	R	F	1993-11-09	1993-10-26	1993-11-18	DELIVER IN PERSON        	RAIL      	ckly carefully bold 
4625152	175230	265	6	49	63956.27	0.08	0.06	R	F	1993-09-30	1993-12-08	1993-10-20	COLLECT COD              	RAIL      	s boost fluffily blithely 
4659813	85060	2585	1	7	7315.42	0.01	0.03	R	F	1992-08-27	1992-09-11	1992-08-29	COLLECT COD              	FOB       	counts. slyly final
4683938	156110	8626	1	10	11661.10	0.02	0.01	N	O	1996-07-21	1996-08-14	1996-08-20	COLLECT COD              	MAIL      	 dogged instructions hinder carefully abou
4683938	132196	2197	2	12	14738.28	0.10	0.04	N	O	1996-09-05	1996-08-27	1996-09-13	DELIVER IN PERSON        	SHIP      	sts at the even, final accounts sleep reg
4683938	17380	2383	3	27	35029.26	0.00	0.08	N	O	1996-08-23	1996-08-30	1996-08-31	COLLECT COD              	AIR       	sits are. furio
4692995	55136	147	1	39	42554.07	0.10	0.04	R	F	1994-07-02	1994-05-06	1994-07-28	COLLECT COD              	REG AIR   	regular deposits wake fluffily. fu
4692995	65033	46	2	47	46907.41	0.01	0.02	R	F	1994-06-22	1994-05-10	1994-06-30	NONE                     	AIR       	. fluffily silent packages sleep quickly. f
4692995	6180	3681	3	28	30413.04	0.01	0.04	R	F	1994-08-03	1994-05-11	1994-09-01	NONE                     	SHIP      	ites among the blithely pending e
4692995	58034	3045	4	1	992.03	0.02	0.06	R	F	1994-06-07	1994-06-23	1994-06-23	NONE                     	TRUCK     	riously regular accounts. dependencies wak
4692995	163863	3864	5	2	3853.72	0.04	0.01	R	F	1994-05-06	1994-06-09	1994-05-20	TAKE BACK RETURN         	AIR       	 thinly despite the final sentiments. de
4692995	38966	1470	6	23	43814.08	0.04	0.00	A	F	1994-07-24	1994-06-15	1994-08-03	TAKE BACK RETURN         	AIR       	tions. slowly final 
4692995	86160	3685	7	38	43554.08	0.04	0.06	R	F	1994-05-28	1994-06-28	1994-05-31	TAKE BACK RETURN         	REG AIR   	ccounts above the
4702695	75876	8384	1	24	44444.88	0.02	0.01	R	F	1994-11-22	1994-10-16	1994-12-10	COLLECT COD              	RAIL      	ly pending requests cajole blithel
4702695	36329	1336	2	13	16449.16	0.01	0.06	R	F	1994-11-09	1994-10-01	1994-11-10	COLLECT COD              	RAIL      	aggle carefully. carefully s
4702695	175230	7748	3	15	19578.45	0.01	0.01	A	F	1994-11-29	1994-10-21	1994-12-23	TAKE BACK RETURN         	RAIL      	n sentimen
4702695	78670	8671	4	35	57703.45	0.04	0.08	A	F	1994-10-10	1994-10-25	1994-10-25	DELIVER IN PERSON        	SHIP      	es above the unusual excuses boo
4702695	199827	2347	5	39	75145.98	0.01	0.08	A	F	1994-08-26	1994-10-01	1994-09-04	NONE                     	TRUCK     	y. slyly even 
4773985	25209	5210	1	12	13610.40	0.06	0.02	N	O	1996-09-07	1996-08-14	1996-09-13	COLLECT COD              	FOB       	detect never according to the 
4773985	84096	4097	2	47	50764.23	0.07	0.00	N	O	1996-07-03	1996-07-19	1996-07-13	TAKE BACK RETURN         	MAIL      	ironic requests. 
4773985	189591	7146	3	35	58820.65	0.07	0.05	N	O	1996-08-16	1996-07-05	1996-09-09	TAKE BACK RETURN         	RAIL      	beans. fluffily final depe
4773985	42114	4619	4	20	21122.20	0.08	0.04	N	O	1996-07-17	1996-08-08	1996-08-02	TAKE BACK RETURN         	MAIL      	 quickly ironi
4808192	118792	8793	1	26	47080.54	0.05	0.08	N	O	1996-07-26	1996-09-07	1996-08-03	TAKE BACK RETURN         	REG AIR   	inal deposits haggle b
4808192	121572	9109	2	11	17529.27	0.03	0.01	N	O	1996-09-15	1996-09-04	1996-09-25	COLLECT COD              	MAIL      	g, final requests sleep quickly fluffi
4860421	171296	8848	1	12	16407.48	0.06	0.08	N	O	1996-01-14	1995-12-31	1996-01-16	COLLECT COD              	MAIL      	etect furiously about the accounts.
4860421	164298	6815	2	41	55853.89	0.08	0.03	N	O	1995-12-31	1995-12-21	1996-01-16	COLLECT COD              	MAIL      	gside of the foxes. slyly ironic ac
4860421	150892	3408	3	5	9714.45	0.06	0.03	N	O	1995-11-04	1995-12-13	1995-11-11	TAKE BACK RETURN         	SHIP      	thely regular packages across 
4860421	32413	7420	4	4	5381.64	0.06	0.02	N	O	1995-11-08	1995-12-17	1995-11-28	NONE                     	REG AIR   	ly. quickly unusual pinto beans 
4860421	160771	5804	5	39	71439.03	0.08	0.03	N	O	1996-01-21	1995-12-09	1996-02-18	TAKE BACK RETURN         	FOB       	 along the regular, ironic theodolites. c
4860421	63006	525	6	29	28101.00	0.03	0.07	N	O	1995-12-23	1995-11-28	1996-01-12	DELIVER IN PERSON        	AIR       	carefully acc
4860421	140576	577	7	6	9699.42	0.03	0.05	N	O	1995-12-13	1995-12-01	1996-01-10	NONE                     	AIR       	ss the final foxes. fluffily fi
4909280	28910	1413	1	32	58845.12	0.09	0.02	A	F	1993-09-27	1993-11-12	1993-10-04	DELIVER IN PERSON        	SHIP      	ilently regular pinto
4909280	78956	6478	2	37	71593.15	0.03	0.04	R	F	1993-12-14	1993-12-04	1993-12-21	COLLECT COD              	AIR       	 stealthily fluffily unusual accounts. s
4909280	9307	1808	3	17	20677.10	0.09	0.02	R	F	1994-01-09	1993-11-22	1994-02-06	DELIVER IN PERSON        	TRUCK     	blithely final requests ar
4909280	9356	4357	4	6	7592.10	0.06	0.01	A	F	1994-01-05	1993-11-29	1994-01-30	NONE                     	FOB       	 express theodolites lo
4909280	41481	3986	5	40	56899.20	0.06	0.01	A	F	1994-01-08	1993-12-11	1994-02-01	COLLECT COD              	RAIL      	hely final packages. regular 
4953319	17356	9858	1	24	30560.40	0.01	0.03	N	O	1997-04-09	1997-04-01	1997-04-30	TAKE BACK RETURN         	REG AIR   	symptotes wake fluffily at the
4953319	85493	510	2	37	54704.13	0.04	0.06	N	O	1997-05-01	1997-04-02	1997-05-15	DELIVER IN PERSON        	RAIL      	o beans. fina
4953319	65610	5611	3	7	11029.27	0.09	0.05	N	O	1997-04-27	1997-04-14	1997-05-19	DELIVER IN PERSON        	RAIL      	requests shall are 
4953319	101732	9263	4	25	43343.25	0.10	0.05	N	O	1997-04-21	1997-04-27	1997-05-11	COLLECT COD              	REG AIR   	he warhorses. carefully regular asymp
4953319	272	5273	5	41	48063.07	0.08	0.03	N	O	1997-04-13	1997-04-12	1997-04-15	NONE                     	SHIP      	n instructions affix. pendin
4953319	163005	3006	6	12	12816.00	0.07	0.06	N	O	1997-04-16	1997-05-12	1997-05-15	NONE                     	FOB       	carefully regul
4953319	24006	4007	7	24	22320.00	0.04	0.02	N	O	1997-03-13	1997-04-06	1997-03-26	DELIVER IN PERSON        	AIR       	ets. express deposits boo
4976422	38924	1428	1	3	5588.76	0.10	0.04	N	O	1998-07-30	1998-08-17	1998-08-06	COLLECT COD              	RAIL      	ilent instructions. deposits use always re
4976422	185334	7853	2	43	61031.19	0.01	0.07	N	O	1998-09-18	1998-09-29	1998-10-04	NONE                     	AIR       	the excuses haggle along th
4976422	99607	9608	3	8	12852.80	0.09	0.03	N	O	1998-08-30	1998-09-18	1998-09-21	COLLECT COD              	FOB       	ep across the busy instructions. quickly i
4976422	82889	2890	4	28	52412.64	0.02	0.07	N	O	1998-07-14	1998-09-01	1998-07-16	COLLECT COD              	SHIP      	gular dolphins are furiously. ironic in
4976422	107580	7581	5	31	49214.98	0.04	0.06	N	O	1998-08-05	1998-09-22	1998-08-29	DELIVER IN PERSON        	REG AIR   	 blithely ironic accounts? ironic dep
4976422	85062	79	6	29	30364.74	0.10	0.01	N	O	1998-09-27	1998-08-20	1998-10-14	NONE                     	SHIP      	 requests haggle slyly. ironic, iro
4976422	21016	8523	7	24	22488.24	0.10	0.06	N	O	1998-08-10	1998-09-04	1998-09-01	COLLECT COD              	REG AIR   	 wake slyly according to the slyly f
5008352	65778	3297	1	25	43594.25	0.09	0.07	N	O	1995-08-29	1995-09-08	1995-09-19	TAKE BACK RETURN         	RAIL      	ests. regular instructions doubt bli
5008352	122387	9924	2	46	64831.48	0.06	0.00	N	O	1995-07-21	1995-08-24	1995-08-01	NONE                     	MAIL      	ly ironic notornis wake pending re
5008352	145270	299	3	2	2630.54	0.01	0.06	N	O	1995-09-15	1995-08-30	1995-10-07	COLLECT COD              	TRUCK     	ong the furiously regular ins
5008352	143546	1089	4	47	74708.38	0.04	0.03	N	O	1995-10-05	1995-09-20	1995-10-09	TAKE BACK RETURN         	SHIP      	ructions wake slyly
5008352	166152	3701	5	43	52380.45	0.02	0.05	N	O	1995-09-03	1995-09-02	1995-09-15	DELIVER IN PERSON        	FOB       	ic deposits! blithely express de
5008352	112190	4702	6	36	43278.84	0.05	0.03	N	O	1995-07-20	1995-08-27	1995-08-08	DELIVER IN PERSON        	REG AIR   	e slowly against the slyly special pint
5008352	169545	2062	7	26	41978.04	0.05	0.00	N	O	1995-08-29	1995-08-20	1995-09-18	DELIVER IN PERSON        	MAIL      	ts sleep across the ironic, pen
5024647	68668	6187	1	38	62193.08	0.10	0.08	N	O	1996-02-11	1996-01-27	1996-02-20	TAKE BACK RETURN         	SHIP      	ths are care
5024647	92033	7052	2	9	9225.27	0.07	0.05	N	O	1996-03-01	1996-02-09	1996-03-13	NONE                     	AIR       	eposits. ironic, silent ideas could doze b
5024647	189017	4054	3	11	12166.11	0.08	0.00	N	O	1996-01-20	1996-01-20	1996-02-15	NONE                     	MAIL      	deposits. pending, even 
5024647	199103	6661	4	44	52892.40	0.02	0.01	N	O	1996-01-04	1996-03-10	1996-02-01	DELIVER IN PERSON        	REG AIR   	eans against the
5024647	119096	1608	5	11	12265.99	0.02	0.02	N	O	1996-04-13	1996-01-22	1996-04-14	DELIVER IN PERSON        	RAIL      	al packages a
5024647	32205	9715	6	6	6823.20	0.03	0.02	N	O	1996-02-03	1996-02-10	1996-03-04	COLLECT COD              	MAIL      	nic theodolites accordi
5024647	58477	983	7	46	66031.62	0.10	0.01	N	O	1996-01-13	1996-02-03	1996-01-16	NONE                     	RAIL      	ajole furiously evenly brave packages. 
5027840	177886	7887	1	23	45169.24	0.06	0.08	N	O	1998-07-18	1998-07-21	1998-07-25	TAKE BACK RETURN         	RAIL      	- slyly bold instructions haggle care
5027840	119064	1576	2	23	24910.38	0.02	0.06	N	O	1998-07-13	1998-08-20	1998-08-02	COLLECT COD              	AIR       	sublate fluf
5027840	102424	2425	3	18	25675.56	0.02	0.04	N	O	1998-06-20	1998-07-27	1998-07-15	TAKE BACK RETURN         	TRUCK     	. final packages wake quickl
5027840	28829	3834	4	15	26367.30	0.03	0.07	N	O	1998-09-16	1998-06-26	1998-10-16	TAKE BACK RETURN         	SHIP      	final, even ideas across the accounts wa
5027840	123694	6207	5	32	54966.08	0.02	0.01	N	O	1998-09-22	1998-07-18	1998-09-30	TAKE BACK RETURN         	MAIL      	refully busy depe
5027840	110378	379	6	34	47204.58	0.01	0.01	N	O	1998-07-26	1998-07-01	1998-08-14	DELIVER IN PERSON        	FOB       	ts over the carefully final packa
5066818	8144	5645	1	8	8417.12	0.01	0.06	N	O	1996-06-09	1996-07-14	1996-07-09	COLLECT COD              	REG AIR   	ously slyly regular dolphins. regular war
5066818	74073	9088	2	40	41882.80	0.06	0.00	N	O	1996-08-26	1996-07-18	1996-09-20	TAKE BACK RETURN         	SHIP      	fluffily. quickly expr
5066818	141564	1565	3	15	24083.40	0.00	0.03	N	O	1996-09-17	1996-07-11	1996-10-05	DELIVER IN PERSON        	REG AIR   	ets. express, ironic ideas
5066818	49386	6899	4	50	66769.00	0.07	0.02	N	O	1996-08-03	1996-08-20	1996-08-14	DELIVER IN PERSON        	MAIL      	nts are furiously. 
5081639	188854	6409	1	28	54399.80	0.04	0.02	N	O	1996-04-20	1996-02-18	1996-05-06	TAKE BACK RETURN         	REG AIR   	jole after th
5081639	63374	5881	2	34	45470.58	0.06	0.08	N	O	1996-03-31	1996-03-02	1996-04-14	TAKE BACK RETURN         	SHIP      	egular foxes integrate above th
5081639	116944	4478	3	25	49023.50	0.06	0.01	N	O	1996-02-25	1996-03-25	1996-03-26	TAKE BACK RETURN         	SHIP      	uickly from the quickly fina
5081639	76137	1152	4	28	31167.64	0.10	0.04	N	O	1996-01-21	1996-03-21	1996-02-12	NONE                     	RAIL      	 are quickly furiously ironic pi
5081639	20642	3145	5	49	76569.36	0.02	0.01	N	O	1996-05-06	1996-02-25	1996-05-21	NONE                     	TRUCK     	rate quickly across the slyly
5090183	123589	3590	1	14	22576.12	0.05	0.04	N	O	1996-10-08	1996-08-05	1996-10-10	NONE                     	RAIL      	cuses. quickly special requests wake: de
5090183	150056	7602	2	20	22121.00	0.08	0.06	N	O	1996-09-17	1996-07-23	1996-09-22	NONE                     	REG AIR   	osits nag slyly across the sl
5090183	190935	5974	3	18	36466.74	0.08	0.05	N	O	1996-09-02	1996-07-11	1996-09-07	TAKE BACK RETURN         	REG AIR   	nic, express theodolites h
5091364	198182	3221	1	5	6400.90	0.05	0.00	N	O	1997-01-23	1996-12-16	1997-02-06	NONE                     	SHIP      	ounts haggle about the quickly final
5091364	41547	9060	2	23	34236.42	0.06	0.04	N	O	1997-01-03	1997-01-09	1997-02-02	DELIVER IN PERSON        	REG AIR   	 furiously alon
5101668	133603	6117	1	44	72010.40	0.02	0.01	R	F	1995-02-05	1995-01-08	1995-02-09	DELIVER IN PERSON        	TRUCK     	asymptotes-- furiously bold pi
5101668	54088	9099	2	7	7294.56	0.08	0.01	R	F	1994-11-18	1995-01-04	1994-11-23	COLLECT COD              	RAIL      	 sleep. pack
5101668	46206	6207	3	34	39174.80	0.04	0.07	A	F	1994-11-24	1994-12-16	1994-11-29	TAKE BACK RETURN         	TRUCK     	ckly quickly regular requests. slyly regula
5101668	174906	9941	4	46	91121.40	0.08	0.00	R	F	1995-02-15	1995-01-19	1995-03-10	NONE                     	FOB       	gside of the caref
5101668	31597	4101	5	21	32100.39	0.00	0.07	A	F	1994-12-03	1995-01-08	1994-12-13	NONE                     	AIR       	lyly regular, bold ideas. ironic foxes in
5101668	40965	5974	6	28	53366.88	0.06	0.07	A	F	1995-01-24	1994-12-31	1995-01-27	COLLECT COD              	AIR       	onic ideas cajole along the carefully speci
5104326	427	2928	1	13	17256.46	0.01	0.07	R	F	1993-01-08	1993-03-21	1993-01-16	NONE                     	REG AIR   	of the pinto beans. 
5104326	168091	3124	2	10	11590.90	0.00	0.07	A	F	1993-01-19	1993-02-02	1993-01-26	NONE                     	RAIL      	 the regular, final pinto beans. caref
5104326	92168	9696	3	42	48726.72	0.08	0.08	R	F	1993-03-02	1993-03-29	1993-03-07	TAKE BACK RETURN         	MAIL      	 ironic instructions use outsi
5104326	56304	6305	4	36	45370.80	0.04	0.05	A	F	1993-04-18	1993-03-04	1993-05-16	TAKE BACK RETURN         	REG AIR   	uickly pending excuses affix near the 
5106981	100205	2716	1	33	39771.60	0.10	0.05	N	O	1998-10-28	1998-09-21	1998-11-22	NONE                     	MAIL      	ily bold pinto beans are requests. fluffily
5106981	140979	3494	2	37	74738.89	0.00	0.02	N	O	1998-11-02	1998-09-15	1998-11-23	COLLECT COD              	AIR       	osits snooze fluffily betw
5129313	180110	111	1	32	38083.52	0.07	0.05	R	F	1992-12-17	1992-12-18	1993-01-05	NONE                     	AIR       	ites. ruthlessly final accounts boost d
5129313	104113	6624	2	29	32396.19	0.10	0.08	A	F	1992-11-14	1992-12-13	1992-11-19	TAKE BACK RETURN         	TRUCK     	al deposits. final, ironic re
5129313	24360	4361	3	14	17981.04	0.07	0.07	A	F	1992-11-20	1993-01-21	1992-11-23	DELIVER IN PERSON        	FOB       	ic forges in
5129313	16643	4147	4	28	43669.92	0.05	0.02	A	F	1993-03-01	1993-01-06	1993-03-02	COLLECT COD              	AIR       	ccounts sleep fluffily accounts. ide
5133509	196216	6217	1	28	36741.88	0.02	0.03	N	O	1996-08-19	1996-09-07	1996-09-18	DELIVER IN PERSON        	REG AIR   	encies nag quickly regular, special de
5133509	198707	6265	2	35	63199.50	0.06	0.05	N	O	1996-08-12	1996-09-24	1996-08-28	COLLECT COD              	REG AIR   	 haggle furiously acr
5133509	60989	6002	3	12	23399.76	0.05	0.00	N	O	1996-09-28	1996-08-07	1996-10-24	NONE                     	TRUCK     	the, final dependencies sleep. f
5133509	31696	1697	4	33	53713.77	0.07	0.06	N	O	1996-08-16	1996-09-16	1996-08-30	NONE                     	SHIP      	ites use across the fluffily regula
5279264	152696	5212	1	21	36722.49	0.02	0.05	A	F	1993-11-28	1993-11-18	1993-12-04	DELIVER IN PERSON        	SHIP      	l have to print blithely according to t
5279264	154754	7270	2	19	34366.25	0.01	0.01	A	F	1993-09-19	1993-11-20	1993-10-08	COLLECT COD              	MAIL      	regular instructions. ironically final plat
5279264	146857	4400	3	13	24750.05	0.01	0.02	R	F	1993-11-06	1993-10-12	1993-12-06	COLLECT COD              	TRUCK     	ithely unusual sentiments haggle do
5279264	20181	2684	4	23	25327.14	0.02	0.07	R	F	1993-12-17	1993-10-19	1993-12-28	DELIVER IN PERSON        	FOB       	xes wake furiously. bol
5279264	172537	7572	5	20	32190.60	0.08	0.02	R	F	1993-10-08	1993-11-09	1993-10-16	COLLECT COD              	AIR       	e carefully even orbits. silent foxes hag
5279264	29122	4127	6	15	15766.80	0.08	0.05	R	F	1993-11-19	1993-11-17	1993-11-28	COLLECT COD              	REG AIR   	tructions. 
5314916	75250	2772	1	13	15928.25	0.03	0.06	R	F	1995-02-02	1995-03-24	1995-02-26	TAKE BACK RETURN         	RAIL      	ully bold packages. quic
5314916	123235	3236	2	19	23906.37	0.03	0.00	A	F	1995-01-29	1995-03-05	1995-02-02	COLLECT COD              	REG AIR   	telets unwind furiously. bli
5314916	195642	681	3	1	1737.64	0.04	0.03	R	F	1995-03-22	1995-03-19	1995-03-28	TAKE BACK RETURN         	RAIL      	ously even packages alongside of t
5314916	147571	5114	4	12	19422.84	0.05	0.03	A	F	1995-03-03	1995-03-22	1995-03-18	NONE                     	TRUCK     	s. furiously pend
5314916	22006	7011	5	14	12992.00	0.04	0.00	R	F	1995-01-26	1995-04-12	1995-02-07	TAKE BACK RETURN         	REG AIR   	anently bold c
5407235	36216	3726	1	19	21891.99	0.05	0.02	N	O	1998-03-27	1998-01-16	1998-04-02	TAKE BACK RETURN         	TRUCK     	sits. furiously ironic p
5407235	67147	4666	2	18	20054.52	0.04	0.06	N	O	1998-03-19	1998-03-11	1998-04-10	NONE                     	REG AIR   	counts. final excuses mold-- 
5407235	129153	4178	3	34	40193.10	0.06	0.07	N	O	1998-04-11	1998-02-24	1998-05-04	DELIVER IN PERSON        	AIR       	ckages. carefully even realms nag carefu
5407235	62187	9706	4	34	39072.12	0.00	0.04	N	O	1998-04-05	1998-02-22	1998-05-02	TAKE BACK RETURN         	TRUCK     	ke carefully sly
5407235	164363	4364	5	24	34256.64	0.03	0.03	N	O	1998-02-27	1998-02-03	1998-03-24	DELIVER IN PERSON        	REG AIR   	blithely regular packages 
5407235	33971	6475	6	45	85723.65	0.08	0.05	N	O	1997-12-17	1998-02-08	1997-12-22	NONE                     	FOB       	the quickl
5433765	13092	596	1	22	22111.98	0.06	0.07	R	F	1993-07-07	1993-07-06	1993-07-09	TAKE BACK RETURN         	FOB       	al foxes haggle care
5636773	190310	7868	1	20	28006.20	0.05	0.00	R	F	1994-05-10	1994-05-07	1994-05-14	COLLECT COD              	SHIP      	carefully furious
5636773	130076	5103	2	14	15484.98	0.10	0.01	A	F	1994-06-24	1994-05-09	1994-07-18	TAKE BACK RETURN         	MAIL      	eposits haggle carefully
5636773	171784	1785	3	3	5567.34	0.06	0.02	R	F	1994-05-24	1994-05-11	1994-06-17	COLLECT COD              	FOB       	slyly final
5636773	167119	7120	4	32	37955.52	0.04	0.07	R	F	1994-03-18	1994-05-13	1994-03-25	DELIVER IN PERSON        	REG AIR   	l orbits are careful
5636773	159260	6806	5	26	34300.76	0.08	0.00	R	F	1994-05-19	1994-04-26	1994-06-18	COLLECT COD              	RAIL      	y quickly even deposits.
5636773	72848	2849	6	15	27312.60	0.08	0.07	A	F	1994-06-13	1994-04-20	1994-06-21	TAKE BACK RETURN         	MAIL      	refully among the close account
5692738	26219	6220	1	47	53824.87	0.05	0.05	A	F	1992-07-02	1992-09-17	1992-07-11	COLLECT COD              	AIR       	ickly regul
5692738	53497	1013	2	35	50767.15	0.00	0.08	A	F	1992-07-09	1992-08-30	1992-08-03	NONE                     	RAIL      	fully about the slyly even platelets. T
5692738	120435	7972	3	23	33474.89	0.02	0.06	R	F	1992-09-01	1992-08-02	1992-09-27	TAKE BACK RETURN         	TRUCK     	ed, unusual foxes. final, fin
5715522	88879	3896	1	13	24282.31	0.10	0.00	R	F	1993-07-04	1993-08-14	1993-07-14	NONE                     	FOB       	kly regular deposits. furiously final p
5715522	177075	7076	2	5	5760.35	0.09	0.08	A	F	1993-10-04	1993-08-15	1993-10-26	COLLECT COD              	REG AIR   	 unusual requests
5715522	91272	1273	3	6	7579.62	0.08	0.04	A	F	1993-07-18	1993-08-16	1993-08-02	COLLECT COD              	AIR       	. slyly ironic accounts mold. f
5715522	197907	2946	4	3	6014.70	0.07	0.07	R	F	1993-08-23	1993-08-01	1993-09-05	DELIVER IN PERSON        	SHIP      	refully reques
5715522	20751	752	5	39	65198.25	0.06	0.02	R	F	1993-08-26	1993-08-22	1993-09-09	COLLECT COD              	RAIL      	nic ideas cajole slyly
5742502	94995	2523	1	13	25869.87	0.04	0.04	N	O	1998-08-03	1998-05-24	1998-08-12	TAKE BACK RETURN         	MAIL      	carefully even deposits.
5789540	32770	5274	1	37	63002.49	0.04	0.01	A	F	1992-05-15	1992-04-27	1992-05-16	COLLECT COD              	REG AIR   	ly blithely ironic
5789540	155650	5651	2	21	35818.65	0.05	0.08	A	F	1992-06-02	1992-05-07	1992-06-03	DELIVER IN PERSON        	REG AIR   	elets wake across the final, ev
5789540	9350	6851	3	48	60448.80	0.07	0.07	A	F	1992-03-23	1992-06-03	1992-04-20	NONE                     	FOB       	ully. blithely even pinto bean
5789540	132176	7203	4	48	57992.16	0.03	0.08	A	F	1992-06-06	1992-05-08	1992-06-28	COLLECT COD              	MAIL      	nstructions! unusual, unusual deposits are
5789540	76980	6981	5	35	68494.30	0.10	0.02	A	F	1992-06-17	1992-06-03	1992-07-12	COLLECT COD              	AIR       	 even requests s
5791200	122093	4606	1	6	6690.54	0.09	0.07	N	O	1998-07-09	1998-06-13	1998-08-03	COLLECT COD              	SHIP      	kages promise furiously slyl
5805349	161584	1585	1	11	18101.38	0.08	0.07	N	O	1998-01-28	1998-02-27	1998-02-22	NONE                     	MAIL      	n packages. furiously pending dep
5805349	131435	1436	2	50	73321.50	0.07	0.07	N	O	1998-04-16	1998-03-15	1998-05-02	TAKE BACK RETURN         	FOB       	e among the caref
5805349	15596	3100	3	19	28720.21	0.04	0.05	N	O	1998-04-30	1998-03-12	1998-05-05	DELIVER IN PERSON        	TRUCK     	deas. even foxes are furiously. blit
5805349	109441	4462	4	16	23207.04	0.09	0.02	N	O	1998-04-25	1998-03-04	1998-04-29	COLLECT COD              	TRUCK     	ully pending theodolites nag furiously alo
5805349	170542	543	5	29	46763.66	0.08	0.04	N	O	1998-04-10	1998-03-02	1998-05-01	TAKE BACK RETURN         	SHIP      	nt courts. fi
5805349	94540	9559	6	29	44501.66	0.00	0.06	N	O	1998-03-16	1998-03-16	1998-03-25	DELIVER IN PERSON        	FOB       	ress foxes use against the carefully ir
5805349	118560	8561	7	14	22099.84	0.08	0.08	N	O	1998-02-12	1998-02-23	1998-02-23	DELIVER IN PERSON        	REG AIR   	s. blithely express accounts snooze s
5876613	169763	7312	1	19	34822.44	0.04	0.08	A	F	1992-05-22	1992-06-18	1992-06-05	COLLECT COD              	FOB       	arefully even deposits. express account
5876613	198962	8963	2	16	32975.36	0.01	0.04	R	F	1992-06-11	1992-06-30	1992-06-24	NONE                     	FOB       	ts. slyly final accounts na
5876613	151943	4459	3	50	99747.00	0.05	0.03	R	F	1992-06-04	1992-05-15	1992-06-29	TAKE BACK RETURN         	REG AIR   	 according to the caref
5876613	96956	9466	4	30	58588.50	0.08	0.06	A	F	1992-06-14	1992-05-29	1992-06-24	COLLECT COD              	MAIL      	y about the pending, even packages. 
5876613	130573	3087	5	12	19242.84	0.06	0.08	A	F	1992-05-25	1992-06-24	1992-06-05	TAKE BACK RETURN         	MAIL      	gle blithely above the ca
5876613	131251	6278	6	6	7693.50	0.00	0.03	A	F	1992-08-02	1992-06-10	1992-08-20	NONE                     	FOB       	 use around
5885190	6369	8870	1	33	42086.88	0.01	0.01	A	F	1995-02-24	1995-05-08	1995-03-24	TAKE BACK RETURN         	SHIP      	en deposits. sen
5885190	26999	9502	2	48	92447.52	0.10	0.00	A	F	1995-02-28	1995-04-30	1995-03-03	TAKE BACK RETURN         	TRUCK     	nstructions. 
5897442	83448	3449	1	45	64414.80	0.02	0.07	N	O	1997-01-24	1997-01-03	1997-02-19	NONE                     	MAIL      	 the ironic, regular 
5897442	97623	7624	2	21	34033.02	0.10	0.03	N	O	1997-02-08	1997-01-25	1997-02-27	DELIVER IN PERSON        	FOB       	equests. carefully express deposit
5957861	69442	6961	1	37	52223.28	0.07	0.04	N	O	1997-02-19	1997-03-26	1997-03-05	TAKE BACK RETURN         	FOB       	ns nag against the carefull
5957861	100169	2680	2	21	24552.36	0.02	0.02	N	O	1997-01-17	1997-02-18	1997-02-14	TAKE BACK RETURN         	SHIP      	egular depe
5957861	136542	1569	3	40	63141.60	0.00	0.08	N	O	1997-02-18	1997-02-24	1997-03-17	DELIVER IN PERSON        	AIR       	symptotes. carefully final 
5957861	53555	6061	4	42	63359.10	0.01	0.04	N	O	1997-01-09	1997-02-18	1997-01-27	NONE                     	RAIL      	ic packages affix. enticing, exp
5957861	83784	6293	5	16	28284.48	0.03	0.05	N	O	1997-03-24	1997-02-10	1997-04-20	COLLECT COD              	RAIL      	 solve quickly ironic atta
5975365	158847	3878	1	4	7623.36	0.09	0.03	A	F	1992-06-08	1992-08-22	1992-06-20	DELIVER IN PERSON        	RAIL      	-- quickly ironic sheaves must have t
5975365	138884	8885	2	6	11537.28	0.06	0.04	R	F	1992-07-30	1992-07-29	1992-08-21	NONE                     	MAIL      	ously blithely express d
5975365	8265	3266	3	12	14079.12	0.03	0.00	R	F	1992-07-08	1992-07-05	1992-07-26	NONE                     	SHIP      	s use across the carefully bold pack
5975365	94555	7065	4	48	74378.40	0.01	0.03	A	F	1992-06-09	1992-08-24	1992-06-15	COLLECT COD              	RAIL      	s use carefully ironic pinto beans: expre
5976837	72516	2517	1	21	31258.71	0.06	0.08	N	O	1998-06-16	1998-07-05	1998-07-02	DELIVER IN PERSON        	TRUCK     	the quickly regular inst
5976837	172866	2867	2	28	54288.08	0.00	0.01	N	O	1998-09-17	1998-08-09	1998-09-25	NONE                     	MAIL      	onic packages. slyl
5976837	150681	682	3	16	27706.88	0.04	0.01	N	O	1998-07-29	1998-07-28	1998-08-06	NONE                     	MAIL      	 nag slyly quickly regular pinto beans.
5976837	94251	6761	4	42	52300.50	0.09	0.06	N	O	1998-08-30	1998-08-19	1998-09-21	NONE                     	AIR       	kly even asympt
5976837	68124	8125	5	44	48053.28	0.03	0.01	N	O	1998-09-05	1998-07-17	1998-09-16	NONE                     	REG AIR   	old foxes. fluffily bold excuses haggle 
5976837	146617	6618	6	41	68208.01	0.03	0.02	N	O	1998-07-31	1998-07-09	1998-08-26	NONE                     	SHIP      	ackages sleep ag
5980134	38057	3064	1	47	46767.35	0.09	0.04	A	F	1995-04-03	1995-04-02	1995-04-13	COLLECT COD              	MAIL      	y. slow courts cajole
5980134	14798	4799	2	21	35968.59	0.08	0.08	A	F	1995-05-07	1995-03-31	1995-05-11	DELIVER IN PERSON        	FOB       	accounts detect furiou
5980134	95129	5130	3	25	28103.00	0.02	0.00	A	F	1995-05-11	1995-04-14	1995-06-05	COLLECT COD              	FOB       	 carefully special dep
5980134	22273	7278	4	12	14343.24	0.01	0.07	R	F	1995-02-11	1995-04-16	1995-02-23	COLLECT COD              	FOB       	ual, silent o
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
43879	31	F	86826.97	1993-08-13	2-HIGH         	Clerk#000000092	0	enly regular instructions	1993
52263	19	F	56020.27	1994-05-08	4-NOT SPECIFIED	Clerk#000000080	0	lly ironic, even packages. bold foxes wake carefully. bold requests hag	1994
70819	34	O	225707.50	1996-11-20	4-NOT SPECIFIED	Clerk#000000415	0	wake enticingly about the ironic, s	1996
90019	19	F	83402.82	1993-10-28	5-LOW          	Clerk#000000057	0	uickly even requests hang above the slyly regular platelets. ironic packages	1993
100064	19	O	68035.60	1996-04-10	2-HIGH         	Clerk#000000604	0	ins nag at the special foxes. 	1996
120160	20	P	234161.48	1995-04-09	3-MEDIUM       	Clerk#000000105	0	unts besides the ironic deposits are blithely after	1995
135943	7	F	314307.46	1993-06-22	4-NOT SPECIFIED	Clerk#000000685	0	ies. blithely pending deposits 	1993
193030	34	F	322439.88	1992-06-09	4-NOT SPECIFIED	Clerk#000000839	0	 ideas. carefully regular packages promise special, final instruct	1992
212870	11	O	171711.48	1996-10-30	5-LOW          	Clerk#000000637	0	 fluffily final dolphins. unusual pinto beans use blithely regular ideas	1996
226818	31	O	117221.98	1995-05-13	3-MEDIUM       	Clerk#000000909	0	al, regular instructions. quickly final packages wake q	1995
235779	31	F	220513.94	1994-04-29	5-LOW          	Clerk#000000766	0	bove the ideas. furiously express excuses about the slyly unu	1994
306439	11	O	281477.00	1997-05-17	2-HIGH         	Clerk#000000794	0	 of the bold packages. final waters detect sly	1997
360067	20	F	223207.90	1992-12-07	1-URGENT       	Clerk#000000455	0	ds. notornis wake. slyly silent packages above the furiousl	1992
430243	2	F	33082.83	1994-12-24	4-NOT SPECIFIED	Clerk#000000121	0	n braids among the final packages cajole pendin	1994
454791	1	F	74602.81	1992-04-19	1-URGENT       	Clerk#000000815	0	uriously ironic deposits wake across the s	1992
529350	31	O	41526.14	1996-03-03	5-LOW          	Clerk#000000393	0	pecial theodolites. packages are after the busy requests. fluffily special p	1996
554115	34	F	229562.20	1992-10-11	1-URGENT       	Clerk#000000506	0	y silent accounts. final requests are unusual, regular	1992
576263	34	F	278030.48	1994-03-02	2-HIGH         	Clerk#000000899	0	ckly express deposits. fluffily ironic ac	1994
579908	1	O	54048.26	1996-12-09	5-LOW          	Clerk#000000783	0	 asymptotes. regular requests boost blithely. pending gifts according to the	1996
601794	31	O	350102.45	1996-12-10	5-LOW          	Clerk#000000126	0	ckly ironic accounts wake through the quickly e	1996
639490	19	O	241244.61	1995-10-27	5-LOW          	Clerk#000000288	0	platelets cajole fluffily ironic deposi	1995
641253	31	O	142954.89	1995-05-24	2-HIGH         	Clerk#000000323	0	. brave ideas haggl	1995
653063	20	F	56303.53	1995-03-11	5-LOW          	Clerk#000000859	0	gular theodolites integrate furiously	1995
655010	20	P	127748.12	1995-03-10	3-MEDIUM       	Clerk#000000291	0	ing to the quickly final packages. furiously eve	1995
729504	44	F	115240.12	1993-10-08	3-MEDIUM       	Clerk#000000404	0	 detect. carefully regular theodolites wake among the fi	1993
787105	31	F	122926.80	1994-05-28	4-NOT SPECIFIED	Clerk#000000526	0	pinto beans sleep. qu	1994
792900	19	O	106967.86	1995-07-11	5-LOW          	Clerk#000000243	0	ronic accounts haggle slyly according to th	1995
823525	34	O	113922.14	1997-08-22	4-NOT SPECIFIED	Clerk#000000748	0	 beans cajole above the blithely express deposits. ca	1997
835173	11	F	198998.83	1993-08-18	2-HIGH         	Clerk#000000031	0	requests. packages across the blithely ironic instructions haggle carefully a	1993
849831	43	F	221736.86	1993-11-13	3-MEDIUM       	Clerk#000000251	0	ts. silent, even deposits haggle caref	1993
943270	43	F	222382.39	1993-08-15	4-NOT SPECIFIED	Clerk#000000762	0	sits wake furiously fluffily regular foxes. furiously fina	1993
1008069	34	O	327731.43	1995-09-30	2-HIGH         	Clerk#000000335	0	accounts haggle blithely across the special accounts. regu	1995
1071617	2	P	221397.35	1995-03-10	2-HIGH         	Clerk#000000408	0	al, unusual packages. furiously final requests use quickly. f	1995
1072832	44	F	187685.34	1993-12-31	4-NOT SPECIFIED	Clerk#000000199	0	s deposits play carefully regular mult	1993
1073670	8	F	76193.83	1994-05-24	4-NOT SPECIFIED	Clerk#000000710	0	ly alongside of the slyly ironic theodolites. accoun	1994
1074979	20	F	75719.72	1992-04-18	3-MEDIUM       	Clerk#000000323	0	the quickly pending requests. blithely iron	1992
1115366	34	O	215578.16	1996-07-23	1-URGENT       	Clerk#000000241	0	 foxes. quickly unusual tithes along the idly special dolphin	1996
1176518	43	F	183152.11	1994-09-06	1-URGENT       	Clerk#000000890	0	counts. requests cajole silently furiously special waters. unusual, silent pa	1994
1186787	32	F	110784.85	1995-02-22	3-MEDIUM       	Clerk#000000524	0	ilent packages. slyly ironic requ	1995
1201223	8	O	138984.74	1996-01-13	5-LOW          	Clerk#000000287	0	st the carefully final foxes wake carefully according to the	1996
1243748	31	O	8956.25	1996-03-28	3-MEDIUM       	Clerk#000000880	0	onic pinto beans cajole. blithely regular deposits boos	1996
1252005	34	F	287241.12	1992-01-14	4-NOT SPECIFIED	Clerk#000000188	0	p blithely instead of the regular,	1992
1358113	35	F	106502.09	1992-02-13	2-HIGH         	Clerk#000000048	0	s haggle slyly pending waters. ideas wake never furiously regular accounts. 	1992
1374019	2	F	167016.61	1992-04-05	1-URGENT       	Clerk#000000440	0	lithe packages. acco	1992
1406820	11	O	201295.40	1996-02-24	5-LOW          	Clerk#000000763	0	dolites. slyly express asymptotes affix slyly final ideas. account	1996
1428645	19	F	132982.81	1992-08-30	3-MEDIUM       	Clerk#000000175	0	ans integrate slyly unusual requests. furiously bold packages wake carefully 	1992
1485505	7	O	286525.11	1998-07-24	1-URGENT       	Clerk#000000228	0	posits. final packages wake carefully express ideas. carefully bold deposits a	1998
1490341	44	O	143570.09	1995-11-01	5-LOW          	Clerk#000000434	0	ounts from the blithely pendi	1995
1490849	44	F	204317.57	1993-06-27	3-MEDIUM       	Clerk#000000888	0	ions across the instructions sleep blithely beside the fi	1993
1495106	26	O	163530.39	1997-01-09	5-LOW          	Clerk#000000499	0	encies nag furiously. quickly pending accounts around t	1997
1526534	31	P	164419.65	1995-06-01	2-HIGH         	Clerk#000000877	0	 bold packages. sil	1995
1543201	19	O	229349.02	1998-06-16	3-MEDIUM       	Clerk#000000018	0	hrash. quickly regular requests along the 	1998
1546274	44	F	326139.34	1994-12-08	4-NOT SPECIFIED	Clerk#000000908	0	ilent requests use. special accounts sleep quickly. theodolit	1994
1576097	32	O	26754.84	1998-05-20	1-URGENT       	Clerk#000000854	0	l accounts use furiously on the expres	1998
1593344	44	F	87426.56	1994-01-03	1-URGENT       	Clerk#000000371	0	tipliers. express requests sleep blith	1994
1693984	34	O	288559.00	1996-03-20	2-HIGH         	Clerk#000000767	0	ing, special dolphins. slyly regular notornis haggle 	1996
1709703	19	O	239981.68	1995-06-16	1-URGENT       	Clerk#000000976	0	eposits. carefully ironic instructions nag slyly	1995
1763205	2	F	16495.33	1994-08-28	1-URGENT       	Clerk#000000450	0	ven requests. blithely express excuses nag or	1994
1783749	43	O	208696.27	1998-07-22	4-NOT SPECIFIED	Clerk#000000555	0	ously ironic depths are quickly even packages. expr	1998
1823424	34	F	231383.80	1992-01-10	5-LOW          	Clerk#000000310	0	ly ironic pinto beans at the furiously regular ideas serve ironic deposits. 	1992
1842406	2	O	174291.41	1996-08-05	1-URGENT       	Clerk#000000521	0	s. excuses lose. pen	1996
1859778	8	O	263567.37	1998-02-04	3-MEDIUM       	Clerk#000000731	0	counts boost slyly. express pinto beans use. furious	1998
1868608	19	O	186711.17	1998-03-02	4-NOT SPECIFIED	Clerk#000000613	0	side of the furiously pending accounts. fluffily pending ideas bo	1998
1894087	7	F	36515.55	1994-02-20	1-URGENT       	Clerk#000000343	0	 bold, even request	1994
1895491	31	F	194077.51	1993-02-17	1-URGENT       	Clerk#000000113	0	regularly final depths.	1993
1925447	8	O	134464.25	1997-02-19	4-NOT SPECIFIED	Clerk#000000348	0	nts snooze furiously according to the carefully bold ideas. sometimes regular	1997
1959075	7	F	192318.07	1994-12-23	1-URGENT       	Clerk#000000590	0	long the regular instructions. bli	1994
1959365	34	O	144987.81	1998-05-01	5-LOW          	Clerk#000000583	0	 the carefully pending packages. fluffily	1998
2014848	7	O	181378.37	1997-01-19	5-LOW          	Clerk#000000333	0	lets use pending accounts. qu	1997
2022438	26	O	159744.53	1997-07-11	5-LOW          	Clerk#000000748	0	into beans. slowly silent requests sleep furiously packages? ide	1997
2046658	34	F	204799.96	1992-05-31	2-HIGH         	Clerk#000000882	0	g blithely after the ironic requests. pending braids according to	1992
2073893	11	F	184662.35	1993-11-20	1-URGENT       	Clerk#000000701	0	eodolites are furiously along the sl	1993
2096544	7	F	185203.14	1992-04-17	1-URGENT       	Clerk#000000964	0	ts. final, silent theodoli	1992
2125318	31	O	8186.45	1995-04-13	5-LOW          	Clerk#000000278	0	ake furiously blithely express grouches. furiously 	1995
2129698	26	O	190369.06	1995-07-01	3-MEDIUM       	Clerk#000000902	0	 beans. silent platelets sublate ironic deposits. furiously pending	1995
2164326	20	O	303324.90	1996-05-14	5-LOW          	Clerk#000000994	0	ss the blithely stealthy packages are fluffily express 	1996
2177348	34	F	47214.55	1993-12-19	3-MEDIUM       	Clerk#000000252	0	ep furiously for the courts. silent, ironi	1993
2205862	20	O	238348.59	1998-01-02	1-URGENT       	Clerk#000000168	0	riously after the even grouches. ironic 	1998
2264739	31	O	280337.12	1998-05-23	1-URGENT       	Clerk#000000494	0	ths. ironic asymptotes affix carefully	1998
2290245	11	O	251492.10	1996-12-07	3-MEDIUM       	Clerk#000000959	0	cajole final deposits. pending deposits cajole s	1996
2332928	31	F	57884.71	1994-10-06	4-NOT SPECIFIED	Clerk#000000849	0	ely silent dependencies wake among the 	1994
2415204	8	F	184908.92	1994-11-03	5-LOW          	Clerk#000000368	0	inal requests are final, bold dolph	1994
2420194	11	O	83996.01	1996-04-17	1-URGENT       	Clerk#000000855	0	lithely special packages. quickly	1996
2462560	31	O	68940.31	1996-04-14	4-NOT SPECIFIED	Clerk#000000126	0	gle against the close requests. blithely bold requests a	1996
2514754	19	F	96448.82	1994-04-07	4-NOT SPECIFIED	Clerk#000000028	0	e slyly even hockey play	1994
2541764	43	F	21765.41	1993-02-08	2-HIGH         	Clerk#000000534	0	ently final accounts. special, unusu	1993
2755046	43	F	89296.11	1993-04-20	5-LOW          	Clerk#000000925	0	kages. carefully express pinto beans after th	1993
2799425	19	O	83888.78	1995-11-14	1-URGENT       	Clerk#000000421	0	as. fluffily regular accounts after the carefully regula	1995
2842599	35	F	302954.69	1994-08-18	3-MEDIUM       	Clerk#000000957	0	 bold deposits haggle.	1994
2872675	34	O	321568.67	1997-02-27	4-NOT SPECIFIED	Clerk#000000649	0	e express accounts sleep fl	1997
2923650	31	F	168295.93	1994-02-14	5-LOW          	Clerk#000000065	0	ld even accounts. quickly final ideas cajole carefully among the pending, i	1994
2943521	7	O	31698.35	1998-02-18	1-URGENT       	Clerk#000000675	0	kly against the qui	1998
2953057	19	O	97549.03	1996-04-21	2-HIGH         	Clerk#000000284	0	he deposits cajole carefully according to the furiously	1996
2992930	2	F	103297.68	1994-05-21	4-NOT SPECIFIED	Clerk#000000913	0	owly slyly final platelets. ironic instructions wake carefully unusual plate	1994
3038880	7	O	327615.97	1995-09-06	2-HIGH         	Clerk#000000458	0	y careful ideas. final, regular theodolites haggle. quic	1995
3082272	43	O	64099.17	1996-11-21	2-HIGH         	Clerk#000000498	0	ccording to the deposits	1996
3154370	11	F	69607.13	1993-06-02	5-LOW          	Clerk#000000020	0	y busy requests wake quickly t	1993
3178627	19	O	118482.02	1996-02-27	2-HIGH         	Clerk#000000340	0	s cajole at the carefully unusual p	1996
3211909	7	F	187396.96	1993-06-29	2-HIGH         	Clerk#000000918	0	 slyly according to the even theodolites. slyly ironic requests nag carefu	1993
3247714	26	F	135113.24	1993-08-17	3-MEDIUM       	Clerk#000000098	0	ly ironic accounts detect alongs	1993
3318789	7	F	249621.82	1992-04-30	3-MEDIUM       	Clerk#000000331	0	ly blithely regular deposits. furiously regular accounts s	1992
3330881	35	F	221241.42	1995-02-08	2-HIGH         	Clerk#000000112	0	aggle furiously furi	1995
3358691	43	F	401474.91	1993-11-25	2-HIGH         	Clerk#000000820	0	ly. enticingly bold req	1993
3419044	19	O	240740.53	1998-02-03	3-MEDIUM       	Clerk#000000191	0	t the slyly silent requests.	1998
3431909	7	O	151988.10	1997-02-04	2-HIGH         	Clerk#000000744	0	ose carefully. slyly unusual ins	1997
3478503	34	F	28787.52	1992-01-20	1-URGENT       	Clerk#000000404	0	nto beans. even accounts 	1992
3541094	20	O	4649.68	1998-04-15	4-NOT SPECIFIED	Clerk#000000014	0	 special requests integrate accounts. carefully even requ	1998
3692388	8	O	27557.56	1997-02-11	4-NOT SPECIFIED	Clerk#000000060	0	s packages are slyly around the regula	1997
3692615	32	O	160985.69	1995-10-06	3-MEDIUM       	Clerk#000000285	0	refully unusual pinto bea	1995
3723172	31	O	121890.06	1995-05-16	2-HIGH         	Clerk#000000787	0	 against the silent, final requests? ca	1995
3783492	11	O	198540.63	1996-02-14	2-HIGH         	Clerk#000000752	0	 unusual platelets use. carefully final instructions sleep fluffily e	1996
3807267	34	O	88194.69	1995-08-30	4-NOT SPECIFIED	Clerk#000000550	0	e since the slyly bold warthogs. 	1995
3860257	19	O	275265.60	1998-03-21	3-MEDIUM       	Clerk#000000233	0	longside of the carefully regular instructions. blithely pend	1998
3864070	35	F	41313.52	1993-06-02	5-LOW          	Clerk#000000904	0	ckages cajole. regular deposit	1993
3868359	1	F	123076.84	1992-08-22	5-LOW          	Clerk#000000536	0	. furiously bold asymptotes are instructions. quickly ironic dep	1992
3940897	34	F	3760.03	1992-03-26	2-HIGH         	Clerk#000000468	0	. dogged theodolites w	1992
3947046	19	F	241257.99	1993-07-13	4-NOT SPECIFIED	Clerk#000000232	0	t blithely. special foxes haggle after the blithely dogged frets. furio	1993
3955908	44	O	187244.89	1995-06-23	2-HIGH         	Clerk#000000176	0	express requests cajole	1995
3986496	2	O	312692.22	1997-02-22	1-URGENT       	Clerk#000000768	0	. theodolites nag fluffily above the blithely iron	1997
3994567	34	O	109593.49	1997-01-30	4-NOT SPECIFIED	Clerk#000000127	0	ves. quickly special requests haggle closely after the requests. packages 	1997
3995111	7	F	140088.51	1994-02-13	3-MEDIUM       	Clerk#000000724	0	ts. slyly ironic requests nag blithely. instruct	1994
4099329	20	F	118229.84	1992-06-05	1-URGENT       	Clerk#000000436	0	carefully according to th	1992
4128289	35	O	57306.40	1998-01-17	4-NOT SPECIFIED	Clerk#000000628	0	quests cajole careful	1998
4163013	7	F	80777.53	1993-11-14	3-MEDIUM       	Clerk#000000392	0	t slyly. furiously silent packag	1993
4204384	11	F	232311.12	1993-09-27	4-NOT SPECIFIED	Clerk#000000205	0	c packages. blithely even dependencies nag carefully about the furiously pendi	1993
4208006	34	O	161049.61	1997-02-07	4-NOT SPECIFIED	Clerk#000000663	0	quickly final instructions abo	1997
4208674	8	P	158327.78	1995-05-02	3-MEDIUM       	Clerk#000000657	0	riously-- final theodolites int	1995
4225824	8	O	110757.73	1995-07-21	4-NOT SPECIFIED	Clerk#000000060	0	furiously final packages print slyly express accounts. r	1995
4254181	32	O	298849.22	1997-03-08	1-URGENT       	Clerk#000000431	0	 final theodolites boost slyly bold accounts. slyly ironic braids boost 	1997
4273923	1	O	95911.01	1997-03-23	3-MEDIUM       	Clerk#000000381	0	ep fluffily along the carefull	1997
4303206	20	F	34276.52	1994-12-07	2-HIGH         	Clerk#000000704	0	ies use against the special, pending 	1994
4328998	7	F	322431.85	1992-03-25	4-NOT SPECIFIED	Clerk#000000521	0	nto beans according to the regular accounts cajole per	1992
4378720	19	O	101293.88	1997-11-08	4-NOT SPECIFIED	Clerk#000000169	0	al instructions haggl	1997
4383265	32	F	239533.87	1995-03-25	3-MEDIUM       	Clerk#000000090	0	ts detect furiously express package	1995
4391456	26	F	242204.96	1992-04-09	5-LOW          	Clerk#000000468	0	egular instructions. s	1992
4395841	19	O	302071.17	1998-01-24	5-LOW          	Clerk#000000180	0	al requests. carefully f	1998
4402819	34	O	92780.82	1995-11-06	1-URGENT       	Clerk#000000383	0	ffix quickly along the pending ideas. slyly express theodolites cajole among 	1995
4404676	31	F	71921.03	1992-01-20	3-MEDIUM       	Clerk#000000743	0	ng to the pinto beans. ironic deposits boost quickly across the final, bold	1992
4411653	32	F	78496.35	1992-01-13	4-NOT SPECIFIED	Clerk#000000822	0	s requests nag furiously according to the regular dino	1992
4468801	32	O	257726.31	1996-12-11	1-URGENT       	Clerk#000000244	0	ar requests. slyly unusual courts doubt furiously against the fluffily 	1996
4474501	43	O	128469.54	1995-09-13	3-MEDIUM       	Clerk#000000468	0	ithely fluffily express requests. slyly final dependenc	1995
4550145	8	O	140300.54	1995-06-13	3-MEDIUM       	Clerk#000000554	0	ly fluffy requests. frets affix courts. packages play above the carefully fin	1995
4563873	35	O	38705.05	1997-02-26	2-HIGH         	Clerk#000000081	0	 packages haggle. carefully regular requests haggle slyly except the blithel	1997
4569348	11	O	261496.99	1998-02-19	4-NOT SPECIFIED	Clerk#000000299	0	s are blithely slyly final accounts. blithely idle decoys sl	1998
4587526	43	O	112836.45	1997-05-20	1-URGENT       	Clerk#000000438	0	y pending packages. reque	1997
4625152	34	F	233948.20	1993-09-26	3-MEDIUM       	Clerk#000000108	0	ions are against the ironic, ironic a	1993
4659813	8	F	7459.52	1992-07-26	4-NOT SPECIFIED	Clerk#000000133	0	ely regular accounts haggle carefully regular pains. ironi	1992
4683938	35	O	63168.76	1996-06-05	2-HIGH         	Clerk#000000824	0	ses use furiously above the furiously ironic deposit	1996
4692995	31	F	209660.14	1994-04-06	3-MEDIUM       	Clerk#000000571	0	nts are carefully careful	1994
4702695	43	F	221002.75	1994-07-31	4-NOT SPECIFIED	Clerk#000000759	0	slyly even frays sleep about the always bold o	1994
4773985	32	O	137908.44	1996-06-03	4-NOT SPECIFIED	Clerk#000000275	0	special deposits above the slyly iron	1996
4808192	1	O	65478.05	1996-06-29	2-HIGH         	Clerk#000000473	0	eposits use against the express accounts. carefully regular ideas sleep blithe	1996
4860421	7	O	190889.91	1995-10-24	5-LOW          	Clerk#000000572	0	 express pinto beans. requests believe slyly ex	1995
4909280	31	F	207263.69	1993-09-24	2-HIGH         	Clerk#000000724	0	arefully regular requests print. regular, express instructions affi	1993
4953319	35	O	218361.47	1997-03-02	3-MEDIUM       	Clerk#000000739	0	ickly platelets. furiously regular 	1997
4976422	19	O	236024.67	1998-07-06	2-HIGH         	Clerk#000000875	0	r deposits detect instead of the even, final asymptotes.	1998
5008352	34	O	316148.36	1995-07-14	5-LOW          	Clerk#000000536	0	ctions. fluffily ex	1995
5024647	44	O	212040.53	1995-12-18	3-MEDIUM       	Clerk#000000044	0	believe bravely against the	1995
5027840	43	O	226873.09	1998-05-27	1-URGENT       	Clerk#000000750	0	 alongside of the enticing theodolite	1998
5066818	20	O	136345.71	1996-05-22	5-LOW          	Clerk#000000034	0	 quiet deposits sleep among the furiously express instructions. epit	1996
5081639	31	O	250934.14	1996-01-19	5-LOW          	Clerk#000000607	0	es haggle blithely 	1996
5090183	7	O	79104.46	1996-06-10	2-HIGH         	Clerk#000000046	0	as. furiously bold requests according to the quickly ironic packages c	1996
5091364	8	O	39550.36	1996-11-12	5-LOW          	Clerk#000000894	0	mptotes after the slyly final asymptotes nag quickly before the carefully ex	1996
5101668	44	F	290149.81	1994-11-12	3-MEDIUM       	Clerk#000000972	0	ly. deposits detect furi	1994
5104326	35	F	124830.63	1993-01-02	4-NOT SPECIFIED	Clerk#000000595	0	s boost. pending foxes detect carefully ir	1993
5106981	19	O	113817.82	1998-07-31	4-NOT SPECIFIED	Clerk#000000922	0	sts mold. bold, regular packages mold blithely. unusual pack	1998
5129313	43	F	128886.70	1992-11-02	1-URGENT       	Clerk#000000666	0	g, final theodolites. furiously even grouches cajole c	1992
5133509	1	O	174645.94	1996-07-01	1-URGENT       	Clerk#000000463	0	al accounts could have to cajole furiously even	1996
5279264	31	F	169139.23	1993-08-29	1-URGENT       	Clerk#000000054	0	ts are carefully regular, final deposits.	1993
5314916	26	F	72762.32	1995-01-20	2-HIGH         	Clerk#000000995	0	g to the ironic instructions. regular dolphins play careful	1995
5407235	26	O	239716.83	1997-12-16	4-NOT SPECIFIED	Clerk#000000676	0	te ironically about the carefully ironic requests. slyly b	1997
5433765	34	F	22240.22	1993-05-06	3-MEDIUM       	Clerk#000000353	0	gainst the regular instructions. silent, ironic accounts cajole quickly	1993
5636773	20	F	143450.79	1994-03-06	5-LOW          	Clerk#000000839	0	express courts according to the carefully regular platel	1994
5692738	11	F	143292.53	1992-06-20	4-NOT SPECIFIED	Clerk#000000113	0	ve the blithely ironic foxes. express requests according 	1992
5715522	44	F	103264.80	1993-06-19	4-NOT SPECIFIED	Clerk#000000352	0	ickly special orbits cajole quickly. fluffily regular plat	1993
5742502	32	O	25828.47	1998-04-20	3-MEDIUM       	Clerk#000000471	0	ully above the furiously unusual dolphins. quickly even c	1998
5789540	43	F	281620.06	1992-03-16	5-LOW          	Clerk#000000954	0	fully quickly express sentiments. final, final deposits ac	1992
5791200	43	O	6514.57	1998-04-30	1-URGENT       	Clerk#000000328	0	thely regular packages. furiously final	1998
5805349	8	O	255145.52	1998-01-01	1-URGENT       	Clerk#000000956	0	ites doze special, express deposits. 	1998
5876613	19	F	252252.87	1992-04-09	1-URGENT       	Clerk#000000851	0	s affix quickly across the unusual, ironic packages. slyly final packages b	1992
5885190	11	F	125285.43	1995-02-16	1-URGENT       	Clerk#000000865	0	side of the slyly ruthless deposits. quickly regular instructions wake	1995
5897442	20	O	99093.95	1996-11-21	5-LOW          	Clerk#000000797	0	ld pinto beans wake about the silent deposits. bol	1996
5957861	43	O	237288.05	1996-12-28	5-LOW          	Clerk#000000068	0	instructions boost blithely after the deposits. even accounts outside the even	1996
5975365	19	F	107924.58	1992-05-29	3-MEDIUM       	Clerk#000000854	0	egular, bold pearls according t	1992
5976837	20	O	278441.20	1998-05-27	2-HIGH         	Clerk#000000095	0	ites. slyly express request	1998
5980134	43	F	122733.71	1995-01-27	4-NOT SPECIFIED	Clerk#000000479	0	 requests boost furiously. furiousl	1995
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
10	linen pink saddle puff powder	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	44	LG CAN    	910.01	ithely final deposit
13	ghost olive orange rosy thistle	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED NICKEL	1	JUMBO PACK	913.01	osits.
15	blanched honeydew sky turquoise medium	Manufacturer#1           	Brand#15  	LARGE ANODIZED BRASS	45	LG CASE   	915.01	usual ac
16	deep sky turquoise drab peach	Manufacturer#3           	Brand#32  	PROMO PLATED TIN	2	MED PACK  	916.01	unts a
17	indian navy coral pink deep	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED STEEL	16	LG BOX    	917.01	 regular accounts
18	turquoise indian lemon lavender misty	Manufacturer#1           	Brand#11  	SMALL BURNISHED STEEL	42	JUMBO PACK	918.01	s cajole slyly a
24	seashell coral metallic midnight floral	Manufacturer#5           	Brand#52  	MEDIUM PLATED STEEL	20	MED CASE  	924.02	 final the
25	aquamarine steel firebrick light turquoise	Manufacturer#5           	Brand#55  	STANDARD BRUSHED COPPER	3	JUMBO BAG 	925.02	requests wake
26	beige frosted moccasin chocolate snow	Manufacturer#3           	Brand#32  	SMALL BRUSHED STEEL	32	SM CASE   	926.02	 instructions i
29	lemon sky grey salmon orchid	Manufacturer#3           	Brand#33  	PROMO PLATED COPPER	7	LG DRUM   	929.02	 carefully fluffi
30	cream misty steel spring medium	Manufacturer#4           	Brand#42  	PROMO ANODIZED TIN	17	LG BOX    	930.03	carefully bus
31	slate seashell steel medium moccasin	Manufacturer#5           	Brand#53  	STANDARD BRUSHED TIN	10	LG BAG    	931.03	uriously s
34	khaki steel rose ghost salmon	Manufacturer#1           	Brand#13  	LARGE BRUSHED STEEL	8	JUMBO BOX 	934.03	riously ironic
35	green blush tomato burlywood seashell	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED BRASS	14	JUMBO PACK	935.03	e carefully furi
39	rose medium floral salmon powder	Manufacturer#5           	Brand#53  	SMALL POLISHED TIN	43	JUMBO JAR 	939.03	se slowly above the fl
40	lemon midnight metallic sienna steel	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED COPPER	27	SM CASE   	940.04	! blithely specia
41	burlywood goldenrod pink peru sienna	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED TIN	7	WRAP JAR  	941.04	uriously. furiously cl
45	lawn peru ghost khaki maroon	Manufacturer#4           	Brand#43  	SMALL BRUSHED NICKEL	9	WRAP BAG  	945.04	nts bo
46	honeydew turquoise aquamarine spring tan	Manufacturer#1           	Brand#11  	STANDARD POLISHED TIN	45	WRAP CASE 	946.04	the blithely unusual 
47	honeydew red azure magenta brown	Manufacturer#4           	Brand#45  	LARGE BURNISHED BRASS	14	JUMBO PACK	947.04	 even plate
48	slate thistle cornsilk pale forest	Manufacturer#5           	Brand#53  	STANDARD BRUSHED STEEL	27	JUMBO CASE	948.04	ng to the depo
2500	wheat rose grey khaki floral	Manufacturer#2           	Brand#25  	STANDARD PLATED TIN	43	JUMBO BAG 	1402.50	e furiously aga
2503	saddle steel aquamarine antique orchid	Manufacturer#4           	Brand#44  	ECONOMY POLISHED BRASS	6	SM CASE   	1405.50	kly ironic d
2505	steel indian misty cyan navy	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED TIN	37	LG BOX    	1407.50	 solve slyly
2506	plum chartreuse lemon ghost dim	Manufacturer#2           	Brand#24  	PROMO ANODIZED BRASS	24	WRAP PACK 	1408.50	r the blithel
2507	spring misty smoke moccasin navajo	Manufacturer#1           	Brand#13  	SMALL POLISHED BRASS	34	MED DRUM  	1409.50	e final fox
2509	powder wheat plum peach deep	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	17	JUMBO BAG 	1411.50	dolphi
2510	white rosy drab firebrick spring	Manufacturer#4           	Brand#42  	PROMO PLATED TIN	38	WRAP CAN  	1412.51	its abou
2513	frosted drab almond purple brown	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED NICKEL	30	LG PKG    	1415.51	ully thin ideas hagg
2515	cornflower mint lemon sienna burlywood	Manufacturer#4           	Brand#43  	SMALL PLATED BRASS	13	WRAP PACK 	1417.51	xes are around
2516	lace khaki burlywood bisque goldenrod	Manufacturer#4           	Brand#43  	SMALL BRUSHED COPPER	12	SM CASE   	1418.51	lar theodolites a
2517	goldenrod orange seashell saddle tan	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED STEEL	34	SM CAN    	1419.51	ges bel
2518	gainsboro lace magenta purple drab	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED COPPER	8	SM PKG    	1420.51	ns detect. regular th
2524	khaki snow antique chiffon navajo	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED COPPER	21	LG PKG    	1426.52	 nag car
2525	orchid blush firebrick gainsboro saddle	Manufacturer#5           	Brand#54  	SMALL BURNISHED STEEL	44	LG CAN    	1427.52	ding esc
2526	firebrick navy plum medium red	Manufacturer#3           	Brand#35  	PROMO ANODIZED BRASS	17	SM DRUM   	1428.52	efully final dugouts:
2529	almond honeydew light peach cream	Manufacturer#2           	Brand#25  	STANDARD BRUSHED COPPER	24	SM CAN    	1431.52	ate above 
2530	khaki thistle burlywood cornflower cornsilk	Manufacturer#5           	Brand#54  	LARGE ANODIZED STEEL	33	MED PKG   	1432.53	detect blithel
2531	ivory sky pink firebrick thistle	Manufacturer#3           	Brand#32  	LARGE BRUSHED BRASS	39	SM BOX    	1433.53	ites. fin
2534	almond magenta saddle light lemon	Manufacturer#5           	Brand#55  	LARGE BURNISHED BRASS	38	SM JAR    	1436.53	riously qui
2535	chiffon ghost mint wheat metallic	Manufacturer#1           	Brand#13  	ECONOMY BRUSHED TIN	2	SM CAN    	1437.53	pinto bea
2539	seashell thistle brown smoke sandy	Manufacturer#3           	Brand#35  	LARGE PLATED NICKEL	26	MED CAN   	1441.53	es against
2540	antique sandy cornsilk steel thistle	Manufacturer#2           	Brand#22  	LARGE BRUSHED STEEL	40	MED BAG   	1442.54	quickly unusual depo
2541	sandy floral turquoise misty burlywood	Manufacturer#2           	Brand#22  	LARGE ANODIZED BRASS	16	SM BOX    	1443.54	ly unusual 
2545	olive saddle thistle maroon goldenrod	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	7	JUMBO BOX 	1447.54	nding accounts s
2546	yellow papaya chocolate rosy coral	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED NICKEL	12	LG BAG    	1448.54	e carefu
2547	steel maroon rose spring salmon	Manufacturer#2           	Brand#23  	PROMO BRUSHED NICKEL	29	WRAP BOX  	1449.54	ckly past the iro
2548	medium navajo saddle yellow wheat	Manufacturer#5           	Brand#52  	ECONOMY PLATED BRASS	7	LG CAN    	1450.54	. idea
5000	lace antique light cream grey	Manufacturer#3           	Brand#34  	MEDIUM BRUSHED BRASS	31	LG JAR    	905.00	ow accounts.
5003	bisque moccasin plum snow olive	Manufacturer#4           	Brand#42  	ECONOMY POLISHED TIN	35	LG DRUM   	908.00	 reque
5005	purple hot light floral peach	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED BRASS	31	WRAP PKG  	910.00	ve the s
5006	smoke frosted wheat steel sky	Manufacturer#2           	Brand#22  	LARGE BRUSHED BRASS	48	JUMBO DRUM	911.00	iously. theo
5007	drab lavender coral mint black	Manufacturer#3           	Brand#33  	MEDIUM PLATED BRASS	10	LG CASE   	912.00	ly re
5009	brown burnished coral smoke plum	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED TIN	37	LG PACK   	914.00	ackages. depo
5010	chocolate puff spring linen light	Manufacturer#2           	Brand#21  	LARGE BURNISHED TIN	49	LG BAG    	915.01	arefully regular 
5013	yellow chartreuse royal aquamarine azure	Manufacturer#5           	Brand#52  	LARGE POLISHED NICKEL	34	JUMBO PACK	918.01	telets. re
5015	ghost magenta pink frosted plum	Manufacturer#1           	Brand#13  	STANDARD POLISHED NICKEL	14	WRAP PACK 	920.01	nic theodoli
5016	lime yellow white smoke green	Manufacturer#3           	Brand#32  	SMALL POLISHED STEEL	37	SM PKG    	921.01	y ruthles
5017	grey tan magenta plum saddle	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED NICKEL	17	MED BAG   	922.01	usly special d
5018	lawn violet coral maroon dim	Manufacturer#2           	Brand#25  	STANDARD PLATED COPPER	20	MED BOX   	923.01	osits are
5024	chocolate misty slate moccasin mint	Manufacturer#4           	Brand#43  	ECONOMY PLATED COPPER	12	WRAP PACK 	929.02	s haggle perma
5025	blush goldenrod burlywood powder rosy	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	46	MED CAN   	930.02	the fluff
5026	blanched blush pink light wheat	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED NICKEL	4	SM PACK   	931.02	 theodol
5029	aquamarine green deep firebrick frosted	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED TIN	36	MED PACK  	934.02	ests boost fluf
5030	olive black sandy wheat moccasin	Manufacturer#4           	Brand#41  	PROMO BURNISHED TIN	14	JUMBO CASE	935.03	bold platelet
5031	snow magenta purple cornflower orange	Manufacturer#1           	Brand#11  	ECONOMY POLISHED TIN	14	SM BAG    	936.03	stealthy a
5034	goldenrod chartreuse tan beige black	Manufacturer#2           	Brand#25  	ECONOMY PLATED COPPER	2	LG PKG    	939.03	ies. foxes s
5035	coral gainsboro royal white smoke	Manufacturer#4           	Brand#44  	SMALL ANODIZED NICKEL	18	LG PKG    	940.03	into beans cajole alo
5039	deep chiffon cornflower peach yellow	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED BRASS	46	MED BAG   	944.03	 regula
5040	blanched deep ivory orange blush	Manufacturer#5           	Brand#51  	STANDARD BRUSHED NICKEL	17	WRAP JAR  	945.04	s. carefully 
5041	salmon chartreuse drab misty honeydew	Manufacturer#5           	Brand#53  	MEDIUM POLISHED COPPER	26	SM CASE   	946.04	ages 
5045	gainsboro black orange snow violet	Manufacturer#1           	Brand#12  	PROMO BURNISHED COPPER	43	MED CAN   	950.04	 bold, even platelets
5046	orange pale lime rose maroon	Manufacturer#1           	Brand#14  	LARGE BRUSHED BRASS	47	LG BOX    	951.04	r, final requ
5047	burnished red deep firebrick yellow	Manufacturer#2           	Brand#24  	STANDARD PLATED STEEL	20	SM DRUM   	952.04	s haggle blithely p
5048	coral green blue blanched light	Manufacturer#5           	Brand#54  	SMALL BRUSHED BRASS	36	MED DRUM  	953.04	furiously 
7500	azure royal aquamarine peach goldenrod	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED TIN	41	WRAP DRUM 	1407.50	 fluffily. sp
7503	forest red maroon olive cornsilk	Manufacturer#2           	Brand#21  	LARGE BURNISHED COPPER	7	JUMBO CASE	1410.50	ggle blithely. ideas 
7505	ghost rosy olive tomato burnished	Manufacturer#1           	Brand#13  	LARGE POLISHED BRASS	23	WRAP CASE 	1412.50	deas c
7506	green linen royal lavender azure	Manufacturer#5           	Brand#54  	LARGE PLATED STEEL	48	SM PKG    	1413.50	eas after the excus
7507	slate blush peach indian light	Manufacturer#3           	Brand#35  	LARGE ANODIZED NICKEL	35	JUMBO PKG 	1414.50	ithely pending package
7509	papaya rose turquoise lawn magenta	Manufacturer#3           	Brand#32  	LARGE ANODIZED NICKEL	35	LG DRUM   	1416.50	ts. slyly silent 
7510	sandy drab midnight ivory firebrick	Manufacturer#2           	Brand#22  	ECONOMY POLISHED TIN	50	MED JAR   	1417.51	deposits. final ep
7513	burlywood white ghost cornflower light	Manufacturer#3           	Brand#31  	PROMO POLISHED BRASS	28	MED BAG   	1420.51	quickly unusua
7515	hot metallic cornflower chartreuse yellow	Manufacturer#2           	Brand#21  	ECONOMY PLATED STEEL	43	MED JAR   	1422.51	even deposit
7516	slate blanched cornflower powder blue	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	4	LG BOX    	1423.51	al packages are
7517	olive midnight sandy maroon mint	Manufacturer#5           	Brand#51  	LARGE BRUSHED NICKEL	47	SM CAN    	1424.51	t pinto be
7518	sky slate forest powder gainsboro	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED BRASS	39	MED DRUM  	1425.51	nusual 
7524	lace blue violet light saddle	Manufacturer#5           	Brand#51  	SMALL POLISHED BRASS	50	SM BAG    	1431.52	ing pinto beans na
7525	pale blue ivory dodger honeydew	Manufacturer#5           	Brand#51  	MEDIUM BURNISHED BRASS	18	LG PACK   	1432.52	eve slyly furi
7526	gainsboro violet light burnished deep	Manufacturer#2           	Brand#24  	ECONOMY POLISHED BRASS	23	MED CAN   	1433.52	 frets about the
7529	antique rose honeydew puff salmon	Manufacturer#3           	Brand#35  	PROMO PLATED NICKEL	34	LG BAG    	1436.52	 sile
7530	navajo hot cyan rosy sky	Manufacturer#3           	Brand#34  	SMALL BRUSHED BRASS	36	SM CASE   	1437.53	press packages. fi
7531	sky cyan blush cornflower deep	Manufacturer#1           	Brand#14  	PROMO BURNISHED COPPER	27	SM PACK   	1438.53	ar inst
7534	burnished maroon blue metallic lime	Manufacturer#4           	Brand#42  	STANDARD ANODIZED NICKEL	32	LG PACK   	1441.53	 special requests
7535	lime spring rose dim dark	Manufacturer#4           	Brand#45  	STANDARD POLISHED BRASS	44	JUMBO PKG 	1442.53	ges. special, un
7539	puff peach grey frosted thistle	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	1	LG PACK   	1446.53	osits abou
7540	orange lavender puff linen peru	Manufacturer#1           	Brand#11  	STANDARD POLISHED NICKEL	7	MED CAN   	1447.54	ckly regular pl
7541	azure misty red beige magenta	Manufacturer#2           	Brand#22  	PROMO POLISHED NICKEL	9	WRAP CAN  	1448.54	kly. 
7545	forest firebrick grey purple linen	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED COPPER	34	JUMBO CAN 	1452.54	r requests. q
7546	royal orange aquamarine hot yellow	Manufacturer#3           	Brand#35  	SMALL POLISHED COPPER	33	LG BOX    	1453.54	careful
7547	smoke grey tomato firebrick plum	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED COPPER	38	LG CAN    	1454.54	s. ca
7548	chocolate rose drab white red	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED NICKEL	27	MED PACK  	1455.54	es according to 
10000	navajo lace black steel brown	Manufacturer#3           	Brand#35  	LARGE POLISHED NICKEL	29	MED BAG   	910.00	efully even deposits! 
10003	spring sky moccasin orchid forest	Manufacturer#2           	Brand#23  	STANDARD PLATED COPPER	30	WRAP CASE 	913.00	s. pending, fin
10005	sky mint magenta black misty	Manufacturer#3           	Brand#35  	MEDIUM POLISHED COPPER	4	WRAP PACK 	915.00	hely unusual pac
10006	azure metallic violet puff saddle	Manufacturer#2           	Brand#25  	LARGE ANODIZED TIN	28	JUMBO PKG 	916.00	thely. quickly pend
10007	burlywood ivory pink yellow forest	Manufacturer#2           	Brand#25  	PROMO POLISHED BRASS	2	SM CAN    	917.00	nt theodolites wake al
10009	indian smoke lime magenta tan	Manufacturer#4           	Brand#44  	SMALL ANODIZED BRASS	47	MED CASE  	919.00	ronic acco
10010	sienna saddle chartreuse rose honeydew	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED NICKEL	10	MED DRUM  	920.01	 even excuses against 
10013	beige ivory rosy lemon seashell	Manufacturer#3           	Brand#31  	PROMO BURNISHED STEEL	34	SM JAR    	923.01	inal 
10015	pale ivory olive lace aquamarine	Manufacturer#1           	Brand#15  	LARGE PLATED COPPER	40	MED PACK  	925.01	lar notornis wake qu
10016	royal rosy ghost moccasin puff	Manufacturer#4           	Brand#41  	SMALL ANODIZED STEEL	34	LG BAG    	926.01	sly fi
10017	navy honeydew aquamarine firebrick black	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED STEEL	44	SM JAR    	927.01	 accounts sleep
10018	powder tan misty pale chocolate	Manufacturer#1           	Brand#11  	ECONOMY BRUSHED BRASS	21	LG BAG    	928.01	ording to the car
10024	dark navy sandy yellow cornsilk	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED BRASS	2	JUMBO CASE	934.02	ironic requests. neve
10025	white honeydew olive misty burlywood	Manufacturer#5           	Brand#53  	STANDARD POLISHED COPPER	28	SM PACK   	935.02	le quickly slyly
10026	blush magenta thistle azure cornflower	Manufacturer#2           	Brand#21  	SMALL BURNISHED NICKEL	28	LG BAG    	936.02	as. regula
10029	black midnight misty saddle blush	Manufacturer#5           	Brand#51  	STANDARD BRUSHED STEEL	36	JUMBO JAR 	939.02	g the furiously even
10030	plum saddle chiffon lime deep	Manufacturer#2           	Brand#21  	ECONOMY PLATED COPPER	35	SM PACK   	940.03	onic accounts use fur
10031	misty chartreuse yellow violet red	Manufacturer#4           	Brand#42  	SMALL PLATED BRASS	2	SM CAN    	941.03	iousl
10034	khaki forest lime white coral	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED TIN	49	WRAP PKG  	944.03	l packa
10035	sky smoke antique drab lawn	Manufacturer#1           	Brand#14  	PROMO BURNISHED BRASS	27	MED PKG   	945.03	ecial reques
10039	hot linen antique forest dim	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED BRASS	28	JUMBO CASE	949.03	quests nag c
10040	green rosy olive hot cream	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	2	MED BOX   	950.04	xes. final acco
10041	black olive white navajo cyan	Manufacturer#4           	Brand#45  	PROMO ANODIZED STEEL	46	JUMBO DRUM	951.04	lly even, 
10045	salmon maroon ghost navajo wheat	Manufacturer#2           	Brand#24  	LARGE PLATED BRASS	18	WRAP CAN  	955.04	ly even requests hi
10046	yellow grey violet coral azure	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED TIN	21	MED PKG   	956.04	rmanently u
10047	midnight rose pink dodger chiffon	Manufacturer#2           	Brand#22  	PROMO ANODIZED STEEL	24	LG CAN    	957.04	egular instruction
10048	lemon cornsilk thistle seashell smoke	Manufacturer#4           	Brand#44  	LARGE PLATED TIN	34	SM JAR    	958.04	 regular deposits. s
12497	cornsilk rose yellow turquoise lime	Manufacturer#5           	Brand#52  	MEDIUM PLATED NICKEL	35	LG BAG    	1409.49	gular deposits wa
12500	gainsboro frosted cyan floral blanched	Manufacturer#4           	Brand#41  	ECONOMY POLISHED STEEL	43	LG DRUM   	1412.50	nal excuse
12502	frosted chocolate brown tomato goldenrod	Manufacturer#1           	Brand#12  	ECONOMY POLISHED BRASS	31	WRAP CASE 	1414.50	ts play a
12503	magenta hot dark almond ghost	Manufacturer#2           	Brand#25  	STANDARD BRUSHED STEEL	29	JUMBO JAR 	1415.50	ironic req
12504	forest antique khaki linen hot	Manufacturer#2           	Brand#22  	STANDARD ANODIZED COPPER	46	WRAP JAR  	1416.50	usly 
12506	navy ivory rose turquoise blanched	Manufacturer#4           	Brand#43  	LARGE PLATED TIN	45	MED BAG   	1418.50	es are. permanent, fin
12507	sienna burlywood navy seashell pale	Manufacturer#1           	Brand#12  	STANDARD BURNISHED TIN	10	SM PKG    	1419.50	le requ
12510	grey burnished smoke dim drab	Manufacturer#3           	Brand#33  	STANDARD BRUSHED STEEL	26	SM CASE   	1422.51	eposits a
12512	green lavender puff orchid honeydew	Manufacturer#2           	Brand#24  	ECONOMY PLATED BRASS	4	MED CASE  	1424.51	ously 
12513	drab orange lime beige red	Manufacturer#5           	Brand#54  	SMALL BRUSHED COPPER	19	JUMBO BAG 	1425.51	furiously e
12514	navy dim seashell indian mint	Manufacturer#4           	Brand#41  	SMALL BRUSHED NICKEL	10	WRAP PACK 	1426.51	as. slyly pending dolp
12515	dodger indian cornflower magenta puff	Manufacturer#4           	Brand#44  	PROMO PLATED COPPER	42	WRAP JAR  	1427.51	tly iro
12521	indian rose pink misty hot	Manufacturer#2           	Brand#21  	PROMO PLATED NICKEL	8	WRAP JAR  	1433.52	ites. 
12522	sandy honeydew mint burlywood orchid	Manufacturer#2           	Brand#25  	LARGE POLISHED NICKEL	46	JUMBO BAG 	1434.52	 accounts 
12523	goldenrod lawn light tan linen	Manufacturer#1           	Brand#14  	LARGE ANODIZED BRASS	7	JUMBO BAG 	1435.52	s. final, s
12526	light purple red ivory chocolate	Manufacturer#4           	Brand#44  	STANDARD BURNISHED BRASS	38	MED CASE  	1438.52	rts nag carefully r
12527	turquoise rose plum spring white	Manufacturer#5           	Brand#52  	MEDIUM POLISHED STEEL	40	MED BAG   	1439.52	p furiously speci
12528	dodger cyan smoke lawn sienna	Manufacturer#1           	Brand#13  	LARGE PLATED STEEL	21	WRAP PKG  	1440.52	lyly. slyly
12531	sandy burnished blue tan azure	Manufacturer#1           	Brand#15  	SMALL BRUSHED TIN	44	LG BAG    	1443.53	eposi
12532	navajo khaki antique blush steel	Manufacturer#4           	Brand#44  	MEDIUM BURNISHED BRASS	8	SM JAR    	1444.53	es eat. sly
12536	seashell indian violet tan saddle	Manufacturer#4           	Brand#42  	STANDARD BURNISHED TIN	19	JUMBO PACK	1448.53	e quickly s
12537	brown yellow magenta violet chiffon	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED BRASS	29	WRAP JAR  	1449.53	packages. instru
12538	navajo salmon white honeydew tomato	Manufacturer#5           	Brand#52  	STANDARD ANODIZED NICKEL	40	JUMBO PACK	1450.53	ruthlessly abov
12542	seashell yellow medium honeydew peach	Manufacturer#1           	Brand#12  	MEDIUM POLISHED TIN	40	LG JAR    	1454.54	blithe deposits w
12543	dark olive beige metallic drab	Manufacturer#2           	Brand#22  	LARGE BRUSHED STEEL	5	LG PKG    	1455.54	accou
12544	peach moccasin light orange cream	Manufacturer#5           	Brand#52  	LARGE POLISHED NICKEL	24	JUMBO PACK	1456.54	 blithely never expr
12545	sienna pale coral pink rose	Manufacturer#1           	Brand#12  	PROMO ANODIZED STEEL	50	LG JAR    	1457.54	lites
15001	green pink dodger violet khaki	Manufacturer#5           	Brand#52  	PROMO BURNISHED BRASS	38	WRAP DRUM 	916.00	efully. unusual r
14998	saddle indian mint olive wheat	Manufacturer#3           	Brand#32  	PROMO ANODIZED BRASS	41	JUMBO PACK	1912.99	ole slyly furiou
15003	spring cornsilk smoke orange lace	Manufacturer#1           	Brand#14  	LARGE BRUSHED TIN	13	MED BOX   	918.00	ding d
15004	puff purple light lemon grey	Manufacturer#5           	Brand#53  	MEDIUM POLISHED NICKEL	2	WRAP JAR  	919.00	after the slyl
15005	bisque chiffon puff saddle khaki	Manufacturer#5           	Brand#51  	PROMO ANODIZED NICKEL	14	WRAP BOX  	920.00	 wake slyly ironic pa
15007	firebrick aquamarine gainsboro almond moccasin	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED BRASS	37	JUMBO PKG 	922.00	ng packages. regul
15008	blanched green black thistle maroon	Manufacturer#2           	Brand#21  	PROMO PLATED STEEL	32	WRAP PKG  	923.00	lly special i
15011	tan aquamarine burlywood steel smoke	Manufacturer#3           	Brand#35  	PROMO ANODIZED BRASS	35	JUMBO CAN 	926.01	 about the quickly 
15013	steel cornflower wheat burlywood grey	Manufacturer#1           	Brand#12  	PROMO POLISHED STEEL	46	JUMBO PACK	928.01	s. furious
15014	metallic coral smoke orchid magenta	Manufacturer#1           	Brand#15  	SMALL BRUSHED NICKEL	29	JUMBO BOX 	929.01	ar warthogs s
15015	magenta rosy spring burnished saddle	Manufacturer#5           	Brand#54  	LARGE PLATED TIN	4	MED DRUM  	930.01	oost carefully ironic
15016	ghost aquamarine spring coral lace	Manufacturer#2           	Brand#23  	PROMO BRUSHED TIN	33	SM JAR    	931.01	quests. fluffily fina
15022	tomato mint violet rosy papaya	Manufacturer#4           	Brand#42  	STANDARD BRUSHED BRASS	34	WRAP PACK 	937.02	ts x-ray sl
15023	aquamarine lemon slate blanched beige	Manufacturer#3           	Brand#31  	MEDIUM PLATED TIN	9	LG PKG    	938.02	y regular packages. 
15024	plum goldenrod sandy turquoise smoke	Manufacturer#4           	Brand#44  	SMALL PLATED STEEL	20	SM BOX    	939.02	ide of the f
15027	forest almond misty wheat burnished	Manufacturer#4           	Brand#45  	LARGE BRUSHED NICKEL	4	SM BAG    	942.02	 regular pinto b
15028	blush dark saddle tan cream	Manufacturer#2           	Brand#25  	SMALL BRUSHED BRASS	44	JUMBO BAG 	943.02	sits around 
15029	seashell midnight blanched dark dim	Manufacturer#3           	Brand#34  	ECONOMY ANODIZED STEEL	31	JUMBO JAR 	944.02	slyly unusual 
15032	salmon dim firebrick burnished light	Manufacturer#3           	Brand#31  	MEDIUM PLATED COPPER	5	WRAP DRUM 	947.03	iously bold accounts
15033	thistle moccasin rose sky dim	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED COPPER	48	WRAP PKG  	948.03	at the special, unusua
15037	khaki salmon dark chiffon grey	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED STEEL	22	WRAP PKG  	952.03	le slyly requests. reg
15038	lime thistle burlywood orange tan	Manufacturer#5           	Brand#55  	LARGE BURNISHED STEEL	24	WRAP CAN  	953.03	 packages against 
15039	gainsboro hot blue steel floral	Manufacturer#2           	Brand#25  	PROMO BRUSHED TIN	42	MED BAG   	954.03	ke blithely
15043	firebrick linen lawn cornflower papaya	Manufacturer#2           	Brand#22  	ECONOMY BURNISHED STEEL	14	WRAP CAN  	958.04	thely
15044	bisque sky goldenrod olive floral	Manufacturer#3           	Brand#31  	LARGE POLISHED BRASS	46	MED PKG   	959.04	s. blithely bold re
15045	yellow burlywood burnished peru lemon	Manufacturer#3           	Brand#32  	STANDARD ANODIZED NICKEL	20	MED CASE  	960.04	g courts grow q
15046	dodger royal grey ghost hot	Manufacturer#5           	Brand#52  	STANDARD POLISHED NICKEL	45	LG JAR    	961.04	es serve. regular ac
17499	almond rosy gainsboro plum rose	Manufacturer#4           	Brand#44  	PROMO BRUSHED STEEL	33	WRAP PACK 	1416.49	sts sleep blithely 
17502	tan powder metallic lemon blue	Manufacturer#2           	Brand#22  	PROMO ANODIZED TIN	13	WRAP BOX  	1419.50	l ideas w
17504	navajo saddle light seashell chiffon	Manufacturer#1           	Brand#11  	MEDIUM PLATED BRASS	2	MED BOX   	1421.50	slyly final orbi
17505	orange navajo almond coral wheat	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	14	MED BAG   	1422.50	ickly. ironic platele
17506	ghost white brown spring rose	Manufacturer#3           	Brand#34  	STANDARD PLATED COPPER	21	MED PACK  	1423.50	l orbits. 
17508	khaki pink linen orchid turquoise	Manufacturer#4           	Brand#41  	STANDARD ANODIZED BRASS	27	WRAP BOX  	1425.50	blithely regular
17509	chiffon seashell beige mint plum	Manufacturer#4           	Brand#41  	LARGE BRUSHED COPPER	49	LG BOX    	1426.50	r instructions. blithe
17512	salmon wheat orange blush brown	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED STEEL	21	JUMBO BAG 	1429.51	row blit
17514	orange light black cream sky	Manufacturer#5           	Brand#51  	SMALL BRUSHED COPPER	44	MED PKG   	1431.51	ly ironic accounts a
17515	puff coral seashell wheat medium	Manufacturer#5           	Brand#54  	PROMO POLISHED TIN	49	LG BOX    	1432.51	careful pinto beans 
17516	aquamarine dim peach lace wheat	Manufacturer#5           	Brand#53  	MEDIUM PLATED NICKEL	1	MED CASE  	1433.51	ts dazzle reg
17517	antique azure cyan red cornsilk	Manufacturer#2           	Brand#25  	SMALL ANODIZED STEEL	2	MED BOX   	1434.51	 sly 
17523	powder rosy deep almond sandy	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED STEEL	28	MED CASE  	1440.52	s. qui
17524	plum floral sandy sienna ivory	Manufacturer#1           	Brand#11  	SMALL BRUSHED TIN	14	LG PKG    	1441.52	ven pinto bea
17525	azure seashell cream sky orange	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED NICKEL	32	JUMBO DRUM	1442.52	e packages ar
17528	peach red bisque ghost magenta	Manufacturer#2           	Brand#25  	LARGE POLISHED STEEL	4	WRAP CAN  	1445.52	y pending a
17529	orange white chiffon steel light	Manufacturer#5           	Brand#51  	LARGE BURNISHED NICKEL	14	SM DRUM   	1446.52	l lose. ca
17530	peach tan maroon puff dim	Manufacturer#4           	Brand#43  	SMALL PLATED TIN	49	WRAP PACK 	1447.53	xpress
17533	cyan hot antique misty indian	Manufacturer#3           	Brand#34  	SMALL PLATED NICKEL	43	WRAP BOX  	1450.53	s run
17534	cream plum hot mint turquoise	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED TIN	21	LG DRUM   	1451.53	s. fu
17538	goldenrod light seashell lawn honeydew	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED STEEL	8	SM PKG    	1455.53	even excuses
17539	burlywood smoke chiffon dark chocolate	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED TIN	37	LG JAR    	1456.53	y special i
17540	light dim brown saddle bisque	Manufacturer#5           	Brand#53  	PROMO ANODIZED STEEL	34	LG DRUM   	1457.54	ven instructions. ca
17544	seashell almond chocolate sandy dim	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED TIN	37	MED JAR   	1461.54	the slyly even deposi
17545	burlywood almond seashell tomato deep	Manufacturer#5           	Brand#55  	STANDARD BURNISHED COPPER	18	JUMBO PACK	1462.54	pitaph
17546	chartreuse floral peru lavender cream	Manufacturer#4           	Brand#41  	PROMO BRUSHED TIN	33	MED CASE  	1463.54	ackages r
17547	orchid deep pink wheat green	Manufacturer#4           	Brand#42  	PROMO BURNISHED BRASS	25	WRAP CAN  	1464.54	thely spec
20000	royal red metallic dim azure	Manufacturer#2           	Brand#25  	STANDARD BURNISHED NICKEL	48	SM JAR    	920.00	sts-- 
20003	turquoise papaya navajo cornflower orchid	Manufacturer#2           	Brand#24  	PROMO PLATED BRASS	43	WRAP PKG  	923.00	sentiments slee
20005	dim white violet beige sandy	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED STEEL	12	JUMBO JAR 	925.00	cial theod
20006	snow rosy sienna frosted misty	Manufacturer#5           	Brand#53  	STANDARD BURNISHED NICKEL	32	WRAP JAR  	926.00	hely pending 
20007	thistle cornflower brown snow coral	Manufacturer#5           	Brand#53  	ECONOMY BURNISHED STEEL	47	WRAP PACK 	927.00	refully
20009	floral spring burlywood blush mint	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	2	MED BOX   	929.00	t blithely slyly un
20010	navajo violet burnished cream ivory	Manufacturer#1           	Brand#11  	PROMO POLISHED STEEL	31	MED BOX   	930.01	er the furiously
20013	turquoise azure frosted lawn dim	Manufacturer#5           	Brand#54  	SMALL POLISHED TIN	29	LG PACK   	933.01	ternes wake furiousl
20015	forest blush goldenrod violet frosted	Manufacturer#1           	Brand#15  	PROMO PLATED COPPER	37	SM CAN    	935.01	ic dependencies ha
20016	ivory blush olive forest peru	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED NICKEL	32	JUMBO PKG 	936.01	y ironic pint
20017	green violet ivory floral steel	Manufacturer#5           	Brand#54  	PROMO BURNISHED STEEL	19	LG PKG    	937.01	ideas. excuses 
20018	beige forest gainsboro mint grey	Manufacturer#1           	Brand#15  	ECONOMY PLATED BRASS	48	MED PACK  	938.01	s. expres
20024	cornsilk firebrick white brown plum	Manufacturer#2           	Brand#24  	PROMO BRUSHED COPPER	9	LG JAR    	944.02	nto be
20025	green blue cornflower red forest	Manufacturer#3           	Brand#34  	LARGE POLISHED NICKEL	4	JUMBO DRUM	945.02	side of the 
20026	ivory cornflower magenta blanched yellow	Manufacturer#3           	Brand#35  	STANDARD BURNISHED BRASS	36	JUMBO JAR 	946.02	 across the regular
20029	blanched wheat grey dim lawn	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED STEEL	12	MED BOX   	949.02	s affix carefu
20030	rosy midnight mint saddle frosted	Manufacturer#3           	Brand#35  	ECONOMY BRUSHED NICKEL	29	SM BOX    	950.03	ites. fina
20031	light chiffon orange rosy powder	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	38	JUMBO CAN 	951.03	n requ
20034	azure hot seashell light rosy	Manufacturer#3           	Brand#33  	PROMO PLATED NICKEL	14	JUMBO BOX 	954.03	 hang slyly after 
20035	cornflower brown lawn dodger navy	Manufacturer#2           	Brand#24  	MEDIUM POLISHED TIN	19	LG JAR    	955.03	 slowly final i
20039	dark white tomato steel rose	Manufacturer#2           	Brand#21  	LARGE BRUSHED STEEL	35	WRAP PACK 	959.03	 wake fluffily carefu
20040	royal lawn pink tan burlywood	Manufacturer#3           	Brand#32  	PROMO BURNISHED NICKEL	3	LG BAG    	960.04	 thrash. sheaves slee
20041	lavender olive papaya snow light	Manufacturer#1           	Brand#13  	ECONOMY ANODIZED NICKEL	21	LG CASE   	961.04	equests caj
20045	royal frosted spring rose blanched	Manufacturer#2           	Brand#21  	ECONOMY PLATED BRASS	44	WRAP CAN  	965.04	theodolites 
20046	olive rosy ivory medium spring	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED NICKEL	38	WRAP DRUM 	966.04	s sleep asymptot
20047	bisque azure white forest floral	Manufacturer#1           	Brand#11  	LARGE BRUSHED STEEL	45	WRAP BAG  	967.04	y regular packages. 
20048	royal moccasin papaya purple pink	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	5	JUMBO JAR 	968.04	de of 
22494	burnished bisque dark rose dim	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	14	LG DRUM   	1416.49	theodolites sleep 
22497	firebrick ivory chartreuse peach hot	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED BRASS	23	LG PACK   	1419.49	latelets cajole regula
22499	magenta misty tan olive cornsilk	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED COPPER	4	LG BAG    	1421.49	jole slyly furiously
22500	chocolate pink cream spring snow	Manufacturer#2           	Brand#23  	PROMO BURNISHED COPPER	32	WRAP PKG  	1422.50	, ironic foxes. slyly 
22501	navajo puff chartreuse tomato plum	Manufacturer#4           	Brand#43  	STANDARD PLATED BRASS	19	LG DRUM   	1423.50	ackages. evenly b
22503	metallic saddle violet navajo smoke	Manufacturer#2           	Brand#24  	SMALL PLATED COPPER	12	LG BAG    	1425.50	ss the fluffily ev
22504	azure firebrick khaki medium black	Manufacturer#3           	Brand#35  	MEDIUM ANODIZED BRASS	48	WRAP CAN  	1426.50	fily bold requests
22507	black spring brown chiffon honeydew	Manufacturer#2           	Brand#25  	STANDARD ANODIZED NICKEL	4	WRAP CAN  	1429.50	quickly. accounts ca
22509	drab misty ghost chocolate turquoise	Manufacturer#5           	Brand#55  	MEDIUM POLISHED TIN	2	SM PKG    	1431.50	 ironic foxes nod
22510	navajo forest metallic saddle purple	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED NICKEL	1	WRAP CAN  	1432.51	tes haggle blithel
22511	turquoise snow rosy maroon yellow	Manufacturer#3           	Brand#35  	MEDIUM BRUSHED TIN	21	LG CAN    	1433.51	oss th
22512	hot orange steel cornsilk rose	Manufacturer#2           	Brand#23  	PROMO ANODIZED TIN	5	WRAP DRUM 	1434.51	ajole about
22518	medium red saddle linen orange	Manufacturer#1           	Brand#15  	STANDARD BRUSHED NICKEL	6	WRAP DRUM 	1440.51	he furiously i
22519	spring chocolate misty forest rosy	Manufacturer#5           	Brand#53  	LARGE POLISHED STEEL	48	JUMBO PACK	1441.51	es are slyly ir
22520	goldenrod powder yellow orchid cream	Manufacturer#4           	Brand#42  	PROMO BRUSHED STEEL	2	SM BAG    	1442.52	blithely ex
22523	tan green floral red spring	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED NICKEL	18	LG DRUM   	1445.52	; regular asymptotes w
22524	lemon burlywood frosted turquoise sky	Manufacturer#2           	Brand#25  	STANDARD POLISHED TIN	27	JUMBO CAN 	1446.52	ickly bold 
22525	sienna floral forest puff slate	Manufacturer#3           	Brand#32  	SMALL BRUSHED TIN	7	WRAP DRUM 	1447.52	 cajole above th
22528	peach beige blush saddle lace	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED STEEL	32	LG PKG    	1450.52	ts across
22529	ivory cornflower black magenta linen	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED COPPER	39	WRAP CAN  	1451.52	ay are across th
22533	dark light navy wheat chocolate	Manufacturer#5           	Brand#53  	PROMO ANODIZED NICKEL	38	WRAP BAG  	1455.53	express, regu
22534	ghost deep sky blue almond	Manufacturer#2           	Brand#22  	SMALL POLISHED COPPER	25	LG DRUM   	1456.53	 realms integ
22535	tan white midnight rosy floral	Manufacturer#3           	Brand#35  	SMALL PLATED TIN	32	MED CASE  	1457.53	ely pinto beans. foxes
22539	misty light frosted salmon black	Manufacturer#3           	Brand#32  	ECONOMY POLISHED NICKEL	6	MED CASE  	1461.53	 accounts breach fluff
22540	orchid papaya orange indian almond	Manufacturer#1           	Brand#15  	LARGE POLISHED BRASS	37	SM JAR    	1462.54	 accounts. regul
22541	thistle turquoise white seashell burlywood	Manufacturer#1           	Brand#13  	PROMO PLATED NICKEL	50	MED CASE  	1463.54	y final foxe
22542	white burlywood yellow maroon misty	Manufacturer#3           	Brand#32  	SMALL BURNISHED STEEL	33	SM PACK   	1464.54	hely 
24996	yellow lawn rose burlywood papaya	Manufacturer#1           	Brand#13  	STANDARD ANODIZED TIN	7	WRAP BOX  	1920.99	eep carefully. eve
24999	grey black plum sandy papaya	Manufacturer#4           	Brand#45  	LARGE BURNISHED STEEL	18	MED JAR   	1923.99	ans w
25001	violet turquoise aquamarine beige blue	Manufacturer#4           	Brand#43  	ECONOMY PLATED BRASS	7	WRAP CAN  	926.00	deas 
25002	metallic rosy misty puff lemon	Manufacturer#3           	Brand#35  	STANDARD ANODIZED COPPER	14	LG BAG    	927.00	hely. regular theodoli
25003	almond sienna peru smoke navy	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	10	JUMBO BAG 	928.00	o beans boost; fluffil
25005	misty navajo medium peru yellow	Manufacturer#2           	Brand#22  	SMALL ANODIZED TIN	5	SM BAG    	930.00	its. furiously speci
25006	saddle spring cornsilk moccasin goldenrod	Manufacturer#3           	Brand#33  	LARGE BRUSHED NICKEL	15	LG PACK   	931.00	nal, regular r
25009	dark chiffon cyan steel mint	Manufacturer#1           	Brand#15  	ECONOMY POLISHED STEEL	20	LG BOX    	934.00	jole carefully. slyly
25011	cream salmon mint black seashell	Manufacturer#1           	Brand#12  	SMALL POLISHED COPPER	7	SM CASE   	936.01	telets. s
25012	steel spring antique sandy aquamarine	Manufacturer#2           	Brand#23  	LARGE POLISHED BRASS	33	MED CASE  	937.01	 packages use 
25013	steel snow ivory chocolate peru	Manufacturer#2           	Brand#22  	PROMO POLISHED BRASS	7	LG CAN    	938.01	he final accou
25014	khaki lemon blue cornsilk lavender	Manufacturer#1           	Brand#13  	LARGE PLATED STEEL	34	LG JAR    	939.01	ely regular asymptot
25020	cornsilk dim honeydew coral burlywood	Manufacturer#3           	Brand#32  	PROMO POLISHED STEEL	33	MED JAR   	945.02	across the stea
25021	midnight white mint yellow spring	Manufacturer#1           	Brand#11  	ECONOMY BRUSHED TIN	2	MED CASE  	946.02	 haggle final
25022	tan brown metallic lemon khaki	Manufacturer#3           	Brand#34  	SMALL PLATED STEEL	3	WRAP JAR  	947.02	ets use among the f
25025	indian violet dim sandy chiffon	Manufacturer#3           	Brand#34  	STANDARD POLISHED NICKEL	31	MED PKG   	950.02	tions
25026	firebrick ghost spring sky orange	Manufacturer#1           	Brand#11  	STANDARD BURNISHED TIN	6	LG CASE   	951.02	ending theod
25027	white moccasin purple orange beige	Manufacturer#4           	Brand#44  	SMALL BURNISHED STEEL	30	WRAP BOX  	952.02	uctions. sly
25030	medium misty burnished pale magenta	Manufacturer#5           	Brand#51  	STANDARD PLATED COPPER	45	LG DRUM   	955.03	heodolites. s
25031	cyan white blush powder puff	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED STEEL	47	SM CASE   	956.03	ly pe
25035	lawn blanched cornsilk steel puff	Manufacturer#5           	Brand#55  	PROMO PLATED COPPER	34	SM DRUM   	960.03	y besides the 
25036	yellow seashell royal drab violet	Manufacturer#3           	Brand#33  	MEDIUM BRUSHED TIN	7	MED CASE  	961.03	oxes cajole amon
25037	red olive moccasin blue ivory	Manufacturer#1           	Brand#15  	SMALL BRUSHED TIN	26	MED PKG   	962.03	 are furiously f
25041	khaki sky blue sandy orchid	Manufacturer#2           	Brand#25  	MEDIUM BURNISHED COPPER	24	JUMBO PACK	966.04	egular
25042	azure pink navy moccasin midnight	Manufacturer#3           	Brand#34  	LARGE PLATED COPPER	30	LG CASE   	967.04	ding to the furiously
25043	dim rose almond spring slate	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED NICKEL	20	WRAP PKG  	968.04	y ironic account
25044	frosted salmon blue ghost floral	Manufacturer#1           	Brand#14  	ECONOMY POLISHED NICKEL	19	WRAP CASE 	969.04	e to the sly
27498	coral drab brown rose black	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED NICKEL	40	LG PKG    	1425.49	thinly regu
27501	ghost pink beige turquoise purple	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED STEEL	19	WRAP BAG  	1428.50	posits slee
27503	turquoise smoke olive maroon navy	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED NICKEL	26	MED CASE  	1430.50	g dependenc
27504	blush mint goldenrod light wheat	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	18	JUMBO BOX 	1431.50	nto beans cajo
27505	puff lemon brown chartreuse lavender	Manufacturer#1           	Brand#11  	PROMO BURNISHED STEEL	9	WRAP PKG  	1432.50	gged courts. c
27507	moccasin tan smoke seashell dark	Manufacturer#3           	Brand#33  	SMALL ANODIZED COPPER	41	SM PACK   	1434.50	tions. carefully regul
27508	white yellow blue saddle drab	Manufacturer#1           	Brand#13  	PROMO ANODIZED BRASS	4	MED PKG   	1435.50	re busily bravel
27511	sienna cyan saddle drab chiffon	Manufacturer#3           	Brand#32  	SMALL BRUSHED BRASS	24	MED CASE  	1438.51	s about the accoun
27513	violet metallic lace pink deep	Manufacturer#2           	Brand#21  	SMALL BRUSHED COPPER	3	JUMBO BOX 	1440.51	fully final depo
27514	lime blush rosy ivory orchid	Manufacturer#5           	Brand#52  	SMALL BRUSHED TIN	10	WRAP BAG  	1441.51	al dolphin
27515	pink black cornflower spring misty	Manufacturer#1           	Brand#11  	STANDARD POLISHED COPPER	15	WRAP JAR  	1442.51	ly. quickly regu
27516	light gainsboro violet chocolate orchid	Manufacturer#2           	Brand#24  	ECONOMY BRUSHED TIN	44	JUMBO CAN 	1443.51	ic deposits p
27522	rosy floral almond beige cream	Manufacturer#3           	Brand#35  	SMALL ANODIZED COPPER	38	MED CASE  	1449.52	above the pending, ir
27523	floral sienna magenta wheat grey	Manufacturer#5           	Brand#52  	LARGE PLATED NICKEL	30	MED PKG   	1450.52	cial ideas 
27524	lavender sienna spring chartreuse olive	Manufacturer#1           	Brand#13  	ECONOMY POLISHED BRASS	22	LG BAG    	1451.52	ular pinto beans. 
27527	lime orchid saddle cyan almond	Manufacturer#3           	Brand#32  	STANDARD BURNISHED BRASS	22	WRAP PKG  	1454.52	ts boost ca
27528	orange coral drab burnished cornflower	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED BRASS	43	LG JAR    	1455.52	equests grow against
27529	brown rosy lemon olive cyan	Manufacturer#5           	Brand#54  	STANDARD BURNISHED STEEL	29	LG DRUM   	1456.52	nst the quickly ir
27532	cream dodger violet tan magenta	Manufacturer#2           	Brand#22  	MEDIUM PLATED NICKEL	6	LG CAN    	1459.53	ndencies. blithely ru
27533	cream gainsboro maroon beige navajo	Manufacturer#1           	Brand#11  	ECONOMY PLATED COPPER	20	WRAP BAG  	1460.53	en acco
27537	yellow peru powder dark goldenrod	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED STEEL	26	MED DRUM  	1464.53	lithely speci
27538	peach pale powder snow almond	Manufacturer#2           	Brand#23  	LARGE POLISHED BRASS	15	MED CASE  	1465.53	nal platelets are f
27539	violet beige brown maroon green	Manufacturer#3           	Brand#35  	MEDIUM BRUSHED BRASS	24	JUMBO CASE	1466.53	nto beans 
27543	salmon azure peru deep metallic	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED TIN	23	SM PKG    	1470.54	uffily special 
27544	lavender snow papaya pale yellow	Manufacturer#3           	Brand#33  	STANDARD ANODIZED COPPER	30	LG BOX    	1471.54	 blith
27545	maroon puff orchid olive magenta	Manufacturer#5           	Brand#53  	LARGE BURNISHED TIN	1	MED CAN   	1472.54	 fluffily bus
27546	azure hot almond rose frosted	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED STEEL	33	MED CASE  	1473.54	s. ideas accordin
30000	goldenrod metallic powder floral seashell	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED NICKEL	49	SM CAN    	930.00	counts ha
30003	smoke orchid cornsilk almond dodger	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	9	WRAP JAR  	933.00	al sentiments breac
30005	drab indian honeydew brown burnished	Manufacturer#2           	Brand#21  	SMALL ANODIZED COPPER	16	MED CASE  	935.00	ven deposits. r
30006	medium drab midnight frosted indian	Manufacturer#2           	Brand#21  	SMALL POLISHED NICKEL	24	JUMBO PKG 	936.00	yly silent
30007	misty purple peru lemon spring	Manufacturer#3           	Brand#33  	PROMO BURNISHED STEEL	34	MED JAR   	937.00	 platelets use 
30009	pale sandy sienna linen dark	Manufacturer#3           	Brand#34  	LARGE POLISHED COPPER	34	JUMBO DRUM	939.00	 slyly bold requests 
30010	grey honeydew forest sky wheat	Manufacturer#1           	Brand#15  	STANDARD BURNISHED NICKEL	14	LG CASE   	940.01	e regular, special wa
30013	ghost lemon medium light beige	Manufacturer#3           	Brand#34  	STANDARD BRUSHED STEEL	11	MED PKG   	943.01	ly regular theo
30015	yellow peru snow lemon coral	Manufacturer#4           	Brand#42  	STANDARD POLISHED COPPER	16	SM PKG    	945.01	. fluffily unu
30016	cornsilk hot grey pink ghost	Manufacturer#2           	Brand#22  	ECONOMY ANODIZED STEEL	10	LG JAR    	946.01	ithely reg
30017	violet slate peru khaki moccasin	Manufacturer#2           	Brand#22  	PROMO BURNISHED TIN	48	LG BOX    	947.01	fully special accoun
30018	floral cream peru dark violet	Manufacturer#1           	Brand#12  	SMALL POLISHED TIN	22	LG JAR    	948.01	doze even dep
30024	ghost orchid dark yellow pale	Manufacturer#4           	Brand#42  	LARGE ANODIZED NICKEL	6	JUMBO JAR 	954.02	he final, regular f
30025	khaki wheat seashell misty spring	Manufacturer#3           	Brand#31  	SMALL PLATED NICKEL	48	WRAP BAG  	955.02	ong the 
30026	black mint rosy saddle tomato	Manufacturer#3           	Brand#31  	STANDARD PLATED BRASS	5	MED JAR   	956.02	s haggle: p
30029	coral orange brown smoke orchid	Manufacturer#5           	Brand#52  	STANDARD ANODIZED STEEL	1	MED DRUM  	959.02	lly regular requests. 
30030	steel lace coral rose peach	Manufacturer#4           	Brand#41  	STANDARD POLISHED BRASS	31	WRAP BOX  	960.03	iments!
30031	saddle metallic orange lace grey	Manufacturer#5           	Brand#52  	ECONOMY POLISHED NICKEL	25	MED BAG   	961.03	ing instruct
30034	maroon firebrick purple cornflower sky	Manufacturer#3           	Brand#32  	MEDIUM PLATED BRASS	49	MED PKG   	964.03	 to the regular pin
30035	aquamarine purple lime firebrick deep	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	24	SM PACK   	965.03	 final package
30039	spring violet peach tan brown	Manufacturer#1           	Brand#11  	PROMO BRUSHED COPPER	1	JUMBO CAN 	969.03	n deposits wake blit
30040	azure peach blanched beige chiffon	Manufacturer#3           	Brand#35  	SMALL ANODIZED BRASS	45	LG CASE   	970.04	courts grow alon
30041	light sienna salmon azure goldenrod	Manufacturer#2           	Brand#22  	PROMO PLATED TIN	15	MED CASE  	971.04	y bol
30045	magenta tan frosted cream forest	Manufacturer#5           	Brand#52  	ECONOMY ANODIZED NICKEL	24	WRAP PKG  	975.04	uring the f
30046	dodger red magenta smoke turquoise	Manufacturer#2           	Brand#23  	MEDIUM PLATED STEEL	37	SM PKG    	976.04	 sleep
30047	yellow chiffon dim hot lawn	Manufacturer#5           	Brand#54  	STANDARD POLISHED STEEL	24	LG BOX    	977.04	ckages poach sly
30048	pale orchid tan pink grey	Manufacturer#5           	Brand#51  	LARGE PLATED COPPER	1	WRAP CAN  	978.04	ide of the furiously 
32491	khaki metallic ghost forest powder	Manufacturer#1           	Brand#14  	MEDIUM PLATED STEEL	41	MED CAN   	1423.49	 dazz
32494	dark deep sienna mint yellow	Manufacturer#4           	Brand#42  	SMALL POLISHED STEEL	10	LG JAR    	1426.49	ts. quickly r
32496	hot puff peach azure magenta	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED TIN	35	LG BOX    	1428.49	 requests wake regular
32497	powder orchid burnished ghost almond	Manufacturer#3           	Brand#31  	PROMO ANODIZED BRASS	6	SM CAN    	1429.49	ecial pa
32498	lime spring brown moccasin cream	Manufacturer#1           	Brand#12  	STANDARD POLISHED COPPER	17	JUMBO PKG 	1430.49	g accounts. regula
32500	light brown pink lawn tan	Manufacturer#1           	Brand#14  	PROMO POLISHED STEEL	25	MED PACK  	1432.50	ckly special foxes us
32501	light dodger linen ivory steel	Manufacturer#1           	Brand#13  	LARGE POLISHED NICKEL	4	LG PACK   	1433.50	ular decoys
32504	dark light powder navy royal	Manufacturer#3           	Brand#34  	LARGE BURNISHED COPPER	33	MED BOX   	1436.50	its impress fur
32506	azure olive pale ivory honeydew	Manufacturer#4           	Brand#41  	LARGE BRUSHED STEEL	28	MED PKG   	1438.50	iously even warthog
32507	blue deep lemon lace dark	Manufacturer#5           	Brand#52  	ECONOMY POLISHED BRASS	19	LG CASE   	1439.50	kly regular 
32508	medium coral sky saddle chiffon	Manufacturer#4           	Brand#41  	LARGE ANODIZED NICKEL	49	LG BOX    	1440.50	ches. request
32509	lemon yellow navy gainsboro seashell	Manufacturer#5           	Brand#54  	PROMO ANODIZED COPPER	44	LG PACK   	1441.50	riously. ironic pearls
32515	pale firebrick seashell cyan olive	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED COPPER	1	WRAP DRUM 	1447.51	above the furious
32516	maroon spring blue lavender saddle	Manufacturer#4           	Brand#43  	LARGE ANODIZED TIN	30	MED PKG   	1448.51	sts are sly
32517	rose aquamarine orange thistle pink	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED STEEL	42	LG CAN    	1449.51	o beans. ironic dep
32520	honeydew indian gainsboro burlywood chiffon	Manufacturer#2           	Brand#21  	STANDARD PLATED COPPER	29	WRAP JAR  	1452.52	he final, ironi
32521	yellow peru turquoise blush tomato	Manufacturer#2           	Brand#24  	LARGE PLATED STEEL	40	MED PKG   	1453.52	sits. even pinto
32522	rosy slate antique dodger sky	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	7	SM DRUM   	1454.52	ole slyly slyly iro
32525	azure white plum brown black	Manufacturer#5           	Brand#53  	SMALL ANODIZED NICKEL	33	MED CASE  	1457.52	ndencies. evenly regul
32526	mint floral cyan indian cornsilk	Manufacturer#2           	Brand#22  	PROMO POLISHED TIN	22	MED JAR   	1458.52	ular packages was. f
32530	purple metallic antique black puff	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED NICKEL	24	WRAP JAR  	1462.53	unts along t
32531	drab deep blanched khaki metallic	Manufacturer#5           	Brand#51  	PROMO BURNISHED BRASS	18	MED BAG   	1463.53	: pending foxes alon
32532	blanched grey pink saddle almond	Manufacturer#4           	Brand#44  	SMALL BRUSHED COPPER	30	LG CAN    	1464.53	ts nag care
32536	navy blush red burlywood lace	Manufacturer#4           	Brand#42  	ECONOMY PLATED BRASS	35	MED CASE  	1468.53	uffy somas. furiously 
32537	chiffon lavender mint indian green	Manufacturer#3           	Brand#33  	PROMO PLATED BRASS	31	LG PACK   	1469.53	use ironically? furi
32538	salmon pink chartreuse tan magenta	Manufacturer#1           	Brand#14  	LARGE PLATED STEEL	15	SM BOX    	1470.53	ideas. careful
32539	snow bisque rosy pink pale	Manufacturer#2           	Brand#25  	MEDIUM PLATED TIN	40	LG DRUM   	1471.53	 requests are sl
34994	lace violet blush medium pale	Manufacturer#2           	Brand#22  	STANDARD PLATED TIN	29	WRAP BAG  	1928.99	le carefully.
34997	pale azure yellow mint khaki	Manufacturer#5           	Brand#52  	SMALL BURNISHED BRASS	30	LG JAR    	1931.99	 slyly pending 
34999	burnished purple chocolate dodger blanched	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED BRASS	33	SM DRUM   	1933.99	the entic
35000	saddle indian gainsboro midnight forest	Manufacturer#2           	Brand#25  	SMALL BURNISHED STEEL	25	SM BAG    	935.00	ronic inst
35001	rose indian lime midnight medium	Manufacturer#5           	Brand#52  	SMALL BRUSHED COPPER	22	LG BAG    	936.00	ronic a
35003	peru metallic chocolate turquoise light	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	38	WRAP CAN  	938.00	 furiously eve
35004	cream sky sandy tan floral	Manufacturer#4           	Brand#44  	STANDARD POLISHED COPPER	31	SM BOX    	939.00	 to the ca
35007	black khaki cream coral steel	Manufacturer#1           	Brand#12  	ECONOMY PLATED BRASS	19	SM JAR    	942.00	telets 
35009	brown metallic ivory blanched pink	Manufacturer#5           	Brand#55  	SMALL BURNISHED TIN	50	SM CASE   	944.00	ly fur
35010	spring pink indian blue gainsboro	Manufacturer#2           	Brand#24  	STANDARD PLATED STEEL	32	JUMBO CAN 	945.01	uctions. care
35011	azure violet salmon purple honeydew	Manufacturer#4           	Brand#43  	PROMO BURNISHED TIN	14	SM PKG    	946.01	ly pending decoys. ca
35012	rosy metallic chartreuse peru ivory	Manufacturer#5           	Brand#55  	SMALL POLISHED STEEL	25	WRAP PACK 	947.01	e accounts boo
35018	light royal olive smoke linen	Manufacturer#2           	Brand#24  	PROMO PLATED COPPER	34	JUMBO CASE	953.01	eposits sleep blithely
35019	chartreuse violet ghost midnight dark	Manufacturer#1           	Brand#11  	SMALL POLISHED COPPER	17	WRAP JAR  	954.01	ly even re
35020	bisque dark sienna turquoise lime	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED COPPER	8	SM BOX    	955.02	s plat
35023	navajo coral pink puff thistle	Manufacturer#3           	Brand#35  	LARGE POLISHED NICKEL	24	LG CASE   	958.02	thely regular acc
35024	beige medium red black snow	Manufacturer#2           	Brand#24  	LARGE BURNISHED TIN	29	JUMBO CASE	959.02	ously bol
35025	pale sky dim forest antique	Manufacturer#4           	Brand#45  	STANDARD POLISHED STEEL	21	JUMBO DRUM	960.02	ts cajole fluffily. ca
35028	brown drab purple sandy red	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED STEEL	36	LG PKG    	963.02	s nod bl
35029	burnished cornsilk metallic yellow lime	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED TIN	27	WRAP DRUM 	964.02	iously expre
35033	papaya linen powder forest indian	Manufacturer#4           	Brand#45  	SMALL BRUSHED STEEL	3	SM BOX    	968.03	ronic deposits 
35034	brown magenta beige almond moccasin	Manufacturer#5           	Brand#51  	ECONOMY ANODIZED BRASS	49	MED CASE  	969.03	ts doubt. expre
35035	metallic cyan royal peru almond	Manufacturer#5           	Brand#54  	SMALL POLISHED TIN	29	SM JAR    	970.03	s. sometime
35039	burlywood cornflower cream chiffon dark	Manufacturer#1           	Brand#13  	PROMO ANODIZED STEEL	47	MED CAN   	974.03	ular, regular theodoli
35040	antique rose maroon pink honeydew	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED BRASS	7	JUMBO PACK	975.04	ccount
35041	grey white chartreuse steel chiffon	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED NICKEL	44	WRAP CAN  	976.04	ependenc
35042	tan ivory brown pink khaki	Manufacturer#2           	Brand#22  	ECONOMY BURNISHED STEEL	42	JUMBO DRUM	977.04	uffily ironic, regu
37510	pale blue purple light rose	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED NICKEL	35	WRAP CAN  	1447.51	its sleep
37497	goldenrod rosy green peru azure	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	16	WRAP BAG  	1434.49	 beans cajol
37500	navajo indian beige tan sandy	Manufacturer#2           	Brand#22  	PROMO PLATED NICKEL	47	JUMBO DRUM	1437.50	 final i
37502	puff chiffon gainsboro cyan grey	Manufacturer#2           	Brand#23  	SMALL BURNISHED BRASS	33	MED PKG   	1439.50	posits. theodolites
37503	misty red dodger honeydew aquamarine	Manufacturer#1           	Brand#14  	LARGE POLISHED STEEL	2	SM BAG    	1440.50	 slyly express pack
37504	medium black snow lavender cyan	Manufacturer#2           	Brand#22  	MEDIUM POLISHED STEEL	21	JUMBO PACK	1441.50	e bli
37506	ghost orange cyan lavender hot	Manufacturer#3           	Brand#33  	LARGE ANODIZED COPPER	48	WRAP BAG  	1443.50	thy excuses could nag
37507	navajo chartreuse goldenrod wheat puff	Manufacturer#1           	Brand#14  	SMALL BRUSHED TIN	35	MED CASE  	1444.50	the de
37512	smoke drab plum cornsilk mint	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED TIN	7	LG CAN    	1449.51	ress 
37513	plum indian steel burnished smoke	Manufacturer#4           	Brand#45  	PROMO BRUSHED STEEL	15	LG CASE   	1450.51	ide of the furiously
37514	navajo papaya dodger slate spring	Manufacturer#5           	Brand#51  	MEDIUM PLATED TIN	21	WRAP BOX  	1451.51	ncies sleep perma
37515	spring cyan rose orange mint	Manufacturer#2           	Brand#22  	MEDIUM POLISHED STEEL	3	JUMBO CASE	1452.51	into bea
37521	dodger chocolate forest violet slate	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED NICKEL	11	MED CAN   	1458.52	ggle qui
37522	peru burnished navy almond chocolate	Manufacturer#5           	Brand#54  	PROMO BURNISHED STEEL	37	SM PACK   	1459.52	 regular 
37523	moccasin ivory peach navy orange	Manufacturer#1           	Brand#15  	PROMO POLISHED NICKEL	16	LG CASE   	1460.52	furiously p
37526	sky chiffon lime ghost orange	Manufacturer#1           	Brand#11  	LARGE BRUSHED STEEL	37	SM CAN    	1463.52	ly final ac
37527	papaya red chocolate pale sky	Manufacturer#3           	Brand#35  	STANDARD BRUSHED BRASS	18	WRAP PKG  	1464.52	 furiously about th
37528	linen slate chiffon mint lace	Manufacturer#3           	Brand#33  	PROMO BURNISHED COPPER	1	SM PKG    	1465.52	s. express, speci
37531	maroon burlywood antique saddle forest	Manufacturer#5           	Brand#55  	SMALL BURNISHED BRASS	32	WRAP BOX  	1468.53	es abou
37532	chocolate firebrick gainsboro chartreuse seashell	Manufacturer#1           	Brand#14  	LARGE ANODIZED BRASS	22	SM BAG    	1469.53	en pinto beans cajole 
37536	indian peach powder dim black	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED STEEL	20	JUMBO DRUM	1473.53	 sleep b
37537	ivory khaki purple azure cornsilk	Manufacturer#2           	Brand#23  	SMALL PLATED STEEL	6	JUMBO CAN 	1474.53	hely! slyly quiet id
37538	medium lime chiffon maroon salmon	Manufacturer#3           	Brand#31  	PROMO PLATED BRASS	33	WRAP BAG  	1475.53	y regular pl
37542	maroon chocolate lace peach light	Manufacturer#1           	Brand#14  	LARGE PLATED STEEL	17	MED PACK  	1479.54	counts. fluffil
37543	almond navajo turquoise plum mint	Manufacturer#2           	Brand#25  	MEDIUM PLATED NICKEL	34	JUMBO JAR 	1480.54	counts? slyly re
37544	lace wheat navajo goldenrod lemon	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED BRASS	24	WRAP CASE 	1481.54	blithely special fo
37545	khaki deep burnished light grey	Manufacturer#1           	Brand#11  	PROMO PLATED COPPER	50	MED JAR   	1482.54	nic deposits na
40000	frosted rose maroon yellow dark	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED TIN	34	SM PKG    	940.00	? qui
40003	drab thistle navajo light purple	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	17	JUMBO PKG 	943.00	 slyl
40005	green cornflower tomato tan dark	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	45	MED CAN   	945.00	nusual, reg
40006	seashell lace rosy midnight tan	Manufacturer#3           	Brand#33  	PROMO BRUSHED NICKEL	36	MED JAR   	946.00	counts cajole carefull
40007	rose lace yellow burlywood lawn	Manufacturer#4           	Brand#43  	LARGE BURNISHED STEEL	45	MED CASE  	947.00	 furiously among the
40009	tan dark red rosy bisque	Manufacturer#3           	Brand#35  	MEDIUM ANODIZED BRASS	45	JUMBO PKG 	949.00	 stealthily unusual de
40010	turquoise tan maroon slate rose	Manufacturer#5           	Brand#52  	LARGE BRUSHED NICKEL	35	SM DRUM   	950.01	s the regular,
40013	frosted mint puff smoke grey	Manufacturer#5           	Brand#53  	LARGE PLATED TIN	39	WRAP CAN  	953.01	ular de
40015	orchid cyan indian yellow grey	Manufacturer#1           	Brand#11  	STANDARD ANODIZED TIN	39	JUMBO DRUM	955.01	ly ironic pl
40016	mint lemon saddle honeydew green	Manufacturer#1           	Brand#12  	STANDARD BRUSHED TIN	7	LG CAN    	956.01	c instru
40017	red chocolate cyan powder puff	Manufacturer#5           	Brand#52  	PROMO BRUSHED BRASS	25	LG DRUM   	957.01	ans. bol
40018	dim medium slate almond plum	Manufacturer#4           	Brand#42  	LARGE POLISHED STEEL	34	WRAP CASE 	958.01	e boldly. quickly s
40024	lace deep lime mint gainsboro	Manufacturer#1           	Brand#11  	PROMO ANODIZED NICKEL	20	JUMBO BAG 	964.02	uests boost fu
40025	linen blue wheat steel blush	Manufacturer#3           	Brand#34  	MEDIUM PLATED BRASS	18	SM PKG    	965.02	ckly quic
40026	cyan pink midnight lime steel	Manufacturer#3           	Brand#31  	STANDARD BURNISHED NICKEL	11	LG DRUM   	966.02	e ironic, even foxes a
40029	royal orchid seashell pink sienna	Manufacturer#5           	Brand#55  	SMALL ANODIZED BRASS	41	LG DRUM   	969.02	telets integrate acc
40030	misty goldenrod khaki ivory royal	Manufacturer#4           	Brand#44  	ECONOMY PLATED NICKEL	40	MED PACK  	970.03	ly ironic pains. car
40031	puff coral spring peru turquoise	Manufacturer#4           	Brand#42  	LARGE ANODIZED TIN	46	SM DRUM   	971.03	onic accoun
40034	dodger sandy gainsboro mint sienna	Manufacturer#2           	Brand#22  	SMALL BRUSHED COPPER	24	MED CASE  	974.03	ly. slyly eve
40035	navy chocolate pink indian lawn	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED STEEL	22	SM BAG    	975.03	egrate carefully alon
40039	maroon linen blue papaya wheat	Manufacturer#4           	Brand#45  	SMALL BURNISHED BRASS	13	WRAP DRUM 	979.03	ing accounts ar
40040	drab saddle plum khaki floral	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	30	LG BOX    	980.04	g to the e
40041	black deep olive navajo violet	Manufacturer#5           	Brand#51  	LARGE POLISHED TIN	48	LG PKG    	981.04	ing to the blithely
40045	black red orchid tomato floral	Manufacturer#3           	Brand#33  	STANDARD ANODIZED BRASS	18	JUMBO BAG 	985.04	the furiously 
40046	honeydew drab chocolate moccasin metallic	Manufacturer#2           	Brand#24  	STANDARD BURNISHED TIN	21	LG DRUM   	986.04	 slyly even deposi
40047	rosy steel gainsboro almond slate	Manufacturer#3           	Brand#31  	PROMO BRUSHED COPPER	45	SM CAN    	987.04	s must cajole about th
40048	light medium deep blush lawn	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED TIN	39	LG CASE   	988.04	s sleep. carefu
42488	green wheat goldenrod indian spring	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED NICKEL	48	MED CASE  	1430.48	 the accounts. slyly r
42491	azure antique goldenrod medium navy	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED BRASS	12	WRAP JAR  	1433.49	ly expre
42493	peru gainsboro light lace red	Manufacturer#1           	Brand#14  	LARGE BRUSHED TIN	47	JUMBO CAN 	1435.49	special,
42494	ivory blush coral lawn navy	Manufacturer#3           	Brand#31  	LARGE ANODIZED TIN	27	WRAP CASE 	1436.49	ial warth
42495	olive blush chocolate cyan violet	Manufacturer#2           	Brand#24  	STANDARD BRUSHED NICKEL	42	MED PKG   	1437.49	l excuses. silent
42497	peach dark frosted saddle ghost	Manufacturer#5           	Brand#55  	STANDARD ANODIZED STEEL	44	JUMBO JAR 	1439.49	es detect d
42498	sandy medium brown green peru	Manufacturer#3           	Brand#34  	STANDARD ANODIZED STEEL	21	WRAP PACK 	1440.49	l pearls run. 
42501	misty chiffon cornflower tomato firebrick	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED COPPER	44	MED JAR   	1443.50	ans. regular theodol
42503	turquoise grey khaki sandy thistle	Manufacturer#5           	Brand#52  	MEDIUM ANODIZED BRASS	46	SM PKG    	1445.50	 the slyly 
42504	violet drab green pink dodger	Manufacturer#1           	Brand#12  	ECONOMY POLISHED COPPER	9	SM JAR    	1446.50	iousl
42505	floral yellow red olive cream	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED TIN	34	MED BOX   	1447.50	counts. furiously 
42506	aquamarine chiffon blue plum rosy	Manufacturer#5           	Brand#51  	STANDARD PLATED STEEL	25	MED BAG   	1448.50	dependencies
42512	honeydew lawn linen thistle dim	Manufacturer#2           	Brand#23  	STANDARD PLATED COPPER	33	WRAP CASE 	1454.51	fully above the 
42513	puff maroon peru sandy ivory	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED STEEL	15	MED PKG   	1455.51	. carefully bold pack
42514	lavender brown drab chartreuse blanched	Manufacturer#1           	Brand#12  	PROMO ANODIZED NICKEL	21	JUMBO CAN 	1456.51	uriously
42517	brown antique blanched moccasin chartreuse	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED NICKEL	50	JUMBO BOX 	1459.51	 ruthlessly after 
42518	moccasin salmon black chocolate wheat	Manufacturer#5           	Brand#55  	MEDIUM BRUSHED COPPER	12	JUMBO BOX 	1460.51	counts use carefully 
42519	cyan gainsboro navy khaki green	Manufacturer#2           	Brand#21  	ECONOMY BRUSHED NICKEL	8	SM DRUM   	1461.51	fully express foxe
42522	brown honeydew bisque tomato sienna	Manufacturer#5           	Brand#52  	MEDIUM PLATED BRASS	24	JUMBO DRUM	1464.52	 pending dependencie
42523	green antique tomato seashell blush	Manufacturer#4           	Brand#41  	PROMO POLISHED STEEL	50	LG BAG    	1465.52	ly slyly unus
42527	black midnight navy medium steel	Manufacturer#3           	Brand#31  	PROMO PLATED STEEL	6	WRAP PKG  	1469.52	ons use
42528	ghost tomato snow cream turquoise	Manufacturer#4           	Brand#44  	MEDIUM PLATED STEEL	35	LG PKG    	1470.52	 slyly 
42529	mint cornsilk blush moccasin navajo	Manufacturer#5           	Brand#51  	PROMO POLISHED NICKEL	36	SM CASE   	1471.52	 furiously fi
42533	frosted lemon tomato cornsilk violet	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED NICKEL	34	SM BAG    	1475.53	lithely pend
42534	salmon pale medium plum beige	Manufacturer#4           	Brand#43  	PROMO ANODIZED STEEL	32	JUMBO CASE	1476.53	ogs wake. special 
42535	navajo seashell antique burnished chiffon	Manufacturer#3           	Brand#32  	MEDIUM PLATED TIN	33	MED BAG   	1477.53	 packages are
42536	snow azure sandy dark light	Manufacturer#5           	Brand#55  	SMALL POLISHED COPPER	12	WRAP BOX  	1478.53	ronic
44992	slate khaki smoke almond azure	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED NICKEL	34	JUMBO BOX 	1936.99	he quickly express
44995	purple navajo violet cornsilk lawn	Manufacturer#1           	Brand#12  	SMALL ANODIZED TIN	5	LG PKG    	1939.99	unusual asymptotes us
44997	rose dark ivory red peach	Manufacturer#5           	Brand#52  	LARGE PLATED BRASS	8	LG CAN    	1941.99	ng the silent pinto
44998	medium chiffon black cornflower drab	Manufacturer#4           	Brand#41  	SMALL BRUSHED COPPER	6	MED DRUM  	1942.99	eposits. re
44999	sky cornflower forest khaki goldenrod	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED STEEL	24	LG BOX    	1943.99	onic accounts
45001	plum khaki tan rose linen	Manufacturer#3           	Brand#32  	PROMO BRUSHED TIN	50	MED CAN   	946.00	sual platelets! packag
45002	antique metallic red sienna ivory	Manufacturer#2           	Brand#24  	SMALL POLISHED COPPER	46	MED JAR   	947.00	 the fi
45005	magenta powder medium antique burnished	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED STEEL	29	MED BAG   	950.00	wake furiously. flu
45007	khaki green seashell ghost navajo	Manufacturer#4           	Brand#41  	MEDIUM POLISHED BRASS	20	WRAP BAG  	952.00	the ironic, regular th
45008	lace khaki seashell violet dodger	Manufacturer#5           	Brand#53  	PROMO POLISHED COPPER	27	MED CASE  	953.00	 pend
45009	cream papaya tan snow peach	Manufacturer#1           	Brand#11  	ECONOMY BRUSHED TIN	27	SM CASE   	954.00	ckages 
45010	burlywood peach linen dim puff	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED BRASS	6	MED PACK  	955.01	ound th
45016	black almond aquamarine beige chocolate	Manufacturer#2           	Brand#23  	LARGE PLATED COPPER	44	JUMBO JAR 	961.01	sts. reque
45017	spring red maroon ghost lace	Manufacturer#3           	Brand#32  	MEDIUM PLATED STEEL	46	MED PACK  	962.01	gular theodo
45018	magenta navy green chiffon ghost	Manufacturer#4           	Brand#45  	SMALL POLISHED STEEL	37	JUMBO PACK	963.01	even inst
45021	almond snow magenta tan khaki	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED COPPER	17	SM CAN    	966.02	ounts wake after t
45022	dim brown aquamarine magenta navy	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	43	JUMBO BAG 	967.02	nal pint
45023	rosy bisque coral papaya moccasin	Manufacturer#1           	Brand#12  	ECONOMY BRUSHED TIN	32	JUMBO DRUM	968.02	 quickly express 
45026	pink wheat deep smoke plum	Manufacturer#5           	Brand#53  	PROMO PLATED COPPER	22	WRAP BAG  	971.02	ts across the bold, f
45027	navajo blue lemon salmon lace	Manufacturer#5           	Brand#54  	STANDARD ANODIZED TIN	7	LG JAR    	972.02	ructi
45031	almond forest metallic misty sienna	Manufacturer#2           	Brand#23  	MEDIUM POLISHED STEEL	37	WRAP JAR  	976.03	ges a
45032	orange powder azure cream sienna	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED BRASS	37	SM CAN    	977.03	requests sl
45033	slate orchid cornsilk salmon sienna	Manufacturer#3           	Brand#32  	STANDARD POLISHED STEEL	35	JUMBO BOX 	978.03	sly bold requests 
45037	pale orchid white forest plum	Manufacturer#3           	Brand#32  	SMALL ANODIZED NICKEL	9	JUMBO CASE	982.03	bove the careful
45038	burnished ivory royal pale rosy	Manufacturer#5           	Brand#51  	PROMO BRUSHED TIN	2	LG JAR    	983.03	kages cajole s
45039	peach ghost salmon midnight burnished	Manufacturer#2           	Brand#22  	LARGE BURNISHED COPPER	5	WRAP BAG  	984.03	lyly. carefully spec
45040	chocolate firebrick chartreuse bisque rose	Manufacturer#1           	Brand#15  	PROMO BRUSHED NICKEL	25	MED CAN   	985.04	sts wake. blithel
47496	slate linen burnished dodger burlywood	Manufacturer#5           	Brand#52  	PROMO BURNISHED STEEL	16	LG PACK   	1443.49	boost fu
47499	navajo medium sky gainsboro powder	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	18	WRAP JAR  	1446.49	tes. waters ca
47501	indian orange plum antique ghost	Manufacturer#3           	Brand#33  	LARGE BURNISHED TIN	50	WRAP BAG  	1448.50	e furiously quickly i
47502	lace yellow dim linen dark	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED BRASS	34	SM DRUM   	1449.50	 the regu
47503	sienna deep forest metallic goldenrod	Manufacturer#4           	Brand#43  	ECONOMY POLISHED COPPER	9	MED BAG   	1450.50	ages wake since the un
47505	wheat indian lemon grey peach	Manufacturer#3           	Brand#33  	MEDIUM BRUSHED TIN	41	JUMBO PKG 	1452.50	es run furious
47506	dark azure dodger seashell frosted	Manufacturer#3           	Brand#32  	STANDARD BRUSHED STEEL	22	MED JAR   	1453.50	ar, express instructio
47509	lavender coral drab moccasin navajo	Manufacturer#5           	Brand#52  	LARGE ANODIZED NICKEL	10	MED CAN   	1456.50	quickly 
47511	steel blue orange dark almond	Manufacturer#2           	Brand#23  	LARGE ANODIZED BRASS	2	LG BOX    	1458.51	ironic 
47512	steel lace chiffon blue orchid	Manufacturer#3           	Brand#34  	STANDARD BURNISHED NICKEL	23	LG PACK   	1459.51	uriously. blith
47513	wheat bisque puff coral peru	Manufacturer#2           	Brand#22  	LARGE POLISHED BRASS	33	SM PACK   	1460.51	are. foxes cajole bli
47514	lawn firebrick frosted beige snow	Manufacturer#1           	Brand#11  	STANDARD PLATED BRASS	49	MED CASE  	1461.51	ly special deposits
47520	navajo beige indian white lace	Manufacturer#3           	Brand#33  	STANDARD POLISHED TIN	11	SM BAG    	1467.52	ously bold packages
47521	green lemon orchid lawn sandy	Manufacturer#3           	Brand#33  	STANDARD BRUSHED STEEL	22	MED DRUM  	1468.52	ully.
47522	white dim honeydew snow navajo	Manufacturer#5           	Brand#51  	SMALL ANODIZED BRASS	10	SM JAR    	1469.52	egular theod
47525	dodger saddle smoke tomato lavender	Manufacturer#5           	Brand#53  	SMALL BRUSHED NICKEL	10	JUMBO CASE	1472.52	 the sauternes. even
47526	sandy aquamarine green dark magenta	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	49	LG BOX    	1473.52	lms acr
47527	forest salmon misty plum lime	Manufacturer#4           	Brand#42  	STANDARD BRUSHED NICKEL	20	SM BOX    	1474.52	asymptotes
47530	honeydew antique sienna lawn violet	Manufacturer#3           	Brand#33  	LARGE BURNISHED COPPER	25	SM PKG    	1477.53	iously f
47531	smoke tan mint seashell ghost	Manufacturer#5           	Brand#53  	ECONOMY PLATED NICKEL	39	SM CASE   	1478.53	foxes
47535	rosy yellow honeydew lavender aquamarine	Manufacturer#2           	Brand#24  	PROMO ANODIZED NICKEL	25	WRAP DRUM 	1482.53	eans w
47536	snow navajo rosy thistle light	Manufacturer#3           	Brand#33  	SMALL PLATED COPPER	40	WRAP BAG  	1483.53	uriously unusual foxes
47537	firebrick cyan almond mint medium	Manufacturer#1           	Brand#12  	MEDIUM POLISHED NICKEL	12	JUMBO JAR 	1484.53	to beans kindle a
47541	orange dodger wheat rose lavender	Manufacturer#4           	Brand#42  	ECONOMY BRUSHED BRASS	45	LG BOX    	1488.54	jole carefully 
47542	violet floral cornflower medium cream	Manufacturer#2           	Brand#21  	PROMO PLATED BRASS	23	JUMBO CAN 	1489.54	fully ironic sauterne
47543	smoke salmon midnight azure forest	Manufacturer#1           	Brand#15  	PROMO BURNISHED TIN	49	JUMBO BOX 	1490.54	al orbits wake blith
47544	honeydew coral steel dim smoke	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED NICKEL	41	MED JAR   	1491.54	the quick
50000	dark blush snow lime smoke	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED STEEL	46	WRAP CAN  	950.00	yly ironic ideas ar
50003	wheat almond lime powder turquoise	Manufacturer#4           	Brand#42  	SMALL BRUSHED TIN	45	JUMBO BAG 	953.00	final pattern
50005	olive blanched pale chartreuse medium	Manufacturer#5           	Brand#55  	STANDARD PLATED STEEL	29	WRAP DRUM 	955.00	pecial instr
50006	navajo peru white khaki smoke	Manufacturer#1           	Brand#14  	STANDARD POLISHED BRASS	25	LG PACK   	956.00	never thin accounts. f
50007	blush rosy puff seashell royal	Manufacturer#2           	Brand#24  	LARGE PLATED BRASS	15	LG BOX    	957.00	. quickly expres
50009	linen peach powder antique metallic	Manufacturer#1           	Brand#14  	PROMO POLISHED BRASS	36	WRAP JAR  	959.00	ly. packages boost bli
50010	smoke lime burlywood midnight coral	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED BRASS	30	MED PACK  	960.01	to the slyly regu
50013	linen saddle coral aquamarine blanched	Manufacturer#3           	Brand#34  	LARGE BRUSHED BRASS	43	LG CASE   	963.01	l asympto
50015	gainsboro dodger white salmon rosy	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED NICKEL	20	JUMBO JAR 	965.01	 quic
50016	hot midnight violet linen olive	Manufacturer#1           	Brand#14  	MEDIUM PLATED NICKEL	23	WRAP PKG  	966.01	among the furiously
50017	green papaya frosted powder blue	Manufacturer#1           	Brand#14  	LARGE BRUSHED COPPER	4	MED PACK  	967.01	y across the regul
50018	lace blush misty sky saddle	Manufacturer#3           	Brand#34  	PROMO BURNISHED STEEL	9	SM JAR    	968.01	posits. re
50024	floral orchid khaki cornflower puff	Manufacturer#1           	Brand#11  	SMALL BURNISHED COPPER	20	MED JAR   	974.02	nag. furiously
50025	purple midnight grey pale puff	Manufacturer#3           	Brand#32  	STANDARD ANODIZED BRASS	29	MED BAG   	975.02	ily final packages
50026	frosted tomato chocolate violet plum	Manufacturer#3           	Brand#32  	ECONOMY BURNISHED STEEL	8	JUMBO CASE	976.02	egular deposits mainta
50029	saddle navy orchid goldenrod blush	Manufacturer#1           	Brand#14  	ECONOMY POLISHED TIN	46	MED CAN   	979.02	ar theodoli
50030	spring olive frosted dim wheat	Manufacturer#5           	Brand#51  	STANDARD PLATED STEEL	14	WRAP BAG  	980.03	 the final req
50031	honeydew firebrick navy lace rosy	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED TIN	50	WRAP BOX  	981.03	s. regular deposits c
50034	cornflower firebrick medium dodger floral	Manufacturer#2           	Brand#22  	PROMO PLATED COPPER	34	SM BAG    	984.03	o beans? u
50035	papaya rosy snow deep thistle	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED TIN	33	MED CASE  	985.03	the pending tithes.
50039	aquamarine cyan azure red floral	Manufacturer#1           	Brand#14  	STANDARD PLATED STEEL	46	LG DRUM   	989.03	unusual i
50040	aquamarine cream ivory thistle chocolate	Manufacturer#4           	Brand#45  	SMALL POLISHED COPPER	1	MED BOX   	990.04	aggle ironic
50041	tomato pink navy moccasin seashell	Manufacturer#1           	Brand#13  	LARGE PLATED STEEL	5	JUMBO JAR 	991.04	ts sleep
50045	chartreuse honeydew pink light firebrick	Manufacturer#5           	Brand#52  	SMALL POLISHED STEEL	18	MED BAG   	995.04	ctions. furious
50046	seashell blush steel pale peru	Manufacturer#5           	Brand#53  	LARGE BURNISHED COPPER	17	MED BAG   	996.04	wake fluffily. qu
50047	orchid tomato rose cornsilk cream	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED TIN	11	SM BAG    	997.04	. foxes h
50048	blush forest rosy honeydew white	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED STEEL	39	WRAP JAR  	998.04	nding requests
52485	green aquamarine lime midnight purple	Manufacturer#1           	Brand#13  	LARGE ANODIZED NICKEL	29	JUMBO PACK	1437.48	ly above the pe
52488	midnight floral azure black goldenrod	Manufacturer#5           	Brand#54  	LARGE BURNISHED STEEL	32	SM BOX    	1440.48	uriously 
52490	floral azure orchid rosy misty	Manufacturer#4           	Brand#43  	PROMO POLISHED NICKEL	40	JUMBO JAR 	1442.49	packages are about the
52491	puff orange dim chocolate burlywood	Manufacturer#1           	Brand#14  	STANDARD POLISHED TIN	22	LG BOX    	1443.49	ironicall
52492	deep azure magenta salmon peach	Manufacturer#4           	Brand#42  	SMALL BRUSHED NICKEL	36	MED CASE  	1444.49	efully unusual r
52494	lemon wheat powder smoke blue	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED NICKEL	39	WRAP PACK 	1446.49	sly ab
52495	cornflower ghost turquoise light spring	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED BRASS	44	WRAP DRUM 	1447.49	y bold pinto beans pro
52498	blanched beige orchid burlywood puff	Manufacturer#1           	Brand#12  	ECONOMY POLISHED TIN	14	JUMBO JAR 	1450.49	ets nod blithely spec
52500	cornflower magenta cornsilk blue rose	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED COPPER	12	LG PACK   	1452.50	 nag quickly a
52501	magenta tomato burlywood goldenrod pink	Manufacturer#4           	Brand#45  	PROMO BRUSHED TIN	4	MED BOX   	1453.50	 slyly unu
52502	cornflower forest white aquamarine cornsilk	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED STEEL	29	SM PKG    	1454.50	inal theodolites: iro
52503	antique linen saddle plum burnished	Manufacturer#3           	Brand#34  	LARGE POLISHED BRASS	42	SM CAN    	1455.50	d instructions s
52509	rose forest cornsilk lime spring	Manufacturer#5           	Brand#51  	LARGE BRUSHED NICKEL	44	WRAP BOX  	1461.50	ffily special 
52510	navajo light midnight maroon dodger	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	1	LG JAR    	1462.51	press asymptotes w
52511	salmon saddle linen navy dim	Manufacturer#1           	Brand#14  	SMALL POLISHED TIN	27	MED CAN   	1463.51	t have
52514	sienna saddle seashell forest linen	Manufacturer#4           	Brand#41  	PROMO ANODIZED BRASS	6	JUMBO CAN 	1466.51	ously i
52515	rosy deep almond gainsboro tan	Manufacturer#4           	Brand#44  	PROMO PLATED BRASS	34	JUMBO PKG 	1467.51	blithely unu
52516	almond blanched ivory goldenrod chocolate	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED BRASS	15	MED CASE  	1468.51	furiously ironic pack
52519	hot light navy spring firebrick	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED BRASS	25	MED PACK  	1471.51	r theodolites acc
52520	cornsilk cyan green beige rose	Manufacturer#1           	Brand#12  	ECONOMY POLISHED COPPER	5	LG BOX    	1472.52	ep flu
52524	turquoise royal dim antique puff	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED STEEL	9	SM DRUM   	1476.52	equests
52525	hot smoke cream misty navajo	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED NICKEL	49	JUMBO CASE	1477.52	s sleep 
52526	violet lace blue light dodger	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED BRASS	34	SM CASE   	1478.52	 furio
52530	linen dodger slate beige wheat	Manufacturer#5           	Brand#52  	PROMO BURNISHED STEEL	26	MED BAG   	1482.53	posits
52531	slate moccasin misty smoke red	Manufacturer#3           	Brand#32  	SMALL POLISHED BRASS	42	MED BAG   	1483.53	 slyly da
52532	spring blanched royal antique burlywood	Manufacturer#2           	Brand#24  	MEDIUM BURNISHED TIN	44	JUMBO PACK	1484.53	quickly
52533	coral olive drab dodger grey	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED TIN	15	SM PKG    	1485.53	 accordi
54990	dark lemon floral drab lavender	Manufacturer#4           	Brand#42  	MEDIUM POLISHED BRASS	34	LG CAN    	1944.99	uriously reque
54993	sienna light thistle burlywood turquoise	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	13	WRAP BOX  	1947.99	ly furiously
54995	navy white green cornsilk sienna	Manufacturer#3           	Brand#32  	LARGE PLATED STEEL	22	WRAP PKG  	1949.99	counts.
54996	gainsboro peach hot lavender chiffon	Manufacturer#2           	Brand#21  	PROMO POLISHED TIN	40	MED DRUM  	1950.99	atelets wake. bl
54997	powder beige chiffon mint tan	Manufacturer#4           	Brand#41  	PROMO BURNISHED NICKEL	2	WRAP PKG  	1951.99	the express
54999	green dodger rose black magenta	Manufacturer#3           	Brand#33  	STANDARD PLATED TIN	2	SM PKG    	1953.99	efully final attai
55000	cyan yellow floral misty tan	Manufacturer#3           	Brand#32  	LARGE ANODIZED COPPER	17	MED PACK  	955.00	y final instructi
55003	plum goldenrod tan red pink	Manufacturer#4           	Brand#43  	PROMO PLATED NICKEL	47	JUMBO BAG 	958.00	onic dependencies
55005	smoke honeydew drab tomato antique	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED NICKEL	8	MED CASE  	960.00	slyly about the dep
55006	forest pink drab metallic sandy	Manufacturer#5           	Brand#51  	LARGE BURNISHED NICKEL	37	MED JAR   	961.00	sual a
55007	aquamarine slate salmon sky turquoise	Manufacturer#5           	Brand#54  	PROMO ANODIZED STEEL	40	LG JAR    	962.00	slyly after the s
55008	misty drab rose cyan antique	Manufacturer#4           	Brand#42  	ECONOMY PLATED COPPER	29	SM BOX    	963.00	 the silently p
55014	red orange linen medium burlywood	Manufacturer#5           	Brand#52  	STANDARD BURNISHED STEEL	4	JUMBO BOX 	969.01	s affix blit
55015	pale white spring smoke puff	Manufacturer#3           	Brand#31  	ECONOMY POLISHED COPPER	40	MED BAG   	970.01	among the express acc
55016	metallic midnight aquamarine chocolate dark	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED COPPER	6	WRAP JAR  	971.01	le bravely according
55019	frosted beige khaki hot tomato	Manufacturer#1           	Brand#12  	PROMO BRUSHED TIN	49	LG CAN    	974.01	fluffily abo
55020	blue dim powder khaki white	Manufacturer#3           	Brand#35  	MEDIUM PLATED NICKEL	46	JUMBO CASE	975.02	unusual deposit
55021	rose saddle orange peru violet	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED TIN	21	WRAP DRUM 	976.02	y slyly regu
55024	lime lavender sienna puff snow	Manufacturer#3           	Brand#33  	SMALL POLISHED BRASS	7	MED CASE  	979.02	 sleep furiousl
55025	brown wheat cornsilk tan chiffon	Manufacturer#3           	Brand#33  	ECONOMY PLATED TIN	11	LG PKG    	980.02	ways unus
55029	midnight azure deep sienna powder	Manufacturer#4           	Brand#41  	LARGE BURNISHED COPPER	3	SM BOX    	984.02	 beans. ironic, regul
55030	maroon gainsboro powder spring blanched	Manufacturer#1           	Brand#12  	ECONOMY BURNISHED BRASS	33	JUMBO PACK	985.03	ely bold dependenci
55031	almond aquamarine blue puff tan	Manufacturer#1           	Brand#11  	ECONOMY BURNISHED STEEL	40	WRAP CAN  	986.03	he packa
55035	firebrick maroon almond saddle dodger	Manufacturer#3           	Brand#34  	STANDARD POLISHED TIN	4	SM BAG    	990.03	y unusual theo
55036	smoke spring royal sienna medium	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED TIN	34	SM CAN    	991.03	ithely even theodol
55037	black blush linen ivory puff	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED NICKEL	19	LG JAR    	992.03	y ironi
55038	saddle lime pale chocolate orange	Manufacturer#3           	Brand#33  	ECONOMY POLISHED TIN	27	MED PACK  	993.03	ove the even, unusual
57495	green pale rose sienna grey	Manufacturer#5           	Brand#54  	SMALL PLATED TIN	11	JUMBO BOX 	1452.49	ully expres
57498	dark ivory light firebrick thistle	Manufacturer#5           	Brand#54  	STANDARD POLISHED TIN	42	JUMBO CAN 	1455.49	ets. blithely
57500	orchid bisque misty lime beige	Manufacturer#1           	Brand#15  	SMALL POLISHED NICKEL	15	SM BOX    	1457.50	es play quickly ex
57501	ghost blue drab seashell beige	Manufacturer#2           	Brand#22  	STANDARD BRUSHED STEEL	36	WRAP BAG  	1458.50	pliers cajole f
57502	thistle lavender rose orchid firebrick	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED TIN	30	MED CAN   	1459.50	, pending instru
57504	drab purple ghost linen grey	Manufacturer#3           	Brand#33  	STANDARD BURNISHED NICKEL	11	MED CASE  	1461.50	es above the fur
57505	lime chocolate seashell magenta powder	Manufacturer#5           	Brand#53  	PROMO ANODIZED NICKEL	42	WRAP CAN  	1462.50	ully special 
57508	magenta burnished rose red seashell	Manufacturer#3           	Brand#34  	ECONOMY POLISHED NICKEL	50	SM CAN    	1465.50	al pa
57510	rosy blanched red yellow antique	Manufacturer#3           	Brand#31  	ECONOMY BURNISHED STEEL	2	LG CASE   	1467.51	es. b
57511	firebrick cyan cream orange dim	Manufacturer#5           	Brand#51  	ECONOMY BURNISHED STEEL	18	JUMBO CASE	1468.51	. ironic deposits am
57512	chiffon royal papaya thistle navy	Manufacturer#4           	Brand#43  	LARGE PLATED BRASS	19	WRAP BOX  	1469.51	aggle fluffily blith
57513	plum peach ghost mint blush	Manufacturer#1           	Brand#13  	STANDARD POLISHED STEEL	31	SM PACK   	1470.51	 theodolites nag fur
57519	cornflower blue misty floral puff	Manufacturer#3           	Brand#35  	PROMO BRUSHED STEEL	22	LG CASE   	1476.51	 unusual package
57520	indian tan steel peach linen	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED NICKEL	17	SM BOX    	1477.52	asymptotes cajole quic
57521	salmon grey chocolate medium burnished	Manufacturer#1           	Brand#15  	PROMO BRUSHED NICKEL	4	LG CASE   	1478.52	ess deposits
57524	forest lace cornsilk royal green	Manufacturer#4           	Brand#43  	SMALL BRUSHED TIN	3	JUMBO PKG 	1481.52	thely special
57525	khaki ivory chartreuse drab burlywood	Manufacturer#1           	Brand#14  	PROMO BURNISHED STEEL	13	MED PACK  	1482.52	uriously regular depos
57526	brown misty salmon ghost maroon	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED BRASS	7	LG PKG    	1483.52	ly slyl
57529	khaki metallic forest almond salmon	Manufacturer#4           	Brand#44  	MEDIUM POLISHED BRASS	37	JUMBO CASE	1486.52	quests cajole fu
57530	misty white firebrick seashell gainsboro	Manufacturer#2           	Brand#24  	SMALL POLISHED NICKEL	44	LG PKG    	1487.53	counts. car
57534	plum aquamarine khaki hot lavender	Manufacturer#5           	Brand#52  	LARGE BURNISHED STEEL	50	MED PACK  	1491.53	 express
57535	light chocolate chiffon cream rose	Manufacturer#2           	Brand#21  	SMALL PLATED NICKEL	42	MED PACK  	1492.53	t about the even, iron
57536	saddle forest lemon medium almond	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED TIN	38	SM JAR    	1493.53	counts. bo
57540	indian forest pink honeydew navajo	Manufacturer#2           	Brand#22  	LARGE ANODIZED NICKEL	1	SM CASE   	1497.54	leep acc
57541	green lawn maroon salmon blue	Manufacturer#5           	Brand#52  	MEDIUM BURNISHED NICKEL	45	JUMBO BAG 	1498.54	sits a
57542	sienna burnished light indian slate	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED BRASS	38	WRAP BAG  	1499.54	ic sheaves coul
57543	purple olive khaki misty peach	Manufacturer#4           	Brand#41  	STANDARD ANODIZED STEEL	16	LG CAN    	1500.54	ss, even pack
60015	dim red azure papaya ivory	Manufacturer#3           	Brand#34  	PROMO ANODIZED TIN	48	WRAP JAR  	975.01	c packages nag
60000	slate blush blue aquamarine lime	Manufacturer#1           	Brand#14  	MEDIUM BRUSHED TIN	40	LG BOX    	960.00	ggle carefully 
60003	bisque almond beige powder hot	Manufacturer#1           	Brand#12  	SMALL ANODIZED STEEL	45	WRAP DRUM 	963.00	ly caref
60005	metallic salmon aquamarine mint lavender	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED BRASS	26	SM CAN    	965.00	cies cajole slyly even
60006	steel orchid mint goldenrod cornflower	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	3	WRAP DRUM 	966.00	furiously regular 
60007	forest peru lavender almond firebrick	Manufacturer#2           	Brand#25  	SMALL BRUSHED TIN	32	LG PKG    	967.00	ests? qui
60009	gainsboro midnight red antique peru	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED COPPER	24	LG CASE   	969.00	iously even dependenci
60010	blush salmon lavender grey maroon	Manufacturer#2           	Brand#25  	ECONOMY POLISHED STEEL	18	SM PACK   	970.01	press requests.
60013	chocolate plum lavender royal red	Manufacturer#1           	Brand#13  	SMALL BRUSHED STEEL	28	LG CASE   	973.01	otornis. boldly bo
60016	yellow cyan chocolate blanched almond	Manufacturer#4           	Brand#41  	LARGE BRUSHED TIN	47	WRAP PACK 	976.01	cies 
60017	lavender smoke ghost pink linen	Manufacturer#2           	Brand#24  	SMALL BRUSHED COPPER	19	LG CASE   	977.01	he furiously busy fret
60018	black firebrick brown lavender yellow	Manufacturer#3           	Brand#32  	MEDIUM POLISHED TIN	2	MED BAG   	978.01	 foxes sleep acc
60024	saddle goldenrod azure violet orchid	Manufacturer#4           	Brand#44  	PROMO BRUSHED NICKEL	3	MED PACK  	984.02	s detect. slyly pend
60025	powder chocolate navajo linen beige	Manufacturer#2           	Brand#22  	ECONOMY PLATED STEEL	29	JUMBO BAG 	985.02	xpress, pending packa
60026	cyan coral orange tan lace	Manufacturer#2           	Brand#24  	PROMO ANODIZED STEEL	21	JUMBO PKG 	986.02	g to the ca
60029	ivory bisque light magenta burlywood	Manufacturer#4           	Brand#45  	PROMO POLISHED COPPER	45	JUMBO BOX 	989.02	osits use a
60030	turquoise thistle green slate burnished	Manufacturer#4           	Brand#43  	SMALL POLISHED TIN	38	JUMBO CAN 	990.03	ts engage
60031	blush blue magenta deep floral	Manufacturer#4           	Brand#45  	SMALL BRUSHED NICKEL	50	SM DRUM   	991.03	ns about the unusual 
60034	sky hot powder deep chiffon	Manufacturer#5           	Brand#54  	LARGE PLATED COPPER	21	SM JAR    	994.03	gular requests: regul
60035	olive turquoise blush sky blue	Manufacturer#1           	Brand#12  	STANDARD PLATED STEEL	1	LG PKG    	995.03	s the blithely p
60039	bisque misty azure burnished orchid	Manufacturer#3           	Brand#34  	SMALL ANODIZED TIN	16	LG PKG    	999.03	sly regular
60040	forest dark deep cornflower black	Manufacturer#5           	Brand#55  	MEDIUM PLATED TIN	18	SM PACK   	1000.04	ly. packages detect qu
60041	tomato bisque misty firebrick deep	Manufacturer#2           	Brand#22  	STANDARD ANODIZED STEEL	5	WRAP CASE 	1001.04	es. slyly final grouch
60045	beige khaki aquamarine violet turquoise	Manufacturer#3           	Brand#32  	PROMO BRUSHED TIN	19	MED JAR   	1005.04	 pinto beans. accounts
60046	tan aquamarine firebrick sky medium	Manufacturer#1           	Brand#13  	SMALL PLATED NICKEL	45	WRAP CAN  	1006.04	ggle. ironic, fi
60047	floral navy saddle sky bisque	Manufacturer#1           	Brand#12  	PROMO POLISHED COPPER	42	SM PACK   	1007.04	ction
60048	lavender rosy sky purple chartreuse	Manufacturer#1           	Brand#11  	PROMO ANODIZED TIN	30	WRAP CASE 	1008.04	ecial foxes. quickly p
62482	peru purple salmon lime smoke	Manufacturer#1           	Brand#11  	SMALL BURNISHED COPPER	19	WRAP PKG  	1444.48	ronic requests. regula
62485	moccasin antique brown drab ivory	Manufacturer#1           	Brand#15  	SMALL ANODIZED STEEL	29	WRAP DRUM 	1447.48	even, final fo
62487	seashell pale dark magenta firebrick	Manufacturer#2           	Brand#24  	MEDIUM PLATED STEEL	12	LG DRUM   	1449.48	ccounts use 
62488	cornflower thistle chiffon navy peach	Manufacturer#5           	Brand#54  	LARGE BRUSHED STEEL	19	MED JAR   	1450.48	uests according to th
62489	yellow sienna papaya brown chartreuse	Manufacturer#5           	Brand#52  	MEDIUM PLATED BRASS	39	SM BOX    	1451.48	ges nag furiously ag
62491	khaki beige chartreuse seashell indian	Manufacturer#2           	Brand#25  	LARGE PLATED NICKEL	48	JUMBO BAG 	1453.49	pending d
62492	violet linen slate deep antique	Manufacturer#3           	Brand#34  	MEDIUM BRUSHED STEEL	25	WRAP JAR  	1454.49	 pinto
62495	dim hot metallic rosy drab	Manufacturer#3           	Brand#34  	LARGE BURNISHED BRASS	40	LG CAN    	1457.49	 to the instructions w
62497	peru sandy khaki azure wheat	Manufacturer#2           	Brand#25  	PROMO ANODIZED TIN	31	LG PACK   	1459.49	dolites
62498	lime puff papaya purple brown	Manufacturer#5           	Brand#51  	LARGE PLATED COPPER	1	JUMBO PACK	1460.49	e carefully
62499	lime saddle tan honeydew antique	Manufacturer#5           	Brand#54  	SMALL PLATED STEEL	12	LG JAR    	1461.49	 believe blit
62500	ivory tomato forest brown snow	Manufacturer#5           	Brand#51  	LARGE BRUSHED NICKEL	29	SM DRUM   	1462.50	iet courts 
62506	chiffon sienna firebrick blue forest	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED BRASS	29	WRAP JAR  	1468.50	e fluffily slyly ironi
62507	sky cornflower rosy cyan forest	Manufacturer#2           	Brand#22  	STANDARD POLISHED BRASS	10	SM CAN    	1469.50	accounts slee
62508	magenta spring frosted navy thistle	Manufacturer#2           	Brand#22  	ECONOMY BURNISHED COPPER	32	SM PACK   	1470.50	as. slyly re
62511	burlywood forest linen puff moccasin	Manufacturer#2           	Brand#23  	LARGE BURNISHED COPPER	9	MED BOX   	1473.51	uests are furio
62512	honeydew antique blush blue metallic	Manufacturer#4           	Brand#42  	STANDARD ANODIZED TIN	23	JUMBO PKG 	1474.51	en accounts sleep
62513	salmon azure seashell slate lime	Manufacturer#2           	Brand#22  	MEDIUM PLATED TIN	47	WRAP JAR  	1475.51	he th
62516	thistle tan maroon blue plum	Manufacturer#3           	Brand#35  	PROMO PLATED TIN	3	WRAP PACK 	1478.51	y final foxes use bl
62517	mint floral medium ghost burlywood	Manufacturer#1           	Brand#12  	LARGE PLATED STEEL	41	JUMBO PACK	1479.51	iously across
62521	burnished ivory black bisque coral	Manufacturer#4           	Brand#42  	PROMO ANODIZED STEEL	34	SM BAG    	1483.52	en pinto
62522	ghost indian deep steel metallic	Manufacturer#3           	Brand#34  	SMALL BURNISHED NICKEL	9	LG BAG    	1484.52	gle slyl
62523	antique snow cornsilk maroon lawn	Manufacturer#2           	Brand#23  	SMALL ANODIZED BRASS	42	WRAP BOX  	1485.52	r packages h
62527	lawn blanched goldenrod blue olive	Manufacturer#2           	Brand#25  	SMALL BURNISHED TIN	11	LG DRUM   	1489.52	ar accounts ha
62528	chartreuse chocolate beige black blush	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED TIN	36	WRAP CAN  	1490.52	accounts a
62529	light steel seashell orchid blush	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	8	JUMBO PKG 	1491.52	cies are
62530	royal thistle spring peru salmon	Manufacturer#4           	Brand#43  	STANDARD POLISHED STEEL	12	SM BOX    	1492.53	. sil
64988	purple cyan deep thistle orange	Manufacturer#3           	Brand#34  	SMALL PLATED NICKEL	12	SM PACK   	1952.98	gular 
64991	dim linen blush maroon navajo	Manufacturer#1           	Brand#11  	LARGE BURNISHED STEEL	5	JUMBO BOX 	1955.99	totes
64993	navy lime frosted medium cornflower	Manufacturer#2           	Brand#23  	PROMO POLISHED STEEL	37	WRAP JAR  	1957.99	he quickly r
64994	lavender aquamarine mint steel tomato	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED BRASS	15	WRAP JAR  	1958.99	haggle careful
64995	maroon misty frosted slate snow	Manufacturer#5           	Brand#51  	PROMO PLATED NICKEL	41	SM BAG    	1959.99	ggle. slyly ironic a
64997	maroon dark navajo black medium	Manufacturer#4           	Brand#43  	ECONOMY POLISHED TIN	2	WRAP CASE 	1961.99	hy asymptote
64998	spring tan antique lemon yellow	Manufacturer#4           	Brand#41  	SMALL ANODIZED TIN	46	SM PKG    	1962.99	e the dinos w
65001	thistle sandy deep olive lavender	Manufacturer#4           	Brand#44  	STANDARD ANODIZED NICKEL	18	LG DRUM   	966.00	aggle 
65003	navy burlywood hot antique papaya	Manufacturer#4           	Brand#45  	MEDIUM PLATED STEEL	29	MED CAN   	968.00	he carefully
65004	blush light gainsboro rosy sienna	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED BRASS	23	SM JAR    	969.00	y above 
65005	violet chiffon magenta cornsilk papaya	Manufacturer#1           	Brand#11  	MEDIUM ANODIZED STEEL	39	JUMBO PKG 	970.00	ic dependencies-
65006	pink grey blush moccasin steel	Manufacturer#1           	Brand#13  	ECONOMY POLISHED TIN	18	WRAP BOX  	971.00	ronic accounts
65012	steel lace coral sandy snow	Manufacturer#5           	Brand#55  	STANDARD BRUSHED BRASS	32	JUMBO DRUM	977.01	e quickly spec
65013	red chartreuse royal brown green	Manufacturer#2           	Brand#24  	LARGE PLATED NICKEL	44	LG PACK   	978.01	sits after the furi
65014	dodger ghost ivory powder olive	Manufacturer#3           	Brand#31  	PROMO PLATED TIN	32	LG JAR    	979.01	he express
65017	cornflower bisque burlywood dodger green	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED BRASS	43	LG DRUM   	982.01	wake blithely alo
65018	floral olive cornsilk midnight red	Manufacturer#4           	Brand#41  	LARGE BURNISHED BRASS	35	WRAP DRUM 	983.01	fily specia
65019	lime pale green cream frosted	Manufacturer#4           	Brand#45  	STANDARD BRUSHED COPPER	23	LG PACK   	984.01	onic sau
65022	beige red linen violet grey	Manufacturer#3           	Brand#35  	PROMO ANODIZED BRASS	7	LG BAG    	987.02	lar ideas cajo
65023	midnight forest orange chocolate beige	Manufacturer#2           	Brand#22  	LARGE POLISHED TIN	23	SM BOX    	988.02	ously regular 
65027	cornflower metallic blue grey violet	Manufacturer#5           	Brand#54  	LARGE BRUSHED BRASS	39	MED CAN   	992.02	carefully express fox
65028	steel salmon orange royal misty	Manufacturer#5           	Brand#52  	SMALL POLISHED BRASS	47	MED BAG   	993.02	hely final theodolites
65029	burlywood tomato salmon navajo antique	Manufacturer#4           	Brand#45  	STANDARD POLISHED STEEL	11	LG CASE   	994.02	 haggle. r
65033	tan cream bisque drab rose	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED TIN	17	SM CASE   	998.03	 bold de
65034	goldenrod indian aquamarine pale slate	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED COPPER	39	JUMBO CAN 	999.03	nstructions. slyly bol
65035	gainsboro plum pink salmon dodger	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED NICKEL	9	LG BOX    	1000.03	cross the fluff
65036	lime dim saddle gainsboro seashell	Manufacturer#4           	Brand#41  	MEDIUM ANODIZED TIN	25	JUMBO BAG 	1001.03	usual, pending
67494	turquoise rosy blush olive wheat	Manufacturer#3           	Brand#35  	LARGE BRUSHED STEEL	14	WRAP CASE 	1461.49	ng the furiou
67497	black drab seashell orchid cyan	Manufacturer#2           	Brand#22  	STANDARD BURNISHED BRASS	17	LG PKG    	1464.49	sly regu
67499	metallic gainsboro dodger aquamarine ivory	Manufacturer#4           	Brand#45  	LARGE PLATED COPPER	6	MED CAN   	1466.49	carefully final acco
67500	powder chocolate sky purple pink	Manufacturer#2           	Brand#23  	SMALL PLATED COPPER	27	LG BAG    	1467.50	ly speci
67501	salmon dodger chiffon snow grey	Manufacturer#5           	Brand#53  	STANDARD POLISHED STEEL	46	WRAP BAG  	1468.50	blithel
67503	gainsboro sandy dark plum tomato	Manufacturer#4           	Brand#41  	ECONOMY PLATED TIN	43	JUMBO BOX 	1470.50	y ironic, f
67504	medium azure lace puff lawn	Manufacturer#1           	Brand#11  	SMALL PLATED TIN	35	JUMBO CAN 	1471.50	 quickly dependen
67507	misty papaya blue navajo coral	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED NICKEL	6	WRAP PACK 	1474.50	 bravely regu
67509	cornsilk lime medium saddle red	Manufacturer#3           	Brand#35  	STANDARD BURNISHED BRASS	41	WRAP BAG  	1476.50	e carefully pending e
67510	misty peach sandy lime midnight	Manufacturer#3           	Brand#34  	LARGE BRUSHED STEEL	50	WRAP DRUM 	1477.51	ccounts. caref
67511	dim chocolate coral steel hot	Manufacturer#4           	Brand#44  	LARGE PLATED TIN	2	JUMBO BAG 	1478.51	along the
67512	bisque navy sky navajo salmon	Manufacturer#2           	Brand#25  	LARGE POLISHED NICKEL	48	MED PACK  	1479.51	he requests. f
67518	tomato purple almond floral pink	Manufacturer#3           	Brand#35  	SMALL BURNISHED BRASS	22	SM BOX    	1485.51	onic foxes. f
67519	lavender sienna turquoise blanched smoke	Manufacturer#5           	Brand#55  	PROMO BURNISHED NICKEL	20	JUMBO BAG 	1486.51	ithel
67520	antique light navy misty floral	Manufacturer#4           	Brand#44  	PROMO BURNISHED COPPER	42	SM JAR    	1487.52	s use evenly acco
67523	sienna wheat misty slate light	Manufacturer#5           	Brand#52  	LARGE BURNISHED BRASS	6	WRAP PKG  	1490.52	y. slyly blit
67524	cream orange cornflower beige blanched	Manufacturer#4           	Brand#43  	SMALL PLATED NICKEL	40	SM JAR    	1491.52	uses. s
67525	dark saddle magenta moccasin puff	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED BRASS	46	MED BOX   	1492.52	 acco
67528	navajo lavender blue honeydew medium	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED TIN	29	SM BOX    	1495.52	press idea
67529	peach ivory tomato azure thistle	Manufacturer#5           	Brand#52  	SMALL BRUSHED COPPER	48	MED CAN   	1496.52	odolites. d
67533	midnight orchid indian peach sienna	Manufacturer#2           	Brand#22  	PROMO BURNISHED STEEL	28	LG PKG    	1500.53	nic acc
67534	ivory deep lavender lemon magenta	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED TIN	16	LG DRUM   	1501.53	ts. acc
67535	salmon firebrick burnished lemon cornflower	Manufacturer#2           	Brand#23  	STANDARD ANODIZED TIN	28	WRAP JAR  	1502.53	lent requests.
67539	peach deep mint plum floral	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	6	JUMBO CAN 	1506.53	es. slyly reg
67540	steel dodger floral azure navy	Manufacturer#5           	Brand#55  	SMALL BRUSHED NICKEL	34	LG BOX    	1507.54	packages. express acc
67541	moccasin aquamarine turquoise olive red	Manufacturer#3           	Brand#31  	LARGE ANODIZED BRASS	17	LG BAG    	1508.54	ged asy
67542	hot sienna sandy plum blue	Manufacturer#5           	Brand#54  	STANDARD BRUSHED COPPER	43	MED CAN   	1509.54	 packages along the 
70000	seashell burnished khaki navajo dodger	Manufacturer#3           	Brand#31  	STANDARD ANODIZED TIN	9	SM DRUM   	970.00	e special, e
70003	salmon lemon metallic seashell pink	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED NICKEL	28	LG JAR    	973.00	blithely pendin
70005	saddle burnished royal burlywood dark	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	4	LG DRUM   	975.00	uriously final
70006	blanched rose brown steel deep	Manufacturer#2           	Brand#24  	STANDARD PLATED COPPER	35	JUMBO BAG 	976.00	ully s
70007	moccasin midnight peru rose blue	Manufacturer#4           	Brand#42  	STANDARD ANODIZED BRASS	9	JUMBO JAR 	977.00	ular deposits 
70009	ghost antique almond papaya saddle	Manufacturer#1           	Brand#11  	MEDIUM BURNISHED STEEL	6	SM DRUM   	979.00	leep slyly
70010	firebrick tan steel sienna plum	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED BRASS	48	MED CASE  	980.01	inal requests nag
70013	magenta hot rosy orchid navajo	Manufacturer#5           	Brand#52  	SMALL BRUSHED BRASS	29	MED BAG   	983.01	le thinly pen
70015	almond honeydew chiffon linen saddle	Manufacturer#4           	Brand#44  	STANDARD BURNISHED NICKEL	35	SM PKG    	985.01	nic foxes wake f
70016	maroon dark pink royal antique	Manufacturer#4           	Brand#43  	PROMO POLISHED STEEL	2	JUMBO JAR 	986.01	furiously. even
70017	white spring purple turquoise misty	Manufacturer#3           	Brand#32  	STANDARD PLATED TIN	37	SM PACK   	987.01	blithely among th
70018	magenta aquamarine blue spring chiffon	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED TIN	45	SM BOX    	988.01	uriously
70024	coral royal sky gainsboro spring	Manufacturer#5           	Brand#51  	PROMO POLISHED COPPER	38	SM PKG    	994.02	lites. regular,
70025	cornsilk peach sienna navajo chiffon	Manufacturer#3           	Brand#32  	SMALL BURNISHED COPPER	21	SM BOX    	995.02	de of 
70026	misty burnished lavender goldenrod sky	Manufacturer#5           	Brand#52  	LARGE BURNISHED STEEL	2	JUMBO JAR 	996.02	sts pro
70029	peru dodger powder burnished orange	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED BRASS	12	WRAP JAR  	999.02	ccounts among the bli
70030	papaya olive floral almond magenta	Manufacturer#2           	Brand#22  	PROMO ANODIZED BRASS	20	LG BAG    	1000.03	r Tiresias 
70031	blue turquoise dodger drab sandy	Manufacturer#2           	Brand#23  	LARGE POLISHED TIN	21	JUMBO CAN 	1001.03	mptotes haggle. care
70034	green linen forest beige pale	Manufacturer#2           	Brand#23  	LARGE ANODIZED BRASS	33	LG PACK   	1004.03	 packa
70035	blush plum bisque moccasin chartreuse	Manufacturer#5           	Brand#54  	PROMO PLATED COPPER	32	LG PACK   	1005.03	thely unusual p
70039	tan turquoise grey plum black	Manufacturer#1           	Brand#15  	PROMO BURNISHED STEEL	27	WRAP PKG  	1009.03	quickly. furiously dog
70040	smoke aquamarine midnight maroon indian	Manufacturer#2           	Brand#21  	SMALL PLATED COPPER	15	JUMBO CASE	1010.04	ges b
70041	maroon sienna medium mint misty	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED BRASS	4	JUMBO BAG 	1011.04	s. foxes after the fu
70045	violet goldenrod snow tomato steel	Manufacturer#2           	Brand#25  	LARGE ANODIZED NICKEL	47	JUMBO BAG 	1015.04	 ironic
70046	ivory mint lemon pink magenta	Manufacturer#2           	Brand#22  	PROMO POLISHED BRASS	19	SM BOX    	1016.04	beans are blit
70047	firebrick powder light blush snow	Manufacturer#2           	Brand#23  	STANDARD BURNISHED NICKEL	26	WRAP BOX  	1017.04	int slyly ironic reque
70048	almond tomato chartreuse dark maroon	Manufacturer#5           	Brand#55  	MEDIUM PLATED COPPER	25	LG PACK   	1018.04	uriously express multi
72479	snow steel spring forest slate	Manufacturer#1           	Brand#14  	PROMO BURNISHED STEEL	31	WRAP DRUM 	1451.47	nusual packa
72482	grey chocolate metallic linen puff	Manufacturer#3           	Brand#31  	STANDARD ANODIZED TIN	10	LG PKG    	1454.48	ly sly accounts impr
72484	orange honeydew snow blush powder	Manufacturer#2           	Brand#21  	STANDARD BURNISHED BRASS	50	WRAP BOX  	1456.48	 wake
72485	orchid chocolate thistle aquamarine spring	Manufacturer#4           	Brand#41  	LARGE BRUSHED BRASS	31	WRAP BAG  	1457.48	according to the 
72486	burnished grey orchid orange cyan	Manufacturer#4           	Brand#45  	SMALL PLATED BRASS	6	SM JAR    	1458.48	y excuses wake regu
72488	sienna navajo tan rose aquamarine	Manufacturer#1           	Brand#11  	PROMO BURNISHED TIN	6	WRAP JAR  	1460.48	 silent pinto beans 
72489	grey smoke beige salmon lace	Manufacturer#5           	Brand#54  	LARGE POLISHED COPPER	29	WRAP PKG  	1461.48	even pa
72492	snow dark lace papaya blue	Manufacturer#2           	Brand#24  	SMALL POLISHED BRASS	17	WRAP BAG  	1464.49	xpress depths
72494	metallic lace smoke white honeydew	Manufacturer#3           	Brand#32  	PROMO BRUSHED NICKEL	6	WRAP CASE 	1466.49	final excuses. quic
72495	yellow spring cream rose coral	Manufacturer#2           	Brand#23  	LARGE BRUSHED NICKEL	20	JUMBO PKG 	1467.49	r pinto beans. 
72496	almond dodger beige midnight antique	Manufacturer#4           	Brand#42  	PROMO BURNISHED STEEL	25	MED BOX   	1468.49	xes haggle pending as
72497	snow sienna cornflower goldenrod frosted	Manufacturer#4           	Brand#44  	MEDIUM BRUSHED TIN	20	WRAP PACK 	1469.49	iet pinto 
72503	dim dark hot peach orchid	Manufacturer#5           	Brand#53  	PROMO BRUSHED COPPER	3	LG CASE   	1475.50	le qu
72504	cream pale blanched lavender snow	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	38	SM BOX    	1476.50	arhorses. p
72505	grey burlywood chiffon red frosted	Manufacturer#1           	Brand#11  	PROMO PLATED TIN	17	LG PKG    	1477.50	ag. fu
72508	chocolate azure salmon orange steel	Manufacturer#5           	Brand#51  	ECONOMY POLISHED BRASS	6	WRAP BAG  	1480.50	efully
72509	thistle violet drab dark puff	Manufacturer#2           	Brand#23  	SMALL BRUSHED COPPER	35	SM CAN    	1481.50	eans affix ca
72510	spring deep pink ivory antique	Manufacturer#5           	Brand#55  	STANDARD BRUSHED COPPER	35	SM DRUM   	1482.51	ain f
72513	rose misty lace red almond	Manufacturer#3           	Brand#31  	PROMO ANODIZED COPPER	48	JUMBO DRUM	1485.51	eans cajole furiou
72514	lawn ivory tan magenta purple	Manufacturer#1           	Brand#15  	LARGE BURNISHED NICKEL	39	SM PACK   	1486.51	nal accounts 
72518	plum brown dodger metallic purple	Manufacturer#5           	Brand#51  	ECONOMY BURNISHED NICKEL	16	SM CASE   	1490.51	he ironic theo
72519	brown sky gainsboro lavender goldenrod	Manufacturer#2           	Brand#21  	LARGE PLATED STEEL	27	MED DRUM  	1491.51	 regular dep
72520	turquoise metallic medium floral grey	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED NICKEL	9	MED JAR   	1492.52	s cajole 
72524	cornflower yellow black olive azure	Manufacturer#3           	Brand#34  	SMALL BURNISHED TIN	16	WRAP JAR  	1496.52	sits affix ac
72525	smoke turquoise powder almond tan	Manufacturer#3           	Brand#35  	ECONOMY POLISHED TIN	35	WRAP BAG  	1497.52	onic excuses c
72526	seashell white sandy misty violet	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED STEEL	22	LG PKG    	1498.52	final, bold acco
72527	pink linen peru chocolate papaya	Manufacturer#5           	Brand#55  	SMALL POLISHED STEEL	35	LG PACK   	1499.52	cross 
74986	moccasin seashell burlywood slate tan	Manufacturer#4           	Brand#43  	STANDARD BURNISHED STEEL	25	MED BOX   	1960.98	 beans 
74989	orange tomato chartreuse green deep	Manufacturer#3           	Brand#31  	ECONOMY BURNISHED NICKEL	5	SM DRUM   	1963.98	odolites. furiou
74991	ivory slate firebrick green chocolate	Manufacturer#2           	Brand#24  	PROMO POLISHED STEEL	4	JUMBO CAN 	1965.99	ronic fox
74992	purple chartreuse aquamarine peach coral	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED STEEL	13	LG BAG    	1966.99	ons. blithely exp
74993	mint navajo rose yellow violet	Manufacturer#1           	Brand#12  	LARGE BURNISHED TIN	19	WRAP JAR  	1967.99	inal accounts boo
74995	cyan moccasin dark orange thistle	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED COPPER	41	LG CASE   	1969.99	cajole about the 
74996	frosted hot lime navy peach	Manufacturer#2           	Brand#22  	STANDARD BRUSHED STEEL	31	SM DRUM   	1970.99	special reque
74999	papaya pink burlywood chiffon navy	Manufacturer#4           	Brand#44  	LARGE POLISHED BRASS	39	JUMBO PACK	1973.99	fully sl
75001	light smoke grey seashell pale	Manufacturer#4           	Brand#44  	PROMO ANODIZED BRASS	9	JUMBO CAN 	976.00	kly sly
75002	cyan thistle snow slate orchid	Manufacturer#5           	Brand#55  	PROMO BRUSHED STEEL	9	SM DRUM   	977.00	even th
75003	olive turquoise peach honeydew lace	Manufacturer#5           	Brand#52  	STANDARD PLATED COPPER	32	MED DRUM  	978.00	. slyly even fox
75004	dodger cornsilk lemon moccasin pink	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED NICKEL	44	JUMBO DRUM	979.00	 specia
75010	chocolate metallic orchid antique maroon	Manufacturer#4           	Brand#41  	ECONOMY POLISHED COPPER	41	MED PACK  	985.01	use above the f
75011	goldenrod blue spring midnight brown	Manufacturer#5           	Brand#54  	STANDARD ANODIZED BRASS	49	WRAP BOX  	986.01	 deposits 
75012	peach tan dodger hot lawn	Manufacturer#1           	Brand#13  	LARGE ANODIZED COPPER	43	LG PACK   	987.01	rthog
75015	violet mint rosy wheat indian	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED NICKEL	19	MED PACK  	990.01	endencies. pending
75016	turquoise midnight maroon green deep	Manufacturer#1           	Brand#14  	LARGE BURNISHED BRASS	7	JUMBO PACK	991.01	ly. asympto
75017	lace coral aquamarine yellow chiffon	Manufacturer#3           	Brand#32  	STANDARD ANODIZED BRASS	29	SM PKG    	992.01	yly brave deposit
75020	midnight snow misty blue violet	Manufacturer#3           	Brand#31  	MEDIUM POLISHED COPPER	44	WRAP CAN  	995.02	furiously regul
75021	sienna pale saddle peru plum	Manufacturer#5           	Brand#52  	SMALL BURNISHED NICKEL	1	WRAP PKG  	996.02	nding re
75025	cream coral sandy azure aquamarine	Manufacturer#1           	Brand#15  	STANDARD ANODIZED TIN	13	JUMBO PKG 	1000.02	ular instructions: fu
75026	plum sienna magenta cornflower cornsilk	Manufacturer#3           	Brand#32  	MEDIUM POLISHED NICKEL	34	JUMBO JAR 	1001.02	furiously express p
75027	floral powder antique firebrick smoke	Manufacturer#1           	Brand#14  	STANDARD BURNISHED STEEL	1	SM CAN    	1002.02	nusual dependencies b
75031	burnished rose forest steel yellow	Manufacturer#2           	Brand#23  	MEDIUM POLISHED NICKEL	39	JUMBO BOX 	1006.03	out the express re
75032	snow orchid medium navajo linen	Manufacturer#4           	Brand#42  	LARGE PLATED NICKEL	25	MED JAR   	1007.03	quick
75033	cornflower lawn powder royal brown	Manufacturer#3           	Brand#35  	STANDARD BURNISHED BRASS	1	JUMBO PACK	1008.03	es boos
75034	frosted black maroon tomato metallic	Manufacturer#3           	Brand#34  	STANDARD BURNISHED COPPER	43	JUMBO JAR 	1009.03	ess pac
77493	gainsboro burnished aquamarine forest black	Manufacturer#1           	Brand#14  	PROMO POLISHED BRASS	48	SM DRUM   	1470.49	ets alongsid
77496	chartreuse honeydew grey misty peach	Manufacturer#1           	Brand#11  	STANDARD PLATED COPPER	48	LG CASE   	1473.49	ly carefully ir
77498	azure light metallic frosted dodger	Manufacturer#5           	Brand#52  	SMALL BRUSHED BRASS	19	LG PACK   	1475.49	 final instructions.
77499	white navy goldenrod ghost burnished	Manufacturer#3           	Brand#32  	MEDIUM BRUSHED TIN	21	LG PACK   	1476.49	s. unusual
77500	brown rose pale peru burnished	Manufacturer#3           	Brand#35  	SMALL ANODIZED TIN	27	SM BAG    	1477.50	l, ironic pinto beans
77502	green dark magenta puff powder	Manufacturer#3           	Brand#34  	PROMO PLATED TIN	24	JUMBO PACK	1479.50	ndencies. slyly
77503	blanched ghost chartreuse peach coral	Manufacturer#3           	Brand#35  	SMALL POLISHED COPPER	15	WRAP CAN  	1480.50	pains lose furiousl
77506	green azure peru cyan navy	Manufacturer#2           	Brand#24  	STANDARD BURNISHED NICKEL	16	LG BAG    	1483.50	unts try to 
77508	grey thistle tan pale floral	Manufacturer#5           	Brand#52  	STANDARD BURNISHED COPPER	13	JUMBO DRUM	1485.50	ans hagg
77509	black indian rosy lemon turquoise	Manufacturer#4           	Brand#44  	STANDARD ANODIZED COPPER	31	WRAP PACK 	1486.50	nusual gifts w
77510	chiffon rose cornflower pink maroon	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED COPPER	49	WRAP BAG  	1487.51	ly unusual accounts 
77511	midnight khaki lime rose puff	Manufacturer#3           	Brand#35  	PROMO BRUSHED TIN	29	SM CASE   	1488.51	y unusual foxes. 
77517	ghost medium navajo dark peach	Manufacturer#2           	Brand#23  	LARGE POLISHED BRASS	20	JUMBO DRUM	1494.51	. platelets cajo
77518	brown cream almond drab cornflower	Manufacturer#2           	Brand#21  	PROMO BRUSHED TIN	33	SM CASE   	1495.51	even excuses. 
77519	mint papaya purple beige spring	Manufacturer#1           	Brand#11  	SMALL BRUSHED NICKEL	11	JUMBO JAR 	1496.51	 acco
77522	orchid snow mint steel bisque	Manufacturer#2           	Brand#22  	SMALL ANODIZED NICKEL	41	JUMBO BAG 	1499.52	r foxes sublate quickl
77523	seashell forest lavender yellow moccasin	Manufacturer#4           	Brand#43  	STANDARD POLISHED COPPER	25	MED DRUM  	1500.52	ss ideas. ca
77524	saddle blue red floral turquoise	Manufacturer#5           	Brand#52  	PROMO BURNISHED COPPER	46	MED BAG   	1501.52	 furiously ironic 
77527	dark saddle forest tomato powder	Manufacturer#5           	Brand#52  	LARGE POLISHED BRASS	15	WRAP JAR  	1504.52	s along the special re
77528	black frosted chiffon white steel	Manufacturer#2           	Brand#21  	ECONOMY BRUSHED NICKEL	36	LG JAR    	1505.52	ans abo
77532	sienna snow dim black navy	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED NICKEL	31	LG JAR    	1509.53	 carefully bold 
77533	black lace ivory tan blanched	Manufacturer#2           	Brand#22  	PROMO BURNISHED NICKEL	8	WRAP DRUM 	1510.53	ses after t
77534	blue magenta linen brown metallic	Manufacturer#5           	Brand#53  	STANDARD ANODIZED BRASS	11	SM DRUM   	1511.53	truct
77538	hot firebrick midnight salmon brown	Manufacturer#2           	Brand#23  	STANDARD POLISHED COPPER	34	JUMBO CAN 	1515.53	lar pinto bean
77539	cornflower pink dodger azure navajo	Manufacturer#1           	Brand#13  	SMALL ANODIZED COPPER	33	WRAP JAR  	1516.53	tions. carefully
77540	navy coral maroon white pink	Manufacturer#4           	Brand#45  	ECONOMY PLATED TIN	46	LG PKG    	1517.54	ang carefully alongsid
77541	orange dodger magenta blue cornflower	Manufacturer#2           	Brand#25  	MEDIUM POLISHED STEEL	30	MED CAN   	1518.54	ns. blithely fin
80000	purple coral medium ghost beige	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	7	JUMBO BOX 	980.00	 blithely 
80003	lemon powder rose purple slate	Manufacturer#2           	Brand#22  	SMALL PLATED BRASS	44	LG BOX    	983.00	ven accounts ca
80005	brown maroon royal pink slate	Manufacturer#2           	Brand#25  	ECONOMY ANODIZED TIN	21	JUMBO CAN 	985.00	 ironic foxes. s
80006	purple green misty orange royal	Manufacturer#5           	Brand#51  	SMALL PLATED TIN	12	JUMBO JAR 	986.00	run slyl
80007	purple cyan pale spring chiffon	Manufacturer#4           	Brand#42  	STANDARD ANODIZED COPPER	50	WRAP DRUM 	987.00	s; courts haggle a
80009	tan chiffon beige linen plum	Manufacturer#5           	Brand#55  	LARGE ANODIZED BRASS	32	WRAP DRUM 	989.00	 pending,
80010	tomato blanched green beige rose	Manufacturer#3           	Brand#34  	PROMO PLATED BRASS	36	MED CAN   	990.01	e quick
80013	dodger seashell brown chocolate beige	Manufacturer#5           	Brand#53  	ECONOMY POLISHED BRASS	2	MED BOX   	993.01	ole furiously thro
80015	olive medium orchid peru honeydew	Manufacturer#1           	Brand#15  	ECONOMY POLISHED BRASS	41	LG BOX    	995.01	n dep
80016	spring cornsilk gainsboro salmon cream	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	34	WRAP DRUM 	996.01	riously 
80017	rosy dim cyan seashell coral	Manufacturer#2           	Brand#23  	LARGE ANODIZED STEEL	38	SM PKG    	997.01	 wake regularly frays.
80018	gainsboro lace plum peach magenta	Manufacturer#3           	Brand#32  	SMALL ANODIZED TIN	26	SM BAG    	998.01	deposits acc
80024	cream cyan grey snow medium	Manufacturer#2           	Brand#25  	SMALL ANODIZED COPPER	23	MED JAR   	1004.02	counts boost
80025	cyan goldenrod floral coral ghost	Manufacturer#4           	Brand#42  	LARGE ANODIZED STEEL	33	MED CASE  	1005.02	egular de
80026	medium moccasin ivory sandy blush	Manufacturer#2           	Brand#21  	STANDARD POLISHED STEEL	17	MED DRUM  	1006.02	ously. furiously
80029	sandy plum medium floral brown	Manufacturer#5           	Brand#52  	LARGE ANODIZED BRASS	23	SM PKG    	1009.02	 pending packages. f
80030	cornflower spring dim plum snow	Manufacturer#5           	Brand#51  	SMALL PLATED TIN	50	WRAP CASE 	1010.03	eas sleep. blithely
80031	blush frosted peach cornflower honeydew	Manufacturer#5           	Brand#54  	LARGE BURNISHED TIN	30	LG PKG    	1011.03	uffily excuses. care
80034	saddle burlywood smoke slate goldenrod	Manufacturer#1           	Brand#13  	SMALL POLISHED STEEL	28	LG CAN    	1014.03	uffily express pla
80035	olive gainsboro moccasin thistle orange	Manufacturer#1           	Brand#12  	LARGE ANODIZED STEEL	38	SM BOX    	1015.03	ermanent fox
80039	chartreuse spring dim navy moccasin	Manufacturer#5           	Brand#53  	SMALL BRUSHED TIN	10	SM BAG    	1019.03	s. carefully pendin
80040	light peru beige tomato thistle	Manufacturer#2           	Brand#25  	PROMO BRUSHED STEEL	18	JUMBO PACK	1020.04	p slyly. caref
80041	plum navy drab sandy lemon	Manufacturer#3           	Brand#34  	STANDARD BRUSHED COPPER	37	LG DRUM   	1021.04	lar d
80045	papaya lace blue pink medium	Manufacturer#2           	Brand#25  	SMALL ANODIZED STEEL	18	LG BOX    	1025.04	ns affix f
80046	aquamarine violet dim chiffon smoke	Manufacturer#3           	Brand#35  	PROMO BRUSHED NICKEL	11	JUMBO JAR 	1026.04	ven packages boost 
80047	beige cornflower grey green cream	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	49	MED BAG   	1027.04	ly final depend
80048	magenta rosy burnished dodger forest	Manufacturer#5           	Brand#54  	LARGE POLISHED TIN	13	SM CASE   	1028.04	nding, 
82476	burnished almond misty dim light	Manufacturer#1           	Brand#13  	LARGE POLISHED NICKEL	7	WRAP JAR  	1458.47	sleep carefully. 
82479	blanched royal almond goldenrod slate	Manufacturer#3           	Brand#31  	STANDARD BRUSHED COPPER	5	JUMBO PACK	1461.47	arefully
82481	smoke thistle blanched medium gainsboro	Manufacturer#5           	Brand#51  	STANDARD ANODIZED STEEL	39	SM DRUM   	1463.48	eposits sleep quickly
82482	royal maroon violet yellow bisque	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED TIN	9	LG CAN    	1464.48	jole caref
82483	burlywood magenta metallic puff dim	Manufacturer#5           	Brand#55  	SMALL ANODIZED NICKEL	22	MED CASE  	1465.48	uests. re
82485	seashell grey coral mint spring	Manufacturer#1           	Brand#11  	ECONOMY PLATED TIN	34	WRAP DRUM 	1467.48	cajole blithely: eve
82486	salmon honeydew steel maroon chocolate	Manufacturer#2           	Brand#24  	PROMO POLISHED TIN	7	JUMBO JAR 	1468.48	nstructions. packages
82489	hot olive cornsilk cornflower tomato	Manufacturer#5           	Brand#54  	SMALL BRUSHED STEEL	30	LG BOX    	1471.48	oost furio
82491	frosted forest chartreuse azure powder	Manufacturer#4           	Brand#44  	LARGE PLATED COPPER	17	SM CASE   	1473.49	 theodolites poach
82492	chiffon cream navajo bisque metallic	Manufacturer#2           	Brand#23  	MEDIUM PLATED STEEL	15	SM PACK   	1474.49	s. closely even 
82493	dim bisque maroon royal moccasin	Manufacturer#4           	Brand#45  	SMALL BURNISHED STEEL	37	JUMBO BOX 	1475.49	ordin
82494	steel lawn firebrick burnished slate	Manufacturer#4           	Brand#45  	MEDIUM POLISHED BRASS	42	LG PACK   	1476.49	ng packag
82500	lawn tomato indian chartreuse thistle	Manufacturer#1           	Brand#12  	LARGE PLATED TIN	39	JUMBO CASE	1482.50	ns. pend
82501	bisque antique purple ghost metallic	Manufacturer#2           	Brand#23  	PROMO POLISHED NICKEL	10	MED PKG   	1483.50	es. slyly unusu
82502	brown almond honeydew gainsboro linen	Manufacturer#4           	Brand#43  	LARGE BURNISHED COPPER	4	SM BAG    	1484.50	. blithely final inst
82505	orchid sienna rose cornflower wheat	Manufacturer#2           	Brand#22  	PROMO POLISHED BRASS	7	LG BOX    	1487.50	sly un
82506	aquamarine purple sky dim orchid	Manufacturer#1           	Brand#12  	PROMO BRUSHED COPPER	39	LG DRUM   	1488.50	bove the 
82507	khaki coral lemon salmon saddle	Manufacturer#2           	Brand#22  	MEDIUM PLATED NICKEL	39	SM CAN    	1489.50	pinto beans. b
82510	chocolate green almond ivory blush	Manufacturer#5           	Brand#55  	SMALL BRUSHED COPPER	11	MED CASE  	1492.51	ctions. doggedly fin
82511	orange slate blue firebrick salmon	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED TIN	43	LG CASE   	1493.51	ar pinto beans. expre
82515	olive salmon bisque puff brown	Manufacturer#5           	Brand#53  	ECONOMY POLISHED COPPER	50	MED PKG   	1497.51	 regular packa
82516	thistle salmon grey chiffon navy	Manufacturer#5           	Brand#53  	STANDARD ANODIZED BRASS	2	WRAP CASE 	1498.51	s haggle s
82517	sandy ivory green lawn magenta	Manufacturer#2           	Brand#23  	PROMO BURNISHED COPPER	15	WRAP BAG  	1499.51	ffily carefully fi
82521	khaki firebrick azure red drab	Manufacturer#5           	Brand#52  	SMALL PLATED BRASS	14	WRAP DRUM 	1503.52	 are slyly e
82522	azure magenta cream mint smoke	Manufacturer#3           	Brand#33  	MEDIUM POLISHED NICKEL	2	SM DRUM   	1504.52	 dolphins are. slyly
82523	slate almond spring salmon cornsilk	Manufacturer#1           	Brand#12  	LARGE PLATED STEEL	3	WRAP BOX  	1505.52	blithely s
82524	sienna cornsilk maroon blanched lemon	Manufacturer#5           	Brand#53  	STANDARD BURNISHED NICKEL	12	SM CAN    	1506.52	sts hinder before the 
84984	lime antique cream blush linen	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED COPPER	28	SM JAR    	1968.98	the pending i
84987	sky ghost salmon navy wheat	Manufacturer#1           	Brand#13  	STANDARD POLISHED TIN	32	JUMBO BOX 	1971.98	uffily fu
84989	tomato chiffon dodger cornflower burnished	Manufacturer#4           	Brand#44  	STANDARD ANODIZED COPPER	30	WRAP PKG  	1973.98	gular 
84990	peach turquoise saddle lace lawn	Manufacturer#4           	Brand#43  	SMALL PLATED TIN	14	SM PACK   	1974.99	carefully 
84991	burlywood red royal spring ghost	Manufacturer#3           	Brand#31  	ECONOMY BRUSHED BRASS	41	SM PACK   	1975.99	 solve alon
84993	rosy snow powder metallic navy	Manufacturer#3           	Brand#31  	PROMO ANODIZED NICKEL	6	SM BAG    	1977.99	epths. ironic, bu
84994	saddle drab azure royal pale	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED TIN	19	MED BAG   	1978.99	kly dogged requests
84997	papaya misty lace spring violet	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED BRASS	44	WRAP PKG  	1981.99	lites are deposits. qu
84999	blush plum honeydew yellow tomato	Manufacturer#5           	Brand#55  	LARGE BRUSHED BRASS	20	LG PKG    	1983.99	furiously fin
85000	sky peach lawn beige dark	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED NICKEL	27	SM CASE   	985.00	ependencies? fl
85001	bisque coral lavender azure spring	Manufacturer#4           	Brand#45  	ECONOMY PLATED COPPER	12	MED CASE  	986.00	ts. furiously 
85002	ghost goldenrod smoke yellow lemon	Manufacturer#2           	Brand#22  	PROMO POLISHED NICKEL	21	WRAP PACK 	987.00	ptotes. final, re
85008	powder blue cream dodger saddle	Manufacturer#2           	Brand#24  	STANDARD POLISHED NICKEL	30	JUMBO DRUM	993.00	s past the bold in
85009	black chocolate rose sky floral	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED NICKEL	40	LG JAR    	994.00	ringly quickly fin
85010	cornflower olive saddle medium chocolate	Manufacturer#5           	Brand#52  	ECONOMY ANODIZED NICKEL	27	JUMBO CASE	995.01	efully even requests 
85013	yellow burnished aquamarine grey magenta	Manufacturer#2           	Brand#24  	PROMO BURNISHED COPPER	40	MED BOX   	998.01	 the carefull
85014	olive rosy midnight dodger slate	Manufacturer#2           	Brand#24  	LARGE POLISHED STEEL	25	LG BAG    	999.01	ly regular
85015	blush ivory forest floral cream	Manufacturer#5           	Brand#53  	SMALL BRUSHED NICKEL	22	MED CASE  	1000.01	d accounts 
85018	aquamarine floral azure frosted bisque	Manufacturer#1           	Brand#13  	ECONOMY BRUSHED TIN	42	WRAP DRUM 	1003.01	nal pack
85019	sky cyan burnished thistle navajo	Manufacturer#1           	Brand#15  	SMALL PLATED NICKEL	14	WRAP PKG  	1004.01	s. bold, ir
85023	saddle orchid midnight mint olive	Manufacturer#5           	Brand#54  	SMALL ANODIZED COPPER	32	SM BOX    	1008.02	egular dugouts 
85024	cornflower azure almond saddle goldenrod	Manufacturer#3           	Brand#34  	STANDARD ANODIZED TIN	27	SM CASE   	1009.02	leep after the reg
85025	bisque firebrick slate coral puff	Manufacturer#2           	Brand#22  	LARGE BRUSHED STEEL	17	SM BOX    	1010.02	tructions are. blit
85029	seashell floral forest tomato grey	Manufacturer#2           	Brand#22  	STANDARD BRUSHED BRASS	23	SM PKG    	1014.02	eposits
85030	saddle chocolate linen dim aquamarine	Manufacturer#3           	Brand#33  	LARGE ANODIZED NICKEL	26	MED BAG   	1015.03	ular de
85031	snow chartreuse ghost lemon lavender	Manufacturer#4           	Brand#44  	MEDIUM ANODIZED NICKEL	13	MED BOX   	1016.03	ckages. deposits wa
85032	moccasin white black turquoise medium	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED STEEL	1	WRAP PKG  	1017.03	 permanent asy
87492	tomato plum light rose smoke	Manufacturer#2           	Brand#25  	MEDIUM POLISHED NICKEL	30	LG CASE   	1479.49	posits wake fluf
87495	peru honeydew dodger navajo khaki	Manufacturer#4           	Brand#43  	PROMO ANODIZED STEEL	9	LG JAR    	1482.49	even accounts. 
87497	sky powder ivory cyan pale	Manufacturer#3           	Brand#32  	MEDIUM BURNISHED BRASS	10	LG BOX    	1484.49	latelets cajole bl
87498	bisque blush ivory salmon snow	Manufacturer#5           	Brand#54  	MEDIUM POLISHED NICKEL	41	SM PKG    	1485.49	ut the furiously
87499	plum drab metallic navy salmon	Manufacturer#3           	Brand#34  	PROMO ANODIZED BRASS	47	MED BOX   	1486.49	lyly regular re
87501	mint olive red puff thistle	Manufacturer#3           	Brand#32  	STANDARD ANODIZED COPPER	14	JUMBO CASE	1488.50	deposits nag slyly
87502	seashell firebrick turquoise snow hot	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED COPPER	16	SM CASE   	1489.50	ly final accounts--
87505	pale floral frosted goldenrod cornsilk	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED TIN	9	WRAP CASE 	1492.50	 slyly unusu
87507	light orange olive ivory moccasin	Manufacturer#2           	Brand#21  	ECONOMY PLATED NICKEL	26	MED CASE  	1494.50	uests. quickly ironic
87508	powder lawn light papaya spring	Manufacturer#5           	Brand#53  	SMALL BRUSHED BRASS	45	MED CASE  	1495.50	gedly 
87509	sandy snow medium metallic gainsboro	Manufacturer#4           	Brand#43  	LARGE BURNISHED NICKEL	24	JUMBO PACK	1496.50	p furiously 
87510	moccasin orchid blanched dark dim	Manufacturer#3           	Brand#35  	ECONOMY PLATED COPPER	7	WRAP DRUM 	1497.51	gular th
87516	pink wheat snow chartreuse green	Manufacturer#4           	Brand#43  	PROMO PLATED COPPER	45	SM PACK   	1503.51	s. doggedly regular
87517	hot green salmon slate olive	Manufacturer#3           	Brand#31  	LARGE PLATED NICKEL	42	MED JAR   	1504.51	press dependencie
87518	saddle black linen sienna mint	Manufacturer#3           	Brand#32  	STANDARD BURNISHED TIN	10	MED BOX   	1505.51	s. quickly ir
87521	frosted black chiffon pink sky	Manufacturer#2           	Brand#23  	MEDIUM BURNISHED COPPER	27	MED BAG   	1508.52	nently final f
87522	linen orchid mint maroon violet	Manufacturer#1           	Brand#15  	SMALL PLATED BRASS	14	WRAP PKG  	1509.52	 slyly even theodolite
87523	smoke cornsilk khaki hot frosted	Manufacturer#3           	Brand#31  	MEDIUM PLATED STEEL	8	WRAP CAN  	1510.52	ckly-
87526	goldenrod brown pale gainsboro wheat	Manufacturer#4           	Brand#43  	STANDARD ANODIZED NICKEL	32	SM BOX    	1513.52	ealthy ideas
87527	tomato lace smoke navy dim	Manufacturer#2           	Brand#24  	PROMO PLATED STEEL	42	JUMBO CAN 	1514.52	ests cajol
87531	violet red light cornflower hot	Manufacturer#3           	Brand#32  	LARGE ANODIZED BRASS	47	MED BOX   	1518.53	t somas. realms arou
87532	dodger cornsilk orchid lavender salmon	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	24	WRAP CAN  	1519.53	 instruct
87533	powder blue grey antique orchid	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED COPPER	40	LG CASE   	1520.53	yly r
87537	midnight yellow snow saddle antique	Manufacturer#5           	Brand#54  	MEDIUM ANODIZED COPPER	27	MED JAR   	1524.53	n blithe
87538	saddle peru chartreuse sky azure	Manufacturer#2           	Brand#25  	LARGE POLISHED BRASS	32	MED PACK  	1525.53	above th
87539	rose azure navajo navy grey	Manufacturer#5           	Brand#52  	MEDIUM POLISHED STEEL	9	SM DRUM   	1526.53	 foxes boost d
87540	cream pale maroon peru snow	Manufacturer#2           	Brand#23  	LARGE BRUSHED NICKEL	18	LG BAG    	1527.54	accounts us
90000	azure pale lawn cornflower black	Manufacturer#5           	Brand#52  	STANDARD BRUSHED BRASS	22	MED BAG   	990.00	s are qui
90003	khaki almond indian cyan spring	Manufacturer#3           	Brand#34  	STANDARD PLATED BRASS	42	WRAP CASE 	993.00	efully 
90005	smoke lace plum chartreuse azure	Manufacturer#5           	Brand#51  	STANDARD BURNISHED COPPER	16	SM BAG    	995.00	le slyly slyly even 
90006	aquamarine maroon sky coral ivory	Manufacturer#1           	Brand#15  	PROMO BURNISHED TIN	16	JUMBO JAR 	996.00	y final accounts ca
90007	firebrick ghost blue spring snow	Manufacturer#4           	Brand#41  	SMALL ANODIZED TIN	5	MED DRUM  	997.00	lly special
90009	pale dark light rosy wheat	Manufacturer#4           	Brand#43  	LARGE PLATED STEEL	2	MED JAR   	999.00	ld the
90010	azure tan powder aquamarine ghost	Manufacturer#1           	Brand#13  	ECONOMY ANODIZED TIN	7	JUMBO PKG 	1000.01	run slyly bes
90013	antique lace red white hot	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED STEEL	18	LG JAR    	1003.01	oxes. carefully spe
90015	ivory burlywood tan dodger forest	Manufacturer#3           	Brand#34  	STANDARD PLATED TIN	38	WRAP DRUM 	1005.01	ic deposits wake fur
90016	misty linen snow lemon metallic	Manufacturer#3           	Brand#33  	LARGE ANODIZED TIN	42	JUMBO JAR 	1006.01	ld excuses. 
90017	tan honeydew lace lavender white	Manufacturer#4           	Brand#43  	SMALL ANODIZED NICKEL	27	MED BOX   	1007.01	refully 
90018	cornflower wheat gainsboro indian yellow	Manufacturer#3           	Brand#33  	PROMO BRUSHED TIN	5	JUMBO DRUM	1008.01	dolphin
90024	blanched smoke medium white honeydew	Manufacturer#1           	Brand#15  	SMALL BURNISHED BRASS	32	LG CAN    	1014.02	arefull
90025	thistle sandy frosted blush khaki	Manufacturer#5           	Brand#52  	STANDARD POLISHED STEEL	21	MED CASE  	1015.02	yly special ins
90026	linen moccasin spring cornflower navajo	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED BRASS	12	JUMBO JAR 	1016.02	nts. quickly s
90029	deep burlywood cornsilk sandy peach	Manufacturer#2           	Brand#24  	SMALL BRUSHED NICKEL	42	MED JAR   	1019.02	manent deposits. 
90030	misty blanched black sandy cornsilk	Manufacturer#1           	Brand#15  	LARGE BURNISHED TIN	35	MED JAR   	1020.03	 fluffily pe
90031	turquoise medium floral grey slate	Manufacturer#3           	Brand#35  	PROMO POLISHED COPPER	32	WRAP PACK 	1021.03	equests boost sly
90034	smoke lime burnished rose antique	Manufacturer#1           	Brand#15  	STANDARD BURNISHED COPPER	12	LG CAN    	1024.03	 pinto 
90035	blue magenta maroon olive puff	Manufacturer#5           	Brand#55  	LARGE POLISHED STEEL	39	JUMBO PKG 	1025.03	ual pack
90039	papaya plum azure lime lemon	Manufacturer#3           	Brand#34  	SMALL BURNISHED TIN	20	WRAP PKG  	1029.03	ctions. carefully eve
90040	orange mint snow royal pink	Manufacturer#1           	Brand#14  	PROMO BURNISHED COPPER	13	SM BAG    	1030.04	ding platelets affix a
90041	aquamarine seashell dim antique forest	Manufacturer#1           	Brand#15  	STANDARD BRUSHED COPPER	7	MED PKG   	1031.04	 bold p
90045	black lime beige chartreuse blue	Manufacturer#1           	Brand#12  	ECONOMY PLATED COPPER	4	JUMBO PKG 	1035.04	deposits sleep above
90046	dark dim indian grey pale	Manufacturer#3           	Brand#31  	STANDARD BURNISHED STEEL	38	LG PACK   	1036.04	unusual r
90047	cornflower salmon ghost orchid frosted	Manufacturer#4           	Brand#45  	SMALL BRUSHED STEEL	22	LG BOX    	1037.04	he foxes. carefull
90048	aquamarine tan papaya royal pale	Manufacturer#2           	Brand#21  	PROMO POLISHED COPPER	39	MED DRUM  	1038.04	lly special asym
92473	cyan azure gainsboro cornsilk rose	Manufacturer#5           	Brand#55  	PROMO ANODIZED STEEL	45	MED BOX   	1465.47	ding to
92476	black linen mint coral chocolate	Manufacturer#1           	Brand#13  	MEDIUM POLISHED STEEL	18	SM CAN    	1468.47	carefully ironi
92478	coral tomato firebrick chocolate brown	Manufacturer#2           	Brand#22  	PROMO BRUSHED BRASS	17	SM BAG    	1470.47	xpress accounts 
92479	seashell khaki linen goldenrod steel	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	12	MED CASE  	1471.47	blithe
92480	deep hot forest saddle orange	Manufacturer#5           	Brand#51  	LARGE ANODIZED BRASS	7	JUMBO CAN 	1472.48	y ideas. carefully 
92482	thistle dark tan turquoise midnight	Manufacturer#1           	Brand#12  	STANDARD ANODIZED NICKEL	17	SM JAR    	1474.48	into beans 
92483	lime deep green khaki yellow	Manufacturer#1           	Brand#14  	PROMO BURNISHED NICKEL	4	WRAP DRUM 	1475.48	, even requests 
92486	ghost moccasin dark dodger azure	Manufacturer#1           	Brand#11  	LARGE PLATED TIN	44	MED BAG   	1478.48	gular foxes after
92488	olive rosy khaki brown antique	Manufacturer#4           	Brand#44  	SMALL POLISHED COPPER	21	WRAP DRUM 	1480.48	heodolites
92489	beige royal dark papaya ghost	Manufacturer#3           	Brand#31  	PROMO BRUSHED BRASS	14	SM CAN    	1481.48	 quickly. pl
92490	medium coral ivory puff light	Manufacturer#4           	Brand#41  	LARGE PLATED TIN	43	MED BOX   	1482.49	ons no
92491	forest floral burlywood black powder	Manufacturer#5           	Brand#53  	STANDARD PLATED COPPER	37	WRAP PACK 	1483.49	as across the furiou
92497	medium lawn royal coral blush	Manufacturer#5           	Brand#54  	ECONOMY BURNISHED STEEL	34	JUMBO BAG 	1489.49	telets. final du
92498	black magenta frosted midnight beige	Manufacturer#3           	Brand#34  	STANDARD BURNISHED BRASS	1	WRAP PACK 	1490.49	g slyly abo
92499	cornflower brown puff linen maroon	Manufacturer#5           	Brand#51  	ECONOMY BRUSHED STEEL	35	MED DRUM  	1491.49	ounts. even fox
92502	linen rosy wheat cornsilk salmon	Manufacturer#3           	Brand#34  	ECONOMY ANODIZED COPPER	9	SM CAN    	1494.50	e thinly special
92503	chiffon violet azure bisque purple	Manufacturer#1           	Brand#12  	SMALL POLISHED NICKEL	41	SM BAG    	1495.50	xcuses cajole blit
92504	orchid beige sky maroon deep	Manufacturer#4           	Brand#44  	ECONOMY PLATED BRASS	33	MED DRUM  	1496.50	s. furio
92507	steel gainsboro cornflower slate ghost	Manufacturer#3           	Brand#32  	SMALL POLISHED TIN	34	LG CAN    	1499.50	into beans.
92508	hot lawn burnished plum slate	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	27	JUMBO CASE	1500.50	ickly e
92512	salmon maroon royal chiffon medium	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED COPPER	35	MED DRUM  	1504.51	. furious pa
92513	royal cornflower deep olive bisque	Manufacturer#4           	Brand#44  	SMALL BRUSHED NICKEL	6	SM CAN    	1505.51	mong the 
92514	frosted orange saddle royal sky	Manufacturer#5           	Brand#52  	ECONOMY POLISHED BRASS	2	LG CASE   	1506.51	ests u
92518	floral medium brown mint moccasin	Manufacturer#5           	Brand#53  	ECONOMY PLATED NICKEL	1	LG BAG    	1510.51	its run slyly. f
92519	deep medium rose slate bisque	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED STEEL	24	LG JAR    	1511.51	hes. 
92520	black burlywood lace rose antique	Manufacturer#1           	Brand#13  	PROMO BURNISHED BRASS	14	MED PKG   	1512.52	ourts wake. accounts 
92521	spring burlywood hot burnished cyan	Manufacturer#1           	Brand#13  	STANDARD POLISHED NICKEL	5	WRAP PKG  	1513.52	final packages. unusu
94982	lace turquoise orchid blush chiffon	Manufacturer#5           	Brand#55  	SMALL ANODIZED TIN	41	WRAP PKG  	1976.98	ounts; slyly bo
94985	forest turquoise coral wheat mint	Manufacturer#2           	Brand#21  	LARGE PLATED COPPER	35	MED BOX   	1979.98	fily pending hocke
94987	floral almond blush spring orchid	Manufacturer#2           	Brand#24  	SMALL POLISHED STEEL	42	JUMBO CASE	1981.98	quests sleep quic
94988	burnished olive sky khaki firebrick	Manufacturer#5           	Brand#54  	STANDARD BRUSHED STEEL	31	LG BAG    	1982.98	eep above the fluff
94989	plum red peru tan burlywood	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED STEEL	10	MED PKG   	1983.98	ges u
94991	azure forest steel snow blue	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED BRASS	28	JUMBO CAN 	1985.99	accou
94992	lavender burnished gainsboro deep orange	Manufacturer#1           	Brand#11  	STANDARD ANODIZED BRASS	23	MED DRUM  	1986.99	quests. blithely re
94995	orchid black lawn brown chartreuse	Manufacturer#5           	Brand#53  	STANDARD BURNISHED TIN	13	JUMBO JAR 	1989.99	endencies. 
94997	burlywood hot floral cream papaya	Manufacturer#1           	Brand#15  	PROMO PLATED STEEL	22	LG CASE   	1991.99	reque
94998	orchid moccasin goldenrod thistle maroon	Manufacturer#4           	Brand#42  	MEDIUM PLATED NICKEL	6	WRAP CASE 	1992.99	s are slow
94999	metallic honeydew cornsilk sky dark	Manufacturer#3           	Brand#33  	MEDIUM POLISHED BRASS	1	JUMBO CASE	1993.99	refully
95000	maroon chiffon violet green wheat	Manufacturer#3           	Brand#33  	STANDARD BURNISHED NICKEL	46	MED DRUM  	995.00	ual foxes w
95006	ivory rose snow light royal	Manufacturer#1           	Brand#12  	MEDIUM PLATED STEEL	44	LG CAN    	1001.00	along the furi
95007	rose medium brown lime midnight	Manufacturer#3           	Brand#32  	SMALL ANODIZED NICKEL	33	LG BAG    	1002.00	e furiously. carefull
95008	medium orange khaki red lavender	Manufacturer#1           	Brand#13  	LARGE BRUSHED COPPER	38	LG PKG    	1003.00	usual reque
95011	rose violet medium cornflower floral	Manufacturer#4           	Brand#44  	ECONOMY BURNISHED TIN	30	LG JAR    	1006.01	sts grow carefull
95012	dodger saddle peach dim bisque	Manufacturer#2           	Brand#24  	STANDARD POLISHED TIN	37	WRAP CAN  	1007.01	ix furiously. clos
95013	drab khaki blue cornflower salmon	Manufacturer#5           	Brand#52  	SMALL BRUSHED TIN	4	LG PKG    	1008.01	usual asymptotes aroun
95016	floral grey peru tan burnished	Manufacturer#5           	Brand#55  	LARGE PLATED COPPER	8	SM CASE   	1011.01	s? slyly bold deposit
95017	metallic blue pink smoke royal	Manufacturer#1           	Brand#14  	LARGE BURNISHED TIN	38	JUMBO PACK	1012.01	ole blithely along
95021	chartreuse cream sky navajo navy	Manufacturer#3           	Brand#34  	MEDIUM POLISHED COPPER	33	LG BAG    	1016.02	y during the
95022	antique lavender green puff almond	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED BRASS	14	MED CASE  	1017.02	 slyly f
95023	orange deep turquoise coral midnight	Manufacturer#1           	Brand#15  	SMALL ANODIZED BRASS	8	WRAP PKG  	1018.02	 slyly accor
95027	spring orchid white burnished rosy	Manufacturer#1           	Brand#12  	LARGE BURNISHED NICKEL	9	WRAP PACK 	1022.02	posits
95028	puff black rose rosy goldenrod	Manufacturer#2           	Brand#24  	ECONOMY BRUSHED NICKEL	44	MED DRUM  	1023.02	unusual packages. slyl
95029	lawn honeydew white snow smoke	Manufacturer#3           	Brand#34  	ECONOMY ANODIZED TIN	4	LG BAG    	1024.02	ts. furiously iro
95030	pink papaya lime sky grey	Manufacturer#3           	Brand#31  	STANDARD BRUSHED NICKEL	8	JUMBO DRUM	1025.03	luffily iro
97491	plum blush pink yellow turquoise	Manufacturer#1           	Brand#12  	PROMO BURNISHED TIN	3	MED JAR   	1488.49	onic instructions aft
97494	wheat coral grey saddle deep	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED TIN	25	MED JAR   	1491.49	c ideas ar
97496	azure gainsboro hot dim light	Manufacturer#3           	Brand#35  	ECONOMY PLATED TIN	42	SM CASE   	1493.49	al co
97497	spring floral peru cream bisque	Manufacturer#4           	Brand#42  	PROMO PLATED NICKEL	4	LG CASE   	1494.49	he dari
97498	burlywood grey deep red powder	Manufacturer#2           	Brand#23  	LARGE BRUSHED COPPER	6	WRAP DRUM 	1495.49	deposits cajole fluff
97500	chocolate cyan plum bisque lime	Manufacturer#1           	Brand#15  	LARGE ANODIZED TIN	32	JUMBO DRUM	1497.50	uffily even theodolite
97501	pale burlywood cornflower blush floral	Manufacturer#2           	Brand#23  	PROMO PLATED STEEL	7	SM PKG    	1498.50	gular packag
97504	brown dark pink maroon almond	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	19	LG BAG    	1501.50	d courts. carefully
97506	khaki mint navajo goldenrod honeydew	Manufacturer#5           	Brand#54  	SMALL ANODIZED TIN	31	JUMBO CAN 	1503.50	y busy foxes 
97507	wheat plum light maroon lace	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED STEEL	28	SM CASE   	1504.50	deposits nag blithely 
97508	chartreuse antique pink lavender light	Manufacturer#4           	Brand#45  	ECONOMY ANODIZED BRASS	39	LG BAG    	1505.50	ackages
97509	chartreuse goldenrod smoke lavender saddle	Manufacturer#5           	Brand#53  	LARGE ANODIZED STEEL	48	SM PKG    	1506.50	ckages print 
97515	seashell smoke plum honeydew turquoise	Manufacturer#5           	Brand#51  	PROMO BURNISHED STEEL	40	JUMBO DRUM	1512.51	counts.
97516	antique black orange gainsboro saddle	Manufacturer#2           	Brand#23  	ECONOMY PLATED COPPER	37	LG PKG    	1513.51	re blithely. carefully
97517	floral smoke cornsilk cream tan	Manufacturer#2           	Brand#23  	STANDARD POLISHED NICKEL	21	LG PKG    	1514.51	tainme
97520	honeydew navajo orange smoke almond	Manufacturer#1           	Brand#15  	PROMO BRUSHED COPPER	33	JUMBO BOX 	1517.52	orges alongside o
97521	green maroon slate almond frosted	Manufacturer#4           	Brand#45  	MEDIUM POLISHED NICKEL	42	WRAP CAN  	1518.52	ges-- pending, f
97522	lavender sienna smoke magenta peru	Manufacturer#2           	Brand#24  	PROMO ANODIZED NICKEL	21	JUMBO BAG 	1519.52	xcuse
97525	sky dark royal lavender steel	Manufacturer#2           	Brand#23  	MEDIUM BRUSHED STEEL	3	MED PACK  	1522.52	ts nag furiously 
97526	gainsboro azure firebrick steel slate	Manufacturer#4           	Brand#42  	ECONOMY BRUSHED NICKEL	10	WRAP DRUM 	1523.52	kages haggle. careful
97530	olive purple puff red violet	Manufacturer#1           	Brand#14  	MEDIUM BURNISHED COPPER	2	LG PACK   	1527.53	t the 
97531	sky brown rose navajo olive	Manufacturer#5           	Brand#52  	ECONOMY POLISHED TIN	41	JUMBO BOX 	1528.53	indle bravely. fluffi
97532	mint violet peach cornflower orange	Manufacturer#3           	Brand#32  	ECONOMY ANODIZED BRASS	43	MED CAN   	1529.53	y bold
97536	tan royal coral yellow antique	Manufacturer#2           	Brand#23  	MEDIUM PLATED STEEL	46	MED JAR   	1533.53	y pendi
97537	dim burnished papaya dark light	Manufacturer#4           	Brand#42  	STANDARD BRUSHED BRASS	25	SM CAN    	1534.53	ronic accoun
97538	yellow almond light lavender bisque	Manufacturer#1           	Brand#11  	LARGE BRUSHED NICKEL	26	MED JAR   	1535.53	ld accounts. ironic pa
97539	drab dark midnight maroon slate	Manufacturer#4           	Brand#45  	ECONOMY POLISHED TIN	24	MED CAN   	1536.53	lar instructi
100000	goldenrod forest salmon white spring	Manufacturer#1           	Brand#15  	LARGE BURNISHED STEEL	17	MED BOX   	1000.00	ies haggle qu
100003	beige powder violet orchid yellow	Manufacturer#2           	Brand#21  	MEDIUM PLATED BRASS	41	SM BOX    	1003.00	 carefully even pac
100005	grey midnight orange peach pale	Manufacturer#2           	Brand#21  	SMALL POLISHED STEEL	7	MED BAG   	1005.00	ajole? blithe
100006	violet sandy olive yellow orange	Manufacturer#4           	Brand#45  	STANDARD BURNISHED COPPER	23	WRAP CASE 	1006.00	he slyly regular pack
100007	snow magenta pale lemon metallic	Manufacturer#1           	Brand#12  	PROMO BURNISHED COPPER	4	MED PKG   	1007.00	ronic accounts in
100009	goldenrod sandy beige hot orange	Manufacturer#3           	Brand#32  	SMALL BURNISHED STEEL	41	WRAP BOX  	1009.00	dinos about the quick
100010	lime lavender slate cream brown	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	19	JUMBO PACK	1010.01	gle slyly above the b
100013	chartreuse metallic bisque sienna papaya	Manufacturer#5           	Brand#52  	STANDARD ANODIZED BRASS	36	MED BAG   	1013.01	ronic, regular reques
100015	lime mint turquoise drab orange	Manufacturer#5           	Brand#53  	PROMO BURNISHED COPPER	11	MED BAG   	1015.01	ake carefully. quickl
100016	moccasin rosy gainsboro beige chiffon	Manufacturer#2           	Brand#25  	STANDARD ANODIZED COPPER	26	LG DRUM   	1016.01	arefully. 
100017	puff grey moccasin misty floral	Manufacturer#3           	Brand#35  	SMALL BRUSHED BRASS	13	JUMBO BOX 	1017.01	lent ideas. regular re
100018	red ivory lavender lemon lime	Manufacturer#1           	Brand#12  	ECONOMY BRUSHED COPPER	36	JUMBO BOX 	1018.01	ites impress s
100024	sandy saddle maroon medium yellow	Manufacturer#2           	Brand#23  	MEDIUM BURNISHED BRASS	21	WRAP BAG  	1024.02	symptote
100025	firebrick cream puff forest antique	Manufacturer#5           	Brand#51  	STANDARD BURNISHED NICKEL	34	MED DRUM  	1025.02	 the fluffily pending
100026	metallic ivory snow floral moccasin	Manufacturer#2           	Brand#22  	LARGE POLISHED STEEL	1	MED CASE  	1026.02	s. carefully blithe id
100029	antique khaki rose rosy medium	Manufacturer#3           	Brand#35  	STANDARD POLISHED STEEL	20	JUMBO CASE	1029.02	ts cajole. furio
100030	lime puff yellow violet tomato	Manufacturer#2           	Brand#25  	SMALL BRUSHED COPPER	12	MED JAR   	1030.03	sual dependencies.
100031	wheat grey chocolate burlywood maroon	Manufacturer#3           	Brand#31  	SMALL ANODIZED STEEL	13	MED PKG   	1031.03	sleep boldly slyly r
100034	pink maroon chocolate midnight indian	Manufacturer#3           	Brand#34  	STANDARD BRUSHED BRASS	32	WRAP BAG  	1034.03	s det
100035	burnished forest royal drab navajo	Manufacturer#3           	Brand#32  	SMALL PLATED COPPER	36	MED JAR   	1035.03	aters. slyly final 
100039	orchid antique salmon mint beige	Manufacturer#3           	Brand#32  	PROMO ANODIZED BRASS	35	SM JAR    	1039.03	 bold t
100040	chiffon ghost magenta linen chartreuse	Manufacturer#5           	Brand#54  	LARGE ANODIZED STEEL	1	WRAP CAN  	1040.04	ages. quickly b
100041	slate tomato smoke hot puff	Manufacturer#3           	Brand#34  	STANDARD BRUSHED TIN	44	SM PACK   	1041.04	ependencies. even pin
100045	red hot grey sky seashell	Manufacturer#3           	Brand#32  	SMALL ANODIZED NICKEL	44	SM PKG    	1045.04	lets snoo
100046	snow spring forest seashell antique	Manufacturer#1           	Brand#11  	PROMO BURNISHED STEEL	41	SM BOX    	1046.04	ns accordin
100047	salmon yellow red blue lawn	Manufacturer#5           	Brand#53  	SMALL POLISHED TIN	36	JUMBO BOX 	1047.04	al accounts na
100048	powder coral orange wheat rosy	Manufacturer#3           	Brand#31  	LARGE ANODIZED STEEL	31	LG BOX    	1048.04	eposits. careful
102470	forest peach indian red misty	Manufacturer#5           	Brand#55  	STANDARD POLISHED TIN	1	MED BAG   	1472.47	orbits are furious
102473	azure thistle firebrick slate burnished	Manufacturer#4           	Brand#41  	PROMO POLISHED STEEL	50	LG PACK   	1475.47	s across the carefu
102475	mint cyan moccasin ivory peach	Manufacturer#3           	Brand#32  	LARGE BRUSHED NICKEL	18	WRAP PKG  	1477.47	st among the 
102476	lavender hot powder blanched aquamarine	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	26	LG CAN    	1478.47	lar accounts promise s
102477	thistle cream misty khaki green	Manufacturer#2           	Brand#25  	LARGE PLATED TIN	19	JUMBO PKG 	1479.47	ests to t
102479	cyan puff forest dim mint	Manufacturer#2           	Brand#24  	PROMO BURNISHED NICKEL	48	LG CASE   	1481.47	egular grouches
102480	snow midnight sienna tan plum	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED NICKEL	16	LG BAG    	1482.48	tes are furiously. e
102483	white antique rose lavender rosy	Manufacturer#2           	Brand#23  	STANDARD BURNISHED BRASS	25	LG BOX    	1485.48	g patterns
102485	aquamarine frosted moccasin antique grey	Manufacturer#4           	Brand#43  	SMALL ANODIZED NICKEL	10	SM BOX    	1487.48	ial deposits.
102486	chartreuse ivory smoke grey hot	Manufacturer#1           	Brand#11  	LARGE PLATED COPPER	42	SM JAR    	1488.48	sts. r
102487	thistle bisque pink cornflower rosy	Manufacturer#2           	Brand#24  	ECONOMY BURNISHED TIN	7	MED PACK  	1489.48	nic deposits wak
102488	sky navy chartreuse magenta antique	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED BRASS	4	JUMBO CASE	1490.48	earls. q
102494	blush aquamarine yellow saddle papaya	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED STEEL	22	SM BAG    	1496.49	oss the final, regu
102495	tan forest smoke puff rosy	Manufacturer#2           	Brand#23  	STANDARD BRUSHED TIN	6	WRAP CAN  	1497.49	affix ironically i
102496	black goldenrod navy lavender chiffon	Manufacturer#2           	Brand#21  	STANDARD BRUSHED STEEL	35	LG DRUM   	1498.49	ly. packages c
102499	medium azure sienna slate deep	Manufacturer#1           	Brand#13  	STANDARD BURNISHED STEEL	31	SM JAR    	1501.49	usly blithely regu
102500	floral cornsilk cyan dark chartreuse	Manufacturer#5           	Brand#51  	PROMO ANODIZED BRASS	33	MED JAR   	1502.50	hely fina
102501	wheat gainsboro aquamarine cornsilk coral	Manufacturer#5           	Brand#55  	PROMO BURNISHED COPPER	8	MED BAG   	1503.50	 slyly final accounts
102504	ghost sky slate chocolate sandy	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED BRASS	25	MED CASE  	1506.50	olphins bel
102505	green magenta dodger metallic red	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED COPPER	13	JUMBO BOX 	1507.50	even deposits a
102509	beige navy aquamarine blue cornsilk	Manufacturer#1           	Brand#14  	SMALL BRUSHED STEEL	33	MED CAN   	1511.50	es poach iron
102510	floral chiffon peru blush lavender	Manufacturer#2           	Brand#25  	ECONOMY PLATED STEEL	49	SM BAG    	1512.51	express requests. regu
102511	magenta dodger puff ivory olive	Manufacturer#2           	Brand#21  	LARGE BURNISHED STEEL	45	SM CASE   	1513.51	iously 
102515	red almond grey beige sandy	Manufacturer#3           	Brand#32  	SMALL PLATED BRASS	46	SM CASE   	1517.51	regular pinto bea
102516	lemon moccasin burnished steel beige	Manufacturer#3           	Brand#32  	SMALL PLATED TIN	1	LG BOX    	1518.51	ithely express packag
102517	tomato navy forest midnight blush	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED NICKEL	9	SM JAR    	1519.51	he quickly un
102518	indian cornflower honeydew turquoise orchid	Manufacturer#4           	Brand#44  	STANDARD BRUSHED STEEL	8	MED PACK  	1520.51	, silent accounts. bli
104980	green linen azure ivory navy	Manufacturer#4           	Brand#44  	SMALL PLATED NICKEL	38	MED CAN   	1984.98	excus
104983	tomato moccasin bisque slate midnight	Manufacturer#1           	Brand#14  	SMALL POLISHED TIN	44	LG BOX    	1987.98	pecial, unus
104985	peru cornsilk lime papaya pale	Manufacturer#2           	Brand#25  	PROMO PLATED COPPER	43	MED BOX   	1989.98	symptotes 
104986	midnight wheat cornflower sky lemon	Manufacturer#2           	Brand#21  	ECONOMY PLATED STEEL	50	JUMBO PKG 	1990.98	counts w
104987	blue lemon thistle dim mint	Manufacturer#4           	Brand#42  	SMALL PLATED STEEL	44	LG PACK   	1991.98	cajole fu
104989	smoke maroon cyan navy mint	Manufacturer#4           	Brand#42  	MEDIUM POLISHED TIN	45	WRAP PACK 	1993.98	kly final reque
104990	sky hot burnished green brown	Manufacturer#5           	Brand#55  	LARGE BRUSHED COPPER	9	LG CAN    	1994.99	pending deposit
104993	medium antique metallic ghost dim	Manufacturer#4           	Brand#41  	LARGE BURNISHED NICKEL	14	SM PACK   	1997.99	refull
104995	pink floral medium lawn chocolate	Manufacturer#5           	Brand#54  	STANDARD PLATED COPPER	38	JUMBO BOX 	1999.99	! blithe
104996	deep navajo khaki snow navy	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED NICKEL	18	WRAP CAN  	2000.99	ests. care
104997	violet firebrick lace royal white	Manufacturer#3           	Brand#31  	ECONOMY POLISHED BRASS	21	WRAP JAR  	2001.99	nding pinto beans. 
104998	linen sienna cornflower gainsboro dim	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED STEEL	45	WRAP BAG  	2002.99	ns. fina
105004	medium purple slate snow brown	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED STEEL	45	LG PKG    	1009.00	ly special 
105005	spring aquamarine azure lime sky	Manufacturer#3           	Brand#32  	LARGE POLISHED BRASS	47	MED PACK  	1010.00	osits sleep 
105006	sandy mint medium navajo orchid	Manufacturer#2           	Brand#24  	PROMO POLISHED NICKEL	39	WRAP BOX  	1011.00	lly. special braid
105009	orchid blush cream cyan linen	Manufacturer#5           	Brand#53  	ECONOMY PLATED TIN	16	LG PKG    	1014.00	ans wake quickly a
105010	blanched orchid ghost plum cornsilk	Manufacturer#1           	Brand#13  	PROMO BRUSHED NICKEL	14	WRAP BAG  	1015.01	ts. carefully ir
105011	dim coral pale purple rosy	Manufacturer#3           	Brand#35  	SMALL ANODIZED COPPER	4	WRAP BOX  	1016.01	foxes. slyly bold 
105014	blush grey puff white papaya	Manufacturer#4           	Brand#45  	SMALL PLATED STEEL	16	SM CAN    	1019.01	furiously re
105015	chocolate khaki green rosy deep	Manufacturer#3           	Brand#31  	SMALL POLISHED TIN	47	JUMBO DRUM	1020.01	lar deposits after t
105019	metallic peru powder thistle aquamarine	Manufacturer#2           	Brand#21  	STANDARD PLATED NICKEL	26	JUMBO CASE	1024.01	. slyly pen
105020	wheat dark lace antique goldenrod	Manufacturer#4           	Brand#42  	STANDARD BRUSHED COPPER	10	LG PACK   	1025.02	 regular pinto b
105021	chocolate lemon pale frosted purple	Manufacturer#5           	Brand#52  	ECONOMY BRUSHED TIN	12	JUMBO CAN 	1026.02	packages. ca
105025	blue mint puff azure rose	Manufacturer#5           	Brand#51  	PROMO PLATED BRASS	28	JUMBO BAG 	1030.02	 are quickly even s
105026	coral cyan blush azure rosy	Manufacturer#3           	Brand#32  	STANDARD PLATED NICKEL	6	SM BAG    	1031.02	ckly pending ideas 
105027	metallic green maroon papaya lawn	Manufacturer#5           	Brand#55  	MEDIUM BRUSHED STEEL	34	SM CAN    	1032.02	p among the quickly
105028	red goldenrod navajo burnished indian	Manufacturer#2           	Brand#24  	STANDARD ANODIZED COPPER	33	WRAP BAG  	1033.02	ngside of th
107490	honeydew sienna black orchid lime	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	5	LG PACK   	1497.49	ackag
107493	maroon khaki mint puff salmon	Manufacturer#3           	Brand#33  	STANDARD BRUSHED TIN	20	SM BAG    	1500.49	ntain fluffily 
107495	dodger olive salmon cyan azure	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	24	WRAP BAG  	1502.49	riously until the care
107496	salmon hot navy navajo sandy	Manufacturer#5           	Brand#51  	PROMO ANODIZED COPPER	9	LG CAN    	1503.49	t the slyly silent s
107497	lavender bisque sandy rosy aquamarine	Manufacturer#4           	Brand#44  	PROMO PLATED TIN	37	SM CAN    	1504.49	osits h
107499	floral tan ivory bisque ghost	Manufacturer#3           	Brand#34  	ECONOMY PLATED BRASS	40	WRAP CAN  	1506.49	s cajole slyly careful
107500	aquamarine tan pink white firebrick	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED NICKEL	50	SM BOX    	1507.50	ly pending 
107503	purple slate spring cornflower black	Manufacturer#1           	Brand#13  	STANDARD POLISHED STEEL	31	SM BOX    	1510.50	lent deposits. bo
107505	moccasin maroon hot pink honeydew	Manufacturer#3           	Brand#34  	MEDIUM POLISHED STEEL	42	JUMBO CAN 	1512.50	c, silent 
107506	dim lime thistle pale tomato	Manufacturer#4           	Brand#43  	PROMO POLISHED STEEL	8	JUMBO PKG 	1513.50	 according t
107507	brown drab coral goldenrod saddle	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED STEEL	37	JUMBO DRUM	1514.50	he quick, thin 
107508	powder smoke puff slate rose	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED COPPER	19	JUMBO PKG 	1515.50	lay furi
107514	chocolate lavender moccasin cyan tomato	Manufacturer#3           	Brand#31  	ECONOMY ANODIZED NICKEL	31	JUMBO BAG 	1521.51	fully ironic 
107515	papaya cyan rose lawn almond	Manufacturer#1           	Brand#14  	PROMO ANODIZED BRASS	25	LG DRUM   	1522.51	dogge
107516	grey ghost sienna lavender smoke	Manufacturer#1           	Brand#12  	STANDARD BURNISHED COPPER	12	MED JAR   	1523.51	ously final req
107519	lawn cyan burlywood navy black	Manufacturer#5           	Brand#54  	PROMO BURNISHED TIN	17	JUMBO PKG 	1526.51	foxes haggle
107520	red wheat ivory green orchid	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED COPPER	6	SM CASE   	1527.52	alms. 
107521	navajo grey salmon goldenrod pale	Manufacturer#4           	Brand#41  	STANDARD BRUSHED TIN	32	LG BAG    	1528.52	ckly 
107524	olive lavender sienna orange indian	Manufacturer#1           	Brand#12  	PROMO BRUSHED BRASS	3	LG DRUM   	1531.52	uickly 
107525	plum blush grey dim sky	Manufacturer#1           	Brand#12  	MEDIUM ANODIZED BRASS	17	SM JAR    	1532.52	 the carefully e
107529	royal blanched hot puff sandy	Manufacturer#5           	Brand#55  	LARGE BRUSHED COPPER	14	JUMBO CASE	1536.52	express 
107530	antique sandy magenta royal slate	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED NICKEL	1	WRAP CASE 	1537.53	nal, even p
107531	orchid maroon thistle midnight pink	Manufacturer#4           	Brand#45  	LARGE ANODIZED STEEL	7	WRAP PKG  	1538.53	even deposit
107535	sandy metallic green olive seashell	Manufacturer#5           	Brand#51  	PROMO ANODIZED COPPER	38	JUMBO PKG 	1542.53	ong the ca
107536	maroon azure navy dark forest	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED NICKEL	35	WRAP PACK 	1543.53	. iron
107537	sandy orchid goldenrod maroon linen	Manufacturer#3           	Brand#33  	LARGE PLATED BRASS	10	LG DRUM   	1544.53	hely expres
107538	burnished sky turquoise khaki aquamarine	Manufacturer#3           	Brand#31  	PROMO ANODIZED NICKEL	47	LG BAG    	1545.53	ng to the
110000	tomato linen royal turquoise seashell	Manufacturer#5           	Brand#54  	LARGE PLATED NICKEL	24	MED CAN   	1010.00	olphins at t
110003	forest seashell green powder azure	Manufacturer#3           	Brand#31  	LARGE PLATED BRASS	41	LG CASE   	1013.00	egular accounts ca
110005	dodger medium red indian brown	Manufacturer#5           	Brand#53  	STANDARD BURNISHED COPPER	47	SM PKG    	1015.00	- special ideas acc
110006	burnished plum almond orchid magenta	Manufacturer#5           	Brand#53  	LARGE BURNISHED BRASS	38	SM PKG    	1016.00	lly iro
110007	hot drab seashell pale black	Manufacturer#5           	Brand#55  	STANDARD ANODIZED STEEL	8	JUMBO PACK	1017.00	y iro
110009	burlywood plum spring salmon seashell	Manufacturer#4           	Brand#41  	PROMO PLATED COPPER	22	LG JAR    	1019.00	e special theodolite
110010	dark tan ivory mint misty	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	30	WRAP BOX  	1020.01	 across the carefully 
110013	beige khaki rose hot violet	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED TIN	5	WRAP PKG  	1023.01	ular e
110015	orange moccasin rosy navajo lime	Manufacturer#5           	Brand#53  	ECONOMY POLISHED STEEL	26	JUMBO CAN 	1025.01	ptotes. blithely final
110016	purple tomato sky honeydew metallic	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED NICKEL	14	JUMBO CASE	1026.01	 accounts
110017	lemon chartreuse black navy snow	Manufacturer#3           	Brand#34  	PROMO ANODIZED COPPER	5	MED DRUM  	1027.01	ourts boo
110018	puff chiffon cyan dim forest	Manufacturer#1           	Brand#15  	MEDIUM PLATED NICKEL	13	JUMBO BAG 	1028.01	erman
110024	blue pale sandy light navajo	Manufacturer#5           	Brand#53  	STANDARD BURNISHED TIN	31	SM BAG    	1034.02	nts. ironic theodo
110025	tan chocolate salmon chiffon violet	Manufacturer#3           	Brand#31  	LARGE BURNISHED BRASS	6	LG PACK   	1035.02	ns. furiously bold
110026	firebrick chartreuse navy lace yellow	Manufacturer#1           	Brand#12  	LARGE BURNISHED BRASS	14	LG BOX    	1036.02	riously carefully p
110029	lavender slate beige purple tan	Manufacturer#1           	Brand#11  	ECONOMY BURNISHED NICKEL	35	LG PACK   	1039.02	 instructions. packag
110030	dodger puff pale snow gainsboro	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED COPPER	6	SM BAG    	1040.03	lphins cajole
110031	light violet deep antique midnight	Manufacturer#2           	Brand#22  	STANDARD ANODIZED COPPER	9	WRAP PACK 	1041.03	ully pending plate
110034	purple seashell rosy tomato ghost	Manufacturer#2           	Brand#22  	PROMO BRUSHED NICKEL	48	LG CASE   	1044.03	uriou
110035	dark cream sky medium thistle	Manufacturer#4           	Brand#45  	LARGE ANODIZED COPPER	28	WRAP JAR  	1045.03	 cajole. fluf
110039	turquoise black wheat floral bisque	Manufacturer#2           	Brand#24  	LARGE BURNISHED STEEL	31	SM CAN    	1049.03	 bold deposits detect
110040	cyan lime thistle spring peru	Manufacturer#5           	Brand#51  	STANDARD ANODIZED STEEL	22	MED PKG   	1050.04	s integrate slyly f
110041	azure hot wheat lawn peach	Manufacturer#5           	Brand#55  	PROMO PLATED TIN	40	SM BOX    	1051.04	ajole carefully f
110045	drab yellow seashell purple beige	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED NICKEL	48	JUMBO BAG 	1055.04	arefully ironic depos
110046	green blue dim beige chocolate	Manufacturer#4           	Brand#42  	MEDIUM PLATED NICKEL	28	JUMBO CASE	1056.04	uickly e
110047	red salmon bisque forest turquoise	Manufacturer#2           	Brand#25  	MEDIUM POLISHED COPPER	19	JUMBO BOX 	1057.04	ng plate
110048	salmon cyan rose moccasin chocolate	Manufacturer#4           	Brand#43  	ECONOMY ANODIZED TIN	5	WRAP PKG  	1058.04	e stealthy foxes. r
112467	metallic azure sienna dodger mint	Manufacturer#2           	Brand#22  	MEDIUM PLATED BRASS	22	WRAP PACK 	1479.46	ackages. 
112470	ivory cream honeydew grey brown	Manufacturer#4           	Brand#44  	PROMO POLISHED STEEL	21	LG BOX    	1482.47	ular 
112472	pale spring beige snow navajo	Manufacturer#1           	Brand#14  	PROMO BRUSHED COPPER	30	MED PKG   	1484.47	ions wake 
112473	thistle peach black firebrick light	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	43	WRAP CASE 	1485.47	gle quick
112474	slate steel cream lawn olive	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED BRASS	46	WRAP PKG  	1486.47	lar, even 
112476	slate peach light deep seashell	Manufacturer#5           	Brand#52  	SMALL ANODIZED NICKEL	7	JUMBO JAR 	1488.47	about the slyl
112477	honeydew blue plum tomato lace	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED TIN	40	WRAP BAG  	1489.47	 the carefully reg
112480	white indian purple coral forest	Manufacturer#1           	Brand#12  	STANDARD POLISHED NICKEL	9	JUMBO PACK	1492.48	uests. carefully 
112482	slate hot wheat orchid floral	Manufacturer#5           	Brand#55  	SMALL PLATED NICKEL	11	WRAP BOX  	1494.48	 even
112483	spring frosted wheat mint metallic	Manufacturer#4           	Brand#42  	SMALL BURNISHED TIN	50	WRAP PACK 	1495.48	ular r
112484	forest almond orange cornsilk peru	Manufacturer#4           	Brand#42  	SMALL ANODIZED STEEL	5	SM CAN    	1496.48	along the
112485	black orchid navy rosy grey	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED NICKEL	1	SM DRUM   	1497.48	ckages play
112491	pink mint drab forest violet	Manufacturer#1           	Brand#13  	SMALL POLISHED STEEL	12	JUMBO DRUM	1503.49	nticingl
112492	royal mint plum peru sky	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED STEEL	13	LG CAN    	1504.49	furiously spe
112493	yellow indian blue linen plum	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED STEEL	26	JUMBO BOX 	1505.49	deposits w
112496	grey chocolate pink bisque indian	Manufacturer#2           	Brand#21  	LARGE PLATED TIN	13	SM BOX    	1508.49	mong the idle
112497	navajo red lime mint honeydew	Manufacturer#2           	Brand#23  	ECONOMY ANODIZED BRASS	14	LG CASE   	1509.49	ently regular pat
112498	forest green magenta plum papaya	Manufacturer#3           	Brand#35  	MEDIUM PLATED STEEL	2	WRAP DRUM 	1510.49	usly fi
112501	burnished indian cream orange frosted	Manufacturer#5           	Brand#51  	ECONOMY BURNISHED COPPER	6	JUMBO JAR 	1513.50	 waters integrate
112502	gainsboro white sky maroon papaya	Manufacturer#2           	Brand#24  	LARGE POLISHED BRASS	41	SM CAN    	1514.50	y unusual 
112506	aquamarine white red blue antique	Manufacturer#3           	Brand#33  	PROMO BURNISHED STEEL	24	JUMBO CASE	1518.50	gular i
112507	steel orchid bisque ivory frosted	Manufacturer#2           	Brand#21  	SMALL ANODIZED STEEL	7	JUMBO CASE	1519.50	ons; fin
112508	metallic dodger magenta spring burlywood	Manufacturer#2           	Brand#21  	ECONOMY BRUSHED COPPER	10	MED CASE  	1520.50	structions use furio
112512	azure chiffon rosy light peach	Manufacturer#4           	Brand#44  	STANDARD PLATED NICKEL	21	MED JAR   	1524.51	grate furiously pen
112513	snow honeydew cornflower olive floral	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED NICKEL	33	MED PKG   	1525.51	uests are fluff
112514	honeydew beige rose steel moccasin	Manufacturer#1           	Brand#12  	SMALL ANODIZED STEEL	46	WRAP DRUM 	1526.51	ously regul
112515	chiffon puff pale gainsboro peru	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED STEEL	45	LG BAG    	1527.51	ges boo
114978	smoke tan maroon dark metallic	Manufacturer#5           	Brand#51  	SMALL PLATED TIN	47	JUMBO BOX 	1992.97	press 
114981	bisque slate puff plum dodger	Manufacturer#3           	Brand#31  	STANDARD ANODIZED STEEL	13	MED JAR   	1995.98	uickl
114983	blue seashell smoke chiffon chartreuse	Manufacturer#4           	Brand#43  	SMALL BRUSHED TIN	48	WRAP BAG  	1997.98	deposits? caref
114984	honeydew navy floral gainsboro smoke	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED COPPER	18	JUMBO JAR 	1998.98	as are blithely even,
114985	dim pale magenta turquoise linen	Manufacturer#1           	Brand#14  	STANDARD BURNISHED NICKEL	30	MED BAG   	1999.98	leep again
114987	ivory papaya salmon antique white	Manufacturer#1           	Brand#12  	STANDARD BURNISHED BRASS	21	LG PKG    	2001.98	, special requests
114988	steel dim beige forest green	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED COPPER	23	WRAP CASE 	2002.98	ackages. s
114991	hot maroon olive yellow burlywood	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED TIN	42	SM BOX    	2005.99	ans use abou
114993	tomato cyan sandy beige lace	Manufacturer#2           	Brand#24  	SMALL PLATED COPPER	24	SM DRUM   	2007.99	ts. furiously special
114994	aquamarine chocolate sienna seashell orange	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED COPPER	11	JUMBO BAG 	2008.99	 Tiresias na
114995	royal tomato sky antique coral	Manufacturer#4           	Brand#42  	MEDIUM PLATED BRASS	42	WRAP PACK 	2009.99	e furiously even d
114996	puff lace saddle ivory powder	Manufacturer#1           	Brand#14  	LARGE BRUSHED STEEL	47	SM PKG    	2010.99	ckages wake caref
115002	mint antique lawn cornsilk salmon	Manufacturer#5           	Brand#53  	LARGE ANODIZED TIN	29	JUMBO CAN 	1017.00	sual fox
115003	navy deep tomato sienna smoke	Manufacturer#4           	Brand#44  	SMALL PLATED STEEL	23	JUMBO JAR 	1018.00	inal ac
115004	orange olive lime royal floral	Manufacturer#3           	Brand#33  	STANDARD ANODIZED STEEL	45	LG JAR    	1019.00	count
115007	cornsilk midnight lace chartreuse floral	Manufacturer#3           	Brand#32  	ECONOMY BURNISHED STEEL	37	JUMBO BAG 	1022.00	hely final depo
115008	maroon indian beige blush blue	Manufacturer#1           	Brand#15  	LARGE POLISHED COPPER	47	JUMBO PKG 	1023.00	ncies. pi
115009	moccasin smoke sandy aquamarine tomato	Manufacturer#4           	Brand#41  	PROMO PLATED NICKEL	1	WRAP PKG  	1024.00	. furiously final 
115012	dark puff olive hot sienna	Manufacturer#4           	Brand#45  	LARGE POLISHED STEEL	32	WRAP JAR  	1027.01	l deposits sleep
115013	ghost burnished pale spring deep	Manufacturer#2           	Brand#25  	PROMO PLATED TIN	36	JUMBO DRUM	1028.01	ave to eat 
115017	steel midnight rose bisque mint	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED STEEL	23	LG BAG    	1032.01	nusual t
115018	blue tan mint white dark	Manufacturer#3           	Brand#34  	STANDARD ANODIZED NICKEL	18	SM BAG    	1033.01	 requests. furi
115019	dark steel spring aquamarine red	Manufacturer#5           	Brand#54  	MEDIUM PLATED COPPER	30	JUMBO JAR 	1034.01	ole sl
115023	purple forest thistle turquoise midnight	Manufacturer#1           	Brand#13  	SMALL ANODIZED NICKEL	17	MED CASE  	1038.02	al ideas
115024	seashell azure peach midnight coral	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED COPPER	3	WRAP BAG  	1039.02	d foxes-- packages alo
115025	purple drab steel puff dark	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED COPPER	12	MED CASE  	1040.02	ymptotes.
115026	metallic dark grey chiffon burnished	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED TIN	29	LG BOX    	1041.02	 iron
117489	grey saddle light antique orchid	Manufacturer#2           	Brand#21  	PROMO BURNISHED STEEL	41	SM PACK   	1506.48	n pinto beans w
117492	ivory forest tomato azure chiffon	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED STEEL	38	WRAP DRUM 	1509.49	uffil
117494	plum magenta linen bisque metallic	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	3	SM CASE   	1511.49	sual pinto be
117495	burlywood papaya olive yellow navajo	Manufacturer#4           	Brand#44  	MEDIUM POLISHED STEEL	5	MED DRUM  	1512.49	press pinto beans. 
117496	misty frosted maroon moccasin hot	Manufacturer#4           	Brand#43  	SMALL PLATED STEEL	45	JUMBO JAR 	1513.49	gular
117498	lace pale dim steel azure	Manufacturer#4           	Brand#43  	MEDIUM POLISHED COPPER	26	MED BAG   	1515.49	nto beans after 
117499	navy puff red plum lime	Manufacturer#4           	Brand#45  	LARGE BRUSHED STEEL	46	LG JAR    	1516.49	efully
117502	indian green orange ghost azure	Manufacturer#2           	Brand#25  	SMALL PLATED BRASS	42	SM PKG    	1519.50	final th
117504	blush sandy spring cyan hot	Manufacturer#1           	Brand#13  	STANDARD ANODIZED STEEL	22	SM PKG    	1521.50	ns acro
117505	royal black slate lace navajo	Manufacturer#4           	Brand#41  	SMALL POLISHED NICKEL	24	JUMBO JAR 	1522.50	kly unusual p
117506	metallic cyan tomato snow white	Manufacturer#4           	Brand#41  	MEDIUM PLATED BRASS	26	WRAP DRUM 	1523.50	o bea
117507	drab lavender lime papaya orange	Manufacturer#1           	Brand#12  	PROMO BRUSHED COPPER	34	JUMBO CASE	1524.50	 pinto beans. regu
117513	tomato ivory chartreuse spring misty	Manufacturer#5           	Brand#51  	PROMO BURNISHED STEEL	4	LG CASE   	1530.51	 blith
117514	beige ivory lavender ghost lemon	Manufacturer#1           	Brand#12  	PROMO BURNISHED TIN	28	MED BOX   	1531.51	 deposits. blithe
117515	dim medium olive drab smoke	Manufacturer#4           	Brand#45  	PROMO BRUSHED TIN	13	JUMBO DRUM	1532.51	tions wa
117518	tomato ghost ivory bisque antique	Manufacturer#3           	Brand#35  	LARGE PLATED BRASS	30	MED CASE  	1535.51	e special, e
117519	chartreuse pink wheat dark cream	Manufacturer#4           	Brand#45  	MEDIUM PLATED BRASS	41	LG PKG    	1536.51	solve slyly a
117520	orange powder ghost gainsboro plum	Manufacturer#5           	Brand#53  	SMALL POLISHED TIN	18	SM BAG    	1537.52	o beans accor
117523	seashell hot moccasin lawn medium	Manufacturer#5           	Brand#54  	LARGE BURNISHED BRASS	21	WRAP PKG  	1540.52	was fi
117524	pink goldenrod grey sienna ghost	Manufacturer#1           	Brand#12  	SMALL POLISHED NICKEL	17	WRAP PACK 	1541.52	al dol
117528	misty bisque salmon frosted dodger	Manufacturer#5           	Brand#54  	STANDARD PLATED NICKEL	49	SM DRUM   	1545.52	ounts use slyly acr
117529	pale misty yellow dim magenta	Manufacturer#1           	Brand#13  	SMALL POLISHED NICKEL	9	WRAP DRUM 	1546.52	 cajole fluffily. furi
117530	olive ghost dark antique papaya	Manufacturer#3           	Brand#32  	PROMO POLISHED COPPER	46	LG BAG    	1547.53	 ideas? car
117534	dark blanched burnished tomato aquamarine	Manufacturer#1           	Brand#14  	MEDIUM PLATED BRASS	20	JUMBO CAN 	1551.53	xes sleep qu
117535	black chiffon honeydew cornflower olive	Manufacturer#4           	Brand#41  	PROMO PLATED NICKEL	47	WRAP CASE 	1552.53	inall
117536	lawn honeydew purple cream smoke	Manufacturer#4           	Brand#42  	PROMO BURNISHED STEEL	50	LG CAN    	1553.53	gular requests
117537	mint misty ivory dodger blue	Manufacturer#2           	Brand#21  	PROMO POLISHED NICKEL	6	LG CASE   	1554.53	uctions might ar
120000	spring black goldenrod slate beige	Manufacturer#5           	Brand#51  	PROMO POLISHED NICKEL	5	JUMBO CASE	1020.00	onic deposits.
120003	metallic rosy gainsboro dark spring	Manufacturer#4           	Brand#45  	PROMO ANODIZED NICKEL	48	LG DRUM   	1023.00	ounts. c
120005	turquoise floral papaya steel blanched	Manufacturer#2           	Brand#25  	SMALL BURNISHED COPPER	35	SM DRUM   	1025.00	s sleep. slyly final d
120006	maroon olive sky honeydew sienna	Manufacturer#3           	Brand#31  	LARGE BRUSHED TIN	45	LG CAN    	1026.00	carefully fin
120007	black antique bisque misty orange	Manufacturer#3           	Brand#34  	PROMO POLISHED TIN	46	WRAP BOX  	1027.00	al instructions. 
120009	deep coral honeydew cornsilk dodger	Manufacturer#1           	Brand#14  	STANDARD BURNISHED TIN	22	WRAP PACK 	1029.00	ests are ca
120010	seashell ghost grey red mint	Manufacturer#1           	Brand#15  	LARGE POLISHED NICKEL	23	JUMBO BAG 	1030.01	ackag
120013	orange frosted azure rosy goldenrod	Manufacturer#4           	Brand#42  	LARGE PLATED NICKEL	16	MED BAG   	1033.01	refully spec
120015	rosy burnished tomato goldenrod gainsboro	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED COPPER	9	WRAP JAR  	1035.01	ly unusua
120016	floral pale purple beige chartreuse	Manufacturer#5           	Brand#55  	LARGE POLISHED STEEL	37	MED BAG   	1036.01	, final accounts. de
120017	moccasin sienna honeydew wheat khaki	Manufacturer#4           	Brand#45  	MEDIUM ANODIZED COPPER	17	SM PKG    	1037.01	ly. furiously furi
120018	tomato spring ghost grey peru	Manufacturer#5           	Brand#54  	PROMO POLISHED BRASS	26	SM CAN    	1038.01	bold ideas! alwa
120024	plum seashell dark linen puff	Manufacturer#4           	Brand#45  	SMALL ANODIZED NICKEL	15	MED PKG   	1044.02	rding to the 
120025	linen slate violet burnished dark	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	40	MED DRUM  	1045.02	arefully regular
120026	yellow peach gainsboro coral frosted	Manufacturer#5           	Brand#51  	ECONOMY BURNISHED TIN	35	MED BOX   	1046.02	lly ironic accoun
120029	rosy plum linen magenta hot	Manufacturer#2           	Brand#24  	SMALL BURNISHED STEEL	22	SM PKG    	1049.02	s. requests use
120030	seashell red linen yellow royal	Manufacturer#5           	Brand#55  	LARGE BURNISHED COPPER	2	SM CAN    	1050.03	es play. quickl
120031	saddle azure steel magenta dark	Manufacturer#2           	Brand#21  	ECONOMY PLATED BRASS	39	SM BAG    	1051.03	efully ironic ideas bo
120034	khaki white green plum red	Manufacturer#3           	Brand#35  	SMALL POLISHED NICKEL	33	JUMBO DRUM	1054.03	among
120035	rosy thistle light cornsilk maroon	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED COPPER	15	LG PACK   	1055.03	uickly. quickly regul
120039	peru metallic almond sienna saddle	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED STEEL	46	JUMBO BOX 	1059.03	its cajole f
120040	peru goldenrod burnished light mint	Manufacturer#2           	Brand#25  	STANDARD BURNISHED STEEL	14	SM JAR    	1060.04	lly spec
120041	violet olive tan burnished seashell	Manufacturer#3           	Brand#34  	MEDIUM PLATED TIN	11	JUMBO BAG 	1061.04	e furiously pendin
120045	bisque ghost spring blanched frosted	Manufacturer#1           	Brand#14  	PROMO BURNISHED NICKEL	48	LG BAG    	1065.04	uests wake slyly. ca
120046	lemon wheat cream yellow peach	Manufacturer#4           	Brand#44  	ECONOMY PLATED NICKEL	49	MED CAN   	1066.04	ording t
120047	cyan almond beige maroon slate	Manufacturer#1           	Brand#14  	PROMO BURNISHED STEEL	43	LG BAG    	1067.04	ress ideas. regul
120048	papaya dark forest chocolate olive	Manufacturer#2           	Brand#21  	STANDARD PLATED BRASS	34	SM BOX    	1068.04	c accounts. fluff
122464	magenta deep light green honeydew	Manufacturer#3           	Brand#35  	STANDARD ANODIZED COPPER	50	WRAP JAR  	1486.46	ly expre
122467	midnight peru magenta dark black	Manufacturer#3           	Brand#35  	STANDARD BRUSHED COPPER	10	LG BAG    	1489.46	y fina
122469	ghost hot bisque burnished almond	Manufacturer#2           	Brand#22  	ECONOMY POLISHED NICKEL	45	SM JAR    	1491.46	gainst the b
122470	deep firebrick indian red light	Manufacturer#1           	Brand#14  	LARGE PLATED BRASS	33	WRAP DRUM 	1492.47	ar, regular packag
122471	aquamarine linen bisque white lemon	Manufacturer#4           	Brand#44  	MEDIUM POLISHED TIN	41	JUMBO JAR 	1493.47	. never final
122473	deep navy cream firebrick dark	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED TIN	42	WRAP BOX  	1495.47	 deposits 
122474	green lavender royal saddle puff	Manufacturer#3           	Brand#35  	MEDIUM BURNISHED NICKEL	10	SM PACK   	1496.47	e express pinto b
122477	chartreuse bisque sandy pale ivory	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED STEEL	2	JUMBO JAR 	1499.47	phins. sly
122479	medium blue dark coral deep	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED COPPER	12	JUMBO BAG 	1501.47	its. express courts 
122480	goldenrod rose sandy cornsilk violet	Manufacturer#5           	Brand#55  	MEDIUM PLATED NICKEL	12	LG PKG    	1502.48	ously. final ac
122481	plum green ghost hot mint	Manufacturer#5           	Brand#53  	PROMO BRUSHED NICKEL	17	JUMBO CAN 	1503.48	fully final foxe
122482	navajo plum dark rose powder	Manufacturer#5           	Brand#51  	ECONOMY PLATED TIN	42	JUMBO JAR 	1504.48	he instructions 
122488	navajo chiffon navy medium drab	Manufacturer#4           	Brand#45  	MEDIUM PLATED STEEL	43	LG BOX    	1510.48	olites. flu
122489	mint slate maroon cyan black	Manufacturer#3           	Brand#32  	MEDIUM ANODIZED STEEL	17	SM BAG    	1511.48	ggle unusual, daring c
122490	firebrick black green orchid cream	Manufacturer#5           	Brand#53  	MEDIUM POLISHED STEEL	32	SM BOX    	1512.49	uests nag furious
122493	papaya dim frosted orchid violet	Manufacturer#4           	Brand#41  	LARGE ANODIZED TIN	17	WRAP BOX  	1515.49	unts nag alongside o
122494	black tan sienna powder drab	Manufacturer#1           	Brand#12  	SMALL PLATED COPPER	20	MED DRUM  	1516.49	ly about the blith
122495	cyan mint light lemon spring	Manufacturer#4           	Brand#42  	ECONOMY BURNISHED STEEL	16	JUMBO BAG 	1517.49	e foxes. slyly even pi
122498	misty moccasin lime seashell purple	Manufacturer#3           	Brand#31  	SMALL POLISHED BRASS	3	LG BOX    	1520.49	posits are
122499	lace khaki powder orchid dodger	Manufacturer#4           	Brand#43  	STANDARD PLATED NICKEL	14	JUMBO PKG 	1521.49	truct
122503	pink lace navy turquoise wheat	Manufacturer#4           	Brand#42  	SMALL PLATED TIN	13	SM JAR    	1525.50	ronic packages 
122504	papaya goldenrod thistle cornsilk burlywood	Manufacturer#5           	Brand#52  	PROMO POLISHED BRASS	49	WRAP CAN  	1526.50	 packages.
122505	cream puff bisque tomato khaki	Manufacturer#3           	Brand#35  	SMALL ANODIZED TIN	13	JUMBO PKG 	1527.50	ly above t
122509	lemon tomato linen drab burnished	Manufacturer#5           	Brand#53  	STANDARD ANODIZED COPPER	8	LG CASE   	1531.50	bold instruct
122510	pale brown blanched chocolate pink	Manufacturer#1           	Brand#15  	STANDARD ANODIZED BRASS	48	WRAP JAR  	1532.51	 fluffily regul
122511	aquamarine tan turquoise beige frosted	Manufacturer#5           	Brand#53  	MEDIUM PLATED NICKEL	19	MED PKG   	1533.51	s play
122512	burnished thistle pale peach blue	Manufacturer#3           	Brand#35  	STANDARD PLATED BRASS	50	WRAP CASE 	1534.51	arefully bra
124976	slate green ghost honeydew moccasin	Manufacturer#1           	Brand#13  	PROMO POLISHED BRASS	34	WRAP CAN  	2000.97	ans haggle sly
124979	maroon pale goldenrod bisque lime	Manufacturer#2           	Brand#25  	LARGE BRUSHED NICKEL	19	SM CASE   	2003.97	nal dep
124981	lemon medium aquamarine drab pale	Manufacturer#3           	Brand#32  	STANDARD PLATED BRASS	16	JUMBO PKG 	2005.98	l pinto beans cajo
124982	mint sienna floral navy metallic	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED STEEL	33	WRAP BAG  	2006.98	nts. furiously unu
124983	linen thistle brown white deep	Manufacturer#2           	Brand#25  	LARGE POLISHED STEEL	32	JUMBO PACK	2007.98	 furiously ironic
124985	papaya misty spring lime plum	Manufacturer#1           	Brand#15  	SMALL BURNISHED BRASS	35	JUMBO BAG 	2009.98	d escapades nag across
124986	plum sandy indian salmon lime	Manufacturer#3           	Brand#32  	PROMO PLATED BRASS	40	SM DRUM   	2010.98	ounts. un
124989	royal tan indian green red	Manufacturer#4           	Brand#41  	STANDARD POLISHED NICKEL	17	SM BOX    	2013.98	 alongside of the 
124991	sienna cornflower lawn frosted lavender	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED TIN	2	JUMBO PACK	2015.99	the theodolites 
124992	wheat spring turquoise maroon sienna	Manufacturer#2           	Brand#25  	SMALL ANODIZED BRASS	47	SM CASE   	2016.99	ly regular accounts 
124993	red green goldenrod thistle peru	Manufacturer#1           	Brand#13  	PROMO BURNISHED BRASS	6	WRAP JAR  	2017.99	ackages print u
124994	navy black sky rose olive	Manufacturer#5           	Brand#55  	PROMO POLISHED COPPER	28	JUMBO BAG 	2018.99	y express
125000	tan lawn rosy saddle chartreuse	Manufacturer#1           	Brand#13  	SMALL BRUSHED COPPER	45	SM BAG    	1025.00	ound the
125001	turquoise puff floral pale ivory	Manufacturer#3           	Brand#34  	STANDARD BRUSHED TIN	49	SM BOX    	1026.00	ests across th
125002	ghost light papaya thistle hot	Manufacturer#5           	Brand#51  	SMALL PLATED COPPER	14	WRAP JAR  	1027.00	c theodoli
125005	sandy medium linen lime peach	Manufacturer#2           	Brand#21  	STANDARD PLATED BRASS	4	JUMBO CAN 	1030.00	he busily unusual
125006	sky dodger linen sandy cornsilk	Manufacturer#3           	Brand#34  	SMALL POLISHED STEEL	16	WRAP BOX  	1031.00	ts. special, even du
125007	steel drab green sandy lawn	Manufacturer#2           	Brand#25  	SMALL PLATED NICKEL	17	SM PKG    	1032.00	s caj
125010	peach linen green lace dim	Manufacturer#5           	Brand#53  	STANDARD POLISHED TIN	17	LG BOX    	1035.01	luffily. special
125011	medium mint red blanched rosy	Manufacturer#2           	Brand#22  	STANDARD POLISHED COPPER	1	JUMBO CASE	1036.01	d plat
125015	misty mint lace dark slate	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED COPPER	5	WRAP CASE 	1040.01	 bold 
125016	rosy blush honeydew lavender dim	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED TIN	3	SM PKG    	1041.01	ay blithely aft
125017	powder deep royal forest beige	Manufacturer#3           	Brand#33  	PROMO BURNISHED STEEL	35	MED PACK  	1042.01	rious deposits use. f
125021	blue burlywood lace ivory lemon	Manufacturer#1           	Brand#15  	STANDARD BURNISHED TIN	21	WRAP CASE 	1046.02	ets. f
125022	misty grey khaki beige white	Manufacturer#4           	Brand#42  	STANDARD BURNISHED NICKEL	44	MED JAR   	1047.02	accounts. pending pa
125023	rose forest gainsboro honeydew blush	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED NICKEL	50	WRAP DRUM 	1048.02	onic foxes 
125024	dodger rose khaki lawn dim	Manufacturer#4           	Brand#41  	SMALL BURNISHED BRASS	16	WRAP PACK 	1049.02	ly final inst
127488	spring deep olive linen snow	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED TIN	38	JUMBO CASE	1515.48	nding accounts!
127491	maroon cornsilk floral ghost goldenrod	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED COPPER	41	MED BAG   	1518.49	accounts. 
127493	magenta cornsilk orchid violet sandy	Manufacturer#2           	Brand#22  	LARGE ANODIZED TIN	23	JUMBO CASE	1520.49	ons thrash 
127494	ghost green blue yellow ivory	Manufacturer#5           	Brand#54  	ECONOMY BURNISHED TIN	44	WRAP CAN  	1521.49	inal instructions a
127495	thistle chiffon turquoise deep yellow	Manufacturer#3           	Brand#31  	PROMO BURNISHED COPPER	27	WRAP CAN  	1522.49	ly. hockey pla
127497	azure salmon brown yellow green	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED COPPER	40	SM CASE   	1524.49	y special pa
127498	goldenrod beige rose cyan maroon	Manufacturer#5           	Brand#52  	ECONOMY POLISHED COPPER	17	WRAP BAG  	1525.49	kly. quickly 
127501	goldenrod olive dodger lemon hot	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED COPPER	44	WRAP CASE 	1528.50	cing packages
127503	navy mint sienna lemon aquamarine	Manufacturer#1           	Brand#14  	LARGE ANODIZED TIN	21	JUMBO BOX 	1530.50	es affix. fina
127504	slate dodger orange dark green	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED STEEL	32	WRAP CASE 	1531.50	 haggle car
127505	dark rose wheat tomato pink	Manufacturer#1           	Brand#15  	STANDARD BRUSHED COPPER	47	WRAP PACK 	1532.50	sly regular wa
127506	papaya drab peach goldenrod aquamarine	Manufacturer#5           	Brand#55  	ECONOMY POLISHED NICKEL	30	SM PKG    	1533.50	ording to the ironi
127512	lemon rosy orange blush antique	Manufacturer#5           	Brand#52  	PROMO BURNISHED NICKEL	36	WRAP BAG  	1539.51	 regular cou
127513	chartreuse orchid papaya thistle tan	Manufacturer#1           	Brand#12  	STANDARD BURNISHED BRASS	33	WRAP BAG  	1540.51	counts. slyly
127514	puff medium sienna plum cream	Manufacturer#2           	Brand#22  	SMALL ANODIZED STEEL	44	JUMBO PKG 	1541.51	ets. 
127517	peach firebrick tomato powder ghost	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED BRASS	39	JUMBO CAN 	1544.51	sual deposits 
127518	mint sky blanched ivory plum	Manufacturer#4           	Brand#41  	SMALL ANODIZED COPPER	23	SM BOX    	1545.51	was after the 
127519	bisque spring smoke dark mint	Manufacturer#5           	Brand#51  	PROMO POLISHED BRASS	11	MED JAR   	1546.51	e carefully ironic 
127522	floral drab chocolate sienna khaki	Manufacturer#5           	Brand#51  	ECONOMY PLATED NICKEL	21	JUMBO PACK	1549.52	ag blithely bold acco
127523	turquoise midnight royal mint dodger	Manufacturer#2           	Brand#23  	LARGE PLATED BRASS	27	MED CAN   	1550.52	p quickly about 
127527	cornflower sky firebrick deep aquamarine	Manufacturer#2           	Brand#21  	MEDIUM PLATED COPPER	47	LG PACK   	1554.52	to bean
127528	sky orange chiffon rose cornflower	Manufacturer#3           	Brand#32  	PROMO ANODIZED STEEL	15	JUMBO DRUM	1555.52	quests. silent d
127529	blue smoke plum purple dark	Manufacturer#4           	Brand#41  	LARGE PLATED NICKEL	20	WRAP DRUM 	1556.52	c instructi
127533	tan blanched blue forest lime	Manufacturer#1           	Brand#14  	MEDIUM BURNISHED COPPER	43	JUMBO CAN 	1560.53	gular 
127534	bisque indian seashell white orchid	Manufacturer#5           	Brand#52  	SMALL BRUSHED COPPER	46	SM BOX    	1561.53	arefull
127535	metallic misty antique slate plum	Manufacturer#4           	Brand#42  	MEDIUM BRUSHED NICKEL	14	JUMBO PKG 	1562.53	uickly. ironic, iro
127536	sky floral midnight red cornsilk	Manufacturer#2           	Brand#24  	SMALL ANODIZED BRASS	6	WRAP DRUM 	1563.53	riously spe
130000	puff deep forest yellow floral	Manufacturer#3           	Brand#34  	PROMO POLISHED COPPER	8	JUMBO CASE	1030.00	ans sleep blith
130003	cornsilk deep midnight goldenrod dim	Manufacturer#3           	Brand#31  	STANDARD BRUSHED COPPER	45	LG PACK   	1033.00	haggle. slyl
130005	magenta chiffon pale lawn snow	Manufacturer#1           	Brand#11  	SMALL BRUSHED TIN	21	LG PACK   	1035.00	ely accoun
130006	magenta misty blanched metallic orchid	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED TIN	38	SM CASE   	1036.00	 furiously bol
130007	salmon smoke lime dodger puff	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	25	SM PKG    	1037.00	ut the blith
130009	lavender purple white navy magenta	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED TIN	40	LG PACK   	1039.00	rts boost furiou
130010	orchid misty blue chiffon purple	Manufacturer#5           	Brand#51  	SMALL POLISHED NICKEL	11	LG BAG    	1040.01	yly among the iro
130013	cyan tomato goldenrod orange black	Manufacturer#4           	Brand#41  	SMALL BURNISHED BRASS	27	SM PACK   	1043.01	ts hang b
130015	magenta steel midnight aquamarine dark	Manufacturer#2           	Brand#21  	SMALL BRUSHED TIN	8	LG BOX    	1045.01	the slyly pending 
130016	pale tan seashell ghost mint	Manufacturer#2           	Brand#24  	SMALL ANODIZED BRASS	15	JUMBO BOX 	1046.01	iously ironic
130017	frosted midnight metallic sky moccasin	Manufacturer#2           	Brand#25  	PROMO POLISHED BRASS	12	MED BOX   	1047.01	en deposits run a
130018	blue chocolate green floral misty	Manufacturer#2           	Brand#22  	MEDIUM PLATED STEEL	15	LG CASE   	1048.01	deposits nag quickly.
130024	salmon cornsilk cornflower honeydew dim	Manufacturer#2           	Brand#23  	STANDARD POLISHED BRASS	34	SM PKG    	1054.02	 bold, busy de
130025	sandy orange beige green violet	Manufacturer#1           	Brand#12  	PROMO POLISHED NICKEL	44	MED CAN   	1055.02	ole never. slyly pe
130026	dodger plum cyan violet sienna	Manufacturer#1           	Brand#11  	STANDARD PLATED STEEL	25	LG PACK   	1056.02	he exp
130029	firebrick saddle green pink hot	Manufacturer#2           	Brand#21  	STANDARD ANODIZED TIN	30	MED JAR   	1059.02	ly accordi
130030	aquamarine spring lawn ghost lavender	Manufacturer#1           	Brand#11  	LARGE POLISHED BRASS	42	LG BOX    	1060.03	g packages
130031	light thistle sienna dim blush	Manufacturer#2           	Brand#24  	MEDIUM BRUSHED STEEL	23	MED CASE  	1061.03	ic, ironic deposit
130034	spring puff coral papaya magenta	Manufacturer#2           	Brand#23  	PROMO BRUSHED TIN	28	LG DRUM   	1064.03	 the deposits
130035	olive gainsboro magenta lemon cornflower	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED TIN	6	WRAP BAG  	1065.03	taphs. requests ab
130039	powder floral violet royal chiffon	Manufacturer#1           	Brand#15  	STANDARD ANODIZED NICKEL	38	SM BAG    	1069.03	r courts are quickl
130040	salmon deep medium sandy gainsboro	Manufacturer#5           	Brand#51  	PROMO BURNISHED BRASS	11	MED CAN   	1070.04	posits-- 
130041	violet blush drab dodger thistle	Manufacturer#2           	Brand#25  	SMALL POLISHED STEEL	21	WRAP PKG  	1071.04	ully 
130045	slate coral midnight cornsilk cornflower	Manufacturer#2           	Brand#24  	MEDIUM POLISHED COPPER	48	JUMBO CAN 	1075.04	ly ironic
130046	green misty violet chocolate blush	Manufacturer#3           	Brand#33  	ECONOMY BURNISHED COPPER	8	WRAP CAN  	1076.04	 close 
130047	pale puff white rose beige	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED NICKEL	12	LG PKG    	1077.04	ular packages nod. qu
130048	sienna magenta lawn burnished burlywood	Manufacturer#1           	Brand#13  	STANDARD ANODIZED BRASS	48	MED DRUM  	1078.04	fily silent requ
132461	almond royal sky burlywood hot	Manufacturer#1           	Brand#11  	LARGE PLATED TIN	14	LG PKG    	1493.46	slyly 
132464	maroon drab bisque goldenrod dark	Manufacturer#4           	Brand#43  	STANDARD ANODIZED NICKEL	7	LG PACK   	1496.46	y. furiousl
132466	gainsboro midnight forest khaki dim	Manufacturer#3           	Brand#35  	MEDIUM BRUSHED COPPER	33	MED CAN   	1498.46	l instructi
132467	pink smoke slate chocolate lime	Manufacturer#4           	Brand#44  	STANDARD PLATED BRASS	32	WRAP CAN  	1499.46	al requests ca
132468	sky pink maroon turquoise wheat	Manufacturer#5           	Brand#53  	PROMO ANODIZED TIN	23	MED DRUM  	1500.46	old packages. flu
132470	cornflower burnished brown plum linen	Manufacturer#1           	Brand#13  	MEDIUM ANODIZED STEEL	1	JUMBO PKG 	1502.47	lyly 
132471	smoke snow goldenrod floral mint	Manufacturer#3           	Brand#33  	MEDIUM PLATED BRASS	40	LG CAN    	1503.47	ickly e
132474	blanched azure cream lime green	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED NICKEL	33	WRAP DRUM 	1506.47	lithely express de
132476	blush khaki navy metallic salmon	Manufacturer#2           	Brand#22  	MEDIUM PLATED TIN	39	JUMBO BOX 	1508.47	ggle sl
132477	ivory tomato seashell plum pink	Manufacturer#2           	Brand#25  	STANDARD ANODIZED BRASS	9	JUMBO DRUM	1509.47	ts boost 
132478	mint rose royal lace black	Manufacturer#4           	Brand#42  	STANDARD BURNISHED STEEL	5	JUMBO PKG 	1510.47	ar package
132479	coral plum firebrick chartreuse ghost	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED COPPER	32	MED PKG   	1511.47	pending d
132485	chocolate seashell deep misty khaki	Manufacturer#3           	Brand#32  	SMALL BURNISHED BRASS	10	SM BAG    	1517.48	fluffi
132486	yellow steel saddle black antique	Manufacturer#5           	Brand#51  	LARGE BRUSHED STEEL	5	MED BAG   	1518.48	 ironic dep
132487	mint hot spring lace sienna	Manufacturer#5           	Brand#53  	SMALL BRUSHED NICKEL	50	LG DRUM   	1519.48	ealthy, silent pac
132490	snow orchid puff dodger deep	Manufacturer#1           	Brand#15  	STANDARD ANODIZED NICKEL	22	WRAP DRUM 	1522.49	le packages.
132491	ivory slate magenta tomato burnished	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED TIN	29	WRAP CASE 	1523.49	accounts.
132492	cornsilk burlywood chiffon lawn smoke	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED BRASS	20	LG PACK   	1524.49	counts al
132495	firebrick peach honeydew navajo medium	Manufacturer#5           	Brand#51  	SMALL BURNISHED NICKEL	17	WRAP CAN  	1527.49	ts around the furious
132496	dim sandy papaya medium lime	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED STEEL	13	LG CAN    	1528.49	 slyly above the
132500	peru rosy royal forest blush	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED COPPER	26	LG PKG    	1532.50	 the 
132501	hot pale grey ghost goldenrod	Manufacturer#3           	Brand#34  	STANDARD PLATED NICKEL	16	JUMBO BOX 	1533.50	thely even instru
132502	goldenrod burlywood orchid bisque blue	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED TIN	7	JUMBO DRUM	1534.50	inst the 
132506	peru steel brown black antique	Manufacturer#1           	Brand#14  	LARGE BURNISHED STEEL	28	LG BAG    	1538.50	le careful
132507	burnished light chartreuse aquamarine red	Manufacturer#2           	Brand#24  	MEDIUM ANODIZED COPPER	11	WRAP CASE 	1539.50	 from 
132508	navy orchid cream almond misty	Manufacturer#1           	Brand#13  	PROMO POLISHED COPPER	13	SM BAG    	1540.50	 packages? regular, 
132509	beige chocolate metallic rose mint	Manufacturer#4           	Brand#45  	PROMO BURNISHED TIN	16	SM BOX    	1541.50	ic ideas are
134974	orange green dim spring lime	Manufacturer#5           	Brand#51  	SMALL PLATED STEEL	10	LG PKG    	2008.97	press r
134977	sandy orchid black dim hot	Manufacturer#3           	Brand#33  	LARGE POLISHED BRASS	7	SM BOX    	2011.97	deposits.
134979	drab cornsilk goldenrod gainsboro sandy	Manufacturer#1           	Brand#15  	ECONOMY POLISHED COPPER	21	JUMBO CAN 	2013.97	ular,
134980	sienna navajo azure coral pink	Manufacturer#1           	Brand#13  	LARGE BRUSHED NICKEL	17	MED BOX   	2014.98	ackages. 
134981	navy almond grey snow dark	Manufacturer#5           	Brand#53  	SMALL BURNISHED BRASS	28	MED BAG   	2015.98	ounts: packages wak
134983	lavender azure chocolate medium goldenrod	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED BRASS	41	SM CASE   	2017.98	c deposits? fox
134984	drab salmon lime sky plum	Manufacturer#5           	Brand#52  	PROMO BURNISHED COPPER	32	MED BOX   	2018.98	sts. fluf
134987	bisque burnished blue honeydew peach	Manufacturer#3           	Brand#32  	ECONOMY BRUSHED BRASS	32	WRAP JAR  	2021.98	hely even sauternes d
134989	green beige thistle seashell dodger	Manufacturer#2           	Brand#24  	LARGE PLATED COPPER	42	WRAP CASE 	2023.98	pending foxes c
134990	lace sienna misty metallic indian	Manufacturer#4           	Brand#41  	MEDIUM ANODIZED STEEL	38	JUMBO PKG 	2024.99	ess de
134991	cyan drab honeydew turquoise mint	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED COPPER	30	MED CAN   	2025.99	slyly according t
134992	chiffon lemon purple beige sandy	Manufacturer#4           	Brand#41  	SMALL BRUSHED COPPER	1	JUMBO BOX 	2026.99	usly bold ideas. req
134998	white chartreuse sky seashell puff	Manufacturer#4           	Brand#41  	LARGE POLISHED NICKEL	23	LG CASE   	2032.99	. final deposits sle
134999	linen cornsilk ivory turquoise sandy	Manufacturer#1           	Brand#13  	LARGE BRUSHED NICKEL	10	SM PACK   	2033.99	phs use fu
135000	misty cornsilk burnished chocolate sandy	Manufacturer#5           	Brand#52  	MEDIUM BRUSHED BRASS	3	SM PKG    	1035.00	telets. regular pac
135003	rose royal red aquamarine floral	Manufacturer#3           	Brand#31  	ECONOMY PLATED STEEL	48	JUMBO JAR 	1038.00	leep sly
135004	plum salmon blue orange ivory	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	2	WRAP PACK 	1039.00	ttainme
135005	lace khaki forest navy red	Manufacturer#1           	Brand#15  	ECONOMY ANODIZED STEEL	18	LG BAG    	1040.00	e quick
135008	pale sandy tan beige slate	Manufacturer#5           	Brand#52  	ECONOMY PLATED TIN	22	LG PKG    	1043.00	. account
135009	peru chocolate mint metallic dark	Manufacturer#5           	Brand#52  	PROMO BRUSHED BRASS	2	JUMBO PKG 	1044.00	ily express acc
135013	linen cornsilk metallic lemon peach	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED NICKEL	36	MED CASE  	1048.01	ic deposits are furiou
135014	chartreuse yellow almond khaki cyan	Manufacturer#1           	Brand#14  	ECONOMY POLISHED BRASS	39	LG JAR    	1049.01	excuses. e
135015	chiffon blanched blush wheat rose	Manufacturer#4           	Brand#41  	STANDARD PLATED NICKEL	32	MED PACK  	1050.01	ross the f
135019	saddle rosy wheat green blanched	Manufacturer#1           	Brand#12  	MEDIUM ANODIZED TIN	45	JUMBO DRUM	1054.01	its are furiously arou
135020	dark goldenrod plum cornsilk sky	Manufacturer#1           	Brand#13  	STANDARD BRUSHED NICKEL	3	WRAP DRUM 	1055.02	ously above t
135021	aquamarine purple blue grey indian	Manufacturer#4           	Brand#43  	PROMO PLATED BRASS	27	LG BOX    	1056.02	ole. fluffily express
135022	magenta lime mint peach violet	Manufacturer#2           	Brand#21  	ECONOMY ANODIZED BRASS	2	SM BAG    	1057.02	le alongside of
137487	snow papaya green beige chartreuse	Manufacturer#2           	Brand#25  	LARGE ANODIZED COPPER	50	LG BAG    	1524.48	 to the
137490	bisque rosy moccasin beige seashell	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED STEEL	35	LG PKG    	1527.49	le furiously. en
137492	forest saddle coral khaki dark	Manufacturer#2           	Brand#25  	STANDARD ANODIZED STEEL	26	SM JAR    	1529.49	posits. b
137493	frosted dodger salmon honeydew beige	Manufacturer#2           	Brand#23  	STANDARD PLATED BRASS	45	LG PACK   	1530.49	 accounts cajol
137494	medium brown puff indian deep	Manufacturer#3           	Brand#32  	ECONOMY PLATED TIN	45	WRAP DRUM 	1531.49	p; regular accoun
137496	deep burlywood seashell blue chocolate	Manufacturer#3           	Brand#35  	LARGE PLATED NICKEL	40	LG PKG    	1533.49	 depths haggle fluffil
137497	seashell cream chartreuse turquoise almond	Manufacturer#3           	Brand#35  	SMALL BRUSHED TIN	44	LG BAG    	1534.49	e bold, ironic excus
137500	misty maroon black moccasin blush	Manufacturer#2           	Brand#25  	PROMO POLISHED TIN	40	SM JAR    	1537.50	ys regul
137502	forest bisque brown drab black	Manufacturer#3           	Brand#32  	LARGE BRUSHED NICKEL	27	WRAP CASE 	1539.50	sts kindle fur
137503	wheat medium magenta chiffon blush	Manufacturer#4           	Brand#44  	STANDARD ANODIZED STEEL	4	MED JAR   	1540.50	ly regular accounts
137504	firebrick grey magenta white almond	Manufacturer#2           	Brand#23  	MEDIUM PLATED TIN	36	LG PACK   	1541.50	 alwa
137505	gainsboro blue dodger cornflower dark	Manufacturer#1           	Brand#15  	STANDARD ANODIZED STEEL	22	LG JAR    	1542.50	. blithe
137511	dodger navy yellow beige blush	Manufacturer#4           	Brand#44  	ECONOMY BRUSHED TIN	41	LG CAN    	1548.51	even instru
137512	rosy puff sienna purple brown	Manufacturer#4           	Brand#42  	STANDARD BRUSHED TIN	18	LG JAR    	1549.51	ts accor
137513	spring beige blanched antique orchid	Manufacturer#5           	Brand#55  	SMALL POLISHED STEEL	11	LG CAN    	1550.51	inal p
137516	sky ghost thistle smoke chiffon	Manufacturer#5           	Brand#53  	ECONOMY PLATED COPPER	2	SM PKG    	1553.51	iously pending pl
137517	medium honeydew floral linen khaki	Manufacturer#3           	Brand#32  	SMALL BURNISHED TIN	10	JUMBO PACK	1554.51	ash furiously regular
137518	royal ivory thistle coral white	Manufacturer#4           	Brand#41  	PROMO ANODIZED STEEL	41	SM BAG    	1555.51	ctions. regular acc
137521	purple yellow moccasin maroon violet	Manufacturer#2           	Brand#22  	SMALL BURNISHED NICKEL	29	WRAP PKG  	1558.52	blithely e
137522	cream sandy black cornflower brown	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED COPPER	7	MED PKG   	1559.52	lly across the unusua
137526	black ivory thistle burnished chocolate	Manufacturer#1           	Brand#14  	ECONOMY BURNISHED STEEL	8	LG CASE   	1563.52	ly special packages
137527	grey drab steel frosted misty	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED BRASS	47	LG CASE   	1564.52	boost along
137528	ghost lawn burlywood lavender orchid	Manufacturer#3           	Brand#35  	STANDARD ANODIZED TIN	7	LG JAR    	1565.52	 alongsid
137532	chartreuse blue navajo lawn brown	Manufacturer#4           	Brand#43  	MEDIUM PLATED COPPER	4	SM DRUM   	1569.53	regular dugouts 
137533	peach smoke linen magenta red	Manufacturer#5           	Brand#53  	PROMO ANODIZED STEEL	37	MED JAR   	1570.53	about
137534	slate lawn olive salmon azure	Manufacturer#5           	Brand#51  	LARGE BRUSHED NICKEL	31	JUMBO BAG 	1571.53	l theodolites aff
137535	smoke peru lace powder light	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED COPPER	42	SM JAR    	1572.53	sts. furiously re
140000	salmon frosted beige midnight blush	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED NICKEL	8	LG CASE   	1040.00	es wake abov
140003	peach orange magenta cornsilk lavender	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED TIN	38	SM PACK   	1043.00	eep carefull
140005	plum lavender dodger papaya khaki	Manufacturer#4           	Brand#41  	PROMO BURNISHED BRASS	29	WRAP BAG  	1045.00	le regular, regul
140006	sky lime coral plum dodger	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED BRASS	25	SM PACK   	1046.00	grate furio
140007	mint cornflower sandy blanched orchid	Manufacturer#3           	Brand#33  	PROMO BURNISHED COPPER	21	JUMBO JAR 	1047.00	ular instruction
140009	blue misty bisque puff slate	Manufacturer#5           	Brand#52  	PROMO BURNISHED BRASS	37	WRAP PKG  	1049.00	jole iro
140010	pink chartreuse cornflower cyan tomato	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED BRASS	10	WRAP JAR  	1050.01	es sleep b
140013	drab cornflower brown cornsilk dark	Manufacturer#3           	Brand#33  	ECONOMY PLATED COPPER	48	WRAP JAR  	1053.01	regular, pen
140015	steel tomato red navy gainsboro	Manufacturer#1           	Brand#11  	ECONOMY POLISHED NICKEL	40	WRAP BOX  	1055.01	-- ca
140016	medium coral smoke ivory cornflower	Manufacturer#2           	Brand#21  	STANDARD BRUSHED TIN	19	SM BAG    	1056.01	ithely even asymptote
140017	sandy midnight violet honeydew goldenrod	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED COPPER	19	MED BOX   	1057.01	y special reques
140018	floral navajo aquamarine seashell pale	Manufacturer#2           	Brand#24  	ECONOMY ANODIZED TIN	41	JUMBO CASE	1058.01	nts after
140024	coral beige slate orange spring	Manufacturer#1           	Brand#15  	MEDIUM BURNISHED BRASS	34	LG CASE   	1064.02	ts. pending acco
140025	smoke linen peach tan metallic	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED STEEL	8	SM PKG    	1065.02	side of the care
140026	orchid azure indian turquoise snow	Manufacturer#1           	Brand#11  	SMALL BURNISHED BRASS	19	SM PACK   	1066.02	 slyly above
140029	moccasin coral salmon cream maroon	Manufacturer#5           	Brand#51  	ECONOMY BRUSHED TIN	20	LG BAG    	1069.02	 foxes haggl
140030	orchid blanched snow white floral	Manufacturer#4           	Brand#44  	ECONOMY POLISHED STEEL	26	MED BAG   	1070.03	instructi
140031	frosted saddle lawn bisque sienna	Manufacturer#2           	Brand#23  	PROMO PLATED NICKEL	10	SM DRUM   	1071.03	icing 
140034	navajo bisque sandy lemon turquoise	Manufacturer#2           	Brand#22  	MEDIUM BRUSHED TIN	22	MED CASE  	1074.03	count
140035	slate cornflower snow sandy beige	Manufacturer#3           	Brand#34  	MEDIUM POLISHED TIN	4	WRAP JAR  	1075.03	s. flu
140039	navajo almond pink puff ivory	Manufacturer#5           	Brand#52  	SMALL PLATED BRASS	50	WRAP CASE 	1079.03	odolites haggle qui
140040	lace blush red seashell azure	Manufacturer#1           	Brand#11  	LARGE PLATED COPPER	12	LG PACK   	1080.04	ckages. ironic depend
140041	cornsilk sandy lace sienna rose	Manufacturer#1           	Brand#14  	PROMO POLISHED BRASS	17	WRAP CAN  	1081.04	slyly carefu
140045	chocolate green midnight rosy cream	Manufacturer#3           	Brand#31  	MEDIUM ANODIZED NICKEL	34	WRAP BAG  	1085.04	uffily.
140046	salmon grey lavender tan blush	Manufacturer#4           	Brand#41  	MEDIUM PLATED BRASS	20	MED PACK  	1086.04	solve. blith
140047	ghost peru azure lavender chiffon	Manufacturer#1           	Brand#15  	PROMO BURNISHED COPPER	8	LG PACK   	1087.04	refully unusual fo
140048	slate smoke tan salmon yellow	Manufacturer#2           	Brand#22  	ECONOMY PLATED COPPER	12	JUMBO BAG 	1088.04	 packages 
142458	almond rosy lime black plum	Manufacturer#2           	Brand#24  	MEDIUM BURNISHED NICKEL	37	JUMBO JAR 	1500.45	 requests are sly
142461	maroon brown powder puff peach	Manufacturer#3           	Brand#33  	STANDARD BRUSHED BRASS	27	MED JAR   	1503.46	. carefully unus
142463	navajo deep almond seashell midnight	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED COPPER	32	JUMBO DRUM	1505.46	ar accounts.
142464	tomato chiffon khaki steel grey	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	22	MED CAN   	1506.46	tain packages. fluffi
142465	white powder ivory cornflower orange	Manufacturer#1           	Brand#14  	SMALL PLATED STEEL	47	SM JAR    	1507.46	l, re
142467	burlywood plum blue peru forest	Manufacturer#2           	Brand#21  	PROMO POLISHED STEEL	37	JUMBO JAR 	1509.46	ending de
142468	lawn seashell steel almond navajo	Manufacturer#3           	Brand#33  	PROMO PLATED NICKEL	18	SM PKG    	1510.46	accounts cajole s
142471	drab purple cyan wheat ivory	Manufacturer#4           	Brand#44  	LARGE PLATED STEEL	40	JUMBO JAR 	1513.47	 furiously final p
142473	grey pale khaki olive firebrick	Manufacturer#3           	Brand#35  	PROMO POLISHED NICKEL	15	MED CASE  	1515.47	aggle furiously around
142474	cyan olive slate dodger orchid	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED NICKEL	34	LG PACK   	1516.47	always regul
142475	orchid smoke tan lace khaki	Manufacturer#2           	Brand#23  	SMALL POLISHED NICKEL	3	LG BOX    	1517.47	the slyly pending
142476	spring blanched royal honeydew snow	Manufacturer#4           	Brand#42  	ECONOMY ANODIZED STEEL	46	WRAP CASE 	1518.47	gular dependencies 
142482	gainsboro medium coral misty navajo	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	20	WRAP PACK 	1524.48	fully besides the pe
142483	thistle blush cream smoke cornflower	Manufacturer#4           	Brand#43  	SMALL BRUSHED BRASS	36	JUMBO DRUM	1525.48	 carefully final theo
142484	midnight orchid olive indian moccasin	Manufacturer#3           	Brand#34  	ECONOMY BURNISHED TIN	27	MED BOX   	1526.48	osits
142487	rosy medium drab green sky	Manufacturer#1           	Brand#12  	SMALL POLISHED NICKEL	32	WRAP CASE 	1529.48	o beans detec
142488	lace lavender floral blush cornsilk	Manufacturer#3           	Brand#32  	PROMO POLISHED STEEL	21	LG PACK   	1530.48	he fluffily regul
142489	dark magenta almond wheat honeydew	Manufacturer#4           	Brand#44  	STANDARD PLATED STEEL	26	JUMBO CAN 	1531.48	ainst the sometimes sp
142492	medium hot magenta salmon red	Manufacturer#3           	Brand#31  	SMALL POLISHED COPPER	44	WRAP BOX  	1534.49	ickly unusual
142493	midnight metallic red plum cornsilk	Manufacturer#4           	Brand#44  	PROMO PLATED COPPER	39	JUMBO PACK	1535.49	 across the blithe
142497	medium yellow antique tan lawn	Manufacturer#5           	Brand#51  	PROMO BURNISHED TIN	35	LG JAR    	1539.49	equests affix 
142498	orange coral blanched tan tomato	Manufacturer#4           	Brand#41  	MEDIUM PLATED NICKEL	47	LG PACK   	1540.49	ily. bold deposits acr
142499	red drab khaki cyan gainsboro	Manufacturer#2           	Brand#23  	SMALL PLATED STEEL	50	WRAP DRUM 	1541.49	 above
142503	dark tan purple snow coral	Manufacturer#2           	Brand#22  	PROMO PLATED TIN	7	SM PKG    	1545.50	bout the ev
142504	orchid medium pale dodger puff	Manufacturer#2           	Brand#24  	SMALL ANODIZED TIN	5	SM CAN    	1546.50	 final th
142505	tomato papaya lace burnished lemon	Manufacturer#1           	Brand#13  	LARGE BRUSHED COPPER	23	WRAP CASE 	1547.50	s cajole slyly quickly
142506	tomato rosy lemon antique indian	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED STEEL	23	LG BOX    	1548.50	ructions; blithely i
144972	cornsilk cornflower misty aquamarine thistle	Manufacturer#2           	Brand#21  	SMALL ANODIZED TIN	35	SM BOX    	2016.97	deas. acc
144975	honeydew misty red deep magenta	Manufacturer#5           	Brand#52  	STANDARD PLATED STEEL	12	WRAP JAR  	2019.97	nto bean
144977	lemon ivory moccasin saddle magenta	Manufacturer#1           	Brand#15  	STANDARD ANODIZED TIN	15	SM CASE   	2021.97	carefully fi
144978	firebrick chiffon ivory sandy spring	Manufacturer#5           	Brand#52  	LARGE BRUSHED COPPER	21	SM JAR    	2022.97	unts. pending 
144979	almond mint powder honeydew orange	Manufacturer#1           	Brand#13  	LARGE POLISHED TIN	42	MED PKG   	2023.97	iously. fluffily pendi
144981	tomato sky lawn aquamarine wheat	Manufacturer#1           	Brand#14  	SMALL ANODIZED COPPER	14	MED PKG   	2025.98	nal instructio
144982	burnished hot lace olive navy	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED BRASS	34	LG JAR    	2026.98	slyly de
144985	olive burlywood orchid cyan plum	Manufacturer#2           	Brand#25  	PROMO POLISHED BRASS	32	SM PKG    	2029.98	its. 
144987	chocolate turquoise chiffon midnight sienna	Manufacturer#5           	Brand#51  	LARGE ANODIZED COPPER	8	LG CAN    	2031.98	tes. asymptote
144988	goldenrod blanched seashell navy lace	Manufacturer#1           	Brand#12  	PROMO ANODIZED NICKEL	50	MED PACK  	2032.98	quickly fluffi
144989	light dim blanched sky gainsboro	Manufacturer#5           	Brand#53  	LARGE PLATED BRASS	49	SM PKG    	2033.98	g to t
144990	snow beige seashell papaya goldenrod	Manufacturer#2           	Brand#25  	PROMO POLISHED NICKEL	41	SM PACK   	2034.99	dolite
144996	medium orchid magenta lawn mint	Manufacturer#5           	Brand#53  	PROMO BRUSHED STEEL	5	SM PKG    	2040.99	blithe
144997	dark plum magenta papaya olive	Manufacturer#2           	Brand#23  	STANDARD BRUSHED BRASS	18	JUMBO PACK	2041.99	 the blithe
144998	burlywood black tan peru midnight	Manufacturer#1           	Brand#12  	STANDARD POLISHED NICKEL	30	LG PKG    	2042.99	 above t
145001	brown bisque olive azure dim	Manufacturer#2           	Brand#25  	PROMO BRUSHED BRASS	28	JUMBO BOX 	1046.00	blithely
145002	ivory hot brown black blanched	Manufacturer#4           	Brand#42  	PROMO PLATED TIN	25	MED JAR   	1047.00	quests. even deposits 
145003	puff maroon misty white navy	Manufacturer#2           	Brand#22  	MEDIUM PLATED COPPER	8	LG DRUM   	1048.00	 wake blithely beneat
145006	coral thistle tomato blush royal	Manufacturer#1           	Brand#15  	SMALL POLISHED TIN	20	MED JAR   	1051.00	final fox
145007	frosted chiffon pink yellow navy	Manufacturer#1           	Brand#13  	MEDIUM BURNISHED BRASS	44	LG CAN    	1052.00	dly unusu
145011	steel aquamarine light navajo grey	Manufacturer#4           	Brand#43  	SMALL ANODIZED STEEL	1	LG PACK   	1056.01	furiously
145012	indian lime steel thistle orange	Manufacturer#4           	Brand#43  	PROMO PLATED STEEL	35	MED CASE  	1057.01	. deposits hag
145013	wheat lime yellow coral white	Manufacturer#1           	Brand#11  	PROMO BRUSHED TIN	43	JUMBO BOX 	1058.01	s use abou
145017	cream sandy magenta gainsboro forest	Manufacturer#2           	Brand#23  	PROMO PLATED NICKEL	26	LG CAN    	1062.01	final theodol
145018	mint steel orchid sandy lavender	Manufacturer#5           	Brand#52  	ECONOMY BURNISHED STEEL	36	JUMBO CAN 	1063.01	furiously regular
145019	smoke puff burnished firebrick linen	Manufacturer#1           	Brand#12  	ECONOMY BURNISHED BRASS	43	JUMBO CAN 	1064.01	eans us
145020	saddle blush plum coral wheat	Manufacturer#4           	Brand#42  	ECONOMY POLISHED BRASS	48	JUMBO PACK	1065.02	ffily across the expre
147486	grey cyan plum lime thistle	Manufacturer#4           	Brand#42  	PROMO BURNISHED TIN	9	LG PKG    	1533.48	ans boost bl
147489	peru chiffon violet rose sky	Manufacturer#4           	Brand#43  	LARGE BURNISHED BRASS	19	LG PKG    	1536.48	the b
147491	chartreuse steel lime deep honeydew	Manufacturer#2           	Brand#22  	LARGE BRUSHED BRASS	5	MED BAG   	1538.49	oost alongside of
147492	dark pink olive cyan rose	Manufacturer#4           	Brand#44  	LARGE POLISHED BRASS	16	MED JAR   	1539.49	. carefully p
147493	maroon papaya brown chiffon lemon	Manufacturer#1           	Brand#15  	STANDARD POLISHED COPPER	13	LG CASE   	1540.49	se. c
147495	sandy midnight coral lemon chocolate	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED NICKEL	23	JUMBO BOX 	1542.49	fully special
147496	deep salmon cream royal ghost	Manufacturer#2           	Brand#25  	SMALL BRUSHED TIN	17	LG DRUM   	1543.49	 about the 
147499	saddle firebrick ivory papaya mint	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED STEEL	45	SM PACK   	1546.49	 furiously idle courts
147501	tan steel midnight lime orchid	Manufacturer#3           	Brand#33  	PROMO BRUSHED STEEL	44	LG PACK   	1548.50	ix bravely reg
147502	floral almond olive blush magenta	Manufacturer#5           	Brand#54  	ECONOMY ANODIZED NICKEL	10	JUMBO PKG 	1549.50	y care
147503	wheat almond burlywood turquoise steel	Manufacturer#3           	Brand#34  	SMALL ANODIZED NICKEL	24	JUMBO CASE	1550.50	t the 
147504	rose sandy red metallic peru	Manufacturer#1           	Brand#13  	MEDIUM PLATED COPPER	40	WRAP PACK 	1551.50	 the unusual, 
147510	chiffon salmon navy light coral	Manufacturer#3           	Brand#34  	PROMO ANODIZED NICKEL	39	SM CASE   	1557.51	ts boost blithel
147511	peach dodger burlywood dark cornsilk	Manufacturer#3           	Brand#33  	STANDARD ANODIZED STEEL	12	LG DRUM   	1558.51	nstructions. speci
147512	wheat cream firebrick almond yellow	Manufacturer#4           	Brand#41  	MEDIUM POLISHED BRASS	16	WRAP DRUM 	1559.51	fily cl
147515	bisque smoke blush royal chocolate	Manufacturer#2           	Brand#24  	PROMO BRUSHED TIN	39	MED BAG   	1562.51	nal p
147516	tan lemon thistle moccasin smoke	Manufacturer#5           	Brand#51  	MEDIUM PLATED BRASS	49	MED CAN   	1563.51	atelets detect 
147517	puff steel forest brown antique	Manufacturer#1           	Brand#12  	LARGE POLISHED COPPER	16	MED PACK  	1564.51	 packages caj
147520	tomato navy royal purple khaki	Manufacturer#3           	Brand#31  	SMALL BURNISHED BRASS	3	JUMBO CASE	1567.52	ly. theodolite
147521	papaya lemon spring misty black	Manufacturer#3           	Brand#31  	MEDIUM PLATED COPPER	30	WRAP BOX  	1568.52	uests above the fl
147525	lawn coral olive red khaki	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	32	MED BAG   	1572.52	ic packages hagg
147526	antique steel magenta beige black	Manufacturer#3           	Brand#34  	PROMO POLISHED COPPER	12	JUMBO CAN 	1573.52	al ideas. plat
147527	light tomato navy deep khaki	Manufacturer#2           	Brand#21  	MEDIUM BURNISHED COPPER	3	LG JAR    	1574.52	ly unu
147531	tomato dim bisque metallic cornsilk	Manufacturer#3           	Brand#34  	ECONOMY PLATED COPPER	1	SM CAN    	1578.53	ind dogged
147532	papaya sienna peru plum bisque	Manufacturer#2           	Brand#25  	STANDARD POLISHED COPPER	38	MED PKG   	1579.53	lyly special accoun
147533	azure papaya violet brown sandy	Manufacturer#5           	Brand#54  	ECONOMY PLATED NICKEL	34	LG JAR    	1580.53	riously final r
147534	steel papaya seashell moccasin pale	Manufacturer#3           	Brand#34  	PROMO PLATED COPPER	20	JUMBO PKG 	1581.53	xes. pinto bea
150000	lemon dim violet royal dark	Manufacturer#1           	Brand#12  	PROMO PLATED NICKEL	48	LG CAN    	1050.00	ts cajole carefully s
150003	navajo peru light forest steel	Manufacturer#2           	Brand#22  	PROMO BURNISHED COPPER	33	WRAP BAG  	1053.00	ses. blithely silent
150005	firebrick drab puff grey lawn	Manufacturer#5           	Brand#51  	LARGE BURNISHED COPPER	50	JUMBO BOX 	1055.00	nts haggle 
150006	peru cyan medium thistle blanched	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED COPPER	16	WRAP JAR  	1056.00	impress blithely a
150007	antique seashell medium magenta yellow	Manufacturer#4           	Brand#43  	SMALL POLISHED COPPER	12	SM BOX    	1057.00	counts. regular deposi
150009	turquoise chocolate maroon linen cornflower	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED NICKEL	44	LG JAR    	1059.00	t the final, 
150010	light orchid hot honeydew pale	Manufacturer#1           	Brand#12  	LARGE PLATED COPPER	26	LG PACK   	1060.01	s. ironic, unusual pl
150013	firebrick pink maroon plum chiffon	Manufacturer#4           	Brand#44  	PROMO ANODIZED BRASS	11	WRAP CASE 	1063.01	ual ideas. packag
150015	dim ivory powder sandy dodger	Manufacturer#1           	Brand#13  	MEDIUM BURNISHED COPPER	35	LG PACK   	1065.01	 theodolites-- 
150016	moccasin black snow goldenrod chiffon	Manufacturer#4           	Brand#43  	MEDIUM POLISHED NICKEL	48	LG CASE   	1066.01	ins cajol
150017	chiffon drab white smoke burlywood	Manufacturer#4           	Brand#44  	STANDARD BURNISHED COPPER	27	JUMBO CASE	1067.01	ending 
150018	mint antique blue sky goldenrod	Manufacturer#4           	Brand#45  	SMALL ANODIZED NICKEL	5	LG DRUM   	1068.01	 boost
150024	plum wheat pale blanched deep	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED BRASS	25	JUMBO PKG 	1074.02	. even deposits a
150025	hot coral ivory lime thistle	Manufacturer#2           	Brand#21  	LARGE PLATED TIN	23	LG CAN    	1075.02	le furiousl
150026	rosy thistle dodger purple coral	Manufacturer#2           	Brand#24  	STANDARD BRUSHED BRASS	31	JUMBO PACK	1076.02	press e
150029	seashell slate lime honeydew sienna	Manufacturer#4           	Brand#44  	PROMO PLATED TIN	29	SM PKG    	1079.02	nic accoun
150030	peru cream papaya cornsilk green	Manufacturer#1           	Brand#11  	SMALL PLATED COPPER	43	LG PKG    	1080.03	elets. 
150031	violet coral mint dodger frosted	Manufacturer#2           	Brand#24  	PROMO BRUSHED BRASS	42	SM CASE   	1081.03	 regula
150034	powder peach moccasin aquamarine tomato	Manufacturer#2           	Brand#25  	LARGE BRUSHED COPPER	4	WRAP CAN  	1084.03	lithely express accoun
150035	beige orchid chiffon green spring	Manufacturer#3           	Brand#33  	LARGE BRUSHED NICKEL	21	LG PACK   	1085.03	 ironic 
150039	beige peach medium frosted aquamarine	Manufacturer#4           	Brand#41  	PROMO PLATED TIN	20	SM CASE   	1089.03	lently ironic theodol
150040	yellow medium forest red cornflower	Manufacturer#5           	Brand#52  	STANDARD ANODIZED TIN	22	JUMBO DRUM	1090.04	 packages h
150041	almond maroon frosted burlywood royal	Manufacturer#3           	Brand#33  	STANDARD PLATED BRASS	35	MED CAN   	1091.04	e packages. quick
150045	honeydew blue salmon pale olive	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED BRASS	23	JUMBO CAN 	1095.04	usly among the spec
150046	forest ghost lavender deep salmon	Manufacturer#3           	Brand#33  	SMALL BRUSHED COPPER	24	JUMBO JAR 	1096.04	counts across th
150047	forest chiffon dim drab brown	Manufacturer#3           	Brand#33  	SMALL BURNISHED TIN	3	LG JAR    	1097.04	es are unusual
150048	dodger rose frosted misty turquoise	Manufacturer#5           	Brand#54  	ECONOMY POLISHED NICKEL	45	MED CASE  	1098.04	fully even accounts r
152455	powder peach frosted purple midnight	Manufacturer#4           	Brand#41  	PROMO PLATED BRASS	39	LG PACK   	1507.45	s boost furiousl
152458	peru dodger sienna lime peach	Manufacturer#1           	Brand#13  	MEDIUM BRUSHED TIN	10	WRAP PKG  	1510.45	yly iron
152460	linen moccasin lime peach blue	Manufacturer#2           	Brand#22  	PROMO PLATED BRASS	29	LG DRUM   	1512.46	efully 
152461	red hot cornsilk orange floral	Manufacturer#3           	Brand#34  	LARGE BRUSHED NICKEL	38	JUMBO PACK	1513.46	 foxes hinder qui
152462	drab wheat linen metallic peru	Manufacturer#4           	Brand#41  	MEDIUM BRUSHED TIN	28	SM JAR    	1514.46	integrate qui
152464	salmon burlywood spring coral sienna	Manufacturer#2           	Brand#25  	STANDARD BRUSHED TIN	13	WRAP PKG  	1516.46	instructio
152465	cornflower wheat burlywood chartreuse medium	Manufacturer#4           	Brand#45  	MEDIUM BURNISHED COPPER	32	SM JAR    	1517.46	as. carefully regular
152468	bisque honeydew goldenrod spring cornsilk	Manufacturer#5           	Brand#52  	PROMO PLATED STEEL	40	MED BOX   	1520.46	y fluffily ir
152470	midnight sandy lavender dim salmon	Manufacturer#1           	Brand#15  	MEDIUM ANODIZED COPPER	47	SM CAN    	1522.47	ts. slyly ironic acc
152471	azure medium sienna forest misty	Manufacturer#4           	Brand#44  	LARGE ANODIZED STEEL	7	WRAP DRUM 	1523.47	 regular 
152472	rosy turquoise khaki almond chiffon	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED NICKEL	18	WRAP CASE 	1524.47	st. furiousl
152473	smoke deep floral blanched rose	Manufacturer#1           	Brand#11  	LARGE POLISHED STEEL	39	MED PKG   	1525.47	mptotes ar
152479	orange moccasin smoke seashell rose	Manufacturer#3           	Brand#32  	STANDARD BURNISHED STEEL	14	JUMBO DRUM	1531.47	lar platelets. reg
152480	gainsboro orchid peach lavender plum	Manufacturer#5           	Brand#52  	STANDARD PLATED COPPER	27	WRAP JAR  	1532.48	regular
152481	floral light maroon lace mint	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED COPPER	28	WRAP BOX  	1533.48	its al
152484	lemon salmon honeydew rose black	Manufacturer#1           	Brand#13  	LARGE BRUSHED STEEL	49	SM JAR    	1536.48	s. fluffily unu
152485	green cornflower drab chartreuse light	Manufacturer#3           	Brand#33  	LARGE BURNISHED COPPER	36	SM CASE   	1537.48	ounts about the ca
152486	forest dim khaki firebrick antique	Manufacturer#5           	Brand#54  	SMALL BURNISHED BRASS	32	LG CAN    	1538.48	ts are. blithe de
152489	bisque tomato purple seashell chartreuse	Manufacturer#5           	Brand#51  	STANDARD POLISHED NICKEL	37	SM BAG    	1541.48	press packages 
152490	mint chocolate snow lawn azure	Manufacturer#3           	Brand#35  	ECONOMY POLISHED STEEL	34	SM BAG    	1542.49	 escapades h
152494	plum purple drab forest olive	Manufacturer#2           	Brand#23  	ECONOMY POLISHED TIN	46	LG BOX    	1546.49	usual i
152495	goldenrod lace hot burlywood royal	Manufacturer#3           	Brand#34  	PROMO ANODIZED COPPER	49	SM BAG    	1547.49	ly pending packages ag
152496	pink orange seashell floral puff	Manufacturer#4           	Brand#41  	MEDIUM POLISHED STEEL	47	LG BAG    	1548.49	slyly express packages
152500	blue honeydew goldenrod turquoise tan	Manufacturer#4           	Brand#42  	SMALL PLATED TIN	16	SM PACK   	1552.50	en foxes slee
152501	peach peru slate cyan dim	Manufacturer#5           	Brand#55  	STANDARD BURNISHED COPPER	22	SM DRUM   	1553.50	 pint
152502	white rosy cream linen yellow	Manufacturer#4           	Brand#45  	LARGE BURNISHED COPPER	8	JUMBO PKG 	1554.50	unts oug
152503	papaya goldenrod sky azure burnished	Manufacturer#4           	Brand#44  	PROMO ANODIZED TIN	9	MED CASE  	1555.50	o beans. s
154970	moccasin peach antique honeydew medium	Manufacturer#2           	Brand#25  	ECONOMY BRUSHED NICKEL	41	JUMBO CASE	2024.97	ounts. quickly regu
154973	chocolate magenta pale medium powder	Manufacturer#5           	Brand#53  	MEDIUM PLATED COPPER	44	LG PACK   	2027.97	he instructions. pa
154975	chocolate light almond rosy yellow	Manufacturer#1           	Brand#11  	STANDARD ANODIZED STEEL	22	WRAP PACK 	2029.97	even dolphins integr
154976	navy thistle deep bisque navajo	Manufacturer#4           	Brand#41  	LARGE PLATED COPPER	27	LG DRUM   	2030.97	enticing
154977	blush sienna beige grey slate	Manufacturer#5           	Brand#53  	SMALL POLISHED COPPER	25	WRAP PKG  	2031.97	 pending packages. i
154979	goldenrod deep red steel orange	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED STEEL	15	MED DRUM  	2033.97	 accounts. f
154980	chartreuse green slate azure orchid	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	13	MED JAR   	2034.98	as agains
154983	lawn puff moccasin ivory turquoise	Manufacturer#2           	Brand#22  	PROMO POLISHED TIN	44	MED CAN   	2037.98	cajole s
154985	chiffon drab yellow peach grey	Manufacturer#3           	Brand#32  	STANDARD BURNISHED BRASS	8	JUMBO PACK	2039.98	ular 
154986	violet lawn dark cornsilk burnished	Manufacturer#2           	Brand#24  	PROMO PLATED COPPER	22	JUMBO BOX 	2040.98	t the qui
154987	chocolate gainsboro cornflower olive ghost	Manufacturer#1           	Brand#15  	PROMO BURNISHED STEEL	11	JUMBO BOX 	2041.98	xpress, unusual r
154988	midnight pale brown black blanched	Manufacturer#2           	Brand#23  	ECONOMY BRUSHED TIN	40	MED JAR   	2042.98	slyly regu
154994	moccasin linen gainsboro rosy yellow	Manufacturer#2           	Brand#21  	STANDARD POLISHED BRASS	17	LG CASE   	2048.99	 carefully re
154995	magenta black navy violet pink	Manufacturer#2           	Brand#23  	MEDIUM POLISHED TIN	47	WRAP BOX  	2049.99	accounts cajole 
154996	chocolate pink moccasin antique rose	Manufacturer#1           	Brand#15  	ECONOMY POLISHED STEEL	30	JUMBO PKG 	2050.99	 dependencies. blithel
154999	sky grey burlywood snow green	Manufacturer#3           	Brand#31  	STANDARD ANODIZED COPPER	7	WRAP CAN  	2053.99	its ha
155000	ivory metallic powder deep light	Manufacturer#2           	Brand#23  	SMALL BRUSHED COPPER	27	JUMBO BOX 	1055.00	. accounts wake. q
155001	maroon misty bisque indian cornsilk	Manufacturer#2           	Brand#23  	PROMO ANODIZED NICKEL	47	WRAP PKG  	1056.00	sleep furiously ca
155004	steel sienna firebrick chiffon cornsilk	Manufacturer#2           	Brand#21  	SMALL BURNISHED TIN	13	SM CAN    	1059.00	 bold req
155005	green pale slate royal antique	Manufacturer#3           	Brand#32  	STANDARD BRUSHED BRASS	21	WRAP PACK 	1060.00	ise blithely alongs
155009	peach brown rosy light blue	Manufacturer#1           	Brand#12  	PROMO ANODIZED COPPER	3	LG CASE   	1064.00	regular dep
155010	forest sky ghost peach black	Manufacturer#2           	Brand#22  	SMALL BRUSHED TIN	7	WRAP PACK 	1065.01	slyly express 
155011	tan powder snow peru navy	Manufacturer#2           	Brand#22  	LARGE ANODIZED STEEL	42	SM PACK   	1066.01	 pinto beans
155015	azure seashell orchid lawn metallic	Manufacturer#1           	Brand#14  	ECONOMY POLISHED BRASS	23	MED CASE  	1070.01	unts was car
155016	plum sienna gainsboro lavender aquamarine	Manufacturer#4           	Brand#43  	STANDARD POLISHED BRASS	45	SM CASE   	1071.01	es doubt accoun
155017	indian sandy red spring dodger	Manufacturer#3           	Brand#35  	SMALL BURNISHED TIN	37	MED JAR   	1072.01	cording to the furious
155018	orange mint rosy pink chiffon	Manufacturer#3           	Brand#31  	PROMO ANODIZED BRASS	35	JUMBO PACK	1073.01	inments. f
157485	sandy azure orchid green tomato	Manufacturer#4           	Brand#44  	LARGE BURNISHED NICKEL	35	JUMBO JAR 	1542.48	lly unusu
157488	cornsilk azure moccasin chiffon ghost	Manufacturer#1           	Brand#15  	PROMO BRUSHED STEEL	26	LG CASE   	1545.48	s wake bravely 
157490	light cornsilk forest antique honeydew	Manufacturer#3           	Brand#33  	ECONOMY ANODIZED TIN	46	MED PACK  	1547.49	. sometimes pending fo
157491	cornflower rosy deep salmon pale	Manufacturer#2           	Brand#23  	PROMO ANODIZED NICKEL	50	SM DRUM   	1548.49	rate furi
157492	almond burlywood olive tan medium	Manufacturer#5           	Brand#55  	SMALL BRUSHED STEEL	50	LG BOX    	1549.49	ly even ac
157494	bisque wheat cream antique saddle	Manufacturer#3           	Brand#33  	MEDIUM POLISHED STEEL	16	WRAP JAR  	1551.49	its. caref
157495	snow moccasin lemon antique almond	Manufacturer#4           	Brand#42  	SMALL BURNISHED BRASS	33	SM JAR    	1552.49	ly slyly stealthy r
157498	black cyan metallic mint antique	Manufacturer#4           	Brand#41  	ECONOMY BRUSHED COPPER	39	WRAP CAN  	1555.49	uses. packages use f
157500	midnight yellow deep rose lavender	Manufacturer#5           	Brand#52  	SMALL POLISHED COPPER	3	WRAP CASE 	1557.50	 furiously regula
157501	red gainsboro violet grey aquamarine	Manufacturer#2           	Brand#21  	ECONOMY BURNISHED BRASS	44	WRAP JAR  	1558.50	ickly regul
157502	yellow ivory powder lawn peach	Manufacturer#2           	Brand#24  	LARGE PLATED BRASS	28	SM CASE   	1559.50	 theodolite
157503	pale metallic almond linen cream	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED BRASS	35	SM JAR    	1560.50	asymptot
157509	chocolate violet chartreuse dark ivory	Manufacturer#1           	Brand#15  	STANDARD BRUSHED NICKEL	30	SM CASE   	1566.50	ncies sleep among the 
157510	chartreuse dim lemon tomato sienna	Manufacturer#1           	Brand#14  	STANDARD ANODIZED STEEL	11	JUMBO CASE	1567.51	old deposit
157511	chocolate beige papaya turquoise midnight	Manufacturer#1           	Brand#14  	MEDIUM ANODIZED NICKEL	32	MED JAR   	1568.51	asymptotes 
157514	lime dim saddle pink lemon	Manufacturer#2           	Brand#25  	MEDIUM PLATED NICKEL	11	SM CASE   	1571.51	tegrate f
157515	maroon navajo hot linen midnight	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED COPPER	46	MED JAR   	1572.51	nal account
157516	slate thistle honeydew rose yellow	Manufacturer#3           	Brand#31  	LARGE POLISHED COPPER	5	SM CAN    	1573.51	tes. furiou
157519	khaki royal tan blue black	Manufacturer#5           	Brand#55  	MEDIUM PLATED STEEL	11	JUMBO BAG 	1576.51	p bold, fin
157520	lace dim pale misty olive	Manufacturer#1           	Brand#14  	SMALL BRUSHED STEEL	2	SM JAR    	1577.52	sly regularly
157524	forest drab slate deep coral	Manufacturer#3           	Brand#32  	STANDARD BURNISHED NICKEL	38	SM BAG    	1581.52	 spec
157525	saddle forest green sienna magenta	Manufacturer#4           	Brand#45  	SMALL PLATED NICKEL	27	LG PKG    	1582.52	 the quickly final pin
157526	blue lace cream saddle beige	Manufacturer#3           	Brand#33  	STANDARD BRUSHED TIN	17	SM BOX    	1583.52	sts wake s
157530	aquamarine moccasin honeydew turquoise lawn	Manufacturer#4           	Brand#42  	PROMO BURNISHED NICKEL	18	SM DRUM   	1587.53	 ironic ideas
157531	cyan deep green brown seashell	Manufacturer#2           	Brand#21  	SMALL POLISHED NICKEL	50	LG JAR    	1588.53	slyly unusual packages
157532	mint medium brown cornflower lemon	Manufacturer#1           	Brand#12  	MEDIUM PLATED BRASS	2	JUMBO JAR 	1589.53	es haggle against 
157533	saddle blanched salmon lime sienna	Manufacturer#4           	Brand#43  	PROMO PLATED STEEL	19	WRAP BAG  	1590.53	inal dolphins. 
160000	cornsilk moccasin thistle orchid honeydew	Manufacturer#4           	Brand#43  	ECONOMY POLISHED STEEL	13	SM CAN    	1060.00	t the furiously re
160003	medium black blue steel frosted	Manufacturer#2           	Brand#21  	PROMO BURNISHED STEEL	50	WRAP BOX  	1063.00	to beans should ha
160005	medium orchid grey frosted orange	Manufacturer#3           	Brand#32  	LARGE ANODIZED BRASS	8	JUMBO CASE	1065.00	hely iro
160006	lime navy maroon indian drab	Manufacturer#2           	Brand#24  	STANDARD BURNISHED STEEL	50	JUMBO JAR 	1066.00	 foxes cajole quic
160007	forest turquoise purple blush cornflower	Manufacturer#5           	Brand#52  	LARGE POLISHED COPPER	17	MED PKG   	1067.00	ronic instructions
160009	metallic violet antique puff seashell	Manufacturer#4           	Brand#44  	MEDIUM POLISHED COPPER	11	MED JAR   	1069.00	s requests detect 
160010	burnished green powder deep black	Manufacturer#5           	Brand#54  	MEDIUM ANODIZED NICKEL	6	WRAP BAG  	1070.01	arefully even ideas 
160013	violet medium dodger steel midnight	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	11	LG CASE   	1073.01	he bli
160015	royal lawn powder red brown	Manufacturer#4           	Brand#45  	PROMO POLISHED NICKEL	44	MED PKG   	1075.01	 the final, 
160016	lavender purple plum light almond	Manufacturer#1           	Brand#14  	PROMO PLATED STEEL	46	LG BAG    	1076.01	sits. boldly unu
160017	grey blush dodger lawn royal	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED TIN	11	SM PKG    	1077.01	 blithely ironic d
160018	khaki azure white snow aquamarine	Manufacturer#2           	Brand#23  	STANDARD ANODIZED COPPER	24	LG PACK   	1078.01	 ideas. 
160024	drab rosy lawn metallic snow	Manufacturer#4           	Brand#45  	STANDARD BURNISHED BRASS	26	MED CASE  	1084.02	quests. 
160025	rosy metallic cornflower thistle gainsboro	Manufacturer#2           	Brand#24  	LARGE BURNISHED COPPER	34	LG DRUM   	1085.02	ld, bold excuses hin
160026	salmon dark chiffon peach smoke	Manufacturer#4           	Brand#45  	MEDIUM PLATED TIN	38	LG CAN    	1086.02	ake q
160029	maroon grey snow peru midnight	Manufacturer#5           	Brand#55  	PROMO BURNISHED COPPER	32	JUMBO PKG 	1089.02	silen
160030	chartreuse bisque blush lime snow	Manufacturer#3           	Brand#32  	ECONOMY PLATED BRASS	39	SM CASE   	1090.03	ackages.
160031	cornflower bisque burlywood deep lace	Manufacturer#5           	Brand#52  	PROMO BRUSHED COPPER	9	LG JAR    	1091.03	 pinto beans affi
160034	chartreuse lime blush honeydew dodger	Manufacturer#4           	Brand#43  	MEDIUM PLATED NICKEL	4	WRAP PKG  	1094.03	ans a
160035	white orchid papaya lace cornsilk	Manufacturer#2           	Brand#21  	STANDARD ANODIZED TIN	18	MED PACK  	1095.03	encies.
160039	ghost linen bisque turquoise cream	Manufacturer#4           	Brand#41  	SMALL BRUSHED NICKEL	17	WRAP CAN  	1099.03	platelets 
160040	red brown powder cornflower bisque	Manufacturer#5           	Brand#53  	MEDIUM ANODIZED STEEL	6	WRAP BAG  	1100.04	s across the
160041	lavender chartreuse bisque pink chocolate	Manufacturer#2           	Brand#21  	STANDARD PLATED BRASS	41	JUMBO DRUM	1101.04	ts. quickly ironi
160045	metallic dodger cyan saddle sky	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED COPPER	24	SM PKG    	1105.04	refully even acco
160046	medium peach sienna steel tan	Manufacturer#3           	Brand#32  	PROMO BURNISHED STEEL	48	WRAP CASE 	1106.04	 instru
160047	plum misty brown yellow salmon	Manufacturer#2           	Brand#25  	PROMO ANODIZED TIN	6	SM PKG    	1107.04	e unusual p
160048	moccasin powder gainsboro firebrick red	Manufacturer#1           	Brand#15  	LARGE PLATED STEEL	41	MED JAR   	1108.04	 nag quickl
162452	lace brown powder orchid tomato	Manufacturer#5           	Brand#52  	MEDIUM POLISHED COPPER	22	MED JAR   	1514.45	t, pending requ
162455	puff cyan saddle peru plum	Manufacturer#4           	Brand#41  	LARGE BRUSHED TIN	32	JUMBO CASE	1517.45	nts. furiously regu
162457	cornflower maroon steel blue deep	Manufacturer#4           	Brand#41  	SMALL BRUSHED NICKEL	36	JUMBO DRUM	1519.45	thely qu
162458	smoke powder olive goldenrod sandy	Manufacturer#5           	Brand#55  	ECONOMY BRUSHED COPPER	14	LG DRUM   	1520.45	 requests about th
162459	white rosy yellow magenta firebrick	Manufacturer#5           	Brand#53  	PROMO ANODIZED COPPER	36	SM BOX    	1521.45	ons. sile
162461	white steel blue grey peru	Manufacturer#2           	Brand#23  	MEDIUM PLATED COPPER	1	SM BAG    	1523.46	 foxes boost c
162462	gainsboro coral blanched maroon chocolate	Manufacturer#5           	Brand#52  	ECONOMY POLISHED BRASS	18	LG BAG    	1524.46	 foxes x-ray 
162465	chartreuse plum indian lime red	Manufacturer#2           	Brand#23  	PROMO POLISHED COPPER	13	SM BAG    	1527.46	usly above the blit
162467	coral floral peru goldenrod green	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED NICKEL	35	MED BOX   	1529.46	al excuses. re
162468	yellow navajo almond midnight gainsboro	Manufacturer#4           	Brand#42  	SMALL POLISHED STEEL	10	SM CAN    	1530.46	ts haggle slyly-
162469	beige orange purple frosted chiffon	Manufacturer#1           	Brand#15  	PROMO BRUSHED NICKEL	23	JUMBO DRUM	1531.46	even packages
162470	snow ghost bisque forest navy	Manufacturer#4           	Brand#42  	PROMO POLISHED TIN	10	WRAP CAN  	1532.47	osits. furiously bol
162476	cyan red metallic ghost sandy	Manufacturer#2           	Brand#25  	ECONOMY POLISHED COPPER	26	SM BAG    	1538.47	carefully p
162477	brown snow rosy sandy smoke	Manufacturer#3           	Brand#33  	LARGE BRUSHED BRASS	35	SM CAN    	1539.47	le quic
162478	linen magenta dark indian chartreuse	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED STEEL	6	WRAP BAG  	1540.47	nding grouches. c
162481	coral linen peach cyan maroon	Manufacturer#4           	Brand#42  	PROMO PLATED STEEL	45	SM CASE   	1543.48	ngside of the ins
162482	white sky steel wheat purple	Manufacturer#1           	Brand#12  	MEDIUM BURNISHED TIN	47	SM PACK   	1544.48	l requests sleep fluff
162483	chartreuse indian orchid lawn drab	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED STEEL	50	LG BOX    	1545.48	ions. furiously quiet 
162486	hot pale sienna deep ghost	Manufacturer#5           	Brand#53  	STANDARD BURNISHED TIN	8	MED CASE  	1548.48	equests dete
162487	black orange tan metallic firebrick	Manufacturer#4           	Brand#43  	SMALL BURNISHED STEEL	20	LG JAR    	1549.48	ing accounts cajo
162491	royal purple lace rosy navy	Manufacturer#5           	Brand#53  	PROMO POLISHED NICKEL	34	JUMBO PKG 	1553.49	ironic theodolites. f
162492	mint purple sandy drab dodger	Manufacturer#4           	Brand#43  	MEDIUM BRUSHED BRASS	45	SM CAN    	1554.49	ggle: furiously ir
162493	seashell puff frosted black chartreuse	Manufacturer#5           	Brand#54  	SMALL BURNISHED BRASS	11	MED PACK  	1555.49	lent deposits h
162497	thistle pink maroon sienna lime	Manufacturer#5           	Brand#55  	LARGE POLISHED BRASS	17	SM JAR    	1559.49	deposits slee
162498	olive deep floral peach chocolate	Manufacturer#1           	Brand#13  	STANDARD ANODIZED TIN	37	JUMBO CAN 	1560.49	ts cajole blithely e
162499	blanched khaki floral chocolate misty	Manufacturer#4           	Brand#42  	SMALL PLATED TIN	15	JUMBO JAR 	1561.49	r. slyly fina
162500	saddle pink green blush lawn	Manufacturer#1           	Brand#14  	LARGE PLATED TIN	10	SM BOX    	1562.50	rd the blithely sile
164968	sienna violet linen grey chartreuse	Manufacturer#2           	Brand#24  	SMALL PLATED NICKEL	16	SM PACK   	2032.96	aves dazzle. final, re
164971	seashell hot orchid lime grey	Manufacturer#1           	Brand#15  	STANDARD PLATED TIN	9	MED PACK  	2035.97	packages impress
164973	mint chartreuse grey violet beige	Manufacturer#1           	Brand#12  	MEDIUM BRUSHED COPPER	3	JUMBO PKG 	2037.97	ly final depo
164974	indian cream olive metallic firebrick	Manufacturer#2           	Brand#21  	MEDIUM POLISHED BRASS	40	WRAP DRUM 	2038.97	hlessly packag
164975	khaki drab grey green blush	Manufacturer#5           	Brand#53  	MEDIUM BURNISHED BRASS	30	WRAP BAG  	2039.97	 pending packages are.
164977	goldenrod peru blush pale sienna	Manufacturer#3           	Brand#35  	LARGE ANODIZED TIN	8	SM PACK   	2041.97	y slyly unusual orbits
164978	ghost thistle blanched pink blue	Manufacturer#3           	Brand#35  	MEDIUM ANODIZED NICKEL	33	LG PACK   	2042.97	ideas across the 
164981	snow ivory white light salmon	Manufacturer#3           	Brand#35  	STANDARD POLISHED COPPER	27	WRAP BOX  	2045.98	ly stealthy 
164983	lace navy rosy violet green	Manufacturer#4           	Brand#42  	PROMO POLISHED COPPER	32	LG PKG    	2047.98	ets wake f
164984	lawn saddle rose drab chocolate	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED BRASS	15	LG BAG    	2048.98	eodolites nag re
164985	ivory rosy orchid linen firebrick	Manufacturer#1           	Brand#13  	STANDARD PLATED TIN	17	SM PKG    	2049.98	 across the regula
164986	light floral saddle chiffon spring	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED TIN	44	LG DRUM   	2050.98	r permanently a
164992	goldenrod puff navy coral pale	Manufacturer#1           	Brand#13  	STANDARD PLATED STEEL	15	JUMBO JAR 	2056.99	tions wake carefull
164993	mint rose beige lime blanched	Manufacturer#3           	Brand#35  	ECONOMY PLATED TIN	45	MED CASE  	2057.99	s cajole b
164994	chiffon lawn antique seashell puff	Manufacturer#5           	Brand#52  	SMALL POLISHED COPPER	45	MED CAN   	2058.99	gular
164997	chartreuse azure turquoise rosy burlywood	Manufacturer#3           	Brand#34  	STANDARD ANODIZED COPPER	41	WRAP CAN  	2061.99	refully regula
164998	cornflower khaki lawn chartreuse beige	Manufacturer#1           	Brand#11  	PROMO PLATED TIN	7	JUMBO PACK	2062.99	its. wa
164999	cornsilk white azure cream ivory	Manufacturer#2           	Brand#24  	SMALL BURNISHED NICKEL	46	WRAP DRUM 	2063.99	ncies. bus
165002	lime tan salmon lemon deep	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED TIN	33	WRAP PKG  	1067.00	ly final ideas h
165003	beige turquoise moccasin thistle forest	Manufacturer#5           	Brand#53  	LARGE PLATED TIN	28	JUMBO PKG 	1068.00	gains
165007	chocolate black lawn azure purple	Manufacturer#3           	Brand#34  	ECONOMY BRUSHED STEEL	49	LG BAG    	1072.00	hins maintain carefu
165008	navajo tomato orchid deep azure	Manufacturer#3           	Brand#31  	LARGE BRUSHED COPPER	22	SM JAR    	1073.00	gular, regu
165009	indian purple sandy rose plum	Manufacturer#5           	Brand#51  	STANDARD PLATED COPPER	12	SM PACK   	1074.00	leep a
165013	papaya medium honeydew deep antique	Manufacturer#4           	Brand#43  	PROMO ANODIZED COPPER	17	JUMBO CASE	1078.01	aggle blithely 
165014	sandy honeydew medium blanched maroon	Manufacturer#1           	Brand#15  	LARGE POLISHED COPPER	15	SM JAR    	1079.01	ongside of th
165015	smoke lavender hot spring dim	Manufacturer#1           	Brand#15  	STANDARD BURNISHED COPPER	12	WRAP JAR  	1080.01	fully blithe packa
165016	metallic ghost chocolate aquamarine sandy	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED TIN	6	LG BAG    	1081.01	rding to the car
167484	almond lavender magenta lemon green	Manufacturer#1           	Brand#12  	ECONOMY PLATED BRASS	11	WRAP PKG  	1551.48	carefully even r
167487	violet seashell yellow salmon light	Manufacturer#2           	Brand#22  	LARGE BURNISHED NICKEL	1	WRAP BAG  	1554.48	aggle 
167489	khaki burnished drab burlywood dim	Manufacturer#4           	Brand#42  	PROMO ANODIZED STEEL	28	WRAP DRUM 	1556.48	ts. express,
167490	frosted orange smoke pink slate	Manufacturer#5           	Brand#55  	ECONOMY ANODIZED BRASS	32	SM BAG    	1557.49	ly around the slyl
167491	hot lawn light ghost blanched	Manufacturer#3           	Brand#34  	PROMO BRUSHED BRASS	18	JUMBO BAG 	1558.49	slyly unus
167493	grey red khaki plum light	Manufacturer#5           	Brand#55  	ECONOMY BURNISHED TIN	18	MED CAN   	1560.49	telets. furi
167494	sienna honeydew burlywood burnished saddle	Manufacturer#5           	Brand#54  	SMALL POLISHED NICKEL	41	WRAP BOX  	1561.49	ly expre
167497	black cream indian turquoise lime	Manufacturer#2           	Brand#21  	SMALL BRUSHED STEEL	17	MED PKG   	1564.49	the ca
167499	burnished deep floral forest olive	Manufacturer#5           	Brand#53  	STANDARD POLISHED BRASS	29	LG BAG    	1566.49	s. furiously caref
167500	thistle chocolate sky navajo dim	Manufacturer#3           	Brand#32  	LARGE ANODIZED STEEL	14	LG CASE   	1567.50	fter the carefully 
167501	snow plum mint olive green	Manufacturer#3           	Brand#31  	PROMO PLATED TIN	44	WRAP CAN  	1568.50	 deposits
167502	blue moccasin medium indian magenta	Manufacturer#1           	Brand#14  	SMALL BRUSHED BRASS	4	WRAP BOX  	1569.50	eas. even req
167508	moccasin sandy purple mint medium	Manufacturer#5           	Brand#51  	STANDARD BRUSHED BRASS	31	SM JAR    	1575.50	e furiously. fur
167509	lace linen dark navajo sandy	Manufacturer#5           	Brand#51  	ECONOMY BRUSHED NICKEL	43	MED JAR   	1576.50	. expr
167510	goldenrod chartreuse papaya red dodger	Manufacturer#4           	Brand#44  	PROMO BRUSHED BRASS	50	SM PKG    	1577.51	even instr
167513	beige navajo violet smoke plum	Manufacturer#3           	Brand#33  	LARGE POLISHED STEEL	42	SM DRUM   	1580.51	nal accou
167514	snow forest lawn gainsboro dodger	Manufacturer#1           	Brand#14  	STANDARD PLATED COPPER	26	LG PKG    	1581.51	lly expre
167515	floral ghost chocolate bisque wheat	Manufacturer#4           	Brand#43  	ECONOMY BURNISHED TIN	50	WRAP DRUM 	1582.51	lyly fluffy asym
167518	tomato light brown magenta pale	Manufacturer#5           	Brand#52  	MEDIUM PLATED COPPER	2	LG PKG    	1585.51	gular packages. fl
167519	antique black dim almond smoke	Manufacturer#5           	Brand#55  	MEDIUM BURNISHED BRASS	34	MED BOX   	1586.51	 engage 
167523	lemon misty dodger dim chiffon	Manufacturer#2           	Brand#23  	STANDARD POLISHED NICKEL	47	LG JAR    	1590.52	uffily ironi
167524	frosted peru antique azure saddle	Manufacturer#3           	Brand#32  	ECONOMY POLISHED STEEL	5	WRAP BOX  	1591.52	ly final requests.
167525	royal lavender yellow maroon wheat	Manufacturer#2           	Brand#25  	PROMO PLATED NICKEL	22	SM BOX    	1592.52	 integrate 
167529	misty cream linen snow dodger	Manufacturer#2           	Brand#22  	PROMO POLISHED STEEL	29	WRAP JAR  	1596.52	ys. accounts eat.
167530	bisque saddle rose salmon blue	Manufacturer#3           	Brand#32  	SMALL BURNISHED NICKEL	15	MED CAN   	1597.53	lly bold i
167531	rose khaki red maroon smoke	Manufacturer#4           	Brand#44  	LARGE BRUSHED STEEL	15	LG PACK   	1598.53	s alon
167532	cyan wheat goldenrod medium navajo	Manufacturer#2           	Brand#25  	SMALL BURNISHED BRASS	48	WRAP PKG  	1599.53	boldly pending request
170000	tomato lemon burlywood chartreuse indian	Manufacturer#1           	Brand#11  	STANDARD BRUSHED COPPER	10	WRAP CASE 	1070.00	quests are ag
170003	purple lemon sienna brown metallic	Manufacturer#1           	Brand#12  	LARGE BURNISHED BRASS	15	MED CAN   	1073.00	ackages
170005	lawn beige lavender coral peru	Manufacturer#4           	Brand#44  	STANDARD PLATED TIN	19	JUMBO CAN 	1075.00	ly! exp
170006	tomato midnight ivory misty blanched	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED COPPER	34	WRAP JAR  	1076.00	ts. slyly
170007	papaya metallic peru yellow lemon	Manufacturer#2           	Brand#21  	SMALL ANODIZED TIN	35	WRAP JAR  	1077.00	ns se
170009	burnished hot cyan papaya grey	Manufacturer#1           	Brand#12  	STANDARD POLISHED NICKEL	10	WRAP PACK 	1079.00	cajole above
170010	blanched yellow antique slate thistle	Manufacturer#5           	Brand#52  	PROMO POLISHED STEEL	21	WRAP DRUM 	1080.01	e. express deposits 
170013	misty cyan grey red linen	Manufacturer#2           	Brand#22  	LARGE PLATED NICKEL	23	SM CAN    	1083.01	slyly final pint
170015	khaki saddle lavender spring peach	Manufacturer#4           	Brand#41  	ECONOMY ANODIZED BRASS	7	WRAP DRUM 	1085.01	fully 
170016	blue red white aquamarine olive	Manufacturer#3           	Brand#35  	STANDARD ANODIZED STEEL	34	WRAP CAN  	1086.01	ending frays.
170017	cornsilk grey almond orchid light	Manufacturer#2           	Brand#21  	ECONOMY PLATED TIN	7	WRAP PKG  	1087.01	lites alon
170018	wheat peru deep dark dodger	Manufacturer#1           	Brand#12  	PROMO BURNISHED BRASS	3	WRAP BAG  	1088.01	unusu
170024	smoke cyan medium saddle pink	Manufacturer#1           	Brand#15  	ECONOMY PLATED STEEL	7	SM BOX    	1094.02	y express pinto beans
170025	navajo brown royal wheat gainsboro	Manufacturer#2           	Brand#22  	MEDIUM PLATED BRASS	2	SM JAR    	1095.02	 furiously ironic theo
170026	firebrick smoke wheat ghost gainsboro	Manufacturer#2           	Brand#23  	STANDARD BURNISHED TIN	35	MED CASE  	1096.02	blithe
170029	cream cornflower lemon coral linen	Manufacturer#2           	Brand#22  	SMALL BURNISHED STEEL	45	JUMBO JAR 	1099.02	ckages. i
170030	aquamarine saddle sienna burlywood blue	Manufacturer#2           	Brand#25  	MEDIUM BURNISHED COPPER	14	MED DRUM  	1100.03	 slyly regular platele
170031	khaki coral navy burlywood brown	Manufacturer#3           	Brand#34  	MEDIUM PLATED TIN	39	MED PACK  	1101.03	sual 
170034	magenta metallic sandy bisque brown	Manufacturer#3           	Brand#33  	LARGE BRUSHED NICKEL	16	MED PACK  	1104.03	ing requests are int
170035	rosy green bisque almond seashell	Manufacturer#4           	Brand#45  	LARGE POLISHED COPPER	16	MED CASE  	1105.03	 silent, iro
170039	sky thistle rose magenta tomato	Manufacturer#5           	Brand#55  	LARGE BURNISHED TIN	47	JUMBO PACK	1109.03	. slyly fin
170040	tomato pale puff wheat blush	Manufacturer#1           	Brand#12  	SMALL POLISHED BRASS	31	MED CAN   	1110.04	e across the slyly
170041	cyan forest powder burnished seashell	Manufacturer#5           	Brand#54  	STANDARD PLATED NICKEL	34	LG BOX    	1111.04	nts cajole careful
170045	seashell antique coral gainsboro cyan	Manufacturer#5           	Brand#53  	PROMO PLATED COPPER	34	LG PKG    	1115.04	fluffily regu
170046	peach purple lemon beige tan	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED COPPER	3	SM PACK   	1116.04	its. instructions inte
170047	firebrick burlywood light turquoise orchid	Manufacturer#3           	Brand#31  	LARGE ANODIZED NICKEL	42	WRAP DRUM 	1117.04	 alongside of 
170048	rose khaki ghost peru wheat	Manufacturer#5           	Brand#53  	MEDIUM BRUSHED COPPER	16	WRAP JAR  	1118.04	cial packages haggle f
172449	wheat pale misty seashell cyan	Manufacturer#2           	Brand#21  	LARGE BRUSHED COPPER	39	SM CAN    	1521.44	ironic plate
172452	brown azure bisque wheat moccasin	Manufacturer#4           	Brand#45  	SMALL ANODIZED TIN	44	LG PACK   	1524.45	leep. furiou
172454	beige blanched midnight firebrick thistle	Manufacturer#2           	Brand#23  	MEDIUM BURNISHED BRASS	31	SM CAN    	1526.45	lar asymptotes
172455	light turquoise dark blush saddle	Manufacturer#2           	Brand#24  	SMALL PLATED NICKEL	50	WRAP CASE 	1527.45	capades wak
172456	burlywood grey cornflower red salmon	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED NICKEL	11	JUMBO CAN 	1528.45	es af
172458	lime light red aquamarine yellow	Manufacturer#1           	Brand#15  	LARGE BRUSHED COPPER	48	WRAP JAR  	1530.45	special packages
172459	maroon cream lime blue indian	Manufacturer#3           	Brand#31  	MEDIUM BURNISHED TIN	33	LG CASE   	1531.45	 speci
172462	ghost ivory cornsilk yellow dim	Manufacturer#2           	Brand#23  	SMALL BURNISHED TIN	48	WRAP PACK 	1534.46	accoun
172464	seashell turquoise salmon cornflower light	Manufacturer#4           	Brand#42  	STANDARD PLATED STEEL	22	SM DRUM   	1536.46	es. reg
172465	rosy green magenta olive burnished	Manufacturer#5           	Brand#52  	PROMO POLISHED COPPER	46	JUMBO CAN 	1537.46	tions. furiou
172466	salmon maroon powder white steel	Manufacturer#4           	Brand#41  	MEDIUM BURNISHED NICKEL	1	WRAP BAG  	1538.46	s. furiously unusu
172467	lawn spring pink lime maroon	Manufacturer#5           	Brand#51  	STANDARD BURNISHED STEEL	28	SM DRUM   	1539.46	ly. furious
172473	thistle floral ghost turquoise rosy	Manufacturer#3           	Brand#32  	SMALL ANODIZED STEEL	3	SM PACK   	1545.47	ld requests n
172474	blanched orange lime tomato midnight	Manufacturer#3           	Brand#32  	MEDIUM PLATED COPPER	30	SM PKG    	1546.47	 pinto beans are al
172475	antique dodger cyan linen lace	Manufacturer#1           	Brand#13  	PROMO BRUSHED TIN	5	SM CASE   	1547.47	lithely 
172478	navajo plum green dodger dark	Manufacturer#5           	Brand#53  	PROMO ANODIZED NICKEL	15	MED BOX   	1550.47	ickly a
172479	misty dim goldenrod papaya burnished	Manufacturer#4           	Brand#45  	LARGE POLISHED NICKEL	49	SM JAR    	1551.47	tegrate above the f
172480	burlywood olive antique brown almond	Manufacturer#3           	Brand#34  	SMALL ANODIZED TIN	23	LG JAR    	1552.48	ages d
172483	wheat deep brown dim seashell	Manufacturer#3           	Brand#35  	ECONOMY BURNISHED NICKEL	11	SM CAN    	1555.48	quests among the
172484	lime blush navy violet frosted	Manufacturer#1           	Brand#11  	PROMO BRUSHED BRASS	42	LG PACK   	1556.48	g dependen
172488	dodger aquamarine frosted deep lace	Manufacturer#1           	Brand#15  	STANDARD POLISHED TIN	45	SM PKG    	1560.48	iers boost blithel
172489	magenta salmon tomato royal midnight	Manufacturer#4           	Brand#44  	LARGE PLATED NICKEL	47	MED BAG   	1561.48	nding ideas ca
172490	navy chocolate turquoise sienna thistle	Manufacturer#3           	Brand#35  	ECONOMY ANODIZED NICKEL	50	JUMBO CAN 	1562.49	 deposits. expres
172494	hot saddle rose magenta tan	Manufacturer#2           	Brand#25  	STANDARD POLISHED NICKEL	41	LG BOX    	1566.49	s wake
172495	rose slate medium linen white	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED COPPER	37	MED BOX   	1567.49	sleep fluffi
172496	burlywood blanched sandy hot cream	Manufacturer#2           	Brand#25  	MEDIUM BRUSHED STEEL	23	WRAP BOX  	1568.49	bold t
172497	wheat almond papaya lawn peru	Manufacturer#4           	Brand#43  	MEDIUM BURNISHED BRASS	46	JUMBO DRUM	1569.49	y slyl
174966	navajo seashell cyan sandy violet	Manufacturer#3           	Brand#34  	ECONOMY POLISHED COPPER	11	WRAP DRUM 	2040.96	yly re
174969	turquoise peach thistle saddle sky	Manufacturer#2           	Brand#21  	MEDIUM ANODIZED NICKEL	7	LG PKG    	2043.96	slyly ironic
174971	seashell drab peach burnished royal	Manufacturer#4           	Brand#45  	MEDIUM BRUSHED NICKEL	17	LG PACK   	2045.97	 integrate slyly. d
174972	turquoise aquamarine navy sandy cornsilk	Manufacturer#5           	Brand#55  	STANDARD PLATED BRASS	4	LG JAR    	2046.97	e blithely slyly iro
174973	grey floral lemon pale ivory	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED BRASS	12	LG JAR    	2047.97	n deposits
174975	navajo lemon cornflower azure aquamarine	Manufacturer#1           	Brand#12  	PROMO ANODIZED STEEL	39	WRAP PACK 	2049.97	r the carefully expres
174976	grey dim gainsboro lace moccasin	Manufacturer#4           	Brand#44  	SMALL POLISHED NICKEL	41	JUMBO CAN 	2050.97	even, final pinto bean
174979	yellow red lime blanched cyan	Manufacturer#1           	Brand#12  	SMALL BRUSHED NICKEL	47	JUMBO DRUM	2053.97	lly final packages p
174981	royal maroon grey brown magenta	Manufacturer#4           	Brand#44  	ECONOMY BRUSHED STEEL	26	SM PKG    	2055.98	slyly. unusua
174982	thistle tan gainsboro dark forest	Manufacturer#3           	Brand#35  	STANDARD POLISHED COPPER	13	MED JAR   	2056.98	se express idea
174983	gainsboro saddle azure frosted floral	Manufacturer#1           	Brand#15  	MEDIUM BRUSHED TIN	17	JUMBO JAR 	2057.98	are slyl
174984	brown lawn blue cyan sienna	Manufacturer#1           	Brand#15  	STANDARD BURNISHED NICKEL	16	WRAP DRUM 	2058.98	y special
174990	seashell linen lavender lime coral	Manufacturer#1           	Brand#12  	SMALL BRUSHED COPPER	22	MED PACK  	2064.99	y. quickly
174991	coral dark misty linen blue	Manufacturer#4           	Brand#45  	STANDARD BURNISHED COPPER	38	MED CAN   	2065.99	ounts. 
174992	hot snow blue lime indian	Manufacturer#5           	Brand#54  	SMALL PLATED STEEL	13	WRAP BAG  	2066.99	 blithely unusua
174995	puff floral plum burnished dodger	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED TIN	11	MED BAG   	2069.99	pitaphs
174996	navajo ivory thistle sandy sienna	Manufacturer#2           	Brand#21  	LARGE POLISHED NICKEL	10	JUMBO DRUM	2070.99	ng the bl
174997	cornsilk pale yellow powder magenta	Manufacturer#2           	Brand#25  	ECONOMY PLATED TIN	36	SM BOX    	2071.99	quickly accor
175000	brown forest powder cream plum	Manufacturer#1           	Brand#11  	MEDIUM POLISHED COPPER	5	MED CAN   	1075.00	y special 
175001	medium cornsilk purple steel hot	Manufacturer#4           	Brand#45  	SMALL ANODIZED NICKEL	39	WRAP DRUM 	1076.00	ly caref
175005	burnished antique gainsboro blush azure	Manufacturer#1           	Brand#11  	STANDARD BRUSHED TIN	4	JUMBO CASE	1080.00	 around the blithely
175006	white maroon thistle orange pink	Manufacturer#2           	Brand#25  	STANDARD ANODIZED NICKEL	12	LG CASE   	1081.00	ross t
175007	navy forest beige peach ghost	Manufacturer#4           	Brand#44  	STANDARD POLISHED TIN	46	LG BAG    	1082.00	 mold fluffily. fin
175011	burlywood medium blue blanched almond	Manufacturer#4           	Brand#41  	PROMO POLISHED COPPER	43	JUMBO BAG 	1086.01	zzle fluffily fluffi
175012	forest seashell cyan brown hot	Manufacturer#3           	Brand#33  	PROMO ANODIZED NICKEL	16	MED JAR   	1087.01	s abou
175013	medium turquoise coral rosy cream	Manufacturer#1           	Brand#13  	SMALL BURNISHED STEEL	47	MED PKG   	1088.01	e blithely bold ideas.
175014	saddle magenta spring misty ivory	Manufacturer#3           	Brand#31  	PROMO BURNISHED COPPER	19	WRAP CAN  	1089.01	c ideas inte
177483	brown navy blue hot aquamarine	Manufacturer#4           	Brand#44  	STANDARD BURNISHED TIN	23	WRAP JAR  	1560.48	rouches sleep unusua
177486	black chiffon ivory purple blue	Manufacturer#4           	Brand#44  	ECONOMY POLISHED STEEL	3	LG DRUM   	1563.48	x bold pinto be
177488	cornflower firebrick beige powder spring	Manufacturer#4           	Brand#42  	LARGE BURNISHED COPPER	12	SM PACK   	1565.48	. flu
177489	papaya sandy peru olive misty	Manufacturer#1           	Brand#11  	MEDIUM BRUSHED COPPER	40	JUMBO PACK	1566.48	ckly special asy
177490	chartreuse slate goldenrod honeydew drab	Manufacturer#5           	Brand#53  	STANDARD POLISHED TIN	19	JUMBO CASE	1567.49	will boost above 
177492	aquamarine orchid powder almond thistle	Manufacturer#4           	Brand#42  	PROMO PLATED BRASS	20	WRAP PACK 	1569.49	nal pe
177493	aquamarine azure blush brown orchid	Manufacturer#4           	Brand#45  	PROMO POLISHED NICKEL	9	JUMBO BOX 	1570.49	ronic deposits grow 
177496	aquamarine puff mint sky light	Manufacturer#1           	Brand#11  	PROMO POLISHED STEEL	11	MED PKG   	1573.49	e blit
177498	medium cornsilk moccasin slate papaya	Manufacturer#2           	Brand#22  	MEDIUM BURNISHED TIN	16	WRAP PACK 	1575.49	e express foxe
177499	firebrick burnished midnight light cyan	Manufacturer#2           	Brand#23  	MEDIUM PLATED TIN	16	SM CASE   	1576.49	express
177500	lavender midnight gainsboro bisque dim	Manufacturer#5           	Brand#51  	LARGE BURNISHED TIN	4	WRAP BOX  	1577.50	ronic 
177501	mint puff salmon steel olive	Manufacturer#1           	Brand#14  	PROMO PLATED TIN	6	WRAP DRUM 	1578.50	uickl
177507	honeydew smoke yellow seashell forest	Manufacturer#3           	Brand#35  	SMALL BRUSHED BRASS	7	MED PKG   	1584.50	he ironic inst
177508	forest steel aquamarine beige olive	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	40	SM CASE   	1585.50	furious req
177509	chocolate hot thistle yellow dim	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	46	WRAP DRUM 	1586.50	fully express 
177512	orange purple chocolate rosy lavender	Manufacturer#2           	Brand#24  	MEDIUM PLATED COPPER	5	JUMBO JAR 	1589.51	ts. theodolites 
177513	chartreuse misty chocolate smoke salmon	Manufacturer#4           	Brand#45  	PROMO ANODIZED COPPER	49	WRAP PACK 	1590.51	wake quickly bol
177514	plum light almond wheat brown	Manufacturer#1           	Brand#15  	LARGE ANODIZED NICKEL	15	JUMBO JAR 	1591.51	 furi
177517	medium tomato maroon ivory cornsilk	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED STEEL	40	MED DRUM  	1594.51	ly bold
177518	steel thistle magenta floral blanched	Manufacturer#1           	Brand#15  	LARGE BRUSHED STEEL	1	MED PKG   	1595.51	gle blithely. regul
177522	cyan bisque saddle goldenrod firebrick	Manufacturer#2           	Brand#21  	ECONOMY BRUSHED TIN	33	LG PACK   	1599.52	sly ironi
177523	purple powder moccasin spring floral	Manufacturer#3           	Brand#31  	MEDIUM BRUSHED BRASS	30	SM BAG    	1600.52	counts. blithel
177524	misty moccasin gainsboro powder pale	Manufacturer#2           	Brand#25  	PROMO PLATED NICKEL	24	JUMBO CAN 	1601.52	usual req
177528	sandy steel puff salmon navy	Manufacturer#1           	Brand#15  	ECONOMY BURNISHED BRASS	15	MED BAG   	1605.52	t furiously. final pac
177529	blanched purple powder steel black	Manufacturer#2           	Brand#25  	SMALL POLISHED STEEL	2	JUMBO PACK	1606.52	gle fluffily pi
177530	salmon chartreuse mint drab lemon	Manufacturer#3           	Brand#32  	STANDARD PLATED NICKEL	20	MED PACK  	1607.53	unusual theodolite
177531	rosy papaya seashell honeydew metallic	Manufacturer#1           	Brand#15  	ECONOMY PLATED NICKEL	16	MED DRUM  	1608.53	 ideas. 
180000	violet tomato almond navy cream	Manufacturer#3           	Brand#34  	STANDARD BRUSHED TIN	32	WRAP CASE 	1080.00	 regular packages. 
180003	burnished pale lawn misty indian	Manufacturer#2           	Brand#22  	PROMO BURNISHED TIN	13	WRAP CASE 	1083.00	riously ironic id
180005	burlywood cyan sky spring dark	Manufacturer#5           	Brand#54  	MEDIUM PLATED NICKEL	39	LG PACK   	1085.00	encies print furiou
180006	lemon pink pale yellow tomato	Manufacturer#1           	Brand#12  	SMALL BURNISHED STEEL	35	LG JAR    	1086.00	 theo
180007	rose aquamarine lime thistle deep	Manufacturer#2           	Brand#21  	PROMO BRUSHED COPPER	50	WRAP BAG  	1087.00	usly against 
180009	tomato red white pale sandy	Manufacturer#5           	Brand#53  	LARGE BRUSHED TIN	50	SM BAG    	1089.00	unts are slyly a
180010	midnight navajo brown violet goldenrod	Manufacturer#4           	Brand#45  	ECONOMY BURNISHED TIN	28	MED CAN   	1090.01	 deposits affix carefu
180013	slate wheat dodger drab hot	Manufacturer#3           	Brand#34  	STANDARD ANODIZED STEEL	22	LG BOX    	1093.01	 instructions. blith
180015	purple orchid lawn rose goldenrod	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED TIN	12	JUMBO JAR 	1095.01	pecial packages ca
180016	hot papaya lavender purple deep	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED NICKEL	49	LG BOX    	1096.01	ng deposits 
180017	forest slate grey steel spring	Manufacturer#3           	Brand#33  	ECONOMY PLATED STEEL	7	MED CASE  	1097.01	r accounts nag 
180018	blush lace violet lemon navajo	Manufacturer#2           	Brand#21  	LARGE ANODIZED TIN	4	SM DRUM   	1098.01	ngsid
180024	deep hot chartreuse dim ivory	Manufacturer#5           	Brand#51  	MEDIUM POLISHED BRASS	25	LG DRUM   	1104.02	onic foxes. 
180025	sandy rose peru azure deep	Manufacturer#2           	Brand#23  	LARGE BRUSHED BRASS	21	JUMBO PACK	1105.02	ly regular the
180026	powder smoke cream burlywood indian	Manufacturer#1           	Brand#11  	SMALL BRUSHED STEEL	30	LG BOX    	1106.02	tly express theodol
180029	ghost moccasin mint lemon drab	Manufacturer#2           	Brand#23  	MEDIUM POLISHED BRASS	8	LG BOX    	1109.02	 quickly unusual
180030	moccasin tan almond sienna red	Manufacturer#1           	Brand#12  	PROMO BRUSHED STEEL	41	LG CASE   	1110.03	ly ironic foxe
180031	lavender lime purple lemon orange	Manufacturer#4           	Brand#43  	LARGE PLATED COPPER	17	JUMBO PACK	1111.03	usy dependencies wa
180034	rose goldenrod mint purple ivory	Manufacturer#3           	Brand#32  	LARGE BURNISHED BRASS	19	LG DRUM   	1114.03	 fluffily at the clos
180035	moccasin turquoise orange royal saddle	Manufacturer#2           	Brand#23  	ECONOMY PLATED NICKEL	2	JUMBO CAN 	1115.03	e furiously even 
180039	azure dodger dim chocolate floral	Manufacturer#3           	Brand#32  	SMALL ANODIZED COPPER	4	MED JAR   	1119.03	 carefully fur
180040	frosted light dodger blush bisque	Manufacturer#5           	Brand#55  	MEDIUM ANODIZED NICKEL	36	WRAP PACK 	1120.04	sly am
180041	deep orange steel firebrick rosy	Manufacturer#4           	Brand#41  	ECONOMY BURNISHED COPPER	20	LG CAN    	1121.04	, specia
180045	coral slate sienna pale navy	Manufacturer#5           	Brand#51  	MEDIUM ANODIZED NICKEL	12	SM JAR    	1125.04	theodolites.
180046	metallic goldenrod antique blanched red	Manufacturer#5           	Brand#55  	SMALL BURNISHED BRASS	35	LG PACK   	1126.04	uriously above the
180047	cornflower purple sandy green blanched	Manufacturer#5           	Brand#54  	PROMO POLISHED COPPER	24	JUMBO CAN 	1127.04	sual ac
180048	puff blue royal midnight azure	Manufacturer#4           	Brand#44  	SMALL PLATED BRASS	1	MED PACK  	1128.04	ar requests; q
182446	firebrick indian seashell dark dim	Manufacturer#5           	Brand#52  	ECONOMY POLISHED STEEL	7	JUMBO BOX 	1528.44	 furiously qui
182449	slate navajo midnight lemon cornflower	Manufacturer#5           	Brand#51  	SMALL ANODIZED STEEL	27	JUMBO PKG 	1531.44	egular packages. instr
182451	tomato navajo navy papaya lawn	Manufacturer#4           	Brand#42  	STANDARD BRUSHED NICKEL	4	JUMBO PACK	1533.45	arefully carefu
182452	deep mint hot pink aquamarine	Manufacturer#1           	Brand#11  	MEDIUM PLATED NICKEL	45	WRAP PACK 	1534.45	ajole blithely u
182453	misty purple navy white rosy	Manufacturer#2           	Brand#23  	MEDIUM POLISHED NICKEL	46	MED BAG   	1535.45	 foxe
182455	grey white tomato burnished yellow	Manufacturer#4           	Brand#45  	STANDARD BRUSHED STEEL	5	WRAP CASE 	1537.45	al in
182456	chocolate dim light hot saddle	Manufacturer#1           	Brand#12  	PROMO ANODIZED STEEL	15	MED BAG   	1538.45	e. slyly r
182459	drab cream peach ghost navajo	Manufacturer#1           	Brand#12  	PROMO BRUSHED NICKEL	17	LG BOX    	1541.45	s. pending acc
182461	sky floral cornsilk light rosy	Manufacturer#3           	Brand#31  	PROMO BURNISHED NICKEL	36	JUMBO CAN 	1543.46	pending packages 
182462	blush peach cream lavender gainsboro	Manufacturer#3           	Brand#34  	PROMO ANODIZED COPPER	18	MED BAG   	1544.46	blithely bold d
182463	maroon chartreuse rose cornflower goldenrod	Manufacturer#4           	Brand#44  	MEDIUM PLATED STEEL	32	WRAP CAN  	1545.46	the blithely bold fox
182464	peach plum hot tomato purple	Manufacturer#5           	Brand#53  	SMALL BURNISHED TIN	37	LG BAG    	1546.46	 ironi
182470	grey hot orchid spring steel	Manufacturer#2           	Brand#24  	ECONOMY POLISHED STEEL	33	LG PACK   	1552.47	 foxes. 
182471	chiffon dim red grey gainsboro	Manufacturer#3           	Brand#34  	MEDIUM BURNISHED NICKEL	47	MED CAN   	1553.47	yly s
182472	firebrick tomato black salmon peach	Manufacturer#4           	Brand#44  	LARGE PLATED NICKEL	50	LG PACK   	1554.47	the furiously fina
182475	chartreuse misty indian brown yellow	Manufacturer#1           	Brand#14  	MEDIUM POLISHED NICKEL	26	SM CASE   	1557.47	e of the blithel
182476	frosted black goldenrod tan lawn	Manufacturer#3           	Brand#34  	STANDARD BRUSHED TIN	45	WRAP CAN  	1558.47	luffily
182477	frosted floral cream sandy green	Manufacturer#2           	Brand#21  	ECONOMY POLISHED BRASS	29	WRAP BOX  	1559.47	packages. express d
182480	wheat moccasin tomato lavender yellow	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED TIN	35	WRAP PACK 	1562.48	 carefully ir
182481	honeydew linen magenta maroon burnished	Manufacturer#1           	Brand#11  	ECONOMY PLATED COPPER	10	LG DRUM   	1563.48	ges. express, fi
182485	sky peru blush cornsilk orchid	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED TIN	50	SM CAN    	1567.48	ackages
182486	seashell spring frosted midnight yellow	Manufacturer#4           	Brand#45  	STANDARD POLISHED COPPER	35	WRAP PKG  	1568.48	fully even deposits ca
182487	medium sandy ivory linen tomato	Manufacturer#3           	Brand#34  	MEDIUM PLATED NICKEL	32	SM CAN    	1569.48	nusual, even pac
182491	cornsilk rose blush navajo spring	Manufacturer#4           	Brand#43  	MEDIUM ANODIZED TIN	1	MED JAR   	1573.49	 wake alongside o
182492	orchid blue ivory honeydew gainsboro	Manufacturer#3           	Brand#33  	SMALL POLISHED BRASS	6	MED DRUM  	1574.49	s after the pend
182493	navy tan dim cyan lawn	Manufacturer#5           	Brand#52  	ECONOMY BURNISHED COPPER	32	WRAP DRUM 	1575.49	lyly around the
182494	cyan deep plum linen orange	Manufacturer#5           	Brand#53  	ECONOMY BRUSHED STEEL	16	WRAP PACK 	1576.49	haggle caref
184964	blush royal puff firebrick floral	Manufacturer#2           	Brand#23  	STANDARD PLATED COPPER	23	SM CASE   	2048.96	arefully at 
184967	burlywood aquamarine antique tomato frosted	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	45	MED PACK  	2051.96	y even packages unwin
184969	smoke antique cornflower spring lemon	Manufacturer#4           	Brand#44  	ECONOMY PLATED BRASS	2	MED DRUM  	2053.96	nts. ideas again
184970	magenta turquoise yellow dim midnight	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED BRASS	2	SM PACK   	2054.97	 haggle slyly abov
184971	moccasin gainsboro yellow burnished light	Manufacturer#2           	Brand#25  	SMALL POLISHED BRASS	41	LG PACK   	2055.97	ests t
184973	metallic olive aquamarine lawn tan	Manufacturer#3           	Brand#35  	PROMO POLISHED TIN	5	WRAP CASE 	2057.97	detect slyly a
184974	cornflower metallic firebrick lime aquamarine	Manufacturer#1           	Brand#14  	ECONOMY ANODIZED COPPER	27	WRAP CASE 	2058.97	ole slyly acro
184977	tomato wheat pale thistle beige	Manufacturer#1           	Brand#12  	ECONOMY PLATED NICKEL	8	MED PKG   	2061.97	ost sly
184979	brown puff sienna royal light	Manufacturer#4           	Brand#43  	STANDARD BURNISHED TIN	34	WRAP PACK 	2063.97	gle furiously
184980	almond dodger purple slate chiffon	Manufacturer#2           	Brand#23  	ECONOMY BURNISHED BRASS	7	JUMBO DRUM	2064.98	e bold, regular acco
184981	navajo khaki cyan forest sienna	Manufacturer#1           	Brand#13  	MEDIUM PLATED TIN	11	LG CAN    	2065.98	ans s
184982	lime mint medium honeydew plum	Manufacturer#3           	Brand#34  	PROMO PLATED COPPER	19	LG JAR    	2066.98	r deposits. carefu
184988	brown puff dark azure almond	Manufacturer#4           	Brand#44  	MEDIUM PLATED TIN	12	JUMBO BOX 	2072.98	 regular f
184989	floral lavender forest brown sienna	Manufacturer#4           	Brand#41  	LARGE PLATED COPPER	48	MED BAG   	2073.98	usly bold sentiment
184990	saddle medium thistle deep royal	Manufacturer#3           	Brand#32  	SMALL POLISHED NICKEL	6	MED BAG   	2074.99	ake furiously qui
184993	honeydew navajo orchid chiffon bisque	Manufacturer#1           	Brand#15  	STANDARD POLISHED BRASS	4	LG CASE   	2077.99	, even 
184994	cyan sky coral slate blue	Manufacturer#4           	Brand#42  	MEDIUM ANODIZED COPPER	24	LG BAG    	2078.99	inal, ironic 
184995	white thistle steel blanched purple	Manufacturer#2           	Brand#24  	SMALL POLISHED COPPER	7	MED PKG   	2079.99	wake carefully. ironi
184998	chiffon lace navajo moccasin maroon	Manufacturer#4           	Brand#43  	ECONOMY PLATED NICKEL	33	MED BOX   	2082.99	platelet
184999	antique burlywood indian royal midnight	Manufacturer#5           	Brand#54  	MEDIUM BURNISHED STEEL	17	SM CAN    	2083.99	ins haggle carefull
185003	maroon sky mint beige black	Manufacturer#1           	Brand#12  	ECONOMY POLISHED STEEL	47	MED BAG   	1088.00	 deposi
185004	sandy peru papaya brown navajo	Manufacturer#4           	Brand#42  	SMALL ANODIZED TIN	43	SM CASE   	1089.00	ructions play. furious
185005	turquoise moccasin beige sienna wheat	Manufacturer#5           	Brand#55  	PROMO BRUSHED BRASS	18	JUMBO PKG 	1090.00	 are carefully. reg
185009	violet green navy smoke sandy	Manufacturer#1           	Brand#12  	ECONOMY POLISHED COPPER	15	MED JAR   	1094.00	nto bean
185010	cornflower azure violet spring medium	Manufacturer#2           	Brand#24  	LARGE ANODIZED STEEL	31	SM BOX    	1095.01	efully final de
185011	deep saddle coral beige rose	Manufacturer#2           	Brand#25  	SMALL PLATED BRASS	13	MED PKG   	1096.01	 furiously ex
185012	cornsilk cyan bisque green khaki	Manufacturer#4           	Brand#43  	LARGE ANODIZED TIN	10	MED DRUM  	1097.01	 bean
187482	drab blush frosted khaki sandy	Manufacturer#1           	Brand#11  	ECONOMY ANODIZED TIN	37	LG BAG    	1569.48	gside of the quietly
187485	magenta snow seashell mint burnished	Manufacturer#4           	Brand#44  	SMALL POLISHED NICKEL	45	MED BAG   	1572.48	 players. bl
187487	rose moccasin pale salmon lawn	Manufacturer#2           	Brand#21  	MEDIUM BRUSHED TIN	28	JUMBO DRUM	1574.48	lar requests
187488	blanched hot papaya azure aquamarine	Manufacturer#5           	Brand#52  	PROMO PLATED NICKEL	49	WRAP CAN  	1575.48	y. stealthily 
187489	almond floral tan burnished ivory	Manufacturer#1           	Brand#13  	LARGE BRUSHED BRASS	38	WRAP DRUM 	1576.48	ly reg
187491	goldenrod gainsboro blanched frosted sandy	Manufacturer#1           	Brand#15  	LARGE ANODIZED STEEL	50	MED PACK  	1578.49	s war
187492	spring chocolate lawn beige chiffon	Manufacturer#5           	Brand#53  	LARGE PLATED COPPER	11	JUMBO CAN 	1579.49	 accounts accord
187495	lemon hot firebrick medium mint	Manufacturer#3           	Brand#35  	STANDARD BRUSHED COPPER	15	LG BAG    	1582.49	silent courts. expr
187497	cornflower royal black firebrick green	Manufacturer#2           	Brand#22  	SMALL ANODIZED COPPER	13	LG BOX    	1584.49	ng pinto beans! spec
187498	navy metallic cornflower dodger khaki	Manufacturer#4           	Brand#43  	ECONOMY BRUSHED NICKEL	7	SM BOX    	1585.49	old, fluffy depo
187499	cyan brown antique purple pale	Manufacturer#4           	Brand#45  	PROMO PLATED COPPER	23	SM BOX    	1586.49	old deposits a
187500	orange floral azure maroon coral	Manufacturer#4           	Brand#41  	MEDIUM PLATED TIN	48	MED BAG   	1587.50	refully special pint
187506	green tan cream moccasin red	Manufacturer#4           	Brand#42  	SMALL BURNISHED STEEL	1	LG BAG    	1593.50	uctions.
187507	dim turquoise beige thistle linen	Manufacturer#1           	Brand#14  	LARGE ANODIZED BRASS	22	LG BOX    	1594.50	f the blithely regul
187508	grey blush royal floral orchid	Manufacturer#3           	Brand#32  	PROMO BURNISHED COPPER	22	JUMBO JAR 	1595.50	ven requests. carefu
187511	green sky cornflower lemon burlywood	Manufacturer#1           	Brand#12  	MEDIUM PLATED TIN	14	MED JAR   	1598.51	 use slyly. special, u
187512	powder hot forest white orchid	Manufacturer#2           	Brand#21  	SMALL BURNISHED BRASS	8	WRAP PACK 	1599.51	. silent,
187513	firebrick sienna black goldenrod puff	Manufacturer#2           	Brand#22  	STANDARD BRUSHED STEEL	44	MED PKG   	1600.51	s haggle carefully bo
187516	deep puff drab lavender goldenrod	Manufacturer#3           	Brand#35  	PROMO BURNISHED STEEL	6	JUMBO BOX 	1603.51	ns. bol
187517	moccasin orchid puff aquamarine orange	Manufacturer#1           	Brand#13  	ECONOMY BRUSHED COPPER	25	JUMBO BOX 	1604.51	iously regular depo
187521	khaki deep sky midnight coral	Manufacturer#3           	Brand#32  	LARGE BURNISHED TIN	24	SM JAR    	1608.52	s: pin
187522	beige metallic burlywood sienna sky	Manufacturer#5           	Brand#52  	SMALL POLISHED TIN	47	WRAP BOX  	1609.52	ar packages beyond
187523	lavender honeydew olive dark chocolate	Manufacturer#1           	Brand#13  	STANDARD BRUSHED STEEL	48	MED PKG   	1610.52	g accordin
187527	azure light lime saddle metallic	Manufacturer#4           	Brand#45  	ECONOMY BRUSHED COPPER	38	MED JAR   	1614.52	ccounts. blithely 
187528	cream steel tomato hot frosted	Manufacturer#4           	Brand#45  	STANDARD BRUSHED BRASS	30	JUMBO CAN 	1615.52	essly bold packag
187529	almond ivory brown misty magenta	Manufacturer#5           	Brand#53  	STANDARD BURNISHED TIN	11	LG PKG    	1616.52	t tithes. bli
187530	royal tomato rosy purple forest	Manufacturer#4           	Brand#42  	LARGE POLISHED STEEL	4	LG PACK   	1617.53	y regula
190000	beige ivory magenta gainsboro linen	Manufacturer#2           	Brand#21  	MEDIUM PLATED COPPER	23	MED CASE  	1090.00	ly accor
190003	white salmon lemon cornsilk ghost	Manufacturer#4           	Brand#41  	PROMO ANODIZED TIN	41	LG BAG    	1093.00	ckages according to th
190005	slate indian forest chartreuse rosy	Manufacturer#1           	Brand#11  	SMALL BRUSHED BRASS	3	SM CASE   	1095.00	ly blithe, regula
190006	navajo lavender smoke puff olive	Manufacturer#5           	Brand#55  	SMALL BRUSHED BRASS	35	LG CASE   	1096.00	ilent ideas boo
190007	khaki lime goldenrod pink grey	Manufacturer#1           	Brand#11  	STANDARD PLATED BRASS	30	SM PKG    	1097.00	fully final gift
190009	orchid goldenrod metallic frosted powder	Manufacturer#3           	Brand#33  	STANDARD ANODIZED COPPER	25	LG BAG    	1099.00	es cajole f
190010	misty mint white seashell papaya	Manufacturer#3           	Brand#34  	STANDARD POLISHED STEEL	38	JUMBO BOX 	1100.01	pecia
190013	goldenrod peach honeydew blue violet	Manufacturer#3           	Brand#33  	MEDIUM ANODIZED BRASS	15	SM BAG    	1103.01	its. car
190015	linen blush chocolate orchid tan	Manufacturer#4           	Brand#41  	ECONOMY PLATED BRASS	50	JUMBO PKG 	1105.01	bold pains
190016	drab moccasin rosy turquoise hot	Manufacturer#3           	Brand#33  	ECONOMY BRUSHED COPPER	35	MED DRUM  	1106.01	 even asymptotes are 
190017	red turquoise blue aquamarine sky	Manufacturer#3           	Brand#31  	LARGE BURNISHED TIN	23	JUMBO PACK	1107.01	y even foxes! careful
190018	cornflower indian ivory spring navajo	Manufacturer#3           	Brand#35  	STANDARD PLATED COPPER	40	MED BAG   	1108.01	ilent request
190024	black azure forest cream lemon	Manufacturer#5           	Brand#52  	LARGE BRUSHED TIN	18	WRAP JAR  	1114.02	tructions nag 
190025	violet forest royal coral snow	Manufacturer#2           	Brand#21  	MEDIUM PLATED NICKEL	22	WRAP JAR  	1115.02	y regular requests. ir
190026	blush lemon peach chartreuse orange	Manufacturer#5           	Brand#54  	SMALL BURNISHED COPPER	35	MED PKG   	1116.02	oxes lose
190029	seashell floral smoke midnight yellow	Manufacturer#4           	Brand#42  	ECONOMY BRUSHED COPPER	25	MED PKG   	1119.02	le above the regu
190030	wheat red black lawn ivory	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED COPPER	24	SM JAR    	1120.03	 blithely ex
190031	dim pink snow midnight linen	Manufacturer#5           	Brand#55  	LARGE POLISHED STEEL	46	MED DRUM  	1121.03	efully pending p
190034	ivory tomato khaki gainsboro pale	Manufacturer#5           	Brand#51  	SMALL BURNISHED COPPER	18	LG BOX    	1124.03	inal courts are bli
190035	wheat orchid orange medium snow	Manufacturer#5           	Brand#54  	ECONOMY POLISHED NICKEL	37	JUMBO PKG 	1125.03	ously iron
190039	burnished rose midnight papaya lime	Manufacturer#4           	Brand#42  	MEDIUM BURNISHED STEEL	47	JUMBO BOX 	1129.03	packages
190040	forest ghost cream dodger royal	Manufacturer#4           	Brand#45  	SMALL PLATED NICKEL	4	SM BOX    	1130.04	 silent, 
190041	chocolate ivory blush white wheat	Manufacturer#5           	Brand#51  	STANDARD POLISHED COPPER	27	MED PKG   	1131.04	r the regu
190045	royal aquamarine purple almond brown	Manufacturer#2           	Brand#24  	STANDARD BURNISHED COPPER	38	WRAP CAN  	1135.04	ar deposi
190046	chiffon spring mint bisque sky	Manufacturer#1           	Brand#11  	ECONOMY PLATED STEEL	16	JUMBO PKG 	1136.04	fter the special 
190047	orange snow yellow midnight cyan	Manufacturer#5           	Brand#52  	MEDIUM POLISHED TIN	12	MED PKG   	1137.04	lites. close
190048	puff lemon floral thistle peach	Manufacturer#1           	Brand#15  	ECONOMY PLATED COPPER	29	SM BAG    	1138.04	lly express wa
192443	blush hot firebrick deep ghost	Manufacturer#3           	Brand#34  	MEDIUM ANODIZED BRASS	45	SM BOX    	1535.44	re ar
192446	royal turquoise misty burlywood plum	Manufacturer#1           	Brand#12  	ECONOMY ANODIZED BRASS	38	MED DRUM  	1538.44	ckly ironic theodoli
192448	plum blanched azure cornsilk sky	Manufacturer#1           	Brand#11  	ECONOMY BURNISHED BRASS	23	JUMBO BOX 	1540.44	quest
192449	metallic forest cyan cornflower rose	Manufacturer#2           	Brand#23  	ECONOMY POLISHED STEEL	4	LG DRUM   	1541.44	s are slyly packages? 
192450	dodger sandy frosted navajo aquamarine	Manufacturer#2           	Brand#22  	MEDIUM ANODIZED COPPER	45	WRAP CASE 	1542.45	s. quickl
192452	honeydew steel dark white hot	Manufacturer#5           	Brand#55  	LARGE BRUSHED TIN	47	MED BAG   	1544.45	 carefully express do
192453	tomato plum green burnished floral	Manufacturer#5           	Brand#52  	SMALL PLATED STEEL	29	SM BOX    	1545.45	osits. regular accoun
192456	medium ghost olive violet snow	Manufacturer#3           	Brand#34  	STANDARD POLISHED NICKEL	9	WRAP DRUM 	1548.45	xpress depende
192458	dodger salmon azure floral brown	Manufacturer#1           	Brand#13  	ECONOMY BURNISHED STEEL	30	SM PKG    	1550.45	 pinto beans. c
192459	sky magenta light dodger midnight	Manufacturer#2           	Brand#21  	PROMO PLATED STEEL	9	LG BOX    	1551.45	ke quickly 
192460	smoke dodger saddle lemon orchid	Manufacturer#2           	Brand#21  	MEDIUM PLATED COPPER	32	WRAP PACK 	1552.46	patterns must have to
192461	antique frosted saddle olive slate	Manufacturer#4           	Brand#42  	LARGE BURNISHED STEEL	22	JUMBO CASE	1553.46	 sleep 
192467	metallic chartreuse hot cream tomato	Manufacturer#5           	Brand#52  	PROMO PLATED COPPER	17	MED PACK  	1559.46	iously iro
192468	mint burlywood medium deep orange	Manufacturer#2           	Brand#25  	SMALL BURNISHED NICKEL	29	LG JAR    	1560.46	nst the blithel
192469	aquamarine blanched spring ghost hot	Manufacturer#2           	Brand#25  	SMALL PLATED COPPER	31	SM DRUM   	1561.46	ntegrate quickl
192472	blue moccasin azure navy dark	Manufacturer#2           	Brand#22  	PROMO BRUSHED BRASS	21	MED BAG   	1564.47	e silently. deposit
192473	ivory smoke moccasin dim pale	Manufacturer#5           	Brand#55  	PROMO BURNISHED STEEL	22	MED PKG   	1565.47	slyly. furiously pendi
192474	sandy blue slate mint aquamarine	Manufacturer#2           	Brand#23  	PROMO ANODIZED TIN	1	MED CASE  	1566.47	regular a
192477	green dark drab white maroon	Manufacturer#4           	Brand#44  	LARGE BRUSHED NICKEL	33	LG BOX    	1569.47	lar asymptotes d
192478	moccasin forest linen papaya salmon	Manufacturer#2           	Brand#25  	SMALL BRUSHED COPPER	3	LG BOX    	1570.47	 in place of the unu
192482	pink black slate dim antique	Manufacturer#2           	Brand#21  	SMALL BRUSHED STEEL	5	JUMBO BOX 	1574.48	equests. final package
192483	red lavender plum rose puff	Manufacturer#4           	Brand#45  	STANDARD BURNISHED COPPER	50	SM PKG    	1575.48	ic dependencies ca
192484	cream white frosted hot blue	Manufacturer#5           	Brand#53  	ECONOMY ANODIZED NICKEL	17	MED JAR   	1576.48	en deposits. fluf
192488	smoke seashell light pale orchid	Manufacturer#4           	Brand#42  	MEDIUM PLATED NICKEL	50	MED JAR   	1580.48	 furiously. 
192489	drab grey wheat pink white	Manufacturer#1           	Brand#15  	SMALL POLISHED NICKEL	9	SM PKG    	1581.48	ly unusual pa
192490	chiffon linen steel deep grey	Manufacturer#1           	Brand#13  	PROMO ANODIZED NICKEL	23	MED BAG   	1582.49	y. sly
192491	antique almond blue aquamarine brown	Manufacturer#3           	Brand#31  	ECONOMY BURNISHED STEEL	46	WRAP DRUM 	1583.49	requests 
194962	white purple puff chocolate cornsilk	Manufacturer#1           	Brand#15  	SMALL BRUSHED TIN	23	JUMBO JAR 	2056.96	nic accou
194965	dark cornsilk drab smoke khaki	Manufacturer#4           	Brand#42  	LARGE BRUSHED NICKEL	24	MED CASE  	2059.96	ul packages sleep quic
194967	almond sandy pink lime olive	Manufacturer#3           	Brand#33  	SMALL PLATED TIN	8	MED JAR   	2061.96	e quickly final
194968	spring khaki orchid olive dim	Manufacturer#4           	Brand#45  	SMALL PLATED STEEL	30	WRAP DRUM 	2062.96	e. dogged packag
194969	frosted cyan bisque forest sienna	Manufacturer#5           	Brand#54  	LARGE PLATED BRASS	41	WRAP BOX  	2063.96	pending d
194971	tomato floral chiffon deep tan	Manufacturer#4           	Brand#43  	STANDARD ANODIZED TIN	25	SM BAG    	2065.97	 requests use after
194972	navajo plum lemon ivory sienna	Manufacturer#2           	Brand#25  	LARGE ANODIZED STEEL	30	JUMBO CASE	2066.97	cross the f
194975	deep cyan dim cornflower yellow	Manufacturer#4           	Brand#44  	SMALL BURNISHED COPPER	50	MED CAN   	2069.97	s cajole carefully ir
194977	antique sky saddle black light	Manufacturer#1           	Brand#13  	SMALL ANODIZED BRASS	21	MED BAG   	2071.97	 cajole.
194978	pale ivory cornflower white dark	Manufacturer#2           	Brand#25  	MEDIUM ANODIZED STEEL	21	WRAP JAR  	2072.97	ckages boost theodoli
194979	hot smoke ivory honeydew spring	Manufacturer#5           	Brand#51  	ECONOMY PLATED TIN	18	LG BAG    	2073.97	ic requests. 
194980	black beige gainsboro floral indian	Manufacturer#2           	Brand#24  	MEDIUM PLATED TIN	21	WRAP JAR  	2074.98	equests. fi
194986	azure slate burnished snow papaya	Manufacturer#5           	Brand#55  	ECONOMY PLATED STEEL	38	SM BAG    	2080.98	r the carefully s
194987	misty light gainsboro chartreuse forest	Manufacturer#4           	Brand#42  	PROMO ANODIZED TIN	31	SM CAN    	2081.98	ticingly spec
194988	lime steel khaki orange forest	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	14	JUMBO BOX 	2082.98	 bold req
194991	misty rosy purple plum chiffon	Manufacturer#4           	Brand#44  	LARGE BURNISHED BRASS	38	LG BOX    	2085.99	egular dependencie
194992	azure sienna slate cream honeydew	Manufacturer#1           	Brand#11  	MEDIUM POLISHED STEEL	29	JUMBO CASE	2086.99	lly. sp
194993	thistle slate midnight navajo khaki	Manufacturer#2           	Brand#25  	PROMO BRUSHED COPPER	49	MED CAN   	2087.99	s. depende
194996	cornflower sky thistle misty spring	Manufacturer#1           	Brand#13  	LARGE BURNISHED TIN	13	WRAP CASE 	2090.99	quickly 
194997	magenta orange yellow indian seashell	Manufacturer#5           	Brand#55  	MEDIUM PLATED COPPER	12	WRAP JAR  	2091.99	s cajole. regular depo
195001	lawn chartreuse forest chocolate peach	Manufacturer#4           	Brand#42  	STANDARD BURNISHED BRASS	42	WRAP DRUM 	1096.00	long the furiousl
195002	aquamarine tan orange navy burnished	Manufacturer#1           	Brand#14  	ECONOMY POLISHED COPPER	47	WRAP PACK 	1097.00	es; furiously sp
195003	frosted snow beige salmon lemon	Manufacturer#2           	Brand#25  	LARGE ANODIZED COPPER	15	LG JAR    	1098.00	courts slee
195007	peach puff khaki cyan chiffon	Manufacturer#2           	Brand#21  	LARGE BRUSHED BRASS	14	MED JAR   	1102.00	 furiously expre
195008	saddle cornsilk powder antique gainsboro	Manufacturer#3           	Brand#34  	ECONOMY PLATED NICKEL	17	JUMBO PKG 	1103.00	requests 
195009	olive hot lace peru royal	Manufacturer#1           	Brand#13  	ECONOMY PLATED COPPER	19	JUMBO BAG 	1104.00	even deposits. blithel
195010	forest linen lemon burnished sienna	Manufacturer#2           	Brand#23  	MEDIUM ANODIZED STEEL	10	MED DRUM  	1105.01	ents detect a
197481	dim tan lace green blanched	Manufacturer#5           	Brand#52  	LARGE POLISHED BRASS	31	LG DRUM   	1578.48	cording to th
197484	spring lemon orchid blush navajo	Manufacturer#1           	Brand#15  	ECONOMY BRUSHED COPPER	42	JUMBO PKG 	1581.48	gular ideas. boldl
197486	rose firebrick purple seashell midnight	Manufacturer#2           	Brand#21  	MEDIUM POLISHED BRASS	37	SM BOX    	1583.48	he furious
197487	misty pale pink midnight brown	Manufacturer#1           	Brand#12  	LARGE BURNISHED STEEL	2	JUMBO PACK	1584.48	! furiously unusua
197488	pale blue sienna navajo white	Manufacturer#4           	Brand#41  	LARGE POLISHED TIN	21	SM CAN    	1585.48	ts sleep. foxes
197490	aquamarine navy rose chiffon moccasin	Manufacturer#2           	Brand#25  	ECONOMY BURNISHED STEEL	38	WRAP JAR  	1587.49	ts. quickly special
197491	orchid purple goldenrod beige indian	Manufacturer#2           	Brand#25  	STANDARD BURNISHED NICKEL	24	SM CASE   	1588.49	cally fluffily final 
197494	magenta ivory pale turquoise tan	Manufacturer#1           	Brand#14  	MEDIUM BRUSHED BRASS	25	LG BAG    	1591.49	ndencies are
197496	gainsboro light bisque orchid azure	Manufacturer#5           	Brand#54  	ECONOMY BRUSHED COPPER	1	WRAP BAG  	1593.49	ses are after the 
197497	hot steel saddle dark midnight	Manufacturer#2           	Brand#25  	STANDARD PLATED TIN	11	SM PACK   	1594.49	eas. requests
197498	light yellow navajo chartreuse coral	Manufacturer#3           	Brand#34  	STANDARD POLISHED TIN	50	MED PACK  	1595.49	kages. furiously
197499	deep cyan spring royal saddle	Manufacturer#2           	Brand#21  	SMALL PLATED STEEL	18	LG PKG    	1596.49	. slyly e
197505	burnished azure light frosted rose	Manufacturer#1           	Brand#12  	SMALL BRUSHED COPPER	13	JUMBO PACK	1602.50	pades. sl
197506	lavender navy blanched royal coral	Manufacturer#4           	Brand#41  	LARGE POLISHED BRASS	39	SM CAN    	1603.50	ic ac
197507	spring navajo dim burlywood dark	Manufacturer#3           	Brand#32  	LARGE BURNISHED NICKEL	1	MED BOX   	1604.50	ccounts. fluf
197510	snow blush goldenrod spring lace	Manufacturer#2           	Brand#25  	SMALL BURNISHED STEEL	21	JUMBO BAG 	1607.51	key players boost 
197511	burnished aquamarine goldenrod purple cornflower	Manufacturer#1           	Brand#14  	ECONOMY BRUSHED NICKEL	50	SM PACK   	1608.51	elets. bold requests
197512	chocolate seashell white khaki peach	Manufacturer#2           	Brand#23  	LARGE PLATED COPPER	8	JUMBO PACK	1609.51	s bel
197515	tan mint chocolate beige chiffon	Manufacturer#1           	Brand#15  	ECONOMY POLISHED BRASS	40	WRAP PKG  	1612.51	kly express idea
197516	powder firebrick cream lime salmon	Manufacturer#3           	Brand#34  	ECONOMY PLATED TIN	17	JUMBO PACK	1613.51	sts haggle spec
197520	olive blanched seashell lemon chiffon	Manufacturer#5           	Brand#55  	LARGE BRUSHED STEEL	33	LG PKG    	1617.52	 requests. unusual d
197521	smoke powder antique metallic papaya	Manufacturer#5           	Brand#53  	PROMO ANODIZED TIN	49	JUMBO CAN 	1618.52	s. slyly pending idea
197522	tan wheat sienna moccasin almond	Manufacturer#4           	Brand#45  	MEDIUM PLATED NICKEL	33	LG DRUM   	1619.52	 nag about the blithel
197526	slate indian forest brown thistle	Manufacturer#2           	Brand#25  	SMALL ANODIZED COPPER	50	LG CAN    	1623.52	al, regular platelet
197527	moccasin tan frosted almond steel	Manufacturer#3           	Brand#33  	STANDARD ANODIZED BRASS	16	LG PKG    	1624.52	usly about th
197528	indian forest blush blanched chiffon	Manufacturer#4           	Brand#45  	SMALL PLATED COPPER	10	LG CASE   	1625.52	uffy, unusual foxe
197529	cornflower drab forest red chocolate	Manufacturer#4           	Brand#44  	ECONOMY ANODIZED BRASS	29	JUMBO PKG 	1626.52	 the slyly fi
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
10	11	2952	996.12	 bold foxes wake quickly even, final asymptotes. blithely even depe
13	14	612	169.44	s. furiously even asymptotes use slyly blithely express foxes. pending courts integrate blithely among the ironic requests! blithely pending deposits integrate slyly furiously final packa
15	16	7047	835.70	blithely quick requests sleep carefully fluffily regular pinto beans. ironic pinto beans around the slyly regular foxe
16	17	5282	709.16	lithely ironic theodolites should have to are furiously-- 
17	18	8555	995.35	are furiously final accounts. carefully unusual accounts snooze across the requests. carefully special dolphins
18	19	1125	664.17	. ironic, regular accounts across the furiously express 
24	25	5180	905.41	heodolites above the ironic requests poach fluffily carefully unusual pinto beans. even packages acc
25	26	9029	832.74	fully fluffily regular frets. sometimes even requests after the requests wake slyly at the quickly ruthless requests. a
26	27	5020	683.96	es. fluffily express deposits kindle slyly accounts. slyly ironic requests wake blithely bold ideas
29	30	3506	799.27	leep fluffily according to the quietly regular requests: accounts integrate carefully bold foxes. carefully silent
30	31	4767	989.05	ts. slyly final pinto beans cajole ironic accounts. blithely final accounts use among the request
31	32	9685	620.84	he blithely regular ideas. blithely unusual requests haggle fluffily. platelets
34	35	9934	848.75	ven instructions besides the gifts are furiously among the slyly regular packages! instructions use carefully. even requests sleep quickl
35	36	2500	451.58	nic packages boost carefully carefully even theodolites. blithely fina
39	40	3633	463.10	kages are slyly above the slyly pending pinto beans. bold, ironic pinto beans sleep against the blithely regular requests. fluffily even pinto beans use. regular theodolites haggle against the quic
40	41	7690	776.13	lets use fluffily carefully final deposits. blithely ironic instructions sublate against the furiously final ideas; slyly bold courts x-ray silent foxes. regular foxes wake blithely. slyl
41	42	9040	488.55	ss the dinos wake along the blithely regular theodolites. foxes cajole quickly ironic, final foxes. blithely ironic packages haggle against 
45	46	1685	919.63	he doggedly final accounts; carefully regular packages cajole idly regular idea
46	47	4171	244.65	lly quiet instructions. furiously express requests among the final ideas cajole carefully bold waters. furiously regular pac
47	48	6989	292.52	even ideas. blithely final requests boost blithely. final, ironic instruct
48	49	5052	611.16	posits are blithely blithely final foxes. blithely even deposits haggle fluffily express requests. furiously final theodolites use sl
2500	1	6971	97.65	cies haggle doggedly alongside of the quickly regular dependencies. fluffily ironic foxes use against the furiously regular dependencies. packages use slyly. reg
2503	4	568	825.31	fully bold packages. requests use slyly against the final, express accounts. regular instructions alongside of the furiously final packages are unusual, ironic ideas. dolphins use bli
2505	6	8164	389.24	yly regular courts. furiously ironic asymptotes lose furiously permanently daring ideas. slyly pending deposits cajole carefully across the carefully 
2506	7	9435	670.43	uffily even requests. carefully even accounts was. final packages use among
2507	8	4284	451.62	mptotes. closely pending accounts detect furiously requests. furiously perma
2509	10	7928	425.12	lites against the blithely silent courts boost stealthily unusual foxes. furiously unusual 
2510	11	5814	488.68	l escapades across the blithely special foxes may wake carefully according to the regular, ironic ideas. expre
2513	14	4083	508.31	egular accounts cajole. carefully ironic sentiments haggle after the furiously ironic instructions. blithely busy sentiments among the blithely final deposits
2515	16	7847	571.58	deposits are according to the regular deposits. carefully special instructions sleep about the carefully spe
2516	17	5312	348.70	 the slyly final accounts. ironic, final instructions hinder furiously carefully even deposits. ironic dolphins sleep idly slo
2517	18	1296	84.31	equests shall serve blithely. final theodolites wake bravely final instructions. slyly bold ideas cajole slyly even platelets. carefully express orbits are fluffily express, even packages. busy, ev
2518	19	8018	131.37	 unusual pearls nag about the blithely ironic accounts. pending packages boost daringly after the fluffily regular deposits. ironic, regular requ
2524	25	1397	853.75	 blithely final foxes according to the final, regular forges haggle after the furiou
2525	26	4262	614.04	lyly silent deposits. final, bold deposits nag. slyly fluffy theodolites are always according to the foxes. final packages sleep carefully. blit
2526	27	5063	849.34	symptotes sleep pinto beans. permanently regular deposits snooze fluffily. final, silent courts after the special platelets poach unusual packages. excuses run carefully quickly pending accou
2529	30	2449	227.97	en deposits print blithely according to the unusual, ironic packages
2530	31	7266	492.49	around the fluffily ironic accounts. excuses about the sheaves could have to boost slyly along the fluffily even deposits. blithely final deposits play carefully silent ideas. furiously express acc
2531	32	7068	500.88	l accounts above the foxes are slyly even asymptotes. regular, ironic packages after the
2534	35	4098	849.50	. slyly ironic theodolites in place of the pending ideas cajole special requests. ideas poach furiously. blithely express theodolites integrate furiously above the blithely fi
2535	36	7166	872.40	ole among the final accounts. slyly final waters wake furiously furiousl
2539	40	6844	853.13	accounts. slyly regular accounts use blithely above the furiously final packages. packa
2540	41	1724	897.38	dolites. ironic, even accounts alongside of the blithely silent instructions wake carefully special, e
2541	42	5676	295.55	its sleep fluffily. slyly ironic platelets about the enticing packages will wake slyly furiously even notornis? slyly express accounts haggle quickly. asymptotes sleep fluffily ca
2545	46	4039	150.93	y after the packages. slyly express requests haggle slyly enticing theodolites; pending accounts boost. pending, regular depths nag fluffily. furiously fi
2546	47	2147	475.73	inal platelets haggle between the deposits. slyly express instructions wake final pains. slyly regular accounts are quickly after the blithely final deposits. furiously unusual pack
2547	48	287	32.67	es. furiously silent asymptotes use ironic, ironic accounts. slyly ironic packages wake blithely accounts. requests sublate ironic
2548	49	8062	760.73	to beans cajole furiously. slyly final ideas cajole quickly reg
5000	1	510	865.95	ake carefully slyly final deposits. foxes boost furiously after the blithely special pa
5003	4	7336	671.03	usly bold foxes haggle fluffily according to the quickly special packages. blithely even packages sleep according to 
5005	6	7721	208.89	aggle blithely special deposits. final requests wake slyly carefully ironic deposits. ruthlessly express packages nag furiously across the regular, special ideas. theodoli
5006	7	9218	216.35	carefully. slyly regular pinto beans are furiously. final, even theodolites wake slyly accounts. slyly final a
5007	8	9218	211.85	mong the platelets nag final packages. carefully final pains detect quickly according to the silent dugouts. slyly silent instructions cajole. furiously bold plat
5009	10	9192	87.88	ending foxes wake near the quickly express deposits. silent, regular instructions wake slyly furiously fina
5010	11	2913	380.95	ccounts around the final packages sleep slyly carefully even theodolites. silent instructions detect am
5013	14	5686	262.11	 the blithely final accounts haggle furiously even gifts. fluffily ironic asymptotes print furiously. blithely unusual theodolites nag closely slyly ironic theodolites. fur
5015	16	7686	654.03	sts are against the even, express instructions. slowly regular requests above the deposits wake along the slyly pending requests. furiously final deposits use blithely accounts-- slyly ironic re
5016	17	6680	589.41	ess realms play carefully special braids. blithely express instructions use along the slyly final accounts. carefully regular asymptotes impress quickly. quickly express deposits
5017	18	5273	348.06	ges affix. blithely regular excuses will cajole accounts. quickly unusual accounts haggle quickly along the final foxes. special, regular accounts 
5018	19	1488	148.89	nic requests along the carefully bold foxes wake blithely regular excuses. final packages detect fluffily slyly pe
5024	25	8957	929.83	lyly bold packages nod fluffily. blithely express requests are blithely. blithely even patterns sleep carefully. ironic instructions promise ironically blithely 
5025	26	6153	868.77	ost carefully. pains detect blithely silent deposits. furiously pending braids are slyly final dependencies. enticingly even pinto beans should have to affix. q
5026	27	673	447.28	ccounts. packages inside the quiet, final foxes are slyly behind the carefully even pinto beans.
5029	30	4193	50.65	es cajole about the slyly even accounts. instructions after the fluffily ironic theodolites sleep slyly around the even, 
5030	31	8056	910.23	 haggle furiously. carefully brave packages affix furiously except the unusual platelets. foxes sleep
5031	32	854	120.86	scapades. furiously express foxes across the final, final requests affix abou
5034	35	8961	928.17	requests. blithely unusual foxes wake. furiously silent excuses cajol
5035	36	6850	108.39	e blithely. slyly even instructions print blithely. furiously pending requests p
5039	40	6379	112.66	ts. fluffily final packages are. blithely unusual requests sleep quietly about the furiousl
5040	41	2902	666.06	nal theodolites believe quickly blithely special pinto beans; ideas hinder pending instructions. acc
5041	42	9986	722.07	sauternes outside the quickly final warhorses are fluffily blithely regular dependencies. blithely regular foxes believe slyly ironic requests! theodolites integrate. evenly regular ideas lose reg
5045	46	1627	975.16	y dolphins. pending requests after the final excuses use slyly past the blithely bold ideas. quickly even accounts alongside of the even warhorses wake furiously furiously ironic ideas. blithely eve
5046	47	1744	459.31	yly along the ironic dolphins. carefully even packages wake even, regular ideas. deposits among the even ins
5047	48	7590	31.74	und the even instructions use furiously among the carefully final requests. slyly express theodolites boost slyly bold acc
5048	49	3302	540.04	eful requests. slyly final instructions nod. slyly thin packages use. slyly ironic platelets cajole fluffily among th
7500	1	3305	697.68	accounts are fluffily fluffily unusual excuses. ironic escapades wake blithely. silent dugouts boost regular requests. furiously ironic
7503	4	6564	222.81	er the quickly even packages. furiously bold ideas haggle carefully blithely ironic packages. final, unusual accounts are slyly. special, even deposits affix carefully after the furiously brave
7505	6	3282	88.29	the carefully regular packages. quickly final excus
7506	7	9097	509.39	ic, even theodolites. slyly final ideas wake furiously--
7507	8	7904	178.79	refully unusual deposits sleep furiously. ironic ideas haggle carefully. furiously unusual deposits wake permanently express theodolites. blithely final deposits haggle. c
7509	10	7178	481.58	cies. requests x-ray fluffily at the closely bold requests. quickly brave ideas detect blithely. bold foxes wake slyly. carefully even pinto beans alongside of the platelets nag slyly
7510	11	1980	422.23	ly ironic accounts. pending, express frets wake always blithely regular tithes. pinto beans along the furiously unusual packages thrash furiously furiously silent deposits. slyly regular courts wake
7513	14	7116	371.84	ng the slyly final ideas. slyly careful accounts serve carefully about the fluffily sly pinto beans. blithe r
7515	16	2850	795.48	nto the deposits-- quickly unusual instructions haggle carefully
7516	17	4999	392.47	y along the instructions. excuses wake bold pains? blithely special packages nag. evenly regular foxes maintain care
7517	18	2088	209.89	endencies. blithely pending deposits boost furiously fluffily unusual
7518	19	5568	592.31	le requests haggle fluffily pending ideas. ideas must have to sleep slyly ironic deposits. furiously unusual instructions cajole. carefully sly accounts among the bold pack
7524	25	4335	909.07	into beans haggle across the furiously silent ideas. fluffily regular pinto beans sleep furiously along the slyly regular pinto
7525	26	3127	844.57	lithely unusual pearls nod carefully about the furiously 
7526	27	6931	932.56	sits kindle. final courts are ironically. requests breach slyly. slyly regular requests 
7529	30	1760	755.99	t the foxes. dugouts at the theodolites boost thinly final 
7530	31	2156	71.63	ayers cajole furiously inside the regular, silent pinto beans. fluffy excuses haggle carefully bold accounts. bravely regular patterns must have to wake at the final 
7531	32	6300	587.19	 about the fluffily ironic sauternes. daring theodolites lose furiously among the pinto beans. quietly ironic requests lose fluffily. sl
7534	35	8354	434.49	nts. pending packages among the final requests use carefully along the quick, ironic requests. blithely regular accounts print quickly along the final theodolites. slyly bold
7535	36	249	908.24	ely against the packages. unusual asymptotes are silent requests. furiously ironic accounts eat blithely: silent requests are carefully along the slyly regular requests. regular p
7539	40	1452	543.16	s theodolites. final, final platelets haggle carefully. furiously slow ideas haggle. regular, ironic deposits believe. instructions are furious
7540	41	2401	146.93	cording to the packages. slyly final platelets promise carefully final deposits. furiously special platelets haggle slyly final, special dinos. regular, even theodolites among the care
7541	42	5350	621.05	y bold requests nag carefully about the regular, ironic accounts. ironic packages lose slyly. quickly ruthless foxes integrate after the carefully final orbits. quickly unusual package
7545	46	196	894.26	ly near the blithely even theodolites; pinto beans doubt: quietly final ideas against the bold theodolites boost carefully sly accounts. blith
7546	47	6538	750.79	pains. even Tiresias along the requests play quickly final packages. regular accounts
7547	48	5061	115.47	carefully regular requests nag fluffily above the slyly express pinto beans. re
7548	49	5245	832.51	 unusual accounts. express, final pinto beans nag carefully; furiously regular deposits detect furiously along
10000	1	253	563.61	es according to the blithely even deposits solve pending, unusual requests. carefully unusual packages are carefully against the eve
10003	4	7827	787.97	ts boost. dugouts nag final forges. blithely regular packages cajole fluffily furiously final pains. s
10005	6	3463	496.38	unts use blithely blithely even foxes! carefully silent accounts about the blithely silent instructions unwind carefully alongside of the deposits. furiously ironic requests sleep sl
10006	7	7808	748.62	e furiously. final requests above the quickly even deposits boost carefully blithely even accounts-- ruthless pinto beans doubt carefully. blithely ironic 
10007	8	9682	554.10	y silent foxes. pending, even accounts believe even courts; furiously final theodolites according to the accounts use fluffily carefully regular requests. ironic dolphins sleep carefully. even pinto
10009	10	6742	701.78	iously slyly even instructions. slyly brave dolphins are slyly bold accounts! special packages are carefully pending i
10010	11	323	379.36	rmanent accounts. special, silent theodolites serve fluffily. ironic dependencies sleep against the accounts. theodolites wake. slyly express deposits wake furiously. slyly silent packages haggle fu
10013	14	9822	771.91	nts. carefully final deposits are quickly alongside of the quickly regular requests. theodolites among the packages haggle deposits-- quickly bold packages grow. carefully special ideas
10015	16	6411	864.91	luffily from the regular platelets. regular instructions could have to detect carefully even deposits. special, pending deposits cajole furiously regular dependencies. pendi
10016	17	7231	997.65	 the foxes detect stealthy theodolites. slyly unusual theodolites use quickly epitaph
10017	18	9444	864.14	 cajole of the fluffily even requests. furiously ironic deposits use blithely. quickly final packages boost according to the ironic accounts. quietly ironic requests among the unusual, ironic reques
10018	19	4316	784.85	hely. quickly express deposits cajole after the blithely even packages: quickly ironic warthogs grow thinly. blithely even dependencies according to the special, special deposits sleep
10024	25	2673	338.53	g the slyly ironic platelets. furiously even excuses haggle. slyly even dependencies sleep slyly furiously regular accounts. carefully special frays boost inside th
10025	26	9875	729.63	he fluffily regular deposits. carefully ironic reques
10026	27	1453	570.31	lly express requests. slyly regular requests haggle carefully above the carefully special waters. slyly ironic d
10029	30	5777	487.04	ithely across the fluffily even platelets. stealthily pending pinto beans among the 
10030	31	1529	345.43	ccounts. furiously unusual packages wake after the quickly special deposits. slyly final sentiments doubt fluffily quickly final accounts. slyly pending theodolites 
10031	32	8197	286.59	ctions are furiously furiously ironic instructions. carefully special sentiments about the bold forges dazzle blithely above the packages. accounts are furiou
10034	35	1260	438.41	 slyly regular foxes haggle slyly furiously unusual asymptotes. blithely final hockey players n
10035	36	1301	464.65	ideas haggle blithely furiously final ideas. silent theodolites wake fluffily quickly idl
10039	40	9183	671.78	might are quickly carefully pending deposits. even accounts along the final packages haggle blithely above the slowly spe
10040	41	167	633.73	 blithely about the ironic packages-- blithely pending platelets cajole quickly. fluffily unusual theodolites across the quickly iron
10041	42	5211	633.00	around the furiously bold packages! regular, final accounts are carefully pinto beans. slyly regular requests sleep carefully fluffily even acco
10045	46	3131	902.10	 excuses across the express, bold deposits haggle across the special, pending deposits. fluffily ironic pinto beans 
10046	47	2464	641.50	regular, bold theodolites among the pending dinos solve slyly ironic packages. packages haggle after the quickly ironic requests. slyly regular packages over the f
10047	48	8651	552.77	 fluffily silent foxes use blithely carefully final ideas. carefully u
10048	49	1624	18.01	ld pinto beans snooze carefully bold pinto beans! furiously final foxes detect. pending, blithe asymptotes cajole carefully. regular, unusual theodolites cajole fur
12497	1	4839	502.99	 tithes. furiously regular platelets nag carefully express accounts. slyly even dependenci
12500	4	1892	900.83	y ironic instructions need to sleep along the carefully special packages. carefully ironic foxes around the regular, final foxes sleep
12502	6	1147	150.72	ironic ideas sleep quickly alongside of the quickly final epitaphs. ironic, 
12503	7	1774	875.24	requests are finally even requests. furiously bold packages haggle regular, pending foxes. daringly regular depo
12504	8	8653	994.41	fully permanently ironic dugouts! quickly regular requests boost furiously across 
12506	10	261	994.62	 accounts: requests boost carefully. bold ideas play furiously according to the carefully final deposits. final, final foxes by the furiously regular sentiments haggle quickly fur
12507	11	3697	83.81	 final packages. furiously regular packages affix fluffily slyly unusual pinto bea
12510	14	6093	443.69	 bold foxes haggle carefully furiously regular pack
12512	16	8163	871.12	lar somas about the boldly express tithes wake above the slyly even platelets. ruthless, regular deposits detect. ironic accounts nag sly
12513	17	8322	151.15	even foxes cajole slyly. slyly ironic deposits dazzle. furiously silent pinto beans sleep regular, s
12514	18	4218	14.40	ong the carefully close asymptotes. furiously ironic instructions wake
12515	19	4428	222.06	unts. carefully final instructions sleep furiously across the unusual deposits. even ideas run. ironic excuses wake blithel
12521	25	3156	410.81	ages. slyly unusual foxes sublate after the carefully final packages. carefully regular gifts wake furiously final courts. blithely ironic depend
12522	26	1677	654.46	 carefully. packages at the carefully quick pinto beans slee
12523	27	4021	298.04	gle blithely slyly final platelets. carefully express excuses dazzle blithely. furiously even plat
12526	30	2593	806.21	 boost about the final foxes. carefully silent excuses about the special requests wake against the instructions. bold pinto beans sleep furiously. carefully even excuses nag among t
12527	31	4098	873.65	e evenly blithely even deposits. fluffily ironic dependencies unwind. final instructions mold quickly. unusual ideas after the slyly silent packages are slyly slyly bold instruct
12528	32	2764	795.40	even theodolites haggle furiously bold, even accounts. doggedly pending deposits haggle quickly blithely ironic pinto beans. carefully
12531	35	9432	355.93	deas. final packages above the blithely final dependencies sleep across the slyly bold requests. furiously regular deposits wake slyly. carefully ironic courts nag slyly 
12532	36	7225	689.44	es play carefully special foxes. finally unusual platelets dazzle furiously blithely r
12536	40	9348	485.13	deposits-- quickly unusual requests haggle. pending, final requests use according to the final accoun
12537	41	6677	752.42	ly about the furiously regular ideas. blithely final decoys cajole against the regular, ironic theodo
12538	42	6730	48.52	 regular accounts. furiously special deposits boost atop the ironic requests? quickly regular hockey players cajole quickly deposits. unusual deposits nag. fluffily regular requests u
12542	46	891	430.34	 blithely regular asymptotes. carefully express instructions about the ironi
12543	47	1200	859.63	endencies engage blithely atop the fluffily express realms. slowly bold accou
12544	48	611	104.15	riously pending packages wake carefully along the pending theodolites; blithely unusual ideas wake furiously. blithely regular de
12545	49	1430	451.85	 the carefully express deposits. carefully bold deposits maintain quickly above the accounts. i
14998	1	1777	552.89	osits are against the final accounts; blithely regular pac
15001	4	1402	692.24	 enticingly ironic excuses maintain slyly. carefully final escapades wake blithely express excuses. final platelets haggl
15003	6	4601	180.99	idly pinto beans. furiously pending accounts wake furiously. regular, express accounts according to the busily re
15004	7	112	303.98	 pending deposits engage ironic theodolites. furiously even instructions are at the quickly final excuses. unusual, final deposits against the carefully enticing ideas wake blithely against t
15005	8	6119	168.68	deas across the carefully ironic deposits integrate carefully blithely unusual dependencies. furiously regular packages are f
15007	10	5297	534.36	lly final excuses upon the blithely regular theodolites cajole quic
15008	11	5403	430.26	usly regular packages cajole blithely enticing ideas; slyly final platelets lose evenly after the furiously bold ideas. final requests solve furiously. furiously silent accounts ca
15011	14	1377	183.15	osits according to the accounts sleep alongside of the fluffily final foxes: carefully express hockey pl
15013	16	431	493.81	ggle slyly pending packages. slyly bold theodolites wake accounts! fluffily ironic instructions above the carefully special ideas kindle quickly quickly
15014	17	6056	364.59	eodolites. carefully pending requests impress packages. excuses 
15015	18	7480	81.18	 final ideas. furiously express dolphins against the 
15016	19	2173	293.23	ccounts cajole fluffily slyly final instructions. slyly final accounts nag furio
15022	25	8734	573.55	e furiously final theodolites wake among the final, bold deposits. fluffily regular asymptotes wake furiously above the quickly unus
15023	26	3691	113.72	nstructions against the furiously special theodolites use slyly bold account
15024	27	9914	441.98	ts. regular ideas above the furiously bold patterns cajole idly alongside of the packages. deposits doubt quickly. quickly special depen
15027	30	4788	294.20	d packages wake furiously unusual dependencies. slyly special dependencies wake slyly across the requests. blithely even packages detect slyly slyly final dinos. carefully ironic ac
15028	31	7958	906.93	ing packages mold blithely! always final sheaves snooze. furiously even requests integrate carefully pending accounts. packages haggle regular requests?
15029	32	5402	632.59	al deposits. carefully ironic pinto beans maintain blithely ironic packages. furiously even platelets according to t
15032	35	4283	364.76	cuses haggle carefully after the slyly ironic accounts. never regular accounts after the carefully bold requests haggle about the final deposits. idle
15033	36	4127	265.13	rnes. ruthlessly express packages detect slyly foxes. sl
15037	40	7525	295.39	fluffily regular packages. fluffily final instructions around the ironic packages cajole carefu
15038	41	2507	247.08	 regular dinos. quickly sly dependencies haggle blithely against the furiously final requests-- final, final accounts according to the express, regular packages affix among t
15039	42	4333	181.15	ructions. final ideas haggle quickly even asymptotes. final packages wake quickly under the quickly final deposits. car
15043	46	7705	20.88	sts integrate quickly slyly pending realms. careful excuses against the bold theodolites doze ac
15044	47	8898	829.25	reach against the ironic deposits. even excuses wake. permanent ideas after the even pin
15045	48	9381	276.15	 slyly final pinto beans. blithely silent ideas are furiously regular multipliers. deposits sleep. even theodolites haggle. furiously unusual courts cajole about the furiously special instructio
15046	49	4166	622.72	xes cajole furiously. blithely final accounts haggle--
17499	1	9946	982.32	 pending theodolites wake slyly even excuses. fluffily brave realms are a
17502	4	4873	347.06	s. ironic requests according to the blithely ironic deposits cajole bli
17504	6	9537	146.45	ons. final, bold asymptotes haggle above the carefully regular warthogs. pending, dogged deposits atop the bold instructions w
17505	7	7637	738.73	. slyly pending accounts integrate slyly regular requests.
17506	8	5621	3.32	deas play blithely about the special ideas. deposits haggle among the blithely final fo
17508	10	3867	569.36	g pinto beans wake about the blithely unusual deposits:
17509	11	6571	552.85	ly express deposits. ironically pending instructions affix slyly above the final requests. asymptotes ha
17512	14	6068	548.09	ully finally bold requests. bold, final instructions integrate fu
17514	16	3592	849.86	ial packages will have to hang slyly among the furiously bold requests. carefully regular packages boost slyly regular
17515	17	1376	484.41	y unusual accounts. express, final ideas are furiously a
17516	18	7988	391.62	luffily carefully unusual accounts. carefully special dependencies boost. fluffily final pains wake blithely finally ironic foxes. bravely ruthless excuses upon th
17517	19	2490	825.78	nusual courts wake blithely. carefully pending ideas impr
17523	25	9762	731.23	 special dependencies sleep slyly according to the slyly even theodolites. express deposits sleep furiously ironically final braids. blithely even
17524	26	8228	994.45	he pending, even ideas. slyly bold accounts wake among the express, silent courts! 
17525	27	7216	583.96	. bold accounts haggle blithely. furiously unusual packages haggle si
17528	30	6508	168.01	 stealthily unusual dolphins cajole instructions. slyly regular foxes nag against the even instructions. slyly regular foxes along the unusual instructions run s
17529	31	4275	27.74	excuses. final, ironic requests are carefully silent deposits: furiously bold deposits mold carefully pearls? slyly unusual packages would integrate furiously pending deposits. final 
17530	32	620	998.44	old pinto beans cajole quickly final requests. ironic, final foxes haggle slyly furiously express deposits. blithely regular pearls are furiously foxes. reque
17533	35	7243	534.62	counts. blithely careful instructions are carefully regular deposits. regular platelets haggle. accounts sleep blithely according to the blithely special accounts. c
17534	36	7847	502.93	hely regular ideas according to the foxes kindle about the fin
17538	40	7742	126.68	 furiously silent accounts. slyly regular foxes use carefully silent asymp
17539	41	566	108.72	tect furiously ironic requests. ironic requests nag fluffily theodolites? slyly unusual dependencies are about the even, unusual ideas. slyly
17540	42	8424	25.95	en ideas. even packages sleep quickly even instruct
17544	46	351	499.61	iously. ironic, regular dependencies haggle quickly. slyly thin foxes cajole quickly. s
17545	47	576	161.34	sly bold instructions. carefully pending foxes above the blithely final instructions boost above the carefully pending deposits-- even foxes detect quickly furio
17546	48	3576	933.86	s unwind never idly bold requests. regular pains boost slyly. furiously bold requests are. theodolites cajole blithely. blithely final theodolites after the fluffily pending ide
17547	49	8980	986.65	gage furiously even accounts. dependencies use across the blithely pending patterns. requests use furiously across the regular re
20000	1	4397	203.29	ular deposits. express packages cajole. slyly final accounts use slyly. carefully bold ideas nag quickly. bold accounts was fluffily furiously final requests. carefully 
20003	4	4602	113.83	arefully pending deposits above the busy requests affix slyly even requests. slyly final deposits after the pending dependencies haggle carefully furiously final dependencies
20005	6	5156	265.59	ular accounts. furiously final deposits sleep slyly! thinly unusual accounts cajole blithely. ruthless ideas boost furiously grouches. fluffily unusual pin
20006	7	6298	314.52	according to the unusual, even ideas. blithely final deposits wake; accounts nod furiously final ideas. quickly even accounts a
20007	8	5895	475.61	slyly special requests wake according to the ironic excuses. excuses wake carefully regular packages. ironic, regular depo
20009	10	5308	720.73	attainments are carefully courts. furiously regular ideas haggle. regularly even excuses unwind alongside of the carefully iron
20010	11	253	74.31	unts wake. carefully even asymptotes boost slyly blithely spec
20013	14	1964	244.51	. blithely regular foxes cajole always final, even requests. packages poach carefully. slyly ironic excuses wake. special, special platelets haggle slyly according to the fin
20015	16	7178	408.66	ly final dependencies. carefully regular grouches cajole fluffily quietly bold warthogs. slyly 
20016	17	2870	246.57	sits nag evenly closely regular deposits. slyly pending packages are furiously. regular, unusual frays wake among the blithely special deposits-- carefully regular packages are
20017	18	5517	278.03	e bold accounts. requests cajole blithely ironic accounts. fluffily special pinto beans wake. slyly ironic re
20018	19	7643	314.68	aggle. quickly special instructions across the regular deposits cajole blithely carefully regular ideas? permanently regular idea
20024	25	3662	762.73	ages. dolphins along the carefully regular pinto beans sleep against the even, regular dependencies. forges are slyly. unusual accounts boost furi
20025	26	4466	453.59	ng, even asymptotes. carefully express pinto beans cajole. q
20026	27	4524	79.39	egular packages among the ironically bold ideas boost about the bold deposits! unusual req
20029	30	1784	547.07	ly ironic platelets among the special, pending dugouts use over the even requests! requests wake blithely along the
20030	31	141	614.57	 sleep blithely even foxes. blithely pending foxes integrate. furiously unusual packages are carefully quickly pending packages. even courts use furiously a
20031	32	5616	170.44	furiously pending instructions. furiously final instructions wa
20034	35	251	644.32	n requests use-- carefully special foxes nag. furiously special decoys unwind
20035	36	9368	144.99	gular ideas. even instructions cajole carefully according to the quickly silent packages. quickly ironic packages affix according to the ruthlessly ironic pinto beans. carefully regular
20039	40	9099	255.26	lly daring accounts could lose fluffily. furiously regular courts cajole furious, furious accounts. carefully even accounts are quickly. ev
20040	41	9802	956.62	into beans. even deposits alongside of the carefully express foxes wake quickly carefully ir
20041	42	6541	18.90	e blithely quiet packages sleep furiously special accounts. slowly unusual theodolites haggle along the excuses. express, ironic deposits cajole 
20045	46	8381	68.77	sits integrate furiously alongside of the furiously special excuses. furiously s
20046	47	2929	710.97	 pending instructions snooze ironic deposits-- slyly express foxes haggle carefully. permanent packages across the quickly regular sauternes cajole
20047	48	1038	104.39	s are quietly! slyly bold accounts cajole. stealthily final packages are furiously among the furiously ironic
20048	49	1718	238.31	express dependencies. silent ideas wake express i
22494	1	8199	887.64	requests. carefully regular requests along the regular deposits cajole according to the slyly even dependencies-- accounts above the packages boost packages. quietly regular
22497	4	8205	551.63	packages are. blithely regular requests wake furiously slyly even packages. accounts haggle furiously about the ironic, bold multipliers? even accou
22499	6	3188	558.31	onic, final packages are carefully carefully even accounts. thinly even ideas above the fluffily even pinto beans sleep regular theodolites. furiously final p
22500	7	3596	896.06	l packages wake furiously. furiously express dependencies are blithely among the slyly express packages
22501	8	5032	879.83	equests. furiously regular courts cajole inside the bol
22503	10	5130	368.02	even, blithe theodolites. packages sleep never. carefully express frets among the blithely silent asymptotes wake slyly ironic pinto beans. regular, unusual instructions maintain. blithely ironic
22504	11	3476	18.37	kly unusual Tiresias maintain fluffily furiously regul
22507	14	8873	698.19	gular ideas wake quickly along the bold deposits. requests are after the packages. bold requests unwind ac
22509	16	5666	168.21	oost quickly quickly silent deposits. blithely regular foxes cajole slyly slyly
22510	17	4035	709.87	ies serve blithely past the furiously final ideas. slyly express platelets nag at the blithely fluffy pinto beans. slyly ironic requests maintain blithely. regular, unusual dolphins sublate
22511	18	2048	64.47	uests haggle furiously furiously unusual deposits. fluffily express fo
22512	19	6523	600.22	ses sleep carefully ironic, ironic deposits. carefully even instructions sleep quickly regular, final accounts. slyly regular courts sl
22518	25	4571	76.11	gainst the carefully final accounts. special theodolites nag. regular deposits cajole furiously alongside of the even, special deposits. slyly even accounts sleep slyly iron
22519	26	3071	290.54	theodolites are slyly along the bold accounts. quickly express foxes alongside of the carefully special ide
22520	27	3913	490.64	blithely final dependencies haggle express deposits. blithely regu
22523	30	9525	362.13	 carefully about the permanently final ideas. carefully ironic theodolites are carefully-- carefully regular accounts promise. furiously 
22524	31	5191	259.08	ously regular packages wake quickly. carefully ironic frays hinder fluffily according to the furiously regular foxes. regular accounts breach carefully regular
22525	32	7849	976.91	s wake daring, pending excuses. fluffily unusual platelets need to nag according to the carefully regular accounts. carefully ironic dependencies believe blithel
22528	35	2083	140.39	e ironic pinto beans. closely final platelets sublate furiously about the fluffily regular requests. furiously regular pinto beans haggle
22529	36	4997	918.77	ironic, final instructions run regular deposits. furiously regular deposits are slyly 
22533	40	5004	969.34	es. carefully thin instructions cajole after the special packages. slyly bold packages against the express ideas use according to the silent dependencies. final tithes wake qu
22534	41	8298	680.59	osits. slyly pending accounts haggle. final instructions after the special pinto beans haggle blithely along the carefully final deposits. fluffily pending deposits grow carefully around the ca
22535	42	7320	527.39	p blithely ironic requests. special ideas haggle carefully. blithely express theodolites cajole blithely. carefully sp
22539	46	5915	530.61	r the requests. quickly regular packages haggle around the furi
22540	47	5728	248.00	ing dinos! ironic foxes solve besides the quietly ironic packages. slowly ironic pinto beans are never stealthily special packages. quic
22541	48	8469	553.96	posits cajole. slyly busy foxes affix slyly regular accounts. ruthlessly final foxes wake fluffily bold, special requests. blithely pending
22542	49	6047	244.49	nal requests wake quickly. blithely express packages cajole blithely after the enticing, final accounts. slyly unusual packages haggle fluffily theodolites. final ideas print furiously ac
24996	1	9227	152.65	dependencies boost. blithely ironic accounts haggle furiously. furiously unusual accounts haggle carefully. closely express packages are-- fluffily
24999	4	5092	181.43	kages after the bravely even deposits play carefully according to the ironic foxes. furiously ironic deposits a
25001	6	5584	27.45	y final instructions integrate carefully about the carefu
25002	7	5999	989.73	nd the quickly bold theodolites; bold dependencies sleep slyly about the carefully final dependencies. furiously special gifts are slyly ca
25003	8	4199	591.34	kages boost furiously silent excuses. slyly regular instructions solve after the fluffily bold ac
25005	10	7266	738.00	xcuses. requests sleep carefully regular theodolites. ironic foxes nag quickly among the carefully express packages. final foxes use. pinto beans haggle ev
25006	11	704	88.47	ly even dependencies use quickly regular requests. final ideas haggle carefully after the slyly express dependenci
45026	35	6447	797.63	ely even ideas wake: platelets wake furiously among the
25009	14	2266	160.31	near the carefully bold frays are final theodolites. stealthily final packages integrate slyly ironic instructions. carefully speci
25011	16	8263	226.63	. carefully special ideas boost furiously regular requests. pinto beans use carefully. quickly enticing tithes after t
25012	17	5174	534.91	lar excuses after the furiously express ideas boost according to the final waters. slyly ironic theodolites
25013	18	8164	204.62	y after the furiously special instructions. final, pending packages according to the carefully even deposits 
25014	19	6863	218.32	even, unusual requests play boldly fluffily ironic reque
25020	25	1681	981.41	nusual asymptotes sleep even deposits. slyly pending instructions integrate quickly blithely silent grouche
25021	26	6769	758.67	 blithely unusual deposits. fluffily regular deposits haggle carefully even req
25022	27	5016	850.27	 above the quickly bold deposits. furiously ironic accounts boost blithely special ideas. furiously special tithes sleep even, e
25025	30	4165	645.60	deposits haggle quickly stealthy, regular platelets; idly regular theodolites boost after the ironic grouches! carefully special gifts across the carefully bold theodolites cajole f
25026	31	965	305.24	uests across the carefully regular asymptotes believe slyly quickly regular theodoli
25027	32	4205	872.96	nal excuses are after the quickly express excuses; regular instructions alongsid
25030	35	2455	248.16	ong the slyly careful ideas wake blithely about the dinos. furiously final ideas ar
25031	36	7709	460.71	ously. express instructions boost furiously above the fluffily unusual instructions.
25035	40	9005	39.49	usual pinto beans should have to sleep. theodolites poach against the regular, unusual packages. blithely pending foxes are accounts. 
25036	41	9555	209.81	iously ironic ideas haggle across the final instructions. special requests after the slowly unusual deposits boost furiously alongside of the accounts. final accounts serve blithely carefu
25037	42	7292	586.36	e furiously pending pinto beans. pending, even excuses affix across the furiously regular accounts. instructions cajole alongside o
25041	46	6136	577.63	the fluffily unusual accounts. deposits are along the warthogs. express theodolites hang carefully above the slyly express pinto beans. blithely ironic theodolites are about the carefully i
25042	47	3297	887.96	instructions. daring deposits haggle against the final theodolites. theodolites are. instructions sleep carefully. blithely ironic accou
25043	48	8078	49.93	unusual theodolites nag sometimes among the unusual excuses. carefully special instructions wake against the sly requests! pending, final packag
25044	49	337	315.81	unwind furiously above the enticingly unusual accounts! quickly final deposits wake i
27498	1	1953	170.95	dolphins sleep carefully about the special, unusual requests. blithely ironic foxes nag boldly above the pending ideas. slyly unusual dugouts affix among the thinly even deposits: blit
27501	4	7206	651.32	l deposits. carefully express ideas sleep. furiously dogged ideas use carefully even packages. express foxes cajole ironic packages. 
27503	6	4357	623.00	e special pearls boost according to the special, ironic requests. deposits affix slyly unusual instructions. regular, blithe ideas haggle slyly carefully fluffy deposits. slyly ironic deposits sl
27504	7	7102	752.27	 use slyly among the carefully ironic platelets. ironic excuses nag stealthily. furiously unusual ideas sleep. ideas nag quickly. final,
27505	8	7708	503.86	luffily express theodolites after the blithely final packages impress furiously unusual, regular accounts. even requests cajole fluffy requests. pending
27507	10	9679	768.05	blithely even pinto beans. blithely ironic sauternes are carefully. blithely ruthless reques
27508	11	8522	154.23	furiously silent packages. slyly pending dolphins use carefully afte
27511	14	4312	859.20	p against the slyly express packages. quickly pending requests cajole special requests. blithely express de
27513	16	8112	234.26	ind blithely according to the express, even ideas. unusual accounts cajole doggedly along the packages. final re
27514	17	7924	547.09	ccounts use accounts. packages are slyly alongside of the
27515	18	2181	332.35	ld ideas. even pinto beans are. blithely regular packages cajole after the slyly ironic pinto beans. final 
27516	19	9335	778.11	 of the ruthlessly final deposits. even, final foxes cajole slyly across the ironic attainments. carefully regular ideas among the unusual accounts nag after the quick
27522	25	2036	183.83	uriously silent theodolites kindle. express courts sleep carefully! quickly express grouches sleep. slyl
27523	26	370	840.85	ual requests wake blithely bold accounts. furiously regular deposits a
27524	27	5577	9.01	, final dependencies sleep. slyly special deposits cajole slyly final warthogs. slyly silent waters hang blithely blithe packages. slyly even requests lose along the furiously regular
27527	30	5466	292.46	uriously special deposits sleep pending ideas. express instructions boost carefully
27528	31	4149	406.41	the ironic deposits play furiously furiously unusual packages. carefully special foxes x-ray. ironic dolphins across the carefully thin sau
27529	32	7694	912.93	ffily. packages cajole blithely across the bravely ironic platelets. deposits haggle quickly bold packages. requests wake final packages. quickly regular pinto beans haggle even accounts.
27532	35	6982	226.65	efully bold pains. slyly final accounts use slyly 
27533	36	8100	745.48	sts. quickly express foxes nag fluffily. slyly final courts use blithely after the regular, ironic requests. furiously special foxes sleep fluffily thinly ironi
27537	40	2884	179.15	yly against the express packages. carefully pending deposits cajole blithely special requests. slyly final instructions along the unusual asymptotes are quickly final deposits. ironic pl
27538	41	9433	782.20	ntly regular foxes. furiously silent deposits wake slyly even, ironic deposits. final
27539	42	3440	648.08	riously regular ideas are blithely according to the silent pinto beans. asymptotes of the furiously regular packages wake at t
27543	46	1322	576.20	ic waters. carefully regular theodolites are blithely about the express requests. slyly pending excuses cajole furiously above the regular pinto beans. al
27544	47	1928	300.85	iously bold packages. pending pinto beans detect express, even foxes. final platelets use quickly along the fluff
27545	48	5815	454.18	pearls according to the carefully even theodolites impress carefully about the regular, regular depo
27546	49	3127	638.04	regular requests. regular deposits x-ray. quickly reg
30000	1	1063	46.63	arefully alongside of the ironic, silent foxes. regular, regular accounts along the regular, regular instructions sleep evenly across the regular, express ideas! carefully bold hockey 
30003	4	7013	621.77	y final pinto beans boost furiously after the carefully express theodolites. 
30005	6	4501	859.88	lar deposits. carefully regular ideas wake blithel
30006	7	7940	939.46	ckly express deposits serve pinto beans. fluffily final deposits against the slyly careful packages grow unus
30007	8	3209	281.42	tructions wake blithely according to the quickly ironic reque
30009	10	8541	82.77	e slyly platelets. deposits wake. carefully bold asymptotes can sleep above the blithely pending instructions. carefully ironic pi
30010	11	1708	42.80	aggle after the final, pending tithes. quickly ironic requests against the blithely regular escapades
30013	14	5727	615.37	ly bold requests. carefully pending packages haggle furiously slyly bold requests. carefully bold packages nag always. even ideas haggle! regular, ironic deposits u
30015	16	6908	493.15	ly quickly pending theodolites. unusual dugouts print; carefully unusual pa
30016	17	6125	69.14	y above the fluffily dogged instructions. pinto be
30017	18	6554	644.99	usily ironic requests. slyly even pinto beans could cajole. slyly regular deposits agains
30018	19	6196	552.70	blithely. furiously unusual instructions cajole quickly special asymptotes. furiously expr
30024	25	3714	595.53	 ideas. ironic requests nag carefully silent tithes. fluffily unusual instructions detect quickly pending packages. slyly regular pearls cajole special accounts. express,
30025	26	4733	479.23	le blithely alongside of the pending dependencies. slyly regular packages sleep carefull
30026	27	7205	864.66	t theodolites haggle around the ironic, express dependencies: blithely regular deposits are above the furiously ironic asymptotes. 
30029	30	8370	402.34	eposits boost quickly along the slyly ironic accounts. carefully ironic decoys nag regular accounts. quickly final or
30030	31	6119	655.14	gular excuses promise furiously idly unusual accounts. ironic packages integrate slyly; fluffily ironic courts cajole. bold requests boost quickly across the brav
30031	32	744	768.10	e. regular, ironic ideas wake. special, regular packages are furiously according to the furiously final theodolites. slyly even foxes accor
30034	35	266	800.58	s. pending requests detect after the even requests. furiously final hockey players against the blithely silent instructions dazzle slyly al
30035	36	2381	302.85	yly regular deposits cajole fluffily unusual requests: carefully regular packages w
30039	40	509	816.36	ounts haggle slyly furiously regular packages. furiously even accounts after the blithely final packages are blithely pending requests. stealthily ironic requests are fl
30040	41	7759	112.27	attainments nag stealthily quickly even deposits. instructions wake furiously carefully bold accounts. instructions do cajole. even, ironic 
30041	42	2351	730.52	ly. slyly unusual theodolites along the deposits could nod around the quickly even requests. regular asymptotes solve quickly. slyly express packages are. special dolphins after t
30045	46	7229	493.98	 sleep carefully asymptotes. idly even requests wake furiously furiously steal
30046	47	4516	668.89	cajole blithely quickly bold accounts. furiously regular ideas cajole slyly theodolites. quickly quick foxes boost slyly around the bold sheaves. final, regular deposits affix furiou
30047	48	9203	465.46	eodolites cajole. close ideas are along the furiously bold hockey players. pending deposits are b
30048	49	2684	783.31	gle blithely alongside of the bold sheaves. slyly silent realms haggle alongside of the slyly ironic platelets. theodolites 
32491	1	7248	318.51	ally regular deposits along the blithely special theodolites are ironically blithely special 
32494	4	3323	939.19	ly unusual pinto beans serve carefully unusual packages. furiously regular sentiments boost quickly final instructions. s
32496	6	5283	808.78	wake quickly furiously ironic ideas. ironic packages alongside of
32497	7	4350	935.05	y. unusual excuses wake carefully. blithely pending packages use above the unusual, pending foxes. furiously ironic excuses kindle finally. busi
32498	8	6684	330.19	inal dependencies nag quickly special deposits. carefully final sheaves cajole after the blithely bold packages. slyly even instructions hinder enticingly slyly regular requests. slyly final
32500	10	9123	690.91	express theodolites haggle carefully even deposits. quickly unusual accounts alongside of the c
32501	11	1972	220.74	ding accounts snooze. ironic deposits wake blithely. quickly pending packages boost fluffi
32504	14	2370	902.48	he carefully express deposits. idle theodolites alongside of the daring accounts se
32506	16	6495	582.68	. quickly ironic hockey players nag blithely packages. blithely ironic excuses sleep requests; pending, dogged requests lose. ironic warhorses det
32507	17	2535	295.13	ns wake. unusual foxes are about the quickly special requests! packages affix around the furiously silent foxes. carefully ironic dependencies against the furiously unusual packages sleep 
32508	18	875	587.74	he daringly regular hockey players. slyly regular platelets detect fluffily after the ironic accounts. sl
32509	19	5606	632.09	s affix against the slyly unusual requests. slyly express grouches wake. always even deposits cajole against the carefully final requests. daring instructions c
32515	25	5033	899.38	es wake after the even foxes. blithely ironic requests are furiously about the ironic somas. regular, regular accounts haggle quickly slyly even ideas. unu
32516	26	5450	214.00	le never regular, express dolphins-- evenly final ideas hinder carefully. stealthily even packages cajole carefully among
32517	27	5516	785.83	requests detect furiously bold ideas. blithely even packages integrate alongside o
32520	30	6226	104.76	 behind the quickly silent theodolites. theodolites for the blithely even accounts wake slyly bold accounts. quietly silent courts dazzle carefully slyly even theodolites. ir
32521	31	3823	452.32	osits wake quickly pending requests. express accounts haggle. regular, express deposits wake quickly even, dogged deposits. even exc
32522	32	4106	117.09	s dependencies are boldly. carefully ironic accounts x-ray blithely. requests sleep furiously. slyly bold packages boost. quickly unusual accounts above t
32525	35	8162	601.87	re slyly ironic dolphins. fluffily regular deposits use blithely c
140010	11	8779	132.60	ly unusual deposits sleep. carefully special accounts along the fluffily final 
32526	36	5608	221.64	he carefully pending ideas wake slyly fluffily ironic accounts. slyly regular pinto beans nag slyly according to the final instructions. ironic requests sleep slyly quickly silent accounts! furious
32530	40	6776	212.27	ronic packages according to the slyly final foxes will have to boost carefully unusual, final instructions. silent decoys haggle at th
32531	41	5092	457.63	e. furiously even deposits among the deposits play alongside of the final pinto beans. quietly pending instructions haggle after the careful dependencies. final foxes wak
32532	42	1476	615.92	permanent, express deposits sleep at the express ideas. pi
32536	46	5198	61.84	 excuses use quickly about the final pinto beans. quickly express pinto beans sleep along the carefully final accounts. fluffily regular requests cajol
32537	47	6202	531.07	hall wake along the furiously regular requests. slyly regular foxes above the blithely express accounts nag furiously ironic dolphins. blithely ironic ideas are against the blithely even accounts. 
32538	48	7608	90.32	 deposits detect bravely unusual, ironic requests. bold requests engag
32539	49	5370	851.00	ters. blithely final hockey players serve slyly according to the ironic dependencies. quickly special requ
34994	1	3210	995.83	sleep slyly between the blithely regular accounts. carefully even theodolites are. regular, final instructions cajole slyly across the furiously bold deposits.
34997	4	3530	615.75	ornis among the asymptotes use quickly regular requests. quickly regular packages wake. blithely ironic ideas cajole accounts. theodolites haggle thinly above the 
34999	6	2192	649.00	 instructions serve slyly. express packages are blithely
35000	7	7024	875.36	al theodolites lose. slyly special ideas haggle blithely packages: slyly pending requests haggle blithely special instructions. packages cajole furiously past the furiously special courts. furious
35001	8	6147	383.66	o beans above the regular, regular packages dazzle furiously along the carefully regular theodolites. quickly pending packages are furiously furiously regular deposits. bravely ironic pa
35003	10	8329	382.33	kages. blithely express foxes promise even realms. theodolites cajole carefully after the furi
35004	11	9863	99.63	ggle blithely furiously bold packages. slyly regular foxes sleep q
35007	14	6643	152.01	. ironic dependencies detect carefully carefully final foxes. final, even excuses haggle. unusual
35009	16	8669	148.75	 fluffily. quickly even foxes are. regular deposits are carefully final, even courts. carefully final dep
35010	17	624	551.44	ly express theodolites wake always after the final, even theodolites. regular, express foxes among the special plat
35011	18	6865	381.84	sts x-ray carefully ironic deposits. even, express deposits sleep blithely. blithely pending theodolites should poach. slyly express deposits nag slyly. ironic, fin
35012	19	6901	154.03	 regular platelets nag slyly. blithely regular accounts are furiously above the ironic requests. furiously bold platelets integrate. slyly
35018	25	328	543.79	sleep blithely regular accounts; accounts are furiously. ideas after the final accounts wake ruthlessly even platelets. fluffily ironic platelets sleep between the fluffily unusual excuses. furi
35019	26	1366	129.90	, pending courts dazzle carefully quickly regular instructions. even ideas sleep blithely careful requests. bold packages use slyly against the p
35020	27	2844	710.68	t the ironic somas. blithely daring dolphins use-- quickly final requests haggle. blithely unusual deposits are slyly. packages across the slyly ironic requests unwind 
35023	30	6421	80.81	 pending dependencies wake blithely ironic theodolites. ironic, bold ideas wak
35024	31	9338	551.21	ding instructions. regular accounts boost. packages promise carefully slyly ironic requests. ironic requests against the fluffily reg
35025	32	2566	39.82	aggle theodolites. sometimes even theodolites use carefully ironic theodolites. requests integrate slyly. furiously ironic requests at the quickly even accou
35028	35	506	888.94	. carefully final requests cajole slyly about the furiously silent packages. quickly daring packages according to the furiously regular deposits boost carefully special, final idea
35029	36	1263	970.88	 within the instructions wake blithely special escapades. furiously regular requests nag
35033	40	5506	885.99	aggle furiously. fluffily pending instructions are across the slyly special di
35034	41	564	510.08	iously express theodolites: ironic, final requests 
35035	42	5521	289.45	the furiously silent packages. pending pains sleep furiously after the slyly unusual requests. hockey players use flu
35039	46	415	790.05	y unusual instructions wake fluffily. slyly ironic instructions sleep slyly. enticingly regular deposits above the fluffily express instructions try to solve blithely after th
35040	47	2772	242.34	 requests are carefully express, special dependencies; foxes cajole against the requests! pinto beans wa
35041	48	1181	366.71	ven deposits. even, express foxes are. deposits haggle furiously after the unusual pinto beans. slyly bold pinto beans haggle above the slowly unusual pinto beans. re
35042	49	123	136.35	quickly. even deposits alongside of the furiously pending deposits use at the blithely ironic deposits-- carefully re
37497	1	5617	828.23	y ironic deposits are alongside of the express dependencies. permanently unusual requests wake carefully even deposits. fluffily 
37500	4	8835	54.68	nts impress furiously furiously regular packages! quickly regular accounts past the regular theodolites are blithely silent theodolites. furiously unu
37502	6	442	956.96	ar accounts try to nag slyly ironic pinto beans. quickly special instructions haggle. special, final requests are against the furiously special deposits. pend
37503	7	7115	371.16	to the slyly final packages. fluffily final deposits wake blithely ideas. special, final accounts nag carefully. ironic, express asymptotes across the carefully regular 
37504	8	394	158.03	efully silent accounts; furiously special courts kindle. blithely unusual packages haggle blithely. furiously express ideas haggle blithely furiously regular realms. furiously ironic instructions
37506	10	1106	996.08	thogs cajole fluffily bold theodolites. slyly even frays haggle 
37507	11	970	908.98	ts wake furiously after the ironic asymptotes. quickly regular packages should have to nag regularly alongside of the special accounts. accounts are car
37510	14	2096	464.03	al depths. quickly regular theodolites boost furiousl
37512	16	3876	529.94	s. fluffily regular requests use blithely. carefully pending ideas affix blithely among the c
37513	17	8182	422.93	press platelets. regular, final instructions about the fluffily regular excuses are carefully final deposits. slowly regular requests haggle slyly regular, regular pinto bea
37514	18	7593	82.93	efully special requests wake furiously after the special, pending deposits. carefully ironic requests sleep. furiously express foxes haggle carefully.
37515	19	4147	184.39	 unusual platelets are. slyly unusual deposits haggle at the furiously ironic accounts. ironic, regular pla
37521	25	2702	146.36	ven deposits cajole blithely silent dependencies. furiously regular requests engage carefully across the slyly bold packages. packages against the quickly regular instructions sleep furiously 
37522	26	2736	928.27	ly express requests haggle blithely ironic pinto beans. carefully quick warthogs use about the quickly even requests. furiously ironic asymptotes mold carefully alongside of the blithely reg
37523	27	658	508.52	ly pending requests. furiously final packages are blithely final, regular dependencies. busily regular ideas wake across the slyly silent ideas. final deposits eat busily according to t
37526	30	2068	122.46	lites cajole furiously. furiously pending requests nag quickly even pains. slyly express asymptotes use among the regular, final pinto be
37527	31	4614	866.67	to beans. regular deposits kindle furiously along the slyly final re
37528	32	1508	31.13	regular deposits are slyly according to the blithely final packages. furiously express foxes haggle carefully across the accounts. regular accounts eat fluffily a
37531	35	6308	314.72	lets. quickly final platelets hinder carefully regular, bold deposits. dep
37532	36	6446	824.55	al packages. final, regular pinto beans integrate furiously dolphins. fluffily ironic requests sn
37536	40	7701	976.40	 unusual platelets sleep against the silent requests. bl
37537	41	6747	379.58	fter the blithely even packages. slyly pending platelets beside the regular deposits wake carefully silent platelets: bold foxes are c
37538	42	2298	171.41	 carefully special packages after the ironic, even pin
37542	46	9056	326.56	 bold excuses was. carefully quick courts cajole along the careful
37543	47	1899	994.96	sleep. blithely brave ideas are furiously carefully special deposits. furiously final warhorses affix carefully. fluffily final packages are slyly according to the enticingly ironic requests. 
37544	48	6719	787.73	into beans sleep bravely express grouches. blithely final somas about the final id
37545	49	8443	556.75	sly unusual pinto beans boost fluffily ironic foxes. carefully final pack
40000	1	9808	945.41	 silent pains breach. even requests nag? ironic accounts affix slowly. slyly regular pinto beans cajole carefully. regular, even dinos sleep. fluffil
40003	4	5045	70.31	uriously bold accounts. express packages affix silent pinto beans. blithely unusual accounts use slyly along the silent ideas. quiet, 
40005	6	1460	62.69	osits haggle. carefully regular courts haggle across the slyly regular excuses. unusu
40006	7	1748	307.34	 deposits. furiously pending theodolites wake blithely along the ironic platelets. unus
40007	8	8630	294.07	ependencies use deposits. carefully even instruction
40009	10	3949	638.35	sly regular requests wake against the regular deposits. regular, special pinto beans cajole against the furiously unusual patterns. carefully final deposits haggle blithely about the f
40010	11	7369	13.61	y. boldly express deposits detect unusual deposits-- blithely slow deposits sleep. furiously pending accounts ca
40013	14	9007	815.50	above the deposits promise furiously idle accounts. thinly unusual foxes wake carefully after the blithely special theodolites. furiously final dolphins along t
40015	16	4058	97.15	 across the slyly final requests. accounts affix according to the slyly silent accounts. final accounts integrate quickly. furiously final ideas nag enticing
40016	17	8560	593.48	ly regular deposits. final theodolites boost furious
40017	18	2514	837.10	slyly express deposits use carefully of the slyly final requests. evenly special deposits boost blithely pendin
40018	19	4224	94.64	ic ideas wake blithely blithely silent theodolites. ironic deposits along the slyly regular excuses cajole furiously from the pac
40024	25	5898	533.92	usly against the ironic accounts. final platelets are express, express foxes! special accounts was. bold, unusual instructions above the special, even pinto beans boost carefully blithely even pack
40025	26	7042	710.84	uses. busy accounts wake across the blithely ironic instructions. carefully ironic requests ac
40026	27	2931	195.21	structions. even, regular foxes doubt furiously blithely slow deposits. ironic, unusual deposits sleep blithely regular platelets. ironic, even requests after the
40029	30	5742	295.82	 final excuses dazzle carefully. ironic, close requests impress fluffi
40030	31	2910	394.71	en foxes. quickly even packages according to the bold escapades are bold instructions. deposi
40031	32	9025	856.14	. stealthily even sheaves sublate blithely. final ideas sleep slyly even, regular accounts. packages are carefully regular p
40034	35	8240	116.56	 regular pinto beans doze against the accounts. packages wake about the bold, unusual deposits. packages engage about the fina
40035	36	8798	511.38	hely ironic instructions. carefully ironic deposits cajole along the regular, even accounts. furiously final theodolites haggle furiously. furiously permanent requests boost slyly blithely expre
40039	40	2029	599.24	ic ideas would nag furiously. quickly even theodolites
40040	41	7900	271.69	y final deposits among the unusual foxes integrate slyly according to the carefully final asymptotes. quickly express pains sleep pending, even courts. slyly even theodolites after th
40041	42	343	527.33	kages affix. blithely ironic excuses wake. slyly even packages wake; even dolphins above the furiously regular deposits are above the furiously regular excuses.
40045	46	920	293.41	 boost quickly along the fluffily bold requests. ironic courts sleep quickly according to the d
40046	47	2086	450.30	kages. unusual, regular requests solve across the ideas. furiously pending packages haggle carefully carefully unusual packages. slyly silent packages alongside
40047	48	8697	170.63	ular packages. express dolphins wake. unusual packages after the carefully regular requests haggle final epitaphs. unusual, express requests about the furiously brave accounts wake blith
40048	49	3275	177.66	idly even dinos detect; ironic theodolites cajole silently furiously final packages. fluffily ironic dolphins use quickly blithely regular pinto 
42488	1	35	51.48	tructions. requests nag slyly according to the carefully regular dependencies. slyly even deposits sleep. fluffily final packages alongside of the ironic, bold accounts wake furiously fu
42491	4	4479	428.90	hely bold packages. slyly unusual packages affix carefully fluffy deposits? slyly special foxes nag blithely. even deposits across the enticing pains are fluffily across the bravely express cour
42493	6	1341	919.46	gular pinto beans. express, even deposits use after the blithely 
42494	7	1567	428.14	 blithely even packages detect carefully even ideas. blithely regular excuses nag quickly. ideas sleep slyly. fluffily unusual accounts are about the Tiresias. slyly ironic acco
42495	8	990	233.95	cross the accounts. quickly thin pinto beans boost silently. ruthless deposits haggle quickly above the slyly unusual theodolites. carefully thin accounts wake furiously against the 
42497	10	7826	664.41	ndencies. blithely bold accounts was asymptotes. slyly bold packages sleep alongside of the carefully regular deposits. ir
42498	11	5860	120.57	ously quickly bold packages. final ideas across the slyly unusual ideas use quickly ironic, 
42501	14	4887	554.37	the ironic packages wake blithely regular theodolites. slyly even accounts nod along the furiously final deposi
42503	16	2989	648.22	ely pending requests use. even, special instructions haggle furiously furiously regular platelets. accounts snooze quickly. slyly
42504	17	2463	788.63	 packages are regular pinto beans. even, thin requests cajole carefully alongside of the slyly unusual theodolites. even, ironic packages ha
42505	18	1799	333.20	al accounts sleep slyly carefully even packages. express deposits nag quickly! blithely special
42506	19	9173	879.48	ts sleep slyly final deposits: quickly special deposits sleep quietly against the theodolites. slyly regular packages wake. carefully even packages sleep carefully blithely pendin
42512	25	1879	824.42	kages haggle. special deposits at the furiously regular excuses nag fluffily fluffily even platelets. slyly final requests are furiously qui
42513	26	2403	880.49	elets above the requests nag blithely blithely regular accounts
42514	27	4402	352.02	packages cajole blithely. regularly final dugouts eat slyly. ironic instructions poach slyly. stealthy, even requests haggle carefully final foxes. furi
42517	30	4466	346.19	aggle blithely furiously special theodolites. furiously bold instructions wake ideas. carefully final instructions are after the carefully fina
42518	31	7301	666.26	 slyly unusual pinto beans. carefully ironic ideas haggle carefully across the carefully regular instructions. dinos sleep fluff
42519	32	3236	643.92	ithely requests. furiously unusual pearls cajole doggedly. slyly silent requests impress furiously about the furiously special dependencies. ironic packages acr
42522	35	5127	402.16	oost fluffily accounts. furiously ironic asymptotes against the slyly ironic instructions
42523	36	5984	937.81	 permanent ideas. quickly bold asymptotes are after the slyly regular deposits. doggedly 
42527	40	5161	998.68	 the daringly regular excuses. regular requests are carefully. furiously special platelets wake slyly against the fur
42528	41	6949	465.07	l dependencies? pinto beans boost furiously pending packages. slyly bold requests haggle fluffily beneath the quickly bold pinto beans. carefully ironic account
42529	42	7132	912.15	lyly. regular requests above the evenly regular pinto beans are carefully final 
42533	46	8538	592.86	ts are slyly after the slyly busy deposits. furiously brave courts sleep slyly even packages. blithely regular deposits use 
42534	47	9715	614.43	. theodolites sleep slyly final deposits. bold, even ideas are slyly above the express foxes. furiously regular deposits wake slyly express requests. quickly pending deposits are bl
42535	48	8915	942.13	ites detect slyly. blithely enticing packages are from the furiously final accounts; furiously special pinto beans sleep slyly. dolphins 
42536	49	2424	119.14	uctions along the quickly regular dugouts cajole fluffily after the slyly express requests. furiously express ideas breach furiously above the ironic packages.
44992	1	1861	79.74	y regular requests. requests alongside of the carefully pending accoun
44995	4	8303	240.42	pecial, regular somas! final accounts must have to detect. special requests across the slyly express deposits doze against the fluffily bold
44997	6	2947	590.57	lar ideas haggle ironic packages. even, unusual accounts are.
44998	7	6531	346.97	sly since the ironic, ruthless deposits. express accounts sleep
44999	8	6830	88.93	 blithely. regular theodolites among the furiously final deposits nag against the carefully unusual deposits. carefully ironic packages cajole carefully according to the fluffy, even instructions? 
45001	10	4343	260.50	c pinto beans cajole. blithely final pinto beans print after the ironically bold foxes. bold deposits boost slyly. blithely ruthless instructions after the even requests hang blithely special 
45002	11	239	498.44	to beans detect slyly special, regular accounts. express packages haggle regular accounts. even pinto beans haggle carefully around the fluffi
45005	14	6958	760.43	 carefully unusual dinos boost about the regular packages. quietly pending platelets haggle blithely express frays. quickly even ideas wake around the asymp
45007	16	1766	439.08	ously ironic orbits. blithely regular dugouts alongside of the unusual, unusual ideas are carefully final accounts. furio
45008	17	8440	757.62	uctions wake. bravely bold patterns doubt. furiously silent packages haggle carefully across the 
45009	18	9835	978.37	ross the carefully unusual asymptotes. blithely special platelets across the slyly ironic deposits haggle instructions. ironic, ironic deposits haggl
45010	19	5273	303.54	e special warhorses. permanently bold deposits boost slyly even packages. express, special accounts are packages. furiously bold package
45016	25	8838	33.07	y regular foxes sleep against the furiously regular pinto beans. final, iron
45017	26	4602	94.40	ccording to the slyly even requests. regular accounts integrate carefully between the slyly pending p
45018	27	7204	996.99	 have to wake among the slyly ironic pinto beans; special deposits after the furiously regular dolphins sleep pending notornis. 
45021	30	9982	982.72	uriously regular courts. fluffily regular accounts cajole slyly blithely regular dependencies. slyly even theodolites are sly
45022	31	4342	366.22	ckly. carefully special frays cajole slyly pending req
45023	32	5483	855.08	haggle slyly. fluffily final requests use about the unusual ideas. accounts cajole carefully. unusual deposits sleep after the
45027	36	2598	54.11	y pending deposits. sauternes use closely slyly special instructions. regular pinto beans are ruthlessly regular requests. regular instructions 
45031	40	6345	913.15	ongside of the bold accounts. blithely express foxes use slyly 
45032	41	3076	837.05	 ironic theodolites. stealthily final requests boost slyly after the pending, bold courts
45033	42	7404	344.77	lar pinto beans. express escapades along the special accounts detect carefully across the carefully bold accounts. quickly final courts hang blithely instructions. regular ideas ar
45037	46	5234	865.06	its are final deposits. fluffily regular foxes haggle quickly
45038	47	8390	14.78	oxes wake blithely against the furiously even instructions. furiously silent courts are quickly 
45039	48	2448	58.56	s boost along the quickly blithe dolphins. ideas sleep. bold theodolites are slyly deposits. furiously regular instruction
45040	49	5166	558.95	nal, express orbits haggle carefully ironic decoys. requests after the slyly even 
47496	1	9660	804.47	lly express accounts play blithely furiously final packages. bold, silent ac
47499	4	3650	462.22	ngside of the unusual ideas. carefully unusual excuses ought to use at the fluffily quick theodo
47501	6	2083	672.98	pinto beans sleep carefully behind the express deposits. ideas use boldly along the packages. regular deposits haggle across the furiously ironic deposits. ac
47502	7	9297	653.02	 blithely furiously even courts. blithely ironic excus
47503	8	5372	205.45	unts haggle deposits. furiously silent accounts doubt blithely along the ironic, unusual attainments. dep
47505	10	5378	515.49	aggle slyly quickly silent requests. furiously express instructions are furiously about the slyly bold excuses. ironic packages unwind quickly slyly pending dugouts. quickly regular theodolites alo
47506	11	2459	422.70	es wake furiously. deposits wake furiously quickly silent requests. regular ideas wake. fluffi
47509	14	4994	14.99	gular theodolites. carefully regular theodolites eat carefully dinos. ironic platelets past the carefully pending depths sleep against the warhorses. furiously express deposits use bl
47511	16	2387	393.70	ly final accounts. requests after the slyly express dependencies
47512	17	9425	876.25	e. excuses across the express pinto beans boost furiously ac
47513	18	5722	782.65	ccounts along the slyly bold packages wake quickly blithely unusual requests. furi
47514	19	1124	131.21	o poach courts. furiously bold packages are. bold dolphins sleep carefully slyly quiet accounts. blithely bold accounts believe. silent, even requests x
47520	25	6601	12.14	ites about the furiously even theodolites use furiously by the daringly unusual pinto beans. carefully even packages sleep among the furiously bold theodolites. blithely
47521	26	7147	367.74	o the regular packages nag carefully ironic dolphins. pending instructions boost slyly fluffily bold instructions! final foxes around the final packages nag blithely ab
47522	27	7825	99.41	ng patterns about the blithely express accounts haggle carefully alongside of the
47525	30	2126	105.64	sleep according to the slyly regular foxes. dependencies against the blithely bo
47526	31	5700	832.81	uriously above the evenly bold accounts. pending theodolites nag furiously. final packages sleep among the unusu
47527	32	132	19.59	d to sleep furiously regular accounts. express theodolites maintain across the blithely express dolphins. ironically busy accounts sleep furiously. speci
47530	35	4770	470.66	egular accounts sleep carefully slyly ironic ideas. regular ideas against th
47531	36	841	374.07	nstructions. regular, ironic requests after the fluffily express frets sleep fluffily among the bold instructions. slyly even pinto beans affix slyly across the s
47535	40	6904	644.19	r requests use accounts? slyly ironic courts engage slyly alongside of the requests. platelets use blithely regular, silent packages.
47536	41	1845	866.49	ss instructions: quickly unusual somas boost blithely boldly final dependencies. regularly regular deposits sleep carefull
47537	42	8101	92.10	blithely final pinto beans. pending theodolites are bravely after the furiously pending p
47541	46	6216	265.44	ages. quickly even instructions wake excuses. quickly thin accounts are furiously. furiously slow requests 
47542	47	4504	509.82	symptotes across the special packages haggle carefully blithely ironic accounts. furiously regular requests nag. careful excuses are 
47543	48	1359	52.18	fully slyly express theodolites. carefully ironic courts cajole quickly. final packages among the ironic, final accou
47544	49	5731	407.98	 deposits boost blithely upon the slyly even platelets. s
50000	1	6628	389.00	en packages cajole slyly. carefully pending pinto beans wake regular, express deposits. fluffily final accounts haggle. fluffily final dependencies detect carefully unusual requests. blithely
50003	4	5701	745.93	ide of the furiously express sentiments. regular asymptotes wake asymptotes. regular pinto bean
50005	6	9062	276.69	ependencies sleep. even theodolites integrate about the d
50006	7	4989	946.76	. fluffily unusual pearls wake carefully final pinto beans. even pinto beans play. regular, regular pinto beans wake furiousl
50007	8	853	104.11	efully across the slyly regular ideas. blithely ironic
50009	10	4648	10.48	 pinto beans hang fluffily special pinto beans. final platelets affix fluffily final pinto bea
50010	11	9427	295.20	ular, ironic asymptotes. carefully final accounts against the regular ideas haggle slyly blith
50013	14	1189	121.74	ng ideas above the bold, unusual accounts cajole carefully furiously ironic courts. instructions unwind carefully acco
50015	16	398	762.19	azzle quickly ironic, final requests. blithely special requests across the express theodolites integrate carefully furiou
50016	17	3278	473.68	pinto beans. escapades around the fluffy theodolites sleep slyly slyly regular requests. carefully bold pinto beans at the express patterns sleep
50017	18	9515	638.73	slyly bold excuses cajole quickly. dependencies mold final instructions. furiously bold foxes around the express packages maintain alongside of t
50018	19	9879	537.35	 packages detect stealthily quiet pinto beans. furiously regular instructions are busily. bold theodolites about the ideas play quickly even deposits. theodoli
50024	25	21	768.32	unusual packages cajole slyly quickly express ideas. even instructions was 
54997	8	3302	302.16	pliers are blithely ironic warthogs. unusual, final instructions cajole. quickly final accounts grow carefully regu
50025	26	4029	140.70	 the unusual deposits nag carefully ironic pinto beans. slyly bold packages haggle fluffily unusual ideas? ironic platelets eat furiously. carefully unusual excuses along the slyly even ideas ne
50026	27	1844	943.02	otornis sleep quickly according to the quickly ironic accounts. fluffily regular theodolites dazzle slyly quickly eve
50029	30	8405	748.48	re furiously. regular packages should have to haggle ironic foxes. furiously bold
50030	31	4523	654.41	 the requests haggle furiously above the regular dependencies. carefully special ideas haggle blithely alongside of 
50031	32	5044	468.09	 packages. careful waters wake. unusual, ironic instructions are ruthlessly after the slyly regular pl
50034	35	8320	794.45	s. blithely ironic packages about the requests use blithely ironic asymptotes. 
50035	36	4609	931.01	 packages haggle according to the ironic requests. blithely regular ideas cajole carefully even instructions. furiously unusual deposits sleep according to the quickly regular epitap
50039	40	3809	306.58	al foxes. carefully even dependencies about the ironically final instructions hang carefully pending acco
50040	41	8265	437.78	uses. requests maintain above the theodolites. slyly ironic frets
50041	42	6971	287.62	asymptotes. bold pinto beans among the carefully unu
50045	46	6094	315.27	yly thin deposits are quickly. deposits among the slyly bold accounts wake quickly final instructions: unusual dependencies use blithely around the requests. slyly final
50046	47	3145	379.81	ress requests haggle alongside of the final notornis. special theodolites sleep quickly. slyly final theod
50047	48	5119	545.97	. ironic, pending accounts sleep slyly carefully regular requests. even accounts wake. final requests among the ironi
50048	49	4369	763.14	 cajole carefully fluffily regular requests. carefully quiet pint
52485	1	7811	345.35	ers cajole slyly regular hockey players. carefully regular packages are about the quickly unusual accounts? carefully unusual ideas use fluffily about the carefully final courts. fur
52488	4	1846	380.11	 pinto beans. slyly ironic instructions sleep. packages cajole carefully along the quickly ironic accounts: dependencies sleep blith
52490	6	2474	759.52	ironically even packages. even dolphins cajole quickly. blithely pending requests sleep. quickly bold theodolites use 
52491	7	7323	391.78	y regular theodolites. final requests sleep blithely. special, even accounts wake carefully against t
52492	8	6324	966.06	ly unusual packages haggle after the slyly silent deposits. requests wake. accou
52494	10	2865	276.88	ly special requests! frays cajole blithely. express foxes wake blithely regular ideas. slyly final asymptotes integrate slyly 
52495	11	187	913.57	ong the regular, final deposits. unusual asymptotes wake blit
52498	14	9627	402.90	furiously after the special requests. blithely final theodolites 
52500	16	7540	44.39	st the final packages may cajole carefully around the final requests. carefully regular deposits are quickly? regular, regular requests use
52501	17	4229	468.80	equests cajole furiously. furiously idle accounts affix furiously slyly express excuses. final dependencies along the slyly ironic ideas cajole carefully after the final, regular requests.
52502	18	2757	286.51	nic excuses use blithely across the excuses. blithely ironic packages nag furiously requests. carefully furious excuses according to the ironic foxes wake qu
52503	19	1097	957.23	pending pinto beans cajole. pending, ironic theodolites haggle carefully across the ruthlessly even pinto beans-- excuses haggle carefully in place of the slyly pending foxes. fin
52509	25	3082	49.72	s. pinto beans cajole blithely against the blithely special ideas. theodolites wake deposits. regular accounts cajole quickl
52510	26	3119	157.37	 special platelets wake blithely above the blithely bold theodolites. slyly even pinto beans affix slyly sly
52511	27	7182	596.04	packages cajole about the requests. carefully ironic pains use. blithely regular dependencies sublate slyly regular, ironic
52514	30	543	917.27	. regular deposits cajole. slyly special somas boost carefully slyly even pinto bean
52515	31	8522	93.73	thely even deposits are blithely. packages sleep. silent theodolites sleep quickly deposits: special, silent accounts across the final requests wake after the furiously regular instructi
52516	32	7585	545.56	riously platelets. regular excuses use carefully blithely ironic dolphins! idly special requests are blithely about the regular accounts. pendin
52519	35	191	807.46	ndencies: even deposits cajole blithely slyly regular platelets! quickly express ideas of the closely bold theodolites sleep across the even, final dolphins. quickly sly dep
52520	36	2642	406.83	accounts. furiously ironic accounts cajole furiously above the furiously ironic excuses. doggedly silent foxes cajole slyly. carefully regular theodolites boost slyl
52524	40	6921	691.73	refully pending dolphins cajole fluffily alongside of the express, ironic requests. unusual instructions use. fluffily even accounts 
52525	41	5072	793.44	r accounts. ideas above the furiously final accounts boost quickly ironic theodolites. un
52526	42	9780	929.02	cuses. blithely ironic requests haggle carefully along the regular, idle accounts. regular packages are sentiments. asymptotes cajole slyly above t
52530	46	3739	826.85	l, express requests. carefully bold ideas above the fluffily ironic instructions cajole furiously along the blithely express gifts. ironic, unusual requests use 
52531	47	2266	803.80	 carefully bold foxes. carefully express requests wake fu
52532	48	9315	231.82	carefully special asymptotes nag fluffily even dependencies. regular, unusual dependencies x-ray about the furiously pending accounts. hockey players affix.
52533	49	5227	657.53	ve the quickly final packages. slyly special accounts boost furiously blithely unusual requests. regular packages wake carefully regular accounts: gifts nod slyly around the slyly
54990	1	588	644.17	losely regular deposits wake. slyly regular accounts dazzle quickly. ironic, ironic dependencies integrate blithely among the slyly ironic packages. bold, unusual requests among th
54993	4	5696	426.78	ts thrash slyly. furiously slow deposits nag according to the ironic theodolites! slyly final sentiments wake blithely regular waters. even, final 
54995	6	1680	192.91	ts among the furiously pending pinto beans sleep 
54996	7	9497	127.11	y-- patterns may cajole across the theodolites. never final accounts against the ironic, regular waters haggle after the asymptotes. blithely reg
54999	10	4526	59.79	usly unusual packages boost slyly. regular packages haggle furiously even, even packages. ironic, regular attainments eat carefully final theodolites. stealthily regular ideas hag
55000	11	5702	322.21	sts haggle furiously foxes. fluffily regular accounts alongside of the carefully final packages haggle along the final, pending ideas. slyly quiet theodolites a
55003	14	9545	130.67	nts. unusual, quick pinto beans integrate furiously regular requests. doggedly pending packages along the ironic ideas haggle pending requests. quickly re
55005	16	6627	84.98	nts cajole carefully quickly special packages. fluffily regular depo
55006	17	8366	630.92	uriously final packages cajole quickly about the packages.
55007	18	3337	405.12	nt excuses. blithely special foxes maintain furiously after the quickly regular excuses. express accounts haggle blithely furiously silent theodolites. furiously regular accounts
55008	19	8620	102.34	usual frays cajole furiously across the fluffily regular theodolites. ruth
55014	25	4686	362.10	ajole accounts. furiously silent theodolites integrate furiously regular accounts. blithely fluffy foxes wake. accounts cajole slyly alongside of the permanently special 
55015	26	453	932.24	warhorses cajole about the furiously regular instructions. carefully final platelets are blithely. slyly silent deposits about the s
55016	27	6122	430.99	ent platelets. final, regular instructions should 
55019	30	9329	938.05	d pinto beans boost about the carefully even pinto beans. silent deposits wake furiously. slyly ironic requests do nag
55020	31	590	405.74	rs. regular deposits about the express, ironic excuses haggle bold, e
55021	32	8856	630.38	 furiously regular foxes could have to x-ray carefully along the packages. regular, careful requests among the quickly regular packages are after the regular deposits. furiously final packages
55024	35	30	615.29	ic requests use furiously furiously pending accounts. slyly ironic packages sleep furiously. special requests use above the regular dependencies. carefully unusual deposits haggle stealthily along
55025	36	1993	840.59	y furiously regular foxes. carefully pending instructions affix fluffily bravely even deposits. quickly regular asymptotes haggle slyly above the furiously regular r
55029	40	53	531.49	 the special deposits; asymptotes detect among the regular requests. slyly ironic orbits sn
55030	41	5647	174.12	accounts. quickly final theodolites sleep along the fluffily final accounts. regular excuses above the pending, unus
55031	42	5360	372.84	. ironic, final platelets sleep about the slyly ironic accounts. blithe asymptotes across the quickly even grouches use theodolites. furiously bold asymptotes along the fluffily
55035	46	9037	427.24	oss the quickly pending packages nag carefully even, even pinto beans. unusual deposits outside the deposits haggle 
55036	47	6872	767.49	 dependencies boost furiously furious requests. forges among the quickly regular deposit
55037	48	13	880.14	efully final ideas. furiously unusual deposits promise furiously. final deposits wake carefully; slyly bold pinto beans 
55038	49	558	478.33	y. even multipliers wake from the slyly unusual packages. careful packages according to the carefully dar
57495	1	4997	924.52	 blithely silent requests. furiously unusual deposits sleep. platelets sleep. carefully ironic foxes wake fluffily unusual requests. special packages impress fu
57498	4	4355	490.90	ges nag blithely slyly unusual courts. quickly ironic asymptotes use carefully quickly regular asymptotes. slyly special requests wake furiously s
57500	6	2945	53.60	unusual accounts. ironic, final foxes above the carefully b
57501	7	8746	611.48	quests. closely regular platelets wake furiously. fluffily unusual deposits are fluffily. carefully final requests sleep. requests impress among the regular, even sheaves. furiou
57502	8	39	408.52	n packages. quickly furious dependencies wake slyly since the carefu
57504	10	8839	161.03	 carefully unusual foxes try to wake furiously requests. quickly express dependencies cajole furiously ironic accounts! fluffily special packages haggle carefully carefully ironic pinto beans. pac
57505	11	739	74.80	aphs grow quickly pinto beans. slyly express requests after the even, express ideas print blithely express ideas. blithely pending accounts haggle. blithel
57508	14	7699	719.09	es at the fluffily even theodolites are slyly around the stealthy requests. furiously unusual deposits boost blithely even excu
57510	16	2968	947.76	kages. carefully regular packages are slyly blithely bold i
57511	17	6921	360.39	blithely express accounts serve final, pending accounts. deposits will have to wake. carefully ironic theodolites among the quickly 
57512	18	2486	361.01	ously fluffy deposits breach blithely up the slyly special packages. quickly bold pinto beans use. accounts cajole after the blit
57513	19	2233	986.67	as sleep furiously above the furiously final packages. slyly ironic accounts haggle slyly until the furiously regular accounts! blithe, regular accounts lose carefully
57519	25	5926	196.50	ideas. furiously regular ideas boost slyly quickly silent ideas. iro
57520	26	7484	617.41	s sleep blithely alongside of the fluffily idle dependencies. 
57521	27	7124	644.76	 according to the furiously regular somas. idly regular 
57524	30	5385	636.83	onic accounts cajole. pending requests across the slyly special requests haggle fluffily furiously permanent packag
57525	31	2684	875.39	ke doggedly carefully regular ideas. regular theodolites kindle blithely. permanently pend
57526	32	9374	59.92	ily among the regular, express dolphins. furiously ironic pear
57529	35	5189	426.67	pendencies along the furiously ironic deposits wake furiously sauternes. unusual deposits wake. accounts cajole quickly carefully final accounts. bold theodolites cajole above the bold theod
57530	36	4061	63.70	s bold pinto beans haggle quickly final, regular deposits. even accounts impress slyly above the carefully final accounts. slyly even packages 
57534	40	7063	726.93	encies. express foxes sleep fluffily ironic requests. regular pinto beans integr
57535	41	7249	959.03	ously special dependencies haggle slyly against the fluffily regular deposits. evenly ironic requests along the ironic requests eat iro
57536	42	2625	487.38	ut the regular packages. theodolites about the ironic accounts hinder slyly final asymptotes. furious
57540	46	6445	906.32	express accounts. regular pinto beans cajole carefully blithely express requests. unus
67509	16	5674	969.76	ronic, special accounts alongside of the slyly express requests cajole carefully after the special accounts. reg
57541	47	4749	894.38	refully regular dependencies. carefully final requests wake carefully around the regular, special requests. carefully express pinto beans cajole. slyly express 
57542	48	6438	826.67	c foxes! regular, ironic foxes wake quickly according to the slyly express requests. even, even excuses against the bravely regular deposits solve fluffily even foxes. quickly 
57543	49	8422	434.26	final foxes sleep slyly regular epitaphs. quickly even asymptotes sleep bravely about the boldly express de
60000	1	4491	692.11	furiously pending instructions boost carefully among the fluffily blithe asymptotes. slyly bold packages wake slyly. furiously ironic deposits are even, pending platelets. even, even requests na
60003	4	3166	779.45	hely unusual requests about the slyly even accounts wake bold deposits. final courts cajole carefully against the silent, regular foxes. carefully regular deposits wake. unus
60005	6	6613	314.60	tegrate carefully. unusual, fluffy ideas haggle slyly about the bold deposits. even deposits according to the pending requests use f
60006	7	3026	7.95	nto beans after the ironic platelets sleep pinto beans. quickly brave id
60007	8	466	279.60	ilent accounts sleep slyly fluffily ironic requests! slyly regular packages nag carefully. regular pint
60009	10	7369	27.48	nts before the final pinto beans integrate blithely after the quickly regular accounts. foxes alongside of the slyly regular i
60010	11	9039	326.99	onic packages. slyly express deposits nag. regular pinto beans use carefully final packages. final packages cajole special accounts. quickly regular theodolites haggle carefully al
60013	14	9442	689.96	nt dependencies engage against the final theodolites. slyly thin s
60015	16	4367	229.21	e carefully. regular, bold requests haggle slyly. furiously regular accounts haggle against the pending accounts. pinto beans among 
60016	17	686	75.06	s after the carefully final deposits use according to the daringly ironic packages. pending, ironic requests cajole carefully along the c
60017	18	4515	784.48	c foxes solve carefully beyond the blithely ironic realms. quickly final somas haggle carefully final packages. bold reques
60018	19	8729	447.38	against the furiously unusual theodolites affix slyly carefully express deposits. slyly silent dolp
60024	25	5737	23.49	ully pending requests are blithely up the blithely regula
60025	26	5190	564.26	ic instructions nag quietly according to the boldly
60026	27	9502	270.21	even, ironic accounts nag. blithely even packages are. even deposits above the express deposits haggle fluffily according to the furiously special theodolites. furiously final pin
60029	30	3142	142.37	ven dependencies. slyly pending pinto beans haggle. final, final instructions above the ironic, ironic deposits 
60030	31	5251	422.81	sts haggle according to the quickly special reques
60031	32	2339	742.52	uests; slyly ironic accounts between the ironic, regular warthogs cajole carefully quiet
60034	35	1932	649.28	thless deposits alongside of the stealthily express requests boost slyly express foxes. final theodolites about the pending, unusual requests are furiously afte
60035	36	7331	641.45	r carefully furiously ironic pinto beans? blithely bold theodolites haggle fluffily outside the furiously unusual ideas. slyly regular requests nag closely afte
60039	40	5842	503.57	 blithely regular theodolites. bold dolphins sublate slyly. ironic accounts about the platelets eat blithely above the f
60040	41	5344	846.57	side of the pains wake carefully ironic, even dependencies. carefully blithe platelets doze requests. regular requests cajole carefully enticing, express asymptotes. blithely regular asy
60041	42	4382	359.60	 cajole slyly. furiously even platelets are. final accounts are furiously final pearls. final, special accou
60045	46	7586	100.50	 the special, pending asymptotes. final pearls boost. idle, even deposits cajole blithely above the slyly regular theodolites. idly r
60046	47	9394	255.44	lites do sleep slyly. blithely ironic excuses mold carefully. quickly sly excuses boost doggedly final requests. final reque
60047	48	4115	914.60	riously to the pending, unusual instructions. even deposits integrate quickly special requests. silent, re
60048	49	7724	134.61	 brave pinto beans. furiously ironic theodolites boost. quickly ironic platelets against the regular instructions cajole across the furiously final sentiments. instructions 
62482	1	9001	576.52	silent foxes haggle ironic, express requests. sly
62485	4	7857	846.81	are quickly furiously final packages. instructions boost carefully. final accounts detect. carefully regular foxes mold carefully bold accounts. idly
62487	6	3389	426.95	side of the final, even instructions. slow packages haggle furiously about the car
62488	7	9889	128.54	s. slyly ironic requests wake against the slyly final instructions. even platelets unwind furiously. silent depo
62489	8	6014	422.18	arefully along the unusual accounts. carefully thin frets across the carefully final asymptotes snooze slyly blithely unusual requests. carefully final asymptotes haggle ca
62491	10	5666	143.13	 carefully against the ironic dolphins. final pinto beans according to the requests use quickly against the packa
62492	11	2240	207.28	y fluffily. furiously regular packages haggle furiously across the foxes. carefully final foxes haggle blithely. furiously ironic foxes are carefully according to t
62495	14	2827	126.87	 regular requests wake blithely above the express excuses. slyly pending dolphins about the quickly final accounts nag furiously ironic deposits. blithely regular 
62497	16	3190	538.32	ackages haggle. furiously regular frets detect quickly carefully bold theodolites. slyly close theodolites above the special ideas cajole quietly final, even in
62498	17	2578	200.55	accounts. unusual pinto beans are carefully. carefully final foxes are among the requests. blithely even accounts are slyly. express, e
62499	18	1321	676.65	lyly ironic ideas haggle ironic excuses! final excuses above the carefully special theodolites nag about the blithely pend
62500	19	3291	902.27	ts haggle furiously furiously regular instructions. regular exc
62506	25	911	558.24	inder furiously above the carefully regular platelets. blithely even foxes haggle fluffily enticingly silent requests. bold, ironic deposits beneath the 
62507	26	4131	226.27	 deposits haggle along the slyly final theodolites. quickly bold packages sleep blithely slyly ironic excuses! blithely unusual 
62508	27	3528	184.95	ickly unusual platelets use quickly according to the ironically unusual platelets. re
110017	18	7680	342.11	e about the regular asymptotes. slyly regular requests unwind carefully 
62511	30	2194	268.27	nd the busy pinto beans cajole carefully regular courts. silent, pending pinto beans sleep. carefully express pinto beans hinder. even packages are furiously across th
62512	31	877	521.35	 special pinto beans nag. final accounts haggle along the furiously pending accounts; grouches against the blithely pending fr
62513	32	9495	264.86	y unusual requests wake. instructions cajole. final sauternes nag. furiously silent pinto beans affix slyly above the slyly final excuses. regular packages are careful
62516	35	3709	774.57	ound the blithely unusual requests. carefully silent deposits wake slyly even packag
62517	36	4752	313.19	ggle slyly bold ideas. carefully daring packages sleep carefully across the regularly regular ideas. regular escapades along the requests wake slyly carefully final packages. even packag
62521	40	5388	981.20	ins. unusual, express packages shall have to sleep carefully even foxes-- carefully careful accounts sleep slow
62522	41	2510	457.79	tructions cajole until the quick deposits. ironic requests sleep furiously slyly regular packages. fluffily regular accounts cajole:
62523	42	7622	446.85	 haggle slyly. slyly bold instructions impress? carefully fin
62527	46	9181	430.64	 packages. stealthily silent packages sleep blithely ironic instructions; furiously final deposits nag idly. blithely bold dolphins are finally above the ironic, fluffy instructio
62528	47	9162	767.98	inal pains after the carefully final ideas boost carefully after the pending foxes. depo
62529	48	5110	205.60	ily ironic asymptotes breach blithely. theodolites sleep after the ruthlessly regular requ
62530	49	8454	603.37	ans haggle carefully furiously even theodolites. fluffily silent excuses around the quickly final foxes integrate blithely along the express requests. blithely blithe orbits wake doggedly across the
65019	32	9974	27.06	pendencies cajole. blithely daring packages use ca
64988	1	4629	938.78	counts haggle slyly after the bold, special instructions. unusual, furious requests sleep blithely quickly regular the
64991	4	8254	827.15	 pinto beans. even dinos according to the furiously ironic packages cajole slyly regular accounts. accounts nag furiously e
64993	6	1704	863.36	hely regular dependencies sleep against the pending r
64994	7	1641	769.18	 fluffily bold accounts nod even deposits. final pinto beans wake blithely. slyly even deposits haggle slyl
64995	8	3699	943.26	ainst the slyly ironic requests. final packages affix slyly fluffily final packages. quickly
64997	10	2424	727.76	instructions sleep carefully fluffily special waters. slyly express accounts cajole above the deposits! furiously ironic requests against the ironic dependencies cajole carefully ruthles
64998	11	162	538.77	al foxes boost slyly final instructions. carefully close excuses boost in place of the furiously even packages. fluffily unusual requests except the excuses wake
65001	14	9396	33.05	fully express requests. final requests wake carefully above the final, silent pinto beans. furiously even deposits haggle blithely above the sauternes. final, bold Tiresias cajole slyly
65003	16	4717	177.75	quests detect furiously against the furiously ironic packages. pending deposits are. quickly regular dependencies haggle furiously 
65004	17	3064	383.15	rding to the blithely final dolphins. regular requests 
65005	18	8056	342.33	es haggle carefully. furiously even requests cajole slyly according to the thinly final deposit
65006	19	4704	51.48	xes maintain. bold, even notornis haggle among the blithely final foxes. carefully unusual gifts doze carefully blith
65012	25	510	495.38	tterns integrate blithely slyly pending accounts! ironic packages along 
65013	26	6448	460.21	ong the slyly regular dependencies. packages sleep quickly. carefully ironic deposits sleep carefully unusual foxes. expre
65014	27	7768	453.71	. bold platelets will nag slyly bold requests. slyly regular instructions along the furiously pending dependencies
65017	30	1154	26.53	 ideas. slyly busy dependencies are slyly above the final requests. express packages about the even foxes haggle besides the furious
65018	31	9332	476.65	mptotes; slyly regular epitaphs sleep fluffily around the finally even accounts. quickly stea
65022	35	3358	616.05	 bold accounts. finally final accounts haggle boldly pending theodolites! quickly special pinto b
65023	36	7231	823.65	old, even asymptotes breach toward the pinto beans. quickly even r
65027	40	3712	651.79	en, unusual packages serve furiously regular accounts. regular pinto beans wake slyly final ideas. even dolphins cajole carefull
65028	41	9597	917.07	ly even foxes. slyly pending deposits nag around the deposits. instructions haggle blithely above the even, ironic packages. careful ideas wake. special account
65029	42	6315	105.26	ts detect across the furiously unusual dolphins. fluffily unusual pinto beans
65033	46	8409	603.27	. blithely even accounts detect. slyly bold platelets wake. regular packages across the unusual ideas are after the quickly ironic multipliers. ironic courts boost furiously unusual pinto 
65034	47	2686	843.93	side of the regular courts cajole finally carefully express packages; fluffily even packages along the slyly ruthl
65035	48	6655	976.12	ts maintain enticingly final deposits. blithely express accounts boost. slyly final deposits haggle. blithely special in
65036	49	5492	193.11	into beans among the regular deposits affix furiously about the carefully regular warhorses. regular, bold deposits sleep furiously above 
67494	1	5665	469.87	ate. blithely ironic theodolites haggle fluffily pending instructions-- regular accounts nag slyly express, final instructions. quickly final accounts 
67497	4	1662	675.29	luffily special dolphins detect slyly along the furiously 
67499	6	2978	764.17	telets wake silent, even pinto beans. blithely final pinto beans c
67500	7	8208	216.17	 express accounts impress slyly dolphins. furiously final requests hang fluffily above the ironic packages. fluffy packages solve blithely regular sheaves. final requests sleep regularly fin
67501	8	1593	197.17	ages. furiously regular requests was slyly furiously regular pinto beans. furiously even requests are blithely quickly silent accounts. carefully fi
67503	10	8301	488.26	s are about the slyly ironic warhorses. furiously regular accounts affix furiously. 
67504	11	138	599.07	al requests use ironically according to the regular, thin asymptotes. regular instruction
67507	14	4354	361.50	ix about the special, even dependencies. slyly final platelets doze. slyly even requests are alongside of the express platelets. quickly final packages boost ar
67510	17	6441	130.54	ts-- special, bold foxes would cajole carefully. furiously final instructions detect thinly among the final packages. unusua
67511	18	4652	672.87	al theodolites boost across the slyly bold epitaph
67512	19	4783	122.27	special packages nag fluffily according to the final, bold instructions. carefully regular courts nag according to the bold accounts. carefully even accounts alongside of the instructions mold
67518	25	2646	27.16	lyly. regular, unusual requests boost. furiously even pinto beans ought to sleep about the furiously special platelets. unusual, daring packages need to wake alongside of the furiously ironic pa
67519	26	1475	720.49	ly quickly ironic deposits. regular dependencies cajole ironic, final deposits. slyly regular instructions across the quickly regular deposits are quickly according to 
67520	27	8776	978.24	s integrate fluffily even theodolites. carefully final tithes must haggle blithely. furiously special excuses about the theodolites haggle blithely regular, silent deposits. bold pinto
67523	30	8333	810.95	al, pending requests above the special accounts sleep blithely around the ironic waters. furiously final asympt
67524	31	4332	748.00	ending ideas cajole blithely regular theodolites. permanently iron
67525	32	2884	756.79	special deposits wake quickly. carefully even requests sleep carefully according t
67528	35	1015	717.01	 fluffily against the requests. express ideas wake carefully. ruthlessly final accounts about the furiou
67529	36	9356	446.40	ess instructions. furiously bold epitaphs haggle. even accounts wake blithely. special, special
67533	40	4370	302.77	e slyly. instructions was slyly after the furiously unusual pinto beans. ironic, express packages sleep slyly slyly ironic pinto beans. quickly even deposits according to
67534	41	1823	276.25	x blithely furiously final attainments. ideas will have to wake against the carefully silent ideas. foxes above the carefully final packages sleep about the even, re
67535	42	8368	315.51	d instructions. blithely final theodolites are. ironic accounts above the theodolites affix after the carefully ironic packages. slyly enticing accounts wake carefully foxes. carefully pending
67539	46	7473	708.90	lly unusual frets sleep express dependencies. slyly regular packages are at the never sly ideas. slyly bold packages haggle stealthily silent instructions. pinto beans cajole blithely furiously exp
67540	47	1550	377.34	nic requests. blithely regular packages haggle. carefully ironic packages kindle across the bold platelets. accounts solve alongside of the ironic 
67541	48	2174	410.15	efully across the carefully ironic requests. ironic instructions unwind fluffily furiously regular deposits. carefully final platelets cajol
67542	49	5714	770.65	hes. permanently even depths use blithely about the deposits. quickly ironic dependencies x-ray furiously along the pending pinto beans. fluffily regular accounts sleep fluffily ironic instructi
70000	1	9060	282.60	its detect furiously special deposits? even, ironic packages sublate fluffily. silent 
70003	4	7762	67.82	ly against the unusual, final foxes. ironic, permanent theodolites cajole furiously. blithely 
70005	6	3316	38.25	ic warthogs. blithely special requests boost carefully ironic, ironic dependencies. deposits after the regular dependencies boost after the quickly fina
70006	7	5917	785.26	ly even deposits. pending, final accounts nag. blithely ironic packages wake after the quickly ironic packages. express asymptotes wake blithely. furiously silent accounts cajole qu
70007	8	5061	685.61	tructions haggle fluffily pending accounts! final dinos doze furiously above the furiously final foxes. slyly ironic excuses along the furiously even pinto beans haggle fur
70009	10	1404	664.74	ve the final depths. carefully final ideas wake blithely: blithely final r
70010	11	2977	267.35	 quickly regular accounts? furiously final instructions about the stealthily ironic pinto beans b
70013	14	7685	843.33	ing, final pearls sleep against the unusual decoys. express deposits nag. carefully pending requests cajole carefully packages. ironic excuses boos
70015	16	6287	355.50	y unusual platelets cajole furiously after the instructions. packages believe blithely after the furiously regular theodolites. quickly regular deposits cajole accord
70016	17	5905	540.96	sts sleep according to the fluffily bold accounts. unusual deposits among the carefully express requests wake furiously along the blithely final requests. regular, even dependencies boos
70017	18	6326	189.49	e slyly slyly regular accounts. regular deposits affix slyly across the fluffily daring excuses. stealthily even 
70018	19	4303	96.15	 instructions. slyly ironic dependencies among the carefully final platelets wake amo
70024	25	5114	722.65	ess deposits. excuses beside the regular theodolites cajole quietly silent grouches. fluffily ironic dolphins use quickly blithely ironic courts. care
70025	26	8926	656.43	quests are according to the slyly ironic theodolites. furiously unusual pinto beans are fluff
70026	27	4879	175.20	 according to the special ideas. final packages should have to wake blithely express foxes. ironic, ironic accounts sleep quickly according to the fluffily regular requests. quickly ir
70029	30	8330	192.39	 boost slyly. furiously bold deposits are against the finally silent packages. special, blithe platelets haggle permanently regular, final accounts. carefully final frets sleep quickly foxes. clo
70030	31	9005	939.72	egular, bold accounts thrash furiously according to the s
70031	32	8621	445.98	he furiously regular instructions. blithely final packages past the furiously pending p
70034	35	9772	578.77	to the blithely ironic instructions. carefully pending platelets are. furiously u
70035	36	2962	489.89	the carefully silent theodolites. furiously express dependencies mold past the carefully final pat
70039	40	6216	80.48	nts along the final deposits poach slyly against the carefully unusual requests; ironic epitaphs sleep blithely.
70040	41	3795	240.10	mptotes use ironic requests. slyly regular requests snooze carefully car
70041	42	8754	500.27	ymptotes. accounts sleep carefully. blithely express packages wake. slyly close pinto beans among the quickly special pinto beans boost sometimes regular waters. slyly re
70045	46	3607	783.53	totes. express deposits are final, bold packages. furiously special dolphins sleep even packages. carefully final packages might haggle. quickly pending dependencies eat accordi
70046	47	4428	626.10	 against the even, ironic courts. furiously regular packages nag fluffily; blithely iron
70047	48	6711	286.98	lar, silent deposits. final, even theodolites use silently. slyly final courts boost among the
70048	49	4633	567.85	fluffily ironic theodolites haggle after the quickly regular deposits-- even theodolites cajole carefully. carefully special packages against the requests caj
72479	1	9514	616.92	requests cajole blithely above the bold requests. blithely pending platelets dazzle despite the accounts. carefully regular d
72482	4	253	64.79	kages nag blithely slowly regular excuses. special packages haggle furiously. deposits use about the dependencies. permanently regular accounts wak
72484	6	6129	1.03	. silent asymptotes mold enticingly above the unusual, special requests. blithely
72485	7	3352	783.86	y cajole quickly about the regular accounts. silently pending theodolites snooze behind the blithely even ideas. furio
72486	8	3483	310.73	totes. instructions cajole fluffily ironically final packages. quickly unusual excuses was quickly. furiously special requests mold blithely. slyly express foxes against the quic
72488	10	592	553.61	e-- furiously regular foxes sleep furiously furiously regular platelets. attainments after the quickly bold requests cajole furiously blithely ruthless pinto beans. quick inst
72489	11	9181	760.73	ep carefully above the blithely quick packages. ironic packages use. even, regular deposits cajole carefully quickly final theodolites. slyly even accounts
72492	14	3400	476.44	ole. deposits cajole. final, ironic accounts are quickly across the even courts. furiously final theodoli
72494	16	2773	884.89	ages of the blithely silent requests boost along the foxes. slyly final requests wake slyly final packages. sometimes bold requests will have to nag about the ironic, special dinos. slyly ir
72495	17	5083	322.57	requests about the furiously ruthless packages boost sometimes unusual deposits. carefully final accounts hinder carefully ar
72496	18	1226	66.58	kages sleep furiously carefully unusual accounts. express, idle requests wake slyly furiously even deposits. slyly express t
72497	19	354	356.35	y final, regular instructions. pending pinto beans sleep carefully. careful
72503	25	3066	420.14	ending accounts solve carefully near the blithely silent packages. pending, 
72504	26	2583	670.47	usly even requests against the slyly regular deposits boost boldly bold ideas. quickly pending deposits could use quickly according to the ironic, 
72505	27	4886	845.64	s cajole slyly alongside of the accounts. even requests according to the thinly final pinto beans kindle blithely express, silent requests. fl
72508	30	8747	559.22	ly express Tiresias around the final ideas boost above the regular requests. carefully express pinto beans with the express, final excuses nag above the final dependencies! silent requ
72509	31	6388	999.55	 nag quickly final accounts. fluffily final foxes nag express, 
72510	32	8389	852.71	uriously unusual sheaves are from the bold requests. s
72513	35	4426	580.75	yly. blithely bold dependencies integrate carefully around the quickly final requests. furiously pending ideas wake. idly idle requests are quickly. quickly ironic the
72514	36	7215	129.92	ctions detect. blithely final instructions after the final, even platelets breach after the fluffily even deposits. fluffily exp
72518	40	8772	452.00	arefully ironic dependencies. furiously ironic theodolites sleep slyly alongside of the always bold requests. final deposits according to the carefully unusual d
72519	41	8238	979.06	ironic deposits. bold packages sleep blithely atop the furiously silent accounts. furiously slow instructions after the carefully silent accounts affix slyly alon
72520	42	5025	442.95	 accounts. daringly unusual accounts haggle blithely. final, dogged asymptotes detect. fluffily express dolphins according to the regular requests maintain carefully along the bold, final excus
72524	46	700	913.45	 even accounts boost carefully unusual ideas; carefully regular requests nag. quickly ironic dependencies boost after the evenly careful deposits.
72525	47	3444	474.24	riously ironic instructions haggle according to the blithely unusual accounts. furiously ironic requests play. fu
72526	48	7744	580.94	ccounts. furiously even platelets use slyly about t
72527	49	2476	977.01	its; fluffily final deposits are slyly carefully final packages. quickly final dependencies haggle among the quickly even requests. unusual packages nag furiously around the exp
74986	1	3351	963.94	ing to the slyly regular accounts. slyly final ideas are fluffily. ideas above the bold courts cajole slyly furi
74989	4	6070	101.40	ve the slyly final instructions. quickly silent foxes alongside of the slyly even accounts are slyly final, regular plat
74991	6	8418	565.65	yly regular theodolites thrash across the slyly express deposits? deposits cajole blithely. quickly express requests nag blith
74992	7	5674	703.57	luffy accounts about the slyly regular requests are d
74993	8	6483	946.55	lyly express theodolites. carefully ironic foxes haggle alongside of the asymptotes. carefully pending foxes was slyly according to the carefully ironic dolphins! fluffily unusual 
74995	10	4748	286.25	 final excuses sleep slyly above the bold, silent accounts. furiously ironic ideas are according to the slyly regular pinto beans. regular asymptote
74996	11	6246	156.70	 sleep. pending pinto beans are across the blithely express instructions. ironic requests sleep. sl
74999	14	8036	182.84	ackages cajole boldly even deposits. accounts cajole;
75001	16	8345	494.41	iously. quickly express requests integrate blithely. furiously even excuses along the ironic accounts are slyly slyly ironic packages. quickly pending
75002	17	461	811.40	eas. slyly final ideas affix even accounts. slyly e
75003	18	2432	347.70	 regular deposits. final deposits atop the quickly final grouches haggle fluffily furiously regular id
75004	19	3935	848.44	pecial courts boost fluffily express pinto beans. unusual platelets against the express platelets x-ray above the requests. quickly bold packages aroun
75010	25	5204	528.93	counts. ironic, ironic requests cajole blithely express theodolites. bold ideas are carefully along the fluffily final theodolites. asymptotes cajole accounts. express, final grouc
75011	26	4082	165.24	riously bold dolphins. carefully unusual foxes believe furiously along the final decoys. blithely pending theodolites boost again
75012	27	9127	773.14	eans wake furiously ideas. even foxes kindle slyly express excuses
75015	30	1050	827.00	pending accounts above the regular requests impress slyly furiou
75016	31	1685	125.10	os nag special, final packages. quickly final instructions along the ironic accounts breach ideas! blithely brave packages believe furiously. furiously regular instructions wake quickly carefully
75017	32	6101	755.43	unusual packages. special accounts wake blithely about the unusual theodolites. blithely ironic Tiresias haggle slyly
75020	35	4829	15.18	osits wake blithely express decoys. theodolites according to
75021	36	5280	913.10	ever ironic asymptotes. fluffy dependencies cajole across the flu
75025	40	2853	775.39	counts. special asymptotes haggle quickly. slyly quick 
75026	41	5873	730.32	unts use slowly regular ideas. waters haggle furiously according 
75027	42	2893	892.46	ly ironic courts. carefully regular deposits snooze quickly pending deco
75031	46	3007	73.44	al depths. slyly final accounts was regular packages! fu
75032	47	7345	166.35	ular deposits. blithely final requests among the blithely unusual requests integr
75033	48	5961	470.38	uests affix furiously among the slyly regular instructions. ironic, ironic dependencies from the blithely express pinto beans cajole idly about the accounts. quickly pending foxes s
75034	49	8971	357.78	sits over the carefully daring foxes are carefully furiously even theodolites. final
77493	1	5920	220.80	 final packages. unusual, final requests integrate slyly according to the final, final excuses. slyly pending deposits haggle for the bold pinto beans. furiously special i
77496	4	9818	600.06	. carefully regular platelets use ideas. quickly r
77498	6	6505	466.83	mold. slyly regular packages wake quickly carefully regular requests. closely bold packages above the quickly p
77499	7	1513	53.27	pinto beans. furiously final requests sleep blithely. ironic pinto beans wake careful
77500	8	598	899.50	 regular accounts. regular packages wake fluffily according to the slyly ironic
77502	10	6588	798.63	ecial platelets. ironic, ironic instructions mold slyly across the special, 
77503	11	9969	708.56	 wake slyly at the slyly final excuses. slyly express theodolites cajole. stealthy, unusual escapades are about the slyly ironic deposits. regularl
77506	14	6182	869.02	. frets must wake! furiously pending ideas according to the quickly special packages sleep agains
77508	16	9757	154.58	ackages haggle blithely. carefully bold platelets boost slyly regular requests: furiously even requests haggle. ironic requests dazzle blithely. always even d
77509	17	8661	594.76	onic accounts haggle quickly slyly silent requests. ironic, fluffy theodolites are. requests may cajole. furiously special pinto beans kindle blithely after the carefully iron
77510	18	5151	60.49	he blithely unusual accounts. blithely special theodolites haggle carefully special packages. furiously pending instructions after the special packages hang along the furiously b
77511	19	7287	324.41	lay slyly above the furiously regular packages. final packages cajole asymptotes. accounts against t
77517	25	2517	68.89	fily final pinto beans about the blithely quiet accounts nag blithely carefully regular excuses. blithely pending depths nag slyly above the final, ironic dependen
77518	26	1423	677.49	o the regular, bold accounts. ironic sauternes along the dolphins sleep blithely according to the regular instructions
77519	27	7420	638.65	y final theodolites. carefully unusual pinto beans wake along the carefully final dolphins. doggedly final pinto beans along the quiet, final i
77522	30	474	945.50	y final somas. carefully even requests among the slyly bold accounts haggle furiously even Tiresias. express requests haggle fluffily about the quickly fi
77523	31	673	941.51	efully unusual instructions. bold dugouts wake furiously about the slyly ironic requests? idle packages
77524	32	6604	157.65	iously ironic instructions! stealthily regular ideas detect: final packages are fluffily quickly ironic packages. courts sleep sometimes final accounts. ironic ideas sleep slyly aft
77527	35	154	523.78	 carefully alongside of the blithely final dependencies. carefully final
77528	36	6224	539.96	tructions wake quickly carefully even deposits. regular, even pinto beans detect furious
77532	40	8914	623.31	ely blithely final pinto beans. special pearls detect carefully among the carefully unusual realms. ironic foxes ar
77533	41	794	165.47	 carefully ironic theodolites cajole quickly across the carefully quiet accounts. carefully unusual deposits haggle. theodolites wake. blithely even dolphins cajole-
77534	42	5219	324.07	d requests could sleep carefully regular courts. slyly final packages cajol
77538	46	1123	756.40	ly final epitaphs thrash furiously. blithely final requests affix fluffily against the fluffily exp
77539	47	321	834.54	 bravely. special deposits sleep furiously against the special r
77540	48	6409	6.56	efully ironic deposits are furiously against the slyly ironic asymptotes. blithely ironic deposits use slyly requests. slyl
77541	49	8824	280.96	 quiet, ironic requests sublate blithely. slyly regular accounts use carefully. regu
80000	1	9933	248.07	ar packages about the final accounts use blithely at the requests. carefully even ideas haggle. even, regular dependencies nag stealthily regular plat
80003	4	7121	925.81	ets cajole about the unusual requests. ironic accounts are enticingly. slyly special pinto beans eat. special ideas wake slyly above the pl
80005	6	609	573.78	eposits! fluffily regular ideas are alongside of the
80006	7	1543	101.94	dly. blithely ironic requests about the express accounts are ironic, pending asymptotes. carefully regular accounts detect. qui
80007	8	2397	770.70	ccounts nag idly at the carefully ironic accounts. blithely 
80009	10	8572	855.06	e packages. furiously bold requests mold quickly. foxes sleep quickly above the furiously express deposits. furiously even accounts wake care
80010	11	5543	739.95	 packages. quickly regular requests boost slyly quickly special ideas; final, bold frays are slyly quickly careful packages. slyly ironic f
80013	14	5328	739.29	 pinto beans cajole carefully. bravely special requests are above the ironic ideas. blithely even pinto beans wake carefully slyly bold dependencies. s
80015	16	1111	705.47	al deposits-- regular packages hinder about the ironic deposits. carefully express instructions are blithely at the ironic packages. furiously regular foxes are 
80016	17	3929	92.37	ular accounts haggle alongside of the sly escapades. quickly busy sentiments use slyly alongside of
80017	18	8692	694.07	sly. carefully even foxes haggle quickly about the quickly express deposits. blithely ironic theodolites sleep slyly? ironic, furious instructions detect. quickly final i
80018	19	8930	967.96	carefully even accounts. asymptotes sleep slyly above the deposits. slyly express foxes print furiously slyly even instructions. blithely final req
80024	25	5209	985.60	 sleep furiously according to the carefully final packages-- dolphins cajole enticingly fluffily regular theodolites. blithely
80025	26	7680	940.52	after the blithely regular theodolites. silent pinto beans
80026	27	4243	69.61	ng the packages. pending requests affix around the unusual platelets. carefully even requests sleep. regular accounts promise carefully according to the pending deposits. daring
80029	30	4482	64.00	counts about the unusual dolphins solve furiously at the furiously final depende
80030	31	310	469.02	l asymptotes haggle among the even requests. ideas are carefully final accounts. bold theodolites nod furiously about the blithely bold accounts. p
80031	32	4141	49.73	ly. even instructions cajole slyly alongside of the instructions. furiously final
80034	35	1780	746.58	ing to the blithely permanent accounts. closely f
80035	36	9994	315.83	ing grouches sleep furiously above the carefully regular deposits. quick packages are slyly deposits
80039	40	8887	573.06	ss quickly bravely special deposits. blithely even pinto beans nag blithely. carefully even deposits affix. permanently final courts are blithely deposits. quickly regular platelets haggle r
80040	41	5812	592.62	ely regular theodolites eat even, express packages. 
80041	42	5427	475.69	furiously silent theodolites boost. regular accounts cajole slyly slyly final deposits. express asymptotes kindle 
80045	46	7527	192.95	ide of the slyly dogged theodolites. blithely regular requests affix furiously across the ironic, ironic requests. furiously regular instructions su
80046	47	8922	410.71	packages. foxes are. ironic, ironic instructions sleep slyly. deposits about the carefully express requests wake blithely across the quickly ironic notornis. furiousl
80047	48	1381	626.86	 special accounts haggle carefully above the carefully pendi
80048	49	3043	926.73	ole furiously quickly final packages. carefully regular packages detect slyly. always regular attainments sleep slyly. pending pinto bea
82506	31	2489	554.17	deposits. furiously express accounts wake blithely quickly even accoun
82476	1	3906	272.23	gle carefully against the furiously special instructions. slyly ironic packages haggle b
82479	4	5956	476.82	ole quickly. final pinto beans cajole slyly through the closely final notornis. furiously final deposits cajole above the quickly regular accounts? grouches could 
82481	6	2562	287.56	hely regular packages. blithe packages alongside of the final ideas are slyly quickly f
82482	7	7277	973.82	ccounts alongside of the slyly pending instructions boost furiously along the fluffily
82483	8	6631	587.84	even deposits use carefully even packages. regular instructio
82485	10	6438	593.16	 ironically regular warthogs grow beyond the carefully regular requests: ironic, daring pinto beans according to the stealthily daring asymptotes kindle bli
82486	11	5199	498.17	g the fluffily ironic packages-- silently regular packages boost. furiously final courts sleep blithely blithely bold deposits. slyly final foxes cajole slyly even ideas. carefully
82489	14	3767	193.48	 haggle furiously carefully regular ideas. final accounts nag carefully unusual packages. carefully ironic packages kindle even, 
82491	16	1688	244.12	s. express deposits nag finally. fluffily pending patterns 
82492	17	3019	65.59	y regular excuses-- furiously final accounts detect slyly carefully ironic packages. quickly bold platelets are fu
82493	18	6815	894.30	 sleep along the slyly pending foxes. blithely final platelets affix slyly special escapades. carefully special deposits integrate blithely alo
82494	19	3901	822.52	carefully around the pending instructions! pending, bold ideas haggle fluffily slow instructions. fluffily express pinto beans integrate carefully pending deposits. ironic ideas use f
82500	25	3223	446.07	ithely special theodolites cajole furiously furiously regular theodolites. slyly even requests boost above the slyly ironic deposits. slyly regular deposits cajole furiously 
82501	26	8777	125.58	ickly ironic theodolites. regular instructions are quickly around the furiously ironic accounts. carefully express braids are 
82502	27	9397	29.98	furiously ironic packages. furiously even packages sleep. slyly final packages around the carefully final asymptotes sleep carefu
82505	30	2945	617.60	y express accounts use slyly express ideas. bold, ironic ideas wake carefully regular accounts. carefully regular instructions sleep fluf
82507	32	6795	44.08	eodolites. carefully unusual sentiments sleep about the quickly ironic requests. furiously express
82510	35	1793	303.45	gular deposits. ironic foxes around the carefully even dependencies sleep slyly regularly regular deposits. carefully pending asymptotes haggle above the special attainments. 
82511	36	3556	838.91	out the fluffily even ideas. carefully unusual escapade
82515	40	8087	5.73	hang furiously blithely ironic packages. slyly bold pinto beans haggle carefully ironic deposits. furiously pending instructions haggl
82516	41	8016	227.98	asymptotes cajole regular deposits. carefully regular deposits use. ironic dependencies promise carefully across the unusual foxes. express depo
82517	42	8359	325.24	sly. carefully regular requests use carefully fluffy foxes. quickly even requests across the carefully regular deposits use blithely finally even excuses.
82521	46	8013	627.37	ost carefully across the final theodolites. carefully bold accounts cajole slyly unusual accounts. platelets are quickly regular requests. quickly ironic accounts sleep a
82522	47	1140	542.24	ully final accounts. pinto beans sleep express, regular packages? regular packages use furiously. final, regular hock
82523	48	4057	468.16	express pinto beans alongside of the daringly final requests haggle above the express deposits. express depths sleep fluffily! blithely even re
82524	49	7022	957.91	unts are blithely unusual accounts. carefully brave packages lose. furiously regular requests engage ironic, ironic ideas. spe
84984	1	6277	832.44	 blithely regular theodolites-- slyly ironic packages sleep along the slyly regular packages-- bold foxes are furiously regular requ
84987	4	9292	797.68	ng the bold foxes sublate slyly against the slowly idle deposits. final, ironic ideas cajole boldly. carefully ironic foxes wake. ironic dependencies wake never carefully unus
84989	6	1602	1.91	 theodolites integrate around the furiously express warthogs: carefully pending instructions integrate ironically even packages. fluffy deposits 
84990	7	7657	74.29	kages after the carefully express courts wake permanently even accounts. bold, ruthless requests lose carefully. quickly ironic depos
84991	8	9620	982.53	nic theodolites nag furiously among the regular dolphins. silent packages cajole. quickly pending accounts nag. furiously ironic waters haggle quickly. pending, pending deposits haggl
84993	10	8049	895.68	lar, regular requests. slyly express pinto beans nag slyly! slyly even multipliers was. regular requests nag carefully. regular theodolites use slyly around the slyl
84994	11	9365	177.63	arefully unusual accounts haggle furiously above the blithe pinto beans. carefully bold requests after the slyly pending instructions haggle blithely ironic pinto beans. furiously even excuses d
84997	14	941	295.79	its. blithely final theodolites affix after the carefully express deposits. ca
84999	16	9127	707.61	slyly express platelets about the never unusual grouches sleep carefully slyly ironic accounts. furiously regular epitaphs wake furiously quickly ironic requests. blithely ironic deposits lose
85000	17	9059	143.89	es are above the furiously even asymptotes. fluffily unusual excuses beyond the final accounts sleep furiously special de
85001	18	1655	683.42	 even theodolites believe fluffily fluffily ironic pinto beans. slyly final instructions cajol
85002	19	954	966.09	permanent ideas sleep slyly. pinto beans haggle furiously. always pending deposits above the carefu
85008	25	6373	569.47	iously final foxes. busily bold theodolites after the furiously final accounts are quickly even deposits; requests
85009	26	1992	98.46	lyly even accounts. furiously regular accounts could haggle furiously regular, ironic foxes. blithely express instructions after the slyly regular
85010	27	2748	152.38	ing pinto beans serve blithely slyly pending requests. furiously final dolphins cajole carefully slyly unusu
85013	30	5715	127.53	pecial asymptotes wake. furiously final instructions serve carefully foxes. instructions boost furiously. regular grouches haggle final excuses. 
85014	31	2737	122.07	iously silent requests use furiously. quickly ironic requests according to the slyly regular platelets use furiously furious
85015	32	1011	123.91	ly regular excuses about the bold requests are against the express foxes. platelets cajole around the pains. accounts into the slyly even packages 
85018	35	7630	220.83	ding sheaves cajole across the quickly final frays. ironic instructions need to are slyly across the stealthily bold packag
85019	36	6478	852.73	ns use blithely about the blithely regular requests. blithely final dependencies are. express, express ideas about the pending
85023	40	5519	480.37	packages across the slyly regular theodolites are carefully final deposits.
85024	41	1881	33.99	ths. slyly final deposits use carefully about the furiously even pinto beans. enticing, regular foxes would boost to the even fo
85025	42	1124	886.21	furiously after the blithely regular packages. bold, final excuses after the quickly bold requests haggle according to 
85029	46	7641	926.21	 silent pinto beans wake deposits. regular excuses cajole by the quickly daring deposits. fluffily final gifts play around the slyly pen
85030	47	9512	925.64	regular, ironic packages. slyly ironic accounts sleep carefully furiously silent accounts. slyly final requests according to the pending dolphins are slyly upon the 
85031	48	6482	549.86	oss the fluffily express platelets. carefully even requests are
85032	49	89	338.90	ely unusual foxes among the carefully regular requests cajole carefully beyond the special, regular acc
87492	1	1476	38.97	yly ironic packages above the furiously silent acco
87495	4	7635	406.17	ages integrate furiously furiously bold ideas. brave, ironic asymptotes nag blithely quick platelets. furiously regular foxe
87497	6	1094	109.37	aphs. quickly unusual requests poach furiously around the idle, ironic d
87498	7	1544	311.74	ideas. ironic platelets cajole furiously. blithely bold packages use quickly regular packages. regular accounts cajole regular deposits;
87499	8	3311	925.80	old instructions. slyly ironic deposits are bravely alongside of the carefully even package
87501	10	3995	559.24	gular theodolites sleep furiously bold, regular sentiments. blithely express requests are furiously final dolphins. excuses play quickly. furiously ir
87502	11	2554	533.41	en requests. final, ironic requests haggle blithely about the quickly final de
87505	14	7776	642.77	ts across the carefully unusual pinto beans detect quickly requests. express packages detect stealthily. c
87507	16	3393	653.23	he pending requests: regular accounts sleep. carefully unusual requests sleep. sil
87508	17	1246	135.79	ely even accounts affix fluffily along the pinto b
87509	18	1785	829.99	accounts detect furiously. blithely even accounts sleep sometimes even, final platelets. quickly ironic accounts wake after the bold, regul
87510	19	4122	173.87	 the idly regular dinos. accounts solve carefully. accounts sleep quickly slow, regular Tiresias.
87516	25	6818	43.42	arefully. slyly ironic instructions haggle slyly final requests. furiously even accounts wake deposits. depths sleep. special, close waters wake slyly. quickly special dinos o
87517	26	9627	491.74	cajole. doggedly regular deposits cajole around the furiously pending packages. slyly regular asymptotes wake. carefully final packages are carefull
87518	27	3818	387.62	ve the hockey players boost fluffily bold instructions. fluffily pending warthogs haggle. accounts use quickly carefully final packages. blithely unusual requests among the regular, bold ideas detec
87521	30	8443	71.65	sts boost furiously. never ironic requests haggle. pending foxes use unusual asymptotes. slyly ironic theodolites could are at the carefully regular requests. even accounts use
87522	31	3383	770.78	ccounts wake fluffily. furiously even accounts unwind fluffily even instructions. slyly even requests wake furious
87523	32	2152	620.61	st to the furiously final packages. carefully unusual platelets sleep silent requests. quickly special packa
87526	35	3634	251.52	 deposits cajole evenly slyly regular accounts. slyly regular
87527	36	8511	614.61	furiously final realms sleep requests. furiously regular foxes against the instructions play fluffily bold deposits! furiously final frays doze slyly. always final packag
87531	40	7013	753.01	ounts across the carefully unusual deposits poach furiously after the blithely pending theodolites. carefully thin dependencies use carefully above the realms. blithely e
87532	41	1083	338.44	tes. quickly final foxes wake above the platelets. furiously ruthless packages are. carefully unusual accounts along
87533	42	985	111.13	rding to the final deposits. carefully final epitaphs sleep regular, dogged requests. boldly final hockey p
87537	46	3834	870.86	nstructions against the furiously regular dependencies wake blithely special packages. quickly pending ideas nag furiously above
87538	47	5805	956.80	c sheaves. carefully pending requests above the carefully special deposits haggle blithely foxes. regular, regular pinto beans are blithely furiously ironic ideas. deposits 
87539	48	9260	527.52	ironic packages. furiously regular asymptotes up the final accounts doze quickly instructions? slyly regular sentiment
87540	49	245	565.51	riously ironic hockey players haggle furiously special pinto beans. requests haggle furiously carefully regul
90000	1	8758	548.40	y express packages integrate against the furiously ironic theodolites. final ideas sleep slyly along the bold, regular requests. pending packages doze. quickly re
90003	4	7584	67.33	n excuses impress blithely above the carefully pending pinto beans. requests snooze slyly after the furious
90005	6	6207	824.56	otes use furiously. slyly pending warhorses alongside of the quickly regular requests cajole across the blithely regular courts. slyly ev
90006	7	5686	31.98	ts along the final deposits breach pending accounts. quickly final dolphins engage. blithely brave pinto beans integrate blithely. slyly ironic theodolites nag carefully f
90007	8	5621	984.01	ly ironic theodolites. carefully final platelets detect furiously according to the special asymptotes. carefully sly dependencies haggle b
90009	10	3472	772.81	 regular theodolites. final, express requests are carefully special, pending forges! blithely unusual excuses serve f
90010	11	1455	245.29	e furiously above the ironic, regular deposits. regular, ironic realms above the blithely bold pinto beans sleep carefully fu
90013	14	5437	950.01	inal platelets cajole carefully final theodolites. carefully ironic foxes sleep. carefully final foxes use slyly across the stealthily regular pinto beans. slyl
90015	16	4025	218.37	counts wake slyly foxes. furiously regular pinto beans grow always except the regular frets. bold, 
90016	17	3605	657.47	 nag fluffily even foxes. quickly final excuses run. foxes cajole! quickly regular deposits cajole blithel
90017	18	7352	461.22	are after the furiously unusual packages. quickly brave accounts cajole. quickly dogged theod
90018	19	296	695.56	 carefully ironic instructions wake. depths detect among the b
90024	25	6391	355.88	lites. stealthily even packages are final packages. even deposits must have to use according to the bold, ironic foxes. final epitaphs wake. unusual grouches nag. sometimes silent excuses hang ab
90025	26	3197	463.16	fluffily even accounts. carefully special pinto beans are carefully acco
90026	27	614	358.72	foxes. accounts integrate final, special requests. slyly express accounts against the slyly regular accounts wake furiously along the daringly express asymptotes. carefully pending deposits abov
90029	30	3466	584.71	 carefully final deposits haggle! fluffily final packages alongside of the ironic deposits use quickly regular packages. specia
90030	31	5265	498.64	ly unusual deposits. even dependencies boost slyly carefully regular accounts-- final, silent ideas detect f
90031	32	7422	41.11	 asymptotes among the blithely silent deposits wake slyly ironic dependencies. furiously ironic pinto beans engage blithely above the carefully final 
90034	35	5091	977.53	sual accounts. packages about the quickly final deposits wake blithely above
90035	36	4142	517.24	at quickly according to the regular dependencies. slyly regular pinto beans sleep. special, final packages integrate silently. carefully even 
90039	40	7494	407.13	 silent theodolites wake doggedly regular accounts-- warthogs above the pending theodolites haggle s
90040	41	9893	173.45	lyly express accounts detect above the express gifts. regular, regular requests haggle furiously above 
90041	42	7700	30.72	 boost quickly furiously bold deposits. bold dolphins are bl
90045	46	1252	762.02	arefully quickly special pearls. ironic deposits nod ironically. slyly ironic pinto beans sleep fluffily. slyly f
90046	47	267	254.83	nic packages print slyly sly platelets. slyly ironic requests haggle packages. regular requests are according to the busy deposits. b
90047	48	7618	16.54	old deposits haggle. regular requests are quickly across the regular pinto beans. fluffily daring theodolites haggle during the even grouches. carefully i
90048	49	3745	12.16	 carefully regular, pending frets. blithely ironic theodolite
92473	1	7292	284.82	bove the closely silent accounts x-ray alongside of the final ideas. furiously pending instructions serve. regular requests use. even ideas affix
92476	4	7349	283.42	s nag blithely along the special, final platelets. accounts cajole quickly about the stealthily ironic instructions. depend
92478	6	9012	521.77	ove the regular requests. blithely express dependencies cajole final theodolites. furiously ir
92479	7	1400	50.10	ickly; regular packages cajole ironic dolphins. quickly unus
92480	8	8389	912.94	ts breach furiously around the slyly regular platelets. deposits under the ironic pinto beans are after the fluffily regular pa
92482	10	7407	567.87	inst the carefully regular theodolites. regular deposits promise furiously ironic requests. packages boost carefully express accounts. furiousl
92483	11	3404	722.97	usly regular deposits. blithely regular ideas haggle. furiously final packages are bravely slyly express accounts. special deposits hang boldly. thinly bold accounts are furiously. slyly fin
92486	14	4859	973.79	requests wake careful pinto beans. theodolites cajole about the quiet, ironic accounts. slyly even accounts are blithely among the quickl
92488	16	5448	36.58	lyly! accounts are carefully unusual deposits. brave packages are along the slyly regular pinto beans. silently even foxes sleep slyly. ironic accounts sleep fluffily. caref
92489	17	4916	169.15	s are around the carefully ironic ideas. carefully even pinto be
92490	18	6607	822.31	ithely. furiously unusual asymptotes wake. regular requests haggle according to the furious
92491	19	1062	780.74	l requests cajole across the carefully silent packages. packages cajole slyly deposits. blithely permanent pearls h
92497	25	5865	396.56	 the bold accounts. slyly sly instructions wake blithely about the express, bold pinto beans. blithely regular deposits nag carefully unusual instructions. fluffily regul
92498	26	9328	39.89	among the quickly ironic accounts. blithely pending ideas integrate. doggedly regular warthogs cajole. pending, bold accounts at the furiously pending platelets do affix according to the frets. fu
110018	19	5123	169.19	ckages. slyly regular requests according to the furiously
92499	27	6481	682.28	sts. theodolites integrate carefully. slyly regular packages according to the ironic accounts are blithely among the carefully final accounts. final, final instructions nag slyly. regular theo
92502	30	9251	992.32	ckages. fluffily even requests haggle slyly. even pinto beans use furiously special gifts. special courts are blithely about the escapades. furiously regular deposits 
92503	31	6982	401.65	equests around the blithely regular packages wake furiously
92504	32	2065	908.98	h blithely furiously ironic packages. ideas use fluffily unusual deposits. ironic theodolites use slyly final accounts. carefully regular ideas above the regular, r
92507	35	338	335.05	yly express dolphins solve furiously. slyly busy requests across
92508	36	3768	687.08	 asymptotes haggle alongside of the ironic, even instructions. fluffily pending requests affix blithely. express deposits are. blithely furious realms solve
92512	40	1913	143.29	ly even packages. furiously regular packages wake slyly bold deposits. ironic instructions sleep after the ironic, unusual 
92513	41	7022	587.80	he slyly regular deposits haggle at the furiously regular instructions. furiously final theodolites run carefully after the instructions. blithely quick ideas nag against t
92514	42	3707	424.96	efully even asymptotes cajole blithely along the furiously regular depo
92518	46	4184	394.98	to beans cajole blithely fluffily final theodolites. even deposits are blithely. boldly regular packages
92519	47	4791	607.13	nts cajole against the regular, final dependencies-- ironic packages before the regular dependencies use always final requests. carefully ironic dugouts integrate carefully. final as
92520	48	6355	374.95	lithely express requests use carefully. final instructions against the carefully bold packages cajole carefully after the carefully final excuses. bold pinto 
92521	49	4047	589.85	ter the never unusual dependencies. pending requests across the carefully ironic theodolites haggle carefully furiously close accounts. blithely even realms haggle carefully bo
94982	1	5208	113.62	egrate. bold, final ideas around the pending deposits integrate blithely carefully perma
94985	4	4251	26.17	 final instructions boost carefully according to the regular multipliers? carefully fluffy deposits use permanent requests. express, silent pains haggle. even courts are carefully a
94987	6	5283	627.12	ites. furiously express deposits wake quickly furiously regular foxes.
94988	7	946	670.31	eposits around the foxes haggle ironic packages. packages engage near the requests. stealthily 
94989	8	1145	619.88	furiously special, regular accounts. regular, regular ideas haggle furiously regular sheaves. final instructions sleep. pinto beans hagg
94991	10	4417	551.71	round the accounts. quickly daring foxes about the quickly final requests boost
94992	11	5673	5.45	he slyly permanent orbits: deposits x-ray above the carefully thin deposits. special, final attainments agains
94995	14	2671	798.98	nic foxes. furiously ironic pinto beans boost about the regular th
94997	16	9389	228.37	quick accounts. fluffily ironic deposits ought to nag deposits. ironic foxes haggle slyly according to the express, unusual excuses. c
94998	17	6452	148.59	fluffily unusual packages breach among the ruthlessly express instructions. theodolites boost along the slyly ironic pinto beans. ironic instructions n
94999	18	2882	442.13	slyly ironic pinto beans wake? carefully ironic excuses wake slyly carefully pending deposits. fluffy, final accounts use. quickly final pl
95000	19	468	723.68	nal requests. furiously blithe foxes cajole carefully after the un
95006	25	6428	803.84	ag after the slyly regular deposits. carefully special dolphins sleep blithely silent hockey players-- furiously regular realms integrate across th
95007	26	6959	644.61	long the enticing, regular packages. final foxes sleep slyly across the slyly quiet platelets. unusual asymptotes wake across the carefully special pl
95008	27	3686	116.50	ts. slyly regular requests according to the furiously ironic dependencies wake around the
95011	30	1931	600.98	ts about the quickly pending ideas are blithely regul
95012	31	8397	499.21	packages use final, express requests. regular packages h
95013	32	5796	610.98	egular packages. slyly final requests haggle slyly around the express theodolites. bold excuses detect slyly acco
95016	35	9627	701.03	press requests use carefully carefully final dependencies. theodolites hinder along the fluffily final packages. blithely ironic pinto bean
95017	36	1396	964.43	c deposits. carefully final ideas promise blithely. slyly bold accounts grow quickly-- theodolites thrash furiously? final, regular pearls use according to the bold, special deposits. bold package
95021	40	5513	922.96	uctions nag quickly accounts. blithely regular ideas brea
95022	41	9704	603.00	ole furiously unusual deposits. blithely bold courts mold
95023	42	1639	605.06	oost carefully across the carefully unusual requests. ne
95027	46	1933	814.61	s haggle carefully: slyly regular pearls boost quickly under the furiously final ideas. furiously ir
95028	47	8711	704.45	according to the quickly regular packages haggle after the deposits. final platelets among the theodolites was slyly alongside of the evenly final accounts. ironic ideas wake pending, silent ide
95029	48	1298	180.54	among the blithely unusual packages. regular pinto beans boost among the carefully special foxes. carefully express deposits are carefully. furiously regular decoys
95030	49	7428	365.12	fter the regular, ironic deposits cajole along the fluffily ironic accounts. carefully express theodolites wake? accounts sleep slyly regular packages. bli
97491	1	8558	56.34	eodolites wake blithely. regular foxes x-ray carefully carefully ironic deposits. regular theodolites are according to the ironically final accoun
97494	4	4242	885.19	ual requests wake furiously among the ruthless, idle packages. final accounts along the blithely final pinto beans wake about the express instructions. blithely regular packages are furiously e
97496	6	8306	444.09	ronic foxes. ironic, dogged instructions against the bold deposits cajole furiously pending, final asymptotes. special 
97497	7	3629	886.18	gle slyly blithely final packages-- final deposits wake-- fluffily regular packages against the carefully final deposits sleep ironic accounts. special, unusua
97498	8	1064	665.99	s packages. quickly regular accounts affix. silent packages x-ray furiously. regular, ironic accounts sleep according to the regular accounts. furiously express instructions are
97500	10	543	704.78	ely even requests sleep. final packages wake quickly alongside of the 
97501	11	1749	476.60	egular pinto beans. regular ideas across the furiously regular pinto beans nag carefully slyly silent p
97504	14	4314	899.33	out the furiously brave requests. unusual, regular packages cajole carefully according to the slyly daring packages. carefully bold deposits
97506	16	7366	921.85	press excuses. blithely ironic deposits nag slyly requests. ironic platelets serve slyly quickly ironic packages. quickly spe
97507	17	9558	375.24	ounts boost fluffily. blithely ironic accounts nag evenly after the blithely ironic packages. fluffily bold instructions bo
97508	18	8603	629.74	ges haggle blithely. slyly regular dependencies wake slyly fluffily pending requests. ironic pinto beans wake about the carefully pending foxes. even patterns haggle accounts. regul
97509	19	2841	971.57	gular deposits. sly packages detect. carefully express instructions haggle against the regular requests. accounts across the furiously even deposits wake along the carefully final requests.
97515	25	319	761.73	eas integrate quickly. fluffily bold foxes according to the close foxes sleep across the furiously final deposits. accounts cajole about the carefully express foxes. final packages
97516	26	1054	280.27	lyly pending deposits haggle carefully across the fluffily even packag
97517	27	1800	899.13	 across the furiously bold asymptotes. furiously final accounts integrate evenly after the slyly bold deposits. furiously special pinto beans nag blithely above the regular, final pinto beans.
97520	30	2033	199.62	ins boost fluffily about the furiously final deposits. carefully bold deposits are furiously. always regular pinto beans solve fluffily after the express, pend
97521	31	6216	279.93	uriously bold theodolites. furiously express deposits 
97522	32	6120	457.30	iously after the blithely final requests. furiously pending ideas sleep slyly blithely ironic theodolites. carefully final deposits wake even foxes. sheaves aro
97525	35	6247	32.99	the accounts affix slyly with the furiously even tithes. furiously even requests boost carefu
97526	36	4825	888.51	y dependencies wake furiously bold accounts. dependencies sleep carefully! slyly
97530	40	5627	689.89	ns sleep. deposits wake. quickly unusual deposits cajole slyly acco
97531	41	3102	989.24	 boost carefully regular, regular foxes. fluffily final pinto 
97532	42	6445	96.37	re furiously. carefully ruthless asymptotes according to the regular asymptotes sleep special deposits. carefully blithe packages haggle carefully among the 
97536	46	1193	658.65	efully silent dependencies sleep about the slyly pending theodolites. fluffily pending accounts use busily. blithely express ideas nag furious
97537	47	2214	77.98	tegrate carefully across the regular deposits. slyly pending instructions are furiou
97538	48	6039	522.91	fluffily express deposits wake quickly about the special accounts. ironic dolphins nag stealthily. dependencies haggle among the silent packages
97539	49	617	933.81	he furiously final pinto beans. blithely even pinto
100010	11	7372	692.57	ound the slyly pending deposits. slyly regular accounts use quic
100000	1	5944	577.80	unts. foxes sleep quickly. quickly regular deposits nag slyly regular in
100003	4	4086	975.91	g furiously. furiously regular dependencies x-ray. theodolites must nag requests. furiously even asymptotes boost along the furiously final pinto beans. express, regular requests ha
100005	6	6320	378.27	s sleep. silent, pending theodolites about the fluffily ironic requests use slyly blithely express
100006	7	3299	887.13	gside of the bold pinto beans. silent, final grouches haggle quickly against the pending dolphins. silent, unusual packages boost slyly. even, unusual courts a
100007	8	5468	28.49	ual, enticing gifts detect slyly furiously ironic instructions. quickly express deposits cajole. carefully pending accounts use slyly alongside of 
100009	10	8587	581.49	 quietly even packages. carefully careful deposits nag after the unusual, even requests. regular, express pains sleep. even deposits haggle: thinly pending requests sleep regular, re
100013	14	246	333.02	oxes detect. blithely slow ideas sleep across the unusual requests. furiously final courts solve. furiously ironic accounts are. furiously regular forges haggle after the i
100015	16	1763	709.86	lly furiously bold accounts. foxes affix blithely hockey players. instructions against the ironi
100016	17	6277	858.44	y unusual requests. carefully final asymptotes sleep quickly across the unusual platelets. final pinto beans are furiously. regular accounts integrate carefully according t
100017	18	6791	467.09	 carefully unusual notornis according to the theodolites boost fluffi
100018	19	3093	245.82	atop the express deposits. ironically even accounts sl
100024	25	3007	445.81	y according to the regular braids. even accounts according to the 
100025	26	5991	190.88	 accounts. furiously final requests haggle blithely final accounts. quickly ironic deposits among the daringly unusual instructions detect b
100026	27	611	273.96	low ideas are carefully according to the express notornis. even, unusual accounts above the u
100029	30	8409	422.14	c pearls. furiously special packages about the silent instructions nag slyly ironic, even platelets. attainments wake about the slyly unusual packages. sly
100030	31	5033	203.12	 slyly regular requests nag carefully around the foxes. even, dogged a
100031	32	2291	67.86	usual decoys nag stealthily under the carefully final sheaves. even pinto beans integrate. slyly even instructions cajole ironic accounts. even excuses sleep permanently ironic dolphins. carefu
100034	35	3590	863.22	cial instructions. blithely final packages sleep slyly against the pending, final pinto beans. regular, regular packages haggle blithely quickly pending warthogs. always special asymptotes cajole sl
100035	36	5368	125.31	old instructions. furiously regular requests wake carefully. slyly express platelets about the final, pendi
100039	40	3480	891.62	. furiously even deposits across the blithely pending foxes haggle thinly according to the slyly speci
100040	41	1718	934.14	cial packages. slyly regular excuses boost fluffily after the special platelets. f
100041	42	1746	87.29	ts wake fluffily across the furiously special multipliers. quickly regular requests are furiousl
100045	46	2936	685.37	xpress deposits sleep after the ironic requests. quickly regular asymptotes sleep furiously blithely pending accounts. c
100046	47	4605	505.76	 dependencies alongside of the furious requests integrate fluffily careful deposits. express deposits maintain. carefully spec
140013	14	931	166.93	onic instructions grow slyly alongside of the bold request
100047	48	2326	786.81	 detect slyly. final platelets wake. ironic, even pinto beans nag slyly enticingly furious requests. so
100048	49	2943	733.53	 pinto beans nag about the blithely special dependencies. ironic, special frets after the even, regular packages haggle requests. blithely ironic id
102470	1	7520	604.89	ully blithely final packages. always silent pinto beans above the quickly unusual foxes cajole slyly among the asymptotes. special acco
102473	4	295	781.41	e carefully beyond the bold requests. even ideas 
102475	6	4907	32.22	y. carefully unusual pains according to the unusual deposits impress carefully furiously final deposits. pending dolphins grow a
102476	7	7889	488.37	the special, ironic accounts. slyly regular platelets across the regul
102477	8	4569	203.28	ges cajole slyly. requests haggle carefully furiously final grouches. regular pains sleep furiously. pending ideas nag
102479	10	71	369.11	lly regular requests sleep slyly dugouts. unusual instructio
102480	11	1249	854.36	ilent pinto beans sleep never asymptotes. carefully stealthy ideas detect evenly above the busy, ironic somas. unusual deposits
102483	14	7076	913.32	. fluffily final accounts x-ray blithely furiously bold deposits. slyly unusual platelets x-ray furiously silent requests. furiously final accounts use carefully regular d
102485	16	6888	629.45	ly final requests haggle at the pending pinto beans. daring, ironic deposits are fluffily final deposits. furiously busy instructions cajole slyly. regular, special foxes haggle furious
102486	17	4315	432.50	 final packages. slyly unusual accounts are furiously carefully regular requests. furiously final deposits are about the quickly special pinto beans; slyly bold theodolites are carefully caref
102487	18	6652	984.44	in the furiously final pinto beans. carefully final deposits among the bold packages are slyly furiously regular requests. slyly fina
102488	19	7193	695.99	sleep carefully. carefully even pinto beans sleep slyly even instructions. slyly express requests haggle about the furiously final d
102494	25	1411	192.11	egular, express requests? fluffily regular packages use carefully ironic platelets. regular accounts about the slyly unusual ideas sleep quickly among t
102495	26	1542	632.73	as. dinos wake across the regular, bold dependencies
102496	27	267	62.57	 requests nag bold requests: idly unusual instructions do
102499	30	1838	978.03	e regular accounts. carefully quick ideas use accounts. bold platelets sleep fluff
102500	31	4979	282.17	 against the carefully unusual packages boost about the
102501	32	4547	707.87	o the escapades. blithely even deposits nag. fluffily unusual ideas wake blithely quickly special requests. even, regular deposits integrate ca
102504	35	2418	394.48	jole. courts above the fluffily final foxes sleep ironic dependencies. ideas according to the ironic foxes wake carefully silent packages. courts boost fu
102505	36	632	201.01	ld asymptotes. blithely ironic theodolites will are. requests wake slyly slyly ironic packages. furiously special excuses are slowly. furiously pend
102509	40	8698	362.80	ly final deposits along the quickly unusual forges must lose furiously across the furiously even deposits. special, special deposits must believe silent warthogs. unusual wa
102510	41	466	140.98	ending platelets use. quickly final deposits cajole quickly instructions-- final ideas wake slyly. regular requests use blith
102511	42	2756	912.24	old, regular accounts haggle fluffily regular ideas-- blithely ironic foxes wake around the slyly express foxes. 
102515	46	1447	580.02	uriously final instructions haggle after the blithely silent accounts. pinto beans dete
102516	47	7975	778.35	sits. instructions are furiously carefully ironic packages. dep
102517	48	3812	650.39	ronic packages haggle. even ideas cajole furiously. blithe
102518	49	7665	646.86	ross the quick platelets need to cajole requests. closely ironic asymptotes lose carefully against the quickly express theodolites. slyly 
104980	1	2861	348.21	along the unusual requests affix alongside of the slyly regular platelets. slyly bold excuses
104983	4	3168	497.25	dencies believe slyly even accounts. closely bold foxes snooze blithely. blithely
104985	6	9843	53.32	y close platelets. blithely final packages wake daringly above the d
104986	7	7543	849.58	carefully daring ideas. dependencies wake blithely in place of the quickly unusual packages. bold, final
104987	8	4776	103.85	nusual ideas boost quickly regular foxes. pains sleep quickly for the asymptotes. doggedly express excuses cajole ironic platelets. blithely unusual excuses against the courts are qu
104989	10	6839	774.76	counts use fluffily alongside of the requests; carefully ironic asymptotes wake. final, final packages cajole furiously about the slyly ironic packages. carefully regular instructions kindl
104990	11	586	114.87	usly pending accounts. even foxes nag slow, quiet packages. carefully final dependencies affix carefull
104993	14	9143	670.55	ly bold excuses nag carefully around the final deposits. slyly close theodolites wake slyly near the slyly pending accounts. bold, pe
104995	16	640	366.82	e furiously ironic instructions haggle quickly about the regular pinto beans. furiously pending orbits detect regular, bold theodolites. carefully unusual packages may sleep about t
104996	17	5231	18.08	le dependencies use slyly along the carefully special depos
104997	18	2180	842.74	s are enticingly according to the blithely regular ideas. fluffily pending accounts cajole quickly 
104998	19	4190	199.91	 pending packages sleep about the slyly regular dolphins. ironic, even foxes solve
105004	25	3561	230.35	ld packages nag quickly until the bold theodolites. blithely pending instructions use. carefully blithe deposits wake slyly against the furiously brave requ
105005	26	7810	28.63	 snooze fluffily. special, ironic requests boost ruthlessly before the fluffily unusual packages. slyly regular instructions impress. blithe
105006	27	4913	564.48	unts sublate slyly after the special, special requests. final
105009	30	6152	92.17	. slyly express excuses wake slyly according to the silently pending ideas. slyly even accounts haggle fluffily regular instructions. ironic instructi
105010	31	4308	625.56	long the furiously even instructions. regularly ironic deposits doubt blithely among the quickly ironic packages. carefully final requests must nod along the furiously pending asymptot
105011	32	9199	797.28	 pains sleep deposits. carefully final theodolites along the fluffily busy packages wake
152471	17	7179	420.53	carefully pending platelets above the carefully ironic packages are quickly above the unusual foxes. pack
105014	35	6531	872.93	 packages haggle. carefully ironic deposits along the final theodolites haggle carefully carefully express ideas. blithely silent foxes impress above the carefully regular asy
105015	36	4106	766.59	e accounts are stealthily final, unusual requests. final, silent packages sleep bold, ironic ideas. blithely eve
105019	40	5615	418.32	uriously bold foxes. quickly unusual requests are. requests are. slyly reg
105020	41	8652	231.77	osits cajole. furiously pending pinto beans among the fluffily regular theodolites boost along the carefully regular deposits. special platelets hinder f
105021	42	7626	522.50	nto beans. blithely sly requests after the busy theodolites cajole slyly even accounts. fluf
105025	46	8556	900.50	ges. furiously unusual requests about the blithely even dependencies shall wake above the even, 
105026	47	8397	853.65	e carefully after the brave theodolites. fluffily final frays according to the regular accounts boost furiously ironic packages. ironic dolphins haggle furiously slyly final 
105027	48	4997	671.76	h the regular instructions affix alongside of the furiously ironic dolphins. carefully regular asymptotes sleep silently. fluffily even asymptotes against the regular, iro
105028	49	1762	572.14	nst the carefully even warhorses. instructions try to use furiously after the ironic deposits. fluffi
107490	1	4684	676.22	ke slyly regular realms. quickly bold theodolites sleep carefully unusual deposits. silent
107493	4	5457	178.98	r packages affix furiously quickly final asymptotes. slyly reg
107495	6	514	871.19	 silent foxes wake above the realms-- even foxes across the furiously even accounts sleep furiously unusual requests. furiously ironic requests among the regular packages hagg
107496	7	8828	234.20	ts. furiously quick deposits haggle silently above the slyly unusual theodolites. even dolphins are furiously unusual theodolites. 
107497	8	3876	11.95	 ironic, even asymptotes-- quickly regular ideas about the deposits detect silent theodolites
107499	10	3470	122.89	e instructions boost behind the carefully bold deposits. regular packages wake furiously ironic requests. furiously ironic warhorses maintain carefully. enticingly ruthless 
107500	11	4476	960.12	ular deposits according to the packages detect about the blithely regular sheaves. fluffily pending foxes haggle according to the regular requests. pinto bean
107503	14	9770	923.10	structions wake carefully final packages. pending courts cajole about the blithely final asymptotes. slyly regular excuses abov
107505	16	9279	541.57	ress, final asymptotes serve? blithely final instructions haggle for the requests: slyly final requests serve after the theodolites. idle, even theodolites 
107506	17	9770	707.82	s. regular foxes nag carefully. ironic foxes are slyly. blithely regular warhorses use carefully regular, ironic pinto beans. blithely final instructions kindle. busily
107507	18	6412	93.09	counts. furiously regular instructions haggle carefully: slyly unusual frays are blithely carefully bold theodolites. regular ideas try to are regular accounts-- express, unusual asymptotes a
107508	19	6892	946.36	 pending foxes. final foxes are blithely. idle accounts wake about the regular, unusual ideas. ideas use slyly. ca
107514	25	9566	468.61	x blithely ironic requests. furiously regular packages cajole
107515	26	601	671.22	ckages cajole. blithely unusual packages sleep carefully. bold, ironic accounts nag carefully pending, final deposits
107516	27	636	82.15	t carefully. blithely pending packages mold despite the blithely regular excuses. theodolites boost slyly unusual instructions. regular, i
107519	30	4727	807.22	. requests alongside of the quietly bold foxes sleep carefully according to the asym
107520	31	7395	952.34	of the blithely ironic requests. carefully regular deposits mold carefully regular deposits. silently unusual packages cajole 
107521	32	2155	228.54	lar packages. ironic grouches are furiously after the slyly final requests. busily regular packages are among the blithely final theodolites. slyly final deposits sleep furiously. quickly 
107524	35	317	596.01	y sly excuses. final, regular instructions are quickly against the slyly pendi
107525	36	3748	667.11	nic requests sleep furiously final, unusual dolphins. carefully final requests boost slyly across the accoun
107529	40	5911	542.92	close packages solve carefully. regular theodolites are. slyly pending accounts about the silent accounts affix c
107530	41	904	484.48	le. unusual, sly platelets boost. quickly regular requests might use above the furiously unusual pinto beans. boldly pending deposits ar
107531	42	6516	844.30	final dependencies. depths about the unusual theodolites impress carefully closely regular pinto beans. fluffily even sentiments hang enticingly above the furiously regular 
107535	46	2701	392.32	unts wake furiously. final, regular ideas haggle slyly
107536	47	8262	782.84	sly final platelets wake according to the blithely regular foxes. special instructions sublate quickly. furiously regular deposits doubt deposits. regular pl
107537	48	6362	926.96	eodolites. carefully permanent packages hinder. deposits are carefully. special acco
107538	49	2846	925.20	requests must sleep furiously express foxes. regular theodolites wake fluffily furiously ir
110000	1	8121	547.82	packages engage slyly deposits; quickly ironic requests wake. packages according to the blithely idle dependencies are sl
110003	4	213	281.70	usy requests. regular, regular requests haggle carefully ironically ironic pinto beans. furiously pending instructions boos
110005	6	7142	841.55	ronic dependencies. final dependencies snooze requests! furiously express foxes play. fluffily regular foxes nag.
110006	7	861	851.50	e furiously deposits. regular accounts unwind furiously against the carefully unusual theodolites. slyly regular accounts must are
110007	8	4596	943.28	against the even, express accounts. regular instructions us
110009	10	5225	188.46	express deposits along the slyly final dependencies boost alongside of the slyly silent accounts. even ideas wake carefully. bold d
110010	11	809	139.30	iously after the unusual excuses. regular deposits alongside of the
110013	14	2478	525.73	the slyly even packages boost fluffily after the regular pinto beans. carefully regular theodolites wake carefully slyly ironic depende
110015	16	7262	470.38	furiously even pinto beans. furiously final tithes among the blithely pending pack
110016	17	1919	233.42	out the ironic, unusual deposits. final, silent ideas boost after the foxes. express, final requests unwind carefully bold deposits. bold, final deposits
110024	25	127	997.56	wake above the slyly unusual sentiments. quickly regular forges nag blithely furiously final deposits. quietly regular packages sleep blithely ironic deposits. fluffily ev
110025	26	5918	707.38	ts. special requests are slyly above the carefully even packages; furiously special asymptotes integrate carefully blithely bold depos
110026	27	8218	666.40	 requests wake furiously. bravely regular packages hang
110029	30	7781	496.19	sts. pending, ironic dolphins are slyly. unusual dolphins boost busily after the slyly even sauternes. quickly even excuses snooze across the furiously regular deposits. fl
110030	31	7058	897.64	re slyly carefully bold deposits. even, regular pinto beans are furiously. even packages about th
110031	32	7672	650.98	xcuses. carefully regular theodolites wake. even accounts are furiously after the express deposits. silently ironic instructions use quickly along the ironic requests! final instru
110034	35	1619	335.53	refully against the boldly bold foxes. carefully ironic re
110035	36	1220	413.53	 final deposits. even, pending accounts nag. carefully ironic theodolites are furiously furiously bold deposits. blithely final reques
110039	40	2911	118.49	ual accounts. blithely ironic pinto beans impress. even foxes affix blithely according to the asymptotes. slyly regular packages use express, special ideas. reques
110040	41	6627	646.91	pecial, even deposits cajole blithely bold, final ideas. fluffily pending accounts haggle furiously slyly ironic excuses. furiously ironic instructions boost slyly pendi
110041	42	3068	342.42	hely final requests haggle quickly! carefully regular instructions sleep slyly quickly final ideas. regular, ironic deposits about the slyly 
110045	46	9843	478.76	r the unusual, ironic packages wake slyly according to the fluffily even deposits. blithely even requests affix carefully since the decoys. regular as
110046	47	2284	413.31	tealthily along the forges. carefully express realms serve furiously bold foxes. ironic, regular deposits affix slyly across the idly pending courts. attainme
110047	48	5672	602.78	 instructions. ideas could have to are ironic accounts? quickly even pinto beans us
110048	49	2473	669.35	 pending accounts. express, express patterns cajole slyly even ideas. quickly special theodolites haggle idly blithely ironic accounts. blithely final deposits sleep quickly. slowly
112467	1	6818	651.77	lent, unusual instructions. evenly pending pinto beans nod furio
112470	4	4291	707.18	y asymptotes. carefully special platelets use furiously bold patterns: even requests snooze blithely. idly final frets against the s
112472	6	9929	558.52	ependencies. unusual pinto beans above the slyly bold accounts x-ray regular ideas. accounts against the ironic accounts are around the permanently silent pearls. silent requests wake quickl
112473	7	7750	703.02	ts about the ironic theodolites detect at the even accounts. always spe
112474	8	6930	688.68	quests. blithely special packages wake blithely. blithely ironic packages are about the slyly even deposits. quick, ironic d
112476	10	1389	398.13	to beans among the bold, regular deposits haggle blithely furiously ironic foxes. blithely regular accounts detect quickly. regular, ironic requests p
112477	11	2876	571.48	usly. final, pending accounts nag furiously. unusual, regular theodolites cajole carefully. slyly final excuses above the iro
112480	14	2689	169.36	ithely final realms according to the quick pinto beans wake furiously wa
112482	16	715	852.11	ts wake. regular pinto beans above the quickly even escapades cajole enti
112483	17	4385	801.38	ly unusual accounts about the slow, special ideas cajole carefully ironic theodolites. packa
112484	18	7503	783.68	regular requests. blithely pending instructions affix busily
112485	19	6493	656.11	fully special pinto beans unwind even, ironic requests. hockey players haggle slyly even requests. slyly final pinto beans doze. blithely final instructions kindle carefully agai
112491	25	5805	979.14	cies sleep slyly. blithely pending deposits haggle blithely. quickly pending deposits boost. ironic pinto beans after the ironic tithes boost slyly blithely ironic accounts. b
112492	26	521	819.68	pending ideas. express, bold requests do sleep except the ide
112493	27	6167	96.81	press requests was about the packages. requests haggle carefully at the carefully even 
112496	30	5887	67.09	furiously unusual ideas sleep fluffily alongside of the silent, ironic accounts. carefully special dependencies use slyly after the even accounts! carefully bold deposits haggle carefully alon
112497	31	6835	867.61	aggle across the special, even accounts. even packages haggle across the unusual, idle accounts. instructions are about the dugouts. slyly special theodolites shoul
112498	32	8841	536.00	usly even packages nag blithely. final patterns slee
112501	35	9892	628.08	s. fluffily pending deposits wake carefully. pinto beans sleep. blithely final packages above the multipliers
112502	36	6944	872.53	g according to the quick packages. express theodolites against the carefully bold braids haggle fur
112506	40	8011	362.13	 regular ideas. finally bold theodolites cajole furiously carefully final accounts. pending, fluffy dolphins boost even, final requests. pinto beans sleep slyl
112507	41	1628	548.53	ffily ironic foxes; fluffily bold requests affix quickly. foxes believe. blithely silent hockey players haggle daringly slyly bold ac
112508	42	3841	11.87	efully daring pinto beans. blithely even accounts haggle ironicall
112512	46	4385	611.65	ounts. final, idle foxes wake fluffily above the carefully special theodolites. ironic
112513	47	6248	337.02	latelets. careful, pending platelets run carefully. special, special platele
112514	48	2074	763.98	. slowly bold requests along the final, ironic pinto beans hinder ironic, express packages. silent, regular sheaves can use. blithely regular accounts along the blithely bold frets kindle blith
112515	49	7854	306.74	blithely quickly silent deposits: furiously express deposits cajole. furiously dogged packages after
114978	1	9265	783.82	ar deposits around the furiously final requests use carefully across the quickly unusual realms. blithely regular theodolites are quickly. blithe
114981	4	2630	43.85	ages haggle fluffily above the blithely bold requests. carefully careful foxes above th
114983	6	963	364.70	ct quickly forges. packages are. unusual pinto beans doubt requests. stealthily pending deposits wake furiously final platelets-- blithely express packages snooze furiously upon the special, i
117534	46	3404	125.18	lyly bold ideas haggle blithely express deposits. dependencies boost furiously accounts. slyly silent foxes doze atop the final,
114984	7	4609	772.35	s serve carefully quickly express packages. carefully final accounts sleep. slyly silent braids boost according to the even platelets. accounts against the final, express packages haggle quickly a
114985	8	6803	96.61	furiously express theodolites run always regular r
114987	10	7520	602.10	t warhorses. requests are. furiously unusual excuses are about the ironic sauternes. even theodolites are quickly across the carefully regular accounts. slyly ironic orbits above the ironic, i
114988	11	8584	773.19	ly against the accounts. slyly express foxes wake blithely silent requests. furiously special requests wake quickly across the ironic instructions. accounts alongside of 
114991	14	888	790.67	 ideas. ironic ideas are slyly. accounts wake furiously. express asymptotes wake quickly. furiously unusual pinto beans are. bold, fluffy requests cajo
114993	16	9714	107.74	nusual accounts hinder slyly final deposits. bold sheaves use iron
114994	17	1115	545.45	gular attainments-- quickly express requests are express foxes. final, bold pint
114995	18	8408	141.21	usual packages. carefully final pinto beans use qu
114996	19	6659	658.75	ecial dinos. deposits haggle blithely quickly regular packages. silent dolphins sleep slyly final, final requests. even requests thrash blithe
115002	25	8163	517.85	le. quiet deposits run blithely regular foxes. express packages need to believe. bold dependencies wake blithely. final ideas boost slyly even pinto beans. slyly regular foxes are slyly brave fo
115003	26	8035	365.38	y unusual asymptotes. final instructions against the final in
115004	27	936	734.44	y carefully regular accounts. fluffily ironic foxes use. deposits wake quickly platelets. slyly thin packages haggle. slyly pending foxes cajole quickly bold pinto beans! blit
115007	30	9000	738.67	along the regular theodolites. requests detect ironic, final pinto beans. courts are slyly ironic packages. slyly ironic attainments about the quickly final ideas are furiously special accounts. si
115008	31	4530	499.80	olites. boldly even asymptotes unwind at the regular, express packages. carefully 
115009	32	9137	79.13	aggle among the quickly regular dependencies. carefully fluffy packages above the fin
115012	35	1372	81.38	s wake up the frets. pending, even packages believe slow, regular deposits. furiously final deposits integrate quickly bold asympto
115013	36	4994	245.37	, careful dolphins. special foxes about the slyly even foxes are blithely after the furiously final requests. carefully unusual packages wake carefu
115017	40	4024	765.97	 blithely even foxes detect carefully closely final excuses. slowly even accounts integrate 
115018	41	8504	401.20	oost. final, bold deposits cajole furiously. carefully regular packages above the quickly expres
115019	42	5547	817.96	ording to the packages promise slyly among the carefully ironic deposits. regular frays haggle blithely. requests above the 
115023	46	3992	780.30	e carefully carefully special pinto beans. requests according to the slyly express instructions are slyly ironic accounts. pending, express courts haggle furiously carefully final foxes
115024	47	2348	473.80	he furiously regular platelets. furiously express requests sleep against the carefully bold accounts. carefully final platele
115025	48	2998	775.26	 deposits. blithely express excuses about the fluffily ironic accounts wake carefully along the th
115026	49	5609	529.85	egular deposits sleep across the quickly final deposits. furiously 
117489	1	4190	760.49	ns about the furiously final ideas use closely above the furiously final theodol
117492	4	6254	817.85	lithely express deposits. furiously even foxes wake furiously express requests. regular, ironic de
117494	6	7994	784.54	sleep furiously packages. deposits cajole quickly carefully even
117495	7	1008	604.57	. regular, pending realms cajole carefully. pending foxes believe carefully. ironic, pending deposits nag
117496	8	2113	149.31	xpress forges use across the blithely silent packages. quickly fina
117498	10	1788	499.68	ckages wake furiously ironic, final theodolites. furiously pending deposits are fluffily. ironic requests wake fluffily about the regular, final 
117499	11	4009	722.65	ular deposits after the final excuses cajole after the furiously ironic forges. carefully final packages after the daringly exp
117502	14	9917	353.21	y slyly final packages. slyly final requests cajole about the fluffily busy packages. carefully express theodolites along the depo
117504	16	4907	979.67	ithely quiet instructions. blithely special dolphins after the ironic, 
117505	17	9710	870.52	e ironic dolphins-- fluffily even accounts haggle blithely carefully final packages. blithely bold accounts haggle furiously? furious accounts haggle carefully special pinto beans. ca
117506	18	6397	212.93	according to the slyly final packages. carefully final deposits across the ironic epitaphs play furiously above the bold theodolites. package
117507	19	6180	127.13	ep along the bold packages. pending, bold ideas cajole fu
117513	25	226	304.15	ly ironic asymptotes nag. bold theodolites snooze furiously pending packages. express theodolites boost permanently. regular requests cajole blithely even foxes. ruthlessly even accounts engag
117514	26	6745	921.24	jole against the regular platelets. even, close sauternes a
117515	27	7040	524.69	lar deposits cajole blithely fluffily pending courts. furiously pending asymptotes nag requests. quick
117518	30	8835	62.98	ly bold foxes run blithely even, final orbits! silently ironic excuses integrate fu
117519	31	6565	70.52	ites are blithely furious requests. carefully ironic foxes are fluffily slyly unusual theodolites? pending requests print furiously special dependencies. forges sleep aro
117520	32	1758	796.93	y even requests use blithely even courts. fluffy, final t
117523	35	682	279.36	its. ironic requests haggle daringly. even, unusual requests about the blithely bold foxes nag furiously furiously ironic pinto beans. slyly express warthogs boost blithely according to th
117524	36	1955	22.40	ly final courts. slyly express pinto beans boost carefully regular excuses. ironic, final deposits promise carefully. carefully final accounts sleep slyly. final packages slee
117528	40	3987	121.34	 the final instructions. thin, regular deposits haggle carefully along the silent packages. bravely final foxes grow 
117529	41	3697	585.79	ngly bold packages are above the regular, regular packages. slyly regular packages use carefully. furiously regular packages be
117530	42	38	776.90	kages at the quickly unusual accounts sleep slyly silent dependencies. fluffily sly pinto beans cajole after the final accounts. slyly bold packages breach f
117535	47	931	316.43	. quickly express dependencies grow. accounts run quickly. furiously final deposits cajole finally. final, pending ideas haggle 
117536	48	9173	140.33	uctions. furiously special ideas about the regular platelets cajole blithely
117537	49	5771	8.46	ids. carefully pending instructions sleep ironic excuses. carefully regular requests are. quickly even deposits breach quickly. pinto beans are blithely across the gifts. blithely pending deposit
120006	7	8719	592.81	requests x-ray. express, regular deposits nag fluffily. pinto beans haggle carefully. bold deposits across the fluffily even asympto
120000	1	2357	39.73	to integrate. furiously even requests haggle carefully. blithely expres
120003	4	5852	487.52	le slyly along the final, regular accounts? foxes above the dependencies promise carefully furiously silent deposits. ironic requests cajole furiously across the quickly regu
120005	6	5017	554.41	 ruthless accounts boost fluffily; special, silent platelets cajole. slyly 
120007	8	4052	321.07	packages affix blithely accounts. carefully ironic pinto beans sleep blithely? blithely express dependencies believe blithely slyly regular accounts. furiously regular requests
120009	10	5452	275.71	pecial theodolites. carefully final platelets haggle blithely regular accounts. blithely bold requests detect blithely daring deposits. carefully unusual patterns wake. furi
120010	11	2409	831.52	eodolites thrash furiously. carefully bold accounts run quickly close deposits. blithely
120013	14	6584	477.25	ts boost closely platelets. quickly express requests breach carefully regular accounts. slyly regular instructions serve blithely car
120015	16	3954	872.89	y quiet foxes cajole. carefully bold platelets nag. slyly pending accounts affix. furiously bold dependencies cajole carefully blithely final theodolites. ironic, f
120016	17	9243	461.91	regular requests haggle. stealthily regular forges run slyly. quickly express pinto beans detect f
120017	18	1403	179.80	ress evenly. silent, ironic deposits haggle blithely above the slyly even frets. pending, final sheaves affix slyly above the final deposits. daringly regula
120018	19	4991	618.88	after the carefully brave frays. silent requests cajole blithely. fluffily even accounts nag. even requests boost furiously slyly ironic packages. furiously final platelet
120024	25	2909	283.31	requests run slyly after the pending ideas. carefully ironic requests boost furiously above the blithely even asymptotes; slyly exp
120025	26	7203	633.19	ly even foxes sleep blithely carefully final requests. fur
120026	27	1138	396.12	iously in place of the furiously sly pinto beans. quick
120029	30	662	535.00	ic instructions nag ironic, ironic deposits. furiously regular requests along the idle instructions engage carefully final, even packages. ironic pinto beans haggle after the special foxes. carefull
120030	31	6607	935.89	was fluffily regular theodolites. furiously ironic excuses wake after the ironic packages. excuses haggle slyly-- pending, regular accounts cajole quickly final theodolites. final
120031	32	6377	622.75	nding foxes sleep slyly idle, regular patterns! furiously regular packages will are furiously above the quickly final asymptotes. unusual packages despite the express, final requests sleep unus
120034	35	5428	112.22	bold deposits. furiously ironic theodolites grow slyly. regular excuses grow silent, ironic excuses. express instructions along the packages serve furiously quickly ironic asymptotes. slyly re
120035	36	1374	312.46	wake quietly. sly packages despite the theodolites breach near the even
120039	40	4033	362.65	xcuses are quickly. final requests boost finally after the ironic requests. slyly regular packages about the ironic Tiresias are after t
120040	41	891	462.54	the instructions. blithely express accounts wake furiously. instructions breach furiousl
120041	42	7013	574.88	 the regular, even asymptotes. regular dependencies wake carefully furiously even requests. blithely pending packages use slyly fin
120045	46	1787	233.58	e evenly express dependencies. quickly special packages are furiously. special, special excuses sleep slyly across the carefully bold requests. fluffily special
120046	47	5390	943.37	al dependencies. requests sleep furiously. express, regular theodolites unwind slyly. courts wake. even, bold saut
120047	48	6771	982.23	osits at the furiously regular accounts nod carefully ironic deposits! pending accounts nag blithely? blithely regular ideas boost. pending, final ideas d
120048	49	359	54.60	excuses. slyly special accounts affix slyly across the special platelets. even instructions cajole express, bold deposits. fluffily regular packages around the slyly unusual 
122464	1	554	521.19	gular foxes-- furiously quiet platelets haggle slyly against the slyly bo
122467	4	6017	612.10	ly even Tiresias. deposits nag carefully on the slyly special pinto beans. sly packages promise furiously. brave, final deposits haggle express deposits. slyly special pinto beans are carefully a
122469	6	203	473.22	 the carefully bold instructions nag quickly about the even foxes. slyly final theodolites wake blithely among the pending, unusual requests. slyly final d
122470	7	1615	62.19	xpress packages. fluffily pending asymptotes are alongside of the ironic, pending dependencies. blithely pending instructions hag
122471	8	8921	240.02	ar accounts cajole slyly regular frays. special deposits boost carefully final packages. furiously final foxes are carefully. slyly even attainments impress carefully across the fin
122473	10	1979	231.21	ges engage pending, unusual packages. blithely blithe deposits sleep slyly iron
122474	11	6657	657.33	as use slyly. blithely ironic requests haggle furiously regular packages. stealthy theodolites sleep quickly. carefully unusual pinto beans after the slyly express requests sleep blithely de
122477	14	1848	398.28	: final instructions wake quickly across the regular deposits! regular foxes about the unusual, ruthless requests cajole fluffily according to the deposits. blithely f
122479	16	7163	411.00	ckages. fluffily regular excuses wake quickly above the carefully bold theodolites. quickly regular requests caj
122480	17	8364	833.23	ges wake. slyly special excuses cajole carefully across the furiously final patterns. fluffily unusu
122481	18	6295	704.34	 packages. furiously final accounts haggle carefully? fluffily ironic instructions use blithely above the furiously stealthy requests-- blithely express packages detect furiously alongside of the
122482	19	7856	136.55	olve furiously ironic, special deposits. bold epitaphs are fluffily. special, express requests aro
127494	7	3618	450.15	fily between the carefully ironic depths. unusual pinto beans haggle slyly above the pinto beans. quickly reg
122488	25	8134	656.09	oxes cajole carefully with the carefully even excuses. slyly ironic packages kindle furiously enticing sentiments. bravely pending requests alongs
122489	26	8575	966.96	ess asymptotes sleep blithely about the furiously regular accounts. 
122490	27	9139	774.96	cajole fluffily about the quickly final accounts. furiously final accounts affix carefully about the slyly special theodolites. slyly special pinto bea
122493	30	3648	631.22	 special platelets doubt over the regular deposits. carefully ironic packages cajole furiously regular pinto beans. slyly express asymptotes cajole ideas. fluffily even orbits boost. pending 
122494	31	3550	215.46	e requests. blithely ironic packages maintain slyly regular theodolites. regular deposits cajole quickly. pending, bold accounts may promise carefully slyly regular packages. quickly bold so
122495	32	3674	409.61	 fluffily pending hockey players need to sleep foxes. furiously ironic requests haggle quickly quickly silent accounts; blithely express requests alongside of the fluffily final requests use agains
122498	35	6040	661.85	jole carefully across the blithely bold excuses. carefully express packages us
122499	36	9201	441.69	ess deposits use about the furiously silent packages. bold theodolites sleep slyly alongside of the slyly ironic instructions. carefully even courts haggle according to the furiously regula
122503	40	7472	329.97	fully gifts. dependencies cajole atop the instructions. carefully ironic deposits across the furiously iro
122504	41	2271	154.73	ts wake furiously closely unusual asymptotes. furio
122505	42	7184	574.20	 boost slyly fluffily even requests: furiously bold
122509	46	6588	940.00	ans after the ironically regular theodolites are carefully furiously bold deposits. carefully bold requests nag stealthily above the accounts. furiously regular foxes are furiou
122510	47	2241	577.68	ironic requests wake furiously ironic instructions. blithely ironic instructions cajole idly even, ironic accounts: boldly express accounts cajole sl
122511	48	9272	803.97	unts haggle slyly slyly even packages. final deposits grow slyly unusual accounts. slyly ironic requests wake about 
122512	49	1088	762.81	al excuses. fluffily regular theodolites against the foxes wake slyly furiously express ideas. quickly ironic theodolites integrate furiously. enticingly even accounts are slyly
124976	1	9010	705.48	ly. unusual, final dugouts use careful, unusual deposits. permanent packages along the bold deposits use furiously special asymptotes. carefull
124979	4	6813	163.46	gular instructions. accounts nag blithely blithely even deposits. boldly pending accounts nag carefully according to the accounts. ironic deposits according to the slyly fin
124981	6	3663	794.65	final accounts boost furiously deposits. permanently idle ideas play slyly fluffy sauternes. idle ideas sleep doggedly across the regular deposits. furious
124982	7	1328	957.80	eep slyly. quickly regular pains use final, final requests. carefully ironic packages cajole fluffily slyly unusual requests. furiously bol
124983	8	8297	968.31	en theodolites nag quickly-- unusual, bold requests cajole fluffily above the f
124985	10	8728	504.28	ests haggle. carefully final instructions wake fluffily. blithely regular packages across the carefully special
124986	11	4710	58.38	ly express requests. slyly ironic platelets among the furiously final accounts wake against
124989	14	8908	361.11	, final attainments use. blithely regular packages wake carefully. furious
124991	16	3010	54.54	lyly regular accounts x-ray blithely express deposits. carefully enticing asymptotes haggle. slyly regular packages aft
124992	17	5464	107.38	e furiously unusual accounts. fluffily brave packages according to the regular, express ideas use about the express re
124993	18	5176	828.16	ithely bold deposits according to the boldly ironic ideas doze fluffily thinly bold accounts. ironically pending id
124994	19	1704	436.15	 unusual excuses wake regular packages. carefully ironic requests haggle blithely. quickly regular requests sleep fluffily final excuses. even foxes haggle daringly. express pa
125000	25	9971	31.71	olve fluffily. carefully final deposits sleep after the busy dependencies. express, special deposits detect iron
125001	26	6178	959.11	ve slyly special grouches. silent dependencies sleep furiously. regular, express deposits wake. carefully final theodolites haggle blithe
125002	27	3693	16.92	ts. even, express deposits haggle carefully furiously express packages
125005	30	884	225.04	sual, silent notornis engage carefully. bold packages nag slyly pending courts. blithely sly deposits nag
125006	31	493	345.72	y regular deposits nag fluffily after the express, ex
125007	32	6921	235.09	onic decoys. blithely brave dolphins nag blithely according to the instructions. furiously silent realms integrate quietly after the carefully silent pai
125010	35	4329	899.80	thes are permanently. special requests haggle slyly ironic, silent pinto beans. quietly pending
125011	36	6645	733.78	 theodolites. carefully pending excuses after the carefully express pinto beans wake against the carefully even excuses. foxes sleep careful
125015	40	862	471.44	around the fluffy packages wake carefully furiously regular packages. stealthily bold excuses haggle 
125016	41	9942	568.52	ctions. furiously final dolphins sleep quickly. silent dependencies sleep blithely furiously regular requests. final deposits boost carefully. slyly final accounts are sl
125017	42	4629	54.47	ironic instructions sleep blithely around the regular packages. final re
125021	46	7860	152.84	ackages doze among the blithely regular requests! even instructions across the blithely final instructions haggle packages. ironic forges wake fluffily among the 
125022	47	3103	771.71	uctions. slyly daring theodolites are blithely am
125023	48	8505	419.42	ajole carefully blithely silent platelets. slyly even requests boost never above the evenly regular theodolites. ironic sheaves alongside of the special theodolites cajole 
125024	49	2810	366.21	ully final realms affix quickly about the furiously enticing theodolites: final deposits must hang slyly across the carefully ironic somas! final, bold theodolites are. bold orbits are never a
127488	1	1694	625.67	ic packages; quietly even dependencies breach. carefully unusual asymptotes haggle along the even dependencies: foxes boost regularly. expre
127491	4	1719	306.12	 unusual requests after the fluffily brave pearls haggle about the fluffily enticing Tiresias. accounts since the bold instr
127493	6	5813	718.29	ts. regular, express deposits haggle blithely regular deposit
127495	8	8852	488.25	nstructions cajole pending excuses. slyly express asymptotes alongside of the furiously regular requests sleep slyly according to the quickly regular packages. slyly i
127497	10	4977	745.93	tructions. bold, final accounts promise carefully pen
127498	11	4979	543.50	o beans use slyly above the packages. furiously regular deposit
127501	14	6738	440.02	hely special requests about the ironic, regular pinto beans sleep furious accounts. fluffily regular instructions solve after
127503	16	905	398.13	t the bold accounts believe furiously across the carefully ironic excuses? blithely regular theodolites lose carefully according to the carefully special requests
127504	17	5897	94.01	ts. packages detect slyly slow, silent accounts. final excuses was carefully furiously even deposits. daringly ironic deposits after the slyly ironic pinto beans doubt fluffily
127505	18	1278	231.53	ccording to the slyly bold pinto beans. furiously pending accounts wake quickly around the bravely final deposits. final ideas are blithely. slyly even frays sleep quickly fluffily ironic depos
127506	19	4952	8.14	ously even gifts lose. even theodolites wake pending requests. furiously unusual accounts boost furiously past the slyly bold requests. regular ideas sleep doggedly. slyly express instructi
127512	25	119	72.03	t asymptotes cajole according to the theodolites. regular packages about the dugouts wake blithely pending foxes? slyly final deposits cajole furiously. slyly express instructions play ironic r
127513	26	9986	212.18	riously regular requests. slyly fluffy accounts along the carefull
127514	27	5534	413.76	ironic forges cajole. pinto beans across the carefully
127517	30	5987	910.96	esias. quickly final forges among the quickly silent deposits sleep around the slyly pending dependencies. special, permanent requests 
127518	31	3063	89.53	ests are boldly. carefully ironic ideas above the special packages poach regular, silent asymptotes. slyly ironic ideas across the packages shall grow dependencies. blithely
127519	32	6060	60.08	 express asymptotes use blithely pending theodolites. bl
127522	35	9236	900.32	nts. foxes across the slyly special pinto beans impress
127523	36	1689	881.05	ges cajole fluffily fluffily even deposits. accounts boost blithely after the special requests. special, dogged pin
127527	40	2918	251.59	nal packages sleep above the ironic, regular deposits. slyly bold deposits are around the carefully ironic courts. bravely ironic ideas use blithely around the final
127528	41	4549	56.68	e slowly daringly ironic requests. deposits wake quickly among the pending, unusual pinto beans. furiously regular requests haggle slyly. ruthlessly express accounts wake packages. special, re
127529	42	9032	475.11	nal deposits. regular asymptotes boost. permanent, ironic theodolites are furiously. slyly silent platel
127533	46	1197	430.74	yly express dolphins sleep. carefully ironic foxes serve. pending requests are against the special, iron
127534	47	3531	665.98	 final ideas. ironic deposits should have to sleep blithely regular, even instructions? furiously even instructions are! slyly bold pinto bea
127535	48	8147	310.38	 blithely around the bold ideas. ironic, final deposits caj
127536	49	9928	953.49	ously. slyly even platelets sleep. furiously even instructions
130000	1	3558	516.36	ding excuses. silent warthogs about the daringly express orbits wake pe
130003	4	2610	262.11	ions sleep slyly. fluffily regular attainments haggle slyly across the blithely silent ideas. carefully regular requests cajole careful
130005	6	1172	644.67	pinto beans; quickly final deposits above the ironic frays wake carefully regular requests. unusual accounts along the blithely regular req
130006	7	1339	495.95	ily special foxes. blithely special packages wake among the slyly final account
130007	8	1260	366.83	 the instructions. carefully final deposits maintain furiously regular, ironic requests. packages wake quickly. silent, express excuses wake slyly along t
130009	10	10	345.76	ts are quietly dogged accounts. asymptotes serve slyly final packages. furiously final deposits sleep about the daringly unu
130010	11	8700	224.50	yly. quick accounts cajole. slyly final pinto beans wake slyly even requests. even theodolites cajole furious
130013	14	2295	149.56	ideas are alongside of the bold deposits. theodolites nag furiously behind the slyly regular accounts. fluffily close instructions cajole quickly. bold, ironic pinto beans wake furiously alongside o
130015	16	2393	575.66	gular, final requests. dependencies sleep carefully behind the ironic instructions. platelets detect slowly slyly final pinto
130016	17	6281	660.88	ckly blithely even warhorses. slyly pending theodo
130017	18	7870	32.36	ng the even, regular packages. blithely final pinto beans doze furiously ironic requests. ironic acco
130018	19	3350	332.63	sly special deposits hinder blithely. blithely silent dependencies nag above the slyly regular pinto beans. regular theodolites snooze slyly. final platelets can detect slyly after the regular
130024	25	6493	248.79	e blithely bold deposits haggle slyly silent ideas. quickly silent packages sleep against the blithely regular requests. express,
130025	26	5477	240.69	n pinto beans nag slyly alongside of the regular attainments. slyly pending dependencies haggle carefully special accounts-- even pinto beans cajole after the blithely pending packages. fluffily
130026	27	7044	916.15	uriously after the unusual packages. doggedly regular foxes above the ironic multipliers wake carefully close, dogged asymptotes. dependencies sleep fluffily. daring, even pla
130029	30	3178	819.97	ts. carefully pending accounts above the grouches eat quickly even tithes. packages
130030	31	4776	959.86	uctions sleep carefully. carefully special deposits boost quietly. blithely unusual pinto beans haggle quickly along the furiously ironic pa
130031	32	8390	682.21	ously even dugouts among the special packages sleep above the final, final packages. slyly unusual packages above th
130034	35	3097	276.92	heodolites wake carefully carefully special theodolites. quickly blithe theodolites cajole excuses. quickl
130035	36	8664	740.64	blithely. blithely final deposits after the express packages boost carefully even packages. carefully bol
130039	40	6550	897.43	regular instructions after the final packages affix quickly final deposits. fina
130040	41	330	812.45	 nag slyly against the blithely pending ideas. furiously final platelets nag. blithely final instructions affix even, bold multipliers. 
130041	42	9404	77.40	gular platelets boost among the accounts. pending, bold
130045	46	574	641.51	ily furious dependencies haggle carefully quickly regular platelets. furiously ironic foxes snooze before the blithely regular foxes; slyly regular packages use quickly about the ev
130046	47	8494	453.15	y furiously regular deposits. blithely final deposits sleep quickly fluffily final deposits. blithely regular requests 
130047	48	8976	146.11	f the special, express pearls. quickly bold packages sleep quickly final deposits. quickly final pinto beans boost against the regular packages. regular platelets haggle slyly? fl
130048	49	3313	594.43	s. regular, special deposits wake blithely slyly regular somas. finally express excuses around the quickly ironic requests integrate regular pinto beans. final ideas serve 
132461	1	1206	37.53	 dugouts. even accounts wake according to the furi
132464	4	5146	438.12	y regular platelets haggle above the pending accounts. dep
132466	6	7758	400.27	ake among the blithely bold requests. daringly ironic requests wake. theodolites boost along the furious platelets-- special, final deposits nod after
132467	7	9214	457.77	s sleep carefully around the regular foxes. fluffily enticing pinto beans above the unusual requests maintain furiously around the carefully unusual platelets. fur
132468	8	6926	133.23	 the furiously furious theodolites. epitaphs boost among the instructions. dependencies detect above the blithely bold excuses. regular somas thrash slyly. carefull
132470	10	8882	121.22	yly. carefully regular packages sleep furiously furiously ironic dependencies. express, silent warthogs sleep. quickly final excuses across the decoys nag among the express 
132471	11	4204	304.61	iously ironic accounts cajole carefully on the bold deposits. final
132474	14	2546	773.42	 platelets haggle carefully across the fluffily ironic ideas. bold, even pinto beans cajole. furiously special dependencies cajole. bold sheaves across the thinly final the
132476	16	3300	714.69	ts according to the unusual requests nag blithely fluffily regul
132477	17	7538	252.25	 the fluffily idle ideas sleep carefully after the unusual, unusual accounts. theodolites acro
132478	18	8460	967.06	along the deposits. quickly ironic requests detect. carefully bold accounts across the furiously even packages a
132479	19	4780	389.00	hely pending foxes. silently regular somas should have to poach after the furiously thin requests. special theodolites serve slyly. pending foxes e
132485	25	5382	126.11	regular instructions are blithely slyly final instructions. accounts sleep. silent, regular warhorses wake slyly among the blithely ironic asymptotes. slyly bold deposits use furiously about t
132486	26	7170	684.73	instructions detect furiously within the slyly final instructions. regular packages above the re
132487	27	4328	853.15	s. regular, final deposits detect. slyly regular platelets affix furiously about the pending, express courts. somas dazzle among the slyly unusual i
132490	30	8647	580.64	s. regular gifts are carefully according to the fluffily unusual deposits
132491	31	5836	730.38	cies; furiously regular accounts haggle quickly among th
132492	32	8817	996.64	ncies alongside of the blithely silent excuses wake furiously regular d
132495	35	7470	862.24	le ruthlessly. final theodolites wake quickly? silent accounts boost ironic, regul
132496	36	3381	617.57	 the requests. quickly regular packages could are quickly according to the thinly
132500	40	3349	778.59	. regular, final packages maintain. quickly unusual theodolites mold daringly alongside of the slyly regular requests. slyly final T
132501	41	6381	665.75	kages boost. daringly final theodolites are across the dependencies. slyly quiet asymptotes sleep furiously fu
132502	42	150	250.75	uriously silent packages. slyly silent ideas are. blithely regular requ
132506	46	4093	352.08	equests. bold dependencies are blithely against the permanent accounts. blithely pending accounts according to the foxes boost blithely about the furiously unusua
132507	47	8283	729.80	s snooze. furiously express requests promise furiously even packages. slyly regular foxes ha
132508	48	9526	255.93	wake slyly. furiously even accounts unwind furiously around th
132509	49	2255	807.87	. fluffily pending escapades wake blithely around the instructions. carefully pending accounts use ag
134974	1	7143	463.00	pades poach blithely ironic accounts. regular somas along 
134977	4	5537	380.43	e unusual, express accounts. regular epitaphs snooze blithely after the slyly regular dolphins! foxes sleep furiously quickly regular decoys. slyly even depths are abou
134979	6	4469	328.29	 wake slyly. braids impress fluffily! quickly regular dolphins sleep enticingly quickly regular platelets. carefully regular foxes need to cajole fluffily bold reques
134980	7	1396	825.37	lites. requests along the blithely ironic dolphins cajole carefully slyly bold pinto beans. quickly regular packages across the excuses sleep even pinto beans. furiously unusual courts after the reg
134981	8	4037	29.25	 detect furiously accounts. carefully bold packages sleep furiously. ruthlessly ironic ideas impress. furiously pending accounts cajole blithely slyly final packages. packages along the regular, re
134983	10	1111	597.27	kages use carefully above the slowly unusual deposits! express accounts use quickly. even pinto beans sleep blithely bold foxes. quickly unusual instructions haggle furiously ironic pack
134984	11	8344	267.49	 blithely bold accounts print carefully slyly unusual instructions. regular, express ideas across the final, pendin
134987	14	6443	546.66	ackages haggle. accounts sleep slyly final, bold ideas. unusual, enticing theodolites against the slyly ironic ideas affix deposits. never even instructions are unusual ideas.
134989	16	4953	42.37	the pinto beans. asymptotes haggle above the accounts. slyly express deposits sleep fluffily. slyly ironic packages detect along 
134990	17	9852	870.26	totes. silent dependencies sleep quickly carefully ironic theodolites. silent courts among the ironic dependencies haggle quickly a
134991	18	3516	188.64	egular excuses. blithely final theodolites alongside of the carefully ironic packages x-ray furiously even pinto beans. pinto beans sublate against the carefully
134992	19	3893	520.08	sts. quick, regular pinto beans for the fluffily final packages hinder carefully even asymptotes. ironic, special theodolites hinder furiously. blithely f
134998	25	8879	869.33	 ironic foxes. carefully regular somas above the slyly bold platelets solve above the slyly even deposits. blithely fin
134999	26	3370	201.44	re slyly even, final instructions. carefully even requests sleep blithely. even ideas wake. carefully even foxes cajole carefully. quickly even deposits eat furiousl
135000	27	5364	413.78	unts. even foxes along the quickly unusual grouches cajole furiously slyly pending asymptotes. carefully final instructions are thinly. final deposits along the 
135003	30	8913	336.73	p bravely from the quickly ironic pearls. accounts use furiously above the carefully f
135004	31	3039	456.42	dencies sleep furiously after the blithely regular instruc
135005	32	3418	609.58	the regular, express requests haggle quickly across the pending deposits. ideas affix. furiously final packages use furiously special asymptotes
135008	35	5374	312.44	fully regular accounts. deposits according to the final accounts use around the furiously ironic pinto beans. special acco
135009	36	7542	117.57	dependencies cajole silent, ironic ideas. special platelets are fluffily among the unusual dependencies; blithely final dependencies sleep
135013	40	9750	420.81	heodolites. furiously final packages mold quickly above the furiously regular packages. slyly special deposits boost quickly. requests a
135014	41	8951	560.19	iously. regular, special deposits wake quickly through the furiously ironic pinto beans. slyly unusual theodolites sleep among the blithely silent accounts. carefully final requests alongside o
135015	42	4397	26.21	tect around the furiously regular instructions. regular ideas about the 
135019	46	5307	16.74	nticing frays. regular requests sleep quickly. regular, regular ideas doubt finally among the furio
135020	47	4588	812.82	 are fluffily platelets. slyly express foxes hang. furiously ironic ideas boost blithely. regular, express packages above the excuses impress acco
135021	48	4196	618.83	nce the even excuses. regular deposits affix. instructions wake blithely regular accounts. carefully regular realms haggle enticingly according to the carefully express packages. 
135022	49	6805	220.10	 daringly slyly express asymptotes. blithely even excuses affix slyly. requests haggle quickly after the fluffily ironic accounts. furiously bold platelets boost carefully about the pending, iro
137487	1	9322	647.12	 special accounts use carefully slyly bold pinto beans. slyly final courts sleep blithely. ideas x-ray blithely. 
137490	4	6587	813.19	oze fluffily about the fluffily express deposits. slyly regular dinos cajole furio
137492	6	9786	298.08	gular requests are stealthily regular foxes. doggedly even courts detect about the caref
137493	7	502	433.03	ithely final pinto beans. blithely even instructions cajole brave pinto beans. special pinto beans against the slyly even 
137494	8	5705	374.26	te along the slyly even dependencies. final, regular accounts nag quickly. fluffily even packages eat unusual packages. final 
137496	10	3308	393.62	ithely around the fluffily express courts-- slyly eve
137497	11	2191	803.69	lites-- quickly regular requests cajole furiously across the fluffily even deposits. accounts according to the pending excuses cajole fluffily special, final foxes. carefully ir
137500	14	4686	241.81	ffily finally silent instructions. carefully bold asymptotes cajole about the accounts. unusual patterns na
137502	16	4467	122.57	wake blithely bold accounts. unusual, regular accoun
137503	17	3733	42.35	ronic platelets alongside of the gifts cajole above the slyly pending deposits. quickly regular
137504	18	2989	312.35	iously regular sentiments boost slyly ironic, special foxes. blithely silent accounts sleep final, bold platelets. foxes about the furiously even a
137505	19	652	998.67	e. pending requests boost ironic, regular courts. packages haggle carefully regular accounts; orbits are. furiously bold warhorses nag carefully fluffily
137511	25	9682	147.87	dencies. blithely final packages use ironically idly regular foxes. carefully silent deposits detect against the even courts. carefully ir
137512	26	3776	129.66	ecial platelets are across the requests. unusual, regular accounts nag blithely across the quickly ironic foxes. quickly silent accounts among the pending, final theodolites doubt 
137513	27	9128	221.71	gainst the blithely ironic requests boost slyly with the fluffy deposits. accounts sleep carefully about the quickly regular orbits. special packages wake about the blithely even instructi
137516	30	1646	585.56	ly. unusual, regular requests about the enticing theodolites sleep furiously regular dependencies. carefully regular notornis sleep according to the final, spec
137517	31	3707	379.94	xes: ironic accounts are furiously. accounts across the quickly express foxes use doggedly along the furiously ironic warhorses. slyly even theodolites along the furiously final depo
137518	32	3226	758.97	ys. deposits detect special, special pinto beans. furiously regular accounts are furiously-- unusual, express frays after the regular packages cajole fur
137521	35	3754	287.05	lar accounts. close foxes lose. platelets are alongside of the carefully ironic accounts. dependencies are furiously on 
137522	36	3119	402.80	 requests sleep furiously regular accounts. ironic asymptotes integrate among the slyly final packages. furious p
137526	40	5772	149.11	ccounts solve express packages. blithely pending instructions dazzle slyly even requests. unusual, ironic instructi
137527	41	6332	357.32	s the deposits. even, final asymptotes boost carefully. 
137528	42	4616	203.06	 enticingly careful dependencies doze about the slyly eve
137532	46	4387	622.31	ites. regular packages cajole furiously fluffily ironic requests. blithely final foxes haggle after the final, final requests. bold packages wake quickly above the furiously regu
137533	47	3944	826.86	y. final orbits haggle blithely above the express
137534	48	99	437.99	efully ironic ideas. final, regular pearls sleep alongside of the furiously ironic packages. carefully ironic deposits haggle. fluffily unusual instructions cajole carefully furiously even instruct
137535	49	710	493.05	onic dolphins about the furiously regular excuses thrash furious
140000	1	4339	445.68	ffily careful packages wake finally after the furious
140003	4	3983	399.17	en packages. quickly pending asymptotes engage slyly. quickly special requests integrate above the care
140005	6	960	214.48	inal foxes sleep permanently about the carefully final platelets. dependencies unwind furiously against the blithely final foxes. carefully regular theodolites wake alongside of the f
140006	7	1510	544.78	ar foxes. closely regular requests eat fluffily. final, unusual packages hinder closely after the furiously silent requests. r
140007	8	2613	76.33	uses wake slyly carefully regular packages. regular, special theodolites run pinto beans. quickly final orbits aga
140009	10	5164	630.09	oubt blithely. furiously unusual requests are. carefully sil
140015	16	7834	177.54	thogs. quickly bold requests against the blithely unusual foxes haggle blithely express pinto beans. platelets sleep fluffily to the regular accounts. express, even theodolites wake
140016	17	8211	77.33	 accounts wake blithely quickly ironic accounts. carefully bold sentiments are even requests. slyly ironic pinto beans sleep carefully. deposits are slyly close theodolites. 
140017	18	7720	325.65	 blithely silent theodolites. slyly stealthy deposits sleep. pending pinto bean
140018	19	7658	473.28	the furiously express accounts. blithely thin foxes lose alongside of the express deposits. carefully ironic requests cajole furiously fluffily s
140024	25	5558	331.05	 instructions nod blithely even pinto beans. ironic, regular requests
140025	26	5116	356.93	ccounts eat carefully accounts. slyly even asymptotes haggle carefully. ironic ac
140026	27	8158	966.91	es play after the regular requests. pending ideas haggle daringly. slyly brave dolphins are carefully daring packages. slyly regular accounts wake blithely across the quickly regular ideas. 
140029	30	2822	386.97	lar ideas boost blithely slyly regular warhorses. slyly final requests doze carefully final accounts. slyly unusual foxe
140030	31	719	771.19	eposits. quickly even requests around the quickly special a
140031	32	9722	34.52	y final deposits haggle among the regular platelets; slyly even ideas nag blithely slow braids. slyly ironic gifts cajole slyly quickly express requests. blithely pending depos
140034	35	1703	692.31	pending, idle requests print carefully express pinto beans. silent, ironic excu
140035	36	405	965.49	final theodolites wake carefully along the blithely bold packages. express packages affix carefully final packages. ironic pinto beans slee
140039	40	3136	544.98	beans. frays before the fluffily close accounts nag after the express i
140040	41	7393	570.36	egular accounts detect around the regular pinto beans. unusual deposits against the blithely ironic requests boost a
140041	42	9848	461.60	sits could haggle slyly ironic ideas. unusual, bold platelets above the blithely bold packages s
140045	46	1817	368.18	ses nag about the slyly final foxes. final ideas haggle! furiously bold accounts unwind. packages haggle blithely slyly unusual ideas. quickly express instructions
140046	47	5644	501.07	onic accounts are along the platelets. furiously pending deposits after the bold theodolites 
140047	48	4036	664.32	are carefully unusual accounts. blithely ironic excuses are-- express deposits after the fu
140048	49	845	833.30	ing requests breach blithely against the ideas. slyly ironic excuses according to the regular instructions haggle furiously along the
142458	1	6150	973.09	ending theodolites. slyly even instructions after the furiously regular packages nag blithely spec
142461	4	2072	831.13	ronic warthogs. even requests detect blithely. slyly ex
142463	6	1957	420.80	nag. carefully regular requests maintain slyly among the furiously pending asymptotes. furiously regular accounts after the quickly regular requests x-ray carefully furiously pending asymp
142464	7	3504	912.41	ely special packages cajole. carefully even patterns haggle. blithely regular accounts sleep. ironic packages abov
142465	8	5669	611.85	ss the slyly unusual accounts sleep blithely silent pinto beans. slyly ironic platelets abo
142467	10	8750	717.48	 about the slyly pending instructions sleep doggedly bold, special accounts. quickly bold deposits sleep blithely. ruthless req
142468	11	3361	598.16	 deposits impress. ironic, ironic theodolites wake carefully after the ironic accounts. quickly final dolphins detect carefully am
142471	14	3970	288.79	t! regular excuses detect quickly alongside of the requests. pending instructions sleep. requests sle
142473	16	728	575.38	r accounts sleep among the final hockey players. fluffily regular dependencies nag quickly among the even, regular excuses
142474	17	8036	885.29	uests between the bold dinos haggle express instructions. final theodolites boost abo
142475	18	8383	291.74	g the ironic instructions. regular requests are daringly slyly final ideas. express, silent grouches wake quickly pending dependencies! carefully fin
142476	19	9512	214.80	uriously. requests mold. silently ironic warhorses after the requests cajole daringly fluffily regular excuses. slyly special pinto beans are slyly-- blithely ironic requests
142482	25	517	557.70	l packages wake slyly. furiously pending requests might cajole slyly along the blithely even accounts. blithely special courts haggle slyly ironic deposits. furiously express the
142483	26	4775	500.51	uctions boost slyly deposits. carefully final platelets use slyly around the ironic, ironic deposits. final accounts integrate across the ironic pinto beans. regular, even
142484	27	4567	380.29	 the quickly final ideas sublate furiously closely ironic frays. blithely even requests nod furiously. slyly unusual depende
142487	30	4791	869.97	efully pending packages. pending, final packages wake bold foxes. furiously regular warhorses are quickly against the carefully final asymp
142488	31	949	23.62	y along the silent deposits. even foxes was slyly special requests. slyly daring requests affix ruthles
142489	32	8724	512.61	e carefully even instructions. furiously ironic deposits after the slyly final deposits are slyly across the fluffily regular accounts. blithely thin excuses kindle furiously requests. r
142492	35	4430	684.23	le slyly from the packages. slyly even deposits cajole quickly whithout the furiously 
142504	47	4380	442.68	xes along the furiously even ideas cajole against the quickly silent dependencies. special ideas wake 
142493	36	2565	461.65	 platelets mold quickly. final instructions nag. special ideas among the slyly final foxes are above the asymptotes. dependencies nod. carefully ironic foxes nag fluffily. pending deposi
142497	40	6027	643.16	furiously after the ironic dolphins. slyly ironic pearls boost carefully. carefully unusual epitaphs wake sly
142498	41	9642	584.41	bold ideas. pinto beans run. carefully pending instructions maintain slyly across the regular instructions. final accoun
142499	42	3292	729.92	ely permanent deposits after the carefully regular courts s
142503	46	391	728.69	unts haggle stealthily regular requests. slyly special epitaphs about the boldly ironic dependencies try to integrate after the furiously ironic deposits. furiously regular foxes are blithely f
142505	48	7176	296.99	g the even, regular deposits. permanently regular gifts nag never. boldly pending theodolites should use above the even, regular packages. unusual ideas are across the pending, regular accounts. 
142506	49	2261	703.80	iously bold requests sleep even dependencies. deposits maintain slyly. carefully pending platelets along the pending dolphins was along the blithely ironic braids! furiously
144972	1	8418	324.08	y final notornis-- pinto beans are quickly after the ironic, regular pinto beans. ironic deposits use blithely courts. ruthless theodolites print sl
144975	4	9183	649.40	 the slyly express foxes use slyly after the pending packages! carefully regular packages wake carefully after 
144977	6	5962	895.30	lithely above the furiously ironic deposits. final asymptotes around the unusual dependencies are acros
144978	7	8386	437.28	 promise into the carefully pending instructions. blithely final asymptotes haggle furiously final instructions. 
144979	8	447	28.89	even accounts. fluffily furious pinto beans wake slyly even requests! accounts lose foxes. blithely ironic requests nag alongside of the pending requests. pa
144981	10	8638	398.81	 ideas. bold requests sleep silently accounts. excuses detect quickly slyly special requests: pending accounts cajole blithely. slyly final pinto be
144982	11	4723	302.04	yly express accounts wake permanently according to the regular warhorses. carefully unusual theodolites are slyly after 
144985	14	2478	645.15	 pending instructions along the accounts sleep silently after the furiously express deposits. re
144987	16	5213	185.01	the furiously regular platelets. blithely regular
144988	17	5262	923.44	riously. requests cajole furiously furiously unusual frets. regular, special accounts near the furiously silent excuses boost carefully above the bold, pending dolphins
144989	18	136	578.32	al instructions. special requests play. quickly silent dolphins 
144990	19	7851	600.85	ironic packages wake. idle, unusual requests do affix slyly across the slyly express excuses
144996	25	8188	498.36	s are fluffily blithely unusual somas. slyly even requests haggle blithely. regular theodolites wake. quick accounts affix furi
144997	26	1979	723.71	 solve blithely furiously regular platelets! platelets about the regular, ironic dependencies detect slyly theodolites. final, even deposits 
144998	27	7867	940.99	e furiously final ideas wake carefully among the carefully busy deposits. thinly unusual packages wake silen
145001	30	5685	883.67	ole blithely. carefully ironic deposits sleep carefully after the blithely final theodolites? pending packages haggle blithely furiously
145002	31	7120	102.00	ously. dolphins affix slyly about the unusual requests. packages sleep slyly blithely enticing somas. ironic theodolites about the slyly ironic accounts integrate slyly quickly even theo
145003	32	7445	317.68	ptotes boost fluffily along the asymptotes. enticing accounts haggle boldly pending requests. slyly bold instructions alongside of the carefully e
145006	35	8324	798.25	lways. regular packages haggle blithely after the b
145007	36	1158	247.47	blithely quickly unusual deposits. even packages sleep slyly
145011	40	726	885.28	 final dependencies wake slyly furiously unusual theodolites; pending accounts haggle carefu
145012	41	5832	403.95	pades. slyly regular asymptotes would cajole behind the quickl
145013	42	7039	503.96	lar requests at the furiously even courts try to wake alongside of the slyly final forges. silent, unusual pearls cajole. bold, ironic dependencies use fluffily pinto bean
145017	46	3132	495.62	ter the carefully final asymptotes sleep among the special
145018	47	9215	884.01	alongside of the ironic deposits are regular platelets. regular instructions ar
145019	48	4809	763.71	lyly regular requests run slyly. accounts sleep furious
145020	49	9328	365.15	lyly pending, final instructions. final pinto beans across the package
147486	1	736	90.30	gular deposits nag. blithely bold accounts among the regular deposits wake according to 
147489	4	656	78.12	 packages haggle blithely. quickly express theodolites are along the slyly final accounts. carefully pending dependencies nag quickly around the account
147491	6	8006	297.64	ckly. special packages above the even deposits haggle after the requests. quickly silent requests run quickly toward the regular foxes. slyly dogged multipliers cajole silent requests. carefully 
147492	7	4589	545.14	 grow fluffily. bold dependencies use furiously express platelets. quickly regular multipliers cajole quickly. quickly final depths nag fluffily. blithely final accounts are final pinto beans! bold,
147493	8	1269	61.68	thely even accounts. ironic requests engage slyly according to the blithely even instructions. even deposits unwind quickly. final, express in
147495	10	9645	537.95	ounts thrash slyly among the furiously even accounts. furiously unusual deposits thrash carefully c
147496	11	3359	418.58	 pearls nag quickly regular, ruthless instructions. special pinto beans cajole along the slyly re
147499	14	452	438.86	refully alongside of the slyly ironic theodolites-- quickly enticing asymptotes through the furious
147501	16	5985	333.00	 carefully express deposits. carefully even foxes are. furiously even packages integrate around the slyly unusual theodolites. unusual courts cajole carefully silently even pinto
147502	17	237	932.56	sleep carefully. slyly even packages around the regular packages nag carefully platelets. furiously special theodolites was blithely; finally regular pearls kindle blithely? quickly pend
147503	18	8709	415.82	ill have to affix about the silently ironic packages. slyly pending deposits nag. ironic accounts haggle carefully blithely regular deposits. blithely regular dependencies use. packages
147504	19	147	724.02	tructions will have to hang finally according to the ironic, final requests. even, regular decoys sleep blithely. furiously special accounts sleep after th
147510	25	7312	748.74	ages. furiously final deposits across the carefully unusual packages are against the slyly even foxes. fu
147511	26	5509	888.33	 slyly special accounts! slyly regular platelets nag after the slyly express excuses. bold warhorses was furiously. regular requests wake doggedly theodolites. furiously unusual req
147512	27	8817	200.80	s cajole furiously according to the express requests! furiously bold instructions x-ray blithely regular somas. blithely silent pinto beans grow furiously according to the carefully regular
147515	30	459	955.45	eans alongside of the carefully regular dolphins are according to the deposits. blithely regular packages against the express orbits breach carefully blithely regular
147516	31	3794	116.93	usly pending packages. slyly bold courts play furiously around the furiously unusual excuses; even packages are furiously over the fluffily final deposits. slyly final pac
147517	32	5850	540.27	s doze quickly above the furiously even asymptotes. slyly regular ideas cajole. quickly regular forges must cajole above the deposits. furiously bold packages haggle quickly except the quickl
147520	35	2643	729.65	as cajole furiously pending deposits. even, ironic pearls boost foxes. slyly special pains nag furiously across the quickly regular pi
147521	36	4879	93.67	pendencies. even, ironic requests are slyly furiously special pinto beans. unusual foxes hang ruth
147525	40	5140	375.39	lyly furiously regular requests. dogged dependencies are blithely special theodolites. furiously ir
147526	41	2721	328.37	 slyly even instructions along the quickly pending accounts use slyly according to 
147527	42	9026	20.62	 final grouches haggle on the pending accounts. quickly pending deposits acc
147531	46	1593	887.81	oss the ideas cajole furiously unusual deposits. final packages wake blithely. final platelets play express deposits! blithely ev
147532	47	3712	2.28	sly. slyly pending requests sleep packages. furiously ironic requests wake furiously against the bold ideas. regular deposits wake quickly fluffily ironic re
147533	48	4616	806.47	eodolites among the final theodolites wake even asymptotes. quickly final theodolites boost according to the furiously final package
147534	49	9832	514.02	ely unusual ideas. asymptotes cajole about the regular instructions. unusual requests cajole carefully unusual requests. pinto beans cajole pinto beans. special, unusual deposits wake 
150000	1	6975	325.03	enly final theodolites haggle slyly bold, special deposits. regular fox
150003	4	2768	707.77	ously regular packages around the final theodolites nag fluffily among the blithely even deposits; regul
150005	6	4873	423.62	g above the slyly even excuses. accounts before the regular packages promise permanently regular multipliers. instructio
150006	7	3680	625.51	y. blithely regular requests sleep carefully pending platelets. ironic packages detect blithely along the quickly bold dep
150007	8	1640	275.88	arefully. slyly even requests wake. regular deposits alongside of the furiously silent forges play furiously blithely bold packages. carefully pending platelets 
150009	10	1347	512.20	t attainments according to the ironic accounts detect blithely blithely pending theodolites. quickly final dolphins grow quickly slyly final requests
150010	11	8976	974.25	p. express, ironic pinto beans wake slyly slyly pending pinto beans. pending deposits wake across the even foxes. care
150013	14	2434	222.30	deposits. slyly final foxes integrate slyly about the final pinto beans.
150015	16	5156	714.70	lyly regular accounts will detect blithely. instructions wake along the closely express theodolites. bold excuses haggle furi
150016	17	3027	425.14	nic accounts use furiously; carefully ironic asymptotes atop the ironic, pending foxes cajole alongside of the regular platelets. ev
150017	18	9469	57.28	ronic foxes. fluffily ironic requests among the accounts nag slyly against the unusu
150018	19	2269	396.97	 bold, special requests boost slyly. carefully ironic packages use furiously. idly ironic excuses above th
150024	25	7245	309.00	ully ironic requests boost furiously. furiously ironic pinto beans x-ray carefully fluffily stealthy requests. carefully r
150025	26	6412	160.35	e slyly. slyly special theodolites among the carefully ironic courts nag quickly bold, quick foxes. blithely express requests haggle packages. slyly even courts integrate about the b
150026	27	3235	177.36	 theodolites alongside of the quickly even packages boost among the blithely regular accounts. packages cajole slyly. ideas are after the express, even deposits. furiously pending dolphins nag f
150029	30	5387	989.39	 beans boost boldly pending deposits. blithely regular request
150030	31	8197	807.55	cross the furiously ironic requests. ironic requests cajole quic
150031	32	1169	370.45	d deposits wake according to the idle instructions; silently ironic theodolites poach above the carefully ironic accounts. slyly express accounts across th
150034	35	7994	599.47	ts dazzle. fluffy pinto beans are around the furiously ironic instructions. carefully ironic accounts above the slyly final platelets integrate furiousl
150035	36	5846	946.98	ep blithely bold excuses. furiously ironic foxes wake blithely alongside of the carefully regular packages. quickly even accounts after the pending requests sle
150039	40	3670	642.23	ss, regular deposits hinder quickly past the ironically ironic deposits. fluffily slow requests along the unusual accounts nag boldly slyly careful id
150040	41	5257	609.13	sits. quickly ironic dependencies sleep quickly. furiously ironic deposits sleep among the blithely express requests. furiously regular accounts poac
150041	42	2524	186.50	ages. blithely even packages haggle according to the blithely ironic pinto beans. asymptotes integrate blithely among the furiously busy deposits. bold reque
150045	46	7775	609.40	slyly final accounts. fluffily silent requests wake blithely blithely ironic packages. pending 
150046	47	9571	721.97	ld courts. doggedly final packages solve. furiously unusual accounts above the courts sleep against the final, ironic
150047	48	485	274.20	 instructions. close packages according to the quickly special accounts are deposits. theodolites nag. fluffily bold accounts do are furiously. furiously ex
150048	49	9889	267.22	the blithely express pinto beans. bold, final instructions impress idly pending requests? carefully quick requests boost slyly. blithely 
152455	1	5024	873.92	ly special, ironic deposits. blithely even deposits snooze special instructions. carefully even theodolites play qui
152458	4	8124	554.02	y. fluffily final ideas are carefully ironic dependencies. pinto beans use furiously bold, bold instructions. slyly close deposi
152460	6	4447	654.01	. carefully regular warthogs haggle blithely slyl
152461	7	1307	943.74	leep furiously. furiously pending excuses cajole final, ironic dependencies. fluffily pending theodolites 
152462	8	8344	686.30	ly after the deposits. asymptotes affix silently. final foxes integra
152464	10	8016	241.12	e. regular accounts boost along the permanently special dolphins. enticing decoys poach. slyly even foxes after the quickly fina
152465	11	7531	908.87	 regular instructions according to the final gifts are against the cour
152468	14	3758	841.05	y express accounts. instructions across the furiously regular ideas haggle carefully accord
152470	16	8740	748.87	y ironic platelets boost carefully. carefully final pearls against the special, quiet dependencies sleep against the even, regular requests. bold packages
152472	18	9102	308.02	regular notornis detect slyly express, pending accounts. pending ideas cajole permanently carefully regular accounts. furiously pending waters affix. bold, even inst
152473	19	9310	895.12	oxes use slyly unusual requests. furiously even instructions serve around the slyly ruthless courts. silent, final accounts against the slyly regular deposits nag slyly slyly express deposits
152479	25	522	911.16	 pinto beans boost slyly into the quickly bold packages-- dependencies are blith
152480	26	2239	403.58	y. slyly express deposits cajole slyly across the slyly pending
152481	27	7966	98.21	slyly express foxes. quickly bold asymptotes boost. regular, bold deposits cajole slyly. furiously special c
152484	30	3817	508.04	y. unusual accounts beyond the slyly ironic packages
152485	31	5052	495.04	stealthy requests maintain blithely slyly special dolphins. regular, bold packages cajole. slyly regular pinto beans cajole carefully against the ideas. express excuses are furiously quickly silent
152486	32	3779	429.25	cuses. unusual, bold requests above the furiously regular pinto beans d
152489	35	3753	479.76	ccounts boost. slyly silent accounts are blithely. blithely bold pinto beans boost. slyly bold ideas wake carefully around the even packages. carefully ironic pinto beans cajole quickly af
152490	36	6227	880.28	quests after the slyly ironic requests nag carefully regular packages. furiously ironic 
152494	40	3177	652.29	lithely regular sentiments sleep slyly after the quickly regular requests. slyly even asymptotes use furiously by the carefully final deposits. accounts sleep slyly regular p
152495	41	3580	440.89	y carefully silent requests! furiously unusual deposits integrate carefully alo
152496	42	9796	263.26	s are requests. even instructions use around the blithely ironic frets. regular braids use furiously. slyly final ideas cajole quietly pinto beans. quickly special requests cajole blithely regul
152500	46	4680	13.23	ckages are across the regular, special excuses. express, regular ideas cajole fluffily regular requests. care
152501	47	7909	306.73	nal requests haggle slyly among the quickly final accounts. express requests cajole daringly. 
152502	48	1393	540.05	hely express theodolites use blithely. blithely bold theodolites doubt. fluffily final requests haggle fluffily regu
152503	49	2179	483.64	oxes are around the slyly final platelets. final packages wake slyly alongside of the even, unusual packages. even packages
154970	1	5238	907.83	 the ironic braids. furiously regular attainments detect notornis. furiously regular instructions above the carefully pending platelets cajole furiously unu
154973	4	8119	687.52	ges. fluffily regular packages are. final foxes across the 
154975	6	4284	229.52	ld courts sleep quickly among the special dependencies. blithely special theodolites cajole furiously. carefully careful accounts hinder. slyly silent 
154976	7	9200	452.08	eans. regular, ironic deposits nag. furiously quiet
154977	8	8973	489.55	all sleep carefully above the platelets. warthogs must wake regular instructions. sentiments wake blithely furiously permanent ideas. furiously bold dolp
154979	10	7265	173.73	blithely regular accounts was blithely furiously ironic theodolites. pending 
154980	11	9570	700.98	s dazzle. always bold accounts cajole according to the silent accounts; furiously regular instructions nag carefully regular accounts. regular, 
154983	14	5556	490.25	al deposits. deposits boost. ironic accounts haggle among the slyly ruthless theodolites. special, regular asymptotes along the quickly even instructions sl
154985	16	1286	46.45	mong the express instructions. carefully regular theodolites wake furiously re
154986	17	9187	770.31	riously. regular, unusual packages haggle furiously. unusual, regular deposits affix according to the regular instructions. slow pinto beans use. furiously final requests sleep along the final, ir
154987	18	9344	888.14	nts wake slyly pending pinto beans. braids sleep carefully requests. furiously fluffy pinto
154988	19	5172	589.83	s sleep fluffily. blithely even pinto beans against the ironic, even attainments cajole slyly ironic requests. quickly close tithes sleep slyly. final accounts haggle across the express, ironic 
154994	25	8519	782.10	ffy deposits are furiously against the final dugouts. blithely ironic dependencies at the deposits use according to the bold,
154995	26	4873	380.72	ar, regular deposits detect furiously: blithely special accounts nag. carefully ironic packages are pending pinto beans. slyly even accounts nag blithely. quickly regula
154996	27	3235	967.28	ound the regular packages. regular requests integrate enticing theodolites. ironic packages cajole furiously. furiously even accounts cajole slyly. even pinto beans boost bl
154999	30	9530	415.16	s sleep blithely furiously unusual packages. slyly ironic deposits sleep slyly. dolphins according to the carefully special packages sleep blithely alongside of the bold, regular the
155000	31	528	875.16	oxes might detect according to the requests: ironic requests a
155001	32	7169	431.75	ularly brave requests are carefully. carefully regular excuses against
155004	35	3955	378.92	yly carefully even foxes. carefully regular excuses according to the even ideas sleep quietly against the slyly express
155005	36	5919	83.49	s affix carefully slyly regular requests. daring accounts among the carefully final package
155009	40	3273	659.47	sts sleep. accounts nag above the regular ideas. carefully pending accounts cajole furiously. accounts haggle fur
155010	41	1012	915.18	requests. blithely final foxes ought to haggle pinto beans. even instructions sleep blith
155011	42	848	502.28	 ironic accounts. express requests nag after the regular, ironic packages. platelets cajole carefully even ideas. quickly special packa
155015	46	3831	482.61	egular, ironic ideas wake across the regular, unusual accounts. bold accounts sleep boldly against the sometime
155016	47	2245	342.52	luffily blithely furious accounts. dogged, regular accounts nag alongside of the quickly regular accounts.
155017	48	9433	949.97	y above the permanently pending foxes. furiously regular requests are blithely quickly special pinto beans? quickly express platelets sleep alongside o
155018	49	5975	571.77	 deposits hinder furiously special deposits. furiously special foxes are furiously regular instructions. dependencies after the ironic deposits boost quickly dogged instructions. ir
157485	1	6042	690.77	use. bold, final deposits sleep. even requests cajole above the furiously regular instructions. carefully even deposits cajole
157488	4	5355	57.98	ts wake quickly about the theodolites. excuses sleep against the pac
157490	6	1226	228.32	ending packages. silent theodolites integrate special, regular deposits. ruthless excuses haggle carefully 
157491	7	7309	767.39	y regular warhorses haggle slyly quickly ironic instructions. even requests cajole bold instructions-- fluffily final decoys use quickly. pending, final theodolites among the final ideas affix qui
157492	8	5372	190.40	ke after the carefully even deposits. carefully unusual
157494	10	9185	797.59	es nag. unusual warthogs are furiously. finally regular platelets cajole fluffily. blithely even dinos detect blithely alongside of the pen
157495	11	5923	978.22	nusual, ironic packages! carefully regular instructions haggle silently regular dependencies. blithely silent pinto beans are according to the furiously regular foxes. slyly r
157498	14	4369	884.23	unts across the quickly regular dolphins haggle blithely quickly regular instructions. fluffily even packages use
157500	16	9528	349.74	lets wake quickly daringly special platelets. quick requests poach fluffily packages! ruthlessly unusual depo
157501	17	9113	306.90	requests cajole slyly according to the quickly ironic requests. pending requests use slyly after the slyly unusual requests. theodolites detec
157502	18	6175	349.06	inal ideas around the slyly even packages cajole despite the pending, regular instructions: quickly express warthogs cajole quickly quickly final requests. even a
157503	19	9504	532.57	packages nag above the permanently brave theodolites-- blithely sp
157509	25	5369	866.97	sleep above the carefully unusual braids. theodolites are blithely above the ideas. final, silent deposits cajole carefully. fluffily ironic dependencies b
157510	26	7887	80.66	osits. furiously regular instructions wake. requests detect slyly along the regular accounts. furiously ironic accounts affix even accounts! packages boost fluffily. bl
157511	27	3920	500.72	even dependencies serve slyly. furiously ironic depths are quickly even packages. express pinto beans sleep. requests with the acc
157514	30	9372	267.95	 about the thin requests sublate above the unusual, express packages. blithely pending ideas haggle? quickly ironic somas eat after the depos
157515	31	1885	151.57	usual packages wake slyly. slyly unusual asymptotes cajole stealthily
157516	32	3559	491.29	ular instructions. regular, ironic requests are above the ironically final deposits. furiously bold requests after the carefully ironic dependencies haggle fluffily bold,
157519	35	8900	810.35	l asymptotes cajole carefully blithely final requests. carefully unusual deposits are fluffily carefully special warthogs? quickly final deposits haggle carefully. slyly dogged foxes thras
157520	36	3418	498.70	ronic deposits. quickly regular Tiresias haggle car
157524	40	3063	276.82	 wake carefully along the furiously ironic requests. carefully regular pinto beans cajole furiously patterns
157525	41	6159	646.06	o the regular pinto beans integrate after the doggedly express warhorses. regular foxes haggle above the even, bold pinto beans. platelets use instructions. regular as
157526	42	3563	651.46	tions lose furiously across the slyly unusual pinto beans. blithely ironic sentiments use furiously regular deposits. furiously ironic
157530	46	3620	599.46	ies nag furiously. quickly busy depths wake carefully along the
157531	47	8013	838.98	ackages are about the blithely final ideas. enticingly final requests integrate carefully across the slyly silent theodolites. fluffy instructions maintain. carefully pending packages promise regul
157532	48	2734	749.40	 packages would use slyly blithely even pinto beans. furiously permanent requests nag furiously stealthy requests. fluffily ironic accounts wake carefull
157533	49	4451	109.53	nstructions cajole above the ironic pinto beans. regularly final deposits sleep fluffily ironic packages; blithely silent deposits wake bl
160000	1	5962	639.61	e idly ironic courts. ideas sleep furiously silent,
160003	4	9520	872.93	ts haggle carefully. regular accounts along the quickly ironic pinto beans dazzle furiously after the slyly special dependencies. furiously final deposits among the 
160005	6	4895	890.95	sual accounts. blithely special deposits boost slyly around the blithely bold theodolites. furiously final ideas run again
160006	7	5213	43.44	egular accounts. slyly ironic dependencies wake. fluffily slow orbits according 
160007	8	1991	468.03	eposits. furiously special excuses sleep quickly about the quickly regular ideas. regular pinto be
160009	10	4893	641.64	ts cajole after the slyly pending theodolites. regular, final asy
160010	11	9539	997.56	s. regular asymptotes use fluffily blithely express pinto beans. slyly final deposits cajole blithely
160013	14	622	516.66	ing foxes maintain fluffily quickly ironic foxes: carefully even requests promise. even theodolites haggle
160015	16	9930	750.64	fluffily pending realms. regular accounts boost carefully above the carefully pending epitaphs-- bold foxes thrash slyly carefully bold accounts. slyly special instructions accordi
160016	17	3924	495.72	final accounts. special, ironic instructions haggle carefully about the unusu
160017	18	6887	115.32	al, express courts. slyly bold theodolites wake slyly furiously even ideas. carefully ironic 
160018	19	4086	208.72	onic theodolites are fluffily about the silent requests. bold foxes a
160024	25	7148	108.48	ironic foxes. silently ironic packages cajole along the silent deposits; ironic deposits above the quickly regular ideas wake slyly after the accounts. ironic packages cajole. final d
160025	26	7229	962.03	uriously silent decoys boost furiously above the blithely final gifts. furiously special warthogs are fluffily according to the carefully even theodolites. silent somas cajole 
160026	27	3707	321.07	ng requests: fluffily ironic accounts according to the final notornis nag carefully final, pending dolphins. blithely ironic pinto beans detect theodolites. quic
160029	30	4464	216.59	fully above the furiously ironic asymptotes. final packages sleep slyly. regular accounts x-ray slyly blithely final platelets. furiously regular accounts across the slyly fi
160030	31	7246	160.72	usly against the express, regular accounts. ironic packages wake car
160031	32	423	728.33	 sleep evenly express packages. slyly special packages boost slyly slyly
160034	35	6712	334.92	deposits serve slyly? blithely final accounts maintain furiously slyly ironic ideas. carefully pending deposits are! bold, e
160035	36	7795	502.81	 final, even requests wake furiously unusual dependencies. quickly express accounts about the sly
164985	18	7882	835.94	s. carefully bold pinto beans with the furiously bold somas are furiously carefully ironic pinto beans. silent platelets for the slyly
160039	40	724	768.14	lithely final deposits. blithely ironic requests cajole accounts. final theodolites integrate slyly bold frets. fluffily slow dependencies affix quickly. slyly final excuses 
160040	41	7382	878.39	ainst the furiously express theodolites nag fluffily regu
160041	42	6665	791.76	integrate fluffily. blithely final deposits haggle. carefully sp
160045	46	1913	174.54	hely enticing pinto beans. special platelets sleep across the fluffy, unusual packages. furiously special dolp
160046	47	1265	36.55	across the bold, special pinto beans. furiously silent dependencies integrate slyly. blithely regular dependencies haggle along the even, final foxes.
160047	48	1991	700.06	unts are deposits. furiously even packages play. express, regular instructions sleep ironic requests. furiously busy theodol
160048	49	6283	436.15	 the carefully express platelets. furiously regular instructions wake fluffily after the final deposits. unusual, regular pinto beans a
162452	1	5489	56.95	 wake furiously slyly regular accounts. slyly final ideas cajole quickly. furiously final warthogs along t
162455	4	7920	620.52	blithely bold deposits lose alongside of the regular, unusual deposits. carefully ironic dept
162457	6	1934	986.77	y against the blithely even requests. deposits breach across the fluffily ruthless accounts. carefully sly ideas wake slyly packages. fluffily 
162458	7	4730	629.38	ed requests cajole blithely across the slyly final accounts. regular courts around the packages use slyly regular foxes. pinto beans boost furiously according to the pending, even excuses. ideas h
162459	8	6183	487.79	into beans. quickly unusual accounts are furiously ironic excuses. slyly express dependencies boost according to the carefully ironic accounts. sl
162461	10	7081	799.79	 accounts. pending courts nod furiously until the slyly unusual packages. bold, express excuses cajole against the slyly ironi
162462	11	7946	725.24	ainst the regular ideas. final, ironic excuses past the even, bold accounts boost fluffily fluffily even instructions. blithely ironic warhorses above the slyly regular accounts doubt quickly
162465	14	5022	370.53	ly slyly pending requests. blithely express requests use furiously across the even ideas. carefully silent deposits nag never slyly final deposits. furiously sp
162467	16	773	296.00	 ironic courts. unusual, quiet requests sleep slyly even requests. final requests haggle furiously slyly bold requests. blithely ironic accounts c
162468	17	5066	277.69	ounts along the packages nag final requests. regular platelets snooze regular deposits. quickly final frets wake blithely at the carefully
162469	18	6496	8.82	kages? final packages kindle. waters sleep carefully brave deposits. regular, final requests cajole furiously around the final dep
162470	19	4937	866.86	t the blithely regular accounts. carefully special pinto beans sleep blithely along the unusual accounts. ironic ideas 
162476	25	2195	362.56	ly pending instructions are furiously express deposits. carefully pending packages about the unusual, even theodolites are carefully unusual theodolites. quietly pending
162477	26	1344	395.50	he quickly regular foxes. fluffily final instructions integrate regular foxes. slyly regular foxes alongside
162478	27	2419	716.66	 blithely regular pinto beans? slyly bold deposits cajole flu
162481	30	6552	641.13	 regular packages are blithely along the carefully special gifts. quickly final deposits unwind slyly among the blithely regular warhorses-- quickly
162482	31	4381	456.92	ould wake pinto beans. silent packages wake furiously against the carefully pending dependencies. excuses boost fu
162483	32	8171	361.59	 the unusual deposits. quickly unusual instructions after the carefully pending theodolites sleep slyly about the carefully ironic 
162486	35	8694	363.56	asymptotes lose slyly above the slyly regular pains. furiously ironic asymptotes whithout the carefully ironic deposits maintain after the bold, regular theodolites. sometimes
162487	36	1903	648.92	thely ironic deposits. regular, ironic courts use blithely deposits. silent theodolites na
162491	40	4927	374.68	regular requests use furiously foxes. theodolites according to the ironic, even dolphins breach along the final pinto beans. slyly regular dugouts nag quickly grouches. slyly bold de
162492	41	6208	562.64	ymptotes about the carefully ironic instructions caj
162493	42	948	541.69	ly special theodolites. fluffily even ideas after the quickly regular requests nag carefully fluffily silent packages. express theodolites above the carefully bold platelets lose around th
162497	46	3753	221.00	uests? pending deposits sleep carefully asymptotes. carefully regular platelets play foxes. careful
162498	47	1927	962.45	 after the furiously bold requests; closely regular b
162499	48	5507	261.27	arefully regular deposits sleep quickly. busy theodolites sleep along
162500	49	2081	270.79	ss pinto beans sleep furiously; final deposits alongside of the instructions serve blithely regularly even dependencies. furiously unusual accounts nag. deposits affix furi
164968	1	3993	599.78	hely among the bold dependencies. furiously pending requests hang blithely! ironic, express foxes across the slyly final r
164971	4	4862	618.42	ideas? fluffily unusual foxes nag slyly after the fluffily final theodolites. even packages integrate fluffily across the even requests. fu
164973	6	1302	886.44	 ironic ideas wake slyly thinly bold requests. blithely silent deposits mold blithely. furiously final dinos run furiously q
164974	7	4402	595.56	 slyly bold requests cajole quickly. pending packages sleep fluffily. regular packages wake quickly courts. furiously bol
164975	8	1534	496.83	s. pending platelets above the carefully pending platelets are blithely quickly close requests. furiously unusual theodolites affix carefully quickly final pac
164977	10	6323	366.39	ckages boost blithely slyly ironic ideas: slyly even requests was bravely along the slyly even deposits-- furiously silent packages haggl
164978	11	9588	777.92	pinto beans cajole slyly sauternes. carefully regular requests sleep slyly according to the deposits. furi
164981	14	5473	147.62	 wake closely among the blithely ironic sheaves. 
164983	16	7526	324.49	into beans. ironic ideas eat slyly. regular packages maintain furiously. furiously silent dinos are furiously carefully final foxes. final deposits haggle silently bold 
164984	17	4973	184.90	ing to the quickly regular packages. fluffily ironic excuses haggle among the accounts. express asymptotes cajole slyly above the furiously ironic platelets. foxes thrash after the bl
170006	7	7435	256.29	ing requests haggle carefully across the regular pinto beans. carefully express instructions detect blithe
164986	19	9684	546.08	al accounts after the pending, ironic deposits cajole above the blithely even requests-- regular, pending deposits wake care
164992	25	1553	55.87	odolites maintain enticingly. fluffily regular foxes wake blithely besid
164993	26	4994	679.23	ar, regular instructions wake carefully courts. ironic, pending depen
164994	27	7042	133.90	y even packages wake-- furiously final dolphins affix against the fluffily bold asymptotes. dependencies haggle. bold, express dependencies sleep carefully final foxes. furiou
164997	30	4266	832.97	 furiously final deposits. express requests integrate fl
164998	31	7153	448.95	arefully regular accounts. slyly bold instructions according to the furiously sly excuses wake blithely quickly bold packages. requests lose fluffily regular sauternes. blithely spe
164999	32	5773	494.95	 furiously unusual packages. carefully bold ideas haggle. furiously regular deposits across the final, pending requests boost furiously bold packages. accounts nag slyly slyl
165002	35	3120	643.64	e quickly express theodolites. quickly pending theodolites according to the carefully final ideas are carefully special instructions. furious
165003	36	5896	499.23	haggle quickly furiously regular accounts. brave, daring asymptotes around the carefully 
165007	40	64	929.04	 instructions. fluffily ironic instructions use slyly. ironic, even pearls cajole alo
165008	41	5572	444.26	slyly slyly regular deposits. furiously pending accounts around the quic
165009	42	7431	643.08	ic orbits integrate furiously. pending, special gifts are slyly after the regular excuses. furiously bold accounts wake quickly pending deposits. bol
165013	46	6386	30.94	 detect regular theodolites. regular dolphins after the carefully
165014	47	1003	563.67	 blithely fluffily regular packages. furiously regular instructions are above the slyly final deposits. blithely regular packages about the bold pinto beans nag alongside of the careful
165015	48	4327	414.08	ages affix finally. furiously regular foxes wake furiously regular, pending ideas. d
165016	49	1588	641.99	ealthy, special accounts across the final accounts cajole quickly along the quickly ironic platelets. slyly regular dependencies between the carefully bo
167484	1	9514	50.03	ly regular pinto beans sleep slyly. blithely final pinto beans haggle. unusual requests are across the fluffily final packages
167487	4	4421	864.31	ly regular ideas sleep. pinto beans integrate blithely bold excuses. even pinto beans haggle above the ironic accounts. final a
167489	6	3255	439.73	s haggle across the dolphins. special, special requests haggle dogg
167490	7	3706	533.94	usly bold accounts use slowly among the blithely final requests. slyly express accounts are slyly slyly final pinto beans. e
167491	8	4990	213.20	gular excuses. foxes cajole after the express tithes. furiously even dolphins eat carefully. pinto beans cajole carefully. carefully blithe requests use quickly slyly final packages. blithely re
167493	10	5219	602.19	among the quickly even excuses. packages across the quickly bold deposits was slyly after the thin dolphins. even deposits will nag quickly. final plate
167494	11	4527	949.61	tions. furiously bold deposits haggle. slyly express ex
167497	14	2325	877.89	ecial deposits. furiously ironic requests haggle above the courts. unusual accounts use thinly along the quickly ironic platelets. even packages serve. carefully silent 
167499	16	8368	721.79	regularly unusual dolphins thrash furiously among the blithely final instructions. pending, even accounts are slyly 
167500	17	59	980.48	 even ideas doze patterns. silent requests use carefully furiously quick d
167501	18	8883	83.87	 regular deposits cajole theodolites. regular accounts boost above the pinto beans. carefully regular deposits are blithely furiously re
167502	19	9386	361.70	lithely ironic ideas. deposits haggle furiously. slyly pending foxes use about 
167508	25	7242	469.62	ar excuses! slyly pending theodolites wake carefully. carefully e
167509	26	1118	950.71	s sleep. slowly unusual requests boost blithely blithely fi
167510	27	8600	919.61	 the furiously regular packages. special realms engage across the blithely ironic instructions. blithely final instructions wake. unusual excuses sho
167513	30	4526	119.33	fily blithely final platelets. furiously quick requests detect. furiously ironic excuses use quickly silent ide
167514	31	3622	156.34	eodolites are slyly across the furiously ironic pinto beans. furiously ironic packages after the regular deposits run furiously above the
167515	32	6271	210.09	s lose slyly bold theodolites. instructions integrate caref
167518	35	6933	195.29	 slyly pending Tiresias sleep. quickly express foxes run carefully 
167519	36	1812	988.83	se furiously pending requests. express, bold instructions wake after the silent packages. express ideas haggle packages. blithely special sentiments print fu
167523	40	9941	413.81	g the slyly ironic deposits. pinto beans sleep above the quickly regular frets. final dinos across the regular, e
167524	41	4447	717.19	 unusual requests: unusual accounts sleep fluffily quickly regular accounts? regular pinto beans are. carefully final instructions wake. 
167525	42	6658	506.39	ly final deposits-- slyly regular orbits wake carefully. express accounts cajole quickly. carefully express excuses use furiously according to the even waters. final, express pinto beans w
167529	46	8835	841.09	osits. regular, regular warhorses along the pending platelets affix quickly furiously even waters. blithely
167530	47	3106	516.35	sits are blithely quickly unusual deposits. slyly ironic accounts nag furiously. special, even packages wake about the 
167531	48	2140	424.45	y according to the deposits. final deposits after the carefully even requests may sleep final, express accounts. final ideas across
167532	49	3758	17.63	o wake. fluffily special requests are finally. ideas haggle carefully. carefully final foxes integrate. even, even foxes haggle carefully regular asymptotes. slyly ironic deposits integrate al
170000	1	1330	597.35	- bravely enticing dependencies across the ironic requests mold furiously about the ironic requests. even accounts haggle carefully! slyly regular pinto beans grow slyl
170003	4	3609	594.65	ial ideas wake carefully. furiously ruthless theodolites about the ideas sleep never bravely even requests. bold, final requests print furiously furiously final dependencies. iron
170005	6	1389	966.34	ully final dolphins sleep fluffily fluffily special packages. even foxes are bold platelets. quickly even excuses doze carefully even instructions. carefully regular requests shall h
170007	8	6641	952.03	ideas nag slyly pending, unusual foxes. ironic re
170009	10	3315	681.26	 packages haggle quickly carefully final foxes. bold courts wake carefully. special instructions sleep slyly? pending, express packages sleep after th
170010	11	314	134.07	 foxes x-ray among the somas. carefully final pinto beans wake against th
170013	14	6948	796.59	l asymptotes. slyly regular foxes boost slyly final, bold warthogs. furiously bold accounts wake. carefully express dependencies cajole. car
170015	16	5013	682.34	xpress deposits. regular, final requests sleep slyly among the ruthless excuses. carefully unusual platelets sleep fluffily against the blithely ironic platelets. asymptotes about the ironic 
170016	17	6535	794.37	es. bold instructions after the silent, ironic patterns haggle carefully among the depths. carefully unusual requests beyond the special sauternes are furiously special requests; furiously regul
170017	18	4208	2.89	cajole even requests. ironic, silent theodolites boost slyly ironic packages. regular deposits are above the f
170018	19	759	27.47	ithely bold packages must boost fluffily final accounts. quickly even ideas haggle slyly final, final packages. fluffily final theodolite
170024	25	3181	9.36	efully ironic instructions sleep blithely across the slyly final packages. accounts are carefully. regular, even deposits wake alongside of the carefully unusual ideas: carefully pending excuses w
170025	26	4167	526.25	gular theodolites use carefully? quickly unusual accounts detect slyly. carefully pending packages solve after the furiously furious dolphins. special accounts play! quickly ironic platelet
170026	27	6597	566.65	unusual packages. even, regular accounts haggle blithely under the depende
170029	30	3445	146.14	aggle regularly across the slyly final pearls. final, express grouches after the ruthlessly unusual packages are carefully bold requests. blithely final accounts no
170030	31	2415	717.20	en deposits. regular deposits cajole slyly according to the quickly
170031	32	5804	906.93	 blithely final requests. fluffily express accounts sleep. fluffily express deposits across the excuses haggle fluffily quickly regular deposits. permanent requests wake ironic packages. fluf
170034	35	5086	975.27	fully blithe tithes about the furiously even ideas cajole carefully fluffily special deposits. regular, ironic requests are ironic deposits. 
170035	36	884	809.02	uests sleep blithely. blithely final foxes are furiously above the furiously final platelets. carefully silent theodolites 
170039	40	8517	93.09	eodolites haggle ironic, pending theodolites; ironic pinto beans after the dependencies cajole about the furiously idle instructions.
170040	41	1475	801.56	ts. carefully pending orbits integrate furiously final packages. carefully pending accounts sleep. even deposits use quickly. bold package
170041	42	6910	849.32	thely against the blithely silent requests. boldly special asymptotes among the dependencies sleep blithel
170045	46	4977	147.37	ts cajole. finally ironic foxes boost. furiously express foxes cajole. theodolites promise. furiously speci
170046	47	4044	663.36	r accounts haggle quickly express requests. ironic, re
170047	48	2055	767.52	 along the finally pending requests. ruthlessly final deposits nag slyly above the slyly bold requests. quickly regular ideas sleep. express gifts use slyly ironic requests. slyly pending deposit
170048	49	3990	825.86	 requests. bold, even accounts will boost. unusual, ironic packages nag carefully. ir
172449	1	916	108.14	 the furiously even ideas: foxes sleep express packages. bold, pending pinto beans sleep. bold deposits nag blithely express deposits. asymptotes haggle slyly. blithely special p
172452	4	3827	62.08	inder furiously carefully final accounts. warhorses wake slyly express dependencies; regular 
172454	6	8154	124.70	 deposits wake quickly across the quickly pending platelets. quickly final packages use around the bold packages. quick, ironic 
172455	7	7665	719.91	s-- furiously pending pinto beans after the slyly 
172456	8	4007	432.82	ly even asymptotes according to the final ideas thrash slyly across the slowly even deposits. ironic deposits are ruthlessly final at
172458	10	3543	776.71	n requests; ironic, pending foxes nag furiously unusual accounts. final, express instructions wake furiously. care
172459	11	9424	191.76	ts. carefully express instructions by the ironic theodolites sleep among the unusual accounts. carefully pending accounts hinder furiously ironic decoys; pending, special depende
172462	14	6529	969.12	 ironic accounts cajole furiously unusual dependencies. final packages under the ironic foxes are busily quietly ironic dolphins. i
172464	16	5559	515.17	g slyly according to the quickly unusual packages. requests against the carefully ironic frets are ironic packages. expres
172465	17	2789	465.50	eodolites impress after the furiously regular packages. fluffily 
172466	18	5889	624.52	luffily brave accounts sleep slyly slyly final frays. furiously dogged packages x-ray. quickly unusual deposits beyond the final deposits wake against the blithely express theodolit
172467	19	5424	205.71	carefully final instructions. theodolites alongside of the ideas haggle slyly furiously express accounts. fu
172473	25	3269	288.52	closely. furiously unusual deposits about the final, regular theodo
172474	26	5995	316.33	quests. final, silent platelets are fluffily unusual requests. furiously pending accounts wa
172475	27	4679	129.65	are always. furiously ironic Tiresias doubt blithely beside the deposits. enticing, ironic packages haggle furiously bold platelets. pending requests wake quickly regular accounts.
172478	30	1666	908.60	uests. regular asymptotes doubt thinly. unusual foxes wake carefully across the regular dependencies. ideas solve regularly. ironic ideas wake. sentiments sleep theodolites. sly
172479	31	5257	600.03	ke fluffily around the regular packages. pending accounts across the carefully final packages sleep regularly against th
172480	32	6283	739.87	unusual deposits are blithely across the carefully regular requests. requests sleep furiously among the blithely special requests? regular, close pla
172483	35	8297	962.96	refully idle foxes sleep. ironic, stealthy orbits sleep furiously final theodolites. even, express de
172484	36	9477	838.34	 of the fluffily special deposits. deposits boost. blithely ironic packages
172488	40	418	77.16	efully regular warthogs kindle furiously pending asymptotes. thinly final ideas affix. even packages sleep slyly regular, ironic pinto beans. carefully pending hock
172489	41	3689	137.22	the regular, special packages shall have to wake under the furiously regular theodolites. carefully ironic pinto beans among the blithely regular packages use along the bold accounts. e
172490	42	8538	792.24	 the ironic theodolites. fluffily regular pinto beans sleep slyly above the even packages. final pinto beans cajole slyly permanently express platelets. ir
172494	46	5444	725.48	blithely final asymptotes use fluffily pending grouches. carefully final de
172495	47	9518	393.49	eposits. pending deposits about the final instructions integrate quickly about the furiously pending deposits. frets use carefully against the blithely ironic theodo
172496	48	9114	987.06	lly furiously ironic packages. carefully fluffy deposits cajole carefully abo
172497	49	995	723.22	 dolphins. final, regular dolphins cajole regular packages. regular, re
174966	1	6950	34.20	 regular, even packages. special multipliers along the slyly regular packages 
174969	4	8397	239.30	cies haggle furiously among the slyly unusual excuses. fi
174971	6	4889	693.41	lyly final accounts are slyly ironic asymptotes. ironic dolphins mold blithely. slyly unusual asymptotes doze slyly against the carefully final packages. unusual, ironic accounts bre
174972	7	5072	173.24	attainments according to the packages are blithely regular courts. foxes haggle above the unusual, final attainments. furiously even deposits haggle bold accounts. acco
174973	8	3483	687.54	 sleep blithely on the carefully even asymptotes. careful ideas integrate blithely. ironic, regular accounts use slyly. deposits cajole ruthlessly quickly final instructions. regular, silent reques
174975	10	5454	791.56	y regular accounts? regular, pending courts cajole instructions. blithely regular
174976	11	8593	922.67	hely. even accounts among the busily silent dolphins sleep after the ironic, ironic pains. furiously final accounts after the blithely 
174979	14	8898	701.57	dolites nag blithely. blithely final packages sleep carefully until the foxes. slyly pending foxes haggle. accounts abo
174981	16	976	522.05	counts sleep blithely ironic packages. slyly even ideas are bravely. acco
174982	17	4936	553.01	 blithely bold accounts affix furiously furiously unusual deposits. ironic packages use blithely slyly express t
174983	18	8746	601.89	ernes boost after the ironic accounts. silently pending platelets from the blithely pending dolphins sleep fluffily among the furiously ironic ideas. regular s
174984	19	3148	228.04	counts are furiously. furiously quick packages affix fluffily about the blithely final ideas. regular pinto beans thrash furiously. sl
174990	25	851	332.56	ts. final, bold theodolites haggle furiously slyly pending courts. carefully busy theodolites eat furiously. blithely final platelets sleep doggedly according to the fluffily bold theodolit
174991	26	8198	156.50	e final instructions are furiously final dependencies. slyly regular requests along the dependencies boost across the deposits. blithely u
174992	27	2898	651.60	 pending requests boost slyly ironic packages. ironic, regular theodolites according 
174995	30	6579	23.70	lyly final requests are about the slyly close gifts. bold pinto beans poach slyly ironic, daring packages. ideas cajole 
174996	31	2647	399.71	eposits cajole. silent, final deposits wake furiously across the blithely special patterns. quickly regular accounts among the furiously express requests 
174997	32	5047	707.89	hes are carefully. blithely ironic packages sleep furiously. furiously even packages sleep carefully according to t
175000	35	5308	986.26	ccounts boost along the special requests. quickly silent deposits are slyly since the carefully special instr
175001	36	1404	121.52	ress, regular instructions boost carefully regular requests. fluf
175005	40	5559	326.82	ns sleep alongside of the thinly regular accounts. regular, ironic accounts wake furiously against the blithely regular asymptotes. blithely even theodolites cajo
175006	41	2311	708.87	onic requests; foxes wake carefully through the silently regular ideas. express instructions are blithely careful foxes. ironic, regular theodolites above the slyly final packages wake furiously 
175007	42	4416	253.16	 even foxes. final excuses use carefully about the ironic req
175011	46	2314	716.89	le blithely. enticing sheaves alongside of the carefully final deposits detect finally against the furiously final deposits. idle packages cajole fluffily pending 
175012	47	7425	794.48	 quickly carefully bold packages. accounts x-ray furiously. epitaphs against the packages boost finally above the furiously regular packages. ironic,
175013	48	6368	137.74	latelets sleep blithely. regular deposits nag carefully bold courts-- furiously unusual requests boost quickly fluffily regular accounts. regular attainments cajole. regular accounts haggle furious
175014	49	1062	688.52	about the slyly even deposits. special, regular deposits according to the even platelets cajole after the unusual, exp
177483	1	434	496.82	al pinto beans. slyly final packages by the permanent, even pinto beans haggle quickly across the slyly brave instructions. carefully regular deposits haggle. furiously pending pinto b
177486	4	387	807.72	the theodolites. even escapades thrash furiously special pearls. furiously unusual requests haggle slyly according to the even requests. carefully 
177488	6	4606	964.41	ages use. express packages about the slyly even courts haggle daringly above the qu
177489	7	2939	2.35	l theodolites sleep among the quickly special packages. fluffily final accounts boost furiously. quickly final accounts sleep blithely fluffily bold deposits. fluffily pending depos
177490	8	8289	220.86	uses across the quickly furious asymptotes nag bold packages. bold foxes after the f
177492	10	3604	689.76	inal foxes cajole blithely. quickly special accounts haggle furiously about the carefully even deposits. final requests after the slyly final foxes are ironic instruct
177493	11	8588	298.67	tealthy requests; carefully even dolphins cajole furiously. final pinto bea
177496	14	3438	941.45	ously never ironic platelets. express deposits haggle furiously. accounts haggle carefully. accounts cajole fluffily furiously ironic foxes. accounts are s
177498	16	4745	969.25	ruthlessly regular, idle sentiments. bold accounts according to the excuses are permanently around the regular ideas-- furiously furious packages above the pen
177499	17	7188	754.70	y ironic packages are blithely final ideas. idly final requests dazzle above the furiously express platelets. packages eat 
177500	18	1978	491.70	ts. blithely unusual gifts alongside of the furiously ironic escapades hinder before the
177501	19	9174	146.28	ke quickly about the regular, final deposits. express grouches wake quickly. regular dolphins haggle regular, silent foxes. quickly regular ideas wake special i
177507	25	7665	108.30	eas. slyly special deposits according to the regular
177508	26	2281	731.52	riously. blithely ironic accounts haggle quickly after the bold dependencies. pending, final requests use accounts. blithely fu
177509	27	3607	858.08	cuses haggle slyly packages. quickly even asymptotes are furiously. ironic, final packages impress furiously. slyly even realms boost carefully. final instructions use slyly fluffily unus
177512	30	5072	225.26	gular ideas. fluffily regular platelets about the quickly ironic accounts are alongside of the blithely pending pains. theodolites haggle blithely. 
177513	31	6938	353.33	hinly regular instructions haggle carefully above the final frays. blithely special accounts boost. carefully pending deposits near the slyly even a
177514	32	579	750.20	 even requests. regular, pending deposits haggle carefully pending packages. final accounts according t
177517	35	3847	564.92	ges. furiously pending courts cajole slyly. furiously pending deposits among the regular, even theodolites integrate en
177518	36	8882	138.22	. carefully regular excuses are quickly along the packages. slyly pending foxes are blithely regular instructions. furiously ironic packages according to the blithely express packages nag r
177522	40	7130	658.95	pecial packages above the even ideas cajole unusual accounts; blithely bold deposits haggle atop the silent, bold dolphins. bold, final fox
177523	41	7895	323.85	requests. furiously special courts about the blithely bold deposits nag blithely ironically final escapades. slyly unusual pinto beans nag above the slyly 
177524	42	9279	299.95	gular deposits boost furiously pending grouches. blithely express instructions snooze. unusual deposits mold quickly. carefully daring ideas across the e
177528	46	5628	867.37	pinto beans. carefully ironic instructions during the pinto beans are bl
177529	47	8104	558.50	uctions. regular, final forges haggle. quickly ironic idea
177530	48	752	925.86	e quickly final orbits. accounts integrate around the slyly ironic frays. express excuses about the slyly pending deposits kindle fluffily final dependencies. special warthogs haggle furi
177531	49	3448	220.33	 fluffily pending ideas. blithely even asymptotes haggle. pending, even dependencies wake against the deposits. regular dependencies across
180000	1	907	25.88	hely regular forges cajole. blithely unusual platelets affix furiously alongside of the requests. special platelets detect busy, bold accoun
180003	4	6796	551.49	ackages wake. fluffily regular epitaphs are furiously about the slyly silent accounts. epitaphs wake fluffily. carefully bold waters h
180005	6	6514	638.63	nding asymptotes. unusual, unusual instructions run quietly across the accounts. pending foxes integrate. fluffily bold ideas according to the express pa
180006	7	5833	939.09	. slyly bold notornis sleep according to the furiously ironic platelets. final, pending sentiments might haggle slyly! carefully final deposits nag above the slyly final pinto beans
180007	8	8073	562.97	 furiously about the quickly special ideas. accounts sleep fluffil
180009	10	8997	796.76	c, special deposits. furiously regular foxes wake regular deposits. carefully regular deco
180010	11	8678	115.67	 packages. realms wake. final asymptotes nod. slyly regular accounts cajole slyly final dinos. e
180013	14	4301	731.93	ld foxes hinder blithely regular asymptotes. fluffily pending accounts haggle finally within the depos
180015	16	7515	759.51	er the slyly pending deposits. carefully unusual deposits haggle quickly r
180016	17	282	604.55	carefully ironic, bold platelets. requests affix furiously across the special instructions. slyly regular pack
180017	18	3177	705.55	ges. silent deposits since the unusual packages integrate slyly slyly ironic deposits. slyly express waters detect special, regular deposits. ironic, express packages serve above the carefully expre
180018	19	945	364.19	 boost furiously stealthy, special accounts. regular deposits wake blithely. unusual, even ideas boost furiously final instructions. sly
180024	25	9577	803.35	above the regular, regular foxes. carefully special deposits snooze slyly against the 
180025	26	3632	430.41	ngly pending foxes. slyly final accounts haggle above the theodolites. regular platelets wake slyly after the slyly even instructio
180026	27	4185	967.29	eposits. blithely ironic braids detect furiously above the quickly special theodolites. regular, even requests nag fin
180029	30	1940	124.39	y slyly ironic theodolites. slyly final foxes haggle according to the ironic pinto beans. blithely unusual pearls kindle. bold requests against the deposits boost
180030	31	8589	21.07	eposits. blithely final requests according to the requests s
180031	32	5794	698.11	ave to sleep furiously after the even, final packages. ironic packages wake carefully 
180034	35	1604	966.98	cies are slyly. even accounts unwind express packages? 
180035	36	2337	819.49	uriously regular packages about the sometimes pending deposits integrate blithely alongside of the foxes. quickly bold foxes across the even, special deposits are carefully express
180039	40	6924	232.13	xpress packages cajole carefully quickly stealthy accounts. busily express requests are. ironic theodolites near the blithely special pinto
180040	41	5835	404.33	s integrate instructions. platelets according to the blithely ironic instructions dazzle express id
180041	42	1404	287.87	equests wake slyly. express warhorses detect ironically around the blithely unusual pinto beans. carefully furious pinto beans among the furiously even pinto beans cajole quickly after the unusual p
180045	46	2959	962.75	regular requests doze busily around the furiously stealthy requests. sly
180046	47	5979	701.29	requests haggle carefully according to the quickly regular accounts. asymptotes cajole r
180047	48	861	479.32	carefully sly escapades use furiously. final, even instructions use b
180048	49	6522	366.34	busy pinto beans thrash furiously slyly regular dependencies! even packages wake furiously final asy
182472	27	5446	129.28	 theodolites. even instructions are furiously pearls. carefully iro
182446	1	5959	16.83	ss the quickly ironic pinto beans. furiously bold packages will have to play quickly regular de
182449	4	5822	322.61	s integrate; slyly quiet accounts was carefully aft
182451	6	1650	963.44	fully alongside of the courts. regular, ironic pinto beans boost against the slyly e
182452	7	3732	510.10	gedly according to the quickly ironic deposits. furiously pending asymptotes cajole even ideas. bold packages was after the furiously express packages. even packages cajol
182453	8	2375	474.10	ackages sleep furiously about the enticingly even foxes. foxes according to the pending instruct
182455	10	5828	256.80	ounts. slyly pending excuses according to the permanently regular packages sleep furiously across the fluffily final asymptotes. carefully special foxes integr
182456	11	8257	788.14	ests hang furiously bravely express accounts. carefully special accounts sleep slyly. accounts amo
182459	14	131	800.26	nts. carefully regular pinto beans haggle. even accounts use among the always 
182461	16	4704	62.19	to beans. carefully pending instructions run slyly q
182462	17	2057	717.27	ly unusual requests. fluffily final pinto beans boost against the blithely regular foxes. carefully regular asymptotes wake carefully express, r
182463	18	2717	993.74	 requests. carefully final courts sleep furiously near the ironic courts. quickly unusua
182464	19	5548	489.33	the packages-- courts promise. carefully pending theodol
182470	25	1591	131.16	ss, regular courts integrate quickly ironic deposits. slyly even foxes nag quickly against 
182471	26	8471	782.66	ans. blithely final instructions nag accounts. blithely pending deposits are. quickly regular requests against 
182475	30	3961	331.00	ain deposits. express accounts dazzle. furiously express dinos sleep carefully special requests. fluff
182476	31	9514	161.13	 instructions-- regular, unusual dugouts across the ironically silent excuses kindle furiously final accounts. carefully unusual fo
182477	32	1858	235.03	y regular deposits. silent deposits above the slyl
182480	35	5388	701.41	 are slyly. furiously ironic ideas wake slyly. furiously ironic requests haggle above the ironic pinto beans. regular deposits about th
182481	36	7677	502.53	 ironic packages. bold, final theodolites haggle furiously over the brave pinto beans. slyly final deposits wake furiously among t
182485	40	4818	847.11	al requests cajole furiously slyly final sentiments. carefully regular instructions shall have to use. platelets cajole carefully. fluffily even dinos nag slyly. ironic pinto b
182486	41	9633	753.21	e against the carefully quick theodolites. carefully final accounts nag pending dependencies. brave, even requests are fluffily.
182487	42	1010	576.90	d the slyly ironic deposits are up the fluffily regular deposits. quickly regular requests haggle quickly bold foxes. bold platelets about the dogged, ironi
182491	46	2458	424.11	carefully ironic requests affix slowly along the slyly regular theodolites. idly unusual dependencies are fluffily ironic deposits. bold, silent braids should have to nag slyly silent, ironic ac
182492	47	2110	265.16	nding accounts. quickly final packages nag slyly! slyly bold theodolites play carefully. ironic ideas along the ideas promise furiously regular instructions. quickly unusual theodolites sleep 
182493	48	7207	366.81	ns. carefully ironic courts cajole fluffily. final i
182494	49	9470	140.00	ggle fluffily pending deposits. slyly bold packages above the carefully reg
184964	1	6554	412.40	even, brave excuses are quickly even pearls. sauternes wake around the regular, bold asymptotes. final ac
184967	4	6052	401.62	se pinto beans. blithely express foxes are fluffily final requests. slyly special platelets are
184969	6	562	145.81	furiously final platelets kindle blithely quickly final requests. ironically eve
184970	7	7412	290.29	r packages. furiously silent notornis unwind blithely. ironic requests boost slyly quickly final ideas. carefully even platelets cajole. blith
184971	8	5166	820.59	usly pending theodolites. packages against the furio
184973	10	6436	882.37	above the furiously express platelets. asymptotes can boost according to the carefully ironic accounts. regular, final foxes are carefully along the slyly unusual deposits. quickly regular packages
184974	11	2860	499.77	accounts boost furiously unusual requests-- express, even instruc
184977	14	5811	963.57	into beans. slyly silent instructions serve furiously. express deposits n
184979	16	3101	355.35	uriously regular packages are above the blithely fluffy requests. pe
184980	17	3508	538.21	ending platelets! special platelets ought to sleep fluffily. unusual foxes believe furiously slyly unusual instructions. slyly unusual deposits cajole blithely. asymptotes 
184981	18	6876	581.56	eas sublate. silent deposits nod about the furiously brave theodolites. regular, bold notornis thrash along the quie
184982	19	7566	452.70	ng the accounts integrate always according to the asymptotes. fluffily careful deposits are slyly along the blithely express theodolites? fluffily
184988	25	5853	974.80	ending ideas sleep thinly thinly even foxes. idly pending asymptotes are dur
184989	26	1993	332.30	ses at the final dependencies wake above the fluffily special asymptotes. slyly special pinto beans alongside of the requests poach acco
184990	27	8463	421.38	lly foxes. quickly idle requests nag. ironic platelets
184993	30	537	365.21	y slyly pending ideas. requests sleep ironic, bold theodolites. deposits among th
184994	31	373	259.60	egular theodolites serve. regular deposits boost. carefully final sauternes lose blithel
184995	32	2477	657.47	ial accounts haggle furiously. final dinos sleep furiously iro
184998	35	2237	867.17	s. requests sleep along the ironic, ironic requests. furiously final deposits wake requests. furiously ironic instructions hag
184999	36	6157	813.43	, final packages cajole blithely accounts. regular requests use idly along the carefully even
185003	40	1090	452.59	s. silent, express grouches along the accounts promise blithely across the carefully bold deposits. unusual packages sleep above the furiously final realms. bold instructions haggle! furio
185004	41	3757	135.89	y express deposits. regular dolphins haggle above the furiously ironic packages. furiously ironic r
185005	42	1822	140.37	symptotes detect carefully against the furiously ironic deposits. regular, special accounts promise blithely among the pending foxes. carefully unusual requests cajole bli
185009	46	4862	706.23	into beans wake about the even dependencies. bold courts are fluffily according to the furiously ironic sauternes! final foxes boost. quietly express d
185010	47	4220	214.24	foxes sleep slyly carefully ironic requests. blithely bold instructions sleep carefully blithely final pinto beans. furious
185011	48	9721	348.16	aters. carefully even ideas according to the express requests promise carefully ironic 
185012	49	139	307.64	yly pending ideas wake blithely across the blithely ironic deposits. blithely regular deposits boost quickly even packages. unusual, unusual accounts wake the
187482	1	7803	177.21	inst the bravely final escapades cajole among the slyly blithe asymptotes. unusual accounts
187485	4	3375	812.71	ly even deposits. carefully pending requests use furiously fluffily special frays. special theodolites wake quickly furiously ironic dependencies.
187487	6	7	447.80	y doggedly regular excuses. slyly bold deposits according to the instructions sleep carefully even instructions. flu
187488	7	442	229.19	foxes. slyly bold instructions cajole. furiously express deposits sleep blithely alo
187489	8	2750	472.16	he quickly ironic courts. slyly unusual requests above the slyly express foxes are quickly blithely bold platelets. slyly express pinto beans wake. furiously final packages against the unus
187491	10	1164	213.32	across the special accounts. quickly even accounts use furiously even deposits. furiously 
187492	11	3387	544.22	ites can wake unusual dependencies. slyly unusual accounts haggle among the fluffily bold asymptotes. blithely final ideas hang blithely silent packag
187495	14	9262	455.10	onic deposits cajole furiously of the requests. fluffily regular accounts sublate
187497	16	4896	229.00	ests haggle fluffily. regular, bold orbits nag furiously
187498	17	5396	512.80	ts. blithely silent dependencies detect ironically unusual deposits. regular, unusual requests over the regular accounts int
187499	18	5005	548.45	lets sleep carefully after the final theodolites. furio
187500	19	5199	520.04	es. ironic excuses haggle carefully. accounts against the always busy tithes nag furiously regular fox
187506	25	3348	949.87	pinto beans. special, special deposits are fluffily blithely pending packages. blithely regular packages believe slyly final accounts. u
187507	26	9480	884.46	endencies use blithely. quickly special packages maintain. fluffily final deposits x-ray across the regular accounts. furious
187508	27	3076	223.56	ironic dependencies print: express deposits wake accordin
187511	30	762	505.68	ccounts. slyly bold packages against the even pinto beans slee
187512	31	7194	627.28	ide of the quickly regular packages cajole carefully
187513	32	7926	8.13	beans. ironic accounts boost bravely carefully silent ideas. slyly express deposits wake slyly. even requests are furiously al
187516	35	2368	365.88	 sheaves. carefully final pinto beans haggle carefully furiously even theodolites. even instructions after th
187517	36	3908	919.47	rnes sleep furiously carefully ruthless instructions. f
187521	40	1979	169.32	 blithely even deposits. furiously ironic asymptotes sleep blithely. slyly ironic platelets use s
187522	41	3702	23.23	 ironic accounts. blithely special ideas alongside of the furiously final pinto beans haggle blithely packages. quickly regular dependencies wake carefully across 
187523	42	5121	506.25	sides the furiously final requests. blithely ironic instructions among the pendi
187527	46	2342	517.07	y blithely final deposits. pending, regular courts after the b
187528	47	8255	454.76	equests. slyly unusual requests sleep quickly. even packages wake furiously against the ironic grouches. ironic, bold instructions sleep careful
187529	48	4594	284.72	lithely regular instructions above the slyly even packages play fluffily regular asymptotes; fluffily reg
187530	49	2447	38.46	s affix slyly blithely final theodolites. special f
190000	1	4988	209.46	theodolites until the regular accounts are carefully at the final courts. furiously final sentiments use ruthless, even asymptotes. carefully express 
190003	4	6024	588.99	posits detect according to the slyly ironic requests. fluffily even requests integrate carefully. special, special packages boost s
190005	6	9896	794.43	nal, final packages. furiously busy deposits affix silently even foxes. carefully ironic requests use slyly after the daring packages. dolphins 
190006	7	8537	214.97	blithely ironic, pending ideas. slyly express sheaves use blithely express platelets. slyly unusual requests are slyly un
190007	8	8128	718.03	rs detect furiously unusual dependencies. blithely final requests sleep carefully quickly final dependencies. carefully enticing asymptotes until the never ironic theodolites engage furiously slyly
190009	10	6551	613.20	ls haggle. ironic excuses are fluffily about the enticingly ironic
190010	11	9432	919.09	ckages haggle quickly. bold excuses cajole quickly. slyly ironic epitaphs haggle fluffily even courts. express theodolites haggle blithely 
190013	14	6295	32.44	bove the carefully final dolphins. special sauternes are quickly. quickly final foxes across the ironic requests use s
190015	16	3822	903.76	carefully ironic requests among the furiously regular deposits are after the furiously ironic ideas. ironic ideas snooze about the deposits! c
190016	17	6840	763.01	lar packages haggle blithely ironic deposits. slyly dogged dolphins are furiously regular, bold warthogs. furiously regular requests above the fluffily express pinto beans haggle fluffily final dep
190017	18	9232	580.01	cording to the final deposits. blithely even pinto beans play furiously final, ironic foxes. final courts integrate according to the furiously express decoys. regular, bol
190018	19	5268	538.50	sual theodolites. blithely ironic patterns haggle
190024	25	4129	63.11	regular accounts! pending, even dolphins sleep quickly-- carefully regular packages cajole requests. furiously express accounts along the ironic, ironic ideas unwind along
190025	26	5912	537.81	r instructions: quickly pending deposits above the bravely express deposits use alongside of the carefully bold requests. final, final packages are blit
190026	27	6304	208.40	l packages. carefully ironic pinto beans sleep. express, regular somas are aft
190029	30	69	933.16	 above the requests wake furiously about the slyly ironic accounts! furiously even requests are blithely enticing packages. ironic, even p
190030	31	1228	312.67	le after the regular instructions. special, ironic theodolites kindle slyly against the closely ironic instructions. furiously pending deposits cajole above the blithely pending 
190031	32	4842	455.98	fter the unusual deposits are carefully blithely ironic requests. even, special requests according to the furiously unusual requests dazzle quickly unusual inst
190034	35	4355	846.34	. slyly ironic ideas wake quickly. furiously regular packages are blithely against 
190035	36	6936	511.13	s the final sauternes are even requests-- regular packages nag slyly express dependencies. final accounts serve furio
190039	40	9536	32.48	structions about the blithely final requests haggle slyly beside the carefully regular requests. fluffily final packages at the blithely enticing id
190040	41	3438	706.29	 furiously across the carefully final foxes. plate
190041	42	2685	440.60	 furiously unusual dolphins. unusual courts above the idle accounts need to x-ray never sometimes permanent theodolit
190045	46	2104	362.98	le across the packages. unusual deposits around the deposits haggle ideas. deposits cajole at the pending, regular packages. regular, 
190046	47	2498	693.99	ackages. carefully special dolphins among the ironically even pinto beans wake blithely across t
190047	48	8987	645.39	 from the express theodolites: regular accounts hind
190048	49	5007	488.59	long the even accounts wake carefully against the close deposi
192443	1	9554	946.43	 excuses cajole furiously. regular deposits alongside of the furiously pending sheaves cajole quickly pending requests. fluffily u
192446	4	5193	334.94	 boost carefully. packages sleep quickly carefully regular theodoli
192448	6	1899	168.85	, bold theodolites cajole. requests affix. blithely silent pinto beans haggl
192449	7	4010	330.38	althy requests. slyly bold requests maintain quickly. silent depths above
192450	8	5935	336.02	ges according to the ironic requests haggle after the ironic, regular packages. carefully pending instruc
192452	10	1403	799.18	ckages haggle furiously escapades. slowly even theodolites s
192453	11	8844	730.13	nic orbits affix above the never express packages. express, ironic requests must use about the pending deposits. pending foxes are
192456	14	1385	374.86	deposits. slyly special foxes nag alongside of the furiously even packages. carefully dogged packages above the slyly unusual deposits run special, unusual packages. blithely special requests s
192458	16	4036	59.80	. packages about the slyly final deposits unwind alongside of the regular foxes. slyly regular deposits haggle carefully carefully pending requests. regular packages alongside of the fluffily si
192459	17	7763	657.76	lphins. regular instructions nag fluffily above the special requests. ironic, final pinto beans about the regular, unusual deposits haggle quickly quietly ironic instructions. 
192460	18	3795	742.17	al excuses haggle. furiously regular Tiresias against the furiously ironic instructions wake fluffily carefully final deposits. slyly bold pinto beans wake. silent
192461	19	4439	918.78	lly pending requests use slyly about the regular ac
192467	25	9948	905.54	nic packages sleep busily. requests across the fluffily even packages haggle carefully ironic packages. regular deposits poach. slyly regular a
192468	26	2657	726.68	n ideas nag special packages: furiously pending dependencies nag stealthily. blithely regular pinto beans to the pinto beans integrate beneath the carefully even foxes. final 
192469	27	1868	338.66	the carefully silent instructions boost fluffily carefully final instructions. regular theodolites wake furiously f
192472	30	6634	684.46	nts. carefully special foxes detect. instructions of the carefully ironic instructions nag furiously pending pinto beans. blithely unusual depos
192473	31	3926	776.09	deas sleep slyly inside the enticingly express requests. quickly unusual accounts use sometimes. regular, si
192474	32	5958	363.58	kages. quiet accounts boost blithely about the quick, final deposits: enticingly unusua
192477	35	2621	413.27	uctions x-ray blithely blithely regular dolphins. unusual requests wake quickly blithely even asymptotes. slyly regular deposits aro
192478	36	248	22.59	ly! ironic accounts are furiously. regular packages haggle quickly final packages.
192482	40	5482	551.43	leep slyly fluffily pending packages. furiously ironic accounts along t
192483	41	3345	640.14	al accounts boost carefully. slyly special requests a
192484	42	6624	877.99	carefully bold requests serve slyly requests. blithely ironic foxes nag slyly
192488	46	6391	371.30	pecial pinto beans. bold, idle instructions haggle thinly. ruthlessly unusual packages wake r
192489	47	8053	349.97	ts. carefully final forges around the blithely final forges try to integrate fluffily along the blithely regular requests. accounts nag fluffily ironic packages. quietly r
192490	48	4485	435.41	t above the furiously regular pinto beans. furiously even theodolites sleep slyly unusual pack
192491	49	2864	819.55	haggle blithely along the bold requests. pending requests wake fluffily unusual deposit
194962	1	8641	688.71	y. busily pending platelets cajole at the fluffily ironic excuses. express packages must have to cajole quickly sentiments. furiously express depos
194965	4	9614	384.27	 theodolites. blithely pending deposits nag. pending pinto beans boost blithely after the regular theodolites. fluffily pending deposit
194967	6	2667	312.92	oxes. final accounts along the ironic, special accounts integrate closely among the fluffily pending accounts; boldly fluffy excuses haggle. foxes above the theodolites grow carefully
194968	7	1641	214.92	gular courts sleep regular, pending requests. fluffily unusual platelets haggl
194969	8	6673	255.82	deposits sleep blithely ironic packages; daringly idle packages could have to nag alongside of the furiously brave foxes. fluffily unusual escapades impress regular deposits. fluffily final depos
194971	10	7597	336.80	ounts are against the asymptotes. regular packages boost accordi
194972	11	8930	130.91	 even pinto beans integrate quickly against the pending accounts. fina
194975	14	8949	537.59	uickly final, unusual deposits. quickly ironic request
194977	16	8565	243.04	e of the slyly express accounts cajole pending orbits. furiously permanent attainmen
194978	17	8173	835.63	ual accounts. boldly silent asymptotes according to the excuses boost after the final packages. furiously even requests engage
194979	18	8140	973.54	posits about the express excuses nag carefully bold instructions. furiously even packages according to the blithely brave theodolites are
194980	19	4636	26.28	. slyly quick packages doze toward the packages. never ironic theodolites solve before the regular, unusual inst
194986	25	892	658.17	ies. sauternes haggle carefully. even, pending ideas engage blithely behind the furiously regular requests. slyly bold asymptotes wake across the sometimes express 
194987	26	3751	662.60	quickly regular asymptotes was blithely against the accounts. regular packages cajole slyly. fur
194988	27	8711	312.23	fully idle deposits nag between the regular, careful instructions. even excuses after the always regular accounts nag across the b
194991	30	1283	26.96	y unusual, bold instructions. quickly ironic theodolites against the final packages sleep around the furiously unusual
194992	31	7860	622.65	endencies use above the requests. quickly special requests sleep blithely furiously final foxes. slyly final excuses haggle fluffily quickly silent dolphi
194993	32	5612	883.12	yly bold dependencies integrate slowly ironic requests. ironic instructions wake. deposits s
194996	35	65	645.47	ealthily regular packages wake furiously special packages. quickly enticing accounts use against the silent instructions. c
194997	36	524	46.42	fully final deposits are carefully quickly final instructions: carefully even instructions x-ray ab
195001	40	940	501.29	cial waters. even excuses should use asymptotes. bo
195002	41	6971	690.76	ess requests haggle evenly. unusual courts nod special theodolites. furiously regular dependencies sleep. special de
195003	42	5449	963.25	 bold packages. requests need to integrate. blithely express 
195007	46	832	772.17	ngage blithely. unusual platelets cajole quickly furiously even accounts. unusual ideas haggle daringly among the regular asymptotes. quic
195008	47	4355	683.55	ully unusual deposits. blithely ironic dependencies serve after the slyly pending courts. permanently special courts nag carefully according to the furiously express requests. carefully 
195009	48	2561	570.71	- slyly even accounts hang slyly. sly, special deposits use carefully. regular theodolites cajole furiously. carefully regular requests
195010	49	9037	230.65	. furiously furious courts wake. carefully bold requests wake whithout the carefully silent requests. carefully exp
197481	1	5205	465.57	ironic theodolites. fluffily close accounts boost slyly during t
197484	4	7751	619.15	oxes. slowly express accounts haggle furiously special, final theodolites-- furiously pending packages above the blithely final theodoli
197486	6	4031	625.45	 are slyly within the express dolphins. slyly silent requests print. slyly ironic packages sleep blithely. bol
197487	7	3341	862.38	express, express requests are carefully carefully even requests. fluffily ironic instructions wake carefully bold waters. furiously pending asymptotes after the regular packages cajole slyly ins
197488	8	6894	859.63	ronic dependencies kindle quickly furiously pending asymptotes; car
197490	10	5572	189.32	s cajole carefully across the quickly special platelets. furiously regular 
197491	11	9145	994.42	 promise across the carefully silent packages. slyly final deposits around the silently pending ideas are carefully bold pinto beans. furiously final pinto beans
197494	14	1698	160.69	s engage blithely about the regular packages. carefully regular attainments about the ironic packages are quickly regular requests. slyly final accounts haggle carefully regular 
197496	16	4048	528.85	s ideas. special packages sleep quickly according to the pinto beans. carefu
197497	17	7961	883.17	arefully final requests wake. quietly even deposits wake careful
197498	18	9814	977.23	t requests detect across the furiously final foxes. unusual packages across the quietly final accounts affix fluffily above the bold deposits. accounts among the furiously bold packages c
197499	19	1232	315.12	eodolites cajole slyly. ironic warhorses sleep slyly ruthlessly ruthless asymptotes. instructions haggle. furiously final asymptotes alongside of the slyly special excuses integrate ca
197505	25	5606	206.36	er the final theodolites nag above the even packages. furiously sp
197506	26	7807	678.12	quickly. slyly regular instructions boost furiously permanently
197507	27	9041	395.68	ages. carefully final deposits toward the unusual accounts are f
197510	30	1828	931.48	totes use furiously blithely bold requests. requests cajole final, unusual accounts. blithely sp
197511	31	3468	415.28	ependencies haggle about the slyly special instructions. regular, express deposits wake; requests doubt carefully regular packages. carefully unusual requests haggle slyly above the sly
197512	32	8645	158.59	ously against the unusual packages. slyly express requests kindle among the ruthles
197515	35	8166	115.80	carefully even theodolites. final foxes haggle. quickly special requests haggle. silent, final requests are along the slyly ironic theodolites. ironic, unusual decoys
197516	36	9715	919.46	lar pinto beans use according to the ironic, unusual deposits. furiously bold ideas integrate carefully about 
197520	40	9722	979.20	ously carefully regular pinto beans. blithely bold instructions sleep. regular accounts haggle against the furiously special instructions. al
197521	41	8361	65.31	lithely fluffy escapades. slyly regular deposits cajole carefully final instructions. fluffily special packages above the requests caj
197522	42	9226	608.48	out the daring, ironic deposits. furiously bold requests use carefully above the
197526	46	7163	356.27	 unusual packages unwind along the slyly silent requests. blithely regular foxes sleep carefully-- carefully ironic dependencies are fluffily deposits. furiously exp
197527	47	3404	222.68	ach. ironic, ironic sauternes cajole slyly quickly pending packages. final accoun
197528	48	2156	995.15	olites wake furiously along the bold, ironic dependencies. blithely regular
197529	49	1855	960.80	use furiously among the slyly ironic packages. pinto beans solve requests. furiously ironic accounts integrate about the final deposits. slyly regular courts along the 
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
11	Supplier#000000011       	JfwTs,LZrV, M,9C	18	28-613-996-1505	3393.08	y ironic packages. slyly ironic accounts affix furiously; ironically unusual excuses across the flu
14	Supplier#000000014       	EXsnO5pTNj4iZRm	15	25-656-247-5058	9189.82	l accounts boost. fluffily bold warhorses wake
16	Supplier#000000016       	YjP5C55zHDXL7LalK27zfQnwejdpin4AMpvh	22	32-822-502-4215	2972.26	ously express ideas haggle quickly dugouts? fu
17	Supplier#000000017       	c2d,ESHRSkK3WYnxpgw6aOqN0q	19	29-601-884-9219	1687.81	eep against the furiously bold ideas. fluffily bold packa
18	Supplier#000000018       	PGGVE5PWAMwKDZw 	16	26-729-551-1115	7040.82	accounts snooze slyly furiously bold 
19	Supplier#000000019       	edZT3es,nBFD8lBXTGeTl	24	34-278-310-2731	6150.38	refully final foxes across the dogged theodolites sleep slyly abou
25	Supplier#000000025       	RCQKONXMFnrodzz6w7fObFVV6CUm2q	22	32-431-945-3541	9198.31	ely regular deposits. carefully regular sauternes engage furiously above the regular accounts. idly 
26	Supplier#000000026       	iV,MHzAx6Z939uzFNkq09M0a1 MBfH7	21	31-758-894-4436	21.18	 ideas poach carefully after the blithely bold asymptotes. furiously pending theodoli
27	Supplier#000000027       	lC4CjKwNHUr6L4xIpzOBK4NlHkFTg	18	28-708-999-2028	1887.62	s according to the quickly regular hockey playe
30	Supplier#000000030       	84NmC1rmQfO0fj3zkobLT	16	26-940-594-4852	8080.14	ias. carefully silent accounts cajole blithely. pending, special accounts cajole quickly above the f
31	Supplier#000000031       	fRJimA7zchyApqRLHcQeocVpP	16	26-515-530-4159	5916.91	into beans wake after the special packages. slyly fluffy requests cajole furio
32	Supplier#000000032       	yvoD3TtZSx1skQNCK8agk5bZlZLug	23	33-484-637-7873	3556.47	usly even depths. quickly ironic theodolites s
35	Supplier#000000035       	QymmGXxjVVQ5OuABCXVVsu,4eF gU0Qc6	21	31-720-790-5245	4381.41	 ironic deposits! final, bold platelets haggle quickly quickly pendin
36	Supplier#000000036       	mzSpBBJvbjdx3UKTW3bLFewRD78D91lAC879	13	23-273-493-3679	2371.51	ular theodolites must haggle regular, bold accounts. slyly final pinto beans bo
40	Supplier#000000040       	zyIeWzbbpkTV37vm1nmSGBxSgd2Kp	22	32-231-247-6991	-290.06	 final patterns. accounts haggle idly pas
41	Supplier#000000041       	G 1FKHR435 wMKFmyt	18	28-739-447-2525	6942.67	odolites boost across the furiously regular fo
42	Supplier#000000042       	1Y5lwEgpe3j2vbUBYj3SwLhK62JlwEMtDC	22	32-698-298-6317	6565.11	 fluffily even requests cajole blithely fu
46	Supplier#000000046       	e0URUXfDOYMdKe16Z5h5StMRbzGmTs,D2cjap	24	34-748-308-3215	3580.35	gular, regular ideas across th
47	Supplier#000000047       	3XM1x,Pcxqw,HK4XNlgbnZMbLhBHLA	14	24-810-354-4471	2958.09	sly ironic deposits sleep carefully along t
48	Supplier#000000048       	jg0U FNPMQDuyuKvTnLXXaLf3Wl6OtONA6mQlWJ	14	24-722-551-9498	5630.62	xpress instructions affix. fluffily even requests boos
49	Supplier#000000049       	Nvq 6macF4GtJvz	24	34-211-567-6800	9915.24	the finally bold dependencies. dependencies after the fluffily final foxes boost fluffi
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

