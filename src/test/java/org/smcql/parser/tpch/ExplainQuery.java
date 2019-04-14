package org.smcql.parser.tpch;

import java.util.Date;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.jdbc.JavaTypeFactoryImpl;
import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.RelCollations;
import org.apache.calcite.rel.RelFieldCollation;
import org.apache.calcite.rel.RelFieldCollation.Direction;
import org.apache.calcite.rel.RelFieldCollation.NullDirection;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeFactory;
import org.apache.calcite.rel.type.RelDataTypeSystem;
import org.apache.calcite.schema.Schema.TableType;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Statistic;
import org.apache.calcite.schema.Statistics;
import org.apache.calcite.schema.StreamableTable;
import org.apache.calcite.schema.Table;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.parser.SqlParseException;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.Frameworks;
import org.apache.calcite.tools.Planner;
import org.apache.calcite.tools.RelConversionException;
import org.apache.calcite.tools.ValidationException;
import org.apache.calcite.util.ImmutableBitSet;
import org.smcql.config.SystemConfiguration;

import com.google.common.collect.ImmutableList;


public class ExplainQuery {
	
	
		  public static final JavaTypeFactory typeFactory = new JavaTypeFactoryImpl(RelDataTypeSystem.DEFAULT);
		  public static SchemaPlus defaultSchema = null;
		 
		  private Planner planner;
		 
		  public ExplainQuery() throws Exception {
			  FrameworkConfig config = SystemConfiguration.getInstance().getCalciteConfiguration();
			  planner  = Frameworks.getPlanner(config);
			  addTableSchema();
		  }
		 
		  public void addTableSchema() throws Exception{
			 defaultSchema = SystemConfiguration.getInstance().getPdnSchema();
		  }
		 

		  public Statistic getStatistic() {
		    return Statistics.of(100, ImmutableList.of());
		  }
		 
		  public TableType getJdbcTableType() {
		    return TableType.TABLE;
		  }
		 
		 
		  public RelRoot explain(String sql) throws SqlParseException, ValidationException, RelConversionException{
			 
			System.out.println("Initial query: " + sql);
			SqlNode parse = planner.parse(sql);
		    System.out.println("Parsed SqlNode: " + parse.toString());
		 
		    SqlNode validate = planner.validate(parse);
		    RelRoot tree = planner.rel(validate);
		 
		    String plan = RelOptUtil.toString(tree.rel); //explain(tree, SqlExplainLevel.ALL_ATTRIBUTES);
		    System.out.println("Plan: ");
		    System.out.println(plan);
		    return tree;
		 
		  }
}
