package org.vaultdb.privacy;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public class PrivacyStatistics implements Serializable {
	
	private static final long serialVersionUID = -8469898338444878119L;
	public static final double MAX_EPSILON = 1;
	public static final double MAX_DELTA = 0.00001;
	
	private Map<String, Double> epsilonBudget;
	private Map<String, Double> deltaBudget;
	
	public PrivacyStatistics() {
		epsilonBudget = new HashMap<String, Double>();
		deltaBudget = new HashMap<String, Double>();
	}
	
	public double getEpsilonConsumed(String relationName) {
		return epsilonBudget.get(relationName);
	}
	
	public double getDeltaConsumed(String relationName) {
		return epsilonBudget.get(relationName);
	}
	
	public void consumePrivacy(String relationName, PrivacyCost cost) throws Exception{
		consumeEpsilon(relationName, cost.getEpsilon());
		consumeDelta(relationName, cost.getDelta());
	}
	
	private void consumeEpsilon(String relationName, double epsilonConsumed) throws Exception {
		if (!epsilonBudget.containsKey(relationName))
			epsilonBudget.put(relationName, 0.0);
		
		double newTotal = epsilonBudget.get(relationName) + epsilonConsumed;
		
		if (newTotal > MAX_EPSILON)
			throw new Exception("Exceeded epsilon budget for relation: " + relationName);
		
		epsilonBudget.put(relationName, newTotal);
	}
	
	private void consumeDelta(String relationName, double deltaConsumed) throws Exception {
		if (!deltaBudget.containsKey(relationName))
			deltaBudget.put(relationName, 0.0);
		
		double newTotal = deltaBudget.get(relationName) + deltaConsumed;
		
		if (newTotal > MAX_DELTA)
			throw new Exception("Exceeded delta budget for relation: " + relationName);
		
		deltaBudget.put(relationName, newTotal);
	}
}
