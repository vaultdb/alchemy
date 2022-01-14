\c enrich_htn_prod
\set ECHO all

-- run this after pilot/prod/load/alliance/clean.sh

\i 'pilot/prod/load/alliance/load-demographics.sql'
\i 'pilot/prod/load/alliance/load-population-labels.sql'
\i 'pilot/prod/load/alliance/assemble-patient-table.sql'
