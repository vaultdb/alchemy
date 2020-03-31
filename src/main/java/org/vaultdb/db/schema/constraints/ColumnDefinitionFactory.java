package org.vaultdb.db.schema.constraints;

import java.sql.Timestamp;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.util.Utilities;


public class ColumnDefinitionFactory {

	public static ColumnDefinition<?> get(SecureRelDataTypeField col) throws Exception {
		SqlTypeName type = col.getType().getSqlTypeName();
	
		
		switch(type) {
			case  DECIMAL:
			case DOUBLE:
			case FLOAT:
				return new ColumnDefinition<Float>(col);
			case INTEGER:
			case BIGINT:
				return new ColumnDefinition<Long>(col);
			case BOOLEAN:
				return new ColumnDefinition<Boolean>(col);
			case VARCHAR:
				return new ColumnDefinition<String>(col);
			case TIMESTAMP:
			case DATE:
				return new ColumnDefinition<Timestamp>(col);	
			default:
				throw new Exception("Column constraints not supported for type " + type + "!");
			}
		

		
	}

	public static ColumnDefinition<?> get(String table, String attr) throws Exception {
		SecureRelDataTypeField field = Utilities.lookUpAttribute(table, attr);
		return get(field);
	}
	
}
