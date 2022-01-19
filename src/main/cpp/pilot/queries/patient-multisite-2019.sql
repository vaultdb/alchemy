SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
FROM patient
WHERE multisite and study_year = 2019
ORDER BY pat_id