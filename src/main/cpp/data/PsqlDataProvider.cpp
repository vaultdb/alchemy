//
// Created by Jennie Rogers on 7/18/20.
//

#include "PsqlDataProvider.h"
#include "pq_oid_defs.h"
#include "pqxx_compat.h"
#include "util/type_utilities.h"

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

    size_t rowCount = 0;
    // just count the rows first
    for(result::const_iterator resultPos = pqxxResult.begin(); resultPos != pqxxResult.end(); ++resultPos) {
        ++rowCount;
    }


    std::unique_ptr<QueryTable> dstTable(new QueryTable(rowCount, false));


    int counter = 0;
    for(result::const_iterator resultPos = pqxxResult.begin(); resultPos != pqxxResult.end(); ++resultPos) {
        QueryTuple *tuple = dstTable->GetTuple(counter);
        getTuple(*resultPos, tuple);
        ++counter;
    }

    std::unique_ptr<QuerySchema> schema = getSchema(pqxxResult);
    dstTable->SetSchema(schema.get());
    return dstTable;
}

std::unique_ptr<QuerySchema> PsqlDataProvider::getSchema(pqxx::result input) {
    size_t colCount = input.columns();
    std::unique_ptr<QuerySchema> result(new QuerySchema(colCount));

    for(int i = 0; i < colCount; ++i) {
       string colName =  input.column_name(i);
       types::TypeId type = getFieldTypeFromOid(input.column_type(i));
       QueryFieldDesc fieldDesc(i, true, colName, srcTable, type);
       if(type == vaultdb::types::TypeId::VARCHAR) {

           size_t stringLength = getVarCharLength(srcTable, colName);
            fieldDesc.setStringLength(stringLength);

       }


       result->PutField(i, fieldDesc);
    }

    return result;
}

// queries the psql data definition to find the max length of each string in our column definitions
size_t PsqlDataProvider::getVarCharLength(std::string tableName, std::string columnName) const {
    // query the schema to get the column width
    //  SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name='<table>' AND column_name='<col name>';
    std::string queryCharWidth( "SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name=\'");
    queryCharWidth += tableName;
    queryCharWidth += "\' AND column_name=\'";
    queryCharWidth += columnName;
    queryCharWidth += "\'";

    pqxx::result pqxxResult = query("dbname=" + dbName, queryCharWidth);
    // read single row, single val

    row aRow = pqxxResult[0];
    field aField = aRow.at(0);

    return aField.as<size_t>();

}

void PsqlDataProvider::getTuple(pqxx::row row, QueryTuple *dstTuple) {

        dstTuple->SetIsEncrypted(false);
        const int numCols = row.size();
        dstTuple->InitDummy();
        dstTuple->setFieldCount(numCols);


        for (int i=0; i < numCols; i++) {
            const pqxx::field srcField = row[i];

            dstTuple->PutField(i, getField(srcField));
        }

    }


    std::unique_ptr<QueryField> PsqlDataProvider::getField(pqxx::field src) {

        int ordinal = src.num();
        pqxx::oid oid = src.type();
        types::TypeId colType = getFieldTypeFromOid(oid);
        types::Value value;

        std::unique_ptr<QueryField> result(new QueryField(ordinal));


        switch (colType) {
            case vaultdb::types::TypeId::INTEGER32:
            {
                int32_t intVal = src.as<int32_t>();
                return std::unique_ptr<QueryField>(new QueryField(ordinal, intVal));
            }
            case vaultdb::types::TypeId::INTEGER64:
            {
                int64_t intVal = src.as<int64_t>();
                return std::unique_ptr<QueryField>(new QueryField(ordinal, intVal));
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
                return std::unique_ptr<QueryField>(new QueryField(ordinal, boolVal));
            }
            case vaultdb::types::TypeId::FLOAT32:
            {
                float floatVal = src.as<float>();
                return std::unique_ptr<QueryField>(new QueryField(ordinal, floatVal));
            }
            case vaultdb::types::TypeId::FLOAT64:
            {
                double doubleVal = src.as<double>();
                return std::unique_ptr<QueryField>(new QueryField(ordinal, doubleVal));
            }
            case types::TypeId::VARCHAR:
            {
                string stringVal = src.as<string>();
                return std::unique_ptr<QueryField>(new QueryField(ordinal, stringVal));
            }
            default:
                throw std::invalid_argument("Unsupported column type " + std::to_string(oid));

        };

    }




