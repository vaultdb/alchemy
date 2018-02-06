// Copyright (C) 2014 by Xiao Shaun Wang <wangxiao@cs.umd.edu>
package com.oblivm.backend.oram;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;

import org.smcql.codegen.smc.DynamicCompiler;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.Utilities;

import com.oblivm.backend.flexsc.CompEnv;
import com.oblivm.backend.gc.BadLabelException;
import com.oblivm.backend.lang.inter.ISecureRunnable;

public class SecureArray<T> implements java.io.Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 5885430036769465598L;
	static final int threshold = 256;
	boolean useTrivialOram = false;
	public LinearScanOram<T> trivialOram = null;
	public RecursiveCircuitOram<T> circuitOram = null;
	public int lengthOfIden;
	public int length;
	public int dataSize;
	protected T[] nonNullEntries; // number of entries in array that are initialized
	private CompEnv<T> env;
	
	public SecureArray() {
		// needed for serialization
	}
	
	public SecureArray(CompEnv<T> env, int N, int dataSize) throws Exception {
		this.env = env;
		length = N;
		this.dataSize = dataSize;
		useTrivialOram = N <= threshold;
		if (useTrivialOram) {
			trivialOram = new LinearScanOram<T>(env, N, dataSize);
			lengthOfIden = trivialOram.lengthOfIden;
		} else {
			circuitOram = new RecursiveCircuitOram<T>(env, N, dataSize);
			lengthOfIden = circuitOram.lengthOfIden;
		}
	}

	public T[] readAndRemove(T[] iden) throws BadLabelException {
		return circuitOram.clients.get(0).readAndRemove(iden, 
				Arrays.copyOfRange(circuitOram.clients.get(0).lib.declassifyToBoth(iden), 0, circuitOram.clients.get(0).lengthOfPos), false);
	}

	public T[] read(T[] iden) throws BadLabelException {
		if (useTrivialOram)
			return trivialOram.read(iden);
		else
			return circuitOram.read(iden);
	}

	public void write(T[] iden, T[] data) throws Exception {
		if (useTrivialOram)
			trivialOram.write(iden, data);
		else
			circuitOram.write(iden, data);
	}
	
	public void conditionalWrite(T[] iden, T[]data, T condition) throws BadLabelException {
		if(useTrivialOram) {
		T[] readData = trivialOram.readAndRemove(iden);
		T[] toAdd = trivialOram.lib.mux(readData, data, condition);
		trivialOram.putBack(iden, toAdd);
		}
		else {
			//op == 1 means write, 0 means read
			circuitOram.access(iden, data, condition);
		}
	}
	/*** SMCQL Extensions ***/
	
	public void setNonNullEntries(T[] entries) {
		nonNullEntries = entries;
	}
	
	public T[] getNonNullEntries() {
		return nonNullEntries;
	}
	
	
	public String serializeToString() throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ObjectOutputStream oos = new ObjectOutputStream( baos );
        oos.writeObject(this);
        oos.close();
        
        return Base64.getEncoder().encodeToString(baos.toByteArray()); 
    }
	
	public static SecureArray<?> deserializeFromString(String s) throws IOException, ClassNotFoundException {
		try {
			byte [] data = Base64.getDecoder().decode(s);
			ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(data) );
			Object o  = ois.readObject();
			ois.close();
			if (o instanceof SecureArray)
				return extracted(o);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			System.out.println("RecursiveCircuitOram<T> not found." + e.getMessage());
			e.printStackTrace();
		}
		return null;
	}
	
	public static void serialize(SecureArray<?> a, String dstFilename) {
		a.serializeToDisk(dstFilename);
	}
	
	public static void deserialize(SecureArray<?> a, String srcFilename) {
		a = deserializeFromDisk(srcFilename);
	}
	
	
	public void serializeToDisk(String dstFilename) {
		 try
	      {
	         FileOutputStream fileOut =
	         new FileOutputStream(dstFilename);
	         ObjectOutputStream out = new ObjectOutputStream(fileOut);
	         out.writeObject(this);
	         out.close();
	         fileOut.close();
	      }catch(IOException i)
	      {
	          i.printStackTrace();
	      }
	}

	public static SecureArray<?> deserializeFromDisk(String srcFilename) {
		 try
	      {
	         FileInputStream fileIn = new FileInputStream(srcFilename);
	         ObjectInputStream in = new ObjectInputStream(fileIn);
	         Object o = in.readObject();
	         in.close();
	         fileIn.close();

	         if(o instanceof SecureArray) {
	        	 return extracted(o);
	         }
	         
	      }catch(IOException i)
	      {
	         i.printStackTrace();
	         return null;
	      }catch(ClassNotFoundException c)
	      {
	         System.out.println("RecursiveCircuitOram<T> not found.");
	         c.printStackTrace();
	         return null;
	      }
		 return null;
		 
	}
	
	public int getTrueLength(CompEnv<T> env) throws Exception {
		String packageName = "org.smcql.generated.utilities.dpSize";
		Map<String, String> variables = new HashMap<String, String>();
		variables.put("size", Integer.toString(dataSize));
		variables.put("packageName", packageName);
		
		String generatedCode = CodeGenUtils.generateFromTemplate("dp_size.txt", variables);
		DynamicCompiler.compileOblivLang(generatedCode, packageName);
		byte[] byteCode = Utilities.readGeneratedClassFile(packageName);
		ISecureRunnable<T> runnable = DynamicCompiler.loadClass(packageName, byteCode, env);
		
		runnable.run(this, null);
		return 0;
	}
	
	public void shrinkToPrivateLength(double epsilon, double delta, int sensitivity) throws Exception {
		// get the length of the current array (will be d-private later)
		//int trueLen = getTrueLength();
		
		// shrink the ORAM (will be done efficiently later)
		
		/*
		int numDummies = Utilities.generateDiscreteLaplaceNoise(epsilon, delta, sensitivity); //this is the noise, not the full length
		
		if (useTrivialOram) {
			trivialOram = new LinearScanOram<T>(env, length, dataSize);
			lengthOfIden = trivialOram.lengthOfIden;
		} else {
			circuitOram = new RecursiveCircuitOram<T>(env, length, dataSize);
			lengthOfIden = circuitOram.lengthOfIden;
		}*/
	}
	
   /*** End SMCQL extensions ***/
	private static SecureArray<?> extracted(Object o) {
		return (SecureArray<?>) o;
	}

}
	

