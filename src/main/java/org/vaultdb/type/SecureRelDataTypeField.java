package org.vaultdb.type;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rel.type.RelDataTypeFieldImpl;
import org.apache.calcite.rel.type.RelDataTypeSystem;
import org.apache.calcite.sql.type.SqlTypeFactoryImpl;
import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.db.schema.constraints.ColumnDefinition;
import org.vaultdb.db.schema.constraints.ColumnDefinitionFactory;

// thin wrapper on top of RelDataTypeField for attaching security policy to an attribute
// how do we call this at schema construction time?
// create a secure table
// instantiate SecureTable where we do "Table table = schema.getTable(tableName);" now
public class SecureRelDataTypeField extends RelDataTypeFieldImpl implements Serializable {

  /** */
  private static final long serialVersionUID = 1L;

  public enum SecurityPolicy {
    Public,
    Protected,
    Private
  };

  SecurityPolicy policy = SecurityPolicy.Private;
  RelDataTypeField baseField;
  ColumnDefinition<?> constraints = null; // TODO: initialize this
  private boolean aliased;
  private String unaliasedName;

  // both of below are null if field sourced from greater than one attribute
  // stored in original tables
  private String storedTable;
  private String storedAttribute;
  transient List<LogicalFilter> filters;

  public SecureRelDataTypeField(String name, int index, RelDataType type) {
    super(name, index, type);
    filters = new ArrayList<LogicalFilter>();


  }

  public SecureRelDataTypeField(
      RelDataTypeField baseField, SecurityPolicy secPolicy, String aStoredTable) {
    super(baseField.getName(), baseField.getIndex(), baseField.getType());
    this.baseField = baseField;
    policy = secPolicy;
    filters = new ArrayList<LogicalFilter>();
    storedTable = aStoredTable;

  
 
  }

  public SecureRelDataTypeField(
      RelDataTypeField baseField, SecurityPolicy secPolicy, ColumnDefinition theStats) {
    super(baseField.getName(), baseField.getIndex(), baseField.getType());
    this.baseField = baseField;
    policy = secPolicy;
    filters = new ArrayList<LogicalFilter>();
  }

  public SecureRelDataTypeField(
      RelDataTypeField baseField,
      SecurityPolicy secPolicy,
      String aStoredTable,
      String aStoredAttribute,
      LogicalFilter aFilter)
      throws Exception {
    super(baseField.getName(), baseField.getIndex(), baseField.getType());
    this.baseField = baseField;
    policy = secPolicy;
    setStoredTable(aStoredTable);
    setStoredAttribute(aStoredAttribute);
    filters = new ArrayList<LogicalFilter>();
    addFilter(aFilter);

  }

  // quasi-copy constructor
  public SecureRelDataTypeField(RelDataTypeField aBaseField, SecureRelDataTypeField src)  {
    super(aBaseField.getName(), src.getBaseField().getIndex(), src.getBaseField().getType());
    if (!src.getName().equals(aBaseField.getName())) {
      aliased = true;
    }
    unaliasedName = src.getName();
    baseField = aBaseField;
    policy = src.getSecurityPolicy();
    storedTable = src.getStoredTable();
    storedAttribute = src.getStoredAttribute();
    filters = new ArrayList<LogicalFilter>(src.getFilters());
    // TODO: deep copy constraints
    constraints = src.getColumnDefinition();


  }

  private void writeObject(ObjectOutputStream out) throws IOException {
    List<String> field = new ArrayList<String>();
    field.add(Integer.toString(baseField.getIndex()));
    field.add(baseField.getName());
    field.add(Integer.toString(baseField.getType().getPrecision()));
    field.add(Integer.toString(baseField.getType().getScale()));

    out.writeObject(field);
    out.writeObject(baseField.getType().getSqlTypeName());
    out.writeObject(storedTable);
    out.writeObject(storedAttribute);
    out.writeObject(policy);
  }

  @SuppressWarnings("unchecked")
  private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
    List<String> field = (List<String>) ois.readObject();
    SqlTypeName stn = (SqlTypeName) ois.readObject();

    int index = Integer.parseInt(field.get(0));
    String name = field.get(1);
    int precision = Integer.parseInt(field.get(2));
    int scale = Integer.parseInt(field.get(3));

    SqlTypeFactoryImpl factory = new SqlTypeFactoryImpl(RelDataTypeSystem.DEFAULT);
    baseField = new RelDataTypeFieldImpl(name, index, factory.createSqlType(stn, precision, scale));
    storedTable = (String) ois.readObject();
    storedAttribute = (String) ois.readObject();
    policy = (SecurityPolicy) ois.readObject();
  }

  public int size() {
    TypeMap tMap = TypeMap.getInstance();
    return tMap.sizeof(this);
  }

  // for table scans
  public void initializeConstraints() throws Exception {
	  constraints = ColumnDefinitionFactory.get(this);
  }

  // copy constructor
  @SuppressWarnings("unchecked")
public void initializeStatistics(ColumnDefinition<?> c) {
    constraints = new ColumnDefinition(this, c);
  }

  @Override
  public int getIndex() {
    if (baseField == null) return super.getIndex();
    return baseField.getIndex();
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) {
      return true;
    }

    if (!(obj instanceof SecureRelDataTypeField)) {
      return ((RelDataTypeFieldImpl) this).equals(obj); // default to superclass
    }

    SecureRelDataTypeField that = (SecureRelDataTypeField) obj;
    if (this.getIndex() == that.getIndex()
        && this.getName().equals(that.getName())
        && this.getType().equals(that.getType())) return true;

    return false;
  }

  public SecurityPolicy getSecurityPolicy() {
    return policy;
  }

  public void setSecurityPolicy(SecurityPolicy policy) {
    this.policy = policy;
  }

  public RelDataTypeField getBaseField() {
    return baseField;
  }

  @Override
  public String toString() {
    return baseField + " " + policy;
  }

  public String getStoredTable() {
    return storedTable;
  }

  public void setStoredTable(String storedTable) {
    this.storedTable = storedTable;
  }

  public String getStoredAttribute() {
    return storedAttribute;
  }

  public ColumnDefinition<?> getColumnDefinition() {
    return constraints;
  }

  public void setStoredAttribute(String storedAttribute) {
    this.storedAttribute = storedAttribute;
  }

  // for testing equality of slice keys
  public void addFilter(LogicalFilter aFilter) {
    if (aFilter != null) filters.add(aFilter);
  }

  public boolean isAliased() {
    return aliased;
  }

  public String getUnaliasedName() {
    return unaliasedName;
  }

  public List<LogicalFilter> getFilters() {
    return filters;
  }

  public boolean isSliceAble() {
    if (storedAttribute != null && storedTable != null && policy == SecurityPolicy.Public)
      return true;
    return false;
  }
}
