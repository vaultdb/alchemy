package org.vaultdb.codegen.smc.operator;


import org.vaultdb.plan.operator.Aggregate;
import org.vaultdb.plan.operator.Operator;
import org.vaultdb.type.SecureRelDataTypeField;

import java.util.List;

public class SecureOperatorFactory {
	
	public static SecureOperator get(Operator o) throws Exception {
	
		switch(o.getOpName()) {
			case "Aggregate":

				// Ideally these will be merged into one class in the future
				// Currently seperated as SecureAggregate is functional
				Aggregate agg = (Aggregate) o;
				List<SecureRelDataTypeField> groupByAttributes = agg.getGroupByAttributes();

				if (groupByAttributes.isEmpty()) {
					return new SecureAggregate(o);
				}

				else{
					return new SecureGroupByAggregate(o);
				}

			case "Sort":
				return new SecureSort(o);
			case "Distinct":
				return new SecureDistinct(o);
			case "WindowAggregate":
				return new SecureWindowAggregate(o);
			case "Join":
				return new SecureJoin(o);
			case "Filter":
				return new SecureFilter(o);
			case "Merge":
				return new SecureUnion(o);
			case "Correlate":
				return new SecureCorrelate(o);
			default:
				return null;
		
		}
	}
}
