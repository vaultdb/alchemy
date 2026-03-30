WITH diags AS ( -- 86400 seconds in a day
 SELECT patient_id, EXTRACT(EPOCH FROM timestamp_)/86400 day_, row_number()  OVER (PARTITION BY patient_id ORDER BY timestamp_) AS r
 FROM cdiff_cohort_diagnoses
 WHERE icd9 = '008.45')
SELECT DISTINCT d1.patient_id
       FROM diags d1 JOIN diags d2 ON d1.patient_id = d2.patient_id
       WHERE (d2.day_ -  d1.day_) >=  15 AND (d2.day_ -  d1.day_) <= 56  AND d1.r+1 = d2.r
