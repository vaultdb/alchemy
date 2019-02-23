package org.smcql.compiler;

import java.io.IOException;
import org.smcql.util.Utilities;

public class ByteArrayClassLoader extends ClassLoader {

	byte[] byteArray = null;
	String className = null;
	
	public ByteArrayClassLoader(String packageName, byte[] data, ClassLoader parent) {
		super(parent);
		className = packageName;
		byteArray = data;
	}


    public ByteArrayClassLoader() {
		//  Auto-generated constructor stub
	}


	public Class findClass(String fullyQualifiedClassName) {
    	
    	int prefixEnd =  fullyQualifiedClassName.lastIndexOf('.');
	    this.className = fullyQualifiedClassName.substring(prefixEnd+1, fullyQualifiedClassName.length());
    	byteArray = null;
    	String filename =  Utilities.getCodeGenTarget() + "/" + className + ".class";

    	
           	try {
    			byteArray = Utilities.readBinaryFile(filename);
    			} catch (IOException e) {
    				e.printStackTrace();
    			}

    		
    	
    	
    	return defineClass(className,byteArray,0,byteArray.length);
    }

public Class findEmpClass(String fullyQualifiedClassName) {
    	
	
		int prefixEnd =  fullyQualifiedClassName.lastIndexOf('.');
	    this.className = fullyQualifiedClassName.substring(prefixEnd+1, fullyQualifiedClassName.length());
    	String filename = Utilities.getCodeGenTarget() + "/" + className + ".class";



	    // load local file 
    	if(byteArray == null) {
           	try {
    			byteArray = Utilities.readBinaryFile(filename);
    			

    			} catch (IOException e) {
    				e.printStackTrace();
    			}

    	}
    		
    	    return defineClass(className,byteArray,0,byteArray.length);
    }
}
