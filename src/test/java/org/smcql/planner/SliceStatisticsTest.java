package org.smcql.planner;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.junit.Test;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.execution.slice.statistics.SliceStatistics;
import org.smcql.plan.execution.slice.statistics.StatisticsCollector;
import org.smcql.plan.slice.SliceKeyDefinition;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.util.Utilities;

public class SliceStatisticsTest extends SliceKeyTest {
	private Map<String, Map<SecureRelDataTypeField, SliceStatistics> > expectedStatistics; // (test, (key, values))
	private String expectedMerge;
	
	
				
	protected void setUp() throws Exception {
		super.setUp();
		expectedStatistics = new HashMap<String, Map<SecureRelDataTypeField, SliceStatistics> >();
		
		
		
		setupAspirinRate();
		setupCDiff();
		setupComorbidity();
		expectedMerge = setupExpectedMerge();
		

	}
	
	
	public static String setupExpectedMerge() {
		String expectedMergeStr = new String();
		expectedMergeStr += "Key: [medications.patient_id=diagnoses.patient_id]\n";
		expectedMergeStr += "Single site values: <[1], (1, 1)> <[7], (1, 1)> <[9], (1, 1)> <[13], (1, 1)> <[20], (2, 1)> \n";
		expectedMergeStr += "Distributed values: <[3], [(1, 2), (2, 1)]> <[11], [(1, 1), (2, 1)]> <[15], [(1, 2), (2, 3)]> <[18], [(1, 3), (2, 2)]> \n"; 
		return expectedMergeStr;
	}

	private void setupAspirinRate() throws Exception {
		
		String testName = "aspirin_rate";
		final List<SecureRelDataTypeField> expectedKeys = expectedSliceKeys.get(testName);
		SecureRelDataTypeField diagsSlice = expectedKeys.get(0);
		SecureRelDataTypeField medsSlice = expectedKeys.get(1);
		
		
		final Map<SecureRelDataTypeField, SliceStatistics> statistics = new HashMap<SecureRelDataTypeField, SliceStatistics>();
		
		
		// diagnoses.patient_id
		SliceStatistics diagnosisStatistics = new SliceStatistics(new SliceKeyDefinition(Arrays.asList(diagsSlice)));
		
		
		//SELECT patient_id, site.id, COUNT(*)  FROM diagnoses, site WHERE icd9 like '400%' GROUP BY patient_id, site.id
		int[][] diagnosisOutcomes = {
				{ 11, 1, 1 },
				{ 3, 1, 1 },
				{ 7, 1, 1 },
				{ 1, 1, 1 },
				{ 9, 1, 1 },
				{ 13, 1, 1 },
				{ 18, 1, 2 },
				{ 18, 2, 2 },
				{ 15, 2, 2 },
				{ 15, 1, 1 }//,
	//			{ 20, 3, 1 },
	//			{ 5, 3, 1 }	
		};
		for(int i = 0; i < diagnosisOutcomes.length; ++i)  {
			Tuple t = createTuple(diagsSlice.getStoredAttribute(), diagnosisOutcomes[i]);
			diagnosisStatistics.addDataSource(t);
			
		}
		
		statistics.put(diagsSlice, diagnosisStatistics);

		
		Map<SecureRelDataTypeField, Integer> singleSiteValues = new HashMap<SecureRelDataTypeField, Integer>();
		singleSiteValues.put(diagsSlice, 8);  		// 1, 3, 5, 7, 9, 11, 13, 20
		
		Map<SecureRelDataTypeField, Integer> distributedValues = new HashMap<SecureRelDataTypeField, Integer>();
		distributedValues.put(diagsSlice, 2); // 15, 18
	

		
		// medications.patient_id
		// SELECT patient_id, site.id, COUNT(*)  FROM medications, site WHERE medication='aspirin' GROUP BY patient_id, site.id
		SliceStatistics medicationStatistics = new SliceStatistics(new SliceKeyDefinition(Arrays.asList(medsSlice)));
		
		int[][] medicationOutcomes = {
				{ 15, 1, 1 },
				{ 3, 1, 1 },
				{ 20, 2, 1 },
				{ 15, 2, 1 },
				{ 11, 2, 1 },
				{ 3,  2, 1},
				{ 18, 1, 1}
		};
		
		for(int i = 0; i < medicationOutcomes.length; ++i)  {
			Tuple t = createTuple(medsSlice.getStoredAttribute(), medicationOutcomes[i]);
			medicationStatistics.addDataSource(t);
			
		}
		
		statistics.put(medsSlice, medicationStatistics);
		expectedStatistics.put(testName, statistics);

		
		singleSiteValues.put(medsSlice, 3); // 11, 18, 20
		distributedValues.put(medsSlice, 2); // 3, 15
		
	}
	

