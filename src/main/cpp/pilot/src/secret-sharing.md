## Schema for Secret Sharing CSVs
VaultDB can accept inline schema definitions for ad-hoc queries.  This is similar to [BigTable's](https://cloud.google.com/bigquery/docs/external-table-definition#use-inline-schema) design.

### Warmup: Filenames
If you are providing the engine a plaintext CSV, such as one from a full-stack data partner (who runs the MPC protocols), the engine will expect a file with the suffix ".csv" and an additional file with the extension ".schema".  For example in our ENRICH pilot, we would have `enrich-patients.csv` and `enrich-patients.schema`.

If you are sending the engine a secret-shared file, please give it the extension `.alice` (for the generator) and `.bob` for the evaluator.  In our running example, a data partner would send `enrich-patients.alice` to the generator.

When sending one of these files (CSV or .alice/bob) please accompany it with a `.schema` file that denotes the column layout.  

### Schema Format


VaultDB can parse ad-hoc schemas for use in query processing.  It accepts schemas in the form:
```
(col1_name:col1_type,col2_name:col2_type,...,colN_name:colN:type)
```

For example, the pilot's plaintext schema for patients was:

```(study_year:int32, pat_id:int32, age_strata:varchar(1), sex:varchar(1),  ethnicity:varchar(1), race:varchar(1), numerator:bool, denom_excl:bool)```

If your input is secret-shared elsewhere (i.e., it has the suffix `.alice` or `.bob`), then the input types should have the prefix `shared-`.  Thus our Enrich secret shares will have this schema:

```(study_year:shared-int32, pat_id:shared-int32, age_strata:shared-varchar(1), sex:shared-varchar(1),  ethnicity:shared-varchar(1), race:shared-varchar(1), numerator:shared-bool, denom_excl:shared-bool)```

Supported Types:
* `bool`
* `int32`
* `int64`
* `float`
* `date`
* `varchar` (string) 
* `shared-bool`
* `shared-int32`
* `shared-int64`
* `shared-float`
* `shared-varchar`

The parser accepts an upper bound on string length in parentheses.  Thus defining `race:varchar(1)` means that the engine expects an 8-bit string.

All inputs to VaultDB are byte-aligned.  For example, if you send a `shared-bool` then we assume it will take one byte per row.  The engine will take the most significant bit in this case as the bool and discard the rest.

All row entries must be of the same security level.  It will not run if some columns are `shared` and others are not.  For example, this schema will get rejected:
```(study_year:shared-int32, pat_id:int32,...```

  ### Dummy Tags

  In some circumstances, the data partner may wish to pad their inputs.  For example, if they provide partially aggregated data (like counting the patients in each strata locally) they may wish to pad their results to the full domain (all strata).  The engine has a reserved word to denote that the last column is our dummy tag. If you name your last column `dummy_tag` then the engine will treat this as tombstone for the row and disregard any rows that have it set to `true` in its computation.  Of course, it skips the dummies obliviously.

  ***Limitations***: The dummy tag must always be the last column of input.  Also, it must always be a bool and the bool must be compatible with the security level of the other row entries.

  The running example with a dummy tag column:
  ```
  (study_year:int32, pat_id:int32, age_strata:varchar(1), sex:varchar(1),  ethnicity:varchar(1), race:varchar(1), numerator:bool, denom_excl:bool, dummy_tag:bool)```
```
  
  
