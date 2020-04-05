#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
	long total_jiffies = LinuxParser::Jiffies();
	long active_jiffies = LinuxParser::ActiveJiffies();
	
 	return (float)active_jiffies /total_jiffies;
}