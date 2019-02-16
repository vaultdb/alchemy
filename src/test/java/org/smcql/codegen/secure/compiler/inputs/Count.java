package org.smcql.codegen.secure.compiler.inputs;

import java.nio.ByteBuffer;

import org.bytedeco.javacpp.BoolPointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.ByPtrPtr;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;
import org.smcql.codegen.smc.EmpProgram;
import org.smcql.util.Utilities;

@Platform(include="Count.h")
@Namespace("Count")
public class Count extends EmpProgram {

	public Count(int party, int port) {
		super(party, port);
	}

	public static class CountClass extends Pointer {
        static { Loader.load(); }
        public CountClass() { 	
        	allocate(); 
        	}
        private native void allocate();

        public native @ByPtrPtr BoolPointer run(int party, int port); 

        
    }

	@Override
    public boolean[] runProgram() {
        CountClass counter = new CountClass();
        BoolPointer outputPtr = counter.run(party, port);
        ByteBuffer buf = outputPtr.asByteBuffer();
        // copy bytes out of buffer
        byte[] bytes = new byte[buf.capacity()];    
        buf.get(bytes, 0, bytes.length);
        counter.close();
        
        return Utilities.byteArray2BitArray(bytes);
    }
}