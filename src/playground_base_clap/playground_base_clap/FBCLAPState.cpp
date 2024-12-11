#include <playground_base_clap/FBCLAPPlugin.hpp>

bool 
FBCLAPPlugin::implementsState() const noexcept 
{
  return false;
}

bool 
FBCLAPPlugin::stateSave(const clap_ostream* stream) noexcept 
{
  return false;
}

bool 
FBCLAPPlugin::stateLoad(const clap_istream* stream) noexcept 
{
  return false;
}