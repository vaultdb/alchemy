package org.smcql.codegen.smc.operator;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.core.RelFactories;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Table;
import org.gridkit.internal.com.jcraft.jsch.Logger;
import org.smcql.codegen.smc.operator.support.ProcessingStep;
import org.smcql.codegen.smc.operator.support.TypeCorrecter;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Join;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.Utilities;

public class SecureJoin extends SecureOperator {

	/**
	 * 
	 */
	private static final long serialVersionUID = 7379127156933722928L;

	public SecureJoin(Operator o) throws Exception {
		super(o);
		
		assert(o instanceof Join);
		
		
		//Add any join conditions as filters
		LogicalJoin j = (LogicalJoin) o.getSecureRelNode().getRelNode();

		// it is getting stuck because we need a composite schema rather than just that of the first child
        RexNode selectionCriteria = j.getCondition();
        
        SystemConfiguration config = SystemConfiguration.getInstance();

        java.util.logging.Logger logger = config.getLogger();
        SecureRelRecordType schema = o.getSchema();
        RelDataTypeField cCustKey = schema.getAttributes().get(1).getBaseField();
        RelDataTypeField oCustKey = schema.getAttributes().get(3).getBaseField();

        SecureRelDataTypeField cCustKeyOriginal = Utilities.lookUpAttribute("customer", "c_custkey");
        SecureRelDataTypeField oCustKeyOriginal = Utilities.lookUpAttribute("orders", "o_custkey");
        
        
        logger.info("Processing join with selection criteria: " + selectionCriteria + " with schema "  + schema);
        logger.info("Fields: " + cCustKey + " " + cCustKey.getType().isNullable() +  ", " +  oCustKey + " " + oCustKey.getType().isNullable());
        // it starts out with the nullability correct, where does it lose this for o_custkey?
        logger.info("Base fields: " + cCustKeyOriginal + " " + cCustKeyOriginal.getType().isNullable() +  ", " +  oCustKeyOriginal + " " + oCustKeyOriginal.getType().isNullable());
        // might be in how we initialize Join
        // one side becomes nullable because of LEFT join
        // need to make the other side nullable too

        // need to write a RexVisitor that will correct any type mismatches on nullability
        TypeCorrecter correcter = new TypeCorrecter();

        RexNode lhs = ((RexCall) selectionCriteria).getOperands().get(0);
        RexNode rhs = ((RexCall) selectionCriteria).getOperands().get(1);
        
        logger.info("Nullability for rex: " + lhs.getType().isNullable() + ", " + rhs.getType().isNullable());
        
        selectionCriteria =  selectionCriteria.accept(correcter);
        
        // LogicalFilter create is checking out RexChecker.inputTypeList and seeing that the NOT NULL input of rhs of left join is not matched with nullable input of left join.
        // should probably bite the bullet and just accommodate join predicates in-situ in LogicalJoin
        RelNode  f = LogicalFilter.create(j, selectionCriteria);

		//SecureRelNode[] nodeChildren = Arrays.copyOf(o.getSecureRelNode().getChildren().toArray(), o.getSecureRelNode().getChildren().size(), SecureRelNode[].class);
		Operator[] opChildren = Arrays.copyOf(o.getChildren().toArray(), o.getChildren().size(), Operator[].class);
		SecureRelNode src = new SecureRelNode(f, o.getSecureRelNode());
		
		Filter filter = new Filter(this.getPackageName(), src, opChildren, (Join) o);
		this.addFilter(filter);
	}

	@Override
	public Map<String, String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();		

		Join join = (Join) planNode;
		String lSize = Integer.toString(join.getChild(0).getSchema().size());
		String rSize = Integer.toString(join.getChild(1).getSchema().size());

		variables.put("lSize", lSize);
		variables.put("rSize", rSize);
		//String dstSize = (projects.isEmpty()) ? variables.get("sSize") : Integer.toString(projects.get(0).getSchema().size());
		//variables.put("dSize", dstSize);
				
		if(join.getCondition() == null) {
			generatedCode =  CodeGenUtils.generateFromTemplate("join/cross.txt", variables);	
		}
		else {	
			generatedCode =  CodeGenUtils.generateFromTemplate("join/simple.txt", variables);
		}
		
		Map<String, String> result = new HashMap<String, String>();
		result.put(getPackageName(), generatedCode);
		
		for (ProcessingStep p : processingSteps)
			result.put(getPackageName() + "." + p.getProcessName(), p.generate(variables));
		
		return result;
	}
	
	
 	

}
