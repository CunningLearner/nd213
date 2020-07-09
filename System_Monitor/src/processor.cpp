#include "processor.h"
#include "linux_parser.h"
// #include <unistd.h>
// #include <cstdlib>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
//   float cpu_util = (float)(LinuxParser::ActiveJiffies()) / (LinuxParser::Jiffies());
//   sleep(5);
//   float ActJif = LinuxParser::ActiveJiffies();
//   float TotJif = LinuxParser::Jiffies();// * 100;
//   float cpu_util = ActJif/TotJif;
//   float cpu_util = (float)LinuxParser::ActiveJiffies();
//   return (cpu_util);
//   return 0.0;
  return((float)LinuxParser::ActiveJiffies() / LinuxParser::Jiffies());
}