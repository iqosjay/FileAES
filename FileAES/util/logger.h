#ifndef FILE_AES_UTIL_LOGGER_H_
#define FILE_AES_UTIL_LOGGER_H_

#include <sstream>

#define LOGV roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kVerbose)
#define LOGD roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kDebug)
#define LOGI roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kInfo)
#define LOGW roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kWarning)
#define LOGE roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kError)
#define LOGF roy::Logger::Logcat() = roy::Logger(roy::LogLevel::kFatal)

namespace roy {

enum class LogLevel {
  kVerbose,
  kDebug,
  kInfo,
  kWarning,
  kError,
  kFatal
};

class Logger {
 public:
  explicit Logger(LogLevel level);

  template<typename T>
  Logger& operator<<(const T& t) {
    ss_ << t;
    return *this;
  }

  struct Logcat final {
    Logcat& operator=(const Logger& logger) {
      logger.PrintLog();
      return *this;
    }
  };

 private:
  void PrintLog() const;

 private:
  LogLevel level_;
  std::stringstream ss_;
};

}

#endif//FILE_AES_UTIL_LOGGER_H_