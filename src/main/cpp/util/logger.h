#ifndef VAULTDB_LOGGER_H
#define VAULTDB_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <gflags/gflags.h>
namespace Logging {
	enum class Level {
	ALL = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5,
		OFF = 6
	};	

	class Logger {

	public:
		
		void write(const std::string & msg, Level request_level_ = Level::DEBUG); 

		~Logger() {
		}
	};


	inline Logger *get_log() {
		static std::unique_ptr<Logger> log = std::make_unique<Logger>();
		return log.get();
	}
}


#endif //VAULTDB_LOGGER_H
