#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

#include <vector>
#include <memory>
#include <functional>

struct FBStaticTopo;
struct FBStaticParam;
struct FBStaticGUIParam;
struct FBHostExchangeState;

class FBPlugGUI;
class FBHostGUIContext;
class FBGlobalBlockParamState;

struct FBPlugVersion final
{
  int major = 0;
  int minor = 0;
  int patch = 0;
  std::string ToString() const;
};

struct FBStaticTopoMeta final
{
  std::string id;
  std::string name;
  std::string vendor;
  FBPlugVersion version;
  std::string NameAndVersion() const;
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

typedef std::function<FBSpecialParams(
FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;
typedef std::function<FBSpecialGUIParams(
FBStaticTopo const& topo, void* state)>
FBSpecialGUIParamsSelector;
typedef std::function<FBHostExchangeState* (
void* state)>
FBHostExchangeAddrSelector;
typedef std::function<std::array<FBVoiceInfo, FBMaxVoices>* (
void* state)>
FBVoicesExchangeAddrSelector;

typedef std::function<std::unique_ptr<FBPlugGUI>(
FBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticTopo final
{
  int guiWidth = {};
  int guiAspectRatioWidth = {};
  int guiAspectRatioHeight = {};
  FBPlugGUIFactory guiFactory = {};

  FBStaticTopoMeta meta = {};

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