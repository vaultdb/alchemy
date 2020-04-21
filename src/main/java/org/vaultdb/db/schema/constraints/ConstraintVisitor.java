package org.vaultdb.db.schema.constraints;

import java.sql.Timestamp;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexCorrelVariable;
import org.apache.calcite.rex.RexDynamicParam;
import org.apache.calcite.rex.RexFieldAccess;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;
import org.apache.calcite.rex.RexLocalRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexOver;
import org.apache.calcite.rex.RexPatternFieldRef;
import org.apache.calcite.rex.RexRangeRef;
import org.apache.calcite.rex.RexSubQuery;
import org.apache.calcite.rex.RexTableInputRef;
import org.apache.calcite.rex.RexVisitor;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Table;
import org.apache.calcite.sql.SqlOperator;
import org.apache.calcite.sql.type.SqlTypeName;
import org.jetbrains.annotations.NotNull;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.apache.calcite.avatica.util.DateTimeUtils;
import org.apache.calcite.avatica.util.TimeUnit;
import org.apache.calcite.avatica.util.TimeUnitRange;
import org.apache.calcite.rel.metadata.NullSentinel;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.util.NlsString;
import org.apache.calcite.util.Util;

import com.google.common.collect.ImmutableMap;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.function.IntPredicate;


// adds constraints to SystemCatalog
// TODO: for now constraints only cover a single attribute at a time, extend this
// loosely based on RexInterpreter
public class ConstraintVisitor   implements RexVisitor<Comparable> {

/**
 * Evaluates {@link RexNode} expressions.
 *
 * <p>Caveats:
 * <ul>
 *   <li>It uses interpretation, so it is not very efficient.
 *   <li>It is intended for testing, so does not cover very many functions and
 *   operators. (Feel free to contribute more!)
 *   <li>It is not well tested.
 * </ul>
 */
	public static final String DATE_FORMAT = "yyyy-MM-dd";
	public static DateFormat dateFormatter = new SimpleDateFormat(DATE_FORMAT);
	private static final NullSentinel N = NullSentinel.INSTANCE;

	public static final EnumSet<SqlKind> SUPPORTED_SQL_KIND =
			EnumSet.of(SqlKind.IS_NOT_DISTINCT_FROM, SqlKind.EQUALS, SqlKind.IS_DISTINCT_FROM,
					SqlKind.NOT_EQUALS, SqlKind.GREATER_THAN, SqlKind.GREATER_THAN_OR_EQUAL,
					SqlKind.LESS_THAN, SqlKind.LESS_THAN_OR_EQUAL, SqlKind.AND, SqlKind.OR,
					SqlKind.NOT, SqlKind.CASE, SqlKind.IS_TRUE, SqlKind.IS_NOT_TRUE,
					SqlKind.IS_FALSE, SqlKind.IS_NOT_FALSE, SqlKind.PLUS_PREFIX,
					SqlKind.MINUS_PREFIX, SqlKind.PLUS, SqlKind.MINUS, SqlKind.TIMES,
					SqlKind.DIVIDE, SqlKind.COALESCE, SqlKind.CEIL,
					SqlKind.FLOOR, SqlKind.EXTRACT);

	private ColumnDefinition columnDefinition;
	private SecureRelRecordType schema;
	// for handling multiple columns
	private Map <RexInputRef, ColumnDefinition> columnDefinitionMap;

	public ConstraintVisitor(ColumnDefinition columnDefinition){
		this.columnDefinition = columnDefinition;
	}

	public ConstraintVisitor(SecureRelRecordType schema){
		columnDefinitionMap = new HashMap<>();
		this.schema = schema;
	}

	public ColumnDefinition getColumnDefinition() {
		return columnDefinition;
	}

	public Map<RexInputRef, ColumnDefinition> getColumnDefinitionMap() {
		return columnDefinitionMap;
	}

	private IllegalArgumentException unbound(RexNode e) {
		return new IllegalArgumentException("unbound: " + e);
	}

	private Comparable getOrUnbound(RexNode e) {
		return null;
	}

	public Comparable visitInputRef(RexInputRef inputRef) {
		System.out.println("visitInputRef: " + inputRef.toString());
		if (columnDefinitionMap != null){
			try {
				if (!columnDefinitionMap.containsKey(inputRef)){
					int columnIndex = Integer.valueOf(inputRef.getIndex());
					SecureRelDataTypeField secureRelDataTypeField = schema.getSecureField(columnIndex);
					ColumnDefinition columnDefinition = ColumnDefinitionFactory.get(secureRelDataTypeField);
					columnDefinitionMap.put(inputRef, columnDefinition);
				}
			} catch (Exception e) {
				System.out.println("visit input ref unable to create new column definition");
			}
		}
		return null;
	}

	public Comparable visitLocalRef(RexLocalRef localRef) {
		throw unbound(localRef);
	}

