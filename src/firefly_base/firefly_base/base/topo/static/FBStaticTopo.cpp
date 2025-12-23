#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <sstream>

FBDeserializationConverter::
FBDeserializationConverter(
FBPlugVersion const& oldVersion,
FBRuntimeTopo const* topo) :
_oldVersion(oldVersion),
_topo(topo) {}

std::string
FBStaticTopoMeta::NameVersionAndFormat() const
{
  return name + " " + version.ToString() + " " + FBPlugFormatToString(format);
}

std::string 
FBPlugFormatToString(FBPlugFormat format)
{
  switch (format)
  {
  case FBPlugFormat::VST3: return "VST3";
  case FBPlugFormat::CLAP: return "CLAP";
  default: FB_ASSERT(false); return {};
  }
}

std::string
FBPlugVersion::ToString() const
{
  auto result = std::to_string(major);
  result += "." + std::to_string(minor);
  result += "." + std::to_string(patch);
  return result;
}  

FBPlugVersion::
FBPlugVersion(int major, int minor, int patch) :
major(major), minor(minor), patch(patch) {}

bool 
FBPlugVersion::operator<(FBPlugVersion const& rhs) const
{
  if (major < rhs.major) return true;
  if (major > rhs.major) return false;
  if (minor < rhs.minor) return true;
  if (minor > rhs.minor) return false;
  return patch < rhs.patch;
}

std::string
FBStaticTopo::PrintTopology() const
{
  std::ostringstream result = {};
  result << meta.name;
  result << "\nId: " << FBCleanTopoId(meta.id);
  result << "\nModules:";

  for (int m = 0; m < modules.size(); m++)
  {
    auto const& module = modules[m];
    result << "\n\t" << module.name;
    result << "\n\t\tId: " << FBCleanTopoId(module.id);
    result << "\n\t\tPer-Voice: " << (module.voice? "True": "False");
    result << "\n\t\tSlot Count: " << module.slotCount;

    auto const& cvOutputs = modules[m].cvOutputs;
    if (cvOutputs.size() == 0)
      result << "\n\t\tCV Outputs: None";
    else
    {
      result << "\n\t\tCV Outputs:";
      for (int j = 0; j < cvOutputs.size(); j++)
      {
        auto const& cvOutput = cvOutputs[j];
        result << "\n\t\t\t" << cvOutput.name;
        result << "\n\t\t\t\tId: " << FBCleanTopoId(cvOutput.id);
        result << "\n\t\t\t\tSlot Count: " << cvOutput.slotCount;
      }
    }

    auto const& guiParams = modules[m].guiParams;
    if (guiParams.size() == 0)
      result << "\n\t\tGUI Params: None";
    else
    {
      result << "\n\t\tGUI Params:";
      for (int j = 0; j < guiParams.size(); j++)
      {
        auto const& guiParam = guiParams[j];
        result << "\n\t\t\t" << guiParam.name;
        result << "\n\t\t\t\tId: " << FBCleanTopoId(guiParam.id);
        result << "\n\t\t\t\tSlot Count: " << guiParam.slotCount;
      }
    }

    auto const& params = modules[m].params;
    if (params.size() == 0)
      result << "\n\t\tAudio Params: None";
    else
    {
      result << "\n\t\tAudio Params:";
      for (int j = 0; j < params.size(); j++)
      {
        auto const& param = params[j];
        result << "\n\t\t\t" << param.name;
        result << "\n\t\t\t\tId: " << FBCleanTopoId(param.id);
        result << "\n\t\t\t\tSlot Count: " << param.slotCount;
      }
    }
  }

  return result.str();
}