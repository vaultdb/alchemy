package org.smcql.executor;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class ExecutorClient {
    private final ManagedChannel channel;
    private final QueryExecutorGrpc.QueryExecutorBlockingStub blockingStub;

    public ExecutorClient(String host, int port) {
        this(ManagedChannelBuilder.forAddress(host, port).usePlaintext().build());
    }

    public ExecutorClient(ManagedChannel channel) {
        this.channel = channel;
        blockingStub = QueryExecutorGrpc.newBlockingStub(channel);
    }

    public void executeSegment(String source) {

    }
}
