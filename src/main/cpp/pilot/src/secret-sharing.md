## Schema for Secret Sharing CSVs to VaultDB
VaultDB can accept inline schema definitions for ad-hoc queries.  This is inspired by [BigTable's](https://cloud.google.com/bigquery/docs/external-table-definition#use-inline-schema) design.

### Warmup: Filenames
If you are providing the engine a plaintext CSV, such as one from a full-stack data partner (who runs the MPC protocols), the engine will expect a file with the suffix ".csv" and an additional file with the extension ".schema".  For example in our ENRICH pilot, we would have `enrich-patients.csv` and `enrich-patients.schema`.

If you are sending the engine a secret-shared file, please give it the extension `.alice` (for the generator) and `.bob` for the evaluator.  In our running example, a data partner would send `enrich-patients.alice` to the generator.

When sending one of these files (CSV or .alice/bob) please accompany it with a `.schema` file that denotes the column layout.  

### Schema Format


VaultDB can parse ad-hoc schemas for use in query processing with the following format:
```
(col1_name:col1_type,col2_name:col2_type,...,colN_name:colN:type)
```

For example, the pilot's plaintext schema for patients was:

```
(study_year:int32, pat_id:int32, age_strata:varchar(1), sex:varchar(1),  ethnicity:varchar(1), race:varchar(1), numerator:bool, denom_excl:bool)
```

If your input is secret-shared elsewhere (i.e., it has the suffix `.alice` or `.bob`), then the input types should have the prefix `shared-`.  Thus our Enrich secret shares will have this schema:

```
(study_year:shared-int32, pat_id:shared-int32, age_strata:shared-varchar(1), sex:shared-varchar(1),  ethnicity:shared-varchar(1), race:shared-varchar(1), numerator:shared-bool, denom_excl:shared-bool)
```

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

For column names, you can optionally give them table names too - separated with a period.  For example, if you give us the column name `patient.patient_id` then the engine will parse this as having a table name of patient and a column name of patient_id.  Keeping track of this provenance - the relationship between the common data model / schema and the CSVs can be helpful for debugging.

***Code in action***: You can check out an example of this schema parsing facility in [secret_share_csv](https://github.com/vaultdb/vaultdb-core/blob/emp-operators/src/main/cpp/pilot/src/secret_share_csv.cpp).  We also demo it in the [CSVReader unit test](https://github.com/vaultdb/vaultdb-core/blob/emp-operators/src/main/cpp/test/csv_reader_test.cpp).

  #### Dummy Tags

Sometimes a data partner may wish to pad their inputs.  For example, if they provide partially aggregated data (like counting the patients in each strata locally) they may wish to pad their results to the full domain (all strata).  The engine has a reserved word to denote that the last column is our dummy tag. If you name your last column `dummy_tag` then the engine will treat this as tombstone for the row and disregard any rows that have it set to `true` in its computation.  Of course, it skips the dummies obliviously.


The running example with a dummy tag column:
  ```
  (study_year:int32, ..., denom_excl:bool, dummy_tag:bool)
```

  ***Limitations***: The dummy tag must always be the last column of input.  Also, it must always be a bool and the bool must be compatible with the security level of the other row entries.


### OK, but what does this look like in bits?

Say we have our running example schema:
```(study_year:shared-int32, pat_id:shared-int32, age_strata:shared-varchar(1), sex:shared-varchar(1),  ethnicity:shared-varchar(1), race:shared-varchar(1), numerator:shared-bool, denom_excl:shared-bool)```

and we receive an input row: `(2018, 1, 6, M, N, 3, true, true, false)`. In binary, we'd expect our input to be this:
```
01000111 11100000 00000000 00000000 10000000 00000000 00000000 00000000 01101100 10110010 01110010 11001100 10000000 10000000 00000000
```
In other words, 2018 will be `01000111 11100000 00000000 00000000`, the patient ID will be `10000000 00000000 00000000 00000000` and so on.  Note that our bools are byte-aligned - e.g., we represent `true` as `10000000`.  



  
  
