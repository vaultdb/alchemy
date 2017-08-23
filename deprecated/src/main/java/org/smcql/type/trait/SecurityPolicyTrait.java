package org.smcql.type.trait;

import org.apache.calcite.plan.RelOptPlanner;
import org.apache.calcite.plan.RelTrait;
import org.apache.calcite.plan.RelTraitDef;


// based on RelCollationImpl
// for use with relnode
public class SecurityPolicyTrait implements RelTrait {

	 public static final SecurityPolicyTrait DEFAULT =
		      SecurityPolicyTraitDef.INSTANCE.canonize(
		          new SecurityPolicyTrait());

	public enum SecurityPolicy {
        Public, Protected, Private
    };

    protected SecurityPolicy policy = SecurityPolicy.Private;
	
    private SecurityPolicyTrait() {
    	
    }
    
	public RelTraitDef<SecurityPolicyTrait> getTraitDef() {
		return SecurityPolicyTraitDef.INSTANCE;
	}

	public boolean satisfies(RelTrait trait) {
		if(trait instanceof SecurityPolicyTrait) {
			SecurityPolicy oPolicy = ((SecurityPolicyTrait)trait).policy; 
			if(policy.compareTo(oPolicy) <= 0)
				return true;
		}
		
		return false;
	}

	
	public void register(RelOptPlanner planner) {
		
	}
	
	public int hashCode() {
	    return policy.hashCode();
	  }

	  public boolean equals(Object obj) {
	    if (this == obj) {
	      return true;
	    }
	    if (obj instanceof SecurityPolicyTrait) {
	    	SecurityPolicyTrait that = (SecurityPolicyTrait) obj;
	      return this.policy.equals(that.policy);
	    }
	    return false;
	  }

}
