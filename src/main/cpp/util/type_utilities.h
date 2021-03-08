#ifndef _TYPE_UTILITIES_H
#define _TYPE_UTILITIES_H

#include <string>

//#include <emp-tool/circuits/integer.h>
//#include <emp-tool/circuits/float32.h>
//#include <emp-tool/circuits/bit.h>
#include "query_table/field/field.h"


// N.B. Do not use namespace std here, it will cause a naming collision in emp

namespace vaultdb {
    class TypeUtilities {

    public:
        static std::string getTypeString(const FieldType & aTypeid);

        // logical size, hence secure bit will be 1 byte (byte-aligned). needs to be unified between encrypted and plain sizes for reveal/secret share methods
        // See FieldType::getPhysicalSize() for physical, allocated size
        static size_t getTypeSize(const FieldType & id);


        // when reading data from ascii sources like csv
        // Moved this to FieldFactory
        // static types::Value decodeStringValue(const std::string & strValue, const QueryFieldDesc &fieldSpec);

        static FieldType toSecure(const FieldType & plainType);
        static FieldType toPlain(const FieldType & secureType);
        static bool  isEncrypted(const FieldType & type);

        // for reading primitives

        static bool getBool(const Field & boolField);

/*        template<typename T, typename B>
        static int32_t getInt(const Field<T,B> & intField);

        template<typename T, typename B>
        static int64_t getLong(const Field & longField);

        template<typename T, typename B>
        static float_t getFloat(const Field & floatField);
        template<typename T, typename B>
        static std::string getString(const Field & strField);
*/

        static emp::Bit getSecureBool(const Field & boolField);

        /*        static emp::Integer getSecureInt(const Field & intField);
        static emp::Integer getSecureLong(const Field & longField);
        static emp::Float getSecureFloat(const Field & floatField);
        static emp::Integer getSecureString(const Field & strField); */




};

}

#endif // UTILITIES_H
