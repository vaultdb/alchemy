--Part 1
/************************************************************************************************/
/*                                                                                              */
/* Lead PI: Dr. Sage Kim, Dr.Sanjib Basu                                                        */
/* Query Name: CAP_PHAME PART ONE                                                               */
/* Summary:  This query aims to identify and characterize counts of patients with diabetes,     */
/*            hypertension, cervical, breast, lung and colorectal cancer diagnoses              */
/* Developed by University of Illinois, Chicago - CCTS BI-Core Team                             */
/* Date Created: 05/30/2023                                                                     */
/*                                                                                              */
/* Tables Required:                                                                             */
/*                                                                                              */
/* PCORnet Tables                                                                               */                                                                          
/*  a) [ENCOUNTER]                                                                              */
/* CAPriCORN Tables                                                                             */
/*  b) [CAP_DEMOGRAPHICS]                                                                       */
/*                                                                                              */
/* Output Tables: [PHAME_TOKEN.csv]                                                             */
/*                                                                                              */
/************************************************************************************************/

/********-----------------------------  IMPORTANT  ---------------------------------*************/
/* Modify below lines based on instructions provided at corresponding line                      */
/* 1) Line 31                                                                                   */
/* 2) Line 45                                                                                   */
/* 3) Line 155                                                                                  */
/* 4) Line 166                                                                                  */
/*----------------------------------------------------------------------------------------------*/


use [pcv6_2023r2]; --<-- specify database name containing PCORnet CDM tables


DECLARE @STARTStudy date; 		SET @STARTStudy = '01/01/2015';
DECLARE @ENDStudy date; 		SET @ENDStudy = '12/31/2021';

IF OBJECT_ID('tempdb..#study_population') IS NOT NULL DROP TABLE #study_population;
WITH popln AS (
  
   SELECT DISTINCT demo.CAP_ID,
                   CAST(DATEDIFF(dd,demo.BIRTH_DATE, enc.DT)/365.23076923074 AS INT) AS AGE, 
                   demo.SEX,
				   demo.RACE,
				   demo.ZIP
      FROM  CAP_DEMOGRAPHICS demo        --------------<------------  Specify the database name containing CAP_DEMOGRAPHICS table
	  INNER JOIN (SELECT DISTINCT PATID, MAX(ADMIT_DATE) as DT 
	               FROM ENCOUNTER
				    WHERE ADMIT_DATE BETWEEN  @STARTStudy and  @ENDStudy
	                GROUP BY PATID
					) enc
	  ON demo.CAP_ID = enc.PATID
	  WHERE demo.ZIP in ('60004','60077','60163','60411','60463','60558','60625','60654','60005','60078','60164','60412','60464','60601','60626','60655',
                         '60006','60082','60165','60415','60465','60602','60628','60656','60007','60089','60168','60419','60466','60603','60629','60657',
                         '60008','60090','60169','60422','60467','60604','60630','60659','60009','60091','60171','60423','60469','60605','60631','60660',
                         '60010','60093','60172','60425','60471','60606','60632','60661','60015','60094','60173','60426','60472','60607','60633','60664',
                         '60016','60095','60176','60428','60473','60608','60634','60666','60017','60103','60192','60429','60475','60609','60636','60680',
                         '60018','60104','60193','60430','60476','60610','60637','60681','60022','60107','60194','60438','60477','60611','60638','60690',
                         '60025','60120','60195','60439','60478','60612','60639','60691','60026','60126','60201','60443','60480','60613','60640','60701',
                         '60029','60130','60202','60445','60482','60614','60641','60706','60043','60131','60203','60452','60487','60615','60642','60707',
                         '60053','60133','60204','60453','60499','60616','60643','60712','60056','60141','60301','60454','60501','60617','60644','60714',
                         '60062','60153','60302','60455','60513','60618','60645','60803','60065','60154','60303','60456','60521','60619','60646','60804',
                         '60067','60155','60304','60457','60525','60620','60647','60805','60068','60159','60305','60458','60526','60621','60649','60827',
                         '60070','60160','60402','60459','60527','60622','60651','60074', '60161','60406','60461','60534','60623','60652','60076','60162',
                         '60409','60462','60546','60624','60653' )
	  AND demo.SEX != 'NI'
	  AND demo.ZIP is NOT NULL
   )

   SELECT *,
          CASE 
		       WHEN AGE BETWEEN 18 AND 30 THEN '01'
			   WHEN AGE BETWEEN 31 AND 40 THEN '02'
			   WHEN AGE BETWEEN 41 AND 50 THEN '03'
			   WHEN AGE BETWEEN 51 AND 60 THEN '04'
			   WHEN AGE BETWEEN 61 AND 70 THEN '05'
			   WHEN AGE > 70 THEN '06'
		  END AS AGE_CAT
	 INTO #study_population
     FROM popln
	 WHERE AGE >= 18
;


IF OBJECT_ID('tempdb..#percent_tab') IS NOT NULL DROP TABLE #percent_tab;
WITH cte AS (
 SELECT  (
         SELECT CAST(ROUND(COUNT(DISTINCT PATID) * 0.2, 0) AS INT) 
		   FROM DEMOGRAPHIC
          ) AS CDM_20,
		 (
         SELECT COUNT(DISTINCT CAP_ID) 
		   FROM #study_population
         ) AS STUDY_N
)

SELECT 
       CASE 
          WHEN CDM_20 = STUDY_N THEN 100
		  WHEN STUDY_N < CDM_20 THEN 100
		  WHEN STUDY_N > CDM_20 THEN CAST(ROUND((CAST(CDM_20 AS FLOAT) / CAST(STUDY_N AS FLOAT) * 100),0) AS INT)
		END AS _percent
INTO #percent_tab
FROM cte
;


IF OBJECT_ID('tempdb..#randomly_stratified') IS NOT NULL DROP TABLE #randomly_stratified;
WITH stratified AS (
    
	SELECT *,
	       ROW_NUMBER() OVER(PARTITION BY SEX, AGE_CAT ORDER BY NEWID()) as SqNum,
		   COUNT(*) OVER (PARTITION BY SEX, AGE_CAT) as cnt
	 FROM #study_population

), cnts as (
    
	SELECT DISTINCT SEX, AGE_CAT, cnt, cnt/100 * (SELECT _percent FROM #percent_tab) as lmt  FROM stratified
),

random_c as (
   
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '01' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '01')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '01' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '01')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '02' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '02')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '02' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '02')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '03' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '03')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '03' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '03')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '04' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '04')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '04' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '04')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '05' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '05')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '05' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '05')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'F' AND AGE_CAT = '06' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'F' AND AGE_CAT = '06')
   UNION ALL
   SELECT DISTINCT CAP_ID, SEX, AGE_CAT  FROM stratified WHERE SEX = 'M' AND AGE_CAT = '06' AND SqNum <= (SELECT lmt FROM cnts WHERE  SEX = 'M' AND AGE_CAT = '06')
) 

