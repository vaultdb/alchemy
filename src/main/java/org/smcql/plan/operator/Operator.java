package org.smcql.plan.operator;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexUtil;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.smcql.codegen.CodeGenerator;
import org.smcql.codegen.plaintext.PlainOperator;
import org.smcql.codegen.smc.operator.SecureOperator;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.ExecutionMode;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.ShadowRelNode;
import org.smcql.plan.slice.SliceKeyDefinition;
import org.smcql.privacy.PrivacyCost;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.RelNodeMatcher;
import org.smcql.util.Utilities;


// for execution mode planning over a RelNode
public abstract class Operator implements CodeGenerator {
	
	protected SecureRelNode baseRelNode;
	protected ShadowRelNode unoptimizedRelNode = null;
	private SecureOperator secureOperator;
	protected PlainOperator plaintextGenerator;
	protected List<Operator> children;
	
	protected Operator parent;
	
	SliceKeyDefinition sliceKey;
	
	Logger logger;
	
	String operatorId;
	
	protected boolean sliceAgnostic = false;
	
	protected boolean splittable = false;
	
	protected boolean blocking = false;
	
	ExecutionMode  executionMode = null;
	String queryName;
	
	PrivacyCost pCost;
	
	SystemConfiguration config;
	
	
	public Operator(String name, SecureRelNode src, Operator... childOps) throws Exception {
		baseRelNode = src;
		src.setPhysicalNode(this);
		children = new ArrayList<Operator>();
		config = SystemConfiguration.getInstance();
	
		
		operatorId = config.getOperatorId();
		
		for(Operator op : childOps) {
			children.add(op);
			op.setParent(this);
		}
		
		logger = config.getLogger();
		queryName = name.replaceAll("-", "_");
		plaintextGenerator = new PlainOperator(this);
		
		double epsilon = Double.parseDouble(SystemConfiguration.getInstance().getProperty("epsilon")); 
		double delta = Double.parseDouble(SystemConfiguration.getInstance().getProperty("delta"));
		pCost = new PrivacyCost(epsilon, delta); //Epsilon Cost = Epsilon value * # relations touched (1 by default)
	}
	
	public void inferExecutionMode() {
		for(Operator op : children) {
			op.inferExecutionMode();
			
		}
		
		ExecutionMode maxChild = maxChildMode(); 
		SecurityPolicy maxAccess = maxAccessLevel(); // most sensitive attribute it computes on
		
		executionMode = new ExecutionMode(); // defaults to distributed oblivious, i.e. MPC
		executionMode.replicated = getSchema().isReplicated(); // inferred this during schema resolution
		
		
		//String msg = "For " + baseRelNode.getRelNode().getRelTypeName() + " have max child " + maxChild + " and max access " + maxAccess;
		//logger.info(msg);
		
		// if maxChild is DistributedClear or LocalClear and maxAccess is public, 
		// then set it equal to maxChild
		
		
		if(!maxChild.distributed) {
			if(!maxChild.oblivious) { // local-clear
				if(maxAccess == SecurityPolicy.Public) {
					executionMode.distributed = false;
					executionMode.oblivious = false; // sliced init'd t false
				}
				else { // oblivious child
					try {
						if(locallyRunnable()) {
							executionMode.distributed = false;					
						}
					} catch (Exception e) {
						e.printStackTrace();
						System.exit(-1);
					}
					
				}
			} else // max child is local-oblivious
				try {
					if(locallyRunnable()) {
						executionMode.distributed = false;					
					}
				} catch (Exception e) {
					e.printStackTrace();
					System.exit(-1);
				}
		
		}
		
		logger.info("For " + this + " inferred execution mode: " + executionMode);
		
		// return distributed-oblivious-!sliced
		
		// TODO: update this to support slicing, rework old slicing code
			

		
		
	}
	
	
	 
