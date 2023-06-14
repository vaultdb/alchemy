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

SELECT DISTINCT test_suite FROM unit_test ORDER BY test_suite;

WITH selection AS (SELECT test_suite, test_name,
                          CASE WHEN (test_time='2023-05-21 21:04'::timestamp) THEN 'before' ELSE 'after' END trial,
                          runtime_ms
                   FROM unit_test
                   WHERE -- test_suite='SecureTpcHTest' AND
                       mpc AND test_time IN ('2023-05-21 21:04'::timestamp, '2023-06-13 18:02:05'::timestamp)
                   ORDER BY test_name, test_time)
SELECT s1.test_suite, s1.test_name, s1.runtime_ms before_runtime_ms, s2.runtime_ms after_runtime_ms, s1.runtime_ms::FLOAT / s2.runtime_ms ratio, s1.runtime_ms - s2.runtime_ms delta_ms
FROM selection s1 JOIN selection s2 ON s1.test_name = s2.test_name AND s1.test_suite=s2.test_suite
WHERE s1.trial='before' AND s2.trial='after'
ORDER BY s1.test_suite, s1.test_name;