SELECT DISTINCT CAP_ID, SEX, AGE_CAT INTO #randomly_stratified FROM random_c
;
--
-- select * from #randomly_stratified;


-- Table A1 (workplan)
----- Return Summary Counts to PI

SELECT *, 'test' as SITE_ID
INTO #PHAME_COHORT_COUNTS
FROM (            ------------<------------      Specify Site Name
   SELECT 'Total Unique Patients at Site' as Attribute, COUNT(DISTINCT PATID) n FROM dbo.DEMOGRAPHIC
   UNION ALL
   SELECT 'Total Unique Patients before Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #study_population
   UNION ALL
   SELECT 'Total Unique Patients after Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified
   UNION ALL
   SELECT 'Female - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '01'
   UNION ALL
   SELECT 'Male - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '01'
   UNION ALL
   SELECT 'Female - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '02'
   UNION ALL
   SELECT 'Male - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '02'
   UNION ALL
   SELECT 'Female - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '03'
   UNION ALL
   SELECT 'Male - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '03'
   UNION ALL
   SELECT 'Female - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '04'
   UNION ALL
   SELECT 'Male - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '04'
   UNION ALL
   SELECT 'Female - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '05'
   UNION ALL
   SELECT 'Male - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '05'
   UNION ALL
   SELECT 'Female - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '06'
   UNION ALL
   SELECT 'Male - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '06'
) a
;  -- save output as PHAME_COHORT_COUNTS.csv

------------------ END -------------------------
--PART 2
/************************************************************************************************/
/*                                                                                              */
/* Lead PI: Dr. Sage Kim, Dr.Sanjib Basu                                                        */
/* Query Name: CAP_PHAME PART ONE                                                               */
/* Summary:  This query aims to identify and characterize counts of patients with diabetes,     */
/*            hypertension, cervical, breast, lung and colorectal cancer diagnoses              */
/* Developed by University of Illinois, Chicago - CCTS BI-Core Team                             */
/* Date Created: 05/30/2023                                                                     */
/*                                                                                              */
/* Tables Required:                                                                             */
/* PCORnet Tables                                                                               */
/*  a) [DEMOGRAPHIC]                                                                            */
/*  b) [ENCOUNTER]                                                                              */
/*  c) [DIAGNOSIS]                                                                              */
/* CAPriCORN Tables                                                                             */
/*  b) [CAP_DEMOGRAPHICS]                                                                       */
/*                                                                                              */
/************************************************************************************************/

/********-----------------------------  IMPORTANT  ---------------------------------*************/
/* Modify below lines based on instructions provided at corresponding line                      */
/* 1) Line 29                                                                                   */
/* 3) Line 312                                                                                  */
/* 4) Line 386                                                                                  */
/*----------------------------------------------------------------------------------------------*/


use pcv6_2023r2; --<-- specify database name containing PCORnet CDM tables


IF OBJECT_ID('tempdb..#phame_dx') IS NOT NULL DROP TABLE #phame_dx;
CREATE TABLE #phame_dx (
	    [code]		    VARCHAR(20),
		[label]	        VARCHAR(40)
	);

