package org.vaultdb.codegen;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.calcite.util.Pair;
import org.apache.commons.io.FileUtils;
import org.vaultdb.codegen.plaintext.PlainOperator;
import org.vaultdb.codegen.smc.operator.SecureOperator;
import org.vaultdb.codegen.smc.operator.SecureOperatorFactory;
import org.vaultdb.codegen.smc.operator.SecurePreamble;
import org.vaultdb.codegen.smc.operator.support.UnionMethod;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.RunConfig;
import org.vaultdb.executor.smc.ExecutionSegment;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.executor.step.PlaintextStep;
import org.vaultdb.executor.step.SecureStep;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.plan.operator.Filter;
import org.vaultdb.plan.operator.Join;
import org.vaultdb.plan.operator.Operator;
import org.vaultdb.plan.operator.Project;
import org.vaultdb.plan.operator.Sort;
import org.vaultdb.plan.operator.WindowAggregate;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.ClassPathUpdater;
import org.vaultdb.util.CodeGenUtils;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;


public class QueryCompiler {

  Map<String, String> sqlCode;
  Map<String, Pair<Long, Long> >  obliviousCardinalities; // pair<alice, bob>
  Map<ExecutionStep, String> smcCode;
  Map<Operator, ExecutionStep> allSteps;
  String queryId;
  List<String> smcFiles;
  List<String> sqlFiles;
  List<ExecutionSegment> executionSegments;
  String userQuery = null;
  SecureRelRoot queryPlan;
  ExecutionStep compiledRoot;
  ConnectionManager connections;
  
  boolean codeGenerated = false;
  SecureRelRecordType outSchema = null;
  Logger logger;

  public QueryCompiler(SecureRelRoot q) throws Exception {

    queryPlan = q;
    outSchema = q.getPlanRoot().getSchema();
    smcFiles = new ArrayList<String>();
    sqlFiles = new ArrayList<String>();
    sqlCode = new HashMap<String, String>();
    executionSegments = new ArrayList<ExecutionSegment>();
    allSteps = new HashMap<Operator, ExecutionStep>();
    obliviousCardinalities = new HashMap<String, Pair<Long, Long> >();
    connections = ConnectionManager.getInstance();
    smcCode = new HashMap<ExecutionStep, String>();
    
    queryId = q.getName();
    Operator root = q.getPlanRoot();
    logger = SystemConfiguration.getInstance().getLogger();
    
    

    // single plaintext executionstep if no secure computation detected
    if (!root.getExecutionMode().distributed) {
      compiledRoot = generatePlaintextStep(root);
      ExecutionSegment segment = createSegment(compiledRoot);
      executionSegments.add(segment);
      compiledRoot.getExec().parentSegment = segment;
    } else { // recurse
      compiledRoot = addOperator(root, new ArrayList<Operator>());
    }

    inferExecutionSegment(compiledRoot);
  }

  public QueryCompiler(SecureRelRoot q, String sql) throws Exception {

    queryPlan = q;
    outSchema = q.getPlanRoot().getSchema();
    logger = SystemConfiguration.getInstance().getLogger();

    smcFiles = new ArrayList<String>();
    sqlFiles = new ArrayList<String>();
    sqlCode = new HashMap<String, String>();
    smcCode = new HashMap<ExecutionStep, String>();
    executionSegments = new ArrayList<ExecutionSegment>();
    userQuery = sql;
    connections = ConnectionManager.getInstance();
    obliviousCardinalities = new HashMap<String, Pair<Long, Long> >();

    
    allSteps = new HashMap<Operator, ExecutionStep>();

    queryId = q.getName();
    Operator root = q.getPlanRoot();

    // single plaintext executionstep if no secure computation detected
    if (!root.getExecutionMode().distributed) {
      compiledRoot = generatePlaintextStep(root);
      ExecutionSegment segment = createSegment(compiledRoot);
      executionSegments.add(segment);
      compiledRoot.getExec().parentSegment = segment;
    } else { // recurse
      compiledRoot = addOperator(root, new ArrayList<Operator>());
    }

    inferExecutionSegment(compiledRoot);
  }


