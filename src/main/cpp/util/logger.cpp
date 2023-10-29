#include "logger.h"

//DEFINE_int32(log_level, 2, "log level (higher means less log info");
//DEFINE_string(logfile, "", "log file name (empty means don't write to a file)");
#define LOG_LEVEL 2 // default

using namespace Logging;

void Logger::write(const std::string & msg, Level request_level_ ) {
    if ((int) request_level_ >= LOG_LEVEL) {
        std::cout << msg << '\n';
        if (!log_file_.empty()) {
            std::ofstream logfile;
            logfile.open(log_file_, std::ios::out | std::ios::app);
            logfile << msg << "\n";
            logfile.close();
        }
    }
}

//
//void Logger::setup(const std::string & logfile_prefix) {
//    if(!logfile_prefix.empty()) {
//        std::string logfile_name =  logfile_prefix + "_%N.log";
//
//        logging::add_file_log
//                (
//                        keywords::file_name = logfile_name,
//                        // This makes the sink to write log records that look like this:
//                        // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
//                        // YYYY-MM-DD HH:MI:SS: <error> An error severity message
//                        keywords::format =
//                                (
//                                        expr::stream
//                                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
//                                                                                                    "%Y-%m-%d %H:%M:%S")
//                                                << ": <" << logging::trivial::severity
//                                                << "> " << expr::smessage
//                                )
//                );
//    }
//    else { // console logger - default
//        logging::add_console_log
//                (std::cout,
//                        // This makes the sink to write log records that look like this:
//                        // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
//                        // YYYY-MM-DD HH:MI:SS: <error> An error severity message
//                         boost::log::keywords::format =
//                                (
//                                        expr::stream
//                                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
//                                                                                                    "%Y-%m-%d %H:%M:%S")
//                                                << ": <" << logging::trivial::severity
//                                                << "> " << expr::smessage
//                                )
//                );
//    }
////    boost::log::register_simple_formatter_factory< logging::trivial::severity_level, char >("Severity");
//
//    logging::add_common_attributes();
//    // set logging level to one of trace, debug, info, warning, error, fatal
//    /*logging::core::get()->set_filter
//            (
//                    logging::trivial::severity >= logging::trivial::severity_level::trace
//            );
//*/
//
//}
//
//void Logger::write(const std::string &msg, const logging::trivial::severity_level & severity) {
//    src::logger_mt& lg = vaultdb_logger::get();
//    BOOST_LOG_SEV(lg, severity) << msg;
//}
