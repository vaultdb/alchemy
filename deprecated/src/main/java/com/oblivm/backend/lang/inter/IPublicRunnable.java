package com.oblivm.backend.lang.inter;

import com.oblivm.backend.oram.SecureArray;

public interface IPublicRunnable<T> {
	public T[] main(int lenA, int lenB, T[] x, T[] y) throws Exception;
	
	public int[] runPublic(SecureArray<T> a, SecureArray<T> b) throws Exception;
}