INSERT INTO #phame_dx
(code, label) 
VALUES
('E11.00','DM'),
('E11.01','DM'),
('E11.10','DM'),
('E11.11','DM'),
('E11.21','DM'),
('E11.22','DM'),
('E11.29','DM'),
('E11.311','DM'),
('E11.319','DM'),
('E11.3211','DM'),
('E11.3212','DM'),
('E11.3213','DM'),
('E11.3219','DM'),
('E11.3291','DM'),
('E11.3292','DM'),
('E11.3293','DM'),
('E11.3299','DM'),
('E11.3311','DM'),
('E11.3312','DM'),
('E11.3313','DM'),
('E11.3319','DM'),
('E11.3391','DM'),
('E11.3392','DM'),
('E11.3393','DM'),
('E11.3399','DM'),
('E11.3411','DM'),
('E11.3412','DM'),
('E11.3413','DM'),
('E11.3419','DM'),
('E11.3491','DM'),
('E11.3492','DM'),
('E11.3493','DM'),
('E11.3499','DM'),
('E11.3511','DM'),
('E11.3512','DM'),
('E11.3513','DM'),
('E11.3519','DM'),
('E11.3521','DM'),
('E11.3522','DM'),
('E11.3523','DM'),
('E11.3529','DM'),
('E11.3531','DM'),
('E11.3532','DM'),
('E11.3533','DM'),
('E11.3539','DM'),
('E11.3541','DM'),
('E11.3542','DM'),
('E11.3543','DM'),
('E11.3549','DM'),
('E11.3551','DM'),
('E11.3552','DM'),
('E11.3553','DM'),
('E11.3559','DM'),
('E11.3591','DM'),
('E11.3592','DM'),
('E11.3593','DM'),
('E11.3599','DM'),
('E11.36','DM'),
('E11.37X1','DM'),
('E11.37X2','DM'),
('E11.37X3','DM'),
('E11.37X9','DM'),
('E11.39','DM'),
('E11.40','DM'),
('E11.41','DM'),
('E11.42','DM'),
('E11.43','DM'),
('E11.44','DM'),
('E11.49','DM'),
('E11.51','DM'),
('E11.52','DM'),
('E11.59','DM'),
('E11.610','DM'),
('E11.618','DM'),
('E11.620','DM'),
('E11.621','DM'),
('E11.622','DM'),
('E11.628','DM'),
('E11.630','DM'),
('E11.638','DM'),
('E11.641','DM'),
('E11.649','DM'),
('E11.65','DM'),
('E11.69','DM'),
('E11.8','DM'),
('E11.9','DM'),
('I10.','HTN'),
('I11.0','HTN'),
('I11.9','HTN'),
('I12.0','HTN'),
('I12.9','HTN'),
('I13.0','HTN'),
('I13.10','HTN'),
('I13.11','HTN'),
('I13.2','HTN'),
('I15.0','HTN'),
('I15.1','HTN'),
('I15.2','HTN'),
('I15.8','HTN'),
('I15.9','HTN'),
('I16.0','HTN'),
('I16.1','HTN'),
('I16.9','HTN'),
('R03.0','HTN'),
('R03.1','HTN'),
('Y93.I1','HTN'),
('C43.52','BC'),
('C44.501','BC'),
('C44.591','BC'),
('C4A.52','BC'),
('C50','BC'),
('C50.011','BC'),
('C50.012','BC'),
('C50.019','BC'),
('C50.021','BC'),
('C50.022','BC'),
('C50.029','BC'),
('C50.1','BC'),
('C50.11','BC'),
('C50.111','BC'),
('C50.112','BC'),
('C50.119','BC'),
('C50.12','BC'),
('C50.121','BC'),
('C50.122','BC'),
('C50.129','BC'),
('C50.2','BC'),
('C50.21','BC'),
('C50.211','BC'),
('C50.212','BC'),
('C50.219','BC'),
('C50.22','BC'),
('C50.221','BC'),
('C50.222','BC'),
('C50.229','BC'),
('C50.3','BC'),
('C50.31','BC'),
('C50.311','BC'),
('C50.312','BC'),
('C50.319','BC'),
('C50.32','BC'),
('C50.321','BC'),
('C50.322','BC'),
('C50.329','BC'),
('C50.4','BC'),
('C50.41','BC'),
('C50.411','BC'),
('C50.412','BC'),
('C50.419','BC'),
('C50.42','BC'),
('C50.421','BC'),
('C50.422','BC'),
('C50.429','BC'),
('C50.5','BC'),
('C50.51','BC'),
('C50.511','BC'),
('C50.512','BC'),
('C50.519','BC'),
('C50.52','BC'),
('C50.521','BC'),
('C50.522','BC'),
('C50.529','BC'),
('C50.6','BC'),
('C50.61','BC'),
('C50.611','BC'),
('C50.612','BC'),
('C50.619','BC'),
('C50.62','BC'),
('C50.621','BC'),
('C50.622','BC'),
('C50.629','BC'),
('C50.8','BC'),
('C50.81','BC'),
('C50.811','BC'),
('C50.812','BC'),
('C50.819','BC'),
('C50.82','BC'),
('C50.821','BC'),
('C50.822','BC'),
('C50.829','BC'),
('C50.9','BC'),
('C50.91','BC'),
('C50.911','BC'),
('C50.912','BC'),
('C50.919','BC'),
('C50.92','BC'),
('C50.921','BC'),
('C50.922','BC'),
('C50.929','BC'),
('C79.81','BC'),
('C84.7A','BC'),
('D03.52','BC'),
('D05','BC'),
('D05.0','BC'),
('D05.00','BC'),
('D05.01','BC'),
('D05.02','BC'),
('D05.1','BC'),
('D05.10','BC'),
('D05.11','BC'),
('D05.12','BC'),
('D05.8','BC'),
('D05.80','BC'),
('D05.81','BC'),
('D05.82','BC'),
('D05.9','BC'),
('D05.90','BC'),
('D05.91','BC'),
('D05.92','BC'),
('C34','LC'),
('C34.1','LC'),
('C34.10','LC'),
('C34.11','LC'),
('C34.12','LC'),
('C34.2','LC'),
('C34.3','LC'),
('C34.30','LC'),
('C34.31','LC'),
('C34.32','LC'),
('C34.8','LC'),
('C34.80','LC'),
('C34.81','LC'),
('C34.82','LC'),
('C34.9','LC'),
('C34.90','LC'),
('C34.91','LC'),
('C34.92','LC'),
('C46.5','LC'),
('C46.50','LC'),
('C46.51','LC'),
('C46.52','LC'),
('C78.0','LC'),
('C78.00','LC'),
('C78.01','LC'),
('C78.02','LC'),
('C7A.090','LC'),
('D02.2','LC'),
('D02.20','LC'),
('D02.21','LC'),
('D02.22','LC'),
('D3A.090','LC'),
('C18','CLC'),
('C18.2','CLC'),
('C18.4','CLC'),
('C18.6','CLC'),
('C18.7','CLC'),
('C18.8','CLC'),
('C18.9','CLC'),
('C7A.022','CLC'),
('C7A.023','CLC'),
('C7A.024','CLC'),
('C7A.025','CLC'),
('D01.0','CLC'),
('D3A.022','CLC'),
('D3A.023','CLC'),
('D3A.024','CLC'),
('D3A.025','CLC'),
('C53','CVC'),
('C53.0','CVC'),
('C53.1','CVC'),
('C53.8','CVC'),
('C53.9','CVC'),
('R87.614','CVC'),
('D06','CVC'),
('D06.0','CVC'),
('D06.1','CVC'),
('D06.7','CVC'),
('D06.9','CVC')
;


-- /*Please load Mraia matched file into a temprory table #Mraia_Matches */
-- --IF OBJECT_ID('tempdb..#phame_capids') IS NOT NULL DROP TABLE #phame_capids;
-- --CREATE TABLE #phame_capids (
-- --   CAP_ID  VARCHAR(128) NOT NULL
-- --)
-- --;
--
-- --BULK INSERT #phame_capids
-- --FROM 'cap_ids.csv' --tokens file path
-- --WITH (
-- --    FIRSTROW = 2,
-- 	--FIELDTERMINATOR = ',',
-- --	ROWTERMINATOR = '\n'
-- --);
-- select CAP_ID
-- into #phame_capids
-- from #hash
-- ;

--DECLARE @STARTStudy date; 		SET @STARTStudy = '01/01/2015';
--DECLARE @ENDStudy date; 		SET @ENDStudy = '12/31/2021';

