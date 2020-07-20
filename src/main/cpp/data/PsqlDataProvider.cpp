//
// Created by Jennie Rogers on 7/18/20.
//

#include "PsqlDataProvider.h"
#include "pq_oid_defs.h"
#include "pqxx_compat.h"

#include <chrono>



//typedef std::chrono::steady_clock::time_point time_point;

std::unique_ptr<QueryTable> PsqlDataProvider::GetQueryTable(std::string dbname,
                                          std::string query_string) {

   return  GetQueryTable(dbname, query_string, false);
}

// TODO: implement dummy tag support
std::unique_ptr<QueryTable> PsqlDataProvider::GetQueryTable(std::string dbname,
                                          std::string query_string, bool hasDummyTag) {

    result pqxxResult = query("dbname=" + dbname, query_string);
    vector<pqxx::row> rows;

    for(pqxx::row aRow : pqxxResult) {
        rows.emplace_back(aRow);
    }

    std::unique_ptr<QueryTable> result(new QueryTable(false, rows.size()));

    for(int i = 0; i < rows.size(); ++i) {
        QueryTuple *tuple = result->GetTuple(i);
         getTuple(rows[i], tuple);
    }

    std::unique_ptr<QuerySchema> schema = getSchema(pqxxResult);
    result->SetSchema(schema.get());

    return result;
}

std::unique_ptr<QuerySchema> PsqlDataProvider::getSchema(pqxx::result input) {
    size_t colCount = input.columns();
    std::unique_ptr<QuerySchema> result(new QuerySchema(colCount));

    for(int i = 0; i < colCount; ++i) {
       string colName =  input.column_name(i);
       types::TypeId type = getFieldTypeFromOid(input.column_type(i));
       QueryFieldDesc fieldDesc(i, true, colName, "anonymous", type);
       result->PutField(i, fieldDesc);
    }

    return result;
}

void PsqlDataProvider::getTuple(pqxx::row row, QueryTuple *dstTuple) {

        dstTuple->SetIsEncrypted(false);

        const int numCols = row.size();
        dstTuple->InitDummy();


        for (int i=0; i < numCols; i++) {
            const pqxx::field srcField = row[i];
            dstTuple->PutField(i, getField(srcField));
        }

    }


    std::unique_ptr<QueryField> PsqlDataProvider::getField(pqxx::field src) {

        int fieldNumber = src.num();
        pqxx::oid oid = src.type();
        types::TypeId colType = getFieldTypeFromOid(oid);
        types::Value value;

        std::unique_ptr<QueryField> result(new QueryField(fieldNumber));


        //     cout << "Looking up " << res.column_name(i) << " type ID: " << res.column_type(i) << endl;

        switch (colType) {
            case vaultdb::types::TypeId::INTEGER32:
            {
                int32_t intVal = src.as<int32_t>();
                value = types::Value(colType, intVal);
                result->SetValue(&value);
                break;
            }
            case vaultdb::types::TypeId::INTEGER64:
            {
                int64_t intVal = src.as<int64_t>();
                value = types::Value(colType, intVal);
                result->SetValue(&value);
                break;
            }
        /*    case vaultdb::types::TypeId::TIMESTAMP:
            {
                std::chrono::steady_clock::time_point timePoint = src.as<std::chrono::steady_clock::time_point>();
                int64_t epoch = std::chrono::duration_cast<std::chrono::seconds>(
                        timePoint.time_since_epoch()).count();
               value = types::Value(colType, epoch);
               result->SetValue(&value);
               break;
            }*/
            case vaultdb::types::TypeId::BOOLEAN:
            {
                bool boolVal = src.as<bool>();
                value = types::Value(colType, boolVal);
                result->SetValue(&value);
                break;
            }
            case vaultdb::types::TypeId::FLOAT32:
            {
                float floatVal = src.as<float>();
                value = types::Value(colType, floatVal);
                result->SetValue(&value);
                break;
            }
            case vaultdb::types::TypeId::FLOAT64:
            {
                double doubleVal = src.as<double>();
                value = types::Value(colType, doubleVal);
                result->SetValue(&value);
                break;
            }
            case types::TypeId::VARCHAR:
            {  string stringVal = src.as<string>();
                value = types::Value(colType, stringVal);
                result->SetValue(&value);
                break; }
            default:
                throw std::invalid_argument("Unsupported column type " + std::to_string(oid));

        };


        return result;
    }



    pqxx::result PsqlDataProvider::readQuery(string query, string dbName) {
        pqxx::result res;
        try {
            string config= "dbname=" + dbName + " host=127.0.0.1 port=5432"; // TODO: put this into a config file
            connection c(config);
            pqxx::work workObj(c);
            res = workObj.exec(query);
            workObj.commit();
        } catch (const std::exception &e) {
            std::cerr << "Error reading data: " << e.what() << std::endl;

            throw e;
        }

    }

