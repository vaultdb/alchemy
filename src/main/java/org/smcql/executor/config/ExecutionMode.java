package org.smcql.executor.config;



public class ExecutionMode {
		public boolean distributed = true; // || distributed.   Distributed clear usually will be a semi-join or some other rewrite that isolates the public variables
		public boolean oblivious = true;
		public boolean sliced = false; // might get parlayed into something with a fixed output size inferred from schema info, e.g., pkey, # of distinct values, sliced card summed up, this may be an interesting research challenge: optimizing how these knobs play together
		public boolean replicated = false; // when a table is replicated among all data owners, holds true when we join replicated tables too
	

		@Override
		public String toString() {

			String descriptor = distributed ?  "Distributed"
					:  "Local";
			
			descriptor += oblivious ? "Oblivious" : "Clear";
			descriptor += replicated ? "Replicated" : "Sharded";
			return descriptor;
		}
}
