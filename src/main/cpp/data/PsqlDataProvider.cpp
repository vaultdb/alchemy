#include "PsqlDataProvider.h"
#include "pq_oid_defs.h"
#include "util/type_utilities.h"
#include <time.h>

#include <chrono>



//typedef std::chrono::steady_clock::time_point time_point;

std::unique_ptr<QueryTable> PsqlDataProvider::getQueryTable(std::string dbname, std::string query_string) {

   return getQueryTable(dbname, query_string, false);
}

// if hasDummyTag == true, then last column needs to be a boolean that denotes whether the tuple was selected
// tableName == nullptr if query result from more than one table
std::unique_ptr<QueryTable>
PsqlDataProvider::getQueryTable(std::string dbname, std::string query_string, bool hasDummyTag) {

    dbName = dbname;

    // TODO: add in code to disconnect from psql when done
    pqxx::result pqxxResult;
    pqxx::connection dbConn("dbname=" + dbname);

    try {
        pqxx::work txn(dbConn);
        pqxxResult = txn.exec(query_string);
        txn.commit();


    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }

    pqxx::row firstRow = *(pqxxResult.begin());
    int colCount = firstRow.size();
    if(hasDummyTag)
        --colCount;

    QueryTuple tuple(colCount, false);

    size_t rowCount = 0;
    // just count the rows first
    for(result::const_iterator resultPos = pqxxResult.begin(); resultPos != pqxxResult.end(); ++resultPos) {
        ++rowCount;
    }



    std::unique_ptr<QueryTable> dstTable = std::make_unique<QueryTable>(rowCount, colCount, false);
    tableSchema = getSchema(pqxxResult, hasDummyTag);

    dstTable->setSchema(*tableSchema);

    int counter = 0;
    for(result::const_iterator resultPos = pqxxResult.begin(); resultPos != pqxxResult.end(); ++resultPos) {
        tuple = getTuple(*resultPos, hasDummyTag);
        dstTable->putTuple(counter, tuple);
        ++counter;
    }

    dbConn.disconnect();

    return dstTable;
}

std::unique_ptr<QuerySchema> PsqlDataProvider::getSchema(pqxx::result input, bool hasDummyTag) {
    pqxx::row firstRow = *(input.begin());
    int colCount = firstRow.size();
    if(hasDummyTag)
        --colCount; // don't include dummy tag as a separate column


    std::unique_ptr<QuerySchema> result(new QuerySchema(colCount));

    for(int i = 0; i < colCount; ++i) {
       string colName =  input.column_name(i);
       types::TypeId type = getFieldTypeFromOid(input.column_type(i));
        int tableId = input.column_table(i);

        srcTable = getTableName(tableId); // once per col in case of joins

        QueryFieldDesc fieldDesc(i, colName, srcTable, type);

       if(type == vaultdb::types::TypeId::VARCHAR) {

           size_t stringLength = getVarCharLength(srcTable, colName);
            fieldDesc.setStringLength(stringLength);

       }


        result->putField(fieldDesc);
    }

   if(hasDummyTag) {
        pqxx::oid dummyTagOid = input.column_type((int) colCount);
        vaultdb::types::TypeId dummyType = getFieldTypeFromOid(dummyTagOid);
        assert(dummyType == vaultdb::types::TypeId::BOOLEAN); // check that dummy tag is a boolean
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



string PsqlDataProvider::getTableName(int oid) {
    std::string queryTableName = "SELECT relname FROM pg_class WHERE oid=" + std::to_string(oid);
    pqxx::result pqxxResult = query("dbname=" + dbName, queryTableName);
    // read single row, single val

    if(pqxxResult.empty())
        return "";

    row aRow = pqxxResult[0];
    field aField = aRow.at(0);

    return aField.as<string>();

}

QueryTuple PsqlDataProvider::getTuple(pqxx::row row, bool hasDummyTag) {
        int colCount = row.size();


        if(hasDummyTag) {
            --colCount;
        }

        QueryTuple dstTuple(colCount);

    dstTuple.setIsEncrypted(false);


        for (int i=0; i < colCount; i++) {
            const pqxx::field srcField = row[i];

           QueryField parsedField = getField(srcField);
            dstTuple.putField(parsedField);
        }

        if(hasDummyTag) {

                QueryField parsedField(getField(row[colCount])); // get the last col
                bool dummyTag = parsedField.getValue().getBool();
                types::Value dummyTagValue(dummyTag);
                dstTuple.setDummyTag(dummyTagValue);
        }

    return dstTuple;
    }


    QueryField PsqlDataProvider::getField(pqxx::field src) {

        int ordinal = src.num();
        pqxx::oid oid = src.type();
        types::TypeId colType = getFieldTypeFromOid(oid);

        switch (colType) {
            case vaultdb::types::TypeId::INTEGER32:
            {
                int32_t intVal = src.as<int32_t>();
                types::Value val(intVal);
                return QueryField(ordinal, intVal);
            }
            case vaultdb::types::TypeId::INTEGER64:
            {
                int64_t intVal = src.as<int64_t>();
                return QueryField(ordinal, types::Value(intVal));
            }

           case vaultdb::types::TypeId::DATE:
            {
                std::string dateStr = src.as<std::string>(); // YYYY-MM-DD
                std::tm timeStruct = {};
                strptime(dateStr.c_str(), "%Y-%m-%d", &timeStruct);
                int64_t epoch = mktime(&timeStruct) - 21600; // date time function is 6 hours off from how psql does it, TODO: track this down, probably a timezone problem
                return QueryField(ordinal, types::Value(types::TypeId::INTEGER64, epoch));

                /*std::chrono::steady_clock::time_point timePoint = src.as<std::chrono::steady_clock::time_point>();
                int64_t epoch = std::chrono::duration_cast<std::chrono::seconds>(
                        timePoint.time_since_epoch()).count();
               value = types::Value(colType, epoch);
               result->setValue(&value);
               break;*/
            }
            case vaultdb::types::TypeId::BOOLEAN:
            {
                bool boolVal = src.as<bool>();
                return QueryField(ordinal, types::Value(boolVal));
            }
            case vaultdb::types::TypeId::FLOAT32:
            {
                float floatVal = src.as<float>();
                return  QueryField(ordinal, types::Value(floatVal));
            }

            case types::TypeId::VARCHAR:
            {
                string stringVal = src.as<string>();
                size_t strLength = tableSchema->getField(ordinal).getStringLength();

                while(stringVal.size() != strLength) {
                    stringVal += " ";
                }

                return QueryField(ordinal, types::Value(stringVal));
            }
            default:
                throw std::invalid_argument("Unsupported column type " + std::to_string(oid));

        };

    }

pqxx::result PsqlDataProvider::query(const string &dbname, const string &query_string) const {
    pqxx::result res;
    try {
        pqxx::connection c(dbname);
        pqxx::work txn(c);

        res = txn.exec(query_string);
        txn.commit();


    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }

    return res;
}



