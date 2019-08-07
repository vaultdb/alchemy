package org.vaultdb.plan.operator;

import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.rel.core.JoinRelType;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rex.RexBuilder;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexUtil;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.util.Pair;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.privacy.PrivacyCost;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.type.SecureRelDataTypeField.SecurityPolicy;

public class Join extends Operator {

	
	public Join(String name, SecureRelNode src, Operator ...children ) throws Exception {
		super(name, src, children);
		
	}
	
	

	@Override
	public void inferExecutionMode()  throws Exception {
		SecurityPolicy maxAccess = maxAccessLevel();

		super.inferExecutionMode();
		
		if(!children.get(0).executionMode.oblivious  && 
				!children.get(1).executionMode.oblivious && maxAccess == SecurityPolicy.Public) { // results are replicated
				executionMode.oblivious = false;
		}
	}
	
	@Override
	public PrivacyCost getPrivacyCost() throws Exception {
		double epsilon = Double.parseDouble(SystemConfiguration.getInstance().getProperty("epsilon")); 
		super.getPrivacyCost().setEpsilon(epsilon * 2);
		return super.getPrivacyCost();
	}

	public List<SecureRelDataTypeField> getSliceAttributes() {
		
		List<SecureRelDataTypeField> sliceKey = new ArrayList<SecureRelDataTypeField>();
		
		LogicalJoin join = (LogicalJoin) this.getSecureRelNode().getRelNode();
		
		
		assert(join.getJoinType() == JoinRelType.INNER || join.getJoinType() == JoinRelType.FULL
				|| join.getJoinType() == JoinRelType.LEFT); // TODO: LEFT not yet implemented
		
		RexNode joinOn = join.getCondition();
		
		if(joinOn == null)
			return sliceKey; // no keys
		
		
		RexBuilder rexBuilder = join.getCluster().getRexBuilder();
		
		joinOn = RexUtil.toCnf(rexBuilder, joinOn);  // get it to conjunctive normal form for easier optimization
		
		if(joinOn.getKind() == SqlKind.AND) {  // TODO: handle > 2 selection criteria?
			List<RexNode> operands = new ArrayList<RexNode>(((RexCall) joinOn).operands);
			for(RexNode op : operands) {
				sliceKey = checkForSliceField(op, sliceKey);
								
			}
		}
		else // single comparison
			sliceKey = checkForSliceField(joinOn, sliceKey);
		
		//assert(sliceKey.size() <= 2); // multiple, composite slice keys not implemented
		return sliceKey;
	}

	
	// equality predicates only
	List<SecureRelDataTypeField> checkForSliceField(RexNode rex, List<SecureRelDataTypeField> sliceKey) {

		SecureRelRecordType inSchema = getInSchema();
		
		if(rex.getKind() == SqlKind.EQUALS) { // equality predicate
			RexCall cOp = (RexCall) rex;
			RexNode lhs = cOp.getOperands().get(0);
			RexNode rhs = cOp.getOperands().get(1);
		
			if(lhs.getKind() == SqlKind.INPUT_REF && rhs.getKind() == SqlKind.INPUT_REF) {
				RexInputRef lhsRef = (RexInputRef) lhs;
				RexInputRef rhsRef = (RexInputRef) rhs;
				
				int lOrdinal = lhsRef.getIndex();
				int rOrdinal = rhsRef.getIndex();
				
				SecureRelDataTypeField lField = inSchema.getSecureField(lOrdinal);
				SecureRelDataTypeField rField = inSchema.getSecureField(rOrdinal);
				
				
					if(lField.isSliceAble() && rField.isSliceAble())  {							
						if(!sliceKey.contains(lField))
								sliceKey.add(lField);
						if(!sliceKey.contains(rField))
								sliceKey.add(rField);
					}
						
	
				}
	
		}
		return sliceKey;
	}
	
	public SecureRelRecordType getInSchema() {
		
		return  getSchema();
	}
	
	public List<SecureRelDataTypeField> computesOn() {

		LogicalJoin join = (LogicalJoin) this.getSecureRelNode().getRelNode();
		RexNode joinOn = join.getCondition();
		
		return AttributeResolver.getAttributes(joinOn, getSchema());
		
	}



	public RexNode getCondition() {
		LogicalJoin join = (LogicalJoin) this.getSecureRelNode().getRelNode();
		return join.getCondition();
	}

	@Override
	public int getPerformanceCost(int n) {
		// n*log(n)^2
		return n*(int)Math.pow(Math.log((double)n),2);
	}
	
	@Override
	protected int getEstimatedStability() {
		return getEstimatedCardinality();
	}
	
	@Override
	public void initializeStatistics() {
		for(Operator child : children) {
			child.initializeStatistics();
		}
		// TODO: Nisha and May: derive statistics using methods in <repo root>/docs/alchemy-algebra.pdf
	}
	
	
	@Override
	public Pair<Long, Long> obliviousCardinality() {
		Pair<Long, Long>  lhs = children.get(0).obliviousCardinality();
		Pair<Long, Long>  rhs = children.get(1).obliviousCardinality();
		
		return new Pair<Long, Long>(lhs.left * rhs.left, lhs.right * rhs.right);

	}
	
	@Override
	public String toString() {
		
		LogicalJoin join = (LogicalJoin) this.getSecureRelNode().getRelNode();
		RexNode predicate = join.getCondition();
		String ret = baseRelNode.getRelNode().getRelTypeName() + "-" + executionMode + ", schema:" + getSchema() + ", Predicate: " + predicate;
		List<SecureRelDataTypeField> sliceAttrs = this.getSliceAttributes();
		if(!sliceAttrs.isEmpty())
			ret += ", slice key: " + sliceAttrs;

		return ret;
	}

	
};
