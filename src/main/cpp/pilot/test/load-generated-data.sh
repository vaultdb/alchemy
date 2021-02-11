

dropdb  --if-exists enrich_htn_unioned
createdb enrich_htn_unioned


cd pilot/test
psql enrich_htn_unioned < output/enrich_htn_unioned.sql

