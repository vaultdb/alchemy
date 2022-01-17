#include "logger.h"


void Logger::setup(const std::string & logfile_prefix) {
    if(!logfile_prefix.empty()) {
        std::string logfile_name =  logfile_prefix + "_%N.log";

        logging::add_file_log
                (
                        keywords::file_name = logfile_name,
                        // This makes the sink to write log records that look like this:
                        // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
                        // YYYY-MM-DD HH:MI:SS: <error> An error severity message
                        keywords::format =
                                (
                                        expr::stream
                                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
                                                                                                    "%Y-%m-%d %H:%M:%S")
                                                << ": <" << logging::trivial::severity
                                                << "> " << expr::smessage
                                )
                );
    }
    else { // console logger - default
        logging::add_console_log
                (std::cout,
                        // This makes the sink to write log records that look like this:
                        // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
                        // YYYY-MM-DD HH:MI:SS: <error> An error severity message
                         boost::log::keywords::format =
                                (
                                        expr::stream
                                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
                                                                                                    "%Y-%m-%d %H:%M:%S")
                                                << ": <" << logging::trivial::severity
                                                << "> " << expr::smessage
                                )
                );
    }
//    boost::log::register_simple_formatter_factory< logging::trivial::severity_level, char >("Severity");

    logging::add_common_attributes();
    // set logging level to one of trace, debug, info, warning, error, fatal
    /*logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::severity_level::trace
            );
*/

}

void Logger::write(const std::string &msg, const logging::trivial::severity_level & severity) {
    src::logger_mt& lg = vaultdb_logger::get();
    BOOST_LOG_SEV(lg, severity) << msg;
}
