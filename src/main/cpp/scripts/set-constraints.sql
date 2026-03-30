-- Relations:
-- customer | lineitem | nation | orders | part | partsupp | region | supplier

-- Constraints following TPCH documentation:
-- http://www.tpc.org/tpc_documents_current_versions/pdf/tpc-h_v2.18.0.pdf

\set STARTDATE '''1992-01-01'''::DATE
\set ENDDATE '''1998-12-31'''::DATE
\set CURRENTDATE '''1995-06-17'''::DATE


-- lineitem
-- start by dropping constraints if they already exist
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_partkey_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_linenumber_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_quantity_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_discount_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_tax_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_returnflag_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_returnflag_based_on_receiptdate_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_linestatus_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_linestatus_based_on_shipdate_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_receiptdate_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_shipinstruct_domain;
ALTER TABLE lineitem DROP CONSTRAINT IF EXISTS l_shipmode_domain;
-- adding constraints
-- partkey random value [1 .. (SF * 200,000), constraint based on sf of 0.1
ALTER TABLE lineitem ADD CONSTRAINT l_partkey_domain CHECK
    (l_partkey >= 1 and l_partkey <= 20000);
ALTER TABLE lineitem ADD CONSTRAINT l_linenumber_domain CHECK
    (l_linenumber >= 1 and l_linenumber <= 7);
ALTER TABLE lineitem ADD CONSTRAINT l_quantity_domain CHECK
    (l_quantity >= 1 and l_quantity <= 50);
ALTER TABLE lineitem ADD CONSTRAINT l_discount_domain CHECK
    (l_discount >= 0.00 and l_discount <= 0.10);
ALTER TABLE lineitem ADD CONSTRAINT l_tax_domain CHECK
    (l_tax>= 0.00 and l_tax <= 0.08);
ALTER TABLE lineitem ADD CONSTRAINT l_returnflag_domain CHECK
    (l_returnflag in ('R', 'A', 'N'));
-- second l_returnflag domain check, conditional based on l_receiptdate
ALTER TABLE lineitem ADD CONSTRAINT l_returnflag_based_on_receiptdate_domain CHECK
    ((l_returnflag in ('R', 'A') AND l_receiptdate <= :CURRENTDATE) OR
     (l_returnflag = 'N' AND l_receiptdate > :CURRENTDATE));
ALTER TABLE lineitem ADD CONSTRAINT l_linestatus_domain CHECK
    (l_linestatus in ('O', 'F'));
-- second l_linestatus domain check, conditional based on l_shipdate
ALTER TABLE lineitem ADD CONSTRAINT l_linestatus_based_on_shipdate_domain CHECK
    ((l_linestatus = 'O' AND l_shipdate > :CURRENTDATE) OR
     (l_linestatus = 'F' AND l_shipdate <= :CURRENTDATE));
ALTER TABLE lineitem ADD CONSTRAINT l_receiptdate_domain CHECK
    (l_receiptdate >= l_shipdate + 1 and l_receiptdate <= l_shipdate + 30);
ALTER TABLE lineitem ADD CONSTRAINT l_shipinstruct_domain CHECK
    (l_shipinstruct in ('DELIVER IN PERSON', 'COLLECT COD', 'NONE', 'TAKE BACK RETURN'));
ALTER TABLE lineitem ADD CONSTRAINT l_shipmode_domain CHECK
    (l_shipmode in ('REG AIR', 'AIR', 'RAIL', 'SHIP', 'TRUCK', 'MAIL', 'FOB'));


-- order
-- start by dropping constraints if they already exist
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_orderkey_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_orderstatus_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_orderdate_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_orderpriority_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_clerk_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_shippriority_domain;
ALTER TABLE orders DROP CONSTRAINT IF EXISTS o_orderyear_domain;
-- adding constraints
-- orderkey unique within [scalingfactor * 1,500,000 * 4], constraint based on sf of 0.1
ALTER TABLE orders ADD CONSTRAINT o_orderkey_domain CHECK
    (o_orderkey >= 1 and o_orderkey <= 600000);
ALTER TABLE orders ADD CONSTRAINT o_orderstatus_domain CHECK
    (o_orderstatus in ('F', 'O', 'P'));
ALTER TABLE orders ADD CONSTRAINT o_orderdate_domain CHECK
    (o_orderdate >= :STARTDATE and o_orderdate <= :ENDDATE - 151);
ALTER TABLE orders ADD CONSTRAINT o_orderpriority_domain CHECK
    (o_orderpriority in ('1-URGENT', '2-HIGH', '3-MEDIUM', '4-NOT SPECIFIED', '5-LOW'));
-- o_clerk constraint based on sf of 0.1
ALTER TABLE orders ADD CONSTRAINT o_clerk_domain CHECK
    (SPLIT_PART(o_clerk, '#', 1) = 'Clerk' and
     (CAST(SPLIT_PART(o_clerk, '#', 2) AS INTEGER) BETWEEN 1 and 1000) and
     LENGTH(SPLIT_PART(o_clerk, '#', 2)) = 9 and
     ARRAY_LENGTH(STRING_TO_ARRAY(o_clerk, '#'), 1) = 2);
ALTER TABLE orders ADD CONSTRAINT o_shippriority_domain CHECK
    (o_shippriority = 0);

ALTER TABLE ADD CONSTRAINT o_orderyear_domain CHECK (((o_orderyear >= 1992) AND (o_orderyear <= 1998)));

-- nation
-- start by dropping constraints if they already exist
ALTER TABLE nation DROP CONSTRAINT IF EXISTS n_nationkey_domain;
ALTER TABLE nation DROP CONSTRAINT IF EXISTS n_name_domain;
ALTER TABLE nation DROP CONSTRAINT IF EXISTS n_regionkey_domain;
ALTER TABLE nation DROP CONSTRAINT IF EXISTS n_nationkey_name_regionkey_domain;
-- adding constraints
ALTER TABLE nation ADD CONSTRAINT n_nationkey_domain CHECK
    (n_nationkey >= 0 and n_nationkey <= 24);
ALTER TABLE nation ADD CONSTRAINT n_name_domain CHECK
    (n_name in ('ETHIOPIA', 'IRAN', 'EGYPT', 'RUSSIA', 'SAUDI ARABIA',
                'INDONESIA', 'VIETNAM', 'GERMANY', 'PERU', 'FRANCE', 'ALGERIA', 'ROMANIA',
                'JORDAN', 'ARGENTINA', 'MOROCCO', 'CANADA', 'JAPAN', 'INDIA', 'UNITED KINGDOM',
                'UNITED STATES', 'MOZAMBIQUE', 'CHINA', 'BRAZIL', 'KENYA', 'IRAQ' ));
ALTER TABLE nation ADD CONSTRAINT n_regionkey_domain CHECK
    (n_regionkey >= 0 and n_regionkey <= 4);
-- (n_nationkey, n_name, n_regionkey) tuple constraint
ALTER TABLE nation ADD CONSTRAINT n_nationkey_name_regionkey_domain CHECK
    ((n_nationkey = 0 and n_name = 'ALGERIA' and n_regionkey = 0) or
     (n_nationkey = 1 and n_name = 'ARGENTINA' and n_regionkey = 1) or
     (n_nationkey = 2 and n_name = 'BRAZIL' and n_regionkey = 1) or
     (n_nationkey = 3 and n_name = 'CANADA' and n_regionkey = 1) or
     (n_nationkey = 4 and n_name = 'EGYPT' and n_regionkey = 4) or
     (n_nationkey = 5 and n_name = 'ETHIOPIA' and n_regionkey = 0) or
     (n_nationkey = 6 and n_name = 'FRANCE' and n_regionkey = 3) or
     (n_nationkey = 7 and n_name = 'GERMANY' and n_regionkey = 3) or
     (n_nationkey = 8 and n_name = 'INDIA' and n_regionkey = 2) or
     (n_nationkey = 9 and n_name = 'INDONESIA' and n_regionkey = 2) or
     (n_nationkey = 10 and n_name = 'IRAN' and n_regionkey = 4) or
     (n_nationkey = 11 and n_name = 'IRAQ' and n_regionkey = 4) or
     (n_nationkey = 12 and n_name = 'JAPAN' and n_regionkey = 2) or
     (n_nationkey = 13 and n_name = 'JORDAN' and n_regionkey = 4) or
     (n_nationkey = 14 and n_name = 'KENYA' and n_regionkey = 0) or
     (n_nationkey = 15 and n_name = 'MOROCCO' and n_regionkey = 0) or
     (n_nationkey = 16 and n_name = 'MOZAMBIQUE'and n_regionkey = 0) or
     (n_nationkey = 17 and n_name = 'PERU' and n_regionkey = 1) or
     (n_nationkey = 18 and n_name = 'CHINA' and n_regionkey = 2) or
     (n_nationkey = 19 and n_name = 'ROMANIA' and n_regionkey = 3) or
     (n_nationkey = 20 and n_name = 'SAUDI ARABIA' and n_regionkey = 4) or
     (n_nationkey = 21 and n_name = 'VIETNAM' and n_regionkey = 2) or
     (n_nationkey = 22 and n_name = 'RUSSIA' and n_regionkey = 3) or
     (n_nationkey = 23 and n_name = 'UNITED KINGDOM' and n_regionkey = 3) or
     (n_nationkey = 24 and n_name = 'UNITED STATES' and n_regionkey = 1));


-- customer
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_custkey_domain;
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_name_domain;
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_nationkey_domain;
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_phone_domain;
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_acctbal_domain;
ALTER TABLE customer DROP CONSTRAINT IF EXISTS c_mktsegment_domain;
-- cuskey unique within [scalingfactor * 150000], constraint based on sf of 0.1
ALTER TABLE customer ADD CONSTRAINT c_custkey_domain CHECK
    (c_custkey >= 1 and c_custkey <= 15000);
ALTER TABLE customer ADD CONSTRAINT c_name_domain CHECK
    (SPLIT_PART(c_name, '#', 1) = 'Customer' and
     CAST(SPLIT_PART(c_name, '#', 2) AS INTEGER) = c_custkey and
     LENGTH(SPLIT_PART(c_name, '#', 2)) = 9 and
     ARRAY_LENGTH(STRING_TO_ARRAY(c_name, '#'), 1) = 2);
ALTER TABLE customer ADD CONSTRAINT c_nationkey_domain CHECK
    (c_nationkey >= 0 and c_nationkey <= 24);
ALTER TABLE customer ADD CONSTRAINT c_phone_domain CHECK
    ((cast(substring(c_phone, 1,2) as int) = c_nationkey + 10) and
     (substring(c_phone, 3,1) = '-') and
     (cast(substring(c_phone, 4,3) as int) BETWEEN 100 and 999) and
     (substring(c_phone, 7,1) = '-') and
     (cast(substring(c_phone, 8,3) as int) BETWEEN 100 and 999) and
     (substring(c_phone, 11,1) = '-') and
     (cast(substring(c_phone, 12,4) as int) BETWEEN 1000 and 9999));
ALTER TABLE customer ADD CONSTRAINT c_acctbal_domain CHECK
    (c_acctbal >= -999.99 and c_acctbal <= 9999.99);
ALTER TABLE customer ADD CONSTRAINT c_mktsegment_domain CHECK
    (c_mktsegment in ('MACHINERY', 'AUTOMOBILE', 'BUILDING', 'FURNITURE', 'HOUSEHOLD'));


-- supplier
-- start by dropping constraints if they already exist
ALTER TABLE supplier DROP CONSTRAINT IF EXISTS s_suppkey_domain;
ALTER TABLE supplier DROP CONSTRAINT IF EXISTS s_name_domain;
ALTER TABLE supplier DROP CONSTRAINT IF EXISTS s_nationkey_domain;
ALTER TABLE supplier DROP CONSTRAINT IF EXISTS s_phone_domain;
ALTER TABLE supplier DROP CONSTRAINT IF EXISTS s_acctbal_domain;
-- adding constraints
-- suppkey unique within [scalingfactor * 10,000], constraint based on sf of 0.1
ALTER TABLE supplier ADD CONSTRAINT s_suppkey_domain CHECK
    (s_suppkey >= 1 and s_suppkey <= 1000);
ALTER TABLE supplier ADD CONSTRAINT s_name_domain CHECK
    (SPLIT_PART(s_name, '#', 1) = 'Supplier' and
     CAST(SPLIT_PART(s_name, '#', 2) AS INTEGER) = s_suppkey and
     LENGTH(SPLIT_PART(s_name, '#', 2)) = 9 and
     ARRAY_LENGTH(STRING_TO_ARRAY(s_name, '#'), 1) = 2);
ALTER TABLE supplier ADD CONSTRAINT s_nationkey_domain CHECK
    (s_nationkey >= 0 and s_nationkey <= 24);
ALTER TABLE supplier ADD CONSTRAINT s_phone_domain CHECK
    ((cast(substring(s_phone, 1,2) as int) = s_nationkey + 10) and
     (substring(s_phone, 3,1) = '-') and
     (cast(substring(s_phone, 4,3) as int) BETWEEN 100 and 999) and
     (substring(s_phone, 7,1) = '-') and
     (cast(substring(s_phone, 8,3) as int) BETWEEN 100 and 999) and
     (substring(s_phone, 11,1) = '-') and
     (cast(substring(s_phone, 12,4) as int) BETWEEN 1000 and 9999));
ALTER TABLE supplier ADD CONSTRAINT s_acctbal_domain CHECK
    (s_acctbal >= -999.99 and s_acctbal <= 9999.99);


-- part
-- start by dropping constraints if they already exist
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_partkey_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_name_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_mfgr_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_brand_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_type_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_size_domain;
ALTER TABLE part DROP CONSTRAINT IF EXISTS p_container_domain;
-- adding constraints
-- partkey unique within [scalingfactor * 200,000], constraint based on sf of 0.1
ALTER TABLE part ADD CONSTRAINT p_partkey_domain CHECK
    (p_partkey >= 1 and p_partkey <= 20000);

select ('''almond''', '''antique''', '''aquamarine''', '''azure''', '''beige''', '''bisque''', '''black''', '''blanched''', '''blue''',
        '''blush''', '''brown''', '''burlywood''', '''burnished''', '''chartreuse''', '''chiffon''', '''chocolate''', '''coral''',
        '''cornflower''', '''cornsilk''', '''cream''', '''cyan''', '''dark''', '''deep''', '''dim''', '''dodger''', '''drab''', '''firebrick''',
        '''floral''', '''forest''', '''frosted''', '''gainsboro''', '''ghost''', '''goldenrod''', '''green''', '''grey''', '''honeydew''',
        '''hot''', '''indian''', '''ivory''', '''khaki''', '''lace''', '''lavender''', '''lawn''', '''lemon''', '''light''', '''lime''', '''linen''',
        '''magenta''', '''maroon''', '''medium''', '''metallic''', '''midnight''', '''mint''', '''misty''', '''moccasin''', '''navajo''',
        '''navy''', '''olive''', '''orange''', '''orchid''', '''pale''', '''papaya''', '''peach''', '''peru''', '''pink''', '''plum''', '''powder''',
        '''puff''', '''purple''', '''red''', '''rose''', '''rosy''', '''royal''', '''saddle''', '''salmon''', '''sandy''', '''seashell''', '''sienna''',
        '''sky''', '''slate''', '''smoke''', '''snow''', '''spring''', '''steel''', '''tan''', '''thistle''', '''tomato''', '''turquoise''', '''violet''',
        '''wheat''', '''white''', '''yellow''') p_name_words \gset
ALTER TABLE part ADD CONSTRAINT p_name_domain CHECK
    (SPLIT_PART(p_name, ' ', 1) in :p_name_words and
     SPLIT_PART(p_name, ' ', 2) in :p_name_words and
     SPLIT_PART(p_name, ' ', 3) in :p_name_words and
     SPLIT_PART(p_name, ' ', 4) in :p_name_words and
     SPLIT_PART(p_name, ' ', 5) in :p_name_words and
     ARRAY_LENGTH(STRING_TO_ARRAY(p_name, ' '), 1) = 5);
ALTER TABLE part ADD CONSTRAINT p_mfgr_domain CHECK
    (SPLIT_PART(p_mfgr, '#', 1) = 'Manufacturer' and
     CAST(SPLIT_PART(p_mfgr, '#', 2) AS INTEGER) BETWEEN 1 and 5 and
     ARRAY_LENGTH(STRING_TO_ARRAY(p_mfgr, '#'), 1) = 2);
ALTER TABLE part ADD CONSTRAINT p_brand_domain CHECK
    (SPLIT_PART(p_brand, '#', 1) = 'Brand' and
     CAST(LEFT(SPLIT_PART(p_brand, '#', 2), 1) AS INTEGER) = CAST(SPLIT_PART(p_mfgr, '#', 2) AS INTEGER) and
     CAST(RIGHT(SPLIT_PART(p_brand, '#', 2), 1) AS INTEGER) BETWEEN 1 and 5 and
     LENGTH(SPLIT_PART(p_brand, '#', 2)) = 2 and
     ARRAY_LENGTH(STRING_TO_ARRAY(p_brand, '#'), 1) = 2);
ALTER TABLE part ADD CONSTRAINT p_type_domain CHECK
    (SPLIT_PART(p_type, ' ', 1) in ('ECONOMY', 'LARGE', 'MEDIUM', 'PROMO', 'SMALL', 'STANDARD') and
     SPLIT_PART(p_type, ' ', 2) in ('ANODIZED', 'BRUSHED', 'BURNISHED', 'PLATED', 'POLISHED') and
     SPLIT_PART(p_type, ' ', 3) in ('BRASS', 'COPPER', 'NICKEL', 'STEEL', 'TIN') and
     ARRAY_LENGTH(STRING_TO_ARRAY(p_type, ' '), 1) = 3);
ALTER TABLE part ADD CONSTRAINT p_size_domain CHECK
    (p_size >= 1 and p_size <= 50);
ALTER TABLE part ADD CONSTRAINT p_container_domain CHECK
    (SPLIT_PART(p_container, ' ', 1) in ('JUMBO', 'LG', 'MED', 'SM', 'WRAP') and
     SPLIT_PART(p_container, ' ', 2) in ('BAG', 'BOX', 'CAN', 'CASE', 'DRUM', 'JAR', 'PACK', 'PKG') and
     ARRAY_LENGTH(STRING_TO_ARRAY(p_container, ' '), 1) = 2);


-- partsupp
-- start by dropping constraints if they already exist
ALTER TABLE partsupp DROP CONSTRAINT IF EXISTS ps_availqty_domain;
ALTER TABLE partsupp DROP CONSTRAINT IF EXISTS ps_supplycost_domain;
-- adding constraints
ALTER TABLE partsupp ADD CONSTRAINT ps_availqty_domain CHECK
    (ps_availqty >= 1 and ps_availqty <= 9999);
ALTER TABLE partsupp ADD CONSTRAINT ps_supplycost_domain CHECK
    (ps_supplycost >= 1.00 and ps_supplycost <= 1000.00);


-- region
-- start by dropping constraints if they already exist
ALTER TABLE region DROP CONSTRAINT IF EXISTS r_regionkey_domain;
ALTER TABLE region DROP CONSTRAINT IF EXISTS r_name_domain;
ALTER TABLE region DROP CONSTRAINT IF EXISTS r_regionkey_name_domain;
-- adding constraints
ALTER TABLE region ADD CONSTRAINT r_regionkey_domain CHECK
    (r_regionkey >= 0 and r_regionkey <= 4);
ALTER TABLE region ADD CONSTRAINT r_name_domain CHECK
    (r_name in ('MIDDLE EAST', 'AMERICA', 'ASIA', 'EUROPE', 'AFRICA'));
-- (r_regionkey, r_name) tuple constraint
ALTER TABLE region ADD CONSTRAINT r_regionkey_name_domain CHECK
    ((r_regionkey = 0 and r_name = 'AFRICA') or
     (r_regionkey = 1 and r_name = 'AMERICA') or
     (r_regionkey = 2 and r_name = 'ASIA') or
     (r_regionkey = 3 and r_name = 'EUROPE') or
     (r_regionkey = 4 and r_name = 'MIDDLE EAST'));