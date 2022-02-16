SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl
FROM patient
WHERE MOD(hash, :batch_cnt) = :my_batch
ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl
