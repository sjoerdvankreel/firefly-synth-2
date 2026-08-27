#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <map>
#include <vector>
#include <utility>

struct FBRuntimeTopo;

class FBGUIStateContainer final
{
  friend class FBPlugGUIContext;

  std::string _themeName;
  std::string _patchName = "Init Patch";
  std::string _instanceName = "New Session";
  std::map<int, std::string> _paramNames;

  std::vector<double*> _params;
  void* _rawState;
  void (*_freeRawState)(void*);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIStateContainer);
  FBGUIStateContainer(FBRuntimeTopo const& topo);
  ~FBGUIStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBGUIStateContainer const& gui);
  std::vector<double*> const& Params() const { return _params; }
  std::string const& ThemeName() const { return _themeName; }
  std::string const& PatchName() const { return _patchName; }
  std::string const& InstanceName() const { return _instanceName; }
  std::map<int, std::string> const& ParamNames() const { return _paramNames; }
  void SetThemeName(std::string const& name) { _themeName = name; }
  void SetPatchName(std::string const& name) { _patchName = name; }
  void SetInstanceName(std::string const& name) { _instanceName = name; }
  void ClearParamName(int paramId) { _paramNames.erase(paramId); }
  void SetParamName(int paramId, std::string const& name) { _paramNames[paramId] = name; }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};