-- denominator group AKA everyone
IF OBJECT_ID('tempdb..#phame_demo') IS NOT NULL DROP TABLE #phame_demo;
SELECT DISTINCT demo.CAP_ID as PATID,
                CAST(DATEDIFF(dd,demo.BIRTH_DATE, enc_dt.DT)/365.23076923074 AS INT) AS AGE, 
				CASE 
				     WHEN demo.SEX = 'M' THEN 'Male'
					 WHEN demo.SEX = 'F' THEN 'Female'
					 ELSE 'Sex:Other'
			    END AS GENDER,
				CASE 
				     WHEN demo.RACE = '01' THEN 'American  Indian  or  Alaska  Native'
					 WHEN demo.RACE = '02' THEN 'Asian'
					 WHEN demo.RACE = '03' THEN 'Black  or  African  American'
					 WHEN demo.RACE = '04' THEN 'Native  Hawaiian  or  Other  Pacific  Islander'
					 WHEN demo.RACE = '05' THEN 'White'
					 WHEN demo.RACE = '06' THEN 'Multiple  race'
					 WHEN demo.RACE = '07' THEN 'Refuse  to  answer'
					 WHEN demo.RACE = 'NI' THEN 'No  information'
					 WHEN demo.RACE = 'UN' THEN 'Unknown'
					 WHEN demo.RACE = 'OT' THEN 'Other'
					 ELSE NULL
                END AS RACE,
				CASE 
				     WHEN demo.HISPANIC = 'Y' THEN 'Hispanic or Latino'
					 WHEN demo.HISPANIC = 'N' THEN 'Not Hispanic or Latino'
					 WHEN demo.HISPANIC = 'R' THEN 'Refuse  to  answer'
					 WHEN demo.HISPANIC = 'NI' THEN 'No  information'
					 WHEN demo.HISPANIC = 'UN' THEN 'Unknown'
					 WHEN demo.HISPANIC = 'OT' THEN 'Other'
					 ELSE NULL
				END AS HISPANIC,
				ZIP, 
				CASE 
                     WHEN enc_ins.PAYER_TYPE_PRIMARY like ('1%') THEN 'Medicare'
		             WHEN enc_ins.PAYER_TYPE_PRIMARY like ('2%') THEN 'Medicaid'
		             WHEN enc_ins.PAYER_TYPE_PRIMARY like ('5%') THEN 'Private/Commercial'
			         WHEN enc_ins.PAYER_TYPE_PRIMARY like ('6%') THEN 'Private/Commercial'
			         WHEN enc_ins.PAYER_TYPE_PRIMARY = 'NI' THEN 'No  information' 
					 WHEN enc_ins.PAYER_TYPE_PRIMARY = 'OT' THEN 'Other'
			         WHEN enc_ins.PAYER_TYPE_PRIMARY = 'UN' THEN 'Unknown'
					 WHEN enc_ins.PAYER_TYPE_PRIMARY is NULL THEN NULL
			         ELSE 'Other'
		        END AS PAYER_PRIMARY,
	            CASE 
                     WHEN enc_ins.PAYER_TYPE_SECONDARY like ('1%') THEN 'Medicare'
					 WHEN enc_ins.PAYER_TYPE_SECONDARY like ('2%') THEN 'Medicaid'
		             WHEN enc_ins.PAYER_TYPE_SECONDARY like ('5%') THEN 'Private/Commercial'
			         WHEN enc_ins.PAYER_TYPE_SECONDARY like ('6%') THEN 'Private/Commercial'
			         WHEN enc_ins.PAYER_TYPE_SECONDARY = 'NI' THEN 'No  information' 
					 WHEN enc_ins.PAYER_TYPE_SECONDARY = 'OT' THEN 'Other'
			         WHEN enc_ins.PAYER_TYPE_SECONDARY = 'UN' THEN 'Unknown'
					 WHEN enc_ins.PAYER_TYPE_SECONDARY is NULL THEN NULL
					 ELSE 'Other'
		        END AS PAYER_SECONDARY
	INTO #phame_demo
    FROM [capv6_2023r2].[dbo].CAP_DEMOGRAPHICS demo --------------<------------  Specify the database name containing CAP_DEMOGRAPHICS table
	INNER JOIN (
	             SELECT DISTINCT PATID, MIN(ADMIT_DATE) as DT 
	               FROM dbo.ENCOUNTER enc
				    WHERE ADMIT_DATE BETWEEN  @STARTStudy and  @ENDStudy
-- 					AND EXISTS ( SELECT 1 FROM #phame_capids a WHERE enc.PATID = a.CAP_ID)
					and exists ( select 1 from #randomly_stratified rs where enc.PATID = rs.CAP_ID )--only the random sample
	                GROUP BY PATID
			    ) enc_dt
	ON demo.CAP_ID = enc_dt.PATID
    LEFT JOIN (
	            SELECT DISTINCT PATID, PAYER_TYPE_PRIMARY, PAYER_TYPE_SECONDARY  
	              FROM ( 
				    SELECT DISTINCT PATID, PAYER_TYPE_PRIMARY, PAYER_TYPE_SECONDARY,
								   ROW_NUMBER() OVER(PARTITION BY PATID ORDER BY ADMIT_DATE DESC) n
	                FROM dbo.ENCOUNTER enc
				     WHERE ADMIT_DATE BETWEEN @STARTStudy and  @ENDStudy
					 AND (PAYER_TYPE_PRIMARY IS NOT NULL OR PAYER_TYPE_SECONDARY IS NOT NULL)
--					 --AND EXISTS
--					 --  ( SELECT 1 FROM #phame_capids a WHERE enc.PATID = a.CAP_ID )
					and exists ( select 1 from #randomly_stratified rs where enc.PATID = rs.CAP_ID )--only the random sample
					 ) b
				 WHERE n = 1
		       ) enc_ins
	 ON demo.CAP_ID = enc_ins.PATID
	 WHERE demo.ZIP in  ('60004','60077','60163','60411','60463','60558','60625','60654','60005','60078','60164','60412','60464','60601','60626','60655',
                         '60006','60082','60165','60415','60465','60602','60628','60656','60007','60089','60168','60419','60466','60603','60629','60657',
                         '60008','60090','60169','60422','60467','60604','60630','60659','60009','60091','60171','60423','60469','60605','60631','60660',
                         '60010','60093','60172','60425','60471','60606','60632','60661','60015','60094','60173','60426','60472','60607','60633','60664',
                         '60016','60095','60176','60428','60473','60608','60634','60666','60017','60103','60192','60429','60475','60609','60636','60680',
                         '60018','60104','60193','60430','60476','60610','60637','60681','60022','60107','60194','60438','60477','60611','60638','60690',
                         '60025','60120','60195','60439','60478','60612','60639','60691','60026','60126','60201','60443','60480','60613','60640','60701',
                         '60029','60130','60202','60445','60482','60614','60641','60706','60043','60131','60203','60452','60487','60615','60642','60707',
                         '60053','60133','60204','60453','60499','60616','60643','60712','60056','60141','60301','60454','60501','60617','60644','60714',
                         '60062','60153','60302','60455','60513','60618','60645','60803','60065','60154','60303','60456','60521','60619','60646','60804',
                         '60067','60155','60304','60457','60525','60620','60647','60805','60068','60159','60305','60458','60526','60621','60649','60827',
                         '60070','60160','60402','60459','60527','60622','60651','60074', '60161','60406','60461','60534','60623','60652','60076','60162',
                         '60409','60462','60546','60624','60653')
;

------------ PHAME DIAGNOSIS ----------------------------

IF OBJECT_ID('tempdb..#phame_diag_ins') IS NOT NULL DROP TABLE #phame_diag_ins;
SELECT DISTINCT diag.PATID,
                COALESCE(diag.ADMIT_DATE, diag.DX_DATE) as DX_DT,
                diag.DX,
				codes.label,
                CASE 
                     WHEN enc.PAYER_TYPE_PRIMARY like ('1%') THEN 'Medicare'
		             WHEN enc.PAYER_TYPE_PRIMARY like ('2%') THEN 'Medicaid'
		             WHEN enc.PAYER_TYPE_PRIMARY like ('5%') THEN 'Private/Commercial'
			         WHEN enc.PAYER_TYPE_PRIMARY like ('6%') THEN 'Private/Commercial'
			         WHEN enc.PAYER_TYPE_PRIMARY = 'NI' THEN 'No  information' 
					 WHEN enc.PAYER_TYPE_PRIMARY = 'OT' THEN 'Other'
			         WHEN enc.PAYER_TYPE_PRIMARY = 'UN' THEN 'Unknown'
					 WHEN enc.PAYER_TYPE_PRIMARY is NULL THEN NULL
			         ELSE 'Other'
		        END AS PAYER_PRIMARY,
	            CASE 
                     WHEN enc.PAYER_TYPE_SECONDARY like ('1%') THEN 'Medicare'
					 WHEN enc.PAYER_TYPE_SECONDARY like ('2%') THEN 'Medicaid'
		             WHEN enc.PAYER_TYPE_SECONDARY like ('5%') THEN 'Private/Commercial'
			         WHEN enc.PAYER_TYPE_SECONDARY like ('6%') THEN 'Private/Commercial'
			         WHEN enc.PAYER_TYPE_SECONDARY = 'NI' THEN 'No  information' 
					 WHEN enc.PAYER_TYPE_SECONDARY = 'OT' THEN 'Other'
			         WHEN enc.PAYER_TYPE_SECONDARY = 'UN' THEN 'Unknown'
					 WHEN enc.PAYER_TYPE_SECONDARY is NULL THEN NULL
					 ELSE 'Other'
		        END AS PAYER_SECONDARY
  INTO #phame_diag_ins
  FROM dbo.DIAGNOSIS diag
  INNER JOIN #phame_dx codes ON diag.DX = codes.code
  INNER JOIN dbo.ENCOUNTER enc ON diag.ENCOUNTERID = enc.ENCOUNTERID
  WHERE enc.ADMIT_DATE  BETWEEN @STARTStudy and  @ENDStudy
  --AND EXISTS ( SELECT 1 FROM #phame_capids a WHERE diag.PATID = a.CAP_ID)
	and exists ( select 1 from #randomly_stratified rs where diag.PATID = rs.CAP_ID )--only the random sample

IF OBJECT_ID('tempdb..#phame_diagnosis') IS NOT NULL DROP TABLE #phame_diagnosis;
SELECT DISTINCT dx_pats.PATID,
                demo.ZIP,
				CASE 
				    WHEN DM.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_DIABETES,
				CASE 
				    WHEN DM.DX_DIABETES_DT IS NOT NULL THEN DX_DIABETES_DT ELSE NULL
				END AS DX_DIABETES_DT,
				CASE 
				    WHEN HTN.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_HYPERTENSION,
				CASE 
				    WHEN HTN.DX_HYPERTENSION_DT IS NOT NULL THEN DX_HYPERTENSION_DT ELSE NULL
				END AS DX_HYPERTENSION_DT,
				CASE 
				    WHEN CVC.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_CERVICAL_CANCER,
				CASE 
				    WHEN CVC.DX_CERVICAL_CANCER_DT IS NOT NULL THEN DX_CERVICAL_CANCER_DT ELSE NULL
				END AS DX_CERVICAL_CANCER_DT,
				CASE 
				    WHEN BC.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_BREAST_CANCER,
				CASE 
				    WHEN BC.DX_BREAST_CANCER_DT IS NOT NULL THEN DX_BREAST_CANCER_DT ELSE NULL
				END AS DX_BREAST_CANCER_DT,
				CASE 
				    WHEN LC.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_LUNG_CANCER,
				CASE 
				    WHEN LC.DX_LUNG_CANCER_DT IS NOT NULL THEN DX_LUNG_CANCER_DT ELSE NULL
				END AS DX_LUNG_CANCER_DT,
				CASE 
				    WHEN CLC.label IS NOT NULL THEN 1 ELSE 0
				END AS DX_COLORECTAL_CANCER,
				CASE 
				    WHEN CLC.DX_COLORECTAL_CANCER_DT IS NOT NULL THEN DX_COLORECTAL_CANCER_DT ELSE NULL
				END AS DX_COLORECTAL_CANCER_DT
   INTO #phame_diagnosis
   FROM #phame_diag_ins dx_pats
   LEFT JOIN #phame_demo demo
   ON demo.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_DIABETES_DT FROM #phame_diag_ins WHERE LABEL = 'DM'
	  GROUP BY PATID, LABEL ) DM
   ON DM.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_HYPERTENSION_DT FROM #phame_diag_ins WHERE LABEL = 'HTN'
	 GROUP BY PATID, LABEL ) HTN
   ON HTN.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_CERVICAL_CANCER_DT FROM #phame_diag_ins WHERE LABEL = 'CVC'
	 GROUP BY PATID, LABEL ) CVC
   ON CVC.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_BREAST_CANCER_DT FROM #phame_diag_ins WHERE LABEL = 'BC'
	 GROUP BY PATID, LABEL ) BC
   ON BC.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_LUNG_CANCER_DT FROM #phame_diag_ins WHERE LABEL = 'LC'
	 GROUP BY PATID, LABEL ) LC
   ON LC.PATID = dx_pats.PATID
   LEFT JOIN 
   ( SELECT PATID, LABEL, MIN(DX_DT) as DX_COLORECTAL_CANCER_DT FROM #phame_diag_ins WHERE LABEL = 'CLC'
	 GROUP BY PATID, LABEL ) CLC
   ON CLC.PATID = dx_pats.PATID

;
-- Table B4
------------ PHAME DENOMINATOR ----------------------------

IF OBJECT_ID('tempdb..#phame_denominator') IS NOT NULL DROP TABLE #phame_denominator;
SELECT ZIP,
       COUNT(PATID) AS TOTAL_N_POP,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_BLACK_POP,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_WHITE_POP,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_HISPANIC_POP,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_ASIAN_POP,
	   COUNT(CASE WHEN RACE='Black  or  African  American' AND GENDER='Male' THEN 1 ELSE NULL END) AS N_BLACK_MALE_POP,
	   COUNT(CASE WHEN RACE='White' AND GENDER='Male' THEN 1 ELSE NULL END) AS N_WHITE_MALE_POP,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Male' THEN 1 ELSE NULL END) AS N_HISPANIC_MALE_POP,
	   COUNT(CASE WHEN RACE='Asian' AND GENDER='Male' THEN 1 ELSE NULL END) AS N_ASIAN_MALE_POP,
	   COUNT(CASE WHEN RACE='Black  or  African  American' AND GENDER='Female' THEN 1 ELSE NULL END) AS N_BLACK_FEMALE_POP,
	   COUNT(CASE WHEN RACE='White' AND GENDER='Female' THEN 1 ELSE NULL END) AS N_WHITE_FEMALE_POP,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Female' THEN 1 ELSE NULL END) AS N_HISPANIC_FEMALE_POP,
	   COUNT(CASE WHEN RACE='Asian' AND GENDER='Female' THEN 1 ELSE NULL END) AS N_ASIAN_FEMALE_POP
  INTO #phame_denominator
  FROM #phame_demo
  GROUP BY ZIP
  ORDER BY TOTAL_N_POP DESC
;

--------------------- FEMALE BY ZIP CODE, AGE COUNTS ----------------

-- a) Female, Age 18-30
IF OBJECT_ID('tempdb..#phame_female_age_18_30') IS NOT NULL DROP TABLE #phame_female_age_18_30;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_18_30,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_18_30
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE BETWEEN 18 AND 30
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_18_30 DESC
;

