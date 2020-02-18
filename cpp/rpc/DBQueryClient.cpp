//
// Created by madhav on 12/24/19.
//

#include "DBQueryClient.h"

Table DBQueryClient::GetTable(string sqlString, string dbString, string user,
                              string password) {
  GetTableRequest req;
  GetTableResponse resp;
  grpc::ClientContext context;
  req.set_sqlstring(sqlString);
  req.set_dbstring(dbString);
  req.set_user(user);
  req.set_password(password);
  auto status = stub_->GetTable(&context, req, &resp);
  if (status.ok()) {
    return resp.table();
  }
};

int main() {
  DBQueryClient client(grpc::CreateChannel("localhost:50000",
                                           grpc::InsecureChannelCredentials()));
  client.GetTable("SELECT * FROM customer, part LIMIT 10",
                  "jdbc:postgresql://localhost/tpch_scale_01", "madhav",
                  "password");
}
