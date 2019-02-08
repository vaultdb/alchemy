package org.smcql.plan.operator;

import java.util.ArrayList;
import java.util.List;
import java.util.SortedSet;

import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rex.RexBuilder;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexUtil;
import org.apache.calcite.sql.SqlKind;
import org.smcql.plan.SecureRelNode;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;

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
		// TODO: beef this up with coverage of more than equality checks on fixed vals and conjunctive normal form

		// update fields that we compute on to have a distinct cardinality of 1
		// and max multiplicity equal to that of the lowest of what we compute on
		List<SecureRelDataTypeField> fields = this.computesOn();
		LogicalFilter filter = (LogicalFilter) baseRelNode.getRelNode();
		RexNode expr = filter.getCondition();
		RexBuilder builder = filter.getCluster().getRexBuilder();
		expr = RexUtil.toCnf(builder, expr);
		// TODO: support for nested expressions
		List<RexNode> comparisons = new ArrayList<RexNode>();
		RelOptUtil.decomposeConjunction(expr, comparisons);
		
		
		
		
		for(RexNode r : comparisons) {
			assert(r.getKind() == SqlKind.EQUALS); // only support equality predicate
			RexCall cOp = (RexCall) r;
			RexNode lhs = cOp.getOperands().get(0);
			RexNode rhs = cOp.getOperands().get(1);
			if(lhs.getKind() == SqlKind.INPUT_REF) {
				
			}
		}
		
		Long lowestMaxMultiplicity = children.get(0).getCardinalityBound();
		
		
		for(SecureRelDataTypeField f : fields) {
			// field has only one value now since we are comparing it to a constant
			f.getStatistics().setDistinctCardinality(1);
			// TODO: set min and max to the scalar we are comparing it to in the expr

			long localMaxMultiplicity = f.getStatistics().getMaxMultiplicity();
			if(localMaxMultiplicity > 0 && localMaxMultiplicity < lowestMaxMultiplicity) {
				lowestMaxMultiplicity = localMaxMultiplicity;
			}
		}
		
		// make a second pass to line up max multiplicity for relation
		for(SecureRelDataTypeField f : fields) {
				if(f.getStatistics().getMaxMultiplicity() > lowestMaxMultiplicity)
					f.getStatistics().setMaxMultiplicity(lowestMaxMultiplicity);
		}
		
	}

	
	@Override
	public long getCardinalityBound() {
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
	}
	
}