  public String getQueryId() {
    return queryId;
  }

  public List<ExecutionSegment> getSegments() {
    return executionSegments;
  }


  public void setCodeGenerated(boolean val) {
    codeGenerated = val;
  }

  // TODO(madhavsuresh): document this function call
  public String writeOutEmpFile() throws Exception {
    String empCode = getEmpCode();
    return writeOutEmpFile(empCode);
  }
  
  
  // returns filename (but only for C++ code, not jni wrapper.
  // the intent of this function is to write out all code
  public String writeOutEmpFile(String empCode) throws Exception {

    String targetFile = Utilities.getCodeGenTarget() + "/" + queryId + ".h";
    Logger logger = SystemConfiguration.getInstance().getLogger();
    logger.log(Level.INFO, "QueryCompiler writing generated code to " + targetFile);

    FileUtilities.writeFile(targetFile, empCode);


    String jniFile = Utilities.getCodeGenTarget() + "/" + queryId + ".java";
    EmpJniUtilities.createJniWrapper(queryId, jniFile, sqlCode, obliviousCardinalities);
    codeGenerated = true;

    return targetFile;
  }

  
  // This only writes out C++ code, not the jni code.
  public String getEmpCode() throws Exception {
    String wholeFile = new String();

    SecurePreamble preamble = new SecurePreamble(getRoot().getSourceOperator());

    String generatedPreamble = preamble.generate();
    wholeFile = generatedPreamble;

    // traverse the tree bottom-up in our control flow
    Pair<String, String> code = empCodeGeneratorHelper((SecureStep) compiledRoot);
    wholeFile += code.left; // add functions

    String rootOutput = compiledRoot.getFunctionName() + "Output";

    wholeFile += generateEMPMain(code.right, rootOutput); // plug in function calls

    return wholeFile;
  }

  public Map<String,String> getEmpInputs() throws Exception {
    // Method to return map of Sql inputs.
    // Step is the key, and the value is the associated query

    Map<String, String> inputs = new HashMap<String, String>();
    Iterator itr = sqlCode.entrySet().iterator();

    while (itr.hasNext()) {
      Map.Entry entry = (Map.Entry) itr.next();
      ExecutionStep step = (ExecutionStep) entry.getKey();
      String functionName = step.getFunctionName();
      inputs.put(functionName, (String) entry.getValue());
    }


    return inputs;
  }


  public void compileEmpCode() throws Exception {
    if (!codeGenerated) writeOutEmpFile();

    String className = EmpJniUtilities.getFullyQualifiedClassName(queryId);
    EmpJniUtilities.buildEmpProgram(className);
  }

  // generate program flow by traversing the tree bottom-up
  private Pair<String, String> empCodeGeneratorHelper(SecureStep step) {

    String functions = new String();
    String calls = new String();

    String myCode = smcCode.get(step) + "\n";
    String myCall = generateFunctionCall(step) + "\n";
    if (step.isUnion()) {
      functions = myCode;
      calls = myCall;
      return new Pair<String, String>(functions, calls);
    }

    for (ExecutionStep child : step.getChildren()) {
      Pair<String, String> childCode = empCodeGeneratorHelper((SecureStep) child);
      functions += childCode.left;
      calls += childCode.right;
    }

    functions += myCode;
    calls += myCall;

    return new Pair<String, String>(functions, calls);
  }

  private String generateFunctionCall(ExecutionStep srcStep) {
    // desire: Data *$functionName+ "Output" = $functionName(childOutput...)

    String functionName = srcStep.getFunctionName();

    if (((SecureStep) srcStep).isUnion()) {
      String call = "     Data *" + functionName + "Output = " + functionName + "(party, io);\n";
      return call;
    }

    List<ExecutionStep> children = srcStep.getChildren();
    String args = children.get(0).getFunctionName() + "Output";
    for (int i = 1; i < children.size(); ++i) {
      args += ", " + children.get(i).getFunctionName() + "Output";
    }
    String outputVariable = functionName + "Output";
    String call = "    Data * " + outputVariable + " = " + functionName + "(" + args + ");\n";

    return call;
  }

