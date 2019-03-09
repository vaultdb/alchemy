package org.smcql.util;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.PumpStreamHandler;
import org.bytedeco.javacpp.Loader;

public class EmpJniUtilities {
	
    public static String getPropertyFile()  {
	String propertyFile = Utilities.getSMCQLRoot() + "/src/main/resources/org/bytedeco/javacpp/properties/";
	propertyFile += Loader.getPlatform();
	propertyFile += "-emp.properties";
	 
	return propertyFile;
    }

	// output handling
	public static List<String> revealStringOutput(boolean[] alice, boolean[] bob, int tupleWidth) {
		assert(alice.length == bob.length);
		boolean[] decrypted = new boolean[alice.length];
		int tupleBits = tupleWidth*8; // 8 bits / char
		int tupleCount = alice.length / tupleBits;
		List<String> output = new ArrayList<String>();
		
		System.out.print("Decrypted: ");
		for(int i = 0; i < alice.length; ++i) {
			decrypted[i] = alice[i] ^ bob[i];
			System.out.print(decrypted[i] == true ? '1' : '0');
		}

		System.out.println("\n");
		
		int readIdx = 0;
		for(int i = 0; i < tupleCount; ++i) {
			boolean[] bits = Arrays.copyOfRange(decrypted, readIdx*8, (readIdx+tupleWidth)*8);
			String tuple = deserializeString(bits);
			output.add(tuple);
			readIdx += tupleWidth;
		}
		
		return output;		
	}
	
	public static String deserializeString(boolean[] src) {
		assert(src.length % 8 == 0);
		int chars = src.length / 8;
		String value = new String();
		
		for(int i = 0; i < chars; ++i)
		{
			boolean[] bits = Arrays.copyOfRange(src, i*8, (i+1)*8);
			value += deserializeChar(bits);
			
		}
		
		return value;
		
		
	}

	
	
	public static char deserializeChar(boolean[] bits) {
		assert(bits.length == 8);

	    int n = 0;
	    for (boolean b : bits)
	        n = (n << 1) | (b ? 1 : 0);
	    return (char) n;
	}



	 
}
