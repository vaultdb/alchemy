package org.vaultdb.parser;
import org.apache.calcite.plan.RelOptRule;
import org.apache.calcite.plan.RelOptRuleOperand;
import org.apache.calcite.plan.RelOptRuleCall;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.vaultdb.config.SystemConfiguration;
import org.apache.calcite.rel.core.Filter;
import org.apache.calcite.rel.core.TableScan;

import java.util.logging.Logger;



public class PushDownFilter extends RelOptRule {

    public PushDownFilter(RelOptRuleOperand operand, String description) {
        super(operand, "Push_down_rule:" + description);


    }

    public static final PushDownFilter INSTANCE =
            new PushDownFilter(
                    operand(
                            Filter.class,
                            operand(TableScan.class, none())),
                    "filter_tableScan");

    @Override
    public void onMatch(RelOptRuleCall call) {
        LogicalFilter filter = (LogicalFilter) call.rels[0];
        TableScan tableScan = (TableScan) call.rels[1]; //join

        try{
            Logger log = SystemConfiguration.getInstance().getLogger();

            log.info("PUSHDOWNFILER.java : RULEMATCH : "+filter +" "+ tableScan);

        }catch (Exception E){
            System.out.println("Logger FAIL");
        }




        // push down filter
        call.transformTo(tableScan);
    }
}