-- b) Female, Age 31-40
IF OBJECT_ID('tempdb..#phame_female_age_31_40') IS NOT NULL DROP TABLE #phame_female_age_31_40;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_31_40,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_31_40
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE BETWEEN 31 AND 40
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_31_40 DESC
;

-- c) Female, Age 41-50
IF OBJECT_ID('tempdb..#phame_female_age_41_50') IS NOT NULL DROP TABLE #phame_female_age_41_50;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_41_50,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_41_50
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE BETWEEN 41 AND 50
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_41_50 DESC
;

-- d) Female, Age 51-60
IF OBJECT_ID('tempdb..#phame_female_age_51_60') IS NOT NULL DROP TABLE #phame_female_age_51_60;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_51_60,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_51_60
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE BETWEEN 51 AND 60
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_51_60 DESC
;

-- d) Female, Age 61-70
IF OBJECT_ID('tempdb..#phame_female_age_61_70') IS NOT NULL DROP TABLE #phame_female_age_61_70;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_61_70,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_61_70
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE BETWEEN 61 AND 70
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_61_70 DESC
;

-- d) Female, Age > 70
IF OBJECT_ID('tempdb..#phame_female_age_gt_70') IS NOT NULL DROP TABLE #phame_female_age_gt_70;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_gt_70,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_female_age_gt_70
  FROM #phame_demo
  WHERE GENDER = 'Female'
  AND AGE > 70
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_gt_70 DESC
;


