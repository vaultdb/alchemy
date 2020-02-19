import com.dbquery.DBQueryProtos;
import com.dbquery.JDBCReaderGrpc;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

public class JDBCServer {
  private Server server;

  public void start(int port) throws IOException {
    //server = ServerBuilder.forPort(port).addService(new JDBCServerImpl());
    server = ServerBuilder.forPort(port)
    .addService(new JDBCServerImpl())
    .build()
    .start();
    Runtime.getRuntime().addShutdownHook(new Thread() {
      @Override
      public void run() {
        try {
          JDBCServer.this.stop();

        } catch (InterruptedException e) {
          e.printStackTrace(System.err);
        }
      }
    });
  }

  private void stop() throws InterruptedException {
    if (server != null) {
      server.shutdown().awaitTermination(30, TimeUnit.SECONDS);
    }
  }

  private void blockUntilShutdown() throws InterruptedException {
    if (server != null) {
      server.awaitTermination();
    }
  }

  static class JDBCServerImpl extends JDBCReaderGrpc.JDBCReaderImplBase {

    @Override
    public void getTable(
        DBQueryProtos.GetTableRequest request,
        StreamObserver<DBQueryProtos.GetTableResponse> responseObserver) {
      DBQuery query = new DBQuery(request.getSqlString(), request.getDbString(), request.getUser(),
          request.getPassword());
      DBQueryProtos.GetTableResponse response = DBQueryProtos.GetTableResponse.newBuilder()
          .setTable(query.runQuery())
          .build();
      responseObserver.onNext(response);
      responseObserver.onCompleted();
    }
  }
  public static void main(String[] args) throws IOException, InterruptedException {
    final JDBCServer server = new JDBCServer();
    server.start(50000);
    server.blockUntilShutdown();

  }
}
