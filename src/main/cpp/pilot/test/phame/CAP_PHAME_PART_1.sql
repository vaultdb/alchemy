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
/*  a) [CAP_HASH_TOKEN]                                                                         */
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


use [pcornet]; --<-- specify database name containing PCORnet CDM tables


DECLARE @STARTStudy date; 		SET @STARTStudy = '01/01/2015';
DECLARE @ENDStudy date; 		SET @ENDStudy = '12/31/2021';

IF OBJECT_ID('tempdb..#study_population') IS NOT NULL DROP TABLE #study_population;
WITH popln AS (
  
   SELECT DISTINCT demo.CAP_ID,
                   CAST(DATEDIFF(dd,demo.BIRTH_DATE, enc.DT)/365.23076923074 AS INT) AS AGE, 
                   demo.SEX,
				   demo.RACE,
				   demo.ZIP
      FROM [database].[dbo].CAP_DEMOGRAPHICS demo        --------------<------------  Specify the database name containing CAP_DEMOGRAPHICS table
	  INNER JOIN (SELECT DISTINCT PATID, MAX(ADMIT_DATE) as DT 
	               FROM [dbo].ENCOUNTER 
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
		   FROM dbo.DEMOGRAPHIC
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

---- Extract Hash Tokens

IF OBJECT_ID('tempdb..#hash') IS NOT NULL DROP TABLE #hash;
SELECT a.* 
 INTO #hash
 FROM [database].dbo.CAP_HASH_TOKEN a  --------------<------------    Specify the database name containing CAP_DEMOGRAPHICS table
 INNER JOIN #randomly_stratified b
  ON a.CAP_ID = b.CAP_ID
;

----- Return tables to MRAIA

SELECT * FROM #hash; -- save output as PHAME_TOKEN.csv

----- Return Summary Counts to PI

SELECT *, '' as SITE_ID FROM (            ------------<------------      Specify Site Name
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