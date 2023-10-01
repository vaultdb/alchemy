#include "logger.h"

DEFINE_int32(log_level, 2, "log level (higher means less log info");
DEFINE_string(logfile, "", "log file name (empty means don't write to a file)");

using namespace Logging;

void Logger::write(const std::string & msg, Level request_level_ )
{
	if((int)request_level_ >= FLAGS_log_level) {		
		std::cout << msg << '\n';
		if(!FLAGS_logfile.empty()) {
			std::ofstream logfile;
			logfile.open(FLAGS_logfile, std::ios::out | std::ios::app);
			logfile << msg << "\n";
		}
	}
}
