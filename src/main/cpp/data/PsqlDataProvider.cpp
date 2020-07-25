//
// Created by Jennie Rogers on 7/18/20.
//

#include "PsqlDataProvider.h"
#include "pq_oid_defs.h"
#include "pqxx_compat.h"

#include <chrono>



//typedef std::chrono::steady_clock::time_point time_point;

std::unique_ptr<QueryTable> PsqlDataProvider::GetQueryTable(std::string dbname,
                                          std::string query_string, std::string tableName) {

   return  GetQueryTable(dbname, query_string,  tableName,  false);
}

// TODO: implement dummy tag support
// tableName == nullptr if query result from more than one table
// TODO: deduce table name from query, plugging this in for now with a member variable
std::unique_ptr<QueryTable> PsqlDataProvider::GetQueryTable(std::string dbname,
                                          std::string query_string,  std::string tableName, bool hasDummyTag) {

    srcTable = tableName;
    dbName = dbname;
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
       QueryFieldDesc fieldDesc(i, true, colName, srcTable, type);
       if(type == vaultdb::types::TypeId::VARCHAR) {
           // query the schema to get the column width
       //  SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name='<table>' AND column_name='<col name>';
            std::string queryCharWidth( "SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name=\'");
           queryCharWidth += srcTable;
           queryCharWidth += "\' AND column_name=\'";
           queryCharWidth += input.column_name(i);
           queryCharWidth += "\'";

           pqxx::result pqxxResult = query("dbname=" + dbName, queryCharWidth);
           // read single row, single val

            size_t stringWidth = pqxxResult.at(0)[0].as<size_t>();

            fieldDesc.setStringLength(stringWidth);

       }


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




