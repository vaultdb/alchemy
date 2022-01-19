#ifndef PILOT_UTILITIES_H
#define PILOT_UTILITIES_H

#include <string>


namespace  vaultdb {
    class PilotUtilities {
    public:
        static std::string getRollupExpectedResultsSql(const std::string &groupByColName);

        static const std::string data_cube_sql_;
    };

}
#endif //PILOT_UTILITIES_H
