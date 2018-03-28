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

import org.smcql.executor.smc.runnable.SMCRunnable;
import org.smcql.privacy.PrivacyCost;

import com.oblivm.backend.circuits.BitonicSortLib;
import com.oblivm.backend.circuits.arithmetic.IntegerLib;
import com.oblivm.backend.flexsc.CompEnv;
import com.oblivm.backend.gc.BadLabelException;
import com.oblivm.backend.gc.GCSignal;
import com.oblivm.backend.lang.inter.Util;
import com.oblivm.backend.util.Utils;


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
	private int sensitivity;
	
	public SecureArray() {
		// needed for serialization
	}
	
	public SecureArray(CompEnv<T> env, int N, int dataSize) throws Exception {
		this.env = env;
		length = N;
		this.dataSize = dataSize;
		useTrivialOram = N <= threshold;
		sensitivity = 1;
		
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
	
	@SuppressWarnings("unchecked")
	public void shrinkToPrivateLength(SMCRunnable parent, double epsilon, double delta, int inputSensitivity, String packageName) throws Exception {
		//extract values from ORAM into a GCSignal[]
		T[][] data = (useTrivialOram) ? trivialOram.content : circuitOram.baseOram.content;
		
		//sort values
		BitonicSortLib<T> lib = new BitonicSortLib<T>(env);
		if (data != null) 
			lib.sort(data, lib.SIGNAL_ZERO);
			
		//calculate differentially private length
		setSensitivity(inputSensitivity * PrivacyCost.getSensitivity(packageName, (int) Utils.toLong(lib.declassifyToBoth(this.getNonNullEntries()))));
		int dpLength = Util.getDifferentiallyPrivateLength(env, parent, this.getNonNullEntries(), epsilon, delta, getSensitivity());
		
		//determine whether to use dp length
		System.out.println(packageName + ", length: " + length + ", dpLength: " + dpLength + ", inputSensitivity: " + inputSensitivity + ", current sensitivity: " + getSensitivity());
		if (length <= 0 || dpLength <= 0) {
			length = 1;
		} else if (length > dpLength && dpLength > 0) {
			length = dpLength;
		}
		
		//allocate and set new ORAM
		if (useTrivialOram) {
			trivialOram = new LinearScanOram<T>(env, length, dataSize);
			trivialOram.content = Arrays.copyOfRange(data, 0, length);
			//for (int i=0; i<length; i++) {
			//	boolean[] val = lib.declassifyToBoth(trivialOram.content[i]);
			//	System.out.println("val: " + Utils.toLong(val));
			//}
		} else {
			circuitOram = new RecursiveCircuitOram<T>(env, length, dataSize);
			circuitOram.baseOram.content = Arrays.copyOfRange(data, 0, length);
		}		
	}
	
	public int getSensitivity() {
		return sensitivity;
	}

	public void setSensitivity(int sensitivity) {
		this.sensitivity = sensitivity;
	}
	
   /*** End SMCQL extensions ***/
	private static SecureArray<?> extracted(Object o) {
		return (SecureArray<?>) o;
	}

}
	

