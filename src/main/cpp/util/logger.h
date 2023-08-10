#ifndef VAULTDB_LOGGER_H
#define VAULTDB_LOGGER_H
//  based on: https://www.boost.org/doc/libs/1_58_0/libs/log/example/doc/tutorial_logging.cpp

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
//#include <boost/move/utility.hpp>
//#include <boost/log/sources/logger.hpp>
//#include <boost/log/sources/record_ostream.hpp>
//#include <boost/log/sources/global_logger_storage.hpp>
//#include <boost/log/utility/setup/file.hpp>
//#include <boost/log/utility/setup/common_attributes.hpp>
//#include <boost/date_time/posix_time/posix_time_types.hpp>
//#include <boost/log/trivial.hpp>
//#include <boost/log/expressions.hpp>
//#include <boost/log/sources/severity_logger.hpp>
//#include <boost/log/support/date_time.hpp>
//#include <boost/log/utility/setup/console.hpp>

//namespace logging = boost::log;
//namespace src = boost::log::sources;
//namespace expr = boost::log::expressions;
//namespace keywords = boost::log::keywords;


// removing dependency on logger

//BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(vaultdb_logger, src::logger_mt)
namespace Logging {
	enum class Level {
		ALL = 0, DEBUG, WARN, ERROR, OFF
	};	

	class Logger {

	public:
		void setup(const std::string & logfile_, Level log_level_ = Level::DEBUG) {
			if(!logfile_.empty()) {
				logfile.open(logfile_);
			}
			log_level = log_level_;
		}
		
		void write(const std::string & msg, Level request_level_ = Level::DEBUG) {
			if(request_level_ >= log_level) {
				std::cout << msg << '\n';
				if(logfile.is_open()) {
					logfile << msg << "\n";
				}
			}
		}

		~Logger() {
			logfile.close();
	}
	private:
		std::ofstream logfile;
		Level log_level;
	};

	inline Logger *get_log() {
		static std::unique_ptr<Logger> log = std::make_unique<Logger>();
		return log.get();
	}

	inline void start_log(const::std::string & logfile_, Level log_level_) {
		Logger *log = get_log();
		log->setup(logfile_, log_level_);
	}
}


#endif //VAULTDB_LOGGER_H