	public Comparable visitLiteral(RexLiteral literal) {
		System.out.println("visitLiteral: " + literal.getValue());
		switch (literal.getType().getSqlTypeName()){
			case VARCHAR:
			case CHAR:
				return ((NlsString) literal.getValue()).getValue();
			case INTEGER:
				if (literal.getValue() instanceof BigDecimal){
					return ((BigDecimal) literal.getValue()).intValue();
				}
			case DECIMAL:
				if (literal.getValue() instanceof BigDecimal){
					return ((BigDecimal) literal.getValue()).doubleValue();
				}
			default:
				return Util.first(literal.getValue4(), N);
		}
	}

	public Comparable visitOver(RexOver over) {
		throw unbound(over);
	}

	public Comparable visitCorrelVariable(RexCorrelVariable correlVariable) {
		return getOrUnbound(correlVariable);
	}

	public Comparable visitDynamicParam(RexDynamicParam dynamicParam) {
		return getOrUnbound(dynamicParam);
	}

	public Comparable visitRangeRef(RexRangeRef rangeRef) {
		throw unbound(rangeRef);
	}

	public Comparable visitFieldAccess(RexFieldAccess fieldAccess) {
		return getOrUnbound(fieldAccess);
	}

	public Comparable visitSubQuery(RexSubQuery subQuery) {
		throw unbound(subQuery);
	}

	public Comparable visitTableInputRef(RexTableInputRef fieldRef) {
		throw unbound(fieldRef);
	}

	public Comparable visitPatternFieldRef(RexPatternFieldRef fieldRef) {
		throw unbound(fieldRef);
	}

	public Comparable visitCall(RexCall call) {
		final List<Comparable> values = new ArrayList<>(call.operands.size());
		for (RexNode operand : call.operands) {
			values.add(operand.accept(this));
		}
		// TODO: properly handle OR case
		// TODO: set constraint string length based on the total number of characters (i.e. CHAR(7))
		switch (call.getKind()) {
			case IS_NOT_DISTINCT_FROM:
			case EQUALS:
				System.out.println(values);
				Comparable rexLiteralEquals;
				if (call.operands.get(0) instanceof RexInputRef){
					rexLiteralEquals = values.get(1);
				} else {
					rexLiteralEquals = values.get(0);
				}
				if (columnDefinitionMap != null){
					columnDefinitionMap.get(call.operands.get(0)).appendToDomain(rexLiteralEquals);
				} else {
					columnDefinition.appendToDomain(rexLiteralEquals);
				}
				return null;
			case IS_DISTINCT_FROM:
			case NOT_EQUALS:
				return null;
			case GREATER_THAN:
			case GREATER_THAN_OR_EQUAL:
				System.out.println("greater than or equal to");
				if (call.operands.get(0) instanceof RexInputRef){
					// $0  >= literal
					if (columnDefinitionMap != null){
						columnDefinitionMap.get(call.operands.get(0)).setMin(values.get(1));
					} else {
						columnDefinition.setMin(values.get(1));
					}
				} else {
					// literal >= $0
					if (columnDefinitionMap != null){
						columnDefinitionMap.get(call.operands.get(0)).setMax(values.get(0));
					} else {
						columnDefinition.setMax(values.get(0));
					}
				}
				return null;
			case LESS_THAN:
			case LESS_THAN_OR_EQUAL:
				System.out.println("less than or equal to");
				// figure out whether col def is on left or rhs
				if (call.operands.get(0) instanceof RexInputRef){
					// $0  <= literal
					if (columnDefinitionMap != null){
						columnDefinitionMap.get(call.operands.get(0)).setMax(values.get(1));
					} else {
						columnDefinition.setMax(values.get(1));
					}
				} else {
					// literal <= $0
					if (columnDefinitionMap != null){
						columnDefinitionMap.get(call.operands.get(0)).setMin(values.get(0));
					} else {
						columnDefinition.setMin(values.get(0));
					}
				}
				return null;
			case AND:
				System.out.println("AND operator");
				return null;
			case OR:
				System.out.println("OR operator");
				return null;
			case NOT:
				System.out.println("NOT operator");
				return null;
			case CASE:
			case IS_TRUE:
			case IS_NOT_TRUE:
			case IS_NULL:
				System.out.println("IS_NULL operator");
				return null;
			case IS_NOT_NULL:
				System.out.println("IS_NOT_NOT operator");
				return null;
			case IS_FALSE:
			case IS_NOT_FALSE:
			case PLUS_PREFIX:
			case MINUS_PREFIX: ;
			case PLUS:
			case MINUS:
			case TIMES:
			case DIVIDE:
			case CAST:
				return cast(call, values);
			case COALESCE:
				System.out.println("COALESCE operator");
				return null;
//				return coalesce(call, values);
			case CEIL:
			case FLOOR:
				System.out.println("CEIL operator");
				return null;
//				return ceil(call, values);
			case EXTRACT:
				System.out.println("EXTRACT operator");
				return null;
//				return extract(call, values);
			default:
				return null;
		}
	}

	private Comparable extract(RexCall call, List<Comparable> values) {
		final Comparable v = values.get(1);
		if (v == N) {
			return N;
		}
		final TimeUnitRange timeUnitRange = (TimeUnitRange) values.get(0);
		final int v2;
		if (v instanceof Long) {
			// TIMESTAMP
			v2 = (int) (((Long) v) / TimeUnit.DAY.multiplier.longValue());
		} else {
			// DATE
			v2 = (Integer) v;
		}
		return DateTimeUtils.unixDateExtract(timeUnitRange, v2);
	}

