SELECT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl
FROM patient
WHERE :selection
ORDER BY study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl;