	// checks to see if we can use replication, partitioned-alike attributes, or unary ops
	// to run this locally
	protected boolean locallyRunnable()  throws Exception{
		
		boolean local = true;
		
		for(Operator child : children) { // either child is replicated or if binary op, have one input that is replicated
			if(child.getSchema().isReplicated())
				return true;
		}
		
		if(this instanceof Filter || this instanceof Project || this instanceof SeqScan) { // unary ops
			return true;
		}
		
		
		// if not partitioned-by well-known attrs
		if(!(config.getProperty("has-partitioning") == null || config.getProperty("has-partitioning").equals("true"))) {
			return false;
		}
		
		// a binary op where the instances are partitioned-alike
		if(children.size() == 2) {
			
			
			if(!(this instanceof Join)) {
				throw new Exception("Only binary operator supported is join!");
			}
			
			LogicalJoin join = (LogicalJoin) baseRelNode.getRelNode();
			List<RexNode> joinCondition = join.getChildExps();
			List<RexNode> predicates = RexUtil.flattenAnd(joinCondition);
			
			Operator lhsChild = children.get(0);
			Operator rhsChild = children.get(1);
			SecureRelRecordType lhsSchema = lhsChild.getSchema();
			SecureRelRecordType rhsSchema = rhsChild.getSchema();
			
			if(lhsSchema.isReplicated() || rhsSchema.isReplicated()) // automatically partitioned-alike
				return true;

			List<SecureRelDataTypeField> lhsFields = lhsSchema.getAttributes();
			List<SecureRelDataTypeField> rhsFields = rhsSchema.getAttributes();
			
			
			for(RexNode predicate : predicates) {
				SecureRelDataTypeField lhsPartitionBy = null;
				SecureRelDataTypeField rhsPartitionBy = null;
	
				List<SecureRelDataTypeField>  srcAttrs = AttributeResolver.getAttributes(predicate, this.getSchema());
				
				for(SecureRelDataTypeField aField : srcAttrs) {
					
					if(lhsFields.contains(aField)) {					
						if(lhsPartitionBy == null) {
							lhsPartitionBy = aField; // get first field from lhs that we compute on, assumes equality predicates
						}
						else {
							throw new Exception("Composite partitioning keys not yet implemented!");
						}
					}
						
					
						SecureRelDataTypeField rhsField = new SecureRelDataTypeField(aField.getName(), aField.getIndex() - lhsFields.size(), aField.getType());
						if(rhsFields.contains(rhsField)) {
							if(rhsPartitionBy == null) {
								rhsPartitionBy = aField; // get first field from rhs that we compute on
							}
							else {
								throw new Exception("Composite partitioning keys not yet implemented!");
							}
						}
					} // end schema matcher
			

				
				
					if(lhsPartitionBy == null || rhsPartitionBy == null) {
						throw new Exception("Did not find partitioning keys for matching!");
					}
		 
					
					if(!(Utilities.isLocalPartitionKey(lhsChild.getSchema(), lhsPartitionBy) && Utilities.isLocalPartitionKey(rhsChild.getSchema(), rhsPartitionBy)) )
						local = false;
					
					} // end for each predicate	
				return local;
			
		} // end if binary op
		
		// single input, e.g., aggregate
		List<SecureRelDataTypeField> fields = this.computesOn();
		for(SecureRelDataTypeField aField : fields) {
			if(!Utilities.isLocalPartitionKey(children.get(0).getSchema(), aField)) {
				local = false;
				}
			}
			
	   // end unary case
		return local;
	}
	
	
	
	private void debugFieldComparison(String msg, SecureRelDataTypeField aField, List<SecureRelDataTypeField> fields) throws Exception {
		for(SecureRelDataTypeField field : fields) {
			System.out.println("\n" + msg + ": Comparing " + aField + " to " + field + ": " + field.equals(aField));
			
		/*	System.out.println("Name: " + aField.getName().equals(field.getName()));

			System.out.println("Indices: " + aField.getIndex() + ", " + field.getIndex());
			System.out.println("Index: " + (aField.getIndex() == field.getIndex()));
			
			RelDataType schemaType = field.getType();
			RelDataType cmpType = aField.getType();
			
			System.out.println("Schema type: " + schemaType + ", matching to " + cmpType);
			System.out.println("Type: " + schemaType.equals(cmpType));
			*/
			
			if(field.equals(aField))
				break;
			
		
		}
		
	}

	// TODO: use this to manage attribute-level statistics as tuples move up the query tree
	// will need to be overridden in many classes
	public void initializeStatistics() {
		
		// base case: simply copy statistics from children
		// assumes child and parent have same schema
		
		SecureRelRecordType srcSchema = this.getInSchema();
		SecureRelRecordType schema = this.getSchema();
		assert(children.size() == 1);
		assert (schema == srcSchema);
		
		Operator child = children.get(0);
		child.initializeStatistics();
		schema.initializeStatistics(srcSchema);
		
	
	}
		
	public void addChild(Operator op) {
		children.add(op);
		op.setParent(this);
	}
	
	public void addChildren(List<Operator> ops) {
		children.addAll(ops);	
		for(Operator op : ops)
			op.setParent(this);
	}
	
	public void setParent(Operator op) {
		parent = op;
	}

	public SecureRelNode getSecureRelNode() {
		return baseRelNode;
	}
	
	public List<Operator> getChildren() {
		return children;
	}
	
	

	// what fields does this operator reveal information on?
	// some, like SeqScan and Project reveal nothing in their output based on the contents of the tuples they process
	// thus they "computeOn" nothing
	public List<SecureRelDataTypeField> computesOn() {
		return new ArrayList<SecureRelDataTypeField>();
	}

