package org.vaultdb.plan.operator;

import java.util.ArrayList;
import java.util.List;
import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rex.RexBuilder;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexUtil;
import org.apache.calcite.sql.SqlKind;
import org.vaultdb.db.schema.constraints.ColumnDefinition;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;

public class Filter extends Operator {
	
	public Join sourceJoin;
	
	public Filter(String name, SecureRelNode src, Operator[] childOps) throws Exception {
		super(name, src, childOps);
		sliceAgnostic = true;
		sourceJoin = null;
	}
	
	public Filter(String name, SecureRelNode src, Operator[] childOps, Join source) throws Exception {
		super(name, src, childOps);
		sliceAgnostic = true;
		sourceJoin = source;
	}
	
	public List<SecureRelDataTypeField> getSliceAttributes() {
		return super.derivedSliceKey();
	}
	
	public List<SecureRelDataTypeField> computesOn() {
		LogicalFilter filter = (LogicalFilter) baseRelNode.getRelNode();
		RexNode predicate = filter.getCondition();
		SecureRelRecordType schema = getSchema();
		
		return AttributeResolver.getAttributes(predicate, schema);
	}

	public List<SecureRelDataTypeField> computesOn(SecureRelRecordType schema) {
		
		LogicalFilter filter = (LogicalFilter) baseRelNode.getRelNode();
		RexNode predicate = filter.getCondition();
		
		
		return AttributeResolver.getAttributes(predicate, schema);
	}
	
	
	@Override
    public int getPerformanceCost(int n) {
        // 0
        return 0;
    }

	@Override
	public void initializeStatistics() {
		
		children.get(0).initializeStatistics();
		 // initially copy stats from input since this op has the same schema as its source
		super.initializeStatistics(); 
		
		// TODO: beef this up with coverage of more than equality checks on integers

		// update fields that we compute on to have a distinct cardinality of 1
		// and max multiplicity equal to that of the lowest of what we compute on
		// also need to setCardinality() on all attrs to reflect filter's output size 
		LogicalFilter filter = (LogicalFilter) baseRelNode.getRelNode();
		RexNode expr = filter.getCondition();
		RexBuilder builder = filter.getCluster().getRexBuilder();
		
		
		// TODO: support  nested expressions
		List<RexNode> comparisons = new ArrayList<RexNode>();
		long lowestMaxMultiplicity = children.get(0).getCardinalityBound();
		long maxMultiplicity;
		
		expr = RexUtil.toCnf(builder, expr);
		RelOptUtil.decomposeConjunction(expr, comparisons);
		
		
		
		List<SecureRelDataTypeField> filteredFields = new ArrayList<SecureRelDataTypeField>();
		
		for(RexNode r : comparisons) {
			assert(r.getKind() == SqlKind.EQUALS); // only support equality predicate
			RexCall cOp = (RexCall) r;
			RexNode lhs = cOp.getOperands().get(0);
			RexNode rhs = cOp.getOperands().get(1);
			SecureRelDataTypeField dstField = null;
			long targetValue = -1;
			
			if(lhs.getKind() == SqlKind.INPUT_REF && rhs.getKind() == SqlKind.LITERAL) {
				int ordinal = ((RexInputRef) lhs).getIndex();
				
			    dstField = getSchema().getSecureField(ordinal);
				targetValue = RexLiteral.intValue(rhs);			
			}
			else if(lhs.getKind() == SqlKind.LITERAL && rhs.getKind() == SqlKind.INPUT_REF) {
				int ordinal = ((RexInputRef) rhs).getIndex();

				dstField = getSchema().getSecureField(ordinal);
				targetValue = RexLiteral.intValue(lhs);
			}
			
			List<Long> newDomain = new ArrayList<Long>();
			newDomain.add(targetValue);
			
			ColumnDefinition stats = dstField.getColumnConstraints();
			stats.setDomain(newDomain);
			stats.setDistinctCardinality(1);
			stats.setMin(targetValue);
			stats.setMax(targetValue);
			maxMultiplicity = stats.getMaxMultiplicity();
			if(maxMultiplicity != -1)
				stats.setCardinality(maxMultiplicity);

			if(maxMultiplicity > 0 &&  maxMultiplicity < lowestMaxMultiplicity) {
				lowestMaxMultiplicity = maxMultiplicity;
			}
			
			filteredFields.add(dstField);
		}
		

		for(SecureRelDataTypeField f : filteredFields){
			f.getColumnConstraints().setMaxMultiplicity(lowestMaxMultiplicity);
		}
		
		// ensure that all field statistics have the same cardinality bound
		lineUpCardinalityStatistics();
	}

	
//	@Override
/*	public long getCardinalityBound() {
		// TODO: beef this up with coverage of more than equality checks on fixed vals and conjunctive normal form
		List<SecureRelDataTypeField> fields = this.computesOn();
		long cardinality = children.get(0).getCardinalityBound();
		
		// take minimum max multiplicity for cardinality bound
		for(SecureRelDataTypeField f : fields) {
			long maxMultiplicity = f.getStatistics().getMaxMultiplicity();
			if(maxMultiplicity < cardinality) {
				cardinality = maxMultiplicity;
			}
		}
		
		return cardinality;
	}*/
	
}