  private String generateEMPMain(String functions, String rootOutput) throws IOException {

    Map<String, String> variables = new HashMap<String, String>();

    variables.put("functions", functions);
    variables.put("rootOutput", rootOutput);
    String generatedCode = CodeGenUtils.generateFromTemplate("util/main.txt", variables);

    return generatedCode;
  }

  public List<String> getClasses() throws IOException, InterruptedException {

    File path = new File(Utilities.getCodeGenTarget() );
    String[] extensions = new String[1];
    extensions[0] = "class";
    Collection<File> files = FileUtils.listFiles(path, extensions, true);
    List<String> filenames = new ArrayList<String>();

    for (File f : files) {
      filenames.add(f.toString());
    }
    return filenames;
  }

  public void loadClasses()
      throws IOException, InterruptedException, NoSuchMethodException, IllegalAccessException,
          InvocationTargetException {
    List<String> classFiles = getClasses();
    for (String classFile : classFiles) {
      ClassPathUpdater.add(classFile);
    }
  }

  public ExecutionStep getRoot() {
    return compiledRoot;
  }

  private ExecutionStep addOperator(Operator o, List<Operator> opsToCombine) throws Exception {

	  
    /*Deprecated: intermediate results are now registered in the run method of EMP program
	
     * if (o instanceof CommonTableExpressionScan) {
      Operator child = o.getSources().get(0);
      //SecureBufferPool.getInstance().addPointer(o.getPackageName(), child.getPackageName());
    }*/

    if (allSteps.containsKey(o)) {
      return allSteps.get(o);
    }

    if (!o.getExecutionMode().distributed) { // child of a secure leaf
      return generatePlaintextStep(o);
    }

    // If we have a root project, push it down to the next op
    if (o instanceof Project && o.getParent() == null) {
      opsToCombine.add(o);
      o = o.getChild(0);
    }

    // secure case
    List<ExecutionStep> merges = new ArrayList<ExecutionStep>();
    List<ExecutionStep> localChildren = new ArrayList<ExecutionStep>();
    for (Operator child : o.getSources()) {
      List<Operator> nextToCombine = new ArrayList<Operator>();
      while (child instanceof Filter || child instanceof Project) { // Aggregate NYI, needs group-by
        if (child instanceof Filter && child.getExecutionMode().distributed) {
          opsToCombine.add(child);
        } else {
          nextToCombine.add(child);
        }
        child = child.getChild(0);
      }

      if (!child.getExecutionMode().equals(o.getExecutionMode())) { // usually a secure leaf
        ExecutionStep childSource = null;
        Operator tmp = o;
        if (!child.getExecutionMode().distributed) {
          Operator plain = (o.isSplittable() && !(o instanceof WindowAggregate)) ? o : child;
          // TODO: figure out why generate plaintext step is running on JdbcTableScan instead of aggregate
          childSource = generatePlaintextStep(plain);
        } else {
          childSource = addOperator(child, nextToCombine);
        }
        ExecutionStep mergeStep = addMerge(tmp, childSource);
        childSource.setParent(mergeStep);
        localChildren.add(mergeStep);
        merges.add(mergeStep);
      } else {
        ExecutionStep e = addOperator(child, nextToCombine);
        localChildren.add(e);
      }
    } // end iterating over children

    ExecutionStep secStep = null;
    if (o instanceof Sort) {
      Operator sortChild = o.getChild(0);
      if (CodeGenUtils.isSecureLeaf(o)
          || sortChild.sharesComputeOrder(o)) { // implement splittable join
        secStep = localChildren.get(0);
      }
    }

    if (secStep == null) {
      secStep = generateSecureStep(o, localChildren, opsToCombine, merges);
    }

    return secStep;
  }

