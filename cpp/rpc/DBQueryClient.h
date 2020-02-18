//
// Created by madhav on 12/24/19.
//

#ifndef TESTING_DBQUERYCLIENT_H
#define TESTING_DBQUERYCLIENT_H

#include "dbquery.grpc.pb.h"
#include "dbquery.pb.h"
#include <grpcpp/grpcpp.h>

using namespace dbquery;
using namespace std;

class DBQueryClient {
public:
  DBQueryClient(shared_ptr<grpc::Channel> channel)
      : stub_(JDBCReader::NewStub(channel)) {}

  Table GetTable(string sqlString, string dbString, string user,
                 string password);

private:
  unique_ptr<JDBCReader::Stub> stub_;
};

#endif // TESTING_DBQUERYCLIENT_H
