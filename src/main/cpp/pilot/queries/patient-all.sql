SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
FROM patient
WHERE multisite 
ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl