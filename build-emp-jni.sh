#!/bin/bash -x
#usage: ./build-emp-jni.sh <class name>
#example: ./build-emp-jni.sh CountIcd9s

#must be run in emp-jni dir
export CLASS_NAME=$1
export JAVACPP_JAR=$HOME/.m2/repository/org/bytedeco/javacpp/1.4.4/javacpp-1.4.4.jar
export EMP_JNI_ROOT=$PWD
export JAVACPP_SRC='org/smcql/compiler/emp/generated/'$CLASS_NAME'.java'
export JAVACPP_WORKING_DIRECTORY=$EMP_JNI_ROOT/src/main/java

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    export JAVACPP_PROPERTIES=$EMP_JNI_ROOT/src/main/resources/org/bytedeco/javacpp/properties/linux-x86_64-emp.properties
elif [[ "$OSTYPE" == "darwin"* ]]; then
    export JAVACPP_PROPERTIES=$EMP_JNI_ROOT/src/main/resources/org/bytedeco/javacpp/properties/macosx-x86_64-emp.properties
fi

ulimit -c unlimited
cd $JAVACPP_WORKING_DIRECTORY

java -jar $JAVACPP_JAR $JAVACPP_SRC -propertyfile $JAVACPP_PROPERTIES



