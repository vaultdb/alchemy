package org.vaultdb.schema;

import org.vaultdb.TpcHBaseTest;
import org.vaultdb.db.schema.TableConstraints;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.WorkerConfiguration;

public class TableConstraintsTest  extends TpcHBaseTest {
	
	
	
	public void testLineitemConstraints() throws Exception {
		TableConstraints lineitem = new TableConstraints( "lineitem");
		
		
	}
	  
	  

}
