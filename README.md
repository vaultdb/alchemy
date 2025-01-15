# Alchemy: An Optimizer for Oblivious SQL Queries

--------------------------------------------------------------------------------
Authors
--------------------------------------------------------------------------------

Donghyun Sohn, Kelly Jiang, Nicolas Hammer, Jennie Rogers

{donghyun.sohn@u.,kellyjiang2022@u., nicolashammer2021@u., jennie@}northwestern.edu

--------------------------------------------------------------------------------
Abstract
--------------------------------------------------------------------------------
 Data sharing opportunities are everywhere, but privacy concerns and regulatory constraints often prevent organizations from fully realizing their value. A private data federation tackles this challenge by enabling secure querying across multiple privately held data stores where only the final results are revealed to anyone. We investigate optimizing relational queries evaluated under secure multiparty computation, which provides strong privacy guarantees but at a significant performance cost.

We present Alchemy, a query optimization framework that generalizes conventional optimization techniques to secure query processing over circuits, the most popular paradigm for cryptographic computation protocols. We build atop VaultDB, our open-source framework for oblivious query processing. Alchemy leverages schema information and the query's structure to minimize circuit complexity while maintaining strong security guarantees. Our optimization framework builds incrementally through four synergistic phases: (1)  rewrite rules to minimize circuits; (2) cardinality bounding with schema metadata; (3) bushy plan generation; and (4) physical planning with our fine-grained cost model for operator selection and sort reuse.   While our work focuses on MPC, our optimization techniques generalize naturally to other secure computation settings. We validated our approach on TPC-H, demonstrating speedups of up to 2 OOM.

--------------------------------------------------------------------------------
Dependencies
--------------------------------------------------------------------------------
* PostgreSQL 14+
* Apache Calcite 1.18+
* Apache Maven 3+
* Java 11+
* Python 2.7+
* cmake 3.14+
* libpqxx 7.7.4 - may be installed with setup.sh
* libgflags-dev - 1.13


--------------------------------------------------------------------------------
Setup
--------------------------------------------------------------------------------

Install the dependencies above as needed

Configure psql and load TPC-H 1500 databases

Install emp toolkits (VOLE-based protocols) and pqxx

Before setup, you need to download tpch_unioned.sql file.
https://drive.google.com/drive/folders/1ZI6TYcN2aGg-GaAWD9R7xy3SAXtMvP85?usp=sharing
Go to this link and download this file. And then put this file in to ./dbs folder

```
./setup.sh
```
--------------------------------------------------------------------------------
Frontend
--------------------------------------------------------------------------------

In our experiments, we generate our plans for backend based on queries in TPC-H benchmark and plans can be found:

```
cd src/main/cpp/conf/plans/
```

This suite demos a parser for extracting a DAG of database operators from a SQL statement.   It regularizes the operator order to push down filters and projections.  In addition, it eagerly projects out columns as they are no longer needed from the query's intermediate results.  This outputs a JSON file for use in the back-end.


Build this with:

```
mvn compile
```
## Parsing a SQL Query to Its Canonicalized Query Tree

Before running this command, make sure you create a role named 'smcql' with the password 'smcql123'. 
Otherwise, you will get Postgres connection error. 

To generate a JSON query execution plan, run:
```
mvn compile exec:java -Dexec.mainClass="org.vaultdb.ParseSqlToJson" -Dexec.args="<db name> <file with SQL query>  <path to write output file>"
```

For example, to prepare a query for the `tpch` database in PostgreSQL with the query stored in `conf/workload/tpch/queries/01.sql` writing the query tree to `conf/workload/tpch/plans/01.json`, run:

```
mvn compile exec:java -Dexec.mainClass="org.vaultdb.ParseSqlToJson" -Dexec.args="tpch   conf/workload/tpch/queries/01.sql  conf/workload/tpch/plans"
```

--------------------------------------------------------------------------------
Backend
--------------------------------------------------------------------------------
Build:
```
cd src/main/cpp
cmake .
```
Confirm database in use :
```
# vi src/main/cpp/flagfiles/db_names.flags
--unioned_db=tpch_unioned_1500
--alice_db=tpch_alice_1500
--bob_db=tpch_bob_1500
```
Make Alchemy tests:
```
make -j alchemy_test
```
Run tests for Alice and Bob concurrently in separate machines:
```
# Alice machine
bash run-alice.sh ./bin/alchemy_test "alice_ip_address"
# Bob machine
bash run-bob.sh ./bin/alchemy_test "alice_ip_address"
```
To switch databases in use, please modify:
```
# vi src/main/cpp/flagfiles/db_names.flags and change unioned_db, alice_db, bob_db name as follow
--unioned_db=tpch_unioned_1500 
--alice_db=tpch_alice_1500 
--bob_db=tpch_bob_1500 
```













