-- requires sf to be initialized with suffix
\set alice_db 'tpch_alice_':sf
\set bob_db 'tpch_bob_':sf

ALTER TABLE lineitem ADD COLUMN IF NOT EXISTS alice_store BOOL DEFAULT NULL;
UPDATE lineitem SET alice_store=NULL; -- in case of repeat test

UPDATE lineitem l SET alice_store=TRUE
                FROM :alice_db.lineitem l2
                WHERE l.l_orderkey = l2.l_orderkey AND l.l_linenumber = l2.l_linenumber;

UPDATE lineitem l SET alice_store=FALSE
                FROM :bob_db.lineitem l2
                WHERE l.l_orderkey = l2.l_orderkey AND l.l_linenumber = l2.l_linenumber;


ALTER TABLE supplier ADD COLUMN IF NOT EXISTS alice_store BOOL DEFAULT NULL;
UPDATE supplier SET alice_store=NULL; -- in case of repeat test

UPDATE supplier SET alice_store=TRUE WHERE s_suppkey
   IN (SELECT s_suppkey FROM :alice_db.supplier);

UPDATE supplier SET alice_store=FALSE WHERE s_suppkey
   IN (SELECT s_suppkey FROM :bob_db.supplier);

ALTER TABLE partsupp ADD COLUMN IF NOT EXISTS alice_store BOOL DEFAULT NULL;
UPDATE partsupp SET alice_store=NULL; -- in case of repeat test

UPDATE partsupp SET alice_store=TRUE WHERE (ps_suppkey, ps_partkey)
   IN (SELECT ps_suppkey, ps_partkey FROM :alice_db.partsupp);

UPDATE partsupp SET alice_store=FALSE WHERE (ps_suppkey, ps_partkey)
   IN (SELECT ps_suppkey, ps_partkey FROM :bob_db.partsupp);


ALTER TABLE orders ADD COLUMN IF NOT EXISTS alice_store BOOL DEFAULT NULL;
UPDATE orders SET alice_store=NULL; -- in case of repeat test

UPDATE orders SET alice_store=TRUE WHERE o_orderkey
   IN (SELECT o_orderkey FROM :alice_db.orders);

UPDATE orders SET alice_store=FALSE WHERE o_orderkey
   IN (SELECT o_orderkey FROM :bob_db.orders);


ALTER TABLE customer ADD COLUMN IF NOT EXISTS alice_store BOOL DEFAULT NULL;
UPDATE customer SET alice_store=NULL; -- in case of repeat test

UPDATE customer SET alice_store=TRUE WHERE c_custkey
   IN (SELECT c_custkey FROM :alice_db.orders);

UPDATE customer SET alice_store=FALSE WHERE c_custkey
   IN (SELECT c_custkey FROM :bob_db.orders);
