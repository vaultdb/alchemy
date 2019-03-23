package org.smcql.plan.operator;

import java.util.ArrayList;
import java.util.List;
import org.apache.calcite.rel.core.AggregateCall;
import org.apache.calcite.rel.logical.LogicalAggregate;
import org.apache.calcite.util.ImmutableBitSet;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelNode;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;

public class Aggregate extends Operator {

	LogicalAggregate agg;
	
	public Aggregate(String name, SecureRelNode src, Operator ...children) throws Exception {
		super(name, src, children);
		
		blocking = true;
		agg = (LogicalAggregate) this.getSecureRelNode().getRelNode();

		// aggs without group-bys are splittable
		splittable = (agg.getGroupCount() == 0) ? true : false;
		

	}

	public List<SecureRelDataTypeField> getSliceAttributes() {
		List<Integer> groupBy = agg.getGroupSet().asList();
		List<SecureRelDataTypeField> sliceKeys = new ArrayList<SecureRelDataTypeField>();
		SecureRelRecordType inSchema = this.getInSchema();
		
		for(Integer i : groupBy) if(inSchema.getSecureField(i).isSliceAble()) sliceKeys.add(inSchema.getSecureField(i));
		
		for(AggregateCall aggCall : agg.getAggCallList()) {
			List<Integer> args  =  aggCall.getArgList();
			for(Integer i : args) if(inSchema.getSecureField(i).isSliceAble()) sliceKeys.add(inSchema.getSecureField(i));
		}
		return sliceKeys;
	}
	
	public int getComputeAttributeIndex() {
		return agg.getGroupCount(); // only supports one group-by col for now
	}
	
	public List<SecureRelDataTypeField> getGroupByAttributes() {
		List<Integer> groupBy = agg.getGroupSet().asList();
		
		SecureRelRecordType schema = this.getSchema();
		List<SecureRelDataTypeField> result = new ArrayList<SecureRelDataTypeField>();
		for (Integer i : groupBy) result.add(schema.getSecureField(i));

		return result;
	}
	
	public List<SecureRelDataTypeField> computesOn() {
		List<SecureRelDataTypeField> attrs = getSliceAttributes(); // group by
		List<SecureRelDataTypeField> allFields = getSchema().getSecureFieldList();
		
		List<AggregateCall> aggregates = agg.getAggCallList();
		for(AggregateCall aggCall : aggregates) {
			List<Integer> args  =  aggCall.getArgList();
			// compute over entire row
			if(args.isEmpty()) return baseRelNode.getSchema().getSecureFieldList();

			for(Integer i : args) {
				SecureRelDataTypeField lookup = allFields.get(i);
				
				if(!attrs.contains(lookup)) attrs.add(lookup);
			}
		}
		
		for(ImmutableBitSet bits : agg.groupSets.asList()) {
			for(Integer i : bits.asList()) {
				SecureRelDataTypeField lookup = allFields.get(i);
				
				if(!attrs.contains(lookup)) attrs.add(lookup);
			}
		}
		return attrs;
	}

	public List<SecureRelDataTypeField> secureComputeOrder() {
		List<Integer> groupBy = agg.getGroupSet().asList();
		List<SecureRelDataTypeField> orderBy = new ArrayList<SecureRelDataTypeField>();
		SecureRelRecordType inSchema = this.getInSchema();

		for (Integer i : groupBy) orderBy.add(inSchema.getSecureField(i));

		return orderBy;
	}
	
	@Override
	public int getPerformanceCost(int n) {
		// n*log(n)^2
		return n*(int)Math.pow(Math.log((double)n),2);
	}
	
	@Override
	protected int getEstimatedStability() {
		return (getGroupByAttributes().size() > 0) ? 1 : 0;
	}
	
	@Override
	public void initializeStatistics() {
		children.get(0).initializeStatistics();
		//read the code above, copy the statistics for the groupby bin, for the aggregate bin you want to give it
		//range of 1 to n if it's a count
		//
		// TODO: Nisha and May: derive statistics using methods in <repo root>/docs/alchemy-algebra.pdf
	}
	
	@Override
	public boolean pullUpFilter() {
		if(splitAggregate()) {
			return false;
		}
		return true;
	}

	// is this a scalar aggregate with no distributed children?
	public boolean splitAggregate() {

		if(this.executionMode == ExecutionMode.Secure && (agg.getGroupCount() == 0)
				 && childrenLocal()) { 
			return true;
		}
		return false;
		
	}
	
	@Override
	public void inferExecutionMode() {
		super.inferExecutionMode(); 
		
		// if this is a scalar aggregate  (no group-by) with no distributed children
		// then set children to public and run its partial execution locally

		if(splitAggregate()) {
					// recursively set all children to public
					setChildrenToPublic();
				}
		
	}
	

	
	private boolean childrenLocal() {
		return childrenLocalHelper(this.getChild(0));
	}
	private boolean childrenLocalHelper(Operator op) {
		if(!(op instanceof Filter || op instanceof SeqScan || op instanceof Project || op instanceof CommonTableExpressionScan)) {
			
			return false;
		}	
		
		boolean local = true;
		for(Operator child : op.children) {
			local = local & childrenLocalHelper(child);
		}
		return local;
	}
	
	// for split operator case where aggregate has no group-by
	// hence its cardinality is independent of that of its locally-executed children
	private void setChildrenToPublic() {
		for(Operator child : children) {
			setToPublicHelper(child);
		}
	}
	
	// set children to public
	private void setToPublicHelper(Operator op) {
		op.executionMode = ExecutionMode.Plain;
		for(Operator child : op.getChildren()) {
			setToPublicHelper(child);
		}
	}
	
};
