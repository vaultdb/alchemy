DROP TABLE IF EXISTS unit_test;

CREATE TABLE unit_test(
                          id serial PRIMARY KEY,
                          test_suite varchar,
                          test_name varchar,
                          mpc bool,
                          storage_model varchar DEFAULT 'row',
                          party varchar,
                          source_file varchar,
                          branch varchar,
                          test_time timestamp,
                          runtime_ms BIGINT
);

\copy unit_test(test_suite, test_name, mpc, storage_model, party, source_file, branch, test_time, runtime_ms) FROM 'log/all-entries.csv' WITH DELIMITER ',';

UPDATE unit_test SET test_name='tpchQ01Sort' WHERE test_name='tpchQ1Sort';
UPDATE unit_test SET test_name='tpchQ03Sort' WHERE test_name='tpchQ3Sort';
UPDATE unit_test SET test_name='tpchQ05Sort' WHERE test_name='tpchQ5Sort';
UPDATE unit_test SET test_name='tpchQ08Sort' WHERE test_name='tpchQ8Sort';
UPDATE unit_test SET test_name='tpchQ09Sort' WHERE test_name='tpchQ9Sort';
UPDATE unit_test SET test_suite='SecureKeyedJoinTest' WHERE test_suite='SecurePkeyFkeyJoinTest';

UPDATE unit_test SET test_name='tpch_q01' WHERE test_name='tpch_q1';
UPDATE unit_test SET test_name='tpch_q03' WHERE test_name='tpch_q3';
UPDATE unit_test SET test_name='tpch_q05' WHERE test_name='tpch_q5';
UPDATE unit_test SET test_name='tpch_q08' WHERE test_name='tpch_q8';
UPDATE unit_test SET test_name='tpch_q09' WHERE test_name='tpch_q9';

-- delete first test, it is mostly waiting for Alice and Bob to sync up / connect after building
DELETE FROM unit_test WHERE test_name='emp_test_int' AND test_suite='EmpTest';

SELECT DISTINCT test_suite FROM unit_test ORDER BY test_suite;

-- disregard known anomaly
DELETE FROM unit_test WHERE test_suite='SecureKeyedJoinTest' AND test_time::DATE='2023-06-14'::DATE;


\set baseline_timestamp '''2023-06-01 15:01:07'''::timestamp


 WITH latest_measurement AS (SELECT 'after' trial, max(test_time) test_time FROM unit_test),
      before_after AS ((SELECT * FROM latest_measurement) UNION ALL (SELECT 'before', :baseline_timestamp)),
     selection AS (SELECT test_suite, test_name, trial, runtime_ms
                   FROM unit_test u JOIN before_after ba ON u.test_time = ba.test_time
                   WHERE party='bob' AND -- test_suite='SecureTpcHTest' AND
                       mpc 
                   ORDER BY test_name, u.test_time)
SELECT s1.test_suite, s1.test_name, s1.runtime_ms before_runtime_ms, s2.runtime_ms after_runtime_ms, s1.runtime_ms::FLOAT / s2.runtime_ms perf_ratio, s1.runtime_ms - s2.runtime_ms delta_ms
FROM selection s1 JOIN selection s2 ON s1.test_name = s2.test_name AND s1.test_suite=s2.test_suite
WHERE s1.trial='before' AND s2.trial='after'
ORDER BY perf_ratio DESC;
