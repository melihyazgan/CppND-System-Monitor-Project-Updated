#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem.Multiple Lines
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
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem. One line File!!!
string LinuxParser::Kernel() {
  string os,version ,kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {

  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  string line;
  string key;
  float mem,memTotal,memFree;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> mem) {
        if (key == "MemTotal") {memTotal = mem;}
        if (key == "MemFree") {memFree = mem;}
      }
    }
  }
  filestream.close();
  return (memTotal-memFree)/memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string sysupt,idleupt;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> sysupt >> idleupt;
  }
  stream.close();
  return stol(sysupt);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total{0};
  auto cpuuti = LinuxParser::CpuUtilization();
  for(int i= kUser_; i <=kSteal_;i++ ){
    total += stol(cpuuti[i]);
  } 
  return total;
}

// Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) { 
  long utime,stime, cutime,cstime;
  string line,value;
  vector<string> pid_stat{};
  std::ifstream stream(kProcDirectory +to_string(pid)+ kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    while(linestream>>value){
      pid_stat .push_back(value);
  }
  utime = stol(pid_stat[13]);
  stime = stol(pid_stat[14]);
  cutime = stol(pid_stat[15]);
  cstime = stol(pid_stat[16]);
  //starttime = stol(pid_stat[21]);
  }

  float total_time = utime + stime + cutime + cstime;
 
  stream.close();
  return total_time;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto cpuuti = LinuxParser::CpuUtilization();
  return stol(cpuuti[kIdle_]) + stol(cpuuti[kIOwait_]);
 }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::string line,cpu, user,nice,system, idle, iowait,irq,softirq,steal,guest,guest_nice;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream>>cpu>> user >> nice >> system >> idle >> iowait >>irq >> softirq >> steal >> guest >> guest_nice;
  }
  return vector<string> {user,nice,system, idle, iowait,irq,softirq,steal,guest,guest_nice};
}

//  Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key;
  int value, process;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream>>key>>value;
      if (key=="processes"){process=value;break;}
    }
  }
  stream.close();
  return process;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line, key, value; 
  int run;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream>>key>>value;
      if (key == "procs_running"){
        run = stoi(value);
        break;
      }
    }
  }
  stream.close();
  return run;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd;
  std::ifstream stream(kProcDirectory +to_string(pid)+ kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream,cmd);
    }
  stream.close();
  return cmd;
  }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string line, key,value,ram;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize"){
          ram =value;
          break; 
        }
      }
    }
  }
  stream.close();
  return to_string(stol(ram)/1024);
 }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line, key,value,uid;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key>> value){
        if (key == "Uid:"){
          uid =value;
          break;;
        }
      } 
    }
  }    
  stream.close();
  return uid;
}

//Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string line, username,x,id;
  std::string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> x >> id;
      if (id == uid){
        break;
      }
    }
  }
  stream.close();
  return username;
}

//Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line,key;
  long puptime;
  std::vector<std::string> stats;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {   
  std::getline(stream, line);
  std::istringstream linestream(line);
  while(linestream >> key){
        stats.push_back(key);
    }
  }      
  stream.close();
  puptime = UpTime()-(stol(stats[21])/sysconf(_SC_CLK_TCK));
  return puptime;
}