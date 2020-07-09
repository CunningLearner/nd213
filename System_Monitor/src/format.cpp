#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hour = seconds/60/60;
  long rem = (seconds%3600);// * seconds / hour;
  long min = rem/60;
  rem = rem%60;//(rem/60 - (float)min) * hour / min;
//   int sec = rem/100;  
  return OutputTime(hour) + ":" + OutputTime(min) + ":" + OutputTime(rem);
}

string Format::OutputTime(long time) {
  string time_str = to_string(time);
  if(time_str.length() < 2)
    time_str = "0" + time_str;
  return time_str;
}