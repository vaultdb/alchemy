package org.smcql.privacy;

import java.io.Serializable;

public class PrivacyCost implements Serializable {
	private static final long serialVersionUID = -2815166705777531972L;
	double epsilon;
	double delta;
	
	public PrivacyCost(double eps, double del) {
		epsilon = eps;
		delta = del;
	}
	
	public void setEpsilon(double val) {
		epsilon = val;
	}
	
	public double getEpsilon() {
		return epsilon;
	}
	
	public double getDelta() {
		return delta;
	}
	
	public void setDelta(double val) {
		delta = val;
	}
}