	protected SecurityPolicy maxAccessLevel() {
		List<SecureRelDataTypeField> accessed = computesOn();
		SecurityPolicy policy = SecurityPolicy.Public;
		
		for(SecureRelDataTypeField field : accessed) {
			if(field.getSecurityPolicy().compareTo(policy) >  0)
				policy = field.getSecurityPolicy();
		}
		
		return policy;
	}
	
	
	protected ExecutionMode maxChildMode() {
		ExecutionMode maxMode = children.get(0).executionMode;
		
		//join or other binary op
		if(children.size() == 2)  {
			ExecutionMode rhs = children.get(1).executionMode;
			if(rhs.distributed) 
				maxMode.distributed = true;
	
			if(rhs.oblivious)
				maxMode.oblivious = true;
			
		}

		return maxMode;
	}

	// schema of output 
	public SecureRelRecordType getSchema() {
		return baseRelNode.getSchema();
	}

	// schema of output 
	public SecureRelRecordType getSchema(boolean asSecureLeaf) {
		return baseRelNode.getSchema();
	}

	// very  basic case, simply copies child cardinality bound
	// TODO: override for join and other binary ops
	// TODO: write this for SeqScan too
	public long getCardinalityBound() {
		
		return baseRelNode.getSchema().getCardinalityBound();
	}
	
	// for all but SeqScan and join
	public SecureRelRecordType getInSchema() {
		return children.get(0).getSchema();
	}
	

	// for all except sort
	public int getLimit() {
		return  -1;
		
	}
	
	public List<SecureRelDataTypeField> getSliceAttributes() {
		return new ArrayList<SecureRelDataTypeField>();
	}
	
	boolean sliceAble() {
		if(this.sliceAgnostic == true && !this.maxChildMode().oblivious)
			return true;
		return false;
	}
	
	
	
	public String toString() {
		
		String ret = baseRelNode.getRelNode().getRelTypeName() + "-" + executionMode + ", schema:" + getSchema();
		List<SecureRelDataTypeField> sliceAttrs = this.getSliceAttributes();
		if(!sliceAttrs.isEmpty())
			ret += ", slice key: " + sliceAttrs;

		return ret;
	}
	
	// some operators have a slice key equal to that of their children
	// only covers attrs in key that appear in op's schema
	// examples: distinct, project
	protected List<SecureRelDataTypeField> derivedSliceKey() {
		
		assert(this.sliceAgnostic == true);
		List<SecureRelDataTypeField> baseKey;
		
		// if has a sliced child
		if(children.get(0).getExecutionMode().sliced) {
			baseKey = children.get(0).getSliceAttributes();
		}
		else { 
			Operator parentPtr = parent;
			while(parentPtr != null && parentPtr.sliceAgnostic) {
				parentPtr = parentPtr.parent;
			}

			// no well-defined slice keys
			if(parentPtr == null || parentPtr.sliceAgnostic)
				return new ArrayList<SecureRelDataTypeField>();
			
			baseKey = parentPtr.getSliceAttributes();
			
		}
		
		List<SecureRelDataTypeField> derivedKey = new ArrayList<SecureRelDataTypeField>();
		SecureRelRecordType schema = getSchema();
			
		for(SecureRelDataTypeField f : baseKey) {
			for(SecureRelDataTypeField l : schema.getSecureFieldList()) {
				if(l.equals(f)) {
					derivedKey.add(l);
					break;
				}
			}

		}

		
		return derivedKey;
	}



	public ExecutionMode getExecutionMode() {
		return executionMode;
	}



	public Operator getChild(int i) {
		return children.get(i);
	}


	
	
	public String getOpName() {
		String fullName = this.getClass().toString();
		int startIdx = fullName.lastIndexOf('.') + 1;
		return fullName.substring(startIdx);
	}

	public String getPackageName() {
		String opName = getOpName();

        return "org.smcql.generated." + queryName + "." + opName + operatorId;
	}


	
	public List<Operator> getSources() {
		return children;
	}


	// if a SMC operator implementation leverages the order of tuples for comparisons, codify that in this function
	// otherwise empty set = order agnostic
	// order is implicitly ascending
	public List<SecureRelDataTypeField> secureComputeOrder() {
		return new ArrayList<SecureRelDataTypeField>();
	}



	public String getOperatorId() {
		return operatorId;
	}



	public boolean isSplittable() {
		return splittable;
	}



	public String getQueryId() {
		return queryName;
	}



