#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iterator>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float total = 0, free = 0, buffer = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)){
      if(total && free && buffer) break;
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "MemTotal:") total = stof(value);
        if(key == "MemFree:") free = stof(value);
        if(key == "Buffers:") buffer = stof(value);
      }
    }
    if(total) return float(1 - (free / total));
//     return float(1 - (free / (total - buffer))); //Non buffer memory
//     return float(1 - (free / (total - buffer - cache))); //Non cache/buffer memory
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
//   long value = 0;
  string line;
  string s_value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> s_value;
  }
  return std::stol(s_value);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return (ActiveJiffies() + IdleJiffies()); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> proc_util = CpuUtilization(pid);
  return (std::stol(proc_util[13]) + std::stol(proc_util[14]) + std::stol(proc_util[15]) + std::stol(proc_util[16]));
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long Active = 0;
  vector<string> Utilization = CpuUtilization();
  vector<CPUStates> ActiveStates {kUser_ , kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_, kGuest_, kGuestNice_};
  for(auto state : ActiveStates) {
    Active += std::stol(Utilization[state]);
  }
  return Active;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long Idle = 0;
  vector<string> Utilization = CpuUtilization();
  vector<CPUStates> IdleStates {kIdle_, kIOwait_};
  for(auto state : IdleStates) {
    Idle += std::stol(Utilization[state]);
  }
  return Idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, name = "cpu", cpu;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> cpu;
      if(cpu == name){
        std::istream_iterator<string> beg(linestream), end;
        vector<string> values(beg, end);
        return values;
      }
    }
  }
  	return vector<string>(); 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string totalProcKey, totalProc, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> totalProcKey >> totalProc;
      if(totalProcKey == "processes")
        return std::stoi(totalProc);
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string runProcKey, runProc, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> runProcKey >> runProc;
      if(runProcKey == "procs_running")
        return std::stoi(runProc);
    }
  }
  return 0;
}

std::vector<std::string> LinuxParser::CpuUtilization(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      std::istream_iterator<string> beg(linestream), end;
      vector<string> values(beg, end);
      return values;
  }
  return vector<string>(); 
}

float LinuxParser::CpuUtilizationProcess(int pid){
  int total_time;
  float cpu_usage = 0, seconds;
  int Hertz = (int)sysconf(_SC_CLK_TCK);

  std::ifstream stream_(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream_.is_open()){
    total_time = ActiveJiffies(pid); //utime + stime + cutime + cstime;
    seconds = UpTime() - UpTime(pid);
    cpu_usage = ((total_time / Hertz) / seconds);
  }
  return cpu_usage;
}
  
/*//   int total_time = ActiveJiffies(pid);
//   float seconds = (UpTime() - UpTime(pid));
  vector<string> values = CpuUtilization(pid);
  int total_time = std::stoi(values[13]) + std::stoi(values[14]) + std::stoi(values[15]) + std::stoi(values[16]);
  float cpu_usage = 0;
  float seconds = (int)UpTime() - (std::stoi(values[21]) / (int)sysconf(_SC_CLK_TCK));
  
  if(total_time && seconds)
    cpu_usage = ((total_time / (int)sysconf(_SC_CLK_TCK)) / seconds);
  return cpu_usage;
}*/

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()) getline(stream, line);
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, mem_token, mem_size;
  int res_ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> mem_token >> mem_size;
      if(mem_token == "VmSize:") res_ram = std::stoi(mem_size) / 1024;
    }
  }
  return to_string(res_ram);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, uid_token, uid_val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> uid_token >> uid_val;
      if(uid_token == "Uid:") return uid_val;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid), line, result;
  string name =  "x:" + uid;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      if(line.find(name) != std::string::npos){
        result = line.substr(0, line.find(":"));
        return result;
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> beg(linestream), end;
    vector<string> values(beg, end);
    return ((long) std::stoi(values[21]) / sysconf(_SC_CLK_TCK));
  }
  return 0;
}