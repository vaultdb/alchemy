SELECT  pat_id, min(age_strata), sex, ethnicity, race, numerator, denom_excl
FROM patient
WHERE multisite
GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl
ORDER BY pat_id