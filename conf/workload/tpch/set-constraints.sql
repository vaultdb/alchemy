--STARTDATE = 1992-01-01 CURRENTDATE = 1995-06-17 ENDDATE = 1998-12-31
\set STARTDATE '''1992-01-01'''::DATE
\set ENDDATE '''1998-12-31'''::DATE
--lineitem

ALTER TABLE lineitem ADD CONSTRAINT l_returnflag_domain CHECK (l_returnflag in ('R', 'A', 'N'));
ALTER TABLE lineitem ADD CONSTRAINT l_linestatus_domain CHECK (l_linestatus in ('O', 'F'));
ALTER TABLE lineitem ADD CONSTRAINT l_discount_domain CHECK (l_discount >= 0.00 and l_discount <= 0.10);
ALTER TABLE lineitem ADD CONSTRAINT l_tax_domain CHECK (l_tax>= 0.00 and l_tax <= 0.08);
ALTER TABLE lineitem ADD CONSTRAINT l_quantity_domain CHECK (l_quantity>= 1 and l_quantity <= 50);

-- order
-- orderkey unique within [scalingfactor * 1,500,000 * 4]
-- constraint based on sf of 0.1
ALTER TABLE orders ADD CONSTRAINT o_orderkey_domain CHECK (o_orderkey >= 1 and o_orderkey <= 600000);
ALTER TABLE orders ADD CONSTRAINT o_orderdate_domain
CHECK (o_orderdate >= :STARTDATE and o_orderdate <= :ENDDATE);