package org.vaultdb.privacy;

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
	
	public static int getSensitivity(String packageName, int length) {
		if (packageName.contains("Join")) {
			return (length == 0) ? 1 : length;
		} else {
			return 1;
		}
	}
}
