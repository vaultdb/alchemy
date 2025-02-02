CREATE ROLE public_attribute;
CREATE ROLE replicated;
CREATE ROLE partitioned_on;

GRANT SELECT(N_NATIONKEY) ON nation TO public_attribute;
GRANT SELECT(N_NAME) ON nation TO public_attribute;
GRANT SELECT(N_REGIONKEY) ON nation TO public_attribute;
GRANT SELECT(N_COMMENT) ON nation TO public_attribute;

GRANT SELECT(R_REGIONKEY) ON region TO public_attribute;
GRANT SELECT(R_NAME) ON region TO public_attribute;
GRANT SELECT(R_COMMENT) ON region TO public_attribute;

GRANT SELECT(P_PARTKEY) ON part TO public_attribute;
GRANT SELECT(P_NAME) ON part TO public_attribute;
GRANT SELECT(P_MFGR) ON part TO public_attribute;
GRANT SELECT(P_BRAND) ON part TO public_attribute;
GRANT SELECT(P_TYPE) ON part TO public_attribute;
GRANT SELECT(P_SIZE) ON part TO public_attribute;
GRANT SELECT(P_CONTAINER) ON part TO public_attribute;
GRANT SELECT(P_RETAILPRICE) ON part TO public_attribute;
GRANT SELECT(P_COMMENT) ON part TO public_attribute;

GRANT SELECT(PS_PARTKEY) ON partsupp TO public_attribute;
GRANT SELECT(PS_SUPPKEY) ON partsupp TO public_attribute;
GRANT SELECT(PS_SUPPLYCOST) ON partsupp TO public_attribute;

GRANT SELECT(S_SUPPKEY) ON supplier TO public_attribute;
GRANT SELECT(S_NAME) ON supplier TO public_attribute;
GRANT SELECT(S_NATIONKEY) ON supplier TO public_attribute;

GRANT SELECT(C_CUSTKEY) ON customer TO public_attribute;
GRANT SELECT(C_NATIONKEY) ON customer TO public_attribute;

GRANT SELECT(O_ORDERKEY) ON orders TO public_attribute;


-- declare replicated tables
GRANT SELECT ON part TO replicated;
GRANT SELECT ON region TO replicated;
GRANT SELECT ON nation TO replicated;

-- codify partitioning scheme
GRANT SELECT(c_nationkey) ON customer TO partitioned_on;

GRANT SELECT(o_custkey) ON orders TO partitioned_on;

GRANT SELECT(l_orderkey) ON lineitem TO partitioned_on;

GRANT SELECT(s_nationkey) ON supplier TO partitioned_on;

GRANT SELECT(ps_suppkey) ON partsupp TO partitioned_on;



