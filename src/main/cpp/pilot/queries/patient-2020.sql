SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
FROM patient
WHERE study_year=2020
ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
