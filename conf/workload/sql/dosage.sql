SELECT DISTINCT d.patient_id 
FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id 
WHERE lower(m.medication) like '%aspirin%'
  AND d.icd9 like '459%'
  AND m.dosage like '%325 MG%'