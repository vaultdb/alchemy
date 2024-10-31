# Alchemy: An Optimizer for Oblivious SQL Queries

--------------------------------------------------------------------------------
Authors
--------------------------------------------------------------------------------

Donghyun Sohn, Kelly Jiang, Nicolas Hammer, Jennie Rogers

{donghyun.sohn@u.,kellyjiang2022@u., nicolashammer2021@u., jennie@}northwestern.edu

--------------------------------------------------------------------------------
Abstract
--------------------------------------------------------------------------------
Data sharing opportunities are everywhere, but until now many of them have been stymied by privacy concerns and regulatory compliance. Data clean rooms provide one solution to this challenge by querying the union of multiple, privately held data stores such that the only information revealed from sensitive data are their query answers. We investigate optimizing relational data clean room queries evaluated under secure multiparty computation. These cryptographic protocols enable the data providers to privately compute over their joint data by passing encrypted messages amongst themselves. These queries run obliviously their program traces and memory access patterns are independent of their private inputs. This strong security guarantee exacts a performance penalty of several orders of magnitude slowdown viz insecure query evaluation. In database queries, this effect is amplified because each operator pads its output cardinality to the worst case to conceal its selectivity, and this blow-up cascades up the query tree.

Fortunately, relational databases have schemas that give us many opportunities to reduce the runtime of these operators from full oblivious without compromising strong security guarantees.   We leverage schema constraints from the federation's schema to generalize conventional query optimization techniques to this privacy-preserving data clean room we call Alchemy.  It uses public information and schema constraints to reduce the complexity of oblivious query processing.  In addition, we introduce VaultDB, our framework for oblivious query processing and provide a fine-grained cost model with which to identify efficient oblivious query execution plans in this system.   We verified our approach on a workload of TPC-H queries and achieved 1-2 OOM faster speeds compared to non-MPC aware conventional optimizer.

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













