import json
import psycopg2

CARD = None
with open("conf/datagen/healthlnk.json") as f:
  for line in f:
    if '"cardinality"' in line:
      CARD = int("".join(filter(str.isdigit, line)))
      break

assert CARD is not None, "Cardinality not found"
DB = f"healthlnk_alice_{CARD}"
JSON_PATH = f"./conf/plans/conclave_comparison/alchemy_{CARD}.json"

conn = psycopg2.connect(f"dbname={DB}")
cursor = conn.cursor()
cursor.execute("SELECT patient_id FROM pids_in_mpc ORDER BY patient_id;")
PIDS = [row[0] for row in cursor.fetchall()]

print(f"✅ Found {len(PIDS)} PIDs:")
print(PIDS)

id_counter = 1  # Start from 1 now, because 0 will be MultipleUnion
rels = []

# Construct MultipleUnion as operator 0
rels.append({
  "id": "0",
  "relOp": "MultipleUnion",
  "outputFields": "m.patient_id (#0), m.timestamp_ (#1), d.patient_id (#2), d.timestamp_ (#3)",
  "sql1": "SELECT patient_id, timestamp_, NOT (TRIM(medication) = 'aspirin') AS dummy_tag FROM medications WHERE patient_id = ",
  "sql2": "SELECT patient_id, timestamp_, NOT (TRIM(icd9) = 'hd') AS dummy_tag FROM diagnoses WHERE patient_id = ",
  "filter": PIDS,
  "collation": [{"field": 0, "direction": "ASCENDING"}]
})

# rest of the pipeline
filter_id = str(id_counter); id_counter += 1
rels.append({
  "id": filter_id,
  "relOp": "LogicalFilter",
  "condition": {
    "op": {"name": ">=", "kind": "GREATER_THAN_OR_EQUAL", "syntax": "BINARY"},
    "operands": [
      {"input": 1, "name": "$1"},
      {"input": 3, "name": "$3"}
    ]
  }
})

agg1_id = str(id_counter); id_counter += 1
rels.append({
  "id": agg1_id,
  "relOp": "LogicalAggregate",
  "operator-algorithm": "auto",
  "cardBound": "pids_in_mpc",
  "effective-collation": [{"field": 0, "direction": "ASCENDING", "nulls": "FIRST"}],
  "outputFields": "(#0: patient_id, #1: order_count BIGINT)",
  "group": [0],
  "aggs": [{
    "agg": {"name": "COUNT", "kind": "COUNT", "syntax": "FUNCTION"},
    "type": {"type": "LONG", "nullable": False},
    "distinct": True,
    "operands": [2],
    "name": "count"
  }]
})

agg2_id = str(id_counter); id_counter += 1
rels.append({
  "id": agg2_id,
  "relOp": "LogicalAggregate",
  "inputFields": "(#0: patient_id, #1: order_count BIGINT)",
  "outputFields": "(#0: cnt)",
  "aggs": [{
    "agg": {"name": "COUNT", "kind": "COUNT", "syntax": "FUNCTION_STAR"},
    "type": {"type": "INTEGER", "nullable": False},
    "distinct": False,
    "operands": [0],
    "name": "cnt"
  }]
})

sql_str = "SELECT COUNT(DISTINCT d.patient_id)::INT as count FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE d.patient_id NOT IN (SELECT patient_id FROM pids_in_mpc) AND TRIM(d.icd9) = 'hd' AND TRIM(m.medication) = 'aspirin' AND d.timestamp_ <= m.timestamp_"

rels.append({
  "id": str(id_counter),
  "relOp": "LogicalValues",
  "outputFields": "count",
  "sql": sql_str,
  "type": [{"type": "INTEGER", "nullable": False, "name": "count"}],
  "party": 1
}); id_counter += 1

rels.append({
  "id": str(id_counter),
  "relOp": "LogicalJoin",
  "outputFields": "(#0: mpc_count, #1: alice_count)",
  "joinType": "cartesian",
  "inputs": [str(int(agg2_id)), str(id_counter - 1)]
}); id_counter += 1

rels.append({
  "id": str(id_counter),
  "relOp": "LogicalValues",
  "outputFields": "patient_id (#0), medication (#1), timestamp_ (#2)",
  "sql": sql_str,
  "type": [{"type": "INTEGER", "nullable": False, "name": "count"}],
  "party": 2
}); id_counter += 1

rels.append({
  "id": str(id_counter),
  "relOp": "LogicalJoin",
  "outputFields": "(#0: mpc_count, #1: alice_count)",
  "joinType": "cartesian",
  "inputs": [str(id_counter - 2), str(id_counter - 1)]
}); id_counter += 1

rels.append({
  "id": str(id_counter),
  "relOp": "LogicalProject",
  "fields": ["count"],
  "exprs": [{
    "op": {"name": "+", "kind": "PLUS", "syntax": "BINARY"},
    "operands": [
      {
        "op": {"name": "+", "kind": "PLUS", "syntax": "BINARY"},
        "operands": [
          {"input": 1, "name": "$1"},
          {"input": 2, "name": "$2"}
        ]
      },
      {"input": 0, "name": "$0"}
    ]
  }]
}); id_counter += 1

with open(JSON_PATH, 'w') as f:
  json.dump({"rels": rels}, f, indent=2)

print(f"\n✔ Alchemy plan written to {JSON_PATH} with {id_counter} total operators.")