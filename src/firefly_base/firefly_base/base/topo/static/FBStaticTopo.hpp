#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <vector>
#include <functional>

struct FBStaticTopo;
struct FBStaticParam;
struct FBStaticGUIParam;
struct FBRuntimeTopo;
struct FBHostExchangeState;

class FBPlugGUI;
class FBHostGUIContext;
class FBGlobalBlockParamState;

enum class FBPlugFormat { VST3, CLAP };
std::string 
FBPlugFormatToString(FBPlugFormat format);

struct FBPlugVersion final
{
  int major = 0;
  int minor = 0;
  int patch = 0;
  
  FBPlugVersion() = default;
  FBPlugVersion(int major, int minor, int patch);

  std::string ToString() const;
  bool operator<(FBPlugVersion const& rhs) const;
};

struct FBStaticTopoMeta final
{
  bool isFx;
  std::string id;
  std::string name;
  std::string vendor;
  std::string shortName;
  FBPlugVersion version;
  FBPlugFormat format = (FBPlugFormat)-1;

  // To make patch loading across instrument/fx possible.
  std::vector<std::string> allowLoadFromIds = {};
  std::string NameVersionAndFormat() const;
};

class FBDeserializationConverter
{
protected:
  FBPlugVersion const _oldVersion;
  FBRuntimeTopo const* const _topo;
  FBRuntimeTopo const* Topo() const { return _topo; }
  FBPlugVersion const& OldVersion() const { return _oldVersion; }

public:
  virtual ~FBDeserializationConverter() {}
  FBDeserializationConverter(
    FBPlugVersion const& oldVersion,
    FBRuntimeTopo const* topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBDeserializationConverter);

  // Generic catch-all.
  virtual void PostProcess(
    bool /* isGuiState */,
    std::vector<double*> const& /*paramValues*/) const { }

  virtual bool OnParamNotFound(
    bool /* isGuiState */,
    std::string const& /*oldModuleId*/, int /*oldModuleSlot*/,
    std::string const& /*oldParamId*/, int /*oldParamSlot*/,
    std::string& /*newModuleId*/, int& /*newModuleSlot*/,
    std::string& /*newParamId*/, int& /*newParamSlot*/) const { return false; }

  virtual bool OnParamListItemNotFound(
    bool /* isGuiState */,
    std::string const& /*moduleId*/, int /*moduleSlot*/,
    std::string const& /*paramId*/, int /*paramSlot*/,
    std::string const& /*oldParamValue*/, std::string& /* newParamValue*/) const { return false;  }
};

typedef std::function<FBHostExchangeState* (void* state)>
FBHostExchangeAddrSelector;
typedef std::function<std::array<FBVoiceInfo, FBMaxVoices>* (void* state)>
FBVoicesExchangeAddrSelector;

typedef std::function<std::unique_ptr<FBPlugGUI>(
  FBHostGUIContext* context)>
FBPlugGUIFactory;
typedef std::function<std::unique_ptr<FBDeserializationConverter>(
  FBPlugVersion const& oldVersion, FBRuntimeTopo const* topo)>
FBDeserializationConverterFactory;

struct FBStaticTopo
{
  int guiWidth = {};
  int guiAspectRatioWidth = {};
  int guiAspectRatioHeight = {};
  int guiUserScaleParam = -1;
  int guiUserScaleModule = -1;
  FBPlugGUIFactory guiFactory = {};
  FBDeserializationConverterFactory deserializationConverterFactory = {};

  int maxUndoSize = {};
  FBStaticTopoMeta meta = {};
  std::string patchExtension = {};

  void* (*allocRawGUIState)() = {};
  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void* (*allocRawExchangeState)() = {};
  void (*freeRawGUIState)(void* state) = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  void (*freeRawExchangeState)(void* state) = {};

  std::vector<FBStaticModule> modules = {};
  std::vector<FBTopoIndices> moduleProcessOrder = {};

  int exchangeStateSize = {};
  FBHostExchangeAddrSelector hostExchangeAddr = {};
  FBVoicesExchangeAddrSelector voicesExchangeAddr = {};

  virtual ~FBStaticTopo() {}
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};