--------------------- MALE BY ZIP CODE, AGE COUNTS ----------------

-- a) Male, Age 18-30
IF OBJECT_ID('tempdb..#phame_male_age_18_30') IS NOT NULL DROP TABLE #phame_male_age_18_30;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_18_30,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_18_30
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE BETWEEN 18 AND 30
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_18_30 DESC
;

-- b) Male, Age 31-40
IF OBJECT_ID('tempdb..#phame_male_age_31_40') IS NOT NULL DROP TABLE #phame_male_age_31_40;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_31_40,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_31_40
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE BETWEEN 31 AND 40
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_31_40 DESC
;

-- c) Male, Age 41-50
IF OBJECT_ID('tempdb..#phame_male_age_41_50') IS NOT NULL DROP TABLE #phame_male_age_41_50;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_41_50,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_41_50
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE BETWEEN 41 AND 50
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_41_50 DESC
;

-- d) Male, Age 51-60
IF OBJECT_ID('tempdb..#phame_male_age_51_60') IS NOT NULL DROP TABLE #phame_male_age_51_60;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_51_60,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_51_60
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE BETWEEN 51 AND 60
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_51_60 DESC
;

-- d) Male, Age 61-70
IF OBJECT_ID('tempdb..#phame_male_age_61_70') IS NOT NULL DROP TABLE #phame_male_age_61_70;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_61_70,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_61_70
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE BETWEEN 61 AND 70
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_61_70 DESC
;

-- d) Male, Age > 70
IF OBJECT_ID('tempdb..#phame_male_age_gt_70') IS NOT NULL DROP TABLE #phame_male_age_gt_70;
SELECT ZIP,
       COUNT(PATID) AS N_POP_AGE_BTW_gt_70,
	   COUNT(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE NULL END) AS N_PTS_BLACK,
	   COUNT(CASE WHEN RACE='White' THEN 1 ELSE NULL END) AS N_PTS_WHITE,
	   COUNT(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE NULL END) AS N_PTS_HISPANIC,
	   COUNT(CASE WHEN RACE='Asian' THEN 1 ELSE NULL END) AS N_PTS_ASIAN
  INTO #phame_male_age_gt_70
  FROM #phame_demo
  WHERE GENDER = 'Male'
  AND AGE > 70
  GROUP BY ZIP
  ORDER BY N_POP_AGE_BTW_gt_70 DESC
;



------------------- OUTPUT TABLES ------------------

--1) PHAME_DEMOGRAPHICS
-- Table B1
-- SELECT * FROM #phame_demo;

-- --2) PHAME_DIAGNOSIS
-- SELECT * FROM #phame_diagnosis;
--
-- --3) PHAME_DIAGNOSIS_INSURANCE
-- SELECT * FROM #phame_diag_ins;
--
-- --4) PHAME_DENOMINATOR
-- SELECT * FROM #phame_denominator;

