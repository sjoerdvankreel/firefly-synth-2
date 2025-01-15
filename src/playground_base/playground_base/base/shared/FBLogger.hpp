#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <string>

#define FB_LOGGER_WRITE(msg) \
FBLoggerWrite(__FILE__, __LINE__, __func__, msg)
#define FB_LOG_ENTRY_EXIT() \
FBEntryExitLogger entryExitLogger(__FILE__, __LINE__, __func__)

struct FBStaticTopoMeta;

struct FBEntryExitLogger
{
  int line = {};
  char const* file = {};
  char const* func = {};

  ~FBEntryExitLogger();
  FBEntryExitLogger(char const* file, int line, char const* func);
  FB_NOCOPY_MOVE_NODEFCTOR(FBEntryExitLogger);
};

void FBLoggerTerminate();
void FBLoggerInit(FBStaticTopoMeta const& meta);
void FBLoggerWrite(char const* file, int line, char const* func, std::string const& message);