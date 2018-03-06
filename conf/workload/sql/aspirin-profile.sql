SELECT gender, race, count(*)
FROM mi_cohort_diagnoses d, demographics demo, mi_cohort_medications m
WHERE lower(m.medication) like '%aspirin%' 
    AND d.icd9 like '414%' 
    AND d.patient_id = m.patient_id
    AND d.patient_id = demo.patient_id
GROUP BY gender, race