--5) FEMALE
--a)
-- SELECT * FROM #phame_female_age_18_30;
-- --b)
-- SELECT * FROM #phame_female_age_31_40;
-- --c)
-- SELECT * FROM #phame_female_age_41_50;
-- --d)
-- SELECT * FROM #phame_female_age_51_60;
-- --e)
-- SELECT * FROM #phame_female_age_61_70;
-- --f)
-- SELECT * FROM #phame_female_age_gt_70;
--
-- --6) MALE
-- --a)
-- SELECT * FROM #phame_male_age_18_30;
-- --b)
-- SELECT * FROM #phame_male_age_31_40;
-- --c)
-- SELECT * FROM #phame_male_age_41_50;
-- --d)
-- SELECT * FROM #phame_male_age_51_60;
-- --e)
-- SELECT * FROM #phame_male_age_61_70;
-- --f)
-- SELECT * FROM #phame_male_age_gt_70;

--------------------- END CORE PHAME---------------------
------------
----MPC ----
------------


-- roll up the denominator
IF OBJECT_ID('tempdb..#phame_denominator_catalyst') IS NOT NULL DROP TABLE #phame_denominator_catalyst;
-- this is basically the same as in the main code

-- source schema:
-- Table B4
-- PHAME_DEMO: patid, age, gender, race, hispanic, zip, payer_primary, payer_secondary
-- ***Save to phame_denominator.csv
-- schema:
-- (zip:char(5), TOTAL_N_POP:int64, N_BLACK_POP:int64, N_WHITE_POP:int64, N_HISPANIC_POP:int64, N_ASIAN_POP:int64, N_BLACK_MALE_POP:int64, N_WHITE_MALE_POP:int64, N_HISPANIC_MALE_POP:int64, N_ASIAN_MALE_POP:int64, N_BLACK_FEMALE_POP:int64, N_WHITE_FEMALE_POP:int64, N_HISPANIC_FEMALE_POP:int64, N_ASIAN_FEMALE_POP:int64)
SELECT ZIP,
       COUNT(DISTINCT PATID) AS TOTAL_N_POP,
	   SUM(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE 0 END) AS N_BLACK_POP,
	   SUM(CASE WHEN RACE='White' THEN 1 ELSE 0 END) AS N_WHITE_POP,
	   SUM(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE 0 END) AS N_HISPANIC_POP,
	   SUM(CASE WHEN RACE='Asian' THEN 1 ELSE 0 END) AS N_ASIAN_POP,
	   SUM(CASE WHEN RACE='Black  or  African  American' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_BLACK_MALE_POP,
	   SUM(CASE WHEN RACE='White' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_WHITE_MALE_POP,
	   SUM(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_HISPANIC_MALE_POP,
	   SUM(CASE WHEN RACE='Asian' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_ASIAN_MALE_POP,
	   SUM(CASE WHEN RACE='Black  or  African  American' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_BLACK_FEMALE_POP,
	   SUM(CASE WHEN RACE='White' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_WHITE_FEMALE_POP,
	   SUM(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_HISPANIC_FEMALE_POP,
	   SUM(CASE WHEN RACE='Asian' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_ASIAN_FEMALE_POP
  INTO #phame_denominator_catalyst
  FROM #phame_demo
  GROUP BY ZIP
  ORDER BY TOTAL_N_POP DESC;

--  copy of phame_demo
IF OBJECT_ID('tempdb..#phame_demo_catalyst') IS NOT NULL DROP TABLE #phame_demo_catalyst;
SELECT *  INTO #phame_demo_catalyst FROM #phame_demo;
--select * from #phame_demo;
-- add the age category (+ other cols for later coding)
ALTER TABLE #phame_demo_catalyst ADD age_cat CHAR(1), SEX CHAR(1),  race_coded CHAR(1), ethnicity CHAR(1), zip_code CHAR(5), --primary_payer_coded CHAR(1), secondary_payer_coded CHAR(1);
            payer_primary_coded CHAR(1), payer_secondary_coded CHAR(1);
UPDATE #phame_demo_catalyst SET age_cat=CASE
		       WHEN AGE BETWEEN 18 AND 30 THEN '1'
			   WHEN AGE BETWEEN 31 AND 40 THEN '2'
			   WHEN AGE BETWEEN 41 AND 50 THEN '3'
			   WHEN AGE BETWEEN 51 AND 60 THEN '4'
			   WHEN AGE BETWEEN 61 AND 70 THEN '5'
               WHEN AGE IS NULL THEN '0'
			   WHEN AGE > 70 THEN '6' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN age;

-- revert sex to M/F/N
-- N is for non-binary
UPDATE #phame_demo_catalyst  SET SEX=
    CASE WHEN GENDER='Male' THEN 'M'
		WHEN GENDER='Female' THEN 'F'
        WHEN GENDER IS NULL THEN '0'
		ELSE 'N' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN GENDER;
--ALTER TABLE #phame_demo_catalyst RENAME COLUMN SEX TO GENDER;


UPDATE #phame_demo_catalyst SET race_coded =
    CASE WHEN race='American  Indian  or  Alaska  Native' THEN '1'
         WHEN race='Asian' THEN '2'
         WHEN race='Black  or  African  American' THEN '3'
	  	 WHEN  race='Native  Hawaiian  or  Other  Pacific  Islander' THEN '4'
         WHEN race='White' THEN '5'
         WHEN race='Multiple  race' THEN '6'
         WHEN race='Refuse  to  answer' THEN '7'
         WHEN race='No  information' THEN '8'
         WHEN race IS NULL THEN '8' -- literally no information
         WHEN race='Unknown' THEN '9'
         ELSE '0' END; -- for 'Other'

ALTER TABLE #phame_demo_catalyst DROP COLUMN race;
--ALTER TABLE #phame_demo_catalyst RENAME race_coded TO race;

UPDATE #phame_demo_catalyst SET ethnicity=
    CASE WHEN hispanic='Hispanic or Latino' THEN '1'
        WHEN hispanic='Not Hispanic or Latino' THEN '2'
        WHEN hispanic='Refuse  to  answer' THEN '3'
        WHEN hispanic='No  information' THEN '4'
        WHEN hispanic='Unknown' THEN '5'
        WHEN hispanic IS NULL THEN 0
        ELSE '6' END; -- 5 --> 'Other'

ALTER TABLE #phame_demo_catalyst DROP COLUMN hispanic;

-- all zips are in the domain of starting with '6' and having 5 characters
-- but zip code in original schema is a lot longer - truncate it by taking the first 5 characters
-- can put this in an int32, but keeping it simple for now!
UPDATE #phame_demo_catalyst SET zip_code = LEFT(zip, 5);
UPDATE #phame_demo_catalyst SET zip_code='00000' WHERE zip IS NULL;

ALTER TABLE #phame_demo_catalyst DROP COLUMN zip;
--ALTER TABLE #phame_demo_catalyst RENAME zip_code TO zip;


UPDATE #phame_demo_catalyst SET payer_primary_coded =
        CASE WHEN payer_primary='Medicare' THEN '1'
             WHEN payer_primary='Private/Commercial' THEN '2'
             WHEN payer_primary='No  information' THEN '3'
             WHEN payer_primary='Other' THEN '4'
             WHEN payer_primary='Unknown' THEN '5'
             WHEN payer_primary='Other' THEN '6'
             WHEN payer_primary IS NULL THEN '0' END;


UPDATE #phame_demo_catalyst SET payer_secondary_coded =
        CASE WHEN payer_secondary='Medicare' THEN '1'
             WHEN payer_secondary='Private/Commercial' THEN '2'
             WHEN payer_secondary='No  information' THEN '3'
             WHEN payer_secondary='Other' THEN '4'
             WHEN payer_secondary='Unknown' THEN '5'
             WHEN payer_secondary='Other' THEN '6'
             WHEN payer_secondary IS NULL THEN '0' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN payer_primary;
ALTER TABLE #phame_demo_catalyst DROP COLUMN payer_secondary;

--ALTER TABLE #phame_demo_catalyst RENAME COLUMN payer_primary_coded TO payer_primary;
--ALTER TABLE #phame_demo_catalyst RENAME COLUMN payer_secondary_coded TO payer_secondary;

-- It is possible that we will have more than one payer record on file.
-- Roughly treat this as 3 categories: Medicare (1), private insurance (2), all  others (3)
-- if we have a (1, 2) and a (2, 1) or vice versa, then take (1, 2) and drop other - we don't have date info with which to do something more clever
--select distinct patid from #phame_demo_catalyst
/* ----
there are no dupes in #phame_demo
*/
/*DELETE FROM #phame_demo_catalyst p1 
		WHERE  payer_primary_coded='2' AND payer_secondary_coded='1'  
			AND EXISTS (SELECT patid 
			FROM #phame_demo_catalyst p2 
			WHERE p2.patid = p1.patid AND p2.payer_primary_coded='1' AND p2.payer_secondary_coded='2' 
			);*/
-- dedupe on patid
-- we select min to avoid replacing known insurance with "blank spots" like no info or unknown
-- age cat the one exception, if the participant ages, we select the newest record
-- CASE statements so that if we have a NULL we replace it with the non-null value -  faking a COALESCE()
-- SELECT patid, max(CASE WHEN age_cat='0' THEN 'A' ELSE age_cat END) age_cat,
--        MIN(CASE WHEN sex = '0' THEN 'A' ELSE sex END) sex,
--        MIN(race_coded) race_coded,
--        MIN(CASE WHEN ethnicity = '0' THEN 'A' ELSE ethnicity END) ethnicity,
--        MIN(CASE WHEN zip_code = '00000' THEN 'ABCDE' ELSE zip_code END) zip,
--        MIN(CASE WHEN payer_primary_coded = '0' THEN 'A' ELSE payer_primary_coded END) payer_primary_coded,
--        MIN(CASE WHEN payer_secondary_coded = '0' THEN 'A' ELSE payer_secondary_coded END) payer_secondary_coded
--     INTO #phame_demo_catalyst_tmp
--     FROM #phame_demo_catalyst
-- GROUP BY patid;


--DROP TABLE #phame_demo_catalyst;
--ALTER TABLE #phame_demo_catalyst_tmp RENAME TO phame_demo_catalyst;


-- source schema:
-- PHAME_DIAGNOSIS: patid, zip, dx_diabetes, dx_diabetes_dt, dx_hypertension, dx_hypertension_dt, dx_cervical_cancer, dx_cervical_cancer_dt, dx_breast_cancer, dx_breast_cancer_dt, dx_lung_cancer, dx_lung_cancer_dt, dx_colorectal_cancer, dx_colorectal_cancer_dt
IF OBJECT_ID('tempdb..#phame_diagnosis_catalyst') IS NOT NULL DROP TABLE #phame_diagnosis_catalyst;

-- phame_diagnosis already deduped earlier
-- this is just simplifying our setup and dropping the dates
-- Schema: phame_diagnosis(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool)
SELECT patid, dx_diabetes, dx_hypertension, dx_cervical_cancer, dx_breast_cancer, dx_lung_cancer, dx_colorectal_cancer
INTO #phame_diagnosis_catalyst
FROM #phame_diagnosis;



-- roll up cube with diagnoses
-- everyone to generate this.
-- if data partner is contributing patient-level data, they will send this cube to the investigator to validate our results
-- if a site is contributing aggregate-level only, send this to MPC

-- Site descriptions
-- detailed site:  contributing patient level data
-- aggregate site: ones contributing rollups alone
IF OBJECT_ID('tempdb..#phame_rollup_catalyst') IS NOT NULL DROP TABLE #phame_rollup_catalyst;

SELECT age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded, COUNT(demo.patid) patient_cnt,
       SUM(dx_diabetes) diabetes_cnt, SUM(dx_hypertension) hypertension_cnt, SUM(dx_cervical_cancer) cervical_cancer_cnt,
       SUM(dx_breast_cancer) breast_cancer_cnt, SUM(dx_lung_cancer) lung_cancer_cnt, SUM(dx_colorectal_cancer) colorectal_cancer_cnt
INTO #phame_rollup_catalyst
FROM #phame_demo_catalyst demo  LEFT JOIN #phame_diagnosis dx ON  demo.patid = dx.patid
GROUP BY age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded
ORDER BY age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded;

--
--peek at output:
-- ***save phame_demo_catalyst to phame_demo.csv

-- DETAILED ONLY
-- schema:
-- phame_demo(patid:int32, age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1))
select * from #phame_demo_catalyst;

-- DETAILED ONLY
-- save phame_diagnosis_catalyst as phame_diagnosis.csv
-- schema: (implicitly casting study vars to bools, use count in MPC to add them up)
-- patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool

select * from #phame_diagnosis_catalyst;

-- ALL SITES SEND THIS
-- save output as PHAME_COHORT_COUNTS.csv
-- schema:
-- (attribute_desc:varchar(42), n:int32)
-- Table A1
SELECT * FROM #phame_cohort_counts;

-- ALL SITES SEND THIS
-- *** save phame_rollup_catalyst to phame_rollup.csv
-- schema:
--phame_rollup(age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)
select * from #phame_rollup_catalyst;
