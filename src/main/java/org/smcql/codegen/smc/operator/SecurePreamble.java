package org.smcql.codegen.smc.operator;

import java.net.InetAddress;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.smcql.compiler.emp.EmpParty;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.plan.operator.Operator;
import org.smcql.util.CodeGenUtils;

// setup for emp code
public class SecurePreamble extends SecureOperator {

    /**
     * Generate preamble for emp code
     */
    private static final long serialVersionUID = 1L;

    Vector<String> sqlStatements = null;

    public SecurePreamble(Operator o) throws Exception {
        super(o); // store op in planNode
    }

    @Override
    public String generate() throws Exception {
        Map<String, String> variables = new HashMap<String, String>();

        ConnectionManager connectionManager = ConnectionManager.getInstance();
        String aliceKey = connectionManager.getAlice();
        WorkerConfiguration aliceConfig = connectionManager.getWorker(aliceKey);
        String aliceHost = aliceConfig.hostname;
        // in case it is a hostname and not an IP address, resolve it
        InetAddress address = InetAddress.getByName(aliceHost);
        aliceHost = address.getHostAddress();

        String bobKey = connectionManager.getBob();
        WorkerConfiguration bobConfig = connectionManager.getWorker(bobKey);
        String bobHost = bobConfig.hostname;
        // in case it is a hostname and not an IP address, resolve it
        address = InetAddress.getByName(bobHost);
        bobHost = address.getHostAddress();

        variables.put("bobHost", bobHost);
        variables.put("aliceHost", aliceHost);
        variables.put("queryName", planNode.getQueryId());
        variables.put("queryClass", planNode.getQueryId() + "Class");

        // variables.put("srcSQL", queries);
        String alice = ConnectionManager.getInstance().getConnectionString(1);
        String bob = ConnectionManager.getInstance().getConnectionString(2);

        variables.put("aliceConnectionString", alice);
        variables.put("bobConnectionString", bob);

        // result.put(getPackageName(),
        // CodeGenUtils.generateFromTemplate("aggregate/singular/full/aggregate.txt", variables));
        return CodeGenUtils.generateFromTemplate("util/preamble.txt", variables);
    }

    public String generate(EmpParty party) throws Exception {
        Map<String, String> variables = new HashMap<String, String>();

        ConnectionManager connectionManager = ConnectionManager.getInstance();
        SystemConfiguration config = SystemConfiguration.getInstance();
        String aliceKey = connectionManager.getAlice();
        WorkerConfiguration aliceConfig = connectionManager.getWorker(aliceKey);
        String aliceHost = aliceConfig.hostname;

        String bobKey = connectionManager.getBob();
        WorkerConfiguration bobConfig = connectionManager.getWorker(bobKey);
        String bobHost = bobConfig.hostname;

        variables.put("bobHost", bobHost);
        variables.put("aliceHost", aliceHost);
        String queryName = planNode.getQueryId() + party.asString();

        variables.put("queryName", queryName);
        variables.put("queryClass", queryName + "Class");
        variables.put("party", Integer.toString(party.asInt()));
        variables.put("port", config.getProperty("emp-port"));

        String alice = ConnectionManager.getInstance().getConnectionString(1);
        String bob = ConnectionManager.getInstance().getConnectionString(2);

        variables.put("aliceConnectionString", alice);
        variables.put("bobConnectionString", bob);

        return CodeGenUtils.generateFromTemplate("util/preamble.txt", variables);
    }
}
