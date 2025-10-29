#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <chrono>
#include <thread>
#include <string>
#include <filesystem>

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
  FBEntryExitLog(char const* file_, int line_, char const* func_);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBEntryExitLog);
};

void FBLogTerminate();
void FBLogInit(FBStaticTopoMeta const& meta);
void FBLogWrite(FBLogLevel level, char const* file, int line, char const* func, std::string const& message);
std::filesystem::path FBGetLogPath(FBStaticTopoMeta const& meta);

// Writes exception to log, if any.
// Meant to be used for top level functions (so called from the host).
// It logs the exception then rethrows, not wise to continue on any unknown error.
// Do NOT log unconditionally here since this function may be called from RT.
template <class F, class... Args>
auto FBWithLogException(F f, Args... args) -> decltype(f(args...))
{
  using namespace std::chrono_literals;

  std::exception_ptr eptr = {};
  try { 
    return f(args...); 
  } catch (...) { 
    eptr = std::current_exception(); 
  }

  FB_ASSERT(eptr);
  if (!eptr) 
    return f(args...);

  try { 
    std::rethrow_exception(eptr);
  } catch (std::exception const& e) { 
    FB_LOG_ERROR(std::string("Caught exception: ") + e.what()); 
    std::this_thread::sleep_for(1000ms); // really need for flush
    throw; 
  } catch (...) { 
    FB_LOG_ERROR("Caught unknown exception."); 
    std::this_thread::sleep_for(1000ms); // really need for flush
    throw;
  }

  FB_ASSERT(false);
  return f(args...);
}