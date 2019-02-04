package org.smcql.db.schema.statistics;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;

// for later when we templatize
public class FieldStatisticsFactory {
/*
	public class SecureRelDataTypeFieldFactory {
		   
		   /******* Factories for setting up attributes with statistics *********

		   public static SecureRelDataTypeField<?> get(String name, int index, RelDataType type) {
			   switch(type.getSqlTypeName()){
			   	case INTEGER:
			   	case BIGINT:
				   	return new SecureRelDataTypeField<Long>(name, index, type);
			   	case TIMESTAMP:
				   return new SecureRelDataTypeField<java.util.Date>(name, index, type);
			   	case BOOLEAN:
					   return new SecureRelDataTypeField<Boolean>(name, index, type);
			   	case VARCHAR:
				   return new SecureRelDataTypeField<String>(name, index, type);

			   	default: // unsupported type!
			   		return null; 
			   		
			   }
			   
		   }
		   
		   public static SecureRelDataTypeField<?> get(RelDataTypeField baseField, SecurityPolicy secPolicy, String aStoredTable,
					String aStoredAttribute, LogicalFilter aFilter) {
			   switch(baseField.getType().getSqlTypeName()){
			   	case INTEGER:
			   	case BIGINT:
				   	return new SecureRelDataTypeField<Long>(baseField, secPolicy, aStoredTable, aStoredAttribute, aFilter);
			   	case TIMESTAMP:
				   return new SecureRelDataTypeField<java.util.Date>(baseField, secPolicy, aStoredTable, aStoredAttribute, aFilter);
			   	case BOOLEAN:
					   return new SecureRelDataTypeField<Boolean>(baseField, secPolicy, aStoredTable, aStoredAttribute, aFilter);
			   	case VARCHAR:
				   return new SecureRelDataTypeField<String>(baseField, secPolicy, aStoredTable, aStoredAttribute, aFilter);

			   	default: // unsupported type!
			   		return null; 
			   		
			   }

			   
		   }
		   

	}
*/
}
