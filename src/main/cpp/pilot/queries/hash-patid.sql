CREATE EXTENSION IF NOT EXISTS pgcrypto;

WITH hashed AS (SELECT patid, digest(patid::TEXT, 'sha256') hash FROM patient),
     extracted AS (SELECT patid,  hash,  (get_byte(hash, 31) |
                      (get_byte(hash, 30) << 8) |
                      (get_byte(hash, 29) << 16) |
                      (get_byte(hash, 28) << 24))::INT last_bytes
                  FROM hashed)
SELECT patid, last_bytes, MOD(CASE WHEN last_bytes < 0 THEN last_bytes * -1 ELSE last_bytes END, 10) partition_id
FROM extracted
ORDER BY patid
LIMIT 20;