	private Comparable coalesce(RexCall call, List<Comparable> values) {
		for (Comparable value : values) {
			if (value != N) {
				return value;
			}
		}
		return N;
	}

	private Comparable ceil(RexCall call, List<Comparable> values) {
		if (values.get(0) == N) {
			return N;
		}
		final Long v = (Long) values.get(0);
		final TimeUnitRange unit = (TimeUnitRange) values.get(1);
		switch (unit) {
			case YEAR:
			case MONTH:
				switch (call.getKind()) {
					case FLOOR:
						return DateTimeUtils.unixTimestampFloor(unit, v);
					default:
						return DateTimeUtils.unixTimestampCeil(unit, v);
				}
		}
		final TimeUnitRange subUnit = subUnit(unit);
		for (long v2 = v;;) {
			final int e = DateTimeUtils.unixTimestampExtract(subUnit, v2);
			if (e == 0) {
				return v2;
			}
			v2 -= unit.startUnit.multiplier.longValue();
		}
	}

	private TimeUnitRange subUnit(TimeUnitRange unit) {
		switch (unit) {
			case QUARTER:
				return TimeUnitRange.MONTH;
			default:
				return TimeUnitRange.DAY;
		}
	}
	// only supports casting to decimal
	private Comparable cast(RexCall call, List<Comparable> values) {
		System.out.println("CAST operator");

		String stringValue;
		if (values.get(0) instanceof NlsString){
			stringValue = ((NlsString) values.get(0)).getValue();
		} else if (values.get(0) instanceof String){
			stringValue = (String) values.get(0);
		} else {
			return values.get(0);
		}

		switch (call.getType().getSqlTypeName().getName()){
			case "DECIMAL":
				// TODO: consider scale and precision
				return Double.valueOf(stringValue);
			case "TIMESTAMP":
			case "DATE":
				return parseTimestampFromDateString(stringValue);
			default:
				return values.get(0);
		}
	}

	public static Comparable parseTimestampFromDateString(String stringValue) {
		try {
			Date d = dateFormatter.parse(stringValue);
			return new Timestamp(d.getTime());
		} catch (ParseException e){
			System.out.println(e);
			return stringValue;
		}
	}

	private Comparable not(Comparable value) {
		if (value.equals(true)) {
			return false;
		} else if (value.equals(false)) {
			return true;
		} else {
			return N;
		}
	}

	private Comparable case_(List<Comparable> values) {
		final int size;
		final Comparable elseValue;
		if (values.size() % 2 == 0) {
			size = values.size();
			elseValue = N;
		} else {
			size = values.size() - 1;
			elseValue = Util.last(values);
		}
		for (int i = 0; i < size; i += 2) {
			if (values.get(i).equals(true)) {
				return values.get(i + 1);
			}
		}
		return elseValue;
	}

	private BigDecimal number(Comparable comparable) {
		return comparable instanceof BigDecimal
				? (BigDecimal) comparable
				: comparable instanceof BigInteger
				? new BigDecimal((BigInteger) comparable)
				: comparable instanceof Long
				|| comparable instanceof Integer
				|| comparable instanceof Short
				? new BigDecimal(((Number) comparable).longValue())
				: new BigDecimal(((Number) comparable).doubleValue());
	}

	private Comparable compare(List<Comparable> values, IntPredicate p) {
		if (containsNull(values)) {
			return N;
		}
		Comparable v0 = values.get(0);
		Comparable v1 = values.get(1);

		if (v0 instanceof Number && v1 instanceof NlsString) {
			try {
				v1 = new BigDecimal(((NlsString) v1).getValue());
			} catch (NumberFormatException e) {
				return false;
			}
		}
		if (v1 instanceof Number && v0 instanceof NlsString) {
			try {
				v0 = new BigDecimal(((NlsString) v0).getValue());
			} catch (NumberFormatException e) {
				return false;
			}
		}
		if (v0 instanceof Number) {
			v0 = number(v0);
		}
		if (v1 instanceof Number) {
			v1 = number(v1);
		}
		//noinspection unchecked
		final int c = v0.compareTo(v1);
		return p.test(c);
	}

	private boolean containsNull(List<Comparable> values) {
		for (Comparable value : values) {
			if (value == N) {
				return true;
			}
		}
		return false;
	}

	/** An enum that wraps boolean and unknown values and makes them
	 * comparable. */
	enum Truthy {
		// Order is important; AND returns the min, OR returns the max
		FALSE, UNKNOWN, TRUE;

		static Truthy of(Comparable c) {
			return c.equals(true) ? TRUE : c.equals(false) ? FALSE : UNKNOWN;
		}

		Comparable toComparable() {
			switch (this) {
				case TRUE: return true;
				case FALSE: return false;
				case UNKNOWN: return N;
				default:
					throw new AssertionError();
			}
		}
	}
}