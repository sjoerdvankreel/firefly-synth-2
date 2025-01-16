#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <string>

#define FB_LOG_WRITE(lvl, msg) \
FBLogWrite(lvl, __FILE__, __LINE__, __func__, msg)
#define FB_LOG_ENTRY_EXIT() \
FBEntryExitLog entryExitLog(__FILE__, __LINE__, __func__)

#define FB_LOG_INFO(msg) \
FBLogWrite(FBLogLevel::Info, __FILE__, __LINE__, __func__, msg)
#define FB_LOG_WARN(msg) \
FBLogWrite(FBLogLevel::Warn, __FILE__, __LINE__, __func__, msg)
#define FB_LOG_ERROR(msg) \
FBLogWrite(FBLogLevel::Error, __FILE__, __LINE__, __func__, msg)

struct FBStaticTopoMeta;
enum class FBLogLevel { Info, Warn, Error };
std::string FBLogLevelToString(FBLogLevel level);

struct FBEntryExitLog
{
  int line = {};
  char const* file = {};
  char const* func = {};

  ~FBEntryExitLog();
  FBEntryExitLog(char const* file, int line, char const* func);
  FB_NOCOPY_MOVE_NODEFCTOR(FBEntryExitLog);
};

void FBLogTerminate();
void FBLogInit(FBStaticTopoMeta const& meta);
void FBLogWrite(FBLogLevel level, char const* file, int line, char const* func, std::string const& message);