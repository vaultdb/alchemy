 SELECT d.major_icd9, count(*) as cnt 
 FROM cdiff_cohort_diagnoses d 
 GROUP BY d.major_icd9 
 ORDER BY cnt DESC 
 LIMIT 10
