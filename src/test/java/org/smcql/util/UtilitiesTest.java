package org.smcql.util;

import junit.framework.TestCase;

public class UtilitiesTest extends TestCase {
	
	public void testGenerateDiscreteLaplaceNoise() {
		double sensitivity = 2.0;
		double epsilon = 0.1;
		double delta = 0.00001;
		
		int noise = Utilities.generateDiscreteLaplaceNoise(epsilon, delta, sensitivity);
		
		System.out.println("noise: " + noise);
		assertTrue(noise > 0);
	}
}
