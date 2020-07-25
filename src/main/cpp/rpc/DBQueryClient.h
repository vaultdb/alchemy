//
// Created by madhav on 12/24/19.
//

#ifndef _DBQUERYCLIENT_H
#define _DBQUERYCLIENT_H

#include "rpc/dbquery.pb.h"
#include <grpcpp/grpcpp.h>

using namespace dbquery;

//This a placeholder to serve as dependency for generating protobuf files
class DBQueryClient {
public:
  DBQueryClient(shared_ptr<grpc::Channel> channel)
      //: stub_(JDBCReader::NewStub(channel))
      {}

  Table GetTable(string sqlString, string dbString, string user,
                 string password);

private:
  //unique_ptr<JDBCReader::Stub> stub_;
};

#endif // _DBQUERYCLIENT_H
