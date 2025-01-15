#include <playground_base/base/topo/FBStaticTopoMeta.hpp>
#include <playground_base/base/shared/FBLogger.hpp>
#include <playground_base/base/shared/FBUserDataFolder.hpp>

#include <juce_core/juce_core.h>

#include <memory>
#include <filesystem>

using namespace juce;

static std::unique_ptr<FileLogger> _logger = {};

void
FBLoggerTerminate()
{
  _logger.reset();
}

void
FBLoggerInit(FBStaticTopoMeta const& meta)
{
  auto path = FBGetUserPluginDataFolder(meta) / "lastrun.log";
  auto file = File(String(path.string()));
  _logger = std::make_unique<FileLogger>(file, "", 0);
}

void
FBLoggerWrite(
  std::string const& file, std::string const& line,
  std::string const& func, std::string const& message)
{
}