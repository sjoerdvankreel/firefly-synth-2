#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <juce_core/juce_core.h>
#include <boost/stacktrace.hpp>

#include <ctime>
#include <memory>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <filesystem>

using namespace juce;

static std::unique_ptr<FileLogger> _logger = {};

FBEntryExitLog::
~FBEntryExitLog()
{
  FBLogWrite(FBLogLevel::Info, file, line, func, "Exit.");
}

FBEntryExitLog::
FBEntryExitLog(
char const* file, int line, char const* func):
line(line), file(file), func(func)
{
  FBLogWrite(FBLogLevel::Info, file, line, func, "Enter.");
}

void
FBLogTerminate()
{
  FB_LOG_INFO("Terminating log.");
  _logger.reset();
  FB_LOG_INFO("Terminated log.");
}

void
FBLogInit(FBStaticTopoMeta const& meta)
{
  auto path = FBGetUserPluginDataFolder(meta) / "lastrun.log";
  auto file = File(String(path.string()));
  _logger = std::make_unique<FileLogger>(file, meta.NameVersionAndFormat(), 0);
  FB_LOG_INFO("Initialized log.");
}

std::string
FBLogLevelToString(FBLogLevel level)
{
  switch (level)
  {
  case FBLogLevel::Info: return "INFO";
  case FBLogLevel::Warn: return "WARN";
  case FBLogLevel::Error: return "ERROR";
  default: assert(false); return "";
  }
}

std::string
FBStackTraceFromCurrentException()
{
  std::ostringstream oss;
  auto trace = boost::stacktrace::stacktrace::from_current_exception();
  oss << trace;
  return oss.str();
}

void
FBLogWrite(
  FBLogLevel level, char const* file, int line,
  char const* func, std::string const& message)
{
  time_t rawTime;
  struct tm* timeInfo;
  char buffer[80] = { 0 };
  auto fileName = std::filesystem::path(file).filename().string();

  time(&rawTime);
  timeInfo = localtime(&rawTime);
  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeInfo);
  _logger->logMessage(
    std::string(buffer) + ": " + FBLogLevelToString(level) + " :: " +
    fileName + ":" + std::to_string(line) + " (" + func + "): " + message);
}