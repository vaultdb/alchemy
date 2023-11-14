#include "psql_data_provider.h"
#include "pq_oid_defs.h"
#include <typeinfo>

#include <query_table/plain_tuple.h>
#include "query_table/field/field.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "query_table/query_table.h"
#include "util/system_configuration.h"


// if has_dummy_tag == true, then last column needs to be a boolean that denotes whether the tuple was selected
// tableName == nullptr if query result from more than one table
PlainTable *PsqlDataProvider::getQueryTable(std::string db_name, std::string sql, bool has_dummy_tag) {

    db_name_ = db_name;
    storage_model_ = SystemConfiguration::getInstance().storageModel();

    pqxx::result res;
    pqxx::connection conn("user=vaultdb dbname=" + db_name_);

    try {
        pqxx::work txn(conn);
        res = txn.exec(sql);
        txn.commit();


    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }


    pqxx::row first_row = *(res.begin());
    size_t row_cnt = res.size();

    schema_ = getSchema(res, has_dummy_tag);
    PlainTable *dst_table = PlainTable::getTable(row_cnt, schema_);


    int counter = 0;
    for(result::const_iterator pos = res.begin(); pos != res.end(); ++pos) {
        getTuple(*pos, has_dummy_tag, *dst_table, counter);
        ++counter;
    }

    conn.close();

    return dst_table;
}

QuerySchema PsqlDataProvider::getSchema(pqxx::result input, bool has_dummy_tag) {
    pqxx::row first_row = *(input.begin());
    size_t col_cnt = first_row.size();
    if(has_dummy_tag)
        --col_cnt; // don't include dummy tag as a separate column


    QuerySchema result;

    for(uint32_t i = 0; i < col_cnt; ++i) {
       string col_name =  input.column_name(i);
       FieldType type = getFieldTypeFromOid(input.column_type(i));
        int table_id = input.column_table(i);

        src_table_ = getTableName(table_id); // once per col in case of joins

        size_t len = (type == FieldType::STRING) ? getVarCharLength(src_table_, col_name) : 0;
        QueryFieldDesc fieldDesc(i, col_name, src_table_, type, len);

        result.putField(fieldDesc);
    }

   if(has_dummy_tag) {
        pqxx::oid oid = input.column_type((int) col_cnt);
        FieldType dummy_type = getFieldTypeFromOid(oid);
        assert(dummy_type == FieldType::BOOL); // check that dummy tag is a boolean
    }

    result.initializeFieldOffsets();
    return result;
}

// queries the psql data definition to find the max length of each string in our column definitions
size_t PsqlDataProvider::getVarCharLength(std::string table_name, std::string col_name) const {
    // query the schema to get the column width
    //  SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name='<table>' AND column_name='<col name>';
    if(!table_name.empty()) {
        std::string sql_str_width(
                "SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_schema=\'public\'  AND table_name=\'");
        sql_str_width += table_name;
        sql_str_width += "\' AND column_name=\'";
        sql_str_width += col_name;
        sql_str_width += "\'";

        pqxx::result res = query("user=vaultdb dbname=" + db_name_, sql_str_width);
        // read single row, single val
        // ambiguity with attribute resolution not yet implemented
        assert(res.size() == 1);
        row a_row = res[0];
        field a_field = a_row.at(0);

        return a_field.as<size_t>();
    }

    // otherwise try selecting from public schema
    // if only one column exists with this name, use its stats
    std::string sql_str_width(
            "SELECT character_maximum_length FROM INFORMATION_SCHEMA.COLUMNS WHERE table_schema=\'public\'  AND column_name=\'");
    sql_str_width += col_name;
    sql_str_width += "\'";

    pqxx::result res = query("user=vaultdb dbname=" + db_name_, sql_str_width);
    assert(res.size() == 1);

    row a_row = res[0];
    field a_field = a_row.at(0);

    return a_field.as<size_t>();

}



string PsqlDataProvider::getTableName(int oid) {
    std::string queryTableName = "SELECT relname FROM pg_class WHERE oid=" + std::to_string(oid);
    pqxx::result res = query("user=vaultdb dbname=" + db_name_, queryTableName);
    // read single row, single val

    if(res.empty())
        return "";

    row a_row = res[0];
    field a_field = a_row.at(0);

    return a_field.as<string>();

}

void
PsqlDataProvider::getTuple(pqxx::row row, bool has_dummy_tag, PlainTable &dst_table, const size_t &idx) {
        int col_count = row.size();

        if(has_dummy_tag) {
            --col_count;
        }


        for (int i=0; i < col_count; i++) {
            const pqxx::field src = row[i];

           PlainField  parsed = getField(src);
           dst_table.setField(idx, i, parsed);
        }

        if(has_dummy_tag) {

                PlainField parsed = getField(row[col_count]); // get the last col
                dst_table.setDummyTag(idx, parsed.getValue<bool>());
        }
        else {
            dst_table.setDummyTag(idx, false); // default, not a dummy
        }

    }


    PlainField PsqlDataProvider::getField(pqxx::field src) {
        if(src.is_null())
            throw std::invalid_argument("Null field, aborting!"); // Null handling NYI

        int ordinal = src.num();
        pqxx::oid oid = src.type();
        FieldType col_type = getFieldTypeFromOid(oid);

        switch (col_type) {
            case FieldType::INT:
            {
                return  PlainField(col_type, src.as<int32_t>());
            }
            case FieldType::LONG:
            {
                return  PlainField(col_type, src.as<int64_t>());
            }

           case FieldType::DATE:
            {
	      
	      std::string date_str = src.as<std::string>(); // YYYY-MM-DD
	      boost::gregorian::date date(boost::gregorian::from_string(date_str));
	      boost::gregorian::date epoch_start(1970, 1, 1);
              int64_t epoch = (date - epoch_start).days() * 24 * 3600;
	    
                return  PlainField(FieldType::LONG, epoch);

            }
            case FieldType::BOOL:
            {
                return  PlainField(col_type, src.as<bool>());
            }
            case FieldType::FLOAT:
            {
                return  PlainField(col_type, src.as<float>());
            }

            case FieldType::STRING:
            {
                string str = src.as<string>();
                size_t len = schema_.getField(ordinal).getStringLength();
                while(str.size() < len) {
                    str += " ";
                }


                return  PlainField(col_type, str, len);
            }
            default:
                throw std::invalid_argument("Unsupported column type " + std::to_string(oid));

        };

    }

pqxx::result PsqlDataProvider::query(const string &db_name, const string &sql) const {
    pqxx::result res;
    try {
        pqxx::connection c(db_name);
        pqxx::work txn(c);

        res = txn.exec(sql);
        txn.commit();


    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }

    return res;
}