	public String destFilename(ExecutionMode e) {
        if(!e.distributed || (e.distributed && !e.oblivious)) {
        	return Utilities.getCodeGenTarget() + "/" + getQueryId() +  "/sql/" + getOperatorId() + "_" + getOpName() + ".sql";
        }

        return Utilities.getCodeGenTarget() + "/" + getQueryId() +  "/smc/" + getOperatorId() + "_" + getOpName() + ".lcc";
	}


	@Override
	public String generate() throws Exception {
		return plaintextGenerator.generate();
	}

	
	@Override
	public String generate(boolean asSecureLeaf) throws Exception {
		return new String();
	}

	public void compileIt() throws Exception {
		// nothing to do
	}


	
	public boolean sharesComputeOrder(Operator o) {
		return secureComputeOrder().equals(o.secureComputeOrder());
	}



	public boolean isBlocking() {
		return blocking;
	}



	public Operator getParent() {
		return parent;
	}



	public boolean sharesExecutionProperties(Operator op) {
		if(executionMode != op.getExecutionMode())
			return false;
		
		if(executionMode.sliced && executionMode.oblivious && op.getExecutionMode() == this.executionMode) {
			if(!SliceKeyDefinition.sliceCompatible(this, op)) {
				return false;
			}
			
		}
		return true;
	}



	public PlainOperator getPlainOperator() {
		return plaintextGenerator;
	}



	public SliceKeyDefinition getSliceKey() {
		return (this.sliceKey == null) ? new SliceKeyDefinition(this) : sliceKey;
	}
	
	public void addSqlGenerationNode(RelNode project) {
		if (project == null)
			return;
		
		RelNode next = project;
		RelNodeMatcher matcher = new RelNodeMatcher();
		if (matcher.matches(project, baseRelNode.getRelNode())) {
			unoptimizedRelNode = new ShadowRelNode(project);
			next = (project.getInputs().isEmpty()) ? null : project.getInput(0);
		}
		
		if (!children.isEmpty())
			children.get(0).addSqlGenerationNode(next);
	}

	public SecureOperator getSecureOperator() {
		return secureOperator;
	}

	public void setSecureOperator(SecureOperator secureOperator) {
		this.secureOperator = secureOperator;
	}
	
	@Override
	public boolean equals(Object o) {
		if (!(o instanceof Operator))
			return false;
		return this.toString().equals(o.toString());
	}
	
	//returns first non-Project, non-Filter child Op
	public Operator getNextValidChild() {
		if(children.isEmpty()) {
			return this;
		}
		
		Operator child = children.get(0);
		while (child instanceof Project || child instanceof Filter) {
			child = child.getChild(0);
		}
		return child;
	}
	
	@Override
	public int hashCode() {
		return new HashCodeBuilder(17, 31).append(this.toString()).toHashCode();
	}
	
	//TODO: Implement code to estimate cardinality of output (true cardinality plus number pulled from LaPlace distribution (which depends on the sensitivity of f aka the operator)
	public int getPrivateCardinalityEstimate() {		
		return -1;
	}
	
	public PrivacyCost getPrivacyCost() throws Exception {
		return pCost; 
	}
	
	protected int getEstimatedStability() {
		return 1;
	}
	
	public int getEstimatedSensitivity() {
		int max = 0;
		for (Operator child : children) {
			int cSensitivity = child.getEstimatedSensitivity();
			if (cSensitivity > max)
				max = cSensitivity;
		}
		return getEstimatedStability() + max;
	}
	
    public int getPerformanceCost(int n) {
        // n is the estimated input cardinality
        return 1; // Overriden in sub classes
    }
    
    
    public int getEstimatedCardinality() {
        // Overridden in SeqScan class
        return (int)(getReductionFactor()*children.get(0).getEstimatedCardinality());
    }
    
    
    public double getReductionFactor() {
        // Overridden in Filter class
        return 1.0;
    }

	public String getFunctionName() {
		String opName = getOpName();
		return opName + operatorId;

	}
	
	protected void lineUpCardinalityStatistics() {
		long minCard = children.get(0).getCardinalityBound();
		long localCard = 0;
		
		// find min cardinality for bound
		for(SecureRelDataTypeField f : getSchema().getSecureFieldList()) {
		
			localCard = f.getStatistics().getCardinality();
			if(localCard < minCard && localCard != -1) {
				minCard = localCard;
			}
		}

		// copy it to all fields in schema to make it consistent
		for(SecureRelDataTypeField f : getSchema().getSecureFieldList()) {
			f.getStatistics().setCardinality(minCard);
		}
		
	}

	// by default we pull up filter when generating SQL code
	public boolean pullUpFilter() {
		return true;
	}

	// replace child i with provided node
	/*public void replaceChild(int i, RelNode child) {
		
		children.set(i, child);
		
		
	}*/
	
	
}
