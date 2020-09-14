-- need to delete foreign key reference constraint to allow for distributed joins
-- use unioned database for true constraints

-- delete this integrity constraint to prevent cascading deletes from "spilling over" fact table when we delete by nation
ALTER TABLE lineitem DROP CONSTRAINT lineitem_l_partkey_l_suppkey_fkey;
DELETE FROM customer WHERE c_nationkey > 12;
DELETE FROM supplier WHERE s_nationkey > 12;


ANALYZE;

