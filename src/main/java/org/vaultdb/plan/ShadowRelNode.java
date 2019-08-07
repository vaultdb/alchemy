package org.vaultdb.plan;

import org.apache.calcite.rel.RelNode;

public class ShadowRelNode {
	RelNode baseNode;
	
	public ShadowRelNode(RelNode src) {
		baseNode = src;
	}
}