  // adds the given step to the global collections and adds execution information to the step
  private void processStep(PlaintextStep step) throws Exception {
    Operator op = step.getSourceOperator();
    String sql = op.generate();

    Pair<Long, Long> stepCardinalities = op.obliviousCardinality();
    
    allSteps.put(op, step);
    sqlCode.put(step.getFunctionName(), sql);
    obliviousCardinalities.put(step.getFunctionName(), stepCardinalities);
    
  }

  // creates the PlaintextStep plan from the given operator and parent step
  private PlaintextStep generatePlaintextStep(Operator op, PlaintextStep prevStep)
      throws Exception {
    RunConfig pRunConf = new RunConfig();
    pRunConf.port = 54321; // does not matter for plaintext

    if (prevStep == null) {
      PlaintextStep result = new PlaintextStep(op, pRunConf, null);
      processStep(result);
      return result;
    }

    PlaintextStep curStep = prevStep;
    if (op.isBlocking() || op.getParent() == null || prevStep == null) {
      curStep = new PlaintextStep(op, pRunConf, null);
      curStep.setParent(prevStep);
      if (prevStep != null) prevStep.addChild(curStep);
      processStep(curStep);
    }

    for (Operator child : op.getChildren()) {
      PlaintextStep nextStep = generatePlaintextStep(child, curStep);
      if (nextStep != null) {
        curStep.addChild(nextStep);
        nextStep.setParent(curStep);
      }
    }

    return curStep;
  }

  private ExecutionStep generatePlaintextStep(Operator op) throws Exception {
    return generatePlaintextStep(op, null);
  }

  // join not yet implemented for split execution
  // child.getSourceOp may be equal to op for split execution
  private ExecutionStep addMerge(Operator op, ExecutionStep child) throws Exception {
    // merge input tuples with other party
    UnionMethod merge = null;
    if (op instanceof Join) { // inserts merge for specified child
      Operator childOp = child.getSourceOperator();
      Join joinOp = (Join) op;
      Operator leftChild = joinOp.getChild(0).getNextValidChild();
      Operator rightChild = joinOp.getChild(1).getNextValidChild();

      boolean isLhs = (leftChild == childOp);
      List<SecureRelDataTypeField> orderBy =
          (isLhs) ? leftChild.secureComputeOrder() : rightChild.secureComputeOrder();
     
      merge = new UnionMethod(op, child, orderBy);

    } 
    else {
    	
      merge = new UnionMethod(op, child, op.secureComputeOrder());

    }
    // merge.compileIt();

    RunConfig mRunConf = new RunConfig();
    mRunConf.port = (SystemConfiguration.getInstance()).readAndIncrementPortCounter();

    /* Deprecated: intermediate results are now registered in the run method of EMP program
    if (child.getSourceOperator() instanceof CommonTableExpressionScan) {
      String src = child.getPackageName();
      String dst = merge.getPackageName();
      SecureBufferPool.getInstance().addPointer(src, dst);
    }*/
    

    SecureStep mergeStep = new SecureStep(merge, op, mRunConf, child, null);
    child.setParent(mergeStep);
    String code  = merge.generate();
    smcCode.put(mergeStep, code);
    return mergeStep;
  }

