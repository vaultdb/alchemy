WITH all_diagnoses AS (
    (SELECT * FROM mi_cohort_diagnoses)
    UNION ALL
    (SELECT * FROM remote_mi_cohort_diagnoses)
), all_medications AS (
    (SELECT * FROM mi_cohort_medications)
    UNION ALL
    (SELECT * FROM remote_mi_cohort_medications)
)
SELECT COUNT(DISTINCT d.patient_id) as rx_cnt
FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id 
WHERE lower(m.medication) like '%aspirin%' AND d.icd9 like '414%' AND d.timestamp_ <= m.timestamp_