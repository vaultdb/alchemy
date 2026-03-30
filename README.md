# Alchemy: An Optimizer for Oblivious SQL Queries

## Authors

Donghyun Sohn, Kelly Jiang, Nicolas Hammer, Jennie Rogers

{donghyun.sohn@u.,kellyjiang2022@u., nicolashammer2021@u., jennie@}northwestern.edu

## Abstract

Data sharing opportunities are everywhere, but privacy concerns and regulatory constraints often prevent organizations from fully realizing their value. A private data federation tackles this challenge by enabling secure querying across multiple privately held data stores where only the final results are revealed to anyone. We investigate optimizing relational queries evaluated under secure multiparty computation, which provides strong privacy guarantees but at a significant performance cost.

We present Alchemy, a query optimization framework that generalizes conventional optimization techniques to secure query processing over circuits, the most popular paradigm for cryptographic computation protocols. We build atop VaultDB, our open-source framework for oblivious query processing. Alchemy leverages schema information and the query's structure to minimize circuit complexity while maintaining strong security guarantees. Our optimization framework builds incrementally through four synergistic phases: (1) rewrite rules to minimize circuits; (2) cardinality bounding with schema metadata; (3) bushy plan generation; and (4) physical planning with our fine-grained cost model for operator selection and sort reuse. While our work focuses on MPC, our optimization techniques generalize naturally to other secure computation settings.

We validated our approach on TPC-H, demonstrating speedups of up to 2 OOM.

## Paper

For more details, please refer to our paper:

Alchemy: An Optimizer for Oblivious SQL Queries, VLDB 2025 : [PDF](https://www.vldb.org/pvldb/vol18/p3021-sohn.pdf)

## Dependencies

- PostgreSQL 14+
- Apache Calcite 1.18+
- Apache Maven 3+
- Java 11+
- Python 2.7+
- cmake 3.14+
- libpqxx 7.7.4 - may be installed with setup.sh
- libgflags-dev - 1.13

## Setup

1. Install the dependencies above as needed
2. Configure psql and load TPC-H 1500 databases
3. Install emp toolkits (VOLE-based protocols) and pqxx

**Before setup, you need to download tpch_unioned.sql file.**  
Go to this link and download this file: https://drive.google.com/drive/folders/1ZI6TYcN2aGg-GaAWD9R7xy3SAXtMvP85?usp=sharing  
Then put this file into `./dbs` folder

```bash
./setup.sh
```

## Frontend

In our experiments, we generate our plans for backend based on queries in TPC-H benchmark and plans can be found:

```bash
cd src/main/cpp/conf/plans/
```

This suite demos a parser for extracting a DAG of database operators from a SQL statement. It regularizes the operator order to push down filters and projections. In addition, it eagerly projects out columns as they are no longer needed from the query's intermediate results. This outputs a JSON file for use in the back-end.

### Build

```bash
mvn compile
```

### Parsing a SQL Query to Its Canonicalized Query Tree

**Before running this command, make sure you create a role named 'smcql' with the password 'smcql123'. Otherwise, you will get Postgres connection error.**

To generate a JSON query execution plan, run:

```bash
mvn compile exec:java -Dexec.mainClass="org.vaultdb.ParseSqlToJson" -Dexec.args="<db name> <file with SQL query> <path to write output file>"
```

For example, to prepare a query for the `tpch` database in PostgreSQL with the query stored in `conf/workload/tpch/queries/01.sql` writing the query tree to `conf/workload/tpch/plans/01.json`, run:

```bash
mvn compile exec:java -Dexec.mainClass="org.vaultdb.ParseSqlToJson" -Dexec.args="tpch conf/workload/tpch/queries/01.sql conf/workload/tpch/plans"
```

## Backend

### Build

```bash
cd src/main/cpp
cmake .
```

### Confirm Database in Use

Edit `src/main/cpp/flagfiles/db_names.flags`:

```
--unioned_db=tpch_unioned_1500
--alice_db=tpch_alice_1500
--bob_db=tpch_bob_1500
```

### Make Alchemy Tests

```bash
make -j generalized_comparison_test
```

### Run Tests

Run tests for Alice and Bob concurrently in separate machines:

**Alice machine:**
```bash
bash run-alice.sh ./bin/generalized_comparison_test "alice_ip_address"
```

**Bob machine:**
```bash
bash run-bob.sh ./bin/generalized_comparison_test "alice_ip_address"
```

### Switch Databases

To switch databases in use, modify `src/main/cpp/flagfiles/db_names.flags`:

```
--unioned_db=tpch_unioned_1500 
--alice_db=tpch_alice_1500 
--bob_db=tpch_bob_1500 
```

## Citation

If you use Alchemy in your research, please cite:

```bibtex
@article{sohn2025alchemy,
  title={Alchemy: An Optimizer for Oblivious SQL Queries},
  author={Sohn, Donghyun and Jiang, Kelly and Hammer, Nicolas and Rogers, Jennie},
  journal={Proceedings of the VLDB Endowment},
  volume={18},
  year={2025},
  url={https://www.vldb.org/pvldb/vol18/p3021-sohn.pdf}
}
```
