BEGIN;

	CREATE TABLE PART (

		P_PARTKEY		SERIAL PRIMARY KEY,
		P_NAME			VARCHAR(55),
		P_MFGR			CHAR(25),
		P_BRAND			CHAR(10),
		P_TYPE			VARCHAR(25),
		P_SIZE			INTEGER,
		P_CONTAINER		CHAR(10),
		P_RETAILPRICE	DECIMAL,
		P_COMMENT		VARCHAR(23)
	);

	COPY part FROM '/tmp/dss/data/part.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE REGION (
		R_REGIONKEY	SERIAL PRIMARY KEY,
		R_NAME		CHAR(25),
		R_COMMENT	VARCHAR(152)
	);

	COPY region FROM '/tmp/dss/data/region.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE NATION (
		N_NATIONKEY		SERIAL PRIMARY KEY,
		N_NAME			CHAR(25),
		N_REGIONKEY		INTEGER,  -- references R_REGIONKEY
		N_COMMENT		VARCHAR(152)
	);

	COPY nation FROM '/tmp/dss/data/nation.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE SUPPLIER (
		S_SUPPKEY		SERIAL PRIMARY KEY,
		S_NAME			CHAR(25),
		S_ADDRESS		VARCHAR(40),
		S_NATIONKEY		INTEGER, -- references N_NATIONKEY
		S_PHONE			CHAR(15),
		S_ACCTBAL		DECIMAL,
		S_COMMENT		VARCHAR(101)
	);

	COPY supplier FROM '/tmp/dss/data/supplier.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE CUSTOMER (
		C_CUSTKEY		SERIAL PRIMARY KEY,
		C_NAME			VARCHAR(25),
		C_ADDRESS		VARCHAR(40),
		C_NATIONKEY		INTEGER, -- references N_NATIONKEY
		C_PHONE			CHAR(15),
		C_ACCTBAL		DECIMAL,
		C_MKTSEGMENT	CHAR(10),
		C_COMMENT		VARCHAR(117)
	);

	COPY customer FROM '/tmp/dss/data/customer.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE PARTSUPP (
		PS_PARTKEY		INTEGER, -- references P_PARTKEY
		PS_SUPPKEY		INTEGER, -- references S_SUPPKEY
		PS_AVAILQTY		INTEGER,
		PS_SUPPLYCOST		DECIMAL,
		PS_COMMENT		VARCHAR(199)
	);

COPY partsupp FROM '/tmp/dss/data/partsupp.csv' WITH (FORMAT csv, DELIMITER '|');
-- sometimes when we scale down partsupp generates dupes                                                                                                                                                                                             -- delete the duplicate (partkey, suppkey) rows                                                                                                                                                                                                      
CREATE TABLE deduped AS (
SELECT
  *
FROM (
  SELECT
    ROW_NUMBER() OVER (PARTITION BY ps_partkey, ps_suppkey ORDER BY ps_availqty) AS r,
    partsupp.*
  FROM partsupp) ps
WHERE
  ps.r < 2);

DROP TABLE partsupp;
ALTER TABLE deduped RENAME TO partsupp;

-- only works after dedupe
ALTER TABLE PARTSUPP ADD PRIMARY KEY (PS_PARTKEY, PS_SUPPKEY);


COMMIT;

BEGIN;

	CREATE TABLE ORDERS (
		O_ORDERKEY		SERIAL PRIMARY KEY,
		O_CUSTKEY		INTEGER, -- references C_CUSTKEY
		O_ORDERSTATUS	CHAR(1),
		O_TOTALPRICE	DECIMAL,
		O_ORDERDATE		DATE,
		O_ORDERPRIORITY	CHAR(15),
		O_CLERK			CHAR(15),
		O_SHIPPRIORITY	INTEGER,
		O_COMMENT		VARCHAR(79)
	);

	COPY orders FROM '/tmp/dss/data/orders.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;

BEGIN;

	CREATE TABLE LINEITEM (
		L_ORDERKEY		INTEGER, -- references O_ORDERKEY
		L_PARTKEY		INTEGER, -- references P_PARTKEY (compound fk to PARTSUPP)
		L_SUPPKEY		INTEGER, -- references S_SUPPKEY (compound fk to PARTSUPP)
		L_LINENUMBER	INTEGER,
		L_QUANTITY		DECIMAL,
		L_EXTENDEDPRICE	DECIMAL,
		L_DISCOUNT		DECIMAL,
		L_TAX			DECIMAL,
		L_RETURNFLAG	CHAR(1),
		L_LINESTATUS	CHAR(1),
		L_SHIPDATE		DATE,
		L_COMMITDATE	DATE,
		L_RECEIPTDATE	DATE,
		L_SHIPINSTRUCT	CHAR(25),
		L_SHIPMODE		CHAR(10),
		L_COMMENT		VARCHAR(44)
	);
	ALTER TABLE LINEITEM ADD PRIMARY KEY (L_ORDERKEY, L_LINENUMBER);

	COPY lineitem FROM '/tmp/dss/data/lineitem.csv' WITH (FORMAT csv, DELIMITER '|');

COMMIT;
