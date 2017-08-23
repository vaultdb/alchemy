package org.smcql.type.trait;

import org.apache.calcite.plan.RelOptPlanner;
import org.apache.calcite.plan.RelTraitDef;
import org.apache.calcite.plan.RelTraitSet;
import org.apache.calcite.rel.RelNode;

// based on RelCollationTraitDef
public class SecurityPolicyTraitDef extends RelTraitDef<SecurityPolicyTrait> {
	public static final SecurityPolicyTraitDef INSTANCE =
		      new SecurityPolicyTraitDef();


	@Override
	public Class<SecurityPolicyTrait> getTraitClass() {
		return SecurityPolicyTrait.class;
	}

	@Override
	public String getSimpleName() {
		return "security-policy";
	}

	@Override
	public RelNode convert(RelOptPlanner planner, RelNode rel, SecurityPolicyTrait toTrait,
			boolean allowInfiniteCostConverters) {
		
		 final RelTraitSet newTraitSet = rel.getTraitSet().replace(toTrait);
		 if (!rel.getTraitSet().equals(newTraitSet)) {			    
			 return planner.changeTraits(rel, newTraitSet);
		 }
		   
		return rel;
	}

	@Override
	public boolean canConvert(RelOptPlanner planner, SecurityPolicyTrait fromTrait, SecurityPolicyTrait toTrait) {
		if(!fromTrait.satisfies(toTrait) || fromTrait.equals(toTrait)) // if going from lower security policy to higher one
			return true;
		return false;
	}

	@Override
	public SecurityPolicyTrait getDefault() {
		return  SecurityPolicyTrait.DEFAULT;
	}
	

}
