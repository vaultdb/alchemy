#ifndef VAULTDB_LOGGER_H
#define VAULTDB_LOGGER_H
//  based on: https://www.boost.org/doc/libs/1_58_0/libs/log/example/doc/tutorial_logging.cpp


#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;



BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(vaultdb_logger, src::logger_mt)

class Logger {

public:
    static void setup(const std::string & logfile_prefix = "");
    static void write(const std::string & msg, const logging::trivial::severity_level & severity = logging::trivial::severity_level::info);


};


#endif //VAULTDB_LOGGER_H
