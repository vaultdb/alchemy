package org.vaultdb.plan.operator;

import java.util.ArrayList;
import java.util.List;
import org.apache.calcite.rel.core.AggregateCall;
import org.apache.calcite.rel.logical.LogicalAggregate;
import org.apache.calcite.util.ImmutableBitSet;
import org.apache.calcite.util.Pair;
import org.vaultdb.executor.config.ExecutionMode;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelDataTypeField.SecurityPolicy;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

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
		List<SecureRelDataTypeField> attrs = this.getGroupByAttributes(); // group by only
		
		// don't need the rest of this because evaluation of aggregate is done deterministically
		
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

		for (Integer i : groupBy) 
			orderBy.add(inSchema.getSecureField(i));

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

		if(this.executionMode.distributed  && this.executionMode.oblivious && (agg.getGroupCount() == 0)
				 && childrenLocal()) { 
			return true;
		}
		return false;
		
	}
	
	
	@Override
	public void inferExecutionMode() throws Exception {
	
		Operator child = children.get(0);
		child.inferExecutionMode();
		
		executionMode = new ExecutionMode(child.executionMode);

		// we already know inputs are partitioned-alike
		// see if we are partitioned on group-by or a subset thereof
		boolean partitioned = false;
		// if child runs locally
		if(!child.executionMode.distributed) {
			List<SecureRelDataTypeField> attrs = this.getGroupByAttributes();
			
			for(SecureRelDataTypeField attr : attrs) {
				if(Utilities.isLocalPartitionKey(getSchema(), attr))
					partitioned = true;
			}
			
			
		}
		
		SecurityPolicy maxExecutionMode = super.maxAccessLevel();
		if(maxExecutionMode != SecurityPolicy.Public)
			executionMode.oblivious = true;
		
		if(!partitioned)
			executionMode.distributed = true;
	}
		
	
	private boolean childrenLocal() {
		
		for(Operator op : children) {
			if(op.getExecutionMode().distributed) {
				return false;
			}
			
		}
		return true;
		
	}


	
	public Pair<Long, Long> obliviousCardinality() {
		if(agg.getGroupSet().isEmpty()){
			// no group by, scalar output
			return new Pair<Long, Long>(1L, 1L);
		}
		
		return children.get(0).obliviousCardinality();
		
	}

	
	
	
};
