WITH all_diagnoses AS (
    (SELECT * FROM diagnoses WHERE year=2006 AND site=4)
    UNION ALL
    (SELECT * FROM diagnoses WHERE year=2006 AND site=5)
)
SELECT d.major_icd9, count(*) as cnt 
FROM all_diagnoses d 
GROUP BY d.major_icd9 
ORDER BY count(*) DESC 
LIMIT 10