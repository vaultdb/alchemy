#!/bin/bash -x

#must be run in emp-jni directory
export JAVACPP_JAR=$HOME/.m2/repository/org/bytedeco/javacpp/1.4.4/javacpp-1.4.4.jar
export EMP_JNI_ROOT=$PWD
export JAVACPP_WORKING_DIR=$EMP_JNI_ROOT/src/main/java

ulimit -c unlimited
cd src/main/java
java -cp $JAVACPP_JAR:$JAVACPP_WORKING_DIR  org.smcql.compiler.emp.generated.CountIcd9sLocal 2 54321

