#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <string>
#include <cassert>

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

std::string FBStackTraceFromCurrentException();
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

// Writes exception to log, if any.
// Meant to be used for top level functions (so called from the host).
// It logs the exception then rethrows, not wise to continue on any unknown error.
// Do NOT log unconditionally here since this function may be called from RT.
template <class F, class... Args>
auto FBWithLogException(F f, Args... args) -> decltype(f(args...))
{
  try { 
    return f(args...); 
  } catch (std::exception const& e) {
    FB_LOG_ERROR(std::string("Caught exception: ") + e.what() + "\n" + FBStackTraceFromCurrentException());
    throw;
  } catch (...) { 
    FB_LOG_ERROR(std::string("Caught unknown exception:\n") +  FBStackTraceFromCurrentException());
    throw;
  }
  assert(false);
  return f(args...);
}