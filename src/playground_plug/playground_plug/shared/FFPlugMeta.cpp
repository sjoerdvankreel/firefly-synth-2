#include <playground_plug/shared/FFPlugMeta.hpp>

FBStaticTopoMeta
FFPlugMeta()
{
  FBStaticTopoMeta result;
  result.name = FFPlugName;
  result.vendor = FFVendorName;
  result.id = FFPlugProcessorId;
  result.version.major = FF_PLUG_VERSION_MAJOR;
  result.version.minor = FF_PLUG_VERSION_MINOR;
  result.version.patch = FF_PLUG_VERSION_PATCH;
  return result;
}