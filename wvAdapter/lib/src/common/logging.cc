#include "logging.h"

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace logging {
  std::mutex lock_;

  static const std::unordered_map<const LogLevel, const std::string> logLevelToNameMapping = {
      {LogLevel::NONE,    "NONE "},
      {LogLevel::TRACE,   "TRACE"},
      {LogLevel::DEBUG,   "DEBUG"},
      {LogLevel::INFO,    "INFO "},
      {LogLevel::WARNING, "WARN "},
      {LogLevel::ERROR,   "ERROR"},
      {LogLevel::FATAL,   "FATAL"}
  };
  static std::unordered_map<std::thread::id, std::string> threadIdToNameMapping;
  static LogLevel configuredLogLevel = LogLevel::INFO;

  class NullBuffer : public std::streambuf  {
    public:
      int overflow(int c) { return c; }
  };
  static NullBuffer nullBuffer;
  std::ostream nullStream_(&nullBuffer);

  // example log msg
  // [INFO ]2018-11-03 19:25:25.123(thread1 - file.cc#func#12) This is a log message!
  std::ostream& printLogMessage(const LogLevel &level, const char *file, int line,
                                const char *func, const std::thread::id tid) {
    if (static_cast<int>(level) >= static_cast<int>(configuredLogLevel)) {
      char timestamp[100];
      auto now = std::chrono::system_clock::now();
      std::time_t now_tm_t = std::chrono::system_clock::to_time_t(now);
      std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now_tm_t));
      long millis = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count() % 1000;

      auto iter = threadIdToNameMapping.find(tid);
      std::string tName;
      if (iter != threadIdToNameMapping.end()) {
        tName = iter->second;
      } else {
        std::stringstream sstream;
        sstream << tid;
        tName = sstream.str();
      }

      std::cout << "[" << logLevelToNameMapping.at(level) << "]"
                << timestamp << "." << std::setfill('0') << std::setw(3) << millis
                << "(" << tName << " - " << file << "#" << func << "#" << line << ") ";
      return std::cout;
    } else {
      return nullStream_;
    }
  }

  void addThreadIdNameMapping(const std::thread::id tid, const char *name) {
    threadIdToNameMapping[tid] = name;
  }

  void setLogLevel(const LogLevel &level) {
    configuredLogLevel = level;
  }
}



