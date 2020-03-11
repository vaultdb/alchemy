package org.vaultdb.db.schema;

import java.util.List;

import org.apache.calcite.rel.RelReferentialConstraint;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rex.RexNode;

// keeps track of any constraints associated with a table in the DBMS
public class TableConstraints {
	
	List<List<RelDataTypeField> > keys; // primary keys or UNIQUE constraints
	
	List<RexNode> constraints; // arbitrary expressions
	
	List<RelReferentialConstraint> integrityConstraints; // foreign key relationships
	

}
