WITH demo AS (SELECT DISTINCT patient_id, gender, race
            FROM demographics)
SELECT gender, race, count(*)
FROM demo, mi_cohort_diagnoses d, mi_cohort_vitals v, mi_cohort_medications m
WHERE lower(m.medication) like '%aspirin%' 
    AND d.icd9 like '414%' 
    AND demo.patient_id = m.patient_id
    AND d.patient_id = v.patient_id 
    AND m.patient_id = d.patient_id
GROUP BY gender, race