package org.smcql.executor.RPC;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpRunnable;
import org.smcql.executor.ExecuteSegmentResponse;
import org.smcql.executor.VaultQueryExecutorGrpc;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

import java.io.IOException;

public class RPCExecutorServer {
  private Server server;

  public RPCExecutorServer() {}

  public void Start(int port) throws IOException {
    server = ServerBuilder.forPort(port).addService(new ExecutorServerImpl()).build().start();
    Runtime.getRuntime().addShutdownHook(new Thread(() -> RPCExecutorServer.this.Stop()));
  }

  public void Stop() {
    if (server != null) {
      server.shutdown();
    }
  }

  public void BlockUntilShutdown() throws InterruptedException {
    if (server != null) {
      server.awaitTermination();
    }
  }

  public static void main(String[] args) throws Exception {
    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    RPCExecutorServer server = new RPCExecutorServer();
    server.Start(10000);
    server.BlockUntilShutdown();
  }

  static class ExecutorServerImpl extends VaultQueryExecutorGrpc.VaultQueryExecutorImplBase {

    @Override
    public void executeSegment(
        org.smcql.executor.ExecuteSegmentRequest request,
        io.grpc.stub.StreamObserver<org.smcql.executor.ExecuteSegmentResponse> responseObserver) {
      ExecuteSegmentResponse resp = null;
      try {
        EmpJniUtilities.cleanEmpCode(request.getQueryId());
        FileUtils.writeFile(
            Utilities.getCodeGenTarget() + "/" + request.getEmpFileName(), request.getEmpSource());
        FileUtils.writeFile(
            Utilities.getCodeGenTarget() + "/" + request.getJniFileName(), request.getJniSource());
        String fullyQualifiedClassName =
            EmpJniUtilities.getFullyQualifiedClassName(request.getQueryId());
        EmpRunnable partyRunnable =
            new EmpRunnable(fullyQualifiedClassName, request.getEmpParty(), request.getEmpPort());
        EmpBuilder builder = new EmpBuilder(fullyQualifiedClassName);
        builder.compile();
        partyRunnable.run();
        resp =
            ExecuteSegmentResponse.newBuilder().setXorResp(partyRunnable.getOutputString()).build();
      } catch (Exception e) {
        // TODO(madhavsuresh): fill in exception (just kill the app for now).
      }
      responseObserver.onNext(resp);
      responseObserver.onCompleted();
    }
  }
}