  private ExecutionStep generateSecureStep(
      Operator op,
      List<ExecutionStep> children,
      List<Operator> opsToCombine,
      List<ExecutionStep> merges)
      throws Exception {
    SecureOperator secOp = SecureOperatorFactory.get(op);
    // secOp is null for filter and projection because we coalesce those with their neighbors

    if (!merges.isEmpty()) secOp.setMerges(merges);

    for (Operator cur : opsToCombine) {
      if (cur instanceof Filter) {
        secOp.addFilter((Filter) cur);
      } else if (cur instanceof Project) {
        secOp.addProject((Project) cur);
      }
    }
    // TODO: save this for later once we have whole file
    // secOp.compileIt();

    RunConfig sRunConf = new RunConfig();

    sRunConf.port = (SystemConfiguration.getInstance()).readAndIncrementPortCounter();
    sRunConf.host = getAliceHostname();

    SecureStep smcStep = null;

    if (children.size() == 1) {
      ExecutionStep child = children.get(0);
      smcStep = new SecureStep(secOp, op, sRunConf, child, null);
      child.setParent(smcStep);
    } else if (children.size() == 2) { // join, set ops (intersection/union)
      ExecutionStep lhsChild = children.get(0);
      ExecutionStep rhsChild = children.get(1);
      smcStep = new SecureStep(secOp, op, sRunConf, lhsChild, rhsChild);
      lhsChild.setParent(smcStep);
      rhsChild.setParent(smcStep);
    } else {
      throw new Exception("Operator cannot have >2 children.");
    }

    allSteps.put(op, smcStep);
    String code = secOp.generate();
    smcCode.put(smcStep, code);

    return smcStep;
  }

  public Map<ExecutionStep, String> getSMCCode() {
    return smcCode;
  }

  public Map<String, String> getSQLCode() {
    return sqlCode;
  }

  private String getAliceHostname() throws Exception {
    String alice = connections.getAlice();
    return connections.getWorker(alice).hostname;
  }

  private void inferExecutionSegment(ExecutionStep step) throws Exception {
    if (step instanceof PlaintextStep) return;

    SecureStep secStep = (SecureStep) step;

    if (secStep.getExec().parentSegment != null) {
      return;
    }

    // if root node
    if (secStep.getParent() == null) {
      ExecutionSegment segment = createSegment(secStep);
      executionSegments.add(segment);
      secStep.getExec().parentSegment = segment;

    } else { // non-root
      Operator parentOp = secStep.getParent().getSourceOperator();
      Operator localOp = secStep.getSourceOperator();
      SecureStep parent = (SecureStep) step.getParent();

      if (localOp.sharesExecutionProperties(parentOp)) { // same segment
        secStep.getExec().parentSegment = parent.getExec().parentSegment;

      } else { // create new segment
        ExecutionSegment current = createSegment(secStep);
        executionSegments.add(current);
        secStep.getExec().parentSegment = current;
      }
    }

    List<ExecutionStep> sources = secStep.getChildren();
    for (ExecutionStep s : sources) inferExecutionSegment(s);
  }

  public SecureRelRoot getPlan() {
    return queryPlan;
  }

  private ExecutionSegment createSegment(ExecutionStep secStep) throws Exception {
    ExecutionSegment current = new ExecutionSegment();
    current.rootNode = secStep.getExec();

    current.runConf = secStep.getRunConfig();
    current.outSchema = new SecureRelRecordType(secStep.getSchema());
    current.executionMode = secStep.getSourceOperator().getExecutionMode();

    if (secStep.getSourceOperator().getExecutionMode().sliced
        && userQuery != null) {
      current.sliceSpec = secStep.getSourceOperator().getSliceKey();

      PlainOperator sqlGenRoot = secStep.getSourceOperator().getPlainOperator();
      
      if(!SystemConfiguration.getInstance().slicingEnabled()) {
    	  sqlGenRoot.inferSlicePredicates(current.sliceSpec);
    	  current.sliceValues = sqlGenRoot.getSliceValues();
    	  current.complementValues = sqlGenRoot.getComplementValues();
    	  current.sliceComplementSQL =
    			  sqlGenRoot.generatePlaintextForSliceComplement(
    					  userQuery); // plaintext query for single site values
      }
    }
    return current;
  }

  Byte[] toByteObject(byte[] primitive) {
    Byte[] bytes = new Byte[primitive.length];
    int i = 0;
    for (byte b : primitive) bytes[i++] = Byte.valueOf(b);
    return bytes;
  }

  public SecureRelRecordType getOutSchema() {
    return outSchema;
  }
}
