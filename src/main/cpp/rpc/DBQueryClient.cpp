//
// Created by madhav on 12/24/19.
//

#include "DBQueryClient.h"

Table DBQueryClient::GetTable(string sqlString, string dbString, string user,
                              string password) {
    cout << "We should never get here. " << sqlString << ", " << password << endl;
  /*GetTableRequest req;
  GetTableResponse resp;
  grpc::ClientContext context;
  req.set_sqlstring(sqlString);
  req.set_dbstring(dbString);
  req.set_user(user);
  req.set_password(password);
  //auto status = stub_->GetTable(&context, req, &resp);
  //if (status.ok()) {
 //   return resp.table();
 // } */
}

int main() {


    cout << "This is just a placeholder." << endl;
}
