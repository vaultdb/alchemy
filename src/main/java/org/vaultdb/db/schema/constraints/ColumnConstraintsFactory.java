package org.vaultdb.db.schema.constraints;

import java.sql.Timestamp;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;


public class ColumnConstraintsFactory {

	@SuppressWarnings("rawtypes")
	public static ColumnConstraints get(SecureRelDataTypeField col) throws Exception {
		SqlTypeName type = col.getType().getSqlTypeName();
	
		
		switch(type) {
			case  DECIMAL:
			case DOUBLE:
			case FLOAT:
				return new ColumnConstraints<Float>(col);
			case INTEGER:
			case BIGINT:
				return new ColumnConstraints<Long>(col);
			case BOOLEAN:
				return new ColumnConstraints<Boolean>(col);
			case VARCHAR:
				return new ColumnConstraints<String>(col);
			case TIMESTAMP:
			case DATE:
				return new ColumnConstraints<Timestamp>(col);	
			default:
				throw new Exception("Column constraints not supported for type " + type + "!");
			}
		

		
	}
}
