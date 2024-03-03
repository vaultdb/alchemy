# Catalyst PHAME Workflow 

This document described the tables each data partner will contribute to the Catalyst study.  This study will reproduce and extend the [PHAME](https://phame.uic.edu) study at UIC.  

There are two types of data partners participating in this study.  They are:
* Row-level: These sites will contribute patient-level data that we will aggregate within MPC.
* Aggregate-level: They contribute partially aggregated data that we will include in our analysis.

We detail the input tables from each data partner how we're going to compute over them in MPC below. 

Caveats: for now treating all counts as `int64`s for simplicity.  I am sure there are more to come here.

## Population-Level Statistics

All sites will contribute the following population-level statistics:
* Total Unique Patients at Site
* Total Unique Patients before Randomization
* Total Unique Patients after Randomization
* Female - Age between 18-30
* Male - Age between 18-30
* Female - Age between 31-40
* Male - Age between 31-40
* Female - Age between 41-50
* Male - Age between 41-50
* Female - Age between 51-60
* Male - Age between 51-60
* Female - Age between 61-70
* Male - Age between 61-70
* Female - Age > 70
* Male - Age > 70

These inputs will arrive sorted in this order and we will take the simple sum over all sites.

All of our other statistics pertain only to participants who are in the randomly selected cohort. 

**Table name**: `phame_cohort_counts`\
**Schema**: `(desc:varchar(42), count:int64, site_id:int32)`\
**Filename**: `phame_cohort_counts.csv`\
**Study table**: A1\
**Partners**: all

## Demographics

We start with patient-level demographic data (aka the denominator).  Demographic variables in inputdata are coded as follows:
* `age_cat` $\in [0, 6]$
* `gender` $\in$ {M, F, N, 0}
* `race` $\in [0, 9]$
* `ethnicity` $\in [0, 6]$
* `zip_code` (5-digit string)
* `payer_primary` $\in [0, 6]$
* `payer_secondary`  $\in [0, 6]$


We'll join this with `phame_diagnosis` later to get the rollup under MPC.

**Schema**: `phame_demographic(patid:int32, age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1))`\
**Filename**: `phame_demographic.csv`\
**Study table**: B1\
**Partners**: row-level

### Denominator Rollup
This table aggregates over individuals who meet the study criteria of having one admit date within the study period.  These individuals are all over the age of 18 and within the Chicago-area zip codes in the study (`study_population` in the protocol).

Participants also needed a `cap_id` to qualify for the denominator.  We are not completing that part because we are not doing PPRL in this first round.

This table stratifies by zip code and gives overall statistics of the sample.  We will add these up for each zip over all sites.  

This table is a bit tricky because it seems to partition by race and ethnicity interchangably as if they will add up to the total when ethnicity and race are modeled in the CDM as independent variables. 

We may want to include a preprocessing step that pads the secret shares so that all of the zip codes are covered.  Thus we could just concatenate the partial counts one line at a time and add them up.  Otherwise we sort and sum the partial counts.


## Diagnosis

This is the numerator for the study.
**Table name**: `phame_diagnosis`\
**Schema**: `(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool)`\
**Filename**: `phame_diagnosis.csv`\
**Study table**: B2\
**Partners**: row-level

### Diagnosis Rollup

We will sum the counts of each diagnosis over all sites. This is stratified by all of the demographic variables coded as in the [denominator](#demographics) table.

**Table name**: `phame_diagnosis_rollup`\
**Schema**: `(age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)`\
**Filename**: `phame_diagnosis_rollup.csv`\
Study table: N/A, this will take the place of the B5-B16 tables\
**Partners**: all


## Output Tables and Next Steps

For each age category and gender, we will compute stratified counts by zip code, race, and ethnicity.  They seem to be doing this in the study independant of diagnosis codes.

We will also stratify by dx codes.  If we start with:\
`phame_rollup(age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)`

and we union these together for all sites, we will compute:
```sql
SELECT age_cat, gender, race, ethnicity, zip, SUM(patient_cnt) AS patient_cnt, SUM(diabetes_cnt) AS diabetes_cnt, SUM(hypertension_cnt) AS hypertension_cnt, SUM(cervical_cancer_cnt) AS cervical_cancer_cnt, SUM(breast_cancer_cnt) AS breast_cancer_cnt, SUM(lung_cancer_cnt) AS lung_cancer_cnt, SUM(colorectal_cancer_cnt) AS colorectal_cancer_cnt 
FROM phame_rollup 
GROUP BY age_cat, gender, race, ethnicity, zip;
```

Our other major output is the population-level statistics over the denominator.  This is a simple sum over all sites using the `phame_cohort_counts` table (described in [Population-Level Statistics](#population-level-statistics)).

Implementation:
* Start with data generator for testing the system.  Generate these 4 tables automatically for several parties.
* Implement these tests using JSON plan format.