	protected void setupCDiff() throws Exception {
		
		final String testName = "cdiff";
		
		
		List<SecureRelDataTypeField> slices = expectedSliceKeys.get(testName);
		SecureRelDataTypeField slice = slices.get(0);
		
		final Map<SecureRelDataTypeField, SliceStatistics> statistics = new HashMap<SecureRelDataTypeField, SliceStatistics>();
		SliceStatistics attrStatistics = new SliceStatistics(new SliceKeyDefinition(Arrays.asList(slice)));

		// SELECT patient_id, site.id, COUNT(*)  FROM diagnoses, site WHERE icd9='008.45' GROUP BY patient_id, site.id;
		int[][] outcomes = new int[][] {
				{ 2, 1, 1 },
				{ 5, 1, 2 },
				{ 1, 1, 3 },
				{ 6, 2, 1 },
				{ 2, 2, 1 },
				{ 4, 2, 1 },
				{ 3, 2, 2 }//,
//				{ 5, 3, 1 }
				};

		for(int i = 0; i < outcomes.length; ++i)  {
			Tuple t = createTuple(slice.getStoredAttribute(), outcomes[i]);
			attrStatistics.addDataSource(t);
			
		}
		
		statistics.put(slice, attrStatistics);
		
		expectedStatistics.put(testName, statistics);
		
		//singleSiteValues.put(slice, 4); // 1, 3, 4, 6
		
		//distributedValues.put(slice, 2); // 2
		
		
	}

	
	
	private void setupComorbidity() {
		final String testName = "comorbidity";
		
		final Map<SecureRelDataTypeField, SliceStatistics> statistics = new HashMap<SecureRelDataTypeField, SliceStatistics>();
		
		expectedStatistics.put(testName, statistics);
		

	}



	
	@Test
	public void testAspirinRate() throws Exception {
			testCase("aspirin-count");
	}
	
	
	@Test
	public void testCDiff() throws Exception {
			testCase("cdiff");
	}

	@Test
	public void testComorbidity() throws Exception {
			testCase("comorbidity");
	}


	/*@Test
	public void testMerge() throws Exception {
		Map<SecureRelDataTypeField, SliceStatistics> toMerge = expectedStatistics.get("aspirin_rate");
		List<SliceStatistics> stats = new ArrayList<SliceStatistics>(toMerge.values());

		List<SecureRelDataTypeField> keys = new ArrayList<SecureRelDataTypeField>(toMerge.keySet());
		//EqualitySlice eq = new EqualitySlice(keys.get(0), keys.get(1));
		
		SliceStatistics merged = SliceStatistics.mergeStatistics(stats.get(0), stats.get(1), eq);
		
		System.out.println("Expected: " + expectedMerge);
		System.out.println("Observed: " + merged);
		assertEquals(expectedMerge, merged.toString());
		
	}*/
	
	private void testCase(String testName) throws Exception {
		String sql = super.readSQL(testName);

		System.out.println("Running query " + sql);
		
		List<SecureRelDataTypeField> sliceKeys = expectedSliceKeys.get(testName);
		for(SecureRelDataTypeField key : sliceKeys) {
			SliceStatistics attrStatistics = StatisticsCollector.collect(new SliceKeyDefinition(Arrays.asList(key))); 
			Map<SecureRelDataTypeField, SliceStatistics> expectedLookup = expectedStatistics.get(testName);	
			SliceStatistics expected = expectedLookup.get(key);
			
			System.out.println("Expected: " + expected);
			System.out.println("Observed: " + attrStatistics);
			
			assertTrue(attrStatistics.equals(expected));
						
		
		}
		
	}
	
	// create a tuple with k int fields
	private Tuple createTuple(String src, int[] fields) {
		Tuple t = new Tuple();
		
		SecureRelDataTypeField sPrime = new SecureRelDataTypeField(src, 0, null);
		for(int i = 0; i < fields.length; ++i) {
			IntField intField = new IntField(sPrime, fields[i]);
			t.addField(intField);
		}
		
		return t;
	}
}
