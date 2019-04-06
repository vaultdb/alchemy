package org.smcql.executor.RPC;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import org.smcql.compiler.emp.EmpRunnable;
import org.smcql.executor.ExecuteSegmentRequest;
import org.smcql.executor.ExecuteSegmentResponse;
import org.smcql.executor.VaultQueryExecutorGrpc;

public class RPCExecutorClient {
  private final ManagedChannel channel;
  private final VaultQueryExecutorGrpc.VaultQueryExecutorBlockingStub blockingStub;
  private int party = 0;
  private boolean[] lastOutput;

  public RPCExecutorClient(String host, int port) {
    this(ManagedChannelBuilder.forAddress(host, port).usePlaintext().build());
  }

  public RPCExecutorClient(String host, int port, int party) {
    this(ManagedChannelBuilder.forAddress(host, port).usePlaintext().build());
    this.party = party;
  }

  public RPCExecutorClient(ManagedChannel channel) {
    this.channel = channel;
    blockingStub = VaultQueryExecutorGrpc.newBlockingStub(channel);
  }

  public void setParty(int party) {
    this.party = party;
  }

  public boolean[] getLastOutput() {
    return lastOutput;
  }

  public boolean[] executeSegment(
      String queryId,
      String jniSource,
      String empSource,
      String jniPath,
      String empPath,
      int empPort) {
    ExecuteSegmentRequest request =
        ExecuteSegmentRequest.newBuilder()
            .setEmpSource(empSource)
            .setJniSource(jniSource)
            .setJniFileName(jniPath)
            .setEmpFileName(empPath)
            .setQueryId(queryId)
            .setEmpPort(empPort)
            .setEmpParty(this.party)
            .build();
    ExecuteSegmentResponse response;
    try {
      response = blockingStub.executeSegment(request);
      lastOutput = EmpRunnable.stringToBool(response.getXorResp());
      return lastOutput;
    } catch (StatusRuntimeException e) {
      // TODO(madhavsuresh): add logging here
      return null;
    }
  }
}
