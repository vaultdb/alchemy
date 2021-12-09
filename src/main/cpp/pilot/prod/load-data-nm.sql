-- stitch it all together into a single DB for validation
\c enrich_htn_prod
\set ECHO all

\i 'pilot/prod/load-population-labels-nm.sql'
\i 'pilot/prod/load-demographics-nm.sql'
\i 'pilot/prod/assemble-patient-table-nm.sql'



