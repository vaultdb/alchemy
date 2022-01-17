

dropdb  --if-exists enrich_htn_unioned_3pc
createdb enrich_htn_unioned_3pc


cd pilot/test
psql enrich_htn_unioned < output/enrich_htn_unioned.sql

