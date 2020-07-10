#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  const long hour = seconds/60/60;
  long rem = (seconds%3600);
  const long min = rem/60;
  rem = rem%60;
  return OutputTime(hour) + ":" + OutputTime(min) + ":" + OutputTime(rem);
}

string Format::OutputTime(long time) {
  string time_str = to_string(time);
  if(time_str.length() < 2)
    time_str = "0" + time_str;
  return time_str;
}