#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <juce_core/juce_core.h>

#include <ctime>
#include <memory>
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
char const* file_, int line_, char const* func_):
line(line_), file(file_), func(func_)
{
  FBLogWrite(FBLogLevel::Info, file, line, func, "Enter.");
}

std::filesystem::path 
FBGetLogPath(FBStaticTopoMeta const& meta)
{
  return FBGetUserPluginDataFolder(meta) / "lastrun.log";
}

void
FBLogTerminate()
{
  FB_LOG_INFO("Terminating log.");
  _logger.reset();
  // DON'T log "terminated log" (i actually made that mistake).
}

void
FBLogInit(FBStaticTopoMeta const& meta)
{
  auto path = FBGetLogPath(meta);
  auto file = File(String(path.string()));
  // DON'T log "initializing log" (i actually made that mistake).
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
  default: FB_ASSERT(false); return "";
  }
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
