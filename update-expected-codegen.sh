for testName in CountIcd9sCodegen FilterDistinctCodegen JoinCdiffCodegen
do
     cp src/main/java/org/vaultdb/compiler/emp/generated/$testName.h  src/test/java/org/vaultdb/codegen/secure/expected/$testName.h
     cp src/main/java/org/vaultdb/compiler/emp/generated/$testName.java  src/test/java/org/vaultdb/codegen/secure/expected/$testName.java_
done