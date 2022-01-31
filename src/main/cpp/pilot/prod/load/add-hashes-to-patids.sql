CREATE EXTENSION pgcrypto;

ALTER TABLE patient ADD COLUMN IF NOT EXISTS  hash INT;

-- generate a 32-bit integer based on hash
WITH hashed AS (SELECT pat_id, digest(pat_id::TEXT, 'sha256') hash FROM patient),
     extracted AS (SELECT pat_id,  hash,  (get_byte(hash, 31) |
                      (get_byte(hash, 30) << 8) |
                      (get_byte(hash, 29) << 16) |
                      (get_byte(hash, 28) << 24))::INT last_bytes
                  FROM hashed)
UPDATE patient p SET hash=CASE WHEN last_bytes < 0 THEN last_bytes * -1 ELSE last_bytes END
FROM extracted e
WHERE e.pat_id = p.pat_id;


