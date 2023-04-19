#include "logger.h"

#include <cstdio>

#include "definitions.h"

namespace roy {

Logger::Logger(LogLevel level) : level_(level) {}

void Logger::PrintLog() const {
  const std::string& text = ss_.str();
  switch (level_) {
    case LogLevel::kVerbose:
      printf("VERBOSE  << %s\n", text.c_str());
      break;
    case LogLevel::kDebug:
      printf("DEBUG    << %s\n", text.c_str());
      break;
    case LogLevel::kInfo:
      printf("INFO     << %s\n", text.c_str());
      break;
    case LogLevel::kWarning:
      printf("WARNING  << %s\n", text.c_str());
      break;
    case LogLevel::kError:
      printf("ERROR    << %s\n", text.c_str());
      break;
    case LogLevel::kFatal:
      printf("FATAL    << %s\n", text.c_str());
      break;
  }
}

}