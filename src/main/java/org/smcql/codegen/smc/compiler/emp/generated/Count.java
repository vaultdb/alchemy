package org.smcql.codegen.smc.compiler.emp.generated;

import java.nio.ByteBuffer;

import org.bytedeco.javacpp.BoolPointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.ByPtr;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;

//import org.bytedeco.javacpp.annotation.*;
//import org.bytedeco.javacpp.annotation.Properties;

import org.smcql.codegen.smc.compiler.emp.EmpProgram;


/*@Properties(
	    @Platform(
	        includepath = {"/path/to/include/"},
	        preloadpath = {"/path/to/deps/"},
	        linkpath = {"/path/to/lib/"},
	        include = {"NativeLibrary.h"},
	        preload = {"DependentLib"},
	        link = {"NativeLibrary"}),
	    target = "NativeLibrary"
	)*/

@Platform(include={"Count.h"}, 
			includepath= "/usr/local/include", 
			linkpath="/usr/local/lib/", 
			link={"emp-tool","pqxx"},
			compiler = "cpp11")





@Namespace("Count")
public class Count  extends EmpProgram  {


	
	public Count(int aParty, int aPort) {
		super(aParty, aPort);
	}

	public static class CountClass extends Pointer {
        static { Loader.load(); }
        public CountClass() { 	
        	allocate(); 
        	}
        private native void allocate();

        public native @ByPtr BoolPointer run(int party, int port); 
        
        
	}
	
        @Override
        public  boolean[] runProgram() {
        	CountClass counter = new CountClass();
	        BoolPointer outputPtr = counter.run(party, port);
	        ByteBuffer buf = outputPtr.asByteBuffer();
	        // copy bytes out of buffer
	        byte[] bytes = new byte[buf.capacity()];    
	        buf.get(bytes, 0, bytes.length);
	        counter.close();
	        
	        return byteArray2BitArray(bytes);
	        
        }

        
	
    	
}