WITH all_diagnoses AS (
    (SELECT * FROM diagnoses WHERE year=2006 AND site=4)
    UNION ALL
    (SELECT * FROM diagnoses WHERE year=2006 AND site=5)
), all_medications AS (
    (SELECT * FROM medications WHERE year=2006 AND site=4)
    UNION ALL
    (SELECT * FROM medications WHERE year=2006 AND site=5)
), all_vitals AS (
    (SELECT * FROM medications WHERE year=2006 AND site=4)
    UNION ALL
    (SELECT * FROM medications WHERE year=2006 AND site=5)
), dd AS (SELECT DISTINCT patient_id, gender, race
            FROM demographics)
SELECT gender, race, count(*)
FROM dd, all_diagnoses d, all_vitals v, all_medications m
WHERE lower(m.medication) like '%aspirin%' 
    AND d.icd9 like '414%' 
    AND dd.patient_id = m.patient_id
    AND d.patient_id = v.patient_id 
    AND m.patient_id = d.patient_id
GROUP BY gender, race;