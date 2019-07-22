package org.smcql.schema;

import org.smcql.BaseTest;
import org.smcql.db.schema.SecureSchemaLookup;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;

public class SchemaTest extends BaseTest {
	
	protected void setUp() throws Exception {
		super.setUp();
	}

	public void testHealthLNK() throws Exception {
		SecureSchemaLookup lookup = SecureSchemaLookup.getInstance();
		assertEquals(lookup.getPolicy("diagnoses", "patient_id"), SecurityPolicy.Public);
		assertEquals(lookup.getPolicy("diagnoses", "major_icd9"), SecurityPolicy.Protected);
		assertEquals(lookup.getPolicy("diagnoses", "icd9"), SecurityPolicy.Protected);
		assertEquals(lookup.getPolicy("medications", "patient_id"), SecurityPolicy.Public);
		assertEquals(lookup.getPolicy("medications", "medication"), SecurityPolicy.Protected);
	}
	
	
}
