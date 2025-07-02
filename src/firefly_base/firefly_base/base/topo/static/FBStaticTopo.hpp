#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/static/FBStaticModule.hpp>

#include <vector>
#include <memory>
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
  std::string id;
  std::string name;
  std::string vendor;
  FBPlugVersion version;
  FBPlugFormat format = (FBPlugFormat)-1;
  std::string NameVersionAndFormat() const;
};

struct FBSpecialParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  FBGlobalBlockParamState* state = nullptr;
  FBStaticParam const& ParamTopo(FBStaticTopo const& topo) const;
};

struct FBSpecialGUIParam final
{
  int paramIndex = -1;
  int moduleIndex = -1;
  double* state = nullptr;
  FBStaticGUIParam const& ParamTopo(FBStaticTopo const& topo) const;
};

struct FBSpecialParams final
{
  FBSpecialParam hostSmoothTime = {};
};

struct FBSpecialGUIParams final
{
  FBSpecialGUIParam userScale = {};
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

  virtual bool OnParamNotFound(
    std::string const& /*oldModuleId*/, int /*oldModuleSlot*/,
    std::string const& /*oldParamId*/, int /*oldParamSlot*/,
    std::string& /*newModuleId*/, int& /*newModuleSlot*/,
    std::string& /*newParamId*/, int& /*newParamSlot*/) const { return false; }
};

typedef std::function<FBSpecialParams(
FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;
typedef std::function<FBSpecialGUIParams(
FBStaticTopo const& topo, void* state)>
FBSpecialGUIParamsSelector;

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

struct FBStaticTopo final
{
  int guiWidth = {};
  int guiAspectRatioWidth = {};
  int guiAspectRatioHeight = {};
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

  int exchangeStateSize = {};
  int exchangeStateAlignment = {};
  FBSpecialParamsSelector specialSelector = {};
  FBSpecialGUIParamsSelector specialGUISelector = {};
  FBHostExchangeAddrSelector hostExchangeAddr = {};
  FBVoicesExchangeAddrSelector voicesExchangeAddr = {};

  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};