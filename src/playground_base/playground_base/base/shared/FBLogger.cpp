#include <playground_base/base/topo/FBStaticTopoMeta.hpp>
#include <playground_base/base/shared/FBLogger.hpp>
#include <playground_base/base/shared/FBUserDataFolder.hpp>

#include <juce_core/juce_core.h>

#include <ctime>
#include <memory>
#include <filesystem>

using namespace juce;

static std::unique_ptr<FileLogger> _logger = {};

FBEntryExitLogger::
~FBEntryExitLogger()
{
  FBLoggerWrite(file, line, func, "Exit.");
}

FBEntryExitLogger::
FBEntryExitLogger(
char const* file, int line, char const* func):
line(line), file(file), func(func)
{
  FBLoggerWrite(file, line, func, "Enter.");
}

void
FBLoggerTerminate()
{
  FB_LOGGER_WRITE("Terminating logging.");
  _logger.reset();
}

void
FBLoggerInit(FBStaticTopoMeta const& meta)
{
  auto path = FBGetUserPluginDataFolder(meta) / "lastrun.log";
  auto file = File(String(path.string()));
  _logger = std::make_unique<FileLogger>(file, "", 0);
  FB_LOGGER_WRITE("Initialized logging.");
}

void
FBLoggerWrite(
  char const* file, int line,
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
    std::string(buffer) + " :: " + 
    fileName + ":" + std::to_string(line) + " :: " + 
    func + ": " + message);
}