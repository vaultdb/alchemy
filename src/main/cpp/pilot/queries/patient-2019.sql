SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
FROM patient
WHERE study_year=2019
ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
